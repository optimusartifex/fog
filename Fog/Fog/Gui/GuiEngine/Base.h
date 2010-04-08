// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_GUIENGINE_BASE_H
#define _FOG_GUI_GUIENGINE_BASE_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Core/Timer.h>
#include <Fog/Gui/GuiEngine.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct BaseGuiWindow;
struct BaseGuiEngine;

// ============================================================================
// [Fog::BaseUIEngine]
// ============================================================================

//! @brief Default UIEngine implementation that is shared between Windows and
//! X11 UIEngine.
//!
//! The reason to make this class is fact that all UIEngines needs similar
//! things and only their API is different. Another reason is that anybody can
//! implement own UIEngine not based on this one reimplementing all abstract
//! methods.
struct FOG_API BaseGuiEngine : public GuiEngine
{
  FOG_DECLARE_OBJECT(BaseGuiEngine, GuiEngine)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BaseGuiEngine();
  virtual ~BaseGuiEngine();

  // --------------------------------------------------------------------------
  // [ID <-> GuiWindow]
  // --------------------------------------------------------------------------

  //! Widget mapper (ID <-> GuiWindow)
  typedef UnorderedHash<void*, GuiWindow*> WidgetMapper;

  virtual bool mapHandle(void* handle, GuiWindow* w);
  virtual bool unmapHandle(void* handle);
  virtual GuiWindow* handleToNative(void* handle) const;

  // --------------------------------------------------------------------------
  // [Display]
  // --------------------------------------------------------------------------

  virtual err_t getDisplayInfo(DisplayInfo* out) const;
  virtual err_t getPaletteInfo(PaletteInfo* out) const;

  // --------------------------------------------------------------------------
  // [Caret]
  // --------------------------------------------------------------------------

  virtual err_t getCaretStatus(CaretStatus* out) const;

  // --------------------------------------------------------------------------
  // [Keyboard]
  // --------------------------------------------------------------------------

  virtual err_t getKeyboardStatus(KeyboardStatus* out) const;
  virtual uint32_t getKeyboardModifiers() const;

  // --------------------------------------------------------------------------
  // [Mouse]
  // --------------------------------------------------------------------------

  virtual err_t getMouseStatus(MouseStatus* out) const;
  virtual err_t getSystemMouseStatus(SystemMouseStatus* out) const;

  virtual void invalidateMouseStatus();
  virtual void updateMouseStatus();
  virtual void changeMouseStatus(Widget* w, const IntPoint& pos);

  virtual void clearSystemMouseStatus();

  virtual bool startButtonRepeat(uint32_t button, 
    bool reset, TimeDelta delay, TimeDelta interval);
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

  virtual void dispatchConfigure(Widget* w, const IntRect& rect, bool changedOrientation);

  virtual void widgetDestroyed(Widget* w);

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  virtual void update();
  virtual void doUpdate();
  virtual void doUpdateWindow(GuiWindow* window);

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
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief ID <-> GuiWindow mapper.
  WidgetMapper _widgetMapper;

  //! @brief Circular list of dirty windows.
  List<BaseGuiWindow*> _dirtyList;
  
  //! @brief Display information.
  DisplayInfo _displayInfo;
  //! @brief Palette information.
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

  friend struct BaseGuiWindow;
};

// ============================================================================
// [Fog::BaseGuiWindow]
// ============================================================================

struct FOG_API BaseGuiWindow : public GuiWindow
{
  FOG_DECLARE_OBJECT(BaseGuiWindow, GuiWindow)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BaseGuiWindow(Widget* widget);
  virtual ~BaseGuiWindow();

  // --------------------------------------------------------------------------
  // [Windowing System]
  // --------------------------------------------------------------------------

  virtual void onEnabled(bool enabled);
  virtual void onVisibility(uint32_t visible);

  virtual void onConfigure(const IntRect& windowRect, const IntRect& clientRect);

  virtual void onMouseHover(int x, int y);
  virtual void onMouseMove(int x, int y);
  virtual void onMouseLeave(int x, int y);

  virtual void onMousePress(uint32_t button, bool repeated);
  virtual void onMouseRelease(uint32_t button);
  virtual void onMouseWheel(uint32_t wheel);

  virtual void onFocus(bool focus);

  virtual bool onKeyPress(uint32_t key, uint32_t modifier, uint32_t systemCode, Char unicode);
  virtual bool onKeyRelease(uint32_t key, uint32_t modifier, uint32_t systemCode, Char unicode);

  virtual void clearFocus();
  virtual void setFocus(Widget* w, uint32_t reason);

  // --------------------------------------------------------------------------
  // [PopUp - Handling]
  // --------------------------------------------------------------------------
  FOG_INLINE bool hasPopUp() const {
    return _popup.getLength() > 0;
  }

  void showPopUp(Widget*);
  void closePopUps();

  // --------------------------------------------------------------------------
  // [Modal - Handling]
  // --------------------------------------------------------------------------

  virtual void startModalWindow(GuiWindow* w) {    
    _modal = w;
    disable();
  }

  virtual void endModal(GuiWindow* w) {
    FOG_ASSERT(w == _modal);
    w->setModal(MODAL_NONE);
    enable();
    w->releaseOwner();

    _modal = 0;
  }

  //returns the first window, which is not modal!
  //On windows this is needed to minimize this window
  //instead of only minimize the modal window
  virtual GuiWindow* getModalBaseWindow() {
    GuiWindow* parent = this;
    while(parent->getOwner()) {
      parent = parent->getOwner();
    }

    return parent;
  }

  //show/hide/maximize/minimize all Modal-Windows
  virtual void showAllModalWindows(uint32_t visible);

  // --------------------------------------------------------------------------
  // [Dirty]
  // --------------------------------------------------------------------------

  virtual void setDirty();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Window title.
  String _title;
  //! @brief Window resize granularity.
  IntPoint _sizeGranularity;

  uint32_t _visibility;

  List<Widget*> _popup;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_GUIENGINE_BASE_H
