// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Graphics/Region.h>

#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Calc_p.h>
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Cmd_p.h>
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Engine_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintCalcFillPath]
// ============================================================================

void RasterPaintCalcFillPath::run(RasterPaintContext* ctx)
{
  RasterPaintCmdPath* cmd = reinterpret_cast<RasterPaintCmdPath*>(relatedTo);
  AnalyticRasterizer* ras;

  cmd->rasterizer.init();
  ras = cmd->rasterizer.instancep();

  bool noTransform = (
    transformType == RASTER_TRANSFORM_EXACT &&
    Math::feq(matrix->tx, 0.0) &&
    Math::feq(matrix->ty, 0.0) == 0);

  ras->reset();
  ras->setClipBox(clipBox);
  ras->setAlpha(cmd->ops.alpha255);
  ras->setFillRule(fillRule);
  ras->initialize();

  if (!path->isFlat() || !noTransform)
  {
    if (path->flattenTo(ctx->tmpCalcPath, noTransform ? NULL : matrix.instancep(), approximationScale) != ERR_OK)
    {
      cmd->status.set(RASTER_COMMAND_SKIP);
      return;
    }
    ras->addPath(ctx->tmpCalcPath);
  }
  else
  {
    ras->addPath(path.instance());
  }

  ras->finalize();
  cmd->status.set(ras->isValid() ? RASTER_COMMAND_READY : RASTER_COMMAND_SKIP);
}

void RasterPaintCalcFillPath::release(RasterPaintContext* ctx)
{
  path.destroy();
}

// ============================================================================
// [Fog::RasterPaintCalcStrokePath]
// ============================================================================

void RasterPaintCalcStrokePath::run(RasterPaintContext* ctx)
{
  RasterPaintCmdPath* cmd = reinterpret_cast<RasterPaintCmdPath*>(relatedTo);
  AnalyticRasterizer* ras;

  cmd->rasterizer.init();
  ras = cmd->rasterizer.instancep();

  bool noTransform = (
    transformType == RASTER_TRANSFORM_EXACT &&
    Math::feq(matrix->tx, 0.0) &&
    Math::feq(matrix->ty, 0.0) == 0);

  ctx->tmpCalcPath.clear();
  stroker->stroke(ctx->tmpCalcPath, path.instance());

  if (transformType >= RASTER_TRANSFORM_AFFINE)
    ctx->tmpCalcPath.applyMatrix(matrix.instance());
  else if (!noTransform)
    ctx->tmpCalcPath.translate(matrix->tx, matrix->ty);

  ras->reset();

  ras->setClipBox(clipBox);
  // Stroke not respects fill rule set in caps
  // state, instead we are using FILL_NON_ZERO.
  ras->setFillRule(FILL_NON_ZERO);
  ras->setAlpha(cmd->ops.alpha255);
  ras->initialize();

  ras->addPath(ctx->tmpCalcPath);
  ras->finalize();

  cmd->status.set(ras->isValid() ? RASTER_COMMAND_READY : RASTER_COMMAND_SKIP);
}

void RasterPaintCalcStrokePath::release(RasterPaintContext* ctx)
{
  path.destroy();
  stroker.destroy();
}

} // Fog namespace
