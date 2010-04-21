// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/FlowLayout.h>
#include <Fog/Gui/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::FlowLayout)

namespace Fog {

  // ============================================================================
  // [Fog::FlowLayout]
  // ============================================================================

  FlowLayout::FlowLayout(Widget *parent, int margin, int hSpacing, int vSpacing)
    : Layout(parent), _hSpace(hSpacing), _vSpace(vSpacing)
  {
    setContentMargins(margin, margin, margin, margin);
  }

  FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing) : _hSpace(hSpacing), _vSpace(vSpacing)
  {
    setContentMargins(margin, margin, margin, margin);
  }

  FlowLayout::~FlowLayout()
  {
    LayoutItem *item;
    while ((item = takeAt(0)))
      delete item;
  }

  void FlowLayout::add(LayoutItem *item)
  {
    Layout::addChild(item);
  }

  int FlowLayout::horizontalSpacing() const
  {
    if (_hSpace >= 0) {
      return _hSpace;
    } else {
      return DEFAULT_LAYOUT_SPACING;
      //return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
  }

  int FlowLayout::verticalSpacing() const
  {
    if (_vSpace >= 0) {
      return _vSpace;
    } else {
      return DEFAULT_LAYOUT_SPACING;
      //return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
  }

  uint32_t FlowLayout::getLayoutExpandingDirections() const
  {
    return 0;
  }

  bool FlowLayout::hasLayoutHeightForWidth() const
  {
    return true;
  }

  int FlowLayout::getLayoutHeightForWidth(int width) const
  {
    int height = doLayout(IntRect(0, 0, width, 0), true);
    return height;
  }

  void FlowLayout::setLayoutGeometry(const IntRect &rect)
  {
    Layout::setLayoutGeometry(rect);
    doLayout(rect, false);
  }

  IntSize FlowLayout::getLayoutSizeHint() const
  {
    return getLayoutMinimumSize();
  }

  IntSize FlowLayout::getLayoutMinimumSize() const
  {
    IntSize size;
    int len = getLength();
    for(int i=0;i<len;++i) {
      size = size.expandedTo(getAt(i)->getLayoutMinimumSize());
    }

    int l = 2* getContentMargins().left;

    size = size + IntSize(l, l);
    return size;
  }

  int FlowLayout::doLayout(const IntRect &rect, bool testOnly) const
  {    
    IntMargins margins = getContentMargins();
    IntRect effectiveRect = rect.adjusted(+margins.left, +margins.top, -margins.right, -margins.bottom);
    int x = effectiveRect.x;
    int y = effectiveRect.y;
    int lineHeight = 0;

    int len = getLength();
    for(int i=0;i<len;++i) {
      LayoutItem *item = getAt(i);
      int spaceX = horizontalSpacing();
      if (spaceX == -1)
        spaceX = 0;
      int spaceY = verticalSpacing();
      if (spaceY == -1)
        spaceY = 0;

      int nextX = x + item->getLayoutSizeHint().getWidth() + spaceX;
      if (nextX - spaceX > effectiveRect.getRight() && lineHeight > 0) {
        x = effectiveRect.x;
        y = y + lineHeight + spaceY;
        nextX = x + item->getLayoutSizeHint().getWidth() + spaceX;
        lineHeight = 0;
      }

      if (!testOnly) 
      {
        IntRect rect;
        IntSize tmp = item->getLayoutSizeHint();
        rect.set(x,y,tmp.w,tmp.h);
        item->setLayoutGeometry(rect);
      }

      x = nextX;
      lineHeight = Math::max<int>(lineHeight, item->getLayoutSizeHint().getHeight());
    }
    return y + lineHeight - rect.y + margins.bottom;
  }
}