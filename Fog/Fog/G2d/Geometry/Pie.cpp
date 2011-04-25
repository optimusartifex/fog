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
#include <Fog/G2d/Geometry/Pie.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Circle - GetBoundingBox]
// ============================================================================

template<typename Number>
static bool FOG_CDECL _G2d_PieT_hitTest(const typename PieT<Number>::T* self, const typename PointT<Number>::T* pt)
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

  // If sweep is larger than a 2pi then hit-test is always positive.
  const Number _2pi = Number(2.0 * MATH_PI);
  if (sweep < -_2pi || sweep > _2pi) return true;

  if (sweep < Number(0.0)) { start += sweep; sweep = -sweep; }
  if (start <= -_2pi || start >= _2pi) start = Math::mod(start, _2pi);

  Number angle = Math::repeat(Math::atan2(y, x) - start, _2pi);
  return angle <= sweep;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_pie_init(void)
{
  _g2d.pief.hitTest = _G2d_PieT_hitTest<float>;
  _g2d.pied.hitTest = _G2d_PieT_hitTest<double>;
}

} // Fog namespace
