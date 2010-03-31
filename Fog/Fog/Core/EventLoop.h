// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_EVENTLOOP_H
#define _FOG_CORE_EVENTLOOP_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Core/ListenerList.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Task.h>
#include <Fog/Core/ThreadEvent.h>
#include <Fog/Core/Time.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct EventLoopAutoRunState;
struct EventLoopPendingTask;

}

FOG_DECLARE_TYPEINFO(Fog::EventLoopPendingTask, Fog::TYPEINFO_PRIMITIVE)

namespace Fog {

// ============================================================================
// [Fog::DelayedPendingTask]
// ============================================================================

//! @brief Pending task used inside @c EventLoop.
struct FOG_HIDDEN EventLoopPendingTask
{
  FOG_INLINE EventLoopPendingTask(Task* task, bool nestable)
  {
    setTask(task, nestable);
  }

  //! @brief Get task.
  FOG_INLINE Task* getTask() const { return (Task*)((sysuint_t)_task & ~(sysuint_t)1); }
  //! @brief Get whether task is nestable.
  FOG_INLINE bool isNestable() const { return ((sysuint_t)_task & 1) == 1; }
  //! @brief Get dispatch time (can be null if it's not delayed task).
  FOG_INLINE const Time& getTime() const { return _time; }

  //! @brief Set task and nestable flag.
  FOG_INLINE void setTask(Task* task, bool nestable) { _task = (Task*)((sysuint_t)task | (sysuint_t)nestable); }

  //! @brief Set delayed time.
  FOG_INLINE void setTime(const Time& time) { _time = time; }

private:
  //! @brief Task combined with nestable flag (0x1 bit).
  Task* _task;
  //! @brief Delayed time.
  Time _time;
};

// ============================================================================
// [Fog::NativeEventListener]
// ============================================================================

//! @brief Native event loop event listener. Override this class to create your
//! own one.
struct FOG_API NativeEventListener
{
  NativeEventListener();
  virtual ~NativeEventListener();

  //! This method is called before processing an event.
  virtual void onBeforeDispatch(const void* ev) = 0;

  //! This method is called after processing an event.
  virtual void onAfterDispatch(const void* ev) = 0;
};

// ============================================================================
// [Fog::EventLoop]
// ============================================================================

//! A EventLoop is used to process events for a particular thread. There is at
//! at most one @c EventLoop instance per thread.
//!
//! Events include at a minimum @c Task instances submitted to postTask or those
//! managed by @c Timer. Depending on the type of event loop other events such
//! as UI messages may be processed. On Windows APC calls (as time permits) and
//! signals sent to a registered set of HANDLEs may also be processed.
//!
//! Unless otherwise specified, a @c EventLoop's methods may only be called on
//! the thread where the @c EventLoop's run method executes (this is owner thread).
//!
//! Please be SURE your task is reentrant (nestable) and all global variables
//! are stable and accessible.
struct FOG_API EventLoop
{
  EventLoop(const String& type);
  virtual ~EventLoop();

  //! @brief Add a native event listener, which will start receiving
  //! notifications immediately.
  bool addNativeEventListener(NativeEventListener* listener);

  //! @brief Remove a native event listener. It is safe to call this
  //! method while an observer is receiving a notification callback.
  bool removeNativeEventListener(NativeEventListener* listener);

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
  //! for (;;) {
  //!   didWork |= _doWork();
  //!   if (_quitting) break;
  //!
  //!   didWork |= _doDelayedWork();
  //!   if (_quitting) break;
  //!
  //!   if (didWork) continue;
  //!
  //!   didWork = _doIdleWork();
  //!   if (_quitting) break;
  //!
  //!   if (didWork) continue;
  //!
  //!   _waitForWork();
  //! }
  //! @endverbatim
  //!
  //! Here, @c _doInternalWork() is some private method of the event loop that is
  //! responsible for dispatching the next native event (this can be UI event,
  //! IO event, etc). @c _waitForWork() is a protected method that simply blocks
  //! until there is more work of any type to do.
  //!
  //! Notice that the run loop cycles between calling @c _doInternalWork(),
  //! @c _doWork(), and @c _doDelayedWork() methods. This helps ensure that
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
  //! the callouts (@c _scheduleWork() and @c scheduleDelayedWork()) MUST still
  //! be provided even in nested sub-loops that are "seemingly" outside the
  //! control of this event loop. @c _doWork() in particular must never be
  //! starved for time slices unless it returns false (meaning it has run out
  //! of things to do).
  virtual void run();

  //! Process all pending tasks, windows messages, etc., but don't wait/sleep.
  //! Return as soon as all items that can be run are taken care of.
  virtual void runAllPending();

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
  virtual void quit();

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
  //! on the thread that executes @c run() (event loop owner thread).
  void postTask(Task* task, bool nestable = true, uint32_t delay = 0);

  //! @brief Get event loop type.
  FOG_INLINE const String& getType() const { return _type; }

  //! @brief Get event loop depth (how many nested event loops started).
  FOG_INLINE int getDepth() const { return _depth; }

protected:
  // Run a _workQueue task or @a task, and delete it (if it was processed by
  // @c postTask()). If there are queued tasks, the oldest one is executed and
  // @a task is queued. @a task is optional and can be NULL. In this NULL case,
  // the method will run one pending task (if any exist). Returns true if it
  // executes a task. Queued tasks accumulate only when there is a non-nestable
  // task currently processing, in which case the @a task is appended to the
  // list _workQueue. Such re-entrancy generally happens when an unrequested
  // event loop (typical of a native dialog) is executing in the context of a
  // task.
  // bool _queueOrRunTask(Task* task);

  //! @brief Runs the specified task and deletes it.
  void _runTask(Task* task);

  //! @brief Calls RunTask or queues the pending_task on the deferred task list
  //! if it cannot be run right now. Returns true if the task was run.
  bool _deferOrRunPendingTask(const EventLoopPendingTask& pendingTask);

  //! @brief Adds the pending task to _delayedWorkQueue.
  //!
  //! This method will return @c true if it's needed to re-schedule internal
  //! timer.
  bool _addToDelayedWorkQueue(const EventLoopPendingTask& pendingTask);

  //! @brief Load tasks from the _incomingQueue into _workQueue if the latter is
  //! empty. The former requires a lock to access, while the latter is directly
  //! accessible on this thread.
  void _reloadWorkQueue();

  //! @brief Delete tasks that haven't run yet without running them. Used in the
  //! destructor to make sure all the task's destructors get called. Returns
  //! true if some work was done.
  bool _deletePendingTasks();

  //! @brief Post a task to our incomming queue.
  void _postTask_Helper(Task* task, uint32_t delay);

  //! A surrounding stack frame around the running of the event loop that
  //! supports all saving and restoring of state, as is needed for any/all (ugly)
  //! recursive calls.
  virtual void _runInternal() = 0;

  //! @brief Called from within @c run() in response to @c _scheduleWork() or when
  //! the event loop would otherwise call @c _doDelayedWork(). Returns true
  //! to indicate that work was done. @c _doDelayedWork() will not be called
  //! if @a _doWork() returns true.
  //!
  //! This means that priority of timers is lower than priority of tasks.
  virtual bool _doWork();

  //! @brief Called from within @c run() in response to @c _scheduleDelayedWork()
  //! or when the event loop would otherwise sleep waiting for more work.
  //!
  //! Returns true to indicate that delayed work was done. _doIdleWork() will
  //! not be called if _doDelayedWork() returns true. Upon return
  //! @a nextDelayedWorkTime indicates the time when _doDelayedWork() should be
  //! called again. If @a nextDelayedWorkTime is null, see @c Time::isNull(),
  //! then the queue of future delayed work (timer events) is currently empty,
  //! and no additional calls to this function need to be scheduled.
  virtual bool _doDelayedWork(Time* nextDelayedWorkTime);

  //! @brief Called from within run() just before the event loop goes to sleep.
  //! Returns true to indicate that idle work was done.
  virtual bool _doIdleWork();

  //! @brief Schedule a @c _doWork() callback to happen reasonably soon. Does
  //! nothing if a @c _doWork() callback is already scheduled. This method may
  //! be called from any thread. Once this call is made, @c _doWork() should
  //! not be "starved" at least until it returns a value of false.
  virtual void _scheduleWork() = 0;

  //! @brief Schedule a @c_ doDelayedWork() callback to happen at the specified
  //! time, cancelling any pending @c _doDelayedWork() callback. This method may
  //! only be used on the thread that called @c _run().
  virtual void _scheduleDelayedWork(const Time& delayedWorkTime) = 0;

  virtual void _destroyed();

  //! @brief Event loop type.
  String _type;

  //! @brief List of observers.
  ListenerList<NativeEventListener> _nativeEventListenerList;

  //! @brief Current depth (nested @c run() calls).
  int _depth;

  //! @brief Quitting?
  int _quitting;

  //! @brief _destroyed() method was called. There is assertion in ~EventLoop()
  //! so this must be set to true before EventLoop::~EventLoop() is called.
  int _destroyedCalled;

  // These are accessed only by event loop owner's thread.

  //! @brief Current work queue (may also contain delayed tasks).
  List<EventLoopPendingTask> _workQueue;
  //! @brief Current delayed work queue (parsed originally from @c _workQueue).
  List<EventLoopPendingTask> _delayedWorkQueue;
  //! @brief Current deferred work queue (tasks that will be called by
  //! non-nested event loop).
  List<EventLoopPendingTask> _deferredWorkQueue;

  //! @brief Protect access to _incomingQueue and _incomingDelayedQueue.
  Lock _incomingLock;

  //! @brief List which creates an incomingQueue of tasks that are aquired under
  //! a mutex for processing on this instance's thread. These tasks have not yet
  //! been sorted out into items for our _workQueue vs items that will be
  //! handled by the timer manager.
  List<EventLoopPendingTask> _incomingQueue;

private:
  friend struct EventLoopAutoRunState;

  FOG_DISABLE_COPY(EventLoop)
};

// ============================================================================
// [Fog::DefaultEventLoop]
// ============================================================================

//! @brief Default event loop implementation that doesn't use native event loop
//! functions.
struct FOG_API DefaultEventLoop : public EventLoop
{
  DefaultEventLoop();
  virtual ~DefaultEventLoop();

protected:
  virtual void _runInternal();

  virtual void _scheduleWork();
  virtual void _scheduleDelayedWork(const Time& delayedWorkTime);

  //! @brief Used to sleep until there is more work to do.
  ThreadEvent _event;

  //! @brief The time at which we should call @c scheduleDelayedWork().
  Time _delayedWorkTime;

private:
  FOG_DISABLE_COPY(DefaultEventLoop)
};

// ============================================================================
// [Fog::WinEventLoop]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

//! @c WinEventLoop implements a "traditional" Windows message pump. It
//! contains a nearly infinite loop that peeks out messages, and then
//! dispatches them. Intermixed with those peeks are callouts to _doWork() for
//! pending tasks and _doDelayedWork() for pending timers. When there are no
//! events to be serviced, the event loop goes into a wait state. In most cases,
//! this message pump handles all processing.
//!
//! However, when a task, or windows event, invokes on the stack a native dialog
//! box or such, that window typically provides a bare bones (native?) message
//! loop. That bare-bones message loop generally supports little more than a
//! peek of the Windows message queue, followed by a dispatch of the peeked
//! message. EventLoop extends that bare-bones message pump to also service
//! tasks, at the cost of some complexity.
//!
//! The basic structure of the extension (refered to as a sub-loop) is that a
//! special message, EVENT_LOOP_WM_HAVE_WORK, is repeatedly injected into the
//! Windows Message queue. Each time the EVENT_LOOP_WM_HAVE_WORK message is 
//! peeked, checks are made for an extended set of events, including the 
//! availability of tasks to run.
//!
//! After running a task, the special message EVENT_LOOP_WM_HAVE_WORK is again
//! posted to the Window's Message queue, ensuring a future time slice for
//! processing a future event. To prevent flooding the Window's Message queue,
//! care is taken to be sure that at most one EVENT_LOOP_WM_HAVE_WORK message
//! is evet pending in the Window's Message queue.
//!
//! There are a few additional complexities in this system where, when there
//! are no tasks to run, this otherwise infinite stream of messages which drives
//! the sub-loop is halted. The loop is automatically re-started when tasks are
//! queued.
//!
//! A second complexity is that the presence of this stream of posted tasks may
//! prevent a bare-bones message loop from ever peeking a WM_PAINT or WM_TIMER.
//! Such paint and timer events always give priority to a posted message, such
//! as EVENT_LOOP_WM_HAVE_WORK messages. As a result, care is taken to do some
//! peeking in between the posting of each EVENT_LOOP_WM_HAVE_WORK message
//! (i.e., after EVENT_LOOP_WM_HAVE_WORK is peeked, and before a replacement
//! EVENT_LOOP_WM_HAVE_WORK is posted).
//!
//! @note Although it may seem odd that messages are used to start and stop
//! this flow (as opposed to signaling objects, etc.), it should be understood
//! that the native message loop will only respond to messages. As a result, it
//! is an excellent choice. It is also helpful that the starter messages that
//! are placed in the queue when new task arrive also awakens _doRunLoop().
struct FOG_API WinEventLoop : public EventLoop
{
  WinEventLoop(const String& type);
  virtual ~WinEventLoop();

  //! @brief Applications can call this to encourage us to process all pending 
  //! WM_PAINT messages. This method will process all paint messages the 
  //! Windows Message queue can provide, up to some fixed number (to avoid any 
  //! infinite loops).
  void pumpOutPendingPaintMessages();

protected:
  static LRESULT CALLBACK WndProcThunk(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

  // EventLoop implementation methods.
  virtual void _runInternal();
  virtual void _scheduleWork();
  virtual void _scheduleDelayedWork(const Time& delayedWorkTime);

  virtual void _waitForWork();

  void _initMessageWnd();
  void _handleWorkMessage();
  void _handleTimerMessage();
  bool _processNextWindowsMessage();
  bool _processMessageHelper(const MSG& msg);
  bool _processPumpReplacementMessage();
  int _getCurrentDelay() const;

  //! @brief A hidden message-only window.
  HWND _messageHwnd;

  //! @brief The time at which delayed work should run.
  Time _delayedWorkTime;

  //! @brief A boolean value used to indicate if there is a kMsgDoWork message 
  //! pending in the Windows Message queue.  There is at most one such message,
  //! and it can drive execution of tasks when a native message pump is running.
  LONG _haveWork;
};

// ============================================================================
// [Fog::WinGuiEventLoop]
// ============================================================================

//! @brief Convenience class that sets event loop type to "Gui.Windows". This
//! event loop is also registered by application initializer.
struct FOG_API WinGuiEventLoop : public WinEventLoop
{
  WinGuiEventLoop();
};

#endif // FOG_OS_WINDOWS

} // Fog namespace

//! @}

#endif // _FOG_CORE_EVENTLOOP_H
