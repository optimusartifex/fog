// [Fog-Core Library - Public API]
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
#include <Fog/Core/Assert.h>
#include <Fog/Core/AutoLock.h>
#include <Fog/Core/AutoUnlock.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/ThreadCondition.h>
#include <Fog/Core/Time.h>

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#include <math.h>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
#include <errno.h>
#include <sys/time.h>
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::ThreadCondition]
// ============================================================================

/*
FAQ On subtle implementation details:

1) What makes this problem subtle?  Please take a look at "Strategies
for Implementing POSIX Condition Variables on Win32" by Douglas
C. Schmidt and Irfan Pyarali.
http://www.cs.wustl.edu/~schmidt/win32-cv-1.html It includes
discussions of numerous flawed strategies for implementing this
functionality.  I'm not convinced that even the final proposed
implementation has semantics that are as nice as this implementation
(especially with regard to broadcast() and the impact on threads that
try to Wait() after a broadcast() has been called, but before all the
original waiting threads have been signaled).

2) Why can't you use a single wait_event for all threads that call
Wait()?  See FAQ-question-1, or consider the following: If a single
event were used, then numerous threads calling Wait() could release
their cs locks, and be preempted just before calling
WaitForSingleObject().  If a call to broadcast() was then presented on
a second thread, it would be impossible to actually signal all
waiting(?) threads.  Some number of SetEvent() calls *could* be made,
but there could be no guarantee that those led to to more than one
signaled thread (SetEvent()'s may be discarded after the first!), and
there could be no guarantee that the SetEvent() calls didn't just
awaken "other" threads that hadn't even started waiting yet (oops).
Without any limit on the number of requisite SetEvent() calls, the
system would be forced to do many such calls, allowing many new waits
to receive spurious signals.

3) How does this implementation cause spurious signal events?  The
cause in this implementation involves a race between a signal via
time-out and a signal via Signal() or broadcast().  The series of
actions leading to this are:

a) Timer fires, and a waiting thread exits the line of code:

    WaitForSingleObject(waitingEvent, max_time.InMilliseconds());

b) That thread (in (a)) is randomly pre-empted after the above line,
leaving the waitingEvent reset (unsignaled) and still in the
_waitingList.

c) A call to Signal() (or broadcast()) on a second thread proceeds, and
selects the waiting cv_event (identified in step (b)) as the event to revive
via a call to SetEvent().

d) The Signal() method (step c) calls SetEvent() on waitingEvent (step b).

e) The waiting cv_event (step b) is now signaled, but no thread is
waiting on it.

f) When that waitingEvent (step b) is reused, it will immediately
be signaled (spuriously).


4) Why do you recycle events, and cause spurious signals?  First off,
the spurious events are very rare.  They can only (I think) appear
when the race described in FAQ-question-3 takes place.  This should be
very rare.  Most(?)  uses will involve only timer expiration, or only
Signal/broadcast() actions.  When both are used, it will be rare that
the race will appear, and it would require MANY Wait() and signaling
activities.  If this implementation did not recycle events, then it
would have to create and destroy events for every call to Wait().
That allocation/deallocation and associated construction/destruction
would be costly (per wait), and would only be a rare benefit (when the
race was "lost" and a spurious signal took place). That would be bad
(IMO) optimization trade-off.  Finally, such spurious events are
allowed by the specification of condition variables (such as
implemented in Vista), and hence it is better if any user accommodates
such spurious events (see usage note in ThreadCondition.h).

5) Why don't you reset events when you are about to recycle them, or
about to reuse them, so that the spurious signals don't take place?
The thread described in FAQ-question-3 step c may be pre-empted for an
arbitrary length of time before proceeding to step d.  As a result,
the wait_event may actually be re-used *before* step (e) is reached.
As a result, calling reset would not help significantly.

6) How is it that the callers lock is released atomically with the
entry into a wait state?  We commit to the wait activity when we
allocate the wait_event for use in a given call to Wait().  This
allocation takes place before the caller's lock is released (and
actually before our _internalLock is released).  That allocation is
the defining moment when "the wait state has been entered," as that
thread *can* now be signaled by a call to broadcast() or Signal().
Hence we actually "commit to wait" before releasing the lock, making
the pair effectively atomic.

8) Why do you need to lock your data structures during waiting, as the
caller is already in possession of a lock?  We need to Acquire() and
Release() our internal lock during Signal() and broadcast().  If we tried
to use a callers lock for this purpose, we might conflict with their
external use of the lock.  For example, the caller may use to consistently
hold a lock on one thread while calling Signal() on another, and that would
block Signal().

9) Couldn't a more efficient implementation be provided if you
preclude using more than one external lock in conjunction with a
single ThreadCondition instance?  Yes, at least it could be viewed
as a simpler API (since you don't have to reiterate the lock argument
in each Wait() call).  One of the constructors now takes a specific
lock as an argument, and a there are corresponding Wait() calls that
don't specify a lock now.  It turns that the resulting implmentation
can't be made more efficient, as the internal lock needs to be used by
Signal() and broadcast(), to access internal data structures.  As a
result, I was not able to utilize the user supplied lock (which is
being used by the user elsewhere presumably) to protect the private
member access.

9) Since you have a second lock, how can be be sure that there is no
possible deadlock scenario?  Our _internalLock is always the last
lock acquired, and the first one released, and hence a deadlock (due
to critical section problems) is impossible as a consequence of our
lock.

10) When doing a broadcast(), why did you copy all the events into
an STL queue, rather than making a linked-loop, and iterating over it?
The iterating during broadcast() is done so outside the protection
of the internal lock. As a result, other threads, such as the thread
wherein a related event is waiting, could asynchronously manipulate
the links around a cv_event.  As a result, the link structure cannot
be used outside a lock. broadcast() could iterate over waiting
events by cycling in-and-out of the protection of the internal_lock,
but that appears more expensive than copying the list into an STL
stack.

11) Why did the lock.h file need to be modified so much for this
change?  Central to a Condition Variable is the atomic release of a
lock during a Wait().  This places Wait() functionality exactly
mid-way between the two classes, Lock and Condition Variable.  Given
that there can be nested Acquire()'s of locks, and Wait() had to
Release() completely a held lock, it was necessary to augment the Lock
class with a recursion counter. Even more subtle is the fact that the
recursion counter (in a Lock) must be protected, as many threads can
access it asynchronously.  As a positive fallout of this, there are
now some ASSERTS to be sure no one Release()s a Lock more than they
Acquire()ed it, and there is ifdef'ed functionality that can detect
nested locks (legal under windows, but not under Posix).

12) Why is it that the cv_events removed from list in broadcast() and signal()
are not leaked?  How are they recovered??  The cv_events that appear to leak are
taken from the _waitingList.  For each element in that list, there is currently
a thread in or around the WaitForSingleObject() call of Wait(), and those
threads have references to these otherwise leaked events. They are passed as
arguments to be recycled just aftre returning from WaitForSingleObject().

13) Why did you use a custom container class (the linked list), when STL has
perfectly good containers, such as an STL list?  The STL list, as with any
container, does not guarantee the utility of an iterator across manipulation
(such as insertions and deletions) of the underlying container.  The custom
double-linked-list container provided that assurance.  I don't believe any
combination of STL containers provided the services that were needed at the same
O(1) efficiency as the custom linked list.  The unusual requirement
for the container class is that a reference to an item within a container (an
iterator) needed to be maintained across an arbitrary manipulation of the
container.  This requirement exposes itself in the Wait() method, where a
waitingEvent must be selected prior to the WaitForSingleObject(), and then it
must be used as part of recycling to remove the related instance from the
waiting_list.  A hash table (STL map) could be used, but I was embarrased to
use a complex and relatively low efficiency container when a doubly linked list
provided O(1) performance in all required operations.  Since other operations
to provide performance-and/or-fairness required queue (FIFO) and list (LIFO)
containers, I would also have needed to use an STL list/queue as well as an STL
map.  In the end I decided it would be "fun" to just do it right, and I put 
so many assertions into the container class that it is trivial to code review 
and validate its correctness.
*/

#if defined(FOG_OS_WINDOWS)
ThreadCondition::ThreadCondition(Lock* userLock) :
  _userLock(*userLock),
  _runState(RUNNING),
  _allocationCounter(0),
  _recyclingListSize(0)
{
  FOG_ASSERT(userLock);
}

ThreadCondition::~ThreadCondition()
{
  AutoLock locked(_internalLock);
  _runState = SHUTDOWN; // Prevent any more waiting.

  FOG_ASSERT(_recyclingListSize == _allocationCounter);

  // Rare shutdown problem.
  if (_recyclingListSize != _allocationCounter)
  {
    // There are threads of execution still in this->timedWait() and yet the
    // caller has instigated the destruction of this instance :-/.
    // A common reason for such "overly hasty" destruction is that the caller
    // was not willing to wait for all the threads to terminate.  Such hasty
    // actions are a violation of our usage contract, but we'll give the
    // waiting thread(s) one last chance to exit gracefully (prior to our
    // destruction).
    // Note: _waitingList *might* be empty, but recycling is still pending.
    AutoUnlock autoUnlock(_internalLock);
    broadcast(); // Make sure all waiting threads have been signaled.
    ::Sleep(10); // Give threads a chance to grab _internalLock.
    // All contained threads should be blocked on _userLock by now :-).
  }  // Reacquire _internalLock.

  FOG_ASSERT(_recyclingListSize == _allocationCounter);
}

void ThreadCondition::wait()
{
  // Default to "wait forever" timing, which means have to get a signal()
  // or broadcast() to come out of this wait state.
  timedWait(TimeDelta::fromMilliseconds(INFINITE));
}

void ThreadCondition::timedWait(const TimeDelta& maxTime)
{
  CVEvent* waitingEvent;
  HANDLE handle;
  {
    AutoLock locked(_internalLock);
    // Destruction in progress.
    if (RUNNING != _runState) return;
    waitingEvent = getEventForWaiting();
    handle = waitingEvent->handle();
    FOG_ASSERT(handle);
  }  // Release internal_lock.

  {
    // Release caller's lock
    AutoUnlock autoUnlock(_userLock);  
    WaitForSingleObject(handle, static_cast<DWORD>(maxTime.inMilliseconds()));
    // Minimize spurious signal creation window by recycling asap.
    AutoLock locked(_internalLock);
    recycleEvent(waitingEvent);
    // Release _internalLock
    // Reacquire callers lock to depth at entry.
  }
}

// Broadcast() is guaranteed to signal all threads that were waiting (i.e., had
// a cv_event internally allocated for them) before Broadcast() was called.
void ThreadCondition::broadcast()
{
  // See FAQ-question-10.
  List<HANDLE> handles;
  {
    AutoLock locked(_internalLock);
    if (_waitingList.isEmpty()) return;
    while (!_waitingList.isEmpty())
    {
      // This is not a leak from _waitingList.  See FAQ-question 12.
      handles.append(_waitingList.popBack()->handle());
    }
    // Release _internalLock.
  }
  while (!handles.isEmpty())
  {
    ::SetEvent(handles.top());
    handles.pop();
  }
}

// Signal() will select one of the waiting threads, and signal it (signal its
// cv_event).  For better performance we signal the thread that went to sleep
// most recently (LIFO).  If we want fairness, then we wake the thread that has
// been sleeping the longest (FIFO).
void ThreadCondition::signal()
{
  HANDLE handle;
  {
    AutoLock locked(_internalLock);
    if (_waitingList.isEmpty())
      return;  // No one to signal.
    // Only performance option should be used.
    // This is not a leak from waiting_list.  See FAQ-question 12.
    handle = _waitingList.popBack()->handle();  // LIFO.
    // Release _internalLock.
  }
  ::SetEvent(handle);
}

// GetEventForWaiting() provides a unique cv_event for any caller that needs to
// wait.  This means that (worst case) we may over time create as many cv_event
// objects as there are threads simultaneously using this instance's Wait()
// functionality.
ThreadCondition::CVEvent* ThreadCondition::getEventForWaiting()
{
  // We hold internal_lock, courtesy of Wait().
  CVEvent* cv_event;
  if (0 == _recyclingListSize)
  {
    FOG_ASSERT(_recyclingList.isEmpty());
    cv_event = fog_new CVEvent();
    // TODO: if (cv_event == NULL) ???
    cv_event->initListElement();
    _allocationCounter++;
    FOG_ASSERT(cv_event->handle());
  }
  else
  {
    cv_event = _recyclingList.popFront();
    _recyclingListSize--;
  }
  _waitingList.pushBack(cv_event);
  return cv_event;
}

// RecycleEvent() takes a cv_event that was previously used for Wait()ing, and
// recycles it for use in future Wait() calls for this or other threads.
// Note that there is a tiny chance that the cv_event is still signaled when we
// obtain it, and that can cause spurious signals (if/when we re-use the
// cv_event), but such is quite rare (see FAQ-question-5).
void ThreadCondition::recycleEvent(ThreadCondition::CVEvent* usedEvent)
{
  // We hold internal_lock, courtesy of Wait().
  // If the cv_event timed out, then it is necessary to remove it from
  // _waitingList.  If it was selected by Broadcast() or Signal(), then it is
  // already gone.
  usedEvent->extract();  // Possibly redundant
  _recyclingList.pushBack(usedEvent);
  _recyclingListSize++;
}

//------------------------------------------------------------------------------
// The next section provides the implementation for the private CVEvent class.
//------------------------------------------------------------------------------

// CVEvent provides a doubly-linked-list of events for use exclusively by the
// ThreadCondition class.

// This custom container was crafted because no simple combination of STL
// classes appeared to support the functionality required.  The specific
// unusual requirement for a linked-list-class is support for the Extract()
// method, which can remove an element from a list, potentially for insertion
// into a second list.  Most critically, the Extract() method is idempotent,
// turning the indicated element into an extracted singleton whether it was
// contained in a list or not.  This functionality allows one (or more) of
// threads to do the extraction.  The iterator that identifies this extractable
// element (in this case, a pointer to the list element) can be used after
// arbitrary manipulation of the (possibly) enclosing list container.  In
// general, STL containers do not provide iterators that can be used across
// modifications (insertions/extractions) of the enclosing containers, and
// certainly don't provide iterators that can be used if the identified
// element is *deleted* (removed) from the container.

// It is possible to use multiple redundant containers, such as an STL list,
// and an STL map, to achieve similar container semantics.  This container has
// only O(1) methods, while the corresponding (multiple) STL container approach
// would have more complex O(log(N)) methods (yeah... N isn't that large).
// Multiple containers also makes correctness more difficult to assert, as
// data is redundantly stored and maintained, which is generally evil.

ThreadCondition::CVEvent::CVEvent() : _handle(0)
{
  // Self referencing circular.
  _next = _prev = this;
}

ThreadCondition::CVEvent::~CVEvent()
{
  if (_handle == 0)
  {
    // This is the list holder
    while (!isEmpty())
    {
      CVEvent* cv_event = popFront();
      FOG_ASSERT(cv_event->validateAsItem());
      fog_delete(cv_event);
    }
  }

  FOG_ASSERT(isSingleton());

  if (_handle != 0)
  {
    int retVal = ::CloseHandle(_handle);
    FOG_ASSERT(retVal);
  }
}

// Change a container instance permanently into an element of a list.
void ThreadCondition::CVEvent::initListElement()
{
  FOG_ASSERT(!_handle);
  _handle = ::CreateEvent(NULL, false, false, NULL);
  FOG_ASSERT(_handle);
}

// Methods for use on lists.
bool ThreadCondition::CVEvent::isEmpty() const
{
  FOG_ASSERT(validateAsList());
  return isSingleton();
}

void ThreadCondition::CVEvent::pushBack(CVEvent* other)
{
  FOG_ASSERT(validateAsList());
  FOG_ASSERT(other->validateAsItem());
  FOG_ASSERT(other->isSingleton());

  // Prepare other for insertion.
  other->_prev = _prev;
  other->_next = this;

  // Cut into list.
  _prev->_next = other;
  _prev = other;

  FOG_ASSERT(validateAsDistinct(other));
}

ThreadCondition::CVEvent* ThreadCondition::CVEvent::popFront()
{
  FOG_ASSERT(validateAsList());
  FOG_ASSERT(!isSingleton());
  return _next->extract();
}

ThreadCondition::CVEvent* ThreadCondition::CVEvent::popBack()
{
  FOG_ASSERT(validateAsList());
  FOG_ASSERT(!isSingleton());
  return _prev->extract();
}

// Methods for use on list elements.
// Accessor method.
HANDLE ThreadCondition::CVEvent::handle() const
{
  FOG_ASSERT(validateAsItem());
  return _handle;
}

// Pull an element from a list (if it's in one).
ThreadCondition::CVEvent* ThreadCondition::CVEvent::extract()
{
  FOG_ASSERT(validateAsItem());
  if (!isSingleton())
  {
    // Stitch neighbors together.
    _next->_prev = _prev;
    _prev->_next = _next;
    // Make extractee into a singleton.
    _prev = _next = this;
  }
  FOG_ASSERT(isSingleton());
  return this;
}

// Method for use on a list element or on a list.
bool ThreadCondition::CVEvent::isSingleton() const
{
  FOG_ASSERT(validateLinks());
  return _next == this;
}

// Provide pre/post conditions to validate correct manipulations.
bool ThreadCondition::CVEvent::validateAsDistinct(ThreadCondition::CVEvent* other) const
{
  return validateLinks() && other->validateLinks() && (this != other);
}

bool ThreadCondition::CVEvent::validateAsItem() const
{
  return (0 != _handle) && validateLinks();
}

bool ThreadCondition::CVEvent::validateAsList() const
{
  return (0 == _handle) && validateLinks();
}

bool ThreadCondition::CVEvent::validateLinks() const
{
  // Make sure both of our neighbors have links that point back to us.
  // We don't do the O(n) check and traverse the whole loop, and instead only
  // do a local check to (and returning from) our immediate neighbors.
  return (_next->_prev == this) && (_prev->_next == this);
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
ThreadCondition::ThreadCondition(Lock* userLock) :
  _userMutex(&userLock->_handle)
{
  int rv = pthread_cond_init(&_condition, NULL);
  FOG_ASSERT(rv == 0);
}

ThreadCondition::~ThreadCondition()
{
  int rv = pthread_cond_destroy(&_condition);
  FOG_ASSERT(rv == 0);
}

void ThreadCondition::wait()
{
  int rv = pthread_cond_wait(&_condition, _userMutex);
  FOG_ASSERT(rv == 0);
}

void ThreadCondition::timedWait(const TimeDelta& maxTime)
{
  int64_t usecs = maxTime.inMicroseconds();

  // The timeout argument to pthread_cond_timedwait is in absolute time.
  struct timeval now;
  gettimeofday(&now, NULL);

  struct timespec abstime;
  abstime.tv_sec = now.tv_sec + (usecs / Time::MicrosecondsPerSecond);
  abstime.tv_nsec = (now.tv_usec + (usecs % Time::MicrosecondsPerSecond)) *
                    Time::NanosecondsPerMicrosecond;
  abstime.tv_sec += abstime.tv_nsec / Time::NanosecondsPerSecond;
  abstime.tv_nsec %= Time::NanosecondsPerSecond;

  // Overflow paranoia
  FOG_ASSERT(abstime.tv_sec >= now.tv_sec);

  int rv = pthread_cond_timedwait(&_condition, _userMutex, &abstime);
  FOG_ASSERT(rv == 0 || rv == ETIMEDOUT);
}

void ThreadCondition::broadcast()
{
  int rv = pthread_cond_broadcast(&_condition);
  FOG_ASSERT(rv == 0);
}

void ThreadCondition::signal()
{
  int rv = pthread_cond_signal(&_condition);
  FOG_ASSERT(rv == 0);
}
#endif // FOG_OS_POSIX

} // Fog namespace
