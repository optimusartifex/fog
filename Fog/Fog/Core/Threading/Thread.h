// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_THREAD_H
#define _FOG_CORE_THREADING_THREAD_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/Core/Tools/String.h>

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <pthread.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Fog::Thread]
// ============================================================================

//! @brief Thread.
struct FOG_API Thread
{
  // --------------------------------------------------------------------------
  // [Handle]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  //! @brief Thread system handle.
  typedef void* Handle;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  //! @brief Thread system handle.
  typedef pthread_t Handle;
#endif // FOG_OS_POSIX

  // --------------------------------------------------------------------------
  // [StartupData]
  // --------------------------------------------------------------------------

  //! @brief Thread startup-data.
  struct StartupData
  {
    //! @brief Event loop type (string).
    StringW eventLoopType;

    //! @brief Used to synchronize thread startup.
    ThreadEvent event;

    StartupData(const StringW& eventLoopType) :
      eventLoopType(eventLoopType),
      event(false, false)
    {
    }
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new @c Thread instance.
  //!
  //! @param name StringW to identify the thread (default "").
  Thread();

  //! @brief Destructor.
  virtual ~Thread();

  // --------------------------------------------------------------------------
  // [Accessors - Native]
  // --------------------------------------------------------------------------

  //! @brief Get the thread handle.
  FOG_INLINE Handle& getHandle()
  {
    return _handle;
  }

  //! @overload
  FOG_INLINE const Handle& getHandle() const
  {
    return _handle;
  }

  //! @brief Get the native thread id.
  FOG_INLINE uint32_t getId() const
  {
    return _id;
  }

  // TODO: Rename

  //! @brief Get whether the thread was started.
  FOG_INLINE bool isStarted() const
  {
    return _startupData != NULL;
  }

  // --------------------------------------------------------------------------
  // [Accessors - Stack Size]
  // --------------------------------------------------------------------------

  //! @brief Get thread stack size (0 means default).
  FOG_INLINE uint32_t getStackSize() const
  {
    return _stackSize;
  }

  //! @brief Set thread stack size (0 means default).
  //!
  //! @note This function can be only called before the thread is created,
  //! otherwise the @c ERR_INVALID_STATE is returned.
  err_t setStackSize(uint32_t stackSize);

  // --------------------------------------------------------------------------
  // [Accessors - Affinity]
  // --------------------------------------------------------------------------

  err_t setAffinity(int mask);
  err_t resetAffinity();

  // --------------------------------------------------------------------------
  // [Accessors - Event Loop]
  // --------------------------------------------------------------------------

  //! @brief Get the thread's event loop.
  FOG_INLINE EventLoop* getEventLoop() const
  {
    return _eventLoop;
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! Starts the thread.  Returns true if the thread was successfully started;
  //! otherwise, returns false.  Upon successful return, the eventLoop()
  //! getter will return non-null.
  //!
  //! Note: This function can't be called on Windows with the loader lock held;
  //! i.e. during a DllMain, global object construction or destruction, atexit()
  //! callback.
  virtual bool start(const StringW& eventLoopType);

  //! Signals the thread to exit and returns once the thread has exited.  After
  //! this method returns, the Thread object is completely reset and may be used
  //! as if it were newly constructed (i.e., Start may be called again).
  //!
  //! Stop may be called multiple times and is simply ignored if the thread is
  //! already stopped.
  //!
  //! NOTE: This method is optional.  It is not strictly necessary to call this
  //! method as the Thread's destructor will take care of stopping the thread if
  //! necessary.
  virtual void stop();

  //! Signals the thread to exit in the near future.
  //!
  //! WARNING: This function is not meant to be commonly used. Use at your own
  //! risk. Calling this function will cause eventLoop() to become invalid in
  //! the near future. This function was created to workaround a specific
  //! deadlock on Windows with printer worker thread. In any other case, stop()
  //! should be used.
  //!
  //! StopSoon should not be called multiple times as it is risky to do so. It
  //! could cause a timing issue in eventLoop() access. Call stop() to reset
  //! the thread object once it is known that the thread has quit.
  void stopSoon();

  //! Called just prior to starting the event loop.
  virtual void init();

  virtual void main();
  virtual void finished();

  //! Called just after the event loop ends.
  virtual void cleanUp();

  // --------------------------------------------------------------------------
  // [Statics - Yield / Sleep]
  // --------------------------------------------------------------------------

  //! @brief Yield the current thread.
  static void yield();

  //! @brief Sleeps for the specified duration (units are milliseconds).
  static void sleep(uint32_t ms);

  // --------------------------------------------------------------------------
  // [Statics - Current]
  // --------------------------------------------------------------------------

  //! @brief Get the current thread instance.
  static Thread* getCurrentThread();

  //! @brief Get the current thread id.
  static uint32_t getCurrentThreadId();

  // --------------------------------------------------------------------------
  // [Statics - Main]
  // --------------------------------------------------------------------------

  static Thread* _mainThread;
  static uint32_t _mainThreadId;

  //! @brief Get main thread instance.
  static FOG_INLINE Thread* getMainThread()
  {
    return _mainThread;
  }

  //! @brief Get main thread id.
  static FOG_INLINE uint32_t getMainThreadId()
  {
    return _mainThreadId;
  }

  //! @brief Get whether current thread is main.
  static FOG_INLINE bool isMainThread()
  {
    return getCurrentThreadId() == _mainThreadId;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Thread's handle.
  Handle _handle;

  //! @brief Thread id. Used for debugging purposes.
  uint32_t _id;

  //! @brief Stack size.
  uint32_t _stackSize;

  //! @brief Thread startup-data.
  StartupData* _startupData;

  //! @brief Thread's event-loop instance (can be @c NULL if no event loop
  //! was created).
  EventLoop* _eventLoop;

private:
  friend struct Application;
  friend struct MainThread;
  friend struct QuitTask;

  _FOG_NO_COPY(Thread)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_THREAD_H
