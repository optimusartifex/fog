// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_ELLIPSE_H
#define _FOG_G2D_GEOMETRY_ELLIPSE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

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
  FOG_INLINE EllipseF(const EllipseF& other) { center = other.center; radius = other.radius; }
  FOG_INLINE EllipseF(const PointF& cp, const PointF& rp) { setEllipse(cp, rp); }
  FOG_INLINE EllipseF(const PointF& cp, float rad) { setEllipse(cp, rad); }

  FOG_INLINE EllipseF(float x0, float y0, float rad) { setEllipse(x0, y0, rad); }
  FOG_INLINE EllipseF(float x0, float y0, float rx, float ry) { setEllipse(x0, y0, rx, ry); }

  explicit FOG_INLINE EllipseF(_Uninitialized) {}
  explicit FOG_INLINE EllipseF(const EllipseD& ellipse) { setEllipse(ellipse); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getCenter() const { return center; }
  FOG_INLINE const PointF& getRadius() const { return radius; }

  FOG_INLINE void setCenter(const PointF& cp) { center = cp; }
  FOG_INLINE void setRadius(float rad) { radius.set(rad, rad); }
  FOG_INLINE void setRadius(const PointF& rp) { radius = rp; }

  FOG_INLINE void setEllipse(const EllipseF& ellipse) { center = ellipse.center; radius = ellipse.radius; }
  FOG_INLINE void setEllipse(const EllipseD& ellipse);

  FOG_INLINE void setEllipse(const PointF& cp, float rad) { center = cp; radius.set(rad, rad); }
  FOG_INLINE void setEllipse(const PointF& cp, const PointF& rp) { center = cp; radius = rp; }

  FOG_INLINE void setEllipse(float x0, float y0, float rad) { center.set(x0, y0); radius.set(rad, rad); }
  FOG_INLINE void setEllipse(float x0, float y0, float rx, float ry) { center.set(x0, y0); radius.set(rx, ry); }

  FOG_INLINE void setEllipse(const CircleF& circle) { center = circle.center; radius.set(circle.radius, circle.radius); }
  FOG_INLINE void setEllipse(const RectF& r) { radius.set(r.w * 0.5f, r.h * 0.5f); center.set(r.x + radius.x, r.y + radius.y); }
  FOG_INLINE void setEllipse(const BoxF& r) { radius.set(r.getWidth() * 0.5f, r.getHeight() * 0.5f); center.set(r.x0 + radius.x, r.y0 + radius.y); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    center.reset();
    radius.reset();
  }

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
    return fog_api.ellipsef_getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = fog_api.ellipsef_getBoundingBox(this, reinterpret_cast<BoxF*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt) const
  {
    return fog_api.ellipsef_hitTest(this, &pt);
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
    return fog_api.ellipsef_toCSpline(this, dst);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE EllipseF& operator=(const EllipseF& other)
  {
    setEllipse(other);
    return *this;
  }

  FOG_INLINE bool operator==(const EllipseF& other) const { return  MemOps::eq_t<EllipseF>(this, &other); }
  FOG_INLINE bool operator!=(const EllipseF& other) const { return !MemOps::eq_t<EllipseF>(this, &other); }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE EllipseF fromCircle(const CircleF& circle)
  {
    return EllipseF(circle.center.x, circle.center.y, circle.radius, circle.radius);
  }

  static FOG_INLINE EllipseF fromBox(const BoxF& r)
  {
    float rx = r.getWidth() * 0.5f;
    float ry = r.getHeight() * 0.5f;
    return EllipseF(r.x0 + rx, r.y0 + ry, rx, ry);
  }

  static FOG_INLINE EllipseF fromRect(const RectF& r)
  {
    float rx = r.getWidth() * 0.5f;
    float ry = r.getHeight() * 0.5f;
    return EllipseF(r.x + rx, r.y + ry, rx, ry);
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
  FOG_INLINE EllipseD(const EllipseD& other) { center = other.center; radius = other.radius; }
  FOG_INLINE EllipseD(const PointD& cp, const PointD& rp) { setEllipse(cp, rp); }
  FOG_INLINE EllipseD(const PointD& cp, double rad) { setEllipse(cp, rad); }

  FOG_INLINE EllipseD(double x0, double y0, double rad) { setEllipse(x0, y0, rad); }
  FOG_INLINE EllipseD(double x0, double y0, double rx, double ry) { setEllipse(x0, y0, rx, ry); }

  explicit FOG_INLINE EllipseD(_Uninitialized) {}
  explicit FOG_INLINE EllipseD(const EllipseF& ellipse) { setEllipse(ellipse); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointD& getCenter() const { return center; }
  FOG_INLINE const PointD& getRadius() const { return radius; }

  FOG_INLINE void setCenter(const PointD& cp) { center = cp; }
  FOG_INLINE void setRadius(const PointD& rp) { radius = rp; }
  FOG_INLINE void setRadius(double rad) { radius.set(rad, rad); }

  FOG_INLINE void setEllipse(const EllipseD& ellipse) { center = ellipse.center; radius = ellipse.radius; }
  FOG_INLINE void setEllipse(const EllipseF& ellipse) { center = ellipse.center; radius = ellipse.radius; }

  FOG_INLINE void setEllipse(const PointD& cp, double rad) { center = cp; radius.set(rad, rad); }
  FOG_INLINE void setEllipse(const PointD& cp, const PointD& rp) { center = cp; radius = rp; }

  FOG_INLINE void setEllipse(double x0, double y0, double rad) { center.set(x0, y0); radius.set(rad, rad); }
  FOG_INLINE void setEllipse(double x0, double y0, double rx, double ry) { center.set(x0, y0); radius.set(rx, ry); }

  FOG_INLINE void setEllipse(const CircleD& circle) { center = circle.center; radius.set(circle.radius, circle.radius); }
  FOG_INLINE void setEllipse(const RectD& r) { radius.set(r.w * 0.5, r.h * 0.5); center.set(r.x + radius.x, r.y + radius.y); }
  FOG_INLINE void setEllipse(const BoxD& r) { radius.set(r.getWidth() * 0.5, r.getHeight() * 0.5); center.set(r.x0 + radius.x, r.y0 + radius.y); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    center.reset();
    radius.reset();
  }

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
    return fog_api.ellipsed_getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = fog_api.ellipsed_getBoundingBox(this, reinterpret_cast<BoxD*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt) const
  {
    return fog_api.ellipsed_hitTest(this, &pt);
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
    return fog_api.ellipsed_toCSpline(this, dst);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE EllipseD& operator=(const EllipseD& other)
  {
    setEllipse(other);
    return *this;
  }

  FOG_INLINE bool operator==(const EllipseD& other) const { return  MemOps::eq_t<EllipseD>(this, &other); }
  FOG_INLINE bool operator!=(const EllipseD& other) const { return !MemOps::eq_t<EllipseD>(this, &other); }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE EllipseD fromCircle(const CircleD& circle)
  {
    return EllipseD(circle.center.x, circle.center.y, circle.radius, circle.radius);
  }

  static FOG_INLINE EllipseD fromBox(const BoxD& r)
  {
    double rx = r.getWidth() * 0.5;
    double ry = r.getHeight() * 0.5;
    return EllipseD(r.x0 + rx, r.y0 + ry, rx, ry);
  }

  static FOG_INLINE EllipseD fromRect(const RectD& r)
  {
    double rx = r.getWidth() * 0.5;
    double ry = r.getHeight() * 0.5;
    return EllipseD(r.x + rx, r.y + ry, rx, ry);
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

FOG_INLINE void EllipseF::setEllipse(const EllipseD& ellipse)
{
  center = ellipse.center;
  radius = ellipse.radius;
}

// ============================================================================
// [Fog::EllipseT<>]
// ============================================================================

_FOG_NUM_T(Ellipse)
_FOG_NUM_F(Ellipse)
_FOG_NUM_D(Ellipse)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::EllipseF, 4)
FOG_FUZZY_DECLARE_D_VEC(Fog::EllipseD, 4)

// [Guard]
#endif // _FOG_G2D_GEOMETRY_ELLIPSE_H
