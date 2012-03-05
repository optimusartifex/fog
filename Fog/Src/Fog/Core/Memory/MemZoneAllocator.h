// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMZONEALLOCATOR_H
#define _FOG_CORE_MEMORY_MEMZONEALLOCATOR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemZoneNode]
// ============================================================================

//! @internal
//!
//! @brief One allocated chunk of memory (node).
struct FOG_NO_EXPORT MemZoneNode
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to previous node.
  MemZoneNode* prev;
  //! @brief Link to next node (optional, used by clip-span allocator).
  MemZoneNode* next;

  //! @brief Data.
  uint8_t data[sizeof(void*)];
};

// ============================================================================
// [Fog::MemZoneRecord]
// ============================================================================

//! @internal
//!
//! @brief Zone allocator record, returned by @c MemZoneAllocator::record() call.
struct FOG_NO_EXPORT MemZoneRecord
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Current node.
  MemZoneNode* current;
};

// ============================================================================
// [Fog::MemZoneAllocator]
// ============================================================================

//! @brief Incremental memory allocator designed to allocate memory for objects
//! or data with short lifetime.
//!
//! This is hackery for performance. Concept is that objects created by
//! @c MemZoneAllocator are freed all at once. This means that lifetime of
//! these objects are the same as the lifetime of the @c MemZoneAllocator
//! itself. Optionally it's possible to call @c record() and @c revert() methods
//! that can be used to record current allocation position and to revert
//! it back. This is used mainly by @c RasterPaintEngine.
struct FOG_NO_EXPORT MemZoneAllocator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new instance of zone allocator.
  //! @param nodeSize Default size for one zone node.
  FOG_INLINE MemZoneAllocator(uint32_t nodeSize)
  {
    fog_api.memzoneallocator_ctor(this, nodeSize);
  }

  //! @brief Destroy the zone allocator instance.
  FOG_INLINE ~MemZoneAllocator()
  {
    fog_api.memzoneallocator_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Alloc]
  // --------------------------------------------------------------------------

  //! @brief Allocate @c size bytes of memory and return pointer to it.
  //!
  //! Pointer allocated by this way will be valid until @c MemZoneAllocator object
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
  //!   // Create zone object with node size of 65536 bytes.
  //!   MemZoneAllocator zone(8096);
  //!
  //!   // Create your objects using zone object allocating, for example:
  //!   Object* obj = new(zone.alloc(sizeof(Object))) Object();
  //!
  //!   // ... lifetime of your objects ...
  //!
  //!   // Destroy your objects:
  //!   obj->~Object();
  //!
  //!   // MemZoneAllocator destructor will free all memory allocated through it,
  //!   // alternative is to call @c zone.free().
  //! }
  //! @endcode
  FOG_INLINE void* alloc(size_t size)
  {
    // Current node must be valid pointer if we are here.
    FOG_ASSERT(_current != NULL);

    // This allocator wasn't designed to alloc huge amount of memory (larger
    // than the node size), so never do it!
    FOG_ASSERT(size < _nodeSize);

    uint8_t* p = _pos;
    _pos += size;

    if (FOG_UNLIKELY(_pos > _end))
      return fog_api.memzoneallocator_alloc(this, size);

    return (void*)p;
  }

  //! @brief Get whether the specific count of bytes can be allocated on the
  //! current node.
  FOG_INLINE bool canAlloc(size_t size)
  {
    return _pos + size <= _end;
  }

  FOG_INLINE void* allocNoCheck(size_t size)
  {
    FOG_ASSERT(canAlloc(size));

    uint8_t* p = _pos;
    _pos += size;
    
    return (void*)p;
  }

  // --------------------------------------------------------------------------
  // [Reuse / Reset]
  // --------------------------------------------------------------------------

  //! @brief Invalidate all allocated memory, but do not free allocated nodes.
  //!
  //! This method should be used when one task which needed zone memory ended,
  //! but another needs to be run. It has advantage that heap memory is already
  //! allocated so it doesn't need to be allocated again.
  FOG_INLINE void clear()
  {
    _current = _firstUsable;
    _pos = _current->data;
    _end = _pos + _firstSize;
  }

  //! @brief Free allocated memory.
  FOG_INLINE void reset()
  {
    return fog_api.memzoneallocator_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Record / Revert]
  // --------------------------------------------------------------------------

  //! @brief Record current state.
  FOG_INLINE MemZoneRecord* record()
  {
    return fog_api.memzoneallocator_record(this);
  }

  //! @brief Revert to state previously recorded by @c record() method.
  FOG_INLINE void revert(MemZoneRecord* record, bool keepRecord = false)
  {
    return fog_api.memzoneallocator_revert(this, record, keepRecord);
  }

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

  FOG_INLINE MemZoneNode* _allocChunk()
  {
    return reinterpret_cast<MemZoneNode*>(
      MemMgr::alloc(sizeof(MemZoneNode) - sizeof(void*) + _nodeSize));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Current position in @c _current node (data + offset).
  uint8_t* _pos;
  //! @brief End position (first invalid byte position) in @c _current node.
  uint8_t* _end;

  //! @brief Current allocated node of memory.
  MemZoneNode* _current;
  //! @brief First usable node.
  //!
  //! @note If the @c MemZoneAllocatorTmp<N> is used, then @c _firstUsable is
  //! linked with @c _first node, but if only pure @c MemZoneallocator is used,
  //! then @c _firstUsable is linked to the first node which contains memory.
  //! This is optimization to make @c clear() method as fastest (and smallest)
  //! as possible.
  MemZoneNode* _firstUsable;

  //! @brief One node size.
  uint32_t _nodeSize;

  //! @brief First node size.
  //!
  //! @note If the @c MemZoneAllocatorTmp<N> is used, then @c _firstSize is
  //! always the same as N (template parameter). It's never cleared or changed.
  //! If the pure @c MemZoneAllocator is used, then first size is first set to
  //! zero, and when the first node is dynamically allocated, _firstSize is set
  //! to @c _nodeSize. It's also set to zero when @c reset() is called to
  //! prevent memory corruption, because @c reset() frees all dynamically
  //! allocated nodes.
  uint32_t _firstSize;

  //! @brief First allocated node of memory.
  //!
  //! @note This node is statically allocated and its length is zero. It
  //! prevents some checks in @c alloc() method and it also prevents from
  //! allocation memory in case that the instance has not been used.
  MemZoneNode _first;

private:
  FOG_NO_COPY(MemZoneAllocator)
};

template<size_t N>
struct FOG_NO_EXPORT MemZoneAllocatorTmp : public MemZoneAllocator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MemZoneAllocatorTmp(uint32_t nodeSize) :
    MemZoneAllocator(nodeSize)
  {
    _firstSize = static_cast<uint32_t>(N);
    _end += N;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  char _buffer[N];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMZONEALLOCATOR_H
