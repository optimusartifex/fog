// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_CUBICCURVE_H
#define _FOG_G2D_GEOMETRY_CUBICCURVE_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct CBezierF;
struct CBezierD;

// ============================================================================
// [Fog::CBezierF]
// ============================================================================

struct FOG_NO_EXPORT CBezierF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE CBezierF() {}
  FOG_INLINE CBezierF(_Uninitialized) {}

  FOG_INLINE CBezierF(const PointF& p0, const PointF& p1, const PointF& p2, const PointF& p3)
  {
    p[0] = p0;
    p[1] = p1;
    p[2] = p2;
    p[3] = p3;
  }

  FOG_INLINE CBezierF(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
  {
    p[0].set(x0, y0);
    p[1].set(x1, y1);
    p[2].set(x2, y2);
    p[3].set(x3, y3);
  }

  FOG_INLINE CBezierF(const PointF* pts)
  {
    p[0] = pts[0];
    p[1] = pts[1];
    p[2] = pts[2];
    p[3] = pts[3];
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getX0() const { return p[0].x; }
  FOG_INLINE float getY0() const { return p[0].y; }
  FOG_INLINE float getX1() const { return p[1].x; }
  FOG_INLINE float getY1() const { return p[1].y; }
  FOG_INLINE float getX2() const { return p[2].x; }
  FOG_INLINE float getY2() const { return p[2].y; }
  FOG_INLINE float getX3() const { return p[3].x; }
  FOG_INLINE float getY3() const { return p[3].y; }

  FOG_INLINE void setX0(float v) { p[0].x = v; }
  FOG_INLINE void setY0(float v) { p[0].y = v; }
  FOG_INLINE void setX1(float v) { p[1].x = v; }
  FOG_INLINE void setY1(float v) { p[1].y = v; }
  FOG_INLINE void setX2(float v) { p[2].x = v; }
  FOG_INLINE void setY2(float v) { p[2].y = v; }
  FOG_INLINE void setX3(float v) { p[3].x = v; }
  FOG_INLINE void setY3(float v) { p[3].y = v; }

  FOG_INLINE float getLength() const
  {
    float length;
    _g2d.cubiccurvef.getLength(p, &length);
    return length;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    p[0].reset();
    p[1].reset();
    p[2].reset();
    p[3].reset();
  }

  // --------------------------------------------------------------------------
  // [Evaluate]
  // --------------------------------------------------------------------------

  FOG_INLINE void evaluate(const PointF* pt, double t)
  {
    // TODO.
  }

  // --------------------------------------------------------------------------
  // [Inflection Points]
  // --------------------------------------------------------------------------

  FOG_INLINE int getInflectionPoints(float* t) const
  {
    return _g2d.cubiccurvef.getInflectionPoints(p, t);
  }

  // --------------------------------------------------------------------------
  // [Simplify]
  // --------------------------------------------------------------------------

  FOG_INLINE int simplifyForProcessing(PointF* pts, float flatness) const
  {
    return _g2d.cubiccurvef.simplifyForProcessing(p, pts, flatness);
  }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxF& dst) const
  {
    return _g2d.cubiccurvef.getBoundingBox(p, &dst);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst) const
  {
    err_t err = _g2d.cubiccurvef.getBoundingBox(p, reinterpret_cast<BoxF*>(&dst));
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [MidPoint]
  // --------------------------------------------------------------------------

  FOG_INLINE PointF getMidPoint() const
  {
    PointF result(UNINITIALIZED);
    getMidPoint(this->p, &result);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointF& pt)
  {
    p[0] += pt;
    p[1] += pt;
    p[2] += pt;
    p[3] += pt;
  }

  // --------------------------------------------------------------------------
  // [From]
  // --------------------------------------------------------------------------

  //! @brief Covert a quad bezier curve into the cubic.
  //!
  //! @verbatim
  //! cubic[0] = quad[0]
  //! cubic[1] = quad[0] * (1/3) + quad[1] * (2/3)
  //! cubic[2] = quad[2] * (1/3) + quad[1] * (2/3)
  //! cubic[3] = quad[2]
  //! @endverbatim
  void fromQuad(const PointF* quad)
  {
    float q1x_23 = quad[1].x * (float)(2.0 / 3.0);
    float q1y_23 = quad[1].y * (float)(2.0 / 3.0);

    p[0].x = quad[0].x;
    p[0].y = quad[0].y;
    p[1].x = quad[0].x * (float)(1.0 / 3.0) + q1x_23;
    p[1].y = quad[0].y * (float)(1.0 / 3.0) + q1y_23;
    p[2].x = quad[2].x * (float)(1.0 / 3.0) + q1x_23;
    p[2].y = quad[2].y * (float)(1.0 / 3.0) + q1y_23;
    p[3].x = quad[2].x;
    p[3].y = quad[2].y;
  }

  // --------------------------------------------------------------------------
  // [Approximate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t flatten(PathF& dst, uint8_t initialCommand, float flatness) const
  {
    return _g2d.cubiccurvef.flatten(p, dst, initialCommand, flatness);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE CBezierF& operator=(const CBezierF& other)
  {
    p[0] = other.p[0];
    p[1] = other.p[1];
    p[2] = other.p[2];
    p[3] = other.p[3];
    return *this;
  }

  FOG_INLINE bool operator==(const CBezierF& other) const
  {
    return p[0] == other.p[0] &&
           p[1] == other.p[1] &&
           p[2] == other.p[2] &&
           p[3] == other.p[3] ;
  }

  FOG_INLINE bool operator!=(const CBezierF& other) const
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t getBoundingBox(const CBezierF* self, BoxF* dst)
  {
    return _g2d.cubiccurvef.getBoundingBox(self->p, dst);
  }

  static FOG_INLINE err_t getBoundingBox(const PointF* self, BoxF* dst)
  {
    return _g2d.cubiccurvef.getBoundingBox(self, dst);
  }

  static FOG_INLINE err_t getSplineBBox(const PointF* self, sysuint_t length, BoxF* dst)
  {
    return _g2d.cubiccurvef.getSplineBBox(self, length, dst);
  }

  static FOG_INLINE void getMidPoint(const PointF* self, PointF* dst)
  {
    dst->x = (float)(1.0 / 8.0) * (self[0].x + self[3].x + 3.0f * (self[1].x + self[2].x));
    dst->y = (float)(1.0 / 8.0) * (self[0].y + self[3].y + 3.0f * (self[1].y + self[2].y));
  }

  static FOG_INLINE void splitHalf(const PointF* self, PointF* left, PointF* rght)
  {
    PointF p01 = Math2d::half(self[0], self[1]);
    PointF p12 = Math2d::half(self[1], self[2]);
    PointF p23 = Math2d::half(self[2], self[3]);

    left[0] = self[0];
    left[1] = p01;

    rght[2] = p23;
    rght[3] = self[3];

    left[2] = Math2d::half(p01, p12);
    rght[1] = Math2d::half(p12, p23);

    left[3] = Math2d::half(left[2], rght[1]);
    rght[0] = left[3];
  }

  static FOG_INLINE void splitHalf(const CBezierF* self, CBezierF* left, CBezierF* rght)
  {
    splitHalf(self->p, left->p, rght->p);
  }

  static FOG_INLINE void splitAt(const PointF* self, PointF* left, PointF* rght, float t)
  {
    float inv_t = (1.0f - t);

    PointF p01 = Math2d::lerp(self[0], self[1], t, inv_t);
    PointF p12 = Math2d::lerp(self[1], self[2], t, inv_t);
    PointF p23 = Math2d::lerp(self[2], self[3], t, inv_t);

    left[0] = self[0];
    left[1] = p01;

    rght[2] = p23;
    rght[3] = self[3];

    left[2] = Math2d::lerp(p01, p12, t, inv_t);
    rght[1] = Math2d::lerp(p12, p23, t, inv_t);

    left[3] = Math2d::lerp(left[2], rght[1], t, inv_t);
    rght[0] = left[3];
  }

  static FOG_INLINE void splitAt(const CBezierF* self, CBezierF* left, CBezierF* rght, float t)
  {
    splitAt(self->p, left->p, rght->p, t);
  }

  static FOG_INLINE void leftAt(const PointF* self, PointF* left, float t)
  {
    float inv_t = (1.0f - t);

    PointF p01 = Math2d::lerp(self[0], self[1], t, inv_t);
    PointF p12 = Math2d::lerp(self[1], self[2], t, inv_t);
    PointF p23 = Math2d::lerp(self[2], self[3], t, inv_t);

    left[0] = self[0];
    left[1] = p01;
    left[2] = Math2d::lerp(p01, p12, t, inv_t);
    left[3] = Math2d::lerp(left[2], Math2d::lerp(p12, p23, t, inv_t), t, inv_t);
  }

  static FOG_INLINE void leftAt(const CBezierF* self, CBezierF* left, float t)
  {
    leftAt(self->p, left->p, t);
  }

  static FOG_INLINE void rightAt(const PointF* self, PointF* rght, float t)
  {
    float inv_t = (1.0f - t);

    PointF p01 = Math2d::lerp(self[0], self[1], t, inv_t);
    PointF p12 = Math2d::lerp(self[1], self[2], t, inv_t);
    PointF p23 = Math2d::lerp(self[2], self[3], t, inv_t);

    rght[3] = self[3];
    rght[2] = p23;
    rght[1] = Math2d::lerp(p12, p23, t, inv_t);
    rght[0] = Math2d::lerp(Math2d::lerp(p01, p12, t, inv_t), rght[1], t, inv_t);
  }

  static FOG_INLINE void rightAt(const CBezierF* self, CBezierF* rght, float t)
  {
    rightAt(self->p, rght->p, t);
  }

  static FOG_INLINE err_t flatten(const CBezierF* self, PathF& dst, uint8_t initialCommand, float flatness)
  {
    return _g2d.cubiccurvef.flatten(self->p, dst, initialCommand, flatness);
  }

  static FOG_INLINE err_t flatten(const PointF* self, PathF& dst, uint8_t initialCommand, float flatness)
  {
    return _g2d.cubiccurvef.flatten(self, dst, initialCommand, flatness);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF p[4];
};

// ============================================================================
// [Fog::CBezierD]
// ============================================================================

struct FOG_NO_EXPORT CBezierD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE CBezierD() {}
  FOG_INLINE CBezierD(_Uninitialized) {}

  FOG_INLINE CBezierD(const PointD& p0, const PointD& p1, const PointD& p2, const PointD& p3)
  {
    p[0] = p0;
    p[1] = p1;
    p[2] = p2;
    p[3] = p3;
  }

  FOG_INLINE CBezierD(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3)
  {
    p[0].set(x0, y0);
    p[1].set(x1, y1);
    p[2].set(x2, y2);
    p[3].set(x3, y3);
  }

  FOG_INLINE CBezierD(const PointD* pts)
  {
    p[0] = pts[0];
    p[1] = pts[1];
    p[2] = pts[2];
    p[3] = pts[3];
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getX0() const { return p[0].x; }
  FOG_INLINE double getY0() const { return p[0].y; }
  FOG_INLINE double getX1() const { return p[1].x; }
  FOG_INLINE double getY1() const { return p[1].y; }
  FOG_INLINE double getX2() const { return p[2].x; }
  FOG_INLINE double getY2() const { return p[2].y; }
  FOG_INLINE double getX3() const { return p[3].x; }
  FOG_INLINE double getY3() const { return p[3].y; }

  FOG_INLINE void setX0(double v) { p[0].x = v; }
  FOG_INLINE void setY0(double v) { p[0].y = v; }
  FOG_INLINE void setX1(double v) { p[1].x = v; }
  FOG_INLINE void setY1(double v) { p[1].y = v; }
  FOG_INLINE void setX2(double v) { p[2].x = v; }
  FOG_INLINE void setY2(double v) { p[2].y = v; }
  FOG_INLINE void setX3(double v) { p[3].x = v; }
  FOG_INLINE void setY3(double v) { p[3].y = v; }

  FOG_INLINE double getLength() const
  {
    double length;
    _g2d.cubiccurved.getLength(p, &length);
    return length;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    p[0].reset();
    p[1].reset();
    p[2].reset();
    p[3].reset();
  }

  // --------------------------------------------------------------------------
  // [Evaluate]
  // --------------------------------------------------------------------------

  FOG_INLINE void evaluate(const PointD* pt, double t)
  {
    // TODO.
  }

  // --------------------------------------------------------------------------
  // [Inflection Points]
  // --------------------------------------------------------------------------

  FOG_INLINE int getInflectionPoints(double* t) const
  {
    return _g2d.cubiccurved.getInflectionPoints(p, t);
  }

  // --------------------------------------------------------------------------
  // [Simplify]
  // --------------------------------------------------------------------------

  FOG_INLINE int simplifyForProcessing(PointD* pts, double flatness) const
  {
    return _g2d.cubiccurved.simplifyForProcessing(p, pts, flatness);
  }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxD& dst) const
  {
    return _g2d.cubiccurved.getBoundingBox(p, &dst);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst) const
  {
    err_t err = _g2d.cubiccurved.getBoundingBox(p, reinterpret_cast<BoxD*>(&dst));
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [MidPoint]
  // --------------------------------------------------------------------------

  FOG_INLINE PointD getMidPoint() const
  {
    PointD result(UNINITIALIZED);
    getMidPoint(this->p, &result);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void translate(const PointD& pt)
  {
    p[0] += pt;
    p[1] += pt;
    p[2] += pt;
    p[3] += pt;
  }

  // --------------------------------------------------------------------------
  // [From]
  // --------------------------------------------------------------------------

  //! @brief Covert a quad bezier curve into the cubic.
  //!
  //! @verbatim
  //! cubic[0] = quad[0]
  //! cubic[1] = quad[0] * (1/3) + quad[1] * (2/3)
  //! cubic[2] = quad[2] * (1/3) + quad[1] * (2/3)
  //! cubic[3] = quad[2]
  //! @endverbatim
  void fromQuad(const PointD* quad)
  {
    double q1x_23 = quad[1].x * (2.0 / 3.0);
    double q1y_23 = quad[1].y * (2.0 / 3.0);

    p[0].x = quad[0].x;
    p[0].y = quad[0].y;
    p[1].x = quad[0].x * (1.0 / 3.0) + q1x_23;
    p[1].y = quad[0].y * (1.0 / 3.0) + q1y_23;
    p[2].x = quad[2].x * (1.0 / 3.0) + q1x_23;
    p[2].y = quad[2].y * (1.0 / 3.0) + q1y_23;
    p[3].x = quad[2].x;
    p[3].y = quad[2].y;
  }

  // --------------------------------------------------------------------------
  // [Approximate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t flatten(PathD& dst, uint8_t initialCommand, double flatness) const
  {
    return _g2d.cubiccurved.flatten(p, dst, initialCommand, flatness);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE CBezierD& operator=(const CBezierD& other)
  {
    p[0] = other.p[0];
    p[1] = other.p[1];
    p[2] = other.p[2];
    p[3] = other.p[3];
    return *this;
  }

  FOG_INLINE bool operator==(const CBezierD& other) const
  {
    return p[0] == other.p[0] &&
           p[1] == other.p[1] &&
           p[2] == other.p[2] &&
           p[3] == other.p[3] ;
  }

  FOG_INLINE bool operator!=(const CBezierD& other) const
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t getBoundingBox(const CBezierD* self, BoxD* dst)
  {
    return _g2d.cubiccurved.getBoundingBox(self->p, dst);
  }

  static FOG_INLINE err_t getBoundingBox(const PointD* self, BoxD* dst)
  {
    return _g2d.cubiccurved.getBoundingBox(self, dst);
  }

  static FOG_INLINE err_t getSplineBBox(const PointD* self, sysuint_t length, BoxD* dst)
  {
    return _g2d.cubiccurved.getSplineBBox(self, length, dst);
  }

  static FOG_INLINE void getMidPoint(const PointD* self, PointD* dst)
  {
    dst->x = (1.0 / 8.0) * (self[0].x + self[3].x + 3.0 * (self[1].x + self[2].x));
    dst->y = (1.0 / 8.0) * (self[0].y + self[3].y + 3.0 * (self[1].y + self[2].y));
  }

  static FOG_INLINE void splitHalf(const PointD* self, PointD* left, PointD* rght)
  {
    PointD p01 = Math2d::half(self[0], self[1]);
    PointD p12 = Math2d::half(self[1], self[2]);
    PointD p23 = Math2d::half(self[2], self[3]);

    left[0] = self[0];
    left[1] = p01;

    rght[2] = p23;
    rght[3] = self[3];

    left[2] = Math2d::half(p01, p12);
    rght[1] = Math2d::half(p12, p23);

    left[3] = Math2d::half(left[2], rght[1]);
    rght[0] = left[3];
  }

  static FOG_INLINE void splitHalf(const CBezierD* self, CBezierD* left, CBezierD* rght)
  {
    splitHalf(self->p, left->p, rght->p);
  }

  static FOG_INLINE void splitAt(const PointD* self, PointD* left, PointD* rght, double t)
  {
    double inv_t = (1.0 - t);

    PointD p01 = Math2d::lerp(self[0], self[1], t, inv_t);
    PointD p12 = Math2d::lerp(self[1], self[2], t, inv_t);
    PointD p23 = Math2d::lerp(self[2], self[3], t, inv_t);

    left[0] = self[0];
    left[1] = p01;

    rght[2] = p23;
    rght[3] = self[3];

    left[2] = Math2d::lerp(p01, p12, t, inv_t);
    rght[1] = Math2d::lerp(p12, p23, t, inv_t);

    left[3] = Math2d::lerp(left[2], rght[1], t, inv_t);
    rght[0] = left[3];
  }

  static FOG_INLINE void splitAt(const CBezierD* self, CBezierD* left, CBezierD* rght, double t)
  {
    splitAt(self->p, left->p, rght->p, t);
  }

  static FOG_INLINE void leftAt(const PointD* self, PointD* left, double t)
  {
    double inv_t = (1.0f - t);

    PointD p01 = Math2d::lerp(self[0], self[1], t, inv_t);
    PointD p12 = Math2d::lerp(self[1], self[2], t, inv_t);
    PointD p23 = Math2d::lerp(self[2], self[3], t, inv_t);

    left[0] = self[0];
    left[1] = p01;
    left[2] = Math2d::lerp(p01, p12, t, inv_t);
    left[3] = Math2d::lerp(left[2], Math2d::lerp(p12, p23, t, inv_t), t, inv_t);
  }

  static FOG_INLINE void leftAt(const CBezierD* self, CBezierD* left, double t)
  {
    leftAt(self->p, left->p, t);
  }

  static FOG_INLINE void rightAt(const PointD* self, PointD* rght, double t)
  {
    double inv_t = (1.0f - t);

    PointD p01 = Math2d::lerp(self[0], self[1], t, inv_t);
    PointD p12 = Math2d::lerp(self[1], self[2], t, inv_t);
    PointD p23 = Math2d::lerp(self[2], self[3], t, inv_t);

    rght[3] = self[3];
    rght[2] = p23;
    rght[1] = Math2d::lerp(p12, p23, t, inv_t);
    rght[0] = Math2d::lerp(Math2d::lerp(p01, p12, t, inv_t), rght[1], t, inv_t);
  }

  static FOG_INLINE void rightAt(const CBezierD* self, CBezierD* rght, double t)
  {
    rightAt(self->p, rght->p, t);
  }

  static FOG_INLINE err_t flatten(const CBezierD* self, PathD& dst, uint8_t initialCommand, double flatness)
  {
    return _g2d.cubiccurved.flatten(self->p, dst, initialCommand, flatness);
  }

  static FOG_INLINE err_t flatten(const PointD* self, PathD& dst, uint8_t initialCommand, double flatness)
  {
    return _g2d.cubiccurved.flatten(self, dst, initialCommand, flatness);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD p[4];
};

// ============================================================================
// [Fog::CBezierT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(CBezier)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::CBezierF, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::CBezierD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::CBezierF, Math::feqv((const float *)&a, (const float *)&b, 8))
FOG_FUZZY_DECLARE(Fog::CBezierD, Math::feqv((const double*)&a, (const double*)&b, 8))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_CUBICCURVE_H
