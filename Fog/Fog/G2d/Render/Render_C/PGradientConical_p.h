// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDER_C_PGRADIENTCONICAL_P_H
#define _FOG_G2D_RENDER_RENDER_C_PGRADIENTCONICAL_P_H

// [Dependencies]
#include <Fog/G2d/Render/Render_C/PGradientBase_p.h>

namespace Fog {
namespace Render_C {

// ============================================================================
// [Fog::Render_C - PGradientConical]
// ============================================================================

struct FOG_NO_EXPORT PGradientConical
{
#if 0
  // ==========================================================================
  // [Init]
  // ==========================================================================

  static err_t FOG_FASTCALL init_conical(
    RenderPatternContext* ctx, const Pattern& pattern, const TransformD& transform, uint32_t interpolationType)
  {
    PatternData* d = pattern._d;
    FOG_ASSERT(d->type == PATTERN_TYPE_CONICAL_GRADIENT);

    if (d->obj.stops->getLength() == 0)
    {
      return rasterFuncs.pattern.solid_init(ctx, 0x00000000);
    }
    if (d->obj.stops->getLength() == 1)
    {
      return rasterFuncs.pattern.solid_init(ctx, d->obj.stops->at(0).getArgb32());
    }

    TransformD m(pattern._d->transform.multiplied(transform));

    PointD points[2];
    m.mapPoints(points, d->data.gradient->points, 2);

    int gLength = 256 * (int)d->obj.stops->getLength();
    if (gLength > 4096) gLength = 4096;

    ctx->conicalGradient.dx = points[0].x;
    ctx->conicalGradient.dy = points[0].y;
    ctx->conicalGradient.angle = Math::atan2(
      (points[0].x - points[1].x),
      (points[0].y - points[1].y)) + (MATH_PI/2.0);

    err_t err = init_generic(ctx, d->obj.stops.instance(), gLength, d->spread);
    if (FOG_IS_ERROR(err)) return err;

    // Set fetch function.
    ctx->fetch = rasterFuncs.pattern.conical_gradient_fetch;

    // Set destroy function.
    ctx->destroy = destroy_generic;

    ctx->initialized = true;
    return ERR_OK;
  }

  // ==========================================================================
  // [Fetch]
  // ==========================================================================

  static void FOG_FASTCALL fetch_conical(
    const RenderPatternContext* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    const uint32_t* colors = (const uint32_t*)ctx->radialGradient.colors;

    int index;
    int colorsLength = ctx->radialGradient.colorsLength;

    double dx = (double)x - ctx->conicalGradient.dx;
    double dy = (double)y - ctx->conicalGradient.dy;
    double scale = (double)colorsLength / (MATH_PI * 2.0);
    double add = ctx->conicalGradient.angle;
    if (add < MATH_PI) add += MATH_PI * 2.0;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        index = (int)((Math::atan2(dy, dx) + add) * scale);
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
#endif
};

} // Render_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_PGRADIENTCONICAL_P_H
