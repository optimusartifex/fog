// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_CIRCLE_H
#define _FOG_G2D_GEOMETRY_CIRCLE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Ops.h>
#include <Fog/G2d/Geometry/Box.h>
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
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxF& dst) const
  {
    return _getBoundingBox(dst, NULL);
  }

  FOG_INLINE err_t getBoundingBox(BoxF& dst, const TransformF& tr) const
  {
    return _getBoundingBox(dst, &tr);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst) const
  {
    return _getBoundingRect(dst, NULL);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst, const TransformF& tr) const
  {
    return _getBoundingRect(dst, &tr);
  }

  FOG_INLINE err_t _getBoundingBox(BoxF& dst, const TransformF* tr) const
  {
    return _g2d.circlef.getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = _g2d.circlef.getBoundingBox(this, reinterpret_cast<BoxF*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt) const
  {
    return _g2d.circlef.hitTest(this, &pt);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointF& pt)
  {
    center += pt;
  }

  // --------------------------------------------------------------------------
  // [ToCSpline]
  // --------------------------------------------------------------------------

  FOG_INLINE uint toCSpline(PointF* dst) const
  {
    return _g2d.circlef.toCSpline(this, dst);
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
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxD& dst) const
  {
    return _getBoundingBox(dst, NULL);
  }

  FOG_INLINE err_t getBoundingBox(BoxD& dst, const TransformD& tr) const
  {
    return _getBoundingBox(dst, &tr);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst) const
  {
    return _getBoundingRect(dst, NULL);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst, const TransformD& tr) const
  {
    return _getBoundingRect(dst, &tr);
  }

  FOG_INLINE err_t _getBoundingBox(BoxD& dst, const TransformD* tr) const
  {
    return _g2d.circled.getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = _g2d.circled.getBoundingBox(this, reinterpret_cast<BoxD*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt) const
  {
    return _g2d.circled.hitTest(this, &pt);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointD& pt)
  {
    center += pt;
  }

  // --------------------------------------------------------------------------
  // [ToCSpline]
  // --------------------------------------------------------------------------

  FOG_INLINE uint toCSpline(PointD* dst) const
  {
    return _g2d.circled.toCSpline(this, dst);
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

FOG_CLASS_PRECISION_F_D(Circle)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::CircleF, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::CircleD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::CircleF, Math::feqv((const float *)&a, (const float *)&b, 3))
FOG_FUZZY_DECLARE(Fog::CircleD, Math::feqv((const double*)&a, (const double*)&b, 3))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_CIRCLE_H
