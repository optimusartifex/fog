// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_X11UIENGINE_H
#define _FOG_UI_ENGINE_X11UIENGINE_H

// [Dependencies]
#include <Fog/UI/Engine/UIEngine.h>
#include <Fog/UI/Engine/UIEngineWindow.h>

// [Dependencies - Fog/UI - X11]
#include <Fog/UI/Engine/X11UIEngineConfig.h>
#include <Fog/UI/Engine/X11UIEngineXlib.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::X11_ATOM]
// ============================================================================

enum X11_ATOM
{
  X11_ATOM_WM_PROTOCOLS = 0,
  X11_ATOM_WM_DELETE_WINDOW,
  X11_ATOM_WM_TAKE_FOCUS,
  X11_ATOM_WM_CLASS,
  X11_ATOM_WM_NAME,
  X11_ATOM_WM_COMMAND,
  X11_ATOM_WM_ICON_NAME,
  X11_ATOM_WM_CHANGE_STATE,
  X11_ATOM_WM_CLIENT_LEADER,
  X11_ATOM_MOTIF_WM_HINTS,
  X11_ATOM_WIN_LAYER,

  X11_ATOM_XdndSelection,
  X11_ATOM_XdndAware,
  X11_ATOM_XdndTypeList,
  X11_ATOM_XdndEnter,
  X11_ATOM_XdndPosition,
  X11_ATOM_XdndActionCopy,
  X11_ATOM_XdndActionMove,
  X11_ATOM_XdndActionPrivate,
  X11_ATOM_XdndActionAsk,
  X11_ATOM_XdndActionList,
  X11_ATOM_XdndActionLink,
  X11_ATOM_XdndActionDescription,
  X11_ATOM_XdndProxy,
  X11_ATOM_XdndStatus,
  X11_ATOM_XdndLeave,
  X11_ATOM_XdndDrop,
  X11_ATOM_XdndFinished,

  X11_ATOM_NET_SUPPORTED,
  X11_ATOM_NET_SUPPORTING_WM_CHECK,

  X11_ATOM_NET_NUMBER_OF_DESKTOPS,
  X11_ATOM_NET_DESKTOP_GEOMETRY,
  X11_ATOM_NET_DESKTOP_NAMES,
  X11_ATOM_NET_CURRENT_DESKTOP,
  X11_ATOM_NET_DESKTOP_VIEWPORT,
  X11_ATOM_NET_WORKAREA,
  X11_ATOM_NET_VIRTUAL_ROOTS,

  X11_ATOM_NET_CLIENT_LIST,
  X11_ATOM_NET_CLIENT_LIST_STACKING,
  X11_ATOM_NET_ACTIVE_WINDOW,
  X11_ATOM_NET_CLOSE_WINDOW,

  X11_ATOM_NET_WM_MOVERESIZE,
  X11_ATOM_NET_WM_NAME,
  X11_ATOM_NET_WM_VISIBLE_NAME,
  X11_ATOM_NET_WM_ICON_NAME,
  X11_ATOM_NET_WM_VISIBLE_ICON_NAME,
  X11_ATOM_NET_WM_DESKTOP,
  X11_ATOM_NET_WM_WINDOW_TYPE,
  X11_ATOM_NET_WM_STATE,
  X11_ATOM_NET_WM_ALLOWED_ACTIONS,
  X11_ATOM_NET_WM_STRUT,
  X11_ATOM_NET_WM_STRUT_PARTIAL,
  X11_ATOM_NET_WM_ICON_GEOMETRY,
  X11_ATOM_NET_WM_ICON,
  X11_ATOM_NET_WM_PID,
  X11_ATOM_NET_WM_PING,
  X11_ATOM_NET_WM_USER_TIME,
  X11_ATOM_NET_WM_SYNC_REQUEST,

  X11_ATOM_NET_WM_WINDOW_TYPE_DESKTOP,
  X11_ATOM_NET_WM_WINDOW_TYPE_DOCK,
  X11_ATOM_NET_WM_WINDOW_TYPE_TOOLBAR,
  X11_ATOM_NET_WM_WINDOW_TYPE_MENU,
  X11_ATOM_NET_WM_WINDOW_TYPE_UTILITY,
  X11_ATOM_NET_WM_WINDOW_TYPE_SPLASH,
  X11_ATOM_NET_WM_WINDOW_TYPE_DIALOG,
  X11_ATOM_NET_WM_WINDOW_TYPE_NORMAL,

  X11_ATOM_NET_WM_STATE_MODAL,
  X11_ATOM_NET_WM_STATE_STICKY,
  X11_ATOM_NET_WM_STATE_MAXIMIZED_VERT,
  X11_ATOM_NET_WM_STATE_MAXIMIZED_HORZ,
  X11_ATOM_NET_WM_STATE_SHADED,
  X11_ATOM_NET_WM_STATE_SKIP_TASKBAR,
  X11_ATOM_NET_WM_STATE_SKIP_PAGER,
  X11_ATOM_NET_WM_STATE_HIDDEN,
  X11_ATOM_NET_WM_STATE_FULLSCREEN,
  X11_ATOM_NET_WM_STATE_ABOVE,
  X11_ATOM_NET_WM_STATE_BELOW,
  X11_ATOM_NET_WM_WINDOW_OPACITY,

  X11_ATOM_CLIPBOARD,
  X11_ATOM_TARGETS,
  X11_ATOM_COMPOUND_TEXT,
  X11_ATOM_UTF8_STRING,
  X11_ATOM_FILE_NAME,
  X11_ATOM_STRING,
  X11_ATOM_TEXT,
  X11_ATOM_INCR,

  X11_ATOM_COUNT
};

// ============================================================================
// [Fog:X11UIEngineKeyMap]
// ============================================================================

//! @brief X11 specific keyboard mapping.
struct FOG_NO_EXPORT X11UIEngineKeyMap
{
  uint16_t odd[256];
  uint16_t msc[256];
};

// ============================================================================
// [Fog::X11UIEngineXLibAPI]
// ============================================================================

struct FOG_NO_EXPORT X11UIEngineXLibAPI
{
  XBool (FOG_CDECL *_XSupportsLocale)(void);
  char* (FOG_CDECL *_XSetLocaleModifiers)(const char* modifierList);

  XDisplay* (FOG_CDECL *_XOpenDisplay)(const char* name);
  int (FOG_CDECL *_XCloseDisplay)(XDisplay* display);

  XErrorHandler (FOG_CDECL *_XSetErrorHandler)(XErrorHandler func);
  XIOErrorHandler (FOG_CDECL *_XSetIOErrorHandler)(XIOErrorHandler func);
  int (FOG_CDECL *_XGetErrorText)(XDisplay* display, int code, char* bufferOut, int bufferLength);

  XAtom (FOG_CDECL *_XInternAtom)(XDisplay* display, const char* atomName, XBool onlyIfExists);
  XStatus (FOG_CDECL *_XInternAtoms)(XDisplay* display, char** names, int count, XBool onlyIfExists, XAtom* atomsOut);

  XID (FOG_CDECL *_XRootWindow)(XDisplay* display, int screenNumber);
  int (FOG_CDECL *_XDisplayWidth)(XDisplay* display, int screenNumber);
  int (FOG_CDECL *_XDisplayHeight)(XDisplay* display, int screenNumber);
  int (FOG_CDECL *_XDefaultScreen)(XDisplay* display);
  XVisual* (FOG_CDECL *_XDefaultVisual)(XDisplay* display, int screenNumber);
  XColormap (FOG_CDECL *_XDefaultColormap)(XDisplay* display, int screenNumber);
  int (FOG_CDECL *_XDefaultDepth)(XDisplay* display, int screenNumber);

  int (FOG_CDECL *_XEventsQueued)(XDisplay* display, int mode);
  int (FOG_CDECL *_XNextEvent)(XDisplay* display, XEvent* xEvent);
  XBool (FOG_CDECL *_XCheckMaskEvent)(XDisplay* display, long eventMask, XEvent* eventOut);
  XBool (FOG_CDECL *_XCheckTypedEvent)(XDisplay* display, int eventType, XEvent* eventOut);
  XBool (FOG_CDECL *_XCheckTypedWindowEvent)(XDisplay* display, XID window, int eventType, XEvent* eventOut);
  XBool (FOG_CDECL *_XCheckWindowEvent)(XDisplay* display, XID window, long eventMask, XEvent* eventOut);
  XStatus (FOG_CDECL *_XSendEvent)(XDisplay* display, XID window, XBool propagate, long eventMask, XEvent* eventSend);

  XIM (FOG_CDECL *_XOpenIM)(XDisplay* display, void* rdb, char* resName, char* resClass);
  XStatus (FOG_CDECL *_XCloseIM)(XIM im);

  XIC (FOG_CDECL *_XCreateIC)(XIM im, ...);
  void (FOG_CDECL *_XDestroyIC)(XIC ic);
  int (FOG_CDECL *_XRefreshKeyboardMapping)(XMappingEvent* eventMap);

  XID (FOG_CDECL *_XCreateWindow)(XDisplay* display, XID parent, int x, int y, uint width, uint height, uint boderWidth, int depth, uint _class, XVisual* visual, ulong valueMask, XSetWindowAttributes* attributes);
  int (FOG_CDECL *_XDestroyWindow)(XDisplay* display, XID window);
  int (FOG_CDECL *_XMoveWindow)(XDisplay* display, XID window, int x, int y);
  int (FOG_CDECL *_XResizeWindow)(XDisplay* display, XID window, uint w, uint h);
  int (FOG_CDECL *_XMoveResizeWindow)(XDisplay* display, XID window, int x, int y, uint w, uint h);
  int (FOG_CDECL *_XMapWindow)(XDisplay* display, XID window);
  int (FOG_CDECL *_XUnmapWindow)(XDisplay* display, XID window);
  int (FOG_CDECL *_XRaiseWindow)(XDisplay* display, XID window);
  int (FOG_CDECL *_XLowerWindow)(XDisplay* display, XID window);
  int (FOG_CDECL *_XReparentWindow)(XDisplay* display, XID window, XID parent, int x, int y);
  int (FOG_CDECL *_XChangeWindowAttributes)(XDisplay* display, XID window, ulong valueMask, XSetWindowAttributes* attributes);
  XStatus (FOG_CDECL *_XGetWindowAttributes)(XDisplay* display, XID window, XWindowAttributes* attributesOuts);

  int (FOG_CDECL *_XSetInputFocus)(XDisplay* display, XID window, int revert_to, XTime time);
  int (FOG_CDECL *_XGetInputFocus)(XDisplay* display, XID* window_return, int* revert_to_return);

  int (FOG_CDECL *_XSelectInput)(XDisplay* display, XID window, long eventMask);
  int (FOG_CDECL *_XSetNormalHints)(XDisplay* display, XID window, XSizeHints* hints);

  int (FOG_CDECL *_XSetWMProtocols)(XDisplay* display, XID window, XAtom* protocols, int count);
  int (FOG_CDECL *_XGetWMProtocols)(XDisplay* display, XID window, XAtom** protocolsOut, int* countOut);

  int (FOG_CDECL *_XSetWMProperties)(XDisplay* display, XID window, XTextProperty* window_name, XTextProperty* icon_name, char** argv, int argc, XSizeHints* normalHints, XWMHints* wmHints, XClassHint* classHints);

  int (FOG_CDECL *_XTranslateCoordinates)(XDisplay* display, XWindow srcWindow, XWindow destWindow, int srcX, int srcY, int* destXOut, int* destYOut, XWindow* childOut);
  int (FOG_CDECL *_XwcTextListToTextProperty)(XDisplay* display, wchar_t** list, int count, XICCEncodingStyle style, XTextProperty* textPropertyOut);

  int (FOG_CDECL *_XFlush)(XDisplay* display);
  int (FOG_CDECL *_XSync)(XDisplay* display, XBool discard);

  GC (FOG_CDECL *_XCreateGC)(XDisplay* display, XID drawable, ulong valueMask, XGCValues* values);
  int (FOG_CDECL *_XFreeGC)(XDisplay* display, GC gc);
  int (FOG_CDECL *_XSetClipRectangles)(XDisplay *display, GC gc, int clipXOrigin, int clipYOrigin, XRectangle rects[], int n, int ordering);

  KeySym (FOG_CDECL *_XKeycodeToKeysym)(XDisplay* display, uint keyCode, int index);

  int (FOG_CDECL *_XLookupString)(XKeyEvent* event, char* bufferOut, int bufferLength, KeySym* keySymOut, XComposeStatus* statusInOut);
  int (FOG_CDECL *_XwcLookupString)(XIC ic, XKeyPressedEvent* event, wchar_t* bufferOut, int wcharsBuffer, KeySym* keysymOut, XStatus* statusOut);

  int (FOG_CDECL *_XGrabPointer)(XDisplay* display, XID grabWindow, XBool ownerEvents, uint eventMask, int pointerMode, int keyboardMode, XID confineTo, Cursor cursor, XTime time);
  int (FOG_CDECL *_XUngrabPointer)(XDisplay* display, XTime time);

  int (FOG_CDECL *_XGetPointerMapping)(XDisplay* display, uchar* map_return, int namp);

  XID (FOG_CDECL *_XCreatePixmap)(XDisplay* display, XID drawable, uint width, uint height, uint depth);
  int (FOG_CDECL *_XFreePixmap)(XDisplay* display, XID pixmap);
  int (FOG_CDECL *_XCopyArea)(XDisplay* display, Drawable src, XID dest, GC gc, int srcX, int srcY, uint width, uint height, int destX, int destY);
  int (FOG_CDECL *_XFree)(void* data);

  XImage* (FOG_CDECL *_XCreateImage)(XDisplay* display, XVisual* visual, uint depth, int format, int offset, char* data, uint width, uint height, int bitmapPad, int bytesPerLine);
  int (FOG_CDECL *_XDestroyImage)(XImage* image);
  int (FOG_CDECL *_XPutImage)(XDisplay* display, XID drawable, GC gc, XImage* xImage, int srcX, int srcY, int destX, int destY, uint width, uint height);

  XColormap (FOG_CDECL *_XCreateColormap)(XDisplay* display, XID window, XVisual* visual, int alloc);
  int (FOG_CDECL *_XFreeColormap)(XDisplay* display, XColormap colormap);

  XStatus (FOG_CDECL *_XAllocColor)(XDisplay* display, XColormap, XColor* screenInOut);
  int (FOG_CDECL *_XFreeColors)(XDisplay* display, XColormap, ulong* pixels, int nPixels, ulong planes);

  XStatus (FOG_CDECL *_XMatchVisualInfo)(XDisplay* display, int screen, int depth, int _class, XVisualInfo* vinfoOut);

  // Xutil.h
  void (FOG_CDECL *_XSetWMName)(XDisplay* display, XID window, XTextProperty* textProperty);

  void (FOG_CDECL *_XChangeProperty)(XDisplay* display, XID window, XAtom, XAtom, int, int, unsigned char*, int);
  void (FOG_CDECL *_XSetSelectionOwner)(XDisplay* display, XAtom, XWindow, XTime);
  char* (FOG_CDECL *_XGetAtomName)(XDisplay* display, XAtom);
  void (FOG_CDECL *_XConvertSelection)(XDisplay* display, XAtom, XAtom, XAtom, XWindow, XTime);
  int (FOG_CDECL *_XGetWindowProperty)(XDisplay* display, XWindow, XAtom, long, long, XBool, XAtom, XAtom*, int*, unsigned long*, unsigned long*, unsigned char**);
  void (FOG_CDECL *_XDeleteProperty)(XDisplay* display, XWindow, XAtom);
};

// ============================================================================
// [Fog::X11UIEngineXExtAPI]
// ============================================================================

struct FOG_NO_EXPORT X11UIEngineXExtAPI
{
  XBool (FOG_CDECL *_XShmQueryVersion)(XDisplay* display, int* majorVersion, int* minorVersion, XBool* sharedPixmaps);

  XStatus (FOG_CDECL *_XShmAttach)(XDisplay* display, XShmSegmentInfo* shmInfo);
  XStatus (FOG_CDECL *_XShmDetach)(XDisplay* display, XShmSegmentInfo* shmInfo);

  XImage* (FOG_CDECL *_XShmCreateImage)(XDisplay* display, XVisual* visual, uint depth, int format, char* data, XShmSegmentInfo* shmInfo, uint width, uint height);
  XStatus (FOG_CDECL *_XShmPutImage)(XDisplay *display, XID drawable, GC gc, XImage *image, int srcX, int srcY, int dstX, int dstY, uint width, uint height, XBool sendEvent);
};

// ============================================================================
// [Fog::X11UIEngineXRenderAPI]
// ============================================================================

struct FOG_NO_EXPORT X11UIEngineXRenderAPI
{
  XStatus (FOG_CDECL *_XRenderQueryVersion)(XDisplay* display, int* major, int * minor);
  XStatus (FOG_CDECL *_XRenderQueryFormats)(XDisplay* display);
  int (FOG_CDECL *_XRenderQuerySubpixelOrder)(XDisplay* display, int screen);
  XBool (FOG_CDECL *_XRenderSetSubpixelOrder)(XDisplay* display, int screen, int subpixel);
  XRenderPictFormat* (FOG_CDECL *_XRenderFindVisualFormat)(XDisplay* display, const XVisual* visual);
  XRenderPictFormat* (FOG_CDECL *_XRenderFindFormat)(XDisplay* display, unsigned long mask, const XRenderPictFormat* templ, int count);
  XRenderPictFormat* (FOG_CDECL *_XRenderFindStandardFormat)(XDisplay* display, int format);
  XPicture (FOG_CDECL *_XRenderCreatePicture)(XDisplay* display, XID drawable, const XRenderPictFormat* format, unsigned long valuemask, const XRenderPictureAttributes* attributes);
  void (FOG_CDECL *_XRenderChangePicture)(XDisplay* display, XPicture picture, unsigned long valuemask, const XRenderPictureAttributes* attributes);
  void (FOG_CDECL *_XRenderSetPictureTransform)(XDisplay* display, XPicture picture, XTransform* transform);
  void (FOG_CDECL *_XRenderFreePicture)(XDisplay* display, XPicture picture);
  void (FOG_CDECL *_XRenderComposite)(XDisplay* display, int op, XPicture src, XPicture mask, XPicture dest, int srcX, int srcY, int maskX, int maskY, int destX, int destY, unsigned int width, unsigned int height);
  Cursor (FOG_CDECL *_XRenderCreateCursor)(XDisplay* display, XPicture source, unsigned int x, unsigned int y);
};

// ============================================================================
// [Fog::X11UIEngine]
// ============================================================================

//! @brief X11 specific @ref UIEngine implementation.
struct FOG_UI_X11_API X11UIEngine : public UIEngine
{
  FOG_DECLARE_OBJECT(X11UIEngine, UIEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  X11UIEngine();
  virtual ~X11UIEngine();

  // --------------------------------------------------------------------------
  // [Open / Close]
  // --------------------------------------------------------------------------

  err_t openXlib();

  err_t openDisplay();
  err_t closeDisplay();

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  void initDisplay();
  void initKeyboard();
  void initMouse();

  // --------------------------------------------------------------------------
  // [Display / Palette]
  // --------------------------------------------------------------------------

  virtual void updateDisplayInfo() override;

  // --------------------------------------------------------------------------
  // [Keyboard / Mouse]
  // --------------------------------------------------------------------------

  virtual void updateKeyboardInfo() override;
  virtual void updateMouseInfo() override;

  virtual void setMouseWheelLines(uint32_t lines) override;

  uint32_t getFogKeyFromXKeySym(KeySym xKeySym) const;
  uint32_t getFogButtonFromXButton(uint xButton) const;

  // --------------------------------------------------------------------------
  // [DoUpdate]
  // --------------------------------------------------------------------------

  virtual void doPaintWindow(UIEngineWindow* window, Painter* painter, const RectI& paintRect) override;
  virtual void doBlitWindow(UIEngineWindow* window) override;

  // --------------------------------------------------------------------------
  // [Window Management]
  // --------------------------------------------------------------------------

  virtual err_t createWindow(UIEngineWindow* window, uint32_t flags) override;
  virtual err_t destroyWindow(UIEngineWindow* window) override;

  // --------------------------------------------------------------------------
  // [X11 - WM]
  // --------------------------------------------------------------------------

  XID getWMClientLeader();

  // --------------------------------------------------------------------------
  // [X11 - Atoms]
  // --------------------------------------------------------------------------

  FOG_INLINE XAtom* getAtomList(void) { return _atomList; }
  FOG_INLINE const XAtom* getAtomList(void) const { return _atomList; }

  FOG_INLINE XAtom getAtom(int index) const
  {
    FOG_ASSERT(index < X11_ATOM_COUNT);
    return _atomList[index];
  }

  // --------------------------------------------------------------------------
  // [X11 - Misc]
  // --------------------------------------------------------------------------

  void sendXClientMessage(XWindow window, long mask, long l0, long l1, long l2, long l3, long l4);

  // --------------------------------------------------------------------------
  // [Members - Core]
  // --------------------------------------------------------------------------

  //! @brief X11 display connected to (or @c NULL if not connected).
  XDisplay* _display;
  //! @brief File-descriptor used for connection.
  int _fd;
  //! @brief X11 screen.
  int _screen;

  //! @brief Root window ID.
  XID _root;
  //! @brief Global GC.
  GC _gc;
  //! @brief Visual used.
  XVisual* _visual;
  //! @brief Color-map used.
  XColormap _colorMap;

  // --------------------------------------------------------------------------
  // [Members - XIM / XIC]
  // --------------------------------------------------------------------------

  //! @brief X input context.
  XIM _xim;

  //! @brief Number of buttons.
  uint32_t _numButtons;

  // --------------------------------------------------------------------------
  // [Members - Pipe]
  // --------------------------------------------------------------------------

  //! @brief Pipe used to wake up @c X11UIEventLoop.
  int _wakeUpPipe[2];

  // --------------------------------------------------------------------------
  // [Members - Flags]
  // --------------------------------------------------------------------------

  //! @brief Whether the X-SHM extension is supported.
  uint32_t _xShm : 1;
  //! @brief Whether the private color-map was allocated (8-bpp only).
  uint32_t _xPrivateColorMap : 1;
  //! @brief Force to create XPixmap when creating double-buffer (default true).
  uint32_t _xForcePixmap : 1;

  //! @brief Don't log debugging message about unsupported bpp (secondary buffer).
  uint32_t _xDontLogUnsupportedBpp : 1;

  // --------------------------------------------------------------------------
  // [Members - WM Support]
  // --------------------------------------------------------------------------

  XID _wmClientLeader;

  // --------------------------------------------------------------------------
  // [Members - Atoms]
  // --------------------------------------------------------------------------

  //! @brief X11 atom list (see @ref X11_ATOM).
  XAtom _atomList[X11_ATOM_COUNT];
  //! @brief X11 atom names (see @ref X11_ATOM).
  const char** _atomNames;

  // --------------------------------------------------------------------------
  // [Members - KeyMap]
  // --------------------------------------------------------------------------

  //! @brief Keyboard mapping.
  X11UIEngineKeyMap _keyMap;

  // --------------------------------------------------------------------------
  // [Members - Libraries]
  // --------------------------------------------------------------------------

  //! @brief Xlib library object (dynamically opened libX11 library).
  Library _XLibLibrary;
  //! @brief Xext library object (dynamically opened libXext library).
  Library _XExtLibrary;
  //! @brief Xrender library object (dynamically opened libXrender library).
  Library _XRenderLibrary;

  // --------------------------------------------------------------------------
  // [Members - API]
  // --------------------------------------------------------------------------

  //! @brief Xlib API.
  X11UIEngineXLibAPI _XLib;
  //! @brief Xext API.
  X11UIEngineXExtAPI _XExt;
  //! @brief Xrender API.
  X11UIEngineXRenderAPI _XRender;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_X11UIENGINE_H
