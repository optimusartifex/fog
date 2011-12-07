// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/UI/Layout/BoxLayout.h>
#include <Fog/UI/Layout/LayoutUtil.h>
#include <Fog/UI/Widget/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::BoxLayout)
FOG_IMPLEMENT_OBJECT(Fog::HBoxLayout)
FOG_IMPLEMENT_OBJECT(Fog::VBoxLayout)

namespace Fog {

// ============================================================================
// [Fog::BoxLayout]
// ============================================================================

BoxLayout::BoxLayout(Widget *parent, int margin, int spacing) :
  Layout(parent),
  _direction(LAYOUT_DIRECTION_LEFT_TO_RIGHT),
  _flexibles(0),
  _allocated(0)
{
  setSpacing(spacing);
  setContentMargin(margin, margin, margin, margin);
}

BoxLayout::BoxLayout(int margin, int spacing) :
  _direction(LAYOUT_DIRECTION_LEFT_TO_RIGHT),
  _flexibles(0),
  _allocated(0)
{
  setSpacing(spacing);
  setContentMargin(margin, margin, margin, margin);
}

BoxLayout::~BoxLayout()
{
}

void BoxLayout::addItem(LayoutItem *item, int flex)
{
  if (Layout::addChild(item) == -1)
  {
    return;
  }
  LayoutData* data = fog_new LayoutData(this);
  data->_user.setFlex(flex);

  item->_layoutdata = data;
}

uint32_t BoxLayout::getLayoutExpandingDirections() const
{
  return ORIENTATION_HORIZONTAL;
}

void BoxLayout::setLayoutGeometry(const RectI &rect)
{
  // LAYOUT TODO: check oldrect and newrect if we really need to reposition.
  Layout::setLayoutGeometry(rect);
  doLayout(rect);
}

int BoxLayout::calculateHorizontalGaps(bool collapse)
{
  int spacing = getSpacing();
  if (spacing < 0) spacing = 0;

  int gaps = 0;

  if (collapse)
  {
    for (int i = 1; i < (ssize_t)_children.getLength(); i++)
    {
      gaps += LayoutUtil::collapseMargins(spacing, _children.getAt(i-1)->getContentRightMargin(), _children.getAt(i)->getContentLeftMargin());
    }

    // Add last child.
    gaps += _children.getAt(0)->getContentLeftMargin() + _children.getAt(_children.getLength()-1)->getContentRightMargin();
  }
  else
  {
    // Simple adding of all margins.
    for (int i = 1; i < (ssize_t)_children.getLength(); i++)
    {
      gaps += _children.getAt(i)->getContentLeftMargin() + _children.getAt(i)->getContentRightMargin();
    }

    // Add spacing.
    gaps += (spacing * (int)(_children.getLength() - 1));
  }

  return gaps;
}


int BoxLayout::calculateVerticalGaps(bool collapse)
{
  int spacing = getSpacing();
  if (spacing < 0) spacing = 0;
  int gaps = 0;

  if (collapse)
  {
    for (int i = 1; i < (ssize_t)_children.getLength(); i++)
    {
      gaps += LayoutUtil::collapseMargins(spacing, _children.getAt(i-1)->getContentBottomMargin(), _children.getAt(i)->getContentTopMargin());
    }
    gaps += _children.getAt(0)->getContentLeftMargin() + _children.getAt(_children.getLength()-1)->getContentBottomMargin();
  }
  else
  {
    for (int i = 1; i < (ssize_t)_children.getLength(); i++)
    {
      LayoutItem* item = _children.getAt(i);
      gaps += item->getContentTopMargin() + item->getContentBottomMargin();
    }
    gaps += (spacing * (int)(_children.getLength() - 1));
  }

  return gaps;
}

// ============================================================================
// [Fog::HBoxLayout]
// ============================================================================

HBoxLayout::HBoxLayout(Widget *parent, int margin, int spacing) :
  BoxLayout(parent, margin, spacing)
{
}

HBoxLayout::HBoxLayout(int margin, int spacing) :
  BoxLayout(margin, spacing)
{
}

HBoxLayout::~HBoxLayout()
{
}

void HBoxLayout::calculateLayoutHint(LayoutHint& hint)
{
  // Initialize.
  int minWidth = 0, width = 0;
  int minHeight = 0, height = 0;

  _allocated = 0;
  _flexibles = 0;

  // Iterate over children.
  for (int i = 0; i < getLength(); i++)
  {
    LayoutItem* item = getAt(i);
    SizeI hint = item->getLayoutSizeHint();
    SizeI min = item->getLayoutMinimumSize();
    LayoutData * data = static_cast<LayoutData*> (item->_layoutdata);

    // Sum up widths.
    width += hint.getWidth();

    // Detect if child is shrinkable or has percent width and update minWidth.
    minWidth += data->hasFlex() ? min.getWidth() : hint.getWidth();

    // Build vertical margin sum.
    int margin = item->getContentTopMargin() + item->getContentBottomMargin();

    // Find biggest height.
    if ((hint.getHeight()+margin) > height)
    {
      height = hint.getHeight() + margin;
    }

    // Find biggest minHeight.
    if ((min.getHeight()+margin) > minHeight)
    {
      minHeight = min.getHeight() + margin;
    }

    int whint = hint.getWidth();
    if (data->hasFlex())
    {
      data->_min = item->getLayoutMinimumSize().getWidth();
      data->_max = item->getLayoutMaximumSize().getWidth();
      data->_hint = whint;
      data->_flex = (float)data->getFlex();
      data->_offset = 0;

      data->_next = _flexibles;
      _flexibles = item;
    }

    _allocated += whint;
  }

  // Respect gaps.
  int gaps = (getContentLeftMargin() + calculateHorizontalGaps(true) + getContentRightMargin());

  hint._minimumSize.set(minWidth + gaps,minHeight+getContentTopMargin()+getContentBottomMargin());
  hint._sizeHint.set(width + gaps,height+getContentTopMargin()+getContentBottomMargin());
  hint._maximumSize.set(INT_MAX,INT_MAX);
}

int HBoxLayout::doLayout(const RectI &rect)
{
  if (!rect.isValid())
    return 0;

  int availWidth = rect.getWidth();
  int availHeight = rect.getHeight();

  // Support for Margin of Layout.
  int gaps = getContentLeftMargin() + calculateHorizontalGaps(true) + getContentRightMargin();
  int allocatedWidth = gaps + _allocated;

  if (_flexibles && allocatedWidth != availWidth)
  {
    LayoutUtil::calculateFlexOffsets(_flexibles, availWidth, allocatedWidth);
  }

  int top, height, marginTop, marginBottom;
  int marginRight = -INT_MAX;
  int spacing = getSpacing();

  // Render children and separators.
  bool forward = isForward();

  int i = -1;
  int len = getLength() - 1;
  int start = 0;

  if (!forward)
  {
    i = len + 1;
    len = 0;
    start = len;
  }

  int left = rect.x + LayoutUtil::collapseMargins(getContentLeftMargin(), getAt(start)->getContentLeftMargin());

  while (i != len)
  {
    if (forward)
      i++;
    else
      i--;

    LayoutItem* child = getAt(i);
    SizeI hint = child->getLayoutSizeHint();

    int width = hint.getWidth();
    width += ((LayoutItem::FlexLayoutData*)child->_layoutdata)->_offset;

    marginTop = getContentTopMargin() + child->getContentTopMargin();
    marginBottom = getContentBottomMargin() + child->getContentBottomMargin();

    // Find usable height.
    height = Math::max<int>(child->getLayoutMinimumSize().getHeight(), Math::min(availHeight-marginTop-marginBottom, child->getLayoutMaximumSize().getHeight()));

    // Respect vertical alignment.
    top = rect.y + marginTop;

    // Add collapsed margin.
    if (marginRight != -INT_MAX)
    {
      // Support margin collapsing.
      left += LayoutUtil::collapseMargins(spacing, marginRight, child->getContentLeftMargin());
    }

    // Layout child.
    child->setLayoutGeometry(RectI(left, top, width, height));

    // Add width.
    left += width;

    // Remember right margin (for collapsing).
    marginRight = child->getContentRightMargin();
  }

  return 0;
}

// ============================================================================
// [Fog::VBoxLayout]
// ============================================================================

VBoxLayout::VBoxLayout(Widget *parent, int margin, int spacing) :
  BoxLayout(parent, margin, spacing)
{
}

VBoxLayout::VBoxLayout(int margin, int spacing) :
  BoxLayout(margin, spacing)
{
}

VBoxLayout::~VBoxLayout()
{
}

void VBoxLayout::calculateLayoutHint(LayoutHint& hint)
{
  // Initialize
  int minWidth = 0, width = 0;
  int minHeight = 0, height = 0;

  _allocated = 0;
  _flexibles = 0;

  // Iterate over children.
  for (int i = 0; i < getLength(); i++)
  {
    LayoutItem* item = getAt(i);
    SizeI hint = item->getLayoutSizeHint();
    SizeI min = item->getLayoutMinimumSize();

    LayoutData * prop = static_cast<LayoutData*> (item->_layoutdata);

    // Sum up widths.
    height += hint.getHeight();

    // Detect if child is shrinkable or has percent width and update minWidth.
    if (prop->hasFlex())
    {
      minHeight += min.getHeight();
    }
    else
    {
      minHeight += hint.getHeight();
    }

    // Build vertical margin sum.
    int margin = item->getContentTopMargin() + item->getContentBottomMargin();

    // Find biggest height.
    if ((hint.getWidth()+margin) > width)
    {
      width = hint.getWidth() + width;
    }

    // Find biggest minHeight.
    if ((min.getWidth()+margin) > minWidth)
    {
      minWidth = min.getWidth() + margin;
    }

    int hinth = hint.getHeight();
    if (prop->hasFlex())
    {
      prop->_min = item->getLayoutMinimumSize().getHeight();
      prop->_max = item->getLayoutMaximumSize().getHeight();
      prop->_hint = hinth;
      prop->_flex = (float)prop->getFlex();
      prop->_offset = 0;

      prop->_next = _flexibles;
      _flexibles = item;
    }

    _allocated += hinth;
  }

  // Respect gaps.
  int gaps = (getContentLeftMargin() + calculateVerticalGaps(true) + getContentRightMargin());

  hint._minimumSize.set(minWidth+getContentLeftMargin()+getContentRightMargin(),minHeight+gaps);
  hint._sizeHint.set(width+getContentLeftMargin()+getContentRightMargin(),height+gaps);
  hint._maximumSize.set(INT_MAX,INT_MAX);
}

int VBoxLayout::doLayout(const RectI &rect)
{
  int availWidth = rect.getWidth();
  int availHeight = rect.getHeight();

  // Support for Margin of Layout.
  int gaps = getContentTopMargin() + calculateVerticalGaps(true) + getContentBottomMargin();
  int allocatedHeight = gaps + _allocated;

  if (_flexibles && allocatedHeight != availHeight)
  {
    LayoutUtil::calculateFlexOffsets(_flexibles, availHeight, allocatedHeight);
  }

  int left, height, marginLeft, marginRight;
  int marginBottom = -INT_MAX;
  int spacing = getSpacing();

  // Render children and separators.
  bool forward = isForward();

  int i = -1;
  int len = getLength() - 1;
  int start = 0;

  if (!forward)
  {
    i = len + 1;
    len = 0;
    start = len;
  }

  int top = rect.y + LayoutUtil::collapseMargins(getContentTopMargin(), getAt(start)->getContentTopMargin());

  while (i != len)
  {
    if (forward)
      i++;
    else
      i--;

    LayoutItem* child = getAt(i);
    SizeI hint = child->getLayoutSizeHint();

    height = hint.getHeight();
    height += ((LayoutItem::FlexLayoutData*)child->_layoutdata)->_offset;

    marginLeft = getContentLeftMargin() + child->getContentLeftMargin();
    marginRight = getContentRightMargin() + child->getContentRightMargin();

    // Find usable width.
    int width = Math::max<int>(child->getLayoutMinimumSize().getWidth(), Math::min(availWidth-marginLeft-marginRight, child->getLayoutMaximumSize().getWidth()));

    // Respect vertical alignment.
    left = rect.x + marginLeft;

    // Add collapsed margin.
    if (marginBottom != -INT_MAX)
    {
      // Support margin collapsing.
      top += LayoutUtil::collapseMargins(spacing, marginBottom, child->getContentTopMargin());
    }

    // Layout child.
    child->setLayoutGeometry(RectI(left, top, width, height));

    // Add height.
    top += height;

    // Remember right margin (for collapsing).
    marginRight = child->getContentRightMargin();
  }

  return 0;
}

} // Fog namespaces
