// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

// C implementation can be disabled when hardcoding for SSE2.
#if !defined(FOG_HARDCODE_SSE2)
# define FOG_TRANSFORM_INIT_C
#endif // FOG_HARDCODE_SSE2

// ============================================================================
// [Fog::Transform - Global]
// ============================================================================

static Static<TransformF> TransformF_identity;
static Static<TransformD> TransformD_identity;

// ============================================================================
// [Fog::Transform - Helpers]
// ============================================================================

template<typename NumT>
static bool TransformT_squareToQuad(NumT_(Transform)* self, const NumT_(Point)* p)
{
  NumT dx = p[0].x - p[1].x + p[2].x - p[3].x;
  NumT dy = p[0].y - p[1].y + p[2].y - p[3].y;

  if (dx == NumT(0.0) && dy == NumT(0.0))
  {
    // Affine case (parallelogram).
    self->_setData(TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY,
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
      self->_setData(TRANSFORM_TYPE_DEGENERATE,
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

      self->_setData(
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
static err_t FOG_CDECL TransformT_create(NumT_(Transform)* self,
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
      self->_setData(TRANSFORM_TYPE_IDENTITY,
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

      self->_setData(type,
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

      self->_setData(TRANSFORM_TYPE_SCALING,
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

      self->_setData(TRANSFORM_TYPE_ROTATION,
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

      self->_setData(TRANSFORM_TYPE_AFFINE,
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
      self->_setData(TRANSFORM_TYPE_AFFINE,
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

        self->_setData(TRANSFORM_TYPE_AFFINE,
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

      NumI_(Transform)::multiply(*self, sm, dm);
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

      if (!TransformT_squareToQuad<NumT>(&dm, p->dst) ||
          !TransformT_squareToQuad<NumT>(&sm, p->src) ||
          !sm.invert())
      {
        goto _CreateIdentity;
      }

      NumI_(Transform)::multiply(*self, sm, dm);
      break;
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Transform - Update]
// ============================================================================

template<typename NumT>
static uint32_t FOG_CDECL TransformT_update(const NumT_(Transform)* self)
{
  uint32_t type = self->_type;

  switch (type & (TRANSFORM_TYPE_DIRTY - 1))
  {
    // Transform is not projection if:
    //   [. . 0]
    //   [. . 0]
    //   [. . 1]
    case TRANSFORM_TYPE_DEGENERATE:
    case TRANSFORM_TYPE_PROJECTION:
      if (!Math::isFuzzyZero(self->_02) || !Math::isFuzzyZero(self->_12) || !Math::isFuzzyOne (self->_22))
      {
        type = TRANSFORM_TYPE_PROJECTION;

        NumT d = (self->_22 * self->_11 - self->_21 * self->_12) * self->_00 -
                 (self->_22 * self->_01 - self->_21 * self->_02) * self->_10 +
                 (self->_12 * self->_01 - self->_11 * self->_02) * self->_20 ;
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
      if (!Math::isFuzzyZero(self->_01) || !Math::isFuzzyZero(self->_10))
      {
        if (Math::isFuzzyZero(self->_00) && Math::isFuzzyZero(self->_11))
        {
          type = TRANSFORM_TYPE_SWAP;
        }
        else
        {
          NumT d = self->_00 * self->_11 - self->_01 * self->_10;
          NumT r = self->_00 * self->_01 + self->_10 * self->_11;

          type = TRANSFORM_TYPE_AFFINE;
          if (Math::isFuzzyZero(r))
          {
            type = TRANSFORM_TYPE_ROTATION;
            if (Math::isFuzzyZero(self->_00) && Math::isFuzzyZero(self->_11))
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
      if (!Math::isFuzzyOne(self->_00) || !Math::isFuzzyOne(self->_11))
      {
        NumT d = self->_00 * self->_11;

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
      if (!Math::isFuzzyZero(self->_20) || !Math::isFuzzyZero(self->_21))
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

  self->_type = type;
  return type;
}

// ============================================================================
// [Fog::Transform - Transform]
// ============================================================================

#define ENCODE_OP(_Op_, _Order_) ((uint32_t)(_Op_) | ((uint32_t)(_Order_) << 4))

struct TransformParams
{
  struct RotationF
  {
    float angle;
    PointF point;
  };

  struct RotationD
  {
    double angle;
    PointD point;
  };

  union
  {
    Static<PointF> pointf;
    Static<PointD> pointd;

    Static<RotationF> rotationf;
    Static<RotationD> rotationd;

    Static<TransformF> transformf;
    Static<TransformD> transformd;
  };
};

static err_t FOG_CDECL TransformF_transform(TransformF* self, uint32_t opType, const void* _params)
{
  uint32_t selfType = self->getType();
  const TransformParams* params = reinterpret_cast<const TransformParams*>(_params);

  // Initialize to NULL so invalid access (our bug) can be detected easily.
  const TransformF* srcf = NULL;
  const TransformD* srcd = NULL;

  TransformF srcfTmp(UNINITIALIZED);
  TransformD srcdTmp(UNINITIALIZED);

  switch (opType)
  {
    // ------------------------------------------------------------------------
    // [Translate]
    // ------------------------------------------------------------------------

    //     |1 0 0|
    // M = |0 1 0| * M
    //     |X Y 1|

    case ENCODE_OP(TRANSFORM_OP_TRANSLATEF, MATRIX_ORDER_PREPEND):
    {
      float x = params->pointf().x;
      float y = params->pointf().y;

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          self->_type = TRANSFORM_TYPE_TRANSLATION;
          // ... Fall through ...

        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 += x;
          self->_21 += y;
          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SCALING:
          self->_20 += x * self->_00;
          self->_21 += y * self->_11;
          break;

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
          self->_22 += x * self->_02 + y * self->_12;
          // ... Fall through ...

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
          self->_20 += x * self->_00 + y * self->_10;
          self->_21 += x * self->_01 + y * self->_11;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_TRANSLATED, MATRIX_ORDER_PREPEND):
    {
      double x = params->pointd().x;
      double y = params->pointd().y;

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          self->_type = TRANSFORM_TYPE_TRANSLATION;
          // ... Fall through ...

        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 = float(x + double(self->_20));
          self->_21 = float(y + double(self->_21));
          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SCALING:
          self->_20 = float(x * double(self->_00) + double(self->_20));
          self->_21 = float(y * double(self->_11) + double(self->_21));
          break;

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
          self->_22 = float(x * double(self->_02) + y * double(self->_12) + double(self->_22));
          // ... Fall through ...

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
          self->_20 = float(x * double(self->_00) + y * double(self->_10) + double(self->_20));
          self->_21 = float(x * double(self->_01) + y * double(self->_11) + double(self->_21));
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    //         |1 0 0|
    // M = M * |0 1 0|
    //         |X Y 1|

    case ENCODE_OP(TRANSFORM_OP_TRANSLATEF, MATRIX_ORDER_APPEND):
    {
      float x = params->pointf().x;
      float y = params->pointf().y;

      self->_20 += x;
      self->_21 += y;

      self->_type = Math::max<uint32_t>(
        self->_type                 | TRANSFORM_TYPE_DIRTY,
        TRANSFORM_TYPE_TRANSLATION | TRANSFORM_TYPE_DIRTY);
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_TRANSLATED, MATRIX_ORDER_APPEND):
    {
      double x = params->pointd().x;
      double y = params->pointd().y;

      self->_20 = float(x + double(self->_20));
      self->_21 = float(y + double(self->_21));

      self->_type = Math::max<uint32_t>(
        self->_type                 | TRANSFORM_TYPE_DIRTY,
        TRANSFORM_TYPE_TRANSLATION | TRANSFORM_TYPE_DIRTY);
      break;
    }

    // ------------------------------------------------------------------------
    // [Scale]
    // ------------------------------------------------------------------------

    //     [X 0 0]
    // M = [0 Y 0] * M
    //     [0 0 1]

    case ENCODE_OP(TRANSFORM_OP_SCALEF, MATRIX_ORDER_PREPEND):
    {
      float x = params->pointf().x;
      float y = params->pointf().y;

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
          self->_00 = x;
          self->_11 = y;
          self->_type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
          self->_02 *= x;
          self->_12 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
          self->_01 *= x;
          self->_10 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_SCALING:
          self->_00 *= x;
          self->_11 *= y;

          self->_type = Math::max<uint32_t>(
            self->_type             | TRANSFORM_TYPE_DIRTY,
            TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_SCALED, MATRIX_ORDER_PREPEND):
    {
      double x = params->pointd().x;
      double y = params->pointd().y;

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
          self->_00 = float(x);
          self->_11 = float(y);
          self->_type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
          self->_02 = float(x * double(self->_02));
          self->_12 = float(y * double(self->_12));
          // ... Fall through ...

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
          self->_01 = float(x * double(self->_01));
          self->_10 = float(y * double(self->_10));
          // ... Fall through ...

        case TRANSFORM_TYPE_SCALING:
          self->_00 = float(x * double(self->_00));
          self->_11 = float(y * double(self->_11));

          self->_type = Math::max<uint32_t>(
            self->_type             | TRANSFORM_TYPE_DIRTY,
            TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    //         [X 0 0]
    // M = M * [0 Y 0]
    //         [0 0 1]

    case ENCODE_OP(TRANSFORM_OP_SCALEF, MATRIX_ORDER_APPEND):
    {
      float x = params->pointf().x;
      float y = params->pointf().y;

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 *= x;
          self->_21 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_IDENTITY:
          self->_00 = x;
          self->_11 = y;
          self->_type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
          self->_02 *= x;
          self->_12 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
          self->_01 *= x;
          self->_10 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_SCALING:
          self->_00 *= x;
          self->_11 *= y;
          self->_20 *= x;
          self->_21 *= y;

          self->_type = Math::max<uint32_t>(
            self->_type             | TRANSFORM_TYPE_DIRTY,
            TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_SCALED, MATRIX_ORDER_APPEND):
    {
      double x = params->pointd().x;
      double y = params->pointd().y;

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 = float(x * double(self->_20));
          self->_21 = float(y * double(self->_21));
          // ... Fall through ...

        case TRANSFORM_TYPE_IDENTITY:
          self->_00 = float(x);
          self->_11 = float(y);
          self->_type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
          self->_02 = float(x * double(self->_02));
          self->_12 = float(y * double(self->_12));
          // ... Fall through ...

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
          self->_01 = float(x * double(self->_01));
          self->_10 = float(y * double(self->_10));
          // ... Fall through ...

        case TRANSFORM_TYPE_SCALING:
          self->_00 = float(x * double(self->_00));
          self->_11 = float(y * double(self->_11));
          self->_20 = float(x * double(self->_20));
          self->_21 = float(y * double(self->_21));

          self->_type = Math::max<uint32_t>(
            self->_type             | TRANSFORM_TYPE_DIRTY,
            TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Skew]
    // ------------------------------------------------------------------------

    //     [  1    tan(y) 0]
    // M = [tan(x)   1    0] * M
    //     [  0      0    1]

    case ENCODE_OP(TRANSFORM_OP_SKEWF, MATRIX_ORDER_PREPEND):
    {
      float x = params->pointf().x;
      float y = params->pointf().y;

      float xTan = Math::tan(x);
      float yTan = xTan;

      if (x != y)
        yTan = Math::tan(y);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
        case TRANSFORM_TYPE_SCALING:
          self->_01 = yTan * self->_11;
          self->_10 = xTan * self->_00;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
          self->_type = TRANSFORM_TYPE_AFFINE;
          // ... Fall through ...

        case TRANSFORM_TYPE_AFFINE:
        {
_SkewPrependAffineF:
          float t00 = yTan * self->_10;
          float t01 = yTan * self->_11;

          self->_10 += xTan * self->_00;
          self->_11 += xTan * self->_01;

          self->_00 += t00;
          self->_01 += t01;

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          float t02 = yTan * self->_12;

          self->_12 += xTan * self->_02;
          self->_02 += t02;

          goto _SkewPrependAffineF;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_SKEWD, MATRIX_ORDER_PREPEND):
    {
      double x = params->pointd().x;
      double y = params->pointd().y;

      double xTan = Math::tan(x);
      double yTan = xTan;

      if (x != y)
        yTan = Math::tan(y);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
        case TRANSFORM_TYPE_SCALING:
          self->_01 = float(yTan * double(self->_11));
          self->_10 = float(xTan * double(self->_00));

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
          self->_type = TRANSFORM_TYPE_AFFINE;
          // ... Fall through ...

        case TRANSFORM_TYPE_AFFINE:
        {
_SkewPrependAffineD:
          double t00 = yTan * double(self->_10);
          double t01 = yTan * double(self->_11);

          self->_10 = float(xTan * double(self->_00) + double(self->_10));
          self->_11 = float(xTan * double(self->_01) + double(self->_11));

          self->_00 = float(t00 + double(self->_00));
          self->_01 = float(t01 + double(self->_01));

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          double t02 = yTan * double(self->_12);

          self->_12 = float(xTan * double(self->_02) + double(self->_12));
          self->_02 = float(t02 + double(self->_02));

          goto _SkewPrependAffineD;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    //         [  1    tan(y) 0]
    // M = M * [tan(x)   1    0]
    //         [  0      0    1]

    case ENCODE_OP(TRANSFORM_OP_SKEWF, MATRIX_ORDER_APPEND):
    {
      float x = params->pointf().x;
      float y = params->pointf().y;

      float xTan = Math::tan(x);
      float yTan = xTan;

      if (x != y)
        yTan = Math::tan(y);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          self->_01 = yTan;
          self->_10 = xTan;
          // ... Fall through ...

        case TRANSFORM_TYPE_TRANSLATION:
        {
          float t20 = self->_21 * xTan;
          float t21 = self->_20 * yTan;

          self->_20 += t20;
          self->_21 += t21;

          self->_type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          float t10 = self->_11 * xTan;
          float t20 = self->_21 * xTan;

          self->_01 += self->_00 * yTan;
          self->_21 += self->_20 * yTan;

          self->_10 += t10;
          self->_20 += t20;

          self->_type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
          self->_type = TRANSFORM_TYPE_AFFINE;
          // ... Fall through ...

        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          float t00 = self->_01 * xTan;
          float t10 = self->_11 * xTan;
          float t20 = self->_21 * xTan;

          self->_01 += self->_00 * yTan;
          self->_11 += self->_10 * yTan;
          self->_21 += self->_20 * yTan;

          self->_00 += t00;
          self->_10 += t10;
          self->_20 += t20;

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_SKEWD, MATRIX_ORDER_APPEND):
    {
      double x = params->pointd().x;
      double y = params->pointd().y;

      double xTan = Math::tan(x);
      double yTan = xTan;

      if (x != y)
        yTan = Math::tan(y);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          self->_01 = float(yTan);
          self->_10 = float(xTan);
          // ... Fall through ...

        case TRANSFORM_TYPE_TRANSLATION:
        {
          double t20 = double(self->_21) * xTan;
          double t21 = double(self->_20) * yTan;

          self->_20 = float(t20 + double(self->_20));
          self->_21 = float(t21 + double(self->_21));

          self->_type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          double t10 = double(self->_11) * xTan;
          double t20 = double(self->_21) * xTan;

          self->_01 = float(double(self->_00) * yTan + double(self->_01));
          self->_21 = float(double(self->_20) * yTan + double(self->_21));

          self->_10 = float(t10 + double(self->_10));
          self->_20 = float(t20 + double(self->_20));

          self->_type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
          self->_type = TRANSFORM_TYPE_AFFINE;
          // ... Fall through ...

        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          double t00 = double(self->_01) * xTan;
          double t10 = double(self->_11) * xTan;
          double t20 = double(self->_21) * xTan;

          self->_01 = float(double(self->_00) * yTan + double(self->_01));
          self->_11 = float(double(self->_10) * yTan + double(self->_11));
          self->_21 = float(double(self->_20) * yTan + double(self->_21));

          self->_00 = float(t00 + double(self->_00));
          self->_10 = float(t10 + double(self->_10));
          self->_20 = float(t20 + double(self->_20));

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Rotate]
    // ------------------------------------------------------------------------

    //     [ cos(a) sin(a) 0]
    // M = [-sin(a) cos(a) 0] * M
    //     [   0      0    1]

    case ENCODE_OP(TRANSFORM_OP_ROTATEF, MATRIX_ORDER_PREPEND):
    {
      float angle = params->rotationf().angle;

      float as, ac;
      Math::sincos(angle, &as, &ac);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
          self->_00 = ac;
          self->_01 = as;
          self->_10 =-as;
          self->_11 = ac;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          float m00 = self->_00;
          float m11 = self->_11;

          self->_00 = m00 * ac;
          self->_01 = m11 * as;
          self->_10 = m00 *-as;
          self->_11 = m11 * ac;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          float m00 = self->_00;
          float m01 = self->_01;
          float m10 = self->_10;
          float m11 = self->_11;

          self->_00 = m00 * ac + m10 * as;
          self->_01 = m01 * ac + m11 * as;
          self->_10 = m00 *-as + m10 * ac;
          self->_11 = m01 *-as + m11 * ac;

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          float t00 = ac * self->_00 + as * self->_10;
          float t01 = ac * self->_01 + as * self->_11;
          float t02 = ac * self->_02 + as * self->_12;

          self->_10 = -as * self->_00 + ac * self->_10;
          self->_11 = -as * self->_01 + ac * self->_11;
          self->_12 = -as * self->_02 + ac * self->_12;

          self->_00 = t00;
          self->_01 = t01;
          self->_02 = t02;

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_ROTATED, MATRIX_ORDER_PREPEND):
    {
      double angle = params->rotationd().angle;

      double as, ac;
      Math::sincos(angle, &as, &ac);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
          self->_00 = float(ac);
          self->_01 = float(as);
          self->_10 =-float(as);
          self->_11 = float(ac);

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          double m00 = double(self->_00);
          double m11 = double(self->_11);

          self->_00 = float(m00 * ac);
          self->_01 = float(m11 * as);
          self->_10 = float(m00 *-as);
          self->_11 = float(m11 * ac);

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          double m00 = double(self->_00);
          double m01 = double(self->_01);
          double m10 = double(self->_10);
          double m11 = double(self->_11);

          self->_00 = float(m00 * ac + m10 * as);
          self->_01 = float(m01 * ac + m11 * as);
          self->_10 = float(m00 *-as + m10 * ac);
          self->_11 = float(m01 *-as + m11 * ac);

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = ac * double(self->_00) + as * double(self->_10);
          double t01 = ac * double(self->_01) + as * double(self->_11);
          double t02 = ac * double(self->_02) + as * double(self->_12);

          self->_10 = float(-as * double(self->_00) + ac * double(self->_10));
          self->_11 = float(-as * double(self->_01) + ac * double(self->_11));
          self->_12 = float(-as * double(self->_02) + ac * double(self->_12));

          self->_00 = float(t00);
          self->_01 = float(t01);
          self->_02 = float(t02);

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    //         [ cos(a) sin(a) 0]
    // M = M * [-sin(a) cos(a) 0]
    //         [   0      0    1]

    case ENCODE_OP(TRANSFORM_OP_ROTATEF, MATRIX_ORDER_APPEND):
    {
      float angle = params->rotationf().angle;

      float as, ac;
      Math::sincos(angle, &as, &ac);

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
        {
          float t20 = self->_20 * ac - self->_21 * as;
          float t21 = self->_20 * as + self->_21 * ac;

          self->_20 = t20;
          self->_21 = t21;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_IDENTITY:
          self->_00 = ac;
          self->_01 = as;
          self->_10 =-as;
          self->_11 = ac;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          float t00 = self->_00 * ac;
          float t10 = self->_11 *-as;
          float t20 = self->_20 * ac + self->_21 *-as;

          self->_01 = self->_00 * as;
          self->_11 = self->_11 * ac;
          self->_21 = self->_20 * as + self->_21 * ac;

          self->_00 = t00;
          self->_10 = t10;
          self->_20 = t20;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          float t00 = self->_00 * ac - self->_01 * as;
          float t10 = self->_10 * ac - self->_11 * as;
          float t20 = self->_20 * ac - self->_21 * as;

          self->_01 = self->_00 * as + self->_01 * ac;
          self->_11 = self->_10 * as + self->_11 * ac;
          self->_21 = self->_20 * as + self->_21 * ac;

          self->_00 = t00;
          self->_10 = t10;
          self->_20 = t20;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_ROTATED, MATRIX_ORDER_APPEND):
    {
      double angle = float(params->rotationd().angle);

      double as, ac;
      Math::sincos(angle, &as, &ac);

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
        {
          double t20 = double(self->_20) * ac - double(self->_21) * as;
          double t21 = double(self->_20) * as + double(self->_21) * ac;

          self->_20 = float(t20);
          self->_21 = float(t21);
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_IDENTITY:
          self->_00 = float(ac);
          self->_01 = float(as);
          self->_10 =-float(as);
          self->_11 = float(ac);

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          double t00 = double(self->_00) * ac;
          double t10 = double(self->_11) *-as;
          double t20 = double(self->_20) * ac + double(self->_21) *-as;

          self->_01 = float(double(self->_00) * as);
          self->_11 = float(double(self->_11) * ac);
          self->_21 = float(double(self->_20) * as + double(self->_21) * ac);

          self->_00 = float(t00);
          self->_10 = float(t10);
          self->_20 = float(t20);

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          double t00 = double(self->_00) * ac - double(self->_01) * as;
          double t10 = double(self->_10) * ac - double(self->_11) * as;
          double t20 = double(self->_20) * ac - double(self->_21) * as;

          self->_01 = float(self->_00 * as + double(self->_01) * ac);
          self->_11 = float(self->_10 * as + double(self->_11) * ac);
          self->_21 = float(self->_20 * as + double(self->_21) * ac);

          self->_00 = float(t00);
          self->_10 = float(t10);
          self->_20 = float(t20);
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }

    // ------------------------------------------------------------------------
    // [Multiply]
    // ------------------------------------------------------------------------

    //     [S00 S01 S02]
    // M = [S10 S11 S12] * M
    //     [S20 S21 S22]

    case ENCODE_OP(TRANSFORM_OP_MULTIPLYF, MATRIX_ORDER_PREPEND):
    {
      srcf = &params->transformf();

      uint32_t sType = srcf->getType();
      uint32_t dType = Math::max(selfType, sType);

      self->_type = dType | TRANSFORM_TYPE_DIRTY;
      switch (dType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          break;

        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 += srcf->_20;
          self->_21 += srcf->_21;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          float t20 = srcf->_20 * self->_00;
          float t21 = srcf->_21 * self->_11;

          self->_00 *= srcf->_00;
          self->_11 *= srcf->_11;

          self->_20 += t20;
          self->_21 += t21;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          float t00 = srcf->_00 * self->_00 + srcf->_01 * self->_10;
          float t01 = srcf->_00 * self->_01 + srcf->_01 * self->_11;

          float t10 = srcf->_10 * self->_00 + srcf->_11 * self->_10;
          float t11 = srcf->_10 * self->_01 + srcf->_11 * self->_11;

          float t20 = srcf->_20 * self->_00 + srcf->_21 * self->_10;
          float t21 = srcf->_20 * self->_01 + srcf->_21 * self->_11;

          self->_00 = t00;
          self->_01 = t01;
          self->_10 = t10;
          self->_11 = t11;

          self->_20 += t20;
          self->_21 += t21;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          float t00 = srcf->_00 * self->_00 + srcf->_01 * self->_10 + srcf->_02 * self->_20;
          float t01 = srcf->_00 * self->_01 + srcf->_01 * self->_11 + srcf->_02 * self->_21;
          float t02 = srcf->_00 * self->_02 + srcf->_01 * self->_12 + srcf->_02 * self->_22;

          float t10 = srcf->_10 * self->_00 + srcf->_11 * self->_10 + srcf->_12 * self->_20;
          float t11 = srcf->_10 * self->_01 + srcf->_11 * self->_11 + srcf->_12 * self->_21;
          float t12 = srcf->_10 * self->_02 + srcf->_11 * self->_12 + srcf->_12 * self->_22;

          float t20 = srcf->_20 * self->_00 + srcf->_21 * self->_10 + srcf->_22 * self->_20;
          float t21 = srcf->_20 * self->_01 + srcf->_21 * self->_11 + srcf->_22 * self->_21;
          float t22 = srcf->_20 * self->_02 + srcf->_21 * self->_12 + srcf->_22 * self->_22;

          self->_00 = t00;
          self->_01 = t01;
          self->_02 = t02;

          self->_10 = t10;
          self->_11 = t11;
          self->_12 = t12;

          self->_20 = t20;
          self->_21 = t21;
          self->_22 = t22;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_MULTIPLYD, MATRIX_ORDER_PREPEND):
    {
      srcd = &params->transformd();

_MultiplyPrependD:
      uint32_t sType = srcd->getType();
      uint32_t dType = Math::max(selfType, sType);

      self->_type = dType | TRANSFORM_TYPE_DIRTY;
      switch (dType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          break;

        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 = float(srcd->_20 + double(self->_20));
          self->_21 = float(srcd->_21 + double(self->_21));
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          double t20 = srcd->_20 * double(self->_00);
          double t21 = srcd->_21 * double(self->_11);

          self->_00 = float(srcd->_00 * double(self->_00));
          self->_11 = float(srcd->_11 * double(self->_11));

          self->_20 = float(t20 + double(self->_20));
          self->_21 = float(t21 + double(self->_20));
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          double t00 = srcd->_00 * double(self->_00) + srcd->_01 * double(self->_10);
          double t01 = srcd->_00 * double(self->_01) + srcd->_01 * double(self->_11);

          double t10 = srcd->_10 * double(self->_00) + srcd->_11 * double(self->_10);
          double t11 = srcd->_10 * double(self->_01) + srcd->_11 * double(self->_11);

          double t20 = srcd->_20 * double(self->_00) + srcd->_21 * double(self->_10);
          double t21 = srcd->_20 * double(self->_01) + srcd->_21 * double(self->_11);

          self->_00 = float(t00);
          self->_01 = float(t01);
          self->_10 = float(t10);
          self->_11 = float(t11);

          self->_20 = float(t20 + double(self->_20));
          self->_21 = float(t21 + double(self->_20));
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = srcd->_00 * double(self->_00) + srcd->_01 * double(self->_10) + srcd->_02 * double(self->_20);
          double t01 = srcd->_00 * double(self->_01) + srcd->_01 * double(self->_11) + srcd->_02 * double(self->_21);
          double t02 = srcd->_00 * double(self->_02) + srcd->_01 * double(self->_12) + srcd->_02 * double(self->_22);

          double t10 = srcd->_10 * double(self->_00) + srcd->_11 * double(self->_10) + srcd->_12 * double(self->_20);
          double t11 = srcd->_10 * double(self->_01) + srcd->_11 * double(self->_11) + srcd->_12 * double(self->_21);
          double t12 = srcd->_10 * double(self->_02) + srcd->_11 * double(self->_12) + srcd->_12 * double(self->_22);

          double t20 = srcd->_20 * double(self->_00) + srcd->_21 * double(self->_10) + srcd->_22 * double(self->_20);
          double t21 = srcd->_20 * double(self->_01) + srcd->_21 * double(self->_11) + srcd->_22 * double(self->_21);
          double t22 = srcd->_20 * double(self->_02) + srcd->_21 * double(self->_12) + srcd->_22 * double(self->_22);

          self->_00 = float(t00);
          self->_01 = float(t01);
          self->_02 = float(t02);

          self->_10 = float(t10);
          self->_11 = float(t11);
          self->_12 = float(t12);

          self->_20 = float(t20);
          self->_21 = float(t21);
          self->_22 = float(t22);
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    //         |S00 S01 S02|
    // M = M * |S10 S11 S12|
    //         |S20 S21 S22|

    case ENCODE_OP(TRANSFORM_OP_MULTIPLYF, MATRIX_ORDER_APPEND):
    {
      srcf = &params->transformf();

      uint32_t sType = srcf->getType();
      uint32_t dType = Math::max(selfType, sType);

      self->_type = dType | TRANSFORM_TYPE_DIRTY;
      switch (dType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          break;

        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 += srcf->_20;
          self->_21 += srcf->_21;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          float t00 = self->_00 * srcf->_00;
          float t11 = self->_11 * srcf->_11;

          float t20 = self->_20 * srcf->_00 + srcf->_20;
          float t21 = self->_21 * srcf->_11 + srcf->_21;

          self->_00 = t00;
          self->_11 = t11;

          self->_20 = t20;
          self->_21 = t21;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          float t00 = self->_00 * srcf->_00 + self->_01 * srcf->_10;
          float t01 = self->_00 * srcf->_01 + self->_01 * srcf->_11;

          float t10 = self->_10 * srcf->_00 + self->_11 * srcf->_10;
          float t11 = self->_10 * srcf->_01 + self->_11 * srcf->_11;

          float t20 = self->_20 * srcf->_00 + self->_21 * srcf->_10 + srcf->_20;
          float t21 = self->_20 * srcf->_01 + self->_21 * srcf->_11 + srcf->_21;

          self->_00 = t00;
          self->_01 = t01;

          self->_10 = t10;
          self->_11 = t11;

          self->_20 = t20;
          self->_21 = t21;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          float t00 = self->_00 * srcf->_00 + self->_01 * srcf->_10 + self->_02 * srcf->_20;
          float t01 = self->_00 * srcf->_01 + self->_01 * srcf->_11 + self->_02 * srcf->_21;
          float t02 = self->_00 * srcf->_02 + self->_01 * srcf->_12 + self->_02 * srcf->_22;

          float t10 = self->_10 * srcf->_00 + self->_11 * srcf->_10 + self->_12 * srcf->_20;
          float t11 = self->_10 * srcf->_01 + self->_11 * srcf->_11 + self->_12 * srcf->_21;
          float t12 = self->_10 * srcf->_02 + self->_11 * srcf->_12 + self->_12 * srcf->_22;

          float t20 = self->_20 * srcf->_00 + self->_21 * srcf->_10 + self->_22 * srcf->_20;
          float t21 = self->_20 * srcf->_01 + self->_21 * srcf->_11 + self->_22 * srcf->_21;
          float t22 = self->_20 * srcf->_02 + self->_21 * srcf->_12 + self->_22 * srcf->_22;

          self->_00 = t00;
          self->_01 = t01;
          self->_02 = t02;

          self->_10 = t10;
          self->_11 = t11;
          self->_12 = t12;

          self->_20 = t20;
          self->_21 = t21;
          self->_22 = t22;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    case ENCODE_OP(TRANSFORM_OP_MULTIPLYD, MATRIX_ORDER_APPEND):
    {
      srcd = &params->transformd();

_MultiplyAppendD:
      uint32_t sType = srcd->getType();
      uint32_t dType = Math::max(selfType, sType);

      self->_type = dType | TRANSFORM_TYPE_DIRTY;
      switch (dType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          break;

        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 = float(srcd->_20 + double(self->_20));
          self->_21 = float(srcd->_21 + double(self->_21));
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          double t00 = double(self->_00) * srcd->_00;
          double t11 = double(self->_11) * srcd->_11;

          double t20 = double(self->_20) * srcd->_00 + srcd->_20;
          double t21 = double(self->_21) * srcd->_11 + srcd->_21;

          self->_00 = float(t00);
          self->_11 = float(t11);

          self->_20 = float(t20);
          self->_21 = float(t21);
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          double t00 = double(self->_00) * srcd->_00 + double(self->_01) * srcd->_10;
          double t01 = double(self->_00) * srcd->_01 + double(self->_01) * srcd->_11;

          double t10 = double(self->_10) * srcd->_00 + double(self->_11) * srcd->_10;
          double t11 = double(self->_10) * srcd->_01 + double(self->_11) * srcd->_11;

          double t20 = double(self->_20) * srcd->_00 + double(self->_21) * srcd->_10 + srcd->_20;
          double t21 = double(self->_20) * srcd->_01 + double(self->_21) * srcd->_11 + srcd->_21;

          self->_00 = float(t00);
          self->_01 = float(t01);

          self->_10 = float(t10);
          self->_11 = float(t11);

          self->_20 = float(t20);
          self->_21 = float(t21);
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = double(self->_00) * srcd->_00 + double(self->_01) * srcd->_10 + double(self->_02) * srcd->_20;
          double t01 = double(self->_00) * srcd->_01 + double(self->_01) * srcd->_11 + double(self->_02) * srcd->_21;
          double t02 = double(self->_00) * srcd->_02 + double(self->_01) * srcd->_12 + double(self->_02) * srcd->_22;

          double t10 = double(self->_10) * srcd->_00 + double(self->_11) * srcd->_10 + double(self->_12) * srcd->_20;
          double t11 = double(self->_10) * srcd->_01 + double(self->_11) * srcd->_11 + double(self->_12) * srcd->_21;
          double t12 = double(self->_10) * srcd->_02 + double(self->_11) * srcd->_12 + double(self->_12) * srcd->_22;

          double t20 = double(self->_20) * srcd->_00 + double(self->_21) * srcd->_10 + double(self->_22) * srcd->_20;
          double t21 = double(self->_20) * srcd->_01 + double(self->_21) * srcd->_11 + double(self->_22) * srcd->_21;
          double t22 = double(self->_20) * srcd->_02 + double(self->_21) * srcd->_12 + double(self->_22) * srcd->_22;

          self->_00 = float(t00);
          self->_01 = float(t01);
          self->_02 = float(t02);

          self->_10 = float(t10);
          self->_11 = float(t11);
          self->_12 = float(t12);

          self->_20 = float(t20);
          self->_21 = float(t21);
          self->_22 = float(t22);
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Multiply Inverted]
    // ------------------------------------------------------------------------

    //        |S00 S01 S02|
    // M = Inv|S10 S11 S12| * M
    //        |S20 S21 S22|

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INVF, MATRIX_ORDER_PREPEND):
      srcdTmp = params->transformf();
      if (!srcdTmp.invert())
        return ERR_GEOMETRY_DEGENERATE;

      srcd = &srcdTmp;
      goto _MultiplyPrependD;

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INVD, MATRIX_ORDER_PREPEND):
      if (!TransformD::invert(srcdTmp, params->transformd()))
        return ERR_GEOMETRY_DEGENERATE;

      srcd = &srcdTmp;
      goto _MultiplyPrependD;

    //            |S00 S01 S02|
    // M = M * Inv|S10 S11 S12|
    //            |S20 S21 S22|

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INVF, MATRIX_ORDER_APPEND):
      srcdTmp = params->transformf();
      if (!srcdTmp.invert())
        return ERR_GEOMETRY_DEGENERATE;

      srcd = &srcdTmp;
      goto _MultiplyAppendD;

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INVD, MATRIX_ORDER_APPEND):
      if (!TransformD::invert(srcdTmp, params->transformd()))
        return ERR_GEOMETRY_DEGENERATE;

      srcd = &srcdTmp;
      goto _MultiplyAppendD;

    // ------------------------------------------------------------------------
    // [Flip]
    // ------------------------------------------------------------------------

    case ENCODE_OP(TRANSFORM_OP_FLIP, MATRIX_ORDER_PREPEND):
    case ENCODE_OP(TRANSFORM_OP_FLIP, MATRIX_ORDER_APPEND):
    {
      uint32_t axis = reinterpret_cast<const uint32_t*>(_params)[0];

      if (axis & AXIS_X) { self->_00 = -self->_00; self->_01 = -self->_01; self->_02 = -self->_02; }
      if (axis & AXIS_Y) { self->_10 = -self->_10; self->_11 = -self->_11; self->_12 = -self->_12; }
      if (axis & AXIS_Z) { self->_20 = -self->_20; self->_21 = -self->_21; self->_22 = -self->_22; }

      self->_type = Math::max<uint32_t>(selfType, TRANSFORM_TYPE_SCALING) | TRANSFORM_TYPE_DIRTY;
      break;
    }

    // ------------------------------------------------------------------------
    // [End]
    // ------------------------------------------------------------------------

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_OK;
}

static err_t FOG_CDECL TransformD_transform(TransformD* self, uint32_t opType, const void* _params)
{
  uint32_t selfType = self->getType();

  const TransformParams* params = reinterpret_cast<const TransformParams*>(_params);
  const TransformD* src;
  TransformD srcTmp(UNINITIALIZED);

  // Helper variables used across the transform operators inside switch().
  double angle;
  double x, y;

  switch (opType)
  {
    // ------------------------------------------------------------------------
    // [Translate]
    // ------------------------------------------------------------------------

    //     |1 0 0|
    // M = |0 1 0| * M
    //     |X Y 1|

    case ENCODE_OP(TRANSFORM_OP_TRANSLATEF, MATRIX_ORDER_PREPEND):
      x = double(params->pointf().x);
      y = double(params->pointf().y);
      goto _TranslatePrepend;

    case ENCODE_OP(TRANSFORM_OP_TRANSLATED, MATRIX_ORDER_PREPEND):
      x = params->pointd().x;
      y = params->pointd().y;

_TranslatePrepend:
      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          self->_type = TRANSFORM_TYPE_TRANSLATION;
          // ... Fall through ...

        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 += x;
          self->_21 += y;
          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SCALING:
          self->_20 += x * self->_00;
          self->_21 += y * self->_11;
          break;

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
          self->_22 += x * self->_02 + y * self->_12;
          // ... Fall through ...

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
          self->_20 += x * self->_00 + y * self->_10;
          self->_21 += x * self->_01 + y * self->_11;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;

    //         |1 0 0|
    // M = M * |0 1 0|
    //         |X Y 1|

    case ENCODE_OP(TRANSFORM_OP_TRANSLATEF, MATRIX_ORDER_APPEND):
      x = double(params->pointf().x);
      y = double(params->pointf().y);

    case ENCODE_OP(TRANSFORM_OP_TRANSLATED, MATRIX_ORDER_APPEND):
      x = params->pointd().x;
      y = params->pointd().y;

      self->_20 += x;
      self->_21 += y;

      self->_type = Math::max<uint32_t>(
        self->_type                 | TRANSFORM_TYPE_DIRTY,
        TRANSFORM_TYPE_TRANSLATION | TRANSFORM_TYPE_DIRTY);
      break;

    // ------------------------------------------------------------------------
    // [Scale]
    // ------------------------------------------------------------------------

    //     [X 0 0]
    // M = [0 Y 0] * M
    //     [0 0 1]

    case ENCODE_OP(TRANSFORM_OP_SCALEF, MATRIX_ORDER_PREPEND):
      x = double(params->pointf().x);
      y = double(params->pointf().y);
      goto _ScalePrepend;

    case ENCODE_OP(TRANSFORM_OP_SCALED, MATRIX_ORDER_PREPEND):
      x = params->pointd().x;
      y = params->pointd().y;

_ScalePrepend:
      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
          self->_00 = x;
          self->_11 = y;
          self->_type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
          self->_02 *= x;
          self->_12 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
          self->_01 *= x;
          self->_10 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_SCALING:
          self->_00 *= x;
          self->_11 *= y;

          self->_type = Math::max<uint32_t>(
            self->_type             | TRANSFORM_TYPE_DIRTY,
            TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;

    //         [X 0 0]
    // M = M * [0 Y 0]
    //         [0 0 1]

    case ENCODE_OP(TRANSFORM_OP_SCALEF, MATRIX_ORDER_APPEND):
      x = double(params->pointf().x);
      y = double(params->pointf().y);
      goto _ScaleAppend;

    case ENCODE_OP(TRANSFORM_OP_SCALED, MATRIX_ORDER_APPEND):
      x = params->pointd().x;
      y = params->pointd().y;

_ScaleAppend:
      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 *= x;
          self->_21 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_IDENTITY:
          self->_00 = x;
          self->_11 = y;
          self->_type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
          self->_02 *= x;
          self->_12 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
          self->_01 *= x;
          self->_10 *= y;
          // ... Fall through ...

        case TRANSFORM_TYPE_SCALING:
          self->_00 *= x;
          self->_11 *= y;
          self->_20 *= x;
          self->_21 *= y;
          self->_type = Math::max<uint32_t>(
            self->_type             | TRANSFORM_TYPE_DIRTY,
            TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;

    // ------------------------------------------------------------------------
    // [Skew]
    // ------------------------------------------------------------------------

    //     [  1    tan(y) 0]
    // M = [tan(x)   1    0] * M
    //     [  0      0    1]

    case ENCODE_OP(TRANSFORM_OP_SKEWF, MATRIX_ORDER_PREPEND):
      x = double(params->pointf().x);
      y = double(params->pointf().y);
      goto _SkewPrepend;

    case ENCODE_OP(TRANSFORM_OP_SKEWD, MATRIX_ORDER_PREPEND):
    {
      x = params->pointd().x;
      y = params->pointd().y;

_SkewPrepend:
      double xTan = Math::tan(x);
      double yTan = xTan;

      if (x != y)
        yTan = Math::tan(y);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
        case TRANSFORM_TYPE_SCALING:
          self->_01 = yTan * self->_11;
          self->_10 = xTan * self->_00;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
          self->_type = TRANSFORM_TYPE_AFFINE;
          // ... Fall through ...

        case TRANSFORM_TYPE_AFFINE:
        {
_SkewPrependAffine:
          double t00 = yTan * self->_10;
          double t01 = yTan * self->_11;

          self->_10 += xTan * self->_00;
          self->_11 += xTan * self->_01;

          self->_00 += t00;
          self->_01 += t01;

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          double t02 = yTan * self->_12;

          self->_12 += xTan * self->_02;
          self->_02 += t02;

          goto _SkewPrependAffine;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    //         [  1    tan(y) 0]
    // M = M * [tan(x)   1    0]
    //         [  0      0    1]

    case ENCODE_OP(TRANSFORM_OP_SKEWF, MATRIX_ORDER_APPEND):
      x = double(params->pointf().x);
      y = double(params->pointf().y);
      goto _SkewAppend;

    case ENCODE_OP(TRANSFORM_OP_SKEWD, MATRIX_ORDER_APPEND):
    {
      x = params->pointd().x;
      y = params->pointd().y;

_SkewAppend:
      double xTan = Math::tan(x);
      double yTan = xTan;

      if (x != y)
        yTan = Math::tan(y);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          self->_01 = yTan;
          self->_10 = xTan;
          // ... Fall through ...

        case TRANSFORM_TYPE_TRANSLATION:
        {
          double t20 = self->_21 * xTan;
          double t21 = self->_20 * yTan;

          self->_20 += t20;
          self->_21 += t21;

          self->_type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          double t10 = self->_11 * xTan;
          double t20 = self->_21 * xTan;

          self->_01 += self->_00 * yTan;
          self->_21 += self->_20 * yTan;

          self->_10 += t10;
          self->_20 += t20;

          self->_type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
          self->_type = TRANSFORM_TYPE_AFFINE;
          // ... Fall through ...

        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          double t00 = self->_01 * xTan;
          double t10 = self->_11 * xTan;
          double t20 = self->_21 * xTan;

          self->_01 += self->_00 * yTan;
          self->_11 += self->_10 * yTan;
          self->_21 += self->_20 * yTan;

          self->_00 += t00;
          self->_10 += t10;
          self->_20 += t20;

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Rotate]
    // ------------------------------------------------------------------------

    //     [ cos(a) sin(a) 0]
    // M = [-sin(a) cos(a) 0] * M
    //     [   0      0    1]

    case ENCODE_OP(TRANSFORM_OP_ROTATEF, MATRIX_ORDER_PREPEND):
      angle = double(params->rotationf().angle);
      goto _RotatePrepend;

    case ENCODE_OP(TRANSFORM_OP_ROTATED, MATRIX_ORDER_PREPEND):
    {
      angle = params->rotationd().angle;

_RotatePrepend:
      double as, ac;
      Math::sincos(angle, &as, &ac);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
          self->_00 = ac;
          self->_01 = as;
          self->_10 =-as;
          self->_11 = ac;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          double m00 = self->_00;
          double m11 = self->_11;

          self->_00 = m00 * ac;
          self->_01 = m11 * as;
          self->_10 = m00 *-as;
          self->_11 = m11 * ac;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          double m00 = self->_00;
          double m01 = self->_01;
          double m10 = self->_10;
          double m11 = self->_11;

          self->_00 = m00 * ac + m10 * as;
          self->_01 = m01 * ac + m11 * as;
          self->_10 = m00 *-as + m10 * ac;
          self->_11 = m01 *-as + m11 * ac;

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = ac * self->_00 + as * self->_10;
          double t01 = ac * self->_01 + as * self->_11;
          double t02 = ac * self->_02 + as * self->_12;

          self->_10 = -as * self->_00 + ac * self->_10;
          self->_11 = -as * self->_01 + ac * self->_11;
          self->_12 = -as * self->_02 + ac * self->_12;

          self->_00 = t00;
          self->_01 = t01;
          self->_02 = t02;

          self->_type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    //         [ cos(a) sin(a) 0]
    // M = M * [-sin(a) cos(a) 0]
    //         [   0      0    1]

    case ENCODE_OP(TRANSFORM_OP_ROTATEF, MATRIX_ORDER_APPEND):
      angle = double(params->rotationf().angle);
      goto _RotateAppend;

    case ENCODE_OP(TRANSFORM_OP_ROTATED, MATRIX_ORDER_APPEND):
    {
      angle = params->rotationd().angle;

_RotateAppend:
      double as, ac;
      Math::sincos(angle, &as, &ac);

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
        {
          double t20 = self->_20 * ac - self->_21 * as;
          double t21 = self->_20 * as + self->_21 * ac;

          self->_20 = t20;
          self->_21 = t21;
          // ... Fall through ...
        }

        case TRANSFORM_TYPE_IDENTITY:
          self->_00 = ac;
          self->_01 = as;
          self->_10 =-as;
          self->_11 = ac;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          double t00 = self->_00 * ac;
          double t10 = self->_11 *-as;
          double t20 = self->_20 * ac + self->_21 *-as;

          self->_01 = self->_00 * as;
          self->_11 = self->_11 * ac;
          self->_21 = self->_20 * as + self->_21 * ac;

          self->_00 = t00;
          self->_10 = t10;
          self->_20 = t20;

          self->_type = TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        case TRANSFORM_TYPE_DEGENERATE:
        {
          double t00 = self->_00 * ac - self->_01 * as;
          double t10 = self->_10 * ac - self->_11 * as;
          double t20 = self->_20 * ac - self->_21 * as;

          self->_01 = self->_00 * as + self->_01 * ac;
          self->_11 = self->_10 * as + self->_11 * ac;
          self->_21 = self->_20 * as + self->_21 * ac;

          self->_00 = t00;
          self->_10 = t10;
          self->_20 = t20;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Multiply]
    // ------------------------------------------------------------------------

    //     [S00 S01 S02]
    // M = [S10 S11 S12] * M
    //     [S20 S21 S22]

    case ENCODE_OP(TRANSFORM_OP_MULTIPLYF, MATRIX_ORDER_PREPEND):
      srcTmp = params->transformf();
      src = &srcTmp;
      goto _MultiplyPrepend;

    case ENCODE_OP(TRANSFORM_OP_MULTIPLYD, MATRIX_ORDER_PREPEND):
    {
      src = &params->transformd();

_MultiplyPrepend:
      uint32_t sType = src->getType();
      uint32_t dType = Math::max(selfType, sType);

      self->_type = dType | TRANSFORM_TYPE_DIRTY;
      switch (dType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          break;

        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 += src->_20;
          self->_21 += src->_21;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          double t20 = src->_20 * self->_00;
          double t21 = src->_21 * self->_11;

          self->_00 *= src->_00;
          self->_11 *= src->_11;

          self->_20 += t20;
          self->_21 += t21;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          double t00 = src->_00 * self->_00 + src->_01 * self->_10;
          double t01 = src->_00 * self->_01 + src->_01 * self->_11;

          double t10 = src->_10 * self->_00 + src->_11 * self->_10;
          double t11 = src->_10 * self->_01 + src->_11 * self->_11;

          double t20 = src->_20 * self->_00 + src->_21 * self->_10;
          double t21 = src->_20 * self->_01 + src->_21 * self->_11;

          self->_00 = t00;
          self->_01 = t01;
          self->_10 = t10;
          self->_11 = t11;

          self->_20 += t20;
          self->_21 += t21;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = src->_00 * self->_00 + src->_01 * self->_10 + src->_02 * self->_20;
          double t01 = src->_00 * self->_01 + src->_01 * self->_11 + src->_02 * self->_21;
          double t02 = src->_00 * self->_02 + src->_01 * self->_12 + src->_02 * self->_22;

          double t10 = src->_10 * self->_00 + src->_11 * self->_10 + src->_12 * self->_20;
          double t11 = src->_10 * self->_01 + src->_11 * self->_11 + src->_12 * self->_21;
          double t12 = src->_10 * self->_02 + src->_11 * self->_12 + src->_12 * self->_22;

          double t20 = src->_20 * self->_00 + src->_21 * self->_10 + src->_22 * self->_20;
          double t21 = src->_20 * self->_01 + src->_21 * self->_11 + src->_22 * self->_21;
          double t22 = src->_20 * self->_02 + src->_21 * self->_12 + src->_22 * self->_22;

          self->_00 = t00;
          self->_01 = t01;
          self->_02 = t02;

          self->_10 = t10;
          self->_11 = t11;
          self->_12 = t12;

          self->_20 = t20;
          self->_21 = t21;
          self->_22 = t22;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    //         |S00 S01 S02|
    // M = M * |S10 S11 S12|
    //         |S20 S21 S22|

    case ENCODE_OP(TRANSFORM_OP_MULTIPLYF, MATRIX_ORDER_APPEND):
      srcTmp = params->transformf();
      src = &srcTmp;
      goto _MultiplyAppend;

    case ENCODE_OP(TRANSFORM_OP_MULTIPLYD, MATRIX_ORDER_APPEND):
    {
      src = &params->transformd();

_MultiplyAppend:
      uint32_t sType = src->getType();
      uint32_t dType = Math::max(selfType, sType);

      self->_type = dType | TRANSFORM_TYPE_DIRTY;
      switch (dType)
      {
        case TRANSFORM_TYPE_IDENTITY:
          break;

        case TRANSFORM_TYPE_TRANSLATION:
          self->_20 += src->_20;
          self->_21 += src->_21;
          break;

        case TRANSFORM_TYPE_SCALING:
        {
          double t00 = self->_00 * src->_00;
          double t11 = self->_11 * src->_11;

          double t20 = self->_20 * src->_00 + src->_20;
          double t21 = self->_21 * src->_11 + src->_21;

          self->_00 = t00;
          self->_11 = t11;

          self->_20 = t20;
          self->_21 = t21;
          break;
        }

        case TRANSFORM_TYPE_SWAP:
        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          double t00 = self->_00 * src->_00 + self->_01 * src->_10;
          double t01 = self->_00 * src->_01 + self->_01 * src->_11;

          double t10 = self->_10 * src->_00 + self->_11 * src->_10;
          double t11 = self->_10 * src->_01 + self->_11 * src->_11;

          double t20 = self->_20 * src->_00 + self->_21 * src->_10 + src->_20;
          double t21 = self->_20 * src->_01 + self->_21 * src->_11 + src->_21;

          self->_00 = t00;
          self->_01 = t01;

          self->_10 = t10;
          self->_11 = t11;

          self->_20 = t20;
          self->_21 = t21;
          break;
        }

        case TRANSFORM_TYPE_DEGENERATE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = self->_00 * src->_00 + self->_01 * src->_10 + self->_02 * src->_20;
          double t01 = self->_00 * src->_01 + self->_01 * src->_11 + self->_02 * src->_21;
          double t02 = self->_00 * src->_02 + self->_01 * src->_12 + self->_02 * src->_22;

          double t10 = self->_10 * src->_00 + self->_11 * src->_10 + self->_12 * src->_20;
          double t11 = self->_10 * src->_01 + self->_11 * src->_11 + self->_12 * src->_21;
          double t12 = self->_10 * src->_02 + self->_11 * src->_12 + self->_12 * src->_22;

          double t20 = self->_20 * src->_00 + self->_21 * src->_10 + self->_22 * src->_20;
          double t21 = self->_20 * src->_01 + self->_21 * src->_11 + self->_22 * src->_21;
          double t22 = self->_20 * src->_02 + self->_21 * src->_12 + self->_22 * src->_22;

          self->_00 = t00;
          self->_01 = t01;
          self->_02 = t02;

          self->_10 = t10;
          self->_11 = t11;
          self->_12 = t12;

          self->_20 = t20;
          self->_21 = t21;
          self->_22 = t22;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Multiply Inverted]
    // ------------------------------------------------------------------------

    //        |S00 S01 S02|
    // M = Inv|S10 S11 S12| * M
    //        |S20 S21 S22|

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INVF, MATRIX_ORDER_PREPEND):
      srcTmp = params->transformf();
      if (!srcTmp.invert())
        return ERR_GEOMETRY_DEGENERATE;

      src = &srcTmp;
      goto _MultiplyPrepend;

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INVD, MATRIX_ORDER_PREPEND):
      if (!TransformD::invert(srcTmp, params->transformd()))
        return ERR_GEOMETRY_DEGENERATE;

      src = &srcTmp;
      goto _MultiplyPrepend;

    //            |S00 S01 S02|
    // M = M * Inv|S10 S11 S12|
    //            |S20 S21 S22|

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INVF, MATRIX_ORDER_APPEND):
      srcTmp = params->transformf();
      if (!srcTmp.invert())
        return ERR_GEOMETRY_DEGENERATE;

      src = &srcTmp;
      goto _MultiplyAppend;

    case ENCODE_OP(TRANSFORM_OP_MULTIPLY_INVD, MATRIX_ORDER_APPEND):
      if (!TransformD::invert(srcTmp, params->transformd()))
        return ERR_GEOMETRY_DEGENERATE;

      src = &srcTmp;
      goto _MultiplyAppend;

    // ------------------------------------------------------------------------
    // [Flip]
    // ------------------------------------------------------------------------

    case ENCODE_OP(TRANSFORM_OP_FLIP, MATRIX_ORDER_PREPEND):
    case ENCODE_OP(TRANSFORM_OP_FLIP, MATRIX_ORDER_APPEND):
    {
      uint32_t axis = reinterpret_cast<const uint32_t*>(_params)[0];

      if (axis & AXIS_X) { self->_00 = -self->_00; self->_01 = -self->_01; self->_02 = -self->_02; }
      if (axis & AXIS_Y) { self->_10 = -self->_10; self->_11 = -self->_11; self->_12 = -self->_12; }
      if (axis & AXIS_Z) { self->_20 = -self->_20; self->_21 = -self->_21; self->_22 = -self->_22; }

      self->_type = Math::max<uint32_t>(selfType, TRANSFORM_TYPE_SCALING) | TRANSFORM_TYPE_DIRTY;
      break;
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
static err_t FOG_CDECL TransformT_transform2(NumT_(Transform)* dst,
  const NumT_(Transform)* src, uint32_t opType, const void* params)
{
  *dst = *src;
  return dst->_transform(opType, params);
}

// ============================================================================
// [Fog::Transform - Multiply]
// ============================================================================

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_multiply(NumT_(Transform)* dst,
  const NumT_(Transform)* a,
  const SrcT_(Transform)* b)
{
  FOG_ASSERT(reinterpret_cast<void*>(dst) != reinterpret_cast<const void*>(a));
  FOG_ASSERT(reinterpret_cast<void*>(dst) != reinterpret_cast<const void*>(b));

  uint32_t aType = a->getType();
  uint32_t bType = b->getType();
  uint32_t dType = Math::max(aType, bType);

  dst->_type = dType | TRANSFORM_TYPE_DIRTY;

  switch (dType)
  {
    case TRANSFORM_TYPE_IDENTITY:
      dst->_00 = NumT(1.0); dst->_01 = NumT(0.0); dst->_02 = NumT(0.0);
      dst->_10 = NumT(0.0); dst->_11 = NumT(1.0); dst->_12 = NumT(0.0);
      dst->_20 = NumT(0.0); dst->_21 = NumT(0.0); dst->_22 = NumT(1.0);
      break;

    case TRANSFORM_TYPE_TRANSLATION:
      dst->_00 = NumT(1.0);
      dst->_01 = NumT(0.0);
      dst->_02 = NumT(0.0);

      dst->_10 = NumT(0.0);
      dst->_11 = NumT(1.0);
      dst->_12 = NumT(0.0);

      dst->_20 = NumT(a->_20) + NumT(b->_20);
      dst->_21 = NumT(a->_21) + NumT(b->_21);
      dst->_22 = NumT(1.0);
      break;

    case TRANSFORM_TYPE_SCALING:
      dst->_00 = NumT(a->_00) * NumT(b->_00);
      dst->_01 = NumT(0.0);
      dst->_02 = NumT(0.0);

      dst->_10 = NumT(0.0);
      dst->_11 = NumT(a->_11) * NumT(b->_11);
      dst->_12 = NumT(0.0);

      dst->_20 = NumT(a->_20) * NumT(b->_00) + NumT(b->_20);
      dst->_21 = NumT(a->_21) * NumT(b->_11) + NumT(b->_21);
      dst->_22 = NumT(1.0);
      break;

    case TRANSFORM_TYPE_SWAP:
    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
      dst->_00 = NumT(a->_00) * NumT(b->_00) + NumT(a->_01) * NumT(b->_10);
      dst->_01 = NumT(a->_00) * NumT(b->_01) + NumT(a->_01) * NumT(b->_11);
      dst->_02 = NumT(0.0);

      dst->_10 = NumT(a->_10) * NumT(b->_00) + NumT(a->_11) * NumT(b->_10);
      dst->_11 = NumT(a->_10) * NumT(b->_01) + NumT(a->_11) * NumT(b->_11);
      dst->_12 = NumT(0.0);

      dst->_20 = NumT(a->_20) * NumT(b->_00) + NumT(a->_21) * NumT(b->_10) + NumT(b->_20);
      dst->_21 = NumT(a->_20) * NumT(b->_01) + NumT(a->_21) * NumT(b->_11) + NumT(b->_21);
      dst->_22 = NumT(1.0);
      break;

    case TRANSFORM_TYPE_DEGENERATE:
    case TRANSFORM_TYPE_PROJECTION:
      dst->_00 = NumT(a->_00) * NumT(b->_00) + NumT(a->_01) * NumT(b->_10) + NumT(a->_02) * NumT(b->_20);
      dst->_01 = NumT(a->_00) * NumT(b->_01) + NumT(a->_01) * NumT(b->_11) + NumT(a->_02) * NumT(b->_21);
      dst->_02 = NumT(a->_00) * NumT(b->_02) + NumT(a->_01) * NumT(b->_12) + NumT(a->_02) * NumT(b->_22);

      dst->_10 = NumT(a->_10) * NumT(b->_00) + NumT(a->_11) * NumT(b->_10) + NumT(a->_12) * NumT(b->_20);
      dst->_11 = NumT(a->_10) * NumT(b->_01) + NumT(a->_11) * NumT(b->_11) + NumT(a->_12) * NumT(b->_21);
      dst->_12 = NumT(a->_10) * NumT(b->_02) + NumT(a->_11) * NumT(b->_12) + NumT(a->_12) * NumT(b->_22);

      dst->_20 = NumT(a->_20) * NumT(b->_00) + NumT(a->_21) * NumT(b->_10) + NumT(a->_22) * NumT(b->_20);
      dst->_21 = NumT(a->_20) * NumT(b->_01) + NumT(a->_21) * NumT(b->_11) + NumT(a->_22) * NumT(b->_21);
      dst->_22 = NumT(a->_20) * NumT(b->_02) + NumT(a->_21) * NumT(b->_12) + NumT(a->_22) * NumT(b->_22);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::Transform - Invert]
// ============================================================================

template<typename NumT>
static bool FOG_CDECL TransformT_invert(NumT_(Transform)* self,
  const NumT_(Transform)* a)
{
  // Inverted matrix should be as accurate as possible so the 'double'
  // type is used for calculations.
  switch (a->getType())
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      self->_setData(a->_type,
        NumT(1.0), NumT(0.0), NumT(0.0),
        NumT(0.0), NumT(1.0), NumT(0.0),
        NumT(0.0), NumT(0.0), NumT(1.0));
      return true;
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      self->_setData(a->_type,
        NumT(1.0)  , NumT(0.0)  , NumT(0.0),
        NumT(0.0)  , NumT(1.0)  , NumT(0.0),
       -NumT(a->_20),-NumT(a->_21), NumT(1.0));
      return true;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      if (Math::isFuzzyZero(a->_00 * a->_11)) goto _NonInvertible;

      double inv00 = 1.0 / double(a->_00);
      double inv11 = 1.0 / double(a->_11);

      self->_setData(a->_type,
        NumT(inv00),
        NumT(0.0),
        NumT(0.0),

        NumT(0.0),
        NumT(inv11),
        NumT(0.0),

        NumT(inv00 * -double(a->_20)),
        NumT(inv11 * -double(a->_21)),
        NumT(1.0)
      );
      return true;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      double d = -double(a->_01) * double(a->_10);
      if (Math::isFuzzyZero(d)) goto _NonInvertible;

      d = 1.0 / d;

      double t01 = -double(a->_01) * d;
      double t10 = -double(a->_10) * d;
      double t20 = -double(a->_21) * t10;
      double t21 = -double(a->_20) * t01;

      self->_01 = NumT(t01);
      self->_10 = NumT(t10);
      self->_20 = NumT(t20);
      self->_21 = NumT(t21);
      return true;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      // Inverted matrix should be as accurate as possible so the 'double'
      // type is used instead of 'float' here.
      double d = (double(a->_00) * double(a->_11) - double(a->_01) * double(a->_10));
      if (Math::isFuzzyZero(d)) goto _NonInvertible;

      d = 1.0 / d;

      double t00 =  double(a->_11) * d;
      double t01 = -double(a->_01) * d;
      double t10 = -double(a->_10) * d;
      double t11 =  double(a->_00) * d;
      double t20 = -double(a->_20) * t00 - double(a->_21) * t10;
      double t21 = -double(a->_20) * t01 - double(a->_21) * t11;

      self->_setData(a->_type,
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
      double d0 = double(a->_11) * double(a->_22) - double(a->_12) * double(a->_21);
      double d1 = double(a->_02) * double(a->_21) - double(a->_01) * double(a->_22);
      double d2 = double(a->_01) * double(a->_12) - double(a->_02) * double(a->_11);

      double d  = double(a->_00) * d0 +
                  double(a->_10) * d1 +
                  double(a->_20) * d2;
      if (Math::isFuzzyZero(d)) goto _NonInvertible;

      d = 1.0 / d;
      self->_setData(a->_type,
        NumT(d * d0),
        NumT(d * d1),
        NumT(d * d2),

        NumT(d * (double(a->_12) * double(a->_20) - double(a->_10) * double(a->_22))),
        NumT(d * (double(a->_00) * double(a->_22) - double(a->_02) * double(a->_20))),
        NumT(d * (double(a->_02) * double(a->_10) - double(a->_00) * double(a->_12))),

        NumT(d * (double(a->_10) * double(a->_21) - double(a->_11) * double(a->_20))),
        NumT(d * (double(a->_01) * double(a->_20) - double(a->_00) * double(a->_21))),
        NumT(d * (double(a->_00) * double(a->_11) - double(a->_01) * double(a->_10)))
      );
      return true;
    }

    case TRANSFORM_TYPE_DEGENERATE:
      goto _NonInvertible;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

_NonInvertible:
  self->_setData(TRANSFORM_TYPE_DEGENERATE,
    NumT(0.0), NumT(0.0), NumT(0.0),
    NumT(0.0), NumT(0.0), NumT(0.0),
    NumT(0.0), NumT(0.0), NumT(0.0));
  return false;
}

// ============================================================================
// [Fog::Transform - MapPoint(s)]
// ============================================================================

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointT(const NumT_(Transform)* self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src)
{
  uint32_t selfType = self->getType();

  NumT x = src->x;
  NumT y = src->y;

  switch (selfType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      break;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      x *= self->_00;
      y *= self->_11;
      // ... Fall through ...
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      x += self->_20;
      y += self->_21;
      break;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      x *= src->y * self->_10;
      y *= src->x * self->_01;
      x += self->_20;
      y += self->_21;
      break;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      NumT _x = x;
      NumT _y = y;

      x = _x * self->_00 + _y * self->_10 + self->_20;
      y = _x * self->_01 + _y * self->_11 + self->_21;
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      NumT _x = x;
      NumT _y = y;
      NumT _w = x * self->_02 + y * self->_12 + self->_22;

      if (Math::isFuzzyZero(_w)) _w = MathConstant<NumT>::getEpsilon();
      _w = NumT(1.0) / _w;

      x = _x * self->_00 + _y * self->_10 + self->_20;
      y = _x * self->_01 + _y * self->_11 + self->_21;

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

  dst->set(x, y);
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Identity(const NumT_(Transform)* self,
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
static void FOG_CDECL TransformT_mapPointsT_Translation(const NumT_(Transform)* self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _20 = self->_20;
  NumT _21 = self->_21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x + _20,
             src->y + _21);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Scaling(const NumT_(Transform)* self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _00 = self->_00;
  NumT _11 = self->_11;

  NumT _20 = self->_20;
  NumT _21 = self->_21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x * _00 + _20,
             src->y * _11 + _21);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Swap(const NumT_(Transform)* self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _10 = self->_10;
  NumT _01 = self->_01;

  NumT _20 = self->_20;
  NumT _21 = self->_21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->y * _10 + _20,
             src->x * _01 + _21);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Affine(const NumT_(Transform)* self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _00 = self->_00;
  NumT _01 = self->_01;
  NumT _10 = self->_10;
  NumT _11 = self->_11;
  NumT _20 = self->_20;
  NumT _21 = self->_21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x * _00 + src->y * _10 + _20,
             src->x * _01 + src->y * _11 + _21);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Projection(const NumT_(Transform)* self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src,
  size_t length)
{
  NumT _00 = self->_00;
  NumT _01 = self->_01;
  NumT _10 = self->_10;
  NumT _11 = self->_11;
  NumT _20 = self->_20;
  NumT _21 = self->_21;

  for (size_t i = length; i; i--, dst++, src++)
  {
    NumT _w = src->x * self->_02 + src->y * self->_12 + self->_22;

    if (Math::isFuzzyZero(_w)) _w = MathConstant<NumT>::getEpsilon();
    _w = NumT(1.0) / _w;

    dst->set((src->x * _00 + src->y * _10 + _20) * _w,
             (src->x * _01 + src->y * _11 + _21) * _w);
  }
}

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapPointsT_Degenerate(const NumT_(Transform)* self,
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
static err_t FOG_CDECL TransformT_mapPathT(const NumT_(Transform)* self,
  NumT_(Path)* dst,
  const SrcT_(Path)* src, uint32_t cntOp)
{
  size_t srcLength = src->getLength();
  uint32_t selfType = self->getType();

  if (selfType < TRANSFORM_TYPE_PROJECTION)
  {
    if (cntOp == CONTAINER_OP_REPLACE)
    {
      FOG_RETURN_ON_ERROR(dst->reserve(srcLength));

      self->_mapPoints(dst->_d->vertices, src->_d->vertices, srcLength);
      if (sizeof(NumT) != sizeof(SrcT) || (void*)dst->_d != (void*)src->_d)
      {
        MemOps::copy(dst->_d->commands, src->_d->commands, srcLength);
        dst->_d->length = srcLength;
      }
    }
    else
    {
      size_t pos = dst->_add(srcLength);
      if (FOG_UNLIKELY(pos == INVALID_INDEX)) return ERR_RT_OUT_OF_MEMORY;

      self->_mapPoints(dst->_d->vertices + pos, src->_d->vertices, srcLength);
      MemOps::copy(dst->_d->commands + pos, src->_d->commands, srcLength);
    }

    dst->_modified();
    return ERR_OK;
  }
  else
  {
    if (selfType == TRANSFORM_TYPE_DEGENERATE)
    {
      if (cntOp == CONTAINER_OP_REPLACE)
        dst->clear();
      return ERR_GEOMETRY_DEGENERATE;
    }

    if (sizeof(NumT) == sizeof(SrcT) && (void*)dst == (void*)src)
    {
      SrcT_(Path) tmp(*src);
      return self->mapPathData(*dst, tmp.getCommands(), tmp.getVertices(), srcLength, cntOp);
    }
    else
    {
      return self->mapPathData(*dst, src->getCommands(), src->getVertices(), srcLength, cntOp);
    }
  }
}

template<typename NumT, typename SrcT>
static err_t FOG_CDECL TransformT_mapPathDataT(const NumT_(Transform)* self,
  NumT_(Path)* dst,
  const uint8_t* srcCmd, const SrcT_(Point)* srcPts, size_t srcLength, uint32_t cntOp)
{
  if (FOG_UNLIKELY(srcLength == 0))
  {
    if (cntOp == CONTAINER_OP_REPLACE) dst->clear();
    return ERR_OK;
  }

  if (srcCmd[0] == PATH_CMD_QUAD_TO || srcCmd[0] == PATH_CMD_CUBIC_TO)
  {
    if (cntOp == CONTAINER_OP_REPLACE) dst->clear();
    return ERR_GEOMETRY_INVALID;
  }

  uint32_t transformType = self->getType();
  if (transformType == TRANSFORM_TYPE_DEGENERATE)
  {
    if (cntOp == CONTAINER_OP_REPLACE) dst->clear();
    return ERR_GEOMETRY_DEGENERATE;
  }

  if (transformType != TRANSFORM_TYPE_PROJECTION)
  {
    size_t pos = dst->_prepare(cntOp, srcLength);
    if (FOG_UNLIKELY(pos == INVALID_INDEX)) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* dstCmd = dst->_d->commands + pos;
    NumT_(Point)* dstPts = dst->_d->vertices + pos;

    MemOps::copy(dstCmd, srcCmd, srcLength);
    self->_mapPoints(dstPts, srcPts, srcLength);

    dst->_d->vType |= PATH_FLAG_DIRTY_BBOX | PATH_FLAG_DIRTY_CMD;
    return ERR_OK;
  }
  else
  {
    if (srcLength >= SIZE_MAX / 4)
      return ERR_RT_OUT_OF_MEMORY;

    size_t pos = dst->_prepare(cntOp, srcLength * 4);
    if (FOG_UNLIKELY(pos == INVALID_INDEX)) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* dstCmd = dst->_d->commands + pos;
    NumT_(Point)* dstPts = dst->_d->vertices + pos;

    size_t i = srcLength;
    uint32_t mask = PATH_FLAG_DIRTY_BBOX;
    NumT_(Point) spline[20];

    NumT _00 = self->_00;
    NumT _01 = self->_01;
    NumT _10 = self->_10;
    NumT _11 = self->_11;
    NumT _20 = self->_20;
    NumT _21 = self->_21;
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

          _w = srcPts[0].x * self->_02 + srcPts[0].y * self->_12 + self->_22;
          if (Math::isFuzzyZero(_w)) _w = MathConstant<NumT>::getEpsilon();

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
          mask |= PATH_FLAG_HAS_QBEZIER;

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
            NumT w = spline[j].x * self->_02 + spline[j].y * self->_12 + self->_22;
            if (Math::isFuzzyZero(w)) w = MathConstant<NumT>::getEpsilon();

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
          mask |= PATH_FLAG_HAS_CBEZIER;

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
            NumT w = spline[j].x * self->_02 + spline[j].y * self->_12 + self->_22;
            if (Math::isFuzzyZero(w)) w = MathConstant<NumT>::getEpsilon();

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

    dst->_d->length = (size_t)(dstCmd - dst->_d->commands);
    dst->_d->vType |= mask;
    return ERR_OK;

_Invalid:
    dst->_d->length = pos;
    return ERR_GEOMETRY_INVALID;
  }
}

// ============================================================================
// [Fog::Transform - MapBox]
// ============================================================================

template<typename NumT, typename SrcT>
static void FOG_CDECL TransformT_mapBoxT(const NumT_(Transform)* self,
  NumT_(Box)* dst,
  const SrcT_(Box)* src)
{
  uint32_t transformType = self->getType();

  switch (transformType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      *dst = *src;
      break;
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      dst->x0 = src->x0 + self->_20;
      dst->y0 = src->y0 + self->_21;
      dst->x1 = src->x1 + self->_20;
      dst->y1 = src->y1 + self->_21;
      break;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      dst->x0 = src->x0 * self->_00 + self->_20;
      dst->y0 = src->y0 * self->_11 + self->_21;
      dst->x1 = src->x1 * self->_00 + self->_20;
      dst->y1 = src->y1 * self->_11 + self->_21;

_Swap:
      if (dst->x0 > dst->x1) swap(dst->x0, dst->x1);
      if (dst->y0 > dst->y1) swap(dst->y0, dst->y1);
      break;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      NumT x, y;

      x = src->x0;
      y = src->y0;

      dst->x0 = y * self->_10 + self->_20;
      dst->y0 = x * self->_01 + self->_21;

      x = src->x1;
      y = src->y1;

      dst->x1 = y * self->_10 + self->_20;
      dst->y1 = x * self->_01 + self->_21;
      goto _Swap;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      NumT x;
      NumT y;

      x = src->x0 * self->_00 + src->y0 * self->_10;
      y = src->x0 * self->_01 + src->y0 * self->_11;

      NumT minX = x, maxX = x;
      NumT minY = y, maxY = y;

      x = src->x1 * self->_00 + src->y0 * self->_10;
      y = src->x1 * self->_01 + src->y0 * self->_11;

      if (minX > x) minX = x; else if (maxX < x) maxX = x;
      if (minY > y) minY = y; else if (maxY < y) maxY = y;

      x = src->x1 * self->_00 + src->y1 * self->_10;
      y = src->x1 * self->_01 + src->y1 * self->_11;

      if (minX > x) minX = x; else if (maxX < x) maxX = x;
      if (minY > y) minY = y; else if (maxY < y) maxY = y;

      x = src->x0 * self->_00 + src->y1 * self->_10;
      y = src->x0 * self->_01 + src->y1 * self->_11;

      if (minX > x) minX = x; else if (maxX < x) maxX = x;
      if (minY > y) minY = y; else if (maxY < y) maxY = y;

      minX += self->_20;
      minY += self->_21;
      maxX += self->_20;
      maxY += self->_21;

      dst->setBox(minX, minY, maxX, maxY);
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      NumT_(Point) pts[4];

      pts[0].set(src->x0, src->y0);
      pts[1].set(src->x1, src->y0);
      pts[2].set(src->x1, src->y1);
      pts[3].set(src->x0, src->y1);
      self->_mapPoints(pts, pts, 4);

      dst->x0 = Math::min(pts[0].x, pts[1].x, pts[2].x, pts[3].x);
      dst->y0 = Math::min(pts[0].y, pts[1].y, pts[2].y, pts[3].y);

      dst->x1 = Math::max(pts[0].x, pts[1].x, pts[2].x, pts[3].x);
      dst->y1 = Math::max(pts[0].y, pts[1].y, pts[2].y, pts[3].y);
      break;
    }

    case TRANSFORM_TYPE_DEGENERATE:
    {
      dst->reset();
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
static void FOG_CDECL TransformT_mapVectorT(const NumT_(Transform)* self,
  NumT_(Point)* dst,
  const SrcT_(Point)* src)
{
  uint32_t selfType = self->getType();
  NumT x, y;

  switch (selfType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    case TRANSFORM_TYPE_TRANSLATION:
      break;

    case TRANSFORM_TYPE_SCALING:
      x = src->x * self->_00;
      y = src->y * self->_11;
      break;

    case TRANSFORM_TYPE_SWAP:
      x = src->y * self->_10;
      y = src->x * self->_01;
      break;

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    case TRANSFORM_TYPE_PROJECTION:
    {
      x = src->x * self->_00 + src->y * self->_10;
      y = src->x * self->_01 + src->y * self->_11;
      if (selfType != TRANSFORM_TYPE_PROJECTION) break;

      NumT _w0 = (self->_22);
      NumT _w1 = (self->_22 + src->x * self->_02 + src->y * self->_12);

      if (Math::isFuzzyZero(_w0)) _w0 = MathConstant<NumT>::getEpsilon();
      if (Math::isFuzzyZero(_w1)) _w1 = MathConstant<NumT>::getEpsilon();

      _w0 = NumT(1.0) / _w0;
      _w1 = NumT(1.0) / _w1;

      x += self->_20;
      y += self->_21;

      x *= _w1;
      y *= _w1;

      x -= self->_20 * _w0;
      y -= self->_21 * _w0;
      break;
    }

    case TRANSFORM_TYPE_DEGENERATE:
      x = NumT(0.0);
      y = NumT(0.0);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  dst->set(x, y);
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
static err_t FOG_CDECL TransformT_getScaling(const NumT_(Transform)* self,
  NumT_(Point)* dst, bool absolute)
{
  if (absolute)
  {
    // Absolute scaling.
    dst->x = Math::sqrt(self->_00 * self->_00 + self->_10 * self->_10);
    dst->y = Math::sqrt(self->_01 * self->_01 + self->_11 * self->_11);
  }
  else
  {
    // Average scaling.
    NumT_(Transform) t(self->rotated(-self->getRotation(), MATRIX_ORDER_APPEND));

    dst->x = NumT(1.0);
    dst->y = NumT(1.0);
    t.mapVector(*dst);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Transform - GetRotation]
// ============================================================================

template<typename NumT>
static NumT FOG_CDECL TransformT_getRotation(const NumT_(Transform)* self)
{
  NumT_(Point) pt(NumT(1.0), NumT(0.0));
  self->mapVector(pt);
  return Math::atan2(pt.y, pt.x);
}

// ============================================================================
// [Fog::Transform - GetAverageScaling]
// ============================================================================

template<typename NumT>
static NumT FOG_CDECL TransformT_getAverageScaling(const NumT_(Transform)* self)
{
  NumT x = self->_00 + self->_10;
  NumT y = self->_01 + self->_11;
  return Math::sqrt((x * x + y * y) * NumT(0.5));
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_CPU_DECLARE_INITIALIZER_3DNOW( Transform_init_3dNow(void) )
FOG_CPU_DECLARE_INITIALIZER_SSE( Transform_init_SSE(void) )
FOG_CPU_DECLARE_INITIALIZER_SSE2( Transform_init_SSE2(void) )

FOG_NO_EXPORT void Transform_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.transformf_create = TransformT_create<float>;
  fog_api.transformf_update = TransformT_update<float>;
  fog_api.transformf_transform = TransformF_transform;
  fog_api.transformf_transform2 = TransformT_transform2<float>;
  fog_api.transformf_multiply = TransformT_multiply<float, float>;
  fog_api.transformf_invert = TransformT_invert<float>;
  fog_api.transformf_getScaling = TransformT_getScaling<float>;
  fog_api.transformf_getRotation = TransformT_getRotation<float>;
  fog_api.transformf_getAverageScaling = TransformT_getAverageScaling<float>;

  fog_api.transformf_mapPointF = TransformT_mapPointT<float, float>;
  fog_api.transformf_mapPathF = TransformT_mapPathT<float, float>;
  fog_api.transformf_mapPathDataF = TransformT_mapPathDataT<float, float>;
  fog_api.transformf_mapBoxF = TransformT_mapBoxT<float, float>;
  fog_api.transformf_mapVectorF = TransformT_mapVectorT<float, float>;

  fog_api.transformd_create = TransformT_create<double>;
  fog_api.transformd_update = TransformT_update<double>;
  fog_api.transformd_transform = TransformD_transform;
  fog_api.transformd_transform2 = TransformT_transform2<double>;
  fog_api.transformd_multiply = TransformT_multiply<double, double>;
  fog_api.transformd_invert = TransformT_invert<double>;
  fog_api.transformd_getScaling = TransformT_getScaling<double>;
  fog_api.transformd_getRotation = TransformT_getRotation<double>;
  fog_api.transformd_getAverageScaling = TransformT_getAverageScaling<double>;

  fog_api.transformd_mapPathF = TransformT_mapPathT<double, float>;
  fog_api.transformd_mapPathD = TransformT_mapPathT<double, double>;
  fog_api.transformd_mapPathDataF = TransformT_mapPathDataT<double, float>;
  fog_api.transformd_mapPathDataD = TransformT_mapPathDataT<double, double>;
  fog_api.transformd_mapBoxD = TransformT_mapBoxT<double, double>;

#if defined(FOG_TRANSFORM_INIT_C)
  fog_api.transformd_mapPointD = TransformT_mapPointT<double, double>;
  fog_api.transformd_mapVectorD = TransformT_mapVectorT<double, double>;

  fog_api.transformf_mapPointsF[TRANSFORM_TYPE_IDENTITY   ] = TransformT_mapPointsT_Identity   <float, float>;
  fog_api.transformf_mapPointsF[TRANSFORM_TYPE_TRANSLATION] = TransformT_mapPointsT_Translation<float, float>;
  fog_api.transformf_mapPointsF[TRANSFORM_TYPE_SCALING    ] = TransformT_mapPointsT_Scaling    <float, float>;
  fog_api.transformf_mapPointsF[TRANSFORM_TYPE_SWAP       ] = TransformT_mapPointsT_Swap       <float, float>;
  fog_api.transformf_mapPointsF[TRANSFORM_TYPE_ROTATION   ] = TransformT_mapPointsT_Affine     <float, float>;
  fog_api.transformf_mapPointsF[TRANSFORM_TYPE_AFFINE     ] = TransformT_mapPointsT_Affine     <float, float>;
  fog_api.transformf_mapPointsF[TRANSFORM_TYPE_PROJECTION ] = TransformT_mapPointsT_Projection <float, float>;
  fog_api.transformf_mapPointsF[TRANSFORM_TYPE_DEGENERATE ] = TransformT_mapPointsT_Degenerate <float, float>;

  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_IDENTITY   ] = TransformT_mapPointsT_Identity   <double, float>;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_TRANSLATION] = TransformT_mapPointsT_Translation<double, float>;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_SCALING    ] = TransformT_mapPointsT_Scaling    <double, float>;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_SWAP       ] = TransformT_mapPointsT_Swap       <double, float>;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_ROTATION   ] = TransformT_mapPointsT_Affine     <double, float>;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_AFFINE     ] = TransformT_mapPointsT_Affine     <double, float>;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_PROJECTION ] = TransformT_mapPointsT_Projection <double, float>;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_DEGENERATE ] = TransformT_mapPointsT_Degenerate <double, float>;

  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_IDENTITY   ] = TransformT_mapPointsT_Identity   <double, double>;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_TRANSLATION] = TransformT_mapPointsT_Translation<double, double>;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_SCALING    ] = TransformT_mapPointsT_Scaling    <double, double>;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_SWAP       ] = TransformT_mapPointsT_Swap       <double, double>;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_ROTATION   ] = TransformT_mapPointsT_Affine     <double, double>;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_AFFINE     ] = TransformT_mapPointsT_Affine     <double, double>;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_PROJECTION ] = TransformT_mapPointsT_Projection <double, double>;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_DEGENERATE ] = TransformT_mapPointsT_Degenerate <double, double>;
#endif // FOG_TRANSFORM_INIT_C

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  TransformF_identity->setData(1.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 1.0f);

  TransformD_identity->setData(1.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 1.0f);

  fog_api.transformf_oIdentity = &TransformF_identity;
  fog_api.transformd_oIdentity = &TransformD_identity;

  // --------------------------------------------------------------------------
  // [CPU Based Optimizations]
  // --------------------------------------------------------------------------

  FOG_CPU_USE_INITIALIZER_3DNOW( Transform_init_3dNow() )
  FOG_CPU_USE_INITIALIZER_SSE( Transform_init_SSE() )
  FOG_CPU_USE_INITIALIZER_SSE2( Transform_init_SSE2() )
}

} // Fog namespace
