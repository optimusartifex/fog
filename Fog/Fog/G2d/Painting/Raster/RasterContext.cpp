// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Debug.h>
#include <Fog/G2d/Painting/Raster/RasterContext_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterContext - Construction / Destruction]
// ============================================================================

RasterContext::RasterContext() :
  engine(NULL),
  renderer(NULL),
  offset(0),
  delta(1),
  precision(0xFFFFFFFF),
  finalClipType(RASTER_CLIP_NULL),
  finalClipBoxI(0, 0, 0, 0),
  finalClipperF(BoxF(0, 0, 0, 0)),
  finalClipperD(BoxD(0, 0, 0, 0)),
  // 4096 - Page-size.
  // 100  - Some memory for the OS/LibC memory allocator.
  maskMemoryAllocator(4096 * 2 - 100),
  mask(NULL),
  maskSnapshot(NULL),
  maskY1(-1),
  maskY2(-1),
  maskRowsAdj(NULL)
{
  layer.reset();
  maskResetPools();

  paintHints.packed = 0;
  rasterHints.packed = 0;

  solid.prgb32.u32 = 0xFF000000;
  solid.prgb64.u64 = FOG_UINT64_C(0xFFFF000000000000);
  pc = (RenderPatternContext*)(size_t)0x1;

  closure.ditherOrigin.reset();
  closure.palette = NULL;
  closure.data = NULL;
}

RasterContext::~RasterContext()
{
  _initPrecision(0xFFFFFFFF);
}

// ============================================================================
// [Fog::RasterContext - Init / Reset]
// ============================================================================

err_t RasterContext::_initByMaster(const RasterContext& master)
{
  // If mask was created we can't copy the data from master context into thread's
  // own. The setEngine() method must check for this condition before and return
  // @c ERR_PAINTER_NOT_ALLOWED error.
  FOG_ASSERT(mask == NULL && master.mask == NULL);

  layer = master.layer;

  finalClipType = master.finalClipType;
  finalClipBoxI = master.finalClipBoxI;
  finalClipperF.setClipBox(master.finalClipperF.getClipBox());
  finalClipperD.setClipBox(master.finalClipperD.getClipBox());
  finalRegion = master.finalRegion;

  paintHints = master.paintHints;
  rasterHints = master.rasterHints;

  closure = master.closure;
  solid.prgb64 = master.solid.prgb64;
  solid.prgb32 = master.solid.prgb32;
  pc = (RenderPatternContext*)(size_t)0x1;

  return _initPrecision(master.precision);
}

err_t RasterContext::_initPrecision(uint32_t precision)
{
  if (this->precision != precision)
  {
    // Destroy resources using an old precision.
    switch (this->precision)
    {
      case IMAGE_PRECISION_BYTE:
        rasterizer8.destroy();
        scanline8.destroy();
        scanlineExt8.destroy();
        break;

      case IMAGE_PRECISION_WORD:
        // TODO: 16-bit rasterizer.
        // rasterizer16.destroy();
        // scanline16.destroy();
        // scanlineExt16.destroy();
        break;
    }

    // Create resources using the new precision.
    this->precision = precision;

    switch (this->precision)
    {
      case IMAGE_PRECISION_BYTE:
        fullOpacityValueU = 0x100;
        fullOpacityValueF = (float)(0x100 << 4);
        rasterizer8.init();
        scanline8.init();
        scanlineExt8.initCustom1((uint32_t)sizeof(SpanExt8));
        break;

      case IMAGE_PRECISION_WORD:
        fullOpacityValueU = 0x10000;
        fullOpacityValueF = (float)(0x10000 << 4);
        // TODO: 16-bit rasterizer.
        // rasterizer16.init();
        // scanline16.init();
        // scanlineExt16.init((uint32_t)sizeof(SpanExt16));
        break;
    }
  }

  uint32_t pcBpl = layer.size.w;

  switch (this->precision)
  {
    case IMAGE_PRECISION_BYTE:
      pcBpl *= 4;
      FOG_RETURN_ON_ERROR(scanline8->prepare(layer.size.w));
      FOG_RETURN_ON_ERROR(scanlineExt8->prepare(layer.size.w));
      break;

    case IMAGE_PRECISION_WORD:
      pcBpl *= 8;
      // TODO: 16-bit rasterizer.
      // FOG_RETURN_ON_ERROR(scanline16->prepare(layer.size.w));
      // FOG_RETURN_ON_ERROR(scanlineExt16->prepare(layer.size.w));
      break;

    default:
      pcBpl = 0;
      break;
  }

  uint8_t* pcBuf = pcRowBuffer.alloc(pcBpl);
  return pcBuf != NULL ? (err_t)ERR_OK : (err_t)ERR_RT_OUT_OF_MEMORY;
}

void RasterContext::_reset()
{
  // TODO: Not used.
}

// ============================================================================
// [Fog::RasterContext - Mask - Snapshots]
// ============================================================================

RasterMaskSnapshot* RasterContext::makeSnapshot()
{
#if defined(FOG_DEBUG_RASTER_MASK)
  Debug::dbgFormat("Fog::RasterContext[#%d]::makeSnapshot() - Mask=%p y=[%d %d]\n",
    offset,
    mask,
    maskY1,
    maskY2);
#endif // FOG_DEBUG_RASTER_MASK

  ZoneMemoryAllocator::Record* record = maskMemoryAllocator.record();

  RasterMaskSnapshot* snapshot = reinterpret_cast<RasterMaskSnapshot*>(
    maskMemoryAllocator.alloc(sizeof(RasterMaskSnapshot)));

  if (FOG_UNLIKELY(snapshot == NULL))
  {
    if (record) maskMemoryAllocator.revert(record, false);
    return NULL;
  }

  FOG_ASSERT(record != NULL);

  snapshot->prev = maskSnapshot;
  snapshot->mask = mask;

  snapshot->y1 = maskY1;
  snapshot->y2 = maskY2;

  snapshot->rowsAdj = maskRowsAdj;
  snapshot->record = record;

  snapshot->cSpanPool = maskCSpanPool;
  memcpy(snapshot->vSpanPool, maskVSpanPool, RASTER_MASK_VSPAN_POOL_COUNT * sizeof(void*));

  return snapshot;
}

RasterMaskSnapshot* RasterContext::findSnapshot(RasterMask* forMask)
{
  RasterMaskSnapshot* current = maskSnapshot;

  while (current)
  {
    if (current->mask == forMask) return current;
    current = current->prev;
  }

  return NULL;
}

void RasterContext::restoreSnapshot(RasterMaskSnapshot* snapshot)
{
#if defined(FOG_DEBUG_RASTER_MASK)
  if (snapshot)
  {
    Debug::dbgFormat("Fog::RasterContext[#%d]::restoreSnapshot() - Mask=%p y=[%d %d]\n",
      offset,
      snapshot->mask,
      snapshot->y1,
      snapshot->y2);
  }
  else
  {
    Debug::dbgFormat("Fog::RasterContext[#%d]::restoreSnapshot() - None\n");
  }
#endif // FOG_DEBUG_RASTER_MASK

  if (snapshot)
  {
    maskY1 = snapshot->y1;
    maskY2 = snapshot->y2;

    maskRowsAdj = snapshot->rowsAdj;
    maskCSpanPool = snapshot->cSpanPool;
    memcpy(maskVSpanPool, snapshot->vSpanPool, RASTER_MASK_VSPAN_POOL_COUNT * sizeof(void*));
  }
  else
  {
    maskY1 = -1;
    maskY2 = -1;

    maskRowsAdj = NULL;
    maskCSpanPool = NULL;
    memset(maskVSpanPool, 0, RASTER_MASK_VSPAN_POOL_COUNT * sizeof(void*));
  }
}

} // Fog namespace
