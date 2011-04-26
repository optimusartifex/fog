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
#include <Fog/G2d/Geometry/Chord.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Chord - HitTest]
// ============================================================================

template<typename Number>
static bool FOG_CDECL _G2d_ChordT_hitTest(const typename ChordT<Number>::T* self, const typename PointT<Number>::T* pt)
{
  Number cx = self->center.x;
  Number cy = self->center.y;

  Number rx = Math::abs(self->radius.x);
  Number ry = Math::abs(self->radius.y);

  Number x = pt->x - cx;
  Number y = pt->y - cy;

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
  Number ptDist = Math::pow2(x) + Math::pow2(y);
  Number maxDist = Math::pow2(rx);

  if (ptDist > maxDist) return false;

  // Hit-test the rest.
  typename PointT<Number>::T p0, p1;

  Number start = self->start;
  Number sweep = self->sweep;

  const Number _2pi = Number(2.0 * MATH_PI);
  // If sweep is larger than a 2pi then hit-test is always positive.
  if (sweep < -_2pi || sweep > _2pi) return true;

  Math::sincos(start        , &p0.y, &p0.x); p0.x *= rx; p0.y *= rx;
  Math::sincos(start + sweep, &p1.y, &p1.x); p1.x *= rx; p1.y *= rx;

  Number dx = p1.x - p0.x;
  Number dy = p1.y - p0.y;

  int onRight = sweep < Number(0.0);

  if (Math::isFuzzyZero(dy))
  {
    if (Math::abs(sweep) < Number(MATH_PI * 0.5)) return false;
    return onRight ? y <= p0.y : y >= p0.y;
  }

  Number lx = p0.x + (y - p0.y) * dx / dy;
  onRight ^= (dy > Number(0.0));

  if (onRight)
    return x >= lx;
  else
    return x < lx;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_chord_init(void)
{
  _g2d.chordf.hitTest = _G2d_ChordT_hitTest<float>;
  _g2d.chordd.hitTest = _G2d_ChordT_hitTest<double>;
}

} // Fog namespace
