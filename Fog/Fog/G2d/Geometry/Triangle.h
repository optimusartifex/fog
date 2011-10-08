// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_TRIANGLE_H
#define _FOG_G2D_GEOMETRY_TRIANGLE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::Forward Declarations]
// ============================================================================

struct TriangleF;
struct TriangleD;

// ============================================================================
// [Fog::TriangleF]
// ============================================================================

//! @brief Triangle (float).
struct FOG_NO_EXPORT TriangleF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE TriangleF() { reset(); }

  FOG_INLINE TriangleF(const PointF& pt0, const PointF& pt1, const PointF& pt2)
  {
    setTriangle(pt0, pt1, pt2);
  }

  FOG_INLINE TriangleF(float p0x, float p0y, float p1x, float p1y, float p2x, float p2y)
  {
    setTriangle(p0x, p0y, p1x, p1y, p2x, p2y);
  }

  FOG_INLINE TriangleF(const TriangleF& other)
  {
    setTriangle(other);
  }

  explicit FOG_INLINE TriangleF(_Uninitialized) {}
  explicit FOG_INLINE TriangleF(const TriangleD& other) { setTriangle(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void setTriangle(const TriangleF& other)
  {
    p[0] = other.p[0];
    p[1] = other.p[1];
    p[2] = other.p[2];
  }

  // Implemented-Later.
  FOG_INLINE void setTriangle(const TriangleD& other);

  FOG_INLINE void setTriangle(const PointF& pt0, const PointF& pt1, const PointF& pt2)
  {
    p[0] = pt0;
    p[1] = pt1;
    p[2] = pt2;
  }

  FOG_INLINE void setTriangle(float p0x, float p0y, float p1x, float p1y, float p2x, float p2y)
  {
    p[0].set(p0x, p0y);
    p[1].set(p1x, p1y);
    p[2].set(p2x, p2y);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    p[0].reset();
    p[1].reset();
    p[2].reset();
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
    return _api.trianglef_getBoundingBox(this->p, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = _api.trianglef_getBoundingBox(this->p, reinterpret_cast<BoxF*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt) const
  {
    return _api.trianglef_hitTest(this->p, &pt);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointF& pt)
  {
    p[0].translate(pt);
    p[1].translate(pt);
    p[2].translate(pt);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TriangleF& operator=(const TriangleF& other) { setTriangle(other); return *this; }
  FOG_INLINE TriangleF& operator=(const TriangleD& other) { setTriangle(other); return *this; }

  FOG_INLINE bool operator==(const TriangleF& other) const { return  MemOps::eq_t<TriangleF>(this, &other); }
  FOG_INLINE bool operator!=(const TriangleF& other) const { return !MemOps::eq_t<TriangleF>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF p[3];
};

// ============================================================================
// [Fog::TriangleD]
// ============================================================================

//! @brief Triangle (float).
struct FOG_NO_EXPORT TriangleD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE TriangleD() { reset(); }

  FOG_INLINE TriangleD(const PointD& pt0, const PointD& pt1, const PointD& pt2)
  {
    setTriangle(pt0, pt1, pt2);
  }

  FOG_INLINE TriangleD(double p0x, double p0y, double p1x, double p1y, double p2x, double p2y)
  {
    setTriangle(p0x, p0y, p1x, p1y, p2x, p2y);
  }

  FOG_INLINE TriangleD(const TriangleD& other)
  {
    setTriangle(other);
  }

  explicit FOG_INLINE TriangleD(const TriangleF& other) { setTriangle(other); }
  explicit FOG_INLINE TriangleD(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void setTriangle(const TriangleF& other)
  {
    p[0] = other.p[0];
    p[1] = other.p[1];
    p[2] = other.p[2];
  }

  FOG_INLINE void setTriangle(const TriangleD& other)
  {
    p[0] = other.p[0];
    p[1] = other.p[1];
    p[2] = other.p[2];
  }

  FOG_INLINE void setTriangle(const PointD& pt0, const PointD& pt1, const PointD& pt2)
  {
    p[0] = pt0;
    p[1] = pt1;
    p[2] = pt2;
  }

  FOG_INLINE void setTriangle(double p0x, double p0y, double p1x, double p1y, double p2x, double p2y)
  {
    p[0].set(p0x, p0y);
    p[1].set(p1x, p1y);
    p[2].set(p2x, p2y);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    p[0].reset();
    p[1].reset();
    p[2].reset();
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
    return _api.triangled_getBoundingBox(this->p, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = _api.triangled_getBoundingBox(this->p, reinterpret_cast<BoxD*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt) const
  {
    return _api.triangled_hitTest(this->p, &pt);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointD& pt)
  {
    p[0].translate(pt);
    p[1].translate(pt);
    p[2].translate(pt);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TriangleD& operator=(const TriangleF& other) { setTriangle(other); return *this; }
  FOG_INLINE TriangleD& operator=(const TriangleD& other) { setTriangle(other); return *this; }

  FOG_INLINE bool operator==(const TriangleD& other) const { return  MemOps::eq_t<TriangleD>(this, &other); }
  FOG_INLINE bool operator!=(const TriangleD& other) const { return !MemOps::eq_t<TriangleD>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD p[3];
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE void TriangleF::setTriangle(const TriangleD& other)
{
  p[0] = other.p[0];
  p[1] = other.p[1];
  p[2] = other.p[2];
}

// ============================================================================
// [Fog::TriangleT<>]
// ============================================================================

_FOG_NUM_T(Triangle)
_FOG_NUM_F(Triangle)
_FOG_NUM_D(Triangle)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::TriangleF, 6);
FOG_FUZZY_DECLARE_D_VEC(Fog::TriangleD, 6);

// [Guard]
#endif // _FOG_G2D_GEOMETRY_TRIANGLE_H
