// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_ALLOC_H
#define _FOG_CORE_MEMORY_ALLOC_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Api.h>

namespace Fog {
namespace Memory {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::Memory - Alloc / Free]
// ============================================================================

static FOG_INLINE void* alloc(size_t size)
{
  return _core.memory._m_alloc(size);
}

static FOG_INLINE void* calloc(size_t size)
{
  return _core.memory._m_calloc(size);
}

static FOG_INLINE void* realloc(void* addr, size_t size)
{
  return _core.memory._m_realloc(addr, size);
}

static FOG_INLINE void free(void* addr)
{
  return _core.memory._m_free(addr);
}

//! @}

} // Memory namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_ALLOC_H
