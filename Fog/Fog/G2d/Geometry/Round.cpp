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
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Round - HitTest]
// ============================================================================

template<typename Number>
static bool FOG_CDECL _G2d_RoundT_hitTest(const typename RoundT<Number>::T* self, const typename PointT<Number>::T* pt)
{
  Number x = pt->x - self->rect.x;
  Number y = pt->y - self->rect.y;

  Number w = self->rect.w;
  Number h = self->rect.h;

  Number rx = Math::abs(self->radius.x);
  Number ry = Math::abs(self->radius.y);

  // Hit-test the bounding box.
  if (x < Number(0.0) || y < Number(0.0) || x > w || y > h) return false;

  // Normalize rx/ry.
  rx = Math::min(rx, w * Number(2.0));
  ry = Math::min(ry, h * Number(2.0));

  // Hit-test the inner two boxes.
  if (x >= rx && x <= w - rx) return true;
  if (y >= ry && y <= h - ry) return true;

  // Hit-test the four symmetric rounded corners. There are used the same trick
  // as in Ellipse hit-test (the elliptic corners are scaled to be circualr).
  x -= rx;
  y -= ry;

  if (x >= rx) x -= self->rect.w - rx - rx;
  if (y >= ry) y -= self->rect.h - ry - ry;

  // No radius or very close to zero (positive hit-test).
  if (Math::isFuzzyZero(rx) || Math::isFuzzyZero(ry)) return true;

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

FOG_NO_EXPORT void _g2d_round_init(void)
{
  _g2d.roundf.hitTest = _G2d_RoundT_hitTest<float>;
  _g2d.roundd.hitTest = _G2d_RoundT_hitTest<double>;
}

} // Fog namespace
