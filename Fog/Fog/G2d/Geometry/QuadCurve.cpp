// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Internals_p.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/QuadCurve.h>
#include <Fog/G2d/Global/Init_p.h>

namespace Fog {

// ============================================================================
// Based on the Graphics-Gems and several papers related to curve approximation,
// curve-arc length calculation, etc...
//
// Please read these articles/papers (search for them):
// - Fast, precise flattening of cubic Bézier path and offset curves
// - Computing the Arc Length of Quadratic/Cubic Bezier Curves
// ============================================================================

// ============================================================================
// [Fog::QuadCurve - GetExtrema]
// ============================================================================

template<typename Number>
static void FOG_CDECL _G2d_QuadCurveT_getExtrema(const typename PointT<Number>::T* self, typename BoxT<Number>::T* dstBox)
{
  // Init pMin/pMax - self[0].
  typename PointT<Number>::T pMin = self[0];
  typename PointT<Number>::T pMax = self[0];

  // Merge end point - self[2].
  if (self[2].x < pMin.x) pMin.x = self[2].x; else if (self[2].x > pMax.x) pMax.x = self[2].x;
  if (self[2].y < pMin.y) pMin.y = self[2].y; else if (self[2].y > pMax.y) pMax.y = self[2].y;

  // X/Y extrema.
  Number t0 = (self[0].x - self[1].x) / (self[0].x - Number(2.0) * self[1].x + self[2].x);
  Number t1 = (self[0].y - self[1].y) / (self[0].y - Number(2.0) * self[1].y + self[2].y);

  Number a, b, c;
  _FOG_QUAD_MERGE(Number, t0);
  _FOG_QUAD_MERGE(Number, t1);

  dstBox->set(pMin.x, pMin.y, pMax.x, pMax.y);
}

// ============================================================================
// [Fog::QuadCurve - GetLength]
// ============================================================================

// Implementation based on segfaultlabs.com 'Quadratic Bezier Curve Length':
//   http://segfaultlabs.com/docs/quadratic-bezier-curve-length

static double _G2d_QuadCurveT_getLength_private(
  double p0x, double p0y,
  double p1x, double p1y,
  double p2x, double p2y)
{
  double ax = p0x + p2x - p1x * 2.0;
  double ay = p0y + p2y - p1y * 2.0;

  double bx = (p1x - p0x) * 2.0;
  double by = (p1y - p0y) * 2.0;

  double a = (ax * ax + ay * ay) * 4.0;
  double b = (ax * bx + ay * by) * 4.0;
  double c = (bx * bx + by * by);

  double Sabc = Math::sqrt(a + b + c);
  double Sa   = Math::sqrt(a);
  double Sc   = Math::sqrt(c);

  double bDivSa = b / Sa;
  double logPart = Math::log((2.0 * (Sabc + Sa) + bDivSa) / (2.0 * Sc + bDivSa));

  return (2.0 * Sa * (Sabc * (2.0 * a + b) - b * Sc) + (4.0 * a * c - b * b) * logPart)
         //----------------------------------------------------------------------------
                                       / (8.0 * Sa * a);
}

template<typename Number>
static void FOG_CDECL _G2d_QuadCurveT_getLength(const typename PointT<Number>::T* self, Number* length)
{
  // Always using 'double' to get maximum precision.
  *length = (Number)_G2d_QuadCurveT_getLength_private(
    self[0].x, self[0].y,
    self[1].x, self[1].y,
    self[2].x, self[2].y);
}

// ============================================================================
// [Fog::QuadCurve - Approximate]
// ============================================================================

#define QUAD_CURVE_APPROXIMATE_RECURSION_LIMIT 32
#define QUAD_CURVE_VERTEX_INITIAL_SIZE 256

#define ADD_VERTEX(_x, _y) \
  do { \
    curVertex->set(_x, _y); \
    curVertex++; \
  } while(0)

template<typename Number>
static err_t FOG_CDECL _G2d_QuadCurveT_flatten(
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

  sysuint_t initialLength = dst._d->length;
  sysuint_t level = 0;

  typename PointT<Number>::T* curVertex;
  typename PointT<Number>::T* endVertex;

  typename PointT<Number>::T _stack[QUAD_CURVE_APPROXIMATE_RECURSION_LIMIT * 3];
  typename PointT<Number>::T* stack = _stack;

_Realloc:
  {
    sysuint_t pos = dst._add(QUAD_CURVE_VERTEX_INITIAL_SIZE);
    if (pos == INVALID_INDEX)
    {
      // Purge dst length to its initial state.
      if (dst._d->length != initialLength) dst._d->length = initialLength;
      return ERR_RT_OUT_OF_MEMORY;
    }

    curVertex = dst._d->vertices + pos;
    endVertex = curVertex + QUAD_CURVE_VERTEX_INITIAL_SIZE - 1;
  }

  for (;;)
  {
    // Realloc if needed, but update length if need to do.
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
    Number x012  = (x01 + x12) * Number(0.5);
    Number y012  = (y01 + y12) * Number(0.5);

    Number dx = x2 - x0;
    Number dy = y2 - y0;
    Number d = Math::abs(((x1 - x2) * dy - (y1 - y2) * dx));
    Number da;

    if (d > Math2dConst<Number>::getCollinearityEpsilon())
    {
      // Regular case.
      if (d * d <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        ADD_VERTEX(x012, y012);
        goto _Ret;
      }
    }
    else
    {
      // Collinear case.
      da = dx*dx + dy*dy;
      if (da == 0)
      {
        d = Math2d::distSquare(x0, y0, x1, y1);
      }
      else
      {
        d = ((x1 - x0)*dx + (y1 - y0)*dy) / da;

        if (d > 0 && d < 1)
        {
          // Simple collinear case, 0---1---2.
          // We can leave just two endpoints.
          goto _Ret;
        }

        if (d <= 0)
          d = Math2d::distSquare(x1, y1, x0, y0);
        else if (d >= 1)
          d = Math2d::distSquare(x1, y1, x2, y2);
        else
          d = Math2d::distSquare(x1, y1, x0 + d*dx, y0 + d*dy);
      }
      if (d < distanceToleranceSquare)
      {
        ADD_VERTEX(x1, y1);
        goto _Ret;
      }
    }

    // Continue subdivision.
    //
    // Original code from antigrain was:
    //   recursive_bezier(x0, y0, x01, y01, x012, y012, level + 1);
    //   recursive_bezier(x012, y012, x12, y12, x2, y2, level + 1);
    //
    // First recursive subdivision will be set into x0, y0, x1, y1, x2, y2,
    // second subdivision will be added into stack.

    if (level < QUAD_CURVE_APPROXIMATE_RECURSION_LIMIT)
    {
      stack[0].set(x012, y012);
      stack[1].set(x12 , y12 );
      stack[2].set(x2  , y2  );

      stack += 3;
      level++;

      x1 = x01;
      y1 = y01;
      x2 = x012;
      y2 = y012;

      continue;
    }
    else
    {
      if (Math::isNaN(x012)) goto _InvalidNumber;
    }

_Ret:
    if (level == 0) break;

    stack -= 3;
    level--;

    x0 = stack[0].x;
    y0 = stack[0].y;
    x1 = stack[1].x;
    y1 = stack[1].y;
    x2 = stack[2].x;
    y2 = stack[2].y;
  }

  // Add end point.
  ADD_VERTEX(x2, y2);

  {
    // Update dst length.
    sysuint_t length = (sysuint_t)(curVertex - dst._d->vertices);
    dst._d->length = length;

    // Make sure we are not out of bounds.
    FOG_ASSERT(dst._d->capacity >= length);

    // Fill initial and LINE_TO commands.
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

FOG_NO_EXPORT void _g2d_quadcurve_init(void)
{
  _g2d.quadcurvef.getExtrema  = _G2d_QuadCurveT_getExtrema<float>;
  _g2d.quadcurvef.getLength   = _G2d_QuadCurveT_getLength<float>;
  _g2d.quadcurvef.flatten     = _G2d_QuadCurveT_flatten<float>;

  _g2d.quadcurved.getExtrema  = _G2d_QuadCurveT_getExtrema<double>;
  _g2d.quadcurved.getLength   = _G2d_QuadCurveT_getLength<double>;
  _g2d.quadcurved.flatten     = _G2d_QuadCurveT_flatten<double>;
}

} // Fog namespace
