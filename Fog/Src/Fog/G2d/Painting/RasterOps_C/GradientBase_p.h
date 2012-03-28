// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_GRADIENTBASE_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_GRADIENTBASE_P_H

// [Dependencies]
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Painting/RasterOps_C/BaseDefs_p.h>
#include <Fog/G2d/Painting/RasterOps_C/BaseHelpers_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - PGradientBase]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT PGradientBase
{
  // ==========================================================================
  // [Interpolate]
  // ==========================================================================

  static void FOG_FASTCALL interpolate_prgb32(uint8_t* _dst, int _w, const ColorStop* stops, size_t length)
  {
    FOG_ASSUME(length >= 1);

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    uint32_t c0 = stops[0].getArgb32();
    uint32_t c1;

    if (length == 1)
    {
      Acc::p32PRGB32FromARGB32(c0, c0);
      Helpers::p_fill_prgb32(_dst, c0, _w);
      return;
    }

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    _w--;

    uint p0 = 0;
    uint p1;

    float wf = (float)(_w << 8);

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    size_t pos;
    for (pos = 0; pos < length; pos++)
    {
      c1 = stops[pos].getArgb32();
      p1 = Math::uround(stops[pos].getOffset() * wf);
      uint len = (p1 >> 8) - (p0 >> 8);

      uint32_t* dst = reinterpret_cast<uint32_t*>(_dst) + (p0 >> 8);

      if (len > 0)
      {
        uint i = len + 1;

        if (c0 == c1)
        {
          uint32_t cp;
          Acc::p32PRGB32FromARGB32(cp, c0);

          do {
            dst[0] = cp;
            dst++;
          } while (--i);
        }
        else
        {
          uint32_t aPos = (c0 & 0xFF000000);
          uint32_t rPos = (c0 & 0x00FF0000) <<  8;
          uint32_t gPos = (c0 & 0x0000FF00) << 16;
          uint32_t bPos = (c0             ) << 24;

          uint32_t aInc = (c1 & 0xFF000000);
          uint32_t rInc = (c1 & 0x00FF0000) <<  8;
          uint32_t gInc = (c1 & 0x0000FF00) << 16;
          uint32_t bInc = (c1             ) << 24;

          uint32_t mask = 0;

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

          if (aInc == 0 && Acc::p32ARGB32IsAlphaFF(c1))
          {
            mask |= 0xFF000000;

            do {
              dst[0] =  _FOG_ACC_COMBINE_3((rPos & 0xFF000000) >>  8,
                                           (gPos & 0xFF000000) >> 16,
                                           (bPos             ) >> 24) ^ mask;
              dst++;

              rPos += rInc;
              gPos += gInc;
              bPos += bInc;
            } while (--i);
          }
          else
          {
            uint32_t mask0 = mask & 0x00FF00FFU;
            uint32_t mask1 = mask >> 8;

            do {
              uint32_t t0 = _FOG_ACC_COMBINE_2((rPos & 0xFF000000) >> 8, bPos >> 24);
              uint32_t t1 = (gPos        ) >> 24;
              uint32_t ta = (aPos ^ mask ) >> 24;

              t0 ^= mask0;
              t1 ^= mask1;

              Acc::p32MulDiv255PBW_SBW_2x_Pack_20Z1(t0, t0, ta, t1, ta);
              dst[0] = _FOG_ACC_COMBINE_2(t0, ta << 24);
              dst++;

              aPos += aInc;
              rPos += rInc;
              gPos += gInc;
              bPos += bInc;
            } while (--i);
          }
        }
      }
      else
      {
        uint32_t t0;
        Acc::p32PRGB32FromARGB32(t0, c1);
        dst[0] = t0;
      }

      c0 = c1;
      p0 = p1;
    }

    p1 >>= 8;
    if (p1 < (uint)_w)
    {
      uint32_t cp;
      Acc::p32PRGB32FromARGB32(cp, c1);

      uint32_t* dst = reinterpret_cast<uint32_t*>(_dst) + p1;
      uint i = (uint)_w - p1 + 1;
      FOG_ASSUME(i > 0);

      do {
        dst[0] = cp;
        dst++;
      } while (--i);
    }
  }

  // ==========================================================================
  // [Create / Destroy]
  // ==========================================================================

  static err_t FOG_FASTCALL create(
    RasterPattern* ctx, uint32_t dstFormat, const BoxI* boundingBox,
    uint32_t spread, const ColorStopList* stops)
  {
    FOG_ASSERT(spread < GRADIENT_SPREAD_COUNT);

    switch (dstFormat)
    {
      case IMAGE_FORMAT_PRGB32:
      case IMAGE_FORMAT_XRGB32:
      case IMAGE_FORMAT_RGB24:
      {
        // Get whether the gradient is opaque or not.
        bool isOpaque = stops->isOpaqueARGB32();
        // Decide which pixel format to use.
        uint32_t srcFormat = isOpaque ? IMAGE_FORMAT_XRGB32 : IMAGE_FORMAT_PRGB32;

        // Get or create the color-table (ColorStopCache instance).
        ColorStopCache* cache = AtomicCore<ColorStopCache*>::get(&stops->_d->stopCachePrgb32);
        if (cache != NULL)
        {
          cache->reference.inc();
        }
        else
        {
          // Try to create the color-stop cache.
          cache = ColorStopCache::create32(srcFormat, get_optimal_cache_length(stops));
          if (FOG_IS_NULL(cache)) return ERR_RT_OUT_OF_MEMORY;

          cache->reference.init(2);

          _api_raster.gradient.interpolate[srcFormat](
            reinterpret_cast<uint8_t*>(cache->getData()), cache->getLength(), stops->getList(), stops->getLength());

          // Assign also the end point.
          uint32_t* table = reinterpret_cast<uint32_t*>(cache->getData());
          table[cache->getLength()] = table[cache->getLength() - 1];

          // Try to add it back to the ColorStopList instance. If we failed then
          // some other thread was faster than us, in this case it's needed to
          // decrease the reference count of the instance we created.
          if (!AtomicCore<ColorStopCache*>::cmpXchg(&stops->_d->stopCachePrgb32, (ColorStopCache*)NULL, cache))
            cache->reference.dec();
        }

        // Setup the context.
        ctx->_initDst(dstFormat);
        ctx->_srcFormat = srcFormat;
        ctx->_srcBPP = 4;
        ctx->_isOpaque = isOpaque;
        ctx->_boundingBox = *boundingBox;

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
    RasterPattern* ctx)
  {
    FOG_ASSERT(ctx->isInitialized());

    ctx->_d.gradient.base.cache->release();
    ctx->reset();
  }

  // ==========================================================================
  // [Helpers - Cache]
  // ==========================================================================

  static int FOG_FASTCALL get_optimal_cache_length(const ColorStopList* stops)
  {
    size_t len = stops->getLength();
    if (len == 2)
    {
      float diff = stops->getAt(1).getOffset() - stops->getAt(0).getOffset();
      return (diff == 1.0f) ? 128 : (diff >= 0.5f) ? 256 : 512;
    }
    else if (len == 3)
    {
      return (stops->getAt(0).getOffset() == 0.0f &&
              stops->getAt(1).getOffset() == 0.5f &&
              stops->getAt(2).getOffset() == 1.0f) ? 256 : 512;
    }
    else
    {
      float oPrev = 0.0f;
      float maxDiff = 1.0f;

      // First get the minimal difference between stops.
      for (size_t i = 0; i < len; i++)
      {
        float oStop = stops->getAt(i).getOffset();
        float oDiff = oStop - oPrev;
        if (maxDiff > oDiff) maxDiff = oDiff;
      }

      return (maxDiff < 0.02f) ? 1024 : 512;
    }
  }
};

// ============================================================================
// [Fog::RasterOps_C - PGradientAccessor_PRGB32_Base]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_PRGB32_Base
{
  typedef uint32_t Pixel;
  enum { DST_BPP = 4 };

  FOG_INLINE PGradientAccessor_PRGB32_Base(const RasterPattern* ctx) :
    _table(reinterpret_cast<const uint32_t*>(ctx->_d.gradient.base.table)) {}

  FOG_INLINE void fetchRaw(Pixel& dst, int position) { dst = _table[position]; }
  FOG_INLINE void storePix(uint8_t* dst, const Pixel& src) { Acc::p32Store4a(dst, src); }

  FOG_INLINE void storeRaw(uint8_t* dst, int position)
  {
    Pixel pixel;
    fetchRaw(pixel, position);
    storePix(dst, pixel);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const uint32_t* _table;
};

// ============================================================================
// [Fog::RasterOps_C - PGradientAccessor_A8_Base]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_A8_Base
{
  typedef uint8_t Pixel;
  enum { DST_BPP = 1 };

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  FOG_INLINE PGradientAccessor_A8_Base(const RasterPattern* ctx) :
    _table(reinterpret_cast<const uint8_t*>(ctx->_d.gradient.base.table) + PIXEL_ARGB32_POS_A) {}

  FOG_INLINE void fetchRaw(Pixel& dst, int position) { dst = _table[position * 4]; }
  FOG_INLINE void storePix(uint8_t* dst, const Pixel& src) { Acc::p32Store1b(dst, src); }
#else
  FOG_INLINE PGradientAccessor_A8_Base(const RasterPattern* ctx) :
    _table(reinterpret_cast<const uint32_t*>(ctx->_d.gradient.base.table)) {}

  FOG_INLINE void fetchRaw(Pixel& dst, int position) { dst = (uint8_t)(_table[position] >> 24); }
  FOG_INLINE void storePix(uint8_t* dst, const Pixel& src) { Acc::p32Store1b(dst, src); }
#endif // FOG_ARCH_...

  FOG_INLINE void storeRaw(uint8_t* dst, int position)
  {
    Pixel pixel;
    fetchRaw(pixel, position);
    storePix(dst, pixel);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  const uint8_t* _table;
#else
  const uint32_t* _table;
#endif // FOG_ARCH_...
};

// ============================================================================
// [Fog::RasterOps_C - PGradientAccessor_PRGB32_Pad]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_PRGB32_Pad : public PGradientAccessor_PRGB32_Base
{
  FOG_INLINE PGradientAccessor_PRGB32_Pad(const RasterPattern* ctx) :
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
    fetchRaw(dst, (int)d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int _len;
  double _len_d;
};

// ============================================================================
// [Fog::RasterOps_C - PGradientAccessor_A8_Pad]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_A8_Pad : public PGradientAccessor_A8_Base
{
  FOG_INLINE PGradientAccessor_A8_Pad(const RasterPattern* ctx) :
    PGradientAccessor_A8_Base(ctx),
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
    fetchRaw(dst, (int)d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int _len;
  double _len_d;
};

// ============================================================================
// [Fog::RasterOps_C - PGradientAccessor_PRGB32_Repeat]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_PRGB32_Repeat : public PGradientAccessor_PRGB32_Base
{
  FOG_INLINE PGradientAccessor_PRGB32_Repeat(const RasterPattern* ctx) :
    PGradientAccessor_PRGB32_Base(ctx),
    _lenMask(ctx->_d.gradient.base.len - 1)
  {
  }

  FOG_INLINE void fetchAtD(Pixel& dst, double d)
  {
    fetchRaw(dst, (int)d & _lenMask);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint _lenMask;
};

// ============================================================================
// [Fog::RasterOps_C - PGradientAccessor_A8_Repeat]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_A8_Repeat : public PGradientAccessor_A8_Base
{
  FOG_INLINE PGradientAccessor_A8_Repeat(const RasterPattern* ctx) :
    PGradientAccessor_A8_Base(ctx),
    _lenMask(ctx->_d.gradient.base.len - 1)
  {
  }

  FOG_INLINE void fetchAtD(Pixel& dst, double d)
  {
    fetchRaw(dst, (int)d & _lenMask);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint _lenMask;
};

// ============================================================================
// [Fog::RasterOps_C - PGradientAccessor_PRGB32_Reflect]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_PRGB32_Reflect : public PGradientAccessor_PRGB32_Base
{
  FOG_INLINE PGradientAccessor_PRGB32_Reflect(const RasterPattern* ctx) :
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
    fetchRaw(dst, i);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int _len;
  uint _lenMask2;
};

// ============================================================================
// [Fog::RasterOps_C - PGradientAccessor_A8_Reflect]
// ============================================================================

struct FOG_NO_EXPORT PGradientAccessor_A8_Reflect : public PGradientAccessor_A8_Base
{
  FOG_INLINE PGradientAccessor_A8_Reflect(const RasterPattern* ctx) :
    PGradientAccessor_A8_Base(ctx),
    _len(ctx->_d.gradient.base.len),
    _lenMask2(ctx->_d.gradient.base.len * 2 - 1)
  {
  }

  FOG_INLINE void fetchAtD(Pixel& dst, double d)
  {
    uint i = (int)d;

    i &= _lenMask2;
    if (i > (uint)_len) i ^= _lenMask2;
    fetchRaw(dst, i);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int _len;
  uint _lenMask2;
};

} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_GRADIENTBASE_P_H
