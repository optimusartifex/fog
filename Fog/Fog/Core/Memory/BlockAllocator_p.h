// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_BLOCKALLOCATOR_P_H
#define _FOG_CORE_MEMORY_BLOCKALLOCATOR_P_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::BlockAllocator]
// ============================================================================

//! @internal
//!
//! @brief Custom memory allocator used by raster paint engine.
//!
//! This allocator allocates larger blocks (see @c BLOCK_SIZE) dividing them
//! into small pieces demanded throught @c BlockAllocator::alloc() method.
//! Each allocation contains information about memory block used by allocator
//! and when the memory is not needed (and @c BlockAllocator::free() is called)
//! it's atomically removed from memory block.
//!
//! In short: Each memory block has information about used memory, increased by
//! @c BlockAllocator::alloc() and decreased by @c BlockAllocator::free().
//! When the number is decreased to zero then the block is free and will be
//! reused. The goal of this algorithm is to provide fast memory alloc/free,
//! but do not eat too much memory (reuse it).
//!
//! @note The @c BlockAllocator::alloc() is reentrant and can be called only
//! by main thread, but @c BlockAllocator::free() is thread-safe and it can
//! be called (and it is) from worker threads.
//!
//! @section BlockAllocator vs. ZoneAllocator
//!
//! Block allocator can be used to alloc / free memory chunks, the chunks can
//! be allocated and freed individually. Block allocator can be also used to
//! allocate chunks in main thread and to free them in other threads without
//! locking.
//!
//! Zone allocator can be used only to alloc memory chunks that will be all
//! freed by ZoneAllocator::reset(). Zone allocator arhieves extra performance
//! and nearly zero overhead when allocating very small chunks.
struct FOG_NO_EXPORT BlockAllocator
{
  // --------------------------------------------------------------------------
  // [Constants]
  // --------------------------------------------------------------------------

  //! @brief Size of one block (decreased by some value to always fit in
  //! one/two pages). 96 bytes reserved for @c ::malloc() data and our
  //! @c Block data.
  enum { BLOCK_SIZE = 8080 };

  // --------------------------------------------------------------------------
  // [Block]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief One larger memory block.
  struct Block
  {
    //! @brief Link to next block.
    Block* next;

    //! @brief Size of the block.
    size_t size;
    //! @brief Allocator position, incremented by each @c BlockAllocator::alloc().
    size_t pos;
    //! @brief Count of bytes used by the block (atomic).
    Atomic<size_t> used;

    //! @brief The memory.
    uint8_t memory[BLOCK_SIZE];
  };

  // --------------------------------------------------------------------------
  // [Header]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief Block header.
  struct Header
  {
    //! @brief Link to block structure.
    Block* block;
    //! @brief Size of block (at this time same as BLOCK_SIZE, for future).
    size_t size;
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Block allocator constructor.
  BlockAllocator();
  //! @brief Block allocator destructor, will check if all blocks are freed.
  ~BlockAllocator();

  // --------------------------------------------------------------------------
  // [Alloc / Free]
  // --------------------------------------------------------------------------

  //! @brief Alloc @a size bytes of memory, like @c ::malloc().
  //!
  //! This method is reentrant, never call it from other than engine thread.
  void* alloc(size_t size);

  //! @brief Free @a ptr allocated by @c alloc();
  FOG_INLINE void free(void* ptr);

  //! @brief Reset, will check if all blocks are freed.
  void reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to first memory block (should be the most free one).
  Block* blocks;

private:
  _FOG_CLASS_NO_COPY(BlockAllocator)
};

FOG_INLINE void BlockAllocator::free(void* ptr)
{
  Header* header = reinterpret_cast<Header*>(reinterpret_cast<uint8_t*>(ptr) - sizeof(Header));
  header->block->used.sub(header->size);
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_BLOCKALLOCATOR_P_H
