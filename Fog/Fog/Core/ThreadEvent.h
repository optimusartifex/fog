// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADEVENT_H
#define _FOG_CORE_THREADEVENT_H

// [Dependencies]
#include <Fog/Core/Build.h>

#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/TypeInfo.h>
#include <Fog/Core/Constants.h>

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
#include <Fog/Core/Lock.h>
#include <Fog/Core/ThreadCondition.h>
#include <pthread.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct TimeDelta;

// ============================================================================
// [Fog::ThreadEvent]
// ============================================================================

//! @brief A ThreadEvent can be a useful thread synchronization tool when you 
//! want to allow one thread to wait for another thread to finish some work.
//!
//! Use a ThreadEvent when you would otherwise use a Lock+ThreadCondition to
//! protect a simple boolean value.  However, if you find yourself using a
//! ThreadEvent in conjunction with a Lock to wait for a more complex state
//! change (e.g., for an item to be added to a queue), then you should probably
//! be using a ThreadCondition instead of a ThreadEvent.
//!
//! NOTE: On Windows, this class provides a subset of the functionality afforded
//! by a Windows event object.  This is intentional.  If you are writing Windows
//! specific code and you need other features of a Windows event, then you might
//! be better off just using an Windows event directly.
struct FOG_API ThreadEvent
{
public:
  //! If manual_reset is true, then to set the event state to non-signaled,
  //! a consumer must call the @c reset() method.  If this parameter is false,
  //! then the system automatically resets the event state to non-signaled after
  //! a single waiting thread has been released.
  ThreadEvent(bool manualReset = false, bool initiallySignaled = false);

  //! WARNING: Destroying a ThreadEvent while threads are waiting on it is not
  //! supported.  Doing so will cause crashes or other instability.
  ~ThreadEvent();

  //! Put the event in the un-signaled state.
  void reset();

  //! Put the event in the signaled state.  Causing any thread blocked on wait()
  //! to be woken up.
  void signal();

  //! Returns true if the event is in the signaled state, else false.  If this
  //! is not a manual reset event, then this test will cause a reset.
  bool isSignaled();

  //! Wait indefinitely for the event to be signaled.  Returns true if the event
  //! was signaled, else false is returned to indicate that waiting failed.
  bool wait();

  //! Wait up until max_time has passed for the event to be signaled.  Returns
  //! true if the event was signaled.  If this method returns false, then it
  //! does not necessarily mean that max_time was exceeded.
  bool timedWait(const TimeDelta& maxTime);

private:
#if defined(FOG_OS_WINDOWS)
  HANDLE _event;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  // Needs to be listed first so it will be constructed first.
  Lock _lock;
  ThreadCondition _cvar;
  bool _signaled;
  bool _manualReset;
#endif // FOG_OS_POSIX

  FOG_DISABLE_COPY(ThreadEvent)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADEVENT_H
