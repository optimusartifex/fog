// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Thread.h>
#include <Fog/Core/ThreadPool.h>

namespace Fog {

// ============================================================================
// [Fog::ThreadPool]
// ============================================================================

static Static<ThreadPool> threadpool_global;

ThreadPool::ThreadPool() :
  // Safe defaults
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
      fog_debug("Fog::ThreadPool::~ThreadPool() - Destroying instance, but some "
                "threads are still used, waiting...");

      while (_usedThreads != NULL) Thread::_yield();
    }
  }

  releaseAllAvailable();
}

Thread* ThreadPool::getThread(int workId)
{
  AutoLock locked(_lock);
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
    if (!pe) return NULL;

    Thread* thread = _createThread();
    if (!thread)
    {
      Memory::free(pe);
      return NULL;
    }

    if (!thread->start(Ascii8("Default")))
    {
      fog_delete(thread);
      Memory::free(pe);
      return NULL;
    }

    pe->thread = thread;
    pe->workId = -1;
  }
  else
  {
    return NULL;
  }

  pe->next = _usedThreads;
  _usedThreads = pe;

  return pe->thread;
}

void ThreadPool::releaseThread(Thread* thread, int workId)
{
  AutoLock locked(_lock);

  PoolEntry* cur = _usedThreads;
  PoolEntry* prev = NULL;

  while (cur)
  {
    if (cur->thread == thread)
    {
      if (prev)
        prev->next = cur->next;
      else
        _usedThreads = cur->next;

      cur->next = _unusedThreads;
      if (workId != -1) cur->workId = workId;
      _unusedThreads = cur;

      return;
    }

    prev = cur;
    cur = cur->next;
  }

  fog_debug(
    "Fog::ThreadPool::releaseThread() - Releasing thread (%p) that is not in thread list", thread);
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

ThreadPool* ThreadPool::getInstance()
{
  return threadpool_global.instancep();
}

Thread* ThreadPool::_createThread()
{
  String threadName;
  threadName.format("Fog::ThreadPool #", _numThreads);

  Thread* thread = fog_new Thread(threadName);
  if (!thread) return NULL;

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

} // Fog namespace

FOG_INIT_DECLARE err_t fog_threadpool_init(void)
{
  using namespace Fog;

  // Init global thread pool instance.
  threadpool_global.init();

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_threadpool_shutdown(void)
{
  using namespace Fog;

  threadpool_global.destroy();
}
