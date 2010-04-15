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

Layout::Layout() : _parentItem(0)
{
  _flags |= OBJ_IS_LAYOUT;
}

Layout::Layout(Widget *parent, Layout* parentLayout) {
  _flags |= OBJ_IS_LAYOUT;

  if (parentLayout) {
    _parentItem = parent;
     parentLayout->add(this);
  } else if (parent) {
    if (parent->getLayout()) {
      //WARNING!
    } else {
      _toplevel = true;

      _parentItem = parent;
       parent->setLayout(this);
    }
  } else {
    _parentItem = 0;
  }
}

Layout::~Layout() 
{
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

bool Layout::isEmpty() const {
  FOR_EACH_ITEM(iitem, 
  {
    if (!iitem->isEmpty())
      return false;
  }, this)

  return true;
}

int Layout::indexOf(LayoutItem* item) const
{
  FOR_EACH_ITEM(iitem, 
  {
    if (iitem == item)
      return i;
  }, this)
  return -1;
}

void Layout::remove(LayoutItem* item) {
  FOR_EACH_ITEM(iitem, 
  {
    if (iitem == item) {
      takeAt(i);
      invalidateLayout();
    }
  }, this)
}

bool Layout::removeWidgetRecursively(LayoutItem *li, Widget *w)
{  
  if (!li->isLayout())
    return false;

  Layout *lay = static_cast<Layout*>(li);

  FOR_EACH_ITEM(iitem, 
  {
    if (iitem == w) {
      delete lay->takeAt(i);
      lay->invalidateLayout();
      return true;
    } else if (removeWidgetRecursively(iitem, w)) {
      return true;
    } 
  }, lay)

  return false;
}

void Layout::reparentChildWidgets(Widget* mw)
{
  FOR_EACH_ITEM(iitem, 
  {
    if(iitem->isWidget()) {
      Widget* w = static_cast<Widget*>(iitem);
      if (w->getParent() != mw)
        w->setParent(mw);
    } else if (iitem->isLayout()) {
      static_cast<Layout*>(iitem)->reparentChildWidgets(mw);
    }
  }, this)
}

void Layout::add(Widget* w) {
  Widget *mw = getParentWidget();
  Widget *pw = w->getParent();

  if(pw && pw->getLayout()) {
    if(removeWidgetRecursively(pw->getLayout(), w)) {
      //WARNING: removed from excisting layout
    }
  }

  if (pw && mw && pw != mw) {
    w->setParent(mw);
  } else if(!pw && mw) {
    w->setParent(mw);
  }

  add((LayoutItem*)w);
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


void Layout::update() {

}

bool Layout::activate() {
  return false;
}



} // Fog namespace
