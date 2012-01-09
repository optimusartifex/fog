// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_THREADEVENT_H
#define _FOG_CORE_THREADING_THREADEVENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <Fog/Core/Threading/Lock.h>
# include <Fog/Core/Threading/ThreadCondition.h>
# include <pthread.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Fog::ThreadEvent]
// ============================================================================

//! @brief An event based synchronization.
//!
//! @note There is slight difference betweens Windows threading implementation
//! and POSIX one. The implementation of @c ThreadEvent and other threading
//! classes were inspired by http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
//! and http://www.cs.wustl.edu/~schmidt/win32-cv-2.html articles. These cover
//! the problematic and suggest default implementation for emulating Win32
//! functionality on POSIX and vice versa. This implementation doesn't support
//! @c PulseEvent(), the deprecated Win32 function.
struct FOG_NO_EXPORT ThreadEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create @c ThreadEvent.
  FOG_INLINE ThreadEvent(bool manualReset = false, bool initialState = false)
  {
    fog_api.threadevent_ctor(this, manualReset, initialState);
  }

  //! @brief Destroy @c ThreadEvent.
  //!
  //! @note Make sure that there is no object waiting for that event.
  FOG_INLINE ~ThreadEvent()
  {
    fog_api.threadevent_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Signal / Reset]
  // --------------------------------------------------------------------------

  //! @brief Put the event in the signaled state.
  FOG_INLINE void signal()
  {
    fog_api.threadevent_signal(this);
  }

  //! @brief Put the event in the un-signaled state.
  FOG_INLINE void reset()
  {
    fog_api.threadevent_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Wait]
  // --------------------------------------------------------------------------

  //! @brief Get whether the event is in the signaled state.
  FOG_INLINE bool isSignaled()
  {
    return fog_api.threadevent_isSignaled(this);
  }

  //! @brief Wait until the thread is signaled.
  FOG_INLINE bool wait()
  {
    return fog_api.threadevent_wait(this, NULL);
  }

  //! @brief Wait max @a maxTime time until the thread is signaled.
  FOG_INLINE bool wait(const TimeDelta& maxTime)
  {
    return fog_api.threadevent_wait(this, &maxTime);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  HANDLE _event;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  // We emulate the Windows EVENT functionality by using condition variable,
  // lock, and two boolean state members.

  Static<ThreadCondition> _cvar;
  Static<Lock> _lock;

  uint32_t _manualReset;
  uint32_t _signaled;
  size_t _waitingThreads;
#endif // FOG_OS_POSIX

private:
  _FOG_NO_COPY(ThreadEvent)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_THREADEVENT_H
