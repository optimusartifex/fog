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
// [Fog::RasterEngine::SSE2 - Composite - DstAtop]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeDstAtopSSE2 :
  public CompositeCommonSSE2<CompositeDstAtopSSE2>,
  public CompositeBaseFuncsSSE2<CompositeDstAtopSSE2>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DST_ATOP };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    // dstmm[HI] = SRC, x0xmm[HI] = 1 - DA
    // dstmm[LO] = DST, x0xmm[LO] = SA

    dst0xmm = _mm_or_si128(a0xmm, _mm_slli_si128(b0xmm, 8));
    sse2_expand_alpha_1x2W(x0xmm, dst0xmm);
    x0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    sse2_negate_1x2W_hi(x0xmm, x0xmm);

    sse2_muldiv255_1x2W(dst0xmm, dst0xmm, x0xmm);
    dst0xmm = _mm_adds_epu8(dst0xmm, _mm_srli_si128(dst0xmm, 8));
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
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

    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }
};

} // RasterEngine namespace
} // Fog namespace
