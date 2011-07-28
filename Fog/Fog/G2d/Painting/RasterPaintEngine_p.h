// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTENGINE_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTENGINE_P_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Memory/BlockAllocator_p.h>
#include <Fog/Core/Memory/ZoneAllocator_p.h>
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
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterState_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Painting/RasterWorker_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterPaintEngineVTable]
// ============================================================================

//! @internal
//!
//! @brief @c RasterPaintEngine VTable struct.
//!
//! Currently there are no more members, but in future there could be.
struct FOG_NO_EXPORT RasterPaintEngineVTable : public PaintEngineVTable
{
};

//! @internal
//!
//! @brief Raster paint-engine vtable (versions for different precision).
extern RasterPaintEngineVTable _RasterPaintEngine_vtable;

// ============================================================================
// [Fog::RasterPaintSerializer]
// ============================================================================

//! @internal
//!
//! @brief Raster paint-engine serializer.
//!
//! This class contains function pointers to low-level painter operations. When
//! single-threaded mode is used, the serializer contains function pointers to
//! render functions, otherwise serialize functions are used.
struct FOG_NO_EXPORT RasterPaintSerializer
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_FASTCALL *FillNormalizedBoxI)(RasterPaintEngine* engine, const BoxI& box);
  typedef err_t (FOG_FASTCALL *FillNormalizedBoxF)(RasterPaintEngine* engine, const BoxF& box);
  typedef err_t (FOG_FASTCALL *FillNormalizedBoxD)(RasterPaintEngine* engine, const BoxD& box);

  typedef err_t (FOG_FASTCALL *DrawRawPathF)(RasterPaintEngine* engine, const PathF& path);
  typedef err_t (FOG_FASTCALL *DrawRawPathD)(RasterPaintEngine* engine, const PathD& path);
  typedef err_t (FOG_FASTCALL *FillRawPathF)(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *FillRawPathD)(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *FillNormalizedPathF)(RasterPaintEngine* engine, const PathF& path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *FillNormalizedPathD)(RasterPaintEngine* engine, const PathD& path, uint32_t fillRule);

  typedef err_t (FOG_FASTCALL *BlitRawImageD)(RasterPaintEngine* engine, const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform);
  typedef err_t (FOG_FASTCALL *BlitNormalizedImageI)(RasterPaintEngine* engine, const PointI& pt, const Image& srcImage, const RectI& srcFragment);
  typedef err_t (FOG_FASTCALL *BlitNormalizedTransformedImageI)(RasterPaintEngine* engine, const BoxI& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform);
  typedef err_t (FOG_FASTCALL *BlitNormalizedTransformedImageD)(RasterPaintEngine* engine, const BoxD& box, const Image& srcImage, const RectI& srcFragment, const TransformD& srcTransform);

  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  FillNormalizedBoxI fillNormalizedBoxI;
  FillNormalizedBoxF fillNormalizedBoxF;
  FillNormalizedBoxD fillNormalizedBoxD;

  DrawRawPathF drawRawPathF;
  DrawRawPathD drawRawPathD;
  FillRawPathF fillRawPathF;
  FillRawPathD fillRawPathD;
  FillNormalizedPathF fillNormalizedPathF;
  FillNormalizedPathD fillNormalizedPathD;

  BlitRawImageD blitRawImageD;
  BlitNormalizedImageI blitNormalizedImageI;
  BlitNormalizedTransformedImageI blitNormalizedTransformedImageI;
  BlitNormalizedTransformedImageD blitNormalizedTransformedImageD;
};

//! @internal
//!
//! @brief Raster paint-engine serializer (versions for different mode / precision).
extern RasterPaintSerializer RasterPaintEngine_serialize[RASTER_MODE_COUNT][IMAGE_PRECISION_COUNT];

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
  // [MaxThreads]
  // --------------------------------------------------------------------------

  uint getMaxThreads();

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE bool ensureFinalTransformF()
  {
    if (finalTransformD._type == TRANSFORM_TYPE_IDENTITY)
      return false;
    if (ctx.rasterHints.finalTransformF)
      return true;

    finalTransformF = finalTransformD;
    ctx.rasterHints.finalTransformF = 1;
    return true;
  }

  FOG_INLINE bool isIntegralTransform() const
  {
    return integralTransformType != RASTER_INTEGRAL_TRANSFORM_NONE;
  }

  bool doIntegralTransformAndClip(BoxI& dst, const RectI& src);

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

  void saveSource();
  void saveStroke();
  void saveTransform();
  void saveClipping();

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
  FOG_INLINE void destroyPatternContext(RenderPatternContext* pc);

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

  //! @brief Source type (see @c PATTERN_TYPE).
  uint8_t sourceType;

  //! @brief 'Saved-State' flags.
  //!
  //! Which states was saved to the last @c RasterState instance. If there is
  //! no such instance (the initial state) all bits are set to logical ones.
  uint8_t savedStateFlags;

  //! @brief The strokeParams[F|D] precision.
  uint8_t strokeParamsPrecision;

  // --------------------------------------------------------------------------
  // [Members - Integral-Transform]
  // --------------------------------------------------------------------------

  //! @brief The integralTransform type, see @c RASTER_INTEGRAL_TRANSFORM.
  uint8_t integralTransformType;

  //! @brief The final transform/scaling in integral units.
  struct _IntegralTransform
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
  //! @brief Meta clip-box.
  BoxI metaClipBox;

  // --------------------------------------------------------------------------
  // [Members - Transform]
  // --------------------------------------------------------------------------

  //! @brief The meta transformation matrix.
  TransformD metaTransformD;
  //! @brief The user transformation matrix.
  TransformD userTransformD;

  //! @brief The final transformation matrix (double).
  TransformD finalTransformD;
  //! @brief The final transformation matrix (float).
  TransformF finalTransformF;

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
  struct _StrokeParams
  {
    //! @brief Stroke parameters (float).
    Static<PathStrokerParamsF> f;
    //! @brief Stroke parameters (double).
    Static<PathStrokerParamsD> d;
  } strokeParams;

  // --------------------------------------------------------------------------
  // [Members - State]
  // --------------------------------------------------------------------------

  //! @brief Zone memory allocator for @c RasterState objects.
  ZoneAllocator stateAllocator;
  //! @brief Unused states pool.
  RasterState* statePool;
  //! @brief State.
  RasterState* state;

  // --------------------------------------------------------------------------
  // [Members - Pattern Context]
  // --------------------------------------------------------------------------

  //! @brief Pattern-context allocator.
  ZoneAllocator pcAllocator;
  //! @brief Pattern-context pool.
  RasterAbstractLinkedList* pcPool;

  // --------------------------------------------------------------------------
  // [Members - Multithreading]
  // --------------------------------------------------------------------------

  //! @brief The worker manager.
  RasterWorkerManager* wm;

  //! @brief The maximum number of threads that can be used for rendering after
  //! the multithreading is initialized.
  //!
  //! This value is initialized to number of processors.
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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTENGINE_P_H
