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
// [Fog::RasterEngine::SSE2 - Interpolate]
// ============================================================================

//! @internal
struct FOG_HIDDEN InterpolateSSE2
{
  // --------------------------------------------------------------------------
  // [Gradient]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL gradient_argb32(uint8_t* dst, uint32_t c0, uint32_t c1, int w, int x1, int x2)
  {
    uint8_t* dstCur = dst;

    // Sanity checks.
    FOG_ASSERT(w >= 0 && x1 <= x2);

    int xw = (x2 - x1);
    if (xw == 0) return;

    // Width is decreased by 1 to fit our gradient schema that first and last
    // points in interpolation are always equal to c0 and c1 recpectively.
    if (w) w--;

    // 0op counter.
    int i;

    // Fill c0 before gradient start.
    if (x1 < 0)
    {
      i = Math::min<int>(0, x2) - x1;

      xw -= i;
      x1 = 0;

      do { set4(dstCur, c0); dstCur += 4; } while (--i);
      if (xw == 0) return;
    }

    // Fill c0 to c1 using linear interpolation.
    if (x1 < w)
    {
      __m128i xmmz;
      __m128i xmm0, xmm1;
      __m128i xmm2, xmm3;
      __m128i xmm4, xmm5;

      FOG_ALIGNED_VAR(sse2_t, tmpARGB, 16);

      xmmz = _mm_setzero_si128();
      xmm0 = _mm_cvtsi32_si128((int)c0);           // xmm0 = [            ARGB] c0
      xmm1 = _mm_cvtsi32_si128((int)c1);           // xmm1 = [            ARGB] c1

      xmm0 = _mm_unpacklo_epi8(xmm0, xmmz);        // xmm0 = [        0A0R0G0B] c0
      xmm1 = _mm_unpacklo_epi8(xmm1, xmmz);        // xmm1 = [        0A0R0G0B] c1

      xmm0 = _mm_unpacklo_epi16(xmmz, xmm0);       // xmm0 = [0A000R000G000B00] c0
      xmm1 = _mm_unpacklo_epi16(xmmz, xmm1);       // xmm1 = [0A000R000G000B00] c1

      xmm1 = _mm_sub_epi32(xmm1, xmm0);            // xmm1 = difference

      // Divide.
      tmpARGB.m128i = xmm1;                        // copy xmm1 to temporary buffer

      tmpARGB.sd[0] /= (int32_t)w;
      tmpARGB.sd[1] /= (int32_t)w;
      tmpARGB.sd[2] /= (int32_t)w;
      tmpARGB.sd[3] /= (int32_t)w;

      xmm1 = tmpARGB.m128i;                        // xmm1 = increment

      // Offset interpolation to x1.
      tmpARGB.sd[0] *= (int32_t)x1;
      tmpARGB.sd[1] *= (int32_t)x1;
      tmpARGB.sd[2] *= (int32_t)x1;
      tmpARGB.sd[3] *= (int32_t)x1;

      xmm0 = _mm_add_epi32(xmm0, tmpARGB.m128i);   // xmm0 = c0 + offset

      i = Math::min(w + 1, x2) - x1;
      x1 += i;

      // Align.
      while (((sysuint_t)dstCur & 15) != 0)
      {
        xmm2 = xmm0;                               // xmm2 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm2 = _mm_packus_epi16(xmm2, xmm2);       // xmm2 = [AxRxGxBxAxRxGxBx]
        xmm2 = _mm_srli_epi16(xmm2, 8);            // xmm2 = [0A0R0G0B0A0R0G0B]
        xmm2 = _mm_packus_epi16(xmm2, xmm2);       // xmm2 = [ARGBARGBARGBARGB]
        sse2_store4(dstCur, xmm2);

        dstCur += 4;
        if (--i == 0) goto interpolation_end;
      }

      // 0op: 4 pixels at time.
      while (i >= 4)
      {
        xmm2 = xmm0;                               // xmm2 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm3 = xmm0;                               // xmm3 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm4 = xmm0;                               // xmm4 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm5 = xmm0;                               // xmm5 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm2 = _mm_packus_epi16(xmm2, xmm3);       // xmm2 = [AxRxGxBxAxRxGxBx]
        xmm4 = _mm_packus_epi16(xmm4, xmm5);       // xmm4 = [AxRxGxBxAxRxGxBx]
        xmm2 = _mm_srli_epi16(xmm2, 8);            // xmm2 = [0A0R0G0B0A0R0G0B]
        xmm4 = _mm_srli_epi16(xmm4, 8);            // xmm4 = [0A0R0G0B0A0R0G0B]

        xmm2 = _mm_packus_epi16(xmm2, xmm4);       // xmm2 = [ARGBARGBARGBARGB]
        sse2_store16a(dstCur, xmm2);

        dstCur += 16;
        i -= 4;
      }

      // Tail.
      while (i)
      {
        xmm2 = xmm0;                               // xmm2 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm2 = _mm_packus_epi16(xmm2, xmm2);       // xmm2 = [AxRxGxBxAxRxGxBx]
        xmm2 = _mm_srli_epi16(xmm2, 8);            // xmm2 = [0A0R0G0B0A0R0G0B]
        xmm2 = _mm_packus_epi16(xmm2, xmm2);       // xmm2 = [ARGBARGBARGBARGB]
        sse2_store4(dstCur, xmm2);

        dstCur += 4;
        i--;
      }
  interpolation_end:
      if (x1 == x2) return;
    }

    // Fill c1 after gradient end.
    i = x2 - x1;
    do { set4(dstCur, c1); dstCur += 4; } while (--i);
  }

  static void FOG_FASTCALL gradient_prgb32(uint8_t* dst, uint32_t c0, uint32_t c1, int w, int x1, int x2)
  {
    if (RasterUtil::isAlpha0xFF_ARGB32(c0) &&
        RasterUtil::isAlpha0xFF_ARGB32(c1))
    {
      gradient_argb32(dst, c0, c1, w, x1, x2);
      return;
    }

    uint8_t* dstCur = dst;

    // Sanity checks.
    FOG_ASSERT(w >= 0 && x1 <= x2);

    int xw = (x2 - x1);
    if (xw == 0) return;

    // Width is decreased by 1 to fit our gradient schema that first and last
    // points in interpolation are always equal to c0 and c1 recpectively.
    if (w) w--;

    // 0op counter.
    int i;

    // Fill c0 before gradient start.
    if (x1 < 0)
    {
      uint32_t c0p = ArgbUtil::premultiply(c0);
      i = Math::min<int>(0, x2) - x1;

      xw -= i;
      x1 = 0;

      do { set4(dstCur, c0p); dstCur += 4; } while (--i);
      if (xw == 0) return;
    }

    // Fill c0 to c1 using linear interpolation.
    if (x1 < w)
    {
      __m128i xmmz;
      __m128i xmm0, xmm1;
      __m128i xmm2, xmm3;
      __m128i xmm4, xmm5;

      FOG_ALIGNED_VAR(sse2_t, tmpARGB, 16);

      xmmz = _mm_setzero_si128();
      xmm0 = _mm_cvtsi32_si128((int)c0);           // xmm0 = [            ARGB] c0
      xmm1 = _mm_cvtsi32_si128((int)c1);           // xmm1 = [            ARGB] c1

      xmm0 = _mm_unpacklo_epi8(xmm0, xmmz);        // xmm0 = [        0A0R0G0B] c0
      xmm1 = _mm_unpacklo_epi8(xmm1, xmmz);        // xmm1 = [        0A0R0G0B] c1

      xmm0 = _mm_unpacklo_epi16(xmmz, xmm0);       // xmm0 = [0A000R000G000B00] c0
      xmm1 = _mm_unpacklo_epi16(xmmz, xmm1);       // xmm1 = [0A000R000G000B00] c1

      xmm1 = _mm_sub_epi32(xmm1, xmm0);            // xmm1 = difference

      // Divide.
      tmpARGB.m128i = xmm1;                        // copy xmm1 to temporary buffer

      tmpARGB.sd[0] /= (int32_t)w;
      tmpARGB.sd[1] /= (int32_t)w;
      tmpARGB.sd[2] /= (int32_t)w;
      tmpARGB.sd[3] /= (int32_t)w;

      xmm1 = tmpARGB.m128i;                        // xmm1 = increment

      // Offset interpolation to x1.
      tmpARGB.sd[0] *= (int32_t)x1;
      tmpARGB.sd[1] *= (int32_t)x1;
      tmpARGB.sd[2] *= (int32_t)x1;
      tmpARGB.sd[3] *= (int32_t)x1;

      xmm0 = _mm_add_epi32(xmm0, tmpARGB.m128i);   // xmm0 = c0 + offset

      i = Math::min(w + 1, x2) - x1;
      x1 += i;

      // Align.
      while (((sysuint_t)dstCur & 15) != 0)
      {
        xmm2 = xmm0;                               // xmm2 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm2 = _mm_packus_epi16(xmm2, xmm2);       // xmm2 = [AxRxGxBxAxRxGxBx]
        xmm2 = _mm_srli_epi16(xmm2, 8);            // xmm2 = [0A0R0G0B0A0R0G0B]
        sse2_premultiply_1x1W(xmm2, xmm2);
        xmm2 = _mm_packus_epi16(xmm2, xmm2);       // xmm2 = [ARGBARGBARGBARGB]
        sse2_store4(dstCur, xmm2);

        dstCur += 4;
        if (--i == 0) goto interpolation_end;
      }

      // 0op: 4 pixels at time.
      while (i >= 4)
      {
        xmm2 = xmm0;                               // xmm2 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm3 = xmm0;                               // xmm3 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm4 = xmm0;                               // xmm4 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm5 = xmm0;                               // xmm5 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm2 = _mm_packus_epi16(xmm2, xmm3);       // xmm2 = [AxRxGxBxAxRxGxBx]
        xmm4 = _mm_packus_epi16(xmm4, xmm5);       // xmm4 = [AxRxGxBxAxRxGxBx]
        xmm2 = _mm_srli_epi16(xmm2, 8);            // xmm2 = [0A0R0G0B0A0R0G0B]
        xmm4 = _mm_srli_epi16(xmm4, 8);            // xmm4 = [0A0R0G0B0A0R0G0B]
        sse2_premultiply_2x2W(xmm2, xmm2, xmm4, xmm4);
        xmm2 = _mm_packus_epi16(xmm2, xmm4);       // xmm2 = [ARGBARGBARGBARGB]
        sse2_store16a(dstCur, xmm2);

        dstCur += 16;
        i -= 4;
      }

      // Tail.
      while (i)
      {
        xmm2 = xmm0;                               // xmm2 = [xAxxxRxxxGxxxBxx]
        xmm0 = _mm_add_epi32(xmm0, xmm1);          // xmm0 += xmm1

        xmm2 = _mm_packus_epi16(xmm2, xmm2);       // xmm2 = [AxRxGxBxAxRxGxBx]
        xmm2 = _mm_srli_epi16(xmm2, 8);            // xmm2 = [0A0R0G0B0A0R0G0B]
        sse2_premultiply_1x1W(xmm2, xmm2);
        xmm2 = _mm_packus_epi16(xmm2, xmm2);       // xmm2 = [ARGBARGBARGBARGB]
        sse2_store4(dstCur, xmm2);

        dstCur += 4;
        i--;
      }

interpolation_end:
      if (x1 == x2) return;
    }

    // Fill c1 after gradient end.
    {
      uint32_t c1p = ArgbUtil::premultiply(c1);
      i = x2 - x1;
      do { set4(dstCur, c1p); dstCur += 4; } while (--i);
    }
  }
};

} // RasterEngine namespace
} // Fog namespace
