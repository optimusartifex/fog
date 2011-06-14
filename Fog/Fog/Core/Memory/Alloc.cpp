// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/Core/Memory/Cleanup.h>

// [Dependencies - C]
#include <stdio.h>
#include <stdlib.h>

namespace Fog {

// ===========================================================================
// [Fog::Memory - Alloc / Realloc / Free]
// ===========================================================================

static void* FOG_CDECL _Memory_alloc(size_t size)
{
  void* p = ::malloc(size);

  if (FOG_IS_NULL(p) && size > 0)
  {
    Memory::cleanup();
    p = ::malloc(size);
  }

  return p;
}

static void* FOG_CDECL _Memory_calloc(size_t size)
{
  void* p = ::calloc(size, 1);

  if (FOG_IS_NULL(p) && size > 0)
  {
    Memory::cleanup();
    p = ::calloc(size, 1);
  }

  return p;
}

static void* FOG_CDECL _Memory_realloc(void* p, size_t size)
{
  void* newp;

  if (FOG_IS_NULL(p))
  {
    return _core.memory._m_alloc(size);
  }

  if (FOG_UNLIKELY(size == 0))
  {
    _core.memory._m_free(p);
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

static void FOG_CDECL _Memory_free(void* p)
{
  ::free(p);
}

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_memory_init_alloc(void)
{
  _core.memory._m_alloc = _Memory_alloc;
  _core.memory._m_calloc = _Memory_calloc;
  _core.memory._m_realloc = _Memory_realloc;
  _core.memory._m_free = _Memory_free;
}

} // Fog namespace
