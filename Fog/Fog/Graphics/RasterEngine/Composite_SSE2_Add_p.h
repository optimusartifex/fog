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
// [Fog::RasterEngine::SSE2 - Composite - Add]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeAddSSE2 :
  public CompositeCommonSSE2<CompositeAddSSE2>,
  public CompositeBaseFuncsSSE2<CompositeAddSSE2>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_ADD };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1B(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
  }


  static FOG_INLINE void prgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x4B(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
  }
};

} // RasterEngine namespace
} // Fog namespace
