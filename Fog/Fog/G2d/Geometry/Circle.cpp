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
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Circle - HitTest]
// ============================================================================

template<typename Number>
static bool FOG_CDECL _G2d_CircleT_hitTest(const typename CircleT<Number>::T* self, const typename PointT<Number>::T* pt)
{
  // Hit-test in circle can be simplified to checking the distance of a center
  // point and a given pt. It's not needed to calculate normalized distance
  // using Math::sqrt().

  Number r = self->radius;

  Number x = pt->x - self->center.x;
  Number y = pt->y - self->center.y;

  Number maxDist = Math::pow2(r);
  Number ptDist = Math::pow2(x) + Math::pow2(y);

  return ptDist <= maxDist;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_circle_init(void)
{
  _g2d.circlef.hitTest = _G2d_CircleT_hitTest<float>;
  _g2d.circled.hitTest = _G2d_CircleT_hitTest<double>;
}

} // Fog namespace
