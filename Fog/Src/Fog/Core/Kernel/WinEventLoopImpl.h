// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_WINEVENTLOOPIMPL_H
#define _FOG_CORE_KERNEL_WINEVENTLOOPIMPL_H

// [Dependencies]
#include <Fog/Core/Kernel/EventLoopImpl.h>

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::WinEventLoopImpl]
// ============================================================================

//! @brief Windows message event loop implementation.
//!
//! It contains a nearly infinite loop that peeks out messages, and then
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
struct FOG_API WinEventLoopImpl : public EventLoopImpl
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinEventLoopImpl();
  virtual ~WinEventLoopImpl();

  // --------------------------------------------------------------------------
  // [Run / Quit]
  // --------------------------------------------------------------------------

  virtual err_t runInternal();

  // --------------------------------------------------------------------------
  // [ScheduleWork]
  // --------------------------------------------------------------------------

  virtual void scheduleWork();
  virtual void scheduleDelayedWork(const Time& delayedWorkTime);

  // --------------------------------------------------------------------------
  // [WaitForWork]
  // --------------------------------------------------------------------------

  virtual void waitForWork();

  // --------------------------------------------------------------------------
  // [Windows Specific]
  // --------------------------------------------------------------------------

  void initMessageWnd();
  void destroyMessageWnd();

  void handleWorkMessage();
  void handleTimerMessage();

  bool processNextWindowsMessage();
  bool processMessageHelper(const MSG& msg);
  bool processPumpReplacementMessage();

  //! @brief Applications can call this to encourage us to process all pending
  //! WM_PAINT messages. This method will process all paint messages the
  //! Windows Message queue can provide, up to some fixed number (to avoid any
  //! infinite loops).
  void pumpOutPendingPaintMessages();

  int getCurrentDelay() const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief A hidden message-only window.
  HWND _messageHwnd;

  //! @brief The time at which delayed work should run.
  Time _delayedWorkTime;

  // TODO: kMsgDoWork renamed
  //! @brief A boolean value used to indicate if there is a kMsgDoWork message
  //! pending in the Windows Message queue.  There is at most one such message,
  //! and it can drive execution of tasks when a native message pump is running.
  LONG _haveWork;
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_KERNEL_WINEVENTLOOPIMPL_H
