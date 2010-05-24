// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_BORDERLAYOUT_H
#define _FOG_GUI_BORDERLAYOUT_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Gui/Layout.h>

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

  // LAYOUT TODO: Move to Constants.h, LAYOUT_EDGE?
  enum Edge {
    X_MASK = 1,
    Y_MASK = 2,     
    
    NORTH = 8 | Y_MASK,
    SOUTH = 16 | Y_MASK,
    WEST = 32 | X_MASK,
    EAST = 64 | X_MASK,
    CENTER = 128
  };

  // LAYOUT TODO: Move to Constants.h, LAYOUT_SORT?
  enum SortType
  {
    SORTNONE = 0,
    SORTX = 1,
    SORTY = 2
  };

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

  void addItem(LayoutItem *item, Edge edge, int flex = -1);

  virtual void onRemove(LayoutItem* item);

  virtual void calculateLayoutHint(LayoutHint& hint);
  virtual void setLayoutGeometry(const IntRect&);

  void setSort(SortType s);
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
#endif // _FOG_GUI_BORDERLAYOUT_H
