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
// [Fog::RasterEngine::SSE2 - Composite - Clear]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeClearSSE2
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_CLEAR };

  // --------------------------------------------------------------------------
  // [CompositeClearSSE2 - Helpers]
  // --------------------------------------------------------------------------

  static FOG_INLINE void xxxx32_cblit_full_inline(
    uint8_t* dst, int w, const __m128i& zero0xmm)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x16_ALIGN_BEGIN(cMaskOpaque)
      sse2_store4(dst, zero0xmm);
      dst += 4;
    SSE2_BLIT_32x16_ALIGN_END(cMaskOpaque)

    SSE2_BLIT_32x16_MAIN_BEGIN(cMaskOpaque)
      sse2_store16a(dst, zero0xmm);
      sse2_store16a(dst + 16, zero0xmm);
      sse2_store16a(dst + 32, zero0xmm);
      sse2_store16a(dst + 48, zero0xmm);
      dst += 64;
    SSE2_BLIT_32x16_MAIN_END(cMaskOpaque)

    SSE2_BLIT_32x16_TAIL_4(cMaskOpaque, {
      sse2_store16a(dst, zero0xmm);
      dst += 16;
    });

    SSE2_BLIT_32x16_TAIL_1(cMaskOpaque, {
      sse2_store4(dst, zero0xmm);
      dst += 4;
    });
  }

  // --------------------------------------------------------------------------
  // [CompositeClearSSE2 - PRGB32 - CBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    FOG_UNUSED(src);
    __m128i zero0xmm = _mm_setzero_si128();

    xxxx32_cblit_full_inline(dst, w, zero0xmm);
  }

  static void FOG_FASTCALL prgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    FOG_UNUSED(src);
    __m128i zero0xmm = _mm_setzero_si128();

    // [Begin] ----------------------------------------------------------------
    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      xxxx32_cblit_full_inline(dst, w, zero0xmm);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i msk0xmm;

      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
      sse2_negate_1x2W(msk0xmm, msk0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i dst0xmm, dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
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

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
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

        uint32_t msk0 = READ_8(msk0);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        msk0 = Face::b32_1x1Negate255(msk0);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        if (msk0 == 0x00) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, dst0xmm);
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

        msk0 = ~msk0;
        msk0xmm = _mm_cvtsi32_si128(msk0);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
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
  // [CompositeClearSSE2 - PRGB32 - VBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    prgb32_cblit_full(dst, reinterpret_cast<const RasterSolid*>(src), w, closure);
  }

  static void FOG_FASTCALL prgb32_vblit_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    prgb32_cblit_span(dst, NULL, span, closure);
  }

  // --------------------------------------------------------------------------
  // [CompositeClearSSE2 - XRGB32 - CBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    FOG_UNUSED(src);
    __m128i zero0xmm = FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000);

    xxxx32_cblit_full_inline(dst, w, zero0xmm);
  }

  static void FOG_FASTCALL xrgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    FOG_UNUSED(src);
    __m128i zero0xmm = FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000);

    // [Begin] ----------------------------------------------------------------
    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      xxxx32_cblit_full_inline(dst, w, zero0xmm);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i msk0xmm;

      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
      sse2_negate_1x2W(msk0xmm, msk0xmm);
      sse2_fill_alpha_1x2W(msk0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i dst0xmm, dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
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

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
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

        msk0 = Face::b32_1x1Negate255(msk0);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        if (msk0 == 0x00) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, dst0xmm);
        sse2_pack_1x1W(msk0xmm, msk0xmm);

vMaskAlphaSparseFill1:
        sse2_fill_alpha_1x1B(msk0xmm);
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

        msk0 = ~msk0;
        msk0xmm = _mm_cvtsi32_si128(msk0);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseFill4;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
        sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

vMaskAlphaSparseFill4:
        sse2_fill_alpha_1x4B(msk0xmm);
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
  // [CompositeClearSSE2 - XRGB32 - VBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    xrgb32_cblit_full(dst, reinterpret_cast<const RasterSolid*>(src), w, closure);
  }

  static void FOG_FASTCALL xrgb32_vblit_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    xrgb32_cblit_span(dst, NULL, span, closure);
  }
};

} // RasterEngine namespace
} // Fog namespace
