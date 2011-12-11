// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_THICKNESS_H
#define _FOG_G2D_GEOMETRY_THICKNESS_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>

namespace Fog {
  
//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::ThicknessI]
// ============================================================================

//! @brief Thickness - information about padding, margin, and border ().
struct FOG_NO_EXPORT ThicknessI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------
  
  FOG_INLINE ThicknessI()
  {
  }

  FOG_INLINE ThicknessI(const ThicknessI& other) :
    top(other.top),
    right(other.right),
    bottom(other.bottom),
    left(other.left)
  {
  }
  
  explicit FOG_INLINE ThicknessI(int uniform) :
    top(uniform),
    right(uniform),
    bottom(uniform),
    left(uniform)
  {
  }
  
  FOG_INLINE ThicknessI(int vertical, int horizontal) :
    top(vertical),
    right(horizontal),
    bottom(vertical),
    left(horizontal)
  {
  }
  
  FOG_INLINE ThicknessI(int top, int right, int bottom, int left) :
    top(top),
    right(right),
    bottom(bottom),
    left(left)
  {
  }
  
  explicit FOG_INLINE ThicknessI(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool isEmpty() const { return top == 0 && right == 0 && bottom == 0 && left == 0; }
  FOG_INLINE bool isUniform() const { return top == right && right == bottom && bottom == left; }
  
  FOG_INLINE int getLeft() const { return left; }
  FOG_INLINE int getTop() const { return top; }
  FOG_INLINE int getRight() const { return right; }
  FOG_INLINE int getBottom() const { return bottom; }
  
  FOG_INLINE ThicknessI& setLeft(int left) { this->left = left; return *this; }
  FOG_INLINE ThicknessI& setTop(int top) { this->top = top; return *this; }
  FOG_INLINE ThicknessI& setRight(int right) { this->right = right; return *this; }
  FOG_INLINE ThicknessI& setBottom(int bottom) { this->bottom = bottom; return *this; }
  
  FOG_INLINE ThicknessI& setVertical(int vertical) { this->top = this->bottom = vertical; return *this; }
  FOG_INLINE ThicknessI& setHorizontal(int horizontal) { this->left = this->right = horizontal; return *this; }
  
  FOG_INLINE ThicknessI& setThickness(const ThicknessI& other)
  {
    MemOps::copy_t<ThicknessI>(this, &other);
    return *this;
  }

  FOG_INLINE ThicknessI& setThickness(int uniform)
  {
    this->top    = uniform;
    this->right  = uniform;
    this->bottom = uniform;
    this->left   = uniform;
    
    return *this;
  }
  
  FOG_INLINE ThicknessI& setThickness(int vertical, int horizontal)
  {
    this->top    = vertical;
    this->right  = horizontal;
    this->bottom = vertical;
    this->left   = horizontal;
    
    return *this;
  }

  FOG_INLINE ThicknessI& setThickness(int top, int right, int bottom, int left)
  {
    this->top    = top;
    this->right  = right;
    this->bottom = bottom;
    this->left   = left;
    
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------
  
  FOG_INLINE ThicknessI& clear()
  {
    MemOps::zero_t<ThicknessI>(this);
    return *this;
  }
  
  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool eq(int uniform) const
  {
    return (this->top    == uniform) &
           (this->right  == uniform) &
           (this->bottom == uniform) &
           (this->left   == uniform) ;
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
  
  FOG_INLINE bool eq(const ThicknessI& other) const
  {
    return MemOps::eq_t<ThicknessI>(this, &other);
  }
  
  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool operator==(const ThicknessI& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ThicknessI& other) const { return !eq(other); }
  
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int top;
  int right;
  int bottom;
  int left;
};

// ============================================================================
// [Fog::ThicknessF]
// ============================================================================

//! @brief Thickness - information about padding, margin, and border (float).
struct FOG_NO_EXPORT ThicknessF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------
  
  FOG_INLINE ThicknessF()
  {
  }
  
  FOG_INLINE ThicknessF(const ThicknessF& other) :
    top(other.top),
    right(other.right),
    bottom(other.bottom),
    left(other.left)
  {
  }
  
  explicit FOG_INLINE ThicknessF(float uniform) :
    top(uniform),
    right(uniform),
    bottom(uniform),
    left(uniform)
  {
  }
  
  FOG_INLINE ThicknessF(float vertical, float horizontal) :
    top(vertical),
    right(horizontal),
    bottom(vertical),
    left(horizontal)
  {
  }
  
  FOG_INLINE ThicknessF(float top, float right, float bottom, float left) :
    top(top),
    right(right),
    bottom(bottom),
    left(left)
  {
  }
  
  explicit FOG_INLINE ThicknessF(_Uninitialized) {}

  explicit FOG_INLINE ThicknessF(const ThicknessI& other) { setThickness(other); }
  explicit FOG_INLINE ThicknessF(const ThicknessD& other) { setThickness(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool isEmpty() const { return top == 0 && right == 0 && bottom == 0 && left == 0; }
  FOG_INLINE bool isUniform() const { return top == right && right == bottom && bottom == left; }
  
  FOG_INLINE float getLeft() const { return left; }
  FOG_INLINE float getTop() const { return top; }
  FOG_INLINE float getRight() const { return right; }
  FOG_INLINE float getBottom() const { return bottom; }
  
  FOG_INLINE ThicknessF& setLeft(float left) { this->left = left; return *this; }
  FOG_INLINE ThicknessF& setTop(float top) { this->top = top; return *this; }
  FOG_INLINE ThicknessF& setRight(float right) { this->right = right; return *this; }
  FOG_INLINE ThicknessF& setBottom(float bottom) { this->bottom = bottom; return *this; }
  
  FOG_INLINE ThicknessF& setVertical(float vertical) { this->top = this->bottom = vertical; return *this; }
  FOG_INLINE ThicknessF& setHorizontal(float horizontal) { this->left = this->right = horizontal; return *this; }
  
  FOG_INLINE ThicknessF& setThickness(const ThicknessI& other)
  {
    this->top    = float(other.top);
    this->right  = float(other.right);
    this->bottom = float(other.bottom);
    this->left   = float(other.left);

    return *this;
  }

  FOG_INLINE ThicknessF& setThickness(const ThicknessF& other)
  {
    MemOps::copy_t<ThicknessF>(this, &other);
    return *this;
  }

  // Implemented-Later.
  FOG_INLINE ThicknessF& setThickness(const ThicknessD& other);

  FOG_INLINE ThicknessF& setThickness(float uniform)
  {
    this->top    = uniform;
    this->right  = uniform;
    this->bottom = uniform;
    this->left   = uniform;
    
    return *this;
  }
  
  FOG_INLINE ThicknessF& setThickness(float vertical, float horizontal)
  {
    this->top    = vertical;
    this->right  = horizontal;
    this->bottom = vertical;
    this->left   = horizontal;

    return *this;
  }

  FOG_INLINE ThicknessF& setThickness(float top, float right, float bottom, float left)
  {
    this->top    = top;
    this->right  = right;
    this->bottom = bottom;
    this->left   = left;
    
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------
  
  FOG_INLINE ThicknessF& clear()
  {
    MemOps::zero_t<ThicknessF>(this);
    return *this;
  }
  
  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool eq(float uniform) const
  {
    return (this->top    == uniform) &
           (this->right  == uniform) &
           (this->bottom == uniform) &
           (this->left   == uniform) ;
  }
  
  FOG_INLINE bool eq(float vertical, float horizontal) const
  {
    return (this->top    == vertical  ) &
           (this->right  == horizontal) &
           (this->bottom == vertical  ) &
           (this->left   == horizontal) ;
  }
  
  FOG_INLINE bool eq(float top, float right, float bottom, float left) const
  {
    return (this->top    == top   ) &
           (this->right  == right ) &
           (this->bottom == bottom) &
           (this->left   == left  ) ;
  }
  
  FOG_INLINE bool eq(const ThicknessF& other) const
  {
    return MemOps::eq_t<ThicknessF>(this, &other);
  }
  
  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool operator==(const ThicknessF& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ThicknessF& other) const { return !eq(other); }
  
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float top;
  float right;
  float bottom;
  float left;
};

// ============================================================================
// [Fog::ThicknessD]
// ============================================================================

//! @brief Thickness - information about padding, margin, and border (double).
struct FOG_NO_EXPORT ThicknessD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------
  
  FOG_INLINE ThicknessD()
  {
  }
  
  FOG_INLINE ThicknessD(const ThicknessD& other) :
    top(other.top),
    right(other.right),
    bottom(other.bottom),
    left(other.left)
  {
  }
  
  explicit FOG_INLINE ThicknessD(double uniform) :
    top(uniform),
    right(uniform),
    bottom(uniform),
    left(uniform)
  {
  }
  
  FOG_INLINE ThicknessD(double vertical, double horizontal) :
    top(vertical),
    right(horizontal),
    bottom(vertical),
    left(horizontal)
  {
  }
  
  FOG_INLINE ThicknessD(double top, double right, double bottom, double left) :
    top(top),
    right(right),
    bottom(bottom),
    left(left)
  {
  }
  
  explicit FOG_INLINE ThicknessD(_Uninitialized) {}

  explicit FOG_INLINE ThicknessD(const ThicknessI& other) { setThickness(other); }
  explicit FOG_INLINE ThicknessD(const ThicknessF& other) { setThickness(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool isEmpty() const { return top == 0 && right == 0 && bottom == 0 && left == 0; }
  FOG_INLINE bool isUniform() const { return top == right && right == bottom && bottom == left; }
  
  FOG_INLINE double getLeft() const { return left; }
  FOG_INLINE double getTop() const { return top; }
  FOG_INLINE double getRight() const { return right; }
  FOG_INLINE double getBottom() const { return bottom; }
  
  FOG_INLINE ThicknessD& setLeft(double left) { this->left = left; return *this; }
  FOG_INLINE ThicknessD& setTop(double top) { this->top = top; return *this; }
  FOG_INLINE ThicknessD& setRight(double right) { this->right = right; return *this; }
  FOG_INLINE ThicknessD& setBottom(double bottom) { this->bottom = bottom; return *this; }
  
  FOG_INLINE ThicknessD& setVertical(double vertical) { this->top = this->bottom = vertical; return *this; }
  FOG_INLINE ThicknessD& setHorizontal(double horizontal) { this->left = this->right = horizontal; return *this; }
  
  FOG_INLINE ThicknessD& setThickness(const ThicknessI& other)
  {
    this->top    = other.top;
    this->right  = other.right;
    this->bottom = other.bottom;
    this->left   = other.left;

    return *this;
  }

  FOG_INLINE ThicknessD& setThickness(const ThicknessF& other)
  {
    this->top    = other.top;
    this->right  = other.right;
    this->bottom = other.bottom;
    this->left   = other.left;

    return *this;
  }

  FOG_INLINE ThicknessD& setThickness(const ThicknessD& other)
  {
    MemOps::copy_t<ThicknessD>(this, &other);

    return *this;
  }

  FOG_INLINE ThicknessD& setThickness(double uniform)
  {
    this->top    = uniform;
    this->right  = uniform;
    this->bottom = uniform;
    this->left   = uniform;
    
    return *this;
  }
  
  FOG_INLINE ThicknessD& setThickness(double vertical, double horizontal)
  {
    this->top    = vertical;
    this->right  = horizontal;
    this->bottom = vertical;
    this->left   = horizontal;

    return *this;
  }

  FOG_INLINE ThicknessD& setThickness(double top, double right, double bottom, double left)
  {
    this->top    = top;
    this->right  = right;
    this->bottom = bottom;
    this->left   = left;
    
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------
  
  FOG_INLINE ThicknessD& clear()
  {
    MemOps::zero_t<ThicknessD>(this);
    return *this;
  }
  
  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool eq(double uniform) const
  {
    return (this->top    == uniform) &
           (this->right  == uniform) &
           (this->bottom == uniform) &
           (this->left   == uniform) ;
  }
  
  FOG_INLINE bool eq(double vertical, double horizontal) const
  {
    return (this->top    == vertical  ) &
           (this->right  == horizontal) &
           (this->bottom == vertical  ) &
           (this->left   == horizontal) ;
  }

  FOG_INLINE bool eq(double top, double right, double bottom, double left) const
  {
    return (this->top    == top   ) &
           (this->right  == right ) &
           (this->bottom == bottom) &
           (this->left   == left  ) ;
  }
  
  FOG_INLINE bool eq(const ThicknessD& other) const
  {
    return MemOps::eq_t<ThicknessD>(this, &other);
  }
  
  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool operator==(const ThicknessD& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ThicknessD& other) const { return !eq(other); }
  
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double top;
  double right;
  double bottom;
  double left;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE ThicknessF& ThicknessF::setThickness(const ThicknessD& other)
{
  this->top    = float(other.top);
  this->right  = float(other.right);
  this->bottom = float(other.bottom);
  this->left   = float(other.left);

  return *this;
}

//! @}
  
} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::ThicknessF, 4)
FOG_FUZZY_DECLARE_D_VEC(Fog::ThicknessD, 4)

// [Guard]
#endif // _FOG_G2D_GEOMETRY_THICKNESS_H
