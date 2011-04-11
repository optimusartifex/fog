// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_MARGIN_H
#define _FOG_GUI_LAYOUT_MARGIN_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/Gui/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Gui_Layout
//! @{

//! @brief Margin.
//!
//! Margin contains margin for top, right, bottom and left locations.
struct FOG_NO_EXPORT Margin
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Margin()
  {
  }

  FOG_INLINE Margin(const Margin& other) :
    top(other.top),
    right(other.right),
    bottom(other.bottom),
    left(other.left)
  {
  }

  FOG_INLINE Margin(int all) :
    top(all),
    right(all),
    bottom(all),
    left(all)
  {
  }

  FOG_INLINE Margin(int vertical, int horizontal) :
    top(vertical),
    right(horizontal),
    bottom(vertical),
    left(horizontal)
  {
  }

  FOG_INLINE Margin(int top, int right, int bottom, int left) :
    top(top),
    right(right),
    bottom(bottom),
    left(left)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isEmpty() const { return top == 0 && right == 0 && bottom == 0 && left == 0; }
  FOG_INLINE bool isUniform() const { return top == right && right == bottom && bottom == left; }

  FOG_INLINE int getLeft() const { return left; }
  FOG_INLINE int getTop() const { return top; }
  FOG_INLINE int getRight() const { return right; }
  FOG_INLINE int getBottom() const { return bottom; }

  FOG_INLINE Margin& setLeft(int left) { this->left = left; return *this; }
  FOG_INLINE Margin& setTop(int top) { this->top = top; return *this; }
  FOG_INLINE Margin& setRight(int right) { this->right = right; return *this; }
  FOG_INLINE Margin& setBottom(int bottom) { this->bottom = bottom; return *this; }

  FOG_INLINE Margin& setVertical(int vertical) { this->top = this->bottom = vertical; return *this; }
  FOG_INLINE Margin& setHorizontal(int horizontal) { this->left = this->right = horizontal; return *this; }

  FOG_INLINE Margin& setMargin(int all)
  {
    this->top    = all;
    this->right  = all;
    this->bottom = all;
    this->left   = all;

    return *this;
  }

  FOG_INLINE Margin& setMargin(int vertical, int horizontal)
  {
    this->top    = vertical;
    this->right  = horizontal;
    this->bottom = vertical;
    this->left   = horizontal;
    
    return *this;
  }

  FOG_INLINE Margin& setMargin(int top, int right, int bottom, int left)
  { 
    this->top    = top;
    this->right  = right;
    this->bottom = bottom;
    this->left   = left;

    return *this;
  }

  FOG_INLINE Margin& setMargin(const Margin &other)
  {
    Memory::copy_t<Margin>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE Margin& clear()
  {
    Memory::zero_t<Margin>(this);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(int all) const
  {
    return (this->top    == all) & 
           (this->right  == all) &
           (this->bottom == all) &
           (this->left   == all) ;
  }

  FOG_INLINE bool eq(int vertical, int horizontal) const
  {
    return (this->top    == vertical  ) & 
           (this->right  == horizontal) &
           (this->bottom == vertical  ) &
           (this->left   == horizontal) ;
  }

  FOG_INLINE bool eq(int top, int right, int bottom, int left) const
  {
    return (this->top    == top   ) & 
           (this->right  == right ) &
           (this->bottom == bottom) &
           (this->left   == left  ) ;
  }

  FOG_INLINE bool eq(const Margin& other) const
  {
    return Memory::eq_t<Margin>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator==(const Margin& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Margin& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int top;
  int right;
  int bottom;
  int left;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Margin, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GUI_LAYOUT_MARGIN_H
