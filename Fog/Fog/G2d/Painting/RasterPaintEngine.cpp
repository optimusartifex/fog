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
#include <Fog/G2d/Painting/RasterPaintConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintEngine_p.h>
#include <Fog/G2d/Painting/RasterPaintFuncs_p.h>
#include <Fog/G2d/Painting/RasterPaintState_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterPaintUtil_p.h>
#include <Fog/G2d/Painting/RasterPaintWorker_p.h>
#include <Fog/G2d/Rasterizer/Rasterizer_p.h>
#include <Fog/G2d/Rasterizer/Scanline_p.h>
#include <Fog/G2d/Rasterizer/Span_p.h>
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
      FOG_RETURN_ON_ERROR(createPatternContext(_Engine_)); \
    } \
  FOG_MACRO_END

// ============================================================================
// [Fog::RasterPainterImpl_]
// ============================================================================

//! @internal
//!
//! @brief Non-template based implementation of RasterPainterImpl<>.
struct RasterPainterImpl_
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL release(Painter& self);

  // --------------------------------------------------------------------------
  // [Meta Params]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getMetaParams(const Painter& self, Region& region, PointI& origin);
  static err_t FOG_CDECL setMetaParams(Painter& self, const Region& region, const PointI& origin);
  static err_t FOG_CDECL resetMetaParams(Painter& self);

  // --------------------------------------------------------------------------
  // [User Params]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getUserParams(const Painter& self, Region& region, PointI& origin);
  static err_t FOG_CDECL setUserParams(Painter& self, const Region& region, const PointI& origin);
  static err_t FOG_CDECL resetUserParams(Painter& self);

  // --------------------------------------------------------------------------
  // [Parameters]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getParameter(const Painter& self, uint32_t parameterId, void* value);
  static err_t FOG_CDECL setParameter(Painter& self, uint32_t parameterId, const void* value);
  static err_t FOG_CDECL resetParameter(Painter& self, uint32_t parameterId);

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getSourceType(const Painter& self, uint32_t& val);
  static err_t FOG_CDECL getSourceColor(const Painter& self, Color& color);
  static err_t FOG_CDECL getSourcePatternF(const Painter& self, PatternF& pattern);
  static err_t FOG_CDECL getSourcePatternD(const Painter& self, PatternD& pattern);

  static err_t FOG_CDECL setSourceArgb32(Painter& self, uint32_t argb32);
  static err_t FOG_CDECL setSourceArgb64(Painter& self, const Argb64& argb64);
  static err_t FOG_CDECL setSourceColor(Painter& self, const Color& color);
  static err_t FOG_CDECL setSourcePatternF(Painter& self, const PatternF& pattern);
  static err_t FOG_CDECL setSourcePatternD(Painter& self, const PatternD& pattern);
  static err_t FOG_CDECL setSourceAbstract(Painter& self, uint32_t sourceId, const void* value, const void* tr);

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getTransformF(const Painter& self, TransformF& tr);
  static err_t FOG_CDECL getTransformD(const Painter& self, TransformD& tr);

  static err_t FOG_CDECL setTransformF(Painter& self, const TransformF& tr);
  static err_t FOG_CDECL setTransformD(Painter& self, const TransformD& tr);

  static err_t FOG_CDECL applyTransformF(Painter& self, uint32_t transformOp, const void* params);
  static err_t FOG_CDECL applyTransformD(Painter& self, uint32_t transformOp, const void* params);

  static err_t FOG_CDECL resetTransform(Painter& self);

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL save(Painter& self);
  static err_t FOG_CDECL restore(Painter& self);

  static FOG_INLINE RasterState* createState(RasterPaintEngine* engine);
  static FOG_INLINE void poolState(RasterPaintEngine* engine, RasterState* state);

  static void saveSource(RasterPaintEngine* engine);
  static void saveStroke(RasterPaintEngine* engine);
  static void saveTransform(RasterPaintEngine* engine);
  static void saveClipping(RasterPaintEngine* engine);

  static void discardStates(RasterPaintEngine* engine);

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL mapPointF(const Painter& self, uint32_t mapOp, PointF& pt);
  static err_t FOG_CDECL mapPointD(const Painter& self, uint32_t mapOp, PointD& pt);

  // --------------------------------------------------------------------------
  // [Mode]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint getMaxThreads();

  // --------------------------------------------------------------------------
  // [Setup]
  // --------------------------------------------------------------------------

  static void setupLayer(RasterPaintEngine* engine);
  static void setupOps(RasterPaintEngine* engine);

  static void setupDefaultClip(RasterPaintEngine* engine);
  static void setupDefaultRenderer(RasterPaintEngine* engine);

  // --------------------------------------------------------------------------
  // [Helpers - Source]
  // --------------------------------------------------------------------------

  static FOG_INLINE void discardSource(RasterPaintEngine* engine);

  static err_t createPatternContext(RasterPaintEngine* engine);
  static FOG_INLINE void destroyPatternContext(RasterPaintEngine* engine, RenderPatternContext* pc);

  // --------------------------------------------------------------------------
  // [Helpers - Transform]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool ensureFinalTransformF(RasterPaintEngine* engine);

  // --------------------------------------------------------------------------
  // [Changed - Core]
  // --------------------------------------------------------------------------

  //! @brief Called when core clip variables were changed.
  static err_t changedCoreClip(RasterPaintEngine* engine);
  //! @brief Called when transform was changed.
  static void changedTransform(RasterPaintEngine* engine);

  // --------------------------------------------------------------------------
  // [Helpers - Clipping]
  // --------------------------------------------------------------------------

  static bool doIntegralTransformAndClip(RasterPaintEngine* engine, BoxI& dst, const RectI& src);
};

// ============================================================================
// [Fog::RasterPainterImpl_ - AddRef / Release]
// ============================================================================

err_t FOG_CDECL RasterPainterImpl_::release(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  engine->finalizing = true;
  fog_delete(engine);

  self._engine = _api.painter.getNullEngine();
  self._vtable = self._engine->vtable;

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPainterImpl_ - Meta Params]
// ============================================================================

err_t FOG_CDECL RasterPainterImpl_::getMetaParams(const Painter& self, Region& region, PointI& origin)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  region = engine->metaRegion;
  origin = engine->metaOrigin;

  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::setMetaParams(Painter& self, const Region& region, const PointI& origin)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  discardStates(engine);

  engine->metaRegion = region;
  engine->metaOrigin = origin;

  engine->userRegion = Region::infinite();
  engine->userOrigin.reset();

  return changedCoreClip(engine);
}

err_t FOG_CDECL RasterPainterImpl_::resetMetaParams(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  discardStates(engine);

  engine->metaRegion = Region::infinite();
  engine->metaOrigin.reset();

  engine->userRegion = Region::infinite();
  engine->userOrigin.reset();

  return changedCoreClip(engine);
}

// ============================================================================
// [Fog::RasterPainterImpl_ - User Params]
// ============================================================================

err_t FOG_CDECL RasterPainterImpl_::getUserParams(const Painter& self, Region& region, PointI& origin)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  region = engine->userRegion;
  origin = engine->userOrigin;

  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::setUserParams(Painter& self, const Region& region, const PointI& origin)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  engine->userRegion = region;
  engine->userOrigin = origin;

  return changedCoreClip(engine);
}

err_t FOG_CDECL RasterPainterImpl_::resetUserParams(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  engine->metaRegion = Region::infinite();
  engine->metaOrigin.reset();

  return changedCoreClip(engine);
}

// ============================================================================
// [Fog::RasterPainterImpl_ - Parameters]
// ============================================================================

#define VALUE_M(_Type_) (*reinterpret_cast<_Type_*>(value))
#define VALUE_C(_Type_) (*reinterpret_cast<const _Type_*>(value))

err_t FOG_CDECL RasterPainterImpl_::getParameter(const Painter& self, uint32_t parameterId, void* value)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  switch (parameterId)
  {
    // ------------------------------------------------------------------------
    // [Backend]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_SIZE_I:
    {
      VALUE_M(SizeI).set(engine->ctx.layer.size);
      return ERR_OK;
    }

    case PAINTER_PARAMETER_SIZE_F:
    {
      VALUE_M(SizeF).set(engine->ctx.layer.size);
      return ERR_OK;
    }

    case PAINTER_PARAMETER_SIZE_D:
    {
      VALUE_M(SizeD).set(engine->ctx.layer.size);
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FORMAT_I:
    {
      VALUE_M(uint32_t) = engine->ctx.layer.primaryFormat;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_DEVICE_I:
    {
      VALUE_M(uint32_t) = PAINT_DEVICE_RASTER;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Multithreading]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_MULTITHREADED_I:
    {
      VALUE_M(uint32_t) = engine->wm ? true : false;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_MAX_THREADS_I:
    {
      VALUE_M(uint32_t) = engine->maxThreads;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_PARAMS_F:
    {
      PaintParamsF& params = VALUE_M(PaintParamsF);

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
      PaintParamsD& params = VALUE_M(PaintParamsD);

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
      VALUE_M(PaintHints) = engine->ctx.paintHints;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_COMPOSITING_OPERATOR_I:
    {
      VALUE_M(uint32_t) = engine->ctx.paintHints.compositingOperator;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_RENDER_QUALITY_I:
    {
      VALUE_M(uint32_t) = engine->ctx.paintHints.renderQuality;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_IMAGE_QUALITY_I:
    {
      VALUE_M(uint32_t) = engine->ctx.paintHints.imageQuality;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GRADIENT_QUALITY_I:
    {
      VALUE_M(uint32_t) = engine->ctx.paintHints.gradientQuality;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OUTLINED_TEXT_I:
    {
      VALUE_M(uint32_t) = engine->ctx.paintHints.outlinedText;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FAST_LINE_I:
    {
      VALUE_M(uint32_t) = engine->ctx.paintHints.fastLine;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GEOMETRIC_PRECISION_I:
    {
      VALUE_M(uint32_t) = engine->ctx.paintHints.geometricPrecision;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint - Opacity]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_OPACITY_F:
    {
      VALUE_M(float) = engine->opacityF;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OPACITY_D:
    {
      VALUE_M(double) = engine->opacityF;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Fill Params - Fill Rule]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_FILL_RULE_I:
    {
      VALUE_M(uint32_t) = engine->ctx.paintHints.fillRule;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_STROKE_PARAMS_F:
    {
      PathStrokerParamsF& params = VALUE_M(PathStrokerParamsF);

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
      PathStrokerParamsD& params = VALUE_M(PathStrokerParamsD);

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
        VALUE_M(float) = (float)engine->strokeParams.d->getLineWidth();
      else
        VALUE_M(float) = engine->strokeParams.f->getLineWidth();

      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_WIDTH_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_F)
        VALUE_M(double) = engine->strokeParams.f->getLineWidth();
      else
        VALUE_M(double) = engine->strokeParams.d->getLineWidth();

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Join]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_JOIN_I:
    {
      VALUE_M(uint32_t) = engine->strokeParams.f->getLineJoin();
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Caps]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_START_CAP_I:
    {
      VALUE_M(uint32_t) = engine->strokeParams.f->getStartCap();
      return ERR_OK;
    }

    case PAINTER_PARAMETER_END_CAP_I:
    {
      VALUE_M(uint32_t) = engine->strokeParams.f->getEndCap();
      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_CAPS_I:
    {
      uint32_t startCap = engine->strokeParams.f->getStartCap();
      uint32_t endCap = engine->strokeParams.f->getEndCap();

      if (startCap == endCap)
      {
        VALUE_M(uint32_t) = startCap;
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
        VALUE_M(float) = (float)engine->strokeParams.d->getMiterLimit();
      else
        VALUE_M(float) = engine->strokeParams.f->getMiterLimit();

      return ERR_OK;
    }

    case PAINTER_PARAMETER_MITER_LIMIT_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_F)
        VALUE_M(double) = engine->strokeParams.f->getMiterLimit();
      else
        VALUE_M(double) = engine->strokeParams.d->getMiterLimit();

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-Offset]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_OFFSET_F:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_D)
        VALUE_M(float) = (float)engine->strokeParams.d->getDashOffset();
      else
        VALUE_M(float) = engine->strokeParams.f->getDashOffset();

      return ERR_OK;
    }

    case PAINTER_PARAMETER_DASH_OFFSET_D:
    {
      if (engine->strokeParamsPrecision == RASTER_PRECISION_F)
        VALUE_M(double) = engine->strokeParams.f->getDashOffset();
      else
        VALUE_M(double) = engine->strokeParams.d->getDashOffset();

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Dash-List]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_DASH_LIST_F:
    {
      List<float>& v = VALUE_M(List<float>);

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
      List<double>& v = VALUE_M(List<double>);

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

err_t FOG_CDECL RasterPainterImpl_::setParameter(Painter& self, uint32_t parameterId, const void* value)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
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
      uint32_t v = VALUE_C(uint32_t);

      // TODO:
      return ERR_OK;
    }

    case PAINTER_PARAMETER_MAX_THREADS_I:
    {
      uint32_t v = VALUE_C(uint32_t);

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
      const PaintParamsF& v = VALUE_C(PaintParamsF);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->masterFlags &= ~(RASTER_NO_PAINT_COMPOSITING_OPERATOR |
                               RASTER_NO_PAINT_OPACITY              |
                               RASTER_NO_PAINT_STROKE               );

      engine->ctx.paintHints = v._hints;
      engine->ctx.rasterHints.opacity = (int)(v._opacity * engine->ctx.fullOpacityValueF) >> 4;
      engine->opacityF = v._opacity;
      engine->strokeParams.f.instance() = v._strokeParams;

      if (RasterUtil::isCompositingOperatorNop(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      if (engine->ctx.rasterHints.opacity == 0)
        engine->masterFlags |= RASTER_NO_PAINT_OPACITY;

      if (engine->ctx.paintHints.geometricPrecision)
      {
        engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
        engine->strokeParams.d.instance() = v._strokeParams;
      }
      else
      {
        engine->strokeParamsPrecision = RASTER_PRECISION_F;
        engine->strokeParams.d->setHints(v._strokeParams.getHints());
      }

      // TODO: ImageQuality/GradientQuality hints changed.
      // TODO: Stroke changed.

      return ERR_OK;
    }

    case PAINTER_PARAMETER_PARAMS_D:
    {
      const PaintParamsD& v = VALUE_C(PaintParamsD);
      float opacity = (float)v._opacity;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->masterFlags &= ~(RASTER_NO_PAINT_COMPOSITING_OPERATOR |
                               RASTER_NO_PAINT_OPACITY              |
                               RASTER_NO_PAINT_STROKE               );

      engine->ctx.paintHints = v._hints;
      engine->ctx.rasterHints.opacity = (int)(opacity * engine->ctx.fullOpacityValueF) >> 4;
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
      const PaintHints& v = VALUE_C(PaintHints);

      engine->masterFlags &= ~RASTER_NO_PAINT_COMPOSITING_OPERATOR;
      engine->ctx.paintHints = v;

      if (RasterUtil::isCompositingOperatorNop(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      // TODO: Something interesting might be changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_COMPOSITING_OPERATOR_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (v >= COMPOSITE_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->masterFlags &= ~RASTER_NO_PAINT_COMPOSITING_OPERATOR;
      engine->ctx.paintHints.compositingOperator = v;

      if (RasterUtil::isCompositingOperatorNop(engine->ctx.paintHints.compositingOperator))
        engine->masterFlags |= RASTER_NO_PAINT_COMPOSITING_OPERATOR;

      return ERR_OK;
    }

    case PAINTER_PARAMETER_RENDER_QUALITY_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (v >= RENDER_QUALITY_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.renderQuality = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_IMAGE_QUALITY_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (v >= IMAGE_QUALITY_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.imageQuality = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GRADIENT_QUALITY_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (v >= GRADIENT_QUALITY_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.gradientQuality = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OUTLINED_TEXT_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (v >= 2) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.outlinedText = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FAST_LINE_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (v >= 2) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.fastLine = v;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_GEOMETRIC_PRECISION_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (v >= GEOMETRIC_PRECISION_COUNT) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.geometricPrecision = v;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Paint Opacity]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_OPACITY_F:
    {
      vFloat = VALUE_C(float);
      if (vFloat < 0.0f || vFloat > 1.0f) return ERR_RT_INVALID_ARGUMENT;

_GlobalOpacityF:
      engine->masterFlags &= ~RASTER_NO_PAINT_OPACITY;
      engine->opacityF = vFloat;
      engine->ctx.rasterHints.opacity = (int)(vFloat * engine->ctx.fullOpacityValueF) >> 4;

      if (engine->ctx.rasterHints.opacity == 0) engine->masterFlags |= RASTER_NO_PAINT_OPACITY;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_OPACITY_D:
    {
      double v = VALUE_C(double);
      if (v < 0.0 || v > 1.0) return ERR_RT_INVALID_ARGUMENT;

      vFloat = (float)v;
      goto _GlobalOpacityF;
    }

    // ------------------------------------------------------------------------
    // [Fill Params - Fill Rule]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_FILL_RULE_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (FOG_UNLIKELY(v >= FILL_RULE_COUNT)) return ERR_RT_INVALID_ARGUMENT;

      engine->ctx.paintHints.fillRule = v;
      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Params]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_STROKE_PARAMS_F:
    {
      const PathStrokerParamsF& v = VALUE_C(PathStrokerParamsF);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParamsPrecision = RASTER_PRECISION_F;
      engine->strokeParams.f.instance() = v;
      engine->strokeParams.d->setHints(v.getHints());

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_STROKE_PARAMS_D:
    {
      const PathStrokerParamsD& v = VALUE_C(PathStrokerParamsD);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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
      float v = VALUE_C(float);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setLineWidth(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision) break;
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
      double v = VALUE_C(double);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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
      uint32_t v = VALUE_C(uint32_t);
      if (v >= LINE_JOIN_COUNT) return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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
      uint32_t v = VALUE_C(uint32_t);
      if (v >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setStartCap(v);
      engine->strokeParams.d->setStartCap(v);

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_END_CAP_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (v >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setEndCap(v);
      engine->strokeParams.d->setEndCap(v);

      // TODO: Stroke params changed.
      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_CAPS_I:
    {
      uint32_t v = VALUE_C(uint32_t);
      if (v >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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
      float v = VALUE_C(float);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setMiterLimit(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision) break;
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
      double v = VALUE_C(double);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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
      float v = VALUE_C(float);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setDashOffset(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision) break;
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
      double v = VALUE_C(double);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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
      const List<float>& v = VALUE_C(List<float>);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setDashList(v);

      switch (engine->strokeParamsPrecision)
      {
        case RASTER_PRECISION_F:
          break;

        case RASTER_PRECISION_NONE:
          // Initialize to 'Float' or 'Double' precision.
          engine->strokeParamsPrecision = RASTER_PRECISION_F;

          if (!engine->ctx.paintHints.geometricPrecision) break;
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
      const List<double>& v = VALUE_C(List<double>);

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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

err_t FOG_CDECL RasterPainterImpl_::resetParameter(Painter& self, uint32_t parameterId)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

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
      engine->ctx.rasterHints.opacity = engine->ctx.fullOpacityValueU;
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
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setLineWidth(1.0f);
      engine->strokeParams.d->setLineWidth(1.0);

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Join]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_LINE_JOIN_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setStartCap(LINE_JOIN_DEFAULT);
      engine->strokeParams.d->setStartCap(LINE_JOIN_DEFAULT);

      return ERR_OK;
    }

    // ------------------------------------------------------------------------
    // [Stroke Params - Line-Caps]
    // ------------------------------------------------------------------------

    case PAINTER_PARAMETER_START_CAP_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setStartCap(LINE_CAP_DEFAULT);
      engine->strokeParams.d->setStartCap(LINE_CAP_DEFAULT);

      return ERR_OK;
    }

    case PAINTER_PARAMETER_END_CAP_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
      engine->strokeParams.f->setEndCap(LINE_CAP_DEFAULT);
      engine->strokeParams.d->setEndCap(LINE_CAP_DEFAULT);

      return ERR_OK;
    }

    case PAINTER_PARAMETER_LINE_CAPS_I:
    {
      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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

      if ((engine->savedStateFlags & RASTER_STATE_STROKE) == 0) saveStroke(engine);
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

// #undef VALUE_M
// #undef VALUE_C

// ============================================================================
// [Fog::RasterPainterImpl_ - Source]
// ============================================================================

err_t FOG_CDECL RasterPainterImpl_::getSourceType(const Painter& self, uint32_t& val)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  val = engine->sourceType;
  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::getSourceColor(const Painter& self, Color& color)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if (engine->sourceType != PATTERN_TYPE_COLOR) return ERR_RT_INVALID_STATE;

  color = engine->source.color.instance();
  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::getSourcePatternF(const Painter& self, PatternF& pattern)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

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

err_t FOG_CDECL RasterPainterImpl_::getSourcePatternD(const Painter& self, PatternD& pattern)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

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

err_t FOG_CDECL RasterPainterImpl_::setSourceArgb32(Painter& self, uint32_t argb32)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0) saveSource(engine);
  discardSource(engine);
  engine->masterFlags &= ~RASTER_NO_PAINT_SOURCE;

  engine->sourceType = PATTERN_TYPE_COLOR;
  engine->source.color.initCustom1(Argb32(argb32));
  Face::p32PRGB32FromARGB32(engine->ctx.solid.prgb32.p32, argb32);

  engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
  if (engine->ctx.precision == IMAGE_PRECISION_BYTE) return ERR_OK;

  Face::p64PRGB64FromPRGB32(engine->ctx.solid.prgb64.p64, engine->ctx.solid.prgb32.p32);
  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::setSourceArgb64(Painter& self, const Argb64& argb64)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0) saveSource(engine);
  discardSource(engine);
  engine->masterFlags &= ~RASTER_NO_PAINT_SOURCE;

  engine->sourceType = PATTERN_TYPE_COLOR;
  engine->source.color.initCustom1(argb64);
  Face::p64PRGB64FromARGB64(engine->ctx.solid.prgb64.p64, argb64.p64);

  engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
  if (engine->ctx.precision == IMAGE_PRECISION_WORD) return ERR_OK;

  Face::p64PRGB32FromPRGB64(engine->ctx.solid.prgb32.p32, engine->ctx.solid.prgb64.p64);
  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::setSourceColor(Painter& self, const Color& color)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if (!color.isValid()) goto _Invalid;

  if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0) saveSource(engine);
  discardSource(engine);
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

err_t FOG_CDECL RasterPainterImpl_::setSourcePatternF(Painter& self, const PatternF& pattern)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  switch (pattern.getType())
  {
    case PATTERN_TYPE_NONE:
      if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0) saveSource(engine);
      discardSource(engine);

      engine->sourceType = PATTERN_TYPE_NONE;
      engine->masterFlags |= RASTER_NO_PAINT_SOURCE;
      engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
      return ERR_OK;

    case PATTERN_TYPE_COLOR:
      return self._vtable->setSourceColor(self, pattern._d->color.instance());

    case PATTERN_TYPE_TEXTURE:
      return self._vtable->setSourceAbstract(self, PAINTER_SOURCE_TEXTURE_F, pattern._d->texture.instancep(), &pattern._d->transform);

    case PATTERN_TYPE_GRADIENT:
      return self._vtable->setSourceAbstract(self, PAINTER_SOURCE_GRADIENT_F, pattern._d->texture.instancep(), &pattern._d->transform);

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL RasterPainterImpl_::setSourcePatternD(Painter& self, const PatternD& pattern)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  switch (pattern.getType())
  {
    case PATTERN_TYPE_NONE:
      if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0) saveSource(engine);
      discardSource(engine);

      engine->sourceType = PATTERN_TYPE_NONE;
      engine->masterFlags |= RASTER_NO_PAINT_SOURCE;
      engine->ctx.pc = (RenderPatternContext*)(size_t)0x1;
      return ERR_OK;

    case PATTERN_TYPE_COLOR:
      return self._vtable->setSourceColor(self, pattern._d->color.instance());

    case PATTERN_TYPE_TEXTURE:
      return self._vtable->setSourceAbstract(self, PAINTER_SOURCE_TEXTURE_D, pattern._d->texture.instancep(), &pattern._d->transform);

    case PATTERN_TYPE_GRADIENT:
      return self._vtable->setSourceAbstract(self, PAINTER_SOURCE_GRADIENT_D, pattern._d->texture.instancep(), &pattern._d->transform);

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL RasterPainterImpl_::setSourceAbstract(Painter& self, uint32_t sourceId, const void* value, const void* tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  if ((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0) saveSource(engine);
  discardSource(engine);
  engine->masterFlags &= ~RASTER_NO_PAINT_SOURCE;

  switch (sourceId)
  {
    // ------------------------------------------------------------------------
    // [Texture]
    // ------------------------------------------------------------------------

    case PAINTER_SOURCE_TEXTURE_F:
    case PAINTER_SOURCE_TEXTURE_D:
    {
      const Texture& v = VALUE_C(Texture);
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
      const GradientF& gradient = VALUE_C(GradientF);

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
      const GradientD& gradient = VALUE_C(GradientD);

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
// [Fog::RasterPainterImpl_ - Transform]
// ============================================================================

err_t FOG_CDECL RasterPainterImpl_::getTransformF(const Painter& self, TransformF& tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  tr = engine->userTransform;
  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::getTransformD(const Painter& self, TransformD& tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  tr = engine->userTransform;
  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::setTransformF(Painter& self, const TransformF& tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) saveTransform(engine);

  engine->userTransform = tr;
  changedTransform(engine);
  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::setTransformD(Painter& self, const TransformD& tr)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) saveTransform(engine);

  engine->userTransform = tr;
  changedTransform(engine);
  return ERR_OK;
}

err_t FOG_CDECL RasterPainterImpl_::applyTransformF(Painter& self, uint32_t transformOp, const void* params)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  err_t err = ERR_OK;

  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) saveTransform(engine);

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

  changedTransform(engine);
  return err;
}

err_t FOG_CDECL RasterPainterImpl_::applyTransformD(Painter& self, uint32_t transformOp, const void* params)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  err_t err = ERR_OK;

  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) saveTransform(engine);
  err = _api.transformd.transform(engine->userTransform, transformOp, params);
  if (FOG_IS_ERROR(err)) engine->userTransform.reset();

  changedTransform(engine);
  return err;
}

err_t FOG_CDECL RasterPainterImpl_::resetTransform(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  if ((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0) saveTransform(engine);
  engine->userTransform.reset();

  changedTransform(engine);
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPainterImpl_ - State ]
// ============================================================================

err_t FOG_CDECL RasterPainterImpl_::save(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  RasterState* prev = engine->state;
  RasterState* state = RasterPainterImpl_::createState(engine);
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

err_t FOG_CDECL RasterPainterImpl_::restore(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

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
  RasterPainterImpl_::poolState(engine, state);

  // We can simply return if there are no flags.
  if (restoreFlags == 0) return ERR_OK;

  // ------------------------------------------------------------------------
  // [Source]
  // ------------------------------------------------------------------------

  if (restoreFlags & RASTER_STATE_SOURCE)
  {
    discardSource(engine);

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
// [Fog::RasterPainterImpl_ - State - Create / Pool]
// ============================================================================

RasterState* RasterPainterImpl_::createState(RasterPaintEngine* engine)
{
  RasterState* s = engine->statePool;

  if (FOG_IS_NULL(s))
  {
    s = reinterpret_cast<RasterState*>(engine->stateAllocator.alloc(sizeof(RasterState)));
    return s;
  }
  else
  {
    engine->statePool = s->prevState;
    return s;
  }
}

void RasterPainterImpl_::poolState(RasterPaintEngine* engine, RasterState* state)
{
  state->prevState = engine->statePool;
  engine->statePool = state;
}

// ============================================================================
// [Fog::RasterPainterImpl_ - State - Save ...]
// ============================================================================

void RasterPainterImpl_::saveSource(RasterPaintEngine* engine)
{
  FOG_ASSERT((engine->savedStateFlags & RASTER_STATE_SOURCE) == 0);

  RasterState* state = engine->state;
  FOG_ASSERT(state != NULL);

  engine->savedStateFlags |= RASTER_STATE_SOURCE;
  state->sourceType = (uint8_t)engine->sourceType;

  switch (engine->sourceType)
  {
    case PATTERN_TYPE_NONE:
      break;

    case PATTERN_TYPE_COLOR:
      state->source.color.initCustom1(engine->source.color.instance());
      break;

    case PATTERN_TYPE_TEXTURE:
      state->source.texture.initCustom1(engine->source.texture.instance());
      goto _SaveSourceContinue;

    case PATTERN_TYPE_GRADIENT:
      state->source.gradient.initCustom1(engine->source.gradient.instance());

_SaveSourceContinue:
      state->source.transform.initCustom1(engine->source.transform.instance());
      state->pc = NULL;
      if (!RasterUtil::isPatternContext(engine->ctx.pc)) break;

      state->pc = engine->ctx.pc;
      state->pc->_refCount.inc();
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

void RasterPainterImpl_::saveStroke(RasterPaintEngine* engine)
{
  FOG_ASSERT((engine->savedStateFlags & RASTER_STATE_STROKE) == 0);

  RasterState* state = engine->state;
  FOG_ASSERT(state != NULL);

  engine->savedStateFlags |= RASTER_STATE_STROKE;
  state->strokeParamsPrecision = engine->strokeParamsPrecision;

  switch (state->strokeParamsPrecision)
  {
    case RASTER_PRECISION_NONE:
      break;

    case RASTER_PRECISION_BOTH:
      state->strokeParams.f.initCustom1(engine->strokeParams.f.instance());
      // ... Fall through ...

    case RASTER_PRECISION_D:
      state->strokeParams.d.initCustom1(engine->strokeParams.d.instance());
      break;

    case RASTER_PRECISION_F:
      state->strokeParams.f.initCustom1(engine->strokeParams.f.instance());
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

void RasterPainterImpl_::saveTransform(RasterPaintEngine* engine)
{
  FOG_ASSERT((engine->savedStateFlags & RASTER_STATE_TRANSFORM) == 0);

  RasterState* state = engine->state;
  FOG_ASSERT(state != NULL);

  engine->savedStateFlags |= RASTER_STATE_TRANSFORM;
  state->userTransform.initCustom1(engine->userTransform);
  state->finalTransform.initCustom1(engine->finalTransform);
  state->finalTransformF.initCustom1(engine->finalTransformF);

  state->coreTranslationI = engine->coreTranslationI;
  state->finalTransformI._type = engine->finalTransformI._type;
  state->finalTransformI._sx = engine->finalTransformI._sx;
  state->finalTransformI._sy = engine->finalTransformI._sy;
  state->finalTransformI._tx = engine->finalTransformI._tx;
  state->finalTransformI._ty = engine->finalTransformI._ty;
}

void RasterPainterImpl_::saveClipping(RasterPaintEngine* engine)
{
  FOG_ASSERT((engine->savedStateFlags & RASTER_STATE_CLIPPING) == 0);

  RasterState* state = engine->state;
  FOG_ASSERT(state != NULL);

  engine->savedStateFlags |= RASTER_STATE_CLIPPING;

  // TODO: Clipping.
}

// ============================================================================
// [Fog::RasterPainterImpl_ - State - Discard States]
// ============================================================================

void RasterPainterImpl_::discardStates(RasterPaintEngine* engine)
{
  RasterState* state = engine->state;
  RasterState* last = NULL;
  if (state == NULL) return;

  do {
    // Get the states which must be restored. It's important to destroy all
    // state resources or memory leak will occur.
    uint32_t restoreFlags = engine->savedStateFlags;

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
            destroyPatternContext(engine, state->pc);
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
  last->prevState = engine->statePool;

  engine->statePool = engine->state;
  engine->state = NULL;
}

// ============================================================================
// [Fog::RasterPainterImpl_ - Map]
// ============================================================================

err_t FOG_CDECL RasterPainterImpl_::mapPointF(const Painter& self, uint32_t mapOp, PointF& pt)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

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

err_t FOG_CDECL RasterPainterImpl_::mapPointD(const Painter& self, uint32_t mapOp, PointD& pt)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

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
// [Fog::RasterPainterImpl_ - Mode]
// ============================================================================

FOG_INLINE uint RasterPainterImpl_::getMaxThreads()
{
  return Math::max<uint>(Cpu::get()->numberOfProcessors, RASTER_MAX_THREADS_SUGGESTED);
}

// ============================================================================
// [Fog::RasterPainterImpl_ - Initialize]
// ============================================================================

void RasterPainterImpl_::setupLayer(RasterPaintEngine* engine)
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

  uint32_t primaryFormat = engine->ctx.layer.primaryFormat;
  uint32_t secondaryFormat = secondaryFromPrimary[primaryFormat];

  const ImageFormatDescription& primaryDescription = ImageFormatDescription::getByFormat(primaryFormat);
  const ImageFormatDescription& secondaryDescription = ImageFormatDescription::getByFormat(secondaryFormat);

  engine->ctx.layer.primaryBPP = (uint32_t)primaryDescription.getBytesPerPixel();
  engine->ctx.layer.primaryBPL = (uint32_t)engine->ctx.layer.size.w * engine->ctx.layer.primaryBPP;
  engine->ctx.layer.precision = primaryDescription.getPrecision();

  if (primaryFormat != secondaryFormat)
  {
    engine->ctx.layer.secondaryFormat = secondaryFormat;
    engine->ctx.layer.secondaryBPP = (uint32_t)secondaryDescription.getBytesPerPixel();
    engine->ctx.layer.secondaryBPL = (uint32_t)engine->ctx.layer.size.w * engine->ctx.layer.secondaryBPP;

    engine->ctx.layer.cvtSecondaryFromPrimary = _g2d_render.getCompositeCoreFuncs(secondaryFormat, COMPOSITE_SRC)->vblit_line[primaryFormat];
    engine->ctx.layer.cvtPrimaryFromSecondary = _g2d_render.getCompositeCoreFuncs(primaryFormat, COMPOSITE_SRC)->vblit_line[secondaryFormat];

    FOG_ASSERT(engine->ctx.layer.cvtSecondaryFromPrimary != NULL);
    FOG_ASSERT(engine->ctx.layer.cvtPrimaryFromSecondary != NULL);
  }
  else
  {
    engine->ctx.layer.secondaryFormat = IMAGE_FORMAT_NULL;
    engine->ctx.layer.secondaryBPP = 0;
    engine->ctx.layer.secondaryBPL = 0;

    engine->ctx.layer.cvtSecondaryFromPrimary = NULL;
    engine->ctx.layer.cvtPrimaryFromSecondary = NULL;
  }
}

void RasterPainterImpl_::setupOps(RasterPaintEngine* engine)
{
  engine->ctx.paintHints.packed = 0;
  engine->ctx.paintHints.compositingOperator = COMPOSITE_SRC_OVER;
  engine->ctx.paintHints.renderQuality = RENDER_QUALITY_DEFAULT;
  engine->ctx.paintHints.imageQuality = IMAGE_QUALITY_DEFAULT;
  engine->ctx.paintHints.gradientQuality = GRADIENT_QUALITY_NORMAL;
  engine->ctx.paintHints.outlinedText = 0;
  engine->ctx.paintHints.fastLine = 0;
  engine->ctx.paintHints.geometricPrecision = GEOMETRIC_PRECISION_NORMAL;
  engine->ctx.paintHints.fillRule = FILL_RULE_DEFAULT;

  engine->ctx.rasterHints.packed = 0;
  engine->ctx.rasterHints.opacity = engine->ctx.fullOpacityValueU;
  engine->ctx.rasterHints.rectToRectTransform = 1;
  engine->ctx.rasterHints.finalTransformF = 0;
  engine->ctx.rasterHints.idealLine = 1;

  engine->opacityF = 1.0f;

  // TODO: Move.
  engine->sourceType = PATTERN_TYPE_COLOR;

  engine->source.color.init();
  engine->source.transform.init();
}

void RasterPainterImpl_::setupDefaultClip(RasterPaintEngine* engine)
{
  BoxI boundingBox(
    0, 0,
    (int)engine->ctx.layer.size.w,
    (int)engine->ctx.layer.size.h);
  FOG_ASSERT(boundingBox.isValid());

  // Final matrix is translated by the finalOrigin, we are translating it back.
  // After this function is called it remains fully usable and valid.
  // TODO: Painter.
  //engine->finalTransformI._tx -= engine->finalOrigin.x;
  //engine->finalTransformI._ty -= engine->finalOrigin.y;

  // Clear the regions and origins and set work and final region to the bounds.
  engine->metaOrigin.reset();
  engine->metaRegion = Region::infinite();

  engine->userOrigin.reset();
  engine->userRegion = Region::infinite();

  engine->coreClipType = RASTER_CLIP_BOX;
  engine->coreClipBox = boundingBox;
  engine->coreRegion = boundingBox;

  // TODO:?
  engine->ctx.finalClipType = RASTER_CLIP_BOX;
  engine->ctx.finalClipBoxI = boundingBox;
  engine->ctx.finalClipperF.setClipBox(BoxF(boundingBox));
  engine->ctx.finalClipperD.setClipBox(BoxD(boundingBox));
  engine->ctx.finalRegion = engine->coreRegion;

  // TODO:
  //engine->ctx.state |= RASTER_STATE_PENDING_CLIP_REGION;
}

void RasterPainterImpl_::setupDefaultRenderer(RasterPaintEngine* engine)
{
  engine->ctx.renderer = &_RasterRender_vtable[RASTER_MODE_ST][engine->ctx.layer.precision][engine->ctx.finalClipType];
}

// ============================================================================
// [Fog::RasterPainterImpl_ - Helpers - Source]
// ============================================================================

void RasterPainterImpl_::discardSource(RasterPaintEngine* engine)
{
  switch (engine->sourceType)
  {
    case PATTERN_TYPE_NONE:
    case PATTERN_TYPE_COLOR:
      break;

    case PATTERN_TYPE_TEXTURE:
      engine->source.texture.destroy();
      goto _DiscardContinue;

    case PATTERN_TYPE_GRADIENT:
      engine->source.gradient.destroy();

_DiscardContinue:
      if (engine->source.transform->_type != TRANSFORM_TYPE_IDENTITY) engine->source.transform->reset();

      if (RasterUtil::isPatternContext(engine->ctx.pc) && engine->ctx.pc->_refCount.deref())
        destroyPatternContext(engine, engine->ctx.pc);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

err_t RasterPainterImpl_::createPatternContext(RasterPaintEngine* engine)
{
  err_t err = ERR_RT_NOT_IMPLEMENTED;
  uint32_t sourceType = engine->sourceType;

  FOG_ASSERT(sourceType != PATTERN_TYPE_NONE);
  FOG_ASSERT(sourceType != PATTERN_TYPE_COLOR);

  // First try to reuse context from context-pool.
  RenderPatternContext* pc = reinterpret_cast<RenderPatternContext*>(engine->pcPool);

  if (FOG_IS_NULL(pc))
  {
    pc = reinterpret_cast<RenderPatternContext*>(engine->pcAllocator.alloc(sizeof(RenderPatternContext)));
    if (FOG_IS_NULL(pc)) return ERR_RT_OUT_OF_MEMORY;
  }
  else
  {
    engine->pcPool = reinterpret_cast<RasterAbstractLinkedList*>(pc)->next;
  }

  engine->ctx.pc = pc;

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
        engine->ctx.layer.primaryFormat,
        engine->coreClipBox,
        engine->source.texture->getImage(),
        engine->source.texture->getFragment(),
        engine->source.adjusted.instance(),
        engine->source.texture->getClampColor(),
        engine->source.texture->getTileType(),
        engine->ctx.paintHints.imageQuality);
      break;
    }

    case PATTERN_TYPE_GRADIENT:
    {
      uint32_t gradientType = engine->source.gradient->getGradientType();
      err = _g2d_render.gradient.create[gradientType](pc,
        engine->ctx.layer.primaryFormat,
        engine->coreClipBox,
        engine->source.gradient.instance(),
        engine->source.adjusted.instance(),
        engine->ctx.paintHints.gradientQuality);
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  if (FOG_IS_ERROR(err))
  {
    reinterpret_cast<RasterAbstractLinkedList*>(pc)->next = engine->pcPool;
    engine->pcPool = reinterpret_cast<RasterAbstractLinkedList*>(pc);

    engine->ctx.pc = NULL;
  }

  return err;
}

void RasterPainterImpl_::destroyPatternContext(RasterPaintEngine* engine, RenderPatternContext* pc)
{
  pc->destroy();

  reinterpret_cast<RasterAbstractLinkedList*>(pc)->next = engine->pcPool;
  engine->pcPool = reinterpret_cast<RasterAbstractLinkedList*>(pc);
}

// ============================================================================
// [Fog::RasterPainterImpl_ - Helpers - Transform]
// ============================================================================

bool RasterPainterImpl_::ensureFinalTransformF(RasterPaintEngine* engine)
{
  if (engine->finalTransform._type == TRANSFORM_TYPE_IDENTITY) return false;

  if (!engine->ctx.rasterHints.finalTransformF)
  {
    engine->finalTransformF = engine->finalTransform;
    engine->ctx.rasterHints.finalTransformF = 1;
  }
  return true;
}

// ============================================================================
// [Fog::RasterPainterImpl_ - Changed - Core]
// ============================================================================

err_t RasterPainterImpl_::changedCoreClip(RasterPaintEngine* engine)
{
  err_t err = ERR_OK;
  BoxI bounds(0, 0, engine->ctx.layer.size.w, engine->ctx.layer.size.h);

  engine->masterFlags &= ~RASTER_NO_PAINT_CORE_REGION;
  engine->coreOrigin = engine->metaOrigin + engine->userOrigin;

  if (engine->metaRegion.isInfinite())
  {
    if (engine->userRegion.isInfinite())
    {
      // Meta=Infinite, User=Infinite.
      engine->coreRegion = Region::infinite();
    }
    else
    {
      // Meta=Infinite, User=Finite.
      err = Region::translateAndClip(engine->coreRegion, engine->userRegion, engine->metaOrigin, bounds);
      if (FOG_IS_ERROR(err)) goto _Fail;
    }
  }
  else
  {
    if (engine->userRegion.isInfinite())
    {
      // Meta=Finite, User=Infinite.
      engine->coreRegion = engine->metaRegion;
    }
    else
    {
      // Meta=Finite, User=Finite.
      err = Region::translateAndClip(engine->tmpRegion[0], engine->userRegion, engine->metaOrigin, bounds);
      if (FOG_IS_ERROR(err)) goto _Fail;

      err = Region::combine(engine->coreRegion, engine->metaRegion, engine->tmpRegion[0], REGION_OP_INTERSECT);
      if (FOG_IS_ERROR(err)) goto _Fail;
    }
  }

  // Detect the core clip-type.
  switch (engine->coreRegion.getType())
  {
    case REGION_TYPE_EMPTY:
      engine->masterFlags |= RASTER_NO_PAINT_CORE_REGION;

      engine->coreClipType = RASTER_CLIP_NULL;
      engine->coreClipBox.reset();
      break;

    case REGION_TYPE_SIMPLE:
      engine->coreClipType = RASTER_CLIP_BOX;
      engine->coreClipBox = engine->coreRegion.getExtents();
      break;

    case REGION_TYPE_COMPLEX:
      engine->coreClipType = RASTER_CLIP_REGION;
      engine->coreClipBox = engine->coreRegion.getExtents();
      break;

    case REGION_TYPE_INFINITE:
      engine->coreClipType = RASTER_CLIP_BOX;
      engine->coreClipBox = bounds;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  if (engine->coreOrigin.x == 0 && engine->coreOrigin.y == 0)
  {
    engine->coreTranslationI.reset();

    engine->coreTransform._type = TRANSFORM_TYPE_IDENTITY;
    engine->coreTransform._20 = 0.0;
    engine->coreTransform._21 = 0.0;
  }
  else
  {
    engine->coreTranslationI.set(-engine->coreOrigin.x, -engine->coreOrigin.y);

    engine->coreTransform._type = TRANSFORM_TYPE_TRANSLATION;
    engine->coreTransform._20 = (double)(engine->coreTranslationI.x);
    engine->coreTransform._21 = (double)(engine->coreTranslationI.y);
  }
  return ERR_OK;

_Fail:
  engine->masterFlags |= RASTER_NO_PAINT_CORE_REGION;

  engine->coreRegion.clear();
  engine->coreOrigin.reset();

  return err;
}

// ============================================================================
// [Fog::RasterPainterImpl_ - Changed - Transform]
// ============================================================================

void RasterPainterImpl_::changedTransform(RasterPaintEngine* engine)
{
  engine->masterFlags &= ~RASTER_NO_PAINT_FINAL_TRANSFORM;

  engine->ctx.rasterHints.rectToRectTransform = 1;
  engine->ctx.rasterHints.finalTransformF = 0;

  engine->finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_NULL;
  engine->finalTransformI._sx = 1;
  engine->finalTransformI._sy = 1;
  engine->finalTransformI._tx = 0;
  engine->finalTransformI._ty = 0;

  if (engine->userTransform.getType() == TRANSFORM_TYPE_IDENTITY)
    engine->finalTransform = engine->coreTransform;
  else
    TransformD::multiply(engine->finalTransform, engine->coreTransform, engine->userTransform);

  switch (engine->finalTransform.getType())
  {
    case TRANSFORM_TYPE_DEGENERATE:
      engine->masterFlags |= RASTER_NO_PAINT_FINAL_TRANSFORM;
      engine->ctx.rasterHints.rectToRectTransform = 0;
      return;

    case TRANSFORM_TYPE_PROJECTION:
      engine->ctx.rasterHints.rectToRectTransform = 0;
      return;

    case TRANSFORM_TYPE_AFFINE:
    case TRANSFORM_TYPE_ROTATION:
      engine->ctx.rasterHints.rectToRectTransform = 0;
      return;

    case TRANSFORM_TYPE_SWAP:
      if (!Math::isFuzzyToInt(engine->finalTransform._01, engine->finalTransformI._sx)) break;
      if (!Math::isFuzzyToInt(engine->finalTransform._10, engine->finalTransformI._sy)) break;

      engine->finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_SWAP;
      goto _Translation;

    case TRANSFORM_TYPE_SCALING:
      if (!Math::isFuzzyToInt(engine->finalTransform._00, engine->finalTransformI._sx)) break;
      if (!Math::isFuzzyToInt(engine->finalTransform._11, engine->finalTransformI._sy)) break;

      engine->finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_SCALING;
      goto _Translation;

    case TRANSFORM_TYPE_TRANSLATION:
      engine->finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_SIMPLE;

_Translation:
      if (!Math::isFuzzyToInt(engine->finalTransform._20, engine->finalTransformI._tx)) break;
      if (!Math::isFuzzyToInt(engine->finalTransform._21, engine->finalTransformI._ty)) break;
      return;

    case TRANSFORM_TYPE_IDENTITY:
      engine->finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_SIMPLE;
      return;
  }

  // Transform is not integral.
  engine->finalTransformI._type = RASTER_INTEGRAL_TRANSFORM_NULL;
}

// ============================================================================
// [Fog::RasterPainterImpl_ - Helpers - Clipping]
// ============================================================================

bool RasterPainterImpl_::doIntegralTransformAndClip(RasterPaintEngine* engine, BoxI& dst, const RectI& src)
{
  int tx = engine->finalTransformI._tx;
  int ty = engine->finalTransformI._ty;

  int sw = src.w;
  int sh = src.h;

  switch (engine->finalTransformI._type)
  {
    case RASTER_INTEGRAL_TRANSFORM_SWAP:
      tx += src.y * engine->finalTransformI._sx;
      ty += src.x * engine->finalTransformI._sy;

      sw *= engine->finalTransformI._sy;
      sh *= engine->finalTransformI._sx;
      goto _Scaled;

    case RASTER_INTEGRAL_TRANSFORM_SCALING:
      tx += src.x * engine->finalTransformI._sx;
      ty += src.y * engine->finalTransformI._sy;

      sw *= engine->finalTransformI._sx;
      sh *= engine->finalTransformI._sy;

_Scaled:
      dst.x0 = tx;
      dst.y0 = ty;

      tx += sw;
      ty += sh;

      dst.x1 = tx;
      dst.y1 = ty;

      if (dst.x0 > dst.x1) swap(dst.x0, dst.x1);
      if (dst.y0 > dst.y1) swap(dst.x0, dst.x1);
      return BoxI::intersect(dst, dst, engine->ctx.finalClipBoxI);

    case RASTER_INTEGRAL_TRANSFORM_SIMPLE:
      tx += src.x;
      ty += src.y;

      dst.x0 = tx;
      dst.y0 = ty;

      tx += sw;
      ty += sh;

      dst.x1 = tx;
      dst.y1 = ty;
      return BoxI::intersect(dst, dst, engine->ctx.finalClipBoxI);

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::RasterPainter - Impl]
// ============================================================================

template<int _MODE>
struct RasterPainterImpl : public RasterPainterImpl_
{
  // --------------------------------------------------------------------------
  // [Initialization]
  // --------------------------------------------------------------------------

  static void initVTable(RasterPainterVTable& v);

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL drawRectI(Painter& self, const RectI& r);
  static err_t FOG_CDECL drawRectF(Painter& self, const RectF& r);
  static err_t FOG_CDECL drawRectD(Painter& self, const RectD& r);

  static err_t FOG_CDECL drawPolylineI(Painter& self, const PointI* p, size_t count);
  static err_t FOG_CDECL drawPolylineF(Painter& self, const PointF* p, size_t count);
  static err_t FOG_CDECL drawPolylineD(Painter& self, const PointD* p, size_t count);

  static err_t FOG_CDECL drawPolygonI(Painter& self, const PointI* p, size_t count);
  static err_t FOG_CDECL drawPolygonF(Painter& self, const PointF* p, size_t count);
  static err_t FOG_CDECL drawPolygonD(Painter& self, const PointD* p, size_t count);

  static err_t FOG_CDECL drawShapeF(Painter& self, uint32_t shapeType, const void* shapeData);
  static err_t FOG_CDECL drawShapeD(Painter& self, uint32_t shapeType, const void* shapeData);

  static err_t FOG_CDECL drawPathF(Painter& self, const PathF& p);
  static err_t FOG_CDECL drawPathD(Painter& self, const PathD& p);

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL fillAll(Painter& self);

  static err_t FOG_CDECL fillRectI(Painter& self, const RectI& r);
  static err_t FOG_CDECL fillRectF(Painter& self, const RectF& r);
  static err_t FOG_CDECL fillRectD(Painter& self, const RectD& r);

  static err_t FOG_CDECL fillRectsI(Painter& self, const RectI* r, size_t count);
  static err_t FOG_CDECL fillRectsF(Painter& self, const RectF* r, size_t count);
  static err_t FOG_CDECL fillRectsD(Painter& self, const RectD* r, size_t count);

  static err_t FOG_CDECL fillPolygonI(Painter& self, const PointI* p, size_t count);
  static err_t FOG_CDECL fillPolygonF(Painter& self, const PointF* p, size_t count);
  static err_t FOG_CDECL fillPolygonD(Painter& self, const PointD* p, size_t count);

  static err_t FOG_CDECL fillShapeF(Painter& self, uint32_t shapeType, const void* shapeData);
  static err_t FOG_CDECL fillShapeD(Painter& self, uint32_t shapeType, const void* shapeData);

  static err_t FOG_CDECL fillPathF(Painter& self, const PathF& p);
  static err_t FOG_CDECL fillPathD(Painter& self, const PathD& p);

  static err_t FOG_CDECL fillTextAtI(Painter& self, const PointI& p, const String& text, const Font& font, const RectI* clip);
  static err_t FOG_CDECL fillTextAtF(Painter& self, const PointF& p, const String& text, const Font& font, const RectF* clip);
  static err_t FOG_CDECL fillTextAtD(Painter& self, const PointD& p, const String& text, const Font& font, const RectD* clip);

  static err_t FOG_CDECL fillTextInI(Painter& self, const TextRectI& r, const String& text, const Font& font, const RectI* clip);
  static err_t FOG_CDECL fillTextInF(Painter& self, const TextRectF& r, const String& text, const Font& font, const RectF* clip);
  static err_t FOG_CDECL fillTextInD(Painter& self, const TextRectD& r, const String& text, const Font& font, const RectD* clip);

  static err_t FOG_CDECL fillMaskAtI(Painter& self, const PointI& p, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL fillMaskAtF(Painter& self, const PointF& p, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL fillMaskAtD(Painter& self, const PointD& p, const Image& m, const RectI* mFragment);

  static err_t FOG_CDECL fillMaskInI(Painter& self, const RectI& r, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL fillMaskInF(Painter& self, const RectF& r, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL fillMaskInD(Painter& self, const RectD& r, const Image& m, const RectI* mFragment);

  static err_t FOG_CDECL fillRegion(Painter& self, const Region& r);

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL blitImageAtI(Painter& self, const PointI& p, const Image& i, const RectI* iFragment);
  static err_t FOG_CDECL blitImageAtF(Painter& self, const PointF& p, const Image& i, const RectI* iFragment);
  static err_t FOG_CDECL blitImageAtD(Painter& self, const PointD& p, const Image& i, const RectI* iFragment);

  static err_t FOG_CDECL blitImageInI(Painter& self, const RectI& r, const Image& i, const RectI* iFragment);
  static err_t FOG_CDECL blitImageInF(Painter& self, const RectF& r, const Image& i, const RectI* iFragment);
  static err_t FOG_CDECL blitImageInD(Painter& self, const RectD& r, const Image& i, const RectI* iFragment);

  static err_t FOG_CDECL blitMaskedImageAtI(Painter& self, const PointI& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);
  static err_t FOG_CDECL blitMaskedImageAtF(Painter& self, const PointF& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);
  static err_t FOG_CDECL blitMaskedImageAtD(Painter& self, const PointD& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);

  static err_t FOG_CDECL blitMaskedImageInI(Painter& self, const RectI& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);
  static err_t FOG_CDECL blitMaskedImageInF(Painter& self, const RectF& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);
  static err_t FOG_CDECL blitMaskedImageInD(Painter& self, const RectD& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL clipRectI(Painter& self, uint32_t clipOp, const RectI& r);
  static err_t FOG_CDECL clipRectF(Painter& self, uint32_t clipOp, const RectF& r);
  static err_t FOG_CDECL clipRectD(Painter& self, uint32_t clipOp, const RectD& r);

  static err_t FOG_CDECL clipRectsI(Painter& self, uint32_t clipOp, const RectI* r, size_t count);
  static err_t FOG_CDECL clipRectsF(Painter& self, uint32_t clipOp, const RectF* r, size_t count);
  static err_t FOG_CDECL clipRectsD(Painter& self, uint32_t clipOp, const RectD* r, size_t count);

  static err_t FOG_CDECL clipPolygonI(Painter& self, uint32_t clipOp, const PointI* p, size_t count);
  static err_t FOG_CDECL clipPolygonF(Painter& self, uint32_t clipOp, const PointF* p, size_t count);
  static err_t FOG_CDECL clipPolygonD(Painter& self, uint32_t clipOp, const PointD* p, size_t count);

  static err_t FOG_CDECL clipShapeF(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData);
  static err_t FOG_CDECL clipShapeD(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData);

  static err_t FOG_CDECL clipPathF(Painter& self, uint32_t clipOp, const PathF& p);
  static err_t FOG_CDECL clipPathD(Painter& self, uint32_t clipOp, const PathD& p);

  static err_t FOG_CDECL clipTextAtI(Painter& self, uint32_t clipOp, const PointI& p, const String& text, const Font& font, const RectI* clip);
  static err_t FOG_CDECL clipTextAtF(Painter& self, uint32_t clipOp, const PointF& p, const String& text, const Font& font, const RectF* clip);
  static err_t FOG_CDECL clipTextAtD(Painter& self, uint32_t clipOp, const PointD& p, const String& text, const Font& font, const RectD* clip);

  static err_t FOG_CDECL clipTextInI(Painter& self, uint32_t clipOp, const TextRectI& r, const String& text, const Font& font, const RectI* clip);
  static err_t FOG_CDECL clipTextInF(Painter& self, uint32_t clipOp, const TextRectF& r, const String& text, const Font& font, const RectF* clip);
  static err_t FOG_CDECL clipTextInD(Painter& self, uint32_t clipOp, const TextRectD& r, const String& text, const Font& font, const RectD* clip);

  static err_t FOG_CDECL clipMaskAtI(Painter& self, uint32_t clipOp, const PointI& p, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL clipMaskAtF(Painter& self, uint32_t clipOp, const PointF& p, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL clipMaskAtD(Painter& self, uint32_t clipOp, const PointD& p, const Image& m, const RectI* mFragment);

  static err_t FOG_CDECL clipMaskInI(Painter& self, uint32_t clipOp, const RectI& r, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL clipMaskInF(Painter& self, uint32_t clipOp, const RectF& r, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL clipMaskInD(Painter& self, uint32_t clipOp, const RectD& r, const Image& m, const RectI* mFragment);

  static err_t FOG_CDECL clipRegion(Painter& self, uint32_t clipOp, const Region& r);

  static err_t FOG_CDECL resetClip(Painter& self);

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL beginLayer(Painter& self);
  static err_t FOG_CDECL endLayer(Painter& self);

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL flush(Painter& self, uint32_t flags);

  // --------------------------------------------------------------------------
  // [Painting - FillNormalizedBox]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t doFillNormalizedBoxI(RasterPaintEngine* engine, const BoxI& box);
  static FOG_INLINE err_t doFillNormalizedBoxF(RasterPaintEngine* engine, const BoxF& box);
  static FOG_INLINE err_t doFillNormalizedBoxD(RasterPaintEngine* engine, const BoxD& box);

  // --------------------------------------------------------------------------
  // [Painting - FillNormalizedPath]
  // --------------------------------------------------------------------------

  static FOG_NO_INLINE err_t doFillNormalizedPathF(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule);
  static FOG_NO_INLINE err_t doFillNormalizedPathD(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule);

  // --------------------------------------------------------------------------
  // [Painting - FillUntransformedPath]
  // --------------------------------------------------------------------------

  static err_t doFillUntransformedPathF(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule, bool clip);
  static err_t doFillUntransformedPathD(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule, bool clip);

  // --------------------------------------------------------------------------
  // [Painting - Blit]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t doBlitNormalizedImageI(RasterPaintEngine* engine,
    const PointI& pt, const Image& srcImage, const RectI& srcFragment);

  static err_t doBlitNormalizedTransformedImageI(RasterPaintEngine* engine,
    const BoxI& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform);

  static err_t doBlitNormalizedTransformedImageD(RasterPaintEngine* engine,
    const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform);

  static err_t doBlitUntransformedImageD(RasterPaintEngine* engine,
    const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform);
};

// ============================================================================
// [Fog::RasterPainterImpl<> - Initialization]
// ============================================================================

template<int _MODE>
void RasterPainterImpl<_MODE>::initVTable(RasterPainterVTable& v)
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  v.release = release;

  // --------------------------------------------------------------------------
  // [Meta Params]
  // --------------------------------------------------------------------------

  v.getMetaParams = getMetaParams;
  v.setMetaParams = setMetaParams;
  v.resetMetaParams = resetMetaParams;

  // --------------------------------------------------------------------------
  // [User Params]
  // --------------------------------------------------------------------------

  v.getUserParams = getUserParams;
  v.setUserParams = setUserParams;
  v.resetUserParams = resetUserParams;

  // --------------------------------------------------------------------------
  // [Parameters]
  // --------------------------------------------------------------------------

  v.getParameter = getParameter;
  v.setParameter = setParameter;
  v.resetParameter = resetParameter;

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  v.getSourceType = getSourceType;
  v.getSourceColor = getSourceColor;
  v.getSourcePatternF = getSourcePatternF;
  v.getSourcePatternD = getSourcePatternD;

  v.setSourceArgb32 = setSourceArgb32;
  v.setSourceArgb64 = setSourceArgb64;
  v.setSourceColor = setSourceColor;
  v.setSourcePatternF = setSourcePatternF;
  v.setSourcePatternD = setSourcePatternD;
  v.setSourceAbstract = setSourceAbstract;

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  v.getTransformF = getTransformF;
  v.getTransformD = getTransformD;

  v.setTransformF = setTransformF;
  v.setTransformD = setTransformD;

  v.applyTransformF = applyTransformF;
  v.applyTransformD = applyTransformD;

  v.resetTransform = resetTransform;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  v.save = save;
  v.restore = restore;

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  v.mapPointF = mapPointF;
  v.mapPointD = mapPointD;

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  v.drawRectI = drawRectI;
  v.drawRectF = drawRectF;
  v.drawRectD = drawRectD;

  v.drawPolylineI = drawPolylineI;
  v.drawPolylineF = drawPolylineF;
  v.drawPolylineD = drawPolylineD;

  v.drawPolygonI = drawPolygonI;
  v.drawPolygonF = drawPolygonF;
  v.drawPolygonD = drawPolygonD;

  v.drawShapeF = drawShapeF;
  v.drawShapeD = drawShapeD;

  v.drawPathF = drawPathF;
  v.drawPathD = drawPathD;

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  v.fillAll = fillAll;

  v.fillRectI = fillRectI;
  v.fillRectF = fillRectF;
  v.fillRectD = fillRectD;

  v.fillRectsI = fillRectsI;
  v.fillRectsF = fillRectsF;
  v.fillRectsD = fillRectsD;

  v.fillPolygonI = fillPolygonI;
  v.fillPolygonF = fillPolygonF;
  v.fillPolygonD = fillPolygonD;

  v.fillShapeF = fillShapeF;
  v.fillShapeD = fillShapeD;

  v.fillPathF = fillPathF;
  v.fillPathD = fillPathD;

  v.fillTextAtI = fillTextAtI;
  v.fillTextAtF = fillTextAtF;
  v.fillTextAtD = fillTextAtD;

  v.fillTextInI = fillTextInI;
  v.fillTextInF = fillTextInF;
  v.fillTextInD = fillTextInD;

  v.fillMaskAtI = fillMaskAtI;
  v.fillMaskAtF = fillMaskAtF;
  v.fillMaskAtD = fillMaskAtD;

  v.fillMaskInI = fillMaskInI;
  v.fillMaskInF = fillMaskInF;
  v.fillMaskInD = fillMaskInD;

  v.fillRegion = fillRegion;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  v.blitImageAtI = blitImageAtI;
  v.blitImageAtF = blitImageAtF;
  v.blitImageAtD = blitImageAtD;

  v.blitImageInI = blitImageInI;
  v.blitImageInF = blitImageInF;
  v.blitImageInD = blitImageInD;

  v.blitMaskedImageAtI = blitMaskedImageAtI;
  v.blitMaskedImageAtF = blitMaskedImageAtF;
  v.blitMaskedImageAtD = blitMaskedImageAtD;

  v.blitMaskedImageInI = blitMaskedImageInI;
  v.blitMaskedImageInF = blitMaskedImageInF;
  v.blitMaskedImageInD = blitMaskedImageInD;

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  v.clipRectI = clipRectI;
  v.clipRectF = clipRectF;
  v.clipRectD = clipRectD;

  v.clipRectsI = clipRectsI;
  v.clipRectsF = clipRectsF;
  v.clipRectsD = clipRectsD;

  v.clipPolygonI = clipPolygonI;
  v.clipPolygonF = clipPolygonF;
  v.clipPolygonD = clipPolygonD;

  v.clipShapeF = clipShapeF;
  v.clipShapeD = clipShapeD;

  v.clipPathF = clipPathF;
  v.clipPathD = clipPathD;

  v.clipTextAtI = clipTextAtI;
  v.clipTextAtF = clipTextAtF;
  v.clipTextAtD = clipTextAtD;

  v.clipTextInI = clipTextInI;
  v.clipTextInF = clipTextInF;
  v.clipTextInD = clipTextInD;

  v.clipMaskAtI = clipMaskAtI;
  v.clipMaskAtF = clipMaskAtF;
  v.clipMaskAtD = clipMaskAtD;

  v.clipMaskInI = clipMaskInI;
  v.clipMaskInF = clipMaskInF;
  v.clipMaskInD = clipMaskInD;

  v.clipRegion = clipRegion;

  v.resetClip = resetClip;

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  v.beginLayer = beginLayer;
  v.endLayer = endLayer;

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  v.flush = flush;
}

// ============================================================================
// [Fog::RasterPainterImpl<> - Clear]
// ============================================================================

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillAll(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  doFillNormalizedBoxI(engine, engine->ctx.finalClipBoxI);
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPainterImpl<> - Draw]
// ============================================================================

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawRectI(Painter& self, const RectI& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (engine->ctx.paintHints.geometricPrecision)
    return engine->vtable->drawRectD(self, RectD(r));
  else
    return engine->vtable->drawRectF(self, RectF(r));
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawRectF(Painter& self, const RectF& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathF& path = engine->ctx.tmpPathF[2];
  path.clear();
  path.rect(r);
  return engine->vtable->drawPathF(self, path);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawRectD(Painter& self, const RectD& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathD& path = engine->ctx.tmpPathD[2];
  path.clear();
  path.rect(r);
  return engine->vtable->drawPathD(self, path);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawPolylineI(Painter& self, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (engine->ctx.paintHints.geometricPrecision)
  {
    PathD& path = engine->ctx.tmpPathD[2];
    path.clear();
    path.polyline(p, count);
    return engine->vtable->drawPathD(self, path);
  }
  else
  {
    PathF& path = engine->ctx.tmpPathF[2];
    path.clear();
    path.polyline(p, count);
    return engine->vtable->drawPathF(self, path);
  }
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawPolylineF(Painter& self, const PointF* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathF& path = engine->ctx.tmpPathF[2];
  path.clear();
  path.polyline(p, count);
  return engine->vtable->drawPathF(self, path);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawPolylineD(Painter& self, const PointD* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathD& path = engine->ctx.tmpPathD[2];
  path.clear();
  path.polyline(p, count);
  return engine->vtable->drawPathD(self, path);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawPolygonI(Painter& self, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (engine->ctx.paintHints.geometricPrecision)
  {
    PathD& path = engine->ctx.tmpPathD[2];
    path.clear();
    path.polygon(p, count);
    return engine->vtable->drawPathD(self, path);
  }
  else
  {
    PathF& path = engine->ctx.tmpPathF[2];
    path.clear();
    path.polygon(p, count);
    return engine->vtable->drawPathF(self, path);
  }
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawPolygonF(Painter& self, const PointF* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathF& path = engine->ctx.tmpPathF[2];
  path.clear();
  path.polygon(p, count);
  return engine->vtable->drawPathF(self, path);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawPolygonD(Painter& self, const PointD* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathD& path = engine->ctx.tmpPathD[2];
  path.clear();
  path.polygon(p, count);
  return engine->vtable->drawPathD(self, path);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawShapeF(Painter& self, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathF& path = engine->ctx.tmpPathF[2];
  path.clear();
  path._shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
  return engine->vtable->drawPathF(self, path);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawShapeD(Painter& self, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  PathD& path = engine->ctx.tmpPathD[2];
  path.clear();
  path._shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
  return engine->vtable->drawPathD(self, path);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawPathF(Painter& self, const PathF& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (engine->strokeParamsPrecision == RASTER_PRECISION_D)
  {
    engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
    engine->strokeParams.f.instance() = engine->strokeParams.d.instance();
  }

  PathF& tmp = engine->ctx.tmpPathF[0];
  tmp.clear();

  PathStrokerF stroker(engine->strokeParams.f.instance());
  stroker.strokePath(tmp, p);

  return doFillUntransformedPathF(engine, tmp, FILL_RULE_NON_ZERO, true);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::drawPathD(Painter& self, const PathD& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_STROKE_FUNC();

  if (engine->strokeParamsPrecision == RASTER_PRECISION_F)
  {
    engine->strokeParamsPrecision = RASTER_PRECISION_BOTH;
    engine->strokeParams.d.instance() = engine->strokeParams.f.instance();
  }

  PathD& tmp = engine->ctx.tmpPathD[0];
  tmp.clear();

  PathStrokerD stroker(engine->strokeParams.d.instance());
  stroker.strokePath(tmp, p);

  return doFillUntransformedPathD(engine, tmp, FILL_RULE_NON_ZERO, true);
}

// ============================================================================
// [Fog::RasterPainterImpl<> - Fill]
// ============================================================================

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillRectI(Painter& self, const RectI& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (engine->finalTransformI._type == RASTER_INTEGRAL_TRANSFORM_NULL)
  {
    if (engine->ctx.paintHints.geometricPrecision)
      return fillRectD(self, RectD(r));
    else
      return fillRectF(self, RectF(r));
  }
  else
  {
    BoxI box(UNINITIALIZED);
    if (doIntegralTransformAndClip(engine, box, r))
      return doFillNormalizedBoxI(engine, box);
    else
      return ERR_OK;
  }
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillRectF(Painter& self, const RectF& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  BoxF box(r);
  if (ensureFinalTransformF(engine))
  {
    if (!engine->ctx.rasterHints.rectToRectTransform)
    {
      PathF& path = engine->ctx.tmpPathF[0];
      path.clear();
      path.rect(r, PATH_DIRECTION_CW);
      return doFillUntransformedPathF(engine, path, FILL_RULE_NON_ZERO, true);
    }

    engine->finalTransformF.mapBox(box, box);
  }

  if (BoxF::intersect(box, box, engine->ctx.finalClipperF.getClipBox()))
    return doFillNormalizedBoxF(engine, box);
  else
    return ERR_OK;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillRectD(Painter& self, const RectD& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  BoxD box(r);
  if (engine->finalTransform.getType() != TRANSFORM_TYPE_IDENTITY)
  {
    if (!engine->ctx.rasterHints.rectToRectTransform)
    {
      PathD& path = engine->ctx.tmpPathD[0];
      path.clear();
      path.rect(r, PATH_DIRECTION_CW);
      return doFillUntransformedPathD(engine, path, FILL_RULE_NON_ZERO, true);
    }

    engine->finalTransform.mapBox(box, box);
  }

  if (BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
    return doFillNormalizedBoxD(engine, box);
  else
    return ERR_OK;
}

// TODO: It's easy to clip rectangles, so clip them here and call
// doFillNormalizedPath() instead.

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillRectsI(Painter& self, const RectI* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (engine->ctx.paintHints.geometricPrecision)
  {
    PathD& path = engine->ctx.tmpPathD[0];
    path.clear();
    path.rects(r, count);
    return doFillUntransformedPathD(engine, path, FILL_RULE_NON_ZERO, true);
  }
  else
  {
    PathF& path = engine->ctx.tmpPathF[0];
    path.clear();
    path.rects(r, count);
    return doFillUntransformedPathF(engine, path, FILL_RULE_NON_ZERO, true);
  }
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillRectsF(Painter& self, const RectF* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (count == 0) return ERR_OK;
  if (count == 1) return fillRectF(self, *r);

  PathF& path = engine->ctx.tmpPathF[0];
  path.clear();
  path.rects(r, count, PATH_DIRECTION_CW);

  return doFillUntransformedPathF(engine, path, FILL_RULE_NON_ZERO, true);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillRectsD(Painter& self, const RectD* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (count == 0) return ERR_OK;
  if (count == 1) return fillRectD(self, *r);

  PathD& path = engine->ctx.tmpPathD[0];
  path.clear();
  path.rects(r, count, PATH_DIRECTION_CW);
  return doFillUntransformedPathD(engine, path, FILL_RULE_NON_ZERO, true);
}

// TODO: It's easy to clip polygon, do it here, PathClipper should be enabled to do that.

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillPolygonI(Painter& self, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  if (engine->ctx.paintHints.geometricPrecision)
  {
    PathD& path = engine->ctx.tmpPathD[0];
    path.clear();
    path.polygon(p, count, PATH_DIRECTION_CW);
    return doFillUntransformedPathD(engine, path, engine->ctx.paintHints.fillRule, true);
  }
  else
  {
    PathF& path = engine->ctx.tmpPathF[0];
    path.clear();
    path.polygon(p, count, PATH_DIRECTION_CW);
    return doFillUntransformedPathF(engine, path, engine->ctx.paintHints.fillRule, true);
  }
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillPolygonF(Painter& self, const PointF* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  PathF& path = engine->ctx.tmpPathF[0];
  path.clear();
  path.polygon(p, count, PATH_DIRECTION_CW);
  return doFillUntransformedPathF(engine, path, engine->ctx.paintHints.fillRule, true);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillPolygonD(Painter& self, const PointD* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  PathD& path = engine->ctx.tmpPathD[0];
  path.clear();
  path.polygon(p, count, PATH_DIRECTION_CW);
  return doFillUntransformedPathD(engine, path, engine->ctx.paintHints.fillRule, true);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillShapeF(Painter& self, uint32_t shapeType, const void* shapeData)
{
  if (shapeType == SHAPE_TYPE_RECT)
    return fillRectF(self, *(const RectF*)shapeData);

  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  PathF& path = engine->ctx.tmpPathF[0];
  path.clear();
  path._shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
  return doFillUntransformedPathF(engine, path, engine->ctx.paintHints.fillRule, true);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillShapeD(Painter& self, uint32_t shapeType, const void* shapeData)
{
  if (shapeType == SHAPE_TYPE_RECT)
    return fillRectD(self, *(const RectD*)shapeData);

  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  PathD& path = engine->ctx.tmpPathD[0];
  path.clear();
  path._shape(shapeType, shapeData, PATH_DIRECTION_CW, NULL);
  return doFillUntransformedPathD(engine, path, engine->ctx.paintHints.fillRule, true);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillPathF(Painter& self, const PathF& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  return doFillUntransformedPathF(engine, p, engine->ctx.paintHints.fillRule, true);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillPathD(Painter& self, const PathD& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  return doFillUntransformedPathD(engine, p, engine->ctx.paintHints.fillRule, true);
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillTextAtI(Painter& self, const PointI& p, const String& text, const Font& font, const RectI* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillTextAtF(Painter& self, const PointF& p, const String& text, const Font& font, const RectF* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillTextAtD(Painter& self, const PointD& p, const String& text, const Font& font, const RectD* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillTextInI(Painter& self, const TextRectI& r, const String& text, const Font& font, const RectI* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillTextInF(Painter& self, const TextRectF& r, const String& text, const Font& font, const RectF* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillTextInD(Painter& self, const TextRectD& r, const String& text, const Font& font, const RectD* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillMaskAtI(Painter& self, const PointI& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillMaskAtF(Painter& self, const PointF& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillMaskAtD(Painter& self, const PointD& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillMaskInI(Painter& self, const RectI& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillMaskInF(Painter& self, const RectF& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillMaskInD(Painter& self, const RectD& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::fillRegion(Painter& self, const Region& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  _FOG_RASTER_ENTER_FILL_FUNC();

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPainterImpl<> - Blit]
// ============================================================================

#define _FOG_RASTER_GET_IMAGE_PARAMS(_Image_, _ImageFragment_) \
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
    if ((uint)(iX) >= (uint)iW ||        \
        (uint)(iY) >= (uint)iH ||        \
        (uint)(_ImageFragment_->w - iX) > (uint)iW || \
        (uint)(_ImageFragment_->h - iY) > (uint)iH)  \
    { \
      return ERR_RT_INVALID_ARGUMENT; \
    } \
    \
    iW = _ImageFragment_->w; \
    iH = _ImageFragment_->h; \
    if (iW == 0 || iH == 0) return ERR_OK; \
  }

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitImageAtI(Painter& self, const PointI& p, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_GET_IMAGE_PARAMS(i, iFragment)

  int dstX = p.x, dstW;
  int dstY = p.y, dstH;
  int d;

  switch (engine->finalTransformI._type)
  {
    case RASTER_INTEGRAL_TRANSFORM_NULL:
    {
      if (engine->ctx.paintHints.geometricPrecision)
        return engine->vtable->blitImageAtD(self, PointD(p), i, iFragment);
      else
        return engine->vtable->blitImageAtF(self, PointF(p), i, iFragment);
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

      return doBlitNormalizedImageI(engine, PointI(dstX, dstY), i, RectI(iX, iY, iW, iH));
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

      return doBlitNormalizedTransformedImageI(engine, BoxI(dstX, dstY, dstW, dstH), i, RectI(iX, iY, iW, iH), tr);
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

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitImageAtF(Painter& self, const PointF& p, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_GET_IMAGE_PARAMS(i, iFragment)

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

        return doBlitNormalizedImageI(engine, PointI(dstX, dstY), i, RectI(iX, iY, iW, iH));
      }
      else
      {
        box.translate(engine->finalTransform._20, engine->finalTransform._21);
        if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
          return ERR_OK;

        TransformD tr(engine->finalTransform);
        tr.translate(PointD(p.x, p.y));

        return doBlitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
      }
    }

    case TRANSFORM_TYPE_SCALING:
    {
      engine->finalTransform.mapBox(box, box);

      if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
        return ERR_OK;

      TransformD tr(engine->finalTransform);
      tr.translate(PointD(p.x, p.y));

      return doBlitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
    }

    default:
    {
      TransformD tr(engine->finalTransform);
      tr.translate(PointD(p.x, p.y));

      return doBlitUntransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
    }
  }
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitImageAtD(Painter& self, const PointD& p, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_GET_IMAGE_PARAMS(i, iFragment)

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

        return doBlitNormalizedImageI(engine, PointI(dstX, dstY), i, RectI(iX, iY, iW, iH));
      }
      else
      {
        box.translate(engine->finalTransform._20, engine->finalTransform._21);
        if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
          return ERR_OK;

        TransformD tr(engine->finalTransform);
        tr.translate(PointD(p.x, p.y));

        return doBlitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
      }
    }

    case TRANSFORM_TYPE_SCALING:
    {
      engine->finalTransform.mapBox(box, box);

      if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
        return ERR_OK;

      TransformD tr(engine->finalTransform);
      tr.translate(PointD(p.x, p.y));

      return doBlitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
    }

    default:
    {
      TransformD tr(engine->finalTransform);
      tr.translate(PointD(p.x, p.y));

      return doBlitUntransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
    }
  }
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitImageInI(Painter& self, const RectI& r, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_GET_IMAGE_PARAMS(i, iFragment)

  // Try to use unscaled blit if possible.
  if (r.w == iW && r.h == iH) return engine->vtable->blitImageAtI(self, PointI(r.x, r.y), i, iFragment);

  uint32_t transformType = engine->finalTransform.getType();
  BoxD box(double(r.x), double(r.y), double(r.x) + double(r.w), double(r.y) + double(r.h));

  TransformD tr(engine->finalTransform);
  double sx = double(r.w) / double(iW);
  double sy = double(r.h) / double(iH);

  TransformD scaling(sx, 0.0, 0.0, sy, r.x, r.y);
  tr.transform(scaling);
  //tr.translate(PointD(r.x, r.y));
  //tr.scale(PointD(sx, sy));

  if (transformType <= TRANSFORM_TYPE_SCALING)
  {
    engine->finalTransform.mapBox(box, box);
    if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
      return ERR_OK;

    return doBlitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
  }
  else
  {
    return doBlitUntransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
  }

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitImageInF(Painter& self, const RectF& r, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_GET_IMAGE_PARAMS(i, iFragment)

  uint32_t transformType = engine->finalTransform.getType();
  BoxD box(double(r.x), double(r.y), double(r.x) + double(r.w), double(r.y) + double(r.h));

  TransformD tr(double(r.w) / double(iW), 0.0, 0.0, double(r.h) / double(iH), r.x, r.y);
  tr.transform(engine->finalTransform, MATRIX_ORDER_APPEND);

  if (transformType <= TRANSFORM_TYPE_SCALING)
  {
    engine->finalTransform.mapBox(box, box);
    if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
      return ERR_OK;

    return doBlitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
  }
  else
  {
    return doBlitUntransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
  }
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitImageInD(Painter& self, const RectD& r, const Image& i, const RectI* iFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);
  if (i.isEmpty()) return ERR_OK;

  _FOG_RASTER_ENTER_BLIT_FUNC();
  _FOG_RASTER_GET_IMAGE_PARAMS(i, iFragment)

  uint32_t transformType = engine->finalTransform.getType();
  BoxD box(double(r.x), double(r.y), double(r.x) + double(r.w), double(r.y) + double(r.h));

  TransformD tr(double(r.w) / double(iW), 0.0, 0.0, double(r.h) / double(iH), r.x, r.y);
  tr.transform(engine->finalTransform, MATRIX_ORDER_APPEND);

  if (transformType <= TRANSFORM_TYPE_SCALING)
  {
    engine->finalTransform.mapBox(box, box);
    if (!BoxD::intersect(box, box, engine->ctx.finalClipperD.getClipBox()))
      return ERR_OK;

    return doBlitNormalizedTransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
  }
  else
  {
    return doBlitUntransformedImageD(engine, box, i, RectI(iX, iY, iW, iH), tr);
  }
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitMaskedImageAtI(Painter& self, const PointI& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitMaskedImageAtF(Painter& self, const PointF& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitMaskedImageAtD(Painter& self, const PointD& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitMaskedImageInI(Painter& self, const RectI& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitMaskedImageInF(Painter& self, const RectF& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::blitMaskedImageInD(Painter& self, const RectD& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPainterImpl<> - Clip]
// ============================================================================

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipRectI(Painter& self, uint32_t clipOp, const RectI& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipRectF(Painter& self, uint32_t clipOp, const RectF& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipRectD(Painter& self, uint32_t clipOp, const RectD& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipRectsI(Painter& self, uint32_t clipOp, const RectI* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipRectsF(Painter& self, uint32_t clipOp, const RectF* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipRectsD(Painter& self, uint32_t clipOp, const RectD* r, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipPolygonI(Painter& self, uint32_t clipOp, const PointI* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipPolygonF(Painter& self, uint32_t clipOp, const PointF* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipPolygonD(Painter& self, uint32_t clipOp, const PointD* p, size_t count)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipShapeF(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipShapeD(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipPathF(Painter& self, uint32_t clipOp, const PathF& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipPathD(Painter& self, uint32_t clipOp, const PathD& p)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipTextAtI(Painter& self, uint32_t clipOp, const PointI& p, const String& text, const Font& font, const RectI* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipTextAtF(Painter& self, uint32_t clipOp, const PointF& p, const String& text, const Font& font, const RectF* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipTextAtD(Painter& self, uint32_t clipOp, const PointD& p, const String& text, const Font& font, const RectD* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipTextInI(Painter& self, uint32_t clipOp, const TextRectI& r, const String& text, const Font& font, const RectI* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipTextInF(Painter& self, uint32_t clipOp, const TextRectF& r, const String& text, const Font& font, const RectF* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipTextInD(Painter& self, uint32_t clipOp, const TextRectD& r, const String& text, const Font& font, const RectD* clip)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipMaskAtI(Painter& self, uint32_t clipOp, const PointI& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipMaskAtF(Painter& self, uint32_t clipOp, const PointF& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipMaskAtD(Painter& self, uint32_t clipOp, const PointD& p, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipMaskInI(Painter& self, uint32_t clipOp, const RectI& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipMaskInF(Painter& self, uint32_t clipOp, const RectF& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipMaskInD(Painter& self, uint32_t clipOp, const RectD& r, const Image& m, const RectI* mFragment)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::clipRegion(Painter& self, uint32_t clipOp, const Region& r)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::resetClip(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPainterImpl<> - Layer]
// ============================================================================

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::beginLayer(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::endLayer(Painter& self)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPainterImpl<> - Flush]
// ============================================================================

template<int _MODE>
err_t FOG_CDECL RasterPainterImpl<_MODE>::flush(Painter& self, uint32_t flags)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self._engine);

  // TODO: MT version.
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPainterImpl<> - FillNormalizedBox]
// ============================================================================

template<int _MODE>
FOG_INLINE err_t RasterPainterImpl<_MODE>::doFillNormalizedBoxI(RasterPaintEngine* engine, const BoxI& box)
{
  _FOG_RASTER_ENSURE_PATTERN(engine);

  if (_MODE == RASTER_MODE_ST)
  {
    engine->ctx.renderer->fillNormalizedBoxI(engine->ctx, box);
    return ERR_OK;
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

template<int _MODE>
FOG_INLINE err_t RasterPainterImpl<_MODE>::doFillNormalizedBoxF(RasterPaintEngine* engine, const BoxF& box)
{
  _FOG_RASTER_ENSURE_PATTERN(engine);

  if (_MODE == RASTER_MODE_ST)
  {
    engine->ctx.renderer->fillNormalizedBoxF(engine->ctx, box);
    return ERR_OK;
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

template<int _MODE>
FOG_INLINE err_t RasterPainterImpl<_MODE>::doFillNormalizedBoxD(RasterPaintEngine* engine, const BoxD& box)
{
  _FOG_RASTER_ENSURE_PATTERN(engine);

  if (_MODE == RASTER_MODE_ST)
  {
    engine->ctx.renderer->fillNormalizedBoxD(engine->ctx, box);
    return ERR_OK;
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

// ============================================================================
// [Fog::RasterPainterImpl<> - FillNormalizedPath]
// ============================================================================

template<int _MODE>
err_t RasterPainterImpl<_MODE>::doFillNormalizedPathF(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule)
{
  _FOG_RASTER_ENSURE_PATTERN(engine);

  if (_MODE == RASTER_MODE_ST)
  {
    engine->ctx.renderer->fillNormalizedPathF(engine->ctx, path, fillRule);
    return ERR_OK;
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

template<int _MODE>
err_t RasterPainterImpl<_MODE>::doFillNormalizedPathD(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule)
{
  _FOG_RASTER_ENSURE_PATTERN(engine);

  if (_MODE == RASTER_MODE_ST)
  {
    engine->ctx.renderer->fillNormalizedPathD(engine->ctx, path, fillRule);
    return ERR_OK;
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

// ============================================================================
// [Fog::RasterPainterImpl<> - FillUntransformedPath]
// ============================================================================

template<int _MODE>
err_t RasterPainterImpl<_MODE>::doFillUntransformedPathF(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule, bool clip)
{
  _FOG_RASTER_ENSURE_PATTERN(engine);

  if (_MODE == RASTER_MODE_ST)
  {
    bool hasTransform = ensureFinalTransformF(engine);

    if (clip)
    {
      engine->ctx.tmpPathF[1].clear();

      if (!hasTransform)
      {
        switch (engine->ctx.finalClipperF.initPath(path))
        {
          case PATH_CLIPPER_STATUS_CLIPPED:
            engine->ctx.renderer->fillNormalizedPathF(engine->ctx, path, fillRule);
            return ERR_OK;

          case PATH_CLIPPER_STATUS_MUST_CLIP:
            FOG_RETURN_ON_ERROR(engine->ctx.finalClipperF.continuePath(engine->ctx.tmpPathF[1], path));
            engine->ctx.renderer->fillNormalizedPathF(engine->ctx, engine->ctx.tmpPathF[1], fillRule);
            return ERR_OK;

          default:
            return ERR_GEOMETRY_INVALID;
        }
      }
      else
      {
        FOG_RETURN_ON_ERROR(engine->ctx.finalClipperF.clipPath(engine->ctx.tmpPathF[1], path, engine->finalTransformF));
        engine->ctx.renderer->fillNormalizedPathF(engine->ctx, engine->ctx.tmpPathF[1], fillRule);
        return ERR_OK;
      }
    }
    else
    {
      if (!hasTransform)
      {
        engine->ctx.renderer->fillNormalizedPathF(engine->ctx, path, fillRule);
        return ERR_OK;
      }
      else
      {
        FOG_RETURN_ON_ERROR(engine->finalTransformF.mapPath(engine->ctx.tmpPathF[1], path));
        engine->ctx.renderer->fillNormalizedPathF(engine->ctx, engine->ctx.tmpPathF[1], fillRule);
        return ERR_OK;
      }
    }
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

template<int _MODE>
err_t RasterPainterImpl<_MODE>::doFillUntransformedPathD(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule, bool clip)
{
  _FOG_RASTER_ENSURE_PATTERN(engine);

  if (_MODE == RASTER_MODE_ST)
  {
    bool hasTransform = (engine->finalTransform.getType() != TRANSFORM_TYPE_IDENTITY);

    if (clip)
    {
      engine->ctx.tmpPathD[1].clear();

      if (!hasTransform)
      {
        switch (engine->ctx.finalClipperD.initPath(path))
        {
          case PATH_CLIPPER_STATUS_CLIPPED:
            engine->ctx.renderer->fillNormalizedPathD(engine->ctx, path, fillRule);
            return ERR_OK;

          case PATH_CLIPPER_STATUS_MUST_CLIP:
            FOG_RETURN_ON_ERROR(engine->ctx.finalClipperD.continuePath(engine->ctx.tmpPathD[1], path));
            engine->ctx.renderer->fillNormalizedPathD(engine->ctx, engine->ctx.tmpPathD[1], fillRule);
            return ERR_OK;

          default:
            return ERR_GEOMETRY_INVALID;
        }
      }
      else
      {
        FOG_RETURN_ON_ERROR(engine->ctx.finalClipperD.clipPath(engine->ctx.tmpPathD[1], path, engine->finalTransform));
        engine->ctx.renderer->fillNormalizedPathD(engine->ctx, engine->ctx.tmpPathD[1], fillRule);
        return ERR_OK;
      }
    }
    else
    {
      if (!hasTransform)
      {
        engine->ctx.renderer->fillNormalizedPathD(engine->ctx, path, fillRule);
        return ERR_OK;
      }
      else
      {
        FOG_RETURN_ON_ERROR(engine->finalTransform.mapPath(engine->ctx.tmpPathD[1], path));
        engine->ctx.renderer->fillNormalizedPathD(engine->ctx, engine->ctx.tmpPathD[1], fillRule);
        return ERR_OK;
      }
    }
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

// ============================================================================
// [Fog::RasterPainterImpl<> - Blit]
// ============================================================================

template<int _MODE>
FOG_INLINE err_t RasterPainterImpl<_MODE>::doBlitNormalizedImageI(
  RasterPaintEngine* engine,
  const PointI& pt, const Image& srcImage, const RectI& srcFragment)
{
  // Must be already clipped.
  FOG_ASSERT(engine->ctx.finalClipBoxI.subsumes(BoxI(pt.x, pt.y, pt.x + srcFragment.w, pt.y + srcFragment.h)));

  if (_MODE == RASTER_MODE_ST)
  {
    engine->ctx.renderer->blitNormalizedImageI(engine->ctx, pt, srcImage, srcFragment);
    return ERR_OK;
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

template<int _MODE>
err_t RasterPainterImpl<_MODE>::doBlitNormalizedTransformedImageI(RasterPaintEngine* engine,
  const BoxI& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform)
{
  // Must be already clipped.
  FOG_ASSERT(engine->ctx.finalClipBoxI.subsumes(box));

  if (_MODE == RASTER_MODE_ST)
  {
    err_t err;

    RenderPatternContext* pcOld = engine->ctx.pc;
    RenderPatternContext pc;

    FOG_RETURN_ON_ERROR(
      _g2d_render.texture.create(&pc,
        engine->ctx.layer.primaryFormat,
        engine->coreClipBox,
        srcImage, srcFragment,
        srcTransform, Color(), TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
    );

    engine->ctx.pc = &pc;
    err = doFillNormalizedBoxI(engine, box);
    engine->ctx.pc = pcOld;

    pc.destroy();
    return err;
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

template<int _MODE>
err_t RasterPainterImpl<_MODE>::doBlitNormalizedTransformedImageD(RasterPaintEngine* engine,
  const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform)
{
  // Must be already clipped.
  FOG_ASSERT(engine->ctx.finalClipperD.getClipBox().subsumes(box));

  if (_MODE == RASTER_MODE_ST)
  {
    err_t err;

    RenderPatternContext* pcOld = engine->ctx.pc;
    RenderPatternContext pc;

    FOG_RETURN_ON_ERROR(
      _g2d_render.texture.create(&pc,
        engine->ctx.layer.primaryFormat,
        engine->coreClipBox,
        srcImage, srcFragment,
        srcTransform, Color(), TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
    );

    engine->ctx.pc = &pc;
    err = doFillNormalizedBoxD(engine, box);
    engine->ctx.pc = pcOld;

    pc.destroy();
    return err;
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

template<int _MODE>
err_t RasterPainterImpl<_MODE>::doBlitUntransformedImageD(
  RasterPaintEngine* engine, const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform)
{
  BoxD boxClipped(box);
  engine->finalTransform.mapBox(boxClipped, boxClipped);

  if (!BoxD::intersect(boxClipped, boxClipped, engine->ctx.finalClipperD.getClipBox()))
    return ERR_OK;

  if (_MODE == RASTER_MODE_ST)
  {
    err_t err;

    RenderPatternContext* pcOld = engine->ctx.pc;
    RenderPatternContext pc;

    if (srcTransform.getType() <= TRANSFORM_TYPE_SCALING)
    {
      FOG_RETURN_ON_ERROR(
        _g2d_render.texture.create(&pc,
          engine->ctx.layer.primaryFormat,
          engine->coreClipBox,
          srcImage, srcFragment,
          srcTransform, Color(), TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
      );

      engine->ctx.pc = &pc;
      err = doFillNormalizedBoxD(engine, boxClipped);
    }
    else
    {
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
      err = doFillUntransformedPathD(engine, path, FILL_RULE_NON_ZERO, true);
    }

    engine->ctx.pc = pcOld;
    pc.destroy();
    return err;
  }
  else
  {
    // TODO:
    return ERR_RT_NOT_IMPLEMENTED;
  }
}

// ============================================================================
// [Fog::RasterPainterVTable]
// ============================================================================

RasterPainterVTable RasterPaintEngine_vtable[RASTER_MODE_COUNT];

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

  maxThreads = RasterPainterImpl_::getMaxThreads();
  finalizing = 0;
}

RasterPaintEngine::~RasterPaintEngine()
{
  if (ctx.layer.imageData)
    ctx.layer.imageData->locked--;

  RasterPainterImpl_::discardStates(this);
  RasterPainterImpl_::discardSource(this);

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

  RasterPainterImpl_::setupLayer(this);
  FOG_RETURN_ON_ERROR(ctx._initPrecision(ctx.layer.precision));

  // Setup defaults.
  vtable = &RasterPaintEngine_vtable[RASTER_MODE_ST];

  RasterPainterImpl_::setupOps(this);
  RasterPainterImpl_::setupDefaultClip(this);
  RasterPainterImpl_::setupDefaultRenderer(this);

  return ERR_OK;
}

err_t RasterPaintEngine::switchTo(const ImageBits& imageBits, ImageData* imaged)
{
  // TODO.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Init / Fini]
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

static err_t FOG_CDECL RasterPaintEngine_beginImage(Painter& self, Image& image, const RectI* rect, uint32_t initFlags)
{
  err_t err;

  ImageBits imageBits(UNINITIALIZED);
  RasterPaintEngine* engine;

  // Release the painter engine.
  if (self._engine) self._vtable->release(self);

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

  self._engine = engine;
  self._vtable = engine->vtable;
  return ERR_OK;

_Fail:
  self._engine = _api.painter.getNullEngine();
  self._vtable = self._engine->vtable;
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_beginIBits(Painter& self, const ImageBits& _imageBits, const RectI* rect, uint32_t initFlags)
{
  err_t err;

  ImageBits imageBits(UNINITIALIZED);
  RasterPaintEngine* engine;

  // Release the painter engine.
  if (self._engine) self._vtable->release(self);

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

  self._engine = engine;
  self._vtable = engine->vtable;
  return ERR_OK;

_Fail:
  self._engine = _api.painter.getNullEngine();
  self._vtable = self._engine->vtable;
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_switchToImage(Painter& self, Image& image, const RectI* rect)
{
  err_t err;
  ImageBits imageBits(UNINITIALIZED);

  uint32_t deviceId;

  // Prepare.
  if ((err = RasterPaintEngine_prepareToImage(imageBits, image, rect)) != ERR_OK) goto _Fail;
  if ((err = self.getDeviceId(deviceId)) != ERR_OK) goto _Fail;

  if (deviceId == PAINT_DEVICE_RASTER)
  {
    return reinterpret_cast<RasterPaintEngine*>(self._engine)->switchTo(imageBits, image._d);
  }
  else
  {
    return _api.painter.beginImage(self, image, rect, NO_FLAGS);
  }

_Fail:
  self.end();
  return err;
}

static err_t FOG_CDECL RasterPaintEngine_switchToIBits(Painter& self, const ImageBits& _imageBits, const RectI* rect)
{
  err_t err;
  ImageBits imageBits(UNINITIALIZED);

  uint32_t deviceId;

  // Prepare.
  if ((err = RasterPaintEngine_prepareToImage(imageBits, _imageBits, rect)) != ERR_OK) goto _Fail;
  if ((err = self.getDeviceId(deviceId)) != ERR_OK) goto _Fail;

  if (deviceId == PAINT_DEVICE_RASTER)
    return reinterpret_cast<RasterPaintEngine*>(self._engine)->switchTo(imageBits, NULL);
  else
    return _api.painter.beginIBits(self, _imageBits, rect, NO_FLAGS);

_Fail:
  self.end();
  return err;
}

FOG_NO_EXPORT void Painter_initRaster(void)
{
  _api.painter.beginImage = RasterPaintEngine_beginImage;
  _api.painter.beginIBits = RasterPaintEngine_beginIBits;

  _api.painter.switchToImage = RasterPaintEngine_switchToImage;
  _api.painter.switchToIBits = RasterPaintEngine_switchToIBits;

  RasterPainterImpl<RASTER_MODE_ST>::initVTable(RasterPaintEngine_vtable[RASTER_MODE_ST]);
  RasterPainterImpl<RASTER_MODE_MT>::initVTable(RasterPaintEngine_vtable[RASTER_MODE_MT]);

  Painter_initRasterRender();
}

} // Fog namespace
