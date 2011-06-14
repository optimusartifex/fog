// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_CLEANUP_H
#define _FOG_CORE_MEMORY_CLEANUP_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>

namespace Fog {

// ============================================================================
// [Function Prototypes]
// ============================================================================

//! @addtogroup Fog_Core_Memory
//! @{

typedef void (FOG_CDECL *MemoryCleanupHandlerFn)(void* closure);

//! @}

namespace Memory {

//! @addtogroup Fog_Core_Memory
//! @{

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

// [Guard]
#endif // _FOG_CORE_MEMORY_CLEANUP_H
