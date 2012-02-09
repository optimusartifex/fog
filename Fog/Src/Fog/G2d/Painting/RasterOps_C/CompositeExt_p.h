// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITEEXT_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITEEXT_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/CompositeBase_p.h>
#include <Fog/G2d/Painting/RasterOps_C/CompositeClear_p.h>
#include <Fog/G2d/Painting/RasterOps_C/CompositeSrc_p.h>
#include <Fog/G2d/Painting/RasterOps_C/CompositeSrcOver_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - CompositeCondition]
// ============================================================================

template<uint32_t CombineFlags>
struct CompositeExtCondition
{
  // ==========================================================================
  // [Pixel32]
  // ==========================================================================

  static FOG_INLINE bool p32ProcessDstPixel32(const uint32_t& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_DA_ZERO)
      return !Face::p32PRGB32IsAlpha00(c0);
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_DA_FULL)
      return !Face::p32PRGB32IsAlphaFF(c0);
    else
      return true;
  }

  static FOG_INLINE bool p32ProcessSrcPixel32(const uint32_t& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO)
      return !Face::p32PRGB32IsAlpha00(c0);
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL)
      return !Face::p32PRGB32IsAlphaFF(c0);
    else
      return true;
  }

  // ==========================================================================
  // [PixelA8]
  // ==========================================================================

  static FOG_INLINE bool p32ProcessSrcPixelA8(const uint32_t& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO)
      return c0 == 0x00;
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL)
      return c0 == 0xFF;
    else
      return true;
  }

  // ==========================================================================
  // [Pixel64]
  // ==========================================================================

  static FOG_INLINE bool p64ProcessDstPixel64(const __p64& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_DA_ZERO)
      return !Face::p64PRGB64IsAlpha0000(c0);
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_DA_FULL)
      return !Face::p64PRGB64IsAlphaFFFF(c0);
    else
      return true;
  }

  static FOG_INLINE bool p64ProcessSrcPixel64(const __p64& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO)
      return !Face::p64PRGB64IsAlpha0000(c0);
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL)
      return !Face::p64PRGB64IsAlphaFFFF(c0);
    else
      return true;
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeExtGeneric]
// ============================================================================

template<typename CompositeOp, uint32_t CombineFlags, uint32_t PrepareFlags>
struct CompositeExtGeneric
{
  typedef CompositeExtCondition<CombineFlags> Cond;

  // ==========================================================================
  // [Defs]
  // ==========================================================================

  enum
  {
    NopIfDaZero    = (CombineFlags & RASTER_COMBINE_NOP_IF_DA_ZERO) != 0,
    NopIfDaFull    = (CombineFlags & RASTER_COMBINE_NOP_IF_DA_FULL) != 0,
    NopIfSaZero    = (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO) != 0,
    NopIfSaFull    = (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL) != 0,

    IsUnbound      = (CombineFlags & RASTER_COMBINE_UNBOUND       ) != 0,
    IsUnboundMskIn = (CombineFlags & RASTER_COMBINE_UNBOUND_MSK_IN) != 0,

    PrepareNeeded = (PrepareFlags != RASTER_PRGB_PREPARE_NONE),
    PrepareToFRGB = (PrepareFlags == RASTER_PRGB_PREPARE_FRGB),
    PrepareToZRGB = (PrepareFlags == RASTER_PRGB_PREPARE_ZRGB)
  };

  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_prepare_pixel32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
    if (DstF::HAS_ALPHA && !SrcF::HAS_ALPHA && SrcF::HAS_X && PrepareNeeded)
    {
      CompositeOp::prgb32_prepare_xrgb32(dst0p, src0p);
    }
  }

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_prepare_pixel32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
    if (DstF::HAS_ALPHA && !SrcF::HAS_ALPHA && SrcF::HAS_X && PrepareNeeded)
    {
      CompositeOp::prgb32_prepare_xrgb32_2031(dst0p_20, src0p_20, dst0p_31, src0p_31);
    }
  }

  // ==========================================================================
  // [Func - Op]
  // ==========================================================================

  template<typename DstF, typename SrcF, bool Pack>
  static FOG_INLINE void pixel32_op_pixel32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31)
  {
    if (DstF::HAS_ALPHA)
    {
      if (SrcF::HAS_ALPHA)
        CompositeOp::prgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else if (PrepareToFRGB ||  SrcF::HAS_F)
        CompositeOp::prgb32_op_frgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else if (PrepareToZRGB || !SrcF::HAS_X)
        CompositeOp::prgb32_op_zrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else
        CompositeOp::prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
    }
    else
    {
      if (SrcF::HAS_ALPHA)
        CompositeOp::xrgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else if (DstF::HAS_X || SrcF::HAS_X)
        CompositeOp::xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else
        CompositeOp::zrgb32_op_zrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
    }
  }

  template<typename DstF, bool Pack>
  static FOG_INLINE void pixel32_op_pixela8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8)
  {
    if (DstF::HAS_ALPHA)
    {
      CompositeOp::prgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, Pack);
    }
    else
    {
      CompositeOp::xrgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, Pack);
    }
  }

  // ==========================================================================
  // [Pixel32 - CBlit - Pixel32 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_cblit_pixel32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_DstFx1_INIT()

    uint32_t sro0p, sro0p_20, sro0p_31;

    Face::p32Copy(sro0p, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;

      DstF::p32LoadPixel32(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
        goto _C_Opaque_Skip;
      
      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

      pixel32_op_pixel32_2031<DstF, SrcF, true>(
        dst0p_20, dst0p_20, sro0p_20,
        dst0p_31, dst0p_31, sro0p_31);
      DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
      dst += DstF::SIZE;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - CBlit - Pixel32 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_cblit_pixel32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sro0p, sro0p_20, sro0p_31;

    Face::p32Copy(sro0p, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    C_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t dst0p_20, dst0p_31;

        DstF::p32LoadPixel32(dst0p_20, dst);
        if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
          goto _C_Opaque_Skip;
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        pixel32_op_pixel32_2031<DstF, SrcF, true>(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
        dst += DstF::SIZE;
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

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_UnboundIn)
      }
      else if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t src0p_20, src0p_31;
        Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0, src0p_31, sro0p_31, msk0);

        BLIT_LOOP_32x1_BEGIN(C_Mask)
          uint32_t dst0p_20, dst0p_31;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Skip:
          dst += DstF::SIZE;
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

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          if (msk0p != 0xFF)
            goto _A8_Glyph_Unbound_Mask;

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Unbound_Skip:
          dst += DstF::SIZE;
          msk += 1;
          continue;

_A8_Glyph_Unbound_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
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

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Glyph_Skip;

          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0p, src0p_31, sro0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Skip:
          dst += DstF::SIZE;
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

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32Load2a(msk0p, msk);

          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Unbound_Skip;

          Face::p32Negate256SBW(minv0p, msk0p);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Unbound_Skip:
          dst += DstF::SIZE;
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

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32Load2a(msk0p, msk);

          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0p, src0p_31, sro0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Skip:
          dst += DstF::SIZE;
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

        DstF::p32LoadPixel32(dst0p_20, dst);
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        pixel32_op_pixel32_2031<DstF, SrcF, true>(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_ARGB32_Skip:
        dst += DstF::SIZE;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32Negate255PBB(msk0p_20, msk0p_20);
        Face::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);

        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        pixel32_op_pixel32_2031<DstF, SrcF, false>(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        Face::p32Add(dst0p_20, dst0p_20, dinv0p);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [Pixel32 - VBlit - Pixel32 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixel32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;
      uint32_t src0p_20, src0p_31;

      DstF::p32LoadPixel32(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
        goto _C_Opaque_Skip;

      SrcF::p32LoadPixel32(src0p_20, src);
      pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);

      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
      Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

      pixel32_op_pixel32_2031<DstF, SrcF, true>(
        dst0p_20, dst0p_20, src0p_20,
        dst0p_31, dst0p_31, src0p_31);
      DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
      dst += DstF::SIZE;
      src += SrcF::SIZE;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - VBlit - Pixel32 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixel32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_20, src0p_31;

        DstF::p32LoadPixel32(dst0p_20, dst);
        if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
          goto _C_Opaque_Skip;

        SrcF::p32LoadPixel32(src0p_20, src);
        if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
          goto _C_Opaque_Skip;

        pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

        pixel32_op_pixel32_2031<DstF, SrcF, true>(
          dst0p_20, dst0p_20, src0p_20,
          dst0p_31, dst0p_31, src0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      Face::p32Copy(msk0p, msk0);

      if (IsUnbound && IsUnboundMskIn)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t minv0p;
        Face::p32Negate256SBW(minv0p, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound_MskIn)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Unbound_MskIn_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _C_Mask_Unbound_MskIn_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, src0p_20, msk0p, src0p_31, src0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Unbound_MskIn_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound_MskIn)
      }
      else if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t minv0p;
        Face::p32Negate256SBW(minv0p, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Unbound_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _C_Mask_Unbound_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(C_Mask)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _C_Mask_Skip;

          pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

          Face::p32MulDiv256PBW_SBW_2x(src0p_20, src0p_20, msk0p, src0p_31, src0p_31, msk0p);
          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------
  
    V_BLIT_SPAN8_A8_GLYPH()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;

          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Unbound_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixel32(src0p_20, src);

          pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

          if (msk0p != 0xFF)
            goto _A8_Glyph_Unbound_Mask;

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
          continue;

_A8_Glyph_Unbound_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          src += SrcF::SIZE;
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

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixel32(src0p_20, src);

          pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

          if (msk0p != 0xFF)
            goto _A8_Glyph_Mask;

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
          continue;

_A8_Glyph_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, src0p_20, msk0p, src0p_31, src0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;

          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Unbound_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _A8_Extra_Unbound_Skip;

          Face::p32Load2a(msk0p, msk);
          pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
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

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _A8_Extra_Skip;

          Face::p32Load2a(msk0p, msk);
          pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, src0p_20, msk0p, src0p_31, src0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra)
      }
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_20, src0p_31;

        uint32_t dinv0p;
        uint32_t msk0p_20, msk0p_31;

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000)
          goto _ARGB32_Skip;

        DstF::p32LoadPixel32(dst0p_20, dst);
        SrcF::p32LoadPixel32(src0p_20, src);

        pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        pixel32_op_pixel32_2031<DstF, SrcF, true>(
          dst0p_20, dst0p_20, src0p_20,
          dst0p_31, dst0p_31, src0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_ARGB32_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32Negate255PBB(msk0p_20, msk0p_20);
        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        pixel32_op_pixel32_2031<DstF, SrcF, false>(
          dst0p_20, dst0p_20, src0p_20,
          dst0p_31, dst0p_31, src0p_31);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        Face::p32Add(dst0p_20, dst0p_20, dinv0p);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [Pixel32 - CBlit - PixelA8 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_cblit_pixela8_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_DstFx1_INIT()

    uint32_t sro0p_a8 = src->prgb32.u32 >> 24;

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;

      DstF::p32LoadPixel32(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
        goto _C_Opaque_Skip;
      
      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

      pixel32_op_pixela8_2031<DstF, true>(
        dst0p_20, dst0p_20,
        dst0p_31, dst0p_31, sro0p_a8);
      DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
      dst += DstF::SIZE;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - CBlit - PixelA8 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_cblit_pixela8_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sro0p_a8 = src->prgb32.u32 >> 24;

    C_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t dst0p_20, dst0p_31;

        DstF::p32LoadPixel32(dst0p_20, dst);
        if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
          goto _C_Opaque_Skip;
        
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        pixel32_op_pixela8_2031<DstF, true>(
          dst0p_20, dst0p_20,
          dst0p_31, dst0p_31, sro0p_a8);
        DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
        dst += DstF::SIZE;
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

        uint32_t src0p_a8;
        Face::p32MulDiv256SBW(src0p_a8, sro0p_a8, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask_UnboundIn)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_UnboundIn)
      }
      else if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, sro0p_a8);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t src0p_a8;
        Face::p32MulDiv256SBW(src0p_a8, sro0p_a8, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask)
          uint32_t dst0p_20, dst0p_31;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Skip:
          dst += DstF::SIZE;
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

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          if (msk0p != 0xFF)
            goto _A8_Glyph_Unbound_Mask;

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, sro0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Unbound_Skip:
          dst += DstF::SIZE;
          msk += 1;
          continue;

_A8_Glyph_Unbound_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, sro0p_a8);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph)
          uint32_t dst0p_20, dst0p_31;
          uint32_t msk0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Glyph_Skip;

          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32MulDiv256SBW(msk0p, msk0p, sro0p_a8);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, msk0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Skip:
          dst += DstF::SIZE;
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

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32Load2a(msk0p, msk);

          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Unbound_Skip;

          Face::p32Negate256SBW(minv0p, msk0p);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, sro0p_a8);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Unbound_Skip:
          dst += DstF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra)
          uint32_t dst0p_20, dst0p_31;
          uint32_t msk0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32Load2a(msk0p, msk);

          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256SBW(msk0p, msk0p, sro0p_a8);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, msk0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Skip:
          dst += DstF::SIZE;
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

        DstF::p32LoadPixelA8(dst0p_20, dst);
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        pixel32_op_pixela8_2031<DstF, true>(
          dst0p_20, dst0p_20,
          dst0p_31, dst0p_31, sro0p_a8);
        DstF::p32StorePixel32(dst, dst0p_20);

_ARGB32_Skip:
        dst += DstF::SIZE;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32Negate255PBB(msk0p_20, msk0p_20);
        Face::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);

        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        pixel32_op_pixela8_2031<DstF, false>(
          dst0p_20, dst0p_20,
          dst0p_31, dst0p_31, sro0p_a8);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        Face::p32Add(dst0p_20, dst0p_20, dinv0p);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [Pixel32 - VBlit - PixelA8 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixela8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;
      uint32_t src0p_a8;

      DstF::p32LoadPixel32(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
        goto _C_Opaque_Skip;
      SrcF::p32LoadPixelA8(src0p_a8, src);

      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
      pixel32_op_pixela8_2031<DstF, true>(
        dst0p_20, dst0p_20,
        dst0p_31, dst0p_31, src0p_a8);
      DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
      dst += DstF::SIZE;
      src += SrcF::SIZE;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - VBlit - PixelA8 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixela8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_a8;

        DstF::p32LoadPixel32(dst0p_20, dst);
        if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
          goto _C_Opaque_Skip;
        SrcF::p32LoadPixelA8(src0p_a8, src);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        pixel32_op_pixela8_2031<DstF, true>(
          dst0p_20, dst0p_20,
          dst0p_31, dst0p_31, src0p_a8);
        DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      Face::p32Copy(msk0p, msk0);

      if (IsUnbound && IsUnboundMskIn)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t minv0p;
        Face::p32Negate256SBW(minv0p, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound_MskIn)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Unbound_MskIn_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _C_Mask_Unbound_MskIn_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          Face::p32MulDiv256SBW(src0p_a8, src0p_a8, msk0p);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Unbound_MskIn_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound_MskIn)
      }
      else if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t minv0p;
        Face::p32Negate256SBW(minv0p, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Unbound_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _C_Mask_Unbound_MskIn_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(C_Mask)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _C_Mask_Unbound_MskIn_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256SBW(src0p_a8, src0p_a8, msk0p);
          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------
  
    V_BLIT_SPAN8_A8_GLYPH()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;

          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Unbound_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixelA8(src0p_a8, src);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          if (msk0p != 0xFF)
            goto _A8_Glyph_Unbound_Mask;

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
          continue;

_A8_Glyph_Unbound_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;
          uint32_t msk0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixelA8(src0p_a8, src);

          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32MulDiv256SBW(src0p_a8, src0p_a8,  msk0p);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;

          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Unbound_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _A8_Extra_Unbound_Skip;

          Face::p32Load2a(msk0p, msk);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;
          uint32_t msk0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _A8_Extra_Skip;

          Face::p32Load2a(msk0p, msk);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256SBW(src0p_a8, src0p_a8, msk0p);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra)
      }
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_a8;

        uint32_t dinv0p;
        uint32_t msk0p_20, msk0p_31;

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000)
          goto _ARGB32_Skip;

        DstF::p32LoadPixel32(dst0p_20, dst);
        SrcF::p32LoadPixelA8(src0p_a8, src);
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        pixel32_op_pixela8_2031<DstF, true>(
          dst0p_20, dst0p_20,
          dst0p_31, dst0p_31, src0p_a8);
        DstF::p32StorePixel32(dst, dst0p_20);

_ARGB32_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32Negate255PBB(msk0p_20, msk0p_20);
        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        pixel32_op_pixela8_2031<DstF, false>(
          dst0p_20, dst0p_20,
          dst0p_31, dst0p_31, src0p_a8);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        Face::p32Add(dst0p_20, dst0p_20, dinv0p);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeExtSrcInSrcOut]
// ============================================================================

//! @internal
//!
//! @brief Specialized template used by @ref CompositeSrcIn and @ref CompositeSrcOut.
template<typename CompositeOp, uint32_t CombineFlags, uint32_t PrepareFlags, bool NegateDA>
struct CompositeExtSrcInSrcOut
{
  typedef CompositeExtCondition<CombineFlags> Cond;

  // ==========================================================================
  // [Defs]
  // ==========================================================================

  enum
  {
    NopIfDaZero    = (CombineFlags & RASTER_COMBINE_NOP_IF_DA_ZERO) != 0,
    NopIfDaFull    = (CombineFlags & RASTER_COMBINE_NOP_IF_DA_FULL) != 0,
    NopIfSaZero    = (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO) != 0,
    NopIfSaFull    = (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL) != 0,

    IsUnbound      = (CombineFlags & RASTER_COMBINE_UNBOUND       ) != 0,
    IsUnboundMskIn = (CombineFlags & RASTER_COMBINE_UNBOUND_MSK_IN) != 0,

    PrepareNeeded = (PrepareFlags != RASTER_PRGB_PREPARE_NONE),
    PrepareToFRGB = (PrepareFlags == RASTER_PRGB_PREPARE_FRGB),
    PrepareToZRGB = (PrepareFlags == RASTER_PRGB_PREPARE_ZRGB)
  };

  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_prepare_pixel32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
    if (!SrcF::HAS_ALPHA && SrcF::HAS_X && PrepareNeeded)
    {
      CompositeOp::prgb32_prepare_xrgb32(dst0p, src0p);
    }
  }

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_prepare_pixel32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
    if (!SrcF::HAS_ALPHA && SrcF::HAS_X && PrepareNeeded)
    {
      CompositeOp::prgb32_prepare_xrgb32_2031(dst0p_20, src0p_20, dst0p_31, src0p_31);
    }
  }

  static FOG_INLINE bool pixel32_skip(const uint32_t& pix0)
  {
    if (NegateDA)
      return false;
    else
      return pix0 == 0;
  }

  // ==========================================================================
  // [Pixel32 - CBlit - Pixel32 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_cblit_pixel32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_DstFx1_INIT()

    uint32_t sro0p, sro0p_20, sro0p_31;

    Face::p32Copy(sro0p, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t dst0p;

      DstF::p32LoadPixelA8(dst0p, dst);
      if (pixel32_skip(dst0p))
        goto _C_Opaque_Skip;
      if (NegateDA) Face::p32Negate255SBW(dst0p, dst0p);

      Face::p32MulDiv255PBW_SBW_2x_Pack_2031(dst0p, sro0p_20, dst0p, sro0p_31, dst0p);
      DstF::p32StorePixel32(dst, dst0p);

_C_Opaque_Skip:
      dst += DstF::SIZE;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - CBlit - Pixel32 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_cblit_pixel32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sro0p, sro0p_20, sro0p_31;

    Face::p32Copy(sro0p, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    C_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_DstFx1_INIT()

      BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
        uint32_t dst0p;

        DstF::p32LoadPixelA8(dst0p, dst);
        if (pixel32_skip(dst0p))
          goto _C_Opaque_Skip;
        if (NegateDA) Face::p32Negate255SBW(dst0p, dst0p);

        Face::p32MulDiv255PBW_SBW_2x_Pack_2031(dst0p, sro0p_20, dst0p, sro0p_31, dst0p);
        DstF::p32StorePixel32(dst, dst0p);

_C_Opaque_Skip:
        dst += DstF::SIZE;
      BLIT_LOOP_DstFx1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t src0p_20, src0p_31;
      uint32_t minv0p;

      Face::p32Negate256SBW(minv0p, msk0);
      Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0, src0p_31, sro0p_31, msk0);

      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p_20, dst0p_31;
        uint32_t t0p_20, t0p_31;

        DstF::p32LoadPixelA8(dst0p_20, dst);
        if (pixel32_skip(dst0p_20))
          goto _C_Mask_Skip;
        DstF::p32LoadPixel32(dst0p_20, dst);
        Face::p32ExtractPBB3(t0p_20, dst0p_31);
        if (NegateDA) Face::p32Negate255SBW(t0p_20, t0p_20);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32Mul_2x(dst0p_20, dst0p_20, minv0p, dst0p_31, dst0p_31, minv0p);
        Face::p32Mul_2x(t0p_20, src0p_20, t0p_20, t0p_31, src0p_31, t0p_20);
        Face::p32Add_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

        Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);
_C_Mask_Skip:
        dst += DstF::SIZE;
      BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------
  
    C_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p_20, dst0p_31;
        uint32_t t0p_20, t0p_31;
        uint32_t msk0p;

        Face::p32Load1b(msk0p, msk);
        if (msk0p == 0x00)
          goto _A8_Glyph_Skip;

        DstF::p32LoadPixel32(dst0p_20, dst);
        Face::p32ExtractPBB3(t0p_20, dst0p_31);
        if (NegateDA) Face::p32Negate255SBW(t0p_20, t0p_20);

        Face::p32MulDiv255SBW(t0p_20, t0p_20, msk0p);
        Face::p32Negate255PBW(msk0p, msk0p);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32Mul_2x(dst0p_20, dst0p_20, msk0p, dst0p_31, dst0p_31, msk0p);
        Face::p32Mul_2x(t0p_20, sro0p_20, t0p_20, t0p_31, sro0p_31, t0p_20);
        Face::p32Add_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

        Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Skip:
        dst += DstF::SIZE;
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
        uint32_t dst0p_20, dst0p_31;
        uint32_t t0p;
        uint32_t msk0p;

        DstF::p32LoadPixel32(dst0p_20, dst);
        if (pixel32_skip(dst0p_20))
          goto _A8_Extra_Skip;

        Face::p32Load2a(msk0p, msk);
        Face::p32ExtractPBB3(t0p, dst0p_20);
        if (NegateDA) Face::p32Negate255SBW(t0p, t0p);

        Face::p32MulDiv256SBW(t0p, t0p, msk0p);
        Face::p32Negate256PBW(msk0p, msk0p);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
        Face::p32MulDiv255PBW_SBW_2x_Pack_2031(t0p, sro0p_20, t0p, sro0p_31, t0p);
        Face::p32Add(dst0p_20, dst0p_20, t0p);
        DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Skip:
        dst += DstF::SIZE;
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
        uint32_t dst0p_20, dst0p_31;
        uint32_t dinv0p;
        uint32_t msk0p_20, msk0p_31;

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000)
          goto _ARGB32_Skip;

        DstF::p32LoadPixel32(dst0p_20, dst);
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        Face::p32RShift(dst0p_20, dst0p_20, 24);
        if (NegateDA) Face::p32Negate255SBW(dst0p_20, dst0p_20);

        Face::p32MulDiv255PBW_SBW_2x_Pack_2031(dst0p_20, sro0p_20, dst0p_20, sro0p_31, dst0p_20);
        DstF::p32StorePixel32(dst, dst0p_20);

_ARGB32_Skip:
        dst += DstF::SIZE;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32Negate255PBB(msk0p_20, msk0p_20);
        Face::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);

        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        Face::p32RShift(dst0p_20, dst0p_20, 24);
        Face::p32MulDiv255PBW_SBW_2x(msk0p_20, msk0p_20, dst0p_20, msk0p_31, msk0p_31, dst0p_20);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, msk0p_20, sro0p_20, msk0p_31, sro0p_31);

        Face::p32Add(dst0p_20, dst0p_20, dinv0p);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [Pixel32 - VBlit - Pixel32 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixel32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_DstFx1_INIT()

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t dst0p;
      uint32_t src0p_20, src0p_31;

      DstF::p32LoadPixelA8(dst0p, dst);
      if (pixel32_skip(dst0p))
        goto _C_Opaque_Skip;

      SrcF::p32LoadPixel32(src0p_20, src);
      pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);

      Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
      if (NegateDA) Face::p32Negate255SBW(dst0p, dst0p);

      Face::p32MulDiv255PBW_SBW_2x_Pack_2031(dst0p, src0p_20, dst0p, src0p_31, dst0p);
      DstF::p32StorePixel32(dst, dst0p);

_C_Opaque_Skip:
      dst += DstF::SIZE;
      src += SrcF::SIZE;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - VBlit - Pixel32 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixel32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_DstFx1_INIT()

      BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
        uint32_t dst0p;
        uint32_t src0p_20, src0p_31;

        DstF::p32LoadPixelA8(dst0p, dst);
        if (pixel32_skip(dst0p))
          goto _C_Opaque_Skip;

        SrcF::p32LoadPixel32(src0p_20, src);
        pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);

        Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
        if (NegateDA) Face::p32Negate255SBW(dst0p, dst0p);

        Face::p32MulDiv255PBW_SBW_2x_Pack_2031(dst0p, src0p_20, dst0p, src0p_31, dst0p);
        DstF::p32StorePixel32(dst, dst0p);

_C_Opaque_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
      BLIT_LOOP_DstFx1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      uint32_t minv0p;

      Face::p32Copy(msk0p, msk0);
      Face::p32Negate256SBW(minv0p, msk0);

      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_20, src0p_31;
        uint32_t t0p;

        DstF::p32LoadPixel32(dst0p_20, dst);
        SrcF::p32LoadPixel32(src0p_20, src);
        pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);

        Face::p32ExtractPBB3(t0p, dst0p_31);
        if (NegateDA) Face::p32Negate255SBW(t0p, t0p);

        Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        Face::p32MulDiv255SBW(t0p, t0p, msk0p);
        Face::p32Mul_2x(src0p_20, src0p_20, t0p, src0p_31, src0p_31, t0p);
        Face::p32Mul_2x(dst0p_20, dst0p_20, minv0p, dst0p_31, dst0p_31, minv0p);

        Face::p32Add_2x(dst0p_20, dst0p_20, src0p_20, dst0p_31, dst0p_31, src0p_31);
        Face::p32Div256PBW_2x_Pack_2031(dst0p_20, dst0p_20, dst0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        src += SrcF::SIZE;
      BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------
  
    V_BLIT_SPAN8_A8_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_20, src0p_31;

        uint32_t t0p;
        uint32_t msk0p;

        Face::p32Load1b(msk0p, msk);
        if (msk0p == 0x00)
          goto _A8_Glyph_Skip;

        DstF::p32LoadPixel32(dst0p_20, dst);
        SrcF::p32LoadPixel32(src0p_20, src);
        pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);

        Face::p32ExtractPBB3(t0p, dst0p_31);
        if (NegateDA) Face::p32Negate255SBW(t0p, t0p);

        Face::p32MulDiv255SBW(t0p, t0p, msk0p);
        Face::p32Negate255PBW(msk0p, msk0p);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32Mul_2x(dst0p_20, dst0p_20, msk0p, dst0p_31, dst0p_31, msk0p);
        Face::p32Mul_2x(src0p_20, src0p_20, t0p, src0p_31, src0p_31, t0p);
        Face::p32Add_2x(dst0p_20, dst0p_20, src0p_20, dst0p_31, dst0p_31, src0p_31);

        Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 1;
      BLIT_LOOP_32x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(A8_Glyph)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_20, src0p_31;

        uint32_t t0p;
        uint32_t msk0p;

        Face::p32Load2a(msk0p, msk);

        DstF::p32LoadPixel32(dst0p_20, dst);
        SrcF::p32LoadPixel32(src0p_20, src);
        pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);

        Face::p32ExtractPBB3(t0p, dst0p_31);
        if (NegateDA) Face::p32Negate255SBW(t0p, t0p);

        Face::p32MulDiv255SBW(t0p, t0p, msk0p);
        Face::p32Negate255PBW(msk0p, msk0p);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32Mul_2x(dst0p_20, dst0p_20, msk0p, dst0p_31, dst0p_31, msk0p);
        Face::p32Mul_2x(src0p_20, src0p_20, t0p, src0p_31, src0p_31, t0p);
        Face::p32Add_2x(dst0p_20, dst0p_20, src0p_20, dst0p_31, dst0p_31, src0p_31);

        Face::p32Div256PBW_2x_Pack_2031(dst0p_20, dst0p_20, dst0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        src += SrcF::SIZE;
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
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_20, src0p_31;
        uint32_t t0p;
        uint32_t msk0p_20, msk0p_31;

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000)
          goto _ARGB32_Skip;

        DstF::p32LoadPixel32(dst0p_20, dst);
        DstF::p32LoadPixel32(src0p_20, src);

        pixel32_prepare_pixel32<DstF, SrcF>(src0p_20, src0p_20);
        Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        Face::p32RShift(dst0p_20, dst0p_20, 24);
        if (NegateDA) Face::p32Negate255SBW(dst0p_20, dst0p_20);

        Face::p32MulDiv255PBW_SBW_2x_Pack_2031(src0p_20, src0p_20, dst0p_20, src0p_31, dst0p_20);
        DstF::p32StorePixel32(dst, src0p_20);

_ARGB32_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32ExtractPBW1(t0p, dst0p_20);
        if (NegateDA) Face::p32Negate255SBW(t0p, t0p);

        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);

        Face::p32MulDiv255PBW_SBW_2x(src0p_20, src0p_20, t0p, src0p_31, src0p_31, t0p);
        Face::p32MulDiv256PBW_2x_Pack_2031(src0p_20, src0p_20, msk0p_20, src0p_31, msk0p_31);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        Face::p32Add(dst0p_20, dst0p_20, src0p_20);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [Pixel32 - VBlit - PixelA8 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixela8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t dst0p;
      uint32_t src0p;

      DstF::p32LoadPixel32(dst0p, dst);
      if (pixel32_skip(dst0p))
        goto _C_Opaque_Skip;

      SrcF::p32LoadPixelA8(src0p, src);
      Face::p32RShift(dst0p, dst0p, 24);
      if (NegateDA) Face::p32Negate255SBW(dst0p, dst0p);

      Face::p32MulDiv255SBW(dst0p, dst0p, src0p);
      Face::p32ExtendPBBFromSBB(dst0p, dst0p);

      DstF::p32StorePixel32(dst, dst0p);

_C_Opaque_Skip:
      dst += DstF::SIZE;
      src += SrcF::SIZE;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - VBlit - PixelA8 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixela8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_DstFx1_INIT()

      BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
        uint32_t dst0p;
        uint32_t src0p;

        DstF::p32LoadPixel32(dst0p, dst);
        if (pixel32_skip(dst0p))
          goto _C_Opaque_Skip;

        SrcF::p32LoadPixelA8(src0p, src);
        Face::p32RShift(dst0p, dst0p, 24);
        if (NegateDA) Face::p32Negate255SBW(dst0p, dst0p);

        Face::p32MulDiv255SBW(dst0p, dst0p, src0p);
        Face::p32ExtendPBBFromSBB(dst0p, dst0p);

        DstF::p32StorePixel32(dst, dst0p);

_C_Opaque_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      uint32_t minv0p;

      Face::p32Copy(msk0p, msk0);
      Face::p32Negate256SBW(minv0p, msk0);

      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;
        uint32_t t0p;

        DstF::p32LoadPixel32(dst0p, dst);
        if (pixel32_skip(dst0p))
          goto _C_Mask_Skip;

        SrcF::p32LoadPixelA8(src0p, src);
        Face::p32ExtractPBB3(t0p, dst0p);
        if (NegateDA) Face::p32Negate255SBW(t0p, t0p);

        Face::p32MulDiv256PBB_SBW(dst0p, dst0p, minv0p);
        Face::p32MulDiv256SBW(src0p, src0p, t0p);
        Face::p32ExtendPBBFromSBB(src0p, src0p);

        Face::p32Add(dst0p, dst0p, src0p);
        DstF::p32StorePixel32(dst, dst0p);

_C_Mask_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
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
        uint32_t t0p;

        Face::p32Load1b(msk0p, msk);
        DstF::p32LoadPixel32(dst0p, dst);

        if ((msk == 0x00) | pixel32_skip(dst0p))
          goto _A8_Glyph_Skip;

        SrcF::p32LoadPixelA8(src0p, src);
        Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
        Face::p32ExtractPBB3(t0p, dst0p);

        if (NegateDA) Face::p32Negate255SBW(t0p, t0p);
        Face::p32MulDiv255SBW(src0p, src0p, t0p);
        Face::p32MulDiv256SBW(src0p, src0p, msk0p);

        Face::p32Negate256SBW(msk0p, msk0p);
        Face::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Face::p32ExtendPBBFromSBB(src0p, src0p);

        Face::p32Add(dst0p, dst0p, src0p);
        DstF::p32StorePixel32(dst, dst0p);

_A8_Glyph_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
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
        uint32_t t0p;

        Face::p32Load2a(msk0p, msk);
        DstF::p32LoadPixel32(dst0p, dst);

        if (pixel32_skip(dst0p))
          goto _A8_Extra_Skip;

        SrcF::p32LoadPixelA8(src0p, src);
        Face::p32ExtractPBB3(t0p, dst0p);

        if (NegateDA) Face::p32Negate255SBW(t0p, t0p);
        Face::p32MulDiv255SBW(src0p, src0p, t0p);
        Face::p32MulDiv256SBW(src0p, src0p, msk0p);

        Face::p32Negate256SBW(msk0p, msk0p);
        Face::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
        Face::p32ExtendPBBFromSBB(src0p, src0p);

        Face::p32Add(dst0p, dst0p, src0p);
        DstF::p32StorePixel32(dst, dst0p);

_A8_Extra_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
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

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000)
          goto _ARGB32_Skip;

        DstF::p32LoadPixel32(dst0p, dst);
        if (pixel32_skip(dst0p))
          goto _ARGB32_Skip;

        SrcF::p32LoadPixelA8(src0p, src);
        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        Face::p32RShift(dst0p, dst0p, 24);
        if (NegateDA) Face::p32Negate255SBW(dst0p, dst0p);

        Face::p32MulDiv255SBW(dst0p, dst0p, src0p);
        Face::p32ExtendPBBFromSBB(dst0p, dst0p);
        DstF::p32StorePixel32(dst, dst0p);

_ARGB32_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32ExtractPBB3(msk0p_31, dst0p);
        Face::p32MulDiv255SBW(src0p, src0p, msk0p_31);
        
        Face::p32UnpackPBW256FromPBB255_2031(msk0p_20, msk0p_31, msk0p_20);
        Face::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, msk0p_20, src0p, msk0p_31, src0p);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBB_PBW_2031(dst0p, dst0p, msk0p_20, msk0p_31);

        Face::p32Add(dst0p, dst0p, src0p);
        DstF::p32StorePixel32(dst, dst0p);

        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_line<PixelPRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_span<PixelPRGB32, PixelPRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_xrgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_line<PixelPRGB32, PixelFRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_xrgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_span<PixelPRGB32, PixelFRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_line<PixelPRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_span<PixelPRGB32, PixelPRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_xrgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_line<PixelPRGB32, PixelXRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_xrgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_span<PixelPRGB32, PixelXRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_rgb24_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_line<PixelPRGB32, PixelRGB24>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_rgb24_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_span<PixelPRGB32, PixelRGB24>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixela8_line<PixelPRGB32, PixelA8>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixela8_span<PixelPRGB32, PixelA8>(dst, span, closure);
  }
};

template<typename CompositeOp, uint32_t CombineFlags, uint32_t PrepareFlags>
struct CompositeExtPrgbVsPrgb
{
  typedef CompositeExtGeneric<CompositeOp, CombineFlags, PrepareFlags> Impl;

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixel32_line<PixelPRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixel32_span<PixelPRGB32, PixelPRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_xrgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixel32_line<PixelPRGB32, PixelFRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_xrgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixel32_span<PixelPRGB32, PixelFRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_line<PixelPRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_span<PixelPRGB32, PixelPRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_xrgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_line<PixelPRGB32, PixelXRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_xrgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_span<PixelPRGB32, PixelXRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_rgb24_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_line<PixelPRGB32, PixelRGB24>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_rgb24_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_span<PixelPRGB32, PixelRGB24>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_line<PixelPRGB32, PixelA8>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_span<PixelPRGB32, PixelA8>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixel32_line<PixelXRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixel32_span<PixelXRGB32, PixelPRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_xrgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixel32_line<PixelXRGB32, PixelFRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_xrgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixel32_span<PixelXRGB32, PixelFRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_line<PixelXRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_span<PixelXRGB32, PixelPRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_xrgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_line<PixelXRGB32, PixelXRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_xrgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_span<PixelXRGB32, PixelXRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_rgb24_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_line<PixelXRGB32, PixelRGB24>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_rgb24_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixel32_span<PixelXRGB32, PixelRGB24>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_line<PixelXRGB32, PixelA8>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_span<PixelXRGB32, PixelA8>(dst, span, closure);
  }
};

template<typename CompositeOp, uint32_t CombineFlags, uint32_t PrepareFlags>
struct CompositeExtPrgbVsA
{
  typedef CompositeExtGeneric<CompositeOp, CombineFlags, PrepareFlags> Impl;

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixela8_line<PixelPRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixela8_span<PixelPRGB32, PixelPRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_line<PixelPRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_span<PixelPRGB32, PixelPRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_line<PixelPRGB32, PixelA8>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_span<PixelPRGB32, PixelA8>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixela8_line<PixelXRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_cblit_pixela8_span<PixelXRGB32, PixelPRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_line<PixelXRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_span<PixelXRGB32, PixelPRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_line<PixelXRGB32, PixelA8>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    Impl::template pixel32_vblit_pixela8_span<PixelXRGB32, PixelA8>(dst, span, closure);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeSrcIn]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeSrcIn : public CompositeExtSrcInSrcOut<
  CompositeSrcIn, RASTER_COMBINE_OP_SRC_IN, RASTER_PRGB_PREPARE_NONE,
  false>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeSrcOut]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeSrcOut : public CompositeExtSrcInSrcOut<
  CompositeSrcOut, RASTER_COMBINE_OP_SRC_OUT, RASTER_PRGB_PREPARE_NONE,
  true>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeSrcAtop]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeSrcAtop : public CompositeExtPrgbVsPrgb<
  CompositeSrcAtop, RASTER_COMBINE_OP_SRC_ATOP, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Sca.Da + Dca.(1 - Sa)
  // Da'  = Sa.Da + Da.(1 - Sa) 
  //      = Sa.Da + Da - Da.Sa
  //      = Da
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t sainv;
    uint32_t t0p_20, t0p_31;

    Face::p32ExtractPBW1(sainv, b0p_31);
    Face::p32ExtractPBW1(t0p_20, a0p_31);

    Face::p32Negate255SBW(sainv, sainv);
    Face::p32Copy(t0p_31, t0p_20);

    // Sca.Da.
    Face::p32Mul_2x(t0p_20, t0p_20, b0p_20, t0p_31, t0p_31, b0p_31);
    // Dca.(1 - Sa).
    Face::p32Mul_2x(dst0p_20, a0p_20, sainv, dst0p_31, a0p_31, sainv);
    // Sca.Da + Dca.(1 - Sa).
    Face::p32Add_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

    if (pack)
      Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
    else
      Face::p32Div255PBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as SrcIn.
    uint32_t a0p_a8;
    Face::p32ExtractPBW1(a0p_a8, a0p_31);

    if (pack)
      Face::p32MulDiv255PBW_SBW_2x_Pack_20F1(dst0p_20, b0p_20, a0p_a8, b0p_31, a0p_a8);
    else
      Face::p32MulDiv255PBW_SBW_2x(dst0p_20, b0p_20, a0p_a8, dst0p_31, b0p_31, a0p_a8);
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

  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as SrcOver.
    CompositeSrcOver::xrgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as Src.
    CompositeSrc::xrgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Sa.(Da - Dca) + Dca
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;
    
    Face::p32ExtractPBW1(t0p_20, a0p_31);
    Face::p32ExtendPBWFromSBW(t0p_20, t0p_20);
    Face::p32Copy(t0p_31, t0p_20);
    Face::p32Sub_2x(t0p_20, t0p_20, a0p_20, t0p_31, t0p_31, a0p_31);
    Face::p32MulDiv255PBW_SBW_2x(t0p_20, t0p_20, b0p_a8, t0p_31, t0p_31, b0p_a8);
    Face::p32Add_2x(dst0p_20, a0p_20, t0p_20, dst0p_31, a0p_31, t0p_31);
    
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    // Same as SrcOver.
    CompositeSrcOver::xrgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, pack);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // NOP.
    Face::p32Copy(dst0p, a0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeDstOver]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeDstOver : public CompositeExtPrgbVsPrgb<
  CompositeDstOver, RASTER_COMBINE_OP_DST_OVER, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Dca + Sca.(1 - Da)
  // Da'  = Da + Sa.(1 - Da)
  //      = Da + Sa - Da.Sa
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;

    // Sca.(1 - Da).
    Face::p32ExtractPBW1(t0p_20, a0p_31);
    Face::p32Negate255SBW(t0p_20, t0p_20);
    Face::p32MulDiv255PBW_SBW_2x(t0p_20, b0p_20, t0p_20, t0p_31, b0p_31, t0p_20);

    // Dca + Sca.(1 - Da).
    Face::p32Add_2x(dst0p_20, a0p_20, t0p_20, dst0p_31, a0p_31, t0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dca' = Dca + Sc.(1 - Da)
  // Da'  = 1
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // We can use PRGB32 OP PRGB32 function, there is no difference.
    prgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
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

  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as Dst.
    if (pack)
      Face::p32PackPBB2031FromPBW(dst0p_20, a0p_20, a0p_31);
    else
      Face::p32Copy_2x(dst0p_20, a0p_20, dst0p_31, a0p_31);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Dca + Sa.(1 - Da).
  // Da'  = Da + Sa.(1 - Da).
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t t0p;

    Face::p32ExtractPBW1(t0p, a0p_31);
    Face::p32Negate255SBW(t0p, t0p);
    Face::p32MulDiv255SBW(t0p, t0p, b0p_a8);

    if (pack)
    {
      Face::p32PackPBB2031FromPBW(dst0p_20, a0p_20, a0p_31);
      Face::p32ExtendPBBFromSBB(t0p, t0p);
      Face::p32Add(dst0p_20, dst0p_20, t0p);
    }
    else
    {
      Face::p32ExtendPBWFromSBW(t0p, t0p);
      Face::p32Add_2x(dst0p_20, a0p_20, t0p, dst0p_31, a0p_31, t0p);
    }
  }

  // Dc' = Dc + Sa.(1 - Da).
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t t0p;

    Face::p32ExtractPBW1(t0p, a0p_31);
    Face::p32Negate255SBW(t0p, t0p);
    Face::p32MulDiv255SBW(t0p, t0p, b0p_a8);

    if (pack)
    {
      Face::p32PackPBB2031FromPBW(dst0p_20, a0p_20, a0p_31);
      Face::p32ExtendPBBFromSBB_Z210(t0p, t0p);
      Face::p32Add(dst0p_20, dst0p_20, t0p);
      Face::p32FillPBB3(dst0p_20, dst0p_20);
    }
    else
    {
      Face::p32Add(dst0p_31, a0p_31, t0p);
      Face::p32ExtendPBWFromSBW(t0p, t0p);
      Face::p32Add(dst0p_20, a0p_20, t0p);
      Face::p32FillPBW1(dst0p_31, dst0p_31);
    }
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // Same as SrcOver.
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeDstIn]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeDstIn : public CompositeExtPrgbVsA<
  CompositeDstIn, RASTER_COMBINE_OP_DST_IN, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Dca.Sa.
  // Da'  = Da.Sa.
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    if (pack)
      Face::p32MulDiv255PBW_SBW_2x_Pack_2031(dst0p_20, a0p_20, b0p_a8, a0p_31, b0p_a8);
    else
      Face::p32MulDiv255PBW_SBW_2x(dst0p_20, a0p_20, b0p_a8, dst0p_31, a0p_31, b0p_a8);
  }

  // Dc' = Dc.Sa.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    if (pack)
    {
      Face::p32MulDiv255PBW_SBW_2x_Pack_20F1(dst0p_20, a0p_20, b0p_a8, a0p_31, b0p_a8);
    }
    else
    {
      Face::p32MulDiv255PBW(dst0p_20, a0p_20, b0p_a8);
      Face::p32ZeroPBW1(dst0p_31, a0p_31);
      Face::p32MulDiv255SBW(dst0p_31, a0p_31, b0p_a8);
      Face::p32FillPBW1(dst0p_31, dst0p_31);
    }
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  // Da' = Da.Sa.
  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    Face::p32MulDiv255SBW(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeDstOut]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeDstOut : public CompositeExtPrgbVsA<
  CompositeDstOut, RASTER_COMBINE_OP_DST_OUT, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Dca.(1 - Sa).
  // Da'  = Da.(1 - Sa).
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t binv0p_a8;
    Face::p32Negate255SBW(binv0p_a8, b0p_a8);

    if (pack)
      Face::p32MulDiv255PBW_SBW_2x_Pack_2031(dst0p_20, a0p_20, binv0p_a8, a0p_31, binv0p_a8);
    else
      Face::p32MulDiv255PBW_SBW_2x(dst0p_20, a0p_20, binv0p_a8, dst0p_31, a0p_31, binv0p_a8);
  }

  // Dc' = Dc.(1 - Sa).
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t binv0p_a8;
    Face::p32Negate255SBW(binv0p_a8, b0p_a8);

    if (pack)
    {
      Face::p32MulDiv255PBW_SBW_2x_Pack_20F1(dst0p_20, a0p_20, binv0p_a8, a0p_31, binv0p_a8);
    }
    else
    {
      Face::p32MulDiv255PBW(dst0p_20, a0p_20, binv0p_a8);
      Face::p32ZeroPBW1(dst0p_31, a0p_31);
      Face::p32MulDiv255SBW(dst0p_31, a0p_31, binv0p_a8);
      Face::p32FillPBW1(dst0p_31, dst0p_31);
    }
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  // Da' = Da.(1 - Sa).
  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    uint32_t binv0p;
    Face::p32Negate255SBW(binv0p, b0p);
    Face::p32MulDiv255SBW(dst0p, a0p, binv0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeDstAtop]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeDstAtop : public CompositeExtPrgbVsPrgb<
  CompositeDstAtop, RASTER_COMBINE_OP_DST_ATOP, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Dca.Sa + Sca.(1 - Da)
  // Da'  = Da.Sa + Sa.(1 - Da)
  //      = Sa.(Da + 1 - Da)
  //      = Sa
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t sa;
    uint32_t t0p_20, t0p_31;

    Face::p32ExtractPBW1(t0p_20, a0p_31);
    Face::p32ExtractPBW1(sa, b0p_31);

    Face::p32Negate255SBW(t0p_20, t0p_20);
    Face::p32Copy(t0p_31, t0p_20);

    // Sca.(1 - Da)
    Face::p32Mul_2x(t0p_20, t0p_20, b0p_20, t0p_31, t0p_31, b0p_31);
    // Dca.Sa.
    Face::p32Mul_2x(dst0p_20, a0p_20, sa, dst0p_31, a0p_31, sa);
    // Dca.Sa + Sca.(1 - Da).
    Face::p32Add_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

    if (pack)
      Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
    else
      Face::p32Div255PBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as DstOver.
    CompositeDstOver::prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
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

  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as DstIn.
    uint32_t b0p_a8;
    Face::p32ExtractPWW1(b0p_a8, b0p_31);
    CompositeDstIn::xrgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, pack);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as Dst.
    Face::p32Copy_2x(dst0p_20, a0p_20, dst0p_31, a0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Dca.Sa + Sa.(1 - Da) = Sa.(Dca + 1 - Da).
  // Da'  = Da.Sa + Sa.(1 - Da) = Sa.(Da + 1 - Da).
  //      = Sa
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t da;
    Face::p32ExtractPBW1(da, a0p_31);
    Face::p32ExtendPBWFromSBW(da, da);

    Face::p32Add_2x(dst0p_20, a0p_20, 0xFF, dst0p_31, a0p_31, 0xFF);
    Face::p32Sub_2x(dst0p_20, dst0p_20, da, dst0p_31, dst0p_31, da);

    if (pack)
      Face::p32MulDiv255PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, b0p_a8, dst0p_31, b0p_a8);
    else
      Face::p32MulDiv255PBW_SBW_2x(dst0p_20, dst0p_20, b0p_a8, dst0p_31, dst0p_31, b0p_a8);
  }

  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    // Same as SrcOver.
    CompositeSrcOver::xrgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, pack);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // NOP.
    Face::p32Copy(dst0p, a0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeXor]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeXor : public CompositeExtPrgbVsPrgb<
  CompositeXor, RASTER_COMBINE_OP_XOR, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
    
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Sca.(1 - Da) + Dca.(1 - Sa)
  // Da'  = Sa.(1 - Da) + Da.(1 - Sa)
  //      = Sa + Da - 2.Sa.Da
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;
    uint32_t sa;
    
    Face::p32ExtractPBW1(t0p_20, a0p_31);
    Face::p32ExtractPBW1(sa, b0p_31);

    Face::p32Negate255SBW(t0p_20, t0p_20);
    Face::p32Negate255SBW(sa, sa);
    
    Face::p32Copy(t0p_31, t0p_20);

    // Sca.(1 - Da).
    Face::p32Mul_2x(t0p_20, t0p_20, b0p_20, t0p_31, t0p_31, b0p_31);
    // Dca.(1 - Sa).
    Face::p32Mul_2x(dst0p_20, a0p_20, sa, dst0p_31, a0p_31, sa);
    // Sca.(1 - Da) + Dca.(1 - Sa).
    Face::p32Add_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);
    
    if (pack)
      Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
    else
      Face::p32Div255PBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as SrcOut.
    uint32_t a0p_a8;
    Face::p32ExtractPBW1(a0p_a8, a0p_31);
    Face::p32Negate255SBW(a0p_a8, a0p_a8);

    if (pack)
      Face::p32MulDiv255PBW_SBW_2x_Pack_20F1(dst0p_20, b0p_20, a0p_a8, b0p_31, a0p_a8);
    else
      Face::p32MulDiv255PBW_SBW_2x(dst0p_20, b0p_20, a0p_a8, dst0p_31, b0p_31, a0p_a8);
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

  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as DstOut.
    uint32_t b0p_a8;

    Face::p32ExtractPBW1(b0p_a8, b0p_31);
    xrgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // Same as Clear.
    Face::p32Zero_2x(dst0p_20, dst0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Sa.(1 - Da) + Dca.(1 - Sa).
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t t0p;
    uint32_t u0p;

    // Sa.(1 - Da).
    Face::p32ExtractPBW1(t0p, a0p_31);
    Face::p32Negate255SBW(t0p, t0p);
    Face::p32Mul(t0p, t0p, b0p_a8);

    Face::p32ExtendPBWFromSBW(t0p, t0p);

    // Dca.(1 - Sa).
    Face::p32Negate255SBW(u0p, b0p_a8);
    Face::p32Mul_2x(dst0p_20, dst0p_20, u0p, dst0p_31, dst0p_31, u0p);

    // Sa.(1 - Da) + Dca.(1 - Sa).
    Face::p32Add_2x(dst0p_20, dst0p_20, t0p, dst0p_31, dst0p_31, t0p);
    
    if (pack)
      Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
    else
      Face::p32Div255PBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31);
  }

  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    // Same as DstOut.
    CompositeDstOut::xrgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, pack);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    uint32_t ainv0p;
    uint32_t binv0p;

    Face::p32Negate255SBW(ainv0p, a0p);
    Face::p32Negate255SBW(binv0p, b0p);

    Face::p32Mul(ainv0p, ainv0p, b0p);
    Face::p32Mul(binv0p, binv0p, a0p);

    Face::p32Add(dst0p, ainv0p, binv0p);
    Face::p32Div255SBW(dst0p, dst0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeAdd]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeAdd : public CompositeExtPrgbVsPrgb<
  CompositeAdd, RASTER_COMBINE_OP_ADD, RASTER_PRGB_PREPARE_FRGB>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
    Face::p32FillPBB3(dst0p, src0p);
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
    Face::p32FillPBB3(dst0p_31, src0p_31);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
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

  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t b0p_a8_extended;

    Face::p32ExtendPBWFromSBW(b0p_a8_extended, b0p_a8);
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_a8_extended, dst0p_31, a0p_31, b0p_a8_extended);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    prgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    Face::p32Addus255SBW(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeSubtract]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeSubtract : public CompositeExtPrgbVsPrgb<
  CompositeSubtract, RASTER_COMBINE_OP_SUBTRACT, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Dca - Sca.
  // Da'  = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t dst0p_a0;
    uint32_t dst0p_b0;

    Face::p32ExtractPBW1(dst0p_a0, a0p_31);
    Face::p32ExtractPBW1(dst0p_b0, b0p_31);
    Face::p32OpOverA8(dst0p_a0, dst0p_a0, dst0p_b0);

    Face::p32Subus255PBW(dst0p_20, a0p_20, b0p_20);
    Face::p32Subus255PBW_ZeroPBW1(dst0p_31, a0p_31, b0p_31);

    if (pack)
    {
      Face::p32LShift(dst0p_a0, dst0p_a0, 24);
      Face::p32LShift(dst0p_31, dst0p_31,  8);

      Face::p32Or(dst0p_20, dst0p_20, dst0p_a0);
      Face::p32Or(dst0p_20, dst0p_20, dst0p_31);
    }
    else
    {
      Face::p32LShift(dst0p_a0, dst0p_a0, 16);
      Face::p32Or(dst0p_31, dst0p_31, dst0p_a0);
    }
  }

  // Dca' = Dca - Sca.
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Subus255PBW(dst0p_20, a0p_20, b0p_20);
    Face::p32Subus255PBW_FillPBW1(dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
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
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Dca - Sa.
  // Dc'  = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t dst0p_a0;
    uint32_t b0p_a8_extended;

    Face::p32ExtendPBWFromSBW(b0p_a8_extended, b0p_a8);

    Face::p32ExtractPBW1(dst0p_a0, a0p_31);
    Face::p32OpOverA8(dst0p_a0, dst0p_a0, b0p_a8);

    Face::p32Subus255PBW(dst0p_20, a0p_20, b0p_a8_extended);
    Face::p32Subus255PBW_ZeroPBW1(dst0p_31, a0p_31, b0p_a8_extended);

    if (pack)
    {
      Face::p32LShift(dst0p_a0, dst0p_a0, 24);
      Face::p32LShift(dst0p_31, dst0p_31,  8);

      Face::p32Or(dst0p_20, dst0p_20, dst0p_a0);
      Face::p32Or(dst0p_20, dst0p_20, dst0p_31);
    }
    else
    {
      Face::p32LShift(dst0p_a0, dst0p_a0, 16);
      Face::p32Or(dst0p_31, dst0p_31, dst0p_a0);
    }

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dca' = Dc - Sa.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t b0p_a8_extended;

    Face::p32ExtendPBWFromSBW(b0p_a8_extended, b0p_a8);
    Face::p32Subus255PBW(dst0p_20, a0p_20, b0p_a8_extended);
    Face::p32Subus255PBW_FillPBW1(dst0p_31, a0p_31, b0p_a8_extended);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // Same as SrcOver.
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeMultiply]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeMultiply : public CompositeExtPrgbVsPrgb<
  CompositeMultiply, RASTER_COMBINE_OP_MULTIPLY, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Dca.(Sca + 1 - Sa) + Sca.(1 - Da).
  // Da'  = Da.(Sa + 1 - Sa) + Sa.(1 - Sa).
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t a0p_a8;
    uint32_t b0p_a8;
    uint32_t t0p_20;
    uint32_t t0p_31;

    // Da, Sa.
    Face::p32ExtractPBW1(a0p_a8, a0p_31);
    Face::p32ExtractPBW1(b0p_a8, b0p_31);

    // Sca + 1 - Sa.
    Face::p32Add(t0p_20, b0p_20, 0x00FF00FF);
    Face::p32Add(t0p_31, b0p_31, 0x00FF00FF);
    Face::p32SubPBW_SBW_2x(t0p_20, t0p_20, t0p_31, t0p_31, b0p_a8);

    // Dca.(Sca + 1 - Sa).
    Face::p32MulPBW_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

    // Sca.(1 - Sa).
    Face::p32Negate255SBW(a0p_a8, a0p_a8);
    Face::p32MulPBW_SBW_2x(t0p_20, b0p_20, a0p_a8, t0p_31, b0p_31, a0p_a8);

    // Dca.(Sca + 1 - Sa) + Sca.(1 - Da).
    Face::p32Add_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);
    if (pack)
      Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
    else
      Face::p32Div255PBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31);
  }

  // Dca' = Sc.(Dca + 1 - Da).
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t a0p_a8;

    // Da.
    Face::p32ExtractPBW1(a0p_a8, a0p_31);

    // Dca + 1 - Da.
    Face::p32Add(dst0p_20, a0p_20, 0x00FF00FF);
    Face::p32Add(dst0p_31, a0p_31, 0x00FF00FF);
    Face::p32SubPBW_SBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31, a0p_a8);

    // Sc.(Dca + 1 - Da).
    if (pack)
    {
      Face::p32MulDiv255PBW_2x_Pack_20F1(dst0p_20, dst0p_20, b0p_20, dst0p_31, b0p_31);
    }
    else
    {
      Face::p32MulDiv255PBW(dst0p_20, dst0p_20, b0p_20);
      Face::p32MulDiv255PBW_FillPBW1(dst0p_31, dst0p_31, b0p_31);
    }
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dc' = Dc.(Sca + 1 - Sa).
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t b0p_a8;
    uint32_t t0p_20;
    uint32_t t0p_31;

    // Sa.
    Face::p32ExtractPBW1(b0p_a8, b0p_31);

    // Sca + 1 - Sa.
    Face::p32Add(t0p_20, b0p_20, 0x00FF00FF);
    Face::p32Add(t0p_31, b0p_31, 0x00FF00FF);
    Face::p32SubPBW_SBW_2x(t0p_20, t0p_20, t0p_31, t0p_31, b0p_a8);

    // Dc.(Sca + 1 - Sa).
    if (pack)
    {
      Face::p32MulDiv255PBW_2x_Pack_20F1(dst0p_20, dst0p_20, t0p_20, dst0p_31, t0p_31);
    }
    else
    {
      Face::p32MulDiv255PBW(dst0p_20, dst0p_20, t0p_20);
      Face::p32MulDiv255PBW_FillPBW1(dst0p_31, dst0p_31, t0p_31);
    }
  }

  // Dc' = Dc.Sc.
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    if (pack)
    {
      Face::p32MulDiv255PBW_2x_Pack_20F1(dst0p_20, a0p_20, b0p_20, a0p_31, b0p_31);
    }
    else
    {
      Face::p32MulDiv255PBW(dst0p_20, dst0p_20, a0p_20);
      Face::p32MulDiv255PBW_FillPBW1(dst0p_31, dst0p_31, b0p_31);
    }
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Dca + Sa.(1 - Da).
  // Da'  = Da + Sa.(1 - Da).
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t a0p_a8;
    
    // Sa.(1 - Da).
    Face::p32ExtractPBW1(a0p_a8, a0p_31);
    Face::p32Negate255SBW(a0p_a8, a0p_a8);
    Face::p32MulDiv255SBW(a0p_a8, a0p_a8, b0p_a8);

    // Dca + Sa.(1 - Da).
    if (pack)
    {
      Face::p32ExtendPBBFromSBB(a0p_a8, a0p_a8);
      Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
      Face::p32Add(dst0p_20, a0p_20, a0p_a8);
    }
    else
    {
      Face::p32ExtendPBWFromSBW(a0p_a8, a0p_a8);
      Face::p32Add_2x(dst0p_20, a0p_20, a0p_a8, dst0p_31, a0p_31, a0p_a8);
    }
  }

  // Dc' = Dc.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    if (pack)
      Face::p32PackPBB2031FromPBW(dst0p_20, a0p_20, a0p_31);
    else
      Face::p32Copy_2x(dst0p_20, a0p_20, dst0p_31, a0p_31);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // Same as SrcOver.
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeScreen]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeScreen : public CompositeExtPrgbVsPrgb<
  CompositeScreen, RASTER_COMBINE_OP_SCREEN, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Sca + Dca.(1 - Sca).
  // Da'  = Sa + Da.(1 - Sa).
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t binv0p_20;
    uint32_t binv0p_31;

    // Dca.(1 - Sca).
    Face::p32Negate255PBW_2x(binv0p_20, b0p_20, binv0p_31, b0p_31);
    Face::p32MulDiv255PBW_2x(dst0p_20, a0p_20, binv0p_20, dst0p_31, a0p_31, binv0p_31);

    // Sca + Dca.(1 - Sca).
    Face::p32Add_2x(dst0p_20, dst0p_20, b0p_20, dst0p_31, dst0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dca' = Sc + Dca.(1 - Sc).
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t binv0p_20;
    uint32_t binv0p_31;

    // Dca.(1 - Sc).
    Face::p32Negate255PBW_2x(binv0p_20, b0p_20, binv0p_31, b0p_31);
    Face::p32MulDiv255PBW(dst0p_20, a0p_20, binv0p_20);
    Face::p32MulDiv255PBW_ZeroPBW1(dst0p_31, a0p_31, binv0p_31);

    // Sc + Dca.(1 - Sc).
    Face::p32Add_2x(dst0p_20, dst0p_20, b0p_20, dst0p_31, dst0p_31, b0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dca' = Sca + Dc.(1 - Sca).
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t binv0p_20;
    uint32_t binv0p_31;

    // Dc.(1 - Sca).
    Face::p32Negate255PBW_2x(binv0p_20, b0p_20, binv0p_31, b0p_31);
    Face::p32MulDiv255PBW(dst0p_20, dst0p_20, binv0p_20);
    Face::p32MulDiv255PBW_ZeroPBW1(dst0p_31, dst0p_31, binv0p_31);

    // Sca + Dc.(1 - Sca).
    Face::p32Add_2x(dst0p_20, dst0p_20, b0p_20, dst0p_31, dst0p_31, b0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dc' = Sc + Dc(1 - Sc).
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Sa + Dca.(1 - Sa).
  // Da'  = Sa + Da.(1 - Sa).
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t a0p_a8;
    
    // Dca.(1 - Sa).
    Face::p32ExtractPBW1(a0p_a8, a0p_31);
    Face::p32Negate255SBW(a0p_a8, a0p_a8);
    Face::p32MulDiv255PBW_SBW_2x(dst0p_20, dst0p_20, a0p_a8, dst0p_31, dst0p_31, a0p_a8);

    // Sa + Dca.(1 - Sa).
    if (pack)
    {
      Face::p32ExtendPBBFromSBB(a0p_a8, a0p_a8);
      Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
      Face::p32Add(dst0p_20, dst0p_20, a0p_a8);
    }
    else
    {
      Face::p32ExtendPBWFromSBW(a0p_a8, a0p_a8);
      Face::p32Add_2x(dst0p_20, dst0p_20, a0p_a8, dst0p_31, dst0p_31, a0p_a8);
    }
  }

  // Dc' = Dc.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    if (pack)
      Face::p32PackPBB2031FromPBW(dst0p_20, a0p_20, a0p_31);
    else
      Face::p32Copy_2x(dst0p_20, a0p_20, dst0p_31, a0p_31);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // Same as SrcOver.
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeDarken]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeDarken : public CompositeExtPrgbVsPrgb<
  CompositeDarken, RASTER_COMBINE_OP_DARKEN, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa).
  // Da'  = min(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
  //      = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t d0p_20, d0p_31;
    uint32_t s0p_20, s0p_31;

    Face::p32ExtractPBW1(d0p_20, a0p_31);
    Face::p32ExtractPBW1(s0p_20, b0p_31);

    // (1 - Da).
    Face::p32Negate255SBW(d0p_20, d0p_20);
    // (1 - Sa).
    Face::p32Negate255SBW(s0p_20, s0p_20);

    Face::p32Copy(d0p_31, d0p_20);
    Face::p32Copy(s0p_31, s0p_20);

    // Sca.(1 - Da).
    Face::p32Mul_2x(d0p_20, b0p_20, d0p_20, d0p_31, b0p_31, d0p_31);
    // Dca.(1 - Sa).
    Face::p32Mul_2x(s0p_20, a0p_20, s0p_20, s0p_31, a0p_31, s0p_31);
    // Dca.(1 - Sa) + Sca.(1 - Da).
    Face::p32Add_2x(d0p_20, d0p_20, s0p_20, d0p_31, d0p_31, s0p_31);

    // Da.
    Face::p32ExtractPBW1(s0p_20, a0p_31);
    // Sa.
    Face::p32ExtractPBW1(s0p_31, b0p_31);

    // Dca.Sa.
    Face::p32Mul_2x(dst0p_20, a0p_20, s0p_31, dst0p_31, a0p_31, s0p_31);
    // Sca.Da.
    Face::p32Mul_2x(s0p_20, b0p_20, s0p_20, s0p_31, b0p_31, s0p_20);
    
    // min(Sca.Da, Dca.Sa).
    Face::p32MinPBW(dst0p_20, dst0p_20, s0p_20);
    Face::p32Min(dst0p_31, dst0p_31, s0p_31);
    // min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa).
    Face::p32Add_2x(dst0p_20, dst0p_20, d0p_20, dst0p_31, dst0p_31, d0p_31);

    if (pack)
      Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
    else
      Face::p32Div255PBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31);
  }

  // Dca' = min(Sc.Da, Dca) + Sc.(1 - Da).
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t d0p_20, d0p_31;
    uint32_t s0p_20, s0p_31;

    // Sc.Da.
    Face::p32ExtractPBW1(d0p_20, a0p_31);
    Face::p32MulDiv255PBW_SBW(s0p_20, b0p_20, d0p_20);
  
    Face::p32ExtractPBW0(s0p_31, b0p_31);
    Face::p32MulDiv255SBW(s0p_31, s0p_31, d0p_20);
  
    // Sc.(1 - Da).
    Face::p32Negate255SBW(d0p_20, d0p_20);
    Face::p32ExtractPBW0(d0p_31, b0p_31);
    Face::p32MulDiv255SBW(d0p_31, d0p_31, d0p_20);
    Face::p32MulDiv255PBW_SBW(d0p_20, b0p_20, d0p_20);
    
    // min(Sc.Da, Dca).
    Face::p32MinPBW(dst0p_20, a0p_20, s0p_20);
    Face::p32Min(dst0p_31, a0p_31, s0p_31);
    // min(Sc.Da, Dca) + Sc.(1 - Da).
    Face::p32Add_2x(dst0p_20, dst0p_20, d0p_20, dst0p_31, dst0p_31, d0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dc' = min(Sca, Dc.Sa) + Dc.(1 - Sa).
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t d0p_20, d0p_31;
    uint32_t sa;

    // Dc.(1 - Sa).
    Face::p32ExtractPBW1(sa, b0p_31);
    Face::p32Negate255SBW(d0p_20, sa);
    
    Face::p32ExtractPBW0(d0p_31, a0p_31);
    Face::p32MulDiv255SBW(d0p_31, d0p_31, d0p_20);
    Face::p32MulDiv255PBW_SBW(d0p_20, a0p_20, d0p_20);

    // Dc.Sa.
    Face::p32MulDiv255PBW_SBW(dst0p_20, a0p_20, sa);
    Face::p32ExtractPBW0(dst0p_31, a0p_31);
    Face::p32MulDiv255SBW(dst0p_31, dst0p_31, sa);

    // min(Sca, Dc.Sa).
    Face::p32ExtractPBW0(sa, b0p_31);
    Face::p32MinPBW(dst0p_20, dst0p_20, b0p_20);
    Face::p32Min(dst0p_31, dst0p_31, sa);

    // min(Sca, Dc.Sa) + Dc.(1 - Sa).
    Face::p32Add_2x(dst0p_20, dst0p_20, d0p_20, dst0p_31, dst0p_31, d0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dc' = min(Sc, Dc).
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32MinPBW(dst0p_20, a0p_20, b0p_20);
    Face::p32MinPBW_FillPBW1(dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32MinPBW(dst0p_20, a0p_20, b0p_20);
    Face::p32Min(dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Dca + Sa.(1 - Da).
  // Da'  = Da + Sa.(1 - Da).
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    // Same as CompositeMultiply.
    CompositeMultiply::prgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, pack);
  }

  // Dc' = Dc.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    if (pack)
      Face::p32PackPBB2031FromPBW(dst0p_20, a0p_20, a0p_31);
    else
      Face::p32Copy_2x(dst0p_20, a0p_20, dst0p_31, a0p_31);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // Same as SrcOver.
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeLighten]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeLighten : public CompositeExtPrgbVsPrgb<
  CompositeLighten, RASTER_COMBINE_OP_LIGHTEN, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = max(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa).
  // Da'  = max(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
  //      = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t d0p_20, d0p_31;
    uint32_t s0p_20, s0p_31;

    Face::p32ExtractPBW1(d0p_20, a0p_31);
    Face::p32ExtractPBW1(s0p_20, b0p_31);

    // (1 - Da).
    Face::p32Negate255SBW(d0p_20, d0p_20);
    // (1 - Sa).
    Face::p32Negate255SBW(s0p_20, s0p_20);

    Face::p32Copy(d0p_31, d0p_20);
    Face::p32Copy(s0p_31, s0p_20);

    // Sca.(1 - Da).
    Face::p32Mul_2x(d0p_20, b0p_20, d0p_20, d0p_31, b0p_31, d0p_31);
    // Dca.(1 - Sa).
    Face::p32Mul_2x(s0p_20, a0p_20, s0p_20, s0p_31, a0p_31, s0p_31);
    // Dca.(1 - Sa) + Sca.(1 - Da).
    Face::p32Add_2x(d0p_20, d0p_20, s0p_20, d0p_31, d0p_31, s0p_31);

    // Da.
    Face::p32ExtractPBW1(s0p_20, a0p_31);
    // Sa.
    Face::p32ExtractPBW1(s0p_31, b0p_31);

    // Dca.Sa.
    Face::p32Mul_2x(dst0p_20, a0p_20, s0p_31, dst0p_31, a0p_31, s0p_31);
    // Sca.Da.
    Face::p32Mul_2x(s0p_20, b0p_20, s0p_20, s0p_31, b0p_31, s0p_20);
    
    // max(Sca.Da, Dca.Sa).
    Face::p32MaxPBW(dst0p_20, dst0p_20, s0p_20);
    Face::p32Max(dst0p_31, dst0p_31, s0p_31);
    // max(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa).
    Face::p32Add_2x(dst0p_20, dst0p_20, d0p_20, dst0p_31, dst0p_31, d0p_31);

    if (pack)
      Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
    else
      Face::p32Div255PBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31);
  }

  // Dca' = max(Sc.Da, Dca) + Sc.(1 - Da).
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t d0p_20, d0p_31;
    uint32_t s0p_20, s0p_31;

    // Sc.Da.
    Face::p32ExtractPBW1(d0p_20, a0p_31);
    Face::p32MulDiv255PBW_SBW(s0p_20, b0p_20, d0p_20);
  
    Face::p32ExtractPBW0(s0p_31, b0p_31);
    Face::p32MulDiv255SBW(s0p_31, s0p_31, d0p_20);
  
    // Sc.(1 - Da).
    Face::p32Negate255SBW(d0p_20, d0p_20);
    Face::p32ExtractPBW0(d0p_31, b0p_31);
    Face::p32MulDiv255SBW(d0p_31, d0p_31, d0p_20);
    Face::p32MulDiv255PBW_SBW(d0p_20, b0p_20, d0p_20);
    
    // max(Sc.Da, Dca).
    Face::p32MaxPBW(dst0p_20, a0p_20, s0p_20);
    Face::p32Max(dst0p_31, a0p_31, s0p_31);
    // max(Sc.Da, Dca) + Sc.(1 - Da).
    Face::p32Add_2x(dst0p_20, dst0p_20, d0p_20, dst0p_31, dst0p_31, d0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dc' = max(Sca, Dc.Sa) + Dc.(1 - Sa).
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t d0p_20, d0p_31;
    uint32_t sa;

    // Dc.(1 - Sa).
    Face::p32ExtractPBW1(sa, b0p_31);
    Face::p32Negate255SBW(d0p_20, sa);
    
    Face::p32ExtractPBW0(d0p_31, a0p_31);
    Face::p32MulDiv255SBW(d0p_31, d0p_31, d0p_20);
    Face::p32MulDiv255PBW_SBW(d0p_20, a0p_20, d0p_20);

    // Dc.Sa.
    Face::p32MulDiv255PBW_SBW(dst0p_20, a0p_20, sa);
    Face::p32ExtractPBW0(dst0p_31, a0p_31);
    Face::p32MulDiv255SBW(dst0p_31, dst0p_31, sa);

    // max(Sca, Dc.Sa).
    Face::p32ExtractPBW0(sa, b0p_31);
    Face::p32MaxPBW(dst0p_20, dst0p_20, b0p_20);
    Face::p32Max(dst0p_31, dst0p_31, sa);

    // max(Sca, Dc.Sa) + Dc.(1 - Sa).
    Face::p32Add_2x(dst0p_20, dst0p_20, d0p_20, dst0p_31, dst0p_31, d0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dc' = max(Sc, Dc).
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32MaxPBW(dst0p_20, a0p_20, b0p_20);
    Face::p32MaxPBW_FillPBW1(dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32MaxPBW(dst0p_20, a0p_20, b0p_20);
    Face::p32Max(dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Sa + Dca.(1 - Sa).
  // Da'  = Sa + Da.(1 - Sa).
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    // Same as CompositeScreen.
    CompositeScreen::prgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, pack);
  }

  // Dc' = Sa + Dc.(1 - Sa).
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t t0;
    Face::p32Negate255SBW(t0, b0p_a8);

    if (pack)
    {
      Face::p32MulDiv255PBW_SBW_2x_Pack_20F1(dst0p_20, dst0p_20, t0, dst0p_31, t0);
      Face::p32Mul(t0, b0p_a8, 0x00010101);
      Face::p32Add(dst0p_20, dst0p_20, t0);
    }
    else
    {
      Face::p32MulDiv255PBW_SBW_2x(dst0p_20, dst0p_20, t0, dst0p_31, dst0p_31, t0);
      Face::p32Add(dst0p_31, dst0p_31, b0p_a8);
      Face::p32ExtendPBWFromSBW(t0, b0p_a8);
      Face::p32Add(dst0p_20, dst0p_20, t0);
    }
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // Same as SrcOver.
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeDifference]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeDifference : public CompositeExtPrgbVsPrgb<
  CompositeDifference, RASTER_COMBINE_OP_DIFFERENCE, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Dca + Sca - 2.min(Sca.Da, Dca.Sa).
  // Da'  = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;
    uint32_t u0p_20, u0p_31;

    uint32_t da;
    uint32_t sa;
    
    Face::p32ExtractPBW1(da, a0p_31);
    Face::p32ExtractPBW1(sa, b0p_31);

    // Dca.Sa.
    Face::p32Mul_2x(t0p_20, a0p_20, sa, t0p_31, a0p_31, sa);
    // Sca.Da.
    Face::p32Mul_2x(u0p_20, b0p_20, da, u0p_31, b0p_31, da);

    // Sa.Da.
    Face::p32MulDiv255SBW(da, da, sa);

    // 2.Min(Dca.Da, Dca.Sa).
    Face::p32MinPBW(t0p_20, t0p_20, u0p_20);
    Face::p32Min(t0p_31, t0p_31, u0p_31);
    Face::p32Div255PBW_LShift1(t0p_20, t0p_20);
    Face::p32Div255PBW(t0p_31, t0p_31);
    Face::p32LShiftBy1PBW0(t0p_31, t0p_31);

    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dca' = Dca + Sc - 2.min(Sc.Da, Dca).
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t u0p_20, u0p_31;
    uint32_t da;

    Face::p32ExtractPBW1(da, a0p_31);

    // Sc.Da.
    Face::p32ExtractPBW0(u0p_31, b0p_31);
    Face::p32MulDiv255PBW_SBW(u0p_20, b0p_20, da);
    Face::p32MulDiv255SBW(u0p_31, u0p_31, da);

    // Min(Sc.Da, Dca).
    Face::p32ExtractPBW0(da, a0p_31);
    Face::p32MinPBW(u0p_20, u0p_20, a0p_20);
    Face::p32Min(u0p_31, u0p_31, da);

    // 2.Min(Sc.Da, Dca).
    Face::p32LShiftBy1(u0p_20, u0p_20);
    Face::p32LShiftBy1(u0p_31, u0p_31);

    // Dca + Sc - 2.min(Sc.Da, Dca).
    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    Face::p32ZeroPWW1(dst0p_31, dst0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, u0p_20, dst0p_31, dst0p_31, u0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dc' = Dc + Sca - 2.min(Sca, Dc.Sa).
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;
    uint32_t sa;

    Face::p32ExtractPBW1(sa, b0p_31);

    // Dc.Sa.
    Face::p32ExtractPBW0(t0p_31, a0p_31);
    Face::p32MulDiv255PBW(t0p_20, a0p_20, sa);
    Face::p32MulDiv255SBW(t0p_31, t0p_31, sa);

    // Min(Sca, Dc.Sa).
    Face::p32ExtractPBW0(sa, b0p_31);
    Face::p32MinPBW(t0p_20, t0p_20, b0p_20);
    Face::p32Min(t0p_31, t0p_31, sa);

    // 2.Min(Sca, Dc.Sa).
    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1(t0p_31, t0p_31);

    // Dc + Sca - 2.min(Sca, Dc.Sa).
    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    Face::p32ZeroPWW1(dst0p_31, dst0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dc' = Dc + Sc - 2.min(Sc, Dc).
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;
    uint32_t u0p_31;

    // Min(Sc, Dc).
    Face::p32ExtractPBW0(t0p_31, a0p_31);
    Face::p32ExtractPBW0(u0p_31, b0p_31);

    Face::p32Min(t0p_31, t0p_31, u0p_31);
    Face::p32MinPBW(t0p_20, a0p_20, b0p_20);

    // 2.Min(Sc, Dc).
    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1(t0p_31, t0p_31);

    // Dc + Sc - 2.min(Sc, Dc).
    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    Face::p32ZeroPWW1(dst0p_31, dst0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;

    // Min(Sc, Dc).
    Face::p32MinPBW(t0p_20, a0p_20, b0p_20);
    Face::p32Min(t0p_31, a0p_31, b0p_31);

    // 2.Min(Sc, Dc).
    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1(t0p_31, t0p_31);

    // Dc + Sc - 2.min(Sc, Dc).
    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Dca + Sa - 2.min(Sa.Da, Dca.Sa)
  //      = Dca + Sa - 2.min(Da, Dca).Sa     || min(Da, Dca) == Dca.
  //      = Dca + Sa - 2.Dca.Sa
  // Da'  = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;
    uint32_t b0p;

    // 2.Dca.Sa || Sa.Da.
    Face::p32MulDiv255PBW_SBW_2x(t0p_20, a0p_20, b0p_a8, t0p_31, a0p_31, b0p_a8);
    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1PBW0(t0p_31, t0p_31);
    
    // Dca + Sa - 2.Dca.Sa.
    Face::p32ExtendPBWFromSBW(b0p, b0p_a8);
    Face::p32Add_2x(dst0p_20, a0p_20, b0p, dst0p_31, a0p_31, b0p);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, a0p_31, t0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dc' = Dc + Sa - 2.min(Sa, Dc.Sa)        || min(Sa, Dc.Sa) == Dc.
  //     = Dc + Sa - 2.Dc.Sa
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;
    uint32_t b0p;

    // 2.Dc.Sa.
    Face::p32MulDiv255PBW_SBW(t0p_20, a0p_20, b0p_a8);
    Face::p32ExtractPBW0(t0p_31, a0p_31);
    Face::p32MulDiv255SBW(t0p_31, t0p_31, b0p_a8);

    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1(t0p_31, t0p_31);

    // Dc + Sa - 2.Dc.Sa.
    Face::p32ExtendPBWFromSBW(b0p, b0p_a8);
    Face::p32Add_2x(dst0p_20, a0p_20, b0p, dst0p_31, a0p_31, b0p);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, a0p_31, t0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // Same as SrcOver.
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeExclusion]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeExclusion : public CompositeExtPrgbVsPrgb<
  CompositeExclusion, RASTER_COMBINE_OP_EXCLUSION, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = Dca + Sca - 2.Sca.Dca.
  // Da'  = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;

    // Dca.Sca.
    Face::p32MulDiv255PBW_2x(t0p_20, a0p_20, b0p_20, t0p_31, a0p_31, b0p_31);
    // Dca + Sca.
    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    // Dca + Sca - 2.Sca.Dca.
    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1PBW0(t0p_31, t0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dca' = Dca + Sc - 2.Sc.Dca.
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;

    // Dca.Sc.
    Face::p32ExtractPBW0(t0p_20, a0p_31);
    Face::p32ExtractPBW0(t0p_31, b0p_31);

    Face::p32MulDiv255SBW(t0p_31, t0p_31, t0p_20);
    Face::p32MulDiv255PBW(t0p_20, a0p_20, b0p_20);

    // Dca + Sc.
    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    // Dca + Sc - 2.Sc.Dca.
    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1(t0p_31, t0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dc' = Dc + Sca - 2.Sca.Dc.
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;

    // Dc.Sca.
    Face::p32ExtractPBW0(t0p_20, a0p_31);
    Face::p32ExtractPBW0(t0p_31, b0p_31);

    Face::p32MulDiv255SBW(t0p_31, t0p_31, t0p_20);
    Face::p32MulDiv255PBW(t0p_20, a0p_20, b0p_20);

    // Dc + Sca.
    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    // Dc + Sca - 2.Sca.Dc.
    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1(t0p_31, t0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dc' = Dc + Sc - 2.Sc.Dc.
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;

    // Dc.Sc.
    Face::p32ExtractPBW0(t0p_20, a0p_31);
    Face::p32ExtractPBW0(t0p_31, b0p_31);

    Face::p32MulDiv255SBW(t0p_31, t0p_31, t0p_20);
    Face::p32MulDiv255PBW(t0p_20, a0p_20, b0p_20);

    // Dc + Sc.
    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    // Dc + Sc - 2.Sc.Dc.
    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1(t0p_31, t0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t t0p_20, t0p_31;

    // Dc.Sc.
    Face::p32MulDiv255PBW(t0p_20, a0p_20, b0p_20);
    Face::p32MulDiv255SBW(t0p_31, a0p_31, b0p_20);

    // Dc + Sc.
    Face::p32Add_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    // Dc + Sc - 2.Sc.Dc.
    Face::p32LShiftBy1(t0p_20, t0p_20);
    Face::p32LShiftBy1(t0p_31, t0p_31);
    Face::p32Sub_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  // Dca' = Dca + Sa - 2.Sa.Dca.
  // Da'  = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    // Same as Difference.
    prgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, pack);
  }

  // Dc' = Dc + Sa - 2.Sa.Dc.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    // Same as Difference.
    CompositeDifference::xrgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, pack);
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // Same as SrcOver.
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};
















#if 0
// ============================================================================
// [Fog::RasterOps_C - CompositeColorDodge]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeColorDodge : public CompositeExtPrgbVsPrgb<
  CompositeColorDodge, RASTER_COMBINE_OP_COLOR_DODGE, RASTER_PRGB_PREPARE_NONE>
{
  // ==========================================================================
  // [Func - Prepare]
  // ==========================================================================

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - Pixel32]
  // ==========================================================================

  // Dca' = if (Sca.Da + Dca.Sa >= Sa.Da)
  //          Sa.Da + Sca.(1 - Da) + Dca.(1 - Sa).
  //        else
  //          Dca.Sa.Sa/(Sa - Sca) + Sca.(1 - Da) + Dca.(1 - Sa).
  // Da'  = Sa + Da - Sa.Da
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t da, dr, dg, db;
    uint32_t sa,

    uint32_t t0p_20;
    uint32_t t0p_31;

    Face::p32ExtractPBW1(da, a0p_31);
    Face::p32ExtractPBW1(sa, b0p_31);

    Face::p32ExtractPBW1(dr, a0p_20);
    Face::p32ExtractPBW0(dg, a0p_31);
    Face::p32ExtractPBW0(db, a0p_20);

    if (pack)
      Face::p32Pack(dst0p_20, t0p_20, t0p_31);
    else
      Face::p32Copy_2x(dst0p_20, t0p_20, dst0p_31, t0p_31);
  }

  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // TODO:
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    // TODO:
  }

  // Dc' = max(Sc, Dc).
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32MaxPBW(dst0p_20, a0p_20, b0p_20);
    Face::p32MaxPBW_FillPBW1(dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // ==========================================================================
  // [Func - Pixel32 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    // TODO:
  }

  // Dc' = Sa + Dc.(1 - Sa).
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    // TODO:
  }

  // ==========================================================================
  // [Func - PixelA8 - Op - PixelA8]
  // ==========================================================================

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    // Same as SrcOver.
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};
#endif












} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITEEXT_P_H
