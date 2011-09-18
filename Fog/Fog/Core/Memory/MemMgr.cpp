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
#include <Fog/Core/Memory/MemDebug_p.h>
#include <Fog/Core/Memory/MemMgr.h>

// [Dependencies - C]
#include <stdio.h>
#include <stdlib.h>

namespace Fog {

// ===========================================================================
// [Configuration]
// ===========================================================================

#define FOG_DEBUG_MEMORY 0

// ===========================================================================
// [Fog::MemMgr - Alloc / Realloc / Free]
// ===========================================================================

static void* FOG_CDECL Memory_alloc(size_t size)
{
  void* p = ::malloc(size);

  if (FOG_IS_NULL(p) && size > 0)
  {
    MemMgr::cleanup();
    p = ::malloc(size);
  }

  return p;
}

static void* FOG_CDECL Memory_calloc(size_t size)
{
  void* p = ::calloc(size, 1);

  if (FOG_IS_NULL(p) && size > 0)
  {
    MemMgr::cleanup();
    p = ::calloc(size, 1);
  }

  return p;
}

static void* FOG_CDECL Memory_realloc(void* p, size_t size)
{
  void* newp;

  if (FOG_IS_NULL(p))
  {
    return _api.memmgr._m_alloc(size);
  }

  if (FOG_UNLIKELY(size == 0))
  {
    _api.memmgr._m_free(p);
    return NULL;
  }

  newp = ::realloc(p, size);
  if (FOG_IS_NULL(newp))
  {
    MemMgr::cleanup();
    newp = ::realloc(p, size);
  }

  return newp;
}

static void FOG_CDECL Memory_free(void* p)
{
  ::free(p);
}

// ============================================================================
// [Fog::MemMgr - Cleanup]
// ============================================================================

static void FOG_CDECL MemMgr_cleanup()
{
  // TODO:
}

static err_t FOG_CDECL MemMgr_registerCleanupFunc(MemCleanupFunc func, void* closure)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MemMgr_unregisterCleanupFunc(MemCleanupFunc func, void* closure)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemMgr_init(void)
{
  _api.memmgr._m_alloc = Memory_alloc;
  _api.memmgr._m_calloc = Memory_calloc;
  _api.memmgr._m_realloc = Memory_realloc;
  _api.memmgr._m_free = Memory_free;

  _api.memmgr.cleanup = MemMgr_cleanup;
  _api.memmgr.registerCleanupFunc = MemMgr_registerCleanupFunc;
  _api.memmgr.unregisterCleanupFunc = MemMgr_unregisterCleanupFunc;

  if (FOG_DEBUG_MEMORY)
  {
    MemDebug_init();
  }
}

FOG_NO_EXPORT void MemMgr_fini(void)
{
  if (FOG_DEBUG_MEMORY)
  {
    MemDebug_fini();
  }
}

} // Fog namespace
