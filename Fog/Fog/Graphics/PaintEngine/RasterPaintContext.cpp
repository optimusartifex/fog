// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/PaintEngine/RasterPaintContext_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintContext]
// ============================================================================

// 4096 - Page-size.
// 80   - Some memory for system memory allocator.
RasterPaintContext::RasterPaintContext() :
  scanlineSolid8(sizeof(Span8)),
  scanlineExt8(sizeof(SpanExt8)),
  maskSpanAllocator(4096 * 2 - 80)
{
  engine = NULL;

  offset = 0;
  delta = 1;

  ops.op = OPERATOR_SRC_OVER;
  ops.sourceType = PAINTER_SOURCE_ARGB;
  ops.sourceFormat = IMAGE_FORMAT_XRGB32;
  ops.alpha255 = 0xFF;

  solid.argb = 0xFF000000;
  solid.prgb = 0xFF000000;

  pctx = NULL;
  funcs = NULL;

  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

  finalClipBox.clear();
  finalClipType = RASTER_CLIP_NULL;

  finalOrigin.clear();

  mask = NULL;
  clearSpanPools();
}

RasterPaintContext::~RasterPaintContext()
{
}

RasterClipSnapshot* RasterPaintContext::makeSnapshot()
{
#if defined(FOG_DEBUG_RASTER_MASK)
  fog_debug("Fog::Painter[Worker #%d]::makeSnapshot() - mask=%p y=[%d %d]",
    offset,
    mask,
    maskY1,
    maskY2);
#endif // FOG_DEBUG_RASTER_MASK

  ZoneMemoryAllocator::Record* record = maskSpanAllocator.record();

  RasterClipSnapshot* snapshot = reinterpret_cast<RasterClipSnapshot*>(
    maskSpanAllocator.alloc(sizeof(RasterClipSnapshot)));

  if (FOG_UNLIKELY(snapshot == NULL))
  {
    if (record) maskSpanAllocator.revert(record, false);
    return NULL;
  }

  FOG_ASSERT(record != NULL);

  snapshot->prev = maskSnapshot;
  snapshot->mask = mask;

  snapshot->maskY1 = maskY1;
  snapshot->maskY2 = maskY2;

  snapshot->maskSpanAdj = maskSpanAdj;
  snapshot->maskSpanRecord = record;

  snapshot->maskCSpanPool = maskCSpanPool;
  memcpy(snapshot->maskVSpanPool, maskVSpanPool,
    RASTER_CLIP_VSPAN_POOL_COUNT * sizeof(RasterClipVSpan8*));

  return snapshot;
}

RasterClipSnapshot* RasterPaintContext::findSnapshot(RasterClipMask* forMask)
{
  RasterClipSnapshot* current = maskSnapshot;

  while (current)
  {
    if (current->mask == forMask) return current;
    current = current->prev;
  }

  return NULL;
}

void RasterPaintContext::restoreSnapshot(RasterClipSnapshot* snapshot)
{
#if defined(FOG_DEBUG_RASTER_MASK)
  if (snapshot)
  {
    fog_debug("Fog::Painter[Worker #%d]::restoreSnapshot() - mask=%p y=[%d %d]",
      offset,
      snapshot->mask,
      snapshot->maskY1,
      snapshot->maskY2);
  }
  else
  {
    fog_debug("Fog::Painter[Worker #%d]::restoreSnapshot() - NULL");
  }
#endif // FOG_DEBUG_RASTER_MASK

  if (snapshot)
  {
    maskY1 = snapshot->maskY1;
    maskY2 = snapshot->maskY2;

    maskSpanAdj = snapshot->maskSpanAdj;

    maskCSpanPool = snapshot->maskCSpanPool;
    memcpy(maskVSpanPool, snapshot->maskVSpanPool,
      RASTER_CLIP_VSPAN_POOL_COUNT * sizeof(RasterClipVSpan8*));
  }
  else
  {
    maskY1 = -1;
    maskY2 = -1;

    maskSpanAdj = NULL;

    maskCSpanPool = NULL;
    memset(maskVSpanPool, 0,
      RASTER_CLIP_VSPAN_POOL_COUNT * sizeof(RasterClipVSpan8*));
  }
}

// ============================================================================
// [Fog::RasterPaintMasterContext]
// ============================================================================

RasterPaintMasterContext::RasterPaintMasterContext()
{
  state = 0;

  layerId = 0;

  maskId = 0;
  maskSavedCounter = 0;

  hints.data = 0;
  alpha = 1.0f;

  workClipType = RASTER_CLIP_NULL;

  metaOrigin.clear();
  userOrigin.clear();

  workClipBox.clear();
  finalTranslate.clear();

  approximationScale = 1.0;
}

RasterPaintMasterContext::~RasterPaintMasterContext()
{
}

} // Fog namespace
