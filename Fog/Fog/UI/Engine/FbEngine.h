// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBENGINE_H
#define _FOG_UI_ENGINE_FBENGINE_H

// [Dependencies]
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Kernel/Timer.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Source/Color.h>

#include <Fog/UI/Engine/FbCaretState.h>
#include <Fog/UI/Engine/FbDisplayInfo.h>
#include <Fog/UI/Engine/FbKeyboardState.h>
#include <Fog/UI/Engine/FbMouseState.h>
#include <Fog/UI/Engine/FbPaletteInfo.h>

#if 0
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

struct UpdateStatus
{
  uint32_t scheduled;
  uint32_t updating;
  CancelableTask* task;
};
#endif

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbEngine]
// ============================================================================

//! @brief Frame-buffer engine (abstract).
struct FOG_API FbEngine : public Object
{
  FOG_DECLARE_OBJECT(FbEngine, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FbEngine();
  virtual ~FbEngine();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Lock& getLock() { return _lock; }

  // --------------------------------------------------------------------------
  // [Display / Palette]
  // --------------------------------------------------------------------------

  virtual const FbDisplayInfo* getDisplayInfo() const;
  virtual const FbPaletteInfo* getPaletteInfo() const;

  virtual void updateDisplayInfo() = 0;

  // --------------------------------------------------------------------------
  // [Handle <-> FbWindow]
  // --------------------------------------------------------------------------

  //! @brief Add a window @a handle and frame-buffer window @a w to the mapping.
  virtual err_t addHandle(void* handle, FbWindow* w);
  //! @brief Remove a window @a handle from the mapping.
  virtual err_t removeHandle(void* handle);

  //! @brief Translate window @a handle to @ref FbWindow.
  virtual FbWindow* getWindowByHandle(void* handle) const;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  virtual const FbCaretState* getCaretStatus() const;

  virtual const FbKeyboardState* getKeyboardState() const;

  virtual const FbMouseState* getMouseState(uint32_t id) const;

  // --------------------------------------------------------------------------
  // [Keyboard]
  // --------------------------------------------------------------------------

  virtual uint32_t keyToModifier(uint32_t key) const;

  // --------------------------------------------------------------------------
  // [Mouse]
  // --------------------------------------------------------------------------

  //virtual err_t getSystemMouseStatus(SystemMouseStatus* out) const;

  //virtual void invalidateMouseStatus();
  //virtual void updateMouseStatus();
  //virtual void changeMouseStatus(Widget* w, const PointI& pos);

  //virtual void clearSystemMouseStatus();

  //virtual bool startButtonRepeat(uint32_t button, bool reset, TimeDelta delay, TimeDelta interval);
  //virtual bool stopButtonRepeat(uint32_t button);
  //virtual void clearButtonRepeat();

  // --------------------------------------------------------------------------
  // [Wheel]
  // --------------------------------------------------------------------------

  virtual int getWheelLines() const;
  virtual void setWheelLines(int lines);

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
  virtual void doUpdateWindow(FbWindow* window);

  //! @brief Blits window content into screen. Called usually from @c doUpdateWindow().
  virtual void doBlitWindow(FbWindow* window, const BoxI* rects, size_t count) = 0;

  // --------------------------------------------------------------------------
  // [GuiWindow Create / Destroy]
  // --------------------------------------------------------------------------

  virtual FbWindow* createWindow() = 0;
  virtual void destroyWindow(FbWindow* window) = 0;

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  void _onButtonRepeatTimeOut(TimerEvent* e);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Each engine contains lock, used in case the FbWindows are created
  //! multi-threaded.
  Lock _lock;

  //! @brief Library, if the FbEngine was opened as a plugin (see @c X11FbEngine).
  Library _library;

  //! @brief Whether The GuiEngine is correctly initialized.
  bool _initialized;

  //! @brief Count of mouse devices (default 1).
  int _mouseDevicesCount;
  //! @brief Count of lines to scroll through mouse wheel.
  int _wheelLines;

  //! @brief Frame-buffer ID <-> FbWindow.
  Hash<void*, FbWindow*> _windowMap;

  //! @brief List of dirty frame-buffer windows which need to be updated.
  List<FbWindow*> _dirtyList;

  //! @brief Display information.
  FbDisplayInfo _displayInfo;
  //! @brief ImagePalette information.
  FbPaletteInfo _paletteInfo;

  //! @brief Keyboard status information.
  FbKeyboardState _keyboardStatus;

  //! @brief System mouse status information.
  FbMouseState _mouseStatus[16];

  //! @brief Caret status.
  FbCaretState _caretStatus;

  TimeDelta _repeatingDelay;
  TimeDelta _repeatingInterval;
  TimeDelta _doubleClickInterval;

  Timer _buttonRepeat[3];
  TimeDelta _buttonRepeatInterval[3];
  TimeTicks _buttonTime[3];

  //FbUpdateStatus _updateStatus;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBENGINE_H
