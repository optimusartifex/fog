// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_THREAD_H
#define _FOG_CORE_THREAD_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/EventLoop.h>
#include <Fog/Core/String.h>
#include <Fog/Core/ThreadEvent.h>

#if defined(FOG_OS_POSIX)
#include <pthread.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core
//! @{

// ============================================================================
// [FOG_THREAD_VAR]
// ============================================================================

//! @def FOG_THREAD_VAR
//! @brief Per thread variable declaration.
//!
//! Variable declared by @c FOG_THREAD_VAR uses specific compiler
//! keywords and must be always declared as static
//!
//! @note This specific keyword is not supported by all compilers and it's
//! very unportable to use it directly. See @c Fog::ThreadLocalStorage if
//! you want to use per thread variables.
#if defined(FOG_CC_GNU) && !defined(__MINGW32__)
#define FOG_THREAD_VAR __thread
#elif defined(FOG_CC_MSVC)
#define FOG_THREAD_VAR __declspec(thread)
#endif

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Application;

// ============================================================================
// [Fog::Thread]
// ============================================================================

//! A simple thread abstraction that establishes an @c EventLoop on a new thread.
//! The consumer uses the @c EventLoop of the thread to cause code to execute on
//! the thread. When this object is destroyed the thread is terminated. All
//! pending tasks queued on the thread's event loop will run to completion
//! before the thread is terminated.
//!
//! If you want to run backgroun thread with no event loop. Use @c "" type
//! in Thread constructor and override main() method.
struct FOG_API Thread
{
  // --------------------------------------------------------------------------
  // [Handle]
  // --------------------------------------------------------------------------

  // Fog::Thread::Handle should not be assumed to be a numeric type, 
  // since the standard intends to allow pthread_t to be a structure. This 
  // means you should not initialize it to a value, like 0.  If it's a member 
  // variable, the constructor can safely "value initialize" using () in the 
  // initializer list.

#if defined(FOG_OS_WINDOWS)
  //! @brief Thread system handle.
  typedef void* Handle;
#elif defined(FOG_OS_POSIX)
  //! @brief Thread system handle.
  typedef pthread_t Handle;
#endif

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Constructor.
  //! @param name String to identify the thread (default "").
  explicit Thread(const String& name = String());

  //! Destroy the thread, stopping (joining) it if necessary.
  //!
  //! NOTE: If you are subclassing from @c Thread, and you wish for your
  //! cleanUp method to be called, then you need to call stop() from your
  //! destructor.
  virtual ~Thread();

  // --------------------------------------------------------------------------
  // [Options]
  // --------------------------------------------------------------------------

  //! Specifies the maximum stack size that the thread is allowed to use.
  //! This does not necessarily correspond to the thread's initial stack size.
  //! A value of 0 indicates that the default maximum should be used.
  void setStackSize(sysuint_t ssize);

  //! @brief Get thread stack size (0 means default).
  FOG_INLINE sysuint_t getStackSize() const { return _stackSize; }

  err_t setAffinity(int mask);
  err_t resetAffinity();

  //! @brief Get thread flags
  FOG_INLINE uint32_t getFlags() const { return _flags; }

  //! @brief Get the native thread handle.
  FOG_INLINE Handle& getHandle() { return _handle; }
  //! @brief Get the native thread handle (const).
  FOG_INLINE const Handle& getHandle() const { return _handle; }

  //! @brief Get the native thread id.
  FOG_INLINE uint32_t getId() const { return _id; }

  //! @brief Get the name of this thread (for debugging for example).
  FOG_INLINE const String& getName() const { return _name; }

  //! @brief Return the event loop for this thread.
  //!
  //! Use the @c EventLoop::postTask() methods to execute code on the thread.
  //! This only returns non-null after a successful call to @c start(). After
  //! @c stop() has been called, this will return NULL.
  //!
  //! @note You must not call @c EventLoop::quit() method directly. Use
  //! the @c Thread::stop() method instead.
  FOG_INLINE EventLoop* getEventLoop() const { return _eventLoop; }

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
  virtual bool start(const String& eventLoopType);

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
  // [Static methods]
  // --------------------------------------------------------------------------

  //! @brief Gets the current thread id, which may be useful for logging purposes.
  static uint32_t _tid();

  //! @brief Yield the current thread so another thread can be scheduled.
  static void _yield();

  //! @brief Sleeps for the specified duration (units are milliseconds).
  static void _sleep(uint32_t ms);

  //! @brief Sets the thread name visible to a debugger. This has no effect otherwise.
  static void _setName(const String& name);

  //! @brief Creates a new thread.  The @c stackSize parameter can be 0 to
  //! indicate that the default stack size should be used. Upon success,
  //! @c *handle will be assigned a handle to the newly created thread, and
  //! @c d's main() method will be executed on the newly created thread.
  //!
  //! NOTE: When you are done with the thread handle, you must call join() to
  //! release system resources associated with the thread. You must ensure that
  //! the Delegate object outlives the thread.
  static bool _create(sysuint_t stackSize, Thread* thread);

  //! @brief Joins with a thread created via the create() function. This
  //! function blocks the caller until the designated thread exits. This
  //! will invalidate @c handle.
  static void _join(Thread* thread);

  // --------------------------------------------------------------------------
  // [Main thread]
  // --------------------------------------------------------------------------

  //! @brief Get main thread instance.
  static FOG_INLINE Thread* getMainThread() { return _mainThread; }

  //! @brief Get main thread id.
  static FOG_INLINE uint32_t getMainThreadId() { return _mainThreadId; }

  //! @brief Get whether current thread is main.
  static FOG_INLINE bool isMainThread() { return _tid() == _mainThreadId; }

  // --------------------------------------------------------------------------
  // [Current thread]
  // --------------------------------------------------------------------------

  //! @brief Get current thread.
  static Thread* getCurrent();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! We piggy-back on the _startupData member to know if we successfully
  //! started the thread.  This way we know that we need to call Join.
  FOG_INLINE bool _threadWasStarted() const { return _startupData != NULL; }

  //! The thread's handle.
  Handle _handle;

  //! Thread flags
  uint32_t _flags;

  //! Thread id. Used for debugging purposes.
  uint32_t _id;

  //! The name of the thread. Used for debugging purposes.
  String _name;

  //! Stack size.
  sysuint_t _stackSize;

  //! Used to pass data to threadMain. This structure is allocated on the stack
  //! from within start().
  struct StartupData
  {
    //! @brief Event loop type (string).
    String eventLoopType;

    //! @brief Used to synchronize thread startup.
    ThreadEvent event;

    StartupData(const String& eventLoopType) : 
      eventLoopType(eventLoopType),
      event(false, false)
    {
    }
  };

  StartupData* _startupData;

  //! The thread's event loop. Valid only while the thread is alive. Set by
  //! the created thread.
  EventLoop* _eventLoop;

  static Thread* _mainThread;
  static uint32_t _mainThreadId;

  friend struct QuitTask;
  friend struct MainThread;

private:
  friend struct Application;

  FOG_DISABLE_COPY(Thread)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_THREAD_H
