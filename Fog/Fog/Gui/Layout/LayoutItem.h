// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUTITEM_H
#define _FOG_GUI_LAYOUT_LAYOUTITEM_H

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Object.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Layout;
struct Widget;
struct SpacerItem;

//Currently I'm to lazy to insert this into own file ;-)
//TODO: split horizontal & vertical policies into 2 variables?
struct LayoutPolicy {
  LayoutPolicy(uint32_t policy)  {
    _data._all = 0;
    _data._policy = policy;
  }

  FOG_INLINE uint32_t getHorizontalStretch() const { return _data._horizontalStretch; }
  FOG_INLINE uint32_t getVerticalStretch() const { return _data._verticalStretch; }
  FOG_INLINE void setHorizontalStretch(uchar stretchFactor) { _data._horizontalStretch = stretchFactor; }
  FOG_INLINE void setVerticalStretch(uchar stretchFactor) { _data._verticalStretch = stretchFactor; }

  FOG_INLINE uint32_t getPolicy() const { return _data._policy; }
  FOG_INLINE void setPolicy(uint32_t policy) { _data._policy = policy; }


  uint32_t expandingDirections() const {
    uint32_t result;
    if (_data._policy & LAYOUT_EXPANDING_WIDTH)
      result |= ORIENTATION_HORIZONTAL;
    if (_data._policy & LAYOUT_EXPANDING_HEIGHT)
      result |= ORIENTATION_VERTICAL;

    return result;
  }

  //is this method really usefull?
  void transpose() {    
    uint32_t result = (_data._policy << LAYOUT_HEIGHT_SHIFT); //move vertial to horizontal position
    result |= (_data._policy >> LAYOUT_HEIGHT_SHIFT);
    _data._policy = (result & 0xFF);
    
    uint32_t hStretch = uint32_t(getHorizontalStretch());
    uint32_t vStretch = uint32_t(getVerticalStretch());
    setHorizontalStretch(vStretch);
    setVerticalStretch(hStretch);
  }


  FOG_INLINE void setHeightForWidth(bool b) { _data._heightForWidth = b; }
  FOG_INLINE bool hasHeightForWidth() const { return _data._heightForWidth; }

  bool operator==(const LayoutPolicy& s) const { return _data._all == s._data._all; }
  bool operator!=(const LayoutPolicy& s) const { return _data._all != s._data._all; }

  union u {
    struct {
      uint32_t _policy : 8;  
      uint32_t _horizontalStretch : 8;
      uint32_t _verticalStretch : 8;

      uint32_t _heightForWidth : 1;
      uint32_t _unused : 7;
    };

    uint32_t _all;
  } _data;

};

// ============================================================================
// [Fog::LayoutItem]
// ============================================================================

//! @brief Base for @c Widget and @c Layout classes.
struct FOG_API LayoutItem : public Object
{
  FOG_DECLARE_OBJECT(LayoutItem, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  LayoutItem(uint32_t alignment = 0);
  virtual ~LayoutItem();

  virtual IntSize getSizeHint() const = 0;
  virtual IntSize getMinimumSize() const = 0;
  virtual IntSize getMaximumSize() const = 0;
  virtual uint32_t getExpandingDirections() const = 0;
  virtual bool isEmpty() const = 0;
  
  //reimpl if needed
  virtual bool hasHeightForWidth() const { return false; }
  virtual int getHeightForWidth(int width) const { return -1; }
  virtual int getMinimumHeightForWidth(int width) const { return getHeightForWidth(width); }

  virtual void invalidateLayout() { }

  //methods for doing real geometry changes
  virtual void setLayoutGeometry(const IntRect&) = 0;
  virtual IntRect getLayoutGeometry() const = 0;

  uint32_t getAlignment() const { return _alignment; }
  void setAlignment(uint32_t a) { _alignment = a; }

  //don't know if we really need this methods

  virtual Widget* widget() { return 0; }
  virtual Layout* layout() { return 0; }
  virtual SpacerItem* spacerItem() { return 0; }
  
  uint32_t _alignment;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUTITEM_H
