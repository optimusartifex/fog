// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_THREADLOCAL_H
#define _FOG_CORE_THREADING_THREADLOCAL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <pthread.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Typedefs]
// ============================================================================

// Prototype for the TLS destructor, which can be optionally used to cleanup
// thread local storage on thread exit. The @c value argument is the data
// stored by thread local.
typedef void (*ThreadLocalDestructorFunc)(void* value);

// ============================================================================
// [Fog::ThreadLocal]
// ============================================================================

struct FOG_NO_EXPORT ThreadLocal
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ThreadLocal() : _slot(0) {}
  explicit FOG_INLINE ThreadLocal(_Uninitialized) {}

  FOG_INLINE ~ThreadLocal()
  {
    _api.threadlocal.destroy(_slot);
  }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return _slot != 0;
  }

  FOG_INLINE err_t create(ThreadLocalDestructorFunc dtor = NULL)
  {
    return _api.threadlocal.create(&_slot, (void*)dtor);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void* get() const
  {
    return _api.threadlocal.get(_slot);
  }

  FOG_INLINE err_t set(void* p)
  {
    return _api.threadlocal.set(_slot, p);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _slot;

private:
  _FOG_NO_COPY(ThreadLocal)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_THREADLOCAL_H
