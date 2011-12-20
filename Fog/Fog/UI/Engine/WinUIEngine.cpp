// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/UI/Engine/WinUIEngine.h>
#include <Fog/UI/Engine/WinUIEngineWindow.h>

FOG_IMPLEMENT_OBJECT(Fog::WinUIEngine)

namespace Fog {

// ============================================================================
// [Fog::WinUIEngine - Constants]
// ============================================================================

#ifndef MSH_MOUSEWHEEL
# define MSH_MOUSEWHEEL L"MSWHEEL_ROLLMSG"
#endif

#ifndef WHEEL_DELTA
# define WHEEL_DELTA 120
#endif

#ifndef WM_MOUSEWHEEL
# define WM_MOUSEWHEEL (WM_MOUSELAST + 1)
#endif

#ifndef MOUSEZ_CLASSNAME
# define MOUSEZ_CLASSNAME L"MouseZ"
#endif

#ifndef MOUSEZ_TITLE
# define MOUSEZ_TITLE L"Magellan MSWHEEL"
#endif

#define MSH_WHEELMODULE_CLASS (MOUSEZ_CLASSNAME)
#define MSH_WHEELMODULE_TITLE (MOUSEZ_TITLE)

#ifndef MSH_WHEELSUPPORT
// MSH_WHEELSUPPORT returns TRUE if wheel support is active, otherwise FALSE.
# define MSH_WHEELSUPPORT L"MSH_WHEELSUPPORT_MSG"
#endif

#ifndef MSH_SCROLL_LINES
# define MSH_SCROLL_LINES L"MSH_SCROLL_LINES_MSG"
#endif

#ifndef WHEEL_PAGESCROLL
// MSH_SCROLL_LINES returns number of lines to scroll.
# define WHEEL_PAGESCROLL (UINT_MAX)
#endif

#ifndef SPI_SETWHEELSCROLLLINES
# define SPI_SETWHEELSCROLLLINES 105
#endif

// Keymasks.
#ifndef REPEATED_KEYMASK
# define REPEATED_KEYMASK (1 << 30)
#endif

#ifndef EXTENDED_KEYMASK
# define EXTENDED_KEYMASK (1 << 24)
#endif

// ============================================================================
// [Fog::WinUIEngine - Helpers]
// ============================================================================

static int WinUIEngine_getMouseWheelLines()
{
  UINT ucNumLines = UI_ENGINE_MISC_DEFAULT_WHEEL_LINES;
  SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ucNumLines, 0);
  return (int)ucNumLines;
}

// ============================================================================
// [Fog::WinUIEngine - WndProc]
// ============================================================================

static LRESULT CALLBACK WinUIEngine_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  WinUIEngine* engine = reinterpret_cast<WinUIEngine*>(Application::get()->getUIEngine());
  WinUIEngineWindowImpl* wImpl = NULL;

  if (FOG_IS_NULL(engine))
    goto _DefWindowProc;

  wImpl = reinterpret_cast<WinUIEngineWindowImpl*>(engine->getWindowByHandle(static_cast<void*>(hwnd)));

  if (FOG_IS_NULL(wImpl))
    goto _DefWindowProc;

  return wImpl->onWinMsg(message, wParam, lParam);

_DefWindowProc:
  return DefWindowProcW(hwnd, message, wParam, lParam);
}

// ============================================================================
// [Fog::WinUIEngine - Construction / Destruction]
// ============================================================================

WinUIEngine::WinUIEngine()
{
  initDisplay();
  initKeyboard();
  initMouse();
  initWndClass();

  _isInitialized = true;
}

WinUIEngine::~WinUIEngine()
{
  // TODO: Is it safe to unregister window classes when engine is destroyed?
  //
  // HINSTANCE hInstance = (HINSTANCE)GetModuleHandleW(NULL);
  // UnregisterClassW(L"Fog_Window", hInstance);
}

// ============================================================================
// [Fog::WinUIEngine - Init]
// ============================================================================

void WinUIEngine::initDisplay()
{
  updateDisplayInfo();
}

void WinUIEngine::initKeyboard()
{
  // Initialize key translation tables.
  MemOps::zero(&_keyMap, sizeof(_keyMap));

  _keyMap.vk[VK_BACK] = KEY_BACKSPACE;
  _keyMap.vk[VK_TAB] = KEY_TAB;
  _keyMap.vk[VK_CLEAR] = KEY_CLEAR;
  _keyMap.vk[VK_RETURN] = KEY_ENTER;
  _keyMap.vk[VK_PAUSE] = KEY_PAUSE;
  _keyMap.vk[VK_ESCAPE] = KEY_ESC;
  _keyMap.vk[VK_SPACE] = KEY_SPACE;
  _keyMap.vk[0xDE] = KEY_QUOTE;
  _keyMap.vk[0xBC] = KEY_COMMA;
  _keyMap.vk[0xBD] = KEY_MINUS;
  _keyMap.vk[0xBE] = KEY_PERIOD;
  _keyMap.vk[0xBF] = KEY_SLASH;
  _keyMap.vk['0'] = KEY_0;
  _keyMap.vk['1'] = KEY_1;
  _keyMap.vk['2'] = KEY_2;
  _keyMap.vk['3'] = KEY_3;
  _keyMap.vk['4'] = KEY_4;
  _keyMap.vk['5'] = KEY_5;
  _keyMap.vk['6'] = KEY_6;
  _keyMap.vk['7'] = KEY_7;
  _keyMap.vk['8'] = KEY_8;
  _keyMap.vk['9'] = KEY_9;
  _keyMap.vk[0xBA] = KEY_SEMICOLON;
  _keyMap.vk[0xBB] = KEY_EQUALS;
  _keyMap.vk[0xDB] = KEY_LEFT_BRACE;
  _keyMap.vk[0xDC] = KEY_BACKSLASH;
  _keyMap.vk[0xE2] = KEY_LESS;
  _keyMap.vk[0xDD] = KEY_RIGHT_BRACE;
  _keyMap.vk[0xC0] = KEY_BACKQUOTE;
  _keyMap.vk[0xDF] = KEY_BACKQUOTE;
  _keyMap.vk['A'] = KEY_A;
  _keyMap.vk['B'] = KEY_B;
  _keyMap.vk['C'] = KEY_C;
  _keyMap.vk['D'] = KEY_D;
  _keyMap.vk['E'] = KEY_E;
  _keyMap.vk['F'] = KEY_F;
  _keyMap.vk['G'] = KEY_G;
  _keyMap.vk['H'] = KEY_H;
  _keyMap.vk['I'] = KEY_I;
  _keyMap.vk['J'] = KEY_J;
  _keyMap.vk['K'] = KEY_K;
  _keyMap.vk['L'] = KEY_L;
  _keyMap.vk['M'] = KEY_M;
  _keyMap.vk['N'] = KEY_N;
  _keyMap.vk['O'] = KEY_O;
  _keyMap.vk['P'] = KEY_P;
  _keyMap.vk['Q'] = KEY_Q;
  _keyMap.vk['R'] = KEY_R;
  _keyMap.vk['S'] = KEY_S;
  _keyMap.vk['T'] = KEY_T;
  _keyMap.vk['U'] = KEY_U;
  _keyMap.vk['V'] = KEY_V;
  _keyMap.vk['W'] = KEY_W;
  _keyMap.vk['X'] = KEY_X;
  _keyMap.vk['Y'] = KEY_Y;
  _keyMap.vk['Z'] = KEY_Z;
  _keyMap.vk[VK_DELETE] = KEY_DELETE;

  _keyMap.vk[VK_NUMPAD0] = KEY_KP_0;
  _keyMap.vk[VK_NUMPAD1] = KEY_KP_1;
  _keyMap.vk[VK_NUMPAD2] = KEY_KP_2;
  _keyMap.vk[VK_NUMPAD3] = KEY_KP_3;
  _keyMap.vk[VK_NUMPAD4] = KEY_KP_4;
  _keyMap.vk[VK_NUMPAD5] = KEY_KP_5;
  _keyMap.vk[VK_NUMPAD6] = KEY_KP_6;
  _keyMap.vk[VK_NUMPAD7] = KEY_KP_7;
  _keyMap.vk[VK_NUMPAD8] = KEY_KP_8;
  _keyMap.vk[VK_NUMPAD9] = KEY_KP_9;
  _keyMap.vk[VK_DECIMAL] = KEY_KP_PERIOD;
  _keyMap.vk[VK_DIVIDE] = KEY_KP_DIVIDE;
  _keyMap.vk[VK_MULTIPLY] = KEY_KP_MULTIPLY;
  _keyMap.vk[VK_SUBTRACT] = KEY_KP_MINUS;
  _keyMap.vk[VK_ADD] = KEY_KP_PLUS;

  _keyMap.vk[VK_UP] = KEY_UP;
  _keyMap.vk[VK_DOWN] = KEY_DOWN;
  _keyMap.vk[VK_RIGHT] = KEY_RIGHT;
  _keyMap.vk[VK_LEFT] = KEY_LEFT;
  _keyMap.vk[VK_INSERT] = KEY_INSERT;
  _keyMap.vk[VK_HOME] = KEY_HOME;
  _keyMap.vk[VK_END] = KEY_END;
  _keyMap.vk[VK_PRIOR] = KEY_PAGE_UP;
  _keyMap.vk[VK_NEXT] = KEY_PAGE_DOWN;

  _keyMap.vk[VK_F1] = KEY_F1;
  _keyMap.vk[VK_F2] = KEY_F2;
  _keyMap.vk[VK_F3] = KEY_F3;
  _keyMap.vk[VK_F4] = KEY_F4;
  _keyMap.vk[VK_F5] = KEY_F5;
  _keyMap.vk[VK_F6] = KEY_F6;
  _keyMap.vk[VK_F7] = KEY_F7;
  _keyMap.vk[VK_F8] = KEY_F8;
  _keyMap.vk[VK_F9] = KEY_F9;
  _keyMap.vk[VK_F10] = KEY_F10;
  _keyMap.vk[VK_F11] = KEY_F11;
  _keyMap.vk[VK_F12] = KEY_F12;
  _keyMap.vk[VK_F13] = KEY_F13;
  _keyMap.vk[VK_F14] = KEY_F14;
  _keyMap.vk[VK_F15] = KEY_F15;

  _keyMap.vk[VK_NUMLOCK] = KEY_NUM_LOCK;
  _keyMap.vk[VK_CAPITAL] = KEY_CAPS_LOCK;
  _keyMap.vk[VK_SCROLL] = KEY_SCROLL_LOCK;
  _keyMap.vk[VK_RSHIFT] = KEY_RIGHT_SHIFT;
  _keyMap.vk[VK_LSHIFT] = KEY_LEFT_SHIFT;
  _keyMap.vk[VK_RCONTROL] = KEY_RIGHT_CTRL;
  _keyMap.vk[VK_LCONTROL] = KEY_LEFT_CTRL;
  _keyMap.vk[VK_RMENU] = KEY_RIGHT_ALT;
  _keyMap.vk[VK_LMENU] = KEY_LEFT_ALT;
  _keyMap.vk[VK_RWIN] = KEY_RIGHT_SUPER;
  _keyMap.vk[VK_LWIN] = KEY_LEFT_SUPER;

  _keyMap.vk[VK_HELP] = KEY_HELP;
  _keyMap.vk[42 /*VK_PRINT*/] = KEY_PRINT;
  _keyMap.vk[VK_SNAPSHOT] = KEY_PRINT;
  _keyMap.vk[VK_CANCEL] = KEY_BREAK;
  _keyMap.vk[VK_APPS] = KEY_MENU;

  updateKeyboardInfo();
}

void WinUIEngine::initMouse()
{
  _uMSH_MOUSEWHEEL = RegisterWindowMessageW(MSH_MOUSEWHEEL);
  _mouseInfo.setWheelLines(WinUIEngine_getMouseWheelLines());

  updateMouseInfo();
}

void WinUIEngine::initWndClass()
{
  HINSTANCE hInstance = (HINSTANCE)GetModuleHandleW(NULL);
  WNDCLASSEXW wc;

  // Init Window Class structure.
  wc.cbSize        = sizeof(WNDCLASSEXW);
  wc.style         = 0;
  wc.lpfnWndProc   = WinUIEngine_WndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hInstance;
  wc.hIcon         = LoadIconW(NULL, (LPCWSTR)IDI_APPLICATION);
  wc.hCursor       = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = NULL;
  wc.hIconSm       = LoadIconW(NULL, (LPCWSTR)IDI_APPLICATION);

  // "Fog_Window" window class.
  wc.style         = CS_OWNDC;
  wc.lpszClassName = L"Fog_Window";

  if (!RegisterClassExW(&wc))
    return;
}

// ============================================================================
// [Fog::WinUIEngine - Display / Palette]
// ============================================================================

void WinUIEngine::updateDisplayInfo()
{
  uint dibSize;
  LPBITMAPINFOHEADER dibHdr;

  HBITMAP hbm;
  HDC hdc;

  // Get screen size.
  _displayInfo._size.w  = ::GetSystemMetrics(SM_CXSCREEN);
  _displayInfo._size.h = ::GetSystemMetrics(SM_CYSCREEN);
  _displayInfo._is16BppSwapped = false;

  // Allocate enough space for a DIB header plus palette (for 8-bit depth) or 
  // bit-fields (for 16 and 32-bit depth).
  dibSize = sizeof(BITMAPINFOHEADER) + 256 * sizeof (RGBQUAD);
  dibHdr = (LPBITMAPINFOHEADER)MemMgr::calloc(dibSize);

  if (!dibHdr)
    return;

  dibHdr->biSize = sizeof(BITMAPINFOHEADER);

  // Get a device-dependent bitmap that's compatible with the screen.
  hdc = GetDC(NULL);
  hbm = CreateCompatibleBitmap(hdc, 1, 1);

  // Convert the DDB to a DIB.  We need to call GetDIBits twice:
  // - The first call just fills in the BITMAPINFOHEADER.
  // - The second fills in the bitfields or palette.
  GetDIBits(hdc, hbm, 0, 1, NULL, (LPBITMAPINFO)dibHdr, DIB_RGB_COLORS);
  GetDIBits(hdc, hbm, 0, 1, NULL, (LPBITMAPINFO)dibHdr, DIB_RGB_COLORS);

  DeleteObject(hbm);
  ReleaseDC(NULL, hdc);

  _displayInfo.setDepth(dibHdr->biBitCount);
  _paletteInfo.setLength(0);

  MemOps::zero(_paletteInfo._map, sizeof(_paletteInfo._map));

  switch (dibHdr->biBitCount)
  {
    case 8:
      _paletteInfo.setLength(256);
      break;

    case 16:
      if (dibHdr->biCompression == BI_BITFIELDS)
      {
        _displayInfo.setAMask32(0x00000000);
        _displayInfo.setRMask32(((DWORD*)((uint8_t*)dibHdr + dibHdr->biSize))[0]);
        _displayInfo.setGMask32(((DWORD*)((uint8_t*)dibHdr + dibHdr->biSize))[1]);
        _displayInfo.setBMask32(((DWORD*)((uint8_t*)dibHdr + dibHdr->biSize))[2]);
      }
      else
      {
        // I don't know if this could happen...
        _displayInfo.setAMask32(0x00000000);
        _displayInfo.setRMask32(0x0000F800);
        _displayInfo.setGMask32(0x000007E0);
        _displayInfo.setBMask32(0x0000001F);
      }
      break;

    case 24:
      _displayInfo.setAMask32(0x00000000);
      _displayInfo.setRMask32(0x00FF0000);
      _displayInfo.setGMask32(0x0000FF00);
      _displayInfo.setBMask32(0x000000FF);
      break;

    case 32:
      // This is the only valid format that MS-Windows can use for alpha 
      // compositing on the desktop. We don't need to detect anything.
      _displayInfo.setAMask32(0xFF000000);
      _displayInfo.setRMask32(0x00FF0000);
      _displayInfo.setGMask32(0x0000FF00);
      _displayInfo.setBMask32(0x000000FF);
      break;

    default:
      // Something failed, assume the worst.
      _displayInfo.setDepth(0);
      _displayInfo.setRMask32(0x00000000);
      _displayInfo.setGMask32(0x00000000);
      _displayInfo.setBMask32(0x00000000);
      break;
  }

  MemMgr::free(dibHdr);
}

// ============================================================================
// [Fog::WinUIEngine - Keyboard / Mouse]
// ============================================================================

void WinUIEngine::updateKeyboardInfo()
{
  INT keyboardDelay;
  DWORD keyboardSpeed;

  if (SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &keyboardDelay, 0))
  {
    _keyboardInfo.setRepeatingDelay(
      TimeDelta::fromMilliseconds((int64_t)((0.25f * (float)(keyboardDelay + 1)) * 1000.0f)));
  }

  if (SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &keyboardSpeed, 0))
  {
    _keyboardInfo.setRepeatingInterval(
      TimeDelta::fromMilliseconds((int64_t)((1.0f / (2.5f + (float)keyboardSpeed * 0.90f)) * 1000.0f)));
  }
}

void WinUIEngine::updateMouseInfo()
{
  DWORD dwTime;

  if (SystemParametersInfo(SPI_SETDOUBLECLICKTIME, 0, &dwTime, 0))
  {
    _mouseInfo.setDoubleClickInterval(TimeDelta::fromMilliseconds(dwTime));
  }
}

void WinUIEngine::setMouseWheelLines(uint32_t lines)
{
  if (lines == 0)
    lines = WinUIEngine_getMouseWheelLines();

  base::setMouseWheelLines(lines);
}

uint32_t WinUIEngine::getFogKeyFromWinKey(WPARAM vk, UINT scancode) const
{
  uint32_t key;

  if ((vk == VK_RETURN) && (scancode & 0x100))
    key = KEY_KP_ENTER;
  else
    key = _keyMap.vk[vk];

  return key;
}

uint32_t WinUIEngine::getUnicodeFromWinKey(WPARAM vk, UINT scancode) const
{
  switch (vk)
  {
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_TAB:
    case VK_ESCAPE:
    case VK_BACK:
    case VK_RETURN:
      return 0;
    default:
    {
      BYTE keystate[256];
      WCHAR unicode[2];

      if (!::GetKeyboardState(keystate))
        return 0;

      int count = ::ToUnicode((UINT)vk, scancode, keystate, unicode, FOG_ARRAY_SIZE(unicode), 0);

      if (count == 1 && unicode[0] > 31)
        return unicode[0];
      else if (count == 2)
        return CharW::ucs4FromSurrogate(unicode[0], unicode[1]);
      else
        return 0;
    }
  }
}

// ============================================================================
// [Fog::WinUIEngine - ScheduleUpdate / DoUpdate]
// ============================================================================

void WinUIEngine::doBlitWindow(UIEngineWindow* window)
{
  WinUIEngineWindowImpl* d = reinterpret_cast<WinUIEngineWindowImpl*>(window->_d);

  if (d->_bufferImage.isEmpty())
    return;

  if (d->_regionOfInterest.isEmpty())
    return;

  HDC winHdc = ::GetDC(static_cast<HWND>(d->_handle));
  HDC imgHdc;

  if (d->_bufferImage.getDC(&imgHdc) != ERR_OK)
    return;

  BoxI bBox = d->_regionOfInterest.getBoundingBox();

  if (d->_regionOfInterest.getLength() > 1)
  {
    HRGN hRgn;

    if (d->_regionOfInterest.toHRGN(&hRgn) == ERR_OK)
    {
      SelectClipRgn(winHdc, hRgn);
      DeleteObject((HGDIOBJ)hRgn);
    }
  }

  if (d->_windowHints & WINDOW_HINT_COMPOSITE)
  {
    POINT pt;
    pt.x = bBox.x0;
    pt.y = bBox.y0;

    SIZE size;
    size.cx = bBox.x1 - bBox.x0;
    size.cy = bBox.y1 - bBox.y0;
    
    BLENDFUNCTION blendFunc;
    blendFunc.BlendOp             = AC_SRC_OVER;
    blendFunc.BlendFlags          = 0;
    blendFunc.SourceConstantAlpha = Math::iround(d->_windowOpacity * 255.0f);
    blendFunc.AlphaFormat         = AC_SRC_ALPHA;

    ::UpdateLayeredWindow(static_cast<HWND>(d->_handle), winHdc, NULL, &size, imgHdc, &pt, 0, &blendFunc, ULW_ALPHA);
  }
  else
  {
    ::BitBlt(winHdc, bBox.x0, bBox.y0, bBox.x1 - bBox.x0, bBox.y1 - bBox.y0, imgHdc, bBox.x0, bBox.y0, SRCCOPY);
  }

  d->_bufferImage.releaseDC(imgHdc);
  ::ReleaseDC(static_cast<HWND>(d->_handle), winHdc);
}

// ============================================================================
// [Fog::WinUIEngine - Window Management]
// ============================================================================

err_t WinUIEngine::createWindow(UIEngineWindow* window, uint32_t flags)
{
  WinUIEngineWindowImpl* impl = fog_new WinUIEngineWindowImpl(this, window);
  if (FOG_IS_NULL(impl))
    return ERR_RT_OUT_OF_MEMORY;

  err_t err = impl->create(flags);
  if (FOG_IS_ERROR(err))
    fog_delete(impl);

  return err;
}

err_t WinUIEngine::destroyWindow(UIEngineWindow* window)
{
  UIEngineWindowImpl* d = window->_d;
  if (FOG_IS_NULL(d))
    return ERR_RT_INVALID_STATE;

  fog_delete(window->_d);
  return ERR_OK;
}

// ============================================================================
// [Fog::WinUIEngine - Misc]
// ============================================================================

const char* WinUIEngine::strFromMsg(uint msg)
{
  struct MsgTable
  {
    uint msg;
    const char *str;
  };

  static const MsgTable msgTable[] =
  {
    {  WM_NULL, "WM_NULL"  },
    {  WM_CREATE, "WM_CREATE"  },
    {  WM_DESTROY, "WM_DESTROY"  },
    {  WM_MOVE, "WM_MOVE"  },
    {  WM_SIZE, "WM_SIZE"  },
    {  WM_ACTIVATE, "WM_ACTIVATE"  },
    {  WM_SETFOCUS, "WM_SETFOCUS"  },
    {  WM_KILLFOCUS, "WM_KILLFOCUS"  },
    {  WM_ENABLE, "WM_ENABLE"  },
    {  WM_SETREDRAW, "WM_SETREDRAW"  },
    {  WM_SETTEXT, "WM_SETTEXT"  },
    {  WM_GETTEXT, "WM_GETTEXT"  },
    {  WM_GETTEXTLENGTH, "WM_GETTEXTLENGTH"  },
    {  WM_PAINT, "WM_PAINT"  },
    {  WM_CLOSE, "WM_CLOSE"  },
    {  WM_QUERYENDSESSION, "WM_QUERYENDSESSION"  },
    {  WM_QUIT, "WM_QUIT"  },
    {  WM_QUERYOPEN, "WM_QUERYOPEN"  },
    {  WM_ERASEBKGND, "WM_ERASEBKGND"  },
    {  WM_SYSCOLORCHANGE, "WM_SYSCOLORCHANGE"  },
    {  WM_ENDSESSION, "WM_ENDSESSION"  },
    {  WM_SHOWWINDOW, "WM_SHOWWINDOW"  },
    {  WM_WININICHANGE, "WM_WININICHANGE"  },
    {  WM_DEVMODECHANGE, "WM_DEVMODECHANGE"  },
    {  WM_ACTIVATEAPP, "WM_ACTIVATEAPP"  },
    {  WM_FONTCHANGE, "WM_FONTCHANGE"  },
    {  WM_TIMECHANGE, "WM_TIMECHANGE"  },
    {  WM_CANCELMODE, "WM_CANCELMODE"  },
    {  WM_SETCURSOR, "WM_SETCURSOR"  },
    {  WM_MOUSEACTIVATE, "WM_MOUSEACTIVATE"  },
    {  WM_CHILDACTIVATE, "WM_CHILDACTIVATE"  },
    {  WM_QUEUESYNC, "WM_QUEUESYNC"  },
    {  WM_GETMINMAXINFO, "WM_GETMINMAXINFO"  },
    {  WM_PAINTICON, "WM_PAINTICON"  },
    {  WM_ICONERASEBKGND, "WM_ICONERASEBKGND"  },
    {  WM_NEXTDLGCTL, "WM_NEXTDLGCTL"  },
    {  WM_SPOOLERSTATUS, "WM_SPOOLERSTATUS"  },
    {  WM_DRAWITEM, "WM_DRAWITEM"  },
    {  WM_MEASUREITEM, "WM_MEASUREITEM"  },
    {  WM_DELETEITEM, "WM_DELETEITEM"  },
    {  WM_VKEYTOITEM, "WM_VKEYTOITEM"  },
    {  WM_CHARTOITEM, "WM_CHARTOITEM"  },
    {  WM_SETFONT, "WM_SETFONT"  },
    {  WM_GETFONT, "WM_GETFONT"  },
    {  WM_SETHOTKEY, "WM_SETHOTKEY"  },
    {  WM_GETHOTKEY, "WM_GETHOTKEY"  },
    {  WM_QUERYDRAGICON, "WM_QUERYDRAGICON"  },
    {  WM_COMPAREITEM, "WM_COMPAREITEM"  },
    {  WM_COMPACTING, "WM_COMPACTING"  },
    {  WM_WINDOWPOSCHANGING, "WM_WINDOWPOSCHANGING"  },
    {  WM_WINDOWPOSCHANGED, "WM_WINDOWPOSCHANGED"  },
    {  WM_POWER, "WM_POWER"  },
    {  WM_COPYDATA, "WM_COPYDATA"  },
    {  WM_CANCELJOURNAL, "WM_CANCELJOURNAL"  },
    {  WM_NCCREATE, "WM_NCCREATE"  },
    {  WM_NCDESTROY, "WM_NCDESTROY"  },
    {  WM_NCCALCSIZE, "WM_NCCALCSIZE"  },
    {  WM_NCHITTEST, "WM_NCHITTEST"  },
    {  WM_NCPAINT, "WM_NCPAINT"  },
#if defined(WM_SYNCPAINT)
    {  WM_SYNCPAINT, "WM_SYNCPAINT"  },
#endif
    {  WM_NCACTIVATE, "WM_NCACTIVATE"  },
    {  WM_GETDLGCODE, "WM_GETDLGCODE"  },
    {  WM_NCMOUSEMOVE, "WM_NCMOUSEMOVE"  },
    {  WM_NCLBUTTONDOWN, "WM_NCLBUTTONDOWN"  },
    {  WM_NCLBUTTONUP, "WM_NCLBUTTONUP"  },
    {  WM_NCLBUTTONDBLCLK, "WM_NCLBUTTONDBLCLK"  },
    {  WM_NCRBUTTONDOWN, "WM_NCRBUTTONDOWN"  },
    {  WM_NCRBUTTONUP, "WM_NCRBUTTONUP"  },
    {  WM_NCRBUTTONDBLCLK, "WM_NCRBUTTONDBLCLK"  },
    {  WM_NCMBUTTONDOWN, "WM_NCMBUTTONDOWN"  },
    {  WM_NCMBUTTONUP, "WM_NCMBUTTONUP"  },
    {  WM_NCMBUTTONDBLCLK, "WM_NCMBUTTONDBLCLK"  },
    {  WM_KEYFIRST, "WM_KEYFIRST"  },
    {  WM_KEYDOWN, "WM_KEYDOWN"  },
    {  WM_KEYUP, "WM_KEYUP"  },
    {  WM_CHAR, "WM_CHAR"  },
    {  WM_DEADCHAR, "WM_DEADCHAR"  },
    {  WM_SYSKEYDOWN, "WM_SYSKEYDOWN"  },
    {  WM_SYSKEYUP, "WM_SYSKEYUP"  },
    {  WM_SYSCHAR, "WM_SYSCHAR"  },
    {  WM_SYSDEADCHAR, "WM_SYSDEADCHAR"  },
    {  WM_KEYLAST, "WM_KEYLAST"  },
    {  WM_INITDIALOG, "WM_INITDIALOG"  },
    {  WM_COMMAND, "WM_COMMAND"  },
    {  WM_SYSCOMMAND, "WM_SYSCOMMAND"  },
    {  WM_TIMER, "WM_TIMER"  },
    {  WM_HSCROLL, "WM_HSCROLL"  },
    {  WM_VSCROLL, "WM_VSCROLL"  },
    {  WM_INITMENU, "WM_INITMENU"  },
    {  WM_INITMENUPOPUP, "WM_INITMENUPOPUP"  },
    {  WM_MENUSELECT, "WM_MENUSELECT"  },
    {  WM_MENUCHAR, "WM_MENUCHAR"  },
    {  WM_ENTERIDLE, "WM_ENTERIDLE"  },
    {  WM_CTLCOLORMSGBOX, "WM_CTLCOLORMSGBOX"  },
    {  WM_CTLCOLOREDIT, "WM_CTLCOLOREDIT"  },
    {  WM_CTLCOLORLISTBOX, "WM_CTLCOLORLISTBOX"  },
    {  WM_CTLCOLORBTN, "WM_CTLCOLORBTN"  },
    {  WM_CTLCOLORDLG, "WM_CTLCOLORDLG"  },
    {  WM_CTLCOLORSCROLLBAR, "WM_CTLCOLORSCROLLBAR"  },
    {  WM_CTLCOLORSTATIC, "WM_CTLCOLORSTATIC"  },
    {  WM_MOUSEFIRST, "WM_MOUSEFIRST"  },
    {  WM_MOUSEMOVE, "WM_MOUSEMOVE"  },
    {  WM_LBUTTONDOWN, "WM_LBUTTONDOWN"  },
    {  WM_LBUTTONUP, "WM_LBUTTONUP"  },
    {  WM_LBUTTONDBLCLK, "WM_LBUTTONDBLCLK"  },
    {  WM_RBUTTONDOWN, "WM_RBUTTONDOWN"  },
    {  WM_RBUTTONUP, "WM_RBUTTONUP"  },
    {  WM_RBUTTONDBLCLK, "WM_RBUTTONDBLCLK"  },
    {  WM_MBUTTONDOWN, "WM_MBUTTONDOWN"  },
    {  WM_MBUTTONUP, "WM_MBUTTONUP"  },
    {  WM_MBUTTONDBLCLK, "WM_MBUTTONDBLCLK"  },
    {  WM_MOUSELAST, "WM_MOUSELAST"  },
    {  WM_PARENTNOTIFY, "WM_PARENTNOTIFY"  },
    {  WM_ENTERMENULOOP, "WM_ENTERMENULOOP"  },
    {  WM_EXITMENULOOP, "WM_EXITMENULOOP"  },
    {  WM_MDICREATE, "WM_MDICREATE"  },
    {  WM_MDIDESTROY, "WM_MDIDESTROY"  },
    {  WM_MDIACTIVATE, "WM_MDIACTIVATE"  },
    {  WM_MDIRESTORE, "WM_MDIRESTORE"  },
    {  WM_MDINEXT, "WM_MDINEXT"  },
    {  WM_MDIMAXIMIZE, "WM_MDIMAXIMIZE"  },
    {  WM_MDITILE, "WM_MDITILE"  },
    {  WM_MDICASCADE, "WM_MDICASCADE"  },
    {  WM_MDIICONARRANGE, "WM_MDIICONARRANGE"  },
    {  WM_MDIGETACTIVE, "WM_MDIGETACTIVE"  },
    {  WM_MDISETMENU, "WM_MDISETMENU"  },
    {  WM_DROPFILES, "WM_DROPFILES"  },
    {  WM_MDIREFRESHMENU, "WM_MDIREFRESHMENU"  },
    {  WM_CUT, "WM_CUT"  },
    {  WM_COPY, "WM_COPY"  },
    {  WM_PASTE, "WM_PASTE"  },
    {  WM_CLEAR, "WM_CLEAR"  },
    {  WM_UNDO, "WM_UNDO"  },
    {  WM_RENDERFORMAT, "WM_RENDERFORMAT"  },
    {  WM_RENDERALLFORMATS, "WM_RENDERALLFORMATS"  },
    {  WM_DESTROYCLIPBOARD, "WM_DESTROYCLIPBOARD"  },
    {  WM_DRAWCLIPBOARD, "WM_DRAWCLIPBOARD"  },
    {  WM_PAINTCLIPBOARD, "WM_PAINTCLIPBOARD"  },
    {  WM_VSCROLLCLIPBOARD, "WM_VSCROLLCLIPBOARD"  },
    {  WM_SIZECLIPBOARD, "WM_SIZECLIPBOARD"  },
    {  WM_ASKCBFORMATNAME, "WM_ASKCBFORMATNAME"  },
    {  WM_CHANGECBCHAIN, "WM_CHANGECBCHAIN"  },
    {  WM_HSCROLLCLIPBOARD, "WM_HSCROLLCLIPBOARD"  },
    {  WM_QUERYNEWPALETTE, "WM_QUERYNEWPALETTE"  },
    {  WM_PALETTEISCHANGING, "WM_PALETTEISCHANGING"  },
    {  WM_PALETTECHANGED, "WM_PALETTECHANGED"  },
    {  WM_HOTKEY, "WM_HOTKEY"  },
    {  WM_PENWINFIRST, "WM_PENWINFIRST"  },
    {  WM_PENWINLAST, "WM_PENWINLAST"  }
#if defined(WM_SIZING)
    ,{  WM_SIZING, "WM_SIZING"  }
#endif
#if defined(WM_MOVING)
    ,{  WM_MOVING, "WM_MOVING"  }
#endif
#if defined(WM_ENTERSIZEMOVE)
    ,{  WM_ENTERSIZEMOVE, "WM_ENTERSIZEMOVE"  }
#endif
#if defined(WM_EXITSIZEMOVE)
    ,{  WM_EXITSIZEMOVE, "WM_EXITSIZEMOVE"  }
#endif
#if defined(WM_ASYNCIO)
    ,{  WM_ASYNCIO, "WM_ASYNCIO"  }
#endif
  };

  for (size_t i = 0; i != FOG_ARRAY_SIZE(msgTable); i++)
  {
    if (msgTable[i].msg == msg)
      return msgTable[i].str;
  }

  return "Unknown";
}

} // Fog namespace
