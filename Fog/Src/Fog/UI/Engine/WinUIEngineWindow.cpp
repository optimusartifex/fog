// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/UI/Engine/UIEngineEvent.h>
#include <Fog/UI/Engine/WinUIEngine.h>
#include <Fog/UI/Engine/WinUIEngineWindow.h>

namespace Fog {

#define WM_FOG_UPDATE_GEOMETRY WM_USER

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Helpers]
// ============================================================================

static void WinUIEngineWindow_getGeometry(WinUIEngineWindowImpl* wImpl, RectI* windowGeometry, RectI* clientGeometry)
{
  RECT wr;
  RECT cr;

  GetWindowRect(static_cast<HWND>(wImpl->_handle), &wr);
  GetClientRect(static_cast<HWND>(wImpl->_handle), &cr);

  windowGeometry->setRect(wr.left, wr.top, wr.right - wr.left, wr.bottom - wr.top);
  clientGeometry->setRect(cr.left, cr.top, cr.right - cr.left, cr.bottom - cr.top);
}

static uint32_t WinUIEngineWindow_getHintsFromWindow(WinUIEngineWindowImpl* wImpl)
{
  switch (wImpl->_windowType)
  {
    case WINDOW_TYPE_NORMAL:
      return WINDOW_HINT_HAS_SYSTEM_MENU | 
             WINDOW_HINT_HAS_MINIMIZE    |
             WINDOW_HINT_HAS_MAXIMIZE    |
             WINDOW_HINT_HAS_CLOSE       ;

    case WINDOW_TYPE_DIALOG:
      return WINDOW_HINT_HAS_SYSTEM_MENU |
             WINDOW_HINT_HAS_CLOSE       ;

    case WINDOW_TYPE_TOOL:
      return WINDOW_HINT_HAS_SYSTEM_MENU |
             WINDOW_HINT_HAS_CLOSE       ;

    case WINDOW_TYPE_POPUP:
    case WINDOW_TYPE_TOOLTIP:
    case WINDOW_TYPE_SPLASH:
    case WINDOW_TYPE_FRAMELESS:
    default:
      return WINDOW_HINT_FRAMELESS;
  }
}

static void WinUIEngineWindow_getStyleFlags(WinUIEngineWindowImpl* wImpl, DWORD* pStyle, DWORD* pStyleEx)
{
  uint32_t style = 0;
  uint32_t styleEx = 0;

  switch (wImpl->_windowType)
  {
    case WINDOW_TYPE_NORMAL:
      style   = WS_OVERLAPPED | WS_CAPTION;
      styleEx = WS_EX_WINDOWEDGE;
      break;

    case WINDOW_TYPE_DIALOG:
      style   = WS_OVERLAPPED | WS_CAPTION | WS_DLGFRAME;
      styleEx = WS_EX_TOPMOST | WS_EX_DLGMODALFRAME;
      break;

    case WINDOW_TYPE_POPUP:
      style   = WS_POPUP;
      styleEx = WS_EX_TOPMOST | WS_EX_APPWINDOW;
      break;

    case WINDOW_TYPE_TOOL:
      style   = WS_OVERLAPPED | WS_CAPTION;
      styleEx = WS_EX_TOOLWINDOW;
      break;

    case WINDOW_TYPE_TOOLTIP:
    case WINDOW_TYPE_SPLASH:
    case WINDOW_TYPE_FRAMELESS:
      style   = WS_POPUP;
      styleEx = WS_EX_APPWINDOW;
      break;
  }

  if (!wImpl->_isWindowOpaque)
    styleEx |= WS_EX_LAYERED;

  if (wImpl->_windowHints & WINDOW_HINT_HAS_SYSTEM_MENU)
  {
    style |= WS_SYSMENU;

    // The Close Button is only available if the system menu is set.
    if (!(wImpl->_windowHints & WINDOW_HINT_HAS_CLOSE))
    {
      // We will disable the entry within SystemMenu on our own later.
      style |= CS_NOCLOSE;
    }
  }

  if (!(wImpl->_windowHints & WINDOW_HINT_FRAMELESS))
  {
    if (wImpl->_windowHints & WINDOW_HINT_FIXED_SIZE)
      style |= WS_BORDER;
    else
      style |= WS_THICKFRAME;

    // A Context-Help Button is only visible if no min/max button are defined.
    if (wImpl->_windowHints & WINDOW_HINT_HAS_CONTEXT_HELP)
    {
      styleEx |= WS_EX_CONTEXTHELP;
    }
    else
    {
      // We will disable the entry within SystemMenu on our own later.
      if (wImpl->_windowHints & WINDOW_HINT_HAS_MINIMIZE)
        style |= WS_MINIMIZEBOX;

      // We will disable the entry within SystemMenu on our own later.
      if (wImpl->_windowHints & WINDOW_HINT_HAS_MAXIMIZE)
        style |= WS_MAXIMIZEBOX;
    }
  }

  if (wImpl->_windowHints & WINDOW_HINT_ALWAYS_ON_TOP)
    styleEx |= WS_EX_TOPMOST;

  *pStyle = style;
  *pStyleEx = styleEx;
}

static void WinUIEngineWindow_setWindowMenu(WinUIEngineWindowImpl* wImpl)
{
  static const uint32_t menuState[2] =
  {
    MF_GRAYED,
    MF_ENABLED
  };

  // Do it here, so we don't need to do it in INIT_MENU everytime
  HMENU hMenu = ::GetSystemMenu(static_cast<HWND>(wImpl->_handle), FALSE);

  if (hMenu == INVALID_HANDLE_VALUE)
    return;

  ::EnableMenuItem(hMenu, SC_SIZE    , menuState[(wImpl->_windowHints & WINDOW_HINT_FIXED_SIZE  ) == 0]);
  ::EnableMenuItem(hMenu, SC_MOVE    , menuState[(wImpl->_windowHints & WINDOW_HINT_FIXED_POS   ) == 0]);
  ::EnableMenuItem(hMenu, SC_MINIMIZE, menuState[(wImpl->_windowHints & WINDOW_HINT_HAS_MINIMIZE) != 0]);
  ::EnableMenuItem(hMenu, SC_MAXIMIZE, menuState[(wImpl->_windowHints & WINDOW_HINT_HAS_MAXIMIZE) != 0]);
  ::EnableMenuItem(hMenu, SC_CLOSE   , menuState[(wImpl->_windowHints & WINDOW_HINT_HAS_CLOSE   ) != 0]);
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Construction / Destruction]
// ============================================================================

WinUIEngineWindowImpl::WinUIEngineWindowImpl(UIEngine* engine, UIEngineWindow* window) :
  UIEngineWindowImpl(engine, window)
{
}

WinUIEngineWindowImpl::~WinUIEngineWindowImpl()
{
  freeDoubleBuffer();
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Create / Destroy]
// ============================================================================

err_t WinUIEngineWindowImpl::create(uint32_t hints)
{
  // We need the original value of these two.
  bool isVisible = _isVisible;
  bool resetHints = _resetHints;

  if (resetHints)
    _windowHints = WinUIEngineWindow_getHintsFromWindow(this);

  DWORD dwStyle, dwStyleEx;
  WinUIEngineWindow_getStyleFlags(this, &dwStyle, &dwStyleEx);

  if (_handle != NULL)
  {
    // Just update window with new styles.
    DWORD style   = ::GetWindowLongW(static_cast<HWND>(_handle), GWL_STYLE);
    DWORD styleEx = ::GetWindowLongW(static_cast<HWND>(_handle), GWL_EXSTYLE);

    // Prevent flicker in case that the Window is visible on the screen. We 
    // disable all redrawing events and then enable them at the end of this
    // function.
    if (isVisible)
    {
      ::SendMessageW(static_cast<HWND>(_handle), WM_SETREDRAW, (WPARAM)FALSE, (LPARAM)0);
    }

    if ((_windowHints & WINDOW_HINT_COMPOSITE) != (hints & WINDOW_HINT_COMPOSITE))
    {
      _windowHints ^= WINDOW_HINT_COMPOSITE;

      if (hints & WINDOW_HINT_COMPOSITE)
      {
        ::SetWindowLongW(static_cast<HWND>(_handle), GWL_EXSTYLE, styleEx & ~WS_EX_LAYERED);
        dwStyleEx |= WS_EX_LAYERED;
      }
    }

    // It's much easier to first remove all possible flags and then create
    // a complete new flag and or it with the clean old one.
    style &=~ (WS_OVERLAPPED | WS_CAPTION | WS_POPUP | WS_SYSMENU | WS_DLGFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | CS_NOCLOSE | WS_THICKFRAME | WS_BORDER);

    ::SetWindowLongW(static_cast<HWND>(_handle), GWL_STYLE  , dwStyle | style);
    ::SetWindowLongW(static_cast<HWND>(_handle), GWL_EXSTYLE, dwStyleEx);

    WinUIEngineWindow_setWindowMenu(this);
    // getWidget()->setTransparency(getWidget()->getTransparency());

    // Enable redrawing and schedule redraw.
    if (isVisible)
    {
      // Allow repaint again!
      ::SendMessageW(static_cast<HWND>(_handle), WM_SETREDRAW, (WPARAM) TRUE, (LPARAM) 0);
      ::UpdateWindow(static_cast<HWND>(_handle));

      ::SetWindowPos(static_cast<HWND>(_handle), 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
      ::RedrawWindow(static_cast<HWND>(_handle), 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME );
    }

    return ERR_OK;
  }
  else
  {
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    const WCHAR* wndClass = L"Fog_Window";

    // TODO:
    // bool b = (flags & WINDOW_FRAMELESS) != 0 || (flags & WINDOW_POPUP) != 0 || (flags & WINDOW_FULLSCREEN) != 0;

    if (_windowHints & WINDOW_HINT_COMPOSITE)
    {
      dwStyleEx |= WS_EX_LAYERED;
    }

    _handle = (void*)::CreateWindowExW(
      dwStyleEx, wndClass, L"",
      dwStyle, x, y,
      1, 1,
      NULL, NULL, (HINSTANCE)GetModuleHandleW(NULL), NULL);

    if (FOG_IS_NULL(_handle))
    {
      DWORD winError = GetLastError();

      Logger::error("Fog::WinUIEngineWindowImpl", "create",
        "CreateWindowExW() failed (WinError=%u).", winError);
      return OSUtil::getErrFromOSErrorCode(winError);
    }

    // Create HWND <-> GuiWindow* connection.
    _engine->addHandle(_handle, this);

    // Windows are enabled by default.
    _isEnabled = true;
    // Windows are not visible by default.
    _isVisible = false;
    // Default focus is no focus, Windows will inform us if this gets changed.
    _hasFocus = false;
    // This window should be updated (after update the shouldBlit flag should 
    // be set too).
    _shouldUpdate = true;
    // Will be set if window state would be changed.
    _shouldBlit = false;

    // Update window menu and window/client rectangle.
    WinUIEngineWindow_setWindowMenu(this);
    WinUIEngineWindow_getGeometry(this, &_windowGeometry, &_clientGeometry);

    return ERR_OK;
  }
}

err_t WinUIEngineWindowImpl::destroy()
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  _windowBeingDestroyed = true;
  ::DestroyWindow(static_cast<HWND>(_handle));

  destroyed();
  return ERR_OK;
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Enabled / Disabled]
// ============================================================================

err_t WinUIEngineWindowImpl::setEnabled(bool enabled)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  ::EnableWindow(static_cast<HWND>(_handle), enabled);
  return ERR_OK;
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Focus]
// ============================================================================

err_t WinUIEngineWindowImpl::focus()
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  if (::SetFocus(static_cast<HWND>(_handle)) == NULL)
    return OSUtil::getErrFromOSLastError();

  return ERR_OK;
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Window State]
// ============================================================================

err_t WinUIEngineWindowImpl::setState(uint32_t state)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  switch (state)
  {
    case WINDOW_STATE_HIDDEN:
      ::ShowWindow(static_cast<HWND>(_handle), SW_HIDE);
      return ERR_OK;

    case WINDOW_STATE_NORMAL:
      if (!::IsWindowVisible(static_cast<HWND>(_handle)))
      {
        ::ShowWindow(static_cast<HWND>(_handle), SW_SHOW);
      }
      return ERR_OK;

    case WINDOW_STATE_MAXIMIZED:
      if (!::IsZoomed(static_cast<HWND>(_handle)))
      {
        bool updatePosition = !::IsWindowVisible(static_cast<HWND>(_handle));
        ::ShowWindow(static_cast<HWND>(_handle), SW_SHOWMAXIMIZED);

        if (updatePosition)
          ::SendMessageW(static_cast<HWND>(_handle), WM_FOG_UPDATE_GEOMETRY, 0, 0);
      }
      return ERR_OK;

    case WINDOW_STATE_FULLSCREEN:
      if (!::IsWindowVisible(static_cast<HWND>(_handle)))
      {
        ::ShowWindow(static_cast<HWND>(_handle), SW_SHOW);
      }
      return ERR_OK;

    default:
      return ERR_RT_INVALID_STATE;
  }
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Window Geometry]
// ============================================================================

err_t WinUIEngineWindowImpl::setWindowPosition(const PointI& pos)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  if (_windowGeometry.x == pos.x && _windowGeometry.y == pos.y)
    return ERR_OK;

  if (!::MoveWindow(static_cast<HWND>(_handle),
    pos.x, pos.y, _windowGeometry.w, _windowGeometry.h, FALSE))
  {
    return OSUtil::getErrFromOSLastError();
  }

  return ERR_OK;
}

err_t WinUIEngineWindowImpl::setWindowSize(const SizeI& size)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  if (_windowGeometry.w == size.w && _windowGeometry.h == size.h)
    return ERR_OK;

  if (!::MoveWindow(static_cast<HWND>(_handle),
    _windowGeometry.x, _windowGeometry.y, size.w, size.h, TRUE))
  {
    return OSUtil::getErrFromOSLastError();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Window Stack]
// ============================================================================

static void WinUIEngineWindowImpl_fixAlwaysOnTop(WinUIEngineWindowImpl* wImpl)
{
  if ((wImpl->_windowHints & WINDOW_HINT_ALWAYS_ON_TOP) == 0)
    return;

  DWORD styleEx = ::GetWindowLongW(static_cast<HWND>(wImpl->_handle), GWL_EXSTYLE);
  styleEx |= WS_EX_TOPMOST;
  ::SetWindowLongW(static_cast<HWND>(wImpl->_handle), GWL_EXSTYLE, styleEx);
}

err_t WinUIEngineWindowImpl::moveToTop(void* targetHandle)
{
  HWND targetHwnd;

  if (targetHandle != NULL)
    targetHwnd = static_cast<HWND>(targetHandle);
  else
    targetHwnd = HWND_TOP;

  ::SetWindowPos(static_cast<HWND>(_handle), targetHwnd, 0, 0, 0, 0,
    SWP_NOMOVE | SWP_NOACTIVATE |
    SWP_NOSIZE | SWP_NOCOPYBITS | SWP_NOREDRAW);

  // Make sure that the ALWAYS_ON_TOP flag is valid.
  WinUIEngineWindowImpl_fixAlwaysOnTop(this);

  return ERR_OK;
}

err_t WinUIEngineWindowImpl::moveToBottom(void* targetHandle)
{
  HWND targetHwnd;

  if (targetHandle != NULL)
    targetHwnd = static_cast<HWND>(targetHandle);
  else
    targetHwnd = HWND_BOTTOM;

  ::SetWindowPos(static_cast<HWND>(_handle), targetHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

  // Make sure that the ALWAYS_ON_TOP flag is valid.
  WinUIEngineWindowImpl_fixAlwaysOnTop(this);

  return ERR_OK;
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Window Coordinates]
// ============================================================================

err_t WinUIEngineWindowImpl::worldToClient(PointI& pt) const
{
  FOG_ASSERT(sizeof(PointI) == sizeof(POINT));

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  if (!::ScreenToClient(static_cast<HWND>(_handle), (POINT *)&pt))
    return OSUtil::getErrFromOSLastError();

  return ERR_OK;
}

err_t WinUIEngineWindowImpl::clientToWorld(PointI& pt) const
{
  FOG_ASSERT(sizeof(PointI) == sizeof(POINT));

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  if (!::ClientToScreen(static_cast<HWND>(_handle), (POINT *)&pt))
    return OSUtil::getErrFromOSLastError();

  return ERR_OK;
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Window Opacity]
// ============================================================================

err_t WinUIEngineWindowImpl::setWindowOpacity(float opacity)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_HANDLE;

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Window Title]
// ============================================================================

err_t WinUIEngineWindowImpl::setWindowTitle(const StringW& title)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_HANDLE;

  if (!::SetWindowTextW(static_cast<HWND>(_handle), reinterpret_cast<const wchar_t*>(title.getData())))
    return OSUtil::getErrFromOSLastError();

  _windowTitle = title;
  return ERR_OK;
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - Window Frame-Buffer]
// ============================================================================

err_t WinUIEngineWindowImpl::allocDoubleBuffer(const SizeI& size)
{
  uint32_t format = (_windowHints & WINDOW_HINT_COMPOSITE)
    ? IMAGE_FORMAT_PRGB32
    : IMAGE_FORMAT_XRGB32;

  FOG_RETURN_ON_ERROR(_bufferImage.create(size, format, IMAGE_TYPE_WIN_DIB));

  _bufferData.setData(
    _bufferImage.getSize(),
    _bufferImage.getFormat(),
    _bufferImage.getStride(),
    _bufferImage.getDataX());
  _bufferType = UI_ENGINE_BUFFER_WIN_DIB;

  return ERR_OK;
}

err_t WinUIEngineWindowImpl::freeDoubleBuffer()
{
  _bufferImage.reset();

  _bufferData.reset();
  _bufferType = UI_ENGINE_BUFFER_NONE;

  _bufferCacheSize.reset();
  _bufferCacheCreated.reset();
  _bufferCacheExpire.reset();

  return ERR_OK;
}

// ============================================================================
// [Fog::WinUIEngineWindowImpl - OnWinMsg]
// ============================================================================

LRESULT WinUIEngineWindowImpl::onWinMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_CREATE:
    {
      _engine->doCreateAction(_window);
      goto _DefWindowProc;
    }

    // Should be never catched.
    case WM_DESTROY:
    {
      return 0;
    }

    case WM_SETFOCUS:
    {
      _engine->doFocusAction(_window, UI_ENGINE_EVENT_FOCUS_IN);
      return 0;
    }

    case WM_KILLFOCUS:
    {
      _engine->doFocusAction(_window, UI_ENGINE_EVENT_FOCUS_OUT);
      return 0;
    }

    case WM_ENABLE:
    {
      if (wParam == TRUE)
        _engine->doEnableAction(_window, UI_ENGINE_EVENT_ENABLE);
      else
        _engine->doEnableAction(_window, UI_ENGINE_EVENT_DISABLE);
      return 0;
    }

    case WM_CLOSE:
    {
      _engine->doCloseAction(_window);
      return 0;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
      uint32_t key = reinterpret_cast<WinUIEngine*>(_engine)->getFogKeyFromWinKey(wParam, HIWORD(lParam));
      uint32_t mod = reinterpret_cast<WinUIEngine*>(_engine)->getModifierFromKey(key);
      uint32_t uc  = reinterpret_cast<WinUIEngine*>(_engine)->getUnicodeFromWinKey(wParam, HIWORD(lParam));

      uint32_t flags = _engine->doKeyAction(_window,
        UI_ENGINE_EVENT_KEY_DOWN, 0, key, mod, (uint32_t)wParam, uc);

      // Prevent default action if event was accepted.
      if ((flags & UI_ENGINE_EVENT_FLAG_ACCEPT) != 0)
        return 0;

      break;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
      uint32_t key = reinterpret_cast<WinUIEngine*>(_engine)->getFogKeyFromWinKey(wParam, HIWORD(lParam));
      uint32_t mod = reinterpret_cast<WinUIEngine*>(_engine)->getModifierFromKey(key);
      uint32_t uc  = reinterpret_cast<WinUIEngine*>(_engine)->getUnicodeFromWinKey(wParam, HIWORD(lParam));

      uint32_t flags = _engine->doKeyAction(_window,
        UI_ENGINE_EVENT_KEY_UP, 0, key, mod, (uint32_t)wParam, uc);

      if ((flags & UI_ENGINE_EVENT_FLAG_ACCEPT) != 0)
        return 0;
      break;
    }

    case WM_MOUSEMOVE:
    case WM_MOUSEHOVER:
    {
      uint32_t mId = 0;
      UIEngineMouseState* mState = &_engine->_mouseState[mId];

      // Mouse tracking for WM_MOUSELEAVE message.
      TRACKMOUSEEVENT tme;

      tme.cbSize    = sizeof(TRACKMOUSEEVENT);
      tme.dwFlags   = TME_LEAVE;
      tme.hwndTrack = static_cast<HWND>(_handle);
      TrackMouseEvent(&tme);

      int x = (int)(short)LOWORD(lParam);
      int y = (int)(short)HIWORD(lParam);

      // WM_MOUSEHOVER message is sometimes not sent, thus we joined both, 
      // WM_MOUSEMOVE and WM_MOUSEHOVER into a single block and we decide
      // which one to send onto the window.
      if (mState->getWindow() != _window || mState->getHover() == 0)
        _engine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_IN, mId, PointI(x, y), BUTTON_NONE, mState->getButtonMask());
      else
        _engine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_MOVE, mId, PointI(x, y), BUTTON_NONE, mState->getButtonMask());

      return 0;
    }

    case WM_MOUSELEAVE:
    {
      uint32_t mId = 0;
      UIEngineMouseState* mState = &_engine->_mouseState[mId];

      if (mState->getWindow() == _window && mState->getButtonMask() == 0)
        _engine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_OUT, mId, PointI(INT_MIN, INT_MIN), BUTTON_NONE, mState->getButtonMask());
      return 0;
    }

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    {
      uint32_t mId = 0;
      UIEngineMouseState* mState = &_engine->_mouseState[mId];

      int x = (int)(short)LOWORD(lParam);
      int y = (int)(short)HIWORD(lParam);
      uint32_t buttonCode = 0;

      switch (msg)
      {
        case WM_LBUTTONDOWN:
          buttonCode = BUTTON_LEFT;
          break;
        case WM_RBUTTONDOWN:
          buttonCode = BUTTON_RIGHT;
          break;
        case WM_MBUTTONDOWN:
          buttonCode = BUTTON_MIDDLE;
          break;
        default:
          buttonCode = (HIWORD(wParam) == XBUTTON1) ? BUTTON_X1 : BUTTON_X2;
          break;
      }

      _engine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_DOWN, 0, PointI(x, y), buttonCode, mState->getButtonMask());
      return 0;
    }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
      uint32_t mId = 0;
      UIEngineMouseState* mState = &_engine->_mouseState[mId];

      int x = (int)(short)LOWORD(lParam);
      int y = (int)(short)HIWORD(lParam);
      uint32_t buttonCode = 0;

      switch (msg)
      {
        case WM_LBUTTONUP:
          buttonCode = BUTTON_LEFT;
          break;
        case WM_RBUTTONUP:
          buttonCode = BUTTON_RIGHT;
          break;
        case WM_MBUTTONUP:
          buttonCode = BUTTON_MIDDLE;
          break;
        default:
          buttonCode = (HIWORD(wParam) == XBUTTON1) ? BUTTON_X1 : BUTTON_X2;
          break;
      }

      _engine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_UP, 0, PointI(x, y), buttonCode, mState->getButtonMask());
      return 0;
    }

    case WM_MOUSEWHEEL:
    {
      uint32_t mId = 0;
      UIEngineMouseState* mState = &_engine->_mouseState[mId];

      int x = (int)(short)LOWORD(lParam);
      int y = (int)(short)HIWORD(lParam);

      // Typical for synaptics.
      if (mState->getWindow() != _window || mState->getHover() == 0)
      {
        POINT pt;
        DWORD cursorPos = GetMessagePos();

        pt.x = (int)(short)LOWORD(cursorPos);
        pt.y = (int)(short)HIWORD(cursorPos);

        ::ScreenToClient(static_cast<HWND>(_handle), &pt);

        x = pt.x;
        y = pt.y;

        _engine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_IN, mId, PointI(x, y), BUTTON_NONE, mState->getButtonMask());
      }

      if (((int16_t)HIWORD(wParam)) >= 0)
        _engine->doMouseAction(_window, UI_ENGINE_EVENT_WHEEL_UP, mId, PointI(x, y), BUTTON_WHEEL_UP, mState->getButtonMask());
      else
        _engine->doMouseAction(_window, UI_ENGINE_EVENT_WHEEL_UP, mId, PointI(x, y), BUTTON_WHEEL_DOWN, mState->getButtonMask());
      return 0;
    }

    case WM_SHOWWINDOW:
      goto _DefWindowProc;

    case WM_SYSCOMMAND:
    {
      int cmd = int(wParam) & 0xFFF0;

      switch (cmd)
      {
        case SC_MOVE:
          // Do not allow to move if window has FIXED_POS.
          if ((_windowHints & WINDOW_HINT_FIXED_POS) != 0)
            return 0;
          break;
      }

      goto _DefWindowProc;
    }

    // If we handle WM_WINDOWPOSCHANGED and return 0, WM_MOVE, WM_SIZE, and
    // WM_SHOWWINDOW messages won't be sent. This is something what we want
    // to improve performance and to stay simple (handling all geometry at 
    // one place).
    case WM_WINDOWPOSCHANGED:
    {
      WINDOWPOS *pos = (WINDOWPOS*)lParam;

      if ((pos->flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW | SWP_FRAMECHANGED)) != 0)
      {
        if (pos->flags & SWP_SHOWWINDOW)
        {
          _engine->doStateAction(_window, UI_ENGINE_EVENT_SHOW, WINDOW_STATE_NORMAL);
          return 0;
        }

        if (pos->flags & SWP_HIDEWINDOW)
        {
          _engine->doStateAction(_window, UI_ENGINE_EVENT_HIDE, WINDOW_STATE_HIDDEN);
          return 0;
        }

        if (pos->flags & SWP_FRAMECHANGED)
        {
          // Window minimized.
          if (::IsIconic(static_cast<HWND>(_handle)))
          {
            _engine->doStateAction(_window, UI_ENGINE_EVENT_HIDE, WINDOW_STATE_HIDDEN);
            return 0;
          }

          // Window maximized.
          if (::IsZoomed(static_cast<HWND>(_handle)))
          {
            _engine->doStateAction(_window, UI_ENGINE_EVENT_SHOW, WINDOW_STATE_MAXIMIZED);
            goto _UpdateGeometry;
          }
        }
      }

      if ((pos->flags & (SWP_NOMOVE | SWP_NOSIZE)) == 0 &&
          (pos->flags & (SWP_FRAMECHANGED       )) != 0)
      {
        _engine->doStateAction(_window, UI_ENGINE_EVENT_SHOW, WINDOW_STATE_NORMAL);
      }

      goto _UpdateGeometry;
    }
    
    case WM_PAINT:
    {
      if (!_bufferImage.isEmpty() && !_isDirty)
      {
        PAINTSTRUCT ps;
        const RECT& rp = ps.rcPaint;

        HDC winHdc = ::BeginPaint(static_cast<HWND>(_handle), &ps);
        HDC imgHdc;

        // This really shouldn't fail. If it does the Windows is running out of
        // resources, which could mean that there is a HDC leak.
        if (_bufferImage.getDC(&imgHdc) == ERR_OK)
        {
          ::BitBlt(winHdc, rp.left, rp.top, rp.right - rp.left, rp.bottom - rp.top, imgHdc, rp.left, rp.top, SRCCOPY);
          _bufferImage.releaseDC(imgHdc);
        }

        ::EndPaint(static_cast<HWND>(_handle), &ps);
      }
      else
      {
        RECT cr;
        ::GetClientRect(static_cast<HWND>(_handle), &cr);

        _blitRegion.union_(BoxI(0, 0, cr.right - cr.left, cr.bottom - cr.top));
        _shouldBlit = true;
      }

      return 0;
    }

    // Always ignore this message, because of flickering. Since we can paint 
    // everything ourselves, we never need to erase window background by OS.
    case WM_ERASEBKGND:
      return 1;

    // Message is sent to create a onGeometryAction on maximized application
    // start.
    case WM_FOG_UPDATE_GEOMETRY:
_UpdateGeometry:
    {
      RectI wr, cr;
      WinUIEngineWindow_getGeometry(this, &wr, &cr);

      _engine->doGeometryAction(_window, UI_ENGINE_EVENT_GEOMETRY, ORIENTATION_HORIZONTAL, wr, cr);
      return 0;
    }

    default:
      if (msg == reinterpret_cast<WinUIEngine*>(_engine)->_uMSH_MOUSEWHEEL)
      {
        uint32_t mId = 0;
        UIEngineMouseState* mState = &_engine->_mouseState[mId];

        int x = (int)(short)LOWORD(lParam);
        int y = (int)(short)HIWORD(lParam);

        if ((int)wParam >= 0)
          _engine->doMouseAction(_window, UI_ENGINE_EVENT_WHEEL_UP, mId, PointI(x, y), BUTTON_WHEEL_UP, mState->getButtonMask());
        else
          _engine->doMouseAction(_window, UI_ENGINE_EVENT_WHEEL_UP, mId, PointI(x, y), BUTTON_WHEEL_DOWN, mState->getButtonMask());
        return 0;
      }
      break;
  }

_DefWindowProc:
  return DefWindowProcW(static_cast<HWND>(_handle), msg, wParam, lParam);
}

} // Fog namespace





















































#if 0
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
      LONG flag = ::GetWindowLongW((HWND)getHandle(),GWL_EXSTYLE);
      if ((flag & WS_EX_LAYERED) != 0)
      {
        flag &=~WS_EX_LAYERED;
        ::SetWindowLongW((HWND)getHandle(),GWL_EXSTYLE,flag);
      }
    }
    else
    {
      // Make sure window flag is set!
      LONG flag = ::GetWindowLongW((HWND)getHandle(),GWL_EXSTYLE);
      if ((flag & WS_EX_LAYERED) == 0)
      {
        ::SetWindowLongW((HWND)getHandle(), GWL_EXSTYLE, flag | WS_EX_LAYERED);
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

    LONG flag = ::GetWindowLongW((HWND)getHandle(), GWL_EXSTYLE);
    if ((flag & WS_EX_LAYERED) == 0)
    {
      ::SetWindowLongW((HWND)getHandle(), GWL_EXSTYLE, flag | WS_EX_LAYERED);
    }

    bool ret = ::UpdateLayeredWindow((HWND)getHandle(), NULL, NULL, NULL, NULL, NULL, (COLORREF)NULL, &blend, ULW_ALPHA);
    if (!ret)
    {
      int e = GetLastError();
      ret = ret;
    }
  }
}


err_t WinGuiWindow::setSize(const SizeI& size)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;
  if ((size.getWidth() <= 0) | (size.getHeight() <= 0)) return ERR_RT_INVALID_ARGUMENT;

  if (_windowRect.getSize() != size)
  {
    ::MoveWindow((HWND)_handle,
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
    ::MoveWindow((HWND)_handle,
      geometry.getX(),
      geometry.getY(),
      geometry.getWidth(),
      geometry.getHeight(),
      TRUE);
  }

  return ERR_OK;
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

void WinGuiWindow::onMousePress(uint32_t button, bool repeated)
{
  WinUIEngine* guiEngine = GUI_ENGINE();

  if (guiEngine->_systemMouseStatus.uiWindow != this)
    return;

  if (guiEngine->_systemMouseStatus.buttons == 0 && !repeated)
  {
    SetCapture((HWND)_handle);
  }

  base::onMousePress(button, repeated);
}

void WinGuiWindow::onMouseRelease(uint32_t button)
{
  WinUIEngine* guiEngine = GUI_ENGINE();

  if (guiEngine->_systemMouseStatus.uiWindow != this)
    return;

  if ((guiEngine->_systemMouseStatus.buttons & ~button) == 0)
  {
    ReleaseCapture();
  }

  base::onMouseRelease(button);
}
#endif
