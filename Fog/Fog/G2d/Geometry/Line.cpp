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
#include <Fog/G2d/Geometry/Line.h>

namespace Fog {

// ============================================================================
// [Fog::Line - Intersect]
// ============================================================================

template<typename NumT>
static uint32_t FOG_CDECL LineT_intersect(NumT_(Point)* dst,
  const NumT_(Point)* lineA,
  const NumT_(Point)* lineB)
{
  double ax = double(lineA[1].x) - double(lineA[0].x);
  double ay = double(lineA[1].y) - double(lineA[0].y);
  double bx = double(lineB[1].x) - double(lineB[0].x);
  double by = double(lineB[1].y) - double(lineB[0].y);

  double d = ay * bx - ax * by;
  if (Math::isFuzzyZero(d) || !Math::isFinite(d))
    return LINE_INTERSECTION_NONE;

  double ox = double(lineA[0].x - lineB[0].x);
  double oy = double(lineA[0].y - lineB[0].y);

  double t = (by * ox - bx * oy) / d;

  dst->x = NumT(double(lineA[0].x) + ax * t);
  dst->y = NumT(double(lineA[0].y) + ay * t);
  if (t < 0.0 && t > 1.0)
    return LINE_INTERSECTION_UNBOUNDED;

  t = (ax * oy - ay * ox) / d;
  if (t < 0.0 && t > 1.0)
    return LINE_INTERSECTION_UNBOUNDED;
  else
    return LINE_INTERSECTION_BOUNDED;
}

// ============================================================================
// [Fog::Line - GetAngle]
// ============================================================================

template<typename NumT>
static NumT FOG_CDECL LineT_polyAngle(const NumT_(Point)* pts)
{
  // Inspired by stackoverflow QA:
  //   http://stackoverflow.com/questions/3486172/angle-between-3-points
  //
  // Input arguments are:
  //   pts[0] == a
  //   pts[1] == b
  //   pts[2] == c

  // Create ab/cb vectors.
  NumT_(Point) ab(pts[1].x - pts[0].x, pts[1].y - pts[0].y);
  NumT_(Point) cb(pts[1].x - pts[2].x, pts[1].y - pts[2].y);

  // Dot product.
  NumT dotp = ab.x * cb.x + ab.y * cb.y;

  // Length square.
  NumT sqab = ab.x * ab.x + ab.y * ab.y;
  NumT sqcb = cb.x * cb.x + cb.y * cb.y;

  // Square of cosine of the needed angle.
  NumT sqcos = (dotp * dotp) / (sqab * sqcb);
  // cos(a*2) == cos(a)^2 * 2 - 1.
  NumT cos2 = NumT(2) * sqcos - NumT(1);

  // It's a good idea to check explicitly if cos2 is within [-1 .. 1] range
  NumT angle = cos2 <= NumT(-1) ? NumT(MATH_PI)
                                : (cos2 >= NumT(1)) ? NumT(0) : Math::acos(cos2);
  angle *= NumT(0.5);

  // Now revolve the ambiguities.
  //
  // 1. If dot product of two vectors is negative - the angle is definitely
  //    above 90 degrees. Still we have no information regarding the sign of
  //    the angle.
  //
  //    NOTE: This ambiguity is the consequence of our method: calculating
  //    the cosine of the double angle. This allows us to get rid of calling
  //    sqrt.
  if (dotp < NumT(0))
    angle = NumT(MATH_PI) - angle;

  // 2. Determine the sign using Determinant of [ab, cb] vectors.
  NumT det = ab.x * cb.y - ab.y * cb.x;
  if (det < NumT(0))
    angle = -angle;

  return angle;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Line_init(void)
{
  fog_api.linef_intersect = LineT_intersect<float>;
  fog_api.linef_polyAngle = LineT_polyAngle<float>;

  fog_api.lined_intersect = LineT_intersect<double>;
  fog_api.lined_polyAngle = LineT_polyAngle<double>;
}

} // Fog namespace
