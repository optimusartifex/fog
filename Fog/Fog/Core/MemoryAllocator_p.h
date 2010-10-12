// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORYALLOCATOR_P_H
#define _FOG_CORE_MEMORYALLOCATOR_P_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Memory.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::BlockMemoryAllocator]
// ============================================================================

//! @internal
//!
//! @brief Custom memory allocator used by raster paint engine.
//!
//! This allocator allocates larger blocks (see @c BLOCK_SIZE) dividing them
//! into small pieces demanded throught @c BlockMemoryAllocator::alloc() method.
//! Each allocation contains information about memory block used by allocator
//! and when the memory is not needed (and @c BlockMemoryAllocator::free() is called)
//! it's atomically removed from memory block.
//!
//! In short: Each memory block has information about used memory, increased by
//! @c BlockMemoryAllocator::alloc() and decreased by @c BlockMemoryAllocator::free().
//! When the number is decreased to zero then the block is free and will be 
//! reused. The goal of this algorithm is to provide fast memory alloc/free,
//! but do not eat too much memory (reuse it).
//!
//! @note The @c BlockMemoryAllocator::alloc() is reentrant and can be called only
//! by main thread, but @c BlockMemoryAllocator::free() is thread-safe and it can
//! be called (and it is) from worker threads.
//!
//! @section BlockMemoryAllocator vs. ZoneMemoryAllocator
//!
//! Block allocator can be used to alloc / free memory chunks, the chunks can
//! be allocated and freed individually. Block allocator can be also used to
//! allocate chunks in main thread and to free them in other threads without
//! locking.
//!
//! Zone allocator can be used only to alloc memory chunks that will be all
//! freed by ZoneMemoryAllocator::reset(). Zone allocator arhieves extra performance
//! and nearly zero overhead when allocating very small chunks.
struct FOG_HIDDEN BlockMemoryAllocator
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
    sysuint_t size;
    //! @brief Allocator position, incremented by each @c BlockMemoryAllocator::alloc().
    sysuint_t pos;
    //! @brief Count of bytes used by the block (atomic).
    Atomic<sysuint_t> used;

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
    sysuint_t size;
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Block allocator constructor.
  BlockMemoryAllocator();
  //! @brief Block allocator destructor, will check if all blocks are freed.
  ~BlockMemoryAllocator();

  // --------------------------------------------------------------------------
  // [Alloc / Free]
  // --------------------------------------------------------------------------

  //! @brief Alloc @a size bytes of memory, like @c ::malloc().
  //!
  //! This method is reentrant, never call it from other than engine thread.
  void* alloc(sysuint_t size);

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
  FOG_DISABLE_COPY(BlockMemoryAllocator)
};

FOG_INLINE void BlockMemoryAllocator::free(void* ptr)
{
  Header* header = reinterpret_cast<Header*>(reinterpret_cast<uint8_t*>(ptr) - sizeof(Header));
  header->block->used.sub(header->size);
}

// ============================================================================
// [Fog::ZoneMemoryAllocator]
// ============================================================================

//! @internal
//!
//! @brief Memory allocator designed to fast alloc memory that will be freed
//! in one step (used by raster paint engine and scanline container for 
//! temporary objects).
//!
//! This is hackery for performance. Concept is that objects created by
//! @c ZoneMemoryAllocator are freed all at once. This means that lifetime of
//! these objects are the same as the lifetime of the @c ZoneMemoryAllocator
//! itself. Optionally it's possible to call @c record() and @c revert() methods
//! that can be used to record current allocation position and to revert
//! it back. This is used by clip-span engine to reuse memory used by the 
//! clip state that was restored (data not needed anymore).
//!
//! This class was stripped from AsmJit, and little modified for clip-span 
//! allocator:
//!   http://code.google.com/p/asmjit/
struct FOG_HIDDEN ZoneMemoryAllocator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new instance of zone allocator.
  //! @param chunkSize Default size for one zone chunk.
  ZoneMemoryAllocator(sysuint_t chunkSize);

  //! @brief Destroy the zone allocator instance.
  ~ZoneMemoryAllocator();

  // --------------------------------------------------------------------------
  // [Chunk]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief One allocated chunk of memory.
  struct Chunk
  {
    //! @brief Link to previous chunk.
    Chunk* prev;
    //! @brief Link to next chunk (optional, used by clip-span allocator).
    Chunk* next;

    //! @brief Current position in this chunk (data + offset).
    uint8_t* pos;
    //! @brief End position (first invalid byte position) of this chunk.
    uint8_t* end;

    //! @brief Data.
    uint8_t data[sizeof(void*)];
  };

  // --------------------------------------------------------------------------
  // [Record]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief Zone allocator record, returned by @c record() call.
  struct Record
  {
    //! @brief Current chunk.
    Chunk* current;

    //! @brief Current chunk position pointer (saved).
    uint8_t* pos;
  };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Allocate @c size bytes of memory and return pointer to it.
  //!
  //! Pointer allocated by this way will be valid until @c ZoneMemoryAllocator object
  //! is destroyed. To create class by this way use placement @c new and
  //! @c delete operators:
  //!
  //! @code
  //! // Example of allocating simple class
  //!
  //! // Your class
  //! class Object
  //! {
  //!   // members...
  //! };
  //!
  //! // Your function
  //! void f()
  //! {
  //!   // We are using AsmJit namespace
  //!   using namespace AsmJit
  //!
  //!   // Create zone object with chunk size of 65536 bytes.
  //!   ZoneMemoryAllocator zone(8096);
  //!
  //!   // Create your objects using zone object allocating, for example:
  //!   Object* obj = new(zone.alloc(sizeof(YourClass))) Object();
  //!
  //!   // ... lifetime of your objects ...
  //!
  //!   // Destroy your objects:
  //!   obj->~Object();
  //!
  //!   // ZoneMemoryAllocator destructor will free all memory allocated through it,
  //!   // alternative is to call @c zone.free().
  //! }
  //! @endcode
  FOG_INLINE void* alloc(sysuint_t size)
  {
    // Chunks must be valid pointer if we are here.
    FOG_ASSERT(_current != NULL);

    // This allocator wasn't designed to alloc huge amount of memory (larger
    // than the chunk size), so never do it!
    FOG_ASSERT(size < _chunkSize);

    uint8_t* p = _current->pos;
    _current->pos += size;

    if (FOG_UNLIKELY(_current->pos > _current->end)) return _alloc(size);
    return (void*)p;
  }

  //! @brief Internal alloc method (called by @c alloc() inline).
  //!
  //! There is no reason to inline this method, because the @c alloc() is
  //! sufficient (if we need to call libc malloc then the cost of calling
  //! @c _alloc() is zero).
  void* _alloc(sysuint_t size);

  //! @brief Record current state.
  Record* record();

  //! @brief Revert to state previously recorded by @c record() method.
  void revert(Record* record, bool keepRecord = false);

  //! @brief Invalidate all allocated memory, next call to @a alloc() will
  //! return memory chunk allocated using the first chunk.
  void reset();

  //! @brief Free all chunks, except the first one that must be always 
  //! available.
  void free();

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

private:
  FOG_INLINE Chunk* _allocChunk()
  {
    return reinterpret_cast<Chunk*>(
      Memory::alloc(sizeof(Chunk) - sizeof(void*) + _chunkSize));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief First allocated chunk of memory.
  //!
  //! @note This chunk is statically allocated and its length is zero. It
  //! prevents some checks in @c alloc() method and it also prevents from
  //! allocation memory in case that the instance wasn't used.
  Chunk _first;
  //! @brief Current allocated chunk of memory.
  Chunk* _current;

  //! @brief One chunk size.
  sysuint_t _chunkSize;

private:
  FOG_DISABLE_COPY(ZoneMemoryAllocator)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORYALLOCATOR_P_H
