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

    enum BorderType {
      NORTH = 1,
      SOUTH = 2,
      WEST = 3,
      EAST = 4,
      CENTER = 5
    };

    BorderLayout(Widget* parent=0) : _center(0), Layout(parent) {
    
    }

    FOG_INLINE virtual void prepareItem(LayoutItem* item, sysuint_t index) {
      item->_layoutdata = new(std::nothrow) ItemProperties();
    }

    FOG_INLINE void addItem(LayoutItem *item, BorderType type) { 
      Layout::addChild(item); 
      if(item->_withinLayout == this) {
        if(type == CENTER) {
          FOG_ASSERT(!_center);
          _center = item;
        } else if(type == NORTH) {
          _north.add(item);
        } else if(type == SOUTH) {
          _south.add(item);
        } else if(type == EAST) {
          _east.add(item);
        } else if(type == WEST) {
          _west.add(item);
        }
      }
    }

    virtual uint32_t getLayoutExpandingDirections() const {
      return 0;
    }

    struct Part {
      Part() : _dirty(1) {

      }

      FOG_INLINE void add(LayoutItem* i) {        
        ItemProperties* prop = static_cast<ItemProperties*>(i->_layoutdata);
        prop->_id = _items.getLength(); 
        prop->_part = this;
        _items.append(i);
        _dirty = 1;
      }

      FOG_INLINE void remove(LayoutItem* i) {
        ItemProperties* prop = static_cast<ItemProperties*>(i->_layoutdata);
        _items.take(prop->_id);
        _dirty = 1;
      }

      FOG_INLINE void invalidate() {
        _dirty = 1;
      }

      int _dimension:31; //dimension of all items in part
      int _dirty: 1;
      List<LayoutItem*> _items;
    };

    struct Horizontal : public Part {
      void update() {
        int width = 0;
        for(sysuint_t i=0;i<_items.getLength();++i) {
          width += _items.at(i)->getLayoutSizeHint().getWidth();
        }

        _dimension = width;
        _dirty = 0;
      }

      FOG_INLINE int getDimension() {
        if(_dirty)
          update();
        return _dimension;
      }
    } _east, _west;

    struct Vertical : public Part {
      void update() {
        int height = 0;
        for(sysuint_t i=0;i<_items.getLength();++i) {
          LayoutItem* item = _items.at(i);
          height += item->getLayoutSizeHint().getHeight() + item->getContentTopMargin() + item->getContentBottomMargin();
        }

        _dimension = height;
        _dirty = 0;
      }

    FOG_INLINE int getDimension() {
        if(_dirty)
          update();
        return _dimension;
      }
    }_south, _north;


    struct ItemProperties : public LayoutItem::LayoutStruct {
      int _id;
      Part* _part;
    };

    LayoutItem* _center;
  };
}
#endif