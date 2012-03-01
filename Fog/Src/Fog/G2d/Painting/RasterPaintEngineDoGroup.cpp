// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Acc/AccC.h>
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
#include <Fog/G2d/Painting/RasterPaintCmd_p.h>
#include <Fog/G2d/Painting/RasterPaintContext_p.h>
#include <Fog/G2d/Painting/RasterPaintEngine_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintDoGroup - VTable]
// ============================================================================

FOG_NO_EXPORT RasterPaintDoCmd RasterPaintDoGroup_vtable[RASTER_MODE_COUNT];

// ============================================================================
// [Fog::RasterPaintDoGroup - Pending]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_processPendingFlags(RasterPaintEngine* engine, uint32_t pending)
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
      _FOG_RASTER_ENSURE_PATTERN(engine);

      RasterPaintCmd_SetOpacityAndPattern* cmd = engine->newCmd<RasterPaintCmd_SetOpacityAndPattern>();
      if (FOG_IS_NULL(cmd))
        return ERR_RT_OUT_OF_MEMORY;
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

  if (pending & RASTER_PENDING_PAINT_HINTS)
  {
    RasterPaintCmd_SetPaintHints* cmd = engine->newCmd<RasterPaintCmd_SetPaintHints>();
    if (FOG_IS_NULL(cmd))
      return ERR_RT_OUT_OF_MEMORY;
    cmd->init(engine, RASTER_PAINT_CMD_SET_PAINT_HINTS, engine->ctx.paintHints);
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
      FOG_RETURN_ON_ERROR(RasterPaintDoGroup_processPendingFlags(engine, pending)); \
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
      FOG_RETURN_ON_ERROR(RasterPaintDoGroup_processPendingFlags(engine, pending)); \
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
      FOG_RETURN_ON_ERROR(RasterPaintDoGroup_processPendingFlags(engine, pending)); \
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
      FOG_RETURN_ON_ERROR(RasterPaintDoGroup_processPendingFlags(engine, pending)); \
    } \
  FOG_MACRO_END

// ============================================================================
// [Fog::RasterPaintDoGroup - Fill - All]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_fillAll(
  RasterPaintEngine* engine)
{
  _SERIALIZE_PENDING_FLAGS_FILL_NORMALIZED_BOX();

  RasterPaintCmd_FillAll* cmd = engine->newCmd<RasterPaintCmd_FillAll>();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;
  cmd->init(engine, RASTER_PAINT_CMD_FILL_ALL);

  engine->curGroup->mergeBoundingBox(engine->ctx.clipBoxI);
  return ERR_OK;
}


// ============================================================================
// [Fog::RasterPaintDoGroup - Fill - NormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_fillNormalizedBoxI(
  RasterPaintEngine* engine, const BoxI* box)
{
  _SERIALIZE_PENDING_FLAGS_FILL_NORMALIZED_BOX();

  RasterPaintCmd_FillNormalizedBoxI* cmd = engine->newCmd<RasterPaintCmd_FillNormalizedBoxI>();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;
  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_I, *box);

  engine->curGroup->mergeBoundingBox(*box);
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_fillNormalizedBoxF(
  RasterPaintEngine* engine, const BoxF* box)
{
  _SERIALIZE_PENDING_FLAGS_FILL_NORMALIZED_BOX();

  RasterPaintCmd_FillNormalizedBoxF* cmd = engine->newCmd<RasterPaintCmd_FillNormalizedBoxF>();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;
  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_F, *box);

  engine->curGroup->mergeBoundingBox(
    Math::ifloor(box->x0),
    Math::ifloor(box->y0),
    Math::iceil(box->x1),
    Math::iceil(box->y1));
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_fillNormalizedBoxD(
  RasterPaintEngine* engine, const BoxD* box)
{
  _SERIALIZE_PENDING_FLAGS_FILL_NORMALIZED_BOX();

  RasterPaintCmd_FillNormalizedBoxD* cmd = engine->newCmd<RasterPaintCmd_FillNormalizedBoxD>();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;
  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_D, *box);

  engine->curGroup->mergeBoundingBox(
    Math::ifloor(box->x0),
    Math::ifloor(box->y0),
    Math::iceil(box->x1),
    Math::iceil(box->y1));
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - Fill - NormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_fillNormalizedPathF(
  RasterPaintEngine* engine, const PathF* path, const PointF* pt, uint32_t fillRule)
{
  BoxF boundingBox;
  FOG_RETURN_ON_ERROR(path->getBoundingBox(boundingBox));

  _SERIALIZE_PENDING_FLAGS_FILL();

  RasterPaintCmd_FillNormalizedPathF* cmd = engine->newCmd<RasterPaintCmd_FillNormalizedPathF>();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;
  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_PATH_F,
    *path, *pt, engine->ctx.paintHints.fillRule);

  engine->curGroup->mergeBoundingBox(
    Math::ifloor(boundingBox.x0),
    Math::ifloor(boundingBox.y0),
    Math::iceil(boundingBox.x1),
    Math::iceil(boundingBox.y1));
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_fillNormalizedPathD(
  RasterPaintEngine* engine, const PathD* path, const PointD* pt, uint32_t fillRule)
{
  BoxD boundingBox;
  FOG_RETURN_ON_ERROR(path->getBoundingBox(boundingBox));

  _SERIALIZE_PENDING_FLAGS_FILL();

  RasterPaintCmd_FillNormalizedPathD* cmd = engine->newCmd<RasterPaintCmd_FillNormalizedPathD>();
  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;
  cmd->init(engine, RASTER_PAINT_CMD_FILL_NORMALIZED_PATH_D,
    *path, *pt, engine->ctx.paintHints.fillRule);

  engine->curGroup->mergeBoundingBox(
    Math::ifloor(boundingBox.x0),
    Math::ifloor(boundingBox.y0),
    Math::iceil(boundingBox.x1),
    Math::iceil(boundingBox.y1));
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - Blit - Image]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_blitImageD(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform, uint32_t imageQuality)
{
  _SERIALIZE_PENDING_FLAGS_BLIT();

  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - Blit - NormalizedImageA]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_blitNormalizedImageA(
  RasterPaintEngine* engine, const PointI* pt, const Image* srcImage, const RectI* srcFragment)
{
  _SERIALIZE_PENDING_FLAGS_BLIT();

  ImageData* srcD = srcImage->_d;

  if (srcFragment->x == 0 &&
      srcFragment->y == 0 &&
      srcFragment->w == srcD->size.w &&
      srcFragment->h == srcD->size.h)
  {
    RasterPaintCmd_BlitNormalizedImageA* cmd =
      engine->newCmd<RasterPaintCmd_BlitNormalizedImageA>();

    if (FOG_IS_NULL(cmd))
      return ERR_RT_OUT_OF_MEMORY;

    cmd->init(engine, RASTER_PAINT_CMD_BLIT_NORMALIZED_IMAGE_A,
      *pt, *srcImage);
  }
  else
  {
    RasterPaintCmd_BlitNormalizedImageFragmentA* cmd =
      engine->newCmd<RasterPaintCmd_BlitNormalizedImageFragmentA>();

    if (FOG_IS_NULL(cmd))
      return ERR_RT_OUT_OF_MEMORY;

    cmd->init(engine, RASTER_PAINT_CMD_BLIT_NORMALIZED_IMAGE_FRAGMENT_A,
      *pt, *srcImage, *srcFragment);
  }

  engine->curGroup->mergeBoundingBox(
    pt->x,
    pt->y,
    pt->x + srcFragment->w,
    pt->y + srcFragment->h);
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - Blit - NormalizedImage]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_blitNormalizedImageI(
  RasterPaintEngine* engine, const BoxI* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform, uint32_t imageQuality)
{
  _SERIALIZE_PENDING_FLAGS_BLIT();

  RasterPaintCmd_BlitNormalizedImageI* cmd =
    engine->newCmd<RasterPaintCmd_BlitNormalizedImageI>();

  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(engine, RASTER_PAINT_CMD_BLIT_NORMALIZED_IMAGE_I,
    *box, *srcImage, *srcFragment, *srcTransform, imageQuality);

  engine->curGroup->mergeBoundingBox(*box);
  return ERR_OK;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_blitNormalizedImageD(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform, uint32_t imageQuality)
{
  _SERIALIZE_PENDING_FLAGS_BLIT();

  RasterPaintCmd_BlitNormalizedImageD* cmd =
    engine->newCmd<RasterPaintCmd_BlitNormalizedImageD>();

  if (FOG_IS_NULL(cmd))
    return ERR_RT_OUT_OF_MEMORY;

  cmd->init(engine, RASTER_PAINT_CMD_BLIT_NORMALIZED_IMAGE_D,
    *box, *srcImage, *srcFragment, *srcTransform, imageQuality);

  engine->curGroup->mergeBoundingBox(
    Math::ifloor(box->x0),
    Math::ifloor(box->y0),
    Math::iceil(box->x1),
    Math::iceil(box->y1));
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - Filter - NormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_filterNormalizedBoxI(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxI* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_filterNormalizedBoxF(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxF* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_filterNormalizedBoxD(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxD* box)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - Filter - NormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_filterNormalizedPathF(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, const PointF* pt, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_filterNormalizedPathD(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, const PointD* pt, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - SwitchToMask / DiscardMask]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_switchToMask(RasterPaintEngine* engine)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_discardMask(RasterPaintEngine* engine)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - SaveMask / RestoreMask]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_saveMask(RasterPaintEngine* engine)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_restoreMask(RasterPaintEngine* engine)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - MaskNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_maskNormalizedBoxI(RasterPaintEngine* engine, uint32_t clipOp, const BoxI* box)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_maskNormalizedBoxF(RasterPaintEngine* engine, uint32_t clipOp, const BoxF* box)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_maskNormalizedBoxD(RasterPaintEngine* engine, uint32_t clipOp, const BoxD* box)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - MaskNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoGroup_maskNormalizedPathF(RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoGroup_maskNormalizedPathD(RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - Init]
// ============================================================================

void FOG_NO_EXPORT RasterPaintDoGroup_init(void)
{
  RasterPaintDoCmd* v = &RasterPaintDoGroup_vtable[RASTER_MODE_ST];

  // --------------------------------------------------------------------------
  // [Fill/Stroke]
  // --------------------------------------------------------------------------

  v->fillAll = RasterPaintDoGroup_fillAll;
  v->fillNormalizedBoxI = RasterPaintDoGroup_fillNormalizedBoxI;
  v->fillNormalizedBoxF = RasterPaintDoGroup_fillNormalizedBoxF;
  v->fillNormalizedBoxD = RasterPaintDoGroup_fillNormalizedBoxD;
  v->fillNormalizedPathF = RasterPaintDoGroup_fillNormalizedPathF;
  v->fillNormalizedPathD = RasterPaintDoGroup_fillNormalizedPathD;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  v->blitImageD = RasterPaintDoGroup_blitImageD;
  v->blitNormalizedImageA = RasterPaintDoGroup_blitNormalizedImageA;
  v->blitNormalizedImageI = RasterPaintDoGroup_blitNormalizedImageI;
  v->blitNormalizedImageD = RasterPaintDoGroup_blitNormalizedImageD;

  // --------------------------------------------------------------------------
  // [Filter]
  // --------------------------------------------------------------------------

  v->filterNormalizedBoxI = RasterPaintDoGroup_filterNormalizedBoxI;
  v->filterNormalizedBoxF = RasterPaintDoGroup_filterNormalizedBoxF;
  v->filterNormalizedBoxD = RasterPaintDoGroup_filterNormalizedBoxD;
  v->filterNormalizedPathF = RasterPaintDoGroup_filterNormalizedPathF;
  v->filterNormalizedPathD = RasterPaintDoGroup_filterNormalizedPathD;

  // --------------------------------------------------------------------------
  // [Mask]
  // --------------------------------------------------------------------------

  v->switchToMask = RasterPaintDoGroup_switchToMask;
  v->discardMask = RasterPaintDoGroup_discardMask;

  v->saveMask = RasterPaintDoGroup_saveMask;
  v->restoreMask = RasterPaintDoGroup_restoreMask;

  v->maskNormalizedBoxI = RasterPaintDoGroup_maskNormalizedBoxI;
  v->maskNormalizedBoxF = RasterPaintDoGroup_maskNormalizedBoxF;
  v->maskNormalizedBoxD = RasterPaintDoGroup_maskNormalizedBoxD;
  v->maskNormalizedPathF = RasterPaintDoGroup_maskNormalizedPathF;
  v->maskNormalizedPathD = RasterPaintDoGroup_maskNormalizedPathD;
}

} // Fog namespace
