// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITESRC_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITESRC_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/CompositeBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - CompositeSrc]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeSrc
{
  enum { COMBINE_FLAGS = RASTER_COMBINE_OP_SRC };

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Sca.
  // Da'  = Sa.
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    if (pack)
      Acc::p32PackPBB2031FromPBW(dst0p_20, b0p_20, b0p_31);
    else
      Acc::p32Copy_2x(dst0p_20, b0p_20, dst0p_31, b0p_31);
  }

  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    if (pack)
    {
      Acc::p32PackPBB2031FromPBW(dst0p_20, b0p_20, b0p_31);
      Acc::p32FillPBB3(dst0p_20, dst0p_20);
    }
    else
    {
      Acc::p32Copy_2x(dst0p_20, b0p_20, dst0p_31, b0p_31);
      Acc::p32FillPBW1(dst0p_31, dst0p_31);
    }
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dc' = Sca.
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Sa.
  // Da'  = Sa.
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    if (pack)
    {
      Acc::p32ExtendPBBFromSBB(dst0p_20, b0p_a8);
    }
    else
    {
      Acc::p32ExtendPBWFromSBW(dst0p_20, b0p_a8);
      Acc::p32Copy(dst0p_31, dst0p_20);
    }
  }

  // Dc' = Sa.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    if (pack)
    {
      Acc::p32ExtendPBBFromSBB(dst0p_20, b0p_a8);
      Acc::p32FillPBB3(dst0p_20, dst0p_20);
    }
    else
    {
      Acc::p32ExtendPBWFromSBW(dst0p_20, b0p_a8);
      Acc::p32FillPBW1(dst0p_31, b0p_a8);
    }
  }

  // ==========================================================================
  // [Func - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    Acc::p32Copy(dst0p, b0p);
  }




















































  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    CompositeBase::MemFill32Context ctx;

    ctx.init(src->prgb32.u32);
    ctx.fill(dst, w);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sro0p;
    uint32_t sro0p_20, sro0p_31;

    Acc::p32Copy(sro0p, src->prgb32.u32);
    Acc::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    C_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        Acc::p32Store4a(dst, sro0p);
        dst += 4;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Copy(src0p, sro0p);
      Acc::p32Copy(msk0p, msk0);

      Acc::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);
      Acc::p32Negate256SBW(msk0p, msk0p);

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
      BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Copy(src0p, sro0p);
        if (msk0p == 0xFF) goto _A8_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);

        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);

_A8_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_A8_Glyph_Skip:
        dst += 4;
        msk += 1;
      BLIT_LOOP_32x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load2a(msk0p, msk);

        Acc::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);

        Acc::p32Add(src0p, src0p, dst0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        msk += 2;
      BLIT_LOOP_32x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p_20, msk0p_31;

        Acc::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        Acc::p32Copy(src0p, sro0p);
        if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;
        Acc::p32Load4a(dst0p, dst);

        Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32Lerp256PBB_PBW_2031(src0p, sro0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 4;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  // USE: prgb32_cblit_prgb32_full

  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  // USE: prgb32_cblit_prgb32_span

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  // Use: Convert::copy_32

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t src0p;

        Acc::p32Load4a(src0p, src);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 4;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      uint32_t minv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(minv0p, msk0p);

      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;

        Acc::p32Load4a(src0p, src);
        Acc::p32Load4a(dst0p, dst);

        Acc::p32Lerp256PBB_SBW(dst0p, dst0p, src0p, minv0p, msk0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += 4;
      BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Load4a(src0p, src);
        if (msk0p == 0xFF) goto _A8_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);

_A8_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_A8_Glyph_Skip:
        dst += 4;
        src += 4;
        msk += 1;
      BLIT_LOOP_32x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load4a(src0p, src);
        Acc::p32Load2a(msk0p, msk);

        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 4;
        msk += 2;
      BLIT_LOOP_32x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p_20, msk0p_31;

        Acc::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        Acc::p32Load4a(src0p, src);
        if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);

        Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32Lerp256PBB_PBW_2031(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 4;
        src += 4;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Line]
  // ==========================================================================

  static FOG_INLINE void prgb32_vblit_xrgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t src0p;

      Acc::p32Load4a(src0p, src);
      Acc::p32FillPBB3(src0p, src0p);
      Acc::p32Store4a(dst, src0p);

      dst += 4;
      src += 4;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_xrgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t src0p;

        Acc::p32Load4a(src0p, src);
        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 4;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      uint32_t minv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(minv0p, msk0p);

      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;

        Acc::p32Load4a(src0p, src);
        Acc::p32Load4a(dst0p, dst);
        Acc::p32FillPBB3(src0p, src0p);

        Acc::p32Lerp256PBB_SBW(dst0p, dst0p, src0p, minv0p, msk0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += 4;
      BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Load4a(src0p, src);
        Acc::p32FillPBB3(src0p, src0p);
        if (msk0p == 0xFF) goto _A8_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);

_A8_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_A8_Glyph_Skip:
        dst += 4;
        src += 4;
        msk += 1;
      BLIT_LOOP_32x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load4a(src0p, src);
        Acc::p32Load2a(msk0p, msk);

        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 4;
        msk += 2;
      BLIT_LOOP_32x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p_20, msk0p_31;

        Acc::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        Acc::p32Load4a(src0p, src);
        Acc::p32FillPBB3(src0p, src0p);
        if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);

        Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32Lerp256PBB_PBW_2031(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 4;
        src += 4;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }






































  // ==========================================================================
  // [COMP8 - VBlit - A8 - Helpers]
  // ==========================================================================

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a8_c_opaque(
    uint8_t* dst, const uint8_t* src, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t src0p;

      Acc::p32Load1b(src0p, src);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 1;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a8_c_mask(
    uint8_t* dst, const uint8_t* src, const uint32_t& msk0p_a, const uint32_t& msk0p_i, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(C_Mask)
      uint32_t dst0p;
      uint32_t src0p;

      if (DstF::HAS_ALPHA)
      {
        DstF::p32LoadPRGB32(dst0p, dst);
        Acc::p32Load1b(src0p, src);
        Acc::p32MulDiv256SBW(src0p, src0p, msk0p_a);
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p_i);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        DstF::p32LoadXRGB32(dst0p, dst);
        Acc::p32Load1b(src0p, src);
        Acc::p32MulDiv256SBW(src0p, src0p, msk0p_a);
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, msk0p_i);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 1;
    BLIT_LOOP_DstFx1_END(C_Mask)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a8_a8_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(A8_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Load1b(msk0p, msk);
      if (msk0p == 0x00) goto _A8_Glyph_Skip;

      if (DstF::HAS_ALPHA)
        DstF::p32LoadPRGB32(dst0p, dst);
      else
        DstF::p32LoadXRGB32(dst0p, dst);

      Acc::p32Load1b(src0p, src);
      Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
      Acc::p32MulDiv256SBW(src0p, src0p, msk0p);
      Acc::p32Negate256SBW(msk0p, msk0p);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

_A8_Glyph_Skip:
      dst += DstF::SIZE;
      src += 1;
      msk += 1;
    BLIT_LOOP_DstFx1_END(A8_Glyph)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a8_a8_extra(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(A8_Extra)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      if (DstF::HAS_ALPHA)
        DstF::p32LoadPRGB32(dst0p, dst);
      else
        DstF::p32LoadXRGB32(dst0p, dst);

      Acc::p32Load1b(src0p, src);
      Acc::p32Load2a(msk0p, msk);
      Acc::p32MulDiv256SBW(src0p, src0p, msk0p);
      Acc::p32Negate256SBW(msk0p, msk0p);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 1;
      msk += 2;
    BLIT_LOOP_DstFx1_END(A8_Extra)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a8_argb32_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(ARGB32_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p_20, msk0p_31;

      Acc::p32Load4a(msk0p_20, msk);
      if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

      Acc::p32Load1b(src0p, src);
      if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

      if (DstF::HAS_ALPHA)
      {
        DstF::p32LoadPRGB32(dst0p, dst);
        Acc::p32UnpackPBW256FromPBB255_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32And(src0p, src0p, 0xFF);
        Acc::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, msk0p_20, src0p, msk0p_31, src0p);
        Acc::p32MulDiv256PBB_PBW_2031(dst0p, dst0p, msk0p_20, msk0p_31);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        DstF::p32LoadXRGB32(dst0p, dst);
        Acc::p32UnpackPBW256FromPBB255_20Z1(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32And(src0p, src0p, 0xFF);
        Acc::p32MulDiv256PBW_SBW_2x_Pack_20Z1(src0p, msk0p_20, src0p, msk0p_31, src0p);
        Acc::p32MulDiv256PBB_PBW_20Z1(dst0p, dst0p, msk0p_20, msk0p_31);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

_ARGB32_Glyph_Skip:
      dst += DstF::SIZE;
      src += 1;
      msk += 4;
      BLIT_LOOP_DstFx1_CONTINUE(ARGB32_Glyph)

_ARGB32_Glyph_Fill:
      if (DstF::HAS_ALPHA)
      {
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 1;
      msk += 4;
    BLIT_LOOP_DstFx1_END(ARGB32_Glyph)
  }

  // ==========================================================================
  // [COMP8 - VBlit - A8 - Line]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _comp8_vblit_a8_c_opaque<DstF>(dst, src, w);
  }

  // ==========================================================================
  // [COMP8 - VBlit - A8 - Span]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _comp8_vblit_a8_c_opaque<DstF>(dst, src, w);
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      uint32_t inv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(inv0p, msk0p);

      _comp8_vblit_a8_c_mask<DstF>(dst, src, msk0p, inv0p, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      _comp8_vblit_a8_a8_glyph<DstF>(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      _comp8_vblit_a8_a8_extra<DstF>(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      _comp8_vblit_a8_argb32_glyph<DstF>(dst, src, msk, w);
    }

    V_BLIT_SPAN8_END()
  }



  // ==========================================================================
  // [COMP8 - VBlit - I8 - Helpers]
  // ==========================================================================

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_i8_c_opaque(
    uint8_t* dst, const uint8_t* src, int w, const ImagePaletteData* paletteData)
  {
    BLIT_LOOP_DstFx1_INIT()
    const uint32_t* pal = reinterpret_cast<const uint32_t*>(paletteData->data);

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t src0p;

      Acc::p32Load4a(src0p, pal + src[0]);

      if (DstF::HAS_ALPHA)
        DstF::p32StorePRGB32(dst, src0p);
      else
        DstF::p32StoreXRGB32(dst, src0p);

      dst += DstF::SIZE;
      src += 1;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_i8_c_mask(
    uint8_t* dst, const uint8_t* src, const uint32_t& msk0p_a, const uint32_t& msk0p_i, int w, const ImagePaletteData* paletteData)
  {
    BLIT_LOOP_DstFx1_INIT()
    const uint32_t* pal = reinterpret_cast<const uint32_t*>(paletteData->data);

    BLIT_LOOP_DstFx1_BEGIN(C_Mask)
      uint32_t dst0p;
      uint32_t src0p;

      DstF::p32LoadPRGB32(dst0p, dst);
      Acc::p32Load4a(src0p, pal + src[0]);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p_a, msk0p_i);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32Lerp256PBB_SBW_10Z2(src0p, src0p, dst0p, msk0p_a, msk0p_i);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 1;
    BLIT_LOOP_DstFx1_END(C_Mask)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_i8_a8_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w, const ImagePaletteData* paletteData)
  {
    BLIT_LOOP_DstFx1_INIT()
    const uint32_t* pal = reinterpret_cast<const uint32_t*>(paletteData->data);

    BLIT_LOOP_DstFx1_BEGIN(A8_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Load1b(msk0p, msk);
      if (msk0p == 0x00) goto _A8_Glyph_Skip;

      Acc::p32Load4a(src0p, pal + src[0]);
      if (msk0p == 0xFF) goto _A8_Glyph_Fill;

      DstF::p32LoadPRGB32(dst0p, dst);
      Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);

      if (DstF::HAS_ALPHA)
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);
      else
        Acc::p32Lerp256PBB_SBW_10Z2(src0p, src0p, dst0p, msk0p);

_A8_Glyph_Fill:
      if (DstF::HAS_ALPHA)
        DstF::p32StorePRGB32(dst, src0p);
      else
        DstF::p32StoreZRGB32(dst, src0p);

_A8_Glyph_Skip:
      dst += DstF::SIZE;
      src += 1;
      msk += 1;
    BLIT_LOOP_DstFx1_END(A8_Glyph)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_i8_a8_extra(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w, const ImagePaletteData* paletteData)
  {
    BLIT_LOOP_DstFx1_INIT()
    const uint32_t* pal = reinterpret_cast<const uint32_t*>(paletteData->data);

    BLIT_LOOP_DstFx1_BEGIN(A8_Extra)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      DstF::p32LoadPRGB32(dst0p, dst);
      Acc::p32Load4a(src0p, pal + src[0]);
      Acc::p32Load2a(msk0p, msk);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32Lerp256PBB_SBW_10Z2(src0p, src0p, dst0p, msk0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 1;
      msk += 2;
    BLIT_LOOP_DstFx1_END(A8_Extra)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_i8_argb32_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w, const ImagePaletteData* paletteData)
  {
    BLIT_LOOP_DstFx1_INIT()
    const uint32_t* pal = reinterpret_cast<const uint32_t*>(paletteData->data);

    BLIT_LOOP_DstFx1_BEGIN(ARGB32_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p_20, msk0p_31;

      Acc::p32Load4a(msk0p_20, msk);
      if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

      Acc::p32Load4a(src0p, pal + src[0]);
      if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

      DstF::p32LoadPRGB32(dst0p, dst);
      Acc::p32UnpackPBW256FromPBB255_2031(msk0p_20, msk0p_31, msk0p_20);

      if (DstF::HAS_ALPHA)
        Acc::p32Lerp256PBB_PBW_2031(src0p, src0p, dst0p, msk0p_20, msk0p_31);
      else
        Acc::p32Lerp256PBB_PBW_20Z1(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
      if (DstF::HAS_ALPHA)
        DstF::p32StorePRGB32(dst, src0p);
      else
        DstF::p32StoreZRGB32(dst, src0p);

_ARGB32_Glyph_Skip:
      dst += DstF::SIZE;
      src += 1;
      msk += 4;
    BLIT_LOOP_DstFx1_END(ARGB32_Glyph)
  }

  // ==========================================================================
  // [COMP8 - VBlit - I8 - Line]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_i8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _comp8_vblit_i8_c_opaque<DstF>(dst, src, w, closure->palette);
  }

  // ==========================================================================
  // [COMP8 - VBlit - I8 - Span]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_i8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _comp8_vblit_i8_c_opaque<DstF>(dst, src, w, closure->palette);
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      uint32_t inv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(inv0p, msk0p);

      _comp8_vblit_i8_c_mask<DstF>(dst, src, msk0p, inv0p, w, closure->palette);
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      _comp8_vblit_i8_a8_glyph<DstF>(dst, src, msk, w, closure->palette);
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      _comp8_vblit_i8_a8_extra<DstF>(dst, src, msk, w, closure->palette);
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      _comp8_vblit_i8_argb32_glyph<DstF>(dst, src, msk, w, closure->palette);
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [COMP8 - VBlit - PRGB64 - Helpers]
  // ==========================================================================

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_prgb64_c_opaque(
    uint8_t* dst, const uint8_t* src, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t src0p_10, src0p_32;

      Acc::p32Load8a(src0p_10, src0p_32, src);
      Acc::p32PRGB32FromPRGB64_1032(src0p_10, src0p_10, src0p_32);
      Acc::p32Store4a(dst, src0p_10);

      dst += 4;
      src += 8;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_prgb64_c_mask(
    uint8_t* dst, const uint8_t* src, const uint32_t& msk0p_a, const uint32_t& msk0p_i, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(C_Mask)
      uint32_t dst0p;
      uint32_t src0p_10, src0p_32;

      DstF::p32LoadPRGB32(dst0p, dst);
      Acc::p32Load8a(src0p_10, src0p_32, src);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32PRGB32FromPRGB64_1032(src0p_10, src0p_10, src0p_32);
        Acc::p32Lerp256PBB_SBW(src0p_10, src0p_10, dst0p, msk0p_a, msk0p_i);
        DstF::p32StorePRGB32(dst, src0p_10);
      }
      else
      {
        Acc::p32ZRGB32FromPRGB64_1032(src0p_10, src0p_10, src0p_32);
        Acc::p32Lerp256PBB_SBW_10Z2(src0p_10, src0p_10, dst0p, msk0p_a, msk0p_i);
        DstF::p32StoreZRGB32(dst, src0p_10);
      }

      dst += DstF::SIZE;
      src += 8;
    BLIT_LOOP_DstFx1_END(C_Mask)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_prgb64_a8_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(A8_Glyph)
      uint32_t dst0p;
      uint32_t src0p_10, src0p_32;
      uint32_t msk0p;

      Acc::p32Load1b(msk0p, msk);
      if (msk0p == 0x00) goto _A8_Glyph_Skip;

      Acc::p32Load8a(src0p_10, src0p_32, src);
      if (DstF::HAS_ALPHA)
        Acc::p32PRGB32FromPRGB64_1032(src0p_10, src0p_10, src0p_32);
      else
        Acc::p32ZRGB32FromPRGB64_1032(src0p_10, src0p_10, src0p_32);

      if (msk0p == 0xFF) goto _A8_Glyph_Fill;

      DstF::p32LoadPRGB32(dst0p, dst);
      Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);

      if (DstF::HAS_ALPHA)
        Acc::p32Lerp256PBB_SBW(src0p_10, src0p_10, dst0p, msk0p);
      else
        Acc::p32Lerp256PBB_SBW_10Z2(src0p_10, src0p_10, dst0p, msk0p);

_A8_Glyph_Fill:
      if (DstF::HAS_ALPHA)
        DstF::p32StorePRGB32(dst, src0p_10);
      else
        DstF::p32StoreZRGB32(dst, src0p_10);

_A8_Glyph_Skip:
      dst += DstF::SIZE;
      src += 8;
      msk += 1;
    BLIT_LOOP_DstFx1_END(A8_Glyph)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_prgb64_a8_extra(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(A8_Extra)
      uint32_t dst0p;
      uint32_t src0p_10, src0p_32;
      uint32_t msk0p;

      DstF::p32LoadPRGB32(dst0p, dst);
      Acc::p32Load8a(src0p_10, src0p_32, src);
      Acc::p32Load2a(msk0p, msk);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32PRGB32FromPRGB64_1032(src0p_10, src0p_10, src0p_32);
        Acc::p32Lerp256PBB_SBW(src0p_10, src0p_10, dst0p, msk0p);
        DstF::p32StorePRGB32(dst, src0p_10);
      }
      else
      {
        Acc::p32ZRGB32FromPRGB64_1032(src0p_10, src0p_10, src0p_32);
        Acc::p32Lerp256PBB_SBW_10Z2(src0p_10, src0p_10, dst0p, msk0p);
        DstF::p32StoreZRGB32(dst, src0p_10);
      }

      dst += DstF::SIZE;
      src += 8;
      msk += 2;
    BLIT_LOOP_DstFx1_END(A8_Extra)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_prgb64_argb32_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(ARGB32_Glyph)
      uint32_t dst0p;
      uint32_t src0p_10, src0p_32;
      uint32_t msk0p_20, msk0p_31;

      Acc::p32Load4a(msk0p_20, msk);
      if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

      Acc::p32Load8a(src0p_10, src0p_32, src);
      if (DstF::HAS_ALPHA)
        Acc::p32PRGB32FromPRGB64_1032(src0p_10, src0p_10, src0p_32);
      else
        Acc::p32ZRGB32FromPRGB64_1032(src0p_10, src0p_10, src0p_32);

      if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

      DstF::p32LoadPRGB32(dst0p, dst);
      Acc::p32UnpackPBW256FromPBB255_2031(msk0p_20, msk0p_31, msk0p_20);

      if (DstF::HAS_ALPHA)
        Acc::p32Lerp256PBB_PBW_2031(src0p_10, src0p_10, dst0p, msk0p_20, msk0p_31);
      else
        Acc::p32Lerp256PBB_PBW_20Z1(src0p_10, src0p_10, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
      if (DstF::HAS_ALPHA)
        DstF::p32StorePRGB32(dst, src0p_10);
      else
        DstF::p32StoreZRGB32(dst, src0p_10);

_ARGB32_Glyph_Skip:
      dst += DstF::SIZE;
      src += 8;
      msk += 4;
    BLIT_LOOP_DstFx1_END(ARGB32_Glyph)
  }

  // ==========================================================================
  // [COMP8 - VBlit - PRGB64 - Line]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_prgb64_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _comp8_vblit_prgb64_c_opaque<DstF>(dst, src, w);
  }

  // ==========================================================================
  // [COMP8 - VBlit - PRGB64 - Span]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_prgb64_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _comp8_vblit_prgb64_c_opaque<DstF>(dst, src, w);
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      uint32_t inv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(inv0p, msk0p);

      _comp8_vblit_prgb64_c_mask<DstF>(dst, src, msk0p, inv0p, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      _comp8_vblit_prgb64_a8_glyph<DstF>(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      _comp8_vblit_prgb64_a8_extra<DstF>(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      _comp8_vblit_prgb64_argb32_glyph<DstF>(dst, src, msk, w);
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [COMP8 - VBlit - RGB48 - Helpers]
  // ==========================================================================

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_rgb48_c_opaque(
    uint8_t* dst, const uint8_t* src, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t src0p;

      if (DstF::HAS_ALPHA)
      {
        Acc::p32RGB48LoadToFRGB32(src0p, src);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32RGB48LoadToZRGB32(src0p, src);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 6;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_rgb48_c_mask(
    uint8_t* dst, const uint8_t* src, const uint32_t& msk0p_a, const uint32_t& msk0p_i, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(C_Mask)
      uint32_t dst0p;
      uint32_t src0p;

      if (DstF::HAS_ALPHA)
      {
        DstF::p32LoadPRGB32(dst0p, dst);
        Acc::p32RGB48LoadToFRGB32(src0p, src);
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p_a, msk0p_i);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        DstF::p32LoadXRGB32(dst0p, dst);
        Acc::p32RGB48LoadToZRGB32(src0p, src);
        Acc::p32Lerp256PBB_SBW_10Z2(src0p, src0p, dst0p, msk0p_a, msk0p_i);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 6;
    BLIT_LOOP_DstFx1_END(C_Mask)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_rgb48_a8_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(A8_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Load1b(msk0p, msk);
      if (msk0p == 0x00) goto _A8_Glyph_Skip;

      if (DstF::HAS_ALPHA)
        Acc::p32RGB48LoadToFRGB32(src0p, src);
      else
        Acc::p32RGB48LoadToZRGB32(src0p, src);
      if (msk0p == 0xFF) goto _A8_Glyph_Fill;

      if (DstF::HAS_ALPHA)
      {
        DstF::p32LoadPRGB32(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);
      }
      else
      {
        DstF::p32LoadXRGB32(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Lerp256PBB_SBW_10Z2(src0p, src0p, dst0p, msk0p);
      }

_A8_Glyph_Fill:
      if (DstF::HAS_ALPHA)
        DstF::p32StorePRGB32(dst, src0p);
      else
        DstF::p32StoreZRGB32(dst, src0p);

_A8_Glyph_Skip:
      dst += DstF::SIZE;
      src += 6;
      msk += 1;
    BLIT_LOOP_DstFx1_END(A8_Glyph)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_rgb48_a8_extra(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(A8_Extra)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      if (DstF::HAS_ALPHA)
      {
        DstF::p32LoadPRGB32(dst0p, dst);
        Acc::p32RGB48LoadToFRGB32(src0p, src);
        Acc::p32Load2a(msk0p, msk);
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);
      }
      else
      {
        DstF::p32LoadXRGB32(dst0p, dst);
        Acc::p32RGB48LoadToZRGB32(src0p, src);
        Acc::p32Load2a(msk0p, msk);
        Acc::p32Lerp256PBB_SBW_10Z2(src0p, src0p, dst0p, msk0p);
      }

      dst += DstF::SIZE;
      src += 6;
      msk += 2;
    BLIT_LOOP_DstFx1_END(A8_Extra)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_rgb48_argb32_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(ARGB32_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p_20, msk0p_31;

      Acc::p32Load4a(msk0p_20, msk);
      if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

      if (DstF::HAS_ALPHA)
        Acc::p32RGB48LoadToFRGB32(src0p, src);
      else
        Acc::p32RGB48LoadToZRGB32(src0p, src);
      if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

      if (DstF::HAS_ALPHA)
      {
        DstF::p32LoadPRGB32(dst0p, dst);
        Acc::p32UnpackPBW256FromPBB255_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Lerp256PBB_PBW_2031(src0p, src0p, dst0p, msk0p_20, msk0p_31);
      }
      else
      {
        DstF::p32LoadXRGB32(dst0p, dst);
        Acc::p32UnpackPBW256FromPBB255_20Z1(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Lerp256PBB_PBW_20Z1(src0p, src0p, dst0p, msk0p_20, msk0p_31);
      }


_ARGB32_Glyph_Fill:
      if (DstF::HAS_ALPHA)
        DstF::p32StorePRGB32(dst, src0p);
      else
        DstF::p32StoreZRGB32(dst, src0p);

_ARGB32_Glyph_Skip:
      dst += DstF::SIZE;
      src += 6;
      msk += 4;
    BLIT_LOOP_DstFx1_END(ARGB32_Glyph)
  }

  // ==========================================================================
  // [COMP8 - VBlit - RGB48 - Line]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_rgb48_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _comp8_vblit_rgb48_c_opaque<DstF>(dst, src, w);
  }

  // ==========================================================================
  // [COMP8 - VBlit - RGB48 - Span]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_rgb48_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _comp8_vblit_rgb48_c_opaque<DstF>(dst, src, w);
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      uint32_t inv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(inv0p, msk0p);

      _comp8_vblit_rgb48_c_mask<DstF>(dst, src, msk0p, inv0p, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      _comp8_vblit_rgb48_a8_glyph<DstF>(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      _comp8_vblit_rgb48_a8_extra<DstF>(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      _comp8_vblit_rgb48_argb32_glyph<DstF>(dst, src, msk, w);
    }

    V_BLIT_SPAN8_END()
  }



  // ==========================================================================
  // [COMP8 - VBlit - A16 - Helpers]
  // ==========================================================================

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a16_c_opaque(
    uint8_t* dst, const uint8_t* src, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t src0p;

      Acc::p32Load2a(src0p, src);
      Acc::p32RShift(src0p, src0p, 8);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 2;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a16_c_mask(
    uint8_t* dst, const uint8_t* src, const uint32_t& msk0p_a, const uint32_t& msk0p_i, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(C_Mask)
      uint32_t dst0p;
      uint32_t src0p;

      if (DstF::HAS_ALPHA)
      {
        DstF::p32LoadPRGB32(dst0p, dst);
        Acc::p32Load2a(src0p, src);
        Acc::p32Mul(src0p, src0p, msk0p_a);
        Acc::p32RShift(src0p, src0p, 16);
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p_i);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        DstF::p32LoadXRGB32(dst0p, dst);
        Acc::p32Load2a(src0p, src);
        Acc::p32Mul(src0p, src0p, msk0p_a);
        Acc::p32RShift(src0p, src0p, 16);
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, msk0p_i);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 2;
    BLIT_LOOP_DstFx1_END(C_Mask)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a16_a8_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(A8_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Load1b(msk0p, msk);
      if (msk0p == 0x00) goto _A8_Glyph_Skip;

      if (DstF::HAS_ALPHA)
        DstF::p32LoadPRGB32(dst0p, dst);
      else
        DstF::p32LoadXRGB32(dst0p, dst);

      Acc::p32Load2a(src0p, src);
      Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
      Acc::p32Mul(src0p, src0p, msk0p);
      Acc::p32RShift(src0p, src0p, 16);
      Acc::p32Negate256SBW(msk0p, msk0p);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

_A8_Glyph_Skip:
      dst += DstF::SIZE;
      src += 2;
      msk += 1;
    BLIT_LOOP_DstFx1_END(A8_Glyph)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a16_a8_extra(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(A8_Extra)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      if (DstF::HAS_ALPHA)
        DstF::p32LoadPRGB32(dst0p, dst);
      else
        DstF::p32LoadXRGB32(dst0p, dst);

      Acc::p32Load2a(src0p, src);
      Acc::p32Load2a(msk0p, msk);
      Acc::p32Mul(src0p, src0p, msk0p);
      Acc::p32RShift(src0p, src0p, 16);
      Acc::p32Negate256SBW(msk0p, msk0p);

      if (DstF::HAS_ALPHA)
      {
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 2;
      msk += 2;
    BLIT_LOOP_DstFx1_END(A8_Extra)
  }

  template<typename DstF>
  static FOG_INLINE void _comp8_vblit_a16_argb32_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(ARGB32_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p_20, msk0p_31;

      Acc::p32Load4a(msk0p_20, msk);
      if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

      Acc::p32Load2a(src0p, src);
      Acc::p32RShift(src0p, src0p, 8);
      if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

      if (DstF::HAS_ALPHA)
      {
        DstF::p32LoadPRGB32(dst0p, dst);
        Acc::p32UnpackPBW256FromPBB255_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32And(src0p, src0p, 0xFF);
        Acc::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, msk0p_20, src0p, msk0p_31, src0p);
        Acc::p32MulDiv256PBB_PBW_2031(dst0p, dst0p, msk0p_20, msk0p_31);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        DstF::p32LoadXRGB32(dst0p, dst);
        Acc::p32UnpackPBW256FromPBB255_20Z1(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32And(src0p, src0p, 0xFF);
        Acc::p32MulDiv256PBW_SBW_2x_Pack_20Z1(src0p, msk0p_20, src0p, msk0p_31, src0p);
        Acc::p32MulDiv256PBB_PBW_20Z1(dst0p, dst0p, msk0p_20, msk0p_31);
        Acc::p32Add(src0p, src0p, dst0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

_ARGB32_Glyph_Skip:
      dst += DstF::SIZE;
      src += 2;
      msk += 4;
      BLIT_LOOP_DstFx1_CONTINUE(ARGB32_Glyph)

_ARGB32_Glyph_Fill:
      if (DstF::HAS_ALPHA)
      {
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        DstF::p32StorePRGB32(dst, src0p);
      }
      else
      {
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        DstF::p32StoreZRGB32(dst, src0p);
      }

      dst += DstF::SIZE;
      src += 2;
      msk += 4;
    BLIT_LOOP_DstFx1_END(ARGB32_Glyph)
  }

  // ==========================================================================
  // [COMP8 - VBlit - A16 - Line]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_a16_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _comp8_vblit_a16_c_opaque<DstF>(dst, src, w);
  }

  // ==========================================================================
  // [COMP8 - VBlit - A16 - Span]
  // ==========================================================================

  template<typename DstF>
  static void FOG_FASTCALL comp8_vblit_a16_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _comp8_vblit_a16_c_opaque<DstF>(dst, src, w);
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      uint32_t inv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(inv0p, msk0p);

      _comp8_vblit_a16_c_mask<DstF>(dst, src, msk0p, inv0p, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      _comp8_vblit_a16_a8_glyph<DstF>(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      _comp8_vblit_a16_a8_extra<DstF>(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      _comp8_vblit_a16_argb32_glyph<DstF>(dst, src, msk, w);
    }

    V_BLIT_SPAN8_END()
  }














































  // ==========================================================================
  // [FRGB32 - VBlit - RGB24 - Helpers]
  // ==========================================================================

  static FOG_INLINE void _frgb32_vblit_rgb24_c_opaque(
    uint8_t* dst, const uint8_t* src, int w)
  {
#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
    // X86/X64 Specific code relying on unaligned read. It seems that this is
    // really unbeatable by the any other code except SSSE3 using PSHUFB.
    uint32_t src0p;

    // We know that each scanline should be aligned to 32-bits so reading one
    // more BYTE than scanline has should never fail, but I'd like to make
    // this code bug-free and valgrind-ready.
    FOG_ASSUME(w > 0);

    while (--w)
    {
      Acc::p32Load4u(src0p, src);
      Acc::p32FillPBB3(src0p, src0p);
      Acc::p32Store4a(dst, src0p);

      dst += 4;
      src += 3;
    }

    Acc::p32Load3b(src0p, src);
    Acc::p32FillPBB3(src0p, src0p);
    Acc::p32Store4a(dst, src0p);
#else
    BLIT_LOOP_32x4_VS_24_INIT()

    BLIT_LOOP_32x4_VS_24_SMALL_BEGIN(C_Opaque)
      uint32_t src0p;

      Acc::p32Load3b(src0p, src);
      Acc::p32FillPBB3(src0p, src0p);
      Acc::p32Store4a(dst, src0p);

      dst += 4;
      src += 3;
    BLIT_LOOP_32x4_VS_24_SMALL_END(C_Opaque)

    BLIT_LOOP_32x4_VS_24_MAIN_BEGIN(C_Opaque)
      Acc::p32FRGB32QuadFromRGB24Quad(
        ((uint32_t*)dst)[0], ((uint32_t*)dst)[1], ((uint32_t*)dst)[2], ((uint32_t*)dst)[3],
        ((uint32_t*)src)[0], ((uint32_t*)src)[1], ((uint32_t*)src)[2]);

      dst += 16;
      src += 12;
    BLIT_LOOP_32x4_VS_24_MAIN_END(C_Opaque)
#endif // FOG_ARCH_X86 || FOG_ARCH_X86_64
  }

  static FOG_INLINE void _frgb32_vblit_rgb24_c_mask(
    uint8_t* dst, const uint8_t* src, const uint32_t& msk0p_a, const uint32_t& msk0p_i, int w)
  {
#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
    // X86/X64 Specific code relying on unaligned read. It seems that this is
    // really unbeatable by the any other code except SSSE3 using PSHUFB.
    uint32_t src0p;
    uint32_t dst0p;

    // We know that each scanline should be aligned to 32-bits so reading one
    // more BYTE than scanline has should never fail, but I'd like to make
    // this code bug-free and valgrind-ready.
    FOG_ASSUME(w > 0);

    while (--w)
    {
      Acc::p32Load4a(dst0p, dst);
      Acc::p32Load4u(src0p, src);
      Acc::p32FillPBB3(src0p, src0p);
      Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p_a, msk0p_i);
      Acc::p32Store4a(dst, src0p);

      dst += 4;
      src += 3;
    }

    Acc::p32Load4a(dst0p, dst);
    Acc::p32Load3b(src0p, src);
    Acc::p32FillPBB3(src0p, src0p);
    Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p_a, msk0p_i);
    Acc::p32Store4a(dst, src0p);
#else
    BLIT_LOOP_32x4_VS_24_INIT()

    BLIT_LOOP_32x4_VS_24_SMALL_BEGIN(C_Opaque)
      uint32_t dst0p;
      uint32_t src0p;

      Acc::p32Load4a(dst0p, dst);
      Acc::p32Load3b(src0p, src);
      Acc::p32FillPBB3(src0p, src0p);
      Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p_a, msk0p_i);
      Acc::p32Store4a(dst, src0p);

      dst += 4;
      src += 3;
    BLIT_LOOP_32x4_VS_24_SMALL_END(C_Opaque)

    BLIT_LOOP_32x4_VS_24_MAIN_BEGIN(C_Opaque)
      uint32_t dst0p, dst1p, dst2p, dst3p;
      uint32_t src0p, src1p, src2p, src3p;

      Acc::p32FRGB32QuadFromRGB24Quad(
        src0p, src1p, src2p, src3p, ((uint32_t*)src)[0], ((uint32_t*)src)[1], ((uint32_t*)src)[2]);

      Acc::p32Load4a(dst0p, dst + 0);
      Acc::p32Load4a(dst1p, dst + 4);
      Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p_a, msk0p_i);
      Acc::p32Lerp256PBB_SBW(src1p, src1p, dst1p, msk0p_a, msk0p_i);
      Acc::p32Store4a(dst + 0, src0p);
      Acc::p32Store4a(dst + 4, src1p);

      Acc::p32Load4a(dst2p, dst + 8);
      Acc::p32Load4a(dst3p, dst + 12);
      Acc::p32Lerp256PBB_SBW(src2p, src2p, dst2p, msk0p_a, msk0p_i);
      Acc::p32Lerp256PBB_SBW(src3p, src3p, dst3p, msk0p_a, msk0p_i);
      Acc::p32Store4a(dst + 8, src2p);
      Acc::p32Store4a(dst + 12, src3p);

      dst += 16;
      src += 12;
    BLIT_LOOP_32x4_VS_24_MAIN_END(C_Opaque)
#endif // FOG_ARCH_X86 || FOG_ARCH_X86_64
  }

  static FOG_INLINE void _frgb32_vblit_rgb24_a8_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(A8_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Load1b(msk0p, msk);
      if (msk0p == 0x00) goto _A8_Glyph_Skip;

      Acc::p32Load3b(src0p, src);
      Acc::p32FillPBB3(src0p, src0p);
      if (msk0p == 0xFF) goto _A8_Glyph_Fill;

      Acc::p32Load4a(dst0p, dst);
      Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
      Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);

_A8_Glyph_Fill:
      Acc::p32Store4a(dst, src0p);

_A8_Glyph_Skip:
      dst += 4;
      src += 3;
      msk += 1;
    BLIT_LOOP_32x1_END(A8_Glyph)
  }

  static FOG_INLINE void _frgb32_vblit_rgb24_a8_extra(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(A8_Extra)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Load4a(dst0p, dst);
      Acc::p32Load3b(src0p, src);
      Acc::p32Load2a(msk0p, msk);

      Acc::p32FillPBB3(src0p, src0p);
      Acc::p32Lerp256PBB_SBW(dst0p, src0p, dst0p, msk0p);
      Acc::p32Store4a(dst, src0p);

      dst += 4;
      src += 3;
      msk += 2;
    BLIT_LOOP_32x1_END(A8_Extra)
  }

  static FOG_INLINE void _frgb32_vblit_rgb24_argb32_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p_20, msk0p_31;

      Acc::p32Load4a(msk0p_20, msk);
      if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

      Acc::p32Load3b(src0p, src);
      Acc::p32FillPBB3(src0p, src0p);
      if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

      Acc::p32Load4a(dst0p, dst);

      Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
      Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
      Acc::p32Lerp256PBB_PBW_2031(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
      Acc::p32Store4a(dst, src0p);

_ARGB32_Glyph_Skip:
      dst += 4;
      src += 3;
      msk += 4;
    BLIT_LOOP_32x1_END(ARGB32_Glyph)
  }

  // ==========================================================================
  // [FRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  static void FOG_FASTCALL frgb32_vblit_rgb24_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _frgb32_vblit_rgb24_c_opaque(dst, src, w);
  }

  // ==========================================================================
  // [FRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  static void FOG_FASTCALL frgb32_vblit_rgb24_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _frgb32_vblit_rgb24_c_opaque(dst, src, w);
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      uint32_t inv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(inv0p, msk0p);

      _frgb32_vblit_rgb24_c_mask(dst, src, msk0p, inv0p, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      _frgb32_vblit_rgb24_a8_glyph(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      _frgb32_vblit_rgb24_a8_extra(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      _frgb32_vblit_rgb24_argb32_glyph(dst, src, msk, w);
    }

    V_BLIT_SPAN8_END()
  }
























  // ==========================================================================
  // XPRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    CompositeBase::MemFill32Context ctx;

    ctx.init(src->prgb32.u32 | 0xFF000000);
    ctx.fill(dst, w);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sro0p;
    uint32_t sro0p_20, sro0p_31;

    Acc::p32Copy(sro0p, src->prgb32.u32);
    Acc::p32FillPBB3(sro0p, sro0p);

    Acc::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    C_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        Acc::p32Store4a(dst, sro0p);
        dst += 4;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Copy(src0p, sro0p);
      Acc::p32Copy(msk0p, msk0);

      Acc::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);
      Acc::p32Negate256SBW(msk0p, msk0p);

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
      BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Copy(src0p, sro0p);
        if (msk0p == 0xFF) goto _A8_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);

        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);

        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(src0p, src0p, dst0p);

_A8_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_A8_Glyph_Skip:
        dst += 4;
        msk += 1;
      BLIT_LOOP_32x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load2a(msk0p, msk);

        Acc::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);

        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);

        Acc::p32Add(src0p, src0p, dst0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        msk += 2;
      BLIT_LOOP_32x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p_20, msk0p_31;

        Acc::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        Acc::p32Copy(src0p, sro0p);
        if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;
        Acc::p32Load4a(dst0p, dst);

        Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32Lerp256PBB_PBW_20F1(src0p, sro0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 4;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // XPRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  // USE: xrgb32_cblit_prgb32_full

  // ==========================================================================
  // [XRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  // USE: xrgb32_cblit_prgb32_span

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  // ...

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t src0p;

        Acc::p32Load4a(src0p, src);
        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 4;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      uint32_t inv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(inv0p, msk0p);

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load4a(src0p, src);

        Acc::p32Lerp256PBB_SBW_10F2(dst0p, dst0p, src0p, inv0p, msk0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += 4;
      BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Load4a(src0p, src);
        if (msk0p == 0xFF) goto _A8_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Lerp256PBB_SBW_10Z2(src0p, src0p, dst0p, msk0p);

_A8_Glyph_Fill:
        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

_A8_Glyph_Skip:
        dst += 4;
        src += 4;
        msk += 1;
      BLIT_LOOP_32x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load4a(src0p, src);
        Acc::p32Load2a(msk0p, msk);

        Acc::p32Lerp256PBB_SBW_10F2(src0p, src0p, dst0p, msk0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 4;
        msk += 2;
      BLIT_LOOP_32x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p_20, msk0p_31;

        Acc::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        Acc::p32Load4a(src0p, src);
        if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);

        Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32Lerp256PBB_PBW_20Z1(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 4;
        src += 4;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [RGB24 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_24x4_INIT()

    uint32_t sro0p;
    uint32_t sro0p_packed_0;
    uint32_t sro0p_packed_1;
    uint32_t sro0p_packed_2;

    Acc::p32Copy(sro0p, src->prgb32.u32);
    Acc::p32RGB24QuadFromXRGB32Solid(sro0p_packed_0, sro0p_packed_1, sro0p_packed_2, sro0p);

    BLIT_LOOP_24x4_SMALL_BEGIN(C_Opaque)
      Acc::p32Store3b(dst, sro0p);

      dst += 3;
    BLIT_LOOP_24x4_SMALL_END(C_Opaque)

    BLIT_LOOP_24x4_MAIN_BEGIN(C_Opaque)
      Acc::p32Store4a(dst + 0, sro0p_packed_0);
      Acc::p32Store4a(dst + 4, sro0p_packed_1);
      Acc::p32Store4a(dst + 8, sro0p_packed_2);

      dst += 12;
    BLIT_LOOP_24x4_MAIN_END(C_Opaque)
  }

  // ==========================================================================
  // [RGB24 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sro0p;
    Acc::p32Copy(sro0p, src->prgb32.u32);
    Acc::p32ZeroPBB3(sro0p, sro0p);

    uint32_t sro0p_packed_0;
    uint32_t sro0p_packed_1;
    uint32_t sro0p_packed_2;
    Acc::p32RGB24QuadFromXRGB32Solid(sro0p_packed_0, sro0p_packed_1, sro0p_packed_2, sro0p);

    C_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_24x4_INIT()

      BLIT_LOOP_24x4_SMALL_BEGIN(C_Opaque)
        Acc::p32Store3b(dst, sro0p);

        dst += 3;
      BLIT_LOOP_24x4_SMALL_END(C_Opaque)

      BLIT_LOOP_24x4_MAIN_BEGIN(C_Opaque)
        Acc::p32Store4a(dst + 0, sro0p_packed_0);
        Acc::p32Store4a(dst + 4, sro0p_packed_1);
        Acc::p32Store4a(dst + 8, sro0p_packed_2);

        dst += 12;
      BLIT_LOOP_24x4_MAIN_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_24x4_INIT()

      uint32_t msk0p;
      Acc::p32Copy(msk0p, msk0);

      uint32_t src0p;
      uint32_t src0p_packed_0;
      uint32_t src0p_packed_1;
      uint32_t src0p_packed_2;

      Acc::p32Copy(src0p, sro0p);
      Acc::p32MulDiv256PBB_SBW(src0p, src0p, msk0p);
      Acc::p32Negate256SBW(msk0p, msk0p);

      BLIT_LOOP_24x4_SMALL_BEGIN(C_Mask)
        uint32_t dst0p;

        Acc::p32Load3b(dst0p, dst);
        Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store3b(dst, dst0p);

        dst += 3;
      BLIT_LOOP_24x4_SMALL_END(C_Mask)

      Acc::p32RGB24QuadFromXRGB32Solid(src0p_packed_0, src0p_packed_1, src0p_packed_2, src0p);

      BLIT_LOOP_24x4_MAIN_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t dst1p;
        uint32_t dst2p;

        Acc::p32Load4a(dst0p, dst + 0);
        Acc::p32Load4a(dst1p, dst + 4);
        Acc::p32Load4a(dst2p, dst + 8);

        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32MulDiv256PBB_SBW(dst1p, dst1p, msk0p);
        Acc::p32MulDiv256PBB_SBW(dst2p, dst2p, msk0p);

        Acc::p32Add(dst0p, dst0p, src0p_packed_0);
        Acc::p32Add(dst1p, dst1p, src0p_packed_1);
        Acc::p32Add(dst2p, dst2p, src0p_packed_2);

        Acc::p32Store4a(dst + 0, dst0p);
        Acc::p32Store4a(dst + 4, dst1p);
        Acc::p32Store4a(dst + 8, dst2p);

        dst += 12;
      BLIT_LOOP_24x4_MAIN_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_24x4_INIT()

      BLIT_LOOP_24x4_SMALL_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load3b(dst0p, dst);
        Acc::p32Load1b(msk0p, msk);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32MulDiv256PBB_SBW_Z210(src0p, sro0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store3b(dst, dst0p);

        dst += 3;
        msk += 1;
      BLIT_LOOP_24x4_SMALL_END(A8_Glyph)

      BLIT_LOOP_24x4_MAIN_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t dst1p;
        uint32_t dst2p;

        uint32_t msk0p_4a, msk0p_4b;
        Acc::p32Load4u(msk0p_4a, msk);

        if (msk0p_4a == 0x00000000) goto _A8_Glyph_Skip;
        Acc::p32Negate255PBB(msk0p_4a, msk0p_4a);
        if (msk0p_4a == 0x00000000) goto _A8_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst + 0);
        Acc::p32Load4a(dst1p, dst + 4);
        Acc::p32Load4a(dst2p, dst + 8);

        Acc::p32UnpackPBW256FromPBB255_2031(msk0p_4a, msk0p_4b, msk0p_4a);
        Acc::p32RGB24LerpTriadSolid_PBW256_PMA8(
          dst0p, dst1p, dst2p,
          dst0p, dst1p, dst2p,
          sro0p, msk0p_4a, msk0p_4b);

        Acc::p32Store4a(dst + 0, dst0p);
        Acc::p32Store4a(dst + 4, dst1p);
        Acc::p32Store4a(dst + 8, dst2p);

_A8_Glyph_Skip:
        dst += 12;
        msk += 4;
        BLIT_LOOP_24x4_MAIN_CONTINUE(A8_Glyph)

_A8_Glyph_Fill:
        Acc::p32Store4a(dst + 0, sro0p_packed_0);
        Acc::p32Store4a(dst + 4, sro0p_packed_1);
        Acc::p32Store4a(dst + 8, sro0p_packed_2);

        dst += 12;
        msk += 4;
      BLIT_LOOP_24x4_MAIN_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_24x4_INIT()

      BLIT_LOOP_24x4_SMALL_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load3b(dst0p, dst);
        Acc::p32Load2a(msk0p, msk);
        Acc::p32MulDiv256PBB_SBW_Z210(src0p, sro0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store3b(dst, dst0p);

        dst += 3;
        msk += 2;
      BLIT_LOOP_24x4_SMALL_END(A8_Extra)

      BLIT_LOOP_24x4_MAIN_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t dst1p;
        uint32_t dst2p;

        uint32_t msk0p_4a, msk0p_4b;
        Acc::p32Load8aRaw(msk0p_4a, msk0p_4b, msk);

        Acc::p32Load4a(dst0p, dst + 0);
        Acc::p32Load4a(dst1p, dst + 4);
        Acc::p32Load4a(dst2p, dst + 8);

        Acc::p32RGB24LerpTriadSolid_PBW256_PMAE(
          dst0p, dst1p, dst2p,
          dst0p, dst1p, dst2p,
          sro0p, msk0p_4a, msk0p_4b);

        Acc::p32Store4a(dst + 0, dst0p);
        Acc::p32Store4a(dst + 4, dst1p);
        Acc::p32Store4a(dst + 8, dst2p);

        dst += 12;
        msk += 8;
      BLIT_LOOP_24x4_MAIN_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p_20, msk0p_31;

        Acc::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        Acc::p32Copy(src0p, sro0p);
        if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;
        Acc::p32Load3b(dst0p, dst);

        Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32Lerp256PBB_PBW_20Z1(src0p, sro0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
        Acc::p32Store3b(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 3;
        msk += 4;
      BLIT_LOOP_24x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [RGB24 - CBlit - XRGB32 - Line]
  // ==========================================================================

  // USE: rgb24_cblit_prgb32_full

  // ==========================================================================
  // [RGB24 - CBlit - XRGB32 - Span]
  // ==========================================================================

  // USE: rgb24_cblit_prgb32_span

  // ==========================================================================
  // [RGB24 - VBlit - XRGB32 - Helpers]
  // ==========================================================================

  static FOG_INLINE void _rgb24_vblit_xrgb32_c_opaque(
    uint8_t* dst, const uint8_t* src, int w)
  {
    BLIT_LOOP_24x4_INIT()

    BLIT_LOOP_24x4_SMALL_BEGIN(C_Opaque)
      uint32_t src0p;

      Acc::p32Load4a(src0p, src);
      Acc::p32Store3b(dst, src0p);

      dst += 3;
      src += 4;
    BLIT_LOOP_24x4_SMALL_END(C_Opaque)

    BLIT_LOOP_24x4_MAIN_BEGIN(C_Opaque)
      uint32_t dst0p, dst1p, dst2p;
      uint32_t src0p, src1p, src2p, src3p;

      Acc::p32Load4a(src0p, src +  0);
      Acc::p32Load4a(src1p, src +  4);
      Acc::p32Load4a(src2p, src +  8);
      Acc::p32Load4a(src3p, src + 12);

      Acc::p32RGB24QuadFromXRGB32Quad(dst0p, dst1p, dst2p, src0p, src1p, src2p, src3p);
      Acc::p32Store4a(dst + 0, dst0p);
      Acc::p32Store4a(dst + 4, dst1p);
      Acc::p32Store4a(dst + 8, dst2p);

      dst += 12;
      src += 16;
    BLIT_LOOP_24x4_MAIN_END(C_Opaque)
  }

  static FOG_INLINE void _rgb24_vblit_xrgb32_c_mask(
    uint8_t* dst, const uint8_t* src, const uint32_t& msk0p_a, const uint32_t& msk0p_i, int w)
  {
    BLIT_LOOP_24x4_INIT()

    BLIT_LOOP_24x4_SMALL_BEGIN(C_Mask)
      uint32_t dst0p;
      uint32_t src0p;

      Acc::p32Load3b(dst0p, dst);
      Acc::p32Load4a(src0p, src);
      Acc::p32Lerp256PBB_SBW_10Z2(dst0p, dst0p, msk0p_i, src0p, msk0p_a);
      Acc::p32Store3b(dst, src0p);

      dst += 3;
      src += 4;
    BLIT_LOOP_24x4_SMALL_END(C_Mask)

    BLIT_LOOP_24x4_MAIN_BEGIN(C_Mask)
      uint32_t dst0p, dst1p;
      uint32_t src0p, src1p, src2p, src3p;

      Acc::p32Load4a(dst0p, dst +  0);
      Acc::p32Load4a(dst1p, dst +  4);

      Acc::p32Load4a(src0p, src +  0);
      Acc::p32Load4a(src1p, src +  4);
      Acc::p32Load4a(src2p, src +  8);
      Acc::p32Load4a(src3p, src + 12);

      Acc::p32RGB24QuadFromXRGB32Quad(src0p, src1p, src2p, src0p, src1p, src2p, src3p);
      Acc::p32Lerp255PBW_SBW_2x(dst0p, dst0p, msk0p_i, src0p, msk0p_a,
                                 dst1p, dst1p, msk0p_i, src0p, msk0p_a);

      Acc::p32Store4a(dst + 0, dst0p);
      Acc::p32Store4a(dst + 4, dst1p);

      Acc::p32Load4a(dst0p, dst +  8);
      Acc::p32Lerp255PBW_SBW(dst0p, dst0p, msk0p_i, src2p, msk0p_a);
      Acc::p32Store4a(dst + 8, dst0p);

      dst += 12;
      src += 16;
    BLIT_LOOP_24x4_MAIN_END(C_Mask)
  }

  static FOG_INLINE void _rgb24_vblit_xrgb32_a8_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_24x1_INIT()

    BLIT_LOOP_24x1_BEGIN(A8_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Load1b(msk0p, msk);
      if (msk0p == 0x00) goto _A8_Glyph_Skip;

      Acc::p32Load4a(src0p, src);
      if (msk0p == 0xFF) goto _A8_Glyph_Fill;

      Acc::p32Load3b(dst0p, dst);
      Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);

      Acc::p32Lerp256PBB_SBW_10Z2(src0p, src0p, dst0p, msk0p);

_A8_Glyph_Fill:
      Acc::p32Store3b(dst, src0p);

_A8_Glyph_Skip:
      dst += 3;
      src += 4;
      msk += 1;
    BLIT_LOOP_24x1_END(A8_Glyph)
  }

  static FOG_INLINE void _rgb24_vblit_xrgb32_a8_extra(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_24x1_INIT()

    BLIT_LOOP_24x1_BEGIN(A8_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Load3b(dst0p, dst);
      Acc::p32Load4a(src0p, src);
      Acc::p32Load1b(msk0p, msk);
      Acc::p32Lerp256PBB_SBW_10Z2(src0p, src0p, dst0p, msk0p);
      Acc::p32Store3b(dst, src0p);

      dst += 3;
      src += 4;
      msk += 1;
    BLIT_LOOP_24x1_END(A8_Glyph)
  }

  static FOG_INLINE void _rgb24_vblit_xrgb32_argb32_glyph(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, int w)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(ARGB32_Glyph)
      uint32_t dst0p;
      uint32_t src0p;
      uint32_t msk0p_20, msk0p_31;

      Acc::p32Load4a(msk0p_20, msk);
      if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

      Acc::p32Load4a(src0p, src);
      if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

      Acc::p32Load3b(dst0p, dst);
      Acc::p32UnpackPBW256FromPBB255_2031(msk0p_20, msk0p_31, msk0p_20);
      Acc::p32Lerp256PBB_PBW_2031(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
      Acc::p32Store3b(dst, src0p);

_ARGB32_Glyph_Skip:
      dst += 3;
      src += 4;
      msk += 4;
    BLIT_LOOP_DstFx1_END(ARGB32_Glyph)
  }

  // ==========================================================================
  // [RGB24 - VBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_xrgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _rgb24_vblit_xrgb32_c_opaque(dst, src, w);
  }

  // ==========================================================================
  // [RGB24 - VBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_xrgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(3)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _rgb24_vblit_xrgb32_c_opaque(dst, src, w);
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      uint32_t inv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(inv0p, msk0p);

      _rgb24_vblit_xrgb32_c_mask(dst, src, msk0p, inv0p, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      _rgb24_vblit_xrgb32_a8_glyph(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      _rgb24_vblit_xrgb32_a8_extra(dst, src, msk, w);
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      _rgb24_vblit_xrgb32_argb32_glyph(dst, src, msk, w);
    }

    V_BLIT_SPAN8_END()
  }
};

} // RasterOps_C namespace
} // Fog namespace

#endif // _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITESRC_P_H
