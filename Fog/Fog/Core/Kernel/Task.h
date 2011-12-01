// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_TASK_H
#define _FOG_CORE_KERNEL_TASK_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::Task]
// ============================================================================

//! @brief Task that can be send to thread event loop.
struct FOG_API Task
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Task();
  virtual ~Task();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool destroyOnFinish() const { return _destroyOnFinish; }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual void run() = 0;
  virtual void destroy();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  bool _destroyOnFinish;

private:
  _FOG_NO_COPY(Task)
};

// ============================================================================
// [Fog::CancelableTask]
// ============================================================================

struct FOG_API CancelableTask : public Task
{
  // Not all tasks support cancellation.
  virtual void cancel() = 0;
};

// ============================================================================
// [Fog::DeleteTask<TypeT>]
// ============================================================================

// Task to delete an object
template<typename TypeT>
struct DeleteTask : public CancelableTask
{
  explicit DeleteTask(TypeT* obj) : _obj(obj) {}

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual void run() { if (_obj) fog_delete(_obj); }
  virtual void cancel() { _obj = NULL; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  TypeT* _obj;
};

// ============================================================================
// [Fog::QuitTask]
// ============================================================================

// Invokes quit() on the current EventLoop when run. Useful to schedule an
// arbitrary EventLoop to quit.
struct FOG_API QuitTask : public Task
{
  virtual void run();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_KERNEL_TASK_H
