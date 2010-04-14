// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_BOXLAYOUT_H
#define _FOG_GUI_LAYOUT_BOXLAYOUT_H

#if 0
// [Dependencies]
#include <Fog/Gui/Layout/Layout.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Fog::BoxLayout]
// ============================================================================

//! @brief Base class for all layouts.
struct FOG_API BoxLayout : public Layout
{
  FOG_DECLARE_OBJECT(BoxLayout, Layout)

  // [Construction / Destruction]

  BoxLayout();
  virtual ~BoxLayout();

  virtual void reparentChildren();

  sysint_t indexOf(LayoutItem* item);

  bool addItem(LayoutItem* item);
  bool addItemAt(sysint_t index, LayoutItem* item);

  LayoutItem* takeAt(sysint_t index);

  bool deleteItem(LayoutItem* item);
  bool deleteItemAt(sysint_t index);

  List<LayoutItem*> items() const;

  FOG_INLINE int margin() const { return _margin; }
  FOG_INLINE int spacing() const { return _spacing; }

  void setMargin(int margin);
  void setSpacing(int spacing);

protected:
  List<LayoutItem*> _items;

  int _margin;
  int _spacing;
};

} // Fog namespace

//! @}
#endif
// [Guard]
#endif // _FOG_GUI_LAYOUT_BOXLAYOUT_H
