// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TASK_H
#define _FOG_CORE_TASK_H

// [Dependencies]
#include <Fog/Build/Build.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// ============================================================================
// [Fog::Task]
// ============================================================================

struct FOG_API Task
{
  Task();
  virtual ~Task();

  virtual void run() = 0;
  virtual void destroy();

  FOG_INLINE bool destroyOnFinish() const { return _destroyOnFinish; }

protected:
  bool _destroyOnFinish;

private:
  FOG_DISABLE_COPY(Task)
};

// ============================================================================
// [Fog::CancellableTask]
// ============================================================================

struct FOG_API CancelableTask : public Task
{
  // Not all tasks support cancellation.
  virtual void cancel() = 0;
};

// ============================================================================
// [Fog::DeleteTask<T>]
// ============================================================================

// Task to delete an object
template<typename T>
struct DeleteTask : public CancelableTask
{
  explicit DeleteTask(T* obj) : _obj(obj) {}

  virtual void run() { if (_obj) delete _obj; }
  virtual void cancel() { _obj = NULL; }

private:
  T* _obj;
};

// ============================================================================
// [Fog::QuitTask]
// ============================================================================

// Invokes quit() on the current EventLoop when run. Useful to schedule an
// arbitrary EventLoop to quit.
struct FOG_API QuitTask : public Task
{
public:
  virtual void run();
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_TASK_H
