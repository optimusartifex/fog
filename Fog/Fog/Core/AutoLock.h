// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_AUTOLOCK_H
#define _FOG_CORE_AUTOLOCK_H

// [Dependencies]
#include <Fog/Build/Build.h>

#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Lock.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// ============================================================================
// [Fog::AutoLock]
// ============================================================================

//! @brief Auto @c Lock locker.
//!
//! Locker is simple template class that locks a critical section or mutex 
//! and unlocks it when instance of class is destroyed. It's designed to 
//! simplify code where we need to lock mutex on begin of function and 
//! unlock it on end. So, if we create a static instance of @c Fog::AutoLock,
//! C++ compiler will call destructor (unlock) for us at end of scope.
//!
//! This very simple code shows how @c Fog::AutoLock works:
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
//! @sa @c AutoUnlock, @c Lock
struct FOG_HIDDEN AutoLock
{
private:
  //! @brief Pointer to locked mutex.
  Lock* _target;

public:
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

private:
  FOG_DISABLE_COPY(AutoLock)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_AUTOLOCK_H
