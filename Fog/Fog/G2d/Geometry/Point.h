// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_POINT_H
#define _FOG_G2D_GEOMETRY_POINT_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct PointI;
struct PointF;
struct PointD;

// ============================================================================
// [Fog::PointI]
// ============================================================================

//! @brief Point (32-bit integer version).
struct FOG_NO_EXPORT PointI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PointI() {}
  FOG_INLINE PointI(_Uninitialized) {}

  FOG_INLINE PointI(int px, int py) : x(px), y(py) {}

  FOG_INLINE PointI(const PointI& other)
  {
    x = other.x;
    y = other.y;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getX() const { return x; }
  FOG_INLINE int getY() const { return y; }

  FOG_INLINE void set(const PointI& other)
  {
    x = other.x;
    y = other.y;
  }

  FOG_INLINE void set(int px, int py)
  {
    x = px;
    y = py;
  }

  FOG_INLINE void setX(int px) { x = px; }
  FOG_INLINE void setY(int py) { y = py; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    x = 0;
    y = 0;
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
    return (x == other.x) & (y == other.y);
  }

  FOG_INLINE bool eq(int px, int py) const
  {
    return (x == px) & (y == py);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PointI& operator=(const PointI& other)
  {
    x = other.x;
    y = other.y;
    return *this;
  }

  FOG_INLINE PointI operator+(const PointI& other) const { return PointI(x + other.x, y + other.y); }
  FOG_INLINE PointI operator-(const PointI& other) const { return PointI(x - other.x, y - other.y); }

  FOG_INLINE PointI& operator+=(const PointI& other) { x += other.x; y += other.y; return *this; }
  FOG_INLINE PointI& operator-=(const PointI& other) { x -= other.x; y -= other.y; return *this; }

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

//! @brief Point (float).
struct PointF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF() {}
  FOG_INLINE PointF(_Uninitialized) {}

  FOG_INLINE PointF(const PointF& other) :
    x(other.x),
    y(other.y)
  {
  }

  FOG_INLINE PointF(int px, int py) :
    x(float(px)),
    y(float(py))
  {
  }

  FOG_INLINE PointF(float px, float py) :
    x(px),
    y(py)
  {
  }

  explicit FOG_INLINE PointF(const PointI& other) { set(other); }
  explicit FOG_INLINE PointF(const PointD& other) { set(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getX() const { return x; }
  FOG_INLINE float getY() const { return y; }

  FOG_INLINE void set(const PointI& other)
  {
    x = (float)other.x;
    y = (float)other.y;
  }

  FOG_INLINE void set(const PointF& other)
  {
    x = other.x;
    y = other.y;
  }

  FOG_INLINE void set(const PointD& other);

  FOG_INLINE void set(int px, int py)
  {
    x = (float)px;
    y = (float)py;
  }

  FOG_INLINE void set(float px, float py)
  {
    x = px;
    y = py;
  }

  FOG_INLINE void setX(int px) { x = (float)px; }
  FOG_INLINE void setY(int py) { y = (float)py; }

  FOG_INLINE void setX(float px) { x = px; }
  FOG_INLINE void setY(float py) { y = py; }

  FOG_INLINE void setNaN() { x = y = Math::getQNanF(); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    x = 0.0f;
    y = 0.0f;
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
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF& operator=(const PointI& other) { set(other); return *this; }
  FOG_INLINE PointF& operator=(const PointF& other) { set(other); return *this; }
  FOG_INLINE PointF& operator=(const PointD& other) { set(other); return *this; }

  FOG_INLINE PointF operator+(const PointI& other) const { return PointF(x + (float)other.x, y + (float)other.y); }
  FOG_INLINE PointF operator+(const PointF& other) const { return PointF(x + other.x, y + other.y); }

  FOG_INLINE PointF operator-(const PointI& other) const { return PointF(x - (float)other.x, y - (float)other.y); }
  FOG_INLINE PointF operator-(const PointF& other) const { return PointF(x - other.x, y - other.y); }

  FOG_INLINE PointF& operator+=(const PointI& other) { return translate(other); }
  FOG_INLINE PointF& operator+=(const PointF& other) { return translate(other); }

  FOG_INLINE PointF& operator-=(const PointI& other) { x -= (float)other.x; y -= (float)other.y; return *this; }
  FOG_INLINE PointF& operator-=(const PointF& other) { x -= other.x; y -= other.y; return *this; }

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

//! @brief Point (double).
struct PointD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PointD() {}
  FOG_INLINE PointD(_Uninitialized) {}

  FOG_INLINE PointD(int px, int py) :
    x(double(px)),
    y(double(py))
  {
  }

  FOG_INLINE PointD(float px, float py) :
    x(double(px)),
    y(double(py))
  {
  }

  FOG_INLINE PointD(const PointD& other) :
    x(other.x),
    y(other.y)
  {
  }

  FOG_INLINE PointD(double px, double py) :
    x(px),
    y(py)
  {
  }

  explicit FOG_INLINE PointD(const PointI& other) :
    x((double)other.x),
    y((double)other.y)
  {
  }

  explicit FOG_INLINE PointD(const PointF& other) :
    x((double)other.x),
    y((double)other.y)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getX() const { return x; }
  FOG_INLINE double getY() const { return y; }

  FOG_INLINE void set(const PointI& other)
  {
    x = (double)other.x;
    y = (double)other.y;
  }

  FOG_INLINE void set(const PointF& other)
  {
    x = (double)other.x;
    y = (double)other.y;
  }

  FOG_INLINE void set(const PointD& other)
  {
    x = other.x;
    y = other.y;
  }

  FOG_INLINE void set(double px, double py)
  {
    x = px;
    y = py;
  }

  FOG_INLINE void setX(int px) { x = (double)px; }
  FOG_INLINE void setY(int py) { y = (double)py; }

  FOG_INLINE void setX(float px) { x = (double)px; }
  FOG_INLINE void setY(float py) { y = (double)py; }

  FOG_INLINE void setX(double px) { x = px; }
  FOG_INLINE void setY(double py) { y = py; }

  FOG_INLINE void setNaN() { x = y = Math::getQNanD(); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    x = 0.0;
    y = 0.0;
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
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PointD& operator=(const PointI& other) { set(other); return *this; }
  FOG_INLINE PointD& operator=(const PointF& other) { set(other); return *this; }
  FOG_INLINE PointD& operator=(const PointD& other) { set(other); return *this; }

  FOG_INLINE PointD operator+(const PointI& other) const { return PointD(x + (double)other.x, y + (double)other.y); }
  FOG_INLINE PointD operator+(const PointF& other) const { return PointD(x + (double)other.x, y + (double)other.y); }
  FOG_INLINE PointD operator+(const PointD& other) const { return PointD(x + other.x, y + other.y); }

  FOG_INLINE PointD operator-(const PointI& other) const { return PointD(x - (double)other.x, y - (double)other.y); }
  FOG_INLINE PointD operator-(const PointF& other) const { return PointD(x - (double)other.x, y - (double)other.y); }
  FOG_INLINE PointD operator-(const PointD& other) const { return PointD(x - other.x, y - other.y); }

  FOG_INLINE PointD& operator+=(const PointI& other) { x += (double)other.x; y += (double)other.y; return *this; }
  FOG_INLINE PointD& operator+=(const PointF& other) { x += (double)other.x; y += (double)other.y; return *this; }
  FOG_INLINE PointD& operator+=(const PointD& other) { x += other.x; y += other.y; return *this; }

  FOG_INLINE PointD& operator-=(const PointI& other) { x -= (double)other.x; y -= (double)other.y; return *this; }
  FOG_INLINE PointD& operator-=(const PointF& other) { x -= (double)other.x; y -= (double)other.y; return *this; }
  FOG_INLINE PointD& operator-=(const PointD& other) { x -= other.x; y -= other.y; return *this; }

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
// [Implemented-Later]
// ============================================================================

FOG_INLINE void PointF::set(const PointD& other)
{
  set(float(other.x), float(other.y));
}

// ============================================================================
// [Fog::PointT<>]
// ============================================================================

FOG_CLASS_PRECISION_F_D_I(Point)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::PointI, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::PointF, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::PointD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::PointF, Math::isFuzzyEq(a.x, b.x) && Math::isFuzzyEq(a.y, b.y))
FOG_FUZZY_DECLARE(Fog::PointD, Math::isFuzzyEq(a.x, b.x) && Math::isFuzzyEq(a.y, b.y))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_POINT_H
