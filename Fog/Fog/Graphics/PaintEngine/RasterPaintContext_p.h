// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTCONTEXT_P_H
#define _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTCONTEXT_P_H

// [Dependencies]
#include <Fog/Core/MemoryBuffer.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Graphics/Pattern.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/Region.h>
#include <Fog/Graphics/Scanline_p.h>
#include <Fog/Graphics/Transform.h>

#include <Fog/Graphics/PaintEngine/RasterPaintBase_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct RasterPaintEngine;

// ============================================================================
// [Fog::RasterPaintContext]
// ============================================================================

//! @internal
struct FOG_HIDDEN RasterPaintContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterPaintContext();
  ~RasterPaintContext();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isSingleThreaded() { return delta == 1; }
  FOG_INLINE void copyFromMaster(const RasterPaintContext& master);

  // --------------------------------------------------------------------------
  // [Clip-Mask - Snapshots]
  // --------------------------------------------------------------------------

  RasterClipSnapshot* makeSnapshot();
  RasterClipSnapshot* findSnapshot(RasterClipMask* forMask);
  void restoreSnapshot(RasterClipSnapshot* snapshot);

  // --------------------------------------------------------------------------
  // [Clip-Mask - Span Management]
  // --------------------------------------------------------------------------

  FOG_INLINE void clearSpanPools()
  {
    maskCSpanPool = NULL;
    memset(maskVSpanPool, 0,
      RASTER_CLIP_VSPAN_POOL_COUNT * sizeof(RasterClipVSpan8*));
  }

  FOG_INLINE Span8* getClipSpan(int y)
  {
    FOG_ASSERT(y >= maskY1 && y < maskY2);
    FOG_ASSERT(delta == 1 || (y % delta) == offset);
    return maskSpanAdj[y];
  }

  FOG_INLINE void setClipSpan(int y, Span8* span)
  {
    FOG_ASSERT(y >= maskY1 && y < maskY2);
    FOG_ASSERT(delta == 1 || (y % delta) == offset);
    maskSpanAdj[y] = span;
  }

  // Alloc / Free CSpan.

  FOG_INLINE Span8* allocCSpan()
  {
    Span8* span = maskCSpanPool;

    if (span)
    {
      maskCSpanPool = span->getNext();
      return span;
    }
    else
    {
      return reinterpret_cast<Span8*>(
        maskSpanAllocator.alloc(sizeof(Span8)));
    }
  }

  FOG_INLINE void freeCSpan(Span8* span)
  {
    FOG_ASSERT(span != NULL);

    span->setNext(maskCSpanPool);
    maskCSpanPool = span;
  }

  // Alloc / Free VSpan.

  FOG_INLINE Span8* allocVSpan(int len)
  {
    uint maxlen = (uint)(len + RASTER_CLIP_VSPAN_POOL_GRANULARITY_BASE    ) &
                             ~(RASTER_CLIP_VSPAN_POOL_GRANULARITY_BASE - 1) ;
    uint poolId = (maxlen >> RASTER_CLIP_VSPAN_POOL_GRANULARITY_SHIFT) - 1;

    FOG_ASSERT(len > 0);
    FOG_ASSERT(maxlen <= RASTER_CLIP_VSPAN_MAX_SIZE);
    FOG_ASSERT(poolId < RASTER_CLIP_VSPAN_POOL_COUNT);

    RasterClipVSpan8* span = maskVSpanPool[poolId];
    if (span)
    {
      maskVSpanPool[poolId] = reinterpret_cast<RasterClipVSpan8*>(span->getNext());
      return span;
    }
    else
    {
      span = reinterpret_cast<RasterClipVSpan8*>(
        maskSpanAllocator.alloc(sizeof(RasterClipVSpan8) + maxlen));
      span->setMaskPtr(reinterpret_cast<uint8_t*>(span) + sizeof(RasterClipVSpan8));
      span->len = maxlen;
      span->poolId = poolId;
      return span;
    }
  }

  FOG_INLINE void freeVSpan(Span8* span)
  {
    FOG_ASSERT(span != NULL);

    uint poolId = reinterpret_cast<RasterClipVSpan8*>(span)->poolId;
    FOG_ASSERT(poolId < RASTER_CLIP_VSPAN_POOL_COUNT);

    span->setNext(maskVSpanPool[poolId]);
    maskVSpanPool[poolId] = reinterpret_cast<RasterClipVSpan8*>(span);
  }

  // Unknown Span.

  FOG_INLINE void freeUnknownSpan(Span8* span)
  {
    FOG_ASSERT(span != NULL);

    if (span->getMaskPtr() == reinterpret_cast<uint8_t*>(span) + sizeof(RasterClipVSpan8))
      freeVSpan(span);
    else
      freeCSpan(span);
  }

  FOG_INLINE void freeChainedSpans(Span8* span)
  {
    FOG_ASSERT(span != NULL);

    do {
      Span8* next = span->getNext();
      freeUnknownSpan(span);
      span = next;
    } while (span);
  }

  // --------------------------------------------------------------------------
  // [Members - Engine]
  // --------------------------------------------------------------------------

  //! @brief Owner of this context.
  RasterPaintEngine* engine;

  // --------------------------------------------------------------------------
  // [Members - MultiThreading]
  // --------------------------------------------------------------------------

  //! @brief Context offset (multithreading).
  //!
  //! @note If multithreading is disabled, offset is 0.
  int offset;

  //! @brief Context delta (multithreading).
  //!
  //! @note If multithreading is disabled, delta is 1.
  int delta;

  // --------------------------------------------------------------------------
  // [Members - Painting]
  // --------------------------------------------------------------------------

  //! @brief Current context ops (serialized by the paint command).
  RasterPaintOps ops;

  //! @brief Current paint layer.
  RasterPaintLayer paintLayer;

  // --------------------------------------------------------------------------
  // [Members - Clipping / Origin]
  // --------------------------------------------------------------------------

  //! @brief Type of work clip area (workRegion, workBox). See also
  //! @c workRegion member.
  //!
  //! @note Don't miss it with the @c finalClipType member!
  uint32_t workClipType;

  //! @brief Type of clip area, see @c RASTER_CLIP_TYPE.
  uint32_t finalClipType;

  //! @brief Work clip box.
  BoxI workClipBox;

  //! @brief Final clip box (or final region extents if final region is complex).
  BoxI finalClipBox;

  //! @brief Private work region that contains only metaRegion & userRegion,
  //! it not contains clip region (if clip region not exists then this is the
  //! same as @c RasterPaintContext::workRegion member). See @c finalRegion
  //! that contains workRegion & clipRegion.
  Region workRegion;

  //! @brief Meta region intersected with user region translated by meta origin.
  Region finalRegion;

  //! @brief Meta origin translated by used origin.
  PointI finalOrigin;

  //! @brief Current clip mask.
  RasterClipMask* mask;
  //! @brief Current clip mask snapshot (used to restore allocators / pools).
  RasterClipSnapshot* maskSnapshot;

  //! @brief Clip mask y1 (min-y), per context.
  int maskY1;

  //! @brief Clip mask y2 (max-y), per context.
  int maskY2;

  //! @brief Spans data adjusted by -workClipBox.y1. This pointer points to
  //! INVALID location, but this location is NEVER accessed. Caller must ensure
  //! that data are accessed at [workBox.y1 -> workBox.y2] index.
  //!
  //! To access mask spans use always @c getClipSpan() / setClipSpan() methods.
  //!
  //! Length of array is the same as @c mask->spans.
  Span8** maskSpanAdj;

  // --------------------------------------------------------------------------
  // [Members - Parameters / Contexts]
  // --------------------------------------------------------------------------

  //! @brief RasterSolid source color (applicable if source type is @c PAINTER_SOURCE_ARGB.
  RasterSolid solid;

  //! @brief Pattern source context.
  RasterPattern* pctx;

  //! @brief Pointer to compositing functions, see @c ops.op.
  const RasterFuncs::CompositeFuncs* funcs;

  //! @brief Raster engine closure (used together with blitter from @c funcs).
  RasterClosure closure;

  // --------------------------------------------------------------------------
  // [Members - Helper objects / Pools]
  // --------------------------------------------------------------------------

  //! @brief The Scanline8 (for solid fills) instance owned by the context, for 
  //! current per thread work.
  Scanline8 scanlineSolid8;

  //! @brief The Scanline8 (for pattern filles) instance owned by the context, 
  //! for current per thread work.
  Scanline8 scanlineExt8;

  //! @brief The clip-mask span allocator owned by the context.
  ZoneMemoryAllocator maskSpanAllocator;

  //! @brief Unused clip CSpan instances (pooled) for current clip state.
  Span8* maskCSpanPool;

  //! @brief Unused clip VSpan instances (pooled) for current clip state.
  RasterClipVSpan8* maskVSpanPool[RASTER_CLIP_VSPAN_POOL_COUNT];

  // --------------------------------------------------------------------------
  // [Members - Buffer]
  // --------------------------------------------------------------------------

  //! @brief Reusable/temporary memory buffer.
  MemoryBuffer buffer;

  // --------------------------------------------------------------------------
  // [Temporary Objects]
  // --------------------------------------------------------------------------

  //! @brief Temporary path per context, used by calculations.
  PathD tmpCalcPath;

private:
  FOG_DISABLE_COPY(RasterPaintContext)
};

// ============================================================================
// [Fog::RasterPaintMasterContext]
// ============================================================================

//! @internal
//!
//! @brief Main state structure where is stored the current state.
struct FOG_HIDDEN RasterPaintMasterContext : public RasterPaintContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterPaintMasterContext();
  ~RasterPaintMasterContext();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief State, no-paint flags, schedule flags and fatal error flag.
  uint32_t state;

  //! @brief Paint layer id (starts at zero).
  //!
  //! If layerId is zero then there is only master layer.
  uint32_t layerId;

  //! @brief Current (or last) clip mask id. Each new mask get this id increased
  //! by one.
  //!
  //! If clipMaskId is zero then there is no active mask.
  //!
  //! @note Clip mask id is increased / decreased mainly by state management,
  //! because if state (and mask in it) is saved then new mask is needed. This
  //! new mask can be based on the old mask, but it's mutable. IDs are managed
  //! only by master context so if multithreading is used then some threads
  //! can use same mask id on different mask instance.
  uint32_t maskId;
  //! @brief Contains how many the state was saved for current mask, but mask
  //! is not changed after that.
  uint32_t maskSavedCounter;

  //! @brief Context paint hints.
  RasterPaintHints hints;

  //! @brief Paint alpha.
  float alpha;

  //! @brief Pattern source data (applicable if source type is @c PAINTER_SOURCE_PATTERN).
  Static<Pattern> pattern;

  //! @brief The meta origin.
  PointI metaOrigin;
  //! @brief The meta region.
  Region metaRegion;

  //! @brief User origin.
  PointI userOrigin;
  //! @brief User region.
  Region userRegion;

  //! @brief Meta matrix.
  TransformD metaTransform;

  //! @brief User transformation matrix.
  TransformD userTransform;

  //! @brief Work transformation matrix (the matrix used to transform
  //! coordinates from user space to raster).
  TransformD finalTransform;

  //! @brief Transformation translate point in pixels (can be used if
  //! transform type is @c RASTER_TRANSFORM_EXACT).
  PointI finalTranslate;

  //! @brief Transformation approximation scale used by path flattening
  //! and stroking.
  double approximationScale;

  //! @brief Stroke parameters.
  PathStrokeParams strokeParams;

private:
  FOG_DISABLE_COPY(RasterPaintMasterContext)
};

// ============================================================================
// [Inline]
// ============================================================================

FOG_INLINE void RasterPaintContext::copyFromMaster(const RasterPaintContext& master)
{
  paintLayer = master.paintLayer;
  ops.data = master.ops.data;

  workClipType = master.workClipType;
  finalClipType = master.finalClipType;

  workClipBox = master.workClipBox;
  finalClipBox = master.finalClipBox;

  workRegion = master.workRegion;
  finalRegion = master.finalRegion;

  finalOrigin = master.finalOrigin;

  // If mask was created we can't copy the data from master context into thread's
  // own. The setEngine() method must check for this condition before and return
  // @c ERR_PAINTER_NOT_ALLOWED error.
  FOG_ASSERT(mask == NULL && master.mask == NULL);

  solid = master.solid;
  pctx = NULL;

  funcs = master.funcs;
  closure = master.closure;
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTCONTEXT_P_H
