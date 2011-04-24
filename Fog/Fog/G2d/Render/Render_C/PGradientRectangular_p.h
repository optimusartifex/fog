// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDER_C_PGRADIENTRECTANGULAR_P_H
#define _FOG_G2D_RENDER_RENDER_C_PGRADIENTRECTANGULAR_P_H

// [Dependencies]
#include <Fog/G2d/Render/Render_C/PGradientBase_p.h>

namespace Fog {
namespace Render_C {

// ============================================================================
// [Fog::Render_C - PGradientRectangular]
// ============================================================================

struct FOG_NO_EXPORT PGradientRectangular
{
  // ==========================================================================
  // [Create]
  // ==========================================================================

  // TODO: 16-bit rasterizer.
  static err_t FOG_FASTCALL create(
    RenderPatternContext* ctx, uint32_t dstFormat, const BoxI& clipBox,
    const GradientD& gradient,
    const TransformD& tr,
    uint32_t gradientQuality)
  {
    const ColorStopList& stops = gradient.getStops();
    uint32_t spread = gradient.getGradientSpread();

    FOG_ASSERT(gradient.getGradientType() == GRADIENT_TYPE_RECTANGULAR);
    FOG_ASSERT(stops.getLength() >= 1);
    FOG_ASSERT(spread < GRADIENT_SPREAD_COUNT);

    // ------------------------------------------------------------------------
    // [Move focal point to (0, 0)]
    // ------------------------------------------------------------------------

    double fx = gradient._pts[2].x;
    double fy = gradient._pts[2].y;

    TransformD t(tr);
    t.translate(PointD(fx, fy));

    TransformD inv(UNINITIALIZED);
    bool isInverted = TransformD::invert(inv, t);

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    if (stops.getLength() < 2 || !isInverted)
    {
      return Helpers::p_solid_create_color(ctx, dstFormat, stops.getAt(stops.getLength()-1).getColor());
    }

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    FOG_RETURN_ON_ERROR(PGradientBase::create(ctx, dstFormat, clipBox, spread, stops));
    int tableLength = ctx->_d.gradient.base.len;

    // TODO:
    uint32_t srcFormat = IMAGE_FORMAT_PRGB32;

    double cx = gradient._pts[0].x;
    double cy = gradient._pts[0].y;

    ctx->_d.gradient.rectangular.shared.xx = inv._00;
    ctx->_d.gradient.rectangular.shared.xy = inv._01;

    ctx->_d.gradient.rectangular.shared.yx = inv._10;
    ctx->_d.gradient.rectangular.shared.yy = inv._11;

    ctx->_d.gradient.rectangular.shared.tx = inv._20 + 0.5 * (inv._00 + inv._10); // Center.
    ctx->_d.gradient.rectangular.shared.ty = inv._21 + 0.5 * (inv._01 + inv._11); // Center.

    double len_d = (double)(tableLength - 1);
    double dx0 = Math::abs(gradient._pts[0].x - fx);
    double dy0 = Math::abs(gradient._pts[0].y - fy);
    double dx1 = Math::abs(gradient._pts[1].x - fx);
    double dy1 = Math::abs(gradient._pts[1].y - fy);

    if (Math::isFuzzyZero(dx0)) dx0 = 0.01;
    if (Math::isFuzzyZero(dy0)) dy0 = 0.01;
    if (Math::isFuzzyZero(dx1)) dx1 = 0.01;
    if (Math::isFuzzyZero(dy1)) dy1 = 0.01;

    ctx->_d.gradient.rectangular.shared.px0 = -len_d / dx0;
    ctx->_d.gradient.rectangular.shared.py0 = -len_d / dy0;
    ctx->_d.gradient.rectangular.shared.px1 =  len_d / dx1;
    ctx->_d.gradient.rectangular.shared.py1 =  len_d / dy1;

    // ------------------------------------------------------------------------
    // [Simple]
    // ------------------------------------------------------------------------

    if (inv.getType() <= TRANSFORM_TYPE_AFFINE)
    {
      ctx->_prepare = prepare_simple;
      ctx->_destroy = PGradientBase::destroy;
      ctx->_fetch = _g2d_render.gradient.rectangular.fetch_simple_nearest[srcFormat][spread];
      ctx->_skip = skip_simple;

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Projection]
    // ------------------------------------------------------------------------

    else
    {
      ctx->_d.gradient.rectangular.proj.xz = inv._02;
      ctx->_d.gradient.rectangular.proj.yz = inv._12;
      ctx->_d.gradient.rectangular.proj.tz = inv._22 + 0.5 * (inv._02 + inv._12); // Center.

      ctx->_prepare = prepare_proj;
      ctx->_destroy = PGradientBase::destroy;
      ctx->_fetch = _g2d_render.gradient.rectangular.fetch_proj_nearest[srcFormat][spread];
      ctx->_skip = skip_proj;

      return ERR_OK;
    }
  }

  // ==========================================================================
  // [Prepare]
  // ==========================================================================

  static void FOG_FASTCALL prepare_simple(
    const RenderPatternContext* ctx, RenderPatternFetcher* fetcher, int _y, int _delta, uint32_t mode)
  {
    double y = (double)_y;
    double d = (double)_delta;

    fetcher->_ctx = ctx;
    fetcher->_fetch = ctx->_fetch;
    fetcher->_skip = ctx->_skip;
    fetcher->_mode = mode;

    fetcher->_d.gradient.rectangular.simple.px = y * ctx->_d.gradient.rectangular.simple.yx + ctx->_d.gradient.rectangular.simple.tx;
    fetcher->_d.gradient.rectangular.simple.py = y * ctx->_d.gradient.rectangular.simple.yy + ctx->_d.gradient.rectangular.simple.ty;

    fetcher->_d.gradient.rectangular.simple.dx = d * ctx->_d.gradient.rectangular.simple.yx;
    fetcher->_d.gradient.rectangular.simple.dy = d * ctx->_d.gradient.rectangular.simple.yy;
  }

  static void FOG_FASTCALL prepare_proj(
    const RenderPatternContext* ctx, RenderPatternFetcher* fetcher, int _y, int _delta, uint32_t mode)
  {
    double y = (double)_y;
    double d = (double)_delta;

    fetcher->_ctx = ctx;
    fetcher->_fetch = ctx->_fetch;
    fetcher->_skip = ctx->_skip;
    fetcher->_mode = mode;

    fetcher->_d.gradient.rectangular.proj.px = y * ctx->_d.gradient.rectangular.proj.yx + ctx->_d.gradient.rectangular.proj.tx;
    fetcher->_d.gradient.rectangular.proj.py = y * ctx->_d.gradient.rectangular.proj.yy + ctx->_d.gradient.rectangular.proj.ty;
    fetcher->_d.gradient.rectangular.proj.pz = y * ctx->_d.gradient.rectangular.proj.yz + ctx->_d.gradient.rectangular.proj.tz;

    fetcher->_d.gradient.rectangular.proj.dx = d * ctx->_d.gradient.rectangular.proj.yx;
    fetcher->_d.gradient.rectangular.proj.dy = d * ctx->_d.gradient.rectangular.proj.yy;
    fetcher->_d.gradient.rectangular.proj.dz = d * ctx->_d.gradient.rectangular.proj.yz;
  }

  // ==========================================================================
  // [Fetch - Simple]
  // ==========================================================================

  template<typename Accessor>
  static void FOG_FASTCALL fetch_simple_nearest(
    RenderPatternFetcher* fetcher, Span* span, uint8_t* buffer)
  {
    const RenderPatternContext* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    P_FETCH_SPAN8_INIT()

    double dx = ctx->_d.gradient.rectangular.simple.xx;
    double dy = ctx->_d.gradient.rectangular.simple.xy;

    double px0 = ctx->_d.gradient.rectangular.simple.px0;
    double py0 = ctx->_d.gradient.rectangular.simple.py0;
    double px1 = ctx->_d.gradient.rectangular.simple.px1;
    double py1 = ctx->_d.gradient.rectangular.simple.py1;

    int lenMask = ctx->_d.gradient.base.len - 1;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      double _x = (double)x;
      double px = _x * dx + fetcher->_d.gradient.rectangular.simple.px;
      double py = _x * dy + fetcher->_d.gradient.rectangular.simple.py;

      do {
        double rx = (px < 0.0) ? px * px0 : px * px1;
        double ry = (py < 0.0) ? py * py0 : py * py1;

        typename Accessor::Pixel c0;
        accessor.fetchAtD(c0, Math::max<double>(rx, ry));
        accessor.store(dst, c0);

        dst += Accessor::DST_BPP;
        px += dx;
        py += dy;
      } while (--w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    fetcher->_d.gradient.rectangular.simple.px += fetcher->_d.gradient.rectangular.simple.dx;
    fetcher->_d.gradient.rectangular.simple.py += fetcher->_d.gradient.rectangular.simple.dy;
  }

  // ==========================================================================
  // [Fetch - Projection]
  // ==========================================================================

  template<typename Accessor>
  static void FOG_FASTCALL fetch_proj_nearest(
    RenderPatternFetcher* fetcher, Span* span, uint8_t* buffer)
  {
    const RenderPatternContext* ctx = fetcher->getContext();
    Accessor accessor(ctx);

    P_FETCH_SPAN8_INIT()

    double dx = ctx->_d.gradient.rectangular.proj.xx;
    double dy = ctx->_d.gradient.rectangular.proj.xy;
    double dz = ctx->_d.gradient.rectangular.proj.xz;

    double px0 = ctx->_d.gradient.rectangular.proj.px0;
    double py0 = ctx->_d.gradient.rectangular.proj.py0;
    double px1 = ctx->_d.gradient.rectangular.proj.px1;
    double py1 = ctx->_d.gradient.rectangular.proj.py1;

    int lenMask = ctx->_d.gradient.base.len - 1;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      double _x = (double)x;
      double pz = _x * ctx->_d.gradient.rectangular.proj.xz + fetcher->_d.gradient.rectangular.proj.pz;
      double pzRecip = 1.0 / pz;

      double px = _x * dx + fetcher->_d.gradient.rectangular.proj.px;
      double py = _x * dy + fetcher->_d.gradient.rectangular.proj.py;

      do {
        double rx = px * pzRecip;
        double ry = py * pzRecip;

        pz += dz;
        pzRecip = 1.0 / pz;

        rx = (rx < 0.0) ? rx * px0 : rx * px1;
        ry = (ry < 0.0) ? ry * py0 : ry * py1;

        typename Accessor::Pixel c0;
        accessor.fetchAtD(c0, Math::max<double>(rx, ry));
        accessor.store(dst, c0);

        px += dx;
        py += dy;

        dst += Accessor::DST_BPP;
      } while (--w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    fetcher->_d.gradient.rectangular.proj.px += fetcher->_d.gradient.rectangular.proj.dx;
    fetcher->_d.gradient.rectangular.proj.py += fetcher->_d.gradient.rectangular.proj.dy;
    fetcher->_d.gradient.rectangular.proj.pz += fetcher->_d.gradient.rectangular.proj.dz;
  }

  // ==========================================================================
  // [Skip]
  // ==========================================================================

  static void FOG_FASTCALL skip_simple(
    RenderPatternFetcher* fetcher, int step)
  {
    double s = (double)step;

    fetcher->_d.gradient.rectangular.simple.px += fetcher->_d.gradient.rectangular.simple.dx * s;
    fetcher->_d.gradient.rectangular.simple.py += fetcher->_d.gradient.rectangular.simple.dy * s;
  }

  static void FOG_FASTCALL skip_proj(
    RenderPatternFetcher* fetcher, int step)
  {
    double s = (double)step;

    fetcher->_d.gradient.rectangular.proj.px += fetcher->_d.gradient.rectangular.proj.dx * s;
    fetcher->_d.gradient.rectangular.proj.py += fetcher->_d.gradient.rectangular.proj.dy * s;
    fetcher->_d.gradient.rectangular.proj.pz += fetcher->_d.gradient.rectangular.proj.dz * s;
  }
};

} // Render_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_PGRADIENTRECTANGULAR_P_H
