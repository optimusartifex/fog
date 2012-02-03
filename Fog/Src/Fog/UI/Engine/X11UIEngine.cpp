// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/UI/Engine/X11UIEngine.h>
#include <Fog/UI/Engine/X11UIEngineWindow.h>
#include <Fog/UI/Engine/X11UIEventLoopImpl_p.h>

// [Dependencies - C -Shared memory and IPC]
#include <errno.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/utsname.h>
#include <unistd.h>

// [gethostname() / uname()]
#define FOG_HAVE_GETHOSTNAME

FOG_IMPLEMENT_OBJECT(Fog::X11UIEngine)

namespace Fog {

// ============================================================================
// [Fog::X11UIEngine - Symbol Names]
// ============================================================================

// Xlib symbols.
static const char X11UIEngine_XLibSymbolNames[] =
{
  "XSupportsLocale\0"
  "XSetLocaleModifiers\0"
  "XOpenDisplay\0"
  "XCloseDisplay\0"
  "XSetErrorHandler\0"
  "XSetIOErrorHandler\0"
  "XGetErrorText\0"
  "XInternAtom\0"
  "XInternAtoms\0"
  "XRootWindow\0"
  "XDisplayWidth\0"
  "XDisplayHeight\0"
  "XDefaultScreen\0"
  "XDefaultVisual\0"
  "XDefaultColormap\0"
  "XDefaultDepth\0"
  "XEventsQueued\0"
  "XNextEvent\0"
  "XCheckMaskEvent\0"
  "XCheckTypedEvent\0"
  "XCheckTypedWindowEvent\0"
  "XCheckWindowEvent\0"
  "XSendEvent\0"
  "XOpenIM\0"
  "XCloseIM\0"
  "XCreateIC\0"
  "XDestroyIC\0"
  "XRefreshKeyboardMapping\0"
  "XCreateWindow\0"
  "XDestroyWindow\0"
  "XMoveWindow\0"
  "XResizeWindow\0"
  "XMoveResizeWindow\0"
  "XMapWindow\0"
  "XUnmapWindow\0"
  "XRaiseWindow\0"
  "XLowerWindow\0"
  "XReparentWindow\0"
  "XChangeWindowAttributes\0"
  "XGetWindowAttributes\0"
  "XSetInputFocus\0"
  "XGetInputFocus\0"
  "XSelectInput\0"
  "XSetNormalHints\0"
  "XSetWMProtocols\0"
  "XGetWMProtocols\0"
  "XSetWMProperties\0"
  "XTranslateCoordinates\0"
  "XwcTextListToTextProperty\0"
  "XFlush\0"
  "XSync\0"
  "XCreateGC\0"
  "XFreeGC\0"
  "XSetClipRectangles\0"
  "XKeycodeToKeysym\0"
  "XLookupString\0"
  "XwcLookupString\0"
  "XGrabPointer\0"
  "XUngrabPointer\0"
  "XGetPointerMapping\0"
  "XCreatePixmap\0"
  "XFreePixmap\0"
  "XCopyArea\0"
  "XFree\0"
  "XCreateImage\0"
  "XDestroyImage\0"
  "XPutImage\0"
  "XCreateColormap\0"
  "XFreeColormap\0"
  "XAllocColor\0"
  "XFreeColors\0"
  "XMatchVisualInfo\0"
  "XSetWMName\0"
  "XChangeProperty\0"
  "XSetSelectionOwner\0"
  "XGetAtomName\0"
  "XConvertSelection\0"
  "XGetWindowProperty\0"
  "XDeleteProperty\0"
};

// Xext symbols.
static const char X11UIEngine_XExtSymbolNames[] =
{
  "XShmQueryVersion\0"
  "XShmAttach\0"
  "XShmDetach\0"
  "XShmCreateImage\0"
  "XShmPutImage\0"
};

// Xrender symbols.
static const char X11UIEngine_XRenderSymbolNames[] =
{
  "XRenderQueryVersion\0"
  "XRenderQueryFormats\0"
  "XRenderQuerySubpixelOrder\0"
  "XRenderSetSubpixelOrder\0"
  "XRenderFindVisualFormat\0"
  "XRenderFindFormat\0"
  "XRenderFindStandardFormat\0"
  "XRenderCreatePicture\0"
  "XRenderChangePicture\0"
  "XRenderSetPictureTransform\0"
  "XRenderFreePicture\0"
  "XRenderComposite\0"
  "XRenderCreateCursor\0"
};

// ============================================================================
// [Fog::X11UIEngine - Atom Names]
// ============================================================================

static const char *X11UIEngine_atomNames[X11_ATOM_COUNT] =
{
  // WM Events.
  "WM_PROTOCOLS",                 /* 0  */
  "WM_DELETE_WINDOW",             /* 1  */
  "WM_TAKE_FOCUS",                /* 2  */
  "WM_CLASS",                     /* 3  */
  "WM_NAME",                      /* 4  */
  "WM_COMMAND",                   /* 5  */
  "WM_ICON_NAME",                 /* 6  */
  "WM_CHANGE_STATE",              /* 7  */
  "WM_CLIENT_LEADER",             /* 8  */

  // Motif WM hints.
  "_MOTIF_WM_HINTS",              /* 9  */

  "_WIN_LAYER",                   /* 10  */

  // Drag'n'Drop.
  "XdndSelection",                /* 11 */
  "XdndAware",                    /* 12 */
  "XdndTypeList",                 /* 13 */
  "XdndEnter",                    /* 14 */
  "XdndPosition",                 /* 15 */
  "XdndActionCopy",               /* 16 */
  "XdndActionMove",               /* 17 */
  "XdndActionPrivate",            /* 18 */
  "XdndActionAsk",                /* 19 */
  "XdndActionList",               /* 20 */
  "XdndActionLink",               /* 21 */
  "XdndActionDescription",        /* 22 */
  "XdndProxy",                    /* 23 */
  "XdndStatus",                   /* 24 */
  "XdndLeave",                    /* 25 */
  "XdndDrop",                     /* 26 */
  "XdndFinished",                 /* 27 */

  // Standard _NET.
  "_NET_SUPPORTED",               /* 28 */
  "_NET_SUPPORTING_WM_CHECK",     /* 29 */

  "_NET_NUMBER_OF_DESKTOPS",      /* 30 */
  "_NET_DESKTOP_GEOMETRY",        /* 31 */
  "_NET_DESKTOP_NAMES",           /* 32 */
  "_NET_CURRENT_DESKTOP",         /* 33 */
  "_NET_DESKTOP_VIEWPORT",        /* 34 */
  "_NET_WORKAREA",                /* 35 */
  "_NET_VIRTUAL_ROOTS",           /* 36 */

  "_NET_CLIENT_LIST",             /* 37 */
  "_NET_CLIENT_LIST_STACKING",    /* 38 */
  "_NET_ACTIVE_WINDOW",           /* 39 */
  "_NET_CLOSE_WINDOW",            /* 40 */

  "_NET_WM_MOVERESIZE",           /* 41 */
  "_NET_WM_NAME",                 /* 42 */
  "_NET_WM_VISIBLE_NAME",         /* 43 */
  "_NET_WM_ICON_NAME",            /* 44 */
  "_NET_WM_VISIBLE_ICON_NAME",    /* 45 */
  "_NET_WM_DESKTOP",              /* 46 */
  "_NET_WM_WINDOW_TYPE",          /* 47 */
  "_NET_WM_STATE",                /* 48 */
  "_NET_WM_ALLOWED_ACTIONS",      /* 49 */
  "_NET_WM_STRUT",                /* 50 */
  "_NET_WM_STRUT_PARTIAL",        /* 51 */
  "_NET_WM_ICON_GEOMETRY",        /* 52 */
  "_NET_WM_ICON",                 /* 53 */
  "_NET_WM_PID",                  /* 54 */
  "_NET_WM_PING",                 /* 55 */
  "_NET_WM_USER_TIME",            /* 56 */
  "_NET_WM_SYNC_REQUEST",         /* 57 */

  "_NET_WM_WINDOW_TYPE_DESKTOP",  /* 58 */
  "_NET_WM_WINDOW_TYPE_DOCK",     /* 59 */
  "_NET_WM_WINDOW_TYPE_TOOLBAR",  /* 60 */
  "_NET_WM_WINDOW_TYPE_MENU",     /* 61 */
  "_NET_WM_WINDOW_TYPE_UTILITY",  /* 62 */
  "_NET_WM_WINDOW_TYPE_SPLASH",   /* 63 */
  "_NET_WM_WINDOW_TYPE_DIALOG",   /* 64 */
  "_NET_WM_WINDOW_TYPE_NORMAL",   /* 65 */

  "_NET_WM_STATE_MODAL",          /* 66 */
  "_NET_WM_STATE_STICKY",         /* 67 */
  "_NET_WM_STATE_MAXIMIZED_VERT", /* 68 */
  "_NET_WM_STATE_MAXIMIZED_HORZ", /* 69 */
  "_NET_WM_STATE_SHADED",         /* 70 */
  "_NET_WM_STATE_SKIP_TASKBAR",   /* 71 */
  "_NET_WM_STATE_SKIP_PAGER",     /* 72 */
  "_NET_WM_STATE_HIDDEN",         /* 73 */
  "_NET_WM_STATE_FULLSCREEN",     /* 74 */
  "_NET_WM_STATE_ABOVE",          /* 75 */
  "_NET_WM_STATE_BELOW",          /* 76 */
  "_NET_WM_WINDOW_OPACITY",       /* 77 */

  // Clipboard.
  "CLIPBOARD",                    /* 78 */
  "TARGETS",                      /* 79 */
  "COMPOUND_TEXT",                /* 80 */
  "UTF8_STRING",                  /* 81 */
  "FILE_NAME",                    /* 82 */
  "STRING",                       /* 83 */
  "TEXT",                         /* 84 */
  "INCR"
};

// ============================================================================
// [Fog::X11UIEngine - Error]
// ============================================================================

static int FOG_CDECL X11UIEngine_IOErrorHandler(XDisplay* display)
{
  Logger::fatal("Fog::X11UIEngine", "IOErrorHandler", "IO Error.");
  Application::terminate(-1);

  return 0;
}

static int FOG_CDECL X11UIEngine_ErrorHandler(XDisplay* display, XErrorEvent* xe)
{
  X11UIEngine* engine = static_cast<X11UIEngine*>(Application::get()->getUIEngine());
  char buffer[256];

  engine->_XLib._XGetErrorText(display, xe->error_code, buffer, 255);
  buffer[255] = '\0';

  Logger::error("Fog::X11UIEngine", "ErrorHandler", "%s", buffer);
  return 0;
}

// ============================================================================
// [Fog::X11UIEngine - Helpers]
// ============================================================================

static int X11UIEngine_getMouseWheelLines(X11UIEngine* self)
{
  // TODO:
  return UI_ENGINE_MISC_DEFAULT_WHEEL_LINES;
}

// ============================================================================
// [Fog::X11UIEngine - ColorMap]
// ============================================================================

static err_t X11UIEngine_createColorMap(X11UIEngine* self);

static uint X11UIEngine_allocRGB(X11UIEngine* self, XColormap cMap, int nr, int ng, int nb, uint8_t* pMap);
static void X11UIEngine_freeRGB(X11UIEngine* self, XColormap cMap, const uint8_t* pMap, uint count);

// ============================================================================
// [Fog::X11UIEngine - X11 ColorMap]
// ============================================================================

static err_t X11UIEngine_createColorMap(X11UIEngine* self)
{
  struct ColorMapData
  {
    uint32_t rColors;
    uint32_t gColors;
    uint32_t bColors;

    uint32_t rMask;
    uint32_t gMask;
    uint32_t bMask;
  };

  ColorMapData cMapData[] =
  {
    { 4, 8, 4, 0x60, 0x1C, 0x03 },
    { 4, 4, 4, 0x30, 0x0C, 0x03 },
    { 2, 2, 2, 0x04, 0x02, 0x01 }
  };

  uint i, v;

  // First try to match some visuals, because some X modes can't be used to
  // alloc requested number of colors.
  XVisualInfo vi;
  XVisual* visuals[4];
  XVisual** visualPtr = visuals;
  uint visualsCount;

  // Default or created colormap, see loop.
  XColormap cMap;

  // Default visual is the most wanted.
  *visualPtr++ = self->_visual;

  // Match some interesting visuals.
  if (self->_XLib._XMatchVisualInfo(self->_display, self->_screen, self->_displayInfo._depth, DirectColor, &vi))
  {
    *visualPtr++ = vi.visual;
  }

  if (self->_XLib._XMatchVisualInfo(self->_display, self->_screen, self->_displayInfo._depth, PseudoColor, &vi))
  {
    *visualPtr++ = vi.visual;
  }

  visualsCount = (uint)(visualPtr - visuals);

  for (v = 0; v != visualsCount; v++)
  {
    if (v == 0)
    {
      cMap = self->_colorMap;
    }
    else
    {
      cMap = self->_XLib._XCreateColormap(self->_display, self->_root, visuals[v], AllocNone);
      if (!cMap) return false;
    }

    for (i = 0; i != FOG_ARRAY_SIZE(cMapData); i++)
    {
      self->_paletteInfo._length = X11UIEngine_allocRGB(self, cMap,
        cMapData[i].rColors,
        cMapData[i].gColors,
        cMapData[i].bColors,
        self->_paletteInfo._map);

      if (self->_paletteInfo._length > 0)
      {
        self->_displayInfo._aMask = FOG_UINT64_C(0);
        self->_displayInfo._rMask = cMapData[i].rMask;
        self->_displayInfo._gMask = cMapData[i].gMask;
        self->_displayInfo._bMask = cMapData[i].bMask;

        if (v != 0)
        {
          self->_visual = visuals[v];
          self->_colorMap = cMap;
          self->_xPrivateColorMap = true;
        }
        return true;
      }

    }

    if (v != 0)
    {
      self->_XLib._XFreeColormap(self->_display, cMap);
    }
  }

  return false;
}

static uint X11UIEngine_allocRGB(X11UIEngine* self, XColormap cMap, int nr, int ng, int nb, uint8_t* pMap)
{
  int r, g, b, i;
  int cMask = 0;

  for (i = 0; i < self->_visual->bits_per_rgb; i++)
  {
    cMask |= (0x1 << i);
  }
  cMask <<= (16 - self->_visual->bits_per_rgb);

  ushort rTable[256];
  ushort gTable[256];
  ushort bTable[256];

  for (r = 0; r < nr; r++) { rTable[r] = (ushort)(int)( (float(r) / (float(nr - 1))) * float(65535) ); }
  for (g = 0; g < ng; g++) { gTable[g] = (ushort)(int)( (float(g) / (float(ng - 1))) * float(65535) ); }
  for (b = 0; b < nb; b++) { bTable[b] = (ushort)(int)( (float(b) / (float(nb - 1))) * float(65535) ); }

  i = 0;

  for (r = 0; r < nr; r++)
  {
    for (g = 0; g < ng; g++)
    {
      for (b = 0; b < nb; b++)
      {
        XColor xcl;
        XColor xcl_in;
        XStatus ret;

        xcl.red = rTable[r];
        xcl.green = gTable[g];
        xcl.blue = bTable[b];
        xcl_in = xcl;

        ret = self->_XLib._XAllocColor(self->_display, cMap, &xcl);

        if ((ret == 0) ||
            ((xcl_in.red   & cMask) != (xcl.red   & cMask)) ||
            ((xcl_in.green & cMask) != (xcl.green & cMask)) ||
            ((xcl_in.blue  & cMask) != (xcl.blue  & cMask)) )
        {
          X11UIEngine_freeRGB(self, cMap, pMap, i);
          return 0;
        }

        pMap[i++] = xcl.pixel;
      }
    }
  }

  return i;
}

void X11UIEngine_freeRGB(X11UIEngine* self, XColormap cMap, const uint8_t* pMap, uint count)
{
  ulong pixels[256];
  FOG_ASSERT(count <= 256);

  if (count == 0)
    return;

  for (uint i = 0; i < count; i++)
    pixels[i] = (ulong)pMap[i];

  self->_XLib._XFreeColors(self->_display, cMap, pixels, count, 0);
}

// ============================================================================
// [Fog::X11UIEngine - Construction / Destruction]
// ============================================================================

X11UIEngine::X11UIEngine() : 
  _display(NULL),
  _fd(-1),
  _screen(-1),
  _root(0),
  _gc(0),
  _visual(NULL),
  _colorMap(0),
  _xim(0),
  _numButtons(0),
  _xShm(false),
  _xPrivateColorMap(false),
  _xForcePixmap(true),
  _xDontLogUnsupportedBpp(false),
  _wmClientLeader(0),
  _atomNames(X11UIEngine_atomNames)
{
  _wakeUpPipe[0] = -1;
  _wakeUpPipe[1] = -1;
  
  // Make sure that there is no garbage in these arrays.
  MemOps::zero(_atomList, X11_ATOM_COUNT * sizeof(XAtom));
  MemOps::zero(&_keyMap , sizeof(_keyMap));
  MemOps::zero(&_XLib   , sizeof(_XLib));
  MemOps::zero(&_XExt   , sizeof(_XExt));
  MemOps::zero(&_XRender, sizeof(_XRender));

  if (openXlib() != ERR_OK)
    return;

  if (openDisplay() != ERR_OK)
    return;
  
  initDisplay();
  initKeyboard();
  initMouse();

  _eventLoop.adopt(fog_new X11UIEventLoopImpl(this));
  _isInitialized = true;
}

X11UIEngine::~X11UIEngine()
{
  _eventLoop.reset();
  
  closeDisplay();
}

// ============================================================================
// [Fog::X11UIEngine - Open / Close]
// ============================================================================

err_t X11UIEngine::openXlib()
{
  // Try to load X11 library and all helper libraries. We need only X11 to be
  // present, all other libraries are optional and will be only used if found.
  // Load X11.

  StringW name;
  char* badSymbol;

  const size_t numXLibSymbols = sizeof(X11UIEngineXLibAPI) / sizeof(void*);
  const size_t numXExtSymbols = sizeof(X11UIEngineXExtAPI) / sizeof(void*);
  const size_t numXRenderSymbols = sizeof(X11UIEngineXRenderAPI) / sizeof(void*);  

  // Load X11.
  FOG_RETURN_ON_ERROR(name.set(Ascii8("X11")));
  if (_XLibLibrary.openLibrary(name) != ERR_OK)
  {
    Logger::error("Fog::X11UIEngine", "openXlib", "Failed to open X11 library.");
    return ERR_UI_X11_ENGINE_XLIB_NOT_AVAILABLE;
  }

  if (_XLibLibrary.getSymbols(reinterpret_cast<void**>(&_XLib),
    X11UIEngine_XLibSymbolNames, FOG_ARRAY_SIZE(X11UIEngine_XLibSymbolNames),
    numXLibSymbols, &badSymbol) != numXLibSymbols)
  {
    Logger::error("Fog::X11UIEngine", "openXlib", "Failed to load X11 symbol %s.", badSymbol);
    return ERR_UI_X11_ENGINE_XLIB_NOT_AVAILABLE;
  }

  // Load Xext.
  name.set(Ascii8("Xext"));
  if (_XExtLibrary.openLibrary(name) != ERR_OK)
  {
    Logger::debug("Fog::X11UIEngine", "openXlib", "Failed to open Xext library.");
  }
  else if (_XExtLibrary.getSymbols(reinterpret_cast<void**>(&_XExt),
    X11UIEngine_XExtSymbolNames, FOG_ARRAY_SIZE(X11UIEngine_XExtSymbolNames),
    numXExtSymbols, &badSymbol) != numXExtSymbols)
  {
    Logger::debug("Fog::X11UIEngine", "openXlib", "Failed to load Xext symbol %s.", badSymbol);
    _XExtLibrary.close();
  }

  // Load Xrender.
  name.set(Ascii8("Xrender"));
  if (_XRenderLibrary.openLibrary(name) != ERR_OK)
  {
    Logger::debug("Fog::X11UIEngine", "openXlib", "Failed to open Xrender library.");
  }
  else if (_XRenderLibrary.getSymbols(reinterpret_cast<void**>(&_XRender),
    X11UIEngine_XRenderSymbolNames, FOG_ARRAY_SIZE(X11UIEngine_XRenderSymbolNames),
    numXRenderSymbols, &badSymbol) != numXRenderSymbols)
  {
    Logger::debug("Fog::X11UIEngine", "openXlib", "Failed to load Xrender symbol %s.", badSymbol);
    _XRenderLibrary.close();
  }

  // Setup locale.
  if (!_XLib._XSupportsLocale())
  {
    Logger::debug("Fog::X11UIEngine", "openXlib", "X does not support locale.");
  }
  else
  {
    char* localeModifiers = _XLib._XSetLocaleModifiers("");
    if (localeModifiers == NULL)
    {
      Logger::debug("Fog::X11UIEngine", "openXlib", "Can't set X locale modifiers.");
    }
  }
  
  // Setup error handlers.
  _XLib._XSetIOErrorHandler(X11UIEngine_IOErrorHandler);
  _XLib._XSetErrorHandler(X11UIEngine_ErrorHandler);

  return ERR_OK;
}

err_t X11UIEngine::openDisplay()
{
  err_t err = ERR_OK;

  // Open X11 Display.
  _display = _XLib._XOpenDisplay("");
  if (_display == NULL)
  {
    Logger::debug("Fog::X11UIEngine", "openDisplay", "Can't open display.");
    return ERR_UI_X11_ENGINE_CANT_OPEN_DISPLAY;
  }

  _fd = ConnectionNumber(_display);
  _screen = _XLib._XDefaultScreen(_display);
  _visual = _XLib._XDefaultVisual(_display, _screen);
  _colorMap = _XLib._XDefaultColormap(_display, _screen);
  _root = _XLib._XRootWindow(_display, _screen);
  _gc = _XLib._XCreateGC(_display, _root, 0, NULL);

  _xShm = false;
  _xPrivateColorMap = false;

  // Create wakeup pipe.
  if (::pipe(_wakeUpPipe) < 0)
  {
    Logger::error("Fog::X11UIEngine", "openDisplay", "Failed to create pipe using pipe(), errno=%d.", errno);

    err = ERR_UI_X11_ENGINE_CANT_CREATE_PIPE;
    goto _Fail;
  }

  // Intern atoms.
  _atomNames = X11UIEngine_atomNames;
  _XLib._XInternAtoms(_display, (char **)_atomNames, X11_ATOM_COUNT, false, _atomList);

  // Open XIM.
  _xim = _XLib._XOpenIM(_display, NULL, NULL, NULL);

  // Get whether the X-SHM pixmap extension is supported.
  _xShm = _XExtLibrary.isOpen();
  return ERR_OK;

_Fail:
  closeDisplay();
  return err;
}

err_t X11UIEngine::closeDisplay()
{
  // Delete helper windows.
  if (_wmClientLeader)
    _XLib._XDestroyWindow(_display, _wmClientLeader);

  // Close display.
  if (_xim)
    _XLib._XCloseIM(_xim);

  if (_gc)
    _XLib._XFreeGC(_display, _gc);

  if (_xPrivateColorMap)
    _XLib._XFreeColormap(_display, _colorMap);
  
  if (_display)
    _XLib._XCloseDisplay(_display);

  // Close pipes.
  if (_wakeUpPipe[0] != -1)
    ::close(_wakeUpPipe[0]);

  if (_wakeUpPipe[1] != -1)
    ::close(_wakeUpPipe[1]);
  
  _display = NULL;
  _xim = 0;
  _gc = 0;
  _xPrivateColorMap = false;

  _wakeUpPipe[0] = -1;
  _wakeUpPipe[1] = -1;
  
  return ERR_OK;
}

// ============================================================================
// [Fog::X11UIEngine - Init]
// ============================================================================

void X11UIEngine::initDisplay()
{
  updateDisplayInfo();

  // Create colormap for 8-bpp or less.
  if (_displayInfo.getDepth() <= 8)
  {
    err_t err = X11UIEngine_createColorMap(this);
    if (err != ERR_OK)
      Logger::error("Fog::X11UIEngine", "initDisplay()", "Can't create X11 color-map (error=%u).", err);
  }
}

void X11UIEngine::initKeyboard()
{
  // 0xFE
  _keyMap.odd[32] = KEY_TAB; // TAB

  _keyMap.odd[XK_dead_grave            & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_acute            & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_tilde            & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_macron           & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_breve            & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_abovedot         & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_diaeresis        & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_abovering        & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_doubleacute      & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_caron            & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_cedilla          & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_ogonek           & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_iota             & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_voiced_sound     & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_semivoiced_sound & 0xFF] = KEY_COMPOSE;
  _keyMap.odd[XK_dead_belowdot         & 0xFF] = KEY_COMPOSE;

#if defined(XK_dead_hook)
  _keyMap.odd[XK_dead_hook             & 0xFF] = KEY_COMPOSE;
#endif // XK_dead_hook

#if defined(XK_dead_horn)
  _keyMap.odd[XK_dead_horn             & 0xFF] = KEY_COMPOSE;
#endif // XK_dead_horn

#if defined(XK_dead_circumflex)
  _keyMap.odd[XK_dead_circumflex       & 0xFF] = KEY_CARET;
#endif // XK_dead_circumflex

#if defined(XK_ISO_Level3_Shift)
  // _keyMap.odd[XK_ISO_Level3_Shift   & 0xFF] = KEY_MOVE;
#endif // XK_ISO_Level3_Shift

  // 0xFF
  _keyMap.msc[XK_BackSpace            & 0xFF] = KEY_BACKSPACE;
  _keyMap.msc[XK_Tab                  & 0xFF] = KEY_TAB;
  _keyMap.msc[XK_Clear                & 0xFF] = KEY_CLEAR;
  _keyMap.msc[XK_Return               & 0xFF] = KEY_ENTER;
  _keyMap.msc[XK_Pause                & 0xFF] = KEY_PAUSE;
  _keyMap.msc[XK_Escape               & 0xFF] = KEY_ESC;
  _keyMap.msc[XK_Delete               & 0xFF] = KEY_DELETE;

  _keyMap.msc[XK_KP_0                 & 0xFF] = KEY_KP_0;
  _keyMap.msc[XK_KP_1                 & 0xFF] = KEY_KP_1;
  _keyMap.msc[XK_KP_2                 & 0xFF] = KEY_KP_2;
  _keyMap.msc[XK_KP_3                 & 0xFF] = KEY_KP_3;
  _keyMap.msc[XK_KP_4                 & 0xFF] = KEY_KP_4;
  _keyMap.msc[XK_KP_5                 & 0xFF] = KEY_KP_5;
  _keyMap.msc[XK_KP_6                 & 0xFF] = KEY_KP_6;
  _keyMap.msc[XK_KP_7                 & 0xFF] = KEY_KP_7;
  _keyMap.msc[XK_KP_8                 & 0xFF] = KEY_KP_8;
  _keyMap.msc[XK_KP_9                 & 0xFF] = KEY_KP_9;
  _keyMap.msc[XK_KP_Insert            & 0xFF] = KEY_KP_0;
  _keyMap.msc[XK_KP_End               & 0xFF] = KEY_KP_1;
  _keyMap.msc[XK_KP_Down              & 0xFF] = KEY_KP_2;
  _keyMap.msc[XK_KP_Page_Down         & 0xFF] = KEY_KP_3;
  _keyMap.msc[XK_KP_Left              & 0xFF] = KEY_KP_4;
  _keyMap.msc[XK_KP_Begin             & 0xFF] = KEY_KP_5;
  _keyMap.msc[XK_KP_Right             & 0xFF] = KEY_KP_6;
  _keyMap.msc[XK_KP_Home              & 0xFF] = KEY_KP_7;
  _keyMap.msc[XK_KP_Up                & 0xFF] = KEY_KP_8;
  _keyMap.msc[XK_KP_Page_Up           & 0xFF] = KEY_KP_9;
  _keyMap.msc[XK_KP_Delete            & 0xFF] = KEY_KP_PERIOD;
  _keyMap.msc[XK_KP_Decimal           & 0xFF] = KEY_KP_PERIOD;
  _keyMap.msc[XK_KP_Divide            & 0xFF] = KEY_KP_DIVIDE;
  _keyMap.msc[XK_KP_Multiply          & 0xFF] = KEY_KP_MULTIPLY;
  _keyMap.msc[XK_KP_Subtract          & 0xFF] = KEY_KP_MINUS;
  _keyMap.msc[XK_KP_Add               & 0xFF] = KEY_KP_PLUS;
  _keyMap.msc[XK_KP_Enter             & 0xFF] = KEY_KP_ENTER;
  _keyMap.msc[XK_KP_Equal             & 0xFF] = KEY_KP_EQUALS;

  _keyMap.msc[XK_Up                   & 0xFF] = KEY_UP;
  _keyMap.msc[XK_Down                 & 0xFF] = KEY_DOWN;
  _keyMap.msc[XK_Right                & 0xFF] = KEY_RIGHT;
  _keyMap.msc[XK_Left                 & 0xFF] = KEY_LEFT;
  _keyMap.msc[XK_Insert               & 0xFF] = KEY_INSERT;
  _keyMap.msc[XK_Home                 & 0xFF] = KEY_HOME;
  _keyMap.msc[XK_End                  & 0xFF] = KEY_END;
  _keyMap.msc[XK_Page_Up              & 0xFF] = KEY_PAGE_UP;
  _keyMap.msc[XK_Page_Down            & 0xFF] = KEY_PAGE_DOWN;

  _keyMap.msc[XK_F1                   & 0xFF] = KEY_F1;
  _keyMap.msc[XK_F2                   & 0xFF] = KEY_F2;
  _keyMap.msc[XK_F3                   & 0xFF] = KEY_F3;
  _keyMap.msc[XK_F4                   & 0xFF] = KEY_F4;
  _keyMap.msc[XK_F5                   & 0xFF] = KEY_F5;
  _keyMap.msc[XK_F6                   & 0xFF] = KEY_F6;
  _keyMap.msc[XK_F7                   & 0xFF] = KEY_F7;
  _keyMap.msc[XK_F8                   & 0xFF] = KEY_F8;
  _keyMap.msc[XK_F9                   & 0xFF] = KEY_F9;
  _keyMap.msc[XK_F10                  & 0xFF] = KEY_F10;
  _keyMap.msc[XK_F11                  & 0xFF] = KEY_F11;
  _keyMap.msc[XK_F12                  & 0xFF] = KEY_F12;
  _keyMap.msc[XK_F13                  & 0xFF] = KEY_F13;
  _keyMap.msc[XK_F14                  & 0xFF] = KEY_F14;
  _keyMap.msc[XK_F15                  & 0xFF] = KEY_F15;

  _keyMap.msc[XK_Num_Lock             & 0xFF] = KEY_NUM_LOCK;
  _keyMap.msc[XK_Caps_Lock            & 0xFF] = KEY_CAPS_LOCK;
  _keyMap.msc[XK_Scroll_Lock          & 0xFF] = KEY_SCROLL_LOCK;
  _keyMap.msc[XK_Shift_L              & 0xFF] = KEY_LEFT_SHIFT;
  _keyMap.msc[XK_Shift_R              & 0xFF] = KEY_RIGHT_SHIFT;
  _keyMap.msc[XK_Control_L            & 0xFF] = KEY_LEFT_CTRL;
  _keyMap.msc[XK_Control_R            & 0xFF] = KEY_RIGHT_CTRL;
  _keyMap.msc[XK_Alt_L                & 0xFF] = KEY_LEFT_ALT;
  _keyMap.msc[XK_Alt_R                & 0xFF] = KEY_RIGHT_ALT;
  _keyMap.msc[XK_Meta_L               & 0xFF] = KEY_LEFT_META;
  _keyMap.msc[XK_Meta_R               & 0xFF] = KEY_RIGHT_META;
  _keyMap.msc[XK_Super_L              & 0xFF] = KEY_LEFT_SUPER;
  _keyMap.msc[XK_Super_R              & 0xFF] = KEY_RIGHT_SUPER;
  _keyMap.msc[XK_Mode_switch          & 0xFF] = KEY_MODE;
  _keyMap.msc[XK_Multi_key            & 0xFF] = KEY_COMPOSE;

  _keyMap.msc[XK_Help                 & 0xFF] = KEY_HELP;
  _keyMap.msc[XK_Print                & 0xFF] = KEY_PRINT;
  _keyMap.msc[XK_Sys_Req              & 0xFF] = KEY_SYS_REQ;
  _keyMap.msc[XK_Break                & 0xFF] = KEY_BREAK;
  _keyMap.msc[XK_Menu                 & 0xFF] = KEY_MENU;
  _keyMap.msc[XK_Hyper_R              & 0xFF] = KEY_MENU;

  updateKeyboardInfo();
}

void X11UIEngine::initMouse()
{
  _mouseInfo.setWheelLines(X11UIEngine_getMouseWheelLines(this));

  updateMouseInfo();
}

// ============================================================================
// [Fog::X11UIEngine - Display / Palette]
// ============================================================================

void X11UIEngine::updateDisplayInfo()
{
  _displayInfo._size.w = _XLib._XDisplayWidth(_display, _screen);
  _displayInfo._size.h = _XLib._XDisplayHeight(_display, _screen);
  _displayInfo._depth = _XLib._XDefaultDepth(_display, _screen);

  _displayInfo._aMask = FOG_UINT64_C(0);
  _displayInfo._rMask = _visual->red_mask;
  _displayInfo._gMask = _visual->green_mask;
  _displayInfo._bMask = _visual->blue_mask;

  _displayInfo._is16BppSwapped = false;
}

// ============================================================================
// [Fog::X11UIEngine - Keyboard / Mouse]
// ============================================================================

void X11UIEngine::updateKeyboardInfo()
{
}

void X11UIEngine::updateMouseInfo()
{
}

void X11UIEngine::setMouseWheelLines(uint32_t lines)
{
  if (lines == 0)
    lines = X11UIEngine_getMouseWheelLines(this);

  Base::setMouseWheelLines(lines);
}

uint32_t X11UIEngine::getFogKeyFromXKeySym(KeySym xKeySym) const
{
  uint key = 0;

  if (xKeySym == 0)
    return key;

  switch (xKeySym >> 8)
  {
    case 0x00: // Latin 1
    case 0x01: // Latin 2
    case 0x02: // Latin 3
    case 0x03: // Latin 4
    case 0x04: // Katakana
    case 0x05: // Arabic
    case 0x06: // Cyrillic
    case 0x07: // Greek
    case 0x08: // Technical
    case 0x0A: // Publishing
    case 0x0C: // Hebrew
    case 0x0D: // Thai
      key = CharA::toLower((uint8_t)(xKeySym & 0xFF));
      break;

    case 0xFE:
      key = _keyMap.odd[xKeySym & 0xFF];
      break;

    case 0xFF:
      key = _keyMap.msc[xKeySym & 0xFF];
      break;

    case 0x1005FF:
#if defined(SunXK_F36)
      if (xKeySym == SunXK_F36)
        key = KEY_F11;
#endif // SunXK_F36
#if defined(SunXK_F37)
      if (xKeySym == SunXK_F37)
        key = KEY_F12;
#endif // SunXK_F37
      break;

    // Unknown key
    default:
      break;
  }

  return key;
}

uint32_t X11UIEngine::getFogButtonFromXButton(uint xButton) const
{
  switch (xButton)
  {
    case 1:
      return BUTTON_LEFT;

    case 2:
      if (_numButtons >= 3)
        return BUTTON_MIDDLE;
      // ... Fall through ...

    case 3:
      return BUTTON_RIGHT;

    case 4:
      return BUTTON_WHEEL_UP;

    case 5:
      return BUTTON_WHEEL_DOWN;

    default:
      return BUTTON_NONE;
  }
}

// ============================================================================
// [Fog::X11UIEngine - DoPaint]
// ============================================================================

void X11UIEngine::doPaintWindow(UIEngineWindow* window, Painter* painter, const RectI& paintRect)
{
  Base::doPaintWindow(window, painter, paintRect);
}

// ============================================================================
// [Fog::X11UIEngine - DoBlit]
// ============================================================================

static void X11UIEngine_setGCRegion(X11UIEngine* self, GC gc, int xOrigin, int yOrigin, Region& region)
{
  size_t regionLength = region.getLength();
  const BoxI* regionData = region.getData();

  MemBufferTmp<1024> xRectBuffer;
  XRectangle* xRectList = static_cast<XRectangle*>(xRectBuffer.alloc(regionLength * sizeof(XRectangle)));

  for (size_t i = 0; i < regionLength; i++)
  {
    xRectList[i].x      = short(regionData[i].x0);
    xRectList[i].y      = short(regionData[i].y0);
    xRectList[i].width  = short(regionData[i].getWidth());
    xRectList[i].height = short(regionData[i].getHeight());
  }

  self->_XLib._XSetClipRectangles(self->_display, gc, xOrigin, yOrigin, xRectList, (uint)regionLength, YXBanded);
}

void X11UIEngine::doBlitWindow(UIEngineWindow* window)
{
  X11UIEngineWindowImpl* d = reinterpret_cast<X11UIEngineWindowImpl*>(window->_d);

  if (!d->_bufferData._size.isValid())
    return;

  if (d->_blitRegion.isEmpty())
    return;

  // --------------------------------------------------------------------------
  // [GC Clip]
  // --------------------------------------------------------------------------

  XID wnd = (XID)d->_handle;
  uint cw = uint(d->_clientGeometry.w);
  uint ch = uint(d->_clientGeometry.h);

  GC gc = d->_gc;
  X11UIEngine_setGCRegion(this, gc, 0, 0, d->_blitRegion);

  // --------------------------------------------------------------------------
  // [GC Blit]
  // --------------------------------------------------------------------------

  switch (d->_bufferType)
  {
    case UI_ENGINE_BUFFER_X11_XIMAGE:
    {
      _XLib._XPutImage(_display, wnd, gc, d->_ximage, 0, 0, 0, 0, cw, ch);
      break;
    }

    case UI_ENGINE_BUFFER_X11_XSHMIMAGE:
    {
      _XExt._XShmPutImage(_display, wnd, gc, d->_ximage, 0, 0, 0, 0, cw, ch, false);
      break;
    }

    default:
      break;
  }
}

// ============================================================================
// [Fog::X11UIEngine - Window Management]
// ============================================================================

err_t X11UIEngine::createWindow(UIEngineWindow* window, uint32_t flags)
{
  X11UIEngineWindowImpl* wImpl = fog_new X11UIEngineWindowImpl(this, window);
  if (FOG_IS_NULL(wImpl))
    return ERR_RT_OUT_OF_MEMORY;

  err_t err = wImpl->create(flags);
  if (FOG_IS_ERROR(err))
    fog_delete(wImpl);

  return err;
}

err_t X11UIEngine::destroyWindow(UIEngineWindow* window)
{
  X11UIEngineWindowImpl* wImpl = static_cast<X11UIEngineWindowImpl*>(window->_d);
  if (FOG_IS_NULL(wImpl))
    return ERR_RT_INVALID_STATE;

  fog_delete(wImpl);
  return ERR_OK;
}

// ============================================================================
// [Fog::X11UIEngine - X11 - WM]
// ============================================================================

XID X11UIEngine::getWMClientLeader()
{
  if (_wmClientLeader)
    return _wmClientLeader;

  XSetWindowAttributes attr;
  long clientLeader = _XLib._XCreateWindow(
    _display, _root, -1, -1, 1, 1, 0, CopyFromParent, InputOutput, CopyFromParent, 0UL, &attr);

  _XLib._XChangeProperty(
    _display, clientLeader, getAtom(X11_ATOM_WM_CLIENT_LEADER),
    XA_WINDOW, 32, PropModeReplace, (unsigned char* )&clientLeader, 1);

  _wmClientLeader = static_cast<XID>(clientLeader);
  return _wmClientLeader;
}

// ============================================================================
// [Fog::X11UIEngine - Misc]
// ============================================================================

void X11UIEngine::sendXClientMessage(XWindow window, long mask, long l0, long l1, long l2, long l3, long l4)
{
  XClientMessageEvent xe;

  xe.type = XClientMessage;
  xe.window = window;
  xe.message_type = getAtom(X11_ATOM_WM_PROTOCOLS);
  xe.format = 32;
  xe.data.l[0] = l0;
  xe.data.l[1] = l1;
  xe.data.l[2] = l2;
  xe.data.l[3] = l3;
  xe.data.l[4] = l4;

  _XLib._XSendEvent(_display, window, false, mask, reinterpret_cast<XEvent*>(&xe));
}

} // Fog namespace

// ============================================================================
// [Library]
// ============================================================================

#if defined(FOG_BUILD_UI_X11_MODULE)
extern "C" FOG_UI_X11_API void* FOG_CDECL ctor(void)
{
  return static_cast<void*>(fog_new Fog::X11UIEngine());
}
#endif // FOG_BUILD_UI_X11_MODULE
