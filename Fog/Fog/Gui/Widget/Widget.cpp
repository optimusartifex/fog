// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/System/Application.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Tools/Region.h>
#include <Fog/Gui/Global/Constants.h>
#include <Fog/Gui/Engine/GuiEngine.h>
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Gui/Layout/LayoutItem.h>
#include <Fog/Gui/Widget/Widget.h>
#include <Fog/Gui/Widget/Widget_p.h>

FOG_IMPLEMENT_OBJECT(Fog::Widget)

namespace Fog {

// ============================================================================
// [Fog::Widget - Construction / Destruction]
// ============================================================================

Widget::Widget(uint32_t createFlags) :
  _parentWidget(NULL),
  _guiWindow(NULL),
  _widgetGeometry(0, 0, 0, 0),
  _clientGeometry(0, 0, 0, 0),
  _clientOrigin(0, 0),
  _fullscreendata(NULL),
  _extra(0),
  _layout(NULL),
  _layoutPolicy(LAYOUT_POLICY_WIDTH_PREFERRED|LAYOUT_POLICY_HEIGHT_PREFERRED),
  _tabOrder(0),
  _transparency(1.0),
  _lastFocus(NULL),
  _focusLink(NULL),
  _uflags(0),
  _windowFlags(createFlags),
  _minset(0),
  _maxset(0),
  _orientation(ORIENTATION_HORIZONTAL),
  _hasFocus(false),
  _state(WIDGET_ENABLED),
  _visibility(WIDGET_HIDDEN),
  _focusPolicy(FOCUS_NONE),
  _hasNcArea(false),
  _reserved(0),
  _widgetflags(0)
{
  _objectFlags |= OBJECT_FLAG_IS_WIDGET;

  // TODO ?
  _focusLink = NULL;
  _contentmargin.clear();

  if ((createFlags & WINDOW_TYPE_MASK) != 0)
  {
    createWindow(createFlags);

    //not needed under windows, maybe on linux?
    //setTransparency(1.0);
  }
}

Widget::~Widget()
{
  deleteLayout();
  if (_guiWindow) destroyWindow();
}

// ============================================================================
// [Fog::Widget - Object Hierarchy]
// ============================================================================

err_t Widget::_addChild(sysuint_t index, Object* child)
{
  Widget* w = fog_object_cast<Widget*>(child);

  if (w)
  {
    // Default behavior is to set widget-parent to 'this', this means that
    // the widget-parent is same as object-parent.
    w->_parentWidget = this;

    // Handle an inline-popup widgets.
    //
    // The inline-popup should be always at the end of the children list and
    // can be only added to a widget that has GuiWindow (the top-level one).
    bool isInlinePopup = ((w->getWindowFlags() & WINDOW_INLINE_POPUP) != 0);

    if (isInlinePopup && (getWindowFlags() & WINDOW_TYPE_MASK) == 0)
    {
      // WIDGET TODO: We need an error code for this!
      return ERR_RT_INVALID_ARGUMENT;
    }

    // WIDGET TODO: Stefan, this is not optimal, we should create some stuff in
    // GuiWindow for this, so inline popup can be handled by it. Hmm, what is
    // purpose of this widget, shouldn't be sufficient to just create a regular
    // widget and add it to the top-level hoerarchy?
    if (!isInlinePopup && _children.getLength() > 0)
    {
      List<Object*>::ConstIterator it(_children);
      for (it.toEnd(); it.isValid(); it.toPrevious())
      {
        Widget* widget = fog_object_cast<Widget*>(it.value());
        if (widget && ((widget->getWindowFlags() & WINDOW_INLINE_POPUP) != 0))
        {
          // Now element can be added to that index.
          return _addChild(it.index(), w);
        }
      }

      // No inline popup found...
    }
  }

  return Object::_addChild(index, child);
}

err_t Widget::_removeChild(sysuint_t index, Object* child)
{
  Widget* w = fog_object_cast<Widget*>(child);

  if (w)
  {
    // Clear widget parent.
    w->_parentWidget = NULL;
  }

  return Object::_removeChild(index, child);
}

// ============================================================================
// [Fog::Widget - Gui-Window]
// ============================================================================

GuiWindow* Widget::getClosestGuiWindow() const
{
  Widget* widget = const_cast<Widget*>(this);
  GuiWindow* window = NULL;

  do {
    window = widget->getGuiWindow();
    if (window != NULL) break;

    widget = widget->getParentWidget();
  } while (widget);

  return window;
}

err_t Widget::createWindow(uint32_t createFlags)
{
  if (hasGuiWindow())
    return ERR_OK;

  GuiEngine* ge = Application::getInstance()->getGuiEngine();
  if (ge == NULL) return ERR_GUI_NO_ENGINE;

  _guiWindow = ge->createGuiWindow(this);

  err_t err = _guiWindow->create(createFlags);
  if (FOG_IS_ERROR(err)) destroyWindow();

  return err;
}

err_t Widget::destroyWindow()
{
  if (!hasGuiWindow())
    return ERR_OK;

  if (_guiWindow->isModal())
  {
    _guiWindow->getOwner()->endModal(_guiWindow);
  }

  fog_delete(_guiWindow);
  _guiWindow = NULL;
  return true;
}

String Widget::getWindowTitle() const
{
  String title;
  if (_guiWindow) _guiWindow->getTitle(title);
  return title;
}

err_t Widget::setWindowTitle(const String& title)
{
  if (!_guiWindow) return ERR_RT_INVALID_HANDLE;
  return _guiWindow->setTitle(title);
}

Image Widget::getWindowIcon() const
{
  Image i;
  if (_guiWindow) _guiWindow->getIcon(i);
  return i;
}

err_t Widget::setWindowIcon(const Image& icon)
{
  if (!_guiWindow) return ERR_RT_INVALID_HANDLE;
  return _guiWindow->setIcon(icon);
}

PointI Widget::getWindowGranularity() const
{
  PointI sz(0, 0);
  if (_guiWindow) _guiWindow->getSizeGranularity(sz);
  return sz;
}

err_t Widget::setWindowGranularity(const PointI& pt)
{
  if (!_guiWindow) return ERR_RT_INVALID_HANDLE;
  return _guiWindow->setSizeGranularity(pt);
}

// ============================================================================
// [Fog::Widget - Core Geometry]
// ============================================================================

void Widget::setPosition(const PointI& pos)
{
  if (_widgetGeometry.getPosition() == pos) return;

  if (hasGuiWindow())
  {
    _guiWindow->setPosition(pos);
  }
  else
  {
    GuiEngine* engine = Application::getInstance()->getGuiEngine();
    if (engine == NULL) return;

    engine->dispatchConfigure(this, RectI(pos.x, pos.y, _widgetGeometry.w, _widgetGeometry.h), false);
  }
}

void Widget::setSize(const SizeI& sz)
{
  if (_widgetGeometry.getSize() == sz) return;

  if (hasGuiWindow())
  {
    _guiWindow->setSize(sz);
  }
  else
  {
    GuiEngine* engine = Application::getInstance()->getGuiEngine();
    if (engine == NULL) return;

    engine->dispatchConfigure(this, RectI(_widgetGeometry.x, _widgetGeometry.y, sz.w, sz.h), false);
  }
}

void Widget::setGeometry(const RectI& geometry)
{
  if (_widgetGeometry == geometry) return;

  if (hasGuiWindow())
  {
    _guiWindow->setGeometry(geometry);
  }
  else
  {
    GuiEngine* engine = Application::getInstance()->getGuiEngine();
    if (engine == NULL) return;

    engine->dispatchConfigure(this, geometry, false);
  }
}

// ============================================================================
// [Fog::Widget - Client Geometry]
// ============================================================================

void Widget::calcWidgetSize(SizeI& size) const
{
  // Default action is to do nothing.
}

void Widget::calcClientGeometry(RectI& geometry) const
{
  // Default action is to do nothing.
}

void Widget::updateClientGeometry()
{
  if (hasGuiWindow())
  {
    _guiWindow->setGeometry(_widgetGeometry);
  }
  else
  {
    GuiEngine* engine = Application::getInstance()->getGuiEngine();
    if (engine == NULL) return;

    engine->dispatchConfigure(this, _widgetGeometry, false);
  }
}

// ============================================================================
// [Fog::Widget - Client Origin]
// ============================================================================

void Widget::setOrigin(const PointI& pt)
{
  if (_clientOrigin == pt) return;

  OriginEvent e;
  e._origin = pt;
  e._difference = pt - _clientOrigin;

  _clientOrigin = pt;
  sendEvent(&e);

  // TODO: Not optimal, widget can tell to scroll and update only changed part.
  update(WIDGET_UPDATE_ALL);
}

// ============================================================================
// [Fog::Widget - Translate Coordinates]
// ============================================================================

bool Widget::worldToClient(PointI* coords) const
{
  Widget* w = const_cast<Widget*>(this);

  do {
    // TODO, disable origin here?
    // coords->translate(w->getOrigin());

    if (w->hasGuiWindow())
      return w->getGuiWindow()->worldToClient(coords);

    coords->translate(-(w->_widgetGeometry.x), -(w->_widgetGeometry.y));
    w = w->getParentWidget();
  } while (w);

  return false;
}

bool Widget::clientToWorld(PointI* coords) const
{
  Widget* w = const_cast<Widget*>(this);

  do {
    coords->translate(w->getOrigin());

    if (w->hasGuiWindow())
      return w->getGuiWindow()->clientToWorld(coords);

    coords->translate(w->_widgetGeometry.x, w->_widgetGeometry.y);
    w = w->getParentWidget();
  } while (w);

  return false;
}

bool Widget::translateCoordinates(Widget* to, Widget* from, PointI* coords)
{
  Widget* w;
  int x, y;

  if (to == from) return true;

  // Traverse 'from' -> 'to'.
  w = from;
  x = coords->x;
  y = coords->y;

  while (w->hasParent() && w->getParent()->isWidget())
  {
    x += w->_clientOrigin.x;
    y += w->_clientOrigin.y;
    x += w->_widgetGeometry.x;
    y += w->_widgetGeometry.y;

    w = reinterpret_cast<Widget*>(w->getParent());

    if (w == to)
    {
      coords->set(x, y);
      return true;
    }
  }

  // Traverse 'to' -> 'from'.
  w = to;
  x = coords->x;
  y = coords->y;

  while (w->hasParent() && w->getParent()->isWidget())
  {
    x -= w->_clientOrigin.x;
    y -= w->_clientOrigin.y;
    x -= w->_widgetGeometry.x;
    y -= w->_widgetGeometry.y;

    w = reinterpret_cast<Widget*>(w->getParent());

    if (w == from)
    {
      coords->set(x, y);
      return true;
    }
  }

  // These widgets are not in relationship.
  return false;
}

// ============================================================================
// [Fog::Widget - Hit Testing]
// ============================================================================

Widget* Widget::getChildAt(const PointI& pt, bool recursive) const
{
  int x = pt.getX();
  int y = pt.getY();

  if (x < 0 || y < 0 || x > _widgetGeometry.w || y > _widgetGeometry.h) return NULL;

  Widget* current = const_cast<Widget*>(this);

_Repeat:
  {
    List<Object*>::ConstIterator it(current->_children);
    for (it.toEnd(); it.isValid(); it.toPrevious())
    {
      Widget* widget = fog_object_cast<Widget*>(it.value());
      if (widget && widget->_widgetGeometry.contains(pt))
      {
        current = widget;
        if (!recursive) break;

        x -= current->getX();
        y -= current->getY();
        if (current->hasChildren()) goto _Repeat;
        break;
      }
    }
  }

  return current;
}

// ============================================================================
// [Fog::Widget - Layout]
// ============================================================================

void Widget::invalidateLayout()
{
  _dirty = 1;

  if (_layout)
    _layout->updateLayout();
}

void Widget::calculateLayoutHint(LayoutHint& hint)
{
  hint._minimumSize = LayoutItem::calculateMinimumSize();
  hint._maximumSize = LayoutItem::calculateMaximumSize();

  hint._sizeHint = getSizeHint().expandedTo(getMinimumSizeHint());
  hint._sizeHint = hint._sizeHint.boundedTo(getMaximumSize()).expandedTo(getMinimumSize());

  if (_layoutPolicy.isHorizontalPolicyIgnored())
    hint._sizeHint.setWidth(0);
  if (_layoutPolicy.isVerticalPolicyIgnored())
    hint._sizeHint.setHeight(0);

  //TODO: REMOVE THIS AFTER EASY TESTING!!!
  //TODO: REMOVED...
  // hint._sizeHint = SizeI(40, 40);
}

void Widget::setLayout(Layout* lay)
{
  if (lay->_parentItem == this) return;
  if (lay->_parentItem)
  {
    Debug::dbgFunc("Fog::Widget", "setLayout", "Can't set layout that has already parent.\n");
    return;
  }

  deleteLayout();

  if (lay)
  {
    _layout = lay;
    lay->_parentItem = this;

    LayoutEvent e(EVENT_LAYOUT_SET);
    this->sendEvent(&e);
    lay->sendEvent(&e);
  }
}

void Widget::deleteLayout()
{
  Layout* lay = takeLayout();
  if (lay) lay->destroy();
}

Layout* Widget::takeLayout()
{
  Layout* lay = _layout;

  if (lay)
  {
    lay->_parentItem = NULL;
    _layout = NULL;

    invalidateLayout();

    LayoutEvent e(EVENT_LAYOUT_REMOVE);
    lay->sendEvent(&e);
    this->sendEvent(&e);
  }

  return lay;
}

// ============================================================================
// [Layout Policy]
// ============================================================================

bool Widget::hasLayoutHeightForWidth() const
{
  if (isEmpty())
    return false;

  if (_layout)
    return _layout->hasLayoutHeightForWidth();

  return _layoutPolicy.hasHeightForWidth();
}

int Widget::getLayoutHeightForWidth(int width) const
{
  if (isEmpty())
    return -1;

  //TODO: widget margin

  int ret = 0;
  if (!_layout)
    ret = getHeightForWidth(width);
  else
    ret = _layout->getTotalHeightForWidth(width);

  ret = Math::min(ret, getMaximumHeight());
  ret = Math::max(ret, getMinimumHeight());

  ret = Math::max(ret, 0);
  return ret;
}

uint32_t Widget::getLayoutExpandingDirections() const
{
  if (isEmpty())
    return 0;

  uint32_t e = _orientation;

  if (_layout)
  {
    if (_layoutPolicy.getPolicy() & LAYOUT_GROWING_WIDTH  && (_layout->getLayoutExpandingDirections() & ORIENTATION_HORIZONTAL))
      e |= ORIENTATION_HORIZONTAL;
    if (_layoutPolicy.getPolicy() & LAYOUT_GROWING_HEIGHT  && (_layout->getLayoutExpandingDirections() & ORIENTATION_VERTICAL))
      e |= ORIENTATION_VERTICAL;
  }

  if (_alignment & ALIGNMENT_HORIZONTAL_MASK)
    e &= ~ORIENTATION_HORIZONTAL;
  if (_alignment & ALIGNMENT_VERTICAL_MASK)
    e &= ~ORIENTATION_VERTICAL;

  return e;
}

// SetLayoutGeometry using rect as layoutRect (without margins).
void Widget::setLayoutGeometry(const RectI& rect)
{
  // If widget isn't visible -> nothing to do.
  if (isEmpty())
    return;

  // LAYOUT TODO: widget margin.
  RectI r = rect;

  // Make sure the widget will never be bigger than maximum size.
  SizeI s = r.getSize().boundedTo(getLayoutMaximumSize());
  uint32_t alignment = _alignment;

  if (alignment & (ALIGNMENT_HORIZONTAL_MASK | ALIGNMENT_VERTICAL_MASK))
  {
    SizeI prefered(getLayoutSizeHint());
    LayoutPolicy sp = _layoutPolicy;

    if (sp.isHorizontalPolicyIgnored())
    {
      // The getLayoutSizeHint() is ignored. The widget will get prefered Size
      prefered.setWidth(getSizeHint().expandedTo(getMinimumSize()).getWidth());
    }
    if (sp.isVerticalPolicyIgnored())
    {
      // The getLayoutSizeHint() is ignored. The widget will get prefered Size
      prefered.setHeight(getSizeHint().expandedTo(getMinimumSize()).getHeight());
    }

    // LAYOUT TODO: margins!
    //pref += widgetRectSurplus;

    if (alignment & ALIGNMENT_HORIZONTAL_MASK)
    {
      // If preferred size is possible then use it, otherwise use
      // a complete available size.
      s.setWidth(Math::min(s.getWidth(), prefered.getWidth()));
    }

    if (alignment & ALIGNMENT_VERTICAL_MASK)
    {
      if (hasLayoutHeightForWidth())
        s.setHeight(Math::min(s.getHeight(), getLayoutHeightForWidth(s.getWidth())));
      else
        s.setHeight(Math::min(s.getHeight(), prefered.getHeight()));
    }
  }

  //TODO: support for right2left layouts!

  //If no alignment is set, set it to the std. alignment -> left
  if (!(alignment & ALIGNMENT_HORIZONTAL_MASK))
  {
    alignment |= ALIGNMENT_LEFT;
  }

  int x = r.getX();
  int y = r.getY();

  if (alignment & ALIGNMENT_RIGHT)
  {
    x += (r.getWidth() - s.getWidth());
  }
  else if (alignment & ALIGNMENT_HCENTER)
  {
    x += (r.getWidth() - s.getWidth()) / 2;
  }

  if (alignment & ALIGNMENT_BOTTOM)
  {
    y += (r.getHeight() - s.getHeight());
  }
  else if (alignment & ALIGNMENT_VCENTER)
  {
    y += (r.getHeight() - s.getHeight()) / 2;
  }

  // We don't need to use setGeometry(), because the Layout is only activated
  // during update process.
  RectI geometry(x, y, s.getWidth(), s.getHeight());

  if (_guiWindow)
  {
    //Do we really provide LayoutManager for Native Windows?
    setGeometry(geometry);
  }
  else
  {
    // IMPORTANT TODO:
    // TODO WIDGET: create method for this! (currently copied from Base::dispatchConfigure)
    uint32_t changedFlags = 0;

    if (getPosition() != geometry.getPosition())
      changedFlags |= GeometryEvent::CHANGED_WIDGET_POSITION;

    if (getSize() != geometry.getSize())
      changedFlags |= GeometryEvent::CHANGED_WIDGET_SIZE;

    if (changedFlags)
    {
      GeometryEvent e;

      e._widgetGeometry = geometry;
      e._clientGeometry.setRect(0, 0, geometry.w, geometry.h);
      calcClientGeometry(e._clientGeometry);

      _widgetGeometry = e._widgetGeometry;
      _clientGeometry = e._clientGeometry;
      _updateHasNcArea();

      e._changedFlags = changedFlags;
      sendEvent(&e);
    }
  }
}

LayoutPolicy Widget::getLayoutPolicy() const
{
  return _layoutPolicy;
}

void Widget::setLayoutPolicy(const LayoutPolicy& policy)
{
  if (_layoutPolicy == policy) return;

  _layoutPolicy = policy;
  if (_layout) _layout->invalidateLayout();
}

// ============================================================================
// [Layout Height For Width]
// ============================================================================

bool Widget::hasHeightForWidth() const
{
  return false;
}

int Widget::getHeightForWidth(int width) const
{
  return -1;
}

SizeI Widget::getMinimumSizeHint() const
{
  if (_layout)
    return _layout->getTotalMinimumSize();
  else
    return SizeI(-1, -1);
}

SizeI Widget::getMaximumSizeHint() const
{
  if (_layout)
    return _layout->getTotalMaximumSize();
  else
    return SizeI(-1, -1);
}

SizeI Widget::getSizeHint() const
{
  if (_layout)
    return _layout->getTotalSizeHint();
  else
    return SizeI(-1, -1);
}

// ============================================================================
// [Layout Minimum And Maximum Size]
// ============================================================================

bool Widget::checkMinimumSize(int w, int h)
{
  int set = 0;
  if (w >= 0) set |=MAX_WIDTH_IS_SET;
  if (h >= 0) set |=MAX_HEIGHT_IS_SET;

  w = Math::min<int>(w,WIDGET_MAX_SIZE);
  w = Math::max<int>(w,0);
  h = Math::min<int>(h,WIDGET_MAX_SIZE);
  h = Math::max<int>(h,0);

  checkMinMaxBlock();
  if (_extra->_minwidth == w && _extra->_minheight == h)
    return false;

  _extra->_minwidth = w;
  _extra->_minheight = h;
  _minset = set;
  return true;
}

bool Widget::checkMaximumSize(int w, int h)
{
  int set = 0;
  if (w >= 0) set |=MAX_WIDTH_IS_SET;
  if (h >= 0) set |=MAX_HEIGHT_IS_SET;

  w = Math::min<int>(w,WIDGET_MAX_SIZE);
  w = Math::max<int>(w,0);
  h = Math::min<int>(h,WIDGET_MAX_SIZE);
  h = Math::max<int>(h,0);

  checkMinMaxBlock();
  if (_extra->_maxwidth == w && _extra->_maxheight == h)
    return false;

  _extra->_maxwidth = w;
  _extra->_maxheight = h;
  _maxset = set;
  return true;
}

void Widget::setMinimumSize(const SizeI& minSize)
{
  SizeI lastmin(-1,-1);

  if (_extra)
  {
    lastmin.set(_extra->_minwidth, _extra->_minheight);
  }

  if (!checkMinimumSize(minSize.getWidth(),minSize.getHeight())) //nothing changed
  {
    return;
  }

  SizeI size(getMinimumWidth(),getMinimumHeight());

  invalidateLayout();

  if (_widgetGeometry.getWidth() < size.getWidth() || _widgetGeometry.getHeight() < size.getHeight())
  {
    resize(size);
  }

  //TODO: Write EventListener for GuiWindow to allow/disallow min/max
}

void Widget::setMaximumSize(const SizeI& maxSize)
{
  SizeI lastmin(-1, -1);

  if (_extra)
  {
    lastmin.set(_extra->_minwidth, _extra->_minheight);
  }

  if (!checkMaximumSize(maxSize.getWidth(),maxSize.getHeight())) //nothing changed
    return;

  SizeI size(getMaximumWidth(),getMaximumHeight());

  invalidateLayout();

  if (_widgetGeometry.getWidth() > size.getWidth() || _widgetGeometry.getHeight() > size.getHeight())
  {
    resize(size);
  }

  // TODO: Write EventListener for GuiWindow to allow/disallow min/max
}

// ============================================================================
// [Layout State]
// ============================================================================

// LAYOUT TODO: ?

// ============================================================================
// [Fog::Widget - State]
// ============================================================================

void Widget::setEnabled(bool val)
{
  if ( val && _state != WIDGET_DISABLED) return;
  if (!val && _state == WIDGET_DISABLED) return;

  if (_guiWindow)
  {
    if (val)
      _guiWindow->enable();
    else
      _guiWindow->disable();
  }
  else
  {
    GuiEngine* ge = Application::getInstance()->getGuiEngine();
    if (!ge) return;

    ge->dispatchEnabled(this, val);
  }
}

// ============================================================================
// [Fog::Widget - Visibility]
// ============================================================================

void Widget::setVisible(uint32_t val)
{
  //TODO: Check if an optimization makes sense here (hidden_by_parent)
  if (val == _visibility) return;

  if (val == WIDGET_VISIBLE_FULLSCREEN)
  {
    if (!_guiWindow) return;

    int rrr = _windowFlags & WINDOW_TRANSPARENT;

    _fullscreendata = fog_new FullScreenData;
    _fullscreendata->_restorewindowFlags = _windowFlags;
    _fullscreendata->_restoregeometry = _guiWindow->_windowRect;
    _fullscreendata->_restoretransparency = _transparency;

    GuiEngine::DisplayInfo info;
    Application::getInstance()->getGuiEngine()->getDisplayInfo(&info);

    setWindowFlags(WINDOW_TYPE_FULLSCREEN | getWindowHints());
    setGeometry(RectI(0,0,info.width, info.height));
  }
  else if (_visibility == WIDGET_VISIBLE_FULLSCREEN && val == WIDGET_VISIBLE)
  {
    FOG_ASSERT(_fullscreendata);
    _visibility = WIDGET_VISIBLE;
    setWindowFlags(_fullscreendata->_restorewindowFlags);
    setGeometry(_fullscreendata->_restoregeometry);

    fog_delete(_fullscreendata);
    _fullscreendata = 0;
  }

  if (_guiWindow)
  {
    if (val >= WIDGET_VISIBLE_MINIMIZED)
    {
      if (_guiWindow->isModal() && val != WIDGET_VISIBLE_RESTORE)
      {
        // TODO: Application wide modality do not have a owner
        if (_guiWindow->getModality() == MODAL_WINDOW)
        {
          // Start this window as modal window above our owner.
          _guiWindow->getOwner()->startModalWindow(_guiWindow);
        }
        else
        {
          // TODO: What here?
        }
      }
      _guiWindow->show(val);
    }
    else
    {
      if (_guiWindow->isModal() && val == WIDGET_HIDDEN)
      {
        if (_guiWindow->getModality() == MODAL_WINDOW)
        {
          _guiWindow->getOwner()->endModal(_guiWindow);
        }
        else
        {
          // TODO: What here?
        }
      }
      _guiWindow->hide();
    }
  }
  else
  {
    GuiEngine* ge = Application::getInstance()->getGuiEngine();
    if (!ge) return;

    ge->dispatchVisibility(this, val);
  }

  if (val == WIDGET_VISIBLE_RESTORE)
  {
    _visibility = WIDGET_VISIBLE;
  }
  else
  {
    _visibility = val;
  }
}

void Widget::setTransparency(float val)
{
  if (val < 0.0f)
  {
    val = 0.0f;
  }
  else if (val > 1.0f)
  {
    val = 1.0f;
  }

  if (_guiWindow)
  {
    //some window manager need a flag to be set (e.g. windows)
    //so make sure the flag is already set
    _transparency = val;
    _guiWindow->setTransparency(val);
  }
  else
  {
    // TODO WIDGET: Transparency.
  }
}

void Widget::showModal(GuiWindow* owner)
{
  if (_guiWindow && !_guiWindow->getOwner())
  {
    // Only TopLevel Windows may be modal!
    if (owner != 0)
    {
      _guiWindow->setModal(MODAL_WINDOW);
      // This will implicitly set this window above owner window!
      _guiWindow->setOwner(owner);
    }
    else
    {
      _guiWindow->setModal(MODAL_APPLICATION);
    }

    setVisible(WIDGET_VISIBLE);
  }
}

// ============================================================================
// [Fog::Widget - Window Style]
// ============================================================================

void Widget::setWindowFlags(uint32_t flags)
{
  if (flags == _windowFlags) return;

  if (_guiWindow)
  {
    if (_visibility == WIDGET_VISIBLE_FULLSCREEN)
    {
      // If it is currently fullscreen, just set the restore flags to flags!
      _fullscreendata->_restorewindowFlags = flags;
      return;
    }
    else
    {
      _guiWindow->create(flags);
    }
  }

  _windowFlags = flags;

  //setTransparency(_transparency);
}

void Widget::setWindowHints(uint32_t flags)
{
  if (flags == getWindowHints()) return;

  // Make sure to keep window type and to only update the hints.
  flags = (_windowFlags & WINDOW_TYPE_MASK) | (flags & WINDOW_HINTS_MASK);

  setWindowFlags(flags);
}

// TODO: Update unused, purpose?
void Widget::changeFlag(uint32_t flag, bool set, bool update)
{
  uint32_t flags = _windowFlags;

  if (set)
  {
    flags |= flag;
  }
  else
  {
    flags &= ~flag;
  }

  setWindowFlags(flags);
}

void Widget::setDragAble(bool drag, bool update)
{
  if (drag == isDragAble()) return;

  changeFlag(WINDOW_DRAGABLE, drag, update);
}

void Widget::setResizeAble(bool resize, bool update)
{
  if (resize == isResizeAble()) return;

  changeFlag(WINDOW_FIXED_SIZE, !resize, update);
}

// ============================================================================
// [Fog::Widget - Orientation]
// ============================================================================

void Widget::setOrientation(uint32_t val)
{
  if (getOrientation() == val) return;

  _orientation = val;

  GuiEngine* ge = Application::getInstance()->getGuiEngine();
  if (!ge) return;

  ge->dispatchConfigure(this, _widgetGeometry, true);
}

// ============================================================================
// [Fog::Widget - Caret]
// ============================================================================

#if 0
bool Widget::showCaret()
{
  return 0;
}

bool Widget::showCaret(const CaretProperties& properties)
{
  return 0;
}

bool Widget::hideCaret()
{
  return 0;
}
#endif

// ============================================================================
// [Fog::Widget - TabOrder]
// ============================================================================

void Widget::setTabOrder(int tabOrder)
{
  _tabOrder = tabOrder;
}

// ============================================================================
// [Fog::Widget - Focus]
// ============================================================================

void Widget::setFocusPolicy(uint32_t val)
{
  _focusPolicy = val;
}

Widget* Widget::getFocusableWidget(int focusable)
{
  return NULL;
}

void Widget::takeFocus(uint32_t reason)
{
  if (!hasFocus() && getVisibility() == WIDGET_VISIBLE && getState() == WIDGET_ENABLED)
  {
    // TODO:
    //Application::getInstance()->getGuiEngine()->dispatchTakeFocus(this, reason);
  }
}

void Widget::giveFocusNext(uint32_t reason)
{
  Widget* w = getFocusableWidget(FOCUSABLE_NEXT);
  if (w) w->takeFocus(reason);
}

void Widget::giveFocusPrevious(uint32_t reason)
{
  Widget* w = getFocusableWidget(FOCUSABLE_PREVIOUS);
  if (w) w->takeFocus(reason);
}

Widget* Widget::_findFocus() const
{
  Widget* w = const_cast<Widget*>(this);
  while (w->_focusLink) w = w->_focusLink;
  return w;
}

// ============================================================================
// [Fog::Widget - Font]
// ============================================================================

void Widget::setFont(const Font& font)
{
  _font = font;
  update(WIDGET_UPDATE_GEOMETRY | WIDGET_REPAINT_ALL);
}

// ============================================================================
// [Fog::Widget - Update]
// ============================================================================

void Widget::update(uint32_t updateFlags)
{
  uint32_t u = _uflags;
  //if ((u & updateFlags) == updateFlags || (u & WIDGET_UPDATE_ALL)) return;

  _uflags |= updateFlags | WIDGET_UPDATE_SOMETHING;

  if (u & WIDGET_UPDATE_SOMETHING) return;

  if (_guiWindow)
  {
    if (!_guiWindow->isDirty()) _guiWindow->setDirty();
    return;
  }

  // No UFlagUpdate nor GuiWindow, traverse all parents up to GuiWindow and
  // set UFlagUpdateChild to all parents.
  Widget* w = getParentWidget();
  while (w && !(w->_uflags & (WIDGET_UPDATE_CHILD | WIDGET_UPDATE_ALL)))
  {
    w->_uflags |= WIDGET_UPDATE_CHILD;
    if (w->_guiWindow)
    {
      if (!w->_guiWindow->isDirty()) w->_guiWindow->setDirty();
      return;
    }
    w = w->getParentWidget();
  }
}

// ============================================================================
// [Fog::Widget - Repaint]
// ============================================================================

uint32_t Widget::getPaintHint() const
{
  return WIDGET_PAINT_SCREEN;
}

err_t Widget::getPropagatedRegion(Region* dst) const
{
  return dst->set(BoxI(0, 0, getWidth(), getHeight()));
}

// ============================================================================
// [Fog::Widget - Events]
// ============================================================================

void Widget::onState(StateEvent* e)
{
}

void Widget::onVisibility(VisibilityEvent* e)
{
}

void Widget::onGeometry(GeometryEvent* e)
{
}

void Widget::onFocus(FocusEvent* e)
{
}

void Widget::onKey(KeyEvent* e)
{
}

void Widget::onNcMouse(MouseEvent* e)
{
}

void Widget::onMouse(MouseEvent* e)
{
}

void Widget::onClick(MouseEvent* e)
{
}

void Widget::onDoubleClick(MouseEvent* e)
{
}

void Widget::onWheel(MouseEvent* e)
{
}

void Widget::onSelection(SelectionEvent* e)
{
}

void Widget::onNcPaint(PaintEvent* e)
{
}

void Widget::onPaint(PaintEvent* e)
{
}

void Widget::onClose(CloseEvent* e)
{
}

void Widget::onThemeChange(ThemeEvent* e)
{
}

void Widget::onLayout(LayoutEvent* e)
{
}

} // Fog namespace
