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

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <pthread.h>
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
//! Windows, simulates it. This functionality is very helpful for having several
//! threads wait for an event, as is common with a thread pool managed by a
//! master. The meaning of such an event in the (worker) thread pool scenario
//! is that additional tasks are now available for processing.
//!
//! USAGE NOTE 1: spurious signal events are possible with this and most
//! implementations of condition variables. As a result, be sure to retest
//! your condition before proceeding. The following is a good example of doing
//! this correctly:
//!
//! while (!work_to_be_done())
//!   wait(...);
//!
//! In contrast do NOT do the following:
//!
//! if (!work_to_be_done())
//!   wait(...);
//!
//! Especially avoid the above if you are relying on some other thread only
//! issuing a signal up *if* there is work-to-do.  There can/will be spurious
//! signals. Recheck state on waiting thread before assuming the signal was 
//! intentional. Caveat caller.
//!
//! The broadcast() method frees up all waiting threads at once, which leads
//! to contention for the locks they all held when they called wait(). This 
//! results in poor performance. A much better approach to getting a lot of 
//! threads out of wait() is to have each thread (upon exiting wait()) call
//! signal() to free up another waiting thread.
//!
//! Broadcast() can be used nicely during teardown, as it gets the job done,
//! and leaves no sleeping threads... and performance is less critical at that
//! point.
//!
//! The semantics of broadcast() are carefully crafted so that all threads that
//! were waiting when the request was made will indeed get signaled. Some 
//! implementations mess up, and don't signal them all, while others allow the
//! wait to be effectively turned off (for a while while waiting threads come 
//! around). This implementation appears correct, as it will not lose any 
//! signals, and will guarantee that all threads get signaled by broadcast().
//!
//! This implementation offers support for "performance" in its selection of
//! which thread to revive. Performance, in direct contrast with "fairness,"
//! assures that the thread that most recently began to wait() is selected by
//! signal to revive. Fairness would (if publicly supported) assure that the
//! thread that has wait()ed the longest is selected. The default policy may
//! improve performance, as the selected thread may have a greater chance of
//! having some of its stack data in various CPU caches.
struct FOG_NO_EXPORT ThreadCondition
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Construct a condition variable for use with ONLY one user lock.
  explicit FOG_INLINE ThreadCondition(Lock* lock)
  {
    fog_api.threadcondition_ctor(this, lock);
  }

  FOG_INLINE ~ThreadCondition()
  {
    fog_api.threadcondition_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Lock* getLock()
  {
    return _lock;
  }

  // --------------------------------------------------------------------------
  // [Broadcast / Signal]
  // --------------------------------------------------------------------------

  //! @brief revives one waiting thread.
  FOG_INLINE void signal()
  {
    fog_api.threadcondition_signal(this);
  }

  //! @brief Revives all waiting threads.
  FOG_INLINE void broadcast()
  {
    fog_api.threadcondition_broadcast(this);
  }

  // --------------------------------------------------------------------------
  // [Wait]
  // --------------------------------------------------------------------------

  //! @brief Releases the caller's critical section atomically as it starts to
  //! sleep, and the reacquires it when it is signaled.
  FOG_INLINE void wait()
  {
    fog_api.threadcondition_wait(this, NULL);
  }

  FOG_INLINE void wait(const TimeDelta& maxTime)
  {
    fog_api.threadcondition_wait(this, &maxTime);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  void* _cvar;
  Lock* _lock;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  pthread_cond_t _cvar;
  Lock* _lock;
#endif // FOG_OS_POSIX

private:
  _FOG_NO_COPY(ThreadCondition)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_THREADCONDITION_H
