// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#include <Fog/Core/C++/Base.h>
#if defined(FOG_GUI_WINDOWS)

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Gui/Engine/WinGuiEngine.h>
#include <Fog/Gui/Widget/Widget.h>

// ----------------------------------------------------------------------------
#ifndef MSH_MOUSEWHEEL
# define MSH_MOUSEWHEEL L"MSWHEEL_ROLLMSG"
#endif

#ifndef WHEEL_DELTA
# define WHEEL_DELTA 120
#endif

#ifndef WM_MOUSEWHEEL
# define WM_MOUSEWHEEL (WM_MOUSELAST+1)
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
// MSH_WHEELSUPPORT returns TRUE if wheel support is active, otherwise FALSE
# define MSH_WHEELSUPPORT L"MSH_WHEELSUPPORT_MSG"
#endif

#ifndef MSH_SCROLL_LINES
# define MSH_SCROLL_LINES L"MSH_SCROLL_LINES_MSG"
#endif

#ifndef WHEEL_PAGESCROLL
// MSH_SCROLL_LINES returns number of lines to scroll
# define WHEEL_PAGESCROLL (UINT_MAX)
#endif

#ifndef SPI_SETWHEELSCROLLLINES
# define SPI_SETWHEELSCROLLLINES 105
#endif

// Keymasks
#ifndef REPEATED_KEYMASK
# define REPEATED_KEYMASK (1 << 30)
#endif

#ifndef EXTENDED_KEYMASK
# define EXTENDED_KEYMASK (1 << 24)
#endif
// ----------------------------------------------------------------------------

FOG_IMPLEMENT_OBJECT(Fog::WinGuiEngine)
FOG_IMPLEMENT_OBJECT(Fog::WinGuiWindow)

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

#define GUI_ENGINE() \
  reinterpret_cast<WinGuiEngine*>(Application::getInstance()->getGuiEngine())

static void hwndGetRect(HWND handle, RectI* out)
{
  RECT wr;
  RECT cr;

  GetWindowRect(handle, &wr);
  GetClientRect(handle, &cr);

  int cw = cr.right - cr.left;
  int ch = cr.bottom - cr.top;

  out->setRect(wr.left, wr.top, cw, ch);
}

static LRESULT CALLBACK hwndWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  WinGuiWindow* guiWindow =
    reinterpret_cast<WinGuiWindow*>(
      GUI_ENGINE()->getWindowFromHandle((void*)hwnd));

  if (guiWindow)
  {
    return guiWindow->onWinMsg(hwnd, message, wParam, lParam);
  }
  else
  {
    return DefWindowProcW(hwnd, message, wParam, lParam);
  }
}

// Get count of lines to scroll when using mouse wheel, inspired in the MSDN:
//   http://msdn2.microsoft.com/en-us/library/ms645602.aspx
static uint32_t getWheelLinesCount(void)
{
  UINT ucNumLines = 3;
  SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ucNumLines, 0);
  return ucNumLines;
}

// ============================================================================
// [Fog::WinGuiEngine - Construction / Destruction]
// ============================================================================

WinGuiEngine::WinGuiEngine()
{
  memset(&_winKeymap, 0, sizeof(_winKeymap));

  // Initialize key translation tables.
  Fog::MemOps::zero(&_winKeymap, sizeof(WinKeymap));

  _winKeymap.vk[VK_BACK] = KEY_BACKSPACE;
  _winKeymap.vk[VK_TAB] = KEY_TAB;
  _winKeymap.vk[VK_CLEAR] = KEY_CLEAR;
  _winKeymap.vk[VK_RETURN] = KEY_ENTER;
  _winKeymap.vk[VK_PAUSE] = KEY_PAUSE;
  _winKeymap.vk[VK_ESCAPE] = KEY_ESC;
  _winKeymap.vk[VK_SPACE] = KEY_SPACE;
  _winKeymap.vk[0xDE] = KEY_QUOTE;
  _winKeymap.vk[0xBC] = KEY_COMMA;
  _winKeymap.vk[0xBD] = KEY_MINUS;
  _winKeymap.vk[0xBE] = KEY_PERIOD;
  _winKeymap.vk[0xBF] = KEY_SLASH;
  _winKeymap.vk['0'] = KEY_0;
  _winKeymap.vk['1'] = KEY_1;
  _winKeymap.vk['2'] = KEY_2;
  _winKeymap.vk['3'] = KEY_3;
  _winKeymap.vk['4'] = KEY_4;
  _winKeymap.vk['5'] = KEY_5;
  _winKeymap.vk['6'] = KEY_6;
  _winKeymap.vk['7'] = KEY_7;
  _winKeymap.vk['8'] = KEY_8;
  _winKeymap.vk['9'] = KEY_9;
  _winKeymap.vk[0xBA] = KEY_SEMICOLON;
  _winKeymap.vk[0xBB] = KEY_EQUALS;
  _winKeymap.vk[0xDB] = KEY_LEFT_BRACE;
  _winKeymap.vk[0xDC] = KEY_BACKSLASH;
  _winKeymap.vk[0xE2] = KEY_LESS;
  _winKeymap.vk[0xDD] = KEY_RIGHT_BRACE;
  _winKeymap.vk[0xC0] = KEY_BACKQUOTE;
  _winKeymap.vk[0xDF] = KEY_BACKQUOTE;
  _winKeymap.vk['A'] = KEY_A;
  _winKeymap.vk['B'] = KEY_B;
  _winKeymap.vk['C'] = KEY_C;
  _winKeymap.vk['D'] = KEY_D;
  _winKeymap.vk['E'] = KEY_E;
  _winKeymap.vk['F'] = KEY_F;
  _winKeymap.vk['G'] = KEY_G;
  _winKeymap.vk['H'] = KEY_H;
  _winKeymap.vk['I'] = KEY_I;
  _winKeymap.vk['J'] = KEY_J;
  _winKeymap.vk['K'] = KEY_K;
  _winKeymap.vk['L'] = KEY_L;
  _winKeymap.vk['M'] = KEY_M;
  _winKeymap.vk['N'] = KEY_N;
  _winKeymap.vk['O'] = KEY_O;
  _winKeymap.vk['P'] = KEY_P;
  _winKeymap.vk['Q'] = KEY_Q;
  _winKeymap.vk['R'] = KEY_R;
  _winKeymap.vk['S'] = KEY_S;
  _winKeymap.vk['T'] = KEY_T;
  _winKeymap.vk['U'] = KEY_U;
  _winKeymap.vk['V'] = KEY_V;
  _winKeymap.vk['W'] = KEY_W;
  _winKeymap.vk['X'] = KEY_X;
  _winKeymap.vk['Y'] = KEY_Y;
  _winKeymap.vk['Z'] = KEY_Z;
  _winKeymap.vk[VK_DELETE] = KEY_DELETE;

  _winKeymap.vk[VK_NUMPAD0] = KEY_KP_0;
  _winKeymap.vk[VK_NUMPAD1] = KEY_KP_1;
  _winKeymap.vk[VK_NUMPAD2] = KEY_KP_2;
  _winKeymap.vk[VK_NUMPAD3] = KEY_KP_3;
  _winKeymap.vk[VK_NUMPAD4] = KEY_KP_4;
  _winKeymap.vk[VK_NUMPAD5] = KEY_KP_5;
  _winKeymap.vk[VK_NUMPAD6] = KEY_KP_6;
  _winKeymap.vk[VK_NUMPAD7] = KEY_KP_7;
  _winKeymap.vk[VK_NUMPAD8] = KEY_KP_8;
  _winKeymap.vk[VK_NUMPAD9] = KEY_KP_9;
  _winKeymap.vk[VK_DECIMAL] = KEY_KP_PERIOD;
  _winKeymap.vk[VK_DIVIDE] = KEY_KP_DIVIDE;
  _winKeymap.vk[VK_MULTIPLY] = KEY_KP_MULTIPLY;
  _winKeymap.vk[VK_SUBTRACT] = KEY_KP_MINUS;
  _winKeymap.vk[VK_ADD] = KEY_KP_PLUS;

  _winKeymap.vk[VK_UP] = KEY_UP;
  _winKeymap.vk[VK_DOWN] = KEY_DOWN;
  _winKeymap.vk[VK_RIGHT] = KEY_RIGHT;
  _winKeymap.vk[VK_LEFT] = KEY_LEFT;
  _winKeymap.vk[VK_INSERT] = KEY_INSERT;
  _winKeymap.vk[VK_HOME] = KEY_HOME;
  _winKeymap.vk[VK_END] = KEY_END;
  _winKeymap.vk[VK_PRIOR] = KEY_PAGE_UP;
  _winKeymap.vk[VK_NEXT] = KEY_PAGE_DOWN;

  _winKeymap.vk[VK_F1] = KEY_F1;
  _winKeymap.vk[VK_F2] = KEY_F2;
  _winKeymap.vk[VK_F3] = KEY_F3;
  _winKeymap.vk[VK_F4] = KEY_F4;
  _winKeymap.vk[VK_F5] = KEY_F5;
  _winKeymap.vk[VK_F6] = KEY_F6;
  _winKeymap.vk[VK_F7] = KEY_F7;
  _winKeymap.vk[VK_F8] = KEY_F8;
  _winKeymap.vk[VK_F9] = KEY_F9;
  _winKeymap.vk[VK_F10] = KEY_F10;
  _winKeymap.vk[VK_F11] = KEY_F11;
  _winKeymap.vk[VK_F12] = KEY_F12;
  _winKeymap.vk[VK_F13] = KEY_F13;
  _winKeymap.vk[VK_F14] = KEY_F14;
  _winKeymap.vk[VK_F15] = KEY_F15;

  _winKeymap.vk[VK_NUMLOCK] = KEY_NUM_LOCK;
  _winKeymap.vk[VK_CAPITAL] = KEY_CAPS_LOCK;
  _winKeymap.vk[VK_SCROLL] = KEY_SCROLL_LOCK;
  _winKeymap.vk[VK_RSHIFT] = KEY_RIGHT_SHIFT;
  _winKeymap.vk[VK_LSHIFT] = KEY_LEFT_SHIFT;
  _winKeymap.vk[VK_RCONTROL] = KEY_RIGHT_CTRL;
  _winKeymap.vk[VK_LCONTROL] = KEY_LEFT_CTRL;
  _winKeymap.vk[VK_RMENU] = KEY_RIGHT_ALT;
  _winKeymap.vk[VK_LMENU] = KEY_LEFT_ALT;
  _winKeymap.vk[VK_RWIN] = KEY_RIGHT_SUPER;
  _winKeymap.vk[VK_LWIN] = KEY_LEFT_SUPER;

  _winKeymap.vk[VK_HELP] = KEY_HELP;
  _winKeymap.vk[42 /*VK_PRINT*/] = KEY_PRINT;
  _winKeymap.vk[VK_SNAPSHOT] = KEY_PRINT;
  _winKeymap.vk[VK_CANCEL] = KEY_BREAK;
  _winKeymap.vk[VK_APPS] = KEY_MENU;

  uMSH_MOUSEWHEEL = RegisterWindowMessageW(MSH_MOUSEWHEEL);

  // Get Wheel lines count.
  _wheelLines = getWheelLinesCount();

  // Query timings.
  INT keyboardDelay;
  DWORD keyboardSpeed;
  DWORD dwTime;

  // TODO: We should move this query to somewhere and update it if user
  // updated it in system settings.
  if (SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &keyboardDelay, 0))
  {
    _repeatingDelay = TimeDelta::fromMilliseconds((int64_t)((0.25f * (float)(keyboardDelay + 1)) * 1000.0f));
  }

  if (SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &keyboardSpeed, 0))
  {
    _repeatingInterval = TimeDelta::fromMilliseconds((int64_t)((1 / (2.5f + (float)keyboardSpeed * 0.90f)) * 1000.0f));
  }

  if (SystemParametersInfo(SPI_SETDOUBLECLICKTIME, 0, &dwTime, 0))
  {
    _doubleClickInterval = TimeDelta::fromMilliseconds(dwTime);
  }

  HINSTANCE hInstance = (HINSTANCE)GetModuleHandleW(NULL);
  WNDCLASSEXW wc;

  // Init Window Class structure.
  wc.cbSize        = sizeof(WNDCLASSEXW);
  wc.style         = 0;
  wc.lpfnWndProc   = hwndWndProc;
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
  if (!RegisterClassExW(&wc)) return;

  updateDisplayInfo();
  _initialized = true;
}

WinGuiEngine::~WinGuiEngine()
{
  // TODO: Is safe to unregister window classes when we ends?
  // HINSTANCE hInstance = (HINSTANCE)GetModuleHandleW(NULL);
  // UnregisterClassW(L"Fog_Window", hInstance);
}

void WinGuiEngine::minimize(GuiWindow* w)
{
  //ShowWindow((HWND)w->getHandle(), SW_MINIMIZE);
  w->getWidget()->show(WIDGET_VISIBLE_MINIMIZED);
}

void WinGuiEngine::maximize(GuiWindow* w)
{
  w->getWidget()->show(WIDGET_VISIBLE_MAXIMIZED);
}

// ============================================================================
// [Fog::WinGuiEngine - Display]
// ============================================================================

void WinGuiEngine::updateDisplayInfo()
{
  uint dibSize;
  LPBITMAPINFOHEADER dibHdr;
  HDC hdc;
  HBITMAP hbm;

  // Get screen size.
  _displayInfo.width  = ::GetSystemMetrics(SM_CXSCREEN);
  _displayInfo.height = ::GetSystemMetrics(SM_CYSCREEN);
  _displayInfo.is16BitSwapped = false;

  // Allocate enough space for a DIB header plus palette.
  // (for 8-bit modes) or bitfields (for 16 and 32-bit modes).
  dibSize = sizeof(BITMAPINFOHEADER) + 256 * sizeof (RGBQUAD);

  dibHdr = (LPBITMAPINFOHEADER)MemMgr::calloc(dibSize);
  if (!dibHdr) return;

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

  _displayInfo.depth = dibHdr->biBitCount;

  _paletteInfo.palCount = 0;
  memset(_paletteInfo.palConv, 0, sizeof(_paletteInfo.palConv));

  switch (dibHdr->biBitCount)
  {
    case 8:
      _paletteInfo.palCount = 256;
      break;
    case 16:
      if (dibHdr->biCompression == BI_BITFIELDS)
      {
        _displayInfo.rMask = ((DWORD*)((uint8_t*)dibHdr + dibHdr->biSize))[0];
        _displayInfo.gMask = ((DWORD*)((uint8_t*)dibHdr + dibHdr->biSize))[1];
        _displayInfo.bMask = ((DWORD*)((uint8_t*)dibHdr + dibHdr->biSize))[2];
      }
      else
      {
        // I don't know if this can happen...
        _displayInfo.rMask = 0x0000F800;
        _displayInfo.gMask = 0x000007E0;
        _displayInfo.bMask = 0x0000001F;
      }
      break;
    case 24:
      _displayInfo.rMask = 0x00FF0000;
      _displayInfo.gMask = 0x0000FF00;
      _displayInfo.bMask = 0x000000FF;
      break;
    case 32:
      _displayInfo.rMask = 0x00FF0000;
      _displayInfo.gMask = 0x0000FF00;
      _displayInfo.bMask = 0x000000FF;
      break;
    default:
      // Something failed, assume the worst.
      _displayInfo.depth = 0;
      _displayInfo.rMask = 0x00000000;
      _displayInfo.gMask = 0x00000000;
      _displayInfo.bMask = 0x00000000;
      break;
  }

  MemMgr::free(dibHdr);
}

// ============================================================================
// [Fog::WinGuiEngine - Update]
// ============================================================================

void WinGuiEngine::doBlitWindow(GuiWindow* window, const BoxI* rects, size_t count)
{
  Widget* w = window->getWidget();

  HDC hdc = GetDC((HWND)window->getHandle());
  WinGuiBackBuffer* back = reinterpret_cast<WinGuiBackBuffer*>(window->_backingStore);

  if (back->_prgb)
  {
    SIZE size;
    size.cx = back->getSize().getWidth();
    size.cy = back->getSize().getHeight();
    POINT pointSource;
    pointSource.x = 0;
    pointSource.y = 0;

    BLENDFUNCTION blend;
    blend.BlendOp             = AC_SRC_OVER;
    blend.BlendFlags          = 0;
    blend.SourceConstantAlpha = (int)(255 * window->getWidget()->getTransparency());
    blend.AlphaFormat         = AC_SRC_ALPHA;

    BOOL ret = UpdateLayeredWindow((HWND)window->getHandle(), hdc, 0, &size, back->_hdc, &pointSource, 0, &blend, ULW_ALPHA);

    if (!ret)
    {
      int e = GetLastError();
    }
  }
  else
  {
    back->blitRects(hdc, rects, count);
  }

  ReleaseDC((HWND)window->getHandle(), hdc);
}

// ============================================================================
// [Fog::WinGuiEngine - GuiWindow]
// ============================================================================

GuiWindow* WinGuiEngine::createGuiWindow(Widget* widget)
{
  return fog_new WinGuiWindow(widget);
}

void WinGuiEngine::destroyGuiWindow(GuiWindow* native)
{
  fog_delete(native);
}

// ============================================================================
// [Fog::WinGuiEngine - Windows Support]
// ============================================================================

uint32_t WinGuiEngine::winKeyToModifier(WPARAM* wParam, LPARAM lParam)
{
  uint32_t modifier = 0;

  switch (*wParam)
  {
    case VK_SHIFT:
    {
      if ((_keyboardStatus.modifiers & MODIFIER_LEFT_SHIFT) != 0 && !(GetKeyState(VK_LSHIFT) & 0x8000))
      {
        *wParam = VK_LSHIFT;
      }
      else if ((_keyboardStatus.modifiers & MODIFIER_RIGHT_SHIFT) != 0 && !(GetKeyState(VK_RSHIFT) & 0x8000))
      {
        *wParam = VK_RSHIFT;
      }
      else
      {
        // Win9x
        int sc = HIWORD(lParam) & 0xFF;

        if (sc == 0x2A)
          *wParam = VK_LSHIFT;
        else if (sc == 0x36)
          *wParam = VK_RSHIFT;
        else
          *wParam = VK_LSHIFT;
      }
      break;
    }

    case VK_CONTROL:
      if (lParam & EXTENDED_KEYMASK)
        *wParam = VK_RCONTROL;
      else
        *wParam = VK_LCONTROL;
      break;

    case VK_MENU:
      if (lParam & EXTENDED_KEYMASK)
        *wParam = VK_RMENU;
      else
        *wParam = VK_LMENU;
      break;
  }

  switch (*wParam)
  {
    case VK_LSHIFT  : modifier = MODIFIER_LEFT_SHIFT ; break;
    case VK_RSHIFT  : modifier = MODIFIER_RIGHT_SHIFT; break;
    case VK_LCONTROL: modifier = MODIFIER_LEFT_CTRL  ; break;
    case VK_RCONTROL: modifier = MODIFIER_RIGHT_CTRL ; break;
    case VK_LMENU   : modifier = MODIFIER_LEFT_ALT   ; break;
    case VK_RMENU   : modifier = MODIFIER_RIGHT_ALT  ; break;

    case VK_CAPITAL : modifier = MODIFIER_CAPS       ; break;
    case VK_NUMLOCK : modifier = MODIFIER_NUM        ; break;
    case VK_INSERT  : modifier = MODIFIER_MODE       ; break;
  }

  return modifier;
}

uint32_t WinGuiEngine::winKeyToUnicode(UINT vKey, UINT scanCode)
{
  switch (vKey)
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

      GetKeyboardState(keystate);
      if (ToUnicode(vKey, scanCode, keystate, unicode, FOG_ARRAY_SIZE(unicode), 0) == 1 && unicode[0] > 31)
        return unicode[0];
      else
        return 0;
    }
  }
}

uint32_t WinGuiEngine::winKeyToFogKey(WPARAM vk, UINT scancode)
{
  uint32_t key;

  if ((vk == VK_RETURN) && (scancode & 0x100))
    key = KEY_KP_ENTER;
  else
    key = _winKeymap.vk[vk];

  return key;
}

// ============================================================================
// [Fog::WinGuiEngine - Debug]
// ============================================================================

const char* WinGuiEngine::msgToStr(uint message)
{
  struct MsgTable
  {
    uint message;
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
    if (msgTable[i].message == message) return msgTable[i].str;
  }

  return "Unknown";
}

// ============================================================================
// [Fog::WinGuiWindow]
// ============================================================================

WinGuiWindow::WinGuiWindow(Widget* widget) :
  GuiWindow(widget)
{
  // TODO: Not looks good, better way how to check for this.
  bool b = (widget->getWindowFlags() & WINDOW_FRAMELESS) != 0 || (widget->getWindowFlags() & WINDOW_POPUP) != 0 || (widget->getWindowFlags() & WINDOW_FULLSCREEN) != 0;
  if (b)
  {
    b = (widget->getWindowFlags() & WINDOW_TRANSPARENT) != 0;
  }
  _backingStore = fog_new WinGuiBackBuffer(b);
}

WinGuiWindow::~WinGuiWindow()
{
  destroy();
  fog_delete(_backingStore);
}

void WinGuiWindow::moveToTop(GuiWindow* w)
{
  HWND top = HWND_TOP;
  if (w) top = (HWND)w->getHandle();

  SetWindowPos((HWND)getHandle(), top, 0, 0, 0, 0,
    SWP_NOMOVE |
    SWP_NOSIZE |
    SWP_NOACTIVATE |
    SWP_NOCOPYBITS |
    SWP_NOREDRAW);

  if (_widget)
  {
    // If you call this the always on top flag is automatically removed!
    if (_widget->isAlwaysOnTop())
    {
      _widget->overrideWindowFlags(_widget->getWindowFlags() & ~WINDOW_ALWAYS_ON_TOP);
    }
  }
}

void WinGuiWindow::moveToBottom(GuiWindow* w)
{
  // TODO: The Flag of HWND_TOPMOST will be cleared!! (update internal Flag)
  if (w)
  {
    SetWindowPos((HWND)getHandle(),(HWND)w->getHandle(),0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
  }
  else
  {
    SetWindowPos((HWND)getHandle(),HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
  }

  if (_widget)
  {
    // If you call this the always on top flag is automatically removed!
    if (_widget->isAlwaysOnTop())
    {
      _widget->overrideWindowFlags(_widget->getWindowFlags() & ~WINDOW_ALWAYS_ON_TOP);
    }
  }
}

void WinGuiWindow::setTransparency(float val)
{
  // LWA_COLORKEY
  // Full opaque windows do not have the layered flag.

  if (!static_cast<WinGuiBackBuffer*>(_backingStore)->_prgb)
  {
    if (val == 1.0)
    {
      // If no transparency is set we don't need the transparency flag
      // this is an optimization for windows, because windows always
      // double buffer transparent windows also for full opaque windows.
      LONG flag = GetWindowLong((HWND)getHandle(),GWL_EXSTYLE);
      if ((flag & WS_EX_LAYERED) != 0)
      {
        flag &=~WS_EX_LAYERED;
        SetWindowLong((HWND)getHandle(),GWL_EXSTYLE,flag);
      }
    }
    else
    {
      // Make sure window flag is set!
      LONG flag = GetWindowLong((HWND)getHandle(),GWL_EXSTYLE);
      if ((flag & WS_EX_LAYERED) == 0)
      {
        SetWindowLong((HWND)getHandle(), GWL_EXSTYLE, flag | WS_EX_LAYERED);
      }

      SetLayeredWindowAttributes((HWND)getHandle(), 0, (int)(255 * val), LWA_ALPHA);
    }
  }
  else
  {
    BLENDFUNCTION blend;

    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.AlphaFormat = 0;
    // On update a value of 255 will create an opaque window!
    blend.SourceConstantAlpha = (int)(val * 254.0f);

    LONG flag = GetWindowLong((HWND)getHandle(), GWL_EXSTYLE);
    if ((flag & WS_EX_LAYERED) == 0)
    {
      SetWindowLong((HWND)getHandle(), GWL_EXSTYLE, flag | WS_EX_LAYERED);
    }

    bool ret = UpdateLayeredWindow((HWND)getHandle(), NULL, NULL, NULL, NULL, NULL, (COLORREF)NULL, &blend, ULW_ALPHA);
    if (!ret)
    {
      int e = GetLastError();
      ret = ret;
    }
  }
}

void WinGuiWindow::calculateStyleFlags(uint32_t flags, DWORD& style, DWORD& exstyle)
{
  if (flags & WINDOW_FRAMELESS)
  {
    style = WS_POPUP;
    exstyle = WS_EX_APPWINDOW;
  }
  else if (flags & WINDOW_POPUP)
  {
    style = WS_POPUP;
    exstyle = WS_EX_TOPMOST|WS_EX_APPWINDOW;

    if (flags & WINDOW_TRANSPARENT)
    {
      exstyle |= WS_EX_LAYERED;
    }
    return;
  }
  else if (flags & WINDOW_NATIVE)
  {
    style = WS_OVERLAPPED | WS_CAPTION;
    exstyle = WS_EX_WINDOWEDGE;
  }
  else if (flags & WINDOW_TOOL)
  {
    style = WS_OVERLAPPED | WS_CAPTION;
    exstyle = WS_EX_TOOLWINDOW;
  }
  else if (flags & WINDOW_DIALOG)
  {
    style = WS_OVERLAPPED | WS_CAPTION | WS_DLGFRAME;
    //Dialog is always on top
    exstyle =  WS_EX_TOPMOST;  //probably we should also set WS_EX_DLGMODALFRAME
  }

  if (flags & WINDOW_SYSTEM_MENU)
  {
    style |= WS_SYSMENU;

    //The Close Button is only available if the system menu is set
    if (!(flags & WINDOW_CLOSE_BUTTON))
    {
      //We will disable the entry within SystemMenu on our own later
      style |= CS_NOCLOSE;
    }
  }

  if (!(flags & WINDOW_FRAMELESS))
  {
    //A frameless window does not have any decoration
    //if (helper::isResizeAble(flags)){
    if (flags & WINDOW_FIXED_SIZE)
    {
      style |= WS_BORDER;
    }
    else
    {
      style |=WS_THICKFRAME;
    }

    //A Context-helpbutton is only visible if no min/max button are defined
    //to offer to create a window with a context help button we implicitly
    //have to set min/max button to false

    if (flags & WINDOW_CONTEXT_HELP_BUTTON)
    {
      exstyle |= WS_EX_CONTEXTHELP;
    }
    else
    {
      if (flags & WINDOW_MINIMIZE)
      {
        //We will disable the entry within SystemMenu on our own later
        style |= WS_MINIMIZEBOX;
      }

      if (flags & WINDOW_MAXIMIZE)
      {
        //We will disable the entry within SystemMenu on our own later
        style |= WS_MAXIMIZEBOX;
      }
    }
  }

  if (flags & WINDOW_ALWAYS_ON_TOP)
  {
    exstyle |= WS_EX_TOPMOST;
  }
}

void WinGuiWindow::doSystemMenu(uint32_t flags)
{
  // Do it here, so we don't need to do it in INIT_MENU everytime

  HMENU hMenu = GetSystemMenu((HWND)_handle, false);
  if (hMenu == INVALID_HANDLE_VALUE)
    return;

  if (!(flags & WINDOW_CLOSE_BUTTON))
  {
    EnableMenuItem(hMenu, SC_CLOSE, MF_GRAYED);
  }
  else
  {
    // Make sure it is enabled.
    EnableMenuItem(hMenu, SC_CLOSE, MF_ENABLED);
  }

  if (!(flags & WINDOW_MINIMIZE))
  {
    EnableMenuItem(hMenu, SC_MINIMIZE, MF_GRAYED);
  }
  else
  {
    // Make sure it is enabled.
    EnableMenuItem(hMenu, SC_MINIMIZE, MF_ENABLED);
  }

  if (!(flags & WINDOW_MAXIMIZE))
  {
    EnableMenuItem(hMenu, SC_MAXIMIZE, MF_GRAYED);
  }
  else
  {
    // Make sure it is enabled.
    EnableMenuItem(hMenu, SC_MAXIMIZE, MF_ENABLED);
  }

  if (flags & WINDOW_FIXED_SIZE)
  {
    EnableMenuItem(hMenu, SC_SIZE, MF_GRAYED);
  }
  else
  {
    // Make sure it is enabled.
    EnableMenuItem(hMenu, SC_SIZE, MF_ENABLED);
  }
}

err_t WinGuiWindow::create(uint32_t flags)
{
  // Create the Window (HWND is returned)
  DWORD dwStyle;
  DWORD dwStyleEx;

  calculateStyleFlags(flags, dwStyle,dwStyleEx);

  if (_handle)
  {
    // Just update window with new styles.
    DWORD style = GetWindowLong((HWND)_handle,GWL_STYLE);
    DWORD exstyle = GetWindowLong((HWND)_handle,GWL_EXSTYLE);

    bool b = (flags & WINDOW_FRAMELESS) != 0 || (flags & WINDOW_POPUP) != 0 || (flags & WINDOW_FULLSCREEN) != 0;
    if (b)
    {
      b = (flags & WINDOW_TRANSPARENT) != 0;
    }

    bool visible = getWidget()->isVisible();

    if (visible)
    {
      // Prevent flickering!
      SendMessage((HWND)_handle, WM_SETREDRAW, (WPARAM)FALSE, (LPARAM)0);
    }

    if (reinterpret_cast<WinGuiBackBuffer*>(_backingStore)->_prgb != b)
    {
      reinterpret_cast<WinGuiBackBuffer*>(_backingStore)->_prgb = b;
      reinterpret_cast<WinGuiBackBuffer*>(_backingStore)->_clear();

      if (b)
      {
        SetWindowLong((HWND)_handle,GWL_EXSTYLE, exstyle &~WS_EX_LAYERED);
        dwStyleEx |= WS_EX_LAYERED;
      }
    }

    // It's much easier to first remove all possible flags and then create
    // a complete new flag and or it with the clean old one.
    style &=~ (WS_OVERLAPPED | WS_CAPTION | WS_POPUP | WS_SYSMENU | WS_DLGFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | CS_NOCLOSE | WS_THICKFRAME | WS_BORDER);

    SetWindowLong((HWND)_handle, GWL_STYLE, style | dwStyle);
    SetWindowLong((HWND)_handle, GWL_EXSTYLE, dwStyleEx);

    doSystemMenu(flags);

    getWidget()->setTransparency(getWidget()->getTransparency());

    if (visible)
    {
      // Allow repaint again!
      SendMessage((HWND)_handle, WM_SETREDRAW, (WPARAM) TRUE, (LPARAM) 0);
      UpdateWindow((HWND)_handle);

      SetWindowPos((HWND)_handle, 0,0,0,0,0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
      RedrawWindow((HWND)_handle,0,0, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    }

    return ERR_OK;
  }

  int x;
  int y;
  const WCHAR* wndClass;

// I don't think that we need differen window classes!
//   if (flags & WINDOW_POPUP)
//   {
//     x = -1;
//     y = -1;
//     wndClass = L"Fog_Popup";
//   }
//   else if (flags & WINDOW_TOOL)
//   {
//     x = CW_USEDEFAULT;
//     y = CW_USEDEFAULT;
//     wndClass = L"Fog_Tool";
//   }
//   else if (flags & WINDOW_DIALOG)
//   {
//     x = CW_USEDEFAULT;
//     y = CW_USEDEFAULT;
//     wndClass = L"Fog_Dialog";
//   }
//   else
//   {
    x = CW_USEDEFAULT;
    y = CW_USEDEFAULT;
    wndClass = L"Fog_Window";
//  }

  bool b = (flags & WINDOW_FRAMELESS) != 0 || (flags & WINDOW_POPUP) != 0 || (flags & WINDOW_FULLSCREEN) != 0;
  if (b)
  {
    b = (flags & WINDOW_TRANSPARENT) != 0;
  }

  if (b)
  {
    dwStyleEx |= WS_EX_LAYERED;
  }

  reinterpret_cast<WinGuiBackBuffer*>(_backingStore)->_prgb = b;

  _handle = (void*)CreateWindowExW(
    dwStyleEx, wndClass, L"",
    dwStyle, x, y,
    1, 1,
    NULL, NULL, (HINSTANCE)GetModuleHandleW(NULL), NULL);

  if (_handle == NULL)
  {
    Debug::dbgFunc("Fog::WinGuiWindow", "create", "CreateWindowExW() failed\n.");
    goto _Fail;
  }

  doSystemMenu(flags);

  // Create HWND <-> GuiWindow* connection.
  GUI_ENGINE()->mapHandle(_handle, this);

  // Windows are enabled by default.
  _enabled = true;
  // Windows are not visible by default.
  _visible = false;
  // Need to blit window content.
  _needBlit = true;
  // Default focus is no focus, Windows will inform us if this gets changed.
  _hasFocus = false;

  // Get correct window and client rectangle.
  getWindowRect(&_windowRect, &_clientRect);

  return ERR_OK;

_Fail:
  return ERR_UI_CANT_CREATE_WINDOW;
}

err_t WinGuiWindow::destroy()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  // Destroy HWND <-> GuiWindow* connection.
  GUI_ENGINE()->unmapHandle(_handle);

  // Destroy HWND.
  DestroyWindow((HWND)_handle);

  // Clear all variables.
  _handle = NULL;
  _hasFocus = false;
  _needBlit = false;

  return ERR_OK;
}

err_t WinGuiWindow::enable()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  if (!_enabled)
  {
    //_enabled = true;
    EnableWindow((HWND)_handle, TRUE);
  }
  return ERR_OK;
}

err_t WinGuiWindow::disable()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  if (_enabled)
  {
    EnableWindow((HWND)_handle, FALSE);
    //_enabled = false;
  }
  return ERR_OK;
}

err_t WinGuiWindow::show(uint32_t state)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;
  if (state == WIDGET_VISIBLE)
  {
    if (!IsWindowVisible((HWND)_handle))
    {
      ShowWindow((HWND)_handle, SW_SHOW);
    }
  }
  else if (state == WIDGET_VISIBLE_RESTORE)
  {
    ShowWindow((HWND)_handle, SW_RESTORE);
  }
  else if (state == WIDGET_VISIBLE_MAXIMIZED)
  {
    if (!IsZoomed((HWND)_handle))
    {
      bool pos =  !IsWindowVisible((HWND)_handle);

      ShowWindow((HWND)_handle, SW_SHOWMAXIMIZED);

      if (pos)
      {
        SendMessage((HWND)_handle,WM_USER,0,0);
      }
    }
  }
  else if (state == WIDGET_VISIBLE_MINIMIZED)
  {
    if (!IsIconic((HWND)_handle))
    {
      ShowWindow((HWND)_handle, SW_SHOWMINIMIZED);
    }
  }
  else if (state == WIDGET_VISIBLE_FULLSCREEN)
  {
    if (!IsWindowVisible((HWND)_handle))
    {
      ShowWindow((HWND)_handle, SW_SHOW);
    }
  }

  return ERR_OK;
}

err_t WinGuiWindow::hide()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  if (IsWindowVisible((HWND)_handle))
  {
    ShowWindow((HWND)_handle, SW_HIDE);
  }
  return ERR_OK;
}

err_t WinGuiWindow::setPosition(const PointI& pos)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  if ((_windowRect.x != pos.x) | (_windowRect.y != pos.y))
  {
    MoveWindow((HWND)_handle, pos.x, pos.y, _windowRect.w, _windowRect.h, FALSE);
  }

  return ERR_OK;
}

err_t WinGuiWindow::setSize(const SizeI& size)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;
  if ((size.getWidth() <= 0) | (size.getHeight() <= 0)) return ERR_RT_INVALID_ARGUMENT;

  if (_windowRect.getSize() != size)
  {
    MoveWindow((HWND)_handle,
      _widget->getX(),
      _widget->getY(),
      size.getWidth(),
      size.getHeight(),
      TRUE);
  }

  return ERR_OK;
}

err_t WinGuiWindow::setGeometry(const RectI& geometry)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;
  if (!geometry.isValid()) return ERR_RT_INVALID_ARGUMENT;

  if (_windowRect != geometry)
  {
    MoveWindow((HWND)_handle,
      geometry.getX(),
      geometry.getY(),
      geometry.getWidth(),
      geometry.getHeight(),
      TRUE);
  }

  return ERR_OK;
}

err_t WinGuiWindow::takeFocus()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  SetFocus((HWND)_handle);
  return ERR_OK;
}

err_t WinGuiWindow::setTitle(const StringW& title)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  SetWindowTextW((HWND)_handle, reinterpret_cast<const wchar_t*>(title.getData()));
  _title = title;

  return ERR_OK;
}

err_t WinGuiWindow::getTitle(StringW& title)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  title = _title;
  return ERR_OK;
}

err_t WinGuiWindow::setIcon(const Image& icon)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  // GUI TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t WinGuiWindow::getIcon(Image& icon)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  // GUI TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t WinGuiWindow::setSizeGranularity(const PointI& pt)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  _sizeGranularity = pt;
  return ERR_OK;
}

err_t WinGuiWindow::getSizeGranularity(PointI& pt)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  pt = _sizeGranularity;
  return ERR_OK;
}

err_t WinGuiWindow::worldToClient(PointI* coords)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  return ScreenToClient((HWND)_handle, (POINT *)coords)
    ? (err_t)ERR_OK
    : (err_t)ERR_UI_CANT_TRANSLATE_COORDINATES;
}

err_t WinGuiWindow::clientToWorld(PointI* coords)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  return ClientToScreen((HWND)_handle, (POINT *)coords)
    ? (err_t)ERR_OK
    : (err_t)ERR_UI_CANT_TRANSLATE_COORDINATES;
}

void WinGuiWindow::setOwner(GuiWindow* w)
{
  _owner = w;

  // Always set owner to toplevel window.
  SetWindowLongPtr((HWND)getHandle(), GWLP_HWNDPARENT, (LONG_PTR)_owner->getHandle());
  /* NOTE: To write code that is compatible with both 32-bit and 64-bit versions
     of Windows, use SetWindowLongPtr. When compiling for 32-bit Windows,
	 SetWindowLongPtr is defined as a call to the SetWindowLong function. - MSDN
	 http://msdn.microsoft.com/en-us/library/windows/desktop/ms644898%28v=vs.85%29.aspx
   */
}

void WinGuiWindow::releaseOwner()
{
  SetWindowLong((HWND)getHandle(), GWLP_HWNDPARENT, (LONG)0);
  SetActiveWindow((HWND)_owner->getHandle());
  SetForegroundWindow((HWND)_owner->getHandle());

  _owner = NULL;
}

void WinGuiWindow::onMousePress(uint32_t button, bool repeated)
{
  WinGuiEngine* guiEngine = GUI_ENGINE();

  if (guiEngine->_systemMouseStatus.uiWindow != this) return;

  if (guiEngine->_systemMouseStatus.buttons == 0 && !repeated)
  {
    SetCapture((HWND)_handle);
  }

  base::onMousePress(button, repeated);
}

void WinGuiWindow::onMouseRelease(uint32_t button)
{
  WinGuiEngine* guiEngine = GUI_ENGINE();

  if (guiEngine->_systemMouseStatus.uiWindow != this) return;

  if ((guiEngine->_systemMouseStatus.buttons & ~button) == 0)
  {
    ReleaseCapture();
  }

  base::onMouseRelease(button);
}

LRESULT WinGuiWindow::KeyboardMessageHelper(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  WinGuiEngine* guiEngine = GUI_ENGINE();
  //Keyboard messages
  switch (message)
  {
  case WM_SYSKEYDOWN:
  case WM_KEYDOWN:
    {
      uint32_t modifier = guiEngine->winKeyToModifier(&wParam, lParam);
      bool used = onKeyPress(
        guiEngine->winKeyToFogKey(wParam, HIWORD(lParam)),
        modifier,
        (uint32_t)wParam,
        CharW(guiEngine->winKeyToUnicode(wParam, HIWORD(lParam)))
        );

      if (used) return 0;
    }
  case WM_SYSKEYUP:
  case WM_KEYUP:
    {
      uint32_t modifier = guiEngine->winKeyToModifier(&wParam, lParam);
      bool used = onKeyRelease(
        guiEngine->winKeyToFogKey(wParam, HIWORD(lParam)),
        modifier,
        (uint32_t)wParam,
        CharW(guiEngine->winKeyToUnicode(wParam, HIWORD(lParam)))
        );

      if (used) return 0;
    }
  }
  return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT WinGuiWindow::MouseMessageHelper(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  WinGuiEngine* guiEngine = GUI_ENGINE();

  switch (message)
  {
    case WM_MOUSEHOVER:
    case WM_MOUSEMOVE:
    {
      // Mouse tracking for WM_MOUSELEAVE message.
      TRACKMOUSEEVENT tme;
      tme.cbSize    = sizeof(TRACKMOUSEEVENT);
      tme.dwFlags   = TME_LEAVE;
      tme.hwndTrack = hwnd;
      TrackMouseEvent(&tme);

      int x = (int)(short)LOWORD(lParam);
      int y = (int)(short)HIWORD(lParam);

      // WM_MOUSEHOVER is not sent sometimes.
      if (guiEngine->_systemMouseStatus.uiWindow != this ||
        !guiEngine->_systemMouseStatus.hover)
      {
        onMouseHover(x, y);
      }
      else
      {
        onMouseMove(x, y);
      }

      return 0;
    }

    case WM_MOUSELEAVE:
    {
      if (guiEngine->_systemMouseStatus.uiWindow == this &&
        !guiEngine->_systemMouseStatus.buttons)
      {
        onMouseLeave(-1, -1);
      }
      return 0;
    }

    case WM_LBUTTONDOWN: onMousePress(BUTTON_LEFT  , false); return 0;
    case WM_RBUTTONDOWN: onMousePress(BUTTON_RIGHT , false); return 0;
    case WM_MBUTTONDOWN: onMousePress(BUTTON_MIDDLE, false); return 0;

    case WM_LBUTTONUP: onMouseRelease(BUTTON_LEFT  ); return 0;
    case WM_RBUTTONUP: onMouseRelease(BUTTON_RIGHT ); return 0;
    case WM_MBUTTONUP: onMouseRelease(BUTTON_MIDDLE); return 0;

    case WM_MOUSEWHEEL:
    {
      // Typical for synaptics.
      if (guiEngine->_systemMouseStatus.uiWindow != this ||
        !guiEngine->_systemMouseStatus.hover)
      {
        POINT pt;
        DWORD cursorPos = GetMessagePos();
        pt.x = (int)(short)LOWORD(cursorPos);
        pt.y = (int)(short)HIWORD(cursorPos);
        ScreenToClient(hwnd, &pt);
        onMouseHover(pt.x, pt.y);
      }

      if ((int16_t)HIWORD(wParam) < 0)
      {
        onMouseWheel(WHEEL_DOWN);
      }
      else
      {
        onMouseWheel(WHEEL_UP);
      }
      return 0;
    }
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT WinGuiWindow::onWinMsg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  //IDEA: optimize message handling (make lesser comparisions)
  WinGuiEngine* guiEngine = GUI_ENGINE();
  //TODO:check GuiEngine for modal widget!
  GuiWindow* curmodal = isModal() ? this : 0;
  //bool allowinput = !curmodal || ((HWND)curmodal->getHandle() == hwnd);

  if (message >= WM_NCCREATE && message <= WM_NCXBUTTONDBLCLK)
  {
    if (message > WM_NCMOUSEMOVE)
    {
      if (hasPopUp())
      {
        closePopUps();
      }
    }
    goto defWindowProc;
  }
  else if (message >= WM_KEYFIRST && message <= WM_KEYLAST)
  {
    return KeyboardMessageHelper(hwnd,message,wParam,lParam);
  }
  else if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST || message== WM_MOUSEMOVE || message== WM_MOUSELEAVE)
  {
    return MouseMessageHelper(hwnd,message,wParam,lParam);
  }
  else
  {
    switch (message)
    {
      case WM_SHOWWINDOW:
        goto defWindowProc;

      case WM_SYSCOMMAND:
      {
        int command = wParam & 0xfff0;
        if (curmodal)
        {
          if (command == SC_MINIMIZE)
          {
            //get parent of all modal windows (first parent without modal flag)
            GuiWindow* parent = getModalBaseWindow();

            if (parent->getWidget()->getWindowFlags() & WINDOW_MINIMIZE)
            {
              //hide all modal windows
              //this is because else the system will not move the owned
              //child to taskbar. It only minimize it to the owner-window
              curmodal->showAllModalWindows(WIDGET_HIDDEN_BY_PARENT);
              //we need to enable the parent, else it can not be activated
              //using click in taskbar
              parent->enable();
              //hide our parent widget
              parent->getWidget()->show(WIDGET_VISIBLE_MINIMIZED);
              //set temporary variable to this, so we can use it later
              //to show all widgets again!
              parent->setLastModalWindow(curmodal);
            }
            else
            {
              // If the parent could not be minimized, the modal window also
              // could not be minimized.

              // TODO: Stefan, do we really want to beep?
              MessageBeep(0xFFFFFFFF);
            }
            return 0;
          }
        }

        if (!getWidget()->isDragAble())
        {
          if (command == SC_MOVE)
          {
            //do not allow to move
            return 0;
          }
        }

        goto defWindowProc;
      }

      case WM_USER:
      {
        // Message is sent to create a onGeometry on maximized application start.
        RectI wr;
        RectI cr;
        getWindowRect(&wr, &cr);
        onGeometry(wr, cr);
        return 0;
      }

      case WM_WINDOWPOSCHANGED:
      {
        // Handles everything within here -> no need for WM_MOVE, WM_SIZE, WM_SHOWWINDOW
        // also we have better control when to send onConfiguration-Events!

        WINDOWPOS *pos = (WINDOWPOS*)lParam;

        if (pos->flags & SWP_SHOWWINDOW)
        {
          //window_was_shown();
          onVisibility(WIDGET_VISIBLE);
          return 0;
        }
        else if (pos->flags & SWP_HIDEWINDOW)
        {
          //We need to enable Owner-Window to be able to restore it again
          if (!curmodal)
          {
            onVisibility(WIDGET_HIDDEN);
          }

          return 0;
        }

        if (IsIconic(hwnd) && (pos->flags & SWP_FRAMECHANGED))
        {
          //window_minimized
          onVisibility(WIDGET_VISIBLE_MINIMIZED);
          //no configuration change needed!
          return 0;
        }
        else if (IsZoomed(hwnd) && (pos->flags & SWP_FRAMECHANGED))
        {
          //window_maximized
          onVisibility(WIDGET_VISIBLE_MAXIMIZED);
        }
        else
        {
          if (!(pos->flags & SWP_NOMOVE) && pos->flags & SWP_FRAMECHANGED && !(pos->flags & SWP_NOSIZE))
          {
            //should always be NULL but who knows?
            if (!curmodal)
            {
              //only for the case of minimized modal widgets we need
              //a pointer to last modal window on stack to be able
              //to show all modal widgets on 'restore' again (single linked list)
              curmodal = getLastModalWindow();
            }

            bool mini = getWidget()->getVisibility() == WIDGET_VISIBLE_MINIMIZED;

            if (curmodal && mini)
            {
              curmodal->showAllModalWindows(WIDGET_VISIBLE_RESTORE);
              setLastModalWindow(0);
              disable();
            }

            //TODO: Is it important to know, if it is a change from minimize to normal
            //or from maximize to normal?
            //Current answer: not of interested
            onVisibility(WIDGET_VISIBLE_RESTORE);
          }
        }

        RectI wr;
        RectI cr;
        getWindowRect(&wr, &cr);
        onGeometry(wr, cr);
        return 0;
      }

      case WM_ENABLE:
        onEnabled(wParam==TRUE);
        return 0;
      case WM_SETFOCUS:
        onFocus(true);
        return 0;
      case WM_KILLFOCUS:
        onFocus(false);
        return 0;
      case WM_ERASEBKGND:
       return 1;

      case WM_PAINT:
      {
        RECT rect;
        PAINTSTRUCT ps;
        HDC hdc;

        GetClientRect(hwnd, &rect);
        hdc = BeginPaint(hwnd, &ps);

        if (!_isDirty)
        {
          BitBlt(
            hdc, 0, 0, rect.right-rect.left, rect.bottom-rect.top,
            reinterpret_cast<WinGuiBackBuffer*>(_backingStore)->getHdc(), 0, 0,
            SRCCOPY);
        }
        else
        {
          _needBlit = true;
        }

        EndPaint(hwnd, &ps);
        return 0;
      }

      case WM_CLOSE:
      {
        CloseEvent e;
        _widget->sendEvent(&e);
        return 0;
      }

      case WM_DESTROY:
        return 0;

      default:
        if (message == GUI_ENGINE()->uMSH_MOUSEWHEEL)
        {
          if ((int)wParam < 0)
          {
            onMouseWheel(WHEEL_DOWN);
            return 0;
          }
          else
          {
            onMouseWheel(WHEEL_UP);
            return 0;
          }
        }
        break;
    }
  }

defWindowProc:
  return DefWindowProc(hwnd, message, wParam, lParam);
}

void WinGuiWindow::getWindowRect(RectI* windowRect, RectI* clientRect)
{
  RECT wr;
  RECT cr;

  ::GetWindowRect((HWND)_handle, &wr);
  ::GetClientRect((HWND)_handle, &cr);

  windowRect->setBox(wr.left, wr.top, wr.right, wr.bottom);
  clientRect->setBox(cr.left, cr.top, cr.right, cr.bottom);
}

// ============================================================================
// [Fog::WinGuiBackBuffer]
// ============================================================================

WinGuiBackBuffer::WinGuiBackBuffer(bool b) : _prgb(b)
{
}

WinGuiBackBuffer::~WinGuiBackBuffer()
{
  destroy();
}

bool WinGuiBackBuffer::resize(int width, int height, bool cache)
{
  int targetWidth = width;
  int targetHeight = height;

  bool destroyImage = false;
  bool createImage = false;

  if (width == 0 || height == 0)
  {
    destroyImage = true;
  }
  else
  {
    if (cache)
    {
      if (width <= _cachedSize.w && height <= _cachedSize.h)
      {
        // Cached.
        _buffer.size.w = width;
        _buffer.size.h = height;
        return true;
      }

      // Don't create smaller buffer that previous!
      targetWidth  = Math::max<int>(width, _cachedSize.w);
      targetHeight = Math::max<int>(height, _cachedSize.h);

      // Cache using 128x128 blocks.
      targetWidth  = (targetWidth  + 127) & ~127;
      targetHeight = (targetHeight + 127) & ~127;
    }

    destroyImage = true;
    createImage = (targetWidth > 0 && targetHeight > 0);
  }

  // Destroy image buffer
  if (destroyImage)
  {
    switch (_type)
    {
      case TYPE_NONE:
        break;

      case TYPE_WIN_DIB:
        DeleteDC(_hdc);
        DeleteObject(_hBitmap);
        break;
    }
  }

  // Create image buffer.
  if (createImage)
  {
    int targetBPP = 32;

    // Define bitmap attributes.
    BITMAPINFO bmi;

    _hdc = CreateCompatibleDC(NULL);
    if (_hdc == NULL)
    {
      Debug::dbgFunc("Fog::WinGuiBackBuffer", "resize", "CreateCompatibleDC() failed, WinError=%u.\n", GetLastError());
      goto _Fail;
    }

    Fog::MemOps::zero(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth       = targetWidth;
    bmi.bmiHeader.biHeight      = -targetHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = targetBPP;
    bmi.bmiHeader.biCompression = BI_RGB;

    _primaryPixels = NULL;
    _hBitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&_primaryPixels, NULL, 0);
    if (_hBitmap == NULL)
    {
      Debug::dbgFunc("Fog::WinGuiBackBuffer", "resize", "CreateDIBSection() failed, request size=%dx%d, WinError=%u.\n", targetWidth, targetHeight, GetLastError());
      DeleteDC(_hdc);
      goto _Fail;
    }

    // Select bitmap to DC.
    SelectObject(_hdc, _hBitmap);

    // We need stride, Windows should use 32-bit alignment, but we should
    // be safe and check for stride used by Windows.
    DIBSECTION info;
    GetObjectW(_hBitmap, sizeof(DIBSECTION), &info);

    _type = TYPE_WIN_DIB;
    _primaryStride = info.dsBm.bmWidthBytes;

    _buffer.data = _primaryPixels;
    _buffer.size.set(width, height);
    _buffer.format = _prgb ? IMAGE_FORMAT_PRGB32 : IMAGE_FORMAT_XRGB32;
    _buffer.stride = _primaryStride;

    _cachedSize.set(targetWidth, targetHeight);

    // Secondary buffer not used by this platform.
    _secondaryPixels = NULL;
    _secondaryStride = 0;

    _createdTime = TimeTicks::now();
    if (_prgb)
    {
      _expireTime = _createdTime + TimeDelta::fromDays(1);
    }
    else
    {
      _expireTime = _createdTime + TimeDelta::fromSeconds(15);
    }

    _convertFunc = NULL;
    _convertDepth = 0;

    return true;
  }

_Fail:
  _clear();
  return false;
}

void WinGuiBackBuffer::destroy()
{
  resize(0, 0, false);
}

void WinGuiBackBuffer::updateRects(const BoxI* rects, size_t count)
{
  // There is nothing to do (this is mainly for X11).
}

void WinGuiBackBuffer::blitRects(HDC target, const BoxI* rects, size_t count)
{
  size_t i;

  switch (getType())
  {
    case TYPE_NONE:
      break;

    case TYPE_WIN_DIB:
      for (i = 0; i != count; i++)
      {
        int x = rects[i].getX();
        int y = rects[i].getY();
        int w = rects[i].getWidth();
        int h = rects[i].getHeight();

        BitBlt(target, x, y, w, h, _hdc, x, y, SRCCOPY);
      }
      break;
  }
}

} // Fog namespace

// [Guard]
#endif // FOG_GUI_WINDOWS
