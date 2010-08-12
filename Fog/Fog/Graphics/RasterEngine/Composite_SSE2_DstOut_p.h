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
// [Fog::RasterEngine::SSE2 - Composite - DstOut]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeDstOutSSE2
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DST_OUT };

  // -------------------------------------------------------------------------
  // [CompositeDstOutSSE2 - PRGB32 - CBlit - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_alpha_1x1W(src0xmm, src0xmm);
    sse2_negate_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
  }

  static void FOG_FASTCALL prgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_alpha_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Generic] ------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_GENERIC()
    {
      __m128i minv0xmm;
      sse2_expand_mask_1x1W(minv0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(minv0xmm, minv0xmm);
      sse2_muldiv255_1x2W(minv0xmm, minv0xmm, src0xmm);
      sse2_negate_1x2W(minv0xmm, minv0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskGeneric)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskGeneric)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskGeneric)
        __m128i dst0xmm, dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskGeneric)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
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
        sse2_unpack_1x1W(msk0xmm, msk0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);

        sse2_expand_mask_2x2W_from_1x1W(msk0xmm, msk1xmm, msk0xmm);
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

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
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

        sse2_load16a(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);

        sse2_expand_mask_2x2W_from_1x1W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
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

  // -------------------------------------------------------------------------
  // [CompositeDstOutSSE2 - PRGB32 - VBlit - PRGB32/ARGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_prgb32_or_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_negate_1x4B(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_unpack_1x1W(src0xmm, src0xmm);

      sse2_expand_alpha_1x1W(src0xmm, src0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_negate_1x4B(src0xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);

      sse2_expand_alpha_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
  }

  static void FOG_FASTCALL prgb32_vblit_prgb32_or_argb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Generic] ------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_GENERIC()
    {
      __m128i msk0xmm;
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskGeneric)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskGeneric)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskGeneric)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_expand_alpha_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);
        sse2_negate_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskGeneric)
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

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);

        src0xmm = _mm_srli_epi32(src0xmm, 24);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);
        src0xmm = _mm_packs_epi32(src0xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);

        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
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
        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaSparseSkip1:
        src += 4;
        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        src0xmm = _mm_srli_epi32(src0xmm, 24);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        src0xmm = _mm_packs_epi32(src0xmm, src0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);

        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

vMaskAlphaSparseSkip4:
        src += 16;
        dst += 16;
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
  // [CompositeDstOutSSE2 - PRGB32 - VBlit - A8]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_a8_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_expand_mask_1x1W(src0xmm, READ_8(src));
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_negate_1x1W(src0xmm, src0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 1;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load4(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_expand_mask_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_negate_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 4;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
  }

  static void FOG_FASTCALL prgb32_vblit_a8_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Generic] ------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_GENERIC()
    {
      __m128i msk0xmm;
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskGeneric)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(src0xmm, READ_8(src));

        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
      SSE2_BLIT_32x4_SMALL_END(cMaskGeneric)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskGeneric)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load4(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x2W(src0xmm, src0xmm);

        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 4;
      SSE2_BLIT_32x4_MAIN_END(cMaskGeneric)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        uint32_t src0 = READ_8(src);

        msk0 = Face::b32_1x1MulDiv255(src0, msk0);
        msk0 = Face::b32_1x1Negate255(msk0);

        sse2_load4(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(src0xmm, src);
        sse2_load4(msk0xmm, msk);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);

        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

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
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0;
        uint32_t src0;

        msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        src0 = READ_8(src0);
        msk0 = Face::b32_1x1MulDiv255(src0, msk0);
        msk0 = Face::b32_1x1Negate255(msk0);

        sse2_load4(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(src0xmm, src);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

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
  // [CompositeDstOutSSE2 - XRGB32 - CBlit - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_alpha_1x1W(src0xmm, src0xmm);
    sse2_negate_1x1W(src0xmm, src0xmm);
    sse2_fill_alpha_1x1W(src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
  }

  static void FOG_FASTCALL xrgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_alpha_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Generic] ------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_GENERIC()
    {
      __m128i minv0xmm;
      sse2_expand_mask_1x1W(minv0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(minv0xmm, minv0xmm);
      sse2_muldiv255_1x2W(minv0xmm, minv0xmm, src0xmm);
      sse2_negate_1x2W(minv0xmm, minv0xmm);
      sse2_fill_alpha_1x2W(minv0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskGeneric)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskGeneric)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskGeneric)
        __m128i dst0xmm, dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskGeneric)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
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
        sse2_unpack_1x1W(msk0xmm, msk0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);

        sse2_expand_mask_2x2W_from_1x1W(msk0xmm, msk1xmm, msk0xmm);
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

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
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

        sse2_load16a(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);

        sse2_expand_mask_2x2W_from_1x1W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
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

  // -------------------------------------------------------------------------
  // [CompositeDstOutSSE2 - XRGB32 - VBlit - PRGB32/ARGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_prgb32_or_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_negate_1x4B(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_unpack_1x1W(src0xmm, src0xmm);

      sse2_expand_alpha_1x1W(src0xmm, src0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_negate_1x4B(src0xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);

      sse2_expand_alpha_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
  }

  static void FOG_FASTCALL xrgb32_vblit_prgb32_or_argb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Generic] ------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_GENERIC()
    {
      __m128i msk0xmm;
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskGeneric)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskGeneric)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskGeneric)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_expand_alpha_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);
        sse2_negate_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskGeneric)
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

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);

        src0xmm = _mm_srli_epi32(src0xmm, 24);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);
        src0xmm = _mm_packs_epi32(src0xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);

        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
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
        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaSparseSkip1:
        src += 4;
        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        src0xmm = _mm_srli_epi32(src0xmm, 24);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        src0xmm = _mm_packs_epi32(src0xmm, src0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);

        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

vMaskAlphaSparseSkip4:
        src += 16;
        dst += 16;
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
  // [CompositeDstOutSSE2 - XRGB32 - VBlit - A8]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_a8_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_expand_mask_1x1W(src0xmm, READ_8(src));
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_negate_1x1W(src0xmm, src0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 1;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load4(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_expand_mask_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_negate_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 4;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
  }

  static void FOG_FASTCALL xrgb32_vblit_a8_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Generic] ------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_GENERIC()
    {
      __m128i msk0xmm;
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskGeneric)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(src0xmm, READ_8(src));

        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
      SSE2_BLIT_32x4_SMALL_END(cMaskGeneric)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskGeneric)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load4(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x2W(src0xmm, src0xmm);

        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        src += 4;
      SSE2_BLIT_32x4_MAIN_END(cMaskGeneric)
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        uint32_t src0 = READ_8(src);

        msk0 = Face::b32_1x1MulDiv255(src0, msk0);
        msk0 = Face::b32_1x1Negate255(msk0);

        sse2_load4(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(src0xmm, src);
        sse2_load4(msk0xmm, msk);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);

        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

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
        __m128i msk0xmm;

        uint32_t msk0;
        uint32_t src0;

        msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        src0 = READ_8(src0);
        msk0 = Face::b32_1x1MulDiv255(src0, msk0);
        msk0 = Face::b32_1x1Negate255(msk0);

        sse2_load4(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 1;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(src0xmm, src);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(src0xmm, src0xmm);
        sse2_expand_mask_2x2W_from_1x1W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_store16a(dst, dst0xmm);

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
