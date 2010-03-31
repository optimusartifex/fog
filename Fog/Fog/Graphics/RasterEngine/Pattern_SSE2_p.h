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
// [Fog::RasterEngine::SSE2 - Helpers]
// ============================================================================

// Tricks used in formulas:
// 
//   uint16_t a = ...;
//   (a ^ (uint16_t)0xFFFFU) + (uint16_t)257U == (uint16_t)256U - a;

// TODO
#define PATTERN_SSE2_INTERPOLATE_32_2(DST, SRC0, WEIGHT0, SRC1, WEIGHT1) \
{ \
  ByteUtil::byte1x4 pixT0; \
  ByteUtil::byte1x4 pixT1; \
  \
  ByteUtil::byte1x4_unpack_0213(pixT0, (SRC0)); \
  ByteUtil::byte1x4_unpack_0213(pixT1, (SRC1)); \
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
#define PATTERN_SSE2_INTERPOLATE_32_4(DST, SRC0, WEIGHT0, SRC1, WEIGHT1, SRC2, WEIGHT2, SRC3, WEIGHT3) \
{ \
  __m128i pixT0; \
  __m128i pixT0; \
  ByteUtil::byte1x4 pixT0; \
  ByteUtil::byte1x4 pixT1; \
  ByteUtil::byte1x4 pixT2; \
  \
  ByteUtil::byte1x4_unpack_0213(pixT0, (SRC0)); \
  ByteUtil::byte1x4_unpack_0213(pixT1, (SRC1)); \
  \
  pixT0 *= (uint)(WEIGHT0); \
  pixT1 *= (uint)(WEIGHT1); \
  pixT0 += pixT1; \
  \
  ByteUtil::byte1x4_unpack_0213(pixT1, (SRC2)); \
  ByteUtil::byte1x4_unpack_0213(pixT2, (SRC3)); \
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
  __w0xmm = _mm_xor_si128(__w0xmm, SSE2_GET_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF)); \
  __w0xmm = _mm_add_epi16(__w0xmm, SSE2_GET_CONST_PI(0000000000000000_0101010101010101)); \
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
  __w0xmm = _mm_xor_si128(__w0xmm, SSE2_GET_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF)); \
  __w0xmm = _mm_add_epi16(__w0xmm, SSE2_GET_CONST_PI(0000000000000000_0101010101010101)); \
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
  __w0xmm = _mm_xor_si128(__w0xmm, SSE2_GET_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF)); \
  __x1xmm = _mm_srli_epi16(__x1xmm, 8); \
  __w0xmm = _mm_add_epi16(__w0xmm, SSE2_GET_CONST_PI(0000000000000000_0101010101010101)); \
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
// [Fog::RasterEngine::SSE2 - PreProcess]
// ============================================================================

// These structures are used in template parameters to pre-process pixels.
// Reason is to keep some code small and to specialize it for common pixel 
// formats.

struct FOG_HIDDEN PP_PRGB32_SSE2
{
  enum { BPP = 4 };

  FOG_INLINE PP_PRGB32_SSE2(const PatternContext* ctx) {}

  FOG_INLINE void preprocess_1x1B(__m128i& pix0) {}
  FOG_INLINE void preprocess_1x1W(__m128i& pix0) {}
  FOG_INLINE void preprocess_1x2W(__m128i& pix0) {}
  FOG_INLINE void preprocess_2x2W(__m128i& pix0, __m128i& pix1) {}
};

struct FOG_HIDDEN PP_ARGB32_SSE2
{
  enum { BPP = 4 };

  FOG_INLINE PP_ARGB32_SSE2(const PatternContext* ctx) {}

  FOG_INLINE void preprocess_1x1B(__m128i& pix0)
  {
    sse2_unpack_1x1W(pix0, pix0);
    sse2_premultiply_1x1W(pix0, pix0);
    sse2_pack_1x1W(pix0, pix0);
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

struct FOG_HIDDEN PP_XRGB32_SSE2
{
  enum { BPP = 4 };

  FOG_INLINE PP_XRGB32_SSE2(const PatternContext* ctx) {}

  FOG_INLINE void preprocess_1x1W(__m128i& pix0)
  {
    pix0 = _mm_or_si128(pix0, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  }

  FOG_INLINE void preprocess_1x2W(__m128i& pix0)
  {
    pix0 = _mm_or_si128(pix0, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  }

  FOG_INLINE void preprocess_2x2W(__m128i& pix0, __m128i& pix1)
  {
    pix0 = _mm_or_si128(pix0, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
    pix1 = _mm_or_si128(pix1, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  }
};

// ============================================================================
// [Fog::RasterEngine::SSE2 - Pattern]
// ============================================================================

struct FOG_HIDDEN PatternSSE2
{
  // --------------------------------------------------------------------------
  // [Texture - Exact]
  // --------------------------------------------------------------------------

  static uint8_t* FOG_FASTCALL texture_fetch_exact_repeat_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if ((x >= tw) | (x <= -tw)) x %= tw;
    if ((y >= th) | (y <= -th)) y %= th;

    if (x < 0) x += tw;
    if (y < 0) y += th;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    int i;

    srcCur = srcBase + (uint)x * 4;

    // Return image buffer if span fits to it (this is very efficient
    // optimization for short spans or large textures).
    i = Math::min(tw - x, w);
    if (w < tw - x)
      return const_cast<uint8_t*>(srcCur);

    // This is equal to C implementation in RasterEngine_Pattern_C_p.h.
    for (;;)
    {
      w -= i;

      if (i >= 32)
      {
        // We can improve texture fetching by more pixels at a time.
        while ((sysuint_t)dstCur & 15)
        {
          ((uint32_t*)dstCur)[0] = ((const uint32_t*)srcCur)[0];
          dstCur += 4;
          srcCur += 4;
          if (--i == 0) goto end;
        }

        while (i >= 8)
        {
          __m128i src0mm;
          __m128i src1mm;
          sse2_load16u(src0mm, srcCur +  0);
          sse2_load16u(src1mm, srcCur + 16);
          sse2_store16a(dstCur +  0, src0mm);
          sse2_store16a(dstCur + 16, src1mm);

          dstCur += 32;
          srcCur += 32;
          i -= 8;
        }
        if (i == 0) goto end;
      }

      do {
        ((uint32_t*)dstCur)[0] = ((const uint32_t*)srcCur)[0];
        dstCur += 4;
        srcCur += 4;
      } while (--i);
  end:
      if (!w) break;

      i = Math::min(w, tw);
      srcCur = srcBase;
    }

    return dst;
  }

  static uint8_t* FOG_FASTCALL texture_fetch_exact_reflect_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if ((x >= tw2) | (x <= -tw2)) x %= tw2;
    if ((y >= th2) | (y <= -th2)) y %= th2;

    if (x < 0) x += tw2;
    if (y < 0) y += th2;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th) y = th2 - y - 1;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    if (x >= 0 && x <= tw && w < tw - x)
      return const_cast<uint8_t*>(srcBase + (uint)x * 4);

    do {
      // Reflect mode
      if (x >= tw)
      {
        int i = Math::min(tw2 - x, w);

        srcCur = srcBase + (uint)(tw2 - x - 1) * 4;

        w -= i;
        x = 0;

        if (i >= 32)
        {
          // We can improve texture fetching by more pixels at a time.
          while ((sysuint_t)dstCur & 15)
          {
            ((uint32_t*)dstCur)[0] = ((const uint32_t*)srcCur)[0];
            dstCur += 4;
            srcCur -= 4;
            if (--i == 0) goto end;
          }

          while (i >= 8)
          {
            __m128i src0mm;
            __m128i src1mm;

            srcCur -= 32;

            sse2_load16u(src0mm, srcCur + 20);
            sse2_load16u(src1mm, srcCur + 4);
            src0mm = _mm_shuffle_epi32(src0mm, _MM_SHUFFLE(0, 1, 2, 3));
            src1mm = _mm_shuffle_epi32(src1mm, _MM_SHUFFLE(0, 1, 2, 3));
            sse2_store16a(dstCur +  0, src0mm);
            sse2_store16a(dstCur + 16, src1mm);

            dstCur += 32;
            i -= 8;
          }
          if (i == 0) goto end;
        }

        do {
          ((uint32_t*)dstCur)[0] = ((const uint32_t*)srcCur)[0];
          dstCur += 4;
          srcCur -= 4;
        } while (--i);
      }
      // Repeat mode
      else
      {
        int i = Math::min(tw - x, w);

        srcCur = srcBase + (uint)x * 4;

        w -= i;
        x += i;

        if (i >= 32)
        {
          // We can improve texture fetching by more pixels at a time.
          while ((sysuint_t)dstCur & 15)
          {
            ((uint32_t*)dstCur)[0] = ((const uint32_t*)srcCur)[0];
            dstCur += 4;
            srcCur += 4;
            if (--i == 0) goto end;
          }

          while (i >= 8)
          {
            __m128i src0mm;
            __m128i src1mm;
            sse2_load16u(src0mm, srcCur +  0);
            sse2_load16u(src1mm, srcCur + 16);
            sse2_store16a(dstCur +  0, src0mm);
            sse2_store16a(dstCur + 16, src1mm);

            dstCur += 32;
            srcCur += 32;
            i -= 8;
          }
          if (i == 0) goto end;
        }

        do {
          ((uint32_t*)dstCur)[0] = ((const uint32_t*)srcCur)[0];
          dstCur += 4;
          srcCur += 4;
        } while (--i);
      }
  end:
      ;
    } while (w);

    return dst;
  }

  // --------------------------------------------------------------------------
  // [Texture - Transform - Bilinear - Helpers]
  // --------------------------------------------------------------------------

  static FOG_INLINE void _vector_affine_sse2(__m128d& m_x_y, const double* m)
  {
    __m128d m_y_x = _mm_shuffle_pd(m_x_y, m_x_y, _MM_SHUFFLE2(0, 1));

    __m128d m_sx_sy = _mm_set_pd(m[MATRIX_SY], m[MATRIX_SX]);
    __m128d m_shx_shy = _mm_set_pd(m[MATRIX_SHY], m[MATRIX_SHX]);
    __m128d m_tx_ty = _mm_loadu_pd(&m[MATRIX_TX]);

    m_x_y = _mm_mul_pd(m_x_y, m_sx_sy);
    m_y_x = _mm_mul_pd(m_y_x, m_shx_shy);
    m_x_y = _mm_add_pd(m_x_y, m_tx_ty);
    m_x_y = _mm_add_pd(m_x_y, m_y_x);
  }

  static FOG_INLINE void _vector_fetch_centered_sse2(__m128d& xy, int x, int y)
  {
    xy = _mm_add_pd(_mm_cvtepi32_pd(_mm_set_epi32(0, 0, y, x)), SSE2_GET_CONST_PD(0_5_0_5));
  }

  static FOG_INLINE void _vector_fetch_transformed_sse2(__m128d& fxfy, int x, int y, const double* m)
  {
    _vector_fetch_centered_sse2(fxfy, x, y);
    _vector_affine_sse2(fxfy, m);
    fxfy = _mm_mul_pd(fxfy, SSE2_GET_CONST_PD(65536_0_65536_0));
    fxfy = _mm_sub_pd(fxfy, SSE2_GET_CONST_PD(32768_0_32768_0));
  }

  static FOG_INLINE void _vector_fetch_transformed_sse2(int& fx, int& fy, int x, int y, const double* m)
  {
    __m128d fxfy_d;
    _vector_fetch_transformed_sse2(fxfy_d, x, y, m);
    __m128i fxfy = _mm_cvtpd_epi32(fxfy_d);

    fx = _mm_cvtsi128_si32(fxfy); fxfy = _mm_shuffle_epi32(fxfy, _MM_SHUFFLE(0, 1, 0, 1));
    fy = _mm_cvtsi128_si32(fxfy);
  }

  // --------------------------------------------------------------------------
  // [Texture - Transform - Bilinear]
  // --------------------------------------------------------------------------

  template<typename PP>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_none_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PP pp(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx;
    int fy;
    _vector_fetch_transformed_sse2(fx, fy, x, y, ctx->m);

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    tw--;
    th--;

    int i = w;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      __m128i __x01_y0;
      __m128i __x01_y1;

      __m128i __wx0;
      __m128i __wy0;
      __m128i __ww0;

      const uint8_t* src0 = srcBits + py0 * srcStride + px0 * 4;

      if (FOG_UNLIKELY(((uint)py0 >= (uint)th) | ((uint)px0 >= (uint)tw)))
      {
        __x01_y0 = _mm_setzero_si128();
        __x01_y1 = _mm_setzero_si128();

        __wx0 = _mm_setzero_si128();
        __wy0 = _mm_setzero_si128();

        if ((uint)py0 <= (uint)th)
        {
          if ((uint)px0     <= (uint)tw) sse2_load4(__x01_y0, src0    );
          if ((uint)px0 + 1 <= (uint)tw) sse2_load4(__wx0   , src0 + 4);
        }

        py0++;
        src0 += srcStride;

        if ((uint)py0 <= (uint)th)
        {
          if ((uint)px0     <= (uint)tw) sse2_load4(__x01_y1, src0    );
          if ((uint)px0 + 1 <= (uint)tw) sse2_load4(__wy0   , src0 + 4);
        }

        __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(3, 2, 0, 1));
        __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(3, 2, 0, 1));

        __x01_y0 = _mm_or_si128(__x01_y0, __wx0);
        __x01_y1 = _mm_or_si128(__x01_y1, __wy0);
      }
      else
      {
        sse2_load8(__x01_y0, src0);
        sse2_load8(__x01_y1, src0 + srcStride);
      }

      __wx0 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(0, 1, 1, 0));
      __wy0 = _mm_shuffle_epi32(__x01_y1, _MM_SHUFFLE(1, 0, 1, 0));
      __wx0 = _mm_cmpeq_epi32(__wx0, __wy0);

      if (_mm_movemask_epi8(__wx0) == 0xFFFF)
      {
        pp.preprocess_1x1B(__x01_y0);
        goto done;
      }

      __wx0 = _mm_cvtsi32_si128(fx);
      __wy0 = _mm_cvtsi32_si128(fy);

      __wx0 = _mm_shufflelo_epi16(__wx0, _MM_SHUFFLE(0, 0, 0, 0));
      __wy0 = _mm_shufflelo_epi16(__wy0, _MM_SHUFFLE(0, 0, 0, 0));

      __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(1, 0, 1, 0));
      __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(1, 0, 1, 0));

      __wx0 = _mm_srli_epi16(__wx0, 8);
      sse2_unpack_1x1W(__x01_y0, __x01_y0);
      __wy0 = _mm_srli_epi16(__wy0, 8);
      __wx0 = _mm_xor_si128(__wx0, SSE2_GET_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
      sse2_unpack_1x1W(__x01_y1, __x01_y1);
      __wx0 = _mm_add_epi16(__wx0, SSE2_GET_CONST_PI(0000000000000000_0101010101010101));

      pp.preprocess_2x2W(__x01_y0, __x01_y1);

      __ww0 = _mm_mullo_epi16(__wx0, __wy0);
      __wy0 = _mm_xor_si128(__wy0, SSE2_GET_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
      __ww0 = _mm_srli_epi16(__ww0, 8);
      __wy0 = _mm_add_epi16(__wy0, SSE2_GET_CONST_PI(0101010101010101_0101010101010101));

      __wx0 = _mm_slli_epi16(__wx0, 4);
      __wy0 = _mm_slli_epi16(__wy0, 4);

      __x01_y1 = _mm_mullo_epi16(__x01_y1, __ww0);
      __wy0 = _mm_mulhi_epi16(__wy0, __wx0);
      __x01_y0 = _mm_mullo_epi16(__x01_y0, __wy0);

      __x01_y0 = _mm_add_epi16(__x01_y0, __x01_y1);
      __x01_y1 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(1, 0, 3, 2));
      __x01_y0 = _mm_add_epi16(__x01_y0, __x01_y1);

      __x01_y0 = _mm_srli_epi16(__x01_y0, 8);

      sse2_pack_1x1W(__x01_y0, __x01_y0);
done:
      sse2_store4(dstCur, __x01_y0);
      dstCur += 4;

      fx += dx;
      fy += dy;
    } while (--i);

    return dst;
  }

  template<typename PP>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_pad_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PP pp(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx;
    int fy;
    _vector_fetch_transformed_sse2(fx, fy, x, y, ctx->m);

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    tw--;
    th--;

    int i = w;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      __m128i __x01_y0;
      __m128i __x01_y1;

      __m128i __wx0;
      __m128i __wy0;
      __m128i __ww0;

      if (FOG_UNLIKELY(((uint)py0 >= (uint)th) | ((uint)px0 >= (uint)tw)))
      {
        int px1 = px0 + 1;
        int py1 = py0 + 1;

        if (px0 < 0) { px0 = px1 = 0; } else if (px0 >= tw) { px0 = px1 = tw; }
        if (py0 < 0) { py0 = py1 = 0; } else if (py0 >= th) { py0 = py1 = th; }

        const uint8_t* src0;

        src0 = srcBits + (uint)py0 * srcStride;
        sse2_load4(__x01_y0, src0    );
        sse2_load4(__wx0       , src0 + 4);

        src0 = srcBits + (uint)py1 * srcStride;
        sse2_load4(__x01_y1, src0    );
        sse2_load4(__wy0       , src0 + 4);

        __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(3, 2, 0, 1));
        __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(3, 2, 0, 1));

        __x01_y0 = _mm_or_si128(__x01_y0, __wx0);
        __x01_y1 = _mm_or_si128(__x01_y1, __wy0);
      }
      else
      {
        const uint8_t* src0 = srcBits + py0 * srcStride + px0 * 4;

        sse2_load8(__x01_y0, src0);
        sse2_load8(__x01_y1, src0 + srcStride);
      }

      __wx0 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(0, 1, 1, 0));
      __wy0 = _mm_shuffle_epi32(__x01_y1, _MM_SHUFFLE(1, 0, 1, 0));
      __wx0 = _mm_cmpeq_epi32(__wx0, __wy0);

      if (_mm_movemask_epi8(__wx0) == 0xFFFF)
      {
        pp.preprocess_1x1B(__x01_y0);
        goto done;
      }

      __wx0 = _mm_cvtsi32_si128(fx);
      __wy0 = _mm_cvtsi32_si128(fy);

      __wx0 = _mm_shufflelo_epi16(__wx0, _MM_SHUFFLE(0, 0, 0, 0));
      __wy0 = _mm_shufflelo_epi16(__wy0, _MM_SHUFFLE(0, 0, 0, 0));

      __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(1, 0, 1, 0));
      __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(1, 0, 1, 0));

      __wx0 = _mm_srli_epi16(__wx0, 8);
      sse2_unpack_1x1W(__x01_y0, __x01_y0);
      __wy0 = _mm_srli_epi16(__wy0, 8);
      __wx0 = _mm_xor_si128(__wx0, SSE2_GET_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
      sse2_unpack_1x1W(__x01_y1, __x01_y1);
      __wx0 = _mm_add_epi16(__wx0, SSE2_GET_CONST_PI(0000000000000000_0101010101010101));

      pp.preprocess_2x2W(__x01_y0, __x01_y1);

      __ww0 = _mm_mullo_epi16(__wx0, __wy0);
      __wy0 = _mm_xor_si128(__wy0, SSE2_GET_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
      __ww0 = _mm_srli_epi16(__ww0, 8);
      __wy0 = _mm_add_epi16(__wy0, SSE2_GET_CONST_PI(0101010101010101_0101010101010101));

      __wx0 = _mm_slli_epi16(__wx0, 4);
      __wy0 = _mm_slli_epi16(__wy0, 4);

      __x01_y1 = _mm_mullo_epi16(__x01_y1, __ww0);
      __wy0 = _mm_mulhi_epi16(__wy0, __wx0);
      __x01_y0 = _mm_mullo_epi16(__x01_y0, __wy0);

      __x01_y0 = _mm_add_epi16(__x01_y0, __x01_y1);
      __x01_y1 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(1, 0, 3, 2));
      __x01_y0 = _mm_add_epi16(__x01_y0, __x01_y1);

      __x01_y0 = _mm_srli_epi16(__x01_y0, 8);

      sse2_pack_1x1W(__x01_y0, __x01_y0);
done:
      sse2_store4(dstCur, __x01_y0);
      dstCur += 4;

      fx += dx;
      fy += dy;
    } while (--i);

    return dst;
  }

  template<typename PP>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_repeat_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PP pp(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx;
    int fy;
    _vector_fetch_transformed_sse2(fx, fy, x, y, ctx->m);

    int fxmax = ctx->texture.fxmax;
    int fymax = ctx->texture.fymax;

    int fxrewind = ctx->texture.fxrewind;
    int fyrewind = ctx->texture.fyrewind;

    if ((fx <= -fxmax) | (fx >= fxmax)) fx %= fxmax;
    if ((fy <= -fymax) | (fy >= fymax)) fy %= fymax;

    if (fx < 0) fx += fxmax;
    if (fy < 0) fy += fymax;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    tw--;
    th--;

    int i = w;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      __m128i __x01_y0;
      __m128i __x01_y1;

      __m128i __wx0;
      __m128i __wy0;
      __m128i __ww0;

      const uint8_t* src0 = srcBits + (uint)py0 * srcStride;

      if (FOG_UNLIKELY(((uint)py0 >= (uint)th) | ((uint)px0 >= (uint)tw)))
      {
        const uint8_t* src1 = src0 + srcStride;
        if (py0 >= th) src1 = srcBits;

        if (px0 >= tw)
        {
          sse2_load4(__x01_y0, src0 + (uint)px0 * 4);
          sse2_load4(__x01_y1, src1 + (uint)px0 * 4);

          sse2_load4(__wx0, src0);
          sse2_load4(__wy0, src1);

          __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(3, 2, 0, 1));
          __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(3, 2, 0, 1));

          __x01_y0 = _mm_or_si128(__x01_y0, __wx0);
          __x01_y1 = _mm_or_si128(__x01_y1, __wy0);
        }
        else
        {
          sse2_load8(__x01_y0, src0);
          sse2_load8(__x01_y1, src1);
        }
      }
      else
      {
        src0 += (uint)px0 * 4;

        sse2_load8(__x01_y0, src0);
        sse2_load8(__x01_y1, src0 + srcStride);
      }

      __wx0 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(0, 1, 1, 0));
      __wy0 = _mm_shuffle_epi32(__x01_y1, _MM_SHUFFLE(1, 0, 1, 0));
      __wx0 = _mm_cmpeq_epi32(__wx0, __wy0);

      if (_mm_movemask_epi8(__wx0) == 0xFFFF)
      {
        pp.preprocess_1x1B(__x01_y0);
        goto done;
      }

      __wx0 = _mm_cvtsi32_si128(fx);
      __wy0 = _mm_cvtsi32_si128(fy);

      __wx0 = _mm_shufflelo_epi16(__wx0, _MM_SHUFFLE(0, 0, 0, 0));
      __wy0 = _mm_shufflelo_epi16(__wy0, _MM_SHUFFLE(0, 0, 0, 0));

      __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(1, 0, 1, 0));
      __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(1, 0, 1, 0));

      __wx0 = _mm_srli_epi16(__wx0, 8);
      sse2_unpack_1x1W(__x01_y0, __x01_y0);
      __wy0 = _mm_srli_epi16(__wy0, 8);
      __wx0 = _mm_xor_si128(__wx0, SSE2_GET_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
      sse2_unpack_1x1W(__x01_y1, __x01_y1);
      __wx0 = _mm_add_epi16(__wx0, SSE2_GET_CONST_PI(0000000000000000_0101010101010101));

      pp.preprocess_2x2W(__x01_y0, __x01_y1);

      __ww0 = _mm_mullo_epi16(__wx0, __wy0);
      __wy0 = _mm_xor_si128(__wy0, SSE2_GET_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
      __ww0 = _mm_srli_epi16(__ww0, 8);
      __wy0 = _mm_add_epi16(__wy0, SSE2_GET_CONST_PI(0101010101010101_0101010101010101));

      __wx0 = _mm_slli_epi16(__wx0, 4);
      __wy0 = _mm_slli_epi16(__wy0, 4);

      __x01_y1 = _mm_mullo_epi16(__x01_y1, __ww0);
      __wy0 = _mm_mulhi_epi16(__wy0, __wx0);
      __x01_y0 = _mm_mullo_epi16(__x01_y0, __wy0);

      __x01_y0 = _mm_add_epi16(__x01_y0, __x01_y1);
      __x01_y1 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(1, 0, 3, 2));
      __x01_y0 = _mm_add_epi16(__x01_y0, __x01_y1);

      __x01_y0 = _mm_srli_epi16(__x01_y0, 8);

      sse2_pack_1x1W(__x01_y0, __x01_y0);
done:
      sse2_store4(dstCur, __x01_y0);
      dstCur += 4;

      fx += dx;
      fy += dy;

      if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
      if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
    } while (--i);

    return dst;
  }

  template<typename PP>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_reflect_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PP pp(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw * 2 - 1;
    int th2 = th * 2 - 1;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx;
    int fy;
    _vector_fetch_transformed_sse2(fx, fy, x, y, ctx->m);

    int fxmax = ctx->texture.fxmax;
    int fymax = ctx->texture.fymax;

    int fxrewind = ctx->texture.fxrewind;
    int fyrewind = ctx->texture.fyrewind;

    if ((fx <= -fxmax) | (fx >= fxmax)) fx %= fxmax;
    if ((fy <= -fymax) | (fy >= fymax)) fy %= fymax;

    if (fx < 0) fx += fxmax;
    if (fy < 0) fy += fymax;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    tw--;
    th--;

    int i = w;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      __m128i __x01_y0;
      __m128i __x01_y1;

      __m128i __wx0;
      __m128i __wy0;
      __m128i __ww0;

      if (FOG_UNLIKELY(((uint)py0 >= (uint)th) | ((uint)px0 >= (uint)tw)))
      {
        const uint8_t* src0;

        int px1 = px0 + 1;
        int py1 = py0 + 1;

        if (FOG_UNLIKELY((uint)px0 > (uint)tw)) px0 = tw2 - px0;
        if (FOG_UNLIKELY((uint)py0 > (uint)th)) py0 = th2 - py0;
        if (FOG_UNLIKELY((uint)px1 > (uint)tw)) px1 = tw2 - px1;
        if (FOG_UNLIKELY((uint)py1 > (uint)th)) py1 = th2 - py1;

        src0 = srcBits + (uint)py0 * srcStride;
        sse2_load4(__x01_y0, src0 + (uint)px0 * 4);
        sse2_load4(__wx0   , src0 + (uint)px1 * 4);

        src0 = srcBits + (uint)py1 * srcStride;
        sse2_load4(__x01_y1, src0 + (uint)px0 * 4);
        sse2_load4(__wy0   , src0 + (uint)px1 * 4);

        __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(3, 2, 0, 1));
        __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(3, 2, 0, 1));

        __x01_y0 = _mm_or_si128(__x01_y0, __wx0);
        __x01_y1 = _mm_or_si128(__x01_y1, __wy0);
      }
      else
      {
        const uint8_t* src0 = srcBits + (uint)py0 * srcStride + (uint)px0 * 4;

        sse2_load8(__x01_y0, src0);
        sse2_load8(__x01_y1, src0 + srcStride);
      }

      __wx0 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(0, 1, 1, 0));
      __wy0 = _mm_shuffle_epi32(__x01_y1, _MM_SHUFFLE(1, 0, 1, 0));
      __wx0 = _mm_cmpeq_epi32(__wx0, __wy0);

      if (_mm_movemask_epi8(__wx0) == 0xFFFF)
      {
        pp.preprocess_1x1B(__x01_y0);
        goto done;
      }

      __wx0 = _mm_cvtsi32_si128(fx);
      __wy0 = _mm_cvtsi32_si128(fy);

      __wx0 = _mm_shufflelo_epi16(__wx0, _MM_SHUFFLE(0, 0, 0, 0));
      __wy0 = _mm_shufflelo_epi16(__wy0, _MM_SHUFFLE(0, 0, 0, 0));

      __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(1, 0, 1, 0));
      __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(1, 0, 1, 0));

      __wx0 = _mm_srli_epi16(__wx0, 8);
      sse2_unpack_1x1W(__x01_y0, __x01_y0);
      __wy0 = _mm_srli_epi16(__wy0, 8);
      __wx0 = _mm_xor_si128(__wx0, SSE2_GET_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
      sse2_unpack_1x1W(__x01_y1, __x01_y1);
      __wx0 = _mm_add_epi16(__wx0, SSE2_GET_CONST_PI(0000000000000000_0101010101010101));

      pp.preprocess_2x2W(__x01_y0, __x01_y1);

      __ww0 = _mm_mullo_epi16(__wx0, __wy0);
      __wy0 = _mm_xor_si128(__wy0, SSE2_GET_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
      __ww0 = _mm_srli_epi16(__ww0, 8);
      __wy0 = _mm_add_epi16(__wy0, SSE2_GET_CONST_PI(0101010101010101_0101010101010101));

      __wx0 = _mm_slli_epi16(__wx0, 4);
      __wy0 = _mm_slli_epi16(__wy0, 4);

      __x01_y1 = _mm_mullo_epi16(__x01_y1, __ww0);
      __wy0 = _mm_mulhi_epi16(__wy0, __wx0);
      __x01_y0 = _mm_mullo_epi16(__x01_y0, __wy0);

      __x01_y0 = _mm_add_epi16(__x01_y0, __x01_y1);
      __x01_y1 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(1, 0, 3, 2));
      __x01_y0 = _mm_add_epi16(__x01_y0, __x01_y1);

      __x01_y0 = _mm_srli_epi16(__x01_y0, 8);

      sse2_pack_1x1W(__x01_y0, __x01_y0);
done:
      sse2_store4(dstCur, __x01_y0);
      dstCur += 4;

      fx += dx;
      fy += dy;

      if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
      if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
    } while (--i);

    return dst;
  }

  // --------------------------------------------------------------------------
  // [Gradient - Linear]
  // --------------------------------------------------------------------------

  static uint8_t* FOG_FASTCALL linear_gradient_fetch_subxy_pad(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    const uint32_t* colors = (const uint32_t*)ctx->linearGradient.colors;
    sysint_t colorsLength = ctx->linearGradient.colorsLength;

    int ax = Math::doubleToFixed16x16(ctx->linearGradient.ax);
    int64_t yy = Math::doubleToFixed48x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay );
    int64_t yy_max = Math::doubleToFixed48x16(ctx->linearGradient.dist);

    uint32_t color0 = colors[-1];
    uint32_t color1 = colors[colorsLength];

    int largeThreshold = ax * 4;

    // There are two directions of gradient (ax > 0 or ax < 0):
    if (ax < 0)
    {
      largeThreshold = -largeThreshold;

      while (yy >= yy_max)
      {
        ((uint32_t*)dstCur)[0] = color1;
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      // Large SSE2 loop.
      if (yy >= largeThreshold && w >= 4)
      {
        GRADIENT_SSE2_INTERPOLATE_2_4X_INIT();

        do {
          GRADIENT_SSE2_INTERPOLATE_2_4X_STEP(dstCur, {});
          w -= 4;
          dstCur += 16;
        } while (yy >= largeThreshold && w >= 4);
        if (!w) goto end;
      }

      while (yy >= 0)
      {
        GRADIENT_SSE2_INTERPOLATE_2(dstCur, colors, yy);
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      for (;;)
      {
        ((uint32_t*)dstCur)[0] = color0;
        if (!(--w)) goto end;
        dstCur += 4;
      }
    }
    else
    {
      largeThreshold = (int)yy_max - largeThreshold;

      while (yy <= 0)
      {
        ((uint32_t*)dstCur)[0] = color0;
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      // Large SSE2 loop.
      if (yy <= largeThreshold && w >= 4)
      {
        GRADIENT_SSE2_INTERPOLATE_2_4X_INIT();

        do {
          GRADIENT_SSE2_INTERPOLATE_2_4X_STEP(dstCur, {});
          w -= 4;
          dstCur += 16;
        } while (yy <= largeThreshold && w >= 4);
        if (!w) goto end;
      }

      while (yy < yy_max)
      {
        GRADIENT_SSE2_INTERPOLATE_2(dstCur, colors, yy);
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      for (;;)
      {
        ((uint32_t*)dstCur)[0] = color1;
        if (!(--w)) goto end;
        dstCur += 4;
      }
    }

  end:
    return dst;
  }

  static uint8_t* FOG_FASTCALL linear_gradient_fetch_subxy_repeat(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    const uint32_t* colors = (const uint32_t*)ctx->linearGradient.colors;

    // Not needed to use 64-bit integers for SPREAD_REPEAT, because we can
    // normalize input coordinates to always fit to this data-type.
    int yy_max = Math::doubleToFixed16x16(ctx->linearGradient.dist);
    int ax = Math::doubleToFixed16x16(ctx->linearGradient.ax);
    int yy = (int)((Math::doubleToFixed48x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay )) % yy_max);
    if (yy < 0) yy += yy_max;

    // There are two directions of gradient (ax > 0 or ax < 0):
    if (ax < 0)
    {
      // Large SSE2 loop.
      if (w >= 4)
      {
        GRADIENT_SSE2_INTERPOLATE_2_4X_INIT();

        do {
          GRADIENT_SSE2_INTERPOLATE_2_4X_STEP(dstCur, {
            if (FOG_UNLIKELY(yy < 0)) yy += yy_max;
          });
          w -= 4;
          dstCur += 16;
        } while (w >= 4);
        if (!w) goto end;
      }

      for (;;)
      {
        GRADIENT_SSE2_INTERPOLATE_2(dstCur, colors, yy);
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
        if (FOG_UNLIKELY(yy < 0)) yy += yy_max;
      }
    }
    else
    {
      // Large SSE2 loop.
      if (w >= 4)
      {
        GRADIENT_SSE2_INTERPOLATE_2_4X_INIT();

        do {
          GRADIENT_SSE2_INTERPOLATE_2_4X_STEP(dstCur, {
            if (FOG_UNLIKELY(yy >= yy_max)) yy -= yy_max;
          });
          w -= 4;
          dstCur += 16;
        } while (w >= 4);
        if (!w) goto end;
      }

      for (;;)
      {
        GRADIENT_SSE2_INTERPOLATE_2(dstCur, colors, yy);
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
        if (FOG_UNLIKELY(yy >= yy_max)) yy -= yy_max;
      }
    }

  end:
    return dst;
  }

  // --------------------------------------------------------------------------
  // [Gradient - Radial]
  // --------------------------------------------------------------------------

  static uint8_t* FOG_FASTCALL radial_gradient_fetch_pad(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    const uint32_t* colors = (const uint32_t*)ctx->radialGradient.colors;
    sysint_t colorsLength = ctx->radialGradient.colorsLength;

    // Here we use some tricks to do computation faster. First trick is that we
    // can add incrementing values [3, 2, 1, 0] to dx. We need this, because we
    // are computing 4 pixels at time and we need to set correct offset for each.
    __m128 dx = _mm_add_ps(_mm_set1_ps((float)x - (float)ctx->radialGradient.dx), _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f));
    __m128 dy = _mm_set1_ps((float)y - (float)ctx->radialGradient.dy);

    __m128 fx = _mm_set1_ps((float)ctx->radialGradient.fx);
    __m128 fy = _mm_set1_ps((float)ctx->radialGradient.fy);
    __m128 r2 = _mm_set1_ps((float)ctx->radialGradient.r2);

    __m128 dxdx = _mm_mul_ps(dx, dx);

    // cc = (dy*dy + dx*dx) * r2 - ((dx*fy - dy*fx) * (dx*fy - dy*fx))
    // cx = (dx*r2)              - ((dx*fy - dy*fx) * fy)
    // ci = r2                   - (fy*fy)
    __m128 cc = _mm_add_ps(_mm_mul_ps(dy, dy), dxdx);
    cc = _mm_mul_ps(cc, r2);
    __m128 cx = _mm_mul_ps(dx, r2);
    __m128 ci = _mm_sub_ps(r2, _mm_mul_ps(fy, fy));

    // pp = dx * fy - dy * fx
    __m128 pp = _mm_sub_ps(_mm_mul_ps(dx, fy), _mm_mul_ps(dy, fx));

    cx = _mm_sub_ps(cx, _mm_mul_ps(pp, fy));
    cc = _mm_sub_ps(cc, _mm_mul_ps(pp, pp));

    // dd = (dx*fx + dy*fy)
    // di = fx
    __m128 dd = _mm_add_ps(_mm_mul_ps(dx, fx), _mm_mul_ps(dy, fy));
    __m128 di = _mm_mul_ps(fx, _mm_set1_ps(4.0));

    // ci4 = ci * 4
    // ci16 = ci * 16
    __m128 ci4 = _mm_mul_ps(ci, _mm_set1_ps(4.0));
    __m128 ci16 = _mm_mul_ps(ci, _mm_set1_ps(16.0));

    __m128 eight = _mm_set1_ps(8.0);

    __m128 scale         = _mm_set1_ps((float)ctx->radialGradient.mul);
    __m128 zero          = _mm_set1_ps((float)0.0f);
    __m128 threshold     = _mm_set1_ps((float)(colorsLength-1));

    // Here we are using trict to get another performance gain. The square root
    // calculation is not cheap, so we want situation where FPU calculates square
    // root for next 4 pixels while we are processing current ones.
    __m128 idxf;

    idxf = _mm_and_ps(cc, SSE2_GET_CONST_PS(7FFFFFFF7FFFFFFF_7FFFFFFF7FFFFFFF)); // abs()
    idxf = _mm_sqrt_ps(idxf);

    for (;;)
    {
      __m128i idxi;
      uint index;

      idxf = _mm_add_ps(idxf, dd);
      idxf = _mm_mul_ps(idxf, scale);
      idxf = _mm_max_ps(idxf, zero);
      idxf = _mm_min_ps(idxf, threshold);

      idxi = _mm_cvtps_epi32(idxf);

      // Next 4 pixels

      // cc += (cx + cx + ci) +
      //       (cx+ci + cx+ci + ci) +
      //       (cx+ci+ci + cx+ci+ci + ci) +
      //       (cx+ci+ci+ci + cx+ci+ci+ci + ci)
      // cc += cx*8 + ci*16;
      // cx += ci*4
      cc = _mm_add_ps(cc, _mm_mul_ps(cx, eight));
      cc = _mm_add_ps(cc, ci16);
      cx = _mm_add_ps(cx, ci4);

      dd = _mm_add_ps(dd, di);

      // This step is important, _mm_sqrt_ps() is last called and from now
      // this register will not be used.
      idxf = _mm_and_ps(cc, SSE2_GET_CONST_PS(7FFFFFFF7FFFFFFF_7FFFFFFF7FFFFFFF)); // abs()
      idxf = _mm_sqrt_ps(idxf);

      // Pixel #0
      index = (uint)_mm_cvtsi128_si32(idxi);
      idxi = _mm_shuffle_epi32(idxi, _MM_SHUFFLE(0, 3, 2, 1));

      ((uint32_t*)dstCur)[0] = colors[index];
      if (--w == 0) break;

      // Pixel #1
      index = (uint)_mm_cvtsi128_si32(idxi);
      idxi = _mm_shuffle_epi32(idxi, _MM_SHUFFLE(0, 3, 2, 1));

      ((uint32_t*)dstCur)[1] = colors[index];
      if (--w == 0) break;

      // Pixel #2
      index = (uint)_mm_cvtsi128_si32(idxi);
      idxi = _mm_shuffle_epi32(idxi, _MM_SHUFFLE(0, 3, 2, 1));

      ((uint32_t*)dstCur)[2] = colors[index];
      if (--w == 0) break;

      // Pixel #3
      index = (uint)_mm_cvtsi128_si32(idxi);

      ((uint32_t*)dstCur)[3] = colors[index];
      if (--w == 0) break;

      dstCur += 16;
    }

    return dst;
  }
};

} // RasterEngine namespace
} // Fog namespace
