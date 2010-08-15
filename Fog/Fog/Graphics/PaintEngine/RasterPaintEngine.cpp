// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/CpuInfo.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/MemoryAllocator_p.h>
#include <Fog/Core/OS.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Thread.h>
#include <Fog/Core/ThreadCondition.h>
#include <Fog/Core/ThreadPool.h>
#include <Fog/Face/FaceByte.h>
#include <Fog/Graphics/AnalyticRasterizer_p.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/Constants_p.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/Glyph.h>
#include <Fog/Graphics/GlyphSet.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageFilter.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Painter.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/RasterUtil_p.h>
#include <Fog/Graphics/Scanline_p.h>

#include <Fog/Graphics/PaintEngine/RasterPaintAction_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintBase_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintCalc_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintCmd_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintEngine_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintState_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintUtil_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintRender_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintWorker_p.h>

namespace Fog {

// ============================================================================
// [Documentation and Notes]
// ============================================================================

// Compositing into transparent-black leads usually to OPERATOR_SRC
// ----------------------------------------------------------------
//
// If you look deeply into the code you can see that operations in layers and
// very optimized, because common case is to create layer, use small portion of
// it and blit it back to the main layer. We never know what user wants to do
// with layer so we can't create smaller one than current clip box.
//
// So layers contains special Row records to determine how is used each row,
// to fill pixels outside of row record it's needed firstly to clear them to
// 0x00000000 (fully-transparent color) and then to composite new content to
// them. Because in most cases this operation is not needed the next table
// summarizes the operator replacements that can be used with compositing with
// these uninitialized pixels.
//
// DST(0x00000000) OP SRC summary:
//
// Operator               | ARGB-DST     | XRGB-DST     | A-DST        |
// -----------------------+--------------+--------------+--------------+-------
// OPERATOR_SRC           | OPERATOR_SRC | OPERATOR_SRC | OPERATOR_SRC |
// OPERATOR_DST           | NOP          | NOP          | NOP          |
// OPERATOR_SRC_OVER      | OPERATOR_SRC | OPERATOR_SRC | OPERATOR_SRC |
// OPERATOR_DST_OVER      | OPERATOR_SRC | NOP          | OPERATOR_SRC |
// OPERATOR_SRC_IN        | NOP          | OPERATOR_SRC | NOP          |
// OPERATOR_DST_IN        | NOP          | NOP          | NOP          |
// OPERATOR_SRC_OUT       | OPERATOR_SRC | NOP          | OPERATOR_SRC |
// OPERATOR_DST_OUT       | NOP          | NOP          | NOP          |
// OPERATOR_SRC_ATOP      | NOP          | OPERATOR_SRC | NOP          |
// OPERATOR_DST_ATOP      | OPERATOR_SRC | NOP          | OPERATOR_SRC |
// OPERATOR_XOR           | OPERATOR_SRC | NOP          | OPERATOR_SRC |
// OPERATOR_CLEAR         | NOP          | NOP          | NOP          |
// OPERATOR_ADD           | OPERATOR_SRC | OPERATOR_SRC | OPERATOR_SRC |
// OPERATOR_SUBTRACT      | SAME         | NOP          | SAME         |
// OPERATOR_MULTIPLY      | OPERATOR_SRC | NOP          | OPERATOR_SRC |
// OPERATOR_SCREEN        | OPERATOR_SRC | OPERATOR_SRC | OPERATOR_SRC |
// OPERATOR_DARKEN        | OPERATOR_SRC | OPERATOR_SRC | OPERATOR_SRC |
// OPERATOR_LIGHTEN       | OPERATOR_SRC | OPERATOR_SRC | OPERATOR_SRC |
// OPERATOR_DIFFERENCE    | OPERATOR_SRC | OPERATOR_SRC | OPERATOR_SRC |
// OPERATOR_EXCLUSION     | SAME         | OPERATOR_SRC | SAME         |
// OPERATOR_INVERT        | SAME         | SAME         | SAME         |
// OPERATOR_INVERT_RGB    | SAME         | OPERATOR_SRC | SAME         |
//
// You can see that the situation is really simple, in many cases we just
// use OPERATOR_SRC or do nothing.

// ============================================================================
// [Fog::RasterPaintEngine - Construction / Destruction]
// ============================================================================

RasterPaintEngine::RasterPaintEngine(const ImageBuffer& buffer, ImageData* imaged, uint32_t initFlags) :
  workerManager(NULL),
  finishing(false)
{
  // Setup primary context.
  ctx.engine = this;
  ctx.pctx = NULL;

  // Setup primary layer.
  ctx.paintLayer.pixels = buffer.data;
  ctx.paintLayer.width = buffer.width;
  ctx.paintLayer.height = buffer.height;
  ctx.paintLayer.format = buffer.format;
  ctx.paintLayer.stride = buffer.stride;
  _setupPaintLayer(&ctx.paintLayer);

  // Setup primary clip mask (none at this time).
  ctx.mask = NULL;
  ctx.maskSnapshot = NULL;

  // Setup clip / caps state to defaults.
  _setClipDefaults();
  _setCapsDefaults();

  // May be set by _setClipDefaults() or _setCapsDefaults(), but we are starting
  // so there is not a change actually.
  ctx.state &= ~(RASTER_STATE_PENDING_MASK);

  // Setup multithreading if possible.
  if ((initFlags & PAINTER_INIT_MT) != 0 && getCpuInfo()->numberOfProcessors > 1)
  {
    uint64_t total = (uint64_t)buffer.width * (uint64_t)buffer.height;

    if (total >= RASTER_MIN_SIZE_THRESHOLD)
    {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
      fog_debug("Fog::RasterPaintEngine::new() - Using MT for %dx%d image.", buffer.width, buffer.height);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
      setEngine(PAINT_ENGINE_RASTER_MT, 0);
    }
  }
}

RasterPaintEngine::~RasterPaintEngine()
{
  finishing = true;

  // End with painting will ensure that we can switch to single-threaded mode
  // and destroy the paint engine. It internally calls _deleteStates() and all
  // needed methods to enable such switch.
  _endWithPainting();

  // First set engine to singlethreaded (this means flush and releasing all
  // threads), then we can destroy engine.
  if (!isSingleThreaded())
  {
    setEngine(PAINT_ENGINE_RASTER_ST, 0);
  }

  // This is our context, it's imposible that other thread is using it at this
  // time!
  if (ctx.pctx)
  {
    if (ctx.pctx->initialized) ctx.pctx->destroy(ctx.pctx);

    // This is not important in release mode. In debug mode it's needed to
    // free it because of assertion in the Fog::BlockMemoryAllocator.
    blockAllocator.free(ctx.pctx);
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Width / Height / Format]
// ============================================================================

int RasterPaintEngine::getWidth() const
{
  return ctx.paintLayer.width;
}

int RasterPaintEngine::getHeight() const
{
  return ctx.paintLayer.height;
}

uint32_t RasterPaintEngine::getFormat() const
{
  return ctx.paintLayer.format;
}

// ============================================================================
// [Fog::RasterPaintEngine - Engine / Flush]
// ============================================================================

uint32_t RasterPaintEngine::getEngine() const
{
  return isSingleThreaded()
    ? PAINT_ENGINE_RASTER_ST
    : PAINT_ENGINE_RASTER_MT;
}

err_t RasterPaintEngine::setEngine(uint32_t engine, uint32_t threads)
{
  uint i;
  bool mt = (engine == PAINT_ENGINE_RASTER_MT);

  // If we already initialized demanded engine we can bail safely.
  if ((workerManager != NULL) == mt)
  {
    return ERR_OK;
  }

  // Can't set to different paint engine if already used to do non-trivial
  // operation:
  // - save() - There is saved state used by single-threaded mode, not allowed.
  // - clip() - There is used clip mask created by single-threaded mode, not allowed.
  if (states.getLength() > 0 || ctx.finalClipType == RASTER_CLIP_MASK)
  {
    return ERR_PAINTER_NOT_ALLOWED;
  }

  // We are using global thread pool.
  ThreadPool* threadPool = ThreadPool::getInstance();

  // Start multithreading...
  if (mt)
  {
    uint max = Math::min<uint>(threads > 0 ? threads : getCpuInfo()->numberOfProcessors, RASTER_MAX_WORKERS);

    // Whether to enable main-task mode (currently the main-task mode is less
    // performant so it's disabled by default).
    bool createMainTask = false;

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter::setEngine() - starting multithreading (%d threads)", max);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

    if ((workerManager = fog_new RasterWorkerManager()) == NULL)
    {
      return ERR_RT_OUT_OF_MEMORY;
    }

    // This is for testing multithreaded rendering on single cores.
    if (max < 2)
    {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
      fog_debug("Fog::Painter::setEngine() - cpu detection says 1, switching to 2");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
      max = 2;
    }

    // Create max - 1 threads if main-task mode is enabled. The main thread will
    // work as worked when flush is called (and it must be called to finish the
    // painting).
    if (createMainTask) max--;

    for (i = 0; i < max; i++)
    {
      if ((workerManager->threads[i] = threadPool->getThread(i)) == NULL) break;
    }

    // Failed to get workers. This can happen if there are many threads that
    // uses multithreaded painter, we must destroy all resources and return.
    if (i != max)
    {
      while (i)
      {
        threadPool->releaseThread(workerManager->threads[i]);
        i--;
      }

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
      fog_debug("Fog::Painter::setEngine() - failed to get %d threads from pool, releasing...", max - 1);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

      fog_delete(workerManager);
      workerManager = NULL;

      // Bailout.
      return ERR_RT_OUT_OF_THREADS;
    }

    uint count = i;
    if (createMainTask) count++;

    workerManager->numWorkers = count;

    workerManager->finishedWorkers.init(0);
    // Main thread is pseudo-waiting (so waiting must be 1).
    workerManager->waitingWorkers.init(createMainTask ? 1 : 0);

    // Reset commands and calculations position.
    workerManager->cmdPosition = 0;
    workerManager->calcPosition = 0;

    // Clear pending flags, the starting context is the same.
    ctx.state &= ~(RASTER_STATE_PENDING_MASK);

    // Create worker tasks.
    for (i = 0; i < count; i++)
    {
      workerManager->tasks[i].initCustom1(&workerManager->lock);
      RasterPaintContext& taskCtx = workerManager->tasks[i]->ctx;

      taskCtx.engine = this;
      taskCtx.offset = i;
      taskCtx.delta = count;

      taskCtx.copyFromMaster(ctx);
    }

    // Mark the main thread task if main-task enabled.
    RasterWorkerTask* mainTask = NULL;

    // If the main-task mode is enabled then we need to set special state to
    // that task.
    if (createMainTask)
    {
      mainTask = workerManager->mainTask = workerManager->tasks[count - 1].instancep();
      mainTask->state.init(RasterWorkerTask::WAITING_MAIN);
    }

    // Post worker tasks - all threads except the last one (if main-task mode
    // is enabled) which is manager by the main thread.
    if (createMainTask) count--;
    for (i = 0; i < count; i++)
    {
      RasterWorkerTask* task = workerManager->tasks[i].instancep();
      workerManager->threads[i]->getEventLoop()->postTask(task);
    }

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter::setEngine() - done");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
  }
  // Stop multithreading...
  else
  {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter::setEngine() - stopping multithreading...");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

    uint count = workerManager->numWorkers;

    ThreadEvent releaseEvent(true, false);
    workerManager->releaseEvent = &releaseEvent;

    // Release threads.
    for (i = 0; i < count; i++)
    {
      workerManager->tasks[i]->shouldQuit.set(true);
    }

    // Flush everything and wait for completion.
    flushWithQuit();
    releaseEvent.wait();

    RasterWorkerTask* mainTask = workerManager->mainTask;

    if (mainTask) count--;
    for (i = 0; i < count; i++)
    {
      threadPool->releaseThread(workerManager->threads[i], i);
      workerManager->tasks[i].destroy();
    }

    if (mainTask) mainTask->destroy();

    fog_delete(workerManager);
    workerManager = NULL;

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter::setEngine() - done");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
  }

  return ERR_OK;
}

err_t RasterPaintEngine::flush(uint32_t flags)
{
  // The PAINTER_FLUSH_SYNC flag is ignored, because we are using painter thread
  // as the main one. So if we want to flush something we need to run the
  // worker task. And generally flush helps to free resources so we really need
  // to run it here.
  //
  // The PAINTER_FLUSH_SYNC flag remains here, because other paint engines can
  // take care of that.
  if (workerManager == NULL || workerManager->cmdPosition == 0) return ERR_OK;

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
  fog_debug("Fog::Painter::flush() - begin");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

  RasterWorkerTask* mainTask = workerManager->mainTask;

  {
    AutoLock locked(workerManager->lock);
    if (!workerManager->isCompleted())
    {
      workerManager->wakeUpScheduled(mainTask);
      if (mainTask) mainTask->run();
      workerManager->allFinishedCondition.wait();
    }
  }

  // Reset command position and local command/calculation counters.
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
  fog_debug("Fog::Painter::flush() - done");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

  workerManager->commandAllocator.reset();
  workerManager->cmdPosition = 0;
  workerManager->calcPosition = 0;

  for (sysuint_t i = 0; i < workerManager->numWorkers; i++)
  {
    workerManager->tasks[i]->cmdCurrent = 0;
    workerManager->tasks[i]->calcCurrent = 0;
  }

  return ERR_OK;
}

void RasterPaintEngine::flushWithQuit()
{
  FOG_ASSERT(workerManager != NULL);

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
  fog_debug("Fog::Painter::flushWithQuit() - quitting...");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

  RasterWorkerTask* mainTask = workerManager->mainTask;

  {
    AutoLock locked(workerManager->lock);

    workerManager->wakeUpSleeping(mainTask);
    if (mainTask) mainTask->run();
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Hints]
// ============================================================================

int RasterPaintEngine::getHint(uint32_t hint) const
{
  switch (hint)
  {
    case PAINTER_HINT_ANTIALIASING_QUALITY:
      return (int)ctx.hints.aaQuality;

    case PAINTER_HINT_IMAGE_INTERPOLATION:
      return (int)ctx.hints.imageInterpolation;

    case PAINTER_HINT_COLOR_INTERPOLATION:
      return (int)ctx.hints.colorInterpolation;

    case PAINTER_HINT_OUTLINE_TEXT:
      return (int)ctx.hints.forceOutlineText;

    default:
      return -1;
  }
}

err_t RasterPaintEngine::setHint(uint32_t hint, int value)
{
  switch (hint)
  {
    case PAINTER_HINT_ANTIALIASING_QUALITY:
      if (((uint)value >= ANTI_ALIASING_COUNT))
        return ERR_RT_INVALID_ARGUMENT;

      ctx.hints.aaQuality = (uint8_t)value;
      return ERR_OK;

    case PAINTER_HINT_IMAGE_INTERPOLATION:
      if (((uint)value >= IMAGE_INTERPOLATION_COUNT))
        return ERR_RT_INVALID_ARGUMENT;

      ctx.hints.imageInterpolation = (uint8_t)value;
      return ERR_OK;

    case PAINTER_HINT_COLOR_INTERPOLATION:
      if (((uint)value >= COLOR_INTERPOLATION_COUNT))
        return ERR_RT_INVALID_ARGUMENT;

      ctx.hints.colorInterpolation = (uint8_t)value;
      return ERR_OK;

    case PAINTER_HINT_OUTLINE_TEXT:
      if (((uint)value >= 2))
        return ERR_RT_INVALID_ARGUMENT;

      ctx.hints.forceOutlineText = (uint8_t)value;
      return ERR_OK;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Meta]
// ============================================================================

err_t RasterPaintEngine::setMetaVars(const Region& region, const IntPoint& origin)
{
  _restoreStates(ctx.layerId);

  IntBox bounds(0, 0, ctx.paintLayer.width, ctx.paintLayer.height);
  sysuint_t len = region.getLength();

  ctx.metaOrigin = origin;
  ctx.metaRegion = region;

  ctx.userOrigin.clear();
  ctx.userRegion = Region::infinite();

  // We must set final origin here, because _setCapsDefaults() will set the
  // final transformation matrix to identity translated by final-origin.
  ctx.finalOrigin = origin;

  // Reset caps. We are in setMetaVars() which is designed to reset all caps!
  _setCapsDefaults();

  return _updateWorkRegion();
}

err_t RasterPaintEngine::resetMetaVars()
{
  return setMetaVars(Region::infinite(), IntPoint(0, 0));
}

err_t RasterPaintEngine::setUserVars(const Region& region, const IntPoint& origin)
{
  ctx.userOrigin = origin;
  ctx.userRegion = region;

  return _updateWorkRegion();
}

err_t RasterPaintEngine::resetUserVars()
{
  return setUserVars(Region::infinite(), IntPoint(0, 0));
}

Region RasterPaintEngine::getMetaRegion() const
{
  return ctx.metaRegion;
}

Region RasterPaintEngine::getUserRegion() const
{
  return ctx.userRegion;
}

IntPoint RasterPaintEngine::getMetaOrigin() const
{
  return ctx.metaOrigin;
}

IntPoint RasterPaintEngine::getUserOrigin() const
{
  return ctx.userOrigin;
}

// ============================================================================
// [Fog::RasterPaintEngine - Paint Parameters]
// ============================================================================

uint32_t RasterPaintEngine::getSourceType() const
{
  return ctx.ops.sourceType;
}

Argb RasterPaintEngine::getSourceArgb() const
{
  if (ctx.ops.sourceType == PAINTER_SOURCE_ARGB)
    return Argb(ctx.solid.argb);
  else // if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
    return Argb(0x00000000);
}

Pattern RasterPaintEngine::getSourcePattern() const
{
  if (ctx.ops.sourceType == PAINTER_SOURCE_ARGB)
    return Pattern(ctx.solid.argb);
  else // if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
    return Pattern(ctx.pattern.instance());
}

err_t RasterPaintEngine::setSource(Argb argb)
{
  // Destroy the pattern instance if needed - declared as Static<Pattern>.
  if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
  {
    ctx.ops.sourceType = PAINTER_SOURCE_ARGB;
    ctx.pattern.destroy();
    _resetRasterPatternContext();
  }

  ctx.solid.argb = argb;

  if (argb.isAlpha0xFF())
  {
    ctx.solid.prgb = argb;
    ctx.ops.sourceFormat = IMAGE_FORMAT_XRGB32;
  }
  else
  {
    ctx.solid.prgb = ColorUtil::premultiply(argb);
    ctx.ops.sourceFormat = IMAGE_FORMAT_PRGB32;
  }

  return ERR_OK;
}

err_t RasterPaintEngine::setSource(const Pattern& pattern)
{
  if (pattern.isSolid()) return setSource(pattern.getColor());

  // Create the pattern instance if needed - declared as Static<Pattern>.
  if (ctx.ops.sourceType == PAINTER_SOURCE_ARGB)
  {
    ctx.ops.sourceType = PAINTER_SOURCE_PATTERN;
    ctx.pattern.init(pattern);
  }
  else // if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
  {
    ctx.pattern.instance() = pattern;
  }

  _resetRasterPatternContext();
  return ERR_OK;
}

err_t RasterPaintEngine::resetSource()
{
  return RasterPaintEngine::setSource(Argb(0xFF000000));
}

uint32_t RasterPaintEngine::getOperator() const
{
  return ctx.ops.op;
}

err_t RasterPaintEngine::setOperator(uint32_t op)
{
  if (op >= OPERATOR_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.ops.op = op;
  ctx.state &= ~(RASTER_STATE_NO_PAINT_OPERATOR);
  ctx.funcs = rasterFuncs.getCompositeFuncs(op, ctx.paintLayer.format);

  if (FOG_UNLIKELY(op == OPERATOR_DST)) ctx.state |= RASTER_STATE_NO_PAINT_OPERATOR;
  return ERR_OK;
}

float RasterPaintEngine::getAlpha() const
{
  return ctx.alpha;
}

err_t RasterPaintEngine::setAlpha(float alpha)
{
  if (alpha < 0.0f || alpha > 1.0f) return ERR_RT_INVALID_ARGUMENT;

  ctx.alpha = alpha;
  ctx.ops.alpha255 = (uint8_t)Math::uround(alpha * 255.0f);

  if (FOG_UNLIKELY(ctx.ops.alpha255 == 0x00))
    ctx.state |= RASTER_STATE_NO_PAINT_ALPHA;
  else
    ctx.state &= ~RASTER_STATE_NO_PAINT_ALPHA;

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Clip Parameters]
// ============================================================================

uint32_t RasterPaintEngine::getClipRule() const
{
  return ctx.hints.clipRule;
}

err_t RasterPaintEngine::setClipRule(uint32_t clipRule)
{
  if (clipRule >= CLIP_RULE_COUNT) return ERR_RT_INVALID_ARGUMENT;
  ctx.hints.clipRule = clipRule;

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Fill Parameters]
// ============================================================================

uint32_t RasterPaintEngine::getFillRule() const
{
  return ctx.hints.fillRule;
}

err_t RasterPaintEngine::setFillRule(uint32_t fillRule)
{
  if (fillRule >= FILL_RULE_COUNT) return ERR_RT_INVALID_ARGUMENT;
  ctx.hints.fillRule = fillRule;

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Stroke Parameters]
// ============================================================================

PathStrokeParams RasterPaintEngine::getStrokeParams() const
{
  return ctx.strokeParams;
}

err_t RasterPaintEngine::setStrokeParams(const PathStrokeParams& strokeParams)
{
  if (ctx.strokeParams.getStartCap() >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;
  if (ctx.strokeParams.getEndCap() >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;
  if (ctx.strokeParams.getLineJoin() >= LINE_JOIN_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams = strokeParams;
  _updateLineWidth();
  return ERR_OK;
}

double RasterPaintEngine::getLineWidth() const
{
  return ctx.strokeParams.getLineWidth();
}

err_t RasterPaintEngine::setLineWidth(double lineWidth)
{
  ctx.strokeParams.setLineWidth(lineWidth);
  _updateLineWidth();
  return ERR_OK;
}

uint32_t RasterPaintEngine::getStartCap() const
{
  return ctx.strokeParams.getStartCap();
}

err_t RasterPaintEngine::setStartCap(uint32_t startCap)
{
  if (startCap >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams.setStartCap(startCap);
  _updateLineWidth();
  return ERR_OK;
}

uint32_t RasterPaintEngine::getEndCap() const
{
  return ctx.strokeParams.getEndCap();
}

err_t RasterPaintEngine::setEndCap(uint32_t endCap)
{
  if (endCap >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams.setEndCap(endCap);
  _updateLineWidth();
  return ERR_OK;
}

err_t RasterPaintEngine::setLineCaps(uint32_t lineCaps)
{
  if (lineCaps >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams.setLineCaps(lineCaps);
  _updateLineWidth();
  return ERR_OK;
}

uint32_t RasterPaintEngine::getLineJoin() const
{
  return ctx.strokeParams.getLineJoin();
}

err_t RasterPaintEngine::setLineJoin(uint32_t lineJoin)
{
  if (lineJoin >= LINE_JOIN_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams.setLineJoin(lineJoin);
  return ERR_OK;
}

double RasterPaintEngine::getMiterLimit() const
{
  return ctx.strokeParams.getMiterLimit();
}

err_t RasterPaintEngine::setMiterLimit(double miterLimit)
{
  ctx.strokeParams.setMiterLimit(miterLimit);
  _updateLineWidth();
  return ERR_OK;
}

List<double> RasterPaintEngine::getDashes() const
{
  return ctx.strokeParams.getDashes();
}

err_t RasterPaintEngine::setDashes(const double* dashes, sysuint_t count)
{
  ctx.strokeParams.setDashes(dashes, count);
  _updateLineWidth();
  return ERR_OK;
}

err_t RasterPaintEngine::setDashes(const List<double>& dashes)
{
  ctx.strokeParams.setDashes(dashes);
  _updateLineWidth();
  return ERR_OK;
}

double RasterPaintEngine::getDashOffset() const
{
  return ctx.strokeParams.getDashOffset();
}

err_t RasterPaintEngine::setDashOffset(double offset)
{
  ctx.strokeParams.setDashOffset(offset);
  _updateLineWidth();
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Transformations]
// ============================================================================

err_t RasterPaintEngine::setMatrix(const DoubleMatrix& m)
{
  ctx.userMatrix = m;
  return _updateMatrix(false);
}

err_t RasterPaintEngine::resetMatrix()
{
  ctx.userMatrix.reset();
  return _updateMatrix(false);
}

DoubleMatrix RasterPaintEngine::getMatrix() const
{
  return ctx.userMatrix;
}

err_t RasterPaintEngine::rotate(double angle, uint32_t order)
{
  ctx.userMatrix.rotate(angle, order);
  return _updateMatrix(false);
}

err_t RasterPaintEngine::scale(int sx, int sy, uint32_t order)
{
  ctx.userMatrix.scale((double)sx, (double)sy, order);
  return _updateMatrix(false);
}

err_t RasterPaintEngine::scale(double sx, double sy, uint32_t order)
{
  ctx.userMatrix.scale(sx, sy, order);
  return _updateMatrix(false);
}

err_t RasterPaintEngine::skew(double sx, double sy, uint32_t order)
{
  ctx.userMatrix.scale(sx, sy, order);
  return _updateMatrix(false);
}

err_t RasterPaintEngine::translate(int x, int y, uint32_t order)
{
  ctx.userMatrix.translate((double)x, (double)y, order);
  return _updateMatrix(true);
}

err_t RasterPaintEngine::translate(double x, double y, uint32_t order)
{
  ctx.userMatrix.translate(x, y, order);
  return _updateMatrix(true);
}

err_t RasterPaintEngine::transform(const DoubleMatrix& m, uint32_t order)
{
  ctx.userMatrix.multiply(m, order);
  return _updateMatrix(false);
}

err_t RasterPaintEngine::worldToScreen(DoublePoint* pt) const
{
  if (pt == NULL) return ERR_RT_INVALID_ARGUMENT;

  if (ctx.hints.transformType >= RASTER_TRANSFORM_AFFINE)
  {
    ctx.userMatrix.transformPoint(&pt->x, &pt->y);
  }
  else
  {
    pt->x += ctx.userMatrix.tx;
    pt->y += ctx.userMatrix.ty;
  }

  return ERR_OK;
}

err_t RasterPaintEngine::screenToWorld(DoublePoint* pt) const
{
  if (pt == NULL) return ERR_RT_INVALID_ARGUMENT;

  if (ctx.hints.transformType >= RASTER_TRANSFORM_AFFINE)
  {
    ctx.userMatrix.inverted().transformPoint(&pt->x, &pt->y);
  }
  else
  {
    pt->x -= ctx.userMatrix.tx;
    pt->y -= ctx.userMatrix.ty;
  }

  return ERR_OK;
}

err_t RasterPaintEngine::alignPoint(DoublePoint* pt) const
{
  FOG_RETURN_ON_ERROR(RasterPaintEngine::worldToScreen(pt));
  pt->set(floor(pt->x) + 0.5, floor(pt->y) + 0.5);
  FOG_RETURN_ON_ERROR(RasterPaintEngine::screenToWorld(pt));

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - State]
// ============================================================================

err_t RasterPaintEngine::save(uint32_t flags)
{
  RasterPaintState* state = NULL;

  // Special case, save nothing? We supports this situation adding very simple
  // state to the list.
  if (FOG_UNLIKELY(flags == 0))
  {
    state = reinterpret_cast<RasterPaintState*>(blockAllocator.alloc(sizeof(RasterPaintState)));
    if (FOG_UNLIKELY(state == NULL)) return ERR_RT_OUT_OF_MEMORY;
    fog_new_p(state) RasterPaintState(ctx.layerId, flags);
  }
  else
  {
    state = reinterpret_cast<RasterPaintVarState*>(blockAllocator.alloc(sizeof(RasterPaintVarState)));
    if (FOG_UNLIKELY(state == NULL)) return ERR_RT_OUT_OF_MEMORY;
    fog_new_p(state) RasterPaintVarState(ctx.layerId, flags);
  }

  err_t err = states.append(reinterpret_cast<RasterPaintState*>(state));
  if (FOG_LIKELY(err == ERR_OK))
  {
    state->save(this);
  }
  else
  {
    // Clear flags so state destructor will not delete unsaved state
    // (uninitialized instances).
    state->flags = 0;
    _deleteState(state);
  }
  return err;
}

err_t RasterPaintEngine::restore()
{
  // Called invalid restore?
  if (FOG_UNLIKELY(states.isEmpty()))
    return ERR_PAINTER_NO_STATE;

  if (FOG_UNLIKELY(states.top()->layerId != ctx.layerId))
    return ERR_PAINTER_NO_STATE;

  RasterPaintState* state = states.takeLast();
  state->restore(this);
  _deleteState(state);

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Clipping]
// ============================================================================

err_t RasterPaintEngine::clipRect(const IntRect& rect, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_COND(rect.isValid());

  // Special case (the most used).
  //
  // Clip to rectangle is probably the most used clip function in the painting
  // process. This is reason why it's heavily optimized. The main idea is to
  // not calculate what is not needed and to keep clipping type to be
  // RASTER_CLIP_SIMPLE or RASTER_CLIP_REGION. We just want not to use
  // RASTER_CLIP_MASK if it's really not required.
  if (FOG_LIKELY(ctx.hints.transformType == RASTER_TRANSFORM_EXACT))
  {
    // Convert rectangle to box and translate it to physical raster position.
    IntBox finalBox(rect);
    finalBox.translate(ctx.finalTranslate);

    return _clipOpBox(finalBox, clipOp);
  }

  // Do vector clipping if we can't stay with simple or region based one.
  return clipRect(DoubleRect(rect), clipOp);
}

err_t RasterPaintEngine::clipMask(const IntPoint& pt, const Image& mask, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_COND(!mask.isEmpty());

  if (FOG_LIKELY(ctx.hints.transformType == RASTER_TRANSFORM_EXACT))
  {
    // TODO: PAINTER MASK.
  }

  // TODO: PAINTER MASK.
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t RasterPaintEngine::clipRegion(const Region& region, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_COND(!region.isEmpty());
  if (region.isInfinite()) return _clipOpInfinite(clipOp);

  // Special case (the most used).
  if (FOG_LIKELY(ctx.hints.transformType == RASTER_TRANSFORM_EXACT))
  {
    if (ctx.finalTranslate.x != 0 || ctx.finalTranslate.y != 0)
    {
      FOG_RETURN_ON_ERROR(Region::translateAndClip(tmpRegion0, region, ctx.finalTranslate, ctx.workClipBox));
      return _clipOpRegion(tmpRegion0, clipOp);
    }
    else
    {
      return _clipOpRegion(region, clipOp);
    }
  }

  // Do vector clipping if we can't stay with simple or region based one.
  tmpPath0.clear();
  tmpPath0.addRegion(region);

  return _serializeMaskPath(tmpPath0, false, clipOp);
}

err_t RasterPaintEngine::clipRect(const DoubleRect& rect, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_COND(rect.isValid());

  if (FOG_UNLIKELY(!rect.isValid())) return ERR_OK;

  if (ctx.hints.transformType < RASTER_TRANSFORM_AFFINE)
  {
    IntBox finalBox;

    if (Raster_canAlignToGrid(finalBox, rect, ctx.finalMatrix.tx, ctx.finalMatrix.ty))
    {
      return _clipOpBox(finalBox, clipOp);
    }
  }

usePath:
  tmpPath0.clear();
  tmpPath0.addRect(rect);

  return _serializeMaskPath(tmpPath0, false, clipOp);
}

err_t RasterPaintEngine::clipMask(const DoublePoint& pt, const Image& mask, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_COND(!mask.isEmpty());

  // TODO: PAINTER MASK.
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t RasterPaintEngine::clipRects(const DoubleRect* r, sysuint_t count, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_COND(count > 0);

  tmpPath0.clear();
  tmpPath0.addRects(r, count);

  return _serializeMaskPath(tmpPath0, false, clipOp);
}

err_t RasterPaintEngine::clipRound(const DoubleRect& r, const DoublePoint& radius, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_COND(r.isValid());

  tmpPath0.clear();
  tmpPath0.addRound(r, radius);

  return _serializeMaskPath(tmpPath0, false, clipOp);
}

err_t RasterPaintEngine::clipEllipse(const DoublePoint& cp, const DoublePoint& r, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_FUNC();

  tmpPath0.clear();
  tmpPath0.addEllipse(cp, r, clipOp);

  return _serializeMaskPath(tmpPath0, false, clipOp);
}

err_t RasterPaintEngine::clipArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_FUNC();

  tmpPath0.clear();
  tmpPath0.addArc(cp, r, start, sweep);

  return _serializeMaskPath(tmpPath0, false, clipOp);
}

err_t RasterPaintEngine::clipPath(const DoublePath& path, bool stroke, uint32_t clipOp)
{
  RASTER_ENTER_CLIP_FUNC();

  return _serializeMaskPath(path, stroke, clipOp);
}

err_t RasterPaintEngine::resetClip()
{
  return _clipSetRegion(ctx.workRegion);
}

// ============================================================================
// [Fog::RasterPaintEngine - Raster drawing]
// ============================================================================

err_t RasterPaintEngine::drawPoint(const IntPoint& p)
{
  return RasterPaintEngine::drawPoint(
    DoublePoint((double)p.x + 0.5, (double)p.y + 0.5));
}

err_t RasterPaintEngine::drawLine(const IntPoint& start, const IntPoint& end)
{
  return RasterPaintEngine::drawLine(
    DoublePoint((double)start.x + 0.5, (double)start.y + 0.5),
    DoublePoint((double)end.x   + 0.5, (double)end.y   + 0.5));
}

err_t RasterPaintEngine::drawRect(const IntRect& r)
{
  RASTER_ENTER_PAINT_COND(r.isValid());

  if (FOG_LIKELY(ctx.hints.transformType == RASTER_TRANSFORM_EXACT &&
                 ctx.hints.lineIsSimple))
  {
    int tx = ctx.finalTranslate.x;
    int ty = ctx.finalTranslate.y;

    int x1 = r.getX1() + tx    , y1 = r.getY1() + ty    ;
    int x2 = r.getX2() + tx + 1, y2 = r.getY2() + ty + 1;

    TemporaryRegion<4> box;
    RegionData* box_d = box._d;

    box_d->extents.set(x1, y1, x2, y2);

    if (r.w <= 2 || r.h <= 2)
    {
      box_d->rects[0] = box_d->extents;
      box_d->length = 1;
    }
    else
    {
      box_d->rects[0].set(x1    , y1    , x2    , y1 + 1);
      box_d->rects[1].set(x1    , y1 + 1, x1 + 1, y2 - 1);
      box_d->rects[2].set(x2 - 1, y1 + 1, x2    , y2 - 1);
      box_d->rects[3].set(x1    , y2 - 1, x2    , y2    );
      box_d->length = 4;
    }

    if (ctx.finalClipType != RASTER_CLIP_SIMPLE || ctx.finalClipBox.subsumes(box_d->extents))
    {
      return _serializePaintBoxes(box_d->rects, box_d->length);
    }
    else
    {
      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion0, box, ctx.finalRegion, REGION_OP_INTERSECT));
      return _serializePaintRegion(tmpRegion0);
    }
  }
  else
  {
    return RasterPaintEngine::drawRect(
      DoubleRect((double)r.x + 0.5, (double)r.y + 0.5,
                 (double)r.w      , (double)r.h)     );
  }
}

err_t RasterPaintEngine::drawRound(const IntRect& r, const IntPoint& radius)
{
  return RasterPaintEngine::drawRound(
    DoubleRect(
      (double)r.x + 0.5,
      (double)r.y + 0.5,
      (double)r.getWidth(),
      (double)r.getHeight()),
    DoublePoint(
      (double)radius.x,
      (double)radius.y));
}

err_t RasterPaintEngine::fillRect(const IntRect& r)
{
  RASTER_ENTER_PAINT_COND(r.isValid());

  if (FOG_LIKELY(ctx.hints.transformType == RASTER_TRANSFORM_EXACT))
  {
    IntBox box(r);
    box.translate(ctx.finalTranslate);

    if (ctx.finalClipType == RASTER_CLIP_SIMPLE)
    {
      if (!IntBox::intersect(box, box, ctx.finalClipBox)) return ERR_OK;
      return _serializePaintBoxes(&box, 1);
    }
    else if (ctx.finalClipType == RASTER_CLIP_REGION)
    {
      if (!IntBox::intersect(box, box, ctx.finalClipBox)) return ERR_OK;
      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion0, ctx.finalRegion, box, REGION_OP_INTERSECT));
      return _serializePaintRegion(tmpRegion0);
    }
    else
    {
      if (!IntBox::intersect(box, box, ctx.workClipBox)) return ERR_OK;
      return _serializePaintBoxes(&box, 1);
    }
  }
  else
  {
    return RasterPaintEngine::fillRect(
      DoubleRect((double)r.x, (double)r.y,
                 (double)r.w, (double)r.h));
  }
}

err_t RasterPaintEngine::fillRects(const IntRect* r, sysuint_t count)
{
  RASTER_ENTER_PAINT_COND(count > 0);

  if (FOG_LIKELY(ctx.hints.transformType == RASTER_TRANSFORM_EXACT))
  {
    tmpRegion0.set(r, count);
    FOG_RETURN_ON_ERROR(Region::translateAndClip(tmpRegion0, tmpRegion0, ctx.finalTranslate, ctx.finalClipBox));

    if (ctx.finalClipType == RASTER_CLIP_REGION)
    {
      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion1, tmpRegion0, ctx.finalRegion, REGION_OP_INTERSECT));
      return _serializePaintRegion(tmpRegion1);
    }
    else
    {
      return _serializePaintRegion(tmpRegion0);
    }
  }
  else
  {
    curPath.clear();
    for (sysuint_t i = 0; i < count; i++)
    {
      curPath.addRect(
        DoubleRect((double)r[i].x, (double)r[i].y,
                   (double)r[i].w, (double)r[i].h));
    }
    return fillPath(curPath);
  }
}

err_t RasterPaintEngine::fillRound(const IntRect& r, const IntPoint& radius)
{
  return RasterPaintEngine::fillRound(
    DoubleRect((double)r.x, (double)r.y, (double)r.w, (double)r.h),
    DoublePoint((double)radius.x, (double)radius.y));
}

err_t RasterPaintEngine::fillRegion(const Region& region)
{
  RASTER_ENTER_PAINT_COND(!region.isEmpty());

  if (FOG_UNLIKELY(region.isInfinite())) return fillAll();

  if (FOG_LIKELY(ctx.hints.transformType == RASTER_TRANSFORM_EXACT))
  {
    FOG_RETURN_ON_ERROR(region.translateAndClip(tmpRegion0, region, ctx.finalTranslate, ctx.finalClipBox));

    if (ctx.finalClipType == RASTER_CLIP_REGION)
    {
      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion1, tmpRegion0, ctx.finalRegion, REGION_OP_INTERSECT));
      return _serializePaintRegion(tmpRegion1);
    }
    else
    {
      return _serializePaintRegion(tmpRegion0);
    }
  }

  curPath.clear();
  curPath.addRegion(region);

  return fillPath(curPath);
}

err_t RasterPaintEngine::fillAll()
{
  RASTER_ENTER_PAINT_FUNC();

  if (ctx.finalClipType != RASTER_CLIP_REGION)
    return _serializePaintBoxes(&ctx.finalClipBox, 1);
  else
    return _serializePaintRegion(ctx.finalRegion);
}

// ============================================================================
// [Fog::RasterPaintEngine - Vector drawing]
// ============================================================================

err_t RasterPaintEngine::drawPoint(const DoublePoint& p)
{
  RASTER_ENTER_PAINT_FUNC();

  curPath.clear();
  curPath.moveTo(p);
  curPath.lineTo(p.x, p.y + 0.0001);
  return _serializePaintPath(curPath, true);
}

err_t RasterPaintEngine::drawLine(const DoublePoint& start, const DoublePoint& end)
{
  RASTER_ENTER_PAINT_FUNC();

  curPath.clear();
  curPath.moveTo(start);
  curPath.lineTo(end);
  return _serializePaintPath(curPath, true);
}

err_t RasterPaintEngine::drawLine(const DoublePoint* pts, sysuint_t count)
{
  RASTER_ENTER_PAINT_COND(count > 0);

  curPath.clear();
  curPath.moveTo(pts[0]);
  if (count > 1)
    curPath.lineTo(pts + 1, count - 1);
  else
    curPath.lineTo(pts[0].x, pts[0].y + 0.0001);
  return _serializePaintPath(curPath, true);
}

err_t RasterPaintEngine::drawPolygon(const DoublePoint* pts, sysuint_t count)
{
  RASTER_ENTER_PAINT_COND(count > 0);

  curPath.clear();
  curPath.moveTo(pts[0]);
  if (count > 1)
    curPath.lineTo(pts + 1, count - 1);
  else
    curPath.lineTo(pts[0].x, pts[0].y + 0.0001);
  curPath.closePolygon();
  return _serializePaintPath(curPath, true);
}

err_t RasterPaintEngine::drawRect(const DoubleRect& r)
{
  RASTER_ENTER_PAINT_COND(r.isValid());

  curPath.clear();
  curPath.addRect(r);
  return _serializePaintPath(curPath, true);
}

err_t RasterPaintEngine::drawRects(const DoubleRect* r, sysuint_t count)
{
  RASTER_ENTER_PAINT_COND(count > 0);

  curPath.clear();
  curPath.addRects(r, count);
  return _serializePaintPath(curPath, true);
}

err_t RasterPaintEngine::drawRound(const DoubleRect& r, const DoublePoint& radius)
{
  RASTER_ENTER_PAINT_FUNC();

  curPath.clear();
  curPath.addRound(r, radius);
  return _serializePaintPath(curPath, true);
}

err_t RasterPaintEngine::drawEllipse(const DoublePoint& cp, const DoublePoint& r)
{
  RASTER_ENTER_PAINT_FUNC();

  curPath.clear();
  curPath.addEllipse(cp, r);
  return _serializePaintPath(curPath, true);
}

err_t RasterPaintEngine::drawArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep)
{
  RASTER_ENTER_PAINT_FUNC();

  curPath.clear();
  curPath.addArc(cp, r, start, sweep);
  return _serializePaintPath(curPath, true);
}

err_t RasterPaintEngine::drawPath(const DoublePath& path)
{
  RASTER_ENTER_PAINT_FUNC();

  return _serializePaintPath(path, true);
}

err_t RasterPaintEngine::fillPolygon(const DoublePoint* pts, sysuint_t count)
{
  RASTER_ENTER_PAINT_COND(count > 0);

  curPath.clear();
  curPath.moveTo(pts[0]);
  if (count > 1)
    curPath.lineTo(pts + 1, count - 1);
  else
    curPath.lineTo(pts[0].x, pts[0].y + 0.0001);
  curPath.closePolygon();
  return _serializePaintPath(curPath, false);
}

err_t RasterPaintEngine::fillRect(const DoubleRect& r)
{
  RASTER_ENTER_PAINT_COND(r.isValid());

  if (ctx.hints.transformType < RASTER_TRANSFORM_AFFINE)
  {
    IntBox finalBox;
    if (Raster_canAlignToGrid(finalBox, r, ctx.finalMatrix.tx, ctx.finalMatrix.ty))
    {
      if (ctx.finalClipType == RASTER_CLIP_SIMPLE)
      {
        if (!IntBox::intersect(finalBox, finalBox, ctx.finalClipBox)) return ERR_OK;
        return _serializePaintBoxes(&finalBox, 1);
      }
      else if (ctx.finalClipType == RASTER_CLIP_REGION)
      {
        FOG_RETURN_ON_ERROR(Region::combine(tmpRegion0, ctx.finalRegion, finalBox, REGION_OP_INTERSECT));
        return _serializePaintRegion(tmpRegion0);
      }
      else
      {
        if (!IntBox::intersect(finalBox, finalBox, ctx.workClipBox)) return ERR_OK;
        return _serializePaintBoxes(&finalBox, 1);
      }
    }
  }

usePath:
  curPath.clear();
  curPath.addRect(r);
  return _serializePaintPath(curPath, false);
}

err_t RasterPaintEngine::fillRects(const DoubleRect* r, sysuint_t count)
{
  RASTER_ENTER_PAINT_COND(count > 0);

  // Single rect fills can be more optimized.
  if (count == 1) fillRect(r[0]);

  curPath.clear();
  curPath.addRects(r, count);
  return _serializePaintPath(curPath, false);
}

err_t RasterPaintEngine::fillRound(const DoubleRect& r, const DoublePoint& radius)
{
  RASTER_ENTER_PAINT_FUNC();

  curPath.clear();
  curPath.addRound(r, radius);
  return _serializePaintPath(curPath, false);
}

err_t RasterPaintEngine::fillEllipse(const DoublePoint& cp, const DoublePoint& r)
{
  RASTER_ENTER_PAINT_FUNC();

  curPath.clear();
  curPath.addEllipse(cp, r);
  return _serializePaintPath(curPath, false);
}

err_t RasterPaintEngine::fillArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep)
{
  RASTER_ENTER_PAINT_FUNC();

  curPath.clear();
  curPath.addArc(cp, r, start, sweep);
  return _serializePaintPath(curPath, false);
}

err_t RasterPaintEngine::fillPath(const DoublePath& path)
{
  RASTER_ENTER_PAINT_FUNC();

  return _serializePaintPath(path, false);
}

// ============================================================================
// [Fog::RasterPaintEngine - Glyph Drawing]
// ============================================================================

err_t RasterPaintEngine::drawGlyph(const IntPoint&p, const Image& glyph, const IntRect* irect_)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t RasterPaintEngine::drawGlyph(const DoublePoint&p, const Image& glyph, const IntRect* irect_)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t RasterPaintEngine::drawGlyph(const IntPoint& pt_, const Glyph& glyph, const IntRect* grect_)
{
  RASTER_ENTER_PAINT_FUNC();

  tmpGlyphSet.clear();

  int tx = ctx.finalTranslate.x;
  int ty = ctx.finalTranslate.y;

  IntPoint pt(pt_.x + tx, pt_.y + ty);
  IntRect grect;

  if (grect_)
  {
    grect.set(grect_->x + tx, grect_->y + ty, grect_->w, grect_->h);
    grect_ = &grect;
  }

  FOG_RETURN_ON_ERROR(tmpGlyphSet.begin(1));
  tmpGlyphSet._add(glyph._d->ref());
  FOG_RETURN_ON_ERROR(tmpGlyphSet.end());

  return _serializePaintGlyphSet(pt, tmpGlyphSet, grect_);
}

err_t RasterPaintEngine::drawGlyph(const DoublePoint& pt, const Glyph& glyph, const IntRect* grect_)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t RasterPaintEngine::drawGlyphSet(const IntPoint& pt_, const GlyphSet& glyphSet, const IntRect* clip_)
{
  RASTER_ENTER_PAINT_FUNC();

  int tx = ctx.finalTranslate.x;
  int ty = ctx.finalTranslate.y;

  IntPoint pt(pt_.x + tx, pt_.y + ty);
  IntRect clip;

  if (clip_)
  {
    clip.set(clip_->x + tx, clip_->y + ty, clip_->w, clip_->h);
    clip_ = &clip;
  }

  return _serializePaintGlyphSet(pt, glyphSet, clip_);
}

err_t RasterPaintEngine::drawGlyphSet(const DoublePoint& pt, const GlyphSet& glyphSet, const IntRect* clip)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintEngine - Text Drawing]
// ============================================================================

err_t RasterPaintEngine::drawText(const IntPoint& pt_, const String& text, const Font& font, const IntRect* clip_)
{
  RASTER_ENTER_PAINT_FUNC();

  // TODO: Not optimal, no clip used.
  if (ctx.hints.transformType >= RASTER_TRANSFORM_SUBPX || ctx.hints.forceOutlineText)
  {
    tmpPath0.clear();
    FOG_RETURN_ON_ERROR(font.getOutline(text, tmpPath0));
    tmpPath0.translate((double)pt_.x, (double)pt_.y + font.getAscent());
    return _serializePaintPath(tmpPath0, false);
  }
  else
  {
    int tx = ctx.finalTranslate.x;
    int ty = ctx.finalTranslate.y;

    IntPoint pt(pt_.x + tx, pt_.y + ty);
    IntRect clip;

    if (clip_)
    {
      clip.set(clip_->x + tx, clip_->y + ty, clip_->w, clip_->h);
      clip_ = &clip;
    }

    tmpGlyphSet.clear();
    FOG_RETURN_ON_ERROR(font.getGlyphSet(text.getData(), text.getLength(), tmpGlyphSet));

    return _serializePaintGlyphSet(pt, tmpGlyphSet, clip_);
  }
}

err_t RasterPaintEngine::drawText(const DoublePoint& pt_, const String& text, const Font& font, const IntRect* clip_)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t RasterPaintEngine::drawText(const IntRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip_)
{
  RASTER_ENTER_PAINT_FUNC();

  int tx = ctx.finalTranslate.x;
  int ty = ctx.finalTranslate.y;

  IntRect clip;

  if (clip_)
  {
    clip.set(clip_->x + tx, clip_->y + ty, clip_->w, clip_->h);
    clip_ = &clip;
  }
  
  int wsize;
  int hsize;

  // TODO: Not optimal, no clip used.
  if (ctx.hints.transformType >= RASTER_TRANSFORM_SUBPX || ctx.hints.forceOutlineText)
  {
    tmpPath0.clear();
    FOG_RETURN_ON_ERROR(font.getOutline(text, tmpPath0));

    wsize = (int)tmpPath0.getBoundingRect().getWidth();
    hsize = (int)font.getHeight();
  }
  else
  {
    tmpGlyphSet.clear();
    FOG_RETURN_ON_ERROR(font.getGlyphSet(text.getData(), text.getLength(), tmpGlyphSet));

    wsize = tmpGlyphSet.getAdvance();
    hsize = (int)font.getHeight();
  }

  int x = rect.x + tx;
  int y = rect.y + ty;
  int w = rect.w;
  int h = rect.h;

  switch (align & TEXT_ALIGN_HMASK)
  {
    case TEXT_ALIGN_LEFT:
      break;
    case TEXT_ALIGN_RIGHT:
      x = x + (w - wsize);
      break;
    case TEXT_ALIGN_HCENTER:
      x = x + (w - wsize) / 2;
      break;
  }

  switch (align & TEXT_ALIGN_VMASK)
  {
    case TEXT_ALIGN_TOP:
      break;
    case TEXT_ALIGN_BOTTOM:
      y = y + (h - hsize);
      break;
    case TEXT_ALIGN_VCENTER:
      y = y + (h - hsize) / 2;
      break;
  }

  // TODO: Not optimal, no clip used.
  if (ctx.hints.transformType >= RASTER_TRANSFORM_SUBPX || ctx.hints.forceOutlineText)
  {
    tmpPath0.translate((double)x - tx, (double)y - ty + font.getAscent());
    return _serializePaintPath(tmpPath0, false);
  }
  else
  {
    return _serializePaintGlyphSet(IntPoint(x, y), tmpGlyphSet, clip_);
  }
}

err_t RasterPaintEngine::drawText(const DoubleRect& rect, const String& text, const Font& font, uint32_t align, const IntRect* clip_)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintEngine - Image drawing]
// ============================================================================

err_t RasterPaintEngine::drawImage(const IntPoint& p, const Image& image, const IntRect* irect)
{
  RASTER_ENTER_PAINT_IMAGE(image);

  // Check whether we can use pixel-aligned blitter (speed optimization).
  if (FOG_LIKELY(ctx.hints.transformType == RASTER_TRANSFORM_EXACT))
  {
    int srcx = 0;
    int srcy = 0;
    int dstx = p.x + ctx.finalTranslate.x;
    int dsty = p.y + ctx.finalTranslate.y;
    int dstw;
    int dsth;

    if (irect == NULL)
    {
      dstw = image.getWidth();
      dsth = image.getHeight();
    }
    else
    {
      if (!irect->isValid()) return ERR_OK;

      srcx = irect->x;
      if (srcx < 0) return ERR_RT_INVALID_ARGUMENT;
      srcy = irect->y;
      if (srcy < 0) return ERR_RT_INVALID_ARGUMENT;

      dstw = Math::min(image.getWidth(), irect->getWidth());
      if (dstw == 0) return ERR_OK;
      dsth = Math::min(image.getHeight(), irect->getHeight());
      if (dsth == 0) return ERR_OK;
    }

    int d;

    if ((uint)(d = dstx - ctx.finalClipBox.getX1()) >= (uint)ctx.finalClipBox.getWidth())
    {
      if (d < 0)
      {
        if ((dstw += d) <= 0) return ERR_OK;
        dstx = 0;
        srcx = -d;
      }
      else
      {
        return ERR_OK;
      }
    }

    if ((uint)(d = dsty - ctx.finalClipBox.getY1()) >= (uint)ctx.finalClipBox.getHeight())
    {
      if (d < 0)
      {
        if ((dsth += d) <= 0) return ERR_OK;
        dsty = 0;
        srcy = -d;
      }
      else
      {
        return ERR_OK;
      }
    }

    if ((d = ctx.finalClipBox.x2 - dstx) < dstw) dstw = d;
    if ((d = ctx.finalClipBox.y2 - dsty) < dsth) dsth = d;

    IntRect dst(dstx, dsty, dstw, dsth);
    IntRect src(srcx, srcy, dstw, dsth);
    return _serializePaintImage(dst, image, src);
  }
  else
  {
    IntRect srcRect(0, 0, image.getWidth(), image.getHeight());

    if (irect)
    {
      // Caller must ensure that irect is valid!
      srcRect = *irect;
      if (!srcRect.isValid() ||
          srcRect.x < 0 ||
          srcRect.y < 0 ||
          srcRect.x + srcRect.w > image.getWidth() &&
          srcRect.y + srcRect.h > image.getHeight())
      {
        return ERR_RT_INVALID_ARGUMENT;
      }
    }

    DoublePoint pd((double)p.x, (double)p.y);
    return _serializePaintImageAffine(pd, image, srcRect);
  }
}

err_t RasterPaintEngine::drawImage(const DoublePoint& p, const Image& image, const IntRect* irect)
{
  RASTER_ENTER_PAINT_IMAGE(image);

  // Check whether we can use pixel-aligned blitter (speed optimization).
  if (ctx.hints.transformType <= RASTER_TRANSFORM_SUBPX)
  {
    int64_t xbig = (int64_t)((p.x + ctx.finalMatrix.tx) * 256.0);
    int64_t ybig = (int64_t)((p.y + ctx.finalMatrix.ty) * 256.0);

    int xf = (int)(xbig & 0xFF);
    int yf = (int)(ybig & 0xFF);

    // If resulting x and y isn't aligned then we can't use pixel-aligned blitter.
    if (xf == 0x00 && yf == 0x00)
    {
      int srcx = 0;
      int srcy = 0;
      int dstx = (int)(xbig >> 8);
      int dsty = (int)(ybig >> 8);
      int dstw;
      int dsth;

      if (irect == NULL)
      {
        dstw = image.getWidth();
        dsth = image.getHeight();
      }
      else
      {
        if (!irect->isValid()) return ERR_OK;

        srcx = irect->x;
        if (srcx < 0) return ERR_OK;
        srcy = irect->y;
        if (srcy < 0) return ERR_OK;

        dstw = Math::min(image.getWidth(), irect->getWidth());
        if (dstw == 0) return ERR_OK;
        dsth = Math::min(image.getHeight(), irect->getHeight());
        if (dsth == 0) return ERR_OK;
      }

      int d;

      if ((uint)(d = dstx - ctx.finalClipBox.x1) >= (uint)ctx.finalClipBox.getWidth())
      {
        if (d < 0)
        {
          if ((dstw += d) <= 0) return ERR_OK;
          dstx = 0;
          srcx = -d;
        }
        else
        {
          return ERR_OK;
        }
      }

      if ((uint)(d = dsty - ctx.finalClipBox.y1) >= (uint)ctx.finalClipBox.getHeight())
      {
        if (d < 0)
        {
          if ((dsth += d) <= 0) return ERR_OK;
          dsty = 0;
          srcy = -d;
        }
        else
        {
          return ERR_OK;
        }
      }

      if ((d = ctx.finalClipBox.x2 - dstx) < dstw) dstw = d;
      if ((d = ctx.finalClipBox.y2 - dsty) < dsth) dsth = d;

      IntRect dst(dstx, dsty, dstw, dsth);
      IntRect src(srcx, srcy, dstw, dsth);
      return _serializePaintImage(dst, image, src);
    }
  }

  {
    IntRect srcRect(0, 0, image.getWidth(), image.getHeight());

    if (irect)
    {
      // Caller must ensure that irect is valid!
      srcRect = *irect;
      if (!srcRect.isValid() ||
          srcRect.x < 0 ||
          srcRect.y < 0 ||
          srcRect.x + srcRect.w > image.getWidth() &&
          srcRect.y + srcRect.h > image.getHeight())
      {
        return ERR_RT_INVALID_ARGUMENT;
      }
    }

    return _serializePaintImageAffine(p, image, srcRect);
  }
}

err_t RasterPaintEngine::drawImage(const IntRect& rect, const Image& image, const IntRect* irect)
{
  // It's better to call drawImage() without stretching if rect size is equal to
  // irect size.
  if (irect)
  {
    if (rect.w == irect->w && rect.h == irect->h)
      return drawImage(IntPoint(rect.x, rect.y), image, irect);
  }
  else
  {
    if (rect.w == image.getWidth() && rect.h == image.getHeight())
      return drawImage(IntPoint(rect.x, rect.y), image, irect);
  }

  IntRect srcRect(0, 0, image.getWidth(), image.getHeight());
  if (irect)
  {
    if (!irect->isValid() ||
        irect->x < 0 ||
        irect->y < 0 ||
        irect->x + irect->w > srcRect.w ||
        irect->y + irect->h > srcRect.h)
    {
      return ERR_RT_INVALID_ARGUMENT;
    }
    srcRect = *irect;
  }

  if (FOG_LIKELY(ctx.hints.transformType == RASTER_TRANSFORM_EXACT))
  {
    // TODO PAINTER:
  }

  // If we can't use pixel aligned blitter then we create temporary affine
  // matrix and call image-affine serializer.

  // Calculate scaling factors.
  double scaleX = (double)rect.w / (double)srcRect.w;
  double scaleY = (double)rect.h / (double)srcRect.h;

  return stretchImageHelper(DoublePoint(rect.x, rect.y), image, srcRect, scaleX, scaleY);
}

err_t RasterPaintEngine::drawImage(const DoubleRect& rect, const Image& image, const IntRect* irect)
{
  // Check whether we can use pixel-aligned blitter (speed optimization).
  /*
  // TODO PAINTER:
  if (FOG_LIKELY(ctx.hints.transformType <= RASTER_TRANSFORM_SUBPX))
  {
    int w = (rect.w * 256.0);
    int h = (rect.h * 256.0);

    // If width is not aligned we can't use the pixel-aligned blitter.
    if ((w & 0xFF) == 0x00 && (h & 0xFF) == 0x00)
    {
      int x = (rect.x * 256.0);
      int y = (rect.y * 256.0);
    }
  }
  */

  IntRect srcRect(0, 0, image.getWidth(), image.getHeight());
  if (irect)
  {
    if (!irect->isValid() ||
        irect->x < 0 ||
        irect->y < 0 ||
        irect->x + irect->w > srcRect.w ||
        irect->y + irect->h > srcRect.h)
    {
      return ERR_RT_INVALID_ARGUMENT;
    }
    srcRect = *irect;
  }

  double scaleX = rect.w / (double)srcRect.w;
  double scaleY = rect.h / (double)srcRect.h;

  return stretchImageHelper(DoublePoint(rect.x, rect.y), image, srcRect, scaleX, scaleY);
}

err_t RasterPaintEngine::stretchImageHelper(
  const DoublePoint& pt, const Image& image, const IntRect& srcRect,
  double scaleX, double scaleY)
{
  DoubleMatrix savedMatrix(ctx.userMatrix);
  DoubleMatrix newMatrix(ctx.userMatrix);

  newMatrix.translate(pt.x, pt.y, MATRIX_PREPEND);
  newMatrix.scale(scaleX, scaleY, MATRIX_PREPEND);

  // I don't know what evil can happen there, but if it failed we should return.
  err_t err = setMatrix(newMatrix);
  if (err) return err;

  // This is the error value we want to return.
  err = _serializePaintImageAffine(DoublePoint(0.0, 0.0), image, srcRect);

  // Okay, restore the matrix and return.
  setMatrix(savedMatrix);
  return err;
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - Core]
// ============================================================================

err_t RasterPaintEngine::switchTo(const ImageBuffer& buffer, ImageData* d)
{
  // End with current painting (deleting states, etc...).
  _endWithPainting();

  // Sync, making all changes visible to current image / layer.
  flush(PAINTER_FLUSH_SYNC);

  // Destroy pattern context.
  if (ctx.pctx && ctx.pctx->initialized) ctx.pctx->destroy(ctx.pctx);

  // Setup primary layer.
  ctx.paintLayer.pixels = buffer.data;
  ctx.paintLayer.width = buffer.width;
  ctx.paintLayer.height = buffer.height;
  ctx.paintLayer.format = buffer.format;
  ctx.paintLayer.stride = buffer.stride;
  _setupPaintLayer(&ctx.paintLayer);

  // Setup clip / caps state to defaults.
  _setClipDefaults();
  _setCapsDefaults();

  return ERR_OK;
}

void RasterPaintEngine::_endWithPainting()
{
  // Delete all saved states.
  _restoreStates(0);

  // If current state is mask we need to reset it (to work state).
  if (ctx.finalClipType == RASTER_CLIP_MASK) _clipSetRegion(ctx.workRegion);

  if (!isSingleThreaded())
  {
    // TODO: Flush commands to discard everything?
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - Region]
// ============================================================================

err_t RasterPaintEngine::_updateWorkRegion()
{
  bool shouldResetMask = (ctx.finalClipType == RASTER_CLIP_MASK);

  IntBox bounds(0, 0, ctx.paintLayer.width, ctx.paintLayer.height);

  err_t err = ERR_OK;

  // Clear no-paint states, if resulting region won't be valid then these
  // states would be set back.
  ctx.state &= ~(RASTER_STATE_NO_PAINT_WORK_REGION  |
                 RASTER_STATE_NO_PAINT_FINAL_REGION |
                 RASTER_STATE_NO_PAINT_CLIP_MASK    );

  // Work origin is point that is added to painter translation matrix and it
  // ensured that raster will be always from [0, 0] -> [W-1, H-1] inclusive.
  IntPoint finalOrigin(ctx.metaOrigin + ctx.userOrigin);

  // We are really interested whether the origin was changed. See end of the
  // function.
  bool finalOriginChanged = (ctx.finalOrigin != finalOrigin);

  // Calculate final clip region.
  uint userBits = ((uint)(ctx.metaRegion.isInfinite()) << 1) |
                  ((uint)(ctx.userRegion.isInfinite()) << 0) ;

  switch (userBits)
  {
    // Meta region is 'finite', user region is 'finite'.
    case 0x00000000:
    {
      Region* inp = &ctx.userRegion;

      if (ctx.metaOrigin.x || ctx.metaOrigin.y)
      {
        err = Region::translateAndClip(tmpRegion0, ctx.userRegion, ctx.metaOrigin, bounds);
        if (err != ERR_OK) goto failed;

        inp = &tmpRegion0;
      }

      err = Region::intersectAndClip(ctx.workRegion, ctx.metaRegion, *inp, bounds);
      if (err != ERR_OK) goto failed;

      break;
    }

    // Meta region is 'finite', user region is 'infinite'.
    case 0x00000001:
    {
      err = Region::translateAndClip(ctx.workRegion, ctx.metaRegion, IntPoint(0, 0), bounds);
      if (err != ERR_OK) goto failed;
      break;
    }

    // Meta region is 'infinite', user region is 'finite'.
    case 0x00000002:
    {
      err = Region::translateAndClip(ctx.workRegion, ctx.userRegion, ctx.metaOrigin, bounds);
      if (err != ERR_OK) goto failed;
      break;
    }

    // Meta region is 'infinite', user region is 'infinite'.
    case 0x00000003:
    {
      err = ctx.workRegion.set(bounds);
      if (err != ERR_OK) goto failed;
      break;
    }
  }

  // Copy work region to final region (we are resetting, there is no active
  // user clipping at this time) and classify the clip type - the result can
  // be only:
  // - RASTER_CLIP_NULL - If the resulting region is empty.
  // - RASTER_CLIP_SIMPLE - If the resulting region is single rectangle.
  // - RASTER_CLIP_REGION - If there are more rectangles (complex clipping).
  {
    sysuint_t len = ctx.workRegion.getLength();

    ctx.workClipType = (len == 0)
      ? RASTER_CLIP_NULL
      : (len == 1) ? RASTER_CLIP_SIMPLE : RASTER_CLIP_REGION;
    ctx.workClipBox = ctx.workRegion.getExtents();

    // No clipping region -> set no-paint flags so painting will be disabled
    // until some other region will be used.
    if (len == 0)
    {
      ctx.state |= (RASTER_STATE_NO_PAINT_WORK_REGION  |
                    RASTER_STATE_NO_PAINT_FINAL_REGION );
    }
  }

  // The setMetaVars() or setUserVars() methods will reset the final region
  // (the region that can be modified by clip...() calls).
  ctx.finalOrigin = finalOrigin;
  ctx.finalRegion = ctx.workRegion;

  ctx.finalClipBox = ctx.workClipBox;
  ctx.finalClipType = ctx.workClipType;

  if (shouldResetMask) _serializeMaskReset();

  if (!isSingleThreaded())
  {
    ctx.state |= RASTER_STATE_PENDING_CLIP_REGION;
  }

  // Update transformation matrix if work origin was changed. It will also
  // update / translate pattern context if it's active.
  if (finalOriginChanged) _updateMatrix(true);
  return ERR_OK;

failed:
  return _failed(err);
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - Matrix]
// ============================================================================

err_t RasterPaintEngine::_updateMatrix(bool translationChangedOnly)
{
  // Apply meta matrix if used.
  if (ctx.hints.metaMatrixUsed)
  {
    ctx.finalMatrix = ctx.metaMatrix;
    ctx.finalMatrix.multiply(ctx.userMatrix, MATRIX_PREPEND);
    translationChangedOnly = false;
  }
  else
  {
    ctx.finalMatrix = ctx.userMatrix;
  }

  // Translate work matrix by work origin (meta origin + user origin).
  ctx.finalMatrix.tx += ctx.finalOrigin.x;
  ctx.finalMatrix.ty += ctx.finalOrigin.y;

  // Update translation in pixels.
  ctx.finalTranslate.set(Math::iround(ctx.finalMatrix.tx),
                         Math::iround(ctx.finalMatrix.ty));

  // If only matrix translation was changed, we can skip some expensive
  // calculations and checking.
  if (translationChangedOnly)
  {
    if (ctx.hints.transformType < RASTER_TRANSFORM_AFFINE)
    {
      bool isExact = ((Math::iround(ctx.userMatrix.tx * 256.0) & 0xFF) == 0x00) &
                     ((Math::iround(ctx.userMatrix.ty * 256.0) & 0xFF) == 0x00) ;
      ctx.hints.transformType = isExact ? RASTER_TRANSFORM_EXACT : RASTER_TRANSFORM_SUBPX;
    }
  }
  else
  {
    // Just check for matrix characteristics.
    bool isIdentity = (Math::feq(ctx.finalMatrix.sx , 1.0)) &
                      (Math::feq(ctx.finalMatrix.sy , 1.0)) &
                      (Math::feq(ctx.finalMatrix.shx, 0.0)) &
                      (Math::feq(ctx.finalMatrix.shy, 0.0)) ;
    bool isExact    = isIdentity && (
                      ((Math::iround(ctx.finalMatrix.tx * 256.0) & 0xFF) == 0x00) &
                      ((Math::iround(ctx.finalMatrix.ty * 256.0) & 0xFF) == 0x00) );

    // Use matrix characteristics to set correct transform type.
    uint transformType = isIdentity
      ? (isExact ? RASTER_TRANSFORM_EXACT : RASTER_TRANSFORM_SUBPX)
      : (RASTER_TRANSFORM_AFFINE);

    ctx.hints.transformType = (uint8_t)transformType;

    // sqrt(2.0)/2 ~~ 0.7071068
    if (transformType >= RASTER_TRANSFORM_AFFINE)
    {
      double x = ctx.finalMatrix.sx + ctx.finalMatrix.shx;
      double y = ctx.finalMatrix.sy + ctx.finalMatrix.shy;
      ctx.approximationScale = Math::sqrt(x * x + y * y) * 0.7071068;
    }
    else
    {
      ctx.approximationScale = 1.0;
    }
  }

  // Free pattern context, because transform was changed.
  if (ctx.pctx && ctx.pctx->initialized) _resetRasterPatternContext();

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - Defaults]
// ============================================================================

void RasterPaintEngine::_setClipDefaults()
{
  IntBox bounds(0, 0, (int)ctx.paintLayer.width, (int)ctx.paintLayer.height);

  // It's impossible to paint into no image. We are setting clip type to
  // RASTER_CLIP_SIMPLE so we need to make sure that it's really that type.
  FOG_ASSERT(bounds.getWidth() > 0 && bounds.getHeight() > 0);

  // Final matrix is translated by the finalOrigin, we are translating it back.
  // After this function is called it remains fully usable and valid.
  ctx.finalMatrix.tx -= ctx.finalOrigin.x;
  ctx.finalMatrix.ty -= ctx.finalOrigin.y;

  // Clear the regions and origins and set work and final region to the bounds.
  ctx.metaOrigin.clear();
  ctx.metaRegion = Region::infinite();

  ctx.userOrigin.clear();
  ctx.userRegion = Region::infinite();

  ctx.workClipType = RASTER_CLIP_SIMPLE;
  ctx.workClipBox = bounds;
  ctx.workRegion = bounds;

  ctx.finalOrigin.clear();
  ctx.finalClipType = RASTER_CLIP_SIMPLE;
  ctx.finalClipBox = bounds;
  ctx.finalRegion = ctx.workRegion;;

  ctx.state |= RASTER_STATE_PENDING_CLIP_REGION;
}

void RasterPaintEngine::_setCapsDefaults()
{
  if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN) ctx.pattern.destroy();
  if (ctx.pctx && ctx.pctx->initialized) _resetRasterPatternContext();

  ctx.ops.op = OPERATOR_SRC_OVER;
  ctx.ops.sourceType = PAINTER_SOURCE_ARGB;
  ctx.ops.sourceFormat = IMAGE_FORMAT_XRGB32;
  ctx.ops.alpha255 = 0xFF;

  ctx.hints.transformType = RASTER_TRANSFORM_EXACT;
  ctx.hints.fillRule = FILL_DEFAULT;
  ctx.hints.clipRule = CLIP_DEFAULT;
  ctx.hints.aaQuality = ANTI_ALIASING_SMOOTH;
  ctx.hints.imageInterpolation = IMAGE_INTERPOLATION_SMOOTH;
  ctx.hints.colorInterpolation = COLOR_INTERPOLATION_SMOOTH;
#if defined(FOG_FONT_MAC)
  ctx.hints.forceOutlineText = true;
#else
  ctx.hints.forceOutlineText = false;
#endif
  ctx.hints.lineIsSimple = true;

  ctx.funcs = rasterFuncs.getCompositeFuncs(OPERATOR_SRC_OVER, ctx.paintLayer.format);

  ctx.solid.argb = 0xFF000000;
  ctx.solid.prgb = 0xFF000000;

  ctx.alpha = 1.0f;

  ctx.strokeParams.reset();

  // Reset user matrix.
  ctx.userMatrix.reset();

  // We do not need to multiply userMatrix with metaMatrix, because we know
  // that user matrix is identity. We just copy that matrix and translate it
  // by the final origin.
  ctx.finalMatrix = ctx.metaMatrix;
  ctx.finalMatrix.tx += (double)ctx.finalOrigin.x;
  ctx.finalMatrix.ty += (double)ctx.finalOrigin.y;

  // TODO: WorkTranslate must be finalMatrix + finalOrigin!
  ctx.finalTranslate = ctx.finalOrigin;

  // TODO: Approximation scale must be calculated using finalMatrix!
  ctx.approximationScale = 1.0;
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - Pattern]
// ============================================================================

RasterPattern* RasterPaintEngine::_getRasterPatternContext()
{
  // Sanity, calling _getRasterPatternContext() for other than pattern
  // source is not allowed.
  FOG_ASSERT(ctx.ops.sourceType == PAINTER_SOURCE_PATTERN);

  RasterPattern* pctx = ctx.pctx;
  err_t err = ERR_OK;

  if (!pctx)
  {
    pctx = ctx.pctx = (RasterPattern*)blockAllocator.alloc(sizeof(RasterPattern));
    if (!pctx) return NULL;

    pctx->refCount.init(1);
    pctx->initialized = false;
  }

  if (!pctx->initialized)
  {
    const Pattern& pattern = ctx.pattern.instance();
    const DoubleMatrix& matrix = ctx.finalMatrix;

    switch (pattern.getType())
    {
      case PATTERN_TEXTURE:
        err = rasterFuncs.pattern.texture_init(pctx, pattern, matrix, ctx.hints.imageInterpolation);
        break;
      case PATTERN_LINEAR_GRADIENT:
        err = rasterFuncs.pattern.linear_gradient_init(pctx, pattern, matrix, ctx.hints.colorInterpolation);
        break;
      case PATTERN_RADIAL_GRADIENT:
        err = rasterFuncs.pattern.radial_gradient_init(pctx, pattern, matrix, ctx.hints.colorInterpolation);
        break;
      case PATTERN_CONICAL_GRADIENT:
        err = rasterFuncs.pattern.conical_gradient_init(pctx, pattern, matrix, ctx.hints.colorInterpolation);
        break;
      default:
        FOG_ASSERT_NOT_REACHED();
        return NULL;
    }
  }

  if (err)
  {
    fog_debug("Fog::RasterPaintEngine::_getRasterPatternContext() - Failed to get context (error %d)\n", err);
    return NULL;
  }

  // Be sure that pattern context is always marked as initialized. If this
  // assert fails then the problem is in RasterEngine pattern initializer.
  FOG_ASSERT(pctx->initialized);

  return pctx;
}

void RasterPaintEngine::_resetRasterPatternContext()
{
  RasterPattern* pctx = ctx.pctx;

  // Ignore non-initialized context.
  if (pctx && pctx->initialized)
  {
    // Each initialized context must have reference count larger or equal to one.
    // If we dereference it here it means that it's not used by another thread
    // so we can reuse allocated memory later.
    if (pctx->refCount.deref())
    {
      // Destroy context and reuse memory location later.
      pctx->destroy(ctx.pctx);
      pctx->refCount.init(1);
    }
    else
    {
      // Other thread will destroy it.
      ctx.pctx = NULL;
    }
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - Clipping]
// ============================================================================

err_t RasterPaintEngine::_clipOpNull(uint32_t clipOp)
{
  // Not allowed if we are here.
  FOG_ASSERT(!ctx.workRegion.isEmpty());

  switch (clipOp)
  {
    // COPY and INTERSECT operator nulls entire region.
    case CLIP_OP_REPLACE:
    case CLIP_OP_INTERSECT:
      return _clipSetNull();

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_RT_ASSERTION_FAILURE;
  }
}

err_t RasterPaintEngine::_clipOpInfinite(uint32_t clipOp)
{
  // Not allowed if we are here.
  FOG_ASSERT(!ctx.workRegion.isEmpty());

  switch (clipOp)
  {
    // REPLACE operator replaces current region by work one.
    case CLIP_OP_REPLACE:
      if (ctx.finalClipType == RASTER_CLIP_MASK) return _clipSetRegion(ctx.workRegion);

      // Hacky, but if these regions are the same then we bailout. This is
      // to prevent scheduling region-changed command.
      if (ctx.finalRegion._d == ctx.workRegion._d) return ERR_OK;

      ctx.finalRegion = ctx.workRegion;
      ctx.finalClipBox = ctx.workClipBox;
      ctx.finalClipType = ctx.workClipType;

      ctx.state |= RASTER_STATE_PENDING_CLIP_REGION;
      return ERR_OK;

    // INTERSECT is NOP.
    case CLIP_OP_INTERSECT:
      return ERR_OK;

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_RT_ASSERTION_FAILURE;
  }
}

err_t RasterPaintEngine::_clipOpBox(const IntBox& box, uint32_t clipOp)
{
  // Not allowed if we are here.
  FOG_ASSERT(!ctx.workRegion.isEmpty());

  // Intersect work clip box with a given box and set inside if they intersects.
  IntBox ibox;
  bool inside = IntBox::intersect(ibox, box, ctx.workClipBox);

  // Special cases.
  if (!inside) return _clipOpNull(clipOp);
  if (ctx.workClipBox == ibox) return _clipOpInfinite(clipOp);

  // Antialiased clipping using clip-mask.
  if (FOG_UNLIKELY(ctx.finalClipType == RASTER_CLIP_MASK))
  {
    if (clipOp == CLIP_OP_REPLACE) goto copy;

    if (ctx.workClipType == RASTER_CLIP_SIMPLE)
    {
      return _serializeMaskBox(ibox, clipOp);
    }
    else
    {
      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion1, ctx.workRegion, box, REGION_OP_INTERSECT));
      return _serializeMaskRegion(tmpRegion1, clipOp);
    }
  }

  // Standard clipping using box or region.
  switch (clipOp)
  {
    case CLIP_OP_REPLACE:
copy:
      if (ctx.workClipType == RASTER_CLIP_SIMPLE)
      {
        return _clipSetBox(ibox);
      }

      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion1, ctx.workRegion, box, REGION_OP_INTERSECT));
      return _clipSetRegion(tmpRegion1);

    case CLIP_OP_INTERSECT:
      if (ctx.finalClipType == RASTER_CLIP_SIMPLE)
      {
        return IntBox::intersect(ibox, ibox, ctx.finalClipBox)
          ? _clipSetBox(ibox)
          : _clipSetNull();
      }

      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion1, ctx.finalRegion, ibox, REGION_OP_INTERSECT));
      return _clipSetRegion(tmpRegion1);

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_RT_ASSERTION_FAILURE;
  }
}

err_t RasterPaintEngine::_clipOpRegion(const Region& region, uint32_t clipOp)
{
  // Not allowed here.
  FOG_ASSERT(!region.isInfinite());

  // Ensure that we are not wasting time with combining simple regions.
  {
    sysuint_t len = region.getLength();
    if (len == 0) return _clipOpNull(clipOp);
    if (len == 1) return _clipOpBox(region.getExtents(), clipOp);
  }

  // Intersect work clip box with a given box and set inside if they intersects.
  IntBox ibox;
  bool inside = IntBox::intersect(ibox, region.getExtents(), ctx.workClipBox);

  // Special cases.
  if (!inside) return _clipOpNull(clipOp);

  // Antialiased clipping using clip-mask.
  if (FOG_UNLIKELY(ctx.finalClipType == RASTER_CLIP_MASK))
  {
    if (clipOp == CLIP_OP_REPLACE) goto copy;

    if (ctx.workClipType == RASTER_CLIP_SIMPLE)
    {
      return _serializeMaskRegion(region, clipOp);
    }
    else
    {
      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion1, ctx.workRegion, region, REGION_OP_INTERSECT));
      return _serializeMaskRegion(tmpRegion1, clipOp);
    }
  }

  // Standard clipping using box or region.
  switch (clipOp)
  {
    case CLIP_OP_REPLACE:
copy:
      if (ctx.workClipType == RASTER_CLIP_SIMPLE && ctx.workClipBox.subsumes(region.getExtents()))
      {
        return _clipSetRegion(region);
      }

      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion1, ctx.workRegion, region, REGION_OP_INTERSECT));
      return _clipSetRegion(tmpRegion1);

    case CLIP_OP_INTERSECT:
      FOG_RETURN_ON_ERROR(Region::combine(tmpRegion1, ctx.finalRegion, region, REGION_OP_INTERSECT));
      return _clipSetRegion(tmpRegion1);

    default:
      FOG_ASSERT_NOT_REACHED();
      return ERR_RT_ASSERTION_FAILURE;
  }
}

err_t RasterPaintEngine::_clipSetNull()
{
  bool shouldResetMask = (ctx.finalClipType == RASTER_CLIP_MASK);

  ctx.state |= RASTER_STATE_NO_PAINT_FINAL_REGION;

  ctx.finalClipType = RASTER_CLIP_NULL;
  ctx.finalClipBox.clear();
  ctx.finalRegion.clear();

  if (shouldResetMask)
    return _serializeMaskReset();
  else
    return ERR_OK;
}

err_t RasterPaintEngine::_clipSetBox(const IntBox& box)
{
  bool shouldResetMask = (ctx.finalClipType == RASTER_CLIP_MASK);

  // Box must be valid.
  FOG_ASSERT(box.isValid());

  ctx.state &= ~(RASTER_STATE_NO_PAINT_FINAL_REGION |
                 RASTER_STATE_NO_PAINT_CLIP_MASK);

  ctx.finalClipType = RASTER_CLIP_SIMPLE;
  ctx.finalClipBox = box;

  // Share workRegion and finalRegion if they are the same.
  if (ctx.workClipBox == box)
    ctx.finalRegion = ctx.workRegion;
  else
    ctx.finalRegion = box;

  if (shouldResetMask)
    return _serializeMaskReset();
  else
    return ERR_OK;
}

err_t RasterPaintEngine::_clipSetRegion(const Region& region)
{
  bool shouldResetMask = (ctx.finalClipType == RASTER_CLIP_MASK);

  // This method is normally called after region combine operation so the
  // region can be empty or simple. Detect these special cases.
  sysuint_t len = region.getLength();
  if (len == 0) return _clipSetNull();
  if (len == 1) return _clipSetBox(region.getExtents());

  ctx.state &= ~(RASTER_STATE_NO_PAINT_FINAL_REGION |
                 RASTER_STATE_NO_PAINT_CLIP_MASK);

  ctx.finalClipType = RASTER_CLIP_REGION;
  ctx.finalClipBox = region.getExtents();
  ctx.finalRegion = region;

  if (shouldResetMask)
    return _serializeMaskReset();
  else
    return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - State]
// ============================================================================

void RasterPaintEngine::_restoreStates(uint32_t layerId)
{
  while (states.getLength())
  {
    RasterPaintState* state = states.top();

    if (state->layerId >= layerId)
    {
      state->restore(this);
      _deleteState(states.takeLast());
    }
    else
    {
      break;
    }
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - Layer]
// ============================================================================

void RasterPaintEngine::_setupPaintLayer(RasterPaintLayer* layer)
{
  switch (layer->format)
  {
    case IMAGE_FORMAT_PRGB32:
    case IMAGE_FORMAT_XRGB32:
      // Direct rendering in all cases, nothing to setup.
      layer->type = RASTER_LAYER_DIRECT32;
      layer->bytesPerPixel = 4;

      layer->secondaryFormat = IMAGE_FORMAT_NULL;
      layer->toSecondaryFull = NULL;
      layer->toSecondarySpan = NULL;
      layer->fromSecondaryFull = NULL;
      layer->fromSecondarySpan = NULL;
      break;

    case IMAGE_FORMAT_ARGB32:
      // Direct rendering if possible, bail to PRGB32 if ARGB32 is not
      // implemented for particular task.
      layer->type = RASTER_LAYER_DIRECT32;
      layer->bytesPerPixel = 4;

      layer->secondaryFormat = IMAGE_FORMAT_NULL;
      layer->toSecondaryFull = rasterFuncs.dib.convert[IMAGE_FORMAT_PRGB32][IMAGE_FORMAT_ARGB32];
      layer->toSecondarySpan = rasterFuncs.dib.prgb32_from_argb32_span;
      layer->fromSecondaryFull = rasterFuncs.dib.convert[IMAGE_FORMAT_ARGB32][IMAGE_FORMAT_PRGB32];
      layer->fromSecondarySpan = rasterFuncs.dib.argb32_from_prgb32_span;
      break;

    case IMAGE_FORMAT_A8:
      // Direct rendering in all cases, nothing to setup.
      layer->type = RASTER_LAYER_DIRECT8;
      layer->bytesPerPixel = 1;

      layer->secondaryFormat = IMAGE_FORMAT_NULL;
      layer->toSecondaryFull = NULL;
      layer->toSecondarySpan = NULL;
      layer->fromSecondaryFull = NULL;
      layer->fromSecondarySpan = NULL;
      break;

    case IMAGE_FORMAT_I8:
      // I8 format is not supported by the Painter/RasterPaintEngine and must 
      // be catched earlier.
    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Serializers - Painting]
// ============================================================================

err_t RasterPaintEngine::_serializePaintRegion(const Region& region)
{
  if (FOG_UNLIKELY(region.getLength() == 0)) return ERR_OK;

  RASTER_SERIALIZE_ENSURE_PATTERN();

  // Singlethreaded.
  if (isSingleThreaded())
  {
    _doPaintBoxes_st(&ctx, region.getData(), region.getLength());
  }
  // Multithreaded.
  else
  {
    _beforeNewAction();

    RasterPaintCmdRegion* cmd = _createCommand<RasterPaintCmdRegion>();
    if (FOG_UNLIKELY(cmd == NULL)) return ERR_RT_OUT_OF_MEMORY;

    cmd->region.init(region);
    cmd->_initPaint(&ctx);
    _postCommand(cmd);
  }

  return ERR_OK;
}

err_t RasterPaintEngine::_serializePaintBoxes(const IntBox* box, sysuint_t count)
{
  RASTER_SERIALIZE_ENSURE_PATTERN();

  // Singlethreaded.
  if (isSingleThreaded())
  {
    _doPaintBoxes_st(&ctx, box, count);
  }
  // Multithreaded.
  else
  {
    while (count > 0)
    {
      _beforeNewAction();

      sysuint_t n = Math::min<sysuint_t>(count, 32);
      RasterPaintCmdBoxes* cmd = _createCommand<RasterPaintCmdBoxes>(sizeof(RasterPaintCmdBoxes) - sizeof(IntBox) + n * sizeof(IntBox));
      if (FOG_UNLIKELY(cmd == NULL)) return ERR_RT_OUT_OF_MEMORY;

      cmd->count = n;
      for (sysuint_t j = 0; j < n; j++) cmd->boxes[j] = box[j];

      cmd->_initPaint(&ctx);
      _postCommand(cmd);

      count -= n;
      box += n;
    }
  }

  return ERR_OK;
}

err_t RasterPaintEngine::_serializePaintImage(const IntRect& dst, const Image& image, const IntRect& src)
{
  // Singlethreaded.
  if (isSingleThreaded())
  {
    _doPaintImage_st(&ctx, dst, image, src);
  }
  // Multithreaded.
  else
  {
    _beforeNewAction();

    RasterPaintCmdImage* cmd = _createCommand<RasterPaintCmdImage>();
    if (FOG_UNLIKELY(cmd == NULL)) return ERR_RT_OUT_OF_MEMORY;

    cmd->dst = dst;
    cmd->src = src;
    cmd->image.init(image);
    cmd->_initBlit(&ctx);
    _postCommand(cmd);
  }

  return ERR_OK;
}

err_t RasterPaintEngine::_serializePaintImageAffine(const DoublePoint& pt, const Image& image, const IntRect& irect)
{
  FOG_ASSERT(!image.isEmpty());

  // Create new transformation matrix (based on current matrix and point
  // where the image should be drawn).
  const DoubleMatrix& tr = ctx.finalMatrix;
  DoublePoint pt_(pt);
  tr.transformPoint(&pt_.x, &pt_.y);
  DoubleMatrix matrix(tr.sx, tr.shy, tr.shx, tr.sy, pt_.x, pt_.y);

  // Make the path.
  curPath.clear();
  curPath.addRect(DoubleRect(pt.x, pt.y, (double)irect.w, (double)irect.h));

  // Singlethreaded.
  if (isSingleThreaded())
  {
    if (_doRasterizePath_st(curPath, ctx.finalClipBox, FILL_EVEN_ODD, false))
    {
      // Save current pattern context (it will be replaced by context created
      // for the image).
      uint32_t oldSourceType = ctx.ops.sourceType;
      RasterPattern* oldPctx = ctx.pctx;

      // Create new pattern context (based on the image).
      RasterPattern imagectx;
      imagectx.initialized = false;
      rasterFuncs.pattern.texture_init_blit(&imagectx,
        image, irect, matrix, PATTERN_SPREAD_PAD, ctx.hints.imageInterpolation);

      ctx.ops.sourceType = PAINTER_SOURCE_PATTERN;
      ctx.pctx = &imagectx;

      // Render path using specific pattern context.
      _doPaintPath_st(&ctx, &rasterizer);

      // Destroy pattern context.
      imagectx.destroy(&imagectx);

      // Restore old pattern context and source type.
      ctx.ops.sourceType = oldSourceType;
      ctx.pctx = oldPctx;
    }
  }
  // Multithreaded.
  else
  {
    _beforeNewAction();

    // This is a bit code duplication (see singlethreaded mode), but it quite
    // different. Here we are not interested about rectangle rasterization and
    // replacing pattern context in ctx, but we instead serialize path command
    // with new pattern context (current context will not be replaced at all).

    // Create new pattern context (based on the image).
    RasterPattern* imagectx =
      reinterpret_cast<RasterPattern*>(
        blockAllocator.alloc(sizeof(RasterPattern)));
    if (!imagectx) return ERR_RT_OUT_OF_MEMORY;

    imagectx->initialized = false;
    rasterFuncs.pattern.texture_init_blit(
      imagectx, image, irect, matrix, PATTERN_SPREAD_PAD, ctx.hints.imageInterpolation);
    imagectx->refCount.init(1);

    RasterPaintCmdPath* cmd = _createCommand<RasterPaintCmdPath>();
    if (FOG_UNLIKELY(cmd == NULL))
    {
      imagectx->destroy(imagectx);
      blockAllocator.free(imagectx);
      return ERR_RT_OUT_OF_MEMORY;
    }

    RasterPaintCalcFillPath* clc = _createCalc<RasterPaintCalcFillPath>();
    if (FOG_UNLIKELY(clc == NULL)) { cmd->release(&ctx); return ERR_RT_OUT_OF_MEMORY; }

    clc->relatedTo = cmd;
    clc->path.init(curPath);
    clc->matrix.init(ctx.finalMatrix);
    clc->clipBox = ctx.finalClipBox;
    clc->transformType = ctx.hints.transformType;
    clc->fillRule = FILL_EVEN_ODD;
    clc->approximationScale = ctx.approximationScale;

    cmd->status.init(RASTER_COMMAND_WAIT);
    cmd->calculation = clc;

    // Init paint specialized for affine blit.
    cmd->ops.data = ctx.ops.data;
    cmd->ops.sourceType = PAINTER_SOURCE_PATTERN;
    cmd->pctx = imagectx;

    _postCommand(cmd, clc);
  }

  return ERR_OK;
}

err_t RasterPaintEngine::_serializePaintGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip)
{
  IntBox boundingBox = ctx.finalClipBox;

  if (clip)
  {
    IntBox::intersect(boundingBox, boundingBox, IntBox(*clip));
    if (!boundingBox.isValid()) return ERR_OK;
  }

  RASTER_SERIALIZE_ENSURE_PATTERN();

  // Singlethreaded.
  if (isSingleThreaded())
  {
    _doPaintGlyphSet_st(&ctx, pt, glyphSet, boundingBox);
  }
  // Multithreaded.
  else
  {
    _beforeNewAction();

    RasterPaintCmdGlyphSet* cmd = _createCommand<RasterPaintCmdGlyphSet>();
    if (FOG_UNLIKELY(cmd == NULL)) return ERR_RT_OUT_OF_MEMORY;

    cmd->pt = pt;
    cmd->boundingBox = boundingBox;
    cmd->glyphSet.init(glyphSet);
    cmd->_initPaint(&ctx);
    _postCommand(cmd);
  }

  return ERR_OK;
}

err_t RasterPaintEngine::_serializePaintPath(const DoublePath& path, bool stroke)
{
  RASTER_SERIALIZE_ENSURE_PATTERN();

  // Singlethreaded.
  if (isSingleThreaded())
  {
    if (_doRasterizePath_st(path, ctx.finalClipBox, ctx.hints.fillRule, stroke))
      _doPaintPath_st(&ctx, &rasterizer);
  }
  // Multithreaded.
  else
  {
    _beforeNewAction();

    RasterPaintCmdPath* cmd = _createCommand<RasterPaintCmdPath>();
    if (FOG_UNLIKELY(cmd == NULL)) return ERR_RT_OUT_OF_MEMORY;

    cmd->status.init(RASTER_COMMAND_WAIT);

    if (stroke)
    {
      RasterPaintCalcStrokePath* clc = _createCalc<RasterPaintCalcStrokePath>();
      if (!clc) { cmd->release(&ctx); return ERR_RT_OUT_OF_MEMORY; }

      clc->relatedTo = cmd;
      clc->path.init(path);
      clc->matrix.init(ctx.finalMatrix);
      clc->clipBox = ctx.finalClipBox;
      clc->transformType = ctx.hints.transformType;
      clc->stroker.initCustom2(ctx.strokeParams, ctx.approximationScale);

      cmd->calculation = clc;
      cmd->_initPaint(&ctx);
      _postCommand(cmd, clc);
    }
    else
    {
      RasterPaintCalcFillPath* clc = _createCalc<RasterPaintCalcFillPath>();
      if (!clc) { cmd->release(&ctx); return ERR_RT_OUT_OF_MEMORY; }

      clc->relatedTo = cmd;
      clc->path.init(path);
      clc->matrix.init(ctx.finalMatrix);
      clc->clipBox = ctx.finalClipBox;
      clc->transformType = ctx.hints.transformType;
      clc->fillRule = ctx.hints.fillRule;
      clc->approximationScale = ctx.approximationScale;

      cmd->calculation = clc;
      cmd->_initPaint(&ctx);
      _postCommand(cmd, clc);
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Serializers - Clipping]
// ============================================================================

#define RASTER_SERIALIZE_ENSURE_MASK_BOUNDS(__bounds__) \
  FOG_BEGIN_MACRO \
    /* Need to switch to RASTER_CLIP_MASK type of clipping. If we are using */ \
    /* this type of clipping already then we only need to check if the clip */ \
    /* was saved.                                                           */ \
    if (FOG_UNLIKELY(ctx.finalClipType != RASTER_CLIP_MASK)) \
    { \
      if (clipOp == CLIP_OP_REPLACE) \
      { \
        /* COPY - Do not need to convert current final region to mask. */ \
        FOG_RETURN_ON_ERROR(_serializeMaskNew()); \
      } \
      else /* if (clipOp == CLIP_OP_INTERSECT) */ \
      { \
        FOG_RETURN_ON_ERROR(_serializeMaskConvert(__bounds__)); \
      } \
    } \
    /* If mask was saved then we need to serialize 'clip-save' command.     */ \
    else if (FOG_UNLIKELY(ctx.maskSavedCounter > 0)) \
    { \
      _serializeMaskSave(); \
      FOG_ASSERT(ctx.maskSavedCounter == 0); \
    } \
  FOG_END_MACRO

#define RASTER_SERIALIZE_ENSURE_MASK_PATH() \
  FOG_BEGIN_MACRO \
    /* Need to switch to RASTER_CLIP_MASK type of clipping. If we are using */ \
    /* this type of clipping already then we only need to check if the clip */ \
    /* was saved.                                                           */ \
    if (FOG_UNLIKELY(ctx.finalClipType != RASTER_CLIP_MASK)) \
    { \
      if (clipOp == CLIP_OP_REPLACE) \
      { \
        /* COPY - Do not need to convert current final region to mask. */ \
        FOG_RETURN_ON_ERROR(_serializeMaskNew()); \
      } \
      else /* if (clipOp == CLIP_OP_INTERSECT) */ \
      { \
        if (ctx.finalClipType == RASTER_CLIP_NULL) return ERR_OK; \
        FOG_RETURN_ON_ERROR(_serializeMaskConvert(ctx.finalRegion.getExtents())); \
      } \
    } \
    /* If mask was saved then we need to serialize 'clip-save' command.     */ \
    else if (FOG_UNLIKELY(ctx.maskSavedCounter > 0)) \
    { \
      _serializeMaskSave(); \
      FOG_ASSERT(ctx.maskSavedCounter == 0); \
    } \
  FOG_END_MACRO

RasterClipMask* RasterPaintEngine::_allocClipMask(const IntBox& workBox)
{
  uint maxh = (uint)workBox.getHeight();

  RasterClipMask* mask;
  uint8_t* p;

  // Allocate spans data using BlockMemoryAllocator if the size is small.
  if (maxh <= 64)
  {
    mask = reinterpret_cast<RasterClipMask*>(
      blockAllocator.alloc(sizeof(RasterClipMask) + maxh * sizeof(void*)));
    if (FOG_UNLIKELY(mask == NULL)) return NULL;

    p = reinterpret_cast<uint8_t*>(mask) + sizeof(RasterClipMask);
  }
  // Otherwise it's not big performance issue to allocate that buffer
  // using dynamic memory allocator.
  else
  {
    mask = reinterpret_cast<RasterClipMask*>(
      blockAllocator.alloc(sizeof(RasterClipMask)));
    if (FOG_UNLIKELY(mask == NULL)) return NULL;

    p = reinterpret_cast<uint8_t*>(Memory::alloc(maxh * sizeof(void*)));
    if (FOG_UNLIKELY(p == NULL))
    {
      blockAllocator.free(mask);
      return NULL;
    }
  }

  mask->refCount.init(1);
  mask->id = ++ctx.maskId;
  mask->workBox = workBox;
  mask->width = (uint)(workBox.getWidth());
  mask->height = maxh;
  mask->spans = reinterpret_cast<Span8**>(p);

  return mask;
}

void RasterPaintEngine::_destroyClipMask(RasterClipMask* mask)
{
  // Dynamically allocated memory must be freed by the same allocator.
  if (reinterpret_cast<uint8_t*>(mask) + sizeof(RasterClipMask) !=
      reinterpret_cast<uint8_t*>(mask->spans))
  {
    Memory::free(mask->spans);
  }

  // And engine memory too.
  blockAllocator.free(mask);
}

err_t RasterPaintEngine::_serializeMaskNew()
{
  FOG_ASSERT(ctx.finalClipType != RASTER_CLIP_MASK);

  RasterClipMask* mask = _allocClipMask(ctx.workClipBox);
  if (FOG_UNLIKELY(mask == NULL)) return ERR_RT_OUT_OF_MEMORY;

  // Set clip type to RASTER_CLIP_MASK and disable final region - shouldn't
  // be accessed when using RASTER_CLIP_MASK, only ctx.finalClipBox can be.
  ctx.finalClipType = RASTER_CLIP_MASK;
  ctx.finalClipBox = ctx.workClipBox;
  ctx.finalRegion.clear();

  ctx.mask = mask;

  if (isSingleThreaded())
  {
    // Set clip mask instance (currently it's empty, the next operation should
    // initialize it).
    ctx.maskY1 = -1;
    ctx.maskY2 = -1;

    ctx.maskSpanAdj = mask->spans - ctx.workClipBox.y1;
    ctx.maskSnapshot = ctx.makeSnapshot();
    ctx.clearSpanPools();

    return ERR_OK;
  }
  else
  {
    // We have 1 bit, there is no room for two pending masks, it's failure
    // if it is set.
    FOG_ASSERT((ctx.state & RASTER_STATE_PENDING_CLIP_MASK_CREATED) == 0);

    // Set pending mask flag.
    ctx.state |= RASTER_STATE_PENDING_CLIP_MASK_CREATED;

    return ERR_OK;
  }
}

err_t RasterPaintEngine::_serializeMaskReset()
{
  RasterClipMask* mask = ctx.mask;
  ctx.mask = NULL;

  // We are only called if mask really exists.
  FOG_ASSERT(mask != NULL);

  if (isSingleThreaded())
  {
    // Be safe.
    ctx.maskY1 = -1;
    ctx.maskY2 = -1;
    ctx.maskSpanAdj = NULL;

    // Dereference it (and maybe destroy it).
    if (mask->refCount.deref())
    {
      _destroyClipMask(mask);
    }
  }
  else
  {
    // TODO: Multithreaded clipping not implemented yet.

    // This mask was saved so we don't need to destroy it, we just abandon it.
    if (ctx.maskSavedCounter)
    {
      ctx.state |= RASTER_STATE_PENDING_CLIP_REGION;
    }

    if (mask->refCount.deref())
    {
      _destroyClipMask(mask);
    }
  }

  ctx.maskSavedCounter = 0;

  return ERR_OK;
}

err_t RasterPaintEngine::_serializeMaskConvert(const IntBox& bounds)
{
  // TODO: Use bounds.

  FOG_ASSERT(ctx.finalClipType != RASTER_CLIP_MASK);
  FOG_ASSERT(ctx.finalRegion.getLength() > 0);

  RasterClipMask* mask = _allocClipMask(ctx.workClipBox);
  if (FOG_UNLIKELY(mask == NULL)) return ERR_RT_OUT_OF_MEMORY;

  if (isSingleThreaded())
  {
    // Set clip type to RASTER_CLIP_MASK.
    ctx.finalClipType = RASTER_CLIP_MASK;
    ctx.finalClipBox = ctx.workClipBox;

    // Convert clip mask.
    ctx.mask = mask;

    ctx.maskY1 = -1;
    ctx.maskY2 = -1;

    ctx.maskSpanAdj = mask->spans - ctx.workClipBox.y1;
    ctx.maskSnapshot = ctx.makeSnapshot();
    ctx.clearSpanPools();

    _doMaskConvert_st(&ctx, ctx.finalRegion.getData(), ctx.finalRegion.getLength());

    // Final region shouldn't be accessed if the clip type is RASTER_CLIP_MASK.
    ctx.finalRegion.clear();
    return ERR_OK;
  }
  else
  {
    // TODO: Multithreaded clipping not implemented yet.
  }
}

err_t RasterPaintEngine::_serializeMaskSave()
{
  FOG_ASSERT(ctx.finalClipType == RASTER_CLIP_MASK);

  RasterClipMask* mask = _allocClipMask(ctx.workClipBox);
  if (FOG_UNLIKELY(mask == NULL)) return ERR_RT_OUT_OF_MEMORY;

  if (isSingleThreaded())
  {
    Span8** oldSpanAdj = ctx.maskSpanAdj;
    RasterClipSnapshot* snapshot = ctx.makeSnapshot();

    ctx.mask = mask;
    ctx.maskSavedCounter = 0;

    // Set clip mask instance (currently it's empty, the next operation should
    // initialize it).
    ctx.maskSpanAdj = mask->spans - ctx.workClipBox.y1;
    ctx.maskSnapshot = snapshot;
    ctx.clearSpanPools();

    // Make weak copy.
    for (int y = ctx.maskY1; y < ctx.maskY2; y++)
    {
      Span8* span = oldSpanAdj[y];
      ctx.maskSpanAdj[y] = span ? (Span8*)((sysuint_t)span | 0x1) : NULL;
    }

    return ERR_OK;
  }
  else
  {
    // TODO: Multithreaded clipping.
    return ERR_OK;
  }
}

err_t RasterPaintEngine::_serializeMaskRestore(RasterClipMask* toMask)
{
  if (ctx.mask != NULL && ctx.mask->refCount.deref())
  {
    _destroyClipMask(ctx.mask);
  }

  if (isSingleThreaded())
  {
    if (toMask != NULL)
    {
      RasterClipSnapshot* snapshot = ctx.findSnapshot(toMask);
      ctx.restoreSnapshot(snapshot);
      ctx.mask = toMask;
    }

    return ERR_OK;
  }
  else
  {
    // TODO: Multithreaded clipping.
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

err_t RasterPaintEngine::_serializeMaskBox(const IntBox& box, uint32_t clipOp)
{
  RASTER_SERIALIZE_ENSURE_MASK_BOUNDS(box);

  if (isSingleThreaded())
  {
    _doMaskBoxes_st(&ctx, &box, 1, clipOp);
    return ERR_OK;
  }
  else
  {
    // TODO: Multithreaded clipping not implemented yet.
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

err_t RasterPaintEngine::_serializeMaskRegion(const Region& region, uint32_t clipOp)
{
  RASTER_SERIALIZE_ENSURE_MASK_BOUNDS(region.getExtents());

  if (isSingleThreaded())
  {
    _doMaskBoxes_st(&ctx, region.getData(), region.getLength(), clipOp);
    return ERR_OK;
  }
  else
  {
    // TODO: Multithreaded clipping not implemented yet.
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

err_t RasterPaintEngine::_serializeMaskPath(const DoublePath& path, bool stroke, uint32_t clipOp)
{
  RASTER_SERIALIZE_ENSURE_MASK_PATH();

  if (isSingleThreaded())
  {
    if (_doRasterizePath_st(path, ctx.workClipBox, ctx.hints.clipRule, stroke))
    {
      _doMaskPath_st(&ctx, &rasterizer, clipOp);
      return ERR_OK;
    }
    else
    {
      ctx.state &= ~RASTER_STATE_NO_PAINT_MASK;
      ctx.state |= RASTER_STATE_NO_PAINT_FINAL_REGION;

      ctx.finalClipType = RASTER_CLIP_NULL;
      ctx.finalClipBox.clear();
      ctx.finalRegion.clear();

      return _serializeMaskReset();
    }
  }
  else
  {
    // TODO: Multithreaded clipping not implemented yet.
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Serializers - Helpers]
// ============================================================================

void RasterPaintEngine::_postCommand(RasterPaintCmd* cmd)
{
#if defined(FOG_DEBUG_RASTER_COMMANDS)
  fog_debug("Fog::Painter::_postCommand() - posting command...");
#endif // FOG_DEBUG_RASTER_COMMANDS

  sysuint_t pos = workerManager->cmdPosition;
  workerManager->cmdData[pos] = cmd;
  workerManager->cmdPosition++;

  if ((pos & 31) == 31 && workerManager->waitingWorkers.get() > 0)
  {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter::_postCommand() - waking up...");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
    AutoLock locked(workerManager->lock);
    workerManager->wakeUpScheduled(NULL);
  }
}

void RasterPaintEngine::_postCommand(RasterPaintCmd* cmd, RasterPaintCalc* clc)
{
  sysuint_t pos = workerManager->calcPosition;
  workerManager->calcData[pos] = clc;
  workerManager->calcPosition++;

  _postCommand(cmd);
}

void RasterPaintEngine::_postPending()
{
#if defined(FOG_DEBUG_RASTER_COMMANDS)
  fog_debug("Fog::Painter::_postPending() - posting pending commands...");
#endif // FOG_DEBUG_RASTER_COMMANDS

  // Don't call me if there no pending task!
  uint32_t pending = ctx.state;
  FOG_ASSERT((pending & RASTER_STATE_PENDING_MASK) != 0);

  // Clear pending flags.
  ctx.state &= ~(RASTER_STATE_PENDING_MASK);

  if (pending & RASTER_STATE_PENDING_CLIP_REGION)
  {
    RasterPaintCmdUpdateRegion* cmd = _createCommand<RasterPaintCmdUpdateRegion>();
    if (FOG_UNLIKELY(cmd == NULL)) { _failed(ERR_RT_OUT_OF_MEMORY); return; }

    cmd->workClipType = ctx.workClipType;
    cmd->finalClipType = ctx.finalClipType;

    cmd->finalOrigin = ctx.finalOrigin;

    cmd->workRegion.init(ctx.workRegion);
    cmd->finalRegion.init(ctx.finalRegion);

    _postCommand(cmd);
  }

  if (pending & RASTER_STATE_PENDING_CLIP_MASK_CREATED)
  {
    // TODO
  }

  // TODO: What about others?
}

// ============================================================================
// [Fog::RasterPaintEngine - Fatal]
// ============================================================================

err_t RasterPaintEngine::_failed(err_t err)
{
  // Mark this context as fatal, all painting will be ignored until the end.
  // Set also other no-paint flags related to clipping.
  ctx.state |= RASTER_STATE_FATAL_ERROR          |
               RASTER_STATE_NO_PAINT_WORK_REGION |
               RASTER_STATE_NO_PAINT_FINAL_REGION;

  // All regions will be cleared, we don't want them after returned.
  ctx.metaOrigin.clear();
  ctx.metaRegion.clear();

  ctx.userOrigin.clear();
  ctx.userRegion.clear();

  ctx.finalTranslate.clear();
  ctx.workRegion.clear();

  ctx.finalOrigin.clear();
  ctx.finalRegion.clear();

  // Also set clipping type to RASTER_CLIP_NULL.
  ctx.workClipType = RASTER_CLIP_NULL;
  ctx.finalClipType = RASTER_CLIP_NULL;

  // Clear clip boxes.
  ctx.workClipBox.clear();
  ctx.finalClipBox.clear();

  return err;
}

// ============================================================================
// [Fog::RasterPaintEngine - Rasterizer]
// ============================================================================

bool RasterPaintEngine::_doRasterizePath_st(const DoublePath& path, const IntBox& clipBox, uint32_t fillRule, bool stroke)
{
  // Can be only used by single-threaded engine.
  FOG_ASSERT(isSingleThreaded());

  // Use transformation matrix only if it makes sense.
  const DoubleMatrix* matrix = (ctx.hints.transformType >= RASTER_TRANSFORM_AFFINE)
    ? matrix = &ctx.finalMatrix
    : NULL;

  rasterizer.reset();

  const DoublePath* p = &path;
  bool noTransform = (
    ctx.hints.transformType == RASTER_TRANSFORM_EXACT &&
    ctx.finalTranslate.x == 0 &&
    ctx.finalTranslate.y == 0);

  if (stroke)
  {
    p = &tmpPath1;

    // Stroke mode. Stroke will flatten the path.
    tmpPath1.clear();
    PathStroker(ctx.strokeParams, ctx.approximationScale).stroke(tmpPath1, path);

    if (ctx.hints.transformType >= RASTER_TRANSFORM_AFFINE)
    {
      tmpPath1.applyMatrix(ctx.finalMatrix);
    }
    else if (!noTransform)
    {
      tmpPath1.translate(ctx.finalMatrix.tx, ctx.finalMatrix.ty);
    }

    // Stroke not respects fill rule set in the caps state, instead
    // we are using FILL_NON_ZERO.
    rasterizer.setFillRule(FILL_NON_ZERO);
  }
  else
  {
    // Flatten path.
    if (!path.isFlat() || !noTransform)
    {
      p = &tmpPath1;
      if (path.flattenTo(tmpPath1, noTransform ? NULL : &ctx.finalMatrix, ctx.approximationScale) != ERR_OK)
        return false;
    }

    // Fill respects the given fill rule.
    rasterizer.setFillRule(fillRule);
  }

  rasterizer.setClipBox(clipBox);
  rasterizer.setAlpha(ctx.ops.alpha255);

  rasterizer.initialize();
  rasterizer.addPath(*p);
  rasterizer.finalize();

  return rasterizer.isValid();
}

// ============================================================================
// [Fog::RasterPaintEngine - Renderers - Painting (shared)]
// ============================================================================

static const int pixelFormatGroupId[] =
{
   1, // IMAGE_FORMAT_PRGB32
  -1, // IMAGE_FORMAT_ARGB32
   1, // IMAGE_FORMAT_XRGB32
  -2, // IMAGE_FORMAT_A8
  -3  // IMAGE_FORMAT_I8
};

// Get whether blit of two pixel formats is fully opaque operation (dst pixel
// is not needed in this case). This method is used to optimize some fast paths.
static FOG_INLINE bool isRawOpaqueBlit(uint32_t dstFormat, uint32_t srcFormat, uint32_t op)
{
  if ((op == OPERATOR_SRC || (op == OPERATOR_SRC_OVER && srcFormat == IMAGE_FORMAT_XRGB32)) &&
      (pixelFormatGroupId[dstFormat] == pixelFormatGroupId[srcFormat]))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Renderers - Clipping (shared)]
// ============================================================================

// Clear row in clip-mask that is used or @c NULL. Never use this macro to clear
// uninitialized row (it's suicide).
static FOG_INLINE void Mask_clearRow(RasterPaintContext* ctx, int y)
{
  Span8* spans = ctx->getClipSpan(y);

  if (spans != NULL)
  {
    if (RASTER_CLIP_SPAN_IS_OWNED(spans)) ctx->freeChainedSpans(spans);
    ctx->setClipSpan(y, NULL);
  }
}

// Get whether boxes completely overlaps all spans. This means that each pixel
// defined by @a spans is also defined by @a boxes, but @a boxes can point also
// to pixels not defined by @a spans.
//
// Used by CLIP_OP_INTERSECT.
//
// True is returned on condition like this:
//
// ----------------------------------------------------------------------------
// SPANS:         XXXXXXX      XXXXX XXXXXX      XXXXXXXXXX
// ----------------------------------------------------------------------------
// BOXES: xxxxx   XXXXXXX  xxxxXXXXXxXXXXXXxxx   XXXXXXXXXXxxxxx xxxxxxx
// ----------------------------------------------------------------------------
//
// False is returned on condition like this:
//
//                                             /- Here (spans not overlapped by
//                                             |        boxes).
// --------------------------------------------+-------------------------------
// SPANS:         XXXXXXX      XXXXX XXXXXX  XxxxXXXXXXXXXX
// ----------------------------------------------------------------------------
// BOXES: xxxxx   XXXXXXX  xxxxXXXXXxXXXXXXxxX---XXXXXXXXXXxxxxx xxxxxxx
// ----------------------------------------------------------------------------
static FOG_INLINE bool Mask_overlaps(const Span8* spans, const IntBox* boxes, const IntBox* end)
{
  FOG_ASSERT(spans != NULL);
  FOG_ASSERT(boxes != NULL);
  FOG_ASSERT(end != NULL);

  // Our working variables.
  int spanX1 = spans->getX1();
  int spanX2 = spans->getX2();

  for (;;)
  {
    // Skip boxes outside.
    while (boxes->x2 <= spanX1) { if (++boxes == end) return false; }

    // Because boxes must be sorted and there can't be box sequence like this:
    //
    //   [0 to 100], [100 to 200]
    //
    // The rule is very simple:
    //
    // - If current box not completely overlaps the span, then we can say
    //   that spans are not overlapped (because next box will not overlap
    //   the span too).
    int boxX1 = boxes->x1;
    int boxX2 = boxes->x2;
    if (boxX1 > spanX1 || boxX2 < spanX2) return false;

    // Okay, span is overlapped, try next ones until they fit into the box.
    do {
      spans = spans->getNext();
      if (spans == NULL) return true;

      spanX1 = spans->getX1();
      spanX2 = spans->getX2();
    } while (spanX1 >= boxX1 && spanX2 <= boxX2);

    // If this span only partially overlaps then we failed.
    if (spanX1 <= boxX2) return false;
  }
}

} // Fog namespace

// ============================================================================
// [Include SingleThreaded Implementation]
// ============================================================================

#define CTX_MT_MODE 0
#define CTX_OFFSET 0
#define CTX_DELTA 1

#define CTX_SYMBOL(symbol) symbol##_st
#include "RasterPaintEngine_DoPaint_p.h"
#include "RasterPaintEngine_DoMask_p.h"
#undef CTX_SYMBOL

#undef CTX_DELTA
#undef CTX_OFFSET
#undef CTX_MT_MODE

// ============================================================================
// [Include MultiThreaded Implementation]
// ============================================================================

#define CTX_MT_MODE 1
#define CTX_OFFSET offset
#define CTX_DELTA delta

#define CTX_SYMBOL(symbol) symbol##_mt
#include "RasterPaintEngine_DoPaint_p.h"
#include "RasterPaintEngine_DoMask_p.h"
#undef CTX_SYMBOL

#undef CTX_DELTA
#undef CTX_OFFSET
#undef CTX_MT_MODE
