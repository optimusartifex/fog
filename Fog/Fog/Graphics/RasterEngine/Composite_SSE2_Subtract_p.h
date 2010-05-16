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
// [Fog::RasterEngine::SSE2 - Composite - Subtract]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeSubtractSSE2 :
  public CompositeCommonSSE2<CompositeSubtractSSE2>,
  public CompositeBaseFuncsSSE2<CompositeSubtractSSE2>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SUBTRACT };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    x0xmm = _mm_srli_epi32(a0xmm, 24);
    y0xmm = _mm_srli_epi32(b0xmm, 24);

    x0xmm = _mm_xor_si128(x0xmm, FOG_GET_SSE_CONST_PI(000000FF000000FF_000000FF000000FF));
    y0xmm = _mm_xor_si128(y0xmm, FOG_GET_SSE_CONST_PI(000000FF000000FF_000000FF000000FF));

    sse2_muldiv255_1x1W(x0xmm, x0xmm, y0xmm);

    x0xmm = _mm_xor_si128(x0xmm, FOG_GET_SSE_CONST_PI(000000FF000000FF_000000FF000000FF));

    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_zero_alpha_1x1B(dst0xmm);

    dst0xmm = _mm_or_si128(dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1B(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1B(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    x0xmm = _mm_srli_epi32(a0xmm, 24);
    y0xmm = _mm_srli_epi32(b0xmm, 24);

    x0xmm = _mm_xor_si128(x0xmm, FOG_GET_SSE_CONST_PI(000000FF000000FF_000000FF000000FF));
    y0xmm = _mm_xor_si128(y0xmm, FOG_GET_SSE_CONST_PI(000000FF000000FF_000000FF000000FF));

    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);

    x0xmm = _mm_xor_si128(x0xmm, FOG_GET_SSE_CONST_PI(000000FF000000FF_000000FF000000FF));
    x0xmm = _mm_slli_epi32(x0xmm, 24);

    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_zero_alpha_1x4B(dst0xmm);

    dst0xmm = _mm_or_si128(dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x4B(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x4B(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    x0xmm = _mm_xor_si128(a0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
    y0xmm = _mm_xor_si128(b0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));

    sse2_muldiv255_1x1W(x0xmm, x0xmm, y0xmm);

    x0xmm = _mm_and_si128(x0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
    x0xmm = _mm_xor_si128(x0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));

    sse2_subs_1x1W(dst0xmm, a0xmm, b0xmm);
    sse2_zero_alpha_1x1W(dst0xmm);
    dst0xmm = _mm_or_si128(dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    x0xmm = _mm_xor_si128(a0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
    x1xmm = _mm_xor_si128(a1xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
    y0xmm = _mm_xor_si128(b0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
    y1xmm = _mm_xor_si128(b1xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));

    sse2_muldiv255_2x2W(x0xmm, x0xmm, y0xmm, x1xmm, x1xmm, y1xmm);

    x0xmm = _mm_and_si128(a0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
    x1xmm = _mm_and_si128(a1xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
    x0xmm = _mm_xor_si128(x0xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
    x1xmm = _mm_xor_si128(x1xmm, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));

    sse2_subs_2x2W(dst0xmm, a0xmm, b0xmm, dst1xmm, a1xmm, b1xmm);
    sse2_zero_alpha_2x2W(dst0xmm, dst1xmm);

    dst0xmm = _mm_or_si128(dst0xmm, x0xmm);
    dst1xmm = _mm_or_si128(dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_subs_1x4B(dst1xmm, a1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_subs_1x4B(dst1xmm, a1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_subs_1x4B(dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_subs_1x4B(dst1xmm, a1xmm, b1xmm);
  }
};

} // RasterEngine namespace
} // Fog namespace
