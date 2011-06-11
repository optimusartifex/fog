// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_ELLIPSE_H
#define _FOG_G2D_GEOMETRY_ELLIPSE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct EllipseF;
struct EllipseD;

// ============================================================================
// [Fog::EllipseF]
// ============================================================================

//! @brief Ellipse (float).
struct FOG_NO_EXPORT EllipseF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE EllipseF() { reset(); }
  FOG_INLINE EllipseF(_Uninitialized) {}

  FOG_INLINE EllipseF(const EllipseF& other) { center = other.center; radius = other.radius; }
  FOG_INLINE EllipseF(const PointF& cp, const PointF& rp) { setEllipse(cp, rp); }
  FOG_INLINE EllipseF(const PointF& cp, float rad) { setEllipse(cp, rad); }

  explicit FOG_INLINE EllipseF(const RectF& r) { setEllipse(r); }
  explicit FOG_INLINE EllipseF(const BoxF& r) { setEllipse(r); }

  explicit FOG_INLINE EllipseF(const EllipseD& ellipse);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getCenter() const { return center; }
  FOG_INLINE const PointF& getRadius() const { return radius; }

  FOG_INLINE void setCenter(const PointF& cp) { center = cp; }
  FOG_INLINE void setRadius(const PointF& rp) { radius = rp; }
  FOG_INLINE void setRadius(float rad) { radius.set(rad, rad); }

  FOG_INLINE void setEllipse(const EllipseF& ellipse) { center = ellipse.center; radius = ellipse.radius; }
  FOG_INLINE void setEllipse(const EllipseD& ellipse);
  FOG_INLINE void setEllipse(const PointF& cp, const PointF& rp) { center = cp; radius = rp; }

  FOG_INLINE void setEllipse(const CircleF& circle) { center = circle.center; radius.set(circle.radius, circle.radius); }
  FOG_INLINE void setEllipse(const PointF& cp, float rad) { center = cp; radius.set(rad, rad); }

  FOG_INLINE void setEllipse(const RectF& r) { radius.set(r.w * 0.5f, r.h * 0.5f); center.set(r.x + radius.x, r.y + radius.y); }
  FOG_INLINE void setEllipse(const BoxF& r) { radius.set(r.getWidth() * 0.5f, r.getHeight() * 0.5f); center.set(r.x0 + radius.x, r.y0 + radius.y); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { center.reset(); radius.reset(); }

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
    return _g2d.ellipsef.getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = _g2d.ellipsef.getBoundingBox(this, reinterpret_cast<BoxF*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt) const
  {
    return _g2d.ellipsef.hitTest(this, &pt);
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
    return _g2d.ellipsef.toCSpline(this, dst);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE EllipseF& operator=(const EllipseF& other)
  {
    setEllipse(other);
    return *this;
  }

  FOG_INLINE bool operator==(const EllipseF& other)
  {
    return center == other.center && radius == other.radius;
  }

  FOG_INLINE bool operator!=(const EllipseF& other)
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF center;
  PointF radius;
};

// ============================================================================
// [Fog::EllipseD]
// ============================================================================

//! @brief Ellipse (double).
struct FOG_NO_EXPORT EllipseD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE EllipseD() { reset(); }
  FOG_INLINE EllipseD(_Uninitialized) {}

  FOG_INLINE EllipseD(const EllipseD& other) { center = other.center; radius = other.radius; }
  FOG_INLINE EllipseD(const PointD& cp, const PointD& rp) { setEllipse(cp, rp); }
  FOG_INLINE EllipseD(const PointD& cp, double rad) { setEllipse(cp, rad); }

  explicit FOG_INLINE EllipseD(const RectD& r) { setEllipse(r); }
  explicit FOG_INLINE EllipseD(const BoxD& r) { setEllipse(r); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointD& getCenter() const { return center; }
  FOG_INLINE const PointD& getRadius() const { return radius; }

  FOG_INLINE void setCenter(const PointD& cp) { center = cp; }
  FOG_INLINE void setRadius(const PointD& rp) { radius = rp; }
  FOG_INLINE void setRadius(double rad) { radius.set(rad, rad); }

  FOG_INLINE void setEllipse(const EllipseD& ellipse) { center = ellipse.center; radius = ellipse.radius; }
  FOG_INLINE void setEllipse(const PointD& cp, const PointD& rp) { center = cp; radius = rp; }

  FOG_INLINE void setEllipse(const CircleD& circle) { center = circle.center; radius.set(circle.radius, circle.radius); }
  FOG_INLINE void setEllipse(const PointD& cp, double rad) { center = cp; radius.set(rad, rad); }

  FOG_INLINE void setEllipse(const RectD& r) { radius.set(r.w * 0.5, r.h * 0.5); center.set(r.x + radius.x, r.y + radius.y); }
  FOG_INLINE void setEllipse(const BoxD& r) { radius.set(r.getWidth() * 0.5, r.getHeight() * 0.5); center.set(r.x0 + radius.x, r.y0 + radius.y); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { center.reset(); radius.reset(); }

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
    return _g2d.ellipsed.getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = _g2d.ellipsed.getBoundingBox(this, reinterpret_cast<BoxD*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt) const
  {
    return _g2d.ellipsed.hitTest(this, &pt);
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
    return _g2d.ellipsed.toCSpline(this, dst);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE EllipseD& operator=(const EllipseD& other)
  {
    setEllipse(other);
    return *this;
  }

  FOG_INLINE bool operator==(const EllipseD& other)
  {
    return center == other.center && radius == other.radius;
  }

  FOG_INLINE bool operator!=(const EllipseD& other)
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD center;
  PointD radius;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE EllipseF::EllipseF(const EllipseD& ellipse) :
  center(ellipse.center),
  radius(ellipse.radius)
{
}

FOG_INLINE void EllipseF::setEllipse(const EllipseD& ellipse)
{
  center = ellipse.center;
  radius = ellipse.radius;
}

// ============================================================================
// [Fog::EllipseT<>]
// ============================================================================

FOG_CLASS_PRECISION_F_D(Ellipse)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::EllipseF, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::EllipseD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::EllipseF, Math::feqv((const float *)&a, (const float *)&b, 4))
FOG_FUZZY_DECLARE(Fog::EllipseD, Math::feqv((const double*)&a, (const double*)&b, 4))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_ELLIPSE_H
