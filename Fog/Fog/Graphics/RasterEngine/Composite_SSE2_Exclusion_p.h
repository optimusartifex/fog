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
// [Fog::RasterEngine::SSE2 - Composite - Exclusion]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeExclusionSSE2 :
  public CompositeCommonSSE2<CompositeExclusionSSE2>,
  public CompositeBaseFuncsSSE2<CompositeExclusionSSE2>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_EXCLUSION };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, a0xmm);
    sse2_fill_alpha_1x1W(y0xmm);
    sse2_muldiv255_1x1W(x0xmm, a0xmm, b0xmm);
    sse2_muldiv255_1x1W(y0xmm, y0xmm, b0xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst0xmm = _mm_min_epi16(dst0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, a0xmm);
    sse2_fill_alpha_1x1W(y0xmm);
    sse2_muldiv255_1x1W(x0xmm, a0xmm, b0xmm);
    sse2_muldiv255_1x1W(y0xmm, y0xmm, b0xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst0xmm = _mm_min_epi16(dst0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    prgb32_op_prgb32_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_muldiv255_1x1W(x0xmm, a0xmm, b0xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst0xmm = _mm_min_epi16(dst0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_muldiv255_1x1W(x0xmm, a0xmm, b0xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst0xmm = _mm_min_epi16(dst0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm);
    sse2_fill_alpha_2x2W(y0xmm, y1xmm);
    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, y1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm);
    sse2_fill_alpha_2x2W(y0xmm, y0xmm);
    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, y1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm);
    sse2_fill_alpha_2x2W(y0xmm, y0xmm);
    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, FOG_GET_SSE_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, y1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);
    x1xmm = _mm_slli_epi16(x1xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
  //!   Dc'  = Sca + Dc - 2.Sca.Dc
    __m128i x0xmm, x1xmm;

    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);
    x1xmm = _mm_slli_epi16(x1xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }
};

} // RasterEngine namespace
} // Fog namespace
