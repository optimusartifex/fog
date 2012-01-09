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
#include <Fog/Core/Kernel/DefaultEventLoopImpl_p.h>
#include <Fog/Core/Tools/ManagedString.h>

namespace Fog {

// ============================================================================
// [Fog::DefaultEventLoopImpl - Construction / Destruction]
// ============================================================================

DefaultEventLoopImpl::DefaultEventLoopImpl() :
  EventLoopImpl(FOG_STR_(APPLICATION_Core_Default)),
  _wakeUpEvent(false, false)
{
}

DefaultEventLoopImpl::~DefaultEventLoopImpl()
{
}

// ============================================================================
// [Fog::DefaultEventLoopImpl - Run / Quit]
// ============================================================================

err_t DefaultEventLoopImpl::runInternal()
{
  for (;;)
  {
    bool didWork = doWork();
    if (_quitting)
      break;

    didWork |= doDelayedWork(&_delayedWorkTime);
    if (_quitting)
      break;

    if (didWork)
      continue;

    didWork = doIdleWork();
    if (_quitting)
      break;

    if (didWork)
      continue;

    if (_delayedWorkTime.isNull())
    {
      _wakeUpEvent.wait();
    }
    else
    {
      TimeDelta delay = _delayedWorkTime - Time::now();
      if (delay > TimeDelta())
        _wakeUpEvent.wait(delay);
      else
        // It looks like delayedWorkTime indicates a time in the past, so we
        // need to call doDelayedWork() now.
        _delayedWorkTime = Time();
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::DefaultEventLoopImpl - Schedule]
// ============================================================================

void DefaultEventLoopImpl::scheduleWork()
{
  // Since this can be called on any thread, we need to ensure that our run()
  // loop wakes up.
  _wakeUpEvent.signal();
}

void DefaultEventLoopImpl::scheduleDelayedWork(const Time& delayedWorkTime)
{
  // We know that we can't be blocked on wait() right now since this method can
  // only be called on the same thread as run(), so we only need to update our
  // record of how long to sleep when we do sleep.
  this->_delayedWorkTime = delayedWorkTime;
}

} // Fog namespace
