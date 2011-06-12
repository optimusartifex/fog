// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Face/Face_SSE2.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Transform - MapPoint(s)]
// ============================================================================

static void FOG_CDECL _G2d_TransformD_mapPointD_SSE2(const TransformD& self, PointD& dst, const PointD& src)
{
  uint32_t selfType = self.getType();

  Face::m128d m_20_21;
  Face::m128d src0;

  Face::m128dLoad16u(m_20_21, &self._20);
  Face::m128dLoad16u(src0, &src);

  switch (selfType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      break;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);
      src0 = _mm_mul_pd(src0, m_00_11);
      // ... Fall through ...
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      Face::m128dAddPD(src0, src0, m_20_21);
      break;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      Face::m128dSwapPD(src0, src0);
      Face::m128dMulPD(src0, src0, _mm_setr_pd(self._10, self._01));
      Face::m128dAddPD(src0, src0, m_20_21);
      break;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);
      Face::m128d m_10_01 = _mm_setr_pd(self._10, self._01);
      Face::m128d rev0;

      Face::m128dSwapPD(rev0, src0);

      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(rev0, rev0, m_10_01);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src0, src0, rev0);
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);
      Face::m128d m_10_01 = _mm_setr_pd(self._10, self._01);

      Face::m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
      Face::m128d inv0 = _mm_mul_pd(src0, _mm_setr_pd(self._02, self._12));

      Face::m128dAddPD(inv0, inv0, _mm_shuffle_pd(inv0, inv0, _MM_SHUFFLE2(0, 1)));
      Face::m128dAddSD(inv0, inv0, _mm_load_sd(&self._22));

      Face::m128dEpsilonSD(inv0, inv0);
      Face::m128dRcpSD(inv0, inv0);
      Face::m128dExtendLo(inv0, inv0);

      // src0.x = (x * _00 + y * _01 + _20)
      // src0.y = (x * _01 + y * _11 + _21)
      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(rev0, rev0, m_10_01);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src0, src0, rev0);

      Face::m128dMulPD(src0, src0, inv0);
      break;
    }

    case TRANSFORM_TYPE_DEGENERATE:
    {
      src0 = _mm_setzero_pd();
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  Face::m128dStore16u(&dst, src0);
}

static void FOG_CDECL _G2d_TransformD_mapPointsF_Identity_SSE2(const TransformD& self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Face::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Face::m128dLoad8CvtFrom2xPS(src3, &src[3]);
      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsD_Identity_SSE2(const TransformD& self, PointD* dst, const PointD* src, size_t length)
{
  if (dst == src) return;

  size_t i;

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad16a(src0, &src[0]);
      Face::m128dLoad16a(src1, &src[1]);
      Face::m128dLoad16a(src2, &src[2]);
      Face::m128dLoad16a(src3, &src[3]);
      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad16a(src0, &src[0]);
      Face::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length; i; i--, dst++, src++)
    {
      Face::m128i src0X;
      Face::m128i src0Y;

      Face::m128iLoad8(src0X, &src[0].x);
      Face::m128iLoad8(src0Y, &src[0].y);
      Face::m128iStore8(&dst[0].x, src0X);
      Face::m128iStore8(&dst[0].y, src0Y);
    }
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsF_Translation_SSE2(const TransformD& self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  Face::m128d m_20_21 = _mm_loadu_pd(&self._20);

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Face::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Face::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Face::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Face::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16u(&dst[0], src0);
      Face::m128dStore16u(&dst[1], src1);
      Face::m128dStore16u(&dst[2], src2);
      Face::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsD_Translation_SSE2(const TransformD& self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  Face::m128d m_20_21 = _mm_loadu_pd(&self._20);

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad16a(src0, &src[0]);
      Face::m128dLoad16a(src1, &src[1]);
      Face::m128dLoad16a(src2, &src[2]);
      Face::m128dLoad16a(src3, &src[3]);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad16a(src0, &src[0]);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad16u(src0, &src[0]);
      Face::m128dLoad16u(src1, &src[1]);
      Face::m128dLoad16u(src2, &src[2]);
      Face::m128dLoad16u(src3, &src[3]);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16u(&dst[0], src0);
      Face::m128dStore16u(&dst[1], src1);
      Face::m128dStore16u(&dst[2], src2);
      Face::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad16u(src0, &src[0]);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsF_Scaling_SSE2(const TransformD& self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  Face::m128d m_20_21 = _mm_loadu_pd(&self._20);
  Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Face::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Face::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(src1, src1, m_00_11);
      Face::m128dMulPD(src2, src2, m_00_11);
      Face::m128dMulPD(src3, src3, m_00_11);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Face::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Face::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(src1, src1, m_00_11);
      Face::m128dMulPD(src2, src2, m_00_11);
      Face::m128dMulPD(src3, src3, m_00_11);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16u(&dst[0], src0);
      Face::m128dStore16u(&dst[1], src1);
      Face::m128dStore16u(&dst[2], src2);
      Face::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsD_Scaling_SSE2(const TransformD& self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  Face::m128d m_20_21 = _mm_loadu_pd(&self._20);
  Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad16a(src0, &src[0]);
      Face::m128dLoad16a(src1, &src[1]);
      Face::m128dLoad16a(src2, &src[2]);
      Face::m128dLoad16a(src3, &src[3]);

      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(src1, src1, m_00_11);
      Face::m128dMulPD(src2, src2, m_00_11);
      Face::m128dMulPD(src3, src3, m_00_11);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad16a(src0, &src[0]);
      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad16u(src0, &src[0]);
      Face::m128dLoad16u(src1, &src[1]);
      Face::m128dLoad16u(src2, &src[2]);
      Face::m128dLoad16u(src3, &src[3]);

      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(src1, src1, m_00_11);
      Face::m128dMulPD(src2, src2, m_00_11);
      Face::m128dMulPD(src3, src3, m_00_11);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16u(&dst[0], src0);
      Face::m128dStore16u(&dst[1], src1);
      Face::m128dStore16u(&dst[2], src2);
      Face::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad16u(src0, &src[0]);
      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsF_Swap_SSE2(const TransformD& self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  Face::m128d m_20_21 = _mm_loadu_pd(&self._20);
  Face::m128d m_01_10 = _mm_setr_pd(self._01, self._10);

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Face::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Face::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Face::m128dSwapPD(src0, src0);
      Face::m128dSwapPD(src1, src1);
      Face::m128dSwapPD(src2, src2);
      Face::m128dSwapPD(src3, src3);

      Face::m128dMulPD(src0, src0, m_01_10);
      Face::m128dMulPD(src1, src1, m_01_10);
      Face::m128dMulPD(src2, src2, m_01_10);
      Face::m128dMulPD(src3, src3, m_01_10);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dSwapPD(src0, src0);
      Face::m128dMulPD(src0, src0, m_01_10);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Face::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Face::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Face::m128dSwapPD(src0, src0);
      Face::m128dSwapPD(src1, src1);
      Face::m128dSwapPD(src2, src2);
      Face::m128dSwapPD(src3, src3);

      Face::m128dMulPD(src0, src0, m_01_10);
      Face::m128dMulPD(src1, src1, m_01_10);
      Face::m128dMulPD(src2, src2, m_01_10);
      Face::m128dMulPD(src3, src3, m_01_10);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16u(&dst[0], src0);
      Face::m128dStore16u(&dst[1], src1);
      Face::m128dStore16u(&dst[2], src2);
      Face::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dSwapPD(src0, src0);
      Face::m128dMulPD(src0, src0, m_01_10);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsD_Swap_SSE2(const TransformD& self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  Face::m128d m_20_21 = _mm_loadu_pd(&self._20);
  Face::m128d m_01_10 = _mm_setr_pd(self._01, self._10);

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad16a(src0, &src[0]);
      Face::m128dLoad16a(src1, &src[1]);
      Face::m128dLoad16a(src2, &src[2]);
      Face::m128dLoad16a(src3, &src[3]);

      Face::m128dSwapPD(src0, src0);
      Face::m128dSwapPD(src1, src1);
      Face::m128dSwapPD(src2, src2);
      Face::m128dSwapPD(src3, src3);

      Face::m128dMulPD(src0, src0, m_01_10);
      Face::m128dMulPD(src1, src1, m_01_10);
      Face::m128dMulPD(src2, src2, m_01_10);
      Face::m128dMulPD(src3, src3, m_01_10);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad16a(src0, &src[0]);
      Face::m128dSwapPD(src0, src0);
      Face::m128dMulPD(src0, src0, m_01_10);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;

      Face::m128dLoad16u(src0, &src[0]);
      Face::m128dLoad16u(src1, &src[1]);
      Face::m128dLoad16u(src2, &src[2]);
      Face::m128dLoad16u(src3, &src[3]);

      Face::m128dSwapPD(src0, src0);
      Face::m128dSwapPD(src1, src1);
      Face::m128dSwapPD(src2, src2);
      Face::m128dSwapPD(src3, src3);

      Face::m128dMulPD(src0, src0, m_01_10);
      Face::m128dMulPD(src1, src1, m_01_10);
      Face::m128dMulPD(src2, src2, m_01_10);
      Face::m128dMulPD(src3, src3, m_01_10);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);

      Face::m128dStore16u(&dst[0], src0);
      Face::m128dStore16u(&dst[1], src1);
      Face::m128dStore16u(&dst[2], src2);
      Face::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      Face::m128d src0;

      Face::m128dLoad16u(src0, &src[0]);
      Face::m128dSwapPD(src0, src0);
      Face::m128dMulPD(src0, src0, m_01_10);
      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsF_Affine_SSE2(const TransformD& self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);
  Face::m128d m_10_01 = _mm_setr_pd(self._10, self._01);
  Face::m128d m_20_21 = _mm_loadu_pd(&self._20);

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;
      Face::m128d rev0, rev1, rev2, rev3;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Face::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Face::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Face::m128dSwapPD(rev0, src0);
      Face::m128dSwapPD(rev1, src1);

      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(src1, src1, m_00_11);
      Face::m128dMulPD(rev0, rev0, m_10_01);
      Face::m128dMulPD(rev1, rev1, m_10_01);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src0, src0, rev0);
      Face::m128dAddPD(src1, src1, rev1);

      Face::m128dSwapPD(rev2, src2);
      Face::m128dSwapPD(rev3, src3);

      Face::m128dMulPD(src2, src2, m_00_11);
      Face::m128dMulPD(src3, src3, m_00_11);
      Face::m128dMulPD(rev2, rev2, m_10_01);
      Face::m128dMulPD(rev3, rev3, m_10_01);

      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);
      Face::m128dAddPD(src2, src2, rev2);
      Face::m128dAddPD(src3, src3, rev3);

      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;
      Face::m128d rev0, rev1, rev2, rev3;

      Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Face::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Face::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Face::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Face::m128dSwapPD(rev0, src0);
      Face::m128dSwapPD(rev1, src1);

      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(src1, src1, m_00_11);
      Face::m128dMulPD(rev0, rev0, m_10_01);
      Face::m128dMulPD(rev1, rev1, m_10_01);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src0, src0, rev0);
      Face::m128dAddPD(src1, src1, rev1);

      Face::m128dSwapPD(rev2, src2);
      Face::m128dSwapPD(rev3, src3);

      Face::m128dMulPD(src2, src2, m_00_11);
      Face::m128dMulPD(src3, src3, m_00_11);
      Face::m128dMulPD(rev2, rev2, m_10_01);
      Face::m128dMulPD(rev3, rev3, m_10_01);

      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);
      Face::m128dAddPD(src2, src2, rev2);
      Face::m128dAddPD(src3, src3, rev3);

      Face::m128dStore16u(&dst[0], src0);
      Face::m128dStore16u(&dst[1], src1);
      Face::m128dStore16u(&dst[2], src2);
      Face::m128dStore16u(&dst[3], src3);
    }
  }

  for (i = length & 3; i; i--, dst++, src++)
  {
    Face::m128d src0, rev0;

    Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);
    Face::m128dSwapPD(rev0, src0);

    Face::m128dMulPD(src0, src0, m_00_11);
    Face::m128dMulPD(rev0, rev0, m_10_01);
    Face::m128dAddPD(src0, src0, m_20_21);
    Face::m128dAddPD(src0, src0, rev0);

    Face::m128dStore16u(&dst[0], src0);
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsD_Affine_SSE2(const TransformD& self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);
  Face::m128d m_10_01 = _mm_setr_pd(self._10, self._01);
  Face::m128d m_20_21 = _mm_loadu_pd(&self._20);

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;
      Face::m128d rev0, rev1, rev2, rev3;

      Face::m128dLoad16a(src0, &src[0]);
      Face::m128dLoad16a(src1, &src[1]);
      Face::m128dLoad16a(src2, &src[2]);
      Face::m128dLoad16a(src3, &src[3]);

      Face::m128dSwapPD(rev0, src0);
      Face::m128dSwapPD(rev1, src1);

      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(src1, src1, m_00_11);
      Face::m128dMulPD(rev0, rev0, m_10_01);
      Face::m128dMulPD(rev1, rev1, m_10_01);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src0, src0, rev0);
      Face::m128dAddPD(src1, src1, rev1);

      Face::m128dSwapPD(rev2, src2);
      Face::m128dSwapPD(rev3, src3);

      Face::m128dMulPD(src2, src2, m_00_11);
      Face::m128dMulPD(src3, src3, m_00_11);
      Face::m128dMulPD(rev2, rev2, m_10_01);
      Face::m128dMulPD(rev3, rev3, m_10_01);

      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);
      Face::m128dAddPD(src2, src2, rev2);
      Face::m128dAddPD(src3, src3, rev3);

      Face::m128dStore16a(&dst[0], src0);
      Face::m128dStore16a(&dst[1], src1);
      Face::m128dStore16a(&dst[2], src2);
      Face::m128dStore16a(&dst[3], src3);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      Face::m128d src0, src1, src2, src3;
      Face::m128d rev0, rev1, rev2, rev3;

      Face::m128dLoad16u(src0, &src[0]);
      Face::m128dLoad16u(src1, &src[1]);
      Face::m128dLoad16u(src2, &src[2]);
      Face::m128dLoad16u(src3, &src[3]);

      Face::m128dSwapPD(rev0, src0);
      Face::m128dSwapPD(rev1, src1);

      Face::m128dMulPD(src0, src0, m_00_11);
      Face::m128dMulPD(src1, src1, m_00_11);
      Face::m128dMulPD(rev0, rev0, m_10_01);
      Face::m128dMulPD(rev1, rev1, m_10_01);

      Face::m128dAddPD(src0, src0, m_20_21);
      Face::m128dAddPD(src1, src1, m_20_21);
      Face::m128dAddPD(src0, src0, rev0);
      Face::m128dAddPD(src1, src1, rev1);

      Face::m128dSwapPD(rev2, src2);
      Face::m128dSwapPD(rev3, src3);

      Face::m128dMulPD(src2, src2, m_00_11);
      Face::m128dMulPD(src3, src3, m_00_11);
      Face::m128dMulPD(rev2, rev2, m_10_01);
      Face::m128dMulPD(rev3, rev3, m_10_01);

      Face::m128dAddPD(src2, src2, m_20_21);
      Face::m128dAddPD(src3, src3, m_20_21);
      Face::m128dAddPD(src2, src2, rev2);
      Face::m128dAddPD(src3, src3, rev3);

      Face::m128dStore16u(&dst[0], src0);
      Face::m128dStore16u(&dst[1], src1);
      Face::m128dStore16u(&dst[2], src2);
      Face::m128dStore16u(&dst[3], src3);
    }
  }

  for (i = length & 3; i; i--, dst++, src++)
  {
    Face::m128d src0, rev0;

    Face::m128dLoad16u(src0, &src[0]);
    Face::m128dSwapPD(rev0, src0);

    Face::m128dMulPD(src0, src0, m_00_11);
    Face::m128dMulPD(rev0, rev0, m_10_01);
    Face::m128dAddPD(src0, src0, m_20_21);
    Face::m128dAddPD(src0, src0, rev0);

    Face::m128dStore16u(&dst[0], src0);
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsF_Projection_SSE2(const TransformD& self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  if (length >= 2)
  {
    Face::m128d m_00_00, m_01_01, m_02_02;
    Face::m128d m_10_10, m_11_11, m_12_12;
    Face::m128d m_20_20, m_21_21, m_22_22;

    Face::m128dExtendLo(m_00_00, &self._00);
    Face::m128dExtendLo(m_01_01, &self._01);
    Face::m128dExtendLo(m_02_02, &self._02);

    Face::m128dExtendLo(m_10_10, &self._10);
    Face::m128dExtendLo(m_11_11, &self._11);
    Face::m128dExtendLo(m_12_12, &self._12);

    Face::m128dExtendLo(m_20_20, &self._20);
    Face::m128dExtendLo(m_21_21, &self._21);
    Face::m128dExtendLo(m_22_22, &self._22);

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128d src0xx, src1xx;
      Face::m128d src0yy, src1yy;

      Face::m128d rcp0;

      // src0xx = [x0, x1]
      // src0yy = [y0, y1]
      Face::m128f tmp0;
      Face::m128fLoad16uLoHi(tmp0, src);
      Face::m128dCvtPDFromPS(src0xx, _mm_shuffle_epi32_f(tmp0, _MM_SHUFFLE(3, 1, 2, 0)));
      Face::m128dCvtPDFromPS(src0yy, _mm_shuffle_epi32_f(tmp0, _MM_SHUFFLE(2, 0, 3, 1)));

      src1xx = src0xx;
      src1yy = src0yy;

      // rcp0 = 1.0 / (x * _02 + y * _12 + _22)
      Face::m128dMulPD(src1xx, src1xx, m_02_02);
      Face::m128dMulPD(src1yy, src1yy, m_12_12);
      Face::m128dAddPD(src1xx, src1xx, m_22_22);
      Face::m128dAddPD(src1xx, src1xx, src1yy);

      Face::m128dEpsilonPD(src1xx, src1xx);
      Face::m128dRcpPD(rcp0, src1xx);

      // src0xx = (x * _00 + y * _10 + _20) * rcp0
      // src0yy = (x * _01 + y * _11 + _21) * rcp0
      src1xx = src0xx;
      src1yy = src0yy;

      Face::m128dMulPD(src0xx, src0xx, m_00_00);
      Face::m128dMulPD(src0yy, src0yy, m_11_11);

      Face::m128dMulPD(src1xx, src1xx, m_01_01);
      Face::m128dMulPD(src1yy, src1yy, m_10_10);

      Face::m128dAddPD(src0xx, src0xx, m_20_20);
      Face::m128dAddPD(src0yy, src0yy, m_21_21);

      Face::m128dAddPD(src0xx, src0xx, src1yy);
      Face::m128dAddPD(src0yy, src0yy, src1xx);

      Face::m128dMulPD(src0xx, src0xx, rcp0);
      Face::m128dMulPD(src0yy, src0yy, rcp0);

      Face::m128dStore8Lo(&dst[0].x, src0xx);
      Face::m128dStore8Lo(&dst[0].y, src0yy);
      Face::m128dStore8Hi(&dst[1].x, src0xx);
      Face::m128dStore8Hi(&dst[1].y, src0yy);
    }
  }

  if (length & 1)
  {
    Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);
    Face::m128d m_10_01 = _mm_setr_pd(self._10, self._01);
    Face::m128d m_20_21 = _mm_loadu_pd(&self._20);

    Face::m128d src0;
    Face::m128dLoad8CvtFrom2xPS(src0, &src[0]);

    Face::m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
    Face::m128d inv0 = _mm_mul_pd(src0, _mm_setr_pd(self._02, self._12));

    // src0.x = (x * _00 + y * _10 + _20)
    // src0.y = (x * _01 + y * _11 + _21)
    Face::m128dMulPD(src0, src0, m_00_11);
    Face::m128dMulPD(rev0, rev0, m_10_01);
    Face::m128dAddPD(src0, src0, m_20_21);
    Face::m128dAddPD(src0, src0, rev0);

    Face::m128dAddPD(inv0, inv0, _mm_shuffle_pd(inv0, inv0, _MM_SHUFFLE2(0, 1)));
    Face::m128dAddSD(inv0, inv0, _mm_load_sd(&self._22));

    Face::m128dEpsilonSD(inv0, inv0);
    Face::m128dRcpSD(inv0, inv0);
    Face::m128dExtendLo(inv0, inv0);

    Face::m128dMulPD(src0, src0, inv0);
    Face::m128dStore16u(dst, src0);
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsD_Projection_SSE2(const TransformD& self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  if (length >= 2)
  {
    Face::m128d m_00_00, m_01_01, m_02_02;
    Face::m128d m_10_10, m_11_11, m_12_12;
    Face::m128d m_20_20, m_21_21, m_22_22;

    Face::m128dExtendLo(m_00_00, &self._00);
    Face::m128dExtendLo(m_01_01, &self._01);
    Face::m128dExtendLo(m_02_02, &self._02);

    Face::m128dExtendLo(m_10_10, &self._10);
    Face::m128dExtendLo(m_11_11, &self._11);
    Face::m128dExtendLo(m_12_12, &self._12);

    Face::m128dExtendLo(m_20_20, &self._20);
    Face::m128dExtendLo(m_21_21, &self._21);
    Face::m128dExtendLo(m_22_22, &self._22);

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      Face::m128d src0xx, src1xx;
      Face::m128d src0yy, src1yy;

      Face::m128d rcp0;

      // src0xx = [x0, x1]
      // src0yy = [y0, y1]
      Face::m128dLoad8(src0xx, &src[0].x);
      Face::m128dLoad8(src0yy, &src[0].y);

      Face::m128dLoad8Hi(src0xx, &src[1].x);
      Face::m128dLoad8Hi(src0yy, &src[1].y);

      src1xx = src0xx;
      src1yy = src0yy;

      // rcp0 = 1.0 / (x * _02 + y * _12 + _22)
      Face::m128dMulPD(src1xx, src1xx, m_02_02);
      Face::m128dMulPD(src1yy, src1yy, m_12_12);
      Face::m128dAddPD(src1xx, src1xx, m_22_22);
      Face::m128dAddPD(src1xx, src1xx, src1yy);

      Face::m128dEpsilonPD(src1xx, src1xx);
      Face::m128dRcpPD(rcp0, src1xx);

      // src0xx = (x * _00 + y * _10 + _20) * rcp0
      // src0yy = (x * _01 + y * _11 + _21) * rcp0
      src1xx = src0xx;
      src1yy = src0yy;

      Face::m128dMulPD(src0xx, src0xx, m_00_00);
      Face::m128dMulPD(src0yy, src0yy, m_11_11);

      Face::m128dMulPD(src1xx, src1xx, m_01_01);
      Face::m128dMulPD(src1yy, src1yy, m_10_10);

      Face::m128dAddPD(src0xx, src0xx, m_20_20);
      Face::m128dAddPD(src0yy, src0yy, m_21_21);

      Face::m128dAddPD(src0xx, src0xx, src1yy);
      Face::m128dAddPD(src0yy, src0yy, src1xx);

      Face::m128dMulPD(src0xx, src0xx, rcp0);
      Face::m128dMulPD(src0yy, src0yy, rcp0);

      Face::m128dStore8Lo(&dst[0].x, src0xx);
      Face::m128dStore8Lo(&dst[0].y, src0yy);
      Face::m128dStore8Hi(&dst[1].x, src0xx);
      Face::m128dStore8Hi(&dst[1].y, src0yy);
    }
  }

  if (length & 1)
  {
    Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);
    Face::m128d m_10_01 = _mm_setr_pd(self._10, self._01);
    Face::m128d m_20_21 = _mm_loadu_pd(&self._20);

    Face::m128d src0 = _mm_loadu_pd(&src[0].x);
    Face::m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
    Face::m128d inv0 = _mm_mul_pd(src0, _mm_setr_pd(self._02, self._12));

    // src0.x = (x * _00 + y * _10 + _20)
    // src0.y = (x * _01 + y * _11 + _21)
    Face::m128dMulPD(src0, src0, m_00_11);
    Face::m128dMulPD(rev0, rev0, m_10_01);
    Face::m128dAddPD(src0, src0, m_20_21);
    Face::m128dAddPD(src0, src0, rev0);

    Face::m128dAddPD(inv0, inv0, _mm_shuffle_pd(inv0, inv0, _MM_SHUFFLE2(0, 1)));
    Face::m128dAddSD(inv0, inv0, _mm_load_sd(&self._22));

    Face::m128dEpsilonSD(inv0, inv0);
    Face::m128dRcpSD(inv0, inv0);
    Face::m128dExtendLo(inv0, inv0);

    Face::m128dMulPD(src0, src0, inv0);
    Face::m128dStore16u(dst, src0);
  }
}

static void FOG_CDECL _G2d_TransformD_mapPointsT_Degenerate_SSE2(const TransformD& self, PointD* dst, const void* src, size_t length)
{
  Face::m128d zero = _mm_setzero_pd();
  size_t i;

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length; i; i--, dst += 1)
    {
      Face::m128dStore16a(dst, zero);
    }
  }
  else
  {
    for (i = length; i; i--, dst += 1)
    {
      Face::m128dStore8Lo(&dst[0].x, zero);
      Face::m128dStore8Lo(&dst[0].y, zero);
    }
  }
}

// ============================================================================
// [Fog::Transform - MapVector]
// ============================================================================

static void FOG_CDECL _G2d_TransformD_mapVectorD_SSE2(const TransformD& self, PointD& dst, const PointD& src)
{
  uint32_t selfType = self.getType();

  Face::m128d src0 = _mm_loadu_pd(&src.x);
  Face::m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));

  Face::m128d m_00_11 = _mm_setr_pd(self._00, self._11);
  Face::m128d m_10_01 = _mm_setr_pd(self._10, self._01);

  switch (selfType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    case TRANSFORM_TYPE_TRANSLATION:
      break;

    case TRANSFORM_TYPE_SCALING:
    {
      Face::m128dMulPD(src0, src0, m_00_11);
      break;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      Face::m128dMulPD(src0, rev0, m_10_01);
      break;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      src0 = _mm_mul_pd(src0, m_00_11);
      rev0 = _mm_mul_pd(rev0, m_10_01);
      src0 = _mm_add_pd(src0, rev0);
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      Face::m128d rcp0 = _mm_setr_pd(self._02, self._12);
      Face::m128d rcpX;

      rcp0 = _mm_mul_pd(rcp0, src0);
      rcpX = _mm_shuffle_pd(rcp0, rcp0, _MM_SHUFFLE2(0, 1));

      src0 = _mm_mul_pd(src0, m_00_11);
      rev0 = _mm_mul_pd(rev0, m_10_01);
      src0 = _mm_add_pd(src0, rev0);

      rcp0 = _mm_add_pd(rcp0, rcpX);
      rcp0 = _mm_add_sd(rcp0, _mm_load_sd(&self._22));
      rcp0 = _mm_loadh_pd(rcp0, &self._22);

      rcpX = FOG_SSE_GET_CONST_PD(m128d_2x_sn);
      rcpX = _mm_and_pd(rcpX, rcp0);
      rcp0 = _mm_xor_pd(rcp0, rcpX);
      rcp0 = _mm_max_pd(rcp0, FOG_SSE_GET_CONST_PD(m128d_2x_eps));
      rcp0 = _mm_xor_pd(rcp0, rcpX);
      rcp0 = _mm_div_pd(FOG_SSE_GET_CONST_PD(m128d_2x_one), rcp0);

      rcpX = _mm_loadu_pd(&self._20);
      src0 = _mm_add_pd(src0, rcpX);

      src0 = _mm_mul_pd(src0, _mm_unpacklo_pd(rcp0, rcp0));
      src0 = _mm_sub_pd(src0, _mm_mul_pd(rcpX, _mm_unpackhi_pd(rcp0, rcp0)));
      break;
    }

    case TRANSFORM_TYPE_DEGENERATE:
    {
      src0 = _mm_setzero_pd();
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  Face::m128dStore16u(&dst, src0);
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_transform_init_sse2(void)
{
  _g2d.transformd.mapPointD = _G2d_TransformD_mapPointD_SSE2;
  _g2d.transformd.mapVectorD = _G2d_TransformD_mapVectorD_SSE2;

  _g2d.transformd.mapPointsF[TRANSFORM_TYPE_IDENTITY   ] = _G2d_TransformD_mapPointsF_Identity_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_IDENTITY   ] = _G2d_TransformD_mapPointsD_Identity_SSE2;
  _g2d.transformd.mapPointsF[TRANSFORM_TYPE_TRANSLATION] = _G2d_TransformD_mapPointsF_Translation_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_TRANSLATION] = _G2d_TransformD_mapPointsD_Translation_SSE2;
  _g2d.transformd.mapPointsF[TRANSFORM_TYPE_SCALING    ] = _G2d_TransformD_mapPointsF_Scaling_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_SCALING    ] = _G2d_TransformD_mapPointsD_Scaling_SSE2;
  _g2d.transformd.mapPointsF[TRANSFORM_TYPE_SWAP       ] = _G2d_TransformD_mapPointsF_Swap_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_SWAP       ] = _G2d_TransformD_mapPointsD_Swap_SSE2;
  _g2d.transformd.mapPointsF[TRANSFORM_TYPE_ROTATION   ] = _G2d_TransformD_mapPointsF_Affine_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_ROTATION   ] = _G2d_TransformD_mapPointsD_Affine_SSE2;
  _g2d.transformd.mapPointsF[TRANSFORM_TYPE_AFFINE     ] = _G2d_TransformD_mapPointsF_Affine_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_AFFINE     ] = _G2d_TransformD_mapPointsD_Affine_SSE2;
  _g2d.transformd.mapPointsF[TRANSFORM_TYPE_PROJECTION ] = _G2d_TransformD_mapPointsF_Projection_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_PROJECTION ] = _G2d_TransformD_mapPointsD_Projection_SSE2;
  _g2d.transformd.mapPointsF[TRANSFORM_TYPE_DEGENERATE ] = (_G2dApi::TransformD_MapPointsF)_G2d_TransformD_mapPointsT_Degenerate_SSE2;
  _g2d.transformd.mapPointsD[TRANSFORM_TYPE_DEGENERATE ] = (_G2dApi::TransformD_MapPointsD)_G2d_TransformD_mapPointsT_Degenerate_SSE2;
}

} // Fog namespace
