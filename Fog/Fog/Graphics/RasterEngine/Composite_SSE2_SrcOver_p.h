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
// [Fog::RasterEngine::SSE2 - Composite - SrcOver]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeSrcOverSSE2
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_SRC_OVER };

  // --------------------------------------------------------------------------
  // [CompositeSrcOverSSE2 - PRGB32 - CBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    __m128i src0xmm;
    __m128i ia0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

    sse2_unpack_1x2W(ia0xmm, src0xmm);
    sse2_expand_alpha_1x2W(ia0xmm, ia0xmm);
    sse2_negate_1x1W(ia0xmm, ia0xmm);

    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, ia0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_adds_1x1B(dst0xmm, dst0xmm, src0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i dst0xmm;
      __m128i dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, ia0xmm, dst1xmm, dst1xmm, ia0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_adds_1x4B(dst0xmm, dst0xmm, src0xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
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

    __m128i inv0xmm;
    sse2_expand_alpha_1x2W(inv0xmm, src0xmm);
    sse2_negate_1x2W(inv0xmm, inv0xmm);

    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, inv0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(dst0xmm, dst0xmm, src0orig);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i dst0xmm;
        __m128i dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, inv0xmm, dst1xmm, dst1xmm, inv0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, src0orig);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i msrc0xmm;
      __m128i minv0xmm;

      sse2_expand_mask_1x1W(msrc0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msrc0xmm, msrc0xmm);
      sse2_muldiv255_1x2W(msrc0xmm, msrc0xmm, src0xmm);

      sse2_negate_1x2W(minv0xmm, msrc0xmm);
      sse2_expand_alpha_1x2W(minv0xmm, minv0xmm);

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
      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i mskinv0xmm;

        uint32_t msk0 = READ_8(msk);

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(mskinv0xmm, msk0xmm);
        sse2_expand_alpha_1x1W(mskinv0xmm, mskinv0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, mskinv0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i mskinv0xmm, mskinv1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
        sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
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
      SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i mskinv0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        if (msk0 == 0xFF) goto vMaskAlphaSparseNoMask1;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(mskinv0xmm, msk0xmm);
        sse2_expand_alpha_1x1W(mskinv0xmm, mskinv0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, mskinv0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        msk += 1;
        SSE2_BLIT_32x4_SMALL_CONTINUE(vMaskAlphaSparse)

vMaskAlphaSparseNoMask1:
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, inv0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(dst0xmm, dst0xmm, src0orig);
        sse2_store4(dst, dst0xmm);
        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i mskinv0xmm, mskinv1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16a(dst0xmm, dst);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseNoMask4;

        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
        sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        msk += 4;
        SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse)

vMaskAlphaSparseNoMask4:
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, inv0xmm, dst1xmm, dst1xmm, inv0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, src0orig);
        sse2_store16a(dst, dst0xmm);

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
  // [CompositeSrcOverSSE2 - PRGB32 - CBlit - XRGB32]
  // --------------------------------------------------------------------------

  // Redirect to:
  //   CompositeSrcSSE2 - PRGB32 - CBlit - XRGB32

  // --------------------------------------------------------------------------
  // [CompositeSrcOverSSE2 - PRGB32 - VBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_prgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i alp0xmm;

      uint32_t src0 = READ_32(src);
      if (RasterUtil::isAlpha0x00_PRGB32(src0)) goto cMaskOpaqueSkip1;
      if (RasterUtil::isAlpha0xFF_PRGB32(src0)) goto cMaskOpaqueFill1;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_expand_alpha_1x1W(alp0xmm, src0xmm);
      sse2_negate_1x1W(alp0xmm, alp0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, alp0xmm);
      sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
      sse2_pack_1x1W(src0xmm, src0xmm);

cMaskOpaqueFill1:
      sse2_store4(dst, src0xmm);

cMaskOpaqueSkip1:
      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i alp0xmm, alp1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);

      SSE2_BLIT_TEST_4_PRGB_PIXELS(src0xmm, alp0xmm, alp1xmm, 
        cMaskOpaqueFill4, 
        cMaskOpaqueSkip4)

      sse2_load16a(dst0xmm, dst);
      alp0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(3, 2, 1, 0));
      alp0xmm = _mm_xor_si128(alp0xmm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_unpack_2x2W(alp0xmm, alp1xmm, alp0xmm);
      sse2_expand_alpha_2x2W(alp0xmm, alp0xmm, alp1xmm, alp1xmm);
      sse2_muldiv255_2x2W(alp0xmm, alp0xmm, dst0xmm, alp1xmm, alp1xmm, dst1xmm);
      sse2_pack_2x2W(alp0xmm, alp0xmm, alp1xmm);
      sse2_adds_1x4B(src0xmm, src0xmm, alp0xmm);

cMaskOpaqueFill4:
      sse2_store16a(dst, src0xmm);

cMaskOpaqueSkip4:
      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
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
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i alp0xmm;

        uint32_t src0 = READ_32(src);
        if (RasterUtil::isAlpha0x00_PRGB32(src0)) goto cMaskOpaqueSkip1;
        src0xmm = _mm_cvtsi32_si128(src0);
        if (RasterUtil::isAlpha0xFF_PRGB32(src0)) goto cMaskOpaqueFill1;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_alpha_1x1W(alp0xmm, src0xmm);
        sse2_negate_1x1W(alp0xmm, alp0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, alp0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);

cMaskOpaqueFill1:
        sse2_store4(dst, src0xmm);

cMaskOpaqueSkip1:
        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i alp0xmm, alp1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load16u(src0xmm, src);

        SSE2_BLIT_TEST_4_PRGB_PIXELS(src0xmm, alp0xmm, alp1xmm, 
          cMaskOpaqueFill4, 
          cMaskOpaqueSkip4)

        sse2_load16a(dst0xmm, dst);
        alp0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(3, 2, 1, 0));
        alp0xmm = _mm_xor_si128(alp0xmm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_2x2W(alp0xmm, alp1xmm, alp0xmm);
        sse2_expand_alpha_2x2W(alp0xmm, alp0xmm, alp1xmm, alp1xmm);
        sse2_muldiv255_2x2W(alp0xmm, alp0xmm, dst0xmm, alp1xmm, alp1xmm, dst1xmm);
        sse2_pack_2x2W(alp0xmm, alp0xmm, alp1xmm);
        sse2_adds_1x4B(src0xmm, src0xmm, alp0xmm);

cMaskOpaqueFill4:
        sse2_store16a(dst, src0xmm);

cMaskOpaqueSkip4:
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
        __m128i a0xmm;

        uint32_t src0 = READ_32(src);
        if (RasterUtil::isAlpha0x00_PRGB32(src0)) goto cMaskAlphaSkip1;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

cMaskAlphaSkip1:
        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i a0xmm, a1xmm;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_PRGB_PIXELS_ZERO(src0xmm, dst0xmm, cMaskAlphaSkip4)

        sse2_load16a(dst0xmm, dst);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
        sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
        sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

cMaskAlphaSkip4:
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

        uint32_t src0 = READ_32(src);
        uint32_t msk0;
        if (RasterUtil::isAlpha0x00_PRGB32(src0)) goto vMaskAlphaDenseSkip1;

        msk0 = READ_8(msk);
        sse2_load4(dst0xmm, dst);
        src0xmm = _mm_cvtsi32_si128(src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_expand_alpha_1x1W(msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaDenseSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_PRGB_PIXELS_ZERO(src0xmm, dst0xmm, vMaskAlphaDenseSkip4)

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_expand_alpha_2x2W(msk0xmm, src0xmm, msk1xmm, src1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

vMaskAlphaDenseSkip4:
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

        uint32_t src0 = READ_32(src);
        uint32_t msk0 = READ_8(msk);

        if ((msk0 == 0x00) | (RasterUtil::isAlpha0x00_PRGB32(src0))) goto vMaskAlphaSparseSkip1;

        sse2_load4(dst0xmm, dst);
        src0xmm = _mm_cvtsi32_si128(src0);
        if ((msk0 == 0xFF) & (RasterUtil::isAlpha0xFF_PRGB32(src0))) goto vMaskAlphaSparseFill1;

        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_expand_alpha_1x1W(msk0xmm, src0xmm);
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
        SSE2_BLIT_TEST_4_PRGB_PIXELS_MASK(src0xmm, dst0xmm, dst1xmm, msk0, 
          vMaskAlphaSparseFill4,
          vMaskAlphaSparseSkip4)

        sse2_load16a(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_expand_alpha_2x2W(msk0xmm, src0xmm, msk1xmm, src1xmm);
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
  // [CompositeSrcOverSSE2 - PRGB32 - VBlit - ARGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i dst0xmm;
      __m128i src0xmm;
      __m128i msk0xmm;

      uint32_t src0 = READ_32(src);
      if (RasterUtil::isAlpha0x00_ARGB32(src0)) goto cMaskOpaqueSkip1;
      src0xmm = _mm_cvtsi32_si128(src0);
      if (RasterUtil::isAlpha0xFF_ARGB32(src0)) goto cMaskOpaqueFill1;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(msk0xmm, src0xmm);
      sse2_fill_alpha_1x1W(src0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

      sse2_negate_1x1W(msk0xmm, msk0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

      sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
      sse2_pack_1x1W(src0xmm, src0xmm);

cMaskOpaqueFill1:
      sse2_store4(dst, src0xmm);

cMaskOpaqueSkip1:
      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;
      __m128i msk0xmm, msk1xmm;

      sse2_load16u(src0xmm, src);

      SSE2_BLIT_TEST_4_ARGB_PIXELS(src0xmm, dst0xmm, dst1xmm,
        cMaskOpaqueFill4, 
        cMaskOpaqueSkip4)

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_expand_alpha_2x2W(msk0xmm, src0xmm, msk1xmm, src1xmm);
      sse2_fill_alpha_2x2W(src0xmm, src1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
      sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
      sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
      sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

cMaskOpaqueFill4:
      sse2_store16a(dst, src0xmm);

cMaskOpaqueSkip4:
      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
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
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i dst0xmm;
        __m128i src0xmm;
        __m128i msk0xmm;

        uint32_t src0 = READ_32(src);
        if (RasterUtil::isAlpha0x00_ARGB32(src0)) goto cMaskOpaqueSkip1;
        src0xmm = _mm_cvtsi32_si128(src0);
        if (RasterUtil::isAlpha0xFF_ARGB32(src0)) goto cMaskOpaqueFill1;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(msk0xmm, src0xmm);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);

cMaskOpaqueFill1:
        sse2_store4(dst, src0xmm);

cMaskOpaqueSkip1:
        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16u(src0xmm, src);

        SSE2_BLIT_TEST_4_ARGB_PIXELS(src0xmm, dst0xmm, dst1xmm,
          cMaskOpaqueFill4, 
          cMaskOpaqueSkip4)

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_alpha_2x2W(msk0xmm, src0xmm, msk1xmm, src1xmm);
        sse2_fill_alpha_2x2W(src0xmm, src1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

cMaskOpaqueFill4:
        sse2_store16a(dst, src0xmm);

cMaskOpaqueSkip4:
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
        __m128i a0xmm;

        uint32_t src0 = READ_32(src);
        uint32_t msk0;
        if (RasterUtil::isAlpha0x00_ARGB32(src0)) goto cMaskAlphaSkip1;

        msk0 = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);

        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

cMaskAlphaSkip1:
        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i a0xmm, a1xmm;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_ARGB_PIXELS_ZERO(src0xmm, dst0xmm, cMaskAlphaSkip4)

        sse2_load16a(dst0xmm, dst);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
        sse2_muldiv255_2x2W(a0xmm, a0xmm, m0xmm, a1xmm, a1xmm, m0xmm);

        sse2_fill_alpha_2x2W(src0xmm, src1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
        sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

cMaskAlphaSkip4:
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
        __m128i a0xmm;

        uint32_t src0 = READ_32(src);
        uint32_t msk0;
        if (RasterUtil::isAlpha0x00_ARGB32(src0)) goto vMaskAlphaSkip1;

        sse2_load4(dst0xmm, dst);
        msk0 = ByteUtil::scalar_muldiv255(src0 >> 24, READ_8(msk));
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i a0xmm, a1xmm;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_ARGB_PIXELS_ZERO(src0xmm, dst0xmm, vMaskAlphaSkip4)

        sse2_load4(dst0xmm, msk);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_mask_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
        sse2_muldiv255_2x2W(a0xmm, a0xmm, dst0xmm, a1xmm, a1xmm, dst1xmm);

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_fill_alpha_2x2W(src0xmm, src1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
        sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

vMaskAlphaSkip4:
        dst += 16;
        src += 16;
        msk += 4;
      SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense)
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
  // [CompositeSrcOverSSE2 - PRGB32 - VBlit - XRGB32]
  // --------------------------------------------------------------------------

  // Redirect to:
  //   CompositeSrcSSE2 - PRGB32 - VBlit - XRGB32

  // --------------------------------------------------------------------------
  // [CompositeSrcOverSSE2 - XRGB32 - CBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

    SSE2_BLIT_32xX_INIT()

    __m128i ia0xmm;
    sse2_unpack_1x2W(ia0xmm, src0xmm);
    sse2_expand_alpha_1x2W(ia0xmm, ia0xmm);
    sse2_negate_1x1W(ia0xmm, ia0xmm);

    SSE2_BLIT_32x4_SMALL_BEGIN(cBlitOpaque)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_fill_alpha_1x1B(dst0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, ia0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_adds_1x1B(dst0xmm, dst0xmm, src0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cBlitOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cBlitOpaque)
      __m128i dst0xmm;
      __m128i dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, ia0xmm, dst1xmm, dst1xmm, ia0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_adds_1x4B(dst0xmm, dst0xmm, src0xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cBlitOpaque)
  }

  static void FOG_FASTCALL xrgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    __m128i src0orig;
    __m128i src0xmm;
    __m128i ia0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x2W(src0xmm, src0orig);

    sse2_expand_alpha_1x2W(ia0xmm, src0xmm);
    sse2_negate_1x2W(ia0xmm, ia0xmm);

    sse2_fill_alpha_1x4B(src0orig);

    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cBlitOpaque)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, ia0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(dst0xmm, dst0xmm, src0orig);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cBlitOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cBlitOpaque)
        __m128i dst0xmm;
        __m128i dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, ia0xmm, dst1xmm, dst1xmm, ia0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, src0orig);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cBlitOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      __m128i msrc0xmm;
      __m128i minv0xmm;

      sse2_expand_mask_1x1W(msrc0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msrc0xmm, msrc0xmm);
      sse2_muldiv255_1x2W(msrc0xmm, msrc0xmm, src0xmm);

      sse2_negate_1x2W(minv0xmm, msrc0xmm);
      sse2_expand_alpha_1x2W(minv0xmm, minv0xmm);

      sse2_pack_1x1W(msrc0xmm, msrc0xmm);

      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
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
        sse2_fill_alpha_1x4B(dst0xmm);
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
        __m128i mskinv0xmm;

        uint32_t msk0 = READ_8(msk);

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(mskinv0xmm, msk0xmm);
        sse2_expand_alpha_1x1W(mskinv0xmm, mskinv0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, mskinv0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i mskinv0xmm, mskinv1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_load4(msk0xmm, msk);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
        sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
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
        __m128i mskinv0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        if (msk0 == 0xFF) goto vMaskAlphaSparseNoMask1;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(mskinv0xmm, msk0xmm);
        sse2_expand_alpha_1x1W(mskinv0xmm, mskinv0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, mskinv0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        msk += 1;
        SSE2_BLIT_32x4_SMALL_CONTINUE(vMaskAlphaSparse);

vMaskAlphaSparseNoMask1:
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, ia0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(dst0xmm, dst0xmm, src0orig);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i mskinv0xmm, mskinv1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16a(dst0xmm, dst);
        msk0xmm = _mm_cvtsi32_si128(msk0);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparseNoMask4;

        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
        sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

vMaskAlphaSparseSkip4:
        dst += 16;
        msk += 4;
        SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse)

vMaskAlphaSparseNoMask4:
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, ia0xmm, dst1xmm, dst1xmm, ia0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, src0orig);
        sse2_store16a(dst, dst0xmm);

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
  // [CompositeSrcOverSSE2 - XRGB32 - CBlit - XRGB32]
  // --------------------------------------------------------------------------

  // Redirect to:
  //   CompositeSrcSSE2 - XRGB32 - CBlit - XRGB32.

  // --------------------------------------------------------------------------
  // [CompositeSrcOverSSE2 - XRGB32 - VBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_prgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);
      if (RasterUtil::isAlpha0x00_PRGB32(src0)) goto cMaskOpaqueSkip1;

      sse2_load4(dst0xmm, dst);
      sse2_fill_alpha_1x1B(dst0xmm);
      sse2_unpack_1x1W(src0xmm, src0);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_expand_alpha_1x1W(a0xmm, src0xmm);
      sse2_negate_1x1W(a0xmm, a0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
      sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

cMaskOpaqueSkip1:
      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i alp0xmm, alp1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);

      SSE2_BLIT_TEST_4_PRGB_PIXELS(src0xmm, alp0xmm, alp1xmm,
        cMaskOpaqueFill4,
        cMaskOpaqueSkip4)

      sse2_load16a(dst0xmm, dst);
      alp0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(3, 2, 1, 0));
      alp0xmm = _mm_xor_si128(alp0xmm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_unpack_2x2W(alp0xmm, alp1xmm, alp0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_expand_alpha_2x2W(alp0xmm, alp0xmm, alp1xmm, alp1xmm);
      sse2_muldiv255_2x2W(alp0xmm, alp0xmm, dst0xmm, alp1xmm, alp1xmm, dst1xmm);
      sse2_pack_2x2W(alp0xmm, alp0xmm, alp1xmm);
      sse2_adds_1x4B(src0xmm, src0xmm, alp0xmm);

cMaskOpaqueFill4:
      sse2_store16a(dst, src0xmm);

cMaskOpaqueSkip4:
      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
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
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;
        __m128i a0xmm;

        uint32_t src0 = READ_32(src);
        if (RasterUtil::isAlpha0x00_PRGB32(src0)) goto cMaskOpaqueSkip1;

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

cMaskOpaqueSkip1:
        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i alp0xmm, alp1xmm;
        __m128i dst0xmm, dst1xmm;

        sse2_load16u(src0xmm, src);

        SSE2_BLIT_TEST_4_PRGB_PIXELS(src0xmm, alp0xmm, alp1xmm,
          cMaskOpaqueFill4,
          cMaskOpaqueSkip4)

        sse2_load16a(dst0xmm, dst);
        alp0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(3, 2, 1, 0));
        alp0xmm = _mm_xor_si128(alp0xmm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_unpack_2x2W(alp0xmm, alp1xmm, alp0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_alpha_2x2W(alp0xmm, alp0xmm, alp1xmm, alp1xmm);
        sse2_muldiv255_2x2W(alp0xmm, alp0xmm, dst0xmm, alp1xmm, alp1xmm, dst1xmm);
        sse2_pack_2x2W(alp0xmm, alp0xmm, alp1xmm);
        sse2_adds_1x4B(src0xmm, src0xmm, alp0xmm);

cMaskOpaqueFill4:
        sse2_store16a(dst, src0xmm);

cMaskOpaqueSkip4:
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
        __m128i a0xmm;

        uint32_t src0 = READ_32(src);
        if (RasterUtil::isAlpha0x00_PRGB32(src0)) goto cMaskAlphaSkip1;

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);

        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

cMaskAlphaSkip1:
        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i a0xmm, a1xmm;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_PRGB_PIXELS_ZERO(src0xmm, dst0xmm, cMaskAlphaSkip4)

        sse2_load16a(dst0xmm, dst);
        sse2_fill_alpha_1x4B(dst0xmm);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
        sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
        sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

cMaskAlphaSkip4:
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
        __m128i a0xmm;

        uint32_t src0 = READ_32(src);
        uint32_t msk0;
        if (RasterUtil::isAlpha0x00_PRGB32(src0)) goto vMaskAlphaDenseSkip1;

        msk0 = READ_8(msk);
        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

vMaskAlphaDenseSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i a0xmm, a1xmm;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_PRGB_PIXELS_ZERO(src0xmm, dst0xmm, vMaskAlphaDenseSkip4)

        sse2_load16a(dst0xmm, dst);
        sse2_load4(a0xmm, msk);
        sse2_fill_alpha_1x4B(dst0xmm);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_mask_2x2W(a0xmm, a1xmm, a0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
        sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
        sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

vMaskAlphaDenseSkip4:
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
        __m128i a0xmm;

        uint32_t src0;
        uint32_t msk0;

        msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        src0 = READ_32(src);
        if (RasterUtil::isAlpha0x00_PRGB32(src0)) goto vMaskAlphaSparseSkip1;
        if (RasterUtil::isAlpha0xFF_PRGB32(src0) & (msk0 == 0xFF)) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
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
        __m128i a0xmm, a1xmm;

        uint32_t msk0 = READ_32(msk0);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_PRGB_PIXELS_MASK(src0xmm, dst0xmm, dst1xmm, msk0, 
          vMaskAlphaSparseFill4,
          vMaskAlphaSparseSkip4)

        sse2_load16a(dst0xmm, dst);
        a0xmm = _mm_cvtsi32_si128(msk0);
        sse2_fill_alpha_1x4B(dst0xmm);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_mask_2x2W(a0xmm, a1xmm, a0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
        sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
        sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparseFill4:
        sse2_store16a(dst, dst0xmm);

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
  // [CompositeSrcOverSSE2 - XRGB32 - VBlit - ARGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i dst0xmm;
      __m128i src0xmm;
      __m128i msk0xmm;

      uint32_t src0 = READ_32(src);
      if (RasterUtil::isAlpha0x00_ARGB32(src0)) goto cMaskOpaqueSkip1;
      src0xmm = _mm_cvtsi32_si128(src0);
      if (RasterUtil::isAlpha0xFF_ARGB32(src0)) goto cMaskOpaqueFill1;

      sse2_load4(dst0xmm, dst);
      sse2_fill_alpha_1x1B(dst0xmm);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(msk0xmm, src0xmm);
      sse2_fill_alpha_1x1W(src0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
      sse2_negate_1x1W(msk0xmm, msk0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
      sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
      sse2_pack_1x1W(src0xmm, src0xmm);

cMaskOpaqueFill1:
      sse2_store4(dst, src0xmm);

cMaskOpaqueSkip1:
      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;
      __m128i msk0xmm, msk1xmm;

      sse2_load16u(src0xmm, src);

      SSE2_BLIT_TEST_4_ARGB_PIXELS(src0xmm, dst0xmm, dst1xmm, 
        cMaskOpaqueFill4,
        cMaskOpaqueSkip4)

      sse2_load16a(dst0xmm, dst);
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_expand_alpha_2x2W(msk0xmm, src0xmm, msk1xmm, src1xmm);
      sse2_fill_alpha_2x2W(src0xmm, src1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
      sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
      sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
      sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

cMaskOpaqueFill4:
      sse2_store16a(dst, src0xmm);

cMaskOpaqueSkip4:
      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
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
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i dst0xmm;
        __m128i src0xmm;
        __m128i msk0xmm;

        uint32_t src0 = READ_32(src);
        if (RasterUtil::isAlpha0x00_ARGB32(src0)) goto cMaskOpaqueSkip1;
        src0xmm = _mm_cvtsi32_si128(src0);
        if (RasterUtil::isAlpha0xFF_ARGB32(src0)) goto cMaskOpaqueFill1;

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(msk0xmm, src0xmm);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, dst0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);

cMaskOpaqueFill1:
        sse2_store4(dst, src0xmm);

cMaskOpaqueSkip1:
        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i dst0xmm, dst1xmm;
        __m128i src0xmm, src1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16u(src0xmm, src);

        SSE2_BLIT_TEST_4_ARGB_PIXELS(src0xmm, dst0xmm, dst1xmm, 
          cMaskOpaqueFill4,
          cMaskOpaqueSkip4)

        sse2_load16a(dst0xmm, dst);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_alpha_2x2W(msk0xmm, src0xmm, msk1xmm, src1xmm);
        sse2_fill_alpha_2x2W(src0xmm, src1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

cMaskOpaqueFill4:
        sse2_store16a(dst, src0xmm);

cMaskOpaqueSkip4:
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
        __m128i a0xmm;

        uint32_t src0 = READ_32(src);
        uint32_t m0;

        if (RasterUtil::isAlpha0x00_ARGB32(src0)) goto cMaskAlphaSkip1;

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);

        m0 = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);

        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, m0);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

cMaskAlphaSkip1:
        dst += 4;
        src += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskAlpha)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i a0xmm, a1xmm;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_ARGB_PIXELS_ZERO(src0xmm, dst0xmm, cMaskAlphaSkip4)

        sse2_load16a(dst0xmm, dst);
        sse2_fill_alpha_1x4B(dst0xmm);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
        sse2_muldiv255_2x2W(a0xmm, a0xmm, m0xmm, a1xmm, a1xmm, m0xmm);

        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_fill_alpha_2x2W(src0xmm, src1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
        sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

cMaskAlphaSkip4:
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
        __m128i a0xmm;

        uint32_t src0;
        uint32_t msk0;

        src0 = READ_32(src);
        if (RasterUtil::isAlpha0x00_ARGB32(src0)) goto vMaskAlphaDenseSkip1;

        msk0 = READ_8(msk);
        msk0 = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

        sse2_store4(dst, dst0xmm);

vMaskAlphaDenseSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i a0xmm, a1xmm;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_ARGB_PIXELS_ZERO(src0xmm, dst0xmm, vMaskAlphaDenseSkip4)

        sse2_load4(dst0xmm, msk);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_mask_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
        sse2_muldiv255_2x2W(a0xmm, a0xmm, dst0xmm, a1xmm, a1xmm, dst1xmm);

        sse2_load16a(dst0xmm, dst);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_fill_alpha_2x2W(src0xmm, src1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
        sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

vMaskAlphaDenseSkip4:
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
        __m128i a0xmm;

        uint32_t src0;
        uint32_t msk0;

        msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip1;

        src0 = READ_32(src);
        if (RasterUtil::isAlpha0x00_ARGB32(src0)) goto vMaskAlphaSparseSkip1;

        msk0 = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);
        src0xmm = _mm_cvtsi32_si128(src0);
        if (msk0 == 0xFF) goto vMaskAlphaSparseFill1;

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_negate_1x1W(a0xmm, a0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

vMaskAlphaSparseFill1:
        sse2_store4(dst, dst0xmm);

vMaskAlphaSparseSkip1:
        dst += 4;
        src += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse)

      SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse)
        __m128i src0xmm, src1xmm;
        __m128i dst0xmm, dst1xmm;
        __m128i a0xmm, a1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto vMaskAlphaSparseSkip4;

        sse2_load16u(src0xmm, src);
        SSE2_BLIT_TEST_4_ARGB_PIXELS_MASK(src0xmm, dst0xmm, dst1xmm, msk0,
          vMaskAlphaSparseFill4,
          vMaskAlphaSparseSkip4)

        dst0xmm = _mm_cvtsi32_si128(msk0);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_expand_mask_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
        sse2_muldiv255_2x2W(a0xmm, a0xmm, dst0xmm, a1xmm, a1xmm, dst1xmm);

        sse2_load16a(dst0xmm, dst);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_fill_alpha_2x2W(src0xmm, src1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
        sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

vMaskAlphaSparseFill4:
        sse2_store16a(dst, dst0xmm);

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
  // [CompositeSrcOverSSE2 - XRGB32 - VBlit - XRGB32]
  // --------------------------------------------------------------------------

  // Redirect to:
  //   CompositeSrcSSE2 - XRGB32 - VBlit - XRGB32.
};

} // RasterEngine namespace
} // Fog namespace
