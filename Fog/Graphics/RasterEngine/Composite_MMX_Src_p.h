// [Fog-Graphics Library - Private API]
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
// [Fog::RasterEngine::MMX - Composite - Src]
// ============================================================================

//! @internal
struct FOG_HIDDEN MMX_SYM(CompositeSrc)
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC };

  // --------------------------------------------------------------------------
  // [CompositeSrcMMX - PRGB32 - CBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    if (w >= MMX_FILL_THRESHOLD)
    {
      __m64 src0mm;

      mmx_load4(src0mm, &src->prgb);
      mmx_expand_pixel_lo_1x1B(src0mm, src0mm);

      if ((sysuint_t)dst & 0x07)
      {
        mmx_store4(dst, src0mm);
        dst += 4;
        w--;
      }

      while ((w -= 8) >= 0)
      {
        mmx_store8(dst     , src0mm);
        mmx_store8(dst + 8 , src0mm);
        mmx_store8(dst + 16, src0mm);
        mmx_store8(dst + 24, src0mm);
        dst += 32;
      }
      w += 8;

      while ((w -= 2) >= 0)
      {
        mmx_store8(dst, src0mm);
        dst += 8;
      }
      w += 2;

      if (w) mmx_store4(dst, src0mm);

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
      } while (--w);
    }
  }

  // --------------------------------------------------------------------------
  // [CompositeSrcMMX - XRGB32 - CBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    if (w >= MMX_FILL_THRESHOLD)
    {
      __m64 src0mm;

      mmx_load4(src0mm, &src->prgb);
      mmx_fill_alpha_1x1B(src0mm);
      mmx_expand_pixel_lo_1x1B(src0mm, src0mm);

      if ((sysuint_t)dst & 0x07)
      {
        mmx_store4(dst, src0mm);
        dst += 4;
        w--;
      }

      while ((w -= 8) >= 0)
      {
        mmx_store8(dst     , src0mm);
        mmx_store8(dst + 8 , src0mm);
        mmx_store8(dst + 16, src0mm);
        mmx_store8(dst + 24, src0mm);
        dst += 32;
      }
      w += 8;

      while ((w -= 2) >= 0)
      {
        mmx_store8(dst, src0mm);
        dst += 8;
      }
      w += 2;

      if (w) mmx_store4(dst, src0mm);

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
      } while (--w);
    }
  }
};

} // RasterEngine namespace
} // Fog namespace
