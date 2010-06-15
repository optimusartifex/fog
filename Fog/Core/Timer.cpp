// [Fog-Core Library - Public API]
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
#include <Fog/Core/EventLoop.h>
#include <Fog/Core/Timer.h>

FOG_IMPLEMENT_OBJECT(Fog::Timer)

namespace Fog {

// ============================================================================
// [Fog::TimerTask]
// ============================================================================

struct FOG_HIDDEN TimerTask : public Task
{
  TimerTask(Timer* timer) : 
    timer(timer)
  {
  }

  virtual void run()
  {
    if (timer)
    {
      // First send events.
      TimerEvent e(timer);
      timer->sendEvent(&e);
      
      // Repeat?
      if (timer)
      {
        timer->getHomeThread()->getEventLoop()->postTask(this, true, static_cast<int>(timer->_interval.inMilliseconds()));
        _destroyOnFinish = false;
      }
      else
      {
        // We must be sure, it can be set to false.
        _destroyOnFinish = true;
      }
    }
  }

  Timer* timer;
};

// ============================================================================
// [Fog::Timer]
// ============================================================================

Timer::Timer() :
  _task(NULL)
{
}

Timer::~Timer()
{
  stop();
}

bool Timer::isRunning()
{
  return _task != NULL;
}

bool Timer::start()
{
  stop();

  _task = new(std::nothrow) TimerTask(this);
  getHomeThread()->getEventLoop()->postTask(_task, true, static_cast<int>(_interval.inMilliseconds()));
  return true;
}

void Timer::stop()
{
  if (_task)
  {
    static_cast<TimerTask *>(_task)->timer = NULL;
    _task = NULL;
  }
}

void Timer::setInterval(TimeDelta interval)
{
  if (_interval != interval)
  {
    _interval = interval;
  }
}

void Timer::onTimer(TimerEvent* e)
{
  // Nothing...
}

} // Fog namespace
