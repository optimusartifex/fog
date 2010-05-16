// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Application.h>
#include <Fog/Core/Byte.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/GuiEngine/X11.h>
#include <Fog/Gui/Widget.h>

// [Shared memory and IPC]
#include <errno.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// [gethostname() / uname()]
#define FOG_HAVE_GETHOSTNAME

#include <sys/utsname.h>
#include <unistd.h>

FOG_IMPLEMENT_OBJECT(Fog::X11GuiEngine)
FOG_IMPLEMENT_OBJECT(Fog::X11GuiWindow)

// Enabled by default.
#define FOG_X11BACKBUFFER_FORCE_PIXMAP

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

#define GUI_ENGINE() \
  reinterpret_cast<X11GuiEngine*>(Application::getInstance()->getGuiEngine())

// ============================================================================
// [Fog::X11GuiEngine - Error]
// ============================================================================

static int X11_IOErrorHandler(XDisplay* d)
{
  fog_fail("Fog::X11GuiEngine::IOErrorHandler() - Fatal error");

  // be quite
  return 0;
}

static int X11_ErrorHandler(XDisplay* d, XErrorEvent* e)
{
  char buffer[256];
  GUI_ENGINE()->pXGetErrorText(d, e->error_code, buffer, 256);

  fog_stderr_msg("Fog::UIsystemX11", "ErrorHandler", "%s", buffer);
  return 0;
}

// ============================================================================
// [Fog::X11GuiEngine - Functions in libX11, libXext, libXrender]
// ============================================================================

// Function names in libX
static const char X11_xlibFunctionNames[] =
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
  "XPending\0"
  "XNextEvent\0"
  "XCheckTypedEvent\0"
  "XCheckTypedWindowEvent\0"
  "XCheckWindowEvent\0"
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
  "XSendEvent\0"
  "XChangeProperty\0"
  "XSetSelectionOwner\0"
  "XGetAtomName\0"
  "XConvertSelection\0"
  "XGetWindowProperty\0"
  "XDeleteProperty\0"
};

// Function names in libXext
static const char X11_xextFunctionNames[] =
{
  "XShmQueryVersion\0"
  "XShmAttach\0"
  "XShmDetach\0"
  "XShmCreateImage\0"
  "XShmCreatePixmap\0"
};

//Function names in libXrender
static const char X11_xrenderFunctionNames[] =
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
// [Fog::X11GuiEngine - Atoms]
// ============================================================================

// XAtom names, order specified in Wde/Gui/Application.h
static const char *X11_atomNames[X11GuiEngine::Atom_Count] =
{
  // WM events
  "WM_PROTOCOLS",                 /* 0  */
  "WM_DELETE_WINDOW",             /* 1  */
  "WM_TAKE_FOCUS",                /* 2  */
  "WM_CLASS",                     /* 3  */
  "WM_NAME",                      /* 4  */
  "WM_COMMAND",                   /* 5  */
  "WM_ICON_NAME",                 /* 6  */
  "WM_CHANGE_STATE",              /* 7  */
  "WM_CLIENT_LEADER",             /* 8  */

  // motif WM hints
  "_MOTIF_WM_HINTS",              /* 9  */

  "_WIN_LAYER",                   /* 10  */

  // Drag and drop atoms
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

  // standartized NET atoms
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

  // Clipboard
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
// [Fog::X11GuiEngine - Helpers]
// ============================================================================

static void X11GuiEngine_sendClientMessage(X11GuiEngine* engine, XWindow win, long mask, long l0, long l1, long l2, long l3, long l4)
{
  XClientMessageEvent xe;

  xe.type = XClientMessage;
  xe.window = win;
  xe.message_type = engine->getAtom(X11GuiEngine::Atom_WM_PROTOCOLS);
  xe.format = 32;
  xe.data.l[0] = l0;
  xe.data.l[1] = l1;
  xe.data.l[2] = l2;
  xe.data.l[3] = l3;
  xe.data.l[4] = l4;

  engine->pXSendEvent(engine->getDisplay(), win, false, mask, (XEvent *)&xe);
}

// ============================================================================
// [Fog::X11GuiEngine - Registration]
// ============================================================================

void X11GuiEngine::registerGuiEngine()
{
  String uiX11(Ascii8("Gui.X11"));
  Application::registerGuiEngineType<X11GuiEngine>(uiX11);
}

// ============================================================================
// [Fog::X11GuiEngine - Construction / Destruction]
// ============================================================================

X11GuiEngine::X11GuiEngine()
{
  err_t err;

  // Clear all members
  Memory::zero(&_atoms, sizeof(_atoms));
  Memory::zero(&_xKeymap, sizeof(_xKeymap));

  _display = NULL;
  _fd = -1;
  _screen = -1;
  _visual = NULL;
  _colormap = 0;
  _root = 0;
  _gc = 0;
  _xShm = false;
  _xPrivateColormap = false;
  _xim = false;
  _xButtons = 0;
  _wakeUpPipe[0] = -1;
  _wakeUpPipe[1] = -1;

  _wmClientLeader = 0;

  // Open X11, Xext and Xrender libraries.
  if ( (err = loadLibraries()) )
  {
    fog_stderr_msg("Fog::X11GuiEngine", "X11GuiEngine", "Can't load X11 libraries.");
    return;
  }

  // X locale support.
  if (!pXSupportsLocale())
  {
    fog_stderr_msg("Fog::X11GuiEngine", "X11GuiEngine", "X does not support locale.");
  }
  else
  {
    char* localeModifiers;
    if ( !(localeModifiers = pXSetLocaleModifiers("")) )
    {
      fog_stderr_msg("Fog::X11GuiEngine", "X11GuiEngine", "Can't set X locale modifiers.");
    }
  }

  if ((_display = pXOpenDisplay("")) == NULL)
  {
    fog_stderr_msg("Fog::X11GuiEngine", "X11GuiEngine", "Can't open display.");
    err = ERR_GUI_CANT_OPEN_DISPLAY;
    return;
  }

  // Set error handlers.
  pXSetIOErrorHandler(&X11_IOErrorHandler);
  pXSetErrorHandler(&X11_ErrorHandler);

  // Setup X11 variables.
  _fd               = ConnectionNumber (_display);
  _screen           = pXDefaultScreen  (_display);
  _visual           = pXDefaultVisual  (_display, _screen);
  _colormap         = pXDefaultColormap(_display, _screen);
  _root             = pXRootWindow     (_display, _screen);
  _gc               = pXCreateGC       (_display, _root, 0, NULL);
  _xPrivateColormap = false;

  // Create wakeup pipe.
  if (pipe(_wakeUpPipe) < 0)
  {
    fog_debug("Fog::X11GuiEngine::X11GuiEngine() - Can't create wakeup pipe (errno=%d).", errno);
    err = ERR_GUI_CANT_CREATE_PIPE;
    goto fail;
  }

  // Now we can setup display info. We must do it here, because we must setup
  // colormap if depth is too low (8 bit or less)
  updateDisplayInfo();

  // Intern atoms.
  _atomNames = X11_atomNames;
  pXInternAtoms(_display, (char **)X11_atomNames, Atom_Count, False, _atoms);

  // Alloc colormap for 4, 8 bit depth
  if (_displayInfo.depth <= 8 && !createColormap())
  {
    fog_debug("Fog::X11GuiEngine::X11GuiEngine() - Can't create colormap.");
    err = ERR_GUI_CANT_CREATE_COLORMAP;
    goto fail;
  }

  // Open XIM.
  _xim = pXOpenIM(_display, NULL, NULL, NULL);

  // Are SHM pixmaps supported?
  {
    int major, minor;
    XBool pixmaps;

    if (pXShmQueryVersion(_display, &major, &minor, &pixmaps))
    {
      _xShm = pixmaps;
    }
  }

  initKeyboard();
  initMouse();

  // Finally add the event loop type into application. Event loop will be
  // instantiated by application after GuiEngine was properly constructed.
  Application::registerEventLoopType<X11GuiEventLoop>(Ascii8("Gui.X11"));

  _initialized = true;
  return;

fail:
  pXCloseDisplay(_display);
}

X11GuiEngine::~X11GuiEngine()
{
  // We don't want that event loop is available after X11GuiEngine was destroyed.
  Application::unregisterEventLoop(Fog::Ascii8("Gui.X11"));

  // Close display and free X resources.
  if (_display)
  {
    if (_xim) pXCloseIM(_xim);
    pXFreeGC(_display, _gc);
    if (hasXPrivateColormap()) pXFreeColormap(_display, _colormap);

    pXCloseDisplay(_display);
  }

  if (_wakeUpPipe[0]) close(_wakeUpPipe[0]);
  if (_wakeUpPipe[1]) close(_wakeUpPipe[1]);
}

// ============================================================================
// [Fog::X11GuiEngine - Display]
// ============================================================================

void X11GuiEngine::updateDisplayInfo()
{
  _displayInfo.width = pXDisplayWidth(_display, _screen);
  _displayInfo.height = pXDisplayHeight(_display, _screen);

  _displayInfo.depth = pXDefaultDepth(_display, _screen);
  _displayInfo.rMask = _visual->red_mask;
  _displayInfo.gMask = _visual->green_mask;
  _displayInfo.bMask = _visual->blue_mask;
  _displayInfo.is16BitSwapped = false;
}

// ============================================================================
// [Fog::X11GuiEngine - Update]
// ============================================================================

void X11GuiEngine::doUpdate()
{
  BaseGuiEngine::doUpdate();
  pXFlush(_display);
}

void X11GuiEngine::doBlitWindow(GuiWindow* window, const IntBox* rects, sysuint_t count)
{
  reinterpret_cast<X11GuiBackBuffer*>(window->_backingStore)->blitRects(
    (XID)window->getHandle(), rects, count);
}

// ============================================================================
// [Fog::X11GuiEngine - X11 Keyboard]
// ============================================================================

void X11GuiEngine::initKeyboard()
{
  // Initialize key translation tables.
  Memory::zero(&_xKeymap, sizeof(_xKeymap));

  // 0xFE
  _xKeymap.odd[32] = KEY_TAB; // TAB

  _xKeymap.odd[XK_dead_grave            & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_acute            & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_tilde            & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_macron           & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_breve            & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_abovedot         & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_diaeresis        & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_abovering        & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_doubleacute      & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_caron            & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_cedilla          & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_ogonek           & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_iota             & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_voiced_sound     & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_semivoiced_sound & 0xFF] = KEY_COMPOSE;
  _xKeymap.odd[XK_dead_belowdot         & 0xFF] = KEY_COMPOSE;
#ifdef XK_dead_hook
  _xKeymap.odd[XK_dead_hook             & 0xFF] = KEY_COMPOSE;
#endif
#ifdef XK_dead_horn
  _xKeymap.odd[XK_dead_horn             & 0xFF] = KEY_COMPOSE;
#endif

#ifdef XK_dead_circumflex
  _xKeymap.odd[XK_dead_circumflex       & 0xFF] = KEY_CARET;
#endif

#ifdef XK_ISO_Level3_Shift
  // _xKeymap.odd[XK_ISO_Level3_Shift   & 0xFF] = KEY_MOVE;
#endif

  // 0xFF
  _xKeymap.misc[XK_BackSpace            & 0xFF] = KEY_BACKSPACE;
  _xKeymap.misc[XK_Tab                  & 0xFF] = KEY_TAB;
  _xKeymap.misc[XK_Clear                & 0xFF] = KEY_CLEAR;
  _xKeymap.misc[XK_Return               & 0xFF] = KEY_ENTER;
  _xKeymap.misc[XK_Pause                & 0xFF] = KEY_PAUSE;
  _xKeymap.misc[XK_Escape               & 0xFF] = KEY_ESC;
  _xKeymap.misc[XK_Delete               & 0xFF] = KEY_DELETE;

  _xKeymap.misc[XK_KP_0                 & 0xFF] = KEY_KP_0;
  _xKeymap.misc[XK_KP_1                 & 0xFF] = KEY_KP_1;
  _xKeymap.misc[XK_KP_2                 & 0xFF] = KEY_KP_2;
  _xKeymap.misc[XK_KP_3                 & 0xFF] = KEY_KP_3;
  _xKeymap.misc[XK_KP_4                 & 0xFF] = KEY_KP_4;
  _xKeymap.misc[XK_KP_5                 & 0xFF] = KEY_KP_5;
  _xKeymap.misc[XK_KP_6                 & 0xFF] = KEY_KP_6;
  _xKeymap.misc[XK_KP_7                 & 0xFF] = KEY_KP_7;
  _xKeymap.misc[XK_KP_8                 & 0xFF] = KEY_KP_8;
  _xKeymap.misc[XK_KP_9                 & 0xFF] = KEY_KP_9;
  _xKeymap.misc[XK_KP_Insert            & 0xFF] = KEY_KP_0;
  _xKeymap.misc[XK_KP_End               & 0xFF] = KEY_KP_1;
  _xKeymap.misc[XK_KP_Down              & 0xFF] = KEY_KP_2;
  _xKeymap.misc[XK_KP_Page_Down         & 0xFF] = KEY_KP_3;
  _xKeymap.misc[XK_KP_Left              & 0xFF] = KEY_KP_4;
  _xKeymap.misc[XK_KP_Begin             & 0xFF] = KEY_KP_5;
  _xKeymap.misc[XK_KP_Right             & 0xFF] = KEY_KP_6;
  _xKeymap.misc[XK_KP_Home              & 0xFF] = KEY_KP_7;
  _xKeymap.misc[XK_KP_Up                & 0xFF] = KEY_KP_8;
  _xKeymap.misc[XK_KP_Page_Up           & 0xFF] = KEY_KP_9;
  _xKeymap.misc[XK_KP_Delete            & 0xFF] = KEY_KP_PERIOD;
  _xKeymap.misc[XK_KP_Decimal           & 0xFF] = KEY_KP_PERIOD;
  _xKeymap.misc[XK_KP_Divide            & 0xFF] = KEY_KP_DIVIDE;
  _xKeymap.misc[XK_KP_Multiply          & 0xFF] = KEY_KP_MULTIPLY;
  _xKeymap.misc[XK_KP_Subtract          & 0xFF] = KEY_KP_MINUS;
  _xKeymap.misc[XK_KP_Add               & 0xFF] = KEY_KP_PLUS;
  _xKeymap.misc[XK_KP_Enter             & 0xFF] = KEY_KP_ENTER;
  _xKeymap.misc[XK_KP_Equal             & 0xFF] = KEY_KP_EQUALS;

  _xKeymap.misc[XK_Up                   & 0xFF] = KEY_UP;
  _xKeymap.misc[XK_Down                 & 0xFF] = KEY_DOWN;
  _xKeymap.misc[XK_Right                & 0xFF] = KEY_RIGHT;
  _xKeymap.misc[XK_Left                 & 0xFF] = KEY_LEFT;
  _xKeymap.misc[XK_Insert               & 0xFF] = KEY_INSERT;
  _xKeymap.misc[XK_Home                 & 0xFF] = KEY_HOME;
  _xKeymap.misc[XK_End                  & 0xFF] = KEY_END;
  _xKeymap.misc[XK_Page_Up              & 0xFF] = KEY_PAGE_UP;
  _xKeymap.misc[XK_Page_Down            & 0xFF] = KEY_PAGE_DOWN;

  _xKeymap.misc[XK_F1                   & 0xFF] = KEY_F1;
  _xKeymap.misc[XK_F2                   & 0xFF] = KEY_F2;
  _xKeymap.misc[XK_F3                   & 0xFF] = KEY_F3;
  _xKeymap.misc[XK_F4                   & 0xFF] = KEY_F4;
  _xKeymap.misc[XK_F5                   & 0xFF] = KEY_F5;
  _xKeymap.misc[XK_F6                   & 0xFF] = KEY_F6;
  _xKeymap.misc[XK_F7                   & 0xFF] = KEY_F7;
  _xKeymap.misc[XK_F8                   & 0xFF] = KEY_F8;
  _xKeymap.misc[XK_F9                   & 0xFF] = KEY_F9;
  _xKeymap.misc[XK_F10                  & 0xFF] = KEY_F10;
  _xKeymap.misc[XK_F11                  & 0xFF] = KEY_F11;
  _xKeymap.misc[XK_F12                  & 0xFF] = KEY_F12;
  _xKeymap.misc[XK_F13                  & 0xFF] = KEY_F13;
  _xKeymap.misc[XK_F14                  & 0xFF] = KEY_F14;
  _xKeymap.misc[XK_F15                  & 0xFF] = KEY_F15;

  _xKeymap.misc[XK_Num_Lock             & 0xFF] = KEY_NUM_LOCK;
  _xKeymap.misc[XK_Caps_Lock            & 0xFF] = KEY_CAPS_LOCK;
  _xKeymap.misc[XK_Scroll_Lock          & 0xFF] = KEY_SCROLL_LOCK;
  _xKeymap.misc[XK_Shift_L              & 0xFF] = KEY_LEFT_SHIFT;
  _xKeymap.misc[XK_Shift_R              & 0xFF] = KEY_RIGHT_SHIFT;
  _xKeymap.misc[XK_Control_L            & 0xFF] = KEY_LEFT_CTRL;
  _xKeymap.misc[XK_Control_R            & 0xFF] = KEY_RIGHT_CTRL;
  _xKeymap.misc[XK_Alt_L                & 0xFF] = KEY_LEFT_ALT;
  _xKeymap.misc[XK_Alt_R                & 0xFF] = KEY_RIGHT_ALT;
  _xKeymap.misc[XK_Meta_L               & 0xFF] = KEY_LEFT_META;
  _xKeymap.misc[XK_Meta_R               & 0xFF] = KEY_RIGHT_META;
  _xKeymap.misc[XK_Super_L              & 0xFF] = KEY_LEFT_SUPER;
  _xKeymap.misc[XK_Super_R              & 0xFF] = KEY_RIGHT_SUPER;
  _xKeymap.misc[XK_Mode_switch          & 0xFF] = KEY_MODE;
  _xKeymap.misc[XK_Multi_key            & 0xFF] = KEY_COMPOSE;

  _xKeymap.misc[XK_Help                 & 0xFF] = KEY_HELP;
  _xKeymap.misc[XK_Print                & 0xFF] = KEY_PRINT;
  _xKeymap.misc[XK_Sys_Req              & 0xFF] = KEY_SYS_REQ;
  _xKeymap.misc[XK_Break                & 0xFF] = KEY_BREAK;
  _xKeymap.misc[XK_Menu                 & 0xFF] = KEY_MENU;
  _xKeymap.misc[XK_Hyper_R              & 0xFF] = KEY_MENU;
}

uint32_t X11GuiEngine::translateXSym(KeySym xsym) const
{
  uint key = 0;

  if (xsym == 0) return 0;

  switch (xsym >> 8)
  {
    case 0x1005FF:
      key = 0;
#ifdef SunXK_F36
      if (xsym == SunXK_F36) key = KeyF11;
#endif // SunXK_F36
#ifdef SunXK_F37
      if (xsym == SunXK_F37) key = KeyF12;
#endif // SunXK_F37
      break;

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
      key = Byte::toLower((uint8_t)(xsym & 0xFF));
      break;
    case 0xFE:
      key = _xKeymap.odd[xsym & 0xFF];
      break;
    case 0xFF:
      key = _xKeymap.misc[xsym & 0xFF];
      break;

    // Unknown key
    default:
      break;
  }

  return key;
}

// ============================================================================
// [Fog::X11GuiEngine - X11 Mouse]
// ============================================================================

void X11GuiEngine::initMouse()
{
  // Get count of mouse buttons.
  uint8_t pm[5];
  _xButtons = pXGetPointerMapping(_display, pm, 5);
}

uint32_t X11GuiEngine::translateButton(uint x11Button) const
{
  switch (x11Button)
  {
    case 1:
      return BUTTON_LEFT;
    case 2:
      if (_xButtons < 3)
        return BUTTON_RIGHT;
      else
        return BUTTON_MIDDLE;
    case 3:
      return BUTTON_RIGHT;
    case 4:
      return WHEEL_UP;
    case 5:
      return WHEEL_DOWN;
    default:
      return 0;
  }
}

// ============================================================================
// [Fog::X11GuiEngine - X11 ColorMap]
// ============================================================================

bool X11GuiEngine::createColormap()
{
  // Synchronize supported parameters with 'PixelConvert'
  struct __PseudoParams
  {
    uint32_t redColors, greenColors, blueColors;
    uint32_t redMask, greenMask, blueMask;
  };

  __PseudoParams p[] =
  {
    { 4, 8, 4, 0x60, 0x1C, 0x03 },
    { 4, 4, 4, 0x30, 0x0C, 0x03 },
    { 2, 2, 2, 0x04, 0x02, 0x01 }
  };

  uint i, v;

  // First try to match some visuals, because some X modes can't
  // alloc requested numbers of colors...
  XVisualInfo vi;
  XVisual* visuals[4];
  XVisual** visualPtr = visuals;
  uint visualsCount;

  // Default or created colormap, see loop
  XColormap colormapToUse;

  // Default visual is the most wanted...
  *visualPtr++ = _visual;

  // Match some interesting visuals
  if (pXMatchVisualInfo(_display, _screen, _displayInfo.depth, DirectColor, &vi))
  {
    *visualPtr++ = vi.visual;
  }

  if (pXMatchVisualInfo(_display, _screen, _displayInfo.depth, PseudoColor, &vi))
  {
    *visualPtr++ = vi.visual;
  }

  visualsCount = (uint)(visualPtr - visuals);

  for (v = 0; v != visualsCount; v++)
  {
    if (v == 0)
    {
      colormapToUse = _colormap;
    }
    else
    {
      colormapToUse = pXCreateColormap(_display, _root, visuals[v], AllocNone);
      if (!colormapToUse) return false;
    }

    for (i = 0; i != FOG_ARRAY_SIZE(p); i++)
    {
      _paletteInfo.palCount = allocRGB(
        colormapToUse,
        p[i].redColors,
        p[i].greenColors,
        p[i].blueColors,
        _paletteInfo.palConv);

      if (_paletteInfo.palCount > 0)
      {
        _displayInfo.rMask = p[i].redMask;
        _displayInfo.gMask = p[i].greenMask;
        _displayInfo.bMask = p[i].blueMask;

        if (v != 0)
        {
          _visual = visuals[v];
          _colormap = colormapToUse;
          _xPrivateColormap = true;
        }
        return true;
      }

    }

    if (v != 0)
    {
      pXFreeColormap(_display, colormapToUse);
    }
  }

  return false;
}

uint X11GuiEngine::allocRGB(XColormap colormap, int nr, int ng, int nb, uint8_t* palconv)
{
  int r, g, b, i;
  int sig_mask = 0;

  for (i = 0; i < _visual->bits_per_rgb; i++)
  {
    sig_mask |= (0x1 << i);
  }
  sig_mask <<= (16 - _visual->bits_per_rgb);

  i = 0;
  for (r = 0; r < nr; r++)
  {
    for (g = 0; g < ng; g++)
    {
      for (b = 0; b < nb; b++)
      {
        XColor xcl;
        XColor xcl_in;
        int val;
        XStatus ret;

        val = (int)((((double)r) / ((nr) - 1)) * 65535.0);
        xcl.red = (unsigned short)(val);
        val = (int)((((double)g) / ((ng) - 1)) * 65535.0);
        xcl.green = (unsigned short)(val);
        val = (int)((((double)b) / ((nb) - 1)) * 65535.0);
        xcl.blue = (unsigned short)(val);
        xcl_in = xcl;

        ret = pXAllocColor(_display, colormap, &xcl);

        if ((ret == 0) ||
          ((xcl_in.red & sig_mask) != (xcl.red & sig_mask)) ||
          ((xcl_in.green & sig_mask) != (xcl.green & sig_mask)) ||
          ((xcl_in.blue & sig_mask) != (xcl.blue & sig_mask)))
        {
          freeRGB(colormap, palconv, i);
          return 0;
        }

        palconv[i++] = xcl.pixel;
      }
    }
  }

  return i;
}

void X11GuiEngine::freeRGB(XColormap colormap, const uint8_t* palconv, uint count)
{
  if (count == 0) return;

  ulong pixels[count];
  for (uint i = 0; i < count; i++) pixels[i] = (ulong)palconv[i];

  pXFreeColors(_display, colormap, pixels, count, 0);
}

// ============================================================================
// [Fog::X11GuiEngine - GuiWindow]
// ============================================================================

GuiWindow* X11GuiEngine::createGuiWindow(Widget* widget)
{
  return new(std::nothrow) X11GuiWindow(widget);
}

void X11GuiEngine::destroyGuiWindow(GuiWindow* native)
{
  delete native;
}

// ============================================================================
// [Fog::X11GuiEngine - X11 API]
// ============================================================================

err_t X11GuiEngine::loadLibraries()
{
  // Load X11.
  if (_xlib.open(Ascii8("X11")) != ERR_OK)
    return ERR_GUI_LIBX11_NOT_LOADED;

  if (_xlib.getSymbols(xlibFunctions,
    X11_xlibFunctionNames, FOG_ARRAY_SIZE(X11_xlibFunctionNames),
    NUM_XLIB_FUNCTIONS,
    (char**)NULL) != NUM_XLIB_FUNCTIONS)
  {
    return ERR_GUI_LIBX11_NOT_LOADED;
  }

  // Load Xext.
  if (_xext.open(Ascii8("Xext")) != ERR_OK)
    return ERR_GUI_LIBEXT_NOT_LOADED;

  if (_xext.getSymbols(xextFunctions,
    X11_xextFunctionNames, FOG_ARRAY_SIZE(X11_xextFunctionNames),
    NUM_XEXT_FUNCTIONS,
    (char**)NULL) != NUM_XEXT_FUNCTIONS)
  {
    return ERR_GUI_LIBEXT_NOT_LOADED;
  }

  // Load Xrender.
  if (_xrender.open(Ascii8("Xrender")) != ERR_OK)
    return ERR_GUI_LIBXRENDER_NOT_LOADED;

  if (_xrender.getSymbols(xrenderFunctions,
    X11_xrenderFunctionNames, FOG_ARRAY_SIZE(X11_xrenderFunctionNames),
    NUM_XRENDER_FUNCTIONS,
    (char**)NULL) != NUM_XRENDER_FUNCTIONS)
  {
    return ERR_GUI_LIBXRENDER_NOT_LOADED;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::X11GuiEngine - WM Support]
// ============================================================================

XID X11GuiEngine::getWmClientLeader()
{
  if (_wmClientLeader) return _wmClientLeader;

  XSetWindowAttributes attr;
  long clientLeader = pXCreateWindow(
    _display, _root,
    -1, -1, 1, 1, 0,
    CopyFromParent, InputOutput, CopyFromParent, 0UL, &attr);

  pXChangeProperty(
    _display, clientLeader, getAtom(Atom_WM_CLIENT_LEADER),
    XA_WINDOW, 32, PropModeReplace, (unsigned char* )&clientLeader, 1);

  _wmClientLeader = clientLeader;
}

// ============================================================================
// [Fog::X11GuiWindow]
// ============================================================================

X11GuiWindow::X11GuiWindow(Widget* widget) :
  BaseGuiWindow(widget),
  _xic(0),
  _inputOnly(false),
  _mapRequest(false),
  _xflags(0)
{
  _backingStore = new(std::nothrow) X11GuiBackBuffer();
}

X11GuiWindow::~X11GuiWindow()
{
  destroy();
  delete _backingStore;
}

err_t X11GuiWindow::create(uint32_t createFlags)
{
  if (_handle) return ERR_GUI_WINDOW_ALREADY_EXISTS;

  X11GuiEngine* engine = GUI_ENGINE();

  XDisplay* display = engine->getDisplay();
  XAtom* atoms = engine->getAtoms();

  int x = _widget->getX();
  int y = _widget->getY();
  int w = _widget->getWidth();
  int h = _widget->getHeight();

  if (w == 0) w++;
  if (h == 0) h++;

  XSetWindowAttributes attr;

  if ((createFlags & WINDOW_X11_PROPERTY_ONLY) == 0)
  {
    ulong attr_mask;

    attr.backing_store = NotUseful;
    attr.override_redirect = (createFlags & WINDOW_X11_OVERRIDE_REDIRECT) != 0;
    attr.colormap = engine->getColormap();
    attr.border_pixel = 0;
    attr.background_pixel = XNone;
    attr.save_under = (createFlags & WINDOW_POPUP) != 0;
    attr.event_mask =
      StructureNotifyMask      | ButtonPressMask    |
      ButtonReleaseMask        | PointerMotionMask  |
      EnterWindowMask          | LeaveWindowMask    |
      KeyPressMask             | KeyReleaseMask     |
      ButtonMotionMask         | ExposureMask       |
      FocusChangeMask          | PropertyChangeMask |
      VisibilityChangeMask;
    attr_mask =
      CWOverrideRedirect       |
      CWSaveUnder              |
      CWBackingStore           |
      CWColormap               |
      CWBorderPixel            |
      CWEventMask;

    _handle = (void*)engine->pXCreateWindow(display,
      engine->getRoot(),
      x, y, w, h,
      0, engine->_displayInfo.depth,
      InputOutput,
      engine->getVisual(),
      attr_mask,
      &attr);
    _inputOnly = false;
  }
  else
  {
    _handle = (void*)engine->pXCreateWindow(display,
      engine->getRoot(),
      x, y, w, h,
      0, CopyFromParent, InputOnly, CopyFromParent, 0, &attr);

    engine->pXSelectInput(display,
      (XID)_handle,
      PropertyChangeMask);
    _inputOnly = true;
  }

  // Create XID <-> GuiWindow* connection.
  engine->mapHandle(_handle, this);

  // Window protocols.
  {
    XAtom protocols[2];

    // WM_DELETE_WINDOW support.
    protocols[0] = atoms[X11GuiEngine::Atom_WM_DELETE_WINDOW];

    // NET_WM_PING support.
    protocols[1] = atoms[X11GuiEngine::Atom_NET_WM_PING];

    engine->pXSetWMProtocols(display, (XID)getHandle(), protocols, 2);
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

    engine->pXSetWMProperties(display, (XID)getHandle(), NULL, NULL, NULL, 0, NULL, NULL, NULL);
  }

  // Set _NET_WM_PID
  long pid = getpid();
  engine->pXChangeProperty(display, (XID)getHandle(), atoms[X11GuiEngine::Atom_NET_WM_PID], XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&pid, 1);

  // Set WM_CLIENT_LEADER
  long clientLeader = engine->getWmClientLeader();
  engine->pXChangeProperty(display, (XID)getHandle(), atoms[X11GuiEngine::Atom_WM_CLIENT_LEADER], XA_WINDOW, 32, PropModeReplace, (unsigned char*)&clientLeader, 1);

  // Get correct window position.
  {
    XWindowAttributes wa;
    engine->pXGetWindowAttributes(display, (XID)getHandle(), &wa);

    _windowRect.set(wa.x, wa.y, (int)wa.width, (int)wa.height);
  }

  // Create X input context.
  if (engine->_xim)
  {
    _xic = engine->pXCreateIC(engine->getXim(),
      XNInputStyle, (XIMPreeditNothing | XIMStatusNothing),
      XNClientWindow, (XID)getHandle(),
      XNFocusWindow, (XID)getHandle(),
      NULL);
  }

  // Windows are enabled by default.
  _enabled = true;
  // Windows are not visible by default.
  _visible = false;
  // Need to blit window content.
  _needBlit = true;
  // Default focus is no focus, X11 will inform us if this gets changed.
  _hasFocus = false;

  // Clear flags, we will wait for events to set them.
  _mapRequest = false;
  _xflags = 0;

  return ERR_OK;
}

err_t X11GuiWindow::destroy()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  X11GuiEngine* engine = GUI_ENGINE();

  engine->pXDestroyIC(_xic);
  engine->pXDestroyWindow(engine->getDisplay(), (XID)getHandle());

  // Destroy XID <-> GuiWindow* connection.
  engine->unmapHandle(_handle);

  // Clear all variables.
  _handle = NULL;
  _hasFocus = false;
  _needBlit = false;
  _mapRequest = false;

  // Flags not needed to save.
  _xflags = 0;

  return ERR_OK;
}

err_t X11GuiWindow::enable()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  // TODO:
  return ERR_OK;
}

err_t X11GuiWindow::disable()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  // TODO:
  return ERR_OK;
}

err_t X11GuiWindow::show()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  X11GuiEngine* engine = GUI_ENGINE();
  engine->pXMapWindow(engine->getDisplay(), (XID)getHandle());

  return ERR_OK;
}

err_t X11GuiWindow::hide()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  X11GuiEngine* engine = GUI_ENGINE();
  engine->pXUnmapWindow(engine->getDisplay(), (XID)getHandle());

  return ERR_OK;
}

err_t X11GuiWindow::move(const IntPoint& pt)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  if (_windowRect.getX() != pt.getX() || _windowRect.getY() != pt.getY())
  {
    X11GuiEngine* engine = GUI_ENGINE();

    // some window managers can change widget move request
    if ((_xflags & XFlag_Configured) == 0)
      setMoveableHints();

    engine->pXMoveWindow(engine->getDisplay(), (XID)getHandle(), pt.getX(), pt.getY());
  }

  return ERR_OK;
}

err_t X11GuiWindow::resize(const IntSize& size)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;
  if (size.getWidth() <= 0 || size.getHeight() <= 0) return ERR_RT_INVALID_ARGUMENT;

  if (_windowRect.getSize() != size)
  {
    X11GuiEngine* engine = GUI_ENGINE();
    engine->pXResizeWindow(engine->getDisplay(), (XID)getHandle(),
      (uint)size.getWidth(), (uint)size.getHeight());
  }

  return ERR_OK;
}

err_t X11GuiWindow::reconfigure(const IntRect& rect)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;
  if (!rect.isValid()) return ERR_RT_INVALID_ARGUMENT;

  if (_windowRect != rect)
  {
    // some window managers can change widget move request
    if ((_xflags & XFlag_Configured) == 0)
      setMoveableHints();

    X11GuiEngine* engine = GUI_ENGINE();
    engine->pXMoveResizeWindow(engine->getDisplay(), (XID)getHandle(),
      rect.getX(),
      rect.getY(),
      rect.getWidth(),
      rect.getHeight());
  }

  return ERR_OK;
}

err_t X11GuiWindow::takeFocus()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  X11GuiEngine* engine = GUI_ENGINE();
  engine->pXSetInputFocus(engine->getDisplay(), (XID)getHandle(), XNone, 0);

  return ERR_OK;
}

err_t X11GuiWindow::setTitle(const String& title)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  err_t err = ERR_OK;

  XTextProperty windowProperty;

#if FOG_SIZEOF_WCHAR_T == 2
  const wchar_t *titleWChar = reinterpret_cast<const wchar_t *>(title.getData());
#else
  TemporaryByteArray<TEMP_LENGTH> titleW;
  if ((err = TextCodec::utf32().appendFromUnicode(titleW, title))) return err;
  const wchar_t *titleWChar = reinterpret_cast<const wchar_t *>(titleW.nullTerminated());
#endif

  X11GuiEngine* engine = GUI_ENGINE();
  int result = engine->pXwcTextListToTextProperty(engine->getDisplay(),
    (wchar_t **)&titleWChar, 1, XTextStyle, &windowProperty);

  if (result == XSuccess)
  {
    engine->pXSetWMName(engine->getDisplay(), (XID)getHandle(), &windowProperty);
    engine->pXFree(windowProperty.value);
    engine->pXSync(engine->getDisplay(), False);

    _title = title;
  }
  else
  {
    err = ERR_GUI_INTERNAL_ERROR;
  }

  return err;
}

err_t X11GuiWindow::getTitle(String& title)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  title = _title;
  return ERR_OK;
}

err_t X11GuiWindow::setIcon(const Image& icon)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  return ERR_RT_NOT_IMPLEMENTED;
}

err_t X11GuiWindow::getIcon(Image& icon)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  return ERR_RT_NOT_IMPLEMENTED;
}

err_t X11GuiWindow::setSizeGranularity(const IntPoint& pt)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  XSizeHints hints;
  Memory::zero(&hints, sizeof(XSizeHints));

  // TODO:
  // hints.flags = PBaseSize;
  // hints.base_width = baseWidth;
  // hints.base_height = baseHeight;

  hints.flags = PResizeInc;
  hints.width_inc = pt.getX();
  hints.height_inc = pt.getY();

  _sizeGranularity = pt;

  X11GuiEngine* engine = GUI_ENGINE();
  engine->pXSetNormalHints(engine->getDisplay(), (XID)getHandle(), &hints);

  return ERR_OK;
}

err_t X11GuiWindow::getSizeGranularity(IntPoint& pt)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  pt = _sizeGranularity;
  return ERR_OK;
}

err_t X11GuiWindow::worldToClient(IntPoint* coords)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  XWindow childRet;
  X11GuiEngine* engine = GUI_ENGINE();
  bool ok = engine->pXTranslateCoordinates(engine->getDisplay(),
    engine->getRoot(), (XID)getHandle(),
    coords->x, coords->y,
    &coords->x, &coords->y,
    &childRet);

  return (ok) ? (err_t)ERR_OK : (err_t)ERR_GUI_CANT_TRANSLETE_COORDINATES;
}

err_t X11GuiWindow::clientToWorld(IntPoint* coords)
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  XWindow childRet;
  X11GuiEngine* engine = GUI_ENGINE();
  bool ok = engine->pXTranslateCoordinates(engine->getDisplay(),
    (XID)getHandle(), engine->getRoot(),
    coords->x, coords->y,
    &coords->x, &coords->y,
    &childRet);

  return (ok) ? (err_t)ERR_OK : (err_t)ERR_GUI_CANT_TRANSLETE_COORDINATES;
}

void X11GuiWindow::onX11Event(XEvent* xe)
{
  X11GuiEngine* engine = GUI_ENGINE();

  switch (xe->xany.type)
  {
    case XCreateNotify:
      fog_debug("Fog::X11GuiWindow::onX11Event() - Create notify");
      break;
    case XDestroyNotify:
      fog_debug("Fog::X11GuiWindow::onX11Event() - Destroy notify");
      break;

    case XMapNotify:
      _mapRequest = false;
      onVisibility(true);
      break;

    case XMapRequest:
      _mapRequest = true;
      onVisibility(false);
      break;

    case XUnmapNotify:
      _mapRequest = false;
      onVisibility(false);
      break;

    case XConfigureRequest:
      fog_debug("Fog::X11GuiWindow::onX11Event() - Configure request: %d %d", xe->xconfigurerequest.x, xe->xconfigurerequest.y);
      break;

    case XConfigureNotify:
    {
      _xflags |= XFlag_Configured;

      // Don't process old configure events.
      while (engine->pXCheckTypedWindowEvent(engine->getDisplay(), xe->xany.window, XConfigureNotify, xe)) ;

      IntRect windowRect(
        xe->xconfigure.x,
        xe->xconfigure.y,
        (int)xe->xconfigure.width,
        (int)xe->xconfigure.height);
      IntRect clientRect(
        0,
        0,
        windowRect.getWidth(),
        windowRect.getHeight());

      onConfigure(windowRect, clientRect);
      break;
    }

    case XFocusIn:
      onFocus(true);
      break;
    
    case XFocusOut:
      onFocus(false);
      break;

    case XKeyPress:
    {
      TemporaryString<8> unicode;

      KeySym xsym = 0;
      uint32_t key;

      if (_xic)
      {
        // Use XIC (X input context) to translate key
        wchar_t buf[32];
        XStatus status;
        int len = engine->pXwcLookupString(_xic,
          (XKeyPressedEvent *)xe,
          buf,
          FOG_ARRAY_SIZE(buf) - 1,
          &xsym,
          &status);

        switch (status)
        {
          case XBufferOverflow:
            fog_debug("Fog::X11GuiWindow::onX11Event() - 'KeyPress', Buffer too small (XIC)");
            // I don't know if this is possible when we have
            // buffer for 31 characters, if this error occurs,
            // we will skip this event.
            return;
          case XLookupChars:
          case XLookupKeySym:
          case XLookupBoth:
            unicode.setWChar(buf, len);
            break;
          default:
            goto __keyPressNoXIC;
        }
      }
      else
      {
__keyPressNoXIC:
        // XIC not supported...?
        char buf[15*6 + 1];
        int len = engine->pXLookupString(&xe->xkey, buf, FOG_ARRAY_SIZE(buf) - 1, &xsym, 0);
        TextCodec::local8().toUnicode(unicode, buf, len);
      }

      key = engine->translateXSym(xsym);
      onKeyPress(key, engine->keyToModifier(key), xe->xkey.keycode,
        Char(unicode.getLength() == 1 ? unicode.at(0).ch() : 0));
      break;
    }

    case XKeyRelease:
    {
      KeySym xsym = engine->pXKeycodeToKeysym(engine->getDisplay(), xe->xkey.keycode, 0);
      uint32_t key = engine->translateXSym(xsym);

      onKeyRelease(key, engine->keyToModifier(key), xe->xkey.keycode, Char(0));
      break;
    }

    case XEnterNotify:
      if (engine->_systemMouseStatus.uiWindow == this &&
          engine->_systemMouseStatus.buttons)
      {
        onMouseMove(xe->xmotion.x, xe->xmotion.y);
      }
      else
      {
        onMouseHover(xe->xmotion.x, xe->xmotion.y);
      }
      break;
    case XLeaveNotify:
      if (engine->_systemMouseStatus.uiWindow == this &&
          engine->_systemMouseStatus.buttons)
      {
        onMouseMove(xe->xmotion.x, xe->xmotion.y);
      }
      else
      {
        onMouseLeave(xe->xmotion.x, xe->xmotion.y);
      }
      break;
    case XMotionNotify:
      onMouseMove(xe->xmotion.x, xe->xmotion.y);
      break;

    case XButtonPress:
    {
      uint button = engine->translateButton(xe->xbutton.button);
      switch (button)
      {
        case BUTTON_LEFT:
        case BUTTON_MIDDLE:
        case BUTTON_RIGHT:
          onMousePress(button, false);
          break;
        case WHEEL_UP:
        case WHEEL_DOWN:
          onMouseWheel(button);
          break;
      }
      break;
    }

    case XButtonRelease:
    {
      uint button = engine->translateButton(xe->xbutton.button);
      switch (button)
      {
        case BUTTON_LEFT:
        case BUTTON_MIDDLE:
        case BUTTON_RIGHT:
          onMouseRelease(button);
          break;
      }
      break;
    }

    case XExpose:
    {
      if (!_isDirty)
      {
        do {
          IntBox box(
            xe->xexpose.x,
            xe->xexpose.y,
            xe->xexpose.x + xe->xexpose.width,
            xe->xexpose.y + xe->xexpose.height);
          reinterpret_cast<X11GuiBackBuffer*>(_backingStore)->blitRects(
            (XID)getHandle(), &box, 1);
        } while (xe->xexpose.count > 0 && engine->pXCheckTypedWindowEvent(engine->getDisplay(), xe->xany.window, XExpose, xe));

        engine->pXFlush(engine->getDisplay());
      }
      else
      {
        // Eat all events, because we will repaint later (in update process).
        while (engine->pXCheckTypedWindowEvent(engine->getDisplay(), xe->xany.window, XExpose, xe)) ;
        _needBlit = true;
      }
      break;
    }

    case XClientMessage:
    {
      // It looks that all client messages should be in 32 bit (long) format.
      if (xe->xclient.format == 32)
      {
        // WM_PROTOCOLS messages
        if (xe->xclient.message_type == engine->getAtom(X11GuiEngine::Atom_WM_PROTOCOLS))
        {
          XAtom msg = (XAtom)xe->xclient.data.l[0];

          if (msg == engine->getAtom(X11GuiEngine::Atom_WM_DELETE_WINDOW))
          {
            CloseEvent e;
            _widget->sendEvent(&e);
          }
          else if (msg == engine->getAtom(X11GuiEngine::Atom_NET_WM_PING))
          {
            if (xe->xclient.window != engine->_root)
            {
              X11GuiEngine_sendClientMessage(
                engine, engine->_root,
                SubstructureNotifyMask | SubstructureRedirectMask,
                xe->xclient.data.l[0],
                xe->xclient.data.l[1],
                xe->xclient.data.l[2],
                xe->xclient.data.l[3],
                xe->xclient.data.l[4]);
            }
          }
        }
      }
      break;
    }

    default:
      break;
  }
}

// tell window manager that we want's to move our window to our position
// (it will be not discarded if we will specify PPosition)
void X11GuiWindow::setMoveableHints()
{
  XSizeHints hints;
  Memory::zero(&hints, sizeof(XSizeHints));
  hints.flags = PPosition;

  X11GuiEngine* engine = GUI_ENGINE();
  engine->pXSetNormalHints(engine->getDisplay(), (XID)getHandle(), &hints);
}
// ============================================================================
// [Fog::X11GuiBackBuffer]
// ============================================================================

X11GuiBackBuffer::X11GuiBackBuffer()
{
  _pixmap = 0;
  Memory::zero(&_shmi, sizeof(_shmi));
  _ximage = NULL;
}

X11GuiBackBuffer::~X11GuiBackBuffer()
{
  destroy();
}

bool X11GuiBackBuffer::resize(int width, int height, bool cache)
{
  X11GuiEngine* engine = GUI_ENGINE();

  int targetWidth = width;
  int targetHeight = height;
  sysint_t targetSize;
  sysint_t targetStride;

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
      if (width <= _cachedWidth && height <= _cachedHeight)
      {
        // Cached.
        _buffer.width = width;
        _buffer.height = height;
        return true;
      }

      // Don't create smaller buffer than previous was!
      targetWidth  = Math::max<int>(width, _cachedWidth);
      targetHeight = Math::max<int>(height, _cachedHeight);

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

      case TYPE_X11_XSHM_PIXMAP:
        engine->pXShmDetach(engine->getDisplay(), &_shmi);
        engine->pXSync(engine->getDisplay(), False);

        shmdt(_shmi.shmaddr);
        shmctl(_shmi.shmid, IPC_RMID, NULL);

        engine->pXFreePixmap(engine->getDisplay(), _pixmap);

        if (_secondaryPixels) Memory::free(_secondaryPixels);
        break;

      case TYPE_X11_XIMAGE:
      case TYPE_X11_XIMAGE_WITH_PIXMAP:
        // We want to free image data ourselves
        Memory::free(_primaryPixels);
        _ximage->data = NULL;

        engine->pXDestroyImage(_ximage);
#if defined(FOG_X11BACKBUFFER_FORCE_PIXMAP)
        if (_type == TYPE_X11_XIMAGE_WITH_PIXMAP)
        {
          engine->pXFreePixmap(engine->getDisplay(), _pixmap);
        }
#endif
        if (_secondaryPixels) Memory::free(_secondaryPixels);
        break;
    }
  }

  // Create image buffer
  if (createImage)
  {
    // correct target BPP, some X server settings can be amazing:-)
    uint targetDepth = engine->_displayInfo.depth;
    if (targetDepth > 4 && targetDepth < 8) targetDepth = 8;
    else if (targetDepth == 15) targetDepth = 16;
    else if (targetDepth == 24) targetDepth = 32;

    targetStride = Image::getStrideFromWidth(targetWidth, targetDepth);
    targetSize = targetStride * targetHeight;

    // TypeXShmPixmap
    if (engine->hasXShm())
    {
      if ((_shmi.shmid = shmget(IPC_PRIVATE, targetSize, IPC_CREAT | IPC_EXCL | 0666)) < 0)
      {
        fog_stderr_msg("Fog::X11GuiBackBuffer", "resize", "shmget() failed: %s", strerror(errno));
        goto __tryImage;
      }

      if ((_shmi.shmaddr = (char *)shmat(_shmi.shmid, NULL, 0)) == NULL)
      {
        fog_stderr_msg("Fog::X11GuiBackBuffer", "resize", "shmat() failed: %s", strerror(errno));
        shmctl(_shmi.shmid, IPC_RMID, NULL);
        goto __tryImage;
      }

      _shmi.readOnly = False;

      // Get the X server to attach this segment to a pixmap
      engine->pXShmAttach(engine->getDisplay(), &_shmi);
      engine->pXSync(engine->getDisplay(), False);

      _pixmap = engine->pXShmCreatePixmap(
        engine->getDisplay(),
        engine->getRoot(),
        _shmi.shmaddr,
        &_shmi,
        targetWidth, targetHeight,
        engine->_displayInfo.depth);

      _type = TYPE_X11_XSHM_PIXMAP;
      _primaryPixels = (uint8_t*)_shmi.shmaddr;
    }

    // TypeXImage
    else
    {
__tryImage:
      // try to alloc image data
      _primaryPixels = (uint8_t*)Memory::alloc(targetSize);
      if (!_primaryPixels)
      {
        fog_stderr_msg("Fog::X11GuiBackBuffer", "resize", "Memory allocation error %s", strerror(errno));
        goto fail;
      }

      // try to create XImage
      _ximage = engine->pXCreateImage(
        engine->getDisplay(),
        engine->getVisual(),
        engine->_displayInfo.depth,
        ZPixmap, 0, (char *)_primaryPixels,
        targetWidth, targetHeight,
        32, targetStride);

      if (!_ximage)
      {
        fog_stderr_msg("Fog::X11GuiBackBuffer", "resize", "XCreateImage() failed");
        Memory::free(_primaryPixels);
        goto fail;
      }
#if defined(FOG_X11BACKBUFFER_FORCE_PIXMAP)
      // this step should be optional, but can increase performance
      // on remote machines (local machines should use XSHM extension)
      _pixmap = engine->pXCreatePixmap(
        engine->getDisplay(),
        engine->getRoot(),
        targetWidth, targetHeight,
        engine->_displayInfo.depth);

      if (_pixmap)
        _type = TYPE_X11_XIMAGE_WITH_PIXMAP;
      else
#endif
        _type = TYPE_X11_XIMAGE;
    }

    if (_type != TYPE_NONE)
    {
      _createdTime = TimeTicks::now();
      _expireTime = _createdTime + TimeDelta::fromSeconds(15);

      _buffer.format = IMAGE_FORMAT_XRGB32;

      _primaryStride = targetStride;
      _cachedWidth = targetWidth;
      _cachedHeight = targetHeight;

      // Now image is created and we must check if we have correct
      // depth and pixel format, if not, we must create secondary
      // buffer that will be used for conversion
      sysint_t secondaryStride = (sysint_t)targetWidth * 4;

      if (targetStride != secondaryStride)
      {
        // Alloc extra buffer.
        _secondaryPixels = (uint8_t*)Memory::alloc(secondaryStride * targetHeight);
        _secondaryStride = secondaryStride;

        if (!_secondaryPixels)
        {
          fog_stderr_msg("Fog::X11GuiBackBuffer", "resize", "Can't create secondary backing store buffer");
        }

        _convertFunc = NULL;
        _convertDepth = targetDepth;

        uint32_t rMask = engine->_displayInfo.rMask;
        uint32_t gMask = engine->_displayInfo.gMask;
        uint32_t bMask = engine->_displayInfo.bMask;

        switch (targetDepth)
        {
          // 8-bit target.
          case 8:
            if (rMask == 0x60 && gMask == 0x1C && bMask == 0x03)
              _convertFunc = (void*)rasterFuncs.dib.i8rgb232_from_xrgb32_dither;
            else if (rMask == 0x30 && gMask == 0x0C && bMask == 0x03)
              _convertFunc = (void*)rasterFuncs.dib.i8rgb222_from_xrgb32_dither;
            else if (rMask == 0x04 && gMask == 0x02 && bMask == 0x01)
              _convertFunc = (void*)rasterFuncs.dib.i8rgb111_from_xrgb32_dither;
            break;

          // 16-bit target.
          case 16:
            if (rMask == 0xF800 && gMask == 0x07E0 && bMask == 0x001F)
            {
              if (engine->_displayInfo.is16BitSwapped)
                _convertFunc = (void*)rasterFuncs.dib.convert[DIB_FORMAT_RGB16_565_NATIVE][IMAGE_FORMAT_XRGB32];
              else
                _convertFunc = (void*)rasterFuncs.dib.convert[DIB_FORMAT_RGB16_565_NATIVE][IMAGE_FORMAT_XRGB32];
            }
            else if (rMask == 0x7C00 && gMask == 0x03E0 && bMask == 0x001F)
            {
              if (engine->_displayInfo.is16BitSwapped)
                _convertFunc = (void*)rasterFuncs.dib.convert[DIB_FORMAT_RGB16_555_NATIVE][IMAGE_FORMAT_XRGB32];
              else
                _convertFunc = (void*)rasterFuncs.dib.convert[DIB_FORMAT_RGB16_555_NATIVE][IMAGE_FORMAT_XRGB32];
            }
#if 0
            if (rMask == 0x7C00 && gMask == 0x03E0 && bMask == 0x001F)
            {
              if (engine->_displayInfo.is16BitSwapped)
                _convertFunc = (void*)rasterFuncs.dib.rgb16_555_swapped_from_xrgb32_dither;
              else
                _convertFunc = (void*)rasterFuncs.dib.rgb16_555_native_from_xrgb32_dither;
            }
            else if (rMask == 0xF800 && gMask == 0x07E0 && bMask == 0x001F)
            {
              if (engine->_displayInfo.is16BitSwapped)
                _convertFunc = (void*)rasterFuncs.dib.rgb16_565_swapped_from_xrgb32_dither;
              else
                _convertFunc = (void*)rasterFuncs.dib.rgb16_565_native_from_xrgb32_dither;
            }
#endif
            break;

          // 24-bit target.
          case 24:
            _convertFunc = (void*)rasterFuncs.dib.convert[DIB_FORMAT_RGB24_NATIVE][IMAGE_FORMAT_XRGB32];
            break;

          // 32-bit target.
          case 32:
            _convertFunc = (void*)rasterFuncs.dib.memcpy32;
            break;
        }

        if (!_convertFunc)
        {
          fog_stderr_msg("Fog::X11GuiBackBuffer", "resize", "Not available converter for %d bit depth", targetDepth);
        }

        _buffer.data = _secondaryPixels;
        _buffer.width = width;
        _buffer.height = height;
        _buffer.stride = _secondaryStride;

        return true;
      }
      else
      {
        _convertFunc = NULL;

        _buffer.data = _primaryPixels;
        _buffer.width = width;
        _buffer.height = height;
        _buffer.stride = _primaryStride;

        // Secondary buffer not used.
        _secondaryPixels = NULL;
        _secondaryStride = 0;

        return true;
      }
    }
  }

fail:
  _clear();
  _pixmap = 0;
  _ximage = NULL;
  return false;
}

void X11GuiBackBuffer::destroy()
{
  resize(0, 0, false);
}

void X11GuiBackBuffer::updateRects(const IntBox* rects, sysuint_t count)
{
  X11GuiEngine* engine = GUI_ENGINE();

  // If there is secondary buffer, we need to convert it to primary
  // one that has same depth and pixel format as X display.
  if (_secondaryPixels && _convertFunc)
  {
    sysuint_t i;

    int bufw = getWidth();
    int bufh = getHeight();

    sysint_t dstStride = _primaryStride;
    sysint_t srcStride = _secondaryStride;

    uint8_t* dstBase = _primaryPixels;
    uint8_t* srcBase = _secondaryPixels;

    const uint8_t* palConv = engine->_paletteInfo.palConv;

    sysint_t dstxmul = _convertDepth >> 3;
    sysint_t srcxmul = 4;

    for (i = 0; i != count; i++)
    {
      int x1 = rects[i].getX1();
      int y1 = rects[i].getY1();
      int x2 = rects[i].getX2();
      int y2 = rects[i].getY2();

      // Apply clip. In rare cases rectangles can contain bigger
      // coordinates that buffers are (reason can be resizing).
      if (x1 < 0) x1 = 0;
      if (y1 < 0) y1 = 0;
      if (x2 > bufw) x2 = bufw;
      if (y2 > bufh) y2 = bufh;

      if (x1 >= x2 || y1 >= y2) continue;

      uint w = (uint)(x2 - x1);

      uint8_t* dstCur = dstBase + y1 * dstStride + x1 * dstxmul;
      uint8_t* srcCur = srcBase + y1 * srcStride + x1 * srcxmul;

      switch (_convertDepth)
      {
        case 8:
          while (y1 < y2)
          {
            ((RasterDither8Fn)_convertFunc)(dstCur, srcCur, w, IntPoint(x1, y1), palConv);

            dstCur += dstStride;
            srcCur += srcStride;
            y1++;
          }
          break;
        case 16:
#if 0
          while (y1 < y2)
          {
            ((RasterDither16Fn)_convertFunc)(dstCur, srcCur, w, IntPoint(x1, y1));

            dstCur += dstStride;
            srcCur += srcStride;
            y1++;
          }
#endif
          while (y1 < y2)
          {
            ((RasterVBlitFullFn)_convertFunc)(dstCur, srcCur, w, NULL);

            dstCur += dstStride;
            srcCur += srcStride;
            y1++;
          }
          break;
        case 24:
        case 32:
          while (y1 < y2)
          {
            ((RasterVBlitFullFn)_convertFunc)(dstCur, srcCur, w, NULL);

            dstCur += dstStride;
            srcCur += srcStride;
            y1++;
          }
          break;
      }
    }
  }

  // Possible secondary step is to put XImage to Pixmap here, because
  // it can increase performance on remote machines. Idea is to do
  // put XImage here instead in blitRects() method.
  if (_type == TYPE_X11_XIMAGE_WITH_PIXMAP)
  {
    for (sysuint_t i = 0; i != count; i++)
    {
      int x = rects[i].getX();
      int y = rects[i].getY();
      uint w = uint(rects[i].getWidth());
      uint h = uint(rects[i].getHeight());

      engine->pXPutImage(
        engine->getDisplay(),
        _pixmap,
        engine->getGc(),
        _ximage,
        x, y,
        x, y,
        w, h);
    }
  }
}

void X11GuiBackBuffer::blitRects(XID target, const IntBox* rects, sysuint_t count)
{
  X11GuiEngine* engine = GUI_ENGINE();
  sysuint_t i;

  switch (getType())
  {
    case TYPE_NONE:
      break;

    // These ones uses pixmap as X resource
    case TYPE_X11_XSHM_PIXMAP:
    case TYPE_X11_XIMAGE_WITH_PIXMAP:
      for (i = 0; i != count; i++)
      {
        int x = rects[i].getX();
        int y = rects[i].getY();
        int w = rects[i].getWidth();
        int h = rects[i].getHeight();

        engine->pXCopyArea(
          engine->getDisplay(),
          _pixmap,
          target,
          engine->getGc(),
          x, y,
          w, h,
          x, y);
      }
      break;

    case TYPE_X11_XIMAGE:
      for (i = 0; i != count; i++)
      {
        int x = rects[i].getX();
        int y = rects[i].getY();
        int w = rects[i].getWidth();
        int h = rects[i].getHeight();

        engine->pXPutImage(
          engine->getDisplay(),
          target,
          engine->getGc(),
          _ximage,
          x, y,
          x, y,
          w, h);
      }
      break;
  }
}

// ============================================================================
// [Fog::X11GuiEventLoop]
// ============================================================================

X11GuiEventLoop::X11GuiEventLoop() : EventLoop(Ascii8("Gui.X11"))
{
  _wakeUpSent.init(0);
}

X11GuiEventLoop::~X11GuiEventLoop()
{
  _destroyed();
}

void X11GuiEventLoop::_runInternal()
{
  static const sysuint_t WORK_PER_LOOP = 2;

  sysuint_t work = 0;

  // Inspired in WinEventLoop and ported to X11GuiEventLoop.
  for (;;)
  {
    bool didWork = false;
    bool more;

    // Process XEvents. This is the biggest priority task.
    do {
      more = _processNextXEvent();
      didWork |= more;

      if (_quitting) goto end;
    } while (more);

    // doWork.
    didWork |= _doWork();
    if (_quitting) goto end;
    if (didWork && work < WORK_PER_LOOP) continue;

    work++;

    // doDelayedWork.
    didWork |= _doDelayedWork(&_delayedWorkTime);
    if (_quitting) goto end;
    if (didWork && work < WORK_PER_LOOP) continue;

    // doIdleWork.
    //
    // If quit is received in nestedLoop or through runAllPending(), we will
    // quit here, because we don't want to do XSync().
    didWork |= _doIdleWork();
    if (_quitting) goto end;

    // Call XSync, this is round-trip operation and can generate events.
    didWork |= _xsync();
    if (_quitting) goto end;

    // Clear work status, this is why it's here...
    if (didWork || work < WORK_PER_LOOP) continue;
    work = 0;

    // Finally wait.
    _waitForWork();
  }

end:
  return;
}

void X11GuiEventLoop::_scheduleWork()
{
  _sendWakeUp();
}

void X11GuiEventLoop::_scheduleDelayedWork(const Time& delayedWorkTime)
{
  // We know that we can't be blocked on wait() right now since this method can
  // only be called on the same thread as run(), so we only need to update our
  // record of how long to sleep when we do sleep.
  _delayedWorkTime = delayedWorkTime;
}

void X11GuiEventLoop::_waitForWork()
{
  X11GuiEngine* engine = GUI_ENGINE();

  int fd = engine->_fd;
  int fdSize = Math::max(fd, engine->_wakeUpPipe[0]) + 1;
  fd_set fdSet;

  struct timeval tval;
  struct timeval* ptval = NULL;

  FD_ZERO(&fdSet);
  FD_SET(fd, &fdSet);
  FD_SET(engine->_wakeUpPipe[0], &fdSet);

  if (_delayedWorkTime.isNull())
  {
    // There are no scheduled tasks, so ptval is NULL and this tells to select()
    // that it should wait infitine time.
  }
  else
  {
    TimeDelta delay = _delayedWorkTime - Time::now();

    if (delay > TimeDelta())
    {
      // Go to sleep. X11 will wake us to process X events and we also set
      // interval to wake up to run planned tasks (usually Timers).
      int64_t udelay = delay.inMicroseconds();
      tval.tv_sec = (int)(udelay / 1000000);
      tval.tv_usec = (int)(udelay % 1000000);
      if (tval.tv_usec <= 100) tval.tv_usec = 100;
      ptval = &tval;
    }
    else
    {
      // It looks like delayedWorkTime indicates a time in the past, so we
      // need to call doDelayedWork now.
      _delayedWorkTime = Time();
      return;
    }
  }

  int ret = ::select(fdSize, &fdSet, NULL, NULL, ptval);

  if (ret < 0)
  {
    fog_debug("Fog::X11GuiEventLoop::waitForWork() - select() failed (errno=%d).", errno);
  }

  if (ret > 0)
  {
    if (FD_ISSET(engine->_wakeUpPipe[0], &fdSet))
    {
      // Dummy c, the actual value is out of our interest.
      uint8_t c;

      if (read(engine->_wakeUpPipe[0], &c, 1) != 1)
      {
        fog_debug("Fog::X11GuiEventLoop::waitForWork() - Can't read from weak-up pipe.");
      }

      _wakeUpSent.cmpXchg(1, 0);
    }
  }
}

bool X11GuiEventLoop::_xsync()
{
  X11GuiEngine* engine = GUI_ENGINE();
  engine->pXSync(engine->getDisplay(), false);
  return engine->pXPending(engine->getDisplay());
}

bool X11GuiEventLoop::_processNextXEvent()
{
  X11GuiEngine* engine = GUI_ENGINE();
  if (!engine->pXPending(engine->getDisplay())) return false;

  XEvent xe;
  engine->pXNextEvent(engine->getDisplay(), &xe);

  X11GuiWindow* uiWindow = reinterpret_cast<X11GuiWindow*>(engine->handleToNative((void*)xe.xany.window));

  FOG_LISTENER_FOR_EACH(NativeEventListener, _nativeEventListenerList, onBeforeDispatch(&xe));
  if (uiWindow) uiWindow->onX11Event(&xe);
  FOG_LISTENER_FOR_EACH(NativeEventListener, _nativeEventListenerList, onAfterDispatch(&xe));

  return true;
}

void X11GuiEventLoop::_sendWakeUp()
{
  if (_wakeUpSent.cmpXchg(0, 1))
  {
    static const uint8_t c[1] = { 'W' };
    X11GuiEngine* engine = GUI_ENGINE();

    if (write(engine->_wakeUpPipe[1], c, 1) != 1)
    {
      {
        fog_debug("Fog::X11GuiEventLoop::sendWakeUp() - Can't write to weak-up pipe");
      }
    }
  }
}

} // Fog namespace

// ============================================================================
// [CAPI]
// ============================================================================

extern "C" FOG_DLL_EXPORT void* createGuiEngine()
{
  return new (std::nothrow) Fog::X11GuiEngine();
}
