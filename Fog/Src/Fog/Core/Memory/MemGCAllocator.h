// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMGCALLOCATOR_H
#define _FOG_CORE_MEMORY_MEMGCALLOCATOR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemGCFuncs]
// ============================================================================

//! @brief @ref MemGCAllocator callback used to destroy object at @a p.
typedef void (FOG_CDECL *MemGCDestroyObjectFunc)(void* p);

//! @brief @ref MemGCAllocator callback to get object size.
typedef size_t (FOG_CDECL *MemGCGetObjectSizeFunc)(void* p);

//! @brief @ref MemGCAllocator callback to check whether the given object at
//! @a p is used. Unused objects can be immediately destroyed.
typedef bool (FOG_CDECL *MemGCIsObjectUsedFunc)(void* p);

//! @brief @ref MemGCAllocator callback to get object size.
typedef bool (FOG_CDECL *MemGCGetObjectMarkFunc)(void* p);

//! @brief @ref MemGCAllocator callback to get object size.
typedef void (FOG_CDECL *MemGCSetObjectMarkFunc)(void* p, bool mark);

struct FOG_NO_EXPORT MemGCFuncs
{
  MemGCDestroyObjectFunc destroyObject;
  MemGCGetObjectSizeFunc getObjectSize;
  MemGCIsObjectUsedFunc isObjectUsed;
  MemGCGetObjectMarkFunc getObjectMark;
  MemGCSetObjectMarkFunc setObjectMark;
};

// ============================================================================
// [Fog::MemGCChunk]
// ============================================================================

//! @brief Chunk of memory allocated by @ref MemGCAllocator.
struct FOG_NO_EXPORT MemGCChunk
{
  //! @brief Incremental allocator position (initially set to zero).
  size_t pos;
  //! @brief Count of bytes currently unused (stored in memory pool).
  size_t unused;
  //! @brief Count of bytes available in this chunk
  //!
  //! Available means that sizeof(MemGCChunk) was subtracted to get an accurate
  //! number.
  size_t length;

  //! @brief Data.
  uint8_t data[sizeof(void*)];
};

// ============================================================================
// [Fog::MemGCQueue]
// ============================================================================

//! @brief Objects marked for GC traversal used by @ref MemGCAllocator.
struct FOG_NO_EXPORT MemGCQueue
{
  //! @brief Link to previous queue node.
  MemGCQueue* prev;
  //! @brief Link to next queue node.
  MemGCQueue* next;

  //! @brief Position in queue (index to the first unused position).
  size_t pos;
  //! @brief Length of queue (count of possible objects to have here).
  size_t length;

  //! @brief Marked objects or scope mark (NULL).
  void* data[1];
};

// ============================================================================
// [Fog::MemGCScope]
// ============================================================================

//! @brief @ref MemGCAllocator scope, used internally.
struct FOG_NO_EXPORT MemGCScope
{
  //! @brief Previous scope.
  MemGCScope* prev;
  //! @brief First GC queue used by this scope.
  MemGCQueue* queue;
  //! @brief First position in GC queue used by this scope.
  size_t pos;
};

// ============================================================================
// [Fog::MemGCAllocatorData]
// ============================================================================

struct FOG_NO_EXPORT MemGCAllocatorData
{
  enum
  {
    POOL_LENGTH = 32,
    POOL_SHIFT = 4,
    POOL_GRANULARITY = (1 << POOL_SHIFT),

    POOL_MAX_SIZE = POOL_LENGTH * POOL_GRANULARITY
  };
  
  enum
  {
    QUEUE_LENGTH = 512
  };

  //! @brief List of memory chunks (sorted by their addresses).
  Static< List<MemGCChunk*> > chunkList;

  //! @brief Actual chunk.
  MemGCChunk* actualChunk;

  //! @brief Fake chunk.
  MemGCChunk fakeChunk;

  //! @brief Size of all memory chunks (size of MemGCChunk is included) and
  //! all large objects allocated by using @ref MemMgr.
  size_t allocatedMemory;
  //! @brief Bytes of memory actually used.
  size_t usedMemory;
  //! @brief Count of blocks in all pools.
  size_t poolCounter;
  //! @brief Base chunk size.
  size_t baseChunkSize;

  //! @brief Pooled blocks using 16-byte granularity.
  void* pool[POOL_LENGTH];
};

// ============================================================================
// [Fog::MemGCAllocator]
// ============================================================================

//! @brief Memory allocator with assisted garbage collection designed to manage
//! small objects (no more than 512 bytes).
struct FOG_API MemGCAllocator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MemGCAllocator(const MemGCFuncs* funcs);
  ~MemGCAllocator();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getAllocatedMemory() const { return _d->allocatedMemory; }
  FOG_INLINE size_t getUsedMemory() const { return _d->usedMemory; }

  // --------------------------------------------------------------------------
  // [Alloc]
  // --------------------------------------------------------------------------

  //! @brief Allocate new object for using GC allocator.
  //!
  //! @note You need to initialize the memory yourself.
  void* alloc(size_t size, size_t* realSize = NULL);

  // --------------------------------------------------------------------------
  // [Mark]
  // --------------------------------------------------------------------------

  //! @brief Put existing object into GC queue, so it can be deleted when the
  //! GC queue is traversed.
  //!
  //! You can use the object after putting into GC queue, it's only mechanism
  //! to prevent memory leaks in case that the object won't be used anymore.
  //!
  //! @note This method shouldn't generally fail. It can however return failure
  //! in extremely rare situation in case the memory allocation for GC queue
  //! failed. Caller must handle such case.
  err_t mark(void* p);

  // --------------------------------------------------------------------------
  // [Release]
  // --------------------------------------------------------------------------

  //! @brief Release the existing object you don't need anymore.
  //!
  //! You can't release a memory block which is in GC queue. Use this method
  //! only if your object (or data) is not queued.
  //!
  //! @note This method can be called by GC to any marked object (object in
  //! queue).
  void release(void* p, size_t size);

  // --------------------------------------------------------------------------
  // [Collect]
  // --------------------------------------------------------------------------

  void collect();

  // --------------------------------------------------------------------------
  // [Scope]
  // --------------------------------------------------------------------------

  void enterScope(MemGCScope* scope);
  void leaveScope(MemGCScope* scope);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief GC allocator data (to decrease size of MemGCAllocator).
  MemGCAllocatorData* _d;

  //! @brief Actual GC scope.
  MemGCScope* _actualScope;
  //! @brief List of queued objects.
  MemGCQueue* _firstQueue;
  //! @brief Link to an actual queue.
  MemGCQueue* _actualQueue;

  //! @brief Fake GC scope (always the first one).
  MemGCScope _fakeScope;
  //! @brief Fake queue to so @c _queue and @c _actual members contains always
  //! a valid pointer.
  MemGCQueue _fakeQueue;

  //! @brief Functions used by GC.
  const MemGCFuncs* _funcs;

private:
  _FOG_NO_COPY(MemGCAllocator)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMGCALLOCATOR_H
