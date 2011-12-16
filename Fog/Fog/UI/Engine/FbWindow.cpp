// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/UI/Engine/FbEngine.h>
#include <Fog/UI/Engine/FbWindow.h>

namespace Fog {

// ============================================================================
// [Fog::FbWindowData]
// ============================================================================

FbWindowData::FbWindowData(FbEngine* engine, FbWindow* window) :
  engine(engine),
  window(window),
  handle(NULL),
  state(STATE_DISABLED),
  visibility(VISIBILITY_HIDDEN),
  isDirty(false),
  hasFocus(false),
  isWindowOpaque(true),
  isCompositingEnabled(false),
  isBufferCacheEnabled(true),
  windowDepth(0),
  windowVisibility(0),
  orientation(ORIENTATION_HORIZONTAL),
  shouldUpdate(false),
  shouldPaint(false),
  shouldBlit(false),
  windowGeometry(0, 0, 0, 0),
  clientGeometry(0, 0, 0, 0),
  sizeGranularity(1, 1),
  bufferCacheSize(0, 0),
  bufferCacheCreated(),
  bufferCacheExpire(),
  windowScreen(0),
  windowOpacity(1.0f)
{
  bufferData.reset();
}

FbWindowData::~FbWindowData()
{
}

// ============================================================================
// [Fog::FbWindow - Window Stack]
// ============================================================================

void FbWindow::moveToTop(FbWindow* w)
{
  void* handle = NULL;

  if (w != NULL)
    handle = w->getHandle();
  
  return _d->moveToTop(handle);
}

void FbWindow::moveToBottom(FbWindow* w)
{
  void* handle = NULL;

  if (w != NULL)
    handle = w->getHandle();
  
  return _d->moveToBottom(handle);
}

// ============================================================================
// [Fog::FbWindow - Window Coordinates]
// ============================================================================

err_t FbWindow::worldToClient(PointI& pt) const
{
  return _d->worldToClient(pt);
}

err_t FbWindow::clientToWorld(PointI& pt) const
{
  return _d->clientToWorld(pt);
}

// ============================================================================
// [Fog::FbWindow - Events]
// ============================================================================
 
void FbWindow::onFbEvent(FbEvent* ev)
{
  // Nothing to do here.
}

} // Fog namespace





































#if 0
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
  size_t i = guiEngine->_dirtyList.indexOf(this);
  if (i != INVALID_INDEX) guiEngine->_dirtyList.setAt(i, NULL);
}

// ============================================================================
// [Fog::GuiWindow - Windowing System]
// ============================================================================

void GuiWindow::onEnabled(bool enabled)
{
  _enabled = enabled;

  // TODO:
  // Question: dispatch disable-events during modality?
  // current answer: No, because it is an indirect disable
  // and should not be changed/react by application!
  if (_modal == NULL)
  {
    GUI_ENGINE()->dispatchEnabled(_widget, enabled);
  }
}

void GuiWindow::onVisibility(uint32_t visible)
{
  _visible = (visible >= WIDGET_VISIBLE);
  GUI_ENGINE()->dispatchVisibility(_widget, visible);
}

void GuiWindow::onGeometry(const RectI& windowRect, const RectI& clientRect)
{
  _windowRect = windowRect;
  _clientRect = clientRect;

  GUI_ENGINE()->dispatchConfigure(_widget, RectI(
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

  PointI p(x, y);

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

_Repeat:
  {
    // Add origin.
    p -= w->_clientOrigin;

    // Iterate over children and try to find child widget where a mouse
    // position is. Iteration must be done through end, becuase we want
    // widget with highest Z-Order to match mouse position first.
    ListReverseIterator<Object*> it(w->_objectExtra->_children);

    while (it.isValid())
    {
      Widget* current = fog_object_cast<Widget*>(it.getItem());

      if (current &&
          current->getVisibility() >= WIDGET_VISIBLE &&
          current->_widgetGeometry.hitTest(p))
      {
        w = current;
        p -= w->getPosition();
        goto _Repeat;
      }

      it.next();
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
    guiEngine->_buttonTime[buttonId].reset();
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

bool GuiWindow::onKeyPress(uint32_t key, uint32_t modifier, uint32_t systemCode, CharW unicode)
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

  if (e.getUnicode().isAt(1, 31) || e.getUnicode().getValue() == 127 /* DEL key */)
  {
    e._unicode = 0;
  }

  _widget->_findFocus()->sendEvent(&e);
  return e.isAccepted();
}

bool GuiWindow::onKeyRelease(uint32_t key, uint32_t modifier, uint32_t systemCode, CharW unicode)
{
  GuiEngine* guiEngine = GUI_ENGINE();
  guiEngine->_keyboardStatus.modifiers &= ~modifier;

  KeyEvent e(EVENT_KEY_RELEASE);
  e._key = key;
  e._modifiers = guiEngine->getKeyboardModifiers();
  e._systemCode = systemCode;
  e._unicode = 0;

  if (isShiftMod(guiEngine->_keyboardStatus.modifiers)) e._key |= KEY_SHIFT;
  if (isCtrlMod (guiEngine->_keyboardStatus.modifiers)) e._key |= KEY_CTRL;
  if (isAltMod  (guiEngine->_keyboardStatus.modifiers)) e._key |= KEY_ALT;

  _widget->_findFocus()->sendEvent(&e);
  return e.isAccepted();
}

void GuiWindow::resetFocus()
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
  if (_widget->_focusLink) resetFocus();

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
  if (_isDirty)
    return;
  _isDirty = true;

  GuiEngine* guiEngine = GUI_ENGINE();
  guiEngine->_dirtyList.append(this);
  guiEngine->update();
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
  _buffer.size.reset();
  _buffer.format = IMAGE_FORMAT_NULL;
  _buffer.stride = 0;

  _cachedSize.reset();

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
  return _buffer.size != _cachedSize && now >= _expireTime;
}
#endif
