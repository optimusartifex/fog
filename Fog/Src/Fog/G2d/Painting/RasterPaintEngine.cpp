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
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadCondition.h>
#include <Fog/Core/Threading/ThreadPool.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/ImageBits.h>
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
#include <Fog/G2d/Text/Font.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintEngine - Helpers]
// ============================================================================

// ${CLIP_OP:BEGIN}
static const uint32_t RasterPaintEngine_clipAllFlags[CLIP_OP_COUNT] =
{
  // 0 - CLIP_OP_REPLACE
  RASTER_NO_PAINT_META_REGION    |
  RASTER_NO_PAINT_META_TRANSFORM |
  RASTER_NO_PAINT_USER_TRANSFORM ,

  // 1 - CLIP_OP_INTERSECT
  RASTER_NO_PAINT_META_REGION    |
  RASTER_NO_PAINT_META_TRANSFORM |
  RASTER_NO_PAINT_USER_CLIP      |
  RASTER_NO_PAINT_USER_MASK      |
  RASTER_NO_PAINT_USER_TRANSFORM
};
// ${CLIP_OP:END}

// ============================================================================
// [Fog::RasterPaintEngine - Defs]
// ============================================================================

#define _PARAM_C(_Type_) (*static_cast<const _Type_*>(value))
#define _PARAM_M(_Type_) (*static_cast<_Type_*>(value))

// ============================================================================
// [Fog::RasterPaintEngine - VTable]
// ============================================================================

FOG_NO_EXPORT PaintEngineVTable RasterPaintEngine_vtable[IMAGE_PRECISION_COUNT];

// ============================================================================
// [Fog::RasterPaintEngine - AddRef / Release]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_release(Painter* self)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  engine->finalizing = true;
  fog_delete(engine);

  self->_engine = fog_api.painter_getNullEngine();
  self->_vtable = self->_engine->vtable;

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Meta Params]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_getMetaParams(const Painter* self, Region* region, PointI* origin)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  *region = engine->metaRegion;
  *origin = engine->metaOrigin;

  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setMetaParams(Painter* self, const Region* region, const PointI* origin)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  engine->discardStates();

  BoxI screen(0, 0, engine->ctx.target.size.w, engine->ctx.target.size.h);
  engine->metaOrigin = *origin;

  if (region->isInfinite())
    engine->metaRegion = screen;
  else if (screen.subsumes(region->getBoundingBox()))
    engine->metaRegion = *region;
  else
    Region::intersect(engine->metaRegion, *region, screen);

  engine->changedMetaParams();
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_resetMetaParams(Painter* self)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  engine->discardStates();

  BoxI screen(0, 0, engine->ctx.target.size.w, engine->ctx.target.size.h);

  engine->metaOrigin.reset();
  engine->metaRegion = screen;

  engine->changedMetaParams();
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Parameters]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_getParameter(const Painter* self, uint32_t parameterId, void* value)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  switch (parameterId)
  {
    // ------------------------------------------------------------------------
    // [Backend]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_SIZE_I:
    {
      _PARAM_M(SizeI).set(engine->ctx.target.size);
      return ERR_OK;
    }

    case PAINTER_PARAMETER_SIZE_F:
    {
      _PARAM_M(SizeF).set(engine->ctx.target.size);
      return ERR_OK;
    }

    case PAINTER_PARAMETER_SIZE_D:
    {
      _PARAM_M(SizeD).set(engine->ctx.target.size);
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FORMAT_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.target.format;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_DEVICE_I:
    {
      _PARAM_M(uint32_t) = PAINT_DEVICE_RASTER;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Multithreading]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_MULTITHREADED_I:
    {
      _PARAM_M(uint32_t) = false;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_MAX_THREADS_I:
    {
      _PARAM_M(uint32_t) = engine->maxThreads;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_PAINT_PARAMS_F:
    {
      PaintParamsF& params = _PARAM_M(PaintParamsF);

      params._hints = engine->ctx.paintHints;
      params._opacity = engine->opacityF;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_NONE:
          params._strokeParams.reset();
          params._strokeParams.setHints(engine->stroker.f->getParams().getHints());
          break;

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_F:
          params._strokeParams = engine->stroker.f->getParams();
          break;

        case RASTER_PRECISION_D:
          params._strokeParams = engine->stroker.d->getParams();
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      return ERR_OK;
    }

    case PAINTER_PARAMETER_PAINT_PARAMS_D:
    {
      PaintParamsD& params = _PARAM_M(PaintParamsD);

      params._hints = engine->ctx.paintHints;
      params._opacity  = engine->opacityF;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_NONE:
          params._strokeParams.reset();
          params._strokeParams.setHints(engine->stroker.f->getParams().getHints());
          break;

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          params._strokeParams = engine->stroker.d->getParams();
          break;

        case RASTER_PRECISION_F:
          params._strokeParams = engine->stroker.f->getParams();
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Hints]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_PAINT_HINTS:
    {
      _PARAM_M(PaintHints) = engine->ctx.paintHints;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_COMPOSITING_OPERATOR_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.paintHints.compositingOperator;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_RENDER_QUALITY_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.paintHints.renderQuality;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_IMAGE_QUALITY_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.paintHints.imageQuality;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GRADIENT_QUALITY_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.paintHints.gradientQuality;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OUTLINED_TEXT_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.paintHints.outlinedText;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FAST_LINE_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.paintHints.fastLine;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GEOMETRIC_PRECISION_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.paintHints.geometricPrecision;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint - Opacity]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_OPACITY_F:
    {
      _PARAM_M(float) = engine->opacityF;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OPACITY_D:
    {
      _PARAM_M(double) = engine->opacityF;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Fill Params - Fill Rule]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_FILL_RULE_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.paintHints.fillRule;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_STROKE_PARAMS_F:
    {
      PathStrokerParamsF& params = _PARAM_M(PathStrokerParamsF);

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_NONE:
          params.reset();
          params.setHints(engine->stroker.f->getParams().getHints());
          break;

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_F:
          params = engine->stroker.f->getParams();
          break;

        case RASTER_PRECISION_D:
          params = engine->stroker.d->getParams();
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      return ERR_OK;
    }

    case PAINTER_PARAMETER_STROKE_PARAMS_D:
    {
      PathStrokerParamsD& params = _PARAM_M(PathStrokerParamsD);

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_NONE:
          params.reset();
          params.setHints(engine->stroker.f->getParams().getHints());
          break;

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          params = engine->stroker.d->getParams();
          break;

        case RASTER_PRECISION_F:
          params = engine->stroker.f->getParams();
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Width]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_WIDTH_F:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_D)
        _PARAM_M(float) = (float)engine->stroker.d->getParams().getLineWidth();
      else
        _PARAM_M(float) = engine->stroker.f->getParams().getLineWidth();

      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_WIDTH_D:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_F)
        _PARAM_M(double) = engine->stroker.f->getParams().getLineWidth();
      else
        _PARAM_M(double) = engine->stroker.d->getParams().getLineWidth();

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Join]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_JOIN_I:
    {
      _PARAM_M(uint32_t) = engine->stroker.f->getParams().getLineJoin();
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Caps]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_START_CAP_I:
    {
      _PARAM_M(uint32_t) = engine->stroker.f->getParams().getStartCap();
      return ERR_OK;
    }

    case PAINTER_PARAMETER_END_CAP_I:
    {
      _PARAM_M(uint32_t) = engine->stroker.f->getParams().getEndCap();
      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_CAPS_I:
    {
      uint32_t startCap = engine->stroker.f->getParams().getStartCap();
      uint32_t endCap = engine->stroker.f->getParams().getEndCap();

      if (startCap == endCap)
      {
        _PARAM_M(uint32_t) = startCap;
        return ERR_OK;
      }
      else
      {
        return ERR_RT_INVALID_STATE;
      }
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Miter-Limit]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_MITER_LIMIT_F:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_D)
        _PARAM_M(float) = (float)engine->stroker.d->getParams().getMiterLimit();
      else
        _PARAM_M(float) = engine->stroker.f->getParams().getMiterLimit();

      return ERR_OK;
    }

    case PAINTER_PARAMETER_MITER_LIMIT_D:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_F)
        _PARAM_M(double) = engine->stroker.f->getParams().getMiterLimit();
      else
        _PARAM_M(double) = engine->stroker.d->getParams().getMiterLimit();

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-Offset]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_OFFSET_F:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_D)
        _PARAM_M(float) = (float)engine->stroker.d->getParams().getDashOffset();
      else
        _PARAM_M(float) = engine->stroker.f->getParams().getDashOffset();

      return ERR_OK;
    }

    case PAINTER_PARAMETER_DASH_OFFSET_D:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_F)
        _PARAM_M(double) = engine->stroker.f->getParams().getDashOffset();
      else
        _PARAM_M(double) = engine->stroker.d->getParams().getDashOffset();

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-List]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_LIST_F:
    {
      List<float>& v = _PARAM_M(List<float>);

      if (engine->strokerPrecision == RASTER_PRECISION_D)
        return v.setList(engine->stroker.d->getParams().getDashList());
      else
        return v.setList(engine->stroker.f->getParams().getDashList());
    }

    case PAINTER_PARAMETER_DASH_LIST_D:
    {
      List<double>& v = _PARAM_M(List<double>);

      if (engine->strokerPrecision == RASTER_PRECISION_F)
        return v.setList(engine->stroker.f->getParams().getDashList());
      else
        return v.setList(engine->stroker.d->getParams().getDashList());
    }

    // ------------------------------------------------------------------------
    // [Filter - Scale]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_FILTER_SCALE_F:
    {
      ImageFilterScaleF& v = _PARAM_M(ImageFilterScaleF);
      v.setFilterScale(engine->ctx.filterScale);

      return ERR_OK;
    }

    case PAINTER_PARAMETER_FILTER_SCALE_D:
    {
      ImageFilterScaleD& v = _PARAM_M(ImageFilterScaleD);
      v.setFilterScale(engine->ctx.filterScale);

      return ERR_OK;
    }

    default:
    {
      return ERR_RT_INVALID_ARGUMENT;
    }
  }

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_setParameter(Painter* self, uint32_t parameterId, const void* value)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  float vFloat;

  switch (parameterId)
  {
    // ------------------------------------------------------------------------
    // [Backend]
    // ------------------------------------------------------------------------

    // These values can't be changed after the engine is initialized.
    case PAINTER_PARAMETER_SIZE_I:
    case PAINTER_PARAMETER_SIZE_F:
    case PAINTER_PARAMETER_SIZE_D:
    case PAINTER_PARAMETER_FORMAT_I:
    case PAINTER_PARAMETER_DEVICE_I:
    {
      return ERR_RT_INVALID_ARGUMENT;
    }

    // ------------------------------------------------------------------------
    // [Multithreading]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_MULTITHREADED_I:
    {
      uint32_t v = _PARAM_C(uint32_t);

      // TODO:
      return ERR_OK;
    }

    case PAINTER_PARAMETER_MAX_THREADS_I:
    {
      uint32_t v = _PARAM_C(uint32_t);

      if (v == 0)
        return ERR_RT_INVALID_ARGUMENT;

      if (v > RASTER_MAX_THREADS_LIMIT)
        v = RASTER_MAX_THREADS_LIMIT;

      engine->maxThreads = v;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_PAINT_PARAMS_F:
    {
      const PaintParamsF& v = _PARAM_C(PaintParamsF);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->masterFlags &= ~(RASTER_NO_PAINT_COMPOSITING_OPERATOR |
                               RASTER_NO_PAINT_OPACITY              |
                               RASTER_NO_PAINT_STROKE               );

      engine->ctx.paintHints = v._hints;
      engine->ctx.rasterHints.opacity = Math::iround(v._opacity * engine->ctx.fullOpacity.f);
      engine->opacityF = v._opacity;

      engine->stroker.f->_params() = v._strokeParams;
      engine->stroker.f->_isDirty = true;

      if (RasterUtil::isCompositeNopOp(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      if (engine->ctx.rasterHints.opacity == 0)
        engine->masterFlags |= RASTER_NO_PAINT_OPACITY;

      if (!engine->ctx.paintHints.geometricPrecision)
      {
        engine->strokerPrecision = RASTER_PRECISION_F;
        engine->stroker.d->_params->setHints(v._strokeParams.getHints());
      }
      else
      {
        engine->strokerPrecision = RASTER_PRECISION_BOTH;
        engine->stroker.d->_params() = v._strokeParams;
        engine->stroker.d->_isDirty = true;
      }

      engine->masterFlags |=
        RASTER_PENDING_OPACITY       |
        RASTER_PENDING_HINTS         |
        RASTER_PENDING_STROKE_PARAMS ;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_PAINT_PARAMS_D:
    {
      const PaintParamsD& v = _PARAM_C(PaintParamsD);
      float opacity = (float)v._opacity;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->masterFlags &= ~(RASTER_NO_PAINT_COMPOSITING_OPERATOR |
                               RASTER_NO_PAINT_OPACITY              |
                               RASTER_NO_PAINT_STROKE               );

      engine->ctx.paintHints = v._hints;
      engine->ctx.rasterHints.opacity = Math::iround(opacity * engine->ctx.fullOpacity.f);
      engine->opacityF = opacity;

      engine->strokerPrecision = RASTER_PRECISION_D;
      engine->stroker.f->_params->setHints(v._strokeParams.getHints());
      engine->stroker.f->_isDirty = true;

      engine->stroker.d->_params() = v._strokeParams;
      engine->stroker.d->_isDirty = true;

      if (RasterUtil::isCompositeNopOp(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      if (engine->ctx.rasterHints.opacity == 0)
        engine->masterFlags |= RASTER_NO_PAINT_OPACITY;

      engine->masterFlags |=
        RASTER_PENDING_OPACITY       |
        RASTER_PENDING_HINTS         |
        RASTER_PENDING_STROKE_PARAMS ;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Hints]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_PAINT_HINTS:
    {
      const PaintHints& v = _PARAM_C(PaintHints);

      engine->masterFlags &= ~RASTER_NO_PAINT_COMPOSITING_OPERATOR;
      engine->ctx.paintHints = v;

      if (RasterUtil::isCompositeNopOp(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      engine->masterFlags |= RASTER_PENDING_HINTS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_COMPOSITING_OPERATOR_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= COMPOSITE_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->masterFlags &= ~RASTER_NO_PAINT_COMPOSITING_OPERATOR;
      engine->ctx.paintHints.compositingOperator = v;

      if (RasterUtil::isCompositeNopOp(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      engine->masterFlags |= RASTER_PENDING_HINTS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_RENDER_QUALITY_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= RENDER_QUALITY_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.renderQuality = v;
      engine->masterFlags |= RASTER_PENDING_HINTS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_IMAGE_QUALITY_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= IMAGE_QUALITY_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.imageQuality = v;
      engine->masterFlags |= RASTER_PENDING_HINTS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GRADIENT_QUALITY_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= GRADIENT_QUALITY_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.gradientQuality = v;
      engine->masterFlags |= RASTER_PENDING_HINTS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OUTLINED_TEXT_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= 2) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.outlinedText = v;
      engine->masterFlags |= RASTER_PENDING_HINTS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FAST_LINE_I:
    {
      uint32_t v = _PARAM_C(uint32_t);

      if (v >= 2)
        return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.fastLine = v;
      engine->masterFlags |= RASTER_PENDING_HINTS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GEOMETRIC_PRECISION_I:
    {
      uint32_t v = _PARAM_C(uint32_t);

      if (v >= GEOMETRIC_PRECISION_COUNT)
        return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.geometricPrecision = v;
      engine->masterFlags |= RASTER_PENDING_HINTS;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Opacity]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_OPACITY_F:
    {
      vFloat = _PARAM_C(float);

      if (vFloat < 0.0f || vFloat > 1.0f)
        return ERR_RT_INVALID_ARGUMENT;

_GlobalOpacityF:
      engine->masterFlags &= ~RASTER_NO_PAINT_OPACITY;
      engine->opacityF = vFloat;
      engine->ctx.rasterHints.opacity = Math::iround(vFloat * engine->ctx.fullOpacity.f);

      if (engine->ctx.rasterHints.opacity == 0)
        engine->masterFlags |= RASTER_NO_PAINT_OPACITY;

      engine->masterFlags |= RASTER_PENDING_OPACITY;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OPACITY_D:
    {
      double v = _PARAM_C(double);

      if (v < 0.0 || v > 1.0)
        return ERR_RT_INVALID_ARGUMENT;

      vFloat = (float)v;
      goto _GlobalOpacityF;
    }

    // ------------------------------------------------------------------------
    // [Fill Params - Fill Rule]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_FILL_RULE_I:
    {
      uint32_t v = _PARAM_C(uint32_t);

      if (FOG_UNLIKELY(v >= FILL_RULE_COUNT))
        return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.fillRule = v;
      engine->masterFlags |= RASTER_PENDING_HINTS;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_STROKE_PARAMS_F:
    {
      const PathStrokerParamsF& v = _PARAM_C(PathStrokerParamsF);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->strokerPrecision = RASTER_PRECISION_F;
      engine->stroker.f->_params() = v;
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setHints(v.getHints());
      engine->stroker.d->_isDirty = true;

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_STROKE_PARAMS_D:
    {
      const PathStrokerParamsD& v = _PARAM_C(PathStrokerParamsD);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->strokerPrecision = RASTER_PRECISION_D;
      engine->stroker.f->_params->setHints(v.getHints());
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params() = v;
      engine->stroker.d->_isDirty = true;

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Width]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_WIDTH_F:
    {
      float v = _PARAM_C(float);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setLineWidth(v);
      engine->stroker.f->_isDirty = true;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision)
            break;

          engine->strokerPrecision = RASTER_PRECISION_BOTH;
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          engine->stroker.d->_params->setLineWidth(v);
          engine->stroker.d->_isDirty = true;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_WIDTH_D:
    {
      double v = _PARAM_C(double);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.d->_params->setLineWidth(v);
      engine->stroker.d->_isDirty = true;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_NONE:
          // Initialize to 'Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_D;
          break;

        case RASTER_PRECISION_D:
          break;

        case RASTER_PRECISION_F:
          FOG_RETURN_ON_ERROR(engine->stroker.d->_params->_dashList.setList(engine->stroker.f->_params->_dashList));

          // Initialize to 'Float & Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_BOTH;

          // Convert the 'Float' parameters into the 'Double' ones.
          engine->stroker.d->_params->setMiterLimit(engine->stroker.f->_params->_miterLimit);
          engine->stroker.d->_params->setDashOffset(engine->stroker.f->_params->_dashOffset);

          engine->stroker.f->_params->setLineWidth((float)v);
          engine->stroker.f->_isDirty = true;
          break;

        case RASTER_PRECISION_BOTH:
          engine->stroker.f->_params->setLineWidth((float)v);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Join]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_JOIN_I:
    {
      uint32_t v = _PARAM_C(uint32_t);

      if (v >= LINE_JOIN_COUNT)
        return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setLineJoin(v);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setLineJoin(v);
      engine->stroker.d->_isDirty = true;

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Caps]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_START_CAP_I:
    {
      uint32_t v = _PARAM_C(uint32_t);

      if (v >= LINE_CAP_COUNT)
        return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setStartCap(v);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setStartCap(v);
      engine->stroker.d->_isDirty = true;

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_END_CAP_I:
    {
      uint32_t v = _PARAM_C(uint32_t);

      if (v >= LINE_CAP_COUNT)
        return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setEndCap(v);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setEndCap(v);
      engine->stroker.d->_isDirty = true;

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_CAPS_I:
    {
      uint32_t v = _PARAM_C(uint32_t);

      if (v >= LINE_CAP_COUNT)
        return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setLineCaps(v);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setLineCaps(v);
      engine->stroker.d->_isDirty = true;

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Miter-Limit]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_MITER_LIMIT_F:
    {
      float v = _PARAM_C(float);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setMiterLimit(v);
      engine->stroker.f->_isDirty = true;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision)
            break;

          engine->strokerPrecision = RASTER_PRECISION_BOTH;
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          engine->stroker.d->_params->setMiterLimit(v);
          engine->stroker.d->_isDirty = true;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_MITER_LIMIT_D:
    {
      double v = _PARAM_C(double);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.d->_params->setMiterLimit(v);
      engine->stroker.d->_isDirty = true;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_D:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_D;
          break;

        case RASTER_PRECISION_F:
          FOG_RETURN_ON_ERROR(engine->stroker.d->_params->_dashList.setList(engine->stroker.f->_params->_dashList));

          // Initialize to 'Float & Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_BOTH;

          // Convert the 'Float' parameters into the 'Double' ones.
          engine->stroker.d->_params->setLineWidth(engine->stroker.f->_params->_lineWidth);
          engine->stroker.d->_params->setDashOffset(engine->stroker.f->_params->_dashOffset);
          engine->stroker.f->_params->setMiterLimit((float)v);
          break;

        case RASTER_PRECISION_BOTH:
          engine->stroker.f->_params->setMiterLimit((float)v);
          engine->stroker.f->_isDirty = true;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-Offset]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_OFFSET_F:
    {
      float v = _PARAM_C(float);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setDashOffset(v);
      engine->stroker.f->_isDirty = true;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision)
            break;
          engine->strokerPrecision = RASTER_PRECISION_BOTH;
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          engine->stroker.d->_params->setDashOffset(v);
          engine->stroker.d->_isDirty = true;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_DASH_OFFSET_D:
    {
      double v = _PARAM_C(double);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.d->_params->setDashOffset(v);
      engine->stroker.d->_isDirty = true;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_D:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_D;
          break;

        case RASTER_PRECISION_F:
          FOG_RETURN_ON_ERROR(engine->stroker.d->_params->_dashList.setList(engine->stroker.f->_params->_dashList));

          // Initialize to 'Float & Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_BOTH;

          // Convert the 'Float' parameters into the 'Double' ones.
          engine->stroker.d->_params->setLineWidth(engine->stroker.f->_params->_lineWidth);
          engine->stroker.d->_params->setMiterLimit(engine->stroker.f->_params->_miterLimit);

          engine->stroker.f->_params->setDashOffset((float)v);
          break;

        case RASTER_PRECISION_BOTH:
          engine->stroker.f->_params->setDashOffset((float)v);
          engine->stroker.f->_isDirty = true;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-List]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_LIST_F:
    {
      const List<float>& v = _PARAM_C(List<float>);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setDashList(v);
      engine->stroker.f->_isDirty = true;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision)
            break;

          engine->strokerPrecision = RASTER_PRECISION_BOTH;
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          FOG_RETURN_ON_ERROR(engine->stroker.d->_params->_dashList.setList(v));
          engine->stroker.d->_isDirty = true;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_DASH_LIST_D:
    {
      const List<double>& v = _PARAM_C(List<double>);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.d->_params->setDashList(v);
      engine->stroker.d->_isDirty = true;

      switch (engine->strokerPrecision)
      {
        case RASTER_PRECISION_D:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_D;
          break;

        case RASTER_PRECISION_F:
          FOG_RETURN_ON_ERROR(engine->stroker.f->_params->_dashList.setList(v));

          // Initialize to 'Float & Double' precision.
          engine->strokerPrecision = RASTER_PRECISION_BOTH;

          // Convert the 'Float' parameters into the 'Double' ones.
          engine->stroker.d->_params->setLineWidth(engine->stroker.f->_params->_lineWidth);
          engine->stroker.d->_params->setMiterLimit(engine->stroker.f->_params->_miterLimit);
          engine->stroker.d->_params->setDashOffset(engine->stroker.f->_params->_dashOffset);
          break;

        case RASTER_PRECISION_BOTH:
          FOG_RETURN_ON_ERROR(engine->stroker.f->_params->_dashList.setList(v));
          engine->stroker.f->_isDirty = true;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      engine->masterFlags |= RASTER_PENDING_STROKE_PARAMS;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Filter - Scale]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_FILTER_SCALE_F:
    {
      const ImageFilterScaleF& v = _PARAM_C(ImageFilterScaleF);

      if ((engine->savedStateFlags & RASTER_STATE_FILTER) == 0)
        engine->saveFilter();

      engine->ctx.filterScale.setFilterScale(v);
      engine->masterFlags |= RASTER_PENDING_FILTER_PARAMS;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FILTER_SCALE_D:
    {
      const ImageFilterScaleD& v = _PARAM_C(ImageFilterScaleD);
      
      if ((engine->savedStateFlags & RASTER_STATE_FILTER) == 0)
        engine->saveFilter();
      
      engine->ctx.filterScale.setFilterScale(v);
      engine->masterFlags |= RASTER_PENDING_FILTER_PARAMS;
      return ERR_OK;
    }

    default:
    {
      return ERR_RT_INVALID_ARGUMENT;
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_resetParameter(Painter* self, uint32_t parameterId)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  switch (parameterId)
  {
    // ------------------------------------------------------------------------
    // [Backend]
    // ------------------------------------------------------------------------

    // These values can't be changed after the engine is initialized.
    case PAINTER_PARAMETER_SIZE_I:
    case PAINTER_PARAMETER_SIZE_F:
    case PAINTER_PARAMETER_SIZE_D:
    case PAINTER_PARAMETER_FORMAT_I:
    case PAINTER_PARAMETER_DEVICE_I:
    {
      return ERR_RT_INVALID_ARGUMENT;
    }

    // ------------------------------------------------------------------------
    // [Multithreading]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_MULTITHREADED_I:
    {
      // TODO: ...
      return ERR_RT_NOT_IMPLEMENTED;
    }

    case PAINTER_PARAMETER_MAX_THREADS_I:
    {
      // TODO: ...
      return ERR_RT_NOT_IMPLEMENTED;
    }

    // ------------------------------------------------------------------------
    // [Paint Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_PAINT_PARAMS_F:
    case PAINTER_PARAMETER_PAINT_PARAMS_D:
    {
      // TODO:
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Hints]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_PAINT_HINTS:
    {
      // TODO:
      return ERR_OK;
    }

    case PAINTER_PARAMETER_COMPOSITING_OPERATOR_I:
    {
      engine->masterFlags &= ~(RASTER_NO_PAINT_COMPOSITING_OPERATOR);
      engine->ctx.paintHints.compositingOperator = COMPOSITE_SRC_OVER;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_RENDER_QUALITY_I:
    {
      engine->ctx.paintHints.renderQuality = RENDER_QUALITY_DEFAULT;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_IMAGE_QUALITY_I:
    {
      engine->ctx.paintHints.imageQuality = IMAGE_QUALITY_DEFAULT;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GRADIENT_QUALITY_I:
    {
      engine->ctx.paintHints.gradientQuality = GRADIENT_QUALITY_NORMAL;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OUTLINED_TEXT_I:
    {
      engine->ctx.paintHints.outlinedText = false;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FAST_LINE_I:
    {
      engine->ctx.paintHints.fastLine = true;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GEOMETRIC_PRECISION_I:
    {
      engine->ctx.paintHints.geometricPrecision = false;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Opacity]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_OPACITY_F:
    case PAINTER_PARAMETER_OPACITY_D:
    {
      engine->masterFlags &= ~(RASTER_NO_PAINT_OPACITY);
      engine->opacityF = 1.0f;
      engine->ctx.rasterHints.opacity = engine->ctx.fullOpacity.u;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Fill Params - Fill Rule]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_FILL_RULE_I:
    {
      engine->ctx.paintHints.fillRule = FILL_RULE_DEFAULT;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_STROKE_PARAMS_F:
    case PAINTER_PARAMETER_STROKE_PARAMS_D:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->masterFlags &= ~RASTER_NO_PAINT_STROKE;

      engine->stroker.f->_params->resetHints();
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->resetHints();
      engine->stroker.d->_isDirty = true;

      if (engine->strokerPrecision & RASTER_PRECISION_F)
        engine->stroker.f->_params->reset();

      if (engine->strokerPrecision & RASTER_PRECISION_D)
        engine->stroker.d->_params->reset();

      engine->strokerPrecision = RASTER_PRECISION_NONE;

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Width]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_WIDTH_F:
    case PAINTER_PARAMETER_LINE_WIDTH_D:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_NONE)
        return ERR_OK;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setLineWidth(1.0f);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setLineWidth(1.0);
      engine->stroker.d->_isDirty = true;

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Join]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_JOIN_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setStartCap(LINE_JOIN_DEFAULT);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setStartCap(LINE_JOIN_DEFAULT);
      engine->stroker.d->_isDirty = true;

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Caps]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_START_CAP_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setStartCap(LINE_CAP_DEFAULT);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setStartCap(LINE_CAP_DEFAULT);
      engine->stroker.d->_isDirty = true;

      return ERR_OK;
    }

    case PAINTER_PARAMETER_END_CAP_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setEndCap(LINE_CAP_DEFAULT);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setEndCap(LINE_CAP_DEFAULT);
      engine->stroker.d->_isDirty = true;

      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_CAPS_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setLineCaps(LINE_CAP_DEFAULT);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setLineCaps(LINE_CAP_DEFAULT);
      engine->stroker.d->_isDirty = true;

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Miter-Limit]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_MITER_LIMIT_F:
    case PAINTER_PARAMETER_MITER_LIMIT_D:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_NONE)
        return ERR_OK;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setMiterLimit(4.0f);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setMiterLimit(4.0);
      engine->stroker.d->_isDirty = true;

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-Offset]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_OFFSET_F:
    case PAINTER_PARAMETER_DASH_OFFSET_D:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_NONE)
        return ERR_OK;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->setDashOffset(1.0f);
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->setDashOffset(1.0);
      engine->stroker.d->_isDirty = true;

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-List]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_LIST_F:
    case PAINTER_PARAMETER_DASH_LIST_D:
    {
      if (engine->strokerPrecision == RASTER_PRECISION_NONE)
        return ERR_OK;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0)
        engine->saveStroke();

      engine->stroker.f->_params->_dashList.clear();
      engine->stroker.f->_isDirty = true;
      engine->stroker.d->_params->_dashList.clear();
      engine->stroker.d->_isDirty = true;

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Filter - Scale]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_FILTER_SCALE_F:
    case PAINTER_PARAMETER_FILTER_SCALE_D:
    {
      if ((engine->savedStateFlags & RASTER_STATE_FILTER) == 0)
        engine->saveFilter();

      engine->ctx.filterScale.reset();

      return ERR_OK;
    }

    default:
    {
      return ERR_RT_INVALID_ARGUMENT;
    }
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Source]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_getSourceType(const Painter* self, uint32_t* val)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  uint32_t patternType = RASTER_SOURCE_NONE;

  switch (engine->sourceType)
  {
    case RASTER_SOURCE_NONE    : patternType = PATTERN_TYPE_NULL    ; break;
    case RASTER_SOURCE_ARGB32  : patternType = PATTERN_TYPE_COLOR   ; break;
    case RASTER_SOURCE_COLOR   : patternType = PATTERN_TYPE_COLOR   ; break;
    case RASTER_SOURCE_TEXTURE : patternType = PATTERN_TYPE_TEXTURE ; break;
    case RASTER_SOURCE_GRADIENT: patternType = PATTERN_TYPE_GRADIENT; break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  *val = patternType;
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_getSourceColor(const Painter* self, Color* color)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  switch (engine->sourceType)
  {
    case RASTER_SOURCE_ARGB32:
      color->setArgb32(engine->source.color->_argb32);
      return ERR_OK;

    case RASTER_SOURCE_COLOR:
      color->setColor(engine->source.color);
      return ERR_OK;

    default:
      color->reset();
      return ERR_RT_INVALID_STATE;
  }
}

static err_t FOG_CDECL RasterPaintEngine_getSourcePattern(const Painter* self, Pattern* pattern)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  switch (engine->sourceType)
  {
    case RASTER_SOURCE_NONE:
      pattern->reset();
      return ERR_OK;

    case RASTER_SOURCE_ARGB32:
      return pattern->createColor(engine->source.color->_argb32);

    case RASTER_SOURCE_COLOR:
      return pattern->createColor(engine->source.color);

    case RASTER_SOURCE_TEXTURE:
      return pattern->createTexture(engine->source.texture, engine->source.transform);

    case RASTER_SOURCE_GRADIENT:
      return pattern->createGradient(engine->source.gradient, engine->source.transform);

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static err_t FOG_CDECL RasterPaintEngine_setSourceNone(Painter* self)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  engine->saveSourceAndDiscard();
  engine->masterFlags |= RASTER_NO_PAINT_SOURCE;
  engine->sourceType = RASTER_SOURCE_NONE;
  engine->ctx.pc = (RasterPattern*)(size_t)0x1;

  engine->masterFlags |= RASTER_PENDING_SOURCE;
  return ERR_OK;
}

template<int _PRECISION>
static err_t FOG_CDECL RasterPaintEngine_setSourceArgb32(Painter* self, uint32_t argb32)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  FOG_ASSERT(engine->ctx.precision == _PRECISION);

  switch (_PRECISION)
  {
    case IMAGE_PRECISION_BYTE:
    {
      if (engine->sourceType != RASTER_SOURCE_ARGB32)
      {
        engine->saveSourceAndDiscard();
        engine->sourceType = RASTER_SOURCE_ARGB32;
        engine->ctx.pc = (RasterPattern*)(size_t)0x1;
      }
      else
      {
        engine->saveSourceArgb32();
      }

      engine->source.color->_argb32.u32 = argb32;
      Face::p32PRGB32FromARGB32(engine->ctx.solid.prgb32.u32, argb32);
      break;
    }

    case IMAGE_PRECISION_WORD:
    {
      uint32_t prgb32;

      if (engine->sourceType != RASTER_SOURCE_COLOR)
      {
        engine->saveSourceAndDiscard();
        engine->sourceType = RASTER_SOURCE_COLOR;
        engine->ctx.pc = (RasterPattern*)(size_t)0x1;
      }
      else
      {
        engine->saveSourceColor();
      }

      engine->source.color.initCustom1(Argb32(argb32));
      Face::p32PRGB32FromARGB32(prgb32, argb32);
      Face::p64PRGB64FromPRGB32(engine->ctx.solid.prgb64.p64, prgb32);
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  engine->masterFlags |= RASTER_PENDING_SOURCE;
  return ERR_OK;
}

template<int _PRECISION>
static err_t FOG_CDECL RasterPaintEngine_setSourceArgb64(Painter* self, const Argb64* argb64)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  FOG_ASSERT(engine->ctx.precision == _PRECISION);

  if (engine->sourceType != RASTER_SOURCE_COLOR)
  {
    engine->saveSourceAndDiscard();
    engine->sourceType = RASTER_SOURCE_COLOR;
    engine->ctx.pc = (RasterPattern*)(size_t)0x1;
  }
  else
  {
    engine->saveSourceColor();
  }

  switch (_PRECISION)
  {
    case IMAGE_PRECISION_BYTE:
      engine->source.color.initCustom1(*argb64);
      Face::p32PRGB32FromARGB32(engine->ctx.solid.prgb32.u32, engine->source.color->_argb32.u32);
      break;

    case IMAGE_PRECISION_WORD:
      engine->source.color.initCustom1(*argb64);
      Face::p64PRGB64FromARGB64(engine->ctx.solid.prgb64.p64, argb64->p64);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_OK;
}

template<int _PRECISION>
static err_t FOG_CDECL RasterPaintEngine_setSourceColor(Painter* self, const Color* color)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  FOG_ASSERT(engine->ctx.precision == _PRECISION);

  if (engine->sourceType != RASTER_SOURCE_COLOR)
  {
    engine->saveSourceAndDiscard();
    engine->sourceType = RASTER_SOURCE_COLOR;
    engine->ctx.pc = (RasterPattern*)(size_t)0x1;
  }
  else
  {
    engine->saveSourceColor();
  }

  if (!color->isValid())
    goto _Invalid;

  switch (_PRECISION)
  {
    case IMAGE_PRECISION_BYTE:
      engine->source.color.initCustom1(*color);
      Face::p32PRGB32FromARGB32(engine->ctx.solid.prgb32.u32, color->_argb32.u32);
      break;

    case IMAGE_PRECISION_WORD:
      engine->source.color.initCustom1(*color);
      Face::p64PRGB64FromARGB64(engine->ctx.solid.prgb64.p64, color->getArgb64().p64);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  engine->masterFlags |= RASTER_PENDING_SOURCE;
  return ERR_OK;

_Invalid:
  engine->sourceType = RASTER_SOURCE_NONE;
  engine->masterFlags |= RASTER_NO_PAINT_SOURCE | RASTER_PENDING_SOURCE;
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setSourcePattern(Painter* self, const Pattern* pattern)
{
  PatternData* d = pattern->_d;

  switch (d->vType & VAR_TYPE_MASK)
  {
    case VAR_TYPE_NULL:
      return self->_vtable->setSourceNone(self);

    case VAR_TYPE_COLOR:
      return self->_vtable->setSourceColor(self, &static_cast<PatternColorData*>(d)->color);

    case VAR_TYPE_PATTERN:
      switch (d->pType)
      {
        case PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_F:
          return self->_vtable->setSourceAbstract(self, PAINTER_SOURCE_TEXTURE_F,
            &reinterpret_cast<PatternTextureDataF*>(d)->texture,
            &reinterpret_cast<PatternTextureDataF*>(d)->transform);
          break;

        case PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_D:
          return self->_vtable->setSourceAbstract(self, PAINTER_SOURCE_TEXTURE_D,
            &reinterpret_cast<PatternTextureDataD*>(d)->texture,
            &reinterpret_cast<PatternTextureDataD*>(d)->transform);
          break;

        case PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_F:
          return self->_vtable->setSourceAbstract(self, PAINTER_SOURCE_GRADIENT_F,
            &reinterpret_cast<PatternGradientDataF*>(d)->gradient,
            &reinterpret_cast<PatternGradientDataF*>(d)->transform);
          break;

        case PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_D:
          return self->_vtable->setSourceAbstract(self, PAINTER_SOURCE_GRADIENT_D,
            &reinterpret_cast<PatternGradientDataD*>(d)->gradient,
            &reinterpret_cast<PatternGradientDataD*>(d)->transform);
          break;
      }
      break;
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL RasterPaintEngine_setSourceAbstract(Painter* self, uint32_t sourceId, const void* value, const void* tr)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  engine->saveSourceAndDiscard();
  switch (sourceId)
  {
    // ------------------------------------------------------------------------
    // [Texture]
    // ------------------------------------------------------------------------

    case PAINTER_SOURCE_TEXTURE_F:
    case PAINTER_SOURCE_TEXTURE_D:
    {
      const Texture& v = _PARAM_C(Texture);
      if (v._image.isEmpty())
        goto _Invalid;

      engine->sourceType = RASTER_SOURCE_TEXTURE;
      engine->source.texture.initCustom1(v);
      engine->ctx.pc = NULL;

      if (tr)
      {
        if (sourceId == PAINTER_SOURCE_TEXTURE_F)
          engine->source.transform() = *reinterpret_cast<const TransformF*>(tr);
        else
          engine->source.transform() = *reinterpret_cast<const TransformD*>(tr);
        goto _HasTransform;
      }
      else
      {
        engine->source.transform->reset();
        goto _NoTransform;
      }
    }

    // ------------------------------------------------------------------------
    // [Gradient]
    // ------------------------------------------------------------------------

    case PAINTER_SOURCE_GRADIENT_F:
    {
      const GradientF& gradient = _PARAM_C(GradientF);

      engine->sourceType = RASTER_SOURCE_GRADIENT;
      engine->source.gradient.initCustom1(gradient);
      engine->ctx.pc = NULL;

      if (tr)
      {
        engine->source.transform() = *reinterpret_cast<const TransformF*>(tr);
        goto _HasTransform;
      }
      else
      {
        engine->source.transform->reset();
        goto _NoTransform;
      }

      engine->masterFlags |= RASTER_PENDING_SOURCE;
      return ERR_OK;
    }

    case PAINTER_SOURCE_GRADIENT_D:
    {
      const GradientD& gradient = _PARAM_C(GradientD);

      engine->sourceType = RASTER_SOURCE_GRADIENT;
      engine->source.gradient.initCustom1(gradient);
      engine->ctx.pc = NULL;

      if (tr)
      {
        engine->source.transform() = *reinterpret_cast<const TransformD*>(tr);
        goto _HasTransform;
      }
      else
      {
        engine->source.transform->reset();
        goto _NoTransform;
      }

      engine->masterFlags |= RASTER_PENDING_SOURCE;
      return ERR_OK;
    }
  }

_Invalid:
  engine->sourceType = RASTER_SOURCE_NONE;
  engine->ctx.pc = (RasterPattern*)(size_t)0x1;

  engine->masterFlags |= RASTER_NO_PAINT_SOURCE | RASTER_PENDING_SOURCE;
  return ERR_OK;

_HasTransform:
  TransformD::multiply(engine->source.adjusted, engine->source.transform, engine->getFinalTransformD());

  engine->masterFlags |= RASTER_PENDING_SOURCE;
  return ERR_OK;

_NoTransform:
  if (engine->getFinalTransformD()._getType() == TRANSFORM_TYPE_IDENTITY)
    engine->source.adjusted->reset();
  else
    engine->source.adjusted() = engine->getFinalTransformD();

  engine->masterFlags |= RASTER_PENDING_SOURCE;
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Transform]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_getTransformF(const Painter* self, TransformF* tr)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  *tr = engine->userTransformD;
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_getTransformD(const Painter* self, TransformD* tr)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  *tr = engine->userTransformD;
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setTransformF(Painter* self, const TransformF* tr)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) engine->saveTransform();

  engine->userTransformD = *tr;
  engine->changedUserTransform();
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setTransformD(Painter* self, const TransformD* tr)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) engine->saveTransform();

  engine->userTransformD = *tr;
  engine->changedUserTransform();
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_applyTransform(Painter* self, uint32_t transformOp, const void* params)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  err_t err = ERR_OK;

  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0)
    engine->saveTransform();

  err = fog_api.transformd_transform(&engine->userTransformD, transformOp, params);

  if (FOG_IS_ERROR(err))
    engine->userTransformD.reset();

  engine->changedUserTransform();
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_resetTransform(Painter* self)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0)
    engine->saveTransform();

  engine->userTransformD.reset();
  engine->changedUserTransform();
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - State]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_save(Painter* self)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  RasterState* prev = engine->state;
  RasterState* state = engine->createState();
  if (FOG_IS_NULL(state)) return ERR_RT_OUT_OF_MEMORY;

  // --------------------------------------------------------------------------
  // [Previous]
  // --------------------------------------------------------------------------

  state->prevState = prev;
  state->prevMasterFlags = engine->masterFlags;
  state->savedStateFlags = engine->savedStateFlags;

  // --------------------------------------------------------------------------
  // [Id]
  // --------------------------------------------------------------------------

  state->layerId = engine->masterLayerId;
  state->stateId = prev ? prev->stateId + 1 : 0;

  // --------------------------------------------------------------------------
  // [Always Saved / Restored]
  // --------------------------------------------------------------------------

  state->paintHints.packed = engine->ctx.paintHints.packed;
  state->rasterHints.packed = engine->ctx.rasterHints.packed;
  state->opacityF = engine->opacityF;

  // --------------------------------------------------------------------------
  // [Finalize]
  // --------------------------------------------------------------------------

  engine->state = state;
  engine->savedStateFlags = 0;

  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_restore(Painter* self)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  RasterState* state = engine->state;
  if (FOG_IS_NULL(state)) return ERR_PAINTER_NO_STATE;

  // Get the states which must be restored.
  uint32_t restoreFlags = engine->savedStateFlags;

  // --------------------------------------------------------------------------
  // [Previous]
  // --------------------------------------------------------------------------

  engine->state = state->prevState;
  engine->masterFlags = state->prevMasterFlags;
  engine->savedStateFlags = state->savedStateFlags;

  // --------------------------------------------------------------------------
  // [Always Saved / Restored]
  // --------------------------------------------------------------------------

  engine->ctx.paintHints.packed = state->paintHints.packed;
  engine->ctx.rasterHints.packed = state->rasterHints.packed;
  engine->opacityF = state->opacityF;

  // Pool here so it can be reused later.
  engine->poolState(state);

  // We can simply return if there are no flags.
  if (restoreFlags == 0) return ERR_OK;

  // ------------------------------------------------------------------------
  // [Source]
  // ------------------------------------------------------------------------

  if (restoreFlags & RASTER_STATE_SOURCE)
  {
    engine->discardSource();
    engine->sourceType = state->sourceType;

    switch (state->sourceType)
    {
      case RASTER_SOURCE_NONE:
        engine->ctx.pc = (RasterPattern*)(size_t)0x1;
        break;

      case RASTER_SOURCE_ARGB32:
        engine->source.color->_argb32.u32 = state->source.color->_argb32.u32;
        engine->ctx.solid = state->solid;
        engine->ctx.pc = (RasterPattern*)(size_t)0x1;
        break;

      case RASTER_SOURCE_COLOR:
        MemOps::copy_t<Color>(&engine->source.color, &state->source.color);
        engine->ctx.solid = state->solid;
        engine->ctx.pc = (RasterPattern*)(size_t)0x1;
        break;

      case RASTER_SOURCE_TEXTURE:
        MemOps::copy_t<Texture>(&engine->source.texture, &state->source.texture);
        goto _RestoreSourceContinue;

      case RASTER_SOURCE_GRADIENT:
        MemOps::copy_t<GradientD>(&engine->source.gradient, &state->source.gradient);
_RestoreSourceContinue:
        MemOps::copy_t<TransformD>(&engine->source.transform, &state->source.transform);
        engine->ctx.pc = state->pc;
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }

  // ------------------------------------------------------------------------
  // [Stroke]
  // ------------------------------------------------------------------------

  if (restoreFlags & RASTER_STATE_STROKE)
  {
    switch (state->strokerPrecision)
    {
      case RASTER_PRECISION_NONE:
        if (engine->strokerPrecision == RASTER_PRECISION_NONE)
          break;

        engine->stroker.f->_params->reset();
        engine->stroker.d->_params->reset();
        break;

      case RASTER_PRECISION_F:
        engine->stroker.f->_params() = state->strokeParams.f();
        engine->stroker.d->_params->setHints(engine->stroker.f->_params->getHints());

        state->strokeParams.f.destroy();

        if (engine->strokerPrecision == RASTER_PRECISION_F)
          break;

        engine->stroker.d->_params->reset();
        break;

      case RASTER_PRECISION_D:
        engine->stroker.d->_params() = state->strokeParams.d();
        engine->stroker.f->_params->setHints(engine->stroker.d->_params->getHints());

        state->strokeParams.d.destroy();

        if (engine->strokerPrecision == RASTER_PRECISION_D)
          break;

        engine->stroker.f->_params->reset();
        break;

      case RASTER_PRECISION_BOTH:
        engine->stroker.f->_params->setHints(engine->stroker.f->_params->getHints());
        engine->stroker.d->_params->setHints(engine->stroker.d->_params->getHints());

        state->strokeParams.f.destroy();
        state->strokeParams.d.destroy();
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    engine->stroker.f->_isDirty = true;
    engine->stroker.d->_isDirty = true;
    engine->strokerPrecision = state->strokerPrecision;
  }

  // ------------------------------------------------------------------------
  // [Transform]
  // ------------------------------------------------------------------------

  if (restoreFlags & RASTER_STATE_TRANSFORM)
  {
    engine->userTransformD = state->userTransformD;

    engine->stroker.f->_transform = state->finalTransformF;
    engine->stroker.f->_isDirty = true;

    engine->stroker.d->_transform = state->finalTransformD;
    engine->stroker.d->_isDirty = true;

    engine->integralTransformType = state->integralTransformType;
    engine->integralTransform._sx = state->integralTransform._sx;
    engine->integralTransform._sy = state->integralTransform._sy;
    engine->integralTransform._tx = state->integralTransform._tx;
    engine->integralTransform._ty = state->integralTransform._ty;
  }

  // ------------------------------------------------------------------------
  // [Clipping]
  // ------------------------------------------------------------------------

  if (restoreFlags & RASTER_STATE_CLIPPING)
  {
    switch (state->clipType)
    {
      case RASTER_CLIP_BOX:
        if (engine->ctx.clipType == RASTER_CLIP_MASK)
        {
          // TODO: RasterPaintEngine - clip-mask.
        }

        engine->ctx.clipType = state->clipType;
        engine->ctx.clipBoxI = state->clipBoxI;
        engine->stroker.f().setClipBox(state->clipBoxF);
        engine->stroker.d().setClipBox(state->clipBoxD);

        // TODO:
        engine->ctx.clipRegion.clear();
        break;

      case RASTER_CLIP_REGION:
        if (engine->ctx.clipType == RASTER_CLIP_MASK)
        {
          // TODO: RasterPaintEngine - clip-mask.
        }

        engine->ctx.clipType = state->clipType;
        engine->ctx.clipBoxI = state->clipBoxI;
        engine->stroker.f().setClipBox(state->clipBoxF);
        engine->stroker.d().setClipBox(state->clipBoxD);

        // TODO:
        engine->ctx.clipRegion = state->clipRegion();
        state->clipRegion.destroy();
        break;

      case RASTER_CLIP_MASK:
        // TODO: RasterPaintEngine - clip-mask.
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }

  // ------------------------------------------------------------------------
  // [Filter]
  // ------------------------------------------------------------------------

  if (restoreFlags & RASTER_STATE_FILTER)
  {
    engine->ctx.filterScale = state->filterScale();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Map]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_mapPointF(const Painter* self, uint32_t mapOp, PointF* pt)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  PointD pd(*pt);

  switch (mapOp)
  {
    case PAINTER_MAP_USER_TO_DEVICE:
      engine->userTransformD.mapPoint(pd);
      *pt = pd;
      return ERR_OK;

    case PAINTER_MAP_DEVICE_TO_USER:
      // TODO: Not optimal, should we cache the inverted matrix?
      engine->userTransformD.inverted().mapPoint(pd);
      *pt = pd;
      return ERR_OK;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

static err_t FOG_CDECL RasterPaintEngine_mapPointD(const Painter* self, uint32_t mapOp, PointD* pt)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  switch (mapOp)
  {
    case PAINTER_MAP_USER_TO_DEVICE:
      engine->userTransformD.mapPoint(*pt);
      return ERR_OK;

    case PAINTER_MAP_DEVICE_TO_USER:
      // TODO: Not optimal, should we cache the inverted matrix?
      engine->userTransformD.inverted().mapPoint(*pt);
      return ERR_OK;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - FillAll]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_fillAll(Painter* self)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  return engine->serializer->fillAll(engine);
}

// ============================================================================
// [Fog::RasterPaintEngine - Draw]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_drawRectI(Painter* self, const RectI* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    RectF rf(*r);
    return engine->vtable->drawRectF(self, &rf);
  }
  else
  {
    RectD rd(*r);
    return engine->vtable->drawRectD(self, &rd);
  }
}

static err_t FOG_CDECL RasterPaintEngine_drawRectF(Painter* self, const RectF* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathF* path = &engine->ctx.tmpPathF[2];
  path->clear();
  path->rect(*r);
  return engine->serializer->fillStrokedPathF(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawRectD(Painter* self, const RectD* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathD* path = &engine->ctx.tmpPathD[2];
  path->clear();
  path->rect(*r);
  return engine->serializer->fillStrokedPathD(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawPolylineI(Painter* self, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF* path = &engine->ctx.tmpPathF[2];
    path->clear();
    path->polyline(p, count);
    return engine->serializer->fillStrokedPathF(engine, path);
  }
  else
  {
    PathD* path = &engine->ctx.tmpPathD[2];
    path->clear();
    path->polyline(p, count);
    return engine->serializer->fillStrokedPathD(engine, path);
  }
}

static err_t FOG_CDECL RasterPaintEngine_drawPolygonI(Painter* self, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF* path = &engine->ctx.tmpPathF[2];
    path->clear();
    path->polygon(p, count);
    return engine->serializer->fillStrokedPathF(engine, path);
  }
  else
  {
    PathD* path = &engine->ctx.tmpPathD[2];
    path->clear();
    path->polygon(p, count);
    return engine->serializer->fillStrokedPathD(engine, path);
  }
}

static err_t FOG_CDECL RasterPaintEngine_drawShapeF(Painter* self, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  switch (shapeType)
  {
    case SHAPE_TYPE_NONE:
    {
      return ERR_GEOMETRY_NONE;
    }
  
    case SHAPE_TYPE_PATH:
    {
      const PathF* path = reinterpret_cast<const PathF*>(shapeData);
      return engine->serializer->fillStrokedPathF(engine, path);
    }

    default:
    {
      PathF* path = &engine->ctx.tmpPathF[2];
      path->clear();
      path->_shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
      return engine->serializer->fillStrokedPathF(engine, path);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_drawShapeD(Painter* self, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  switch (shapeType)
  {
    case SHAPE_TYPE_NONE:
    {
      return ERR_GEOMETRY_NONE;
    }
  
    case SHAPE_TYPE_PATH:
    {
      const PathD* path = reinterpret_cast<const PathD*>(shapeData);
      return engine->serializer->fillStrokedPathD(engine, path);
    }

    default:
    {
      PathD* path = &engine->ctx.tmpPathD[2];
      path->clear();
      path->_shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
      return engine->serializer->fillStrokedPathD(engine, path);
    }
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Fill]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_fillRectI(Painter* self, const RectI* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (engine->isIntegralTransform())
  {
    BoxI box(UNINITIALIZED);
    if (engine->doIntegralTransformAndClip(box, *r, engine->ctx.clipBoxI))
      return engine->serializer->fillNormalizedBoxI(engine, &box);
    else
      return ERR_OK;
  }
  else
  {
    if (!engine->ctx.paintHints.geometricPrecision)
    {
      RectF rf(*r);
      return engine->vtable->fillRectF(self, &rf);
    }
    else
    {
      RectD rd(*r);
      return engine->vtable->fillRectD(self, &rd);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_fillRectF(Painter* self, const RectF* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  BoxF box(*r);
  if (engine->ensureFinalTransformF())
  {
    if (!engine->ctx.rasterHints.rectToRectTransform)
    {
      PathF* path = &engine->ctx.tmpPathF[0];
      path->clear();
      path->rect(*r, PATH_DIRECTION_CW);
      return engine->serializer->fillPathF(engine, path, FILL_RULE_NON_ZERO);
    }
    engine->getFinalTransformF().mapBox(box, box);
  }

  if (!BoxF::intersect(box, box, engine->getClipBoxF()))
    return ERR_OK;

  return engine->serializer->fillNormalizedBoxF(engine, &box);
}

static err_t FOG_CDECL RasterPaintEngine_fillRectD(Painter* self, const RectD* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  BoxD box(*r);
  if (engine->getFinalTransformD()._getType() != TRANSFORM_TYPE_IDENTITY)
  {
    if (!engine->ctx.rasterHints.rectToRectTransform)
    {
      PathD* path = &engine->ctx.tmpPathD[0];
      path->clear();
      path->rect(*r, PATH_DIRECTION_CW);
      return engine->serializer->fillPathD(engine, path, FILL_RULE_NON_ZERO);
    }
    engine->getFinalTransformD().mapBox(box, box);
  }

  if (!BoxD::intersect(box, box, engine->getClipBoxD()))
    return ERR_OK;

  return engine->serializer->fillNormalizedBoxD(engine, &box);
}

static err_t FOG_CDECL RasterPaintEngine_fillRectsI(Painter* self, const RectI* r, size_t count)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF* path = &engine->ctx.tmpPathF[0];
    path->clear();
    path->rects(r, count);
    return engine->serializer->fillPathF(engine, path, FILL_RULE_NON_ZERO);
  }
  else
  {
    PathD* path = &engine->ctx.tmpPathD[0];
    path->clear();
    path->rects(r, count);
    return engine->serializer->fillPathD(engine, path, FILL_RULE_NON_ZERO);
  }
}

static err_t FOG_CDECL RasterPaintEngine_fillPolygonI(Painter* self, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF* path = &engine->ctx.tmpPathF[0];
    path->clear();
    path->polygon(p, count, PATH_DIRECTION_CW);
    return engine->serializer->fillPathF(engine, path, engine->ctx.paintHints.fillRule);
  }
  else
  {
    PathD* path = &engine->ctx.tmpPathD[0];
    path->clear();
    path->polygon(p, count, PATH_DIRECTION_CW);
    return engine->serializer->fillPathD(engine, path, engine->ctx.paintHints.fillRule);
  }
}

static err_t FOG_CDECL RasterPaintEngine_fillShapeF(Painter* self, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  switch (shapeType)
  {
    case SHAPE_TYPE_RECT:
    {
      return self->_vtable->fillRectF(self, static_cast<const RectF*>(shapeData));
    }

    case SHAPE_TYPE_RECT_ARRAY:
    {
      const RectArrayF* rects = reinterpret_cast<const RectArrayF*>(shapeData);
      if (rects->getLength() == 1)
        return self->_vtable->fillRectF(self, rects->getData());
      else
        goto _Default;
    }

    case SHAPE_TYPE_PATH:
    {
      const PathF* path = reinterpret_cast<const PathF*>(shapeData);
      return engine->serializer->fillPathF(engine, path, engine->ctx.paintHints.fillRule);
    }

    default:
    {
_Default:
      PathF* path = &engine->ctx.tmpPathF[0];
      path->clear();
      path->_shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
      return engine->serializer->fillPathF(engine, path, engine->ctx.paintHints.fillRule);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_fillShapeD(Painter* self, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  switch (shapeType)
  {
    case SHAPE_TYPE_RECT:
    {
      return self->_vtable->fillRectD(self, static_cast<const RectD*>(shapeData));
    }

    case SHAPE_TYPE_RECT_ARRAY:
    {
      const RectArrayD* rects = reinterpret_cast<const RectArrayD*>(shapeData);
      if (rects->getLength() == 1)
        return self->_vtable->fillRectD(self, rects->getData());
      else
        goto _Default;
    }

    case SHAPE_TYPE_PATH:
    {
      const PathD* path = reinterpret_cast<const PathD*>(shapeData);
      return engine->serializer->fillPathD(engine, path, engine->ctx.paintHints.fillRule);
    }

    default:
    {
_Default:
      PathD* path = &engine->ctx.tmpPathD[0];
      path->clear();
      path->_shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
      return engine->serializer->fillPathD(engine, path, engine->ctx.paintHints.fillRule);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_fillTextAtI(Painter* self, const PointI* p, const StringW* text, const Font* font, const RectI* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextAtF(Painter* self, const PointF* p, const StringW* text, const Font* font, const RectF* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextAtD(Painter* self, const PointD* p, const StringW* text, const Font* font, const RectD* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextInI(Painter* self, const TextRectI* r, const StringW* text, const Font* font, const RectI* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextInF(Painter* self, const TextRectF* r, const StringW* text, const Font* font, const RectF* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextInD(Painter* self, const TextRectD* r, const StringW* text, const Font* font, const RectD* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskAtI(Painter* self, const PointI* p, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskAtF(Painter* self, const PointF* p, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskAtD(Painter* self, const PointD* p, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskInI(Painter* self, const RectI* r, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskInF(Painter* self, const RectF* r, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskInD(Painter* self, const RectD* r, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillRegion(Painter* self, const Region* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO: Not optimal.

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF* path = &engine->ctx.tmpPathF[0];
    path->clear();
    path->region(*r);
    return engine->serializer->fillPathF(engine, path, FILL_RULE_NON_ZERO);
  }
  else
  {
    PathD* path = &engine->ctx.tmpPathD[0];
    path->clear();
    path->region(*r);
    return engine->serializer->fillPathD(engine, path, FILL_RULE_NON_ZERO);
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Blit]
// ============================================================================

#define _FOG_RASTER_IMAGE_PARAMS(_Image_, _ImageFragment_) \
  int sX = 0; \
  int sY = 0; \
  int sW = _Image_->getWidth(); \
  int sH = _Image_->getHeight(); \
  \
  if (_ImageFragment_ != NULL) \
  { \
    if (!_ImageFragment_->isValid()) \
      return ERR_RT_INVALID_ARGUMENT; \
    \
    sX = _ImageFragment_->x; \
    sY = _ImageFragment_->y; \
    \
    if ((uint)(sX) >= (uint)sW || \
        (uint)(sY) >= (uint)sH || \
        (uint)(_ImageFragment_->w - sX) > (uint)sW || \
        (uint)(_ImageFragment_->h - sY) > (uint)sH) \
    { \
      return ERR_RT_INVALID_ARGUMENT; \
    } \
    \
    sW = _ImageFragment_->w; \
    sH = _ImageFragment_->h; \
    if (sW == 0 || sH == 0) return ERR_OK; \
  }

static err_t FOG_CDECL RasterPaintEngine_blitImageAtI(Painter* self, const PointI* p, const Image* src, const RectI* sFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  if (src->isEmpty())
    return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(src, sFragment)

  int dX = p->x, dW;
  int dY = p->y, dH;
  int t;

  switch (engine->integralTransformType)
  {
    case RASTER_INTEGRAL_TRANSFORM_NONE:
    {
      if (!engine->ctx.paintHints.geometricPrecision)
      {
        PointF pf(*p);
        return engine->vtable->blitImageAtF(self, &pf, src, sFragment);
      }
      else
      {
        PointD pd(*p);
        return engine->vtable->blitImageAtD(self, &pd, src, sFragment);
      }
    }

    case RASTER_INTEGRAL_TRANSFORM_SIMPLE:
    {
      dX += engine->integralTransform._tx;
      dY += engine->integralTransform._ty;

      if ((uint)(t = dX - engine->ctx.clipBoxI.x0) >= (uint)engine->ctx.clipBoxI.getWidth())
      {
        dX = engine->ctx.clipBoxI.x0; sX -= t;
        if (t >= 0 || (sW += t) <= 0) return ERR_OK;
      }

      if ((uint)(t = dY - engine->ctx.clipBoxI.y0) >= (uint)engine->ctx.clipBoxI.getHeight())
      {
        dY = engine->ctx.clipBoxI.y0; sY -= t;
        if (t >= 0 || (sH += t) <= 0) return ERR_OK;
      }

      if ((t = engine->ctx.clipBoxI.x1 - dX) < sW) sW = t;
      if ((t = engine->ctx.clipBoxI.y1 - dY) < sH) sH = t;

      PointI dPos(dX, dY);
      RectI sRect(sX, sY, sW, sH);
      return engine->serializer->blitNormalizedImageA(engine, &dPos, src, &sRect);
    }

    case RASTER_INTEGRAL_TRANSFORM_SCALING:
    {
      dX = dX * engine->integralTransform._sx;
      dY = dY * engine->integralTransform._sy;

      dW = sW * engine->integralTransform._sx;
      dH = sH * engine->integralTransform._sy;

_Scaling:
      dX += engine->integralTransform._tx;
      dY += engine->integralTransform._ty;

      if (dW < 0) { dX += dW; dW = -dW; }
      if (dH < 0) { dY += dH; dH = -dH; }

      // Make a box model (dstX, dstY -> dstW, dstH).
      dW += dX;
      dH += dY;

      if (dX < engine->ctx.clipBoxI.x0) dX = engine->ctx.clipBoxI.x0;
      if (dY < engine->ctx.clipBoxI.y0) dY = engine->ctx.clipBoxI.y0;

      if (dW > engine->ctx.clipBoxI.x1) dW = engine->ctx.clipBoxI.x1;
      if (dH > engine->ctx.clipBoxI.y1) dH = engine->ctx.clipBoxI.y1;

      if (dX >= dW || dY >= dH) return ERR_OK;

      TransformD tr(engine->getFinalTransformD());
      tr.translate(PointD(p->x, p->y));
      
      BoxI dBox(dX, dY, dW, dH);
      RectI sRect(sX, sY, sW, sH);
      return engine->serializer->blitNormalizedImageI(engine, &dBox, src, &sRect, &tr);
    }

    case RASTER_INTEGRAL_TRANSFORM_SWAP:
    {
      t = dX;

      dX = dY * engine->integralTransform._sx;
      dY = t  * engine->integralTransform._sy;

      dW = sH * engine->integralTransform._sx;
      dH = sW * engine->integralTransform._sy;

      goto _Scaling;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static err_t FOG_CDECL RasterPaintEngine_blitImageAtF(Painter* self, const PointF* p, const Image* src, const RectI* sFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  if (src->isEmpty())
    return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(src, sFragment)

  uint32_t transformType = engine->getFinalTransformD()._getType();
  BoxD box(double(p->x), double(p->y), double(p->x) + double(sW), double(p->y) + double(sH));

  switch (transformType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    case TRANSFORM_TYPE_TRANSLATION:
    {
      PointD t(double(p->x) + engine->getFinalTransformD()._20,
               double(p->y) + engine->getFinalTransformD()._21);

      Fixed48x16 x48x16 = Math::fixed48x16FromFloat(t.x);
      Fixed48x16 y48x16 = Math::fixed48x16FromFloat(t.y);

      // Aligned.
      if ((((int)x48x16 | (int)y48x16) & 0xFF00) == 0)
      {
        int dX = (int)(x48x16 >> 16);
        int dY = (int)(y48x16 >> 16);

        int t;

        if ((uint)(t = dX - engine->ctx.clipBoxI.x0) >= (uint)engine->ctx.clipBoxI.getWidth())
        {
          dX = engine->ctx.clipBoxI.x0; sX -= t;
          if (t >= 0 || (sW += t) <= 0) return ERR_OK;
        }

        if ((uint)(t = dY - engine->ctx.clipBoxI.y0) >= (uint)engine->ctx.clipBoxI.getHeight())
        {
          dY = engine->ctx.clipBoxI.y0; sY -= t;
          if (t >= 0 || (sH += t) <= 0) return ERR_OK;
        }

        if ((t = engine->ctx.clipBoxI.x1 - dX) < sW) sW = t;
        if ((t = engine->ctx.clipBoxI.y1 - dY) < sH) sH = t;

        PointI dPos(dX, dY);
        RectI sRect(sX, sY, sW, sH);
        return engine->serializer->blitNormalizedImageA(engine, &dPos, src, &sRect);
      }
      else
      {
        box.translate(engine->getFinalTransformD()._20,
                      engine->getFinalTransformD()._21);

        if (!BoxD::intersect(box, box, engine->getClipBoxD()))
          return ERR_OK;

        TransformD tr(engine->getFinalTransformD());
        tr.translate(PointD(p->x, p->y));

        RectI sRect(sX, sY, sW, sH);
        return engine->serializer->blitNormalizedImageD(engine, &box, src, &sRect, &tr);
      }
    }

    case TRANSFORM_TYPE_SCALING:
    {
      engine->getFinalTransformD().mapBox(box, box);

      if (!BoxD::intersect(box, box, engine->getClipBoxD()))
        return ERR_OK;

      TransformD tr(engine->getFinalTransformD());
      tr.translate(PointD(p->x, p->y));

      RectI sRect(sX, sY, sW, sH);
      return engine->serializer->blitNormalizedImageD(engine, &box, src, &sRect, &tr);
    }

    default:
    {
      BoxD transformedBox(UNINITIALIZED);
      engine->getFinalTransformD().mapBox(transformedBox, box);

      if (!BoxD::intersect(transformedBox, transformedBox, engine->getClipBoxD()))
        return ERR_OK;

      TransformD tr(engine->getFinalTransformD());
      tr.translate(PointD(p->x, p->y));

      RectI sRect(sX, sY, sW, sH);
      return engine->serializer->blitImageD(engine, &box, src, &sRect, &tr);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_blitImageAtD(Painter* self, const PointD* p, const Image* src, const RectI* sFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  if (src->isEmpty())
    return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(src, sFragment)

  uint32_t transformType = engine->getFinalTransformD()._getType();
  BoxD box(double(p->x), double(p->y), double(p->x) + double(sW), double(p->y) + double(sH));

  switch (transformType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    case TRANSFORM_TYPE_TRANSLATION:
    {
      PointD t(double(p->x) + engine->getFinalTransformD()._20,
               double(p->y) + engine->getFinalTransformD()._21);

      Fixed48x16 x48x16 = Math::fixed48x16FromFloat(t.x);
      Fixed48x16 y48x16 = Math::fixed48x16FromFloat(t.y);

      // Aligned.
      if ((((int)x48x16 | (int)y48x16) & 0xFF00) == 0)
      {
        int dX = (int)(x48x16 >> 16);
        int dY = (int)(y48x16 >> 16);

        int t;

        if ((uint)(t = dX - engine->ctx.clipBoxI.x0) >= (uint)engine->ctx.clipBoxI.getWidth())
        {
          dX = engine->ctx.clipBoxI.x0; sX -= t;
          if (t >= 0 || (sW += t) <= 0) return ERR_OK;
        }

        if ((uint)(t = dY - engine->ctx.clipBoxI.y0) >= (uint)engine->ctx.clipBoxI.getHeight())
        {
          dY = engine->ctx.clipBoxI.y0; sY -= t;
          if (t >= 0 || (sH += t) <= 0) return ERR_OK;
        }

        if ((t = engine->ctx.clipBoxI.x1 - dX) < sW) sW = t;
        if ((t = engine->ctx.clipBoxI.y1 - dY) < sH) sH = t;

        PointI dPos(dX, dY);
        RectI sRect(sX, sY, sW, sH);
        return engine->serializer->blitNormalizedImageA(engine, &dPos, src, &sRect);
      }
      else
      {
        box.translate(engine->getFinalTransformD()._20,
                      engine->getFinalTransformD()._21);

        if (!BoxD::intersect(box, box, engine->getClipBoxD()))
          return ERR_OK;

        TransformD tr(engine->getFinalTransformD());
        tr.translate(PointD(p->x, p->y));

        RectI sRect(sX, sY, sW, sH);
        return engine->serializer->blitNormalizedImageD(engine, &box, src, &sRect, &tr);
      }
    }

    case TRANSFORM_TYPE_SCALING:
    {
      engine->getFinalTransformD().mapBox(box, box);

      if (!BoxD::intersect(box, box, engine->getClipBoxD()))
        return ERR_OK;

      TransformD tr(engine->getFinalTransformD());
      tr.translate(PointD(p->x, p->y));

      RectI sRect(sX, sY, sW, sH);
      return engine->serializer->blitNormalizedImageD(engine, &box, src, &sRect, &tr);
    }

    default:
    {
      BoxD transformedBox(UNINITIALIZED);
      engine->getFinalTransformD().mapBox(transformedBox, box);

      if (!BoxD::intersect(transformedBox, transformedBox, engine->getClipBoxD()))
        return ERR_OK;

      TransformD tr(engine->getFinalTransformD());
      tr.translate(PointD(p->x, p->y));

      RectI sRect(sX, sY, sW, sH);
      return engine->serializer->blitImageD(engine, &box, src, &sRect, &tr);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_blitImageInI(Painter* self, const RectI* r, const Image* src, const RectI* sFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  if (src->isEmpty())
    return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(src, sFragment)

  // Try to use unscaled blit if possible.
  if (r->w == sW && r->h == sH)
  {
    PointI dPos(r->x, r->y);
    return engine->vtable->blitImageAtI(self, &dPos, src, sFragment);
  }

  uint32_t transformType = engine->getFinalTransformD()._getType();
  BoxD box(double(r->x), double(r->y), double(r->x) + double(r->w), double(r->y) + double(r->h));

  TransformD tr(engine->getFinalTransformD());
  double scaleX = double(r->w) / double(sW);
  double scaleY = double(r->h) / double(sH);

  TransformD scaling(scaleX, 0.0, 0.0, scaleY, r->x, r->y);
  tr.transform(scaling);

  BoxD transformedBox(UNINITIALIZED);
  engine->getFinalTransformD().mapBox(transformedBox, box);

  if (!BoxD::intersect(transformedBox, transformedBox, engine->getClipBoxD()))
    return ERR_OK;

  RectI sRect(sX, sY, sW, sH);
  if (transformType <= TRANSFORM_TYPE_SWAP)
    return engine->serializer->blitNormalizedImageD(engine, &transformedBox, src, &sRect, &tr);
  else
    return engine->serializer->blitImageD(engine, &box, src, &sRect, &tr);
}

static err_t FOG_CDECL RasterPaintEngine_blitImageInF(Painter* self, const RectF* r, const Image* src, const RectI* sFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  if (src->isEmpty())
    return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(src, sFragment)

  uint32_t transformType = engine->getFinalTransformD()._getType();
  BoxD box(double(r->x), double(r->y), double(r->x) + double(r->w), double(r->y) + double(r->h));

  TransformD tr(
    double(r->w) / double(sW), 0.0,
    0.0, double(r->h) / double(sH),
    r->x, r->y);
  tr.transform(engine->getFinalTransformD(), MATRIX_ORDER_APPEND);

  BoxD transformedBox(UNINITIALIZED);
  engine->getFinalTransformD().mapBox(transformedBox, box);

  if (!BoxD::intersect(transformedBox, transformedBox, engine->getClipBoxD()))
    return ERR_OK;

  RectI sRect(sX, sY, sW, sH);
  if (transformType <= TRANSFORM_TYPE_SWAP)
    return engine->serializer->blitNormalizedImageD(engine, &transformedBox, src, &sRect, &tr);
  else
    return engine->serializer->blitImageD(engine, &box, src, &sRect, &tr);
}

static err_t FOG_CDECL RasterPaintEngine_blitImageInD(Painter* self, const RectD* r, const Image* src, const RectI* sFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  if (src->isEmpty())
    return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(src, sFragment)

  uint32_t transformType = engine->getFinalTransformD()._getType();
  BoxD box(double(r->x), double(r->y), double(r->x) + double(r->w), double(r->y) + double(r->h));

  TransformD tr(
    double(r->w) / double(sW), 0.0,
    0.0, double(r->h) / double(sH),
    r->x, r->y);
  tr.transform(engine->getFinalTransformD(), MATRIX_ORDER_APPEND);

  BoxD transformedBox(UNINITIALIZED);
  engine->getFinalTransformD().mapBox(transformedBox, box);

  if (!BoxD::intersect(transformedBox, transformedBox, engine->getClipBoxD()))
    return ERR_OK;

  RectI sRect(sX, sY, sW, sH);
  if (transformType <= TRANSFORM_TYPE_SWAP)
    return engine->serializer->blitNormalizedImageD(engine, &transformedBox, src, &sRect, &tr);
  else
    return engine->serializer->blitImageD(engine, &box, src, &sRect, &tr);
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageAtI(Painter* self, const PointI* p, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageAtF(Painter* self, const PointF* p, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageAtD(Painter* self, const PointD* p, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageInI(Painter* self, const RectI* r, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageInF(Painter* self, const RectF* r, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageInD(Painter* self, const RectD* r, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintEngine - Filter]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_filterAll(Painter* self, const FeBase* feBase)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILTER_FUNC();

  if (feBase->getFeType() == FE_TYPE_NONE)
    return ERR_OK;

  return engine->serializer->filterNormalizedBoxI(engine, feBase, &engine->ctx.clipBoxI);
}

static err_t FOG_CDECL RasterPaintEngine_filterRectI(Painter* self, const FeBase* feBase, const RectI* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILTER_FUNC();

  if (feBase->getFeType() == FE_TYPE_NONE)
    return ERR_OK;

  if (engine->isIntegralTransform())
  {
    BoxI box(UNINITIALIZED);
    if (engine->doIntegralTransformAndClip(box, *r, engine->ctx.clipBoxI))
      return engine->serializer->filterNormalizedBoxI(engine, feBase, &box);
    else
      return ERR_OK;
  }
  else
  {
    if (!engine->ctx.paintHints.geometricPrecision)
    {
      RectF rf(*r);
      return engine->vtable->filterRectF(self, feBase, &rf);
    }
    else
    {
      RectD rd(*r);
      return engine->vtable->filterRectD(self, feBase, &rd);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_filterRectF(Painter* self, const FeBase* feBase, const RectF* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILTER_FUNC();

  if (feBase->getFeType() == FE_TYPE_NONE)
    return ERR_OK;

  BoxF box(*r);
  if (engine->ensureFinalTransformF())
  {
    if (!engine->ctx.rasterHints.rectToRectTransform)
    {
      PathF* path = &engine->ctx.tmpPathF[0];
      path->clear();
      path->rect(*r, PATH_DIRECTION_CW);
      return engine->serializer->filterPathF(engine, feBase, path, FILL_RULE_NON_ZERO);
    }
    engine->getFinalTransformF().mapBox(box, box);
  }

  if (!BoxF::intersect(box, box, engine->getClipBoxF()))
    return ERR_OK;

  return engine->serializer->filterNormalizedBoxF(engine, feBase, &box);
}

static err_t FOG_CDECL RasterPaintEngine_filterRectD(Painter* self, const FeBase* feBase, const RectD* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILTER_FUNC();

  if (feBase->getFeType() == FE_TYPE_NONE)
    return ERR_OK;

  BoxD box(*r);
  if (engine->getFinalTransformD()._getType() != TRANSFORM_TYPE_IDENTITY)
  {
    if (!engine->ctx.rasterHints.rectToRectTransform)
    {
      PathD* path = &engine->ctx.tmpPathD[0];
      path->clear();
      path->rect(*r, PATH_DIRECTION_CW);
      return engine->serializer->filterPathD(engine, feBase, path, FILL_RULE_NON_ZERO);
    }
    engine->getFinalTransformD().mapBox(box, box);
  }

  if (!BoxD::intersect(box, box, engine->getClipBoxD()))
    return ERR_OK;

  return engine->serializer->filterNormalizedBoxD(engine, feBase, &box);
}

static err_t FOG_CDECL RasterPaintEngine_filterShapeF(Painter* self, const FeBase* feBase, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILTER_FUNC();

  if (feBase->getFeType() == FE_TYPE_NONE)
    return ERR_OK;

  switch (shapeType)
  {
    case SHAPE_TYPE_NONE:
    {
      return ERR_GEOMETRY_NONE;
    }
  
    case SHAPE_TYPE_RECT:
    {
      return self->_vtable->filterRectF(self, feBase, static_cast<const RectF*>(shapeData));
    }

    case SHAPE_TYPE_RECT_ARRAY:
    {
      const RectArrayF* rects = reinterpret_cast<const RectArrayF*>(shapeData);
      if (rects->getLength() == 1)
        return engine->vtable->filterRectF(self, feBase, rects->getData());
      else
        goto _Default;
    }

    case SHAPE_TYPE_PATH:
    {
      const PathF* path = reinterpret_cast<const PathF*>(shapeData);
      return engine->serializer->filterPathF(engine, feBase, path, engine->ctx.paintHints.fillRule);
    }

    default:
    {
_Default:
      PathF* path = &engine->ctx.tmpPathF[0];
      path->clear();
      path->_shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
      return engine->serializer->filterPathF(engine, feBase, path, engine->ctx.paintHints.fillRule);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_filterShapeD(Painter* self, const FeBase* feBase, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILTER_FUNC();

  if (feBase->getFeType() == FE_TYPE_NONE)
    return ERR_OK;

  switch (shapeType)
  {
    case SHAPE_TYPE_NONE:
    {
      return ERR_GEOMETRY_NONE;
    }
  
    case SHAPE_TYPE_RECT:
    {
      return self->_vtable->filterRectD(self, feBase, static_cast<const RectD*>(shapeData));
    }

    case SHAPE_TYPE_RECT_ARRAY:
    {
      const RectArrayD* rects = reinterpret_cast<const RectArrayD*>(shapeData);
      if (rects->getLength() == 1)
        return engine->vtable->filterRectD(self, feBase, rects->getData());
      else
        goto _Default;
    }

    case SHAPE_TYPE_PATH:
    {
      const PathD* path = reinterpret_cast<const PathD*>(shapeData);
      return engine->serializer->filterPathD(engine, feBase, path, engine->ctx.paintHints.fillRule);
    }

    default:
    {
_Default:
      PathD* path = &engine->ctx.tmpPathD[0];
      path->clear();
      path->_shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
      return engine->serializer->filterPathD(engine, feBase, path, engine->ctx.paintHints.fillRule);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_filterStrokedShapeF(Painter* self, const FeBase* feBase, uint32_t shapeType, const void* shapeData)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_filterStrokedShapeD(Painter* self, const FeBase* feBase, uint32_t shapeType, const void* shapeData)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintEngine - Clip]
// ============================================================================










#if 0
// ============================================================================
// [Fog::RasterPaintSerializer - Render(st) - ClipAll]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipAll_render_st(
  RasterPaintEngine* engine)
{
  if ((engine->savedStateFlags & RASTER_STATE_CLIPPING) == 0)
    engine->saveClipping();

  engine->masterFlags |= RASTER_NO_PAINT_USER_CLIP;

  engine->ctx.clipBoxI.reset();
  engine->ctx.clipRegion.clear();
  engine->stroker.f->_clipBox.reset();
  engine->stroker.d->_clipBox.reset();

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Render(st) - ClipPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipPathF(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  PathF* tmp = &engine->ctx.tmpPathF[1];

  bool hasTransform = engine->ensureFinalTransformF();
  PathClipperF clipper(clipOp == CLIP_OP_REPLACE ? engine->getMetaClipBoxF() : engine->getClipBoxF());

  if (!hasTransform)
  {
    switch (clipper.measurePath(*path))
    {
      case PATH_CLIPPER_MEASURE_BOUNDED:
        engine->serializer->clipNormalizedPathF(engine, clipOp, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_MEASURE_UNBOUNDED:
        tmp->clear();
        FOG_RETURN_ON_ERROR(clipper.continuePath(*tmp, *path));
        engine->serializer->clipNormalizedPathF(engine, clipOp, tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp->clear();
    FOG_RETURN_ON_ERROR(clipper.clipPath(*tmp, *path, engine->getFinalTransformF()));
    engine->serializer->clipNormalizedPathF(engine, clipOp, tmp, fillRule);
    return ERR_OK;
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipPathD(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule)
{
  PathD* tmp = &engine->ctx.tmpPathD[1];

  bool hasTransform = (engine->getFinalTransformD()._getType() != TRANSFORM_TYPE_IDENTITY);
  PathClipperD clipper(clipOp == CLIP_OP_REPLACE ? engine->metaClipBoxD : engine->getClipBoxD());

  if (!hasTransform)
  {
    switch (clipper.measurePath(*path))
    {
      case PATH_CLIPPER_MEASURE_BOUNDED:
        engine->serializer->clipNormalizedPathD(engine, clipOp, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_MEASURE_UNBOUNDED:
        tmp->clear();
        FOG_RETURN_ON_ERROR(clipper.continuePath(*tmp, *path));
        engine->serializer->clipNormalizedPathD(engine, clipOp, tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp->clear();
    FOG_RETURN_ON_ERROR(clipper.clipPath(*tmp, *path, engine->getFinalTransformD()));
    engine->serializer->clipNormalizedPathD(engine, clipOp, tmp, fillRule);
    return ERR_OK;
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipStrokedPathF(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path)
{
  if (engine->strokerPrecision == RASTER_PRECISION_D)
  {
    engine->strokerPrecision = RASTER_PRECISION_BOTH;
    engine->stroker.f->_params() = engine->stroker.d->_params();
    engine->stroker.f->_isDirty = true;
  }

  PathStrokerF* stroker = &engine->stroker.f;
  PathF* tmp = &engine->ctx.tmpPathF[0];

  tmp->clear();
  stroker->strokePath(*tmp, *path);
  return engine->serializer->clipNormalizedPathF(engine, clipOp, tmp, FILL_RULE_NON_ZERO);
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipStrokedPathD(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path)
{
  if (engine->strokerPrecision == RASTER_PRECISION_F)
  {
    engine->strokerPrecision = RASTER_PRECISION_BOTH;
    engine->stroker.d->_params() = engine->stroker.f->_params();
    engine->stroker.d->_isDirty = true;
  }

  PathStrokerD* stroker = &engine->stroker.d;
  PathD* tmp = &engine->ctx.tmpPathD[0];

  tmp->clear();
  stroker->strokePath(*tmp, *path);
  return engine->serializer->clipNormalizedPathD(engine, clipOp, tmp, FILL_RULE_NON_ZERO);
}

// ============================================================================
// [Fog::RasterPaintSerializer - Render(st) - ClipNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedBoxI_render_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxI* box)
{
  FOG_ASSERT(box->isValid());

  if ((engine->savedStateFlags & RASTER_STATE_CLIPPING) == 0)
    engine->saveClipping();

  Region* newRegion;
  size_t newLength;

  switch (clipOp)
  {
    case CLIP_OP_REPLACE:
      switch (engine->ctx.clipType)
      {
        case RASTER_CLIP_BOX:
          // RasterPaintEngine always clips only to clip-box, if there is a
          // meta-region then we have to clip manually.
_ReplaceTryMeta:
          if (engine->metaRegion.getLength() > 1)
          {
            newRegion = engine->getTemporaryRegion();
            FOG_RETURN_ON_ERROR(Region::intersect(*newRegion, engine->metaRegion, *box));

            newLength = newRegion->getLength();
            box = &newRegion->_d->boundingBox;

            if (newLength == 0)
              return engine->serializer->clipAll(engine);

            if (newLength > 1)
              goto _ReplaceClipRegion;
          }

_ReplaceClipBox:
          engine->masterFlags &= ~RASTER_NO_PAINT_USER_CLIP;

          engine->ctx.clipType = RASTER_CLIP_BOX;
          engine->ctx.clipBoxI = *box;
          engine->stroker.f->_clipBox.setBox(*box);
          engine->stroker.d->_clipBox.setBox(*box);
          return ERR_OK;

        case RASTER_CLIP_REGION:
          // Not used anymore.
          engine->ctx.clipRegion.clear();
          goto _ReplaceTryMeta;

        case RASTER_CLIP_MASK:
          // TODO: RasterPaintEngine - clip-mask.
          goto _ReplaceTryMeta;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;

    case CLIP_OP_INTERSECT:
      switch (engine->ctx.clipType)
      {
        case RASTER_CLIP_BOX:
          // ClipNormalizedBox is always called with already clipped box to
          // the current clip-box or meta clip-box. This means that we don't
          // need to clip it again.
          FOG_ASSERT(engine->ctx.clipBoxI.subsumes(*box));
          goto _ReplaceClipBox;

        case RASTER_CLIP_REGION:
          // Just assure that there is something in current clipRegion, 
          // because clipping is set to RASTER_CLIP_REGION.
          FOG_ASSERT(engine->ctx.clipRegion.getLength() > 1);

          newRegion = engine->getTemporaryRegion();
          FOG_RETURN_ON_ERROR(Region::intersect(*newRegion, engine->ctx.clipRegion, *box));

          newLength = newRegion->getLength();
          box = &newRegion->_d->boundingBox;

          // If the result is empty region, then everything is clipped (and
          // painting is temporary disabled).
          if (newLength == 0)
            return engine->serializer->clipAll(engine);

          // If the result is single rectangle, then we switch to different
          // clip-type (RASTER_CLIP_BOX), which is simpler and much faster.
          if (newLength == 1)
            goto _ReplaceClipBox;

_ReplaceClipRegion:
          // We use swap to prevent old clipRegion to be deallocated. It's
          // likely that it will be used again.
          swap(engine->ctx.clipRegion, *newRegion);

          // And now we have to update all clip-boxes.
          engine->ctx.clipType = RASTER_CLIP_REGION;
          engine->ctx.clipBoxI = *box;
          engine->stroker.f->_clipBox.setBox(engine->ctx.clipBoxI);
          engine->stroker.d->_clipBox.setBox(engine->ctx.clipBoxI);
          return ERR_OK;

        case RASTER_CLIP_MASK:
          // TODO: RasterPaintEngine - clip-mask.
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedBoxF_render_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxF* box)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      BoxI box24x8(UNINITIALIZED);
      box24x8.x0 = Math::fixed24x8FromFloat(box->x0);
      box24x8.y0 = Math::fixed24x8FromFloat(box->y0);
      box24x8.x1 = Math::fixed24x8FromFloat(box->x1);
      box24x8.y1 = Math::fixed24x8FromFloat(box->y1);

      if (RasterUtil::isBox24x8Aligned(box24x8))
      {
        BoxI boxI(box24x8.x0 >> 8, box24x8.y0 >> 8, box24x8.x1 >> 8, box24x8.y1 >> 8);
        return engine->serializer->clipNormalizedBoxI(engine, clipOp, &boxI);
      }

      BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
      rasterizer->setSceneBox(engine->ctx.clipBoxI);
      rasterizer->setOpacity(0x100);

      rasterizer->init24x8(box24x8);
      // TODO:
      // return RasterPaintSerializer_filterRasterizedShape8_render_st(engine, rasterizer, &rasterizer->_boxBounds);
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedBoxD_render_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxD* box)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      BoxI box24x8(UNINITIALIZED);
      box24x8.x0 = Math::fixed24x8FromFloat(box->x0);
      box24x8.y0 = Math::fixed24x8FromFloat(box->y0);
      box24x8.x1 = Math::fixed24x8FromFloat(box->x1);
      box24x8.y1 = Math::fixed24x8FromFloat(box->y1);

      if (RasterUtil::isBox24x8Aligned(box24x8))
      {
        BoxI boxI(box24x8.x0 >> 8, box24x8.y0 >> 8, box24x8.x1 >> 8, box24x8.y1 >> 8);
        return engine->serializer->clipNormalizedBoxI(engine, clipOp, &boxI);
      }

      BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
      rasterizer->setSceneBox(engine->ctx.clipBoxI);
      rasterizer->setOpacity(0x100);

      rasterizer->init24x8(box24x8);
      // TODO:
      // return RasterPaintSerializer_filterRasterizedShape8_render_st(engine, rasterizer, &rasterizer->_boxBounds);
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Render(st) - ClipNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedPathF_render_st(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedPathD_render_st(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}
#endif // 0

















static err_t FOG_FASTCALL RasterPaintEngine_clipAll(
  RasterPaintEngine* engine)
{
  if ((engine->savedStateFlags & RASTER_STATE_CLIPPING) == 0)
    engine->saveClipping();

  engine->masterFlags |= RASTER_NO_PAINT_USER_CLIP;

  engine->ctx.clipBoxI.reset();
  engine->ctx.clipRegion.clear();
  engine->stroker.f->_clipBox.reset();
  engine->stroker.d->_clipBox.reset();

  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_clipRectI(Painter* self, uint32_t clipOp, const RectI* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_CLIP_FUNC();

  if (engine->isIntegralTransform())
  {
    const BoxI& clipBox = (clipOp == CLIP_OP_REPLACE) 
      ? engine->metaClipBoxI
      : engine->ctx.clipBoxI;
    BoxI normBox(UNINITIALIZED);

    if (engine->doIntegralTransformAndClip(normBox, *r, clipBox))
      return engine->serializer->clipNormalizedBoxI(engine, clipOp, &normBox);
    else
      return engine->serializer->clipAll(engine);
  }

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    RectF rf(*r);
    return engine->vtable->clipRectF(self, clipOp, &rf);
  }
  else
  {
    RectD rd(*r);
    return engine->vtable->clipRectD(self, clipOp, &rd);
  }
}

static err_t FOG_CDECL RasterPaintEngine_clipRectF(Painter* self, uint32_t clipOp, const RectF* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_CLIP_FUNC();

  const BoxF& clipBox = (clipOp == CLIP_OP_REPLACE) ? engine->getMetaClipBoxF() : engine->getClipBoxF();
  BoxF normBox(*r);

  if (!engine->ctx.rasterHints.rectToRectTransform)
  {
    PathF* path = &engine->ctx.tmpPathF[0];
    path->clear();
    path->rect(*r, PATH_DIRECTION_CW);
    return engine->serializer->clipPathF(engine, clipOp, path, FILL_RULE_NON_ZERO);
  }

  if (engine->ensureFinalTransformF())
  {
    engine->getFinalTransformF().mapBox(normBox, normBox);
  }

  if (!BoxF::intersect(normBox, normBox, clipBox))
    return ERR_OK;

  return engine->serializer->clipNormalizedBoxF(engine, clipOp, &normBox);
}

static err_t FOG_CDECL RasterPaintEngine_clipRectD(Painter* self, uint32_t clipOp, const RectD* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_CLIP_FUNC();

  const BoxD& clipBox = (clipOp == CLIP_OP_REPLACE) ? engine->getMetaClipBoxD() : engine->getClipBoxD();
  BoxD normBox(*r);

  if (!engine->ctx.rasterHints.rectToRectTransform)
  {
    PathD* path = &engine->ctx.tmpPathD[0];
    path->clear();
    path->rect(*r, PATH_DIRECTION_CW);
    return engine->serializer->clipPathD(engine, clipOp, path, FILL_RULE_NON_ZERO);
  }

  engine->getFinalTransformD().mapBox(normBox, normBox);

  if (!BoxD::intersect(normBox, normBox, clipBox))
    return ERR_OK;

  return engine->serializer->clipNormalizedBoxD(engine, clipOp, &normBox);
}

static err_t FOG_CDECL RasterPaintEngine_clipRectsI(Painter* self, uint32_t clipOp, const RectI* r, size_t count)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipPolygonI(Painter* self, uint32_t clipOp, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipShapeF(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipShapeD(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipStrokedShapeF(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipStrokedShapeD(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextAtI(Painter* self, uint32_t clipOp, const PointI* p, const StringW* text, const Font* font, const RectI* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextAtF(Painter* self, uint32_t clipOp, const PointF* p, const StringW* text, const Font* font, const RectF* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextAtD(Painter* self, uint32_t clipOp, const PointD* p, const StringW* text, const Font* font, const RectD* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextInI(Painter* self, uint32_t clipOp, const TextRectI* r, const StringW* text, const Font* font, const RectI* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextInF(Painter* self, uint32_t clipOp, const TextRectF* r, const StringW* text, const Font* font, const RectF* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextInD(Painter* self, uint32_t clipOp, const TextRectD* r, const StringW* text, const Font* font, const RectD* clip)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskAtI(Painter* self, uint32_t clipOp, const PointI* p, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskAtF(Painter* self, uint32_t clipOp, const PointF* p, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskAtD(Painter* self, uint32_t clipOp, const PointD* p, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskInI(Painter* self, uint32_t clipOp, const RectI* r, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskInF(Painter* self, uint32_t clipOp, const RectF* r, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskInD(Painter* self, uint32_t clipOp, const RectD* r, const Image* mask, const RectI* mFragment)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipRegion(Painter* self, uint32_t clipOp, const Region* r)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_resetClip(Painter* self)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintEngine - Group]
// ============================================================================

static void RasterPaintEngine_evaluateCommands(Painter* self, uint8_t* p, uint8_t* pEnd)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  const RasterPaintSerializer* serializer = engine->serializer;

  while (p != pEnd)
  {
    switch (reinterpret_cast<RasterPaintCmd*>(p)->getCommand())
    {
      case RASTER_PAINT_CMD_NULL:
      default:
      {
        break;
      }

      case RASTER_PAINT_CMD_NEXT:
      {
        RasterPaintCmd_Next* cmd = reinterpret_cast<RasterPaintCmd_Next*>(p);
        p = cmd->getPtr();
        break;
      }

      case RASTER_PAINT_CMD_SET_OPACITY:
      {
        RasterPaintCmd_SetOpacity* cmd = reinterpret_cast<RasterPaintCmd_SetOpacity*>(p);
        p += sizeof(RasterPaintCmd_SetOpacity);

        engine->ctx.rasterHints.opacity = cmd->getOpacity();
        break;
      }

      case RASTER_PAINT_CMD_SET_OPACITY_AND_PRGB32:
      {
        RasterPaintCmd_SetOpacityAndPrgb32* cmd = reinterpret_cast<RasterPaintCmd_SetOpacityAndPrgb32*>(p);
        p += sizeof(RasterPaintCmd_SetOpacityAndPrgb32);

        engine->discardSource();
        engine->sourceType = RASTER_SOURCE_ARGB32;
        engine->ctx.pc = (RasterPattern*)(size_t)0x1;
        engine->ctx.solid.prgb32.u32 = cmd->getPrgb32();
        engine->ctx.rasterHints.opacity = cmd->getOpacity();
        break;
      }
      
      case RASTER_PAINT_CMD_SET_OPACITY_AND_PATTERN:
      {
        RasterPaintCmd_SetOpacityAndPattern* cmd = reinterpret_cast<RasterPaintCmd_SetOpacityAndPattern*>(p);
        p += sizeof(RasterPaintCmd_SetOpacityAndPattern);

        engine->discardSource();
        engine->sourceType = RASTER_SOURCE_TEXTURE;
        engine->ctx.pc = cmd->getPatternContext();
        engine->ctx.rasterHints.opacity = cmd->getOpacity();
        break;
      }

      case RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_I:
      {
        RasterPaintCmd_FillNormalizedShape<BoxI>* cmd = reinterpret_cast<RasterPaintCmd_FillNormalizedShape<BoxI>*>(p);
        p += sizeof(RasterPaintCmd_FillNormalizedShape<RectI>);

        serializer->fillNormalizedBoxI(engine, &cmd->_shape);
        break;
      }

      case RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_F:
      {
        RasterPaintCmd_FillNormalizedShape<BoxF>* cmd = reinterpret_cast<RasterPaintCmd_FillNormalizedShape<BoxF>*>(p);
        p += sizeof(RasterPaintCmd_FillNormalizedShape<RectF>);

        serializer->fillNormalizedBoxF(engine, &cmd->_shape);
        break;
      }

      case RASTER_PAINT_CMD_FILL_NORMALIZED_BOX_D:
      {
        RasterPaintCmd_FillNormalizedShape<BoxD>* cmd = reinterpret_cast<RasterPaintCmd_FillNormalizedShape<BoxD>*>(p);
        p += sizeof(RasterPaintCmd_FillNormalizedShape<RectD>);

        serializer->fillNormalizedBoxD(engine, &cmd->_shape);
        break;
      }

      case RASTER_PAINT_CMD_SET_CLIP_BOX:
      {
        RasterPaintCmd_SetClipBox* cmd = reinterpret_cast<RasterPaintCmd_SetClipBox*>(p);
        p += sizeof(RasterPaintCmd_SetClipBox);

        engine->ctx.clipType = RASTER_CLIP_BOX;
        engine->ctx.clipBoxI = cmd->getClipBox();
        break;
      }

      case RASTER_PAINT_CMD_SET_CLIP_REGION:
      {
        RasterPaintCmd_SetClipRegion* cmd = reinterpret_cast<RasterPaintCmd_SetClipRegion*>(p);
        p += sizeof(RasterPaintCmd_SetClipRegion);

        engine->ctx.clipType = RASTER_CLIP_REGION;
        engine->ctx.clipRegion = cmd->getClipRegion();
        engine->ctx.clipBoxI = engine->ctx.clipRegion.getBoundingBox();
        break;
      }
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_newGroup(Painter* self, uint32_t flags)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  MemZoneRecord* cRecord = engine->cmdAllocator.record();
  MemZoneRecord* gRecord = engine->groupAllocator.record();

  RasterPaintGroup* g = static_cast<RasterPaintGroup*>(
    engine->groupAllocator.alloc(sizeof(RasterPaintGroup)));

  if (FOG_IS_NULL(g))
  {
    engine->cmdAllocator.revert(cRecord);
    engine->groupAllocator.revert(gRecord);

    return ERR_RT_OUT_OF_MEMORY;
  }

  g->reset();
  g->top = engine->curGroup;
  g->groupRecord = gRecord;
  g->cmdRecord = cRecord;
  g->cmdStart = engine->cmdAllocator._pos;

  err_t err = engine->vtable->save(self);
  if (FOG_IS_ERROR(err))
  {
    engine->cmdAllocator.revert(cRecord);
    engine->groupAllocator.revert(gRecord);

    return err;
  }

  engine->curGroup = g;
  engine->serializer = &RasterPaintSerializer_group_vtable[RASTER_MODE_ST];
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_endGroup(Painter* self)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);
  RasterPaintGroup* g = engine->curGroup;

  if (g == &engine->topGroup)
    return ERR_PAINTER_NO_GROUP;

  engine->curGroup = g->top;

  engine->serializer = &RasterPaintSerializer_render_vtable[RASTER_MODE_ST];
  RasterPaintEngine_evaluateCommands(self, g->cmdStart, engine->cmdAllocator._pos);

  if (engine->curGroup != &engine->topGroup)
    engine->serializer = &RasterPaintSerializer_group_vtable[RASTER_MODE_ST];
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Flush]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_flush(Painter* self, uint32_t flags)
{
  RasterPaintEngine* engine = static_cast<RasterPaintEngine*>(self->_engine);

  // TODO: MT version.
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine]
// ============================================================================

RasterPaintEngine::RasterPaintEngine() :
  masterFlags(0),
  masterLayerId(0),
  masterMaskId(0),
  masterMaskSaved(0),
  sourceType(RASTER_SOURCE_NONE),
  savedStateFlags(0xFF),
  strokerPrecision(RASTER_PRECISION_NONE),
  integralTransformType(RASTER_INTEGRAL_TRANSFORM_SIMPLE),
  metaOrigin(0, 0),
  metaRegion(Region::getInfiniteInstance()),
  metaClipBoxI(0, 0, 0, 0),
  metaClipBoxF(0.0f, 0.0f, 0.0f, 0.0f),
  metaClipBoxD(0.0, 0.0, 0.0, 0.0),
  stateAllocator(16300),
  statePool(NULL),
  state(NULL),
  pcAllocator(16300),
  pcPool(NULL),
  groupAllocator(500),
  curGroup(&topGroup),
  cmdAllocator(16300),
  maxThreads(0),
  finalizing(0)
{
  // Setup the essentials.
  vtable = NULL;
  ctx.engine = this;

  integralTransform._sx = 1;
  integralTransform._sy = 1;
  integralTransform._tx = 0;
  integralTransform._ty = 0;

  source.color.initCustom1(Argb32(0xFF000000));
  opacityF = 1.0f;

  stroker.f.init();
  stroker.f->_isClippingEnabled = true;
  stroker.d.init();
  stroker.d->_isClippingEnabled = true;

  topGroup.reset();

  maxThreads = detectMaxThreads();
}

RasterPaintEngine::~RasterPaintEngine()
{
  if (ctx.target.imageData)
    ctx.target.imageData->locked--;

  discardStates();
  discardSource();

  stroker.f.destroy();
  stroker.d.destroy();
}

err_t RasterPaintEngine::init(const ImageBits& imageBits, ImageData* imaged, uint32_t initFlags)
{
  // Setup the primary group.
  ctx.target.pixels = imageBits.getData();
  ctx.target.size = imageBits.getSize();
  ctx.target.stride = imageBits.getStride();
  ctx.target.format = imageBits.getFormat();

  ctx.target.imageData = imaged;
  if (imaged) imaged->locked++;

  vtable = &RasterPaintEngine_vtable[ctx.target.precision];
  serializer = &RasterPaintSerializer_render_vtable[RASTER_MODE_ST];

  setupLayer();
  FOG_RETURN_ON_ERROR(ctx._initPrecision(ctx.target.precision));

  setupOps();
  setupDefaultClip();

  return ERR_OK;
}

err_t RasterPaintEngine::switchTo(const ImageBits& imageBits, ImageData* imaged)
{
  // TODO.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintEngine - DetectMaxThreads]
// ============================================================================

uint RasterPaintEngine::detectMaxThreads()
{
  return Math::max<uint>(Cpu::get()->getNumberOfProcessors(), RASTER_MAX_THREADS_SUGGESTED);
}

// ============================================================================
// [Fog::RasterPaintEngine - Transform]
// ============================================================================

bool RasterPaintEngine::doIntegralTransformAndClip(BoxI& dst, const RectI& src, const BoxI& clipBox)
{
  int tx = integralTransform._tx;
  int ty = integralTransform._ty;

  int sw = src.w;
  int sh = src.h;

  switch (integralTransformType)
  {
    case RASTER_INTEGRAL_TRANSFORM_SWAP:
      tx += src.y * integralTransform._sx;
      ty += src.x * integralTransform._sy;

      sw *= integralTransform._sy;
      sh *= integralTransform._sx;
      goto _Scaled;

    case RASTER_INTEGRAL_TRANSFORM_SCALING:
      tx += src.x * integralTransform._sx;
      ty += src.y * integralTransform._sy;

      sw *= integralTransform._sx;
      sh *= integralTransform._sy;

_Scaled:
      dst.x0 = tx;
      dst.y0 = ty;

      tx += sw;
      ty += sh;

      dst.x1 = tx;
      dst.y1 = ty;

      if (dst.x0 > dst.x1) swap(dst.x0, dst.x1);
      if (dst.y0 > dst.y1) swap(dst.x0, dst.x1);
      return BoxI::intersect(dst, dst, clipBox);

    case RASTER_INTEGRAL_TRANSFORM_SIMPLE:
      tx += src.x;
      ty += src.y;

      dst.x0 = tx;
      dst.y0 = ty;

      tx += sw;
      ty += sh;

      dst.x1 = tx;
      dst.y1 = ty;
      return BoxI::intersect(dst, dst, clipBox);

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - State - Save ...]
// ============================================================================

void RasterPaintEngine::saveSourceAndDiscard()
{
  if ((savedStateFlags & RASTER_STATE_SOURCE) == 0)
  {
    FOG_ASSERT(state != NULL);

    masterFlags &= ~RASTER_NO_PAINT_SOURCE;
    savedStateFlags |= RASTER_STATE_SOURCE;

    state->sourceType = (uint8_t)sourceType;
    switch (sourceType)
    {
      case RASTER_SOURCE_NONE:
        break;

      case RASTER_SOURCE_ARGB32:
        state->source.color->_argb32.u32 = source.color->_argb32.u32;
        state->solid = ctx.solid;
        break;

      case RASTER_SOURCE_COLOR:
        MemOps::copy_t<Color>(&state->source.color, &source.color);
        state->solid = ctx.solid;
        break;

      case RASTER_SOURCE_TEXTURE:
        MemOps::copy_t<Texture>(&state->source.texture, &source.texture);
        goto _SaveSourceContinue;

      case RASTER_SOURCE_GRADIENT:
        MemOps::copy_t<GradientD>(&state->source.gradient, &source.gradient);

_SaveSourceContinue:
        state->source.transform.initCustom1(source.transform());
        state->pc = ctx.pc;
        if (state->pc != NULL)
          state->pc->_reference.inc();
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }
  else
  {
    discardSource();
    masterFlags &= ~RASTER_NO_PAINT_SOURCE;
  }
}

void RasterPaintEngine::saveStroke()
{
  FOG_ASSERT((savedStateFlags & RASTER_STATE_STROKE) == 0);
  FOG_ASSERT(state != NULL);

  uint32_t prec = strokerPrecision;

  savedStateFlags |= RASTER_STATE_STROKE;
  state->strokerPrecision = prec;

  if (prec & RASTER_PRECISION_F)
    state->strokeParams.f.initCustom1(stroker.f->_params());

  if (prec & RASTER_PRECISION_D)
    state->strokeParams.d.initCustom1(stroker.d->_params());
}

void RasterPaintEngine::saveTransform()
{
  FOG_ASSERT((savedStateFlags & RASTER_STATE_TRANSFORM) == 0);
  FOG_ASSERT(state != NULL);

  savedStateFlags |= RASTER_STATE_TRANSFORM;

  state->userTransformD.initCustom1(userTransformD);

  state->finalTransformD.initCustom1(getFinalTransformD());
  state->finalTransformF.initCustom1(getFinalTransformF());

  state->integralTransformType = integralTransformType;
  state->integralTransform._sx = integralTransform._sx;
  state->integralTransform._sy = integralTransform._sy;
  state->integralTransform._tx = integralTransform._tx;
  state->integralTransform._ty = integralTransform._ty;
}

void RasterPaintEngine::saveClipping()
{
  FOG_ASSERT((savedStateFlags & RASTER_STATE_CLIPPING) == 0);
  FOG_ASSERT(state != NULL);

  savedStateFlags |= RASTER_STATE_CLIPPING;

  state->clipType = static_cast<uint8_t>(ctx.clipType);
  state->clipBoxI = ctx.clipBoxI;
  state->clipBoxF = stroker.f().getClipBox();
  state->clipBoxD = stroker.d().getClipBox();

  switch (ctx.clipType)
  {
    case RASTER_CLIP_BOX:
      break;

    case RASTER_CLIP_REGION:
      state->clipRegion.init(ctx.clipRegion);
      break;

    case RASTER_CLIP_MASK:
      // TODO: RasterPaintEngine - clip-mask.
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

void RasterPaintEngine::saveFilter()
{
  FOG_ASSERT((savedStateFlags & RASTER_STATE_FILTER) == 0);
  FOG_ASSERT(state != NULL);

  savedStateFlags |= RASTER_STATE_FILTER;
  state->filterScale().setFilterScale(ctx.filterScale);
}

// ============================================================================
// [Fog::RasterPaintEngine - State - Discard States]
// ============================================================================

void RasterPaintEngine::discardStates()
{
  if (state == NULL) return;
  RasterState* last = NULL;

  do {
    // Get the states which must be restored. It's important to destroy all
    // state resources or memory leak will occur.
    uint32_t restoreFlags = savedStateFlags;

    if (restoreFlags & RASTER_STATE_SOURCE)
    {
      switch (state->sourceType)
      {
        case RASTER_SOURCE_NONE:
        case RASTER_SOURCE_ARGB32:
        case RASTER_SOURCE_COLOR:
          break;

        case RASTER_SOURCE_TEXTURE:
          state->source.texture.destroy();
          goto _DiscardSourceContinue;

        case RASTER_SOURCE_GRADIENT:
          state->source.gradient.destroy();
_DiscardSourceContinue:
          if (state->pc != NULL && state->pc->_reference.deref())
            destroyPatternContext(state->pc);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }

    if (restoreFlags & RASTER_STATE_STROKE)
    {
      if (state->strokerPrecision & RASTER_PRECISION_F) state->strokeParams.f.destroy();
      if (state->strokerPrecision & RASTER_PRECISION_D) state->strokeParams.d.destroy();
    }

    if (restoreFlags & RASTER_STATE_TRANSFORM)
    {
      // Nothing here.
    }

    if (restoreFlags & RASTER_STATE_CLIPPING)
    {
      switch (state->clipType)
      {
        case RASTER_CLIP_BOX:
          // Nothing here.
          break;

        case RASTER_CLIP_REGION:
          state->clipRegion.destroy();
          break;

        case RASTER_CLIP_MASK:
          // TODO: RasterPaintEngine - clip-mask.
          break;
          
        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }

    last = state;
    state = state->prevState;
  } while (state != NULL);

  // Pool all discarded states.
  last->prevState = statePool;

  statePool = state;
  state = NULL;
}

// ============================================================================
// [Fog::RasterPaintEngine - Setup]
// ============================================================================

void RasterPaintEngine::setupLayer()
{
  uint32_t format = ctx.target.format;
  const ImageFormatDescription& desc = ImageFormatDescription::getByFormat(format);

  ctx.target.bpp = (uint32_t)desc.getBytesPerPixel();
  ctx.target.bpl = (uint32_t)ctx.target.size.w * ctx.target.bpp;
  ctx.target.precision = desc.getPrecision();
}

void RasterPaintEngine::setupOps()
{
  ctx.paintHints.packed = 0;
  ctx.paintHints.compositingOperator = COMPOSITE_SRC_OVER;
  ctx.paintHints.renderQuality = RENDER_QUALITY_DEFAULT;
  ctx.paintHints.imageQuality = IMAGE_QUALITY_DEFAULT;
  ctx.paintHints.gradientQuality = GRADIENT_QUALITY_NORMAL;
  ctx.paintHints.outlinedText = 0;
  ctx.paintHints.fastLine = 0;
  ctx.paintHints.geometricPrecision = GEOMETRIC_PRECISION_NORMAL;
  ctx.paintHints.fillRule = FILL_RULE_DEFAULT;

  ctx.rasterHints.packed = 0;
  ctx.rasterHints.opacity = ctx.fullOpacity.u;
  ctx.rasterHints.rectToRectTransform = 1;
  ctx.rasterHints.finalTransformF = 0;
  ctx.rasterHints.idealLine = 1;

  opacityF = 1.0f;

  // TODO: Move.
  sourceType = RASTER_SOURCE_ARGB32;
  source.color.init();
  source.transform.init();
}

void RasterPaintEngine::setupDefaultClip()
{
  BoxI bounds(0, 0, (int)ctx.target.size.w, (int)ctx.target.size.h);
  FOG_ASSERT(bounds.isValid());

  // Final matrix is translated by the finalOrigin, we are translating it back.
  // After this function is called it remains fully usable and valid.
  // TODO: Painter.
  //integralTransform._tx -= finalOrigin.x;
  //integralTransform._ty -= finalOrigin.y;

  ctx.clipType = RASTER_CLIP_BOX;
  ctx.clipRegion.clear();
  ctx.clipBoxI = bounds;
  stroker.f->_clipBox.setBox(bounds);
  stroker.d->_clipBox.setBox(bounds);

  // Clear the regions and origins and set work and final region to the bounds.
  metaOrigin.reset();
  metaRegion = bounds;
  metaClipBoxI = bounds;
  metaClipBoxF = bounds;
  metaClipBoxD = bounds;
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - Source]
// ============================================================================

err_t RasterPaintEngine::createPatternContext()
{
  FOG_ASSERT(sourceType != RASTER_SOURCE_NONE);
  FOG_ASSERT(sourceType != RASTER_SOURCE_ARGB32);
  FOG_ASSERT(sourceType != RASTER_SOURCE_COLOR);

  err_t err = ERR_RT_NOT_IMPLEMENTED;

  // First try to reuse context from context-pool.
  RasterPattern* pc = reinterpret_cast<RasterPattern*>(pcPool);

  if (FOG_IS_NULL(pc))
  {
    pc = reinterpret_cast<RasterPattern*>(pcAllocator.alloc(sizeof(RasterPattern)));
    if (FOG_IS_NULL(pc)) return ERR_RT_OUT_OF_MEMORY;
  }
  else
  {
    pcPool = reinterpret_cast<RasterAbstractLinkedList*>(pc)->next;
  }

  ctx.pc = pc;

  // Set reference count to 1 and initialize the context state to UNINITIALIZED.
  // It's important to mark context as uninitialized, because there are asserts
  // inside pattern-context initializers and leaving the pattern-context state
  // as is might cause crash in debug-mode.
  pc->_reference.set(1);
  pc->reset();

  switch (sourceType)
  {
    case RASTER_SOURCE_TEXTURE:
    {
      err = _api_raster.texture.create(pc,
        ctx.target.format,
        &metaClipBoxI,
        &source.texture->_image,
        &source.texture->_fragment,
        &source.adjusted,
        &source.texture->_clampColor,
        source.texture->getTileType(),
        ctx.paintHints.imageQuality);
      break;
    }

    case RASTER_SOURCE_GRADIENT:
    {
      uint32_t gradientType = source.gradient->getGradientType();
      err = _api_raster.gradient.create[gradientType](pc,
        ctx.target.format,
        &metaClipBoxI,
        &source.gradient,
        &source.adjusted,
        ctx.paintHints.gradientQuality);
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  if (FOG_IS_ERROR(err))
  {
    reinterpret_cast<RasterAbstractLinkedList*>(pc)->next = pcPool;
    pcPool = reinterpret_cast<RasterAbstractLinkedList*>(pc);
    ctx.pc = NULL;
  }

  return err;
}

// ============================================================================
// [Fog::RasterPaintEngine - Changed - Meta-Params]
// ============================================================================

void RasterPaintEngine::changedMetaParams()
{
  BoxI bounds(0, 0, ctx.target.size.w, ctx.target.size.h);

  metaClipBoxI = metaRegion.getBoundingBox();
  if (!metaClipBoxI.isValid())
  {
    masterFlags |= RASTER_NO_PAINT_META_REGION;
    metaClipBoxF.reset();
    metaClipBoxD.reset();
    
    stroker.f->_clipBox.reset();
    stroker.d->_clipBox.reset();

    // RASTER_CLIP_BOX is used for invalid clip-box as well.
    ctx.clipType = RASTER_CLIP_BOX;
    ctx.clipBoxI.reset();
    ctx.clipRegion.clear();
  }
  else
  {
    masterFlags &= ~RASTER_NO_PAINT_META_REGION;
    metaClipBoxF = metaClipBoxI;
    metaClipBoxD = metaClipBoxI;

    stroker.f->_clipBox.setBox(metaClipBoxI);
    stroker.d->_clipBox.setBox(metaClipBoxI);

    if (metaRegion.getLength() > 1)
    {
      ctx.clipType = RASTER_CLIP_REGION;
      ctx.clipBoxI = metaClipBoxI;
      ctx.clipRegion = metaRegion;
    }
    else
    {
      ctx.clipType = RASTER_CLIP_BOX;
      ctx.clipBoxI = metaClipBoxI;
      ctx.clipRegion.clear();
    }
  }

  metaTransformD._type = (metaOrigin.x | metaOrigin.y) == 0 
    ? TRANSFORM_TYPE_IDENTITY
    : TRANSFORM_TYPE_TRANSLATION;
  metaTransformD._20 = double(-metaOrigin.x);
  metaTransformD._21 = double(-metaOrigin.y);
}

// ============================================================================
// [Fog::RasterPaintEngine - Changed - Transform]
// ============================================================================

void RasterPaintEngine::changedUserTransform()
{
  masterFlags &= ~RASTER_NO_PAINT_USER_TRANSFORM;

  ctx.rasterHints.rectToRectTransform = 1;
  ctx.rasterHints.finalTransformF = 0;

  integralTransformType = RASTER_INTEGRAL_TRANSFORM_NONE;
  integralTransform._sx = 1;
  integralTransform._sy = 1;
  integralTransform._tx = 0;
  integralTransform._ty = 0;

  if (userTransformD.getType() == TRANSFORM_TYPE_IDENTITY)
    stroker.d->_transform->setTransform(metaTransformD);
  else
    TransformD::multiply(stroker.d->_transform, metaTransformD, userTransformD);

  stroker.f->_isDirty = true;
  stroker.d->_isDirty = true;

  switch (getFinalTransformD().getType())
  {
    case TRANSFORM_TYPE_DEGENERATE:
      ctx.rasterHints.rectToRectTransform = 0;
      masterFlags |= RASTER_NO_PAINT_USER_TRANSFORM | RASTER_PENDING_TRANSFORM;
      return;

    case TRANSFORM_TYPE_PROJECTION:
      ctx.rasterHints.rectToRectTransform = 0;
      masterFlags |= RASTER_PENDING_TRANSFORM;
      return;

    case TRANSFORM_TYPE_AFFINE:
    case TRANSFORM_TYPE_ROTATION:
      ctx.rasterHints.rectToRectTransform = 0;
      masterFlags |= RASTER_PENDING_TRANSFORM;
      return;

    case TRANSFORM_TYPE_SCALING:
      if (!Math::isFuzzyToInt(getFinalTransformD()._00, integralTransform._sx)) break;
      if (!Math::isFuzzyToInt(getFinalTransformD()._11, integralTransform._sy)) break;

      integralTransformType = RASTER_INTEGRAL_TRANSFORM_SCALING;
      goto _Translation;

    case TRANSFORM_TYPE_SWAP:
      if (!Math::isFuzzyToInt(getFinalTransformD()._01, integralTransform._sx)) break;
      if (!Math::isFuzzyToInt(getFinalTransformD()._10, integralTransform._sy)) break;

      integralTransformType = RASTER_INTEGRAL_TRANSFORM_SWAP;
      goto _Translation;

    case TRANSFORM_TYPE_TRANSLATION:
      integralTransformType = RASTER_INTEGRAL_TRANSFORM_SIMPLE;

_Translation:
      if (!Math::isFuzzyToInt(getFinalTransformD()._20, integralTransform._tx)) break;
      if (!Math::isFuzzyToInt(getFinalTransformD()._21, integralTransform._ty)) break;

      masterFlags |= RASTER_PENDING_TRANSFORM;
      return;

    case TRANSFORM_TYPE_IDENTITY:
      integralTransformType = RASTER_INTEGRAL_TRANSFORM_SIMPLE;

      masterFlags |= RASTER_PENDING_TRANSFORM;
      return;
  }

  // Transform is not integral.
  integralTransformType = RASTER_INTEGRAL_TRANSFORM_NONE;
  masterFlags |= RASTER_PENDING_TRANSFORM;
}

// ============================================================================
// [Fog::RasterPaintEngine - API]
// ============================================================================

static FOG_INLINE bool RasterPaintEngine_isFormatSupported(uint32_t format)
{
  return format < IMAGE_FORMAT_COUNT && format != IMAGE_FORMAT_I8;
}

static FOG_INLINE bool RasterPaintEngine_checkRect(const SizeI& size, const RectI& rect)
{
  return !((uint)(rect.x) >= (uint)(size.w         ) ||
           (uint)(rect.y) >= (uint)(size.h         ) ||
           (uint)(rect.w) >  (uint)(size.w - rect.x) ||
           (uint)(rect.h) >  (uint)(size.h - rect.y) );
}

static err_t RasterPaintEngine_prepareToImage(ImageBits& imageBits, Image& image, const RectI* rect)
{
  uint32_t format = image.getFormat();

  // Basic checks.
  if (image.isEmpty())
    return ERR_RT_INVALID_ARGUMENT;

  if (rect && !RasterPaintEngine_checkRect(image.getSize(), *rect))
    return ERR_RT_INVALID_ARGUMENT;

  if (!RasterPaintEngine_isFormatSupported(format))
    return ERR_IMAGE_INVALID_FORMAT;

  // Fill the raw-image buffer.
  FOG_RETURN_ON_ERROR(image.detach());

  imageBits.setData(image.getSize(), format, image.getStride(), image.getFirstX());

  if (rect)
  {
    imageBits._size.set(rect->w, rect->h);
    imageBits._data += rect->h * imageBits._stride +
                       rect->w * image.getBytesPerPixel();
  }

  return ERR_OK;
}

static err_t RasterPaintEngine_prepareToImage(ImageBits& imageBits, const ImageBits& _src, const RectI* rect)
{
  // Basic checks.
  if (rect && !RasterPaintEngine_checkRect(_src._size, *rect))
    return ERR_RT_INVALID_ARGUMENT;

  if (!RasterPaintEngine_isFormatSupported(_src._format))
    return ERR_IMAGE_INVALID_FORMAT;

  // Fill the raw-image buffer.
  imageBits = _src;

  if (rect)
  {
    imageBits._size.set(rect->w, rect->h);
    imageBits._data += rect->h * imageBits._stride +
                       rect->w * ImageFormatDescription::getByFormat(imageBits._format).getBytesPerPixel();
  }

  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_beginImage(Painter* self, Image* image, const RectI* rect, uint32_t initFlags)
{
  err_t err;

  ImageBits imageBits(UNINITIALIZED);
  RasterPaintEngine* engine;

  // Release the painter engine.
  if (self->_engine)
    self->_vtable->release(self);

  // Prepare.
  err = RasterPaintEngine_prepareToImage(imageBits, *image, rect);
  if (FOG_IS_ERROR(err))
    goto _Fail;

  // Create the raster painter engine.
  engine = fog_new RasterPaintEngine();
  if (FOG_IS_NULL(engine))
  {
    err = ERR_RT_OUT_OF_MEMORY;
    goto _Fail;
  }

  err = engine->init(imageBits, image->_d, initFlags);
  if (FOG_IS_ERROR(err))
  {
    fog_delete(engine);
    goto _Fail;
  }

  self->_engine = engine;
  self->_vtable = engine->vtable;
  return ERR_OK;

_Fail:
  self->_engine = fog_api.painter_getNullEngine();
  self->_vtable = self->_engine->vtable;
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_beginIBits(Painter* self, const ImageBits* _imageBits, const RectI* rect, uint32_t initFlags)
{
  err_t err;

  ImageBits imageBits(UNINITIALIZED);
  RasterPaintEngine* engine;

  // Release the painter engine.
  if (self->_engine)
    self->_vtable->release(self);

  // Prepare.
  err = RasterPaintEngine_prepareToImage(imageBits, *_imageBits, rect);
  if (FOG_IS_ERROR(err))
    goto _Fail;

  // Create the raster painter engine.
  engine = fog_new RasterPaintEngine();
  if (FOG_IS_NULL(engine))
  {
    err = ERR_RT_OUT_OF_MEMORY;
    goto _Fail;
  }

  err = engine->init(imageBits, NULL, initFlags);
  if (FOG_IS_ERROR(err))
  {
    fog_delete(engine);
    goto _Fail;
  }

  self->_engine = engine;
  self->_vtable = engine->vtable;
  return ERR_OK;

_Fail:
  self->_engine = fog_api.painter_getNullEngine();
  self->_vtable = self->_engine->vtable;
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_switchToImage(Painter* self, Image* image, const RectI* rect)
{
  ImageBits imageBits(UNINITIALIZED);
  uint32_t deviceId;

  // Prepare.
  err_t err = RasterPaintEngine_prepareToImage(imageBits, *image, rect);
  if (FOG_IS_ERROR(err))
    goto _Fail;

  err = self->getDeviceId(deviceId);
  if (FOG_IS_ERROR(err))
    goto _Fail;

  // If we know that the paint engine is RASTER then we use switchTo() instead
  // of beginImage(), because there can be a lot of cache objects already
  // initialized, thus they can be used.
  if (deviceId == PAINT_DEVICE_RASTER)
    return static_cast<RasterPaintEngine*>(self->_engine)->switchTo(imageBits, image->_d);
  else
    return fog_api.painter_beginImage(self, image, rect, NO_FLAGS);

_Fail:
  self->end();
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_switchToIBits(Painter* self, const ImageBits* _imageBits, const RectI* rect)
{
  ImageBits imageBits(UNINITIALIZED);
  uint32_t deviceId;

  // Prepare.
  err_t err = RasterPaintEngine_prepareToImage(imageBits, *_imageBits, rect);
  if (FOG_IS_ERROR(err))
    goto _Fail;

  err = self->getDeviceId(deviceId);
  if (FOG_IS_ERROR(err))
    goto _Fail;

  if (deviceId == PAINT_DEVICE_RASTER)
    return static_cast<RasterPaintEngine*>(self->_engine)->switchTo(imageBits, NULL);
  else
    return fog_api.painter_beginIBits(self, _imageBits, rect, NO_FLAGS);

_Fail:
  self->end();
  return err;
}

// ============================================================================
// [Fog::RasterPaintEngine - Init / Fini]
// ============================================================================

FOG_CPU_DECLARE_INITIALIZER_SSE2( RasterPaintEngine_init_SSE2(void) )

template<int _PRECISION>
static void RasterPaintEngine_init_vtable_t()
{
  PaintEngineVTable* v = &RasterPaintEngine_vtable[_PRECISION];

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  v->release = RasterPaintEngine_release;

  // --------------------------------------------------------------------------
  // [Meta Params]
  // --------------------------------------------------------------------------

  v->getMetaParams = RasterPaintEngine_getMetaParams;
  v->setMetaParams = RasterPaintEngine_setMetaParams;
  v->resetMetaParams = RasterPaintEngine_resetMetaParams;

  // --------------------------------------------------------------------------
  // [Parameters]
  // --------------------------------------------------------------------------

  v->getParameter = RasterPaintEngine_getParameter;
  v->setParameter = RasterPaintEngine_setParameter;
  v->resetParameter = RasterPaintEngine_resetParameter;

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  v->getSourceType = RasterPaintEngine_getSourceType;
  v->getSourceColor = RasterPaintEngine_getSourceColor;
  v->getSourcePattern = RasterPaintEngine_getSourcePattern;

  v->setSourceNone = RasterPaintEngine_setSourceNone;
  v->setSourceArgb32 = RasterPaintEngine_setSourceArgb32<_PRECISION>;
  v->setSourceArgb64 = RasterPaintEngine_setSourceArgb64<_PRECISION>;
  v->setSourceColor = RasterPaintEngine_setSourceColor<_PRECISION>;
  v->setSourcePattern = RasterPaintEngine_setSourcePattern;
  v->setSourceAbstract = RasterPaintEngine_setSourceAbstract;

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  v->getTransformF = RasterPaintEngine_getTransformF;
  v->getTransformD = RasterPaintEngine_getTransformD;

  v->setTransformF = RasterPaintEngine_setTransformF;
  v->setTransformD = RasterPaintEngine_setTransformD;

  v->applyTransform = RasterPaintEngine_applyTransform;
  v->resetTransform = RasterPaintEngine_resetTransform;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  v->save = RasterPaintEngine_save;
  v->restore = RasterPaintEngine_restore;

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  v->mapPointF = RasterPaintEngine_mapPointF;
  v->mapPointD = RasterPaintEngine_mapPointD;

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  v->drawRectI = RasterPaintEngine_drawRectI;
  v->drawRectF = RasterPaintEngine_drawRectF;
  v->drawRectD = RasterPaintEngine_drawRectD;

  v->drawPolylineI = RasterPaintEngine_drawPolylineI;
  v->drawPolygonI = RasterPaintEngine_drawPolygonI;

  v->drawShapeF = RasterPaintEngine_drawShapeF;
  v->drawShapeD = RasterPaintEngine_drawShapeD;

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  v->fillAll = RasterPaintEngine_fillAll;

  v->fillRectI = RasterPaintEngine_fillRectI;
  v->fillRectF = RasterPaintEngine_fillRectF;
  v->fillRectD = RasterPaintEngine_fillRectD;

  v->fillRectsI = RasterPaintEngine_fillRectsI;
  v->fillPolygonI = RasterPaintEngine_fillPolygonI;

  v->fillShapeF = RasterPaintEngine_fillShapeF;
  v->fillShapeD = RasterPaintEngine_fillShapeD;

  v->fillTextAtI = RasterPaintEngine_fillTextAtI;
  v->fillTextAtF = RasterPaintEngine_fillTextAtF;
  v->fillTextAtD = RasterPaintEngine_fillTextAtD;

  v->fillTextInI = RasterPaintEngine_fillTextInI;
  v->fillTextInF = RasterPaintEngine_fillTextInF;
  v->fillTextInD = RasterPaintEngine_fillTextInD;

  v->fillMaskAtI = RasterPaintEngine_fillMaskAtI;
  v->fillMaskAtF = RasterPaintEngine_fillMaskAtF;
  v->fillMaskAtD = RasterPaintEngine_fillMaskAtD;

  v->fillMaskInI = RasterPaintEngine_fillMaskInI;
  v->fillMaskInF = RasterPaintEngine_fillMaskInF;
  v->fillMaskInD = RasterPaintEngine_fillMaskInD;

  v->fillRegion = RasterPaintEngine_fillRegion;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  v->blitImageAtI = RasterPaintEngine_blitImageAtI;
  v->blitImageAtF = RasterPaintEngine_blitImageAtF;
  v->blitImageAtD = RasterPaintEngine_blitImageAtD;

  v->blitImageInI = RasterPaintEngine_blitImageInI;
  v->blitImageInF = RasterPaintEngine_blitImageInF;
  v->blitImageInD = RasterPaintEngine_blitImageInD;

  v->blitMaskedImageAtI = RasterPaintEngine_blitMaskedImageAtI;
  v->blitMaskedImageAtF = RasterPaintEngine_blitMaskedImageAtF;
  v->blitMaskedImageAtD = RasterPaintEngine_blitMaskedImageAtD;

  v->blitMaskedImageInI = RasterPaintEngine_blitMaskedImageInI;
  v->blitMaskedImageInF = RasterPaintEngine_blitMaskedImageInF;
  v->blitMaskedImageInD = RasterPaintEngine_blitMaskedImageInD;

  // --------------------------------------------------------------------------
  // [Filter]
  // --------------------------------------------------------------------------

  v->filterAll = RasterPaintEngine_filterAll;

  v->filterRectI = RasterPaintEngine_filterRectI;
  v->filterRectF = RasterPaintEngine_filterRectF;
  v->filterRectD = RasterPaintEngine_filterRectD;

  v->filterShapeF = RasterPaintEngine_filterShapeF;
  v->filterShapeD = RasterPaintEngine_filterShapeD;

  v->filterStrokedShapeF = RasterPaintEngine_filterStrokedShapeF;
  v->filterStrokedShapeD = RasterPaintEngine_filterStrokedShapeD;

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  v->clipRectI = RasterPaintEngine_clipRectI;
  v->clipRectF = RasterPaintEngine_clipRectF;
  v->clipRectD = RasterPaintEngine_clipRectD;

  v->clipRectsI = RasterPaintEngine_clipRectsI;
  v->clipPolygonI = RasterPaintEngine_clipPolygonI;

  v->clipShapeF = RasterPaintEngine_clipShapeF;
  v->clipShapeD = RasterPaintEngine_clipShapeD;

  v->clipStrokedShapeF = RasterPaintEngine_clipStrokedShapeF;
  v->clipStrokedShapeD = RasterPaintEngine_clipStrokedShapeD;

  v->clipTextAtI = RasterPaintEngine_clipTextAtI;
  v->clipTextAtF = RasterPaintEngine_clipTextAtF;
  v->clipTextAtD = RasterPaintEngine_clipTextAtD;

  v->clipTextInI = RasterPaintEngine_clipTextInI;
  v->clipTextInF = RasterPaintEngine_clipTextInF;
  v->clipTextInD = RasterPaintEngine_clipTextInD;

  v->clipMaskAtI = RasterPaintEngine_clipMaskAtI;
  v->clipMaskAtF = RasterPaintEngine_clipMaskAtF;
  v->clipMaskAtD = RasterPaintEngine_clipMaskAtD;

  v->clipMaskInI = RasterPaintEngine_clipMaskInI;
  v->clipMaskInF = RasterPaintEngine_clipMaskInF;
  v->clipMaskInD = RasterPaintEngine_clipMaskInD;

  v->clipRegion = RasterPaintEngine_clipRegion;

  v->resetClip = RasterPaintEngine_resetClip;

  // --------------------------------------------------------------------------
  // [Group]
  // --------------------------------------------------------------------------

  v->newGroup = RasterPaintEngine_newGroup;
  v->endGroup = RasterPaintEngine_endGroup;

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  v->flush = RasterPaintEngine_flush;
}

FOG_NO_EXPORT void RasterPaintEngine_init_vtable()
{
  RasterPaintEngine_init_vtable_t<IMAGE_PRECISION_BYTE>();
  RasterPaintEngine_init_vtable_t<IMAGE_PRECISION_WORD>();
}

FOG_NO_EXPORT void RasterPaintEngine_init(void)
{
  // --------------------------------------------------------------------------
  // [Painter - API]
  // --------------------------------------------------------------------------

  fog_api.painter_beginImage = RasterPaintEngine_beginImage;
  fog_api.painter_beginIBits = RasterPaintEngine_beginIBits;

  fog_api.painter_switchToImage = RasterPaintEngine_switchToImage;
  fog_api.painter_switchToIBits = RasterPaintEngine_switchToIBits;

  // --------------------------------------------------------------------------
  // [RasterPaintEngine / RasterPaintSerializer - Init]
  // --------------------------------------------------------------------------

  RasterPaintEngine_init_vtable();
  RasterPaintSerializer_init_render_st();
  RasterPaintSerializer_init_group_st();

  // --------------------------------------------------------------------------
  // [RasterPaintEngine - CPU Based Optimizations]
  // --------------------------------------------------------------------------

  FOG_CPU_USE_INITIALIZER_SSE2( RasterPaintEngine_init_SSE2() )
}

} // Fog namespace
