// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Api.h>
#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/Core/Memory/Cleanup.h>

namespace Fog {

// ============================================================================
// [Fog::Memory - Cleanup]
// ============================================================================

static void FOG_CDECL _Memory_cleanup()
{
  // TODO:
}

static err_t FOG_CDECL _Memory_registerCleanupHandler(void* handler, void* closure)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL _Memory_unregisterCleanupHandler(void* handler, void* closure)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_memory_init_cleanup(void)
{
  _core.memory.cleanup = _Memory_cleanup;
  _core.memory.registerCleanupHandler = _Memory_registerCleanupHandler;
  _core.memory.unregisterCleanupHandler = _Memory_unregisterCleanupHandler;
}

} // Fog namespace
