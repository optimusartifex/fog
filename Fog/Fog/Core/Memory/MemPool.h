// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMPOOL_H
#define _FOG_CORE_MEMORY_MEMPOOL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemPool]
// ============================================================================

struct FOG_NO_EXPORT MemPool
{
  // --------------------------------------------------------------------------
  // [Link]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT Link
  {
    Link* next;
  };

  // --------------------------------------------------------------------------
  // [Chunk]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT Chunk
  {
    Chunk* next;
    size_t capacity;
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MemPool()
  {
    _api.mempool.ctor(this);
  }

  FOG_INLINE ~MemPool()
  {
    _api.mempool.dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getCapacity() const
  {
    return _api.mempool.getCapacity(this);
  }

  // --------------------------------------------------------------------------
  // [Reset / Save]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.mempool.reset(this);
  }

  FOG_INLINE void* save()
  {
    return _api.mempool.save(this);
  }

  // --------------------------------------------------------------------------
  // [Alloc / Free]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t prealloc(size_t szItem, size_t count)
  {
    return _api.mempool.prealloc(this, szItem, count);
  }

  //! @brief Alloc a memory (or reuse the existing allocation) of @a size bytes.
  //!
  //! @param szItem Bytes of memory to alloc, all allocations of a single
  //! memory pool must use the same @a size value, it's not stored by the pool
  //! to minimize the runtime size of it.
  FOG_INLINE void* alloc(size_t szItem)
  {
    Link* p = unused;

    if (p == NULL)
      return _api.mempool.alloc(this, szItem);

    unused = p->next;
    return reinterpret_cast<void*>(p);
  }

  FOG_INLINE void free(void* _p)
  {
    Link* p = reinterpret_cast<Link*>(_p);
    
    p->next = unused;
    unused = p;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE void freeSaved(void* p)
  {
    _api.mempool.freeSaved(p);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Link* unused;
  Chunk* chunk;

private:
  _FOG_NO_COPY(MemPool)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMPOOL_H
