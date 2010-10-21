// [Fog-Graphics]
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
// [Fog::RasterEngine::C - Pattern - Texture]
// ============================================================================

//! @internal
struct FOG_HIDDEN PatternTextureC
{
  // --------------------------------------------------------------------------
  // [Pattern - Texture - Formats]
  // --------------------------------------------------------------------------

  struct FOG_HIDDEN FMT_XRGB32
  {
    enum { BPP = 4, AS_IS = 1 };

    FOG_INLINE FMT_XRGB32(const RasterPattern* ctx) {}
    FOG_INLINE uint32_t fetch(const uint8_t* p) { return READ_32(p) | 0xFF000000; }
  };

  struct FOG_HIDDEN FMT_ARGB32
  {
    enum { BPP = 4, AS_IS = 0 };

    FOG_INLINE FMT_ARGB32(const RasterPattern* ctx) {}
    FOG_INLINE uint32_t fetch(const uint8_t* p) { return ColorUtil::premultiply(READ_32(p)); }
  };

  struct FOG_HIDDEN FMT_PRGB32
  {
    enum { BPP = 4, AS_IS = 1 };

    FOG_INLINE FMT_PRGB32(const RasterPattern* ctx) {}
    FOG_INLINE uint32_t fetch(const uint8_t* p) { return READ_32(p); }
  };

  struct FOG_HIDDEN FMT_A8
  {
    enum { BPP = 1, AS_IS = 0 };

    FOG_INLINE FMT_A8(const RasterPattern* ctx) {}
    FOG_INLINE uint32_t fetch(const uint8_t* p) { return (uint32_t)p[0] << 24; }
  };

  struct FOG_HIDDEN FMT_I8
  {
    enum { BPP = 1, AS_IS = 0 };

    FOG_INLINE FMT_I8(const RasterPattern* ctx) { pal = ctx->texture.pal; }
    FOG_INLINE uint32_t fetch(const uint8_t* p) { return pal[p[0]]; }

    const uint32_t* pal;
  };

  // --------------------------------------------------------------------------
  // [Pattern - Texture - Init / Destroy]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL init(
    RasterPattern* ctx,
    const Pattern& pattern,
    const TransformD& transform,
    uint32_t interpolationType)
  {
    PatternData* d = pattern._d;

    // Only PATTERN_TEXTURE type pattern can be passed to init().
    FOG_ASSERT(d->type == PATTERN_TEXTURE);

    // If texture is empty image we tread it as 0x00000000 solid color.
    if (d->obj.texture->isEmpty()) return rasterFuncs.pattern.solid_init(ctx, 0x00000000);

    // Multiply pattern transformation matrix with a given transform (painter transformations).
    TransformD m(pattern._d->transform.multiplied(transform));

    // Call init_blit() which will initialize the context.
    return init_blit(ctx, d->obj.texture.instance(), d->data.texture->area, m, d->spread, interpolationType);
  }

  static err_t FOG_FASTCALL init_blit(
    RasterPattern* ctx,
    const Image& image, const RectI& irect, 
    const TransformD& transform,
    uint32_t spread, uint32_t interpolationType)
  {
    // Only valid images can be passed to init_blit.
    FOG_ASSERT(!image.isEmpty());
    FOG_ASSERT(spread < PATTERN_SPREAD_COUNT);

    uint32_t format = image.getFormat();
    bool isTransformed = (!Math::feq(transform._00, 1.0) || !Math::feq(transform._01, 0.0) ||
                          !Math::feq(transform._11, 1.0) || !Math::feq(transform._10, 0.0) );

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
      case IMAGE_FORMAT_PRGB32:
      case IMAGE_FORMAT_ARGB32:
      {
        ctx->texture.bits += irect.x * 4;
        ctx->format = IMAGE_FORMAT_PRGB32;
        ctx->depth = 32;
        ctx->bytesPerPixel = 4;
        break;
      }

      case IMAGE_FORMAT_XRGB32:
      {
        ctx->texture.bits += irect.x * 4;
        ctx->format = (spread == PATTERN_SPREAD_NONE)
          ? IMAGE_FORMAT_PRGB32
          : IMAGE_FORMAT_XRGB32;
        ctx->depth = 32;
        ctx->bytesPerPixel = 4;
        break;
      }

      case IMAGE_FORMAT_A8:
      {
        ctx->texture.bits += irect.x;
        ctx->format = IMAGE_FORMAT_A8;
        ctx->depth = 8;
        ctx->bytesPerPixel = 1;
        break;
      }

      case IMAGE_FORMAT_I8:
      {
        const Palette& pal = ctx->texture.texture->getPalette();
        ctx->texture.pal = reinterpret_cast<const uint32_t*>(pal.getData()) + Palette::INDEX_PRGB32;

        ctx->texture.bits += irect.x;
        ctx->format = (pal.isAlphaUsed() && spread != PATTERN_SPREAD_NONE)
          ? IMAGE_FORMAT_PRGB32
          : IMAGE_FORMAT_XRGB32;
        ctx->depth = 32;
        ctx->bytesPerPixel = 4;
        break;
      }

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    ctx->fetch = NULL;

    if (!isTransformed)
    {
      int64_t startx = Math::doubleToFixed48x16(transform._20);
      int64_t starty = Math::doubleToFixed48x16(transform._21);

      uint fx = (int)(startx >> 8) & 0xFF;
      uint fy = (int)(starty >> 8) & 0xFF;
      bool exact = ((fx == 0) & (fy == 0));

      ctx->texture.dx = -(int)(startx >> 16);
      ctx->texture.dy = -(int)(starty >> 16);

      // if ((startx & 0xFF) >= 128 && ++fx == 256) { fx = 0; ctx->texture.dx--; }
      // if ((starty & 0xFF) >= 128 && ++fy == 256) { fy = 0; ctx->texture.dy--; }

      if (interpolationType == IMAGE_INTERPOLATION_NEAREST)
      {
        // TODO: Check if it's correct.
        if (fx < 128) ctx->texture.dx++;
        if (fy < 128) ctx->texture.dy++;
        exact = true;
      }

      if (exact)
      {
        // Shouldn't be needed, but be safe.
        ctx->texture.fY0X0 = 0;
        ctx->texture.fY0X1 = 0;
        ctx->texture.fY1X0 = 0;
        ctx->texture.fY1X1 = 0;

        // Set fetch function.
        ctx->fetch = rasterFuncs.pattern.texture_fetch_exact[format][spread];
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
        ctx->texture.fY0X0 = ((      fy) * (      fx)) >> 8;
        ctx->texture.fY0X1 = ((      fy) * (256 - fx)) >> 8;
        ctx->texture.fY1X0 = ((256 - fy) * (      fx)) >> 8;
        ctx->texture.fY1X1 = ((256 - fy) * (256 - fx)) >> 8;

        // Set fetch function.
        if (fx && fy)
          ctx->fetch = rasterFuncs.pattern.texture_fetch_subxy[format][spread];
        else if (fx)
          ctx->fetch = rasterFuncs.pattern.texture_fetch_subx0[format][spread];
        else if (fy)
          ctx->fetch = rasterFuncs.pattern.texture_fetch_sub0y[format][spread];
        else
          FOG_ASSERT_NOT_REACHED();
      }
    }

    if (!ctx->fetch)
    {
      // Transform.
      // TODO: Check for return value.
      ctx->inv = transform.inverted();

      // Inner loop increments and bounds (16.16 fixed point).
      {
        int dx = Math::doubleToFixed16x16(ctx->inv._00);
        int dy = Math::doubleToFixed16x16(ctx->inv._01);

        int fxmax = ctx->texture.w << 16;
        int fymax = ctx->texture.h << 16;

        if (spread == PATTERN_SPREAD_REFLECT)
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
      if (interpolationType == IMAGE_INTERPOLATION_NEAREST)
        ctx->fetch = rasterFuncs.pattern.texture_fetch_trans_affine_nearest[format][spread];
      else
        ctx->fetch = rasterFuncs.pattern.texture_fetch_trans_affine_bilinear[format][spread];
    }

    // Set destroy function.
    ctx->destroy = destroy;

    ctx->initialized = true;
    return ERR_OK;
  }

  static void FOG_FASTCALL destroy(
    RasterPattern* ctx)
  {
    FOG_ASSERT(ctx->initialized);

    ctx->texture.texture.destroy();
    ctx->initialized = false;
  }

  // --------------------------------------------------------------------------
  // [Pattern - Texture - Fetch - Exact - 32]
  // --------------------------------------------------------------------------

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_exact_none_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    y += ctx->texture.dy;
    if ((uint)y >= (uint)th)
    {
      PatternCoreC::fetch_solid(span, buffer, mode, 0x00000000);
      return;
    }

    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      const uint8_t* src = srcBase;
      x += ctx->texture.dx;

      // Fetch before.
      if (x < 0)
      {
        int i = Math::min(-x, w);
        w -= i;

        dst = PatternCoreC::fill32(dst, 0x00000000, i);
        if (!w) goto fetchSkip;

        x = 0;
      }
      // Or set image buffer if span fits into it.
      else if (P_TEXTURE_FORMAT::AS_IS && mode == PATTERN_FETCH_CAN_USE_SRC && 
                            x < tw && w < tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(src);
        goto fetchSkip;
      }

      // Fetch texture.
      if (x < tw)
      {
        int i = Math::min(tw - x, w);
        src += (uint)x * P_TEXTURE_FORMAT::BPP;
        w -= i;

        do {
          ((uint32_t*)dst)[0] = tex.fetch(src);
          dst += 4;
          src += P_TEXTURE_FORMAT::BPP;
        } while (--i);
        if (!w) goto fetchSkip;
      }

      // Fetch after.
      goto fetchSolidLoop;

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
    return;

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = 0x00000000;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_exact_pad_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    y += ctx->texture.dy;
    if (y < 0) y = 0;
    if (y >= th) y = th - 1;

    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    uint32_t src0;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      const uint8_t* src = srcBase;
      x += ctx->texture.dx;

      // Fetch before.
      if (x < 0)
      {
        int i = Math::min(-x, w);
        x = 0;
        w -= i;

        uint32_t c0 = tex.fetch(src);
        dst = PatternCoreC::fill32(dst, c0, i);
        if (!w) goto fetchSkip;
      }
      // Or set image buffer if span fits into it.
      else if (P_TEXTURE_FORMAT::AS_IS && mode == PATTERN_FETCH_CAN_USE_SRC && 
                            x < tw && w < tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(src);
        goto fetchSkip;
      }

      // Fetch texture.
      if (x < tw)
      {
        int i = Math::min(tw - x, w);
        src += (uint)x * P_TEXTURE_FORMAT::BPP;
        w -= i;

        do {
          ((uint32_t*)dst)[0] = tex.fetch(src);
          dst += 4;
          src += P_TEXTURE_FORMAT::BPP;
        } while (--i);
        if (!w) goto fetchSkip;
      }

      // Fetch after.
      src0 = tex.fetch(srcBase + (uint)tw * P_TEXTURE_FORMAT::BPP - P_TEXTURE_FORMAT::BPP);
      goto fetchSolidLoop;

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
    return;

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = src0;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_exact_repeat_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if ((x >= tw) | (x <= -tw)) x %= tw;
    if ((y >= th) | (y <= -th)) y %= th;

    if (x < 0) x += tw;
    if (y < 0) y += th;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* src = srcBase + (uint)x * P_TEXTURE_FORMAT::BPP;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      // Set image buffer if span fits into it (this is very efficient
      // optimization for short spans or large textures).
      int i = Math::min(tw - x, w);
      if (P_TEXTURE_FORMAT::AS_IS && mode == PATTERN_FETCH_CAN_USE_SRC && 
                       w < tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(src);
        src += (uint)w * P_TEXTURE_FORMAT::BPP;

        if ((x += w) == tw) { x = 0; src = srcBase; }
        goto fetchSkip;
      }

      // Fetch pattern.
      for (;;)
      {
        w -= i;
        x += i;

        do {
          ((uint32_t*)dst)[0] = tex.fetch(src);
          dst += 4;
          src += P_TEXTURE_FORMAT::BPP;
        } while (--i);

        if (x == tw) { x = 0; src = srcBase; }
        if (!w) break;

        i = Math::min(tw, w);
      }

fetchSkip:
      P_FETCH_SPAN8_HOLE(
      {
        x += hole;
        src += (uint)hole * P_TEXTURE_FORMAT::BPP;
        if (x >= tw) { x %= tw; src = srcBase + (uint)x * P_TEXTURE_FORMAT::BPP; }
      })
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_exact_reflect_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    x += ctx->texture.dx;
    y += ctx->texture.dy;

    if ((x >= tw2) | (x <= -tw2)) x %= tw2;
    if ((y >= th2) | (y <= -th2)) y %= th2;

    if (x < 0) x += tw2;
    if (y < 0) y += th2;
  
    // Modify Y if reflected (if it lies in second section).
    if (y >= th) y = th2 - y - 1;

    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      if (P_TEXTURE_FORMAT::AS_IS && mode == PATTERN_FETCH_CAN_USE_SRC && 
                       x <= tw && w < tw - x)
      {
        P_FETCH_SPAN8_SET_CUSTOM(srcBase + (uint)x * P_TEXTURE_FORMAT::BPP);

        x += w;
        goto fetchSkip;
      }

      do {
        // Reflect mode.
        if (x >= tw)
        {
          int i = Math::min(tw2 - x, w);
          const uint8_t* src = srcBase + (tw2 - x - 1) * P_TEXTURE_FORMAT::BPP;

          w -= i;
          x += i;
          if (x == tw2) x = 0;

          do {
            ((uint32_t*)dst)[0] = tex.fetch(src);
            dst += 4;
            src -= P_TEXTURE_FORMAT::BPP;
          } while (--i);
        }
        // Repeat mode.
        else
        {
          int i = Math::min(tw - x, w);
          const uint8_t* src = srcBase + x * P_TEXTURE_FORMAT::BPP;

          w -= i;
          x += i;

          do {
            ((uint32_t*)dst)[0] = tex.fetch(src);
            dst += 4;
            src += P_TEXTURE_FORMAT::BPP;
          } while (--i);
        }
      } while (w);

fetchSkip:
      P_FETCH_SPAN8_HOLE(
      {
        x += hole;
        if ((x >= tw2) | (x <= -tw2)) x %= tw2;
        if (x < 0) x += tw2;
      })
    P_FETCH_SPAN8_END()
  }

  // --------------------------------------------------------------------------
  // [Pattern - Texture - Fetch - SubX0 - 32]
  // --------------------------------------------------------------------------

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_subx0_none_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    y += ctx->texture.dy;
    if ((y < 0) | (y >= th))
    {
      PatternCoreC::fetch_solid(span, buffer, mode, 0x00000000);
      return;
    }

    P_FETCH_SPAN8_INIT()

    uint fY0X0 = ctx->texture.fY1X0;
    uint fY0X1 = ctx->texture.fY1X1;

    P_TEXTURE_FORMAT tex(ctx);
    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

      x += ctx->texture.dx;
      const uint8_t* src = srcBase;

      uint32_t back_0 = 0x00000000;
      int i;

      // Fetch before.
      if (x < 0)
      {
        i = Math::min(-x, w);
        w -= i;
        x = 0;

        dst = PatternCoreC::fill32(dst, back_0, i);

        if (!w) goto fetchSkip;
        goto fetchFill;
      }
      // Fetch texture.
      else if (x <= tw)
      {
        if (x > 0)
        {
          src += (uint)x * P_TEXTURE_FORMAT::BPP;
          back_0 = tex.fetch(src - P_TEXTURE_FORMAT::BPP);
        }

fetchFill:
        i = Math::min(tw - x, w);
        w -= i;

        while (i)
        {
          C_PATTERN_INTERPOLATE_32_2(
            ((uint32_t*)dst)[0],
            back_0                 , fY0X0,
            back_0 = tex.fetch(src), fY0X1)

          dst += 4;
          src += P_TEXTURE_FORMAT::BPP;
          i--;
        }

        // Interpolate last pixel on the row.
        if (!w) goto fetchSkip;

        C_PATTERN_INTERPOLATE_32_2_WITH_ZERO(((uint32_t*)dst)[0], back_0, fY0X0)
        dst += 4;
        if (!--w) goto fetchSkip;
      }

      goto fetchSolidLoop;

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
    return;

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = 0x00000000;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_subx0_pad_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY1X0;
    uint fY0X1 = ctx->texture.fY1X1;

    y += ctx->texture.dy;
    x += ctx->texture.dx - 1;

    if (y < 0) y = 0;
    else if (y >= th) y = th - 1;

    P_TEXTURE_FORMAT tex(ctx);
    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

    uint32_t back_0;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

      const uint8_t* src = srcBase;
      int i;

      // Fetch before.
      if (x < 0)
      {
        i = Math::min(-x, w);
        w -= i;
        x = 1;

        back_0 = tex.fetch(src);
        dst = PatternCoreC::fill32(dst, back_0, i);
        src += P_TEXTURE_FORMAT::BPP;

        if (!w) goto fetchSkip;
        goto fetchFill;
      }
      // Fetch texture.
      else if (x < tw)
      {
        src += (uint)x * P_TEXTURE_FORMAT::BPP;

        back_0 = tex.fetch(src);
        src += P_TEXTURE_FORMAT::BPP;

        if (++x == tw) goto fetchSolidLoop;

fetchFill:
        i = Math::min(tw - x, w);
        w -= i;

        do {
          C_PATTERN_INTERPOLATE_32_2(
            ((uint32_t*)dst)[0],
            back_0                 , fY0X0,
            back_0 = tex.fetch(src), fY0X1);

          dst += 4;
          src += P_TEXTURE_FORMAT::BPP;
        } while (--i);
        if (!w) goto fetchSkip;
        goto fetchSolidLoop;
      }

      back_0 = tex.fetch(srcBase + tw * P_TEXTURE_FORMAT::BPP - P_TEXTURE_FORMAT::BPP);
      goto fetchSolidLoop;

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
    return;

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = back_0;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_subx0_repeat_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY1X0;
    uint fY0X1 = ctx->texture.fY1X1;

    y += ctx->texture.dy;
    if ((y >= th) | (y <= -th)) y %= th;
    if (y < 0) y += th;

    P_TEXTURE_FORMAT tex(ctx);
    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

      x += ctx->texture.dx - 1;
      if ((x >= tw) | (x <= -tw)) x %= tw;
      if (x < 0) x += tw;

      const uint8_t* src = srcBase + (uint)x * P_TEXTURE_FORMAT::BPP;
      uint32_t back_0 = tex.fetch(src);
      src += P_TEXTURE_FORMAT::BPP;

      if (++x == tw)
      {
        x = 0;
        src = srcBase;
      }

      int r = Math::min(w, tw);
      int i = Math::min(tw - x, r);

      w -= r;

      // Fetch only texture width line.
      for (;;)
      {
        r -= i;

        do {
          C_PATTERN_INTERPOLATE_32_2(
            ((uint32_t*)dst)[0],
            back_0                 , fY0X0,
            back_0 = tex.fetch(src), fY0X1);

          dst += 4;
          src += P_TEXTURE_FORMAT::BPP;
        } while (--i);
        if (!r) break;

        i = Math::min(r, tw);
        src = srcBase;
      }

      // Fetch the rest.
      if (w) dst = PatternCoreC::repeat32(dst, tw, w);

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_subx0_reflect_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    uint fY0X0 = ctx->texture.fY1X0;
    uint fY0X1 = ctx->texture.fY1X1;

    y += ctx->texture.dy;
    if ((y >= th2) | (y <= -th2)) y %= th2;
    if (y < 0) y += th2;

    // Modify Y if reflected (if it lies in second section).
    if (y >= th) y = th2 - y - 1;

    P_TEXTURE_FORMAT tex(ctx);
    const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* src;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

      x += ctx->texture.dx - 1;
      if ((x >= tw2) | (x <= -tw2)) x %= tw2;
      if (x < 0) x += tw2;

      if (x >= tw)
      {
        // Reflect mode.
        src = srcBase + (uint)(tw2 - x - 1) * P_TEXTURE_FORMAT::BPP;
      }
      else
      {
        // Repeat mode.
        src = srcBase + (uint)x * P_TEXTURE_FORMAT::BPP;
      }

      uint32_t back_0 = tex.fetch(src);
      if (++x >= tw2) x -= tw2;

      int r = Math::min(w, tw2);
      w -= r;

      // Fetch only texture width line * 2.
      do {
        // Reflect mode.
        if (x >= tw)
        {
          int i = Math::min(tw2 - x, r);
          src = srcBase + (tw2 - x - 1) * P_TEXTURE_FORMAT::BPP;

          r -= i;
          x = 0;

          do {
            C_PATTERN_INTERPOLATE_32_2(
              ((uint32_t*)dst)[0],
              back_0                 , fY0X0,
              back_0 = tex.fetch(src), fY0X1);

            dst += 4;
            src -= P_TEXTURE_FORMAT::BPP;
          } while (--i);
        }
        // Repeat mode.
        else
        {
          int i = Math::min(tw - x, r);
          src = srcBase + (uint)x * P_TEXTURE_FORMAT::BPP;

          r -= i;
          x += i;

          do {
            C_PATTERN_INTERPOLATE_32_2(
              ((uint32_t*)dst)[0],
              back_0                 , fY0X0,
              back_0 = tex.fetch(src), fY0X1);

            dst += 4;
            src += P_TEXTURE_FORMAT::BPP;
          } while (--i);
        }
      } while (r);

      // Fetch the rest.
      if (w) dst = PatternCoreC::repeat32(dst, tw2, w);

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  // --------------------------------------------------------------------------
  // [Pattern - Texture - Fetch - Sub0Y - 32]
  // --------------------------------------------------------------------------

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_sub0y_none_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X1;

    y += ctx->texture.dy - 1;

    if ((y < -1) | (y >= th))
    {
      PatternCoreC::fetch_solid(span, buffer, mode, 0x00000000);
      return;
    }

    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    if (y == -1 || y == th - 1)
    {
      // This is Y border case, we need to interpolate with zero.

      // Swap weight if (y == -1).
      if (y < 0) { fY0X0 = fY1X0; y++; }
      const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

        x += ctx->texture.dx;

        // Fetch before.
        if (x < 0)
        {
          int i = Math::min(-x, w);
          x = 0;
          w -= i;

          dst = PatternCoreC::fill32(dst, 0x00000000, i);
          if (!w) goto fetchBorderSkip;
        }

        // Fetch texture.
        if (x < tw)
        {
          const uint8_t* src = srcBase + (uint)x * P_TEXTURE_FORMAT::BPP;
          int i = Math::min(tw - x, w);
          w -= i;

          do {
            C_PATTERN_INTERPOLATE_32_2_WITH_ZERO(
              ((uint32_t*)dst)[0],
              tex.fetch(src), fY0X0)

            dst += 4;
            src += P_TEXTURE_FORMAT::BPP;
          } while (--i);
          if (!w) goto fetchBorderSkip;
        }

        // Fetch the rest.
        goto fetchSolidLoop;

fetchBorderSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    else
    {
      const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

        // Fetch before.
        if (x < 0)
        {
          int i = Math::min(-x, w);
          x = 0;
          w -= i;

          dst = PatternCoreC::fill32(dst, 0x00000000, i);
          if (!w) goto fetchInnerSkip;
        }

        // Fetch texture.
        if (x < tw)
        {
          const uint8_t* srcCur0 = srcBase0 + (uint)x * P_TEXTURE_FORMAT::BPP;
          const uint8_t* srcCur1 = srcBase1 + (uint)x * P_TEXTURE_FORMAT::BPP;

          int i = Math::min(tw - x, w);
          w -= i;

          do {
            C_PATTERN_INTERPOLATE_32_2(
              ((uint32_t*)dst)[0],
              tex.fetch(srcCur0), fY0X0,
              tex.fetch(srcCur1), fY1X0);

            dst += 4;
            srcCur0 += P_TEXTURE_FORMAT::BPP;
            srcCur1 += P_TEXTURE_FORMAT::BPP;
          } while (--i);
          if (!w) goto fetchInnerSkip;
        }

        // Fetch the rest.
        goto fetchSolidLoop;

fetchInnerSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    return;

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = 0x00000000;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_sub0y_pad_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X1;

    y += ctx->texture.dy - 1;
    P_TEXTURE_FORMAT tex(ctx);

    uint32_t src0;

    if (y < 0 || y >= th - 1)
    {
      // This exact fetch.
      if (y < 0) y = 0;
      else y = th - 1;

      const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

      P_FETCH_SPAN8_BEGIN()
        // Here we can't use MERGE_NEIGHBOURS, because there is there is
        // used P_FETCH_SPAN8_SET_CUSTOM in loop.
        P_FETCH_SPAN8_SET_CURRENT()

        x += ctx->texture.dx;

        // Fetch before.
        if (x < 0)
        {
          int i = Math::min(-x, w);
          x = 0;
          w -= i;

          uint32_t c0 = tex.fetch(srcBase);
          dst = PatternCoreC::fill32(dst, c0, i);
          if (!w) goto fetchExactSkip;
        }
        // Or set image buffer if span fits into it.
        else if (P_TEXTURE_FORMAT::AS_IS && mode == PATTERN_FETCH_CAN_USE_SRC &&
                                            x < tw && w < tw - x)
        {
          P_FETCH_SPAN8_SET_CUSTOM(srcBase + (uint)x * P_TEXTURE_FORMAT::BPP);
          goto fetchExactSkip;
        }

        // Fetch texture.
        if (x < tw)
        {
          const uint8_t* src = srcBase + (uint)x * P_TEXTURE_FORMAT::BPP;
          int i = Math::min(tw - x, w);
          w -= i;

          do {
            ((uint32_t*)dst)[0] = tex.fetch(src);
            dst += 4;
            src += P_TEXTURE_FORMAT::BPP;
          } while (--i);
          if (!w) goto fetchExactSkip;
        }

        // Fetch after.
        src0 = tex.fetch(srcBase + (uint)tw * P_TEXTURE_FORMAT::BPP - P_TEXTURE_FORMAT::BPP);
        goto fetchSolidLoop;

fetchExactSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    else
    {
      const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

        x += ctx->texture.dx;

        // Fetch before.
        if (x < 0)
        {
          int i = Math::min(-x, w);
          x = 0;
          w -= i;

          uint32_t c0;
          C_PATTERN_INTERPOLATE_32_2(c0,
            tex.fetch(srcBase0), fY0X0,
            tex.fetch(srcBase1), fY1X0);

          dst = PatternCoreC::fill32(dst, c0, i);
          if (!w) goto fetchInnerSkip;
        }

        // Fetch texture.
        if (x < tw)
        {
          const uint8_t* srcCur0 = srcBase0 + (uint)x * P_TEXTURE_FORMAT::BPP;
          const uint8_t* srcCur1 = srcBase1 + (uint)x * P_TEXTURE_FORMAT::BPP;

          int i = Math::min(tw - x, w);
          w -= i;

          do {
            C_PATTERN_INTERPOLATE_32_2(
              ((uint32_t*)dst)[0],
              tex.fetch(srcCur0), fY0X0,
              tex.fetch(srcCur1), fY1X0);

            dst += 4;
            srcCur0 += P_TEXTURE_FORMAT::BPP;
            srcCur1 += P_TEXTURE_FORMAT::BPP;
          } while (--i);
          if (!w) goto fetchInnerSkip;
        }

        // Fetch the rest.
        C_PATTERN_INTERPOLATE_32_2(src0,
          tex.fetch(srcBase0 + (uint)tw * P_TEXTURE_FORMAT::BPP - P_TEXTURE_FORMAT::BPP), fY0X0,
          tex.fetch(srcBase1 + (uint)tw * P_TEXTURE_FORMAT::BPP - P_TEXTURE_FORMAT::BPP), fY1X0);
        goto fetchSolidLoop;

fetchInnerSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    return;

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = src0;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_sub0y_repeat_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X1;

    y += ctx->texture.dy - 1;
    if ((y >= th) | (y <= -th)) y %= th;
    if (y < 0) y += th;

    const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;
    if (y + 1 >= th) srcBase1 = ctx->texture.bits;

    P_TEXTURE_FORMAT tex(ctx);
    const uint8_t* srcCur0 = srcBase0 + (uint)x * P_TEXTURE_FORMAT::BPP;
    const uint8_t* srcCur1 = srcBase1 + (uint)x * P_TEXTURE_FORMAT::BPP;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

      x += ctx->texture.dx;
      if ((x >= tw) | (x <= -tw)) x %= tw;
      if (x < 0) x += tw;

      int r = Math::min(w, tw);
      int i = Math::min(tw - x, r);

      w -= r;

      // Fetch only texture width line.
      for (;;)
      {
        r -= i;

        do {
          C_PATTERN_INTERPOLATE_32_2(
            ((uint32_t*)dst)[0],
            tex.fetch(srcCur0), fY0X0,
            tex.fetch(srcCur1), fY1X0);

          dst += 4;
          srcCur0 += P_TEXTURE_FORMAT::BPP;
          srcCur1 += P_TEXTURE_FORMAT::BPP;
        } while (--i);
        if (!r) break;

        i = Math::min(r, tw);
        srcCur0 = srcBase0;
        srcCur1 = srcBase1;
      }

      // Fetch the rest.
      if (w) dst = PatternCoreC::repeat32(dst, tw, w);

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_sub0y_reflect_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    uint fY0X0 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X1;

    y += ctx->texture.dy - 1;
    if ((y >= th2) | (y <= -th2)) y %= th2;
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

    P_TEXTURE_FORMAT tex(ctx);
    const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcBase1 = ctx->texture.bits + y1 * ctx->texture.stride;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

      x += ctx->texture.dx;
      if ((x >= tw2) | (x <= -tw2)) x %= tw2;
      if (x < 0) x += tw2;

      const uint8_t* srcCur0 = srcBase0 + (uint)x * P_TEXTURE_FORMAT::BPP;
      const uint8_t* srcCur1 = srcBase1 + (uint)x * P_TEXTURE_FORMAT::BPP;

      int r = Math::min(w, tw2);
      w -= r;

      // Fetch only texture width line * 2.
      do {
        // Reflect mode.
        if (x >= tw)
        {
          int i = Math::min(tw2 - x, r);

          srcCur0 = srcBase0 + (uint)(tw2 - x - 1) * P_TEXTURE_FORMAT::BPP;
          srcCur1 = srcBase1 + (uint)(tw2 - x - 1) * P_TEXTURE_FORMAT::BPP;

          r -= i;
          x = 0;

          do {
            C_PATTERN_INTERPOLATE_32_2(
              ((uint32_t*)dst)[0],
              tex.fetch(srcCur0), fY0X0,
              tex.fetch(srcCur1), fY1X0);

            dst += 4;
            srcCur0 -= P_TEXTURE_FORMAT::BPP;
            srcCur1 -= P_TEXTURE_FORMAT::BPP;
          } while (--i);
        }
        // Repeat mode.
        else
        {
          int i = Math::min(tw - x, r);

          srcCur0 = srcBase0 + (uint)x * P_TEXTURE_FORMAT::BPP;
          srcCur1 = srcBase1 + (uint)x * P_TEXTURE_FORMAT::BPP;

          r -= i;
          x += i;

          do {
            C_PATTERN_INTERPOLATE_32_2(
              ((uint32_t*)dst)[0],
              tex.fetch(srcCur0), fY0X0,
              tex.fetch(srcCur1), fY1X0);

            dst += 4;
            srcCur0 += P_TEXTURE_FORMAT::BPP;
            srcCur1 += P_TEXTURE_FORMAT::BPP;
          } while (--i);
        }
      } while (r);

      // Fetch the rest.
      if (w) dst = PatternCoreC::repeat32(dst, tw2, w);

fetchSkip:
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  // --------------------------------------------------------------------------
  // [Pattern - Texture - Fetch - SubXY - 32]
  // --------------------------------------------------------------------------

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_subxy_none_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    y += ctx->texture.dy - 1;

    if ((y < -1) | (y >= th))
    {
      PatternCoreC::fetch_solid(span, buffer, mode, 0x00000000);
      return;
    }

    uint fY0X0 = ctx->texture.fY0X0;
    uint fY0X1 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X0;
    uint fY1X1 = ctx->texture.fY1X1;

    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    if ((y < 0) | (y >= th - 1))
    {
      if (y < 0) { y++; fY0X0 = fY1X0; fY0X1 = fY1X1; }

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

        x += ctx->texture.dx;

        const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
        const uint8_t* src = srcBase;

        uint32_t back_0 = 0x00000000;
        int i;

        // Fetch before.
        if (x < 0)
        {
          i = Math::min(-x, w);
          w -= i;
          x = 0;

          dst = PatternCoreC::fill32(dst, back_0, i);

          if (w) goto fetchBorderFill;
          goto fetchBorderSkip;
        }
        // Fetch texture.
        else if (x <= tw)
        {
          if (x > 0)
          {
            src += (uint)x * P_TEXTURE_FORMAT::BPP;
            back_0 = tex.fetch(src - P_TEXTURE_FORMAT::BPP);
          }

fetchBorderFill:
          i = Math::min(tw - x, w);
          w -= i;

          while (i)
          {
            C_PATTERN_INTERPOLATE_32_2(
              ((uint32_t*)dst)[0],
              back_0                 , fY0X0,
              back_0 = tex.fetch(src), fY0X1)

            dst += 4;
            src += P_TEXTURE_FORMAT::BPP;
            i--;
          }

          // Interpolate last pixel on the row.
          if (!w) goto fetchBorderSkip;
          C_PATTERN_INTERPOLATE_32_2_WITH_ZERO(((uint32_t*)dst)[0], back_0, fY0X0)

          dst += 4;
          if (!--w) goto fetchBorderSkip;
        }

        // Fetch after.
        goto fetchSolidLoop;

fetchBorderSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    else
    {
      if (y < 0) { y++; fY0X0 = fY1X0; fY0X1 = fY1X1; }

      const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcCur0 = srcBase;
      const uint8_t* srcCur1 = srcBase + ctx->texture.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

        x += ctx->texture.dx;

        uint32_t back_0 = 0x00000000;
        uint32_t back_1 = 0x00000000;
        int i;

        // Fetch before.
        if (x < 0)
        {
          i = Math::min(-x, w);
          w -= i;
          x = 0;

          dst = PatternCoreC::fill32(dst, back_0, i);

          if (w) goto fetchInnerFill;
          goto fetchInnerSkip;
        }
        // Fetch texture.
        else if (x <= tw)
        {
          if (x > 0)
          {
            srcCur0 += x * P_TEXTURE_FORMAT::BPP;
            srcCur1 += x * P_TEXTURE_FORMAT::BPP;

            back_0 = tex.fetch(srcCur0 - P_TEXTURE_FORMAT::BPP);
            back_1 = tex.fetch(srcCur1 - P_TEXTURE_FORMAT::BPP);
          }

fetchInnerFill:
          i = Math::min(tw - x, w);
          w -= i;

          while (i)
          {
            C_PATTERN_INTERPOLATE_32_4(
              ((uint32_t*)dst)[0],
              back_0                     , fY0X0,
              back_0 = tex.fetch(srcCur0), fY0X1,
              back_1                     , fY1X0,
              back_1 = tex.fetch(srcCur1), fY1X1)

            dst += 4;
            srcCur0 += P_TEXTURE_FORMAT::BPP;
            srcCur1 += P_TEXTURE_FORMAT::BPP;
            i--;
          }

          // Interpolate last pixel on the row.
          if (!w) goto fetchInnerSkip;

          C_PATTERN_INTERPOLATE_32_2(
            ((uint32_t*)dst)[0],
            back_0, fY0X0,
            back_1, fY1X0)

          dst += 4;
          if (!--w) goto fetchInnerSkip;
        }

        // Fetch after.
        goto fetchSolidLoop;

fetchInnerSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    return;

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = 0x00000000;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_subxy_pad_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X0;
    uint fY0X1 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X0;
    uint fY1X1 = ctx->texture.fY1X1;

    P_TEXTURE_FORMAT tex(ctx);
    uint32_t src0;

    y += ctx->texture.dy - 1;

    if ((y < 0) | (y >= th - 1))
    {
      y = (y < 0) ? 0 : th - 1;

      fY0X0 += fY1X0;
      fY0X1 += fY1X1;

      const uint8_t* srcBase = ctx->texture.bits + y * ctx->texture.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

        x += ctx->texture.dx - 1;

        const uint8_t* src = srcBase;
        uint32_t back_0;
        int i;

        // Fetch before.
        if (x < 0)
        {
          i = Math::min(-x, w);
          w -= i;
          x = 1;

          back_0 = tex.fetch(src);
          dst = PatternCoreC::fill32(dst, back_0, i);
          src += P_TEXTURE_FORMAT::BPP;

          if (w) goto fetchBorderFill;
          goto fetchBorderSkip;
        }
        // Fetch texture.
        else if (x < tw)
        {
          src += (uint)x * P_TEXTURE_FORMAT::BPP;

          back_0 = tex.fetch(src);
          src += P_TEXTURE_FORMAT::BPP;
          if (++x == tw) goto fetchBorderAfter;

fetchBorderFill:
          i = Math::min(tw - x, w);
          w -= i;

          do {
            C_PATTERN_INTERPOLATE_32_2(
              ((uint32_t*)dst)[0],
              back_0                 , fY0X0,
              back_0 = tex.fetch(src), fY0X1);

            dst += 4;
            src += P_TEXTURE_FORMAT::BPP;
          } while (--i);
          if (!w) goto fetchBorderSkip;
        }

fetchBorderAfter:
        // Fetch after.
        src0 = tex.fetch(srcBase + (uint)tw * P_TEXTURE_FORMAT::BPP - P_TEXTURE_FORMAT::BPP);
        goto fetchSolidLoop;

fetchBorderSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    else
    {
      const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
      const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;

      P_FETCH_SPAN8_BEGIN()
        P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

        x += ctx->texture.dx - 1;

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

          back_0 = tex.fetch(srcBase0);
          back_1 = tex.fetch(srcBase1);

          uint32_t c0;
          C_PATTERN_INTERPOLATE_32_2(c0, back_0, fY0X0 + fY0X1, back_1, fY1X0 + fY1X1);
          dst = PatternCoreC::fill32(dst, c0, i);
          if (!w) goto fetchInnerSkip;
          goto fetchInnerFill;
        }
        else if (x < tw - 1)
        {
          srcCur0 += (uint)x * P_TEXTURE_FORMAT::BPP;
          srcCur1 += (uint)x * P_TEXTURE_FORMAT::BPP;

          back_0 = tex.fetch(srcCur0);
          back_1 = tex.fetch(srcCur1);
fetchInnerFill:
          srcCur0 += P_TEXTURE_FORMAT::BPP;
          srcCur1 += P_TEXTURE_FORMAT::BPP;

          i = Math::min(tw - 1 - x, w);
          w -= i;

          do {
            C_PATTERN_INTERPOLATE_32_4(((uint32_t*)dst)[0],
              back_0, fY0X0,
              back_1, fY1X0,
              back_0 = tex.fetch(srcCur0), fY0X1,
              back_1 = tex.fetch(srcCur1), fY1X1);

            dst += 4;
            srcCur0 += P_TEXTURE_FORMAT::BPP;
            srcCur1 += P_TEXTURE_FORMAT::BPP;
          } while (--i);
          if (!w) goto fetchInnerSkip;
        }
        else
        {
          back_0 = tex.fetch(srcBase0 + (uint)tw * P_TEXTURE_FORMAT::BPP - P_TEXTURE_FORMAT::BPP);
          back_1 = tex.fetch(srcBase1 + (uint)tw * P_TEXTURE_FORMAT::BPP - P_TEXTURE_FORMAT::BPP);
        }

        // Fetch the rest.
        C_PATTERN_INTERPOLATE_32_2(src0, back_0, fY0X0 + fY0X1, back_1, fY1X0 + fY1X1)
        goto fetchSolidLoop;

fetchInnerSkip:
        P_FETCH_SPAN8_NEXT()
      P_FETCH_SPAN8_END()
    }
    return;

    // Fetch solid color fast-path.
fetchSolidBegin:
    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()
      do {
fetchSolidLoop:
        ((uint32_t*)dst)[0] = src0;
        dst += 4;
      } while (--w);
      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_subxy_repeat_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    uint fY0X0 = ctx->texture.fY0X0;
    uint fY0X1 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X0;
    uint fY1X1 = ctx->texture.fY1X1;

    y += ctx->texture.dy - 1;
    if ((y >= th) | (y <= -th)) y %= th;
    if (y < 0) y += th;

    const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcBase1 = srcBase0 + ctx->texture.stride;
    if (y + 1 >= th) srcBase1 = ctx->texture.bits;

    P_TEXTURE_FORMAT tex(ctx);

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

      x += ctx->texture.dx - 1;
      if ((x >= tw) | (x <= -tw)) x %= tw;
      if (x < 0) x += tw;

      const uint8_t* srcCur0 = srcBase0 + (uint)x * P_TEXTURE_FORMAT::BPP;
      const uint8_t* srcCur1 = srcBase1 + (uint)x * P_TEXTURE_FORMAT::BPP;

      uint32_t back_0 = tex.fetch(srcCur0);
      uint32_t back_1 = tex.fetch(srcCur1);

      srcCur0 += P_TEXTURE_FORMAT::BPP;
      srcCur1 += P_TEXTURE_FORMAT::BPP;

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
          C_PATTERN_INTERPOLATE_32_4(((uint32_t*)dst)[0],
            back_0, fY0X0,
            back_1, fY1X0,
            back_0 = tex.fetch(srcCur0), fY0X1,
            back_1 = tex.fetch(srcCur1), fY1X1);

          dst += 4;
          srcCur0 += P_TEXTURE_FORMAT::BPP;
          srcCur1 += P_TEXTURE_FORMAT::BPP;
        } while (--i);
        if (!r) break;

        i = Math::min(r, tw);
        srcCur0 = srcBase0;
        srcCur1 = srcBase1;
      }

      // Fetch the rest.
      if (w) dst = PatternCoreC::repeat32(dst, tw, w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_subxy_reflect_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw << 1;
    int th2 = th << 1;

    uint fY0X0 = ctx->texture.fY0X0;
    uint fY0X1 = ctx->texture.fY0X1;
    uint fY1X0 = ctx->texture.fY1X0;
    uint fY1X1 = ctx->texture.fY1X1;

    y += ctx->texture.dy - 1;
    if ((y >= th2) | (y <= -th2)) y %= th2;
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

    P_TEXTURE_FORMAT tex(ctx);
    const uint8_t* srcBase0 = ctx->texture.bits + y * ctx->texture.stride;
    const uint8_t* srcBase1 = ctx->texture.bits + y1 * ctx->texture.stride;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(4)

      x += ctx->texture.dx - 1;
      if ((x >= tw2) | (x <= -tw2)) x %= tw2;
      if (x < 0) x += tw2;

      const uint8_t* srcCur0;
      const uint8_t* srcCur1;

      uint32_t back_0;
      uint32_t back_1;

      if (x >= tw)
      {
        // Reflect mode.
        srcCur0 = srcBase0 + (uint)(tw2 - x - 1) * P_TEXTURE_FORMAT::BPP;
        srcCur1 = srcBase1 + (uint)(tw2 - x - 1) * P_TEXTURE_FORMAT::BPP;
      }
      else
      {
        // Repeat mode.
        srcCur0 = srcBase0 + (uint)x * P_TEXTURE_FORMAT::BPP;
        srcCur1 = srcBase1 + (uint)x * P_TEXTURE_FORMAT::BPP;
      }

      back_0 = tex.fetch(srcCur0);
      back_1 = tex.fetch(srcCur1);

      if (++x >= tw2) x -= tw2;

      int r = Math::min(w, tw2);
      w -= r;

      // Fetch only texture width line * 2.
      do {
        // Reflect mode.
        if (x >= tw)
        {
          int i = Math::min(tw2 - x, r);

          srcCur0 = srcBase0 + (uint)(tw2 - x - 1) * P_TEXTURE_FORMAT::BPP;
          srcCur1 = srcBase1 + (uint)(tw2 - x - 1) * P_TEXTURE_FORMAT::BPP;

          r -= i;
          x = 0;

          do {
            C_PATTERN_INTERPOLATE_32_4(((uint32_t*)dst)[0],
              back_0, fY0X0,
              back_1, fY1X0,
              back_0 = tex.fetch(srcCur0), fY0X1,
              back_1 = tex.fetch(srcCur1), fY1X1);

            dst += 4;
            srcCur0 -= P_TEXTURE_FORMAT::BPP;
            srcCur1 -= P_TEXTURE_FORMAT::BPP;
          } while (--i);
        }
        // Repeat mode.
        else
        {
          int i = Math::min(tw - x, r);

          srcCur0 = srcBase0 + (uint)x * P_TEXTURE_FORMAT::BPP;
          srcCur1 = srcBase1 + (uint)x * P_TEXTURE_FORMAT::BPP;

          r -= i;
          x += i;

          do {
            C_PATTERN_INTERPOLATE_32_4(((uint32_t*)dst)[0],
              back_0, fY0X0,
              back_1, fY1X0,
              back_0 = tex.fetch(srcCur0), fY0X1,
              back_1 = tex.fetch(srcCur1), fY1X1);

            dst += 4;
            srcCur0 += P_TEXTURE_FORMAT::BPP;
            srcCur1 += P_TEXTURE_FORMAT::BPP;
          } while (--i);
        }
      } while (r);

      // Fetch the rest.
      if (w) dst = PatternCoreC::repeat32(dst, tw2, w);

      P_FETCH_SPAN8_NEXT()
    P_FETCH_SPAN8_END()
  }

  // --------------------------------------------------------------------------
  // [Pattern - Texture - Fetch - Transform, Nearest]
  // --------------------------------------------------------------------------

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_trans_affine_nearest_none_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int fx = Math::doubleToFixed16x16(rx * ctx->inv._00 + ry * ctx->inv._10 + ctx->inv._20);
    int fy = Math::doubleToFixed16x16(rx * ctx->inv._01 + ry * ctx->inv._11 + ctx->inv._21);

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    fx -= 0x8000;
    fy -= 0x8000;

    const uint8_t* srcBase = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        uint32_t c0 = 0x00000000;

        int px0 = fx >> 16;
        int py0 = fy >> 16;

        if (FOG_LIKELY(((uint)px0 < (uint)tw) & ((uint)py0 < (uint)th)))
        {
          c0 = tex.fetch(srcBase + (uint)py0 * srcStride + (uint)px0 * P_TEXTURE_FORMAT::BPP);
        }

        ((uint32_t*)dst)[0] = c0;
        dst += 4;

        fx += dx;
        fy += dy;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += dy * hole;
      })
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_trans_affine_nearest_pad_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int fx = Math::doubleToFixed16x16(rx * ctx->inv._00 + ry * ctx->inv._10 + ctx->inv._20);
    int fy = Math::doubleToFixed16x16(rx * ctx->inv._01 + ry * ctx->inv._11 + ctx->inv._21);

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    fx -= 0x8000;
    fy -= 0x8000;

    const uint8_t* srcBase = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    tw--;
    th--;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        int px0 = fx >> 16;
        int py0 = fy >> 16;

        if (px0 < 0) px0 = 0; else if (px0 > tw) px0 = tw;
        if (py0 < 0) py0 = 0; else if (py0 > th) py0 = th;

        ((uint32_t*)dst)[0] = tex.fetch(srcBase + (uint)py0 * srcStride + (uint)px0 * P_TEXTURE_FORMAT::BPP);
        dst += 4;

        fx += dx;
        fy += dy;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += dy * hole;
      })
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_trans_affine_nearest_repeat_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int fx = Math::doubleToFixed16x16(rx * ctx->inv._00 + ry * ctx->inv._10 + ctx->inv._20);
    int fy = Math::doubleToFixed16x16(rx * ctx->inv._01 + ry * ctx->inv._11 + ctx->inv._21);

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

    const uint8_t* srcBase = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        int px0 = fx >> 16;
        int py0 = fy >> 16;

        ((uint32_t*)dst)[0] = tex.fetch(srcBase + (uint)py0 * srcStride + (uint)px0 * P_TEXTURE_FORMAT::BPP);
        dst += 4;

        fx += dx;
        fy += dy;

        if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
        if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += dy * hole;

        if (fx <= -fxmax || fx >= fxmax) fx %= fxmax;
        if (fy <= -fymax || fy >= fymax) fy %= fymax;

        if (fx < 0) fx += fxmax;
        if (fy < 0) fy += fymax;
      })
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_trans_affine_nearest_reflect_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int fx = Math::doubleToFixed16x16(rx * ctx->inv._00 + ry * ctx->inv._10 + ctx->inv._20);
    int fy = Math::doubleToFixed16x16(rx * ctx->inv._01 + ry * ctx->inv._11 + ctx->inv._21);

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

    const uint8_t* srcBase = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

      do {
        int px0 = fx >> 16;
        int py0 = fy >> 16;

        if (px0 >= tw) px0 = tw2 - px0;
        if (py0 >= th) py0 = th2 - py0;

        ((uint32_t*)dst)[0] = tex.fetch(srcBase + (uint)py0 * srcStride + (uint)px0 * P_TEXTURE_FORMAT::BPP);
        dst += 4;

        fx += dx;
        fy += dy;

        if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
        if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        if (fx <= -fxmax || fx >= fxmax) fx %= fxmax;
        if (fy <= -fymax || fy >= fymax) fy %= fymax;

        if (fx < 0) fx += fxmax;
        if (fy < 0) fy += fymax;

      })
    P_FETCH_SPAN8_END()
  }

  // --------------------------------------------------------------------------
  // [Pattern - Texture - Fetch - Transform, Bilinear]
  // --------------------------------------------------------------------------

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_trans_affine_bilinear_none_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx = Math::doubleToFixed16x16(rx * ctx->inv._00 + ry * ctx->inv._10 + ctx->inv._20);
    int fy = Math::doubleToFixed16x16(rx * ctx->inv._01 + ry * ctx->inv._11 + ctx->inv._21);

    fx -= 0x8000;
    fy -= 0x8000;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    tw--;
    th--;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

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

            pix_x0y0 = (((uint)px0 <= (uint)tw)) ? tex.fetch(src0 + (uint)px0 * P_TEXTURE_FORMAT::BPP) : 0x00000000;
            pix_x1y0 = (((uint)px1 <= (uint)tw)) ? tex.fetch(src0 + (uint)px1 * P_TEXTURE_FORMAT::BPP) : 0x00000000;
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

            pix_x0y1 = (((uint)px0 <= (uint)tw)) ? tex.fetch(src0 + (uint)px0 * P_TEXTURE_FORMAT::BPP) : 0x00000000;
            pix_x1y1 = (((uint)px1 <= (uint)tw)) ? tex.fetch(src0 + (uint)px1 * P_TEXTURE_FORMAT::BPP) : 0x00000000;
          }
          else
          {
            pix_x0y1 = 0x00000000;
            pix_x1y1 = 0x00000000;
          }
        }
        else
        {
          const uint8_t* src0 = srcBits + py0 * srcStride + px0 * P_TEXTURE_FORMAT::BPP;

          pix_x0y0 = tex.fetch(src0          );
          pix_x1y0 = tex.fetch(src0 + P_TEXTURE_FORMAT::BPP);
          src0 += srcStride;
          pix_x0y1 = tex.fetch(src0          );
          pix_x1y1 = tex.fetch(src0 + P_TEXTURE_FORMAT::BPP);
        }

        uint weightx = (fx >> 8) & 0xFF;
        uint weighty = (fy >> 8) & 0xFF;

        C_PATTERN_INTERPOLATE_32_4(((uint32_t*)dst)[0],
          pix_x0y0, ((0x100 - weightx) * (0x100 - weighty)) >> 8,
          pix_x1y0, ((weightx        ) * (0x100 - weighty)) >> 8,
          pix_x0y1, ((0x100 - weightx) * (weighty        )) >> 8,
          pix_x1y1, ((weightx        ) * (weighty        )) >> 8);
        dst += 4;

        fx += dx;
        fy += dy;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += dy * hole;
      })
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_trans_affine_bilinear_pad_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx = Math::doubleToFixed16x16(rx * ctx->inv._00 + ry * ctx->inv._10 + ctx->inv._20);
    int fy = Math::doubleToFixed16x16(rx * ctx->inv._01 + ry * ctx->inv._11 + ctx->inv._21);

    fx -= 0x8000;
    fy -= 0x8000;

    const uint8_t* srcBits = ctx->texture.bits;
    sysint_t srcStride = ctx->texture.stride;

    tw--;
    th--;

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

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

          pix_x0y0 = tex.fetch(src0 + (uint)px0 * P_TEXTURE_FORMAT::BPP);
          pix_x1y0 = tex.fetch(src0 + (uint)px1 * P_TEXTURE_FORMAT::BPP);
          pix_x0y1 = tex.fetch(src1 + (uint)px0 * P_TEXTURE_FORMAT::BPP);
          pix_x1y1 = tex.fetch(src1 + (uint)px1 * P_TEXTURE_FORMAT::BPP);
        }
        else
        {
          const uint8_t* src0 = srcBits + py0 * srcStride + (uint)px0 * P_TEXTURE_FORMAT::BPP;

          pix_x0y0 = tex.fetch(src0          );
          pix_x1y0 = tex.fetch(src0 + P_TEXTURE_FORMAT::BPP);
          src0 += srcStride;
          pix_x0y1 = tex.fetch(src0          );
          pix_x1y1 = tex.fetch(src0 + P_TEXTURE_FORMAT::BPP);
        }

        uint weightx = (fx >> 8) & 0xFF;
        uint weighty = (fy >> 8) & 0xFF;

        C_PATTERN_INTERPOLATE_32_4(((uint32_t*)dst)[0],
          pix_x0y0, ((0x100 - weightx) * (0x100 - weighty)) >> 8,
          pix_x1y0, ((weightx        ) * (0x100 - weighty)) >> 8,
          pix_x0y1, ((0x100 - weightx) * (weighty        )) >> 8,
          pix_x1y1, ((weightx        ) * (weighty        )) >> 8);
        dst += 4;

        fx += dx;
        fy += dy;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += dy * hole;
      })
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_trans_affine_bilinear_repeat_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx = Math::doubleToFixed16x16(rx * ctx->inv._00 + ry * ctx->inv._10 + ctx->inv._20);
    int fy = Math::doubleToFixed16x16(rx * ctx->inv._01 + ry * ctx->inv._11 + ctx->inv._21);

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

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

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

        pix_x0y0 = tex.fetch(src0 + (uint)px0 * P_TEXTURE_FORMAT::BPP);
        pix_x1y0 = tex.fetch(src0 + (uint)px1 * P_TEXTURE_FORMAT::BPP);
        pix_x0y1 = tex.fetch(src1 + (uint)px0 * P_TEXTURE_FORMAT::BPP);
        pix_x1y1 = tex.fetch(src1 + (uint)px1 * P_TEXTURE_FORMAT::BPP);

        C_PATTERN_INTERPOLATE_32_4(((uint32_t*)dst)[0],
          pix_x0y0, ((0x100 - weightx) * (0x100 - weighty)) >> 8,
          pix_x1y0, ((weightx        ) * (0x100 - weighty)) >> 8,
          pix_x0y1, ((0x100 - weightx) * (weighty        )) >> 8,
          pix_x1y1, ((weightx        ) * (weighty        )) >> 8);
        dst += 4;

        fx += dx;
        fy += dy;

        if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
        if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += dy * hole;

        if (fx <= -fxmax || fx >= fxmax) fx %= fxmax;
        if (fy <= -fymax || fy >= fymax) fy %= fymax;

        if (fx < 0) fx += fxmax;
        if (fy < 0) fy += fymax;
      })
    P_FETCH_SPAN8_END()
  }

  template<typename P_TEXTURE_FORMAT>
  static void FOG_FASTCALL fetch_trans_affine_bilinear_reflect_32(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    P_FETCH_SPAN8_INIT()
    P_TEXTURE_FORMAT tex(ctx);

    double rx = (double)x + 0.5;
    double ry = (double)y + 0.5;

    int tw = ctx->texture.w;
    int th = ctx->texture.h;

    int tw2 = tw * 2 - 1;
    int th2 = th * 2 - 1;

    int dx = ctx->texture.dx;
    int dy = ctx->texture.dy;

    int fx = Math::doubleToFixed16x16(rx * ctx->inv._00 + ry * ctx->inv._10 + ctx->inv._20);
    int fy = Math::doubleToFixed16x16(rx * ctx->inv._01 + ry * ctx->inv._11 + ctx->inv._21);

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

    P_FETCH_SPAN8_BEGIN()
      P_FETCH_SPAN8_SET_CURRENT()

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

        uint32_t pix_x0y0 = tex.fetch(src0 + (uint)px0 * P_TEXTURE_FORMAT::BPP);
        uint32_t pix_x1y0 = tex.fetch(src0 + (uint)px1 * P_TEXTURE_FORMAT::BPP);
        uint32_t pix_x0y1 = tex.fetch(src1 + (uint)px0 * P_TEXTURE_FORMAT::BPP);
        uint32_t pix_x1y1 = tex.fetch(src1 + (uint)px1 * P_TEXTURE_FORMAT::BPP);

        C_PATTERN_INTERPOLATE_32_4(((uint32_t*)dst)[0],
          pix_x0y0, ((0x100 - weightx) * (0x100 - weighty)) >> 8,
          pix_x1y0, ((weightx        ) * (0x100 - weighty)) >> 8,
          pix_x0y1, ((0x100 - weightx) * (weighty        )) >> 8,
          pix_x1y1, ((weightx        ) * (weighty        )) >> 8);
        dst += 4;

        fx += dx;
        fy += dy;

        if (FOG_UNLIKELY((uint)fx >= (uint)fxmax)) fx += fxrewind;
        if (FOG_UNLIKELY((uint)fy >= (uint)fymax)) fy += fyrewind;
      } while (--w);

      P_FETCH_SPAN8_HOLE(
      {
        fx += dx * hole;
        fy += dy * hole;

        if ((fx <= -fxmax) | (fx >= fxmax)) fx %= fxmax;
        if ((fy <= -fymax) | (fy >= fymax)) fy %= fymax;

        if (fx < 0) fx += fxmax;
        if (fy < 0) fy += fymax;
      })
    P_FETCH_SPAN8_END()
  }
};

} // RasterEngine namespace
} // Fog namespace
