// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_THREADPOOL_H
#define _FOG_CORE_THREADING_THREADPOOL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Fog::ThreadPoolEntry]
// ============================================================================

//! @brief Allocated or pooled thread record.
struct FOG_NO_EXPORT ThreadPoolEntry
{
  ThreadPoolEntry* next;
  Thread* thread;
  int workerId;
};


// ============================================================================
// [Fog::ThreadPool]
// ============================================================================

//! @brief Thread pool can be used get/release threads in very efficient way.
//!
//! Instead of creating / destroying each thread the thread pool can store
//! all threads for their efficient reuse. Using thread pool significantly
//! reduces overhead caused by operating-system calls.
//!
//! Default thread pool used for example by:
//! - @c RasterPaintEngine.
//!
//! All methods in @c ThreadPool are thread safe.
struct FOG_NO_EXPORT ThreadPool
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create thread pool.
  FOG_INLINE ThreadPool()
  {
    fog_api.threadpool_ctor(this);
  }

  //! @brief Destroy thread pool.
  FOG_INLINE ~ThreadPool()
  {
    fog_api.threadpool_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Thread Management]
  // --------------------------------------------------------------------------

  //! @brief Get thread optionally using @a workId parameter.
  //!
  //! Use @a workId parameter to specify the thread id used by your task. Work
  //! id represents usually the index of the thread in your thread list. For
  //! example if you have computation for 4 threads then work IDs can be
  //! [0, 1, 2, 3]. Thread allocator tries return thread where the id was last
  //! used. Using workId may or may not improve your code efficiency (if some
  //! data remains in cpu-cache and os-cpu scheduler can use the cpu again).
  FOG_INLINE err_t getThread(Thread** threads, int workerId = -1)
  {
    return fog_api.threadpool_getThread(this, threads, workerId);
  }

  //! @brief Get @a count of threads.
  FOG_INLINE err_t getThreads(Thread** threads, size_t count, int workerId = -1)
  {
    return fog_api.threadpool_getThreads(this, threads, count, workerId);
  }

  //! @brief Release @a thread previously returned by @c getThread().
  //!
  //! Use @a workId optionally to set thread workId (it not remembers the
  //! @a workId passed to the @c getThread() method).
  FOG_INLINE err_t releaseThread(Thread* thread, int workerId = -1)
  {
    return fog_api.threadpool_releaseThread(this, thread, workerId);
  }

  //! @brief Release @a count of threads.
  FOG_INLINE err_t releaseThreads(Thread** threads, size_t count, int workerId = -1)
  {
    return fog_api.threadpool_releaseThreads(this, threads, count, workerId);
  }

  //! @brief Get minimum count of threads that can be used by the thread pool.
  FOG_INLINE int getMinThreads() const
  {
    return fog_api.threadpool_getMinThreads(this);
  }

  //! @brief Get maximum count of threads that can be used by the thread pool.
  FOG_INLINE int getMaxThreads() const
  {
    return fog_api.threadpool_getMaxThreads(this);
  }

  //! @brief Get number of threads used by the thread pool.
  FOG_INLINE int getNumThreads() const
  {
    return fog_api.threadpool_getNumThreads(this);
  }

  //! @brief Set maximum threads that can be created and used by the thread
  //! pool. If all threads are used then @c getThread() or @c getThreads() will
  //! return @c NULL.
  FOG_INLINE err_t setMaxThreads(int maxThreads)
  {
    return fog_api.threadpool_setMaxThreads(this, maxThreads);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE ThreadPool* get()
  {
    return fog_api.threadpool_oInstance;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Lock to protect members.
  mutable Static<Lock> _lock;

  //! @brief List of used threads.
  ThreadPoolEntry* _usedThreads;
  //! @brief List of unused (pooled) threads.
  ThreadPoolEntry* _unusedThreads;

  //! @brief Minimum threads count.
  int _minThreads;
  //! @brief Maximum threads count.
  int _maxThreads;
  //! @brief Number of threads.
  int _numThreads;

private:
  FOG_NO_COPY(ThreadPool)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_THREADPOOL_H
