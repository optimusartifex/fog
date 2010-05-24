// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Dib_SSSE3_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::SSSE3 - Composite - SrcOver]
// ============================================================================

FOG_DECLARE_SSE_CONST_PI8_VAR(SSSE3_unpack_argb_1x1W_from_1x1B,
  0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80,
  0x80, 0x03, 0x80, 0x02,
  0x80, 0x01, 0x80, 0x00);

FOG_DECLARE_SSE_CONST_PI8_VAR(SSSE3_unpack_argb_2x1W_from_4x1B_lo,
  0x80, 0x07, 0x80, 0x06,
  0x80, 0x05, 0x80, 0x04,
  0x80, 0x03, 0x80, 0x02,
  0x80, 0x01, 0x80, 0x00);

FOG_DECLARE_SSE_CONST_PI8_VAR(SSSE3_unpack_argb_2x1W_from_4x1B_hi,
  0x80, 0x0F, 0x80, 0x0E,
  0x80, 0x0D, 0x80, 0x0C,
  0x80, 0x0B, 0x80, 0x0A,
  0x80, 0x09, 0x80, 0x08);

FOG_DECLARE_SSE_CONST_PI8_VAR(SSSE3_unpack_0rgb_1x1W_from_1x1B,
  0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x02,
  0x80, 0x01, 0x80, 0x00);

FOG_DECLARE_SSE_CONST_PI8_VAR(SSSE3_unpack_aaaa_1x1W_from_1x1B,
  0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80,
  0x80, 0x03, 0x80, 0x03,
  0x80, 0x03, 0x80, 0x03);

FOG_DECLARE_SSE_CONST_PI8_VAR(SSSE3_unpack_aaaa_2x1W_from_4x1B_lo,
  0x80, 0x07, 0x80, 0x07,
  0x80, 0x07, 0x80, 0x07,
  0x80, 0x03, 0x80, 0x03,
  0x80, 0x03, 0x80, 0x03);

FOG_DECLARE_SSE_CONST_PI8_VAR(SSSE3_unpack_aaaa_2x1W_from_4x1B_hi,
  0x80, 0x0F, 0x80, 0x0F,
  0x80, 0x0F, 0x80, 0x0F,
  0x80, 0x0B, 0x80, 0x0B,
  0x80, 0x0B, 0x80, 0x0B);

template<typename T>
static FOG_INLINE void ssse3_load16u(__m128i& dst, const T* src)
{
  dst = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(src));
}

template<uint8_t T00, uint8_t T01, uint8_t T02, uint8_t T03,
         uint8_t T04, uint8_t T05, uint8_t T06, uint8_t T07,
         uint8_t T08, uint8_t T09, uint8_t T10, uint8_t T11,
         uint8_t T12, uint8_t T13, uint8_t T14, uint8_t T15>
static FOG_INLINE void ssse3_pshufb(__m128i& dst, __m128i src)
{
  FOG_DECLARE_SSE_CONST_PI8_VAR(c,
    T00, T01, T02, T03,
    T04, T05, T06, T07,
    T08, T09, T10, T11,
    T12, T13, T14, T15);
  dst = _mm_shuffle_epi8(src, FOG_GET_SSE_CONST_PI(c));
}

static FOG_INLINE void ssse3_unpack_argb_1x1W_from_1x1B(__m128i& dst, const __m128i& src)
{
  dst = _mm_shuffle_epi8(src, FOG_GET_SSE_CONST_PI(SSSE3_unpack_argb_1x1W_from_1x1B));
}

static FOG_INLINE void ssse3_unpack_argb_2x1W_from_4x1B_lo(__m128i& dst, const __m128i& src)
{
  dst = _mm_shuffle_epi8(src, FOG_GET_SSE_CONST_PI(SSSE3_unpack_argb_2x1W_from_4x1B_lo));
}

static FOG_INLINE void ssse3_unpack_argb_2x1W_from_4x1B_hi(__m128i& dst, const __m128i& src)
{
  dst = _mm_shuffle_epi8(src, FOG_GET_SSE_CONST_PI(SSSE3_unpack_argb_2x1W_from_4x1B_hi));
}

static FOG_INLINE void ssse3_unpack_0rgb_1x1W_from_1x1B(__m128i& dst, const __m128i& src)
{
  dst = _mm_shuffle_epi8(src, FOG_GET_SSE_CONST_PI(SSSE3_unpack_0rgb_1x1W_from_1x1B));
}

static FOG_INLINE void ssse3_unpack_aaaa_1x1W_from_1x1B(__m128i& dst, const __m128i& src)
{
  dst = _mm_shuffle_epi8(src, FOG_GET_SSE_CONST_PI(SSSE3_unpack_aaaa_1x1W_from_1x1B));
}

static FOG_INLINE void ssse3_unpack_aaaa_2x1W_from_4x1B_lo(__m128i& dst, const __m128i& src)
{
  dst = _mm_shuffle_epi8(src, FOG_GET_SSE_CONST_PI(SSSE3_unpack_aaaa_2x1W_from_4x1B_lo));
}

static FOG_INLINE void ssse3_unpack_aaaa_2x1W_from_4x1B_hi(__m128i& dst, const __m128i& src)
{
  dst = _mm_shuffle_epi8(src, FOG_GET_SSE_CONST_PI(SSSE3_unpack_aaaa_2x1W_from_4x1B_hi));
}

static FOG_INLINE void sse2_unpack_4x2W(
  __m128i& dst0,
  __m128i& dst1, __m128i data0_1,
  __m128i& dst2,
  __m128i& dst3, __m128i data2_3)
{
  __m128i xmmz = _mm_setzero_si128();

  dst1 = _mm_unpackhi_epi8(data0_1, xmmz);
  dst0 = _mm_unpacklo_epi8(data0_1, xmmz);

  dst3 = _mm_unpackhi_epi8(data2_3, xmmz);
  dst2 = _mm_unpacklo_epi8(data2_3, xmmz);
}

static FOG_INLINE void sse2_muldiv255_4x2W(
  __m128i& dst0, const __m128i& data0, const __m128i& alpha0,
  __m128i& dst1, const __m128i& data1, const __m128i& alpha1,
  __m128i& dst2, const __m128i& data2, const __m128i& alpha2,
  __m128i& dst3, const __m128i& data3, const __m128i& alpha3)
{
  dst0 = _mm_mullo_epi16(data0, alpha0);
  dst1 = _mm_mullo_epi16(data1, alpha1);
  dst2 = _mm_mullo_epi16(data2, alpha2);
  dst3 = _mm_mullo_epi16(data3, alpha3);

  dst0 = _mm_adds_epu16(dst0, FOG_GET_SSE_CONST_PI(0080008000800080_0080008000800080));
  dst1 = _mm_adds_epu16(dst1, FOG_GET_SSE_CONST_PI(0080008000800080_0080008000800080));
  dst2 = _mm_adds_epu16(dst2, FOG_GET_SSE_CONST_PI(0080008000800080_0080008000800080));
  dst3 = _mm_adds_epu16(dst3, FOG_GET_SSE_CONST_PI(0080008000800080_0080008000800080));

  dst0 = _mm_mulhi_epu16(dst0, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
  dst1 = _mm_mulhi_epu16(dst1, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
  dst2 = _mm_mulhi_epu16(dst2, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
  dst3 = _mm_mulhi_epu16(dst3, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
}

//! @internal
struct FOG_HIDDEN CompositeSrcOverSSSE3
{
  // -------------------------------------------------------------------------
  // [CompositeSrcOverSSSE3 - PRGB32 - VBlit - PRGB32]
  // -------------------------------------------------------------------------

  // OPERATOR_SRC_OVER implementation for newest architectures with SSSE3
  // support. Idea is very simple. These architectures are really powerful,
  // so I all jumps were eliminated and inner loop was unrolled a lot.
  static void FOG_FASTCALL prgb32_vblit_prgb32_full(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSERT(w);

    sysint_t i = w;

    while ((sysuint_t)dst & 15)
    {
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_unpack_1x1W(msk0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(msk0xmm, msk0xmm);
      sse2_negate_1x1W(msk0xmm, msk0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_adds_1x1B(src0xmm, src0xmm, dst0xmm);

      sse2_store4(dst, src0xmm);
      if (--i == 0) return;

      src += 4;
      dst += 4;
    }

#if defined(FOG_ARCH_X86_64)
    goto start;

skip:
    src += 64;
    dst += 64;

start:
    while ((i -= 16) >= 0)
    {
      __m128i src0xmm;
      __m128i src2xmm;
      __m128i src4xmm, src6xmm;

      __m128i inv0xmm, inv1xmm;
      __m128i inv2xmm, inv3xmm;

      __m128i dst0xmm, dst1xmm;
      __m128i dst2xmm, dst3xmm;
      __m128i dst4xmm, dst6xmm;

      // Prefetch next cache line, here this increases the performance about
      // 0.0001% :) Not really much, but it's the reason why prefetches are
      // generally not used in Fog-Framework.
      sse2_prefetch_t0(src + 64);

      ssse3_load16u(src0xmm, src     );
      ssse3_load16u(src2xmm, src + 16);
      ssse3_load16u(src4xmm, src + 32);
      ssse3_load16u(src6xmm, src + 48);

      inv0xmm = _mm_packs_epi16(src0xmm, src2xmm);
      inv2xmm = _mm_packs_epi16(src4xmm, src6xmm);

      inv0xmm = _mm_packs_epi16(inv0xmm, inv2xmm);
      inv0xmm = _mm_cmpeq_epi8(inv0xmm, _mm_setzero_si128());

      int k = _mm_movemask_epi8(inv0xmm);
      if (k == 0xFFFF) goto skip;

      sse2_load16a(dst0xmm, dst     );
      sse2_load16a(dst2xmm, dst + 16);
      sse2_negate_alpha_1x4B(inv0xmm, src0xmm);
      sse2_negate_alpha_1x4B(inv2xmm, src2xmm);

      sse2_load16a(dst4xmm, dst + 32);
      sse2_load16a(dst6xmm, dst + 48);

      sse2_unpack_4x2W(
        dst0xmm, dst1xmm, dst0xmm,
        dst2xmm, dst3xmm, dst2xmm);

      ssse3_unpack_aaaa_2x1W_from_4x1B_hi(inv1xmm, inv0xmm);
      ssse3_unpack_aaaa_2x1W_from_4x1B_lo(inv0xmm, inv0xmm);
      ssse3_unpack_aaaa_2x1W_from_4x1B_hi(inv3xmm, inv2xmm);
      ssse3_unpack_aaaa_2x1W_from_4x1B_lo(inv2xmm, inv2xmm);

      sse2_muldiv255_4x2W(
        dst0xmm, dst0xmm, inv0xmm, dst1xmm, dst1xmm, inv1xmm,
        dst2xmm, dst2xmm, inv2xmm, dst3xmm, dst3xmm, inv3xmm);

      sse2_negate_alpha_1x4B(inv0xmm, src4xmm);
      sse2_negate_alpha_1x4B(inv2xmm, src6xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_pack_2x2W(dst2xmm, dst2xmm, dst3xmm);

      ssse3_unpack_aaaa_2x1W_from_4x1B_hi(inv1xmm, inv0xmm);
      ssse3_unpack_aaaa_2x1W_from_4x1B_lo(inv0xmm, inv0xmm);
      ssse3_unpack_aaaa_2x1W_from_4x1B_hi(inv3xmm, inv2xmm);
      ssse3_unpack_aaaa_2x1W_from_4x1B_lo(inv2xmm, inv2xmm);

      sse2_unpack_4x2W(
        dst4xmm, dst1xmm, dst4xmm,
        dst6xmm, dst3xmm, dst6xmm);

      sse2_adds_1x4B(src0xmm, src0xmm, dst0xmm);
      sse2_adds_1x4B(src2xmm, src2xmm, dst2xmm);

      sse2_muldiv255_4x2W(
        dst4xmm, dst4xmm, inv0xmm, dst1xmm, dst1xmm, inv1xmm,
        dst6xmm, dst6xmm, inv2xmm, dst3xmm, dst3xmm, inv3xmm);

      sse2_pack_2x2W(dst4xmm, dst4xmm, dst1xmm);
      sse2_pack_2x2W(dst6xmm, dst6xmm, dst3xmm);

      sse2_adds_1x4B(src4xmm, src4xmm, dst4xmm);
      sse2_adds_1x4B(src6xmm, src6xmm, dst6xmm);

      sse2_store16a(dst     , src0xmm);
      sse2_store16a(dst + 16, src2xmm);
      sse2_store16a(dst + 32, src4xmm);
      sse2_store16a(dst + 48, src6xmm);

      src += 64;
      dst += 64;
    }
    i += 16;
#endif // FOG_ARCH_X86_64

    while ((i -= 4) >= 0)
    {
      __m128i src0xmm;
      __m128i inv0xmm, inv1xmm;
      __m128i dst0xmm, dst1xmm;

      ssse3_load16u(src0xmm, src);
      sse2_load16a(dst0xmm, dst);

      sse2_negate_alpha_1x4B(inv0xmm, src0xmm);
      sse2_unpack_2x2W(dst0xmm, dst1xmm, dst0xmm);

      ssse3_unpack_aaaa_2x1W_from_4x1B_hi(inv1xmm, inv0xmm);
      ssse3_unpack_aaaa_2x1W_from_4x1B_lo(inv0xmm, inv0xmm);
      sse2_muldiv255_2x2W(dst0xmm, dst0xmm, inv0xmm, dst1xmm, dst1xmm, inv1xmm);

      sse2_pack_2x2W(dst0xmm, dst0xmm, dst1xmm);
      sse2_adds_1x4B(src0xmm, src0xmm, dst0xmm);

      sse2_store16a(dst, src0xmm);

      src += 16;
      dst += 16;
    }
    i += 4;

    while (i)
    {
      __m128i src0xmm;
      __m128i dst0xmm;
      __m128i msk0xmm;

      sse2_load4(src0xmm, src);
      sse2_load4(dst0xmm, dst);

      sse2_unpack_1x1W(msk0xmm, src0xmm);
      sse2_unpack_1x1W(dst0xmm, dst0xmm);

      sse2_expand_alpha_1x1W(msk0xmm, msk0xmm);
      sse2_negate_1x1W(msk0xmm, msk0xmm);
      sse2_muldiv255_1x1W(dst0xmm, dst0xmm, msk0xmm);

      sse2_pack_1x1W(dst0xmm, dst0xmm);
      sse2_adds_1x1B(src0xmm, src0xmm, dst0xmm);

      sse2_store4(dst, src0xmm);

      src += 4;
      dst += 4;

      i--;
    }
  }
};

} // RasterEngine namespace
} // Fog namespace
