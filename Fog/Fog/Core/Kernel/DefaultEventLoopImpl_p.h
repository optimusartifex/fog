// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_DEFAULTEVENTLOOPIMPL_P_H
#define _FOG_CORE_KERNEL_DEFAULTEVENTLOOPIMPL_P_H

// [Dependencies]
#include <Fog/Core/Kernel/EventLoopImpl.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/Core/Tools/Time.h>

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::DefaultEventLoopImpl]
// ============================================================================

//! @internal
//!
//! @brief Default event loop implementation, operating system independent.
struct FOG_NO_EXPORT DefaultEventLoopImpl : public EventLoopImpl
{
  // --------------------------------------------------------------------------
  // [DefaultEventLoop]
  // --------------------------------------------------------------------------

  DefaultEventLoopImpl();
  virtual ~DefaultEventLoopImpl();

  // --------------------------------------------------------------------------
  // [Run / Quit]
  // --------------------------------------------------------------------------

  virtual err_t runInternal();

  // --------------------------------------------------------------------------
  // [Schedule]
  // --------------------------------------------------------------------------

  virtual void scheduleWork();
  virtual void scheduleDelayedWork(const Time& delayedWorkTime);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Used to sleep until there is more work to do.
  ThreadEvent wakeUpEvent;

  //! @brief The time at which we should call @c scheduleDelayedWork().
  Time delayedWorkTime;

private:
  _FOG_NO_COPY(DefaultEventLoopImpl)
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_KERNEL_DEFAULTEVENTLOOPIMPL_P_H
