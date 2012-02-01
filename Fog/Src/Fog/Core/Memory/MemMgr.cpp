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
#include <Fog/Core/Threading/Lock.h>

// [Dependencies - C]
#include <stdio.h>
#include <stdlib.h>

// [Dependencies - POSIX]
#if defined(FOG_OS_POSIX)
# include <stdlib.h>
# include <unistd.h>
#endif // FOG_OS_POSIX

// [Dependencies - MAC]
#if defined(FOG_OS_MAC)
# include <mach/mach_host.h>
# include <mach/mach_init.h>
#endif // FOG_OS_MAC

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
    MemMgr::cleanup(MEMORY_CLEANUP_REASON_NO_MEMORY);
    p = ::malloc(size);
  }

  return p;
}

static void* FOG_CDECL Memory_calloc(size_t size)
{
  void* p = ::calloc(size, 1);

  if (FOG_IS_NULL(p) && size > 0)
  {
    MemMgr::cleanup(MEMORY_CLEANUP_REASON_NO_MEMORY);
    p = ::calloc(size, 1);
  }

  return p;
}

static void* FOG_CDECL Memory_realloc(void* p, size_t size)
{
  void* newp;

  if (FOG_IS_NULL(p))
  {
    return fog_api.memmgr_alloc(size);
  }

  if (FOG_UNLIKELY(size == 0))
  {
    fog_api.memmgr_free(p);
    return NULL;
  }

  newp = ::realloc(p, size);
  if (FOG_IS_NULL(newp))
  {
    MemMgr::cleanup(MEMORY_CLEANUP_REASON_NO_MEMORY);
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

struct FOG_NO_EXPORT MemCleanupItem
{
  MemCleanupItem* next;

  MemCleanupFunc func;
  void* closure;
};

struct FOG_NO_EXPORT MemMgrGlobal
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MemMgrGlobal() :
    first(NULL),
    iteratorNext(NULL),
    registerAfter(NULL),
    iterating(false)
  {
  }

  ~MemMgrGlobal()
  {
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Lock to protect MemMgr::cleanup(), registerCleanupHandler(), and
  //! unregisterCleanupHandler().
  Lock lock;

  //! @brief First cleanup handler in the list.
  MemCleanupItem* first;

  //! @brief Next cleanup handler for traversing.
  //!
  //! If the cleanup functions are currently iterated it's important to allow
  //! them to unregister themselves. So the iterator stores each next handler
  //! into this member, thus MemMgr::unregisterCleanupFunc() can check for it
  //! and advance iterator if needed.
  MemCleanupItem* iteratorNext;

  //! @brief All cleanup handlers which was added during cleanup iteration.
  //! These are added to the global list after iteration finishes.
  MemCleanupItem* registerAfter;

  //! @brief True when iterating and running handlers.
  bool iterating;
};

static Static<MemMgrGlobal> MemMgr_global;

static void FOG_CDECL MemMgr_cleanup(uint32_t reason)
{
  // Synchronized section.
  { AutoLock locked(MemMgr_global->lock);

    MemMgr_global->iterating = true;

    MemCleanupItem* item = MemMgr_global->first;
    while (item)
    {
      MemMgr_global->iteratorNext = item->next;

      MemCleanupFunc func = item->func;
      void* closure = item->closure;

      // Release the lock and call the handler.
      {
        AutoUnlock unlocked(MemMgr_global->lock);
        func(closure, reason);
      }

      item = MemMgr_global->iteratorNext;
    }

    // If some handler(s) were added during iteration, we need to move them
    // from 'registerAfter' list into the global list.
    if (MemMgr_global->registerAfter != NULL)
    {
      MemCleanupItem** prev = &MemMgr_global->first;
      item = *prev;

      while (item)
      {
        prev = &item->next;
        item = *prev;
      }

      *prev = MemMgr_global->registerAfter;
      MemMgr_global->registerAfter = NULL;
    }

    MemMgr_global->iterating = false;
  }
}

static err_t FOG_CDECL MemMgr_registerCleanupFunc(MemCleanupFunc func, void* closure)
{
  // We need to allocate the memory before we enter the synchronized section,
  // because if we lock the lock and out-of-memory happen then cleanup handler
  // will be called which we need to avoid.
  MemCleanupItem* newItem = reinterpret_cast<MemCleanupItem*>(
    MemMgr::alloc(sizeof(MemCleanupItem)));

  if (FOG_IS_NULL(newItem))
    return ERR_RT_OUT_OF_MEMORY;

  // Synchronized section.
  { AutoLock locked(MemMgr_global->lock);

    MemCleanupItem** prev = &MemMgr_global->first;
    MemCleanupItem* item = *prev;

    while (item)
    {
      if (item->func == func && item->closure == closure)
        goto _HandlerAlreadyExists;

      prev = &item->next;
      item = *prev;
    }

    // We are iterating now! Try also 'registerAfter' list.
    if (MemMgr_global->iterating)
    {
      prev = &MemMgr_global->registerAfter;
      item = *prev;

      while (item)
      {
        if (item->func == func && item->closure == closure)
          goto _HandlerAlreadyExists;

        prev = &item->next;
        item = *prev;
      }
    }

    newItem->next = NULL;
    newItem->func = func;
    newItem->closure = closure;
    *prev = newItem;
  }
  return ERR_OK;

_HandlerAlreadyExists:
  MemMgr::free(newItem);
  return ERR_RT_OBJECT_ALREADY_EXISTS;
}

static err_t FOG_CDECL MemMgr_unregisterCleanupFunc(MemCleanupFunc func, void* closure)
{
  MemCleanupItem* item = NULL;

  // Synchronized section.
  { AutoLock locked(MemMgr_global->lock);

    MemCleanupItem** prev = &MemMgr_global->first;

    item = *prev;
    while (item)
    {
      if (item->func == func && item->closure == closure)
      {
        *prev = item->next;
        break;
      }

      prev = &item->next;
      item = *prev;
    }

    // We are iterating now! Check also 'registerAfter' list. It's unprobable
    // to unregistered freshy registered handler, but it's possible and not
    // against the rules.
    if (MemMgr_global->iterating)
    {
      if (item == NULL)
      {
        prev = &MemMgr_global->first;

        item = *prev;
        while (item)
        {
          if (item->func == func && item->closure == closure)
          {
            *prev = item->next;
            break;
          }

          prev = &item->next;
          item = *prev;
        }
      }

      // Make sure that this item won't be used by iterator.
      if (item != NULL && MemMgr_global->iteratorNext == item)
      {
        MemMgr_global->iteratorNext = item->next;
      }
    }
  }

  if (FOG_IS_NULL(item))
    return ERR_RT_OBJECT_NOT_FOUND;

  MemMgr::free(item);
  return ERR_OK;
}

// ===========================================================================
// [Fog::MemMgr - Physical Memory (Windows)]
// ===========================================================================

#if defined(FOG_OS_WINDOWS)
static uint64_t FOG_CDECL MemMgr_getAmountOfPhysicalMemory(void)
{
  MEMORYSTATUSEX meminfo;
  meminfo.dwLength = sizeof(meminfo);

  if (!GlobalMemoryStatusEx(&meminfo))
    return 0;

  return (uint64_t)meminfo.ullTotalPhys;
}
#endif // FOG_OS_WINDOWS

// ===========================================================================
// [Fog::MemMgr - Physical Memory (Posix)]
// ===========================================================================

#if defined(FOG_OS_POSIX)
static uint64_t FOG_CDECL MemMgr_getAmountOfPhysicalMemory(void)
{
// _SC_PHYS_PAGES is not part of POSIX and not available on OS X.
#if defined(FOG_OS_MAC)
  struct host_basic_info hostinfo;
  mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;

  int result = host_info(mach_host_self(), HOST_BASIC_INFO,
    reinterpret_cast<host_info_t>(&hostinfo), &count);

  if (result != KERN_SUCCESS)
    return 0;
  if (count != HOST_BASIC_INFO_COUNT)
    return 0;

  return static_cast<uint64_t>(hostinfo.max_mem);
#else
  long pages = sysconf(_SC_PHYS_PAGES);
  long pageSize = sysconf(_SC_PAGE_SIZE);

  if (pages == -1 || pageSize == -1)
    return 0;

  return (uint64_t)pages * (ulong)pageSize;
#endif
}
#endif // FOG_OS_POSIX

// ===========================================================================
// [Fog::MemMgr - Physical Memory (Shared)]
// ===========================================================================

static uint32_t FOG_CDECL MemMgr_getAmountOfPhysicalMemoryMB(void)
{
  return (uint32_t)(MemMgr::getAmountOfPhysicalMemory() / 1048576);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemMgr_init(void)
{
  fog_api.memmgr_alloc = Memory_alloc;
  fog_api.memmgr_calloc = Memory_calloc;
  fog_api.memmgr_realloc = Memory_realloc;
  fog_api.memmgr_free = Memory_free;

  fog_api.memmgr_cleanup = MemMgr_cleanup;
  fog_api.memmgr_registerCleanupFunc = MemMgr_registerCleanupFunc;
  fog_api.memmgr_unregisterCleanupFunc = MemMgr_unregisterCleanupFunc;

  fog_api.memmgr_getAmountOfPhysicalMemory = MemMgr_getAmountOfPhysicalMemory;
  fog_api.memmgr_getAmountOfPhysicalMemoryMB = MemMgr_getAmountOfPhysicalMemoryMB;

  if (FOG_DEBUG_MEMORY)
    MemDebug_init();

  MemMgr_global.init();
}

FOG_NO_EXPORT void MemMgr_fini(void)
{
  MemMgr_global.destroy();

  if (FOG_DEBUG_MEMORY)
    MemDebug_fini();
}

} // Fog namespace
