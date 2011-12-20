// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_EVENTLOOPOBSERVER_H
#define _FOG_CORE_KERNEL_EVENTLOOPOBSERVER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::EventLoopObserver]
// ============================================================================

//! @brief Event loop observer.
//!
//! @note Event loop observer is only used to observe dispatching of native
//! events or messages.
struct FOG_API EventLoopObserver
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EventLoopObserver();
  virtual ~EventLoopObserver();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  //! This method is called before processing an event.
  virtual void onBeforeDispatch(const void* ev) = 0;

  //! This method is called after processing an event.
  virtual void onAfterDispatch(const void* ev) = 0;
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_KERNEL_EVENTLOOPOBSERVER_H
