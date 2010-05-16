// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_GUIENGINE_WIN_H
#define _FOG_GUI_GUIENGINE_WIN_H

// [Dependencies]
#include <Fog/Core/Object.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/GuiEngine.h>
#include <Fog/Gui/GuiEngine/Base.h>

namespace Fog {

//! @addtogroup Fog_Gui_Engine
//! @{

// ============================================================================
// [Fog::WinGuiEngine]
// ============================================================================

struct FOG_API WinGuiEngine : public BaseGuiEngine
{
  FOG_DECLARE_OBJECT(WinGuiEngine, BaseGuiEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinGuiEngine();
  virtual ~WinGuiEngine();

  // --------------------------------------------------------------------------
  // [Display]
  // --------------------------------------------------------------------------

  virtual void updateDisplayInfo();

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  virtual void doBlitWindow(GuiWindow* window, const IntBox* rects, sysuint_t count);

  // --------------------------------------------------------------------------
  // [GuiWindow]
  // --------------------------------------------------------------------------

  virtual GuiWindow* createGuiWindow(Widget* widget);
  virtual void destroyGuiWindow(GuiWindow* native);

  // --------------------------------------------------------------------------
  // [Windows Specific]
  // --------------------------------------------------------------------------

  //! @brief Mouse wheel message ID.
  UINT uMSH_MOUSEWHEEL;

  //! @brief Virtual keys map.
  struct WinKeymap
  {
    uint16_t vk[256];
  } _winKeymap;

  uint32_t winKeyToModifier(WPARAM* wParam, LPARAM lParam);
  uint32_t winKeyToUnicode(UINT vKey, UINT scanCode);
  uint32_t winKeyToFogKey(WPARAM vk, UINT scancode);

  // --------------------------------------------------------------------------
  // [Debug]
  // --------------------------------------------------------------------------

  const char* msgToStr(uint message);

  friend struct WinGuiWindow;
};

// ============================================================================
// [Fog::WinGuiWindow]
// ============================================================================

struct FOG_API WinGuiWindow : public BaseGuiWindow
{
  FOG_DECLARE_OBJECT(WinGuiWindow, BaseGuiWindow)

  typedef HWND Handle;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinGuiWindow(Widget* widget);
  virtual ~WinGuiWindow();

  // --------------------------------------------------------------------------
  // [Window Manipulation]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t flags);
  virtual err_t destroy();

  virtual err_t enable();
  virtual err_t disable();

  virtual err_t show();
  virtual err_t hide();

  virtual err_t move(const IntPoint& pt);
  virtual err_t resize(const IntSize& size);
  virtual err_t reconfigure(const IntRect& rect);

  virtual err_t takeFocus();

  virtual err_t setTitle(const String& title);
  virtual err_t getTitle(String& title);

  virtual err_t setIcon(const Image& icon);
  virtual err_t getIcon(Image& icon);

  virtual err_t setSizeGranularity(const IntPoint& pt);
  virtual err_t getSizeGranularity(IntPoint& pt);

  virtual err_t worldToClient(IntPoint* coords);
  virtual err_t clientToWorld(IntPoint* coords);

  // --------------------------------------------------------------------------
  // [Windowing System]
  // --------------------------------------------------------------------------

  virtual void onMousePress(uint32_t button, bool repeated);
  virtual void onMouseRelease(uint32_t button);

  virtual LRESULT onWinMsg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

  void getWindowRect(IntRect* windowRect, IntRect* clientRect);

  friend struct WinGuiEngine;

private:
  void calculateStyleFlags(uint32_t flags, DWORD& style, DWORD& exstyle);
  void doSystemMenu(uint32_t flags);
};

// ============================================================================
// [Fog::WinGuiBackBuffer]
// ============================================================================

//! @brief Provides cacheable buffer for widgets.
struct FOG_API WinGuiBackBuffer : public GuiBackBuffer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinGuiBackBuffer();
  virtual ~WinGuiBackBuffer();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual bool resize(int width, int height, bool cache);
  virtual void destroy();
  virtual void updateRects(const IntBox* rects, sysuint_t count);

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  void blitRects(HDC target, const IntBox* rects, sysuint_t count);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE HBITMAP getHBitmap() const { return _hBitmap; }
  FOG_INLINE HDC getHdc() const { return _hdc; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  HBITMAP _hBitmap;
  HDC _hdc;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_GUIENGINE_WIN_H
