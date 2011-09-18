// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/Task.h>
#include <Fog/Core/Threading/Thread.h>

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
  fog_delete(this);
}

// ============================================================================
// [Fog::QuitTask]
// ============================================================================

void QuitTask::run()
{
  Thread::getCurrentThread()->getEventLoop()->quit();
}

} // Fog namespace
