// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

#include <Fog/Core/Acc/AccSse2.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

// ============================================================================
// [Fog::Transform - MapPoint(s)]
// ============================================================================

static void FOG_CDECL TransformD_mapPointD_SSE2(const TransformD* self, PointD* dst, const PointD* src)
{
  uint32_t selfType = self->getType();

  __m128d m_20_21;
  __m128d src0;

  Acc::m128dLoad16u(m_20_21, &self->_20);
  Acc::m128dLoad16u(src0, src);

  switch (selfType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    {
      break;
    }

    case TRANSFORM_TYPE_SCALING:
    {
      __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);
      src0 = _mm_mul_pd(src0, m_00_11);
      // ... Fall through ...
    }

    case TRANSFORM_TYPE_TRANSLATION:
    {
      Acc::m128dAddPD(src0, src0, m_20_21);
      break;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      Acc::m128dSwapPD(src0, src0);
      Acc::m128dMulPD(src0, src0, _mm_setr_pd(self->_10, self->_01));
      Acc::m128dAddPD(src0, src0, m_20_21);
      break;
    }

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
    {
      __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);
      __m128d m_10_01 = _mm_setr_pd(self->_10, self->_01);
      __m128d rev0;

      Acc::m128dSwapPD(rev0, src0);

      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(rev0, rev0, m_10_01);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src0, src0, rev0);
      break;
    }

    case TRANSFORM_TYPE_PROJECTION:
    {
      __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);
      __m128d m_10_01 = _mm_setr_pd(self->_10, self->_01);

      __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
      __m128d inv0 = _mm_mul_pd(src0, _mm_setr_pd(self->_02, self->_12));

      Acc::m128dAddPD(inv0, inv0, _mm_shuffle_pd(inv0, inv0, _MM_SHUFFLE2(0, 1)));
      Acc::m128dAddSD(inv0, inv0, _mm_load_sd(&self->_22));

      Acc::m128dEpsilonSD(inv0, inv0);
      Acc::m128dRcpSD(inv0, inv0);
      Acc::m128dExtendLo(inv0, inv0);

      // src0.x = (x * _00 + y * _01 + _20)
      // src0.y = (x * _01 + y * _11 + _21)
      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(rev0, rev0, m_10_01);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src0, src0, rev0);

      Acc::m128dMulPD(src0, src0, inv0);
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

  Acc::m128dStore16u(dst, src0);
}

static void FOG_CDECL TransformD_mapPointsF_Identity_SSE2(const TransformD* self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Acc::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Acc::m128dLoad8CvtFrom2xPS(src3, &src[3]);
      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL TransformD_mapPointsD_Identity_SSE2(const TransformD* self, PointD* dst, const PointD* src, size_t length)
{
  if (dst == src) return;

  size_t i;

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad16a(src0, &src[0]);
      Acc::m128dLoad16a(src1, &src[1]);
      Acc::m128dLoad16a(src2, &src[2]);
      Acc::m128dLoad16a(src3, &src[3]);
      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad16a(src0, &src[0]);
      Acc::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length; i; i--, dst++, src++)
    {
      __m128i src0X;
      __m128i src0Y;

      Acc::m128iLoad8(src0X, &src[0].x);
      Acc::m128iLoad8(src0Y, &src[0].y);
      Acc::m128iStore8(&dst[0].x, src0X);
      Acc::m128iStore8(&dst[0].y, src0Y);
    }
  }
}

static void FOG_CDECL TransformD_mapPointsF_Translation_SSE2(const TransformD* self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  __m128d m_20_21 = _mm_loadu_pd(&self->_20);

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Acc::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Acc::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Acc::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Acc::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16u(&dst[0], src0);
      Acc::m128dStore16u(&dst[1], src1);
      Acc::m128dStore16u(&dst[2], src2);
      Acc::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL TransformD_mapPointsD_Translation_SSE2(const TransformD* self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  __m128d m_20_21 = _mm_loadu_pd(&self->_20);

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad16a(src0, &src[0]);
      Acc::m128dLoad16a(src1, &src[1]);
      Acc::m128dLoad16a(src2, &src[2]);
      Acc::m128dLoad16a(src3, &src[3]);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad16a(src0, &src[0]);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad16u(src0, &src[0]);
      Acc::m128dLoad16u(src1, &src[1]);
      Acc::m128dLoad16u(src2, &src[2]);
      Acc::m128dLoad16u(src3, &src[3]);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16u(&dst[0], src0);
      Acc::m128dStore16u(&dst[1], src1);
      Acc::m128dStore16u(&dst[2], src2);
      Acc::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad16u(src0, &src[0]);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL TransformD_mapPointsF_Scaling_SSE2(const TransformD* self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  __m128d m_20_21 = _mm_loadu_pd(&self->_20);
  __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Acc::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Acc::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(src1, src1, m_00_11);
      Acc::m128dMulPD(src2, src2, m_00_11);
      Acc::m128dMulPD(src3, src3, m_00_11);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Acc::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Acc::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(src1, src1, m_00_11);
      Acc::m128dMulPD(src2, src2, m_00_11);
      Acc::m128dMulPD(src3, src3, m_00_11);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16u(&dst[0], src0);
      Acc::m128dStore16u(&dst[1], src1);
      Acc::m128dStore16u(&dst[2], src2);
      Acc::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL TransformD_mapPointsD_Scaling_SSE2(const TransformD* self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  __m128d m_20_21 = _mm_loadu_pd(&self->_20);
  __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad16a(src0, &src[0]);
      Acc::m128dLoad16a(src1, &src[1]);
      Acc::m128dLoad16a(src2, &src[2]);
      Acc::m128dLoad16a(src3, &src[3]);

      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(src1, src1, m_00_11);
      Acc::m128dMulPD(src2, src2, m_00_11);
      Acc::m128dMulPD(src3, src3, m_00_11);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad16a(src0, &src[0]);
      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad16u(src0, &src[0]);
      Acc::m128dLoad16u(src1, &src[1]);
      Acc::m128dLoad16u(src2, &src[2]);
      Acc::m128dLoad16u(src3, &src[3]);

      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(src1, src1, m_00_11);
      Acc::m128dMulPD(src2, src2, m_00_11);
      Acc::m128dMulPD(src3, src3, m_00_11);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16u(&dst[0], src0);
      Acc::m128dStore16u(&dst[1], src1);
      Acc::m128dStore16u(&dst[2], src2);
      Acc::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad16u(src0, &src[0]);
      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL TransformD_mapPointsF_Swap_SSE2(const TransformD* self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  __m128d m_20_21 = _mm_loadu_pd(&self->_20);
  __m128d m_01_10 = _mm_setr_pd(self->_01, self->_10);

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Acc::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Acc::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Acc::m128dSwapPD(src0, src0);
      Acc::m128dSwapPD(src1, src1);
      Acc::m128dSwapPD(src2, src2);
      Acc::m128dSwapPD(src3, src3);

      Acc::m128dMulPD(src0, src0, m_01_10);
      Acc::m128dMulPD(src1, src1, m_01_10);
      Acc::m128dMulPD(src2, src2, m_01_10);
      Acc::m128dMulPD(src3, src3, m_01_10);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dSwapPD(src0, src0);
      Acc::m128dMulPD(src0, src0, m_01_10);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Acc::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Acc::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Acc::m128dSwapPD(src0, src0);
      Acc::m128dSwapPD(src1, src1);
      Acc::m128dSwapPD(src2, src2);
      Acc::m128dSwapPD(src3, src3);

      Acc::m128dMulPD(src0, src0, m_01_10);
      Acc::m128dMulPD(src1, src1, m_01_10);
      Acc::m128dMulPD(src2, src2, m_01_10);
      Acc::m128dMulPD(src3, src3, m_01_10);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16u(&dst[0], src0);
      Acc::m128dStore16u(&dst[1], src1);
      Acc::m128dStore16u(&dst[2], src2);
      Acc::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dSwapPD(src0, src0);
      Acc::m128dMulPD(src0, src0, m_01_10);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL TransformD_mapPointsD_Swap_SSE2(const TransformD* self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  __m128d m_20_21 = _mm_loadu_pd(&self->_20);
  __m128d m_01_10 = _mm_setr_pd(self->_01, self->_10);

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad16a(src0, &src[0]);
      Acc::m128dLoad16a(src1, &src[1]);
      Acc::m128dLoad16a(src2, &src[2]);
      Acc::m128dLoad16a(src3, &src[3]);

      Acc::m128dSwapPD(src0, src0);
      Acc::m128dSwapPD(src1, src1);
      Acc::m128dSwapPD(src2, src2);
      Acc::m128dSwapPD(src3, src3);

      Acc::m128dMulPD(src0, src0, m_01_10);
      Acc::m128dMulPD(src1, src1, m_01_10);
      Acc::m128dMulPD(src2, src2, m_01_10);
      Acc::m128dMulPD(src3, src3, m_01_10);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad16a(src0, &src[0]);
      Acc::m128dSwapPD(src0, src0);
      Acc::m128dMulPD(src0, src0, m_01_10);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16a(&dst[0], src0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;

      Acc::m128dLoad16u(src0, &src[0]);
      Acc::m128dLoad16u(src1, &src[1]);
      Acc::m128dLoad16u(src2, &src[2]);
      Acc::m128dLoad16u(src3, &src[3]);

      Acc::m128dSwapPD(src0, src0);
      Acc::m128dSwapPD(src1, src1);
      Acc::m128dSwapPD(src2, src2);
      Acc::m128dSwapPD(src3, src3);

      Acc::m128dMulPD(src0, src0, m_01_10);
      Acc::m128dMulPD(src1, src1, m_01_10);
      Acc::m128dMulPD(src2, src2, m_01_10);
      Acc::m128dMulPD(src3, src3, m_01_10);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);

      Acc::m128dStore16u(&dst[0], src0);
      Acc::m128dStore16u(&dst[1], src1);
      Acc::m128dStore16u(&dst[2], src2);
      Acc::m128dStore16u(&dst[3], src3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d src0;

      Acc::m128dLoad16u(src0, &src[0]);
      Acc::m128dSwapPD(src0, src0);
      Acc::m128dMulPD(src0, src0, m_01_10);
      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dStore16u(&dst[0], src0);
    }
  }
}

static void FOG_CDECL TransformD_mapPointsF_Affine_SSE2(const TransformD* self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);
  __m128d m_10_01 = _mm_setr_pd(self->_10, self->_01);
  __m128d m_20_21 = _mm_loadu_pd(&self->_20);

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;
      __m128d rev0, rev1, rev2, rev3;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Acc::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Acc::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Acc::m128dSwapPD(rev0, src0);
      Acc::m128dSwapPD(rev1, src1);

      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(src1, src1, m_00_11);
      Acc::m128dMulPD(rev0, rev0, m_10_01);
      Acc::m128dMulPD(rev1, rev1, m_10_01);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src0, src0, rev0);
      Acc::m128dAddPD(src1, src1, rev1);

      Acc::m128dSwapPD(rev2, src2);
      Acc::m128dSwapPD(rev3, src3);

      Acc::m128dMulPD(src2, src2, m_00_11);
      Acc::m128dMulPD(src3, src3, m_00_11);
      Acc::m128dMulPD(rev2, rev2, m_10_01);
      Acc::m128dMulPD(rev3, rev3, m_10_01);

      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);
      Acc::m128dAddPD(src2, src2, rev2);
      Acc::m128dAddPD(src3, src3, rev3);

      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;
      __m128d rev0, rev1, rev2, rev3;

      Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
      Acc::m128dLoad8CvtFrom2xPS(src1, &src[1]);
      Acc::m128dLoad8CvtFrom2xPS(src2, &src[2]);
      Acc::m128dLoad8CvtFrom2xPS(src3, &src[3]);

      Acc::m128dSwapPD(rev0, src0);
      Acc::m128dSwapPD(rev1, src1);

      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(src1, src1, m_00_11);
      Acc::m128dMulPD(rev0, rev0, m_10_01);
      Acc::m128dMulPD(rev1, rev1, m_10_01);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src0, src0, rev0);
      Acc::m128dAddPD(src1, src1, rev1);

      Acc::m128dSwapPD(rev2, src2);
      Acc::m128dSwapPD(rev3, src3);

      Acc::m128dMulPD(src2, src2, m_00_11);
      Acc::m128dMulPD(src3, src3, m_00_11);
      Acc::m128dMulPD(rev2, rev2, m_10_01);
      Acc::m128dMulPD(rev3, rev3, m_10_01);

      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);
      Acc::m128dAddPD(src2, src2, rev2);
      Acc::m128dAddPD(src3, src3, rev3);

      Acc::m128dStore16u(&dst[0], src0);
      Acc::m128dStore16u(&dst[1], src1);
      Acc::m128dStore16u(&dst[2], src2);
      Acc::m128dStore16u(&dst[3], src3);
    }
  }

  for (i = length & 3; i; i--, dst++, src++)
  {
    __m128d src0, rev0;

    Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);
    Acc::m128dSwapPD(rev0, src0);

    Acc::m128dMulPD(src0, src0, m_00_11);
    Acc::m128dMulPD(rev0, rev0, m_10_01);
    Acc::m128dAddPD(src0, src0, m_20_21);
    Acc::m128dAddPD(src0, src0, rev0);

    Acc::m128dStore16u(&dst[0], src0);
  }
}

static void FOG_CDECL TransformD_mapPointsD_Affine_SSE2(const TransformD* self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);
  __m128d m_10_01 = _mm_setr_pd(self->_10, self->_01);
  __m128d m_20_21 = _mm_loadu_pd(&self->_20);

  if ((((size_t)dst | (size_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;
      __m128d rev0, rev1, rev2, rev3;

      Acc::m128dLoad16a(src0, &src[0]);
      Acc::m128dLoad16a(src1, &src[1]);
      Acc::m128dLoad16a(src2, &src[2]);
      Acc::m128dLoad16a(src3, &src[3]);

      Acc::m128dSwapPD(rev0, src0);
      Acc::m128dSwapPD(rev1, src1);

      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(src1, src1, m_00_11);
      Acc::m128dMulPD(rev0, rev0, m_10_01);
      Acc::m128dMulPD(rev1, rev1, m_10_01);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src0, src0, rev0);
      Acc::m128dAddPD(src1, src1, rev1);

      Acc::m128dSwapPD(rev2, src2);
      Acc::m128dSwapPD(rev3, src3);

      Acc::m128dMulPD(src2, src2, m_00_11);
      Acc::m128dMulPD(src3, src3, m_00_11);
      Acc::m128dMulPD(rev2, rev2, m_10_01);
      Acc::m128dMulPD(rev3, rev3, m_10_01);

      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);
      Acc::m128dAddPD(src2, src2, rev2);
      Acc::m128dAddPD(src3, src3, rev3);

      Acc::m128dStore16a(&dst[0], src0);
      Acc::m128dStore16a(&dst[1], src1);
      Acc::m128dStore16a(&dst[2], src2);
      Acc::m128dStore16a(&dst[3], src3);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d src0, src1, src2, src3;
      __m128d rev0, rev1, rev2, rev3;

      Acc::m128dLoad16u(src0, &src[0]);
      Acc::m128dLoad16u(src1, &src[1]);
      Acc::m128dLoad16u(src2, &src[2]);
      Acc::m128dLoad16u(src3, &src[3]);

      Acc::m128dSwapPD(rev0, src0);
      Acc::m128dSwapPD(rev1, src1);

      Acc::m128dMulPD(src0, src0, m_00_11);
      Acc::m128dMulPD(src1, src1, m_00_11);
      Acc::m128dMulPD(rev0, rev0, m_10_01);
      Acc::m128dMulPD(rev1, rev1, m_10_01);

      Acc::m128dAddPD(src0, src0, m_20_21);
      Acc::m128dAddPD(src1, src1, m_20_21);
      Acc::m128dAddPD(src0, src0, rev0);
      Acc::m128dAddPD(src1, src1, rev1);

      Acc::m128dSwapPD(rev2, src2);
      Acc::m128dSwapPD(rev3, src3);

      Acc::m128dMulPD(src2, src2, m_00_11);
      Acc::m128dMulPD(src3, src3, m_00_11);
      Acc::m128dMulPD(rev2, rev2, m_10_01);
      Acc::m128dMulPD(rev3, rev3, m_10_01);

      Acc::m128dAddPD(src2, src2, m_20_21);
      Acc::m128dAddPD(src3, src3, m_20_21);
      Acc::m128dAddPD(src2, src2, rev2);
      Acc::m128dAddPD(src3, src3, rev3);

      Acc::m128dStore16u(&dst[0], src0);
      Acc::m128dStore16u(&dst[1], src1);
      Acc::m128dStore16u(&dst[2], src2);
      Acc::m128dStore16u(&dst[3], src3);
    }
  }

  for (i = length & 3; i; i--, dst++, src++)
  {
    __m128d src0, rev0;

    Acc::m128dLoad16u(src0, &src[0]);
    Acc::m128dSwapPD(rev0, src0);

    Acc::m128dMulPD(src0, src0, m_00_11);
    Acc::m128dMulPD(rev0, rev0, m_10_01);
    Acc::m128dAddPD(src0, src0, m_20_21);
    Acc::m128dAddPD(src0, src0, rev0);

    Acc::m128dStore16u(&dst[0], src0);
  }
}

static void FOG_CDECL TransformD_mapPointsF_Projection_SSE2(const TransformD* self, PointD* dst, const PointF* src, size_t length)
{
  size_t i;

  if (length >= 2)
  {
    __m128d m_00_00, m_01_01, m_02_02;
    __m128d m_10_10, m_11_11, m_12_12;
    __m128d m_20_20, m_21_21, m_22_22;

    Acc::m128dExtendLo(m_00_00, &self->_00);
    Acc::m128dExtendLo(m_01_01, &self->_01);
    Acc::m128dExtendLo(m_02_02, &self->_02);

    Acc::m128dExtendLo(m_10_10, &self->_10);
    Acc::m128dExtendLo(m_11_11, &self->_11);
    Acc::m128dExtendLo(m_12_12, &self->_12);

    Acc::m128dExtendLo(m_20_20, &self->_20);
    Acc::m128dExtendLo(m_21_21, &self->_21);
    Acc::m128dExtendLo(m_22_22, &self->_22);

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      __m128d src0xx, src1xx;
      __m128d src0yy, src1yy;

      __m128d rcp0;

      // src0xx = [x0, x1]
      // src0yy = [y0, y1]
      __m128f tmp0;
      Acc::m128fLoad16uLoHi(tmp0, src);
      Acc::m128dCvtPDFromPS(src0xx, _mm_shuffle_epi32_f(tmp0, _MM_SHUFFLE(3, 1, 2, 0)));
      Acc::m128dCvtPDFromPS(src0yy, _mm_shuffle_epi32_f(tmp0, _MM_SHUFFLE(2, 0, 3, 1)));

      src1xx = src0xx;
      src1yy = src0yy;

      // rcp0 = 1.0 / (x * _02 + y * _12 + _22)
      Acc::m128dMulPD(src1xx, src1xx, m_02_02);
      Acc::m128dMulPD(src1yy, src1yy, m_12_12);
      Acc::m128dAddPD(src1xx, src1xx, m_22_22);
      Acc::m128dAddPD(src1xx, src1xx, src1yy);

      Acc::m128dEpsilonPD(src1xx, src1xx);
      Acc::m128dRcpPD(rcp0, src1xx);

      // src0xx = (x * _00 + y * _10 + _20) * rcp0
      // src0yy = (x * _01 + y * _11 + _21) * rcp0
      src1xx = src0xx;
      src1yy = src0yy;

      Acc::m128dMulPD(src0xx, src0xx, m_00_00);
      Acc::m128dMulPD(src0yy, src0yy, m_11_11);

      Acc::m128dMulPD(src1xx, src1xx, m_01_01);
      Acc::m128dMulPD(src1yy, src1yy, m_10_10);

      Acc::m128dAddPD(src0xx, src0xx, m_20_20);
      Acc::m128dAddPD(src0yy, src0yy, m_21_21);

      Acc::m128dAddPD(src0xx, src0xx, src1yy);
      Acc::m128dAddPD(src0yy, src0yy, src1xx);

      Acc::m128dMulPD(src0xx, src0xx, rcp0);
      Acc::m128dMulPD(src0yy, src0yy, rcp0);

      Acc::m128dStore8Lo(&dst[0].x, src0xx);
      Acc::m128dStore8Lo(&dst[0].y, src0yy);
      Acc::m128dStore8Hi(&dst[1].x, src0xx);
      Acc::m128dStore8Hi(&dst[1].y, src0yy);
    }
  }

  if (length & 1)
  {
    __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);
    __m128d m_10_01 = _mm_setr_pd(self->_10, self->_01);
    __m128d m_20_21 = _mm_loadu_pd(&self->_20);

    __m128d src0;
    Acc::m128dLoad8CvtFrom2xPS(src0, &src[0]);

    __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
    __m128d inv0 = _mm_mul_pd(src0, _mm_setr_pd(self->_02, self->_12));

    // src0.x = (x * _00 + y * _10 + _20)
    // src0.y = (x * _01 + y * _11 + _21)
    Acc::m128dMulPD(src0, src0, m_00_11);
    Acc::m128dMulPD(rev0, rev0, m_10_01);
    Acc::m128dAddPD(src0, src0, m_20_21);
    Acc::m128dAddPD(src0, src0, rev0);

    Acc::m128dAddPD(inv0, inv0, _mm_shuffle_pd(inv0, inv0, _MM_SHUFFLE2(0, 1)));
    Acc::m128dAddSD(inv0, inv0, _mm_load_sd(&self->_22));

    Acc::m128dEpsilonSD(inv0, inv0);
    Acc::m128dRcpSD(inv0, inv0);
    Acc::m128dExtendLo(inv0, inv0);

    Acc::m128dMulPD(src0, src0, inv0);
    Acc::m128dStore16u(dst, src0);
  }
}

static void FOG_CDECL TransformD_mapPointsD_Projection_SSE2(const TransformD* self, PointD* dst, const PointD* src, size_t length)
{
  size_t i;

  if (length >= 2)
  {
    __m128d m_00_00, m_01_01, m_02_02;
    __m128d m_10_10, m_11_11, m_12_12;
    __m128d m_20_20, m_21_21, m_22_22;

    Acc::m128dExtendLo(m_00_00, &self->_00);
    Acc::m128dExtendLo(m_01_01, &self->_01);
    Acc::m128dExtendLo(m_02_02, &self->_02);

    Acc::m128dExtendLo(m_10_10, &self->_10);
    Acc::m128dExtendLo(m_11_11, &self->_11);
    Acc::m128dExtendLo(m_12_12, &self->_12);

    Acc::m128dExtendLo(m_20_20, &self->_20);
    Acc::m128dExtendLo(m_21_21, &self->_21);
    Acc::m128dExtendLo(m_22_22, &self->_22);

    for (i = length >> 1; i; i--, dst += 2, src += 2)
    {
      __m128d src0xx, src1xx;
      __m128d src0yy, src1yy;

      __m128d rcp0;

      // src0xx = [x0, x1]
      // src0yy = [y0, y1]
      Acc::m128dLoad8(src0xx, &src[0].x);
      Acc::m128dLoad8(src0yy, &src[0].y);

      Acc::m128dLoad8Hi(src0xx, &src[1].x);
      Acc::m128dLoad8Hi(src0yy, &src[1].y);

      src1xx = src0xx;
      src1yy = src0yy;

      // rcp0 = 1.0 / (x * _02 + y * _12 + _22)
      Acc::m128dMulPD(src1xx, src1xx, m_02_02);
      Acc::m128dMulPD(src1yy, src1yy, m_12_12);
      Acc::m128dAddPD(src1xx, src1xx, m_22_22);
      Acc::m128dAddPD(src1xx, src1xx, src1yy);

      Acc::m128dEpsilonPD(src1xx, src1xx);
      Acc::m128dRcpPD(rcp0, src1xx);

      // src0xx = (x * _00 + y * _10 + _20) * rcp0
      // src0yy = (x * _01 + y * _11 + _21) * rcp0
      src1xx = src0xx;
      src1yy = src0yy;

      Acc::m128dMulPD(src0xx, src0xx, m_00_00);
      Acc::m128dMulPD(src0yy, src0yy, m_11_11);

      Acc::m128dMulPD(src1xx, src1xx, m_01_01);
      Acc::m128dMulPD(src1yy, src1yy, m_10_10);

      Acc::m128dAddPD(src0xx, src0xx, m_20_20);
      Acc::m128dAddPD(src0yy, src0yy, m_21_21);

      Acc::m128dAddPD(src0xx, src0xx, src1yy);
      Acc::m128dAddPD(src0yy, src0yy, src1xx);

      Acc::m128dMulPD(src0xx, src0xx, rcp0);
      Acc::m128dMulPD(src0yy, src0yy, rcp0);

      Acc::m128dStore8Lo(&dst[0].x, src0xx);
      Acc::m128dStore8Lo(&dst[0].y, src0yy);
      Acc::m128dStore8Hi(&dst[1].x, src0xx);
      Acc::m128dStore8Hi(&dst[1].y, src0yy);
    }
  }

  if (length & 1)
  {
    __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);
    __m128d m_10_01 = _mm_setr_pd(self->_10, self->_01);
    __m128d m_20_21 = _mm_loadu_pd(&self->_20);

    __m128d src0 = _mm_loadu_pd(&src[0].x);
    __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));
    __m128d inv0 = _mm_mul_pd(src0, _mm_setr_pd(self->_02, self->_12));

    // src0.x = (x * _00 + y * _10 + _20)
    // src0.y = (x * _01 + y * _11 + _21)
    Acc::m128dMulPD(src0, src0, m_00_11);
    Acc::m128dMulPD(rev0, rev0, m_10_01);
    Acc::m128dAddPD(src0, src0, m_20_21);
    Acc::m128dAddPD(src0, src0, rev0);

    Acc::m128dAddPD(inv0, inv0, _mm_shuffle_pd(inv0, inv0, _MM_SHUFFLE2(0, 1)));
    Acc::m128dAddSD(inv0, inv0, _mm_load_sd(&self->_22));

    Acc::m128dEpsilonSD(inv0, inv0);
    Acc::m128dRcpSD(inv0, inv0);
    Acc::m128dExtendLo(inv0, inv0);

    Acc::m128dMulPD(src0, src0, inv0);
    Acc::m128dStore16u(dst, src0);
  }
}

static void FOG_CDECL TransformD_mapPointsT_Degenerate_SSE2(const TransformD* self, PointD* dst, const void* src, size_t length)
{
  __m128d zero = _mm_setzero_pd();
  size_t i;

  if (((size_t)dst & 0xF) == 0)
  {
    for (i = length; i; i--, dst += 1)
    {
      Acc::m128dStore16a(dst, zero);
    }
  }
  else
  {
    for (i = length; i; i--, dst += 1)
    {
      Acc::m128dStore8Lo(&dst[0].x, zero);
      Acc::m128dStore8Lo(&dst[0].y, zero);
    }
  }
}

// ============================================================================
// [Fog::Transform - MapVector]
// ============================================================================

static void FOG_CDECL TransformD_mapVectorD_SSE2(const TransformD* self, PointD* dst, const PointD* src)
{
  uint32_t selfType = self->getType();

  __m128d src0 = _mm_loadu_pd(&src->x);
  __m128d rev0 = _mm_shuffle_pd(src0, src0, _MM_SHUFFLE2(0, 1));

  __m128d m_00_11 = _mm_setr_pd(self->_00, self->_11);
  __m128d m_10_01 = _mm_setr_pd(self->_10, self->_01);

  switch (selfType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    case TRANSFORM_TYPE_TRANSLATION:
      break;

    case TRANSFORM_TYPE_SCALING:
    {
      Acc::m128dMulPD(src0, src0, m_00_11);
      break;
    }

    case TRANSFORM_TYPE_SWAP:
    {
      Acc::m128dMulPD(src0, rev0, m_10_01);
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
      __m128d rcp0 = _mm_setr_pd(self->_02, self->_12);
      __m128d rcpX;

      rcp0 = _mm_mul_pd(rcp0, src0);
      rcpX = _mm_shuffle_pd(rcp0, rcp0, _MM_SHUFFLE2(0, 1));

      src0 = _mm_mul_pd(src0, m_00_11);
      rev0 = _mm_mul_pd(rev0, m_10_01);
      src0 = _mm_add_pd(src0, rev0);

      rcp0 = _mm_add_pd(rcp0, rcpX);
      rcp0 = _mm_add_sd(rcp0, _mm_load_sd(&self->_22));
      rcp0 = _mm_loadh_pd(rcp0, &self->_22);

      rcpX = FOG_XMM_GET_CONST_PD(m128d_sn_sn);
      rcpX = _mm_and_pd(rcpX, rcp0);
      rcp0 = _mm_xor_pd(rcp0, rcpX);
      rcp0 = _mm_max_pd(rcp0, FOG_XMM_GET_CONST_PD(m128d_eps_eps));
      rcp0 = _mm_xor_pd(rcp0, rcpX);
      rcp0 = _mm_div_pd(FOG_XMM_GET_CONST_PD(m128d_p1_p1), rcp0);

      rcpX = _mm_loadu_pd(&self->_20);
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

  Acc::m128dStore16u(dst, src0);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Transform_init_SSE2(void)
{
  fog_api.transformd_mapPointD = TransformD_mapPointD_SSE2;
  fog_api.transformd_mapVectorD = TransformD_mapVectorD_SSE2;

  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_IDENTITY   ] = TransformD_mapPointsF_Identity_SSE2;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_IDENTITY   ] = TransformD_mapPointsD_Identity_SSE2;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_TRANSLATION] = TransformD_mapPointsF_Translation_SSE2;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_TRANSLATION] = TransformD_mapPointsD_Translation_SSE2;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_SCALING    ] = TransformD_mapPointsF_Scaling_SSE2;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_SCALING    ] = TransformD_mapPointsD_Scaling_SSE2;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_SWAP       ] = TransformD_mapPointsF_Swap_SSE2;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_SWAP       ] = TransformD_mapPointsD_Swap_SSE2;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_ROTATION   ] = TransformD_mapPointsF_Affine_SSE2;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_ROTATION   ] = TransformD_mapPointsD_Affine_SSE2;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_AFFINE     ] = TransformD_mapPointsF_Affine_SSE2;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_AFFINE     ] = TransformD_mapPointsD_Affine_SSE2;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_PROJECTION ] = TransformD_mapPointsF_Projection_SSE2;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_PROJECTION ] = TransformD_mapPointsD_Projection_SSE2;
  fog_api.transformd_mapPointsF[TRANSFORM_TYPE_DEGENERATE ] = (Api::TransformD_MapPointsF)TransformD_mapPointsT_Degenerate_SSE2;
  fog_api.transformd_mapPointsD[TRANSFORM_TYPE_DEGENERATE ] = (Api::TransformD_MapPointsD)TransformD_mapPointsT_Degenerate_SSE2;
}

} // Fog namespace
