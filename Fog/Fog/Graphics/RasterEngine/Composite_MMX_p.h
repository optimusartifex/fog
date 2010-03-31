// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Build/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Dib_MMX_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// How many pixels are needed to use MMX in very simple fills.
#define MMX_FILL_THRESHOLD 9

// ############################################################################
// Implementation starts from here.
// ############################################################################

// ============================================================================
// [Fog::RasterEngine::MMX - Composite - Src]
// ============================================================================

struct FOG_HIDDEN MMX_SYM(CompositeSrc)
{
  // -------------------------------------------------------------------------
  // [CompositeSrcMMX - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;

    if (i >= MMX_FILL_THRESHOLD)
    {
      __m64 src0mm;

      mmx_load4(src0mm, &src->prgb);
      mmx_expand_pixel_lo_1x1B(src0mm, src0mm);

      if ((sysuint_t)dst & 0x07)
      {
        mmx_store4(dst, src0mm);
        dst += 4;
        i--;
      }

      while ((i -= 8) >= 0)
      {
        mmx_store8(dst     , src0mm);
        mmx_store8(dst + 8 , src0mm);
        mmx_store8(dst + 16, src0mm);
        mmx_store8(dst + 24, src0mm);
        dst += 32;
      }
      i += 8;

      while ((i -= 2) >= 0)
      {
        mmx_store8(dst, src0mm);
        dst += 8;
      }
      i += 2;

      if (i) mmx_store4(dst, src0mm);

      mmx_end();
    }
    else
    {
      // If span is very small then it's better to not use MMX
      // (EMMS is expensive).
      uint32_t src0 = src->prgb;
      do {
        ((uint32_t*)dst)[0] = src0;
        dst += 4;
      } while (--i);
    }
  }

  // -------------------------------------------------------------------------
  // [CompositeSrcMMX - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;

    if (i >= MMX_FILL_THRESHOLD)
    {
      __m64 src0mm;

      mmx_load4(src0mm, &src->prgb);
      mmx_fill_alpha_1x1B(src0mm);
      mmx_expand_pixel_lo_1x1B(src0mm, src0mm);

      if ((sysuint_t)dst & 0x07)
      {
        mmx_store4(dst, src0mm);
        dst += 4;
        i--;
      }

      while ((i -= 8) >= 0)
      {
        mmx_store8(dst     , src0mm);
        mmx_store8(dst + 8 , src0mm);
        mmx_store8(dst + 16, src0mm);
        mmx_store8(dst + 24, src0mm);
        dst += 32;
      }
      i += 8;

      while ((i -= 2) >= 0)
      {
        mmx_store8(dst, src0mm);
        dst += 8;
      }
      i += 2;

      if (i) mmx_store4(dst, src0mm);

      mmx_end();
    }
    else
    {
      // If span is very small then it's better to not use MMX
      // (EMMS is expensive).
      uint32_t src0 = src->prgb | 0xFF000000;
      do {
        ((uint32_t*)dst)[0] = src0;
        dst += 4;
      } while (--i);
    }
  }
};

// ============================================================================
// [Fog::RasterEngine::MMX - Composite - SrcOver]
// ============================================================================

struct FOG_HIDDEN MMX_SYM(CompositeSrcOver)
{
  static void FOG_FASTCALL prgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    MMX_BLIT_32x2_INIT(dst, w)

    MMX_BLIT_32x2_SMALL_BEGIN(blt)
      __m64 dst0mm;
      __m64 src0mm;
      __m64 msk0mm;

      mmx_load4(src0mm, src);

      MMX_BLIT_TEST_1_PRGB_PIXEL(src0mm, dst0mm,
        blt_small_fill,
        blt_small_skip);

      mmx_load4(dst0mm, dst);

      mmx_unpack_1x1W(src0mm, src0mm);
      mmx_unpack_1x1W(dst0mm, dst0mm);

      mmx_expand_alpha_1x1W(msk0mm, src0mm);
      mmx_negate_1x1W(msk0mm, msk0mm);

      mmx_muldiv255_1x1W(dst0mm, dst0mm, msk0mm);
      mmx_adds_1x1W(src0mm, src0mm, dst0mm);

      mmx_pack_1x1W(src0mm, src0mm);

blt_small_fill:
      mmx_store4(dst, src0mm);

blt_small_skip:
      dst += 4;
      src += 4;
    MMX_BLIT_32x2_SMALL_END(blt)

    MMX_BLIT_32x2_LARGE_BEGIN(blt)
      __m64 dst0mm, dst1mm;
      __m64 src0mm, src1mm;
      __m64 msk0mm, msk1mm;

      mmx_load8(src0mm, src);

      MMX_BLIT_TEST_2_PRGB_PIXELS(src0mm, dst0mm, dst1mm, 
        blt_large_fill,
        blt_large_skip);

      mmx_load8(dst0mm, dst);

      mmx_unpack_2x1W(src0mm, src1mm, src0mm);
      mmx_unpack_2x1W(dst0mm, dst1mm, dst0mm);

      mmx_expand_alpha_2x1W(msk0mm, src0mm, msk1mm, src1mm);
      mmx_negate_2x1W(msk0mm, msk0mm, msk1mm, msk1mm);

      mmx_muldiv255_2x1W(dst0mm, dst0mm, msk0mm, dst1mm, dst1mm, msk1mm);
      mmx_adds_2x1W(src0mm, src0mm, dst0mm, src1mm, src1mm, dst1mm);

      mmx_pack_2x1W(src0mm, src0mm, src1mm);
blt_large_fill:
      mmx_store8(dst, src0mm);

blt_large_skip:
      dst += 8;
      src += 8;
    MMX_BLIT_32x2_LARGE_END(blt)

    mmx_end();
  }
};

} // RasterEngine namespace
} // Fog namespace
