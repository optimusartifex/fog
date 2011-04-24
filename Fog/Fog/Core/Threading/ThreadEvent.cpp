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
#include <Fog/Core/DateTime/Time.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/ThreadEvent.h>

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#include <math.h>

#include <stack>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
#include <errno.h>
#include <sys/time.h>
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::ThreadEvent]
// ============================================================================

// In Core/Event.h is this macro undefined
#if !defined CreateEvent
#if defined(_UNICODE)
#define CreateEvent CreateEventW
#else
#define CreateEvent CreateEventA
#endif // _UNICODE
#endif // CreateEvent

#if defined(FOG_OS_WINDOWS)
ThreadEvent::ThreadEvent(bool manualReset, bool signaled)
  : _event(::CreateEventW(NULL, manualReset, signaled, NULL))
{
  // We're probably going to crash anyways if this is ever NULL, so we might
  // as well make our stack reports more informative by crashing here.
  FOG_ASSERT(_event);
}

ThreadEvent::~ThreadEvent()
{
  ::CloseHandle(_event);
}

void ThreadEvent::reset()
{
  ::ResetEvent(_event);
}

void ThreadEvent::signal()
{
  ::SetEvent(_event);
}

bool ThreadEvent::isSignaled()
{
  return timedWait(TimeDelta());
}

bool ThreadEvent::wait()
{
  DWORD result = WaitForSingleObject(_event, INFINITE);
  // It is most unexpected that this should ever fail.  Help consumers learn
  // about it if it should ever fail.
  FOG_ASSERT(result == WAIT_OBJECT_0);
  return result == WAIT_OBJECT_0;
}

bool ThreadEvent::timedWait(const TimeDelta& maxTime)
{
  FOG_ASSERT(maxTime >= TimeDelta::fromMicroseconds(0));

  // Be careful here.  TimeDelta has a precision of microseconds, but this API
  // is in milliseconds.  If there are 5.5ms left, should the delay be 5 or 6?
  // It should be 6 to avoid returning too early.
  double timeout = ceil(maxTime.inMillisecondsF());
  DWORD result = ::WaitForSingleObject(_event, static_cast<DWORD>(timeout));
  switch (result)
  {
    case WAIT_OBJECT_0:
      return true;
    case WAIT_TIMEOUT:
      return false;
  }

  // It is most unexpected that this should ever fail.  Help consumers learn
  // about it if it should ever fail.
  return false;
}
#endif

#if defined(FOG_OS_POSIX)
ThreadEvent::ThreadEvent(bool manualReset, bool signaled) :
  _lock(),
  _cvar(&_lock),
  _signaled(signaled),
  _manualReset(manualReset)
{
}

ThreadEvent::~ThreadEvent()
{
  // Members are destroyed in the reverse of their initialization order, so we
  // should not have to worry about _lock being destroyed before _cvar.
}

void ThreadEvent::reset()
{
  AutoLock locked(_lock);
  _signaled = false;
}

void ThreadEvent::signal()
{
  AutoLock locked(_lock);

  if (!_signaled)
  {
    _signaled = true;
    if (_manualReset)
      _cvar.broadcast();
    else
      _cvar.signal();
  }
}

bool ThreadEvent::isSignaled()
{
  return timedWait(TimeDelta());
}

bool ThreadEvent::wait()
{
  AutoLock locked(_lock);

  while (!_signaled) _cvar.wait();
  if (!_manualReset) _signaled = false;
  return true;
}

bool ThreadEvent::timedWait(const TimeDelta& maxTime)
{
  AutoLock locked(_lock);

  // In case of spurious wake-ups, we need to adjust the amount of time that we
  // spend sleeping.
  TimeDelta totalTime;

  for (;;)
  {
    TimeTicks start = TimeTicks::now();
    _cvar.timedWait(maxTime - totalTime);
    if (_signaled) break;
    totalTime += TimeTicks::now() - start;
    if (totalTime >= maxTime) break;
  }

  bool result = _signaled;
  if (!_manualReset) _signaled = false;
  return result;
}
#endif // FOG_OS_POSIX

} // Fog namespace
