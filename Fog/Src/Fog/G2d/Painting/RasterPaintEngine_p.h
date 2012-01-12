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
#include <Fog/G2d/Painting/RasterPaintWork_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterState_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterSerializer]
// ============================================================================

//! @internal
//!
//! @brief Raster paint-engine serializer.
//!
//! This class contains function pointers to low-level painter operations. When
//! single-threaded mode is used, the serializer contains function pointers to
//! render functions, otherwise serialize functions are used.
struct FOG_NO_EXPORT RasterSerializer
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_FASTCALL *FillAll)(RasterPaintEngine* engine);
  typedef err_t (FOG_FASTCALL *FillPathF)(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *FillPathD)(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *StrokeAndFillPathF)(RasterPaintEngine* engine, const PathF& path);
  typedef err_t (FOG_FASTCALL *StrokeAndFillPathD)(RasterPaintEngine* engine, const PathD& path);

  typedef err_t (FOG_FASTCALL *FillNormalizedBoxI)(RasterPaintEngine* engine, const BoxI& box);
  typedef err_t (FOG_FASTCALL *FillNormalizedBoxF)(RasterPaintEngine* engine, const BoxF& box);
  typedef err_t (FOG_FASTCALL *FillNormalizedBoxD)(RasterPaintEngine* engine, const BoxD& box);
  typedef err_t (FOG_FASTCALL *FillNormalizedPathF)(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *FillNormalizedPathD)(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule);

  typedef err_t (FOG_FASTCALL *BlitImageD)(RasterPaintEngine* engine, const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform);
  typedef err_t (FOG_FASTCALL *BlitNormalizedImageA)(RasterPaintEngine* engine, const PointI& pt, const Image& srcImage, const RectI& srcFragment);
  typedef err_t (FOG_FASTCALL *BlitNormalizedImageI)(RasterPaintEngine* engine, const BoxI& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform);
  typedef err_t (FOG_FASTCALL *BlitNormalizedImageD)(RasterPaintEngine* engine, const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform);

  typedef err_t (FOG_FASTCALL *ClipAll)(RasterPaintEngine* engine);
  typedef err_t (FOG_FASTCALL *ClipPathF)(RasterPaintEngine* engine, uint32_t clipOp, const PathF& path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *ClipPathD)(RasterPaintEngine* engine, uint32_t clipOp, const PathD& path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *StrokeAndClipPathF)(RasterPaintEngine* engine, uint32_t clipOp, const PathF& path);
  typedef err_t (FOG_FASTCALL *StrokeAndClipPathD)(RasterPaintEngine* engine, uint32_t clipOp, const PathD& path);

  typedef err_t (FOG_FASTCALL *ClipNormalizedBoxI)(RasterPaintEngine* engine, uint32_t clipOp, const BoxI& box);
  typedef err_t (FOG_FASTCALL *ClipNormalizedBoxF)(RasterPaintEngine* engine, uint32_t clipOp, const BoxF& box);
  typedef err_t (FOG_FASTCALL *ClipNormalizedBoxD)(RasterPaintEngine* engine, uint32_t clipOp, const BoxD& box);
  typedef err_t (FOG_FASTCALL *ClipNormalizedPathF)(RasterPaintEngine* engine, uint32_t clipOp, const PathF& path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *ClipNormalizedPathD)(RasterPaintEngine* engine, uint32_t clipOp, const PathD& path, uint32_t fillRule);

  typedef err_t (FOG_FASTCALL *FilterPathF)(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule, const ImageFilter& filter);
  typedef err_t (FOG_FASTCALL *FilterPathD)(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule, const ImageFilter& filter);

  typedef err_t (FOG_FASTCALL *FilterNormalizedBoxI)(RasterPaintEngine* engine, const BoxI& box, const ImageFilter& filter);
  typedef err_t (FOG_FASTCALL *FilterNormalizedBoxF)(RasterPaintEngine* engine, const BoxF& box, const ImageFilter& filter);
  typedef err_t (FOG_FASTCALL *FilterNormalizedBoxD)(RasterPaintEngine* engine, const BoxD& box, const ImageFilter& filter);

  typedef err_t (FOG_FASTCALL *FilterNormalizedPathF)(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule, const ImageFilter& filter);
  typedef err_t (FOG_FASTCALL *FilterNormalizedPathD)(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule, const ImageFilter& filter);

  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  FillAll fillAll;
  FillPathF fillPathF;
  FillPathD fillPathD;
  StrokeAndFillPathF strokeAndFillPathF;
  StrokeAndFillPathD strokeAndFillPathD;

  FillNormalizedBoxI fillNormalizedBoxI;
  FillNormalizedBoxF fillNormalizedBoxF;
  FillNormalizedBoxD fillNormalizedBoxD;
  FillNormalizedPathF fillNormalizedPathF;
  FillNormalizedPathD fillNormalizedPathD;

  BlitImageD blitImageD;
  BlitNormalizedImageA blitNormalizedImageA;
  BlitNormalizedImageI blitNormalizedImageI;
  BlitNormalizedImageD blitNormalizedImageD;

  ClipAll clipAll;
  ClipPathF clipPathF;
  ClipPathD clipPathD;
  StrokeAndClipPathF strokeAndClipPathF;
  StrokeAndClipPathD strokeAndClipPathD;

  ClipNormalizedBoxI clipNormalizedBoxI;
  ClipNormalizedBoxF clipNormalizedBoxF;
  ClipNormalizedBoxD clipNormalizedBoxD;

  ClipNormalizedPathF clipNormalizedPathF;
  ClipNormalizedPathD clipNormalizedPathD;

  FilterPathF filterPathF;
  FilterPathD filterPathD;

  FilterNormalizedBoxI filterNormalizedBoxI;
  FilterNormalizedBoxF filterNormalizedBoxF;
  FilterNormalizedBoxD filterNormalizedBoxD;

  FilterNormalizedPathF filterNormalizedPathF;
  FilterNormalizedPathD filterNormalizedPathD;
};

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
  FOG_INLINE const TransformF& getFinalTransformF() { return stroker.f->_transform; }
  //! @brief The final transformation matrix (double).
  FOG_INLINE const TransformD& getFinalTransformD() { return stroker.d->_transform; }

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
      state->source.color->_argb32.p32 = source.color->_argb32.p32;
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

  FOG_INLINE void discardSource();
  FOG_INLINE void destroyPatternContext(RasterPattern* pc);

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
  const RasterSerializer* serializer;

  // --------------------------------------------------------------------------
  // [Members - Context]
  // --------------------------------------------------------------------------

  //! @brief Context (st).
  RasterContext ctx;

  // --------------------------------------------------------------------------
  // [Members - Flags]
  // --------------------------------------------------------------------------

  //! @brief 'NoPaint', 'Pending' and 'Error' flags.
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
  RasterSource source;
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
  // [Members - Multithreading]
  // --------------------------------------------------------------------------

  //! @brief The worker manager.
  RasterPaintWorkMgr* wm;

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
  Region tmpRegion[2];
};

// ============================================================================
// [Fog::RasterPaintEngine - VTable]
// ============================================================================

extern FOG_NO_EXPORT PaintEngineVTable RasterPaintEngine_vtable[IMAGE_PRECISION_COUNT];
extern FOG_NO_EXPORT RasterSerializer RasterPaintEngine_serializer[RASTER_MODE_COUNT];

// ============================================================================
// [Fog::RasterPaintEngine - Defs]
// ============================================================================

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
