// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_SSE2_COMPOSITESRC_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_SSE2_COMPOSITESRC_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_SSE2/CompositeBase_p.h>

namespace Fog {
namespace RasterOps_SSE2 {

// ============================================================================
// [Fog::RasterOps_SSE2 - CompositeSrc]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeSrc
{
  enum { COMBINE_FLAGS = RASTER_COMBINE_OP_SRC };

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    __m128i sro0xmm;
    Acc::m128iLoad4(sro0xmm, &src->prgb32);

    if (w <= 4)
    {
      Acc::m128iStore4(dst +  0, sro0xmm); if (--w == 0) goto _C_Opaque_End;
      Acc::m128iStore4(dst +  4, sro0xmm); if (--w == 0) goto _C_Opaque_End;
      Acc::m128iStore4(dst +  8, sro0xmm); if (--w == 0) goto _C_Opaque_End;
      Acc::m128iStore4(dst + 12, sro0xmm);
    }
    else
    {
      uint8_t* mark = dst;

      Acc::m128iExtendPI32FromSI32(sro0xmm, sro0xmm);
      Acc::m128iStore16u(dst, sro0xmm);

      dst = (uint8_t*)( (uintptr_t(dst) + 16) & ~(uintptr_t)15 );
      w -= (int)(intptr_t)(dst - mark) >> 2;

      while ((w -= 16) >= 0)
      {
        Acc::m128iStore16a(dst +  0, sro0xmm);
        Acc::m128iStore16a(dst + 16, sro0xmm);
        Acc::m128iStore16a(dst + 32, sro0xmm);
        Acc::m128iStore16a(dst + 48, sro0xmm);
        dst += 64;
      }

      if ((w += 16) < 4) goto _C_Opaque_Tail;
      Acc::m128iStore16a(dst, sro0xmm);
      dst += 16;

      if ((w -= 4) < 4) goto _C_Opaque_Tail;
      Acc::m128iStore16a(dst, sro0xmm);
      dst += 16;

      if ((w -= 4) < 4) goto _C_Opaque_Tail;
      Acc::m128iStore16a(dst, sro0xmm);
      dst += 16;
      w -= 4;

_C_Opaque_Tail:
      if (w == 0) goto _C_Opaque_End;
      Acc::m128iStore4(dst + 0, sro0xmm);

      if (--w == 0) goto _C_Opaque_End;
      Acc::m128iStore4(dst + 4, sro0xmm);

      if (--w == 0) goto _C_Opaque_End;
      Acc::m128iStore4(dst + 8, sro0xmm);
    }
_C_Opaque_End:
    ;
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    __m128i sro0xmm;
    __m128i sru0xmm;

    Acc::m128iLoad4(sro0xmm, &src->prgb32);
    Acc::m128iExtendPI32FromSI32(sro0xmm, sro0xmm);
    Acc::m128iUnpackPI16FromPI8Lo(sru0xmm, sro0xmm);

    FOG_CBLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    FOG_CBLIT_SPAN8_C_OPAQUE()
    {
      if (w <= 4)
      {
        Acc::m128iStore4(dst +  0, sro0xmm); if (--w == 0) goto _C_Opaque_End;
        Acc::m128iStore4(dst +  4, sro0xmm); if (--w == 0) goto _C_Opaque_End;
        Acc::m128iStore4(dst +  8, sro0xmm); if (--w == 0) goto _C_Opaque_End;
        Acc::m128iStore4(dst + 12, sro0xmm);
      }
      else
      {
        uint8_t* mark = dst;
        Acc::m128iStore16u(dst, sro0xmm);

        dst = (uint8_t*)( (uintptr_t(dst) + 16) & ~(uintptr_t)15 );
        w -= (int)(intptr_t)(dst - mark) >> 2;

        while ((w -= 16) >= 0)
        {
          Acc::m128iStore16a(dst +  0, sro0xmm);
          Acc::m128iStore16a(dst + 16, sro0xmm);
          Acc::m128iStore16a(dst + 32, sro0xmm);
          Acc::m128iStore16a(dst + 48, sro0xmm);
          dst += 64;
        }

        if ((w += 16) < 4) goto _C_Opaque_Tail;
        Acc::m128iStore16a(dst, sro0xmm);
        dst += 16;

        if ((w -= 4) < 4) goto _C_Opaque_Tail;
        Acc::m128iStore16a(dst, sro0xmm);
        dst += 16;

        if ((w -= 4) < 4) goto _C_Opaque_Tail;
        Acc::m128iStore16a(dst, sro0xmm);
        dst += 16;
        w -= 4;

  _C_Opaque_Tail:
        if (w == 0) goto _C_Opaque_End;
        Acc::m128iStore4(dst + 0, sro0xmm);

        if (--w == 0) goto _C_Opaque_End;
        Acc::m128iStore4(dst + 4, sro0xmm);

        if (--w == 0) goto _C_Opaque_End;
        Acc::m128iStore4(dst + 8, sro0xmm);
      }
  _C_Opaque_End:
      ;
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    FOG_CBLIT_SPAN8_C_MASK()
    {
      FOG_BLIT_LOOP_32x8_SSE2_INIT()

      __m128i src0xmm;
      __m128i msk0xmm;

      Acc::m128iCvtSI128FromSI(msk0xmm, msk0);
      Acc::m128iExtendPI16FromSI16(msk0xmm, msk0xmm);

      Acc::m128iMulDiv256PI16(src0xmm, sru0xmm, msk0xmm);
      Acc::m128iNegate256PI16(msk0xmm, msk0xmm);
      Acc::m128iPackPU8FromPU16(src0xmm, src0xmm);
      Acc::m128iLShiftPU16<8>(msk0xmm, msk0xmm);

      FOG_BLIT_LOOP_32x8_SSE2_ONE_BEGIN(C_Mask)
        __m128i dst0xmm;

        Acc::m128iLoad4(dst0xmm, dst);
        Acc::m128iUnpackPI16FromPI8Lo(dst0xmm, dst0xmm);
        Acc::m128iMulHiPU16(dst0xmm, dst0xmm, msk0xmm);
        Acc::m128iPackPU8FromPU16(dst0xmm, dst0xmm);
        Acc::m128iAddPI16(dst0xmm, dst0xmm, src0xmm);
        Acc::m128iStore4(dst, dst0xmm);

        dst += 4;
      FOG_BLIT_LOOP_32x8_SSE2_ONE_END(C_Mask)

      FOG_BLIT_LOOP_32x8_SSE2_TWO_BEGIN(C_Mask)
        __m128i dst0xmm;

        Acc::m128iLoad8(dst0xmm, dst);
        Acc::m128iUnpackPI16FromPI8Lo(dst0xmm, dst0xmm);
        Acc::m128iMulHiPU16(dst0xmm, dst0xmm, msk0xmm);
        Acc::m128iPackPU8FromPU16(dst0xmm, dst0xmm);
        Acc::m128iAddPI16(dst0xmm, dst0xmm, src0xmm);
        Acc::m128iStore8(dst, dst0xmm);

        dst += 8;
      FOG_BLIT_LOOP_32x8_SSE2_TWO_END(C_Mask)

      FOG_BLIT_LOOP_32x8_SSE2_MAIN_BEGIN(C_Mask)
        __m128i dst0xmm, dst1xmm;
        __m128i dst2xmm, dst3xmm;

        Acc::m128iLoad16a(dst0xmm, dst +  0);
        Acc::m128iLoad16a(dst2xmm, dst + 16);

        Acc::m128iUnpackPI16FromPI8Hi(dst1xmm, dst0xmm);
        Acc::m128iUnpackPI16FromPI8Lo(dst0xmm, dst0xmm);
        Acc::m128iUnpackPI16FromPI8Hi(dst3xmm, dst2xmm);
        Acc::m128iMulHiPU16(dst1xmm, dst1xmm, msk0xmm);
        Acc::m128iUnpackPI16FromPI8Lo(dst2xmm, dst2xmm);
        Acc::m128iMulHiPU16(dst0xmm, dst0xmm, msk0xmm);

        Acc::m128iMulHiPU16(dst2xmm, dst2xmm, msk0xmm);
        Acc::m128iMulHiPU16(dst3xmm, dst3xmm, msk0xmm);
        Acc::m128iPackPU8FromPU16(dst0xmm, dst0xmm, dst1xmm);
        Acc::m128iPackPU8FromPU16(dst2xmm, dst2xmm, dst3xmm);

        Acc::m128iAddPI16(dst0xmm, dst0xmm, src0xmm);
        Acc::m128iAddPI16(dst2xmm, dst2xmm, src0xmm);
        Acc::m128iStore16a(dst +  0, dst0xmm);
        Acc::m128iStore16a(dst + 16, dst2xmm);

        dst += 32;
      FOG_BLIT_LOOP_32x8_SSE2_MAIN_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    FOG_CBLIT_SPAN8_A8_GLYPH()
    {
      FOG_BLIT_LOOP_32x4_INIT()

      FOG_BLIT_LOOP_32x4_SMALL_BEGIN(A8_Glyph)
        __m128i dst0xmm;
        __m128i msk0xmm;
        uint32_t msk0p;

        Acc::p32Load1b(msk0p, msk);
        if (msk0p == 0x00) goto _A8_Glyph_Small_Skip;

        msk0p ^= 0xFF;
        if (msk0p == 0x00) goto _A8_Glyph_Small_Fill;

        Acc::m128iCvtSI128FromSI(msk0xmm, msk0p);
        Acc::m128iLoad4(dst0xmm, dst);
        Acc::m128iExtendPI16FromSI16Lo(msk0xmm, msk0xmm);
        Acc::m128iUnpackPI16FromPI8Lo(dst0xmm, dst0xmm);
        Acc::m128iCvt256From255PI16(msk0xmm, msk0xmm);

        Acc::m128iMulLoPI16(dst0xmm, dst0xmm, msk0xmm);
        Acc::m128iNegate256PI16(msk0xmm, msk0xmm);
        Acc::m128iMulLoPI16(msk0xmm, msk0xmm, sru0xmm);
        Acc::m128iAddPI16(dst0xmm, dst0xmm, msk0xmm);

        Acc::m128iRShiftPU16<8>(dst0xmm, dst0xmm);
        Acc::m128iPackPU8FromPU16(dst0xmm, dst0xmm);
        Acc::m128iStore4(dst, dst0xmm);

_A8_Glyph_Small_Skip:
        dst += 4;
        msk += 1;
        FOG_BLIT_LOOP_32x4_SMALL_CONTINUE(A8_Glyph)

_A8_Glyph_Small_Fill:
        Acc::m128iStore4(dst, sro0xmm);

        dst += 4;
        msk += 1;
      FOG_BLIT_LOOP_32x4_SMALL_END(A8_Glyph)

      FOG_BLIT_LOOP_32x4_MAIN_BEGIN(A8_Glyph)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        uint32_t msk0p;

        Acc::p32Load4u(msk0p, msk);
        if (msk0p == 0x00000000) goto _A8_Glyph_Main_Skip;

        msk0p = ~msk0p;
        if (msk0p == 0x00000000) goto _A8_Glyph_Main_Fill;

        Acc::m128iCvtSI128FromSI(msk0xmm, msk0p);
        Acc::m128iLoad16a(dst0xmm, dst);

        Acc::m128iUnpackPI16FromPI8Lo(msk0xmm, msk0xmm);
        Acc::m128iUnpackPI16FromPI8Hi(dst1xmm, dst0xmm);
        Acc::m128iCvt256From255PI16(msk0xmm, msk0xmm);
        Acc::m128iUnpackPI16FromPI8Lo(dst0xmm, dst0xmm);
        Acc::m128iUnpackMask4PI16(msk0xmm, msk1xmm, msk0xmm);

        Acc::m128iMulLoPI16_2x(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        Acc::m128iNegate256PI16_2x(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        Acc::m128iMulLoPI16_2x(msk0xmm, msk0xmm, sru0xmm, msk1xmm, msk1xmm, sru0xmm);
        Acc::m128iAddPI16_2x(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        Acc::m128iRShiftPU16<8>(dst0xmm, dst0xmm);
        Acc::m128iRShiftPU16<8>(dst1xmm, dst1xmm);
        Acc::m128iPackPU8FromPU16(dst0xmm, dst0xmm, dst1xmm);
        Acc::m128iStore16a(dst, dst0xmm);

_A8_Glyph_Main_Skip:
        dst += 16;
        msk += 4;
        FOG_BLIT_LOOP_32x4_MAIN_CONTINUE(A8_Glyph)

_A8_Glyph_Main_Fill:
        Acc::m128iStore16a(dst, sro0xmm);

        dst += 16;
        msk += 4;
      FOG_BLIT_LOOP_32x4_MAIN_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    FOG_CBLIT_SPAN8_A8_EXTRA()
    {
      FOG_BLIT_LOOP_32x8_SSE2_INIT()

      FOG_BLIT_LOOP_32x8_SSE2_ONE_BEGIN(A8_Extra)
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i minv0xmm;

        Acc::m128iLoad2(msk0xmm, msk);
        Acc::m128iLoad4(dst0xmm, dst);

        Acc::m128iExtendPI16FromSI16Lo(msk0xmm, msk0xmm);
        Acc::m128iUnpackPI16FromPI8Lo(dst0xmm, dst0xmm);
        Acc::m128iNegate256PI16(minv0xmm, msk0xmm);

        Acc::m128iMulLoPI16(msk0xmm, msk0xmm, sru0xmm);
        Acc::m128iMulLoPI16(dst0xmm, dst0xmm, minv0xmm);
        Acc::m128iAddPI16(dst0xmm, dst0xmm, msk0xmm);

        Acc::m128iRShiftPU16<8>(dst0xmm, dst0xmm);
        Acc::m128iPackPU8FromPU16(dst0xmm, dst0xmm);
        Acc::m128iStore4(dst, dst0xmm);

        dst += 4;
        msk += 2;
      FOG_BLIT_LOOP_32x8_SSE2_ONE_END(A8_Extra)

      FOG_BLIT_LOOP_32x8_SSE2_TWO_BEGIN(A8_Extra)
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i minv0xmm;

        Acc::m128iLoad4(msk0xmm, msk);
        Acc::m128iLoad8(dst0xmm, dst);

        Acc::m128iUnpackMask2PI16(msk0xmm, msk0xmm);
        Acc::m128iUnpackPI16FromPI8Lo(dst0xmm, dst0xmm);
        Acc::m128iNegate256PI16(minv0xmm, msk0xmm);

        Acc::m128iMulLoPI16(msk0xmm, msk0xmm, sru0xmm);
        Acc::m128iMulLoPI16(dst0xmm, dst0xmm, minv0xmm);
        Acc::m128iAddPI16(dst0xmm, dst0xmm, msk0xmm);

        Acc::m128iRShiftPU16<8>(dst0xmm, dst0xmm);
        Acc::m128iPackPU8FromPU16(dst0xmm, dst0xmm);
        Acc::m128iStore8(dst, dst0xmm);

        dst += 8;
        msk += 4;
      FOG_BLIT_LOOP_32x8_SSE2_TWO_END(A8_Extra)

      FOG_BLIT_LOOP_32x8_SSE2_MAIN_BEGIN(A8_Extra)
        __m128i dst0xmm, dst1xmm, dst2xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i minv0xmm, minv1xmm;

        Acc::m128iLoad16a(dst0xmm, dst +  0);
        Acc::m128iLoad8(msk0xmm, msk);

        Acc::m128iUnpackPI16FromPI8Hi(dst1xmm, dst0xmm);
        Acc::m128iUnpackMask4PI16(msk0xmm, msk1xmm, msk0xmm);
        Acc::m128iUnpackPI16FromPI8Lo(dst0xmm, dst0xmm);
        Acc::m128iNegate256PI16_2x(minv0xmm, msk0xmm, minv1xmm, msk1xmm);
        Acc::m128iMulLoPI16_2x(msk0xmm, msk0xmm, sru0xmm, msk1xmm, msk1xmm, sru0xmm);
        Acc::m128iMulLoPI16_2x(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv1xmm);

        Acc::m128iLoad16a(dst2xmm, dst + 16);
        Acc::m128iAddPI16(dst0xmm, dst0xmm, msk0xmm);
        Acc::m128iLoad8(msk0xmm, msk + 8);
        Acc::m128iAddPI16(dst1xmm, dst1xmm, msk1xmm);
       
        Acc::m128iRShiftPU16<8>(dst0xmm, dst0xmm);
        Acc::m128iUnpackMask4PI16(msk0xmm, msk1xmm, msk0xmm);
        Acc::m128iRShiftPU16<8>(dst1xmm, dst1xmm);
        Acc::m128iNegate256PI16_2x(minv0xmm, msk0xmm, minv1xmm, msk1xmm);
        Acc::m128iPackPU8FromPU16(dst0xmm, dst0xmm, dst1xmm);

        Acc::m128iUnpackPI16FromPI8Hi(dst1xmm, dst2xmm);
        Acc::m128iUnpackPI16FromPI8Lo(dst2xmm, dst2xmm);
        Acc::m128iMulLoPI16_2x(msk0xmm, msk0xmm, sru0xmm, msk1xmm, msk1xmm, sru0xmm);
        Acc::m128iMulLoPI16_2x(dst2xmm, dst2xmm, minv0xmm, dst1xmm, dst1xmm, minv1xmm);
        Acc::m128iAddPI16(dst2xmm, dst2xmm, msk0xmm);
        Acc::m128iAddPI16(dst1xmm, dst1xmm, msk1xmm);
       
        Acc::m128iRShiftPU16<8>(dst2xmm, dst2xmm);
        Acc::m128iRShiftPU16<8>(dst1xmm, dst1xmm);
        Acc::m128iStore16a(dst +  0, dst0xmm);

        Acc::m128iPackPU8FromPU16(dst2xmm, dst2xmm, dst1xmm);
        Acc::m128iStore16a(dst + 16, dst2xmm);

        dst += 32;
        msk += 16;
      FOG_BLIT_LOOP_32x8_SSE2_MAIN_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    FOG_CBLIT_SPAN8_ARGB32_GLYPH()
    {
      // TODO:
    }

    FOG_CBLIT_SPAN8_END()
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
/*
  static void FOG_FASTCALL prgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    FOG_VBLIT_SPAN8_BEGIN(4)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    FOG_VBLIT_SPAN8_C_OPAQUE()
    {
      FOG_BLIT_LOOP_32x1_INIT()

      FOG_BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t src0p;

        Acc::p32Load4a(src0p, src);
        Acc::p32Store4a(dst, src0p);

        dst += 4;
        src += 4;
      FOG_BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    FOG_VBLIT_SPAN8_C_MASK()
    {
      uint32_t msk0p;
      uint32_t minv0p;

      Acc::p32Copy(msk0p, msk0);
      Acc::p32Negate256SBW(minv0p, msk0p);

      FOG_BLIT_LOOP_32x1_INIT()

      FOG_BLIT_LOOP_32x1_BEGIN(C_Mask)
        uint32_t dst0p;
        uint32_t src0p;

        Acc::p32Load4a(src0p, src);
        Acc::p32Load4a(dst0p, dst);

        Acc::p32Lerp256PBB_SBW(dst0p, dst0p, src0p, minv0p, msk0p);
        Acc::p32Store4a(dst, dst0p);

        dst += 4;
        src += 4;
      FOG_BLIT_LOOP_32x1_END(C_Mask)
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------

    FOG_VBLIT_SPAN8_A8_GLYPH()
    {
      FOG_BLIT_LOOP_32x1_INIT()

      FOG_BLIT_LOOP_32x1_BEGIN(A8_Glyph)
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
      FOG_BLIT_LOOP_32x1_END(A8_Glyph)
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    FOG_VBLIT_SPAN8_A8_EXTRA()
    {
      FOG_BLIT_LOOP_32x1_INIT()

      FOG_BLIT_LOOP_32x1_BEGIN(A8_Extra)
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
      FOG_BLIT_LOOP_32x1_END(A8_Extra)
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    FOG_VBLIT_SPAN8_ARGB32_GLYPH()
    {
      FOG_BLIT_LOOP_32x1_INIT()

      FOG_BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
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
      FOG_BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    FOG_VBLIT_SPAN8_END()
  }
*/
};

} // RasterOps_SSE2 namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_SSE2_COMPOSITESRC_P_H
