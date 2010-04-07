// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_GUIENGINE_H
#define _FOG_GUI_GUIENGINE_H

// [Dependencies]
#include <Fog/Core/Hash.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/Object.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Gui/Constants.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Application;
struct CancelableTask;
struct String;
struct Widget;

// Defined here.
struct GuiBackBuffer;
struct GuiEngine;
struct GuiWindow;

// ============================================================================
// [Fog::GuiEngine]
// ============================================================================

//! @brief Base for @c Fog::Widget or @c Fog::Layout classes.
//!
//! Layout item can be imagined as interface. It contains pure virtual methods
//! that's overriden by widgets or layouts.
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

  //! @brief Palette information.
  struct PaletteInfo
  {
    //! @brief Count of entities in palConv[] 2 to 256.
    uint32_t palCount;
    //! @brief Palette conversion (image quantization to 8 bits)
    //! Also for X server that is running in low resolution (8 bit or less).
    uint8_t palConv[256];
  };

  //! @brief Caret status.
  struct CaretStatus
  {
    Widget* widget;
    IntRect rect;
    Argb color;
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
    IntPoint position;
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
    IntPoint position;
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
  // [ID <-> GuiWindow]
  // --------------------------------------------------------------------------

  virtual bool mapHandle(void* handle, GuiWindow* w) = 0;
  virtual bool unmapHandle(void* handle) = 0;
  virtual GuiWindow* handleToNative(void* handle) const = 0;

  // --------------------------------------------------------------------------
  // [Display]
  // --------------------------------------------------------------------------

  virtual err_t getDisplayInfo(DisplayInfo* out) const = 0;
  virtual err_t getPaletteInfo(PaletteInfo* out) const = 0;

  virtual void updateDisplayInfo() = 0;

  // --------------------------------------------------------------------------
  // [Caret]
  // --------------------------------------------------------------------------

  virtual err_t getCaretStatus(CaretStatus* out) const = 0;

  // --------------------------------------------------------------------------
  // [Keyboard]
  // --------------------------------------------------------------------------

  virtual err_t getKeyboardStatus(KeyboardStatus* out) const = 0;
  virtual uint32_t getKeyboardModifiers() const = 0;
  virtual uint32_t keyToModifier(uint32_t key) const;

  // --------------------------------------------------------------------------
  // [Mouse]
  // --------------------------------------------------------------------------

  virtual err_t getMouseStatus(MouseStatus* out) const = 0;
  virtual err_t getSystemMouseStatus(SystemMouseStatus* out) const = 0;

  virtual void invalidateMouseStatus() = 0;
  virtual void updateMouseStatus() = 0;
  virtual void changeMouseStatus(Widget* w, const IntPoint& pos) = 0;

  virtual void clearSystemMouseStatus() = 0;

  virtual bool startButtonRepeat(uint32_t button, 
    bool reset, TimeDelta delay, TimeDelta interval) = 0;
  virtual bool stopButtonRepeat(uint32_t button) = 0;
  virtual void clearButtonRepeat() = 0;

  // --------------------------------------------------------------------------
  // [Wheel]
  // --------------------------------------------------------------------------

  virtual int getWheelLines() const = 0;
  virtual void setWheelLines(int count) = 0;

  // --------------------------------------------------------------------------
  // [Timing]
  // --------------------------------------------------------------------------

  virtual TimeDelta getRepeatingDelay() const = 0;
  virtual TimeDelta getRepeatingInterval() const = 0;
  virtual TimeDelta getDoubleClickInterval() const = 0;

  // --------------------------------------------------------------------------
  // [Windowing System]
  // --------------------------------------------------------------------------

  virtual void dispatchEnabled(Widget* w, bool enabled) = 0;
  virtual void dispatchVisibility(Widget* w, uint32_t visible) = 0;
  virtual void dispatchConfigure(Widget* w, const IntRect& rect, bool changedOrientation) = 0;

  //! @brief Called by widget destructor to erase all links to the widget from UIEngine.
  virtual void widgetDestroyed(Widget* w) = 0;

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  //! Tells application that some widget needs updating. This is key feature
  //! in the library that updating is in one place, so widgets can update()
  //! very often.
  virtual void update() = 0;

  //! @brief Runs updating. Do not use directly, use @c update() or you get into troubles.
  virtual void doUpdate() = 0;

  //! @brief Runs updating to specific window. This is internally done by 
  //! @c doUpdate() for all needed windows.
  virtual void doUpdateWindow(GuiWindow* window) = 0;

  //! @brief Blits window content into screen. Called usually from @c doUpdateWindow().
  virtual void doBlitWindow(GuiWindow* window, const IntBox* rects, sysuint_t count) = 0;

  // --------------------------------------------------------------------------
  // [GuiWindow Create / Destroy]
  // --------------------------------------------------------------------------

  virtual GuiWindow* createGuiWindow(Widget* widget) = 0;
  virtual void destroyGuiWindow(GuiWindow* native) = 0;


  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Lock.
  Lock _lock;

  //! @brief Library, if this UIEngine was opened as plugin.
  Library _library;

  //! @brief Whether UIEngine is correctly initialized.
  bool _initialized;

private:
  friend struct Application;
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
  virtual err_t move(const IntPoint& pt) = 0;
  virtual err_t resize(const IntSize& size) = 0;
  virtual err_t reconfigure(const IntRect& rect) = 0;

  virtual err_t takeFocus() = 0;

  virtual err_t setTitle(const String& title) = 0;
  virtual err_t getTitle(String& title) = 0;

  virtual err_t setIcon(const Image& icon) = 0;
  virtual err_t getIcon(Image& icon) = 0;

  virtual err_t setSizeGranularity(const IntPoint& pt) = 0;
  virtual err_t getSizeGranularity(IntPoint& pt) = 0;

  virtual err_t worldToClient(IntPoint* coords) = 0;
  virtual err_t clientToWorld(IntPoint* coords) = 0;

  // --------------------------------------------------------------------------
  // [Windowing System]
  // --------------------------------------------------------------------------

  virtual void onEnabled(bool enabled) = 0;
  virtual void onVisibility(uint32_t visible) = 0;

  virtual void onConfigure(const IntRect& windowRect, const IntRect& clientRect) = 0;

  virtual void onMouseHover(int x, int y) = 0;
  virtual void onMouseMove(int x, int y) = 0;
  virtual void onMouseLeave(int x, int y) = 0;

  virtual void onMousePress(uint32_t button, bool repeated) = 0;
  virtual void onMouseRelease(uint32_t button) = 0;
  virtual void onMouseWheel(uint32_t wheel) = 0;

  virtual void onFocus(bool focus) = 0;

  virtual bool onKeyPress(uint32_t key, uint32_t modifier, uint32_t systemCode, Char unicode) = 0;
  virtual bool onKeyRelease(uint32_t key, uint32_t modifier, uint32_t systemCode, Char unicode) = 0;

  virtual void clearFocus() = 0;
  virtual void setFocus(Widget* w, uint32_t reason) = 0;

  // --------------------------------------------------------------------------
  // [Dirty]
  // --------------------------------------------------------------------------

  virtual void setDirty() = 0;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Widget* getWidget() const { return _widget; }
  FOG_INLINE void* getHandle() const { return _handle; }
  FOG_INLINE GuiBackBuffer* getBackBuffer() const { return _backingStore; }

  FOG_INLINE bool isDirty() const { return _isDirty; }
  FOG_INLINE bool hasFocus() const { return _hasFocus; }

  FOG_INLINE bool isModal() const { return _modal; }
  FOG_INLINE void setModal(bool b) { _modal = b; }

  FOG_INLINE void setLastModalWindow(GuiWindow* w) {
    _lastmodal = w;
  }

  FOG_INLINE GuiWindow* getLastModalWindow() const {
    return _lastmodal;
  }

  // --------------------------------------------------------------------------
  // [Owner Handling]
  // --------------------------------------------------------------------------

  //SetOwner not only set the member variable
  //It should also do the z-order work of window manager!
  virtual void setOwner(GuiWindow* w) {
    _owner = w;
    //always set owner to toplevel window!
    GuiWindow* parent = _owner;
    SetWindowLong((HWND)getHandle(),GWLP_HWNDPARENT, (LONG)parent->getHandle());
  }

  virtual void releaseOwner() {
    SetWindowLong((HWND)getHandle(),GWLP_HWNDPARENT, (LONG)0);
    _owner = 0;
  }

  FOG_INLINE GuiWindow* getOwner() const { return _owner; }
  virtual void setTransparency(float val) = 0;

  // --------------------------------------------------------------------------
  // [Modal Window Handling]
  // --------------------------------------------------------------------------
  virtual void startModalWindow(GuiWindow* w) = 0;
  virtual void endModal(GuiWindow* w) = 0;
  virtual GuiWindow* getModalWindow() = 0;
  virtual void showAllModalWindows(uint32_t visible) = 0;

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

  GuiWindow* _owner;
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

  bool _modal;

  //! @brief Window bound rectangle.
  IntRect _windowRect;
  //! @brief Window client rectangle.
  IntRect _clientRect;

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
  virtual void updateRects(const IntBox* rects, sysuint_t count) = 0;

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

  FOG_INLINE ImageBuffer& getBuffer() { return _buffer; }

  FOG_INLINE uint8_t* getPixels() const { return _buffer.data; }
  FOG_INLINE int getWidth() const { return _buffer.width; }
  FOG_INLINE int getHeight() const { return _buffer.height; }
  FOG_INLINE int getFormat() const { return _buffer.format; }
  FOG_INLINE sysint_t getStride() const { return _buffer.stride; }

  FOG_INLINE int getCachedWidth() const { return _cachedWidth; }
  FOG_INLINE int getCachedHeight() const { return _cachedHeight; }

  FOG_INLINE uint8_t* getPrimaryPixels() const { return _primaryPixels; }
  FOG_INLINE sysint_t getPrimaryStride() const { return _primaryStride; }

  FOG_INLINE uint8_t* getSecondaryPixels() const { return _secondaryPixels; }
  FOG_INLINE sysint_t getSecondaryStride() const { return _secondaryStride; }

  FOG_INLINE void* getConvertFunc() const { return _convertFunc; }
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

  //! @brief Main painting back-buffer as @c ImageBuffer structure.
  //!
  //! These parameters are used for creating a painter, they aren't cached.
  //! Note that image stride can be larger than aligned (width * bytesPerPixel).
  ImageBuffer _buffer;

  //! @brief Cached width, can be greater or equal to @c _buffer.width.
  int _cachedWidth;
  //! @brief Cached height, can be greater or equal to @c _buffer.height.
  int _cachedHeight;

  //! @brief Cached 32-bit double buffer pixel data.
  uint8_t* _primaryPixels;
  //! @brief Cached 32-bit double buffer stride.
  sysint_t _primaryStride;

  //! @brief Cached 4/8/16/24/32 bit buffer pixel data (only created when needed).
  uint8_t* _secondaryPixels;
  //! @brief Cached 4/8/16/24/32 bit buffer stride (only created when needed).
  sysint_t _secondaryStride;

  //! @brief Converter used to convert pixels from secondary buffer to primary
  //! one.
  void* _convertFunc;
  //! @brief Converter depth.
  int _convertDepth;

  //! @brief Time when the backing store wes created (for caching).
  TimeTicks _createdTime;
  //! @brief Time when cached backing store will be expired.
  TimeTicks _expireTime;
};

} // Fog namespace

//! @addtogroup Fog_Gui
//! @}

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::GuiEngine::DisplayInfo, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::GuiEngine::PaletteInfo, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::GuiEngine::CaretStatus, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::GuiEngine::MouseStatus, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::GuiEngine::SystemMouseStatus, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::GuiEngine::KeyboardStatus, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GUI_GUIENGINE_H
