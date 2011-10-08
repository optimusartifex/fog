// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_LOCK_H
#define _FOG_CORE_THREADING_LOCK_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

#if defined(FOG_OS_POSIX)
#include <pthread.h>
#endif // FOG_OS_POSIX

#if defined(FOG_OS_POSIX)
FOG_CVAR_EXTERN pthread_mutexattr_t fog_lock_recursive_attrs;
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Fog::Lock]
// ============================================================================

//! @brief Lock.
struct FOG_NO_EXPORT Lock
{
  // --------------------------------------------------------------------------
  // [Windows Support]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  typedef CRITICAL_SECTION Handle;

  FOG_INLINE Lock()
  {
    InitializeCriticalSection(&_handle);
    // InitializeCriticalSectionAndSpinCount(&_handle, 2000);
  }

  FOG_INLINE ~Lock()
  {
    DeleteCriticalSection(&_handle);
  }

  FOG_INLINE void lock()
  {
    EnterCriticalSection(&_handle);
  }

  FOG_INLINE void unlock()
  {
    LeaveCriticalSection(&_handle);
  }

  FOG_INLINE bool tryLock()
  {
    return TryEnterCriticalSection(&_handle) != 0;
  }
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Posix Support]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_POSIX)
  typedef pthread_mutex_t Handle;

  FOG_INLINE Lock()
  {
    pthread_mutex_init(&_handle, &fog_lock_recursive_attrs);
  }

  FOG_INLINE ~Lock()
  {
    pthread_mutex_destroy(&_handle);
  }

  FOG_INLINE void lock()
  {
    pthread_mutex_lock(&_handle);
  }

  FOG_INLINE void unlock()
  {
    pthread_mutex_unlock(&_handle);
  }

  FOG_INLINE bool tryLock()
  {
    return pthread_mutex_trylock(&_handle) != 0;
  }
#endif // FOG_OS_POSIX

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Handle& getHandle()
  {
    return _handle;
  }

  FOG_INLINE const Handle& getHandle() const
  {
    return _handle;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Handle _handle;

private:
  _FOG_NO_COPY(Lock)
};

// ============================================================================
// [Fog::AutoLock]
// ============================================================================

//! @brief Auto @c Lock locker.
//!
//! @c AutoLock is simple class that locks a @c Lock and unlocks it when the
//! instance of class is being destroyed. It's designed to simplify the code 
//! where it's needed to lock a @c Lock on begin of a function and unlock it at
//! the end.
//!
//! This very simple code shows how @c AutoLock works:
//!
//! @verbatim
//! static Fog::Lock lock;
//!
//! void lockedFunction(void)
//! {
//!   Fog::AutoLock locked(lock);
//!   // Now mutex is locked
//!
//!   // some code here ...
//!
//!   // return; (Return is not necessary, it's only example)
//!   // After return the lock will be unlocked
//! }
//! @endverbatim
//!
//! @sa @c Lock, @c AutoUnlock
struct FOG_NO_EXPORT AutoLock
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Creates an instance of @c Locker<Lock> and locks it.
  FOG_INLINE AutoLock(Lock& target)
  {
    _target = &target;
    _target->lock();
  }

  //! @brief Creates an instance of @c Locker<Lock> and locks a it.
  FOG_INLINE AutoLock(Lock* target)
  {
    _target = target;
    _target->lock();
  }

  //! @brief Destructor that unlocks locked mutex.
  FOG_INLINE ~AutoLock()
  {
    _target->unlock();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Pointer to a locked mutex.
  Lock* _target;

private:
  _FOG_NO_COPY(AutoLock)
};

// ============================================================================
// [Fog::AutoUnlock]
// ============================================================================

//! @brief Opposite to @c AutoLock.
//!
//! @sa @c Lock, @c AutoLock
struct FOG_NO_EXPORT AutoUnlock
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Creates an instance of @c Locker<Lock> and unlocks it.
  FOG_INLINE AutoUnlock(Lock& target)
  {
    _target = &target;
    _target->unlock();
  }

  //! @brief Creates an instance of @c Locker<Lock> and unlocks a it.
  FOG_INLINE AutoUnlock(Lock* target)
  {
    _target = target;
    _target->unlock();
  }

  //! @brief Destructor that locks unlocked mutex.
  FOG_INLINE ~AutoUnlock()
  {
    _target->lock();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Pointer to a unlocked mutex.
  Lock* _target;

private:
  _FOG_NO_COPY(AutoUnlock)
};

//! @}

} // Fog namespace

//! @brief Lock used when you need to initialize something only once.
//!
//! @sa @c FOG_ONCE_LOCK, @c FOG_ONCE_UNLOCK.
extern FOG_API Fog::Static<Fog::Lock> fog_once_lock;

#define FOG_ONCE_LOCK() fog_once_lock->lock()
#define FOG_ONCE_UNLOCK() fog_once_lock->unlock()

// [Guard]
#endif // _FOG_CORE_THREADING_LOCK_H
