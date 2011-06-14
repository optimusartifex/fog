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
#include <Fog/G2d/Painting/RasterPaintConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintFuncs_p.h>
#include <Fog/G2d/Painting/RasterPaintState_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterPaintUtil_p.h>
#include <Fog/G2d/Painting/RasterPaintWorker_p.h>
#include <Fog/G2d/Rasterizer/Rasterizer_p.h>
#include <Fog/G2d/Rasterizer/Scanline_p.h>
#include <Fog/G2d/Rasterizer/Span_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting_Raster
//! @{

// ============================================================================
// [Fog::RasterPainterVTable]
// ============================================================================

//! @internal
//!
//! @brief @c RasterPaintEngine VTable struct.
//!
//! Currently there are no more members, but in future there could be.
struct FOG_NO_EXPORT RasterPainterVTable : public PainterVTable {};

//! @internal
//!
//! @brief @c RasterPaintEngine VTable instances.
//!
//! For performance reasons there are vtable for these combinations:
//!
//!   1. Threading model - ST(Singlethreaded) and MT(Multithreaded).
//!   2. Precision - BYTE, WORD and FLOAT.
extern RasterPainterVTable _RasterPaintEngine_vtable[RASTER_MODE_COUNT];

// ============================================================================
// [Fog::RasterPaintEngine]
// ============================================================================

//! @internal
//!
//! @brief Raster engine.
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
  // [Members - Context]
  // --------------------------------------------------------------------------

  //! @brief The context (st).
  RasterContext ctx;

  // --------------------------------------------------------------------------
  // [Members - Allocators]
  // --------------------------------------------------------------------------

  //! @brief Block memory allocator for small objects.
  BlockAllocator blockAllocator;

  // --------------------------------------------------------------------------
  // [Methods - Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isIntegralTransform() const
  {
    return finalTransformI._type != RASTER_INTEGRAL_TRANSFORM_NULL;
  }

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

  //! @brief The type of core clip (coreRegion, coreClipBox).
  //!
  //! @note Don't miss it with the @c finalType member!
  uint8_t coreClipType;

  // --------------------------------------------------------------------------
  // [Members - Source & Opacity]
  // --------------------------------------------------------------------------

  //! @brief Source.
  RasterSource source;
  //! @brief The original opacity.
  float opacityF;

  // --------------------------------------------------------------------------
  // [Members - Stroke Params]
  // --------------------------------------------------------------------------

  RasterStrokeParams strokeParams;

  // --------------------------------------------------------------------------
  // [Members - Transform]
  // --------------------------------------------------------------------------

  //! @brief The core transformation matrix.
  TransformD coreTransform;
  //! @brief The user transformation matrix.
  TransformD userTransform;

  //! @brief The final transformation matrix (double).
  TransformD finalTransform;
  //! @brief The final transformation matrix (float).
  TransformF finalTransformF;

  //! @brief The core translation point in pixels (negated coreOrigin).
  PointI coreTranslationI;

  //! @brief The final translation point in pixels, available if the core
  //! transformation matrix is @c TRANSFORM_TYPE_IDENTITY or
  //! @c TRANSFORM_TYPE_TRANSLATION and the values of the final transformation
  //! matrix are integrals.
  struct
  {
    //! @brief The finalTransformI type, see @c RASTER_INTEGRAL_TRANSFORM.
    int _type;

    int _sx, _sy;
    int _tx, _ty;
  } finalTransformI;

  // --------------------------------------------------------------------------
  // [Members - Clipping]
  // --------------------------------------------------------------------------

  //! @brief The core clip-box.
  BoxI coreClipBox;

  //! @brief The meta origin.
  PointI metaOrigin;
  //! @brief The user origin.
  PointI userOrigin;
  //! @brief The core origin (the meta origin + the user origin).
  PointI coreOrigin;

  //! @brief The meta region.
  Region metaRegion;
  //! @brief The user region.
  Region userRegion;
  //! @brief The core region (the meta region & the user region).
  Region coreRegion;

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

  ZoneAllocator pcAllocator;
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

  // TODO: Remove?
  // Temporary glyph set.
  //GlyphSet tmpGlyphSet;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTENGINE_P_H
