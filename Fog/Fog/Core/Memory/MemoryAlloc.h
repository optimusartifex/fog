// Not ready...
#if 0

// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMORYALLOC_H
#define _FOG_CORE_MEMORY_MEMORYALLOC_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Api.h>

namespace Fog {

// ============================================================================
// [Function Prototypes]
// ============================================================================

typedef void (FOG_CDECL *MemoryCleanupHandlerFn)(void* closure);

// ============================================================================
// [Fog::Memory]
// ============================================================================

//! @brief Memory related functions.
//!
//! @c new() and @c delete() in @c Fog namespace are modified to call
//! @c Memory::alloc() and @c Memory::free() functions.
namespace Memory {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::Memory - Alloc / Free]
// ============================================================================

static FOG_INLINE void* alloc(sysuint_t size)
{
  return _core.memory.alloc(size);
}

static FOG_INLINE void* calloc(sysuint_t size)
{
  return _core.memory.calloc(size);
}

static FOG_INLINE void* realloc(void* addr, sysuint_t size)
{
  return _core.memory.realloc(addr, size);
}

static FOG_INLINE void* reallocOrFree(void* addr, sysuint_t size)
{
  return _core.memory.reallocOrFree(addr, size);
}

static FOG_INLINE void free(void* addr)
{
  return _core.memory.free(addr);
}

// ============================================================================
// [Fog::Memory - Cleanup]
// ============================================================================

static FOG_INLINE void cleanup()
{
  _core.memory.cleanup();
}

static FOG_INLINE err_t registerCleanupHandler(MemoryCleanupHandlerFn handler, void* closure)
{
  return _core.memory.registerCleanupHandler((void*)handler, closure);
}

static FOG_INLINE err_t unregisterCleanupHandler(MemoryCleanupHandlerFn handler, void* closure)
{
  return _core.memory.unregisterCleanupHandler((void*)handler, closure);
}

//! @}

} // Memory namespace
} // Fog namespace

#if !defined(FOG_USE_STDNEW)
static FOG_INLINE void* _fog_memory_alloc(sysuint_t size) { return Fog::Memory::alloc(size); }
static FOG_INLINE void _fog_memory_free(void* addr) { return Fog::Memory::free(addr); }
#endif

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMORYALLOC_H


#endif
