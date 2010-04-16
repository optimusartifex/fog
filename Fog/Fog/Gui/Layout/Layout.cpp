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

Layout::Layout() : _parentItem(0), _margininvalid(1), _toplevel(0), _spacing(0), _enabled(1), _activated(1)
{
  _flags |= OBJ_IS_LAYOUT;  
}

Layout::Layout(Widget *parent, Layout* parentLayout) : _margininvalid(1), _toplevel(0), _spacing(0), _enabled(1), _activated(1), _parentItem(0) {
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

//Macro for easy iteration over all items
//Item will be accessible over VARNAME
//at code you can add the code to execute for every item
//The index will always be accessible with in variable i
#define FOR_EACH_ITEM(VARNAME, CODE, THIS) \
int i = 0;\
LayoutItem *VARNAME = THIS->getAt(i);\
while (VARNAME) {\
  CODE\
  ++i;\
  VARNAME = THIS->getAt(i);\
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
  FOR_EACH(iitem, this) {
    if (iitem == item)
      return i;
  }

  return -1;
}

void Layout::remove(LayoutItem* item) {
  FOR_EACH(iitem, this) {
    if (iitem == item) {
      takeAt(i);
      invalidateLayout();
    }
  }
}

bool Layout::removeWidgetRecursively(LayoutItem *li, Widget *w)
{  
  if (!li->isLayout())
    return false;

  if(!li->_withinLayout)
    return false;

  Layout *lay = static_cast<Layout*>(li);

  FOR_EACH(iitem, lay) {
    if (iitem == w) {
      delete lay->takeAt(i);
      lay->invalidateLayout();
      return true;
    } else if (removeWidgetRecursively(iitem, w)) {
      return true;
    } 
  }

  return false;
}

void Layout::reparentChildWidgets(Widget* mw)
{
  FOR_EACH(iitem, this) {
    if(iitem->isWidget()) {
      Widget* w = static_cast<Widget*>(iitem);
      if (w->getParent() != mw)
        w->setParent(mw);
    } else if (iitem->isLayout()) {
      static_cast<Layout*>(iitem)->reparentChildWidgets(mw);
    }
  }
}

void Layout::add(LayoutItem* item) {
  item->_withinLayout = this;

  if(item->isWidget()) {
    Widget* w = static_cast<Widget*>(item);
    Widget *mw = getParentWidget();
    Widget *pw = w->getParent();

    if(pw && pw->getLayout()) {
      if(removeWidgetRecursively(pw->getLayout(), w)) {
        //WARNING: removed from existing layout
      }
    }

    if (pw && mw && pw != mw) {
      w->setParent(mw);
    } else if(!pw && mw) {
      w->setParent(mw);
    }
  }
}

IntSize Layout::getTotalMinimumSize() const
{
  int side=0, top=0;
  if (_toplevel) {
    //    Widget *pw = parentWidget();
    //     QWidgetPrivate *wd = pw->d_func();
    //     side += wd->leftmargin + wd->rightmargin;
    //     top += wd->topmargin + wd->bottommargin;
  }

  IntSize s = getLayoutMinimumSize();
  return s + IntSize(side, top);
}

IntSize Layout::getTotalMaximumSize() const { 
  int side=0, top=0;
  if (_toplevel) {
    //    Widget *pw = parentWidget();
    //     QWidgetPrivate *wd = pw->d_func();
    //     side += wd->leftmargin + wd->rightmargin;
    //     top += wd->topmargin + wd->bottommargin;
  }

  IntSize s = getLayoutMaximumSize();

  if (_toplevel)
    s = IntSize(Math::min<int>(s.getWidth() + side, WIDGET_MAX_SIZE), Math::min<int>(s.getHeight() + top, WIDGET_MAX_SIZE));

  return s;
}

int Layout::getTotalHeightForWidth(int w) const
{
  int side=0, top=0;
  if (_toplevel) {
//     QWidget *parent = parentWidget();
//     parent->ensurePolished();
//     QWidgetPrivate *wd = parent->d_func();
//     side += wd->leftmargin + wd->rightmargin;
//     top += wd->topmargin + wd->bottommargin;
  }
  int h = getLayoutHeightForWidth(w - side) + top;
  return h;
}


IntSize Layout::getTotalSizeHint() const
{
  int side=0, top=0;
  if (_toplevel) {
    //     QWidget *parent = parentWidget();
    //     parent->ensurePolished();
    //     QWidgetPrivate *wd = parent->d_func();
    //     side += wd->leftmargin + wd->rightmargin;
    //     top += wd->topmargin + wd->bottommargin;
  }

  IntSize s = getLayoutSizeHint();
  if (hasLayoutHeightForWidth())
    s.setHeight(getLayoutHeightForWidth(s.getWidth() + side));

  return s + IntSize(side, top);
}

void Layout::callSetGeometry(const IntSize& size) {
  IntRect rect = getParentWidget()->getClientGeometry();
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

void Layout::activateAll(LayoutItem *item, bool activate)
{
  item->invalidateLayout();
  if (item->isLayout()) {
    FOR_EACH_ITEM(child, 
    {
      activateAll(child, activate);
    }, this)

    ((Layout*)item)->_activated = true;
  }
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
  activateAll(this, true);

  Widget *md = mw;
  bool hasH = md->hasMinimumHeight();
  bool hasW = md->hasMinimumWidth();
  bool calc = (!hasH || !hasW);
  
  if (mw->isGuiWindow()) {
    IntSize ms = getTotalMinimumSize();
    bool calc = hasLayoutHeightForWidth();  
    if (hasW) {
      ms.setWidth(mw->getMinimumSize().getWidth());      
    }
    if (hasH) {
      ms.setHeight(mw->getMinimumSize().getHeight());
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
      activate();

  }
}



} // Fog namespace
