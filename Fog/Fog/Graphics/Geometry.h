// [Fog-Graphics Library - Public API]
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

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct IntBox;
struct IntPoint;
struct IntRect;
struct IntSize;

struct FloatPoint;
struct FloatRect;
struct FloatSize;

struct DoublePoint;
struct DoubleRect;
struct DoubleSize;

// ============================================================================
// [Rules]
// ============================================================================

// This file includes several implementations of common geometric structures:
// - Point  - [x, y].
// - Size   - [width, height].
// - Rect   - [x, y] and [width height].
// - Box    - [x1, y1] and [x2, y2].
//
// The implementation for several data-types exists:
// - Int    - All members as 32-bit integers - int.
// - Float  - All members as 32-bit floats   - float.
// - Double - All members as 64-bit floats   - double.
//
// Each datatype contains implicit conversion from less-precision types, so for
// example you can translate FloatPoint by IntPoint, but you can't translate
// IntPoint by FloatPoint or DoublePoint.
//
// These structures are so easy so the members are not prefixes with underscore
// and all members are public.

// ============================================================================
// [Fog::IntPoint]
// ============================================================================

//! @brief Point (32-bit integer based).
struct FOG_HIDDEN IntPoint
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE IntPoint()
  {
  }
  
  FOG_INLINE IntPoint(int px, int py) : x(px), y(py)
  {
  }

  FOG_INLINE IntPoint(const IntPoint& other)
  {
    if (sizeof(IntPoint) == 8)
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

  FOG_INLINE IntPoint& set(const IntPoint& other)
  { 
    if (sizeof(IntPoint) == 8)
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

  FOG_INLINE IntPoint& set(int px, int py)
  { 
    x = px;
    y = py;
    return *this;
  }

  FOG_INLINE IntPoint& setX(int px) { x = px; return *this; }
  FOG_INLINE IntPoint& setY(int py) { y = py; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE IntPoint& clear()
  { 
    x = 0; 
    y = 0; 
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE IntPoint& translate(const IntPoint& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  FOG_INLINE IntPoint& translate(int px, int py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE IntPoint translated(const IntPoint& other) const
  {
    return IntPoint(x + other.x, y + other.y);
  }

  FOG_INLINE IntPoint translated(int px, int py) const
  {
    return IntPoint(x + px, y + py);
  }

  // --------------------------------------------------------------------------
  // [Negate]
  // --------------------------------------------------------------------------

  FOG_INLINE IntPoint& negate()
  {
    x = -x;
    y = -y;
    return *this;
  }

  FOG_INLINE IntPoint negated() const
  {
    return IntPoint(-x, -y);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const IntPoint& other) const
  {
    if (sizeof(IntPoint) == 8)
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

  FOG_INLINE FloatPoint toFloatPoint() const;
  FOG_INLINE DoublePoint toDoublePoint() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE IntPoint& operator=(const IntPoint& other)
  {
    x = other.x;
    y = other.y;
    return *this;
  }

  FOG_INLINE IntPoint& operator+=(const IntPoint& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  FOG_INLINE IntPoint& operator-=(const IntPoint& other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const IntPoint& other) const { return (x == other.x) & (y == other.y); }
  FOG_INLINE bool operator!=(const IntPoint& other) const { return (x != other.x) | (y != other.y); }

  FOG_INLINE bool operator< (const IntPoint& other) { return (y < other.y) | ((y <= other.y) & (x <  other.x)); }
  FOG_INLINE bool operator> (const IntPoint& other) { return (y > other.y) | ((y <= other.y) & (x >  other.x)); }
  FOG_INLINE bool operator<=(const IntPoint& other) { return (y < other.y) | ((y == other.y) & (x <= other.x)); }
  FOG_INLINE bool operator>=(const IntPoint& other) { return (y > other.y) | ((y == other.y) & (x >= other.x)); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int x;
  int y;
};

// ============================================================================
// [Fog::FloatPoint]
// ============================================================================

//! @brief Point (32-bit float based).
struct FloatPoint
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatPoint()
  {
  }

  FOG_INLINE FloatPoint(const IntPoint& other) : x((float)other.x), y((float)other.y)
  {
  }

  FOG_INLINE FloatPoint(const FloatPoint& other) : x(other.x), y(other.y)
  {
  }

  FOG_INLINE FloatPoint(int px, int py) : x((float)px), y((float)py)
  {
  }

  FOG_INLINE FloatPoint(float px, float py) : x(px), y(py)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getX() const { return x; }
  FOG_INLINE float getY() const { return y; }

  FOG_INLINE FloatPoint& set(const IntPoint& other)
  {
    x = (float)other.x;
    y = (float)other.y;
    return *this;
  }

  FOG_INLINE FloatPoint& set(int px, int py)
  {
    x = (float)px;
    y = (float)py;
    return *this;
  }

  FOG_INLINE FloatPoint& set(const FloatPoint& other)
  {
    if (sizeof(FloatPoint) == 8)
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

  FOG_INLINE FloatPoint& set(float px, float py)
  {
    x = px;
    y = py;
    return *this;
  }

  FOG_INLINE FloatPoint& setX(int px) { x = (float)px; return *this; }
  FOG_INLINE FloatPoint& setY(int py) { y = (float)py; return *this; }

  FOG_INLINE FloatPoint& setX(float px) { x = px; return *this; }
  FOG_INLINE FloatPoint& setY(float py) { y = py; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatPoint& clear()
  {
    x = 0.0;
    y = 0.0;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatPoint& translate(const IntPoint& other)
  {
    x += (float)other.x;
    y += (float)other.y;
    return *this;
  }

  FOG_INLINE FloatPoint& translate(int px, int py)
  {
    x += (float)px;
    y += (float)py;
    return *this;
  }

  FOG_INLINE FloatPoint& translate(const FloatPoint& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  FOG_INLINE FloatPoint& translate(float px, float py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE FloatPoint translated(const IntPoint& other) const
  {
    return FloatPoint(x + (float)other.x, y + (float)other.y);
  }

  FOG_INLINE FloatPoint translated(int px, int py) const
  {
    return FloatPoint(x + (float)px, y + (float)py);
  }

  FOG_INLINE FloatPoint translated(const FloatPoint& other) const
  {
    return FloatPoint(x + other.x, y + other.y);
  }

  FOG_INLINE FloatPoint translated(float px, float py) const
  {
    return FloatPoint(x + px, y + py);
  }

  // --------------------------------------------------------------------------
  // [Negate]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatPoint& negate()
  {
    x = -x;
    y = -y;
    return *this;
  }

  FOG_INLINE FloatPoint negated() const
  {
    return FloatPoint(-x, -y);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FloatPoint& other) const
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

  FOG_INLINE DoublePoint toDoublePoint() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatPoint& operator=(const IntPoint& other)
  {
    return set(other);
  }

  FOG_INLINE FloatPoint& operator=(const FloatPoint& other)
  {
    return set(other);
  }

  FOG_INLINE FloatPoint& operator+=(const IntPoint& other)
  {
    return translate(other);
  }

  FOG_INLINE FloatPoint& operator+=(const FloatPoint& other)
  {
    return translate(other);
  }

  FOG_INLINE FloatPoint& operator-=(const IntPoint& other)
  {
    x -= (float)other.x;
    y -= (float)other.y;
    return *this;
  }

  FOG_INLINE FloatPoint& operator-=(const FloatPoint& other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const FloatPoint& other) const { return (x == other.x) & (y == other.y); }
  FOG_INLINE bool operator!=(const FloatPoint& other) const { return (x != other.x) | (y != other.y); }

  FOG_INLINE bool operator< (const FloatPoint& other) { return (y < other.y) | ((y <= other.y) & (x <  other.x)); }
  FOG_INLINE bool operator> (const FloatPoint& other) { return (y > other.y) | ((y <= other.y) & (x >  other.x)); }
  FOG_INLINE bool operator<=(const FloatPoint& other) { return (y < other.y) | ((y == other.y) & (x <= other.x)); }
  FOG_INLINE bool operator>=(const FloatPoint& other) { return (y > other.y) | ((y == other.y) & (x >= other.x)); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float x;
  float y;
};

// ============================================================================
// [Fog::DoublePoint]
// ============================================================================

//! @brief Point (64-bit float based).
struct DoublePoint
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE DoublePoint()
  {
  }

  FOG_INLINE DoublePoint(const IntPoint& other) : x((double)other.x), y((double)other.y)
  {
  }

  FOG_INLINE DoublePoint(int px, int py) : x((double)px), y((double)py)
  {
  }

  FOG_INLINE DoublePoint(const FloatPoint& other) : x((double)other.x), y((double)other.y)
  {
  }

  FOG_INLINE DoublePoint(float px, float py) : x((float)px), y((float)py)
  {
  }

  FOG_INLINE DoublePoint(const DoublePoint& other) : x(other.x), y(other.y)
  {
  }

  FOG_INLINE DoublePoint(double px, double py) : x(px), y(py)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getX() const { return x; }
  FOG_INLINE double getY() const { return y; }

  FOG_INLINE DoublePoint& set(const IntPoint& other)
  {
    x = (double)other.x;
    y = (double)other.y;
    return *this;
  }

  FOG_INLINE DoublePoint& set(const FloatPoint& other)
  {
    x = (double)other.x;
    y = (double)other.y;
    return *this;
  }

  FOG_INLINE DoublePoint& set(const DoublePoint& other)
  {
    x = other.x;
    y = other.y;
    return *this;
  }

  FOG_INLINE DoublePoint& set(double px, double py) 
  { 
    x = px;
    y = py;
    return *this;
  }

  FOG_INLINE DoublePoint& setX(int px) { x = (double)px; return *this; }
  FOG_INLINE DoublePoint& setY(int py) { y = (double)py; return *this; }

  FOG_INLINE DoublePoint& setX(float px) { x = (double)px; return *this; }
  FOG_INLINE DoublePoint& setY(float py) { y = (double)py; return *this; }

  FOG_INLINE DoublePoint& setX(double px) { x = px; return *this; }
  FOG_INLINE DoublePoint& setY(double py) { y = py; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE DoublePoint& clear()
  { 
    x = 0.0;
    y = 0.0;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE DoublePoint& translate(const IntPoint& other)
  {
    x += (double)other.x;
    y += (double)other.y;
    return *this;
  }

  FOG_INLINE DoublePoint& translate(const DoublePoint& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  FOG_INLINE DoublePoint& translate(double px, double py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE DoublePoint translated(const IntPoint& other) const
  {
    return DoublePoint(x + (double)other.x, y + (double)other.y);
  }

  FOG_INLINE DoublePoint translated(int px, int py) const
  {
    return DoublePoint(x + (double)px, y + (double)py);
  }

  FOG_INLINE DoublePoint translated(const FloatPoint& other) const
  {
    return DoublePoint(x + (double)other.x, y + (double)other.y);
  }

  FOG_INLINE DoublePoint translated(float px, float py) const
  {
    return DoublePoint(x + (double)px, y + (double)py);
  }

  FOG_INLINE DoublePoint translated(const DoublePoint& other) const
  {
    return DoublePoint(x + other.x, y + other.y);
  }

  FOG_INLINE DoublePoint translated(double px, double py) const
  {
    return DoublePoint(x + px, y + py);
  }

  // --------------------------------------------------------------------------
  // [Negate]
  // --------------------------------------------------------------------------

  FOG_INLINE DoublePoint& negate()
  {
    x = -x;
    y = -y;
    return *this;
  }

  FOG_INLINE DoublePoint negated() const
  {
    return DoublePoint(-x, -y);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const DoublePoint& other) const
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

  FOG_INLINE FloatPoint toFloatPoint() const { return FloatPoint((float)x, (float)y); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE DoublePoint& operator=(const IntPoint& other)
  {
    return set(other);
  }

  FOG_INLINE DoublePoint& operator=(const FloatPoint& other)
  {
    return set(other);
  }

  FOG_INLINE DoublePoint& operator=(const DoublePoint& other)
  {
    return set(other);
  }

  FOG_INLINE DoublePoint& operator+=(const IntPoint& other)
  {
    x += (double)other.x;
    y += (double)other.y;
    return *this;
  }

  FOG_INLINE DoublePoint& operator+=(const FloatPoint& other)
  {
    x += (double)other.x;
    y += (double)other.y;
    return *this;
  }

  FOG_INLINE DoublePoint& operator+=(const DoublePoint& other) 
  {
    x += other.x;
    y += other.y;
    return *this;
  }
  
  FOG_INLINE DoublePoint& operator-=(const IntPoint& other)
  {
    x -= (double)other.x;
    y -= (double)other.y;
    return *this;
  }

  FOG_INLINE DoublePoint& operator-=(const FloatPoint& other)
  {
    x -= (double)other.x;
    y -= (double)other.y;
    return *this;
  }

  FOG_INLINE DoublePoint& operator-=(const DoublePoint& other) 
  { 
    x -= other.x;
    y -= other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const DoublePoint& other) const { return (x == other.x) & (y == other.y); }
  FOG_INLINE bool operator!=(const DoublePoint& other) const { return (x != other.x) | (y != other.y); }

  FOG_INLINE bool operator< (const DoublePoint& other) { return (y < other.y) | ((y <= other.y) & (x <  other.x)); }
  FOG_INLINE bool operator> (const DoublePoint& other) { return (y > other.y) | ((y <= other.y) & (x >  other.x)); }
  FOG_INLINE bool operator<=(const DoublePoint& other) { return (y < other.y) | ((y == other.y) & (x <= other.x)); }
  FOG_INLINE bool operator>=(const DoublePoint& other) { return (y > other.y) | ((y == other.y) & (x >= other.x)); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double x;
  double y;
};

// ============================================================================
// [Fog::IntSize]
// ============================================================================

//! @brief Size (32-bit integer based).
struct IntSize
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE IntSize()
  {
  }

  FOG_INLINE IntSize(const IntSize& other) : w(other.w), h(other.h)
  {
  }

  FOG_INLINE IntSize(int sw, int sh) : w(sw), h(sh)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getWidth() const { return w; }
  FOG_INLINE int getHeight() const { return h; }

  FOG_INLINE IntSize& set(const IntSize& other)
  {
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE IntSize& set(int sw, int sh)
  {
    w = sw;
    h = sh;
    return *this;
  }

  FOG_INLINE IntSize& setWidth(int sw) { w = sw; return *this; }
  FOG_INLINE IntSize& setHeight(int sh) { h = sh; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE IntSize& clear()
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

  FOG_INLINE bool eq(const IntSize& other) const
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

  FOG_INLINE IntSize& adjust(int sw, int sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE IntSize adjusted(int sw, int sh) const
  {
    return IntSize(w + sw, h + sh);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatSize toFloatSize() const;
  FOG_INLINE DoubleSize toDoubleSize() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const IntSize& operator=(const IntSize& other)
  {
    return set(other);
  }

  FOG_INLINE bool operator==(const IntSize& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool operator!=(const IntSize& other) const
  {
    return (w != other.w) | (h != other.h);
  }

  FOG_INLINE IntSize expandedTo(const IntSize& otherSize) const
  {
    return IntSize(Math::max(w,otherSize.w), Math::max(h,otherSize.h));
  }

  FOG_INLINE IntSize boundedTo(const IntSize& otherSize) const
  {
    return IntSize(Math::min(w,otherSize.w), Math::min(h,otherSize.h));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int w;
  int h;
};

// ============================================================================
// [Fog::FloatSize]
// ============================================================================

struct FloatSize
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatSize()
  {
  }

  FOG_INLINE FloatSize(const IntSize& other) : w((float)other.w), h((float)other.h)
  {
  }

  FOG_INLINE FloatSize(int sw, int sh) : w((float)sw), h((float)sh)
  {
  }

  FOG_INLINE FloatSize(const FloatSize& other) : w(other.w), h(other.h)
  {
  }

  FOG_INLINE FloatSize(float sw, float sh) : w(sw), h(sh)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getWidth() const { return w; }
  FOG_INLINE float getHeight() const { return h; }

  FOG_INLINE FloatSize& set(const IntSize& other)
  {
    w = (float)other.w;
    h = (float)other.h;
    return *this;
  }

  FOG_INLINE FloatSize& set(int sw, int sh)
  {
    w = (float)sw;
    h = (float)sh;
    return *this;
  }

  FOG_INLINE FloatSize& set(const FloatSize& other)
  {
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE FloatSize& set(float sw, float sh)
  {
    w = sw;
    h = sh;
    return *this;
  }

  FOG_INLINE FloatSize& setWidth(int sw) { w = (float)sw; return *this; }
  FOG_INLINE FloatSize& setHeight(int sh) { h = (float)sh; return *this; }

  FOG_INLINE FloatSize& setWidth(float sw) { w = sw; return *this; }
  FOG_INLINE FloatSize& setHeight(float sh) { h = sh; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatSize& clear()
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

  FOG_INLINE bool eq(const FloatSize& other) const
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

  FOG_INLINE FloatSize& adjust(float sw, float sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE FloatSize adjusted(float sw, float sh) const
  {
    return FloatSize(w + sw, h + sh);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleSize toDoubleSize() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const FloatSize& operator=(const IntSize& other)
  {
    return set(other);
  }

  FOG_INLINE const FloatSize& operator=(const FloatSize& other)
  {
    return set(other);
  }

  FOG_INLINE bool operator==(const FloatSize& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool operator!=(const FloatSize& other) const
  {
    return (w != other.w) | (h != other.h);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float w;
  float h;
};

// ============================================================================
// [Fog::DoubleSize]
// ============================================================================

struct DoubleSize
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleSize()
  {
  }

  FOG_INLINE DoubleSize(const IntSize& other) : w((double)other.w), h((double)other.h)
  {
  }

  FOG_INLINE DoubleSize(int sw, int sh) : w((double)sw), h((double)sh)
  {
  }

  FOG_INLINE DoubleSize(const FloatSize& other) : w((double)other.w), h((double)other.h)
  {
  }

  FOG_INLINE DoubleSize(float sw, float sh) : w((double)sw), h((double)sh)
  {
  }

  FOG_INLINE DoubleSize(const DoubleSize& other) : w(other.w), h(other.h)
  {
  }

  FOG_INLINE DoubleSize(double sw, double sh) : w(sw), h(sh)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getWidth() const { return w; }
  FOG_INLINE double getHeight() const { return h; }

  FOG_INLINE DoubleSize& set(const IntSize& other)
  {
    w = (double)other.w;
    h = (double)other.h;
    return *this;
  }

  FOG_INLINE DoubleSize& set(int sw, int sh)
  {
    w = (double)sw;
    h = (double)sh;
    return *this;
  }

  FOG_INLINE DoubleSize& set(const FloatSize& other)
  {
    w = (double)other.w;
    h = (double)other.h;
    return *this;
  }

  FOG_INLINE DoubleSize& set(float sw, float sh)
  {
    w = (double)sw;
    h = (double)sh;
    return *this;
  }

  FOG_INLINE DoubleSize& set(const DoubleSize& other)
  {
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE DoubleSize& set(double sw, double sh)
  {
    w = sw;
    h = sh;
    return *this;
  }

  FOG_INLINE DoubleSize& setWidth(int sw) { w = sw; return *this; }
  FOG_INLINE DoubleSize& setHeight(int sh) { h = sh; return *this; }

  FOG_INLINE DoubleSize& setWidth(float sw) { w = sw; return *this; }
  FOG_INLINE DoubleSize& setHeight(float sh) { h = sh; return *this; }

  FOG_INLINE DoubleSize& setWidth(double sw) { w = sw; return *this; }
  FOG_INLINE DoubleSize& setHeight(double sh) { h = sh; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleSize& clear()
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

  FOG_INLINE bool eq(const DoubleSize& other) const
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

  FOG_INLINE DoubleSize& adjust(double sw, double sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE DoubleSize adjusted(double sw, double sh) const
  {
    return DoubleSize(w + sw, h + sh);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatSize toFloatSize() const { return FloatSize((float)w, (float)h); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const DoubleSize& operator=(const IntSize& other)
  {
    return set(other);
  }

  FOG_INLINE const DoubleSize& operator=(const FloatSize& other)
  {
    return set(other);
  }

  FOG_INLINE const DoubleSize& operator=(const DoubleSize& other)
  {
    return set(other);
  }

  FOG_INLINE bool operator==(const DoubleSize& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool operator!=(const DoubleSize& other) const
  {
    return (w != other.w) | (h != other.h);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double w;
  double h;
};

// ============================================================================
// [Fog::IntRect]
// ============================================================================

//! @brief Rectangle (32-bit integer based).
struct IntRect
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE IntRect()
  {
  }

  FOG_INLINE IntRect(const IntRect& other) :
    x(other.x), y(other.y), w(other.w), h(other.h)
  {
  }

  FOG_INLINE IntRect(int rx, int ry, int rw, int rh) :
    x(rx), y(ry), w(rw), h(rh)
  {
  }

  FOG_INLINE explicit IntRect(const IntBox& box);
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

  FOG_INLINE const IntPoint& getPosition() const { return *(const IntPoint *)(const void*)(&x); }
  FOG_INLINE const IntSize& getSize() const { return *(const IntSize *)(const void*)(&w); }

  FOG_INLINE IntRect& set(int rx, int ry, int rw, int rh)
  { 
    x = rx; 
    y = ry; 
    w = rw; 
    h = rh; 

    return *this;
  }

  FOG_INLINE IntRect& set(const IntRect &other)
  {
    if (sizeof(IntRect) == 16)
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

  FOG_INLINE IntRect& set(const IntBox& box);

  FOG_INLINE IntRect& setX(int rx) { x = rx; return *this; }
  FOG_INLINE IntRect& setY(int ry) { y = ry; return *this; }
  FOG_INLINE IntRect& setWidth(int rw) { w = rw; return *this; }
  FOG_INLINE IntRect& setHeight(int rh) { h = rh; return *this; }

  FOG_INLINE IntRect& setX1(int x1) { x = x1; return *this; }
  FOG_INLINE IntRect& setY1(int y1) { y = y1; return *this; }
  FOG_INLINE IntRect& setX2(int x2) { w = x2 - x; return *this; }
  FOG_INLINE IntRect& setY2(int y2) { h = y2 - y; return *this; }

  FOG_INLINE IntRect& setLeft(int x1) { x = x1; return *this; }
  FOG_INLINE IntRect& setTop(int y1) { y = y1; return *this; }
  FOG_INLINE IntRect& setRight(int x2) { w = x2 - x; return *this; }
  FOG_INLINE IntRect& setBottom(int y2) { h = y2 - y; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE IntRect& clear()
  {
    if (sizeof(IntRect) == 16)
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

  FOG_INLINE IntRect& translate(int rx, int ry) { x += rx; y += ry; return *this; }
  FOG_INLINE IntRect& translate(const IntPoint& p) { x += p.x; y += p.y; return *this; }

  FOG_INLINE IntRect translated(int rx, int ry) { return IntRect(x + rx, y + ry, w, h); }
  FOG_INLINE IntRect translated(const IntPoint& p) { return IntRect(x + p.x, y + p.y, w, h); }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap. Remember, x2() and y2() coords aren't in the rectangle.
  FOG_INLINE bool overlaps(const IntRect& r) const
  {
    return (( ((getY1()-r.getY2()) ^ (getY2()-r.getY1())) &
              ((getX1()-r.getX2()) ^ (getX2()-r.getX1())) ) < 0);
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const IntRect& r) const
  {
    return ((r.getX1() >= getX1()) & (r.getX2() <= getX2()) &
            (r.getY1() >= getY1()) & (r.getY2() <= getY2()) );
  }

  static FOG_INLINE bool intersect(IntRect& dest, const IntRect& src1, const IntRect& src2)
  {
    int xx = Math::max(src1.getX1(), src2.getX1());
    int yy = Math::max(src1.getY1(), src2.getY1());

    dest.set(xx,
             yy,
             Math::min(src1.getX2(), src2.getX2()) - xx,
             Math::min(src1.getY2(), src2.getY2()) - yy);
    return dest.isValid();
  }

  static FOG_INLINE void unite(IntRect& dst, const IntRect& src1, const IntRect& src2)
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
  FOG_INLINE bool contains(const IntPoint& pt) const
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

  FOG_INLINE bool eq(const IntRect& other) const
  {
    if (sizeof(IntRect) == 16)
      return Memory::eq16B(static_cast<const void*>(this), static_cast<const void*>(&other));
    else
      return (x == other.x) & (y == other.y) & (w == other.w) & (h == other.h);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrink rectangle by @c n.
  FOG_INLINE IntRect& shrink(int n)
  {
    x += n;
    y += n;
    n <<= 1;
    w -= n;
    h -= n;
    
    return *this;
  }

  //! @brief Expand rectangle by @c n.
  FOG_INLINE IntRect& expand(int n)
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

  FOG_INLINE IntRect& adjust(int px, int py)
  {
    x += px;
    y += py;
    w -= px;
    h -= py;

    w -= px;
    h -= py;

    return *this;
  }

  FOG_INLINE IntRect& adjust(int px1, int py1, int px2, int py2)
  {
    x += px1;
    y += py1;
    w -= px1;
    h -= py1;

    w += px2;
    h += py2;

    return *this;
  }

  FOG_INLINE IntRect adjusted(int px, int py) const
  {
    return IntRect(x + px, y + py, w - px - px, h - py - py);
  }

  FOG_INLINE IntRect adjusted(int px1, int py1, int px2, int py2) const
  {
    return IntRect(x + px1, y + py1, w - px1 + px2, h - py1 + py2);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatRect toFloatRect() const;
  FOG_INLINE DoubleRect toDoubleRect() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE IntRect& operator=(const IntRect& other)
  {
    return set(other);
  }

  FOG_INLINE IntRect& operator+=(const IntPoint& p)
  {
    return translate(p);
  }

  FOG_INLINE IntRect& operator-=(const IntPoint& p)
  {
    x -= p.x;
    y -= p.y;
    return *this;
  }

  FOG_INLINE bool operator==(const IntRect& other) const { return (x == other.x) & (y == other.y) & (w == other.w) & (h == other.h); }
  FOG_INLINE bool operator!=(const IntRect& other) const { return (x != other.x) | (y != other.y) | (w != other.w) | (h != other.h); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int x;
  int y;
  int w;
  int h;
};

// ============================================================================
// [Fog::FloatRect]
// ============================================================================

//! @brief Rectangle (32-bit float based).
struct FloatRect
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatRect()
  {
  }

  FOG_INLINE FloatRect(const IntRect& other)
    : x((float)other.x), y((float)other.y), w((float)other.w), h((float)other.h)
  {
  }

  FOG_INLINE FloatRect(const IntPoint& pt0, const IntPoint& pt1)
    : x((float)pt0.x), y((float)pt0.y), w((float)pt1.x - (float)pt0.x), h((float)pt1.y - (float)pt0.y)
  {
  }

  FOG_INLINE FloatRect(const IntPoint& pt0, const IntSize& sz)
    : x((float)pt0.x), y((float)pt0.y), w((float)sz.w), h((float)sz.h)
  {
  }

  FOG_INLINE FloatRect(int rx, int ry, int rw, int rh)
    : x((float)rx), y((float)ry), w((float)rw), h((float)rh)
  {
  }

  FOG_INLINE FloatRect(const FloatRect& other)
    : x(other.x), y(other.y), w(other.w), h(other.h)
  {
  }

  FOG_INLINE FloatRect(const FloatPoint& pt0, const FloatPoint& pt1)
    : x(pt0.x), y(pt0.y), w(pt1.x - pt0.x), h(pt1.y - pt0.y)
  {
  }

  FOG_INLINE FloatRect(const FloatPoint& pt0, const FloatSize& sz)
    : x(pt0.x), y(pt0.y), w(sz.w), h(sz.h)
  {
  }

  FOG_INLINE FloatRect(float rx, float ry, float rw, float rh)
    : x(rx), y(ry), w(rw), h(rh)
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

  FOG_INLINE DoublePoint getPosition() const { return DoublePoint(x, y); }
  FOG_INLINE DoubleSize getSize() const { return DoubleSize(w, h); }

  FOG_INLINE FloatRect& set(const IntRect &other)
  {
    x = (float)other.x; y = (float)other.y;
    w = (float)other.w; h = (float)other.h;
    return *this;
  }

  FOG_INLINE FloatRect& set(int rx, int ry, int rw, int rh)
  {
    x = (float)rx; y = (float)ry;
    w = (float)rw; h = (float)rh;
    return *this;
  }

  FOG_INLINE FloatRect& set(const FloatRect &other)
  {
    x = other.x; y = other.y;
    w = other.w; h = other.h;
    return *this;
  }

  FOG_INLINE FloatRect& set(float rx, float ry, float rw, float rh)
  {
    x = rx; y = ry;
    w = rw; h = rh;
    return *this;
  }

  FOG_INLINE FloatRect& setX(float x) { x = x; return *this; }
  FOG_INLINE FloatRect& setY(float y) { y = y; return *this; }
  FOG_INLINE FloatRect& setWidth(float w) { w = w; return *this; }
  FOG_INLINE FloatRect& setHeight(float h) { h = h; return *this; }

  FOG_INLINE FloatRect& setX1(float x1) { x = x1; return *this; }
  FOG_INLINE FloatRect& setY1(float y1) { y = y1; return *this; }
  FOG_INLINE FloatRect& setX2(float x2) { w = x2 - x; return *this; }
  FOG_INLINE FloatRect& setY2(float y2) { h = y2 - y; return *this; }

  FOG_INLINE FloatRect& setLeft(float x1) { x = x1; return *this; }
  FOG_INLINE FloatRect& setTop(float y1) { y = y1; return *this; }
  FOG_INLINE FloatRect& setRight(float x2) { w = x2 - x; return *this; }
  FOG_INLINE FloatRect& setBottom(float y2) { h = y2 - y; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatRect& clear()
  {
    x = 0.0f; y = 0.0f;
    w = 0.0f; h = 0.0f;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatRect& translate(const IntPoint& pt)
  {
    x += (float)pt.x;
    y += (float)pt.y;
    return *this;
  }

  FOG_INLINE FloatRect& translate(int px, int py)
  {
    x += (float)px;
    y += (float)py;
    return *this;
  }

  FOG_INLINE FloatRect& translate(const FloatPoint& pt)
  {
    x += pt.x;
    y += pt.y;
    return *this;
  }

  FOG_INLINE FloatRect& translate(float px, float py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE FloatRect translated(const IntPoint& pt) const
  {
    return FloatRect(x + (float)pt.x, y + (float)pt.y, w, h);
  }

  FOG_INLINE FloatRect translated(int px, int py) const
  {
    return FloatRect(x + (float)px, y + (float)py, w, h);
  }

  FOG_INLINE FloatRect translated(const FloatPoint& pt) const
  {
    return FloatRect(x + (float)pt.x, y + (float)pt.y, w, h);
  }

  FOG_INLINE FloatRect translated(float px, float py) const
  {
    return FloatRect(x + (float)px, y + (float)py, w, h);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatRect& adjust(const IntPoint& pt)
  {
    return adjust((float)pt.x, (float)pt.y);
  }

  FOG_INLINE FloatRect& adjust(int px, int py)
  {
    return adjust((float)px, (float)py);
  }

  FOG_INLINE FloatRect& adjust(const FloatPoint& pt)
  {
    return adjust(pt.x, pt.y);
  }

  FOG_INLINE FloatRect& adjust(float px, float py)
  {
    x += px; y += py;
    w -= px; h -= py;
    w -= px; h -= py;

    return *this;
  }

  FOG_INLINE FloatRect adjusted(const IntPoint& pt) const
  {
    return adjusted((float)pt.x, (float)pt.y);
  }

  FOG_INLINE FloatRect adjusted(int px, int py) const
  {
    return adjusted((float)px, (float)py);
  }

  FOG_INLINE FloatRect adjusted(const FloatPoint& pt) const
  {
    return adjusted((float)pt.x, (float)pt.y);
  }

  FOG_INLINE FloatRect adjusted(float px, float py) const
  {
    return adjusted(px, py);
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap.
  FOG_INLINE bool overlaps(const FloatRect& r) const
  {
    return (Math::max(getX1(), r.getX1()) < Math::min(getX1(), r.getX2()) &&
            Math::max(getY1(), r.getY1()) < Math::min(getY2(), r.getY2()) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const FloatRect& r) const
  {
    return ((r.getX1() >= getX1()) & (r.getX2() <= getX2()) &
            (r.getY1() >= getY1()) & (r.getY2() <= getY2()) );
  }

  static FOG_INLINE bool intersect(FloatRect& dest, const FloatRect& src1, const FloatRect& src2)
  {
    dest.x = Math::max(src1.getX1(), src2.getX1());
    dest.y = Math::max(src1.getY1(), src2.getY1());
    dest.w = Math::min(src1.getX2(), src2.getX2()) - dest.x;
    dest.h = Math::min(src1.getY2(), src2.getY2()) - dest.y;

    return dest.isValid();
  }

  static FOG_INLINE void unite(FloatRect& dest, const FloatRect& src1, const FloatRect& src2)
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
  FOG_INLINE bool contains(const FloatPoint& other) const
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

  FOG_INLINE bool eq(const FloatRect& other) const
  {
    return (x == other.x) & (y == other.y) & (w == other.w) & (h == other.h);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleRect toDoubleRect() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatRect& operator=(const IntRect& other)
  {
    return set(other);
  }

  FOG_INLINE FloatRect& operator=(const FloatRect& other)
  {
    return set(other);
  }

  FOG_INLINE FloatRect& operator+=(const IntPoint& other)
  {
    return translate(other);
  }

  FOG_INLINE FloatRect& operator+=(const FloatPoint& other)
  {
    return translate(other);
  }

  FOG_INLINE FloatRect& operator-=(const IntPoint& other)
  {
    x -= (float)other.x;
    y -= (float)other.y;
    return *this;
  }

  FOG_INLINE FloatRect& operator-=(const FloatPoint& other)
  {
    x -= (float)other.x;
    y -= (float)other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const FloatRect& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FloatRect& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float x;
  float y;
  float w;
  float h;
};

// ============================================================================
// [Fog::DoubleRect]
// ============================================================================

//! @brief Rectangle (64-bit integer based).
struct DoubleRect
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleRect()
  {
  }

  FOG_INLINE DoubleRect(const IntRect& other)
    : x((double)other.x), y((double)other.y), w((double)other.w), h((double)other.h)
  {
  }

  FOG_INLINE DoubleRect(const IntPoint& pt0, const IntPoint& pt1)
    : x((double)pt0.x), y((double)pt0.y), w((double)pt1.x - (double)pt0.x), h((double)pt1.y - (double)pt0.y)
  {
  }

  FOG_INLINE DoubleRect(const IntPoint& pt0, const IntSize& sz)
    : x((double)pt0.x), y((double)pt0.y), w((double)sz.w), h((double)sz.h)
  {
  }

  FOG_INLINE DoubleRect(int rx, int ry, int rw, int rh)
    : x((double)rx), y((double)ry), w((double)rw), h((double)rh)
  {
  }

  FOG_INLINE DoubleRect(const FloatRect& other)
    : x((double)other.x), y((double)other.y), w((double)other.w), h((double)other.h)
  {
  }

  FOG_INLINE DoubleRect(const FloatPoint& pt0, const FloatPoint& pt1)
    : x((double)pt0.x), y((double)pt0.y), w((double)pt1.x - (double)pt0.x), h((double)pt1.y - (double)pt0.y)
  {
  }

  FOG_INLINE DoubleRect(const FloatPoint& pt0, const FloatSize& sz)
    : x((double)pt0.x), y((double)pt0.y), w((double)sz.w), h((double)sz.h)
  {
  }

  FOG_INLINE DoubleRect(float rx, float ry, float rw, float rh)
    : x((double)rx), y((double)ry), w((double)rw), h((double)rh)
  {
  }

  FOG_INLINE DoubleRect(const DoubleRect& other)
    : x(other.x), y(other.y), w(other.w), h(other.h)
  {
  }

  FOG_INLINE DoubleRect(const DoublePoint& pt0, const DoublePoint& pt1)
    : x(pt0.x), y(pt0.y), w(pt1.x - pt0.x), h(pt1.y - pt0.y)
  {
  }

  FOG_INLINE DoubleRect(const DoublePoint& pt0, const DoubleSize& sz)
    : x(pt0.x), y(pt0.y), w(sz.w), h(sz.h)
  {
  }

  FOG_INLINE DoubleRect(double rx, double ry, double rw, double rh)
    : x(rx), y(ry), w(rw), h(rh)
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

  FOG_INLINE DoublePoint getPosition() const { return DoublePoint(x, y); }
  FOG_INLINE DoubleSize getSize() const { return DoubleSize(w, h); }

  FOG_INLINE DoubleRect& set(const IntRect &other)
  {
    x = (double)other.x; y = (double)other.y;
    w = (double)other.w; h = (double)other.h;
    return *this;
  }

  FOG_INLINE DoubleRect& set(int rx, int ry, int rw, int rh)
  {
    x = (double)rx; y = (double)ry;
    w = (double)rw; h = (double)rh;
    return *this;
  }

  FOG_INLINE DoubleRect& set(const FloatRect &other)
  {
    x = (double)other.x; y = (double)other.y;
    w = (double)other.w; h = (double)other.h;
    return *this;
  }

  FOG_INLINE DoubleRect& set(float rx, float ry, float rw, float rh)
  {
    x = (double)rx; y = (double)ry;
    w = (double)rw; h = (double)rh;
    return *this;
  }

  FOG_INLINE DoubleRect& set(const DoubleRect &other)
  {
    x = other.x; y = other.y;
    w = other.w; h = other.h;
    return *this;
  }

  FOG_INLINE DoubleRect& set(double rx, double ry, double rw, double rh)
  {
    x = rx; y = ry;
    w = rw; h = rh;
    return *this;
  }

  FOG_INLINE DoubleRect& setX(double x) { x = x; return *this; }
  FOG_INLINE DoubleRect& setY(double y) { y = y; return *this; }
  FOG_INLINE DoubleRect& setWidth(double w) { w = w; return *this; }
  FOG_INLINE DoubleRect& setHeight(double h) { h = h; return *this; }

  FOG_INLINE DoubleRect& setX1(double x1) { x = x1; return *this; }
  FOG_INLINE DoubleRect& setY1(double y1) { y = y1; return *this; }
  FOG_INLINE DoubleRect& setX2(double x2) { w = x2 - x; return *this; }
  FOG_INLINE DoubleRect& setY2(double y2) { h = y2 - y; return *this; }

  FOG_INLINE DoubleRect& setLeft(double x1) { x = x1; return *this; }
  FOG_INLINE DoubleRect& setTop(double y1) { y = y1; return *this; }
  FOG_INLINE DoubleRect& setRight(double x2) { w = x2 - x; return *this; }
  FOG_INLINE DoubleRect& setBottom(double y2) { h = y2 - y; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleRect& clear() 
  {
    x = 0.0; y = 0.0;
    w = 0.0; h = 0.0;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleRect& translate(const IntPoint& pt)
  {
    x += (double)pt.x;
    y += (double)pt.y;
    return *this;
  }

  FOG_INLINE DoubleRect& translate(int px, int py)
  {
    x += (double)px;
    y += (double)py;
    return *this;
  }

  FOG_INLINE DoubleRect& translate(const FloatPoint& pt)
  {
    x += (double)pt.x;
    y += (double)pt.y;
    return *this;
  }

  FOG_INLINE DoubleRect& translate(float px, float py)
  {
    x += (double)px;
    y += (double)py;
    return *this;
  }

  FOG_INLINE DoubleRect& translate(const DoublePoint& pt)
  {
    x += pt.x;
    y += pt.y;
    return *this;
  }

  FOG_INLINE DoubleRect& translate(double px, double py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE DoubleRect translated(const IntPoint& pt) const
  {
    return DoubleRect(x + (double)pt.x, y + (double)pt.y, w, h);
  }

  FOG_INLINE DoubleRect translated(int px, int py) const
  {
    return DoubleRect(x + (double)px, y + (double)py, w, h);
  }

  FOG_INLINE DoubleRect translated(const FloatPoint& pt) const
  {
    return DoubleRect(x + (double)pt.x, y + (double)pt.y, w, h);
  }

  FOG_INLINE DoubleRect translated(float px, float py) const
  {
    return DoubleRect(x + (double)px, y + (double)py, w, h);
  }

  FOG_INLINE DoubleRect translated(const DoublePoint& pt) const
  {
    return DoubleRect(x + pt.x, y + pt.y, w, h);
  }

  FOG_INLINE DoubleRect translated(double px, double py) const
  {
    return DoubleRect(x + px, y + py, w, h);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleRect& adjust(const IntPoint& pt)
  {
    return adjust((double)pt.x, (double)pt.y);
  }

  FOG_INLINE DoubleRect& adjust(int px, int py)
  {
    return adjust((double)px, (double)py);
  }

  FOG_INLINE DoubleRect& adjust(const FloatPoint& pt)
  {
    return adjust((double)pt.x, (double)pt.y);
  }

  FOG_INLINE DoubleRect& adjust(float px, float py)
  {
    return adjust((double)px, (double)py);
  }

  FOG_INLINE DoubleRect& adjust(const DoublePoint& pt)
  {
    return adjust(pt.x, pt.y);
  }

  FOG_INLINE DoubleRect& adjust(double px, double py)
  {
    x += px; y += py;
    w -= px; h -= py;
    w -= px; h -= py;

    return *this;
  }

  FOG_INLINE DoubleRect adjusted(const IntPoint& pt) const
  {
    return adjusted((double)pt.x, (double)pt.y);
  }

  FOG_INLINE DoubleRect adjusted(int px, int py) const
  {
    return adjusted((double)px, (double)py);
  }

  FOG_INLINE DoubleRect adjusted(const FloatPoint& pt) const
  {
    return adjusted((double)pt.x, (double)pt.y);
  }

  FOG_INLINE DoubleRect adjusted(float px, float py) const
  {
    return adjusted((double)px, (double)py);
  }

  FOG_INLINE DoubleRect adjusted(const DoublePoint& pt) const
  {
    return adjusted(pt.x, pt.y);
  }

  FOG_INLINE DoubleRect adjusted(double px, double py) const
  {
    return DoubleRect(x + px, y + py, w - px - px, h - py - py);
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap.
  FOG_INLINE bool overlaps(const DoubleRect& r) const
  {
    return (Math::max(getX1(), r.getX1()) < Math::min(getX1(), r.getX2()) &&
            Math::max(getY1(), r.getY1()) < Math::min(getY2(), r.getY2()) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const DoubleRect& r) const
  {
    return ((r.getX1() >= getX1()) & (r.getX2() <= getX2()) &
            (r.getY1() >= getY1()) & (r.getY2() <= getY2()) );
  }

  static FOG_INLINE bool intersect(DoubleRect& dest, const DoubleRect& src1, const DoubleRect& src2)
  {
    dest.x = Math::max(src1.getX1(), src2.getX1());
    dest.y = Math::max(src1.getY1(), src2.getY1());
    dest.w = Math::min(src1.getX2(), src2.getX2()) - dest.x;
    dest.h = Math::min(src1.getY2(), src2.getY2()) - dest.y;

    return dest.isValid();
  }

  static FOG_INLINE void unite(DoubleRect& dest, const DoubleRect& src1, const DoubleRect& src2)
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
  FOG_INLINE bool contains(const DoublePoint& other) const
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

  FOG_INLINE bool eq(const DoubleRect& other) const
  {
    return (x == other.x) & (y == other.y) & (w == other.w) & (h == other.h);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatRect toFloatRect() const { return FloatRect((float)x, (float)y, (float)w, (float)h); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleRect& operator=(const IntRect& other)
  {
    return set(other);
  }

  FOG_INLINE DoubleRect& operator=(const FloatRect& other)
  {
    return set(other);
  }

  FOG_INLINE DoubleRect& operator=(const DoubleRect& other)
  {
    return set(other);
  }

  FOG_INLINE DoubleRect& operator+=(const IntPoint& other)
  {
    return translate(other);
  }

  FOG_INLINE DoubleRect& operator+=(const FloatPoint& other)
  {
    return translate(other);
  }

  FOG_INLINE DoubleRect& operator+=(const DoublePoint& other)
  {
    return translate(other);
  }

  FOG_INLINE DoubleRect& operator-=(const IntPoint& other)
  {
    x -= (double)other.x;
    y -= (double)other.y;
    return *this;
  }

  FOG_INLINE DoubleRect& operator-=(const FloatPoint& other)
  {
    x -= (double)other.x;
    y -= (double)other.y;
    return *this;
  }

  FOG_INLINE DoubleRect& operator-=(const DoublePoint& other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  FOG_INLINE bool operator==(const DoubleRect& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const DoubleRect& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double x;
  double y;
  double w;
  double h;
};

// ============================================================================
// [Fog::IntBox]
// ============================================================================

//! @brief Box (32-bit integer based).
struct IntBox
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE IntBox()
  {
  }

  FOG_INLINE IntBox(int px1, int py1, int px2, int py2) :
    x1(px1), y1(py1), x2(px2), y2(py2)
  {
  }

  FOG_INLINE IntBox(const IntBox& other) :
    x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2)
  {
  }

  FOG_INLINE IntBox(const IntRect& other);
  // Defined later.

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

  FOG_INLINE IntPoint getPosition() const { return IntPoint(x1, y1); }
  FOG_INLINE IntSize getSize() const { return IntSize(x2 - x1, y2 - y1); }

  FOG_INLINE IntBox& set(const IntBox &other)
  { 
    if (sizeof(IntBox) == 16)
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
  
  FOG_INLINE IntBox& set(int px1, int py1, int px2, int py2)
  { 
    x1 = px1; 
    y1 = py1; 
    x2 = px2; 
    y2 = py2;

    return *this;
  }

  FOG_INLINE IntBox& setX(int x) { x1 = x; return *this; }
  FOG_INLINE IntBox& setY(int y) { y1 = y; return *this; }
  FOG_INLINE IntBox& setWidth(int w) { x2 = x1 + w; return *this; }
  FOG_INLINE IntBox& setHeight(int h) { y2 = y1 + h; return *this; }

  FOG_INLINE IntBox& setX1(int px1) { x1 = px1; return *this; }
  FOG_INLINE IntBox& setY1(int py1) { y1 = py1; return *this; }
  FOG_INLINE IntBox& setX2(int px2) { x2 = px2; return *this; }
  FOG_INLINE IntBox& setY2(int py2) { y2 = py2; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE IntBox& clear()
  {
    if (sizeof(IntBox) == 16)
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

  FOG_INLINE IntBox& translate(int px, int py)
  { 
    x1 += px;
    y1 += py;
    x2 += px;
    y2 += py;
    return *this;
  }

  FOG_INLINE IntBox& translate(const IntPoint& pt)
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
  FOG_INLINE bool overlaps(const IntBox& r) const
  {
    return (( ((y1 - r.y2) ^ (y2-r.y1)) &
              ((x1 - r.x2) ^ (x2-r.x1)) ) < 0);
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const IntBox& r) const
  {
    return ((r.x1 >= x1) & (r.x2 <= x2) &
            (r.y1 >= y1) & (r.y2 <= y2) );
  }

  static FOG_INLINE bool intersect(IntBox& dest, const IntBox& src1, const IntBox& src2)
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
  FOG_INLINE bool contains(const IntPoint& pt) const
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

  FOG_INLINE bool eq(const IntBox& other) const
  {
    if (sizeof(IntBox) == 16)
      return Memory::eq16B(static_cast<const void*>(this), static_cast<const void*>(&other));
    else
      return (x1 == other.x1) & (y1 == other.y1) & (x2 == other.x2) & (y2 == other.y2);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrinks rectangle by @c n coordinates.
  FOG_INLINE IntBox& shrink(int n)
  {
    x1 += n;
    y1 += n;
    x2 -= n;
    y2 -= n;
    
    return *this;
  }

  //! @brief Expands rectangle by @c n coordinates.
  FOG_INLINE IntBox& expand(int n)
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

  FOG_INLINE IntRect toIntRect() const { return IntRect(x1, y1, x2 - x1, y2 - y1); }
  FOG_INLINE FloatRect toFloatRect() const { return FloatRect(x1, y1, x2 - x1, y2 - y1); }
  FOG_INLINE DoubleRect toDoubleRect() const { return DoubleRect(x1, y1, x2 - x1, y2 - y1); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE IntBox& operator=(const IntBox& other)
  {
    return set(other);
  }

  FOG_INLINE IntBox& operator+=(const IntPoint& pt)
  {
    return translate(pt);
  }

  FOG_INLINE IntBox& operator-=(const IntPoint& pt)
  {
    x1 -= pt.x;
    y1 -= pt.y;
    x2 -= pt.x;
    y2 -= pt.y;
    return *this;
  }

  FOG_INLINE bool operator==(const IntBox& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const IntBox& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int x1;
  int y1;
  int x2;
  int y2;
};


struct IntMargins
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE IntMargins()
  {
  }

  FOG_INLINE IntMargins(const IntMargins& other) :
  left(other.left), bottom(other.bottom), right(other.right), top(other.top)
  {
  }

  FOG_INLINE IntMargins(int rleft, int rright, int rtop, int rbottom) :
  left(rleft), bottom(rbottom), right(rright), top(rtop)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isEmpty() const { return left == 0 && top == 0 && right == 0 && bottom == 0; }

  FOG_INLINE int getLeft() const { return left; }
  FOG_INLINE int getTop() const { return top; }
  FOG_INLINE int getRight() const { return right; }
  FOG_INLINE int getBottom() const { return bottom; }

  FOG_INLINE IntMargins& set(int rleft, int rright, int rtop, int rbottom)
  { 
    left = rleft; 
    bottom = rbottom; 
    right = rright; 
    top = rtop; 

    return *this;
  }

  FOG_INLINE IntMargins& set(const IntMargins &other)
  {
    if (sizeof(IntMargins) == 16)
    {
      Memory::copy16B(static_cast<void*>(this), static_cast<const void*>(&other));
    }
    else
    {
      left = other.left;
      bottom = other.bottom;
      right = other.right;
      top = other.top;
    }
    return *this;
  }

  FOG_INLINE IntMargins& setLeft(int left) { left = left; return *this; }
  FOG_INLINE IntMargins& setTop(int top) { top = top; return *this; }
  FOG_INLINE IntMargins& setRight(int right) { right = right; return *this; }
  FOG_INLINE IntMargins& setBottom(int bottom) { bottom = bottom; return *this; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  FOG_INLINE IntMargins& clear()
  {
    if (sizeof(IntMargins) == 16)
    {
      Memory::zero16B(static_cast<void*>(this));
    }
    else
    {
      left = 0;
      bottom = 0;
      right = 0;
      top = 0;
    }
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(int pleft, int pright, int ptop, int pbottom) const
  {
    return (left == ptop) & (bottom == pbottom) & (right == pright) & (top == ptop);
  }

  FOG_INLINE bool eq(const IntMargins& other) const
  {
    if (sizeof(IntMargins) == 16)
      return Memory::eq16B(static_cast<const void*>(this), static_cast<const void*>(&other));
    else
      return (left == other.left) & (bottom == other.bottom) & (right == other.right) & (top == other.top);
  }

  FOG_INLINE bool operator==(const IntMargins& other) const { return (left == other.left) & (bottom == other.bottom) & (right == other.right) & (top == other.top); }
  FOG_INLINE bool operator!=(const IntMargins& other) const { return (left != other.left) | (bottom != other.bottom) | (right != other.right) | (top != other.top); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int left;
  int right;
  int top;
  int bottom;
};


// ============================================================================
// [Defined Later]
// ============================================================================

FOG_INLINE FloatPoint IntPoint::toFloatPoint() const { return FloatPoint((float)x, (float)y); }
FOG_INLINE DoublePoint IntPoint::toDoublePoint() const { return DoublePoint((double)x, (double)y); }
FOG_INLINE DoublePoint FloatPoint::toDoublePoint() const { return DoublePoint((double)x, (double)y); }

FOG_INLINE FloatSize IntSize::toFloatSize() const { return FloatSize((float)w, (float)h); }
FOG_INLINE DoubleSize IntSize::toDoubleSize() const { return DoubleSize((double)w, (double)h); }
FOG_INLINE DoubleSize FloatSize::toDoubleSize() const { return DoubleSize((double)w, (double)h); }

FOG_INLINE FloatRect IntRect::toFloatRect() const { return FloatRect((float)x, (float)y, (float)w, (float)h); }
FOG_INLINE DoubleRect IntRect::toDoubleRect() const { return DoubleRect((double)x, (double)y, (double)w, (double)h); }
FOG_INLINE DoubleRect FloatRect::toDoubleRect() const { return DoubleRect((double)x, (double)y, (double)w, (double)h); }

FOG_INLINE IntRect::IntRect(const IntBox& box) : x(box.x1), y(box.y1), w(box.x2 - box.x1), h(box.y2 - box.y1) {}
FOG_INLINE IntBox::IntBox(const IntRect& rect) : x1(rect.x), y1(rect.y), x2(rect.x + rect.w), y2(rect.y + rect.h) {}

} // Fog namespace

// ============================================================================
// [Operator Overload]
// ============================================================================

FOG_INLINE Fog::IntPoint operator+(const Fog::IntPoint& a, const Fog::IntPoint& b) { return Fog::IntPoint(a.x + b.x, a.y + b.y); }
FOG_INLINE Fog::IntPoint operator-(const Fog::IntPoint& a, const Fog::IntPoint& b) { return Fog::IntPoint(a.x - b.x, a.y - b.y); }

FOG_INLINE Fog::FloatPoint operator+(const Fog::FloatPoint& a, const Fog::FloatPoint& b) { return Fog::FloatPoint(a.x + b.x, a.y + b.y); }
FOG_INLINE Fog::FloatPoint operator-(const Fog::FloatPoint& a, const Fog::FloatPoint& b) { return Fog::FloatPoint(a.x - b.x, a.y - b.y); }

FOG_INLINE Fog::DoublePoint operator+(const Fog::DoublePoint& a, const Fog::DoublePoint& b) { return Fog::DoublePoint(a.x + b.x, a.y + b.y); }
FOG_INLINE Fog::DoublePoint operator-(const Fog::DoublePoint& a, const Fog::DoublePoint& b) { return Fog::DoublePoint(a.x - b.x, a.y - b.y); }

FOG_INLINE Fog::IntSize operator+(const Fog::IntSize& a, const Fog::IntSize& b) { return Fog::IntSize(a.w + b.w, a.h + b.h); }
FOG_INLINE Fog::IntSize operator-(const Fog::IntSize& a, const Fog::IntSize& b) { return Fog::IntSize(a.w - b.w, a.h - b.h); }

FOG_INLINE Fog::FloatSize operator+(const Fog::FloatSize& a, const Fog::FloatSize& b) { return Fog::FloatSize(a.w + b.w, a.h + b.h); }
FOG_INLINE Fog::FloatSize operator-(const Fog::FloatSize& a, const Fog::FloatSize& b) { return Fog::FloatSize(a.w - b.w, a.h - b.h); }

FOG_INLINE Fog::DoubleSize operator+(const Fog::DoubleSize& a, const Fog::DoubleSize& b) { return Fog::DoubleSize(a.w + b.w, a.h + b.h); }
FOG_INLINE Fog::DoubleSize operator-(const Fog::DoubleSize& a, const Fog::DoubleSize& b) { return Fog::DoubleSize(a.w - b.w, a.h - b.h); }

FOG_INLINE Fog::IntRect operator+(const Fog::IntRect& a, const Fog::IntPoint& b) { return Fog::IntRect(a.x + b.x, a.y + b.y, a.w, a.h); }
FOG_INLINE Fog::IntRect operator-(const Fog::IntRect& a, const Fog::IntPoint& b) { return Fog::IntRect(a.x - b.x, a.y - b.y, a.w, a.h); }

FOG_INLINE Fog::FloatRect operator+(const Fog::FloatRect& a, const Fog::FloatPoint& b) { return Fog::FloatRect(a.x + b.x, a.y + b.y, a.w, a.h); }
FOG_INLINE Fog::FloatRect operator-(const Fog::FloatRect& a, const Fog::FloatPoint& b) { return Fog::FloatRect(a.x - b.x, a.y - b.y, a.w, a.h); }

FOG_INLINE Fog::DoubleRect operator+(const Fog::DoubleRect& a, const Fog::DoublePoint& b) { return Fog::DoubleRect(a.x + b.x, a.y + b.y, a.w, a.h); }
FOG_INLINE Fog::DoubleRect operator-(const Fog::DoubleRect& a, const Fog::DoublePoint& b) { return Fog::DoubleRect(a.x - b.x, a.y - b.y, a.w, a.h); }

FOG_INLINE Fog::IntBox operator+(const Fog::IntBox& a, const Fog::IntPoint& b) { return Fog::IntBox(a.x1 + b.x, a.y1 + b.y, a.x2 + b.x, a.y2 + b.y); }
FOG_INLINE Fog::IntBox operator-(const Fog::IntBox& a, const Fog::IntPoint& b) { return Fog::IntBox(a.x1 - b.x, a.y1 - b.y, a.x2 - b.x, a.y2 - b.y); }

//! @}

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::IntPoint   , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::FloatPoint , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::DoublePoint, Fog::TYPEINFO_PRIMITIVE)

FOG_DECLARE_TYPEINFO(Fog::IntSize    , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::FloatSize  , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::DoubleSize , Fog::TYPEINFO_PRIMITIVE)

FOG_DECLARE_TYPEINFO(Fog::IntRect    , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::FloatRect  , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::DoubleRect , Fog::TYPEINFO_PRIMITIVE)

FOG_DECLARE_TYPEINFO(Fog::IntBox     , Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GRAPHICS_GEOMETRY_H
