// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_SSE2_p.h>
#include <Fog/Graphics/RasterEngine/Dib_SSE2_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::SSE2 - Pattern - Core]
// ============================================================================

// Tricks used in formulas:
// 
//   uint16_t a = ...;
//   (a ^ (uint16_t)0xFFFFU) + (uint16_t)257U == (uint16_t)256U - a;

// TODO
#define SSE2_PATTERN_INTERPOLATE_32_2(DST, SRC0, WEIGHT0, SRC1, WEIGHT1) \
{ \
  ByteSIMD::b64_1x4 pixT0; \
  ByteSIMD::b64_1x4 pixT1; \
  \
  ByteSIMD::b64_1x4Unpack0213(pixT0, (SRC0)); \
  ByteSIMD::b64_1x4Unpack0213(pixT1, (SRC1)); \
  \
  pixT0 *= (uint)(WEIGHT0); \
  pixT1 *= (uint)(WEIGHT1); \
  pixT0 += pixT1; \
  \
  pixT0 &= FOG_UINT64_C(0xFF00FF00FF00FF00); \
  \
  DST = (uint32_t)((pixT0 >> 8) | (pixT0 >> 32)); \
}

// TODO
#define SSE2_PATTERN_INTERPOLATE_32_4(DST, SRC0, WEIGHT0, SRC1, WEIGHT1, SRC2, WEIGHT2, SRC3, WEIGHT3) \
{ \
  __m128i pixT0; \
  __m128i pixT0; \
  ByteSIMD::b64_1x4 pixT0; \
  ByteSIMD::b64_1x4 pixT1; \
  ByteSIMD::b64_1x4 pixT2; \
  \
  ByteSIMD::b64_1x4Unpack0213(pixT0, (SRC0)); \
  ByteSIMD::b64_1x4Unpack0213(pixT1, (SRC1)); \
  \
  pixT0 *= (uint)(WEIGHT0); \
  pixT1 *= (uint)(WEIGHT1); \
  pixT0 += pixT1; \
  \
  ByteSIMD::b64_1x4Unpack0213(pixT1, (SRC2)); \
  ByteSIMD::b64_1x4Unpack0213(pixT2, (SRC3)); \
  \
  pixT1 *= (uint)(WEIGHT2); \
  pixT2 *= (uint)(WEIGHT3); \
  pixT0 += pixT1; \
  pixT0 += pixT2; \
  \
  pixT0 &= FOG_UINT64_C(0xFF00FF00FF00FF00); \
  \
  DST = (uint32_t)((pixT0 >> 8) | (pixT0 >> 32)); \
}

#define GRADIENT_SSE2_INTERPOLATE_2(DST, COLORS, YY) \
{ \
  __m128i __x0xmm; \
  __m128i __w0xmm; \
  \
  sse2_load8(__x0xmm, &COLORS[(int)(YY >> 16)]); \
  \
  __w0xmm = _mm_cvtsi32_si128((int)YY); \
  __w0xmm = _mm_srli_epi16(__w0xmm, 8); \
  __w0xmm = _mm_shufflelo_epi16(__w0xmm, _MM_SHUFFLE(0, 0, 0, 0)); \
  __w0xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(0, 0, 0, 0)); \
  __w0xmm = _mm_xor_si128(__w0xmm, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF)); \
  __w0xmm = _mm_add_epi16(__w0xmm, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101)); \
  \
  sse2_unpack_1x2W(__x0xmm, __x0xmm); \
  \
  __x0xmm = _mm_mullo_epi16(__x0xmm, __w0xmm); \
  __w0xmm = _mm_shuffle_epi32(__x0xmm, _MM_SHUFFLE(1, 0, 3, 2)); \
  \
  __x0xmm = _mm_adds_epu16(__x0xmm, __w0xmm); \
  __x0xmm = _mm_srli_epi16(__x0xmm, 8); \
  \
  sse2_pack_1x1W(__x0xmm, __x0xmm); \
  sse2_store4(DST, __x0xmm); \
}

#define GRADIENT_SSE2_INTERPOLATE_2_4X_INIT() \
  __m128i __yypos; \
  __m128i __axinc; \
  \
  __yypos = _mm_cvtsi32_si128((int)yy); \
  __axinc = _mm_cvtsi32_si128((int)ax); \
  \
  __yypos = _mm_shufflelo_epi16(__yypos, _MM_SHUFFLE(0, 0, 0, 0)); \
  __axinc = _mm_shufflelo_epi16(__axinc, _MM_SHUFFLE(0, 0, 0, 0)); \
  \
  __yypos = _mm_shuffle_epi32(__yypos, _MM_SHUFFLE(1, 0, 1, 0)); \
  __axinc = _mm_shuffle_epi32(__axinc, _MM_SHUFFLE(1, 0, 1, 0)); \
  \
  __axinc = _mm_slli_si128(__axinc, 4); \
  __yypos = _mm_add_epi16(__yypos, __axinc); \
  \
  __axinc = _mm_slli_si128(__axinc, 4); \
  __yypos = _mm_add_epi16(__yypos, __axinc); \
  \
  __axinc = _mm_slli_si128(__axinc, 4); \
  __yypos = _mm_add_epi16(__yypos, __axinc); \
  \
  __axinc = _mm_shuffle_epi32(__axinc, _MM_SHUFFLE(3, 3, 3, 3)); \
  __axinc = _mm_slli_epi16(__axinc, 2);

#define GRADIENT_SSE2_INTERPOLATE_2_4X_STEP(DST, COND) \
{ \
  __m128i __x0xmm, __w0xmm; \
  __m128i __x1xmm, __w1xmm; \
  __m128i __x2xmm; \
  \
  int pos; \
  \
  /* LOAD PIXEL [0], [1], [3], AND SOME 'WEIGHT' STUFF FOR [0] AND [1] PIXELS */ \
  __w0xmm = _mm_shuffle_epi32(__yypos, _MM_SHUFFLE(1, 0, 1, 0)); \
  \
  pos = (int)(yy >> 16); yy += ax; \
  COND \
  sse2_load8(__x0xmm, &colors[pos]); \
  \
  __w0xmm = _mm_srli_epi16(__w0xmm, 8); \
  \
  pos = (int)(yy >> 16); yy += ax; \
  COND \
  sse2_load8(__x1xmm, &colors[pos]); \
  \
  __w0xmm = _mm_xor_si128(__w0xmm, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF)); \
  __w0xmm = _mm_add_epi16(__w0xmm, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101)); \
  \
  pos = (int)(yy >> 16); yy += ax; \
  COND \
  sse2_load8(__x2xmm, &colors[pos]); \
  \
  /* INTERPOLATE [0] AND [1], AND SOME WEIGHT STUFF FOR [2] AND [3] PIXELS */ \
  __w1xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(3, 3, 1, 1)); \
  __w0xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(2, 2, 0, 0)); \
  \
  sse2_unpack_1x2W(__x0xmm, __x0xmm); \
  sse2_unpack_1x2W(__x1xmm, __x1xmm); \
  \
  __x0xmm = _mm_mullo_epi16(__x0xmm, __w0xmm); \
  __x1xmm = _mm_mullo_epi16(__x1xmm, __w1xmm); \
  \
  __w0xmm = _mm_shuffle_epi32(__x0xmm, _MM_SHUFFLE(1, 0, 3, 2)); \
  __w1xmm = _mm_shuffle_epi32(__x1xmm, _MM_SHUFFLE(1, 0, 3, 2)); \
  \
  __x0xmm = _mm_adds_epu16(__x0xmm, __w0xmm); \
  __w0xmm = _mm_shuffle_epi32(__yypos, _MM_SHUFFLE(3, 2, 3, 2)); \
  __x1xmm = _mm_adds_epu16(__x1xmm, __w1xmm); \
  __w0xmm = _mm_srli_epi16(__w0xmm, 8); \
  \
  __x0xmm = _mm_srli_epi16(__x0xmm, 8); \
  __w0xmm = _mm_xor_si128(__w0xmm, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF)); \
  __x1xmm = _mm_srli_epi16(__x1xmm, 8); \
  __w0xmm = _mm_add_epi16(__w0xmm, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101)); \
  \
  __x0xmm = _mm_packus_epi16(__x0xmm, __x1xmm); \
  \
  /* LOAD PIXEL [3] */ \
  pos = (int)(yy >> 16); yy += ax; \
  COND \
  sse2_load8(__x1xmm, &colors[pos]); \
  \
  /* INTERPOLATE [2] AND [3] */ \
  __w1xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(3, 3, 1, 1)); \
  __w0xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(2, 2, 0, 0)); \
  \
  sse2_unpack_1x2W(__x2xmm, __x2xmm); \
  sse2_unpack_1x2W(__x1xmm, __x1xmm); \
  \
  __x2xmm = _mm_mullo_epi16(__x2xmm, __w0xmm); \
  __x1xmm = _mm_mullo_epi16(__x1xmm, __w1xmm); \
  \
  __w0xmm = _mm_shuffle_epi32(__x2xmm, _MM_SHUFFLE(1, 0, 3, 2)); \
  __w1xmm = _mm_shuffle_epi32(__x1xmm, _MM_SHUFFLE(1, 0, 3, 2)); \
  \
  __x2xmm = _mm_adds_epu16(__x2xmm, __w0xmm); \
  __x1xmm = _mm_adds_epu16(__x1xmm, __w1xmm); \
  \
  __x2xmm = _mm_srli_epi16(__x2xmm, 8); \
  __x1xmm = _mm_srli_epi16(__x1xmm, 8); \
  \
  __x2xmm = _mm_packus_epi16(__x2xmm, __x1xmm); \
  \
  __x0xmm = _mm_shuffle_epi32(__x0xmm, _MM_SHUFFLE(2, 0, 2, 0)); \
  __x2xmm = _mm_shuffle_epi32(__x2xmm, _MM_SHUFFLE(2, 0, 2, 0)); \
  \
  /* STORE */ \
  sse2_store8(reinterpret_cast<char*>(DST)    , __x0xmm); \
  sse2_store8(reinterpret_cast<char*>(DST) + 8, __x2xmm); \
  \
  /* ADVANCE */ \
  __yypos = _mm_add_epi16(__yypos, __axinc); \
}

// ============================================================================
// [Fog::RasterEngine::SSE2 - Pattern - PreProcess]
// ============================================================================

// These structures are used in template parameters to pre-process pixels.
// Reason is to keep some code small and to specialize it for common pixel 
// formats.

//! @internal
struct FOG_HIDDEN PP_PRGB32_SSE2
{
  enum { BPP = 4 };
  enum { USE_PREPROCESS = 0 };
  enum { USE_ALPHA = 1 };

  FOG_INLINE PP_PRGB32_SSE2(const RasterPattern* ctx) {}

  FOG_INLINE void preprocess_1x1B(__m128i& pix0) {}
  FOG_INLINE void preprocess_1x2B(__m128i& pix0) {}
  FOG_INLINE void preprocess_1x4B(__m128i& pix0) {}

  FOG_INLINE void preprocess_1x1W(__m128i& pix0) {}
  FOG_INLINE void preprocess_1x2W(__m128i& pix0) {}
  FOG_INLINE void preprocess_2x2W(__m128i& pix0, __m128i& pix1) {}
};

//! @internal
struct FOG_HIDDEN PP_ARGB32_SSE2
{
  enum { BPP = 4 };
  enum { USE_PREPROCESS = 1 };
  enum { USE_ALPHA = 1 };

  FOG_INLINE PP_ARGB32_SSE2(const RasterPattern* ctx) {}

  FOG_INLINE void preprocess_1x1B(__m128i& pix0)
  {
    sse2_unpack_1x1W(pix0, pix0);
    sse2_premultiply_1x1W(pix0, pix0);
    sse2_pack_1x1W(pix0, pix0);
  }

  FOG_INLINE void preprocess_1x2B(__m128i& pix0)
  {
    sse2_unpack_1x2W(pix0, pix0);
    sse2_premultiply_1x2W(pix0, pix0);
    sse2_pack_1x1W(pix0, pix0);
  }

  FOG_INLINE void preprocess_1x4B(__m128i& pix0)
  {
    __m128i pix1;

    sse2_unpack_2x2W(pix0, pix1, pix0);
    sse2_premultiply_2x2W(pix0, pix0, pix1, pix1);
    sse2_pack_2x2W(pix0, pix0, pix1);
  }

  FOG_INLINE void preprocess_1x1W(__m128i& pix0)
  {
    sse2_premultiply_1x1W(pix0, pix0);
  }

  FOG_INLINE void preprocess_1x2W(__m128i& pix0)
  {
    sse2_premultiply_1x2W(pix0, pix0);
  }

  FOG_INLINE void preprocess_2x2W(__m128i& pix0, __m128i& pix1)
  {
    sse2_premultiply_2x2W(pix0, pix0, pix1, pix1);
  }
};

//! @internal
struct FOG_HIDDEN PP_XRGB32_SSE2
{
  enum { BPP = 4 };
  enum { USE_PREPROCESS = 1 };
  enum { USE_ALPHA = 0 };

  FOG_INLINE PP_XRGB32_SSE2(const RasterPattern* ctx) {}

  FOG_INLINE void preprocess_1x1B(__m128i& pix0)
  {
    pix0 = _mm_or_si128(pix0, FOG_GET_SSE_CONST_PI(0000000000000000_00000000FF000000));
  }

  FOG_INLINE void preprocess_1x2B(__m128i& pix0)
  {
    pix0 = _mm_or_si128(pix0, FOG_GET_SSE_CONST_PI(0000000000000000_FF000000FF000000));
  }

  FOG_INLINE void preprocess_1x4B(__m128i& pix0)
  {
    pix0 = _mm_or_si128(pix0, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
  }

  FOG_INLINE void preprocess_1x1W(__m128i& pix0)
  {
    pix0 = _mm_or_si128(pix0, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
  }

  FOG_INLINE void preprocess_1x2W(__m128i& pix0)
  {
    pix0 = _mm_or_si128(pix0, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
  }

  FOG_INLINE void preprocess_2x2W(__m128i& pix0, __m128i& pix1)
  {
    pix0 = _mm_or_si128(pix0, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
    pix1 = _mm_or_si128(pix1, FOG_GET_SSE_CONST_PI(00FF000000000000_00FF000000000000));
  }
};

} // RasterEngine namespace
} // Fog namespace
