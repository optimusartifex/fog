// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Pattern - Gradient]
// ============================================================================

#define MAX_INTERPOLATION_POINTS 4096

// For antialiasing tests use 16 points.
// #undef MAX_INTERPOLATION_POINTS
// #define MAX_INTERPOLATION_POINTS 16

//! @internal
struct FOG_HIDDEN PatternGradientC
{
  // --------------------------------------------------------------------------
  // [Pattern - Gradient - Generic]
  // --------------------------------------------------------------------------

  // C++ Implementation Notes:
  //
  // We are using gradient-lut table that contains all interpolated colors in
  // gradient. If the table is created it's easy to make linear / radial or
  // conical gradient simply calculating the formula and using this array[].
  // Calculation is normally done in 48.16 fixed point, may be scaled to 16.16
  // fixed point if it can fit into it.
  //
  // Large gradients and antialiasing:
  //
  // Antialiasing is a bit problem. If the gradient-lut table length is same as
  // length of linear / radial gradient then we can use (('yy' >> 8) & 0xFF)
  // formula to get correct interpolation weight, but if we scaled the
  // gradient-lut table (because without scaling it could be too big) then we
  // can no longer use this formula.
  //
  // If gradient is too large then we are using gradient scaling. This means
  // generating smaller gradient lut table and modifying calculated values to
  // fit into this smaller table. Problem is that when using antialiasing the
  // weight that is normally computed as ('yy' >> 8 & 0xFF) is no longer valid
  // (it's scaled, you just can't use this value).

  static err_t FOG_FASTCALL init_generic(
    RasterPattern* ctx, const List<ArgbStop>& stops, sysint_t gLength, uint32_t spread)
  {
    // Never initialize this if there are no stops or only single one (solid color).
    FOG_ASSERT(stops.getLength() >= 2);
    FOG_ASSERT(spread < PATTERN_SPREAD_COUNT);

    bool hasAlpha = (spread == PATTERN_SPREAD_NONE) 
      ? true
      : ColorAnalyzer::analyzeAlpha(stops) != 0xFF;

    // Alloc twice memory for reflect spread.
    sysint_t gAlloc = gLength;
    if (spread == PATTERN_SPREAD_REFLECT) gAlloc <<= 1;

    sysint_t gMyLen = gAlloc;

    // Alloc two pixels more for interpolation (one at the beginning and one at the end).
    gAlloc += 2;

    // alloc space for pattern or use reserved buffer.
    if ((ctx->genericGradient.colors = (uint32_t*)Memory::alloc(gAlloc * sizeof(uint32_t))) == NULL)
    {
      return ERR_RT_OUT_OF_MEMORY;
    }

    // Adjust colors pointer, colors[-1] is used by interpolation. We are doing
    // this to remove useless checks in inner loops.
    ctx->genericGradient.colors += 1;

    ctx->format = hasAlpha ? IMAGE_FORMAT_PRGB32 : IMAGE_FORMAT_XRGB32;
    ctx->depth = 32;
    ctx->bytesPerPixel = 4;

    calc_stops(
      (uint8_t*)ctx->genericGradient.colors, stops,
      rasterFuncs.interpolate.gradient[ctx->format],
      0, (int)gLength, (int)gLength, false);

    ctx->genericGradient.colorsAlloc = gAlloc;
    ctx->genericGradient.colorsLength = gMyLen;

    // Create mirror for reflect spread.
    if (spread == PATTERN_SPREAD_REFLECT)
    {
      uint32_t* patternTo = ctx->genericGradient.colors + gLength;
      uint32_t* patternFrom = patternTo - 1;

      size_t i;
      for (i = (size_t)gLength; i; i--, patternTo++, patternFrom--)
      {
        *patternTo = *patternFrom;
      }
    }

    // First and last colors in gradient lut table (for interpolation).
    uint32_t cFirst = 0x00000000;
    uint32_t cLast = 0x00000000;

    if (spread != PATTERN_SPREAD_NONE)
    {
      cFirst = stops.at(0).getArgb();
      cLast = (spread != PATTERN_SPREAD_PAD) ? cFirst : (uint32_t)stops.top().getArgb();

      // Premultiply.
      if (hasAlpha)
      {
        cFirst = ColorUtil::premultiply(cFirst);
        cLast = ColorUtil::premultiply(cLast);
      }
    }

    ctx->genericGradient.colors[-1] = cFirst;
    ctx->genericGradient.colors[gMyLen] = cLast;

    return ERR_OK;
  }

  static void FOG_FASTCALL destroy_generic(
    RasterPattern* ctx)
  {
    FOG_ASSERT(ctx->initialized);

    Memory::free(ctx->linearGradient.colors - 1);
    ctx->initialized = false;
  }

  static void FOG_FASTCALL calc_stops(
    uint8_t* dst, const List<ArgbStop>& stops,
    RasterInterpolateArgbFn gradientSpan,
    int offset, int size, int w,
    bool reverse)
  {
    // Sanity check.
    FOG_ASSERT(w <= size || offset <= (size - w));

    sysint_t count = (sysint_t)stops.getLength();
    sysint_t end = offset + w;

    if (count == 0 || w == 0) return;

    if (count == 1 || size == 1)
    {
      Argb color = stops.at(0).getArgb();
      gradientSpan(dst, color, color, size, 0, size);
    }
    else
    {
      sysint_t i = (reverse) ? count - 1 : 0;
      sysint_t iinc = (reverse) ? -1 : 1;

      Argb primaryStopColor = stops.at(i).getArgb();
      Argb secondaryStopArgb;

      float primaryStopOffset = 0.0;
      float secondaryStopOffset;
      sysint_t x1 = 0;
      sysint_t x2 = 0;

      for (; i < count && (sysint_t)x1 < end; i += iinc,
        primaryStopOffset = secondaryStopOffset,
        primaryStopColor = secondaryStopArgb,
        x1 = x2)
      {
        secondaryStopOffset = stops.at(i).getOffset();
        secondaryStopArgb = stops.at(i).getArgb();

        // Stop offset can be at range from 0.0 to 1.0 including.
        if (secondaryStopOffset < 0.0f) secondaryStopOffset = 0.0f;
        if (secondaryStopOffset > 1.0f) secondaryStopOffset = 1.0f;
        if (reverse) secondaryStopOffset = 1.0f - secondaryStopOffset;

        // Don't trust input data...
        if (secondaryStopOffset < primaryStopOffset) return;

        // Skip all siblings and the first one.
        if (secondaryStopOffset == primaryStopOffset) continue;

        // get pixel coordinates and skip that caller not wants
        x2 = (sysint_t)((float)size * secondaryStopOffset);
        if (x2 < (sysint_t)offset) continue; // not reached the beggining
        if (x2 > (sysint_t)size) return;     // reached the end

        sysint_t cx1 = x1; if (cx1 < (sysint_t)offset) cx1 = (sysint_t)offset;
        sysint_t cx2 = x2; if (cx2 > (sysint_t)end) cx2 = (sysint_t)end;

        if (cx2 - cx1)
        {
          gradientSpan(
            // pointer to destination, it's needed to decrease it by 'offset'
            dst + (sysint_t)(cx1 - offset) * 4,
            // primary and secondary colors
            primaryStopColor, secondaryStopArgb,
            // width, x1, x2
            cx2 - cx1, cx1 - x1, x2 - x1);
        }
      }

      // TODO: draw last point
      // if (size == width) ((uint32_t*)dst)[size-1] = secondaryStopArgb;
    }
  }

  // --------------------------------------------------------------------------
  // [Pattern - Gradient - Linear]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL init_linear(
    RasterPattern* ctx, const Pattern& pattern, const DoubleMatrix& matrix, uint32_t interpolationType)
  {
    PatternData* d = pattern._d;
    FOG_ASSERT(d->type == PATTERN_TYPE_LINEAR_GRADIENT);

    if (d->obj.stops->getLength() == 0)
    {
      return rasterFuncs.pattern.solid_init(ctx, 0x00000000);
    }
    if (d->obj.stops->getLength() == 1)
    {
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(0).getArgb());
    }

    // If points are equal, we will fill everything by last color. This is
    // defined in SVG.
    if (Math::feq(d->data.gradient->points[0].x, d->data.gradient->points[1].x) &&
        Math::feq(d->data.gradient->points[0].y, d->data.gradient->points[1].y))
    {
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(d->obj.stops->getLength() - 1).getArgb());
    }

    // FIXME: TODO: Not correct code
#if 0
    DoubleMatrix m(pattern._d->matrix.multiplied(matrix));
    DoublePoint points[2];

    double f = (m.sx * m.sy) - (m.shy * m.shx);
    if (fabs(f) < 1e-20)
    {
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(0).rgba);
    }

    f = 1.0 / f;

    points[0].x = (( (m.sy  * d->points[0].x) - (m.shy * d->points[0].y)) * f);
    points[0].y = ((-(m.shx * d->points[0].x) + (m.sx  * d->points[0].y)) * f);

    points[1].x = (( (m.sy  * d->points[1].x) - (m.shy * d->points[1].y)) * f);
    points[1].y = ((-(m.shx * d->points[1].x) + (m.sx  * d->points[1].y)) * f);

    double dx = points[1].x - points[0].x;
    double dy = points[1].y - points[0].y;
    double dx2dy2 = dx * dx + dy * dy;
    double sqrtxxyy = Math::sqrt(dx2dy2);

    if (Math::abs(dx) < 0.000001 && Math::abs(dy) < 0.000001)
    {
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(0).rgba);
    }

    sysint_t gLength = (int)(sqrtxxyy + 0.5);
    if (gLength < 1) gLength = 1;
    if (gLength > MAX_INTERPOLATION_POINTS) gLength = MAX_INTERPOLATION_POINTS;

    double scale = (double)gLength / (dx2dy2);

    // Set dx/dy, adjust it by 0.5 (subpixel) to avoid this adjusting in the
    // blitter.
    ctx->linearGradient.dx = points[0].x + m.tx - 0.5;
    ctx->linearGradient.dy = points[0].y + m.ty - 0.5;

    ctx->linearGradient.ax = dx * scale;
    ctx->linearGradient.ay = dy * scale;

    ctx->linearGradient.dist = (double)gLength;
    if (d->spread == PATTERN_SPREAD_REFLECT) ctx->linearGradient.dist *= 2.0;
#endif


#if 1
    DoubleMatrix m(pattern._d->matrix.multiplied(matrix));
    DoublePoint pts[2];

    double px = d->data.gradient->points[0].x;
    double py = d->data.gradient->points[0].y;

    m.transformPoints(pts, d->data.gradient->points, 2);

    double vx = d->data.gradient->points[1].x - d->data.gradient->points[0].x;
    double vy = d->data.gradient->points[1].y - d->data.gradient->points[0].y;

    double wx = pts[1].x - pts[0].x;
    double wy = pts[1].y - pts[0].y;

    double dx = ( (m.sy  * vx) - (m.shy * vy));
    double dy = (-(m.shx * vx) + (m.sx  * vy));

    //double dx2dy2 = dx * dx + dy * dy;
    double dx2dy2 = dx * dx + dy * dy;
    double sqrtxxyy = Math::sqrt(dx2dy2);

    if (Math::abs(dx) < 0.000001 && Math::abs(dy) < 0.000001)
    {
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(0).getArgb());
    }

    sysint_t gLength = (int)(sqrtxxyy + 0.5);
    if (gLength < 1) gLength = 1;
    if (gLength > MAX_INTERPOLATION_POINTS) gLength = MAX_INTERPOLATION_POINTS;

    double scale = (double)gLength / (dx2dy2);

    // Set dx/dy, adjust it by 0.5 (subpixel) to avoid this adjusting in the
    // blitter.
    ctx->linearGradient.dx = pts[0].x - 0.5;
    ctx->linearGradient.dy = pts[0].y - 0.5;

    ctx->linearGradient.ax = (dx) * scale;
    ctx->linearGradient.ay = (dy) * scale;

    ctx->linearGradient.dist = (double)gLength;
    if (d->spread == PATTERN_SPREAD_REFLECT) ctx->linearGradient.dist *= 2.0;
#endif



#if 0
    DoubleMatrix m(pattern._d->matrix.multiplied(matrix));

    double px = d->points[0].x;
    double py = d->points[0].y;

    m.transformPoint(&px, &py);

    double vx = d->points[1].x - d->points[0].x;
    double vy = d->points[1].y - d->points[0].y;

    double wx = vx * m.sx + vy * m.shx;
    double wy = vy * m.sy - vx * m.shy;

    double dx = wx;// + vy * m.shx;
    double dy = wy;// - vx * m.shy;

    double sh = m.shx + m.shy;

    double wx2wy2 = wx * wx + wy * wy;
    double sqrtwx2wy2 = Math::sqrt(wx2wy2);

    if (Math::abs(wx) < 0.000001 && Math::abs(wy) < 0.000001)
    {
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(0).rgba);
    }

    sysint_t gLength = (int)(sqrtwx2wy2 + 0.5);
    if (gLength < 1) gLength = 1;
    if (gLength > MAX_INTERPOLATION_POINTS) gLength = MAX_INTERPOLATION_POINTS;

    double scale = (double)gLength / wx2wy2;

    // Set dx/dy, adjust it by 0.5 (subpixel) to avoid this adjusting in the blitter.
    ctx->linearGradient.dx = px - 0.5;
    ctx->linearGradient.dy = py - 0.5;

    ctx->linearGradient.ax = (wx) * scale - wy * (m.shy) * m.sx * scale;
    ctx->linearGradient.ay = (wy) * scale - wx * (m.shx) * m.sy * scale;

    ctx->linearGradient.dist = (double)gLength;
    if (d->spread == PATTERN_SPREAD_REFLECT) ctx->linearGradient.dist *= 2.0;

#endif
/*
    fog_debug("[sx=%g shy=%g]\n[shx=%g sy=%g]", m.sx, m.shy, m.shx, m.sy);
    fog_debug("  [ax=%g, ay=%g] [dist=%g]", 
      ctx->linearGradient.ax,
      ctx->linearGradient.ay,
      ctx->linearGradient.dist);
    fog_debug("  [DX=%g DY=%g]", dx, dy);
    fog_debug("  [WX=%g WY=%g]", wx, wy);

    fog_debug("-");
*/
    err_t err = init_generic(ctx, d->obj.stops.instance(), gLength, d->spread);
    if (err) return err;

    // Whether gradient definition is exact (no antialiasing, very fast).
    bool exact = false;
    if (interpolationType == COLOR_INTERPOLATION_NEAREST) exact = true;
    /*
    {
      // Use exact rendering if points are aligned and gradient is vertical or horizontal.
      int ep0x = (int)(points[0].x * 256.0) & 0xFF;
      int ep0y = (int)(points[0].y * 256.0) & 0xFF;
      int ep1x = (int)(points[1].x * 256.0) & 0xFF;
      int ep1y = (int)(points[1].y * 256.0) & 0xFF;

      int eax = (int)(ctx->linearGradient.ax * 65536.0) & 0xFFFF;
      int eay = (int)(ctx->linearGradient.ay * 65536.0) & 0xFFFF;

      // fog_debug("Fog::RasterEngine::LinearGradient - [x0=%d y0=%d] -> [x1=%d y1=%d] :: [%d %d]", ep0x, ep0y, ep1x, ep1y, eax, eay);
      if (ep0x == 128 && ep0y == 128 && ep1x == 128 && ep1y == 128 && (eax == 0 || eay == 0))
      {
        // fog_debug("Fog::RasterEngine::LinearGradient - Using exact...\n");
        exact = true;
      }
    }
    */

    // Set fetch function.
    if (exact)
      ctx->fetch = rasterFuncs.pattern.linear_gradient_fetch_exact[d->spread];
    else
      ctx->fetch = rasterFuncs.pattern.linear_gradient_fetch_subxy[d->spread];

    // Set destroy function.
    ctx->destroy = destroy_generic;

    ctx->initialized = true;
    return ERR_OK;
  }

  static void FOG_FASTCALL fetch_linear_exact_pad(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->linearGradient.colors;
    sysint_t colorsLength = ctx->linearGradient.colorsLength;

    int ax = Math::doubleToFixed16x16(ctx->linearGradient.ax);
    int yy = Math::doubleToFixed16x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay );
    int yy_max = Math::doubleToFixed16x16(ctx->linearGradient.dist);

    uint32_t color0 = colors[-1];
    uint32_t color1 = colors[colorsLength];

    if (ax < 0)
    {
      // Backward direction.
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        while (yy >= yy_max)
        {
          ((uint32_t*)dst)[0] = color1;

          dst += 4;
          yy += ax;
          if (--w == 0) goto fetchBackwardSkip;
        }

        while (yy >= 0)
        {
          ((uint32_t*)dst)[0] = colors[(sysint_t)(yy >> 16)];

          dst += 4;
          yy += ax;
          if (--w == 0) goto fetchBackwardSkip;
        }

        goto fetchSolidLoop;

fetchBackwardSkip:
        P_FETCH_SPAN8_HOLE(
        {
          yy += ax * hole;
        })
      P_FETCH_SPAN8_END()
      return;
    }
    else if (ax > 0)
    {
      // Forward direction.
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        while (yy <= 0)
        {
          ((uint32_t*)dst)[0] = color0;

          dst += 4;
          yy += ax;
          if (--w == 0) goto fetchForwardSkip;
        }

        while (yy < yy_max)
        {
          ((uint32_t*)dst)[0] = colors[(sysint_t)(yy >> 16)];

          dst += 4;
          yy += ax;
          if (--w == 0) goto fetchForwardSkip;
        }

        color0 = color1;
        goto fetchSolidLoop;

fetchForwardSkip:
        P_FETCH_SPAN8_HOLE(
        {
          yy += ax * hole;
        })
      P_FETCH_SPAN8_END()
      return;
    }
    else
    {
      if (yy > yy_max)
        color0 = color1;
      else if (yy >= 0)
        color0 = colors[(sysint_t)(yy >> 16)];
    }

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = color0;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  static void FOG_FASTCALL fetch_linear_exact_repeat(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->linearGradient.colors;

    int yy_max = Math::doubleToFixed16x16(ctx->linearGradient.dist);
    int ax = Math::doubleToFixed16x16(ctx->linearGradient.ax);
    int yy = (int)((Math::doubleToFixed16x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay )) % yy_max);
    if (yy < 0) yy += yy_max;

    if (ax < 0)
    {
      // Backward direction.
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        do {
          ((uint32_t*)dst)[0] = colors[yy >> 16];

          dst += 4;
          yy += ax;
          if (yy < 0) yy += yy_max;
        } while (--w);

        P_FETCH_SPAN8_HOLE(
        {
          yy = (int)(yy + ax * hole) % yy_max;
          if (yy < 0) yy += yy_max;
        })
      P_FETCH_SPAN8_END()
    }
    else if (ax > 0)
    {
      // Forward direction.
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        do {
          ((uint32_t*)dst)[0] = colors[yy >> 16];

          dst += 4;
          yy += ax;
          if (yy >= yy_max) yy -= yy_max;
        } while (--w);

        P_FETCH_SPAN8_HOLE(
        {
          yy = (int)(yy + (int64_t)ax * hole) % yy_max;
        })
      P_FETCH_SPAN8_END()
    }
    else
    {
      // Horizontal line.
      uint32_t c0 = colors[yy >> 16];
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()
        PatternCoreC::fill32(dst, c0, w);
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
  }

  static void FOG_FASTCALL fetch_linear_subxy_pad(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->linearGradient.colors;
    sysint_t colorsLength = ctx->linearGradient.colorsLength;

    int ax = Math::doubleToFixed16x16(ctx->linearGradient.ax);
    int yy = Math::doubleToFixed16x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay );
    int yy_max = Math::doubleToFixed16x16(ctx->linearGradient.dist);

    uint32_t color0 = colors[-1];
    uint32_t color1 = colors[colorsLength];

    if (ax < 0)
    {
      // Backward direction.
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        while (yy >= yy_max)
        {
          ((uint32_t*)dst)[0] = color1;

          dst += 4;
          yy += ax;
          if (!(--w)) goto fetchBackwardNext;
        }

        while (yy >= -(FIXED_16x16_ONE - 1))
        {
          int pos = (yy >> 16);
          uint weight = (yy >> 8) & 0xFF;
          C_PATTERN_INTERPOLATE_32_2(
            ((uint32_t*)dst)[0],
            colors[pos    ], 256 - weight,
            colors[pos + 1], weight);

          dst += 4;
          yy += ax;
          if (!(--w)) goto fetchBackwardNext;
        }

        goto fetchSolidLoop;

fetchBackwardNext:
        P_FETCH_SPAN8_HOLE(
        {
          yy += ax * hole;
        })
      P_FETCH_SPAN8_END()
      return;
    }
    else if (ax > 0)
    {
      // Forward direction.
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        while (yy < -(FIXED_16x16_ONE - 1))
        {
          ((uint32_t*)dst)[0] = color0;

          dst += 4;
          yy += ax;
          if (!(--w)) goto fetchForwardNext;
        }

        while (yy < yy_max)
        {
          int pos = (yy >> 16);
          uint weight = (yy >> 8) & 0xFF;
          C_PATTERN_INTERPOLATE_32_2(
            ((uint32_t*)dst)[0],
            colors[pos    ], 256 - weight,
            colors[pos + 1], weight);

          dst += 4;
          yy += ax;
          if (!(--w)) goto fetchForwardNext;
        }

        color0 = color1;
        goto fetchSolidLoop;

fetchForwardNext:
        P_FETCH_SPAN8_HOLE(
        {
          yy += ax * hole;
        })
      P_FETCH_SPAN8_END()
      return;
    }
    else
    {
      // Horizontal line.
      if (yy > -(FIXED_16x16_ONE - 1))
      {
        if (yy < yy_max)
        {
          int pos = (yy >> 16);
          uint weight = (yy >> 8) & 0xFF;
          C_PATTERN_INTERPOLATE_32_2(
            color0,
            colors[pos    ], 256 - weight,
            colors[pos + 1], weight);
        }
        else
        {
          color0 = color1;
        }
      }
      // ... continue to fetch solid.
    }

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = color0;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  static void FOG_FASTCALL fetch_linear_subxy_repeat(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->linearGradient.colors;

    int yy_max = Math::doubleToFixed16x16(ctx->linearGradient.dist);
    int ax = Math::doubleToFixed16x16(ctx->linearGradient.ax);
    int yy = (int)((Math::doubleToFixed48x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay )) % yy_max);
    if (yy < 0) yy += yy_max;

    if (ax < 0)
    {
      // Backward direction.
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        do {
          int pos = (yy >> 16);
          uint weight = (yy >> 8) & 0xFF;
          C_PATTERN_INTERPOLATE_32_2(
            ((uint32_t*)dst)[0],
            colors[pos    ], 256 - weight,
            colors[pos + 1], weight);

          dst += 4;
          yy += ax;
          if (yy < 0) yy += yy_max;
          if (!(--w)) goto fetchBackwardNext;
        } while (true);

fetchBackwardNext:
        P_FETCH_SPAN8_HOLE(
        {
          yy += ax * hole;
          if (yy < 0) yy = (yy % yy_max) + yy_max;
        })
      P_FETCH_SPAN8_END()
    }
    else
    {
      // Forward direction.
      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT()

        do {
          int pos = (yy >> 16);
          uint weight = (yy >> 8) & 0xFF;
          C_PATTERN_INTERPOLATE_32_2(
            ((uint32_t*)dst)[0],
            colors[pos    ], 256 - weight,
            colors[pos + 1], weight);

          dst += 4;
          yy += ax;
          if (yy >= yy_max) yy -= yy_max;
          if (!(--w)) goto fetchForwardNext;
        } while (true);

fetchForwardNext:
        P_FETCH_SPAN8_HOLE(
        {
          yy += ax * hole;
          if (yy >= yy_max) yy %= yy_max;
        })
      P_FETCH_SPAN8_END()
    }
  }

  // --------------------------------------------------------------------------
  // [Pattern - Gradient - Radial]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL init_radial(
    RasterPattern* ctx, const Pattern& pattern, const DoubleMatrix& matrix, uint32_t interpolationType)
  {
    PatternData* d = pattern._d;
    FOG_ASSERT(d->type == PATTERN_TYPE_RADIAL_GRADIENT);

    if (d->obj.stops->getLength() == 0)
      return rasterFuncs.pattern.solid_init(ctx, 0x00000000);

    if (d->obj.stops->getLength() == 1)
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(0).getArgb());

    DoubleMatrix m(pattern._d->matrix.multiplied(matrix));

    DoublePoint points[2];
    m.transformPoints(points, d->data.gradient->points, 2);

    sysint_t gLength = 256 * d->obj.stops->getLength();
    if (gLength > 4096) gLength = 4096;
    sysint_t gAlloc = gLength;

    // This calculation is based on AntiGrain 2.4 <www.antigrain.com>
    // ----------------------------------------------------------------
    // Calculate the invariant values. In case the focal center
    // lies exactly on the gradient circle the divisor degenerates
    // into zero. In this case we just move the focal center by
    // one subpixel unit possibly in the direction to the origin (0,0)
    // and calculate the values again.
    //-----------------------------------------------------------------
    double dx , dy;
    double fx , fy;
    double fx2, fy2;
    double r  , r2;
    double dd;

    dx = points[1].x;
    dy = points[1].y;
    fx = points[1].x - points[0].x;
    fy = points[1].y - points[0].y;
    r = d->data.gradient->radius;

    r2 = r  * r;
    fx2 = fx * fx;
    fy2 = fy * fy;
    dd = r2 - (fx2 + fy2);

    // Here is what we talked on the top (divisor degenerated to zero).
    if (dd == 0.0)
    {
      if (fx) fx += (fx < 0.0) ? 1.0 : -1.0;
      if (fy) fy += (fy < 0.0) ? 1.0 : -1.0;

      fx2 = fx * fx;
      fy2 = fy * fy;
      dd = r2 - (fx2 + fy2);
    }

    // Alloc twice memory for reflect spread.
    gAlloc = gLength;
    if (d->spread == PATTERN_SPREAD_REFLECT) gAlloc <<= 1;

    ctx->radialGradient.mul = (double)gLength / dd;

    err_t err = init_generic(ctx, d->obj.stops.instance(), gLength, d->spread);
    if (err) return err;

    ctx->radialGradient.dx  = dx;
    ctx->radialGradient.dy  = dy;
    ctx->radialGradient.fx  = fx;
    ctx->radialGradient.fy  = fy;
    ctx->radialGradient.fx2 = fx2;
    ctx->radialGradient.fy2 = fy2;
    ctx->radialGradient.r   = r;
    ctx->radialGradient.r2  = r2;

    // Set fetch function.
    ctx->fetch = rasterFuncs.pattern.radial_gradient_fetch[d->spread];

    // Set destroy function.
    ctx->destroy = destroy_generic;

    ctx->initialized = true;
    return ERR_OK;
  }

  /*

  This is reference implementation from AntiGrain 2.4.

  static void FOG_FASTCALL radial_gradient_fetch_pad(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    uint8_t* dst = dst;

    const uint32_t* colors = (const uint32_t*)ctx->radialGradient.colors;
    sysint_t colorsLength = ctx->radialGradient.colorsLength;

    uint32_t color0 = colors[0];
    uint32_t color1 = colors[colorsLength-1];

    int index;

    double dx = (double)x - ctx->radialGradient.dx;
    double dy = (double)y - ctx->radialGradient.dy;

    double fx = ctx->radialGradient.fx;
    double fy = ctx->radialGradient.fy;
    double r2 = ctx->radialGradient.r2;
    double scale = ctx->radialGradient.mul;

    double dyfx = dy * fx;
    double dyfy = dy * dy;
    double dydy = dy * dy;

    do {
      double d2 = dx * fy - dyfx;
      double d3 = r2 * (dx * dx + dydy) - d2 * d2;

      index = (int) ((dx * fx + dyfy + Math::sqrt(fabs(d3))) * scale);

      if (index < 0)
        ((uint32_t*)dst)[0] = color0;
      else if (index >= colorsLength)
        ((uint32_t*)dst)[0] = color1;
      else
        ((uint32_t*)dst)[0] = colors[index];

      dst += 4;
      dx += 1.0;
    } while (--w);

    return dst;
  }

  */

  static void FOG_FASTCALL fetch_radial_pad(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->radialGradient.colors;
    int colorsLength = ctx->radialGradient.colorsLength;

    uint32_t color0 = colors[0];
    uint32_t color1 = colors[colorsLength];

    int index;

    double dx = (double)x - ctx->radialGradient.dx;
    double dy = (double)y - ctx->radialGradient.dy;

    double fx = ctx->radialGradient.fx;
    double fy = ctx->radialGradient.fy;
    double r2 = ctx->radialGradient.r2;
    double scale = ctx->radialGradient.mul;

    double dyfx = dy * fx;
    double dyfy = dy * fy;
    double dydy = dy * dy;

    double dxdx = dx * dx;
    double dxfx = dx * fx;
    double dxfy = dx * fy;

    double dxfx_p_dyfy = dxfx + dyfy;
    double dxfy_m_dyfx = dxfy - dyfx;

    double cc = (dydy + dxdx) * r2 - (dxfy_m_dyfx * dxfy_m_dyfx);
    double cx = (dx * r2)          - (dxfy_m_dyfx * fy);
    double ci = r2                 - (fy * fy);

    double dd = (dxfx_p_dyfy);
    double di = fx;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        index = (int)((dd + Math::sqrt(fabs(cc))) * scale);

        if (index < 0)
          ((uint32_t*)dst)[0] = color0;
        else if (index >= colorsLength)
          ((uint32_t*)dst)[0] = color1;
        else
          ((uint32_t*)dst)[0] = colors[index];
        dst += 4;

        // cc += cx + cx + ci
        // cx += ci
        cc += cx;
        cx += ci;
        cc += cx;

        dd += di;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        cc += (cx + cx + ci * hole) * hole;
        cx += ci * hole;

        dd += di * hole;
      })
    P_FETCH_SPAN8_END()
  }

  static void FOG_FASTCALL fetch_radial_repeat(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->radialGradient.colors;
    sysint_t colorsLength = ctx->radialGradient.colorsLength;

    uint32_t color0 = colors[0];
    uint32_t color1 = colors[colorsLength-1];

    int index;

    double dx = (double)x - ctx->radialGradient.dx;
    double dy = (double)y - ctx->radialGradient.dy;

    double fx = ctx->radialGradient.fx;
    double fy = ctx->radialGradient.fy;
    double r2 = ctx->radialGradient.r2;
    double scale = ctx->radialGradient.mul;

    double dyfx = dy * fx;
    double dyfy = dy * fy;
    double dydy = dy * dy;

    double dxdx = dx * dx;
    double dxfx = dx * fx;
    double dxfy = dx * fy;

    double dxfx_p_dyfy = dxfx + dyfy;
    double dxfy_m_dyfx = dxfy - dyfx;

    double cc = (dydy + dxdx) * r2 - (dxfy_m_dyfx * dxfy_m_dyfx);
    double cx = (dx * r2)          - (dxfy_m_dyfx * fy);
    double ci = r2                 - (fy * fy);

    double dd = (dxfx_p_dyfy);
    double di = fx;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        index = (int)((dd + Math::sqrt(fabs(cc))) * scale) % colorsLength;
        if (index < 0) index += colorsLength;

        ((uint32_t*)dst)[0] = colors[index];
        dst += 4;

        // cc += cx + cx + ci
        // cx += ci
        cc += cx;
        cx += ci;
        cc += cx;

        dd += di;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        cc += (cx + cx + ci * hole) * hole;
        cx += ci * hole;

        dd += di * hole;
      })
    P_FETCH_SPAN8_END()
  }

  // --------------------------------------------------------------------------
  // [Pattern - Gradient - Conical]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL init_conical(
    RasterPattern* ctx, const Pattern& pattern, const DoubleMatrix& matrix, uint32_t interpolationType)
  {
    PatternData* d = pattern._d;
    FOG_ASSERT(d->type == PATTERN_TYPE_CONICAL_GRADIENT);

    if (d->obj.stops->getLength() == 0)
    {
      return rasterFuncs.pattern.solid_init(ctx, 0x00000000);
    }
    if (d->obj.stops->getLength() == 1)
    {
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(0).getArgb());
    }

    DoubleMatrix m(pattern._d->matrix.multiplied(matrix));

    DoublePoint points[2];
    m.transformPoints(points, d->data.gradient->points, 2);

    sysint_t gLength = 256 * d->obj.stops->getLength();
    if (gLength > 4096) gLength = 4096;

    ctx->conicalGradient.dx = points[0].x;
    ctx->conicalGradient.dy = points[0].y;
    ctx->conicalGradient.angle = atan2(
      (points[0].x - points[1].x),
      (points[0].y - points[1].y)) + (M_PI/2.0);

    err_t err = init_generic(ctx, d->obj.stops.instance(), gLength, d->spread);
    if (err) return err;

    // Set fetch function.
    ctx->fetch = rasterFuncs.pattern.conical_gradient_fetch;

    // Set destroy function.
    ctx->destroy = destroy_generic;

    ctx->initialized = true;
    return ERR_OK;
  }

  static void FOG_FASTCALL fetch_conical(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->radialGradient.colors;
    sysint_t colorsLength = ctx->radialGradient.colorsLength;

    int index;

    double dx = (double)x - ctx->conicalGradient.dx;
    double dy = (double)y - ctx->conicalGradient.dy;
    double scale = (double)colorsLength / (M_PI * 2.0);
    double add = ctx->conicalGradient.angle;
    if (add < M_PI) add += M_PI * 2.0;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        index = (int)((atan2(dy, dx) + add) * scale);
        if (index >= colorsLength) index -= colorsLength;

        ((uint32_t*)dst)[0] = colors[index];
        dst += 4;
        dx += 1.0;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        dx += hole;
      })
    P_FETCH_SPAN8_END()
  }
};

} // RasterEngine namespace
} // Fog namespace
