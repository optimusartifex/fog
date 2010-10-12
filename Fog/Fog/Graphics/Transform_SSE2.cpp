// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Face/FaceSSE2.h>
#include <Fog/Core/Math.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Transform.h>

namespace Fog {

// ============================================================================
// [Fog::Transform - Helpers]
// ============================================================================

static __m128d _mm_load_pd_unpack(const double* p)
{
  __m128d result = _mm_load_sd(p);
  result = _mm_unpacklo_pd(result, result);
  return result;
}

// ============================================================================
// [Fog::Transform - MapPoint(s)]
// ============================================================================

static void FOG_FASTCALL _G2d_TransformD_mapPointD_SSE2(const TransformD& self, PointD& dst, const PointD& src)
{
  uint32_t selfType = self.getType();

  __m128d m_20_21 = _mm_loadu_pd(&self._20);
  __m128d src0 = _mm_loadu_pd(&src.x);

  switch (selfType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      break;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      __m128d m_00_11 = _mm_setr_pd(self._00, self._11);
      src0 = _mm_mul_pd(src0, m_00_11);
      // ... fall through ...
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      src0 = _mm_add_pd(src0, m_20_21);
      break;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      __m128d m_00_11 = _mm_setr_pd(self._00, self._11);
      __m128d m_10_01 = _mm_setr_pd(self._10, self._01);
      __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));

      src0 = _mm_mul_pd(src0, m_00_11);
      rev0 = _mm_mul_pd(rev0, m_10_01);
      src0 = _mm_add_pd(src0, m_20_21);
      src0 = _mm_add_pd(src0, rev0);
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      __m128d m_00_11 = _mm_setr_pd(self._00, self._11);
      __m128d m_10_01 = _mm_setr_pd(self._10, self._01);

      __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
      __m128d inv0 = _mm_mul_pd(src0, _mm_setr_pd(self._02, self._12));
      __m128d tmp0 = _mm_load_sd(&FOG_SSE_GET_CONST_SD(m128d_sgn_mask));

      // src0.x = (x * _00 + y * _01 + _20)
      // src0.y = (x * _01 + y * _11 + _21)
      src0 = _mm_mul_pd(src0, m_00_11);
      rev0 = _mm_mul_pd(rev0, m_10_01);
      src0 = _mm_add_pd(src0, m_20_21);
      src0 = _mm_add_pd(src0, rev0);

      inv0 = _mm_add_pd(inv0, _mm_shuffle_pd(inv0, inv0, _MM_SHUFFLE2(0, 1)));
      inv0 = _mm_add_sd(inv0, _mm_load_sd(&self._22));

      tmp0 = _mm_and_pd(tmp0, inv0);
      inv0 = _mm_xor_pd(inv0, tmp0);
      inv0 = _mm_max_sd(inv0, _mm_load_sd(&FOG_SSE_GET_CONST_SD(m128d_epsilon)));
      inv0 = _mm_xor_pd(inv0, tmp0);
      inv0 = _mm_div_sd(_mm_load_sd(&FOG_SSE_GET_CONST_SD(m128d_one)), inv0);
      inv0 = _mm_unpacklo_pd(inv0, inv0);

      src0 = _mm_mul_pd(src0, inv0);
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  _mm_storeu_pd(&dst.x, src0);
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Identity_SSE2(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  sysuint_t i;

  if (dst == src) return;

  for (i = length; i; i--, dst++, src++)
  {
    *dst = *src;
  }
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Translation_SSE2(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  sysuint_t i;

  __m128d m_20_21 = _mm_loadu_pd(&self._20);

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0 = _mm_load_pd(&src[0].x);
      __m128d src1 = _mm_load_pd(&src[1].x);
      __m128d src2 = _mm_load_pd(&src[2].x);
      __m128d src3 = _mm_load_pd(&src[3].x);

      src0 = _mm_add_pd(src0, m_20_21);
      src1 = _mm_add_pd(src1, m_20_21);
      src2 = _mm_add_pd(src2, m_20_21);
      src3 = _mm_add_pd(src3, m_20_21);

      _mm_store_pd(&dst[0].x, src0);
      _mm_store_pd(&dst[1].x, src1);
      _mm_store_pd(&dst[2].x, src2);
      _mm_store_pd(&dst[3].x, src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0 = _mm_load_pd(&src[0].x);
      src0 = _mm_add_pd(src0, m_20_21);
      _mm_store_pd(&dst[0].x, src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0 = _mm_loadu_pd(&src[0].x);
      __m128d src1 = _mm_loadu_pd(&src[1].x);
      __m128d src2 = _mm_loadu_pd(&src[2].x);
      __m128d src3 = _mm_loadu_pd(&src[3].x);

      src0 = _mm_add_pd(src0, m_20_21);
      src1 = _mm_add_pd(src1, m_20_21);
      src2 = _mm_add_pd(src2, m_20_21);
      src3 = _mm_add_pd(src3, m_20_21);

      _mm_storeu_pd(&dst[0].x, src0);
      _mm_storeu_pd(&dst[1].x, src1);
      _mm_storeu_pd(&dst[2].x, src2);
      _mm_storeu_pd(&dst[3].x, src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0 = _mm_loadu_pd(&src[0].x);
      src0 = _mm_add_pd(src0, m_20_21);
      _mm_storeu_pd(&dst[0].x, src0);
    }
  }
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Scaling_SSE2(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  sysuint_t i;

  __m128d m_20_21 = _mm_loadu_pd(&self._20);
  __m128d m_00_11 = _mm_setr_pd(self._00, self._11);

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0 = _mm_load_pd(&src[0].x);
      __m128d src1 = _mm_load_pd(&src[1].x);
      __m128d src2 = _mm_load_pd(&src[2].x);
      __m128d src3 = _mm_load_pd(&src[3].x);

      src0 = _mm_mul_pd(src0, m_00_11);
      src1 = _mm_mul_pd(src1, m_00_11);
      src2 = _mm_mul_pd(src2, m_00_11);
      src3 = _mm_mul_pd(src3, m_00_11);

      src0 = _mm_add_pd(src0, m_20_21);
      src1 = _mm_add_pd(src1, m_20_21);
      src2 = _mm_add_pd(src2, m_20_21);
      src3 = _mm_add_pd(src3, m_20_21);

      _mm_store_pd(&dst[0].x, src0);
      _mm_store_pd(&dst[1].x, src1);
      _mm_store_pd(&dst[2].x, src2);
      _mm_store_pd(&dst[3].x, src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0 = _mm_load_pd(&src[0].x);

      src0 = _mm_mul_pd(src0, m_00_11);
      src0 = _mm_add_pd(src0, m_20_21);

      _mm_store_pd(&dst[0].x, src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0 = _mm_loadu_pd(&src[0].x);
      __m128d src1 = _mm_loadu_pd(&src[1].x);
      __m128d src2 = _mm_loadu_pd(&src[2].x);
      __m128d src3 = _mm_loadu_pd(&src[3].x);

      src0 = _mm_mul_pd(src0, m_00_11);
      src1 = _mm_mul_pd(src1, m_00_11);
      src2 = _mm_mul_pd(src2, m_00_11);
      src3 = _mm_mul_pd(src3, m_00_11);

      src0 = _mm_add_pd(src0, m_20_21);
      src1 = _mm_add_pd(src1, m_20_21);
      src2 = _mm_add_pd(src2, m_20_21);
      src3 = _mm_add_pd(src3, m_20_21);

      _mm_storeu_pd(&dst[0].x, src0);
      _mm_storeu_pd(&dst[1].x, src1);
      _mm_storeu_pd(&dst[2].x, src2);
      _mm_storeu_pd(&dst[3].x, src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0 = _mm_loadu_pd(&src[0].x);

      src0 = _mm_mul_pd(src0, m_00_11);
      src0 = _mm_add_pd(src0, m_20_21);

      _mm_storeu_pd(&dst[0].x, src0);
    }
  }
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Affine_SSE2(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  sysuint_t i;

  __m128d m_00_11 = _mm_setr_pd(self._00, self._11);
  __m128d m_10_01 = _mm_setr_pd(self._10, self._01);
  __m128d m_20_21 = _mm_loadu_pd(&self._20);

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0 = _mm_load_pd(&src[0].x);
      __m128d src1 = _mm_load_pd(&src[1].x);
      __m128d src2 = _mm_load_pd(&src[2].x);
      __m128d src3 = _mm_load_pd(&src[3].x);

      __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
      __m128d rev1 = _mm_shuffle_pd(src1, src1, _MM_SHUFFLE2(0, 1));

      src0 = _mm_mul_pd(src0, m_00_11);
      src1 = _mm_mul_pd(src1, m_00_11);
      rev0 = _mm_mul_pd(rev0, m_10_01);
      rev1 = _mm_mul_pd(rev1, m_10_01);
      src0 = _mm_add_pd(src0, m_20_21);
      src1 = _mm_add_pd(src1, m_20_21);
      src0 = _mm_add_pd(src0, rev0);
      src1 = _mm_add_pd(src1, rev1);

      __m128d rev2 = _mm_shuffle_pd(src2, src2, _MM_SHUFFLE2(0, 1));
      __m128d rev3 = _mm_shuffle_pd(src3, src3, _MM_SHUFFLE2(0, 1));

      src2 = _mm_mul_pd(src2, m_00_11);
      src3 = _mm_mul_pd(src3, m_00_11);
      rev2 = _mm_mul_pd(rev2, m_10_01);
      rev3 = _mm_mul_pd(rev3, m_10_01);
      src2 = _mm_add_pd(src2, m_20_21);
      src3 = _mm_add_pd(src3, m_20_21);
      src2 = _mm_add_pd(src2, rev2);
      src3 = _mm_add_pd(src3, rev3);

      _mm_store_pd(&dst[0].x, src0);
      _mm_store_pd(&dst[1].x, src1);
      _mm_store_pd(&dst[2].x, src2);
      _mm_store_pd(&dst[3].x, src3);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0 = _mm_loadu_pd(&src[0].x);
      __m128d src1 = _mm_loadu_pd(&src[1].x);
      __m128d src2 = _mm_loadu_pd(&src[2].x);
      __m128d src3 = _mm_loadu_pd(&src[3].x);

      __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
      __m128d rev1 = _mm_shuffle_pd(src1, src1, _MM_SHUFFLE2(0, 1));

      src0 = _mm_mul_pd(src0, m_00_11);
      src1 = _mm_mul_pd(src1, m_00_11);
      rev0 = _mm_mul_pd(rev0, m_10_01);
      rev1 = _mm_mul_pd(rev1, m_10_01);
      src0 = _mm_add_pd(src0, m_20_21);
      src1 = _mm_add_pd(src1, m_20_21);
      src0 = _mm_add_pd(src0, rev0);
      src1 = _mm_add_pd(src1, rev1);

      __m128d rev2 = _mm_shuffle_pd(src2, src2, _MM_SHUFFLE2(0, 1));
      __m128d rev3 = _mm_shuffle_pd(src3, src3, _MM_SHUFFLE2(0, 1));

      src2 = _mm_mul_pd(src2, m_00_11);
      src3 = _mm_mul_pd(src3, m_00_11);
      rev2 = _mm_mul_pd(rev2, m_10_01);
      rev3 = _mm_mul_pd(rev3, m_10_01);
      src2 = _mm_add_pd(src2, m_20_21);
      src3 = _mm_add_pd(src3, m_20_21);
      src2 = _mm_add_pd(src2, rev2);
      src3 = _mm_add_pd(src3, rev3);

      _mm_storeu_pd(&dst[0].x, src0);
      _mm_storeu_pd(&dst[1].x, src1);
      _mm_storeu_pd(&dst[2].x, src2);
      _mm_storeu_pd(&dst[3].x, src3);
    }
  }

  for (i = length & 3; i; i--, dst++, src++)
  {
    __m128d src0 = _mm_loadu_pd(&src[0].x);
    __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));

    src0 = _mm_mul_pd(src0, m_00_11);
    rev0 = _mm_mul_pd(rev0, m_10_01);
    src0 = _mm_add_pd(src0, m_20_21);
    src0 = _mm_add_pd(src0, rev0);

    _mm_storeu_pd(&dst[0].x, src0);
  }
}

static void FOG_FASTCALL _G2d_TransformD_mapPointsD_Projection_SSE2(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length)
{
  sysuint_t i;

  if (length >= 2)
  {
    __m128d m_00_00 = _mm_load_pd_unpack(&self._00);
    __m128d m_01_01 = _mm_load_pd_unpack(&self._01);
    __m128d m_02_02 = _mm_load_pd_unpack(&self._02);

    __m128d m_10_10 = _mm_load_pd_unpack(&self._10);
    __m128d m_11_11 = _mm_load_pd_unpack(&self._11);
    __m128d m_12_12 = _mm_load_pd_unpack(&self._12);

    __m128d m_20_20 = _mm_load_pd_unpack(&self._20);
    __m128d m_21_21 = _mm_load_pd_unpack(&self._21);
    __m128d m_22_22 = _mm_load_pd_unpack(&self._22);

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      __m128d src0xx, src1xx;
      __m128d src0yy, src1yy;

      __m128d rcp0;

      // src0xx = [x0, x1]
      // src0yy = [y0, y1]
      src0xx = _mm_load_sd(&src[0].x);
      src0yy = _mm_load_sd(&src[0].y);

      src0xx = _mm_loadh_pd(src0xx, &src[1].x);
      src0yy = _mm_loadh_pd(src0yy, &src[1].y);

      src1xx = src0xx;
      src1yy = src0yy;

      // rcp0 = 1.0 / (x * _02 + y * _12 + _22)
      src1xx = _mm_mul_pd(src1xx, m_02_02);
      src1yy = _mm_mul_pd(src1yy, m_12_12);
      src1xx = _mm_add_pd(src1xx, m_22_22);
      src1xx = _mm_add_pd(src1xx, src1yy);

      rcp0   = FOG_SSE_GET_CONST_PD(m128d_sgn_mask);
      rcp0   = _mm_and_pd(rcp0, src1xx);
      src1xx = _mm_and_pd(src1xx, FOG_SSE_GET_CONST_PD(m128d_num_mask));
      rcp0   = _mm_or_pd(rcp0, FOG_SSE_GET_CONST_PD(m128d_one));
      src1xx = _mm_max_pd(src1xx, FOG_SSE_GET_CONST_PD(m128d_epsilon));
      rcp0   = _mm_div_pd(rcp0, src1xx);

      // src0xx = (x * _00 + y * _10 + _20) * rcp0
      // src0yy = (x * _01 + y * _11 + _21) * rcp0
      src1xx = src0xx;
      src1yy = src0yy;

      src0xx = _mm_mul_pd(src0xx, m_00_00);
      src0yy = _mm_mul_pd(src0yy, m_11_11);

      src1xx = _mm_mul_pd(src1xx, m_01_01);
      src1yy = _mm_mul_pd(src1yy, m_10_10);

      src0xx = _mm_add_pd(src0xx, m_20_20);
      src0yy = _mm_add_pd(src0yy, m_21_21);

      src0xx = _mm_add_pd(src0xx, src1yy);
      src0yy = _mm_add_pd(src0yy, src1xx);

      src0xx = _mm_mul_pd(src0xx, rcp0);
      src0yy = _mm_mul_pd(src0yy, rcp0);

      _mm_storel_pd(&dst[0].x, src0xx);
      _mm_storel_pd(&dst[0].y, src0yy);
      _mm_storeh_pd(&dst[1].x, src0xx);
      _mm_storeh_pd(&dst[1].y, src0yy);
    }
  }

  if (length & 1)
  {
    __m128d m_00_11 = _mm_setr_pd(self._00, self._11);
    __m128d m_10_01 = _mm_setr_pd(self._10, self._01);
    __m128d m_20_21 = _mm_loadu_pd(&self._20);

    __m128d src0 = _mm_loadu_pd(&src[0].x);
    __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
    __m128d inv0 = _mm_mul_pd(src0, _mm_setr_pd(self._02, self._12));
    __m128d tmp0 = _mm_load_sd(&FOG_SSE_GET_CONST_SD(m128d_sgn_mask));

    // src0.x = (x * _00 + y * _10 + _20)
    // src0.y = (x * _01 + y * _11 + _21)
    src0 = _mm_mul_pd(src0, m_00_11);
    rev0 = _mm_mul_pd(rev0, m_10_01);
    src0 = _mm_add_pd(src0, m_20_21);
    src0 = _mm_add_pd(src0, rev0);

    inv0 = _mm_add_pd(inv0, _mm_shuffle_pd(inv0, inv0, _MM_SHUFFLE2(0, 1)));
    inv0 = _mm_add_sd(inv0, _mm_load_sd(&self._22));

    tmp0 = _mm_and_pd(tmp0, inv0);
    inv0 = _mm_xor_pd(inv0, tmp0);
    inv0 = _mm_max_sd(inv0, _mm_load_sd(&FOG_SSE_GET_CONST_SD(m128d_epsilon)));
    inv0 = _mm_xor_pd(inv0, tmp0);
    inv0 = _mm_div_sd(_mm_load_sd(&FOG_SSE_GET_CONST_SD(m128d_one)), inv0);
    inv0 = _mm_unpacklo_pd(inv0, inv0);

    src0 = _mm_mul_pd(src0, inv0);
    _mm_storeu_pd(&dst[0].x, src0);
  }
}

// ============================================================================
// [Fog::Transform - MapVector]
// ============================================================================

static void FOG_FASTCALL _G2d_TransformD_mapVectorD_SSE2(const TransformD& self, PointD& dst, const PointD& src)
{
  uint32_t selfType = self.getType();

  __m128d src0 = _mm_loadu_pd(&src.x);
  __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));

  __m128d m_00_11 = _mm_setr_pd(self._00, self._11);
  __m128d m_10_01 = _mm_setr_pd(self._10, self._01);

  if (selfType < TRANSFORM_TYPE_PROJECTION)
  {
    src0 = _mm_mul_pd(src0, m_00_11);
    rev0 = _mm_mul_pd(rev0, m_10_01);
    src0 = _mm_add_pd(src0, rev0);
  }
  else
  {
    __m128d rcp0 = _mm_setr_pd(self._02, self._12);
    __m128d rcpX;
      
    rcp0 = _mm_mul_pd(rcp0, src0);
    rcpX = _mm_shuffle_pd(rcp0, rcp0, _MM_SHUFFLE2(0, 1));

    src0 = _mm_mul_pd(src0, m_00_11);
    rev0 = _mm_mul_pd(rev0, m_10_01);
    src0 = _mm_add_pd(src0, rev0);

    rcp0 = _mm_add_pd(rcp0, rcpX);
    rcp0 = _mm_add_sd(rcp0, _mm_load_sd(&self._22));
    rcp0 = _mm_loadh_pd(rcp0, &self._22);

    rcpX = FOG_SSE_GET_CONST_PD(m128d_sgn_mask);
    rcpX = _mm_and_pd(rcpX, rcp0);
    rcp0 = _mm_xor_pd(rcp0, rcpX);
    rcp0 = _mm_max_pd(rcp0, FOG_SSE_GET_CONST_PD(m128d_epsilon));
    rcp0 = _mm_xor_pd(rcp0, rcpX);
    rcp0 = _mm_div_pd(FOG_SSE_GET_CONST_PD(m128d_one), rcp0);

    rcpX = _mm_loadu_pd(&self._20);
    src0 = _mm_add_pd(src0, rcpX);

    src0 = _mm_mul_pd(src0, _mm_unpacklo_pd(rcp0, rcp0));
    src0 = _mm_sub_pd(src0, _mm_mul_pd(rcpX, _mm_unpackhi_pd(rcp0, rcp0)));
  }

  _mm_storeu_pd(&dst.x, src0);
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE void fog_transform_init_sse2(void)
{
  using namespace Fog;

  _g2d.transformd.mapPointD = _G2d_TransformD_mapPointD_SSE2;
  _g2d.transformd.mapVectorD = _G2d_TransformD_mapVectorD_SSE2;

  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_IDENTITY   ] = _G2d_TransformD_mapPointsD_Identity_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_TRANSLATION] = _G2d_TransformD_mapPointsD_Translation_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_SCALING    ] = _G2d_TransformD_mapPointsD_Scaling_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_ROTATION   ] = _G2d_TransformD_mapPointsD_Affine_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_AFFINE     ] = _G2d_TransformD_mapPointsD_Affine_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_PROJECTION ] = _G2d_TransformD_mapPointsD_Projection_SSE2;
}
