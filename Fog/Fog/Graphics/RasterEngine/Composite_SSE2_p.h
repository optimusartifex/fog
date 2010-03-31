// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Build/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_SSE2_p.h>
#include <Fog/Graphics/RasterEngine/Dib_SSE2_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Templates]
// ============================================================================

// Compositing mode.
//
// - "PACKED"        - Compositing is done in packed (8-bit) entities (SRC/ADD OPERATOR).
// - "UNPACKED"      - Need to unpack pixels to 16 bit entities before compositing.
#define OPERATOR_MODE_PACKED 0x0
#define OPERATOR_MODE_UNPACKED 0x1

// Compositing proc (similar to C++ version).
//
// - "ALL"           - Process all pixels (each pixel will change DST - SRC OPERATOR).
// - "SRC_NON_ZERO"  - Only non-zero SRC alpha will modify DST.
// - "DST_NON_ZERO"  - Only non-zero DST alpha will modify DST. All OUT operators.
#define OPERATOR_PROC_ALL 0x0
#define OPERATOR_PROC_SRC_NON_ZERO 0x1
#define OPERATOR_PROC_DST_NON_ZERO 0x2
#define OPERATOR_PROC_PRGB32_OP_XRGB32_PREF_0xFF 0x4

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
    if (OP::PROC & OPERATOR_PROC_PRGB32_OP_XRGB32_PREF_0xFF)
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
    if (OP::PROC & OPERATOR_PROC_PRGB32_OP_XRGB32_PREF_0xFF)
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
    if (OP::PROC & OPERATOR_PROC_PRGB32_OP_XRGB32_PREF_0xFF)
      OP::prgb32_op_xrgb32_0xFF_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
    else
      OP::prgb32_op_xrgb32_0xXX_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);

  }

  static FOG_INLINE void prgb32_op_xrgb32_PREF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    if (OP::PROC & OPERATOR_PROC_PRGB32_OP_XRGB32_PREF_0xFF)
      OP::prgb32_op_xrgb32_0xFF_unpacked_2x2W(dst0xmm, a0xmm, b0xmm, dst1xmm, a1xmm, b1xmm);
    else
      OP::prgb32_op_xrgb32_0xXX_unpacked_2x2W(dst0xmm, a0xmm, b0xmm, dst1xmm, a1xmm, b1xmm);
  }
};

// This template implements compositing for any operator, compositing with mask
// isn't included here.
template<typename OP>
struct CompositeBaseFuncsSSE2
{
  // -------------------------------------------------------------------------
  // [CompositeBaseFuncsSSE2 - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);

    if (OP::MODE == OPERATOR_MODE_PACKED)
    {
      sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    }
    else
    {
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);
    }

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm;
      sse2_load16a(dst0xmm, dst);
      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::PROC & OPERATOR_PROC_PRGB32_OP_XRGB32_PREF_0xFF) sse2_fill_alpha_1x4B(src0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      if (OP::PROC & OPERATOR_PROC_PRGB32_OP_XRGB32_PREF_0xFF) sse2_fill_alpha_1x4B(src0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  // -------------------------------------------------------------------------
  // [CompositeBaseFuncsSSE2 - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);

    if (OP::MODE == OPERATOR_MODE_PACKED)
    {
      sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    }
    else
    {
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);
    }

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm;
      sse2_load16a(dst0xmm, dst);
      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
      {
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }
};

// This template implements compositing of BOUND operators.
template<typename OP>
struct CompositeMaskInSSE2
{
  // -------------------------------------------------------------------------
  // [CompositeMaskInSSE2 - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        __m128i dst0xmm;
        __m128i msk0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        __m128i dst0xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    {
      __m128i msk0xmm;

      sse2_load4(src0xmm, &src->prgb);
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);
      sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);
    }

    if (OP::MODE == OPERATOR_MODE_PACKED)
    {
      sse2_pack_1x1W(src0xmm, src0xmm);
    }

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm;
      sse2_load16a(dst0xmm, dst);
      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    sse2_unpack_1x2W(src0xmm, src0xmm);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      __m128i msk0xmm;

      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
      sse2_muldiv255_1x2W(msk0xmm, msk0xmm, src0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
      {
        sse2_pack_1x1W(msk0xmm, msk0xmm);
      }

      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      SSE2_BLIT_32x4_SMALL_END(bltConst)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
        __m128i dst0xmm;
        sse2_load16a(dst0xmm, dst);
        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      SSE2_BLIT_32x4_LARGE_END(bltConst)
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltMask)
        uint32_t msk0 = READ_8(msk);

        __m128i dst0xmm;
        __m128i msk0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      SSE2_BLIT_32x4_SMALL_END(bltMask)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltMask)
        __m128i dst0xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      SSE2_BLIT_32x4_LARGE_END(bltMask)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        {
          __m128i msk0xmm, msk1xmm;
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        }

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
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

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        // TODO: AUDIT

        // Multiply source alpha with mask and premultiply pixel by result. 
        // Here is used optimization to save one multiply when using classic
        // approach (multiply 2x2W, premultiply 2x2W).
        {
          __m128i msk0xmm, msk1xmm;

          sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0);

          msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
          msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
          msk1xmm = _mm_srli_epi16(msk1xmm, 8);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

          msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
          msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        }

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x4B(src0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        {
          __m128i msk0xmm, msk1xmm;
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        }

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      // TODO: AUDIT

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

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_fill_alpha_1x4B(src0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_fill_alpha_1x4B(src0xmm);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  // -------------------------------------------------------------------------
  // [CompositeMaskInSSE2 - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        __m128i dst0xmm;
        __m128i msk0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        __m128i dst0xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    {
      __m128i msk0xmm;

      sse2_load4(src0xmm, &src->prgb);
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);
      sse2_muldiv255_1x2W(src0xmm, src0xmm, msk0xmm);
    }

    if (OP::MODE == OPERATOR_MODE_PACKED)
    {
      sse2_pack_1x1W(src0xmm, src0xmm);
    }

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm;
      sse2_load16a(dst0xmm, dst);
      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    sse2_unpack_1x2W(src0xmm, src0xmm);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      __m128i msk0xmm;

      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
      sse2_muldiv255_1x2W(msk0xmm, msk0xmm, src0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
      {
        sse2_pack_1x1W(msk0xmm, msk0xmm);
      }

      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      SSE2_BLIT_32x4_SMALL_END(bltConst)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
        __m128i dst0xmm;
        sse2_load16a(dst0xmm, dst);
        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      SSE2_BLIT_32x4_LARGE_END(bltConst)
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltMask)
        uint32_t msk0 = READ_8(msk);

        __m128i dst0xmm;
        __m128i msk0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      SSE2_BLIT_32x4_SMALL_END(bltMask)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltMask)
        __m128i dst0xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      SSE2_BLIT_32x4_LARGE_END(bltMask)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        {
          __m128i msk0xmm, msk1xmm;
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        }

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
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

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        // TODO: AUDIT

        // Multiply source alpha with mask and premultiply pixel by result.
        // Here is used optimization to save one multiply when using classic
        // approach (multiply 2x2W, premultiply 2x2W).
        {
          __m128i msk0xmm, msk1xmm;

          sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0);

          msk1xmm = _mm_shufflelo_epi16(src0xmm, _MM_SHUFFLE(2, 2, 0, 0));
          msk1xmm = _mm_shufflehi_epi16(msk1xmm, _MM_SHUFFLE(2, 2, 0, 0));
          msk1xmm = _mm_srli_epi16(msk1xmm, 8);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_1x2W(msk0xmm, msk0xmm, msk1xmm);

          msk1xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(3, 3, 2, 2));
          msk0xmm = _mm_shuffle_epi32(msk0xmm, _MM_SHUFFLE(1, 1, 0, 0));

          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        }

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x4B(src0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0);

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        {
          __m128i msk0xmm, msk1xmm;
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        }

        if (OP::MODE == OPERATOR_MODE_PACKED)
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      // TODO: AUDIT

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
        tmp1xmm = _mm_shuffle_epi32(tmp1xmm, _MM_SHUFFLE(1, 1, 0, 0));

        sse2_muldiv255_2x2W(src0xmm, src0xmm, tmp0xmm, src1xmm, src1xmm, tmp1xmm);
      }

      // Multiply source alpha with mask and premultiply pixel by result.
      // Here is used optimization to save one multiply when using classic
      // approach (multiply 2x2W, premultiply 2x2W).
      {
        __m128i tmp0xmm, tmp1xmm;

        tmp0xmm = _mm_slli_epi32(src0xmm, 24);
        tmp0xmm = _mm_packus_epi16(tmp0xmm, tmp0xmm);
        sse2_muldiv255_1x2W(tmp0xmm, tmp0xmm, msk0xmm);

        tmp1xmm = _mm_shuffle_epi32(tmp0xmm, _MM_SHUFFLE(3, 3, 2, 2));
        tmp0xmm = _mm_shuffle_epi32(tmp0xmm, _MM_SHUFFLE(1, 1, 0, 0));

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, tmp0xmm, src1xmm, src1xmm, tmp1xmm);
      }

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_fill_alpha_1x4B(src0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_fill_alpha_1x4B(src0xmm);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);

      if (OP::MODE == OPERATOR_MODE_PACKED)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }
};

// This template implements compositing of UNBOUND operators.
template<typename OP>
struct CompositeMaskLerpInSSE2
{
  // -------------------------------------------------------------------------
  // [CompositeMaskLerpIn - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i inv0xmm;

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
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i inv0xmm, inv1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_negate_2x2W(inv0xmm, msk0xmm, inv1xmm, msk1xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_muldiv255_2x2W(inv0xmm, inv0xmm, dst0xmm, inv1xmm, inv1xmm, dst1xmm);
        sse2_pack_2x2W(inv0xmm, inv0xmm, inv1xmm);

        OP::prgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, inv0xmm);
        sse2_store16a(dst, dst0xmm);
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_load4(msk0xmm, &src->prgb);
    sse2_unpack_1x1W(msk0xmm, msk0xmm);
    sse2_expand_mask_1x1W(inv0xmm, msk0);

    sse2_muldiv255_1x1W(msk0xmm, msk0xmm, inv0xmm);
    sse2_negate_1x1W(inv0xmm, inv0xmm);

    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_expand_pixel_lo_1x2W(inv0xmm, inv0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }
  
  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    sse2_unpack_1x2W(src0xmm, src0xmm);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      __m128i msk0xmm;
      __m128i inv0xmm;

      sse2_expand_mask_1x1W(inv0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(inv0xmm, inv0xmm);
      sse2_muldiv255_1x2W(msk0xmm, src0xmm, inv0xmm);
      sse2_negate_1x2W(inv0xmm, inv0xmm);

      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
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
      SSE2_BLIT_32x4_SMALL_END(bltConst)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
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
      SSE2_BLIT_32x4_LARGE_END(bltConst)
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltMask)
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
      SSE2_BLIT_32x4_SMALL_END(bltMask)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltMask)
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
      SSE2_BLIT_32x4_LARGE_END(bltMask)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        // TODO: AUDIT

        // Multiply source alpha with mask and premultiply pixel by result.
        // Here is used optimization to save one multiply when using classic
        // approach (multiply 2x2W, premultiply 2x2W).
        {
          sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0);

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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_fill_alpha_1x4B(src0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x2W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x2W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i tmp0xmm, tmp1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      // TODO: AUDIT

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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x2W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  // -------------------------------------------------------------------------
  // [CompositeMaskLeprIn - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i inv0xmm;

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
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i inv0xmm, inv1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_negate_2x2W(inv0xmm, msk0xmm, inv1xmm, msk1xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_muldiv255_2x2W(inv0xmm, inv0xmm, dst0xmm, inv1xmm, inv1xmm, dst1xmm);
        sse2_pack_2x2W(inv0xmm, inv0xmm, inv1xmm);

        OP::xrgb32_op_prgb32_unpacked_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, inv0xmm);
        sse2_store16a(dst, dst0xmm);
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_load4(msk0xmm, &src->prgb);
    sse2_unpack_1x1W(msk0xmm, msk0xmm);
    sse2_expand_mask_1x1W(inv0xmm, msk0);

    sse2_muldiv255_1x1W(msk0xmm, msk0xmm, inv0xmm);
    sse2_negate_1x1W(inv0xmm, inv0xmm);

    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_expand_pixel_lo_1x2W(inv0xmm, inv0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    sse2_unpack_1x2W(src0xmm, src0xmm);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      __m128i msk0xmm;
      __m128i inv0xmm;

      sse2_expand_mask_1x1W(inv0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(inv0xmm, inv0xmm);
      sse2_muldiv255_1x2W(msk0xmm, src0xmm, inv0xmm);
      sse2_negate_1x2W(inv0xmm, inv0xmm);

      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
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
      SSE2_BLIT_32x4_SMALL_END(bltConst)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
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
      SSE2_BLIT_32x4_LARGE_END(bltConst)
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltMask)
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
      SSE2_BLIT_32x4_SMALL_END(bltMask)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltMask)
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
      SSE2_BLIT_32x4_LARGE_END(bltMask)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        // TODO: AUDIT

        // Multiply source alpha with mask and premultiply pixel by result.
        // Here is used optimization to save one multiply when using classic
        // approach (multiply 2x2W, premultiply 2x2W).
        {
          sse2_expand_mask_1x4B_to_1x4WW(msk0xmm, msk0);

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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_fill_alpha_1x4B(src0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
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
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x2W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x2W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i tmp0xmm, tmp1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      // TODO: AUDIT

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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x2W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }
};

// ############################################################################
// Implementation starts from here.
// ############################################################################

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Src]
// ============================================================================

struct FOG_HIDDEN CompositeSrcSSE2
{
  // -------------------------------------------------------------------------
  // [CompositeSrcSSE2 - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      sse2_store4(dst, src0xmm);
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    while (w >= 4)
    {
      sse2_store16a(dst, src0xmm);
      sse2_store16a(dst + 16, src0xmm);
      sse2_store16a(dst + 32, src0xmm);
      sse2_store16a(dst + 48, src0xmm);

      dst += 64;
      w -= 4;
    }

    switch (w & 3)
    {
      case 3: sse2_store16a(dst, src0xmm); dst += 16;
      case 2: sse2_store16a(dst, src0xmm); dst += 16;
      case 1: sse2_store16a(dst, src0xmm); dst += 16;
    }

    if ((_i = _j)) { w = 0; goto blt; }
    SSE2_BLIT_GENERIC_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x2W(src0xmm, src0orig);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 == 0xFF)
      {
        sse2_store4(dst, src0orig);
      }
      else if (msk0)
      {
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_lerp_1x1W(dst0xmm, src0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        if (msk0 == 0xFFFFFFFF)
        {
          sse2_store16a(dst, src0orig);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
          sse2_lerp_2x2W(dst0xmm, src0xmm, msk0xmm, dst1xmm, src0xmm, msk1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msrc0xmm;
    __m128i minv0xmm;

    sse2_load4(msrc0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(msrc0xmm, msrc0xmm);
    sse2_unpack_1x2W(msrc0xmm, msrc0xmm);

    sse2_expand_mask_1x1W(minv0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(minv0xmm, minv0xmm);

    sse2_muldiv255_1x2W(msrc0xmm, msrc0xmm, minv0xmm);
    sse2_negate_1x2W(minv0xmm, minv0xmm);
    sse2_pack_1x1W(msrc0xmm, msrc0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_adds_1x1B(dst0xmm, dst0xmm, msrc0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_adds_1x4B(dst0xmm, dst0xmm, msrc0xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x2W(src0xmm, src0orig);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      if (msk0 == 255)
      {
        SSE2_BLIT_32x4_INIT(dst, w)

        SSE2_BLIT_32x4_SMALL_BEGIN(bltOpaque)
          sse2_store4(dst, src0orig);
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltOpaque)

        while (w >= 4)
        {
          sse2_store16a(dst, src0orig);
          sse2_store16a(dst + 16, src0orig);
          sse2_store16a(dst + 32, src0orig);
          sse2_store16a(dst + 48, src0orig);

          dst += 64;
          w -= 4;
        }

        switch (w & 3)
        {
          case 3: sse2_store16a(dst, src0orig); dst += 16;
          case 2: sse2_store16a(dst, src0orig); dst += 16;
          case 1: sse2_store16a(dst, src0orig); dst += 16;
        }

        if ((_i = _j)) { w = 0; goto bltOpaque; }
        SSE2_BLIT_GENERIC_END(bltOpaque)
      }
      else
      {
        __m128i msrc0xmm;
        __m128i minv0xmm;

        sse2_expand_mask_1x1W(msrc0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msrc0xmm, msrc0xmm);

        sse2_negate_1x2W(minv0xmm, msrc0xmm);
        sse2_muldiv255_1x1W(msrc0xmm, msrc0xmm, src0xmm);
        sse2_pack_1x1W(msrc0xmm, msrc0xmm);

        SSE2_BLIT_32x4_INIT(dst, w);

        SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_adds_1x1B(dst0xmm, dst0xmm, msrc0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltConst)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
          __m128i dst0xmm, dst1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msrc0xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_LARGE_END(bltConst)
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w);

      SSE2_BLIT_32x4_SMALL_BEGIN(bltMsk)
        __m128i dst0xmm;
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_lerp_1x1W(dst0xmm, src0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(bltMsk)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltMsk)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        uint32_t msk0 = READ_32(msk);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_lerp_2x2W(dst0xmm, src0xmm, msk0xmm, dst1xmm, src0xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_LARGE_END(bltMsk)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);

        if (msk0 != 0xFF)
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(a0xmm, msk0);
          sse2_lerp_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
        else
        {
          sse2_store4(dst, src0xmm);
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);

        if (msk0 != 0xFFFFFFFF)
        {
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_lerp_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }
        else
        {
          sse2_store16a(dst, src0xmm);
        }
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);

        if (msk0 != 0xFF)
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(a0xmm, msk0);
          sse2_lerp_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
        else
        {
          sse2_pack_1x1W(src0xmm, src0xmm);
          sse2_store4(dst, src0xmm);
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      uint32_t msk0 = READ_32(msk);

      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

        if (msk0 != 0xFFFFFFFF)
        {
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_lerp_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }
        else
        {
          sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
          sse2_store16a(dst, src0xmm);
        }
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        src0xmm = _mm_or_si128(src0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));

        if (msk0 != 0xFF)
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(a0xmm, msk0);
          sse2_lerp_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
        else
        {
          sse2_store4(dst, src0xmm);
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        src0xmm = _mm_or_si128(src0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));

        if (msk0 != 0xFFFFFFFF)
        {
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_lerp_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }
        else
        {
          sse2_store16a(dst, src0xmm);
        }
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_a8_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;
      __m128i dst0mm;
      __m128i a0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        uint32_t src0 = src[0] << 24;

        if (msk0 == 0xFF)
        {
          ((uint32_t*)dst)[0] = src0;
        }
        else
        {
          sse2_load4(dst0mm, dst);
          sse2_unpack_1x1W(src0mm, src0);
          sse2_unpack_1x1W(dst0mm, dst0mm);
          sse2_expand_mask_1x1W(a0xmm, msk0);
          sse2_lerp_1x1W(dst0mm, src0mm, a0xmm);
          sse2_pack_1x1W(dst0mm, dst0mm);
          sse2_store4(dst, dst0mm);
        }
      }

      dst += 4;
      src += 1;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0mm, src1mm;
      __m128i dst0mm, dst1mm;
      __m128i a0xmm, a1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load4(src0mm, src);
        sse2_unpack_1x1D(src0mm, src0mm);
        src0mm = _mm_slli_epi32(src0mm, 24);

        if (msk0 == 0xFFFFFFFF)
        {
          sse2_store16a(dst, src0mm);
        }
        else
        {
          sse2_load16a(dst0mm, dst);
          sse2_unpack_2x2W(src0mm, src1mm, src0mm);
          sse2_unpack_2x2W(dst0mm, dst1mm, dst0mm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_lerp_2x2W(dst0mm, src0mm, a0xmm, dst1mm, src1mm, a1xmm);
          sse2_pack_2x2W(dst0mm, dst0mm, dst1mm);
          sse2_store16a(dst, dst0mm);
        }
      }

      dst += 16;
      src += 4;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_i8_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    const Argb* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load_1xI8(src0xmm, src, srcPal);

        if (msk0 == 0xFF)
        {
          sse2_store4(dst, src0xmm);
        }
        else
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(a0xmm, msk0);
          sse2_lerp_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      src += 1;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load_4xI8(src0xmm, src, srcPal);

        if (msk0 == 0xFFFFFFFF)
        {
          sse2_store16a(dst, src0xmm);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_lerp_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 4;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm, im0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_lerp_ialpha_1x1W(dst0xmm, src0xmm, m0xmm, im0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_lerp_ialpha_2x2W(dst0xmm, src0xmm, m0xmm, im0xmm, dst1xmm, src1xmm, m0xmm, im0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm, im0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_premultiply_1x1W(src0xmm, src0xmm);
      sse2_lerp_ialpha_1x1W(dst0xmm, src0xmm, m0xmm, im0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_lerp_ialpha_2x2W(dst0xmm, src0xmm, m0xmm, im0xmm, dst1xmm, src1xmm, m0xmm, im0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm, im0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      src0xmm = _mm_or_si128(src0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_lerp_ialpha_1x1W(dst0xmm, src0xmm, m0xmm, im0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);
      src0xmm = _mm_or_si128(src0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_lerp_ialpha_2x2W(dst0xmm, src0xmm, m0xmm, im0xmm, dst1xmm, src1xmm, m0xmm, im0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_a8_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm, im0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;
      __m128i dst0mm;

      sse2_load4(dst0mm, dst);
      sse2_unpack_1x1W(src0mm, src[0]);
      sse2_unpack_1x1W(dst0mm, dst0mm);
      sse2_lerp_ialpha_1x1W(dst0mm, src0mm, m0xmm, im0xmm);
      sse2_pack_1x1W(dst0mm, dst0mm);
      sse2_store4(dst, dst0mm);

      dst += 4;
      src += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0mm, src1mm;
      __m128i dst0mm, dst1mm;

      sse2_load4(src0mm, src);
      sse2_load16a(dst0mm, dst);
      sse2_unpack_1x1D(src0mm, src0mm);
      src0mm = _mm_slli_epi32(src0mm, 24);
      sse2_unpack_2x2W(src0mm, src1mm, src0mm);
      sse2_unpack_2x2W(dst0mm, dst1mm, dst0mm);
      sse2_lerp_ialpha_2x2W(dst0mm, src0mm, m0xmm, im0xmm, dst1mm, src1mm, m0xmm, im0xmm);
      sse2_pack_2x2W(dst0mm, dst0mm, dst1mm);
      sse2_store16a(dst, dst0mm);

      dst += 16;
      src += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_i8_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    const Argb* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    __m128i m0xmm, im0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load_1xI8(src0xmm, src, srcPal);
      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_lerp_ialpha_1x1W(dst0xmm, src0xmm, m0xmm, im0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load_4xI8(src0xmm, src, srcPal);
      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_lerp_ialpha_2x2W(dst0xmm, src0xmm, m0xmm, im0xmm, dst1xmm, src1xmm, m0xmm, im0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  // -------------------------------------------------------------------------
  // [CompositeSrcSSE2 - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_fill_alpha_1x4B(src0xmm);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      sse2_store4(dst, src0xmm);
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    while (w >= 4)
    {
      sse2_store16a(dst, src0xmm);
      sse2_store16a(dst + 16, src0xmm);
      sse2_store16a(dst + 32, src0xmm);
      sse2_store16a(dst + 48, src0xmm);

      dst += 64;
      w -= 4;
    }
    switch (w & 3)
    {
      case 3: sse2_store16a(dst, src0xmm); dst += 16;
      case 2: sse2_store16a(dst, src0xmm); dst += 16;
      case 1: sse2_store16a(dst, src0xmm); dst += 16;
    }

    if ((_i = _j)) { w = 0; goto blt; }
    SSE2_BLIT_GENERIC_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_fill_alpha_1x4B(src0orig);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x2W(src0xmm, src0orig);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        if (msk0 == 0xFF)
        {
          sse2_store4(dst, src0orig);
        }
        else
        {
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x1B(dst0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_lerp_1x1W(dst0xmm, src0xmm, msk0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        if (msk0 == 0xFFFFFFFF)
        {
          sse2_store16a(dst, src0orig);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(dst0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
          sse2_lerp_2x2W(dst0xmm, src0xmm, msk0xmm, dst1xmm, src0xmm, msk1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msrc0xmm;
    __m128i minv0xmm;

    sse2_load4(msrc0xmm, &src->prgb);
    sse2_fill_alpha_1x4B(msrc0xmm);
    sse2_expand_pixel_lo_1x4B(msrc0xmm, msrc0xmm);
    sse2_unpack_1x2W(msrc0xmm, msrc0xmm);

    sse2_expand_mask_1x1W(minv0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(minv0xmm, minv0xmm);
    sse2_muldiv255_1x1W(msrc0xmm, msrc0xmm, minv0xmm);
    sse2_negate_1x1W(minv0xmm, minv0xmm);
    sse2_pack_1x1W(msrc0xmm, msrc0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_fill_alpha_1x1B(dst0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_adds_1x1B(dst0xmm, dst0xmm, msrc0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_adds_1x4B(dst0xmm, dst0xmm, msrc0xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_fill_alpha_1x1B(src0orig);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x2W(src0xmm, src0orig);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      if (msk0 == 255)
      {
        SSE2_BLIT_32x4_INIT(dst, w)

        SSE2_BLIT_32x4_SMALL_BEGIN(bltOpaque)
          sse2_store4(dst, src0orig);
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltOpaque)

        while (w >= 4)
        {
          sse2_store16a(dst, src0orig);
          sse2_store16a(dst + 16, src0orig);
          sse2_store16a(dst + 32, src0orig);
          sse2_store16a(dst + 48, src0orig);

          dst += 64;
          w -= 4;
        }

        switch (w & 3)
        {
          case 3: sse2_store16a(dst, src0orig); dst += 16;
          case 2: sse2_store16a(dst, src0orig); dst += 16;
          case 1: sse2_store16a(dst, src0orig); dst += 16;
        }

        if ((_i = _j)) { w = 0; goto bltOpaque; }
        SSE2_BLIT_GENERIC_END(bltOpaque)
      }
      else
      {
        __m128i msrc0xmm;
        __m128i minv0xmm;

        sse2_expand_mask_1x1W(msrc0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msrc0xmm, msrc0xmm);

        sse2_negate_1x2W(minv0xmm, msrc0xmm);
        sse2_muldiv255_1x1W(msrc0xmm, msrc0xmm, src0xmm);
        sse2_pack_1x1W(msrc0xmm, msrc0xmm);

        SSE2_BLIT_32x4_INIT(dst, w);

        SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x1B(dst0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_adds_1x1B(dst0xmm, dst0xmm, msrc0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltConst)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
          __m128i dst0xmm, dst1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(dst0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msrc0xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_LARGE_END(bltConst)
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w);

      SSE2_BLIT_32x4_SMALL_BEGIN(bltMsk)
        __m128i dst0xmm;
        __m128i msk0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_lerp_1x1W(dst0xmm, src0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(bltMsk)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltMsk)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_fill_alpha_1x4B(dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_lerp_2x2W(dst0xmm, src0xmm, msk0xmm, dst1xmm, src0xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_LARGE_END(bltMsk)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i src0xmm;
      __m128i msk0mm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);

        if (msk0 == 0xFF)
        {
          sse2_fill_alpha_1x1W(src0xmm);
          sse2_pack_1x1W(src0xmm, src0xmm);
          sse2_store4(dst, src0xmm);
        }
        else
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(msk0mm, msk0);
          sse2_lerp_1x1W(dst0xmm, src0xmm, msk0mm);
          sse2_fill_alpha_1x1W(dst0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;
      __m128i msk0mm, msk1mm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

        if (msk0 == 0xFFFFFFFF)
        {
          sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
          src0xmm = _mm_or_si128(src0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
          sse2_store16a(dst, src0xmm);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0mm, msk1mm, msk0);
          sse2_lerp_2x2W(dst0xmm, src0xmm, msk0mm, dst1xmm, src1xmm, msk1mm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i src0xmm;
      __m128i msk0mm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);

        if (msk0 == 0xFF)
        {
          sse2_fill_alpha_1x1W(src0xmm);
          sse2_store4(dst, src0xmm);
        }
        else
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(msk0mm, msk0);
          sse2_lerp_1x1W(dst0xmm, src0xmm, msk0mm);
          sse2_fill_alpha_1x1W(dst0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;
      __m128i msk0mm, msk1mm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);

        if (msk0 == 0xFFFFFFFF)
        {
          src0xmm = _mm_or_si128(src0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
          sse2_store16a(dst, src0xmm);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0mm, msk1mm, msk0);
          sse2_lerp_2x2W(dst0xmm, src0xmm, msk0mm, dst1xmm, src1xmm, msk1mm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_i8_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    const Argb* srcPal = closure->srcPalette;

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i src0xmm;
      __m128i msk0mm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load_1xI8(src0xmm, src, srcPal);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_premultiply_1x1W(src0xmm, src0xmm);

        if (msk0 == 0xFF)
        {
          sse2_fill_alpha_1x1W(src0xmm);
          sse2_pack_1x1W(src0xmm, src0xmm);
          sse2_store4(dst, src0xmm);
        }
        else
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(msk0mm, msk0);
          sse2_lerp_1x1W(dst0xmm, src0xmm, msk0mm);
          sse2_fill_alpha_1x1W(dst0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      src += 1;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;
      __m128i msk0mm, msk1mm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load_4xI8(src0xmm, src, srcPal);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

        if (msk0 == 0xFFFFFFFF)
        {
          sse2_pack_2x2W(src0xmm, src0xmm, src1xmm);
          src0xmm = _mm_or_si128(src0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
          sse2_store16a(dst, src0xmm);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0mm, msk1mm, msk0);
          sse2_lerp_2x2W(dst0xmm, src0xmm, msk0mm, dst1xmm, src1xmm, msk1mm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 4;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0mm, imsk0mm;
    sse2_expand_mask_1x1W(msk0mm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0mm, msk0mm);
    sse2_negate_1x2W(imsk0mm, msk0mm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i src0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_premultiply_1x1W(src0xmm, src0xmm);
      sse2_lerp_ialpha_1x1W(dst0xmm, src0xmm, msk0mm, imsk0mm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_lerp_ialpha_2x2W(dst0xmm, src0xmm, msk0mm, imsk0mm, dst1xmm, src1xmm, msk0mm, imsk0mm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0mm, imsk0mm;
    sse2_expand_mask_1x1W(msk0mm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0mm, msk0mm);
    sse2_negate_1x2W(imsk0mm, msk0mm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i src0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_lerp_ialpha_1x1W(dst0xmm, src0xmm, msk0mm, imsk0mm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_lerp_ialpha_2x2W(dst0xmm, src0xmm, msk0mm, imsk0mm, dst1xmm, src1xmm, msk0mm, imsk0mm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_i8_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    const Argb* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    __m128i msk0mm, imsk0mm;
    sse2_expand_mask_1x1W(msk0mm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0mm, msk0mm);
    sse2_negate_1x2W(imsk0mm, msk0mm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i src0xmm;

      sse2_load_1xI8(src0xmm, src, srcPal);
      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_lerp_ialpha_1x1W(dst0xmm, src0xmm, msk0mm, imsk0mm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;

      sse2_load_4xI8(src0xmm, src, srcPal);
      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_lerp_ialpha_2x2W(dst0xmm, src0xmm, msk0mm, imsk0mm, dst1xmm, src1xmm, msk0mm, imsk0mm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - SrcOver]
// ============================================================================

// This is template based approach currently not used (because this operator
// is most common and it's really optimized).
/*
struct FOG_HIDDEN CompositeSrcOverSSE2 :
  public CompositeCommonSSE2<CompositeSrcOverSSE2>,
  public CompositeBaseFuncsSSE2<CompositeSrcOverSSE2>,
  public CompositeMaskInSSE2<CompositeSrcOverSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.

  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_expand_alpha_1x1W(i0xmm, b0xmm);
    sse2_negate_1x1W(i0xmm, i0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    dst0xmm = _mm_or_si128(b0xmm, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm;
    __m128i i1xmm;

    sse2_expand_alpha_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_negate_2x2W(i0xmm, i0xmm, i1xmm, i1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    dst0xmm = _mm_or_si128(b0xmm, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
    dst1xmm = _mm_or_si128(b1xmm, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  }
};
*/

struct FOG_HIDDEN CompositeSrcOverSSE2
{
  // -------------------------------------------------------------------------
  // [CompositeSrcOverSSE2 - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    if (ArgbUtil::isAlpha0xFF(src->prgb))
    {
      SSE2_BLIT_32x4_SMALL_BEGIN(bltOpaque)
        sse2_store4(dst, src0xmm);
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(bltOpaque)

      while (w >= 4)
      {
        sse2_store16a(dst, src0xmm);
        sse2_store16a(dst + 16, src0xmm);
        sse2_store16a(dst + 32, src0xmm);
        sse2_store16a(dst + 48, src0xmm);

        dst += 64;
        w -= 4;
      }
      switch (w & 3)
      {
        case 3: sse2_store16a(dst, src0xmm); dst += 16;
        case 2: sse2_store16a(dst, src0xmm); dst += 16;
        case 1: sse2_store16a(dst, src0xmm); dst += 16;
      }

      switch (_j)
      {
        case 3: sse2_store4(dst, src0xmm); dst += 4;
        case 2: sse2_store4(dst, src0xmm); dst += 4;
        case 1: sse2_store4(dst, src0xmm);
      }

      SSE2_BLIT_GENERIC_END(bltOpaque)
    }
    else
    {
      __m128i ia0xmm;
      sse2_unpack_1x2W(ia0xmm, src0xmm);
      sse2_expand_alpha_1x2W(ia0xmm, ia0xmm);
      sse2_negate_1x1W(ia0xmm, ia0xmm);

      SSE2_BLIT_32x4_SMALL_BEGIN(bltAlpha)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, ia0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(dst0xmm, dst0xmm, src0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(bltAlpha)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltAlpha)
        __m128i dst0xmm;
        __m128i dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, ia0xmm, dst1xmm, dst1xmm, ia0xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_adds_1x4B(dst0xmm, dst0xmm, src0xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_LARGE_END(bltAlpha)
    }
  }

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x1W(src0xmm, src0orig);

    SSE2_BLIT_32x4_INIT(dst, w);

    if (ArgbUtil::isAlpha0xFF(src->prgb))
    {
      __m128i dst0xmm;

      // Dca' = Sca.m + Dca.(1 - m)
      // Da'  = Sa.m + Da.(1 - m)
      SSE2_BLIT_32x4_SMALL_PREPARE(bltOpaque)

      dst0xmm = src0orig;
      sse2_zero_pixel_lo_1x1W(src0xmm, src0xmm);

      SSE2_BLIT_32x4_SMALL_DO(bltOpaque)
        __m128i msk0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 == 0x00) goto bltOpaqueSkip1;
        if (msk0 == 0xFF) goto bltOpaqueStore1;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        dst0xmm = _mm_or_si128(dst0xmm, src0xmm);
        sse2_negate_1x2W_lo(msk0xmm, msk0xmm);
        sse2_muldiv255_1x2W(dst0xmm, dst0xmm, msk0xmm);
        sse2_reverse_1x2W(msk0xmm, dst0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

bltOpaqueStore1:
        sse2_store4(dst, dst0xmm);
        dst0xmm = src0orig;

bltOpaqueSkip1:
        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(bltOpaque)

      dst0xmm = src0orig;
      sse2_expand_pixel_hi_1x2W(src0xmm, src0xmm);

      SSE2_BLIT_32x4_LARGE_BEGIN(bltOpaque)
        __m128i dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i mskinv0xmm, mskinv1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 == 0x00000000) goto bltOpaqueSkip4;
        if (msk0 == 0xFFFFFFFF) goto bltOpaqueStore4;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

bltOpaqueStore4:
        sse2_store16a(dst, dst0xmm);
        dst0xmm = src0orig;
bltOpaqueSkip4:

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_LARGE_END(bltOpaque)
    }
    else
    {
      SSE2_BLIT_32x4_SMALL_BEGIN(bltAlpha)
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i mskinv0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 != 0x00)
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(msk0xmm, msk0);
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
          sse2_negate_1x1W(mskinv0xmm, msk0xmm);
          sse2_expand_alpha_1x1W(mskinv0xmm, mskinv0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, mskinv0xmm);
          sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(bltAlpha)

      sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
      sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

      SSE2_BLIT_32x4_LARGE_BEGIN(bltAlpha)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i mskinv0xmm, mskinv1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 != 0x00000000)
        {
          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
          sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
          sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
          sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_LARGE_END(bltAlpha)
    }
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;
    __m128i m0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
    sse2_expand_alpha_1x1W(m0xmm, src0xmm);
    sse2_negate_1x1W(m0xmm, m0xmm);

    sse2_pack_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, m0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_adds_1x1B(dst0xmm, dst0xmm, src0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, m0xmm, dst1xmm, dst1xmm, m0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_adds_1x4B(dst0xmm, dst0xmm, src0xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    int alphaTest = ArgbUtil::isAlpha0xFF(src->prgb) ? -255 : -256;

    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x1W(src0xmm, src0orig);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      if (msk0 + alphaTest == 0)
      {
        SSE2_BLIT_32x4_INIT(dst, w)

        SSE2_BLIT_32x4_SMALL_BEGIN(bltOpaque)
          sse2_store4(dst, src0orig);
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltOpaque)

        while (w >= 4)
        {
          sse2_store16a(dst, src0orig);
          sse2_store16a(dst + 16, src0orig);
          sse2_store16a(dst + 32, src0orig);
          sse2_store16a(dst + 48, src0orig);

          dst += 64;
          w -= 4;
        }

        switch (w & 3)
        {
          case 3: sse2_store16a(dst, src0orig); dst += 16;
          case 2: sse2_store16a(dst, src0orig); dst += 16;
          case 1: sse2_store16a(dst, src0orig); dst += 16;
        }

        switch (_j)
        {
          case 3: sse2_store4(dst, src0orig); dst += 4;
          case 2: sse2_store4(dst, src0orig); dst += 4;
          case 1: sse2_store4(dst, src0orig); dst += 4;
        }

        SSE2_BLIT_GENERIC_END(bltOpaque)
      }
      else
      {
        __m128i msrc0xmm;
        __m128i minv0xmm;

        sse2_expand_mask_1x1W(msrc0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msrc0xmm, msrc0xmm);
        sse2_muldiv255_1x2W(msrc0xmm, msrc0xmm, src0xmm);

        sse2_negate_1x2W(minv0xmm, msrc0xmm);
        sse2_expand_alpha_1x2W(minv0xmm, minv0xmm);

        sse2_pack_1x1W(msrc0xmm, msrc0xmm);

        SSE2_BLIT_32x4_INIT(dst, w);

        SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_adds_1x1B(dst0xmm, dst0xmm, msrc0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltConst)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
          __m128i dst0xmm, dst1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msrc0xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_LARGE_END(bltConst)
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w);

      if (alphaTest == -255)
      {
        __m128i dst0xmm;

        SSE2_BLIT_32x4_SMALL_PREPARE(bltOpaqueMsk)

        sse2_zero_pixel_lo_1x1W(src0xmm, src0xmm);

        // Dca' = Sca.m + Dca.(1 - m)
        // Da'  = Sa.m + Da.(1 - m)
        SSE2_BLIT_32x4_SMALL_DO(bltOpaqueMsk)
          __m128i msk0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          dst0xmm = _mm_or_si128(dst0xmm, src0xmm);
          sse2_negate_1x2W_lo(msk0xmm, msk0xmm);
          sse2_muldiv255_1x2W(dst0xmm, dst0xmm, msk0xmm);
          sse2_reverse_1x2W(msk0xmm, dst0xmm);
          sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(bltOpaqueMsk)

        sse2_expand_pixel_hi_1x2W(src0xmm, src0xmm);

        SSE2_BLIT_32x4_LARGE_BEGIN(bltOpaqueMsk)
          __m128i dst1xmm;
          __m128i msk0xmm, msk1xmm;
          __m128i mskinv0xmm, mskinv1xmm;

          sse2_load4(msk0xmm, msk);
          sse2_load16a(dst0xmm, dst);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
          sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
          sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);

          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_LARGE_END(bltOpaqueMsk)
      }
      else
      {
        SSE2_BLIT_32x4_SMALL_BEGIN(bltTransMsk)
          __m128i dst0xmm;
          __m128i msk0xmm;
          __m128i mskinv0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
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
        SSE2_BLIT_32x4_SMALL_END(bltTransMsk)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltTransMsk)
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
        SSE2_BLIT_32x4_LARGE_END(bltTransMsk)
      }
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL prgb32_vspan_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i src0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);
      if (!ArgbUtil::isAlpha0x00(src0))
      {
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);

        sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;
      __m128i msk0xmm, msk1xmm;

      sse2_load16u(src0xmm, src);

      SSE2_BLIT_TEST_4_ARGB_PIXELS(src0xmm, dst0xmm, dst1xmm, blt_fill, blt_away)

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_expand_alpha_2x2W(msk0xmm, src0xmm, msk1xmm, src1xmm);
      sse2_fill_alpha_2x2W(src0xmm, src1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
      sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
      sse2_adds_2x2W(src0xmm, src0xmm, dst0xmm, src1xmm, src1xmm, dst1xmm);
blt_fill:
      sse2_store16a(dst, src0xmm);
blt_away:

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);
      uint32_t msk0 = READ_8(msk);

      if ((!ArgbUtil::isAlpha0x00(src0)) & (msk0 != 0x00))
      {
        msk0 = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      sse2_load16u(src0xmm, src);
      uint32_t src0a = sse2_pack_alpha_to_uint32(src0xmm);
      uint32_t msk0 = ((uint32_t*)msk)[0];

      if ((src0a != 0x00000000) & (msk0 != 0x00000000))
      {
        if ((src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
        {
          sse2_store16a(dst, src0xmm);
        }
        else
        {
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_expand_mask_2x2W(dst0xmm, dst1xmm, msk0);
          sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
          sse2_muldiv255_2x2W(a0xmm, a0xmm, dst0xmm, a1xmm, a1xmm, dst1xmm);

          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_fill_alpha_2x2W(src0xmm, src1xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
          sse2_over_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);

      if (FOG_LIKELY(src0))
      {
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm;
      __m128i alp0xmm, alp1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);

      SSE2_BLIT_TEST_4_PRGB_PIXELS(src0xmm, alp0xmm, alp1xmm, blt_fill, blt_away)

      sse2_load16a(dst0xmm, dst);
      alp0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(3, 2, 1, 0));
      alp0xmm = _mm_xor_si128(alp0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_unpack_2x2W(alp0xmm, alp1xmm, alp0xmm);
      sse2_expand_alpha_2x2W(alp0xmm, alp0xmm, alp1xmm, alp1xmm);
      sse2_muldiv255_2x2W(alp0xmm, alp0xmm, dst0xmm, alp1xmm, alp1xmm, dst1xmm);
      sse2_pack_2x2W(alp0xmm, alp0xmm, alp1xmm);
      sse2_adds_1x4B(src0xmm, src0xmm, alp0xmm);
blt_fill:
      sse2_store16a(dst, src0xmm);
blt_away:
      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);
      uint32_t msk0 = READ_8(msk);

      if ((src0 != 0x00000000) & (msk0 != 0x00))
      {
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      sse2_load16u(src0xmm, src);
      uint32_t src0a = sse2_pack_alpha_to_uint32(src0xmm);
      uint32_t msk0 = ((uint32_t*)msk)[0];

      if ((src0a != 0x00000000) & (msk0 != 0x00000000))
      {
        if ((src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
        {
          sse2_store16a(dst, src0xmm);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
          sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
          sse2_over_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);

        if (msk0 != 0xFF)
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_unpack_1x1W(src0xmm, src0xmm);

          sse2_expand_mask_1x1W(a0xmm, msk0);
          sse2_fill_alpha_1x1W(src0xmm);
          sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
          sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(src0xmm, dst0xmm);
        }

        sse2_store4(dst, src0xmm);
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      uint32_t msk0 = ((uint32_t*)msk)[0];

      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        if (msk0 != 0xFFFFFFFF)
        {
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_fill_alpha_2x2W(src0xmm, src1xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
          sse2_over_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(src0xmm, dst0xmm, dst1xmm);
        }
        sse2_store16a(dst, src0xmm);
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm, im0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);

      if (!ArgbUtil::isAlpha0x00(src0))
      {
        if (ArgbUtil::isAlpha0xFF(src0))
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);

          sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
          sse2_over_ialpha_1x1W(dst0xmm, src0xmm, im0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);

          sse2_store4(dst, dst0xmm);
        }
        else
        {
          uint32_t msk0 = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);

          sse2_expand_mask_1x1W(a0xmm, msk0);
          sse2_fill_alpha_1x1W(src0xmm);
          sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
          sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);

          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      sse2_load16u(src0xmm, src);

      uint32_t src0a = sse2_pack_alpha_to_uint32(src0xmm);

      if (src0a != 0x00000000)
      {
        if (src0a == 0xFFFFFFFF)
        {
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
          sse2_over_ialpha_2x2W(dst0xmm, src0xmm, im0xmm, dst1xmm, src1xmm, im0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
          sse2_muldiv255_2x2W(a0xmm, a0xmm, m0xmm, a1xmm, a1xmm, m0xmm);

          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_fill_alpha_2x2W(src0xmm, src1xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
          sse2_over_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm, im0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);

      if (!ArgbUtil::isAlpha0x00(src0))
      {
        if (ArgbUtil::isAlpha0xFF(src0))
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);

          sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
          sse2_over_ialpha_1x1W(dst0xmm, src0xmm, im0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);

          sse2_store4(dst, dst0xmm);
        }
        else
        {
          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(src0xmm, src0);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
          sse2_expand_alpha_1x1W(a0xmm, src0xmm);
          sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);

          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      sse2_load16u(src0xmm, src);
      uint32_t src0a = sse2_pack_alpha_to_uint32(src0xmm);

      if (src0a != 0x00000000)
      {
        if (src0a == 0xFFFFFFFF)
        {
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
          sse2_over_ialpha_2x2W(dst0xmm, src0xmm, im0xmm, dst1xmm, src1xmm, im0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

          sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
          sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
          sse2_over_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm, im0xmm;
    __m128i amask = SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000);
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      uint32_t src0 = READ_32(src) | 0xFF000000;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(src0xmm, src0);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
      sse2_over_ialpha_1x1W(dst0xmm, src0xmm, im0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);

      sse2_store4(dst, dst0xmm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);
      src0xmm = _mm_or_si128(src0xmm, amask);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
      sse2_over_ialpha_2x2W(dst0xmm, src0xmm, im0xmm, dst1xmm, src1xmm, im0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

      sse2_store16a(dst, dst0xmm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  // -------------------------------------------------------------------------
  // [CompositeSrcOverSSE2 - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    if (ArgbUtil::isAlpha0xFF(src->prgb))
    {
      SSE2_BLIT_32x4_SMALL_BEGIN(bltOpaque)
        sse2_store4(dst, src0xmm);
        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(bltOpaque)

      while (w >= 4)
      {
        sse2_store16a(dst, src0xmm);
        sse2_store16a(dst + 16, src0xmm);
        sse2_store16a(dst + 32, src0xmm);
        sse2_store16a(dst + 48, src0xmm);

        dst += 64;
        w -= 4;
      }
      switch (w & 3)
      {
        case 3: sse2_store16a(dst, src0xmm); dst += 16;
        case 2: sse2_store16a(dst, src0xmm); dst += 16;
        case 1: sse2_store16a(dst, src0xmm); dst += 16;
      }

      switch (_j)
      {
        case 3: sse2_store4(dst, src0xmm); dst += 4;
        case 2: sse2_store4(dst, src0xmm); dst += 4;
        case 1: sse2_store4(dst, src0xmm);
      }

      SSE2_BLIT_GENERIC_END(bltOpaque)
    }
    else
    {
      __m128i ia0xmm;
      sse2_unpack_1x2W(ia0xmm, src0xmm);
      sse2_expand_alpha_1x2W(ia0xmm, ia0xmm);
      sse2_negate_1x1W(ia0xmm, ia0xmm);

      SSE2_BLIT_32x4_SMALL_BEGIN(bltAlpha)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, ia0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_adds_1x1B(dst0xmm, dst0xmm, src0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(bltAlpha)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltAlpha)
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
      SSE2_BLIT_32x4_LARGE_END(bltAlpha)
    }
  }

  static void FOG_FASTCALL xrgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x2W(src0xmm, src0orig);

    SSE2_BLIT_32x4_INIT(dst, w);

    if (ArgbUtil::isAlpha0xFF(src->prgb))
    {
      SSE2_BLIT_32x4_SMALL_BEGIN(bltOpaque)
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i mskinv0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 != 0x00)
        {
          if (msk0 == 0xFF)
          {
            sse2_store4(dst, src0orig);
          }
          else
          {
            sse2_load4(dst0xmm, dst);
            sse2_fill_alpha_1x1B(dst0xmm);
            sse2_unpack_1x1W(dst0xmm, dst0xmm);
            sse2_expand_mask_1x1W(msk0xmm, msk0);
            sse2_negate_1x1W(mskinv0xmm, msk0xmm);
            sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
            sse2_expand_alpha_1x1W(mskinv0xmm, mskinv0xmm);
            sse2_muldiv255_1x1W(dst0xmm, dst0xmm, mskinv0xmm);
            sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
            sse2_pack_1x1W(dst0xmm, dst0xmm);
            sse2_store4(dst, dst0xmm);
          }
        }

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(bltOpaque)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltOpaque)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i mskinv0xmm, mskinv1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 != 0x00000000)
        {
          if (msk0 == 0xFFFFFFFF)
          {
            sse2_store16a(dst, src0orig);
          }
          else
          {
            sse2_load16a(dst0xmm, dst);
            sse2_fill_alpha_1x4B(dst0xmm);
            sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
            sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
            sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
            sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
            sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
            sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
            sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
            sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
            sse2_store16a(dst, dst0xmm);
          }
        }

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_LARGE_END(bltOpaque)
    }
    else
    {
      SSE2_BLIT_32x4_SMALL_BEGIN(bltAlpha)
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i mskinv0xmm;

        uint32_t msk0 = READ_8(msk);
        if (msk0 != 0x00)
        {
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
        }

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(bltAlpha)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltAlpha)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i mskinv0xmm, mskinv1xmm;

        uint32_t msk0 = READ_32(msk);
        if (msk0 != 0x00000000)
        {
          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(dst0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
          sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
          sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
          sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_LARGE_END(bltAlpha)
    }
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;
    __m128i m0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x2W(src0xmm, src0xmm);
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
    sse2_expand_alpha_1x1W(m0xmm, src0xmm);
    sse2_negate_1x1W(m0xmm, m0xmm);

    sse2_pack_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_fill_alpha_1x1B(dst0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, m0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_adds_1x1B(dst0xmm, dst0xmm, src0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, m0xmm, dst1xmm, dst1xmm, m0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_adds_1x4B(dst0xmm, dst0xmm, src0xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    int alphaTest = ArgbUtil::isAlpha0xFF(src->prgb) ? -255 : -256;

    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0orig;
    __m128i src0xmm;

    sse2_load4(src0orig, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0orig, src0orig);
    sse2_unpack_1x2W(src0xmm, src0orig);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      if (msk0 + alphaTest == 0)
      {
        SSE2_BLIT_32x4_INIT(dst, w)

        SSE2_BLIT_32x4_SMALL_BEGIN(bltOpaque)
          sse2_store4(dst, src0orig);
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltOpaque)

        while (w >= 4)
        {
          sse2_store16a(dst, src0orig);
          sse2_store16a(dst + 16, src0orig);
          sse2_store16a(dst + 32, src0orig);
          sse2_store16a(dst + 48,src0orig);

          dst += 64;
          w -= 4;
        }

        switch (w & 3)
        {
          case 3: sse2_store16a(dst, src0orig); dst += 16;
          case 2: sse2_store16a(dst, src0orig); dst += 16;
          case 1: sse2_store16a(dst, src0orig); dst += 16;
        }

        switch (_j)
        {
          case 3: sse2_store4(dst, src0orig); dst += 4;
          case 2: sse2_store4(dst, src0orig); dst += 4;
          case 1: sse2_store4(dst, src0orig); dst += 4;
        }

        SSE2_BLIT_GENERIC_END(bltOpaque)
      }
      else
      {
        __m128i msrc0xmm;
        __m128i minv0xmm;

        sse2_expand_mask_1x1W(msrc0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msrc0xmm, msrc0xmm);
        sse2_muldiv255_1x2W(msrc0xmm, msrc0xmm, src0xmm);

        sse2_negate_1x2W(minv0xmm, msrc0xmm);
        sse2_expand_alpha_1x2W(minv0xmm, minv0xmm);

        sse2_pack_1x1W(msrc0xmm, msrc0xmm);

        SSE2_BLIT_32x4_INIT(dst, w);

        SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x1B(dst0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_adds_1x1B(dst0xmm, dst0xmm, msrc0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltConst)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
          __m128i dst0xmm, dst1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(dst0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_adds_1x4B(dst0xmm, dst0xmm, msrc0xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_LARGE_END(bltConst)
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w);
      if (alphaTest == -255)
      {
        SSE2_BLIT_32x4_SMALL_PREPARE(bltMaskLerp)

        sse2_zero_pixel_lo_1x1W(src0xmm, src0xmm);

        SSE2_BLIT_32x4_SMALL_DO(bltMaskLerp)
          __m128i dst0xmm;
          __m128i msk0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
          dst0xmm = _mm_or_si128(dst0xmm, src0xmm);
          sse2_negate_1x2W_lo(msk0xmm, msk0xmm);
          sse2_muldiv255_1x2W(dst0xmm, dst0xmm, msk0xmm);
          sse2_reverse_1x2W(msk0xmm, dst0xmm);
          sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_fill_alpha_1x1B(dst0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(bltMaskLerp)

        sse2_expand_pixel_hi_1x2W(src0xmm, src0xmm);

        SSE2_BLIT_32x4_LARGE_BEGIN(bltMaskLerp)
          __m128i dst0xmm, dst1xmm;
          __m128i msk0xmm, msk1xmm;
          __m128i mskinv0xmm, mskinv1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(dst0xmm);
          sse2_load4(msk0xmm, msk);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
          sse2_negate_2x2W(mskinv0xmm, msk0xmm, mskinv1xmm, msk1xmm);
          sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
          sse2_expand_alpha_2x2W(mskinv0xmm, mskinv0xmm, mskinv1xmm, mskinv1xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, mskinv0xmm, dst1xmm, dst1xmm, mskinv1xmm);
          sse2_adds_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
          msk += 4;
        SSE2_BLIT_32x4_LARGE_END(bltMaskLerp)
      }
      else
      {
        SSE2_BLIT_32x4_SMALL_BEGIN(bltMask)
          __m128i dst0xmm;
          __m128i msk0xmm;
          __m128i mskinv0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x1B(dst0xmm);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
          sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
          sse2_negate_1x1W(mskinv0xmm, msk0xmm);
          sse2_expand_alpha_1x1W(mskinv0xmm, mskinv0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, mskinv0xmm);
          sse2_adds_1x1W(dst0xmm, dst0xmm, msk0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
          msk += 1;
        SSE2_BLIT_32x4_SMALL_END(bltMask)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltMask)
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
        SSE2_BLIT_32x4_LARGE_END(bltMask)
      }
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i src0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);
      if (!ArgbUtil::isAlpha0x00(src0))
      {
        if (ArgbUtil::isAlpha0xFF(src0))
        {
          ((uint32_t*)dst)[0] = src0;
        }
        else
        {
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x1B(dst0xmm);
          sse2_unpack_1x1W(src0xmm, src0);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);

          sse2_expand_alpha_1x1W(a0xmm, src0xmm);
          sse2_fill_alpha_1x1W(src0xmm);
          sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);

          sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);

          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i src0xmm, src1xmm;
      __m128i msk0xmm, msk1xmm;

      sse2_load16u(src0xmm, src);

      SSE2_BLIT_TEST_4_ARGB_PIXELS(src0xmm, dst0xmm, dst1xmm, blt_fill, blt_away)

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
blt_fill:
      sse2_store16a(dst, src0xmm);
blt_away:
      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);

      if (FOG_LIKELY(src0))
      {
        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm;
      __m128i alp0xmm, alp1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);

      SSE2_BLIT_TEST_4_PRGB_PIXELS(src0xmm, alp0xmm, alp1xmm, blt_fill, blt_away)

      sse2_load16a(dst0xmm, dst);
      alp0xmm = _mm_shuffle_epi32(src0xmm, _MM_SHUFFLE(3, 2, 1, 0));
      alp0xmm = _mm_xor_si128(alp0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_fill_alpha_1x4B(dst0xmm);
      sse2_unpack_2x2W(alp0xmm, alp1xmm, alp0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_expand_alpha_2x2W(alp0xmm, alp0xmm, alp1xmm, alp1xmm);
      sse2_muldiv255_2x2W(alp0xmm, alp0xmm, dst0xmm, alp1xmm, alp1xmm, dst1xmm);
      sse2_pack_2x2W(alp0xmm, alp0xmm, alp1xmm);
      sse2_adds_1x4B(src0xmm, src0xmm, alp0xmm);
blt_fill:
      sse2_store16a(dst, src0xmm);
blt_away:
      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);
      uint32_t msk0 = READ_8(msk);

      if ((!ArgbUtil::isAlpha0x00(src0)) & (msk0 != 0x00))
      {
        msk0 = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);

        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_fill_alpha_1x1W(src0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      sse2_load16u(src0xmm, src);
      uint32_t src0a = sse2_pack_alpha_to_uint32(src0xmm);
      uint32_t msk0 = ((uint32_t*)msk)[0];

      if ((src0a != 0x00000000) & (msk0 != 0x00000000))
      {
        if ((src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
        {
          sse2_store16a(dst, src0xmm);
        }
        else
        {
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_expand_mask_2x2W(dst0xmm, dst1xmm, msk0);
          sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
          sse2_muldiv255_2x2W(a0xmm, a0xmm, dst0xmm, a1xmm, a1xmm, dst1xmm);

          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(dst0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_fill_alpha_2x2W(src0xmm, src1xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
          sse2_over_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);
      uint32_t msk0 = READ_8(msk);

      if ((src0 != 0x00000000) & (msk0 != 0x00))
      {
        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);
        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_mask_1x1W(a0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
        sse2_expand_alpha_1x1W(a0xmm, src0xmm);
        sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
        sse2_pack_1x1W(dst0xmm, dst0xmm);

        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      src += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      sse2_load16u(src0xmm, src);
      uint32_t src0a = sse2_pack_alpha_to_uint32(src0xmm);
      uint32_t msk0 = ((uint32_t*)msk)[0];

      if ((src0a != 0x00000000) & (msk0 != 0x00000000))
      {
        if ((src0a == 0xFFFFFFFF) & (msk0 == 0xFFFFFFFF))
        {
          sse2_store16a(dst, src0xmm);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(dst0xmm);

          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
          sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
          sse2_over_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm, im0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);

      if (!ArgbUtil::isAlpha0x00(src0))
      {
        if (ArgbUtil::isAlpha0xFF(src0))
        {
          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x1B(dst0xmm);

          sse2_unpack_1x1W(src0xmm, src0);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);

          sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);
          sse2_over_ialpha_1x1W(dst0xmm, src0xmm, im0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);

          sse2_store4(dst, dst0xmm);
        }
        else
        {
          uint32_t m0 = ByteUtil::scalar_muldiv255(src0 >> 24, msk0);

          sse2_load4(dst0xmm, dst);
          sse2_fill_alpha_1x1B(dst0xmm);

          sse2_unpack_1x1W(src0xmm, src0);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);

          sse2_expand_mask_1x1W(a0xmm, m0);
          sse2_fill_alpha_1x1W(src0xmm);
          sse2_muldiv255_1x1W(src0xmm, src0xmm, a0xmm);
          sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);

          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      sse2_load16u(src0xmm, src);
      uint32_t src0a = sse2_pack_alpha_to_uint32(src0xmm);

      if (src0a != 0x00000000)
      {
        if (src0a == 0xFFFFFFFF)
        {
          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(dst0xmm);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);
          sse2_over_ialpha_2x2W(dst0xmm, src0xmm, im0xmm, dst1xmm, src1xmm, im0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
        else
        {
          sse2_load16a(dst0xmm, dst);
          sse2_fill_alpha_1x4B(dst0xmm);

          sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
          sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
          sse2_muldiv255_2x2W(a0xmm, a0xmm, m0xmm, a1xmm, a1xmm, m0xmm);

          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_fill_alpha_2x2W(src0xmm, src1xmm);
          sse2_muldiv255_2x2W(src0xmm, src0xmm, a0xmm, src1xmm, src1xmm, a1xmm);
          sse2_over_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);

          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm, im0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(im0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i a0xmm;

      uint32_t src0 = READ_32(src);

      if (!(ArgbUtil::isAlpha0x00(src0)))
      {
        sse2_load4(dst0xmm, dst);
        sse2_fill_alpha_1x1B(dst0xmm);

        sse2_unpack_1x1W(src0xmm, src0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, m0xmm);

        if (ArgbUtil::isAlpha0xFF(src0))
        {
          sse2_over_ialpha_1x1W(dst0xmm, src0xmm, im0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
        else
        {
          sse2_expand_alpha_1x1W(a0xmm, src0xmm);
          sse2_over_1x1W(dst0xmm, src0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i a0xmm, a1xmm;

      sse2_load16u(src0xmm, src);
      uint32_t src0a = sse2_pack_alpha_to_uint32(src0xmm);

      if (src0a != 0x00000000)
      {
        sse2_load16a(dst0xmm, dst);
        sse2_fill_alpha_1x4B(dst0xmm);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, m0xmm, src1xmm, src1xmm, m0xmm);

        if (src0a == 0xFFFFFFFF)
        {
          sse2_over_ialpha_2x2W(dst0xmm, src0xmm, im0xmm, dst1xmm, src1xmm, im0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }
        else
        {
          sse2_expand_alpha_2x2W(a0xmm, src0xmm, a1xmm, src1xmm);
          sse2_over_2x2W(dst0xmm, src0xmm, a0xmm, dst1xmm, src1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - DstOver]
// ============================================================================

struct FOG_HIDDEN CompositeDstOverSSE2 :
  public CompositeCommonSSE2<CompositeDstOverSSE2>,
  public CompositeBaseFuncsSSE2<CompositeDstOverSSE2>,
  public CompositeMaskInSSE2<CompositeDstOverSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_DST_NON_ZERO };

  // PRGB32 destination.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_expand_alpha_1x1W(i0xmm, a0xmm);
    sse2_negate_1x1W(i0xmm, i0xmm);
    sse2_muldiv255_1x1W(i0xmm, i0xmm, b0xmm);
    sse2_adds_1x1W(dst0xmm, a0xmm, i0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOP
    dst0xmm = a0xmm;
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOP
    dst0xmm = a0xmm;
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOP
    dst0xmm = a0xmm;
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOP
    dst0xmm = a0xmm;
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm;
    __m128i i1xmm;

    sse2_expand_alpha_2x2W(i0xmm, a0xmm, i1xmm, a1xmm);
    sse2_negate_2x2W(i0xmm, i0xmm, i1xmm, i1xmm);
    sse2_muldiv255_2x2W(i0xmm, i0xmm, b0xmm, i1xmm, i1xmm, b1xmm);
    sse2_adds_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOP
    dst0xmm = a0xmm;
    dst1xmm = a1xmm;
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOP
    dst0xmm = a0xmm;
    dst1xmm = a1xmm;
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOP
    dst0xmm = a0xmm;
    dst1xmm = a1xmm;
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOP
    dst0xmm = a0xmm;
    dst1xmm = a1xmm;
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - SrcIn]
// ============================================================================

struct CompositeSrcInSSE2
{
  // -------------------------------------------------------------------------
  // [CompositeSrcInSSE2 - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_expand_alpha_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    sse2_unpack_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i tmp0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(tmp0xmm, dst0xmm);
        sse2_muldiv255_1x1W(tmp0xmm, tmp0xmm, src0xmm);

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(tmp0xmm, tmp0xmm, msk0xmm);

        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_adds_1x1W(dst0xmm, dst0xmm, tmp0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i tmp0xmm, tmp1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_expand_alpha_2x2W(tmp0xmm, dst0xmm, tmp1xmm, dst1xmm);
        sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, src0xmm, tmp1xmm, tmp1xmm, src0xmm);

        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, msk0xmm, tmp1xmm, tmp1xmm, msk1xmm);

        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_adds_2x2W(dst0xmm, dst0xmm, tmp0xmm, dst1xmm, dst1xmm, tmp1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;
    __m128i inv0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_mask_1x1W(inv0xmm, msk0);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    
    sse2_muldiv255_1x1W(src0xmm, src0xmm, inv0xmm);
    sse2_negate_1x1W(inv0xmm, inv0xmm);

    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(inv0xmm, inv0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;
      __m128i tmp0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(tmp0xmm, dst0xmm);
      sse2_muldiv255_1x1W(tmp0xmm, tmp0xmm, src0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, inv0xmm);
      sse2_adds_1x1W(dst0xmm, dst0xmm, tmp0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;
      __m128i tmp0xmm, tmp1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_expand_alpha_2x2W(tmp0xmm, dst0xmm, tmp1xmm, dst1xmm);
      sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, src0xmm, tmp1xmm, tmp1xmm, src0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, inv0xmm, dst1xmm, dst1xmm, inv0xmm);
      sse2_adds_2x2W(dst0xmm, dst0xmm, tmp0xmm, dst1xmm, dst1xmm, tmp1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    sse2_unpack_1x2W(src0xmm, src0xmm);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      __m128i msrc0xmm;
      __m128i minv0xmm;

      sse2_expand_mask_1x1W(msrc0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(msrc0xmm, msrc0xmm);
      sse2_negate_1x2W(minv0xmm, msrc0xmm);
      sse2_muldiv255_1x2W(msrc0xmm, msrc0xmm, src0xmm);

      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
        __m128i dst0xmm;
        __m128i tmp0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(tmp0xmm, dst0xmm);
        sse2_muldiv255_1x1W(tmp0xmm, tmp0xmm, msrc0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);
        sse2_adds_1x1W(dst0xmm, dst0xmm, tmp0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(bltConst)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
        __m128i dst0xmm, dst1xmm;
        __m128i tmp0xmm, tmp1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_expand_alpha_2x2W(tmp0xmm, dst0xmm, tmp1xmm, dst1xmm);
        sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, msrc0xmm, tmp1xmm, tmp1xmm, msrc0xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);
        sse2_adds_2x2W(dst0xmm, dst0xmm, tmp0xmm, dst1xmm, dst1xmm, tmp1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_LARGE_END(bltConst)
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(blt)
        uint32_t msk0 = READ_8(msk);

        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i tmp0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(tmp0xmm, dst0xmm);
        sse2_muldiv255_1x1W(tmp0xmm, tmp0xmm, src0xmm);

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(tmp0xmm, tmp0xmm, msk0xmm);

        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_adds_1x1W(dst0xmm, dst0xmm, tmp0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(blt)

      SSE2_BLIT_32x4_LARGE_BEGIN(blt)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i tmp0xmm, tmp1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_expand_alpha_2x2W(tmp0xmm, dst0xmm, tmp1xmm, dst1xmm);
        sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, src0xmm, tmp1xmm, tmp1xmm, src0xmm);

        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_muldiv255_2x2W(tmp0xmm, tmp0xmm, msk0xmm, tmp1xmm, tmp1xmm, msk1xmm);

        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_adds_2x2W(dst0xmm, dst0xmm, tmp0xmm, dst1xmm, dst1xmm, tmp1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_LARGE_END(blt)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_expand_alpha_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_premultiply_1x1W(src0xmm, src0xmm);

      sse2_expand_alpha_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

      sse2_expand_alpha_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_fill_alpha_1x4B(src0xmm);

      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_fill_alpha_1x4B(src0xmm);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_expand_alpha_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(msk0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_expand_alpha_2x2W(msk0xmm, dst0xmm, msk1xmm, dst1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, dst1xmm, msk1xmm);

        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_premultiply_1x1W(src0xmm, src0xmm);

        sse2_expand_alpha_1x1W(msk0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

        sse2_expand_alpha_2x2W(msk0xmm, dst0xmm, msk1xmm, dst1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, dst1xmm, msk1xmm);

        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);

        sse2_fill_alpha_1x4B(src0xmm);

        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_expand_alpha_1x1W(msk0xmm, dst0xmm);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);

        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);

        sse2_fill_alpha_1x4B(src0xmm);

        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_expand_alpha_2x2W(msk0xmm, dst0xmm, msk1xmm, dst1xmm);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, dst1xmm, msk1xmm);

        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x1W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i tmp0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(tmp0xmm, dst0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, tmp0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, inv0xmm);
      sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i tmp0xmm, tmp1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_expand_alpha_2x2W(tmp0xmm, dst0xmm, tmp1xmm, dst1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, tmp0xmm, src1xmm, src1xmm, tmp1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, inv0xmm, dst1xmm, dst1xmm, inv0xmm);
      sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x2W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i tmp0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_premultiply_1x1W(src0xmm, src0xmm);

      sse2_expand_alpha_1x1W(tmp0xmm, dst0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, tmp0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, inv0xmm);
      sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i tmp0xmm, tmp1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_premultiply_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);

      sse2_expand_alpha_2x2W(tmp0xmm, dst0xmm, tmp1xmm, dst1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, tmp0xmm, src0xmm, src0xmm, tmp1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, inv0xmm, dst1xmm, dst1xmm, inv0xmm);
      sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst0xmm, dst0xmm, src0xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_xrgb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;

    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x2W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i tmp0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_fill_alpha_1x4B(src0xmm);

      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(tmp0xmm, dst0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, tmp0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, inv0xmm);
      sse2_adds_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i tmp0xmm, tmp1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_fill_alpha_1x4B(src0xmm);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_expand_alpha_2x2W(tmp0xmm, dst0xmm, tmp1xmm, dst1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, tmp0xmm, src0xmm, src0xmm, tmp1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, inv0xmm, dst1xmm, dst1xmm, inv0xmm);
      sse2_adds_2x2W(dst0xmm, dst0xmm, src0xmm, dst0xmm, dst0xmm, src0xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - DstIn]
// ============================================================================

struct CompositeDstInSSE2
{
  // -------------------------------------------------------------------------
  // [CompositeDstInSSE2 - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_alpha_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_alpha_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_negate_1x1W(inv0xmm, msk0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_adds_1x1W(msk0xmm, msk0xmm, inv0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i inv0xmm, inv1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_negate_2x2W(inv0xmm, msk0xmm, inv1xmm, msk1xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_adds_2x2W(msk0xmm, msk0xmm, inv0xmm, msk1xmm, msk1xmm, inv1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;

    // Dca' = Dca.Sa.m + Dca.(1 - m)
    //
    // Sa.m and (1 - m) are constants, we can substitute formula as:
    // Dca.A + Dca.B = Dca.(A + B) -> Dca * msk0mm
    {
      __m128i src0xmm;
      __m128i inv0xmm;

      sse2_load4(src0xmm, &src->prgb);
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_negate_1x1W(inv0xmm, msk0xmm);
      sse2_expand_alpha_1x1W(src0xmm, src0xmm);
      sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
      sse2_adds_1x1W(msk0xmm, msk0xmm, inv0xmm);

      sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    }

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);
    sse2_unpack_1x2W(src0xmm, src0xmm);
    sse2_expand_alpha_1x2W(src0xmm, src0xmm);

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      __m128i msk0xmm;
      {
        __m128i inv0xmm;

        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_negate_1x1W(inv0xmm, msk0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_adds_1x1W(msk0xmm, msk0xmm, inv0xmm);

        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
      }

      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(bltConst)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
        __m128i dst0xmm, dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_LARGE_END(bltConst)
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(blt)
        __m128i dst0xmm;
        __m128i msk0xmm;
        __m128i inv0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_negate_1x1W(inv0xmm, msk0xmm);
        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_adds_1x1W(msk0xmm, msk0xmm, inv0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(blt)

      SSE2_BLIT_32x4_LARGE_BEGIN(blt)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;
        __m128i inv0xmm, inv1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);

        sse2_negate_2x2W(inv0xmm, msk0xmm, inv1xmm, msk1xmm);
        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_adds_2x2W(msk0xmm, msk0xmm, inv0xmm, msk1xmm, msk1xmm, inv1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_LARGE_END(blt)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(src0xmm, src0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_expand_alpha_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        sse2_load4(src0xmm, src);
        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(src0xmm, src0xmm);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_adds_1x1W(src0xmm, src0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_adds_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    __m128i inv0xmm;
    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
    sse2_negate_1x2W(inv0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(src0xmm, src0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
      sse2_adds_1x1W(src0xmm, src0xmm, inv0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_expand_alpha_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk0xmm);
      sse2_adds_2x2W(src0xmm, src0xmm, inv0xmm, src1xmm, src1xmm, inv0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - SrcOut]
// ============================================================================

struct FOG_HIDDEN CompositeSrcOutSSE2 :
  public CompositeCommonSSE2<CompositeSrcOutSSE2>,
  public CompositeBaseFuncsSSE2<CompositeSrcOutSSE2>,
  public CompositeMaskLerpInSSE2<CompositeSrcOutSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = 0 };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_negate_1x1W(dst0xmm, dst0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm = b0xmm;
    sse2_fill_alpha_1x1W(x0xmm);

    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_negate_1x1W(dst0xmm, dst0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_negate_1x1W(dst0xmm, dst0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_negate_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm = b0xmm, x1xmm = b1xmm;
    sse2_fill_alpha_2x2W(x0xmm, x1xmm);

    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_negate_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_negate_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - DstOut]
// ============================================================================

struct CompositeDstOutSSE2
{
  // -------------------------------------------------------------------------
  // [CompositeDstOutSSE2 - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_alpha_1x1W(src0xmm, src0xmm);
    sse2_negate_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_alpha_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      uint32_t msk0 = READ_8(msk);

      if (msk0 != 0x00)
      {
        __m128i dst0xmm;
        __m128i msk0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i src0xmm;

    {
      __m128i msk0xmm;

      sse2_load4(src0xmm, &src->prgb);
      sse2_expand_mask_1x1W(msk0xmm, msk0);
      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_expand_alpha_1x1W(src0xmm, src0xmm);
      sse2_muldiv255_1x1W(src0xmm, src0xmm, msk0xmm);
      sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);
    }

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src0xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    __m128i src0xmm;

    sse2_load4(src0xmm, &src->prgb);
    sse2_unpack_1x1W(src0xmm, src0xmm);
    sse2_expand_alpha_1x1W(src0xmm, src0xmm);
    sse2_expand_pixel_lo_1x2W(src0xmm, src0xmm);

    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      __m128i minv0xmm;
      sse2_expand_mask_1x1W(minv0xmm, msk0);
      sse2_expand_pixel_lo_1x2W(minv0xmm, minv0xmm);
      sse2_muldiv255_1x2W(minv0xmm, minv0xmm, src0xmm);
      sse2_negate_1x2W(minv0xmm, minv0xmm);

      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
        __m128i dst0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, minv0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
      SSE2_BLIT_32x4_SMALL_END(bltConst)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
        __m128i dst0xmm, dst1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, minv0xmm, dst1xmm, dst1xmm, minv0xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
      SSE2_BLIT_32x4_LARGE_END(bltConst)
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltMask)
        __m128i dst0xmm;
        __m128i msk0xmm;

        sse2_load4(dst0xmm, dst);
        sse2_expand_mask_1x1W(msk0xmm, READ_8(msk));
        sse2_unpack_1x1W(dst0xmm, dst0xmm);

        sse2_muldiv255_1x1W(msk0xmm, msk0xmm, src0xmm);
        sse2_negate_1x1W(msk0xmm, msk0xmm);
        sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

        sse2_pack_1x1W(dst0xmm, dst0xmm);
        sse2_store4(dst, dst0xmm);

        dst += 4;
        msk += 1;
      SSE2_BLIT_32x4_SMALL_END(bltMask)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltMask)
        __m128i dst0xmm, dst1xmm;
        __m128i msk0xmm, msk1xmm;

        sse2_load16a(dst0xmm, dst);
        sse2_load4(msk0xmm, msk);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_2x2W(msk0xmm, msk0xmm, src0xmm, msk1xmm, msk1xmm, src0xmm);
        sse2_negate_2x2W(msk0xmm, msk0xmm, msk1xmm, msk1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);

        dst += 16;
        msk += 4;
      SSE2_BLIT_32x4_LARGE_END(bltMask)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_unpack_1x1W(src0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(src0xmm, src0xmm);
      sse2_negate_1x1W(src0xmm, src0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, src0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;

      sse2_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      sse2_expand_alpha_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_negate_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32_a8(
    uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
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
      }

      src += 4;
      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i src0xmm, src1xmm;
      __m128i dst0xmm, dst1xmm;
      __m128i msk0xmm, msk1xmm;

      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        sse2_load16u(src0xmm, src);
        sse2_load16a(dst0xmm, dst);
        sse2_expand_mask_2x2W(msk0xmm, msk1xmm, msk0);
        sse2_unpack_2x2W(src0xmm, src1xmm, src0xmm);
        sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

        sse2_muldiv255_2x2W(src0xmm, src0xmm, msk0xmm, src1xmm, src1xmm, msk1xmm);
        sse2_negate_2x2W(src0xmm, src0xmm, src1xmm, src1xmm);
        sse2_muldiv255_2x2W(dst0xmm, dst0xmm, src0xmm, dst1xmm, dst1xmm, src1xmm);

        sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
        sse2_store16a(dst, dst0xmm);
      }

      src += 16;
      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_vspan_prgb32_or_argb32_a8_const(
    uint8_t* dst, const uint8_t* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i msk0xmm;
    sse2_expand_mask_1x1W(msk0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
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
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
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
    SSE2_BLIT_32x4_LARGE_END(blt)
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - SrcAtop]
// ============================================================================

struct FOG_HIDDEN CompositeSrcAtopSSE2 :
  public CompositeCommonSSE2<CompositeSrcAtopSSE2>,
  public CompositeBaseFuncsSSE2<CompositeSrcAtopSSE2>,
  public CompositeMaskInSSE2<CompositeSrcAtopSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO |
                OPERATOR_PROC_PRGB32_OP_XRGB32_PREF_0xFF
  };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;
    __m128i j0xmm;

    sse2_expand_alpha_1x1W(i0xmm, a0xmm);
    sse2_expand_alpha_1x1W(j0xmm, b0xmm);
    sse2_negate_1x1W(j0xmm, j0xmm);

    sse2_muldiv255_1x1W(i0xmm, i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, j0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, i0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i b0xmm_0xFF = b0xmm;
    sse2_fill_alpha_1x1W(b0xmm_0xFF);

    prgb32_op_xrgb32_0xFF_unpacked_1x1W(dst0xmm, a0xmm, b0xmm_0xFF);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;
    __m128i j0xmm, j1xmm;

    sse2_expand_alpha_2x2W(i0xmm, a0xmm, i1xmm, a1xmm);
    sse2_expand_alpha_2x2W(j0xmm, b0xmm, j1xmm, b1xmm);
    sse2_negate_2x2W(j0xmm, j0xmm, j1xmm, j1xmm);

    sse2_muldiv255_2x2W(i0xmm, i0xmm, b0xmm, i1xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, j0xmm, dst1xmm, a1xmm, j1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, i0xmm, dst1xmm, dst1xmm, i1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - DstAtop]
// ============================================================================

struct FOG_HIDDEN CompositeDstAtopSSE2 :
  public CompositeCommonSSE2<CompositeDstAtopSSE2>,
  public CompositeBaseFuncsSSE2<CompositeDstAtopSSE2>,
  public CompositeMaskLerpInSSE2<CompositeDstAtopSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = 0 };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    // dstmm[HI] = SRC, x0xmm[HI] = 1 - DA
    // dstmm[LO] = DST, x0xmm[LO] = SA

    dst0xmm = _mm_or_si128(a0xmm, _mm_slli_si128(b0xmm, 8));
    sse2_expand_alpha_1x2W(x0xmm, dst0xmm);
    x0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    sse2_negate_1x2W_hi(x0xmm, x0xmm);

    sse2_muldiv255_1x2W(dst0xmm, dst0xmm, x0xmm);
    dst0xmm = _mm_adds_epu8(dst0xmm, _mm_srli_si128(dst0xmm, 8));
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, a0xmm, x1xmm, a1xmm);
    sse2_expand_alpha_2x2W(y0xmm, b0xmm, y1xmm, b1xmm);
    sse2_negate_2x2W(x0xmm, x0xmm, x1xmm, x1xmm);

    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    // NOT USED!
    FOG_ASSERT_NOT_REACHED();
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Xor]
// ============================================================================

struct FOG_HIDDEN CompositeXorSSE2 :
  public CompositeCommonSSE2<CompositeXorSSE2>,
  public CompositeBaseFuncsSSE2<CompositeXorSSE2>,
  public CompositeMaskInSSE2<CompositeXorSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_DST_NON_ZERO };

  // PRGB32 destination.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    dst0xmm = _mm_or_si128(a0xmm, _mm_slli_si128(b0xmm, 8));
    sse2_expand_alpha_1x2W(x0xmm, dst0xmm);
    sse2_negate_1x2W(x0xmm, x0xmm);
    x0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    sse2_muldiv255_1x2W(dst0xmm, dst0xmm, x0xmm);
    x0xmm = _mm_shuffle_epi32(dst0xmm, _MM_SHUFFLE(1, 0, 3, 2));

    sse2_adds_1x1W(dst0xmm, dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm = b0xmm;
    sse2_fill_alpha_1x1W(x0xmm);

    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_negate_1x1W(dst0xmm, dst0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_negate_1x1W(dst0xmm, dst0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    dst0xmm = _mm_setzero_si128();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_expand_alpha_1x1W(x0xmm, b0xmm);
    sse2_negate_1x1W(x0xmm, x0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, a0xmm, x1xmm, a1xmm);
    sse2_expand_alpha_2x2W(y0xmm, b0xmm, y1xmm, b1xmm);

    sse2_negate_2x2W(x0xmm, x0xmm, x1xmm, x1xmm);
    sse2_negate_2x2W(y0xmm, y0xmm, y1xmm, y1xmm);

    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);

    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm = b0xmm;
    __m128i x1xmm = b1xmm;

    sse2_fill_alpha_2x2W(x0xmm, x1xmm);

    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_negate_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_negate_2x2W(dst0xmm, dst0xmm, dst1xmm, dst1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    dst0xmm = _mm_setzero_si128();
    dst1xmm = _mm_setzero_si128();
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm);
    sse2_negate_2x2W(x0xmm, x0xmm, x1xmm, x1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, x0xmm, dst1xmm, a1xmm, x1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Clear]
// ============================================================================

struct FOG_HIDDEN CompositeClearSSE2
{
  // -------------------------------------------------------------------------
  // [CompositeClearSSE2 - PRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    FOG_UNUSED(src);

    SSE2_BLIT_32x4_INIT(dst, w)

    __m128i mmzero = _mm_setzero_si128();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      sse2_store4(dst, mmzero);
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      sse2_store16a(dst, mmzero);
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    FOG_UNUSED(src);

    SSE2_BLIT_32x4_INIT(dst, w)

    __m128i mmzero = _mm_setzero_si128();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        if (msk0 == 0xFF)
        {
          sse2_store4(dst, mmzero);
        }
        else
        {
          __m128i dst0xmm;
          __m128i a0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(a0xmm, msk0);
          sse2_negate_1x1W(a0xmm, a0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        if (msk0 == 0xFFFFFFFF)
        {
          sse2_store16a(dst, mmzero);
        }
        else
        {
          __m128i dst0xmm, dst1xmm;
          __m128i a0xmm, a1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    FOG_UNUSED(src);

    __m128i m0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x2W(m0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, m0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, m0xmm, dst1xmm, dst1xmm, m0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL prgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    FOG_UNUSED(src);
    __m128i src0xmm = _mm_setzero_si128();

    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      if (msk0 == 0xFF)
      {
        SSE2_BLIT_32x4_INIT(dst, w)

        SSE2_BLIT_32x4_SMALL_BEGIN(bltTrans)
          sse2_store4(dst, src0xmm);
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltTrans)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltTrans)
          sse2_store16a(dst, src0xmm);
          dst += 16;
        SSE2_BLIT_32x4_LARGE_END(bltTrans)
      }
      else
      {
        __m128i msk0xmm;
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_negate_1x2W(msk0xmm, msk0xmm);

        SSE2_BLIT_32x4_INIT(dst, w)

        SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltConst)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
          __m128i dst0xmm, dst1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_LARGE_END(bltConst)
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltMask)
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
      SSE2_BLIT_32x4_SMALL_END(bltMask)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltMask)
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
      SSE2_BLIT_32x4_LARGE_END(bltMask)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }

  // -------------------------------------------------------------------------
  // [CompositeClearSSE2 - XRGB32]
  // -------------------------------------------------------------------------

  static void FOG_FASTCALL xrgb32_cspan(
    uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure)
  {
    FOG_UNUSED(src);

    SSE2_BLIT_32x4_INIT(dst, w)

    __m128i mmzero = SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      sse2_store4(dst, mmzero);
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      sse2_store16a(dst, mmzero);
      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8(
    uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure)
  {
    FOG_UNUSED(src);

    SSE2_BLIT_32x4_INIT(dst, w)

    __m128i mmzero = SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000);

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      uint32_t msk0 = READ_8(msk);
      if (msk0 != 0x00)
      {
        if (msk0 == 0xFF)
        {
          sse2_store4(dst, mmzero);
        }
        else
        {
          __m128i dst0xmm;
          __m128i a0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_expand_mask_1x1W(a0xmm, msk0);
          sse2_negate_1x1W(a0xmm, a0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, a0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
          sse2_store4(dst, dst0xmm);
        }
      }

      dst += 4;
      msk += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      uint32_t msk0 = READ_32(msk);
      if (msk0 != 0x00000000)
      {
        if (msk0 == 0xFFFFFFFF)
        {
          sse2_store16a(dst, mmzero);
        }
        else
        {
          __m128i dst0xmm, dst1xmm;
          __m128i a0xmm, a1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_expand_mask_2x2W(a0xmm, a1xmm, msk0);
          sse2_negate_2x2W(a0xmm, a0xmm, a1xmm, a1xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, a0xmm, dst1xmm, dst1xmm, a1xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
          sse2_store16a(dst, dst0xmm);
        }
      }

      dst += 16;
      msk += 4;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_const(
    uint8_t* dst, const Solid* src, uint32_t msk0, sysint_t w, const Closure* closure)
  {
    __m128i m0xmm;
    sse2_expand_mask_1x1W(m0xmm, msk0);
    sse2_expand_pixel_lo_1x2W(m0xmm, m0xmm);
    sse2_negate_1x1W(m0xmm, m0xmm);

    SSE2_BLIT_32x4_INIT(dst, w)

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i dst0xmm;

      sse2_load4(dst0xmm, dst);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, m0xmm);
      sse2_pack_1x1W(dst0xmm, dst0xmm);
      dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store4(dst, dst0xmm);

      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_LARGE_BEGIN(blt)
      __m128i dst0xmm, dst1xmm;

      sse2_load16a(dst0xmm, dst);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, m0xmm, dst1xmm, dst1xmm, m0xmm);
      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store16a(dst, dst0xmm);

      dst += 16;
    SSE2_BLIT_32x4_LARGE_END(blt)
  }

  static void FOG_FASTCALL xrgb32_cspan_a8_scanline(
    uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure)
  {
    FOG_UNUSED(src);
    __m128i src0xmm = SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000);

    BLIT_CSPAN_SCANLINE_STEP1_BEGIN(4)

    // Const mask.
    BLIT_CSPAN_SCANLINE_STEP2_CONST()
    {
      if (msk0 == 0xFF)
      {
        SSE2_BLIT_32x4_INIT(dst, w)

        SSE2_BLIT_32x4_SMALL_BEGIN(bltTrans)
          sse2_store4(dst, src0xmm);
          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltTrans)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltTrans)
          sse2_store16a(dst, src0xmm);
          dst += 16;
        SSE2_BLIT_32x4_LARGE_END(bltTrans)
      }
      else
      {
        __m128i msk0xmm;
        sse2_expand_mask_1x1W(msk0xmm, msk0);
        sse2_expand_pixel_lo_1x2W(msk0xmm, msk0xmm);
        sse2_negate_1x2W(msk0xmm, msk0xmm);

        SSE2_BLIT_32x4_INIT(dst, w)

        SSE2_BLIT_32x4_SMALL_BEGIN(bltConst)
          __m128i dst0xmm;

          sse2_load4(dst0xmm, dst);
          sse2_unpack_1x1W(dst0xmm, dst0xmm);
          sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);
          sse2_pack_1x1W(dst0xmm, dst0xmm);
          sse2_fill_alpha_1x1B(dst0xmm);
          sse2_store4(dst, dst0xmm);

          dst += 4;
        SSE2_BLIT_32x4_SMALL_END(bltConst)

        SSE2_BLIT_32x4_LARGE_BEGIN(bltConst)
          __m128i dst0xmm, dst1xmm;

          sse2_load16a(dst0xmm, dst);
          sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);
          sse2_muldiv255_2x2W(dst0xmm, dst0xmm, msk0xmm, dst1xmm, dst1xmm, msk0xmm);
          sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
          sse2_fill_alpha_1x4B(dst0xmm);
          sse2_store16a(dst, dst0xmm);

          dst += 16;
        SSE2_BLIT_32x4_LARGE_END(bltConst)
      }
    }
    // Variable mask.
    BLIT_CSPAN_SCANLINE_STEP3_MASK()
    {
      SSE2_BLIT_32x4_INIT(dst, w)

      SSE2_BLIT_32x4_SMALL_BEGIN(bltMask)
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
      SSE2_BLIT_32x4_SMALL_END(bltMask)

      SSE2_BLIT_32x4_LARGE_BEGIN(bltMask)
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
      SSE2_BLIT_32x4_LARGE_END(bltMask)
    }
    BLIT_CSPAN_SCANLINE_STEP4_END()
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Add]
// ============================================================================

struct FOG_HIDDEN CompositeAddSSE2 :
  public CompositeCommonSSE2<CompositeAddSSE2>,
  public CompositeBaseFuncsSSE2<CompositeAddSSE2>,
  public CompositeMaskInSSE2<CompositeAddSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_PACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1B(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x1B(dst0xmm, a0xmm, b0xmm);
  }


  static FOG_INLINE void prgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x4B(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_adds_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_adds_1x4B(dst1xmm, a1xmm, b1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Subtract]
// ============================================================================

struct FOG_HIDDEN CompositeSubtractSSE2 :
  public CompositeCommonSSE2<CompositeSubtractSSE2>,
  public CompositeBaseFuncsSSE2<CompositeSubtractSSE2>,
  public CompositeMaskInSSE2<CompositeSubtractSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_PACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    x0xmm = _mm_srli_epi32(a0xmm, 24);
    y0xmm = _mm_srli_epi32(b0xmm, 24);

    x0xmm = _mm_xor_si128(x0xmm, SSE2_GET_CONST_PI(000000FF000000FF_000000FF000000FF));
    y0xmm = _mm_xor_si128(y0xmm, SSE2_GET_CONST_PI(000000FF000000FF_000000FF000000FF));

    sse2_muldiv255_1x1W(x0xmm, x0xmm, y0xmm);

    x0xmm = _mm_xor_si128(x0xmm, SSE2_GET_CONST_PI(000000FF000000FF_000000FF000000FF));

    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_zero_alpha_1x1B(dst0xmm);

    dst0xmm = _mm_or_si128(dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1B(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1B(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x1B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    x0xmm = _mm_srli_epi32(a0xmm, 24);
    y0xmm = _mm_srli_epi32(b0xmm, 24);

    x0xmm = _mm_xor_si128(x0xmm, SSE2_GET_CONST_PI(000000FF000000FF_000000FF000000FF));
    y0xmm = _mm_xor_si128(y0xmm, SSE2_GET_CONST_PI(000000FF000000FF_000000FF000000FF));

    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);

    x0xmm = _mm_xor_si128(x0xmm, SSE2_GET_CONST_PI(000000FF000000FF_000000FF000000FF));
    x0xmm = _mm_slli_epi32(x0xmm, 24);

    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_zero_alpha_1x4B(dst0xmm);

    dst0xmm = _mm_or_si128(dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x4B(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x4B(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_packed_1x4B(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    x0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));
    y0xmm = _mm_xor_si128(b0xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));

    sse2_muldiv255_1x1W(x0xmm, x0xmm, y0xmm);

    x0xmm = _mm_and_si128(a0xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));
    x0xmm = _mm_xor_si128(x0xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));

    sse2_subs_1x1W(dst0xmm, a0xmm, b0xmm);
    sse2_zero_alpha_1x1W(dst0xmm);

    dst0xmm = _mm_or_si128(dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    x0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));
    x1xmm = _mm_xor_si128(a1xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));
    y0xmm = _mm_xor_si128(b0xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));
    y1xmm = _mm_xor_si128(b1xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));

    sse2_muldiv255_2x2W(x0xmm, x0xmm, y0xmm, x1xmm, x1xmm, y1xmm);

    x0xmm = _mm_and_si128(a0xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));
    x1xmm = _mm_and_si128(a1xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));
    x0xmm = _mm_xor_si128(x0xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));
    x1xmm = _mm_xor_si128(x1xmm, SSE2_GET_CONST_PI(00000000000000FF_00000000000000FF));

    sse2_subs_2x2W(dst0xmm, a0xmm, b0xmm, dst1xmm, a1xmm, b1xmm);
    sse2_zero_alpha_2x2W(dst0xmm, dst1xmm);

    dst0xmm = _mm_or_si128(dst0xmm, x0xmm);
    dst1xmm = _mm_or_si128(dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_subs_1x4B(dst1xmm, a1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_subs_1x4B(dst1xmm, a1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_subs_1x4B(dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    sse2_subs_1x4B(dst0xmm, a0xmm, b0xmm);
    sse2_subs_1x4B(dst1xmm, a1xmm, b1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Multiply]
// ============================================================================

struct FOG_HIDDEN CompositeMultiplySSE2 :
  public CompositeCommonSSE2<CompositeMultiplySSE2>,
  public CompositeBaseFuncsSSE2<CompositeMultiplySSE2>,
  public CompositeMaskInSSE2<CompositeMultiplySSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_zero_alpha_1x1W(i0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_zero_alpha_2x2W(i0xmm, i1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Screen]
// ============================================================================

struct FOG_HIDDEN CompositeScreenSSE2 :
  public CompositeCommonSSE2<CompositeScreenSSE2>,
  public CompositeBaseFuncsSSE2<CompositeScreenSSE2>,
  public CompositeMaskInSSE2<CompositeScreenSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_zero_alpha_1x1W(i0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i i0xmm;

    sse2_negate_1x1W(i0xmm, b0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, i0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_zero_alpha_2x2W(i0xmm, i1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i i0xmm, i1xmm;

    sse2_negate_2x2W(i0xmm, b0xmm, i1xmm, b1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, i0xmm, dst1xmm, a1xmm, i1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Darken]
// ============================================================================

struct FOG_HIDDEN CompositeDarkenSSE2 :
  public CompositeCommonSSE2<CompositeDarkenSSE2>,
  public CompositeBaseFuncsSSE2<CompositeDarkenSSE2>,
  public CompositeMaskInSSE2<CompositeDarkenSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;
    __m128i z0xmm;

    // x0xmm = SA SR SG SB DA DR DG DB
    x0xmm = _mm_slli_si128(b0xmm, 8);
    x0xmm = _mm_or_si128(x0xmm, a0xmm);

    // y0xmm = DA DA DA DA SA SA SA SA
    sse2_expand_alpha_1x2W(y0xmm, x0xmm);
    y0xmm = _mm_shuffle_epi32(y0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    z0xmm = x0xmm;

    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    sse2_negate_1x2W(y0xmm, y0xmm);
    sse2_muldiv255_1x2W(z0xmm, z0xmm, y0xmm);

    dst0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    y0xmm = _mm_shuffle_epi32(z0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    dst0xmm = _mm_min_epi16(dst0xmm, x0xmm); // dst0xmm  = min(Sca.Da, Dca.Sa)
    dst0xmm = _mm_adds_epu8(dst0xmm, y0xmm); // dst0xmm += Sca.(1 - Da)
    dst0xmm = _mm_adds_epu8(dst0xmm, z0xmm); // dst0xmm += Dca.(1 - Sa)
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, a0xmm);
    sse2_expand_pixel_lo_1x2W(x0xmm, b0xmm);
    sse2_expand_pixel_lo_1x2W(y0xmm, y0xmm);

    y0xmm = _mm_xor_si128(y0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_0000000000000000));
    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    y0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    dst0xmm = _mm_min_epi16(a0xmm, x0xmm);   // dst0xmm = min(Sc.Da, Dca)
    dst0xmm = _mm_adds_epu8(dst0xmm, y0xmm); // dst0xmm += Sc.(1 - Da)
    dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    prgb32_op_xrgb32_0xXX_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    dst0xmm = _mm_min_epi16(a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, b0xmm);
    sse2_expand_pixel_lo_1x2W(x0xmm, a0xmm);
    sse2_expand_pixel_lo_1x2W(y0xmm, y0xmm);

    y0xmm = _mm_xor_si128(y0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_0000000000000000));
    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    y0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    dst0xmm = _mm_min_epi16(b0xmm, x0xmm);   // dst0xmm = min(Sca, Dca.Sa)
    dst0xmm = _mm_adds_epu8(dst0xmm, y0xmm); // dst0xmm += Dc.(1 - Sa)
    dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm); // x0xmm = Sa
    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm); // y0xmm = Da
    sse2_muldiv255_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm); // x0xmm = Dca.Sa
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm); // y0xmm = Sca.Da

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, x0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, x1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, y1xmm);

    x0xmm = _mm_min_epi16(x0xmm, y0xmm); // x0xmm = min(Dca.Sa, Sca.Da)
    x1xmm = _mm_min_epi16(x1xmm, y1xmm); // x1xmm = min(Dca.Sa, Sca.Da)

    dst0xmm = _mm_add_epi16(dst0xmm, x0xmm);
    dst1xmm = _mm_add_epi16(dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm); // y0xmm = Da
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm); // y0xmm = Sc.Da

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, y1xmm);

    y0xmm = _mm_min_epi16(y0xmm, a0xmm); // x0xmm = min(Dca, Sc.Da)
    y1xmm = _mm_min_epi16(y1xmm, a1xmm); // x1xmm = min(Dca, Sc.Da)

    dst0xmm = _mm_add_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(dst1xmm, y1xmm);

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm); // y0xmm = Da
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm); // y0xmm = Sc.Da

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, y1xmm);

    y0xmm = _mm_min_epi16(y0xmm, a0xmm); // x0xmm = min(Dca, Sc.Da)
    y1xmm = _mm_min_epi16(y1xmm, a1xmm); // x1xmm = min(Dca, Sc.Da)

    dst0xmm = _mm_add_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(dst1xmm, y1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    dst0xmm = _mm_min_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_min_epi16(a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm); // x0xmm = Sa
    sse2_negate_2x2W(y0xmm, x0xmm, y1xmm, x1xmm); // y0xmm = 1 - Sa
    sse2_muldiv255_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm); // Dc.Sa
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm); // Dc.(1 - Sa)

    x0xmm = _mm_min_epi16(x0xmm, b0xmm); // x0xmm = min(Sca, Dc.Sa)
    x1xmm = _mm_min_epi16(x1xmm, b1xmm); // x1xmm = min(Sca, Dc.Sa)

    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Lighten]
// ============================================================================

struct FOG_HIDDEN CompositeLightenSSE2 :
  public CompositeCommonSSE2<CompositeLightenSSE2>,
  public CompositeBaseFuncsSSE2<CompositeLightenSSE2>,
  public CompositeMaskInSSE2<CompositeLightenSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;
    __m128i z0xmm;

    // x0xmm = SA SR SG SB DA DR DG DB
    x0xmm = _mm_slli_si128(b0xmm, 8);
    x0xmm = _mm_or_si128(x0xmm, a0xmm);

    // y0xmm = DA DA DA DA SA SA SA SA
    sse2_expand_alpha_1x2W(y0xmm, x0xmm);
    y0xmm = _mm_shuffle_epi32(y0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    z0xmm = x0xmm;

    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    sse2_negate_1x2W(y0xmm, y0xmm);
    sse2_muldiv255_1x2W(z0xmm, z0xmm, y0xmm);

    dst0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    y0xmm = _mm_shuffle_epi32(z0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    dst0xmm = _mm_max_epi16(dst0xmm, x0xmm); // dst0xmm  = max(Sca.Da, Dca.Sa)
    dst0xmm = _mm_adds_epu8(dst0xmm, y0xmm); // dst0xmm += Sca.(1 - Da)
    dst0xmm = _mm_adds_epu8(dst0xmm, z0xmm); // dst0xmm += Dca.(1 - Sa)
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, a0xmm);
    sse2_expand_pixel_lo_1x2W(x0xmm, b0xmm);
    sse2_expand_pixel_lo_1x2W(y0xmm, y0xmm);

    y0xmm = _mm_xor_si128(y0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_0000000000000000));
    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    y0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    dst0xmm = _mm_max_epi16(a0xmm, x0xmm);   // dst0xmm = max(Sc.Da, Dca)
    dst0xmm = _mm_adds_epu8(dst0xmm, y0xmm); // dst0xmm += Sc.(1 - Da)
    dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    prgb32_op_xrgb32_0xXX_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    dst0xmm = _mm_max_epi16(a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, b0xmm);
    sse2_expand_pixel_lo_1x2W(x0xmm, a0xmm);
    sse2_expand_pixel_lo_1x2W(y0xmm, y0xmm);

    y0xmm = _mm_xor_si128(y0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_0000000000000000));
    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    y0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    dst0xmm = _mm_max_epi16(b0xmm, x0xmm);   // dst0xmm = max(Sca, Dca.Sa)
    dst0xmm = _mm_adds_epu8(dst0xmm, y0xmm); // dst0xmm += Dc.(1 - Sa)
    dst0xmm = _mm_or_si128(dst0xmm, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm); // x0xmm = Sa
    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm); // y0xmm = Da
    sse2_muldiv255_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm); // x0xmm = Dca.Sa
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm); // y0xmm = Sca.Da

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, x0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, x1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, y1xmm);

    x0xmm = _mm_max_epi16(x0xmm, y0xmm); // x0xmm = min(Dca.Sa, Sca.Da)
    x1xmm = _mm_max_epi16(x1xmm, y1xmm); // x1xmm = min(Dca.Sa, Sca.Da)

    dst0xmm = _mm_add_epi16(dst0xmm, x0xmm);
    dst1xmm = _mm_add_epi16(dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm); // y0xmm = Da
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm); // y0xmm = Sc.Da

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, y1xmm);

    y0xmm = _mm_max_epi16(y0xmm, a0xmm); // x0xmm = min(Dca, Sc.Da)
    y1xmm = _mm_max_epi16(y1xmm, a1xmm); // x1xmm = min(Dca, Sc.Da)

    dst0xmm = _mm_add_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(dst1xmm, y1xmm);

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm); // y0xmm = Da
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm); // y0xmm = Sc.Da

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);

    dst0xmm = _mm_sub_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_sub_epi16(dst1xmm, y1xmm);

    y0xmm = _mm_max_epi16(y0xmm, a0xmm); // x0xmm = min(Dca, Sc.Da)
    y1xmm = _mm_max_epi16(y1xmm, a1xmm); // x1xmm = min(Dca, Sc.Da)

    dst0xmm = _mm_add_epi16(dst0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(dst1xmm, y1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    dst0xmm = _mm_max_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_max_epi16(a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm); // x0xmm = Sa
    sse2_negate_2x2W(y0xmm, x0xmm, y1xmm, x1xmm); // y0xmm = 1 - Sa
    sse2_muldiv255_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm); // Dc.Sa
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm); // Dc.(1 - Sa)

    x0xmm = _mm_max_epi16(x0xmm, b0xmm); // x0xmm = max(Sca, Dc.Sa)
    x1xmm = _mm_max_epi16(x1xmm, b1xmm); // x1xmm = max(Sca, Dc.Sa)

    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Difference]
// ============================================================================

struct FOG_HIDDEN CompositeDifferenceSSE2 :
  public CompositeCommonSSE2<CompositeDifferenceSSE2>,
  public CompositeBaseFuncsSSE2<CompositeDifferenceSSE2>,
  public CompositeMaskInSSE2<CompositeDifferenceSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;
  
    // x0xmm = SA SR SG SB DA DR DG DB
    x0xmm = _mm_slli_si128(b0xmm, 8);
    x0xmm = _mm_or_si128(x0xmm, a0xmm);

    // y0xmm = DA DA DA DA SA SA SA SA
    sse2_expand_alpha_1x2W(y0xmm, x0xmm);
    y0xmm = _mm_shuffle_epi32(y0xmm, _MM_SHUFFLE(1, 0, 3, 2));

    // x0xmm = min(x0xmm, y0xmm)
    sse2_muldiv255_1x2W(x0xmm, x0xmm, y0xmm);
    y0xmm = _mm_shuffle_epi32(x0xmm, _MM_SHUFFLE(1, 0, 3, 2));
    x0xmm = _mm_min_epi16(x0xmm, y0xmm);

    // x0xmm *= 2 (except alpha)
    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_expand_alpha_1x1W(x0xmm, a0xmm);
    sse2_muldiv255_1x1W(x0xmm, x0xmm, b0xmm);
    x0xmm = _mm_min_epi16(x0xmm, a0xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    prgb32_op_xrgb32_0xXX_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    x0xmm = a0xmm;
    x0xmm = _mm_min_epi16(x0xmm, b0xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_expand_alpha_1x1W(x0xmm, b0xmm);
    sse2_muldiv255_1x1W(x0xmm, x0xmm, a0xmm);
    x0xmm = _mm_min_epi16(x0xmm, a0xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, a0xmm, x1xmm, a1xmm);
    sse2_expand_alpha_2x2W(y0xmm, b0xmm, y1xmm, b1xmm);

    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);
    sse2_muldiv255_2x2W(y0xmm, y0xmm, a0xmm, y1xmm, y1xmm, a1xmm);

    x0xmm = _mm_min_epi16(x0xmm, y0xmm);
    x1xmm = _mm_min_epi16(x1xmm, y1xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    sse2_expand_alpha_2x2W(x0xmm, a0xmm, x1xmm, a1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);
    x0xmm = _mm_min_epi16(x0xmm, a0xmm);
    x1xmm = _mm_min_epi16(x1xmm, a1xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    prgb32_op_xrgb32_0xXX_unpacked_2x2W(dst0xmm, a0xmm, b0xmm, dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    x0xmm = a0xmm;
    x1xmm = a1xmm;
    x0xmm = _mm_min_epi16(x0xmm, b0xmm);
    x1xmm = _mm_min_epi16(x1xmm, b1xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);
    x1xmm = _mm_slli_epi16(x1xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    sse2_expand_alpha_2x2W(x0xmm, b0xmm, x1xmm, b1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm);

    x0xmm = _mm_min_epi16(x0xmm, a0xmm);
    x1xmm = _mm_min_epi16(x1xmm, a1xmm);
    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Exclusion]
// ============================================================================

struct FOG_HIDDEN CompositeExclusionSSE2 :
  public CompositeCommonSSE2<CompositeExclusionSSE2>,
  public CompositeBaseFuncsSSE2<CompositeExclusionSSE2>,
  public CompositeMaskInSSE2<CompositeExclusionSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, a0xmm);
    sse2_fill_alpha_1x1W(y0xmm);
    sse2_muldiv255_1x1W(x0xmm, a0xmm, b0xmm);
    sse2_muldiv255_1x1W(y0xmm, y0xmm, b0xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst0xmm = _mm_min_epi16(dst0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(y0xmm, a0xmm);
    sse2_fill_alpha_1x1W(y0xmm);
    sse2_muldiv255_1x1W(x0xmm, a0xmm, b0xmm);
    sse2_muldiv255_1x1W(y0xmm, y0xmm, b0xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst0xmm = _mm_min_epi16(dst0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    prgb32_op_prgb32_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_muldiv255_1x1W(x0xmm, a0xmm, b0xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst0xmm = _mm_min_epi16(dst0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;

    sse2_muldiv255_1x1W(x0xmm, a0xmm, b0xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst0xmm = _mm_min_epi16(dst0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm);
    sse2_fill_alpha_2x2W(y0xmm, y1xmm);
    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, y1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm);
    sse2_fill_alpha_2x2W(y0xmm, y0xmm);
    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, y1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(y0xmm, a0xmm, y1xmm, a1xmm);
    sse2_fill_alpha_2x2W(y0xmm, y0xmm);
    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    sse2_muldiv255_2x2W(y0xmm, y0xmm, b0xmm, y1xmm, y1xmm, b1xmm);

    x0xmm = _mm_mullo_epi16(x0xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));
    x1xmm = _mm_mullo_epi16(x1xmm, SSE2_GET_CONST_PI(0001000200020002_0001000200020002));

    dst0xmm = _mm_add_epi16(a0xmm, y0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, y1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;

    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);
    x1xmm = _mm_slli_epi16(x1xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
  //!   Dc'  = Sca + Dc - 2.Sca.Dc
    __m128i x0xmm, x1xmm;

    sse2_muldiv255_2x2W(x0xmm, a0xmm, b0xmm, x1xmm, a1xmm, b1xmm);
    x0xmm = _mm_slli_epi16(x0xmm, 1);
    x1xmm = _mm_slli_epi16(x1xmm, 1);

    dst0xmm = _mm_add_epi16(a0xmm, b0xmm);
    dst1xmm = _mm_add_epi16(a1xmm, b1xmm);
    dst0xmm = _mm_subs_epu16(dst0xmm, x0xmm);
    dst1xmm = _mm_subs_epu16(dst1xmm, x1xmm);

    dst0xmm = _mm_min_epi16(dst0xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
    dst1xmm = _mm_min_epi16(dst1xmm, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));

    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - Invert]
// ============================================================================

struct FOG_HIDDEN CompositeInvertSSE2 :
  public CompositeCommonSSE2<CompositeInvertSSE2>,
  public CompositeBaseFuncsSSE2<CompositeInvertSSE2>,
  public CompositeMaskInSSE2<CompositeInvertSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(x0xmm, a0xmm);
    sse2_expand_alpha_1x1W(y0xmm, b0xmm);
    sse2_subs_1x1W(x0xmm, x0xmm, a0xmm);
    sse2_fill_alpha_1x1W(x0xmm);
    sse2_muldiv255_1x1W(x0xmm, x0xmm, y0xmm);
    sse2_negate_1x1W(y0xmm, y0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, y0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm = a0xmm;
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_subs_1x1W(dst0xmm, dst0xmm, x0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    prgb32_op_xrgb32_0xXX_unpacked_1x1W(dst0xmm, a0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    dst0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    x0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
    sse2_expand_alpha_1x1W(y0xmm, b0xmm);
    sse2_muldiv255_1x1W(x0xmm, x0xmm, y0xmm);
    sse2_negate_1x1W(y0xmm, y0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, y0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, a0xmm, x1xmm, a1xmm);
    sse2_expand_alpha_2x2W(y0xmm, b0xmm, y1xmm, b1xmm);
    sse2_subs_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm);
    sse2_fill_alpha_2x2W(x0xmm, x1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, y0xmm, x1xmm, x1xmm, y1xmm);
    sse2_negate_2x2W(y0xmm, y0xmm, y1xmm, y1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm = a0xmm, x1xmm = a1xmm;
    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_subs_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    prgb32_op_xrgb32_0xXX_unpacked_2x2W(dst0xmm, a0xmm, b0xmm, dst1xmm, a1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    dst0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
    dst1xmm = _mm_xor_si128(a1xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    x0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
    x1xmm = _mm_xor_si128(a1xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));

    sse2_expand_alpha_2x2W(y0xmm, b0xmm, y1xmm, b1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, y0xmm, x1xmm, x1xmm, y1xmm);
    sse2_negate_2x2W(y0xmm, y0xmm, y1xmm, y1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Composite - InvertRgb]
// ============================================================================

struct FOG_HIDDEN CompositeInvertRgbSSE2 :
  public CompositeCommonSSE2<CompositeInvertRgbSSE2>,
  public CompositeBaseFuncsSSE2<CompositeInvertRgbSSE2>,
  public CompositeMaskInSSE2<CompositeInvertRgbSSE2>
{
  // Operator definitions.
  enum { MODE = OPERATOR_MODE_UNPACKED };
  enum { PROC = OPERATOR_PROC_SRC_NON_ZERO };

  // Template.
  static FOG_INLINE void prgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    sse2_expand_alpha_1x1W(x0xmm, a0xmm);
    sse2_expand_alpha_1x1W(y0xmm, b0xmm);
    sse2_subs_1x1W(x0xmm, x0xmm, a0xmm);
    sse2_fill_alpha_1x1W(x0xmm);
    sse2_muldiv255_1x1W(x0xmm, x0xmm, b0xmm);
    sse2_negate_1x1W(y0xmm, y0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, y0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm = a0xmm;
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_subs_1x1W(dst0xmm, dst0xmm, x0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, b0xmm);
    sse2_fill_alpha_1x1W(dst0xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm = a0xmm;
    sse2_expand_alpha_1x1W(dst0xmm, a0xmm);
    sse2_subs_1x1W(dst0xmm, dst0xmm, x0xmm);
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    dst0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
    sse2_muldiv255_1x1W(dst0xmm, dst0xmm, b0xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_1x1W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm)
  {
    __m128i x0xmm;
    __m128i y0xmm;

    x0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
    sse2_expand_alpha_1x1W(y0xmm, b0xmm);
    sse2_fill_alpha_1x1W(x0xmm);
    sse2_muldiv255_1x1W(x0xmm, x0xmm, b0xmm);
    sse2_negate_1x1W(y0xmm, y0xmm);
    sse2_muldiv255_1x1W(dst0xmm, a0xmm, y0xmm);
    sse2_adds_1x1W(dst0xmm, dst0xmm, x0xmm);
  }

  static FOG_INLINE void prgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    sse2_expand_alpha_2x2W(x0xmm, a0xmm, x1xmm, a1xmm);
    sse2_expand_alpha_2x2W(y0xmm, b0xmm, y1xmm, b1xmm);
    sse2_subs_2x2W(x0xmm, x0xmm, a0xmm, x1xmm, x1xmm, a1xmm);
    sse2_fill_alpha_2x2W(x0xmm, x1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);
    sse2_negate_2x2W(y0xmm, y0xmm, y1xmm, y1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xXX_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm = a0xmm, x1xmm = a1xmm;
    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_subs_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
    sse2_fill_alpha_2x2W(dst0xmm, dst1xmm);
  }

  static FOG_INLINE void prgb32_op_xrgb32_0xFF_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm = a0xmm, x1xmm = a1xmm;
    sse2_expand_alpha_2x2W(dst0xmm, a0xmm, dst1xmm, a1xmm);
    sse2_subs_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    dst0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
    dst1xmm = _mm_xor_si128(a1xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
    sse2_muldiv255_2x2W(dst0xmm, dst0xmm, b0xmm, dst1xmm, dst1xmm, b1xmm);
  }

  static FOG_INLINE void xrgb32_op_prgb32_unpacked_2x2W(
    __m128i& dst0xmm, const __m128i& a0xmm, const __m128i& b0xmm,
    __m128i& dst1xmm, const __m128i& a1xmm, const __m128i& b1xmm)
  {
    __m128i x0xmm, x1xmm;
    __m128i y0xmm, y1xmm;

    x0xmm = _mm_xor_si128(a0xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
    x1xmm = _mm_xor_si128(a1xmm, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));

    sse2_expand_alpha_2x2W(y0xmm, b0xmm, y1xmm, b1xmm);
    sse2_fill_alpha_2x2W(x0xmm, x1xmm);
    sse2_muldiv255_2x2W(x0xmm, x0xmm, b0xmm, x1xmm, x1xmm, b1xmm);
    sse2_negate_2x2W(y0xmm, y0xmm, y1xmm, y1xmm);
    sse2_muldiv255_2x2W(dst0xmm, a0xmm, y0xmm, dst1xmm, a1xmm, y1xmm);
    sse2_adds_2x2W(dst0xmm, dst0xmm, x0xmm, dst1xmm, dst1xmm, x1xmm);
  }
};

} // RasterEngine namespace
} // Fog namespace
