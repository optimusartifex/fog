// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Kernel/EventLoopObserver.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/UI/Engine/X11UIEngine.h>
#include <Fog/UI/Engine/X11UIEngineWindow.h>
#include <Fog/UI/Engine/X11UIEventLoopImpl_p.h>

// [Dependencies - C]
#include <stdlib.h>
#include <errno.h>

namespace Fog {

// ============================================================================
// [Fog::X11UIEventLoopImpl - Construction / Destruction]
// ============================================================================

X11UIEventLoopImpl::X11UIEventLoopImpl(X11UIEngine* engine) :
  EventLoopImpl(FOG_STR_(APPLICATION_UI_X11))
{
  _engine = engine;
  _wakeUpSent.init(0);
  _workPerLoop = 2;
}

X11UIEventLoopImpl::~X11UIEventLoopImpl()
{
}

// ============================================================================
// [Fog::X11UIEventLoopImpl - Run]
// ============================================================================

err_t X11UIEventLoopImpl::runInternal()
{
  int work = 0;

  // Sync with server before we get into the event loop.
  doXSync();

  for (;;)
  {
    bool didWork = false;
    bool more;

    // Process 'XEvent's - Highest priority.
    do {
      more = processNextXEvent();
      didWork |= more;

      if (_quitting)
        goto _End;
    } while (more);

    // doWork.
    didWork |= doWork();

    if (_quitting)
      goto _End;

    if (didWork && work < _workPerLoop)
      continue;

    work++;

    // doDelayedWork.
    didWork |= doDelayedWork(&_delayedWorkTime);
    if (_quitting)
      goto _End;

    if (didWork && work < _workPerLoop)
      continue;

    // doIdleWork.
    //
    // If quit is received in nestedLoop or through runAllPending(), we will
    // quit here, because we don't want to doXSync().
    didWork |= doIdleWork();

    if (_quitting)
      goto _End;

    // Call doXSync(), this is round-trip operation and can generate events.
    didWork |= doXSync();

    if (_quitting)
      goto _End;

    // Clear work status, this is why it's here...
    if (didWork || work < _workPerLoop)
      continue;
    work = 0;

    // Finally wait.
    waitForWork();
  }

_End:
  return ERR_OK;
}

// ============================================================================
// [Fog::X11UIEventLoopImpl - Schedule]
// ============================================================================

void X11UIEventLoopImpl::scheduleWork()
{
  wakeUp();
}

void X11UIEventLoopImpl::scheduleDelayedWork(const Time& delayedWorkTime)
{
  // We know that we can't be blocked on wait() right now since this method can
  // only be called on the same thread as run(), so we only need to update our
  // record of how long to sleep when we do sleep.
  _delayedWorkTime = delayedWorkTime;
}

// ============================================================================
// [Fog::X11UIEventLoopImpl - Helpers]
// ============================================================================

void X11UIEventLoopImpl::wakeUp()
{
  if (_wakeUpSent.cmpXchg(0, 1))
  {
    static const char c[4] = { 'W' };

    if (::write(_engine->_wakeUpPipe[1], c, sizeof(char)) != sizeof(char))
    {
      Logger::error("Fog::X11UIEventLoopImpl", "wakeUp", "Failed to write to weak-up pipe.");
    }
  }
}

void X11UIEventLoopImpl::waitForWork()
{
  int fd = _engine->_fd;
  int fdSize = Math::max(fd, _engine->_wakeUpPipe[0]) + 1;
  fd_set fdSet;

  struct timeval tval;
  struct timeval* ptval = NULL;

  FD_ZERO(&fdSet);
  FD_SET(fd, &fdSet);
  FD_SET(_engine->_wakeUpPipe[0], &fdSet);

  if (_delayedWorkTime.isNull())
  {
    // There are no scheduled tasks, so ptval is NULL and this tells to select()
    // that it should wait infinite time.
  }
  else
  {
    TimeDelta delay = _delayedWorkTime - Time::now();

    if (delay > TimeDelta())
    {
      // Go to sleep. X11 will wake us to process X events and we also set
      // interval to wake up to run planned tasks (usually Timers).
      int64_t udelay = delay.getMicroseconds();
      tval.tv_sec = (int)(udelay / 1000000);
      tval.tv_usec = (int)(udelay % 1000000);
      if (tval.tv_usec <= 100) tval.tv_usec = 100;
      ptval = &tval;
    }
    else
    {
      // It looks like delayedWorkTime indicates a time in the past, so we
      // need to call doDelayedWork now.
      _delayedWorkTime = Time();
      return;
    }
  }

  int ret = ::select(fdSize, &fdSet, NULL, NULL, ptval);

  if (ret < 0)
  {
    Logger::error("Fog::X11UIEventLoopImpl", "waitForWork", "Failed to call select(), errno=%d.", errno);
  }

  if (ret > 0)
  {
    if (FD_ISSET(_engine->_wakeUpPipe[0], &fdSet))
    {
      // Dummy c, the actual value is out of our interest.
      char c;

      if (::read(_engine->_wakeUpPipe[0], &c, sizeof(char)) != sizeof(char))
      {
        Logger::error("Fog::X11UIEventLoopImpl", "waitForWork()", "Failed to read from wake-up pipe.");
      }

      _wakeUpSent.cmpXchg(1, 0);
    }
  }
}

bool X11UIEventLoopImpl::doXSync()
{
  _engine->_XLib._XSync(_engine->_display, false);
  return _engine->_XLib._XEventsQueued(_engine->_display, QueuedAlready) != 0;
}

bool X11UIEventLoopImpl::processNextXEvent()
{
  if (!_engine->_XLib._XEventsQueued(_engine->_display, QueuedAlready))
    return false;

  XEvent xe;
  _engine->_XLib._XNextEvent(_engine->_display, &xe);

  X11UIEngineWindowImpl* wImpl = static_cast<X11UIEngineWindowImpl*>(
    _engine->getWindowByHandle((void*)xe.xany.window));

  FOG_EVENT_LOOP_OBSERVER_LIST_ENTER(_observerList);
  FOG_EVENT_LOOP_OBSERVER_LIST_EACH(_observerList, onBeforeDispatch(&xe));

  if (wImpl)
    wImpl->onXEvent(&xe);

  FOG_EVENT_LOOP_OBSERVER_LIST_EACH(_observerList, onAfterDispatch(&xe));
  FOG_EVENT_LOOP_OBSERVER_LIST_LEAVE(_observerList);

  return true;
}

} // Fog namespace
