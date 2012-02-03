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
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/UI/Engine/UIEngineEvent.h>
#include <Fog/UI/Engine/X11UIEngine.h>
#include <Fog/UI/Engine/X11UIEngineWindow.h>
#include <Fog/UI/Engine/X11UIEngineXlib.h>

// [Dependencies - C -Shared memory and IPC]
#include <errno.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/utsname.h>
#include <unistd.h>

namespace Fog {

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Construction / Destruction]
// ============================================================================

X11UIEngineWindowImpl::X11UIEngineWindowImpl(UIEngine* engine, UIEngineWindow* window) :
  UIEngineWindowImpl(engine, window)
{
  _xic = NULL;
  _bufferPtr = NULL;
  MemOps::zero(&_shmInfo, sizeof(_shmInfo));
  _pixmap = 0;
  _ximage = NULL;

  _isInputOnly = false;
  _isConfigured = false;
  _mapRequest = false;
}

X11UIEngineWindowImpl::~X11UIEngineWindowImpl()
{
  freeDoubleBuffer();
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Create / Destroy]
// ============================================================================

err_t X11UIEngineWindowImpl::create(uint32_t hints)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (_handle != NULL)
    return ERR_OK;

  int x = _windowGeometry.x;
  int y = _windowGeometry.y;
  int w = _windowGeometry.w;
  int h = _windowGeometry.h;

  w += (w == 0);
  h += (h == 0);

  XSetWindowAttributes attr;
  _isInputOnly = (hints & WINDOW_HINT_X11_PROPERTY_ONLY) != 0;

  if (_isInputOnly)
  {
    // Create X11-PropertyOnly window.
    _handle = (void*)xEngine->_XLib._XCreateWindow(
      xEngine->_display, xEngine->_root, x, y, w, h, 0, CopyFromParent, InputOnly, CopyFromParent, 0, &attr);

    xEngine->_XLib._XSelectInput(xEngine->_display, (XID)_handle, PropertyChangeMask);
  }
  else
  {
    ulong attrMask;

    attr.backing_store = NotUseful;
    attr.override_redirect = (hints & WINDOW_HINT_X11_OVERRIDE_REDIRECT) != 0;
    attr.colormap = xEngine->_colorMap;
    attr.border_pixel = 0;
    attr.background_pixel = XNone;
    attr.save_under = (hints & WINDOW_HINT_POPUP) != 0;
    attr.event_mask =
      StructureNotifyMask      | ButtonPressMask    |
      ButtonReleaseMask        | PointerMotionMask  |
      EnterWindowMask          | LeaveWindowMask    |
      KeyPressMask             | KeyReleaseMask     |
      ButtonMotionMask         | ExposureMask       |
      FocusChangeMask          | PropertyChangeMask |
      VisibilityChangeMask;

    attrMask =
      CWOverrideRedirect       |
      CWSaveUnder              |
      CWBackingStore           |
      CWColormap               |
      CWBorderPixel            |
      CWEventMask;

    _handle = (void*)xEngine->_XLib._XCreateWindow(
      xEngine->_display, xEngine->_root, x, y, w, h, 0, xEngine->_displayInfo._depth, InputOutput, xEngine->_visual, attrMask, &attr);
  }

  XGCValues gcValues;
  MemOps::zero(&gcValues, sizeof(XGCValues));

  // Create GC.
  _gc = xEngine->_XLib._XCreateGC(xEngine->_display, (XID)_handle, 0, &gcValues);

  // Create XID <-> X11UIEngineWindowImpl* connection.
  xEngine->addHandle(_handle, this);
  
  // Window protocols.
  {
    XAtom protocols[2];

    protocols[0] = xEngine->getAtom(X11_ATOM_WM_DELETE_WINDOW);
    protocols[1] = xEngine->getAtom(X11_ATOM_NET_WM_PING);

    xEngine->_XLib._XSetWMProtocols(xEngine->_display, (XID)_handle, protocols, 2);
  }

  //
  {
    /*
    XSizeHints sizeHints;
    sizeHints.flags = USSize | PSize | PWinGravity;
    sizeHints.x = x;
    sizeHints.y = y;
    sizeHints.width = w;
    sizeHints.height = h;
    sizeHints.win_gravity = NorthWestGravity;

    XWMHints wmHints;
    wmHints.flags = InputHint | StateHint;// | WindowGroupHint;
    wmHints.input = true;
    wmHints.initial_state = NormalState;
    wmHints.window_group = 0;
    */

    xEngine->_XLib._XSetWMProperties(xEngine->_display, (XID)_handle, NULL, NULL, NULL, 0, NULL, NULL, NULL);
  }

  // Set _NET_WM_PID.
  long pid = getpid();
  xEngine->_XLib._XChangeProperty(xEngine->_display, (XID)_handle,
    xEngine->getAtom(X11_ATOM_NET_WM_PID), XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&pid, 1);

  // Set WM_CLIENT_LEADER.
  long clientLeader = xEngine->getWMClientLeader();
  xEngine->_XLib._XChangeProperty(xEngine->_display, (XID)_handle,
    xEngine->getAtom(X11_ATOM_WM_CLIENT_LEADER), XA_WINDOW, 32, PropModeReplace, (unsigned char*)&clientLeader, 1);

  // Get correct window position.
  {
    XWindowAttributes wa;
    xEngine->_XLib._XGetWindowAttributes(xEngine->_display, (XID)_handle, &wa);

    _windowGeometry.setRect(wa.x, wa.y, (int)wa.width, (int)wa.height);
  }

  // Create X input context.
  if (xEngine->_xim)
  {
    _xic = xEngine->_XLib._XCreateIC(xEngine->_xim,
      XNInputStyle, (XIMPreeditNothing | XIMStatusNothing),
      XNClientWindow, (XID)_handle,
      XNFocusWindow, (XID)_handle,
      NULL);
  }

  // Windows are enabled by default.
  _isEnabled = true;

  // Windows are not visible by default.
  _isVisible = false;
  _isVisibleToParent = false;

  // Default focus is no focus, X11 will inform us if this gets changed.
  _hasFocus = false;

  // Will be set by show.
  _shouldBlit = false;

  // Clear flags, we will wait for events to set them.
  _mapRequest = false;

  return ERR_OK;
}

err_t X11UIEngineWindowImpl::destroy()
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  _windowBeingDestroyed = true;

  // Destroy XWindow and all associated resources.
  xEngine->_XLib._XFreeGC(xEngine->_display, _gc);
  xEngine->_XLib._XDestroyIC(_xic);
  xEngine->_XLib._XDestroyWindow(xEngine->_display, (XID)_handle);

  destroyed();
  return ERR_OK;
}

void X11UIEngineWindowImpl::destroyed()
{
  Base::destroyed();

  _xic = 0;
  _gc = 0;

  _isInputOnly = false;
  _isConfigured = false;
  _mapRequest = false;
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Enabled / Disabled]
// ============================================================================

err_t X11UIEngineWindowImpl::setEnabled(bool enabled)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Focus]
// ============================================================================

err_t X11UIEngineWindowImpl::focus()
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  xEngine->_XLib._XSetInputFocus(xEngine->_display, (XID)_handle, XNone, 0);
  return ERR_OK;
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Window State]
// ============================================================================

err_t X11UIEngineWindowImpl::setState(uint32_t state)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  switch (state)
  {
    case WINDOW_STATE_HIDDEN:
      xEngine->_XLib._XUnmapWindow(xEngine->_display, (XID)_handle);
      return ERR_OK;

    case WINDOW_STATE_NORMAL:
      xEngine->_XLib._XMapWindow(xEngine->_display, (XID)_handle);
      return ERR_OK;

    case WINDOW_STATE_MAXIMIZED:
      // TODO:
      return ERR_RT_NOT_IMPLEMENTED;

    case WINDOW_STATE_FULLSCREEN:
      // TODO:
      return ERR_RT_NOT_IMPLEMENTED;

    default:
      return ERR_RT_INVALID_STATE;
  }
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Window Geometry]
// ============================================================================

err_t X11UIEngineWindowImpl::setWindowPosition(const PointI& pos)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  if (_windowGeometry.x == pos.x && _windowGeometry.y == pos.y)
    return ERR_OK;

  xEngine->_XLib._XMoveWindow(xEngine->_display, (XID)_handle, pos.x, pos.y);
  return ERR_OK;
}

err_t X11UIEngineWindowImpl::setWindowSize(const SizeI& size)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  if (_windowGeometry.w == size.w && _windowGeometry.h == size.h)
    return ERR_OK;

  xEngine->_XLib._XResizeWindow(xEngine->_display, (XID)_handle, size.w, size.h);
  return ERR_OK;
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Window Stack]
// ============================================================================

err_t X11UIEngineWindowImpl::moveToTop(void* targetHandle)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t X11UIEngineWindowImpl::moveToBottom(void* targetHandle)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Window Coordinates]
// ============================================================================

err_t X11UIEngineWindowImpl::worldToClient(PointI& pt) const
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  XWindow child;
  int result = xEngine->_XLib._XTranslateCoordinates(
    xEngine->_display, xEngine->_root, (XID)_handle, pt.x, pt.y, &pt.x, &pt.y, &child);

  if (!result)
    return ERR_RT_INVALID_STATE;
  
  return ERR_OK;
}

err_t X11UIEngineWindowImpl::clientToWorld(PointI& pt) const
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  XWindow child;
  int result = xEngine->_XLib._XTranslateCoordinates(
    xEngine->_display, (XID)_handle, xEngine->_root, pt.x, pt.y, &pt.x, &pt.y, &child);

  if (!result)
    return ERR_RT_INVALID_STATE;
  
  return ERR_OK;
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Window Opacity]
// ============================================================================

err_t X11UIEngineWindowImpl::setWindowOpacity(float opacity)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_HANDLE;

  UInt64Bits opacityFixed;
  opacityFixed.u32Lo = uint32_t(Math::bound(Math::iround(opacity * 255.0f), 0, 255)) * 0x01010101;
  opacityFixed.u32Hi = opacityFixed.u32Lo;

  XAtom atom = xEngine->getAtom(X11_ATOM_NET_WM_WINDOW_OPACITY);
  xEngine->_XLib._XChangeProperty(
    xEngine->_display, (XID)_handle, atom, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&opacityFixed, 1L);

  _windowOpacity = opacity;
  return ERR_OK;
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Window Title]
// ============================================================================

err_t X11UIEngineWindowImpl::setWindowTitle(const StringW& title)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_HANDLE;

  XTextProperty textProperty;

  StringTmpA<TEMPORARY_LENGTH> titleUtf32;
  const wchar_t* titleW;

  if (sizeof(wchar_t) == 2)
  {
    titleW = reinterpret_cast<const wchar_t*>(title.getData());
  }
  else
  {
    FOG_RETURN_ON_ERROR(TextCodec::utf32().encode(titleUtf32, title));
    titleW = reinterpret_cast<const wchar_t*>(titleUtf32.getData());

    // StringA guarantees only 8-bit NULL terminator. We need to append wchar_t
    // NULL terminator manually.
    reinterpret_cast<wchar_t*>((uint8_t*)titleW + titleUtf32.getLength())[0] = wchar_t(0);
  }

  int result = xEngine->_XLib._XwcTextListToTextProperty(
    xEngine->_display, (wchar_t**)&titleW, 1, XTextStyle, &textProperty);

  if (result != XSuccess)
    return ERR_RT_INVALID_STATE;

  xEngine->_XLib._XSetWMName(xEngine->_display, (XID)_handle, &textProperty);
  xEngine->_XLib._XFree(textProperty.value);
  xEngine->_XLib._XSync(xEngine->_display, false);

  _windowTitle = title;
  return ERR_OK;
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Window Frame-Buffer]
// ============================================================================

err_t X11UIEngineWindowImpl::allocDoubleBuffer(const SizeI& size)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);
  err_t err = ERR_OK;

  uint32_t xFormat = (_windowHints & WINDOW_HINT_COMPOSITE)
    ? IMAGE_FORMAT_PRGB32
    : IMAGE_FORMAT_XRGB32;

  // Make target bpp correct, X server allows really weird combinations of bit
  // depth and component masks.
  uint32_t xDepth = xEngine->_displayInfo._depth;

  if (xDepth == 1)
    FOG_NOP;
  else if (xDepth <= 4)
    xDepth = 4;
  else if (xDepth <= 8)
    xDepth = 8;
  else if (xDepth <= 16)
    xDepth = 16;
  else if (xDepth <= 32)
    xDepth = 32;

  size_t xStride = (size_t)Image::getStrideFromWidth(size.w, xDepth);
  size_t xSize = xStride * size.h;

  // --------------------------------------------------------------------------
  // [Secondary-Buffer]
  // --------------------------------------------------------------------------

  // At this time the 32-bit depth is the only pixex format supported natively
  // by Fog-Framework. So if we want to display anything into the target device
  // we need to convert our 32-bpp data into the X-Server specific format. This
  // is the purpose of Secondary-Buffer.
  if (xDepth != 32)
  {
    if (!xEngine->_xDontLogUnsupportedBpp)
    {
      Logger::debug("Fog::X11UIEngineWindowImpl", "allocDoubleBuffer",
        "X-Server bit-depth (%d) is not supported by Fog, secondary frame-buffer needed.", xDepth);
      xEngine->_xDontLogUnsupportedBpp = true;
    }

    uint32_t dbDepth = 32;
    size_t fbStride = (size_t)Image::getStrideFromWidth(size.w, dbDepth);
    size_t fbSize = fbStride * size.h;

    uint8_t* fbData = static_cast<uint8_t*>(MemMgr::alloc(fbSize));
    if (FOG_IS_NULL(fbData))
    {
      Logger::error("Fog::X11UIEngineWindowImpl", "allocDoubleBuffer",
        "Can't allocate secondary frame-buffer, request size=%llu.", (unsigned long long)fbSize);
      return ERR_RT_OUT_OF_MEMORY;
    }

    _secondaryFB._data = fbData;
    _secondaryFB._stride = ssize_t(fbStride);
    _secondaryFB._convertFunc = NULL; // Will be set later.
    _secondaryFB._depth = dbDepth;
  }

  // --------------------------------------------------------------------------
  // [Primary-Buffer]
  // --------------------------------------------------------------------------
  
  if (xEngine->_xShm)
  {
    FOG_ASSERT(xEngine._XExtLibrary.isOpen());

    MemOps::zero_t<XShmSegmentInfo>(&_shmInfo);
    _shmInfo.readOnly = false;

    _shmInfo.shmid = ::shmget(IPC_PRIVATE, xSize, IPC_CREAT | 0666);
    if (_shmInfo.shmid < 0)
    {
      Logger::error("Fog::X11UIEngineWindowImpl", "allocDoubleBuffer",
        "Failed to call shmget(), errno=%d.", errno);
      goto _TryXImage;
    }

    if ((_shmInfo.shmaddr = (char *)::shmat(_shmInfo.shmid, NULL, 0)) == NULL)
    {
      Logger::error("Fog::X11UIEngineWindowImpl", "allocDoubleBuffer",
        "Failed to call shmat(), errno=%d.", errno);
      ::shmctl(_shmInfo.shmid, IPC_RMID, NULL);
      goto _TryXImage;
    }

    // Create XShmImage.
    if (!xEngine->_XExt._XShmAttach(xEngine->_display, &_shmInfo))
    {
      Logger::error("Fog::X11UIEngineWindowImpl", "allocDoubleBuffer",
        "Failed to call XShmAttach().");

      // TODO:
    }

    xEngine->_XLib._XSync(xEngine->_display, false);

    _ximage = xEngine->_XExt._XShmCreateImage(
      xEngine->_display, xEngine->_visual, xEngine->_displayInfo._depth, ZPixmap, _shmInfo.shmaddr, &_shmInfo, size.w, size.h);
    
    if (FOG_IS_NULL(_ximage))
    {
      Logger::error("Fog::X11UIEngineWindowImpl", "allocDoubleBuffer",
        "Failed to call XShmCreateImage().");
      // TODO:
    }

    _bufferType = UI_ENGINE_BUFFER_X11_XSHMIMAGE;
    _bufferPtr = (uint8_t*)_shmInfo.shmaddr;
  }
  else
  {
_TryXImage:
    _bufferPtr = (uint8_t*)MemMgr::alloc(xSize);
    if (FOG_IS_NULL(_bufferPtr))
    {
      Logger::error("Fog::X11UIEngineWindowImpl", "allocDoubleBuffer",
        "Out of memory, requested size=%llu.", (unsigned long long)xSize);

      goto _Fail;
    }

    // Create XImage.
    _ximage = xEngine->_XLib._XCreateImage(
      xEngine->_display, xEngine->_visual, xEngine->_displayInfo._depth, ZPixmap, 0, (char *)_bufferPtr, size.w, size.h, 32, xStride);

    if (FOG_IS_NULL(_ximage))
    {
      Logger::error("Fog::X11UIEngineWindowImpl", "allocDoubleBuffer",
        "Failed to call XCreateImage().");

      MemMgr::free(_bufferPtr);
      _bufferPtr = NULL;

      goto _Fail;
    }

    _bufferType = UI_ENGINE_BUFFER_X11_XIMAGE;
  }

  // --------------------------------------------------------------------------
  // [Primary-Buffer / Secondary-Buffer Converter]
  // --------------------------------------------------------------------------

  if (_secondaryFB._data == NULL)
  {
    _bufferData.data = _bufferPtr;
    _bufferData.size = size;
    _bufferData.format = xFormat;
    _bufferData.stride = xStride;
  }
  else
  {
    _bufferData.data = _secondaryFB._data;
    _bufferData.size = size;
    _bufferData.format = xFormat;
    _bufferData.stride = _secondaryFB._stride;

    uint64_t rMask = xEngine->_displayInfo._rMask;
    uint64_t gMask = xEngine->_displayInfo._gMask;
    uint64_t bMask = xEngine->_displayInfo._bMask;

    // TODO: Create converter.
    FOG_ASSERT(0);
  }

  return ERR_OK;

_Fail:
  if (_secondaryFB._data != NULL)
  {
    MemMgr::free(_secondaryFB._data);
    _secondaryFB.reset();
  }

  return err;
}

err_t X11UIEngineWindowImpl::freeDoubleBuffer()
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  // Free the secondary buffer if used.
  if (_secondaryFB._data != NULL)
  {
    MemMgr::free(_secondaryFB._data);
    _secondaryFB.reset();
  }

  switch (_bufferType)
  {
    case UI_ENGINE_BUFFER_NONE:
    {
      return ERR_OK;
    }

    case UI_ENGINE_BUFFER_X11_XIMAGE:
    {
      _ximage->data = NULL;
      xEngine->_XLib._XDestroyImage(_ximage);
      break;
    }

    case UI_ENGINE_BUFFER_X11_XSHMIMAGE:
    {
      _ximage->data = NULL;
      xEngine->_XLib._XDestroyImage(_ximage);

      xEngine->_XExt._XShmDetach(xEngine->_display, &_shmInfo);
      xEngine->_XLib._XSync(xEngine->_display, false);

      if (::shmdt(_shmInfo.shmaddr) != 0)
      {
        Logger::error("Fog::X11UIEngineWindowImpl", "freeDoubleBuffer",
          "Failed to call shmdt(), errno=%d.", errno);
      }

      if (::shmctl(_shmInfo.shmid, IPC_RMID, NULL) == -1)
      {
        Logger::error("Fog::X11UIEngineWindowImpl", "freeDoubleBuffer",
          "Failed to call shmctl(), errno=%d.", errno);
      }
      break;
    }

    default:
      return ERR_RT_INVALID_STATE;
  }

  _bufferData.reset();
  _bufferType = UI_ENGINE_BUFFER_NONE;

  _bufferCacheSize.reset();
  _bufferCacheCreated.reset();
  _bufferCacheExpire.reset();

  _bufferPtr = NULL;
  _pixmap = XNone;
  _ximage = NULL;

  return ERR_OK;
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - OnXEvent]
// ============================================================================

void X11UIEngineWindowImpl::onXEvent(XEvent* xe)
{
  X11UIEngine* xEngine = static_cast<X11UIEngine*>(_engine);

  switch (xe->xany.type)
  {
    case XCreateNotify:
    {
      // Logger::debug("Fog::X11UIEngineWindowImpl", "onXEvent",
      //   "XCreateNotify.");
      break;
    }

    case XDestroyNotify:
    {
      // Logger::debug("Fog::X11UIEngineWindowImpl", "onXEvent",
      //   "XDestroyNotify.");
      break;
    }

    case XMapNotify:
    {
      _isVisible = true;
      _mapRequest = false;

      xEngine->doStateAction(_window, UI_ENGINE_EVENT_SHOW, WINDOW_STATE_NORMAL);
      break;
    }

    case XMapRequest:
    {
      _isVisible = false;
      _mapRequest = true;

      xEngine->doStateAction(_window, UI_ENGINE_EVENT_HIDE, WINDOW_STATE_HIDDEN);
      break;
    }

    case XUnmapNotify:
    {
      _isVisible = false;
      _mapRequest = false;

      xEngine->doStateAction(_window, UI_ENGINE_EVENT_HIDE, WINDOW_STATE_HIDDEN);
      break;
    }

    case XConfigureRequest:
    {
      // Logger::debug("Fog::X11UIEngineWindowImpl", "onXEvent",
      //   "XConfigureRequest [%d %d %d %d].",
      //   xe->xconfigurerequest.x,
      //   xe->xconfigurerequest.y,
      //   xe->xconfigurerequest.width,
      //   xe->xconfigurerequest.height);
      break;
    }

    case XConfigureNotify:
    {
      _isConfigured = true;

      // Don't process old configure events.
      while (xEngine->_XLib._XCheckTypedWindowEvent(xEngine->_display, xe->xany.window, XConfigureNotify, xe))
        continue;

      // Logger::debug("Fog::X11UIEngineWindowImpl", "onXEvent",
      //   "XConfigureNotify [%d %d %d %d].",
      //   xe->xconfigure.x,
      //   xe->xconfigure.y,
      //   xe->xconfigure.width,
      //   xe->xconfigure.height);

      RectI wr(xe->xconfigure.x, xe->xconfigure.y, (int)xe->xconfigure.width, (int)xe->xconfigure.height);
      RectI cr(0, 0, wr.w, wr.h);

      xEngine->doGeometryAction(_window, UI_ENGINE_EVENT_GEOMETRY, ORIENTATION_HORIZONTAL, wr, cr);
      break;
    }

    case XFocusIn:
    {
      xEngine->doFocusAction(_window, UI_ENGINE_EVENT_FOCUS_IN);
      break;
    }

    case XFocusOut:
    {
      xEngine->doFocusAction(_window, UI_ENGINE_EVENT_FOCUS_OUT);
      break;
    }

    case XKeyPress:
    {
      uint32_t kId = 0;

      StringTmpW<8> tmp;
      KeySym xKeySym = 0;

      if (_xic)
      {
        // Use XIC (X input context) to translate key
        wchar_t wBuf[32];
        XStatus status;

        int len = xEngine->_XLib._XwcLookupString(_xic,
          reinterpret_cast<XKeyPressedEvent *>(xe), wBuf, FOG_ARRAY_SIZE(wBuf) - 1, &xKeySym, &status);

        switch (status)
        {
          case XBufferOverflow:
            // I don't know if this is possible when we have buffer for 31 
            // characters, if this error occurs, we simply skip this event.
            Logger::debug("Fog::X11UIEngineWindowImpl", "onXEvent",
              "XKeyPress - Buffer too small (XIC)");
            goto _OnXKeyPress_NoXIC;

          case XLookupChars:
          case XLookupKeySym:
          case XLookupBoth:
            tmp.setWChar(wBuf, len);
            break;

          default:
            goto _OnXKeyPress_NoXIC;
        }
      }
      else
      {
_OnXKeyPress_NoXIC:
        char cBuf[15*6 + 1];
        int len = xEngine->_XLib._XLookupString(&xe->xkey, cBuf, FOG_ARRAY_SIZE(cBuf) - 1, &xKeySym, 0);
        TextCodec::local8().decode(tmp, StubA(cBuf, len));
      }

      {
        uint32_t keyCode = xEngine->getFogKeyFromXKeySym(xKeySym);
        uint32_t keyUC = 0;

        if (tmp.getLength() > 0)
        {
          keyUC = tmp.getAt(0);
          if (CharW::isSurrogate(keyUC))
          {
            if (tmp.getLength() < 2)
              keyUC = 0;
            else
              keyUC = CharW::ucs4FromSurrogate((uint16_t)keyUC, tmp.getAt(1));
          }
        }
        
        xEngine->doKeyAction(_window, UI_ENGINE_EVENT_KEY_DOWN, kId, keyCode, xEngine->getModifierFromKey(keyCode), xe->xkey.keycode, keyUC);
      }
      break;
    }

    case XKeyRelease:
    {
      uint32_t kId = 0;

      KeySym xKeySym = xEngine->_XLib._XKeycodeToKeysym(xEngine->_display, xe->xkey.keycode, 0);
      uint32_t keyCode = xEngine->getFogKeyFromXKeySym(xKeySym);

      xEngine->doKeyAction(_window, UI_ENGINE_EVENT_KEY_UP, kId, keyCode, xEngine->getModifierFromKey(keyCode), xe->xkey.keycode, 0);
      break;
    }

    case XEnterNotify:
    {
      uint32_t mId = 0;
      UIEngineMouseState& mState = xEngine->_mouseState[mId];

      if (mState._window == _window && mState._buttonMask != 0)
        xEngine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_MOVE, mId, PointI(xe->xmotion.x, xe->xmotion.y), BUTTON_NONE, mState._buttonMask);
      else
        xEngine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_IN, mId, PointI(xe->xmotion.x, xe->xmotion.y), BUTTON_NONE, mState._buttonMask);
      break;
    }

    case XLeaveNotify:
    {
      uint32_t mId = 0;
      UIEngineMouseState& mState = xEngine->_mouseState[mId];

      if (mState._window == _window && mState._buttonMask != 0)
        xEngine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_MOVE, mId, PointI(xe->xmotion.x, xe->xmotion.y), BUTTON_NONE, mState._buttonMask);
      else
        xEngine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_OUT, mId, PointI(xe->xmotion.x, xe->xmotion.y), BUTTON_NONE, mState._buttonMask);
      break;
    }

    case XMotionNotify:
    {
      uint32_t mId = 0;
      UIEngineMouseState& mState = xEngine->_mouseState[mId];

      xEngine->doMouseAction(_window, UI_ENGINE_EVENT_MOUSE_MOVE, mId, PointI(xe->xmotion.x, xe->xmotion.y), BUTTON_NONE, mState._buttonMask);
      break;
    }

    case XButtonPress:
    {
      uint32_t mId = 0;
      UIEngineMouseState& mState = xEngine->_mouseState[mId];

      uint32_t eventCode = UI_ENGINE_EVENT_NONE;
      uint32_t mButton = xEngine->getFogButtonFromXButton(xe->xbutton.button);

      switch (mButton)
      {
        case BUTTON_NONE:
          break;

        case BUTTON_LEFT:
        case BUTTON_MIDDLE:
        case BUTTON_RIGHT:
        default:
          eventCode = UI_ENGINE_EVENT_MOUSE_DOWN;
          break;

        case BUTTON_WHEEL_UP:
          eventCode = UI_ENGINE_EVENT_WHEEL_UP;
          break;

        case BUTTON_WHEEL_DOWN:
          eventCode = UI_ENGINE_EVENT_WHEEL_DOWN;
          break;
      }
      
      if (eventCode != UI_ENGINE_EVENT_NONE)
        xEngine->doMouseAction(_window, eventCode, mId, PointI(xe->xbutton.x, xe->xbutton.y), mButton, mState._buttonMask);
      break;
    }

    case XButtonRelease:
    {
      uint32_t mId = 0;
      UIEngineMouseState& mState = xEngine->_mouseState[mId];

      uint32_t eventCode = UI_ENGINE_EVENT_NONE;
      uint32_t mButton = xEngine->getFogButtonFromXButton(xe->xbutton.button);

      switch (mButton)
      {
        case BUTTON_NONE:
        case BUTTON_WHEEL_UP:
        case BUTTON_WHEEL_DOWN:
          break;

        case BUTTON_LEFT:
        case BUTTON_MIDDLE:
        case BUTTON_RIGHT:
        default:
          eventCode = UI_ENGINE_EVENT_MOUSE_DOWN;
          break;
      }

      if (eventCode != UI_ENGINE_EVENT_NONE)
        xEngine->doMouseAction(_window, eventCode, mId, PointI(xe->xbutton.x, xe->xbutton.y), mButton, mState._buttonMask);
      break;
    }

    case XExpose:
    {
      if (_shouldUpdate || _shouldPaint)
      {
        _shouldBlit = true;

        // Eat all events, because we will repaint later (in update process).
        while (xEngine->_XLib._XCheckTypedWindowEvent(xEngine->_display, xe->xany.window, XExpose, xe))
        {
          _paintRegion.union_(RectI(xe->xexpose.x, xe->xexpose.y, xe->xexpose.width, xe->xexpose.height));
          continue;
        }
      }
      else
      {
        do {
          int x = xe->xexpose.x;
          int y = xe->xexpose.y;
          _blitRegion.union_(BoxI(x, y, x + xe->xexpose.width, y + xe->xexpose.height));
        } while (xe->xexpose.count > 0 && xEngine->_XLib._XCheckTypedWindowEvent(xEngine->_display, xe->xany.window, XExpose, xe));

        xEngine->doBlitWindow(_window);
        _shouldBlit = false;
      }
      break;
    }

    case XClientMessage:
    {
      // It looks that all client messages should be in 32 bit (long) format.
      if (xe->xclient.format == 32)
      {
        // WM_PROTOCOLS messages.
        if (xe->xclient.message_type == xEngine->getAtom(X11_ATOM_WM_PROTOCOLS))
        {
          XAtom msg = (XAtom)xe->xclient.data.l[0];

          if (msg == xEngine->getAtom(X11_ATOM_WM_DELETE_WINDOW))
          {
            xEngine->doCloseAction(_window);
          }
          else if (msg == xEngine->getAtom(X11_ATOM_NET_WM_PING))
          {
            if (xe->xclient.window != xEngine->_root)
              xEngine->sendXClientMessage(xEngine->_root,
                SubstructureNotifyMask | SubstructureRedirectMask,
                xe->xclient.data.l[0],
                xe->xclient.data.l[1],
                xe->xclient.data.l[2],
                xe->xclient.data.l[3],
                xe->xclient.data.l[4]);
          }
        }
      }
      break;
    }

    default:
      break;
  }
}

// ============================================================================
// [Fog::X11UIEngineWindowImpl - Misc]
// ============================================================================

void X11UIEngineWindowImpl::setMoveableHints()
{
  // TODO:
}

} // Fog namespace
