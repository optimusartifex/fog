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
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/Core/Tools/Time.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
# include <Fog/Core/OS/WinUtil.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <errno.h>
# include <sys/time.h>
#endif // FOG_OS_POSIX

namespace Fog {

#if defined(FOG_OS_WINDOWS)

// ============================================================================
// [Fog::ThreadEvent - Construction / Destruction (Windows)]
// ============================================================================

static err_t FOG_CDECL ThreadEvent_ctor(ThreadEvent* self, bool manualReset, bool initialState)
{
  self->_event = ::CreateEventW(NULL, manualReset, initialState, NULL);
  if (self->_event == NULL)
    return ERR_OK;
  else
    return WinUtil::getErrFromWinLastError();
}

static void FOG_CDECL ThreadEvent_dtor(ThreadEvent* self)
{
  ::CloseHandle(self->_event);
}

// ============================================================================
// [Fog::ThreadEvent - Signal / Reset (Windows)]
// ============================================================================

static void FOG_CDECL ThreadEvent_signal(ThreadEvent* self)
{
  ::SetEvent(self->_event);
}

static void FOG_CDECL ThreadEvent_reset(ThreadEvent* self)
{
  ::ResetEvent(self->_event);
}

// ============================================================================
// [Fog::ThreadEvent - Wait]
// ============================================================================

static bool FOG_CDECL ThreadEvent_isSignaled(ThreadEvent* self)
{
  return ::WaitForSingleObject(self->_event, 0) == WAIT_OBJECT_0;
}

static bool FOG_CDECL ThreadEvent_wait(ThreadEvent* self, const TimeDelta* maxTime)
{
  if (maxTime == NULL)
    return ::WaitForSingleObject(self->_event, INFINITE) == WAIT_OBJECT_0;

  TimeDelta t = *maxTime;
  if (t < TimeDelta::fromMicroseconds(0))
    return false;

  // WaitForSingleObject() has only milliseconds precision, so ceil maxTime.
  DWORD timeout = (DWORD)t.getMilliseconds();
  if ((t.getDelta() % 1000) > 0)
    timeout++;

  DWORD result = ::WaitForSingleObject(self->_event, timeout);
  switch (result)
  {
    case WAIT_OBJECT_0:
      return true;

    case WAIT_TIMEOUT:
      return false;

    default:
      return false;
  }
}
#endif

#if defined(FOG_OS_POSIX)

// ============================================================================
// [Fog::ThreadEvent - Construction / Destruction (Posix)]
// ============================================================================

static err_t FOG_CDECL ThreadEvent_ctor(ThreadEvent* self, bool manualReset, bool initialState)
{
  self->_lock.init();
  self->_cvar.initCustom1(&self->_lock);

  self->_manualReset = manualReset;
  self->_signaled = initialState;
}

static void FOG_CDECL ThreadEvent_dtor(ThreadEvent* self)
{
  self->_cvar.destroy();
  self->_lock.destroy();
}

// ============================================================================
// [Fog::ThreadEvent - Signal / Reset (Posix)]
// ============================================================================

static void FOG_CDECL ThreadEvent_signal(ThreadEvent* self)
{
  AutoLock locked(self->_lock);

  if (!self->_signaled)
  {
    self->_signaled = true;

    if (self->_manualReset)
      self->_cvar->broadcast();
    else
      self->_cvar->signal();
  }
}

static void FOG_CDECL ThreadEvent_reset(ThreadEvent* self)
{
  AutoLock locked(self->_lock);
  self->_signaled = false;
}

// ============================================================================
// [Fog::ThreadEvent - Wait (Posix)]
// ============================================================================

static bool FOG_CDECL ThreadEvent_isSignaled(ThreadEvent* self)
{
  return self->wait(TimeDelta());
}

static bool FOG_CDECL ThreadEvent_wait(ThreadEvent* self, const TimeDelta* maxTime)
{
  AutoLock locked(self->_lock);

  if (maxTime == NULL)
  {
    while (!self->_signaled)
      self->_cvar->wait();

    if (!self->_manualReset)
      self->_signaled = false;
    return true;
  }
  else
  {
    TimeDelta t = *maxTime;
    TimeDelta total;

    for (;;)
    {
      TimeTicks start = TimeTicks::now();
      self->_cvar->wait(t - total);

      if (self->_signaled)
        break;

      total += TimeTicks::now() - start;
      if (total >= t)
        break;
    }

    bool result = self->_signaled;

    if (!self->_manualReset)
      self->_signaled = false;
    return result;
  }
}

#endif // FOG_OS_POSIX

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ThreadEvent_init(void)
{
  fog_api.threadevent_ctor = ThreadEvent_ctor;
  fog_api.threadevent_dtor = ThreadEvent_dtor;

  fog_api.threadevent_signal = ThreadEvent_signal;
  fog_api.threadevent_reset = ThreadEvent_reset;
  fog_api.threadevent_isSignaled = ThreadEvent_isSignaled;
  fog_api.threadevent_wait = ThreadEvent_wait;
}

} // Fog namespace
