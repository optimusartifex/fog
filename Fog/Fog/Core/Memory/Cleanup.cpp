// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/Cleanup.h>

namespace Fog {

// ============================================================================
// [Fog::Memory - Cleanup]
// ============================================================================

static void FOG_CDECL Memory_cleanup()
{
  // TODO:
}

static err_t FOG_CDECL Memory_registerCleanupHandler(void* handler, void* closure)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL Memory_unregisterCleanupHandler(void* handler, void* closure)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemoryCleanup_init(void)
{
  _api.memory.cleanup = Memory_cleanup;
  _api.memory.registerCleanupHandler = Memory_registerCleanupHandler;
  _api.memory.unregisterCleanupHandler = Memory_unregisterCleanupHandler;
}

} // Fog namespace
