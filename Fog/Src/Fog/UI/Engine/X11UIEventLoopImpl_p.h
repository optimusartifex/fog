// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_X11UIEVENTLOOPIMPL_P_H
#define _FOG_UI_ENGINE_X11UIEVENTLOOPIMPL_P_H

// [Dependencies]
#include <Fog/Core/Kernel/EventLoopImpl.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/UI/Engine/X11UIEngine.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::X11UIEventLoopImpl]
// ============================================================================

struct FOG_NO_EXPORT X11UIEventLoopImpl : public EventLoopImpl
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  X11UIEventLoopImpl(X11UIEngine* engine);
  virtual ~X11UIEventLoopImpl();

  // --------------------------------------------------------------------------
  // [Run]
  // --------------------------------------------------------------------------

  virtual err_t runInternal() override;

  // --------------------------------------------------------------------------
  // [Schedule]
  // --------------------------------------------------------------------------

  virtual void scheduleWork() override;
  virtual void scheduleDelayedWork(const Time& delayedWorkTime) override;

  // --------------------------------------------------------------------------
  // [X11 Specific]
  // --------------------------------------------------------------------------

  void wakeUp();
  void waitForWork();

  bool doXSync();

  bool processNextXEvent();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief X11UIEngine link (owner of this event loop).
  X11UIEngine* _engine;
  
  //! @brief 1 if wake-up byte was activated.
  Atomic<int> _wakeUpSent;

  //! @brief Count of work per loop to try before sleep.
  int _workPerLoop;
  
  //! @brief The time at which we should call @c scheduleDelayedWork().
  Time _delayedWorkTime;
};

//! @}

} // Fog namespace

#endif // _FOG_UI_ENGINE_X11UIEVENTLOOPIMPL_P_H
