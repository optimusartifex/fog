// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/EventLoop.h>
#include <Fog/Core/Task.h>
#include <Fog/Core/Thread.h>

namespace Fog {

// ============================================================================
// [Fog::Task]
// ============================================================================

Task::Task() : _destroyOnFinish(true)
{
}

Task::~Task()
{
}

void Task::destroy()
{
  delete this;
}

// ============================================================================
// [Fog::QuitTask]
// ============================================================================

void QuitTask::run()
{
  Thread::getCurrent()->getEventLoop()->quit();
}

} // Fog namespace
