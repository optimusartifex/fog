// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/LayoutItem.h>
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Gui/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::LayoutItem)

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

static IntSize calculateMinimumSizeHelper(const IntSize& sizeHint, const IntSize& minSizeHint, const IntSize& minSize, const IntSize& maxSize, const LayoutPolicy& sizePolicy)
{
  IntSize s(0, 0);

  if (sizePolicy.isHorizontalPolicyIgnored())
  {
    if (sizePolicy.getHorizontalPolicy() & LAYOUT_SHRINKING_WIDTH)
      s.setWidth(minSizeHint.getWidth());
    else
      s.setWidth(Math::max(sizeHint.getWidth(), minSizeHint.getWidth()));
  }

  if (sizePolicy.isVerticalPolicyIgnored())
  {
    if (sizePolicy.getVerticalPolicy() & LAYOUT_SHRINKING_HEIGHT)
      s.setHeight(minSizeHint.getHeight());
    else
      s.setHeight(Math::max(sizeHint.getHeight(), minSizeHint.getHeight()));
  }

  s = s.boundedTo(maxSize);
  if (minSize.getWidth() > 0)
    s.setWidth(minSize.getWidth());
  if (minSize.getHeight() > 0)
    s.setHeight(minSize.getHeight());

  return s.expandedTo(IntSize(0, 0));
}

static IntSize calculateMaximumSizeHelper(const IntSize& sizeHint, const IntSize& minSize, const IntSize& maxSize, const LayoutPolicy& sizePolicy, uint32_t align)
{
  if (align & ALIGNMENT_HORIZONTAL_MASK && align & ALIGNMENT_VERTICAL_MASK)
    return IntSize(WIDGET_MAX_SIZE, WIDGET_MAX_SIZE);

  IntSize s = maxSize;
  IntSize hint = sizeHint.expandedTo(minSize);

  if (s.getWidth() == WIDGET_MAX_SIZE && !(align & ALIGNMENT_HORIZONTAL_MASK))
    if (!(sizePolicy.getPolicy() & LAYOUT_GROWING_WIDTH))
      s.setWidth(hint.getWidth());

  if (s.getHeight() == WIDGET_MAX_SIZE && !(align & ALIGNMENT_VERTICAL_MASK))
    if (!(sizePolicy.getPolicy() & LAYOUT_GROWING_WIDTH))
      s.setHeight(hint.getHeight());

  if (align & ALIGNMENT_HORIZONTAL_MASK)
    s.setWidth(WIDGET_MAX_SIZE);
  if (align & ALIGNMENT_VERTICAL_MASK)
    s.setHeight(WIDGET_MAX_SIZE);

  return s;
}

// ============================================================================
// [Fog::LayoutItem]
// ============================================================================

LayoutItem::LayoutItem(uint32_t alignment) : 
  _alignment(alignment),
  _withinLayout(0),
  _layoutdata(0),
  _dirty(1),
  _propertydirty(1),
  _contentmargin(0)
{
}

LayoutItem::~LayoutItem() 
{
}

int LayoutItem::calcMargin(int margin, uint32_t location) const
{
  return margin;
}

#define MARGIN_CHANGED() \
  _dirty = 1; \
  updateLayout();

void LayoutItem::setContentMargin(const Margin& m)
{
  setContentMargin(m.top, m.right, m.bottom, m.left);
}

void LayoutItem::setContentMargin(int top, int right, int bottom, int left)
{
  Margin newMargin(
    calcMargin(top, MARGIN_TOP),
    calcMargin(right, MARGIN_RIGHT),
    calcMargin(bottom, MARGIN_BOTTOM),
    calcMargin(left, MARGIN_LEFT));

  if (_contentmargin == newMargin) return;
  MARGIN_CHANGED()
}

void LayoutItem::setContentTopMargin(int m)
{
  int newMargin = calcMargin(m, MARGIN_TOP);

  if (_contentmargin.getTop() == newMargin) return;
  MARGIN_CHANGED()
}

void LayoutItem::setContentRightMargin(int m)
{
  int newMargin = calcMargin(m, MARGIN_RIGHT);

  if (_contentmargin.getRight() == newMargin) return;
  MARGIN_CHANGED()
}

void LayoutItem::setContentBottomMargin(int m)
{
  int newMargin = calcMargin(m, MARGIN_BOTTOM);

  if (_contentmargin.getBottom() == newMargin) return;
  MARGIN_CHANGED()
}

void LayoutItem::setContentLeftMargin(int m)
{
  int newMargin = calcMargin(m, MARGIN_LEFT);

  if (_contentmargin.getLeft() == newMargin) return;
  MARGIN_CHANGED()
}

#undef MARGIN_CHANGED

bool LayoutItem::hasLayoutHeightForWidth() const
{
  return false;
}

int LayoutItem::getLayoutHeightForWidth(int width) const
{
  return -1;
}

int LayoutItem::getLayoutMinimumHeightForWidth(int width) const
{
  return getLayoutHeightForWidth(width);
}

void LayoutItem::updateLayout()
{
  if (_withinLayout) _withinLayout->updateLayout();
}

void LayoutItem::clearDirty()
{
  FOG_ASSERT(_dirty);

  if (!isEmpty())
  {
    calculateLayoutHint(_cache);
    _propertydirty = 0;
  }
  else
  {
    _cache._maximumSize = _cache._minimumSize = _cache._sizeHint = IntSize(0,0);
  }

  _dirty = 0;
}

const LayoutHint& LayoutItem::getLayoutHint() const
{
  if (_dirty)
  {
    const_cast<LayoutItem*>(this)->clearDirty();
  }

  if (isLayout())
  {
    int i = 100;
  }

  return _cache;
}

IntSize LayoutItem::calculateMaximumSize() const 
{
  FOG_ASSERT(this->isWidget());
  const Widget *w = (Widget *)this;
  return calculateMaximumSizeHelper(w->getSizeHint().expandedTo(w->getMinimumSizeHint()), w->getMinimumSize(), w->getMaximumSize(), w->getLayoutPolicy(), w->getLayoutAlignment());
}

IntSize LayoutItem::calculateMinimumSize() const
{
  FOG_ASSERT(this->isWidget());
  const Widget *w = (Widget *)this;
  return calculateMinimumSizeHelper(w->getSizeHint(), w->getMinimumSizeHint(),w->getMinimumSize(), w->getMaximumSize(),w->getLayoutPolicy());
}

void LayoutItem::removeLayoutStruct()
{
  if (_layoutdata)
  {
    delete _layoutdata;
    _layoutdata = NULL;
  }
}

} // Fog namespace
