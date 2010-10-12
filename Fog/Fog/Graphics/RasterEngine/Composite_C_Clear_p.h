// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Composite - Clear]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeClearC
{
  enum { CHARACTERISTICS = OPERATOR_CLEAR };

  // --------------------------------------------------------------------------
  // [CompositeClearC - PRGB32 - Helpers]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_span_inline(uint8_t* dst, const Span* span)
  {
    // [Begin] ----------------------------------------------------------------
    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        ((uint32_t*)dst)[0] = 0x00000000;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      msk0 = Face::b32_1x1Negate255(msk0);

      do {
        uint32_t dst0, dst1;
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        ((uint32_t*)dst)[0] = Face::b32_2x2MulDiv255U_Pack0213(dst0, dst1, msk0);

        dst += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        uint32_t dst0, dst1;
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        uint32_t msk0 = Face::b32_1x1Negate255(READ_8(msk));
        ((uint32_t*)dst)[0] = Face::b32_2x2MulDiv255U_Pack0213(dst0, dst1, msk0);

        dst += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      do {
        uint32_t dst0, dst1;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        msk0 = Face::b32_1x1Negate255(msk0);
        if (msk0 == 0x00) goto vMaskAlphaSparseFill;

        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        msk0 = Face::b32_2x2MulDiv255U_Pack0213(dst0, dst1, msk0);

vMaskAlphaSparseFill:
        ((uint32_t*)dst)[0] = msk0;

vMaskAlphaSparseSkip:
        dst += 4;
        msk += 1;
      } while (--w);
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
  // [CompositeClearC - PRGB32 - CBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    FOG_UNUSED(src);

    do {
      ((uint32_t*)dst)[0] = 0;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    prgb32_span_inline(dst, span);
  }

  // --------------------------------------------------------------------------
  // [CompositeClearC - PRGB32 - VBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_UNUSED(src);

    do {
      ((uint32_t*)dst)[0] = 0;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_vblit_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    prgb32_span_inline(dst, span);
  }

  // --------------------------------------------------------------------------
  // [CompositeClearC - XRGB32 - Helpers]
  // --------------------------------------------------------------------------

  static FOG_INLINE void xrgb32_span_inline(uint8_t* dst, const Span* span)
  {
    // [Begin] ----------------------------------------------------------------
    C_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        ((uint32_t*)dst)[0] = 0xFF000000;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      msk0 = Face::b32_1x1Negate255(msk0);

      do {
        uint32_t dst0, dst1;
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        ((uint32_t*)dst)[0] = Face::b32_2x2MulDiv255U_Pack0213(dst0, dst1, msk0) | 0xFF000000;

        dst += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        uint32_t dst0, dst1;
        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        uint32_t msk0 = Face::b32_1x1Negate255(READ_8(msk));
        ((uint32_t*)dst)[0] = Face::b32_2x2MulDiv255U_Pack0213(dst0, dst1, msk0) | 0xFF000000;

        dst += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      do {
        uint32_t dst0, dst1;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        msk0 = Face::b32_1x1Negate255(msk0);
        if (msk0 == 0x00) goto vMaskAlphaSparseFill;

        Face::b32_2x2Unpack0213(dst0, dst1, READ_32(dst));
        msk0 = Face::b32_2x2MulDiv255U_Pack0213(dst0, dst1, msk0);

vMaskAlphaSparseFill:
        ((uint32_t*)dst)[0] = msk0 | 0xFF000000;

vMaskAlphaSparseSkip:
        dst += 4;
        msk += 1;
      } while (--w);
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
  // [CompositeClearC - XRGB32 - CBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cblit_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    FOG_UNUSED(src);

    do {
      ((uint32_t*)dst)[0] = 0xFF000000;
    } while (--w);
  }

  static void FOG_FASTCALL xrgb32_cblit_span(
    uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure)
  {
    xrgb32_span_inline(dst, span);
  }

  // --------------------------------------------------------------------------
  // [CompositeClearC - XRGB32 - VBlit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_UNUSED(src);

    do {
      ((uint32_t*)dst)[0] = 0xFF000000;
    } while (--w);
  }

  static void FOG_FASTCALL xrgb32_vblit_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    xrgb32_span_inline(dst, span);
  }
};

} // RasterEngine namespace
} // Fog namespace
