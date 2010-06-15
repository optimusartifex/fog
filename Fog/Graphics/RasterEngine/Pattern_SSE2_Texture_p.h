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
// [Fog::RasterEngine::SSE2 - Pattern - Texture]
// ============================================================================

//! @internal
struct FOG_HIDDEN PatternTextureSSE2
{
  // --------------------------------------------------------------------------
  // [Pattern - Texture - Exact]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL fetch_exact_repeat_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    y += ctx->texture.dy;
    if ((y >= th) | (y <= -th)) y %= th;
    if (y < 0) y += th;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      x += ctx->texture.dx;
      if ((x >= tw) | (x <= -tw)) x %= tw;
      if (x < 0) x += tw;

      srcCur = srcBase + (uint)x * 4;

      // Set image buffer if span fits to it (this is very efficient
      // optimization for short spans or large textures).
      int i = Math::min(tw - x, w);
      if (mode == PATTERN_FETCH_CAN_USE_SRC && w < tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(srcCur);
        goto fetchSkip;
      }

      // This is equal to C implementation in RasterEngine_Pattern_C_p.h.
      for (;;)
      {
        w -= i;

        if (i >= 32)
        {
          // We can improve texture fetching by more pixels at a time.
          while ((sysuint_t)dst & 15)
          {
            ((uint32_t*)dst)[0] = ((const uint32_t*)srcCur)[0];
            dst += 4;
            srcCur += 4;
            if (--i == 0) goto fetchEnd;
          }

          while (i >= 8)
          {
            __m128i src0mm;
            __m128i src1mm;
            sse2_load16u(src0mm, srcCur +  0);
            sse2_load16u(src1mm, srcCur + 16);
            sse2_store16a(dst +  0, src0mm);
            sse2_store16a(dst + 16, src1mm);

            dst += 32;
            srcCur += 32;
            i -= 8;
          }
          if (i == 0) goto fetchEnd;
        }

        do {
          ((uint32_t*)dst)[0] = ((const uint32_t*)srcCur)[0];
          dst += 4;
          srcCur += 4;
        } while (--i);
fetchEnd:
        if (!w) break;

        i = Math::min(w, tw);
        srcCur = srcBase;
      }

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  static void FOG_FASTCALL fetch_exact_reflect_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    y += ctx->texture.dy;
    if ((y >= th2) | (y <= -th2)) y %= th2;
    if (y < 0) y += th2;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th) y = th2 - y - 1;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      x += ctx->texture.dx;
      if ((x >= tw2) | (x <= -tw2)) x %= tw2;
      if (x < 0) x += tw2;

      // Set image buffer if span fits to it (this is very efficient
      // optimization for short spans or large textures).
      if (mode == PATTERN_FETCH_CAN_USE_SRC && x >= 0 && x <= tw && w < tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(srcBase + (uint)x * 4)
        goto fetchSkip;
      }

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
            while ((sysuint_t)dst & 15)
            {
              ((uint32_t*)dst)[0] = ((const uint32_t*)srcCur)[0];
              dst += 4;
              srcCur -= 4;
              if (--i == 0) goto fetchEnd;
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
              sse2_store16a(dst +  0, src0mm);
              sse2_store16a(dst + 16, src1mm);

              dst += 32;
              i -= 8;
            }
            if (i == 0) goto fetchEnd;
          }

          do {
            ((uint32_t*)dst)[0] = ((const uint32_t*)srcCur)[0];
            dst += 4;
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
            while ((sysuint_t)dst & 15)
            {
              ((uint32_t*)dst)[0] = ((const uint32_t*)srcCur)[0];
              dst += 4;
              srcCur += 4;
              if (--i == 0) goto fetchEnd;
            }

            while (i >= 8)
            {
              __m128i src0mm;
              __m128i src1mm;
              sse2_load16u(src0mm, srcCur +  0);
              sse2_load16u(src1mm, srcCur + 16);
              sse2_store16a(dst +  0, src0mm);
              sse2_store16a(dst + 16, src1mm);

              dst += 32;
              srcCur += 32;
              i -= 8;
            }
            if (i == 0) goto fetchEnd;
          }

          do {
            ((uint32_t*)dst)[0] = ((const uint32_t*)srcCur)[0];
            dst += 4;
            srcCur += 4;
          } while (--i);
        }
fetchEnd:
        ;
      } while (w);

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  // --------------------------------------------------------------------------
  // [Pattern - Texture - Transform - Bilinear]
  // --------------------------------------------------------------------------

  template<typename PP>
  static void FOG_FASTCALL fetch_trans_affine_bilinear_none_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    PP pp(ctx);

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx;
    int fy;
    sse2_vector_fetch_transformed(fx, fy, x, y, ctx->m);

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    tw--;
    th--;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      for (;;)
      {
        int px0 = fx >> 16;
        int py0 = fy >> 16;

        __m128i __x01_y0;
        __m128i __x01_y1;

        __m128i __wx0;
        __m128i __wy0;

        const uint8_t* src0 = srcBits + py0 * srcStride + px0 * 4;

        if (FOG_UNLIKELY(((uint)py0 >= (uint)th) | ((uint)px0 >= (uint)tw)))
        {
          __x01_y0 = _mm_setzero_si128();
          __x01_y1 = _mm_setzero_si128();

          __wx0 = _mm_setzero_si128();
          __wy0 = _mm_setzero_si128();

          if ((uint)py0 <= (uint)th)
          {
            if ((uint)px0     <= (uint)tw) { sse2_load4(__x01_y0, src0    ); if (!PP::USE_ALPHA) pp.preprocess_1x1B(__x01_y0); }
            if ((uint)px0 + 1 <= (uint)tw) { sse2_load4(__wx0   , src0 + 4); if (!PP::USE_ALPHA) pp.preprocess_1x1B(__wx0   ); }
          }

          py0++;
          src0 += srcStride;

          if ((uint)py0 <= (uint)th)
          {
            if ((uint)px0     <= (uint)tw) { sse2_load4(__x01_y1, src0    ); if (!PP::USE_ALPHA) pp.preprocess_1x1B(__x01_y1); }
            if ((uint)px0 + 1 <= (uint)tw) { sse2_load4(__wy0   , src0 + 4); if (!PP::USE_ALPHA) pp.preprocess_1x1B(__wy0   ); }
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

          if (!PP::USE_ALPHA)
          {
            pp.preprocess_1x2B(__x01_y0);
            pp.preprocess_1x2B(__x01_y1);
          }
        }

        __wx0 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(0, 1, 1, 0));
        __wy0 = _mm_shuffle_epi32(__x01_y1, _MM_SHUFFLE(1, 0, 1, 0));
        __wx0 = _mm_cmpeq_epi32(__wx0, __wy0);

        if (_mm_movemask_epi8(__wx0) == 0xFFFF)
        {
          if (PP::USE_PREPROCESS)
            goto fetchNoFilter;
          else
            goto fetchDone;
        }

        __wx0 = _mm_cvtsi32_si128(fx);
        __wy0 = _mm_cvtsi32_si128(fy);

        __wx0 = _mm_shufflelo_epi16(__wx0, _MM_SHUFFLE(0, 0, 0, 0));
        __wy0 = _mm_shufflelo_epi16(__wy0, _MM_SHUFFLE(0, 0, 0, 0));

        __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(1, 0, 1, 0));
        __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(1, 0, 1, 0));

        __wx0 = _mm_srli_epi16(__wx0, 8);
        __wy0 = _mm_srli_epi16(__wy0, 8);

        sse2_unpack_1x2W(__x01_y0, __x01_y0);
        sse2_unpack_1x2W(__x01_y1, __x01_y1);

        if (PP::USE_ALPHA)
        {
          pp.preprocess_2x2W(__x01_y0, __x01_y1);
        }

        __x01_y1 = _mm_mullo_epi16(__x01_y1, __wy0);
        __wy0 = _mm_xor_si128(__wy0, FOG_GET_SSE_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
        __wx0 = _mm_xor_si128(__wx0, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
        __wy0 = _mm_add_epi16(__wy0, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
        __wx0 = _mm_add_epi16(__wx0, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101));
        __x01_y0 = _mm_mullo_epi16(__x01_y0, __wy0);
        __x01_y0 = _mm_adds_epu8(__x01_y0, __x01_y1);
        __x01_y0 = _mm_srli_epi16(__x01_y0, 8);
        __x01_y0 = _mm_mullo_epi16(__x01_y0, __wx0);
        __x01_y1 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(1, 0, 3, 2));
        __x01_y0 = _mm_adds_epu8(__x01_y0, __x01_y1);
        __x01_y0 = _mm_srli_epi16(__x01_y0, 8);
        sse2_pack_1x1W(__x01_y0, __x01_y0);

fetchDone:
        sse2_store4(dst, __x01_y0);
        dst += 4;

        fx += dx;
        fy += dy;

        if (--w) continue;
        break;

fetchNoFilter:
        pp.preprocess_1x1B(__x01_y0);
        sse2_store4(dst, __x01_y0);
        dst += 4;

        fx += dx;
        fy += dy;

        if (--w) continue;
        break;
      }

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += dy * hole;
      })
    P_FETCH_SPAN8_END()
  }

  template<typename PP>
  static void FOG_FASTCALL fetch_trans_affine_bilinear_pad_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    PP pp(ctx);

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx;
    int fy;
    sse2_vector_fetch_transformed(fx, fy, x, y, ctx->m);

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    tw--;
    th--;

    // ------------------------------------------------------------------------
    // [Scale]
    // ------------------------------------------------------------------------

    if (dy == 0)
    {
      const uint8_t* src0;
      const uint8_t* src1;

      {
        const uint8_t* srcBits = ctx->texture.bits;
        sysint_t srcStride = ctx->texture.stride;

        int py0 = fy >> 16;
        int py1 = py0 + 1;

        if      (py0 <  0) { py0 = py1 =  0; }
        else if (py0 > th) { py0 = py1 = th; }

        src0 = srcBits + py0 * srcStride;
        src1 = srcBits + py1 * srcStride;
      }

      __m128i __wy0_val = _mm_cvtsi32_si128(fy);
      __m128i __wy0_inv;

      __wy0_val = _mm_shufflelo_epi16(__wy0_val, _MM_SHUFFLE(0, 0, 0, 0));
      __wy0_val = _mm_shuffle_epi32(__wy0_val, _MM_SHUFFLE(1, 0, 1, 0));
      __wy0_val = _mm_srli_epi16(__wy0_val, 8);

      __wy0_inv = _mm_xor_si128(__wy0_val, FOG_GET_SSE_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
      __wy0_inv = _mm_add_epi16(__wy0_inv, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        for (;;)
        {
          int px0 = fx >> 16;

          __m128i __x01_y0_0, __x01_y0_1;
          __m128i __x01_y1_0, __x01_y1_1;
          __m128i __wx0_0   , __wx0_1   ;

          __wx0_0 = _mm_cvtsi32_si128(fx);

          fx += dx;

          if ((uint)px0 >= (uint)tw)
            goto fetchScaleOneBegin;

          sse2_load8(__x01_y0_0, src0 + px0 * 4);
          sse2_load8(__x01_y1_0, src1 + px0 * 4);

          if (--w == 0)
            goto fetchScaleOneDetect;

          __x01_y0_1 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(0, 1, 1, 0));
          __x01_y1_1 = _mm_shuffle_epi32(__x01_y1_0, _MM_SHUFFLE(1, 0, 1, 0));

          __wx0_0 = _mm_shufflelo_epi16(__wx0_0, _MM_SHUFFLE(0, 0, 0, 0));

          __x01_y0_1 = _mm_cmpeq_epi32(__x01_y0_1, __x01_y1_1);
          if (_mm_movemask_epi8(__x01_y0_1) == 0xFFFF)
            goto fetchScaleOneNoFilter;

          px0 = fx >> 16;

          if ((uint)px0 >= (uint)tw)
            goto fetchScaleOneContinue;

          sse2_load8(__x01_y0_1, src0 + px0 * 4);
          sse2_load8(__x01_y1_1, src1 + px0 * 4);

          __wx0_1 = _mm_cvtsi32_si128(fx);
          __wx0_0 = _mm_shuffle_epi32(__wx0_0, _MM_SHUFFLE(1, 0, 1, 0));
          __wx0_1 = _mm_shufflelo_epi16(__wx0_1, _MM_SHUFFLE(0, 0, 0, 0));
          __wx0_0 = _mm_srli_epi16(__wx0_0, 8);
          __wx0_1 = _mm_shuffle_epi32(__wx0_1, _MM_SHUFFLE(1, 0, 1, 0));
          __wx0_1 = _mm_srli_epi16(__wx0_1, 8);

          sse2_unpack_1x2W(__x01_y0_0, __x01_y0_0);
          sse2_unpack_1x2W(__x01_y0_1, __x01_y0_1);
          sse2_unpack_1x2W(__x01_y1_0, __x01_y1_0);
          sse2_unpack_1x2W(__x01_y1_1, __x01_y1_1);

          pp.preprocess_2x2W(__x01_y0_0, __x01_y1_0);
          pp.preprocess_2x2W(__x01_y0_1, __x01_y1_1);

          __x01_y0_0 = _mm_mullo_epi16(__x01_y0_0, __wy0_inv);
          __x01_y0_1 = _mm_mullo_epi16(__x01_y0_1, __wy0_inv);
          __x01_y1_0 = _mm_mullo_epi16(__x01_y1_0, __wy0_val);
          __x01_y1_1 = _mm_mullo_epi16(__x01_y1_1, __wy0_val);
          __wx0_0 = _mm_xor_si128(__wx0_0, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
          __wx0_1 = _mm_xor_si128(__wx0_1, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
          __wx0_0 = _mm_add_epi16(__wx0_0, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101));
          __wx0_1 = _mm_add_epi16(__wx0_1, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101));
          __x01_y0_0 = _mm_adds_epu8(__x01_y0_0, __x01_y1_0);
          __x01_y0_1 = _mm_adds_epu8(__x01_y0_1, __x01_y1_1);
          __x01_y0_0 = _mm_srli_epi16(__x01_y0_0, 8);
          __x01_y0_1 = _mm_srli_epi16(__x01_y0_1, 8);
          __x01_y0_0 = _mm_mullo_epi16(__x01_y0_0, __wx0_0);
          __x01_y0_1 = _mm_mullo_epi16(__x01_y0_1, __wx0_1);
          __x01_y1_0 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(1, 0, 3, 2));
          __x01_y1_1 = _mm_shuffle_epi32(__x01_y0_1, _MM_SHUFFLE(1, 0, 3, 2));
          __x01_y0_0 = _mm_adds_epu8(__x01_y0_0, __x01_y1_0);
          __x01_y0_1 = _mm_adds_epu8(__x01_y0_1, __x01_y1_1);
          __x01_y0_0 = _mm_srli_epi16(__x01_y0_0, 8);
          __x01_y0_1 = _mm_srli_epi16(__x01_y0_1, 8);
          sse2_pack_2x2W(__x01_y0_0, __x01_y0_0, __x01_y0_1);
          __x01_y0_0 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(0, 0, 2, 0));

          sse2_store8(dst, __x01_y0_0);
          dst += 8;
          fx += dx;

          if (--w) continue;
          break;

fetchScaleOneBegin:
          w--;
          {
            if (px0 < 0) px0 = 0; else px0 = tw;

            sse2_load4(__x01_y0_0, src0 + px0 * 4);
            sse2_load4(__x01_y0_1, src0 + px0 * 4);

            sse2_load4(__x01_y1_0, src1 + px0 * 4);
            sse2_load4(__x01_y1_1, src1 + px0 * 4);

            __x01_y0_1 = _mm_shuffle_epi32(__x01_y0_1, _MM_SHUFFLE(3, 2, 0, 1));
            __x01_y1_1 = _mm_shuffle_epi32(__x01_y1_1, _MM_SHUFFLE(3, 2, 0, 1));

            __x01_y0_0 = _mm_or_si128(__x01_y0_0, __x01_y0_1);
            __x01_y1_0 = _mm_or_si128(__x01_y1_0, __x01_y1_1);
          }

fetchScaleOneDetect:
          __x01_y0_1 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(0, 1, 1, 0));
          __x01_y1_1 = _mm_shuffle_epi32(__x01_y1_0, _MM_SHUFFLE(1, 0, 1, 0));
          __wx0_0 = _mm_shufflelo_epi16(__wx0_0, _MM_SHUFFLE(0, 0, 0, 0));
          __x01_y0_1 = _mm_cmpeq_epi32(__x01_y0_1, __x01_y1_1);

          if (_mm_movemask_epi8(__x01_y0_1) == 0xFFFF)
          {
            if (PP::USE_PREPROCESS)
              goto fetchScaleOneNoFilter;
            else
              goto fetchScaleOneDone;
          }

fetchScaleOneContinue:
          __wx0_0 = _mm_shuffle_epi32(__wx0_0, _MM_SHUFFLE(1, 0, 1, 0));
          __wx0_0 = _mm_srli_epi16(__wx0_0, 8);

          sse2_unpack_1x2W(__x01_y0_0, __x01_y0_0);
          sse2_unpack_1x2W(__x01_y1_0, __x01_y1_0);

          pp.preprocess_2x2W(__x01_y0_0, __x01_y1_0);

          __x01_y1_0 = _mm_mullo_epi16(__x01_y1_0, __wy0_val);
          __wx0_0 = _mm_xor_si128(__wx0_0, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
          __x01_y0_0 = _mm_mullo_epi16(__x01_y0_0, __wy0_inv);
          __wx0_0 = _mm_add_epi16(__wx0_0, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101));
          __x01_y0_0 = _mm_adds_epu8(__x01_y0_0, __x01_y1_0);
          __x01_y0_0 = _mm_srli_epi16(__x01_y0_0, 8);
          __x01_y0_0 = _mm_mullo_epi16(__x01_y0_0, __wx0_0);
          __x01_y1_0 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(1, 0, 3, 2));
          __x01_y0_0 = _mm_adds_epu8(__x01_y0_0, __x01_y1_0);
          __x01_y0_0 = _mm_srli_epi16(__x01_y0_0, 8);
          sse2_pack_1x1W(__x01_y0_0, __x01_y0_0);
          goto fetchScaleOneDone;

fetchScaleOneNoFilter:
          pp.preprocess_1x1B(__x01_y0_0);

fetchScaleOneDone:
          sse2_store4(dst, __x01_y0_0);
          dst += 4;

          if (w) continue;
          break;
        }

        P_FETCH_SPAN8_HOLE(
        {
          fx += dx * hole;
          fy += dy * hole;
        })
      P_FETCH_SPAN8_END()
    }

    // ------------------------------------------------------------------------
    // [Affine]
    // ------------------------------------------------------------------------

    else
    {
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        for (;;)
        {
          int px0 = fx >> 16;
          int py0 = fy >> 16;

          __m128i __x01_y0_0, __x01_y0_1;
          __m128i __x01_y1_0, __x01_y1_1;

          __m128i __wx0_0, __wx0_1;
          __m128i __wy0_0, __wy0_1;

          __wx0_0 = _mm_cvtsi32_si128(fx);
          __wy0_0 = _mm_cvtsi32_si128(fy);

          fx += dx;
          fy += dy;

          if (((uint)py0 >= (uint)th) | ((uint)px0 >= (uint)tw)) 
            goto fetchAffineOneBegin;

          {
            const uint8_t* src0 = srcBits + py0 * srcStride + px0 * 4;
            sse2_load8(__x01_y0_0, src0);
            sse2_load8(__x01_y1_0, src0 + srcStride);
          }

          if (--w == 0)
            goto fetchAffineOneDetect;

          __x01_y0_1 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(0, 1, 1, 0));
          __x01_y1_1 = _mm_shuffle_epi32(__x01_y1_0, _MM_SHUFFLE(1, 0, 1, 0));

          __wx0_0 = _mm_shufflelo_epi16(__wx0_0, _MM_SHUFFLE(0, 0, 0, 0));
          __wy0_0 = _mm_shufflelo_epi16(__wy0_0, _MM_SHUFFLE(0, 0, 0, 0));

          __x01_y0_1 = _mm_cmpeq_epi32(__x01_y0_1, __x01_y1_1);
          if (_mm_movemask_epi8(__x01_y0_1) == 0xFFFF)
            goto fetchAffineOneNoFilter;

          px0 = fx >> 16;
          py0 = fy >> 16;

          if (((uint)py0 >= (uint)th) | ((uint)px0 >= (uint)tw)) 
            goto fetchAffineOneContinue;

          {
            const uint8_t* src0 = srcBits + py0 * srcStride + px0 * 4;
            sse2_load8(__x01_y0_1, src0);
            sse2_load8(__x01_y1_1, src0 + srcStride);
          }

          __wx0_1 = _mm_cvtsi32_si128(fx);
          __wy0_1 = _mm_cvtsi32_si128(fy);

          __wx0_0 = _mm_shuffle_epi32(__wx0_0, _MM_SHUFFLE(1, 0, 1, 0));
          __wy0_0 = _mm_shuffle_epi32(__wy0_0, _MM_SHUFFLE(1, 0, 1, 0));

          __wx0_0 = _mm_srli_epi16(__wx0_0, 8);
          __wy0_0 = _mm_srli_epi16(__wy0_0, 8);

          __wx0_1 = _mm_shufflelo_epi16(__wx0_1, _MM_SHUFFLE(0, 0, 0, 0));
          __wy0_1 = _mm_shufflelo_epi16(__wy0_1, _MM_SHUFFLE(0, 0, 0, 0));

          __wx0_1 = _mm_shuffle_epi32(__wx0_1, _MM_SHUFFLE(1, 0, 1, 0));
          __wy0_1 = _mm_shuffle_epi32(__wy0_1, _MM_SHUFFLE(1, 0, 1, 0));

          __wx0_1 = _mm_srli_epi16(__wx0_1, 8);
          __wy0_1 = _mm_srli_epi16(__wy0_1, 8);

          sse2_unpack_1x2W(__x01_y0_0, __x01_y0_0);
          sse2_unpack_1x2W(__x01_y0_1, __x01_y0_1);
          sse2_unpack_1x2W(__x01_y1_0, __x01_y1_0);
          sse2_unpack_1x2W(__x01_y1_1, __x01_y1_1);

          pp.preprocess_2x2W(__x01_y0_0, __x01_y1_0);
          pp.preprocess_2x2W(__x01_y0_1, __x01_y1_1);

          __x01_y1_0 = _mm_mullo_epi16(__x01_y1_0, __wy0_0);
          __x01_y1_1 = _mm_mullo_epi16(__x01_y1_1, __wy0_1);
          __wy0_0 = _mm_xor_si128(__wy0_0, FOG_GET_SSE_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
          __wy0_1 = _mm_xor_si128(__wy0_1, FOG_GET_SSE_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
          __wx0_0 = _mm_xor_si128(__wx0_0, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
          __wx0_1 = _mm_xor_si128(__wx0_1, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
          __wy0_0 = _mm_add_epi16(__wy0_0, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
          __wy0_1 = _mm_add_epi16(__wy0_1, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
          __wx0_0 = _mm_add_epi16(__wx0_0, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101));
          __wx0_1 = _mm_add_epi16(__wx0_1, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101));
          __x01_y0_0 = _mm_mullo_epi16(__x01_y0_0, __wy0_0);
          __x01_y0_1 = _mm_mullo_epi16(__x01_y0_1, __wy0_1);
          __x01_y0_0 = _mm_adds_epu8(__x01_y0_0, __x01_y1_0);
          __x01_y0_1 = _mm_adds_epu8(__x01_y0_1, __x01_y1_1);
          __x01_y0_0 = _mm_srli_epi16(__x01_y0_0, 8);
          __x01_y0_1 = _mm_srli_epi16(__x01_y0_1, 8);
          __x01_y0_0 = _mm_mullo_epi16(__x01_y0_0, __wx0_0);
          __x01_y0_1 = _mm_mullo_epi16(__x01_y0_1, __wx0_1);
          __x01_y1_0 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(1, 0, 3, 2));
          __x01_y1_1 = _mm_shuffle_epi32(__x01_y0_1, _MM_SHUFFLE(1, 0, 3, 2));
          __x01_y0_0 = _mm_adds_epu8(__x01_y0_0, __x01_y1_0);
          __x01_y0_1 = _mm_adds_epu8(__x01_y0_1, __x01_y1_1);
          __x01_y0_0 = _mm_srli_epi16(__x01_y0_0, 8);
          __x01_y0_1 = _mm_srli_epi16(__x01_y0_1, 8);
          sse2_pack_2x2W(__x01_y0_0, __x01_y0_0, __x01_y0_1);
          __x01_y0_0 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(0, 0, 2, 0));

          sse2_store8(dst, __x01_y0_0);
          dst += 8;
          fx += dx;
          fy += dy;

          if (--w) continue;
          break;

fetchAffineOneBegin:
          w--;
          {
            int px1 = px0 + 1;
            int py1 = py0 + 1;
            const uint8_t* src0;

            if (px0 < 0) { px0 = px1 = 0; } else if (px0 >= tw) { px0 = px1 = tw; }
            if (py0 < 0) { py0 = py1 = 0; } else if (py0 >= th) { py0 = py1 = th; }

            src0 = srcBits + (uint)py0 * srcStride;
            sse2_load4(__x01_y0_0, src0 + px0 * 4);
            sse2_load4(__x01_y0_1, src0 + px1 * 4);

            src0 = srcBits + (uint)py1 * srcStride;
            sse2_load4(__x01_y1_0, src0 + px0 * 4);
            sse2_load4(__x01_y1_1, src0 + px1 * 4);

            __x01_y0_1 = _mm_shuffle_epi32(__x01_y0_1, _MM_SHUFFLE(3, 2, 0, 1));
            __x01_y1_1 = _mm_shuffle_epi32(__x01_y1_1, _MM_SHUFFLE(3, 2, 0, 1));

            __x01_y0_0 = _mm_or_si128(__x01_y0_0, __x01_y0_1);
            __x01_y1_0 = _mm_or_si128(__x01_y1_0, __x01_y1_1);
          }

fetchAffineOneDetect:
          __x01_y0_1 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(0, 1, 1, 0));
          __x01_y1_1 = _mm_shuffle_epi32(__x01_y1_0, _MM_SHUFFLE(1, 0, 1, 0));

          __wx0_0 = _mm_shufflelo_epi16(__wx0_0, _MM_SHUFFLE(0, 0, 0, 0));
          __wy0_0 = _mm_shufflelo_epi16(__wy0_0, _MM_SHUFFLE(0, 0, 0, 0));

          __x01_y0_1 = _mm_cmpeq_epi32(__x01_y0_1, __x01_y1_1);
          if (_mm_movemask_epi8(__x01_y0_1) == 0xFFFF)
          {
            if (PP::USE_PREPROCESS)
              goto fetchAffineOneNoFilter;
            else
              goto fetchAffineOneDone;
          }

fetchAffineOneContinue:
          __wx0_0 = _mm_shuffle_epi32(__wx0_0, _MM_SHUFFLE(1, 0, 1, 0));
          __wy0_0 = _mm_shuffle_epi32(__wy0_0, _MM_SHUFFLE(1, 0, 1, 0));

          __wx0_0 = _mm_srli_epi16(__wx0_0, 8);
          __wy0_0 = _mm_srli_epi16(__wy0_0, 8);

          sse2_unpack_1x2W(__x01_y0_0, __x01_y0_0);
          sse2_unpack_1x2W(__x01_y1_0, __x01_y1_0);

          pp.preprocess_2x2W(__x01_y0_0, __x01_y1_0);

          __x01_y1_0 = _mm_mullo_epi16(__x01_y1_0, __wy0_0);
          __wy0_0 = _mm_xor_si128(__wy0_0, FOG_GET_SSE_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
          __wx0_0 = _mm_xor_si128(__wx0_0, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
          __wy0_0 = _mm_add_epi16(__wy0_0, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
          __wx0_0 = _mm_add_epi16(__wx0_0, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101));
          __x01_y0_0 = _mm_mullo_epi16(__x01_y0_0, __wy0_0);
          __x01_y0_0 = _mm_adds_epu8(__x01_y0_0, __x01_y1_0);
          __x01_y0_0 = _mm_srli_epi16(__x01_y0_0, 8);
          __x01_y0_0 = _mm_mullo_epi16(__x01_y0_0, __wx0_0);
          __x01_y1_0 = _mm_shuffle_epi32(__x01_y0_0, _MM_SHUFFLE(1, 0, 3, 2));
          __x01_y0_0 = _mm_adds_epu8(__x01_y0_0, __x01_y1_0);
          __x01_y0_0 = _mm_srli_epi16(__x01_y0_0, 8);
          sse2_pack_1x1W(__x01_y0_0, __x01_y0_0);
          goto fetchAffineOneDone;

fetchAffineOneNoFilter:
          pp.preprocess_1x1B(__x01_y0_0);

fetchAffineOneDone:
          sse2_store4(dst, __x01_y0_0);
          dst += 4;

          if (w) continue;
          break;
        }

        P_FETCH_SPAN8_HOLE(
        {
          fx += dx * hole;
          fy += dy * hole;
        })
      P_FETCH_SPAN8_END()
    }
  }

  template<typename PP>
  static void FOG_FASTCALL fetch_trans_affine_bilinear_repeat_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    PP pp(ctx);

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx;
    int fy;
    sse2_vector_fetch_transformed(fx, fy, x, y, ctx->m);

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

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        int px0 = fx >> 16;
        int py0 = fy >> 16;

        __m128i __x01_y0;
        __m128i __x01_y1;

        __m128i __wx0;
        __m128i __wy0;

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
          goto fetchDone;
        }

        __wx0 = _mm_cvtsi32_si128(fx);
        __wy0 = _mm_cvtsi32_si128(fy);

        __wx0 = _mm_shufflelo_epi16(__wx0, _MM_SHUFFLE(0, 0, 0, 0));
        __wy0 = _mm_shufflelo_epi16(__wy0, _MM_SHUFFLE(0, 0, 0, 0));

        __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(1, 0, 1, 0));
        __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(1, 0, 1, 0));

        __wx0 = _mm_srli_epi16(__wx0, 8);
        __wy0 = _mm_srli_epi16(__wy0, 8);

        sse2_unpack_1x2W(__x01_y0, __x01_y0);
        sse2_unpack_1x2W(__x01_y1, __x01_y1);

        pp.preprocess_2x2W(__x01_y0, __x01_y1);

        __x01_y1 = _mm_mullo_epi16(__x01_y1, __wy0);
        __wy0 = _mm_xor_si128(__wy0, FOG_GET_SSE_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
        __wx0 = _mm_xor_si128(__wx0, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
        __wy0 = _mm_add_epi16(__wy0, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
        __wx0 = _mm_add_epi16(__wx0, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101));
        __x01_y0 = _mm_mullo_epi16(__x01_y0, __wy0);
        __x01_y0 = _mm_adds_epu8(__x01_y0, __x01_y1);
        __x01_y0 = _mm_srli_epi16(__x01_y0, 8);
        __x01_y0 = _mm_mullo_epi16(__x01_y0, __wx0);
        __x01_y1 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(1, 0, 3, 2));
        __x01_y0 = _mm_adds_epu8(__x01_y0, __x01_y1);
        __x01_y0 = _mm_srli_epi16(__x01_y0, 8);
        sse2_pack_1x1W(__x01_y0, __x01_y0);

fetchDone:
        sse2_store4(dst, __x01_y0);
        dst += 4;

        fx += dx;
        fy += dy;

        if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
        if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += dy * hole;

        if ((fx <= -fxmax) | (fx >= fxmax)) fx %= fxmax;
        if ((fy <= -fymax) | (fy >= fymax)) fy %= fymax;

        if (fx < 0) fx += fxmax;
        if (fy < 0) fy += fymax;
      })
    P_FETCH_SPAN8_END()
  }

  template<typename PP>
  static void FOG_FASTCALL fetch_trans_affine_bilinear_reflect_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    PP pp(ctx);

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw * 2 - 1;
    int th2 = th * 2 - 1;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx;
    int fy;
    sse2_vector_fetch_transformed(fx, fy, x, y, ctx->m);

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

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        int px0 = fx >> 16;
        int py0 = fy >> 16;

        __m128i __x01_y0;
        __m128i __x01_y1;

        __m128i __wx0;
        __m128i __wy0;

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
          goto fetchDone;
        }

        __wx0 = _mm_cvtsi32_si128(fx);
        __wy0 = _mm_cvtsi32_si128(fy);

        __wx0 = _mm_shufflelo_epi16(__wx0, _MM_SHUFFLE(0, 0, 0, 0));
        __wy0 = _mm_shufflelo_epi16(__wy0, _MM_SHUFFLE(0, 0, 0, 0));

        __wx0 = _mm_shuffle_epi32(__wx0, _MM_SHUFFLE(1, 0, 1, 0));
        __wy0 = _mm_shuffle_epi32(__wy0, _MM_SHUFFLE(1, 0, 1, 0));

        __wx0 = _mm_srli_epi16(__wx0, 8);
        __wy0 = _mm_srli_epi16(__wy0, 8);

        sse2_unpack_1x2W(__x01_y0, __x01_y0);
        sse2_unpack_1x2W(__x01_y1, __x01_y1);

        pp.preprocess_2x2W(__x01_y0, __x01_y1);

        __x01_y1 = _mm_mullo_epi16(__x01_y1, __wy0);
        __wy0 = _mm_xor_si128(__wy0, FOG_GET_SSE_CONST_PI(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF));
        __wx0 = _mm_xor_si128(__wx0, FOG_GET_SSE_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
        __wy0 = _mm_add_epi16(__wy0, FOG_GET_SSE_CONST_PI(0101010101010101_0101010101010101));
        __wx0 = _mm_add_epi16(__wx0, FOG_GET_SSE_CONST_PI(0000000000000000_0101010101010101));
        __x01_y0 = _mm_mullo_epi16(__x01_y0, __wy0);
        __x01_y0 = _mm_adds_epu8(__x01_y0, __x01_y1);
        __x01_y0 = _mm_srli_epi16(__x01_y0, 8);
        __x01_y0 = _mm_mullo_epi16(__x01_y0, __wx0);
        __x01_y1 = _mm_shuffle_epi32(__x01_y0, _MM_SHUFFLE(1, 0, 3, 2));
        __x01_y0 = _mm_adds_epu8(__x01_y0, __x01_y1);
        __x01_y0 = _mm_srli_epi16(__x01_y0, 8);
        sse2_pack_1x1W(__x01_y0, __x01_y0);

fetchDone:
        sse2_store4(dst, __x01_y0);
        dst += 4;

        fx += dx;
        fy += dy;

        if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
        if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += fy * hole;

        if ((fx <= -fxmax) | (fx >= fxmax)) fx %= fxmax;
        if ((fy <= -fymax) | (fy >= fymax)) fy %= fymax;

        if (fx < 0) fx += fxmax;
        if (fy < 0) fy += fymax;
      })
    P_FETCH_SPAN8_END()
  }
};

} // RasterEngine namespace
} // Fog namespace
