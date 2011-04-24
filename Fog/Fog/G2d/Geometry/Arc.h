// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_ARC_H
#define _FOG_G2D_GEOMETRY_ARC_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ArcF;
struct ArcD;

// ============================================================================
// [Fog::ArcF]
// ============================================================================

//! @brief Arc (float).
struct FOG_NO_EXPORT ArcF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ArcF() { reset(); }
  FOG_INLINE ArcF(_Uninitialized) {}

  FOG_INLINE ArcF(const ArcF& other) { setArc(other); }
  explicit FOG_INLINE ArcF(const ArcD& other) { setArc(other); }

  FOG_INLINE ArcF(const PointF& cp, float rad, float start_, float sweep_)
  {
    setEllipse(cp, rad);
    start = start_;
    sweep = sweep_;
  }

  FOG_INLINE ArcF(const PointF& cp, const PointF& rp, float start_, float sweep_)
  {
    setEllipse(cp, rp);
    start = start_;
    sweep = sweep_;
  }

  FOG_INLINE ArcF(const RectF& r, float start_, float sweep_)
  {
    setEllipse(r);
    start = start_;
    sweep = sweep_;
  }

  FOG_INLINE ArcF(const BoxF& r, float start_, float sweep_)
  {
    setEllipse(r);
    start = start_;
    sweep = sweep_;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getCenter() const { return center; }
  FOG_INLINE const PointF& getRadius() const { return radius; }
  FOG_INLINE float getStart() const { return start; }
  FOG_INLINE float getSweep() const { return sweep; }

  FOG_INLINE void setCenter(const PointF& cp) { center = cp; }
  FOG_INLINE void setRadius(const PointF& rp) { radius = rp; }
  FOG_INLINE void setRadius(float rad) { radius.set(rad, rad); }

  FOG_INLINE void setEllipse(const CircleF& circle) { center = circle.center; radius.set(circle.radius, circle.radius); }
  FOG_INLINE void setEllipse(const EllipseF& ellipse) { center = ellipse.center; radius = ellipse.radius; }

  FOG_INLINE void setEllipse(const PointF& cp, float rad) { center = cp; radius.set(rad, rad); }
  FOG_INLINE void setEllipse(const PointF& cp, const PointF& rp) { center = cp; radius = rp; }

  FOG_INLINE void setEllipse(const RectF& r) { radius.set(r.w * 0.5f, r.h * 0.5f); center.set(r.x + radius.x, r.y + radius.y); }
  FOG_INLINE void setEllipse(const BoxF& r) { radius.set(r.getWidth() * 0.5f, r.getHeight() * 0.5f); center.set(r.x0 + radius.x, r.y0 + radius.y); }

  FOG_INLINE void setStart(float start_) { start = start_; }
  FOG_INLINE void setSweep(float sweep_) { sweep = sweep_; }

  FOG_INLINE void setArc(const ArcF& other)
  {
    center = other.center;
    radius = other.radius;
    start = other.start;
    sweep = other.sweep;
  }

  FOG_INLINE void setArc(const ArcD& other);

  FOG_INLINE void setArc(const PointF& cp, float rad, float start_, float sweep_)
  {
    center = cp;
    radius.set(rad, rad);
    start = start_;
    sweep = sweep_;
  }

  FOG_INLINE void setArc(const PointF& cp, const PointF& rad, float start_, float sweep_)
  {
    center = cp;
    radius = rad;
    start = start_;
    sweep = sweep_;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    center.reset();
    radius.reset();
    start = 0.0f;
    sweep = 0.0f;
  }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxF getBoundingBox() const
  {
    BoxF result;
    _g2d.arcf.getBoundingBox(this, &result, false);
    return result;
  }

  FOG_INLINE RectF getBoundingRect() const
  {
    BoxF result;
    _g2d.arcf.getBoundingBox(this, &result, false);
    return RectF(result);
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

  FOG_INLINE ArcF& operator=(const ArcF& other) { setArc(other); return *this; }
  FOG_INLINE ArcF& operator=(const ArcD& other) { setArc(other); return *this; }

  FOG_INLINE bool operator==(const ArcF& other)
  {
    return center == other.center &&
           radius == other.radius &&
           start  == other.start  &&
           sweep  == other.sweep;
  }

  FOG_INLINE bool operator!=(const ArcF& other)
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF center;
  PointF radius;
  float start;
  float sweep;
};

// ============================================================================
// [Fog::ArcD]
// ============================================================================

//! @brief Arc base (double).
struct FOG_NO_EXPORT ArcD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ArcD() { reset(); }
  FOG_INLINE ArcD(_Uninitialized) {}

  FOG_INLINE ArcD(const ArcD& other) { setArc(other); }

  FOG_INLINE ArcD(const PointD& cp, double rad, double start_, double sweep_)
  {
    setEllipse(cp, rad);
    start = start_;
    sweep = sweep_;
  }

  FOG_INLINE ArcD(const PointD& cp, const PointD& rad, double start_, double sweep_)
  {
    setEllipse(cp, rad);
    start = start_;
    sweep = sweep_;
  }

  FOG_INLINE ArcD(const RectD& r, double start_, double sweep_)
  {
    setEllipse(r);
    start = start_;
    sweep = sweep_;
  }

  FOG_INLINE ArcD(const BoxD& r, double start_, double sweep_)
  {
    setEllipse(r);
    start = start_;
    sweep = sweep_;
  }

  explicit FOG_INLINE ArcD(const ArcF& other) { setArc(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointD& getCenter() const { return center; }
  FOG_INLINE const PointD& getRadius() const { return radius; }
  FOG_INLINE double getStart() const { return start; }
  FOG_INLINE double getSweep() const { return sweep; }

  FOG_INLINE void setCenter(const PointD& cp) { center = cp; }
  FOG_INLINE void setRadius(const PointD& rp) { radius = rp; }
  FOG_INLINE void setRadius(double rad) { radius.set(rad, rad); }

  FOG_INLINE void setEllipse(const CircleD& circle) { center = circle.center; radius.set(circle.radius, circle.radius); }
  FOG_INLINE void setEllipse(const EllipseD& ellipse) { center = ellipse.center; radius = ellipse.radius; }

  FOG_INLINE void setEllipse(const PointD& cp, double rad) { center = cp; radius.set(rad, rad); }
  FOG_INLINE void setEllipse(const PointD& cp, const PointD& rp) { center = cp; radius = rp; }

  FOG_INLINE void setEllipse(const RectD& r) { radius.set(r.w * 0.5, r.h * 0.5); center.set(r.x + radius.x, r.y + radius.y); }
  FOG_INLINE void setEllipse(const BoxD& r) { radius.set(r.getWidth() * 0.5, r.getHeight() * 0.5); center.set(r.x0 + radius.x, r.y0 + radius.y); }

  FOG_INLINE void setStart(double start_) { start = start_; }
  FOG_INLINE void setSweep(double sweep_) { sweep = sweep_; }

  FOG_INLINE void setArc(const ArcD& other)
  {
    center = other.center;
    radius = other.radius;
    start = other.start;
    sweep = other.sweep;
  }

  FOG_INLINE void setArc(const ArcF& other)
  {
    center = other.center;
    radius = other.radius;
    start = other.start;
    sweep = other.sweep;
  }

  FOG_INLINE void setArc(const PointD& cp, double rad, double start_, double sweep_)
  {
    center = cp;
    radius.set(rad, rad);
    start = start_;
    sweep = sweep_;
  }

  FOG_INLINE void setArc(const PointD& cp, const PointD& rad, double start_, double sweep_)
  {
    center = cp;
    radius = rad;
    start = start_;
    sweep = sweep_;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    center.reset();
    radius.reset();
    start = 0.0;
    sweep = 0.0;
  }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxD getBoundingBox() const
  {
    BoxD result;
    _g2d.arcd.getBoundingBox(this, &result, false);
    return result;
  }

  FOG_INLINE RectD getBoundingRect() const
  {
    BoxD result;
    _g2d.arcd.getBoundingBox(this, &result, false);
    return RectD(result);
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

  FOG_INLINE ArcD& operator=(const ArcD& other) { setArc(other); return *this; }
  FOG_INLINE ArcD& operator=(const ArcF& other) { setArc(other); return *this; }

  FOG_INLINE bool operator==(const ArcD& other)
  {
    return center == other.center &&
           radius == other.radius &&
           start  == other.start  &&
           sweep  == other.sweep;
  }

  FOG_INLINE bool operator!=(const ArcD& other)
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD center;
  PointD radius;
  double start;
  double sweep;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE void ArcF::setArc(const ArcD& other)
{
  center = other.center;
  radius = other.radius;
  start = float(other.start);
  sweep = float(other.sweep);
}

// ============================================================================
// [Fog::ArcT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(Arc)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::ArcF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ArcD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::ArcF, Math::feqv((const float *)&a, (const float *)&b, 6))
FOG_DECLARE_FUZZY(Fog::ArcD, Math::feqv((const double*)&a, (const double*)&b, 6))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_ARC_H
