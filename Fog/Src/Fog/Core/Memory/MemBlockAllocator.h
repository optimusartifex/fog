// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMBLOCKALLOCATOR_H
#define _FOG_CORE_MEMORY_MEMBLOCKALLOCATOR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemBlockNode]
// ============================================================================

//! @internal
//!
//! @brief One larger memory block.
struct FOG_NO_EXPORT MemBlockNode
{
  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t size)
  {
    return sizeof(MemBlockNode) - 16 + size;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to next node.
  MemBlockNode* next;

  //! @brief Size of the data.
  size_t size;
  //! @brief Allocator position, incremented by each @c MemBlockAllocator::alloc().
  size_t pos;

  //! @brief Count of allocations (atomically increased / decreased).
  Atomic<size_t> used;

  //! @brief The memory.
  uint8_t data[16];
};

// ============================================================================
// [Fog::MemBlockAllocator]
// ============================================================================

//! @internal
//!
//! @brief Custom memory allocator used by raster paint engine.
//!
//! This allocator allocates larger blocks (see @c BLOCK_SIZE) dividing them
//! into small pieces demanded through @c MemBlockAllocator::alloc() method.
//! Each allocation contains information about memory block used by allocator
//! and when the memory is not needed (and @c MemBlockAllocator::free() is called)
//! it's atomically removed from memory block.
//!
//! In short: Each memory block has information about used memory, increased by
//! @c MemBlockAllocator::alloc() and decreased by @c MemBlockAllocator::free().
//! When the number is decreased to zero then the block is free and will be
//! reused. The goal of this algorithm is to provide fast memory alloc/free,
//! but do not eat too much memory (reuse it).
//!
//! @note The @c MemBlockAllocator::alloc() is reentrant and can be called only
//! by main thread, but @c MemBlockAllocator::free() is thread-safe and it can
//! be called (and it is) from worker threads.
//!
//! @section MemBlockAllocator vs. MemZoneAllocator
//!
//! Block allocator can be used to alloc / free memory nodes, the nodes can
//! be allocated and freed individually. Block allocator can be also used to
//! allocate nodes in main thread and to free them in other threads without
//! locking.
//!
//! Zone allocator can be used only to alloc memory nodes that will be all
//! freed by MemZoneAllocator::reset(). Zone allocator arhieves extra performance
//! and nearly zero overhead when allocating very small nodes.
struct FOG_NO_EXPORT MemBlockAllocator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Block allocator constructor.
  FOG_INLINE MemBlockAllocator()
  {
    fog_api.memblockallocator_ctor(this);
  }

  //! @brief Block allocator destructor, will check if all blocks are freed.
  FOG_INLINE ~MemBlockAllocator()
  {
    fog_api.memblockallocator_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Alloc / Free]
  // --------------------------------------------------------------------------

  //! @brief Alloc @a size bytes of memory, like @c ::malloc().
  //!
  //! This method is reentrant, never call it from other than engine thread.
  FOG_INLINE void* alloc(size_t size)
  {
    return fog_api.memblockallocator_alloc(this, size);
  }

  //! @brief Free @a ptr allocated by @c alloc();
  FOG_INLINE void free(void* ptr)
  {
    MemBlockNode* node = *reinterpret_cast<MemBlockNode**>(reinterpret_cast<uint8_t*>(ptr) - sizeof(MemBlockNode**));
    node->used.dec();
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  //! @brief Reset, will check if all blocks are freed.
  FOG_INLINE void reset()
  {
    fog_api.memblockallocator_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to first memory block (should be the most free one).
  MemBlockNode* nodes;

private:
  FOG_NO_COPY(MemBlockAllocator)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMBLOCKALLOCATOR_H
