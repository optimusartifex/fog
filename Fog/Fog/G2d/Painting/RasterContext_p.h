// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERCONTEXT_P_H
#define _FOG_G2D_PAINTING_RASTERCONTEXT_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemoryBuffer.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Painting/PaintParams.h>
#include <Fog/G2d/Painting/RasterMask_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Render/RenderStructs_p.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterContext]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT RasterContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterContext();
  ~RasterContext();

  // --------------------------------------------------------------------------
  // [Init / Reset]
  // --------------------------------------------------------------------------

  err_t _initByMaster(const RasterContext& master);
  err_t _initPrecision(uint32_t precision);

  void _reset();

  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isSingleThreaded() const { return scope.isSingleThreaded(); }
  FOG_INLINE bool isMultiThreaded() const { return scope.isMultiThreaded(); }

  // --------------------------------------------------------------------------
  // [Mask - Pools]
  // --------------------------------------------------------------------------

  FOG_INLINE void maskResetPools()
  {
    maskCSpanPool = NULL;
    for (uint i = 0; i < RASTER_MASK_VSPAN_POOL_COUNT; i++) maskVSpanPool[i] = NULL;
  }

  // --------------------------------------------------------------------------
  // [Mask - Rows]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterSpan* _getMaskRow(int y) const
  {
    FOG_ASSERT(y >= maskY1 && y < maskY2);
    return maskRowsAdj[y];
  }

  FOG_INLINE void _setMaskRow(int y, RasterSpan* span) const
  {
    FOG_ASSERT(y >= maskY1 && y < maskY2);
    maskRowsAdj[y] = span;
  }

  FOG_INLINE RasterSpan8* getMaskRow8(int y) const { return reinterpret_cast<RasterSpan8*>(_getMaskRow(y)); }
  FOG_INLINE RasterSpan16* getMaskRow16(int y) const { return reinterpret_cast<RasterSpan16*>(_getMaskRow(y)); }

  FOG_INLINE void setMaskRow8(int y, RasterSpan8* span) const { _setMaskRow(y, reinterpret_cast<RasterSpan8*>(span)); }
  FOG_INLINE void setMaskRow16(int y, RasterSpan16* span) const { _setMaskRow(y, reinterpret_cast<RasterSpan16*>(span)); }

  // --------------------------------------------------------------------------
  // [Mask - CSpan Management]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterSpan* _allocMaskCSpan()
  {
    RasterSpan* span = maskCSpanPool;
    if (span) { maskCSpanPool = span->getNext(); return span; }
    return reinterpret_cast<RasterSpan*>(maskMemoryAllocator.alloc(sizeof(RasterSpan)));
  }

  FOG_INLINE void _freeMaskCSpan(RasterSpan* span)
  {
    FOG_ASSERT(span != NULL);
    span->setNext(maskCSpanPool);
    maskCSpanPool = span;
  }

  FOG_INLINE RasterSpan8* allocMaskCSpan8() { return reinterpret_cast<RasterSpan8*>(_allocMaskCSpan()); }
  FOG_INLINE RasterSpan16* allocMaskCSpan16() { return reinterpret_cast<RasterSpan16*>(_allocMaskCSpan()); }

  FOG_INLINE void freeMaskCSpan8(RasterSpan8* span) { _freeMaskCSpan(span); }
  FOG_INLINE void freeMaskCSpan16(RasterSpan16* span) { _freeMaskCSpan(span); }

  // --------------------------------------------------------------------------
  // [Mask - VSpan Management]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterSpan* _allocMaskVSpan(int len)
  {
    uint maxLen = (uint)(len + RASTER_MASK_VSPAN_POOL_GRANULARITY_BASE    ) &
                             ~(RASTER_MASK_VSPAN_POOL_GRANULARITY_BASE - 1) ;
    uint poolId = (maxLen >> RASTER_MASK_VSPAN_POOL_GRANULARITY_SHIFT) - 1;

    FOG_ASSERT(len > 0);
    FOG_ASSERT(maxLen <= RASTER_MASK_VSPAN_MAX_LENGTH_8);
    FOG_ASSERT(poolId < RASTER_MASK_VSPAN_POOL_COUNT);

    RasterMaskSpan8* span = reinterpret_cast<RasterMaskSpan8*>(maskVSpanPool[poolId]);
    if (span)
    {
      maskVSpanPool[poolId] = reinterpret_cast<RasterMaskSpan8*>(span->getNext());
      return span;
    }
    else
    {
      span = reinterpret_cast<RasterMaskSpan8*>(maskMemoryAllocator.alloc(sizeof(RasterMaskSpan8) + maxLen));
      span->setGenericMask(reinterpret_cast<uint8_t*>(span) + sizeof(RasterMaskSpan8));
      span->maxLen = maxLen;
      span->poolId = poolId;
      return span;
    }
  }

  FOG_INLINE void _freeMaskVSpan(RasterSpan* span)
  {
    FOG_ASSERT(span != NULL);

    uint poolId = reinterpret_cast<RasterMaskSpan8*>(span)->poolId;
    FOG_ASSERT(poolId < RASTER_MASK_VSPAN_POOL_COUNT);

    span->setNext(maskVSpanPool[poolId]);
    maskVSpanPool[poolId] = span;
  }

  FOG_INLINE RasterSpan8* allocVSpan8_8(int len) { return reinterpret_cast<RasterSpan8*>(_allocMaskVSpan(len)); }
  FOG_INLINE RasterSpan8* allocVSpan8_16(int len) { return reinterpret_cast<RasterSpan8*>(_allocMaskVSpan(len * 2)); }
  FOG_INLINE void freeVSpan8(RasterSpan8* span) { _freeMaskVSpan(span); }

  FOG_INLINE RasterSpan16* allocVSpan16_16(int len) { return reinterpret_cast<RasterSpan16*>(_allocMaskVSpan(len * 2)); }
  FOG_INLINE RasterSpan16* allocVSpan16_32(int len) { return reinterpret_cast<RasterSpan16*>(_allocMaskVSpan(len * 4)); }
  FOG_INLINE void freeVSpan16(RasterSpan16* span) { _freeMaskVSpan(span); }

  // --------------------------------------------------------------------------
  // [Mask - XSpan Management]
  // --------------------------------------------------------------------------

  FOG_INLINE void freeMaskSpan(RasterSpan* span)
  {
    FOG_ASSERT(span != NULL);

    if (span->getGenericMask() == reinterpret_cast<uint8_t*>(span) + sizeof(RasterMaskSpan8))
      _freeMaskVSpan(span);
    else
      _freeMaskCSpan(span);
  }

  FOG_INLINE void freeMaskChain(RasterSpan* span)
  {
    FOG_ASSERT(span != NULL);

    do {
      RasterSpan* next = span->getNext();
      freeMaskSpan(span);
      span = next;
    } while (span);
  }

  // --------------------------------------------------------------------------
  // [Mask - Snapshots]
  // --------------------------------------------------------------------------

  RasterMaskSnapshot* makeSnapshot();
  RasterMaskSnapshot* findSnapshot(RasterMask* forMask);
  void restoreSnapshot(RasterMaskSnapshot* snapshot);

  // --------------------------------------------------------------------------
  // [Members - Engine / Renderer]
  // --------------------------------------------------------------------------

  //! @brief Owner of this context.
  RasterPaintEngine* engine;

  //! @brief Context scope (used by multithreaded paint-engine)
  RasterScope scope;

  // --------------------------------------------------------------------------
  // [Members - Precision]
  // --------------------------------------------------------------------------

  //! @brief Context precision (see @c IMAGE_PRECISION).
  uint32_t precision;

  struct FOG_NO_EXPORT _FullOpacity
  {
    //! @brief Full opacity (0x100 or 0x10000). Depends on @c IMAGE_PRECISION.
    uint32_t u;
    //! @brief Full opacity for float conversion. Depends on @c IMAGE_PRECISION.
    float f;
  } fullOpacity;

  // --------------------------------------------------------------------------
  // [Members - Layer]
  // --------------------------------------------------------------------------

  //! @brief Layer.
  RasterLayer layer;

  // --------------------------------------------------------------------------
  // [Members - Render]
  // --------------------------------------------------------------------------

  //! @brief Paint hints.
  PaintHints paintHints;
  //! @brief Raster hints.
  RasterHints rasterHints;

  //! @brief Solid source.
  RenderSolid solid;
  //! @brief Pattern context.
  //!
  //! Applicable if source type is @c PATTERN_TYPE_TEXTURE or @c PATTERN_TYPE_GRADIENT.
  RenderPatternContext* pc;
  //! @brief Render closure.
  RenderClosure closure;

  union
  {
    //! @brief The box analytic rasterizer (8-bit).
    Static<BoxRasterizer8> boxRasterizer8;

    // TODO: 16-bit rasterizer.
    // //! @brief The box analytic rasterizer (16-bit).
    // Static<BoxRasterizer16> boxRasterizer16;
  };

  union
  {
    //! @brief The path/polygon analytic rasterizer (8-bit).
    Static<PathRasterizer8> pathRasterizer8;

    // TODO: 16-bit rasterizer.
    // //! @brief The analytic rasterizer (16-bit).
    // Static<PathRasterizer16> pathRasterizer16;
  };

  union
  {
    //! @brief The scanline container for solid spans (8-bit).
    Static<RasterScanline8> scanline8;

    // TODO: 16-bit rasterizer.
    // //! @brief The scanline container for solid spans (16-bit).
    // Static<RasterScanline16> scanline16;
  };

  union
  {
    //! @brief The scanline container for extended spans (8-bit).
    Static<RasterScanline8> scanlineExt8;

    // TODO: 16-bit rasterizer.
    // //! @brief  The scanline container for extended spans (16-bit).
    // Static<RasterScanline16> scanlineExt16;
  };

  // --------------------------------------------------------------------------
  // [Members - Clip]
  // --------------------------------------------------------------------------

  //! @brief Type of clipping, see @c RASTER_CLIP.
  uint32_t finalClipType;
  //! @brief Clip box (int).
  BoxI finalClipBoxI;

  //! @brief Clip box and clipper instance (float).
  PathClipperF finalClipperF;
  //! @brief Clip box and clipper instance (double).
  PathClipperD finalClipperD;

  //! @brief Clip region.
  Region finalRegion;

  // --------------------------------------------------------------------------
  // [Members - Mask]
  // --------------------------------------------------------------------------

  //! @brief The clip-mask span allocator owned by the raster-context.
  ZoneAllocator maskMemoryAllocator;

  //! @brief The clip mask.
  RasterMask* mask;
  //! @brief The clip mask snapshot (used to restore allocators / pools).
  RasterMaskSnapshot* maskSnapshot;

  //! @brief Clip mask y1 (min-y), per context.
  int maskY1;
  //! @brief Clip mask y2 (max-y), per context.
  int maskY2;

  //! @brief Spans data adjusted by -coreClipBox.y1. This pointer points to
  //! INVALID location, but this location is NEVER accessed. Caller must ensure
  //! that data are accessed at [coreClipBox.y1 -> coreClipBox.y2] index.
  //!
  //! To access mask spans use always @c getClipSpan() / setClipSpan() methods.
  //!
  //! Length of array is the same as @c mask->spans.
  RasterSpan** maskRowsAdj;

  //! @brief Unused clip 'CSpan' instances (pooled) for current clip state.
  RasterSpan* maskCSpanPool;

  //! @brief Unused clip 'VSpan' instances (pooled) for current clip state.
  RasterSpan* maskVSpanPool[RASTER_MASK_VSPAN_POOL_COUNT];

  // --------------------------------------------------------------------------
  // [Members - Temp]
  // --------------------------------------------------------------------------

  //! @brief Temporary memory buffer.
  MemoryBuffer buffer;

  //! @brief Temporary path per context, used by calculations (float).
  PathF tmpPathF[3];

  //! @brief Temporary path per context, used by calculations (double).
  PathD tmpPathD[3];

private:
  _FOG_CLASS_NO_COPY(RasterContext)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERCONTEXT_P_H
