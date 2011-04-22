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
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryManager.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Threading/Lock.h>

#include <stdio.h>
#include <stdlib.h>

// ===========================================================================
// [Fog::MemMgr]
// ===========================================================================

#if !defined(FOG_USE_STDNEW)
FOG_CVAR_DECLARE __FogMemMgr fog_memmgr;
#endif // FOG_USE_STDNEW

namespace Fog {

// ===========================================================================
// [Fog::Memory - Alloc / Free]
// ===========================================================================

static void* FOG_CDECL _Core_Memory_alloc(sysuint_t size)
{
  void* addr = ::malloc(size);

  if (FOG_IS_NULL(addr) && size > 0)
  {
    Memory::cleanup();
    addr = ::malloc(size);
  }

  return addr;
}

static void* FOG_CDECL _Core_Memory_calloc(sysuint_t size)
{
  void* addr = ::calloc(size, 1);

  if (FOG_IS_NULL(addr) && size > 0)
  {
    Memory::cleanup();
    addr = ::calloc(size, 1);
  }

  return addr;
}

static void* FOG_CDECL _Core_Memory_realloc(void* addr, sysuint_t size)
{
  if (FOG_IS_NULL(addr)) { return Memory::alloc(size); }
  if (FOG_UNLIKELY(size == 0)) { Memory::free(addr); return NULL; }

  void* anew = ::realloc(addr, size);

  if (FOG_IS_NULL(anew))
  {
    Memory::cleanup();
    anew = ::realloc(addr, size);
  }

  return anew;
}

static void* FOG_CDECL _Core_Memory_reallocOrFree(void* addr, sysuint_t size)
{
  void* anew = Memory::realloc(addr, size);
  if (FOG_IS_NULL(anew) && size > 0) Memory::free(addr);
  return anew;
}

static void FOG_CDECL _Core_Memory_free(void* addr)
{
  ::free(addr);
}

// ============================================================================
// [Fog::Memory - Cleanup]
// ============================================================================

static void FOG_CDECL _Core_Memory_cleanup()
{
  // TODO:
}

static err_t FOG_CDECL _Core_Memory_registerCleanupHandler(void* handler, void* closure)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL _Core_Memory_unregisterCleanupHandler(void* handler, void* closure)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::Memory - Copy]
// ============================================================================

static void* FOG_CDECL _Core_Memory_zero(void* dst, sysuint_t size)
{
  return memset(dst, 0, size);
}

// ============================================================================
// [Fog::Memory - Xchg]
// ============================================================================

static void _Core_Memory_xchg(uint8_t* addr1, uint8_t* addr2, sysuint_t count)
{
  sysuint_t i;

  for (i = count / (sizeof(sysuint_t)); i; i--)
  {
    Memory::xchg_t<sysuint_t>((sysuint_t*)addr1, (sysuint_t*)addr2);

    addr1 += sizeof(sysuint_t);
    addr2 += sizeof(sysuint_t);
  }

  for (i = count & (sizeof(sysuint_t) - 1); i; i--)
  {
    Memory::xchg_1((void*)addr1, (void*)addr2);

    addr1++;
    addr2++;
  }
}

// ===========================================================================
// [Fog::Memory - Memory Manager]
// ===========================================================================

struct FOG_NO_EXPORT DefaultMemoryManager : public MemoryManager
{
  DefaultMemoryManager();

  virtual void* alloc(sysuint_t size, sysuint_t* allocated);
  virtual void free(void* ptr, sysuint_t size);
};

DefaultMemoryManager::DefaultMemoryManager()
{
}

void* DefaultMemoryManager::alloc(sysuint_t size, sysuint_t* allocated)
{
  void* ptr = Memory::alloc(size);
  if (allocated) *allocated = ptr ? size : 0;
  return ptr;
}

void DefaultMemoryManager::free(void* ptr, sysuint_t size)
{
  Memory::free(ptr);
}

static Static<DefaultMemoryManager> _core_default_memory_manager;
MemoryManager* Memory::getDefaultManager() { return _core_default_memory_manager.instancep(); }

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_memory_init(void)
{
  _core_default_memory_manager.init();

  _core.memory.alloc = _Core_Memory_alloc;
  _core.memory.calloc = _Core_Memory_calloc;
  _core.memory.realloc = _Core_Memory_realloc;
  _core.memory.reallocOrFree = _Core_Memory_reallocOrFree;
  _core.memory.free = _Core_Memory_free;

  _core.memory.cleanup = _Core_Memory_cleanup;
  _core.memory.registerCleanupHandler = _Core_Memory_registerCleanupHandler;
  _core.memory.unregisterCleanupHandler = _Core_Memory_unregisterCleanupHandler;

  _core.memory.copy = (_CoreApi::Memory_Copy)::memcpy;
  _core.memory.move = (_CoreApi::Memory_Move)::memmove;
  _core.memory.zero = (_CoreApi::Memory_Zero)_Core_Memory_zero;
  _core.memory.set = (_CoreApi::Memory_Set)::memset;

  _core.memory.copy_nt = _core.memory.copy;
  _core.memory.zero_nt = _core.memory.zero;
  _core.memory.set_nt = _core.memory.set;

  _core.memory.xchg = (_CoreApi::Memory_Xchg)_Core_Memory_xchg;
}

FOG_NO_EXPORT void _core_memory_fini(void)
{
  _core_default_memory_manager.destroy();
}

} // Fog namespace
