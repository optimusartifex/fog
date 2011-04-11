// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDER_C_PGRADIENTRADIAL_P_H
#define _FOG_G2D_RENDER_RENDER_C_PGRADIENTRADIAL_P_H

// [Dependencies]
#include <Fog/G2d/Render/Render_C/PGradientBase_p.h>

namespace Fog {
namespace Render_C {

// ============================================================================
// [Fog::Render_C - PGradientRadial]
// ============================================================================

struct FOG_NO_EXPORT PGradientRadial
{
#if 0
  // ==========================================================================
  // [Init]
  // ==========================================================================

  static err_t FOG_FASTCALL init_radial(
    RenderPatternContext* ctx, const Pattern& pattern, const TransformD& transform, uint32_t interpolationType)
  {
    PatternData* d = pattern._d;
    FOG_ASSERT(d->type == PATTERN_TYPE_RADIAL_GRADIENT);

    if (d->obj.stops->getLength() == 0)
      return rasterFuncs.pattern.solid_init(ctx, 0x00000000);

    if (d->obj.stops->getLength() == 1)
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(0).getArgb32());

    TransformD m(pattern._d->transform.multiplied(transform));

    PointD points[2];
    m.mapPoints(points, d->data.gradient->points, 2);
    
    int gLength = 256 * (int)d->obj.stops->getLength();
    if (gLength > 4096) gLength = 4096;
    int gAlloc = gLength;

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
    if (d->spread == GRADIENT_SPREAD_REFLECT) gAlloc <<= 1;

    ctx->radialGradient.mul = (double)gLength / dd;

    err_t err = init_generic(ctx, d->obj.stops.instance(), gLength, d->spread);
    if (FOG_IS_ERROR(err)) return err;

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

  // ==========================================================================
  // [Fetch]
  // ==========================================================================

  /*

  This is reference implementation from AntiGrain 2.4.

  static void FOG_FASTCALL radial_gradient_fetch_pad(
    const RenderPatternContext* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
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

      index = (int) ((dx * fx + dyfy + Math::sqrt(Math::abs(d3))) * scale);

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
    const RenderPatternContext* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->radialGradient.colors;

    int index;
    int colorsLength = ctx->radialGradient.colorsLength;

    uint32_t color0 = colors[0];
    uint32_t color1 = colors[colorsLength];

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
        index = (int)((dd + Math::sqrt(Math::abs(cc))) * scale);

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
    const RenderPatternContext* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->radialGradient.colors;

    int index;
    int colorsLength = ctx->radialGradient.colorsLength;

    uint32_t color0 = colors[0];
    uint32_t color1 = colors[colorsLength-1];

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
        index = (int)((dd + Math::sqrt(Math::abs(cc))) * scale) % colorsLength;
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
#endif
};

} // Render_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_PGRADIENTRADIAL_P_H
