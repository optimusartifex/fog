// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTWORKER_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTWORKER_P_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Memory/ZoneMemoryAllocator_p.h>
#include <Fog/Core/System/Task.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/AtomicPadding.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadCondition.h>
#include <Fog/G2d/Painting/RasterPaintContext_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterWorker]
// ============================================================================

//! @internal
//!
//! @brief This is a @c Task created per painter thread that contains all variables
//! needed to process painter commands in parallel. The condition variable is
//! shared across all painter threads so only one signal is needed to wake them all.
struct FOG_NO_EXPORT RasterWorker : public Task
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterWorker(Lock* condLock);
  virtual ~RasterWorker();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run();
  virtual void destroy();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The worker's thread (or NULL if this is the main worker).
  Thread* thread;
  //! @brief The worker's condition variable.
  ThreadCondition cond;

  //! @brief Worker context.
  RasterContext ctx;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  AtomicPaddingSeparator _padding_separator;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief Current task state.
  Atomic<int> state;
  AtomicPadding1<int> _padding_state;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief Whether the worker should quit from the main loop.
  Atomic<int> shouldQuit;
  AtomicPadding1<int> _padding_shouldQuit;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief The current command position.
  volatile size_t cmdPosition;
};

// ============================================================================
// [Fog::RasterWorkerManager]
// ============================================================================

//! @internal
//!
//! @brief Worker manager (used by multithreaded paint engine).
struct FOG_NO_EXPORT RasterWorkerManager
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterWorkerManager();
  ~RasterWorkerManager();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  // Must be locked!
  RasterWorker* wakeUpScheduled(RasterWorker* calledFrom);
  RasterWorker* wakeUpSleeping(RasterWorker* calledFrom);
  bool isCompleted();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The count of workers used by the worker manager (0 initial).
  uint numWorkers;
  //! @brief The count of the maximum workers which can be created/used by the
  //! worker manager.
  uint maxWorkers;

  //! @brief Lock to synchronize access to the manager from multiple threads.
  Lock lock;
  ThreadCondition allFinishedCondition;
  ThreadEvent* releaseEvent;

  //! @brief Main task (if exists).
  RasterWorker* main;
  //! @brief Worker instances, see @c numWorkers.
  RasterWorker* workers;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  AtomicPaddingSeparator _padding_separator;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief Count of workers finished (used to quit).
  Atomic<uint> finishedWorkers;
  AtomicPadding1<uint> _padding_finishedWorkers;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief Count of workers waiting (for calculation).
  Atomic<uint> waitingWorkers;
  AtomicPadding1<uint> _padding_waitingWorkers;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief Action allocator.
  ZoneMemoryAllocator allocator;
  AtomicPadding1<ZoneMemoryAllocator> _padding_allocator;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief The count of commands in @c cmdBuffer.
  Atomic<size_t> cmdCount;
  AtomicPadding1<size_t> _padding_cmdCount;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief The count of calculations in @c calcBuffer.
  Atomic<size_t> calcCount;
  AtomicPadding1<size_t> _padding_calcCount;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief The position of next calculation in @c calcBuffer for processing.
  //!
  //! This number is increased by a worker.
  Atomic<size_t> calcPosition;
  AtomicPadding1<size_t> _padding_calcPosition;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //! @brief Commands data.
  RasterCmd* volatile cmdBuffer[RASTER_MAX_COMMANDS];
  //! @brief Calculations data.
  RasterCalc* volatile calcBuffer[RASTER_MAX_CALCULATIONS];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTWORKER_P_H
