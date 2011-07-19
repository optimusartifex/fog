// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

#include <Fog/Core/Cpu/Cpu.h>
#include <Fog/Core/Cpu/Initializer.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Internals_p.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

// C implementation can be disabled when hardcoding for SSE2.
#if !defined(FOG_HARDCODE_SSE2)
# define FOG_TRANSFORM_INIT_C
#endif // FOG_HARDCODE_SSE2

// ============================================================================
// [Fog::Transform - Helpers]
// ============================================================================

template<typename NumT>
static bool TransformT_squareToQuad(NumT_(Transform)& self, const NumT_(Point)* p)
{
  NumT dx = p[0].x - p[1].x + p[2].x - p[3].x;
  NumT dy = p[0].y - p[1].y + p[2].y - p[3].y;

  if (dx == NumT(0.0) && dy == NumT(0.0))
  {
    // Affine case (parallelogram).
    self._setData(TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY,
      p[1].x - p[0].x, p[1].y - p[0].y, NumT(0.0),
      p[2].x - p[1].x, p[2].y - p[1].y, NumT(0.0),
      p[0].x         , p[0].y         , NumT(1.0));

    return true;
  }
  else
  {
    // Higher precision.
    double dx1 = double(p[1].x) - double(p[2].x);
    double dy1 = double(p[1].y) - double(p[2].y);
    double dx2 = double(p[3].x) - double(p[2].x);
    double dy2 = double(p[3].y) - double(p[2].y);
    double den = dx1 * dy2 - dx2 * dy1;

    if (Math::isFuzzyZero(den))
    {
      // Singular case.
      self._setData(TRANSFORM_TYPE_DEGENERATE,
        NumT(0.0), NumT(0.0), NumT(0.0),
        NumT(0.0), NumT(0.0), NumT(0.0),
        NumT(0.0), NumT(0.0), NumT(0.0));
      return false;
    }
    else
    {
      // General case.
      double u = (dx * dy2 - dy * dx2) / den;
      double v = (dy * dx1 - dx * dy1) / den;

      self._setData(
        TRANSFORM_TYPE_PROJECTION,
        NumT(double(p[1].x) - double(p[0].x) + u * double(p[1].x)),
        NumT(double(p[1].y) - double(p[0].y) + u * double(p[1].y)),
        NumT(u),
        NumT(double(p[3].x) - double(p[0].x) + v * double(p[3].x)),
        NumT(double(p[3].y) - double(p[0].y) + v * double(p[3].y)),
        NumT(v),
        NumT(p[0].x),
        NumT(p[0].y),
        NumT(1.0));
      return true;
    }
  }
}

// ============================================================================
// [Fog::Transform - Create]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL TransformT_create(NumT_(Transform)& self,
  uint32_t createType, const void* params)
{
  const NumT* paramsT = reinterpret_cast<const NumT*>(params);

  NumT p0;
  NumT p1;

  switch (createType)
  {
    // ------------------------------------------------------------------------
    // [Identity]
    // ------------------------------------------------------------------------

    default:
    case TRANSFORM_CREATE_IDENTITY:
    {
_CreateIdentity:
      self._setData(TRANSFORM_TYPE_IDENTITY,
        NumT(1.0), NumT(0.0), NumT(0.0),
        NumT(0.0), NumT(1.0), NumT(0.0),
        NumT(0.0), NumT(0.0), NumT(1.0));
      break;
    }

    // ------------------------------------------------------------------------
    // [Translation]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_TRANSLATION:
    {
      uint32_t type = TRANSFORM_TYPE_TRANSLATION;
      uint32_t identity = 2;

      p0 = paramsT[0];
      p1 = paramsT[1];

      if (Math::isFuzzyZero(p0)) { p0 = NumT(0.0); identity--; }
      if (Math::isFuzzyZero(p1)) { p1 = NumT(0.0); if (--identity == 0) goto _CreateIdentity; }

      self._setData(type,
        NumT(1.0), NumT(0.0), NumT(0.0),
        NumT(0.0), NumT(1.0), NumT(0.0),
        p0         , p1         , NumT(1.0));
      break;
    }

    // ------------------------------------------------------------------------
    // [Scaling]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_SCALING:
    {
      uint32_t identity = 2;

      p0 = paramsT[0];
      p1 = paramsT[1];

      if (Math::isFuzzyEq(p0, NumT(1.0))) { p0 = NumT(1.0); identity--; }
      if (Math::isFuzzyEq(p1, NumT(1.0))) { p1 = NumT(1.0); if (--identity == 0) goto _CreateIdentity; }

      self._setData(TRANSFORM_TYPE_SCALING,
        p0         , NumT(0.0), NumT(0.0),
        NumT(0.0), p1         , NumT(0.0),
        NumT(0.0), NumT(0.0), NumT(1.0));
      break;
    }

    // ------------------------------------------------------------------------
    // [Rotation]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_ROTATION:
    {
      NumT angle = paramsT[0];

      NumT as;
      NumT ac;
      Math::sincos(angle, &as, &ac);
      if (Math::isFuzzyZero(as)) goto _CreateIdentity;

      self._setData(TRANSFORM_TYPE_ROTATION,
        ac         , as         , NumT(0.0),
       -as         , ac         , NumT(0.0),
        NumT(0.0)  , NumT(0.0)  , NumT(1.0));
      break;
    }

    // ------------------------------------------------------------------------
    // [Skewing]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_SKEWING:
    {
      uint32_t identity = 2;

      p0 = tan(paramsT[0]);
      p1 = p0;

      if (paramsT[0] != paramsT[1])
        p1 = tan(paramsT[1]);

      if (Math::isFuzzyZero(p0)) { p0 = NumT(0.0); identity--; }
      if (Math::isFuzzyZero(p1)) { p1 = NumT(0.0); if (--identity) goto _CreateIdentity; }

      self._setData(TRANSFORM_TYPE_AFFINE,
        NumT(1.0), p1         , NumT(0.0),
        p0         , NumT(1.0), NumT(0.0),
        NumT(0.0), NumT(0.0), NumT(1.0));
      break;
    }

    // ------------------------------------------------------------------------
    // [Line-Segment]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_LINE_SEGMENT:
    {
      NumT x1 = paramsT[0];
      NumT y1 = paramsT[1];
      NumT x2 = paramsT[2];
      NumT y2 = paramsT[3];

      NumT dx = x2 - x1;
      NumT dy = y2 - y1;

      NumT dist = paramsT[4];

      NumT as = NumT(0.0);
      NumT ac = NumT(0.0);

      if (dist > NumT(0.0))
      {
        dist = Math::hypot(dx, dy);
        if (Math::isFuzzyZero(dist))
          goto _CreateLineSegment_SkipDiv;
      }

      as = dy / dist;
      ac = dx / dist;

_CreateLineSegment_SkipDiv:
      self._setData(TRANSFORM_TYPE_AFFINE,
        ac, as, NumT(0.0),
       -as, ac, NumT(0.0),
        x1, y1, NumT(1.0));
      break;
    }

    // ------------------------------------------------------------------------
    // [Reflection]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_REFLECTION_U:
    {
      Math::sincos(paramsT[0], &p1, &p0);
      goto _CreateReflectionUnit;
    }

    case TRANSFORM_CREATE_REFLECTION_XY:
    {
      p0 = paramsT[0];
      p1 = paramsT[1];

      NumT t = Math::hypot(p0, p1);
      p0 /= t;
      p1 /= t;

      goto _CreateReflectionUnit;
    }

    case TRANSFORM_CREATE_REFLECTION_UNIT:
    {
      p0 = paramsT[0];
      p1 = paramsT[1];

_CreateReflectionUnit:
      {
        NumT p02 = p0 * NumT(2.0);
        NumT p12 = p1 * NumT(2.0);

        self._setData(TRANSFORM_TYPE_AFFINE,
          p02 * p0 - NumT(1.0), p02 * p1              , NumT(0.0),
          p02 * p1              , p12 * p1 - NumT(1.0), NumT(0.0),
          NumT(0.0)           , NumT(0.0)           , NumT(1.0));
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Parallelogram-To-Parallelogram]
    //
    // Transform a parallelogram to another one. Dst/src are tree points that
    // identify three corners of the parallelograms assuming implicit fourth point.
    //
    // Coordinates are:
    //        *-----------------*
    //       /          (x3,y3)/
    //      /                 /
    //     /(x1,y1)   (x2,y2)/
    //    *-----------------*
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_PARALLELOGRAM:
    {
      const NumT_(ParallelogramParams)* p = reinterpret_cast<const NumT_(ParallelogramParams)*>(params);

      NumT_(Transform) dm(
        p->dst[1].x - p->dst[0].x, p->dst[1].y - p->dst[0].y,
        p->dst[2].x - p->dst[0].x, p->dst[2].y - p->dst[0].y,
        p->dst[0].x              , p->dst[0].y             );
      NumT_(Transform) sm(
        p->src[1].x - p->src[0].x, p->src[1].y - p->src[0].y,
        p->src[2].x - p->src[0].x, p->src[2].y - p->src[0].y,
        p->src[0].x              , p->src[0].y             );

      // We know the types...
      dm._type = TRANSFORM_TYPE_AFFINE;
      sm._type = TRANSFORM_TYPE_AFFINE;
      if (!sm.invert()) goto _CreateIdentity;

      NumI_(Transform)::multiply(self, sm, dm);
      break;
    }

    // ------------------------------------------------------------------------
    // [Quad-To-Quad]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_QUAD_TO_QUAD:
    {
      const NumT_(QuadToQuadParams)* p = reinterpret_cast<const NumT_(QuadToQuadParams)*>(params);

      NumT_(Transform) dm(UNINITIALIZED);
      NumT_(Transform) sm(UNINITIALIZED);

      if (!TransformT_squareToQuad<NumT>(dm, p->dst) ||
          !TransformT_squareToQuad<NumT>(sm, p->src) ||
          !sm.invert())
      {
        goto _CreateIdentity;
      }

      NumI_(Transform)::multiply(self, sm, dm);
      break;
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Transform - Update]
// ============================================================================

template<typename NumT>
static uint32_t FOG_CDECL TransformT_update(const NumT_(Transform)& self)
{
  uint32_t type = self._type;

  switch (type & (TRANSFORM_TYPE_DIRTY - 1))
  {
    // Transform is not projection if:
    //   [. . 0]
    //   [. . 0]
    //   [. . 1]
    case TRANSFORM_TYPE_DEGENERATE:
    case TRANSFORM_TYPE_PROJECTION:
      if (!Math::isFuzzyZero(self._02) || !Math::isFuzzyZero(self._12) || !Math::isFuzzyOne (self._22))
      {
        type = TRANSFORM_TYPE_PROJECTION;

        NumT d = (self._22 * self._11 - self._21 * self._12) * self._00 -
                 (self._22 * self._01 - self._21 * self._02) * self._10 +
                 (self._12 * self._01 - self._11 * self._02) * self._20 ;
        if (Math::isFuzzyZero(d))
          type = TRANSFORM_TYPE_DEGENERATE;
        break;
      }
      // ... Fall through ...

    // Transform is not affine/rotation if:
    //   [. 0 .]
    //   [0 . .]
    //   [. . .]
    case TRANSFORM_TYPE_AFFINE:
    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_SWAP:
      if (!Math::isFuzzyZero(self._01) || !Math::isFuzzyZero(self._10))
      {
        if (Math::isFuzzyZero(self._00) && Math::isFuzzyZero(self._11))
        {
          type = TRANSFORM_TYPE_SWAP;
        }
        else
        {
          NumT d = self._00 * self._11 - self._01 * self._10;
          NumT r = self._00 * self._01 + self._10 * self._11;

          type = TRANSFORM_TYPE_AFFINE;
          if (Math::isFuzzyZero(r))
          {
            type = TRANSFORM_TYPE_ROTATION;
            if (Math::isFuzzyZero(self._00) && Math::isFuzzyZero(self._11))
              type = TRANSFORM_TYPE_SWAP;
          }
          if (Math::isFuzzyZero(d)) type = TRANSFORM_TYPE_DEGENERATE;
        }
        break;
      }
      // ... Fall through ...

    // Transform is not scaling if:
    //   [1 . .]
    //   [. 1 .]
    //   [. . .]
    case TRANSFORM_TYPE_SCALING:
      if (!Math::isFuzzyOne(self._00) || !Math::isFuzzyOne(self._11))
      {
        NumT d = self._00 * self._11;

        type = TRANSFORM_TYPE_SCALING;
        if (Math::isFuzzyZero(d)) type = TRANSFORM_TYPE_DEGENERATE;
        break;
      }
      // ... Fall through ...

    // Transform is not translation if:
    //   [. . .]
    //   [. . .]
    //   [0 0 .]
    case TRANSFORM_TYPE_TRANSLATION:
      if (!Math::isFuzzyZero(self._20) || !Math::isFuzzyZero(self._21))
      {
        type = TRANSFORM_TYPE_TRANSLATION;
        break;
      }
      // ... Fall through ...

    // Transform is identity if:
    //   [1 0 0]
    //   [0 1 0]
    //   [0 0 1]
    case TRANSFORM_TYPE_IDENTITY:
      type = TRANSFORM_TYPE_IDENTITY;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  self._type = type;
  return type;
}

// ============================================================================
// [Fog::Transform - Transform]
// ============================================================================

#define ENCODE_OP(_Op_, _Order_) ((uint32_t)(_Op_) | ((uint32_t)(_Order_) << 4))

template<typename NumT>
static err_t FOG_CDECL TransformT_transform(NumT_(Transform)& self, uint32_t opType, const void* params)
{
  const NumT* paramsT = reinterpret_cast<const NumT*>(params);
  uint32_t selfType = self.getType();

  NumT_(Transform) tm(UNINITIALIZED);

  switch (opType)
  {
    // ------------------------------------------------------------------------
    // [Translate]
    //
    // [1 0 0]
    // [0 1 0]
    // [X Y 1]
    // ------------------------------------------------------------------------

    case ENCODE_OP(TRANSFORM_OP_TRANSLATE, MATRIX_ORDER_PREPEND):
    {
      NumT x = paramsT[0];
      NumT y = paramsT[1];

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        {
          self._type = TRANSFORM_TYPE_TRANSLATION;
          // ... Fall through ...
        }
        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._20 += x;
          self._21 += y;
          self._type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          self._20 += x * self._00;
          self._21 += y * self._11;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          self._22 += x * self._02 + y * self._12;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          self._20 += x * self._00 + y * self._10;
          self._21 += x * self._01 + y * self._11;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_TRANSLATE, MATRIX_ORDER_APPEND):
    {
      NumT x = paramsT[0];
      NumT y = paramsT[1];

      self._20 += x;
      self._21 += y;

      self._type = Math::max<uint32_t>(
        self._type                 | TRANSFORM_TYPE_DIRTY,
        TRANSFORM_TYPE_TRANSLATION | TRANSFORM_TYPE_DIRTY);
      break;
    }

    // ------------------------------------------------------------------------
    // [Scale]
    //
    // [X 0 0]
    // [0 Y 0]
    // [0 0 1]
    // ------------------------------------------------------------------------

    case ENCODE_OP(TRANSFORM_OP_SCALE, MATRIX_ORDER_PREPEND):
    {
      NumT x = paramsT[0];
      NumT y = paramsT[1];

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._00 = x;
          self._11 = y;
          self._type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          self._02 *= x;
          self._12 *= y;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          self._01 *= x;
          self._10 *= y;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_SCALING:
        {
          self._00 *= x;
          self._11 *= y;

          self._type = Math::max<uint32_t>(
            self._type             | TRANSFORM_TYPE_DIRTY,
            TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY);
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_SCALE, MATRIX_ORDER_APPEND):
    {
      NumT x = paramsT[0];
      NumT y = paramsT[1];

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._20 *= x;
          self._21 *= y;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_IDENTITY:
        {
          self._00 = x;
          self._11 = y;
          self._type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          self._02 *= x;
          self._12 *= y;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          self._01 *= x;
          self._10 *= y;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_SCALING:
        {
          self._00 *= x;
          self._11 *= y;
          self._20 *= x;
          self._21 *= y;
          self._type = Math::max<uint32_t>(
            self._type             | TRANSFORM_TYPE_DIRTY,
            TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY);
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Rotate]
    //
    // [ CosA SinA 0]
    // [-SinA CosA 0]
    // [ 0    0    1]
    // ------------------------------------------------------------------------

    case ENCODE_OP(TRANSFORM_OP_ROTATE, MATRIX_ORDER_PREPEND):
    {
      NumT angle = paramsT[0];

      NumT as;
      NumT ac;
      Math::sincos(angle, &as, &ac);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._00 = ac;
          self._01 = as;
          self._10 =-as;
          self._11 = ac;

          self._type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          NumT m00 = self._00;
          NumT m11 = self._11;

          self._00 = m00 * ac;
          self._01 = m11 * as;
          self._10 = m00 *-as;
          self._11 = m11 * ac;

          self._type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          NumT m00 = self._00;
          NumT m01 = self._01;
          NumT m10 = self._10;
          NumT m11 = self._11;

          self._00 = m00 * ac + m10 * as;
          self._01 = m01 * ac + m11 * as;
          self._10 = m00 *-as + m10 * ac;
          self._11 = m01 *-as + m11 * ac;

          self._type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          NumT t00 = ac * self._00 + as * self._10;
          NumT t01 = ac * self._01 + as * self._11;
          NumT t02 = ac * self._02 + as * self._12;

          self._10 = -as * self._00 + ac * self._10;
          self._11 = -as * self._01 + ac * self._11;
          self._12 = -as * self._02 + ac * self._12;

          self._00 = t00;
          self._01 = t01;
          self._02 = t02;

          self._type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_ROTATE, MATRIX_ORDER_APPEND):
    {
      NumT angle = paramsT[0];

      NumT as;
      NumT ac;
      Math::sincos(angle, &as, &ac);

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
        {
          NumT t20 = self._20 * ac - self._21 * as;
          NumT t21 = self._20 * as + self._21 * ac;

          self._20 = t20;
          self._21 = t21;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_IDENTITY:
        {
          self._00 = ac;
          self._01 = as;
          self._10 =-as;
          self._11 = ac;

          self._type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          NumT t00 = self._00 * ac;
          NumT t10 = self._11 *-as;
          NumT t20 = self._20 * ac + self._21 *-as;

          self._01 = self._00 * as;
          self._11 = self._11 * ac;
          self._21 = self._20 * as + self._21 * ac;

          self._00 = t00;
          self._10 = t10;
          self._20 = t20;

          self._type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          NumT t00 = self._00 * ac - self._01 * as;
          NumT t10 = self._10 * ac - self._11 * as;
          NumT t20 = self._20 * ac - self._21 * as;

          self._01 = self._00 * as + self._01 * ac;
          self._11 = self._10 * as + self._11 * ac;
          self._21 = self._20 * as + self._21 * ac;

          self._00 = t00;
          self._10 = t10;
          self._20 = t20;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Skew]
    //
    // [ 1   TanY 0]
    // [TanX 1    0]
    // [ 0   0    1]
    // ------------------------------------------------------------------------

    case ENCODE_OP(TRANSFORM_OP_SKEW, MATRIX_ORDER_PREPEND):
    {
      NumT xTan = Math::tan(paramsT[0]);
      NumT yTan = xTan;

      if (paramsT[0] != paramsT[1])
        yTan = Math::tan(paramsT[1]);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
        case TRANSFORM_TYPE_SCALING:
        {
          self._01 = yTan * self._11;
          self._10 = xTan * self._00;

          self._type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        {
          self._type = TRANSFORM_TYPE_AFFINE;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_AFFINE:
skewPrependAffine:
        {
          NumT t00 = yTan * self._10;
          NumT t01 = yTan * self._11;

          self._10 += xTan * self._00;
          self._11 += xTan * self._01;

          self._00 += t00;
          self._01 += t01;

          self._type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          NumT t02 = yTan * self._12;

          self._12 += xTan * self._02;
          self._02 += t02;

          goto skewPrependAffine;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_SKEW, MATRIX_ORDER_APPEND):
    {
      NumT xTan = Math::tan(paramsT[0]);
      NumT yTan = xTan;

      if (paramsT[0] != paramsT[1])
        yTan = Math::tan(paramsT[1]);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        {
          self._01 = yTan;
          self._10 = xTan;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_TRANSLATION:
        {
          NumT t20 = self._21 * xTan;
          NumT t21 = self._20 * yTan;

          self._20 += t20;
          self._21 += t21;

          self._type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          NumT t10 = self._11 * xTan;
          NumT t20 = self._21 * xTan;

          self._01 += self._00 * yTan;
          self._21 += self._20 * yTan;

          self._10 += t10;
          self._20 += t20;

          self._type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        {
          self._type = TRANSFORM_TYPE_AFFINE;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          NumT t00 = self._01 * xTan;
          NumT t10 = self._11 * xTan;
          NumT t20 = self._21 * xTan;

          self._01 += self._00 * yTan;
          self._11 += self._10 * yTan;
          self._21 += self._20 * yTan;

          self._00 += t00;
          self._10 += t10;
          self._20 += t20;

          self._type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Flip]
    // ------------------------------------------------------------------------

    case ENCODE_OP(TRANSFORM_OP_FLIP, MATRIX_ORDER_PREPEND):
    case ENCODE_OP(TRANSFORM_OP_FLIP, MATRIX_ORDER_APPEND):
    {
      uint32_t axis = reinterpret_cast<const uint32_t*>(params)[0];

      if (axis & AXIS_X) { self._00 = -self._00; self._01 = -self._01; self._02 = -self._02; }
      if (axis & AXIS_Y) { self._10 = -self._10; self._11 = -self._11; self._12 = -self._12; }
      if (axis & AXIS_Z) { self._20 = -self._20; self._21 = -self._21; self._22 = -self._22; }

      self._type = Math::max<uint32_t>(selfType, TRANSFORM_TYPE_SCALING) | TRANSFORM_TYPE_DIRTY;
      break;
    }

    // ------------------------------------------------------------------------
    // [Multiply]
    //
    // [S00 S01 S02]
    // [S10 S11 S12]
    // [S20 S21 S22]
    // ------------------------------------------------------------------------

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY, MATRIX_ORDER_PREPEND):
_MultiplyPrepend:
    {
      const NumT_(Transform)& s = *reinterpret_cast<const NumT_(Transform)*>(params);
      uint32_t sType = s.getType();
      uint32_t dType = Math::max(selfType, sType);

      self._type = dType | TRANSFORM_TYPE_DIRTY;
      switch (dType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        {
          break;
        }

        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._20 += s._20;
          self._21 += s._21;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          NumT t20 = s._20 * self._00;
          NumT t21 = s._21 * self._11;

          self._00 *= s._00;
          self._11 *= s._11;

          self._20 += t20;
          self._21 += t21;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          NumT t00 = s._00 * self._00 + s._01 * self._10;
          NumT t01 = s._00 * self._01 + s._01 * self._11;

          NumT t10 = s._10 * self._00 + s._11 * self._10;
          NumT t11 = s._10 * self._01 + s._11 * self._11;

          NumT t20 = s._20 * self._00 + s._21 * self._10;
          NumT t21 = s._20 * self._01 + s._21 * self._11;

          self._00 = t00;
          self._01 = t01;
          self._10 = t10;
          self._11 = t11;

          self._20 += t20;
          self._21 += t21;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          NumT t00 = s._00 * self._00 + s._01 * self._10 + s._02 * self._20;
          NumT t01 = s._00 * self._01 + s._01 * self._11 + s._02 * self._21;
          NumT t02 = s._00 * self._02 + s._01 * self._12 + s._02 * self._22;

          NumT t10 = s._10 * self._00 + s._11 * self._10 + s._12 * self._20;
          NumT t11 = s._10 * self._01 + s._11 * self._11 + s._12 * self._21;
          NumT t12 = s._10 * self._02 + s._11 * self._12 + s._12 * self._22;

          NumT t20 = s._20 * self._00 + s._21 * self._10 + s._22 * self._20;
          NumT t21 = s._20 * self._01 + s._21 * self._11 + s._22 * self._21;
          NumT t22 = s._20 * self._02 + s._21 * self._12 + s._22 * self._22;

          self._00 = t00;
          self._01 = t01;
          self._02 = t02;

          self._10 = t10;
          self._11 = t11;
          self._12 = t12;

          self._20 = t20;
          self._21 = t21;
          self._22 = t22;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY, MATRIX_ORDER_APPEND):
_MultiplyAppend:
    {
      const NumT_(Transform)& s = *reinterpret_cast<const NumT_(Transform)*>(params);
      uint32_t sType = s.getType();
      uint32_t dType = Math::max(selfType, sType);

      self._type = dType | TRANSFORM_TYPE_DIRTY;
      switch (dType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        {
          break;
        }

        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._20 += s._20;
          self._21 += s._21;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          NumT t00 = self._00 * s._00;
          NumT t11 = self._11 * s._11;

          NumT t20 = self._20 * s._00 + s._20;
          NumT t21 = self._21 * s._11 + s._21;

          self._00 = t00;
          self._11 = t11;

          self._20 = t20;
          self._21 = t21;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          NumT t00 = self._00 * s._00 + self._01 * s._10;
          NumT t01 = self._00 * s._01 + self._01 * s._11;

          NumT t10 = self._10 * s._00 + self._11 * s._10;
          NumT t11 = self._10 * s._01 + self._11 * s._11;

          NumT t20 = self._20 * s._00 + self._21 * s._10 + s._20;
          NumT t21 = self._20 * s._01 + self._21 * s._11 + s._21;

          self._00 = t00;
          self._01 = t01;

          self._10 = t10;
          self._11 = t11;

          self._20 = t20;
          self._21 = t21;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          NumT t00 = self._00 * s._00 + self._01 * s._10 + self._02 * s._20;
          NumT t01 = self._00 * s._01 + self._01 * s._11 + self._02 * s._21;
          NumT t02 = self._00 * s._02 + self._01 * s._12 + self._02 * s._22;

          NumT t10 = self._10 * s._00 + self._11 * s._10 + self._12 * s._20;
          NumT t11 = self._10 * s._01 + self._11 * s._11 + self._12 * s._21;
          NumT t12 = self._10 * s._02 + self._11 * s._12 + self._12 * s._22;

          NumT t20 = self._20 * s._00 + self._21 * s._10 + self._22 * s._20;
          NumT t21 = self._20 * s._01 + self._21 * s._11 + self._22 * s._21;
          NumT t22 = self._20 * s._02 + self._21 * s._12 + self._22 * s._22;

          self._00 = t00;
          self._01 = t01;
          self._02 = t02;

          self._10 = t10;
          self._11 = t11;
          self._12 = t12;

          self._20 = t20;
          self._21 = t21;
          self._22 = t22;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Multiply Inverted]
    //
    // [S00 S01 S02]
    // [S10 S11 S12]
    // [S20 S21 S22]
    // ------------------------------------------------------------------------

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INV, MATRIX_ORDER_PREPEND):
    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INV, MATRIX_ORDER_APPEND):
    {
      if (!NumI_(Transform)::invert(tm, *reinterpret_cast<const NumT_(Transform)*>(params)))
        return ERR_GEOMETRY_DEGENERATE;

      params = &tm;
      if (opType == ENCODE_OP(TRANSFORM_OP_MULTIPLY_INV, MATRIX_ORDER_PREPEND))
        goto _MultiplyPrepend;
      else
        goto _MultiplyAppend;
    }

    // ------------------------------------------------------------------------
    // [End]
    // ------------------------------------------------------------------------

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_OK;
}

template<typename NumT>
static NumT_(Transform) FOG_CDECL TransformT_transformed(const NumT_(Transform)& self,
  uint32_t opType, const void* params)
{
  NumT_(Transform) result(self);
  result._transform(opType, params);
  return result;
}

// ============================================================================
// [Fog::Transform - Multiply]
// ============================================================================

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_multiply(NumT_(Transform)& dst,
  const NumT_(Transform)& a,
  const SrcT_(Transform)& b)
{
  FOG_ASSERT(reinterpret_cast<void*>(&dst) != reinterpret_cast<const void*>(&a));
  FOG_ASSERT(reinterpret_cast<void*>(&dst) != reinterpret_cast<const void*>(&b));

  uint32_t aType = a.getType();
  uint32_t bType = b.getType();
  uint32_t dType = Math::max(aType, bType);

  dst._type = dType | TRANSFORM_TYPE_DIRTY;

  switch (dType)
  {
    case TRANSFORM_TYPE_IDENTITY:
      dst._00 = NumT(1.0); dst._01 = NumT(0.0); dst._02 = NumT(0.0);
      dst._10 = NumT(0.0); dst._11 = NumT(1.0); dst._12 = NumT(0.0);
      dst._20 = NumT(0.0); dst._21 = NumT(0.0); dst._22 = NumT(1.0);
      break;

    case TRANSFORM_TYPE_TRANSLATION:
      dst._00 = NumT(1.0);
      dst._01 = NumT(0.0);
      dst._02 = NumT(0.0);

      dst._10 = NumT(0.0);
      dst._11 = NumT(1.0);
      dst._12 = NumT(0.0);

      dst._20 = NumT(a._20) + NumT(b._20);
      dst._21 = NumT(a._21) + NumT(b._21);
      dst._22 = NumT(1.0);
      break;

    case TRANSFORM_TYPE_SCALING:
      dst._00 = NumT(a._00) * NumT(b._00);
      dst._01 = NumT(0.0);
      dst._02 = NumT(0.0);

      dst._10 = NumT(0.0);
      dst._11 = NumT(a._11) * NumT(b._11);
      dst._12 = NumT(0.0);

      dst._20 = NumT(a._20) * NumT(b._00) + NumT(b._20);
      dst._21 = NumT(a._21) * NumT(b._11) + NumT(b._21);
      dst._22 = NumT(1.0);
      break;

    case TRANSFORM_TYPE_SWAP:
    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
      dst._00 = NumT(a._00) * NumT(b._00) + NumT(a._01) * NumT(b._10);
      dst._01 = NumT(a._00) * NumT(b._01) + NumT(a._01) * NumT(b._11);
      dst._02 = NumT(0.0);

      dst._10 = NumT(a._10) * NumT(b._00) + NumT(a._11) * NumT(b._10);
      dst._11 = NumT(a._10) * NumT(b._01) + NumT(a._11) * NumT(b._11);
      dst._12 = NumT(0.0);

      dst._20 = NumT(a._20) * NumT(b._00) + NumT(a._21) * NumT(b._10) + NumT(b._20);
      dst._21 = NumT(a._20) * NumT(b._01) + NumT(a._21) * NumT(b._11) + NumT(b._21);
      dst._22 = NumT(1.0);
      break;

    case TRANSFORM_TYPE_DEGENERATE:
    case TRANSFORM_TYPE_PROJECTION:
      dst._00 = NumT(a._00) * NumT(b._00) + NumT(a._01) * NumT(b._10) + NumT(a._02) * NumT(b._20);
      dst._01 = NumT(a._00) * NumT(b._01) + NumT(a._01) * NumT(b._11) + NumT(a._02) * NumT(b._21);
      dst._02 = NumT(a._00) * NumT(b._02) + NumT(a._01) * NumT(b._12) + NumT(a._02) * NumT(b._22);

      dst._10 = NumT(a._10) * NumT(b._00) + NumT(a._11) * NumT(b._10) + NumT(a._12) * NumT(b._20);
      dst._11 = NumT(a._10) * NumT(b._01) + NumT(a._11) * NumT(b._11) + NumT(a._12) * NumT(b._21);
      dst._12 = NumT(a._10) * NumT(b._02) + NumT(a._11) * NumT(b._12) + NumT(a._12) * NumT(b._22);

      dst._20 = NumT(a._20) * NumT(b._00) + NumT(a._21) * NumT(b._10) + NumT(a._22) * NumT(b._20);
      dst._21 = NumT(a._20) * NumT(b._01) + NumT(a._21) * NumT(b._11) + NumT(a._22) * NumT(b._21);
      dst._22 = NumT(a._20) * NumT(b._02) + NumT(a._21) * NumT(b._12) + NumT(a._22) * NumT(b._22);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::Transform - Invert]
// ============================================================================

template<typename NumT>
static bool FOG_CDECL TransformT_invert(NumT_(Transform)& self,
  const NumT_(Transform)& a)
{
  // Inverted matrix should be as accurate as possible so the 'double'
  // type is used for calculations.
  switch (a.getType())
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      self._setData(a._type,
        NumT(1.0), NumT(0.0), NumT(0.0),
        NumT(0.0), NumT(1.0), NumT(0.0),
        NumT(0.0), NumT(0.0), NumT(1.0));
      return true;
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      self._setData(a._type,
        NumT(1.0)  , NumT(0.0)  , NumT(0.0),
        NumT(0.0)  , NumT(1.0)  , NumT(0.0),
       -NumT(a._20),-NumT(a._21), NumT(1.0));
      return true;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      if (Math::isFuzzyZero(a._00 * a._11)) goto _NonInvertible;

      double inv00 = 1.0 / double(a._00);
      double inv11 = 1.0 / double(a._11);

      self._setData(a._type,
        NumT(inv00),
        NumT(0.0),
        NumT(0.0),

        NumT(0.0),
        NumT(inv11),
        NumT(0.0),

        NumT(inv00 * -double(a._20)),
        NumT(inv11 * -double(a._21)),
        NumT(1.0)
      );
      return true;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      double d = -double(a._01) * double(a._10);
      if (Math::isFuzzyZero(d)) goto _NonInvertible;

      d = 1.0 / d;

      double t01 = -double(a._01) * d;
      double t10 = -double(a._10) * d;
      double t20 = -double(a._21) * t10;
      double t21 = -double(a._20) * t01;

      self._01 = NumT(t01);
      self._10 = NumT(t10);
      self._20 = NumT(t20);
      self._21 = NumT(t21);
      return true;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      // Inverted matrix should be as accurate as possible so the 'double'
      // type is used instead of 'float' here.
      double d = (double(a._00) * double(a._11) - double(a._01) * double(a._10));
      if (Math::isFuzzyZero(d)) goto _NonInvertible;

      d = 1.0 / d;

      double t00 =  double(a._11) * d;
      double t01 = -double(a._01) * d;
      double t10 = -double(a._10) * d;
      double t11 =  double(a._00) * d;
      double t20 = -double(a._20) * t00 - double(a._21) * t10;
      double t21 = -double(a._20) * t01 - double(a._21) * t11;

      self._setData(a._type,
        NumT(t00), NumT(t01), NumT(0.0),
        NumT(t10), NumT(t11), NumT(0.0),
        NumT(t20), NumT(t21), NumT(1.0)
      );
      return true;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      // Inverted matrix should be as accurate as possible so the 'double'
      // type is used instead of 'float' here.
      double d0 = double(a._11) * double(a._22) - double(a._12) * double(a._21);
      double d1 = double(a._02) * double(a._21) - double(a._01) * double(a._22);
      double d2 = double(a._01) * double(a._12) - double(a._02) * double(a._11);

      double d  = double(a._00) * d0 +
                  double(a._10) * d1 +
                  double(a._20) * d2;
      if (Math::isFuzzyZero(d)) goto _NonInvertible;

      d = 1.0 / d;
      self._setData(a._type,
        NumT(d * d0),
        NumT(d * d1),
        NumT(d * d2),

        NumT(d * (double(a._12) * double(a._20) - double(a._10) * double(a._22))),
        NumT(d * (double(a._00) * double(a._22) - double(a._02) * double(a._20))),
        NumT(d * (double(a._02) * double(a._10) - double(a._00) * double(a._12))),

        NumT(d * (double(a._10) * double(a._21) - double(a._11) * double(a._20))),
        NumT(d * (double(a._01) * double(a._20) - double(a._00) * double(a._21))),
        NumT(d * (double(a._00) * double(a._11) - double(a._01) * double(a._10)))
      );
      return true;
    }

    case TRANSFORM_TYPE_DEGENERATE:
      goto _NonInvertible;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

_NonInvertible:
  self._setData(TRANSFORM_TYPE_DEGENERATE,
    NumT(0.0), NumT(0.0), NumT(0.0),
    NumT(0.0), NumT(0.0), NumT(0.0),
    NumT(0.0), NumT(0.0), NumT(0.0));
  return false;
}

// ============================================================================
// [Fog::Transform - MapPoint(s)]
// ============================================================================

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointT(const NumT_(Transform)& self,
  NumT_(Point)& dst,
  const SrcT_(Point)& src)
{
  uint32_t selfType = self.getType();

  NumT x = src.x;
  NumT y = src.y;

  switch (selfType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      break;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      x *= self._00;
      y *= self._11;
      // ... Fall through ...
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      x += self._20;
      y += self._21;
      break;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      x *= src.y * self._10;
      y *= src.x * self._01;
      x += self._20;
      y += self._21;
      break;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      NumT _x = x;
      NumT _y = y;

      x = _x * self._00 + _y * self._10 + self._20;
      y = _x * self._01 + _y * self._11 + self._21;
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      NumT _x = x;
      NumT _y = y;
      NumT _w = x * self._02 + y * self._12 + self._22;

      if (Math::isFuzzyZero(_w)) _w = Math2dConst<NumT>::getMathEpsilon();
      _w = NumT(1.0) / _w;

      x = _x * self._00 + _y * self._10 + self._20;
      y = _x * self._01 + _y * self._11 + self._21;

      x *= _w;
      y *= _w;
      break;
    }

    case TRANSFORM_TYPE_DEGENERATE:
    {
      x = NumT(0.0);
      y = NumT(0.0);
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  dst.set(x, y);
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Identity(const NumT_(Transform)& self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  if (sizeof(NumT) == sizeof(SrcT) &&
      reinterpret_cast<void*>(dst) == reinterpret_cast<const void*>(src))
  {
    // NOP if true.
    return;
  }

  for (size_t i = length; i; i--, dst++, src++)
  {
    *dst = *src;
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Translation(const NumT_(Transform)& self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _20 = self._20;
  NumT _21 = self._21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x + _20,
             src->y + _21);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Scaling(const NumT_(Transform)& self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _00 = self._00;
  NumT _11 = self._11;

  NumT _20 = self._20;
  NumT _21 = self._21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x * _00 + _20,
             src->y * _11 + _21);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Swap(const NumT_(Transform)& self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _10 = self._10;
  NumT _01 = self._01;

  NumT _20 = self._20;
  NumT _21 = self._21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->y * _10 + _20,
             src->x * _01 + _21);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Affine(const NumT_(Transform)& self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _00 = self._00;
  NumT _01 = self._01;
  NumT _10 = self._10;
  NumT _11 = self._11;
  NumT _20 = self._20;
  NumT _21 = self._21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x * _00 + src->y * _10 + _20,
             src->x * _01 + src->y * _11 + _21);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Projection(const NumT_(Transform)& self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _00 = self._00;
  NumT _01 = self._01;
  NumT _10 = self._10;
  NumT _11 = self._11;
  NumT _20 = self._20;
  NumT _21 = self._21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    NumT _w = src->x * self._02 + src->y * self._12 + self._22;

    if (Math::isFuzzyZero(_w)) _w = Math2dConst<NumT>::getMathEpsilon();
    _w = NumT(1.0) / _w;

    dst->set((src->x * _00 + src->y * _10 + _20) * _w,
             (src->x * _01 + src->y * _11 + _21) * _w);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Degenerate(const NumT_(Transform)& self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  for (size_t i = 0; i < length; i++) dst[i].reset();
}

// ============================================================================
// [Fog::Transform - MapPath]
// ============================================================================

template<typename NumT, typename SrcT>
static err_t FOG_CDECL TransformT_mapPathT(const NumT_(Transform)& self,
  NumT_(Path)& dst,
  const SrcT_(Path)& src, uint32_t cntOp)
{
  size_t srcLength = src.getLength();
  uint32_t selfType = self.getType();

  if (selfType < TRANSFORM_TYPE_PROJECTION)
  {
    if (cntOp == CONTAINER_OP_REPLACE)
    {
      FOG_RETURN_ON_ERROR(dst.reserve(srcLength));

      self._mapPoints(dst._d->vertices, src._d->vertices, srcLength);
      if (sizeof(NumT) != sizeof(SrcT) || (void*)dst._d != (void*)src._d)
      {
        Memory::copy(dst._d->commands, src._d->commands, srcLength);
        dst._d->length = srcLength;
      }
    }
    else
    {
      size_t pos = dst._add(srcLength);
      if (FOG_UNLIKELY(pos == INVALID_INDEX)) return ERR_RT_OUT_OF_MEMORY;

      self._mapPoints(dst._d->vertices + pos, src._d->vertices, srcLength);
      Memory::copy(dst._d->commands + pos, src._d->commands, srcLength);
    }

    return ERR_OK;
  }
  else
  {
    if (selfType == TRANSFORM_TYPE_DEGENERATE)
    {
      if (cntOp == CONTAINER_OP_REPLACE) dst.clear();
      return ERR_GEOMETRY_DEGENERATE;
    }

    if (sizeof(NumT) == sizeof(SrcT) && (void*)&dst == (void*)&src)
    {
      SrcT_(Path) tmp(src);
      return self.mapPathData(dst, tmp.getCommands(), tmp.getVertices(), srcLength, cntOp);
    }
    else
    {
      return self.mapPathData(dst, src.getCommands(), src.getVertices(), srcLength, cntOp);
    }
  }
}

template<typename NumT, typename SrcT>
static err_t FOG_CDECL TransformT_mapPathDataT(const NumT_(Transform)& self,
  NumT_(Path)& dst,
  const uint8_t* srcCmd, const SrcT_(Point)* srcPts, size_t srcLength, uint32_t cntOp)
{
  if (FOG_UNLIKELY(srcLength == 0))
  {
    if (cntOp == CONTAINER_OP_REPLACE) dst.clear();
    return ERR_OK;
  }

  if (srcCmd[0] == PATH_CMD_QUAD_TO || srcCmd[0] == PATH_CMD_CUBIC_TO)
  {
    if (cntOp == CONTAINER_OP_REPLACE) dst.clear();
    return ERR_GEOMETRY_INVALID;
  }

  uint32_t transformType = self.getType();
  if (transformType == TRANSFORM_TYPE_DEGENERATE)
  {
    if (cntOp == CONTAINER_OP_REPLACE) dst.clear();
    return ERR_GEOMETRY_DEGENERATE;
  }

  if (transformType != TRANSFORM_TYPE_PROJECTION)
  {
    size_t pos = dst._prepare(srcLength, cntOp);
    if (FOG_UNLIKELY(pos == INVALID_INDEX)) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* dstCmd = dst._d->commands + pos;
    NumT_(Point)* dstPts = dst._d->vertices + pos;

    Memory::copy(dstCmd, srcCmd, srcLength);
    self._mapPoints(dstPts, srcPts, srcLength);

    dst._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_DIRTY_CMD;
    return ERR_OK;
  }
  else
  {
    if (srcLength >= SYSUINT_MAX / 4)
      return ERR_RT_OUT_OF_MEMORY;

    size_t pos = dst._prepare(srcLength * 4, cntOp);
    if (FOG_UNLIKELY(pos == INVALID_INDEX)) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* dstCmd = dst._d->commands + pos;
    NumT_(Point)* dstPts = dst._d->vertices + pos;

    size_t i = srcLength;
    uint32_t mask = PATH_DATA_DIRTY_BBOX;
    NumT_(Point) spline[20];

    NumT _00 = self._00;
    NumT _01 = self._01;
    NumT _10 = self._10;
    NumT _11 = self._11;
    NumT _20 = self._20;
    NumT _21 = self._21;
    NumT _w;

    // PATH_CMD_QUAD_TO and PATH_CMD_CUBIC_TO were already checked.
    bool hasPrevious = true;

    do {
      uint8_t c = srcCmd[0];

      switch (c)
      {
        case PATH_CMD_MOVE_TO:
        {
          hasPrevious = true;
          // .. Fall through ...
        }

        case PATH_CMD_LINE_TO:
        {
          if (FOG_UNLIKELY(!hasPrevious)) goto _Invalid;

          _w = srcPts[0].x * self._02 + srcPts[0].y * self._12 + self._22;
          if (Math::isFuzzyZero(_w)) _w = Math2dConst<NumT>::getMathEpsilon();

          NumT _wRecip = NumT(1.0) / _w;

          dstCmd[0] = c;
          dstPts[0].set((srcPts[0].x * _00 + srcPts[0].y * _10 + _20) * _wRecip,
                        (srcPts[0].x * _01 + srcPts[0].y * _11 + _21) * _wRecip);

          dstCmd++;
          dstPts++;

          srcCmd++;
          srcPts++;
          i--;
          break;
        }

        case PATH_CMD_QUAD_TO:
        {
          FOG_ASSERT(i >= 2);

          if (FOG_UNLIKELY(!hasPrevious)) goto _Invalid;
          mask |= PATH_DATA_HAS_QBEZIER;

          if (sizeof(NumT) != sizeof(SrcT))
          {
            NumT_(Point) srcTmp[3];

            srcTmp[0] = srcPts[-1];
            srcTmp[1] = srcPts[ 0];
            srcTmp[2] = srcPts[ 1];

            NumI_(QBezier)::splitHalf(srcTmp, &spline[0], &spline[6]);
            NumI_(QBezier)::splitHalf(&spline[ 0], &spline[0], &spline[2]);
            NumI_(QBezier)::splitHalf(&spline[ 6], &spline[4], &spline[6]);
          }
          else
          {
            NumI_(QBezier)::splitHalf(reinterpret_cast<const NumT_(Point)*>(&srcPts[-1]), &spline[0], &spline[6]);
            NumI_(QBezier)::splitHalf(&spline[ 0], &spline[0], &spline[2]);
            NumI_(QBezier)::splitHalf(&spline[ 6], &spline[4], &spline[6]);
          }

          for (uint j = 1; j < 10; j++)
          {
            NumT w = spline[j].x * self._02 + spline[j].y * self._12 + self._22;
            if (Math::isFuzzyZero(w)) w = Math2dConst<NumT>::getMathEpsilon();

            NumT wRecip = NumT(1.0) / w;

            dstCmd[0] = PATH_CMD_QUAD_TO;
            dstPts[0].set((spline[j].x * _00 + spline[j].y * _10 + _20) * wRecip,
                          (spline[j].x * _01 + spline[j].y * _11 + _21) * wRecip);
            dstCmd++;
            dstPts++;
          }

          srcCmd += 2;
          srcPts += 2;
          i      -= 2;
          break;
        }

        case PATH_CMD_CUBIC_TO:
        {
          FOG_ASSERT(i >= 3);

          if (FOG_UNLIKELY(!hasPrevious)) goto _Invalid;
          mask |= PATH_DATA_HAS_CBEZIER;

          if (sizeof(NumT) != sizeof(SrcT))
          {
            NumT_(Point) srcTmp[3];

            srcTmp[0] = srcPts[-1];
            srcTmp[1] = srcPts[ 0];
            srcTmp[2] = srcPts[ 1];

            NumI_(CBezier)::splitHalf(srcTmp, &spline[0], &spline[9]);
            NumI_(CBezier)::splitHalf(&spline[ 0], &spline[0], &spline[3]);
            NumI_(CBezier)::splitHalf(&spline[ 9], &spline[6], &spline[9]);
          }
          else
          {
            NumI_(CBezier)::splitHalf(reinterpret_cast<const NumT_(Point)*>(&srcPts[-1]), &spline[0], &spline[9]);
            NumI_(CBezier)::splitHalf(&spline[ 0], &spline[0], &spline[3]);
            NumI_(CBezier)::splitHalf(&spline[ 9], &spline[6], &spline[9]);
          }

          for (uint j = 1; j < 13; j++)
          {
            NumT w = spline[j].x * self._02 + spline[j].y * self._12 + self._22;
            if (Math::isFuzzyZero(w)) w = Math2dConst<NumT>::getMathEpsilon();

            NumT wRecip = NumT(1.0) / w;

            dstCmd[0] = PATH_CMD_CUBIC_TO;
            dstPts[0].set((spline[j].x * _00 + spline[j].y * _10 + _20) * wRecip,
                          (spline[j].x * _01 + spline[j].y * _11 + _21) * wRecip);
            dstCmd++;
            dstPts++;
          }

          srcCmd += 3;
          srcPts += 3;
          i      -= 3;
          break;
        }

        case PATH_CMD_CLOSE:
        {
          hasPrevious = false;

          dstCmd[0] = c;
          dstPts[0] = srcPts[0];

          dstCmd++;
          dstPts++;

          srcCmd++;
          srcPts++;
          i--;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    } while (i);

    dst._d->length = (size_t)(dstCmd - dst._d->commands);
    dst._d->flags |= mask;
    return ERR_OK;

_Invalid:
    dst._d->length = pos;
    return ERR_GEOMETRY_INVALID;
  }
}

// ============================================================================
// [Fog::Transform - MapBox]
// ============================================================================

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapBoxT(const NumT_(Transform)& self,
  NumT_(Box)& dst,
  const SrcT_(Box)& src)
{
  uint32_t transformType = self.getType();

  switch (transformType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      dst = src;
      break;
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      dst.x0 = src.x0 + self._20;
      dst.y0 = src.y0 + self._21;
      dst.x1 = src.x1 + self._20;
      dst.y1 = src.y1 + self._21;
      break;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      dst.x0 = src.x0 * self._00 + self._20;
      dst.y0 = src.y0 * self._11 + self._21;
      dst.x1 = src.x1 * self._00 + self._20;
      dst.y1 = src.y1 * self._11 + self._21;

_Swap:
      if (dst.x0 > dst.x1) swap(dst.x0, dst.x1);
      if (dst.y0 > dst.y1) swap(dst.y0, dst.y1);
      break;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      NumT x, y;

      x = src.x0;
      y = src.y0;

      dst.x0 = y * self._10 + self._20;
      dst.y0 = x * self._01 + self._21;

      x = src.x1;
      y = src.y1;

      dst.x1 = y * self._10 + self._20;
      dst.y1 = x * self._01 + self._21;
      goto _Swap;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      NumT x;
      NumT y;

      x = src.x0 * self._00 + src.y0 * self._10;
      y = src.x0 * self._01 + src.y0 * self._11;

      NumT minX = x, maxX = x;
      NumT minY = y, maxY = y;

      x = src.x1 * self._00 + src.y0 * self._10;
      y = src.x1 * self._01 + src.y0 * self._11;

      if (minX > x) minX = x; else if (maxX < x) maxX = x;
      if (minY > y) minY = y; else if (maxY < y) maxY = y;

      x = src.x1 * self._00 + src.y1 * self._10;
      y = src.x1 * self._01 + src.y1 * self._11;

      if (minX > x) minX = x; else if (maxX < x) maxX = x;
      if (minY > y) minY = y; else if (maxY < y) maxY = y;

      x = src.x0 * self._00 + src.y1 * self._10;
      y = src.x0 * self._01 + src.y1 * self._11;

      if (minX > x) minX = x; else if (maxX < x) maxX = x;
      if (minY > y) minY = y; else if (maxY < y) maxY = y;

      minX += self._20;
      minY += self._21;
      maxX += self._20;
      maxY += self._21;

      dst.setBox(minX, minY, maxX, maxY);
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      NumT_(Point) pts[4];

      pts[0].set(src.x0, src.y0);
      pts[1].set(src.x1, src.y0);
      pts[2].set(src.x1, src.y1);
      pts[3].set(src.x0, src.y1);
      self._mapPoints(pts, pts, 4);

      dst.x0 = Math::min(pts[0].x, pts[1].x, pts[2].x, pts[3].x);
      dst.y0 = Math::min(pts[0].y, pts[1].y, pts[2].y, pts[3].y);

      dst.x1 = Math::max(pts[0].x, pts[1].x, pts[2].x, pts[3].x);
      dst.y1 = Math::max(pts[0].y, pts[1].y, pts[2].y, pts[3].y);
      break;
    }

    case TRANSFORM_TYPE_DEGENERATE:
    {
      dst.reset();
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::Transform - MapVector]
// ============================================================================

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapVectorT(const NumT_(Transform)& self,
  NumT_(Point)& dst,
  const SrcT_(Point)& src)
{
  uint32_t selfType = self.getType();
  NumT x, y;

  switch (selfType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    case TRANSFORM_TYPE_TRANSLATION:
      break;

    case TRANSFORM_TYPE_SCALING:
      x = src.x * self._00;
      y = src.y * self._11;
      break;

    case TRANSFORM_TYPE_SWAP:
      x = src.y * self._10;
      y = src.x * self._01;
      break;

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    case TRANSFORM_TYPE_PROJECTION:
    {
      x = src.x * self._00 + src.y * self._10;
      y = src.x * self._01 + src.y * self._11;
      if (selfType != TRANSFORM_TYPE_PROJECTION) break;

      NumT _w0 = (self._22);
      NumT _w1 = (self._22 + src.x * self._02 + src.y * self._12);

      if (Math::isFuzzyZero(_w0)) _w0 = Math2dConst<NumT>::getMathEpsilon();
      if (Math::isFuzzyZero(_w1)) _w1 = Math2dConst<NumT>::getMathEpsilon();

      _w0 = NumT(1.0) / _w0;
      _w1 = NumT(1.0) / _w1;

      x += self._20;
      y += self._21;

      x *= _w1;
      y *= _w1;

      x -= self._20 * _w0;
      y -= self._21 * _w0;
      break;
    }

    case TRANSFORM_TYPE_DEGENERATE:
      x = NumT(0.0);
      y = NumT(0.0);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  dst.set(x, y);
}

// ============================================================================
// [Fog::Transform - GetScaling]
// ============================================================================

// Notes from Anti-Grain:
//
// Used to calculate scaling coefficients in image resampling. When there is
// considerable shear this method gives us much better estimation than just
// sx, sy.

template<typename NumT>
static NumT_(Point) FOG_CDECL TransformT_getScaling(const NumT_(Transform)& self,
  bool absolute)
{
  NumT_(Point) result;

  if (absolute)
  {
    // Absolute scaling.
    result.x = Math::sqrt(self._00 * self._00 + self._10 * self._10);
    result.y = Math::sqrt(self._01 * self._01 + self._11 * self._11);
  }
  else
  {
    // Average scaling.
    NumT_(Transform) t(self.rotated(-self.getRotation(), MATRIX_ORDER_APPEND));

    result.set(NumT(1.0), NumT(1.0));
    t.mapVector(result);
  }

  return result;
}

// ============================================================================
// [Fog::Transform - GetRotation]
// ============================================================================

template<typename NumT>
static NumT FOG_CDECL TransformT_getRotation(const NumT_(Transform)& self)
{
  NumT_(Point) pt(NumT(1.0), NumT(0.0));
  self.mapVector(pt);
  return Math::atan2(pt.y, pt.x);
}

// ============================================================================
// [Fog::Transform - GetAverageScaling]
// ============================================================================

template<typename NumT>
static NumT FOG_CDECL TransformT_getAverageScaling(const NumT_(Transform)& self)
{
  NumT x = self._00 + self._10;
  NumT y = self._01 + self._11;
  return Math::sqrt((x * x + y * y) * NumT(0.5));
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_CPU_DECLARE_INITIALIZER_3DNOW(Transform_init3dNow)
FOG_CPU_DECLARE_INITIALIZER_SSE(Transform_initSSE)
FOG_CPU_DECLARE_INITIALIZER_SSE2(Transform_initSSE2)

FOG_NO_EXPORT void Transform_init(void)
{
  _api.transformf.create = TransformT_create<float>;
  _api.transformf.update = TransformT_update<float>;
  _api.transformf.transform = TransformT_transform<float>;
  _api.transformf.transformed = TransformT_transformed<float>;
  _api.transformf.multiply = TransformT_multiply<float, float>;
  _api.transformf.invert = TransformT_invert<float>;
  _api.transformf.getScaling = TransformT_getScaling<float>;
  _api.transformf.getRotation = TransformT_getRotation<float>;
  _api.transformf.getAverageScaling = TransformT_getAverageScaling<float>;

  _api.transformd.create = TransformT_create<double>;
  _api.transformd.update = TransformT_update<double>;
  _api.transformd.transform = TransformT_transform<double>;
  _api.transformd.transformed = TransformT_transformed<double>;
  _api.transformd.multiply = TransformT_multiply<double, double>;
  _api.transformd.invert = TransformT_invert<double>;
  _api.transformd.getScaling = TransformT_getScaling<double>;
  _api.transformd.getRotation = TransformT_getRotation<double>;
  _api.transformd.getAverageScaling = TransformT_getAverageScaling<double>;

  _api.transformf.mapPointF = TransformT_mapPointT<float, float>;
  _api.transformf.mapPathF = TransformT_mapPathT<float, float>;
  _api.transformf.mapPathDataF = TransformT_mapPathDataT<float, float>;
  _api.transformf.mapBoxF = TransformT_mapBoxT<float, float>;
  _api.transformf.mapVectorF = TransformT_mapVectorT<float, float>;

  _api.transformd.mapPathF = TransformT_mapPathT<double, float>;
  _api.transformd.mapPathD = TransformT_mapPathT<double, double>;
  _api.transformd.mapPathDataF = TransformT_mapPathDataT<double, float>;
  _api.transformd.mapPathDataD = TransformT_mapPathDataT<double, double>;
  _api.transformd.mapBoxD = TransformT_mapBoxT<double, double>;

#if defined(FOG_TRANSFORM_INIT_C)
  _api.transformd.mapPointD = TransformT_mapPointT<double, double>;
  _api.transformd.mapVectorD = TransformT_mapVectorT<double, double>;

  _api.transformf.mapPointsF[TRANSFORM_TYPE_IDENTITY   ] = TransformT_mapPointsT_Identity   <float, float>;
  _api.transformf.mapPointsF[TRANSFORM_TYPE_TRANSLATION] = TransformT_mapPointsT_Translation<float, float>;
  _api.transformf.mapPointsF[TRANSFORM_TYPE_SCALING    ] = TransformT_mapPointsT_Scaling    <float, float>;
  _api.transformf.mapPointsF[TRANSFORM_TYPE_SWAP       ] = TransformT_mapPointsT_Swap       <float, float>;
  _api.transformf.mapPointsF[TRANSFORM_TYPE_ROTATION   ] = TransformT_mapPointsT_Affine     <float, float>;
  _api.transformf.mapPointsF[TRANSFORM_TYPE_AFFINE     ] = TransformT_mapPointsT_Affine     <float, float>;
  _api.transformf.mapPointsF[TRANSFORM_TYPE_PROJECTION ] = TransformT_mapPointsT_Projection <float, float>;
  _api.transformf.mapPointsF[TRANSFORM_TYPE_DEGENERATE ] = TransformT_mapPointsT_Degenerate <float, float>;

  _api.transformd.mapPointsF[TRANSFORM_TYPE_IDENTITY   ] = TransformT_mapPointsT_Identity   <double, float>;
  _api.transformd.mapPointsF[TRANSFORM_TYPE_TRANSLATION] = TransformT_mapPointsT_Translation<double, float>;
  _api.transformd.mapPointsF[TRANSFORM_TYPE_SCALING    ] = TransformT_mapPointsT_Scaling    <double, float>;
  _api.transformd.mapPointsF[TRANSFORM_TYPE_SWAP       ] = TransformT_mapPointsT_Swap       <double, float>;
  _api.transformd.mapPointsF[TRANSFORM_TYPE_ROTATION   ] = TransformT_mapPointsT_Affine     <double, float>;
  _api.transformd.mapPointsF[TRANSFORM_TYPE_AFFINE     ] = TransformT_mapPointsT_Affine     <double, float>;
  _api.transformd.mapPointsF[TRANSFORM_TYPE_PROJECTION ] = TransformT_mapPointsT_Projection <double, float>;
  _api.transformd.mapPointsF[TRANSFORM_TYPE_DEGENERATE ] = TransformT_mapPointsT_Degenerate <double, float>;

  _api.transformd.mapPointsD[TRANSFORM_TYPE_IDENTITY   ] = TransformT_mapPointsT_Identity   <double, double>;
  _api.transformd.mapPointsD[TRANSFORM_TYPE_TRANSLATION] = TransformT_mapPointsT_Translation<double, double>;
  _api.transformd.mapPointsD[TRANSFORM_TYPE_SCALING    ] = TransformT_mapPointsT_Scaling    <double, double>;
  _api.transformd.mapPointsD[TRANSFORM_TYPE_SWAP       ] = TransformT_mapPointsT_Swap       <double, double>;
  _api.transformd.mapPointsD[TRANSFORM_TYPE_ROTATION   ] = TransformT_mapPointsT_Affine     <double, double>;
  _api.transformd.mapPointsD[TRANSFORM_TYPE_AFFINE     ] = TransformT_mapPointsT_Affine     <double, double>;
  _api.transformd.mapPointsD[TRANSFORM_TYPE_PROJECTION ] = TransformT_mapPointsT_Projection <double, double>;
  _api.transformd.mapPointsD[TRANSFORM_TYPE_DEGENERATE ] = TransformT_mapPointsT_Degenerate <double, double>;
#endif // FOG_TRANSFORM_INIT_C

  FOG_CPU_USE_INITIALIZER_3DNOW(Transform_init3dNow)
  FOG_CPU_USE_INITIALIZER_SSE(Transform_initSSE)
  FOG_CPU_USE_INITIALIZER_SSE2(Transform_initSSE2)
}

} // Fog namespace
