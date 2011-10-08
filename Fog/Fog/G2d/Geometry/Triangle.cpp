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
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Geometry/Triangle.h>

namespace Fog {

// ============================================================================
// [Fog::Triangle - GetBoundingBox]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL TriangleT_getBoundingBox(const NumT_(Point)* self,
  NumT_(Box)* dst,
  const NumT_(Transform)* tr)
{
  NumT_(Point) tmp[3];

  if (tr != NULL)
  {
    tr->mapPoints(tmp, self, 3);
    self = tmp;
  }

  NumT xMin, xMax;
  NumT yMin, yMax;

  xMin = xMax = self[0].x;
  yMin = yMax = self[0].y;

  if (xMin > self[1].x) xMin = self[1].x; else xMax = self[1].x;
  if (yMin > self[1].y) yMin = self[1].y; else yMax = self[1].y;

  if (xMin > self[2].x) xMin = self[2].x; else xMax = self[2].x;
  if (yMin > self[2].y) yMin = self[2].y; else yMax = self[2].y;

  dst->setBox(xMin, yMin, xMax, yMax);
  return ERR_OK;
}

// ============================================================================
// [Fog::Triangle - HitTest]
// ============================================================================

template<typename NumT>
static bool FOG_CDECL TriangleT_hitTest(const NumT_(Point)* self,
  const NumT_(Point)* pt)
{
  // Hit-Test using Barycentric Technique.
  NumT v0x = self[2].x - self[0].x;
  NumT v0y = self[2].y - self[0].y;

  NumT v1x = self[1].x - self[0].x;
  NumT v1y = self[1].y - self[0].y;

  NumT v2x = pt->x - self[0].x;
  NumT v2y = pt->y - self[0].y;

  // Dot.
  NumT d00 = v0x * v0x + v0y * v0y;
  NumT d01 = v0x * v1x + v0y * v1y;
  NumT d02 = v0x * v2x + v0y * v2y;

  NumT d11 = v1x * v1x + v1y * v1y;
  NumT d12 = v1x * v2x + v1y * v2y;

  // Barycentric coordinates.
  NumT dRcp = (d00 * d11 - d01 * d01);
  if (Math::isFuzzyZero(dRcp)) return false;

  dRcp = NumT(1.0) / dRcp;
  NumT u = (d11 * d02 - d01 * d12) * dRcp;
  NumT v = (d00 * d12 - d01 * d02) * dRcp;

  return (u >= 0) & (v >= 0) & (u + v <= 1);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Triangle_init(void)
{
  _api.trianglef_getBoundingBox = TriangleT_getBoundingBox<float>;
  _api.triangled_getBoundingBox = TriangleT_getBoundingBox<double>;

  _api.trianglef_hitTest = TriangleT_hitTest<float>;
  _api.triangled_hitTest = TriangleT_hitTest<double>;
}

} // Fog namespace
