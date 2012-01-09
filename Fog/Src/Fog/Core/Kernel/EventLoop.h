// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_EVENTLOOP_H
#define _FOG_CORE_KERNEL_EVENTLOOP_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Task.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Time.h>

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::EventLoopImpl]
// ============================================================================

struct FOG_NO_EXPORT EventLoop
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE EventLoop()
  {
    fog_api.eventloop_ctor(this);
  }

  FOG_INLINE EventLoop(const EventLoop& other)
  {
    fog_api.eventloop_ctorCopy(this, &other);
  }
  
  explicit FOG_INLINE EventLoop(EventLoopImpl* d) :
    _d(d)
  {
  }
  
  FOG_INLINE ~EventLoop()
  {
    fog_api.eventloop_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get whether the event loop is created.
  FOG_INLINE bool isCreated() const
  {
    return _d != NULL;
  }

  //! @brief Get event loop depth (how many nested event loops started).
  FOG_INLINE int getDepth() const
  {
    return fog_api.eventloop_getDepth(this);
  }

  //! @brief Get event loop type as @ref StringW.
  FOG_INLINE StringW getType() const 
  {
    StringW result;
    fog_api.eventloop_getType(this, &result);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Observer]
  // --------------------------------------------------------------------------

  //! @brief Get whether the event loop is observable.
  //!
  //! OBservable event loop is uses operating system native API to access the
  //! event loop features specific for the target platform. EventLoopObserver 
  //! can be in such case used before and after the event is dispatched by the
  //! event loop.
  FOG_INLINE bool isObservable() const
  {
    return fog_api.eventloop_isObservable(this);
  }

  //! @brief Add a native event listener, which will start receiving events
  //! immediately.
  FOG_INLINE err_t addObserver(EventLoopObserver* obj)
  {
    return fog_api.eventloop_addObserver(this, obj);
  }

  //! @brief Remove a native event listener. It is safe to call this method
  //! while a native event listener is receiving a callback.
  FOG_INLINE err_t removeObserver(EventLoopObserver* obj)
  {
    return fog_api.eventloop_removeObserver(this, obj);
  }

  // --------------------------------------------------------------------------
  // [Run / Quit]
  // --------------------------------------------------------------------------

  //! @brief Run event loop.
  FOG_INLINE err_t run()
  {
    return fog_api.eventloop_run(this);
  }

  //! Process all pending events, but don't wait/sleep.
  FOG_INLINE err_t runAllPending()
  {
    return fog_api.eventloop_runAllPending(this);
  }

  //! @brief Quit immediately from the most recently entered event loop.
  FOG_INLINE err_t quit()
  {
    return fog_api.eventloop_quit(this);
  }

  // --------------------------------------------------------------------------
  // [Post]
  // --------------------------------------------------------------------------

  //! @brief Post task to the event loop (FIFO).
  FOG_INLINE err_t postTask(Task* task, bool nestable = true, uint32_t delay = 0)
  {
    return fog_api.eventloop_postTask(this, task, nestable, delay);
  }

  // --------------------------------------------------------------------------
  // [Adopt]
  // --------------------------------------------------------------------------

  FOG_INLINE void adopt(EventLoopImpl* impl)
  {
    fog_api.eventloop_dtor(this);
    _d = impl;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.eventloop_dtor(this);
    _d = NULL;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE EventLoop& operator=(const EventLoop& other)
  {
    fog_api.eventloop_copy(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  EventLoopImpl* _d;
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_KERNEL_EVENTLOOP_H
