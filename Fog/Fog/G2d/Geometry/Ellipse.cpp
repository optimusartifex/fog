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
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Ellipse - HitTest]
// ============================================================================

template<typename Number>
static bool FOG_CDECL _G2d_EllipseT_hitTest(const typename EllipseT<Number>::T* self, const typename PointT<Number>::T* pt)
{
  // Point-In-Ellipse problem reduced to Point-In-Circle problem. It is always
  // possible to scale an ellipse to get a circle. This trick is used to get
  // the hit-test without using too much calculations.
  Number rx = Math::abs(self->radius.x);
  Number ry = Math::abs(self->radius.y);

  Number x = pt->x - self->center.x;
  Number y = pt->y - self->center.y;

  // No radius or very close to zero (can't hit-test in such case).
  if (Math::isFuzzyZero(rx) || Math::isFuzzyZero(ry)) return false;

  if (rx > ry)
  {
    y = y * rx / ry;
  }
  else if (rx < ry)
  {
    x = x * ry / rx;
    rx = ry;
  }

  Number ptDist = Math::pow2(x) + Math::pow2(y);
  Number maxDist = Math::pow2(rx);

  return ptDist <= maxDist;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_ellipse_init(void)
{
  _g2d.ellipsef.hitTest = _G2d_EllipseT_hitTest<float>;
  _g2d.ellipsed.hitTest = _G2d_EllipseT_hitTest<double>;
}

} // Fog namespace
