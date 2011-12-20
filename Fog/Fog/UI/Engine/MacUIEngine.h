// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_MACFBENGINE_H
#define _FOG_UI_ENGINE_MACFBENGINE_H

// [Dependencies]
#include <Fog/UI/Engine/UIEngine.h>
#include <Fog/UI/Engine/UIEngineWindow.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::MacUIEngine]
// ============================================================================

//! @brief Mac OS X or iOS specific @ref UIEngine implementation.
struct FOG_API MacUIEngine : public UIEngine
{
  FOG_DECLARE_OBJECT(MacUIEngine, UIEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacUIEngine();
  virtual ~MacUIEngine();

  // --------------------------------------------------------------------------
  // [Display / Palette]
  // --------------------------------------------------------------------------

  virtual void updateDisplayInfo();

  // --------------------------------------------------------------------------
  // [Keyboard / Mouse]
  // --------------------------------------------------------------------------

  virtual void setMouseWheelLines(uint32_t lines);

  // --------------------------------------------------------------------------
  // [ScheduleUpdate / DoUpdate]
  // --------------------------------------------------------------------------

  virtual void doBlitWindow(UIEngineWindow* window);

  // --------------------------------------------------------------------------
  // [Window Management]
  // --------------------------------------------------------------------------

  virtual err_t createWindow(UIEngineWindow* window);
  virtual err_t destroyWindow(UIEngineWindow* window);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_MACFBENGINE_H
