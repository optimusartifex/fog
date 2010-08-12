// [Fog-Graphics Library - Private API]
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
// [Fog::RasterEngine::SSE2 - Composite - Src]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeSrcSSE2
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC };

  // --------------------------------------------------------------------------
  // [CompositeSrcSSE2 - PRGB32 - CBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    __m128i src0orig;

    sse2_load4(src0orig, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);

    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x16_ALIGN_BEGIN(cMaskOpaque)
      sse2_store4(dst, src0orig);
      dst += 4;
    SSE2_BLIT_32x16_ALIGN_END(cMaskOpaque)

    SSE2_BLIT_32x16_MAIN_BEGIN(cMaskOpaque)
      sse2_store16a(dst, src0orig);
      sse2_store16a(dst + 16, src0orig);
      sse2_store16a(dst + 32, src0orig);
      sse2_store16a(dst + 48, src0orig);
      dst += 64;
    SSE2_BLIT_32x16_MAIN_END(cMaskOpaque)

    SSE2_BLIT_32x16_TAIL_4(cMaskOpaque, {
      sse2_store16a(dst, src0orig);
      dst += 16;
    });

    SSE2_BLIT_32x16_TAIL_1(cMaskOpaque, {
      sse2_store4(dst, src0orig);
      dst += 4;
    });
  }

  static void FOG_FASTCALL prgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x2W(src0xmm, src0orig);

    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x16_ALIGN_BEGIN(cMaskOpaque)
        sse2_store4(dst, src0orig);
        dst += 4;
      SSE2_BLIT_32x16_ALIGN_END(cMaskOpaque)

      SSE2_BLIT_32x16_MAIN_BEGIN(cMaskOpaque)
        sse2_store16a(dst, src0orig);
        sse2_store16a(dst + 16, src0orig);
        sse2_store16a(dst + 32, src0orig);
        sse2_store16a(dst + 48, src0orig);
        dst += 64;
      SSE2_BLIT_32x16_MAIN_END(cMaskOpaque)

      SSE2_BLIT_32x16_TAIL_4(cMaskOpaque, {
        sse2_store16a(dst, src0orig);
        dst += 16;
      });

      SSE2_BLIT_32x16_TAIL_1(cMaskOpaque, {
        sse2_store4(dst, src0orig);
        dst += 4;
      });
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i msrc0xmm;
      __m128i minv0xmm;

      sse2_expand_mask_1x1W(msrc0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msrc0xmm, msrc0xmm);

      sse2_negate_1x2W(minv0xmm, msrc0xmm);
      sse2_muldiv255_1x1W(msrc0xmm, msrc0xmm, src0xmm);
      sse2_pack_1x1W(msrc0xmm, msrc0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(dst0xmm, dst0xmm, msrc0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i dst0xmm, dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, msrc0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = Face::u32Negate255(READ_8(msk));

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load4(msk0xmm, msk);
        sse2_load16a(dst0xmm, dst);
        sse2_negate_1x4B(msk0xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        dst0xmm = src0orig;
        msk0 = Face::u32Negate255(msk0);
        if (msk0 == 0x00) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

vMaskAlphaSparseFill1:
        sse2_store4(dst, dst0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        dst0xmm = src0orig;
        msk0 = ~msk0;
        if (msk0 == 0x00000000) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

vMaskAlphaSparseFill4:
        sse2_store16a(dst, dst0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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
  // [CompositeSrcSSE2 - PRGB32 - VBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_prgb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i src0xmm;

        sse2_load4(src0xmm, src);
        sse2_store4(dst, src0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;

        sse2_load16u(src0xmm, src);
        sse2_store16a(dst, src0xmm);

        dst += 16;
        src += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i m0xmm, im0xmm;
      sse2_expand_mask_1x1W(m0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
      sse2_negate_1x2W(im0xmm, m0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_load4(src0xmm, src);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, im0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load16u(src0xmm, src);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, im0xmm, dst1xmm, dst1xmm, im0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load4(src0xmm, src);
        if (msk0 == 0xFF) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);

vMaskAlphaSparseFill1:
        sse2_store4(dst, src0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16u(src0xmm, src);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparseFill4:
        sse2_store16a(dst, src0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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

  // -------------------------------------------------------------------------
  // [CompositeSrcSSE2 - PRGB32 - VBlit - ARGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_argb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i src0xmm;

        sse2_load4(src0xmm, src);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);
        sse2_store4(dst, src0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm, src1xmm;

        sse2_load16u(src0xmm, src);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
        sse2_store16a(dst, src0xmm);

        dst += 16;
        src += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i m0xmm, im0xmm;
      sse2_expand_mask_1x1W(m0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
      sse2_negate_1x2W(im0xmm, m0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, im0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, im0xmm, dst1xmm, dst1xmm, im0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load16u(src0xmm, src);
        sse2_load4(msk0xmm, msk);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load4(src0xmm, src);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        if (msk0 == 0xFF) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);

vMaskAlphaSparseFill1:
        sse2_pack_1x1W(src0xmm, src0xmm);
        sse2_store4(dst, src0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16u(src0xmm, src);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);

vMaskAlphaSparseFill4:
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
        sse2_store16a(dst, src0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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
  // [CompositeSrcSSE2 - PRGB32 - VBlit - XRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_xrgb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x16_ALIGN_BEGIN(cMaskOpaque)
        __m128i src0xmm;

        sse2_load4(src0xmm, src);
        sse2_fill_alpha_1x1B(src0xmm);
        sse2_store4(dst, src0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x16_ALIGN_END(cMaskOpaque)

      SSE2_BLIT_32x16_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm, src1xmm;
        __m128i src2xmm, src3xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16u(src1xmm, src + 16);
        sse2_load16u(src2xmm, src + 32);
        sse2_load16u(src3xmm, src + 48);

        sse2_fill_alpha_1x4B(src0xmm);
        sse2_fill_alpha_1x4B(src1xmm);
        sse2_fill_alpha_1x4B(src2xmm);
        sse2_fill_alpha_1x4B(src3xmm);

        sse2_store16a(dst, src0xmm);
        sse2_store16a(dst + 16, src1xmm);
        sse2_store16a(dst + 32, src2xmm);
        sse2_store16a(dst + 48, src3xmm);

        dst += 64;
        src += 64;
      SSE2_BLIT_32x16_MAIN_END(cMaskOpaque)

      SSE2_BLIT_32x16_TAIL_4(cMaskOpaque, {
        __m128i src0xmm;

        sse2_load16u(src0xmm, src);
        sse2_fill_alpha_1x4B(src0xmm);
        sse2_store16a(dst, src0xmm);

        dst += 16;
        src += 16;
      });

      SSE2_BLIT_32x16_TAIL_1(cMaskOpaque, {
        __m128i src0xmm;

        sse2_load4(src0xmm, src);
        sse2_fill_alpha_1x1B(src0xmm);
        sse2_store4(dst, src0xmm);

        dst += 4;
        src += 4;
      });
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i m0xmm, im0xmm;
      sse2_expand_mask_1x1W(m0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
      sse2_negate_1x2W(im0xmm, m0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, im0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_fill_alpha_1x1B(src0xmm);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, im0xmm, dst1xmm, dst1xmm, im0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);

        sse2_fill_alpha_1x4B(src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load4(src0xmm, src);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_fill_alpha_1x1B(src0xmm);
        if (msk0 == 0xFF) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);

vMaskAlphaSparseFill1:
        sse2_store4(dst, src0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16u(src0xmm, src);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_fill_alpha_1x4B(src0xmm);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparseFill4:
        sse2_store16a(dst, src0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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
  // [CompositeSrcSSE2 - PRGB32 - VBlit - A8]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_a8_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        ((uint32_t*)dst)[0] = (uint32_t)src[0] << 24;
        dst += 4;
        src += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i m0xmm, im0xmm;
      sse2_expand_mask_1x1W(m0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
      sse2_negate_1x2W(im0xmm, m0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        src0xmm = _mm_cvtsi32_si128(READ_8(src));

        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        src0xmm = _mm_slli_si128(src0xmm, 6);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, im0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i src0xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load4(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
        src0xmm = _mm_unpacklo_epi16(src0xmm, _mm_setzero_si128());
        src0xmm = _mm_slli_epi32(src0xmm, 24);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, im0xmm, dst1xmm, dst1xmm, im0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, src0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 4;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        uint32_t src0 = Face::u32MulDiv255(READ_8(src), msk0);

        msk0 = Face::u32Negate255(msk0);
        sse2_load4(dst0xmm, dst);

        src0xmm = _mm_cvtsi32_si128(src0 << 24);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(dst0xmm, dst0xmm, src0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(src0xmm, src);
        sse2_load4(msk0xmm, msk);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        src0xmm = _mm_unpacklo_epi16(src0xmm, _mm_setzero_si128());
        src0xmm = _mm_slli_epi32(src0xmm, 24);

        sse2_expand_mask_2x2W_from_1x1W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, src0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 4;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t src0;
        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        src0 = Face::u32MulDiv255(READ_8(src), msk0);
        msk0 = Face::u32Negate255(msk0);
        src0xmm = _mm_cvtsi32_si128(src0 << 24);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(src0xmm, src0xmm, dst0xmm);
        sse2_store4(dst, src0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load4(src0xmm, src);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        src0xmm = _mm_unpacklo_epi16(src0xmm, _mm_setzero_si128());
        src0xmm = _mm_slli_epi32(src0xmm, 24);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W_from_1x1W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(src0xmm, src0xmm, dst0xmm);

vMaskAlphaSparseFill4:
        sse2_store16a(dst, src0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        src += 4;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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

  // -------------------------------------------------------------------------
  // [CompositeSrcSSE2 - PRGB32 - VBlit - I8]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_i8_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    const uint32_t* pal = 
      reinterpret_cast<const uint32_t*>(closure->srcPalette) + Palette::INDEX_PRGB32;

    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        __m128i src0xmm;

        sse2_load_1xI8(src0xmm, src, pal);
        sse2_store4(dst, src0xmm);

        dst += 4;
        src += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i m0xmm, im0xmm;
      sse2_expand_mask_1x1W(m0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
      sse2_negate_1x2W(im0xmm, m0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load_1xI8(src0xmm, src, pal);
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, im0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load_4xI8(src0xmm, src, pal);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, im0xmm, dst1xmm, dst1xmm, im0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 4;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);

        sse2_load4(dst0xmm, dst);
        sse2_load_1xI8(src0xmm, src, pal);
        sse2_expand_mask_1x1W(msk0xmm, msk0);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load_4xI8(src0xmm, src, pal);
        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 4;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load_1xI8(src0xmm, src, pal);
        if (msk0 == 0xFF) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);

vMaskAlphaSparseFill1:
        sse2_store4(dst, src0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load_4xI8(src0xmm, src, pal);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparseFill4:
        sse2_store16a(dst, src0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        src += 4;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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

  // -------------------------------------------------------------------------
  // [CompositeSrcSSE2 - XRGB32 - CBlit]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_fill_alpha_1x4B(src0xmm);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x16_ALIGN_BEGIN(cMaskOpaque)
      sse2_store4(dst, src0xmm);
      dst += 4;
    SSE2_BLIT_32x16_ALIGN_END(cMaskOpaque)

    SSE2_BLIT_32x16_MAIN_BEGIN(cMaskOpaque)
      sse2_store16a(dst, src0xmm);
      sse2_store16a(dst + 16, src0xmm);
      sse2_store16a(dst + 32, src0xmm);
      sse2_store16a(dst + 48, src0xmm);
      dst += 64;
    SSE2_BLIT_32x16_MAIN_END(cMaskOpaque)

    SSE2_BLIT_32x16_TAIL_4(cMaskOpaque, {
      sse2_store16a(dst, src0xmm);
      dst += 16;
    })

    SSE2_BLIT_32x16_TAIL_1(cMaskOpaque, {
      sse2_store4(dst, src0xmm);
      dst += 4;
    })
  }

  static void FOG_FASTCALL xrgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_fill_alpha_1x1B(src0orig);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x2W(src0xmm, src0orig);

    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x16_ALIGN_BEGIN(cMaskOpaque)
        sse2_store4(dst, src0orig);
        dst += 4;
      SSE2_BLIT_32x16_ALIGN_END(cMaskOpaque)

      SSE2_BLIT_32x16_MAIN_BEGIN(cMaskOpaque)
        sse2_store16a(dst, src0orig);
        sse2_store16a(dst + 16, src0orig);
        sse2_store16a(dst + 32, src0orig);
        sse2_store16a(dst + 48, src0orig);
        dst += 64;
      SSE2_BLIT_32x16_MAIN_END(cMaskOpaque)

      SSE2_BLIT_32x16_TAIL_4(cMaskOpaque, {
        sse2_store16a(dst, src0orig);
        dst += 16;
      });

      SSE2_BLIT_32x16_TAIL_1(cMaskOpaque, {
        sse2_store4(dst, src0orig);
        dst += 4;
      });
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i msrc0xmm;
      __m128i minv0xmm;

      sse2_expand_mask_1x1W(msrc0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msrc0xmm, msrc0xmm);

      sse2_negate_1x2W(minv0xmm, msrc0xmm);
      sse2_muldiv255_1x2W(msrc0xmm, msrc0xmm, src0xmm);
      sse2_pack_1x1W(msrc0xmm, msrc0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(dst0xmm, dst0xmm, msrc0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i dst0xmm, dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, msrc0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = Face::u32Negate255(READ_8(msk));

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load4(msk0xmm, msk);
        sse2_load16a(dst0xmm, dst);
        sse2_negate_1x4B(msk0xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        msk0 = Face::u32Negate255(msk0);
        msk0xmm = src0orig;
        if (msk0 == 0x00) goto vMaskAlphaSparseFill1;
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_adds_1x1W(msk0xmm, msk0xmm, dst0xmm);
        sse2_pack_1x1W(msk0xmm, msk0xmm);

vMaskAlphaSparseFill1:
        sse2_store4(dst, msk0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        msk0xmm = src0orig;
        msk0 = ~msk0;
        if (msk0 == 0x00000000) goto vMaskAlphaSparseFill4;

        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_adds_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
        sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

vMaskAlphaSparseFill4:
        sse2_store16a(dst, msk0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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
  // [CompositeSrcSSE2 - XRGB32 - VBlit - PRGB32]
  // --------------------------------------------------------------------------

  // Redirect to:
  //   CompositeSrcSSE2 - XRGB32 - VBlit - XRGB32.

  // --------------------------------------------------------------------------
  // [CompositeSrcSSE2 - XRGB32 - VBlit - ARGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_argb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      SSE2_BLIT_32xX_INIT();

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i src0xmm;

        sse2_load4(src0xmm, src);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);
        sse2_fill_alpha_1x1B(src0xmm);
        sse2_store4(dst, src0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i src1xmm;

        sse2_load16u(src0xmm, src);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
        sse2_fill_alpha_1x4B(src0xmm);
        sse2_store16a(dst, src0xmm);

        dst += 16;
        src += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i msk0xmm, imsk0xmm;
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
      sse2_negate_1x2W(imsk0xmm, msk0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i dst0xmm;
        __m128i src0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, imsk0xmm);
        sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, imsk0xmm, dst1xmm, dst1xmm, imsk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i src0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load16u(src0xmm, src);
        sse2_load4(msk0xmm, msk);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm;
        __m128i src0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load4(src0xmm, src);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        if (msk0 == 0xFF) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);

vMaskAlphaSparseFill1:
        sse2_pack_1x1W(src0xmm, src0xmm);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk0);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16u(src0xmm, src);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);

vMaskAlphaSparseFill4:
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
        sse2_fill_alpha_1x1B(src0xmm);
        sse2_store16a(dst, src0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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
  // [CompositeSrcSSE2 - XRGB32 - VBlit - XRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_xrgb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x16_ALIGN_BEGIN(cMaskOpaque)
        __m128i src0xmm;

        sse2_load4(src0xmm, src);
        sse2_fill_alpha_1x4B(src0xmm);
        sse2_store4(dst, src0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x16_ALIGN_END(cMaskOpaque)

      SSE2_BLIT_32x16_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i src1xmm;
        __m128i src2xmm;
        __m128i src3xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16u(src1xmm, src + 16);
        sse2_load16u(src2xmm, src + 32);
        sse2_load16u(src3xmm, src + 48);

        sse2_fill_alpha_1x4B(src0xmm);
        sse2_fill_alpha_1x4B(src1xmm);
        sse2_fill_alpha_1x4B(src2xmm);
        sse2_fill_alpha_1x4B(src3xmm);

        sse2_store16a(dst, src0xmm);
        sse2_store16a(dst + 16, src1xmm);
        sse2_store16a(dst + 32, src2xmm);
        sse2_store16a(dst + 48, src3xmm);

        dst += 64;
        src += 64;
      SSE2_BLIT_32x16_MAIN_END(cMaskOpaque)

      SSE2_BLIT_32x16_TAIL_4(cMaskOpaque, {
        __m128i src0xmm;

        sse2_load16u(src0xmm, src);
        sse2_fill_alpha_1x4B(src0xmm);
        sse2_store16a(dst, src0xmm);

        dst += 16;
        src += 16;
      });

      SSE2_BLIT_32x16_TAIL_1(cMaskOpaque, {
        __m128i src0xmm;

        sse2_load4(src0xmm, src);
        sse2_fill_alpha_1x4B(src0xmm);
        sse2_store4(dst, src0xmm);

        dst += 4;
        src += 4;
      });
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i msk0xmm, imsk0xmm;
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
      sse2_negate_1x2W(imsk0xmm, msk0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i dst0xmm;
        __m128i src0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, imsk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, imsk0xmm, dst1xmm, dst1xmm, imsk0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i src0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm;
        __m128i src0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load4(src0xmm, src);
        if (msk0 == 0xFF) goto vMaskAlphaSparseFill1;
        
        sse2_load4(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);

vMaskAlphaSparseFill1:
        sse2_fill_alpha_1x1B(src0xmm);
        sse2_store4(dst, src0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16u(src0xmm, src);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparseFill4:
        sse2_fill_alpha_1x4B(src0xmm);
        sse2_store16a(dst, src0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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
  // [CompositeSrcSSE2 - XRGB32 - VBlit - I8]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_i8_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    const uint32_t* pal = 
      reinterpret_cast<const uint32_t*>(closure->srcPalette) + Palette::INDEX_PRGB32;

    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        __m128i src0xmm;

        sse2_load_1xI8(src0xmm, src, pal);
        sse2_store4(dst, src0xmm);

        dst += 4;
        src += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i msk0xmm, imsk0xmm;
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
      sse2_negate_1x2W(imsk0xmm, msk0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i dst0xmm;
        __m128i src0xmm;

        sse2_load_1xI8(src0xmm, src, pal);
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, imsk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;

        sse2_load_4xI8(src0xmm, src, pal);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, imsk0xmm, dst1xmm, dst1xmm, imsk0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 4;
      SSE2_BLIT_32x4_MAIN_END(cMaskAlpha)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i src0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);

        sse2_load_1xI8(src0xmm, src, pal);
        sse2_load4(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load_4xI8(src0xmm, src, pal);
        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 4;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm;
        __m128i src0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load_1xI8(src0xmm, src, pal);
        if (msk0 == 0xFF) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);
        sse2_fill_alpha_1x1B(src0xmm);

vMaskAlphaSparseFill1:
        sse2_store4(dst, src0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load_4xI8(src0xmm, src, pal);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparseFill4:
        sse2_fill_alpha_1x4B(src0xmm);
        sse2_store16a(dst, src0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        src += 4;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse)
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

} // RasterEngine namespace
} // Fog namespace
