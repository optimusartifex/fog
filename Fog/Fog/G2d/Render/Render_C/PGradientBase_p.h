// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDER_C_PGRADIENTBASE_P_H
#define _FOG_G2D_RENDER_RENDER_C_PGRADIENTBASE_P_H

// [Dependencies]
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Render/Render_C/Defs_p.h>
#include <Fog/G2d/Render/Render_C/Helpers_p.h>

namespace Fog {
namespace Render_C {

// ============================================================================
// [Fog::Render_C - PGradientBase]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT PGradientBase
{
  // ==========================================================================
  // [Interpolate]
  // ==========================================================================

  static void FOG_FASTCALL interpolate_prgb32(uint8_t* _dst, int _w, const ColorStop* stops, sysuint_t length)
  {
    FOG_ASSUME(length >= 1);

    if (length == 1)
    {
      Helpers::p_fill_prgb32(_dst, stops[0].getArgb32(), _w);
      return;
    }

    _w--;

    uint p0 = 0;
    uint p1;

    Face::p32 c0 = stops[0].getArgb32();
    Face::p32 c1;

    float wf = (float)(_w << 8);

    sysuint_t pos;
    for (pos = 0; pos < length; pos++)
    {
      c1 = stops[pos].getArgb32();
      p1 = Math::uround(stops[pos].getOffset() * wf);
      uint len = (p1 >> 8) - (p0 >> 8);

      uint32_t* dst = reinterpret_cast<uint32_t*>(_dst) + (p0 >> 8);
      uint i;

      if (len > 0)
      {
        if (c0 == c1)
        {
          Face::p32 cp;
          Face::p32PRGB32FromARGB32(cp, c0);

          for (i = 0; i < len; i++) dst[i] = cp;
        }
        else
        {
          Face::p32 aPos = (c0 & 0xFF000000);
          Face::p32 rPos = (c0 & 0x00FF0000) <<  8;
          Face::p32 gPos = (c0 & 0x0000FF00) << 16;
          Face::p32 bPos = (c0             ) << 24;

          Face::p32 aInc = (c1 & 0xFF000000);
          Face::p32 rInc = (c1 & 0x00FF0000) <<  8;
          Face::p32 gInc = (c1 & 0x0000FF00) << 16;
          Face::p32 bInc = (c1             ) << 24;

          Face::p32 mask = 0;

          if (aPos > aInc) { aPos ^= 0xFF000000; aInc ^= 0xFF000000; mask ^= 0xFF000000; }
          if (rPos > rInc) { rPos ^= 0xFF000000; rInc ^= 0xFF000000; mask ^= 0x00FF0000; }
          if (gPos > gInc) { gPos ^= 0xFF000000; gInc ^= 0xFF000000; mask ^= 0x0000FF00; }
          if (bPos > bInc) { bPos ^= 0xFF000000; bInc ^= 0xFF000000; mask ^= 0x000000FF; }

          aInc -= aPos; rInc -= rPos;
          gInc -= gPos; bInc -= bPos;

          aInc /= len; rInc /= len;
          gInc /= len; bInc /= len;

          aPos += 0x800000; rPos += 0x800000;
          gPos += 0x800000; bPos += 0x800000;

          if (aInc == 0 && Face::p32ARGB32IsAlphaFF(c1))
          {
            mask |= 0xFF000000;

            for (i = 0; i <= len; i++)
            {
              Face::p32 cp = _FOG_FACE_COMBINE_3((rPos & 0xFF000000) >>  8,
                                                 (gPos & 0xFF000000) >> 16,
                                                 (bPos             ) >> 24) ^ mask;
              dst[i] = cp;

              rPos += rInc;
              gPos += gInc;
              bPos += bInc;
            }
          }
          else
          {
            for (i = 0; i <= len; i++)
            {
              Face::p32 cp = _FOG_FACE_COMBINE_4((aPos & 0xFF000000)      ,
                                                 (rPos & 0xFF000000) >>  8,
                                                 (gPos & 0xFF000000) >> 16,
                                                 (bPos             ) >> 24) ^ mask;
              Face::p32PRGB32FromARGB32(cp, cp);
              dst[i] = cp;

              aPos += aInc;
              rPos += rInc;
              gPos += gInc;
              bPos += bInc;
            }
          }
        }
      }
      else
      {
        Face::p32 cp;
        Face::p32PRGB32FromARGB32(cp, c1);

        dst[0] = cp;
      }

      c0 = c1;
      p0 = p1;
    }

    p1 >>= 8;
    if (p1 < (uint)_w)
    {
      Face::p32 cp;
      Face::p32PRGB32FromARGB32(cp, c1);

      uint32_t* dst = reinterpret_cast<uint32_t*>(_dst) + p1;
      uint i, len = (uint)_w - p1 + 1;
      for (i = 0; i < len; i++) dst[i] = cp;
    }
  }

  // ==========================================================================
  // [Create / Destroy]
  // ==========================================================================

  static err_t FOG_FASTCALL create(
    RenderPatternContext* ctx, uint32_t dstFormat, const BoxI& boundingBox,
    uint32_t spread, const ColorStopList& stops)
  {
    FOG_ASSERT(spread < GRADIENT_SPREAD_COUNT);

    switch (dstFormat)
    {
      case IMAGE_FORMAT_PRGB32:
      case IMAGE_FORMAT_XRGB32:
      case IMAGE_FORMAT_RGB24:
      {
        // Get whether the gradient is opaque or not.
        bool isOpaque = stops.isOpaque_ARGB32();
        // Decide which pixel format to use.
        uint32_t srcFormat = isOpaque ? IMAGE_FORMAT_XRGB32 : IMAGE_FORMAT_PRGB32;

        // Get or create the color-table (ColorStopCache instance).
        ColorStopCache* cache = AtomicCore<ColorStopCache*>::get(&stops._d->stopCachePrgb32);
        if (cache != NULL)
        {
          cache->refCount.inc();
        }
        else
        {
          // Try to create the color-stop cache.
          cache = ColorStopCache::create32(srcFormat, get_optimal_cache_length(stops));
          if (FOG_IS_NULL(cache)) return ERR_RT_OUT_OF_MEMORY;

          cache->refCount.init(2);

          _g2d_render.gradient.interpolate[srcFormat](
            reinterpret_cast<uint8_t*>(cache->getData()), cache->getLength(), stops.getList(), stops.getLength());

          // Assign also the end point.
          uint32_t* table = reinterpret_cast<uint32_t*>(cache->getData());
          table[cache->getLength()] = table[cache->getLength() - 1];

          // Try to add it back to the ColorStopList instance. If we failed then
          // some other thread was faster than us, in this case it's needed to
          // decrease the reference count of the instance we created.
          if (!AtomicCore<ColorStopCache*>::cmpXchg(&stops._d->stopCachePrgb32, (ColorStopCache*)NULL, cache))
            cache->refCount.dec();
        }

        // Setup the context.
        ctx->_initDst(dstFormat);
        ctx->_srcFormat = srcFormat;
        ctx->_srcBPP = 4;
        ctx->_boundingBox = boundingBox;

        ctx->_d.gradient.base.cache = cache;
        ctx->_d.gradient.base.table = cache->getData();
        ctx->_d.gradient.base.len = cache->getLength();
        ctx->_d.gradient.base.len16x16 = ctx->_d.gradient.base.len << 16;
        break;
      }

      // TODO: Support A8 and 64-bit formats.
      default:
        FOG_ASSERT_NOT_REACHED();
    }

    return ERR_OK;
  }

  static void FOG_FASTCALL destroy(
    RenderPatternContext* ctx)
  {
    FOG_ASSERT(ctx->isInitialized());

    ctx->_d.gradient.base.cache->deref();
    ctx->reset();
  }

  // ==========================================================================
  // [Helpers - Cache]
  // ==========================================================================

  static int FOG_FASTCALL get_optimal_cache_length(const ColorStopList& stops)
  {
    sysuint_t len = stops.getLength();
    if (len == 2)
    {
      float diff = stops.getAt(1).getOffset() - stops.getAt(0).getOffset();
      return (diff == 1.0f) ? 128 : (diff >= 0.5f) ? 256 : 512;
    }
    else if (len == 3)
    {
      return (stops.getAt(0).getOffset() == 0.0f && 
              stops.getAt(1).getOffset() == 0.5f &&
              stops.getAt(2).getOffset() == 1.0f) ? 256 : 512;
    }
    else
    {
      float oPrev = 0.0f;
      float maxDiff = 1.0f;

      // First get the minimal difference between stops.
      for (sysuint_t i = 0; i < len; i++)
      {
        float oStop = stops.getAt(i).getOffset();
        float oDiff = oStop - oPrev;
        if (maxDiff > oDiff) maxDiff = oDiff;
      }

      return (maxDiff < 0.02f) ? 1024 : 512;
    }
  }
};

// ============================================================================
// [Fog::Render_C - PGradientAccessor_PRGB32_Base]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_PRGB32_Base
{
  typedef Face::p32 Pixel;
  enum { DST_BPP = 4 };

  FOG_INLINE PGradientAccessor_PRGB32_Base(const RenderPatternContext* ctx) :
    _table(reinterpret_cast<const uint32_t*>(ctx->_d.gradient.base.table))
  {
  }

  FOG_INLINE void fetchTable(Pixel& dst, int position)
  {
    dst = _table[position];
  }

  FOG_INLINE void store(uint8_t* dst, const Pixel& src)
  {
    Face::p32Store4aNative(dst, src);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const uint32_t* _table;
};

// ============================================================================
// [Fog::Render_C - PGradientAccessor_PRGB32_Pad]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_PRGB32_Pad : public PGradientAccessor_PRGB32_Base
{
  FOG_INLINE PGradientAccessor_PRGB32_Pad(const RenderPatternContext* ctx) :
    PGradientAccessor_PRGB32_Base(ctx),
    _len(ctx->_d.gradient.base.len),
    _len_d(ctx->_d.gradient.base.len)
  {
  }

  FOG_INLINE void fetchAtD(Pixel& dst, double d)
  {
    if (d < 0.0)
      d = 0.0;
    else if (d > _len_d)
      d = _len_d;
    fetchTable(dst, (int)d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int _len;
  double _len_d;
};

// ============================================================================
// [Fog::Render_C - PGradientAccessor_PRGB32_Repeat]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_PRGB32_Repeat : public PGradientAccessor_PRGB32_Base
{
  FOG_INLINE PGradientAccessor_PRGB32_Repeat(const RenderPatternContext* ctx) :
    PGradientAccessor_PRGB32_Base(ctx),
    _lenMask(ctx->_d.gradient.base.len - 1)
  {
  }

  FOG_INLINE void fetchAtD(Pixel& dst, double d)
  {
    fetchTable(dst, (int)d & _lenMask);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint _lenMask;
};

// ============================================================================
// [Fog::Render_C - PGradientAccessor_PRGB32_Reflect]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_PRGB32_Reflect : public PGradientAccessor_PRGB32_Base
{
  FOG_INLINE PGradientAccessor_PRGB32_Reflect(const RenderPatternContext* ctx) :
    PGradientAccessor_PRGB32_Base(ctx),
    _len(ctx->_d.gradient.base.len),
    _lenMask2(ctx->_d.gradient.base.len * 2 - 1)
  {
  }

  FOG_INLINE void fetchAtD(Pixel& dst, double d)
  {
    uint i = (int)d;

    i &= _lenMask2;
    if (i > (uint)_len) i ^= _lenMask2;
    fetchTable(dst, i);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int _len;
  uint _lenMask2;
};

} // Render_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_PGRADIENTBASE_P_H
