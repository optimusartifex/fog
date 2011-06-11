// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_ROUND_H
#define _FOG_G2D_GEOMETRY_ROUND_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
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

struct RoundF;
struct RoundD;

// ============================================================================
// [Fog::RoundF]
// ============================================================================

//! @brief Rounded rectangle (float).
struct FOG_NO_EXPORT RoundF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RoundF() { reset(); }
  FOG_INLINE RoundF(_Uninitialized) {}

  FOG_INLINE RoundF(const RoundF& other) { rect = other.rect; radius = other.radius; }

  FOG_INLINE RoundF(const RectF& r, const PointF& rp) { rect = r; radius = rp; }
  FOG_INLINE RoundF(const RectF& r, float rad) { rect = r; radius.set(rad, rad); }

  FOG_INLINE RoundF(float rx, float ry, float rw, float rh, float rad) { rect.setRect(rx, ry, rw, rh); radius.set(rad, rad); }
  FOG_INLINE RoundF(float rx, float ry, float rw, float rh, float radx, float rady) { rect.setRect(rx, ry, rw, rh); radius.set(radx, rady); }

  explicit RoundF(const RoundD& other) { setRound(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const RectF& getRect() const { return rect; }
  FOG_INLINE const PointF& getRadius() const { return radius; }

  FOG_INLINE void setRect(const RectF& r) { rect = r; }
  FOG_INLINE void setRadius(const PointF& rp) { radius = rp; }
  FOG_INLINE void setRadius(float rad) { radius.set(rad, rad); }

  FOG_INLINE void setRound(const RoundF& other) { rect = other.rect; radius = other.radius; }
  FOG_INLINE void setRound(const RoundD& other);

  FOG_INLINE void setRound(const RectF& r, const PointF& rp) { rect = r; radius = rp; }
  FOG_INLINE void setRound(const RectF& r, float rad) { rect = r; radius.set(rad, rad); }

  FOG_INLINE void setRound(float rx, float ry, float rw, float rh, float rad) { rect.setRect(rx, ry, rw, rh); radius.set(rad, rad); }
  FOG_INLINE void setRound(float rx, float ry, float rw, float rh, float radx, float rady) { rect.setRect(rx, ry, rw, rh); radius.set(radx, rady); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    rect.reset();
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
    return _g2d.roundf.getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = _g2d.roundf.getBoundingBox(this, reinterpret_cast<BoxF*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt) const
  {
    return _g2d.roundf.hitTest(this, &pt);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointF& pt)
  {
    rect.translate(pt);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RoundF& operator=(const RoundF& other)
  {
    setRound(other);
    return *this;
  }

  FOG_INLINE bool operator==(const RoundF& other)
  {
    return rect == other.rect && radius == other.radius;
  }

  FOG_INLINE bool operator!=(const RoundF& other)
  {
    return rect != other.rect || radius != other.radius;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectF rect;
  PointF radius;
};

// ============================================================================
// [Fog::RoundD]
// ============================================================================

//! @brief Rounded rectangle (double).
struct FOG_NO_EXPORT RoundD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RoundD() { reset(); }
  FOG_INLINE RoundD(_Uninitialized) {}

  FOG_INLINE RoundD(const RoundD& other) { rect = other.rect; radius = other.radius; }

  FOG_INLINE RoundD(const RectD& r, const PointD& rp) { rect = r; radius = rp; }
  FOG_INLINE RoundD(const RectD& r, double rad) { rect = r; radius.set(rad, rad); }

  FOG_INLINE RoundD(double rx, double ry, double rw, double rh, double rad) { rect.setRect(rx, ry, rw, rh); radius.set(rad, rad); }
  FOG_INLINE RoundD(double rx, double ry, double rw, double rh, double radx, double rady) { rect.setRect(rx, ry, rw, rh); radius.set(radx, rady); }

  explicit FOG_INLINE RoundD(const RoundF& other) { rect = other.rect; radius = other.radius; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const RectD& getRect() const { return rect; }
  FOG_INLINE const PointD& getRadius() const { return radius; }

  FOG_INLINE void setRect(const RectD& r) { rect = r; }
  FOG_INLINE void setRadius(const PointD& rp) { radius = rp; }
  FOG_INLINE void setRadius(double rad) { radius.set(rad, rad); }

  FOG_INLINE void setRound(const RoundF& other) { rect = other.rect; radius = other.radius; }
  FOG_INLINE void setRound(const RoundD& other) { rect = other.rect; radius = other.radius; }

  FOG_INLINE void setRound(const RectD& r, const PointD& rp) { rect = r; radius = rp; }
  FOG_INLINE void setRound(const RectD& r, double rad) { rect = r; radius.set(rad, rad); }

  FOG_INLINE void setRound(double rx, double ry, double rw, double rh, double rad) { rect.setRect(rx, ry, rw, rh); radius.set(rad, rad); }
  FOG_INLINE void setRound(double rx, double ry, double rw, double rh, double radx, double rady) { rect.setRect(rx, ry, rw, rh); radius.set(radx, rady); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    rect.reset();
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
    return _g2d.roundd.getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = _g2d.roundd.getBoundingBox(this, reinterpret_cast<BoxD*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt) const
  {
    return _g2d.roundd.hitTest(this, &pt);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointD& pt)
  {
    rect.translate(pt);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RoundD& operator=(const RoundD& other)
  {
    setRound(other);
    return *this;
  }

  FOG_INLINE bool operator==(const RoundD& other)
  {
    return rect == other.rect && radius == other.radius;
  }

  FOG_INLINE bool operator!=(const RoundD& other)
  {
    return rect != other.rect || radius != other.radius;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectD rect;
  PointD radius;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE void RoundF::setRound(const RoundD& other)
{
  rect = other.rect;
  radius = other.radius;
}

// ============================================================================
// [Fog::RoundT<>]
// ============================================================================

FOG_CLASS_PRECISION_F_D(Round)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::RoundF, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::RoundD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::RoundF, Math::feqv((const float *)&a, (const float *)&b, 6))
FOG_FUZZY_DECLARE(Fog::RoundD, Math::feqv((const double*)&a, (const double*)&b, 6))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_ROUND_H
