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
namespace MemMgr {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemMgr - Alloc / Free]
// ============================================================================

static FOG_INLINE void* alloc(size_t size)
{
  return _api.memmgr._m_alloc(size);
}

static FOG_INLINE void* calloc(size_t size)
{
  return _api.memmgr._m_calloc(size);
}

static FOG_INLINE void* realloc(void* addr, size_t size)
{
  return _api.memmgr._m_realloc(addr, size);
}

static FOG_INLINE void free(void* addr)
{
  return _api.memmgr._m_free(addr);
}

// ============================================================================
// [Fog::MemMgr - Cleanup]
// ============================================================================

static FOG_INLINE void cleanup()
{
  _api.memmgr.cleanup();
}

static FOG_INLINE err_t registerCleanupFunc(MemCleanupFunc func, void* closure)
{
  return _api.memmgr.registerCleanupFunc(func, closure);
}

static FOG_INLINE err_t unregisterCleanupFunc(MemCleanupFunc func, void* closure)
{
  return _api.memmgr.unregisterCleanupFunc(func, closure);
}

//! @}

} // MemMgr namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMMGR_H
