// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_MACEVENTLOOPIMPL_H
#define _FOG_CORE_KERNEL_MACEVENTLOOPIMPL_H

// [Dependencies]
#include <Fog/Core/Kernel/EventLoopImpl.h>

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::MacEventLoopImpl]
// ============================================================================

//! @brief Mac event loop implementation.
struct FOG_API MacEventLoopImpl : public EventLoopImpl
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacEventLoopImpl();
  virtual ~MacEventLoopImpl();

  // --------------------------------------------------------------------------
  // [Run / Quit]
  // --------------------------------------------------------------------------

  virtual err_t runInternal();

  bool runWork();
  bool runDelayedWork();
  bool runIdleWork();

  virtual err_t quit();

  // --------------------------------------------------------------------------
  // [ScheduleWork]
  // --------------------------------------------------------------------------

  virtual void scheduleWork();
  virtual void scheduleDelayedWork(const Time& delayedWorkTime);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // @brief The thread's run loop.
  CFRunLoopRef _runLoop;

  CFRunLoopSourceRef _workSource;
  CFRunLoopSourceRef _delayedWorkSource;
  CFRunLoopSourceRef _idleWorkSource;

  CFRunLoopTimerRef _delayedWorkTimer;
  Time _delayedWorkTime;
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_KERNEL_MACEVENTLOOPIMPL_H
