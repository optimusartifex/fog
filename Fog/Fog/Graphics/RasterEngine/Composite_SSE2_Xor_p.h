// [Fog-Graphics]
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
// [Fog::RasterEngine::SSE2 - Composite - Xor]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeXorSSE2 :
  public CompositeCommonSSE2<CompositeXorSSE2>,
  public CompositeBaseFuncsSSE2<CompositeXorSSE2>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_XOR };

  // PRGB32 destination.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    dst0xmm = _mm_or_si128(a0xmm, _mm_slli_si128(b0xmm, 8));
    sse2_expand_alpha_1x2W(x0xmm, dst0xmm);
    sse2_negate_1x2W(x0xmm, x0xmm);
    x0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    sse2_muldiv255_1x2W(dst0xmm, dst0xmm, x0xmm);
    x0xmm = _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2));

    sse2_adds_1x1W(dst0xmm, dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm = b0xmm;
    sse2_fill_alpha_1x1W(x0xmm);

    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_negate_1x1W(dst0xmm, dst0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_negate_1x1W(dst0xmm, dst0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    dst0xmm = _mm_setzero_si128();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_expand_alpha_1x1W(x0xmm, b0xmm);
    sse2_negate_1x1W(x0xmm, x0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, a0xmm, x1xmm, a1xmm);
    sse2_expand_alpha_2x2W(y0xmm, b0xmm, y1xmm, b1xmm);

    sse2_negate_2x2W(x0xmm, x0xmm, x1xmm, x1xmm);
    sse2_negate_2x2W(y0xmm, y0xmm, y1xmm, y1xmm);

    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);

    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm = b0xmm;
    __m128i x1xmm = b1xmm;

    sse2_fill_alpha_2x2W(x0xmm, x1xmm);

    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_negate_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_negate_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    dst0xmm = _mm_setzero_si128();
    dst1xmm = _mm_setzero_si128();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm);
    sse2_negate_2x2W(x0xmm, x0xmm, x1xmm, x1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, x0xmm, dst1xmm, a1xmm, x1xmm);
  }
};

} // RasterEngine namespace
} // Fog namespace
