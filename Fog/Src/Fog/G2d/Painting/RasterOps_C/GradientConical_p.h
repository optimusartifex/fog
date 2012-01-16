// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_GRADIENTCONICAL_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_GRADIENTCONICAL_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/GradientBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - PGradientConical]
// ============================================================================

struct FOG_NO_EXPORT PGradientConical
{
  // ==========================================================================
  // [Create]
  // ==========================================================================

  // TODO: 16-bit rasterizer.
  static err_t FOG_FASTCALL create(
    RasterPattern* ctx, uint32_t dstFormat, const BoxI* clipBox,
    const GradientD* gradient,
    const TransformD* tr,
    uint32_t gradientQuality)
  {
    const ColorStopList& stops = gradient->getStops();
    uint32_t spread = gradient->getGradientSpread();

    FOG_ASSERT(gradient->getGradientType() == GRADIENT_TYPE_CONICAL);
    FOG_ASSERT(spread < GRADIENT_SPREAD_COUNT);

    if (stops.isEmpty())
    {
      Color color(Argb32(0x00000000));
      return Helpers::p_solid_create_color(ctx, dstFormat, &color);
    }

    // ------------------------------------------------------------------------
    // [Transform input matrix to get the center point at (0, 0)].
    // ------------------------------------------------------------------------

    TransformD t(*tr);

    double cx = gradient->_pts[0].x;
    double cy = gradient->_pts[0].y;
    t.translate(PointD(cx, cy));

    TransformD inv(UNINITIALIZED);
    bool isInverted = TransformD::invert(inv, t);

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    if (stops.getLength() < 2 || !isInverted)
    {
      const ColorStop& stop = stops.getAt(stops.getLength() - 1);
      return Helpers::p_solid_create_color(ctx, dstFormat, &stop._color);
    }

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    FOG_RETURN_ON_ERROR(PGradientBase::create(ctx, dstFormat, clipBox, spread, &stops));
    int tableLength = ctx->_d.gradient.base.len;

    // TODO:
    uint32_t srcFormat = IMAGE_FORMAT_PRGB32;

    double angle = Math::repeat(gradient->_pts[1].x * MATH_1_DIV_TWO_PI, 1.0);

    // There is no such concept like conical gradient using perspective
    // transform, because the function atan2(y/w, x/w) can be simplified to
    // atan2(y, x), later to atan(y/x). This allows to completely remove the
    // projection part of the transform.
    //
    // TODO: Maybe the sign of the projection position is important, more
    // research needed.
    ctx->_d.gradient.conical.shared.xx = inv._00;
    ctx->_d.gradient.conical.shared.xy = inv._01;

    ctx->_d.gradient.conical.shared.yx = inv._10;
    ctx->_d.gradient.conical.shared.yy = inv._11;

    ctx->_d.gradient.conical.shared.tx = inv._20 + 0.5 * (inv._00 + inv._10); // Center.
    ctx->_d.gradient.conical.shared.ty = inv._21 + 0.5 * (inv._01 + inv._11); // Center.

    ctx->_d.gradient.conical.shared.offset = (double)tableLength * (2.0 - angle);
    ctx->_d.gradient.conical.shared.scale = (double)(tableLength - 1) / (MATH_TWO_PI);

    ctx->_prepare = prepare_simple;
    ctx->_destroy = PGradientBase::destroy;
    ctx->_fetch = _api_raster.gradient.conical.fetch_simple_nearest[srcFormat];
    ctx->_skip = skip_simple;

    return ERR_OK;
  }

  // ==========================================================================
  // [Prepare]
  // ==========================================================================

  static void FOG_FASTCALL prepare_simple(
    const RasterPattern* ctx, RasterPatternFetcher* fetcher, int _y, int _delta, uint32_t mode)
  {
    double y = (double)_y;
    double d = (double)_delta;

    fetcher->_ctx = ctx;
    fetcher->_fetch = ctx->_fetch;
    fetcher->_skip = ctx->_skip;
    fetcher->_mode = mode;

    fetcher->_d.gradient.conical.simple.px = y * ctx->_d.gradient.conical.simple.yx + ctx->_d.gradient.conical.simple.tx;
    fetcher->_d.gradient.conical.simple.py = y * ctx->_d.gradient.conical.simple.yy + ctx->_d.gradient.conical.simple.ty;

    fetcher->_d.gradient.conical.simple.dx = d * ctx->_d.gradient.conical.simple.yx;
    fetcher->_d.gradient.conical.simple.dy = d * ctx->_d.gradient.conical.simple.yy;
  }

  // ==========================================================================
  // [Fetch - Simple]
  // ==========================================================================

  template<typename Accessor>
  static void FOG_FASTCALL fetch_simple_nearest(
    RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer)
  {
    const RasterPattern* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    P_FETCH_SPAN8_INIT()

    double dx = ctx->_d.gradient.conical.simple.xx;
    double dy = ctx->_d.gradient.conical.simple.xy;

    double offset = ctx->_d.gradient.conical.simple.offset;
    double scale = ctx->_d.gradient.conical.simple.scale;

    int lenMask = ctx->_d.gradient.base.len - 1;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      double _x = (double)x;
      double px = _x * dx + fetcher->_d.gradient.conical.simple.px;
      double py = _x * dy + fetcher->_d.gradient.conical.simple.py;

      do {
        double a = Math::atan2(py, px);
        int ipos = (int)(offset - a * scale) & lenMask;

        typename Accessor::Pixel c0;
        accessor.fetchRaw(c0, ipos);
        accessor.storePix(dst, c0);

        dst += Accessor::DST_BPP;
        px += dx;
        py += dy;
      } while (--w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    fetcher->_d.gradient.conical.simple.px += fetcher->_d.gradient.conical.simple.dx;
    fetcher->_d.gradient.conical.simple.py += fetcher->_d.gradient.conical.simple.dy;
  }

  // ==========================================================================
  // [Skip]
  // ==========================================================================

  static void FOG_FASTCALL skip_simple(
    RasterPatternFetcher* fetcher, int step)
  {
    double s = (double)step;

    fetcher->_d.gradient.conical.simple.px += fetcher->_d.gradient.conical.simple.dx * s;
    fetcher->_d.gradient.conical.simple.py += fetcher->_d.gradient.conical.simple.dy * s;
  }
};

} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_GRADIENTCONICAL_P_H
