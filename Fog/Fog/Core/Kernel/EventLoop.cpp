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
#include <Fog/Core/Kernel/EventLoopImpl.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

// ============================================================================
// [Fog::EventLoop - Construction / Destruction]
// ============================================================================

static void FOG_CDECL EventLoop_ctor(EventLoop* self)
{
  self->_d = NULL;
}

static void FOG_CDECL EventLoop_ctorCopy(EventLoop* self, const EventLoop* other)
{
  EventLoopImpl* d = other->_d;

  if (d != NULL)
    d = d->addRef();

  self->_d = d;
}

static void FOG_CDECL EventLoop_dtor(EventLoop* self)
{
  EventLoopImpl* d = self->_d;
  
  if (d)
    d->release();
}

// ============================================================================
// [Fog::EventLoop - Accessors]
// ============================================================================

static err_t FOG_CDECL EventLoop_getType(const EventLoop* self, StringW* dst)
{
  EventLoopImpl* d = self->_d;
  
  if (d == NULL)
  {
    dst->clear();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    return dst->set(d->type);
  }
}

static int FOG_CDECL EventLoop_getDepth(const EventLoop* self)
{
  EventLoopImpl* d = self->_d;
  
  if (d == NULL)
    return 0;
  else
    return d->depth;
}

// ============================================================================
// [Fog::EventLoop - Native]
// ============================================================================

static bool FOG_CDECL EventLoop_isObservable(const EventLoop* self)
{
  EventLoopImpl* d = self->_d;
  
  if (d == NULL)
    return false;

  return d->isObservable;
}

static err_t FOG_CDECL EventLoop_addObserver(EventLoop* self, EventLoopObserver* obj)
{
  EventLoopImpl* d = self->_d;
  
  if (d == NULL)
    return ERR_RT_INVALID_STATE;

  return d->addObserver(obj);
}

static err_t FOG_CDECL EventLoop_removeObserver(EventLoop* self, EventLoopObserver* obj)
{
  EventLoopImpl* d = self->_d;
  
  if (d == NULL)
    return ERR_RT_INVALID_STATE;

  return d->removeObserver(obj);
}

// ============================================================================
// [Fog::EventLoop - Run / Quit]
// ============================================================================

static err_t FOG_CDECL EventLoop_run(EventLoop* self)
{
  EventLoopImpl* d = self->_d;
  
  if (d == NULL)
    return ERR_RT_INVALID_STATE;
  
  return d->run();
}

static err_t FOG_CDECL EventLoop_runAllPending(EventLoop* self)
{
  EventLoopImpl* d = self->_d;
  
  if (d == NULL)
    return ERR_RT_INVALID_STATE;

  return d->runAllPending();
}

static err_t FOG_CDECL EventLoop_quit(EventLoop* self)
{
  EventLoopImpl* d = self->_d;
  
  if (d == NULL)
    return ERR_RT_INVALID_STATE;

  return d->quit();
}

// ============================================================================
// [Fog::EventLoop - Post]
// ============================================================================

static err_t FOG_CDECL EventLoop_postTask(EventLoop* self, Task* task, bool nestable, uint32_t delay)
{
  EventLoopImpl* d = self->_d;
  
  if (d == NULL)
    return ERR_RT_INVALID_STATE;

  return d->postTask(task, nestable, delay);
}

// ============================================================================
// [Fog::EventLoop - Copy]
// ============================================================================

static err_t FOG_CDECL EventLoop_copy(EventLoop* self, const EventLoop* other)
{
  EventLoopImpl* d = other->_d;
  
  if (d)
    d = d->addRef();

  atomicPtrXchg(&self->_d, d);

  if (d)
    d->release();
    
  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void EventLoop_init(void)
{
  fog_api.eventloop_ctor = EventLoop_ctor;
  fog_api.eventloop_ctorCopy = EventLoop_ctorCopy;
  fog_api.eventloop_dtor = EventLoop_dtor;

  fog_api.eventloop_getType = EventLoop_getType;
  fog_api.eventloop_getDepth = EventLoop_getDepth;

  fog_api.eventloop_isObservable = EventLoop_isObservable;
  fog_api.eventloop_addObserver = EventLoop_addObserver;
  fog_api.eventloop_removeObserver = EventLoop_removeObserver;

  fog_api.eventloop_run = EventLoop_run;
  fog_api.eventloop_runAllPending = EventLoop_runAllPending;
  fog_api.eventloop_quit = EventLoop_quit;
  fog_api.eventloop_postTask = EventLoop_postTask;

  fog_api.eventloop_copy = EventLoop_copy;
}

} // Fog namespace
