// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_CIRCLE_H
#define _FOG_G2D_GEOMETRY_CIRCLE_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct CircleF;
struct CircleD;

// ============================================================================
// [Fog::CircleF]
// ============================================================================

//! @brief Circle (float).
struct FOG_NO_EXPORT CircleF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE CircleF() { reset(); }
  FOG_INLINE CircleF(_Uninitialized) {}

  FOG_INLINE CircleF(const CircleF& other) { center = other.center; radius = other.radius; }
  FOG_INLINE CircleF(const PointF& cp, float rad) { center = cp; radius = rad; }

  explicit FOG_INLINE CircleF(const CircleD& circle);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getCenter() const { return center; }
  FOG_INLINE float getRadius() const { return radius; }

  FOG_INLINE void setCenter(const PointF& cp) { center = cp; }
  FOG_INLINE void setRadius(float rad) { radius = rad; }

  FOG_INLINE void setCircle(const CircleF& circle) { center = circle.center; radius = circle.radius; }
  FOG_INLINE void setCircle(const PointF& cp, float rad) { center = cp; radius = rad; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { center.reset(); radius = 0.0f; }

  // --------------------------------------------------------------------------
  // [BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF getBoundingRect() const
  {
    float x0 = center.x - radius;
    float y0 = center.y - radius;
    float x1 = center.x + radius;
    float y1 = center.y + radius;

    return RectF(x0, y0, x1 - x0, y1 - y0);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointF& pt)
  {
    center += pt;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE CircleF& operator=(const CircleF& other)
  {
    setCircle(other);
    return *this;
  }

  FOG_INLINE bool operator==(const CircleF& other)
  {
    return center == other.center && radius == other.radius;
  }

  FOG_INLINE bool operator!=(const CircleF& other)
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF center;
  float radius;
};

// ============================================================================
// [Fog::CircleD]
// ============================================================================

//! @brief Circle (double).
struct FOG_NO_EXPORT CircleD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE CircleD()
  {
    reset();
  }

  FOG_INLINE CircleD(_Uninitialized)
  {
  }

  FOG_INLINE CircleD(const CircleD& other) : 
    center(other.center),
    radius(other.radius)
  {
  }
  
  FOG_INLINE CircleD(const PointD& cp, double r) :
    center(cp),
    radius(r)
  {
  }

  explicit FOG_INLINE CircleD(const CircleF& circle) :
    center(circle.center),
    radius(circle.radius)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointD& getCenter() const { return center; }
  FOG_INLINE double getRadius() const { return radius; }

  FOG_INLINE void setCenter(const PointD& cp) { center = cp; }
  FOG_INLINE void setRadius(double r) { radius = r; }

  FOG_INLINE void setCircle(const CircleD& circle) { center = circle.center; radius = circle.radius; }
  FOG_INLINE void setCircle(const PointD& cp, double r) { center = cp; radius = r; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { center.reset(); radius = 0.0; }

  // --------------------------------------------------------------------------
  // [BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD getBoundingRect() const
  {
    double x0 = center.x - radius;
    double y0 = center.y - radius;
    double x1 = center.x + radius;
    double y1 = center.y + radius;

    return RectD(x0, y0, x1 - x0, y1 - y0);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointD& pt)
  {
    center += pt;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE CircleD& operator=(const CircleD& other)
  {
    setCircle(other);
    return *this;
  }

  FOG_INLINE bool operator==(const CircleD& other)
  {
    return center == other.center && radius == other.radius;
  }

  FOG_INLINE bool operator!=(const CircleD& other)
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD center;
  double radius;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE CircleF::CircleF(const CircleD& circle) :
  center(float(circle.center.x), float(circle.center.y)),
  radius(float(circle.radius))
{
}

// ============================================================================
// [Fog::CircleT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(Circle)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::CircleF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::CircleD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::CircleF, Math::feqv((const float *)&a, (const float *)&b, 3))
FOG_DECLARE_FUZZY(Fog::CircleD, Math::feqv((const double*)&a, (const double*)&b, 3))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_CIRCLE_H
