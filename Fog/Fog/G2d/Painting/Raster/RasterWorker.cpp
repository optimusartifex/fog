// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Debug.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/ThreadCondition.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/G2d/Painting/Raster/RasterEngine_p.h>
#include <Fog/G2d/Painting/Raster/RasterWorker_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterWorker]
// ============================================================================

RasterWorker::RasterWorker(Lock* lock) :
  cond(lock)
{
  state.init(RASTER_WORKER_RUNNING);
  shouldQuit.init(false);

  cmdPosition = 0;
}

RasterWorker::~RasterWorker()
{
}

void RasterWorker::run()
{
  RasterPaintEngine* engine = ctx.engine;
  RasterWorkerManager* wm = engine->wm;

#if defined(FOG_DEBUG_RASTER_SYNC)
  Debug::dbgMessage("Fog::RasterWorker[#%d]::run() - ThreadID=%d.\n", ctx.id, Thread::getCurrent()->getId());
#endif // FOG_DEBUG_RASTER_SYNC

#if 0
  bool isMainWorker = (wm->main == this);
  if (isMainWorker)
  {
    state.set(RASTER_WORKER_RUNNING);
    wm->waitingWorkers.dec();

    if (shouldQuit.get() == (int)true)
      wm->wakeUpSleeping(this);
    else
      wm->wakeUpScheduled(this);
  }

  for (;;)
  {
    // Do calculations and commands.
    int cont = 0;
    bool wait = false;

    do {
      wait = false;

      // Do calculations.
      while (calcCurrent < wm->calcPosition)
      {
        // Here is high thread concurrency and we are doing this without locking,
        // atomic operations helps us to get RasterCalc* or NULL. If we get NULL,
        // it's being calculated or done.
        RasterCalc** pclc = (RasterCalc**)&wm->calcData[calcCurrent++];
        RasterCalc* clc = *pclc;

        if (clc && AtomicCore<RasterCalc*>::cmpXchg(pclc, clc, NULL))
        {
#if defined(FOG_DEBUG_RASTER_CMD)
          Debug::dbgMessage("Fog::RasterWorker[#%d]::run() - Calculation %d (%p)\n", ctx.id, (int)calcCurrent, clc);
#endif // FOG_DEBUG_RASTER_CMD

          // If we are here, we won a battle with other threads
          // and RasterCalc* is ours.
          RasterCmd* cmd = clc->relatedTo;

          clc->run(&ctx);
          clc->release(&ctx);

          if (clc->wakeUpWorkers.get() != 0)
          {
            AutoLock locked(wm->lock);
            wm->wakeUpScheduled(this);
          }

          cont = 0;
          break;
        }
      }

      // Do command (as many as possible).
      while (cmdCurrent < wm->cmdPosition)
      {
        RasterCmd* cmd = wm->cmdData[cmdCurrent];

#if defined(FOG_DEBUG_RASTER_CMD)
        static const char* statusMsg[] = { "WAIT", "READY", "SKIP" };
        Debug::dbgMessage("Fog::RasterWorker[#%d]::run() - command %d (%p) status=%s\n", ctx.id, (int)cmdCurrent, cmd, statusMsg[cmd->status.get()]);
#endif // FOG_DEBUG_RASTER_CMD

        switch (cmd->status.get())
        {
          case RASTER_CMD_READY:
            cmd->run(&ctx);
            // ... Fall through ...

          case RASTER_CMD_SKIP:
            if (cmd->refCount.deref()) cmd->release(&ctx);

            cmdCurrent++;
            cont = 0;
            break;

          case RASTER_CMD_WAIT:
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
      AutoLock locked(wm->lock);

      if (calcCurrent < wm->calcPosition || (cmdCurrent < wm->cmdPosition && !wait))
      {
        continue;
      }

#if defined(FOG_DEBUG_RASTER_SYNC)
      Debug::dbgMessage("Fog::RasterWorker[#%d]::run() - Going to wait (currently waiting=%d)\n",
        ctx.id,
        (int)wm->waitingWorkers.get());
#endif // FOG_DEBUG_RASTER_SYNC

      state.set(RASTER_WORKER_WAITING);

      if (wm->waitingWorkers.addXchg(1) + 1 == wm->numWorkers)
      {
        // If count of waiting workers will be now count of workers (so this is
        // the last running one), we need to check if everything was completed
        // and if we can fire allFinishedCondition signal.
        if (wm->isCompleted())
        {
#if defined(FOG_DEBUG_RASTER_SYNC)
          Debug::dbgMessage("Fog::RasterWorker[#%d]::run() - Everything done, signaling allFinished...\n", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNC
          wm->allFinishedCondition.signal();
        }
        else
        {
          if (wm->wakeUpScheduled(this) == this)
          {
            wm->waitingWorkers.dec();
            state.set(RASTER_WORKER_RUNNING);
            continue;
          }
        }
      }

      if (cmdCurrent == wm->cmdPosition)
      {
        if (shouldQuit.get() == (int)true || isMainWorker)
        {
  #if defined(FOG_DEBUG_RASTER_SYNC)
          Debug::dbgMessage("Fog::RasterWorker[#%d]::run() - Quitting...\n", ctx.id);
  #endif // FOG_DEBUG_RASTER_SYNC

          // Set Running state so wakeUpSleeping() can't return us.
          state.set(RASTER_WORKER_RUNNING);
          wm->wakeUpSleeping(this);

          state.set(shouldQuit.get() == (int)true ? RASTER_WORKER_DONE : RASTER_WORKER_WAITING_AS_MAIN);
          return;
        }
      }

      cond.wait();
      state.set(RASTER_WORKER_RUNNING);
      wm->waitingWorkers.dec();

      if (shouldQuit.get() == (int)true)
        wm->wakeUpSleeping(this);
      else
        wm->wakeUpScheduled(this);
    }
  }
#endif
}

void RasterWorker::destroy()
{
  RasterPaintEngine* engine = ctx.engine;
  RasterWorkerManager* wm = engine->wm;

#if defined(FOG_DEBUG_RASTER_SYNC)
  Debug::dbgMessage("Fog::RasterWorker[#%d]::destroy()\n", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNC

  uint numWorkers = wm->numWorkers;
  if (wm->main) numWorkers--;

  if (wm->finishedWorkers.addXchg(1) + 1 == numWorkers)
  {
#if defined(FOG_DEBUG_RASTER_SYNC)
    Debug::dbgMessage("Fog::RasterWorker[#%d]::destroy() - I'm last, signaling release event...\n", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNC

    wm->releaseEvent->signal();
  }
}

// ============================================================================
// [Fog::RasterWorkerManager]
// ============================================================================

RasterWorkerManager::RasterWorkerManager() :
  numWorkers(0),
  maxWorkers(0),
  allFinishedCondition(&lock),
  releaseEvent(NULL),
  main(NULL),
  workers(NULL),
  allocator(16000)
{
  finishedWorkers.init(0);
  waitingWorkers.init(0);

  cmdCount.init(0);

  calcCount.init(0);
  calcPosition.init(0);
}

RasterWorkerManager::~RasterWorkerManager()
{
}

RasterWorker* RasterWorkerManager::wakeUpScheduled(RasterWorker* calledFrom)
{
  size_t c = cmdCount.get();

  for (size_t i = 0; i < numWorkers; i++)
  {
    RasterWorker* worker = &workers[i];

    if (worker->state.get() == RASTER_WORKER_WAITING &&
        worker->cmdPosition < c)
    {
      if (calledFrom != worker) worker->cond.signal();
      return worker;
    }
  }

  return NULL;
}

RasterWorker* RasterWorkerManager::wakeUpSleeping(RasterWorker* calledFrom)
{
  for (size_t i = 0; i < numWorkers; i++)
  {
    RasterWorker* worker = &workers[i];

    if (worker->state.get() == RASTER_WORKER_WAITING)
    {
      AutoLock locked(lock);
      if (calledFrom != worker) worker->cond.signal();
      return worker;
    }
  }

  return NULL;
}

bool RasterWorkerManager::isCompleted()
{
  size_t done = 0;
  size_t c = cmdCount.get();

  for (size_t i = 0; i < numWorkers; i++)
  {
    RasterWorker* worker = &workers[i];
    if (worker->cmdPosition == c) done++;
  }

  return done == numWorkers;
}

} // Fog namespace
