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
#include <Fog/Core/Tools/Swap.h>
#include <Fog/G2d/Geometry/CBezier.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

// ============================================================================
// [Fog::Ellipse - GetBoundingBox]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL EllipseT_getBoundingBox(
  const NumT_(Ellipse)* self,
  NumT_(Box)* dst,
  const NumT_(Transform)* transform)
{
  NumT cx = self->center.x;
  NumT cy = self->center.y;

  NumT rx = self->radius.x;
  NumT ry = self->radius.y;

  if (transform == NULL) goto _Identity;

  switch (transform->getType())
  {
    case TRANSFORM_TYPE_SWAP:
      rx = self->radius.y * transform->_10;
      ry = self->radius.x * transform->_01;

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

      NumT rx_xx = rx * xx;
      NumT rx_xy = rx * xy;
      NumT ry_yx = ry * yx;
      NumT ry_yy = ry * yy;

      NumT t0 = Math::isFuzzyZero(rx_xx) ? NumT(0.0) : Math::atan((ry_yx) / (rx_xx));
      NumT t1 = Math::isFuzzyZero(rx_xy) ? NumT(0.0) : Math::atan((ry_yy) / (rx_xy));

      NumT ts, tc;

      Math::sincos(t0, &ts, &tc);
      dst->x0 = rx_xx*tc + ry_yx*ts;

      Math::sincos(t1, &ts, &tc);
      dst->y0 = rx_xy*tc + ry_yy*ts;

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
      pathTmp.ellipse(*self);
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
// [Fog::Ellipse - HitTest]
// ============================================================================

template<typename NumT>
static bool FOG_CDECL EllipseT_hitTest(const NumT_(Ellipse)* self, const NumT_(Point)* pt)
{
  // Point-In-Ellipse problem reduced to Point-In-Circle problem. It is always
  // possible to scale an ellipse to get a circle. This trick is used to get
  // the hit-test without using too much calculations.
  NumT rx = Math::abs(self->radius.x);
  NumT ry = Math::abs(self->radius.y);

  NumT x = pt->x - self->center.x;
  NumT y = pt->y - self->center.y;

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

  NumT ptDist = Math::pow2(x) + Math::pow2(y);
  NumT maxDist = Math::pow2(rx);

  return ptDist <= maxDist;
}

// ============================================================================
// [Fog::Ellipse - ToCSpline]
// ============================================================================

template<typename NumT>
static uint FOG_CDECL EllipseT_toCSpline(const NumT_(Ellipse)* self, NumT_(Point)* pts)
{
  NumT cx = self->center.x;
  NumT cy = self->center.y;

  NumT rx = self->radius.x;
  NumT ry = self->radius.y;

  NumT rxKappa = rx * NumT(MATH_KAPPA);
  NumT ryKappa = ry * NumT(MATH_KAPPA);

  pts[ 0].set(cx + rx     , cy          );
  pts[ 1].set(cx + rx     , cy + ryKappa);
  pts[ 2].set(cx + rxKappa, cy + ry     );
  pts[ 3].set(cx          , cy + ry     );
  pts[ 4].set(cx - rxKappa, cy + ry     );
  pts[ 5].set(cx - rx     , cy + ryKappa);
  pts[ 6].set(cx - rx     , cy          );
  pts[ 7].set(cx - rx     , cy - ryKappa);
  pts[ 8].set(cx - rxKappa, cy - ry     );
  pts[ 9].set(cx          , cy - ry     );
  pts[10].set(cx + rxKappa, cy - ry     );
  pts[11].set(cx + rx     , cy - ryKappa);
  pts[12].set(cx + rx     , cy          );

  return 13;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Ellipse_init(void)
{
  _api.ellipsef_getBoundingBox = EllipseT_getBoundingBox<float>;
  _api.ellipsed_getBoundingBox = EllipseT_getBoundingBox<double>;

  _api.ellipsef_hitTest = EllipseT_hitTest<float>;
  _api.ellipsed_hitTest = EllipseT_hitTest<double>;

  _api.ellipsef_toCSpline = EllipseT_toCSpline<float>;
  _api.ellipsed_toCSpline = EllipseT_toCSpline<double>;
}

} // Fog namespace
