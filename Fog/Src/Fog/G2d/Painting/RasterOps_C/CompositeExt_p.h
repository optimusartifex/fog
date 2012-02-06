// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITEEXT_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITEEXT_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/CompositeBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - CompositeCondition]
// ============================================================================

template<uint32_t COMBINE_FLAGS>
struct CompositeCondition
{
  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool p32ProcessDst(const uint32_t& c0)
  {
    if (COMBINE_FLAGS & RASTER_COMBINE_NOP_IF_DA_ZERO)
      return !Face::p32PRGB32IsAlpha00(c0);
    else if (COMBINE_FLAGS & RASTER_COMBINE_NOP_IF_DA_FULL)
      return !Face::p32PRGB32IsAlphaFF(c0);
    else
      return true;
  }

  static FOG_INLINE bool p32ProcessSrc(const uint32_t& c0)
  {
    if (COMBINE_FLAGS & RASTER_COMBINE_NOP_IF_SA_ZERO)
      return !Face::p32PRGB32IsAlpha00(c0);
    else if (COMBINE_FLAGS & RASTER_COMBINE_NOP_IF_SA_FULL)
      return !Face::p32PRGB32IsAlphaFF(c0);
    else
      return true;
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool p64ProcessDst(const __p64& c0)
  {
    if (COMBINE_FLAGS & RASTER_COMBINE_NOP_IF_DA_ZERO)
      return !Face::p64PRGB64IsAlpha0000(c0);
    else if (COMBINE_FLAGS & RASTER_COMBINE_NOP_IF_DA_FULL)
      return !Face::p64PRGB64IsAlphaFFFF(c0);
    else
      return true;
  }

  static FOG_INLINE bool p64ProcessSrc(const __p64& c0)
  {
    if (COMBINE_FLAGS & RASTER_COMBINE_NOP_IF_SA_ZERO)
      return !Face::p64PRGB64IsAlpha0000(c0);
    else if (COMBINE_FLAGS & RASTER_COMBINE_NOP_IF_SA_FULL)
      return !Face::p64PRGB64IsAlphaFFFF(c0);
    else
      return true;
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeGeneric]
// ============================================================================

template<typename CompositeOp, uint32_t CombineFlags>
struct CompositeGeneric
{
  typedef CompositeCondition<CombineFlags> Cond;

  enum
  {
    NopIfDaZero    = (CombineFlags & RASTER_COMBINE_NOP_IF_DA_ZERO) != 0,
    NopIfDaFull    = (CombineFlags & RASTER_COMBINE_NOP_IF_DA_FULL) != 0,
    NopIfSaZero    = (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO) != 0,
    NopIfSaFull    = (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL) != 0,
    IsUnbound      = (CombineFlags & RASTER_COMBINE_UNBOUND       ) != 0,
    IsUnboundMskIn = (CombineFlags & RASTER_COMBINE_UNBOUND_MSK_IN) != 0
  };

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    uint32_t sro0p, sro0p_20, sro0p_31;

    Face::p32Copy(sro0p, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;

      Face::p32Load4a(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
        goto _C_Opaque_Skip;

      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

      CompositeOp::prgb32_op_prgb32_2031(
        dst0p_20, dst0p_20, sro0p_20,
        dst0p_31, dst0p_31, sro0p_31);

      Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
      Face::p32Store4a(dst, dst0p_20);

_C_Opaque_Skip:
      dst += 4;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sro0p, sro0p_20, sro0p_31;

    Face::p32Copy(sro0p, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    C_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t dst0p_20, dst0p_31;

        Face::p32Load4a(dst0p_20, dst);
        if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
          goto _C_Opaque_Skip;

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        CompositeOp::prgb32_op_prgb32_2031(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);

        Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
        Face::p32Store4a(dst, dst0p_20);

_C_Opaque_Skip:
        dst += 4;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      uint32_t minv0p;

      Face::p32Copy(msk0p, msk0);
      Face::p32Negate256SBW(minv0p, msk0p);

      if (IsUnbound && IsUnboundMskIn)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t src0p_20, src0p_31;
        Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0, src0p_31, sro0p_31, msk0);

        BLIT_LOOP_32x1_BEGIN(C_Mask_UnboundIn)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;

          Face::p32Load4a(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          CompositeOp::prgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          Face::p32Store4a(dst, dst0p_20);

          dst += 4;
        BLIT_LOOP_32x1_END(C_Mask_UnboundIn)
      }
      else if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;

          Face::p32Load4a(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          CompositeOp::prgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          Face::p32Store4a(dst, dst0p_20);

          dst += 4;
        BLIT_LOOP_32x1_END(C_Mask_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t src0p_20, src0p_31;
        Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0, src0p_31, sro0p_31, msk0);

        BLIT_LOOP_32x1_BEGIN(C_Mask)
          uint32_t dst0p_20, dst0p_31;

          Face::p32Load4a(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
            goto _C_Mask_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          CompositeOp::prgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Store4a(dst, dst0p_20);

_C_Mask_Skip:
          dst += 4;
        BLIT_LOOP_32x1_END(C_Mask)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------
  
    C_BLIT_SPAN8_A8_GLYPH()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Unbound_Skip;

          Face::p32Load4a(dst0p_20, dst);
          if (msk0p != 0xFF)
            goto _A8_Glyph_Unbound_Mask;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          CompositeOp::prgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Store4a(dst, dst0p_20);

_A8_Glyph_Unbound_Skip:
          dst += 4;
          msk += 1;
          continue;

_A8_Glyph_Unbound_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          CompositeOp::prgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          Face::p32Store4a(dst, dst0p_20);

          dst += 4;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t msk0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Skip;

          Face::p32Load4a(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
            goto _A8_Glyph_Skip;

          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0p, src0p_31, sro0p_31, msk0p);

          CompositeOp::prgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Store4a(dst, dst0p_20);

_A8_Glyph_Skip:
          dst += 4;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_EXTRA()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          Face::p32Load2a(msk0p, msk);
          Face::p32Load4a(dst0p_20, dst);

          Face::p32Negate256SBW(minv0p, msk0p);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          CompositeOp::prgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          Face::p32Store4a(dst, dst0p_20);

          dst += 4;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t msk0p;

          Face::p32Load2a(msk0p, msk);
          Face::p32Load4a(dst0p_20, dst);

          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
            goto _A8_Extra_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0p, src0p_31, sro0p_31, msk0p);

          CompositeOp::prgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Store4a(dst, dst0p_20);

_A8_Extra_Skip:
          dst += 4;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra)
      }
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_ARGB32_GLYPH()
    {
        BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p_20, dst0p_31;
        uint32_t dinv0p;
        uint32_t msk0p_20, msk0p_31;

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000)
          goto _ARGB32_Skip;

        Face::p32Load4a(dst0p_20, dst);
        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        CompositeOp::prgb32_op_prgb32_2031(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);

        Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
        Face::p32Store4a(dst, dst0p_20);

_ARGB32_Skip:
        dst += 4;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32Negate255PBB(dst0p_20, dst0p_20);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);

        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        CompositeOp::prgb32_op_prgb32_2031(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);
        Face::p32Store4a(dst, dst0p_20);

        dst += 4;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;
      uint32_t src0p_20, src0p_31;

      Face::p32Load4a(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
        goto _C_Opaque_Skip;

      Face::p32Load4a(src0p_20, dst);

      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
      Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

      CompositeOp::prgb32_op_prgb32_2031(
        dst0p_20, dst0p_20, src0p_20,
        dst0p_31, dst0p_31, src0p_31);

      Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
      Face::p32Store4a(dst, dst0p_20);

_C_Opaque_Skip:
      dst += 4;
      src += 4;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Line]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    uint32_t sro0p_20, sro0p_31;

    Face::p32Copy(sro0p_20, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p_20);

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;

      Face::p32Load4a(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
        goto _C_Opaque_Skip;

      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

      CompositeOp::prgb32_op_prgb32_2031(
        dst0p_20, dst0p_20, sro0p_20,
        dst0p_31, dst0p_31, sro0p_31);

      Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
      Face::p32Store4a(dst, dst0p_20);

_C_Opaque_Skip:
      dst += 4;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sro0p, sro0p_20, sro0p_31;

    Face::p32Copy(sro0p, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    C_BLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t dst0p_20, dst0p_31;

        Face::p32Load4a(dst0p_20, dst);
        if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
          goto _C_Opaque_Skip;

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        CompositeOp::xrgb32_op_prgb32_2031(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);

        Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
        Face::p32Store4a(dst, dst0p_20);

_C_Opaque_Skip:
        dst += 4;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      uint32_t minv0p;

      Face::p32Copy(msk0p, msk0);
      Face::p32Negate256SBW(minv0p, msk0p);

      if (IsUnbound && IsUnboundMskIn)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t src0p_20, src0p_31;
        Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0, src0p_31, sro0p_31, msk0);

        BLIT_LOOP_32x1_BEGIN(C_Mask_UnboundIn)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;

          Face::p32Load4a(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          CompositeOp::xrgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          Face::p32Store4a(dst, dst0p_20);

          dst += 4;
        BLIT_LOOP_32x1_END(C_Mask_UnboundIn)
      }
      else if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;

          Face::p32Load4a(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          CompositeOp::xrgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          Face::p32Store4a(dst, dst0p_20);

          dst += 4;
        BLIT_LOOP_32x1_END(C_Mask_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t src0p_20, src0p_31;
        Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0, src0p_31, sro0p_31, msk0);

        BLIT_LOOP_32x1_BEGIN(C_Mask)
          uint32_t dst0p_20, dst0p_31;

          Face::p32Load4a(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
            goto _C_Mask_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          CompositeOp::xrgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Store4a(dst, dst0p_20);

_C_Mask_Skip:
          dst += 4;
        BLIT_LOOP_32x1_END(C_Mask)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------
  
    C_BLIT_SPAN8_A8_GLYPH()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Unbound_Skip;

          Face::p32Load4a(dst0p_20, dst);
          if (msk0p != 0xFF)
            goto _A8_Glyph_Unbound_Mask;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          CompositeOp::xrgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Store4a(dst, dst0p_20);

_A8_Glyph_Unbound_Skip:
          dst += 4;
          msk += 1;
          continue;

_A8_Glyph_Unbound_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          CompositeOp::xrgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          Face::p32Store4a(dst, dst0p_20);

          dst += 4;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t msk0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Skip;

          Face::p32Load4a(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
            goto _A8_Glyph_Skip;

          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0p, src0p_31, sro0p_31, msk0p);

          CompositeOp::xrgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Store4a(dst, dst0p_20);

_A8_Glyph_Skip:
          dst += 4;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_EXTRA()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          Face::p32Load2a(msk0p, msk);
          Face::p32Load4a(dst0p_20, dst);

          Face::p32Negate256SBW(minv0p, msk0p);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          CompositeOp::xrgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          Face::p32Store4a(dst, dst0p_20);

          dst += 4;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t msk0p;

          Face::p32Load2a(msk0p, msk);
          Face::p32Load4a(dst0p_20, dst);

          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
            goto _A8_Extra_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0p, src0p_31, sro0p_31, msk0p);

          CompositeOp::xrgb32_op_prgb32_2031(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Store4a(dst, dst0p_20);

_A8_Extra_Skip:
          dst += 4;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra)
      }
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_ARGB32_GLYPH()
    {
        BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p_20, dst0p_31;
        uint32_t dinv0p;
        uint32_t msk0p_20, msk0p_31;

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000)
          goto _ARGB32_Skip;

        Face::p32Load4a(dst0p_20, dst);
        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        CompositeOp::xrgb32_op_prgb32_2031(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);

        Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
        Face::p32Store4a(dst, dst0p_20);

_ARGB32_Skip:
        dst += 4;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32Negate255PBB(dst0p_20, dst0p_20);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);

        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        CompositeOp::xrgb32_op_prgb32_2031(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);
        Face::p32Store4a(dst, dst0p_20);

        dst += 4;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;
      uint32_t src0p_20, src0p_31;

      Face::p32Load4a(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDst(dst0p_20))
        goto _C_Opaque_Skip;

      Face::p32Load4a(src0p_20, src);

      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
      Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

      CompositeOp::xrgb32_op_prgb32_2031(
        dst0p_20, dst0p_20, src0p_20,
        dst0p_31, dst0p_31, src0p_31);

      Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
      Face::p32Store4a(dst, dst0p_20);

_C_Opaque_Skip:
      dst += 4;
      src += 4;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [XRGB32 - VBlit - XRGB32 - Line]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [XRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [XRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [XRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  // TODO:

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  // TODO:
};













#if 0
// ============================================================================
// [Fog::RasterEngine::C - Composite - Base Funcs]
// ============================================================================

template<typename OP>
struct CompositeBaseFuncsC32
{
  // --------------------------------------------------------------------------
  // [CompositeBaseFuncsC32 - PRGB32 - CBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    Face::b32_1x2 src0orig, src1orig;
    Face::b32_2x2Unpack0213(src0orig, src1orig, src->prgb);

    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        Face::b32_1x2 dst0, dst1;
        dst0 = READ_32(dst);

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
        {
          if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
            goto cMaskOpaqueSkip;
        }

        Face::b32_2x2Unpack0213(dst0, dst1, dst0);

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;

cMaskOpaqueSkip:
        dst += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
        {
          Face::b32_1x2 src0, src1;
          Face::b32_2x2MulDiv255U(src0, src0orig, src1, src1orig, msk0);
          msk0 = 255 - msk0;

          do {
            Face::b32_1x2 dst0, dst1;
            Face::b32_1x2 dst0inv, dst1inv;

            dst0 = READ_32(dst);

            Face::b32_2x2Unpack0213(dst0, dst1, dst0);
            Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0);
            Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            Face::b32_2x2Pack0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;

            dst += 4;
          } while (--w);
        }
        else
        {
          do {
            Face::b32_1x2 dst0, dst1;
            Face::b32_1x2 dst0inv, dst1inv;

            dst0 = READ_32(dst);

            Face::b32_2x2Unpack0213(dst0, dst1, dst0);
            Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
            Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

            Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
            Face::b32_2x2Pack0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;

            dst += 4;
          } while (--w);
        }
      }
      else
      {
        Face::b32_1x2 src0, src1;
        Face::b32_2x2MulDiv255U(src0, src0orig, src1, src1orig, msk0);

        do {
          Face::b32_1x2 dst0, dst1;

          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
              goto cMaskAlphaBoundSkip;
          }

          Face::b32_2x2Unpack0213(dst0, dst1, dst0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;

cMaskAlphaBoundSkip:
          dst += 4;
        } while (--w);
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        do {
          Face::b32_1x2 dst0, dst1;
          Face::b32_1x2 dst0inv, dst1inv;
          uint32_t msk0;

          msk0 = READ_8(msk);
          dst0 = READ_32(dst);

          Face::b32_2x2Unpack0213(dst0, dst1, dst0);
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;

          dst += 4;
          msk += 1;
        } while (--w);
      }
      else
      {
        do {
          Face::b32_1x2 dst0, dst1;
          Face::b32_1x2 src0, src1;
          uint32_t msk0;

          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
              goto vMaskAlphaDenseBoundSkip;
          }

          msk0 = READ_8(msk);

          Face::b32_2x2Unpack0213(dst0, dst1, dst0);
          Face::b32_2x2MulDiv255U(src0, src0orig, src1, src1orig, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;

vMaskAlphaDenseBoundSkip:
          dst += 4;
          msk += 1;
        } while (--w);
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        for (;;)
        {
          Face::b32_1x2 dst0, dst1;
          Face::b32_1x2 dst0inv, dst1inv;
          uint32_t msk0;

          msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparseUnBoundSkip;

          dst0 = READ_32(dst);
          if (msk0 == 0xFF) goto vMaskAlphaSparseUnBoundNoMask;

          Face::b32_2x2Unpack0213(dst0, dst1, dst0);
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;

vMaskAlphaSparseUnBoundSkip:
          dst += 4;
          msk += 1;

          if (--w) continue;
          break;

vMaskAlphaSparseUnBoundNoMask:
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;

          dst += 4;
          msk += 1;

          if (--w) continue;
          break;
        }
      }
      else
      {
        for (;;)
        {
          Face::b32_1x2 dst0, dst1;
          Face::b32_1x2 src0, src1;
          uint32_t msk0;

          msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparseBoundSkip;
          if (msk0 == 0xFF) goto vMaskAlphaSparseBoundNoMask;

          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
              goto vMaskAlphaSparseBoundSkip;
          }

          Face::b32_2x2Unpack0213(dst0, dst1, dst0);
          Face::b32_2x2MulDiv255U(src0, src0orig, src1, src1orig, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;

vMaskAlphaSparseBoundSkip:
          dst += 4;
          msk += 1;

          if (--w) continue;
          break;

vMaskAlphaSparseBoundNoMask:
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;

          dst += 4;
          msk += 1;

          if (--w) continue;
          break;
        }
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Dense] -------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Sparse] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [End] ------------------------------------------------------------------
    C_BLIT_SPAN8_END()
    // ------------------------------------------------------------------------
  }

  // --------------------------------------------------------------------------
  // [CompositeBaseFuncsC32 - PRGB32 - VBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_prgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      Face::b32_1x2 src0, src1;
      Face::b32_1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
      {
        dst0 = READ_32(dst);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
        {
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2Unpack0213(dst0, dst1, dst0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
      {
        src0 = READ_32(src);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
        {
          Face::b32_2x2Unpack0213(src0, src1, src0);
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else
      {
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_vblit_prgb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
        {
          dst0 = READ_32(dst);
          if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
          {
            Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
            Face::b32_2x2Unpack0213(dst0, dst1, dst0);

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            Face::b32_2x2Pack0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }
        else if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
        {
          src0 = READ_32(src);
          if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
          {
            Face::b32_2x2Unpack0213(src0, src1, src0);
            Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

            OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            Face::b32_2x2Pack0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      uint32_t msk0inv;
      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = Face::b32_1x1Negate255(msk0);

      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;
        uint32_t msk0 = READ_8(msk);

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      for (;;)
      {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        dst0 = READ_32(dst);
        src0 = READ_32(src);

        Face::b32_2x2Unpack0213(dst0, dst1, dst0);
        Face::b32_2x2Unpack0213(src0, src1, src0);

        if (msk0 == 0xFF) goto vMaskAlphaSparseNoMask;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

vMaskAlphaSparseSkip:
        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;

vMaskAlphaSparseNoMask:
        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Dense] -------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Sparse] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [End] ------------------------------------------------------------------
    V_BLIT_SPAN8_END()
    // ------------------------------------------------------------------------
  }

  // --------------------------------------------------------------------------
  // [CompositeBaseFuncsC32 - PRGB32 - VBlit - ARGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      Face::b32_1x2 src0, src1;
      Face::b32_1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
      {
        dst0 = READ_32(dst);
        if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
          goto cMaskOpaqueSkip;

        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
        Face::b32_2x2Unpack0213(dst0, dst1, dst0);

        // Premultiply only if source colors are used.
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);
        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }
      else if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
      {
        src0 = READ_32(src);
        if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
          goto cMaskOpaqueSkip;

        Face::b32_2x2Unpack0213(src0, src1, src0);
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

        // Premultiply only if source colors are used.
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);
        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }
      else
      {
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

        // Premultiply only if source colors are used.
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);
        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

cMaskOpaqueSkip:
      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_vblit_argb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
        {
          dst0 = READ_32(dst);
          if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
            goto cMaskOpaqueSkip;

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2Unpack0213(dst0, dst1, dst0);

          // Premultiply only if source colors are used.
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);
          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
        else if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
        {
          src0 = READ_32(src);
          if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
            goto cMaskOpaqueSkip;

          Face::b32_2x2Unpack0213(src0, src1, src0);
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

          // Premultiply only if source colors are used.
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);
          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          // Premultiply only if source colors are used.
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);
          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

cMaskOpaqueSkip:
        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      uint32_t msk0inv;
      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = Face::b32_1x1Negate255(msk0);

      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;
        uint32_t msk0 = READ_8(msk);

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      for (;;)
      {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        dst0 = READ_32(dst);
        src0 = READ_32(src);

        Face::b32_2x2Unpack0213(dst0, dst1, dst0);
        Face::b32_2x2Unpack0213(src0, src1, src0);

        if (msk0 == 0xFF) goto vMaskAlphaSparseNoMask;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);
          Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

vMaskAlphaSparseSkip:
        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;

vMaskAlphaSparseNoMask:
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);
        OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Dense] -------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Sparse] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [End] ------------------------------------------------------------------
    V_BLIT_SPAN8_END()
    // ------------------------------------------------------------------------
  }

  // --------------------------------------------------------------------------
  // [CompositeBaseFuncsC32 - PRGB32 - VBlit - XRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_xrgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      Face::b32_1x2 src0, src1;
      Face::b32_1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
      {
        dst0 = READ_32(dst);
        if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
          goto cMaskOpaqueSkip;

        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
        Face::b32_2x2Unpack0213(dst0, dst1, dst0);

        OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }
      else
      {
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

        OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

cMaskOpaqueSkip:
      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_vblit_xrgb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
        {
          dst0 = READ_32(dst);
          if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
            goto cMaskOpaqueSkip;

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2Unpack0213(dst0, dst1, dst0);

          OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

cMaskOpaqueSkip:
        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      uint32_t msk0inv;
      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = Face::b32_1x1Negate255(msk0);

      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);

          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;
        uint32_t msk0 = READ_8(msk);

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);

          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      for (;;)
      {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaDenseSkip;

        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);
        if (msk0 == 0xFF) goto vMaskAlphaDenseNoMask;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::prgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

vMaskAlphaDenseSkip:
        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;

vMaskAlphaDenseNoMask:
        OP::prgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Dense] -------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Sparse] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [End] ------------------------------------------------------------------
    V_BLIT_SPAN8_END()
    // ------------------------------------------------------------------------
  }

  // --------------------------------------------------------------------------
  // [CompositeBaseFuncsC - XRGB32 - CBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    Face::b32_1x2 src0, src1;
    Face::b32_2x2Unpack0213(src0, src1, src->prgb);

    do {
      Face::b32_1x2 dst0, dst1;
      Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

      OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

      Face::b32_2x2Pack0213(dst0, dst0, dst1);
      ((uint32_t*)dst)[0] = dst0;

      dst += 4;
    } while (--w);
  }

  static void FOG_FASTCALL xrgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    Face::b32_1x2 src0orig, src1orig;
    Face::b32_2x2Unpack0213(src0orig, src1orig, src->prgb);

    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        Face::b32_1x2 dst0, dst1;
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
        {
          Face::b32_1x2 src0, src1;
          Face::b32_2x2MulDiv255U(src0, src0orig, src1, src1orig, msk0);
          msk0 = 255 - msk0;

          do {
            Face::b32_1x2 dst0, dst1;
            Face::b32_1x2 dst0inv, dst1inv;

            dst0 = READ_32(dst);

            Face::b32_2x2Unpack0213(dst0, dst1, dst0);
            Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0);
            Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            Face::b32_2x2Pack0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;

            dst += 4;
          } while (--w);
        }
        else
        {
          do {
            Face::b32_1x2 dst0, dst1;
            Face::b32_1x2 dst0inv, dst1inv;

            dst0 = READ_32(dst);

            Face::b32_2x2Unpack0213(dst0, dst1, dst0);
            Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
            Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

            Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
            Face::b32_2x2Pack0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0 + dst0inv;

            dst += 4;
          } while (--w);
        }
      }
      else
      {
        Face::b32_1x2 src0, src1;
        Face::b32_2x2MulDiv255U(src0, src0orig, src1, src1orig, msk0);

        do {
          Face::b32_1x2 dst0, dst1;

          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
            {
              Face::b32_2x2Unpack0213(dst0, dst1, dst0);

              OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

              Face::b32_2x2Pack0213(dst0, dst0, dst1);
              ((uint32_t*)dst)[0] = dst0;
            }
          }
          else
          {
            Face::b32_2x2Unpack0213(dst0, dst1, dst0);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            Face::b32_2x2Pack0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }

          dst += 4;
        } while (--w);
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        do {
          Face::b32_1x2 dst0, dst1;
          Face::b32_1x2 dst0inv, dst1inv;
          uint32_t msk0;

          msk0 = READ_8(msk);
          dst0 = READ_32(dst);

          Face::b32_2x2Unpack0213(dst0, dst1, dst0);
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;

          dst += 4;
          msk += 1;
        } while (--w);
      }
      else
      {
        do {
          Face::b32_1x2 dst0, dst1;
          Face::b32_1x2 src0, src1;
          uint32_t msk0;

          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
              goto vMaskAlphaDenseBoundSkip;
          }

          msk0 = READ_8(msk);

          Face::b32_2x2Unpack0213(dst0, dst1, dst0);
          Face::b32_2x2MulDiv255U(src0, src0orig, src1, src1orig, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;

vMaskAlphaDenseBoundSkip:
          dst += 4;
          msk += 1;
        } while (--w);
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)
      {
        for (;;)
        {
          Face::b32_1x2 dst0, dst1;
          Face::b32_1x2 dst0inv, dst1inv;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparseUnBoundSkip;

          dst0 = READ_32(dst);
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          if (msk0 == 0xFF) goto vMaskAlphaSparseUnBoundNoMask;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, 255 - msk0);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;

vMaskAlphaSparseUnBoundSkip:
          dst += 4;
          msk += 1;

          if (--w) continue;
          break;

vMaskAlphaSparseUnBoundNoMask:
          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;

          dst += 4;
          msk += 1;

          if (--w) continue;
          break;
        }
      }
      else
      {
        for (;;)
        {
          Face::b32_1x2 dst0, dst1;
          Face::b32_1x2 src0, src1;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparseBoundSkip;

          dst0 = READ_32(dst);

          if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_DST_A_ZERO | OPERATOR_CHAR_NOP_IF_DST_A_FULL))
          {
            if (!CompositeHelpersC32<OP::CHARACTERISTICS>::processDstPixel(dst0))
              goto vMaskAlphaSparseBoundSkip;
          }

          Face::b32_2x2Unpack0213(dst0, dst1, dst0);
          if (msk0 == 0xFF) goto vMaskAlphaSparseBoundNoMask;

          Face::b32_2x2MulDiv255U(src0, src0orig, src1, src1orig, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;

vMaskAlphaSparseBoundSkip:
          dst += 4;
          msk += 1;

          if (--w) continue;
          break;

vMaskAlphaSparseBoundNoMask:
          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0orig, dst1, dst1, src1orig);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;

          dst += 4;
          msk += 1;

          if (--w) continue;
          break;
        }
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Dense] -------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Sparse] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [End] ------------------------------------------------------------------
    C_BLIT_SPAN8_END()
    // ------------------------------------------------------------------------
  }

  // --------------------------------------------------------------------------
  // [CompositeBaseFuncsC - XRGB32 - VBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_prgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      Face::b32_1x2 src0, src1;
      Face::b32_1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
      {
        src0 = READ_32(src);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
        {
          Face::b32_2x2Unpack0213(src0, src1, src0);
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else
      {
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL xrgb32_vblit_prgb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
        {
          src0 = READ_32(src);
          if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
          {
            Face::b32_2x2Unpack0213(src0, src1, src0);
            Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            Face::b32_2x2Pack0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      uint32_t msk0inv;
      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = Face::b32_1x1Negate255(msk0);

      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;
        uint32_t msk0 = READ_8(msk);

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      for (;;)
      {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
        if (msk0 == 0xFF) goto vMaskAlphaSparseNoMask;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

vMaskAlphaSparseSkip:
        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;

vMaskAlphaSparseNoMask:
        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Dense] -------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Sparse] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [End] ------------------------------------------------------------------
    V_BLIT_SPAN8_END()
    // ------------------------------------------------------------------------
  }

  // --------------------------------------------------------------------------
  // [CompositeBaseFuncsC - XRGB32 - VBlit - ARGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      Face::b32_1x2 src0, src1;
      Face::b32_1x2 dst0, dst1;

      if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
      {
        src0 = READ_32(src);
        if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
        {
          Face::b32_2x2Unpack0213(src0, src1, src0);
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

          // Premultiply only if source colors are used.
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }
      }
      else
      {
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

        // Premultiply only if source colors are used.
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;
      }

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL xrgb32_vblit_argb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if (OP::CHARACTERISTICS & (OPERATOR_CHAR_NOP_IF_SRC_A_ZERO | OPERATOR_CHAR_NOP_IF_SRC_A_FULL))
        {
          src0 = READ_32(src);
          if (CompositeHelpersC32<OP::CHARACTERISTICS>::processSrcPixel(src0))
          {
            Face::b32_2x2Unpack0213(src0, src1, src0);
            Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));

            // Premultiply only if source colors are used.
            if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

            OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

            Face::b32_2x2Pack0213(dst0, dst0, dst1);
            ((uint32_t*)dst)[0] = dst0;
          }
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          // Premultiply only if source colors are used.
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_SRC_C_USED) Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      uint32_t msk0inv;
      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = Face::b32_1x1Negate255(msk0);

      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;
        uint32_t msk0 = READ_8(msk);

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      for (;;)
      {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));
        if (msk0 == 0xFF) goto vMaskAlphaSparseSkip;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);
          Face::b32_2x2PremultiplyA(src0, src0, src1, src1);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2PremultiplyU(src0, src0, src1, src1, Face::b32_1x1MulDiv255(Face::b32_1x2GetB1(src1), msk0));

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

vMaskAlphaSparseSkip:
        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;

vMaskAlphaSparseNoMask:
        Face::b32_2x2PremultiplyA(src0, src0, src1, src1);
        OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;

        if (--w) continue;
        break;
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Dense] -------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Sparse] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [End] ------------------------------------------------------------------
    V_BLIT_SPAN8_END()
    // ------------------------------------------------------------------------
  }

  // --------------------------------------------------------------------------
  // [CompositeBaseFuncsC - XRGB32 - VBlit - XRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_xrgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      Face::b32_1x2 src0, src1;
      Face::b32_1x2 dst0, dst1;

      Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
      Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

      OP::xrgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

      Face::b32_2x2Pack0213(dst0, dst0, dst1);
      ((uint32_t*)dst)[0] = dst0;

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL xrgb32_vblit_xrgb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

        OP::xrgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      uint32_t msk0inv;
      if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND)) msk0inv = Face::b32_1x1Negate255(msk0);

      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, msk0inv);
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::xrgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);

          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;
        uint32_t msk0 = READ_8(msk);

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          Face::b32_2x2Unpack0213(src0, src1, READ_32(src));

          OP::xrgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
          Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);

          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      for (;;)
      {
        Face::b32_1x2 src0, src1;
        Face::b32_1x2 dst0, dst1;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        Face::b32_2x2Unpack0213(src0, src1, READ_32(src) | 0xFF000000);
        if (msk0 == 0xFF) goto vMaskAlphaSparseNoMask;

        if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND) && OP::CHARACTERISTICS & (OPERATOR_CHAR_UNBOUND_MSK_IN))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);
          Face::b32_2x2MulDiv255U(src0, src0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else if ((OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
        {
          Face::b32_1x2 dst0inv, dst1inv;

          Face::b32_2x2MulDiv255U(dst0inv, dst0, dst1inv, dst1, Face::b32_1x1Negate255(msk0));
          Face::b32_2x2Pack0213(dst0inv, dst0inv, dst1inv);

          OP::xrgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);
          Face::b32_2x2MulDiv255U(dst0, dst0, dst1, dst1, msk0);
          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0 + dst0inv;
        }
        else
        {
          Face::b32_2x2MulDiv255U(src0, src0, msk0, src1, src1, msk0);

          OP::xrgb32_op_prgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

          Face::b32_2x2Pack0213(dst0, dst0, dst1);
          ((uint32_t*)dst)[0] = dst0;
        }

vMaskAlphaSparseSkip:
        dst += 4;
        src += 4;
        msk += 1;

        if (--w) continue;
        break;

vMaskAlphaSparseNoMask:
        dst += 4;
        src += 4;
        msk += 1;

        OP::xrgb32_op_xrgb32_32b(dst0, dst0, src0, dst1, dst1, src1);

        Face::b32_2x2Pack0213(dst0, dst0, dst1);
        ((uint32_t*)dst)[0] = dst0;

        if (--w) continue;
        break;
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Dense] -------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Sparse] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [End] ------------------------------------------------------------------
    V_BLIT_SPAN8_END()
    // ------------------------------------------------------------------------
  }
};
#endif


















// ============================================================================
// [Fog::RasterOps_C - CompositeAdd]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeAdd : public CompositeGeneric<CompositeAdd, RASTER_COMBINE_OP_ADD>
{
  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
  }

  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    Face::p32FillPBW1(dst0p_20, dst0p_20);
    Face::p32FillPBW1(dst0p_31, dst0p_31);
  }

  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
  }
};

} // Render namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITEEXT_P_H
