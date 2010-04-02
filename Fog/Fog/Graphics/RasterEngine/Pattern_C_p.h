// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Build/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Helpers]
// ============================================================================

#define MAX_INTERPOLATION_POINTS 4096

// For antialiasing tests use 16 points.
// #undef MAX_INTERPOLATION_POINTS
// #define MAX_INTERPOLATION_POINTS 16

#if FOG_ARCH_BITS > 32

#define PATTERN_C_INTERPOLATE_32_2(DST, SRC0, WEIGHT0, SRC1, WEIGHT1) \
{ \
  ByteUtil::byte1x4 pixT0; \
  ByteUtil::byte1x4 pixT1; \
  \
  ByteUtil::byte1x4_unpack_0213(pixT0, (SRC0)); \
  ByteUtil::byte1x4_unpack_0213(pixT1, (SRC1)); \
  \
  pixT0 *= (uint)(WEIGHT0); \
  pixT1 *= (uint)(WEIGHT1); \
  pixT0 += pixT1; \
  \
  pixT0 &= FOG_UINT64_C(0xFF00FF00FF00FF00); \
  \
  DST = (uint32_t)((pixT0 >> 8) | (pixT0 >> 32)); \
}

#define PATTERN_C_INTERPOLATE_32_2_WITH_ZERO(DST, SRC0, WEIGHT0) \
{ \
  ByteUtil::byte1x4 pixT0; \
  ByteUtil::byte1x4_unpack_0213(pixT0, (SRC0)); \
  pixT0 *= (uint)(WEIGHT0); \
  pixT0 &= FOG_UINT64_C(0xFF00FF00FF00FF00); \
  DST = (uint32_t)((pixT0 >> 8) | (pixT0 >> 32)); \
}

#define PATTERN_C_INTERPOLATE_32_4(DST, SRC0, WEIGHT0, SRC1, WEIGHT1, SRC2, WEIGHT2, SRC3, WEIGHT3) \
{ \
  ByteUtil::byte1x4 pixT0; \
  ByteUtil::byte1x4 pixT1; \
  ByteUtil::byte1x4 pixT2; \
  \
  ByteUtil::byte1x4_unpack_0213(pixT0, (SRC0)); \
  ByteUtil::byte1x4_unpack_0213(pixT1, (SRC1)); \
  \
  pixT0 *= (uint)(WEIGHT0); \
  pixT1 *= (uint)(WEIGHT1); \
  pixT0 += pixT1; \
  \
  ByteUtil::byte1x4_unpack_0213(pixT1, (SRC2)); \
  ByteUtil::byte1x4_unpack_0213(pixT2, (SRC3)); \
  \
  pixT1 *= (uint)(WEIGHT2); \
  pixT2 *= (uint)(WEIGHT3); \
  pixT0 += pixT1; \
  pixT0 += pixT2; \
  \
  pixT0 &= FOG_UINT64_C(0xFF00FF00FF00FF00); \
  \
  DST = (uint32_t)((pixT0 >> 8) | (pixT0 >> 32)); \
}

#else

#define PATTERN_C_INTERPOLATE_32_2(DST, SRC0, WEIGHT0, SRC1, WEIGHT1) \
{ \
  ByteUtil::byte1x2 pixT0_0, pixT0_1; \
  ByteUtil::byte1x2 pixT1_0, pixT1_1; \
  \
  uint __weight; \
  \
  ByteUtil::byte2x2_unpack_0213(pixT0_0, pixT0_1, (SRC0)); \
  ByteUtil::byte2x2_unpack_0213(pixT1_0, pixT1_1, (SRC1)); \
  \
  __weight = WEIGHT0; \
  pixT0_0 *= __weight; \
  pixT0_1 *= __weight; \
  \
  __weight = WEIGHT1; \
  pixT1_0 *= __weight; \
  pixT1_1 *= __weight; \
  \
  pixT0_0 += pixT1_0; \
  pixT0_1 += pixT1_1; \
  \
  pixT0_0 = (pixT0_0 >> 8) & 0x00FF00FF; \
  pixT0_1 = (pixT0_1     ) & 0xFF00FF00; \
  \
  DST = pixT0_0 | pixT0_1; \
}

#define PATTERN_C_INTERPOLATE_32_2_WITH_ZERO(DST, SRC0, WEIGHT0) \
{ \
  uint __weight; \
  \
  ByteUtil::byte1x2 pixT0_0, pixT0_1; \
  ByteUtil::byte2x2_unpack_0213(pixT0_0, pixT0_1, (SRC0)); \
  \
  __weight = WEIGHT0; \
  pixT0_0 *= __weight; \
  pixT0_1 *= __weight; \
  \
  pixT0_0 = (pixT0_0 >> 8) & 0x00FF00FF; \
  pixT0_1 = (pixT0_1     ) & 0xFF00FF00; \
  \
  DST = pixT0_0 | pixT0_1; \
}

#define PATTERN_C_INTERPOLATE_32_4(DST, SRC0, WEIGHT0, SRC1, WEIGHT1, SRC2, WEIGHT2, SRC3, WEIGHT3) \
{ \
  ByteUtil::byte1x2 pixT0_0, pixT0_1; \
  ByteUtil::byte1x2 pixT1_0, pixT1_1; \
  \
  uint __weight; \
  \
  ByteUtil::byte2x2_unpack_0213(pixT0_0, pixT0_1, (SRC0)); \
  ByteUtil::byte2x2_unpack_0213(pixT1_0, pixT1_1, (SRC1)); \
  \
  __weight = (WEIGHT0); \
  pixT0_0 *= __weight; \
  pixT0_1 *= __weight; \
  \
  __weight = (WEIGHT1); \
  pixT1_0 *= __weight; \
  pixT1_1 *= __weight; \
  \
  pixT0_0 += pixT1_0; \
  pixT0_1 += pixT1_1; \
  \
  ByteUtil::byte2x2_unpack_0213(pixT1_0, pixT1_1, (SRC2)); \
  \
  __weight = (WEIGHT2); \
  pixT1_0 *= __weight; \
  pixT1_1 *= __weight; \
  \
  pixT0_0 += pixT1_0; \
  pixT0_1 += pixT1_1; \
  \
  ByteUtil::byte2x2_unpack_0213(pixT1_0, pixT1_1, (SRC3)); \
  \
  __weight = (WEIGHT3); \
  pixT1_0 *= __weight; \
  pixT1_1 *= __weight; \
  \
  pixT0_0 += pixT1_0; \
  pixT0_1 += pixT1_1; \
  \
  pixT0_0 = (pixT0_0 >> 8) & 0x00FF00FF; \
  pixT0_1 = (pixT0_1     ) & 0xFF00FF00; \
  \
  DST = pixT0_0 | pixT0_1; \
}

#endif

// ============================================================================
// [Fog::RasterEngine::C - Fetch]
// ============================================================================

struct FOG_HIDDEN PF_XRGB32
{
  enum { BPP = 4, AS_IS = 1 };

  FOG_INLINE PF_XRGB32(const RasterEngine::PatternContext* ctx) {}
  FOG_INLINE uint32_t fetch(const uint8_t* p) { return READ_32(p) | 0xFF000000; }
};

struct FOG_HIDDEN PF_ARGB32
{
  enum { BPP = 4, AS_IS = 0 };

  FOG_INLINE PF_ARGB32(const RasterEngine::PatternContext* ctx) {}
  FOG_INLINE uint32_t fetch(const uint8_t* p) { return ArgbUtil::premultiply(READ_32(p)); }
};

struct FOG_HIDDEN PF_PRGB32
{
  enum { BPP = 4, AS_IS = 1 };

  FOG_INLINE PF_PRGB32(const RasterEngine::PatternContext* ctx) {}
  FOG_INLINE uint32_t fetch(const uint8_t* p) { return READ_32(p); }
};

struct FOG_HIDDEN PF_I8
{
  enum { BPP = 1, AS_IS = 0 };

  FOG_INLINE PF_I8(const RasterEngine::PatternContext* ctx) { pal = ctx->texture.pal; }
  FOG_INLINE uint32_t fetch(const uint8_t* p) { return pal[p[0]]; }

  const Argb* pal;
};

// ============================================================================
// [Fog::RasterEngine::C - Pattern]
// ============================================================================

struct FOG_HIDDEN PatternC
{
  // --------------------------------------------------------------------------
  // [Helpers - For making repeat patterns and fills]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL _do_repeat_32(uint8_t* dst, int linew, int w)
  {
    FOG_ASSERT(w > 0);

    uint8_t* base = dst - (linew * 4);

    uint8_t* srcCur = base;
    uint8_t* dstCur = dst;

    int i;

    for (;;)
    {
      i = Math::min(linew, w);
      w -= i;

      i -= 4;
      while (i >= 0)
      {
        Memory::copy16B(dstCur, srcCur);

        dstCur += 16;
        srcCur += 16;
        i -= 4;
      }
      i += 4;

      while (i)
      {
        ((uint32_t*)dstCur)[0] = READ_32(srcCur);
        dstCur += 4;
        srcCur += 4;
        i--;
      }
      if (!w) break;

      srcCur = base;
      linew <<= 1;
    }
  }

  static void FOG_FASTCALL _do_repeat_8(uint8_t* dst, int linew, int w)
  {
    FOG_ASSERT(w > 0);

    uint8_t* base = dst - (linew * 4);

    uint8_t* srcCur = base;
    uint8_t* dstCur = dst;

    int i;

    for (;;)
    {
      i = Math::min(linew, w);
      w -= i;

      memcpy(dstCur, srcCur, i);
      dstCur += i;
      if (!w) break;

      srcCur = base;
      linew <<= 1;
    }
  }

  static FOG_INLINE uint8_t* _do_fill_32(uint8_t* dstCur, uint32_t c0, int w)
  {
    FOG_ASSERT(w > 0);

    while ((w -= 4) >= 0)
    {
      ((uint32_t*)dstCur)[0] = c0;
      ((uint32_t*)dstCur)[1] = c0;
      ((uint32_t*)dstCur)[2] = c0;
      ((uint32_t*)dstCur)[3] = c0;

      dstCur += 16;
    }
    w += 4;

    while (w)
    {
      ((uint32_t*)dstCur)[0] = c0;
      dstCur += 4;
      w--;
    }

    return dstCur;
  }

  static FOG_INLINE uint8_t* _do_fill_8(uint8_t* dstCur, uint32_t c0, int w)
  {
    FOG_ASSERT(w > 0);

    memset(dstCur, (int)c0, (uint)w);
    return dstCur + w;
  }

  // --------------------------------------------------------------------------
  // [Solid]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL solid_init(
    PatternContext* ctx, uint32_t prgb)
  {
    // Solid fill is always using 32-bit format. We degrade to XRGB32 if alpha
    // channel is 0xFF (fully opaque).
    ctx->format = ArgbUtil::isAlpha0xFF(prgb)
      ? PIXEL_FORMAT_XRGB32
      : PIXEL_FORMAT_PRGB32;
    ctx->depth = 32;

    // TODO: What about solid.argb?
    ctx->solid.prgb = prgb;
    ctx->fetch = solid_fetch;
    ctx->destroy = solid_destroy;

    ctx->initialized = true;
    return ERR_OK;
  }

  static void FOG_FASTCALL solid_destroy(
    PatternContext* ctx)
  {
    FOG_ASSERT(ctx->initialized);
    ctx->initialized = false;
  }

  static uint8_t* FOG_FASTCALL solid_fetch(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;
    uint32_t c0 = ctx->solid.prgb;

    int i = w;
    do {
      ((uint32_t*)dstCur)[0] = c0;
      dstCur += 4;
    } while (--i);

    return dst;
  }

  // --------------------------------------------------------------------------
  // [Texture]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL texture_init(
    PatternContext* ctx,
    const Pattern& pattern,
    const DoubleMatrix& matrix,
    uint32_t interpolationType)
  {
    PatternData* d = pattern._d;

    // Only PATTERN_TEXTURE type pattern can be passed to texture_init().
    FOG_ASSERT(d->type == PATTERN_TEXTURE);

    // If texture is empty image we tread it as 0x00000000 solid color.
    if (d->obj.texture->isEmpty()) return functionMap->pattern.solid_init(ctx, 0x00000000);

    // Multiply pattern matrix with a given matrix (painter transformations).
    DoubleMatrix m(pattern._d->matrix.multiplied(matrix));

    // Call texture_init_blit() which will initialize the context.
    return texture_init_blit(ctx, d->obj.texture.instance(), d->data.texture->area, m, d->spread, interpolationType);
  }

  static err_t FOG_FASTCALL texture_init_blit(
    PatternContext* ctx,
    const Image& image, const IntRect& irect, 
    const DoubleMatrix& matrix,
    uint32_t spread, uint32_t interpolationType)
  {
    // Only valid images can be passed to texture_init_blit.
    FOG_ASSERT(!image.isEmpty());
    FOG_ASSERT((uint)spread < SPREAD_COUNT);

    uint32_t format = image.getFormat();
    bool isTransformed = (!Math::feq(matrix.sx, 1.0) || !Math::feq(matrix.shy, 0.0) ||
                          !Math::feq(matrix.sy, 1.0) || !Math::feq(matrix.shx, 0.0) );

    ctx->texture.texture.init(image);
    ctx->texture.pal = NULL;

    // Copy texture variables into pattern context.
    ctx->texture.bits = ctx->texture.texture->getScanline(irect.y);
    ctx->texture.stride = ctx->texture.texture->getStride();
    ctx->texture.w = irect.w;
    ctx->texture.h = irect.h;

    FOG_ASSERT(irect.w <= ctx->texture.texture->getWidth());
    FOG_ASSERT(irect.h <= ctx->texture.texture->getHeight());

    switch (format)
    {
      case PIXEL_FORMAT_PRGB32:
      case PIXEL_FORMAT_ARGB32:
      {
        ctx->texture.bits += irect.x * 4;
        ctx->format = PIXEL_FORMAT_PRGB32;
        ctx->depth = 32;
        break;
      }

      case PIXEL_FORMAT_XRGB32:
      {
        ctx->texture.bits += irect.x * 4;
        ctx->format = (spread == SPREAD_NONE)
          ? PIXEL_FORMAT_PRGB32
          : PIXEL_FORMAT_XRGB32;
        ctx->depth = 32;
        break;
      }

      case PIXEL_FORMAT_A8:
      {
        ctx->texture.bits += irect.x;
        ctx->format = PIXEL_FORMAT_A8;
        ctx->depth = 8;
        break;
      }

      case PIXEL_FORMAT_I8:
      {
        const Palette& pal = ctx->texture.texture->getPalette();
        ctx->texture.pal = pal.getData() + Palette::INDEX_PRGB32;

        ctx->texture.bits += irect.x;
        ctx->format = (pal.isAlphaUsed() && spread != SPREAD_NONE)
          ? PIXEL_FORMAT_PRGB32
          : PIXEL_FORMAT_XRGB32;
        ctx->depth = 32;
        break;
      }

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    ctx->fetch = NULL;

    if (!isTransformed)
    {
      int64_t startx = Math::doubleToFixed48x16(matrix.tx);
      int64_t starty = Math::doubleToFixed48x16(matrix.ty);

      uint fx = (int)(startx >> 8) & 0xFF;
      uint fy = (int)(starty >> 8) & 0xFF;
      bool exact = ((fx == 0) & (fy == 0));

      ctx->texture.dx = -(int)(startx >> 16);
      ctx->texture.dy = -(int)(starty >> 16);

      // if ((startx & 0xFF) >= 128 && ++fx == 256) { fx = 0; ctx->texture.dx--; }
      // if ((starty & 0xFF) >= 128 && ++fy == 256) { fy = 0; ctx->texture.dy--; }

      if (interpolationType == INTERPOLATION_NEAREST)
      {
        // TODO: Check if it's correct.
        if (fx < 128) ctx->texture.dx++;
        if (fy < 128) ctx->texture.dy++;
        exact = true;
      }

      if (exact)
      {
        // Shouldn't be needed.
        ctx->texture.fY0X0 = 0;
        ctx->texture.fY0X1 = 0;
        ctx->texture.fY1X0 = 0;
        ctx->texture.fY1X1 = 0;

        // Set fetch function.
        ctx->fetch = functionMap->pattern.texture_fetch_exact[(uint)format][(uint)spread];
      }
      else
      {
        // Calculate subpixel weights.
        //
        // +--+--+
        // |00|01| - Y0/X0 and Y0/X1
        // +--+--+
        // |11|11| - Y1/X0 and Y1/X1
        // +--+--+
        ctx->texture.fY0X0 = ((      fy) * (      fx)) >> 8;
        ctx->texture.fY0X1 = ((      fy) * (256 - fx)) >> 8;
        ctx->texture.fY1X0 = ((256 - fy) * (      fx)) >> 8;
        ctx->texture.fY1X1 = ((256 - fy) * (256 - fx)) >> 8;

        // Set fetch function.
        if (fx && fy)
          ctx->fetch = functionMap->pattern.texture_fetch_subxy[(uint)format][(uint)spread];
        else if (fx)
          ctx->fetch = functionMap->pattern.texture_fetch_subx0[(uint)format][(uint)spread];
        else if (fy)
          ctx->fetch = functionMap->pattern.texture_fetch_sub0y[(uint)format][(uint)spread];
        else
          FOG_ASSERT_NOT_REACHED();
      }
    }

    if (!ctx->fetch)
    {
      // Transform.
      matrix.inverted().saveTo(ctx->m);

      // Inner loop increments and bounds (16.16 fixed point).
      {
        int dx = Math::doubleToFixed16x16(ctx->m[MATRIX_SX]);
        int dy = Math::doubleToFixed16x16(ctx->m[MATRIX_SHY]);

        int fxmax = ctx->texture.w << 16;
        int fymax = ctx->texture.h << 16;

        if (spread == SPREAD_REFLECT)
        {
          fxmax *= 2;
          fymax *= 2;
        }

        // dx/dy can't be smaller than -fxmax/-fymax or larger than fxmax/fymax.
        if ((dx <= -fxmax) | (dx >= fxmax)) { dx %= fxmax; }
        if ((dy <= -fymax) | (dy >= fymax)) { dy %= fymax; }

        ctx->texture.dx = dx;
        ctx->texture.dy = dy;

        ctx->texture.fxmax = fxmax;
        ctx->texture.fymax = fymax;

        // Rewind is used when fx/fy exceeds fxmax/fymax.
        ctx->texture.fxrewind = (dx > 0) ? -fxmax : fxmax;
        ctx->texture.fyrewind = (dy > 0) ? -fymax : fymax;
      }

      // Set fetch function.
      if (interpolationType == INTERPOLATION_NEAREST)
        ctx->fetch = functionMap->pattern.texture_fetch_transform_nearest[(uint)format][(uint)spread];
      else
        ctx->fetch = functionMap->pattern.texture_fetch_transform_bilinear[(uint)format][(uint)spread];
    }

    // Set destroy function.
    ctx->destroy = texture_destroy;

    ctx->initialized = true;
    return ERR_OK;
  }

  static void FOG_FASTCALL texture_destroy(
    PatternContext* ctx)
  {
    FOG_ASSERT(ctx->initialized);

    ctx->texture.texture.destroy();
    ctx->initialized = false;
  }

  // --------------------------------------------------------------------------
  // [Texture - Exact]
  // --------------------------------------------------------------------------

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_exact_none_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if ((uint)y >= (uint)th)
    {
      _do_fill_32(dstCur, 0x00000000, w);
      return dst;
    }

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur = srcBase;

    int i;

    // Fetch before.
    if (x < 0)
    {
      i = Math::min(-x, w);
      x = 0;
      w -= i;

      dstCur = _do_fill_32(dstCur, 0x00000000, i);
      if (!w) return dst;
    }
    // Or return image buffer if span fits to it.
    else if (PF::AS_IS && x < tw && w < tw - x)
    {
      return const_cast<uint8_t*>(srcCur);
    }

    // Fetch texture.
    if (x < tw)
    {
      srcCur += (uint)x * PF::BPP;
      i = Math::min(tw - x, w);
      w -= i;

      do {
        ((uint32_t*)dstCur)[0] = pf.fetch(srcCur);
        dstCur += 4;
        srcCur += PF::BPP;
      } while (--i);
      if (!w) return dst;
    }

    // Fetch after.
    {
      dstCur = _do_fill_32(dstCur, 0x00000000, w);
    }

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_exact_pad_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if (y < 0) y = 0;
    if (y >= th) y = th - 1;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur = srcBase;

    int i;

    // Fetch before.
    if (x < 0)
    {
      i = Math::min(-x, w);
      x = 0;
      w -= i;

      uint32_t c0 = pf.fetch(srcCur);
      dstCur = _do_fill_32(dstCur, c0, i);
      if (!w) return dst;
    }
    // Or return image buffer if span fits to it.
    else if (PF::AS_IS && x < tw && w < tw - x)
    {
      return const_cast<uint8_t*>(srcCur);
    }

    // Fetch texture.
    if (x < tw)
    {
      srcCur += (uint)x * PF::BPP;
      i = Math::min(tw - x, w);
      w -= i;

      do {
        ((uint32_t*)dstCur)[0] = pf.fetch(srcCur);
        dstCur += 4;
        srcCur += PF::BPP;
      } while (--i);
      if (!w) return dst;
    }

    // Fetch after.
    {
      uint32_t c0 = pf.fetch(srcBase + (uint)tw * PF::BPP - PF::BPP);
      dstCur = _do_fill_32(dstCur, c0, w);
    }

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_exact_repeat_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if ((x >= tw) | (x <= -tw)) x %= tw;
    if ((y >= th) | (y <= -th)) y %= th;

    if (x < 0) x += tw;
    if (y < 0) y += th;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    int i;

    srcCur = srcBase + (uint)x * PF::BPP;

    // Return image buffer if span fits to it (this is very efficient
    // optimization for short spans or large textures).
    i = Math::min(tw - x, w);
    if (PF::AS_IS && w < tw - x)
      return const_cast<uint8_t*>(srcCur);

    for (;;)
    {
      w -= i;

      do {
        ((uint32_t*)dstCur)[0] = pf.fetch(srcCur);
        dstCur += 4;
        srcCur += PF::BPP;
      } while (--i);
      if (!w) break;

      i = Math::min(w, tw);
      srcCur = srcBase;
    }

    return dst;
  }

  static uint8_t* FOG_FASTCALL texture_fetch_exact_repeat_a8(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if ((x >= tw) | (x <= -tw)) x %= tw;
    if ((y >= th) | (y <= -th)) y %= th;

    if (x < 0) x += tw;
    if (y < 0) y += th;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    int i;

    srcCur = srcBase + x;

    // Return image buffer if span fits to it (this is very efficient
    // optimization for short spans or large textures).
    i = Math::min(tw - x, w);
    if (w < tw - x)
      return const_cast<uint8_t*>(srcCur);

    for (;;)
    {
      w -= i;

      if (i >= 8)
      {
        while ((sysuint_t)dstCur & 0x3)
        {
          *dstCur++ = *srcCur++;
          i--;
        }

        i -= 4;
        do {
          ((uint32_t*)dstCur)[0] = READ_32(srcCur);
          dstCur += 4;
          srcCur += 4;
          i -= 4;
        } while (i >= 0);
        i += 4;
      }

      while (i)
      {
        *dstCur++ = *srcCur++;
        i--;
      }
      if (!w) break;

      i = Math::min(w, tw);
      srcCur = srcBase;
    }

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_exact_reflect_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if ((x >= tw2) | (x <= -tw2)) x %= tw2;
    if ((y >= th2) | (y <= -th2)) y %= th2;

    if (x < 0) x += tw2;
    if (y < 0) y += th2;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th) y = th2 - y - 1;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    if (PF::AS_IS && x >= 0 && x <= tw && w < tw - x)
      return const_cast<uint8_t*>(srcBase + x * PF::BPP);

    do {
      // Reflect mode.
      if (x >= tw)
      {
        int i = Math::min(tw2 - x, w);
        srcCur = srcBase + (tw2 - x - 1) * PF::BPP;

        w -= i;
        x = 0;

        do {
          ((uint32_t*)dstCur)[0] = pf.fetch(srcCur);
          dstCur += 4;
          srcCur -= PF::BPP;
        } while (--i);
      }
      // Repeat mode.
      else
      {
        int i = Math::min(tw - x, w);

        srcCur = srcBase + x * PF::BPP;

        w -= i;
        x += i;

        do {
          ((uint32_t*)dstCur)[0] = pf.fetch(srcCur);
          dstCur += 4;
          srcCur += PF::BPP;
        } while (--i);
      }
    } while (w);

    return dst;
  }

  static uint8_t* FOG_FASTCALL texture_fetch_exact_reflect_a8(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if ((x >= tw2) | (x <= -tw2)) x %= tw2;
    if ((y >= th2) | (y <= -th2)) y %= th2;

    if (x < 0) x += tw2;
    if (y < 0) y += th2;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th) y = th2 - y - 1;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    if (x >= 0 && x <= tw && w < tw - x)
      return const_cast<uint8_t*>(srcBase + x);

    do {
      // Reflect mode.
      if (x >= tw)
      {
        int i = Math::min(tw2 - x, w);
        srcCur = srcBase + (tw2 - x - 1);

        w -= i;
        x = 0;

        if (i >= 8)
        {
          while ((sysuint_t)dstCur & 0x3)
          {
            *dstCur++ = *srcCur--;
            i--;
          }

          i -= 4;
          do {
            ((uint32_t*)dstCur)[0] = Memory::bswap32(READ_32(srcCur));
            dstCur += 4;
            srcCur -= 4;
            i -= 4;
          } while (i >= 0);
          i += 4;
        }

        while (i)
        {
          *dstCur++ = *srcCur--;
          i--;
        }
        if (!w) break;
      }
      // Repeat mode.
      else
      {
        int i = Math::min(tw - x, w);
        srcCur = srcBase + x;

        w -= i;
        x += i;

        if (i >= 8)
        {
          while ((sysuint_t)dstCur & 0x3)
          {
            *dstCur++ = *srcCur++;
            i--;
          }

          i -= 4;
          do {
            ((uint32_t*)dstCur)[0] = READ_32(srcCur);
            dstCur += 4;
            srcCur += 4;
            i -= 4;
          } while (i >= 0);
          i += 4;
        }

        while (i)
        {
          *dstCur++ = *srcCur++;
          i--;
        }
        if (!w) break;
      }
    } while (w);

    return dst;
  }

  // --------------------------------------------------------------------------
  // [Texture - SubXY]
  // --------------------------------------------------------------------------

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_subx0_none_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY1X0;
    uint fY0X1 = ctx->texture.fY1X1;

    y += ctx->texture.dy;
    x += ctx->texture.dx;

    if ((y < 0) | (y >= th))
    {
      _do_fill_32(dstCur, 0x00000000, w);
      return dst;
    }

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur = srcBase;

    uint32_t back_0 = 0x00000000;
    int i;

    // Fetch before.
    if (x < 0)
    {
      i = Math::min(-x, w);
      w -= i;
      x = 0;

      dstCur = _do_fill_32(dstCur, back_0, i);

      if (w) goto doFill;
      else return dst;
    }
    // Fetch texture.
    else if (x <= tw)
    {
      if (x > 0)
      {
        srcCur += (uint)x * PF::BPP;
        back_0 = pf.fetch(srcCur - PF::BPP);
      }

doFill:
      i = Math::min(tw - x, w);
      w -= i;

      while (i)
      {
        PATTERN_C_INTERPOLATE_32_2(
          ((uint32_t*)dstCur)[0],
          back_0                   , fY0X0,
          back_0 = pf.fetch(srcCur), fY0X1)

        dstCur += 4;
        srcCur += PF::BPP;
        i--;
      }

      // Interpolate last pixel on the row.
      if (!w) return dst;
      PATTERN_C_INTERPOLATE_32_2_WITH_ZERO(((uint32_t*)dstCur)[0], back_0, fY0X0)

      dstCur += 4;
      if (!--w) return dst;
    }

skipFill:
    // Fetch after.
    if (w) dstCur = _do_fill_32(dstCur, 0x00000000, w);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_subx0_pad_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY1X0;
    uint fY0X1 = ctx->texture.fY1X1;

    y += ctx->texture.dy;
    x += ctx->texture.dx - 1;

    if (y < 0) y = 0;
    else if (y >= th) y = th - 1;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur = srcBase;

    uint32_t back_0;
    int i;

    // Fetch before.
    if (x < 0)
    {
      i = Math::min(-x, w);
      w -= i;
      x = 1;

      back_0 = pf.fetch(srcCur);
      dstCur = _do_fill_32(dstCur, back_0, i);
      srcCur += PF::BPP;

      if (w) goto doFill;
      else return dst;
    }
    // Fetch texture.
    else if (x < tw)
    {
      srcCur += (uint)x * PF::BPP;

      back_0 = pf.fetch(srcCur);
      srcCur += PF::BPP;
      if (++x == tw) goto skipFill;

doFill:
      i = Math::min(tw - x, w);
      w -= i;

      do {
        PATTERN_C_INTERPOLATE_32_2(
          ((uint32_t*)dstCur)[0],
          back_0                   , fY0X0,
          back_0 = pf.fetch(srcCur), fY0X1);

        dstCur += 4;
        srcCur += PF::BPP;
      } while (--i);
      if (!w) return dst;
    }

skipFill:
    // Fetch after.
    if (w) dstCur = _do_fill_32(dstCur, pf.fetch(srcBase + (uint)tw * PF::BPP - PF::BPP), w);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_subx0_repeat_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY1X0;
    uint fY0X1 = ctx->texture.fY1X1;

    x += ctx->texture.dx - 1;
    y += ctx->texture.dy;

    if ((x >= tw) | (x <= -tw)) x %= tw;
    if ((y >= th) | (y <= -th)) y %= th;

    if (x < 0) x += tw;
    if (y < 0) y += th;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur = srcBase + (uint)x * PF::BPP;

    uint32_t back_0 = pf.fetch(srcCur);
    srcCur += PF::BPP;

    if (++x == tw)
    {
      x = 0;
      srcCur = srcBase;
    }

    int r = Math::min(w, tw);
    int i = Math::min(tw - x, r);

    w -= r;

    // Fetch only texture width line.
    for (;;)
    {
      r -= i;

      do {
        PATTERN_C_INTERPOLATE_32_2(
          ((uint32_t*)dstCur)[0],
          back_0                   , fY0X0,
          back_0 = pf.fetch(srcCur), fY0X1);

        dstCur += 4;
        srcCur += PF::BPP;
      } while (--i);
      if (!r) break;

      i = Math::min(r, tw);
      srcCur = srcBase;
    }

    // Fetch the rest.
    if (w) _do_repeat_32(dstCur, tw, w);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_subx0_reflect_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    uint fY0X0 = ctx->texture.fY1X0;
    uint fY0X1 = ctx->texture.fY1X1;

    x += ctx->texture.dx - 1;
    y += ctx->texture.dy;

    if ((x >= tw2) | (x <= -tw2)) x %= tw2;
    if ((y >= th2) | (y <= -th2)) y %= th2;

    if (x < 0) x += tw2;
    if (y < 0) y += th2;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th) y = th2 - y - 1;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcCur;

    uint32_t back_0;

    if (x >= tw)
    {
      // Reflect mode.
      srcCur = srcBase + (uint)(tw2 - x - 1) * PF::BPP;
    }
    else
    {
      // Repeat mode.
      srcCur = srcBase + (uint)x * PF::BPP;
    }

    back_0 = pf.fetch(srcCur);
    if (++x >= tw2) x -= tw2;

    int r = Math::min(w, tw2);
    w -= r;

    // Fetch only texture width line * 2.
    do {
      // Reflect mode.
      if (x >= tw)
      {
        int i = Math::min(tw2 - x, r);
        srcCur = srcBase + (tw2 - x - 1) * PF::BPP;

        r -= i;
        x = 0;

        do {
          PATTERN_C_INTERPOLATE_32_2(
            ((uint32_t*)dstCur)[0],
            back_0                   , fY0X0,
            back_0 = pf.fetch(srcCur), fY0X1);

          dstCur += 4;
          srcCur -= PF::BPP;
        } while (--i);
      }
      // Repeat mode.
      else
      {
        int i = Math::min(tw - x, r);
        srcCur = srcBase + (uint)x * PF::BPP;

        r -= i;
        x += i;

        do {
          PATTERN_C_INTERPOLATE_32_2(
            ((uint32_t*)dstCur)[0],
            back_0                   , fY0X0,
            back_0 = pf.fetch(srcCur), fY0X1);

          dstCur += 4;
          srcCur += PF::BPP;
        } while (--i);
      }
    } while (r);

    // Fetch the rest.
    if (w) _do_repeat_32(dstCur, tw2, w);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_sub0y_none_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X1;

    x += ctx->texture.dx;
    y += ctx->texture.dy - 1;

    if ((y < -1) | (y >= th))
    {
      _do_fill_32(dstCur, 0x00000000, w);
      return dst;
    }
    else if (y == -1 || y == th - 1)
    {
      // This is Y border case, we need to interpolate with zero.

      // Swap weight if (y == -1).
      if (y < 0) { fY0X0 = fY1X0; y++; }
      const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

      int i;

      // Fetch before.
      if (x < 0)
      {
        i = Math::min(-x, w);
        x = 0;
        w -= i;

        dstCur = _do_fill_32(dstCur, 0x00000000, i);
        if (!w) return dst;
      }

      // Fetch texture.
      if (x < tw)
      {
        const uint8_t* srcCur = srcBase + (uint)x * PF::BPP;

        i = Math::min(tw - x, w);
        w -= i;

        do {
          PATTERN_C_INTERPOLATE_32_2_WITH_ZERO(
            ((uint32_t*)dstCur)[0],
            pf.fetch(srcCur), fY0X0)

          dstCur += 4;
          srcCur += PF::BPP;
        } while (--i);
        if (!w) return dst;
      }

      // Fetch the rest.
      {
        dstCur = _do_fill_32(dstCur, 0x00000000, w);
      }

      return dst;
    }
    else
    {
      const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;

      int i;

      // Fetch before.
      if (x < 0)
      {
        i = Math::min(-x, w);
        x = 0;
        w -= i;

        dstCur = _do_fill_32(dstCur, 0x00000000, i);
        if (!w) return dst;
      }

      // Fetch texture.
      if (x < tw)
      {
        const uint8_t* srcCur0 = srcBase0 + (uint)x * PF::BPP;
        const uint8_t* srcCur1 = srcBase1 + (uint)x * PF::BPP;

        i = Math::min(tw - x, w);
        w -= i;

        do {
          PATTERN_C_INTERPOLATE_32_2(
            ((uint32_t*)dstCur)[0],
            pf.fetch(srcCur0), fY0X0,
            pf.fetch(srcCur1), fY1X0);

          dstCur += 4;
          srcCur0 += PF::BPP;
          srcCur1 += PF::BPP;
        } while (--i);
        if (!w) return dst;
      }

      // Fetch the rest.
      {
        dstCur = _do_fill_32(dstCur, 0x00000000, w);
      }

      return dst;
    }
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_sub0y_pad_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X1;

    x += ctx->texture.dx;
    y += ctx->texture.dy - 1;

    if (y < 0 || y >= th - 1)
    {
      // This exact fetch.
      if (y < 0) y = 0;
      else y = th - 1;

      const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

      int i;

      // Fetch before.
      if (x < 0)
      {
        i = Math::min(-x, w);
        x = 0;
        w -= i;

        uint32_t c0 = pf.fetch(srcBase);
        dstCur = _do_fill_32(dstCur, c0, i);
        if (!w) return dst;
      }
      // Or return image buffer if span fits to it.
      else if (x < tw && w < tw - x)
      {
        return const_cast<uint8_t*>(srcBase + (uint)x * PF::BPP);
      }

      // Fetch texture.
      if (x < tw)
      {
        const uint8_t* srcCur = srcBase + (uint)x * PF::BPP;
        i = Math::min(tw - x, w);
        w -= i;

        do {
          ((uint32_t*)dstCur)[0] = pf.fetch(srcCur);
          dstCur += 4;
          srcCur += PF::BPP;
        } while (--i);
        if (!w) return dst;
      }

      // Fetch after.
      {
        uint32_t c0 = pf.fetch(srcBase + (uint)tw * PF::BPP - PF::BPP);
        dstCur = _do_fill_32(dstCur, c0, w);
      }

      return dst;
    }
    else
    {
      const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;

      int i;

      // Fetch before.
      if (x < 0)
      {
        i = Math::min(-x, w);
        x = 0;
        w -= i;

        uint32_t c0;
        PATTERN_C_INTERPOLATE_32_2(c0,
          pf.fetch(srcBase0), fY0X0,
          pf.fetch(srcBase1), fY1X0);

        dstCur = _do_fill_32(dstCur, c0, i);
        if (!w) return dst;
      }

      // Fetch texture.
      if (x < tw)
      {
        const uint8_t* srcCur0 = srcBase0 + (uint)x * PF::BPP;
        const uint8_t* srcCur1 = srcBase1 + (uint)x * PF::BPP;

        i = Math::min(tw - x, w);
        w -= i;

        do {
          PATTERN_C_INTERPOLATE_32_2(
            ((uint32_t*)dstCur)[0],
            pf.fetch(srcCur0), fY0X0,
            pf.fetch(srcCur1), fY1X0);

          dstCur += 4;
          srcCur0 += PF::BPP;
          srcCur1 += PF::BPP;
        } while (--i);
        if (!w) return dst;
      }

      // Fetch the rest.
      {
        uint32_t c0;
        PATTERN_C_INTERPOLATE_32_2(c0,
          pf.fetch(srcBase0 + (uint)tw * PF::BPP - PF::BPP), fY0X0,
          pf.fetch(srcBase1 + (uint)tw * PF::BPP - PF::BPP), fY1X0);
        dstCur = _do_fill_32(dstCur, c0, w);
      }

      return dst;
    }
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_sub0y_repeat_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X1;

    x += ctx->texture.dx;
    y += ctx->texture.dy - 1;

    if ((x >= tw) | (x <= -tw)) x %= tw;
    if ((y >= th) | (y <= -th)) y %= th;

    if (x < 0) x += tw;
    if (y < 0) y += th;

    const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;
    if (y + 1 >= th) srcBase1 = ctx->texture.bits;

    const uint8_t* srcCur0 = srcBase0 + (uint)x * PF::BPP;
    const uint8_t* srcCur1 = srcBase1 + (uint)x * PF::BPP;

    int r = Math::min(w, tw);
    int i = Math::min(tw - x, r);

    w -= r;

    // Fetch only texture width line.
    for (;;)
    {
      r -= i;

      do {
        PATTERN_C_INTERPOLATE_32_2(
          ((uint32_t*)dstCur)[0],
          pf.fetch(srcCur0), fY0X0,
          pf.fetch(srcCur1), fY1X0);

        dstCur += 4;
        srcCur0 += PF::BPP;
        srcCur1 += PF::BPP;
      } while (--i);
      if (!r) break;

      i = Math::min(r, tw);
      srcCur0 = srcBase0;
      srcCur1 = srcBase1;
    }

    // Fetch the rest.
    if (w) _do_repeat_32(dstCur, tw, w);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_sub0y_reflect_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    uint fY0X0 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X1;

    x += ctx->texture.dx;
    y += ctx->texture.dy - 1;

    if ((x >= tw2) | (x <= -tw2)) x %= tw2;
    if ((y >= th2) | (y <= -th2)) y %= th2;

    if (x < 0) x += tw2;
    if (y < 0) y += th2;

    int y1 = y + 1;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th)
    {
      y = th2 - y - 1;

      y1 = y - 1;
      if (y1 < 0) y1 = 0;
    }
    else
    {
      if (y1 >= th) y1 = th2 - y1 - 1;
    }

    const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcBase1 = ctx->texture.bits + y1 * ctx->texture.stride;

    const uint8_t* srcCur0 = srcBase0 + (uint)x * PF::BPP;
    const uint8_t* srcCur1 = srcBase1 + (uint)x * PF::BPP;

    int r = Math::min(w, tw2);
    w -= r;

    // Fetch only texture width line * 2.
    do {
      // Reflect mode.
      if (x >= tw)
      {
        int i = Math::min(tw2 - x, r);

        srcCur0 = srcBase0 + (uint)(tw2 - x - 1) * PF::BPP;
        srcCur1 = srcBase1 + (uint)(tw2 - x - 1) * PF::BPP;

        r -= i;
        x = 0;

        do {
          PATTERN_C_INTERPOLATE_32_2(
            ((uint32_t*)dstCur)[0],
            pf.fetch(srcCur0), fY0X0,
            pf.fetch(srcCur1), fY1X0);

          dstCur += 4;
          srcCur0 -= PF::BPP;
          srcCur1 -= PF::BPP;
        } while (--i);
      }
      // Repeat mode.
      else
      {
        int i = Math::min(tw - x, r);

        srcCur0 = srcBase0 + (uint)x * PF::BPP;
        srcCur1 = srcBase1 + (uint)x * PF::BPP;

        r -= i;
        x += i;

        do {
          PATTERN_C_INTERPOLATE_32_2(
            ((uint32_t*)dstCur)[0],
            pf.fetch(srcCur0), fY0X0,
            pf.fetch(srcCur1), fY1X0);

          dstCur += 4;
          srcCur0 += PF::BPP;
          srcCur1 += PF::BPP;
        } while (--i);
      }
    } while (r);

    // Fetch the rest.
    if (w) _do_repeat_32(dstCur, tw2, w);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_subxy_none_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X0;
    uint fY0X1 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X0;
    uint fY1X1 = ctx->texture.fY1X1;

    y += ctx->texture.dy - 1;
    x += ctx->texture.dx;

    if ((y < -1) | (y >= th))
    {
      _do_fill_32(dstCur, 0x00000000, w);
      return dst;
    }
    else if ((y < 0) | (y >= th - 1))
    {
      if (y < 0) { y++; fY0X0 = fY1X0; fY0X1 = fY1X1; }

      const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcCur = srcBase;

      uint32_t back_0 = 0x00000000;
      int i;

      // Fetch before.
      if (x < 0)
      {
        i = Math::min(-x, w);
        w -= i;
        x = 0;

        dstCur = _do_fill_32(dstCur, back_0, i);

        if (w) goto doFill_2;
        else return dst;
      }
      // Fetch texture.
      else if (x <= tw)
      {
        if (x > 0)
        {
          srcCur += (uint)x * PF::BPP;
          back_0 = pf.fetch(srcCur - PF::BPP);
        }

doFill_2:
        i = Math::min(tw - x, w);
        w -= i;

        while (i)
        {
          PATTERN_C_INTERPOLATE_32_2(
            ((uint32_t*)dstCur)[0],
            back_0                   , fY0X0,
            back_0 = pf.fetch(srcCur), fY0X1)

          dstCur += 4;
          srcCur += PF::BPP;
          i--;
        }

        // Interpolate last pixel on the row.
        if (!w) return dst;
        PATTERN_C_INTERPOLATE_32_2_WITH_ZERO(((uint32_t*)dstCur)[0], back_0, fY0X0)

        dstCur += 4;
        if (!--w) return dst;
      }

      // Fetch after.
      dstCur = _do_fill_32(dstCur, 0x00000000, w);

      return dst;
    }
    else
    {
      if (y < 0) { y++; fY0X0 = fY1X0; fY0X1 = fY1X1; }

      const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcCur0 = srcBase;
      const uint8_t* srcCur1 = srcBase + ctx->texture.stride;

      uint32_t back_0 = 0x00000000;
      uint32_t back_1 = 0x00000000;
      int i;

      // Fetch before.
      if (x < 0)
      {
        i = Math::min(-x, w);
        w -= i;
        x = 0;

        dstCur = _do_fill_32(dstCur, back_0, i);

        if (w) goto doFill_4;
        else return dst;
      }
      // Fetch texture.
      else if (x <= tw)
      {
        if (x > 0)
        {
          srcCur0 += x * PF::BPP;
          srcCur1 += x * PF::BPP;

          back_0 = pf.fetch(srcCur0 - PF::BPP);
          back_1 = pf.fetch(srcCur1 - PF::BPP);
        }

doFill_4:
        i = Math::min(tw - x, w);
        w -= i;

        while (i)
        {
          PATTERN_C_INTERPOLATE_32_4(
            ((uint32_t*)dstCur)[0],
            back_0                    , fY0X0,
            back_0 = pf.fetch(srcCur0), fY0X1,
            back_1                    , fY1X0,
            back_1 = pf.fetch(srcCur1), fY1X1)

          dstCur += 4;
          srcCur0 += PF::BPP;
          srcCur1 += PF::BPP;
          i--;
        }

        // Interpolate last pixel on the row.
        if (!w) return dst;
        PATTERN_C_INTERPOLATE_32_2(
          ((uint32_t*)dstCur)[0],
          back_0, fY0X0,
          back_1, fY1X0)

        dstCur += 4;
        if (!--w) return dst;
      }

      // Fetch after.
      dstCur = _do_fill_32(dstCur, 0x00000000, w);

      return dst;
    }
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_subxy_pad_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X0;
    uint fY0X1 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X0;
    uint fY1X1 = ctx->texture.fY1X1;

    x += ctx->texture.dx - 1;
    y += ctx->texture.dy - 1;

    if ((y < 0) | (y >= th - 1))
    {
      y = (y < 0) ? 0 : th - 1;

      fY0X0 += fY1X0;
      fY0X1 += fY1X1;

      const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcCur = srcBase;

      uint32_t back_0;
      int i;

      // Fetch before.
      if (x < 0)
      {
        i = Math::min(-x, w);
        w -= i;
        x = 1;

        back_0 = pf.fetch(srcCur);
        dstCur = _do_fill_32(dstCur, back_0, i);
        srcCur += PF::BPP;

        if (w) goto doFill_2;
        else return dst;
      }
      // Fetch texture.
      else if (x < tw)
      {
        srcCur += (uint)x * PF::BPP;

        back_0 = pf.fetch(srcCur);
        srcCur += PF::BPP;
        if (++x == tw) goto skipFill_2;

doFill_2:
        i = Math::min(tw - x, w);
        w -= i;

        do {
          PATTERN_C_INTERPOLATE_32_2(
            ((uint32_t*)dstCur)[0],
            back_0                   , fY0X0,
            back_0 = pf.fetch(srcCur), fY0X1);

          dstCur += 4;
          srcCur += PF::BPP;
        } while (--i);
        if (!w) return dst;
      }

skipFill_2:
      // Fetch after.
      if (w) dstCur = _do_fill_32(dstCur, pf.fetch(srcBase + (uint)tw * PF::BPP - PF::BPP), w);

      return dst;
    }
    else
    {
      const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;

      const uint8_t* srcCur0 = srcBase0;
      const uint8_t* srcCur1 = srcBase1;

      int i;
      uint32_t back_0;
      uint32_t back_1;

      if (x < 0)
      {
        i = Math::min(-x, w);
        x = 0;
        w -= i;

        back_0 = pf.fetch(srcBase0);
        back_1 = pf.fetch(srcBase1);

        uint32_t c0;
        PATTERN_C_INTERPOLATE_32_2(c0, back_0, fY0X0 + fY0X1, back_1, fY1X0 + fY1X1);
        dstCur = _do_fill_32(dstCur, c0, i);
        if (!w) return dst;
        else goto doFill_4;
      }
      else if (x < tw - 1)
      {
        srcCur0 += (uint)x * PF::BPP;
        srcCur1 += (uint)x * PF::BPP;

        back_0 = pf.fetch(srcCur0);
        back_1 = pf.fetch(srcCur1);
doFill_4:
        srcCur0 += PF::BPP;
        srcCur1 += PF::BPP;

        i = Math::min(tw - 1 - x, w);
        w -= i;

        do {
          PATTERN_C_INTERPOLATE_32_4(((uint32_t*)dstCur)[0],
            back_0, fY0X0,
            back_1, fY1X0,
            back_0 = pf.fetch(srcCur0), fY0X1,
            back_1 = pf.fetch(srcCur1), fY1X1);

          dstCur += 4;
          srcCur0 += PF::BPP;
          srcCur1 += PF::BPP;
        } while (--i);
        if (!w) return dst;
      }
      else
      {
        back_0 = pf.fetch(srcBase0 + (uint)tw * PF::BPP - PF::BPP);
        back_1 = pf.fetch(srcBase1 + (uint)tw * PF::BPP - PF::BPP);
      }

      // Fetch the rest.
      {
        uint32_t c0;
        PATTERN_C_INTERPOLATE_32_2(c0, back_0, fY0X0 + fY0X1, back_1, fY1X0 + fY1X1)
        _do_fill_32(dstCur, c0, w);
      }
    }

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_subxy_repeat_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X0;
    uint fY0X1 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X0;
    uint fY1X1 = ctx->texture.fY1X1;

    x += ctx->texture.dx - 1;
    y += ctx->texture.dy - 1;

    if ((x >= tw) | (x <= -tw)) x %= tw;
    if ((y >= th) | (y <= -th)) y %= th;

    if (x < 0) x += tw;
    if (y < 0) y += th;

    const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;
    if (y + 1 >= th) srcBase1 = ctx->texture.bits;

    const uint8_t* srcCur0 = srcBase0 + (uint)x * PF::BPP;
    const uint8_t* srcCur1 = srcBase1 + (uint)x * PF::BPP;

    uint32_t back_0 = pf.fetch(srcCur0);
    uint32_t back_1 = pf.fetch(srcCur1);

    srcCur0 += PF::BPP;
    srcCur1 += PF::BPP;

    if (++x == tw)
    {
      x = 0;
      srcCur0 = srcBase0;
      srcCur1 = srcBase1;
    }

    int r = Math::min(w, tw);
    int i = Math::min(tw - x, r);

    w -= r;

    // Fetch only texture width line.
    for (;;)
    {
      r -= i;

      do {
        PATTERN_C_INTERPOLATE_32_4(((uint32_t*)dstCur)[0],
          back_0, fY0X0,
          back_1, fY1X0,
          back_0 = pf.fetch(srcCur0), fY0X1,
          back_1 = pf.fetch(srcCur1), fY1X1);

        dstCur += 4;
        srcCur0 += PF::BPP;
        srcCur1 += PF::BPP;
      } while (--i);
      if (!r) break;

      i = Math::min(r, tw);
      srcCur0 = srcBase0;
      srcCur1 = srcBase1;
    }

    // Fetch the rest.
    if (w) _do_repeat_32(dstCur, tw, w);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_subxy_reflect_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    uint fY0X0 = ctx->texture.fY0X0;
    uint fY0X1 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X0;
    uint fY1X1 = ctx->texture.fY1X1;

    x += ctx->texture.dx - 1;
    y += ctx->texture.dy - 1;

    if ((x >= tw2) | (x <= -tw2)) x %= tw2;
    if ((y >= th2) | (y <= -th2)) y %= th2;

    if (x < 0) x += tw2;
    if (y < 0) y += th2;

    int y1 = y + 1;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th)
    {
      y = th2 - y - 1;

      y1 = y - 1;
      if (y1 < 0) y1 = 0;
    }
    else
    {
      if (y1 >= th) y1 = th2 - y1 - 1;
    }

    const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcBase1 = ctx->texture.bits + y1 * ctx->texture.stride;

    const uint8_t* srcCur0;
    const uint8_t* srcCur1;

    uint32_t back_0;
    uint32_t back_1;

    if (x >= tw)
    {
      // Reflect mode.
      srcCur0 = srcBase0 + (uint)(tw2 - x - 1) * PF::BPP;
      srcCur1 = srcBase1 + (uint)(tw2 - x - 1) * PF::BPP;
    }
    else
    {
      // Repeat mode.
      srcCur0 = srcBase0 + (uint)x * PF::BPP;
      srcCur1 = srcBase1 + (uint)x * PF::BPP;
    }

    back_0 = pf.fetch(srcCur0);
    back_1 = pf.fetch(srcCur1);

    if (++x >= tw2) x -= tw2;

    int r = Math::min(w, tw2);
    w -= r;

    // Fetch only texture width line * 2.
    do {
      // Reflect mode.
      if (x >= tw)
      {
        int i = Math::min(tw2 - x, r);

        srcCur0 = srcBase0 + (uint)(tw2 - x - 1) * PF::BPP;
        srcCur1 = srcBase1 + (uint)(tw2 - x - 1) * PF::BPP;

        r -= i;
        x = 0;

        do {
          PATTERN_C_INTERPOLATE_32_4(((uint32_t*)dstCur)[0],
            back_0, fY0X0,
            back_1, fY1X0,
            back_0 = pf.fetch(srcCur0), fY0X1,
            back_1 = pf.fetch(srcCur1), fY1X1);

          dstCur += 4;
          srcCur0 -= PF::BPP;
          srcCur1 -= PF::BPP;
        } while (--i);
      }
      // Repeat mode.
      else
      {
        int i = Math::min(tw - x, r);

        srcCur0 = srcBase0 + (uint)x * PF::BPP;
        srcCur1 = srcBase1 + (uint)x * PF::BPP;

        r -= i;
        x += i;

        do {
          PATTERN_C_INTERPOLATE_32_4(((uint32_t*)dstCur)[0],
            back_0, fY0X0,
            back_1, fY1X0,
            back_0 = pf.fetch(srcCur0), fY0X1,
            back_1 = pf.fetch(srcCur1), fY1X1);

          dstCur += 4;
          srcCur0 += PF::BPP;
          srcCur1 += PF::BPP;
        } while (--i);
      }
    } while (r);

    // Fetch the rest.
    if (w) _do_repeat_32(dstCur, tw2, w);

    return dst;
  }

  // --------------------------------------------------------------------------
  // [Texture - Transform - Nearest]
  // --------------------------------------------------------------------------

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_nearest_none_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    fx -= 0x8000;
    fy -= 0x8000;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;

    do {
      uint32_t c0 = 0x00000000;
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      if (FOG_LIKELY(((uint)px0 < (uint)tw) & ((uint)py0 < (uint)th)))
      {
        c0 = pf.fetch(srcBits + (uint)py0 * srcStride + (uint)px0 * PF::BPP);
      }

      ((uint32_t*)dstCur)[0] = c0;
      dstCur += 4;

      fx += dx;
      fy += dy;
    } while (--i);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_nearest_pad_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    fx -= 0x8000;
    fy -= 0x8000;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;

    tw--;
    th--;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      if (px0 < 0) px0 = 0; else if (px0 > tw) px0 = tw;
      if (py0 < 0) py0 = 0; else if (py0 > th) py0 = th;

      ((uint32_t*)dstCur)[0] = pf.fetch(srcBits + (uint)py0 * srcStride + (uint)px0 * PF::BPP);
      dstCur += 4;

      fx += dx;
      fy += dy;
    } while (--i);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_nearest_repeat_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fxmax = ctx->texture.fxmax;
    int fymax = ctx->texture.fymax;

    int fxrewind = ctx->texture.fxrewind;
    int fyrewind = ctx->texture.fyrewind;

    fx -= 0x8000;
    fy -= 0x8000;

    if (fx <= -fxmax || fx >= fxmax) fx %= fxmax;
    if (fy <= -fymax || fy >= fymax) fy %= fymax;

    if (fx < 0) fx += fxmax;
    if (fy < 0) fy += fymax;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;
    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      ((uint32_t*)dstCur)[0] = pf.fetch(srcBits + (uint)py0 * srcStride + (uint)px0 * PF::BPP);
      dstCur += 4;

      fx += dx;
      fy += dy;

      if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
      if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
    } while (--i);

    return dst;
  }

  static uint8_t* FOG_FASTCALL texture_fetch_transform_nearest_repeat_a8(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fxmax = ctx->texture.fxmax;
    int fymax = ctx->texture.fymax;

    int fxrewind = ctx->texture.fxrewind;
    int fyrewind = ctx->texture.fyrewind;

    fx -= 0x8000;
    fy -= 0x8000;

    if ((fx <= -fxmax) | (fx >= fxmax)) fx %= fxmax;
    if ((fy <= -fymax) | (fy >= fymax)) fy %= fymax;

    if (fx < 0) fx += fxmax;
    if (fy < 0) fy += fymax;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;
    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      dstCur[0] = READ_8(srcBits + (uint)py0 * srcStride + (uint)px0);
      dstCur += 1;

      fx += dx;
      fy += dy;

      if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
      if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
    } while (--i);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_nearest_reflect_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fxmax = ctx->texture.fxmax;
    int fymax = ctx->texture.fymax;

    int fxrewind = ctx->texture.fxrewind;
    int fyrewind = ctx->texture.fyrewind;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw * 2 - 1;
    int th2 = th * 2 - 1;

    fx -= 0x8000;
    fy -= 0x8000;

    if (fx <= -fxmax || fx >= fxmax) fx %= fxmax;
    if (fy <= -fymax || fy >= fymax) fy %= fymax;

    if (fx < 0) fx += fxmax;
    if (fy < 0) fy += fymax;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;
    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      if (px0 >= tw) px0 = tw2 - px0;
      if (py0 >= th) py0 = th2 - py0;

      ((uint32_t*)dstCur)[0] = pf.fetch(srcBits + (uint)py0 * srcStride + (uint)px0 * PF::BPP);
      dstCur += 4;

      fx += dx;
      fy += dy;

      if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
      if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
    } while (--i);

    return dst;
  }

  // --------------------------------------------------------------------------
  // [Texture - Transform - Bilinear]
  // --------------------------------------------------------------------------

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_none_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    fx -= 0x8000;
    fy -= 0x8000;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;

    tw--;
    th--;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      uint32_t pix_x0y0;
      uint32_t pix_x1y0;
      uint32_t pix_x0y1;
      uint32_t pix_x1y1;

      if (FOG_UNLIKELY(((uint)px0 >= (uint)tw) | ((uint)py0 >= (uint)th)))
      {
        int px1 = px0 + 1;

        if (((uint)py0 <= (uint)th))
        {
          const uint8_t* src0 = srcBits + py0 * srcStride;

          pix_x0y0 = (((uint)px0 <= (uint)tw)) ? pf.fetch(src0 + (uint)px0 * PF::BPP) : 0x00000000;
          pix_x1y0 = (((uint)px1 <= (uint)tw)) ? pf.fetch(src0 + (uint)px1 * PF::BPP) : 0x00000000;
        }
        else
        {
          pix_x0y0 = 0x00000000;
          pix_x1y0 = 0x00000000;
        }

        py0++;
        if (((uint)py0 <= (uint)th))
        {
          const uint8_t* src0 = srcBits + py0 * srcStride;

          pix_x0y1 = (((uint)px0 <= (uint)tw)) ? pf.fetch(src0 + (uint)px0 * PF::BPP) : 0x00000000;
          pix_x1y1 = (((uint)px1 <= (uint)tw)) ? pf.fetch(src0 + (uint)px1 * PF::BPP) : 0x00000000;
        }
        else
        {
          pix_x0y1 = 0x00000000;
          pix_x1y1 = 0x00000000;
        }
      }
      else
      {
        const uint8_t* src0 = srcBits + py0 * srcStride + px0 * PF::BPP;

        pix_x0y0 = pf.fetch(src0          );
        pix_x1y0 = pf.fetch(src0 + PF::BPP);
        src0 += srcStride;
        pix_x0y1 = pf.fetch(src0          );
        pix_x1y1 = pf.fetch(src0 + PF::BPP);
      }

      uint weightx = (fx >> 8) & 0xFF;
      uint weighty = (fy >> 8) & 0xFF;

      PATTERN_C_INTERPOLATE_32_4(((uint32_t*)dstCur)[0],
        pix_x0y0, ((0x100 - weightx) * (0x100 - weighty)) >> 8,
        pix_x1y0, ((weightx        ) * (0x100 - weighty)) >> 8,
        pix_x0y1, ((0x100 - weightx) * (weighty        )) >> 8,
        pix_x1y1, ((weightx        ) * (weighty        )) >> 8);
      dstCur += 4;

      fx += dx;
      fy += dy;
    } while (--i);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_pad_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    fx -= 0x8000;
    fy -= 0x8000;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;

    tw--;
    th--;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      uint32_t pix_x0y0;
      uint32_t pix_x1y0;
      uint32_t pix_x0y1;
      uint32_t pix_x1y1;

      if (FOG_UNLIKELY(((uint)px0 >= (uint)tw) | ((uint)py0 >= (uint)th)))
      {
        int px1 = px0 + 1;
        int py1 = py0 + 1;

        if (px0 < 0) { px0 = px1 = 0; } else if (px0 >= tw) { px0 = px1 = tw; }
        if (py0 < 0) { py0 = py1 = 0; } else if (py0 >= th) { py0 = py1 = th; }

        const uint8_t* src0 = srcBits + (uint)py0 * srcStride;
        const uint8_t* src1 = srcBits + (uint)py1 * srcStride;

        pix_x0y0 = pf.fetch(src0 + (uint)px0 * PF::BPP);
        pix_x1y0 = pf.fetch(src0 + (uint)px1 * PF::BPP);
        pix_x0y1 = pf.fetch(src1 + (uint)px0 * PF::BPP);
        pix_x1y1 = pf.fetch(src1 + (uint)px1 * PF::BPP);
      }
      else
      {
        const uint8_t* src0 = srcBits + py0 * srcStride + (uint)px0 * PF::BPP;

        pix_x0y0 = pf.fetch(src0          );
        pix_x1y0 = pf.fetch(src0 + PF::BPP);
        src0 += srcStride;
        pix_x0y1 = pf.fetch(src0          );
        pix_x1y1 = pf.fetch(src0 + PF::BPP);
      }

      uint weightx = (fx >> 8) & 0xFF;
      uint weighty = (fy >> 8) & 0xFF;

      PATTERN_C_INTERPOLATE_32_4(((uint32_t*)dstCur)[0],
        pix_x0y0, ((0x100 - weightx) * (0x100 - weighty)) >> 8,
        pix_x1y0, ((weightx        ) * (0x100 - weighty)) >> 8,
        pix_x0y1, ((0x100 - weightx) * (weighty        )) >> 8,
        pix_x1y1, ((weightx        ) * (weighty        )) >> 8);
      dstCur += 4;

      fx += dx;
      fy += dy;
    } while (--i);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_repeat_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    int fxmax = ctx->texture.fxmax;
    int fymax = ctx->texture.fymax;

    int fxrewind = ctx->texture.fxrewind;
    int fyrewind = ctx->texture.fyrewind;

    fx -= 0x8000;
    fy -= 0x8000;

    if ((fx <= -fxmax) | (fx >= fxmax)) fx %= fxmax;
    if ((fy <= -fymax) | (fy >= fymax)) fy %= fymax;

    if (fx < 0) fx += fxmax;
    if (fy < 0) fy += fymax;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      int px1 = px0 + 1;
      int py1 = py0 + 1;

      uint32_t pix_x0y0;
      uint32_t pix_x1y0;
      uint32_t pix_x0y1;
      uint32_t pix_x1y1;

      if (FOG_UNLIKELY(py1 >= th)) py1 -= th;
      if (FOG_UNLIKELY(px1 >= tw)) px1 -= tw;

      const uint8_t* src0 = srcBits + (uint)py0 * srcStride;
      const uint8_t* src1 = srcBits + (uint)py1 * srcStride;

      uint weightx = (fx >> 8) & 0xFF;
      uint weighty = (fy >> 8) & 0xFF;

      pix_x0y0 = pf.fetch(src0 + (uint)px0 * PF::BPP);
      pix_x1y0 = pf.fetch(src0 + (uint)px1 * PF::BPP);
      pix_x0y1 = pf.fetch(src1 + (uint)px0 * PF::BPP);
      pix_x1y1 = pf.fetch(src1 + (uint)px1 * PF::BPP);

      PATTERN_C_INTERPOLATE_32_4(((uint32_t*)dstCur)[0],
        pix_x0y0, ((0x100 - weightx) * (0x100 - weighty)) >> 8,
        pix_x1y0, ((weightx        ) * (0x100 - weighty)) >> 8,
        pix_x0y1, ((0x100 - weightx) * (weighty        )) >> 8,
        pix_x1y1, ((weightx        ) * (weighty        )) >> 8);
      dstCur += 4;

      fx += dx;
      fy += dy;

      if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
      if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
    } while (--i);

    return dst;
  }

  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_repeat_a8(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    int fxmax = ctx->texture.fxmax;
    int fymax = ctx->texture.fymax;

    int fxrewind = ctx->texture.fxrewind;
    int fyrewind = ctx->texture.fyrewind;

    fx -= 0x8000;
    fy -= 0x8000;

    if ((fx <= -fxmax) | (fx >= fxmax)) fx %= fxmax;
    if ((fy <= -fymax) | (fy >= fymax)) fy %= fymax;

    if (fx < 0) fx += fxmax;
    if (fy < 0) fy += fymax;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      int px1 = px0 + 1;
      int py1 = py0 + 1;

      uint32_t pix_y0;
      uint32_t pix_y1;

      if (FOG_UNLIKELY(py1 >= th)) py1 -= th;
      if (FOG_UNLIKELY(px1 >= tw)) px1 -= tw;

      const uint8_t* src0 = srcBits + (uint)py0 * srcStride;
      const uint8_t* src1 = srcBits + (uint)py1 * srcStride;

      uint weightx = (fx >> 8) & 0xFF;
      uint weighty = (fy >> 8) & 0xFF;

      uint weightinvx = 256 - weightx;
      uint weightinvy = 256 - weighty;

      pix_y0 = ((uint)src0[px0] * weightinvx) + ((uint)src0[px1] * weightx);
      pix_y1 = ((uint)src1[px0] * weightinvx) + ((uint)src1[px1] * weightx);

      pix_y0 *= weightinvy;
      pix_y1 *= weighty;

      dstCur[0] = (uint8_t)((pix_y0 + pix_y1) >> 16);

      dstCur += 1;

      fx += dx;
      fy += dy;

      if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
      if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
    } while (--i);

    return dst;
  }

  template<typename PF>
  static uint8_t* FOG_FASTCALL texture_fetch_transform_bilinear_reflect_32(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    PF pf(ctx);
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw * 2 - 1;
    int th2 = th * 2 - 1;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + ry * ctx->m[MATRIX_SHX] + ctx->m[MATRIX_TX]);
    int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] + ry * ctx->m[MATRIX_SY ] + ctx->m[MATRIX_TY]);

    int fxmax = ctx->texture.fxmax;
    int fymax = ctx->texture.fymax;

    int fxrewind = ctx->texture.fxrewind;
    int fyrewind = ctx->texture.fyrewind;

    fx -= 0x8000;
    fy -= 0x8000;

    if ((fx <= -fxmax) | (fx >= fxmax)) fx %= fxmax;
    if ((fy <= -fymax) | (fy >= fymax)) fy %= fymax;

    if (fx < 0) fx += fxmax;
    if (fy < 0) fy += fymax;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    int i = w;

    do {
      int px0 = fx >> 16;
      int py0 = fy >> 16;

      int px1 = px0 + 1;
      int py1 = py0 + 1;

      if (FOG_UNLIKELY(px0 >= tw)) px0 = tw2 - px0;
      if (FOG_UNLIKELY(py0 >= th)) py0 = th2 - py0;
      if (FOG_UNLIKELY(px1 >= tw)) { px1 = tw2 - px1; if (FOG_UNLIKELY(px1 < 0)) px1 = 0; }
      if (FOG_UNLIKELY(py1 >= th)) { py1 = th2 - py1; if (FOG_UNLIKELY(py1 < 0)) py1 = 0; }

      const uint8_t* src0 = srcBits + (uint)py0 * srcStride;
      const uint8_t* src1 = srcBits + (uint)py1 * srcStride;

      uint weightx = (fx >> 8) & 0xFF;
      uint weighty = (fy >> 8) & 0xFF;

      uint32_t pix_x0y0 = pf.fetch(src0 + (uint)px0 * PF::BPP);
      uint32_t pix_x1y0 = pf.fetch(src0 + (uint)px1 * PF::BPP);
      uint32_t pix_x0y1 = pf.fetch(src1 + (uint)px0 * PF::BPP);
      uint32_t pix_x1y1 = pf.fetch(src1 + (uint)px1 * PF::BPP);

      PATTERN_C_INTERPOLATE_32_4(((uint32_t*)dstCur)[0],
        pix_x0y0, ((0x100 - weightx) * (0x100 - weighty)) >> 8,
        pix_x1y0, ((weightx        ) * (0x100 - weighty)) >> 8,
        pix_x0y1, ((0x100 - weightx) * (weighty        )) >> 8,
        pix_x1y1, ((weightx        ) * (weighty        )) >> 8);
      dstCur += 4;

      fx += dx;
      fy += dy;

      if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
      if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
    } while (--i);

    return dst;
  }

  // --------------------------------------------------------------------------
  // [Gradient - Generic]
  // --------------------------------------------------------------------------

  // C++ Implementation Notes:
  //
  // We are using GRADIENT LUT table that contains all interpolated colors in
  // graient. If the table is created it's easy to make linear / radial or
  // conical gradient simply calculating the formula and using this array[].
  // Calculation is normally done in 48.16 fixed point, may be scaled to 16.16
  // fixed point if it can fit here.
  //
  // Large gradients and antialiasing:
  //
  // Antialiasing is a bit problem. If the gradient lut table length is same as
  // length of linear / radial gradient then we can use ('yy' >> 8 & 0xFF) formula
  // to get correct interpolation weight, but if we scaled the gradient lut table
  // (because without scaling it was too big) then we can no longer use this formula.
  //
  // If gradient is too large then we are using gradient scaling. This means
  // generating smaller gradient lut table and modifying calculated values to
  // fit into this smaller table. Problem is that when using antialiasing the
  // weight that is normally computed as ('yy' >> 8 & 0xFF) is no longer valid
  // (it's scaled, you just can't use this value).

  static err_t FOG_FASTCALL generic_gradient_init(
    PatternContext* ctx, const List<ArgbStop>& stops, sysint_t gLength, uint32_t spread)
  {
    // Never initialize this if there are no stops or only single one (solid color).
    FOG_ASSERT(stops.getLength() >= 2);
    FOG_ASSERT((uint)spread < SPREAD_COUNT);

    bool hasAlpha = (spread == SPREAD_NONE) ? true : ArgbAnalyzer::analyzeAlpha(stops) != 0xFF;

    // Alloc twice memory for reflect spread.
    sysint_t gAlloc = gLength;
    if (spread == SPREAD_REFLECT) gAlloc <<= 1;

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

    ctx->format = hasAlpha ? PIXEL_FORMAT_PRGB32 : PIXEL_FORMAT_XRGB32;
    ctx->depth = 32;

    generic_gradient_stops(
      (uint8_t*)ctx->genericGradient.colors, stops,
      functionMap->interpolate.gradient[ctx->format],
      0, (int)gLength, (int)gLength, false);

    ctx->genericGradient.colorsAlloc = gAlloc;
    ctx->genericGradient.colorsLength = gMyLen;

    // Create mirror for reflect spread.
    if (spread == SPREAD_REFLECT)
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

    if (spread != SPREAD_NONE)
    {
      cFirst = stops.at(0).argb;
      cLast = (spread != SPREAD_PAD) ? cFirst : (uint32_t)stops.top().argb;

      // Premultiply.
      if (hasAlpha)
      {
        cFirst = ArgbUtil::premultiply(cFirst);
        cLast = ArgbUtil::premultiply(cLast);
      }
    }

    ctx->genericGradient.colors[-1] = cFirst;
    ctx->genericGradient.colors[gMyLen] = cLast;

    return ERR_OK;
  }

  static void FOG_FASTCALL generic_gradient_destroy(
    PatternContext* ctx)
  {
    FOG_ASSERT(ctx->initialized);

    Memory::free(ctx->linearGradient.colors - 1);
    ctx->initialized = false;
  }

  static void FOG_FASTCALL generic_gradient_stops(
    uint8_t* dst, const List<ArgbStop>& stops,
    InterpolateArgbFn gradientSpan,
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
      Argb color = stops.at(0).argb;
      gradientSpan(dst, color, color, size, 0, size);
    }
    else
    {
      sysint_t i = (reverse) ? count - 1 : 0;
      sysint_t iinc = (reverse) ? -1 : 1;

      Argb primaryStopColor = stops.at(i).argb;
      Argb secondaryStopArgb;

      double primaryStopOffset = 0.0;
      double secondaryStopOffset;
      long x1 = 0;
      long x2 = 0;

      for (; i < count && (sysint_t)x1 < end; i += iinc,
        primaryStopOffset = secondaryStopOffset,
        primaryStopColor = secondaryStopArgb,
        x1 = x2)
      {
        secondaryStopOffset = stops.at(i).offset;
        secondaryStopArgb = stops.at(i).argb;

        // Stop offset can be at range from 0.0 to 1.0 including.
        if (secondaryStopOffset < 0.0) secondaryStopOffset = 0.0;
        if (secondaryStopOffset > 1.0) secondaryStopOffset = 1.0;
        if (reverse) secondaryStopOffset = 1.0 - secondaryStopOffset;

        // Don't trust input data...
        if (secondaryStopOffset < primaryStopOffset) return;

        // Skip all siblings and the first one.
        if (secondaryStopOffset == primaryStopOffset) continue;

        // get pixel coordinates and skip that caller not wants
        x2 = (sysint_t)((double)size * secondaryStopOffset);
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
  // [Gradient - Linear]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL linear_gradient_init(
    PatternContext* ctx, const Pattern& pattern, const DoubleMatrix& matrix, uint32_t interpolationType)
  {
    PatternData* d = pattern._d;
    if (d->type != PATTERN_LINEAR_GRADIENT) return ERR_RT_INVALID_ARGUMENT;

    if (d->obj.stops->getLength() == 0)
    {
      return functionMap->pattern.solid_init(ctx, 0x00000000);
    }
    if (d->obj.stops->getLength() == 1)
    {
      return functionMap->pattern.solid_init(ctx, ArgbUtil::premultiply(d->obj.stops->at(0).argb));
    }

    // If points are equal, we will fill everything by last color. This is
    // defined in SVG.
    if (Math::feq(d->data.gradient->points[0].x, d->data.gradient->points[1].x) &&
        Math::feq(d->data.gradient->points[0].y, d->data.gradient->points[1].y))
    {
      return functionMap->pattern.solid_init(ctx, ArgbUtil::premultiply(d->obj.stops->at(d->obj.stops->getLength() -1).argb));
    }

    // FIXME: TODO: Not correct code
#if 0
    DoubleMatrix m(pattern._d->matrix.multiplied(matrix));
    DoublePoint points[2];

    double f = (m.sx * m.sy) - (m.shy * m.shx);
    if (fabs(f) < 1e-20)
    {
      return functionMap->pattern.solid_init(ctx, premultiply(d->obj.stops->at(0).rgba));
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
      return functionMap->pattern.solid_init(ctx, premultiply(d->obj.stops->at(0).rgba));
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
    if (d->spread == SPREAD_REFLECT) ctx->linearGradient.dist *= 2.0;
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
      return functionMap->pattern.solid_init(ctx, ArgbUtil::premultiply(d->obj.stops->at(0).argb));
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
    if (d->spread == SPREAD_REFLECT) ctx->linearGradient.dist *= 2.0;
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
      return functionMap->pattern.solid_init(ctx, premultiply(d->obj.stops->at(0).rgba));
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
    if (d->spread == SPREAD_REFLECT) ctx->linearGradient.dist *= 2.0;

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
    err_t err = generic_gradient_init(ctx, d->obj.stops.instance(), gLength, d->spread);
    if (err) return err;

    // Whether gradient definition is exact (no antialiasing, very fast).
    bool exact = false;
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
      ctx->fetch = functionMap->pattern.linear_gradient_fetch_exact[(uint)d->spread];
    else
      ctx->fetch = functionMap->pattern.linear_gradient_fetch_subxy[(uint)d->spread];

    // Set destroy function.
    ctx->destroy = generic_gradient_destroy;

    ctx->initialized = true;
    return ERR_OK;
  }

  static uint8_t* FOG_FASTCALL linear_gradient_fetch_exact_pad(
    PatternContext* ctx,
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

    // There are two directions of gradient (ax > 0 or ax < 0):
    if (ax < 0)
    {
      // Negative ax.
      while (yy >= yy_max)
      {
        ((uint32_t*)dstCur)[0] = color1;
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      while (yy >= 0)
      {
        ((uint32_t*)dstCur)[0] = colors[(sysint_t)(yy >> 16)];
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      do {
        ((uint32_t*)dstCur)[0] = color0;
        if (!(--w)) goto end;
        dstCur += 4;
      } while (true);
    }
    else if (ax > 0)
    {
      // Positive ax.
      while (yy <= 0)
      {
        ((uint32_t*)dstCur)[0] = color0;
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      while (yy < yy_max)
      {
        ((uint32_t*)dstCur)[0] = colors[(sysint_t)(yy >> 16)];
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      do {
        ((uint32_t*)dstCur)[0] = color1;
        if (!(--w)) goto end;
        dstCur += 4;
      } while (true);
    }
    else
    {
      // Horizontal line.
      uint32_t color;

      if (yy <= 0)
        color = color0;
      else if (yy >= yy_max)
        color = color1;
      else
        color = colors[(sysint_t)(yy >> 16)];

      do {
        ((uint32_t*)dstCur)[0] = color;
        if (!(--w)) goto end;
        dstCur += 4;
      } while (true);
    }

  end:
    return dst;
  }

  static uint8_t* FOG_FASTCALL linear_gradient_fetch_exact_repeat(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    const uint32_t* colors = (const uint32_t*)ctx->linearGradient.colors;

    // Not needed to use 64-bit integers for SPREAD_REPEAT, because we can
    // normalize input coordinates to always fit to this data-type.
    int yy_max = Math::doubleToFixed16x16(ctx->linearGradient.dist);
    int ax = Math::doubleToFixed16x16(ctx->linearGradient.ax);
    int yy = (int)((Math::doubleToFixed48x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay )) % yy_max);
    if (yy < 0) yy += yy_max;

    // There are two directions of gradient (ax > 0 or ax < 0):
    if (ax < 0)
    {
      // Negative ax.
      do {
        ((uint32_t*)dstCur)[0] = colors[yy >> 16];
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
        if (yy < 0) yy += yy_max;
      } while (true);
    }
    else if (ax > 0)
    {
      // Positive ax.
      do {
        ((uint32_t*)dstCur)[0] = colors[yy >> 16];
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
        if (yy >= yy_max) yy -= yy_max;
      } while (true);
    }
    else
    {
      // Horizontal line.
      uint32_t c0 = colors[yy >> 16];
      _do_fill_32(dstCur, c0, w);
    }

  end:
    return dst;
  }

  static uint8_t* FOG_FASTCALL linear_gradient_fetch_subxy_pad(
    PatternContext* ctx,
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

    // There are two directions of gradient (ax > 0 or ax < 0):
    if (ax < 0)
    {
      while (yy >= yy_max)
      {
        ((uint32_t*)dstCur)[0] = color1;
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      while (yy >= 0)
      {
        int pos = (int)(yy >> 16);
        uint weight = (((int)yy & 0xFFFF) >> 8);
        PATTERN_C_INTERPOLATE_32_2(
          ((uint32_t*)dstCur)[0],
          colors[pos    ], 256 - weight,
          colors[pos + 1], weight);
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      do {
        ((uint32_t*)dstCur)[0] = color0;
        if (!(--w)) goto end;
        dstCur += 4;
      } while (true);
    }
    else
    {
      while (yy <= 0)
      {
        ((uint32_t*)dstCur)[0] = color0;
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      while (yy < yy_max)
      {
        int pos = (int)(yy >> 16);
        uint weight = (((int)yy & 0xFFFF) >> 8);
        PATTERN_C_INTERPOLATE_32_2(
          ((uint32_t*)dstCur)[0],
          colors[pos    ], 256 - weight,
          colors[pos + 1], weight);
        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
      }

      do {
        ((uint32_t*)dstCur)[0] = color1;
        if (!(--w)) goto end;
        dstCur += 4;
      } while (true);
    }

  end:
    return dst;
  }

  static uint8_t* FOG_FASTCALL linear_gradient_fetch_subxy_repeat(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    const uint32_t* colors = (const uint32_t*)ctx->linearGradient.colors;

    // Not needed to use 64-bit integers for SPREAD_REPEAT, because we can
    // normalize input coordinates to always fit to this data-type.
    int yy_max = Math::doubleToFixed16x16(ctx->linearGradient.dist);
    int ax = Math::doubleToFixed16x16(ctx->linearGradient.ax);
    int yy = (int)((Math::doubleToFixed48x16(
      ((double)x - ctx->linearGradient.dx) * ctx->linearGradient.ax +
      ((double)y - ctx->linearGradient.dy) * ctx->linearGradient.ay )) % yy_max);
    if (yy < 0) yy += yy_max;

    // There are two directions of gradient (ax > 0 or ax < 0):
    if (ax < 0)
    {
      do {
        int pos = (yy >> 16);
        uint weight = ((yy & 0xFFFF) >> 8);
        PATTERN_C_INTERPOLATE_32_2(
          ((uint32_t*)dstCur)[0],
          colors[pos    ], 256 - weight,
          colors[pos + 1], weight);

        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
        if (yy < 0) yy += yy_max;
      } while (true);
    }
    else
    {
      do {
        int pos = (yy >> 16);
        uint weight = ((yy & 0xFFFF) >> 8);
        PATTERN_C_INTERPOLATE_32_2(
          ((uint32_t*)dstCur)[0],
          colors[pos    ], 256 - weight,
          colors[pos + 1], weight);

        if (!(--w)) goto end;

        dstCur += 4;
        yy += ax;
        if (yy >= yy_max) yy -= yy_max;
      } while (true);
    }

  end:
    return dst;
  }

  // --------------------------------------------------------------------------
  // [Gradient - Radial]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL radial_gradient_init(
    PatternContext* ctx, const Pattern& pattern, const DoubleMatrix& matrix, uint32_t interpolationType)
  {
    PatternData* d = pattern._d;
    if (d->type != PATTERN_RADIAL_GRADIENT) return ERR_RT_INVALID_ARGUMENT;

    if (d->obj.stops->getLength() == 0)
    {
      return functionMap->pattern.solid_init(ctx, 0x00000000);
    }
    if (d->obj.stops->getLength() == 1)
    {
      return functionMap->pattern.solid_init(ctx, ArgbUtil::premultiply(d->obj.stops->at(0).argb));
    }

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
    if (d->spread == SPREAD_REFLECT) gAlloc <<= 1;

    ctx->radialGradient.mul = (double)gLength / dd;

    err_t err = generic_gradient_init(ctx, d->obj.stops.instance(), gLength, d->spread);
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
    ctx->fetch = functionMap->pattern.radial_gradient_fetch[(uint)d->spread];

    // Set destroy function.
    ctx->destroy = generic_gradient_destroy;

    ctx->initialized = true;
    return ERR_OK;
  }

  /*

  This is reference implementation from AntiGrain 2.4.

  static uint8_t* FOG_FASTCALL radial_gradient_fetch_pad(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    uint8_t* dstCur = dst;

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
        ((uint32_t*)dstCur)[0] = color0;
      else if (index >= colorsLength)
        ((uint32_t*)dstCur)[0] = color1;
      else
        ((uint32_t*)dstCur)[0] = colors[index];

      dstCur += 4;
      dx += 1.0;
    } while (--w);

    return dst;
  }

  */

  static uint8_t* FOG_FASTCALL radial_gradient_fetch_pad(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

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

    do {
      index = (int)((dd + Math::sqrt(fabs(cc))) * scale);

      if (index < 0)
        ((uint32_t*)dstCur)[0] = color0;
      else if (index >= colorsLength)
        ((uint32_t*)dstCur)[0] = color1;
      else
        ((uint32_t*)dstCur)[0] = colors[index];
      dstCur += 4;

      // cc += cx + cx + ci
      // cx += ci
      cc += cx;
      cx += ci;
      cc += cx;

      dd += di;
    } while (--w);

    return dst;
  }

  static uint8_t* FOG_FASTCALL radial_gradient_fetch_repeat(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

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

    do {
      index = (int)((dd + Math::sqrt(fabs(cc))) * scale) % colorsLength;
      if (index < 0) index += colorsLength;

      ((uint32_t*)dstCur)[0] = colors[index];
      dstCur += 4;

      // cc += cx + cx + ci
      // cx += ci
      cc += cx;
      cx += ci;
      cc += cx;

      dd += di;
    } while (--w);

    return dst;
  }

  // --------------------------------------------------------------------------
  // [Gradient - Conical]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL conical_gradient_init(
    PatternContext* ctx, const Pattern& pattern, const DoubleMatrix& matrix, uint32_t interpolationType)
  {
    PatternData* d = pattern._d;
    if (d->type != PATTERN_CONICAL_GRADIENT) return ERR_RT_INVALID_ARGUMENT;

    if (d->obj.stops->getLength() == 0)
    {
      return functionMap->pattern.solid_init(ctx, 0x00000000);
    }
    if (d->obj.stops->getLength() == 1)
    {
      return functionMap->pattern.solid_init(ctx, ArgbUtil::premultiply(d->obj.stops->at(0).argb));
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

    err_t err = generic_gradient_init(ctx, d->obj.stops.instance(), gLength, d->spread);
    if (err) return err;

    // Set fetch function.
    ctx->fetch = functionMap->pattern.conical_gradient_fetch;

    // Set destroy function.
    ctx->destroy = generic_gradient_destroy;

    ctx->initialized = true;
    return ERR_OK;
  }

  static uint8_t* FOG_FASTCALL conical_gradient_fetch(
    PatternContext* ctx,
    uint8_t* dst, int x, int y, int w)
  {
    FOG_ASSERT(w);

    uint8_t* dstCur = dst;

    const uint32_t* colors = (const uint32_t*)ctx->radialGradient.colors;
    sysint_t colorsLength = ctx->radialGradient.colorsLength;

    int index;

    double dx = (double)x - ctx->conicalGradient.dx;
    double dy = (double)y - ctx->conicalGradient.dy;
    double scale = (double)colorsLength / (M_PI * 2.0);
    double add = ctx->conicalGradient.angle;
    if (add < M_PI) add += M_PI * 2.0;

    do {
      index = (int)((atan2(dy, dx) + add) * scale);
      if (index >= colorsLength) index -= colorsLength;

      ((uint32_t*)dstCur)[0] = colors[index];
      dstCur += 4;
      dx += 1.0;
    } while (--w);

    return dst;
  }
};

} // RasterEngine namespace
} // Fog namespace
