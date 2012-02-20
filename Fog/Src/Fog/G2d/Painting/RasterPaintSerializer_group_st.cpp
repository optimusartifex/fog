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
// [Fog::RasterPaintSerializer - Group(st) - Pending]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_processPendingFlags(RasterPaintEngine* engine, uint32_t pending)
{
  FOG_ASSERT(pending != 0);
  engine->masterFlags ^= pending;

  if (pending & RASTER_PENDING_SOURCE)
  {
    if (RasterUtil::isSolidContext(engine->ctx.pc))
    {
      RasterPaintCmd_SetOpacityAndPrgb32* cmd = engine->newCmd<RasterPaintCmd_SetOpacityAndPrgb32>();
      if (FOG_IS_NULL(cmd))
        return ERR_RT_OUT_OF_MEMORY;
      cmd->init(engine, RASTER_PAINT_CMD_SET_OPACITY_AND_PRGB32, engine->ctx.rasterHints.opacity, engine->ctx.solid.prgb32.u32);
    }
    else
    {
      RasterPaintCmd_SetOpacityAndPattern* cmd = engine->newCmd<RasterPaintCmd_SetOpacityAndPattern>();
      if (FOG_IS_NULL(cmd))
        return ERR_RT_OUT_OF_MEMORY;
      _FOG_RASTER_ENSURE_PATTERN(engine);
      cmd->init(engine, RASTER_PAINT_CMD_SET_OPACITY_AND_PATTERN, engine->ctx.rasterHints.opacity, engine->ctx.pc);
    }
  }
  else if (pending & RASTER_PENDING_OPACITY)
  {
    RasterPaintCmd_SetOpacity* cmd = engine->newCmd<RasterPaintCmd_SetOpacity>();
    if (FOG_IS_NULL(cmd))
      return ERR_RT_OUT_OF_MEMORY;
    cmd->init(engine, RASTER_PAINT_CMD_SET_OPACITY, engine->ctx.rasterHints.opacity);
  }

  if (pending & RASTER_PENDING_HINTS)
  {
  }

  if (pending & RASTER_PENDING_TRANSFORM)
  {
  }

  if (pending & RASTER_PENDING_CLIP)
  {
    uint32_t clipType = engine->ctx.clipType;
    
    if (clipType == RASTER_CLIP_BOX)
    {
      RasterPaintCmd_SetClipBox* cmd = engine->newCmd<RasterPaintCmd_SetClipBox>();
      if (FOG_IS_NULL(cmd))
        return ERR_RT_OUT_OF_MEMORY;
      cmd->init(engine, RASTER_PAINT_CMD_SET_CLIP_BOX, engine->ctx.clipBoxI);
    }
    else
    {
      RasterPaintCmd_SetClipRegion* cmd = engine->newCmd<RasterPaintCmd_SetClipRegion>();
      if (FOG_IS_NULL(cmd))
        return ERR_RT_OUT_OF_MEMORY;
      cmd->init(engine, RASTER_PAINT_CMD_SET_CLIP_REGION, engine->ctx.clipRegion);
    }
  }

  if (pending & RASTER_PENDING_STROKE_PARAMS)
  {
  }

  if (pending & RASTER_PENDING_FILTER_PARAMS)
  {
  }

  engine->masterFlags ^= pending;
  return ERR_OK;
}

#define _SERIALIZE_PENDING_FLAGS_FILL_NORMALIZED_BOX() \
  FOG_MACRO_BEGIN \
    uint32_t pending = engine->masterFlags & ( \
      RASTER_PENDING_BASE_FLAGS     | \
      RASTER_PENDING_SOURCE        ); \
    \
    if (pending != 0) \
    { \
      FOG_RETURN_ON_ERROR(RasterPaintSerializer_processPendingFlags(engine, pending)); \
    } \
  FOG_MACRO_END

#define _SERIALIZE_PENDING_FLAGS_FILL() \
  FOG_MACRO_BEGIN \
    uint32_t pending = engine->masterFlags & ( \
      RASTER_PENDING_BASE_FLAGS     | \
      RASTER_PENDING_SOURCE         | \
      RASTER_PENDING_TRANSFORM     ); \
    \
    if (pending != 0) \
    { \
      FOG_RETURN_ON_ERROR(RasterPaintSerializer_processPendingFlags(engine, pending)); \
    } \
  FOG_MACRO_END

#define _SERIALIZE_PENDING_FLAGS_STROKE() \
  FOG_MACRO_BEGIN \
    uint32_t pending = engine->masterFlags & ( \
      RASTER_PENDING_BASE_FLAGS     | \
      RASTER_PENDING_SOURCE         | \
      RASTER_PENDING_TRANSFORM      | \
      RASTER_PENDING_STROKE_PARAMS ); \
    \
    if (pending != 0) \
    { \
      FOG_RETURN_ON_ERROR(RasterPaintSerializer_processPendingFlags(engine, pending)); \
    } \
  FOG_MACRO_END

#define _SERIALIZE_PENDING_FLAGS_BLIT() \
  FOG_MACRO_BEGIN \
    uint32_t pending = engine->masterFlags & ( \
      RASTER_PENDING_BASE_FLAGS     | \
      RASTER_PENDING_TRANSFORM     ); \
    \
    if (pending != 0) \
    { \
      FOG_RETURN_ON_ERROR(RasterPaintSerializer_processPendingFlags(engine, pending)); \
    } \
  FOG_MACRO_END

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FillAll]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillAll_group_st(
  RasterPaintEngine* engine)
{
  _SERIALIZE_PENDING_FLAGS_FILL_NORMALIZED_BOX();

  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FillPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillPathF_group_st(
  RasterPaintEngine* engine, const PathF* path, uint32_t fillRule)
{
  _SERIALIZE_PENDING_FLAGS_FILL();

  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillPathD_group_st(
  RasterPaintEngine* engine, const PathD* path, uint32_t fillRule)
{
  _SERIALIZE_PENDING_FLAGS_FILL();

  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - StrokeAndFillPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillStrokedPathF_group_st(
  RasterPaintEngine* engine, const PathF* path)
{
  _SERIALIZE_PENDING_FLAGS_STROKE();

  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillStrokedPathD_group_st(
  RasterPaintEngine* engine, const PathD* path)
{
  _SERIALIZE_PENDING_FLAGS_STROKE();

  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FillNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedBoxI_group_st(
  RasterPaintEngine* engine, const BoxI* box)
{
  _SERIALIZE_PENDING_FLAGS_FILL_NORMALIZED_BOX();

  RasterPaintCmd_FillShape<BoxI>* cmd = engine->newCmd< RasterPaintCmd_FillShape<BoxI> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_I, *box, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedBoxF_group_st(
  RasterPaintEngine* engine, const BoxF* box)
{
  _SERIALIZE_PENDING_FLAGS_FILL_NORMALIZED_BOX();

  RasterPaintCmd_FillShape<BoxF>* cmd = engine->newCmd< RasterPaintCmd_FillShape<BoxF> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_F, *box, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedBoxD_group_st(
  RasterPaintEngine* engine, const BoxD* box)
{
  _SERIALIZE_PENDING_FLAGS_FILL_NORMALIZED_BOX();

  RasterPaintCmd_FillShape<BoxD>* cmd = engine->newCmd< RasterPaintCmd_FillShape<BoxD> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_D,
    *box, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FillNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedPathF_group_st(
  RasterPaintEngine* engine, const PathF* path, uint32_t fillRule)
{
  _SERIALIZE_PENDING_FLAGS_FILL();

  RasterPaintCmd_FillShape<PathF>* cmd = engine->newCmd< RasterPaintCmd_FillShape<PathF> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_PATH_F,
    *path, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedPathD_group_st(
  RasterPaintEngine* engine, const PathD* path, uint32_t fillRule)
{
  _SERIALIZE_PENDING_FLAGS_FILL();

  RasterPaintCmd_FillShape<PathD>* cmd = engine->newCmd< RasterPaintCmd_FillShape<PathD> >();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_PATH_D,
    *path, engine->ctx.paintHints.fillRule);
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - BlitImage]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_blitImageD_group_st(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform)
{
  _SERIALIZE_PENDING_FLAGS_BLIT();

  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - BlitNormalizedImageA]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_blitNormalizedImageA_group_st(
  RasterPaintEngine* engine, const PointI* pt, const Image* srcImage, const RectI* srcFragment)
{
  _SERIALIZE_PENDING_FLAGS_BLIT();

  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - BlitNormalizedImage]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_blitNormalizedImageI_group_st(
  RasterPaintEngine* engine, const BoxI* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_blitNormalizedImageD_group_st(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FilterPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_filterPathF_group_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_filterPathD_group_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_filterStrokedPathF_group_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_filterStrokedPathD_group_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FilterNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedBoxI_group_st(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxI* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedBoxF_group_st(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxF* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedBoxD_group_st(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxD* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - FilterNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedPathF_group_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedPathD_group_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - ClipAll]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipAll_group_st(
  RasterPaintEngine* engine)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - ClipPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipPathF(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipPathD(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipStrokedPathF(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipStrokedPathD(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - ClipNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedBoxI_group_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxI* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedBoxF_group_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxF* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedBoxD_group_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxD* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Group(st) - ClipNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedPathF_group_st(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedPathD_group_st(
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

  s->fillAll = RasterPaintSerializer_fillAll_group_st;

  s->fillPathF = RasterPaintSerializer_fillPathF_group_st;
  s->fillPathD = RasterPaintSerializer_fillPathD_group_st;

  s->fillStrokedPathF = RasterPaintSerializer_fillStrokedPathF_group_st;
  s->fillStrokedPathD = RasterPaintSerializer_fillStrokedPathD_group_st;

  s->fillNormalizedBoxI = RasterPaintSerializer_fillNormalizedBoxI_group_st;
  s->fillNormalizedBoxF = RasterPaintSerializer_fillNormalizedBoxF_group_st;
  s->fillNormalizedBoxD = RasterPaintSerializer_fillNormalizedBoxD_group_st;
  s->fillNormalizedPathF = RasterPaintSerializer_fillNormalizedPathF_group_st;
  s->fillNormalizedPathD = RasterPaintSerializer_fillNormalizedPathD_group_st;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  s->blitImageD = RasterPaintSerializer_blitImageD_group_st;

  s->blitNormalizedImageA = RasterPaintSerializer_blitNormalizedImageA_group_st;
  s->blitNormalizedImageI = RasterPaintSerializer_blitNormalizedImageI_group_st;
  s->blitNormalizedImageD = RasterPaintSerializer_blitNormalizedImageD_group_st;

  // --------------------------------------------------------------------------
  // [Filter]
  // --------------------------------------------------------------------------

  s->filterPathF = RasterPaintSerializer_filterPathF_group_st;
  s->filterPathD = RasterPaintSerializer_filterPathD_group_st;

  s->filterStrokedPathF = RasterPaintSerializer_filterStrokedPathF_group_st;
  s->filterStrokedPathD = RasterPaintSerializer_filterStrokedPathD_group_st;

  s->filterNormalizedBoxI = RasterPaintSerializer_filterNormalizedBoxI_group_st;
  s->filterNormalizedBoxF = RasterPaintSerializer_filterNormalizedBoxF_group_st;
  s->filterNormalizedBoxD = RasterPaintSerializer_filterNormalizedBoxD_group_st;
  s->filterNormalizedPathF = RasterPaintSerializer_filterNormalizedPathF_group_st;
  s->filterNormalizedPathD = RasterPaintSerializer_filterNormalizedPathD_group_st;

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  s->clipAll = RasterPaintSerializer_clipAll_group_st;

  s->clipPathF = RasterPaintSerializer_clipPathF;
  s->clipPathD = RasterPaintSerializer_clipPathD;

  s->clipStrokedPathF = RasterPaintSerializer_clipStrokedPathF;
  s->clipStrokedPathD = RasterPaintSerializer_clipStrokedPathD;

  s->clipNormalizedBoxI = RasterPaintSerializer_clipNormalizedBoxI_group_st;
  s->clipNormalizedBoxF = RasterPaintSerializer_clipNormalizedBoxF_group_st;
  s->clipNormalizedBoxD = RasterPaintSerializer_clipNormalizedBoxD_group_st;
  s->clipNormalizedPathF = RasterPaintSerializer_clipNormalizedPathF_group_st;
  s->clipNormalizedPathD = RasterPaintSerializer_clipNormalizedPathD_group_st;
}

} // Fog namespace
