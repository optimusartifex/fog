// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_SSE2_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Difference]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeDifferenceSSE2 :
  public CompositeCommonSSE2<CompositeDifferenceSSE2>,
  public CompositeBaseFuncsSSE2<CompositeDifferenceSSE2>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DIFFERENCE };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;
  
    // x0xmm = SA SR SG SB DA DR DG DB
    x0xmm = _mm_slli_si128(b0xmm, 8);
    x0xmm = _mm_or_si128(x0xmm, a0xmm);

    // y0xmm = DA DA DA DA SA SA SA SA
    sse2_expand_alpha_1x2W(y0xmm, x0xmm);
    y0xmm = _mm_shuffle_epi32(y0xmm, _MM_SHUFFLE(1, 0, 3, 2));

    // x0xmm = min(x0xmm, y0xmm)
    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    y0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    x0xmm = _mm_min_epi16(x0xmm, y0xmm);

    // x0xmm *= 2 (except alpha)
    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_expand_alpha_1x1W(x0xmm, a0xmm);
    sse2_muldiv255_1x1W(x0xmm, x0xmm, b0xmm);
    x0xmm = _mm_min_epi16(x0xmm, a0xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    prgb32_op_xrgb32_0xXX_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    x0xmm = a0xmm;
    x0xmm = _mm_min_epi16(x0xmm, b0xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_expand_alpha_1x1W(x0xmm, b0xmm);
    sse2_muldiv255_1x1W(x0xmm, x0xmm, a0xmm);
    x0xmm = _mm_min_epi16(x0xmm, a0xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, a0xmm, x1xmm, a1xmm);
    sse2_expand_alpha_2x2W(y0xmm, b0xmm, y1xmm, b1xmm);

    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);
    sse2_muldiv255_2x2W(y0xmm, y0xmm, a0xmm, y1xmm, y1xmm, a1xmm);

    x0xmm = _mm_min_epi16(x0xmm, y0xmm);
    x1xmm = _mm_min_epi16(x1xmm, y1xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    sse2_expand_alpha_2x2W(x0xmm, a0xmm, x1xmm, a1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);
    x0xmm = _mm_min_epi16(x0xmm, a0xmm);
    x1xmm = _mm_min_epi16(x1xmm, a1xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    prgb32_op_xrgb32_0xXX_unpacked_2x2W(dst0xmm, a0xmm, b0xmm, dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    x0xmm = a0xmm;
    x1xmm = a1xmm;
    x0xmm = _mm_min_epi16(x0xmm, b0xmm);
    x1xmm = _mm_min_epi16(x1xmm, b1xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);
    x1xmm = _mm_slli_epi16(x1xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm);

    x0xmm = _mm_min_epi16(x0xmm, a0xmm);
    x1xmm = _mm_min_epi16(x1xmm, a1xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }
};

} // RasterEngine namespace
} // Fog namespace
