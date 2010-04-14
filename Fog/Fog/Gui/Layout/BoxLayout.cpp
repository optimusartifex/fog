// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

#if 0
// [Dependencies]
#include <Fog/Gui/Layout/BoxLayout.h>
#include <Fog/Gui/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::BoxLayout)

namespace Fog {

// ============================================================================
// [Fog::BoxLayout]
// ============================================================================

BoxLayout::BoxLayout()
{
}

BoxLayout::~BoxLayout() 
{
}

void BoxLayout::reparentChildren()
{
  Widget* parent = getParentWidget();
  if (!parent) return;

  List<LayoutItem*>::ConstIterator it(_items);
  for (it.toStart(); it.isValid(); it.toNext())
  {
    Widget* w = fog_object_cast<Widget*>(it.value());
    if (w) w->setParent(parent);
  }

  invalidateLayout();
}

sysint_t BoxLayout::indexOf(LayoutItem* item)
{
  return (sysint_t)_items.indexOf(item);
}

bool BoxLayout::addItem(LayoutItem* item)
{
  // TODO: What about adding single item multiple times?
  _items.append(item);

  invalidateLayout();
  return true;
}

bool BoxLayout::addItemAt(sysint_t index, LayoutItem* item)
{
  // TODO: Is this correct?
  if ((sysuint_t)index > _items.getLength()) return false;
  _items.insert(index, item);

  invalidateLayout();
  return true;
}

LayoutItem* BoxLayout::takeAt(sysint_t index)
{
  if ((sysuint_t)index >= _items.getLength()) return NULL;
  LayoutItem* item = _items.take((sysuint_t)index);

  invalidateLayout();
  return item;
}

bool BoxLayout::deleteItem(LayoutItem* item)
{
  return deleteItemAt(indexOf(item));
}

bool BoxLayout::deleteItemAt(sysint_t index)
{
  LayoutItem* item = takeAt(index);
  if (!item) return false;

  item->destroy();
  return true;
}

List<LayoutItem*> BoxLayout::items() const
{
  return _items;
}

void BoxLayout::setMargin(int margin)
{
  if (_margin == margin) return;

  _margin = margin;
  invalidateLayout();
}

void BoxLayout::setSpacing(int spacing)
{
  if (_spacing == spacing) return;

  _spacing = spacing;
  invalidateLayout();
}

} // Fog namespace

#endif