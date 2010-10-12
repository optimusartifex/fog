// [Fog-Graphics]
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
// [Fog::RasterEngine::SSE2 - PatternGradientSSE2]
// ============================================================================

//! @internal
struct FOG_HIDDEN PatternGradientSSE2
{
  // --------------------------------------------------------------------------
  // [Pattern - Gradient - Linear]
  // --------------------------------------------------------------------------

  static uint8_t* FOG_FASTCALL linear_gradient_fetch_subxy_pad(
    RasterPattern* ctx,
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

    if (ax < 0)
    {
      // Backward direction.
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
      // Forward direction.
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
    RasterPattern* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    const uint32_t* colors = (const uint32_t*)ctx->linearGradient.colors;

    // Not needed to use 64-bit integers for PATTERN_SPREAD_REPEAT, because we can
    // normalize input coordinates to always fit to this data-type.
    int yy_max = Math::doubleToFixed16x16(ctx->linearGradient.dist);
    int ax = Math::doubleToFixed16x16(ctx->linearGradient.ax);
    int yy = (int)((Math::doubleToFixed48x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay )) % yy_max);
    if (yy < 0) yy += yy_max;

    if (ax < 0)
    {
      // Backward direction.

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
      // Forward direction.

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
  // [Pattern - Gradient - Radial]
  // --------------------------------------------------------------------------

  static uint8_t* FOG_FASTCALL radial_gradient_fetch_pad(
    RasterPattern* ctx,
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

    idxf = _mm_and_ps(cc, FOG_SSE_GET_CONST_PS(7FFFFFFF7FFFFFFF_7FFFFFFF7FFFFFFF)); // abs()
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
      idxf = _mm_and_ps(cc, FOG_SSE_GET_CONST_PS(7FFFFFFF7FFFFFFF_7FFFFFFF7FFFFFFF)); // abs()
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
