// [Gui library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_GUIENGINE_X11_P_H
#define _FOG_GUI_GUIENGINE_X11_P_H

// [Visibility]
#include <Fog/Core/Build.h>

#if defined(Fog_Gui_X11_EXPORTS)
# define FOG_GUIENGINE_X11_API FOG_DLL_EXPORT
#else
# define FOG_GUIENGINE_X11_API FOG_DLL_IMPORT
#endif // FogUI_X11_EXPORTS

// [Dependencies]
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Object.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/GuiEngine.h>
#include <Fog/Gui/GuiEngine/GuiEngine_X11_Xlib.h>

namespace Fog {

//! @addtogroup Fog_Gui_Engine
//! @{

// ============================================================================
// [Fog::X11GuiEngine]
// ============================================================================

//! @brief X11 Gui engine.
struct FOG_GUIENGINE_X11_API X11GuiEngine : public GuiEngine
{
  FOG_DECLARE_OBJECT(X11GuiEngine, GuiEngine)

  // --------------------------------------------------------------------------
  // [Registration]
  // --------------------------------------------------------------------------

  static void registerGuiEngine();

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  X11GuiEngine();
  virtual ~X11GuiEngine();

  // --------------------------------------------------------------------------
  // [Display]
  // --------------------------------------------------------------------------

  virtual void updateDisplayInfo();

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  virtual void doUpdate();
  virtual void doBlitWindow(GuiWindow* window, const IntBox* rects, sysuint_t count);

  // --------------------------------------------------------------------------
  // [GuiWindow]
  // --------------------------------------------------------------------------

  virtual GuiWindow* createGuiWindow(Widget* widget);
  virtual void destroyGuiWindow(GuiWindow* native);

  // --------------------------------------------------------------------------
  // [X11 Atoms]
  // --------------------------------------------------------------------------

  enum Atom_Enum
  {
    Atom_WM_PROTOCOLS = 0,
    Atom_WM_DELETE_WINDOW,
    Atom_WM_TAKE_FOCUS,
    Atom_WM_CLASS,
    Atom_WM_NAME,
    Atom_WM_COMMAND,
    Atom_WM_ICON_NAME,
    Atom_WM_CHANGE_STATE,
    Atom_WM_CLIENT_LEADER,
    Atom_MOTIF_WM_HINTS,
    Atom_WIN_LAYER,

    Atom_XdndSelection,
    Atom_XdndAware,
    Atom_XdndTypeList,
    Atom_XdndEnter,
    Atom_XdndPosition,
    Atom_XdndActionCopy,
    Atom_XdndActionMove,
    Atom_XdndActionPrivate,
    Atom_XdndActionAsk,
    Atom_XdndActionList,
    Atom_XdndActionLink,
    Atom_XdndActionDescription,
    Atom_XdndProxy,
    Atom_XdndStatus,
    Atom_XdndLeave,
    Atom_XdndDrop,
    Atom_XdndFinished,

    Atom_NET_SUPPORTED,
    Atom_NET_SUPPORTING_WM_CHECK,

    Atom_NET_NUMBER_OF_DESKTOPS,
    Atom_NET_DESKTOP_GEOMETRY,
    Atom_NET_DESKTOP_NAMES,
    Atom_NET_CURRENT_DESKTOP,
    Atom_NET_DESKTOP_VIEWPORT,
    Atom_NET_WORKAREA,
    Atom_NET_VIRTUAL_ROOTS,

    Atom_NET_CLIENT_LIST,
    Atom_NET_CLIENT_LIST_STACKING,
    Atom_NET_ACTIVE_WINDOW,
    Atom_NET_CLOSE_WINDOW,

    Atom_NET_WM_MOVERESIZE,
    Atom_NET_WM_NAME,
    Atom_NET_WM_VISIBLE_NAME,
    Atom_NET_WM_ICON_NAME,
    Atom_NET_WM_VISIBLE_ICON_NAME,
    Atom_NET_WM_DESKTOP,
    Atom_NET_WM_WINDOW_TYPE,
    Atom_NET_WM_STATE,
    Atom_NET_WM_ALLOWED_ACTIONS,
    Atom_NET_WM_STRUT,
    Atom_NET_WM_STRUT_PARTIAL,
    Atom_NET_WM_ICON_GEOMETRY,
    Atom_NET_WM_ICON,
    Atom_NET_WM_PID,
    Atom_NET_WM_PING,
    Atom_NET_WM_USER_TIME,
    Atom_NET_WM_SYNC_REQUEST,

    Atom_NET_WM_WINDOW_TYPE_DESKTOP,
    Atom_NET_WM_WINDOW_TYPE_DOCK,
    Atom_NET_WM_WINDOW_TYPE_TOOLBAR,
    Atom_NET_WM_WINDOW_TYPE_MENU,
    Atom_NET_WM_WINDOW_TYPE_UTILITY,
    Atom_NET_WM_WINDOW_TYPE_SPLASH,
    Atom_NET_WM_WINDOW_TYPE_DIALOG,
    Atom_NET_WM_WINDOW_TYPE_NORMAL,

    Atom_NET_WM_STATE_MODAL,
    Atom_NET_WM_STATE_STICKY,
    Atom_NET_WM_STATE_MAXIMIZED_VERT,
    Atom_NET_WM_STATE_MAXIMIZED_HORZ,
    Atom_NET_WM_STATE_SHADED,
    Atom_NET_WM_STATE_SKIP_TASKBAR,
    Atom_NET_WM_STATE_SKIP_PAGER,
    Atom_NET_WM_STATE_HIDDEN,
    Atom_NET_WM_STATE_FULLSCREEN,
    Atom_NET_WM_STATE_ABOVE,
    Atom_NET_WM_STATE_BELOW,
    Atom_NET_WM_WINDOW_OPACITY,

    Atom_CLIPBOARD,
    Atom_TARGETS,
    Atom_COMPOUND_TEXT,
    Atom_UTF8_STRING,
    Atom_FILE_NAME,
    Atom_STRING,
    Atom_TEXT,
    Atom_INCR,

    Atom_Count
  };

  XAtom _atoms[Atom_Count];
  const char** _atomNames;

  FOG_INLINE XAtom* getAtoms(void) const { return (XAtom*)_atoms; }

  FOG_INLINE XAtom getAtom(int index) const
  {
    FOG_ASSERT(index < Atom_Count);
    return _atoms[index];
  }

  // --------------------------------------------------------------------------
  // [X11 Driver]
  // --------------------------------------------------------------------------

  XDisplay* _display;
  int _fd;
  int _screen;
  XVisual* _visual;
  XColormap _colormap;
  XID _root;
  GC _gc;

  //! @brief Pipe to wake up event loop.
  int _wakeUpPipe[2];

  uint32_t _xShm : 1;
  uint32_t _xPrivateColormap : 1;

  XIM _xim;

  FOG_INLINE XDisplay* getDisplay(void) const { return _display; }
  FOG_INLINE int getFd(void) const { return _fd; }
  FOG_INLINE int getScreen(void) const { return _screen; }
  FOG_INLINE XVisual* getVisual(void) const { return _visual; }
  FOG_INLINE XColormap getColormap(void) const { return _colormap; }
  FOG_INLINE XID getRoot(void) const { return _root; }
  FOG_INLINE GC getGc(void) const { return _gc; }

  FOG_INLINE uint32_t hasXShm(void) const { return _xShm; }
  FOG_INLINE uint32_t hasXPrivateColormap(void) const { return _xPrivateColormap; }
  FOG_INLINE uint32_t getXButtons(void) const { return _xButtons; }

  FOG_INLINE XIM getXim(void) const { return _xim; }

  // --------------------------------------------------------------------------
  // [X11 Keyboard / Unicode]
  // --------------------------------------------------------------------------

  struct X11Keymap
  {
    uint16_t odd[256];
    uint16_t misc[256];
  };

  X11Keymap _xKeymap;

  void initKeyboard();
  uint32_t translateXSym(KeySym xsym) const;

  // --------------------------------------------------------------------------
  // [X11 Mouse]
  // --------------------------------------------------------------------------

  uint32_t _xButtons;

  void initMouse();
  uint32_t translateButton(uint x11Button) const;

  // --------------------------------------------------------------------------
  // [X11 ColorMap]
  // --------------------------------------------------------------------------

  bool createColormap();
  uint allocRGB(XColormap colormap, int nr, int ng, int nb, uint8_t* palconv);
  void freeRGB(XColormap colormap, const uint8_t* palconv, uint count);

  // --------------------------------------------------------------------------
  // [X11 Libraries]
  // --------------------------------------------------------------------------

  //! @brief Xlib library object (dynamically opened libX11 library).
  Library _xlib;
  //! @brief Xext library object (dynamically opened libXext library).
  Library _xext;
  //! @brief Xrender library object (dynamically opened libXrender library).
  Library _xrender;

  // --------------------------------------------------------------------------
  // [X11 API]
  // --------------------------------------------------------------------------

  enum { NUM_XLIB_FUNCTIONS = 77 };
  enum { NUM_XEXT_FUNCTIONS = 5 };
  enum { NUM_XRENDER_FUNCTIONS = 13 };

  union
  {
    struct
    {
      XBool (*pXSupportsLocale)(void);
      char* (*pXSetLocaleModifiers)(const char* /* modifierList */);

      XDisplay* (*pXOpenDisplay)(const char*);
      int (*pXCloseDisplay)(XDisplay*);

      XErrorHandler (*pXSetErrorHandler)(XErrorHandler /* func */);
      XIOErrorHandler (*pXSetIOErrorHandler)(XIOErrorHandler /* func */);
      int (*pXGetErrorText)(XDisplay*, int /* code */, char* /* bufferReturn */, int /* bufferLength */);

      XAtom (*pXInternAtom)(XDisplay*, const char* /* atomName */, XBool /* onlyIfExists */);
      XStatus (*pXInternAtoms)(XDisplay*, char** /* names */, int /* count */, XBool /* onlyIfExists */, XAtom* /* atomsReturn */);

      XID (*pXRootWindow)(XDisplay*, int /* screenNumber */);
      int (*pXDisplayWidth)(XDisplay*, int /* screenNumber */);
      int (*pXDisplayHeight)(XDisplay*, int /* screenNumber */);
      int (*pXDefaultScreen)(XDisplay*);
      XVisual* (*pXDefaultVisual)(XDisplay*, int /* screenNumber */);
      XColormap (*pXDefaultColormap)(XDisplay*, int /* screenNumber */);
      int (*pXDefaultDepth)(XDisplay*, int /* screenNumber */);

      int (*pXPending)(XDisplay*);
      int (*pXNextEvent)(XDisplay*, XEvent* /* xEvent */);
      XBool (*pXCheckTypedEvent)(XDisplay*, int /* eventType */, XEvent* /* eventReturn */);
      XBool (*pXCheckTypedWindowEvent)(XDisplay*, XID /* window */, int /* eventType */, XEvent* /* eventReturn */);
      XBool (*pXCheckWindowEvent)(XDisplay*, XID /* window */, long /* eventMask */, XEvent* /* eventReturn */);

      XIM (*pXOpenIM)(XDisplay*, void* /* rdb */, char* /* resName */, char* /* resClass */);
      XStatus (*pXCloseIM)(XIM /* im */);

      XIC (*pXCreateIC)(XIM /* im */, ...);
      void (*pXDestroyIC)(XIC /* ic */);
      int (*pXRefreshKeyboardMapping)(XMappingEvent* /* eventMap */);

      XID (*pXCreateWindow)(XDisplay*, XID /* parent */, int /* x */, int /* y */, uint /* width */, uint /* height */, uint /* boderWidth */, int /* depth */, uint /* class */, XVisual* /* visual */, ulong /* valueMask */, XSetWindowAttributes* /* attributes */);
      int (*pXDestroyWindow)(XDisplay*, XID /* window */);
      int (*pXMoveWindow)(XDisplay*, XID /* window */, int /* x */, int /* y */);
      int (*pXResizeWindow)(XDisplay*, XID /* window */, uint /* w */, uint /* h */);
      int (*pXMoveResizeWindow)(XDisplay*, XID /* window */, int /* x */, int /* y */, uint /* w */, uint /* h */);
      int (*pXMapWindow)(XDisplay*, XID /* window */);
      int (*pXUnmapWindow)(XDisplay*, XID /* window */);
      int (*pXRaiseWindow)(XDisplay*, XID /* window */);
      int (*pXLowerWindow)(XDisplay*, XID /* window */);
      int (*pXReparentWindow)(XDisplay*, XID /* window */, XID /* parent */, int /* x */, int /* y */);
      int (*pXChangeWindowAttributes)(XDisplay*, XID /* window */, ulong /* valueMask */, XSetWindowAttributes* /* attributes */);
      XStatus (*pXGetWindowAttributes)(XDisplay*, XID /* window */, XWindowAttributes* /* attributesReturns */);

      int (*pXSetInputFocus)(XDisplay*, XID /* window */, int /* revert_to */, XTime /* time */);
      int (*pXGetInputFocus)(XDisplay*, XID* /* window_return */, int* /* revert_to_return */);

      int (*pXSelectInput)(XDisplay* , XID /* window */, long /* event mask */);
      int (*pXSetNormalHints)(XDisplay*, XID /* window */, XSizeHints* /* hints */);

      int (*pXSetWMProtocols)(XDisplay*, XID /* window */, XAtom* /* protocols */, int /* count */);
      int (*pXGetWMProtocols)(XDisplay*, XID /* window */, XAtom** /* protocolsReturn */, int* /* countReturn */);

      int (*pXSetWMProperties)(XDisplay*, XID /* window */, XTextProperty* /* window_name*/, XTextProperty* icon_name, char** /* argv */, int /* argc */, XSizeHints* /* normal_hints*/, XWMHints* /* wm_hints */, XClassHint* /* class_hints */);

      int (*pXTranslateCoordinates)(XDisplay*, XWindow /* src_w */, XWindow /* dest_w */, int /* src_x */, int /* src_y */, int* /* dest_x_return */, int* /* dest_y_return */, XWindow* /* child_return */);
      int (*pXwcTextListToTextProperty)(XDisplay*, wchar_t** /* list */, int /* count */, XICCEncodingStyle /* style */, XTextProperty* /* textPropertyReturn */);

      int (*pXFlush)(XDisplay*);
      int (*pXSync)(XDisplay*, XBool /* discard */);

      GC (*pXCreateGC)(XDisplay*, XID /* drawable */, ulong /* valueMask */, XGCValues* /* values */);
      int (*pXFreeGC)(XDisplay*, GC /* gc */);

      KeySym (*pXKeycodeToKeysym)(XDisplay*, uint /* keyCode */, int /* index */);

      int (*pXLookupString)(XKeyEvent* /* event */, char* /* bufferReturn */, int /* bufferLength */, KeySym* /* keySymReturn */, XComposeStatus* /* statusInOut */);
      int (*pXwcLookupString)(XIC /* ic */, XKeyPressedEvent* /* event */, wchar_t* /* bufferReturn*/, int /* wcharsBuffer */, KeySym* /* keysymReturn */, XStatus* /* statusReturn */);

      int (*pXGrabPointer)(XDisplay*, XID /* grabWindow */, XBool /* ownerEvents */, uint /* eventMask */, int /* pointerMode */, int /* keyboardMode */, XID /* confineTo */, Cursor /* cursor */, XTime /* time */);
      int (*pXUngrabPointer)(XDisplay*, XTime /* time */);

      int (*pXGetPointerMapping)(XDisplay*, uchar* /* map_return*/, int /* namp*/);

      XID (*pXCreatePixmap)(XDisplay*, XID /* drawable */, uint /* width */, uint /* height */, uint /* depth */);
      int (*pXFreePixmap)(XDisplay*, XID /* pixmap */);
      int (*pXCopyArea)(XDisplay*, Drawable /* src*/, XID /* dest*/, GC /* gc */, int /* srcX */, int /* srcY */, uint /* width */, uint /* height */, int /* destX */, int /* destY */);
      int (*pXFree)(void* /* data */);

      XImage* (*pXCreateImage)(XDisplay*, XVisual* /* visual */, uint /* depth */, int /* format */, int /* offset */, char* /* data */, uint /* width */, uint /* height */, int /* bitmapPad */, int /* bytesPerLine */);
      int (*pXDestroyImage)(XImage* /* image */);
      int (*pXPutImage)(XDisplay*, XID /* drawable */, GC /* gc */, XImage* /* xImage */, int /* srcX */, int /* srcY */, int /* destX */, int /* destY */, uint /* width */, uint /* height */);

      XColormap (*pXCreateColormap)(XDisplay*, XID /* window */, XVisual* /* visual */, int /* alloc */);
      int (*pXFreeColormap)(XDisplay*, XColormap /* colormap */);

      XStatus (*pXAllocColor)(XDisplay*, XColormap, XColor* /* screen_in_out */);
      int (*pXFreeColors)(XDisplay*, XColormap, ulong* /* pixels */, int /* npixels */, ulong /* planes*/);

      XStatus (*pXMatchVisualInfo)(XDisplay*, int /* screen */, int /* depth */, int /* class */, XVisualInfo* /* vinfo_return */);

      // present in Xutil.h
      void (*pXSetWMName)(XDisplay*, XID /* window */, XTextProperty* /* textProperty */);

      XStatus (*pXSendEvent)(XDisplay*, XID /* window */, XBool, long, XEvent*);
      void (*pXChangeProperty)(XDisplay*, XID /*window */, XAtom, XAtom, int, int, unsigned char*, int);
      void (*pXSetSelectionOwner)(XDisplay*, XAtom, XWindow, XTime);
      char* (*pXGetAtomName)(XDisplay*, XAtom);
      void (*pXConvertSelection)(XDisplay*, XAtom, XAtom, XAtom, XWindow, XTime);
      int (*pXGetWindowProperty)(XDisplay*, XWindow, XAtom, long, long, XBool, XAtom, XAtom*, int*, unsigned long*, unsigned long*, unsigned char**);
      void (*pXDeleteProperty)(XDisplay*, XWindow, XAtom);
    };

    // Function pointers for loader
    void* xlibFunctions[NUM_XLIB_FUNCTIONS];
  };

  union
  {
    struct
    {
      XBool (*pXShmQueryVersion)(XDisplay*, int* /* majorVersion */, int* /* minorVersion */, XBool* /* sharedPixmaps */);
      XStatus (*pXShmAttach)(XDisplay*, XShmSegmentInfo* /* shmInfo */);
      XStatus (*pXShmDetach)(XDisplay*, XShmSegmentInfo* /* shmInfo */);
      XImage* (*pXShmCreateImage)(XDisplay*, XVisual* /* visual */, uint /* depth */, int /* format */, char* /* data */, XShmSegmentInfo* /* shmInfo */, uint /* width */, uint /* height */);
      Pixmap (*pXShmCreatePixmap)(XDisplay*, XID /* drawable */, char* /* data */, XShmSegmentInfo* /* shmInfo */, uint /* width */, uint /* height */, uint /* depth */);
    };

    // Function pointers for loader
    void* xextFunctions[NUM_XEXT_FUNCTIONS];
  };

  union
  {
    struct
    {
      XStatus (*pXRenderQueryVersion)(XDisplay*, int* /* major */, int * /* minor */);
      XStatus (*pXRenderQueryFormats)(XDisplay*);
      int (*pXRenderQuerySubpixelOrder)(XDisplay*, int /* screen */);
      XBool (*pXRenderSetSubpixelOrder)(XDisplay*, int /* screen */, int /* subpixel */);
      XRenderPictFormat* (*pXRenderFindVisualFormat)(XDisplay*, _Xconst XVisual* visual);
      XRenderPictFormat* (*pXRenderFindFormat)(XDisplay*, unsigned long /* mask */, _Xconst XRenderPictFormat* /* templ */, int /* count */);
      XRenderPictFormat* (*pXRenderFindStandardFormat)(XDisplay*, int /* format */);
      XPicture (*pXRenderCreatePicture)(XDisplay*, XID /* drawable */, _Xconst XRenderPictFormat* /* format */, unsigned long /* valuemask */, _Xconst XRenderPictureAttributes* /* attributes */);
      void (*pXRenderChangePicture)(XDisplay*, XPicture /* picture */, unsigned long /* valuemask */, _Xconst XRenderPictureAttributes* /* attributes */);
      void (*pXRenderSetPictureTransform)(XDisplay*, XPicture /* picture */, XTransform* /* transform */);
      void (*pXRenderFreePicture)(XDisplay*, XPicture /* picture*/);
      void (*pXRenderComposite)(XDisplay*, int /* op */, XPicture /* src */, XPicture /* mask */, XPicture /* dest */, int /* srcX */, int /* srcY */, int /* maskX */, int /* maskY */, int /* destX */, int /* destY */, unsigned int /* width */, unsigned int /* height */);
      Cursor (*pXRenderCreateCursor)(XDisplay*, XPicture /* source */, unsigned int /* x */, unsigned int /* y */);
    };

    // Function pointers for loader
    void* xrenderFunctions[NUM_XRENDER_FUNCTIONS];
  };

  err_t loadLibraries();

  // --------------------------------------------------------------------------
  // [X11 WM Support]
  // --------------------------------------------------------------------------

  XID _wmClientLeader;

  XID getWmClientLeader();
};

// ============================================================================
// [Fog::X11GuiWindow]
// ============================================================================

//! @brief X11 native window.
struct FOG_GUIENGINE_X11_API X11GuiWindow : public GuiWindow
{
  FOG_DECLARE_OBJECT(X11GuiWindow, GuiWindow)

  typedef XID Handle;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  X11GuiWindow(Widget* widget);
  virtual ~X11GuiWindow();

  // --------------------------------------------------------------------------
  // [Window Manipulation]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t flags);
  virtual err_t destroy();

  virtual err_t enable();
  virtual err_t disable();

  virtual err_t show(uint32_t state);
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

  virtual void setOwner(GuiWindow* owner);
  virtual void releaseOwner();

  virtual void setTransparency(float val);

  virtual void moveToTop(GuiWindow* w);
  virtual void moveToBottom(GuiWindow* w);

  // --------------------------------------------------------------------------
  // [Windowing System]
  // --------------------------------------------------------------------------

  virtual void onX11Event(XEvent* xe);

  // Tell window manager that we want's to move our window to our position
  // (it will be not discarded if we will specify PPosition)
  void setMoveableHints();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief X Input Context (XIC).
  XIC _xic;

  //! @brief Whether the x11 window is input only (default false)
  bool _inputOnly;
  //! @brief Whether the x11 window received MapRequest and currently is hidden.
  //! (It's requested to be mapped, but parent is hidden).
  bool _mapRequest;

  //! @brief X11 flags.
  uint _xflags;

  enum XFlag {
    XFlag_Configured
  };
};

// ============================================================================
// [Fog::X11GuiBackBuffer]
// ============================================================================

struct FOG_GUIENGINE_X11_API X11GuiBackBuffer : public GuiBackBuffer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  X11GuiBackBuffer();
  virtual ~X11GuiBackBuffer();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual bool resize(int width, int height, bool cache);
  virtual void destroy();
  virtual void updateRects(const IntBox* rects, sysuint_t count);

  void blitRects(XID target, const IntBox* rects, sysuint_t count);

  FOG_INLINE Pixmap getPixmap() const { return _pixmap; }
  FOG_INLINE XImage* getXImage() const { return _ximage; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Pixmap _pixmap;
  XShmSegmentInfo _shmi;
  XImage* _ximage;
};

// ============================================================================
// [Fog::X11GuiEventLoop]
// ============================================================================

//! @brief X11 event loop.
struct FOG_GUIENGINE_X11_API X11GuiEventLoop : public EventLoop
{
  X11GuiEventLoop();
  virtual ~X11GuiEventLoop();

protected:
  virtual void _runInternal();
  virtual void _scheduleWork();
  virtual void _scheduleDelayedWork(const Time& delayedWorkTime);

  void _doRunLoop();
  void _waitForWork();
  bool _xsync();
  bool _processNextXEvent();
  void _sendWakeUp();

  //! @brief 1 if wake-up byte was activated.
  Atomic<int> _wakeUpSent;

  //! @brief The time at which we should call @c scheduleDelayedWork().
  Time _delayedWorkTime;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_GUIENGINE_X11_P_H
