// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/CpuInfo.h>
#include <Fog/Core/Math.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Transform.h>

namespace Fog {

// We can disable C implementation when hardcoding for SSE2.
#if !defined(FOG_HARDCODE_SSE2)
# define FOG_TRANSFORM_INIT_C
#endif // FOG_HARDCODE_SSE2

// ============================================================================
// [Fog::Transform - Helpers]
// ============================================================================

static bool _G2d_TransformF_squareToQuad(TransformF& self, const PointF* p)
{
  float dx = p[0].x - p[1].x + p[2].x - p[3].x;
  float dy = p[0].y - p[1].y + p[2].y - p[3].y;

  if (dx == 0.0f && dy == 0.0f)
  {   
    // Affine case (parallelogram).
    self._setData(TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY,
      p[1].x - p[0].x, p[1].y - p[0].y, 0.0f,
      p[2].x - p[1].x, p[2].y - p[1].y, 0.0f,
      p[0].x         , p[0].y         , 1.0f);
    return true;
  }
  else
  {
    float dx1 = p[1].x - p[2].x;
    float dy1 = p[1].y - p[2].y;
    float dx2 = p[3].x - p[2].x;
    float dy2 = p[3].y - p[2].y;
    float den = dx1 * dy2 - dx2 * dy1;

    if (den == 0.0f)
    {
      // Singular case
      self._setData(TRANSFORM_TYPE_PROJECTION,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f);
      return false;
    }
    else
    {
      // General case
      float u = (dx * dy2 - dy * dx2) / den;
      float v = (dy * dx1 - dx * dy1) / den;

      self._setData(
        TRANSFORM_TYPE_PROJECTION,
        p[1].x - p[0].x + u * p[1].x,
        p[1].y - p[0].y + u * p[1].y,
        u,
        p[3].x - p[0].x + v * p[3].x,
        p[3].y - p[0].y + v * p[3].y,
        v,
        p[0].x,
        p[0].y,
        1.0f);
      return true;
    }
  }
}

static bool _G2d_TransformD_squareToQuad(TransformD& self, const PointD* p)
{
  double dx = p[0].x - p[1].x + p[2].x - p[3].x;
  double dy = p[0].y - p[1].y + p[2].y - p[3].y;

  if (dx == 0.0 && dy == 0.0)
  {   
    // Affine case (parallelogram).
    self._setData(TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY,
      p[1].x - p[0].x, p[1].y - p[0].y, 0.0,
      p[2].x - p[1].x, p[2].y - p[1].y, 0.0,
      p[0].x         , p[0].y         , 1.0);
    return true;
  }
  else
  {
    double dx1 = p[1].x - p[2].x;
    double dy1 = p[1].y - p[2].y;
    double dx2 = p[3].x - p[2].x;
    double dy2 = p[3].y - p[2].y;
    double den = dx1 * dy2 - dx2 * dy1;

    if (den == 0.0)
    {
      // Singular case
      self._setData(TRANSFORM_TYPE_PROJECTION,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0);
      return false;
    }
    else
    {
      // General case
      double u = (dx * dy2 - dy * dx2) / den;
      double v = (dy * dx1 - dx * dy1) / den;

      self._setData(
        TRANSFORM_TYPE_PROJECTION,
        p[1].x - p[0].x + u * p[1].x,
        p[1].y - p[0].y + u * p[1].y,
        u,
        p[3].x - p[0].x + v * p[3].x,
        p[3].y - p[0].y + v * p[3].y,
        v,
        p[0].x,
        p[0].y,
        1.0);
      return true;
    }
  }
}

// ============================================================================
// [Fog::Transform - Create]
// ============================================================================

static err_t FOG_FASTCALL _G2d_TransformF_create(TransformF& self, uint32_t createType, const void* params)
{
  float p0;
  float p1;

  const float* paramsf = reinterpret_cast<const float*>(params);

  switch (createType)
  {
    // ------------------------------------------------------------------------
    // [Identity]
    // ------------------------------------------------------------------------

    default:
    case TRANSFORM_CREATE_IDENTITY:
    {
createIdentity:
      self._setData(TRANSFORM_TYPE_IDENTITY,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f);
      break;
    }

    // ------------------------------------------------------------------------
    // [Translation]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_TRANSLATION:
    {
      uint32_t type = TRANSFORM_TYPE_TRANSLATION;
      uint32_t identity = 2;

      p0 = paramsf[0];
      p1 = paramsf[1];

      if (Math::fzero(p0)) { p0 = 0.0f; identity--; }
      if (Math::fzero(p1)) { p1 = 0.0f; if (--identity == 0) goto createIdentity; }

      self._setData(type,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        p0  , p1  , 1.0f);
      break;
    }

    // ------------------------------------------------------------------------
    // [Scaling]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_SCALING:
    {
      uint32_t identity = 2;

      p0 = paramsf[0];
      p1 = paramsf[1];

      if (Math::feq(p0, 1.0f)) { p0 = 1.0f; identity--; }
      if (Math::feq(p1, 1.0f)) { p1 = 1.0f; if (--identity == 0) goto createIdentity; }

      self._setData(TRANSFORM_TYPE_SCALING,
        p0  , 0.0f, 0.0f,
        0.0f, p1  , 0.0f,
        0.0f, 0.0f, 1.0f);
      break;
    }

    // ------------------------------------------------------------------------
    // [Rotation]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_ROTATION:
    {
      float angle = paramsf[0];

      float aSin;
      float aCos;
      Math::sincos(angle, &aSin, &aCos);
      if (Math::fzero(aSin)) goto createIdentity;

      self._setData(TRANSFORM_TYPE_ROTATION,
        aCos, aSin, 0.0f,
       -aSin, aCos, 0.0f,
        0.0f, 0.0f, 1.0f);
      break;
    }

    // ------------------------------------------------------------------------
    // [Skewing]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_SKEWING:
    {
      uint32_t identity = 2;

      p0 = tan(paramsf[0]);
      p1 = tan(paramsf[1]);

      if (Math::fzero(p0)) { p0 = 0.0f; identity--; }
      if (Math::fzero(p1)) { p1 = 0.0f; if (--identity) goto createIdentity; }

      self._setData(TRANSFORM_TYPE_AFFINE,
        1.0f, p1  , 0.0f,
        p0  , 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f);
      break;
    }

    // ------------------------------------------------------------------------
    // [Line-Segment]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_LINE_SEGMENT:
    {
      float x1 = paramsf[0];
      float y1 = paramsf[1];
      float x2 = paramsf[2];
      float y2 = paramsf[3];

      float dx = x2 - x1;
      float dy = y2 - y1;

      float dist = paramsf[4];
      float s = dist > 0.0f ? Math::sqrt(dx * dx + dy * dy) / dist : 1.0f;

      float aSin;
      float aCos;
      Math::sincos(Math::atan2(dy, dx), &aSin, &aCos);

      self._setData(TRANSFORM_TYPE_AFFINE,
        s * aCos, s * aSin, 0.0f,
        s *-aSin, s * aCos, 0.0f,
        x1      , y1      , 1.0f);
      break;
    }

    // ------------------------------------------------------------------------
    // [Reflection]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_REFLECTION_U:
    {
      Math::sincos(paramsf[0], &p1, &p0);
      goto createReflectionUnit;
    }

    case TRANSFORM_CREATE_REFLECTION_XY:
    {
      p0 = paramsf[0];
      p1 = paramsf[1];

      float t = Math::sqrt(p0 * p0 + p1 * p1);
      p0 /= t;
      p1 /= t;

      goto createReflectionUnit;
    }

    case TRANSFORM_CREATE_REFLECTION_UNIT:
    {
      p0 = paramsf[0];
      p1 = paramsf[1];

createReflectionUnit:
      {
        float p02 = p0 * 2.0f;
        float p12 = p1 * 2.0f;

        self._setData(TRANSFORM_TYPE_AFFINE,
          p02 * p0 - 1.0f, p02 * p1       , 0.0f,
          p02 * p1       , p12 * p1 - 1.0f, 0.0f,
          0.0f           , 0.0f           , 1.0f);
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
      const ParallelogramParamsF* p = reinterpret_cast<const ParallelogramParamsF*>(params);

      TransformF dm(p->dst[1].x - p->dst[0].x, p->dst[1].y - p->dst[0].y,
                 p->dst[2].x - p->dst[0].x, p->dst[2].y - p->dst[0].y,
                 p->dst[0].x              , p->dst[0].y             );
      TransformF sm(p->src[1].x - p->src[0].x, p->src[1].y - p->src[0].y,
                 p->src[2].x - p->src[0].x, p->src[2].y - p->src[0].y,
                 p->src[0].x              , p->src[0].y             );

      // We know the types...
      dm._type = TRANSFORM_TYPE_AFFINE;
      sm._type = TRANSFORM_TYPE_AFFINE;

      if (sm.invert())
      {
        _g2d.transformf.multiply(self, sm, dm);
      }
      else
      {
        goto createIdentity;
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Quad-To-Quad]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_QUAD_TO_QUAD:
    {
      const QuadToQuadParamsF* p = reinterpret_cast<const QuadToQuadParamsF*>(params);

      TransformF dm(DONT_INITIALIZE);
      TransformF sm(DONT_INITIALIZE);

      if (_G2d_TransformF_squareToQuad(dm, p->dst) &&
          _G2d_TransformF_squareToQuad(sm, p->src) &&
          sm.invert())
      {
        _g2d.transformf.multiply(self, sm, dm);
      }
      else
      {
        goto createIdentity;
      }
    }
  }

  return ERR_OK;
}

static err_t FOG_FASTCALL _G2d_TransformD_create(TransformD& self, uint32_t createType, const void* params)
{
  double p0;
  double p1;

  const double* paramsd = reinterpret_cast<const double*>(params);

  switch (createType)
  {
    // ------------------------------------------------------------------------
    // [Identity]
    // ------------------------------------------------------------------------

    default:
    case TRANSFORM_CREATE_IDENTITY:
    {
createIdentity:
      self._setData(TRANSFORM_TYPE_IDENTITY,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0);
      break;
    }

    // ------------------------------------------------------------------------
    // [Translation]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_TRANSLATION:
    {
      uint32_t type = TRANSFORM_TYPE_TRANSLATION;
      uint32_t identity = 2;

      p0 = paramsd[0];
      p1 = paramsd[1];

      if (Math::fzero(p0)) { p0 = 0.0; identity--; }
      if (Math::fzero(p1)) { p1 = 0.0; if (--identity == 0) goto createIdentity; }

      self._setData(type,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        p0 , p1 , 1.0);
      break;
    }

    // ------------------------------------------------------------------------
    // [Scaling]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_SCALING:
    {
      uint32_t identity = 2;

      p0 = paramsd[0];
      p1 = paramsd[1];

      if (Math::feq(p0, 1.0)) { p0 = 1.0; identity--; }
      if (Math::feq(p1, 1.0)) { p1 = 1.0; if (--identity == 0) goto createIdentity; }

      self._setData(TRANSFORM_TYPE_SCALING,
        p0 , 0.0, 0.0,
        0.0, p1 , 0.0,
        0.0, 0.0, 1.0);
      break;
    }

    // ------------------------------------------------------------------------
    // [Rotation]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_ROTATION:
    {
      double angle = paramsd[0];

      double aSin;
      double aCos;
      Math::sincos(angle, &aSin, &aCos);
      if (Math::fzero(aSin)) goto createIdentity;

      self._setData(TRANSFORM_TYPE_ROTATION,
        aCos, aSin, 0.0,
       -aSin, aCos, 0.0,
        0.0 , 0.0 , 1.0);
      break;
    }

    // ------------------------------------------------------------------------
    // [Skewing]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_SKEWING:
    {
      uint32_t identity = 2;

      p0 = tan(paramsd[0]);
      p1 = tan(paramsd[1]);

      if (Math::fzero(p0)) { p0 = 0.0; identity--; }
      if (Math::fzero(p1)) { p1 = 0.0; if (--identity) goto createIdentity; }

      self._setData(TRANSFORM_TYPE_AFFINE,
        1.0, p1 , 0.0,
        p0 , 1.0, 0.0,
        0.0, 0.0, 1.0);
      break;
    }

    // ------------------------------------------------------------------------
    // [Line-Segment]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_LINE_SEGMENT:
    {
      double x1 = paramsd[0];
      double y1 = paramsd[1];
      double x2 = paramsd[2];
      double y2 = paramsd[3];

      double dx = x2 - x1;
      double dy = y2 - y1;

      double dist = paramsd[4];
      double s = dist > 0.0f ? Math::sqrt(dx * dx + dy * dy) / dist : 1.0;

      double aSin;
      double aCos;
      Math::sincos(Math::atan2(dy, dx), &aSin, &aCos);

      self._setData(TRANSFORM_TYPE_AFFINE,
        s * aCos, s * aSin, 0.0,
        s *-aSin, s * aCos, 0.0,
        x1      , y1      , 1.0);
      break;
    }

    // ------------------------------------------------------------------------
    // [Reflection]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_REFLECTION_U:
    {
      Math::sincos(paramsd[0], &p1, &p0);
      goto createReflectionUnit;
    }

    case TRANSFORM_CREATE_REFLECTION_XY:
    {
      p0 = paramsd[0];
      p1 = paramsd[1];

      double t = Math::sqrt(p0 * p0 + p1 * p1);
      p0 /= t;
      p1 /= t;

      goto createReflectionUnit;
    }

    case TRANSFORM_CREATE_REFLECTION_UNIT:
    {
      p0 = paramsd[0];
      p1 = paramsd[1];

createReflectionUnit:
      {
        double p02 = p0 * 2.0;
        double p12 = p1 * 2.0;

        self._setData(TRANSFORM_TYPE_AFFINE,
          p02 * p0 - 1.0f, p02 * p1       , 0.0,
          p02 * p1       , p12 * p1 - 1.0f, 0.0,
          0.0            , 0.0            , 1.0);
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
      const ParallelogramParamsD* p = reinterpret_cast<const ParallelogramParamsD*>(params);

      TransformD dm(p->dst[1].x - p->dst[0].x, p->dst[1].y - p->dst[0].y,
                 p->dst[2].x - p->dst[0].x, p->dst[2].y - p->dst[0].y,
                 p->dst[0].x              , p->dst[0].y             );
      TransformD sm(p->src[1].x - p->src[0].x, p->src[1].y - p->src[0].y,
                 p->src[2].x - p->src[0].x, p->src[2].y - p->src[0].y,
                 p->src[0].x              , p->src[0].y             );

      // We know the types...
      dm._type = TRANSFORM_TYPE_AFFINE;
      sm._type = TRANSFORM_TYPE_AFFINE;

      if (sm.invert())
      {
        _g2d.transformd.multiply(self, sm, dm);
      }
      else
      {
        goto createIdentity;
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Quad-To-Quad]
    // ------------------------------------------------------------------------

    case TRANSFORM_CREATE_QUAD_TO_QUAD:
    {
      const QuadToQuadParamsD* p = reinterpret_cast<const QuadToQuadParamsD*>(params);

      TransformD dm(DONT_INITIALIZE);
      TransformD sm(DONT_INITIALIZE);

      if (_G2d_TransformD_squareToQuad(dm, p->dst) &&
          _G2d_TransformD_squareToQuad(sm, p->src) &&
          sm.invert())
      {
        _g2d.transformd.multiply(self, sm, dm);
      }
      else
      {
        goto createIdentity;
      }
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Transform - Update]
// ============================================================================

static uint32_t FOG_FASTCALL _G2d_TransformF_update(const TransformF& self)
{
  uint32_t type;
  switch (self._type & (TRANSFORM_TYPE_DIRTY - 1))
  {
    // Matrix is not projection if:
    //   [. . 0]
    //   [. . 0]
    //   [. . 1]
    case TRANSFORM_TYPE_PROJECTION:
      if (!Math::fzero(self._02) || 
          !Math::fzero(self._12) ||
          !Math::feq(self._22, 1.0f))
      {
        type = TRANSFORM_TYPE_PROJECTION;
        break;
      }
      // ... fall through ...

    // Matrix is not affine/rotation if:
    //   [. 0 .]
    //   [0 . .]
    //   [. . .]
    case TRANSFORM_TYPE_AFFINE:
    case TRANSFORM_TYPE_ROTATION:
      if (!Math::fzero(self._01) ||
          !Math::fzero(self._10))
      {
        float d = self._00 * self._01 + self._10 * self._11 ;

        type = Math::fzero(d) 
          ? TRANSFORM_TYPE_ROTATION
          : TRANSFORM_TYPE_AFFINE;
        break;
      }
      // ... fall through ...

    // Matrix is not scaling if:
    //   [1 . .]
    //   [. 1 .]
    //   [. . .]
    case TRANSFORM_TYPE_SCALING:
      if (!Math::feq(self._00, 1.0f) ||
          !Math::feq(self._11, 1.0f))
      {
        type = TRANSFORM_TYPE_SCALING;
        break;
      }
      // ... fall through ...

    // Matrix is not translation if:
    //   [. . .]
    //   [. . .]
    //   [0 0 .]
    case TRANSFORM_TYPE_TRANSLATION:
      if (!Math::fzero(self._20) || 
          !Math::fzero(self._21))
      {
        type = TRANSFORM_TYPE_TRANSLATION;
        break;
      }
      // ... fall through ...

    // Matrix is identity:
    //   [1 0 0]
    //   [0 1 0]
    //   [0 0 1]
    case TRANSFORM_TYPE_IDENTITY:
    default:
      type = TRANSFORM_TYPE_IDENTITY;
      break;
  }

  self._type = type;
  return type;
}

static uint32_t FOG_FASTCALL _G2d_TransformD_update(const TransformD& self)
{
  uint32_t type;
  switch (self._type & (TRANSFORM_TYPE_DIRTY - 1))
  {
    // Matrix is not projection if:
    //   [. . 0]
    //   [. . 0]
    //   [. . 1]
    case TRANSFORM_TYPE_PROJECTION:
      if (!Math::fzero(self._02) || 
          !Math::fzero(self._12) ||
          !Math::feq(self._22, 1.0))
      {
        type = TRANSFORM_TYPE_PROJECTION;
        break;
      }
      // ... fall through ...

    // Matrix is not affine/rotation if:
    //   [. 0 .]
    //   [0 . .]
    //   [. . .]
    case TRANSFORM_TYPE_AFFINE:
    case TRANSFORM_TYPE_ROTATION:
      if (!Math::fzero(self._01) ||
          !Math::fzero(self._10))
      {
        double d = self._00 * self._01 + self._10 * self._11;

        type = Math::fzero(d) 
          ? TRANSFORM_TYPE_ROTATION
          : TRANSFORM_TYPE_AFFINE;
        break;
      }
      // ... fall through ...

    // Matrix is not scaling if:
    //   [1 . .]
    //   [. 1 .]
    //   [. . .]
    case TRANSFORM_TYPE_SCALING:
      if (!Math::feq(self._00, 1.0) ||
          !Math::feq(self._11, 1.0))
      {
        type = TRANSFORM_TYPE_SCALING;
        break;
      }
      // ... fall through ...

    // Matrix is not translation if:
    //   [. . .]
    //   [. . .]
    //   [0 0 .]
    case TRANSFORM_TYPE_TRANSLATION:
      if (!Math::fzero(self._20) || 
          !Math::fzero(self._21))
      {
        type = TRANSFORM_TYPE_TRANSLATION;
        break;
      }
      // ... fall through ...

    // Matrix is identity:
    //   [1 0 0]
    //   [0 1 0]
    //   [0 0 1]
    case TRANSFORM_TYPE_IDENTITY:
    default:
      type = TRANSFORM_TYPE_IDENTITY;
      break;
  }

  self._type = type;
  return type;
}

// ============================================================================
// [Fog::Transform - Transform]
// ============================================================================

#define ENCODE_OP(__opType__, __order__) ((int)(__opType__) | ((int)(__order__) << 4))

static err_t FOG_FASTCALL _G2d_TransformF_transform(TransformF& self, uint32_t opType, const void* params)
{
  const float* paramsf = reinterpret_cast<const float*>(params);
  uint32_t selfType = self.getType();

  TransformF tm(DONT_INITIALIZE);

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
      float x = paramsf[0];
      float y = paramsf[1];

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        {
          self._type = TRANSFORM_TYPE_TRANSLATION;
          // ... fall through ...
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

        case TRANSFORM_TYPE_PROJECTION:
        {
          self._22 += x * self._02 + y * self._12;
          // ... fall through ...
        }

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
      float x = paramsf[0];
      float y = paramsf[1];

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
      float x = paramsf[0];
      float y = paramsf[1];

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

        case TRANSFORM_TYPE_PROJECTION:
        {
          self._02 *= x;
          self._12 *= y;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          self._01 *= x;
          self._10 *= y;
          // ... fall through ...
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
      float x = paramsf[0];
      float y = paramsf[1];

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._20 *= x;
          self._21 *= y;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_IDENTITY:
        {
          self._00 = x;
          self._11 = y;
          self._type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          self._02 *= x;
          self._12 *= y;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          self._01 *= x;
          self._10 *= y;
          // ... fall through ...
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
      float angle = paramsf[0];

      float aSin;
      float aCos;
      Math::sincos(angle, &aSin, &aCos);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._00 = aCos;
          self._01 = aSin;
          self._10 =-aSin;
          self._11 = aCos;

          self._type = TRANSFORM_TYPE_ROTATION | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          float m00 = self._00;
          float m11 = self._11;

          self._00 = m00 * aCos;
          self._01 = m11 * aSin;
          self._10 = m00 *-aSin;
          self._11 = m11 * aCos;

          self._type = TRANSFORM_TYPE_ROTATION | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          float m00 = self._00;
          float m01 = self._01;
          float m10 = self._10;
          float m11 = self._11;

          self._00 = m00 * aCos + m10 * aSin;
          self._01 = m01 * aCos + m11 * aSin;
          self._10 = m00 *-aSin + m10 * aCos;
          self._11 = m01 *-aSin + m11 * aCos;

          self._type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          float t00 = aCos * self._00 + aSin * self._10;
          float t01 = aCos * self._01 + aSin * self._11;
          float t02 = aCos * self._02 + aSin * self._12;

          self._10 = -aSin * self._00 + aCos * self._10;
          self._11 = -aSin * self._01 + aCos * self._11;
          self._12 = -aSin * self._02 + aCos * self._12;

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
      float angle = paramsf[0];

      float aSin;
      float aCos;
      Math::sincos(angle, &aSin, &aCos);

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
        {
          float t20 = self._20 * aCos + self._21 *-aSin;
          float t21 = self._20 * aSin + self._21 * aCos;

          self._20 = t20;
          self._21 = t21;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_IDENTITY:
        {
          self._00 = aCos;
          self._01 = aSin;
          self._10 =-aSin;
          self._11 = aCos;

          self._type = TRANSFORM_TYPE_ROTATION | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          float t00 = self._00 * aCos;
          float t10 = self._11 *-aSin;
          float t20 = self._20 * aCos + self._21 *-aSin;

          self._01 = self._00 * aSin;
          self._11 = self._11 * aCos;
          self._21 = self._20 * aSin + self._21 * aCos;

          self._00 = t00;
          self._10 = t10;
          self._20 = t20;

          self._type = TRANSFORM_TYPE_ROTATION | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          float t00 = self._00 * aCos + self._01 * -aSin;
          float t10 = self._10 * aCos + self._11 * -aSin;
          float t20 = self._20 * aCos + self._21 * -aSin;

          self._01 = self._00 * aSin + self._01 * aCos;
          self._11 = self._10 * aSin + self._11 * aCos;
          self._21 = self._20 * aSin + self._21 * aCos;

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
      float xTan = Math::tan(paramsf[0]);
      float yTan = Math::tan(paramsf[1]);

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

        case TRANSFORM_TYPE_ROTATION:
        {
          self._type = TRANSFORM_TYPE_AFFINE;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_AFFINE:
skewPrependAffine:
        {
          float t00 = yTan * self._10;
          float t01 = yTan * self._11;

          self._10 += xTan * self._00;
          self._11 += xTan * self._01;

          self._00 += t00;
          self._01 += t01;

          self._type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          float t02 = yTan * self._12;

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
      float xTan = Math::tan(paramsf[0]);
      float yTan = Math::tan(paramsf[1]);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        {
          self._01 = yTan;
          self._10 = xTan;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_TRANSLATION:
        {
          float t20 = self._21 * xTan;
          float t21 = self._20 * yTan;

          self._20 += t20;
          self._21 += t21;

          self._type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          float t10 = self._11 * xTan;
          float t20 = self._21 * xTan;

          self._01 += self._00 * yTan;
          self._21 += self._20 * yTan;

          self._10 += t10;
          self._20 += t20;

          self._type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        {
          self._type = TRANSFORM_TYPE_AFFINE;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          float t00 = self._01 * xTan;
          float t10 = self._11 * xTan;
          float t20 = self._21 * xTan;

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
multiplyPrepend:
    {
      const TransformF& s = *reinterpret_cast<const TransformF*>(params);
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
          float t20 = s._20 * self._00;
          float t21 = s._21 * self._11;

          self._00 *= s._00;
          self._11 *= s._11;

          self._20 += t20;
          self._21 += t21;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          float t00 = s._00 * self._00 + s._01 * self._10;
          float t01 = s._00 * self._01 + s._01 * self._11;

          float t10 = s._10 * self._00 + s._11 * self._10;
          float t11 = s._10 * self._01 + s._11 * self._11;

          float t20 = s._20 * self._00 + s._21 * self._10;
          float t21 = s._20 * self._01 + s._21 * self._11;

          self._00 = t00;
          self._01 = t01;
          self._10 = t10;
          self._11 = t11;

          self._20 += t20;
          self._21 += t21;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          float t00 = s._00 * self._00 + s._01 * self._10 + s._02 * self._20;
          float t01 = s._00 * self._01 + s._01 * self._11 + s._02 * self._21;
          float t02 = s._00 * self._02 + s._01 * self._12 + s._02 * self._22;

          float t10 = s._10 * self._00 + s._11 * self._10 + s._12 * self._20;
          float t11 = s._10 * self._01 + s._11 * self._11 + s._12 * self._21;
          float t12 = s._10 * self._02 + s._11 * self._12 + s._12 * self._22;

          float t20 = s._20 * self._00 + s._21 * self._10 + s._22 * self._20;
          float t21 = s._20 * self._01 + s._21 * self._11 + s._22 * self._21;
          float t22 = s._20 * self._02 + s._21 * self._12 + s._22 * self._22;

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
multiplyAppend:
    {
      const TransformF& s = *reinterpret_cast<const TransformF*>(params);
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
          float t00 = self._00 * s._00;
          float t11 = self._11 * s._11;

          float t20 = self._20 * s._00 + s._20;
          float t21 = self._21 * s._11 + s._21;

          self._00 = t00;
          self._11 = t11;

          self._20 = t20;
          self._21 = t21;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          float t00 = self._00 * s._00 + self._01 * s._10;
          float t01 = self._00 * s._01 + self._01 * s._11;

          float t10 = self._10 * s._00 + self._11 * s._10;
          float t11 = self._10 * s._01 + self._11 * s._11;

          float t20 = self._20 * s._00 + self._21 * s._10 + s._20;
          float t21 = self._20 * s._01 + self._21 * s._11 + s._21;

          self._00 = t00;
          self._01 = t01;

          self._10 = t10;
          self._11 = t11;

          self._20 = t20;
          self._21 = t21;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          float t00 = self._00 * s._00 + self._01 * s._10 + self._02 * s._20;
          float t01 = self._00 * s._01 + self._01 * s._11 + self._02 * s._21;
          float t02 = self._00 * s._02 + self._01 * s._12 + self._02 * s._22;

          float t10 = self._10 * s._00 + self._11 * s._10 + self._12 * s._20;
          float t11 = self._10 * s._01 + self._11 * s._11 + self._12 * s._21;
          float t12 = self._10 * s._02 + self._11 * s._12 + self._12 * s._22;

          float t20 = self._20 * s._00 + self._21 * s._10 + self._22 * s._20;
          float t21 = self._20 * s._01 + self._21 * s._11 + self._22 * s._21;
          float t22 = self._20 * s._02 + self._21 * s._12 + self._22 * s._22;

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
      if (_g2d.transformf.invert(tm, *reinterpret_cast<const TransformF*>(params)))
      {
        params = &tm;
        if (opType == ENCODE_OP(TRANSFORM_OP_MULTIPLY_INV, MATRIX_ORDER_PREPEND))
          goto multiplyPrepend;
        else
          goto multiplyAppend;
      }
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

static err_t FOG_FASTCALL _G2d_TransformD_transform(TransformD& self, uint32_t opType, const void* params)
{
  const double* paramsd = reinterpret_cast<const double*>(params);
  uint32_t selfType = self.getType();

  TransformD tm(DONT_INITIALIZE);

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
      double x = paramsd[0];
      double y = paramsd[1];

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        {
          self._type = TRANSFORM_TYPE_TRANSLATION;
          // ... fall through ...
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

        case TRANSFORM_TYPE_PROJECTION:
        {
          self._22 += x * self._02 + y * self._12;
          // ... fall through ...
        }

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
      double x = paramsd[0];
      double y = paramsd[1];

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
      double x = paramsd[0];
      double y = paramsd[1];

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

        case TRANSFORM_TYPE_PROJECTION:
        {
          self._02 *= x;
          self._12 *= y;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          self._01 *= x;
          self._10 *= y;
          // ... fall through ...
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
      double x = paramsd[0];
      double y = paramsd[1];

      switch (selfType)
      {
        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._20 *= x;
          self._21 *= y;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_IDENTITY:
        {
          self._00 = x;
          self._11 = y;
          self._type = TRANSFORM_TYPE_SCALING | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          self._02 *= x;
          self._12 *= y;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          self._01 *= x;
          self._10 *= y;
          // ... fall through ...
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
      double angle = paramsd[0];

      double aSin;
      double aCos;
      Math::sincos(angle, &aSin, &aCos);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        case TRANSFORM_TYPE_TRANSLATION:
        {
          self._00 = aCos;
          self._01 = aSin;
          self._10 =-aSin;
          self._11 = aCos;

          self._type = TRANSFORM_TYPE_ROTATION | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          double m00 = self._00;
          double m11 = self._11;

          self._00 = m00 * aCos;
          self._01 = m11 * aSin;
          self._10 = m00 *-aSin;
          self._11 = m11 * aCos;

          self._type = TRANSFORM_TYPE_ROTATION | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          double m00 = self._00;
          double m01 = self._01;
          double m10 = self._10;
          double m11 = self._11;

          self._00 = m00 * aCos + m10 * aSin;
          self._01 = m01 * aCos + m11 * aSin;
          self._10 = m00 *-aSin + m10 * aCos;
          self._11 = m01 *-aSin + m11 * aCos;

          self._type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = aCos * self._00 + aSin * self._10;
          double t01 = aCos * self._01 + aSin * self._11;
          double t02 = aCos * self._02 + aSin * self._12;

          self._10 = -aSin * self._00 + aCos * self._10;
          self._11 = -aSin * self._01 + aCos * self._11;
          self._12 = -aSin * self._02 + aCos * self._12;

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
      double angle = paramsd[0];

      double aSin;
      double aCos;
      Math::sincos(angle, &aSin, &aCos);

      switch (selfType)
      {
        // TODO: Something is wrong...
        case TRANSFORM_TYPE_TRANSLATION:
        {
          double t20 = self._20 * aCos + self._21 *-aSin;
          double t21 = self._20 * aSin + self._21 * aCos;

          self._20 = t20;
          self._21 = t21;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_IDENTITY:
        {
          self._00 = aCos;
          self._01 = aSin;
          self._10 =-aSin;
          self._11 = aCos;

          self._type = TRANSFORM_TYPE_ROTATION | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          double t00 = self._00 * aCos;
          double t10 = self._11 *-aSin;
          double t20 = self._20 * aCos + self._21 *-aSin;

          self._01 = self._00 * aSin;
          self._11 = self._11 * aCos;
          self._21 = self._20 * aSin + self._21 * aCos;

          self._00 = t00;
          self._10 = t10;
          self._20 = t20;

          self._type = TRANSFORM_TYPE_ROTATION | TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = self._00 * aCos + self._01 * -aSin;
          double t10 = self._10 * aCos + self._11 * -aSin;
          double t20 = self._20 * aCos + self._21 * -aSin;

          self._01 = self._00 * aSin + self._01 * aCos;
          self._11 = self._10 * aSin + self._11 * aCos;
          self._21 = self._20 * aSin + self._21 * aCos;

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
      double xTan = Math::tan(paramsd[0]);
      double yTan = Math::tan(paramsd[1]);

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

        case TRANSFORM_TYPE_ROTATION:
        {
          self._type = TRANSFORM_TYPE_AFFINE;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_AFFINE:
skewPrependAffine:
        {
          double t00 = yTan * self._10;
          double t01 = yTan * self._11;

          self._10 += xTan * self._00;
          self._11 += xTan * self._01;

          self._00 += t00;
          self._01 += t01;

          self._type |= TRANSFORM_TYPE_DIRTY;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          double t02 = yTan * self._12;

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
      double xTan = Math::tan(paramsd[0]);
      double yTan = Math::tan(paramsd[1]);

      switch (selfType)
      {
        case TRANSFORM_TYPE_IDENTITY:
        {
          self._01 = yTan;
          self._10 = xTan;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_TRANSLATION:
        {
          double t20 = self._21 * xTan;
          double t21 = self._20 * yTan;

          self._20 += t20;
          self._21 += t21;

          self._type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_SCALING:
        {
          double t10 = self._11 * xTan;
          double t20 = self._21 * xTan;

          self._01 += self._00 * yTan;
          self._21 += self._20 * yTan;

          self._10 += t10;
          self._20 += t20;

          self._type = TRANSFORM_TYPE_AFFINE;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        {
          self._type = TRANSFORM_TYPE_AFFINE;
          // ... fall through ...
        }

        case TRANSFORM_TYPE_AFFINE:
        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = self._01 * xTan;
          double t10 = self._11 * xTan;
          double t20 = self._21 * xTan;

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
multiplyPrepend:
    {
      const TransformD& s = *reinterpret_cast<const TransformD*>(params);
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
          double t20 = s._20 * self._00;
          double t21 = s._21 * self._11;

          self._00 *= s._00;
          self._11 *= s._11;

          self._20 += t20;
          self._21 += t21;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          double t00 = s._00 * self._00 + s._01 * self._10;
          double t01 = s._00 * self._01 + s._01 * self._11;

          double t10 = s._10 * self._00 + s._11 * self._10;
          double t11 = s._10 * self._01 + s._11 * self._11;

          double t20 = s._20 * self._00 + s._21 * self._10;
          double t21 = s._20 * self._01 + s._21 * self._11;

          self._00 = t00;
          self._01 = t01;
          self._10 = t10;
          self._11 = t11;

          self._20 += t20;
          self._21 += t21;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = s._00 * self._00 + s._01 * self._10 + s._02 * self._20;
          double t01 = s._00 * self._01 + s._01 * self._11 + s._02 * self._21;
          double t02 = s._00 * self._02 + s._01 * self._12 + s._02 * self._22;

          double t10 = s._10 * self._00 + s._11 * self._10 + s._12 * self._20;
          double t11 = s._10 * self._01 + s._11 * self._11 + s._12 * self._21;
          double t12 = s._10 * self._02 + s._11 * self._12 + s._12 * self._22;

          double t20 = s._20 * self._00 + s._21 * self._10 + s._22 * self._20;
          double t21 = s._20 * self._01 + s._21 * self._11 + s._22 * self._21;
          double t22 = s._20 * self._02 + s._21 * self._12 + s._22 * self._22;

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
multiplyAppend:
    {
      const TransformD& s = *reinterpret_cast<const TransformD*>(params);
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
          double t00 = self._00 * s._00;
          double t11 = self._11 * s._11;

          double t20 = self._20 * s._00 + s._20;
          double t21 = self._21 * s._11 + s._21;

          self._00 = t00;
          self._11 = t11;

          self._20 = t20;
          self._21 = t21;
          break;
        }

        case TRANSFORM_TYPE_ROTATION:
        case TRANSFORM_TYPE_AFFINE:
        {
          double t00 = self._00 * s._00 + self._01 * s._10;
          double t01 = self._00 * s._01 + self._01 * s._11;

          double t10 = self._10 * s._00 + self._11 * s._10;
          double t11 = self._10 * s._01 + self._11 * s._11;

          double t20 = self._20 * s._00 + self._21 * s._10 + s._20;
          double t21 = self._20 * s._01 + self._21 * s._11 + s._21;

          self._00 = t00;
          self._01 = t01;

          self._10 = t10;
          self._11 = t11;

          self._20 = t20;
          self._21 = t21;
          break;
        }

        case TRANSFORM_TYPE_PROJECTION:
        {
          double t00 = self._00 * s._00 + self._01 * s._10 + self._02 * s._20;
          double t01 = self._00 * s._01 + self._01 * s._11 + self._02 * s._21;
          double t02 = self._00 * s._02 + self._01 * s._12 + self._02 * s._22;

          double t10 = self._10 * s._00 + self._11 * s._10 + self._12 * s._20;
          double t11 = self._10 * s._01 + self._11 * s._11 + self._12 * s._21;
          double t12 = self._10 * s._02 + self._11 * s._12 + self._12 * s._22;

          double t20 = self._20 * s._00 + self._21 * s._10 + self._22 * s._20;
          double t21 = self._20 * s._01 + self._21 * s._11 + self._22 * s._21;
          double t22 = self._20 * s._02 + self._21 * s._12 + self._22 * s._22;

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
      if (_g2d.transformd.invert(tm, *reinterpret_cast<const TransformD*>(params)))
      {
        params = &tm;
        if (opType == ENCODE_OP(TRANSFORM_OP_MULTIPLY_INV, MATRIX_ORDER_PREPEND))
          goto multiplyPrepend;
        else
          goto multiplyAppend;
      }
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

static TransformF FOG_FASTCALL _G2d_TransformF_transformed(const TransformF& self, uint32_t opType, const void* params)
{
  TransformF result(self);
  _g2d.transformf.transform(result, opType, params);
  return result;
}

static TransformD FOG_FASTCALL _G2d_TransformD_transformed(const TransformD& self, uint32_t opType, const void* params)
{
  TransformD result(self);
  _g2d.transformd.transform(result, opType, params);
  return result;
}

// ============================================================================
// [Fog::Transform - Multiply]
// ============================================================================

static void FOG_FASTCALL _G2d_TransformF_multiply(TransformF& dst, const TransformF& a, const TransformF& b)
{
  FOG_ASSERT(&dst != &a);
  FOG_ASSERT(&dst != &b);

  uint32_t aType = a.getType();
  uint32_t bType = b.getType();
  uint32_t dType = Math::max(aType, bType);

  dst._type = dType | TRANSFORM_TYPE_DIRTY;

  switch (dType)
  {
    case TRANSFORM_TYPE_IDENTITY:
      dst._00 = 1.0f; dst._01 = 0.0f; dst._02 = 0.0f;
      dst._10 = 0.0f; dst._11 = 1.0f; dst._12 = 0.0f;
      dst._20 = 0.0f; dst._21 = 0.0f; dst._22 = 1.0f;
      break;

    case TRANSFORM_TYPE_TRANSLATION:
      dst._00 = 1.0f;
      dst._01 = 0.0f;
      dst._02 = 0.0f;

      dst._10 = 0.0f;
      dst._11 = 1.0f;
      dst._12 = 0.0f;

      dst._20 = a._20 + b._20;
      dst._21 = a._21 + b._21;
      dst._22 = 1.0f;
      break;

    case TRANSFORM_TYPE_SCALING:
      dst._00 = a._00 * b._00;
      dst._01 = 0.0f;
      dst._02 = 0.0f;

      dst._10 = 0.0f;
      dst._11 = a._11 * b._11;
      dst._12 = 0.0f;

      dst._20 = a._20 * b._00 + b._20;
      dst._21 = a._21 * b._11 + b._21;
      dst._22 = 1.0f;
      break;

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
      dst._00 = a._00 * b._00 + a._01 * b._10;
      dst._01 = a._00 * b._01 + a._01 * b._11;
      dst._02 = 0.0f;

      dst._10 = a._10 * b._00 + a._11 * b._10;
      dst._11 = a._10 * b._01 + a._11 * b._11;
      dst._12 = 0.0f;

      dst._20 = a._20 * b._00 + a._21 * b._10 + b._20;
      dst._21 = a._20 * b._01 + a._21 * b._11 + b._21;
      dst._22 = 1.0f;
      break;

    case TRANSFORM_TYPE_PROJECTION:
      dst._00 = a._00 * b._00 + a._01 * b._10 + a._02 * b._20;
      dst._01 = a._00 * b._01 + a._01 * b._11 + a._02 * b._21;
      dst._02 = a._00 * b._02 + a._01 * b._12 + a._02 * b._22;

      dst._10 = a._10 * b._00 + a._11 * b._10 + a._12 * b._20;
      dst._11 = a._10 * b._01 + a._11 * b._11 + a._12 * b._21;
      dst._12 = a._10 * b._02 + a._11 * b._12 + a._12 * b._22;

      dst._20 = a._20 * b._00 + a._21 * b._10 + a._22 * b._20;
      dst._21 = a._20 * b._01 + a._21 * b._11 + a._22 * b._21;
      dst._22 = a._20 * b._02 + a._21 * b._12 + a._22 * b._22;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static void FOG_FASTCALL _G2d_TransformD_multiply(TransformD& dst, const TransformD& a, const TransformD& b)
{
  FOG_ASSERT(&dst != &a);
  FOG_ASSERT(&dst != &b);

  uint32_t aType = a.getType();
  uint32_t bType = b.getType();
  uint32_t dType = Math::max(aType, bType);

  dst._type = dType | TRANSFORM_TYPE_DIRTY;

  switch (dType)
  {
    case TRANSFORM_TYPE_IDENTITY:
      dst._00 = 1.0; dst._01 = 0.0; dst._02 = 0.0;
      dst._10 = 0.0; dst._11 = 1.0; dst._12 = 0.0;
      dst._20 = 0.0; dst._21 = 0.0; dst._22 = 1.0;
      break;

    case TRANSFORM_TYPE_TRANSLATION:
      dst._00 = 1.0;
      dst._01 = 0.0;
      dst._02 = 0.0;

      dst._10 = 0.0;
      dst._11 = 1.0;
      dst._12 = 0.0;

      dst._20 = a._20 + b._20;
      dst._21 = a._21 + b._21;
      dst._22 = 1.0;
      break;

    case TRANSFORM_TYPE_SCALING:
      dst._00 = a._00 * b._00;
      dst._01 = 0.0;
      dst._02 = 0.0;

      dst._10 = 0.0;
      dst._11 = a._11 * b._11;
      dst._12 = 0.0;

      dst._20 = a._20 * b._00 + b._20;
      dst._21 = a._21 * b._11 + b._21;
      dst._22 = 1.0;
      break;

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
      dst._00 = a._00 * b._00 + a._01 * b._10;
      dst._01 = a._00 * b._01 + a._01 * b._11;
      dst._02 = 0.0;

      dst._10 = a._10 * b._00 + a._11 * b._10;
      dst._11 = a._10 * b._01 + a._11 * b._11;
      dst._12 = 0.0;

      dst._20 = a._20 * b._00 + a._21 * b._10 + b._20;
      dst._21 = a._20 * b._01 + a._21 * b._11 + b._21;
      dst._22 = 1.0;
      break;

    case TRANSFORM_TYPE_PROJECTION:
      dst._00 = a._00 * b._00 + a._01 * b._10 + a._02 * b._20;
      dst._01 = a._00 * b._01 + a._01 * b._11 + a._02 * b._21;
      dst._02 = a._00 * b._02 + a._01 * b._12 + a._02 * b._22;

      dst._10 = a._10 * b._00 + a._11 * b._10 + a._12 * b._20;
      dst._11 = a._10 * b._01 + a._11 * b._11 + a._12 * b._21;
      dst._12 = a._10 * b._02 + a._11 * b._12 + a._12 * b._22;

      dst._20 = a._20 * b._00 + a._21 * b._10 + a._22 * b._20;
      dst._21 = a._20 * b._01 + a._21 * b._11 + a._22 * b._21;
      dst._22 = a._20 * b._02 + a._21 * b._12 + a._22 * b._22;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::Transform - Invert]
// ============================================================================

static bool FOG_FASTCALL _G2d_TransformF_invert(TransformF& self, const TransformF& a)
{
  // Inverted matrix should be as accurate as possible so the 'double' 
  // type is used for calculations.
  switch (a.getType())
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      self._setData(a._type,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f);
      return true;
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      self._setData(a._type,
        1.0f , 0.0f , 0.0f,
        0.0f , 1.0f , 0.0f,
       -a._20,-a._21, 1.0f);
      return true;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      if (Math::fzero(a._00 * a._11)) goto nonInvertible;

      double inv00 = 1.0 / (double)a._00;
      double inv11 = 1.0 / (double)a._11;

      self._setData(a._type,
        (float)inv00,
        0.0f,
        0.0f,
        
        0.0f,
        (float)inv11,
        0.0f,

        (float)(inv00 * (-(double)a._20)),
        (float)(inv11 * (-(double)a._21)),
        1.0f
      );
      return true;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      // Inverted matrix should be as accurate as possible so the 'double' 
      // type is used instead of 'float' here.
      double d = ((double)a._00 * (double)a._11 - (double)a._01 * (double)a._10);
      if (Math::fzero(d)) goto nonInvertible;

      d = 1.0 / d;

      double t00 =  (double)a._11 * d;
      double t01 = -(double)a._01 * d;
      double t10 = -(double)a._10 * d;
      double t11 =  (double)a._00 * d;
      double t20 = -(double)a._20 * t00 - (double)a._21 * t10;
      double t21 = -(double)a._20 * t01 - (double)a._21 * t11;

      self._setData(a._type,
        (float)t00, (float)t01, 0.0f,
        (float)t10, (float)t11, 0.0f,
        (float)t20, (float)t21, 1.0f
      );
      return true;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      // Inverted matrix should be as accurate as possible so the 'double' 
      // type is used instead of 'float' here.
      double d0 = (double)a._11 * (double)a._22 - (double)a._12 * (double)a._21;
      double d1 = (double)a._02 * (double)a._21 - (double)a._01 * (double)a._22;
      double d2 = (double)a._01 * (double)a._12 - (double)a._02 * (double)a._11;

      double d = (double)a._00 * d0 + 
                 (double)a._10 * d1 +
                 (double)a._20 * d2;
      if (Math::fzero(d)) goto nonInvertible;

      d = 1.0 / d;
      self._setData(a._type,
        (float)( d * d0 ),
        (float)( d * d1 ),
        (float)( d * d2 ),

        (float)( d * ((double)a._12 * (double)a._20 - (double)a._10 * (double)a._22) ),
        (float)( d * ((double)a._00 * (double)a._22 - (double)a._02 * (double)a._20) ),
        (float)( d * ((double)a._02 * (double)a._10 - (double)a._00 * (double)a._12) ),

        (float)( d * ((double)a._10 * (double)a._21 - (double)a._11 * (double)a._20) ),
        (float)( d * ((double)a._01 * (double)a._20 - (double)a._00 * (double)a._21) ),
        (float)( d * ((double)a._00 * (double)a._11 - (double)a._01 * (double)a._10) )
      );
      return true;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

nonInvertible:
  self._setData(TRANSFORM_TYPE_PROJECTION, 
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f);
  return false;
}

static bool FOG_FASTCALL _G2d_TransformD_invert(TransformD& self, const TransformD& a)
{
  switch (a.getType())
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      self._setData(a._type,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0);
      return true;
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      self._setData(a._type,
        1.0  , 0.0  , 0.0,
        0.0  , 1.0  , 0.0,
       -a._20,-a._21, 1.0);
      return true;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      if (Math::fzero(a._00 * a._11)) goto nonInvertible;

      double inv00 = 1.0 / a._00;
      double inv11 = 1.0 / a._11;

      self._setData(a._type,
        inv00           , 0.0             , 0.0,
        0.0             , inv11           , 0.0,
        inv00 * (-a._20), inv11 * (-a._21), 1.0
      );
      return true;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      // Inverted matrix should be as accurate as possible so the 'double' 
      // type is used instead of 'float' here.
      double d = (a._00 * a._11 - a._01 * a._10);
      if (Math::fzero(d)) goto nonInvertible;

      d = 1.0 / d;

      double t00 =  a._11 * d;
      double t01 = -a._01 * d;
      double t10 = -a._10 * d;
      double t11 =  a._00 * d;
      double t20 = -a._20 * t00 - a._21 * t10;
      double t21 = -a._20 * t01 - a._21 * t11;

      self._setData(a._type,
        t00, t01, 0.0,
        t10, t11, 0.0,
        t20, t21, 1.0
      );
      return true;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      double d0 = a._11 * a._22 - a._12 * a._21;
      double d1 = a._02 * a._21 - a._01 * a._22;
      double d2 = a._01 * a._12 - a._02 * a._11;

      double d = a._00 * d0 + a._10 * d1 + a._20 * d2;
      if (Math::fzero(d)) goto nonInvertible;

      d = 1.0 / d;
      self._setData(a._type,
        d * d0,
        d * d1,
        d * d2,

        d * (a._12 * a._20 - a._10 * a._22),
        d * (a._00 * a._22 - a._02 * a._20),
        d * (a._02 * a._10 - a._00 * a._12),

        d * (a._10 * a._21 - a._11 * a._20),
        d * (a._01 * a._20 - a._00 * a._21),
        d * (a._00 * a._11 - a._01 * a._10)
      );
      return true;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

nonInvertible:
  self._setData(TRANSFORM_TYPE_PROJECTION, 
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0);
  return false;
}

// ============================================================================
// [Fog::Transform - MapPoint(s)]
// ============================================================================

static void FOG_FASTCALL _G2d_TransformF_mapPointF(const TransformF& self, PointF& dst, const PointF& src)
{
  uint32_t selfType = self.getType();

  float x = src.x;
  float y = src.y;

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
      // ... fall through ...
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      x += self._20;
      y += self._21;
      break;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      float _x = x;
      float _y = y;

      x = _x * self._00 + _y * self._10 + self._20;
      y = _x * self._01 + _y * self._11 + self._21;
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      float _x = x;
      float _y = y;
      float _m = _x * self._02 + _y * self._12 + self._22;

      if (Math::fzero(_m)) _m = MATH_EPSILON_F;
      _m = 1.0f / _m;

      x = _x * self._00 + _y * self._10 + self._20;
      y = _x * self._01 + _y * self._11 + self._21;

      x *= _m;
      y *= _m;
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  dst.set(x, y);
}

static void FOG_FASTCALL _G2d_TransformD_mapPointD(const TransformD& self, PointD& dst, const PointD& src)
{
  uint32_t selfType = self.getType();

  double x = src.x;
  double y = src.y;

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
      // ... fall through ...
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      x += self._20;
      y += self._21;
      break;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      double _x = x;
      double _y = y;

      x = _x * self._00 + _y * self._10 + self._20;
      y = _x * self._01 + _y * self._11 + self._21;
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      double _x = x;
      double _y = y;
      double _m = _x * self._02 + _y * self._12 + self._22;

      if (Math::fzero(_m)) _m = MATH_EPSILON_D;
      _m = 1.0 / _m;

      x = _x * self._00 + _y * self._10 + self._20;
      y = _x * self._01 + _y * self._11 + self._21;

      x *= _m;
      y *= _m;
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  dst.set(x, y);
}


static void FOG_FASTCALL _G2d_TransformF_mapPointsF_Identity(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  if (dst == src) return;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    *dst = *src;
  }
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Identity(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  if (dst == src) return;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    *dst = *src;
  }
}

static void FOG_FASTCALL _G2d_TransformF_mapPointsF_Translation(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  float _20 = self._20;
  float _21 = self._21;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x + _20,
             src->y + _21);
  }
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Translation(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  double _20 = self._20;
  double _21 = self._21;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x + _20,
             src->y + _21);
  }
}

static void FOG_FASTCALL _G2d_TransformF_mapPointsF_Scaling(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  float _00 = self._00;
  float _11 = self._11;

  float _20 = self._20;
  float _21 = self._21;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x * _00 + _20,
             src->y * _11 + _21);
  }
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Scaling(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  double _00 = self._00;
  double _11 = self._11;

  double _20 = self._20;
  double _21 = self._21;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x * _00 + _20,
             src->y * _11 + _21);
  }
}

static void FOG_FASTCALL _G2d_TransformF_mapPointsF_Affine(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  float _00 = self._00;
  float _01 = self._01;
  float _10 = self._10;
  float _11 = self._11;
  float _20 = self._20;
  float _21 = self._21;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x * _00 + src->y * _10 + _20,
             src->x * _01 + src->y * _11 + _21);
  }
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Affine(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  double _00 = self._00;
  double _01 = self._01;
  double _10 = self._10;
  double _11 = self._11;
  double _20 = self._20;
  double _21 = self._21;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    dst->set(src->x * _00 + src->y * _10 + _20,
             src->x * _01 + src->y * _11 + _21);
  }
}

static void FOG_FASTCALL _G2d_TransformF_mapPointsF_Projection(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  float _00 = self._00;
  float _01 = self._01;
  float _10 = self._10;
  float _11 = self._11;
  float _20 = self._20;
  float _21 = self._21;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    float _m = src->x * self._02 + src->y * self._12 + self._22;

    if (Math::fzero(_m)) _m = MATH_EPSILON_F;
    _m = 1.0f / _m;

    dst->set((src->x * _00 + src->y * _10 + _20) * _m,
             (src->x * _01 + src->y * _11 + _21) * _m);
  }
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Projection(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  double _00 = self._00;
  double _01 = self._01;
  double _10 = self._10;
  double _11 = self._11;
  double _20 = self._20;
  double _21 = self._21;

  for (sysuint_t i = length; i; i--, dst++, src++)
  {
    double _m = src->x * self._02 + src->y * self._12 + self._22;

    if (Math::fzero(_m)) _m = MATH_EPSILON_D;
    _m = 1.0 / _m;

    dst->set((src->x * _00 + src->y * _10 + _20) * _m,
             (src->x * _01 + src->y * _11 + _21) * _m);
  }
}

// ============================================================================
// [Fog::Transform - MapVector]
// ============================================================================

static void FOG_FASTCALL _G2d_TransformF_mapVectorF(const TransformF& self, PointF& dst, const PointF& src)
{
  uint32_t selfType = self.getType();

  float _x = src.x;
  float _y = src.y;

  float x = _x * self._00 + _y * self._10;
  float y = _x * self._01 + _y * self._11;

  if (selfType >= TRANSFORM_TYPE_PROJECTION)
  {
    float _m0 = (self._22);
    float _m1 = (self._22 + _x * self._02 + _y * self._12);

    if (Math::fzero(_m0)) _m0 = MATH_EPSILON_F;
    if (Math::fzero(_m1)) _m1 = MATH_EPSILON_F;

    _m0 = 1.0f / _m0;
    _m1 = 1.0f / _m1;

    x += self._20;
    y += self._21;

    x *= _m1;
    y *= _m1;

    x -= self._20 * _m0;
    y -= self._21 * _m0;
  }

  dst.set(x, y);
}

static void FOG_FASTCALL _G2d_TransformD_mapVectorD(const TransformD& self, PointD& dst, const PointD& src)
{
  uint32_t selfType = self.getType();

  double _x = src.x;
  double _y = src.y;

  double x = _x * self._00 + _y * self._10;
  double y = _x * self._01 + _y * self._11;

  if (selfType >= TRANSFORM_TYPE_PROJECTION)
  {
    double _m0 = (self._22);
    double _m1 = (self._22 + _x * self._02 + _y * self._12);

    if (Math::fzero(_m0)) _m0 = MATH_EPSILON_D;
    if (Math::fzero(_m1)) _m1 = MATH_EPSILON_D;

    _m0 = 1.0 / _m0;
    _m1 = 1.0 / _m1;

    x += self._20;
    y += self._21;

    x *= _m1;
    y *= _m1;

    x -= self._20 * _m0;
    y -= self._21 * _m0;
  }

  dst.set(x, y);
}

// ============================================================================
// [Fog::Transform - GetScaling]
// ============================================================================

// Notes from AntiGrain about absolute scaling:
//   Used to calculate scaling coefficients in image resampling. When there is 
//   considerable shear this method gives us much better estimation than just 
//   sx, sy.

static PointF FOG_FASTCALL _G2d_TransformF_getScaling(const TransformF& self, bool absolute)
{
  PointF result;

  if (absolute)
  {
    // Absolute scaling.
    result.x = Math::sqrt(self._00 * self._00 + self._10 * self._10);
    result.y = Math::sqrt(self._01 * self._01 + self._11 * self._11);
  }
  else
  {
    // Average scaling.
    TransformF t = self.rotated(-self.getRotation(), MATRIX_ORDER_APPEND);

    result.set(1.0f, 1.0f);
    t.mapVector(result);
  }

  return result;
}

static PointD FOG_FASTCALL _G2d_TransformD_getScaling(const TransformD& self, bool absolute)
{
  PointD result;

  if (absolute)
  {
    // Absolute scaling.
    result.x = Math::sqrt(self._00 * self._00 + self._10 * self._10);
    result.y = Math::sqrt(self._01 * self._01 + self._11 * self._11);
  }
  else
  {
    // Average scaling.
    TransformD t = self.rotated(-self.getRotation(), MATRIX_ORDER_APPEND);

    result.set(1.0, 1.0);
    t.mapVector(result);
  }

  return result;
}

// ============================================================================
// [Fog::Transform - GetRotation]
// ============================================================================

static float FOG_FASTCALL _G2d_TransformF_getRotation(const TransformF& self)
{
  PointF pt(1.0f, 0.0f);
  self.mapVector(pt);
  return Math::atan2(pt.y, pt.x);
}

static double FOG_FASTCALL _G2d_TransformD_getRotation(const TransformD& self)
{
  PointD pt(1.0, 0.0);
  self.mapVector(pt);
  return Math::atan2(pt.y, pt.x);
}

// ============================================================================
// [Fog::Transform - GetAverageScaling]
// ============================================================================

static float FOG_FASTCALL _G2d_TransformF_getAverageScaling(const TransformF& self)
{
  float x = self._00 + self._10;
  float y = self._01 + self._11;
  return Math::sqrt((x * x + y * y) * 0.5f);
}

static double FOG_FASTCALL _G2d_TransformD_getAverageScaling(const TransformD& self)
{
  double x = self._00 + self._10;
  double y = self._01 + self._11;
  return Math::sqrt((x * x + y * y) * 0.5);
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
FOG_INIT_EXTERN void fog_transform_init_3dnow(void);
FOG_INIT_EXTERN void fog_transform_init_sse(void);
FOG_INIT_EXTERN void fog_transform_init_sse2(void);
#endif // FOG_ARCH_X86 || FOG_ARCH_X86_64

FOG_INIT_DECLARE err_t fog_transform_init(void)
{
  using namespace Fog;

  _g2d.transformf.create = _G2d_TransformF_create;
  _g2d.transformf.update = _G2d_TransformF_update;
  _g2d.transformf.transform = _G2d_TransformF_transform;
  _g2d.transformf.transformed = _G2d_TransformF_transformed;
  _g2d.transformf.multiply = _G2d_TransformF_multiply;
  _g2d.transformf.invert = _G2d_TransformF_invert;
  _g2d.transformf.getScaling = _G2d_TransformF_getScaling;
  _g2d.transformf.getRotation = _G2d_TransformF_getRotation;
  _g2d.transformf.getAverageScaling = _G2d_TransformF_getAverageScaling;

  _g2d.transformd.create = _G2d_TransformD_create;
  _g2d.transformd.update = _G2d_TransformD_update;
  _g2d.transformd.transform = _G2d_TransformD_transform;
  _g2d.transformd.transformed = _G2d_TransformD_transformed;
  _g2d.transformd.multiply = _G2d_TransformD_multiply;
  _g2d.transformd.invert = _G2d_TransformD_invert;
  _g2d.transformd.getScaling = _G2d_TransformD_getScaling;
  _g2d.transformd.getRotation = _G2d_TransformD_getRotation;
  _g2d.transformd.getAverageScaling = _G2d_TransformD_getAverageScaling;

  // TODO: SSE implementation required.
  _g2d.transformf.mapPointF = _G2d_TransformF_mapPointF;
  _g2d.transformf.mapVectorF = _G2d_TransformF_mapVectorF;

  _g2d.transformd.mapPointD = _G2d_TransformD_mapPointD;
  _g2d.transformd.mapVectorD = _G2d_TransformD_mapVectorD;

#if defined(FOG_TRANSFORM_INIT_C)

  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_IDENTITY   ] = _G2d_TransformF_mapPointsF_Identity;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_TRANSLATION] = _G2d_TransformF_mapPointsF_Translation;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_SCALING    ] = _G2d_TransformF_mapPointsF_Scaling;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_ROTATION   ] = _G2d_TransformF_mapPointsF_Affine;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_AFFINE     ] = _G2d_TransformF_mapPointsF_Affine;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_PROJECTION ] = _G2d_TransformF_mapPointsF_Projection;

  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_IDENTITY   ] = _G2d_TransformD_mapPointsD_Identity;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_TRANSLATION] = _G2d_TransformD_mapPointsD_Translation;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_SCALING    ] = _G2d_TransformD_mapPointsD_Scaling;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_ROTATION   ] = _G2d_TransformD_mapPointsD_Affine;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_AFFINE     ] = _G2d_TransformD_mapPointsD_Affine;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_PROJECTION ] = _G2d_TransformD_mapPointsD_Projection;
#endif // FOG_TRANSFORM_INIT_C

  // Install 3dNow/SSE/SSE2 optimized code if supported.
#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
#if defined(FOG_HARDCODE_SSE2)
  fog_transform_init_sse();
  fog_transform_init_sse2();
#else
  if (CpuInfo::get()->hasFeature(CPU_FEATURE_3DNOW)) fog_transform_init_3dnow();
  if (CpuInfo::get()->hasFeature(CPU_FEATURE_SSE  )) fog_transform_init_sse();
  if (CpuInfo::get()->hasFeature(CPU_FEATURE_SSE2 )) fog_transform_init_sse2();
#endif // FOG_HARDCODE_SSE2
#endif // FOG_ARCH_X86 || FOG_ARCH_X86_64

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_transform_shutdown(void)
{
}
