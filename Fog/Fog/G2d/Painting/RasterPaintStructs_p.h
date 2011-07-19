// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTSTRUCTS_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTSTRUCTS_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Painting/RasterPaintConstants_p.h>
#include <Fog/G2d/Rasterizer/Rasterizer_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>
#include <Fog/G2d/Render/RenderFuncs_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting_Raster
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct RasterCalc;
struct RasterCmd;

struct RasterContext;
struct RasterContext8;
struct RasterContext16;
struct RasterContextF;

union RasterHints;
struct RasterLayer;
struct RasterPaintEngine;
struct RasterState;
struct RasterWorker;
struct RasterWorkerManager;

// ============================================================================
// [Function Prototypes]
// ============================================================================

// TODO: Probably remove
typedef void (FOG_FASTCALL *RasterRunFn)(RasterContext* ctx, const void* data);
typedef void (FOG_FASTCALL *RasterFinalizeFn)(RasterContext* ctx, const void* data);

// ============================================================================
// [Fog::RasterAbstractLinkedList]
// ============================================================================

struct RasterAbstractLinkedList
{
  RasterAbstractLinkedList* next;
};

// ============================================================================
// [Fog::RasterHints]
// ============================================================================

//! @internal
//!
//! @brief Raster hints.
union FOG_NO_EXPORT RasterHints
{
  struct
  {
    //! @brief Opacity.
    uint32_t opacity : 24;

    //! @brief Whether the final transformation matrix guarantees rect-to-rect
    //! output.
    //!
    //! This means that rectange remains rectangle after transformation. Rotation
    //! is possible, but only by 90, 128 or 270 degrees.
    //!
    //! @note To check whether the output will be in integral numbers, check the
    //! @c integralTransform flag, because @c rectToRectTransform not guarantees
    //! pixel-aligned output.
    uint32_t rectToRectTransform : 1;

    //! @brief Whether the final transform is also available in float format.
    uint32_t finalTransformF : 1;

    //! @brief Whether the line width and caps are ideal.
    //!
    //! Ideal line means one-pixel width and square caps. If fastLineHint is set
    //! to true then different algorithm can be used for line rendering.
    uint32_t idealLine : 1;
  };

  //! @brief All data packed in single integer.
  uint32_t packed;
};

// ============================================================================
// [Fog::RasterSource]
// ============================================================================

struct FOG_NO_EXPORT RasterSource
{
  union
  {
    //! @brief color.
    Static<Color> color;
    //! @brief Texture.
    Static<Texture> texture;
    //! @brief Gradient.
    Static<GradientD> gradient;
  };

  Static<TransformD> transform;
  Static<TransformD> adjusted;
};

// ============================================================================
// [Fog::RasterStrokeParams]
// ============================================================================

struct RasterStrokeParams
{
  //! @brief Stroke parameters (float).
  Static<PathStrokerParamsF> f;
  //! @brief Stroke parameters (double).
  Static<PathStrokerParamsD> d;
};

// ============================================================================
// [Fog::RasterSpan8]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @internal
//!
//! @brief Raster paint engine span managed by clip-manager.
//!
//! This structure extends classic @c Span8 with 'len' and 'poolId' members that
//! describes span length and the pool used by the clip-manager.
struct FOG_NO_EXPORT RasterSpan8 : public Span8
{
  //! @brief Maximum length of span in pixels.
  uint32_t maxLen : 16;
  //! @brief Span allocator category
  uint32_t poolId : 16;
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::RasterSpan16]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @internal
//!
//! @brief Raster paint engine span managed by clip-manager.
//!
//! This structure extends classic @c Span16 with 'maxLen' and 'poolId' members that
//! describes span length and the pool used by the clip-manager.
struct FOG_NO_EXPORT RasterSpan16 : public Span16
{
  //! @brief Maximum length of span in pixels.
  uint32_t maxLen : 16;
  //! @brief Span allocator category
  uint32_t poolId : 16;
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::RasterLayer]
// ============================================================================

//! @internal
//!
//! @brief The Raster layer.
struct FOG_NO_EXPORT RasterLayer
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    memset(this, 0, sizeof(*this));
  }

  // --------------------------------------------------------------------------
  // [Members - Primary Format]
  // --------------------------------------------------------------------------

  //! @brief Pointer to the first scanline, NEEDED FOR _setupLayer().
  uint8_t* pixels;
  //! @brief The raster-layer size, NEEDED FOR _setupLayer().
  SizeI size;
  //! @brief The raster-layer stride, NEEDED FOR _setupLayer().
  sysint_t stride;
  //! @brief The raster-layer format, NEEDED FOR _setupLayer().
  uint32_t primaryFormat;
  //! @brief The raster-layer bytes-per-pixel.
  uint32_t primaryBPP;
  //! @brief The raster-layer bytes-per-line.
  uint32_t primaryBPL;
  //! @brief The raster-layer pixel precision.
  uint32_t precision;

  // --------------------------------------------------------------------------
  // [Members - Secondary Format]
  // --------------------------------------------------------------------------

  //! @brief The secondary-layer format.
  uint32_t secondaryFormat;
  //! @brief The secondary-layer bytes-per-pixel.
  uint32_t secondaryBPP;
  //! @brief The secondary-layer bytes-per-line.
  uint32_t secondaryBPL;

  //! @brief The 'secondary-from-primary' format blitter.
  RenderVBlitLineFn cvtSecondaryFromPrimary;
  //! @brief The 'primary-from-secondary' format blitter.
  RenderVBlitLineFn cvtPrimaryFromSecondary;

  // --------------------------------------------------------------------------
  // [Members - Image Data]
  // --------------------------------------------------------------------------

  //! @brief Image data instance (optional, for lock/unlock).
  ImageData* imageData;
};

// ============================================================================
// [Fog::RasterMask]
// ============================================================================

//! @internal
//!
//! @brief Raster clip mask.
//!
//! Clip mask is always allocated from main thread, but can be freed by any
//! other thread when it's no longer in use. Clipping management is connected
//! to state management and it's based on FIFO solution.
//!
//! @section Clipping and multithreading
//!
//! Multithreaded clipping is inplemented by the same way as singlethreaded,
//! but commands and calculations are used to tell threads what to do. There
//! are also commands for state-management thats unique to the mask-clipping
//! (and only used by the mask-clipping).
//!
//! @section Snapshots.
//!
//! Mask-clipping management is based on the snapshots. Snapshots are very
//! similar to ones contained in @c ZoneMemoryManager, but improved to handle
//! pool buffers and additional data. Snapshot is only used when needed. For
//! example if you save() painter state and do some work that is not related
//! to clipping then snapshot-management is NOT used. Also if you change
//! clipping type to another (using @c CLIP_OP_REPLACE operator) then the
//! snapshot is not created. Snapshot is always created with new mask and
//! describes the state before the mask was created (previous mask data).
struct FOG_NO_EXPORT RasterMask
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count (when it goes to zero then mask should be destroyed).
  //!
  //! Saving mask to state increases the reference count.
  mutable Atomic<uint> refCount;

  //! @brief Mask ID.
  uint id;

  //! @brief Whether the mask was saved.
  //!
  //! Only used by the master context, clip state management and serializers. If
  //! the clip command is being serialized and saved is larger than zero then new
  //! @c RasterMask must be created.
  uint saved;

  //! @brief Work clip box, copied from master.
  BoxI workBox;

  //! @brief Work width (minimal width of allocated mask per row).
  uint width;
  //! @brief Work height.
  uint height;

  //! @brief Spans data, first span - spans[0] is pointer to span at physical
  //! position workBox.y1 in raster. It's to save space and to be correct.
  //! (I don't like buggy pointers, the pointer is adjusted in the code).
  //!
  //! To access the spans use always @c getSpansAt() method.
  //!
  //! The length of the array is workBox.getHeight().
  Span8** spans;
};

// ============================================================================
// [Fog::RasterMaskSnapshot]
// ============================================================================

//! @internal
//!
//! @brief Saved clip-mask snapshot.
//!
//! This structure is used to hold some data needed to save / restore clip-mask.
//! It's managed by the clipper and state manager.
struct FOG_NO_EXPORT RasterMaskSnapshot
{
  //! @brief Previous snapshot (can be @c NULL if this is the first one).
  RasterMaskSnapshot* prev;

  //! @brief The type of snapshot, see @c RASTER_SNAPSHOT_TYPE.
  uint type;

  //! @brief Pointer to the related mask.
  RasterMask* mask;

  //! @brief Saved y1.
  int y1;
  //! @brief Saved y2.
  int y2;

  //! @bried Saved rowsAdj value.
  Span** rowsAdj;

  //! @brief Saved mask span allocator record (will be restored with clip-mask).
  ZoneAllocator::Record* record;

  //! @brief Saved clip CSpan pool (saving clip-mask causes reset).
  Span* cSpanPool;

  //! @brief Saved clip VSpan pool (saving clip-mask causes reset).
  Span* vSpanPool[RASTER_MASK_VSPAN_POOL_COUNT];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTSTRUCTS_P_H
