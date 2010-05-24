// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Application.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Painter.h>
#include <Fog/Gui/GuiEngine.h>
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Gui/Widget_p.h>

FOG_IMPLEMENT_OBJECT(Fog::GuiEngine)
FOG_IMPLEMENT_OBJECT(Fog::GuiWindow)

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

#define GUI_ENGINE() \
  Application::getInstance()->getGuiEngine()

static uint32_t fogButtonToId(uint32_t button)
{
  switch (button)
  {
    case BUTTON_LEFT  : return 0;
    case BUTTON_MIDDLE: return 1;
    case BUTTON_RIGHT : return 2;

    default:
      return BUTTON_INVALID;
  }
}

// ============================================================================
// [Fog::GuiEngine]
// ============================================================================

GuiEngine::GuiEngine()
{
  _initialized = false;

  // Clear all.
  memset(&_displayInfo, 0, sizeof(_displayInfo));
  memset(&_paletteInfo, 0, sizeof(_paletteInfo));
  memset(&_caretStatus, 0, sizeof(_caretStatus));
  memset(&_keyboardStatus, 0, sizeof(_keyboardStatus));
  memset(&_mouseStatus, 0, sizeof(_mouseStatus));
  memset(&_systemMouseStatus, 0, sizeof(_systemMouseStatus));

  _mouseStatus.position.set(INT_MIN, INT_MIN);
  _systemMouseStatus.position.set(INT_MIN, INT_MIN);

  _wheelLines          = 3;
  _repeatingDelay      = TimeDelta::fromMilliseconds(200);
  _repeatingInterval   = TimeDelta::fromMilliseconds(50);
  _doubleClickInterval = TimeDelta::fromMilliseconds(200);

  _buttonRepeat[0].addListener(EVENT_TIMER, this, &GuiEngine::_onButtonRepeatTimeOut);
  _buttonRepeat[1].addListener(EVENT_TIMER, this, &GuiEngine::_onButtonRepeatTimeOut);
  _buttonRepeat[2].addListener(EVENT_TIMER, this, &GuiEngine::_onButtonRepeatTimeOut);

  memset(&_updateStatus, 0, sizeof(_updateStatus));
}

GuiEngine::~GuiEngine()
{
  if (_updateStatus.task) _updateStatus.task->cancel();
}

// ============================================================================
// [Fog::GuiEngine - Handle <-> GuiWindow]
// ============================================================================

bool GuiEngine::mapHandle(void* handle, GuiWindow* native)
{
  return _widgetMapper.put(handle, native, true) == ERR_OK;
}

bool GuiEngine::unmapHandle(void* handle)
{
  bool b= _widgetMapper.remove(handle);
  if (_widgetMapper.getLength() == 0)
  {
    Event e(EVENT_LAST_WINDOW_CLOSED);
    Application::getInstance()->sendEvent(&e);
  }

  return b;
}

GuiWindow* GuiEngine::getWindowFromHandle(void* handle) const
{
  return _widgetMapper.value(handle, NULL);
}

// ============================================================================
// [Fog::GuiEngine - Display]
// ============================================================================

err_t GuiEngine::getDisplayInfo(DisplayInfo* out) const
{
  memcpy(out, &_displayInfo, sizeof(DisplayInfo));
  return ERR_OK;
}

err_t GuiEngine::getPaletteInfo(PaletteInfo* out) const
{
  memcpy(out, &_paletteInfo, sizeof(PaletteInfo));
  return ERR_OK;
}

// ============================================================================
// [Fog::GuiEngine - Caret]
// ============================================================================

err_t GuiEngine::getCaretStatus(CaretStatus* out) const
{
  memcpy(out, &_caretStatus, sizeof(CaretStatus));
  return ERR_OK;
}

// ============================================================================
// [Fog::GuiEngine - Keyboard]
// ============================================================================

err_t GuiEngine::getKeyboardStatus(KeyboardStatus* out) const
{
  memcpy(out, &_keyboardStatus, sizeof(KeyboardStatus));
  return ERR_OK;
}

uint32_t GuiEngine::getKeyboardModifiers() const
{
  return _keyboardStatus.modifiers;
}

uint32_t GuiEngine::keyToModifier(uint32_t key) const
{
  switch (key)
  {
    case KEY_LEFT_SHIFT : return MODIFIER_LEFT_SHIFT ;
    case KEY_RIGHT_SHIFT: return MODIFIER_RIGHT_SHIFT;
    case KEY_LEFT_CTRL  : return MODIFIER_LEFT_CTRL  ;
    case KEY_RIGHT_CTRL : return MODIFIER_RIGHT_CTRL ;
    case KEY_LEFT_ALT   : return MODIFIER_LEFT_ALT   ;
    case KEY_RIGHT_ALT  : return MODIFIER_RIGHT_ALT  ;
  }
  return 0;
}

// ============================================================================
// [Fog::GuiEngine - Mouse]
// ============================================================================

err_t GuiEngine::getMouseStatus(MouseStatus* out) const
{
  memcpy(out, &_mouseStatus, sizeof(MouseStatus));
  return ERR_OK;
}

err_t GuiEngine::getSystemMouseStatus(SystemMouseStatus* out) const
{
  memcpy(out, &_systemMouseStatus, sizeof(SystemMouseStatus));
  return ERR_OK;
}

void GuiEngine::invalidateMouseStatus()
{
  _mouseStatus.valid = false;
}

void GuiEngine::updateMouseStatus()
{
}

void GuiEngine::changeMouseStatus(Widget* w, const IntPoint& pos)
{
  if (_mouseStatus.widget != w)
  {
    // Clear button press times (double click).
    _buttonTime[0].clear();
    _buttonTime[1].clear();
    _buttonTime[2].clear();

    Widget* before = _mouseStatus.widget;

    _mouseStatus.widget = w;
    _mouseStatus.position = pos;
    _mouseStatus.hover = true;
    _mouseStatus.buttons = 0;
    _mouseStatus.valid = true;

    // Send MouseOut to widget where the mouse was before.
    if (before)
    {
      MouseEvent e(EVENT_MOUSE_OUT);
      e._button = BUTTON_INVALID;
      e._modifiers = getKeyboardModifiers();
      e._position = pos;
      Widget::translateCoordinates(before, w, &e._position);
      before->sendEvent(&e);
    }

    {
      MouseEvent e(EVENT_MOUSE_IN);
      e._button = BUTTON_INVALID;
      e._modifiers = getKeyboardModifiers();
      e._position = pos;
      w->sendEvent(&e);
    }
  }
  else if (_mouseStatus.position != pos)
  {
    // Clear button press times (double click).
    _buttonTime[0].clear();
    _buttonTime[1].clear();
    _buttonTime[2].clear();

    uint32_t code = 0; // Be quite.
    uint32_t hoverChange;;

    hoverChange = (!!_mouseStatus.hover) |
                  ((!(pos.getX() < 0 || 
                      pos.getY() < 0 ||
                      pos.getX() >= w->_geometry.w ||
                      pos.getY() >= w->_geometry.h)) << 1);

    enum HOVER_CHANGE
    {
      HOVER_CHANGE_OUTSIDE_MOVE = 0x00, // 00b
      HOVER_CHANGE_OUT          = 0x01, // 01b
      HOVER_CHANGE_IN           = 0x02, // 10b
      HOVER_CHANGE_INSIDE_MOVE  = 0x03  // 11b
    };

    switch (hoverChange)
    {
      case HOVER_CHANGE_OUTSIDE_MOVE: code = EVENT_MOUSE_MOVE; break;
      case HOVER_CHANGE_OUT:          code = EVENT_MOUSE_OUT ; break;
      case HOVER_CHANGE_IN:           code = EVENT_MOUSE_IN  ; break;
      case HOVER_CHANGE_INSIDE_MOVE:  code = EVENT_MOUSE_MOVE; break;
    }

    _mouseStatus.position = pos;
    _mouseStatus.hover = (hoverChange & 0x02) != 0;

    {
      MouseEvent e(code);
      e._button = BUTTON_INVALID;
      e._modifiers = getKeyboardModifiers();
      e._position = pos;
      e._isOutside = (hoverChange == 0x00);
      w->sendEvent(&e);
    }

    // Generate MouseMove events if generated event was MouseOut and widget is
    // in dragging state.
    if (_mouseStatus.buttons && code == EVENT_MOUSE_OUT)
    {
      MouseEvent e(EVENT_MOUSE_MOVE);
      e._button = BUTTON_INVALID;
      e._modifiers = getKeyboardModifiers();
      e._position = pos;
      e._isOutside = true;
      w->sendEvent(&e);
    }
  }
}

void GuiEngine::clearSystemMouseStatus()
{
  _systemMouseStatus.uiWindow = NULL;
  _systemMouseStatus.position.set(INT_MIN, INT_MIN);
  _systemMouseStatus.hover = false;
  _systemMouseStatus.buttons = 0;
}

bool GuiEngine::startButtonRepeat(uint32_t button, bool reset, TimeDelta delay, TimeDelta interval)
{
  uint32_t id = fogButtonToId(button);
  if (id == BUTTON_INVALID) return false;

  if (!_buttonRepeat[id].isRunning())
  {
    _buttonRepeat[id].setInterval(delay);
    _buttonRepeat[id].start();
    _buttonRepeatInterval[id] = interval;
  }
  else if (_buttonRepeat[id].isRunning() && reset)
  {
    _buttonRepeat[id].stop();
    _buttonRepeat[id].setInterval(interval);
    _buttonRepeat[id].start();
    _buttonRepeatInterval[id] = interval;
  }
  return true;
}

bool GuiEngine::stopButtonRepeat(uint32_t button)
{
  uint32_t id = fogButtonToId(button);
  if (id == BUTTON_INVALID) return false;

  if (_buttonRepeat[id].isRunning())
  {
    _buttonRepeat[id].stop();
  }
  return true;
}

void GuiEngine::clearButtonRepeat()
{
  for (sysuint_t i = 0; i < FOG_ARRAY_SIZE(_buttonRepeat); i++)
  {
    if (_buttonRepeat[i].isRunning()) _buttonRepeat[i].stop();
  }
}

// ============================================================================
// [Fog::GuiEngine - Wheel]
// ============================================================================

int GuiEngine::getWheelLines() const
{
  return _wheelLines;
}

void GuiEngine::setWheelLines(int count)
{
  _wheelLines = count;
}

// ============================================================================
// [Fog::GuiEngine - Timing]
// ============================================================================

TimeDelta GuiEngine::getRepeatingDelay() const
{
  return _repeatingDelay;
}

TimeDelta GuiEngine::getRepeatingInterval() const
{
  return _repeatingInterval;
}

TimeDelta GuiEngine::getDoubleClickInterval() const
{
  return _doubleClickInterval;
}

// ============================================================================
// [Fog::GuiEngine - Windowing System]
// ============================================================================

void GuiEngine::dispatchEnabled(Widget* w, bool enabled)
{
  uint32_t state = w->getState();

  // Dispatch 'Enable'.
  if (enabled)
  {
    if (state == WIDGET_ENABLED) return;
    if (state == WIDGET_DISABLED_BY_PARENT && !w->hasGuiWindow()) return;

    w->_state = WIDGET_ENABLED;

    StateEvent e(EVENT_ENABLE);
    w->sendEvent(&e);

    FOG_WIDGET_TREE_ITERATOR(i1, w, true,
      // before traverse
      {
        // show only child that's hidden by parent
        if (child->getState() != WIDGET_DISABLED_BY_PARENT) FOG_WIDGET_TREE_ITERATOR_NEXT(i1);

        child->_state = WIDGET_ENABLED;
        child->sendEvent(&e);
      },
      // after traverse
      {
      }
    );

    w->update(WIDGET_UPDATE_ALL);
  }
  // Dispatch 'Disable'.
  else
  {
    uint32_t state = w->getState();
    if (state == WIDGET_DISABLED) return;

    w->_state = WIDGET_DISABLED;

    //StateEvent e(toState == Widget::Disabled
    //  ? EvDisable
    //  : EvDisableByParent);
    StateEvent e(EVENT_DISABLE);
    w->sendEvent(&e);

    if (state != WIDGET_ENABLED) return;

    e._code = EVENT_DISABLE_BY_PARENT;

    FOG_WIDGET_TREE_ITERATOR(i2, w, true,
      // before traverse
      {
        // Mark by 'DisableByParent' all childs that's visible.
        if (child->getState() != WIDGET_ENABLED)
        {
          FOG_WIDGET_TREE_ITERATOR_NEXT(i2);
        }
        else
        {
          child->_state = WIDGET_DISABLED_BY_PARENT;
          child->sendEvent(&e);
        }
      },
      // after traverse
      {
      }
    );

    w->update(WIDGET_UPDATE_ALL);
  }
}

void GuiEngine::dispatchVisibility(Widget* w, uint32_t visible)
{
  uint32_t visibility = w->getVisibility();
  if (visibility == visible) return;

  // Dispatch 'Show'.
  if (visible >= WIDGET_VISIBLE)
  {
    if (visibility == WIDGET_HIDDEN_BY_PARENT && !w->hasGuiWindow()) return;

    w->_visibility = visible;

    uint32_t code = EVENT_SHOW;
    if (visible == WIDGET_VISIBLE_FULLSCREEN) code = EVENT_SHOW_FULLSCREEN;
    else if (visible == WIDGET_VISIBLE_MAXIMIZED) code = EVENT_SHOW_MAXIMIZE;

    VisibilityEvent e(code);
    w->sendEvent(&e);

    if (visibility == WIDGET_HIDDEN)
    {
      if ((w->getWindowHints() & WINDOW_INLINE_POPUP) != 0 && w->getParentWidget())
      {
        Widget* parent = w->getParentWidget();
        GuiWindow* guiWindow = parent->getGuiWindow();
        if (guiWindow != NULL) guiWindow->showPopUp(w);
      }

      // Only needed if widget was hidden before this event.
      FOG_WIDGET_TREE_ITERATOR(i1, w, true,
        // before traverse
        {
        // Show only child that's hidden by parent.
        if (child->getVisibility() != WIDGET_HIDDEN_BY_PARENT) FOG_WIDGET_TREE_ITERATOR_NEXT(i1);

        child->_visibility = WIDGET_VISIBLE;
        w->sendEvent(&e);
        },
        // after traverse
        {}
      );
    }

    w->update(WIDGET_UPDATE_ALL);
  }
  // Dispatch 'Hidden'.
  else
  {
    w->_visibility = visible;
    uint32_t code = EVENT_HIDE;
    if (visible == WIDGET_VISIBLE_MINIMIZED)
    {
      code = EVENT_SHOW_MINIMIZE;
    }

    VisibilityEvent e(code);
    w->sendEvent(&e);

    if (visible == WIDGET_HIDDEN && visibility >= WIDGET_VISIBLE)
    {
      e._code = EVENT_HIDE_BY_PARENT;

      FOG_WIDGET_TREE_ITERATOR(i2, w, true,
        // before traverse
        {
          // mark 'HiddenByParent' all childs that's visible
          if (child->getVisibility() != WIDGET_VISIBLE)
          {
            FOG_WIDGET_TREE_ITERATOR_NEXT(i2);
          }
          else
          {
            child->_visibility = WIDGET_HIDDEN_BY_PARENT;
            child->sendEvent(&e);
          }
        },
        // after traverse
        {
        }
      );

      Widget* p = w->getParentWidget();
      if (p) p->update(WIDGET_UPDATE_ALL);
    }
  }

  if (visible == WIDGET_VISIBLE_RESTORE)
  {
    visible = WIDGET_VISIBLE;
  }

  w->_visibility = visible;
}

void GuiEngine::dispatchConfigure(Widget* w, const IntRect& rect, bool changedOrientation)
{
  if (w->hasGuiWindow() && ((GuiWindow*)w->getGuiWindow())->hasPopUp())
  {
    ((GuiWindow*)w->getGuiWindow())->closePopUps();
  }

  uint32_t changed = 0;

  if (w->getPosition() != rect.getPosition())
    changed |= ConfigureEvent::CHANGED_POSITION;

  if (w->getSize() != rect.getSize())
    changed |= ConfigureEvent::CHANGED_SIZE;

  if (changedOrientation)
    changed |= ConfigureEvent::CHANGED_ORIENTATION;

  if (!changed) return;

  ConfigureEvent e;
  e._geometry = rect;
  e._changed = changed;

  w->_geometry = rect;
  w->_clientGeometry.set(0, 0, rect.w, rect.h);
  w->sendEvent(&e);

  Layout* layout = w->getLayout();

  // TODO: intelligent enum order so we can omit one check here!
  if (layout && layout->_activated &&  changed & ConfigureEvent::CHANGED_SIZE || changed & ConfigureEvent::CHANGED_ORIENTATION)
  {
    FOG_ASSERT(layout->_toplevel);
    layout->markAsDirty();
  }

  Widget* p = w->getParentWidget();
  if (p)
  {
    // Maximize and fullscreen should not be resized -> only visibile state.
    if (w->getVisibility() == WIDGET_VISIBLE) p->update(WIDGET_UPDATE_ALL);
  }
  else if (changed & ConfigureEvent::CHANGED_SIZE)
  {
    w->update(WIDGET_UPDATE_ALL);
  }
}

void GuiEngine::widgetDestroyed(Widget* w)
{
  if (_mouseStatus.widget == w)
  {
    _mouseStatus.widget = NULL;
    _mouseStatus.position.set(INT_MIN, INT_MIN);
    _mouseStatus.hover = 0;
    _mouseStatus.valid = false;
  }

  if (_caretStatus.widget == w)
  {
    Memory::zero(&_caretStatus, sizeof(CaretStatus));
  }
}

// ============================================================================
// [Fog::GuiEngine - Update]
// ============================================================================

// Working structure for parent and child, also it's record in stack for 
// traversing in widget-tree.
struct UpdateRec
{
  IntBox bounds;
  IntBox paintBounds;
  uint32_t uflags;
  uint32_t implicitFlags;
  uint32_t visible;
  uint32_t painted;
};

struct UpdateTask : public CancelableTask
{
  UpdateTask() : _valid(true) {}

  virtual void run()
  {
    if (!_valid) return;

    // Try to process all pending messages from event queue.
    // EventLoop::getCurrent()->runAllPending();
    // Do update.
    GUI_ENGINE()->doUpdate();
  }

  virtual void cancel()
  {
    _valid = false;
  }

private:
  bool _valid;
};

void GuiEngine::update()
{
  if (!_updateStatus.scheduled)
  {
    _updateStatus.scheduled = true;
    _updateStatus.task = new(std::nothrow) UpdateTask();
    Application::getInstance()->getEventLoop()->postTask(_updateStatus.task);
  }
}

void GuiEngine::doUpdate()
{
  _updateStatus.scheduled = false;
  _updateStatus.updating = true;
  _updateStatus.task = NULL;

  // Iterate over all dirty GuiWindow's and clear the list.
  {
    List<GuiWindow*> dirty(_dirtyList);
    _dirtyList.free();

    List<GuiWindow*>::ConstIterator it(dirty);
    for (it.toStart(); it.isValid(); it.toNext())
    {
      GuiWindow* uiWindow = it.value();
      if (uiWindow) doUpdateWindow(uiWindow);
    }
  }

  _updateStatus.updating = false;
}

void GuiEngine::doUpdateWindow(GuiWindow* window)
{
  // Clear dirty flag
  window->_isDirty = false;

  // GuiWindow widget.
  Widget* top = window->_widget;

  // Hidden windows won't be updated.
  if (!window->_visible) 
  {
    top->_uflags = 0;
    return;
  }
  
  while (window->_activatelist)
  {
    FOG_ASSERT(window->_activatelist->_activated == 0);
    window->_activatelist->activate();
    FOG_ASSERT(window->_activatelist->_activated == 1);
    window->_activatelist = window->_activatelist->_nextactivate;
  }

  // =======================================================
  // Local variables and initialization.
  // =======================================================

  // 10 seconds is extra buffer timeout
  TimeTicks now = TimeTicks::now() - TimeDelta::fromSeconds(10);

  // Painting.
  Painter painter;
  PaintEvent e;
  e._painter = &painter;

  // Temporary regions. We are using more regions, because region operations
  // are most efficient by Region::op(dest, src1, src2) way. So it's safe to
  // use different destination regions. Memory will be copied, but no memory
  // will be allocated that is the goal.
  TemporaryRegion<32> rtmp1;
  TemporaryRegion<32> rtmp2;
  TemporaryRegion<32> rtmp3;
  TemporaryRegion<64> blitRegion;

  // Some temporary data.
  IntSize topSize(top->getSize());
  IntBox  topBox(0, 0, (int)topSize.getWidth(), (int)topSize.getHeight());

  uint32_t uflags = top->_uflags;
  uint32_t implicitFlags = 0;
  bool blitFull = false;
  bool paintAll = false;

  // Paint helper variables.
  uint topBytesPerPixel;

  if (window->_backingStore->getWidth()  != topSize.getWidth() ||
    window->_backingStore->getHeight() != topSize.getHeight())
  {
    window->_backingStore->resize(topSize.getWidth(), topSize.getHeight(), true);

    // We can omit updating here, because if window size was changed,
    // all uflags have to be already set.

    // It should already be set, but nobody knows...
    uflags |= WIDGET_UPDATE_ALL;
  }
  else if (window->_backingStore->expired(now))
  {
    window->_backingStore->resize(topSize.getWidth(), topSize.getHeight(), false);
    uflags |= WIDGET_UPDATE_ALL;
  }

  topBytesPerPixel = window->_backingStore->getDepth() >> 3;

  // =======================================================
  // Update top level widget.
  // =======================================================

  // if there is nothing to do, continue. It's checked here,
  // because next steps needs to create drawing context, etc...
  if ((uflags & (
    WIDGET_UPDATE_SOMETHING |
    WIDGET_UPDATE_CHILD     |
    WIDGET_UPDATE_ALL       |
    WIDGET_UPDATE_GEOMETRY  |
    WIDGET_REPAINT_AREA     |
    WIDGET_REPAINT_CARET   )) == 0)
  {
    return;
  }

  // We will call Painter::begin() here, because it will be shared
  // between all repaints. Also we tweak width and height, because if we
  // set abnormal large width or height (in cases that backing store is cached)
  // we can tell painter to use multithreading in small areas (that we don't
  // want).
  {
    ImageBuffer buffer;
    buffer.data = window->_backingStore->getPixels();
    buffer.width = Math::min<int>(window->_backingStore->getWidth(), topSize.getWidth());
    buffer.height = Math::min<int>(window->_backingStore->getHeight(), topSize.getHeight());
    buffer.format = window->_backingStore->getFormat();
    buffer.stride = window->_backingStore->getStride();

    painter.begin(buffer, NO_FLAGS /*| PAINTER_INIT_MT */);
  }

  if ((uflags & WIDGET_UPDATE_ALL) != 0)
  {
    implicitFlags |=
      WIDGET_UPDATE_SOMETHING |
      WIDGET_UPDATE_CHILD     |
      WIDGET_UPDATE_GEOMETRY  |
      WIDGET_REPAINT_AREA     ;
  }

  uflags |= implicitFlags;

  if ((uflags & WIDGET_REPAINT_AREA) != 0)
  {
    e._code = EVENT_PAINT;
    e._receiver = top;

    painter.setMetaVars(
      TemporaryRegion<1>(IntRect(0, 0, top->getWidth(), top->getHeight())),
      IntPoint(0, 0));
    top->sendEvent(&e);

    uflags |=
      WIDGET_UPDATE_CHILD  ;
    implicitFlags |=
      WIDGET_UPDATE_CHILD  |
      WIDGET_REPAINT_AREA  ;

    blitFull = true;
    paintAll = true;
  }

  // =======================================================
  // Update children.
  // =======================================================

  if (top->hasChildren() && (uflags & WIDGET_UPDATE_CHILD) != 0)
  {
    LocalStack<1024> stack;
    err_t stackerr;

    // Manual object iterator.
    Object* const* ocur;
    Object* const* oend;

    // Parent and child widgets.
    Widget* parent;
    Widget* child;

    UpdateRec parentRec;
    UpdateRec childRec;

    parent = top;
    parentRec.bounds.set(0, 0, parent->getWidth(), parent->getHeight());
    parentRec.paintBounds.set(0, 0, parent->getWidth(), parent->getHeight());
    parentRec.uflags = uflags;
    parentRec.implicitFlags = implicitFlags;
    parentRec.visible = true;
    parentRec.painted = blitFull | paintAll;

__pushed:
    ocur = parent->_children.getData();
    oend = ocur + parent->_children.getLength();

    child = (Widget*)*ocur;
    for (;;)
    {
      // TODO: Hackery
      if (!reinterpret_cast<Object*>(child)->isWidget())
        goto __next;

      if (child->getVisibility() < WIDGET_VISIBLE)
        goto __next;

      childRec.uflags = child->_uflags;
      childRec.implicitFlags = parentRec.implicitFlags;

      if ((childRec.uflags & WIDGET_UPDATE_ALL) != 0)
      {
        childRec.implicitFlags |=
          WIDGET_UPDATE_SOMETHING |
          WIDGET_UPDATE_CHILD     |
          WIDGET_UPDATE_GEOMETRY  |
          WIDGET_REPAINT_AREA     ;
      }
      childRec.uflags |= childRec.implicitFlags;

      if ((childRec.uflags & (
        WIDGET_UPDATE_SOMETHING |
        WIDGET_UPDATE_CHILD     |
        WIDGET_UPDATE_GEOMETRY  |
        WIDGET_REPAINT_AREA     |
        WIDGET_REPAINT_CARET    |
        WIDGET_UPDATE_ALL)) == 0)
      {
        goto __next;
      }

      childRec.bounds.x1 = parentRec.bounds.getX1() + child->getX();
      childRec.bounds.y1 = parentRec.bounds.getY1() + child->getY();
      childRec.bounds.x2 = childRec.bounds.getX1() + child->getWidth();
      childRec.bounds.y2 = childRec.bounds.getY1() + child->getHeight();

      childRec.bounds += parent->getOrigin();

      childRec.visible = IntBox::intersect(childRec.paintBounds, parentRec.paintBounds, childRec.bounds);
      childRec.painted = false;

      if (childRec.visible)
      {
        // paint client area / caret
        if ((childRec.uflags & (WIDGET_REPAINT_AREA  | WIDGET_REPAINT_CARET)) != 0)
        {
          e._code = EVENT_PAINT;
          e._receiver = child;

#if 0
          if (child->children().count() > 0 && child->children().getLength() <= 16)
          {
            rtmp2.set(childRec.paintBounds);
            TemporaryRegion<128> rtmp4;
            List<Widget*>::ConstIterator ci(child->children());
            int ox = childRec.bounds.x1() + child->origin().x();
            int oy = childRec.bounds.y1() + child->origin().y();

            for (ci.toStart(); ci.isValid(); ci.toNext())
            {
              Widget* cw = core_object_cast<Widget*>(ci.value());
              if (cw && cw->visibility() == Widget::Visible)
              {
                if ((cw->_uflags & WIDGET_REPAINT_PARENT_REQUIRED))
                {
                  // this is simple optimization. If widget will call
                  // Widget::paintParent(), we will simply do it
                  // earlier.
                  cw->_uflags |= Widget::paintParent;
                }
                else
                {
                  rtmp4.combine(IntBox(
                    cw->rect().x1() + ox,
                    cw->rect().y1() + oy,
                    cw->rect().x2() + ox,
                    cw->rect().y2() + oy), REGION_OP_UNION);
                }
              }
            }
            Region::combine(rtmp1, rtmp2, rtmp4, REGION_OP_SUBTRACT);
          }
          else
          {
#endif
            rtmp1.set(childRec.paintBounds);
            childRec.painted = true;
#if 0
          }
#endif
          painter.setMetaVars(
            rtmp1,
            IntPoint(childRec.bounds.getX1(), childRec.bounds.getY1()));

          // FIXME: Repaint caret repaints the whole widget.
          if ((childRec.uflags & (WIDGET_REPAINT_AREA | WIDGET_REPAINT_CARET)) != 0)
          {
            child->sendEvent(&e);
            //blitFull = true;
            if (!parentRec.painted) blitRegion.combine(rtmp1, REGION_OP_UNION);
          }

          /*
          if (Application::caretStatus().widget == child &&
          (childRec.uflags & (Widget::UFlagRepaintWidget | Widget::UFlagRepaintCaret)) != 0)
          {
          theme->paintCaret(&painter, Application::caretStatus());
          //blitFull = true;
          if (!parentRec.painted) blitRegion.unite(rtmp1);
          }
          */

          parentRec.implicitFlags |= WIDGET_UPDATE_SOMETHING |
            WIDGET_UPDATE_CHILD     |
            WIDGET_REPAINT_AREA     ;
          childRec.uflags         |= WIDGET_UPDATE_CHILD     ;
          childRec.implicitFlags  |= WIDGET_UPDATE_SOMETHING |
            WIDGET_UPDATE_CHILD     |
            WIDGET_REPAINT_AREA     ;
        }
      }

      if ((childRec.uflags & WIDGET_UPDATE_CHILD) != 0 && child->_children.getLength())
      {
        stackerr = ERR_OK;
        stackerr |= stack.push(parent);
        stackerr |= stack.push(ocur);
        stackerr |= stack.push(oend);
        stackerr |= stack.push(parentRec);
        if (stackerr != ERR_OK) goto end;

        parent = child;
        parentRec = childRec;

        goto __pushed;
__pop:
        childRec = parentRec;
        child = parent;
        stack.pop(parentRec);
        stack.pop(oend);
        stack.pop(ocur);
        stack.pop(parent);
      }

      // Clear update flags.
      child->_uflags &= ~(
        WIDGET_UPDATE_SOMETHING |
        WIDGET_UPDATE_CHILD     |
        WIDGET_UPDATE_ALL       |
        WIDGET_UPDATE_GEOMETRY  |
        WIDGET_REPAINT_AREA     |
        WIDGET_REPAINT_CARET    );

__next:
      // Go to next child or to parent.
      if (++ocur == oend)
      {
        if (!stack.isEmpty())
          goto __pop;
        else
          break;
      }

      child = (Widget*)*ocur;
    }
  }

end:
  painter.end();

  // =======================================================
  // blit root widget content to screen
  // =======================================================

  const IntBox* rptr;
  sysuint_t rlen = 0;

  if (blitFull || window->_needBlit)
  {
    rptr = &topBox;
    rlen = 1;
  }
  else
  {
    rptr = blitRegion.getData();
    rlen = blitRegion.getLength();
  }

  if (rlen)
  {
    window->_backingStore->updateRects(rptr, rlen);
    doBlitWindow(window, rptr, rlen);
  }

  // Clear update flags.
  top->_uflags &= ~(
    WIDGET_UPDATE_SOMETHING |
    WIDGET_UPDATE_CHILD     |
    WIDGET_UPDATE_ALL       |
    WIDGET_UPDATE_GEOMETRY  |
    WIDGET_REPAINT_AREA     |
    WIDGET_REPAINT_CARET    );

  // Clear blit flag.
  window->_needBlit = false;
}

// ============================================================================
// [Fog::GuiEngine - Event Handlers]
// ============================================================================

void GuiEngine::_onButtonRepeatTimeOut(TimerEvent* e)
{
  if (_systemMouseStatus.uiWindow == NULL) 
  {
    e->getTimer()->stop();
  }
  else
  {
    uint32_t id;
    for (id = 0; id != 3; id++)
    {
      if (e->getTimer() == &_buttonRepeat[id])
      {
        _buttonRepeat[id].setInterval(_buttonRepeatInterval[id]);
        _systemMouseStatus.uiWindow->onMousePress(1 << id, true);
        break;
      }
    }
  }
}

// ============================================================================
// [Fog::GuiWindow]
// ============================================================================

GuiWindow::GuiWindow(Widget* widget) :
  _widget(widget),
  _handle(NULL),
  _backingStore(NULL),
  _enabled(true),
  _visible(true),
  _hasFocus(false),
  _needBlit(true),
  _isDirty(false),
  _windowRect(0, 0, 1, 1),
  _clientRect(0, 0, 1, 1),
  _owner(NULL),
  _modalpolicy(MODAL_NONE),
  _modal(0),
  _lastmodal(NULL),
  _activatelist(NULL),
  _sizeGranularity(1, 1)
{
}

GuiWindow::~GuiWindow()
{
  GuiEngine* guiEngine = GUI_ENGINE();

  // Remove GuiWindow from system mouse status.
  if (guiEngine->_systemMouseStatus.uiWindow == this)
  {
    guiEngine->clearSystemMouseStatus();
    guiEngine->clearButtonRepeat();
  }

  // Remove GuiWindow's from dirty list.
  sysuint_t i = guiEngine->_dirtyList.indexOf(this);
  if (i != INVALID_INDEX) guiEngine->_dirtyList.set(i, NULL);
}

GuiWindow* GuiWindow::getModalWindow()
{
  if (getOwner())
  {
    // Normally this would be this. But maybe we need the 
    // owner-variable for other tasks, too!
    return getOwner()->_modal;
  }

  return NULL;
}

// ============================================================================
// [Fog::GuiWindow - Windowing System]
// ============================================================================

void GuiWindow::onEnabled(bool enabled)
{
  _enabled = enabled;
  //Question: dispatch disable-events during modality?
  //current answer: No, because it is an indirect disable
  //and should not be changed/react by application!
  if (_modal == 0)
  {
    GUI_ENGINE()->dispatchEnabled(_widget, enabled);
  }
}

void GuiWindow::onVisibility(uint32_t visible)
{
  _visible = visible >= WIDGET_VISIBLE;
  GUI_ENGINE()->dispatchVisibility(_widget, visible);
}

void GuiWindow::onConfigure(const IntRect& windowRect, const IntRect& clientRect)
{
  _windowRect = windowRect;
  _clientRect = clientRect;

  GUI_ENGINE()->dispatchConfigure(_widget, IntRect(
    windowRect.getX(),
    windowRect.getY(),
    clientRect.getWidth(),
    clientRect.getHeight()), false);
}

void GuiWindow::onMouseHover(int x, int y)
{
  GuiEngine* guiEngine = GUI_ENGINE();

  if (guiEngine->_systemMouseStatus.uiWindow != this)
  {
    guiEngine->_systemMouseStatus.uiWindow = this;
    guiEngine->_systemMouseStatus.buttons = 0;
  }
  guiEngine->_systemMouseStatus.position.set(x, y);
  guiEngine->_systemMouseStatus.hover = true;

  onMouseMove(x, y);
}

void GuiWindow::onMouseMove(int x, int y)
{
  GuiEngine* guiEngine = GUI_ENGINE();
  Widget* w;

  if (guiEngine->_systemMouseStatus.uiWindow != this)
  {
    guiEngine->_systemMouseStatus.uiWindow = this;
    guiEngine->_systemMouseStatus.buttons = 0;
    guiEngine->_systemMouseStatus.hover = false;
  }
  guiEngine->_systemMouseStatus.position.set(x, y);

  IntPoint p(x, y);

  // ----------------------------------
  // Grabbing mode
  // ----------------------------------

  if (guiEngine->_mouseStatus.buttons)
  {
    w = guiEngine->_mouseStatus.widget;
    p -= _widget->getOrigin();

    if (!Widget::translateCoordinates(w, _widget, &p))
    {
      FOG_ASSERT_NOT_REACHED();
    }

    guiEngine->changeMouseStatus(w, p);
    return;
  }

  // ----------------------------------
  // Motion mode
  // ----------------------------------

  // In motion mode, mouse should be in widget bounds (so check for it).
  if (x < 0 || y < 0 || x >= _widget->getWidth() || y >= _widget->getHeight())
  {
    return;
  }

  w = _widget;

__repeat:
  {
    // Add origin.
    p -= w->_origin;

    // Iterate over children and try to find child widget where a mouse
    // position is. Iteration must be done through end, becuase we want
    // widget with highest Z-Order to match mouse position first.
    List<Object*>::ConstIterator it(w->_children);
    for (it.toEnd(); it.isValid(); it.toPrevious())
    {
      Widget* current = fog_object_cast<Widget*>(it.value());
      if (current && 
          current->getVisibility() >= WIDGET_VISIBLE &&
          current->_geometry.contains(p))
      {
        w = current;
        p -= w->getPosition();
        goto __repeat;
      }
    }

    guiEngine->changeMouseStatus(w, p);
  }
}

void GuiWindow::onMouseLeave(int x, int y)
{
  GuiEngine* guiEngine = GUI_ENGINE();

  if (guiEngine->_systemMouseStatus.uiWindow == this)
  {
    Widget* w = guiEngine->_mouseStatus.widget;

    guiEngine->_mouseStatus.widget = NULL;
    guiEngine->_mouseStatus.position.set(-1, -1);
    guiEngine->_mouseStatus.valid = 1;
    guiEngine->_mouseStatus.buttons = 0;
    guiEngine->_mouseStatus.hover = 0;

    guiEngine->clearSystemMouseStatus();
    guiEngine->clearButtonRepeat();

    if (w)
    {
      MouseEvent e(EVENT_MOUSE_OUT);
      e._position.set(-1, -1);

      w->sendEvent(&e);
    }
  }
}

void GuiWindow::onMousePress(uint32_t button, bool repeated)
{
  GuiEngine* guiEngine = GUI_ENGINE();
  if (guiEngine->_systemMouseStatus.uiWindow != this) return;

  Widget* w = guiEngine->_mouseStatus.widget;
  if (!w)
  {
    closePopUps();
    return;
  }

  if (hasPopUp())
  {
    if (!w->isPopUpWindow())
    {
      closePopUps();
    }
  }

  guiEngine->_systemMouseStatus.buttons |= button;
  guiEngine->_mouseStatus.buttons |= button;

  TimeTicks now = TimeTicks::now();

  if (!repeated) guiEngine->startButtonRepeat(
    button, true, guiEngine->_repeatingDelay, guiEngine->_repeatingInterval);

  MouseEvent e(EVENT_MOUSE_PRESS);
  e._button = button;
  e._modifiers = guiEngine->getKeyboardModifiers();
  e._position  = guiEngine->_mouseStatus.position;
  e._isOutside = ((e._position.x >= 0) &
                  (e._position.y >= 0) &
                  (e._position.x < w->getWidth()) &
                  (e._position.y < w->getHeight()));
  e._isRepeated = (repeated != 0);
  w->sendEvent(&e);

  uint32_t buttonId = fogButtonToId(button);
  if (repeated || buttonId == BUTTON_INVALID) return;

  if (!repeated && (now - guiEngine->_buttonTime[buttonId]) <= guiEngine->_doubleClickInterval)
  {
    guiEngine->_buttonTime[buttonId].clear();
    e._code = EVENT_DOUBLE_CLICK;
    w->sendEvent(&e);
  }
  else
  {
    guiEngine->_buttonTime[buttonId] = now;
  }
}

void GuiWindow::onMouseRelease(uint32_t button)
{
  GuiEngine* guiEngine = GUI_ENGINE();
  if (guiEngine->_systemMouseStatus.uiWindow != this) return;

  guiEngine->_systemMouseStatus.buttons &= ~button;
  guiEngine->_mouseStatus.buttons &= ~button;

  guiEngine->stopButtonRepeat(button);

  Widget* w = guiEngine->_mouseStatus.widget;
  if (!w) return;

  bool lastButtonRelease = (guiEngine->_mouseStatus.buttons == 0);

  MouseEvent e(EVENT_MOUSE_RELEASE);
  e._button = button;
  e._modifiers = guiEngine->getKeyboardModifiers();
  e._position = guiEngine->_mouseStatus.position;
  e._isOutside = !(
    e._position.x >= 0 &&
    e._position.y >= 0 &&
    e._position.x < w->getWidth() &&
    e._position.y < w->getHeight());
  w->sendEvent(&e);

  // Send click event only if user did it (if mouse release is outside the 
  // widget then used probably don't want to do the action).
  if (!e._isOutside)
  {
    e._code = EVENT_CLICK;
    w->sendEvent(&e);
  }

  if (lastButtonRelease)
  {
    onMouseMove(guiEngine->_systemMouseStatus.position.x,
                guiEngine->_systemMouseStatus.position.y);
  }
}

void GuiWindow::onMouseWheel(uint32_t wheel)
{
  GuiEngine* guiEngine = GUI_ENGINE();
  if (guiEngine->_systemMouseStatus.uiWindow != this) return;

  Widget* w = guiEngine->_mouseStatus.widget;
  if (!w) return;

  MouseEvent e(EVENT_WHEEL);
  e._button = wheel;
  e._modifiers = guiEngine->getKeyboardModifiers();
  e._position = guiEngine->_mouseStatus.position;
  w->sendEvent(&e);
}

void GuiWindow::onFocus(bool focus)
{
  GuiEngine* guiEngine = GUI_ENGINE();

  _hasFocus = focus;
  if (focus)
  {
    Widget* w = _widget->_findFocus();
    if (!w->_hasFocus)
    {
      FocusEvent e(EVENT_FOCUS_IN);
      _widget->_hasFocus = true;
      _widget->sendEvent(&e);
    }
  }
  else
  {
    guiEngine->_keyboardStatus.modifiers = 0;
    closePopUps();
  }
}

bool GuiWindow::onKeyPress(uint32_t key, uint32_t modifier, uint32_t systemCode, Char unicode)
{
  GuiEngine* guiEngine = GUI_ENGINE();

  KeyEvent e(EVENT_KEY_PRESS);
  e._key = key;
  e._modifiers = guiEngine->getKeyboardModifiers();
  e._systemCode = systemCode;
  e._unicode = unicode;

  if (isShiftMod(guiEngine->_keyboardStatus.modifiers)) e._key |= KEY_SHIFT;
  if (isCtrlMod (guiEngine->_keyboardStatus.modifiers)) e._key |= KEY_CTRL;
  if (isAltMod  (guiEngine->_keyboardStatus.modifiers)) e._key |= KEY_ALT;

  // Set this status after modifiers check.
  guiEngine->_keyboardStatus.modifiers |= modifier;

  if ((e.getUnicode().ch() >= 1 && e.getUnicode().ch() <= 31) ||
      e.getUnicode().ch() == 127 /* DEL key */)
  {
    e._unicode._ch = 0;
  }

  _widget->_findFocus()->sendEvent(&e);
  return e.isAccepted();
}

bool GuiWindow::onKeyRelease(uint32_t key, uint32_t modifier, uint32_t systemCode, Char unicode)
{
  GuiEngine* guiEngine = GUI_ENGINE();
  guiEngine->_keyboardStatus.modifiers &= ~modifier;

  KeyEvent e(EVENT_KEY_RELEASE);
  e._key = key;
  e._modifiers = guiEngine->getKeyboardModifiers();
  e._systemCode = systemCode;
  e._unicode._ch = 0;

  if (isShiftMod(guiEngine->_keyboardStatus.modifiers)) e._key |= KEY_SHIFT;
  if (isCtrlMod (guiEngine->_keyboardStatus.modifiers)) e._key |= KEY_CTRL;
  if (isAltMod  (guiEngine->_keyboardStatus.modifiers)) e._key |= KEY_ALT;

  _widget->_findFocus()->sendEvent(&e);
  return e.isAccepted();
}

void GuiWindow::clearFocus()
{
  Widget* w = _widget;

  // Need to clear focus.
  while (w->_focusLink)
  {
    Widget* t = w->_focusLink;
    w->_focusLink = NULL;
    w = t;
  }

  if (w->_hasFocus)
  {
    w->_hasFocus = false;
    FocusEvent e(EVENT_FOCUS_OUT);
    w->sendEvent(&e);
  }
}

void GuiWindow::setFocus(Widget* w, uint32_t reason)
{
  if (_widget->_focusLink) clearFocus();

  Widget* cur = w;
  while (cur != _widget)
  {
    Widget* parent = cur->getParentWidget();
    parent->_focusLink = cur;
    cur = parent;
  }

  if (!_hasFocus) takeFocus();

  FocusEvent e(EVENT_FOCUS_IN, reason);
  w->_hasFocus = true;
  w->sendEvent(&e);
}

// ============================================================================
// [Fog::GuiWindow - Dirty]
// ============================================================================

void GuiWindow::setDirty()
{
  // Prevent setting isDirty flag more times. If it has been already set we just
  // do nothing.
  if (_isDirty) return;
  _isDirty = true;

  GuiEngine* guiEngine = GUI_ENGINE();
  guiEngine->_dirtyList.append(this);
  guiEngine->update();
}

// ============================================================================
// [Fog::GuiWindow - PopUp]
// ============================================================================

void GuiWindow::showPopUp(Widget* w)
{
  _popup.append(w);
}

void GuiWindow::closePopUps()
{
  List<Widget*>::ConstIterator it(_popup);
  for (it.toStart(); it.isValid(); it.toNext())
  {
    Widget* widget = it.value();
    if (widget) widget->hide();
  } 

  _popup.free();
}

// ============================================================================
// [Fog::GuiWindow - Modal]
// ============================================================================

void GuiWindow::startModalWindow(GuiWindow* w)
{
  _modal = w;
  disable();
}

void GuiWindow::endModal(GuiWindow* w)
{
  FOG_ASSERT(w == _modal);
  w->setModal(MODAL_NONE);
  enable();
  w->releaseOwner();

  _modal = 0;
}

GuiWindow* GuiWindow::getModalBaseWindow()
{
  GuiWindow* parent = this;
  while(parent->getOwner())
  {
    parent = parent->getOwner();
  }

  return parent;
}

void GuiWindow::showAllModalWindows(uint32_t visible)
{
  GuiWindow* parent = getOwner();
  while(parent->getOwner())
  {
    parent->getWidget()->show(visible);
    parent = parent->getOwner();
  }

  // If show, to focus will be at this window.
  getWidget()->show(visible);
}

// ============================================================================
// [Fog::GuiBackBuffer]
// ============================================================================

GuiBackBuffer::GuiBackBuffer()
{
  _clear();
}

GuiBackBuffer::~GuiBackBuffer()
{
}

void GuiBackBuffer::_clear()
{
  _type = 0;
  _depth = 0;

  _buffer.data = NULL;
  _buffer.width = 0;
  _buffer.height = 0;
  _buffer.format = IMAGE_FORMAT_NULL;
  _buffer.stride = 0;

  _cachedWidth = 0;
  _cachedHeight = 0;

  _primaryPixels = NULL;
  _primaryStride = 0;

  _secondaryPixels = NULL;
  _secondaryStride = 0;

  _convertFunc = NULL;
  _convertDepth = 0;

  _createdTime = TimeTicks();
  _expireTime = TimeTicks();
}

bool GuiBackBuffer::expired(TimeTicks now) const
{
  return (_buffer.width != _cachedWidth || _buffer.height != _cachedHeight) && (now >= _expireTime);
}

} // Fog namespace
