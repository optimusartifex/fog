// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTENGINE_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTENGINE_P_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Memory/MemBlockAllocator.h>
#include <Fog/Core/Memory/MemZoneAllocator.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadCondition.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Painting/PaintEngine.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintCmd_p.h>
#include <Fog/G2d/Painting/RasterPaintContext_p.h>
#include <Fog/G2d/Painting/RasterPaintSerializer_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterState_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterPaintEngine]
// ============================================================================

//! @internal
//!
//! @brief Raster paint-engine.
struct FOG_NO_EXPORT RasterPaintEngine : public PaintEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterPaintEngine();
  ~RasterPaintEngine();

  // --------------------------------------------------------------------------
  // [Initialization]
  // --------------------------------------------------------------------------

  err_t init(const ImageBits& imageBits, ImageData* imaged, uint32_t initFlags);
  err_t switchTo(const ImageBits& imageBits, ImageData* imaged);

  // --------------------------------------------------------------------------
  // [Detect Max Threads]
  // --------------------------------------------------------------------------

  static uint detectMaxThreads();

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxI& getClipBoxI() const { return ctx.clipBoxI; }
  FOG_INLINE const BoxF& getClipBoxF() const { return stroker.f->_clipBox; }
  FOG_INLINE const BoxD& getClipBoxD() const { return stroker.d->_clipBox; }

  FOG_INLINE const BoxI& getMetaClipBoxI() const { return metaClipBoxI; }
  FOG_INLINE const BoxF& getMetaClipBoxF() const { return metaClipBoxF; }
  FOG_INLINE const BoxD& getMetaClipBoxD() const { return metaClipBoxD; }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  //! @brief The final transformation matrix (float).
  FOG_INLINE const TransformF& getFinalTransformF() const { return stroker.f->_transform; }
  //! @brief The final transformation matrix (double).
  FOG_INLINE const TransformD& getFinalTransformD() const { return stroker.d->_transform; }

  FOG_INLINE bool ensureFinalTransformF()
  {
    if (getFinalTransformD()._getType() == TRANSFORM_TYPE_IDENTITY)
      return false;

    if (ctx.rasterHints.finalTransformF)
      return true;

    stroker.f->_transform->setTransform(getFinalTransformD());
    ctx.rasterHints.finalTransformF = 1;

    return true;
  }

  FOG_INLINE bool isIntegralTransform() const
  {
    return integralTransformType != RASTER_INTEGRAL_TRANSFORM_NONE;
  }

  bool doIntegralTransformAndClip(BoxI& dst, const RectI& src, const BoxI& clipBox);

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterState* createState()
  {
    RasterState* s = statePool;
    if (FOG_IS_NULL(s))
      s = reinterpret_cast<RasterState*>(stateAllocator.alloc(sizeof(RasterState)));
    else
      statePool = s->prevState;
    return s;
  }

  FOG_INLINE void poolState(RasterState* state)
  {
    state->prevState = statePool;
    statePool = state;
  }

  void saveSourceAndDiscard();

  FOG_INLINE void saveSourceArgb32()
  {
    if ((savedStateFlags & RASTER_STATE_SOURCE) == 0)
    {
      FOG_ASSERT(state != NULL);
      FOG_ASSERT(sourceType == RASTER_SOURCE_ARGB32);

      savedStateFlags |= RASTER_STATE_SOURCE;
      state->sourceType = RASTER_SOURCE_ARGB32;
      state->source.color->_argb32.u32 = source.color->_argb32.u32;
      state->solid = ctx.solid;
    }
  }

  FOG_INLINE void saveSourceColor()
  {
    if ((savedStateFlags & RASTER_STATE_SOURCE) == 0)
    {
      FOG_ASSERT(state != NULL);
      FOG_ASSERT(sourceType == RASTER_SOURCE_COLOR);

      savedStateFlags |= RASTER_STATE_SOURCE;
      state->sourceType = RASTER_SOURCE_COLOR;
      MemOps::copy_t<Color>(&state->source.color, &source.color);
      state->solid = ctx.solid;
    }
  }

  void saveStroke();
  void saveTransform();
  void saveClipping();
  void saveFilter();

  void discardStates();

  // --------------------------------------------------------------------------
  // [Setup]
  // --------------------------------------------------------------------------

  void setupLayer();
  void setupOps();
  void setupDefaultClip();

  // --------------------------------------------------------------------------
  // [Helpers - Source]
  // --------------------------------------------------------------------------

  err_t createPatternContext();

  FOG_INLINE void discardSource()
  {
    switch (sourceType)
    {
      case RASTER_SOURCE_NONE:
      case RASTER_SOURCE_ARGB32:
      case RASTER_SOURCE_COLOR:
        break;

      case RASTER_SOURCE_TEXTURE:
        source.texture.destroy();
        goto _DiscardContinue;

      case RASTER_SOURCE_GRADIENT:
        source.gradient.destroy();
_DiscardContinue:
        if (source.transform->_type != TRANSFORM_TYPE_IDENTITY)
          source.transform->reset();
        if (RasterUtil::isPatternContext(ctx.pc) && ctx.pc->_reference.deref())
          destroyPatternContext(ctx.pc);
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }
  
  FOG_INLINE void destroyPatternContext(RasterPattern* pc)
  {
    pc->destroy();

    reinterpret_cast<RasterAbstractLinkedList*>(pc)->next = pcPool;
    pcPool = reinterpret_cast<RasterAbstractLinkedList*>(pc);
  }

  // --------------------------------------------------------------------------
  // [Helpers - Region]
  // --------------------------------------------------------------------------

  FOG_INLINE Region* getTemporaryRegion()
  {
    size_t i;
    for (i = 0; i < FOG_ARRAY_SIZE(tmpRegion) - 1; i++)
    {
      if (tmpRegion[i].isDetached())
        break;
    }
    return &tmpRegion[i];
  }

  // --------------------------------------------------------------------------
  // [Helpers - Groups]
  // --------------------------------------------------------------------------

  // ...

  // --------------------------------------------------------------------------
  // [Helpers - Commands]
  // --------------------------------------------------------------------------

  template<typename CmdT>
  FOG_INLINE CmdT* newCmd()
  {
    if (!cmdAllocator.canAlloc(sizeof(CmdT) + sizeof(RasterPaintCmd_Next)))
    {
      RasterPaintCmd_Next* nc = static_cast<RasterPaintCmd_Next*>(
        cmdAllocator.allocNoCheck(sizeof(RasterPaintCmd_Next)));

      CmdT* cmd = static_cast<CmdT*>(cmdAllocator.alloc(sizeof(CmdT)));
      if (FOG_IS_NULL(cmd))
        return NULL;

      nc->init(this, RASTER_PAINT_CMD_NEXT, reinterpret_cast<uint8_t*>(cmd));
      return cmd;
    }
    else
    {
      return static_cast<CmdT*>(cmdAllocator.allocNoCheck(sizeof(CmdT)));
    }
  }

  // --------------------------------------------------------------------------
  // [Changed]
  // --------------------------------------------------------------------------

  //! @brief Called when meta-region/meta-origin were changed.
  void changedMetaParams();
  //! @brief Called when user-transform was changed.
  void changedUserTransform();

  // --------------------------------------------------------------------------
  // [Members - Serializer]
  // --------------------------------------------------------------------------

  //! @brief Serializer (st/mt).
  const RasterPaintSerializer* serializer;

  // --------------------------------------------------------------------------
  // [Members - Context]
  // --------------------------------------------------------------------------

  //! @brief Context (st).
  RasterPaintContext ctx;

  // --------------------------------------------------------------------------
  // [Members - Flags]
  // --------------------------------------------------------------------------

  //! @brief 'NoPaint', 'Group' and 'Error' flags.
  uint32_t masterFlags;

  // --------------------------------------------------------------------------
  // [Members - Layer]
  // --------------------------------------------------------------------------

  //! @brief The master layer id (starting at zero).
  uint32_t masterLayerId;

  // --------------------------------------------------------------------------
  // [Members - Mask]
  // --------------------------------------------------------------------------

  //! @brief Current (or last) clip mask id. Each new mask get this id increased
  //! by one.
  //!
  //! If clipMaskId is zero then there is no active mask.
  //!
  //! @note Clip mask id is increased / decreased mainly by state management,
  //! because if state (and mask in it) is saved then new mask is needed. This
  //! new mask can be based on the old mask, but it's mutable. IDs are managed
  //! only by master context so if multithreading is used then some threads
  //! can use same mask id on different mask instance.
  uint32_t masterMaskId;

  //! @brief Contains how many the state was saved for current mask, but mask
  //! is not changed after that.
  uint32_t masterMaskSaved;

  // --------------------------------------------------------------------------
  // [Members - Base (Always Saved / Restored)]
  // --------------------------------------------------------------------------

  //! @brief Source type (see @c RASTER_SOURCE).
  uint8_t sourceType;

  //! @brief 'Saved-State' flags.
  //!
  //! Which states was saved to the last @c RasterState instance. If there is
  //! no such instance (the initial state) all bits are set to logical ones.
  uint8_t savedStateFlags;

  //! @brief The strokeParams[F|D] precision.
  uint8_t strokerPrecision;

  // --------------------------------------------------------------------------
  // [Members - Integral-Transform]
  // --------------------------------------------------------------------------

  //! @brief The integralTransform type, see @c RASTER_INTEGRAL_TRANSFORM.
  uint8_t integralTransformType;

  //! @brief The final transform/scaling in integral units.
  struct FOG_NO_EXPORT _IntegralTransform
  {
    int _sx, _sy;
    int _tx, _ty;
  } integralTransform;

  // --------------------------------------------------------------------------
  // [Members - Meta-Params]
  // --------------------------------------------------------------------------

  //! @brief Meta origin.
  PointI metaOrigin;
  //! @brief Meta clip-region.
  Region metaRegion;

  //! @brief Meta clip-box (int).
  BoxI metaClipBoxI;
  //! @brief Meta clip-box (float).
  BoxF metaClipBoxF;
  //! @brief Meta clip-box (double).
  BoxD metaClipBoxD;

  // --------------------------------------------------------------------------
  // [Members - Transform]
  // --------------------------------------------------------------------------

  //! @brief The meta transformation matrix.
  TransformD metaTransformD;
  //! @brief The user transformation matrix.
  TransformD userTransformD;

  // --------------------------------------------------------------------------
  // [Members - Source & Opacity]
  // --------------------------------------------------------------------------

  //! @brief Source.
  RasterPaintSource source;
  //! @brief opacity (in floating-point format).
  float opacityF;

  // --------------------------------------------------------------------------
  // [Members - Stroke Params]
  // --------------------------------------------------------------------------

  //! @brief Stroke parameters.
  struct FOG_NO_EXPORT _Stroker
  {
    //! @brief Stroke parameters (float).
    Static<PathStrokerF> f;
    //! @brief Stroke parameters (double).
    Static<PathStrokerD> d;
  } stroker;

  // --------------------------------------------------------------------------
  // [Members - State]
  // --------------------------------------------------------------------------

  //! @brief Zone memory allocator for @c RasterState objects.
  MemZoneAllocator stateAllocator;
  //! @brief Unused states pool.
  RasterState* statePool;
  //! @brief State.
  RasterState* state;

  // --------------------------------------------------------------------------
  // [Members - Pattern Context]
  // --------------------------------------------------------------------------

  //! @brief Pattern-context allocator.
  MemZoneAllocator pcAllocator;
  //! @brief Pattern-context pool.
  RasterAbstractLinkedList* pcPool;

  // --------------------------------------------------------------------------
  // [Members - Dummy]
  // --------------------------------------------------------------------------

  Color dummyColor;

  // --------------------------------------------------------------------------
  // [Members - Groups]
  // --------------------------------------------------------------------------

  //! @brief Group object (RasterPaintGroup) allocator.
  MemZoneAllocator groupAllocator;
  //! @brief Top-most group (statically allocated).
  RasterPaintGroup topGroup;
  //! @brief Current group.
  RasterPaintGroup* curGroup;

  // --------------------------------------------------------------------------
  // [Members - Commands]
  // --------------------------------------------------------------------------

  //! @brief Command (RasterPaintCmd) allocator.
  MemZoneAllocator cmdAllocator;

  // --------------------------------------------------------------------------
  // [Members - Multithreading]
  // --------------------------------------------------------------------------

  // @brief The worker manager.
  // RasterPaintWorkMgr* wm;

  //! @brief The maximum number of threads that can be used for rendering after
  //! the multithreading is initialized.
  //!
  //! This value is initialized to number of processors (strictly speaking to
  //! number of native threads available for the host machine).
  uint maxThreads;

  //! @brief Whether the painter engine is finalizing.
  //!
  //! If finalizing is non-zero then the painter is finalizing and methods
  //! related to changing multithreaded mode into singlethreaded can't fail.
  uint finalizing;

  // --------------------------------------------------------------------------
  // [Members - Temporary]
  // --------------------------------------------------------------------------

  // Temporary regions.
  Region tmpRegion[4];
};

// ============================================================================
// [Fog::RasterPaintEngine - Implemented-Later]
// ============================================================================

FOG_INLINE void RasterPaintCmd_SetOpacityAndPattern::destroy(RasterPaintEngine* engine)
{
  if (_pc->_reference.deref())
    engine->destroyPatternContext(_pc);
  Base::destroy(engine);
}

// ============================================================================
// [Fog::RasterPaintEngine - VTable]
// ============================================================================

extern FOG_NO_EXPORT PaintEngineVTable RasterPaintEngine_vtable[IMAGE_PRECISION_COUNT];

// ============================================================================
// [Fog::RasterPaintEngine - Defs]
// ============================================================================

// Called by all 'fill' functions. This macro simply ensures that all fill
// parameters are correct (it's possible to fill something).
//
// Please see RASTER_NO_PAINT flags to better understand how this works.
#define _FOG_RASTER_ENTER_FILL_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY((engine->masterFlags & (RASTER_NO_PAINT_BASE_FLAGS     | \
                                             RASTER_NO_PAINT_SOURCE         | \
                                             RASTER_NO_PAINT_FATAL          )) != 0)) \
    { \
      return ERR_OK; \
    } \
  FOG_MACRO_END

// Called by all 'stroke' functions. This macro simply ensures that all stroke
// parameters are correct (it's possible to stroke something). Note that stroke
// is implemented normally using fill pipeline, there are only few exceptions.
//
// Please see RASTER_NO_PAINT flags to better understand how this works.
#define _FOG_RASTER_ENTER_STROKE_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY((engine->masterFlags & (RASTER_NO_PAINT_BASE_FLAGS     | \
                                             RASTER_NO_PAINT_SOURCE         | \
                                             RASTER_NO_PAINT_STROKE         | \
                                             RASTER_NO_PAINT_FATAL          )) != 0)) \
    { \
      return ERR_OK; \
    } \
  FOG_MACRO_END

#define _FOG_RASTER_ENTER_BLIT_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY((engine->masterFlags & (RASTER_NO_PAINT_BASE_FLAGS     | \
                                             RASTER_NO_PAINT_FATAL          )) != 0)) \
    { \
      return ERR_OK; \
    } \
  FOG_MACRO_END

#define _FOG_RASTER_ENTER_CLIP_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY(clipOp >= CLIP_OP_COUNT)) \
    { \
      return ERR_RT_INVALID_ARGUMENT; \
    } \
    \
    if (FOG_UNLIKELY((engine->masterFlags & RasterPaintEngine_clipAllFlags[clipOp]) != 0)) \
    { \
      return engine->serializer->clipAll(engine); \
    } \
  FOG_MACRO_END

#define _FOG_RASTER_ENTER_FILTER_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY((engine->masterFlags & (RASTER_NO_PAINT_BASE_FLAGS     | \
                                             RASTER_NO_PAINT_FATAL          )) != 0)) \
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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTENGINE_P_H
