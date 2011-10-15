// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Function.h>
#include <Fog/Core/Math/Integrate.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/Solve.h>
#include <Fog/Core/Tools/Algorithm.h>
#include <Fog/G2d/Geometry/CBezier.h>
#include <Fog/G2d/Geometry/Internals_p.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

// ============================================================================
// Based on the Graphics-Gems and several papers related to curve approximation,
// curve-arc length calculation, etc...
//
// Please read these articles/papers (search for them):
// - Fast, precise flattening of cubic Bezier path and offset curves
// - Computing the Arc Length of Quadratic/Cubic Bezier Curves
// ============================================================================

// ============================================================================
// [Fog::CBezier - GetBoundingBox]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL CBezierT_getBoundingBox(const NumT_(Point)* self, NumT_(Box)* dst)
{
  // Init pMin/pMax - self[0].
  NumT_(Point) pMin = self[0];
  NumT_(Point) pMax = self[0];

  // Merge end point - self[3].
  if (self[3].x < pMin.x) pMin.x = self[3].x; else if (self[3].x > pMax.x) pMax.x = self[3].x;
  if (self[3].y < pMin.y) pMin.y = self[3].y; else if (self[3].y > pMax.y) pMax.y = self[3].y;

  // X extrema.
  NumT a = NumT(3.0) * (-self[0].x + NumT(3.0) * (self[1].x - self[2].x) + self[3].x);
  NumT b = NumT(6.0) * ( self[0].x - NumT(2.0) *  self[1].x + self[2].x             );
  NumT c = NumT(3.0) * (-self[0].x +                self[1].x                         );

  for (int i = 0;;)
  {
    // Catch the A and B near zero.
    if (Math::isFuzzyZero(a))
    {
      // A~=0 && B~=0.
      if (!Math::isFuzzyZero(b))
      {
        // Simple case (t = -c / b).
        NumT t0 = -c / b;
        NumT d;

        _FOG_CUBIC_MERGE(NumT, t0, self, pMin, pMax);
      }
    }
    else
    {
      // Calculate roots (t = b^2 - 4ac).
      NumT t = b * b - NumT(4.0) * a * c;
      if (t > NumT(0.0))
      {
        NumT s = Math::sqrt(t);
        NumT q = -NumT(0.5) * (b + ((b < NumT(0.0)) ? -s : s));

        NumT t0 = q / a;
        NumT t1 = c / q;

        NumT d;

        _FOG_CUBIC_MERGE(NumT, t0, self, pMin, pMax);
        _FOG_CUBIC_MERGE(NumT, t1, self, pMin, pMax);
      }
    }

    if (++i == 2) break;

    // Y extrema.
    a = NumT(3.0) * (-self[0].y + NumT(3.0) * (self[1].y - self[2].y) + self[3].y);
    b = NumT(6.0) * ( self[0].y - NumT(2.0) *  self[1].y + self[2].y             );
    c = NumT(3.0) * (-self[0].y +              self[1].y                         );
  }

  dst->setBox(pMin.x, pMin.y, pMax.x, pMax.y);
  return ERR_OK;
}

// ============================================================================
// [Fog::CBezier - GetSplineBBox]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL CBezierT_getSplineBBox(const NumT_(Point)* self, size_t length, NumT_(Box)* dst)
{
  size_t i;

  if (length < 4) return ERR_RT_INVALID_ARGUMENT;
  FOG_ASSERT((length - 1) % 3 == 0);

  // Init pMin/pMax - self[0].
  NumT_(Point) pMin = self[0];
  NumT_(Point) pMax = self[0];

  // Merge start/end points.
  for (i = 3; i < length; i += 3)
  {
    const NumT_(Point)* curve = &self[i];

    if (curve[0].x < pMin.x) pMin.x = curve[0].x; else if (curve[0].x > pMax.x) pMax.x = curve[0].x;
    if (curve[0].y < pMin.y) pMin.y = curve[0].y; else if (curve[0].y > pMax.y) pMax.y = curve[0].y;
  }

  // Merge extremas.
  for (i = 0; i < length; i += 3)
  {
    const NumT_(Point)* curve = &self[i];

    if (curve[1].x < pMin.x || curve[1].y < pMin.y || curve[1].x > pMax.x || curve[1].y > pMax.y ||
        curve[2].x < pMin.x || curve[2].y < pMin.y || curve[2].x > pMax.x || curve[2].y > pMax.y)
    {
      NumT a = NumT(3.0) * (-curve[0].x + NumT(3.0) * (curve[1].x - curve[2].x) + curve[3].x);
      NumT b = NumT(6.0) * ( curve[0].x - NumT(2.0) *  curve[1].x + curve[2].x              );
      NumT c = NumT(3.0) * (-curve[0].x +              curve[1].x                           );

      for (int j = 0;;)
      {
        // Catch the A and B near zero.
        if (Math::isFuzzyZero(a))
        {
          // A~=0 && B~=0.
          if (!Math::isFuzzyZero(b))
          {
            // Simple case (t = -c / b).
            NumT t0 = -c / b;
            NumT d;

            _FOG_CUBIC_MERGE(NumT, t0, curve, pMin, pMax);
          }
        }
        else
        {
          // Calculate roots (t = b^2 - 4ac).
          NumT t = b * b - NumT(4.0) * a * c;
          if (t > NumT(0.0))
          {
            NumT s = Math::sqrt(t);
            NumT q = -NumT(0.5) * (b + ((b < NumT(0.0)) ? -s : s));

            NumT t0 = q / a;
            NumT t1 = c / q;

            NumT d;

            _FOG_CUBIC_MERGE(NumT, t0, curve, pMin, pMax);
            _FOG_CUBIC_MERGE(NumT, t1, curve, pMin, pMax);
          }
        }

        if (++j == 2) break;

        // Y extrema.
        a = NumT(3.0) * (-curve[0].y + NumT(3.0) * (curve[1].y - curve[2].y) + curve[3].y);
        b = NumT(6.0) * ( curve[0].y - NumT(2.0) *  curve[1].y + curve[2].y              );
        c = NumT(3.0) * (-curve[0].y +              curve[1].y                           );
      }
    }
  }

  dst->setBox(pMin.x, pMin.y, pMax.x, pMax.y);
  return ERR_OK;
}
// ============================================================================
// [Fog::CBezier - GetLength]
// ============================================================================

struct CubicDt : public FunctionD
{
  FOG_INLINE CubicDt() {}
  virtual ~CubicDt() {}

  virtual err_t evaluate(double* dst, double t) const
  {
    double tInv = 1.0 - t;

    double x = pd[0].x * Math::pow2(tInv) + pd[1].x * t * tInv + pd[2].x * Math::pow2(t);
    double y = pd[0].y * Math::pow2(tInv) + pd[1].y * t * tInv + pd[2].y * Math::pow2(t);

    *dst = Math::hypot(x, y);
    return ERR_OK;
  }

  PointD pd[3];
};

template<typename NumT>
static void FOG_CDECL CBezierT_getLength(const NumT_(Point)* self, NumT* length)
{
  CubicDt dfunc;

  dfunc.pd[0].x = 3.0 * double(self[1].x - self[0].x);
  dfunc.pd[0].y = 3.0 * double(self[1].y - self[0].y);
  dfunc.pd[1].x = 6.0 * double(self[2].x - self[1].x);
  dfunc.pd[1].y = 6.0 * double(self[2].y - self[1].y);
  dfunc.pd[2].x = 3.0 * double(self[3].x - self[2].x);
  dfunc.pd[2].y = 3.0 * double(self[3].y - self[2].y);

  double dst;
  Math::integrate(&dst, dfunc, IntervalD(0.0, 1.0), MATH_INTEGRATION_METHOD_GAUSS, 4);
  *length = NumT(dst);
}

// ============================================================================
// [Fog::CBezier - GetInflectionPoints]
// ============================================================================

// Cubic Parameters
// ----------------
//
// A =   -p0 + 3*p1 - 3*p2 + p3 == -p0 + 3*(p1 -   p2) + p3
// B =  3*p0 - 6*p1 + 3*p2      ==       3*(p0 - 2*p2  + p3)
// C = -3*p0 + 3*p1             ==       3*(p1 -   p0)
// D =    p0                    ==  p0
//
// Evaluation at t
// ---------------
//
// Value = A * t^3 + b * t^2 + C * t + D

template<typename NumT>
static int FOG_CDECL CBezierT_getInflectionPoints(const NumT_(Point)* self, NumT* t)
{
  // Extract the parameters.
  NumT ax, ay, bx, by, cx, cy, dx, dy;
  _FOG_CUBIC_EXTRACT_PARAMETERS(NumT, ax, ay, bx, by, cx, cy, dx, dy, self);

  // Solve the quadratic function.
  NumT q[3];

  q[0] = NumT(6.0) * (ay * bx - ax * by);
  q[1] = NumT(6.0) * (ay * cx - ax * cy);
  q[2] = NumT(2.0) * (by * cx - bx * cy);

  int count = Math::solve(t, q, MATH_SOLVE_QUADRATIC, NumT_(Interval)(NumT(0.0), NumT(1.0)));

  if (count == 0)
    return 0;

  if (Math::isFuzzyZero(t[0]))
  {
    if (--count == 0)
      return 0;
    t[0] = t[1];
  }

  if (Math::isFuzzyOne(t[count - 1]))
    count--;

  return count;
}

// ============================================================================
// [Fog::CBezier - SimplifyForProcessing]
// ============================================================================

// Based on paper Precise Flattening of Cubic Bezier Segments by T.Hain et al.
//
// This function is designed to pre-process one Bezier curve into B-Spline of
// curves without inflection points. This function can be also used as a first
// step to accomplish requirements for flattening using parabolic method or
// stroking.

template<typename NumT>
static int FOG_CDECL CBezierT_simplifyForProcessing(const NumT_(Point)* self, NumT_(Point)* pts)
{
  // Extract the parameters.
  NumT ax, ay, bx, by, cx, cy, dx, dy;
  _FOG_CUBIC_EXTRACT_PARAMETERS(NumT, ax, ay, bx, by, cx, cy, dx, dy, self);

  NumT q[3];
  NumT t[3];

  q[0] = (ay * bx - ax * by); // Qa
  q[1] = (ay * cx - ax * cy); // Qb
  q[2] = (by * cx - bx * cy); // Qc

  // tCusp = -0.5 * ((ay*cx - ax*cy) / (ay*bx - ax*by)).
  // tCusp = -0.5 * (Qb / Qa)
  NumT tCusp = -NumT(0.5) * (q[1] / q[0]);

  // Solve the quadratic function, needed to find the inflection points.
  q[0] *= NumT(6.0);
  q[1] *= NumT(6.0);
  q[2] *= NumT(2.0);

  int tCount = Math::solve(t, q, MATH_SOLVE_QUADRATIC, NumT_(Interval)(NumT(0.0), NumT(1.0)));
  if (tCusp > NumT(0.0) && tCusp < NumT(1.0))
  {
    t[tCount++] = tCusp;
    Algorithm::isort_t<NumT>(t, tCount);
  }

  switch (tCount)
  {
    case 0:
    {
      pts[0] = self[0];
      pts[1] = self[1];
      pts[2] = self[2];
      pts[3] = self[3];
      return 1;
    }

    case 1:
    case 2:
    case 3:
    {
      int segmentCount = 2;

      NumT t_ = t[0];
      NumT cut = t_;

      NumI_(CBezier)::splitAt(self, pts, pts + 3, t_);
      pts += 3;

      for (int tIndex = 1; tIndex < tCount; tIndex++)
      {
        t_ = t[tIndex];
        if (Math::isFuzzyEq(t_, cut))
          continue;

        NumI_(CBezier)::splitAt(pts, pts, pts + 3, (t_ - cut) / (NumT(1.0) - cut));
        cut = t_;

        pts += 3;
        segmentCount++;
      }

      return segmentCount;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::CBezier - Flatten]
// ============================================================================

#define CUBIC_CURVE_FLATTEN_RECURSION_LIMIT 32
#define CUBIC_CURVE_VERTEX_INITIAL_SIZE 256

#define ADD_VERTEX(_X_, _Y_) \
  FOG_MACRO_BEGIN \
    FOG_ASSERT((size_t)(curVertex - dst->_d->vertices) < dst->_d->capacity); \
    \
    curVertex->set(_X_, _Y_); \
    curVertex++; \
  FOG_MACRO_END

template<typename NumT>
static err_t FOG_CDECL CBezierT_flatten(
  const NumT_(Point)* self,
  NumT_(Path)* dst,
  uint8_t initialCommand,
  NumT flatness)
{
  NumT distanceToleranceSquare = Math::pow2(flatness);
  NumT x0 = self[0].x;
  NumT y0 = self[0].y;
  NumT x1 = self[1].x;
  NumT y1 = self[1].y;
  NumT x2 = self[2].x;
  NumT y2 = self[2].y;
  NumT x3 = self[3].x;
  NumT y3 = self[3].y;

  size_t initialLength = dst->_d->length;
  size_t level = 0;

  NumT_(Point)* curVertex;
  NumT_(Point)* endVertex;

  NumT_(Point) _stack[CUBIC_CURVE_FLATTEN_RECURSION_LIMIT * 4];
  NumT_(Point)* stack = _stack;

_Realloc:
  {
    size_t pos = dst->_add(CUBIC_CURVE_VERTEX_INITIAL_SIZE);

    if (pos == INVALID_INDEX)
    {
      // Purge dst length to it's initial state.
      if (dst->_d->length != initialLength) dst->_d->length = initialLength;
      return ERR_RT_OUT_OF_MEMORY;
    }

    curVertex = dst->_d->vertices + pos;
    endVertex = curVertex + CUBIC_CURVE_VERTEX_INITIAL_SIZE - 2;
  }

  for (;;)
  {
    // Realloc if needed.
    if (curVertex >= endVertex)
    {
      dst->_d->length = (size_t)(curVertex - dst->_d->vertices);
      goto _Realloc;
    }

    // Calculate all the mid-points of the line segments.
    NumT x01   = (x0 + x1) * NumT(0.5);
    NumT y01   = (y0 + y1) * NumT(0.5);
    NumT x12   = (x1 + x2) * NumT(0.5);
    NumT y12   = (y1 + y2) * NumT(0.5);
    NumT x23   = (x2 + x3) * NumT(0.5);
    NumT y23   = (y2 + y3) * NumT(0.5);
    NumT x012  = (x01 + x12) * NumT(0.5);
    NumT y012  = (y01 + y12) * NumT(0.5);
    NumT x123  = (x12 + x23) * NumT(0.5);
    NumT y123  = (y12 + y23) * NumT(0.5);
    NumT x0123 = (x012 + x123) * NumT(0.5);
    NumT y0123 = (y012 + y123) * NumT(0.5);

    // Try to approximate the full cubic curve by a single straight line.
    NumT dx = x3 - x0;
    NumT dy = y3 - y0;

    NumT d2 = Math::abs(((x1 - x3) * dy - (y1 - y3) * dx));
    NumT d3 = Math::abs(((x2 - x3) * dy - (y2 - y3) * dx));
    NumT da1, da2, k;

    switch ((int(d2 > MathConstant<NumT>::getCollinearityEpsilon()) << 1) +
             int(d3 > MathConstant<NumT>::getCollinearityEpsilon()))
    {
      // All collinear OR p0 == p3.
      case 0:
        k = dx*dx + dy*dy;
        if (k == 0)
        {
          d2 = Math2d::distSquare(x0, y0, x1, y1);
          d3 = Math2d::distSquare(x3, y3, x2, y2);
        }
        else
        {
          k   = NumT(1.0) / k;
          da1 = x1 - x0;
          da2 = y1 - y0;
          d2  = k * (da1 * dx + da2 * dy);
          da1 = x2 - x0;
          da2 = y2 - y0;
          d3  = k * (da1 * dx + da2 * dy);

          if (d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1)
          {
            // Simple collinear case, 0---1---2---3.
            // We can leave just two endpoints.
            goto _Ret;
          }

          if (d2 <= 0)
            d2 = Math2d::distSquare(x1, y1, x0, y0);
          else if (d2 >= 1)
            d2 = Math2d::distSquare(x1, y1, x3, y3);
          else
            d2 = Math2d::distSquare(x1, y1, x0 + d2*dx, y0 + d2*dy);

          if (d3 <= 0)
            d3 = Math2d::distSquare(x2, y2, x0, y0);
          else if (d3 >= 1)
            d3 = Math2d::distSquare(x2, y2, x3, y3);
          else
            d3 = Math2d::distSquare(x2, y2, x0 + d3*dx, y0 + d3*dy);
        }

        if (d2 > d3)
        {
          if (d2 < distanceToleranceSquare)
          {
            ADD_VERTEX(x1, y1);
            goto _Ret;
          }
        }
        else
        {
          if (d3 < distanceToleranceSquare)
          {
            ADD_VERTEX(x2, y2);
            goto _Ret;
          }
        }
        break;

    // p0, p1, p3 are collinear, p2 is significant.
    case 1:
      if (d3 * d3 <= distanceToleranceSquare * (dx * dx + dy * dy))
      {
        ADD_VERTEX(x12, y12);
        goto _Ret;
      }
      break;

    // p0, p2, p3 are collinear, p1 is significant.
    case 2:
      if (d2 * d2 <= distanceToleranceSquare * (dx * dx + dy * dy))
      {
        ADD_VERTEX(x12, y12);
        goto _Ret;
      }
      break;

    // Regular case.
    case 3:
      if ((d2 + d3) * (d2 + d3) <= distanceToleranceSquare * (dx * dx + dy * dy))
      {
        ADD_VERTEX(x12, y12);
        goto _Ret;
      }
      break;
    }

    // Continue subdivision.
    //
    // Original antigrain code:
    //   recursive_bezier(x0, y0, x01, y01, x012, y012, x0123, y0123, level + 1);
    //   recursive_bezier(x0123, y0123, x123, y123, x23, y23, x3, y3, level + 1);
    //
    // First recursive subdivision will be set into x0, y0, x1, y1, x2, y2,
    // second subdivision will be added into stack.
    if (level < CUBIC_CURVE_FLATTEN_RECURSION_LIMIT)
    {
      stack[0].set(x0123, y0123);
      stack[1].set(x123 , y123 );
      stack[2].set(x23  , y23  );
      stack[3].set(x3   , y3   );

      stack += 4;
      level++;

      x1 = x01;
      y1 = y01;
      x2 = x012;
      y2 = y012;
      x3 = x0123;
      y3 = y0123;

      continue;
    }
    else if (!Math::isFinite(x0123))
    {
      goto _InvalidNumber;
    }

_Ret:
    if (level == 0) break;

    stack -= 4;
    level--;

    x0 = stack[0].x;
    y0 = stack[0].y;
    x1 = stack[1].x;
    y1 = stack[1].y;
    x2 = stack[2].x;
    y2 = stack[2].y;
    x3 = stack[3].x;
    y3 = stack[3].y;
  }

  // Add end point.
  ADD_VERTEX(x3, y3);

  {
    // Update dst length.
    size_t length = (size_t)(curVertex - dst->_d->vertices);
    dst->_d->length = length;

    // Make sure we are not out of bounds.
    FOG_ASSERT(dst->_d->capacity >= length);

    // Fill initial and MoveTo commands.
    uint8_t* commands = dst->_d->commands + initialLength;
    size_t i = length - initialLength;

    if (i)
    {
      *commands++ = initialCommand;
      i--;
    }

    while (i)
    {
      *commands++ = PATH_CMD_LINE_TO;
      i--;
    }
  }

  return ERR_OK;

_InvalidNumber:
  // Purge dst length to its initial state.
  if (dst->_d->length != initialLength) dst->_d->length = initialLength;
  return ERR_GEOMETRY_INVALID;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void CBezier_init(void)
{
  fog_api.cbezierf_getBoundingBox = CBezierT_getBoundingBox<float>;
  fog_api.cbezierd_getBoundingBox = CBezierT_getBoundingBox<double>;

  fog_api.cbezierf_getSplineBBox = CBezierT_getSplineBBox<float>;
  fog_api.cbezierd_getSplineBBox = CBezierT_getSplineBBox<double>;

  fog_api.cbezierf_getLength = CBezierT_getLength<float>;
  fog_api.cbezierd_getLength = CBezierT_getLength<double>;

  fog_api.cbezierf_getInflectionPoints = CBezierT_getInflectionPoints<float>;
  fog_api.cbezierd_getInflectionPoints = CBezierT_getInflectionPoints<double>;

  fog_api.cbezierf_simplifyForProcessing = CBezierT_simplifyForProcessing<float>;
  fog_api.cbezierd_simplifyForProcessing = CBezierT_simplifyForProcessing<double>;

  fog_api.cbezierf_flatten = CBezierT_flatten<float>;
  fog_api.cbezierd_flatten = CBezierT_flatten<double>;
}

} // Fog namespace
