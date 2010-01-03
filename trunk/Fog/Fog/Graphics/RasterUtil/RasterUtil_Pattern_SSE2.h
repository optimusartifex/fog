// [Fog/Graphics Library - C++ API]
//
// [Licence]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Build/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterUtil/RasterUtil_Defs_SSE2.h>
#include <Fog/Graphics/RasterUtil/RasterUtil_Dib_SSE2.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterUtil {

// ============================================================================
// [Fog::RasterUtil::SSE2 - Helpers]
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
  pix_load8(__x0xmm, &COLORS[(int)(YY >> 16)]); \
  \
  __w0xmm = _mm_cvtsi32_si128((int)YY); \
  __w0xmm = _mm_srli_epi16(__w0xmm, 8); \
  __w0xmm = _mm_shufflelo_epi16(__w0xmm, _MM_SHUFFLE(0, 0, 0, 0)); \
  __w0xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(0, 0, 0, 0)); \
  __w0xmm = _mm_xor_si128(__w0xmm, Mask_0000000000000000_FFFFFFFFFFFFFFFF); \
  __w0xmm = _mm_add_epi16(__w0xmm, Mask_0000000000000000_0101010101010101); \
  \
  pix_unpack_1x2W(__x0xmm, __x0xmm); \
  \
  __x0xmm = _mm_mullo_epi16(__x0xmm, __w0xmm); \
  __w0xmm = _mm_shuffle_epi32(__x0xmm, _MM_SHUFFLE(1, 0, 3, 2)); \
  \
  __x0xmm = _mm_adds_epu16(__x0xmm, __w0xmm); \
  __x0xmm = _mm_srli_epi16(__x0xmm, 8); \
  \
  pix_pack_1x1W(__x0xmm, __x0xmm); \
  pix_store4(DST, __x0xmm); \
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
  pix_load8(__x0xmm, &colors[pos]); \
  \
  __w0xmm = _mm_srli_epi16(__w0xmm, 8); \
  \
  pos = (int)(yy >> 16); yy += ax; \
  COND \
  pix_load8(__x1xmm, &colors[pos]); \
  \
  __w0xmm = _mm_xor_si128(__w0xmm, Mask_0000000000000000_FFFFFFFFFFFFFFFF); \
  __w0xmm = _mm_add_epi16(__w0xmm, Mask_0000000000000000_0101010101010101); \
  \
  pos = (int)(yy >> 16); yy += ax; \
  COND \
  pix_load8(__x2xmm, &colors[pos]); \
  \
  /* INTERPOLATE [0] AND [1], AND SOME WEIGHT STUFF FOR [2] AND [3] PIXELS */ \
  __w1xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(3, 3, 1, 1)); \
  __w0xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(2, 2, 0, 0)); \
  \
  pix_unpack_1x2W(__x0xmm, __x0xmm); \
  pix_unpack_1x2W(__x1xmm, __x1xmm); \
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
  __w0xmm = _mm_xor_si128(__w0xmm, Mask_0000000000000000_FFFFFFFFFFFFFFFF); \
  __x1xmm = _mm_srli_epi16(__x1xmm, 8); \
  __w0xmm = _mm_add_epi16(__w0xmm, Mask_0000000000000000_0101010101010101); \
  \
  __x0xmm = _mm_packus_epi16(__x0xmm, __x1xmm); \
  \
  /* LOAD PIXEL [3] */ \
  pos = (int)(yy >> 16); yy += ax; \
  COND \
  pix_load8(__x1xmm, &colors[pos]); \
  \
  /* INTERPOLATE [2] AND [3] */ \
  __w1xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(3, 3, 1, 1)); \
  __w0xmm = _mm_shuffle_epi32(__w0xmm, _MM_SHUFFLE(2, 2, 0, 0)); \
  \
  pix_unpack_1x2W(__x2xmm, __x2xmm); \
  pix_unpack_1x2W(__x1xmm, __x1xmm); \
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
  pix_store8(reinterpret_cast<char*>(DST)    , __x0xmm); \
  pix_store8(reinterpret_cast<char*>(DST) + 8, __x2xmm); \
  \
  /* ADVANCE */ \
  __yypos = _mm_add_epi16(__yypos, __axinc); \
}

#define TEXTURE_SSE2_INTERPOLATE_BILINEAR_32() \
{ \
  int px0 = fx >> 16; \
  int py0 = fy >> 16; \
  \
  int px1 = px0 + 1; \
  int py1 = py0 + 1; \
  \
  if (FOG_UNLIKELY(py1 >= th)) py1 -= th; \
  \
  __m128i __pix_x01_y0; \
  __m128i __pix_x01_y1; \
  \
  __m128i __wx0; \
  __m128i __wy0; \
  __m128i __ww0; \
  \
  if (FOG_UNLIKELY(px1 >= tw)) \
  { \
    const uint8_t* src0 = srcBits + py0 * srcStride; \
    const uint8_t* src1 = srcBits + py1 * srcStride; \
    px1 -= tw; \
    \
    pix_load4(__pix_x01_y0, src0 + px0 * 4); \
    pix_load4(__pix_x01_y1, src1 + px0 * 4); \
    \
    pix_load4(__wx0, src0 + px1 * 4); \
    pix_load4(__wy0, src1 + px1 * 4); \
    \
    __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(3, 2, 0, 1)); \
    __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(3, 2, 0, 1)); \
    \
    __pix_x01_y0 = _mm_or_si128(__pix_x01_y0, __wx0); \
    __pix_x01_y1 = _mm_or_si128(__pix_x01_y1, __wy0); \
  } \
  else \
  { \
    const uint8_t* src0 = srcBits + px0 * 4; \
    const uint8_t* src1 = src0 + py1 * srcStride; \
    \
    src0 += py0 * srcStride; \
    \
    pix_load8(__pix_x01_y0, src0); \
    pix_load8(__pix_x01_y1, src1); \
  } \
  \
  __wx0 = _mm_cvtsi32_si128(fx); \
  __wy0 = _mm_cvtsi32_si128(fy); \
  \
  __wx0 = _mm_shufflelo_epi16(__wx0, _MM_SHUFFLE(0, 0, 0, 0)); \
  __wy0 = _mm_shufflelo_epi16(__wy0, _MM_SHUFFLE(0, 0, 0, 0)); \
  \
  __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(1, 0, 1, 0)); \
  __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(1, 0, 1, 0)); \
  \
  __wx0 = _mm_srli_epi16(__wx0, 8); \
  pix_unpack_1x1W(__pix_x01_y0, __pix_x01_y0); \
  __wy0 = _mm_srli_epi16(__wy0, 8); \
  __wx0 = _mm_xor_si128(__wx0, Mask_0000000000000000_FFFFFFFFFFFFFFFF); \
  pix_unpack_1x1W(__pix_x01_y1, __pix_x01_y1); \
  __wx0 = _mm_add_epi16(__wx0, Mask_0000000000000000_0101010101010101); \
  \
  __ww0 = _mm_mullo_epi16(__wx0, __wy0); \
  __wy0 = _mm_xor_si128(__wy0, Mask_FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF); \
  __ww0 = _mm_srli_epi16(__ww0, 8); \
  __wy0 = _mm_add_epi16(__wy0, Mask_0101010101010101_0101010101010101); \
  \
  __wy0 = _mm_slli_epi16(__wy0, 4); \
  __wx0 = _mm_slli_epi16(__wx0, 4); \
  \
  __pix_x01_y1 = _mm_mullo_epi16(__pix_x01_y1, __ww0); \
  __wy0 = _mm_mulhi_epi16(__wy0, __wx0); \
  __pix_x01_y0 = _mm_mullo_epi16(__pix_x01_y0, __wy0); \
  \
  __pix_x01_y0 = _mm_add_epi16(__pix_x01_y0, __pix_x01_y1); \
  __pix_x01_y1 = _mm_shuffle_epi32(__pix_x01_y0, _MM_SHUFFLE(1, 0, 3, 2)); \
  __pix_x01_y0 = _mm_add_epi16(__pix_x01_y0, __pix_x01_y1); \
  \
  __pix_x01_y0 = _mm_srli_epi16(__pix_x01_y0, 8); \
  \
  pix_pack_1x1W(__pix_x01_y0, __pix_x01_y0); \
  pix_store4(dstCur, __pix_x01_y0); \
  \
  dstCur += 4; \
}

// ============================================================================
// [Fog::RasterUtil::SSE2 - Pattern]
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

    x -= ctx->texture.dx;
    y -= ctx->texture.dy;

    if (x < 0) x = (x % tw) + tw;
    if (x >= tw) x %= tw;

    if (y < 0) y = (y % th) + th;
    if (y >= th) y %= th;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    int i;

    srcCur = srcBase + ByteUtil::mul4(x);

    // Return image buffer if span fits to it (this is very efficient
    // optimization for short spans or large textures)
    i = Math::min(tw - x, w);
    if (w < tw - x)
      return const_cast<uint8_t*>(srcCur);

    // This is equal to C implementation in Raster_C.cpp
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
          pix_load16u(src0mm, srcCur +  0);
          pix_load16u(src1mm, srcCur + 16);
          pix_store16a(dstCur +  0, src0mm);
          pix_store16a(dstCur + 16, src1mm);

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

    x -= ctx->texture.dx;
    y -= ctx->texture.dy;

    if (x < 0) x = (x % tw2) + tw2;
    if (x >= tw2) x %= tw2;

    if (y < 0) y = (y % th2) + th2;
    if (y >= th2) y %= th2;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th) y = th2 - y - 1;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    if (x >= 0 && x <= tw && w < tw - x)
      return const_cast<uint8_t*>(srcBase + ByteUtil::mul4(x));

    do {
      // Reflect mode
      if (x >= tw)
      {
        int i = Math::min(tw2 - x, w);

        srcCur = srcBase + ByteUtil::mul4(tw2 - x - 1);

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

            pix_load16u(src0mm, srcCur + 20);
            pix_load16u(src1mm, srcCur + 4);
            src0mm = _mm_shuffle_epi32(src0mm, _MM_SHUFFLE(0, 1, 2, 3));
            src1mm = _mm_shuffle_epi32(src1mm, _MM_SHUFFLE(0, 1, 2, 3));
            pix_store16a(dstCur +  0, src0mm);
            pix_store16a(dstCur + 16, src1mm);

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

        srcCur = srcBase + ByteUtil::mul4(x);

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
            pix_load16u(src0mm, srcCur +  0);
            pix_load16u(src1mm, srcCur + 16);
            pix_store16a(dstCur +  0, src0mm);
            pix_store16a(dstCur + 16, src1mm);

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
  // [Texture - Transform - Bilinear]
  // --------------------------------------------------------------------------

  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_repeat_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int fx = double_to_fixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = double_to_fixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    int dx = double_to_fixed16x16(ctx->m[MATRIX_SX]);
    int dy = double_to_fixed16x16(ctx->m[MATRIX_SHY]);

    int fxmax = tw << 16;
    int fymax = th << 16;

    fx -= 0x8000;
    fy -= 0x8000;

    if (fx < 0 || fx >= fxmax) { fx %= fxmax; if (fx < 0) fx += fxmax; }
    if (fy < 0 || fy >= fymax) { fy %= fymax; if (fy < 0) fy += fymax; }

    if (dx <= -fxmax || dx >= fxmax) { dx %= fxmax; }
    if (dy <= -fymax || dy >= fymax) { dy %= fymax; }

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;

    if (dx >= 0 && dy >= 0)
    {
      do {
        TEXTURE_SSE2_INTERPOLATE_BILINEAR_32()

        fx += dx;
        fy += dy;

        if (fx >= fxmax) fx -= fxmax;
        if (fy >= fymax) fy -= fymax;
      } while (--i);
    }
    else if (dx >= 0 && dy < 0)
    {
      do {
        TEXTURE_SSE2_INTERPOLATE_BILINEAR_32()

        fx += dx;
        fy += dy;

        if (fx >= fxmax) fx -= fxmax;
        if (fy < 0) fy += fymax;
      } while (--i);
    }
    else if (dx < 0 && dy >= 0)
    {
      do {
        TEXTURE_SSE2_INTERPOLATE_BILINEAR_32()

        fx += dx;
        fy += dy;

        if (fx < 0) fx += fxmax;
        if (fy >= fymax) fy -= fymax;
      } while (--i);
    }
    else // if (dx < 0 && dy < 0)
    {
      do {
        TEXTURE_SSE2_INTERPOLATE_BILINEAR_32()

        fx += dx;
        fy += dy;

        if (fx < 0) fx += fxmax;
        if (fy < 0) fy += fymax;
      } while (--i);
    }

    // Fetch the rest.
    // if (w) _texture_do_repeat(dstCur, ctx->texture.w, w);

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

    int ax = double_to_fixed16x16(ctx->linearGradient.ax);
    int64_t yy = double_to_fixed48x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay );
    int64_t yy_max = double_to_fixed48x16(ctx->linearGradient.dist);

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
    int yy_max = double_to_fixed16x16(ctx->linearGradient.dist);
    int ax = double_to_fixed16x16(ctx->linearGradient.ax);
    int yy = (int)((double_to_fixed48x16(
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

    idxf = _mm_and_ps(cc, Mask_7FFFFFFF7FFFFFFF_7FFFFFFF7FFFFFFF); // abs()
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
      idxf = _mm_and_ps(cc, Mask_7FFFFFFF7FFFFFFF_7FFFFFFF7FFFFFFF); // abs()
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

} // RasterUtil namespace
} // Fog namespace