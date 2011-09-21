// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/Task.h>
#include <Fog/Core/OS/System.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/Core/Tools/Time.h>

#if defined(FOG_OS_WINDOWS)
# if !defined(USER_TIMER_MINIMUM)
#  define USER_TIMER_MINIMUM 0x0000000A
# endif // USER_TIMER_MINIMUM
# if !defined(USER_TIMER_MAXIMUM)
#  define USER_TIMER_MAXIMUM 0x7FFFFFFF
# endif // USER_TIMER_MAXIMUM
#endif // FOG_OS_WINDOWS

namespace Fog {

// ============================================================================
// [Fog::EventLoopAutoRunState]
// ============================================================================

struct FOG_NO_EXPORT EventLoopAutoRunState
{
  EventLoopAutoRunState(EventLoop* eventLoop) :
    _eventLoop(eventLoop)
  {
    _eventLoop->_depth++;
    _eventLoop->_quitting = false;
  }

  ~EventLoopAutoRunState()
  {
    if (--_eventLoop->_depth > 0)
    {
      _eventLoop->_quitting = false;
    }
  }

private:
  EventLoop* _eventLoop;
};

// ============================================================================
// [Fog::NativeEventListener]
// ============================================================================

NativeEventListener::NativeEventListener() {}
NativeEventListener::~NativeEventListener() {}

// ============================================================================
// [Fog::EventLoop]
// ============================================================================

EventLoop::EventLoop(const StringW& type) :
  _type(type),
  _depth(0),
  _quitting(0),
  _destroyedCalled(0)
{
}

EventLoop::~EventLoop()
{
  // You can't forget to call _destroyed() by upper class destructor.
  FOG_ASSERT(_destroyedCalled);
}

bool EventLoop::addNativeEventListener(NativeEventListener* listener)
{
  return _nativeEventListenerList.add(listener);
}

bool EventLoop::removeNativeEventListener(NativeEventListener* listener)
{
  return _nativeEventListenerList.remove(listener);
}

void EventLoop::run()
{
  EventLoopAutoRunState saveState(this);
  _runInternal();
}

void EventLoop::runAllPending()
{
  EventLoopAutoRunState saveState(this);

  // Means run until we would otherwise block.
  // FIXME:
  // _state->quitReceived = true;
  _runInternal();
}

void EventLoop::quit()
{
  _quitting = true;
}

void EventLoop::postTask(Task* task, bool nestable, uint32_t delay)
{
  bool wasEmpty;

  EventLoopPendingTask pendingTask(task, nestable);

  // Warning: Don't try to short-circuit, and handle this thread's tasks more
  // directly, as it could starve handling of foreign threads. Put every task
  // into this queue.
  if (delay > 0)
  {
    pendingTask.setTime(Time::now() + TimeDelta::fromMilliseconds(delay));
  }

  {
    AutoLock locked(_incomingLock);
    wasEmpty = _incomingQueue.isEmpty();
    _incomingQueue.append(pendingTask);
  }

  // Event loop can be over at this time.
  if (wasEmpty) _scheduleWork();
}

// TODO
//bool EventLoop::_queueOrRunTask(Task* task)
//{
//}

void EventLoop::_runTask(Task* task)
{
  task->run();
  if (task->destroyOnFinish()) task->destroy();
}

bool EventLoop::_deferOrRunPendingTask(const EventLoopPendingTask& pendingTask)
{
  if (_depth == 1 || pendingTask.isNestable())
  {
    _runTask(pendingTask.getTask());
    // Show that we ran a task (Note: a new one might arrive as a consequence!).
    return true;
  }

  // We couldn't run the task now because we're in a nested event loop and the
  // task isn't nestable. So add it to defferedWorkQueue so it can be called
  // later.
  _deferredWorkQueue.append(pendingTask);
  return false;
}

bool EventLoop::_addToDelayedWorkQueue(const EventLoopPendingTask& pendingTask)
{
  size_t i = 0, length = _delayedWorkQueue.getLength();
  const EventLoopPendingTask* tasks = _delayedWorkQueue.getData();

  // TODO: Use binary search.
  while (i < length)
  {
    if (tasks[i].getTime() > pendingTask.getTime()) break;
    i++;
  }

  // Move to the delayed work queue.
  _delayedWorkQueue.insert(i, pendingTask);

  return i == 0;
}

void EventLoop::_reloadWorkQueue()
{
  // We can improve performance of our loading tasks from _incomingQueue to
  // _workQueue by waiting until the last minute (_workQueue is empty) to
  // load. That reduces the number of locks-per-task significantly when our
  // queues get large.

  // Wait till we really need to lock and load.
  if (!_workQueue.isEmpty()) return;

  // Acquire all we can from the inter-thread queue with one lock acquisition.
  {
    AutoLock locked(_incomingLock);

    if (_incomingQueue.isEmpty()) return;
    swap(_incomingQueue, _workQueue);
  }
}

bool EventLoop::_deletePendingTasks()
{
  bool didWork = !_workQueue.isEmpty();
  while (!_workQueue.isEmpty())
  {
    EventLoopPendingTask pendingTask = _workQueue.getFirst();
    _workQueue.removeFirst();

    if (!pendingTask.getTime().isNull())
    {
      // We want to delete delayed tasks in the same order in which they would
      // normally be deleted in case of any funny dependencies between delayed
      // tasks.
      _addToDelayedWorkQueue(pendingTask);
    }
    else
    {
      Task* task = pendingTask.getTask();
      if (task->destroyOnFinish()) task->destroy();
    }
  }

  didWork |= !_deferredWorkQueue.isEmpty();
  while (!_deferredWorkQueue.isEmpty())
  {
    Task* task = _deferredWorkQueue.getFirst().getTask();
    _deferredWorkQueue.removeFirst();

    if (task->destroyOnFinish())
      task->destroy();
  }

  didWork |= !_delayedWorkQueue.isEmpty();
  while (!_delayedWorkQueue.isEmpty())
  {
    Task* task = _delayedWorkQueue.getFirst().getTask();
    _delayedWorkQueue.removeFirst();

    if (task->destroyOnFinish())
      task->destroy();
  }

  return didWork;
}

bool EventLoop::_doWork()
{
  for (;;)
  {
    _reloadWorkQueue();
    if (_workQueue.isEmpty()) break;

    // Execute oldest task.
    do {
      EventLoopPendingTask pendingTask = _workQueue.getFirst();
      _workQueue.removeFirst();

      if (!pendingTask.getTime().isNull())
      {
        if (_addToDelayedWorkQueue(pendingTask))
          _scheduleDelayedWork(pendingTask.getTime());
      }
      else
      {
        if (_deferOrRunPendingTask(pendingTask)) return true;
      }
    } while (!_workQueue.isEmpty());
  }

  // Nothing happened.
  return false;
}

bool EventLoop::_doDelayedWork(Time* nextDelayedWorkTime)
{
  if (_delayedWorkQueue.isEmpty())
  {
    *nextDelayedWorkTime = Time();
    return false;
  }

  Time t = _delayedWorkQueue.getAt(0).getTime();
  if (t > Time::now())
  {
    // Still not elapsed.
    *nextDelayedWorkTime = t;
    return false;
  }

  EventLoopPendingTask pendingTask = _delayedWorkQueue.getFirst();
  _delayedWorkQueue.removeFirst();

  if (!_delayedWorkQueue.isEmpty())
    *nextDelayedWorkTime = _delayedWorkQueue.getAt(0).getTime();

  return _deferOrRunPendingTask(pendingTask);
}

bool EventLoop::_doIdleWork()
{
  // TODO: What about this?
  // if (_processNextDelayedNonNestableTask()) return true;
  // TODO: Move this to the loop.
  //if (_state->quitReceived) _pump->quit();
  return false;
}

void EventLoop::_destroyed()
{
  // Prevent calling this twice or more.
  if (_destroyedCalled) return;

  // Clean up any unprocessed tasks, but take care: deleting a task could
  // result in the addition of more tasks. We set a limit on the number of
  // times we will allow a deleted task to generate more tasks. Normally,
  // we should only pass through this loop once or twice. If we end up
  // hitting the loop limit, then it is probably due to one task that is
  // being stubborn. Inspect the queues to see who is left.
  bool didWork;

  for (int i = 0; i < 100; i++)
  {
    _deletePendingTasks();
    _reloadWorkQueue();
    // If we end up with empty queues, then break out of the loop.
    didWork = _deletePendingTasks();
    if (!didWork) break;
  }

  if (didWork)
  {
    Debug::dbgFunc("Fog::EventLoop", "_destroyed", "Couldn't delete all tasks.\n");
  }

  _destroyedCalled = true;
}

// ============================================================================
// [Fog::DefaultEventLoop]
// ============================================================================

DefaultEventLoop::DefaultEventLoop() :
  EventLoop(StringW::fromAscii8("Default")),
  _event(false, false)
{
}

DefaultEventLoop::~DefaultEventLoop()
{
  _destroyed();
}

void DefaultEventLoop::_runInternal()
{
  for (;;)
  {
    bool didWork = _doWork();
    if (_quitting) break;

    didWork |= _doDelayedWork(&_delayedWorkTime);
    if (_quitting) break;

    if (didWork) continue;

    didWork = _doIdleWork();
    if (_quitting) break;

    if (didWork) continue;

    if (_delayedWorkTime.isNull())
    {
      _event.wait();
    }
    else
    {
      TimeDelta delay = _delayedWorkTime - Time::now();
      if (delay > TimeDelta())
        _event.timedWait(delay);
      else
        // It looks like _delayedWorkTime indicates a time in the past, so we
        // need to call _doDelayedWork() now.
        _delayedWorkTime = Time();
    }
    // Since _event is auto-reset, we don't need to do anything special here
    // other than service each delegate method.
  }
}

void DefaultEventLoop::_scheduleWork()
{
  // Since this can be called on any thread, we need to ensure that our run()
  // loop wakes up.
  _event.signal();
}

void DefaultEventLoop::_scheduleDelayedWork(const Time& delayedWorkTime)
{
  // We know that we can't be blocked on wait() right now since this method can
  // only be called on the same thread as run(), so we only need to update our
  // record of how long to sleep when we do sleep.
  _delayedWorkTime = delayedWorkTime;
}

#if defined(FOG_OS_WINDOWS)

// ============================================================================
// [Fog::WinEventLoop]
// ============================================================================

static const WCHAR EVENT_LOOP_WND_CLASS[] = L"Fog::WinEventLoop";

// Message sent to get an additional time slice for pumping (processing)
// another task (a series of such messages creates a continuous task pump).
static const int EVENT_LOOP_WM_HAVE_WORK = WM_USER + 1;

#if defined(FOG_DEBUG)
// Force exercise of polling model.
static const int EVENT_LOOP_MAX_WAIT_OBJECTS = 8;
#else
static const int EVENT_LOOP_MAX_WAIT_OBJECTS = MAXIMUM_WAIT_OBJECTS;
#endif

// ============================================================================
// [Fog::WinEventLoop]
// ============================================================================

WinEventLoop::WinEventLoop(const StringW& type) :
  EventLoop(type),
  _haveWork(0)
{
  _initMessageWnd();
}

WinEventLoop::~WinEventLoop()
{
  _destroyed();
  ::DestroyWindow(_messageHwnd);
}

void WinEventLoop::pumpOutPendingPaintMessages()
{
  // If we are being called outside of the context of run(), then don't try to
  // do any work.
  if (!_depth) return;

  // Create a mini-message-loop to force immediate processing of only Windows
  // WM_PAINT messages. Don't provide an infinite loop, but do enough peeking
  // to get the job done. Actual common max is 4 peeks, but we'll be a little
  // safe here.
  static const int PAINT_MAX_PEEK_COUNT = 20;
  bool win2k = System::getWindowsVersion() <= OS_WIN_VERSION_2000;
  int peekCount;
  MSG msg;

  for (peekCount = 0; peekCount < PAINT_MAX_PEEK_COUNT; peekCount++)
  {
    if (win2k)
    {
      if (!::PeekMessageW(&msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE)) break;
    }
    else
    {
      if (!::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE | PM_QS_PAINT)) break;
    }

    _processMessageHelper(msg);

    // Handle WM_QUIT.
    if (_quitting) break;
  }
}

// static
LRESULT CALLBACK WinEventLoop::WndProcThunk(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  switch (message)
  {
    case EVENT_LOOP_WM_HAVE_WORK:
      reinterpret_cast<WinEventLoop*>(wparam)->_handleWorkMessage();
      break;
    case WM_TIMER:
      reinterpret_cast<WinEventLoop*>(wparam)->_handleTimerMessage();
      break;
  }

  return ::DefWindowProcW(hwnd, message, wparam, lparam);
}

void WinEventLoop::_runInternal()
{
  // If this was just a simple PeekMessage() loop (servicing all possible work
  // queues), then Windows would try to achieve the following order according
  // to MSDN documentation about PeekMessage with no filter):
  //    * Sent messages
  //    * Posted messages
  //    * Sent messages (again)
  //    * WM_PAINT messages
  //    * WM_TIMER messages
  //
  // Summary: none of the above classes is starved, and sent messages has twice
  // the chance of being processed (i.e., reduced service time).

  for (;;)
  {
    // If we do any work, we may create more messages etc., and more work may
    // possibly be waiting in another task group.  When we (for example)
    // processNextWindowsMessage(), there is a good chance there are still more
    // messages waiting.  On the other hand, when any of these methods return
    // having done no work, then it is pretty unlikely that calling them again
    // quickly will find any work to do.  Finally, if they all say they had no
    // work, then it is a good time to consider sleeping (waiting) for more
    // work.

    bool didWork = _processNextWindowsMessage();
    if (_quitting) break;

    didWork |= _doWork();
    if (_quitting) break;

    didWork |= _doDelayedWork(&_delayedWorkTime);
    // If we did not process any delayed work, then we can assume that our
    // existing WM_TIMER if any will fire when delayed work should run. We
    // don't want to disturb that timer if it is already in flight. However,
    // if we did do all remaining delayed work, then lets kill the WM_TIMER.
    if (didWork && _delayedWorkTime.isNull())
    {
      ::KillTimer(_messageHwnd, reinterpret_cast<UINT_PTR>(this));
    }

    if (_quitting) break;
    if (didWork) continue;

    didWork = _doIdleWork();

    if (_quitting) break;
    if (didWork) continue;

    // Wait (sleep) until we have work to do again.
    _waitForWork();
  }
}

void WinEventLoop::_scheduleWork()
{
  // Someone else continued the pumping.
  if (InterlockedExchange(&_haveWork, 1)) return;

  // Make sure the EventLoop does some work for us.
  ::PostMessageW(_messageHwnd, EVENT_LOOP_WM_HAVE_WORK, reinterpret_cast<WPARAM>(this), 0);
}

void WinEventLoop::_scheduleDelayedWork(const Time& delayedWorkTime)
{
  // We would like to provide high resolution timers. Windows timers using
  // SetTimer() have a 10ms granularity. We have to use WM_TIMER as a wakeup
  // mechanism because the application can enter modal windows loops where it
  // is not running our EventLoop. The only way to have our timers fire in
  // these cases is to post messages there.
  //
  // To provide sub-10ms timers, we process timers directly from our run loop.
  // For the common case, timers will be processed there as the run loop does
  // its normal work. However, we also set the system timer so that WM_TIMER
  // events fire. This mops up the case of timers not being able to work in
  // modal message loops. It is possible for the SetTimer() to pop and have
  // no pending timers, because they could have already been processed by the
  // run loop itself.
  //
  // We use a single SetTimer() corresponding to the timer that will expire
  // soonest. As new timers are created and destroyed, we update SetTimer().
  // Getting a spurrious SetTimer() event firing is benign, as we'll just be
  // processing an empty timer queue.
  _delayedWorkTime = delayedWorkTime;

  int delay_msec = _getCurrentDelay();
  FOG_ASSERT(delay_msec >= 0);
  if (delay_msec < USER_TIMER_MINIMUM) delay_msec = USER_TIMER_MINIMUM;

  // Create a WM_TIMER event that will wake us up to check for any pending
  // timers (in case we are running within a nested event loop).
  ::SetTimer(_messageHwnd, reinterpret_cast<UINT_PTR>(this), delay_msec, NULL);
}

void WinEventLoop::_waitForWork()
{
  // Wait until a message is available, up to the time needed by the timer
  // manager to fire the next set of timers.
  int delay = _getCurrentDelay();

  // Negative value means no timers waiting.
  if (delay < 0) delay = INFINITE;

  DWORD result;
  result = ::MsgWaitForMultipleObjectsEx(0, NULL, delay, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

  if (result == WAIT_OBJECT_0)
  {
    // A WM_* message is available.
    //
    // If a parent child relationship exists between windows across threads
    // then their thread inputs are implicitly attached.
    // This causes the MsgWaitForMultipleObjectsEx API to return indicating
    // that messages are ready for processing (specifically mouse messages
    // intended for the child window. Occurs if the child window has capture)
    // The subsequent PeekMessage()'s call fails to return any messages thus
    // causing us to enter a tight loop at times.
    // The WaitMessage call below is a workaround to give the child window
    // sometime to process its input messages.

    // Here was bug that dalayed NC messages. Thank guys from #chromium-dev
    // for help with catching it. The problem is that QS_MOUSE will return
    // true also for NC messages, so there should be two PeekMessage() calls,
    // first for standard messages and second for NC messages.
    static const DWORD ncMouseFirst = WM_NCLBUTTONDOWN;
    static const DWORD ncMouseLast  = WM_NCMBUTTONDBLCLK;

    MSG msg = {0};
    DWORD queueStatus = GetQueueStatus(QS_MOUSE);

    if ((HIWORD(queueStatus) & QS_MOUSE) &&
        !::PeekMessageW(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE) &&
        !::PeekMessageW(&msg, NULL, ncMouseFirst , ncMouseLast , PM_NOREMOVE) )
    {
      WaitMessage();
    }

    return;
  }

  FOG_ASSERT(result != WAIT_FAILED);
}

void WinEventLoop::_initMessageWnd()
{
  HINSTANCE hInst = GetModuleHandle(NULL);

  WNDCLASSEXW wc;
  ZeroMemory(&wc, sizeof(WNDCLASSEXW));
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = WndProcThunk;
  wc.hInstance = hInst;
  wc.lpszClassName = EVENT_LOOP_WND_CLASS;
  RegisterClassExW(&wc);

  _messageHwnd = CreateWindowExW(0, EVENT_LOOP_WND_CLASS, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, hInst, 0);
  FOG_ASSERT(_messageHwnd);
}

void WinEventLoop::_handleWorkMessage()
{
  // If we are being called outside of the context of run(), then don't try to
  // do any work. This could correspond to a MessageBox call or something of that
  // sort.
  /*
  if (!_state)
  {
    // Since we handled a EVENT_LOOP_WM_HAVE_WORK message, we must still update
    // this flag.
    InterlockedExchange(&_haveWork, 0);
    return;
  }
  */

  // Let whatever would have run had we not been putting messages in the queue
  // run now. This is an attempt to make our dummy message not starve other
  // messages that may be in the Window's Message queue.
  _processPumpReplacementMessage();

  // Now give the delegate a chance to do some work. He'll let us know if he
  // needs to do more work.
  if (_doWork()) _scheduleWork();
}

void WinEventLoop::_handleTimerMessage()
{
  ::KillTimer(_messageHwnd, reinterpret_cast<UINT_PTR>(this));

  // If we are being called outside of the context of run(), then don't do
  // anything. This could correspond to a MessageBox call or something of that
  // sort.
  if (!_depth) return;

  _doDelayedWork(&_delayedWorkTime);
  if (!_delayedWorkTime.isNull())
  {
    // A bit gratuitous to set _delayedWorkTime again, but oh well.
    _scheduleDelayedWork(_delayedWorkTime);
  }
}

bool WinEventLoop::_processNextWindowsMessage()
{
  // If there are sent messages in the queue then PeekMessage() internally
  // dispatches the message and returns false. We return true in this case
  // to ensure that the message loop peeks again instead of calling
  // MsgWaitForMultipleObjectsEx again.
  DWORD queueStatus = ::GetQueueStatus(QS_SENDMESSAGE);
  bool sentMessagesInQueue = (HIWORD(queueStatus) & QS_SENDMESSAGE) != 0;

  MSG msg;
  if (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) return _processMessageHelper(msg);

  return sentMessagesInQueue;
}

bool WinEventLoop::_processMessageHelper(const MSG& msg)
{
  if (msg.message == WM_QUIT)
  {
    // Repost the QUIT message so that it will be retrieved by the primary
    // GetMessage() loop.
    _quitting = true;
    // _state->shouldQuit = true;
    ::PostQuitMessage(static_cast<int>(msg.wParam));
    return false;
  }

  // While running our main message pump, we discard EVENT_LOOP_WM_HAVE_WORK messages.
  if (msg.message == EVENT_LOOP_WM_HAVE_WORK && msg.hwnd == _messageHwnd)
    return _processPumpReplacementMessage();

  FOG_LISTENER_FOR_EACH(NativeEventListener, _nativeEventListenerList, onBeforeDispatch((void*)&msg));

  ::TranslateMessage(&msg);
  ::DispatchMessageW(&msg);

  FOG_LISTENER_FOR_EACH(NativeEventListener, _nativeEventListenerList, onAfterDispatch((void*)&msg));
  return true;
}

bool WinEventLoop::_processPumpReplacementMessage()
{
  // When we encounter a EVENT_LOOP_WM_HAVE_WORK message, this method is called
  // to peek and process a replacement message, such as a WM_PAINT or WM_TIMER.
  // The goal is to make the EVENT_LOOP_WM_HAVE_WORK as non-intrusive as
  // possible, even though a continuous stream of such messages are posted. This
  // method carefully peeks a message while there is no chance for a
  // EVENT_LOOP_WM_HAVE_WORK to be pending, then resets the _haveWork flag
  // (allowing a replacement EVENT_LOOP_WM_HAVE_WORK to possibly be posted), and
  // finally dispatches that peeked replacement.
  //
  // Note that the re-post of EVENT_LOOP_WM_HAVE_WORK may be asynchronous to
  // this thread!!

  MSG msg;
  bool have_message = (0 != ::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE));

  FOG_ASSERT(!have_message ||
             msg.message != EVENT_LOOP_WM_HAVE_WORK ||
             msg.hwnd != _messageHwnd);

  // Since we discarded a EVENT_LOOP_WM_HAVE_WORK message, we must update the
  // flag.
  InterlockedExchange(&_haveWork, 0);

  // TODO(darin,jar): There is risk of being lost in a sub-loop within the call
  // to processMessageHelper(), which could result in no longer getting a
  // EVENT_LOOP_WM_HAVE_WORK message until the next out-of-band call to
  // _scheduleWork().

  return have_message && _processMessageHelper(msg);
}

int WinEventLoop::_getCurrentDelay() const
{
  if (_delayedWorkTime.isNull()) return -1;

  // Be careful here.  TimeDelta has a precision of microseconds, but we want a
  // value in milliseconds.  If there are 5.5ms left, should the delay be 5 or
  // 6?  It should be 6 to avoid executing delayed work too early.
  double timeout = Math::ceil((_delayedWorkTime - Time::now()).getMillisecondsD());

  // If this value is negative, then we need to run delayed work soon.
  int delay = static_cast<int>(timeout);
  if (delay < 0) delay = 0;

  return delay;
}

// ============================================================================
// [Fog::WinGuiEventLoop]
// ============================================================================

WinGuiEventLoop::WinGuiEventLoop() : WinEventLoop(StringW::fromAscii8("UI.Windows"))
{
}

#endif // FOG_OS_WINDOWS

} // Fog namespace











#if 0
// Wrapper functions for use in above event loop framework.
bool EventLoop::processNextDelayedNonNestableTask()
{
  if (_state->runDepth != 1) return false;
  if (_deferredWorkQueue.isEmpty()) return false;

  Task* task = _deferredWorkQueue.front().task;
  _deferredWorkQueue.pop();

  runTask(task);
  return true;
}

//------------------------------------------------------------------------------

void EventLoop::quit()
{
  FOG_ASSERT(getCurrent() == this);

  if (_state)
  {
    _state->quitReceived = true;

    // FIXME: I'm added this, because if there is timer that is very cpu
    // intensive then pump never quits.
    _pump->quit();
  }
  else
  {
    // Must be inside run() to call quit().
    FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::EventLoop::AutoRunState]
// ============================================================================

EventLoop::AutoRunState::AutoRunState(EventLoop* loop) : _loop(loop)
{
  // Make the loop reference us.
  _previousState = _loop->_state;

  if (_previousState)
    runDepth = _previousState->runDepth + 1;
  else
    runDepth = 1;

  _loop->_state = this;

  // Initialize the other fields:
  quitReceived = false;

  dispatcher = NULL;
}

EventLoop::AutoRunState::~AutoRunState()
{
  _loop->_state = _previousState;
}

} // Fog namespace
#endif
