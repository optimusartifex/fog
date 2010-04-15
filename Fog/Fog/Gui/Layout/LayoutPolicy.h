// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUTPOLICY_H
#define _FOG_GUI_LAYOUT_LAYOUTPOLICY_H

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

// ============================================================================
// [Fog::LayoutPolicy]
// ============================================================================

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
  FOG_INLINE uint32_t getHorizontalPolicy() const { return _data._policy & 0xF; }
  FOG_INLINE uint32_t getVerticalPolicy() const { return _data._policy & 0xF0; }
  FOG_INLINE void setPolicy(uint32_t policy) { _data._policy = policy; }


  FOG_INLINE bool isHorizontalPolicyIgnored() const { return getHorizontalPolicy() == LAYOUT_POLICY_WIDTH_IGNORED; }
  FOG_INLINE bool isVerticalPolicyIgnored() const { return getVerticalPolicy() == LAYOUT_POLICY_HEIGHT_IGNORED; }

  FOG_INLINE bool isHorizontalPolicyExpanding() const { return getHorizontalPolicy() == LAYOUT_POLICY_WIDTH_EXPANDING; }
  FOG_INLINE bool isVerticalPolicyExpanding() const { return getVerticalPolicy() == LAYOUT_POLICY_HEIGHT_EXPANDING; }

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

}

#endif