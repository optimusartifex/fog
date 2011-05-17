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
#include <Fog/G2d/Geometry/Arc.h>
#include <Fog/G2d/Geometry/CubicCurve.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Arc - GetBoundingBox]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL _G2d_ArcT_getBoundingBox(const NumT_(Arc)* self,
  NumT_(Box)* dst, const NumT_(Transform)* transform, bool includeCenterPoint)
{
  NumT cx = self->center.x;
  NumT cy = self->center.y;

  NumT rx = self->radius.x;
  NumT ry = self->radius.y;

  NumT start = self->start;
  NumT sweep = self->sweep;

  // Orientation is not important, let's normalize arc to start -> start + sweep.
  if (sweep <= -MATH_TWO_PI || sweep >= MATH_TWO_PI)
    return reinterpret_cast<const NumT_(Ellipse)*>(self)->_getBoundingBox(*dst, transform);

  if (sweep < NumT(0.0))
  {
    start += sweep;
    sweep = -sweep;
  }

  if (start >= MATH_TWO_PI || start <= -MATH_TWO_PI)
    start = Math::mod(start, NumT(MATH_TWO_PI));
  if (start < NumT(0.0))
    start += NumT(MATH_TWO_PI);

  NumT end = start + sweep;
  if (transform == NULL) goto _Identity;

  switch (transform->getType())
  {
    case TRANSFORM_TYPE_SCALING:
      cx *= transform->_00;
      cy *= transform->_11;

      rx *= transform->_00;
      ry *= transform->_11;
      // ... Fall through ...

    case TRANSFORM_TYPE_TRANSLATION:
      cx += transform->_20;
      cy += transform->_21;
      // ... Fall through ...

    case TRANSFORM_TYPE_IDENTITY:
    {
_Identity:
      // The bounding box is calculated using unit circle (-1, -1 to 1, 1) and then
      // scaled using radius (rx, ry) and translated using center point (cx, cy).
      NumT minX, minY;
      NumT maxX, maxY;

      if (sweep < NumT(MATH_TWO_PI))
      {
        NumT px, py;

        Math::sincos(start, &minY, &minX);
        maxX = minX;
        maxY = minY;

        Math::sincos(end, &py, &px);
        if (minX > px) minX = px;
        if (minY > py) minY = py;
        if (maxX < px) maxX = px;
        if (maxY < py) maxY = py;

        if (NumT(0.0               ) >= start && NumT(0.0               ) <= end) maxX = NumT(1.0); 
        if (NumT(MATH_TWO_PI       ) >= start && NumT(MATH_TWO_PI       ) <= end) maxX = NumT(1.0); 

        if (NumT(MATH_HALF_PI      ) >= start && NumT(MATH_HALF_PI      ) <= end) maxY = NumT(1.0); 
        if (NumT(MATH_TWO_HALF_PI  ) >= start && NumT(MATH_TWO_HALF_PI  ) <= end) maxY = NumT(1.0); 

        if (NumT(MATH_PI           ) >= start && NumT(MATH_PI           ) <= end) minX =-NumT(1.0); 
        if (NumT(MATH_THREE_PI     ) >= start && NumT(MATH_THREE_PI     ) <= end) minX =-NumT(1.0); 

        if (NumT(MATH_ONE_HALF_PI  ) >= start && NumT(MATH_ONE_HALF_PI  ) <= end) minY =-NumT(1.0); 
        if (NumT(MATH_THREE_HALF_PI) >= start && NumT(MATH_THREE_HALF_PI) <= end) minY =-NumT(1.0); 

        // Include the enter point if asked to.
        if (includeCenterPoint)
        {
          if (minX > NumT(0.0)) minX = NumT(0.0);
          if (maxX < NumT(0.0)) maxX = NumT(0.0);
          if (minY > NumT(0.0)) minY = NumT(0.0);
          if (maxY < NumT(0.0)) maxY = NumT(0.0);
        }

        // Scale.
        minX *= rx;
        minY *= ry;
        maxX *= rx;
        maxY *= ry;
      }
      else
      {
        minX = -rx;
        minY = -ry;
        maxX =  rx;
        maxY =  ry;
      }

      // Translate.
      dst->setBox(cx + minX, cy + minY, cx + maxX, cy + maxY);
      break;
    }

    case TRANSFORM_TYPE_SWAP:
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

      NumT minX, minY;
      NumT maxX, maxY;

      NumT ax, ay;

      Math::sincos(start, &ts, &tc);
      ax = rx_xx*tc + ry_yx*ts;
      ay = rx_xx*tc + ry_yx*ts;

      minX = maxX = ax;
      minY = maxY = ay;

      Math::sincos(end, &ts, &tc);
      ax = rx_xx*tc + ry_yx*ts;
      ay = rx_xx*tc + ry_yx*ts;

      if (minX > ax) minX = ax;
      if (maxX < ax) maxX = ax;
      if (minY > ay) minY = ay;
      if (maxY < ay) maxY = ay;

      if ((t0 >= start && t0 <= end) || t0 + MATH_TWO_PI >= start && t0 + MATH_TWO_PI <= end)
      {
        Math::sincos(t0, &ts, &tc);
        
        ax = rx_xx*tc + ry_yx*ts;
        if (minX > ax) minX = ax;
        if (maxX < ax) maxX = ax;

        ax = -ax;
        if (minX > ax) minX = ax;
        if (maxX < ax) maxX = ax;
      }

      if ((t1 >= start && t1 <= end) || t1 + MATH_TWO_PI >= start && t1 + MATH_TWO_PI <= end)
      {
        Math::sincos(t1, &ts, &tc);
        
        ay = rx_xy*tc + ry_yy*ts;
        if (minY > ay) minY = ay;
        if (maxY < ay) maxY = ay;

        ay = -ay;
        if (minY > ay) minY = ay;
        if (maxY < ay) maxY = ay;
      }

      // Include a enter point if asked to do.
      if (includeCenterPoint)
      {
        if (minX > NumT(0.0)) minX = NumT(0.0);
        if (maxX < NumT(0.0)) maxX = NumT(0.0);
        if (minY > NumT(0.0)) minY = NumT(0.0);
        if (maxY < NumT(0.0)) maxY = NumT(0.0);
      }

      NumT tx = cx * xx + cy * yx + transform->_20;
      NumT ty = cx * xy + cy * yy + transform->_21;

      dst->x0 = minX + tx;
      dst->y0 = minY + ty;
      dst->x1 = maxX + tx;
      dst->y1 = maxY + ty;
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      // Analytic solution is too expensive. Convert to Path.
      NumT_T1(PathTmp, 20) pathTmp;
      if (includeCenterPoint)
        pathTmp.pie(*self);
      else
        pathTmp.chord(*self);
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
// [Fog::Arc - ToCSpline]
// ============================================================================

template<typename NumT>
static uint FOG_CDECL _G2d_ArcT_toCSpline(const NumT_(Arc)* self,
  NumT_(Point)* pts)
{
  NumT cx = self->center.x;
  NumT cy = self->center.y;
  NumT rx = self->radius.x;
  NumT ry = self->radius.y;

  NumT start = self->start;
  NumT sweep = self->sweep;

  if (start >= MATH_TWO_PI || start <= -MATH_TWO_PI)
    start = Math::mod(start, NumT(MATH_TWO_PI));

  NumT z       = NumT(0.0);
  NumT rxOne   = NumT(1.0);
  NumT ryOne   = NumT(1.0);
  NumT rxKappa = NumT(MATH_KAPPA);
  NumT ryKappa = NumT(MATH_KAPPA);

  NumT as = NumT(0.0);
  NumT ac = NumT(1.0);

  if (start != NumT(0.0))
  {
    Math::sincos(start, &as, &ac);
  }

  NumT xx = ac * rx, yx =-as * rx;
  NumT xy = as * ry, yy = ac * ry;

  if (sweep < NumT(0.0))
  {
    ryOne   = -ryOne;
    ryKappa = -ryKappa;
    sweep   = -sweep;
  }

  uint numSegments;
  NumT lastSegment;

  if (sweep >= NumT(MATH_TWO_PI))
  {
    numSegments = 13;
    lastSegment = NumT(MATH_HALF_PI);
  }
  if (sweep >= MATH_ONE_HALF_PI)
  {
    numSegments = 13;
    lastSegment = sweep - NumT(MATH_ONE_HALF_PI);
  }
  else if (sweep >= NumT(MATH_PI))
  {
    numSegments = 10;
    lastSegment = sweep - NumT(MATH_PI);
  }
  else if (sweep >= NumT(MATH_HALF_PI))
  {
    numSegments = 7;
    lastSegment = sweep - NumT(MATH_HALF_PI);
  }
  else
  {
    numSegments = 4;
    lastSegment = sweep;
  }

  if (Math::isFuzzyPositiveZero(lastSegment) && numSegments > 4)
  {
    lastSegment = NumT(MATH_HALF_PI);
    numSegments -= 3;
  }

  pts[ 0].set( rxOne  , z      );
  pts[ 1].set( rxOne  , ryKappa);
  pts[ 2].set( rxKappa, ryOne  );
  pts[ 3].set( z      , ryOne  );
  if (numSegments == 4) goto _Skip;

  pts[ 4].set(-rxKappa, ryOne  );
  pts[ 5].set(-rxOne  , ryKappa);
  pts[ 6].set(-rxOne  , z      );
  if (numSegments == 7) goto _Skip;

  pts[ 7].set(-rxOne  ,-ryKappa);
  pts[ 8].set(-rxKappa,-ryOne  );
  pts[ 9].set( z      ,-ryOne  );
  if (numSegments == 10) goto _Skip;

  pts[10].set( rxKappa,-ryOne  );
  pts[11].set( rxOne  ,-ryKappa);
  pts[12].set( rxOne  , z      );

_Skip:
  if (lastSegment < NumT(MATH_HALF_PI - Math2dConst<NumT>::getMathEpsilon()))
  {
    NumT t = lastSegment / NumT(MATH_HALF_PI);
    NumI_(CubicCurve)::leftAt(&pts[numSegments - 4], &pts[numSegments - 4], t);
  }

  for (uint i = 0; i < numSegments; i++)
  {
    NumT px = pts[i].x;
    NumT py = pts[i].y;

    pts[i].x = cx + px * xx + py * yx;
    pts[i].y = cy + px * xy + py * yy;
  }

  return numSegments;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_arc_init(void)
{
  _g2d.arcf.getBoundingBox = _G2d_ArcT_getBoundingBox<float>;
  _g2d.arcd.getBoundingBox = _G2d_ArcT_getBoundingBox<double>;

  _g2d.arcf.toCSpline = _G2d_ArcT_toCSpline<float>;
  _g2d.arcd.toCSpline = _G2d_ArcT_toCSpline<double>;
}

} // Fog namespace
