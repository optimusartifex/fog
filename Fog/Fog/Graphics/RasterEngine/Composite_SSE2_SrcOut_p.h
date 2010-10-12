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
// [Fog::RasterEngine::SSE2 - Composite - SrcOut]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeSrcOutSSE2 :
  public CompositeCommonSSE2<CompositeSrcOutSSE2>,
  public CompositeBaseFuncsSSE2<CompositeSrcOutSSE2>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC_OUT };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_negate_1x1W(dst0xmm, dst0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, b0xmm);
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
    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_negate_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm = b0xmm, x1xmm = b1xmm;
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
