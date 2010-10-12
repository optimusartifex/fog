// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_MMX_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::MMX - Composite - SrcOver]
// ============================================================================

//! @internal
struct FOG_HIDDEN MMX_SYM(CompositeSrcOver)
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC_OVER };

  // --------------------------------------------------------------------------
  // [CompositeSrcMMX - PRGB32 - VBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_prgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    MMX_LOOP_32x2_INIT()

    MMX_LOOP_32x2_SMALL_BEGIN(opaque)
      __m64 dst0mm;
      __m64 src0mm;
      __m64 msk0mm;

      mmx_load4(src0mm, src);

      MMX_BLIT_TEST_1_PRGB_PIXEL(src0mm, dst0mm,
        opaqueSmallFill,
        opaqueSmallSkip);

      mmx_load4(dst0mm, dst);

      mmx_unpack_1x1W(src0mm, src0mm);
      mmx_unpack_1x1W(dst0mm, dst0mm);

      mmx_expand_alpha_1x1W(msk0mm, src0mm);
      mmx_negate_1x1W(msk0mm, msk0mm);

      mmx_muldiv255_1x1W(dst0mm, dst0mm, msk0mm);
      mmx_adds_1x1W(src0mm, src0mm, dst0mm);
      mmx_pack_1x1W(src0mm, src0mm);

opaqueSmallFill:
      mmx_store4(dst, src0mm);

opaqueSmallSkip:
      dst += 4;
      src += 4;
    MMX_LOOP_32x2_SMALL_END(opaque)

    MMX_LOOP_32x2_MAIN_BEGIN(opaque)
      __m64 dst0mm, dst1mm;
      __m64 src0mm, src1mm;
      __m64 msk0mm, msk1mm;

      mmx_load8(src0mm, src);

      MMX_BLIT_TEST_2_PRGB_PIXELS(src0mm, dst0mm, dst1mm,
        opaqueMainFill,
        opaqueMainSkip);

      mmx_load8(dst0mm, dst);

      mmx_unpack_2x1W(src0mm, src1mm, src0mm);
      mmx_unpack_2x1W(dst0mm, dst1mm, dst0mm);

      mmx_expand_alpha_2x1W(msk0mm, src0mm, msk1mm, src1mm);
      mmx_negate_2x1W(msk0mm, msk0mm, msk1mm, msk1mm);

      mmx_muldiv255_2x1W(dst0mm, dst0mm, msk0mm, dst1mm, dst1mm, msk1mm);
      mmx_adds_2x1W(src0mm, src0mm, dst0mm, src1mm, src1mm, dst1mm);
      mmx_pack_2x1W(src0mm, src0mm, src1mm);

opaqueMainFill:
      mmx_store8(dst, src0mm);

opaqueMainSkip:
      dst += 8;
      src += 8;
    MMX_LOOP_32x2_MAIN_END(opaque)

    mmx_end();
  }
};

} // RasterEngine namespace
} // Fog namespace
