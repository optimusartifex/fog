// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Lock.h>
#include <Fog/Core/ThreadCondition.h>
#include <Fog/Core/ThreadEvent.h>

#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Calc_p.h>
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Cmd_p.h>
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Engine_p.h>
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Worker_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterWorkerTask]
// ============================================================================

RasterWorkerTask::RasterWorkerTask(Lock* lock) :
  cond(lock)
{
  state.init(RUNNING);
  shouldQuit.init(false);

  calcCurrent = 0;
  cmdCurrent = 0;
}

RasterWorkerTask::~RasterWorkerTask()
{
}

void RasterWorkerTask::run()
{
  RasterPaintEngine* engine = ctx.engine;
  RasterWorkerManager* mgr = engine->workerManager;

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
  fog_debug("Fog::Painter[Worker #%d]::run() - ThreadID=%d", ctx.id, Thread::getCurrent()->getId());
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

  bool isMainTask = (mgr->mainTask == this);
  if (isMainTask)
  {
    state.set(RUNNING);
    mgr->waitingWorkers.dec();

    if (shouldQuit.get() == (int)true)
      mgr->wakeUpSleeping(this);
    else
      mgr->wakeUpScheduled(this);
  }

  for (;;)
  {
    // Do calculations and commands.
    int cont = 0;
    bool wait = false;

    do {
      wait = false;

      // Do calculations.
      while (calcCurrent < mgr->calcPosition)
      {
        // Here is high thread concurrency and we are doing this without locking,
        // atomic operations helps us to get RasterPaintCalc* or NULL. If we get NULL,
        // it's being calculated or done.
        RasterPaintCalc** pclc = (RasterPaintCalc**)&mgr->calcData[calcCurrent++];
        RasterPaintCalc* clc = *pclc;

        if (clc && AtomicCore<RasterPaintCalc*>::cmpXchg(pclc, clc, NULL))
        {
#if defined(FOG_DEBUG_RASTER_COMMANDS)
          fog_debug("Fog::Painter[Worker #%d]::run() - calculation %d (%p)", ctx.id, (int)calcCurrent, clc);
#endif // FOG_DEBUG_RASTER_COMMANDS

          // If we are here, we won a battle with other threads
          // and RasterPaintCalc* is ours.
          RasterPaintCmd* cmd = clc->relatedTo;

          clc->run(&ctx);
          clc->release(&ctx);

          if (clc->wakeUpWorkers.get() != 0)
          {
            AutoLock locked(mgr->lock);
            mgr->wakeUpScheduled(this);
          }

          cont = 0;
          break;
        }
      }

      // Do command (as many as possible).
      while (cmdCurrent < mgr->cmdPosition)
      {
        RasterPaintCmd* cmd = mgr->cmdData[cmdCurrent];

#if defined(FOG_DEBUG_RASTER_COMMANDS)
        static const char* statusMsg[] = { "WAIT", "READY", "SKIP" };
        fog_debug("Fog::Painter[Worker #%d]::run() - command %d (%p) status=%s", ctx.id, (int)cmdCurrent, cmd, statusMsg[cmd->status.get()]);
#endif // FOG_DEBUG_RASTER_COMMANDS

        switch (cmd->status.get())
        {
          case RASTER_COMMAND_READY:
            cmd->run(&ctx);
            // ... fall through...

          case RASTER_COMMAND_SKIP:
            if (cmd->refCount.deref()) cmd->release(&ctx);

            cmdCurrent++;
            cont = 0;
            break;

          case RASTER_COMMAND_WAIT:
            cmd->calculation->wakeUpWorkers.cmpXchg(0, 1);
            wait = true;
            goto skipCommands;
        }
      }

skipCommands:
      cont++;
      // We try two times to get work before we wait or quit.
    } while (cont <= 2);

    {
      AutoLock locked(mgr->lock);

      if (calcCurrent < mgr->calcPosition || (cmdCurrent < mgr->cmdPosition && !wait))
      {
        continue;
      }

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
      fog_debug("Fog::Painter[Worker #%d]::run() - going to wait (currently waiting=%d)",
        ctx.id,
        (int)mgr->waitingWorkers.get());
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

      state.set(WAITING);

      if (mgr->waitingWorkers.addXchg(1) + 1 == mgr->numWorkers)
      {
        // If count of waiting workers will be now count of workers (so this is
        // the last running one), we need to check if everything was completed
        // and if we can fire allFinishedCondition signal.
        if (mgr->isCompleted())
        {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
          fog_debug("Fog::Painter[Worker #%d]::run() - everything done, signaling allFinished...", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
          mgr->allFinishedCondition.signal();
        }
        else
        {
          if (mgr->wakeUpScheduled(this) == this)
          {
            mgr->waitingWorkers.dec();
            state.set(RUNNING);
            continue;
          }
        }
      }

      if (cmdCurrent == mgr->cmdPosition)
      {
        if (shouldQuit.get() == (int)true || isMainTask)
        {
  #if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
          fog_debug("Fog::Painter[Worker #%d]::run() - quitting...", ctx.id);
  #endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

          // Set Running state so wakeUpSleeping() can't return us.
          state.set(RUNNING);
          mgr->wakeUpSleeping(this);

          state.set(shouldQuit.get() == (int)true ? DONE : WAITING_MAIN);
          return;
        }
      }

      cond.wait();
      state.set(RUNNING);
      mgr->waitingWorkers.dec();

      if (shouldQuit.get() == (int)true)
        mgr->wakeUpSleeping(this);
      else
        mgr->wakeUpScheduled(this);
    }
  }
}

void RasterWorkerTask::destroy()
{
  RasterPaintEngine* engine = ctx.engine;
  RasterWorkerManager* mgr = engine->workerManager;

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
  fog_debug("Fog::Painter[Worker #%d]::WorkerTask::destroy()", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

  uint numWorkers = mgr->numWorkers;
  if (mgr->mainTask) numWorkers--;

  if (mgr->finishedWorkers.addXchg(1) + 1 == numWorkers)
  {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter[Worker #%d]::WorkerTask::destroy() - I'm last, signaling release event...", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

    mgr->releaseEvent->signal();
  }
}

// ============================================================================
// [Fog::RasterWorkerManager]
// ============================================================================

RasterWorkerManager::RasterWorkerManager() :
  allFinishedCondition(&lock),
  releaseEvent(NULL),
  commandAllocator(16000),
  mainTask(NULL)
{
}

RasterWorkerManager::~RasterWorkerManager()
{
}

RasterWorkerTask* RasterWorkerManager::wakeUpScheduled(RasterWorkerTask* calledFrom)
{
  for (sysuint_t i = 0; i < numWorkers; i++)
  {
    RasterWorkerTask* task = tasks[i].instancep();

    if (task->state.get() == RasterWorkerTask::WAITING && task->cmdCurrent < cmdPosition)
    {
      if (calledFrom != task) task->cond.signal();
      return task;
    }
  }

  return NULL;
}

RasterWorkerTask* RasterWorkerManager::wakeUpSleeping(RasterWorkerTask* calledFrom)
{
  for (sysuint_t i = 0; i < numWorkers; i++)
  {
    RasterWorkerTask* task = tasks[i].instancep();

    if (task->state.get() == RasterWorkerTask::WAITING)
    {
      AutoLock locked(lock);
      if (calledFrom != task) task->cond.signal();
      return task;
    }
  }

  return NULL;
}

bool RasterWorkerManager::isCompleted()
{
  sysuint_t done = 0;

  for (sysuint_t i = 0; i < numWorkers; i++)
  {
    RasterWorkerTask* task = tasks[i].instancep();
    if (task->cmdCurrent == cmdPosition) done++;
  }

  return done == numWorkers;
}

} // Fog namespace
