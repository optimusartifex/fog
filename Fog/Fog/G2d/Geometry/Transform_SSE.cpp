// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Face/Face_SSE.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Transform - MapPoint(s)]
// ============================================================================

static void FOG_CDECL _G2d_TransformF_mapPointsF_Identity_SSE(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  sysuint_t i;

  if (dst == src) return;

  if (((sysuint_t)dst & 0xF) != 0)
  {
    Face::m128f src0;
    Face::m128fZero(src0);
    if (length == 0) return;

    Face::m128fLoad8Lo(src0, src);
    Face::m128fStore8Lo(dst, src0);

    dst += 1;
    src += 1;
    length--;
  }

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 3; i; i--, dst += 8, src += 8)
    {
      Face::m128f src01, src23;
      Face::m128f src45, src67;

      Face::m128fLoad16a(src01, src + 0);
      Face::m128fLoad16a(src23, src + 2);
      Face::m128fLoad16a(src45, src + 4);
      Face::m128fLoad16a(src67, src + 6);

      Face::m128fStore16a(dst + 0, src01);
      Face::m128fStore16a(dst + 2, src23);
      Face::m128fStore16a(dst + 4, src45);
      Face::m128fStore16a(dst + 6, src67);
    }

    length &= 7;
  }

  for (i = length; i; i--, dst++, src++)
  {
    *dst = *src;
  }
}

static void FOG_CDECL _G2d_TransformF_mapPointsF_Translation_SSE(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  sysuint_t i;
  Face::m128f m_20_21_20_21 = _mm_setr_ps(self._20, self._21, self._20, self._21);

  if (((sysuint_t)dst & 0xF) != 0)
  {
    Face::m128f src0;
    Face::m128fZero(src0);
    if (length == 0) return;

    Face::m128fLoad8Lo(src0, src);
    Face::m128fAddPS(src0, src0, m_20_21_20_21);
    Face::m128fStore8Lo(dst, src0);

    dst += 1;
    src += 1;
    length--;
  }

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 3; i; i--, dst += 8, src += 8)
    {
      Face::m128f src01, src23;
      Face::m128f src45, src67;

      Face::m128fLoad16a(src01, src + 0);
      Face::m128fLoad16a(src23, src + 2);
      Face::m128fLoad16a(src45, src + 4);
      Face::m128fLoad16a(src67, src + 6);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src23, src23, m_20_21_20_21);
      Face::m128fAddPS(src45, src45, m_20_21_20_21);
      Face::m128fAddPS(src67, src67, m_20_21_20_21);

      Face::m128fStore16a(dst + 0, src01);
      Face::m128fStore16a(dst + 2, src23);
      Face::m128fStore16a(dst + 4, src45);
      Face::m128fStore16a(dst + 6, src67);
    }

    length &= 7;

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01;

      Face::m128fLoad16a(src01, src);
      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fStore16a(dst, src01);
    }
  }
  else
  {
    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01;

      Face::m128fLoad16u(src01, src);
      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fStore16u(dst, src01);
    }
  }

  if (length & 1)
  {
    Face::m128f src0;
    Face::m128fZero(src0);

    Face::m128fLoad8Lo(src0, src);
    Face::m128fAddPS(src0, src0, m_20_21_20_21);
    Face::m128fStore8Lo(dst, src0);
  }
}

static void FOG_CDECL _G2d_TransformF_mapPointsF_Scaling_SSE(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  sysuint_t i;

  Face::m128f m_00_11_00_11 = _mm_setr_ps(self._00, self._11, self._00, self._11);
  Face::m128f m_20_21_20_21 = _mm_setr_ps(self._20, self._21, self._20, self._21);

  if (((sysuint_t)dst & 0xF) != 0)
  {
    Face::m128f src0;

    Face::m128fZero(src0);
    if (length == 0) return;

    Face::m128fLoad8Lo(src0, src);
    Face::m128fMulPS(src0, src0, m_00_11_00_11);
    Face::m128fAddPS(src0, src0, m_20_21_20_21);
    Face::m128fStore8Lo(dst, src0);

    dst += 1;
    src += 1;
    length--;
  }

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 3; i; i--, dst += 8, src += 8)
    {
      Face::m128f src01, src23;
      Face::m128f src45, src67;

      Face::m128fLoad16a(src01, src + 0);
      Face::m128fLoad16a(src23, src + 2);
      Face::m128fLoad16a(src45, src + 4);
      Face::m128fLoad16a(src67, src + 6);

      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fMulPS(src23, src23, m_00_11_00_11);
      Face::m128fMulPS(src45, src45, m_00_11_00_11);
      Face::m128fMulPS(src67, src67, m_00_11_00_11);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src23, src23, m_20_21_20_21);
      Face::m128fAddPS(src45, src45, m_20_21_20_21);
      Face::m128fAddPS(src67, src67, m_20_21_20_21);

      Face::m128fStore16a(dst + 0, src01);
      Face::m128fStore16a(dst + 2, src23);
      Face::m128fStore16a(dst + 4, src45);
      Face::m128fStore16a(dst + 6, src67);
    }

    length &= 7;

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01;

      Face::m128fLoad16a(src01, src + 0);
      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fStore16a(dst + 0, src01);
    }
  }
  else
  {
    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01;

      Face::m128fLoad16u(src01, src + 0);
      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fStore16u(dst + 0, src01);
    }
  }

  if (length & 1)
  {
    Face::m128f src0;

    Face::m128fZero(src0);
    Face::m128fLoad8Lo(src0, src);
    Face::m128fMulPS(src0, src0, m_00_11_00_11);
    Face::m128fAddPS(src0, src0, m_20_21_20_21);
    Face::m128fStore8Lo(dst, src0);
  }
}

static void FOG_CDECL _G2d_TransformF_mapPointsF_Swap_SSE(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  sysuint_t i;

  Face::m128f m_01_10_01_10 = _mm_setr_ps(self._01, self._10, self._01, self._10);
  Face::m128f m_20_21_20_21 = _mm_setr_ps(self._20, self._21, self._20, self._21);

  if (((sysuint_t)dst & 0xF) != 0)
  {
    Face::m128f src0;

    Face::m128fZero(src0);
    if (length == 0) return;

    Face::m128fLoad8Lo(src0, src);
    Face::m128fSwapXY(src0, src0);
    Face::m128fMulPS(src0, src0, m_01_10_01_10);
    Face::m128fAddPS(src0, src0, m_20_21_20_21);
    Face::m128fStore8Lo(dst, src0);

    dst += 1;
    src += 1;
    length--;
  }

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 3; i; i--, dst += 8, src += 8)
    {
      Face::m128f src01, src23;
      Face::m128f src45, src67;

      Face::m128fLoad16a(src01, src + 0);
      Face::m128fLoad16a(src23, src + 2);
      Face::m128fLoad16a(src45, src + 4);
      Face::m128fLoad16a(src67, src + 6);

      Face::m128fSwapXY(src01, src01);
      Face::m128fSwapXY(src23, src23);

      Face::m128fMulPS(src01, src01, m_01_10_01_10);
      Face::m128fMulPS(src23, src23, m_01_10_01_10);

      Face::m128fSwapXY(src45, src45);
      Face::m128fSwapXY(src67, src67);

      Face::m128fMulPS(src45, src45, m_01_10_01_10);
      Face::m128fMulPS(src67, src67, m_01_10_01_10);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src23, src23, m_20_21_20_21);
      Face::m128fAddPS(src45, src45, m_20_21_20_21);
      Face::m128fAddPS(src67, src67, m_20_21_20_21);

      Face::m128fStore16a(dst + 0, src01);
      Face::m128fStore16a(dst + 2, src23);
      Face::m128fStore16a(dst + 4, src45);
      Face::m128fStore16a(dst + 6, src67);
    }

    length &= 7;

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01;

      Face::m128fLoad16a(src01, src + 0);
      Face::m128fSwapXY(src01, src01);
      Face::m128fMulPS(src01, src01, m_01_10_01_10);
      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fStore16a(dst + 0, src01);
    }
  }
  else
  {
    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01;

      Face::m128fLoad16u(src01, src + 0);
      Face::m128fSwapXY(src01, src01);
      Face::m128fMulPS(src01, src01, m_01_10_01_10);
      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fStore16u(dst + 0, src01);
    }
  }

  if (length & 1)
  {
    Face::m128f src0;

    Face::m128fZero(src0);
    Face::m128fLoad8Lo(src0, src);
    Face::m128fSwapXY(src0, src0);
    Face::m128fMulPS(src0, src0, m_01_10_01_10);
    Face::m128fAddPS(src0, src0, m_20_21_20_21);
    Face::m128fStore8Lo(dst, src0);
  }
}

static void FOG_CDECL _G2d_TransformF_mapPointsF_Affine_SSE(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  sysuint_t i;

  Face::m128f m_00_11_00_11 = _mm_setr_ps(self._00, self._11, self._00, self._11);
  Face::m128f m_10_01_10_01 = _mm_setr_ps(self._10, self._01, self._10, self._01);
  Face::m128f m_20_21_20_21 = _mm_setr_ps(self._20, self._21, self._20, self._21);

  if (((sysuint_t)dst & 0xF) != 0)
  {
    Face::m128f src0, tmp0;

    Face::m128fZero(src0);
    if (length == 0) return;

    Face::m128fLoad8Lo(src0, src);
    Face::m128fSwapXY(tmp0, src0);

    Face::m128fMulPS(src0, src0, m_00_11_00_11);
    Face::m128fMulPS(tmp0, tmp0, m_10_01_10_01);

    Face::m128fAddPS(src0, src0, m_20_21_20_21);
    Face::m128fAddPS(src0, src0, tmp0);

    Face::m128fStore8Lo(dst, src0);

    dst += 1;
    src += 1;
    length--;
  }

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 3; i; i--, dst += 8, src += 8)
    {
      Face::m128f src01, src23;
      Face::m128f tmp01, tmp23;

      // DST[0-3] <- SRC[0-3].
      Face::m128fLoad16a(src01, src + 0);
      Face::m128fLoad16a(src23, src + 2);
      Face::m128fSwapXY(tmp01, src01);
      Face::m128fSwapXY(tmp23, src23);

      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fMulPS(src23, src23, m_00_11_00_11);
      Face::m128fMulPS(tmp01, tmp01, m_10_01_10_01);
      Face::m128fMulPS(tmp23, tmp23, m_10_01_10_01);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src23, src23, m_20_21_20_21);
      Face::m128fAddPS(src01, src01, tmp01);
      Face::m128fAddPS(src23, src23, tmp23);

      Face::m128fStore16a(dst + 0, src01);
      Face::m128fStore16a(dst + 2, src23);

      // DST[4-7] <- SRC[4-7].
      Face::m128fLoad16a(src01, src + 4);
      Face::m128fLoad16a(src23, src + 6);
      Face::m128fSwapXY(tmp01, src01);
      Face::m128fSwapXY(tmp23, src23);

      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fMulPS(src23, src23, m_00_11_00_11);
      Face::m128fMulPS(tmp01, tmp01, m_10_01_10_01);
      Face::m128fMulPS(tmp23, tmp23, m_10_01_10_01);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src23, src23, m_20_21_20_21);
      Face::m128fAddPS(src01, src01, tmp01);
      Face::m128fAddPS(src23, src23, tmp23);

      Face::m128fStore16a(dst + 4, src01);
      Face::m128fStore16a(dst + 6, src23);
    }

    length &= 7;

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01, tmp01;

      Face::m128fLoad16a(src01, src);
      Face::m128fSwapXY(tmp01, src01);

      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fMulPS(tmp01, tmp01, m_10_01_10_01);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src01, src01, tmp01);

      Face::m128fStore16a(dst, src01);
    }
  }
  else
  {
    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01, tmp01;

      Face::m128fLoad16u(src01, src);
      Face::m128fSwapXY(tmp01, src01);

      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fMulPS(tmp01, tmp01, m_10_01_10_01);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src01, src01, tmp01);

      Face::m128fStore16u(dst, src01);
    }
  }

  if (length & 1)
  {
    Face::m128f src0, tmp0;

    Face::m128fZero(src0);
    Face::m128fLoad8Lo(src0, src);
    Face::m128fSwapXY(tmp0, src0);

    Face::m128fMulPS(src0, src0, m_00_11_00_11);
    Face::m128fMulPS(tmp0, tmp0, m_10_01_10_01);

    Face::m128fAddPS(src0, src0, m_20_21_20_21);
    Face::m128fAddPS(src0, src0, tmp0);

    Face::m128fStore8Lo(dst, src0);
  }
}

static void FOG_CDECL _G2d_TransformF_mapPointsF_Projection_SSE(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  sysuint_t i;

  Face::m128f m_00_11_00_11 = _mm_setr_ps(self._00, self._11, self._00, self._11);
  Face::m128f m_10_01_10_01 = _mm_setr_ps(self._10, self._01, self._10, self._01);
  Face::m128f m_20_21_20_21 = _mm_setr_ps(self._20, self._21, self._20, self._21);

  Face::m128f m_02_02_02_02 = _mm_set1_ps(self._02);
  Face::m128f m_12_12_12_12 = _mm_set1_ps(self._12);
  Face::m128f m_22_22_22_22 = _mm_set1_ps(self._22);

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128f src01, src23;
      Face::m128f tmp01, tmp23;
      Face::m128f rcp01, rcp23;

      Face::m128fLoad16a(src01, src + 0);
      Face::m128fLoad16a(src23, src + 2);

      Face::m128fShuffle<0, 2, 0, 2>(rcp01, src01, src23);
      Face::m128fShuffle<1, 3, 1, 3>(rcp23, src01, src23);

      Face::m128fMulPS(rcp01, rcp01, m_02_02_02_02);
      Face::m128fMulPS(rcp23, rcp23, m_12_12_12_12);
      Face::m128fAddPS(rcp01, rcp01, m_22_22_22_22);
      Face::m128fAddPS(rcp01, rcp01, rcp23);

      Face::m128fEpsilonPS(rcp01, rcp01);
      Face::m128fRcpPS(rcp01, rcp01);

      Face::m128fSwapXY(tmp01, src01);
      Face::m128fSwapXY(tmp23, src23);

      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fMulPS(src23, src23, m_00_11_00_11);
      Face::m128fMulPS(tmp01, tmp01, m_10_01_10_01);
      Face::m128fMulPS(tmp23, tmp23, m_10_01_10_01);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src23, src23, m_20_21_20_21);
      Face::m128fAddPS(src01, src01, tmp01);
      Face::m128fAddPS(src23, src23, tmp23);

      Face::m128fShuffle<3, 3, 2, 2>(rcp23, rcp01, rcp01);
      Face::m128fShuffle<1, 1, 0, 0>(rcp01, rcp01, rcp01);

      Face::m128fMulPS(src01, src01, rcp01);
      Face::m128fMulPS(src23, src23, rcp23);

      Face::m128fStore16a(dst + 0, src01);
      Face::m128fStore16a(dst + 2, src23);
    }

    length &= 3;

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01;
      Face::m128f tmp01;
      Face::m128f rcp01;

      Face::m128fLoad16a(src01, src);

      Face::m128fShuffle<2, 2, 0, 0>(rcp01, src01, src01);
      Face::m128fShuffle<3, 3, 1, 1>(tmp01, src01, src01);

      Face::m128fMulPS(rcp01, rcp01, m_02_02_02_02);
      Face::m128fMulPS(tmp01, tmp01, m_12_12_12_12);
      Face::m128fAddPS(rcp01, rcp01, m_22_22_22_22);
      Face::m128fAddPS(rcp01, rcp01, tmp01);

      Face::m128fEpsilonPS(rcp01, rcp01);
      Face::m128fRcpPS(rcp01, rcp01);
      Face::m128fShuffle<2, 2, 0, 0>(rcp01, rcp01, rcp01);

      Face::m128fSwapXY(tmp01, src01);
      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fMulPS(tmp01, tmp01, m_10_01_10_01);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src01, src01, tmp01);
      Face::m128fMulPS(src01, src01, rcp01);

      Face::m128fStore16a(dst, src01);
    }
  }
  else
  {
    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128f src01;
      Face::m128f tmp01;
      Face::m128f rcp01;

      Face::m128fLoad16u(src01, src);

      Face::m128fShuffle<2, 2, 0, 0>(rcp01, src01, src01);
      Face::m128fShuffle<3, 3, 1, 1>(tmp01, src01, src01);

      Face::m128fMulPS(rcp01, rcp01, m_02_02_02_02);
      Face::m128fMulPS(tmp01, tmp01, m_12_12_12_12);
      Face::m128fAddPS(rcp01, rcp01, m_22_22_22_22);
      Face::m128fAddPS(rcp01, rcp01, tmp01);

      Face::m128fEpsilonPS(rcp01, rcp01);
      Face::m128fRcpPS(rcp01, rcp01);
      Face::m128fShuffle<2, 2, 0, 0>(rcp01, rcp01, rcp01);

      Face::m128fSwapXY(tmp01, src01);
      Face::m128fMulPS(src01, src01, m_00_11_00_11);
      Face::m128fMulPS(tmp01, tmp01, m_10_01_10_01);

      Face::m128fAddPS(src01, src01, m_20_21_20_21);
      Face::m128fAddPS(src01, src01, tmp01);
      Face::m128fMulPS(src01, src01, rcp01);

      Face::m128fStore16u(dst, src01);
    }
  }

  if (length & 1)
  {
    Face::m128f src01;
    Face::m128f tmp01;
    Face::m128f rcp01;

    Face::m128fZero(src01);
    Face::m128fLoad8Lo(src01, src);

    Face::m128fShuffle<2, 2, 0, 0>(rcp01, src01);
    Face::m128fShuffle<3, 3, 1, 1>(tmp01, src01);

    Face::m128fMulSS(rcp01, rcp01, m_02_02_02_02);
    Face::m128fMulSS(tmp01, tmp01, m_12_12_12_12);
    Face::m128fAddSS(rcp01, rcp01, m_22_22_22_22);
    Face::m128fAddSS(rcp01, rcp01, tmp01);

    Face::m128fEpsilonSS(rcp01, rcp01);
    Face::m128fRcpSS(rcp01, rcp01);
    Face::m128fExtendSS(rcp01, rcp01);

    Face::m128fSwapXY(tmp01, src01);
    Face::m128fMulPS(src01, src01, m_00_11_00_11);
    Face::m128fMulPS(tmp01, tmp01, m_10_01_10_01);
    Face::m128fAddPS(src01, src01, m_20_21_20_21);
    Face::m128fAddPS(src01, src01, tmp01);
    Face::m128fMulPS(src01, src01, rcp01);

    Face::m128fStore8Lo(dst, src01);
  }
}

static void FOG_CDECL _G2d_TransformF_mapPointsF_Degenerate_SSE(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length)
{
  sysuint_t i;

  Face::m128f zero;
  Face::m128fZero(zero);

  if (((sysuint_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4)
    {
      Face::m128fStore16a(dst + 0, zero);
      Face::m128fStore16a(dst + 2, zero);
    }

    length &= 3;

    for (i = length >> 1; i; i--, dst += 2)
    {
      Face::m128fStore16a(dst, zero);
    }

    if (length & 1)
    {
      Face::m128fStore8Lo(dst, zero);
    }
  }
  else
  {
    for (i = length; i; i--, dst += 1)
    {
      Face::m128fStore8Lo(dst, zero);
    }
  }
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_transform_init_sse(void)
{
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_IDENTITY   ] = _G2d_TransformF_mapPointsF_Identity_SSE;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_TRANSLATION] = _G2d_TransformF_mapPointsF_Translation_SSE;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_SCALING    ] = _G2d_TransformF_mapPointsF_Scaling_SSE;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_SWAP       ] = _G2d_TransformF_mapPointsF_Swap_SSE;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_ROTATION   ] = _G2d_TransformF_mapPointsF_Affine_SSE;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_AFFINE     ] = _G2d_TransformF_mapPointsF_Affine_SSE;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_PROJECTION ] = _G2d_TransformF_mapPointsF_Projection_SSE;
  _g2d.transformf.mapPointsF[TRANSFORM_TYPE_DEGENERATE ] = _G2d_TransformF_mapPointsF_Degenerate_SSE;
}

} // Fog namespace
