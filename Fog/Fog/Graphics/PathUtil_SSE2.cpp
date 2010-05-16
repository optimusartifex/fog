// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Compiler/IntrinSSE2.h>
#include <Fog/Core/Math.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/PathUtil.h>

namespace Fog {
namespace PathUtil {

// ============================================================================
// [Fog::PathUtil::Transformations]
// ============================================================================

static void FOG_FASTCALL translatePointsD_sse2(DoublePoint* dst, const DoublePoint* src, sysuint_t length, const DoublePoint* pt)
{
  sysuint_t i;

  __m128d m_tx_ty = _mm_loadu_pd(&pt->x);

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d m_x_y_0 = _mm_load_pd(&src[0].x);
      __m128d m_x_y_1 = _mm_load_pd(&src[1].x);
      __m128d m_x_y_2 = _mm_load_pd(&src[2].x);
      __m128d m_x_y_3 = _mm_load_pd(&src[3].x);

      m_x_y_0 = _mm_add_pd(m_x_y_0, m_tx_ty);
      m_x_y_1 = _mm_add_pd(m_x_y_1, m_tx_ty);
      m_x_y_2 = _mm_add_pd(m_x_y_2, m_tx_ty);
      m_x_y_3 = _mm_add_pd(m_x_y_3, m_tx_ty);

      _mm_store_pd(&dst[0].x, m_x_y_0);
      _mm_store_pd(&dst[1].x, m_x_y_1);
      _mm_store_pd(&dst[2].x, m_x_y_2);
      _mm_store_pd(&dst[3].x, m_x_y_3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d m_x_y_0 = _mm_load_pd(&src[0].x);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_tx_ty);
      _mm_store_pd(&dst[0].x, m_x_y_0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d m_x_y_0 = _mm_loadu_pd(&src[0].x);
      __m128d m_x_y_1 = _mm_loadu_pd(&src[1].x);
      __m128d m_x_y_2 = _mm_loadu_pd(&src[2].x);
      __m128d m_x_y_3 = _mm_loadu_pd(&src[3].x);

      m_x_y_0 = _mm_add_pd(m_x_y_0, m_tx_ty);
      m_x_y_1 = _mm_add_pd(m_x_y_1, m_tx_ty);
      m_x_y_2 = _mm_add_pd(m_x_y_2, m_tx_ty);
      m_x_y_3 = _mm_add_pd(m_x_y_3, m_tx_ty);

      _mm_storeu_pd(&dst[0].x, m_x_y_0);
      _mm_storeu_pd(&dst[1].x, m_x_y_1);
      _mm_storeu_pd(&dst[2].x, m_x_y_2);
      _mm_storeu_pd(&dst[3].x, m_x_y_3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d m_x_y_0 = _mm_loadu_pd(&src[0].x);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_tx_ty);
      _mm_storeu_pd(&dst[0].x, m_x_y_0);
    }
  }
}

static void FOG_FASTCALL transformPointsD_sse2(DoublePoint* dst, const DoublePoint* src, sysuint_t length, const DoubleMatrix* matrix)
{
  sysuint_t i;

  __m128d m_sx_sy = _mm_set_pd(matrix->sy, matrix->sx);
  __m128d m_shx_shy = _mm_set_pd(matrix->shy, matrix->shx);
  __m128d m_tx_ty = _mm_loadu_pd(&matrix->m[4]);

  if ((((sysuint_t)dst | (sysuint_t)src) & 0xF) == 0)
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d m_x_y_0 = _mm_load_pd(&src[0].x);
      __m128d m_x_y_1 = _mm_load_pd(&src[1].x);
      __m128d m_x_y_2 = _mm_load_pd(&src[2].x);
      __m128d m_x_y_3 = _mm_load_pd(&src[3].x);

      __m128d m_y_x_0 = _mm_shuffle_pd(m_x_y_0, m_x_y_0, _MM_SHUFFLE2(0, 1));
      __m128d m_y_x_1 = _mm_shuffle_pd(m_x_y_1, m_x_y_1, _MM_SHUFFLE2(0, 1));

      m_x_y_0 = _mm_mul_pd(m_x_y_0, m_sx_sy);
      m_x_y_1 = _mm_mul_pd(m_x_y_1, m_sx_sy);
      m_y_x_0 = _mm_mul_pd(m_y_x_0, m_shx_shy);
      m_y_x_1 = _mm_mul_pd(m_y_x_1, m_shx_shy);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_tx_ty);
      m_x_y_1 = _mm_add_pd(m_x_y_1, m_tx_ty);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_y_x_0);
      m_x_y_1 = _mm_add_pd(m_x_y_1, m_y_x_1);

      __m128d m_y_x_2 = _mm_shuffle_pd(m_x_y_2, m_x_y_2, _MM_SHUFFLE2(0, 1));
      __m128d m_y_x_3 = _mm_shuffle_pd(m_x_y_3, m_x_y_3, _MM_SHUFFLE2(0, 1));

      m_x_y_2 = _mm_mul_pd(m_x_y_2, m_sx_sy);
      m_x_y_3 = _mm_mul_pd(m_x_y_3, m_sx_sy);
      m_y_x_2 = _mm_mul_pd(m_y_x_2, m_shx_shy);
      m_y_x_3 = _mm_mul_pd(m_y_x_3, m_shx_shy);
      m_x_y_2 = _mm_add_pd(m_x_y_2, m_tx_ty);
      m_x_y_3 = _mm_add_pd(m_x_y_3, m_tx_ty);
      m_x_y_2 = _mm_add_pd(m_x_y_2, m_y_x_2);
      m_x_y_3 = _mm_add_pd(m_x_y_3, m_y_x_3);

      _mm_store_pd(&dst[0].x, m_x_y_0);
      _mm_store_pd(&dst[1].x, m_x_y_1);
      _mm_store_pd(&dst[2].x, m_x_y_2);
      _mm_store_pd(&dst[3].x, m_x_y_3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d m_x_y_0 = _mm_load_pd(&src[0].x);
      __m128d m_y_x_0 = _mm_shuffle_pd(m_x_y_0, m_x_y_0, _MM_SHUFFLE2(0, 1));

      m_x_y_0 = _mm_mul_pd(m_x_y_0, m_sx_sy);
      m_y_x_0 = _mm_mul_pd(m_y_x_0, m_shx_shy);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_tx_ty);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_y_x_0);

      _mm_store_pd(&dst[0].x, m_x_y_0);
    }
  }
  else
  {
    for (i = length >> 2; i; i--, dst += 4, src += 4)
    {
      __m128d m_x_y_0 = _mm_loadu_pd(&src[0].x);
      __m128d m_x_y_1 = _mm_loadu_pd(&src[1].x);
      __m128d m_x_y_2 = _mm_loadu_pd(&src[2].x);
      __m128d m_x_y_3 = _mm_loadu_pd(&src[3].x);

      __m128d m_y_x_0 = _mm_shuffle_pd(m_x_y_0, m_x_y_0, _MM_SHUFFLE2(0, 1));
      __m128d m_y_x_1 = _mm_shuffle_pd(m_x_y_1, m_x_y_1, _MM_SHUFFLE2(0, 1));

      m_x_y_0 = _mm_mul_pd(m_x_y_0, m_sx_sy);
      m_x_y_1 = _mm_mul_pd(m_x_y_1, m_sx_sy);
      m_y_x_0 = _mm_mul_pd(m_y_x_0, m_shx_shy);
      m_y_x_1 = _mm_mul_pd(m_y_x_1, m_shx_shy);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_tx_ty);
      m_x_y_1 = _mm_add_pd(m_x_y_1, m_tx_ty);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_y_x_0);
      m_x_y_1 = _mm_add_pd(m_x_y_1, m_y_x_1);

      __m128d m_y_x_2 = _mm_shuffle_pd(m_x_y_2, m_x_y_2, _MM_SHUFFLE2(0, 1));
      __m128d m_y_x_3 = _mm_shuffle_pd(m_x_y_3, m_x_y_3, _MM_SHUFFLE2(0, 1));

      m_x_y_2 = _mm_mul_pd(m_x_y_2, m_sx_sy);
      m_x_y_3 = _mm_mul_pd(m_x_y_3, m_sx_sy);
      m_y_x_2 = _mm_mul_pd(m_y_x_2, m_shx_shy);
      m_y_x_3 = _mm_mul_pd(m_y_x_3, m_shx_shy);
      m_x_y_2 = _mm_add_pd(m_x_y_2, m_tx_ty);
      m_x_y_3 = _mm_add_pd(m_x_y_3, m_tx_ty);
      m_x_y_2 = _mm_add_pd(m_x_y_2, m_y_x_2);
      m_x_y_3 = _mm_add_pd(m_x_y_3, m_y_x_3);

      _mm_storeu_pd(&dst[0].x, m_x_y_0);
      _mm_storeu_pd(&dst[1].x, m_x_y_1);
      _mm_storeu_pd(&dst[2].x, m_x_y_2);
      _mm_storeu_pd(&dst[3].x, m_x_y_3);
    }

    for (i = length & 3; i; i--, dst++, src++)
    {
      __m128d m_x_y_0 = _mm_loadu_pd(&src[0].x);
      __m128d m_y_x_0 = _mm_shuffle_pd(m_x_y_0, m_x_y_0, _MM_SHUFFLE2(0, 1));

      m_x_y_0 = _mm_mul_pd(m_x_y_0, m_sx_sy);
      m_y_x_0 = _mm_mul_pd(m_y_x_0, m_shx_shy);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_tx_ty);
      m_x_y_0 = _mm_add_pd(m_x_y_0, m_y_x_0);

      _mm_storeu_pd(&dst[0].x, m_x_y_0);
    }
  }
}

} // PathUtil namespace
} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE void fog_pathutil_init_sse2(void)
{
  using namespace Fog;

  PathUtil::functionMap.translatePointsD = PathUtil::translatePointsD_sse2;
  PathUtil::functionMap.transformPointsD = PathUtil::transformPointsD_sse2;
}
