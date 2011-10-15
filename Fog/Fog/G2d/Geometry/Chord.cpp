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
#include <Fog/G2d/Geometry/Chord.h>

namespace Fog {

// ============================================================================
// [Fog::Chord - HitTest]
// ============================================================================

template<typename NumT>
static bool FOG_CDECL ChordT_hitTest(const NumT_(Chord)* self, const NumT_(Point)* pt)
{
  NumT cx = self->center.x;
  NumT cy = self->center.y;

  NumT rx = Math::abs(self->radius.x);
  NumT ry = Math::abs(self->radius.y);

  NumT x = pt->x - cx;
  NumT y = pt->y - cy;

  // No radius or very close to zero (can't hit-test in such case).
  if (Math::isFuzzyZero(rx) || Math::isFuzzyZero(ry)) return false;

  // Transform an ellipse to a circle, transforming also the hit-tested point.
  if (rx > ry)
  {
    y = y * rx / ry;
  }
  else if (rx < ry)
  {
    x = x * ry / rx;
    rx = ry;
  }

  // Hit-test the ellipse.
  NumT ptDist = Math::pow2(x) + Math::pow2(y);
  NumT maxDist = Math::pow2(rx);

  if (ptDist > maxDist) return false;

  // Hit-test the rest.
  NumT_(Point) p0, p1;

  NumT start = self->start;
  NumT sweep = self->sweep;

  const NumT _2pi = NumT(MATH_TWO_PI);
  // If sweep is larger than a 2pi then hit-test is always positive.
  if (sweep < -_2pi || sweep > _2pi) return true;

  Math::sincos(start        , &p0.y, &p0.x); p0.x *= rx; p0.y *= rx;
  Math::sincos(start + sweep, &p1.y, &p1.x); p1.x *= rx; p1.y *= rx;

  NumT dx = p1.x - p0.x;
  NumT dy = p1.y - p0.y;

  int onRight = sweep < NumT(0.0);

  if (Math::isFuzzyZero(dy))
  {
    if (Math::abs(sweep) < NumT(MATH_HALF_PI)) return false;
    return onRight ? y <= p0.y : y >= p0.y;
  }

  NumT lx = p0.x + (y - p0.y) * dx / dy;
  onRight ^= (dy > NumT(0.0));

  if (onRight)
    return x >= lx;
  else
    return x < lx;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Chord_init(void)
{
  fog_api.chordf_hitTest = ChordT_hitTest<float>;
  fog_api.chordd_hitTest = ChordT_hitTest<double>;
}

} // Fog namespace
