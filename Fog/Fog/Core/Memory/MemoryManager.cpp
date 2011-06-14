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
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Memory/MemoryManager.h>

namespace Fog {

// ===========================================================================
// [Fog::DefaultMemoryManager]
// ===========================================================================

struct FOG_NO_EXPORT DefaultMemoryManager : public MemoryManager
{
  FOG_INLINE DefaultMemoryManager() {}

  virtual void* alloc(size_t size, size_t* allocated);
  virtual void free(void* p, size_t size);
};

void* DefaultMemoryManager::alloc(size_t size, size_t* allocated)
{
  void* ptr = _core.memory._m_alloc(size);
  if (allocated) *allocated = ptr ? size : 0;
  return ptr;
}

void DefaultMemoryManager::free(void* p, size_t size)
{
  _core.memory._m_free(p);
}

static Static<DefaultMemoryManager> _MemoryManager_default;

// ============================================================================
// [Fog::MemoryManager]
// ============================================================================

MemoryManager::MemoryManager() {}
MemoryManager::~MemoryManager() {}

MemoryManager* MemoryManager::getDefault()
{
  return _MemoryManager_default.instancep();
}

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_memory_init_manager(void)
{
  _MemoryManager_default.init();
}

} // Fog namespace
