// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITESRCOVER_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITESRCOVER_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/CompositeBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - CompositeSrcOver]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeSrcOver
{
  enum { CHARACTERISTICS = COMPOSITE_SRC_OVER };

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    Face::p32 sro0p;
    Face::p32 sra0p;

    Face::p32Copy(sro0p, src->prgb32.p32);
    Face::p32ExtractPBB3(sra0p, sro0p);
    Face::p32Negate255SBW(sra0p, sra0p);

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      Face::p32 dst0p;

      Face::p32Load4a(dst0p, dst);
      Face::p32MulDiv255PBB_SBW(dst0p, dst0p, sra0p);
      Face::p32Add(dst0p, dst0p, sro0p);
      Face::p32Store4a(dst, dst0p);

      dst += 4;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    Face::p32 sro0p, sro0p_20, sro0p_31;
    Face::p32 inv0p;

    Face::p32Copy(sro0p, src->prgb32.p32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    Face::p32ExtractPBB3(inv0p, sro0p);
    Face::p32Negate255SBW(inv0p, inv0p);

    C_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Any]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_ANY()
    {
      BLIT_LOOP_32x1_INIT()

      Face::p32 src0p;
      Face::p32 sra0p;

      Face::p32Copy(src0p, sro0p);
      Face::p32Copy(sra0p, inv0p);

      if (msk0 != 0x100)
      {
        Face::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0, sro0p_31, msk0);

        Face::p32ExtractPBB3(sra0p, src0p);
        Face::p32Negate255SBW(sra0p, sra0p);
      }

      BLIT_LOOP_32x1_BEGIN(C_Any)
        Face::p32 dst0p;

        Face::p32Load4a(dst0p, dst);
        Face::p32MulDiv256PBB_SBW(dst0p, dst0p, sra0p);
        Face::p32Add(dst0p, dst0p, src0p);
        Face::p32Store4a(dst, dst0p);

        dst += 4;
      BLIT_LOOP_32x1_END(C_Any)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 msk0p;

        Face::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Face::p32Load4a(dst0p, dst);
        if (msk0p != 0xFF) goto _A8_Glyph_Mask;

        Face::p32MulDiv255PBB_SBW(dst0p, dst0p, inv0p);
        Face::p32Add(dst0p, dst0p, sro0p);
        Face::p32Store4a(dst, dst0p);

_A8_Glyph_Skip:
        dst += 4;
        msk += 1;
        continue;

_A8_Glyph_Mask:
        Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Face::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);

        Face::p32ExtractPBB3(msk0p, src0p);
        Face::p32Negate255SBW(msk0p, msk0p);
        Face::p32MulDiv255PBB_SBW(dst0p, dst0p, msk0p);
        Face::p32Add(dst0p, dst0p, src0p);
        Face::p32Store4a(dst, dst0p);

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
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 msk0p;

        Face::p32Load4a(dst0p, dst);
        Face::p32Load2a(msk0p, msk);

        Face::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);
        Face::p32ExtractPBB3(msk0p, src0p);
        Face::p32Negate255SBW(msk0p, msk0p);
        Face::p32MulDiv255PBB_SBW(dst0p, dst0p, msk0p);
        Face::p32Add(dst0p, dst0p, src0p);
        Face::p32Store4a(dst, dst0p);

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
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 sra0p;
        Face::p32 msk0p_20, msk0p_31;

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _VARGBGlyphSkip;

        Face::p32Load4a(dst0p, dst);
        if (msk0p_20 != 0xFFFFFFFF) goto _VARGBGlyphMask;

        Face::p32MulDiv255PBB_SBW(dst0p, dst0p, inv0p);
        Face::p32Add(dst0p, dst0p, sro0p);
        Face::p32Store4a(dst, dst0p);

_VARGBGlyphSkip:
        dst += 4;
        msk += 4;
        continue;

_VARGBGlyphMask:
        Face::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);

        Face::p32MulDiv256PBW_2x_Pack_2031(src0p, sro0p_20, msk0p_20, sro0p_31, msk0p_31);
        Face::p32ExtractPBB3(sra0p, sro0p);

        Face::p32MulDiv256PBW_SBW_2x(msk0p_20, msk0p_20, sra0p, msk0p_31, msk0p_31, sra0p);
        Face::p32Negate255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv255PBB_PBW_2031(dst0p, dst0p, msk0p_20, msk0p_31);
        Face::p32Add(dst0p, dst0p, src0p);
        Face::p32Store4a(dst, dst0p);

        dst += 4;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }



  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  // USE: CSrc::prgb32_cblit_prgb32_full

  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  // USE: CSrc::prgb32_cblit_prgb32_span



  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      Face::p32 dst0p;
      Face::p32 src0p;
      Face::p32 sra0p;

      Face::p32Load4a(src0p, src);
      if (Face::p32PRGB32IsAlpha00(src0p)) goto _C_Opaque_Skip;
      if (Face::p32PRGB32IsAlphaFF(src0p)) goto _C_Opaque_Fill;

      Face::p32Load4a(dst0p, dst);
      Face::p32ExtractPBB3(sra0p, src0p);
      Face::p32Negate255SBW(sra0p, sra0p);
      Face::p32MulDiv255PBB_SBW(dst0p, dst0p, sra0p);
      Face::p32Add(src0p, src0p, dst0p);

_C_Opaque_Fill:
      Face::p32Store4a(dst, src0p);

_C_Opaque_Skip:
      dst += 4;
      src += 4;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

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
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 sra0p;

        Face::p32Load4a(src0p, src);
        if (Face::p32PRGB32IsAlphaFF(src0p)) goto _C_Opaque_Fill;
        if (Face::p32PRGB32IsAlpha00(src0p)) goto _C_Opaque_Skip;

        Face::p32Load4a(dst0p, dst);
        Face::p32ExtractPBB3(sra0p, src0p);
        Face::p32Negate255SBW(sra0p, sra0p);
        Face::p32MulDiv255PBB_SBW(dst0p, dst0p, sra0p);
        Face::p32Add(src0p, src0p, dst0p);

_C_Opaque_Fill:
        Face::p32Store4a(dst, src0p);

_C_Opaque_Skip:
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

      Face::p32 msk0p;
      Face::p32Copy(msk0p, msk0);

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 sra0p;

        Face::p32Load4a(src0p, src);
        if (Face::p32PRGB32IsAlpha00(src0p)) goto _C_Mask_Skip;

        Face::p32Load4a(dst0p, dst);
        Face::p32MulDiv256PBB_SBW(src0p, src0p, msk0p);

        Face::p32ExtractPBB3(sra0p, src0p);
        Face::p32Negate255SBW(sra0p, sra0p);
        Face::p32MulDiv255PBB_SBW(dst0p, dst0p, sra0p);
        Face::p32Add(src0p, src0p, dst0p);
        Face::p32Store4a(dst, src0p);

_C_Mask_Skip:
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
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 sra0p;
        Face::p32 msk0p;

        Face::p32Load4a(src0p, src);
        Face::p32Load1b(msk0p, msk);

        Face::p32MulDiv256PBB_3Z1Z_(sra0p, src0p, msk0p);
        if (Face::p32PRGB32IsAlpha00(sra0p)) goto _A8_Glyph_Skip;
        if (Face::p32PRGB32IsAlphaFF(sra0p)) goto _A8_Glyph_Fill;

        Face::p32Load4a(dst0p, dst);
        Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Face::p32MulDiv256PBB_Z2Z0_(src0p, src0p, msk0p);
        Face::p32Combine(src0p, src0p, sra0p);

        Face::p32RShift(sra0p, sra0p, 16);
        Face::p32Negate255SBW(sra0p, sra0p);
        Face::p32MulDiv255PBB_SBW(dst0p, dst0p, sra0p);
        Face::p32Add(src0p, src0p, dst0p);

_A8_Glyph_Fill:
        Face::p32Store4a(dst, src0p);

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
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 sra0p;
        Face::p32 msk0p;

        Face::p32Load4a(src0p, src);
        Face::p32Load2a(msk0p, msk);
        if (Face::p32PRGB32IsAlpha00(src0p)) goto _VAExtendedSkip;

        Face::p32Load4a(dst0p, dst);
        Face::p32MulDiv256PBB_SBW(src0p, src0p, msk0p);
        Face::p32ExtractPBB3(sra0p, src0p);
        Face::p32Negate255SBW(sra0p, sra0p);
        Face::p32MulDiv255PBB_SBW(dst0p, dst0p, sra0p);
        Face::p32Add(src0p, src0p, dst0p);

        Face::p32Store4a(dst, src0p);

_VAExtendedSkip:
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
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 sra0p;
        Face::p32 msk0p_20, msk0p_31;

        Face::p32Load4a(src0p, src);
        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000 || Face::p32PRGB32IsAlpha00(src0p)) goto _VARGBGlyphSkip;

        Face::p32Load4a(dst0p, dst);
        if (msk0p_20 != 0xFFFFFFFF) goto _VARGBGlyphMask;
        if (Face::p32PRGB32IsAlphaFF(src0p)) goto _VARGBGlyphFill;

        Face::p32ExtractPBB3(sra0p, src0p);
        Face::p32Negate255SBW(sra0p, sra0p);
        Face::p32MulDiv255PBB_SBW(dst0p, dst0p, sra0p);
        Face::p32Add(src0p, src0p, dst0p);

_VARGBGlyphFill:
        Face::p32Store4a(dst, src0p);

_VARGBGlyphSkip:
        dst += 4;
        src += 4;
        msk += 4;
        BLIT_LOOP_32x1_CONTINUE(ARGB32_Glyph)

_VARGBGlyphMask:
        Face::p32ExtractPBB3(sra0p, src0p);
        Face::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBB_PBW_2031(src0p, src0p, msk0p_20, msk0p_31);

        Face::p32MulDiv256PBW_SBW_2x(msk0p_20, msk0p_20, sra0p, msk0p_31, msk0p_31, sra0p);
        Face::p32Negate255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv255PBB_PBW_2031(dst0p, dst0p, msk0p_20, msk0p_31);
        Face::p32Add(src0p, src0p, dst0p);
        Face::p32Store4a(dst, src0p);

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

  // USE: CSrc::prgb32_vblit_xrgb32_full

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  // USE: CSrc::prgb32_vblit_xrgb32_span



  // ==========================================================================
  // [PRGB32 - VBlit - I8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_i8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* pal = reinterpret_cast<const uint32_t*>(closure->palette->data);

    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Src)
      Face::p32 src0p;

      Face::p32Load4a(src0p, pal + src[0]);
      Face::p32Store4a(dst, src0p);

      dst += 4;
      src += 1;
    BLIT_LOOP_32x1_END(C_Src)
  }

  // ==========================================================================
  // [PRGB32 - VBlit - I8 - Span]
  // ==========================================================================

  // NONE:



  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    Face::p32 sro0p;
    Face::p32 sra0p;

    Face::p32Copy(sro0p, src->prgb32.p32);
    Face::p32ExtractPBB3(sra0p, sro0p);
    Face::p32Negate255SBW(sra0p, sra0p);
    Face::p32FillPBB3(sro0p, sro0p);

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      Face::p32 dst0p;

      Face::p32Load4a(dst0p, dst);
      Face::p32MulDiv255PBB_SBW_Z210(dst0p, dst0p, sra0p);
      Face::p32Add(dst0p, dst0p, sro0p);
      Face::p32Store4a(dst, dst0p);

      dst += 4;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    Face::p32 sro0p, sro0p_20, sro0p_31;
    Face::p32 srf0p;
    Face::p32 inv0p;

    Face::p32Copy(sro0p, src->prgb32.p32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    Face::p32ExtractPBB3(inv0p, sro0p);
    Face::p32Negate255SBW(inv0p, inv0p);

    Face::p32Copy(srf0p, sro0p);
    Face::p32FillPBB3(srf0p, srf0p);

    C_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Any]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_ANY()
    {
      BLIT_LOOP_32x1_INIT()

      Face::p32 src0p;
      Face::p32 sra0p;

      Face::p32Copy(src0p, srf0p);
      Face::p32Copy(sra0p, inv0p);

      if (msk0 != 0x100)
      {
        Face::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0, sro0p_31, msk0);

        Face::p32ExtractPBB3(sra0p, src0p);
        Face::p32FillPBB3(src0p, src0p);
        Face::p32Negate255SBW(sra0p, sra0p);
      }

      BLIT_LOOP_32x1_BEGIN(C_Any)
        Face::p32 dst0p;

        Face::p32Load4a(dst0p, dst);
        Face::p32MulDiv255PBB_SBW_Z210(dst0p, dst0p, sra0p);
        Face::p32Add(dst0p, dst0p, src0p);
        Face::p32Store4a(dst, dst0p);

        dst += 4;
      BLIT_LOOP_32x1_END(C_Any)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 msk0p;

        Face::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Skip;

        Face::p32Load4a(dst0p, dst);
        if (msk0p != 0xFF) goto _A8_Glyph_Mask;

_A8_Glyph_Skip:
        Face::p32Load4a(dst0p, dst);
        Face::p32MulDiv255PBB_SBW_Z210(dst0p, dst0p, inv0p);
        Face::p32Add(dst0p, dst0p, srf0p);
        Face::p32Store4a(dst, dst0p);

        dst += 4;
        msk += 1;
        BLIT_LOOP_32x1_CONTINUE(A8_Glyph)

_A8_Glyph_Mask:
        Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Face::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);

        Face::p32ExtractPBB3(msk0p, src0p);
        Face::p32FillPBB3(src0p, src0p);
        Face::p32Negate255SBW(msk0p, msk0p);
        Face::p32MulDiv255PBB_SBW_Z210(dst0p, dst0p, msk0p);
        Face::p32Add(dst0p, dst0p, src0p);
        Face::p32Store4a(dst, dst0p);

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
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 msk0p;

        Face::p32Load4a(dst0p, dst);
        Face::p32Load2a(msk0p, msk);

        Face::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);
        Face::p32ExtractPBB3(msk0p, src0p);
        Face::p32FillPBB3(src0p, src0p);
        Face::p32Negate255SBW(msk0p, msk0p);
        Face::p32MulDiv255PBB_SBW_Z210(dst0p, dst0p, msk0p);
        Face::p32Add(dst0p, dst0p, src0p);
        Face::p32Store4a(dst, dst0p);

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
        Face::p32 dst0p;
        Face::p32 src0p;
        Face::p32 sra0p;
        Face::p32 msk0p_20, msk0p_31;

        Face::p32Load1b(msk0p_20, msk);
        if (msk0p_20 == 0x00000000) goto _VARGBGlyphSkip;

        Face::p32Load4a(dst0p, dst);
        if (msk0p_20 != 0xFFFFFFFF) goto _VARGBGlyphMask;

_VARGBGlyphSkip:
        Face::p32Load4a(dst0p, dst);
        Face::p32MulDiv256PBB_SBW_Z210(dst0p, dst0p, inv0p);
        Face::p32Add(dst0p, dst0p, srf0p);
        Face::p32Store4a(dst, dst0p);

        dst += 4;
        msk += 4;
        BLIT_LOOP_32x1_CONTINUE(ARGB32_Glyph)

_VARGBGlyphMask:
        Face::p32Load4a(dst0p, dst);
        Face::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);
        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_20F1(src0p, sro0p_20, msk0p_20, sro0p_31, msk0p_31);

        Face::p32ExtractPBB3(sra0p, sro0p);
        Face::p32MulDiv256PBW_SBW_2x(msk0p_20, msk0p_20, sra0p, msk0p_31, msk0p_31, sra0p);
        Face::p32Negate255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv255PBB_PBW_20Z1(dst0p, dst0p, msk0p_20, msk0p_31);
        Face::p32Add(dst0p, dst0p, src0p);
        Face::p32Store4a(dst, dst0p);

        dst += 4;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }



  // ==========================================================================
  // [XRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  // USE: CSrc::prgb32_cblit_prgb32_full

  // ==========================================================================
  // [XRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  // USE: CSrc::prgb32_cblit_prgb32_span



  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  //static void FOG_FASTCALL xrgb32_vblit_prgb32_line(
  //  uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  //{
  //}

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  //static void FOG_FASTCALL xrgb32_vblit_prgb32_span(
  //  uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  //{
  //}



  // ==========================================================================
  // [XRGB32 - VBlit - I8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_i8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* pal = reinterpret_cast<const uint32_t*>(closure->palette->data);

    do {
      Face::p32 src0p;

      Face::p32Load4a(src0p, pal + src[0]);
      Face::p32Store4a(dst, src0p);

      dst += 4;
      src += 1;
    } while (--w);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - I8 - Span]
  // ==========================================================================

  // NONE
};

} // RasterOps_C namespace
} // Fog namespace

#endif // _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITESRCOVER_P_H
