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

FOG_IMPLEMENT_OBJECT(Fog::LayoutItem)

namespace Fog {

// ============================================================================
// [Fog::LayoutItem]
// ============================================================================

LayoutItem::LayoutItem()
{
}

LayoutItem::~LayoutItem() 
{
}

IntSize LayoutItem::getSizeHint() const
{
  return getLayoutHint().getSizeHint();
}

IntSize LayoutItem::getMinimumSize() const
{
  return getLayoutHint().getMinimumSize();
}

IntSize LayoutItem::getMaximumSize() const
{
  return getLayoutHint().getMaximumSize();
}

void LayoutItem::setSizeHint(const IntSize& sizeHint)
{
  LayoutHint hint = getLayoutHint();
  hint.setSizeHint(sizeHint);

  setLayoutHint(hint);
}

void LayoutItem::setMinimumSize(const IntSize& minimumSize)
{
  LayoutHint hint = getLayoutHint();
  hint.setMinimumSize(minimumSize);

  setLayoutHint(hint);
}

void LayoutItem::setMaximumSize(const IntSize& maximumSize)
{
  LayoutHint hint = getLayoutHint();
  hint.setMaximumSize(maximumSize);

  setLayoutHint(hint);
}

void LayoutItem::onLayout(LayoutEvent* e)
{
}

} // Fog namespace
