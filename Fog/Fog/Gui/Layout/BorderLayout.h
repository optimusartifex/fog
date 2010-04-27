// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_BORDERLAYOUT_H
#define _FOG_GUI_LAYOUT_BORDERLAYOUT_H

// [Dependencies]
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Core/List.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

  // ============================================================================
  // [Fog::BorderLayout]
  // ============================================================================

  //! @brief Base class for all layouts.
  struct FOG_API BorderLayout : public Layout
  {
    FOG_DECLARE_OBJECT(BorderLayout, Layout)

    enum Edge {
      X_MASK = 1,
      Y_MASK = 2,
      X_FLEX = 4,
      Y_FLEX = 8,      
      
      NORTH = 128|Y_MASK|Y_FLEX,
      SOUTH = 256|Y_MASK|Y_FLEX,
      WEST = 512|X_MASK|X_FLEX,
      EAST = 1024|X_MASK|X_FLEX,
      CENTER = 2048|X_FLEX|Y_FLEX
    };

    BorderLayout(Widget* parent=0) : _center(0), Layout(parent), _sort(0) {
    
    }

    struct LayoutProperty : public Layout::LayoutStruct {
      uint32_t _edge;
    };

    FOG_INLINE virtual void prepareItem(LayoutItem* item, sysuint_t index) {
      item->_layoutdata = new(std::nothrow) LayoutProperty();
    }

    FOG_INLINE void addItem(LayoutItem *item, Edge type) {
      Layout::addChild(item);      
      if(type == NORTH || type == SOUTH) {
        _y.append(item);
      } else if(type == EAST || type == WEST) {
        _x.append(item);
      } else {
        FOG_ASSERT(!_center);
        _center = item;
      }

      LayoutProperty* prop = static_cast<LayoutProperty*>(item->_layoutdata);
      prop->_edge = type;

      if(_sort != SORTNONE)
        _sortdirty = 1;
    }

    virtual void calculateLayoutHint(LayoutHint& hint);
    virtual void setLayoutGeometry(const IntRect&);

    enum SortType {SORTNONE=0, SORTX=1, SORTY=2};

    void setSort(SortType s) {
      if(_sort != s) {
        _sort = s;
        _sortdirty = 1;
      }

      invalidateLayout();
    }

    const List<LayoutItem*>& getList() {
      if(_sort == 0) {
        return _children;
      } else {
        if(_sortdirty) {
          _sorted.clear();
          _sorted.reserve(_x.getLength() + _y.getLength());
          if(_sort == SORTX) {
            _sorted.append(_x);
            _sorted.append(_y);
          } else if(_sort == SORTY) {
            _sorted.append(_y);
            _sorted.append(_x);
          }
        }

        return _sorted;
      }
    }

    FOG_INLINE bool isDirty() { return _dirty || _sortdirty; }

    List<LayoutItem*> _x;
    List<LayoutItem*> _y;
    List<LayoutItem*> _sorted;
    LayoutItem* _center;

    int _sortdirty: 1;
    int _sort:2;    
    int _unused: 1;

    int _allocatedWidth;
    int _allocatedHeight;
  };
}
#endif