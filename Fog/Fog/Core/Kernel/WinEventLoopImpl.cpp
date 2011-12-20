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
#include <Fog/Core/Kernel/EventLoopObserver.h>
#include <Fog/Core/Kernel/WinEventLoopImpl.h>
#include <Fog/Core/Kernel/Task.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/Core/Tools/Time.h>

#if defined(FOG_OS_WINDOWS)
# include <Fog/Core/OS/WinUtil.h>
#endif // FOG_OS_WINDOWS

namespace Fog {

// ============================================================================
// [Fog::WinEventLoopImpl - Constants]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
# if !defined(USER_TIMER_MINIMUM)
#  define USER_TIMER_MINIMUM 0x0000000A
# endif // USER_TIMER_MINIMUM
# if !defined(USER_TIMER_MAXIMUM)
#  define USER_TIMER_MAXIMUM 0x7FFFFFFF
# endif // USER_TIMER_MAXIMUM
#endif // FOG_OS_WINDOWS

static const WCHAR EVENT_LOOP_WND_CLASS[] = L"Fog::WinEventLoopImpl";

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
// [Fog::WinEventLoopImpl - Construction / Destruction]
// ============================================================================

WinEventLoopImpl::WinEventLoopImpl() :
  EventLoopImpl(FOG_STR_(APPLICATION_Core_Win)),
  haveWork(0)
{
  initMessageWnd();
}

WinEventLoopImpl::~WinEventLoopImpl()
{
  destroyMessageWnd();
}

// ============================================================================
// [Fog::WinEventLoopImpl - WinProcThunk]
// ============================================================================

static LRESULT CALLBACK WinEventLoopImpl_WndProcThunk(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  switch (message)
  {
    case EVENT_LOOP_WM_HAVE_WORK:
      reinterpret_cast<WinEventLoopImpl*>(wparam)->handleWorkMessage();
      break;
    case WM_TIMER:
      reinterpret_cast<WinEventLoopImpl*>(wparam)->handleTimerMessage();
      break;
  }

  return ::DefWindowProcW(hwnd, message, wparam, lparam);
}

// ============================================================================
// [Fog::WinEventLoopImpl - Run / Quit]
// ============================================================================

err_t WinEventLoopImpl::runInternal()
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
    // possibly be waiting in another task group. When we (for example)
    // processNextWindowsMessage(), there is a good chance there are still more
    // messages waiting. On the other hand, when any of these methods return
    // having done no work, then it is pretty unlikely that calling them again
    // quickly will find any work to do. Finally, if they all say they had no
    // work, then it is a good time to consider sleeping (waiting) for more
    // work.
    bool didWork = processNextWindowsMessage();
    if (quitting)
      break;

    didWork |= doWork();
    if (quitting)
      break;

    didWork |= doDelayedWork(&delayedWorkTime);
    // If we did not process any delayed work, then we can assume that our
    // existing WM_TIMER if any will fire when delayed work should run. We
    // don't want to disturb that timer if it is already in flight. However,
    // if we did do all remaining delayed work, then lets kill the WM_TIMER.
    if (didWork && delayedWorkTime.isNull())
    {
      ::KillTimer(messageHwnd, reinterpret_cast<UINT_PTR>(this));
    }

    if (quitting)
      break;

    if (didWork)
      continue;

    didWork = doIdleWork();

    if (quitting)
      break;

    if (didWork)
      continue;

    // Wait (sleep) until we have work to do again.
    waitForWork();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::WinEventLoopImpl - ScheduleWork]
// ============================================================================

void WinEventLoopImpl::scheduleWork()
{
  // Someone else continued the pumping.
  if (InterlockedExchange(&haveWork, 1)) return;

  // Make sure the EventLoop does some work for us.
  ::PostMessageW(messageHwnd, EVENT_LOOP_WM_HAVE_WORK, reinterpret_cast<WPARAM>(this), 0);
}

void WinEventLoopImpl::scheduleDelayedWork(const Time& delayedWorkTime)
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
  // Getting a spurious SetTimer() event firing is beyign, as we'll just be
  // processing an empty timer queue.
  this->delayedWorkTime = delayedWorkTime;

  int delay = getCurrentDelay();
  FOG_ASSERT(delay >= 0);

  if (delay < USER_TIMER_MINIMUM)
    delay = USER_TIMER_MINIMUM;

  // Create a WM_TIMER event that will wake us up to check for any pending
  // timers (in case we are running within a nested event loop).
  ::SetTimer(messageHwnd, reinterpret_cast<UINT_PTR>(this), delay, NULL);
}

// ============================================================================
// [Fog::WinEventLoopImpl - WaitForWork]
// ============================================================================

void WinEventLoopImpl::waitForWork()
{
  // Wait until a message is available, up to the time needed by the timer
  // manager to fire the next set of timers.
  int delay = getCurrentDelay();

  // Negative value means no timers waiting.
  if (delay < 0)
    delay = INFINITE;

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

    // Here was a bug that dalayed NC messages. Thank guys from #chromium-dev
    // for help with catching it. The problem is that QS_MOUSE will return
    // true also for NC messages, so there should be two PeekMessage() calls,
    // first for standard messages and second for NC messages.
    static const DWORD ncMouseFirst = WM_NCLBUTTONDOWN;
    static const DWORD ncMouseLast  = WM_NCMBUTTONDBLCLK;

    MSG msg = { 0 };
    DWORD queueStatus = GetQueueStatus(QS_MOUSE);

    if ((HIWORD(queueStatus) & QS_MOUSE) &&
        !::PeekMessageW(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE) &&
        !::PeekMessageW(&msg, NULL, ncMouseFirst , ncMouseLast , PM_NOREMOVE) )
    {
      ::WaitMessage();
    }

    return;
  }

  FOG_ASSERT(result != WAIT_FAILED);
}

// ============================================================================
// [Fog::WinEventLoopImpl - Windows Specific]
// ============================================================================

void WinEventLoopImpl::initMessageWnd()
{
  HINSTANCE hInst = GetModuleHandle(NULL);
  WNDCLASSEXW wc;

  ZeroMemory(&wc, sizeof(WNDCLASSEXW));
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = WinEventLoopImpl_WndProcThunk;
  wc.hInstance = hInst;
  wc.lpszClassName = EVENT_LOOP_WND_CLASS;
  RegisterClassExW(&wc);

  messageHwnd = CreateWindowExW(0, EVENT_LOOP_WND_CLASS, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, hInst, 0);
  FOG_ASSERT(messageHwnd);
}

void WinEventLoopImpl::destroyMessageWnd()
{
  ::DestroyWindow(messageHwnd);
}

void WinEventLoopImpl::handleWorkMessage()
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
  processPumpReplacementMessage();

  // Now give the delegate a chance to do some work. He'll let us know if he
  // needs to do more work.
  if (doWork())
    scheduleWork();
}

void WinEventLoopImpl::handleTimerMessage()
{
  ::KillTimer(messageHwnd, reinterpret_cast<UINT_PTR>(this));

  // If we are being called outside of the context of run(), then don't do
  // anything. This could correspond to a MessageBox call or something of that
  // sort.
  if (!depth)
    return;

  doDelayedWork(&delayedWorkTime);

  // A bit gratuitous to set _delayedWorkTime again, but oh well.
  if (!delayedWorkTime.isNull())
    scheduleDelayedWork(delayedWorkTime);
}

bool WinEventLoopImpl::processNextWindowsMessage()
{
  // If there are sent messages in the queue then PeekMessage() internally
  // dispatches the message and returns false. We return true in this case
  // to ensure that the message loop peeks again instead of calling
  // MsgWaitForMultipleObjectsEx again.
  DWORD queueStatus = ::GetQueueStatus(QS_SENDMESSAGE);

  MSG msg;
  if (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) 
    return processMessageHelper(msg);
  else
    return (HIWORD(queueStatus) & QS_SENDMESSAGE) != 0;
}

bool WinEventLoopImpl::processMessageHelper(const MSG& msg)
{
  if (msg.message == WM_QUIT)
  {
    // Repost the QUIT message so that it will be retrieved by the primary GetMessage()
    // loop.
    quitting = true;
    ::PostQuitMessage(static_cast<int>(msg.wParam));
    return false;
  }

  // While running our main message pump, we discard EVENT_LOOP_WM_HAVE_WORK messages.
  if (msg.message == EVENT_LOOP_WM_HAVE_WORK && msg.hwnd == messageHwnd)
    return processPumpReplacementMessage();

  FOG_EVENT_LOOP_OBSERVER_LIST_ENTER(observerList);
  FOG_EVENT_LOOP_OBSERVER_LIST_EACH(observerList, onBeforeDispatch((void*)&msg));

  ::TranslateMessage(&msg);
  ::DispatchMessageW(&msg);

  FOG_EVENT_LOOP_OBSERVER_LIST_EACH(observerList, onAfterDispatch((void*)&msg));
  FOG_EVENT_LOOP_OBSERVER_LIST_LEAVE(observerList);

  return true;
}

bool WinEventLoopImpl::processPumpReplacementMessage()
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
             msg.hwnd != messageHwnd);

  // Since we discarded a EVENT_LOOP_WM_HAVE_WORK message, we must update the
  // flag.
  InterlockedExchange(&haveWork, 0);

  // TODO(darin,jar): There is risk of being lost in a sub-loop within the call
  // to processMessageHelper(), which could result in no longer getting a
  // EVENT_LOOP_WM_HAVE_WORK message until the next out-of-band call to
  // _scheduleWork().

  return have_message && processMessageHelper(msg);
}

void WinEventLoopImpl::pumpOutPendingPaintMessages()
{
  // If we are being called outside of the context of run(), then don't try to
  // do any work.
  if (depth == 0)
    return;

  // Create a mini-message-loop to force immediate processing of only Windows
  // WM_PAINT messages. Don't provide an infinite loop, but do enough peeking
  // to get the job done. Actual common max is 4 peeks, but we'll be a little
  // safe here.
  static const int PAINT_MAX_PEEK_COUNT = 20;

  UINT msgFilter = 0;
  UINT removeMsg = PM_REMOVE | PM_QS_PAINT;

  if (WinUtil::getWinVersion() <= WIN_VERSION_2000)
  {
    msgFilter = WM_PAINT;
    removeMsg &= ~PM_QS_PAINT;
  }

  int peekCount;
  MSG msg;

  for (peekCount = 0; peekCount < PAINT_MAX_PEEK_COUNT; peekCount++)
  {
    if (!::PeekMessageW(&msg, NULL, msgFilter, msgFilter, removeMsg))
      break;

    // Handle WM_QUIT.
    processMessageHelper(msg);
    if (quitting)
      break;
  }
}

int WinEventLoopImpl::getCurrentDelay() const
{
  if (delayedWorkTime.isNull())
    return -1;

  // Be careful here. TimeDelta has a precision of microseconds, but we want a
  // value in milliseconds.  If there are 5.5ms left, should the delay be 5 or
  // 6? It should be 6 to avoid executing delayed work too early.
  int64_t delta = (delayedWorkTime - Time::now()).getDelta();
  int timeOut = int(delta / FOG_INT64_C(1000));

  if (delta % FOG_INT64_C(1000))
    timeOut++;

  // If this value is negative, then we need to run delayed work soon.
  return Math::max<int>(timeOut, 0);
}

} // Fog namespace
