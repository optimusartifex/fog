// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_THREADPOOL_H
#define _FOG_CORE_THREADING_THREADPOOL_H

// [Dependencies]
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Thread;

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
//! - @c RasterPainterEngine.
//!
//! All methods in @c ThreadPool are thread safe.
struct FOG_API ThreadPool
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create thread pool.
  ThreadPool();
  //! @brief Destroy thread pool.
  virtual ~ThreadPool();

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
  err_t getThread(Thread** threads, int workId = -1);
  //! @brief Get @a count of threads.
  err_t getThreads(Thread** threads, sysuint_t count, int workId = -1);

  //! @brief Release @a thread previously returned by @c getThread().
  //!
  //! Use @a workId optionally to set thread workId (it not remembers the
  //! @a workId passed to the @c getThread() method).
  err_t releaseThread(Thread* thread, int workId = -1);
  //! @brief Release @a count of threads.
  err_t releaseThreads(Thread** threads, sysuint_t count, int workId = -1);

  //! @brief Get minimum count of threads that can be used by the thread pool.
  int getMinThreads() const;
  //! @brief Get maximum count of threads that can be used by the thread pool.
  int getMaxThreads() const;
  //! @brief Get number of threads used by the thread pool.
  int getNumThreads() const;

  //! @brief Set maximum threads that can be created and used by the thread
  //! pool. If all threads are used then @c getThread() or @c getThreads() will
  //! return @c NULL.
  err_t setMaxThreads(int maxThreads);

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

  //! @brief Allocated or pooled thread record.
  struct PoolEntry
  {
    PoolEntry* next;
    Thread* thread;
    int workId;
  };

protected:
  //! @brief Create new thread using operating-system call.
  virtual Thread* _createThread();
  //! @brief Release all unused (pooled) threads.
  void releaseAllAvailable();

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------
public:

  static ThreadPool* getInstance();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Minimum threads count.
  int _minThreads;
  //! @brief Maximum threads count.
  int _maxThreads;
  //! @brief Number of threads.
  int _numThreads;

  //! @brief Lock used to access internal structures.
  Lock _lock;
  //! @brief List of used threads.
  PoolEntry* _usedThreads;
  //! @brief List of unused (pooled) threads.
  PoolEntry* _unusedThreads;

private:
  FOG_DISABLE_COPY(ThreadPool)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_THREADPOOL_H
