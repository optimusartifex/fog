// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Kernel/EventLoopObserver.h>
#include <Fog/Core/Kernel/MacEventLoopImpl.h>
#include <Fog/Core/Kernel/Task.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/Core/Tools/Time.h>

#if defined(FOG_OS_MAC)
# include <Fog/Core/OS/MacUtil.h>
#endif // FOG_OS_MAC

namespace Fog {

// ============================================================================
// [Fog::MacEventLoopImpl - Callbacks]
// ============================================================================

static void MacEventLoopImpl_onDelayedWorkTimer(CFRunLoopTimerRef timer, void* info)
{
  MacEventLoopImpl* impl = static_cast<MacEventLoopImpl*>(info);
  CFRunLoopSourceSignal(impl->_delayedWorkSource);  
}

static void MacEventLoopImpl_onWorkSource(void* info)
{
  static_cast<MacEventLoopImpl*>(info)->runWork();
}

static void MacEventLoopImpl_onDelayedWorkSource(void* info)
{
  static_cast<MacEventLoopImpl*>(info)->runDelayedWork();
}

static void MacEventLoopImpl_onIdleWorkSource(void* info)
{
  static_cast<MacEventLoopImpl*>(info)->runIdleWork();
}

// ============================================================================
// [Fog::MacEventLoopImpl - Construction / Destruction]
// ============================================================================

MacEventLoopImpl::MacEventLoopImpl() :
  EventLoopImpl(FOG_STR_(APPLICATION_Core_Mac))
{
  _runLoop = CFRunLoopGetCurrent();
  CFRetain(_runLoop);

  CFRunLoopTimerContext timerContext = CFRunLoopTimerContext();
  timerContext.info = this;

  _delayedWorkTimer = CFRunLoopTimerCreate(
    NULL,     // Allocator.
    DBL_MAX,  // Fire time.
    DBL_MAX,  // Interval.
    0,        // Flags.
    0,        // Priority.
    MacEventLoopImpl_onDelayedWorkTimer,
    &timerContext);

  CFRunLoopAddTimer(_runLoop, _delayedWorkTimer, kCFRunLoopCommonModes);

  // Run work has 1st priority.
  CFRunLoopSourceContext sourceContext = CFRunLoopSourceContext();
  sourceContext.info = this;
  sourceContext.perform = MacEventLoopImpl_onWorkSource;
  _workSource = CFRunLoopSourceCreate(NULL, 1, &sourceContext);
  CFRunLoopAddSource(_runLoop, _workSource, kCFRunLoopCommonModes);

  // Run delayed work has 2nd priority.
  sourceContext.perform = MacEventLoopImpl_onDelayedWorkSource;
  _delayedWorkSource = CFRunLoopSourceCreate(NULL, 2, &sourceContext);
  CFRunLoopAddSource(_runLoop, _delayedWorkSource, kCFRunLoopCommonModes);
  
  // Run idle work has 3rd priority.
  sourceContext.perform = MacEventLoopImpl_onIdleWorkSource;
  _idleWorkSource = CFRunLoopSourceCreate(NULL, 3, &sourceContext);
  CFRunLoopAddSource(_runLoop, _idleWorkSource, kCFRunLoopCommonModes);
}

MacEventLoopImpl::~MacEventLoopImpl()
{
  CFRunLoopRemoveSource(_runLoop, _idleWorkSource, kCFRunLoopCommonModes);
  CFRelease(_idleWorkSource);

  CFRunLoopRemoveSource(_runLoop, _delayedWorkSource, kCFRunLoopCommonModes);
  CFRelease(_delayedWorkSource);

  CFRunLoopRemoveSource(_runLoop, _workSource, kCFRunLoopCommonModes);
  CFRelease(_workSource);

  CFRunLoopRemoveTimer(_runLoop, _delayedWorkTimer, kCFRunLoopCommonModes);
  CFRelease(_delayedWorkTimer);

  CFRelease(_runLoop);
}

// ============================================================================
// [Fog::WinEventLoopImpl - Run / Quit]
// ============================================================================

err_t MacEventLoopImpl::runInternal()
{
}

bool MacEventLoopImpl::runWork()
{
  bool didWork = doWork();

  if (didWork)
    CFRunLoopSourceSignal(_workSource);

  return didWork;
}

bool MacEventLoopImpl::runDelayedWork()
{
  doDelayedWork(&_delayedWorkTime);

  bool moreWork = !_delayedWorkTime.isNull();
  if (!moreWork)
    return false;

  TimeDelta delay = _delayedWorkTime - Time::now();
  if (delay.getDelta() > 0)
  {
    scheduleDelayedWork(_delayedWorkTime);
    return true;
  }
  else
  {
    CFRunLoopSourceSignal(_delayedWorkSource);
    return true;
  }
}

bool MacEventLoopImpl::runIdleWork()
{
  bool didWork = doIdleWork();

  if (didWork)
    CFRunLoopSourceSignal(_idleWorkSource);

  return didWork;
}

err_t MacEventLoopImpl::quit()
{
  quitting = true;
  return ERR_OK;
}

// ============================================================================
// [Fog::WinEventLoopImpl - ScheduleWork]
// ============================================================================

void MacEventLoopImpl::scheduleWork()
{
  CFRunLoopSourceSignal(_workSource);
  CFRunLoopWakeUp(_runLoop);
}

void MacEventLoopImpl::scheduleDelayedWork(const Time& delayedWorkTime)
{
  double t = double(delayedWorkTime.getValue()) / double(1000000) +
    kCFAbsoluteTimeIntervalSince1970;

  CFRunLoopTimerSetNextFireDate(_delayedWorkTimer, t);
}

} // Fog namespace
