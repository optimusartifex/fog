// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Cpu/Cpu.h>
#include <Fog/Core/Face/Face_C.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/Core/Memory/BlockAllocator_p.h>
#include <Fog/Core/Memory/Ops.h>
#include <Fog/Core/Memory/ZoneAllocator_p.h>
#include <Fog/Core/OS/OS.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadCondition.h>
#include <Fog/Core/Threading/ThreadPool.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageEffect.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterFiller_p.h>
#include <Fog/G2d/Painting/RasterPaintEngine_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterState_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Painting/RasterWorker_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>
#include <Fog/G2d/Render/RenderUtil_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Text/Font.h>

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

// Called by all 'fill' functions. This macro simply ensures that all fill
// parameters are correct (it's possible to fill something).
//
// Please see RASTER_NO_PAINT flags to better understand how this works.
#define _FOG_RASTER_ENTER_FILL_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY((engine->masterFlags & (RASTER_NO_PAINT_BASE_FLAGS | \
                                             RASTER_NO_PAINT_SOURCE     )) != 0)) \
    { \
      return ERR_OK; \
    } \
  FOG_MACRO_END

// Called by all 'stroke' functions. This macro simply ensures that all stroke
// parameters are correct (it's possible to stroke something). Note that stroke
// is implemented normally using fill pipe-line, there are only few exceptions.
//
// Please see RASTER_NO_PAINT flags to better understand how this works.
#define _FOG_RASTER_ENTER_STROKE_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY((engine->masterFlags & (RASTER_NO_PAINT_BASE_FLAGS | \
                                             RASTER_NO_PAINT_SOURCE     | \
                                             RASTER_NO_PAINT_STROKE     )) != 0)) \
    { \
      return ERR_OK; \
    } \
  FOG_MACRO_END

#define _FOG_RASTER_ENTER_BLIT_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY((engine->masterFlags & (RASTER_NO_PAINT_BASE_FLAGS)) != 0)) \
    { \
      return ERR_OK; \
    } \
  FOG_MACRO_END

#define _FOG_RASTER_ENSURE_PATTERN(_Engine_) \
  FOG_MACRO_BEGIN \
    if (_Engine_->ctx.pc == NULL) \
    { \
      FOG_RETURN_ON_ERROR(_Engine_->createPatternContext()); \
    } \
  FOG_MACRO_END

#define _PARAM_C(_Type_) (*reinterpret_cast<const _Type_*>(value))
#define _PARAM_M(_Type_) (*reinterpret_cast<_Type_*>(value))

// ============================================================================
// [Fog::RasterPaintEngine - VTable]
// ============================================================================

RasterPaintEngineVTable RasterPaintEngine_vtable;

// ============================================================================
// [Fog::RasterPaintEngine - Serializer]
// ============================================================================

RasterPaintSerializer RasterPaintEngine_serializer[RASTER_MODE_COUNT];

// ============================================================================
// [Fog::RasterPaintEngine - AddRef / Release]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_release(Painter* self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  engine->finalizing = true;
  fog_delete(engine);

  self->_engine = _api.painter.getNullEngine();
  self->_vtable = self->_engine->vtable;

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Meta Params]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_getMetaParams(const Painter* self, Region& region, PointI& origin)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  region = engine->metaRegion;
  origin = engine->metaOrigin;

  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setMetaParams(Painter* self, const Region& region, const PointI& origin)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  engine->discardStates();

  engine->metaRegion = region;
  engine->metaOrigin = origin;

  engine->userRegion = Region::infinite();
  engine->userOrigin.reset();

  return engine->changedCoreClip();
}

static err_t FOG_CDECL RasterPaintEngine_resetMetaParams(Painter* self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  engine->discardStates();

  engine->metaRegion = Region::infinite();
  engine->metaOrigin.reset();

  engine->userRegion = Region::infinite();
  engine->userOrigin.reset();

  return engine->changedCoreClip();
}

// ============================================================================
// [Fog::RasterPaintEngine - User Params]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_getUserParams(const Painter* self, Region& region, PointI& origin)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  region = engine->userRegion;
  origin = engine->userOrigin;

  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setUserParams(Painter* self, const Region& region, const PointI& origin)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  engine->userRegion = region;
  engine->userOrigin = origin;

  return engine->changedCoreClip();
}

static err_t FOG_CDECL RasterPaintEngine_resetUserParams(Painter* self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  engine->metaRegion = Region::infinite();
  engine->metaOrigin.reset();

  return engine->changedCoreClip();
}

// ============================================================================
// [Fog::RasterPaintEngine - Parameters]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_getParameter(const Painter* self, uint32_t parameterId, void* value)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  switch (parameterId)
  {
    // ------------------------------------------------------------------------
    // [Backend]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_SIZE_I:
    {
      _PARAM_M(SizeI).set(engine->ctx.layer.size);
      return ERR_OK;
    }

    case PAINTER_PARAMETER_SIZE_F:
    {
      _PARAM_M(SizeF).set(engine->ctx.layer.size);
      return ERR_OK;
    }

    case PAINTER_PARAMETER_SIZE_D:
    {
      _PARAM_M(SizeD).set(engine->ctx.layer.size);
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FORMAT_I:
    {
      _PARAM_M(uint32_t) = engine->ctx.layer.primaryFormat;
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
      _PARAM_M(uint32_t) = engine->wm ? true : false;
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

    case PAINTER_PARAMETER_PARAMS_F:
    {
      PaintParamsF& params = _PARAM_M(PaintParamsF);

      params._hints = engine->ctx.paintHints;
      params._opacity = engine->opacityF;

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_NONE:
          params._strokeParams.reset();
          params._strokeParams.setHints(engine->strokeParams.f->getHints());
          break;

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_F:
          params._strokeParams = engine->strokeParams.f.instance();
          break;

        case RASTER_PRECISION_D:
          params._strokeParams = engine->strokeParams.d.instance();
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      return ERR_OK;
    }

    case PAINTER_PARAMETER_PARAMS_D:
    {
      PaintParamsD& params = _PARAM_M(PaintParamsD);

      params._hints = engine->ctx.paintHints;
      params._opacity  = engine->opacityF;

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_NONE:
          params._strokeParams.reset();
          params._strokeParams.setHints(engine->strokeParams.f->getHints());
          break;

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          params._strokeParams = engine->strokeParams.d.instance();
          break;

        case RASTER_PRECISION_F:
          params._strokeParams = engine->strokeParams.f.instance();
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

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_NONE:
          params.reset();
          params.setHints(engine->strokeParams.f->getHints());
          break;

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_F:
          params = engine->strokeParams.f.instance();
          break;

        case RASTER_PRECISION_D:
          params = engine->strokeParams.d.instance();
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      return ERR_OK;
    }

    case PAINTER_PARAMETER_STROKE_PARAMS_D:
    {
      PathStrokerParamsD& params = _PARAM_M(PathStrokerParamsD);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_NONE:
          params.reset();
          params.setHints(engine->strokeParams.f->getHints());
          break;

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          params = engine->strokeParams.d.instance();
          break;

        case RASTER_PRECISION_F:
          params = engine->strokeParams.f.instance();
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
      if (engine->strokeParamsPrecision == RASTER_PRECISION_D)
        _PARAM_M(float) = (float)engine->strokeParams.d->getLineWidth();
      else
        _PARAM_M(float) = engine->strokeParams.f->getLineWidth();

      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_WIDTH_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_F)
        _PARAM_M(double) = engine->strokeParams.f->getLineWidth();
      else
        _PARAM_M(double) = engine->strokeParams.d->getLineWidth();

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Join]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_JOIN_I:
    {
      _PARAM_M(uint32_t) = engine->strokeParams.f->getLineJoin();
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Caps]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_START_CAP_I:
    {
      _PARAM_M(uint32_t) = engine->strokeParams.f->getStartCap();
      return ERR_OK;
    }

    case PAINTER_PARAMETER_END_CAP_I:
    {
      _PARAM_M(uint32_t) = engine->strokeParams.f->getEndCap();
      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_CAPS_I:
    {
      uint32_t startCap = engine->strokeParams.f->getStartCap();
      uint32_t endCap = engine->strokeParams.f->getEndCap();

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
      if (engine->strokeParamsPrecision == RASTER_PRECISION_D)
        _PARAM_M(float) = (float)engine->strokeParams.d->getMiterLimit();
      else
        _PARAM_M(float) = engine->strokeParams.f->getMiterLimit();

      return ERR_OK;
    }

    case PAINTER_PARAMETER_MITER_LIMIT_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_F)
        _PARAM_M(double) = engine->strokeParams.f->getMiterLimit();
      else
        _PARAM_M(double) = engine->strokeParams.d->getMiterLimit();

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-Offset]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_OFFSET_F:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_D)
        _PARAM_M(float) = (float)engine->strokeParams.d->getDashOffset();
      else
        _PARAM_M(float) = engine->strokeParams.f->getDashOffset();

      return ERR_OK;
    }

    case PAINTER_PARAMETER_DASH_OFFSET_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_F)
        _PARAM_M(double) = engine->strokeParams.f->getDashOffset();
      else
        _PARAM_M(double) = engine->strokeParams.d->getDashOffset();

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-List]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_LIST_F:
    {
      List<float>& v = _PARAM_M(List<float>);

      if (engine->strokeParamsPrecision == RASTER_PRECISION_D)
      {
        return _ListFloatFromListDouble(v, engine->strokeParams.d->getDashList());
      }
      else
      {
        v = engine->strokeParams.f->getDashList();
        return ERR_OK;
      }
    }

    case PAINTER_PARAMETER_DASH_LIST_D:
    {
      List<double>& v = _PARAM_M(List<double>);

      if (engine->strokeParamsPrecision == RASTER_PRECISION_F)
      {
        return _ListDoubleFromListFloat(v, engine->strokeParams.f->getDashList());
      }
      else
      {
        v = engine->strokeParams.d->getDashList();
        return ERR_OK;
      }
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
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
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

      if (v == 0) return ERR_RT_INVALID_ARGUMENT;
      if (v > RASTER_MAX_THREADS_USED) v = RASTER_MAX_THREADS_USED;

      engine->maxThreads = v;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_PARAMS_F:
    {
      const PaintParamsF& v = _PARAM_C(PaintParamsF);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->masterFlags &= ~(RASTER_NO_PAINT_COMPOSITING_OPERATOR |
                               RASTER_NO_PAINT_OPACITY              |
                               RASTER_NO_PAINT_STROKE               );

      engine->ctx.paintHints = v._hints;
      engine->ctx.rasterHints.opacity = Math::iround(v._opacity * engine->ctx.fullOpacity.f);
      engine->opacityF = v._opacity;
      engine->strokeParams.f.instance() = v._strokeParams;

      if (RasterUtil::isCompositingOperatorNop(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      if (engine->ctx.rasterHints.opacity == 0)
        engine->masterFlags |= RASTER_NO_PAINT_OPACITY;

      if (!engine->ctx.paintHints.geometricPrecision)
      {
        engine->strokeParamsPrecision = RASTER_PRECISION_F;
        engine->strokeParams.d->setHints(v._strokeParams.getHints());
      }
      else
      {
        engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
        engine->strokeParams.d.instance() = v._strokeParams;
      }

      // TODO: ImageQuality/GradientQuality hints changed.
      // TODO: Stroke changed.

      return ERR_OK;
    }

    case PAINTER_PARAMETER_PARAMS_D:
    {
      const PaintParamsD& v = _PARAM_C(PaintParamsD);
      float opacity = (float)v._opacity;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->masterFlags &= ~(RASTER_NO_PAINT_COMPOSITING_OPERATOR |
                               RASTER_NO_PAINT_OPACITY              |
                               RASTER_NO_PAINT_STROKE               );

      engine->ctx.paintHints = v._hints;
      engine->ctx.rasterHints.opacity = Math::iround(opacity * engine->ctx.fullOpacity.f);
      engine->opacityF = opacity;

      engine->strokeParamsPrecision = RASTER_PRECISION_D;
      engine->strokeParams.f->setHints(v._strokeParams.getHints());
      engine->strokeParams.d.instance() = v._strokeParams;

      if (RasterUtil::isCompositingOperatorNop(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      if (engine->ctx.rasterHints.opacity == 0)
        engine->masterFlags |= RASTER_NO_PAINT_OPACITY;

      // TODO: ImageQuality/GradientQuality hints changed.
      // TODO: Stroke changed.

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

      if (RasterUtil::isCompositingOperatorNop(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      // TODO: Something interesting might be changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_COMPOSITING_OPERATOR_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= COMPOSITE_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->masterFlags &= ~RASTER_NO_PAINT_COMPOSITING_OPERATOR;
      engine->ctx.paintHints.compositingOperator = v;

      if (RasterUtil::isCompositingOperatorNop(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      return ERR_OK;
    }

    case PAINTER_PARAMETER_RENDER_QUALITY_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= RENDER_QUALITY_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.renderQuality = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_IMAGE_QUALITY_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= IMAGE_QUALITY_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.imageQuality = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GRADIENT_QUALITY_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= GRADIENT_QUALITY_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.gradientQuality = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OUTLINED_TEXT_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= 2) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.outlinedText = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FAST_LINE_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= 2) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.fastLine = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GEOMETRIC_PRECISION_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= GEOMETRIC_PRECISION_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.geometricPrecision = v;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Opacity]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_OPACITY_F:
    {
      vFloat = _PARAM_C(float);
      if (vFloat < 0.0f || vFloat > 1.0f) return ERR_RT_INVALID_ARGUMENT;

_GlobalOpacityF:
      engine->masterFlags &= ~RASTER_NO_PAINT_OPACITY;
      engine->opacityF = vFloat;
      engine->ctx.rasterHints.opacity = Math::iround(vFloat * engine->ctx.fullOpacity.f);

      if (engine->ctx.rasterHints.opacity == 0) engine->masterFlags |= RASTER_NO_PAINT_OPACITY;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OPACITY_D:
    {
      double v = _PARAM_C(double);
      if (v < 0.0 || v > 1.0) return ERR_RT_INVALID_ARGUMENT;

      vFloat = (float)v;
      goto _GlobalOpacityF;
    }

    // ------------------------------------------------------------------------
    // [Fill Params - Fill Rule]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_FILL_RULE_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (FOG_UNLIKELY(v >= FILL_RULE_COUNT)) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.fillRule = v;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_STROKE_PARAMS_F:
    {
      const PathStrokerParamsF& v = _PARAM_C(PathStrokerParamsF);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParamsPrecision = RASTER_PRECISION_F;
      engine->strokeParams.f.instance() = v;
      engine->strokeParams.d->setHints(v.getHints());

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_STROKE_PARAMS_D:
    {
      const PathStrokerParamsD& v = _PARAM_C(PathStrokerParamsD);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParamsPrecision = RASTER_PRECISION_D;
      engine->strokeParams.f->setHints(v.getHints());
      engine->strokeParams.d.instance() = v;

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Width]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_WIDTH_F:
    {
      float v = _PARAM_C(float);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setLineWidth(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision)
            break;
          engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          engine->strokeParams.d->setLineWidth(v);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_WIDTH_D:
    {
      double v = _PARAM_C(double);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.d->setLineWidth(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_NONE:
          // Initialize to 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_D;
          break;

        case RASTER_PRECISION_D:
          break;

        case RASTER_PRECISION_F:
          // Initialize to 'Float & Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;

          // Convert the 'Float' parameters into the 'Double' ones.
          engine->strokeParams.d->setMiterLimit(engine->strokeParams.f->getMiterLimit());
          engine->strokeParams.d->setDashOffset(engine->strokeParams.f->getDashOffset());
          _ListDoubleFromListFloat(engine->strokeParams.d->_dashList, engine->strokeParams.f->_dashList);
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
          engine->strokeParams.f->setLineWidth((float)v);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Join]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_JOIN_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= LINE_JOIN_COUNT) return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setLineJoin(v);
      engine->strokeParams.d->setLineJoin(v);

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Caps]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_START_CAP_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setStartCap(v);
      engine->strokeParams.d->setStartCap(v);

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_END_CAP_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setEndCap(v);
      engine->strokeParams.d->setEndCap(v);

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_CAPS_I:
    {
      uint32_t v = _PARAM_C(uint32_t);
      if (v >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setLineCaps(v);
      engine->strokeParams.d->setLineCaps(v);

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Miter-Limit]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_MITER_LIMIT_F:
    {
      float v = _PARAM_C(float);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setMiterLimit(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision)
            break;
          engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          engine->strokeParams.d->setMiterLimit(v);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_MITER_LIMIT_D:
    {
      double v = _PARAM_C(double);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.d->setMiterLimit(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_D:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_D;
          break;

        case RASTER_PRECISION_F:
          // Initialize to 'Float & Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;

          // Convert the 'Float' parameters into the 'Double' ones.
          engine->strokeParams.d->setLineWidth(engine->strokeParams.f->getLineWidth());
          engine->strokeParams.d->setDashOffset(engine->strokeParams.f->getDashOffset());
          _ListDoubleFromListFloat(engine->strokeParams.d->_dashList, engine->strokeParams.f->_dashList);
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
          engine->strokeParams.f->setMiterLimit((float)v);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-Offset]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_OFFSET_F:
    {
      float v = _PARAM_C(float);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setDashOffset(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision)
            break;
          engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          engine->strokeParams.d->setDashOffset(v);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_DASH_OFFSET_D:
    {
      double v = _PARAM_C(double);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.d->setDashOffset(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_D:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_D;
          break;

        case RASTER_PRECISION_F:
          // Initialize to 'Float & Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;

          // Convert the 'Float' parameters into the 'Double' ones.
          engine->strokeParams.d->setLineWidth(engine->strokeParams.f->getLineWidth());
          engine->strokeParams.d->setMiterLimit(engine->strokeParams.f->getMiterLimit());
          _ListDoubleFromListFloat(engine->strokeParams.d->_dashList, engine->strokeParams.f->_dashList);
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
          engine->strokeParams.f->setDashOffset((float)v);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-List]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_LIST_F:
    {
      const List<float>& v = _PARAM_C(List<float>);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setDashList(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision)
            break;
          engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
        case RASTER_PRECISION_D:
          _ListDoubleFromListFloat(engine->strokeParams.d->_dashList, v);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_DASH_LIST_D:
    {
      const List<double>& v = _PARAM_C(List<double>);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.d->setDashList(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_D:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_D;
          break;

        case RASTER_PRECISION_F:
          // Initialize to 'Float & Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;

          // Convert the 'Float' parameters into the 'Double' ones.
          engine->strokeParams.d->setLineWidth(engine->strokeParams.f->getLineWidth());
          engine->strokeParams.d->setMiterLimit(engine->strokeParams.f->getMiterLimit());
          engine->strokeParams.d->setDashOffset(engine->strokeParams.f->getDashOffset());
          // ... Fall through ...

        case RASTER_PRECISION_BOTH:
          _ListFloatFromListDouble(engine->strokeParams.f->_dashList, v);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      // TODO: Stroke params changed.
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
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

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

    case PAINTER_PARAMETER_PARAMS_F:
    case PAINTER_PARAMETER_PARAMS_D:
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
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->masterFlags &= ~RASTER_NO_PAINT_STROKE;

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_NONE:
          engine->strokeParams.f->resetHints();
          engine->strokeParams.d->resetHints();
          break;

        case RASTER_PRECISION_F:
          engine->strokeParams.f->reset();
          engine->strokeParams.d->resetHints();
          engine->strokeParamsPrecision = RASTER_PRECISION_NONE;
          break;

        case RASTER_PRECISION_BOTH:
          engine->strokeParams.f->reset();
          // ...Fall through ...

        case RASTER_PRECISION_D:
          engine->strokeParams.d->reset();
          engine->strokeParamsPrecision = RASTER_PRECISION_NONE;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Width]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_WIDTH_F:
    case PAINTER_PARAMETER_LINE_WIDTH_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_NONE) return ERR_OK;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setLineWidth(1.0f);
      engine->strokeParams.d->setLineWidth(1.0);

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Join]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_JOIN_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setStartCap(LINE_JOIN_DEFAULT);
      engine->strokeParams.d->setStartCap(LINE_JOIN_DEFAULT);

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Caps]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_START_CAP_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setStartCap(LINE_CAP_DEFAULT);
      engine->strokeParams.d->setStartCap(LINE_CAP_DEFAULT);

      return ERR_OK;
    }

    case PAINTER_PARAMETER_END_CAP_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setEndCap(LINE_CAP_DEFAULT);
      engine->strokeParams.d->setEndCap(LINE_CAP_DEFAULT);

      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_CAPS_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setLineCaps(LINE_CAP_DEFAULT);
      engine->strokeParams.d->setLineCaps(LINE_CAP_DEFAULT);

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Miter-Limit]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_MITER_LIMIT_F:
    case PAINTER_PARAMETER_MITER_LIMIT_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_NONE) return ERR_OK;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setMiterLimit(4.0f);
      engine->strokeParams.d->setMiterLimit(4.0);

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-Offset]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_OFFSET_F:
    case PAINTER_PARAMETER_DASH_OFFSET_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_NONE) return ERR_OK;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->setDashOffset(1.0f);
      engine->strokeParams.d->setDashOffset(1.0);

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-List]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_LIST_F:
    case PAINTER_PARAMETER_DASH_LIST_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_NONE) return ERR_OK;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) engine->saveStroke();
      engine->strokeParams.f->_dashList.clear();
      engine->strokeParams.d->_dashList.clear();

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

static err_t FOG_CDECL RasterPaintEngine_getSourceType(const Painter* self, uint32_t& val)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  val = engine->sourceType;
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_getSourceColor(const Painter* self, Color& color)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if (engine->sourceType != PATTERN_TYPE_COLOR) return ERR_RT_INVALID_STATE;

  color = engine->source.color.instance();
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_getSourcePatternF(const Painter* self, PatternF& pattern)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  switch (engine->sourceType)
  {
    case PATTERN_TYPE_NONE:
      pattern.reset();
      return ERR_OK;

    case PATTERN_TYPE_COLOR:
      FOG_RETURN_ON_ERROR(pattern.setColor(engine->source.color.instance()));
      return ERR_OK;

    case PATTERN_TYPE_GRADIENT:
      FOG_RETURN_ON_ERROR(pattern.setGradient(engine->source.gradient.instance()));
      FOG_RETURN_ON_ERROR(pattern.setTransform(engine->source.transform.instance()));
      return ERR_OK;

    case PATTERN_TYPE_TEXTURE:
      FOG_RETURN_ON_ERROR(pattern.setTexture(engine->source.texture.instance()));
      FOG_RETURN_ON_ERROR(pattern.setTransform(engine->source.transform.instance()));
      return ERR_OK;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static err_t FOG_CDECL RasterPaintEngine_getSourcePatternD(const Painter* self, PatternD& pattern)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  switch (engine->sourceType)
  {
    case PATTERN_TYPE_NONE:
      pattern.reset();
      return ERR_OK;

    case PATTERN_TYPE_COLOR:
      FOG_RETURN_ON_ERROR(pattern.setColor(engine->source.color.instance()));
      return ERR_OK;

    case PATTERN_TYPE_GRADIENT:
      FOG_RETURN_ON_ERROR(pattern.setGradient(engine->source.gradient.instance()));
      FOG_RETURN_ON_ERROR(pattern.setTransform(engine->source.transform.instance()));
      return ERR_OK;

    case PATTERN_TYPE_TEXTURE:
      FOG_RETURN_ON_ERROR(pattern.setTexture(engine->source.texture.instance()));
      FOG_RETURN_ON_ERROR(pattern.setTransform(engine->source.transform.instance()));
      return ERR_OK;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static err_t FOG_CDECL RasterPaintEngine_setSourceArgb32(Painter* self, uint32_t argb32)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0)
    engine->saveSource();

  engine->discardSource();
  engine->masterFlags &= ~RASTER_NO_PAINT_SOURCE;

  engine->sourceType = PATTERN_TYPE_COLOR;
  engine->source.color.initCustom1(Argb32(argb32));
  Face::p32PRGB32FromARGB32(engine->ctx.solid.prgb32.p32, argb32);

  engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
  if (engine->ctx.precision == IMAGE_PRECISION_BYTE) return ERR_OK;

  Face::p64PRGB64FromPRGB32(engine->ctx.solid.prgb64.p64, engine->ctx.solid.prgb32.p32);
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setSourceArgb64(Painter* self, const Argb64& argb64)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0)
    engine->saveSource();

  engine->discardSource();
  engine->masterFlags &= ~RASTER_NO_PAINT_SOURCE;

  engine->sourceType = PATTERN_TYPE_COLOR;
  engine->source.color.initCustom1(argb64);
  Face::p64PRGB64FromARGB64(engine->ctx.solid.prgb64.p64, argb64.p64);

  engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
  if (engine->ctx.precision == IMAGE_PRECISION_WORD) return ERR_OK;

  Face::p64PRGB32FromPRGB64(engine->ctx.solid.prgb32.p32, engine->ctx.solid.prgb64.p64);
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setSourceColor(Painter* self, const Color& color)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if (!color.isValid()) goto _Invalid;

  if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0)
    engine->saveSource();

  engine->discardSource();
  engine->masterFlags &= ~RASTER_NO_PAINT_SOURCE;

  engine->sourceType = PATTERN_TYPE_COLOR;
  engine->source.color.initCustom1(color);
  engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;

  if (engine->ctx.precision == IMAGE_PRECISION_BYTE)
  {
    Argb32 argb32 = color.getArgb32();
    Face::p32PRGB32FromARGB32(engine->ctx.solid.prgb32.p32, argb32.p32);
  }
  else
  {
    Argb64 argb64 = color.getArgb64();
    Face::p64PRGB64FromARGB64(engine->ctx.solid.prgb64.p64, argb64.p64);
  }
  return ERR_OK;

_Invalid:
  engine->sourceType = PATTERN_TYPE_NONE;
  engine->masterFlags |= RASTER_NO_PAINT_SOURCE;
  engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
  return ERR_OK;
}
static err_t FOG_CDECL RasterPaintEngine_setSourcePatternF(Painter* self, const PatternF& pattern)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  switch (pattern.getType())
  {
    case PATTERN_TYPE_NONE:
      if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0)
        engine->saveSource();

      engine->discardSource();
      engine->sourceType = PATTERN_TYPE_NONE;
      engine->masterFlags |= RASTER_NO_PAINT_SOURCE;
      engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
      return ERR_OK;

    case PATTERN_TYPE_COLOR:
      return self->_vtable->setSourceColor(self, pattern._d->color.instance());

    case PATTERN_TYPE_TEXTURE:
      return self->_vtable->setSourceAbstract(self, PAINTER_SOURCE_TEXTURE_F, pattern._d->texture.instancep(), &pattern._d->transform);

    case PATTERN_TYPE_GRADIENT:
      return self->_vtable->setSourceAbstract(self, PAINTER_SOURCE_GRADIENT_F, pattern._d->texture.instancep(), &pattern._d->transform);

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL RasterPaintEngine_setSourcePatternD(Painter* self, const PatternD& pattern)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  switch (pattern.getType())
  {
    case PATTERN_TYPE_NONE:
      if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0)
        engine->saveSource();

      engine->discardSource();
      engine->sourceType = PATTERN_TYPE_NONE;
      engine->masterFlags |= RASTER_NO_PAINT_SOURCE;
      engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
      return ERR_OK;

    case PATTERN_TYPE_COLOR:
      return self->_vtable->setSourceColor(self, pattern._d->color.instance());

    case PATTERN_TYPE_TEXTURE:
      return self->_vtable->setSourceAbstract(self, PAINTER_SOURCE_TEXTURE_D, pattern._d->texture.instancep(), &pattern._d->transform);

    case PATTERN_TYPE_GRADIENT:
      return self->_vtable->setSourceAbstract(self, PAINTER_SOURCE_GRADIENT_D, pattern._d->texture.instancep(), &pattern._d->transform);

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL RasterPaintEngine_setSourceAbstract(Painter* self, uint32_t sourceId, const void* value, const void* tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0) engine->saveSource();
  engine->discardSource();
  engine->masterFlags &= ~RASTER_NO_PAINT_SOURCE;

  switch (sourceId)
  {
    // ------------------------------------------------------------------------
    // [Texture]
    // ------------------------------------------------------------------------

    case PAINTER_SOURCE_TEXTURE_F:
    case PAINTER_SOURCE_TEXTURE_D:
    {
      const Texture& v = _PARAM_C(Texture);
      if (!v._image.isValid()) goto _Invalid;

      engine->sourceType = PATTERN_TYPE_TEXTURE;
      engine->source.texture.initCustom1(v);
      engine->ctx.pc = NULL;

      if (tr)
      {
        if (sourceId == PAINTER_SOURCE_TEXTURE_F)
          engine->source.transform.instance() = *reinterpret_cast<const TransformF*>(tr);
        else
          engine->source.transform.instance() = *reinterpret_cast<const TransformD*>(tr);
        goto _HasTransform;
      }
      else
      {
        engine->source.transform.instance().reset();
        goto _NoTransform;
      }
    }

    // ------------------------------------------------------------------------
    // [Gradient]
    // ------------------------------------------------------------------------

    case PAINTER_SOURCE_GRADIENT_F:
    {
      const GradientF& gradient = _PARAM_C(GradientF);

      engine->sourceType = PATTERN_TYPE_GRADIENT;
      engine->source.gradient.initCustom1(gradient);
      engine->ctx.pc = NULL;

      if (tr)
      {
        engine->source.transform.instance() = *reinterpret_cast<const TransformF*>(tr);
        goto _HasTransform;
      }
      else
      {
        engine->source.transform.instance().reset();
        goto _NoTransform;
      }
      return ERR_OK;
    }

    case PAINTER_SOURCE_GRADIENT_D:
    {
      const GradientD& gradient = _PARAM_C(GradientD);

      engine->sourceType = PATTERN_TYPE_GRADIENT;
      engine->source.gradient.initCustom1(gradient);
      engine->ctx.pc = NULL;

      if (tr)
      {
        engine->source.transform.instance() = *reinterpret_cast<const TransformD*>(tr);
        goto _HasTransform;
      }
      else
      {
        engine->source.transform.instance().reset();
        goto _NoTransform;
      }

      return ERR_OK;
    }
  }

_Invalid:
  engine->sourceType = PATTERN_TYPE_NONE;
  engine->masterFlags |= RASTER_NO_PAINT_SOURCE;
  engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
  return ERR_OK;

_HasTransform:
  TransformD::multiply(engine->source.adjusted.instance(), engine->source.transform.instance(), engine->finalTransform);
  return ERR_OK;

_NoTransform:
  if (engine->finalTransform.getType() == TRANSFORM_TYPE_IDENTITY)
    engine->source.adjusted.instance().reset();
  else
    engine->source.adjusted.instance() = engine->finalTransform;
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Transform]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_getTransformF(const Painter* self, TransformF& tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  tr = engine->userTransform;
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_getTransformD(const Painter* self, TransformD& tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  tr = engine->userTransform;
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setTransformF(Painter* self, const TransformF& tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) engine->saveTransform();

  engine->userTransform = tr;
  engine->changedTransform();
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setTransformD(Painter* self, const TransformD& tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) engine->saveTransform();

  engine->userTransform = tr;
  engine->changedTransform();
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_applyTransformF(Painter* self, uint32_t transformOp, const void* params)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  err_t err = ERR_OK;

  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) engine->saveTransform();

  // Need to convert parameters from 'Float' precision into 'Double'
  // precision. This is quite low-level.
  //
  // Please see
  //
  //   Fog/G2d/Geometry/Transform.h
  //
  // to get basic understanding how this works. It's simply a converter
  // from the 'float' into the 'double' and from the 'TransformF' instance
  // into the 'TransformD' instance.
  double d[3];
  Static<TransformD> tr;

  switch (transformOp & 0xF)
  {
    case TRANSFORM_OP_ROTATE_PT:
      d[2] = reinterpret_cast<const float*>(params)[2];
    case TRANSFORM_OP_TRANSLATE:
    case TRANSFORM_OP_SCALE:
    case TRANSFORM_OP_SKEW:
      d[1] = reinterpret_cast<const float*>(params)[1];
    case TRANSFORM_OP_ROTATE:
      d[0] = reinterpret_cast<const float*>(params)[0];
      err = _api.transformd.transform(engine->userTransform, transformOp, d);
      break;

    case TRANSFORM_OP_FLIP:
      err = _api.transformd.transform(engine->userTransform, transformOp, params);
      break;

    case TRANSFORM_OP_MULTIPLY:
    case TRANSFORM_OP_MULTIPLY_INV:
      tr.initCustom1(*reinterpret_cast<const TransformF*>(params));
      err = _api.transformd.transform(engine->userTransform, transformOp, tr);
      break;
  }

  if (FOG_IS_ERROR(err)) engine->userTransform.reset();

  engine->changedTransform();
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_applyTransformD(Painter* self, uint32_t transformOp, const void* params)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  err_t err = ERR_OK;

  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) engine->saveTransform();
  err = _api.transformd.transform(engine->userTransform, transformOp, params);
  if (FOG_IS_ERROR(err)) engine->userTransform.reset();

  engine->changedTransform();
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_resetTransform(Painter* self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) engine->saveTransform();
  engine->userTransform.reset();

  engine->changedTransform();
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - State ]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_save(Painter* self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

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
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

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

    switch (state->sourceType)
    {
      case PATTERN_TYPE_COLOR:
        Memory::copy_t<Color>(engine->source.color.instancep(), state->source.color.instancep());
        // ... Fall through ...

      case PATTERN_TYPE_NONE:
        engine->ctx.pc = (RenderPatternContext*)(size_t)(0x1);
        break;

      case PATTERN_TYPE_TEXTURE:
        Memory::copy_t<Texture>(engine->source.texture.instancep(), state->source.texture.instancep());
        goto _RestoreSourceContinue;

      case PATTERN_TYPE_GRADIENT:
        Memory::copy_t<GradientD>(engine->source.gradient.instancep(), state->source.gradient.instancep());

_RestoreSourceContinue:
        Memory::copy_t<TransformD>(engine->source.transform.instancep(), state->source.transform.instancep());
        engine->ctx.pc = state->pc;
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    engine->sourceType = state->sourceType;
  }

  // ------------------------------------------------------------------------
  // [Stroke]
  // ------------------------------------------------------------------------

  if (restoreFlags & RASTER_STATE_STROKE)
  {
    switch (state->strokeParamsPrecision)
    {
      case RASTER_PRECISION_NONE:
        if (engine->strokeParamsPrecision == RASTER_PRECISION_NONE) break;
        engine->strokeParams.f->reset();
        engine->strokeParams.d->reset();
        break;

      case RASTER_PRECISION_F:
        engine->strokeParams.f.destroy();
        Memory::copy_t<PathStrokerParamsF>(engine->strokeParams.f.instancep(), state->strokeParams.f.instancep());
        engine->strokeParams.d.instance().setHints(engine->strokeParams.f.instance().getHints());

        if (engine->strokeParamsPrecision == RASTER_PRECISION_F) break;
        engine->strokeParams.d->reset();
        break;

      case RASTER_PRECISION_D:
        engine->strokeParams.d.destroy();
        Memory::copy_t<PathStrokerParamsD>(engine->strokeParams.d.instancep(), state->strokeParams.d.instancep());
        engine->strokeParams.f.instance().setHints(engine->strokeParams.d.instance().getHints());

        if (engine->strokeParamsPrecision == RASTER_PRECISION_D) break;
        engine->strokeParams.f->reset();
        break;

      case RASTER_PRECISION_BOTH:
        engine->strokeParams.f.destroy();
        engine->strokeParams.d.destroy();
        Memory::copy_t<PathStrokerParamsF>(engine->strokeParams.f.instancep(), state->strokeParams.f.instancep());
        Memory::copy_t<PathStrokerParamsD>(engine->strokeParams.d.instancep(), state->strokeParams.d.instancep());
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    engine->strokeParamsPrecision = state->strokeParamsPrecision;
  }

  // ------------------------------------------------------------------------
  // [Transform]
  // ------------------------------------------------------------------------

  if (restoreFlags & RASTER_STATE_TRANSFORM)
  {
    engine->userTransform = state->userTransform.instance();
    engine->finalTransform = state->finalTransform.instance();
    engine->finalTransformF = state->finalTransformF.instance();

    engine->coreTranslationI = state->coreTranslationI;
    engine->finalTransformI._type = state->finalTransformI._type;
    engine->finalTransformI._sx = state->finalTransformI._sx;
    engine->finalTransformI._sy = state->finalTransformI._sy;
    engine->finalTransformI._tx = state->finalTransformI._tx;
    engine->finalTransformI._ty = state->finalTransformI._ty;
  }

  // ------------------------------------------------------------------------
  // [Clipping]
  // ------------------------------------------------------------------------

  if (restoreFlags & RASTER_STATE_CLIPPING)
  {
    // TODO: Clipping.
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Map]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_mapPointF(const Painter* self, uint32_t mapOp, PointF& pt)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  PointD ptd(pt);

  switch (mapOp)
  {
    case PAINTER_MAP_USER_TO_DEVICE:
      // TODO:
      return ERR_RT_NOT_IMPLEMENTED;

    case PAINTER_MAP_DEVICE_TO_USER:
      // TODO:
      return ERR_RT_NOT_IMPLEMENTED;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

static err_t FOG_CDECL RasterPaintEngine_mapPointD(const Painter* self, uint32_t mapOp, PointD& pt)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  switch (mapOp)
  {
    case PAINTER_MAP_USER_TO_DEVICE:
      // TODO:
      return ERR_RT_NOT_IMPLEMENTED;

    case PAINTER_MAP_DEVICE_TO_USER:
      // TODO:
      return ERR_RT_NOT_IMPLEMENTED;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

















































// ============================================================================
// [Fog::RasterPaintEngine - FillAll]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_fillAll(Painter* self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  return engine->serializer->fillNormalizedBoxI(engine, engine->ctx.finalClipBoxI);
}

// ============================================================================
// [Fog::RasterPaintEngine - Draw]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_drawRectI(Painter* self, const RectI& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    return engine->vtable->drawRectF(self, RectF(r));
  }
  else
  {
    return engine->vtable->drawRectD(self, RectD(r));
  }
}

static err_t FOG_CDECL RasterPaintEngine_drawRectF(Painter* self, const RectF& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathF& path = engine->ctx.tmpPathF[2];
  path.clear();
  path.rect(r);
  return engine->serializer->drawRawPathF(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawRectD(Painter* self, const RectD& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathD& path = engine->ctx.tmpPathD[2];
  path.clear();
  path.rect(r);
  return engine->serializer->drawRawPathD(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawPolylineI(Painter* self, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF& path = engine->ctx.tmpPathF[2];
    path.clear();
    path.polyline(p, count);
    return engine->serializer->drawRawPathF(engine, path);
  }
  else
  {
    PathD& path = engine->ctx.tmpPathD[2];
    path.clear();
    path.polyline(p, count);
    return engine->serializer->drawRawPathD(engine, path);
  }
}

static err_t FOG_CDECL RasterPaintEngine_drawPolylineF(Painter* self, const PointF* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathF& path = engine->ctx.tmpPathF[2];
  path.clear();
  path.polyline(p, count);
  return engine->serializer->drawRawPathF(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawPolylineD(Painter* self, const PointD* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathD& path = engine->ctx.tmpPathD[2];
  path.clear();
  path.polyline(p, count);
  return engine->serializer->drawRawPathD(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawPolygonI(Painter* self, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF& path = engine->ctx.tmpPathF[2];
    path.clear();
    path.polygon(p, count);
    return engine->serializer->drawRawPathF(engine, path);
  }
  else
  {
    PathD& path = engine->ctx.tmpPathD[2];
    path.clear();
    path.polygon(p, count);
    return engine->serializer->drawRawPathD(engine, path);
  }
}

static err_t FOG_CDECL RasterPaintEngine_drawPolygonF(Painter* self, const PointF* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathF& path = engine->ctx.tmpPathF[2];
  path.clear();
  path.polygon(p, count);
  return engine->serializer->drawRawPathF(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawPolygonD(Painter* self, const PointD* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathD& path = engine->ctx.tmpPathD[2];
  path.clear();
  path.polygon(p, count);
  return engine->serializer->drawRawPathD(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawShapeF(Painter* self, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathF& path = engine->ctx.tmpPathF[2];
  path.clear();
  path._shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
  return engine->serializer->drawRawPathF(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawShapeD(Painter* self, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathD& path = engine->ctx.tmpPathD[2];
  path.clear();
  path._shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
  return engine->serializer->drawRawPathD(engine, path);
}

static err_t FOG_CDECL RasterPaintEngine_drawPathF(Painter* self, const PathF& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  return engine->serializer->drawRawPathF(engine, p);
}

static err_t FOG_CDECL RasterPaintEngine_drawPathD(Painter* self, const PathD& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  return engine->serializer->drawRawPathD(engine, p);
}

// ============================================================================
// [Fog::RasterPaintEngine - Fill]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_fillRectI(Painter* self, const RectI& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (engine->finalTransformI._type != RASTER_INTEGRAL_TRANSFORM_NONE)
  {
    BoxI box(UNINITIALIZED);
    if (engine->doIntegralTransformAndClip(box, r))
      return engine->serializer->fillNormalizedBoxI(engine, box);
    else
      return ERR_OK;
  }
  else
  {
    if (!engine->ctx.paintHints.geometricPrecision)
      return engine->vtable->fillRectF(self, RectF(r));
    else
      return engine->vtable->fillRectD(self, RectD(r));
  }
}

static err_t FOG_CDECL RasterPaintEngine_fillRectF(Painter* self, const RectF& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  BoxF box(r);
  if (engine->ensureFinalTransformF())
  {
    if (!engine->ctx.rasterHints.rectToRectTransform)
    {
      PathF& path = engine->ctx.tmpPathF[0];
      path.clear();
      path.rect(r, PATH_DIRECTION_CW);
      return engine->serializer->fillRawPathF(engine, path, FILL_RULE_NON_ZERO);
    }
    engine->finalTransformF.mapBox(box, box);
  }

  if (!BoxF::intersect(box, box, engine->ctx.finalClipperF.getClipBox()))
    return ERR_OK;

  return engine->serializer->fillNormalizedBoxF(engine, box);
}

static err_t FOG_CDECL RasterPaintEngine_fillRectD(Painter* self, const RectD& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  BoxD box(r);
  if (engine->finalTransform.getType() != TRANSFORM_TYPE_IDENTITY)
  {
    if (!engine->ctx.rasterHints.rectToRectTransform)
    {
      PathD& path = engine->ctx.tmpPathD[0];
      path.clear();
      path.rect(r, PATH_DIRECTION_CW);
      return engine->serializer->fillRawPathD(engine, path, FILL_RULE_NON_ZERO);
    }
    engine->finalTransform.mapBox(box, box);
  }

  if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
    return ERR_OK;

  return engine->serializer->fillNormalizedBoxD(engine, box);
}

// TODO: It's easy to clip rectangles, so clip them here and call
// doFillNormalizedPath() instead.

static err_t FOG_CDECL RasterPaintEngine_fillRectsI(Painter* self, const RectI* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF& path = engine->ctx.tmpPathF[0];
    path.clear();
    path.rects(r, count);
    return engine->serializer->fillRawPathF(engine, path, FILL_RULE_NON_ZERO);
  }
  else
  {
    PathD& path = engine->ctx.tmpPathD[0];
    path.clear();
    path.rects(r, count);
    return engine->serializer->fillRawPathD(engine, path, FILL_RULE_NON_ZERO);
  }
}

static err_t FOG_CDECL RasterPaintEngine_fillRectsF(Painter* self, const RectF* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (count == 0) return ERR_OK;
  if (count == 1) return engine->vtable->fillRectF(self, *r);

  PathF& path = engine->ctx.tmpPathF[0];
  path.clear();
  path.rects(r, count, PATH_DIRECTION_CW);

  return engine->serializer->fillRawPathF(engine, path, FILL_RULE_NON_ZERO);
}

static err_t FOG_CDECL RasterPaintEngine_fillRectsD(Painter* self, const RectD* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (count == 0) return ERR_OK;
  if (count == 1) return engine->vtable->fillRectD(self, *r);

  PathD& path = engine->ctx.tmpPathD[0];
  path.clear();
  path.rects(r, count, PATH_DIRECTION_CW);
  return engine->serializer->fillRawPathD(engine, path, FILL_RULE_NON_ZERO);
}

// TODO: It's easy to clip polygon, do it here, PathClipper should be enabled to do that.

static err_t FOG_CDECL RasterPaintEngine_fillPolygonI(Painter* self, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF& path = engine->ctx.tmpPathF[0];
    path.clear();
    path.polygon(p, count, PATH_DIRECTION_CW);
    return engine->serializer->fillRawPathF(engine, path, engine->ctx.paintHints.fillRule);
  }
  else
  {
    PathD& path = engine->ctx.tmpPathD[0];
    path.clear();
    path.polygon(p, count, PATH_DIRECTION_CW);
    return engine->serializer->fillRawPathD(engine, path, engine->ctx.paintHints.fillRule);
  }
}

static err_t FOG_CDECL RasterPaintEngine_fillPolygonF(Painter* self, const PointF* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  PathF& path = engine->ctx.tmpPathF[0];
  path.clear();
  path.polygon(p, count, PATH_DIRECTION_CW);
  return engine->serializer->fillRawPathF(engine, path, engine->ctx.paintHints.fillRule);
}

static err_t FOG_CDECL RasterPaintEngine_fillPolygonD(Painter* self, const PointD* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  PathD& path = engine->ctx.tmpPathD[0];
  path.clear();
  path.polygon(p, count, PATH_DIRECTION_CW);
  return engine->serializer->fillRawPathD(engine, path, engine->ctx.paintHints.fillRule);
}

static err_t FOG_CDECL RasterPaintEngine_fillShapeF(Painter* self, uint32_t shapeType, const void* shapeData)
{
  if (shapeType == SHAPE_TYPE_RECT)
    return self->_vtable->fillRectF(self, *(const RectF*)shapeData);

  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  PathF& path = engine->ctx.tmpPathF[0];
  path.clear();
  path._shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
  return engine->serializer->fillRawPathF(engine, path, engine->ctx.paintHints.fillRule);
}

static err_t FOG_CDECL RasterPaintEngine_fillShapeD(Painter* self, uint32_t shapeType, const void* shapeData)
{
  if (shapeType == SHAPE_TYPE_RECT)
    return self->_vtable->fillRectD(self, *(const RectD*)shapeData);

  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  PathD& path = engine->ctx.tmpPathD[0];
  path.clear();
  path._shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
  return engine->serializer->fillRawPathD(engine, path, engine->ctx.paintHints.fillRule);
}

static err_t FOG_CDECL RasterPaintEngine_fillPathF(Painter* self, const PathF& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  return engine->serializer->fillRawPathF(engine, p, engine->ctx.paintHints.fillRule);
}

static err_t FOG_CDECL RasterPaintEngine_fillPathD(Painter* self, const PathD& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  return engine->serializer->fillRawPathD(engine, p, engine->ctx.paintHints.fillRule);
}

static err_t FOG_CDECL RasterPaintEngine_fillTextAtI(Painter* self, const PointI& p, const String& text, const Font& font, const RectI* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextAtF(Painter* self, const PointF& p, const String& text, const Font& font, const RectF* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextAtD(Painter* self, const PointD& p, const String& text, const Font& font, const RectD* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextInI(Painter* self, const TextRectI& r, const String& text, const Font& font, const RectI* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextInF(Painter* self, const TextRectF& r, const String& text, const Font& font, const RectF* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillTextInD(Painter* self, const TextRectD& r, const String& text, const Font& font, const RectD* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskAtI(Painter* self, const PointI& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskAtF(Painter* self, const PointF& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskAtD(Painter* self, const PointD& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskInI(Painter* self, const RectI& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskInF(Painter* self, const RectF& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillMaskInD(Painter* self, const RectD& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_fillRegion(Painter* self, const Region& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (!engine->ctx.paintHints.geometricPrecision)
  {
    PathF& path = engine->ctx.tmpPathF[0];
    path.clear();
    path.region(r);
    return engine->serializer->fillRawPathF(engine, path, FILL_RULE_NON_ZERO);
  }
  else
  {
    PathD& path = engine->ctx.tmpPathD[0];
    path.clear();
    path.region(r);
    return engine->serializer->fillRawPathD(engine, path, FILL_RULE_NON_ZERO);
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Blit]
// ============================================================================

#define _FOG_RASTER_IMAGE_PARAMS(_Image_, _ImageFragment_) \
  int iX = 0; \
  int iY = 0; \
  int iW = _Image_.getWidth(); \
  int iH = _Image_.getHeight(); \
  \
  if (_ImageFragment_ != NULL) \
  { \
    if (!_ImageFragment_->isValid()) return ERR_RT_INVALID_ARGUMENT; \
    \
    iX = _ImageFragment_->x; \
    iY = _ImageFragment_->y; \
    \
    if ((uint)(iX) >= (uint)iW || \
        (uint)(iY) >= (uint)iH || \
        (uint)(_ImageFragment_->w - iX) > (uint)iW || \
        (uint)(_ImageFragment_->h - iY) > (uint)iH) \
    { \
      return ERR_RT_INVALID_ARGUMENT; \
    } \
    \
    iW = _ImageFragment_->w; \
    iH = _ImageFragment_->h; \
    if (iW == 0 || iH == 0) return ERR_OK; \
  }

static err_t FOG_CDECL RasterPaintEngine_blitImageAtI(Painter* self, const PointI& p, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(i, iFragment)

  int dstX = p.x, dstW;
  int dstY = p.y, dstH;
  int d;

  switch (engine->finalTransformI._type)
  {
    case RASTER_INTEGRAL_TRANSFORM_NONE:
    {
      if (!engine->ctx.paintHints.geometricPrecision)
        return engine->vtable->blitImageAtF(self, PointF(p), i, iFragment);
      else
        return engine->vtable->blitImageAtD(self, PointD(p), i, iFragment);
    }

    case RASTER_INTEGRAL_TRANSFORM_SIMPLE:
    {
      dstX += engine->finalTransformI._tx;
      dstY += engine->finalTransformI._ty;

      if ((uint)(d = dstX - engine->ctx.finalClipBoxI.x0) >= (uint)engine->ctx.finalClipBoxI.getWidth())
      {
        dstX = engine->ctx.finalClipBoxI.x0; iX -= d;
        if (d >= 0 || (iW += d) <= 0) return ERR_OK;
      }

      if ((uint)(d = dstY - engine->ctx.finalClipBoxI.y0) >= (uint)engine->ctx.finalClipBoxI.getHeight())
      {
        dstY = engine->ctx.finalClipBoxI.y0; iY -= d;
        if (d >= 0 || (iH += d) <= 0) return ERR_OK;
      }

      if ((d = engine->ctx.finalClipBoxI.x1 - dstX) < iW) iW = d;
      if ((d = engine->ctx.finalClipBoxI.y1 - dstY) < iH) iH = d;

      return engine->serializer->blitNormalizedImageI(engine, PointI(dstX, dstY), i, RectI(iX, iY, iW, iH));
    }

    case RASTER_INTEGRAL_TRANSFORM_SCALING:
    {
      dstX = dstX * engine->finalTransformI._sx;
      dstY = dstY * engine->finalTransformI._sy;

      dstW = iW * engine->finalTransformI._sx;
      dstH = iH * engine->finalTransformI._sy;

_Scaling:
      dstX += engine->finalTransformI._tx;
      dstY += engine->finalTransformI._ty;

      if (dstW < 0) { dstX += dstW; dstW = -dstW; }
      if (dstH < 0) { dstY += dstH; dstH = -dstH; }

      // Make a box model (dstX, dstY -> dstW, dstH).
      dstW += dstX;
      dstH += dstY;

      if (dstX < engine->ctx.finalClipBoxI.x0) dstX = engine->ctx.finalClipBoxI.x0;
      if (dstY < engine->ctx.finalClipBoxI.y0) dstY = engine->ctx.finalClipBoxI.y0;

      if (dstW > engine->ctx.finalClipBoxI.x1) dstW = engine->ctx.finalClipBoxI.x1;
      if (dstH > engine->ctx.finalClipBoxI.y1) dstH = engine->ctx.finalClipBoxI.y1;

      if (dstX >= dstW || dstY >= dstH) return ERR_OK;

      TransformD tr(engine->finalTransform);
      tr.translate(PointD(p.x, p.y));

      return engine->serializer->blitNormalizedTransformedImageI(engine, BoxI(dstX, dstY, dstW, dstH), i, RectI(iX, iY, iW, iH), tr);
    }

    case RASTER_INTEGRAL_TRANSFORM_SWAP:
    {
      d = dstX;

      dstX = dstY * engine->finalTransformI._sx;
      dstY = d    * engine->finalTransformI._sy;

      dstW = iH * engine->finalTransformI._sx;
      dstH = iW * engine->finalTransformI._sy;

      goto _Scaling;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static err_t FOG_CDECL RasterPaintEngine_blitImageAtF(Painter* self, const PointF& p, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(i, iFragment)

  uint32_t transformType = engine->finalTransform.getType();
  BoxD box(double(p.x), double(p.y), double(p.x) + double(iW), double(p.y) + double(iH));

  switch (transformType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    case TRANSFORM_TYPE_TRANSLATION:
    {
      PointD t(double(p.x) + engine->finalTransform._20,
               double(p.y) + engine->finalTransform._21);

      Fixed48x16 x48x16 = Math::fixed48x16FromFloat(t.x);
      Fixed48x16 y48x16 = Math::fixed48x16FromFloat(t.y);

      int fx = (int)(x48x16 >> 8) & 0xFF;
      int fy = (int)(y48x16 >> 8) & 0xFF;

      // Aligned.
      if ((fx | fy) == 0)
      {
        int dstX = (int)(x48x16 >> 16);
        int dstY = (int)(y48x16 >> 16);

        int d;

        if ((uint)(d = dstX - engine->ctx.finalClipBoxI.x0) >= (uint)engine->ctx.finalClipBoxI.getWidth())
        {
          dstX = engine->ctx.finalClipBoxI.x0; iX -= d;
          if (d >= 0 || (iW += d) <= 0) return ERR_OK;
        }

        if ((uint)(d = dstY - engine->ctx.finalClipBoxI.y0) >= (uint)engine->ctx.finalClipBoxI.getHeight())
        {
          dstY = engine->ctx.finalClipBoxI.y0; iY -= d;
          if (d >= 0 || (iH += d) <= 0) return ERR_OK;
        }

        if ((d = engine->ctx.finalClipBoxI.x1 - dstX) < iW) iW = d;
        if ((d = engine->ctx.finalClipBoxI.y1 - dstY) < iH) iH = d;

        return engine->serializer->blitNormalizedImageI(engine, PointI(dstX, dstY), i, RectI(iX, iY, iW, iH));
      }
      else
      {
        box.translate(engine->finalTransform._20, engine->finalTransform._21);
        if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
          return ERR_OK;

        TransformD tr(engine->finalTransform);
        tr.translate(PointD(p.x, p.y));

        return engine->serializer->blitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
      }
    }

    case TRANSFORM_TYPE_SCALING:
    {
      engine->finalTransform.mapBox(box, box);

      if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
        return ERR_OK;

      TransformD tr(engine->finalTransform);
      tr.translate(PointD(p.x, p.y));

      return engine->serializer->blitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
    }

    default:
    {
      BoxD transformedBox(UNINITIALIZED);
      engine->finalTransform.mapBox(transformedBox, box);

      if (!BoxD::intersect(transformedBox, transformedBox, engine->ctx.finalClipperD.getClipBox()))
        return ERR_OK;

      TransformD tr(engine->finalTransform);
      tr.translate(PointD(p.x, p.y));

      return engine->serializer->blitRawImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_blitImageAtD(Painter* self, const PointD& p, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(i, iFragment)

  uint32_t transformType = engine->finalTransform.getType();
  BoxD box(double(p.x), double(p.y), double(p.x) + double(iW), double(p.y) + double(iH));

  switch (transformType)
  {
    case TRANSFORM_TYPE_IDENTITY:
    case TRANSFORM_TYPE_TRANSLATION:
    {
      PointD t(double(p.x) + engine->finalTransform._20,
               double(p.y) + engine->finalTransform._21);

      Fixed48x16 x48x16 = Math::fixed48x16FromFloat(t.x);
      Fixed48x16 y48x16 = Math::fixed48x16FromFloat(t.y);

      int fx = (int)(x48x16 >> 8) & 0xFF;
      int fy = (int)(y48x16 >> 8) & 0xFF;

      // Aligned.
      if ((fx | fy) == 0)
      {
        int dstX = (int)(x48x16 >> 16);
        int dstY = (int)(y48x16 >> 16);

        int d;

        if ((uint)(d = dstX - engine->ctx.finalClipBoxI.x0) >= (uint)engine->ctx.finalClipBoxI.getWidth())
        {
          dstX = engine->ctx.finalClipBoxI.x0; iX -= d;
          if (d >= 0 || (iW += d) <= 0) return ERR_OK;
        }

        if ((uint)(d = dstY - engine->ctx.finalClipBoxI.y0) >= (uint)engine->ctx.finalClipBoxI.getHeight())
        {
          dstY = engine->ctx.finalClipBoxI.y0; iY -= d;
          if (d >= 0 || (iH += d) <= 0) return ERR_OK;
        }

        if ((d = engine->ctx.finalClipBoxI.x1 - dstX) < iW) iW = d;
        if ((d = engine->ctx.finalClipBoxI.y1 - dstY) < iH) iH = d;

        return engine->serializer->blitNormalizedImageI(engine, PointI(dstX, dstY), i, RectI(iX, iY, iW, iH));
      }
      else
      {
        box.translate(engine->finalTransform._20, engine->finalTransform._21);
        if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
          return ERR_OK;

        TransformD tr(engine->finalTransform);
        tr.translate(PointD(p.x, p.y));

        return engine->serializer->blitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
      }
    }

    case TRANSFORM_TYPE_SCALING:
    {
      engine->finalTransform.mapBox(box, box);

      if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
        return ERR_OK;

      TransformD tr(engine->finalTransform);
      tr.translate(PointD(p.x, p.y));

      return engine->serializer->blitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
    }

    default:
    {
      BoxD transformedBox(UNINITIALIZED);
      engine->finalTransform.mapBox(transformedBox, box);

      if (!BoxD::intersect(transformedBox, transformedBox, engine->ctx.finalClipperD.getClipBox()))
        return ERR_OK;

      TransformD tr(engine->finalTransform);
      tr.translate(PointD(p.x, p.y));

      return engine->serializer->blitRawImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
    }
  }
}

static err_t FOG_CDECL RasterPaintEngine_blitImageInI(Painter* self, const RectI& r, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(i, iFragment)

  // Try to use unscaled blit if possible.
  if (r.w == iW && r.h == iH) return engine->vtable->blitImageAtI(self, PointI(r.x, r.y), i, iFragment);

  uint32_t transformType = engine->finalTransform.getType();
  BoxD box(double(r.x), double(r.y), double(r.x) + double(r.w), double(r.y) + double(r.h));

  TransformD tr(engine->finalTransform);
  double sx = double(r.w) / double(iW);
  double sy = double(r.h) / double(iH);

  TransformD scaling(sx, 0.0, 0.0, sy, r.x, r.y);
  tr.transform(scaling);

  BoxD transformedBox(UNINITIALIZED);
  engine->finalTransform.mapBox(transformedBox, box);
  if (!BoxD::intersect(transformedBox, transformedBox, engine->ctx.finalClipperD.getClipBox()))
    return ERR_OK;

  if (transformType <= TRANSFORM_TYPE_SWAP)
    return engine->serializer->blitNormalizedTransformedImageD(engine, transformedBox, i, RectI(iX, iY, iW, iH), tr);
  else
    return engine->serializer->blitRawImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitImageInF(Painter* self, const RectF& r, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(i, iFragment)

  uint32_t transformType = engine->finalTransform.getType();
  BoxD box(double(r.x), double(r.y), double(r.x) + double(r.w), double(r.y) + double(r.h));

  TransformD tr(double(r.w) / double(iW), 0.0, 0.0, double(r.h) / double(iH), r.x, r.y);
  tr.transform(engine->finalTransform, MATRIX_ORDER_APPEND);

  BoxD transformedBox(UNINITIALIZED);
  engine->finalTransform.mapBox(transformedBox, box);
  if (!BoxD::intersect(transformedBox, transformedBox, engine->ctx.finalClipperD.getClipBox()))
    return ERR_OK;

  if (transformType <= TRANSFORM_TYPE_SWAP)
    return engine->serializer->blitNormalizedTransformedImageD(engine, transformedBox, i, RectI(iX, iY, iW, iH), tr);
  else
    return engine->serializer->blitRawImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
}

static err_t FOG_CDECL RasterPaintEngine_blitImageInD(Painter* self, const RectD& r, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_IMAGE_PARAMS(i, iFragment)

  uint32_t transformType = engine->finalTransform.getType();
  BoxD box(double(r.x), double(r.y), double(r.x) + double(r.w), double(r.y) + double(r.h));

  TransformD tr(double(r.w) / double(iW), 0.0, 0.0, double(r.h) / double(iH), r.x, r.y);
  tr.transform(engine->finalTransform, MATRIX_ORDER_APPEND);

  BoxD transformedBox(UNINITIALIZED);
  engine->finalTransform.mapBox(transformedBox, box);
  if (!BoxD::intersect(transformedBox, transformedBox, engine->ctx.finalClipperD.getClipBox()))
    return ERR_OK;

  if (transformType <= TRANSFORM_TYPE_SWAP)
    return engine->serializer->blitNormalizedTransformedImageD(engine, transformedBox, i, RectI(iX, iY, iW, iH), tr);
  else
    return engine->serializer->blitRawImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageAtI(Painter* self, const PointI& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageAtF(Painter* self, const PointF& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageAtD(Painter* self, const PointD& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageInI(Painter* self, const RectI& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageInF(Painter* self, const RectF& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_blitMaskedImageInD(Painter* self, const RectD& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintEngine - Clip]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_clipRectI(Painter* self, uint32_t clipOp, const RectI& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipRectF(Painter* self, uint32_t clipOp, const RectF& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipRectD(Painter* self, uint32_t clipOp, const RectD& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipRectsI(Painter* self, uint32_t clipOp, const RectI* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipRectsF(Painter* self, uint32_t clipOp, const RectF* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipRectsD(Painter* self, uint32_t clipOp, const RectD* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipPolygonI(Painter* self, uint32_t clipOp, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipPolygonF(Painter* self, uint32_t clipOp, const PointF* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipPolygonD(Painter* self, uint32_t clipOp, const PointD* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipShapeF(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipShapeD(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipPathF(Painter* self, uint32_t clipOp, const PathF& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipPathD(Painter* self, uint32_t clipOp, const PathD& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextAtI(Painter* self, uint32_t clipOp, const PointI& p, const String& text, const Font& font, const RectI* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextAtF(Painter* self, uint32_t clipOp, const PointF& p, const String& text, const Font& font, const RectF* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextAtD(Painter* self, uint32_t clipOp, const PointD& p, const String& text, const Font& font, const RectD* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextInI(Painter* self, uint32_t clipOp, const TextRectI& r, const String& text, const Font& font, const RectI* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextInF(Painter* self, uint32_t clipOp, const TextRectF& r, const String& text, const Font& font, const RectF* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipTextInD(Painter* self, uint32_t clipOp, const TextRectD& r, const String& text, const Font& font, const RectD* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskAtI(Painter* self, uint32_t clipOp, const PointI& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskAtF(Painter* self, uint32_t clipOp, const PointF& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskAtD(Painter* self, uint32_t clipOp, const PointD& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskInI(Painter* self, uint32_t clipOp, const RectI& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskInF(Painter* self, uint32_t clipOp, const RectF& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipMaskInD(Painter* self, uint32_t clipOp, const RectD& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_clipRegion(Painter* self, uint32_t clipOp, const Region& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_resetClip(Painter* self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintEngine - Layer]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_beginLayer(Painter* self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL RasterPaintEngine_endLayer(Painter* self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintEngine - Flush]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_flush(Painter* self, uint32_t flags)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);

  // TODO: MT version.
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - InitVTable]
// ============================================================================

static void RasterPaintEngine_initVTable(RasterPaintEngineVTable* v)
{
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
  // [User Params]
  // --------------------------------------------------------------------------

  v->getUserParams = RasterPaintEngine_getUserParams;
  v->setUserParams = RasterPaintEngine_setUserParams;
  v->resetUserParams = RasterPaintEngine_resetUserParams;

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
  v->getSourcePatternF = RasterPaintEngine_getSourcePatternF;
  v->getSourcePatternD = RasterPaintEngine_getSourcePatternD;

  v->setSourceArgb32 = RasterPaintEngine_setSourceArgb32;
  v->setSourceArgb64 = RasterPaintEngine_setSourceArgb64;
  v->setSourceColor = RasterPaintEngine_setSourceColor;
  v->setSourcePatternF = RasterPaintEngine_setSourcePatternF;
  v->setSourcePatternD = RasterPaintEngine_setSourcePatternD;
  v->setSourceAbstract = RasterPaintEngine_setSourceAbstract;

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  v->getTransformF = RasterPaintEngine_getTransformF;
  v->getTransformD = RasterPaintEngine_getTransformD;

  v->setTransformF = RasterPaintEngine_setTransformF;
  v->setTransformD = RasterPaintEngine_setTransformD;

  v->applyTransformF = RasterPaintEngine_applyTransformF;
  v->applyTransformD = RasterPaintEngine_applyTransformD;

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
  v->drawPolylineF = RasterPaintEngine_drawPolylineF;
  v->drawPolylineD = RasterPaintEngine_drawPolylineD;

  v->drawPolygonI = RasterPaintEngine_drawPolygonI;
  v->drawPolygonF = RasterPaintEngine_drawPolygonF;
  v->drawPolygonD = RasterPaintEngine_drawPolygonD;

  v->drawShapeF = RasterPaintEngine_drawShapeF;
  v->drawShapeD = RasterPaintEngine_drawShapeD;

  v->drawPathF = RasterPaintEngine_drawPathF;
  v->drawPathD = RasterPaintEngine_drawPathD;

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  v->fillAll = RasterPaintEngine_fillAll;

  v->fillRectI = RasterPaintEngine_fillRectI;
  v->fillRectF = RasterPaintEngine_fillRectF;
  v->fillRectD = RasterPaintEngine_fillRectD;

  v->fillRectsI = RasterPaintEngine_fillRectsI;
  v->fillRectsF = RasterPaintEngine_fillRectsF;
  v->fillRectsD = RasterPaintEngine_fillRectsD;

  v->fillPolygonI = RasterPaintEngine_fillPolygonI;
  v->fillPolygonF = RasterPaintEngine_fillPolygonF;
  v->fillPolygonD = RasterPaintEngine_fillPolygonD;

  v->fillShapeF = RasterPaintEngine_fillShapeF;
  v->fillShapeD = RasterPaintEngine_fillShapeD;

  v->fillPathF = RasterPaintEngine_fillPathF;
  v->fillPathD = RasterPaintEngine_fillPathD;

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
  // [Clip]
  // --------------------------------------------------------------------------

  v->clipRectI = RasterPaintEngine_clipRectI;
  v->clipRectF = RasterPaintEngine_clipRectF;
  v->clipRectD = RasterPaintEngine_clipRectD;

  v->clipRectsI = RasterPaintEngine_clipRectsI;
  v->clipRectsF = RasterPaintEngine_clipRectsF;
  v->clipRectsD = RasterPaintEngine_clipRectsD;

  v->clipPolygonI = RasterPaintEngine_clipPolygonI;
  v->clipPolygonF = RasterPaintEngine_clipPolygonF;
  v->clipPolygonD = RasterPaintEngine_clipPolygonD;

  v->clipShapeF = RasterPaintEngine_clipShapeF;
  v->clipShapeD = RasterPaintEngine_clipShapeD;

  v->clipPathF = RasterPaintEngine_clipPathF;
  v->clipPathD = RasterPaintEngine_clipPathD;

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
  // [Layer]
  // --------------------------------------------------------------------------

  v->beginLayer = RasterPaintEngine_beginLayer;
  v->endLayer = RasterPaintEngine_endLayer;

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  v->flush = RasterPaintEngine_flush;
}

// ============================================================================
// [Fog::RasterPaintEngine - Filler]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintFiller : public RasterFiller
{
  RasterContext* ctx;

  uint8_t* pixels;
  sysint_t stride;
  int delta;

  struct _CBlit
  {
    RenderCBlitSpanFn blit;
    RenderClosure* closure;
    RenderSolid* solid;
  };

  struct _VBlit
  {
    RenderVBlitSpanFn blit;
    RenderClosure* closure;
    RenderPatternContext* pc;
    MemoryBuffer* pb;
    RenderPatternFetcher pf;
  };

  union
  {
    _CBlit c;
    _VBlit v;
  };
};

static void FOG_FASTCALL RasterPaintFiller_prepare_solid(RasterPaintFiller* self, int y)
{
  self->pixels += self->stride * y;
  self->stride *= self->delta;
}

static void FOG_FASTCALL RasterPaintFiller_prepare_pattern(RasterPaintFiller* self, int y)
{
  self->v.pc->prepare(&self->v.pf, y, self->delta, RENDER_FETCH_REFERENCE);

  self->pixels += self->stride * y;
  self->stride *= self->delta;
}

static void FOG_FASTCALL RasterPaintFiller_process_solid(RasterPaintFiller* self, RasterSpan8* spans)
{
#if defined(FOG_DEBUG)
  RasterUtil::validateSpans<RasterSpan8>(spans, self->ctx->finalClipBoxI.x0, self->ctx->finalClipBoxI.x1);
#endif // FOG_DEBUG

  self->c.blit(self->pixels, self->c.solid, spans, self->c.closure);
  self->pixels += self->stride;
}

static void FOG_FASTCALL RasterPaintFiller_process_pattern(RasterPaintFiller* self, RasterSpan8* spans)
{
#if defined(FOG_DEBUG)
  RasterUtil::validateSpans<RasterSpan8>(spans, self->ctx->finalClipBoxI.x0, self->ctx->finalClipBoxI.x1);
#endif // FOG_DEBUG

  self->v.pf.fetch(spans, self->v.pb->getBuffer());
  self->v.blit(self->pixels, spans, self->v.closure);
  self->pixels += self->stride;
}

static void FOG_FASTCALL RasterPaintFiller_skip_solid(RasterPaintFiller* self, int step)
{
  self->pixels += self->stride * step;
}

static void FOG_FASTCALL RasterPaintFiller_skip_pattern(RasterPaintFiller* self, int step)
{
  self->pixels += self->stride * step;
  self->v.pf.skip(step);
}

// ============================================================================
// [Fog::RasterPaintSerializer - FillRasterizedShape (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillRasterizedShape8_st(RasterPaintEngine* engine, Rasterizer8* rasterizer)
{
  RasterPaintFiller filler;

  uint8_t* pixels = engine->ctx.layer.pixels;
  sysint_t stride = engine->ctx.layer.stride;
  uint32_t format = engine->ctx.layer.primaryFormat;
  uint32_t compositingOperator = engine->ctx.paintHints.compositingOperator;

  filler.ctx = &engine->ctx;
  filler.pixels = pixels;
  filler.stride = stride;
  filler.delta = engine->ctx.scope.getDelta();

  if (RasterUtil::isSolidContext(engine->ctx.pc))
  {
    bool isSrcOpaque = Face::p32PRGB32IsAlphaFF(engine->ctx.solid.prgb32.p32);

    filler._prepare = (RasterFiller::PrepareFn)RasterPaintFiller_prepare_solid;
    filler._process = (RasterFiller::ProcessFn)RasterPaintFiller_process_solid;
    filler._skip = (RasterFiller::SkipFn)RasterPaintFiller_skip_solid;

    filler.c.blit = _g2d_render.getCBlitSpan(format, compositingOperator, isSrcOpaque);
    filler.c.closure = &engine->ctx.closure;
    filler.c.solid = &engine->ctx.solid;

    rasterizer->render(&filler, engine->ctx.scanline8.instancep(), &engine->ctx.buffer);
  }
  else
  {
    _FOG_RASTER_ENSURE_PATTERN(engine);

    filler._prepare = (RasterFiller::PrepareFn)RasterPaintFiller_prepare_pattern;
    filler._process = (RasterFiller::ProcessFn)RasterPaintFiller_process_pattern;
    filler._skip = (RasterFiller::SkipFn)RasterPaintFiller_skip_pattern;

    filler.v.blit = _g2d_render.getVBlitSpan(format, compositingOperator, engine->ctx.pc->getSrcFormat());
    filler.v.closure = &engine->ctx.closure;

    filler.v.pc = engine->ctx.pc;
    filler.v.pb = &engine->ctx.buffer;

    rasterizer->render(&filler, engine->ctx.scanlineExt8.instancep(), &engine->ctx.buffer);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintSerializer - FillNormalizedBox (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedBoxI_st(
  RasterPaintEngine* engine, const BoxI& box)
{
  if (engine->ctx.precision == IMAGE_PRECISION_BYTE)
  {
    // Fast-path (clip-box and full-opacity).
    if (engine->ctx.rasterHints.opacity == 0x100 && engine->ctx.finalClipType == RASTER_CLIP_BOX)
    {
      uint8_t* pixels = engine->ctx.layer.pixels;
      sysint_t stride = engine->ctx.layer.stride;
      uint32_t format = engine->ctx.layer.primaryFormat;
      uint32_t compositingOperator = engine->ctx.paintHints.compositingOperator;

      int y0 = box.y0;
      int y1 = box.y1;

      int w = box.x1 - box.x0;
      int i = box.y1 - box.y0;

      pixels += y0 * stride;

      if (RasterUtil::isSolidContext(engine->ctx.pc))
      {
        bool isSrcOpaque = Face::p32PRGB32IsAlphaFF(engine->ctx.solid.prgb32.p32);
        RenderCBlitLineFn blitLine = _g2d_render.getCBlitLine(format, compositingOperator, isSrcOpaque);

        pixels += box.x0 * engine->ctx.layer.primaryBPP;
        do {
          blitLine(pixels, &engine->ctx.solid, w, &engine->ctx.closure);
          pixels += stride;
        } while (--i);
      }
      else
      {
        _FOG_RASTER_ENSURE_PATTERN(engine);

        RenderPatternContext* pc = engine->ctx.pc;
        RenderPatternFetcher pf;
        
        uint32_t srcFormat = pc->getSrcFormat();

        RasterSpanExt8 span[1];
        span[0].setPositionAndType(0, w, RASTER_SPAN_C);
        span[0].setConstMask(0x100);
        span[0].setNext(NULL);

        if (RasterUtil::isCompositeCopyOp(format, srcFormat, compositingOperator))
        {
          pc->prepare(&pf, y0, 1, RENDER_FETCH_COPY);

          pixels += box.x0 * engine->ctx.layer.primaryBPP;
          do {
            pf.fetch(span, pixels);
            pixels += stride;
          } while (--i);
        }
        else
        {
          pc->prepare(&pf, y0, 1, RENDER_FETCH_REFERENCE);

          RenderVBlitLineFn blitLine = _g2d_render.getVBlitLine(format, compositingOperator, srcFormat);
          uint8_t* srcPixels = engine->ctx.buffer.getBuffer();

          do {
            pf.fetch(span, srcPixels);
            blitLine(pixels, span->getData(), w, &engine->ctx.closure);
            pixels += stride;
          } while (--i);
        }
      }
      return ERR_OK;
    }
    else
    {
      BoxRasterizer8& rasterizer = engine->ctx.boxRasterizer8.instance();

      rasterizer.setSceneBox(engine->ctx.finalClipBoxI);
      rasterizer.setOpacity(engine->ctx.rasterHints.opacity);

      rasterizer.initAligned(box);
      return RasterPaintSerializer_fillRasterizedShape8_st(engine, &rasterizer);
    }
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedBoxF_st(
  RasterPaintEngine* engine, const BoxF& box)
{
  if (engine->ctx.precision == IMAGE_PRECISION_BYTE)
  {
    BoxI box24x8(UNINITIALIZED);
    box24x8.x0 = Math::fixed24x8FromFloat(box.x0);
    box24x8.y0 = Math::fixed24x8FromFloat(box.y0);
    box24x8.x1 = Math::fixed24x8FromFloat(box.x1);
    box24x8.y1 = Math::fixed24x8FromFloat(box.y1);

    BoxRasterizer8& rasterizer = engine->ctx.boxRasterizer8.instance();

    rasterizer.setSceneBox(engine->ctx.finalClipBoxI);
    rasterizer.setOpacity(engine->ctx.rasterHints.opacity);

    rasterizer.init24x8(box24x8);
    return RasterPaintSerializer_fillRasterizedShape8_st(engine, &rasterizer);
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedBoxD_st(
  RasterPaintEngine* engine, const BoxD& box)
{
  if (engine->ctx.precision == IMAGE_PRECISION_BYTE)
  {
    BoxI box24x8(UNINITIALIZED);
    box24x8.x0 = Math::fixed24x8FromFloat(box.x0);
    box24x8.y0 = Math::fixed24x8FromFloat(box.y0);
    box24x8.x1 = Math::fixed24x8FromFloat(box.x1);
    box24x8.y1 = Math::fixed24x8FromFloat(box.y1);

    BoxRasterizer8& rasterizer = engine->ctx.boxRasterizer8.instance();

    rasterizer.setSceneBox(engine->ctx.finalClipBoxI);
    rasterizer.setOpacity(engine->ctx.rasterHints.opacity);

    rasterizer.init24x8(box24x8);
    return RasterPaintSerializer_fillRasterizedShape8_st(engine, &rasterizer);
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

// ============================================================================
// [Fog::RasterPaintSerializer - DrawRawPath (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_drawRawPathF_st(
  RasterPaintEngine* engine, const PathF& path)
{
  if (engine->strokeParamsPrecision == RASTER_PRECISION_D)
  {
    engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
    engine->strokeParams.f.instance() = engine->strokeParams.d.instance();
  }

  PathF& tmp = engine->ctx.tmpPathF[0];
  tmp.clear();

  PathStrokerF stroker(engine->strokeParams.f.instance());
  stroker.strokePath(tmp, path);

  return engine->serializer->fillRawPathF(engine, tmp, FILL_RULE_NON_ZERO);
}

static err_t FOG_FASTCALL RasterPaintSerializer_drawRawPathD_st(
  RasterPaintEngine* engine, const PathD& path)
{
  if (engine->strokeParamsPrecision == RASTER_PRECISION_F)
  {
    engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
    engine->strokeParams.d.instance() = engine->strokeParams.f.instance();
  }

  PathD& tmp = engine->ctx.tmpPathD[0];
  tmp.clear();

  PathStrokerD stroker(engine->strokeParams.d.instance());
  stroker.strokePath(tmp, path);

  return engine->serializer->fillRawPathD(engine, tmp, FILL_RULE_NON_ZERO);
}

// ============================================================================
// [Fog::RasterPaintSerializer - FillRawPath (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillRawPathF_st(
  RasterPaintEngine* engine, const PathF& path, uint32_t fillRule)
{
  PathF& tmp = engine->ctx.tmpPathF[1];

  bool hasTransform = engine->ensureFinalTransformF();
  if (!hasTransform)
  {
    switch (engine->ctx.finalClipperF.initPath(path))
    {
      case PATH_CLIPPER_STATUS_CLIPPED:
        engine->serializer->fillNormalizedPathF(engine, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_STATUS_MUST_CLIP:
        tmp.clear();
        FOG_RETURN_ON_ERROR(engine->ctx.finalClipperF.continuePath(tmp, path));
        engine->serializer->fillNormalizedPathF(engine, tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp.clear();
    FOG_RETURN_ON_ERROR(engine->ctx.finalClipperF.clipPath(tmp, path, engine->finalTransformF));
    engine->serializer->fillNormalizedPathF(engine, tmp, fillRule);
    return ERR_OK;
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillRawPathD_st(
  RasterPaintEngine* engine, const PathD& path, uint32_t fillRule)
{
  PathD& tmp = engine->ctx.tmpPathD[1];

  bool hasTransform = (engine->finalTransform.getType() != TRANSFORM_TYPE_IDENTITY);
  if (!hasTransform)
  {
    switch (engine->ctx.finalClipperD.initPath(path))
    {
      case PATH_CLIPPER_STATUS_CLIPPED:
        engine->serializer->fillNormalizedPathD(engine, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_STATUS_MUST_CLIP:
        tmp.clear();
        FOG_RETURN_ON_ERROR(engine->ctx.finalClipperD.continuePath(tmp, path));
        engine->serializer->fillNormalizedPathD(engine, tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp.clear();
    FOG_RETURN_ON_ERROR(engine->ctx.finalClipperD.clipPath(tmp, path, engine->finalTransform));
    engine->serializer->fillNormalizedPathD(engine, tmp, fillRule);
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::RasterPaintSerializer - FillNormalizedPath (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedPathF_st(
  RasterPaintEngine* engine, const PathF& path, uint32_t fillRule)
{
  if (engine->ctx.precision == IMAGE_PRECISION_BYTE)
  {
    PathRasterizer8& rasterizer = engine->ctx.pathRasterizer8.instance();

    rasterizer.setSceneBox(engine->ctx.finalClipBoxI);
    rasterizer.setFillRule(fillRule);
    rasterizer.setOpacity(engine->ctx.rasterHints.opacity);

    if (FOG_IS_ERROR(rasterizer.initialize()))
      return rasterizer.getError();

    rasterizer.addPath(path);
    rasterizer.finalize();

    if (rasterizer.isValid())
      return RasterPaintSerializer_fillRasterizedShape8_st(engine, &rasterizer);
    else
      return ERR_OK;
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedPathD_st(
  RasterPaintEngine* engine, const PathD& path, uint32_t fillRule)
{
  if (engine->ctx.precision == IMAGE_PRECISION_BYTE)
  {
    PathRasterizer8& rasterizer = engine->ctx.pathRasterizer8.instance();

    rasterizer.setSceneBox(engine->ctx.finalClipBoxI);
    rasterizer.setFillRule(fillRule);
    rasterizer.setOpacity(engine->ctx.rasterHints.opacity);

    if (FOG_IS_ERROR(rasterizer.initialize()))
      return rasterizer.getError();

    rasterizer.addPath(path);
    rasterizer.finalize();

    if (rasterizer.isValid())
      return RasterPaintSerializer_fillRasterizedShape8_st(engine, &rasterizer);
    else
      return ERR_OK;
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

// ============================================================================
// [Fog::RasterPaintSerializer - BlitRawImage (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_blitRawImageD_st(
  RasterPaintEngine* engine, const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform)
{
  BoxD boxClipped(box);
  engine->finalTransform.mapBox(boxClipped, boxClipped);

  if (!BoxD::intersect(boxClipped, boxClipped, engine->ctx.finalClipperD.getClipBox()))
    return ERR_OK;

  RenderPatternContext* old = engine->ctx.pc;
  RenderPatternContext pc;

  FOG_RETURN_ON_ERROR(
    _g2d_render.texture.create(&pc,
      engine->ctx.layer.primaryFormat,
      engine->coreClipBox,
      srcImage, srcFragment,
      srcTransform, Color(), TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
  );

  PathD& path = engine->ctx.tmpPathD[0];
  path.clear();
  path.box(box);

  engine->ctx.pc = &pc;
  err_t err = engine->serializer->fillRawPathD(engine, path, FILL_RULE_NON_ZERO);
  engine->ctx.pc = old;

  pc.destroy();
  return err;
}

// ============================================================================
// [Fog::RasterPaintSerializer - BlitNormalizedImage (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_blitNormalizedImageI_st(
  RasterPaintEngine* engine, const PointI& pt, const Image& srcImage, const RectI& srcFragment)
{
  if (engine->ctx.precision == IMAGE_PRECISION_BYTE)
  {
    // Fast-path (clip-box and full-opacity).
    if (engine->ctx.finalClipType == RASTER_CLIP_BOX)
    {
      uint8_t* pixels = engine->ctx.layer.pixels;
      sysint_t stride = engine->ctx.layer.stride;
      uint32_t format = engine->ctx.layer.primaryFormat;

      const ImageData* srcD = srcImage._d;
      const uint8_t* srcPixels = srcD->first;
      sysint_t srcStride = srcD->stride;
      uint32_t srcFormat = srcD->format;

      uint32_t compositingOperator = engine->ctx.paintHints.compositingOperator;
      uint32_t opacity = engine->ctx.rasterHints.opacity;

      // --------------------------------------------------------------------------
      // [Clip == Box]
      // --------------------------------------------------------------------------

      int srcWidth = srcFragment.w;
      int srcHeight = srcFragment.h;

      int x0 = pt.x;
      int y0 = pt.y;

      int i = srcHeight;
      FOG_ASSERT(y0 + srcHeight <= engine->ctx.layer.size.h);

      pixels += y0 * stride;
      srcPixels += srcFragment.y * srcStride;

      if (opacity == 0x100)
      {
        RenderVBlitLineFn blitLine;

        pixels += x0 * engine->ctx.layer.primaryBPP;
        srcPixels += srcFragment.x * srcD->bytesPerPixel;
        engine->ctx.closure.palette = srcD->palette._d;

        // If compositing operator is SRC or SRC_OVER then any image format
        // combination is supported. However, if compositing operator is one
        // of other values, then only few image formats can be mixed together.
        if (RenderUtil::isCompositeCoreOperator(compositingOperator))
        {
          blitLine = _g2d_render.get_FuncsCompositeCore(format, compositingOperator)->vblit_line[srcFormat];

_Blit_ClipBox_Opaque_Direct:
          do {
            blitLine(pixels, srcPixels, srcWidth, &engine->ctx.closure);

            pixels += stride;
            srcPixels += srcStride;
          } while (--i);
        }
        else
        {
          uint32_t vBlitSrc = _g2d_render_compatibleFormat[format][srcFormat].srcFormat;
          uint32_t vBlitId = _g2d_render_compatibleFormat[format][srcFormat].vblitId;

          blitLine = _g2d_render.get_FuncsCompositeExt(format, compositingOperator)->vblit_line[vBlitId];
          if (srcFormat == vBlitSrc)
            goto _Blit_ClipBox_Opaque_Direct;

          uint8_t* tmpPixels = engine->ctx.buffer.getBuffer();
          RenderVBlitLineFn cvtLine = _g2d_render.get_FuncsCompositeCore(vBlitSrc, COMPOSITE_SRC)->vblit_line[srcFormat];

          do {
            cvtLine(tmpPixels, srcPixels, srcWidth, &engine->ctx.closure);
            blitLine(pixels, tmpPixels, srcWidth, &engine->ctx.closure);

            pixels += stride;
            srcPixels += srcStride;
          } while (--i);
        }

        engine->ctx.closure.palette = NULL;
      }
      else
      {
        RenderVBlitSpanFn blitSpan;

        RasterSpanExt8 span[1];
        span[0].setPositionAndType(x0, x0 + srcWidth, RASTER_SPAN_C);
        span[0].setConstMask(opacity);
        span[0].setNext(NULL);

        srcPixels += srcFragment.x * srcD->bytesPerPixel;
        engine->ctx.closure.palette = srcD->palette._d;

        // If compositing operator is SRC or SRC_OVER then any image format
        // combination is supported. However, if compositing operator is one
        // of other values, then only few image formats can be mixed together.
        if (RenderUtil::isCompositeCoreOperator(compositingOperator))
        {
          blitSpan = _g2d_render.get_FuncsCompositeCore(format, compositingOperator)->vblit_span[srcFormat];

_Blit_ClipBox_Alpha_Direct:
          do {
            // SrcPixels won't be changed, it's just needed to remove the const modifier.
            span[0].setData(const_cast<uint8_t*>(srcPixels));
            blitSpan(pixels, span, &engine->ctx.closure);

            pixels += stride;
            srcPixels += srcStride;
          } while (--i);
        }
        else
        {
          uint32_t vBlitSrc = _g2d_render_compatibleFormat[format][srcFormat].srcFormat;
          uint32_t vBlitId = _g2d_render_compatibleFormat[format][srcFormat].vblitId;

          blitSpan = _g2d_render.get_FuncsCompositeExt(format, compositingOperator)->vblit_span[vBlitId];
          if (srcFormat == vBlitSrc)
            goto _Blit_ClipBox_Alpha_Direct;

          uint8_t* tmpPixels = engine->ctx.buffer.getBuffer();
          RenderVBlitLineFn cvtLine = _g2d_render.get_FuncsCompositeCore(vBlitSrc, COMPOSITE_SRC)->vblit_line[srcFormat];

          span[0].setData(tmpPixels);

          do {
            cvtLine(tmpPixels, srcPixels, srcWidth, &engine->ctx.closure);
            blitSpan(pixels, span, &engine->ctx.closure);

            pixels += stride;
            srcPixels += srcStride;
          } while (--i);
        }

        engine->ctx.closure.palette = NULL;
      }
      return ERR_OK;
    }
    else
    {
      BoxRasterizer8& rasterizer = engine->ctx.boxRasterizer8.instance();
      BoxI box(pt.x, pt.y, pt.x + srcFragment.w, pt.y + srcFragment.h);

      rasterizer.setSceneBox(engine->ctx.finalClipBoxI);
      rasterizer.setOpacity(engine->ctx.rasterHints.opacity);

      rasterizer.initAligned(box);

      RenderPatternContext* old = engine->ctx.pc;
      RenderPatternContext pc;

      FOG_RETURN_ON_ERROR(
        _g2d_render.texture.create(&pc,
          engine->ctx.layer.primaryFormat,
          engine->coreClipBox,
          srcImage, srcFragment,
          TransformD::fromTranslation(PointD(pt)), Color(), TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
      );

      engine->ctx.pc = &pc;
      err_t err = RasterPaintSerializer_fillRasterizedShape8_st(engine, &rasterizer);
      engine->ctx.pc = old;

      pc.destroy();
      return err;
    }
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

// ============================================================================
// [Fog::RasterPaintSerializer - BlitNormalizedTrasnformedImage (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_blitNormalizedTransformedImageI_st(
  RasterPaintEngine* engine, const BoxI& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform)
{
  // Must be already clipped.
  FOG_ASSERT(engine->ctx.finalClipBoxI.subsumes(box));

  RenderPatternContext* old = engine->ctx.pc;
  RenderPatternContext pc;

  FOG_RETURN_ON_ERROR(
    _g2d_render.texture.create(&pc,
      engine->ctx.layer.primaryFormat,
      engine->coreClipBox,
      srcImage, srcFragment,
      srcTransform, Color(), TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
  );

  engine->ctx.pc = &pc;
  err_t err = engine->serializer->fillNormalizedBoxI(engine, box);
  engine->ctx.pc = old;

  pc.destroy();
  return err;
}

static err_t FOG_FASTCALL RasterPaintSerializer_blitNormalizedTransformedImageD_st(
  RasterPaintEngine* engine, const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform)
{
  // Must be already clipped.
  FOG_ASSERT(engine->ctx.finalClipperD.getClipBox().subsumes(box));

  RenderPatternContext* old = engine->ctx.pc;
  RenderPatternContext pc;

  FOG_RETURN_ON_ERROR(
    _g2d_render.texture.create(&pc,
      engine->ctx.layer.primaryFormat,
      engine->coreClipBox,
      srcImage, srcFragment,
      srcTransform, Color(), TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
  );

  engine->ctx.pc = &pc;
  err_t err = engine->serializer->fillNormalizedBoxD(engine, box);
  engine->ctx.pc = old;

  pc.destroy();
  return err;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Init (st)]
// ============================================================================

static void FOG_FASTCALL RasterPaintSerializer_init_st(RasterPaintSerializer* self)
{
  self->fillNormalizedBoxI = RasterPaintSerializer_fillNormalizedBoxI_st;
  self->fillNormalizedBoxF = RasterPaintSerializer_fillNormalizedBoxF_st;
  self->fillNormalizedBoxD = RasterPaintSerializer_fillNormalizedBoxD_st;

  self->drawRawPathF = RasterPaintSerializer_drawRawPathF_st;
  self->drawRawPathD = RasterPaintSerializer_drawRawPathD_st;
  self->fillRawPathF = RasterPaintSerializer_fillRawPathF_st;
  self->fillRawPathD = RasterPaintSerializer_fillRawPathD_st;
  self->fillNormalizedPathF = RasterPaintSerializer_fillNormalizedPathF_st;
  self->fillNormalizedPathD = RasterPaintSerializer_fillNormalizedPathD_st;

  self->blitRawImageD = RasterPaintSerializer_blitRawImageD_st;
  self->blitNormalizedImageI = RasterPaintSerializer_blitNormalizedImageI_st;
  self->blitNormalizedTransformedImageI = RasterPaintSerializer_blitNormalizedTransformedImageI_st;
  self->blitNormalizedTransformedImageD = RasterPaintSerializer_blitNormalizedTransformedImageD_st;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Init (mt)]
// ============================================================================

static void FOG_FASTCALL RasterPaintSerializer_init_mt(RasterPaintSerializer* self)
{
}

// ============================================================================
// [Fog::RasterPaintEngine]
// ============================================================================

RasterPaintEngine::RasterPaintEngine() :
  masterFlags(0),
  masterLayerId(0),
  masterMaskId(0),
  masterMaskSaved(0),
  sourceType(PATTERN_TYPE_COLOR),
  savedStateFlags(0xFF),
  strokeParamsPrecision(RASTER_PRECISION_NONE),
  coreClipType(RASTER_CLIP_NULL),
  coreClipBox(0, 0, 0, 0),
  metaOrigin(0, 0),
  userOrigin(0, 0),
  coreOrigin(0, 0),
  metaRegion(Region::infinite()),
  userRegion(Region::infinite()),
  coreRegion(Region::infinite()),
  stateAllocator(16300),
  statePool(NULL),
  state(NULL),
  pcAllocator(16300),
  pcPool(NULL)
{
  vtable = NULL;

  // Setup the essentials.
  ctx.engine = this;

  opacityF = 1.0f;
  source.color.initCustom1(Argb32(0xFF000000));
  ctx.pc = (RenderPatternContext*)NULL;

  finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_SIMPLE;
  finalTransformI._sx = 1;
  finalTransformI._sy = 1;
  finalTransformI._tx = 0;
  finalTransformI._ty = 0;

  strokeParams.f.init();
  strokeParams.d.init();

  // Disable multithreading, it may be enabled by init().
  wm = NULL;

  maxThreads = getMaxThreads();
  finalizing = 0;
}

RasterPaintEngine::~RasterPaintEngine()
{
  if (ctx.layer.imageData)
    ctx.layer.imageData->locked--;

  discardStates();
  discardSource();

  strokeParams.f.destroy();
  strokeParams.d.destroy();
}

err_t RasterPaintEngine::init(const ImageBits& imageBits, ImageData* imaged, uint32_t initFlags)
{
  // Setup the primary layer.
  ctx.layer.pixels = imageBits.data;
  ctx.layer.size = imageBits.size;
  ctx.layer.stride = imageBits.stride;
  ctx.layer.primaryFormat = imageBits.format;

  ctx.layer.imageData = imaged;
  if (imaged) imaged->locked++;

  setupLayer();
  FOG_RETURN_ON_ERROR(ctx._initPrecision(ctx.layer.precision));

  // Setup defaults.
  vtable = &RasterPaintEngine_vtable;
  serializer = &RasterPaintEngine_serializer[RASTER_MODE_ST];

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
// [Fog::RasterPaintEngine - Mode]
// ============================================================================

uint RasterPaintEngine::getMaxThreads()
{
  return Math::max<uint>(Cpu::get()->numberOfProcessors, RASTER_MAX_THREADS_SUGGESTED);
}

// ============================================================================
// [Fog::RasterPaintEngine - Transform]
// ============================================================================

bool RasterPaintEngine::doIntegralTransformAndClip(BoxI& dst, const RectI& src)
{
  int tx = finalTransformI._tx;
  int ty = finalTransformI._ty;

  int sw = src.w;
  int sh = src.h;

  switch (finalTransformI._type)
  {
    case RASTER_INTEGRAL_TRANSFORM_SWAP:
      tx += src.y * finalTransformI._sx;
      ty += src.x * finalTransformI._sy;

      sw *= finalTransformI._sy;
      sh *= finalTransformI._sx;
      goto _Scaled;

    case RASTER_INTEGRAL_TRANSFORM_SCALING:
      tx += src.x * finalTransformI._sx;
      ty += src.y * finalTransformI._sy;

      sw *= finalTransformI._sx;
      sh *= finalTransformI._sy;

_Scaled:
      dst.x0 = tx;
      dst.y0 = ty;

      tx += sw;
      ty += sh;

      dst.x1 = tx;
      dst.y1 = ty;

      if (dst.x0 > dst.x1) swap(dst.x0, dst.x1);
      if (dst.y0 > dst.y1) swap(dst.x0, dst.x1);
      return BoxI::intersect(dst, dst, ctx.finalClipBoxI);

    case RASTER_INTEGRAL_TRANSFORM_SIMPLE:
      tx += src.x;
      ty += src.y;

      dst.x0 = tx;
      dst.y0 = ty;

      tx += sw;
      ty += sh;

      dst.x1 = tx;
      dst.y1 = ty;
      return BoxI::intersect(dst, dst, ctx.finalClipBoxI);

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - State - Save ...]
// ============================================================================

void RasterPaintEngine::saveSource()
{
  FOG_ASSERT((savedStateFlags & RASTER_STATE_SOURCE) == 0);
  FOG_ASSERT(state != NULL);

  savedStateFlags |= RASTER_STATE_SOURCE;
  state->sourceType = (uint8_t)sourceType;

  switch (sourceType)
  {
    case PATTERN_TYPE_NONE:
      break;

    case PATTERN_TYPE_COLOR:
      state->source.color.initCustom1(source.color.instance());
      break;

    case PATTERN_TYPE_TEXTURE:
      state->source.texture.initCustom1(source.texture.instance());
      goto _SaveSourceContinue;

    case PATTERN_TYPE_GRADIENT:
      state->source.gradient.initCustom1(source.gradient.instance());

_SaveSourceContinue:
      state->source.transform.initCustom1(source.transform.instance());
      state->pc = NULL;
      if (!RasterUtil::isPatternContext(ctx.pc)) break;

      state->pc = ctx.pc;
      state->pc->_refCount.inc();
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

void RasterPaintEngine::saveStroke()
{
  FOG_ASSERT((savedStateFlags & RASTER_STATE_STROKE) == 0);
  FOG_ASSERT(state != NULL);

  savedStateFlags |= RASTER_STATE_STROKE;
  state->strokeParamsPrecision = strokeParamsPrecision;

  switch (state->strokeParamsPrecision)
  {
    case RASTER_PRECISION_NONE:
      break;

    case RASTER_PRECISION_BOTH:
      state->strokeParams.f.initCustom1(strokeParams.f.instance());
      // ... Fall through ...

    case RASTER_PRECISION_D:
      state->strokeParams.d.initCustom1(strokeParams.d.instance());
      break;

    case RASTER_PRECISION_F:
      state->strokeParams.f.initCustom1(strokeParams.f.instance());
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

void RasterPaintEngine::saveTransform()
{
  FOG_ASSERT((savedStateFlags & RASTER_STATE_TRANSFORM) == 0);
  FOG_ASSERT(state != NULL);

  savedStateFlags |= RASTER_STATE_TRANSFORM;
  state->userTransform.initCustom1(userTransform);
  state->finalTransform.initCustom1(finalTransform);
  state->finalTransformF.initCustom1(finalTransformF);

  state->coreTranslationI = coreTranslationI;
  state->finalTransformI._type = finalTransformI._type;
  state->finalTransformI._sx = finalTransformI._sx;
  state->finalTransformI._sy = finalTransformI._sy;
  state->finalTransformI._tx = finalTransformI._tx;
  state->finalTransformI._ty = finalTransformI._ty;
}

void RasterPaintEngine::saveClipping()
{
  FOG_ASSERT((savedStateFlags & RASTER_STATE_CLIPPING) == 0);
  FOG_ASSERT(state != NULL);

  savedStateFlags |= RASTER_STATE_CLIPPING;

  // TODO: Clipping.
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
        case PATTERN_TYPE_NONE:
        case PATTERN_TYPE_COLOR:
          break;

        case PATTERN_TYPE_TEXTURE:
          state->source.texture.destroy();
          goto _DiscardSourceContinue;

        case PATTERN_TYPE_GRADIENT:
          state->source.gradient.destroy();

_DiscardSourceContinue:
          if (state->pc != NULL && state->pc->_refCount.deref())
            destroyPatternContext(state->pc);
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }

    if (restoreFlags & RASTER_STATE_STROKE)
    {
      if (state->strokeParamsPrecision & RASTER_PRECISION_F) state->strokeParams.f.destroy();
      if (state->strokeParamsPrecision & RASTER_PRECISION_D) state->strokeParams.d.destroy();
    }

    if (restoreFlags & RASTER_STATE_TRANSFORM)
    {
      // Nothing here.
    }

    if (restoreFlags & RASTER_STATE_CLIPPING)
    {
      // TODO: Clipping.
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
  // ${IMAGE_FORMAT:BEGIN}
  static const uint8_t secondaryFromPrimary[] =
  {
    /* 00: PRGB32    -> */ IMAGE_FORMAT_PRGB32,
    /* 01: XRGB32    -> */ IMAGE_FORMAT_XRGB32,
    /* 02: RGB24     -> */ IMAGE_FORMAT_XRGB32,
    /* 03: A8        -> */ IMAGE_FORMAT_PRGB32,
    /* 04: I8        -> */ IMAGE_FORMAT_NULL  ,
    /* 05: PRGB64    -> */ IMAGE_FORMAT_PRGB64,
    /* 06: RGB48     -> */ IMAGE_FORMAT_PRGB64,
    /* 07: A16       -> */ IMAGE_FORMAT_PRGB64
  };
  // ${IMAGE_FORMAT:END}

  uint32_t primaryFormat = ctx.layer.primaryFormat;
  uint32_t secondaryFormat = secondaryFromPrimary[primaryFormat];

  const ImageFormatDescription& primaryDescription = ImageFormatDescription::getByFormat(primaryFormat);
  const ImageFormatDescription& secondaryDescription = ImageFormatDescription::getByFormat(secondaryFormat);

  ctx.layer.primaryBPP = (uint32_t)primaryDescription.getBytesPerPixel();
  ctx.layer.primaryBPL = (uint32_t)ctx.layer.size.w * ctx.layer.primaryBPP;
  ctx.layer.precision = primaryDescription.getPrecision();

  if (primaryFormat != secondaryFormat)
  {
    ctx.layer.secondaryFormat = secondaryFormat;
    ctx.layer.secondaryBPP = (uint32_t)secondaryDescription.getBytesPerPixel();
    ctx.layer.secondaryBPL = (uint32_t)ctx.layer.size.w * ctx.layer.secondaryBPP;

    ctx.layer.cvtSecondaryFromPrimary = _g2d_render.get_FuncsCompositeCore(secondaryFormat, COMPOSITE_SRC)->vblit_line[primaryFormat];
    ctx.layer.cvtPrimaryFromSecondary = _g2d_render.get_FuncsCompositeCore(primaryFormat, COMPOSITE_SRC)->vblit_line[secondaryFormat];

    FOG_ASSERT(ctx.layer.cvtSecondaryFromPrimary != NULL);
    FOG_ASSERT(ctx.layer.cvtPrimaryFromSecondary != NULL);
  }
  else
  {
    ctx.layer.secondaryFormat = IMAGE_FORMAT_NULL;
    ctx.layer.secondaryBPP = 0;
    ctx.layer.secondaryBPL = 0;

    ctx.layer.cvtSecondaryFromPrimary = NULL;
    ctx.layer.cvtPrimaryFromSecondary = NULL;
  }
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
  sourceType = PATTERN_TYPE_COLOR;

  source.color.init();
  source.transform.init();
}

void RasterPaintEngine::setupDefaultClip()
{
  BoxI boundingBox(0, 0, (int)ctx.layer.size.w, (int)ctx.layer.size.h);
  FOG_ASSERT(boundingBox.isValid());

  // Final matrix is translated by the finalOrigin, we are translating it back.
  // After this function is called it remains fully usable and valid.
  // TODO: Painter.
  //finalTransformI._tx -= finalOrigin.x;
  //finalTransformI._ty -= finalOrigin.y;

  // Clear the regions and origins and set work and final region to the bounds.
  metaOrigin.reset();
  metaRegion = Region::infinite();

  userOrigin.reset();
  userRegion = Region::infinite();

  coreClipType = RASTER_CLIP_BOX;
  coreClipBox = boundingBox;
  coreRegion = boundingBox;

  // TODO:?
  ctx.finalClipType = RASTER_CLIP_BOX;
  ctx.finalClipBoxI = boundingBox;
  ctx.finalClipperF.setClipBox(BoxF(boundingBox));
  ctx.finalClipperD.setClipBox(BoxD(boundingBox));
  ctx.finalRegion = coreRegion;

  // TODO:
  //ctx.state |= RASTER_STATE_PENDING_CLIP_REGION;
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers - Source]
// ============================================================================

void RasterPaintEngine::discardSource()
{
  switch (sourceType)
  {
    case PATTERN_TYPE_NONE:
    case PATTERN_TYPE_COLOR:
      break;

    case PATTERN_TYPE_TEXTURE:
      source.texture.destroy();
      goto _DiscardContinue;

    case PATTERN_TYPE_GRADIENT:
      source.gradient.destroy();

_DiscardContinue:
      if (source.transform->_type != TRANSFORM_TYPE_IDENTITY) source.transform->reset();

      if (RasterUtil::isPatternContext(ctx.pc) && ctx.pc->_refCount.deref())
        destroyPatternContext(ctx.pc);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

err_t RasterPaintEngine::createPatternContext()
{
  FOG_ASSERT(sourceType != PATTERN_TYPE_NONE);
  FOG_ASSERT(sourceType != PATTERN_TYPE_COLOR);

  err_t err = ERR_RT_NOT_IMPLEMENTED;

  // First try to reuse context from context-pool.
  RenderPatternContext* pc = reinterpret_cast<RenderPatternContext*>(pcPool);

  if (FOG_IS_NULL(pc))
  {
    pc = reinterpret_cast<RenderPatternContext*>(pcAllocator.alloc(sizeof(RenderPatternContext)));
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
  pc->_refCount.set(1);
  pc->reset();

  switch (sourceType)
  {
    case PATTERN_TYPE_TEXTURE:
    {
      err = _g2d_render.texture.create(pc,
        ctx.layer.primaryFormat,
        coreClipBox,
        source.texture->getImage(),
        source.texture->getFragment(),
        source.adjusted.instance(),
        source.texture->getClampColor(),
        source.texture->getTileType(),
        ctx.paintHints.imageQuality);
      break;
    }

    case PATTERN_TYPE_GRADIENT:
    {
      uint32_t gradientType = source.gradient->getGradientType();
      err = _g2d_render.gradient.create[gradientType](pc,
        ctx.layer.primaryFormat,
        coreClipBox,
        source.gradient.instance(),
        source.adjusted.instance(),
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

void RasterPaintEngine::destroyPatternContext(RenderPatternContext* pc)
{
  pc->destroy();

  reinterpret_cast<RasterAbstractLinkedList*>(pc)->next = pcPool;
  pcPool = reinterpret_cast<RasterAbstractLinkedList*>(pc);
}

// ============================================================================
// [Fog::RasterPaintEngine - Changed - Core]
// ============================================================================

err_t RasterPaintEngine::changedCoreClip()
{
  err_t err = ERR_OK;
  BoxI bounds(0, 0, ctx.layer.size.w, ctx.layer.size.h);

  masterFlags &= ~RASTER_NO_PAINT_CORE_REGION;
  coreOrigin = metaOrigin + userOrigin;

  if (metaRegion.isInfinite())
  {
    if (userRegion.isInfinite())
    {
      // Meta=Infinite, User=Infinite.
      coreRegion = Region::infinite();
    }
    else
    {
      // Meta=Infinite, User=Finite.
      err = Region::translateAndClip(coreRegion, userRegion, metaOrigin, bounds);
      if (FOG_IS_ERROR(err)) goto _Fail;
    }
  }
  else
  {
    if (userRegion.isInfinite())
    {
      // Meta=Finite, User=Infinite.
      coreRegion = metaRegion;
    }
    else
    {
      // Meta=Finite, User=Finite.
      err = Region::translateAndClip(tmpRegion[0], userRegion, metaOrigin, bounds);
      if (FOG_IS_ERROR(err)) goto _Fail;

      err = Region::combine(coreRegion, metaRegion, tmpRegion[0], REGION_OP_INTERSECT);
      if (FOG_IS_ERROR(err)) goto _Fail;
    }
  }

  // Detect the core clip-type.
  switch (coreRegion.getType())
  {
    case REGION_TYPE_EMPTY:
      masterFlags |= RASTER_NO_PAINT_CORE_REGION;

      coreClipType = RASTER_CLIP_NULL;
      coreClipBox.reset();
      break;

    case REGION_TYPE_SIMPLE:
      coreClipType = RASTER_CLIP_BOX;
      coreClipBox = coreRegion.getExtents();
      break;

    case REGION_TYPE_COMPLEX:
      coreClipType = RASTER_CLIP_REGION;
      coreClipBox = coreRegion.getExtents();
      break;

    case REGION_TYPE_INFINITE:
      coreClipType = RASTER_CLIP_BOX;
      coreClipBox = bounds;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  if (coreOrigin.x == 0 && coreOrigin.y == 0)
  {
    coreTranslationI.reset();

    coreTransform._type = TRANSFORM_TYPE_IDENTITY;
    coreTransform._20 = 0.0;
    coreTransform._21 = 0.0;
  }
  else
  {
    coreTranslationI.set(-coreOrigin.x, -coreOrigin.y);

    coreTransform._type = TRANSFORM_TYPE_TRANSLATION;
    coreTransform._20 = (double)(coreTranslationI.x);
    coreTransform._21 = (double)(coreTranslationI.y);
  }
  return ERR_OK;

_Fail:
  masterFlags |= RASTER_NO_PAINT_CORE_REGION;

  coreRegion.clear();
  coreOrigin.reset();

  return err;
}

// ============================================================================
// [Fog::RasterPaintEngine - Changed - Transform]
// ============================================================================

void RasterPaintEngine::changedTransform()
{
  masterFlags &= ~RASTER_NO_PAINT_FINAL_TRANSFORM;

  ctx.rasterHints.rectToRectTransform = 1;
  ctx.rasterHints.finalTransformF = 0;

  finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_NONE;
  finalTransformI._sx = 1;
  finalTransformI._sy = 1;
  finalTransformI._tx = 0;
  finalTransformI._ty = 0;

  if (userTransform.getType() == TRANSFORM_TYPE_IDENTITY)
    finalTransform = coreTransform;
  else
    TransformD::multiply(finalTransform, coreTransform, userTransform);

  switch (finalTransform.getType())
  {
    case TRANSFORM_TYPE_DEGENERATE:
      masterFlags |= RASTER_NO_PAINT_FINAL_TRANSFORM;
      ctx.rasterHints.rectToRectTransform = 0;
      return;

    case TRANSFORM_TYPE_PROJECTION:
      ctx.rasterHints.rectToRectTransform = 0;
      return;

    case TRANSFORM_TYPE_AFFINE:
    case TRANSFORM_TYPE_ROTATION:
      ctx.rasterHints.rectToRectTransform = 0;
      return;

    case TRANSFORM_TYPE_SWAP:
      if (!Math::isFuzzyToInt(finalTransform._01, finalTransformI._sx)) break;
      if (!Math::isFuzzyToInt(finalTransform._10, finalTransformI._sy)) break;

      finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_SWAP;
      goto _Translation;

    case TRANSFORM_TYPE_SCALING:
      if (!Math::isFuzzyToInt(finalTransform._00, finalTransformI._sx)) break;
      if (!Math::isFuzzyToInt(finalTransform._11, finalTransformI._sy)) break;

      finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_SCALING;
      goto _Translation;

    case TRANSFORM_TYPE_TRANSLATION:
      finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_SIMPLE;

_Translation:
      if (!Math::isFuzzyToInt(finalTransform._20, finalTransformI._tx)) break;
      if (!Math::isFuzzyToInt(finalTransform._21, finalTransformI._ty)) break;
      return;

    case TRANSFORM_TYPE_IDENTITY:
      finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_SIMPLE;
      return;
  }

  // Transform is not integral.
  finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_NONE;
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
  {
    return ERR_RT_INVALID_ARGUMENT;
  }

  if (rect)
  {
    if (!RasterPaintEngine_checkRect(image.getSize(), *rect))
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (!RasterPaintEngine_isFormatSupported(format))
  {
    return ERR_IMAGE_UNSUPPORTED_FORMAT;
  }

  // Fill the raw-image buffer.
  FOG_RETURN_ON_ERROR(image.detach());

  imageBits.size = image.getSize();
  imageBits.format = format;
  imageBits.stride = image.getStride();
  imageBits.data = image.getFirstX();

  if (rect)
  {
    imageBits.size.set(rect->w, rect->h);
    imageBits.data += rect->h * imageBits.stride +
                      rect->w * image.getBytesPerPixel();
  }

  return ERR_OK;
}

static err_t RasterPaintEngine_prepareToImage(ImageBits& imageBits, const ImageBits& _src, const RectI* rect)
{
  // Basic checks.
  if (rect && !RasterPaintEngine_checkRect(_src.size, *rect))
  {
    return ERR_RT_INVALID_ARGUMENT;
  }

  if (!RasterPaintEngine_isFormatSupported(_src.format))
  {
    return ERR_IMAGE_UNSUPPORTED_FORMAT;
  }

  // Fill the raw-image buffer.
  imageBits = _src;

  if (rect)
  {
    imageBits.size.set(rect->w, rect->h);
    imageBits.data += rect->h * imageBits.stride +
                      rect->w * ImageFormatDescription::getByFormat(imageBits.format).getBytesPerPixel();
  }

  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_beginImage(Painter* self, Image& image, const RectI* rect, uint32_t initFlags)
{
  err_t err;

  ImageBits imageBits(UNINITIALIZED);
  RasterPaintEngine* engine;

  // Release the painter engine.
  if (self->_engine) self->_vtable->release(self);

  // Prepare.
  if ((err = RasterPaintEngine_prepareToImage(imageBits, image, rect)) != ERR_OK) goto _Fail;

  // Create the raster painter engine.
  if ((engine = fog_new RasterPaintEngine()) == NULL)
  {
    err = ERR_RT_OUT_OF_MEMORY;
    goto _Fail;
  }

  if ((err = engine->init(imageBits, image._d, initFlags)) != ERR_OK)
  {
    fog_delete(engine);
    goto _Fail;
  }

  self->_engine = engine;
  self->_vtable = engine->vtable;
  return ERR_OK;

_Fail:
  self->_engine = _api.painter.getNullEngine();
  self->_vtable = self->_engine->vtable;
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_beginIBits(Painter* self, const ImageBits& _imageBits, const RectI* rect, uint32_t initFlags)
{
  err_t err;

  ImageBits imageBits(UNINITIALIZED);
  RasterPaintEngine* engine;

  // Release the painter engine.
  if (self->_engine) self->_vtable->release(self);

  // Prepare.
  if ((err = RasterPaintEngine_prepareToImage(imageBits, _imageBits, rect)) != ERR_OK) goto _Fail;

  // Create the raster painter engine.
  if ((engine = fog_new RasterPaintEngine()) == NULL)
  {
    err = ERR_RT_OUT_OF_MEMORY;
    goto _Fail;
  }

  if ((err = engine->init(imageBits, NULL, initFlags)) != ERR_OK)
  {
    fog_delete(engine);
    goto _Fail;
  }

  self->_engine = engine;
  self->_vtable = engine->vtable;
  return ERR_OK;

_Fail:
  self->_engine = _api.painter.getNullEngine();
  self->_vtable = self->_engine->vtable;
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_switchToImage(Painter* self, Image& image, const RectI* rect)
{
  err_t err;
  ImageBits imageBits(UNINITIALIZED);

  uint32_t deviceId;

  // Prepare.
  if ((err = RasterPaintEngine_prepareToImage(imageBits, image, rect)) != ERR_OK) goto _Fail;
  if ((err = self->getDeviceId(deviceId)) != ERR_OK) goto _Fail;

  if (deviceId == PAINT_DEVICE_RASTER)
  {
    return reinterpret_cast<RasterPaintEngine*>(self->_engine)->switchTo(imageBits, image._d);
  }
  else
  {
    return _api.painter.beginImage(self, image, rect, NO_FLAGS);
  }

_Fail:
  self->end();
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_switchToIBits(Painter* self, const ImageBits& _imageBits, const RectI* rect)
{
  err_t err;
  ImageBits imageBits(UNINITIALIZED);

  uint32_t deviceId;

  // Prepare.
  if ((err = RasterPaintEngine_prepareToImage(imageBits, _imageBits, rect)) != ERR_OK) goto _Fail;
  if ((err = self->getDeviceId(deviceId)) != ERR_OK) goto _Fail;

  if (deviceId == PAINT_DEVICE_RASTER)
    return reinterpret_cast<RasterPaintEngine*>(self->_engine)->switchTo(imageBits, NULL);
  else
    return _api.painter.beginIBits(self, _imageBits, rect, NO_FLAGS);

_Fail:
  self->end();
  return err;
}

// ============================================================================
// [Fog::RasterPaintEngine - Init / Fini]
// ============================================================================

FOG_NO_EXPORT void RasterPaintEngine_init(void)
{
  _api.painter.beginImage = RasterPaintEngine_beginImage;
  _api.painter.beginIBits = RasterPaintEngine_beginIBits;

  _api.painter.switchToImage = RasterPaintEngine_switchToImage;
  _api.painter.switchToIBits = RasterPaintEngine_switchToIBits;

  RasterPaintEngine_initVTable(&RasterPaintEngine_vtable);
  RasterPaintSerializer_init_st(&RasterPaintEngine_serializer[RASTER_MODE_ST]);
  RasterPaintSerializer_init_mt(&RasterPaintEngine_serializer[RASTER_MODE_MT]);
}

} // Fog namespace
