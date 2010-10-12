// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_GEOMETRY_H
#define _FOG_GRAPHICS_GEOMETRY_H

// [Dependencies]
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct BoxD;
struct BoxF;
struct BoxI;

struct PointD;
struct PointF;
struct PointI;

struct RectD;
struct RectF;
struct RectI;

struct SizeD;
struct SizeF;
struct SizeI;

// ============================================================================
// [Rules]
// ============================================================================

// This file includes several implementations of common geometric structures:
// - Box    - [x1, y1] and [x2, y2].
// - Point  - [x, y].
// - Rect   - [x, y] and [width height].
// - Size   - [width, height].
//
// The implementation for several data-types exists:
// - I - All members as 32-bit integers - int.
// - F - All members as 32-bit floats   - float.
// - D - All members as 64-bit floats   - double.
//
// Each datatype contains implicit conversion from less-precision types, so for
// example you can translate PointF by PointI, but you can't translate
// PointI by PointF or PointD.
//
// These structures are so easy so the members are not prefixed with underscore
// and all members are public.

// ============================================================================
// [Fog::PointI]
// ============================================================================

//! @brief Point (32-bit integer version).
struct FOG_HIDDEN PointI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PointI()
  {
  }
  
  FOG_INLINE PointI(int px, int py) : x(px), y(py)
  {
  }

  FOG_INLINE PointI(const PointI& other)
  {
    if (sizeof(PointI) == 8)
    {
      Memory::copy8B(static_cast<void*>(this), static_cast<const void*>(&other));
    }
    else
    {
      x = other.x;
      y = other.y;
    }
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getX() const { return x; }
  FOG_INLINE int getY() const { return y; }

  FOG_INLINE PointI& set(const PointI& other)
  { 
    if (sizeof(PointI) == 8)
    {
      Memory::copy8B(static_cast<void*>(this), static_cast<const void*>(&other));
    }
    else
    {
      x = other.x;
      y = other.y;
    }
    return *this;
  }

  FOG_INLINE PointI& set(int px, int py)
  { 
    x = px;
    y = py;
    return *this;
  }

  FOG_INLINE PointI& setX(int px) { x = px; return *this; }
  FOG_INLINE PointI& setY(int py) { y = py; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE PointI& reset()
  { 
    x = 0; 
    y = 0; 
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE PointI& translate(const PointI& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  FOG_INLINE PointI& translate(int px, int py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE PointI translated(const PointI& other) const
  {
    return PointI(x + other.x, y + other.y);
  }

  FOG_INLINE PointI translated(int px, int py) const
  {
    return PointI(x + px, y + py);
  }

  // --------------------------------------------------------------------------
  // [Negate]
  // --------------------------------------------------------------------------

  FOG_INLINE PointI& negate()
  {
    x = -x;
    y = -y;
    return *this;
  }

  FOG_INLINE PointI negated() const
  {
    return PointI(-x, -y);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const PointI& other) const
  {
    if (sizeof(PointI) == 8)
      return Memory::eq8B(static_cast<const void*>(this), static_cast<const void*>(&other));
    else
      return (x == other.x) & (y == other.y);
  }

  FOG_INLINE bool eq(int px, int py) const
  {
    return (x == px) & (y == py);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF toPointF() const;
  FOG_INLINE PointD toPointD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PointI& operator=(const PointI& other)
  {
    x = other.x;
    y = other.y;
    return *this;
  }

  FOG_INLINE PointI& operator+=(const PointI& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  FOG_INLINE PointI& operator-=(const PointI& other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const PointI& other) const { return (x == other.x) & (y == other.y); }
  FOG_INLINE bool operator!=(const PointI& other) const { return (x != other.x) | (y != other.y); }

  FOG_INLINE bool operator< (const PointI& other) { return (y < other.y) | ((y <= other.y) & (x <  other.x)); }
  FOG_INLINE bool operator> (const PointI& other) { return (y > other.y) | ((y <= other.y) & (x >  other.x)); }
  FOG_INLINE bool operator<=(const PointI& other) { return (y < other.y) | ((y == other.y) & (x <= other.x)); }
  FOG_INLINE bool operator>=(const PointI& other) { return (y > other.y) | ((y == other.y) & (x >= other.x)); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int x;
  int y;
};

// ============================================================================
// [Fog::PointF]
// ============================================================================

//! @brief Point (32-bit float version).
struct PointF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF()
  {
  }

  FOG_INLINE PointF(const PointI& other) : x((float)other.x), y((float)other.y)
  {
  }

  FOG_INLINE PointF(const PointF& other) : x(other.x), y(other.y)
  {
  }

  FOG_INLINE PointF(int px, int py) : x((float)px), y((float)py)
  {
  }

  FOG_INLINE PointF(float px, float py) : x(px), y(py)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getX() const { return x; }
  FOG_INLINE float getY() const { return y; }

  FOG_INLINE PointF& set(const PointI& other)
  {
    x = (float)other.x;
    y = (float)other.y;
    return *this;
  }

  FOG_INLINE PointF& set(int px, int py)
  {
    x = (float)px;
    y = (float)py;
    return *this;
  }

  FOG_INLINE PointF& set(const PointF& other)
  {
    if (sizeof(PointF) == 8)
    {
      Memory::copy8B(static_cast<void*>(this), static_cast<const void*>(&other));
    }
    else
    {
      x = other.x;
      y = other.y;
    }
    return *this;
  }

  FOG_INLINE PointF& set(float px, float py)
  {
    x = px;
    y = py;
    return *this;
  }

  FOG_INLINE PointF& setX(int px) { x = (float)px; return *this; }
  FOG_INLINE PointF& setY(int py) { y = (float)py; return *this; }

  FOG_INLINE PointF& setX(float px) { x = px; return *this; }
  FOG_INLINE PointF& setY(float py) { y = py; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF& reset()
  {
    x = 0.0f;
    y = 0.0f;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF& translate(const PointI& other)
  {
    x += (float)other.x;
    y += (float)other.y;
    return *this;
  }

  FOG_INLINE PointF& translate(int px, int py)
  {
    x += (float)px;
    y += (float)py;
    return *this;
  }

  FOG_INLINE PointF& translate(const PointF& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  FOG_INLINE PointF& translate(float px, float py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE PointF translated(const PointI& other) const
  {
    return PointF(x + (float)other.x, y + (float)other.y);
  }

  FOG_INLINE PointF translated(int px, int py) const
  {
    return PointF(x + (float)px, y + (float)py);
  }

  FOG_INLINE PointF translated(const PointF& other) const
  {
    return PointF(x + other.x, y + other.y);
  }

  FOG_INLINE PointF translated(float px, float py) const
  {
    return PointF(x + px, y + py);
  }

  // --------------------------------------------------------------------------
  // [Negate]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF& negate()
  {
    x = -x;
    y = -y;
    return *this;
  }

  FOG_INLINE PointF negated() const
  {
    return PointF(-x, -y);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const PointF& other) const
  {
    return (x == other.x) & (y == other.y);
  }

  FOG_INLINE bool eq(float px, float py) const
  {
    return (x == px) & (y == py);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE PointD toPointD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF& operator=(const PointI& other)
  {
    return set(other);
  }

  FOG_INLINE PointF& operator=(const PointF& other)
  {
    return set(other);
  }

  FOG_INLINE PointF& operator+=(const PointI& other)
  {
    return translate(other);
  }

  FOG_INLINE PointF& operator+=(const PointF& other)
  {
    return translate(other);
  }

  FOG_INLINE PointF& operator-=(const PointI& other)
  {
    x -= (float)other.x;
    y -= (float)other.y;
    return *this;
  }

  FOG_INLINE PointF& operator-=(const PointF& other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const PointF& other) const { return (x == other.x) & (y == other.y); }
  FOG_INLINE bool operator!=(const PointF& other) const { return (x != other.x) | (y != other.y); }

  FOG_INLINE bool operator< (const PointF& other) { return (y < other.y) | ((y <= other.y) & (x <  other.x)); }
  FOG_INLINE bool operator> (const PointF& other) { return (y > other.y) | ((y <= other.y) & (x >  other.x)); }
  FOG_INLINE bool operator<=(const PointF& other) { return (y < other.y) | ((y == other.y) & (x <= other.x)); }
  FOG_INLINE bool operator>=(const PointF& other) { return (y > other.y) | ((y == other.y) & (x >= other.x)); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float x;
  float y;
};

// ============================================================================
// [Fog::PointD]
// ============================================================================

//! @brief Point (64-bit float version).
struct PointD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PointD()
  {
  }

  FOG_INLINE PointD(const PointI& other) : x((double)other.x), y((double)other.y)
  {
  }

  FOG_INLINE PointD(int px, int py) : x((double)px), y((double)py)
  {
  }

  FOG_INLINE PointD(const PointF& other) : x((double)other.x), y((double)other.y)
  {
  }

  FOG_INLINE PointD(float px, float py) : x((float)px), y((float)py)
  {
  }

  FOG_INLINE PointD(const PointD& other) : x(other.x), y(other.y)
  {
  }

  FOG_INLINE PointD(double px, double py) : x(px), y(py)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getX() const { return x; }
  FOG_INLINE double getY() const { return y; }

  FOG_INLINE PointD& set(const PointI& other)
  {
    x = (double)other.x;
    y = (double)other.y;
    return *this;
  }

  FOG_INLINE PointD& set(const PointF& other)
  {
    x = (double)other.x;
    y = (double)other.y;
    return *this;
  }

  FOG_INLINE PointD& set(const PointD& other)
  {
    x = other.x;
    y = other.y;
    return *this;
  }

  FOG_INLINE PointD& set(double px, double py) 
  { 
    x = px;
    y = py;
    return *this;
  }

  FOG_INLINE PointD& setX(int px) { x = (double)px; return *this; }
  FOG_INLINE PointD& setY(int py) { y = (double)py; return *this; }

  FOG_INLINE PointD& setX(float px) { x = (double)px; return *this; }
  FOG_INLINE PointD& setY(float py) { y = (double)py; return *this; }

  FOG_INLINE PointD& setX(double px) { x = px; return *this; }
  FOG_INLINE PointD& setY(double py) { y = py; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE PointD& reset()
  { 
    x = 0.0;
    y = 0.0;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE PointD& translate(const PointI& other)
  {
    x += (double)other.x;
    y += (double)other.y;
    return *this;
  }

  FOG_INLINE PointD& translate(const PointD& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  FOG_INLINE PointD& translate(double px, double py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE PointD translated(const PointI& other) const
  {
    return PointD(x + (double)other.x, y + (double)other.y);
  }

  FOG_INLINE PointD translated(int px, int py) const
  {
    return PointD(x + (double)px, y + (double)py);
  }

  FOG_INLINE PointD translated(const PointF& other) const
  {
    return PointD(x + (double)other.x, y + (double)other.y);
  }

  FOG_INLINE PointD translated(float px, float py) const
  {
    return PointD(x + (double)px, y + (double)py);
  }

  FOG_INLINE PointD translated(const PointD& other) const
  {
    return PointD(x + other.x, y + other.y);
  }

  FOG_INLINE PointD translated(double px, double py) const
  {
    return PointD(x + px, y + py);
  }

  // --------------------------------------------------------------------------
  // [Negate]
  // --------------------------------------------------------------------------

  FOG_INLINE PointD& negate()
  {
    x = -x;
    y = -y;
    return *this;
  }

  FOG_INLINE PointD negated() const
  {
    return PointD(-x, -y);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const PointD& other) const
  {
    return (x == other.x) & (y == other.y);
  }

  FOG_INLINE bool eq(double px, double py) const
  {
    return (x == px) & (y == py);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF toPointF() const { return PointF((float)x, (float)y); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PointD& operator=(const PointI& other)
  {
    return set(other);
  }

  FOG_INLINE PointD& operator=(const PointF& other)
  {
    return set(other);
  }

  FOG_INLINE PointD& operator=(const PointD& other)
  {
    return set(other);
  }

  FOG_INLINE PointD& operator+=(const PointI& other)
  {
    x += (double)other.x;
    y += (double)other.y;
    return *this;
  }

  FOG_INLINE PointD& operator+=(const PointF& other)
  {
    x += (double)other.x;
    y += (double)other.y;
    return *this;
  }

  FOG_INLINE PointD& operator+=(const PointD& other) 
  {
    x += other.x;
    y += other.y;
    return *this;
  }
  
  FOG_INLINE PointD& operator-=(const PointI& other)
  {
    x -= (double)other.x;
    y -= (double)other.y;
    return *this;
  }

  FOG_INLINE PointD& operator-=(const PointF& other)
  {
    x -= (double)other.x;
    y -= (double)other.y;
    return *this;
  }

  FOG_INLINE PointD& operator-=(const PointD& other) 
  { 
    x -= other.x;
    y -= other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const PointD& other) const { return (x == other.x) & (y == other.y); }
  FOG_INLINE bool operator!=(const PointD& other) const { return (x != other.x) | (y != other.y); }

  FOG_INLINE bool operator< (const PointD& other) { return (y < other.y) | ((y <= other.y) & (x <  other.x)); }
  FOG_INLINE bool operator> (const PointD& other) { return (y > other.y) | ((y <= other.y) & (x >  other.x)); }
  FOG_INLINE bool operator<=(const PointD& other) { return (y < other.y) | ((y == other.y) & (x <= other.x)); }
  FOG_INLINE bool operator>=(const PointD& other) { return (y > other.y) | ((y == other.y) & (x >= other.x)); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double x;
  double y;
};

// ============================================================================
// [Fog::SizeI]
// ============================================================================

//! @brief Size (32-bit integer version).
struct SizeI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI()
  {
  }

  FOG_INLINE SizeI(const SizeI& other) : w(other.w), h(other.h)
  {
  }

  FOG_INLINE SizeI(int sw, int sh) : w(sw), h(sh)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getWidth() const { return w; }
  FOG_INLINE int getHeight() const { return h; }

  FOG_INLINE SizeI& set(const SizeI& other)
  {
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE SizeI& set(int sw, int sh)
  {
    w = sw;
    h = sh;
    return *this;
  }

  FOG_INLINE SizeI& setWidth(int sw) { w = sw; return *this; }
  FOG_INLINE SizeI& setHeight(int sh) { h = sh; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI& reset()
  {
    w = 0;
    h = 0;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return (w > 0) & (h > 0);
  }
  
  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const SizeI& other) const
  {
    return (w == other.w) & (h == other.h);
  }
  
  FOG_INLINE bool eq(int sw, int sh) const
  {
    return (w == sw) && (h == sh);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI& adjust(int sw, int sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeI& adjust(const SizeI& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeI adjusted(int sw, int sh) const
  {
    return SizeI(w + sw, h + sh);
  }

  FOG_INLINE SizeI adjusted(const SizeI& sz) const
  {
    return SizeI(w + sz.w, h + sz.h);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF toSizeF() const;
  FOG_INLINE SizeD toSizeD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI& operator=(const SizeI& other)
  {
    return set(other);
  }

  FOG_INLINE bool operator==(const SizeI& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool operator!=(const SizeI& other) const
  {
    return (w != other.w) | (h != other.h);
  }

  FOG_INLINE SizeI& operator+=(const SizeI& other)
  {
    w += other.w;
    h += other.h;
    return *this;
  }

  FOG_INLINE SizeI& operator-=(const SizeI& other)
  {
    w -= other.w;
    h -= other.h;
    return *this;
  }

  FOG_INLINE SizeI expandedTo(const SizeI& otherSize) const
  {
    return SizeI(Math::max(w,otherSize.w), Math::max(h,otherSize.h));
  }

  FOG_INLINE SizeI boundedTo(const SizeI& otherSize) const
  {
    return SizeI(Math::min(w,otherSize.w), Math::min(h,otherSize.h));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int w;
  int h;
};

// ============================================================================
// [Fog::SizeF]
// ============================================================================

struct SizeF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF()
  {
  }

  FOG_INLINE SizeF(const SizeI& other) : w((float)other.w), h((float)other.h)
  {
  }

  FOG_INLINE SizeF(int sw, int sh) : w((float)sw), h((float)sh)
  {
  }

  FOG_INLINE SizeF(const SizeF& other) : w(other.w), h(other.h)
  {
  }

  FOG_INLINE SizeF(float sw, float sh) : w(sw), h(sh)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getWidth() const { return w; }
  FOG_INLINE float getHeight() const { return h; }

  FOG_INLINE SizeF& set(const SizeI& other)
  {
    w = (float)other.w;
    h = (float)other.h;
    return *this;
  }

  FOG_INLINE SizeF& set(int sw, int sh)
  {
    w = (float)sw;
    h = (float)sh;
    return *this;
  }

  FOG_INLINE SizeF& set(const SizeF& other)
  {
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE SizeF& set(float sw, float sh)
  {
    w = sw;
    h = sh;
    return *this;
  }

  FOG_INLINE SizeF& setWidth(int sw) { w = (float)sw; return *this; }
  FOG_INLINE SizeF& setHeight(int sh) { h = (float)sh; return *this; }

  FOG_INLINE SizeF& setWidth(float sw) { w = sw; return *this; }
  FOG_INLINE SizeF& setHeight(float sh) { h = sh; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF& reset()
  {
    w = 0.0f;
    h = 0.0f;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return (w > 0.0) & (h > 0.0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const SizeF& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool eq(float sw, float sh) const
  {
    return (w == sw) & (h == sh);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF& adjust(int sw, int sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeF& adjust(const SizeI& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeF& adjust(float sw, float sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeF& adjust(const SizeF& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeF adjusted(int sw, int sh) const
  {
    return SizeF(w + sw, h + sh);
  }

  FOG_INLINE SizeF adjusted(const SizeI& sz) const
  {
    return SizeF(w + sz.w, h + sz.h);
  }

  FOG_INLINE SizeF adjusted(float sw, float sh) const
  {
    return SizeF(w + sw, h + sh);
  }

  FOG_INLINE SizeF adjusted(const SizeF& sz) const
  {
    return SizeF(w + sz.w, h + sz.h);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD toSizeD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF& operator=(const SizeI& other)
  {
    return set(other);
  }

  FOG_INLINE SizeF& operator=(const SizeF& other)
  {
    return set(other);
  }

  FOG_INLINE bool operator==(const SizeF& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool operator!=(const SizeF& other) const
  {
    return (w != other.w) | (h != other.h);
  }

  FOG_INLINE SizeF& operator+=(const SizeI& other)
  {
    w += other.w;
    h += other.h;
    return *this;
  }

  FOG_INLINE SizeF& operator+=(const SizeF& other)
  {
    w += other.w;
    h += other.h;
    return *this;
  }

  FOG_INLINE SizeF& operator-=(const SizeI& other)
  {
    w -= other.w;
    h -= other.h;
    return *this;
  }

  FOG_INLINE SizeF& operator-=(const SizeF& other)
  {
    w -= other.w;
    h -= other.h;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float w;
  float h;
};

// ============================================================================
// [Fog::SizeD]
// ============================================================================

struct SizeD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD()
  {
  }

  FOG_INLINE SizeD(const SizeI& other) : w((double)other.w), h((double)other.h)
  {
  }

  FOG_INLINE SizeD(int sw, int sh) : w((double)sw), h((double)sh)
  {
  }

  FOG_INLINE SizeD(const SizeF& other) : w((double)other.w), h((double)other.h)
  {
  }

  FOG_INLINE SizeD(float sw, float sh) : w((double)sw), h((double)sh)
  {
  }

  FOG_INLINE SizeD(const SizeD& other) : w(other.w), h(other.h)
  {
  }

  FOG_INLINE SizeD(double sw, double sh) : w(sw), h(sh)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getWidth() const { return w; }
  FOG_INLINE double getHeight() const { return h; }

  FOG_INLINE SizeD& set(const SizeI& other)
  {
    w = (double)other.w;
    h = (double)other.h;
    return *this;
  }

  FOG_INLINE SizeD& set(int sw, int sh)
  {
    w = (double)sw;
    h = (double)sh;
    return *this;
  }

  FOG_INLINE SizeD& set(const SizeF& other)
  {
    w = (double)other.w;
    h = (double)other.h;
    return *this;
  }

  FOG_INLINE SizeD& set(float sw, float sh)
  {
    w = (double)sw;
    h = (double)sh;
    return *this;
  }

  FOG_INLINE SizeD& set(const SizeD& other)
  {
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE SizeD& set(double sw, double sh)
  {
    w = sw;
    h = sh;
    return *this;
  }

  FOG_INLINE SizeD& setWidth(int sw) { w = sw; return *this; }
  FOG_INLINE SizeD& setHeight(int sh) { h = sh; return *this; }

  FOG_INLINE SizeD& setWidth(float sw) { w = sw; return *this; }
  FOG_INLINE SizeD& setHeight(float sh) { h = sh; return *this; }

  FOG_INLINE SizeD& setWidth(double sw) { w = sw; return *this; }
  FOG_INLINE SizeD& setHeight(double sh) { h = sh; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD& reset()
  {
    w = 0.0;
    h = 0.0;
    return *this; 
  }

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return (w > 0.0) & (h > 0.0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const SizeD& other) const
  {
    return (w == other.w) & (h == other.h);
  }
  
  FOG_INLINE bool eq(double sw, double sh) const
  {
    return (w == sw) & (h == sh);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD& adjust(int sw, int sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeD& adjust(const SizeI& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeD& adjust(float sw, float sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeD& adjust(const SizeF& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeD& adjust(double sw, double sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeD& adjust(const SizeD& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeD adjusted(int sw, int sh) const
  {
    return SizeD(w + sw, h + sh);
  }

  FOG_INLINE SizeD adjusted(const SizeI& sz) const
  {
    return SizeD(w + sz.w, h + sz.h);
  }

  FOG_INLINE SizeD adjusted(float sw, float sh) const
  {
    return SizeD(w + sw, h + sh);
  }

  FOG_INLINE SizeD adjusted(const SizeF& sz) const
  {
    return SizeD(w + sz.w, h + sz.h);
  }

  FOG_INLINE SizeD adjusted(double sw, double sh) const
  {
    return SizeD(w + sw, h + sh);
  }

  FOG_INLINE SizeD adjusted(const SizeD& sz) const
  {
    return SizeD(w + sz.w, h + sz.h);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF toSizeF() const { return SizeF((float)w, (float)h); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD& operator=(const SizeI& other)
  {
    return set(other);
  }

  FOG_INLINE SizeD& operator=(const SizeF& other)
  {
    return set(other);
  }

  FOG_INLINE SizeD& operator=(const SizeD& other)
  {
    return set(other);
  }

  FOG_INLINE bool operator==(const SizeD& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool operator!=(const SizeD& other) const
  {
    return (w != other.w) | (h != other.h);
  }

  FOG_INLINE SizeD& operator+=(const SizeI& other)
  {
    w += other.w;
    h += other.h;
    return *this;
  }

  FOG_INLINE SizeD& operator+=(const SizeF& other)
  {
    w += other.w;
    h += other.h;
    return *this;
  }

  FOG_INLINE SizeD& operator+=(const SizeD& other)
  {
    w += other.w;
    h += other.h;
    return *this;
  }

  FOG_INLINE SizeD& operator-=(const SizeI& other)
  {
    w -= other.w;
    h -= other.h;
    return *this;
  }

  FOG_INLINE SizeD& operator-=(const SizeF& other)
  {
    w -= other.w;
    h -= other.h;
    return *this;
  }

  FOG_INLINE SizeD& operator-=(const SizeD& other)
  {
    w -= other.w;
    h -= other.h;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double w;
  double h;
};

// ============================================================================
// [Fog::RectI]
// ============================================================================

//! @brief Rectangle (32-bit integer version).
struct RectI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI()
  {
  }

  FOG_INLINE RectI(const RectI& other) :
    x(other.x), y(other.y), w(other.w), h(other.h)
  {
  }

  FOG_INLINE RectI(int rx, int ry, int rw, int rh) :
    x(rx), y(ry), w(rw), h(rh)
  {
  }

  FOG_INLINE explicit RectI(const BoxI& box);
  // Defined later.

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getX() const { return x; }
  FOG_INLINE int getY() const { return y; }
  FOG_INLINE int getWidth() const { return w; }
  FOG_INLINE int getHeight() const { return h; }

  FOG_INLINE int getX1() const { return x; }
  FOG_INLINE int getY1() const { return y; }
  FOG_INLINE int getX2() const { return x + w; }
  FOG_INLINE int getY2() const { return y + h; }

  FOG_INLINE int getLeft() const { return x; }
  FOG_INLINE int getTop() const { return y; }
  FOG_INLINE int getRight() const { return x + w; }
  FOG_INLINE int getBottom() const { return y + h; }

  FOG_INLINE const PointI& getPosition() const { return *(const PointI *)(const void*)(&x); }
  FOG_INLINE const SizeI& getSize() const { return *(const SizeI *)(const void*)(&w); }

  FOG_INLINE RectI& set(int rx, int ry, int rw, int rh)
  { 
    x = rx; 
    y = ry; 
    w = rw; 
    h = rh; 

    return *this;
  }

  FOG_INLINE RectI& set(const RectI &other)
  {
    if (sizeof(RectI) == 16)
    {
      Memory::copy16B(static_cast<void*>(this), static_cast<const void*>(&other));
    }
    else
    {
      x = other.x;
      y = other.y;
      w = other.w;
      h = other.h;
    }
    return *this;
  }

  FOG_INLINE RectI& set(const BoxI& box);

  FOG_INLINE RectI& setX(int rx) { x = rx; return *this; }
  FOG_INLINE RectI& setY(int ry) { y = ry; return *this; }
  FOG_INLINE RectI& setWidth(int rw) { w = rw; return *this; }
  FOG_INLINE RectI& setHeight(int rh) { h = rh; return *this; }

  FOG_INLINE RectI& setX1(int x1) { x = x1; return *this; }
  FOG_INLINE RectI& setY1(int y1) { y = y1; return *this; }
  FOG_INLINE RectI& setX2(int x2) { w = x2 - x; return *this; }
  FOG_INLINE RectI& setY2(int y2) { h = y2 - y; return *this; }

  FOG_INLINE RectI& setLeft(int x1) { x = x1; return *this; }
  FOG_INLINE RectI& setTop(int y1) { y = y1; return *this; }
  FOG_INLINE RectI& setRight(int x2) { w = x2 - x; return *this; }
  FOG_INLINE RectI& setBottom(int y2) { h = y2 - y; return *this; }

  // --------------------------------------------------------------------------
  // [REset]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI& reset()
  {
    if (sizeof(RectI) == 16)
    {
      Memory::zero16B(static_cast<void*>(this));
    }
    else
    {
      x = 0;
      y = 0;
      w = 0;
      h = 0;
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI& translate(int rx, int ry) { x += rx; y += ry; return *this; }
  FOG_INLINE RectI& translate(const PointI& p) { x += p.x; y += p.y; return *this; }

  FOG_INLINE RectI translated(int rx, int ry) const { return RectI(x + rx, y + ry, w, h); }
  FOG_INLINE RectI translated(const PointI& p) const { return RectI(x + p.x, y + p.y, w, h); }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap. Remember, x2() and y2() coords aren't in the rectangle.
  FOG_INLINE bool overlaps(const RectI& r) const
  {
    return (( ((getY1()-r.getY2()) ^ (getY2()-r.getY1())) &
              ((getX1()-r.getX2()) ^ (getX2()-r.getX1())) ) < 0);
  }

  //! @brief Get whether rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const RectI& r) const
  {
    return ((r.getX1() >= getX1()) & (r.getX2() <= getX2()) &
            (r.getY1() >= getY1()) & (r.getY2() <= getY2()) );
  }

  static FOG_INLINE bool intersect(RectI& dest, const RectI& src1, const RectI& src2)
  {
    int xx = Math::max(src1.getX1(), src2.getX1());
    int yy = Math::max(src1.getY1(), src2.getY1());

    dest.set(xx,
             yy,
             Math::min(src1.getX2(), src2.getX2()) - xx,
             Math::min(src1.getY2(), src2.getY2()) - yy);
    return dest.isValid();
  }

  static FOG_INLINE void unite(RectI& dst, const RectI& src1, const RectI& src2)
  {
    int x1 = Math::min(src1.x, src2.x);
    int y1 = Math::min(src1.y, src2.y);
    int x2 = Math::max(src1.x + src1.w, src2.x + src2.w);
    int y2 = Math::max(src1.y + src1.h, src2.y + src2.h);

    dst.set(x1, y1, x2 - x1, y2 - y1);
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if given point is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(int px, int py) const
  {
    return ((px >= getX1()) & (py >= getY1()) & 
            (px <  getX2()) & (py <  getY2()) );
  }

  //! @brief Returns @c true if given point @a pt is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(const PointI& pt) const
  {
    return ((pt.x >= getX1()) & (pt.y >= getY1()) & 
            (pt.x <  getX2()) & (pt.y <  getY2()) );
  }

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if rectangle is valid.
  //!
  //! Rectangle is only valid if @c x2 is greater than @c x1 and @c y2 is
  //! greater than @c y1.
  FOG_INLINE bool isValid() const
  {
    return (w > 0) & (h > 0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(int px, int py, int pw, int ph) const
  {
    return (x == px) & (y == py) & (w == pw) & (h == ph);
  }

  FOG_INLINE bool eq(const RectI& other) const
  {
    if (sizeof(RectI) == 16)
      return Memory::eq16B(static_cast<const void*>(this), static_cast<const void*>(&other));
    else
      return (x == other.x) & (y == other.y) & (w == other.w) & (h == other.h);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrink rectangle by @c n.
  FOG_INLINE RectI& shrink(int n)
  {
    x += n;
    y += n;
    n <<= 1;
    w -= n;
    h -= n;
    
    return *this;
  }

  //! @brief Expand rectangle by @c n.
  FOG_INLINE RectI& expand(int n)
  {
    x -= n;
    y -= n;
    n <<= 1;
    w += n;
    h += n;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI& adjust(int px, int py)
  {
    x += px;
    y += py;
    w -= px;
    h -= py;

    w -= px;
    h -= py;

    return *this;
  }

  FOG_INLINE RectI& adjust(int px1, int py1, int px2, int py2)
  {
    x += px1;
    y += py1;
    w -= px1;
    h -= py1;

    w += px2;
    h += py2;

    return *this;
  }

  FOG_INLINE RectI adjusted(int px, int py) const
  {
    return RectI(x + px, y + py, w - px - px, h - py - py);
  }

  FOG_INLINE RectI adjusted(int px1, int py1, int px2, int py2) const
  {
    return RectI(x + px1, y + py1, w - px1 + px2, h - py1 + py2);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF toRectF() const;
  FOG_INLINE RectD toRectD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI& operator=(const RectI& other)
  {
    return set(other);
  }

  FOG_INLINE RectI& operator+=(const PointI& p)
  {
    return translate(p);
  }

  FOG_INLINE RectI& operator-=(const PointI& p)
  {
    x -= p.x;
    y -= p.y;
    return *this;
  }

  FOG_INLINE bool operator==(const RectI& other) const { return (x == other.x) & (y == other.y) & (w == other.w) & (h == other.h); }
  FOG_INLINE bool operator!=(const RectI& other) const { return (x != other.x) | (y != other.y) | (w != other.w) | (h != other.h); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int x;
  int y;
  int w;
  int h;
};

// ============================================================================
// [Fog::RectF]
// ============================================================================

//! @brief Rectangle (32-bit float version).
struct RectF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF()
  {
  }

  FOG_INLINE RectF(const RectF& other)
    : x(other.x), y(other.y), w(other.w), h(other.h)
  {
  }

  FOG_INLINE RectF(const PointF& pt0, const PointF& pt1)
    : x(pt0.x), y(pt0.y), w(pt1.x - pt0.x), h(pt1.y - pt0.y)
  {
  }

  FOG_INLINE RectF(const PointF& pt0, const SizeF& sz)
    : x(pt0.x), y(pt0.y), w(sz.w), h(sz.h)
  {
  }

  FOG_INLINE RectF(float rx, float ry, float rw, float rh)
    : x(rx), y(ry), w(rw), h(rh)
  {
  }

  FOG_INLINE explicit RectF(const RectI& other)
    : x((float)other.x), y((float)other.y), w((float)other.w), h((float)other.h)
  {
  }

  FOG_INLINE RectF(const PointI& pt0, const PointI& pt1)
    : x((float)pt0.x), y((float)pt0.y), w((float)pt1.x - (float)pt0.x), h((float)pt1.y - (float)pt0.y)
  {
  }

  FOG_INLINE RectF(const PointI& pt0, const SizeI& sz)
    : x((float)pt0.x), y((float)pt0.y), w((float)sz.w), h((float)sz.h)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getX() const { return x; }
  FOG_INLINE float getY() const { return y; }
  FOG_INLINE float getWidth() const { return w; }
  FOG_INLINE float getHeight() const { return h; }

  FOG_INLINE float getX1() const { return x; }
  FOG_INLINE float getY1() const { return y; }
  FOG_INLINE float getX2() const { return x + w; }
  FOG_INLINE float getY2() const { return y + h; }

  FOG_INLINE float getLeft() const { return x; }
  FOG_INLINE float getTop() const { return y; }
  FOG_INLINE float getRight() const { return x + w; }
  FOG_INLINE float getBottom() const { return y + h; }

  FOG_INLINE PointD getPosition() const { return PointD(x, y); }
  FOG_INLINE SizeD getSize() const { return SizeD(w, h); }

  FOG_INLINE RectF& set(const RectI &other)
  {
    x = (float)other.x; y = (float)other.y;
    w = (float)other.w; h = (float)other.h;
    return *this;
  }

  FOG_INLINE RectF& set(int rx, int ry, int rw, int rh)
  {
    x = (float)rx; y = (float)ry;
    w = (float)rw; h = (float)rh;
    return *this;
  }

  FOG_INLINE RectF& set(const RectF &other)
  {
    x = other.x; y = other.y;
    w = other.w; h = other.h;
    return *this;
  }

  FOG_INLINE RectF& set(float rx, float ry, float rw, float rh)
  {
    x = rx; y = ry;
    w = rw; h = rh;
    return *this;
  }

  FOG_INLINE RectF& setX(float x) { x = x; return *this; }
  FOG_INLINE RectF& setY(float y) { y = y; return *this; }
  FOG_INLINE RectF& setWidth(float w) { w = w; return *this; }
  FOG_INLINE RectF& setHeight(float h) { h = h; return *this; }

  FOG_INLINE RectF& setX1(float x1) { x = x1; return *this; }
  FOG_INLINE RectF& setY1(float y1) { y = y1; return *this; }
  FOG_INLINE RectF& setX2(float x2) { w = x2 - x; return *this; }
  FOG_INLINE RectF& setY2(float y2) { h = y2 - y; return *this; }

  FOG_INLINE RectF& setLeft(float x1) { x = x1; return *this; }
  FOG_INLINE RectF& setTop(float y1) { y = y1; return *this; }
  FOG_INLINE RectF& setRight(float x2) { w = x2 - x; return *this; }
  FOG_INLINE RectF& setBottom(float y2) { h = y2 - y; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF& reset()
  {
    x = 0.0f;
    y = 0.0f;
    w = 0.0f;
    h = 0.0f;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF& translate(const PointI& pt)
  {
    x += (float)pt.x;
    y += (float)pt.y;
    return *this;
  }

  FOG_INLINE RectF& translate(int px, int py)
  {
    x += (float)px;
    y += (float)py;
    return *this;
  }

  FOG_INLINE RectF& translate(const PointF& pt)
  {
    x += pt.x;
    y += pt.y;
    return *this;
  }

  FOG_INLINE RectF& translate(float px, float py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE RectF translated(const PointI& pt) const
  {
    return RectF(x + (float)pt.x, y + (float)pt.y, w, h);
  }

  FOG_INLINE RectF translated(int px, int py) const
  {
    return RectF(x + (float)px, y + (float)py, w, h);
  }

  FOG_INLINE RectF translated(const PointF& pt) const
  {
    return RectF(x + (float)pt.x, y + (float)pt.y, w, h);
  }

  FOG_INLINE RectF translated(float px, float py) const
  {
    return RectF(x + (float)px, y + (float)py, w, h);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF& adjust(const PointI& pt)
  {
    return adjust((float)pt.x, (float)pt.y);
  }

  FOG_INLINE RectF& adjust(int px, int py)
  {
    return adjust((float)px, (float)py);
  }

  FOG_INLINE RectF& adjust(const PointF& pt)
  {
    return adjust(pt.x, pt.y);
  }

  FOG_INLINE RectF& adjust(float px, float py)
  {
    x += px; y += py;
    w -= px; h -= py;
    w -= px; h -= py;

    return *this;
  }

  FOG_INLINE RectF adjusted(const PointI& pt) const
  {
    return adjusted((float)pt.x, (float)pt.y);
  }

  FOG_INLINE RectF adjusted(int px, int py) const
  {
    return adjusted((float)px, (float)py);
  }

  FOG_INLINE RectF adjusted(const PointF& pt) const
  {
    return adjusted((float)pt.x, (float)pt.y);
  }

  FOG_INLINE RectF adjusted(float px, float py) const
  {
    return RectF(x + px, y + py, w - px - px, h - py - py);
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap.
  FOG_INLINE bool overlaps(const RectF& r) const
  {
    return (Math::max(getX1(), r.getX1()) < Math::min(getX1(), r.getX2()) &&
            Math::max(getY1(), r.getY1()) < Math::min(getY2(), r.getY2()) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const RectF& r) const
  {
    return ((r.getX1() >= getX1()) & (r.getX2() <= getX2()) &
            (r.getY1() >= getY1()) & (r.getY2() <= getY2()) );
  }

  static FOG_INLINE bool intersect(RectF& dest, const RectF& src1, const RectF& src2)
  {
    dest.x = Math::max(src1.getX1(), src2.getX1());
    dest.y = Math::max(src1.getY1(), src2.getY1());
    dest.w = Math::min(src1.getX2(), src2.getX2()) - dest.x;
    dest.h = Math::min(src1.getY2(), src2.getY2()) - dest.y;

    return dest.isValid();
  }

  static FOG_INLINE void unite(RectF& dest, const RectF& src1, const RectF& src2)
  {
    dest.x = Math::min(src1.getX1(), src2.getX1());
    dest.y = Math::min(src1.getY1(), src2.getY1());
    dest.w = Math::max(src1.getX2(), src2.getX2()) - dest.x;
    dest.h = Math::max(src1.getY2(), src2.getY2()) - dest.y;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Get whether the point is in the rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(float px, float py) const
  {
    float ix = px - x;
    float iy = py - y;

    return ((ix >= 0.0f) & (ix < w) & (iy > 0.0f) & (iy < h));
  }

  //! @brief Get whether the point is in the rectangle.
  //! @brief pt Point coordinates.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(const PointF& other) const
  {
    float ix = other.x - x;
    float iy = other.y - y;

    return ((ix >= 0.0f) & (ix < w) & (iy >= 0.0f) & (iy < h));
  }

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  //! @brief Get whether the rectangle is valid.
  //!
  //! Rectangle is only valid if it's width and height is equal or larger
  //! than zero (it means that width and height is not negative).
  FOG_INLINE bool isValid() const
  {
    return ((w > 0.0f) & (h > 0.0f));
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(float px, float py, float pw, float ph) const
  {
    return (x == px) & (y == py) & (w == pw) & (h == ph);
  }

  FOG_INLINE bool eq(const RectF& other) const
  {
    return (x == other.x) & (y == other.y) & (w == other.w) & (h == other.h);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD toRectD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF& operator=(const RectI& other)
  {
    return set(other);
  }

  FOG_INLINE RectF& operator=(const RectF& other)
  {
    return set(other);
  }

  FOG_INLINE RectF& operator+=(const PointI& other)
  {
    return translate(other);
  }

  FOG_INLINE RectF& operator+=(const PointF& other)
  {
    return translate(other);
  }

  FOG_INLINE RectF& operator-=(const PointI& other)
  {
    x -= (float)other.x;
    y -= (float)other.y;
    return *this;
  }

  FOG_INLINE RectF& operator-=(const PointF& other)
  {
    x -= (float)other.x;
    y -= (float)other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const RectF& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const RectF& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float x;
  float y;
  float w;
  float h;
};

// ============================================================================
// [Fog::RectD]
// ============================================================================

//! @brief Rectangle (64-bit integer version).
struct RectD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD()
  {
  }

  FOG_INLINE RectD(const RectD& other)
    : x(other.x), y(other.y), w(other.w), h(other.h)
  {
  }

  FOG_INLINE RectD(const PointD& pt0, const PointD& pt1)
    : x(pt0.x), y(pt0.y), w(pt1.x - pt0.x), h(pt1.y - pt0.y)
  {
  }

  FOG_INLINE RectD(const PointD& pt0, const SizeD& sz)
    : x(pt0.x), y(pt0.y), w(sz.w), h(sz.h)
  {
  }

  FOG_INLINE RectD(double rx, double ry, double rw, double rh)
    : x(rx), y(ry), w(rw), h(rh)
  {
  }

  FOG_INLINE explicit RectD(const RectF& other)
    : x((double)other.x), y((double)other.y), w((double)other.w), h((double)other.h)
  {
  }

  FOG_INLINE RectD(const PointF& pt0, const PointF& pt1)
    : x((double)pt0.x), y((double)pt0.y), w((double)pt1.x - (double)pt0.x), h((double)pt1.y - (double)pt0.y)
  {
  }

  FOG_INLINE RectD(const PointF& pt0, const SizeF& sz)
    : x((double)pt0.x), y((double)pt0.y), w((double)sz.w), h((double)sz.h)
  {
  }

  FOG_INLINE explicit RectD(const RectI& other)
    : x((double)other.x), y((double)other.y), w((double)other.w), h((double)other.h)
  {
  }

  FOG_INLINE RectD(const PointI& pt0, const PointI& pt1)
    : x((double)pt0.x), y((double)pt0.y), w((double)pt1.x - (double)pt0.x), h((double)pt1.y - (double)pt0.y)
  {
  }

  FOG_INLINE RectD(const PointI& pt0, const SizeI& sz)
    : x((double)pt0.x), y((double)pt0.y), w((double)sz.w), h((double)sz.h)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getX() const { return x; }
  FOG_INLINE double getY() const { return y; }
  FOG_INLINE double getWidth() const { return w; }
  FOG_INLINE double getHeight() const { return h; }

  FOG_INLINE double getX1() const { return x; }
  FOG_INLINE double getY1() const { return y; }
  FOG_INLINE double getX2() const { return x + w; }
  FOG_INLINE double getY2() const { return y + h; }

  FOG_INLINE double getLeft() const { return x; }
  FOG_INLINE double getTop() const { return y; }
  FOG_INLINE double getRight() const { return x + w; }
  FOG_INLINE double getBottom() const { return y + h; }

  FOG_INLINE PointD getPosition() const { return PointD(x, y); }
  FOG_INLINE SizeD getSize() const { return SizeD(w, h); }

  FOG_INLINE RectD& set(const RectI &other)
  {
    x = (double)other.x; y = (double)other.y;
    w = (double)other.w; h = (double)other.h;
    return *this;
  }

  FOG_INLINE RectD& set(int rx, int ry, int rw, int rh)
  {
    x = (double)rx; y = (double)ry;
    w = (double)rw; h = (double)rh;
    return *this;
  }

  FOG_INLINE RectD& set(const RectF &other)
  {
    x = (double)other.x; y = (double)other.y;
    w = (double)other.w; h = (double)other.h;
    return *this;
  }

  FOG_INLINE RectD& set(float rx, float ry, float rw, float rh)
  {
    x = (double)rx; y = (double)ry;
    w = (double)rw; h = (double)rh;
    return *this;
  }

  FOG_INLINE RectD& set(const RectD &other)
  {
    x = other.x; y = other.y;
    w = other.w; h = other.h;
    return *this;
  }

  FOG_INLINE RectD& set(double rx, double ry, double rw, double rh)
  {
    x = rx; y = ry;
    w = rw; h = rh;
    return *this;
  }

  FOG_INLINE RectD& setX(double x) { x = x; return *this; }
  FOG_INLINE RectD& setY(double y) { y = y; return *this; }
  FOG_INLINE RectD& setWidth(double w) { w = w; return *this; }
  FOG_INLINE RectD& setHeight(double h) { h = h; return *this; }

  FOG_INLINE RectD& setX1(double x1) { x = x1; return *this; }
  FOG_INLINE RectD& setY1(double y1) { y = y1; return *this; }
  FOG_INLINE RectD& setX2(double x2) { w = x2 - x; return *this; }
  FOG_INLINE RectD& setY2(double y2) { h = y2 - y; return *this; }

  FOG_INLINE RectD& setLeft(double x1) { x = x1; return *this; }
  FOG_INLINE RectD& setTop(double y1) { y = y1; return *this; }
  FOG_INLINE RectD& setRight(double x2) { w = x2 - x; return *this; }
  FOG_INLINE RectD& setBottom(double y2) { h = y2 - y; return *this; }

  // --------------------------------------------------------------------------
  // [reset]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD& reset() 
  {
    x = 0.0;
    y = 0.0;
    w = 0.0;
    h = 0.0;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD& translate(const PointI& pt)
  {
    x += (double)pt.x;
    y += (double)pt.y;
    return *this;
  }

  FOG_INLINE RectD& translate(int px, int py)
  {
    x += (double)px;
    y += (double)py;
    return *this;
  }

  FOG_INLINE RectD& translate(const PointF& pt)
  {
    x += (double)pt.x;
    y += (double)pt.y;
    return *this;
  }

  FOG_INLINE RectD& translate(float px, float py)
  {
    x += (double)px;
    y += (double)py;
    return *this;
  }

  FOG_INLINE RectD& translate(const PointD& pt)
  {
    x += pt.x;
    y += pt.y;
    return *this;
  }

  FOG_INLINE RectD& translate(double px, double py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE RectD translated(const PointI& pt) const
  {
    return RectD(x + (double)pt.x, y + (double)pt.y, w, h);
  }

  FOG_INLINE RectD translated(int px, int py) const
  {
    return RectD(x + (double)px, y + (double)py, w, h);
  }

  FOG_INLINE RectD translated(const PointF& pt) const
  {
    return RectD(x + (double)pt.x, y + (double)pt.y, w, h);
  }

  FOG_INLINE RectD translated(float px, float py) const
  {
    return RectD(x + (double)px, y + (double)py, w, h);
  }

  FOG_INLINE RectD translated(const PointD& pt) const
  {
    return RectD(x + pt.x, y + pt.y, w, h);
  }

  FOG_INLINE RectD translated(double px, double py) const
  {
    return RectD(x + px, y + py, w, h);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD& adjust(const PointI& pt)
  {
    return adjust((double)pt.x, (double)pt.y);
  }

  FOG_INLINE RectD& adjust(int px, int py)
  {
    return adjust((double)px, (double)py);
  }

  FOG_INLINE RectD& adjust(const PointF& pt)
  {
    return adjust((double)pt.x, (double)pt.y);
  }

  FOG_INLINE RectD& adjust(float px, float py)
  {
    return adjust((double)px, (double)py);
  }

  FOG_INLINE RectD& adjust(const PointD& pt)
  {
    return adjust(pt.x, pt.y);
  }

  FOG_INLINE RectD& adjust(double px, double py)
  {
    x += px; y += py;
    w -= px; h -= py;
    w -= px; h -= py;

    return *this;
  }

  FOG_INLINE RectD adjusted(const PointI& pt) const
  {
    return adjusted((double)pt.x, (double)pt.y);
  }

  FOG_INLINE RectD adjusted(int px, int py) const
  {
    return adjusted((double)px, (double)py);
  }

  FOG_INLINE RectD adjusted(const PointF& pt) const
  {
    return adjusted((double)pt.x, (double)pt.y);
  }

  FOG_INLINE RectD adjusted(float px, float py) const
  {
    return adjusted((double)px, (double)py);
  }

  FOG_INLINE RectD adjusted(const PointD& pt) const
  {
    return adjusted(pt.x, pt.y);
  }

  FOG_INLINE RectD adjusted(double px, double py) const
  {
    return RectD(x + px, y + py, w - px - px, h - py - py);
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap.
  FOG_INLINE bool overlaps(const RectD& r) const
  {
    return (Math::max(getX1(), r.getX1()) < Math::min(getX1(), r.getX2()) &&
            Math::max(getY1(), r.getY1()) < Math::min(getY2(), r.getY2()) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const RectD& r) const
  {
    return ((r.getX1() >= getX1()) & (r.getX2() <= getX2()) &
            (r.getY1() >= getY1()) & (r.getY2() <= getY2()) );
  }

  static FOG_INLINE bool intersect(RectD& dest, const RectD& src1, const RectD& src2)
  {
    dest.x = Math::max(src1.getX1(), src2.getX1());
    dest.y = Math::max(src1.getY1(), src2.getY1());
    dest.w = Math::min(src1.getX2(), src2.getX2()) - dest.x;
    dest.h = Math::min(src1.getY2(), src2.getY2()) - dest.y;

    return dest.isValid();
  }

  static FOG_INLINE void unite(RectD& dest, const RectD& src1, const RectD& src2)
  {
    dest.x = Math::min(src1.getX1(), src2.getX1());
    dest.y = Math::min(src1.getY1(), src2.getY1());
    dest.w = Math::max(src1.getX2(), src2.getX2()) - dest.x;
    dest.h = Math::max(src1.getY2(), src2.getY2()) - dest.y;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Get whether the point is in the rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(double px, double py) const
  {
    double ix = px - x;
    double iy = py - y;

    return ((ix >= 0) & (ix < w) & (iy > 0) & (iy < h));
  }

  //! @brief Get whether the point is in the rectangle.
  //! @brief pt Point coordinates.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(const PointD& other) const
  {
    double ix = other.x - x;
    double iy = other.y - y;

    return ((ix >= 0) & (ix < w) & (iy >= 0) & (iy < h));
  }

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  //! @brief Get whether the rectangle is valid.
  //!
  //! Rectangle is only valid if it's width and height is equal or larger
  //! than zero (it means that width and height is not negative).
  FOG_INLINE bool isValid() const
  {
    return ((w > 0.0) & (h > 0.0));
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(double px, double py, double pw, double ph) const
  {
    return (x == px) & (y == py) & (w == pw) & (h == ph);
  }

  FOG_INLINE bool eq(const RectD& other) const
  {
    return (x == other.x) & (y == other.y) & (w == other.w) & (h == other.h);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF toRectF() const { return RectF((float)x, (float)y, (float)w, (float)h); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD& operator=(const RectI& other)
  {
    return set(other);
  }

  FOG_INLINE RectD& operator=(const RectF& other)
  {
    return set(other);
  }

  FOG_INLINE RectD& operator=(const RectD& other)
  {
    return set(other);
  }

  FOG_INLINE RectD& operator+=(const PointI& other)
  {
    return translate(other);
  }

  FOG_INLINE RectD& operator+=(const PointF& other)
  {
    return translate(other);
  }

  FOG_INLINE RectD& operator+=(const PointD& other)
  {
    return translate(other);
  }

  FOG_INLINE RectD& operator-=(const PointI& other)
  {
    x -= (double)other.x;
    y -= (double)other.y;
    return *this;
  }

  FOG_INLINE RectD& operator-=(const PointF& other)
  {
    x -= (double)other.x;
    y -= (double)other.y;
    return *this;
  }

  FOG_INLINE RectD& operator-=(const PointD& other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const RectD& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const RectD& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double x;
  double y;
  double w;
  double h;
};

// ============================================================================
// [Fog::BoxI]
// ============================================================================

//! @brief Box (32-bit integer version).
struct BoxI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxI()
  {
  }

  FOG_INLINE BoxI(int px1, int py1, int px2, int py2) :
    x1(px1), y1(py1), x2(px2), y2(py2)
  {
  }

  FOG_INLINE BoxI(const BoxI& other) :
    x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2)
  {
  }

  FOG_INLINE explicit BoxI(const RectI& other) : 
    x1(other.x), y1(other.y), x2(other.x + other.w), y2(other.y + other.h)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getX() const { return x1; }
  FOG_INLINE int getY() const { return y1; }
  FOG_INLINE int getWidth() const { return x2 - x1; }
  FOG_INLINE int getHeight() const { return y2 - y1; }

  FOG_INLINE int getX1() const { return x1; }
  FOG_INLINE int getY1() const { return y1; }
  FOG_INLINE int getX2() const { return x2; }
  FOG_INLINE int getY2() const { return y2; }

  FOG_INLINE PointI getPosition() const { return PointI(x1, y1); }
  FOG_INLINE SizeI getSize() const { return SizeI(x2 - x1, y2 - y1); }

  FOG_INLINE BoxI& set(const BoxI &other)
  { 
    if (sizeof(BoxI) == 16)
    {
      Memory::copy16B(static_cast<void*>(this), static_cast<const void*>(&other));
    }
    else
    {
      x1 = other.x1;
      y1 = other.y1;
      x2 = other.x2;
      y2 = other.y2;
    }
    return *this;
  }
  
  FOG_INLINE BoxI& set(int px1, int py1, int px2, int py2)
  { 
    x1 = px1; 
    y1 = py1; 
    x2 = px2; 
    y2 = py2;

    return *this;
  }

  FOG_INLINE BoxI& setX(int x) { x1 = x; return *this; }
  FOG_INLINE BoxI& setY(int y) { y1 = y; return *this; }
  FOG_INLINE BoxI& setWidth(int w) { x2 = x1 + w; return *this; }
  FOG_INLINE BoxI& setHeight(int h) { y2 = y1 + h; return *this; }

  FOG_INLINE BoxI& setX1(int px1) { x1 = px1; return *this; }
  FOG_INLINE BoxI& setY1(int py1) { y1 = py1; return *this; }
  FOG_INLINE BoxI& setX2(int px2) { x2 = px2; return *this; }
  FOG_INLINE BoxI& setY2(int py2) { y2 = py2; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxI& reset()
  {
    if (sizeof(BoxI) == 16)
    {
      Memory::zero16B(static_cast<void*>(this));
    }
    else
    {
      x1 = 0;
      y1 = 0;
      x2 = 0;
      y2 = 0;
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxI& translate(int px, int py)
  { 
    x1 += px;
    y1 += py;
    x2 += px;
    y2 += py;
    return *this;
  }

  FOG_INLINE BoxI& translate(const PointI& pt)
  { 
    x1 += pt.x;
    y1 += pt.y;
    x2 += pt.x;
    y2 += pt.y;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap. Remember, x2() and y2() coords aren't in the rectangle.
  FOG_INLINE bool overlaps(const BoxI& r) const
  {
    return (( ((y1 - r.y2) ^ (y2 - r.y1)) &
              ((x1 - r.x2) ^ (x2 - r.x1)) ) < 0);
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const BoxI& r) const
  {
    return ((r.x1 >= x1) & (r.x2 <= x2) &
            (r.y1 >= y1) & (r.y2 <= y2) );
  }

  static FOG_INLINE bool intersect(BoxI& dest, const BoxI& src1, const BoxI& src2)
  {
#if defined(FOG_HARDCODE_SSE2)
    FOG_ALIGNED_VAR(static const uint32_t, c0[4], 16) = { 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF };

    __m128i xmm0 = _mm_loadu_si128((__m128i *)(&src1));
    __m128i xmm1 = _mm_loadu_si128((__m128i *)(&src2));
    __m128i xmm2 = _mm_xor_si128(_mm_cmpgt_epi32(xmm0, xmm1), _mm_load_si128((__m128i *)(void*)(&c0[0])));
    xmm0 = _mm_and_si128(xmm0, xmm2);
    xmm2 = _mm_andnot_si128(xmm2, xmm1);
    _mm_storeu_si128((__m128i *)(&dest), _mm_or_si128(xmm0, xmm2));
#else
    dest.set(Math::max(src1.x1, src2.x1),
             Math::max(src1.y1, src2.y1),
             Math::min(src1.x2, src2.x2),
             Math::min(src1.y2, src2.y2));
#endif // FOG_HARDCODE_SSE2
    return dest.isValid();
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if given point is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(int px, int py) const
  {
    return ((px >= x1) & (py >= y1) & (px < x2) & (py < y2));
  }

  //! @brief Returns @c true if given point @a pt is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(const PointI& pt) const
  {
    return ((pt.x >= x1) & (pt.y >= y1) & (pt.x < x2) & (pt.y < y2));
  }

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if rectangle is valid.
  //!
  //! Rectangle is only valid if @c x2 is greater than @c x1 and @c y2 is
  //! greater than @c y1.
  FOG_INLINE bool isValid() const
  {
    return (x2 > x1) & (y2 > y1);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(int px1, int py1, int px2, int py2) const
  {
    return (x1 == px1) & (y1 == py1) & (x2 == px2) & (y2 == py2);
  }

  FOG_INLINE bool eq(const BoxI& other) const
  {
    if (sizeof(BoxI) == 16)
      return Memory::eq16B(static_cast<const void*>(this), static_cast<const void*>(&other));
    else
      return (x1 == other.x1) & (y1 == other.y1) & (x2 == other.x2) & (y2 == other.y2);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrinks rectangle by @c n coordinates.
  FOG_INLINE BoxI& shrink(int n)
  {
    x1 += n;
    y1 += n;
    x2 -= n;
    y2 -= n;
    
    return *this;
  }

  //! @brief Expands rectangle by @c n coordinates.
  FOG_INLINE BoxI& expand(int n)
  {
    x1 -= n;
    y1 -= n;
    x2 += n;
    y2 += n;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI toRectI() const { return RectI(x1, y1, x2 - x1, y2 - y1); }
  FOG_INLINE RectF toRectF() const { return RectF((float)x1, (float)y1, (float)(x2 - x1), (float)(y2 - y1)); }
  FOG_INLINE RectD toRectD() const { return RectD(x1, y1, x2 - x1, y2 - y1); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxI& operator=(const BoxI& other)
  {
    return set(other);
  }

  FOG_INLINE BoxI& operator+=(const PointI& pt)
  {
    return translate(pt);
  }

  FOG_INLINE BoxI& operator-=(const PointI& pt)
  {
    x1 -= pt.x;
    y1 -= pt.y;
    x2 -= pt.x;
    y2 -= pt.y;
    return *this;
  }

  FOG_INLINE bool operator==(const BoxI& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const BoxI& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int x1;
  int y1;
  int x2;
  int y2;
};

// ============================================================================
// [Fog::BoxF]
// ============================================================================

//! @brief Box (32-bit float version).
struct BoxF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxF()
  {
  }

  FOG_INLINE BoxF(float px1, float py1, float px2, float py2) :
    x1(px1), y1(py1), x2(px2), y2(py2)
  {
  }

  FOG_INLINE BoxF(const BoxF& other) :
    x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2)
  {
  }

  FOG_INLINE explicit BoxF(const RectF& other) : 
    x1(other.x), y1(other.y), x2(other.x + other.w), y2(other.y + other.h)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getX() const { return x1; }
  FOG_INLINE float getY() const { return y1; }
  FOG_INLINE float getWidth() const { return x2 - x1; }
  FOG_INLINE float getHeight() const { return y2 - y1; }

  FOG_INLINE float getX1() const { return x1; }
  FOG_INLINE float getY1() const { return y1; }
  FOG_INLINE float getX2() const { return x2; }
  FOG_INLINE float getY2() const { return y2; }

  FOG_INLINE PointF getPosition() const { return PointF(x1, y1); }
  FOG_INLINE SizeF getSize() const { return SizeF(x2 - x1, y2 - y1); }

  FOG_INLINE BoxF& set(const BoxF &other)
  { 
    if (sizeof(BoxF) == 16)
    {
      Memory::copy16B(static_cast<void*>(this), static_cast<const void*>(&other));
    }
    else
    {
      x1 = other.x1;
      y1 = other.y1;
      x2 = other.x2;
      y2 = other.y2;
    }
    return *this;
  }
  
  FOG_INLINE BoxF& set(float px1, float py1, float px2, float py2)
  { 
    x1 = px1; 
    y1 = py1; 
    x2 = px2; 
    y2 = py2;

    return *this;
  }

  FOG_INLINE BoxF& setX(float x) { x1 = x; return *this; }
  FOG_INLINE BoxF& setY(float y) { y1 = y; return *this; }
  FOG_INLINE BoxF& setWidth(float w) { x2 = x1 + w; return *this; }
  FOG_INLINE BoxF& setHeight(float h) { y2 = y1 + h; return *this; }

  FOG_INLINE BoxF& setX1(float px1) { x1 = px1; return *this; }
  FOG_INLINE BoxF& setY1(float py1) { y1 = py1; return *this; }
  FOG_INLINE BoxF& setX2(float px2) { x2 = px2; return *this; }
  FOG_INLINE BoxF& setY2(float py2) { y2 = py2; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxF& reset()
  {
    x1 = 0.0f;
    y1 = 0.0f;
    x2 = 0.0f;
    y2 = 0.0f;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxF& translate(float px, float py)
  { 
    x1 += px;
    y1 += py;
    x2 += px;
    y2 += py;
    return *this;
  }

  FOG_INLINE BoxF& translate(const PointF& pt)
  { 
    x1 += pt.x;
    y1 += pt.y;
    x2 += pt.x;
    y2 += pt.y;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap. Remember, x2() and y2() coords aren't in the rectangle.
  FOG_INLINE bool overlaps(const BoxF& r) const
  {
    return ((r.x1 >= x1) | (r.x2 <= x2) |
            (r.y1 >= y1) | (r.y2 <= y2) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const BoxF& r) const
  {
    return ((r.x1 >= x1) & (r.x2 <= x2) &
            (r.y1 >= y1) & (r.y2 <= y2) );
  }

  static FOG_INLINE bool intersect(BoxF& dest, const BoxF& src1, const BoxF& src2)
  {
    dest.set(Math::max(src1.x1, src2.x1),
             Math::max(src1.y1, src2.y1),
             Math::min(src1.x2, src2.x2),
             Math::min(src1.y2, src2.y2));
    return dest.isValid();
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if given point is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(float px, float py) const
  {
    return ((px >= x1) & (py >= y1) & (px < x2) & (py < y2));
  }

  //! @brief Returns @c true if given point @a pt is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(const PointF& pt) const
  {
    return ((pt.x >= x1) & (pt.y >= y1) & (pt.x < x2) & (pt.y < y2));
  }

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if rectangle is valid.
  //!
  //! Rectangle is only valid if @c x2 is greater than @c x1 and @c y2 is
  //! greater than @c y1.
  FOG_INLINE bool isValid() const
  {
    return (x2 > x1) & (y2 > y1);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(float px1, float py1, float px2, float py2) const
  {
    return (x1 == px1) & (y1 == py1) & (x2 == px2) & (y2 == py2);
  }

  FOG_INLINE bool eq(const BoxF& other) const
  {
    return (x1 == other.x1) & (y1 == other.y1) & (x2 == other.x2) & (y2 == other.y2);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrinks rectangle by @c n coordinates.
  FOG_INLINE BoxF& shrink(float n)
  {
    x1 += n;
    y1 += n;
    x2 -= n;
    y2 -= n;
    
    return *this;
  }

  //! @brief Expands rectangle by @c n coordinates.
  FOG_INLINE BoxF& expand(float n)
  {
    x1 -= n;
    y1 -= n;
    x2 += n;
    y2 += n;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI toRectI() const { return RectI((int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1)); }
  FOG_INLINE RectF toRectF() const { return RectF(x1, y1, x2 - x1, y2 - y1); }
  FOG_INLINE RectD toRectD() const { return RectD(x1, y1, x2 - x1, y2 - y1); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxF& operator=(const BoxF& other)
  {
    return set(other);
  }

  FOG_INLINE BoxF& operator+=(const PointF& pt)
  {
    return translate(pt);
  }

  FOG_INLINE BoxF& operator-=(const PointF& pt)
  {
    x1 -= pt.x;
    y1 -= pt.y;
    x2 -= pt.x;
    y2 -= pt.y;
    return *this;
  }

  FOG_INLINE bool operator==(const BoxF& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const BoxF& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float x1;
  float y1;
  float x2;
  float y2;
};

// ============================================================================
// [Fog::BoxD]
// ============================================================================

//! @brief Box (64-bit float version).
struct BoxD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxD()
  {
  }

  FOG_INLINE BoxD(double px1, double py1, double px2, double py2) :
    x1(px1), y1(py1), x2(px2), y2(py2)
  {
  }

  FOG_INLINE BoxD(const BoxD& other) :
    x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2)
  {
  }

  FOG_INLINE explicit BoxD(const BoxF& other) :
    x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2)
  {
  }

  FOG_INLINE explicit BoxD(const BoxI& other) :
    x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2)
  {
  }

  FOG_INLINE BoxD(const RectD& other) : 
    x1(other.x), y1(other.y), x2(other.x + other.w), y2(other.y + other.h)
  {
  }

  FOG_INLINE explicit BoxD(const RectF& other) : 
    x1(other.x), y1(other.y), x2(other.x + other.w), y2(other.y + other.h)
  {
  }

  FOG_INLINE explicit BoxD(const RectI& other) : 
    x1(other.x), y1(other.y), x2(other.x + other.w), y2(other.y + other.h)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getX() const { return x1; }
  FOG_INLINE double getY() const { return y1; }
  FOG_INLINE double getWidth() const { return x2 - x1; }
  FOG_INLINE double getHeight() const { return y2 - y1; }

  FOG_INLINE double getX1() const { return x1; }
  FOG_INLINE double getY1() const { return y1; }
  FOG_INLINE double getX2() const { return x2; }
  FOG_INLINE double getY2() const { return y2; }

  FOG_INLINE PointD getPosition() const { return PointD(x1, y1); }
  FOG_INLINE SizeD getSize() const { return SizeD(x2 - x1, y2 - y1); }

  FOG_INLINE BoxD& set(const BoxD &other)
  { 
    if (sizeof(BoxD) == 32)
    {
      Memory::copy32B(static_cast<void*>(this), static_cast<const void*>(&other));
    }
    else
    {
      x1 = other.x1;
      y1 = other.y1;
      x2 = other.x2;
      y2 = other.y2;
    }
    return *this;
  }
  
  FOG_INLINE BoxD& set(const BoxF &other)
  { 
    x1 = other.x1;
    y1 = other.y1;
    x2 = other.x2;
    y2 = other.y2;
    return *this;
  }

  FOG_INLINE BoxD& set(const BoxI &other)
  { 
    x1 = other.x1;
    y1 = other.y1;
    x2 = other.x2;
    y2 = other.y2;
    return *this;
  }

  FOG_INLINE BoxD& set(double px1, double py1, double px2, double py2)
  { 
    x1 = px1; 
    y1 = py1; 
    x2 = px2; 
    y2 = py2;

    return *this;
  }

  FOG_INLINE BoxD& setX(double x) { x1 = x; return *this; }
  FOG_INLINE BoxD& setY(double y) { y1 = y; return *this; }
  FOG_INLINE BoxD& setWidth(double w) { x2 = x1 + w; return *this; }
  FOG_INLINE BoxD& setHeight(double h) { y2 = y1 + h; return *this; }

  FOG_INLINE BoxD& setX1(double px1) { x1 = px1; return *this; }
  FOG_INLINE BoxD& setY1(double py1) { y1 = py1; return *this; }
  FOG_INLINE BoxD& setX2(double px2) { x2 = px2; return *this; }
  FOG_INLINE BoxD& setY2(double py2) { y2 = py2; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxD& reset()
  {
    x1 = 0.0;
    y1 = 0.0;
    x2 = 0.0;
    y2 = 0.0;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxD& translate(double px, double py)
  { 
    x1 += px;
    y1 += py;
    x2 += px;
    y2 += py;
    return *this;
  }

  FOG_INLINE BoxD& translate(const PointD& pt)
  { 
    x1 += pt.x;
    y1 += pt.y;
    x2 += pt.x;
    y2 += pt.y;
    return *this;
  }

  FOG_INLINE BoxD& translate(const PointI& pt)
  { 
    x1 += pt.x;
    y1 += pt.y;
    x2 += pt.x;
    y2 += pt.y;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap. Remember, x2() and y2() coords aren't in the rectangle.
  FOG_INLINE bool overlaps(const BoxD& r) const
  {
    return ((r.x1 >= x1) | (r.x2 <= x2) |
            (r.y1 >= y1) | (r.y2 <= y2) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const BoxD& r) const
  {
    return ((r.x1 >= x1) & (r.x2 <= x2) &
            (r.y1 >= y1) & (r.y2 <= y2) );
  }

  static FOG_INLINE bool intersect(BoxD& dest, const BoxD& src1, const BoxD& src2)
  {
    dest.set(Math::max(src1.x1, src2.x1),
             Math::max(src1.y1, src2.y1),
             Math::min(src1.x2, src2.x2),
             Math::min(src1.y2, src2.y2));
    return dest.isValid();
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if given point is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(double px, double py) const
  {
    return ((px >= x1) & (py >= y1) & (px < x2) & (py < y2));
  }

  //! @brief Returns @c true if given point @a pt is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(const PointD& pt) const
  {
    return ((pt.x >= x1) & (pt.y >= y1) & (pt.x < x2) & (pt.y < y2));
  }

  //! @overload
  FOG_INLINE bool contains(const PointF& pt) const
  {
    double px = pt.x;
    double py = pt.y;

    return ((px >= x1) & (py >= y1) & (px < x2) & (py < y2));
  }

  //! @overload
  FOG_INLINE bool contains(const PointI& pt) const
  {
    double px = pt.x;
    double py = pt.y;

    return ((px >= x1) & (py >= y1) & (px < x2) & (py < y2));
  }

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if rectangle is valid.
  //!
  //! Rectangle is only valid if @c x2 is greater than @c x1 and @c y2 is
  //! greater than @c y1.
  FOG_INLINE bool isValid() const
  {
    return (x2 > x1) & (y2 > y1);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(double px1, double py1, double px2, double py2) const
  {
    return (x1 == px1) & (y1 == py1) & (x2 == px2) & (y2 == py2);
  }

  FOG_INLINE bool eq(const BoxD& other) const
  {
    return (x1 == other.x1) & (y1 == other.y1) & (x2 == other.x2) & (y2 == other.y2);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrinks rectangle by @c n coordinates.
  FOG_INLINE BoxD& shrink(double n)
  {
    x1 += n;
    y1 += n;
    x2 -= n;
    y2 -= n;
    
    return *this;
  }

  //! @brief Expands rectangle by @c n coordinates.
  FOG_INLINE BoxD& expand(double n)
  {
    x1 -= n;
    y1 -= n;
    x2 += n;
    y2 += n;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI toRectI() const { return RectI((int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1)); }
  FOG_INLINE RectF toRectF() const { return RectF((float)x1, (float)y1, (float)(x2 - x1), (float)(y2 - y1)); }
  FOG_INLINE RectD toRectD() const { return RectD(x1, y1, x2 - x1, y2 - y1); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxD& operator=(const BoxD& other) { return set(other); }
  FOG_INLINE BoxD& operator=(const BoxF& other) { return set(other); }
  FOG_INLINE BoxD& operator=(const BoxI& other) { return set(other); }

  FOG_INLINE BoxD& operator+=(const PointD& pt) { return translate(pt); }
  FOG_INLINE BoxD& operator+=(const PointF& pt) { return translate(pt); }
  FOG_INLINE BoxD& operator+=(const PointI& pt) { return translate(pt); }

  FOG_INLINE BoxD& operator-=(const PointD& pt)
  {
    x1 -= pt.x; y1 -= pt.y;
    x2 -= pt.x; y2 -= pt.y;
    return *this;
  }

  FOG_INLINE BoxD& operator-=(const PointF& pt)
  {
    x1 -= pt.x; y1 -= pt.y;
    x2 -= pt.x; y2 -= pt.y;
    return *this;
  }

  FOG_INLINE BoxD& operator-=(const PointI& pt)
  {
    x1 -= pt.x; y1 -= pt.y;
    x2 -= pt.x; y2 -= pt.y;
    return *this;
  }

  FOG_INLINE bool operator==(const BoxD& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const BoxD& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double x1;
  double y1;
  double x2;
  double y2;
};

// ============================================================================
// [Defined Later]
// ============================================================================

FOG_INLINE PointF PointI::toPointF() const { return PointF((float)x, (float)y); }
FOG_INLINE PointD PointI::toPointD() const { return PointD((double)x, (double)y); }
FOG_INLINE PointD PointF::toPointD() const { return PointD((double)x, (double)y); }

FOG_INLINE SizeF SizeI::toSizeF() const { return SizeF((float)w, (float)h); }
FOG_INLINE SizeD SizeI::toSizeD() const { return SizeD((double)w, (double)h); }
FOG_INLINE SizeD SizeF::toSizeD() const { return SizeD((double)w, (double)h); }

FOG_INLINE RectF RectI::toRectF() const { return RectF((float)x, (float)y, (float)w, (float)h); }
FOG_INLINE RectD RectI::toRectD() const { return RectD((double)x, (double)y, (double)w, (double)h); }
FOG_INLINE RectD RectF::toRectD() const { return RectD((double)x, (double)y, (double)w, (double)h); }

FOG_INLINE RectI::RectI(const BoxI& box) : x(box.x1), y(box.y1), w(box.x2 - box.x1), h(box.y2 - box.y1) {}

//! @}

} // Fog namespace

// ============================================================================
// [Operator Overload]
// ============================================================================

FOG_INLINE Fog::PointI operator+(const Fog::PointI& a, const Fog::PointI& b) { return Fog::PointI(a.x + b.x, a.y + b.y); }
FOG_INLINE Fog::PointI operator-(const Fog::PointI& a, const Fog::PointI& b) { return Fog::PointI(a.x - b.x, a.y - b.y); }

FOG_INLINE Fog::PointF operator+(const Fog::PointF& a, const Fog::PointF& b) { return Fog::PointF(a.x + b.x, a.y + b.y); }
FOG_INLINE Fog::PointF operator-(const Fog::PointF& a, const Fog::PointF& b) { return Fog::PointF(a.x - b.x, a.y - b.y); }

FOG_INLINE Fog::PointD operator+(const Fog::PointD& a, const Fog::PointD& b) { return Fog::PointD(a.x + b.x, a.y + b.y); }
FOG_INLINE Fog::PointD operator-(const Fog::PointD& a, const Fog::PointD& b) { return Fog::PointD(a.x - b.x, a.y - b.y); }

FOG_INLINE Fog::SizeI operator+(const Fog::SizeI& a, const Fog::SizeI& b) { return Fog::SizeI(a.w + b.w, a.h + b.h); }
FOG_INLINE Fog::SizeI operator-(const Fog::SizeI& a, const Fog::SizeI& b) { return Fog::SizeI(a.w - b.w, a.h - b.h); }

FOG_INLINE Fog::SizeF operator+(const Fog::SizeF& a, const Fog::SizeF& b) { return Fog::SizeF(a.w + b.w, a.h + b.h); }
FOG_INLINE Fog::SizeF operator-(const Fog::SizeF& a, const Fog::SizeF& b) { return Fog::SizeF(a.w - b.w, a.h - b.h); }

FOG_INLINE Fog::SizeD operator+(const Fog::SizeD& a, const Fog::SizeD& b) { return Fog::SizeD(a.w + b.w, a.h + b.h); }
FOG_INLINE Fog::SizeD operator-(const Fog::SizeD& a, const Fog::SizeD& b) { return Fog::SizeD(a.w - b.w, a.h - b.h); }

FOG_INLINE Fog::RectI operator+(const Fog::RectI& a, const Fog::PointI& b) { return Fog::RectI(a.x + b.x, a.y + b.y, a.w, a.h); }
FOG_INLINE Fog::RectI operator-(const Fog::RectI& a, const Fog::PointI& b) { return Fog::RectI(a.x - b.x, a.y - b.y, a.w, a.h); }

FOG_INLINE Fog::RectF operator+(const Fog::RectF& a, const Fog::PointF& b) { return Fog::RectF(a.x + b.x, a.y + b.y, a.w, a.h); }
FOG_INLINE Fog::RectF operator-(const Fog::RectF& a, const Fog::PointF& b) { return Fog::RectF(a.x - b.x, a.y - b.y, a.w, a.h); }

FOG_INLINE Fog::RectD operator+(const Fog::RectD& a, const Fog::PointD& b) { return Fog::RectD(a.x + b.x, a.y + b.y, a.w, a.h); }
FOG_INLINE Fog::RectD operator-(const Fog::RectD& a, const Fog::PointD& b) { return Fog::RectD(a.x - b.x, a.y - b.y, a.w, a.h); }

FOG_INLINE Fog::BoxI operator+(const Fog::BoxI& a, const Fog::PointI& b) { return Fog::BoxI(a.x1 + b.x, a.y1 + b.y, a.x2 + b.x, a.y2 + b.y); }
FOG_INLINE Fog::BoxI operator-(const Fog::BoxI& a, const Fog::PointI& b) { return Fog::BoxI(a.x1 - b.x, a.y1 - b.y, a.x2 - b.x, a.y2 - b.y); }

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::BoxF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::BoxI, Fog::TYPEINFO_PRIMITIVE)

FOG_DECLARE_TYPEINFO(Fog::PointD, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::PointF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::PointI, Fog::TYPEINFO_PRIMITIVE)

FOG_DECLARE_TYPEINFO(Fog::SizeD, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::SizeF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::SizeI, Fog::TYPEINFO_PRIMITIVE)

FOG_DECLARE_TYPEINFO(Fog::RectD, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::RectF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::RectI, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GRAPHICS_GEOMETRY_H
