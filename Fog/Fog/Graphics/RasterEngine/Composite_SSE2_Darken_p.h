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
// [Fog::RasterEngine::SSE2 - Composite - Darken]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeDarkenSSE2 :
  public CompositeCommonSSE2<CompositeDarkenSSE2>,
  public CompositeBaseFuncsSSE2<CompositeDarkenSSE2>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DARKEN };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;
    __m128i z0xmm;

    // x0xmm = SA SR SG SB DA DR DG DB
    x0xmm = _mm_slli_si128(b0xmm, 8);
    x0xmm = _mm_or_si128(x0xmm, a0xmm);

    // y0xmm = DA DA DA DA SA SA SA SA
    sse2_expand_alpha_1x2W(y0xmm, x0xmm);
    y0xmm = _mm_shuffle_epi32(y0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    z0xmm = x0xmm;

    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    sse2_negate_1x2W(y0xmm, y0xmm);
    sse2_muldiv255_1x2W(z0xmm, z0xmm, y0xmm);

    dst0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    y0xmm = _mm_shuffle_epi32(z0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    dst0xmm = _mm_min_epi16(dst0xmm, x0xmm); // dst0xmm  = min(Sca.Da, Dca.Sa)
    dst0xmm = _mm_adds_epu8(dst0xmm, y0xmm); // dst0xmm += Sca.(1 - Da)
    dst0xmm = _mm_adds_epu8(dst0xmm, z0xmm); // dst0xmm += Dca.(1 - Sa)
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, a0xmm);
    sse2_expand_pixel_lo_1x2W(x0xmm, b0xmm);
    sse2_expand_pixel_lo_1x2W(y0xmm, y0xmm);

    y0xmm = _mm_xor_si128(y0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_0000000000000000));
    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    y0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    dst0xmm = _mm_min_epi16(a0xmm, x0xmm);   // dst0xmm = min(Sc.Da, Dca)
    dst0xmm = _mm_adds_epu8(dst0xmm, y0xmm); // dst0xmm += Sc.(1 - Da)
    dst0xmm = _mm_or_si128(dst0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    prgb32_op_xrgb32_0xXX_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    dst0xmm = _mm_min_epi16(a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, b0xmm);
    sse2_expand_pixel_lo_1x2W(x0xmm, a0xmm);
    sse2_expand_pixel_lo_1x2W(y0xmm, y0xmm);

    y0xmm = _mm_xor_si128(y0xmm, FOG_GET_SSE_CONST_PI(00FF00FF00FF00FF_0000000000000000));
    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    y0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    dst0xmm = _mm_min_epi16(b0xmm, x0xmm);   // dst0xmm = min(Sca, Dca.Sa)
    dst0xmm = _mm_adds_epu8(dst0xmm, y0xmm); // dst0xmm += Dc.(1 - Sa)
    dst0xmm = _mm_or_si128(dst0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm); // x0xmm = Sa
    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm); // y0xmm = Da
    sse2_muldiv255_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm); // x0xmm = Dca.Sa
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm); // y0xmm = Sca.Da

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, x0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, x1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, y1xmm);

    x0xmm = _mm_min_epi16(x0xmm, y0xmm); // x0xmm = min(Dca.Sa, Sca.Da)
    x1xmm = _mm_min_epi16(x1xmm, y1xmm); // x1xmm = min(Dca.Sa, Sca.Da)

    dst0xmm = _mm_add_epi16(dst0xmm, x0xmm);
    dst1xmm = _mm_add_epi16(dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm); // y0xmm = Da
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm); // y0xmm = Sc.Da

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, y1xmm);

    y0xmm = _mm_min_epi16(y0xmm, a0xmm); // x0xmm = min(Dca, Sc.Da)
    y1xmm = _mm_min_epi16(y1xmm, a1xmm); // x1xmm = min(Dca, Sc.Da)

    dst0xmm = _mm_add_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(dst1xmm, y1xmm);

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm); // y0xmm = Da
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm); // y0xmm = Sc.Da

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, y1xmm);

    y0xmm = _mm_min_epi16(y0xmm, a0xmm); // x0xmm = min(Dca, Sc.Da)
    y1xmm = _mm_min_epi16(y1xmm, a1xmm); // x1xmm = min(Dca, Sc.Da)

    dst0xmm = _mm_add_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(dst1xmm, y1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    dst0xmm = _mm_min_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_min_epi16(a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm); // x0xmm = Sa
    sse2_negate_2x2W(y0xmm, x0xmm, y1xmm, x1xmm); // y0xmm = 1 - Sa
    sse2_muldiv255_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm); // Dc.Sa
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm); // Dc.(1 - Sa)

    x0xmm = _mm_min_epi16(x0xmm, b0xmm); // x0xmm = min(Sca, Dc.Sa)
    x1xmm = _mm_min_epi16(x1xmm, b1xmm); // x1xmm = min(Sca, Dc.Sa)

    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }
};

} // RasterEngine namespace
} // Fog namespace
