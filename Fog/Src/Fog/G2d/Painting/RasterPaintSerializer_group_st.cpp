// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/FaceC.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Imaging/Filters/FeBase.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintContext_p.h>
#include <Fog/G2d/Painting/RasterPaintEngine_p.h>
#include <Fog/G2d/Painting/RasterPaintGroup_p.h>
#include <Fog/G2d/Painting/RasterPaintSerializer_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterState_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintSerializer - VTable]
// ============================================================================

FOG_NO_EXPORT RasterPaintSerializer RasterPaintSerializer_group_vtable[RASTER_MODE_COUNT];

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FillAll]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillAll_st(
  RasterPaintEngine* engine)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FillPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillPathF_st(
  RasterPaintEngine* engine, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillPathD_st(
  RasterPaintEngine* engine, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - StrokeAndFillPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillStrokedPathF_st(
  RasterPaintEngine* engine, const PathF* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillStrokedPathD_st(
  RasterPaintEngine* engine, const PathD* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FillNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillNormalizedBoxI_st(
  RasterPaintEngine* engine, const BoxI* box)
{
  RasterPaintCmd_FillShape<BoxI>* cmd = engine->newCmd< RasterPaintCmd_FillShape<BoxI> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_I, *box, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillNormalizedBoxF_st(
  RasterPaintEngine* engine, const BoxF* box)
{
  RasterPaintCmd_FillShape<BoxF>* cmd = engine->newCmd< RasterPaintCmd_FillShape<BoxF> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_F, *box, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillNormalizedBoxD_st(
  RasterPaintEngine* engine, const BoxD* box)
{
  RasterPaintCmd_FillShape<BoxD>* cmd = engine->newCmd< RasterPaintCmd_FillShape<BoxD> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_D, *box, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FillNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillNormalizedPathF_st(
  RasterPaintEngine* engine, const PathF* path, uint32_t fillRule)
{
  RasterPaintCmd_FillShape<PathF>* cmd = engine->newCmd< RasterPaintCmd_FillShape<PathF> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(RASTER_PAINT_CMD_FILL_NORMALIZED_PATH_F, *path, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_fillNormalizedPathD_st(
  RasterPaintEngine* engine, const PathD* path, uint32_t fillRule)
{
  RasterPaintCmd_FillShape<PathD>* cmd = engine->newCmd< RasterPaintCmd_FillShape<PathD> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(RASTER_PAINT_CMD_FILL_NORMALIZED_PATH_D, *path, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - BlitImage]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_blitImageD_st(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - BlitNormalizedImageA]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_blitNormalizedImageA_st(
  RasterPaintEngine* engine, const PointI* pt, const Image* srcImage, const RectI* srcFragment)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - BlitNormalizedImage]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_blitNormalizedImageI_st(
  RasterPaintEngine* engine, const BoxI* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_blitNormalizedImageD_st(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FilterPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_filterPathF_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_filterPathD_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_filterStrokedPathF_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_filterStrokedPathD_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FilterNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_filterNormalizedBoxI_st(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxI* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_filterNormalizedBoxF_st(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxF* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_filterNormalizedBoxD_st(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxD* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FilterNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_filterNormalizedPathF_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_filterNormalizedPathD_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - ClipAll]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipAll_st(
  RasterPaintEngine* engine)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - ClipPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipPathF(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipPathD(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipStrokedPathF(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipStrokedPathD(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - ClipNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipNormalizedBoxI_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxI* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipNormalizedBoxF_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxF* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipNormalizedBoxD_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxD* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - ClipNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipNormalizedPathF_st(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_group_clipNormalizedPathD_st(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - Init]
// ============================================================================

void FOG_NO_EXPORT RasterPaintSerializer_init_group_st(void)
{
  RasterPaintSerializer* s = &RasterPaintSerializer_group_vtable[RASTER_MODE_ST];

  // --------------------------------------------------------------------------
  // [Fill/Stroke]
  // --------------------------------------------------------------------------

  s->fillAll = RasterPaintSerializer_group_fillAll_st;

  s->fillPathF = RasterPaintSerializer_group_fillPathF_st;
  s->fillPathD = RasterPaintSerializer_group_fillPathD_st;

  s->fillStrokedPathF = RasterPaintSerializer_group_fillStrokedPathF_st;
  s->fillStrokedPathD = RasterPaintSerializer_group_fillStrokedPathD_st;

  s->fillNormalizedBoxI = RasterPaintSerializer_group_fillNormalizedBoxI_st;
  s->fillNormalizedBoxF = RasterPaintSerializer_group_fillNormalizedBoxF_st;
  s->fillNormalizedBoxD = RasterPaintSerializer_group_fillNormalizedBoxD_st;
  s->fillNormalizedPathF = RasterPaintSerializer_group_fillNormalizedPathF_st;
  s->fillNormalizedPathD = RasterPaintSerializer_group_fillNormalizedPathD_st;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  s->blitImageD = RasterPaintSerializer_group_blitImageD_st;

  s->blitNormalizedImageA = RasterPaintSerializer_group_blitNormalizedImageA_st;
  s->blitNormalizedImageI = RasterPaintSerializer_group_blitNormalizedImageI_st;
  s->blitNormalizedImageD = RasterPaintSerializer_group_blitNormalizedImageD_st;

  // --------------------------------------------------------------------------
  // [Filter]
  // --------------------------------------------------------------------------

  s->filterPathF = RasterPaintSerializer_group_filterPathF_st;
  s->filterPathD = RasterPaintSerializer_group_filterPathD_st;

  s->filterStrokedPathF = RasterPaintSerializer_group_filterStrokedPathF_st;
  s->filterStrokedPathD = RasterPaintSerializer_group_filterStrokedPathD_st;

  s->filterNormalizedBoxI = RasterPaintSerializer_group_filterNormalizedBoxI_st;
  s->filterNormalizedBoxF = RasterPaintSerializer_group_filterNormalizedBoxF_st;
  s->filterNormalizedBoxD = RasterPaintSerializer_group_filterNormalizedBoxD_st;
  s->filterNormalizedPathF = RasterPaintSerializer_group_filterNormalizedPathF_st;
  s->filterNormalizedPathD = RasterPaintSerializer_group_filterNormalizedPathD_st;

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  s->clipAll = RasterPaintSerializer_group_clipAll_st;

  s->clipPathF = RasterPaintSerializer_group_clipPathF;
  s->clipPathD = RasterPaintSerializer_group_clipPathD;

  s->clipStrokedPathF = RasterPaintSerializer_group_clipStrokedPathF;
  s->clipStrokedPathD = RasterPaintSerializer_group_clipStrokedPathD;

  s->clipNormalizedBoxI = RasterPaintSerializer_group_clipNormalizedBoxI_st;
  s->clipNormalizedBoxF = RasterPaintSerializer_group_clipNormalizedBoxF_st;
  s->clipNormalizedBoxD = RasterPaintSerializer_group_clipNormalizedBoxD_st;
  s->clipNormalizedPathF = RasterPaintSerializer_group_clipNormalizedPathF_st;
  s->clipNormalizedPathD = RasterPaintSerializer_group_clipNormalizedPathD_st;
}

} // Fog namespace
