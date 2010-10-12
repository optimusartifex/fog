// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTWORKER_P_H
#define _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTWORKER_P_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Task.h>
#include <Fog/Core/Thread.h>
#include <Fog/Core/ThreadCondition.h>
#include <Fog/Core/MemoryAllocator_p.h>

#include <Fog/Graphics/PaintEngine/RasterPaintBase_p.h>
#include <Fog/Graphics/PaintEngine/RasterPaintContext_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct RasterPaintCalc;
struct RasterPaintCmd;

// ============================================================================
// [Fog::RasterWorkerTask]
// ============================================================================

//! @internal
//!
//! @brief This is task created per painter thread that contains all variables
//! needed to process painter commands in parallel. The goal is that condition
//! variable is shared across all painter threads so one signal will wake them
//! all.
struct FOG_HIDDEN RasterWorkerTask : public Task
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterWorkerTask(Lock* condLock);
  virtual ~RasterWorkerTask();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run();
  virtual void destroy();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // State
  enum STATE
  {
    RUNNING,
    WAITING,
    WAITING_MAIN,
    DONE
  };

  //! @brief Current task state.
  Atomic<int> state;

  //! @brief Whether the worker should quit from the main loop.
  Atomic<int> shouldQuit;

  //! @brief Current position in calculation buffer.
  volatile sysint_t calcCurrent;

  //! @brief Current position in command buffer.
  volatile sysint_t cmdCurrent;

  //! @brief Worker context.
  RasterPaintContext ctx;

  //! @brief Worker condition variable.
  ThreadCondition cond;
};

// ============================================================================
// [Fog::RasterWorkerManager]
// ============================================================================

//! @internal
//!
//! @brief Worker manager (used by multithreaded paint engine).
struct FOG_HIDDEN RasterWorkerManager
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterWorkerManager();
  ~RasterWorkerManager();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  // To call the lock must be locked!
  RasterWorkerTask* wakeUpScheduled(RasterWorkerTask* calledFrom);
  RasterWorkerTask* wakeUpSleeping(RasterWorkerTask* calledFrom);
  bool isCompleted();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Count of workers used in engine.
  uint numWorkers;

  //! @brief Count of workers finished (used to quit).
  Atomic<uint> finishedWorkers;

  //! @brief Count of workers waiting (for calculation).
  Atomic<uint> waitingWorkers;

  //! @brief Lock to synchronize basic access from multiple threads.
  Lock lock;
  ThreadCondition allFinishedCondition;
  ThreadEvent* releaseEvent;

  //! @brief Thread instances, always <code>numWorkers - 1</code>!
  Thread* threads[RASTER_MAX_WORKERS];
  //! @brief Worker tasks, the last one is called from main thread.
  Static<RasterWorkerTask> tasks[RASTER_MAX_WORKERS];

  //! @brief Main task (if exists).
  RasterWorkerTask* mainTask;

  //! @brief Commands and calculations allocator.
  ZoneMemoryAllocator commandAllocator;

  // Commands manager.
  volatile sysint_t cmdPosition;
  RasterPaintCmd* volatile cmdData[RASTER_MAX_COMMANDS];

  // Calculations manager.
  volatile sysint_t calcPosition;
  RasterPaintCalc* volatile calcData[RASTER_MAX_CALCULATIONS];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTWORKER_P_H
