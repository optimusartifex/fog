// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/ThreadCondition.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Time.h>

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <errno.h>
# include <sys/time.h>
#endif // FOG_OS_POSIX

/*
FAQ On subtle implementation details:

1) What makes this problem subtle? Please take a look at "Strategies for
implementing POSIX Condition Variables on Win32" by Douglas C. Schmidt and
Irfan Pyarali:

  http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

It includes discussions of numerous flawed strategies for implementing this
functionality. I'm not convinced that even the final proposed implementation
has semantics that are as nice as this implementation (especially with regard
to broadcast() and the impact on threads that try to wait() after a broadcast()
has been called, but before all the original waiting threads have been signaled).

2) Why can't you use a single wait_event for all threads that call wait()? See
FAQ-question-1, or consider the following: If a single event were used, then
numerous threads calling wait() could release their cs locks, and be preempted
just before calling WaitForSingleObject(). If a call to broadcast() was then
presented on a second thread, it would be impossible to actually signal all
waiting(?) threads. Some number of setEvent() calls *could* be made, but there
could be no guarantee that those led to to more than one signaled thread
(SetEvent()'s may be discarded after the first!), and there could be no
guarantee that the SetEvent() calls didn't just awaken "other" threads that
hadn't even started waiting yet (oops). Without any limit on the number of
requisite SetEvent() calls, the system would be forced to do many such calls,
allowing many new waits to receive spurious signals.

3) How does this implementation cause spurious signal events?  The cause in this
implementation involves a race between a signal via time-out and a signal via
signal() or broadcast().  The series of actions leading to this are:

  a) Timer fires, and a waiting thread exits the line of code:

    WaitForSingleObject(waitingEvent, max_time.InMilliseconds());

  b) That thread (in (a)) is randomly pre-empted after the above line, leaving
     the waitingEvent reset (unsignaled) and still in the _waitingList.

  c) A call to signal() (or broadcast()) on a second thread proceeds, and
     selects the waiting cv_event (identified in step (b)) as the event to
     revive via a call to SetEvent().

  d) The signal() method (step c) calls SetEvent() on waitingEvent (step b).

  e) The waiting cv_event (step b) is now signaled, but no thread is waiting on
     it.

  f) When that waitingEvent (step b) is reused, it will immediately be signaled
     (spuriously).

4) Why do you recycle events, and cause spurious signals?  First off, the
spurious events are very rare. They can only (I think) appear when the race
described in FAQ-question-3 takes place. This should be very rare.  Most(?)
uses will involve only timer expiration, or only signal/broadcast() actions.
When both are used, it will be rare that the race will appear, and it would
require MANY wait() and signaling activities.  If this implementation did not
recycle events, then it would have to create and destroy events for every call
to wait(). That allocation/deallocation and associated construction/destruction
would be costly (per wait), and would only be a rare benefit (when the race was
"lost" and a spurious signal took place). That would be bad (IMO) optimization
trade-off. Finally, such spurious events are allowed by the specification of
condition variables (such as implemented in Vista), and hence it is better if
any user accommodates such spurious events (see usage note in ThreadCondition.h).

5) Why don't you reset events when you are about to recycle them, or about to
reuse them, so that the spurious signals don't take place? The thread described
in FAQ-question-3 step c may be pre-empted for an arbitrary length of time
before proceeding to step d. As a result, the wait_event may actually be re-used
*before* step (e) is reached. As a result, calling reset would not help
significantly.

6) How is it that the callers lock is released atomically with the entry into a
wait state? We commit to the wait activity when we allocate the wait_event for
use in a given call to wait(). This allocation takes place before the caller's
lock is released (and actually before our _internalLock is released). That
allocation is the defining moment when "the wait state has been entered," as
that thread *can* now be signaled by a call to broadcast() or signal(). Hence
we actually "commit to wait" before releasing the lock, making the pair
effectively atomic.

8) Why do you need to lock your data structures during waiting, as the caller is
already in possession of a lock?  We need to acquire() and release() our
internal lock during signal() and broadcast(). If we tried to use a callers lock
for this purpose, we might conflict with their external use of the lock. For
example, the caller may use to consistently hold a lock on one thread while
calling signal() on another, and that would block signal().

9) Couldn't a more efficient implementation be provided if you preclude using
more than one external lock in conjunction with a single ThreadCondition
instance? Yes, at least it could be viewed as a simpler API (since you don't
have to reiterate the lock argument in each wait() call). One of the
constructors now takes a specific lock as an argument, and a there are
corresponding wait() calls that don't specify a lock now. It turns that the
resulting implmentation can't be made more efficient, as the internal lock needs
to be used by signal() and broadcast(), to access internal data structures. As a
result, I was not able to utilize the user supplied lock (which is being used by
the user elsewhere presumably) to protect the private member access.

9) Since you have a second lock, how can be be sure that there is no possible
deadlock scenario? Our _internalLock is always the last lock acquired, and the
first one released, and hence a deadlock (due to critical section problems) is
impossible as a consequence of our lock.

10) When doing a broadcast(), why did you copy all the events into an STL queue,
rather than making a linked-loop, and iterating over it? The iterating during
broadcast() is done so outside the protection of the internal lock. As a result,
other threads, such as the thread wherein a related event is waiting, could
asynchronously manipulate the links around a cv_event. As a result, the link
structure cannot be used outside a lock. broadcast() could iterate over waiting
events by cycling in-and-out of the protection of the _internalLock, but that
appears more expensive than copying the list into an STL stack.

11) Why did the lock.h file need to be modified so much for this change? Central
to a Condition Variable is the atomic release of a lock during a wait(). This
places wait() functionality exactly mid-way between the two classes, Lock and
Condition Variable. Given that there can be nested acquire()'s of locks, and
wait() had to release() completely a held lock, it was necessary to augment the
Lock class with a recursion counter. Even more subtle is the fact that the
recursion counter (in a Lock) must be protected, as many threads can access it
asynchronously. As a positive fallout of this, there are now some ASSERTS to be
sure no one release()s a Lock more than they Acquire()ed it, and there is
ifdef'ed functionality that can detect nested locks (legal under Windows, but
not under Posix).

12) Why is it that the WinCvEvent`s removed from list in broadcast() and signal()
are not leaked? How are they recovered? The WinCvEvent`s that appear to leak are
taken from the _waitingList.  For each element in that list, there is currently
a thread in or around the WaitForSingleObject() call of wait(), and those
threads have references to these otherwise leaked events. They are passed as
arguments to be recycled just aftre returning from WaitForSingleObject().
*/

// ============================================================================
// [Configuration]
// ============================================================================

// It's possible to disable the Windows-Vista CONDITION_VARIABLE support,
// mainly for testing purposes (so we are able to test our implementation
// for Windows 2000/XP on Vista and newer OS.

// #define FOG_NO_WIN_VISTA_SUPPORT
// #define FOG_NO_WIN_VISTA_CONDITION_VARIABLE

namespace Fog {

// ============================================================================
// [Fog::ThreadCondition (Windows 2000/XP)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

//! @internal
//!
//! @brief Define WinCvEvent class that is used to form circularly linked lists.
//!
//! The list container is an element with NULL as its handle_ value.
//! The actual list elements have a non-zero handle_ value.
//! All calls to methods MUST be done under protection of a lock so that links
//! can be validated.  Without the lock, some links might asynchronously
//! change, and the assertions would fail (as would list change operations).
struct FOG_NO_EXPORT WinCvEvent
{
  // --------------------------------------------------------------------------
  // [Methods called on cvar->_waitingList and cvar->_recyclingList]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isEmpty() const
  {
    return this == next;
  }

  FOG_INLINE void pushBack(WinCvEvent* other)
  {
    // Prepare other for insertion.
    other->prev = prev;
    other->next = this;

    // Cut into list.
    prev->next = other;
    prev = other;
  }

  FOG_INLINE WinCvEvent* popFront()
  {
    FOG_ASSERT(!isEmpty());
    return next->extract();
  }

  FOG_INLINE WinCvEvent* popBack()
  {
    FOG_ASSERT(!isEmpty());
    return prev->extract();
  }

  // --------------------------------------------------------------------------
  // [Methods called on elements (items which contain HANDLE)]
  // --------------------------------------------------------------------------

  //! @brief Accessor method.
  FOG_INLINE HANDLE getHandle() const
  {
    FOG_ASSERT(handle != NULL);
    return handle;
  }

  //! @brief Pull an element from a list (if it's in one).
  FOG_INLINE WinCvEvent* extract()
  {
    if (!isEmpty())
    {
      // Stitch neighbors together.
      next->prev = prev;
      prev->next = next;

      // Set to empty.
      prev = next = this;
    }

    FOG_ASSERT(isEmpty());
    return this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  WinCvEvent* next;
  WinCvEvent* prev;
  HANDLE handle;
};

struct FOG_NO_EXPORT WinCvImpl
{
  //! @brief Run state enumeration.
  //!
  //! Note that RUNNING is an unlikely number to have in RAM by accident.
  //! This helps with defensive destructor coding in the face of user error.
  enum RunState { SHUTDOWN = 0, RUNNING = 64213 };

  //! @brief Private critical section for access to member data.
  Static<Lock> _internalLock;

  //! @brief Events that threads are blocked on.
  WinCvEvent _waitingList;
  //! @brief Free list for old events.
  WinCvEvent _recyclingList;

  //! @brief Count of old events in recyclingList.
  uint _recyclingListSize;
  //! @brief The number of allocated, but not yet deleted events.
  uint _allocationCounter;
  //! @brief Run state.
  uint _runState;
};

// GetEventForWaiting() provides a unique cv_event for any caller that needs to
// wait.  This means that (worst case) we may over time create as many cv_event
// objects as there are threads simultaneously using this instance's wait()
// functionality.
static WinCvEvent* ThreadCondition_getEventForWaiting(ThreadCondition* self)
{
  WinCvImpl* cvar = reinterpret_cast<WinCvImpl*>(MemMgr::alloc(sizeof(WinCvImpl)));
  FOG_ASSERT(cvar != NULL);

  // We hold cvar->_internalLock, courtesy of wait().
  WinCvEvent* e;

  if (cvar->_recyclingListSize == 0)
  {
    FOG_ASSERT(cvar->_recyclingList.isEmpty());

    e = reinterpret_cast<WinCvEvent*>(MemMgr::alloc(sizeof(WinCvEvent)));
    if (FOG_IS_NULL(e))
      return NULL;

    e->next = e;
    e->prev = e;
    e->handle = ::CreateEventW(NULL, false, false, NULL);

    if (e->handle == NULL)
    {
      MemMgr::free(e);
      return NULL;
    }

    cvar->_allocationCounter++;
  }
  else
  {
    e = cvar->_recyclingList.popFront();
    cvar->_recyclingListSize--;
  }

  cvar->_waitingList.pushBack(e);
  return e;
}

// ThreadCondition::recycleEvent() takes a WinCvEvent that was previously used
// for wait()ing, and recycles it for use in future wait() calls for this or
// other threads. Note that there is a tiny chance that the WinCvEvent is still
// signaled when we obtain it, and that can cause spurious signals (if/when we
// re-use the WinCvEvent), but such is quite rare (see FAQ-question-5).
static void FOG_CDECL ThreadCondition_recycleEvent(ThreadCondition* self, WinCvEvent* e)
{
  WinCvImpl* cvar = reinterpret_cast<WinCvImpl*>(self->_cvar);
  FOG_ASSERT(cvar != NULL);

  // We hold cvar->_internalLock, courtesy of wait(). If the WinCvEvent timed
  // out, then it is necessary to remove it from cvar->_waitingList. If it was
  // selected by broadcast() or signal(), then it is already gone.

  // Possibly redundant
  e->extract();

  cvar->_recyclingList.pushBack(e);
  cvar->_recyclingListSize++;
}

static err_t FOG_CDECL ThreadCondition_ctor(ThreadCondition* self, Lock* lock)
{
  WinCvImpl* cvar = reinterpret_cast<WinCvImpl*>(MemMgr::alloc(sizeof(WinCvImpl)));
  if (FOG_IS_NULL(cvar))
  {
    self->_cvar = NULL;
    self->_lock = NULL;
    return ERR_RT_OUT_OF_MEMORY;
  }

  cvar->_internalLock.init();
  cvar->_runState = WinCvImpl::RUNNING;
  cvar->_allocationCounter = 0;
  cvar->_recyclingListSize = 0;

  cvar->_waitingList.next = &cvar->_waitingList;
  cvar->_waitingList.prev = &cvar->_waitingList;
  cvar->_waitingList.handle = NULL;

  cvar->_recyclingList.next = &cvar->_recyclingList;
  cvar->_recyclingList.prev = &cvar->_recyclingList;
  cvar->_recyclingList.handle = NULL;

  self->_cvar = cvar;
  self->_lock = lock;

  return ERR_OK;
}

static void FOG_CDECL ThreadCondition_dtor(ThreadCondition* self)
{
  WinCvImpl* cvar = reinterpret_cast<WinCvImpl*>(self->_cvar);
  if (FOG_IS_NULL(cvar))
    return;

  AutoLock locked(cvar->_internalLock);

  // Prevent any more waiting.
  cvar->_runState = WinCvImpl::SHUTDOWN;
  FOG_ASSERT(cvar->_recyclingListSize == cvar->_allocationCounter);

  // Rare shutdown problem.
  if (cvar->_recyclingListSize != cvar->_allocationCounter)
  {
    // There are threads of execution still in this->wait(time) and yet the
    // caller has instigated the destruction of this instance :-/.
    // A common reason for such "overly hasty" destruction is that the caller
    // was not willing to wait for all the threads to terminate.  Such hasty
    // actions are a violation of our usage contract, but we'll give the
    // waiting thread(s) one last chance to exit gracefully (prior to our
    // destruction).
    // Note: _waitingList *might* be empty, but recycling is still pending.
    AutoUnlock autoUnlock(cvar->_internalLock);

    // Make sure all waiting threads have been signaled.
    self->broadcast();

    // Give threads a chance to grab _internalLock.
    ::Sleep(10);
    // All contained threads should be blocked on self->_lock by now.
  } // Reacquire _internalLock.

  // Free WinCvEvent instances, including HANDLEs.
  {
    FOG_ASSERT(cvar->_recyclingListSize == cvar->_allocationCounter);
    WinCvEvent* e = cvar->_recyclingList.next;

    while (e != &cvar->_recyclingList)
    {
      WinCvEvent* next = e->next;

      int retVal = ::CloseHandle(e->handle);
      FOG_ASSERT(retVal);
      MemMgr::free(e);

      e = next;
    }
  }

  cvar->_internalLock.destroy();
  MemMgr::free(cvar);
}

// ThreadCondition::signal() will select one of the waiting threads, and signal
// it (signal its WinCvEvent). For better performance we signal the thread that
// went to sleep most recently (LIFO). If we want fairness, then we wake the
// thread that has been sleeping the longest (FIFO).
static void FOG_CDECL ThreadCondition_signal(ThreadCondition* self)
{
  WinCvImpl* cvar = reinterpret_cast<WinCvImpl*>(self->_cvar);
  if (FOG_IS_NULL(cvar))
    return;

  HANDLE handle;

  {
    AutoLock locked(cvar->_internalLock);
    if (cvar->_waitingList.isEmpty())
      return;  // No one to signal.

    // Only performance option should be used. This is not a leak from
    // cvar->_waitingList. See FAQ-question 12.
    handle = cvar->_waitingList.popBack()->getHandle();  // LIFO.
  } // Release cvar->_internalLock.

  ::SetEvent(handle);
}

// broadcast() is guaranteed to signal all threads that were waiting (i.e., had
// a cv_event internally allocated for them) before broadcast() was called.
static void FOG_CDECL ThreadCondition_broadcast(ThreadCondition* self)
{
  WinCvImpl* cvar = reinterpret_cast<WinCvImpl*>(self->_cvar);
  if (FOG_IS_NULL(cvar))
    return;

  // See FAQ-question-10.
  List<HANDLE> handleList;

  {
    AutoLock locked(cvar->_internalLock);

    if (cvar->_waitingList.isEmpty())
      return;

    while (!cvar->_waitingList.isEmpty())
    {
      // This is not a leak from _waitingList. See FAQ-question 12.
      handleList.append(cvar->_waitingList.popBack()->getHandle());
    }
  } // Release cvar->_internalLock.

  while (!handleList.isEmpty())
  {
    ::SetEvent(handleList.getLast());
    handleList.removeLast();
  }
}

static void FOG_CDECL ThreadCondition_wait(ThreadCondition* self, const TimeDelta* maxTime)
{
  WinCvImpl* cvar = reinterpret_cast<WinCvImpl*>(self->_cvar);
  if (FOG_IS_NULL(cvar))
    return;

  DWORD ms = INFINITE;

  if (maxTime != NULL)
    ms = static_cast<DWORD>(maxTime->getMilliseconds());

  WinCvEvent* waitingEvent;
  HANDLE handle;

  {
    AutoLock locked(cvar->_internalLock);

    // Destruction in progress.
    if (cvar->_runState != WinCvImpl::RUNNING)
      return;

    waitingEvent = ThreadCondition_getEventForWaiting(self);
    handle = waitingEvent->getHandle();
    FOG_ASSERT(handle);
  } // Release _internalLock.

  {
    // Release caller's lock
    AutoUnlock autoUnlock(*self->_lock);
    ::WaitForSingleObject(handle, ms);

    // Minimize spurious signal creation window by recycling asap.
    AutoLock locked(cvar->_internalLock);
    ThreadCondition_recycleEvent(self, waitingEvent);

    // Release _internalLock
  } // Reacquire callers lock to depth at entry.
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::ThreadCondition (Windows Vista and newer)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

typedef void (FOG_STDCALL* InitializeConditionVariableFunc)(void* ConditionVariable);
typedef BOOL (FOG_STDCALL* SleepConditionVariableCSFunc)(void* ConditionVariable, PCRITICAL_SECTION CriticalSection, DWORD dwMilliseconds);
typedef void (FOG_STDCALL* WakeConditionVariableFunc)(void* ConditionVariable);
typedef void (FOG_STDCALL* WakeAllConditionVariableFunc)(void* ConditionVariable);

struct FOG_NO_EXPORT ThreadConditionFuncsVista
{
  HMODULE kernel32;

  InitializeConditionVariableFunc _InitializeConditionVariable;
  SleepConditionVariableCSFunc _SleepConditionVariableCS;
  WakeConditionVariableFunc _WakeConditionVariable;
  WakeAllConditionVariableFunc _WakeAllConditionVariable;
};
static ThreadConditionFuncsVista ThreadCondition_funcs_vista;

static err_t FOG_CDECL ThreadCondition_ctor_vista(ThreadCondition* self, Lock* lock)
{
  ThreadCondition_funcs_vista._InitializeConditionVariable(&self->_cvar);
  self->_lock = lock;

  return ERR_OK;
}

static void FOG_CDECL ThreadCondition_dtor_vista(ThreadCondition* self)
{
  // The documentation says that there is no API to destroy the condition
  // variable, because the implementation doesn't need that. We only null
  // the pointers.
  self->_cvar = NULL;
  self->_lock = NULL;
}

static void FOG_CDECL ThreadCondition_signal_vista(ThreadCondition* self)
{
  ThreadCondition_funcs_vista._WakeConditionVariable(&self->_cvar);
}

static void FOG_CDECL ThreadCondition_broadcast_vista(ThreadCondition* self)
{
  ThreadCondition_funcs_vista._WakeAllConditionVariable(&self->_cvar);
}

static void FOG_CDECL ThreadCondition_wait_vista(ThreadCondition* self, const TimeDelta* maxTime)
{
  DWORD ms = INFINITE;

  if (maxTime != NULL)
    ms = (DWORD)Math::min<int64_t>(maxTime->getMilliseconds(), INFINITE);

  ThreadCondition_funcs_vista._SleepConditionVariableCS(&self->_cvar, &self->_lock->_handle, ms);
}

static void ThreadCondition_init_vista(void)
{
  ThreadConditionFuncsVista& funcs = ThreadCondition_funcs_vista;

  funcs.kernel32 = ::LoadLibraryW(L"Kernel32.dll");
  FOG_ASSERT(funcs.kernel32 != NULL);

  funcs._InitializeConditionVariable = (InitializeConditionVariableFunc)::GetProcAddress(funcs.kernel32, "InitializeConditionVariable");

  // If this symbol wasn't found then the OS is not running Windows-Vista or
  // never. In this case we are unable to use the CONDITION_VARIABLE related
  // functions.
  if (funcs._InitializeConditionVariable == NULL)
    return;

  // Fetch the rest of symbols.
  funcs._SleepConditionVariableCS = (SleepConditionVariableCSFunc)::GetProcAddress(funcs.kernel32, "SleepConditionVariableCS");
  funcs._WakeConditionVariable    = (WakeConditionVariableFunc   )::GetProcAddress(funcs.kernel32, "WakeConditionVariable"   );
  funcs._WakeAllConditionVariable = (WakeAllConditionVariableFunc)::GetProcAddress(funcs.kernel32, "WakeAllConditionVariable");

  // This shouldn't fail.
  FOG_ASSERT(funcs._SleepConditionVariableCS != NULL);
  FOG_ASSERT(funcs._WakeConditionVariable    != NULL);
  FOG_ASSERT(funcs._WakeAllConditionVariable != NULL);

  // Initialize the improved versions of functions to handle ThreadCondition.
  fog_api.threadcondition_ctor = ThreadCondition_ctor_vista;
  fog_api.threadcondition_dtor = ThreadCondition_dtor_vista;
  fog_api.threadcondition_signal = ThreadCondition_signal_vista;
  fog_api.threadcondition_broadcast = ThreadCondition_broadcast_vista;
  fog_api.threadcondition_wait = ThreadCondition_wait_vista;
}

#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::ThreadCondition (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)
static err_t FOG_CDECL ThreadCondition_ctor(ThreadCondition* self, Lock* lock)
{
  int rv = pthread_cond_init(&self->_cvar, NULL);

  if (rv != 0)
  {
    self->_lock = NULL;
    return ERR_RT_OUT_OF_MEMORY;
  }

  self->_lock = lock;
  return ERR_OK;
}

static void ThreadCondition_dtor(ThreadCondition* self)
{
  pthread_cond_destroy(&self->_cvar);
}

static void FOG_CDECL ThreadCondition_signal(ThreadCondition* self)
{
  int rv = pthread_cond_signal(&self->_cvar);
  FOG_ASSERT(rv == 0);
}

static void FOG_CDECL ThreadCondition_broadcast(ThreadCondition* self)
{
  int rv = pthread_cond_broadcast(&self->_cvar);
  FOG_ASSERT(rv == 0);
}

static void FOG_CDECL ThreadCondition_wait(ThreadCondition* self, const TimeDelta* maxTime)
{
  if (maxTime == NULL)
  {
    int rv = pthread_cond_wait(&self->_cvar, &self->_lock->_handle);
    FOG_ASSERT(rv == 0);
  }
  else
  {
    int64_t us = maxTime->getMicroseconds();

    // The timeout argument to pthread_cond_timedwait is in absolute time.
    struct timeval now;
    struct timespec atm;

    ::gettimeofday(&now, NULL);

    atm.tv_sec   = (now.tv_sec  + (us / TIME_US_PER_SECOND));
    atm.tv_nsec  = (now.tv_usec + (us % TIME_US_PER_SECOND)) * 1000;
    atm.tv_sec  += atm.tv_nsec / FOG_INT64_C(1000000000);
    atm.tv_nsec %= FOG_INT64_C(1000000000);

    // Overflow paranoia.
    FOG_ASSERT(atm.tv_sec >= now.tv_sec);

    int rv = pthread_cond_timedwait(&self->_cvar, &self->_lock->_handle, &atm);
    FOG_ASSERT(rv == 0 || rv == ETIMEDOUT);
  }
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ThreadCondition_init(void)
{
  fog_api.threadcondition_ctor = ThreadCondition_ctor;
  fog_api.threadcondition_dtor = ThreadCondition_dtor;
  fog_api.threadcondition_signal = ThreadCondition_signal;
  fog_api.threadcondition_broadcast = ThreadCondition_broadcast;
  fog_api.threadcondition_wait = ThreadCondition_wait;

#if defined(FOG_OS_WINDOWS) && !defined(FOG_NO_WIN_VISTA_SUPPORT) && \
                               !defined(FOG_NO_WIN_VISTA_CONDITION_VARIABLE)
  ThreadCondition_init_vista();
#endif // FOG_OS_WINDOWS
}

} // Fog namespace
