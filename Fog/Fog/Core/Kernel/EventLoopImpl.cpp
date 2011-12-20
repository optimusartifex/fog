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
#include <Fog/Core/Kernel/Event.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/EventLoopImpl.h>
#include <Fog/Core/Kernel/Task.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/Core/Tools/Time.h>

namespace Fog {

// ============================================================================
// [Fog::ScopedEventLoopRunState]
// ============================================================================

struct FOG_NO_EXPORT ScopedEventLoopRunState
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ScopedEventLoopRunState(EventLoopImpl* impl) :
    impl(impl)
  {
    impl->depth++;
    impl->quitting = false;
  }

  ~ScopedEventLoopRunState()
  {
    if (--impl->depth > 0)
      impl->quitting = false;
  }
  
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  EventLoopImpl* impl;

private:
  _FOG_NO_COPY(ScopedEventLoopRunState)
};

// ============================================================================
// [Fog::EventLoopImpl - Construction / Destruction]
// ============================================================================

EventLoopImpl::EventLoopImpl(const StringW& type) :
  type(type),
  depth(0),
  isObservable(0),
  isDestroyed(0),
  quitting(0),
  flag_0(0)
{
  refCount.init(1);
}

EventLoopImpl::~EventLoopImpl()
{
  // EventLoopImpl::destroyed() must be called by EventLoopImpl::release()
  // which must be called to destroy the EventLoopImpl!
  FOG_ASSERT(isDestroyed);
}

// ============================================================================
// [Fog::EventLoopImpl - AddRef / Release]
// ============================================================================

EventLoopImpl* EventLoopImpl::addRef() const
{
  refCount.inc();
  return const_cast<EventLoopImpl*>(this);
}

void EventLoopImpl::release()
{
  if (refCount.deref())
  {
    destroy();
    fog_delete(this);
  }
}

// ============================================================================
// [Fog::EventLoopImpl - Destroy]
// ============================================================================

void EventLoopImpl::destroy()
{
  // Prevent calling this twice or more.
  if (isDestroyed)
    return;

  // Clean up any unprocessed tasks, but take care: deleting a task could
  // result in the addition of more tasks. We set a limit on the number of
  // times we will allow a deleted task to generate more tasks. Normally,
  // we should only pass through this loop once or twice. If we end up
  // hitting the loop limit, then it is probably due to one task that is
  // being stubborn. Inspect the queues to see who is left.
  bool didWork;

  for (int i = 0; i < 100; i++)
  {
    deletePendingTasks();
    reloadWorkQueue();

    // If we end up with empty queues, then break out of the loop.
    didWork = deletePendingTasks();
    if (!didWork)
      break;
  }

  if (didWork)
    Debug::dbgFunc("Fog::EventLoopImpl", "destroy", "Couldn't delete all tasks.\n");

  isDestroyed = true;
}

// ============================================================================
// [Fog::EventLoopImpl - Observable]
// ============================================================================

err_t EventLoopImpl::addObserver(EventLoopObserver* obj)
{
  if (!isObservable)
    return ERR_RT_NOT_IMPLEMENTED;
  else
    return observerList.add(obj);
}

err_t EventLoopImpl::removeObserver(EventLoopObserver* obj)
{
  if (!isObservable)
    return ERR_RT_NOT_IMPLEMENTED;
  else
    return observerList.remove(obj);
}

// ============================================================================
// [Fog::EventLoopImpl - Run / Quit]
// ============================================================================

err_t EventLoopImpl::run()
{
  ScopedEventLoopRunState state(this);
  return runInternal();
}

err_t EventLoopImpl::runAllPending()
{
  ScopedEventLoopRunState state(this);

  // Means run until we would otherwise block.
  // FIXME:
  // _state->quitReceived = true;
  return runInternal();
}

err_t EventLoopImpl::quit()
{
  quitting = true;
  return ERR_OK;
}

err_t EventLoopImpl::postTask(Task* task, bool nestable, uint32_t delay)
{
  EventLoopPendingTask pendingTask(task, nestable);
  bool wasEmpty;

  // Warning: Don't try to short-circuit, and handle this thread's tasks more
  // directly, as it could starve handling of foreign threads. Put every task
  // into this queue.
  if (delay > 0)
    pendingTask.setTime(Time::now() + TimeDelta::fromMilliseconds(delay));

  { // Synchronized.
    AutoLock locked(lock);
    wasEmpty = incomingQueue.isEmpty();
    
    // If this fails then nothing happen, except that the task won't be
    // posted. This means that the called must do something. In many
    // situations this can be end of the application, because posting task
    // into another thread is very important.
    FOG_RETURN_ON_ERROR(incomingQueue.append(pendingTask));
  }

  // Event loop can be over at this time.
  if (wasEmpty)
    scheduleWork();

  return ERR_OK;
}

// TODO:
//bool EventLoopImpl::queueOrRunTask(Task* task)
//{
//}

void EventLoopImpl::runTask(Task* task)
{
  task->run();

  if (task->getDestroyOnFinish())
    task->destroy();
}

bool EventLoopImpl::deferOrRunPendingTask(const EventLoopPendingTask& pendingTask)
{
  if (depth == 1 || pendingTask.isNestable())
  {
    runTask(pendingTask.getTask());
    // Show that we ran a task (Note: a new one might arrive as a consequence!).
    return true;
  }

  // We couldn't run the task now because we're in a nested event loop and the
  // task isn't nestable. So add it to defferedWorkQueue so it can be called
  // later.
  deferredWorkQueue.append(pendingTask);
  return false;
}

bool EventLoopImpl::addToDelayedWorkQueue(const EventLoopPendingTask& pendingTask)
{
  size_t i = 0, length = delayedWorkQueue.getLength();
  const EventLoopPendingTask* tasks = delayedWorkQueue.getData();

  // TODO: Use binary search.
  while (i < length)
  {
    if (tasks[i].getTime() > pendingTask.getTime())
      break;
    i++;
  }

  // Move to the delayed work queue.
  delayedWorkQueue.insert(i, pendingTask);

  return i == 0;
}

void EventLoopImpl::reloadWorkQueue()
{
  // We can improve performance of our loading tasks from _incomingQueue to
  // workQueue by waiting until the last minute (workQueue is empty) to
  // load. That reduces the number of locks-per-task significantly when our
  // queues get large.

  // Wait till we really need to lock and load.
  if (!workQueue.isEmpty()) return;

  // Acquire all we can from the inter-thread queue with one lock acquisition.
  {
    AutoLock locked(lock);

    if (incomingQueue.isEmpty()) return;
    swap(incomingQueue, workQueue);
  }
}

bool EventLoopImpl::deletePendingTasks()
{
  bool didWork = !workQueue.isEmpty();
  while (!workQueue.isEmpty())
  {
    EventLoopPendingTask pendingTask = workQueue.getFirst();
    workQueue.removeFirst();

    if (!pendingTask.getTime().isNull())
    {
      // We want to delete delayed tasks in the same order in which they would
      // normally be deleted in case of any funny dependencies between delayed
      // tasks.
      addToDelayedWorkQueue(pendingTask);
    }
    else
    {
      Task* task = pendingTask.getTask();

      if (task->getDestroyOnFinish())
        task->destroy();
    }
  }

  didWork |= !deferredWorkQueue.isEmpty();
  while (!deferredWorkQueue.isEmpty())
  {
    Task* task = deferredWorkQueue.getFirst().getTask();
    deferredWorkQueue.removeFirst();

    if (task->getDestroyOnFinish())
      task->destroy();
  }

  didWork |= !delayedWorkQueue.isEmpty();
  while (!delayedWorkQueue.isEmpty())
  {
    Task* task = delayedWorkQueue.getFirst().getTask();
    delayedWorkQueue.removeFirst();

    if (task->getDestroyOnFinish())
      task->destroy();
  }

  return didWork;
}

bool EventLoopImpl::doWork()
{
  for (;;)
  {
    reloadWorkQueue();
    if (workQueue.isEmpty()) break;

    // Execute oldest task.
    do {
      EventLoopPendingTask pendingTask = workQueue.getFirst();
      workQueue.removeFirst();

      if (!pendingTask.getTime().isNull())
      {
        if (addToDelayedWorkQueue(pendingTask))
          scheduleDelayedWork(pendingTask.getTime());
      }
      else
      {
        if (deferOrRunPendingTask(pendingTask)) return true;
      }
    } while (!workQueue.isEmpty());
  }

  // Nothing happened.
  return false;
}

bool EventLoopImpl::doDelayedWork(Time* nextDelayedWorkTime)
{
  if (delayedWorkQueue.isEmpty())
  {
    *nextDelayedWorkTime = Time();
    return false;
  }

  Time t = delayedWorkQueue.getAt(0).getTime();
  if (t > Time::now())
  {
    // Still not elapsed.
    *nextDelayedWorkTime = t;
    return false;
  }

  EventLoopPendingTask pendingTask = delayedWorkQueue.getFirst();
  delayedWorkQueue.removeFirst();

  if (!delayedWorkQueue.isEmpty())
    *nextDelayedWorkTime = delayedWorkQueue.getAt(0).getTime();

  return deferOrRunPendingTask(pendingTask);
}

bool EventLoopImpl::doIdleWork()
{
  // TODO: What about this?
  // if (_processNextDelayedNonNestableTask()) return true;
  // TODO: Move this to the loop.
  //if (_state->quitReceived) _pump->quit();
  return false;
}

} // Fog namespace
