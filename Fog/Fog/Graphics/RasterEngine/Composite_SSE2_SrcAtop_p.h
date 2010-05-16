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
// [Fog::RasterEngine::SSE2 - Composite - SrcAtop]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeSrcAtopSSE2 :
  public CompositeCommonSSE2<CompositeSrcAtopSSE2>,
  public CompositeBaseFuncsSSE2<CompositeSrcAtopSSE2>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC_ATOP };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;
    __m128i j0xmm;

    sse2_expand_alpha_1x1W(i0xmm, a0xmm);
    sse2_expand_alpha_1x1W(j0xmm, b0xmm);
    sse2_negate_1x1W(j0xmm, j0xmm);

    sse2_muldiv255_1x1W(i0xmm, i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, j0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, i0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i b0xmm_0xFF = b0xmm;
    sse2_fill_alpha_1x1W(b0xmm_0xFF);

    prgb32_op_xrgb32_0xFF_unpacked_1x1W(dst0xmm, a0xmm, b0xmm_0xFF);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
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
    __m128i i0xmm, i1xmm;
    __m128i j0xmm, j1xmm;

    sse2_expand_alpha_2x2W(i0xmm, a0xmm, i1xmm, a1xmm);
    sse2_expand_alpha_2x2W(j0xmm, b0xmm, j1xmm, b1xmm);
    sse2_negate_2x2W(j0xmm, j0xmm, j1xmm, j1xmm);

    sse2_muldiv255_2x2W(i0xmm, i0xmm, b0xmm, i1xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, j0xmm, dst1xmm, a1xmm, j1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, i0xmm, dst1xmm, dst1xmm, i1xmm);
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
