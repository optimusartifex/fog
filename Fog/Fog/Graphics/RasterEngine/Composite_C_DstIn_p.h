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
// [Fog::RasterEngine::C - Composite - DstIn]
// ============================================================================

//! @internal
struct FOG_HIDDEN CompositeDstInC : public CompositeBaseFuncsC32<CompositeDstInC>
{
  enum { CHARACTERISTICS = OPERATOR_CHAR_DST_IN };

  // --------------------------------------------------------------------------
  // [CompositeDstInC - Generic]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_op_prgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    Face::b32_2x2MulDiv255U(dst0, a0, dst1, a1, Face::b32_1x2GetB1(b1));
  }

  static FOG_INLINE void prgb32_op_xrgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    Face::b32_2x2MulDiv255U(dst0, a0, dst1, a1, Face::b32_1x2GetB1(b1));
  }

  static FOG_INLINE void xrgb32_op_xrgb32_32b(
    Face::b32_1x2& dst0, Face::b32_1x2 a0, Face::b32_1x2 b0,
    Face::b32_1x2& dst1, Face::b32_1x2 a1, Face::b32_1x2 b1)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  // --------------------------------------------------------------------------
  // [CompositeDstInC - PRGB32 - VBlit - PRGB32/ARGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_prgb32_or_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      uint32_t dst0;
      uint32_t srca;

      srca = (uint32_t)READ_32(src) >> 24;
      if (srca == 0xFF) goto cMaskOpaqueSkip;

      dst0 = READ_32(dst);
      dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);
      ((uint32_t*)dst)[0] = dst0;

cMaskOpaqueSkip:
      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_vblit_prgb32_or_argb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = (uint32_t)READ_32(src) >> 24;
        if (srca == 0xFF) goto cMaskOpaqueSkip;

        dst0 = READ_32(dst);
        dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

cMaskOpaqueSkip:
        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = Face::b32_1x1MulDiv255((READ_32(src) >> 24), msk0);
        dst0 = READ_32(dst);
        dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);

        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = Face::b32_1x1MulDiv255((READ_32(src) >> 24), READ_8(msk));

        dst0 = READ_32(dst);
        dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        dst0 = READ_32(dst);
        srca = Face::b32_1x1MulDiv255((READ_32(src) >> 24), msk0);
        dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

vMaskAlphaSparseSkip:
        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
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
  // [CompositeDstOutC - PRGB32 - VSpan - A8]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_a8_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      uint32_t dst0;
      uint32_t srca;

      srca = READ_8(src);
      if (srca == 0xFF) goto cMaskOpaqueSkip;

      dst0 = READ_32(dst);
      dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);
      ((uint32_t*)dst)[0] = dst0;

cMaskOpaqueSkip:
      dst += 4;
      src += 1;
    } while (--w);
  }

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
        uint32_t dst0;
        uint32_t srca;

        srca = READ_8(src);
        if (srca == 0xFF) goto cMaskOpaqueSkip;

        dst0 = READ_32(dst);
        dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

cMaskOpaqueSkip:
        dst += 4;
        src += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = Face::b32_1x1MulDiv255(READ_8(src), msk0);

        dst0 = READ_32(dst);
        dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = Face::b32_1x1MulDiv255(READ_8(src), READ_8(msk));
        dst0 = READ_32(dst);
        dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);

        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 1;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        srca = Face::b32_1x1MulDiv255(READ_8(src), msk0);
        dst0 = READ_32(dst);
        dst0 = Face::p32_1x4MulDiv255U32(dst0, srca);

        ((uint32_t*)dst)[0] = dst0;

vMaskAlphaSparseSkip:
        dst += 4;
        src += 1;
        msk += 1;
      } while (--w);
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
  // [CompositeDstInC - XRGB32 - VBlit - PRGB32/ARGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_prgb32_or_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      uint32_t dst0;
      uint32_t srca;

      srca = (uint32_t)READ_32(src) >> 24;
      if (srca == 0xFF) goto cMaskOpaqueSkip;

      dst0 = READ_32(dst);
      dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);
      ((uint32_t*)dst)[0] = dst0;

cMaskOpaqueSkip:
      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL xrgb32_vblit_prgb32_or_argb32_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = (uint32_t)READ_32(src) >> 24;
        if (srca == 0xFF) goto cMaskOpaqueSkip;

        dst0 = READ_32(dst);
        dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

cMaskOpaqueSkip:
        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = Face::b32_1x1MulDiv255((READ_32(src) >> 24), msk0);
        dst0 = READ_32(dst);
        dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);

        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 4;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = Face::b32_1x1MulDiv255((READ_32(src) >> 24), READ_8(msk));

        dst0 = READ_32(dst);
        dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        srca = Face::b32_1x1MulDiv255((READ_32(src) >> 24), msk0);

        dst0 = READ_32(dst);
        dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

vMaskAlphaSparseSkip:
        dst += 4;
        src += 4;
        msk += 1;
      } while (--w);
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
  // [CompositeDstOutC - XRGB32 - VSpan - A8]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_a8_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0);

    do {
      uint32_t dst0;
      uint32_t srca;

      srca = READ_8(src);
      if (srca == 0xFF) goto cMaskOpaqueSkip;

      dst0 = READ_32(dst);
      dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);
      ((uint32_t*)dst)[0] = dst0;

cMaskOpaqueSkip:
      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL xrgb32_vblit_a8_span(
    uint8_t* dst, const Span* span, const RasterClosure* closure)
  {
    // [Begin] ----------------------------------------------------------------
    V_BLIT_SPAN8_BEGIN(4)
    // ------------------------------------------------------------------------

    // [CMask - Opaque] -------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_OPAQUE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = READ_8(src);
        if (srca == 0xFF) goto cMaskOpaqueSkip;

        dst0 = READ_32(dst);
        dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

cMaskOpaqueSkip:
        dst += 4;
        src += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = Face::b32_1x1MulDiv255(READ_8(src), msk0);

        dst0 = READ_32(dst);
        dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);
        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        srca = Face::b32_1x1MulDiv255(READ_8(src), READ_8(msk));
        dst0 = READ_32(dst);
        dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);

        ((uint32_t*)dst)[0] = dst0;

        dst += 4;
        src += 1;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      do {
        uint32_t dst0;
        uint32_t srca;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto vMaskAlphaSparseSkip;

        srca = Face::b32_1x1MulDiv255(READ_8(src), msk0);
        dst0 = READ_32(dst);
        dst0 = Face::p32_1x3MulDiv255U32_FXXX(dst0, srca);

        ((uint32_t*)dst)[0] = dst0;

vMaskAlphaSparseSkip:
        dst += 4;
        src += 1;
        msk += 1;
      } while (--w);
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Dense] -------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
    {
      // TODO: VMASK-ARGB.
    }
    // ------------------------------------------------------------------------

    // [VMask - ARGB - Sparse] -------------------------------------------------
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
