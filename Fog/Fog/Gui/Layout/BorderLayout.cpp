// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/BorderLayout.h>
#include <Fog/Gui/Layout/LayoutUtil.h>
#include <Fog/Gui/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::BorderLayout)

namespace Fog {

// ============================================================================
// [Fog::BorderLayout]
// ============================================================================

BorderLayout::BorderLayout(Widget* parent) :
  Layout(parent),
  _center(0),
  _sort(0),
  _horizontalflex(0),
  _verticalflex(0),
  _sortdirty(0)
{
}

BorderLayout::~BorderLayout()
{
}

void BorderLayout::addItem(LayoutItem *item, Edge edge, int flex)
{
  if (Layout::addChild(item) == -1) return;

  item->_layoutdata = new(std::nothrow) LayoutData(this);
  LayoutData* prop = item->getLayoutData<LayoutData>();
  prop->_edge = edge;
  prop->_user.setFlex(flex);

  if (edge & Y_MASK)
  {
    _y.append(item);        
  }
  else if (edge & X_MASK)
  {
    _x.append(item);
  }
  else
  {
    FOG_ASSERT(!_center);
    _center = item;
    return;
  }

  if (_sort != SORTNONE && item != _center)
    _sortdirty = 1;
}

void BorderLayout::onRemove(LayoutItem* item)
{
  if (item != _center)
  {
    uint32_t edge = item->getLayoutData<LayoutData>()->_edge;

    if (edge & Y_MASK) {
      _y.remove(item);
    } else if (edge & X_MASK) {
      _x.remove(item);
    }

    if (_sort != SORTNONE)
      _sortdirty = 1;
  }
  else
  {
    _center = 0;
  }
}

void BorderLayout::calculateLayoutHint(LayoutHint& hint)
{
  int widthX=0, minWidthX=0;
  int heightX=0, minHeightX=0;
  int widthY=0, minWidthY=0;
  int heightY=0, minHeightY=0;

  int spacingX= getSpacing(), spacingY=getSpacing();
  int spacingSumX=-spacingX, spacingSumY=-spacingY;

  const List<LayoutItem*>& list = getList();

  _allocatedHeight = _allocatedWidth = 0;

  _horizontalflex = 0;
  _verticalflex = 0;

  for (int i = 0; i < (sysint_t)list.getLength(); i++)
  {
    LayoutItem* item = list.at(i);
    const LayoutHint& hint = item->getLayoutHint();
    LayoutData* prop = item->getLayoutData<LayoutData>();

    int marginX = item->getContentLeftMargin() + item->getContentRightMargin();
    int marginY = item->getContentTopMargin() + item->getContentBottomMargin();

    if (prop->_edge & X_MASK)
    {
      widthY = Math::max(widthY, hint.getSizeHint().getWidth() + widthX + marginX);
      minWidthY = Math::max(minWidthY,hint.getMinimumSize().getWidth() + minWidthX + marginX);

      // Add the needed heights of this widget
      heightY += hint.getSizeHint().getHeight() + marginY;
      minHeightY += hint.getMinimumSize().getHeight() + marginY;

      // Add spacing
      spacingSumY += spacingY;

      //Allocated height
      _allocatedHeight += hint.getSizeHint().getHeight() + marginY + spacingY;

      if (prop->hasFlex())
      {
        LayoutData* prop = item->getLayoutData<LayoutData>();
        prop->_flex = (float)prop->getFlex();
        prop->_hint = hint.getSizeHint().getWidth();
        prop->_min = hint.getMinimumSize().getWidth();
        prop->_max = hint.getMaximumSize().getWidth();

        prop->_next = _horizontalflex;
        _horizontalflex = item;
      }
    }
    else if (prop->_edge & Y_MASK)
    {
      heightX = Math::max(heightX, hint.getSizeHint().getHeight() + heightY + marginY);
      minHeightX = Math::max(minHeightX, hint.getMinimumSize().getHeight() + minHeightY + marginY);

      // Add the needed widths of this widget
      widthX += hint.getSizeHint().getWidth() + marginX;
      minWidthX += hint.getMinimumSize().getWidth() + marginX;

      // Add spacing
      spacingSumX += spacingX;

      //Allocated width
      _allocatedWidth += hint.getSizeHint().getWidth() + marginX + spacingX;

      if (prop->hasFlex())
      {
        LayoutData* prop = item->getLayoutData<LayoutData>();
        prop->_flex = (float)prop->getFlex();
        prop->_hint = hint.getSizeHint().getHeight();
        prop->_min = hint.getMinimumSize().getHeight();
        prop->_max = hint.getMaximumSize().getHeight();          

        prop->_next = _verticalflex;
        _verticalflex = item;
      }
    }
    else
    {
      FOG_ASSERT(item == _center);
      // A centered widget must be added to both sums as
      // it stretches into the remaining available space.
      widthX += hint.getSizeHint().getWidth() + marginX;
      minWidthX += hint.getMinimumSize().getWidth() + marginX;

      heightY += hint.getSizeHint().getHeight() + marginY;
      minHeightY += hint.getMinimumSize().getHeight() + marginY;

      // Add spacing
      spacingSumX += spacingX;
      spacingSumY += spacingY;

      //Allocated height / Allocated width
      _allocatedHeight += hint.getSizeHint().getHeight() + marginY + spacingY;
      _allocatedWidth += hint.getSizeHint().getWidth() + marginX + spacingX;

      LayoutData* prop = item->getLayoutData<LayoutData>();
      prop->setFlex(prop->getFlex() < 1 ? 1:prop->getFlex()); //center has min flex value of 1!
    }
  }

  // Set margins to LayoutMargins.
  int marginX = getContentLeftMargin() + getContentRightMargin();
  int marginY = getContentTopMargin() + getContentBottomMargin();

  int minWidth = Math::max(minWidthX, minWidthY) + spacingSumX + marginX;
  int width = Math::max(widthX, widthY) + spacingSumX + marginX;
  int minHeight = Math::max(minHeightX, minHeightY) + spacingSumY + marginY;
  int height = Math::max(heightX, heightY) + spacingSumY + marginY;

  hint._minimumSize.set(minWidth, minHeight);
  hint._sizeHint.set(width, height);    
}

#define CLEAN(VAR, MIN, MAX) VAR = VAR < MIN ? MIN : ( VAR > MAX ? MAX : VAR)

void BorderLayout::setLayoutGeometry(const IntRect& rect)
{
  int availWidth = rect.getWidth();
  int availHeight = rect.getHeight();
  
  // Do not allow to overlap.
  if (availHeight < getLayoutSizeHint().getHeight())
  {
    availHeight = getLayoutSizeHint().getHeight();
  }

  // Do not allow to overlap.
  if (availWidth < getLayoutSizeHint().getWidth())
  {
    availWidth = getLayoutSizeHint().getWidth();
  }

  int allocatedHeight = _allocatedHeight;
  int allocatedWidth = _allocatedWidth;

  const List<LayoutItem*>& list = getList();

  calculateHorizonzalFlexOffsets(availWidth, allocatedWidth);
  calculateVerticalFlexOffsets(availHeight, allocatedHeight);

  int curTop = rect.y + getContentTopMargin();
  int curLeft = rect.x + getContentLeftMargin();

  // TODO: add Support XY-Spacings!
  int spacingX = getSpacing();
  int spacingY = getSpacing();

  for (int i = 0; i < (sysint_t)list.getLength(); i++)
  {
    LayoutItem* item = list.at(i);
    if (item == _center)
    {
      //Center is also within _children of Layout, but we handle
      //it differently
      continue;
    }
    
    const LayoutHint& hint = item->getLayoutHint();
    LayoutData* prop = item->getLayoutData<LayoutData>();

    int height = item->getLayoutSizeHint().getHeight();
    int width = item->getLayoutSizeHint().getWidth();
    int left = curLeft + item->getContentLeftMargin();
    int top = curTop + item->getContentTopMargin();

    int marginY = item->getContentYMargins();
    int marginX = item->getContentXMargins();

    if (prop->_edge & Y_MASK)
    {
      //NORTH or SOUTH
      width = availWidth - marginX;
      CLEAN(width, hint.getMinimumSize().getWidth(), hint.getMaximumSize().getWidth());        

      if (prop->_offset != 0)
      {
        height += prop->_offset;
        prop->_offset = 0;
      }

      // Update available height
      int used = height + marginY + spacingY;

      // Update coordinates, for next child
      if (prop->_edge == NORTH)
      {
        curTop += used;
      }
      else
      {
        top = curTop + (availHeight - height - item->getContentBottomMargin());
      }

      availHeight -= used;
    }
    else if (prop->_edge & X_MASK)
    {
      height = availHeight - marginY;
      CLEAN(height, hint.getMinimumSize().getHeight(), hint.getMaximumSize().getHeight()); 

      if (prop->_offset != 0) {
        width += prop->_offset;
        prop->_offset = 0;
      }

      // Update available width
      int used = width + marginX + spacingX;        

      // Update coordinates, for next child
      if (prop->_edge == WEST)
      {
        curLeft += used;
      }
      else
      {
        left = curLeft + (availWidth - width - item->getContentRightMargin());
      }
      availWidth -= used;
    }

    item->setLayoutGeometry(IntRect(left,top,width,height));
  }

  if (_center)
  {
    int width = availWidth - _center->getContentXMargins();
    int height = availHeight - _center->getContentYMargins();
    CLEAN(width, _center->getLayoutMinimumSize().getWidth(), _center->getLayoutMaximumSize().getWidth());
    CLEAN(height, _center->getLayoutMinimumSize().getHeight(), _center->getLayoutMaximumSize().getHeight());

    _center->setLayoutGeometry(IntRect(curLeft + _center->getContentLeftMargin(),curTop + _center->getContentTopMargin(),width,height));
  }
}

void BorderLayout::setSort(SortType s)
{
  if (_sort != s)
  {
    _sort = s;
    _sortdirty = 1;
  }

  invalidateLayout();
}

const List<LayoutItem*>& BorderLayout::getList()
{
  if (_sort == 0) {
    return _children;
  }
  else
  {
    if (_sortdirty)
    {
      _sorted.clear();
      _sorted.reserve(_x.getLength() + _y.getLength());

      if (_sort == SORTX)
      {
        _sorted.append(_x);
        _sorted.append(_y);
      }
      else if (_sort == SORTY)
      {
        _sorted.append(_y);
        _sorted.append(_x);
      }
    }

    return _sorted;
  }
}

void BorderLayout::calculateVerticalFlexOffsets(int availHeight, int& allocatedHeight)
{
  if (_verticalflex && allocatedHeight != availHeight)
  {
    if (_center)
    {
      LayoutData* prop = _center->getLayoutData<LayoutData>();
      prop->_next = _verticalflex;
      _verticalflex = _center;

      if (prop->getFlex() == -1)
      {
        prop->setFlex(1);
      }

      prop->_hint = _center->getLayoutSizeHint().getHeight();
      prop->_min = _center->getLayoutMinimumSize().getHeight();
      prop->_max = _center->getLayoutMaximumSize().getHeight();
      prop->_flex = (float)prop->getFlex();
    }

    LayoutUtil::calculateFlexOffsets(_verticalflex, availHeight, allocatedHeight);

    //clean it for next run!
    if (_center)
    {
      LayoutData* prop = _center->getLayoutData<LayoutData>();
      _verticalflex = (LayoutItem*)prop->_next;
    }
  }
}

void BorderLayout::calculateHorizonzalFlexOffsets(int availWidth, int& allocatedWidth)
{
  if (_horizontalflex && allocatedWidth != availWidth)
  {
    if (_center)
    {
      LayoutData* prop = _center->getLayoutData<LayoutData>();
      prop->_next = _horizontalflex;
      _horizontalflex = _center;

      if (prop->getFlex() == -1)
      {
        prop->setFlex(1);
      }

      prop->_hint = _center->getLayoutSizeHint().getWidth();
      prop->_min = _center->getLayoutMinimumSize().getWidth();
      prop->_max = _center->getLayoutMaximumSize().getWidth();
      prop->_flex = (float)prop->getFlex();
    }

    LayoutUtil::calculateFlexOffsets(_horizontalflex, availWidth, allocatedWidth);

    // Clean it for next run!
    if (_center)
    {
      FlexLayoutData* prop = _center->getLayoutData<LayoutData>();
      _horizontalflex = (LayoutItem*)prop->_next;
    }
  }
}

} // Fog namespace
