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
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadPool.h>

namespace Fog {

// ============================================================================
// [Fog::ThreadPool - Global]
// ============================================================================

static Static<ThreadPool> ThreadPool_oInstance;

// ============================================================================
// [Fog::ThreadPool - Helpers]
// ============================================================================

static bool ThreadPool_releaseThreadInternal(ThreadPool* self, Thread* thread, int workerId)
{
  ThreadPoolEntry* cur = self->_usedThreads;
  ThreadPoolEntry* prev = NULL;

  while (cur)
  {
    if (cur->thread == thread)
    {
      if (prev)
        prev->next = cur->next;
      else
        self->_usedThreads = cur->next;

      cur->next = self->_unusedThreads;
      if (workerId != -1) cur->workerId = workerId;
      self->_unusedThreads = cur;

      return true;
    }

    prev = cur;
    cur = cur->next;
  }

  return false;
}

static Thread* FOG_CDECL ThreadPool_createThread(ThreadPool* self)
{
  Thread* thread = fog_new Thread();

  if (FOG_IS_NULL(thread))
    return NULL;

  self->_numThreads++;
  return thread;
}

static void FOG_CDECL ThreadPool_releaseAllAvailable(ThreadPool* self)
{
  AutoLock locked(self->_lock);
  ThreadPoolEntry* cur = self->_unusedThreads;

  while (cur)
  {
    ThreadPoolEntry* next = cur->next;
    Thread* thread = cur->thread;

    MemMgr::free(cur);
    fog_delete(thread);

    cur = next;
    self->_numThreads--;
  }

  self->_unusedThreads = NULL;
}
// ============================================================================
// [Fog::ThreadPool - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ThreadPool_ctor(ThreadPool* self)
{
  self->_lock.init();

  self->_minThreads = 1;
  self->_maxThreads = 32;
  self->_numThreads = 0;
  self->_usedThreads = NULL;
  self->_unusedThreads = NULL;
}

static void FOG_CDECL ThreadPool_dtor(ThreadPool* self)
{
  self->_lock->lock();
  bool used = self->_usedThreads != NULL;
  self->_lock->unlock();

  if (used)
  {
    Debug::dbgFunc("Fog::ThreadPool", "~ThreadPool()", "Destroying instance, but some threads are still used, waiting...\n");
    while (self->_usedThreads != NULL)
      Thread::yield();
  }

  ThreadPool_releaseAllAvailable(self);
  self->_lock.destroy();
}

// ============================================================================
// [Fog::ThreadPool - Thread Management]
// ============================================================================

static err_t FOG_CDECL ThreadPool_getThread(ThreadPool* self, Thread** threads, int workerId)
{
  return self->getThreads(threads, 1, workerId);
}

static err_t FOG_CDECL ThreadPool_getThreads(ThreadPool* self, Thread** threads, size_t _count, int workerId)
{
  AutoLock locked(self->_lock);

  err_t err = ERR_OK;
  uint i;
  uint count = (uint)_count;

  for (i = 0; i < count; i++)
  {
    ThreadPoolEntry* pe = NULL;

    if (self->_unusedThreads)
    {
      // First try to find workerId if specified.
      if (workerId >= 0)
      {
        ThreadPoolEntry* cur = self->_unusedThreads;
        ThreadPoolEntry* prev = NULL;

        while (cur)
        {
          if (cur->workerId == workerId)
          {
            if (prev)
              prev->next = cur->next;
            else
              self->_unusedThreads = cur->next;
            pe = cur;
            break;
          }

          prev = cur;
          cur = cur->next;
        }
      }

      if (pe == NULL)
      {
        pe = self->_unusedThreads;
        pe->workerId = workerId;
        self->_unusedThreads = pe->next;
      }
    }
    else if (self->_numThreads < self->_maxThreads)
    {
      pe = (ThreadPoolEntry*)MemMgr::alloc(sizeof(ThreadPoolEntry));
      if (FOG_IS_NULL(pe)) return ERR_RT_OUT_OF_MEMORY;

      Thread* thread = ThreadPool_createThread(self);
      if (FOG_IS_NULL(thread))
      {
        MemMgr::free(pe);
        err = ERR_RT_OUT_OF_MEMORY;
        goto _Fail;
      }

      if (!thread->start(StringW::fromAscii8("Default")))
      {
        fog_delete(thread);
        MemMgr::free(pe);
        err = ERR_RT_OUT_OF_MEMORY;
        goto _Fail;
      }

      pe->thread = thread;
      pe->workerId = -1;
    }
    else
    {
      err = ERR_RT_OUT_OF_THREADS;
      goto _Fail;
    }

    pe->next = self->_usedThreads;
    self->_usedThreads = pe;

    if (workerId >= 0) workerId++;
    threads[i] = pe->thread;
  }
  return ERR_OK;

_Fail:
  if (i > 0)
  {
    do {
      i--;
      ThreadPool_releaseThreadInternal(self, threads[i], workerId == -1 ? -1 : workerId + i);
    } while (i > 0);
  }

  return err;
}

static err_t FOG_CDECL ThreadPool_releaseThread(ThreadPool* self, Thread* thread, int workerId)
{
  return self->releaseThreads(&thread, 1, workerId);
}

static err_t FOG_CDECL ThreadPool_releaseThreads(ThreadPool* self, Thread** threads, size_t count, int workerId)
{
  AutoLock locked(self->_lock);

  err_t err = ERR_OK;
  size_t i;

  for (i = 0; i < count; i++)
  {
    Thread* t = threads[i];

    if (ThreadPool_releaseThreadInternal(self, t, workerId))
    {
      threads[i] = NULL;
    }
    else
    {
      Debug::dbgFunc("Fog::ThreadPool", "releaseThreads",
        "Releasing thread (%p) that is not in a thread-pool list.\n", t);
      err = ERR_RT_INVALID_ARGUMENT;
    }
  }

  return err;
}

static int ThreadPool_getMinThreads(const ThreadPool* self)
{
  AutoLock locked(self->_lock);
  return self->_minThreads;
}

static int ThreadPool_getMaxThreads(const ThreadPool* self)
{
  AutoLock locked(self->_lock);
  return self->_maxThreads;
}

static int ThreadPool_getNumThreads(const ThreadPool* self)
{
  AutoLock locked(self->_lock);
  return self->_numThreads;
}

static err_t ThreadPool_setMaxThreads(ThreadPool* self, int maxThreads)
{
  AutoLock locked(self->_lock);

  self->_maxThreads = maxThreads;
  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ThreadPool_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.threadpool_ctor = ThreadPool_ctor;
  fog_api.threadpool_dtor = ThreadPool_dtor;

  fog_api.threadpool_getThread = ThreadPool_getThread;
  fog_api.threadpool_getThreads = ThreadPool_getThreads;

  fog_api.threadpool_releaseThread = ThreadPool_releaseThread;
  fog_api.threadpool_releaseThreads = ThreadPool_releaseThreads;

  fog_api.threadpool_getMinThreads = ThreadPool_getMinThreads;
  fog_api.threadpool_getMaxThreads = ThreadPool_getMaxThreads;
  fog_api.threadpool_getNumThreads = ThreadPool_getNumThreads;
  fog_api.threadpool_setMaxThreads = ThreadPool_setMaxThreads;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  fog_api.threadpool_oInstance = ThreadPool_oInstance.init();
}

FOG_NO_EXPORT void ThreadPool_fini(void)
{
  ThreadPool_oInstance.destroy();
}

} // Fog namespace
