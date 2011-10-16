// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Engine/GuiEngine.h>
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Gui/Layout/LayoutItem.h>
#include <Fog/Gui/Widget/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::Layout)

namespace Fog {

// ============================================================================
// [Fog::Layout - Helpers]
// ============================================================================

#define FOG_LAYOUT_FOR_EACH(_CONTAINER_, _ITEM_) \
  LayoutItem* _ITEM_; \
  int i = 0; \
  while ((_ITEM_ = (_CONTAINER_)->getAt(i++)))

// ============================================================================
// [Fog::Layout]
// ============================================================================

Layout::Layout() :
  _parentItem(0),
  _flexcount(0),
  _spacing(0),
  _toplevel(0),
  _activated(1),
  _enabled(1),
  _nextactivate(0)
{
  _objectFlags |= OBJECT_FLAG_IS_LAYOUT;
}

Layout::Layout(Widget* parent, Layout* parentLayout) :
  _flexcount(0),
  _toplevel(0),
  _spacing(0),
  _enabled(1),
  _activated(1),
  _parentItem(0),
  _nextactivate(0)
{
  _objectFlags |= OBJECT_FLAG_IS_LAYOUT;

  if (parentLayout)
  {
    _parentItem = parent;
     parentLayout->addChild(this);
  }
  else if (parent)
  {
    parent->setLayout(this);
    if (_parentItem == parent)
    {
      parent->_withinLayout = this;
      _toplevel = 1;

      updateLayout();
    }
  }
  else
  {
    _parentItem = 0;
  }
}

Layout::~Layout()
{
  FOG_ASSERT(_nextactivate == NULL);

  while(_children.getLength())
  {
    remove(_children.getAt(0));
  }
}

void Layout::onRemove(LayoutItem* item)
{
}

void Layout::updateLayout()
{
  if (_toplevel)
  {
    markAsDirty();
  }
  else
  {
    if (_withinLayout)
    {
      _withinLayout->markAsDirty();
    }
  }
}

void Layout::invalidateLayout()
{
  _dirty = 1;
}

void Layout::markAsDirty()
{
  if (_toplevel == 1)
  {
    if (_activated == 1)
    {
      Widget* w = getParentWidget();
      if (w)
      {
        GuiWindow* window = w->getClosestGuiWindow();
        if (window)
        {
          FOG_ASSERT(window->_activatelist == 0);

          _activated = 0;
          _nextactivate = window->_activatelist;
          window->_activatelist = this;
          w->update(WIDGET_REPAINT_ALL);
        }
      }
    }
  }
  else
  {
    // TODO: Remove this assert?
    //FOG_ASSERT(_withinLayout);
    if (_withinLayout)
      return _withinLayout->markAsDirty();
  }
}

Widget* Layout::getParentWidget() const
{
  LayoutItem* parent = _parentItem;
  if (parent == NULL) return NULL;

  do {
    if (parent->isWidget())
    {
      return reinterpret_cast<Widget*>(parent);
    }

    FOG_ASSERT(parent->isLayout());
    parent = reinterpret_cast<Layout*>(parent)->_parentItem;
  } while (parent);

  return NULL;

/*
  if (!_toplevel)
  {
    if (_parentItem)
    {
      FOG_ASSERT(_parentItem->isLayout());
      return static_cast<Layout*>(_parentItem)->getParentWidget();
    }
    else
    {
      return NULL;
    }
  }

  FOG_ASSERT(_parentItem && _parentItem->isWidget());
  return (Widget*)(_parentItem);
*/
}

int Layout::calcMargin(int margin, uint32_t location) const
{
  if (margin >= 0)
  {
    return margin;
  }
  else if (!_toplevel)
  {
    return 0;
  }
  else if (const Widget* pw = getParentWidget())
  {
    // TODO: Move margins to theme?
    if (pw->hasGuiWindow())
      return LAYOUT_DEFAULT_WINDOW_MARGIN;
    else
      return LAYOUT_DEFAULT_WIDGET_MARGIN;
  }

  return 0;
}

RectI Layout::getContentsRect() const
{
  return _rect.adjusted(
    +_contentmargin.left,
    +_contentmargin.top,
    -_contentmargin.right,
    -_contentmargin.bottom);
}

int Layout::getSpacing() const
{
  if (_spacing >= 0)
  {
    return _spacing;
  }
  else
  {
    if (!_parentItem)
    {
      return -1;
    }
    else if (_parentItem->isWidget())
    {
      return LAYOUT_DEFAULT_SPACING;
    }
    else
    {
      FOG_ASSERT(_parentItem->isLayout());
      return static_cast<Layout*>(_parentItem)->getSpacing();
    }

    return 0;
  }
}

void Layout::setSpacing(int spacing)
{
  _spacing = spacing;
}

uint32_t Layout::getLayoutExpandingDirections() const
{
  // LAYOUT TODO:
  return ORIENTATION_HORIZONTAL | ORIENTATION_VERTICAL;
}

bool Layout::hasLayoutHeightForWidth() const
{
  return false;
}

void Layout::setLayoutGeometry(const RectI& r)
{
  _rect = r;
}

RectI Layout::getLayoutGeometry() const
{
  return _rect;
}

bool Layout::setLayoutAlignment(LayoutItem* item, uint32_t alignment)
{
  if (item->_withinLayout == this)
  {
    item->setLayoutAlignment(alignment);
    return true;
  }

  return false;
}

bool Layout::isEmpty() const
{
  if (_children.getLength() == 0)
    return true;

  FOG_LAYOUT_FOR_EACH(this, iitem)
  {
    if (!iitem->isEmpty())
      return false;
  }

  return true;
}

int Layout::indexOf(LayoutItem* item) const
{
  if (item->_withinLayout != this)
    return -1;

  FOG_LAYOUT_FOR_EACH(this, iitem)
  {
    if (iitem == item)
      return i;
  }

  return -1;
}

void Layout::remove(LayoutItem* item)
{
  if (item->_withinLayout != this)
    return;

  FOG_LAYOUT_FOR_EACH(this, iitem)
  {
    if (iitem == item)
    {
      item->_withinLayout = 0;
      item->removeLayoutStruct();
      takeAt(i);
      // Call virtual method for LM implementation.
      onRemove(item);
      invalidateLayout();
      updateLayout();
      break;
    }
  }
}

bool Layout::removeAllWidgets(LayoutItem* layout, Widget* w)
{
  if (!layout->_withinLayout)
    return false;

  if (!layout->isLayout())
    return false;

  Layout* lay = static_cast<Layout*>(layout);

  FOG_LAYOUT_FOR_EACH(lay, iitem)
  {
    if (iitem == w)
    {
      lay->takeAt(i);
      lay->onRemove(iitem);
      w->_withinLayout = 0;
      lay->invalidateLayout();
      return true;
    }
    else if (removeAllWidgets(iitem, w))
    {
      return true;
    }
  }

  return false;
}

// Reparent.
void Layout::reparentChildWidgets(Widget* widget)
{
  FOG_LAYOUT_FOR_EACH(this, iitem)
  {
    if (iitem->isWidget())
    {
      Widget* w = static_cast<Widget*>(iitem);
      if (w->getParent() != widget)
        w->setParent(widget);
    }
    else if (iitem->isLayout())
    {
      static_cast<Layout*>(iitem)->reparentChildWidgets(widget);
    }
  }
}

int Layout::addChild(LayoutItem* item)
{
  if (item->isWidget())
  {
    Widget* widget = static_cast<Widget*>(item);

    Widget* parentLayout = this->getParentWidget();
    Widget* parentWidget = widget->getParentWidget();

    if (parentWidget != NULL &&
        parentWidget->getLayout() != NULL &&
        item->_withinLayout)
    {
      if (removeAllWidgets(parentWidget->getLayout(), widget))
      {
        // LAYOUT TODO: WARNING: removed from existing layout.
      }
    }

    if (parentWidget != NULL &&
        parentLayout != NULL &&
        parentWidget != parentLayout)
    {
      widget->setParent(parentLayout);
    }
    else if (parentWidget == NULL && parentLayout != NULL)
    {
      widget->setParent(parentLayout);
    }
  }
  else if (item->isLayout())
  {
    // Support for Flex-Layout?
    return addChildLayout(static_cast<Layout*>(item));
  }

  item->_withinLayout = this;
  _children.append(item);
  invalidateLayout();

  return getLength() - 1;
}

int Layout::addChildLayout(Layout* l)
{
  if (l->_parentItem)
  {
    // TODO: WARNING already has a parent!
    return -1;
  }

  if (Widget* mw = getParentWidget())
  {
    l->reparentChildWidgets(mw);
  }

  l->_parentItem = this;
  l->_toplevel = 0;
  l->_withinLayout = this;

  _children.append(l);
  return getLength() -1;
}

SizeI Layout::getTotalMinimumSize() const
{
  int side = 0, top = 0;
  calcContentMargins(side, top);

  SizeI s = getLayoutMinimumSize();
  return s + SizeI(side, top);
}

SizeI Layout::getTotalMaximumSize() const
{
  int side = 0, top = 0;
  calcContentMargins(side, top);

  SizeI s = getLayoutMaximumSize();
  if (_toplevel)
  {
    s = SizeI(Math::min<int>(s.getWidth() + side, WIDGET_MAX_SIZE), Math::min<int>(s.getHeight() + top, WIDGET_MAX_SIZE));
  }

  return s;
}

int Layout::getTotalHeightForWidth(int w) const
{
  int side = 0, top = 0;
  calcContentMargins(side,top);
  int h = getLayoutHeightForWidth(w - side) + top;
  return h;
}

void Layout::calcContentMargins(int& side, int& top) const
{
  if (_toplevel)
  {
    Widget* parent = getParentWidget();
    FOG_ASSERT(parent);

    side = parent->getContentLeftMargin() + parent->getContentRightMargin();
    top = parent->getContentTopMargin() + parent->getContentBottomMargin();
  }
}

SizeI Layout::getTotalSizeHint() const
{
  int side=0, top=0;
  calcContentMargins(side,top);

  SizeI s = getLayoutSizeHint();
  if (hasLayoutHeightForWidth())
    s.setHeight(getLayoutHeightForWidth(s.getWidth() + side));

  return s + SizeI(side, top);
}

void Layout::callSetGeometry(const SizeI& size)
{
  if (size.isValid())
  {
    RectI rect = getParentWidget()->getClientContentGeometry();
    //TODO: EntireRect
    setLayoutGeometry(rect);
  }
}

bool Layout::activate()
{
  // Only TopLevel-Layouts are being activated.
  // Child Layouts are handled like normal widgets.

  // There is no need to check this in release...
  FOG_ASSERT(_toplevel);
  FOG_ASSERT(!_activated);
  FOG_ASSERT(_parentItem && _parentItem->isWidget());

  if (!isEnabled() || isEmpty())
    return false;

  Widget* parentwidget = static_cast<Widget*>(_parentItem);

  bool hasH = parentwidget->hasMinimumHeight();
  bool hasW = parentwidget->hasMinimumWidth();
  bool calc = (!hasH || !hasW);

  if (parentwidget->hasGuiWindow())
  {
    SizeI ms = getTotalMinimumSize();
    if (calc)
    {
      if (hasW) ms.setWidth(parentwidget->getMinimumSize().getWidth());
      if (hasH) ms.setHeight(parentwidget->getMinimumSize().getHeight());
    }

    if (calc && hasLayoutHeightForWidth())
    {
      int h = getLayoutMinimumHeightForWidth(ms.getWidth());
      if (h > ms.getHeight())
      {
        if (!hasH) ms.setHeight(0);
        if (!hasW) ms.setWidth(0);
      }
    }
    parentwidget->setMinimumSize(ms);
  }
  else if (calc)
  {
    SizeI ms = parentwidget->getMinimumSize();
    if (!hasH) ms.setHeight(0);
    if (!hasW) ms.setWidth(0);
    parentwidget->setMinimumSize(ms);
  }

  callSetGeometry(parentwidget->getSize());
  _activated = 1;
  return true;
}


void Layout::onLayout(LayoutEvent* e)
{
  if (!isEnabled())
    return;

  if (e->_code == EVENT_LAYOUT_REQUEST)
  {
    FOG_ASSERT(!_parentItem || (_parentItem && _parentItem->isWidget()));
    if (static_cast<Widget*>(_parentItem)->isVisible())
    {
      activate();
    }
  }
}

} // Fog namespace
