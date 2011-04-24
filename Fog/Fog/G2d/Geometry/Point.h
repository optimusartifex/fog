// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_POINT_H
#define _FOG_G2D_GEOMETRY_POINT_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
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

  FOG_INLINE PointI& set(const PointI& other)
  {
    x = other.x;
    y = other.y;
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

  FOG_INLINE PointF(const PointI& other) : x((float)other.x), y((float)other.y) {}
  FOG_INLINE PointF(const PointF& other) : x(other.x), y(other.y) {}

  FOG_INLINE PointF(int px, int py) : x((float)px), y((float)py) {}
  FOG_INLINE PointF(float px, float py) : x(px), y(py) {}

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

  FOG_INLINE PointF& set(const PointF& other)
  {
    x = other.x;
    y = other.y;
    return *this;
  }

  FOG_INLINE PointF& set(const PointD& other);

  FOG_INLINE PointF& set(int px, int py)
  {
    x = (float)px;
    y = (float)py;
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

  FOG_INLINE PointF& operator=(const PointI& other) { return set(other); }
  FOG_INLINE PointF& operator=(const PointF& other) { return set(other); }
  FOG_INLINE PointF& operator=(const PointD& other) { return set(other); }

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

  FOG_INLINE PointD& operator=(const PointI& other) { return set(other); }
  FOG_INLINE PointD& operator=(const PointF& other) { return set(other); }
  FOG_INLINE PointD& operator=(const PointD& other) { return set(other); }

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

FOG_INLINE PointF& PointF::set(const PointD& other) { return set((float)other.x, (float)other.y); }

FOG_INLINE PointF PointI::toPointF() const { return PointF((float)x, (float)y); }
FOG_INLINE PointD PointI::toPointD() const { return PointD((double)x, (double)y); }
FOG_INLINE PointD PointF::toPointD() const { return PointD((double)x, (double)y); }

// ============================================================================
// [Fog::PointT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D_I(Point)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::PointI, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::PointF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::PointD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::PointF, Math::isFuzzyEq(a.x, b.x) && Math::isFuzzyEq(a.y, b.y))
FOG_DECLARE_FUZZY(Fog::PointD, Math::isFuzzyEq(a.x, b.x) && Math::isFuzzyEq(a.y, b.y))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_POINT_H
