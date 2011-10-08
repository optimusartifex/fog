// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMMGR_H
#define _FOG_CORE_MEMORY_MEMMGR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemMgr - Alloc / Free]
// ============================================================================

struct FOG_NO_EXPORT MemMgr
{
  static FOG_INLINE void* alloc(size_t size)
  {
    return _api.memmgr_alloc(size);
  }

  static FOG_INLINE void* calloc(size_t size)
  {
    return _api.memmgr_calloc(size);
  }

  static FOG_INLINE void* realloc(void* addr, size_t size)
  {
    return _api.memmgr_realloc(addr, size);
  }

  static FOG_INLINE void free(void* addr)
  {
    return _api.memmgr_free(addr);
  }

  // ============================================================================
  // [Fog::MemMgr - Cleanup]
  // ============================================================================

  static FOG_INLINE void cleanup(uint32_t reason)
  {
    _api.memmgr_cleanup(reason);
  }

  static FOG_INLINE err_t registerCleanupFunc(MemCleanupFunc func, void* closure)
  {
    return _api.memmgr_registerCleanupFunc(func, closure);
  }

  static FOG_INLINE err_t unregisterCleanupFunc(MemCleanupFunc func, void* closure)
  {
    return _api.memmgr_unregisterCleanupFunc(func, closure);
  }

  // ============================================================================
  // [Fog::MemMgr - Physical Memory]
  // ============================================================================

  static FOG_INLINE uint64_t getAmountOfPhysicalMemory()
  {
    return _api.memmgr_getAmountOfPhysicalMemory();
  }

  static FOG_INLINE uint64_t getAmountOfPhysicalMemoryMB()
  {
    return _api.memmgr_getAmountOfPhysicalMemoryMB();
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMMGR_H
