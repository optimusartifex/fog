// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERMASK_P_H
#define _FOG_G2D_PAINTING_RASTERMASK_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/ZoneAllocator_p.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>
#include <Fog/G2d/Render/RenderFuncs_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

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
  mutable Atomic<uint> reference;

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
  RasterSpan8** rows;
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
  RasterSpan** rowsAdj;

  //! @brief Saved mask span allocator record (will be restored with clip-mask).
  ZoneAllocator::Record* record;

  //! @brief Saved clip CSpan pool (saving clip-mask causes reset).
  RasterSpan* cSpanPool;

  //! @brief Saved clip VSpan pool (saving clip-mask causes reset).
  RasterSpan* vSpanPool[RASTER_MASK_VSPAN_POOL_COUNT];
};

// ============================================================================
// [Fog::RasterMaskSpan8]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
//! @internal
//!
//! @brief Raster paint engine span managed by context.
//!
//! This structure extends classic @c RasterSpan8 with 'len' and 'poolId'
//! members that describes span length and the pool used by the context.
struct FOG_NO_EXPORT RasterMaskSpan8 : public RasterSpan8
{
  //! @brief Maximum length of span in pixels.
  uint32_t maxLen : 16;
  //! @brief Span allocator category
  uint32_t poolId : 16;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::RasterMaskSpan16]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
//! @internal
//!
//! @brief Raster paint engine span managed by context.
//!
//! This structure extends classic @c RasterSpan16 with 'maxLen' and 'poolId'
//! members that describes span length and the pool used by the context.
struct FOG_NO_EXPORT RasterMaskSpan16 : public RasterSpan16
{
  //! @brief Maximum length of span in pixels.
  uint32_t maxLen : 16;
  //! @brief Span allocator category
  uint32_t poolId : 16;
};
#include <Fog/Core/C++/PackRestore.h>

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERMASK_P_H
