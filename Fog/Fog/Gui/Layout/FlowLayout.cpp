// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/FlowLayout.h>
#include <Fog/Gui/Widget/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::FlowLayout)

namespace Fog {

// ============================================================================
// [Fog::FlowLayout]
// ============================================================================

FlowLayout::FlowLayout(Widget *parent, int margin, int hSpacing, int vSpacing)
  : Layout(parent), _hSpace(hSpacing), _vSpace(vSpacing)
{
  setContentMargin(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing) :
  _hSpace(hSpacing),
  _vSpace(vSpacing)
{
  setContentMargin(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
  LayoutItem *item;
  while ((item = takeAt(0)))
    fog_delete(item);
}

void FlowLayout::add(LayoutItem *item)
{
  Layout::addChild(item);
}

int FlowLayout::getHorizontalSpacing() const
{
  if (_hSpace >= 0)
  {
    return _hSpace;
  }
  else
  {
    return LAYOUT_DEFAULT_SPACING;
  }
}

int FlowLayout::getVerticalSpacing() const
{
  if (_vSpace >= 0)
  {
    return _vSpace;
  }
  else
  {
    return LAYOUT_DEFAULT_SPACING;
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
  int height = doLayout(RectI(0, 0, width, 0), true);
  return height;
}

void FlowLayout::setLayoutGeometry(const RectI &rect)
{
  Layout::setLayoutGeometry(rect);
  doLayout(rect, false);
}

SizeI FlowLayout::getLayoutSizeHint() const
{
  return getLayoutMinimumSize();
}

SizeI FlowLayout::getLayoutMinimumSize() const
{
  SizeI size;
  int len = getLength();

  for (int i = 0; i < len; i++)
  {
    size = size.expandedTo(getAt(i)->getLayoutMinimumSize());
  }

  int l = 2 * getContentMargin().left;

  size = size + SizeI(l, l);
  return size;
}

int FlowLayout::doLayout(const RectI &rect, bool testOnly) const
{
  Margin margins = getContentMargin();
  RectI effectiveRect = rect.adjusted(+margins.left, +margins.top, -margins.right, -margins.bottom);

  int x = effectiveRect.x;
  int y = effectiveRect.y;
  int lineHeight = 0;

  int len = getLength();

  for (int i = 0; i < len; i++)
  {
    LayoutItem *item = getAt(i);

    int spaceX = getHorizontalSpacing();
    if (spaceX == -1) spaceX = 0;

    int spaceY = getVerticalSpacing();
    if (spaceY == -1) spaceY = 0;

    int nextX = x + item->getLayoutSizeHint().getWidth() + spaceX;
    if (nextX - spaceX > effectiveRect.getRight() && lineHeight > 0)
    {
      x = effectiveRect.x;
      y = y + lineHeight + spaceY;
      nextX = x + item->getLayoutSizeHint().getWidth() + spaceX;
      lineHeight = 0;
    }

    if (!testOnly)
    {
      SizeI tmp(item->getLayoutSizeHint());
      RectI rect(x, y, tmp.w, tmp.h);
      item->setLayoutGeometry(rect);
    }

    x = nextX;
    lineHeight = Math::max<int>(lineHeight, item->getLayoutSizeHint().getHeight());
  }
  return y + lineHeight - rect.y + margins.bottom;
}

} // Fog namespace
