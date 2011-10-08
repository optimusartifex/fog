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
#include <Fog/G2d/Geometry/Pie.h>

namespace Fog {

// ============================================================================
// [Fog::Pie - HitTest]
// ============================================================================

template<typename NumT>
static bool FOG_CDECL PieT_hitTest(const NumT_(Pie)* self, const NumT_(Point)* pt)
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

  // If sweep is larger than a 2pi then hit-test is always positive.
  const NumT _2pi = NumT(MATH_TWO_PI);
  if (sweep < -_2pi || sweep > _2pi) return true;

  if (sweep < NumT(0.0)) { start += sweep; sweep = -sweep; }
  if (start <= -_2pi || start >= _2pi) start = Math::mod(start, _2pi);

  NumT angle = Math::repeat(Math::atan2(y, x) - start, _2pi);
  return angle <= sweep;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Pie_init(void)
{
  _api.pief_hitTest = PieT_hitTest<float>;
  _api.pied_hitTest = PieT_hitTest<double>;
}

} // Fog namespace
