// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Geometry/Line.h>
#include <Fog/G2d/Global/Init_p.h>

namespace Fog {

// ============================================================================
// [Fog::Line - Intersect]
// ============================================================================

template<typename Number>
static uint32_t FOG_CDECL _G2d_LineT_intersect(typename PointT<Number>::T* dst,
  const typename PointT<Number>::T* lineA,
  const typename PointT<Number>::T* lineB)
{
  typename PointT<Number>::T ptA = lineA[1] - lineA[0];
  typename PointT<Number>::T ptB = lineB[1] - lineB[0];

  Number d = ptA.y * ptB.x - ptA.x * ptB.y;
  if (d == Number(0.0) || !Math::isFinite(d)) return LINE_INTERSECTION_NONE;

  d = Math::recip(d);

  typename PointT<Number>::T off = lineA[0] - lineB[0];
  Number t = (ptB.y * off.x - ptB.x * off.y) * d;
  dst->set(lineA[0].x + ptA.x * t, lineA[0].y + ptA.y * t);

  if (t < Number(0.0) && t > Number(1.0)) return LINE_INTERSECTION_UNBOUNDED;
  t = (ptA.x * off.y - ptA.y * off.x) * d;
  if (t < Number(0.0) && t > Number(1.0)) return LINE_INTERSECTION_UNBOUNDED;

  return LINE_INTERSECTION_BOUNDED;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_line_init(void)
{
  _g2d.linef.intersect = _G2d_LineT_intersect<float>;
  _g2d.lined.intersect = _G2d_LineT_intersect<double>;
}

} // Fog namespace
