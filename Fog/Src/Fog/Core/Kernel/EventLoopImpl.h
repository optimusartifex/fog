// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_KERNEL_EVENTLOOPIMPL_H
#define _FOG_CORE_KERNEL_EVENTLOOPIMPL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/EventLoopObserverList.h>
#include <Fog/Core/Kernel/Task.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Time.h>

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::EventLoopPendingTask]
// ============================================================================

//! @brief Pending task used inside @c EventLoop.
struct FOG_NO_EXPORT EventLoopPendingTask
{
  FOG_INLINE EventLoopPendingTask(Task* task, bool nestable)
  {
    setTask(task, nestable);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the pending task.
  FOG_INLINE Task* getTask() const
  {
#if FOG_ARCH_BITS >= 64
    return (Task*)((size_t)_task & ~(size_t)1);
#else
    return _task;
#endif
  }

  //! @brief Get whether task is nestable.
  FOG_INLINE bool isNestable() const
  {
#if FOG_ARCH_BITS >= 64
    return ((size_t)_task & 1) == 1; 
#else
    return _isNestable != 0;
#endif
  }

  //! @brief Set task and nestable flag.
  FOG_INLINE void setTask(Task* task, bool nestable)
  {
#if FOG_ARCH_BITS >= 64
    _task = (Task*)((size_t)task | (size_t)nestable);
#else
    _task = task;
    _isNestable = nestable;
#endif
  }

  //! @brief Get dispatch time (can be null if it's not delayed task).
  FOG_INLINE const Time& getTime() const
  {
    return _time;
  }

  //! @brief Set delayed time.
  FOG_INLINE void setTime(const Time& time)
  {
    _time = time;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // Make the structure 16-bytes long. If compiled for 64-bit CPU then nestable
  // flag is stored together with the pointer, because last three bits are 
  // always zero (alignment). If compiled for 32-bit then we have two 32-bit
  // values to use (pointer, and nestable value).

#if FOG_ARCH_BITS >= 64
  //! @brief Task combined with nestable flag (0x1 bit).
  Task* _task;
#else
  //! @brief Task.
  Task* _task;
  //! @brief Nestable flag.
  uint32_t _isNestable;
#endif // FOG_ARCH_BITS

  //! @brief Delayed time.
  Time _time;
};

// ============================================================================
// [Fog::EventLoopImpl]
// ============================================================================

struct FOG_API EventLoopImpl
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  EventLoopImpl(const StringW& type);
  virtual ~EventLoopImpl();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  virtual EventLoopImpl* addRef() const;
  virtual void release();

  //! @brief Called by @c release().
  virtual void destroy();

  // --------------------------------------------------------------------------
  // [Observable]
  // --------------------------------------------------------------------------

  //! @brief Add an event observer @a obj to observe native event dispatching.
  //!
  //! @note Default implementation returns @c ERR_RT_NOT_IMPLEMENTED.
  virtual err_t addObserver(EventLoopObserver* obj);

  //! @brief Remove the event observer @a obj.
  //!
  //! @note Default implementation returns @c ERR_RT_NOT_IMPLEMENTED.
  virtual err_t removeObserver(EventLoopObserver* obj);

  // --------------------------------------------------------------------------
  // [Run / Quit]
  // --------------------------------------------------------------------------

  //! @brief The @c run() method is called to enter the event loop's run loop.
  //!
  //! Within the method, the event loop is responsible for processing native
  //! events as well as for giving cycles to the tasks periodically. The event
  //! loop should take care to mix own events with native event processing
  //! so neither type of event starves the other of cycles.
  //!
  //! The anatomy of a typical run loop:
  //!
  //! @verbatim
  //! for (;;)
  //! {
  //!   didWork |= doWork();
  //!   if (quitting) \
  //!     break;
  //!
  //!   didWork |= doDelayedWork();
  //!   if (quitting) \
  //!     break;
  //!
  //!   if (didWork)
  //!     continue;
  //!
  //!   didWork = doIdleWork();
  //!   if (quitting)
  //!     break;
  //!
  //!   if (didWork)
  //!     continue;
  //!
  //!   waitForWork();
  //! }
  //! @endverbatim
  //!
  //! Here, @c doInternalWork() is some private method of the event loop that is
  //! responsible for dispatching the next native event (this can be UI event,
  //! IO event, etc). @c waitForWork() is a protected method that simply blocks
  //! until there is more work of any type to do.
  //!
  //! Notice that the run loop cycles between calling @c doInternalWork(),
  //! @c doWork(), and @c doDelayedWork() methods. This helps ensure that
  //! neither work queue starves the other. This is important for event loops
  //! that are used to drive animations, for example.
  //!
  //! Notice also that after each callout to foreign code, the run loop checks
  //! to see if it should quit. The quit() method is responsible for setting
  //! this flag. No further work is done once the quit flag is set.
  //!
  //! @note Care must be taken to handle @c run() being called again from within
  //! any of the callouts to foreign code. Native event loops may also need to
  //! deal with other native event loops being run outside their control
  //! (e.g., the MessageBox API on Windows pumps UI messages!). To be specific,
  //! the callouts (@c scheduleWork() and @c scheduleDelayedWork()) MUST still
  //! be provided even in nested sub-loops that are "seemingly" outside the
  //! control of this event loop. @c doWork() in particular must never be
  //! starved for time slices unless it returns false (meaning it has run out
  //! of things to do).
  virtual err_t run();

  //! Process all pending tasks, windows messages, etc., but don't wait/sleep.
  //! Return as soon as all items that can be run are taken care of.
  virtual err_t runAllPending();

  //! A surrounding stack frame around the running of the event loop that
  //! supports all saving and restoring of state, as is needed for any/all (ugly)
  //! recursive calls.
  virtual err_t runInternal() = 0;

  //! @brief Quit immediately from the most recently entered run loop.
  //!
  //! Signals the @c run() method to return after it is done processing all
  //! pending events. This method may only be called on the same thread that
  //! called @c run(), and @c run() must still be on the call stack.
  //!
  //! Use @c QuitTask() if you need to quit another thread's @c EventLoop, but
  //! note that doing so is fairly dangerous if the target thread makes nested
  //! calls to @c run(). The problem being that you won't know which nested
  //! run loop you are quiting, so be careful!
  virtual err_t quit();

  // --------------------------------------------------------------------------
  // [Post]
  // --------------------------------------------------------------------------

  //! The @c postTask() family of methods call the task's run() method
  //! asynchronously from within a event loop at some point in the future.
  //!
  //! With the @c postTask() variant, tasks are invoked in FIFO order,
  //! inter-mixed with normal UI or IO event processing. With the
  //! @c postTask() with delay larger than zero variant, tasks are called after
  //! at least approximately delay have elapsed.
  //!
  //! The EventLoop takes ownership of the @a task, and deletes it after it has
  //! been run.
  //!
  //! @note These methods may be called on any thread.  The Task will be invoked
  //! on the thread that executes @c run() (event loop home thread).
  virtual err_t postTask(Task* task, bool nestable = true, uint32_t delay = 0);

  //! @brief Post a task to our incomming queue.
  void postTask_Helper(Task* task, uint32_t delay);

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  // Run a workQueue task or @a task, and delete it (if it was processed by
  // @c postTask()). If there are queued tasks, the oldest one is executed and
  // @a task is queued. @a task is optional and can be NULL. In this NULL case,
  // the method will run one pending task (if any exist). Returns true if it
  // executes a task. Queued tasks accumulate only when there is a non-nestable
  // task currently processing, in which case the @a task is appended to the
  // list workQueue. Such re-entrancy generally happens when an unrequested
  // event loop (typical of a native dialog) is executing in the context of a
  // task.
  // bool queueOrRunTask(Task* task);

  //! @brief Runs the specified task and deletes it.
  void runTask(Task* task);

  //! @brief Calls RunTask or queues the pendingTask on the deferred task list
  //! if it cannot be run right now. Returns true if the task was run.
  bool deferOrRunPendingTask(const EventLoopPendingTask& pendingTask);

  //! @brief Adds the pending task to delayedWorkQueue.
  //!
  //! This method will return @c true if it's needed to re-schedule internal
  //! timer.
  bool addToDelayedWorkQueue(const EventLoopPendingTask& pendingTask);

  //! @brief Load tasks from the incomingQueue into workQueue if the latter is
  //! empty. The former requires a lock to access, while the latter is directly
  //! accessible on this thread.
  void reloadWorkQueue();

  //! @brief Delete tasks that haven't run yet without running them. Used in the
  //! destructor to make sure all the task's destructors get called. Returns
  //! true if some work was done.
  bool deletePendingTasks();

  // --------------------------------------------------------------------------
  // [DoWork]
  // --------------------------------------------------------------------------

  //! @brief Called from within @c run() in response to @c scheduleWork() or when
  //! the event loop would otherwise call @c doDelayedWork(). Returns true
  //! to indicate that work was done. @c doDelayedWork() will not be called
  //! if @a doWork() returns true.
  //!
  //! This means that priority of timers is lower than priority of tasks.
  virtual bool doWork();

  //! @brief Called from within @c run() in response to @c scheduleDelayedWork()
  //! or when the event loop would otherwise sleep waiting for more work.
  //!
  //! Returns true to indicate that delayed work was done. doIdleWork() will
  //! not be called if doDelayedWork() returns true. Upon return
  //! @a nextDelayedWorkTime indicates the time when doDelayedWork() should be
  //! called again. If @a nextDelayedWorkTime is null, see @c Time::isNull(),
  //! then the queue of future delayed work (timer events) is currently empty,
  //! and no additional calls to this function need to be scheduled.
  virtual bool doDelayedWork(Time* nextDelayedWorkTime);

  //! @brief Called from within run() just before the event loop goes to sleep.
  //! Returns true to indicate that idle work was done.
  virtual bool doIdleWork();

  // --------------------------------------------------------------------------
  // [ScheduleWork]
  // --------------------------------------------------------------------------

  //! @brief Schedule a @c doWork() callback to happen reasonably soon. Does
  //! nothing if a @c doWork() callback is already scheduled. This method may
  //! be called from any thread. Once this call is made, @c doWork() should
  //! not be "starved" at least until it returns a value of false.
  virtual void scheduleWork() = 0;

  //! @brief Schedule a @c doDelayedWork() callback to happen at the specified
  //! time, cancelling any pending @c doDelayedWork() callback. This method may
  //! only be used on the thread that called @c run().
  virtual void scheduleDelayedWork(const Time& delayedWorkTime) = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> _reference;

  //! @brief Event loop type.
  StringW _type;

  //! @brief Current event loop depth (nested @c run() calls).
  int _depth;

  //! @brief Whether this is a native event loop with observable event
  //! dispatching.
  uint8_t _isObservable;

  //! @brief destroy() method was called. There is assertion in ~EventLoop()
  //! so this must be set to true before EventLoop::~EventLoop() is called.
  uint8_t _isDestroyed;

  //! @brief Whether the event loop is at quitting stage (quit() was called).
  uint8_t _quitting;

  //! @brief Free to use for any @c EventLoopImpl implementation.
  uint8_t _flag_0;

  // These are accessed only by event loop's thread.

  //! @brief Current work queue (may also contain delayed tasks).
  List<EventLoopPendingTask> _workQueue;
  //! @brief Current delayed work queue (parsed originally from @c workQueue).
  List<EventLoopPendingTask> _delayedWorkQueue;
  //! @brief Current deferred work queue (tasks that will be called by
  //! non-nested event loop).
  List<EventLoopPendingTask> _deferredWorkQueue;

  //! @brief Protect access to incomingQueue and observerList.
  Lock lock;

  //! @brief List which creates an incomingQueue of tasks that are aquired under
  //! incomingLock for processing on this instance's thread. These tasks have not
  //! yet been sorted out into items for our workQueue vs items that will be
  //! handled by the timer manager.
  List<EventLoopPendingTask> _incomingQueue;

  //! @brief List of event observers.
  EventLoopObserverList<EventLoopObserver> _observerList;

private:
  FOG_NO_COPY(EventLoopImpl)
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_KERNEL_EVENTLOOPIMPL_H
