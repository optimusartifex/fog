// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_ENGINE_GUIENGINE_H
#define _FOG_GUI_ENGINE_GUIENGINE_H

// [Dependencies]
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Kernel/Timer.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_Gui_Engine
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct CancelableTask;
struct Widget;

// Defined here.
struct GuiBackBuffer;
struct GuiEngine;
struct GuiWindow;

struct Layout;

// ============================================================================
// [Fog::GuiEngine]
// ============================================================================

//! @brief Base for @c Fog::Widget or @c Fog::Layout classes.
//!
//! Layout item can be imagined as interface. It contains pure virtual methods
//! that's overridden by widgets or layouts.
struct FOG_API GuiEngine : public Object
{
  FOG_DECLARE_OBJECT(GuiEngine, Object)

  // --------------------------------------------------------------------------
  // [Structures]
  // --------------------------------------------------------------------------

  //! @brief Display information.
  struct DisplayInfo
  {
    //! @brief Screen width.
    uint32_t width;
    //! @brief Screen height.
    uint32_t height;
    //! @brief Screen depth.
    uint32_t depth;

    //! @brief Screen red mask.
    uint32_t rMask;
    //! @brief Screen green mask.
    uint32_t gMask;
    //! @brief Screen blue mask.
    uint32_t bMask;
    //! @brief If true, 16 bit depth is byteswapped (X Server). In other depths
    //! are byteswapped instead rMask, gMask and bMask values.
    uint32_t is16BitSwapped;
  };

  //! @brief ImagePalette information.
  struct PaletteInfo
  {
    //! @brief Count of entities in palConv[] 2 to 256.
    uint32_t palCount;
    //! @brief ImagePalette conversion (image quantization to 8 bits).
    //!
    //! Also used for X server that is running in low resolution (8 bit or less).
    uint8_t palConv[256];
  };

  //! @brief Caret status.
  struct CaretStatus
  {
    Widget* widget;
    RectI rect;
    Argb32 color;
    uint32_t type;
    uint32_t animation;
  };

  //! @brief Contains information about keyboard status.
  struct KeyboardStatus
  {
    uint8_t keymap[256];
    uint32_t modifiers;
  };

  //! @brief Contains information about mouse status.
  struct MouseStatus
  {
    //! @brief Widget where mouse is.
    Widget* widget;
    //! @brief Mouse position relative to @c widget.
    PointI position;
    //! @brief Hover state.
    uint32_t hover;
    //! @brief Pressed buttons.
    uint32_t buttons;
    //! @brief Whether this mouse status is valid.
    uint32_t valid;
  };

  struct SystemMouseStatus
  {
    //! @brief System window where mouse is (or NULL).
    GuiWindow* uiWindow;
    //! @brief Mouse position relative to @c uiWindow (in client area).
    PointI position;
    //! @brief Hover state.
    uint32_t hover;
    //! @brief Pressed buttons.
    uint32_t buttons;
  };

  struct UpdateStatus
  {
    uint32_t scheduled;
    uint32_t updating;
    CancelableTask* task;
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GuiEngine();
  virtual ~GuiEngine();

  // --------------------------------------------------------------------------
  // [Lock]
  // --------------------------------------------------------------------------

  FOG_INLINE Lock& lock() { return _lock; }

  // --------------------------------------------------------------------------
  // [Handle <-> GuiWindow]
  // --------------------------------------------------------------------------

  //! Widget mapper (Handle <-> GuiWindow)
  typedef Hash<void*, GuiWindow*> WidgetMapper;

  //! @brief Map windowing system handle to the @ref GuiWindow instance.
  virtual bool mapHandle(void* handle, GuiWindow* w);
  //! @brief Unmap windowing system handle.
  virtual bool unmapHandle(void* handle);
  //! @brief Translate windowing system handle to @ref GuiWindow instance.
  virtual GuiWindow* getWindowFromHandle(void* handle) const;

  // --------------------------------------------------------------------------
  // [Display]
  // --------------------------------------------------------------------------

  virtual err_t getDisplayInfo(DisplayInfo* out) const;
  virtual err_t getPaletteInfo(PaletteInfo* out) const;

  virtual void updateDisplayInfo() = 0;

  // --------------------------------------------------------------------------
  // [Caret]
  // --------------------------------------------------------------------------

  virtual err_t getCaretStatus(CaretStatus* out) const;

  // --------------------------------------------------------------------------
  // [Keyboard]
  // --------------------------------------------------------------------------

  virtual err_t getKeyboardStatus(KeyboardStatus* out) const;
  virtual uint32_t getKeyboardModifiers() const;
  virtual uint32_t keyToModifier(uint32_t key) const;

  // --------------------------------------------------------------------------
  // [Mouse]
  // --------------------------------------------------------------------------

  virtual err_t getMouseStatus(MouseStatus* out) const;
  virtual err_t getSystemMouseStatus(SystemMouseStatus* out) const;

  virtual void invalidateMouseStatus();
  virtual void updateMouseStatus();
  virtual void changeMouseStatus(Widget* w, const PointI& pos);

  virtual void clearSystemMouseStatus();

  virtual bool startButtonRepeat(uint32_t button, bool reset, TimeDelta delay, TimeDelta interval);
  virtual bool stopButtonRepeat(uint32_t button);
  virtual void clearButtonRepeat();

  // --------------------------------------------------------------------------
  // [Wheel]
  // --------------------------------------------------------------------------

  virtual int getWheelLines() const;
  virtual void setWheelLines(int count);

  // --------------------------------------------------------------------------
  // [Timing]
  // --------------------------------------------------------------------------

  virtual TimeDelta getRepeatingDelay() const;
  virtual TimeDelta getRepeatingInterval() const;
  virtual TimeDelta getDoubleClickInterval() const;

  // --------------------------------------------------------------------------
  // [Windowing System]
  // --------------------------------------------------------------------------

  virtual void dispatchEnabled(Widget* w, bool enabled);
  virtual void dispatchVisibility(Widget* w, uint32_t visible);
  virtual void dispatchConfigure(Widget* w, const RectI& rect, bool changedOrientation);

  //! @brief Called by widget destructor to erase all links to the widget from UIEngine.
  virtual void widgetDestroyed(Widget* w);

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  //! Tells application that some widget needs updating. This is key feature
  //! in the library that updating is in one place, so widgets can update()
  //! very often.
  virtual void update();

  //! @brief Runs updating. Do not use directly, use @c update() or you get into troubles.
  virtual void doUpdate();

  //! @brief Runs updating to specific window. This is internally done by
  //! @c doUpdate() for all needed windows.
  virtual void doUpdateWindow(GuiWindow* window);

  //! @brief Blits window content into screen. Called usually from @c doUpdateWindow().
  virtual void doBlitWindow(GuiWindow* window, const BoxI* rects, size_t count) = 0;

  // --------------------------------------------------------------------------
  // [GuiWindow Create / Destroy]
  // --------------------------------------------------------------------------

  virtual GuiWindow* createGuiWindow(Widget* widget) = 0;
  virtual void destroyGuiWindow(GuiWindow* native) = 0;

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  void _onButtonRepeatTimeOut(TimerEvent* e);

  // --------------------------------------------------------------------------
  // [Modality]
  // --------------------------------------------------------------------------

/*
  //!@brief makes the GuiWindow to be shown as Modal window in front of this window
  virtual void startModalWindow(GuiWindow* w) = 0;
  //!@brief The modal GuiWindow is being closed, so the modality should be removed
  virtual void endModal(GuiWindow* w) = 0;
  //!@brief call show(visible) on all modal windows on the stack (last to first)
  virtual void showAllModalWindows(uint32_t visible) = 0;

  //!@brief tmp variable to set the last modal window int the stack
  //! so we can iterate the modal stack from behind again (without double linked list)
  FOG_INLINE void setLastModalWindow(GuiWindow* w)
  {
    _lastmodal = w;
  }
  FOG_INLINE GuiWindow* getLastModalWindow() const
  {
    return _lastmodal;
  }
  //!@brief returns the current modalWindow
  GuiWindow* getModalWindow() const {
    return _modal;
  }*/



  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Lock.
  Lock _lock;

  //! @brief Library, if this UIEngine was opened as plugin.
  Library _library;

  //! @brief Whether The GuiEngine is correctly initialized.
  bool _initialized;

  GuiWindow* _modal;
  GuiWindow* _lastmodal;

  //! @brief ID <-> GuiWindow mapper.
  WidgetMapper _widgetMapper;

  //! @brief Circular list of dirty windows.
  List<GuiWindow*> _dirtyList;

  //! @brief Display information.
  DisplayInfo _displayInfo;
  //! @brief ImagePalette information.
  PaletteInfo _paletteInfo;

  //! @brief Caret status.
  CaretStatus _caretStatus;

  //! @brief Keyboard status information.
  KeyboardStatus _keyboardStatus;

  //! @brief Count of mouse devices (1 is default).
  int _mouseDevices;
  //! @brief Mouse status information.
  MouseStatus _mouseStatus;
  //! @brief System mouse status information.
  SystemMouseStatus _systemMouseStatus;

  //! @brief Count of lines to scroll through mouse wheel.
  int _wheelLines;

  TimeDelta _repeatingDelay;
  TimeDelta _repeatingInterval;
  TimeDelta _doubleClickInterval;

  Timer _buttonRepeat[3];
  TimeDelta _buttonRepeatInterval[3];
  TimeTicks _buttonTime[3];

  UpdateStatus _updateStatus;
};

// ============================================================================
// [Fog::GuiWindow]
// ============================================================================

struct FOG_API GuiWindow : public Object
{
  FOG_DECLARE_OBJECT(GuiWindow, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GuiWindow(Widget* widget);
  virtual ~GuiWindow();

  // --------------------------------------------------------------------------
  // [Window Manipulation]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t flags) = 0;
  virtual err_t destroy() = 0;

  virtual err_t enable() = 0;
  virtual err_t disable() = 0;

  virtual err_t show(uint32_t state) = 0;
  virtual err_t hide() = 0;

  virtual err_t setPosition(const PointI& pos) = 0;
  virtual err_t setSize(const SizeI& size) = 0;
  virtual err_t setGeometry(const RectI& geometry) = 0;

  virtual err_t takeFocus() = 0;

  virtual err_t setTitle(const StringW& title) = 0;
  virtual err_t getTitle(StringW& title) = 0;

  virtual err_t setIcon(const Image& icon) = 0;
  virtual err_t getIcon(Image& icon) = 0;

  virtual err_t setSizeGranularity(const PointI& pt) = 0;
  virtual err_t getSizeGranularity(PointI& pt) = 0;

  virtual err_t worldToClient(PointI* coords) = 0;
  virtual err_t clientToWorld(PointI* coords) = 0;

  // --------------------------------------------------------------------------
  // [Windowing System]
  // --------------------------------------------------------------------------

  virtual void onEnabled(bool enabled);
  virtual void onVisibility(uint32_t visible);

  virtual void onGeometry(const RectI& windowRect, const RectI& clientRect);

  virtual void onMouseHover(int x, int y);
  virtual void onMouseMove(int x, int y);
  virtual void onMouseLeave(int x, int y);

  virtual void onMousePress(uint32_t button, bool repeated);
  virtual void onMouseRelease(uint32_t button);
  virtual void onMouseWheel(uint32_t wheel);

  virtual void onFocus(bool focus);

  virtual bool onKeyPress(uint32_t key, uint32_t modifier, uint32_t systemCode, CharW unicode);
  virtual bool onKeyRelease(uint32_t key, uint32_t modifier, uint32_t systemCode, CharW unicode);

  virtual void setFocus(Widget* w, uint32_t reason);
  virtual void resetFocus();

  // --------------------------------------------------------------------------
  // [Dirty]
  // --------------------------------------------------------------------------

  virtual void setDirty();

  // --------------------------------------------------------------------------
  // [PopUp]
  // --------------------------------------------------------------------------
  FOG_INLINE bool hasPopUp() const { return _popup.getLength() > 0; }

  void showPopUp(Widget*);
  void closePopUps();

  // --------------------------------------------------------------------------
  // [Modal]
  // --------------------------------------------------------------------------

  //! @brief makes the GuiWindow to be shown as Modal window in front of this window
  virtual void startModalWindow(GuiWindow* w);
  //! @brief The modal GuiWindow is being closed, so the modality should be removed
  virtual void endModal(GuiWindow* w);

  //! @brief Returns the first window, which is not modal! Under windows this is
  //! needed to minimize this window instead of only minimize the modal window.
  virtual GuiWindow* getModalBaseWindow();

  //! @brief call show(visible) on all modal windows on the stack (last to first)
  virtual void showAllModalWindows(uint32_t visible);

  //! @brief tmp variable to set the last modal window int the stack
  //! so we can iterate the modal stack from behind again (without double linked list)
  FOG_INLINE void setLastModalWindow(GuiWindow* w) { _lastmodal = w; }

  FOG_INLINE GuiWindow* getLastModalWindow() const { return _lastmodal; }

  //!@brief returns the current modalWindow
  virtual GuiWindow* getModalWindow();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Widget* getWidget() const { return _widget; }
  FOG_INLINE void* getHandle() const { return _handle; }
  FOG_INLINE GuiBackBuffer* getBackBuffer() const { return _backingStore; }

  FOG_INLINE bool isDirty() const { return _isDirty; }
  FOG_INLINE bool hasFocus() const { return _hasFocus; }

  FOG_INLINE bool isModal() const { return _modalpolicy != MODAL_NONE; }
  FOG_INLINE void setModal(MODAL_POLICY b) { _modalpolicy = b; }
  FOG_INLINE MODAL_POLICY getModality() const { return _modalpolicy; }

  // --------------------------------------------------------------------------
  // [Owner Handling]
  // --------------------------------------------------------------------------

  FOG_INLINE GuiWindow* getOwner() const { return _owner; }

  // SetOwner not only set the member variable. It should also do the z-order
  // work of window manager! (make sure owner is always behind the child).
  virtual void setOwner(GuiWindow* w) = 0;

  // Releases the owner from the child window and makes sure the owner will
  // get the focus.
  virtual void releaseOwner() = 0;

  virtual void setTransparency(float val) = 0;

  // --------------------------------------------------------------------------
  // [Z-Order]
  // --------------------------------------------------------------------------

  //Move Window on Top of other Window! (If w == 0 Move on top of all Windows)
  virtual void moveToTop(GuiWindow* w) = 0;
  //Move Window behind other Window! (If w == 0 Move behind all Windows of screen)
  virtual void moveToBottom(GuiWindow* w) = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Widget.
  Widget* _widget;
  Layout* _activatelist;

  //our owner.
  GuiWindow* _owner;
  GuiWindow* _modal;
  GuiWindow* _lastmodal;

  //! @brief Window handle.
  void* _handle;

  //! @brief Backing store.
  GuiBackBuffer* _backingStore;

  //TODO: use bitset here

  //! @brief Whether window is enabled.
  bool _enabled;

  //! @brief Whether window is visible.
  bool _visible; //do we need this? (already in widget)

  //! @brief Whether window has native windowing system focus.
  bool _hasFocus;

  //! @brief Whether window needs blit to screen.
  bool _needBlit;

  //! @brief Window is dirty and needs update.
  bool _isDirty;

  MODAL_POLICY _modalpolicy;

  //! @brief Window bound rectangle.
  RectI _windowRect;
  //! @brief Window client rectangle.
  RectI _clientRect;

  //! @brief Window title.
  StringW _title;
  //! @brief Window resize granularity.
  PointI _sizeGranularity;

  uint32_t _visibility;

  List<Widget*> _popup;

  friend struct GuiEngine;
  friend struct Widget;
};

// ============================================================================
// [Fog::GuiBackBuffer]
// ============================================================================

//! @brief Provides cacheable backing store for system windows (@c GuiWindow).
struct FOG_API GuiBackBuffer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GuiBackBuffer();
  virtual ~GuiBackBuffer();

  // --------------------------------------------------------------------------
  // [Interface Methods]
  // --------------------------------------------------------------------------

  virtual bool resize(int width, int height, bool cache) = 0;
  virtual void destroy() = 0;
  virtual void updateRects(const BoxI* rects, size_t count) = 0;

  // --------------------------------------------------------------------------
  // [Generic Methods]
  // --------------------------------------------------------------------------

  void _clear();
  bool expired(TimeTicks now) const;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE int getDepth() const { return _depth; }

  FOG_INLINE ImageBits& getBuffer() { return _buffer; }

  FOG_INLINE uint8_t* getPixels() const { return _buffer.data; }
  FOG_INLINE const SizeI& getSize() const { return _buffer.size; }
  FOG_INLINE uint32_t getFormat() const { return _buffer.format; }
  FOG_INLINE ssize_t getStride() const { return _buffer.stride; }

  FOG_INLINE const SizeI& getCachedSize() const { return _cachedSize; }

  FOG_INLINE uint8_t* getPrimaryPixels() const { return _primaryPixels; }
  FOG_INLINE ssize_t getPrimaryStride() const { return _primaryStride; }

  FOG_INLINE uint8_t* getSecondaryPixels() const { return _secondaryPixels; }
  FOG_INLINE ssize_t getSecondaryStride() const { return _secondaryStride; }

  FOG_INLINE ImageConverterBlitLineFunc getConvertFunc() const { return _convertFunc; }
  FOG_INLINE TimeTicks getCreatedTime() const { return _createdTime; }
  FOG_INLINE TimeTicks getExpireTime() const { return _expireTime; }

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  enum TYPE
  {
    TYPE_NONE = 0,

    TYPE_WIN_DIB = 1,

    TYPE_X11_XSHM_PIXMAP = 2,
    TYPE_X11_XIMAGE = 3,
    TYPE_X11_XIMAGE_WITH_PIXMAP = 4
  };

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Type of back buffer.
  uint32_t _type;
  //! @brief Depth of back buffer.
  int _depth;

  //! @brief Main painting back-buffer as @c ImageBits structure.
  //!
  //! These parameters are used for creating a painter, they aren't cached.
  //! Note that image stride can be larger than aligned (width * bytesPerPixel).
  ImageBits _buffer;

  //! @brief Cached size, can be greater or equal to @c _buffer.size.
  SizeI _cachedSize;

  //! @brief Cached 32-bit double buffer pixel data.
  uint8_t* _primaryPixels;
  //! @brief Cached 32-bit double buffer stride.
  ssize_t _primaryStride;

  //! @brief Cached 4/8/16/24/32 bit buffer pixel data (only created when needed).
  uint8_t* _secondaryPixels;
  //! @brief Cached 4/8/16/24/32 bit buffer stride (only created when needed).
  ssize_t _secondaryStride;

  //! @brief Converter used to convert pixels from secondary to primary buffer.
  ImageConverterBlitLineFunc _convertFunc;
  //! @brief Converter depth.
  int _convertDepth;

  //! @brief Time when the backing store wes created (for caching).
  TimeTicks _createdTime;
  //! @brief Time when cached backing store will be expired.
  TimeTicks _expireTime;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPE_DECLARE(Fog::GuiEngine::DisplayInfo, Fog::TYPE_CATEGORY_SIMPLE)
_FOG_TYPE_DECLARE(Fog::GuiEngine::PaletteInfo, Fog::TYPE_CATEGORY_SIMPLE)
_FOG_TYPE_DECLARE(Fog::GuiEngine::CaretStatus, Fog::TYPE_CATEGORY_SIMPLE)
_FOG_TYPE_DECLARE(Fog::GuiEngine::MouseStatus, Fog::TYPE_CATEGORY_SIMPLE)
_FOG_TYPE_DECLARE(Fog::GuiEngine::SystemMouseStatus, Fog::TYPE_CATEGORY_SIMPLE)
_FOG_TYPE_DECLARE(Fog::GuiEngine::KeyboardStatus, Fog::TYPE_CATEGORY_SIMPLE)

// [Guard]
#endif // _FOG_GUI_ENGINE_GUIENGINE_H
