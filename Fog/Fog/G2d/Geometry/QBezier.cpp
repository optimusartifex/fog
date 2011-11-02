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
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/Solve.h>
#include <Fog/G2d/Geometry/Internals_p.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/QBezier.h>

namespace Fog {

// Please read these articles/papers (search for them):
//
//   - Graphics-Gems
//   - Fast, precise flattening of cubic Bézier path and offset curves
//   - Computing the Arc Length of Quadratic/Cubic Bezier Curves

// ============================================================================
// [Fog::QBezier - GetBoundingBox]
// ============================================================================

// We know that there is an extrema only when the control point is outside of
// the bounding box created by the two end points.
template<typename NumT>
static err_t FOG_CDECL QBezierT_getBoundingBox(const NumT_(Point)* self, NumT_(Box)* dst)
{
  // Init pMin/pMax - self[0].
  NumT_(Point) pMin = self[0];
  NumT_(Point) pMax = self[0];

  // Merge end point - self[2].
  if (self[2].x < pMin.x) pMin.x = self[2].x; else if (self[2].x > pMax.x) pMax.x = self[2].x;
  if (self[2].y < pMin.y) pMin.y = self[2].y; else if (self[2].y > pMax.y) pMax.y = self[2].y;

  // Merge X extrema.
  if (self[1].x < pMin.x || self[1].x > pMax.x)
  {
    NumT t = (self[0].x - self[1].x) / (self[0].x - NumT(2.0) * self[1].x + self[2].x);
    NumT a, b, c;
    _FOG_QUAD_COEFF(NumT, t, a, b, c);

    NumT x = a * self[0].x + b * self[1].x + c * self[2].x;
    if (x < pMin.x)
      pMin.x = x;
    else if (x > pMax.x)
      pMax.x = x;
  }

  // Merge Y extrema.
  if (self[1].y < pMin.y || self[1].y > pMax.y)
  {
    NumT t = (self[0].y - self[1].y) / (self[0].y - NumT(2.0) * self[1].y + self[2].y);
    NumT a, b, c;
    _FOG_QUAD_COEFF(NumT, t, a, b, c);

    NumT y = a * self[0].y + b * self[1].y + c * self[2].y;
    if (y < pMin.y)
      pMin.y = y;
    else if (y > pMax.y)
      pMax.y = y;
  }

  dst->setBox(pMin.x, pMin.y, pMax.x, pMax.y);
  return ERR_OK;
}

// ============================================================================
// [Fog::QBezier - GetSplineBBox]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL QBezierT_getSplineBBox(const NumT_(Point)* self, size_t length, NumT_(Box)* dst)
{
  size_t i;

  if (length < 3) return ERR_RT_INVALID_ARGUMENT;
  FOG_ASSERT((length - 1) % 2 == 0);

  // Init pMin/pMax - self[0].
  NumT_(Point) pMin = self[0];
  NumT_(Point) pMax = self[0];

  // Merge start/end points.
  for (i = 2; i < length; i += 2)
  {
    const NumT_(Point)* curve = &self[i];

    if (curve[0].x < pMin.x) pMin.x = curve[0].x; else if (curve[0].x > pMax.x) pMax.x = curve[0].x;
    if (curve[0].y < pMin.y) pMin.y = curve[0].y; else if (curve[0].y > pMax.y) pMax.y = curve[0].y;
  }

  // Merge extremas.
  for (i = 0; i < length; i += 2)
  {
    const NumT_(Point)* curve = &self[i];

    // Merge X extrema.
    if (curve[1].x < pMin.x || curve[1].x > pMax.x)
    {
      NumT t = (curve[0].x - curve[1].x) / (curve[0].x - NumT(2.0) * curve[1].x + curve[2].x);
      NumT a, b, c;
      _FOG_QUAD_COEFF(NumT, t, a, b, c);

      NumT x = a * curve[0].x + b * curve[1].x + c * curve[2].x;
      if (x < pMin.x)
        pMin.x = x;
      else if (x > pMax.x)
        pMax.x = x;
    }

    // Merge Y extrema.
    if (curve[1].y < pMin.y || curve[1].y > pMax.y)
    {
      NumT t = (curve[0].y - curve[1].y) / (curve[0].y - NumT(2.0) * curve[1].y + curve[2].y);
      NumT a, b, c;
      _FOG_QUAD_COEFF(NumT, t, a, b, c);

      NumT y = a * curve[0].y + b * curve[1].y + c * curve[2].y;
      if (y < pMin.y)
        pMin.y = y;
      else if (y > pMax.y)
        pMax.y = y;
    }
  }

  dst->setBox(pMin.x, pMin.y, pMax.x, pMax.y);
  return ERR_OK;
}

// ============================================================================
// [Fog::QBezier - GetLength]
// ============================================================================

// Implementation based on segfaultlabs.com 'Quadratic Bezier Curve Length':
//   http://segfaultlabs.com/docs/quadratic-bezier-curve-length

static double QBezierT_getLength_private(
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

template<typename NumT>
static void FOG_CDECL QBezierT_getLength(const NumT_(Point)* self, NumT* length)
{
  // Always using 'double' to get maximum precision.
  *length = (NumT)QBezierT_getLength_private(
    self[0].x, self[0].y,
    self[1].x, self[1].y,
    self[2].x, self[2].y);
}

// ============================================================================
// [Fog::QBezier - GetClosestPoint]
// ============================================================================

template<typename NumT>
static NumT QBezierT_getClosestPoint(const NumT_(Point)* self, NumT_(Point)* dst, const NumT_(Point)* p)
{
  NumT_(Point) a;
  NumT_(Point) b;
  NumT_(Point) q;

  a.x = self[1].x - self[0].x;
  a.y = self[1].y - self[0].y;
  b.x = self[0].x - NumT(2.0) * self[1].x + self[2].x;
  b.y = self[0].y - NumT(2.0) * self[1].y + self[2].y;
  q.x = self[0].x - p->x;
  q.y = self[0].y - p->y;

  NumT func[4];
  func[0] = b.x * b.x + b.y * b.y;
  func[1] = NumT(3.0) * (a.x * b.x + a.y * b.y);
  func[2] = NumT(2.0) * (a.x * a.x + a.y * a.y) + b.x * q.x + b.y * q.y;
  func[3] = a.x * q.x + a.y * q.y;

  NumT t[3];
  uint count = Math::solvePolynomial(t, func, MATH_POLYNOMIAL_DEGREE_CUBIC, NumT_(Interval)(NumT(0), NumT(1)));

  NumT minimumT = NumT(0.0);
  NumT minimumDistance = Math::squaredDistance(self[0].x - p->x, self[0].y - p->y);
  *dst = self[0];

  NumT distance = Math::squaredDistance(self[2].x - p->x, self[2].y - p->y);
  if (distance < minimumDistance)
  {
    minimumT = NumT(1.0);
    minimumDistance = distance;
    *dst = self[2];
  }

  for (uint i = 0; i < count; i++)
  {
    reinterpret_cast<const NumT_(QBezier)*>(self)->evaluate(q, t[i]);
    distance = Math::squaredDistance(q.x - p->x, q.y - p->y);

    if (distance < minimumDistance)
    {
      minimumT = t[i];
      minimumDistance = distance;
      *dst = q;
    }
  }

  return minimumT;
}

// ============================================================================
// [Fog::QBezier - Flatten]
// ============================================================================

#define QUAD_CURVE_FLATTEN_RECURSION_LIMIT 32
#define QUAD_CURVE_VERTEX_INITIAL_SIZE 256

#define ADD_VERTEX(_x, _y) \
  do { \
    FOG_ASSERT((size_t)(curVertex - dst->_d->vertices) < dst->_d->capacity); \
    \
    curVertex->set(_x, _y); \
    curVertex++; \
  } while(0)

template<typename NumT>
static err_t FOG_CDECL QBezierT_flatten(
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

  size_t initialLength = dst->_d->length;
  size_t level = 0;

  NumT_(Point)* curVertex;
  NumT_(Point)* endVertex;

  NumT_(Point) _stack[QUAD_CURVE_FLATTEN_RECURSION_LIMIT * 3];
  NumT_(Point)* stack = _stack;

_Realloc:
  {
    size_t pos = dst->_add(QUAD_CURVE_VERTEX_INITIAL_SIZE);
    if (pos == INVALID_INDEX)
    {
      // Purge dst length to its initial state.
      if (dst->_d->length != initialLength) dst->_d->length = initialLength;
      return ERR_RT_OUT_OF_MEMORY;
    }

    curVertex = dst->_d->vertices + pos;
    endVertex = curVertex + QUAD_CURVE_VERTEX_INITIAL_SIZE - 1;
  }

  for (;;)
  {
    // Realloc if needed, but update length if need to do.
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
    NumT x012  = (x01 + x12) * NumT(0.5);
    NumT y012  = (y01 + y12) * NumT(0.5);

    NumT dx = x2 - x0;
    NumT dy = y2 - y0;
    NumT d = Math::abs(((x1 - x2) * dy - (y1 - y2) * dx));
    NumT da;

    if (d > MathConstant<NumT>::getCollinearityEpsilon())
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

    if (level < QUAD_CURVE_FLATTEN_RECURSION_LIMIT)
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
    size_t length = (size_t)(curVertex - dst->_d->vertices);
    dst->_d->length = length;

    // Make sure we are not out of bounds.
    FOG_ASSERT(dst->_d->capacity >= length);

    // Fill initial and LINE_TO commands.
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

FOG_NO_EXPORT void QBezier_init(void)
{
  fog_api.qbezierf_getBoundingBox = QBezierT_getBoundingBox<float>;
  fog_api.qbezierd_getBoundingBox = QBezierT_getBoundingBox<double>;

  fog_api.qbezierf_getSplineBBox = QBezierT_getSplineBBox<float>;
  fog_api.qbezierd_getSplineBBox = QBezierT_getSplineBBox<double>;

  fog_api.qbezierf_getLength = QBezierT_getLength<float>;
  fog_api.qbezierd_getLength = QBezierT_getLength<double>;

  fog_api.qbezierf_getClosestPoint = QBezierT_getClosestPoint<float>;
  fog_api.qbezierd_getClosestPoint = QBezierT_getClosestPoint<double>;

  fog_api.qbezierf_flatten = QBezierT_flatten<float>;
  fog_api.qbezierd_flatten = QBezierT_flatten<double>;
}

} // Fog namespace
