// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_ELLIPSE_H
#define _FOG_G2D_GEOMETRY_ELLIPSE_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
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
  // [BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF getBoundingRect() const
  {
    float x0 = center.x - radius.x;
    float y0 = center.y - radius.y;
    float x1 = center.x + radius.x;
    float y1 = center.y + radius.y;

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
  // [BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD getBoundingRect() const
  {
    double x0 = center.x - radius.x;
    double y0 = center.y - radius.y;
    double x1 = center.x + radius.x;
    double y1 = center.y + radius.y;

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

FOG_TYPEVARIANT_DECLARE_F_D(Ellipse)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::EllipseF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::EllipseD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::EllipseF, Math::feqv((const float *)&a, (const float *)&b, 4))
FOG_DECLARE_FUZZY(Fog::EllipseD, Math::feqv((const double*)&a, (const double*)&b, 4))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_ELLIPSE_H
