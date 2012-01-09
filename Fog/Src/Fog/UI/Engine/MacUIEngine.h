// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_MACUIENGINE_H
#define _FOG_UI_ENGINE_MACUIENGINE_H

// [Dependencies]
#include <Fog/UI/Engine/UIEngine.h>
#include <Fog/UI/Engine/UIEngineWindow.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::MacUIEngine]
// ============================================================================

//! @brief Windows specific @ref UIEngine implementation.
struct FOG_API MacUIEngine : public UIEngine
{
  FOG_DECLARE_OBJECT(MacUIEngine, UIEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MacUIEngine();
  virtual ~MacUIEngine();

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  void initDisplay();
  void initKeyboard();
  void initMouse();

  // --------------------------------------------------------------------------
  // [Display / Palette]
  // --------------------------------------------------------------------------

  virtual void updateDisplayInfo();

  // --------------------------------------------------------------------------
  // [Keyboard / Mouse]
  // --------------------------------------------------------------------------

  virtual void updateKeyboardInfo();
  virtual void updateMouseInfo();

  virtual void setMouseWheelLines(uint32_t lines);

  // --------------------------------------------------------------------------
  // [DoUpdate]
  // --------------------------------------------------------------------------

  virtual void doBlitWindow(UIEngineWindow* window);

  // --------------------------------------------------------------------------
  // [Window Management]
  // --------------------------------------------------------------------------

  virtual err_t createWindow(UIEngineWindow* window, uint32_t flags);
  virtual err_t destroyWindow(UIEngineWindow* window);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
  
  NSAutoreleasePool* _nsAutoReleasePool;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_MACUIENGINE_H
