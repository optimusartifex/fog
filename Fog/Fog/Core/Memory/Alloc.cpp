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
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/Core/Memory/AllocDebug_p.h>
#include <Fog/Core/Memory/Cleanup.h>

// [Dependencies - C]
#include <stdio.h>
#include <stdlib.h>

namespace Fog {

// ===========================================================================
// [Configuration]
// ===========================================================================

#define FOG_DEBUG_MEMORY 0

// ===========================================================================
// [Fog::Memory - Alloc / Realloc / Free]
// ===========================================================================

static void* FOG_CDECL Memory_alloc(size_t size)
{
  void* p = ::malloc(size);

  if (FOG_IS_NULL(p) && size > 0)
  {
    Memory::cleanup();
    p = ::malloc(size);
  }

  return p;
}

static void* FOG_CDECL Memory_calloc(size_t size)
{
  void* p = ::calloc(size, 1);

  if (FOG_IS_NULL(p) && size > 0)
  {
    Memory::cleanup();
    p = ::calloc(size, 1);
  }

  return p;
}

static void* FOG_CDECL Memory_realloc(void* p, size_t size)
{
  void* newp;

  if (FOG_IS_NULL(p))
  {
    return _api.memory._m_alloc(size);
  }

  if (FOG_UNLIKELY(size == 0))
  {
    _api.memory._m_free(p);
    return NULL;
  }

  newp = ::realloc(p, size);
  if (FOG_IS_NULL(newp))
  {
    Memory::cleanup();
    newp = ::realloc(p, size);
  }

  return newp;
}

static void FOG_CDECL Memory_free(void* p)
{
  ::free(p);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemoryAlloc_init(void)
{
  _api.memory._m_alloc = Memory_alloc;
  _api.memory._m_calloc = Memory_calloc;
  _api.memory._m_realloc = Memory_realloc;
  _api.memory._m_free = Memory_free;

  if (FOG_DEBUG_MEMORY)
  {
    _core_memoryalloc_init_debug();
  }
}

FOG_NO_EXPORT void MemoryAlloc_fini(void)
{
  if (FOG_DEBUG_MEMORY)
  {
    _core_memoryalloc_fini_debug();
  }
}

} // Fog namespace
