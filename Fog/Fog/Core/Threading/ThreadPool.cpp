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
#include <Fog/Core/Global/Debug.h>
#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadPool.h>

namespace Fog {

// ============================================================================
// [Fog::ThreadPool - Helpers]
// ============================================================================

static bool ThreadPool_releaseThread(ThreadPool* self, Thread* thread, int workId)
{
  ThreadPool::PoolEntry* cur = self->_usedThreads;
  ThreadPool::PoolEntry* prev = NULL;

  while (cur)
  {
    if (cur->thread == thread)
    {
      if (prev)
        prev->next = cur->next;
      else
        self->_usedThreads = cur->next;

      cur->next = self->_unusedThreads;
      if (workId != -1) cur->workId = workId;
      self->_unusedThreads = cur;

      return true;
    }

    prev = cur;
    cur = cur->next;
  }

  return false;
}

// ============================================================================
// [Fog::ThreadPool - Local]
// ============================================================================

static Static<ThreadPool> _core_threadpool_global;

// ============================================================================
// [Fog::ThreadPool - Construction / Destruction]
// ============================================================================

ThreadPool::ThreadPool() :
  // Safe defaults...
  _minThreads(1),
  _maxThreads(32),
  _numThreads(0),
  _usedThreads(NULL),
  _unusedThreads(NULL)
{
}

ThreadPool::~ThreadPool()
{
  {
    bool used;
    _lock.lock();
    used = _usedThreads != NULL;
    _lock.unlock();

    if (used)
    {
      Debug::dbgFunc("Fog::ThreadPool", "~ThreadPool()", "Destroying instance, but some threads are still used, waiting...\n");
      while (_usedThreads != NULL) Thread::_yield();
    }
  }

  releaseAllAvailable();
}

// ============================================================================
// [Fog::ThreadPool - Thread Management]
// ============================================================================

err_t ThreadPool::getThread(Thread** threads, int workId)
{
  return getThreads(threads, 1, workId);
}

err_t ThreadPool::getThreads(Thread** threads, sysuint_t count, int workId)
{
  AutoLock locked(_lock);

  err_t err = ERR_OK;
  sysuint_t i;

  for (i = 0; i < count; i++)
  {
    PoolEntry* pe = NULL;

    if (_unusedThreads)
    {
      // First try to find workId if specified.
      if (workId >= 0)
      {
        PoolEntry* cur = _unusedThreads;
        PoolEntry* prev = NULL;
      
        while (cur)
        {
          if (cur->workId == workId)
          {
            if (prev)
              prev->next = cur->next;
            else
              _unusedThreads = cur->next;
            pe = cur;
            break;
          }

          prev = cur;
          cur = cur->next;
        }
      }
    
      if (pe == NULL)
      {
        pe = _unusedThreads;
        pe->workId = workId;
        _unusedThreads = pe->next;
      }
    }
    else if (_numThreads < _maxThreads)
    {
      pe = (PoolEntry*)Memory::alloc(sizeof(PoolEntry));
      if (FOG_IS_NULL(pe)) return ERR_RT_OUT_OF_MEMORY;

      Thread* thread = _createThread();
      if (FOG_IS_NULL(thread))
      {
        Memory::free(pe);
        err = ERR_RT_OUT_OF_MEMORY;
        goto _Fail;
      }

      if (!thread->start(Ascii8("Default")))
      {
        fog_delete(thread);
        Memory::free(pe);
        err = ERR_RT_OUT_OF_MEMORY;
        goto _Fail;
      }

      pe->thread = thread;
      pe->workId = -1;
    }
    else
    {
      err = ERR_RT_OUT_OF_THREADS;
      goto _Fail;
    }

    pe->next = _usedThreads;
    _usedThreads = pe;

    if (workId >= 0) workId++;
    threads[i] = pe->thread;
  }
  return ERR_OK;

_Fail:
  if (i > 0)
  {
    do {
      i--;
      ThreadPool_releaseThread(this, threads[i], workId == -1 ? -1 : workId + i);
    } while (i > 0);
  }

  return err;
}

err_t ThreadPool::releaseThread(Thread* thread, int workId)
{
  return releaseThreads(&thread, 1, workId);
}

err_t ThreadPool::releaseThreads(Thread** threads, sysuint_t count, int workId)
{
  AutoLock locked(_lock);

  err_t err = ERR_OK;
  sysuint_t i;

  for (i = 0; i < count; i++)
  {
    Thread* t = threads[i];
    if (ThreadPool_releaseThread(this, t, workId))
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

int ThreadPool::getMinThreads() const
{
  return _minThreads;
}

int ThreadPool::getMaxThreads() const
{
  return _maxThreads;
}

int ThreadPool::getNumThreads() const
{
  return _numThreads;
}

err_t ThreadPool::setMaxThreads(int maxThreads)
{
  _maxThreads = maxThreads;
  return ERR_OK;
}

// ============================================================================
// [Fog::ThreadPool - Statics]
// ============================================================================

ThreadPool* ThreadPool::getInstance()
{
  return _core_threadpool_global.instancep();
}

// ============================================================================
// [Fog::ThreadPool - Internal]
// ============================================================================

Thread* ThreadPool::_createThread()
{
  String threadName;
  threadName.format("Fog::ThreadPool #", _numThreads);

  Thread* thread = fog_new Thread(threadName);
  if (FOG_IS_NULL(thread)) return NULL;

  // Update statistics.
  _numThreads++;

  return thread;
}

void ThreadPool::releaseAllAvailable()
{
  AutoLock locked(_lock);

  PoolEntry* cur = _unusedThreads;
  while (cur)
  {
    PoolEntry* next = cur->next;
    Thread* thread = cur->thread;

    Memory::free(cur);
    fog_delete(thread);

    cur = next;
    _numThreads--;
  }

  _unusedThreads = NULL;
}

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_threadpool_init(void)
{
  _core_threadpool_global.init();
}

FOG_NO_EXPORT void _core_threadpool_fini(void)
{
  _core_threadpool_global.destroy();
}

} // Fog namespace
