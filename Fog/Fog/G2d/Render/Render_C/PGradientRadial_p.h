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

    FOG_ASSERT(gradient.getGradientType() == GRADIENT_TYPE_RADIAL);
    FOG_ASSERT(stops.getLength() >= 1);
    FOG_ASSERT(spread < GRADIENT_SPREAD_COUNT);

    // ------------------------------------------------------------------------
    // [Transform elliptic shape to a circle]
    // ------------------------------------------------------------------------

    TransformD t(tr);

    double cx = gradient._pts[0].x;
    double cy = gradient._pts[0].y;
    double fx = gradient._pts[1].x;
    double fy = gradient._pts[1].y;
    double r  = gradient._pts[2].x;

    bool zeroRadius = Math::isFuzzyZero(gradient._pts[2].x) ||
                      Math::isFuzzyZero(gradient._pts[2].y);

    if (r != gradient._pts[2].y && !zeroRadius)
    {
      double s = gradient._pts[2].y / r;

      t.translate(PointD(cx, cy));
      t.scale(PointD(1.0, s));
      t.translate(PointD(-cx, -cy));

      fy = cy + (fy - cy) / s;
    }

    TransformD inv(UNINITIALIZED);
    bool isInverted = TransformD::invert(inv, t);

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    if (stops.getLength() < 2 || !isInverted || zeroRadius)
    {
      return Helpers::p_solid_create_color(ctx, dstFormat, stops.getAt(stops.getLength()-1).getColor());
    }

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    FOG_RETURN_ON_ERROR(PGradientBase::create(ctx, dstFormat, clipBox, spread, stops));
    int tableLength = ctx->_d.gradient.base.len;
    
    double fxOrig = fx;
    double fyOrig = fy;

    fx -= cx;
    fy -= cy;

    double r2 = r  * r;
    double fx2 = fx * fx;
    double fy2 = fy * fy;
    double dd = r2 - (fx2 + fy2);

    // Divisor degenerated to zero, that means that focal point is near
    // the border of a circle.
    if (Math::isFuzzyZero(dd))
    {
      if (!Math::isFuzzyZero(fx)) fx += (fx < 0.0) ? 0.5 : -0.5;
      if (!Math::isFuzzyZero(fy)) fy += (fy < 0.0) ? 0.5 : -0.5;

      fx2 = fx * fx;
      fy2 = fy * fy;
      dd = r2 - (fx2 + fy2);
    }

    double ax = inv._00; // Linear X increment per one device pixel.
    double ay = inv._01; // Linear Y increment per one device pixel.

    double r2mfxfx = r2 - fx2;
    double r2mfyfy = r2 - fy2;
    double _2_fxfy = 2. * fx * fy;

    double axax = ax * ax;
    double ayay = ay * ay;
    double axay = ax * ay;

    // The radial gradient fetcher uses following equation:
    //
    //    b = x * fx + y * fy
    //    d = x^2 * (r^2 - fy^2) + y^2 * (r^2 - fx^2) + x*y * (2*fx*fy)
    //
    //    pos = ((b + sqrt(d))) * scale)
    //
    // Which can be simplified if we eliminate constants:
    //
    //    C1 = r^2 - fy^2
    //    C2 = r^2 - fx^2
    //    C3 = 2*fx*fy
    //
    //    b = x * fx + y * fy
    //    d = x^2 * C1 + y^2 * C2 + x*y * C3
    //
    //    pos = ((b + sqrt(d))) * scale)

    ctx->_d.gradient.radial.shared.fx  = fx;
    ctx->_d.gradient.radial.shared.fy  = fy;
    ctx->_d.gradient.radial.shared.scale = (double)tableLength / dd;

    ctx->_d.gradient.radial.shared.xx = inv._00;
    ctx->_d.gradient.radial.shared.yx = inv._01;

    ctx->_d.gradient.radial.shared.xy = inv._10;
    ctx->_d.gradient.radial.shared.yy = inv._11;

    ctx->_d.gradient.radial.shared.tx = inv._20 - fxOrig + 0.5 * (inv._00 + inv._10); // Center.
    ctx->_d.gradient.radial.shared.ty = inv._21 - fyOrig + 0.5 * (inv._01 + inv._11); // Center.

    ctx->_d.gradient.radial.shared.r2mfxfx = r2mfxfx;
    ctx->_d.gradient.radial.shared.r2mfyfy = r2mfyfy;
    ctx->_d.gradient.radial.shared._2_fxfy = _2_fxfy;

    // ------------------------------------------------------------------------
    // [Simple]
    // ------------------------------------------------------------------------

    if (inv.getType() <= TRANSFORM_TYPE_AFFINE)
    {
      // The x and y variables increase linearly, this means that we can use 
      // double differentiation to get delta (d) and delta-of-delta (dd). We 
      // first separate the equation into three ones:
      //
      //    D1 = x^2 * C1
      //    D2 = y^2 * C2
      //    D3 = x*y * C3
      //
      //    d = D1 + D2 + D3
      //
      // So we have general equation 'x^2 * C' and we must find the deltas:
      //
      //   'x^2 * C': 1st delta 'd' at step 't'    : t^2 * C + x * 2*C
      //   'x^2 * C': 2nd delta 'd' at step 't'    : t^2 * 2*C
      //
      //   ( Hint, use Mathematica DifferenceDelta[x*x*C, {x, 1, t}] )
      //
      // The equation 'x*y*C' can't be separated, so we have:
      //
      //   'x*y * C': 1st delta 'd' at step 'tx/ty': x*ty * C + y*tx * C + tx*ty * C
      //   'x*y * C': 2nd delta 'd' at step 'tx/ty': tx*ty * 2*C

      ctx->_d.gradient.radial.simple.b_d   = ax * fx + ay * fy;
      ctx->_d.gradient.radial.simple.d_d   = axax * r2mfyfy + ayay * r2mfxfx + axay * _2_fxfy;
      ctx->_d.gradient.radial.simple.d_d_x = 2. * (ax * r2mfyfy) + ay * _2_fxfy;
      ctx->_d.gradient.radial.simple.d_d_y = 2. * (ay * r2mfxfx) + ax * _2_fxfy;
      ctx->_d.gradient.radial.simple.d_d_d = 2. * (axax * r2mfyfy + ayay * r2mfxfx + axay * _2_fxfy);

      ctx->_prepare = prepare_simple;
      ctx->_destroy = PGradientBase::destroy;
      ctx->_fetch = _g2d_render.gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][spread];
      ctx->_skip = skip_simple;
    }

    // ------------------------------------------------------------------------
    // [Projection]
    // ------------------------------------------------------------------------

    else
    {
      // No specific optimization can be performed, because x/y don't increase
      // linearly. 

      ctx->_d.gradient.radial.proj.zx = inv._02;
      ctx->_d.gradient.radial.proj.zy = inv._12;
      ctx->_d.gradient.radial.proj.tz = inv._22 + 0.5 * (inv._02 + inv._12); // Center.

      // We can't translate focal point in projection, it has to be calcualted
      // after x/y is divided by z.
      ctx->_d.gradient.radial.proj.tx += fxOrig;
      ctx->_d.gradient.radial.proj.ty += fyOrig;

      ctx->_d.gradient.radial.proj.fxOrig = fxOrig;
      ctx->_d.gradient.radial.proj.fyOrig = fyOrig;

      ctx->_prepare = prepare_proj;
      ctx->_destroy = PGradientBase::destroy;
      ctx->_fetch = _g2d_render.gradient.radial.fetch_proj_nearest[IMAGE_FORMAT_PRGB32][spread];
      ctx->_skip = skip_proj;
    }

    return ERR_OK;
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

    fetcher->_d.gradient.radial.simple.px = y * ctx->_d.gradient.radial.simple.xy + ctx->_d.gradient.radial.simple.tx;
    fetcher->_d.gradient.radial.simple.py = y * ctx->_d.gradient.radial.simple.yy + ctx->_d.gradient.radial.simple.ty;

    fetcher->_d.gradient.radial.simple.dx = d * ctx->_d.gradient.radial.simple.xy;
    fetcher->_d.gradient.radial.simple.dy = d * ctx->_d.gradient.radial.simple.yy;
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

    fetcher->_d.gradient.radial.proj.px = y * ctx->_d.gradient.radial.proj.xy + ctx->_d.gradient.radial.proj.tx;
    fetcher->_d.gradient.radial.proj.py = y * ctx->_d.gradient.radial.proj.yy + ctx->_d.gradient.radial.proj.ty;
    fetcher->_d.gradient.radial.proj.pz = y * ctx->_d.gradient.radial.proj.zy + ctx->_d.gradient.radial.proj.tz;

    fetcher->_d.gradient.radial.proj.dx = d * ctx->_d.gradient.radial.proj.xy;
    fetcher->_d.gradient.radial.proj.dy = d * ctx->_d.gradient.radial.proj.yy;
    fetcher->_d.gradient.radial.proj.dz = d * ctx->_d.gradient.radial.proj.zy;
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

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      double _x    = (double)x;
      double dx    = _x * ctx->_d.gradient.radial.simple.xx + fetcher->_d.gradient.radial.simple.px;
      double dy    = _x * ctx->_d.gradient.radial.simple.yx + fetcher->_d.gradient.radial.simple.py;

      double b     = ctx->_d.gradient.radial.simple.fx * dx + 
                     ctx->_d.gradient.radial.simple.fy * dy;
      double b_d   = ctx->_d.gradient.radial.simple.b_d;

      double d     = ctx->_d.gradient.radial.simple.r2mfyfy * dx * dx + 
                     ctx->_d.gradient.radial.simple.r2mfxfx * dy * dy +
                     ctx->_d.gradient.radial.simple._2_fxfy * dx * dy;
      double d_d   = ctx->_d.gradient.radial.simple.d_d +
                     ctx->_d.gradient.radial.simple.d_d_x * dx +
                     ctx->_d.gradient.radial.simple.d_d_y * dy;
      double d_d_d = ctx->_d.gradient.radial.simple.d_d_d;
      double scale = ctx->_d.gradient.radial.simple.scale;

      do {
        typename Accessor::Pixel c0;
        accessor.fetchAtD(c0, (b + Math::sqrt(Math::abs(d))) * scale);
        accessor.store(dst, c0);
        dst += Accessor::DST_BPP;

        b   += b_d;
        d   += d_d;
        d_d += d_d_d;
      } while (--w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    fetcher->_d.gradient.radial.simple.px += fetcher->_d.gradient.radial.simple.dx;
    fetcher->_d.gradient.radial.simple.py += fetcher->_d.gradient.radial.simple.dy;
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

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      double _x = (double)x;
      double dz = _x * ctx->_d.gradient.radial.proj.zx + fetcher->_d.gradient.radial.proj.pz;
      double dzRecip = 1.0 / dz; 

      double dx = _x * ctx->_d.gradient.radial.proj.xx + fetcher->_d.gradient.radial.proj.px;
      double dy = _x * ctx->_d.gradient.radial.proj.yx + fetcher->_d.gradient.radial.proj.py;
      double scale = ctx->_d.gradient.radial.simple.scale;

      do {
        double rx = dx * dzRecip - ctx->_d.gradient.radial.proj.fxOrig;
        double ry = dy * dzRecip - ctx->_d.gradient.radial.proj.fyOrig;

        dz += ctx->_d.gradient.radial.proj.zx;
        dzRecip = 1.0 / dz;
        dx += ctx->_d.gradient.radial.proj.xx;
        dy += ctx->_d.gradient.radial.proj.yx;

        double b = ctx->_d.gradient.radial.proj.fx * rx + 
                   ctx->_d.gradient.radial.proj.fy * ry;
        double d = ctx->_d.gradient.radial.proj.r2mfyfy * rx * rx + 
                   ctx->_d.gradient.radial.proj.r2mfxfx * ry * ry +
                   ctx->_d.gradient.radial.proj._2_fxfy * rx * ry;

        typename Accessor::Pixel c0;
        accessor.fetchAtD(c0, (b + Math::sqrt(Math::abs(d))) * scale);
        accessor.store(dst, c0);
        dst += Accessor::DST_BPP;
      } while (--w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()

    fetcher->_d.gradient.radial.proj.px += fetcher->_d.gradient.radial.proj.dx;
    fetcher->_d.gradient.radial.proj.py += fetcher->_d.gradient.radial.proj.dy;
    fetcher->_d.gradient.radial.proj.pz += fetcher->_d.gradient.radial.proj.dz;
  }

  // ==========================================================================
  // [Skip]
  // ==========================================================================

  static void FOG_FASTCALL skip_simple(
    RenderPatternFetcher* fetcher, int step)
  {
    double s = (double)step;

    fetcher->_d.gradient.radial.simple.px += fetcher->_d.gradient.radial.simple.dx * s;
    fetcher->_d.gradient.radial.simple.py += fetcher->_d.gradient.radial.simple.dy * s;
  }

  static void FOG_FASTCALL skip_proj(
    RenderPatternFetcher* fetcher, int step)
  {
    double s = (double)step;

    fetcher->_d.gradient.radial.proj.px += fetcher->_d.gradient.radial.proj.dx * s;
    fetcher->_d.gradient.radial.proj.py += fetcher->_d.gradient.radial.proj.dy * s;
    fetcher->_d.gradient.radial.proj.pz += fetcher->_d.gradient.radial.proj.dz * s;
  }
};

} // Render_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_PGRADIENTRADIAL_P_H
