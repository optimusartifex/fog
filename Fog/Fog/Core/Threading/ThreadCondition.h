// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_THREADCONDITION_H
#define _FOG_CORE_THREADING_THREADCONDITION_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
#include <pthread.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Fog::ThreadCondition]
// ============================================================================

//! @brief Condition variable.
//!
//! ThreadCondition wraps pthreads condition variable synchronization or, on
//! Windows, simulates it.  This functionality is very helpful for having
//! several threads wait for an event, as is common with a thread pool managed
//! by a master.  The meaning of such an event in the (worker) thread pool
//! scenario is that additional tasks are now available for processing.  It is
//! used in Chrome in the DNS prefetching system to notify worker threads that
//! a queue now has items (tasks) which need to be tended to.  A related use
//! would have a pool manager waiting on a ThreadCondition, waiting for a
//! thread in the pool to announce (signal) that there is now more room in a
//! (bounded size) communications queue for the manager to deposit tasks, or,
//! as a second example, that the queue of tasks is completely empty and all
//! workers are waiting.
//!
//! USAGE NOTE 1: spurious signal events are possible with this and
//! most implementations of condition variables.  As a result, be
//! *sure* to retest your condition before proceeding.  The following
//! is a good example of doing this correctly:
//!
//! while (!work_to_be_done()) wait(...);
//!
//! In contrast do NOT do the following:
//!
//! if (!work_to_be_done()) wait(...);  //! Don't do this.
//!
//! Especially avoid the above if you are relying on some other thread only
//! issuing a signal up *if* there is work-to-do.  There can/will
//! be spurious signals.  Recheck state on waiting thread before
//! assuming the signal was intentional. Caveat caller ;-).
//!
//! USAGE NOTE 2: Broadcast() frees up all waiting threads at once,
//! which leads to contention for the locks they all held when they
//! called wait(). This results in POOR performance. A much better
//! approach to getting a lot of threads out of wait() is to have each
//! thread (upon exiting wait()) call signal() to free up another
//! wait'ing thread.  Look at condition_variable_unittest.cc in Chrome)
//! for both examples.
//!
//! Broadcast() can be used nicely during teardown, as it gets the job
//! done, and leaves no sleeping threads... and performance is less
//! critical at that point.
//!
//! The semantics of Broadcast() are carefully crafted so that *all*
//! threads that were waiting when the request was made will indeed
//! get signaled.  Some implementations mess up, and don't signal them
//! all, while others allow the wait to be effectively turned off (for
//! a while while waiting threads come around).  This implementation
//! appears correct, as it will not "lose" any signals, and will guarantee
//! that all threads get signaled by Broadcast().
//!
//! This implementation offers support for "performance" in its selection of
//! which thread to revive.  Performance, in direct contrast with "fairness,"
//! assures that the thread that most recently began to wait() is selected by
//! signal to revive. Fairness would (if publicly supported) assure that the
//! thread that has wait()ed the longest is selected. The default policy
//! may improve performance, as the selected thread may have a greater chance of
//! having some of its stack data in various CPU caches.
//!
//! For a discussion of the many very subtle implementation details, see the FAQ
//! at ThreadCondition.cpp.
struct FOG_API ThreadCondition
{
  //! @brief Construct a condition variable for use with ONLY one user lock.
  explicit ThreadCondition(Lock* userLock);

  ~ThreadCondition();

  //! @brief Releases the caller's critical section atomically as it starts to
  //! sleep, and the reacquires it when it is signaled.
  void wait();
  void timedWait(const TimeDelta& maxTime);

  //! @brief Revives all waiting threads.
  void broadcast();
  //! @brief revives one waiting thread.
  void signal();

private:

#if defined(FOG_OS_WINDOWS)
  //! Define CVEvent class that is used to form circularly linked lists.
  //! The list container is an element with NULL as its handle_ value.
  //! The actual list elements have a non-zero handle_ value.
  //! All calls to methods MUST be done under protection of a lock so that links
  //! can be validated.  Without the lock, some links might asynchronously
  //! change, and the assertions would fail (as would list change operations).
  struct FOG_API CVEvent
  {
    //! Default constructor with no arguments creates a list container.
    CVEvent();
    ~CVEvent();

    //! Transitions an instance from a container, to an element.
    void initListElement();

    // Methods for use on lists.

    bool isEmpty() const;
    void pushBack(CVEvent* other);
    CVEvent* popFront();
    CVEvent* popBack();

    // Methods for use on list elements.

    //! Accessor method.
    HANDLE handle() const;
    //! Pull an element from a list (if it's in one).
    CVEvent* extract();

    //! Method for use on a list element or on a list.
    bool isSingleton() const;

  private:
    // Provide pre/post conditions to validate correct manipulations.

    bool validateAsDistinct(CVEvent* other) const;
    bool validateAsItem() const;
    bool validateAsList() const;
    bool validateLinks() const;

    HANDLE _handle;
    CVEvent* _next;
    CVEvent* _prev;

    _FOG_CLASS_NO_COPY(CVEvent)
  };

  //! @brief States
  //!
  //! Note that RUNNING is an unlikely number to have in RAM by accident.
  //! This helps with defensive destructor coding in the face of user error.
  enum RunState { SHUTDOWN = 0, RUNNING = 64213 };

  //! @brief Internal implementation methods supporting Wait().
  CVEvent* getEventForWaiting();
  void recycleEvent(CVEvent* usedEvent);

  RunState _runState;

  //! @brief Private critical section for access to member data.
  Lock _internalLock;

  //! @brief Lock that is acquired before calling Wait().
  Lock& _userLock;

  //! @brief Events that threads are blocked on.
  CVEvent _waitingList;

  //! @brief Free list for old events.
  CVEvent _recyclingList;
  int _recyclingListSize;

  //! @brief The number of allocated, but not yet deleted events.
  int _allocationCounter;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  pthread_cond_t _condition;
  pthread_mutex_t* _userMutex;
#endif // FOG_OS_POSIX

  _FOG_CLASS_NO_COPY(ThreadCondition)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_THREADCONDITION_H
