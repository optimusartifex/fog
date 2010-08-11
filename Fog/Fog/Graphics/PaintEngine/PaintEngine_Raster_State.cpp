// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Engine_p.h>
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_State_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintState]
// ============================================================================

void RasterPaintState::save(RasterPaintEngine* engine)
{
  // Default action is NOP (used when flags are 0x00000000).
  FOG_ASSERT(flags == 0);
}

void RasterPaintState::restore(RasterPaintEngine* engine)
{
  // Default action is NOP (used when flags are 0x00000000).
  FOG_ASSERT(flags == 0);
}

// ============================================================================
// [Fog::RasterPaintState]
// ============================================================================

void RasterPaintVarState::save(RasterPaintEngine* engine)
{
  // --------------------------------------------------------------------------
  // [Paint Params]
  // --------------------------------------------------------------------------

  if (flags & (PAINTER_STATE_PAINT_PARAMS))
  {
    ops = engine->ctx.ops;
    alpha = engine->ctx.alpha;

    if (engine->ctx.ops.sourceType == PAINTER_SOURCE_ARGB)
    {
      solid = engine->ctx.solid;
    }
    else // if (engine->ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
    {
      pattern.init(engine->ctx.pattern.instance());
      if (engine->ctx.pctx && engine->ctx.pctx->initialized)
      {
        pctx = engine->ctx.pctx;
        pctx->refCount.inc();
      }
      else
      {
        pctx = NULL;
      }
    }
  }

  // --------------------------------------------------------------------------
  // [ClipParams, FillParams, StrokeParams and Hints]
  // --------------------------------------------------------------------------

  if (flags & (PAINTER_STATE_CLIP_PARAMS   |
               PAINTER_STATE_FILL_PARAMS   |
               PAINTER_STATE_STROKE_PARAMS |
               PAINTER_STATE_HINTS         ))
  {
    hints = engine->ctx.hints;

    if (flags & PAINTER_STATE_STROKE_PARAMS)
    {
      strokeParams.init(engine->ctx.strokeParams);
    }
  }

  // --------------------------------------------------------------------------
  // [Matrix]
  // --------------------------------------------------------------------------

  if (flags & PAINTER_STATE_MATRIX)
  {
    userMatrix.init(engine->ctx.userMatrix);
  }

  // --------------------------------------------------------------------------
  // [Clip Area]
  // --------------------------------------------------------------------------

  if (flags & PAINTER_STATE_CLIP_AREA)
  {
    clipNoPaint = engine->ctx.state & (RASTER_STATE_NO_PAINT_FINAL_REGION | RASTER_STATE_NO_PAINT_MASK);
    finalOrigin = engine->ctx.finalOrigin;
    finalRegion.init(engine->ctx.finalRegion);
    finalClipType = engine->ctx.finalClipType;
    finalClipBox = engine->ctx.finalClipBox;

    mask = NULL;
    if (finalClipType == RASTER_CLIP_MASK)
    {
      mask = engine->ctx.mask;
      maskSavedCounter = engine->ctx.maskSavedCounter++;

      FOG_ASSERT(mask != NULL);
    }
  }
}

void RasterPaintVarState::restore(RasterPaintEngine* engine)
{
  bool updateFinalMatrix = false;

  // --------------------------------------------------------------------------
  // [Paint Parameters]
  // --------------------------------------------------------------------------

  if (flags & PAINTER_STATE_PAINT_PARAMS)
  {
    engine->ctx.ops.op = ops.op;
    engine->ctx.funcs = rasterFuncs.getCompositeFuncs(engine->ctx.paintLayer.format, ops.op);

    engine->ctx.ops.alpha255 = ops.alpha255;
    engine->ctx.alpha = alpha;
    // TODO RASTERIZER: Why?
    engine->rasterizer.setAlpha(ops.alpha255);

    switch (ops.sourceType)
    {
      case PAINTER_SOURCE_ARGB:
        // Destroy the pattern instance if needed - declared as Static<Pattern>.
        if (engine->ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
        {
          engine->ctx.ops.sourceType = PAINTER_SOURCE_ARGB;
          engine->ctx.pattern.destroy();
          engine->_resetRasterPatternContext();
        }

        engine->ctx.ops.sourceFormat = ops.sourceFormat;
        engine->ctx.solid = solid;
        break;

      case PAINTER_SOURCE_PATTERN:
        // Create the pattern instance if needed - declared as Static<Pattern>.
        if (engine->ctx.ops.sourceType == PAINTER_SOURCE_ARGB)
        {
          engine->ctx.ops.sourceType = PAINTER_SOURCE_PATTERN;
          engine->ctx.pattern.init(pattern.instance());
        }
        else // if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
        {
          engine->ctx.pattern.instance() = pattern.instance();
        }

        if (engine->ctx.pctx != NULL && 
            engine->ctx.pctx->initialized)
        {
          if (engine->ctx.pctx->refCount.deref())
          {
            engine->ctx.pctx->destroy(engine->ctx.pctx);
            engine->blockAllocator.free(engine->ctx.pctx);
          }
          engine->ctx.pctx = NULL;
        }

        engine->ctx.ops.sourceFormat = ops.sourceFormat;

        if (engine->ctx.pctx == NULL)
        {
          engine->ctx.pctx = pctx;
        }
        else if (pctx)
        {
          engine->blockAllocator.free(engine->ctx.pctx);
          engine->ctx.pctx = pctx;
        }

        pattern.destroy();
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }

  // --------------------------------------------------------------------------
  // [ClipParams, FillParams, StrokeParams and Hints]
  // --------------------------------------------------------------------------

  if (flags & PAINTER_STATE_CLIP_PARAMS)
  {
    engine->ctx.hints.clipRule = hints.clipRule;
  }

  if (flags & PAINTER_STATE_FILL_PARAMS)
  {
    engine->ctx.hints.fillRule = hints.fillRule;
  }

  if (flags & PAINTER_STATE_STROKE_PARAMS)
  {
    engine->ctx.strokeParams = strokeParams.instance();
    strokeParams.destroy();
  }

  if (flags & PAINTER_STATE_HINTS)
  {
    engine->ctx.hints.aaQuality             = hints.aaQuality;
    engine->ctx.hints.imageInterpolation    = hints.imageInterpolation;
    engine->ctx.hints.colorInterpolation    = hints.colorInterpolation;
    engine->ctx.hints.forceOutlineText      = hints.forceOutlineText;
  }

  // --------------------------------------------------------------------------
  // [Matrix]
  // --------------------------------------------------------------------------

  if (flags & PAINTER_STATE_MATRIX)
  {
    engine->ctx.userMatrix = userMatrix.instance();
    updateFinalMatrix = true;
  }

  // --------------------------------------------------------------------------
  // [Clip Area]
  // --------------------------------------------------------------------------

  if (flags & PAINTER_STATE_CLIP_AREA)
  {
    engine->ctx.state &= ~(RASTER_STATE_NO_PAINT_FINAL_REGION | RASTER_STATE_NO_PAINT_MASK);
    engine->ctx.state |= clipNoPaint;

    engine->ctx.finalOrigin = finalOrigin;
    engine->ctx.finalRegion = finalRegion.instance();

    engine->ctx.finalClipType = finalClipType;
    engine->ctx.finalClipBox = finalClipBox;

    engine->ctx.maskSavedCounter = maskSavedCounter;
    if (engine->ctx.mask != mask) engine->_serializeMaskRestore(mask);

    if (!(flags & PAINTER_STATE_MATRIX)) updateFinalMatrix = true;

    finalRegion.destroy();
  }

  // --------------------------------------------------------------------------
  // [Finished]
  // --------------------------------------------------------------------------

  if (updateFinalMatrix) engine->_updateMatrix(false);

  // Clear flags.
  flags = 0x00000000;
}

} // Fog namespace
