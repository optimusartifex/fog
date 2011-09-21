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
    MemMgr::cleanup(MEMMGR_CLEANUP_REASON_NO_MEMORY);
    p = ::malloc(size);
  }

  return p;
}

static void* FOG_CDECL Memory_calloc(size_t size)
{
  void* p = ::calloc(size, 1);

  if (FOG_IS_NULL(p) && size > 0)
  {
    MemMgr::cleanup(MEMMGR_CLEANUP_REASON_NO_MEMORY);
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
    MemMgr::cleanup(MEMMGR_CLEANUP_REASON_NO_MEMORY);
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
