// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_BORDERLAYOUT_H
#define _FOG_GUI_LAYOUT_BORDERLAYOUT_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Gui/Layout/Layout.h>

namespace Fog {

//! @addtogroup Fog_Gui_Layout
//! @{

// ============================================================================
// [Fog::BorderLayout]
// ============================================================================

//! @brief Base class for all layouts.
struct FOG_API BorderLayout : public Layout
{
  FOG_DECLARE_OBJECT(BorderLayout, Layout)

  BorderLayout(Widget* parent=0);
  virtual ~BorderLayout();

  struct LayoutProperty
  {
    LayoutProperty(Layout* layout) : _layout(layout)
    {
      FOG_INIT_FLEX_PROPERTY();
      FOG_INIT_PERCENT_SIZE_PROPERTY();
    }

    FOG_DECLARE_FLEX_PROPERTY()
    FOG_DECLARE_PERCENT_SIZE_PROPERTY()

    Layout* _layout;
  };

  struct LayoutData : public LayoutItem::FlexLayoutData
  {
    LayoutData(Layout* layout) : _user(layout) {}

    FOG_INLINE bool hasFlex() const { return _user.hasFlex(); }
    FOG_INLINE int getFlex() const { return _user.getFlex(); }
    FOG_INLINE void setFlex(int flex) { return _user.setFlex(flex); }

    uint32_t _edge : 8;
    uint32_t _unused : 24;

    LayoutProperty _user;
  };

  typedef LayoutProperty PropertyType;

  void addItem(LayoutItem *item, uint32_t edge, int flex = -1);

  virtual void onRemove(LayoutItem* item);

  virtual void calculateLayoutHint(LayoutHint& hint);
  virtual void setLayoutGeometry(const RectI&);

  void setSort(uint32_t s);
  const List<LayoutItem*>& getList();

  FOG_INLINE bool isDirty() { return _dirty || _sortdirty; }

  void calculateVerticalFlexOffsets(int availHeight, int& allocatedHeight);
  void calculateHorizonzalFlexOffsets(int availWidth, int& allocatedWidth);

  List<LayoutItem*> _x;
  List<LayoutItem*> _y;
  List<LayoutItem*> _sorted;
  LayoutItem* _center;

  LayoutItem* _horizontalflex;
  LayoutItem* _verticalflex;

  int _sortdirty : 1;
  int _sort : 2;
  int _unused : 1;

  int _allocatedWidth;
  int _allocatedHeight;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_LAYOUT_BORDERLAYOUT_H
