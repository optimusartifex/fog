// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Internal_Core_p.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/CBezier.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Circle - GetBoundingBox]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL _CircleT_getBoundingBox(
  const NumT_(Circle)* self,
  NumT_(Box)* dst,
  const NumT_(Transform)* transform)
{
  NumT cx = self->center.x;
  NumT cy = self->center.y;
  NumT rx = self->radius;
  NumT ry = self->radius;

  if (transform == NULL) goto _Identity;

  switch (transform->getType())
  {
    case TRANSFORM_TYPE_SWAP:
      rx = rx * transform->_10;
      ry = ry * transform->_01;

      cx = self->center.y * transform->_10;
      cy = self->center.x * transform->_01;
      goto _Translation;

    case TRANSFORM_TYPE_SCALING:
      cx *= transform->_00;
      cy *= transform->_11;
      rx *= transform->_00;
      ry *= transform->_11;
      // ... Fall through ...

    case TRANSFORM_TYPE_TRANSLATION:
_Translation:
      cx += transform->_20;
      cy += transform->_21;
      // ... Fall through ...

    case TRANSFORM_TYPE_IDENTITY:
_Identity:
      dst->x0 = cx - rx;
      dst->y0 = cy - ry;
      dst->x1 = cx + rx;
      dst->y1 = cy + ry;

      if (dst->x0 > dst->x1) swap(dst->x0, dst->x1);
      if (dst->y0 > dst->y1) swap(dst->y0, dst->y1);
      break;

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      NumT xx = transform->_00;
      NumT xy = transform->_01;
      NumT yx = transform->_10;
      NumT yy = transform->_11;

      NumT t0 = Math::isFuzzyZero(xx) ? NumT(0.0) : Math::atan(yx / xx);
      NumT t1 = Math::isFuzzyZero(xy) ? NumT(0.0) : Math::atan(yy / xy);

      NumT r = rx;
      NumT ts, tc;

      Math::sincos(t0, &ts, &tc);
      dst->x0 = r*xx*tc + r*yx*ts;

      Math::sincos(t1, &ts, &tc);
      dst->y0 = r*xy*tc + r*yy*ts;

      dst->x1 = -dst->x0;
      dst->y1 = -dst->y0;

      NumT tx = cx * xx + cy * yx + transform->_20;
      NumT ty = cx * xy + cy * yy + transform->_21;

      dst->x0 += tx;
      dst->y0 += ty;
      dst->x1 += tx;
      dst->y1 += ty;

      if (dst->x0 > dst->x1) swap(dst->x0, dst->x1);
      if (dst->y0 > dst->y1) swap(dst->y0, dst->y1);
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      // Analytic solution is too expensive. Convert to Path.
      NumT_T1(PathTmp, 16) pathTmp;
      pathTmp.circle(*self);
      return pathTmp.getBoundingBox(*dst, *transform);
    }

    case TRANSFORM_TYPE_DEGENERATE:
    {
      dst->reset();
      return ERR_GEOMETRY_DEGENERATE;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Circle - HitTest]
// ============================================================================

template<typename NumT>
static bool FOG_CDECL _CircleT_hitTest(const NumT_(Circle)* self, const NumT_(Point)* pt)
{
  // Hit-test in circle can be simplified to checking the distance of a center
  // point and a given pt. It's not needed to calculate normalized distance
  // using Math::sqrt().

  NumT r = self->radius;

  NumT x = pt->x - self->center.x;
  NumT y = pt->y - self->center.y;

  NumT maxDist = Math::pow2(r);
  NumT ptDist = Math::pow2(x) + Math::pow2(y);

  return ptDist <= maxDist;
}

// ============================================================================
// [Fog::Circle - ToCSpline]
// ============================================================================

template<typename NumT>
static uint FOG_CDECL _CircleT_toCSpline(const NumT_(Circle)* self, NumT_(Point)* pts)
{
  NumT cx = self->center.x;
  NumT cy = self->center.y;

  NumT r = self->radius;
  NumT rKappa = r * NumT(MATH_KAPPA);

  pts[ 0].set(cx + r     , cy         );
  pts[ 1].set(cx + r     , cy + rKappa);
  pts[ 2].set(cx + rKappa, cy + r     );
  pts[ 3].set(cx         , cy + r     );
  pts[ 4].set(cx - rKappa, cy + r     );
  pts[ 5].set(cx - r     , cy + rKappa);
  pts[ 6].set(cx - r     , cy         );
  pts[ 7].set(cx - r     , cy - rKappa);
  pts[ 8].set(cx - rKappa, cy - r     );
  pts[ 9].set(cx         , cy - r     );
  pts[10].set(cx + rKappa, cy - r     );
  pts[11].set(cx + r     , cy - rKappa);
  pts[12].set(cx + r     , cy         );

  return 13;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_circle_init(void)
{
  _g2d.circlef.getBoundingBox = _CircleT_getBoundingBox<float>;
  _g2d.circled.getBoundingBox = _CircleT_getBoundingBox<double>;

  _g2d.circlef.hitTest = _CircleT_hitTest<float>;
  _g2d.circled.hitTest = _CircleT_hitTest<double>;

  _g2d.circlef.toCSpline = _CircleT_toCSpline<float>;
  _g2d.circled.toCSpline = _CircleT_toCSpline<double>;
}

} // Fog namespace
