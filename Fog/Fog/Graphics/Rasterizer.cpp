// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Graphics/ByteUtil_p.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/Rasterizer_p.h>
#include <Fog/Graphics/Rasterizer/Rasterizer_Analytic_p.h>

namespace Fog {

// ============================================================================
// [Fog::Rasterizer - Local]
// ============================================================================

struct FOG_HIDDEN RasterizerLocal
{
  RasterizerLocal() : 
    rasterizers(NULL),
    cellBuffers(NULL),
    cellsBufferCapacity(2048)
  {
  }

  ~RasterizerLocal()
  {
  }

  Lock lock;

  Rasterizer* rasterizers;
  Rasterizer::CellXYBuffer* cellBuffers;

  uint32_t cellsBufferCapacity;
};

static Static<RasterizerLocal> rasterizer_local;

// ============================================================================
// [Fog::Rasterizer]
// ============================================================================

Rasterizer::Rasterizer()
{
  // Rasterizer pool.
  _pool = NULL;

  // Clip-box / bounding-box.
  _clipBox.clear();
  _boundingBox.clear();

  // Default fill rule.
  _fillRule = FILL_DEFAULT;

  // Default alpha is full-opaque.
  _alpha = 0xFF;

  // No error at this time.
  _error = ERR_OK;

  // Not finalized, not valid.
  _isFinalized = false;
  _isValid = false;

  // Sweep-scanline is initialized during initialize() or finalize().
  _sweepScanlineSimpleFn = NULL;
  _sweepScanlineRegionFn = NULL;
  _sweepScanlineSpansFn = NULL;
}

Rasterizer::~Rasterizer()
{
}

// ============================================================================
// [Fog::Rasterizer - Pooling]
// ============================================================================

Rasterizer* Rasterizer::getRasterizer()
{
  AutoLock locked(rasterizer_local->lock);
  Rasterizer* rasterizer;

  if (rasterizer_local->rasterizers)
  {
    rasterizer = rasterizer_local->rasterizers;
    rasterizer_local->rasterizers = rasterizer->_pool;
    rasterizer->_pool = NULL;
  }
  else
  {
    rasterizer = fog_new AnalyticRasterizer();
  }

  return rasterizer;
}

void Rasterizer::releaseRasterizer(Rasterizer* rasterizer)
{
  AutoLock locked(rasterizer_local->lock);

  rasterizer->_pool = rasterizer_local->rasterizers;
  rasterizer_local->rasterizers = rasterizer;
}

Rasterizer::CellXYBuffer* Rasterizer::getCellXYBuffer()
{
  CellXYBuffer* cellBuffer = NULL;

  {
    AutoLock locked(rasterizer_local->lock);
    if (rasterizer_local->cellBuffers)
    {
      cellBuffer = rasterizer_local->cellBuffers;
      rasterizer_local->cellBuffers = cellBuffer->next;
      cellBuffer->next = NULL;
      cellBuffer->prev = NULL;
      cellBuffer->count = 0;
    }
  }

  if (cellBuffer == NULL)
  {
    cellBuffer = (CellXYBuffer*)Memory::alloc(
      sizeof(CellXYBuffer) - sizeof(CellXY) + sizeof(CellXY) * rasterizer_local->cellsBufferCapacity);
    if (cellBuffer == NULL) return NULL;

    cellBuffer->next = NULL;
    cellBuffer->prev = NULL;
    cellBuffer->count = 0;
    cellBuffer->capacity = rasterizer_local->cellsBufferCapacity;
  }

  return cellBuffer;
}

void Rasterizer::releaseCellXYBuffer(CellXYBuffer* cellBuffer)
{
  AutoLock locked(rasterizer_local->lock);

  // Get last.
  CellXYBuffer* last = cellBuffer;
  while (last->next) last = last->next;

  last->next = rasterizer_local->cellBuffers;
  rasterizer_local->cellBuffers = cellBuffer;
}

void Rasterizer::cleanup()
{
  // Free all rasterizers.
  Rasterizer* rasterizerCurr;
  Rasterizer* rasterizerNext;

  {
    AutoLock locked(rasterizer_local->lock);

    rasterizerCurr = rasterizer_local->rasterizers;
    rasterizer_local->rasterizers = NULL;
  }

  while (rasterizerCurr)
  {
    rasterizerNext = rasterizerCurr->_pool;
    fog_delete(rasterizerCurr);
    rasterizerCurr = rasterizerNext;
  }

  // Free all cell buffers.
  CellXYBuffer* bufferCurr;
  CellXYBuffer* bufferNext;

  {
    AutoLock locked(rasterizer_local->lock);

    bufferCurr = rasterizer_local->cellBuffers;
    rasterizer_local->cellBuffers = NULL;
  }

  while (bufferCurr)
  {
    bufferNext = bufferCurr->next;
    Memory::free(bufferCurr);
    bufferCurr = bufferNext;
  }
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_rasterizer_init(void)
{
  using namespace Fog;

  rasterizer_local.init();
  return ERR_OK;
}

FOG_INIT_DECLARE void fog_rasterizer_shutdown(void)
{
  using namespace Fog;

  Rasterizer::cleanup();
  rasterizer_local.destroy();
}
