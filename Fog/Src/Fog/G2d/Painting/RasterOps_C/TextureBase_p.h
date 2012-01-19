// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_TEXTUREBASE_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_TEXTUREBASE_P_H

#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Painting/RasterOps_C/BaseDefs_p.h>
#include <Fog/G2d/Painting/RasterOps_C/BaseHelpers_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - PTextureBase]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT PTextureBase
{
  // ==========================================================================
  // [Create]
  // ==========================================================================

  static err_t FOG_FASTCALL create(
    RasterPattern* ctx, uint32_t dstFormat, const BoxI* boundingBox,
    const Image* srcImage, const RectI* srcFragment, const TransformD* tr,
    const Color* clampColor, uint32_t tileMode,
    uint32_t imageQuality)
  {
    // ------------------------------------------------------------------------
    // [Asserts]
    // ------------------------------------------------------------------------

    FOG_ASSERT(!srcImage->isEmpty());
    FOG_ASSERT(tileMode < TEXTURE_TILE_COUNT);

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    uint32_t transformType = tr->getType();
    TransformD inv(UNINITIALIZED);

    if (transformType > TRANSFORM_TYPE_TRANSLATION && !TransformD::invert(inv, *tr))
    {
      return Helpers::p_solid_create_color(ctx, dstFormat, clampColor);
    }

    uint32_t srcFormat = srcImage->getFormat();
    uint32_t srcBPP = srcImage->getBytesPerPixel();
    uint32_t srcHasAlpha = (srcImage->getFormatDescription().getComponentMask() & IMAGE_COMPONENT_ALPHA) != 0;
    ssize_t srcStride = srcImage->getStride();

    if (tileMode == TEXTURE_TILE_CLAMP)
    {
      srcHasAlpha |= !clampColor->isOpaque();
    }

    // Setup the context.
    ctx->_initDst(dstFormat);
    ctx->_boundingBox = *boundingBox;

    // Initialize base variables, including shallow copy of a given image.
    ctx->_d.texture.base.texture.initCustom1(*srcImage);
    ctx->_d.texture.base.pixels = srcImage->getFirst() +
                                  (ssize_t)srcFragment->y * srcStride +
                                  (ssize_t)srcFragment->x * srcBPP;

    ctx->_d.texture.base.pal = (srcFormat == IMAGE_FORMAT_I8)
      ? reinterpret_cast<const uint32_t*>(srcImage->getPalette().getData())
      : NULL;
    ctx->_d.texture.base.w = srcFragment->w;
    ctx->_d.texture.base.h = srcFragment->h;
    ctx->_d.texture.base.stride = srcStride;

    ctx->_destroy = destroy;

    // Choose the format of fetched pixels.
    uint32_t fetchFormat = srcFormat;
    const RasterTextureFuncs::_Fetch* fetchFuncs;

    // ${IMAGE_FORMAT:BEGIN}
    switch (dstFormat)
    {
      // ----------------------------------------------------------------------
      // [8 Bits Per Component]
      // ----------------------------------------------------------------------

      case IMAGE_FORMAT_PRGB32:
      case IMAGE_FORMAT_XRGB32:
      case IMAGE_FORMAT_RGB24:
        if (srcHasAlpha)
        {
          fetchFormat = IMAGE_FORMAT_PRGB32;
          fetchFuncs = &_api_raster.texture.prgb32;
        }
        else
        {
          fetchFormat = IMAGE_FORMAT_XRGB32;
          fetchFuncs = &_api_raster.texture.prgb32;
        }
        goto _Has8BPC;

      case IMAGE_FORMAT_A8:
        fetchFormat = IMAGE_FORMAT_A8;
        fetchFuncs = &_api_raster.texture.a8;

_Has8BPC:
        if (tileMode == TEXTURE_TILE_CLAMP)
        {
          Face::p32PRGB32FromARGB32(ctx->_d.texture.base.clamp.prgb32.p32, clampColor->getArgb32().p32);
        }
        break;

      // ----------------------------------------------------------------------
      // [16 Bits Per Component]
      // ----------------------------------------------------------------------

      case IMAGE_FORMAT_PRGB64:
      case IMAGE_FORMAT_RGB48:
        if (srcHasAlpha)
        {
          fetchFormat = IMAGE_FORMAT_PRGB64;
          fetchFuncs = &_api_raster.texture.prgb64;
        }
        else
        {
          fetchFormat = IMAGE_FORMAT_RGB48;
          fetchFuncs = &_api_raster.texture.rgb48;
        }
        goto _Has16BPC;

      case IMAGE_FORMAT_A16:
        fetchFormat = IMAGE_FORMAT_A16;
        fetchFuncs = &_api_raster.texture.a16;

_Has16BPC:
        if (tileMode == TEXTURE_TILE_CLAMP)
        {
          Face::p64PRGB64FromARGB64(ctx->_d.texture.base.clamp.prgb64.p64, clampColor->getArgb64().p64);
        }
        break;

      case IMAGE_FORMAT_I8:
      default:
        FOG_ASSERT_NOT_REACHED();
    }
    // ${IMAGE_FORMAT:END}

    ctx->_srcFormat = fetchFormat;
    ctx->_srcBPP = ImageFormatDescription::getByFormat(fetchFormat).getBytesPerPixel();

    // ------------------------------------------------------------------------
    // [Simple - Align or SubXY]
    // ------------------------------------------------------------------------

    if (transformType <= TRANSFORM_TYPE_TRANSLATION)
    {
      int tx = 0;
      int ty = 0;

      uint fx = 0;
      uint fy = 0;

      ctx->_d.texture.simple.repeatY = 0;

      if (tileMode == TEXTURE_TILE_PAD || tileMode == TEXTURE_TILE_CLAMP)
      {
        ctx->_prepare = prepare_simple_pad_clamp;
        ctx->_skip = skip_simple_pad_clamp;
      }
      else
      {
        ctx->_prepare = prepare_simple_repeat_reflect;
        ctx->_skip = skip_simple_repeat_reflect;

        ctx->_d.texture.simple.repeatY = ctx->_d.texture.base.h;
        if (tileMode == TEXTURE_TILE_REFLECT) ctx->_d.texture.simple.repeatY *= 2;
      }

      if (transformType == TRANSFORM_TYPE_TRANSLATION)
      {
        int64_t tx48x16 = Math::fixed48x16FromFloat(tr->_20);
        int64_t ty48x16 = Math::fixed48x16FromFloat(tr->_21);

        tx = -(int)(tx48x16 >> 16);
        ty = -(int)(ty48x16 >> 16);

        fx = (uint)( (int)(tx48x16 >> 8) & 0xFF );
        fy = (uint)( (int)(ty48x16 >> 8) & 0xFF );
      }

      // Misaligned fetch, PTextureOffset must be used.
      if ((fx | fy) != 0)
      {
        if (imageQuality == IMAGE_QUALITY_NEAREST)
        {
          if (fx < 128) tx++;
          if (fy < 128) ty++;
        }
        else
        {
          // Calculate subpixel weights.
          //
          // +--+--+
          // |00|01| - Y0/X0 and Y0/X1
          // +--+--+
          // |10|11| - Y1/X0 and Y1/X1
          // +--+--+

          // TODO: 16-bit image rasterizer requires weight in 16-bit fixed point.
          ctx->_d.texture.simple.fY0X0 = ((      fy) * (      fx)) >> 8;
          ctx->_d.texture.simple.fY0X1 = ((      fy) * (256 - fx)) >> 8;
          ctx->_d.texture.simple.fY1X0 = ((256 - fy) * (      fx)) >> 8;
          ctx->_d.texture.simple.fY1X1 = ((256 - fy) * (256 - fx)) >> 8;

          if (fx) tx -= 1;
          if (fy) ty -= 1;

          ctx->_d.texture.simple.tx = tx;
          ctx->_d.texture.simple.ty = ty;

          // Setup functions.
          if (fx && fy)
            ctx->_fetch = fetchFuncs->fetch_simple_subxy[srcFormat][tileMode];
          else if (fx)
            ctx->_fetch = fetchFuncs->fetch_simple_subx0[srcFormat][tileMode];
          else if (fy)
            ctx->_fetch = fetchFuncs->fetch_simple_sub0y[srcFormat][tileMode];
          else
            FOG_ASSERT_NOT_REACHED();
          return ERR_OK;
        }
      }

      // Aligned or nearest fetch.
      ctx->_d.texture.simple.tx = tx;
      ctx->_d.texture.simple.ty = ty;

      ctx->_fetch = fetchFuncs->fetch_simple_align[srcFormat][tileMode];
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Affine]
    // ------------------------------------------------------------------------

    if (transformType <= TRANSFORM_TYPE_AFFINE)
    {
      // Offset values 'tx' added in-place by fetch(), 'ty' added by prepare().
      // Input values are centered to (0.5, 0.5).
      ctx->_d.texture.affine.tx = 0.5 * (inv._00 + inv._10) + inv._20;
      ctx->_d.texture.affine.ty = 0.5 * (inv._01 + inv._11) + inv._21;

      ctx->_d.texture.affine.mx = (double)ctx->_d.texture.base.w;
      ctx->_d.texture.affine.my = (double)ctx->_d.texture.base.h;

      // Translate the center of pixel back if the filter is not NEAREST.
      if (imageQuality != IMAGE_QUALITY_NEAREST)
      {
        ctx->_d.texture.affine.tx -= 0.5;
        ctx->_d.texture.affine.ty -= 0.5;
      }

      ctx->_d.texture.affine.xx = inv._00;
      ctx->_d.texture.affine.xy = inv._01;

      ctx->_d.texture.affine.yx = inv._10;
      ctx->_d.texture.affine.yy = inv._11;

      ctx->_d.texture.affine.xxZero = Math::isFuzzyZero(ctx->_d.texture.affine.xx);
      ctx->_d.texture.affine.xyZero = Math::isFuzzyZero(ctx->_d.texture.affine.xy);

      if (tileMode == TEXTURE_TILE_REFLECT)
      {
        ctx->_d.texture.affine.mx *= 2.0;
        ctx->_d.texture.affine.my *= 2.0;
      }

      if (tileMode == TEXTURE_TILE_REPEAT || tileMode == TEXTURE_TILE_REFLECT)
      {
        // This could happen for very small (nearly degenerated) scaling.
        if (Math::abs(ctx->_d.texture.affine.xx) >= ctx->_d.texture.affine.mx)
          ctx->_d.texture.affine.xx = Math::mod(ctx->_d.texture.affine.xx, ctx->_d.texture.affine.mx);
        if (Math::abs(ctx->_d.texture.affine.xy) >= ctx->_d.texture.affine.my)
          ctx->_d.texture.affine.xy = Math::mod(ctx->_d.texture.affine.xy, ctx->_d.texture.affine.my);

        // Rewind X/Y.
        ctx->_d.texture.affine.rx = ctx->_d.texture.affine.mx;
        ctx->_d.texture.affine.ry = ctx->_d.texture.affine.my;

        if (ctx->_d.texture.affine.xx > 0.0) ctx->_d.texture.affine.rx = -ctx->_d.texture.affine.rx;
        if (ctx->_d.texture.affine.xy > 0.0) ctx->_d.texture.affine.ry = -ctx->_d.texture.affine.ry;

        ctx->_d.texture.affine.rx16x16 = Math::fixed16x16FromFloat(ctx->_d.texture.affine.rx);
        ctx->_d.texture.affine.ry16x16 = Math::fixed16x16FromFloat(ctx->_d.texture.affine.ry);
      }
      else
      {
        // Shouldn't be used.
        ctx->_d.texture.affine.rx = 0.0;
        ctx->_d.texture.affine.ry = 0.0;

        ctx->_d.texture.affine.rx16x16 = 0;
        ctx->_d.texture.affine.ry16x16 = 0;
      }

      ctx->_d.texture.affine.xx16x16 = Math::fixed16x16FromFloat(ctx->_d.texture.affine.xx);
      ctx->_d.texture.affine.xy16x16 = Math::fixed16x16FromFloat(ctx->_d.texture.affine.xy);

      ctx->_d.texture.affine.mx16x16 = Math::fixed16x16FromFloat(ctx->_d.texture.affine.mx);
      ctx->_d.texture.affine.my16x16 = Math::fixed16x16FromFloat(ctx->_d.texture.affine.my);

      // TODO: Not always true.
      ctx->_d.texture.affine.safeFixedPoint = true;

      // Setup functions.
      if (tileMode == TEXTURE_TILE_PAD || tileMode == TEXTURE_TILE_CLAMP)
      {
        ctx->_prepare = prepare_affine_pad_clamp;
        ctx->_skip = skip_affine_pad_clamp;
      }
      else
      {
        ctx->_prepare = prepare_affine_repeat_reflect;
        ctx->_skip = skip_affine_repeat_reflect;
      }

      if (imageQuality == IMAGE_QUALITY_NEAREST)
        ctx->_fetch = fetchFuncs->fetch_affine_nearest[srcFormat][tileMode];
      else
        ctx->_fetch = fetchFuncs->fetch_affine_bilinear[srcFormat][tileMode];
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Projection]
    // ------------------------------------------------------------------------

    {
      // TODO: Not implemented.
      ctx->_fetch = NULL;
      return ERR_RT_NOT_IMPLEMENTED;
    }
  }

  // ==========================================================================
  // [Destroy]
  // ==========================================================================

  static void FOG_FASTCALL destroy(
    RasterPattern* ctx)
  {
    FOG_ASSERT(ctx->isInitialized());

    ctx->_d.texture.base.texture.destroy();
    ctx->reset();
  }

  // ==========================================================================
  // [Prepare]
  // ==========================================================================

  static void FOG_FASTCALL prepare_simple_pad_clamp(
    const RasterPattern* ctx, RasterPatternFetcher* fetcher, int _y, int _delta, uint32_t mode)
  {
    fetcher->_ctx = ctx;
    fetcher->_fetch = ctx->_fetch;
    fetcher->_skip = ctx->_skip;
    fetcher->_mode = mode;

    _y += ctx->_d.texture.simple.ty;

    fetcher->_d.texture.simple.py = _y;
    fetcher->_d.texture.simple.dy = _delta;
  }

  static void FOG_FASTCALL prepare_simple_repeat_reflect(
    const RasterPattern* ctx, RasterPatternFetcher* fetcher, int _y, int _delta, uint32_t mode)
  {
    fetcher->_ctx = ctx;
    fetcher->_fetch = ctx->_fetch;
    fetcher->_skip = ctx->_skip;
    fetcher->_mode = mode;

    _y += ctx->_d.texture.simple.ty;

    int ry = ctx->_d.texture.simple.repeatY;
    if ((uint)_y > (uint)ry) _y %= ry;
    if (_y < 0) _y += ry;

    if (_delta >= ry) _delta %= ry;

    fetcher->_d.texture.simple.py = _y;
    fetcher->_d.texture.simple.dy = _delta;
  }

  static void FOG_FASTCALL prepare_affine_pad_clamp(
    const RasterPattern* ctx, RasterPatternFetcher* fetcher, int _y, int _delta, uint32_t mode)
  {
    double y = (double)_y;
    double d = (double)_delta;

    fetcher->_ctx = ctx;
    fetcher->_fetch = ctx->_fetch;
    fetcher->_skip = ctx->_skip;
    fetcher->_mode = mode;

    fetcher->_d.texture.affine.px = y * ctx->_d.texture.affine.yx + ctx->_d.texture.affine.tx;
    fetcher->_d.texture.affine.py = y * ctx->_d.texture.affine.yy + ctx->_d.texture.affine.ty;

    fetcher->_d.texture.affine.dx = d * ctx->_d.texture.affine.yx;
    fetcher->_d.texture.affine.dy = d * ctx->_d.texture.affine.yy;
  }

  static void FOG_FASTCALL prepare_affine_repeat_reflect(
    const RasterPattern* ctx, RasterPatternFetcher* fetcher, int _y, int _delta, uint32_t mode)
  {
    double y = (double)_y;
    double d = (double)_delta;

    fetcher->_ctx = ctx;
    fetcher->_fetch = ctx->_fetch;
    fetcher->_skip = ctx->_skip;
    fetcher->_mode = mode;

    fetcher->_d.texture.affine.px = Math::repeat(y * ctx->_d.texture.affine.yx + ctx->_d.texture.affine.tx, ctx->_d.texture.affine.mx);
    fetcher->_d.texture.affine.py = Math::repeat(y * ctx->_d.texture.affine.yy + ctx->_d.texture.affine.ty, ctx->_d.texture.affine.my);

    fetcher->_d.texture.affine.dx = Math::repeat(d * ctx->_d.texture.affine.yx, ctx->_d.texture.affine.mx);
    fetcher->_d.texture.affine.dy = Math::repeat(d * ctx->_d.texture.affine.yy, ctx->_d.texture.affine.my);
  }

  // ==========================================================================
  // [Skip]
  // ==========================================================================

  static void FOG_FASTCALL skip_simple_pad_clamp(
    RasterPatternFetcher* fetcher, int step)
  {
    fetcher->_d.texture.simple.py += fetcher->_d.texture.simple.dy * step;
  }

  static void FOG_FASTCALL skip_simple_repeat_reflect(
    RasterPatternFetcher* fetcher, int step)
  {
    int _y = fetcher->_d.texture.simple.py;
    int ry = fetcher->_ctx->_d.texture.simple.repeatY;

    _y += fetcher->_d.texture.simple.dy * step;

    if (_y > ry) _y %= ry;
    if (_y < 0) _y += ry;

    fetcher->_d.texture.simple.py = _y;
  }

  static void FOG_FASTCALL skip_affine_pad_clamp(
    RasterPatternFetcher* fetcher, int step)
  {
    double s = (double)step;

    fetcher->_d.texture.affine.px += fetcher->_d.texture.affine.dx * s;
    fetcher->_d.texture.affine.py += fetcher->_d.texture.affine.dy * s;
  }

  static void FOG_FASTCALL skip_affine_repeat_reflect(
    RasterPatternFetcher* fetcher, int step)
  {
    double s = (double)step;

    fetcher->_d.texture.affine.px = Math::repeat(
      fetcher->_d.texture.affine.px + fetcher->_d.texture.affine.dx * s,
      fetcher->_ctx->_d.texture.affine.mx);
    fetcher->_d.texture.affine.py = Math::repeat(
      fetcher->_d.texture.affine.py + fetcher->_d.texture.affine.dy * s,
      fetcher->_ctx->_d.texture.affine.my);
  }
};

// ============================================================================
// [Fog::RasterC - PTextureAccessor - PRGB32]
// ============================================================================

struct FOG_NO_EXPORT PTextureAccessor_PRGB32_Base
{
  typedef uint32_t Pixel;
  enum { DST_BPP = 4 };

  FOG_INLINE void fetchNorm(Pixel& dst, const uint8_t* src) { Face::p32Load4a(dst, src); }
  FOG_INLINE void fetchSolid(Pixel& dst, const RasterSolid& solid) { dst = solid.prgb32.p32; }

  FOG_INLINE void interpolateNorm_2(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1)
  {
    P_INTERPOLATE_C_32_2(dst, c0, w0, c1, w1)
  }

  FOG_INLINE void interpolateNorm_4(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1, const Pixel& c2, uint w2, const Pixel& c3, uint w3)
  {
    P_INTERPOLATE_C_32_4(dst, c0, w0, c1, w1, c2, w2, c3, w3);
  }

  FOG_INLINE void store(uint8_t* dst, const Pixel& src) { Face::p32Store4a(dst, src); }
  FOG_INLINE uint8_t* fill(uint8_t* dst, const Pixel& src, int w) { return Helpers::p_fill_prgb32(dst, src, w); }
  FOG_INLINE uint8_t* repeat(uint8_t* dst, int baseW, int repeatW) { return Helpers::p_repeat_prgb32(dst, baseW, repeatW); }

  FOG_INLINE void _cmul(Pixel& dst, const Pixel& c0, uint u0) { Face::p32MulDiv256PBB_SBW(dst, c0, u0); }
  FOG_INLINE void _cadd(Pixel& dst, const Pixel& c0, const Pixel& c1) { Face::p32Add(dst, c0, c1); }
};

// ============================================================================
// [Fog::RasterC - PTextureAccessor - PRGB32 <- PRGB32]
// ============================================================================

struct FOG_NO_EXPORT PTextureAccessor_PRGB32_From_PRGB32 : public PTextureAccessor_PRGB32_Base
{
  enum { SRC_BPP = 4, FETCH_REFERENCE = 1 };

  FOG_INLINE PTextureAccessor_PRGB32_From_PRGB32(const RasterPattern* ctx) {}

  FOG_INLINE void fetchRaw(Pixel& dst, const uint8_t* src) { Face::p32Load4a(dst, src); }

  FOG_INLINE void interpolateRaw_2(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1)
  { P_INTERPOLATE_C_32_2(dst, c0, w0, c1, w1); }

  FOG_INLINE void interpolateRaw_4(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1, const Pixel& c2, uint w2, const Pixel& c3, uint w3)
  { P_INTERPOLATE_C_32_4(dst, c0, w0, c1, w1, c2, w2, c3, w3); }

  FOG_INLINE void normalize(Pixel& dst, const Pixel& src) { dst = src; }
};

// ============================================================================
// [Fog::RasterC - PTextureAccessor - PRGB32 <- XRGB32]
// ============================================================================

struct FOG_NO_EXPORT PTextureAccessor_PRGB32_From_XRGB32 : public PTextureAccessor_PRGB32_Base
{
  enum { SRC_BPP = 4, FETCH_REFERENCE = 0 };

  FOG_INLINE PTextureAccessor_PRGB32_From_XRGB32(const RasterPattern* ctx) {}

  FOG_INLINE void fetchRaw(Pixel& dst, const uint8_t* src) { Face::p32Load4a(dst, src); }

  FOG_INLINE void interpolateRaw_2(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1)
  { P_INTERPOLATE_C_32_2(dst, c0, w0, c1, w1); }

  FOG_INLINE void interpolateRaw_4(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1, const Pixel& c2, uint w2, const Pixel& c3, uint w3)
  { P_INTERPOLATE_C_32_4(dst, c0, w0, c1, w1, c2, w2, c3, w3); }

  FOG_INLINE void normalize(Pixel& dst, const Pixel& src) { Face::p32FillPBB3(dst, dst); }
};

// ============================================================================
// [Fog::RasterC - PTextureAccessor - PRGB32 <- RGB24]
// ============================================================================

struct FOG_NO_EXPORT PTextureAccessor_PRGB32_From_RGB24 : public PTextureAccessor_PRGB32_Base
{
  enum { SRC_BPP = 3, FETCH_REFERENCE = 0 };

  FOG_INLINE PTextureAccessor_PRGB32_From_RGB24(const RasterPattern* ctx) {}

  FOG_INLINE void fetchRaw(Pixel& dst, const uint8_t* src) { Face::p32Load3b(dst, src); }

  FOG_INLINE void interpolateRaw_2(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1)
  { P_INTERPOLATE_C_32_2(dst, c0, w0, c1, w1); }

  FOG_INLINE void interpolateRaw_4(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1, const Pixel& c2, uint w2, const Pixel& c3, uint w3)
  { P_INTERPOLATE_C_32_4(dst, c0, w0, c1, w1, c2, w2, c3, w3); }

  FOG_INLINE void normalize(Pixel& dst, const Pixel& src) { Face::p32FillPBB3(dst, dst); }
};

// ============================================================================
// [Fog::RasterC - PTextureAccessor - PRGB32 <- A8]
// ============================================================================

struct FOG_NO_EXPORT PTextureAccessor_PRGB32_From_A8 : public PTextureAccessor_PRGB32_Base
{
  enum { SRC_BPP = 1, FETCH_REFERENCE = 0 };

  FOG_INLINE PTextureAccessor_PRGB32_From_A8(const RasterPattern* ctx) {}

  FOG_INLINE void fetchRaw(Pixel& dst, const uint8_t* p) { Face::p32Load1b(dst, p); }

  FOG_INLINE void interpolateRaw_2(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1)
  { dst = (c0 * w0 + c1 * w1) >> 8; }

  FOG_INLINE void interpolateRaw_4(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1, const Pixel& c2, uint w2, const Pixel& c3, uint w3)
  { dst = (c0 * w0 + c1 * w1 + c2 * w2 + c3 * w3) >> 8; }

  FOG_INLINE void normalize(Pixel& dst, const Pixel& src) { Face::p32ExtendPBBFromSBB(dst, src); }
};

// ============================================================================
// [Fog::RasterC - PTextureAccessor - PRGB32 <- I8]
// ============================================================================

struct FOG_NO_EXPORT PTextureAccessor_PRGB32_From_I8 : public PTextureAccessor_PRGB32_Base
{
  enum { SRC_BPP = 1, FETCH_REFERENCE = 0 };

  FOG_INLINE PTextureAccessor_PRGB32_From_I8(const RasterPattern* ctx) : pal(ctx->_d.texture.base.pal) {}

  FOG_INLINE void fetchRaw(Pixel& dst, const uint8_t* src) { Face::p32Load4a(dst, pal + src[0]); }

  FOG_INLINE void interpolateRaw_2(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1)
  { P_INTERPOLATE_C_32_2(dst, c0, w0, c1, w1); }

  FOG_INLINE void interpolateRaw_4(Pixel& dst, const Pixel& c0, uint w0, const Pixel& c1, uint w1, const Pixel& c2, uint w2, const Pixel& c3, uint w3)
  { P_INTERPOLATE_C_32_4(dst, c0, w0, c1, w1, c2, w2, c3, w3); }

  FOG_INLINE void normalize(Pixel& dst, const Pixel& src) { dst = src; }

  const uint32_t* pal;
};

} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_TEXTUREBASE_P_H
