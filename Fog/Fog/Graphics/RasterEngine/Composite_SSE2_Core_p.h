// [Fog-Graphics]
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
// [Fog::RasterEngine::SSE2 - Composite - Common]
// ============================================================================

//! @internal
template<typename OP>
struct CompositeCommonSSE2
{
  // Packed to unpacked helpers.

  static FOG_INLINE void prgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i src0xmm;

    sse2_unpack_1x1W(dst0xmm, a0xmm);
    sse2_unpack_1x1W(src0xmm, b0xmm);
    OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
    sse2_pack_1x1W(dst0xmm, dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i src0xmm;

    sse2_unpack_1x1W(dst0xmm, a0xmm);
    sse2_unpack_1x1W(src0xmm, b0xmm);
    OP::prgb32_op_xrgb32_0xXX_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
    sse2_pack_1x1W(dst0xmm, dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i src0xmm;

    sse2_unpack_1x1W(dst0xmm, a0xmm);
    sse2_unpack_1x1W(src0xmm, b0xmm);
    OP::prgb32_op_xrgb32_0xFF_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
    sse2_pack_1x1W(dst0xmm, dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_PREF_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB)
      OP::prgb32_op_xrgb32_0xFF_packed_1x1B(dst0xmm, a0xmm, b0xmm);
    else
      OP::prgb32_op_xrgb32_0xXX_packed_1x1B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i src0xmm;

    sse2_unpack_1x1W(dst0xmm, a0xmm);
    sse2_unpack_1x1W(src0xmm, b0xmm);
    OP::xrgb32_op_xrgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
    sse2_pack_1x1W(dst0xmm, dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i src0xmm;

    sse2_unpack_1x1W(dst0xmm, a0xmm);
    sse2_unpack_1x1W(src0xmm, b0xmm);
    OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
    sse2_pack_1x1W(dst0xmm, dst0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i dst1xmm;
    __m128i src0xmm;
    __m128i src1xmm;

    sse2_unpack_2x2W(dst0xmm, dst1xmm, a0xmm);
    sse2_unpack_2x2W(src0xmm, src1xmm, b0xmm);
    OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
    sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i dst1xmm;
    __m128i src0xmm;
    __m128i src1xmm;

    sse2_unpack_2x2W(dst0xmm, dst1xmm, a0xmm);
    sse2_unpack_2x2W(src0xmm, src1xmm, b0xmm);
    OP::prgb32_op_xrgb32_0xXX_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
    sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i dst1xmm;
    __m128i src0xmm;
    __m128i src1xmm;

    sse2_unpack_2x2W(dst0xmm, dst1xmm, a0xmm);
    sse2_unpack_2x2W(src0xmm, src1xmm, b0xmm);
    OP::prgb32_op_xrgb32_0xFF_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
    sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_PREF_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB)
      OP::prgb32_op_xrgb32_0xFF_packed_1x4B(dst0xmm, a0xmm, b0xmm);
    else
      OP::prgb32_op_xrgb32_0xXX_packed_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i dst1xmm;
    __m128i src0xmm;
    __m128i src1xmm;

    sse2_unpack_2x2W(dst0xmm, dst1xmm, a0xmm);
    sse2_unpack_2x2W(src0xmm, src1xmm, b0xmm);
    OP::xrgb32_op_xrgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
    sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i dst1xmm;
    __m128i src0xmm;
    __m128i src1xmm;

    sse2_unpack_2x2W(dst0xmm, dst1xmm, a0xmm);
    sse2_unpack_2x2W(src0xmm, src1xmm, b0xmm);
    OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
    sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
  }

  // Unpacked helpers.

  static FOG_INLINE void prgb32_op_xrgb32_PREF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB)
      OP::prgb32_op_xrgb32_0xFF_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
    else
      OP::prgb32_op_xrgb32_0xXX_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);

  }

  static FOG_INLINE void prgb32_op_xrgb32_PREF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB)
      OP::prgb32_op_xrgb32_0xFF_unpacked_2x2W(dst0xmm, a0xmm, b0xmm, dst1xmm, a1xmm, b1xmm);
    else
      OP::prgb32_op_xrgb32_0xXX_unpacked_2x2W(dst0xmm, a0xmm, b0xmm, dst1xmm, a1xmm, b1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Base Funcs]
// ============================================================================

//! @internal
template<typename OP>
struct CompositeBaseFuncsSSE2
{
  // --------------------------------------------------------------------------
  // [CompositeBaseFuncsSSE2 - PRGB32 - CBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cblit_prgb32_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    __m128i src0xmm;
    sse2_load4(src0xmm, &src->prgb);

    if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
    {
      sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    }
    else
    {
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);
    }

    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
        sse2_store4(dst, dst0xmm);
      }
      else
      {
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i dst0xmm;
      sse2_load16a(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        __m128i dst1xmm;
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      }
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
  }

  static void FOG_FASTCALL prgb32_cblit_prgb32_span(
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

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0orig);
          sse2_store4(dst, dst0xmm);
        }
        else
        {
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i dst0xmm;
        sse2_load16a(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          __m128i dst1xmm;
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        }
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        __m128i msk0xmm;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x2W(msk0xmm, msk0xmm, src0xmm);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          sse2_pack_1x1W(msk0xmm, msk0xmm);
        }

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_Bound_)
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            __m128i dst0xmm;

            sse2_load4(dst0xmm, dst);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, msk0xmm);
            sse2_store4(dst, dst0xmm);
          }
          else
          {
            __m128i dst0xmm;

            sse2_load4(dst0xmm, dst);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
            sse2_store4(dst, dst0xmm);
          }

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_Bound_)
          __m128i dst0xmm;
          sse2_load16a(dst0xmm, dst);
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            __m128i dst1xmm;
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_expand_mask_1x1W(inv0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(inv0xmm, inv0xmm);
        sse2_muldiv255_1x2W(msk0xmm, src0xmm, inv0xmm);
        sse2_negate_1x2W(inv0xmm, inv0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i dst0xmm;
          __m128i tmp0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_muldiv255_1x1W(tmp0xmm, dst0xmm, inv0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);

          sse2_adds_1x1W(dst0xmm, dst0xmm, tmp0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i dst0xmm, dst1xmm;
          __m128i tmp0xmm, tmp1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(tmp0xmm, dst0xmm, inv0xmm, tmp1xmm, dst1xmm, inv0xmm);
          sse2_pack_2x2W(tmp0xmm, tmp0xmm, tmp1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, tmp0xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_Bound_)
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(msk0xmm, msk0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_Bound_)
          __m128i dst0xmm;
          __m128i msk0xmm, msk1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_load4(msk0xmm, msk);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            __m128i dst1xmm;
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i dst0xmm;
          __m128i msk0xmm;
          __m128i inv0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_negate_1x1W(inv0xmm, msk0xmm);
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
          sse2_muldiv255_1x1W(inv0xmm, inv0xmm, dst0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);

          sse2_adds_1x1W(dst0xmm, dst0xmm, inv0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;
          __m128i inv0xmm, inv1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_load4(msk0xmm, msk);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_negate_2x2W(inv0xmm, msk0xmm, inv1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
          sse2_muldiv255_2x2W(inv0xmm, inv0xmm, dst0xmm, inv1xmm, inv1xmm, dst1xmm);
          sse2_pack_2x2W(inv0xmm, inv0xmm, inv1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, inv0xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_Bound_Skip1;

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(msk0xmm, msk0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip1:
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i dst0xmm;
          __m128i msk0xmm, msk1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_Bound_Skip4;

          sse2_load16a(dst0xmm, dst);
          msk0xmm = _mm_cvtsi32_si128(msk0);

          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            __m128i dst1xmm;
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip4:
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i dst0xmm;
          __m128i msk0xmm;
          __m128i inv0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_UnBoundIn_Skip1;

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_negate_1x1W(inv0xmm, msk0xmm);
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
          sse2_muldiv255_1x1W(inv0xmm, inv0xmm, dst0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);

          sse2_adds_1x1W(dst0xmm, dst0xmm, inv0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip1:
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;
          __m128i inv0xmm, inv1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_UnBoundIn_Skip4;

          sse2_load16a(dst0xmm, dst);
          if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          msk0xmm = _mm_cvtsi32_si128(msk0);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_negate_2x2W(inv0xmm, msk0xmm, inv1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
          sse2_muldiv255_2x2W(inv0xmm, inv0xmm, dst0xmm, inv1xmm, inv1xmm, dst1xmm);
          sse2_pack_2x2W(inv0xmm, inv0xmm, inv1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, inv0xmm);
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip4:
          dst += 16;
          msk += 4;
          SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse_UnBoundIn_)

vMaskAlphaSparse_UnBoundIn_NoMask4:
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0orig);
          }
          else
          {
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
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
  // [CompositeBaseFuncsSSE2 - PRGB32 - VBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_prgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
      }
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        __m128i src1xmm;
        __m128i dst1xmm;

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      }
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
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

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
        }
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          __m128i src1xmm;
          __m128i dst1xmm;

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        }
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        __m128i msk0xmm;
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_negate_1x2W(inv0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i tmp0xmm = msk0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_negate_1x2W_hi(tmp0xmm, tmp0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, tmp0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i tmp0xmm, tmp1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

          tmp0xmm = inv0xmm;
          tmp1xmm = inv0xmm;

          sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, dst0xmm, tmp1xmm, tmp1xmm, dst1xmm);
          sse2_pack_2x2W(tmp0xmm, tmp0xmm, tmp1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, tmp0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          {
            __m128i msk0xmm, msk1xmm;
            sse2_load4(msk0xmm, msk);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_UnBoundIn_)
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
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_Bound_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            if (msk0 == 0xFF) goto vMaskAlphaSparse_Bound_Packed_Fill1;
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
vMaskAlphaSparse_Bound_Packed_Fill1:
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_Bound_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            __m128i msk0xmm, msk1xmm;

            if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparse_Bound_PackedFill4;
            msk0xmm = _mm_cvtsi32_si128(msk0);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);

            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
vMaskAlphaSparse_Bound_PackedFill4:
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;
            __m128i msk0xmm, msk1xmm;

            msk0xmm = _mm_cvtsi32_si128(msk0);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_UnBoundIn_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_UnBoundIn_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          if ((OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          msk0xmm = _mm_cvtsi32_si128(msk0);

          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
          SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse_UnBoundIn_)

vMaskAlphaSparse_UnBoundIn_NoMask4:
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
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
  // [CompositeBaseFuncsSSE2 - PRGB32 - VBlit - ARGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);
        OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
      }
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        __m128i src1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

        OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        __m128i src1xmm;
        __m128i dst1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

        OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      }
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
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
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_premultiply_1x1W(src0xmm, src0xmm);
          sse2_pack_1x1W(src0xmm, src0xmm);
          OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_premultiply_1x1W(src0xmm, src0xmm);
          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
        }
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          __m128i src1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
          sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

          OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          __m128i src1xmm;
          __m128i dst1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        }
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        __m128i msk0xmm;
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          sse2_unpack_1x1W(src0xmm, src0xmm);

          {
            __m128i a0xmm;

            sse2_expand_alpha_1x1W(a0xmm, src0xmm);
            sse2_fill_alpha_1x1W(src0xmm);
            sse2_muldiv255_1x1W(a0xmm, a0xmm, msk0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          // Multiply source alpha with mask and premultiply pixel by result.
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            __m128i tmp0xmm, tmp1xmm;

            tmp1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            tmp1xmm = _mm_shufflehi_epi16(tmp1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            tmp1xmm = _mm_srli_epi16(tmp1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(tmp1xmm, tmp1xmm, msk0xmm);

            tmp0xmm = _mm_shuffle_epi32(tmp1xmm, _MM_SHUFFLE(3, 3, 2, 2));
            tmp0xmm = _mm_shuffle_epi32(tmp0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, tmp0xmm, src1xmm, src1xmm, tmp1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_negate_1x2W(inv0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i tmp0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          tmp0xmm = msk0xmm;
          sse2_negate_1x2W_hi(tmp0xmm, tmp0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, tmp0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i tmp0xmm, tmp1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          // Multiply source alpha with mask and premultiply pixel by result.
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            tmp0xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            tmp0xmm = _mm_shufflehi_epi16(tmp0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            tmp0xmm = _mm_srli_epi16(tmp0xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(tmp0xmm, tmp0xmm, msk0xmm);

            tmp1xmm = _mm_shuffle_epi32(tmp0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            tmp0xmm = _mm_shuffle_epi32(tmp0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, tmp0xmm, src1xmm, src1xmm, tmp1xmm);
          }

          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          tmp0xmm = dst0xmm;
          tmp1xmm = dst1xmm;

          sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, inv0xmm, tmp1xmm, tmp1xmm, inv0xmm);
          sse2_pack_2x2W(tmp0xmm, tmp0xmm, tmp1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, tmp0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          {
            __m128i tmp0xmm;
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_expand_mask_1x1W(msk0xmm, msk0);
            sse2_expand_alpha_1x1W(tmp0xmm, src0xmm);
            sse2_fill_alpha_1x1W(src0xmm);
            sse2_muldiv255_1x1W(msk0xmm, msk0xmm, tmp0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          // Multiply source alpha with mask and premultiply pixel by result. 
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            __m128i msk0xmm, msk1xmm;

            sse2_load4(msk0xmm, msk);
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_load4(msk0xmm, msk);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          // Multiply source alpha with mask and premultiply pixel by result.
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto cMaskAlphaSparse_Bound_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          {
            __m128i tmp0xmm;
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_expand_mask_1x1W(msk0xmm, msk0);
            sse2_expand_alpha_1x1W(tmp0xmm, src0xmm);
            sse2_fill_alpha_1x1W(src0xmm);
            sse2_muldiv255_1x1W(msk0xmm, msk0xmm, tmp0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

cMaskAlphaSparse_Bound_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_Bound_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          // Multiply source alpha with mask and premultiply pixel by result. 
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            __m128i msk0xmm, msk1xmm;

            SSE2_BLIT_TEST_4_ARGB_PIXELS_FULL(src0xmm, msk0xmm, msk0 == 0xFFFFFFFF, 
              vMaskAlphaSparse_Bound_Packed_NoMask4);

            msk0xmm = _mm_cvtsi32_si128(msk0);
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparse_Bound_Packed_NoMask4:
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i msk0xmm, msk1xmm;
            __m128i dst1xmm;

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);

            SSE2_BLIT_TEST_4_ARGB_PIXELS_FULL(src0xmm, msk0xmm, msk0 == 0xFFFFFFFF, 
              vMaskAlphaSparse_Bound_Unpacked_NoMask4);

            msk0xmm = _mm_cvtsi32_si128(msk0);
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparse_Bound_Unpacked_NoMask4:
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_UnBoundIn_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_UnBoundIn_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          if ((OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          msk0xmm = _mm_cvtsi32_si128(msk0);

          // Multiply source alpha with mask and premultiply pixel by result.
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
          SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse_UnBoundIn_)

vMaskAlphaSparse_UnBoundIn_NoMask4:
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
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
  // [CompositeBaseFuncsSSE2 - PRGB32 - VBlit - XRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_vblit_xrgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB) sse2_fill_alpha_1x4B(src0xmm);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::prgb32_op_xrgb32_PREF_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        OP::prgb32_op_xrgb32_PREF_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
      }
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB) sse2_fill_alpha_1x4B(src0xmm);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::prgb32_op_xrgb32_PREF_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        __m128i src1xmm;
        __m128i dst1xmm;

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        OP::prgb32_op_xrgb32_PREF_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      }
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
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
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB) sse2_fill_alpha_1x4B(src0xmm);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::prgb32_op_xrgb32_PREF_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          OP::prgb32_op_xrgb32_PREF_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
        }
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB) sse2_fill_alpha_1x4B(src0xmm);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::prgb32_op_xrgb32_PREF_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          __m128i src1xmm;
          __m128i dst1xmm;

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          OP::prgb32_op_xrgb32_PREF_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        }
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        __m128i msk0xmm;
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          sse2_fill_alpha_1x4B(src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          sse2_fill_alpha_1x4B(src0xmm);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_negate_1x2W(inv0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i tmp0xmm = msk0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          sse2_fill_alpha_1x4B(src0xmm);

          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_negate_1x2W_hi(tmp0xmm, tmp0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, tmp0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i tmp0xmm, tmp1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          sse2_fill_alpha_1x4B(src0xmm);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

          tmp0xmm = inv0xmm;
          tmp1xmm = inv0xmm;

          sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, dst0xmm, tmp1xmm, tmp1xmm, dst1xmm);
          sse2_pack_2x2W(tmp0xmm, tmp0xmm, tmp1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, tmp0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          {
            __m128i msk0xmm, msk1xmm;
            sse2_load4(msk0xmm, msk);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_load4(msk0xmm, msk);

          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_Bound_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::prgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_Bound_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          {
            __m128i msk0xmm, msk1xmm;
            sse2_load4(msk0xmm, msk);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::prgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_UnBoundIn_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::prgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_UnBoundIn_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);

          if ((OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);

          if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          msk0xmm = _mm_cvtsi32_si128(msk0);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
          SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse_UnBoundIn_)

vMaskAlphaSparse_UnBoundIn_NoMask4:
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::prgb32_op_xrgb32_0xFF_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            OP::prgb32_op_xrgb32_0xFF_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
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
  // [CompositeBaseFuncsSSE2 - XRGB32 - CBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cblit_prgb32_full(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    __m128i src0xmm;
    sse2_load4(src0xmm, &src->prgb);

    if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
    {
      sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    }
    else
    {
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);
    }

    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
        sse2_store4(dst, dst0xmm);
      }
      else
      {
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i dst0xmm;
      sse2_load16a(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        __m128i dst1xmm;
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      }
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
  }

  static void FOG_FASTCALL xrgb32_cblit_prgb32_span(
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

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0orig);
          sse2_store4(dst, dst0xmm);
        }
        else
        {
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i dst0xmm;
        sse2_load16a(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0orig);
        }
        else
        {
          __m128i dst1xmm;
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        }
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        __m128i msk0xmm;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x2W(msk0xmm, msk0xmm, src0xmm);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          sse2_pack_1x1W(msk0xmm, msk0xmm);
        }

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_Bound_)
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            __m128i dst0xmm;

            sse2_load4(dst0xmm, dst);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, msk0xmm);
            sse2_store4(dst, dst0xmm);
          }
          else
          {
            __m128i dst0xmm;

            sse2_load4(dst0xmm, dst);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
            sse2_store4(dst, dst0xmm);
          }

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_Bound_)
          __m128i dst0xmm;
          sse2_load16a(dst0xmm, dst);
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            __m128i dst1xmm;
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_expand_mask_1x1W(inv0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(inv0xmm, inv0xmm);
        sse2_muldiv255_1x2W(msk0xmm, src0xmm, inv0xmm);
        sse2_negate_1x2W(inv0xmm, inv0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i dst0xmm;
          __m128i tmp0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_muldiv255_1x1W(tmp0xmm, dst0xmm, inv0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);

          sse2_adds_1x1W(dst0xmm, dst0xmm, tmp0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i dst0xmm, dst1xmm;
          __m128i tmp0xmm, tmp1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(tmp0xmm, dst0xmm, inv0xmm, tmp1xmm, dst1xmm, inv0xmm);
          sse2_pack_2x2W(tmp0xmm, tmp0xmm, tmp1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, tmp0xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_Bound_)
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(msk0xmm, msk0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_Bound_)
          __m128i dst0xmm;
          __m128i msk0xmm, msk1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_load4(msk0xmm, msk);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            __m128i dst1xmm;
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i dst0xmm;
          __m128i msk0xmm;
          __m128i inv0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_negate_1x1W(inv0xmm, msk0xmm);
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
          sse2_muldiv255_1x1W(inv0xmm, inv0xmm, dst0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);

          sse2_adds_1x1W(dst0xmm, dst0xmm, inv0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;
          __m128i inv0xmm, inv1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_load4(msk0xmm, msk);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_negate_2x2W(inv0xmm, msk0xmm, inv1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
          sse2_muldiv255_2x2W(inv0xmm, inv0xmm, dst0xmm, inv1xmm, inv1xmm, dst1xmm);
          sse2_pack_2x2W(inv0xmm, inv0xmm, inv1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, inv0xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    C_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_Bound_Skip1;

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(msk0xmm, msk0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip1:
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i dst0xmm;
          __m128i msk0xmm, msk1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_Bound_Skip4;

          sse2_load16a(dst0xmm, dst);
          msk0xmm = _mm_cvtsi32_si128(msk0);

          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, msk0xmm);
          }
          else
          {
            __m128i dst1xmm;
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip4:
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i dst0xmm;
          __m128i msk0xmm;
          __m128i inv0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_UnBoundIn_Skip1;

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_negate_1x1W(inv0xmm, msk0xmm);
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
          sse2_muldiv255_1x1W(inv0xmm, inv0xmm, dst0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, msk0xmm);

          sse2_adds_1x1W(dst0xmm, dst0xmm, inv0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip1:
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;
          __m128i inv0xmm, inv1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_UnBoundIn_Skip4;

          sse2_load16a(dst0xmm, dst);
          if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          msk0xmm = _mm_cvtsi32_si128(msk0);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_negate_2x2W(inv0xmm, msk0xmm, inv1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
          sse2_muldiv255_2x2W(inv0xmm, inv0xmm, dst0xmm, inv1xmm, inv1xmm, dst1xmm);
          sse2_pack_2x2W(inv0xmm, inv0xmm, inv1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, inv0xmm);
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip4:
          dst += 16;
          msk += 4;
          SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse_UnBoundIn_)

vMaskAlphaSparse_UnBoundIn_NoMask4:
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0orig);
          }
          else
          {
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
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
  // [CompositeBaseFuncsSSE2 - XRGB32 - VBlit - PRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_prgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
      }
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        __m128i src1xmm;
        __m128i dst1xmm;

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      }
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
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

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
        }
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          __m128i src1xmm;
          __m128i dst1xmm;

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        }
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        __m128i msk0xmm;
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_negate_1x2W(inv0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i tmp0xmm = msk0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_negate_1x2W_hi(tmp0xmm, tmp0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, tmp0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i tmp0xmm, tmp1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

          tmp0xmm = inv0xmm;
          tmp1xmm = inv0xmm;

          sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, dst0xmm, tmp1xmm, tmp1xmm, dst1xmm);
          sse2_pack_2x2W(tmp0xmm, tmp0xmm, tmp1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, tmp0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          {
            __m128i msk0xmm, msk1xmm;
            sse2_load4(msk0xmm, msk);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_UnBoundIn_)
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
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_Bound_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            if (msk0 == 0xFF) goto vMaskAlphaSparse_Bound_Packed_Fill1;
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
vMaskAlphaSparse_Bound_Packed_Fill1:
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_Bound_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            __m128i msk0xmm, msk1xmm;

            if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparse_Bound_PackedFill4;
            msk0xmm = _mm_cvtsi32_si128(msk0);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);

            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
vMaskAlphaSparse_Bound_PackedFill4:
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;
            __m128i msk0xmm, msk1xmm;

            msk0xmm = _mm_cvtsi32_si128(msk0);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_UnBoundIn_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_UnBoundIn_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          if (msk0 == 0xFFFFFFFF) goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          msk0xmm = _mm_cvtsi32_si128(msk0);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
          SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse_UnBoundIn_)

vMaskAlphaSparse_UnBoundIn_NoMask4:
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
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
  // [CompositeBaseFuncsSSE2 - XRGB32 - VBlit - ARGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_argb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        sse2_pack_1x1W(src0xmm, src0xmm);
        OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);
        OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
      }
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        __m128i src1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

        OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        __m128i src1xmm;
        __m128i dst1xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

        OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      }
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
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
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_premultiply_1x1W(src0xmm, src0xmm);
          sse2_pack_1x1W(src0xmm, src0xmm);
          OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_premultiply_1x1W(src0xmm, src0xmm);
          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
        }
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          __m128i src1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
          sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

          OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          __m128i src1xmm;
          __m128i dst1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        }
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        __m128i msk0xmm;
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          sse2_unpack_1x1W(src0xmm, src0xmm);

          {
            __m128i a0xmm;

            sse2_expand_alpha_1x1W(a0xmm, src0xmm);
            sse2_fill_alpha_1x1W(src0xmm);
            sse2_muldiv255_1x1W(a0xmm, a0xmm, msk0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          // Multiply source alpha with mask and premultiply pixel by result.
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            __m128i tmp0xmm, tmp1xmm;

            tmp1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            tmp1xmm = _mm_shufflehi_epi16(tmp1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            tmp1xmm = _mm_srli_epi16(tmp1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(tmp1xmm, tmp1xmm, msk0xmm);

            tmp0xmm = _mm_shuffle_epi32(tmp1xmm, _MM_SHUFFLE(3, 3, 2, 2));
            tmp0xmm = _mm_shuffle_epi32(tmp0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, tmp0xmm, src1xmm, src1xmm, tmp1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_negate_1x2W(inv0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i tmp0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          tmp0xmm = msk0xmm;
          sse2_negate_1x2W_hi(tmp0xmm, tmp0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, tmp0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i tmp0xmm, tmp1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          // Multiply source alpha with mask and premultiply pixel by result.
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            tmp0xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            tmp0xmm = _mm_shufflehi_epi16(tmp0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            tmp0xmm = _mm_srli_epi16(tmp0xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(tmp0xmm, tmp0xmm, msk0xmm);

            tmp1xmm = _mm_shuffle_epi32(tmp0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            tmp0xmm = _mm_shuffle_epi32(tmp0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, tmp0xmm, src1xmm, src1xmm, tmp1xmm);
          }

          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          tmp0xmm = dst0xmm;
          tmp1xmm = dst1xmm;

          sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, inv0xmm, tmp1xmm, tmp1xmm, inv0xmm);
          sse2_pack_2x2W(tmp0xmm, tmp0xmm, tmp1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, tmp0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          {
            __m128i tmp0xmm;
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_expand_mask_1x1W(msk0xmm, msk0);
            sse2_expand_alpha_1x1W(tmp0xmm, src0xmm);
            sse2_fill_alpha_1x1W(src0xmm);
            sse2_muldiv255_1x1W(msk0xmm, msk0xmm, tmp0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          // Multiply source alpha with mask and premultiply pixel by result. 
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            __m128i msk0xmm, msk1xmm;

            sse2_load4(msk0xmm, msk);
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_load4(msk0xmm, msk);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          // Multiply source alpha with mask and premultiply pixel by result.
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto cMaskAlphaSparse_Bound_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          {
            __m128i tmp0xmm;
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_expand_mask_1x1W(msk0xmm, msk0);
            sse2_expand_alpha_1x1W(tmp0xmm, src0xmm);
            sse2_fill_alpha_1x1W(src0xmm);
            sse2_muldiv255_1x1W(msk0xmm, msk0xmm, tmp0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

cMaskAlphaSparse_Bound_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_Bound_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          // Multiply source alpha with mask and premultiply pixel by result. 
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            __m128i msk0xmm, msk1xmm;

            SSE2_BLIT_TEST_4_ARGB_PIXELS_FULL(src0xmm, msk0xmm, msk0 == 0xFFFFFFFF, 
              vMaskAlphaSparse_Bound_Packed_NoMask4);

            msk0xmm = _mm_cvtsi32_si128(msk0);
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparse_Bound_Packed_NoMask4:
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i msk0xmm, msk1xmm;
            __m128i dst1xmm;

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);

            SSE2_BLIT_TEST_4_ARGB_PIXELS_FULL(src0xmm, msk0xmm, msk0 == 0xFFFFFFFF, 
              vMaskAlphaSparse_Bound_Unpacked_NoMask4);

            msk0xmm = _mm_cvtsi32_si128(msk0);
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);

vMaskAlphaSparse_Bound_Unpacked_NoMask4:
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_UnBoundIn_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_UnBoundIn_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          if ((OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          msk0xmm = _mm_cvtsi32_si128(msk0);

          // Multiply source alpha with mask and premultiply pixel by result.
          // Here is used optimization to save one multiply when using classic
          // approach (multiply 2x2W, premultiply 2x2W).
          {
            sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0xmm);

            msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
            msk1xmm = _mm_srli_epi16(msk1xmm, 8);

            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

            msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
            msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
          SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse_UnBoundIn_)

vMaskAlphaSparse_UnBoundIn_NoMask4:
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
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
  // [CompositeBaseFuncsSSE2 - XRGB32 - VBlit - XRGB32]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_vblit_xrgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT()

    SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB) sse2_fill_alpha_1x4B(src0xmm);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::xrgb32_op_xrgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        OP::xrgb32_op_xrgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
      }
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

    SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB) sse2_fill_alpha_1x4B(src0xmm);

      if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
      {
        OP::xrgb32_op_xrgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
      }
      else
      {
        __m128i src1xmm;
        __m128i dst1xmm;

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        OP::xrgb32_op_xrgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      }
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
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
      SSE2_BLIT_32xX_INIT()

      SSE2_BLIT_32x4_SMALL_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB) sse2_fill_alpha_1x4B(src0xmm);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::xrgb32_op_xrgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          OP::xrgb32_op_xrgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
        }
        sse2_store4(dst, dst0xmm);

        src += 4;
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(cMaskOpaque)

      SSE2_BLIT_32x4_MAIN_BEGIN(cMaskOpaque)
        __m128i src0xmm;
        __m128i dst0xmm;

        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PREFER_FRGB) sse2_fill_alpha_1x4B(src0xmm);

        if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
        {
          OP::xrgb32_op_xrgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
        }
        else
        {
          __m128i src1xmm;
          __m128i dst1xmm;

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          OP::xrgb32_op_xrgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        }
        sse2_store16a(dst, dst0xmm);

        src += 16;
        dst += 16;
      SSE2_BLIT_32x4_MAIN_END(cMaskOpaque)
    }
    // ------------------------------------------------------------------------

    // [CMask - Alpha] --------------------------------------------------------
    V_BLIT_SPAN8_CASE_CMASK_ALPHA()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        __m128i msk0xmm;
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          sse2_fill_alpha_1x4B(src0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          sse2_fill_alpha_1x4B(src0xmm);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_negate_1x2W(inv0xmm, msk0xmm);

        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i tmp0xmm = msk0xmm;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);

          sse2_fill_alpha_1x4B(src0xmm);

          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_negate_1x2W_hi(tmp0xmm, tmp0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, tmp0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(cMaskAlpha_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(cMaskAlpha_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i tmp0xmm, tmp1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          sse2_fill_alpha_1x4B(src0xmm);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

          tmp0xmm = inv0xmm;
          tmp1xmm = inv0xmm;

          sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, dst0xmm, tmp1xmm, tmp1xmm, dst1xmm);
          sse2_pack_2x2W(tmp0xmm, tmp0xmm, tmp1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, tmp0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
        SSE2_BLIT_32x4_MAIN_END(cMaskAlpha_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Dense] ------------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_DENSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          {
            __m128i msk0xmm, msk1xmm;
            sse2_load4(msk0xmm, msk);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaDense_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaDense_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_load4(msk0xmm, msk);

          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaDense_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
      }
    }
    // ------------------------------------------------------------------------

    // [VMask - Alpha - Sparse] -----------------------------------------------
    V_BLIT_SPAN8_CASE_VMASK_A_SPARSE()
    {
      // Bound: (Dst) OP (Src*Msk).
      if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND))
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_Bound_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            sse2_pack_1x1W(src0xmm, src0xmm);
            OP::xrgb32_op_prgb32_packed_1x1B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            sse2_unpack_1x1W(src0xmm, src0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
            OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
          }
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_Bound_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_Bound_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_Bound_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);

          {
            __m128i msk0xmm, msk1xmm;
            sse2_load4(msk0xmm, msk);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
            sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
            sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          }

          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
            OP::xrgb32_op_prgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            __m128i dst1xmm;

            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_Bound_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_Bound_)
      }
      // Unbound: (Dst) OP (Src*Msk) + Dst*(1-Msk).
      else if (OP::CHARACTERISTICS & OPERATOR_CHAR_UNBOUND_MSK_IN)
      {
        SSE2_BLIT_32xX_INIT()

        SSE2_BLIT_32x4_SMALL_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm;
          __m128i dst0xmm;
          __m128i msk0xmm;

          uint32_t msk0 = READ_8(msk);
          if (msk0 == 0x00) goto vMaskAlphaSparse_UnBoundIn_Skip1;

          sse2_load4(src0xmm, src);
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          src0xmm = _mm_or_si128(src0xmm, _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2)));
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          sse2_negate_1x2W_hi(msk0xmm, msk0xmm);

          // HI = DST * (1 - M)
          // LO = SRC * M
          sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);

          OP::xrgb32_op_prgb32_unpacked_1x1W(dst0xmm, dst0xmm, src0xmm);

          src0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(1, 0, 3, 2));
          sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip1:
          src += 4;
          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(vMaskAlphaSparse_UnBoundIn_)

        SSE2_BLIT_32x4_MAIN_BEGIN(vMaskAlphaSparse_UnBoundIn_)
          __m128i src0xmm, src1xmm;
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;

          uint32_t msk0 = READ_32(msk);
          if (msk0 == 0x00000000) goto vMaskAlphaSparse_UnBoundIn_Skip4;

          sse2_load16u(src0xmm, src);
          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(src0xmm);

          if ((OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);

          if (!(OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED) && msk0 == 0xFFFFFFFF)
            goto vMaskAlphaSparse_UnBoundIn_NoMask4;

          msk0xmm = _mm_cvtsi32_si128(msk0);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
          sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, dst0xmm, msk1xmm, msk1xmm, dst1xmm);
          sse2_pack_2x2W(msk0xmm, msk0xmm, msk1xmm);

          OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msk0xmm);
          sse2_store16a(dst, dst0xmm);

vMaskAlphaSparse_UnBoundIn_Skip4:
          src += 16;
          dst += 16;
          msk += 4;
          SSE2_BLIT_32x4_MAIN_CONTINUE(vMaskAlphaSparse_UnBoundIn_)

vMaskAlphaSparse_UnBoundIn_NoMask4:
          if (OP::CHARACTERISTICS & OPERATOR_CHAR_PACKED)
          {
            OP::xrgb32_op_xrgb32_packed_1x4B(dst0xmm, dst0xmm, src0xmm);
          }
          else
          {
            OP::xrgb32_op_xrgb32_unpacked_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          }
          sse2_store16a(dst, dst0xmm);

          src += 16;
          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_MAIN_END(vMaskAlphaSparse_UnBoundIn_)
      }
      // Unbound: ((Dst) OP (Src))*Msk + Dst*(1-Msk).
      else 
      {
        // TODO.
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

} // RasterEngine namespace
} // Fog namespace
