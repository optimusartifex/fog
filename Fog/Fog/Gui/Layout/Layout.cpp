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

FOG_IMPLEMENT_OBJECT(Fog::Layout)

namespace Fog {

// ============================================================================
// [Fog::Layout]
// ============================================================================

Layout::Layout() : 
  _parentItem(NULL),
  _layoutHint(
    IntSize(WIDGET_MIN_SIZE, WIDGET_MIN_SIZE),
    IntSize(WIDGET_MIN_SIZE, WIDGET_MIN_SIZE),
    IntSize(WIDGET_MAX_SIZE, WIDGET_MAX_SIZE)),
  _isLayoutDirty(true)
{
  _flags |= OBJ_IS_LAYOUT;
}

Layout::~Layout() 
{
}

const LayoutHint& Layout::getLayoutHint() const
{
  return _layoutHint;
}

void Layout::setLayoutHint(const LayoutHint& layoutHint)
{
  if (_layoutHint == layoutHint) return;

  _layoutHint = layoutHint;
  invalidateLayout();
}

const LayoutHint& Layout::getComputedLayoutHint() const
{
  // GUI TODO:
  return _layoutHint;
}

void Layout::computeLayoutHint()
{
  // GUI TODO:
}

uint32_t Layout::getLayoutPolicy() const
{
  return 0;
}

void Layout::setLayoutPolicy(uint32_t policy)
{
  // GUI TODO:
}

bool Layout::hasHeightForWidth() const
{
  return false;
}

int Layout::getHeightForWidth(int width) const
{
  return -1;
}

bool Layout::isLayoutDirty() const
{
  return false;
}

void Layout::invalidateLayout() const
{
}

LayoutItem* Layout::getParentItem() const
{
  return _parentItem;
}

Widget* Layout::getParentWidget() const
{
  LayoutItem* item = _parentItem;

  do {
    if (item->isWidget()) return reinterpret_cast<Widget*>(item);
    item = ((Layout *)item)->_parentItem;
  } while (item);

  return NULL;
}

} // Fog namespace
