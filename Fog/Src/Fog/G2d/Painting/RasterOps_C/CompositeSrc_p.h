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

  static void FOG_FASTCALL prgb32_vblit_xrgb32_line(
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
  // [PRGB32 - VBlit - RGB24 - Helpers]
  // ==========================================================================

  static FOG_INLINE void _prgb32_vblit_rgb24_c_opaque(
    uint8_t* dst, const uint8_t* src, int w)
  {
#if defined(FOG_ARCH_UNALIGNED_ACCESS_32) || FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    // X86/X64 Specific code relying on unaligned read. It seems that this is
    // really unbeatable by the any other code except SSSE3 using PSHUFB.
    uint32_t src0p;

    // We know that each scanline should be aligned to 32-bits so reading one
    // more BYTE we need should never fail, but I'd like to make this code bug
    // free and valgrind-ready, so we process w-1 pixels in loop, and last
    // pixel using standard way.
    FOG_ASSUME(w > 0);

    while (--w)
    {
      Acc::p32Load4u(src0p, src);
#if FOG_BYTE_ORDER == FOG_BIG_ENDIAN
      Acc::p32RShift(src0p, src0p);
#endif // FOG_BIG_ENDIAN
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
#endif // FOG_ARCH_UNALIGNED_ACCESS_32 && FOG_LITTLE_ENDIAN
  }

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_rgb24_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _prgb32_vblit_rgb24_c_opaque(dst, src, w);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_rgb24_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _prgb32_vblit_rgb24_c_opaque(dst, src, w);
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      uint32_t minv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(minv0p, msk0p);

#if defined(FOG_ARCH_UNALIGNED_ACCESS_32) || FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
      // X86/X64 Specific code relying on unaligned read. It seems that this is
      // really unbeatable by the any other code except SSSE3 using PSHUFB.
      uint32_t src0p;
      uint32_t dst0p;

      // We know that each scanline should be aligned to 32-bits so reading one
      // more BYTE we need should never fail, but I'd like to make this code bug
      // free and valgrind-ready, so we process w-1 pixels in loop, and last
      // pixel using standard way.
      FOG_ASSUME(w > 0);

      while (--w)
      {
        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load4u(src0p, src);
        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p, minv0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 3;
      }

      Acc::p32Load4a(dst0p, dst);
      Acc::p32Load3b(src0p, src);
      Acc::p32FillPBB3(src0p, src0p);
      Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p, minv0p);
      Acc::p32Store4a(dst, src0p);
#else
      BLIT_LOOP_32x4_VS_24_INIT()

      BLIT_LOOP_32x4_VS_24_SMALL_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load3b(src0p, src);
        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p, minv0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 3;
      BLIT_LOOP_32x4_VS_24_SMALL_END(C_Mask)

      BLIT_LOOP_32x4_VS_24_MAIN_BEGIN(C_Mask)
        uint32_t dst0p, dst1p, dst2p, dst3p;
        uint32_t src0p, src1p, src2p, src3p;

        Acc::p32FRGB32QuadFromRGB24Quad(
          src0p, src1p, src2p, src3p, ((uint32_t*)src)[0], ((uint32_t*)src)[1], ((uint32_t*)src)[2]);

        Acc::p32Load4a(dst0p, dst + 0);
        Acc::p32Load4a(dst1p, dst + 4);
        Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p, minv0p);
        Acc::p32Lerp256PBB_SBW(src1p, src1p, dst1p, msk0p, minv0p);
        Acc::p32Store4a(dst + 0, src0p);
        Acc::p32Store4a(dst + 4, src1p);

        Acc::p32Load4a(dst2p, dst + 8);
        Acc::p32Load4a(dst3p, dst + 12);
        Acc::p32Lerp256PBB_SBW(src2p, src2p, dst2p, msk0p, minv0p);
        Acc::p32Lerp256PBB_SBW(src3p, src3p, dst3p, msk0p, minv0p);
        Acc::p32Store4a(dst + 8, src2p);
        Acc::p32Store4a(dst + 12, src3p);

        dst += 16;
        src += 12;
      BLIT_LOOP_32x4_VS_24_MAIN_END(C_Mask)
#endif // FOG_ARCH_UNALIGNED_ACCESS_32 && FOG_LITTLE_ENDIAN
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

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8/16 - Helpers]
  // ==========================================================================

  template<uint SrcSize, uint SrcOffset>
  static FOG_INLINE void prgb32_vblit_a8_or_a16_c_opaque(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()
    src += SrcOffset;

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t src0p;

      Acc::p32Load1b(src0p, src);
      Acc::p32ExtendPBBFromSBB(src0p, src0p);
      Acc::p32Store4a(dst, src0p);

      dst += 4;
      src += SrcSize;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  template<uint SrcSize, uint SrcOffset>
  static FOG_INLINE void prgb32_vblit_a8_or_a16_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t src0p;

        Acc::p32Load1b(src0p, src);
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += SrcSize;
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
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;

        Acc::p32Load1b(src0p, src);
        Acc::p32Load4a(dst0p, dst);

        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, minv0p);
        Acc::p32MulDiv256SBW(src0p, src0p, msk0p);
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += SrcSize;
      BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Load1b(src0p, src);
        if (msk0p != 0xFF) goto _A8_Glyph_Mask;

        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

_A8_Glyph_Skip:
        dst += 4;
        src += SrcSize;
        msk += 1;
        BLIT_LOOP_32x1_CONTINUE(A8_Glyph)

_A8_Glyph_Mask:
        Acc::p32Load4a(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);

        Acc::p32MulDiv256SBW(src0p, src0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += SrcSize;
        msk += 1;
      BLIT_LOOP_32x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load1b(src0p, src);
        Acc::p32Load2a(msk0p, msk);

        Acc::p32MulDiv256SBW(src0p, src0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += SrcSize;
        msk += 2;
      BLIT_LOOP_32x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p_20, msk0p_31;

        Acc::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        Acc::p32Load1b(src0p, src);
        if (msk0p_20 != 0xFFFFFFFF) goto _ARGB32_Glyph_Mask;

        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 4;
        src += SrcSize;
        msk += 4;
        BLIT_LOOP_32x1_CONTINUE(ARGB32_Glyph)

_ARGB32_Glyph_Mask:
        Acc::p32Load4a(dst0p, dst);

        Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32MulDiv256PBB_PBW_2031(dst0p, dst0p, msk0p_20, msk0p_31);
        Acc::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32MulDiv256PBW_SBW_2x_Pack_1032(src0p, msk0p_20, src0p, msk0p_31, src0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += SrcSize;
        msk += 4;
        BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    prgb32_vblit_a8_or_a16_c_opaque<1, 0>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    prgb32_vblit_a8_or_a16_span<1, 0>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - I8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_i8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const Argb32* pal = closure->palette->data;
    uint32_t colorKey = closure->colorKey;

    BLIT_LOOP_32x1_INIT()

    if (colorKey <= 0xFF)
    {
      BLIT_LOOP_32x1_BEGIN(C_Opaque_HasKey)
        uint32_t src0p;
        Acc::p32Load1b(src0p, src);

        if (src0p == colorKey)
          goto C_OpaqueHasKey_Zero;

        src0p = pal[src0p];
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 1;
        BLIT_LOOP_32x1_CONTINUE(C_OpaqueHasKey)

C_OpaqueHasKey_Zero:
        Acc::p32Store4a(dst, 0x00000000);
        dst += 4;
        src += 1;
      BLIT_LOOP_32x1_END(C_OpaqueHasKey)
    }
    else
    {
      BLIT_LOOP_32x1_BEGIN(C_OpaqueNoKey)
        uint32_t src0p;

        Acc::p32Load1b(src0p, src);
        src0p = pal[src0p];
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 1;
      BLIT_LOOP_32x1_END(C_OpaqueNoKey)
    }
  }

  // ==========================================================================
  // [PRGB32 - VBlit - I8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_i8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    const Argb32* pal = closure->palette->data;
    uint32_t colorKey = closure->colorKey;

    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(C_Opaque_HasKey)
          uint32_t src0p;
          Acc::p32Load1b(src0p, src);

          if (src0p == colorKey)
            goto C_OpaqueHasKey_Zero;

          src0p = pal[src0p];
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
          BLIT_LOOP_32x1_CONTINUE(C_OpaqueHasKey)

C_OpaqueHasKey_Zero:
          Acc::p32Store4a(dst, 0x00000000);
          dst += 4;
          src += 1;
        BLIT_LOOP_32x1_END(C_OpaqueHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(C_OpaqueNoKey)
          uint32_t src0p;

          Acc::p32Load1b(src0p, src);
          src0p = pal[src0p];
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
        BLIT_LOOP_32x1_END(C_OpaqueNoKey)
      }
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

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(C_Mask_HasKey)
          uint32_t dst0p;
          uint32_t src0p;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Load1b(src0p, src);
          Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, minv0p);

          if (src0p == colorKey)
            goto C_MaskHasKey_Fill;

          src0p = pal[src0p];
          Acc::p32MulDiv256PBB_SBW(src0p, src0p, msk0p);
          Acc::p32Add(dst0p, dst0p, src0p);

C_MaskHasKey_Fill:
          Acc::p32Store4a(dst, dst0p);

          dst += 4;
          src += 1;
        BLIT_LOOP_32x1_END(C_MaskHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(C_MaskNoKey)
          uint32_t dst0p;
          uint32_t src0p;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Load1b(src0p, src);

          src0p = pal[src0p];
          Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, minv0p);
          Acc::p32MulDiv256PBB_SBW(src0p, src0p, msk0p);
          Acc::p32Add(dst0p, dst0p, src0p);
          Acc::p32Store4a(dst, dst0p);

          dst += 4;
          src += 1;
        BLIT_LOOP_32x1_END(C_MaskNoKey)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(A8_GlyphHasKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p;

          Acc::p32Load1b(msk0p, msk);
          if (msk0p == 0x00) goto _A8_GlyphHasKey_Skip;

          Acc::p32Load1b(src0p, src);
          src0p = (src0p == colorKey)
            ? static_cast<uint32_t>(0x00000000)
            : static_cast<uint32_t>(pal[src0p]);
          if (msk0p == 0xFF) goto _A8_GlyphHasKey_Fill;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);

_A8_GlyphHasKey_Fill:
          Acc::p32Store4a(dst, src0p);

_A8_GlyphHasKey_Skip:
          dst += 4;
          src += 1;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_GlyphHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(A8_GlyphNoKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p;

          Acc::p32Load1b(msk0p, msk);
          if (msk0p == 0x00) goto _A8_GlyphNoKey_Skip;

          Acc::p32Load1b(src0p, src);
          src0p = pal[src0p];
          if (msk0p == 0xFF) goto _A8_GlyphHasKey_Fill;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);

_A8_GlyphNoKey_Fill:
          Acc::p32Store4a(dst, src0p);

_A8_GlyphNoKey_Skip:
          dst += 4;
          src += 1;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_GlyphNoKey)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(A8_ExtraHasKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Load1b(src0p, src);
          Acc::p32Load2a(msk0p, msk);

          if (src0p == colorKey)
            goto _A8_ExtraHasKey_Fill;

          src0p = pal[src0p];
          Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
          msk += 2;
          BLIT_LOOP_32x1_CONTINUE(A8_ExtraHasKey)

_A8_ExtraHasKey_Fill:
          Acc::p32Negate256SBW(msk0p, msk0p);
          Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
          Acc::p32Store4a(dst, dst0p);
          
          dst += 4;
          src += 1;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_ExtraHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(A8_ExtraNoKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Load1b(src0p, src);
          Acc::p32Load2a(msk0p, msk);

          src0p = pal[src0p];
          Acc::p32Lerp256PBB_SBW(src0p, src0p, dst0p, msk0p);
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_ExtraNoKey)
      }
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(ARGB32_GlyphHasKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p_20, msk0p_31;

          Acc::p32Load4a(msk0p_20, msk);
          if (msk0p_20 == 0x00000000) goto _ARGB32_GlyphHasKey_Skip;

          Acc::p32Load1b(src0p, src);
          if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_GlyphHasKey_Fill;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
          Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);

          if (src0p == colorKey)
            goto _ARGB32_GlyphHasKey_Zero;

          Acc::p32Lerp256PBB_PBW_2031(src0p, src0p, dst0p, msk0p_20, msk0p_31);
          Acc::p32Store4a(dst, src0p);

_ARGB32_GlyphHasKey_Skip:
          dst += 4;
          src += 1;
          msk += 4;
          BLIT_LOOP_32x1_CONTINUE(ARGB32_GlyphHasKey)

_ARGB32_GlyphHasKey_Fill:
          if (src0p == colorKey) 
            src0p = 0x00000000;
          else
            src0p = pal[src0p];
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
          msk += 4;
          BLIT_LOOP_32x1_CONTINUE(ARGB32_GlyphHasKey)

_ARGB32_GlyphHasKey_Zero:
          Acc::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
          Acc::p32MulDiv256PBB_PBW_2031(dst0p, dst0p, msk0p_20, msk0p_31);
          Acc::p32Store4a(dst, dst0p);
          
          dst += 4;
          src += 1;
          msk += 4;
        BLIT_LOOP_32x1_END(ARGB32_GlyphHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(ARGB32_GlyphNoKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p_20, msk0p_31;

          Acc::p32Load4a(msk0p_20, msk);
          if (msk0p_20 == 0x00000000) goto _ARGB32_GlyphNoKey_Skip;

          Acc::p32Load1b(src0p, src);
          src0p = pal[src0p];
          if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_GlyphNoKey_Fill;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
          Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
          Acc::p32Lerp256PBB_PBW_2031(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_GlyphNoKey_Fill:
          Acc::p32Store4a(dst, src0p);

_ARGB32_GlyphNoKey_Skip:
          dst += 4;
          src += 1;
          msk += 4;
        BLIT_LOOP_32x1_END(ARGB32_GlyphNoKey)
      }
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A16 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a16_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    prgb32_vblit_a8_or_a16_c_opaque<2, PIXEL_A16_BYTE_HI>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A16 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a16_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    prgb32_vblit_a8_or_a16_span<2, PIXEL_A16_BYTE_HI>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Line]
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
  // [XRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  // USE: xrgb32_cblit_prgb32_full

  // ==========================================================================
  // [XRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  // USE: xrgb32_cblit_prgb32_span

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  // USE: prgb32_vblit_xrgb32_line

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  // USE: xrgb32_vblit_xrgb32_span

  // ==========================================================================
  // [XRGB32 - VBlit - XRGB32 - Line]
  // ==========================================================================

  // USE: prgb32_vblit_xrgb32_line

  // ==========================================================================
  // [XRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_xrgb32_span(
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
  // [XRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  // USE: prgb32_vblit_rgb24_line

  // ==========================================================================
  // [XRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_rgb24_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _prgb32_vblit_rgb24_c_opaque(dst, src, w);
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      uint32_t minv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(minv0p, msk0p);

#if defined(FOG_ARCH_UNALIGNED_ACCESS_32) || FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
      // X86/X64 Specific code relying on unaligned read. It seems that this is
      // really unbeatable by the any other code except SSSE3 using PSHUFB.
      uint32_t src0p;
      uint32_t dst0p;

      // We know that each scanline should be aligned to 32-bits so reading one
      // more BYTE we need should never fail, but I'd like to make this code bug
      // free and valgrind-ready, so we process w-1 pixels in loop, and last
      // pixel using standard way.
      FOG_ASSUME(w > 0);

      while (--w)
      {
        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load4u(src0p, src);
        Acc::p32Lerp256PBB_SBW_10F2(src0p, src0p, dst0p, msk0p, minv0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 3;
      }

      Acc::p32Load4a(dst0p, dst);
      Acc::p32Load3b(src0p, src);
      Acc::p32Lerp256PBB_SBW_10F2(src0p, src0p, dst0p, msk0p, minv0p);
      Acc::p32Store4a(dst, src0p);
#else
      BLIT_LOOP_32x4_VS_24_INIT()

      BLIT_LOOP_32x4_VS_24_SMALL_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load3b(src0p, src);
        Acc::p32Lerp256PBB_SBW_10F2(src0p, src0p, dst0p, msk0p, minv0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 3;
      BLIT_LOOP_32x4_VS_24_SMALL_END(C_Mask)

      BLIT_LOOP_32x4_VS_24_MAIN_BEGIN(C_Mask)
        uint32_t dst0p, dst1p, dst2p, dst3p;
        uint32_t src0p, src1p, src2p, src3p;

        Acc::p32ZRGB32QuadFromRGB24Quad(
          src0p, src1p, src2p, src3p, ((uint32_t*)src)[0], ((uint32_t*)src)[1], ((uint32_t*)src)[2]);

        Acc::p32Load4a(dst0p, dst + 0);
        Acc::p32Load4a(dst1p, dst + 4);
        Acc::p32Lerp256PBB_SBW_10F2(src0p, src0p, dst0p, msk0p, minv0p);
        Acc::p32Lerp256PBB_SBW_10F2(src1p, src1p, dst1p, msk0p, minv0p);
        Acc::p32Store4a(dst + 0, src0p);
        Acc::p32Store4a(dst + 4, src1p);

        Acc::p32Load4a(dst2p, dst + 8);
        Acc::p32Load4a(dst3p, dst + 12);
        Acc::p32Lerp256PBB_SBW_10F2(src2p, src2p, dst2p, msk0p, minv0p);
        Acc::p32Lerp256PBB_SBW_10F2(src3p, src3p, dst3p, msk0p, minv0p);
        Acc::p32Store4a(dst + 8, src2p);
        Acc::p32Store4a(dst + 12, src3p);

        dst += 16;
        src += 12;
      BLIT_LOOP_32x4_VS_24_MAIN_END(C_Mask)
#endif // FOG_ARCH_UNALIGNED_ACCESS_32 && FOG_LITTLE_ENDIAN
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

        Acc::p32Load3b(src0p, src);
        if (msk0p == 0xFF) goto _A8_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Lerp256PBB_SBW_10F2(src0p, src0p, dst0p, msk0p);

_A8_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_A8_Glyph_Skip:
        dst += 4;
        src += 3;
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
        Acc::p32Load3b(src0p, src);
        Acc::p32Load2a(msk0p, msk);

        Acc::p32Lerp256PBB_SBW_10F2(dst0p, src0p, dst0p, msk0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 3;
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

        Acc::p32Load3b(src0p, src);
        if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

        Acc::p32Load4a(dst0p, dst);

        Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32Lerp256PBB_PBW_20F1(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
        Acc::p32Store4a(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 4;
        src += 3;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A8/16 - Helpers]
  // ==========================================================================

  template<uint SrcSize, uint SrcOffset>
  static FOG_INLINE void xrgb32_vblit_a8_or_a16_c_opaque(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()
    src += SrcOffset;

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t src0p;

      Acc::p32Load1b(src0p, src);
      Acc::p32ExtendPBBFromSBB(src0p, src0p);
      Acc::p32FillPBB3(src0p, src0p);
      Acc::p32Store4a(dst, src0p);

      dst += 4;
      src += SrcSize;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  template<uint SrcSize, uint SrcOffset>
  static FOG_INLINE void xrgb32_vblit_a8_or_a16_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t src0p;

        Acc::p32Load1b(src0p, src);
        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += SrcSize;
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
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;

        Acc::p32Load1b(src0p, src);
        Acc::p32Load4a(dst0p, dst);

        Acc::p32MulDiv256PBB_SBW_FillPBB3(dst0p, dst0p, minv0p);
        Acc::p32MulDiv256SBW(src0p, src0p, msk0p);
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += SrcSize;
      BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Load1b(src0p, src);
        if (msk0p != 0xFF) goto _A8_Glyph_Mask;

        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

_A8_Glyph_Skip:
        dst += 4;
        src += SrcSize;
        msk += 1;
        BLIT_LOOP_32x1_CONTINUE(A8_Glyph)

_A8_Glyph_Mask:
        Acc::p32Load4a(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);

        Acc::p32MulDiv256SBW(src0p, src0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW_FillPBB3(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += SrcSize;
        msk += 1;
      BLIT_LOOP_32x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load1b(src0p, src);
        Acc::p32Load2a(msk0p, msk);

        Acc::p32MulDiv256SBW(src0p, src0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32ExtendPBBFromSBB_Z210(src0p, src0p);
        Acc::p32MulDiv256PBB_SBW_FillPBB3(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += SrcSize;
        msk += 2;
      BLIT_LOOP_32x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p_20, msk0p_31;

        Acc::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        Acc::p32Load1b(src0p, src);
        if (msk0p_20 != 0xFFFFFFFF) goto _ARGB32_Glyph_Mask;

        Acc::p32ExtendPBBFromSBB(src0p, src0p);
        Acc::p32FillPBB3(src0p, src0p);
        Acc::p32Store4a(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 4;
        src += SrcSize;
        msk += 4;
        BLIT_LOOP_32x1_CONTINUE(ARGB32_Glyph)

_ARGB32_Glyph_Mask:
        Acc::p32Load4a(dst0p, dst);

        Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32MulDiv256PBB_PBW_20F1(dst0p, dst0p, msk0p_20, msk0p_31);
        Acc::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Acc::p32MulDiv256PBW_SBW_2x_Pack_10Z2(src0p, msk0p_20, src0p, msk0p_31, src0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += SrcSize;
        msk += 4;
        BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    xrgb32_vblit_a8_or_a16_c_opaque<1, 0>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    xrgb32_vblit_a8_or_a16_span<1, 0>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - I8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_i8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const Argb32* pal = closure->palette->data;
    uint32_t colorKey = closure->colorKey;

    BLIT_LOOP_32x1_INIT()

    if (colorKey <= 0xFF)
    {
      BLIT_LOOP_32x1_BEGIN(C_Opaque_HasKey)
        uint32_t src0p;
        Acc::p32Load1b(src0p, src);

        if (src0p == colorKey)
          goto C_OpaqueHasKey_Zero;

        src0p = pal[src0p];
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 1;
        BLIT_LOOP_32x1_CONTINUE(C_OpaqueHasKey)

C_OpaqueHasKey_Zero:
        Acc::p32Store4a(dst, 0xFF000000);
        dst += 4;
        src += 1;
      BLIT_LOOP_32x1_END(C_OpaqueHasKey)
    }
    else
    {
      BLIT_LOOP_32x1_BEGIN(C_OpaqueNoKey)
        uint32_t src0p;

        Acc::p32Load1b(src0p, src);
        src0p = pal[src0p];
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 1;
      BLIT_LOOP_32x1_END(C_OpaqueNoKey)
    }
  }

  // ==========================================================================
  // [XRGB32 - VBlit - I8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_i8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    const Argb32* pal = closure->palette->data;
    uint32_t colorKey = closure->colorKey;

    V_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(C_Opaque_HasKey)
          uint32_t src0p;
          Acc::p32Load1b(src0p, src);

          if (src0p == colorKey)
            goto C_OpaqueHasKey_Zero;

          src0p = pal[src0p];
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
          BLIT_LOOP_32x1_CONTINUE(C_OpaqueHasKey)

C_OpaqueHasKey_Zero:
          Acc::p32Store4a(dst, 0xFF000000);
          dst += 4;
          src += 1;
        BLIT_LOOP_32x1_END(C_OpaqueHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(C_OpaqueNoKey)
          uint32_t src0p;

          Acc::p32Load1b(src0p, src);
          src0p = pal[src0p];
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
        BLIT_LOOP_32x1_END(C_OpaqueNoKey)
      }
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

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(C_Mask_HasKey)
          uint32_t dst0p;
          uint32_t src0p;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Load1b(src0p, src);
          Acc::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, minv0p);

          if (src0p == colorKey)
            goto C_MaskHasKey_Fill;

          src0p = pal[src0p];
          Acc::p32MulDiv256PBB_SBW_Z210(src0p, src0p, msk0p);
          Acc::p32Add(dst0p, dst0p, src0p);

C_MaskHasKey_Fill:
          Acc::p32FillPBB3(dst0p, dst0p);
          Acc::p32Store4a(dst, dst0p);

          dst += 4;
          src += 1;
        BLIT_LOOP_32x1_END(C_MaskHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(C_MaskNoKey)
          uint32_t dst0p;
          uint32_t src0p;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Load1b(src0p, src);

          src0p = pal[src0p];
          Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, minv0p);
          Acc::p32MulDiv256PBB_SBW(src0p, src0p, msk0p);
          Acc::p32Add(dst0p, dst0p, src0p);
          Acc::p32Store4a(dst, dst0p);

          dst += 4;
          src += 1;
        BLIT_LOOP_32x1_END(C_MaskNoKey)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(A8_GlyphHasKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p;

          Acc::p32Load1b(msk0p, msk);
          if (msk0p == 0x00) goto _A8_GlyphHasKey_Skip;

          Acc::p32Load1b(src0p, src);
          src0p = (src0p == colorKey)
            ? static_cast<uint32_t>(0xFF000000)
            : static_cast<uint32_t>(pal[src0p]);
          if (msk0p == 0xFF) goto _A8_GlyphHasKey_Fill;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Acc::p32Lerp256PBB_SBW_FillPBB3(src0p, src0p, dst0p, msk0p);

_A8_GlyphHasKey_Fill:
          Acc::p32Store4a(dst, src0p);

_A8_GlyphHasKey_Skip:
          dst += 4;
          src += 1;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_GlyphHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(A8_GlyphNoKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p;

          Acc::p32Load1b(msk0p, msk);
          if (msk0p == 0x00) goto _A8_GlyphNoKey_Skip;

          Acc::p32Load1b(src0p, src);
          src0p = pal[src0p];
          if (msk0p == 0xFF) goto _A8_GlyphHasKey_Fill;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Acc::p32Lerp256PBB_SBW_FillPBB3(src0p, src0p, dst0p, msk0p);

_A8_GlyphNoKey_Fill:
          Acc::p32Store4a(dst, src0p);

_A8_GlyphNoKey_Skip:
          dst += 4;
          src += 1;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_GlyphNoKey)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(A8_ExtraHasKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Load1b(src0p, src);
          Acc::p32Load2a(msk0p, msk);

          if (src0p == colorKey)
            goto _A8_ExtraHasKey_Fill;

          src0p = pal[src0p];
          Acc::p32Lerp256PBB_SBW_FillPBB3(src0p, src0p, dst0p, msk0p);
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
          msk += 2;
          BLIT_LOOP_32x1_CONTINUE(A8_ExtraHasKey)

_A8_ExtraHasKey_Fill:
          Acc::p32Negate256SBW(msk0p, msk0p);
          Acc::p32MulDiv256PBB_SBW_FillPBB3(dst0p, dst0p, msk0p);
          Acc::p32Store4a(dst, dst0p);
          
          dst += 4;
          src += 1;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_ExtraHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(A8_ExtraNoKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32Load1b(src0p, src);
          Acc::p32Load2a(msk0p, msk);

          src0p = pal[src0p];
          Acc::p32Lerp256PBB_SBW_FillPBB3(src0p, src0p, dst0p, msk0p);
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_ExtraNoKey)
      }
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      if (colorKey <= 0xFF)
      {
        BLIT_LOOP_32x1_BEGIN(ARGB32_GlyphHasKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p_20, msk0p_31;

          Acc::p32Load4a(msk0p_20, msk);
          if (msk0p_20 == 0x00000000) goto _ARGB32_GlyphHasKey_Skip;

          Acc::p32Load1b(src0p, src);
          if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_GlyphHasKey_Fill;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
          Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);

          if (src0p == colorKey)
            goto _ARGB32_GlyphHasKey_Zero;

          Acc::p32Lerp256PBB_PBW_20F1(src0p, src0p, dst0p, msk0p_20, msk0p_31);
          Acc::p32Store4a(dst, src0p);

_ARGB32_GlyphHasKey_Skip:
          dst += 4;
          src += 1;
          msk += 4;
          BLIT_LOOP_32x1_CONTINUE(ARGB32_GlyphHasKey)

_ARGB32_GlyphHasKey_Fill:
          if (src0p == colorKey) 
            src0p = 0x00000000;
          else
            src0p = pal[src0p];
          Acc::p32Store4a(dst, src0p);

          dst += 4;
          src += 1;
          msk += 4;
          BLIT_LOOP_32x1_CONTINUE(ARGB32_GlyphHasKey)

_ARGB32_GlyphHasKey_Zero:
          Acc::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
          Acc::p32MulDiv256PBB_PBW_20F1(dst0p, dst0p, msk0p_20, msk0p_31);
          Acc::p32Store4a(dst, dst0p);
          
          dst += 4;
          src += 1;
          msk += 4;
        BLIT_LOOP_32x1_END(ARGB32_GlyphHasKey)
      }
      else
      {
        BLIT_LOOP_32x1_BEGIN(ARGB32_GlyphNoKey)
          uint32_t dst0p;
          uint32_t src0p;
          uint32_t msk0p_20, msk0p_31;

          Acc::p32Load4a(msk0p_20, msk);
          if (msk0p_20 == 0x00000000) goto _ARGB32_GlyphNoKey_Skip;

          Acc::p32Load1b(src0p, src);
          src0p = pal[src0p];
          if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_GlyphNoKey_Fill;

          Acc::p32Load4a(dst0p, dst);
          Acc::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
          Acc::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
          Acc::p32Lerp256PBB_PBW_20F1(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_GlyphNoKey_Fill:
          Acc::p32Store4a(dst, src0p);

_ARGB32_GlyphNoKey_Skip:
          dst += 4;
          src += 1;
          msk += 4;
        BLIT_LOOP_32x1_END(ARGB32_GlyphNoKey)
      }
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A16 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a16_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    xrgb32_vblit_a8_or_a16_c_opaque<2, PIXEL_A16_BYTE_HI>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A16 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a16_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    xrgb32_vblit_a8_or_a16_span<2, PIXEL_A16_BYTE_HI>(dst, span, closure);
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

    C_BLIT_SPAN8_BEGIN(3)

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

      Acc::p32MulDiv256PBB_SBW_Z210(src0p, sro0p, msk0p);
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
        uint32_t dst0p, dst1p, dst2p;
        uint32_t msk0p_4a, msk0p_4b;

        Acc::p32Load4u(msk0p_4a, msk + 0);
        Acc::p32Load4u(msk0p_4b, msk + 4);

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

  // USE: rgb24_cblit_prgb32_line

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
      uint32_t minv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(minv0p, msk0p);

      BLIT_LOOP_24x4_INIT()

      BLIT_LOOP_24x4_SMALL_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;

        Acc::p32Load3b(dst0p, dst);
        Acc::p32Load4a(src0p, src);
        Acc::p32Lerp256PBB_SBW_ZeroPBB3(dst0p, dst0p, src0p, minv0p, msk0p);
        Acc::p32Store3b(dst, dst0p);

        dst += 3;
        src += 4;
      BLIT_LOOP_24x4_SMALL_END(C_Mask)

      BLIT_LOOP_24x4_MAIN_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p, src1p, src2p, src3p;

        Acc::p32Load4a(src0p, src +  0);
        Acc::p32Load4a(src1p, src +  4);
        Acc::p32Load4a(src2p, src +  8);
        Acc::p32Load4a(src3p, src + 12);

        Acc::p32RGB24QuadFromXRGB32Quad(src0p, src1p, src2p, src0p, src1p, src2p, src3p);
        Acc::p32Load4a(dst0p, dst +  0);
        Acc::p32Lerp256PBB_SBW(dst0p, dst0p, src0p, minv0p, msk0p);
        Acc::p32Store4a(dst + 0, dst0p);

        Acc::p32Load4a(dst0p, dst +  4);
        Acc::p32Lerp256PBB_SBW(dst0p, dst0p, src1p, minv0p, msk0p);
        Acc::p32Store4a(dst + 4, dst0p);

        Acc::p32Load4a(dst0p, dst +  8);
        Acc::p32Lerp256PBB_SBW(dst0p, dst0p, src2p, minv0p, msk0p);
        Acc::p32Store4a(dst + 8, dst0p);

        dst += 12;
        src += 16;
      BLIT_LOOP_24x4_MAIN_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
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
        Acc::p32Lerp256PBB_SBW_ZeroPBB3(src0p, src0p, dst0p, msk0p);

_A8_Glyph_Fill:
        Acc::p32Store3b(dst, src0p);

_A8_Glyph_Skip:
        dst += 3;
        src += 4;
        msk += 1;
      BLIT_LOOP_24x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load3b(dst0p, dst);
        Acc::p32Load4a(src0p, src);
        Acc::p32Load2a(msk0p, msk);
        Acc::p32Lerp256PBB_SBW_ZeroPBB3(src0p, src0p, dst0p, msk0p);
        Acc::p32Store3b(dst, src0p);

        dst += 3;
        src += 4;
        msk += 2;
      BLIT_LOOP_24x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
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
        Acc::p32Lerp256PBB_PBW_20Z1(src0p, src0p, dst0p, msk0p_20, msk0p_31);

_ARGB32_Glyph_Fill:
        Acc::p32Store3b(dst, src0p);

_ARGB32_Glyph_Skip:
        dst += 3;
        src += 4;
        msk += 4;
      BLIT_LOOP_DstFx1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [RGB24 - VBlit - A8/A16 - Helpers]
  // ==========================================================================

  template<uint SrcSize, uint SrcOffset>
  static FOG_INLINE void _rgb24_vblit_a8_or_a16_line(
    uint8_t* dst, const uint8_t* src, int w)
  {
    src += SrcOffset;

    BLIT_LOOP_24x1_INIT()

    BLIT_LOOP_24x1_BEGIN(C_Opaque)
      uint32_t src0p;

      Acc::p32Load1b(src0p, src);
#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
      Acc::p32ExtendPBBFromSBB_ZZ10(src0p, src0p);
      Acc::p32Store2u(dst + 0, src0p);
      Acc::p32Store1b(dst + 2, src0p);
#else
      Acc::p32Store1b(dst + 0, src0p);
      Acc::p32Store1b(dst + 1, src0p);
      Acc::p32Store1b(dst + 2, src0p);
#endif // FOG_ARCH_UNALIGNED_ACCESS_16

      dst += 3;
      src += SrcSize;
    BLIT_LOOP_24x1_END(C_Opaque)
  }

  template<uint SrcSize, uint SrcOffset>
  static FOG_INLINE void _rgb24_vblit_a8_or_a16_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(3)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _rgb24_vblit_a8_or_a16_line<SrcSize, SrcOffset>(dst, src, w);
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

      BLIT_LOOP_24x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_24x1_BEGIN(C_Mask)
        uint32_t src0p;

        Acc::p32Load1b(src0p, src);
        Acc::p32Mul(src0p, src0p, msk0p);

        dst[0] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[0]) * minv0p) + src0p) >> 8 );
        dst[1] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[1]) * minv0p) + src0p) >> 8 );
        dst[2] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[2]) * minv0p) + src0p) >> 8 );

        dst += 3;
        src += SrcSize;
      BLIT_LOOP_24x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_24x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_24x1_BEGIN(A8_Glyph)
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Load1b(src0p, src);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Mul(src0p, src0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);

        dst[0] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[0]) * msk0p) + src0p) >> 8 );
        dst[1] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[1]) * msk0p) + src0p) >> 8 );
        dst[2] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[2]) * msk0p) + src0p) >> 8 );

_A8_Glyph_Skip:
        dst += 3;
        src += SrcSize;
        msk += 1;
      BLIT_LOOP_24x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_24x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_24x1_BEGIN(A8_Extra)
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load2a(msk0p, msk);
        Acc::p32Load1b(src0p, src);

        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Mul(src0p, src0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);

        dst[0] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[0]) * msk0p) + src0p) >> 8 );
        dst[1] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[1]) * msk0p) + src0p) >> 8 );
        dst[2] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[2]) * msk0p) + src0p) >> 8 );

_A8_Extra_Skip:
        dst += 3;
        src += SrcSize;
        msk += 2;
      BLIT_LOOP_24x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_24x1_INIT()
      src += SrcOffset;

      BLIT_LOOP_24x1_BEGIN(ARGB32_Glyph)
        uint32_t src0p_20, src0p_X1;
        uint32_t msk0p_20, msk0p_X1;

        Acc::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        Acc::p32Load1b(src0p_20, src);
        if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

        Acc::p32UnpackPBW256FromPBB255_20Z1(msk0p_20, msk0p_X1, msk0p_20);
        Acc::p32MulDiv256SBW(src0p_X1, msk0p_X1, src0p_20);
        Acc::p32MulDiv256PBW_SBW(src0p_20, msk0p_20, src0p_20);
        
        Acc::p32Negate256PBW(msk0p_20, msk0p_20);
        Acc::p32Negate256SBW(msk0p_X1, msk0p_X1);

        dst[PIXEL_RGB24_BYTE_B] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[PIXEL_RGB24_BYTE_B]) * msk0p_20) + src0p_20) >> 8 );
        msk0p_20 >>= 16;
        src0p_20 >>= 16;
        dst[PIXEL_RGB24_BYTE_G] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[PIXEL_RGB24_BYTE_G]) * msk0p_X1) + src0p_X1) >> 8 );
        dst[PIXEL_RGB24_BYTE_R] = static_cast<uint8_t>( ((static_cast<uint32_t>(dst[PIXEL_RGB24_BYTE_R]) * msk0p_20) + src0p_20) >> 8 );

_ARGB32_Glyph_Skip:
        dst += 3;
        src += SrcSize;
        msk += 4;
        BLIT_LOOP_24x1_CONTINUE(ARGB32_Glyph)

_ARGB32_Glyph_Fill:
#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
        Acc::p32ExtendPBBFromSBB_ZZ10(src0p_20, src0p_20);
        Acc::p32Store2u(dst + 0, src0p_20);
        Acc::p32Store1b(dst + 2, src0p_20);
#else
        Acc::p32Store1b(dst + 0, src0p_20);
        Acc::p32Store1b(dst + 1, src0p_20);
        Acc::p32Store1b(dst + 2, src0p_20);
#endif // FOG_ARCH_UNALIGNED_ACCESS_16

        dst += 3;
        src += SrcSize;
        msk += 4;
      BLIT_LOOP_24x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [RGB24 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _rgb24_vblit_a8_or_a16_line<1, 0>(dst, src, w);
  }

  // ==========================================================================
  // [RGB24 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    _rgb24_vblit_a8_or_a16_span<1, 0>(dst, span, closure);
  }

  // ==========================================================================
  // [RGB24 - VBlit - I8 - Helpers]
  // ==========================================================================

  static FOG_INLINE void _rgb24_vblit_i8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const Argb32* pal = closure->palette->data;
    uint32_t colorKey = closure->colorKey;

    if (colorKey <= 0xFF)
    {
      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(C_OpaqueHasKey)
        uint32_t src0p;

        src0p = src[0];
        if (src0p == colorKey)
          goto _C_OpaqueHasKey_Mask;

        src0p = pal[src0p];
        Acc::p32Store3b(dst, src0p);

        dst += 3;
        src += 1;
        BLIT_LOOP_24x1_CONTINUE(C_OpaqueHasKey)

_C_OpaqueHasKey_Mask:
        dst[0] = 0x00;
        dst[1] = 0x00;
        dst[2] = 0x00;

        dst += 3;
        src += 1;

      BLIT_LOOP_24x1_END(C_OpaqueHasKey)
    }
    else
    {
      BLIT_LOOP_24x4_INIT()

      BLIT_LOOP_24x4_SMALL_BEGIN(C_OpaqueNoKey)
        uint32_t src0p;

        src0p = pal[src[0]];
        Acc::p32Store3b(dst, src0p);

        dst += 3;
        src += 1;
      BLIT_LOOP_24x4_SMALL_END(C_OpaqueNoKey)

      BLIT_LOOP_24x4_MAIN_BEGIN(C_OpaqueNoKey)
        uint32_t dst0p, dst1p, dst2p;
        uint32_t src0p, src1p, src2p, src3p;

        src0p = pal[src[0]];
        src1p = pal[src[1]];
        src2p = pal[src[2]];
        src3p = pal[src[3]];

        Acc::p32RGB24QuadFromXRGB32Quad(dst0p, dst1p, dst2p, src0p, src1p, src2p, src3p);
        Acc::p32Store4a(dst + 0, dst0p);
        Acc::p32Store4a(dst + 4, dst1p);
        Acc::p32Store4a(dst + 8, dst2p);

        dst += 12;
        src += 4;
      BLIT_LOOP_24x4_MAIN_END(C_OpaqueNoKey)
    }
  }

  // ==========================================================================
  // [RGB24 - VBlit - I8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_i8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _rgb24_vblit_i8_line(dst, src, w, closure);
  }

  // ==========================================================================
  // [RGB24 - VBlit - I8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_i8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    const Argb32* pal = closure->palette->data;
    uint32_t colorKey = closure->colorKey;

    V_BLIT_SPAN8_BEGIN(3)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _rgb24_vblit_i8_line(dst, src, w, closure);
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

      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(C_Mask)
        uint32_t src0p_20, src0p_X1;

        src0p_20 = src[0];
        src0p_20 = (src0p_20 != colorKey) ? static_cast<uint32_t>(pal[src0p_20]) : 0;

        Acc::p32UnpackPBWFromPBB_20Z1(src0p_20, src0p_X1, src0p_20);
        Acc::p32Mul(src0p_20, src0p_20, msk0p);
        Acc::p32Mul(src0p_X1, src0p_X1, msk0p);

        dst[PIXEL_RGB24_BYTE_B] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_B]) * minv0p + src0p_20) >> 8);
        src0p_20 >>= 16;
        dst[PIXEL_RGB24_BYTE_G] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_G]) * minv0p + src0p_X1) >> 8);
        dst[PIXEL_RGB24_BYTE_R] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_R]) * minv0p + src0p_20) >> 8);

        dst += 3;
        src += 1;
      BLIT_LOOP_24x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(A8_Glyph)
        uint32_t src0p_20, src0p_X1;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        src0p_20 = src[0];
        src0p_20 = (src0p_20 != colorKey) ? static_cast<uint32_t>(pal[src0p_20]) : 0;
        if (msk0p == 0xFF) goto _A8_Glyph_Fill;

        Acc::p32UnpackPBWFromPBB_20Z1(src0p_20, src0p_X1, src0p_20);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Mul(src0p_20, src0p_20, msk0p);
        Acc::p32Mul(src0p_X1, src0p_X1, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);

        dst[PIXEL_RGB24_BYTE_B] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_B]) * msk0p + src0p_20) >> 8);
        src0p_20 >>= 16;
        dst[PIXEL_RGB24_BYTE_G] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_G]) * msk0p + src0p_X1) >> 8);
        dst[PIXEL_RGB24_BYTE_R] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_R]) * msk0p + src0p_20) >> 8);

_A8_Glyph_Skip:
        dst += 3;
        src += 1;
        msk += 1;
        BLIT_LOOP_24x1_CONTINUE(A8_Glyph)

_A8_Glyph_Fill:
        dst[PIXEL_RGB24_BYTE_B] = uint8_t(src0p_20); src0p_20 >>= 8;
        dst[PIXEL_RGB24_BYTE_G] = uint8_t(src0p_20); src0p_20 >>= 8;
        dst[PIXEL_RGB24_BYTE_R] = uint8_t(src0p_20);

        dst += 3;
        src += 1;
        msk += 1;
      BLIT_LOOP_24x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(A8_Extra)
        uint32_t src0p_20, src0p_X1;
        uint32_t msk0p;

        Acc::p32Load2a(msk0p, msk);

        src0p_20 = src[0];
        src0p_20 = (src0p_20 != colorKey) ? static_cast<uint32_t>(pal[src0p_20]) : 0;

        Acc::p32UnpackPBWFromPBB_20Z1(src0p_20, src0p_X1, src0p_20);
        Acc::p32Mul(src0p_20, src0p_20, msk0p);
        Acc::p32Mul(src0p_X1, src0p_X1, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);

        dst[PIXEL_RGB24_BYTE_B] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_B]) * msk0p + src0p_20) >> 8);
        src0p_20 >>= 16;
        dst[PIXEL_RGB24_BYTE_G] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_G]) * msk0p + src0p_X1) >> 8);
        dst[PIXEL_RGB24_BYTE_R] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_R]) * msk0p + src0p_20) >> 8);

        dst += 3;
        src += 1;
        msk += 2;
      BLIT_LOOP_24x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(ARGB32_Glyph)
        uint32_t src0p_20, src0p_X1;
        uint32_t msk0p_20, msk0p_X1;

        Acc::p32Load1b(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _ARGB32_Glyph_Skip;

        src0p_20 = src[0];
        src0p_20 = (src0p_20 != colorKey) ? static_cast<uint32_t>(pal[src0p_20]) : 0;
        if (msk0p_20 == 0xFFFFFFFF) goto _ARGB32_Glyph_Fill;

        Acc::p32UnpackPBWFromPBB_20Z1(src0p_20, src0p_X1, src0p_20);
        Acc::p32UnpackPBW256FromPBB255_20Z1(msk0p_20, msk0p_X1, msk0p_20);

        Acc::p32MulPBW(src0p_20, src0p_20, msk0p_20);
        Acc::p32Mul(src0p_X1, src0p_X1, msk0p_X1);

        Acc::p32Negate256PBW(msk0p_20, msk0p_20);
        Acc::p32Negate256SBW(msk0p_X1, msk0p_X1);

        dst[PIXEL_RGB24_BYTE_B] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_B]) * msk0p_20 + src0p_20) >> 8);
        src0p_20 >>= 16;
        msk0p_20 >>= 16;
        dst[PIXEL_RGB24_BYTE_G] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_G]) * msk0p_X1 + src0p_X1) >> 8);
        dst[PIXEL_RGB24_BYTE_R] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_R]) * msk0p_20 + src0p_20) >> 8);

_ARGB32_Glyph_Skip:
        dst += 3;
        src += 1;
        msk += 4;
        BLIT_LOOP_24x1_CONTINUE(ARGB32_Glyph)

_ARGB32_Glyph_Fill:
        dst[PIXEL_RGB24_BYTE_B] = uint8_t(src0p_20); src0p_20 >>= 8;
        dst[PIXEL_RGB24_BYTE_G] = uint8_t(src0p_20); src0p_20 >>= 8;
        dst[PIXEL_RGB24_BYTE_R] = uint8_t(src0p_20);

        dst += 3;
        src += 1;
        msk += 4;
      BLIT_LOOP_24x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [RGB24 - VBlit - PRGB64/RGB48 - Helpers]
  // ==========================================================================

  template<uint SrcSize, uint SrcR, uint SrcG, uint SrcB>
  static FOG_INLINE void _rgb24_vblit_prgb64_or_rgb48_line(
    uint8_t* dst, const uint8_t* src, int w)
  {
    BLIT_LOOP_24x1_INIT()

    BLIT_LOOP_24x1_BEGIN(C_Opaque)
      dst[PIXEL_RGB24_BYTE_R] = src[SrcR];
      dst[PIXEL_RGB24_BYTE_G] = src[SrcG];
      dst[PIXEL_RGB24_BYTE_B] = src[SrcB];

      dst += 3;
      src += SrcSize;
    BLIT_LOOP_24x1_END(C_Opaque)
  }

  template<uint SrcSize, uint SrcR, uint SrcG, uint SrcB>
  static FOG_INLINE void _rgb24_vblit_prgb64_or_rgb48_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(3)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _rgb24_vblit_prgb64_or_rgb48_line<SrcSize, SrcR, SrcG, SrcB>(dst, src, w);
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

      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(C_Mask)
        dst[PIXEL_RGB24_BYTE_R] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_R]) * minv0p + src[SrcR] * msk0p) >> 8);
        dst[PIXEL_RGB24_BYTE_G] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_G]) * minv0p + src[SrcG] * msk0p) >> 8);
        dst[PIXEL_RGB24_BYTE_B] = uint8_t((uint32_t(dst[PIXEL_RGB24_BYTE_B]) * minv0p + src[SrcB] * msk0p) >> 8);

        dst += 3;
        src += SrcSize;
      BLIT_LOOP_24x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(A8_Glyph)
        uint32_t src0p_r;
        uint32_t src0p_g;
        uint32_t src0p_b;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        src0p_r = src[SrcR];
        src0p_g = src[SrcG];
        src0p_b = src[SrcB];
        if (msk0p == 0xFF) goto _A8_Glyph_Fill;

        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32Mul(src0p_r, src0p_r, msk0p);
        Acc::p32Mul(src0p_g, src0p_g, msk0p);
        Acc::p32Mul(src0p_b, src0p_b, msk0p);

        Acc::p32Negate256SBW(msk0p, msk0p);
        src0p_r += uint32_t(dst[PIXEL_RGB24_BYTE_R]) * msk0p;
        src0p_g += uint32_t(dst[PIXEL_RGB24_BYTE_G]) * msk0p;
        src0p_b += uint32_t(dst[PIXEL_RGB24_BYTE_B]) * msk0p;

        src0p_r >>= 8;
        src0p_g >>= 8;
        src0p_b >>= 8;

_A8_Glyph_Fill:
        dst[PIXEL_RGB24_BYTE_R] = src0p_r;
        dst[PIXEL_RGB24_BYTE_G] = src0p_g;
        dst[PIXEL_RGB24_BYTE_B] = src0p_b;

_A8_Glyph_Skip:
        dst += 3;
        src += SrcSize;
        msk += 1;
      BLIT_LOOP_24x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(A8_Extra)
        uint32_t src0p_r;
        uint32_t src0p_g;
        uint32_t src0p_b;
        uint32_t msk0p;

        Acc::p32Load2a(msk0p, msk);

        src0p_r = src[SrcR];
        src0p_g = src[SrcG];
        src0p_b = src[SrcB];

        Acc::p32Mul(src0p_r, src0p_r, msk0p);
        Acc::p32Mul(src0p_g, src0p_g, msk0p);
        Acc::p32Mul(src0p_b, src0p_b, msk0p);

        Acc::p32Negate256SBW(msk0p, msk0p);

        src0p_r += uint32_t(dst[PIXEL_RGB24_BYTE_R]) * msk0p;
        src0p_g += uint32_t(dst[PIXEL_RGB24_BYTE_G]) * msk0p;
        src0p_b += uint32_t(dst[PIXEL_RGB24_BYTE_B]) * msk0p;

        src0p_r >>= 8;
        src0p_g >>= 8;
        src0p_b >>= 8;

        dst[PIXEL_RGB24_BYTE_R] = src0p_r;
        dst[PIXEL_RGB24_BYTE_G] = src0p_g;
        dst[PIXEL_RGB24_BYTE_B] = src0p_b;

        dst += 3;
        src += SrcSize;
        msk += 2;
      BLIT_LOOP_24x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_24x1_INIT()

      BLIT_LOOP_24x1_BEGIN(ARGB32_Glyph)
        uint32_t src0p_r;
        uint32_t src0p_g;
        uint32_t src0p_b;
        uint32_t msk0p;

        Acc::p32Load4a(msk0p, msk);
        if (msk0p == 0x00000000) goto _ARGB32_Glyph_Skip;

        msk0p &= 0x00FFFFFF;
        src0p_r = src[SrcR];
        src0p_g = src[SrcG];
        src0p_b = src[SrcB];
        if (msk0p == 0x00FFFFFF) goto _ARGB32_Glyph_Fill;

        Acc::p32Mul(src0p_r, src0p_r, (msk0p >> 16)       );
        Acc::p32Mul(src0p_g, src0p_g, (msk0p >>  8) & 0xFF);
        Acc::p32Mul(src0p_b, src0p_b, (msk0p      ) & 0xFF);

        Acc::p32Negate255PBB(msk0p, msk0p);

        src0p_r += uint32_t(dst[PIXEL_RGB24_BYTE_R]) * ((msk0p >> 16)       );
        src0p_g += uint32_t(dst[PIXEL_RGB24_BYTE_G]) * ((msk0p >>  8) & 0xFF);
        src0p_b += uint32_t(dst[PIXEL_RGB24_BYTE_B]) * ((msk0p      ) & 0xFF);

        Acc::p32Div255SBW(src0p_r, src0p_r);
        Acc::p32Div255SBW(src0p_g, src0p_g);
        Acc::p32Div255SBW(src0p_b, src0p_b);

_ARGB32_Glyph_Fill:
        dst[PIXEL_RGB24_BYTE_R] = src0p_r;
        dst[PIXEL_RGB24_BYTE_G] = src0p_g;
        dst[PIXEL_RGB24_BYTE_B] = src0p_b;

_ARGB32_Glyph_Skip:
        dst += 3;
        src += SrcSize;
        msk += 4;
      BLIT_LOOP_24x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [RGB24 - VBlit - PRGB64 - Line]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_prgb64_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _rgb24_vblit_prgb64_or_rgb48_line<8, PIXEL_ARGB64_BYTE_R_HI, PIXEL_ARGB64_BYTE_G_HI, PIXEL_ARGB64_BYTE_B_HI>(dst, src, w);
  }

  // ==========================================================================
  // [RGB24 - VBlit - PRGB64 - Span]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_prgb64_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    _rgb24_vblit_prgb64_or_rgb48_span<8, PIXEL_ARGB64_BYTE_R_HI, PIXEL_ARGB64_BYTE_G_HI, PIXEL_ARGB64_BYTE_B_HI>(dst, span, closure);
  }

  // ==========================================================================
  // [RGB24 - VBlit - RGB48 - Line]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_rgb48_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _rgb24_vblit_prgb64_or_rgb48_line<6, PIXEL_RGB48_BYTE_R_HI, PIXEL_RGB48_BYTE_G_HI, PIXEL_RGB48_BYTE_B_HI>(dst, src, w);
  }

  // ==========================================================================
  // [RGB24 - VBlit - RGB48 - Span]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_rgb48_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    _rgb24_vblit_prgb64_or_rgb48_span<6, PIXEL_RGB48_BYTE_R_HI, PIXEL_RGB48_BYTE_G_HI, PIXEL_RGB48_BYTE_B_HI>(dst, span, closure);
  }

  // ==========================================================================
  // [RGB24 - VBlit - A16 - Line]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_a16_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _rgb24_vblit_a8_or_a16_line<2, PIXEL_A16_BYTE_HI>(dst, src, w);
  }

  // ==========================================================================
  // [RGB24 - VBlit - A16 - Span]
  // ==========================================================================

  static void FOG_FASTCALL rgb24_vblit_a16_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    _rgb24_vblit_a8_or_a16_span<2, PIXEL_A16_BYTE_HI>(dst, span, closure);
  }

  // ==========================================================================
  // [A8 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL a8_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    uint32_t src0p = src->prgb32.a;
    Acc::p32ExtendPBBFromSBB(src0p, src0p);

    BLIT_LOOP_8x8_INIT()

    BLIT_LOOP_8x8_SMALL_BEGIN(C_Opaque)
      Acc::p32Store1b(dst, src0p);

      dst += 1;
    BLIT_LOOP_8x8_SMALL_END(C_Opaque)

    BLIT_LOOP_8x8_MAIN_BEGIN(C_Opaque)
      Acc::p32Store4a(dst + 0, src0p);
      Acc::p32Store4a(dst + 4, src0p);

      dst += 8;
    BLIT_LOOP_8x8_MAIN_END(C_Opaque)
  }

  // ==========================================================================
  // [A8 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL a8_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sra0p = src->prgb32.a;
    uint32_t sro0p;

    Acc::p32ExtendPBBFromSBB(sro0p, sra0p);

    C_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_8x8_INIT()

      BLIT_LOOP_8x8_SMALL_BEGIN(C_Opaque)
        Acc::p32Store1b(dst, sro0p);

        dst += 1;
      BLIT_LOOP_8x8_SMALL_END(C_Opaque)

      BLIT_LOOP_8x8_MAIN_BEGIN(C_Opaque)
        Acc::p32Store4a(dst + 0, sro0p);
        Acc::p32Store4a(dst + 4, sro0p);

        dst += 8;
      BLIT_LOOP_8x8_MAIN_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_8x8_INIT()

      uint32_t src0p;
      uint32_t msk0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32MulDiv256SBW(src0p, sra0p, msk0p);
      Acc::p32ExtendPBBFromSBB(src0p, src0p);
      Acc::p32Negate256SBW(msk0p, msk0p);

      BLIT_LOOP_8x8_SMALL_BEGIN(C_Mask)
        uint32_t dst0p;

        Acc::p32Load1b(dst0p, dst);
        Acc::p32MulDiv256SBW(dst0p, dst0p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Store1b(dst, dst0p);

        dst += 1;
      BLIT_LOOP_8x8_SMALL_END(C_Mask)

      BLIT_LOOP_8x8_MAIN_BEGIN(C_Mask)
        uint32_t dst0p, dst1p;

        Acc::p32Load4a(dst0p, dst);
        Acc::p32Load4a(dst1p, dst);

        Acc::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Acc::p32MulDiv256PBB_SBW(dst1p, dst1p, msk0p);
        Acc::p32Add(dst0p, dst0p, src0p);
        Acc::p32Add(dst1p, dst1p, src0p);

        Acc::p32Store4a(dst + 0, dst0p);
        Acc::p32Store4a(dst + 4, dst1p);

        dst += 8;
      BLIT_LOOP_8x8_MAIN_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_OR_ARGB32_GLYPH()
    {
      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(A8_Glyph)
        uint32_t dst0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Load1b(dst0p, dst);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Acc::p32MulDiv256SBW(dst0p, dst0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32MulDiv256SBW(msk0p, msk0p, sra0p);
        Acc::p32Add(dst0p, dst0p, msk0p);
        Acc::p32Store1b(dst, dst0p);

_A8_Glyph_Skip:
        dst += 1;
        msk += MskSize;
      BLIT_LOOP_8x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(A8_Extra)
        uint32_t dst0p;
        uint32_t msk0p;

        Acc::p32Load2a(msk0p, msk);
        Acc::p32Load1b(dst0p, dst);

        Acc::p32MulDiv256SBW(dst0p, dst0p, msk0p);
        Acc::p32Negate256SBW(msk0p, msk0p);
        Acc::p32MulDiv256SBW(msk0p, msk0p, sra0p);
        Acc::p32Add(dst0p, dst0p, msk0p);
        Acc::p32Store1b(dst, dst0p);

        dst += 1;
        msk += 2;
      BLIT_LOOP_8x1_END(A8_Extra)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [A8 - VBlit - Any - Line]
  // ==========================================================================

  template<uint SrcSize, uint SrcA>
  static FOG_INLINE void _a8_vblit_any_line(
    uint8_t* dst, const uint8_t* src, int w)
  {
    BLIT_LOOP_8x8_INIT()
    src += SrcSize;

    BLIT_LOOP_8x8_SMALL_BEGIN(C_Opaque)
      dst[0] = src[0];

      dst += 1;
      src += SrcSize;
    BLIT_LOOP_8x8_SMALL_END(C_Opaque)

    BLIT_LOOP_8x8_MAIN_BEGIN(C_Opaque)
      uint32_t src0p, src1p;

      if (SrcSize == 1)
      {
        Acc::p32Load4u(src0p, src + 0);
        Acc::p32Load4u(src1p, src + 4);
      }
      else
      {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
        src0p  = src[SrcSize * 3]; src0p <<= 8;
        src1p  = src[SrcSize * 7]; src1p <<= 8;
        src0p |= src[SrcSize * 2]; src0p <<= 8;
        src1p |= src[SrcSize * 6]; src1p <<= 8;
        src0p |= src[SrcSize * 1]; src0p <<= 8;
        src1p |= src[SrcSize * 5]; src1p <<= 8;
        src0p |= src[SrcSize * 0];
        src1p |= src[SrcSize * 4];
#else
        src0p  = src[SrcSize * 0]; src0p <<= 8;
        src1p  = src[SrcSize * 4]; src1p <<= 8;
        src0p |= src[SrcSize * 1]; src0p <<= 8;
        src1p |= src[SrcSize * 5]; src1p <<= 8;
        src0p |= src[SrcSize * 2]; src0p <<= 8;
        src1p |= src[SrcSize * 6]; src1p <<= 8;
        src0p |= src[SrcSize * 3];
        src1p |= src[SrcSize * 7];
#endif // FOG_BYTE_ORDER
      }

      Acc::p32Store4a(dst + 0, src0p);
      Acc::p32Store4a(dst + 4, src1p);

      dst += 8;
      src += 8 * SrcSize;
    BLIT_LOOP_8x8_MAIN_END(C_Opaque)
  }

  // ==========================================================================
  // [A8 - VBlit - Any - Span]
  // ==========================================================================

  template<uint SrcSize, uint SrcA>
  static FOG_INLINE void _a8_vblit_any_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(1)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      _a8_vblit_any_line<SrcSize, SrcA>(dst, src, w);
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

      BLIT_LOOP_8x8_INIT()
      src += SrcSize;

      BLIT_LOOP_8x8_SMALL_BEGIN(C_Mask)
        dst[0] = uint8_t((uint32_t(dst[0]) * minv0p + uint32_t(src[0]) * msk0p) >> 8);

        dst += 1;
        src += SrcSize;
      BLIT_LOOP_8x8_SMALL_END(C_Mask)

      BLIT_LOOP_8x8_MAIN_BEGIN(C_Mask)
        // Little Endian: [3 2 1 0] [7 6 5 4] 
        // Big Endian   : [0 1 2 3] [4 5 6 7]
        uint32_t src0p_20, src0p_31;
        uint32_t src0p_64, src0p_75;

#if defined(FOG_ARCH_UNALIGNED_ACCESS_32)
        if (SrcSize == 1)
        {
          Acc::p32Load4u(src0p_20, src + 0);
          Acc::p32Load4u(src0p_64, src + 4);

          Acc::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
          Acc::p32UnpackPBWFromPBB_2031(src0p_64, src0p_75, src0p_64);
        }
        else
#endif // FOG_ARCH_UNALIGNED_ACCESS_32
        {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
          src0p_20  = src[SrcSize * 1]; src0p_20 <<= 16;
          src0p_31  = src[SrcSize * 0]; src0p_31 <<= 16;
          src0p_64  = src[SrcSize * 5]; src0p_64 <<= 16;
          src0p_75  = src[SrcSize * 4]; src0p_75 <<= 16;

          src0p_20 |= src[SrcSize * 3];
          src0p_31 |= src[SrcSize * 2];
          src0p_64 |= src[SrcSize * 7];
          src0p_75 |= src[SrcSize * 6];
#else
          src0p_20  = src[SrcSize * 2]; src0p_20 <<= 16;
          src0p_31  = src[SrcSize * 3]; src0p_31 <<= 16;
          src0p_64  = src[SrcSize * 6]; src0p_64 <<= 16;
          src0p_75  = src[SrcSize * 7]; src0p_75 <<= 16;

          src0p_20 |= src[SrcSize * 0];
          src0p_31 |= src[SrcSize * 1];
          src0p_64 |= src[SrcSize * 4];
          src0p_75 |= src[SrcSize * 5];
#endif // FOG_BYTE_ORDER
        }

        Acc::p32MulDiv256PBW_2x_Pack_2031(src0p_20, src0p_20, msk0p, src0p_31, msk0p);
        Acc::p32MulDiv256PBW_2x_Pack_2031(src0p_64, src0p_64, msk0p, src0p_75, msk0p);

        Acc::p32Load4a(src0p_31, src + 0);
        Acc::p32Load4a(src0p_75, src + 4);

        Acc::p32MulDiv256PBB_SBW(src0p_31, src0p_31, minv0p);
        Acc::p32MulDiv256PBB_SBW(src0p_75, src0p_75, minv0p);
        Acc::p32Add_2x(src0p_20, src0p_20, src0p_31, src0p_64, src0p_64, src0p_75);

        Acc::p32Store4a(dst + 0, src0p_20);
        Acc::p32Store4a(dst + 4, src0p_64);

        dst += 8;
        src += 8 * SrcSize;
      BLIT_LOOP_8x8_MAIN_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8/ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_OR_ARGB32_GLYPH()
    {
      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(A8_Glyph)
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Load1b(src0p, src);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);

        src0p = uint32_t(src[0]) * msk0p;
        Acc::p32Negate256SBW(msk0p, msk0p);

        src0p += uint32_t(dst[0]) * msk0p;
        Acc::p32Div256SBW(src0p, src0p);

        Acc::p32Store1b(dst, src0p);

_A8_Glyph_Skip:
        dst += 1;
        src += SrcSize;
        msk += MskSize;
      BLIT_LOOP_8x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(A8_Extra)
        uint32_t src0p;
        uint32_t msk0p;

        Acc::p32Load1b(src0p, src);
        Acc::p32Load1b(msk0p, msk);

        src0p = uint32_t(src[0]) * msk0p;
        Acc::p32Negate256SBW(msk0p, msk0p);

        src0p += uint32_t(dst[0]) * msk0p;
        Acc::p32Div256SBW(src0p, src0p);

        Acc::p32Store1b(dst, src0p);

        dst += 1;
        src += SrcSize;
        msk += 2;
      BLIT_LOOP_8x1_END(A8_Extra)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [A8 - VBlit - White - Line]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_white_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    uint32_t src0p = 0xFFFFFFFF;

    BLIT_LOOP_8x8_INIT()

    BLIT_LOOP_8x8_SMALL_BEGIN(C_Opaque)
      Acc::p32Store1b(dst, src0p);

      dst += 1;
    BLIT_LOOP_8x8_SMALL_END(C_Opaque)

    BLIT_LOOP_8x8_MAIN_BEGIN(C_Opaque)
      Acc::p32Store1b(dst + 0, src0p);
      Acc::p32Store1b(dst + 4, src0p);

      dst += 8;
    BLIT_LOOP_8x8_MAIN_END(C_Opaque)
  }

  // ==========================================================================
  // [A8 - VBlit - White - Span]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_white_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t src0p = 0xFFFFFFFF;

    V_BLIT_SPAN8_BEGIN(1)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_8x8_INIT()

      BLIT_LOOP_8x8_SMALL_BEGIN(C_Opaque)
        Acc::p32Store1b(dst, src0p);

        dst += 1;
      BLIT_LOOP_8x8_SMALL_END(C_Opaque)

      BLIT_LOOP_8x8_MAIN_BEGIN(C_Opaque)
        Acc::p32Store1b(dst + 0, src0p);
        Acc::p32Store1b(dst + 4, src0p);

        dst += 8;
      BLIT_LOOP_8x8_MAIN_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      Acc::p32Copy(msk0p, msk0);

      BLIT_LOOP_8x8_INIT()

      BLIT_LOOP_8x8_SMALL_BEGIN(C_Mask)
        dst[0] += (((uint32_t(dst[0])) ^ 0xFF) * msk0p) >> 8;

        dst += 1;
      BLIT_LOOP_8x8_SMALL_END(C_Mask)

      BLIT_LOOP_8x8_MAIN_BEGIN(C_Mask)
        uint32_t dst0p, dst1p;

        Acc::p32Load4a(dst0p, dst + 0);
        Acc::p32Load4a(dst1p, dst + 4);

        Acc::p32Negate255PBB_2x(dst0p, dst0p, dst1p, dst1p);
        Acc::p32MulDiv256PBW_2x(dst0p, dst0p, msk0p, dst1p, dst1p, msk0p);
        
        reinterpret_cast<uint32_t*>(dst + 0)[0] += dst0p;
        reinterpret_cast<uint32_t*>(dst + 4)[0] += dst1p;

        dst += 8;
      BLIT_LOOP_8x8_MAIN_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8/ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_OR_ARGB32_GLYPH()
    {
      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(A8_Glyph)
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        dst[0] += (((uint32_t(dst[0])) ^ 0xFF) * msk0p) >> 8;

_A8_Glyph_Skip:
        dst += 1;
        msk += MskSize;
      BLIT_LOOP_8x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(A8_Extra)
        uint32_t msk0p;

        Acc::p32Load2a(msk0p, msk);
        dst[0] += (((uint32_t(dst[0])) ^ 0xFF) * msk0p) >> 8;

        dst += 1;
        msk += 2;
      BLIT_LOOP_8x1_END(A8_Extra)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [A8 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _a8_vblit_any_line<4, PIXEL_ARGB32_POS_A>(dst, src, w);
  }

  // ==========================================================================
  // [A8 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    _a8_vblit_any_span<4, PIXEL_ARGB32_POS_A>(dst, span, closure);
  }

  // ==========================================================================
  // [A8 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _a8_vblit_any_line<1, 0>(dst, src, w);
  }

  // ==========================================================================
  // [A8 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    _a8_vblit_any_span<1, 0>(dst, span, closure);
  }

  // ==========================================================================
  // [A8 - VBlit - I8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_i8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    uint32_t colorKey = closure->colorKey;

    BLIT_LOOP_8x1_INIT()

    BLIT_LOOP_8x1_BEGIN(C_Opaque)
      uint32_t src0p;
      uint32_t src0i;

      src0i = src[0];
      src0p = 0xFF;

      if (src0i == colorKey)
        src0p = 0x00;
      Acc::p32Store1b(dst, src0p);

      dst += 1;
      src += 1;
    BLIT_LOOP_8x1_END(C_Opaque)
  }

  // ==========================================================================
  // [A8 - VBlit - I8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_i8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t colorKey = closure->colorKey;

    V_BLIT_SPAN8_BEGIN(1)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(C_Opaque)
        uint32_t src0p;
        uint32_t src0i;

        src0i = src[0];
        src0p = 0xFF;

        if (src0i == colorKey)
          src0p = 0x00;
        Acc::p32Store1b(dst, src0p);

        dst += 1;
        src += 1;
      BLIT_LOOP_8x1_END(C_Opaque)
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
      msk0p = (0xFF * msk0p) >> 8;

      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(C_Mask)
        uint32_t src0p;
        uint32_t src0i;

        src0i = src[0];
        src0p = msk0p;

        if (src0i == colorKey)
          src0p = 0x00;
        dst[0] = uint8_t((uint32_t(dst[0]) * minv0p + src0p) >> 8);

        dst += 1;
        src += 1;
      BLIT_LOOP_8x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8/ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_OR_ARGB32_GLYPH()
    {
      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(A8_Glyph)
        uint32_t src0p;
        uint32_t src0i;
        uint32_t msk0p;

        msk0p = msk[0];
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        src0i = src[0];
        src0p = msk0p;

        if (src0i == colorKey)
          src0p = 0x00;

        Acc::p32Negate255SBW(msk0p, msk0p);
        Acc::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        dst[0] = uint8_t((uint32_t(dst[0]) * msk0p + src0p) >> 8);

_A8_Glyph_Skip:
        dst += 1;
        src += 1;
        msk += 1;
      BLIT_LOOP_8x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_8x1_INIT()

      BLIT_LOOP_8x1_BEGIN(A8_Extra)
        uint32_t src0p;
        uint32_t src0i;
        uint32_t msk0p;

        Acc::p32Load2a(msk0p, msk);

        src0i = src[0];
        src0p = (0xFF * msk0p) >> 8;

        if (src0i == colorKey)
          src0p = 0x00;

        Acc::p32Negate256SBW(msk0p, msk0p);
        dst[0] = uint8_t((uint32_t(dst[0]) * msk0p + src0p) >> 8);

        dst += 1;
        src += 1;
        msk += 2;
      BLIT_LOOP_8x1_END(A8_Extra)
    }

    V_BLIT_SPAN8_END()
  }












  // ==========================================================================
  // [A8 - VBlit - PRGB64 - Line]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_prgb64_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _a8_vblit_any_line<8, PIXEL_ARGB64_BYTE_A_HI>(dst, src, w);
  }

  // ==========================================================================
  // [A8 - VBlit - PRGB64 - Span]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_prgb64_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    _a8_vblit_any_span<8, PIXEL_ARGB64_BYTE_A_HI>(dst, span, closure);
  }

  // ==========================================================================
  // [A8 - VBlit - A16 - Line]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_a16_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    _a8_vblit_any_line<2, PIXEL_A16_BYTE_HI>(dst, src, w);
  }

  // ==========================================================================
  // [A8 - VBlit - A16 - Span]
  // ==========================================================================

  static void FOG_FASTCALL a8_vblit_a16_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    _a8_vblit_any_span<2, PIXEL_A16_BYTE_HI>(dst, span, closure);
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
};

} // RasterOps_C namespace
} // Fog namespace

#endif // _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITESRC_P_H
