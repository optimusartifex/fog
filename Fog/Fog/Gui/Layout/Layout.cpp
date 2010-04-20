// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Gui/Layout/LayoutItem.h>
#include <Fog/Gui/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::Layout)

namespace Fog {

// ============================================================================
// [Fog::Layout]
// ============================================================================

Layout::Layout() : _parentItem(0), _margininvalid(1), _toplevel(0), _spacing(0), _enabled(1), _activated(1), _flexcount(0)
{
  _flags |= OBJ_IS_LAYOUT;  
}

Layout::Layout(Widget *parent, Layout* parentLayout) : _flexcount(0), _margininvalid(1), _toplevel(0), _spacing(0), _enabled(1), _activated(1), _parentItem(0) {
  _flags |= OBJ_IS_LAYOUT;

  if (parentLayout) {
    _parentItem = parent;
     parentLayout->add(this);
  } else if (parent) { 
     parent->setLayout(this);
     if(_parentItem == parent) {
       parent->_withinLayout = this;
       _toplevel = 1;

       invalidateLayout();
     }
  } else {
    _parentItem = 0;
  }
}

Layout::~Layout() 
{
}

int Layout::calcMargin(int margin) const {
  if (margin >= 0) {
    return margin;
  } else if (!_toplevel) {
    return 0;
  } else if (const Widget *pw = getParentWidget()) {
    if(pw->isGuiWindow()) {
      return DEFAULT_WINDOW_MARGIN;
    } else {
      return DEFAULT_WIDGET_MARGIN;
    }
  }

  return 0;
}

#define FOR_EACH(ITEM, CONTAINER) \
LayoutItem *ITEM;\
int i=0;\
while ((ITEM = CONTAINER->getAt(i++)))

bool Layout::isEmpty() const {
  FOR_EACH(iitem, this) {
    if (!iitem->isEmpty())
      return false;
  }

  return true;
}

int Layout::indexOf(LayoutItem* item) const
{
  if(item->_withinLayout != this)
    return -1;

  FOR_EACH(iitem, this) {
    if (iitem == item)
      return i;
  }

  return -1;
}

void Layout::remove(LayoutItem* item) {
  if(item->_withinLayout != this)
    return;

  FOR_EACH(iitem, this) {
    if (iitem == item) {
      if(item->hasFlex())
        removeFlexItem();
      item->_withinLayout = 0;
      takeAt(i);
      invalidateLayout();
    }
  }
}

bool Layout::removeAllWidgets(LayoutItem *layout, Widget *w)
{  
  if(!layout->_withinLayout)
    return false;

  if (!layout->isLayout())
    return false;

  Layout *lay = static_cast<Layout*>(layout);

  FOR_EACH(iitem, lay) {
    if (iitem == w) {
      lay->takeAt(i);
      if(iitem->hasFlex())
        lay->removeFlexItem();
      w->_withinLayout = 0;
      lay->invalidateLayout();
      return true;
    } else if (removeAllWidgets(iitem, w)) {
      return true;
    } 
  }

  return false;
}

//reparent 
void Layout::reparentChildWidgets(Widget* widget)
{
  FOR_EACH(iitem, this) {
    if(iitem->isWidget()) {
      Widget* w = static_cast<Widget*>(iitem);
      if (w->getParent() != widget)
        w->setParent(widget);
    } else if (iitem->isLayout()) {
      static_cast<Layout*>(iitem)->reparentChildWidgets(widget);
    }
  }
}

void Layout::add(LayoutItem* item) 
{
  if(item->isWidget()) {
    Widget* w = static_cast<Widget*>(item);
    Widget *layoutparent = getParentWidget();
    Widget *widgetparent = w->getParent();

    if(widgetparent && widgetparent->getLayout() && item->_withinLayout) {
      if(removeAllWidgets(widgetparent->getLayout(), w)) {
        //WARNING: removed from existing layout
      }
    }

    if (widgetparent && layoutparent && widgetparent != layoutparent) {
      w->setParent(layoutparent);
    } else if(!widgetparent && layoutparent) {
      w->setParent(layoutparent);
    }
  }

  item->_withinLayout = this;
  if(item->hasFlex()) {
    addFlexItem();
  }
}

IntSize Layout::getTotalMinimumSize() const
{
  int side=0, top=0;
  calcContentMargins(side,top);

  IntSize s = getLayoutMinimumSize();
  return s + IntSize(side, top);
}

IntSize Layout::getTotalMaximumSize() const { 
  int side=0, top=0;
  calcContentMargins(side,top);

  IntSize s = getLayoutMaximumSize();
  if (_toplevel) {
    s = IntSize(Math::min<int>(s.getWidth() + side, WIDGET_MAX_SIZE), Math::min<int>(s.getHeight() + top, WIDGET_MAX_SIZE));
  }

  return s;
}

int Layout::getTotalHeightForWidth(int w) const
{
  int side=0, top=0;
  calcContentMargins(side,top);
  int h = getLayoutHeightForWidth(w - side) + top;
  return h;
}

void Layout::calcContentMargins(int&side, int&top) const {
  if (_toplevel) {
    Widget* parent = getParentWidget();
    FOG_ASSERT(parent);
    side = parent->getContentLeftMargin() + parent->getContentRightMargin();
    top = parent->getContentTopMargin() + parent->getContentBottomMargin();
  }
}

IntSize Layout::getTotalSizeHint() const
{
  int side=0, top=0;
  calcContentMargins(side,top);

  IntSize s = getLayoutSizeHint();
  if (hasLayoutHeightForWidth())
    s.setHeight(getLayoutHeightForWidth(s.getWidth() + side));

  return s + IntSize(side, top);
}

void Layout::callSetGeometry(const IntSize& size) {
  IntRect rect = getParentWidget()->getClientContentGeometry();
  //IntRect rect = mw->testAttribute(Qt::WA_LayoutOnEntireRect) ? mw->getGeometry() : mw->getContentGeometry()
  setLayoutGeometry(rect);
}

void Layout::addChildLayout(Layout *l)
{
  if (l->_parentItem) {
    //WARNING already has a parent!
    return;
  }

  l->_parentItem = this;
  l->_withinLayout = this;
  l->_toplevel = 0;
  l->_margininvalid = 1;

  if (Widget *mw = getParentWidget()) {
    l->reparentChildWidgets(mw);
  }
}

void Layout::update() {
  Layout *layout = this;
  while (layout && layout->_activated) {
    layout->_activated = false;
    if (layout->_toplevel) {
      FOG_ASSERT(layout->_parentItem->isWidget());      
      LayoutEvent e(EVENT_LAYOUT_REQUEST);
      sendEvent(&e);
      break;
    }
    FOG_ASSERT(!layout->_parentItem || (layout->_parentItem && layout->_parentItem->isLayout()));
    layout = static_cast<Layout*>(layout->_parentItem);
  }
}

//Method invalidates all children
//Also it will mark all Layouts in the hierarchy to the value of activate
void Layout::invalidActivateAll(LayoutItem *item, bool activate)
{
  FOG_ASSERT(item->isLayout());

  item->invalidateLayout();
  FOR_EACH(child, this)
  {
    if (child->isLayout()) {
      invalidActivateAll(child, activate);
    } else {
      child->invalidateLayout();
    }
  }

  ((Layout*)item)->_activated = true;
}

bool Layout::activate() {
  if (!isEnabled() || !_parentItem)
    return false;  

  if (!_toplevel) {
    FOG_ASSERT(!_parentItem || (_parentItem && _parentItem->isLayout()));
    return static_cast<Layout*>(_parentItem)->activate();
  }
  if (_activated)
    return false;

  Widget *mw = static_cast<Widget*>(_parentItem);
  if (mw == 0) {
    return false;
  }
  FOG_ASSERT(!_parentItem || (_parentItem && _parentItem->isWidget()));

  //invalid all childs and mark them directly as activated
  //mark all childs as activated (recursive!)
  invalidActivateAll(this, true);

  Widget *md = mw;
  bool hasH = md->hasMinimumHeight();
  bool hasW = md->hasMinimumWidth();
  bool calc = (!hasH || !hasW);
  
  if (mw->isGuiWindow()) {
    IntSize ms = getTotalMinimumSize();
    if(calc) {
      if (hasW) {
        ms.setWidth(mw->getMinimumSize().getWidth());      
      }
      if (hasH) {
        ms.setHeight(mw->getMinimumSize().getHeight());
      }
    }

    if (calc && hasLayoutHeightForWidth()) {
      int h = getLayoutMinimumHeightForWidth(ms.getWidth());
      if (h > ms.getHeight()) {
        if (!hasH)
          ms.setHeight(0);
        if (!hasW)
          ms.setWidth(0);
      }
    }
    mw->setMinimumSize(ms);
  } else if (calc) {
    IntSize ms = mw->getMinimumSize();
    if (!hasH) {
      ms.setHeight(0);
    }
    if (!hasW) {
      ms.setWidth(0);
    }
    mw->setMinimumSize(ms);
  }

  callSetGeometry(mw->getSize());
  return false;
}


void Layout::onLayout(LayoutEvent* e) {
  if (!isEnabled())
    return;

  if(e->_code == EVENT_LAYOUT_REQUEST) {
    FOG_ASSERT(!_parentItem || (_parentItem && _parentItem->isWidget()));
    if (static_cast<Widget *>(_parentItem)->isVisible())
    {
      activate();
    }
  }
}



} // Fog namespace
