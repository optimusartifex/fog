// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_WINUIENGINE_H
#define _FOG_UI_ENGINE_WINUIENGINE_H

// [Dependencies]
#include <Fog/UI/Engine/UIEngine.h>
#include <Fog/UI/Engine/UIEngineWindow.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::WinUIEngine]
// ============================================================================

//! @brief Windows specific @ref UIEngine implementation.
struct FOG_API WinUIEngine : public UIEngine
{
  FOG_DECLARE_OBJECT(WinUIEngine, UIEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinUIEngine();
  virtual ~WinUIEngine();

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  void initDisplay();
  void initKeyboard();
  void initMouse();
  void initWndClass();

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

  uint32_t getFogKeyFromWinKey(WPARAM vk, UINT scancode) const;
  uint32_t getUnicodeFromWinKey(WPARAM vk, UINT scancode) const;

  // --------------------------------------------------------------------------
  // [ScheduleUpdate / DoUpdate]
  // --------------------------------------------------------------------------

  virtual void doBlitWindow(UIEngineWindow* window);

  // --------------------------------------------------------------------------
  // [Window Management]
  // --------------------------------------------------------------------------

  virtual err_t createWindow(UIEngineWindow* window, uint32_t flags);
  virtual err_t destroyWindow(UIEngineWindow* window);

  // --------------------------------------------------------------------------
  // [Misc]
  // --------------------------------------------------------------------------

  const char* strFromMsg(uint msg);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Mouse wheel message ID (MSH_MOUSEWHEEL).
  UINT _uMSH_MOUSEWHEEL;

  //! @brief Fog-key <- VKey map.
  struct KeyMap
  {
    uint16_t vk[256];
  } _keyMap;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_WINUIENGINE_H
