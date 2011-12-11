// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_CBEZIER_H
#define _FOG_G2D_GEOMETRY_CBEZIER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::CBezierF]
// ============================================================================

struct FOG_NO_EXPORT CBezierF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE CBezierF() {}

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

  explicit FOG_INLINE CBezierF(_Uninitialized) {}

  explicit FOG_INLINE CBezierF(const PointF* pts)
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

  // --------------------------------------------------------------------------
  // [GetLength]
  // --------------------------------------------------------------------------

  FOG_INLINE float getLength() const
  {
    float length;
    fog_api.cbezierf_getLength(p, &length);
    return length;
  }

  // --------------------------------------------------------------------------
  // [GetClosestPoint]
  // --------------------------------------------------------------------------

  FOG_INLINE float getClosestPoint(PointF& dst, const PointF& pt) const
  {
    return fog_api.cbezierf_getClosestPoint(p, &dst, &pt);
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

  FOG_INLINE void evaluate(PointF& pt, float t) const
  {
    evaluate(this, &pt, t);
  }

  // --------------------------------------------------------------------------
  // [Inflection Points]
  // --------------------------------------------------------------------------

  FOG_INLINE int getInflectionPoints(float* t) const
  {
    return fog_api.cbezierf_getInflectionPoints(p, t);
  }

  // --------------------------------------------------------------------------
  // [Simplify]
  // --------------------------------------------------------------------------

  FOG_INLINE int simplifyForProcessing(PointF* pts) const
  {
    return fog_api.cbezierf_simplifyForProcessing(p, pts);
  }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxF& dst) const
  {
    return fog_api.cbezierf_getBoundingBox(p, &dst);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst) const
  {
    err_t err = fog_api.cbezierf_getBoundingBox(p, reinterpret_cast<BoxF*>(&dst));
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

  //! @brief Translate all cubic points by @a pt.
  FOG_INLINE void translate(const PointF& pt)
  {
    p[0] += pt;
    p[1] += pt;
    p[2] += pt;
    p[3] += pt;
  }

  // --------------------------------------------------------------------------
  // [FromQuad / ToQuad]
  // --------------------------------------------------------------------------

  //! @brief Covert a quad Bezier curve into the cubic Bezier curve.
  //!
  //! Any quadratic Bezier curve can be converted into the cubic. The equation
  //! used for conversion is as follows:
  //!
  //! @verbatim
  //! c0 = q0
  //! c1 = q0 + 2/3 * (q1 - q0)
  //! c2 = q2 + 2/3 * (q1 - q2)
  //! c3 = q2
  //! @endverbatim
  FOG_INLINE void fromQuad(const PointF* quad)
  {
    float q1x_23 = quad[1].x * float(MATH_2_DIV_3);
    float q1y_23 = quad[1].y * float(MATH_2_DIV_3);

    p[0].x = quad[0].x;
    p[0].y = quad[0].y;
    p[1].x = quad[0].x * float(MATH_1_DIV_3) + q1x_23;
    p[1].y = quad[0].y * float(MATH_1_DIV_3) + q1y_23;
    p[2].x = quad[2].x * float(MATH_1_DIV_3) + q1x_23;
    p[2].y = quad[2].y * float(MATH_1_DIV_3) + q1y_23;
    p[3].x = quad[2].x;
    p[3].y = quad[2].y;
  }
  
  //! @brief Get whether the cubic Bezier curve was created from quadratic
  //! coordinates, and fill @a quadMiddlePoint by the computed quadratic
  //! control point.
  FOG_INLINE bool toQuad(PointF* quadMiddlePoint, float epsilon = MATH_EPSILON_F) const
  {
    double qx0 = double(1.5) * double(p[1].x) - double(0.5) * double(p[0].x);
    double qy0 = double(1.5) * double(p[1].y) - double(0.5) * double(p[0].y);

    double qx1 = double(1.5) * double(p[2].x) - double(0.5) * double(p[3].x);
    double qy1 = double(1.5) * double(p[2].y) - double(0.5) * double(p[3].y);

    quadMiddlePoint->set(float((qx0 + qx1) * 0.5),
                         float((qy0 + qy1) * 0.5));

    return Math::isFuzzyEq(qx0, qx1, double(epsilon)) &&
           Math::isFuzzyEq(qy0, qy1, double(epsilon));
  }

  // --------------------------------------------------------------------------
  // [Flatten]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t flatten(PathF& dst, uint8_t initialCommand, float flatness) const
  {
    return fog_api.cbezierf_flatten(p, &dst, initialCommand, flatness);
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

  FOG_INLINE bool operator==(const CBezierF& other) const { return  MemOps::eq_t<CBezierF>(this, &other); }
  FOG_INLINE bool operator!=(const CBezierF& other) const { return !MemOps::eq_t<CBezierF>(this, &other); }

  FOG_INLINE const PointF& operator[](size_t i) const
  {
    FOG_ASSERT_X(i < 4,
      "Fog::CBezierF::operator[] - Index out of range");
    
    return p[i];
  }
  
  FOG_INLINE PointF& operator[](size_t i)
  {
    FOG_ASSERT_X(i < 4,
                 "Fog::CBezierF::operator[] - Index out of range");
    
    return p[i];
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE void evaluate(const CBezierF* self, PointF* dst, float t)
  {
    evaluate(self->p, dst, t);
  }

  static FOG_INLINE void evaluate(const PointF* self, PointF* dst, float t)
  {
    float tInv = 1.0f - t;

    float ax = self[0].x * tInv + self[1].x * t;
    float ay = self[0].y * tInv + self[1].y * t;
    
    float bx = self[1].x * tInv + self[2].x * t;
    float by = self[1].y * tInv + self[2].y * t;
    
    float cx = self[2].x * tInv + self[3].x * t;
    float cy = self[2].y * tInv + self[3].y * t;

    dst->x = (ax * tInv + bx * t) * tInv + (bx * tInv + cx * t) * t;
    dst->y = (ay * tInv + by * t) * tInv + (by * tInv + cy * t) * t;
  }

  static FOG_INLINE err_t getBoundingBox(const CBezierF* self, BoxF* dst)
  {
    return fog_api.cbezierf_getBoundingBox(self->p, dst);
  }

  static FOG_INLINE err_t getBoundingBox(const PointF* self, BoxF* dst)
  {
    return fog_api.cbezierf_getBoundingBox(self, dst);
  }

  static FOG_INLINE err_t getSplineBBox(const PointF* self, size_t length, BoxF* dst)
  {
    return fog_api.cbezierf_getSplineBBox(self, length, dst);
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
    return fog_api.cbezierf_flatten(self->p, &dst, initialCommand, flatness);
  }

  static FOG_INLINE err_t flatten(const PointF* self, PathF& dst, uint8_t initialCommand, float flatness)
  {
    return fog_api.cbezierf_flatten(self, &dst, initialCommand, flatness);
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

  explicit FOG_INLINE CBezierD(_Uninitialized) {}

  explicit FOG_INLINE CBezierD(const PointD* pts)
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

  // --------------------------------------------------------------------------
  // [GetLength]
  // --------------------------------------------------------------------------

  FOG_INLINE double getLength() const
  {
    double length;
    fog_api.cbezierd_getLength(p, &length);
    return length;
  }

  // --------------------------------------------------------------------------
  // [GetClosestPoint]
  // --------------------------------------------------------------------------

  FOG_INLINE double getClosestPoint(PointD& dst, const PointD& pt) const
  {
    return fog_api.cbezierd_getClosestPoint(p, &dst, &pt);
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

  FOG_INLINE void evaluate(PointD& pt, double t) const
  {
    evaluate(this, &pt, t);
  }

  // --------------------------------------------------------------------------
  // [Inflection Points]
  // --------------------------------------------------------------------------

  FOG_INLINE int getInflectionPoints(double* t) const
  {
    return fog_api.cbezierd_getInflectionPoints(p, t);
  }

  // --------------------------------------------------------------------------
  // [Simplify]
  // --------------------------------------------------------------------------

  FOG_INLINE int simplifyForProcessing(PointD* pts) const
  {
    return fog_api.cbezierd_simplifyForProcessing(p, pts);
  }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxD& dst) const
  {
    return fog_api.cbezierd_getBoundingBox(p, &dst);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst) const
  {
    err_t err = fog_api.cbezierd_getBoundingBox(p, reinterpret_cast<BoxD*>(&dst));
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

  //! @brief Translate all cubic points by @a pt.
  FOG_INLINE void translate(const PointD& pt)
  {
    p[0] += pt;
    p[1] += pt;
    p[2] += pt;
    p[3] += pt;
  }

  // --------------------------------------------------------------------------
  // [FromQuad / ToQuad]
  // --------------------------------------------------------------------------

  //! @brief Covert a quad Bezier curve into the cubic Bezier curve.
  //!
  //! Any quadratic Bezier curve can be converted into the cubic. The equation
  //! used for conversion is as follows:
  //!
  //! @verbatim
  //! c0 = q0
  //! c1 = q0 + 2/3 * (q1 - q0)
  //! c2 = q2 + 2/3 * (q1 - q2)
  //! c3 = q2
  //! @endverbatim
  FOG_INLINE void fromQuad(const PointD* quad)
  {
    double q1x_23 = quad[1].x * MATH_2_DIV_3;
    double q1y_23 = quad[1].y * MATH_2_DIV_3;

    p[0].x = quad[0].x;
    p[0].y = quad[0].y;
    p[1].x = quad[0].x * MATH_1_DIV_3 + q1x_23;
    p[1].y = quad[0].y * MATH_1_DIV_3 + q1y_23;
    p[2].x = quad[2].x * MATH_1_DIV_3 + q1x_23;
    p[2].y = quad[2].y * MATH_1_DIV_3 + q1y_23;
    p[3].x = quad[2].x;
    p[3].y = quad[2].y;
  }

  //! @brief Get whether the cubic Bezier curve was created from quadratic
  //! coordinates, and fill @a quadMiddlePoint by the computed quadratic
  //! control point.
  FOG_INLINE bool toQuad(PointD* quadMiddlePoint, double epsilon = MATH_EPSILON_D) const
  {
    double qx0 = double(1.5) * p[1].x - double(0.5) * p[0].x;
    double qy0 = double(1.5) * p[1].y - double(0.5) * p[0].y;
    
    double qx1 = double(1.5) * p[2].x - double(0.5) * p[3].x;
    double qy1 = double(1.5) * p[2].y - double(0.5) * p[3].y;
    
    quadMiddlePoint->set(qx0, qy0);
    return Math::isFuzzyEq(qx0, qx1, epsilon) &&
           Math::isFuzzyEq(qy0, qy1, epsilon);
  }

  // --------------------------------------------------------------------------
  // [Flatten]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t flatten(PathD& dst, uint8_t initialCommand, double flatness) const
  {
    return fog_api.cbezierd_flatten(p, &dst, initialCommand, flatness);
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

  FOG_INLINE bool operator==(const CBezierD& other) const { return  MemOps::eq_t<CBezierD>(this, &other); }
  FOG_INLINE bool operator!=(const CBezierD& other) const { return !MemOps::eq_t<CBezierD>(this, &other); }

  FOG_INLINE const PointD& operator[](size_t i) const
  {
    FOG_ASSERT_X(i < 4,
                 "Fog::CBezierD::operator[] - Index out of range");
    
    return p[i];
  }
  
  FOG_INLINE PointD& operator[](size_t i)
  {
    FOG_ASSERT_X(i < 4,
                 "Fog::CBezierD::operator[] - Index out of range");
    
    return p[i];
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE void evaluate(const CBezierD* self, PointD* dst, double t)
  {
    evaluate(self->p, dst, t);
  }

  static FOG_INLINE void evaluate(const PointD* self, PointD* dst, double t)
  {
    double tInv = 1.0f - t;

    double ax = self[0].x * tInv + self[1].x * t;
    double ay = self[0].y * tInv + self[1].y * t;
    
    double bx = self[1].x * tInv + self[2].x * t;
    double by = self[1].y * tInv + self[2].y * t;
    
    double cx = self[2].x * tInv + self[3].x * t;
    double cy = self[2].y * tInv + self[3].y * t;

    dst->x = (ax * tInv + bx * t) * tInv + (bx * tInv + cx * t) * t;
    dst->y = (ay * tInv + by * t) * tInv + (by * tInv + cy * t) * t;
  }

  static FOG_INLINE err_t getBoundingBox(const CBezierD* self, BoxD* dst)
  {
    return fog_api.cbezierd_getBoundingBox(self->p, dst);
  }

  static FOG_INLINE err_t getBoundingBox(const PointD* self, BoxD* dst)
  {
    return fog_api.cbezierd_getBoundingBox(self, dst);
  }

  static FOG_INLINE err_t getSplineBBox(const PointD* self, size_t length, BoxD* dst)
  {
    return fog_api.cbezierd_getSplineBBox(self, length, dst);
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
    return fog_api.cbezierd_flatten(self->p, &dst, initialCommand, flatness);
  }

  static FOG_INLINE err_t flatten(const PointD* self, PathD& dst, uint8_t initialCommand, double flatness)
  {
    return fog_api.cbezierd_flatten(self, &dst, initialCommand, flatness);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD p[4];
};

// ============================================================================
// [Fog::CBezierT<>]
// ============================================================================

_FOG_NUM_T(CBezier)
_FOG_NUM_F(CBezier)
_FOG_NUM_D(CBezier)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::CBezierF, 8)
FOG_FUZZY_DECLARE_D_VEC(Fog::CBezierD, 8)

// [Guard]
#endif // _FOG_G2D_GEOMETRY_CBEZIER_H
