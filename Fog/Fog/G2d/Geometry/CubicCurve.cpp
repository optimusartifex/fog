// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Algorithms.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/Solve.h>
#include <Fog/G2d/Geometry/CubicCurve.h>
#include <Fog/G2d/Geometry/Internals_p.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Global/Init_p.h>

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
// [Fog::CubicCurve - GetExtrema]
// ============================================================================

template<typename Number>
static void FOG_CDECL _G2d_CubicCurveT_getExtrema(const typename PointT<Number>::T* self, typename BoxT<Number>::T* dstBox)
{
  // Init pMin/pMax - self[0].
  typename PointT<Number>::T pMin = self[0];
  typename PointT<Number>::T pMax = self[0];

  // Merge end point - self[3].
  if (self[3].x < pMin.x) pMin.x = self[3].x; else if (self[3].x > pMax.x) pMax.x = self[3].x;
  if (self[3].y < pMin.y) pMin.y = self[3].y; else if (self[3].y > pMax.y) pMax.y = self[3].y;

  // X extrema.
  Number a = Number(3.0) * (-self[0].x + Number(3.0) * (self[1].x - self[2].x) + self[3].x);
  Number b = Number(6.0) * ( self[0].x - Number(2.0) *  self[1].x + self[2].x             );
  Number c = Number(3.0) * (-self[0].x +                self[1].x                         );

  for (int i = 0;;)
  {
    // Catch the A and B near zero.
    if (Math::isFuzzyZero(a))
    {
      // A~=0 && B~=0.
      if (!Math::isFuzzyZero(b))
      {
        // Simple case (t = -c / b).
        Number t0 = -c / b;
        Number d;

        _FOG_CUBIC_MERGE(Number, t0);
      }
    }
    else
    {
      // Calculate roots (t = b^2 - 4ac).
      Number t = b * b - Number(4.0) * a * c;
      if (t > Number(0.0))
      {
        Number s = Math::sqrt(t);
        Number q = -Number(0.5) * (b + ((b < Number(0.0)) ? -s : s));

        Number t0 = q / a;
        Number t1 = c / q;

        Number d;

        _FOG_CUBIC_MERGE(Number, t0);
        _FOG_CUBIC_MERGE(Number, t1);
      }
    }

    if (++i == 2) break;

    // Y extrema.
    a = Number(3.0) * (-self[0].y + Number(3.0) * (self[1].y - self[2].y) + self[3].y);
    b = Number(6.0) * ( self[0].y - Number(2.0) *  self[1].y + self[2].y             );
    c = Number(3.0) * (-self[0].y +                self[1].y                         );
  }

  dstBox->set(pMin.x, pMin.y, pMax.x, pMax.y);
}

// ============================================================================
// [Fog::CubicCurve - GetLength]
// ============================================================================

// Add polyline length if close enough.
static void _G2d_CubicCurveX_addIfClose(const PointD* self, double *length, double error) 
{
  PointD left[4], right[4];
  double arclen   = Math2d::distEuclidean(self[0], self[1]) + 
                    Math2d::distEuclidean(self[1], self[2]) +
                    Math2d::distEuclidean(self[2], self[3]);
  double chordlen = Math2d::distEuclidean(self[0], self[3]);

  if ((arclen - chordlen) > error)
  {
    CubicCurveD::splitHalf(self, left, right);
    _G2d_CubicCurveX_addIfClose(left, length, error);
    _G2d_CubicCurveX_addIfClose(right, length, error);
  }
  else
  {
    *length += arclen;
  }
}

static void FOG_CDECL _G2d_CubicCurveF_getLength(const PointF* self, float* length)
{
  // Using 'double' for maximum precision.
  PointD pd[4];

  pd[0] = self[0];
  pd[1] = self[1];
  pd[2] = self[2];
  pd[3] = self[3];

  double len = 0.0;
  _G2d_CubicCurveX_addIfClose(pd, &len, 0.01);
  *length = (float)len;
}

static void FOG_CDECL _G2d_CubicCurveD_getLength(const PointD* self, double* length)
{
  *length = 0.0;
  _G2d_CubicCurveX_addIfClose(self, length, 0.01);
}

// ============================================================================
// [Fog::CubicCurve - GetInflectionPoints]
// ============================================================================

// Cubic Parameters
// ----------------
//
// A =   -p0 + 3*p1 � 3*p2 + p3 == -p0 + 3*(p1 -   p2) + p3
// B =  3*p0 � 6*p1 + 3*p2      ==       3*(p0 - 2*p2  + p3)
// C = -3*p0 + 3*p1             ==       3*(p1 -   p0)
// D =    p0                    ==  p0
//
// Evaluation at t
// ---------------
//
// Value = A * t^3 + b * t^2 + C * t + D

template<typename Number>
static int FOG_CDECL _G2d_CubicCurveT_getInflectionPoints(const typename PointT<Number>::T* self, Number* t)
{
  // Extract the parameters.
  Number ax, ay, bx, by, cx, cy, dx, dy;
  _FOG_CUBIC_EXTRACT_PARAMETERS(Number, ax, ay, bx, by, cx, cy, dx, dy, self);

  // Solve the quadratic function.
  Number q[3];

  q[0] = Number(6.0) * (ay * bx - ax * by);
  q[1] = Number(6.0) * (ay * cx - ax * cy);
  q[2] = Number(2.0) * (by * cx - bx * cy);

  return Math::solveQuadraticFunction(t, q);
}

// ============================================================================
// [Fog::CubicCurve - SimplifyForProcessing]
// ============================================================================

// Based on paper Precise Flattening of Cubic Bezier Segments by T.Hain et al.
//
// This function is designed to pre-process one Bezier curve into B-Spline of
// curves without inflection points. This function can be also used as a first
// step to accomplish requirements for flattening using parabolic method or
// stroking.

template<typename Number>
static int FOG_CDECL _G2d_CubicCurveT_simplifyForProcessing(const typename PointT<Number>::T* self,typename  PointT<Number>::T* pts, Number flatness)
{
  // Extract the parameters.
  Number ax, ay, bx, by, cx, cy, dx, dy;
  _FOG_CUBIC_EXTRACT_PARAMETERS(Number, ax, ay, bx, by, cx, cy, dx, dy, self);

  Number q[3];
  Number t[3];

  q[0] = (ay * bx - ax * by); // Qa
  q[1] = (ay * cx - ax * cy); // Qb
  q[2] = (by * cx - bx * cy); // Qc

  // tCusp = -0.5 * ((ay*cx - ax*cy) / (ay*bx - ax*by)).
  // tCusp = -0.5 * (Qb / Qa)
  Number tCusp = -Number(0.5) * (q[1] / q[0]);

  // Solve the quadratic function, needed to find the inflection points.
  q[0] *= Number(6.0);
  q[1] *= Number(6.0);
  q[2] *= Number(2.0);

  switch (Math::solveQuadraticFunction(t, q))
  {
    // Two inflection points. Subdivide at t0, tCusp, and t1.
    case 2:
      {
        t[2] = tCusp;
        Algorithms::isort_t<Number>(t, 3);

        // Now split the curve into b-spline and return the count of curves.
        const typename PointT<Number>::T* bPtr = self;
        typename PointT<Number>::T bTmp[4];

        int count = 0;
        Number cut = Number(0.0);

        for (int i = 0; i < 3; i++)
        {
          if (t[i] <= Number(0.0) || t[i] >= Number(1.0)) continue;
          CubicCurveT<Number>::T::splitAt(bPtr, pts, bTmp, cut == Number(0.0) ? t[i] : (t[i] - cut) / (Number(1.0) - cut));
          bPtr = pts;

          cut = t[i];
          pts += 3;
          count++;
        }

        if (count == 0) goto _NoInflection;

        pts[0] = bTmp[1];
        pts[1] = bTmp[2];
        pts[2] = bTmp[3];
        return ++count;
      }

    // One inflection point, subdivide at t[0].
    case 1:
_OneInflection:
      if (t[0] <= Number(0.0) || t[0] >= Number(1.0)) goto _NoInflection;

      CubicCurveT<Number>::T::splitAt(self, pts, pts + 3, t[0]);
      return 2;

    // No inflection points. The given Bezier curve doesn't need to be simplified.
    case 0:
_NoInflection:
      pts[0] = self[0];
      pts[1] = self[1];
      pts[2] = self[2];
      pts[3] = self[3];
      return 1;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::CubicCurve - Approximate]
// ============================================================================

#define CUBIC_CURVE_APPROXIMATE_RECURSION_LIMIT 32
#define CUBIC_CURVE_VERTEX_INITIAL_SIZE 256

#define ADD_VERTEX(_X_, _Y_) \
  FOG_MACRO_BEGIN \
    curVertex->set(_X_, _Y_); \
    curVertex++; \
  FOG_MACRO_END

template<typename Number>
static err_t FOG_CDECL _G2d_CubicCurveT_flatten(
  const typename PointT<Number>::T* self,
  typename PathT<Number>::T& dst,
  uint8_t initialCommand,
  Number flatness)
{
  Number distanceToleranceSquare = Math::pow2(flatness);
  Number x0 = self[0].x;
  Number y0 = self[0].y;
  Number x1 = self[1].x;
  Number y1 = self[1].y;
  Number x2 = self[2].x;
  Number y2 = self[2].y;
  Number x3 = self[3].x;
  Number y3 = self[3].y;

  sysuint_t initialLength = dst._d->length;
  sysuint_t level = 0;

  typename PointT<Number>::T* curVertex;
  typename PointT<Number>::T* endVertex;

  typename PointT<Number>::T _stack[CUBIC_CURVE_APPROXIMATE_RECURSION_LIMIT * 4];
  typename PointT<Number>::T* stack = _stack;

_Realloc:
  {
    sysuint_t pos = dst._add(CUBIC_CURVE_VERTEX_INITIAL_SIZE);

    if (pos == INVALID_INDEX)
    {
      // Purge dst length to it's initial state.
      if (dst._d->length != initialLength) dst._d->length = initialLength;
      return ERR_RT_OUT_OF_MEMORY;
    }

    curVertex = dst._d->vertices + pos;
    endVertex = curVertex + CUBIC_CURVE_VERTEX_INITIAL_SIZE - 2;
  }

  for (;;)
  {
    // Realloc if needed.
    if (curVertex >= endVertex)
    {
      dst._d->length = (sysuint_t)(curVertex - dst._d->vertices);
      goto _Realloc;
    }

    // Calculate all the mid-points of the line segments.
    Number x01   = (x0 + x1) * Number(0.5);
    Number y01   = (y0 + y1) * Number(0.5);
    Number x12   = (x1 + x2) * Number(0.5);
    Number y12   = (y1 + y2) * Number(0.5);
    Number x23   = (x2 + x3) * Number(0.5);
    Number y23   = (y2 + y3) * Number(0.5);
    Number x012  = (x01 + x12) * Number(0.5);
    Number y012  = (y01 + y12) * Number(0.5);
    Number x123  = (x12 + x23) * Number(0.5);
    Number y123  = (y12 + y23) * Number(0.5);
    Number x0123 = (x012 + x123) * Number(0.5);
    Number y0123 = (y012 + y123) * Number(0.5);

    // Try to approximate the full cubic curve by a single straight line.
    Number dx = x3 - x0;
    Number dy = y3 - y0;

    Number d2 = Math::abs(((x1 - x3) * dy - (y1 - y3) * dx));
    Number d3 = Math::abs(((x2 - x3) * dy - (y2 - y3) * dx));
    Number da1, da2, k;

    switch ((int(d2 > Math2dConst<Number>::getCollinearityEpsilon()) << 1) +
             int(d3 > Math2dConst<Number>::getCollinearityEpsilon()))
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
          k   = Number(1.0) / k;
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
    if (level < CUBIC_CURVE_APPROXIMATE_RECURSION_LIMIT)
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
    else
    {
      if (Math::isNaN(x0123)) goto _InvalidNumber;
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
    sysuint_t length = (sysuint_t)(curVertex - dst._d->vertices);
    dst._d->length = length;

    // Make sure we are not out of bounds.
    FOG_ASSERT(dst._d->capacity >= length);

    // Fill initial and MoveTo commands.
    uint8_t* commands = dst._d->commands + initialLength;
    sysuint_t i = length - initialLength;

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
  if (dst._d->length != initialLength) dst._d->length = initialLength;
  return ERR_PATH_INVALID;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_cubiccurve_init(void)
{
  _g2d.cubiccurvef.getExtrema            = _G2d_CubicCurveT_getExtrema<float>;
  _g2d.cubiccurvef.getLength             = _G2d_CubicCurveF_getLength;
  _g2d.cubiccurvef.getInflectionPoints   = _G2d_CubicCurveT_getInflectionPoints<float>;
  _g2d.cubiccurvef.simplifyForProcessing = _G2d_CubicCurveT_simplifyForProcessing<float>;
  _g2d.cubiccurvef.flatten               = _G2d_CubicCurveT_flatten<float>;

  _g2d.cubiccurved.getExtrema            = _G2d_CubicCurveT_getExtrema<double>;
  _g2d.cubiccurved.getLength             = _G2d_CubicCurveD_getLength;
  _g2d.cubiccurved.getInflectionPoints   = _G2d_CubicCurveT_getInflectionPoints<double>;
  _g2d.cubiccurved.simplifyForProcessing = _G2d_CubicCurveT_simplifyForProcessing<double>;
  _g2d.cubiccurved.flatten               = _G2d_CubicCurveT_flatten<double>;
}

} // Fog namespace
