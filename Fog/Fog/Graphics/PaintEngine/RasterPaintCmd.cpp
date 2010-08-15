// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/AnalyticRasterizer_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintCmd_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintContext_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintEngine_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintCmdLayerChange]
// ============================================================================

void RasterPaintCmdLayerChange::run(RasterPaintContext* ctx)
{
  ctx->paintLayer = paintLayer;

  ctx->workClipType = workClipType;
  ctx->finalClipType = finalClipType;

  ctx->finalOrigin = finalOrigin;

  ctx->workClipBox = workRegion->getExtents();
  ctx->finalClipBox = finalRegion->getExtents();

  ctx->workRegion = workRegion.instance();
  ctx->finalRegion = finalRegion.instance();
}

void RasterPaintCmdLayerChange::release(RasterPaintContext* ctx)
{
  workRegion.destroy();
  finalRegion.destroy();
}

// ============================================================================
// [Fog::RasterPaintCmdUpdateFinalRegion]
// ============================================================================

void RasterPaintCmdUpdateRegion::run(RasterPaintContext* ctx)
{
  ctx->workClipType = workClipType;
  ctx->finalClipType = finalClipType;

  ctx->finalOrigin = finalOrigin;

  ctx->workClipBox = workRegion->getExtents();
  ctx->finalClipBox = finalRegion->getExtents();

  ctx->workRegion = workRegion.instance();
  ctx->finalRegion = finalRegion.instance();
}

void RasterPaintCmdUpdateRegion::release(RasterPaintContext* ctx)
{
  workRegion.destroy();
  finalRegion.destroy();
}

// ============================================================================
// [Fog::RasterPaintCmdRegion]
// ============================================================================

void RasterPaintCmdRegion::run(RasterPaintContext* ctx)
{
  _beforePaint(ctx);
  ctx->engine->_doPaintBoxes_mt(ctx, region->getData(), region->getLength());
  _afterPaint(ctx);
}

void RasterPaintCmdRegion::release(RasterPaintContext* ctx)
{
  region.destroy();
  _releasePattern(ctx);
}

// ============================================================================
// [Fog::RasterPaintCmdBoxes]
// ============================================================================

void RasterPaintCmdBoxes::run(RasterPaintContext* ctx)
{
  _beforePaint(ctx);
  ctx->engine->_doPaintBoxes_mt(ctx, boxes, count);
  _afterPaint(ctx);
}

void RasterPaintCmdBoxes::release(RasterPaintContext* ctx)
{
  _releasePattern(ctx);
}

// ============================================================================
// [Fog::RasterPaintCmdImage]
// ============================================================================

void RasterPaintCmdImage::run(RasterPaintContext* ctx)
{
  _beforePaint(ctx);
  ctx->engine->_doPaintImage_mt(ctx, dst, image.instance(), src);
  _afterPaint(ctx);
}

void RasterPaintCmdImage::release(RasterPaintContext* ctx)
{
  image.destroy();
  _releasePattern(ctx);
}

// ============================================================================
// [Fog::RasterPaintCmdGlyphSet]
// ============================================================================

void RasterPaintCmdGlyphSet::run(RasterPaintContext* ctx)
{
  _beforePaint(ctx);
  ctx->engine->_doPaintGlyphSet_mt(ctx, pt, glyphSet.instance(), boundingBox);
  _afterPaint(ctx);
}

void RasterPaintCmdGlyphSet::release(RasterPaintContext* ctx)
{
  glyphSet.destroy();
  _releasePattern(ctx);
}

// ============================================================================
// [Fog::RasterPaintCmdPath]
// ============================================================================

void RasterPaintCmdPath::run(RasterPaintContext* ctx)
{
  _beforePaint(ctx);
  ctx->engine->_doPaintPath_mt(ctx, rasterizer.instancep());
  _afterPaint(ctx);
}

void RasterPaintCmdPath::release(RasterPaintContext* ctx)
{
  rasterizer.destroy();
  _releasePattern(ctx);
}

} // Fog namespace
