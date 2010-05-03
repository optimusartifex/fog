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
      
      NORTH = 8|Y_MASK,
      SOUTH = 16|Y_MASK,
      WEST = 32|X_MASK,
      EAST = 64|X_MASK,
      CENTER = 128
    };

    BorderLayout(Widget* parent=0) : _center(0), Layout(parent), _sort(0), _horizontalflex(0), _verticalflex(0), _sortdirty(0) {
    
    }

    struct LayoutProperty {
      LayoutProperty(Layout* layout) : _layout(layout) {
        FLEXPROPERTYINIT();
        PERCENTPROPERTYINIT();
      }

      FLEXPROPERTY()
      PERCENTPROPERTY()

      Layout* _layout;
    };

    struct LayoutData : public LayoutItem::FlexLayoutData {
      LayoutData(Layout* layout) : _user(layout) {}
      FOG_INLINE bool hasFlex() const { return _user.hasFlex(); }
      FOG_INLINE int getFlex() const { return _user.getFlex(); }
      FOG_INLINE void setFlex(int flex) { return _user.setFlex(flex); }

      uint32_t _edge : 8;
      uint32_t _unused : 24;

      LayoutProperty _user;
    };

    typedef LayoutProperty PropertyType;

    virtual void onRemove(LayoutItem* item) {
      if(item != _center) {
        uint32_t edge = item->getLayoutData<LayoutData>()->_edge;

        if(edge & Y_MASK) {
          _y.remove(item);
        } else if(edge & X_MASK) {
          _x.remove(item);
        }

        if(_sort != SORTNONE)
          _sortdirty = 1;
      } else {
        _center = 0;
      }
    }

    FOG_INLINE void addItem(LayoutItem *item, Edge edge, int flex=-1) {
      if(Layout::addChild(item) == -1) {
        return;
      }

      item->_layoutdata = new(std::nothrow) LayoutData(this);
      LayoutData* prop = item->getLayoutData<LayoutData>();
      prop->_edge = edge;
      prop->_user.setFlex(flex);

      if(edge & Y_MASK) {
        _y.append(item);        
      } else if(edge & X_MASK) {
        _x.append(item);
      } else {
        FOG_ASSERT(!_center);
        _center = item;
        return;
      }

      if(_sort != SORTNONE && item != _center)
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

    void calculateVerticalFlexOffsets(int availHeight, int& allocatedHeight);
    void calculateHorizonzalFlexOffsets(int availWidth, int& allocatedWidth);

    List<LayoutItem*> _x;
    List<LayoutItem*> _y;
    List<LayoutItem*> _sorted;
    LayoutItem* _center;

    LayoutItem* _horizontalflex;
    LayoutItem* _verticalflex;

    int _sortdirty: 1;
    int _sort:2;
    int _unused: 1;

    int _allocatedWidth;
    int _allocatedHeight;
  };
}
#endif