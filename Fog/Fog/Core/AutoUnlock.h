// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_AUTOUNLOCK_H
#define _FOG_CORE_AUTOUNLOCK_H

// [Dependencies]
#include <Fog/Build/Build.h>

#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Lock.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// ============================================================================
// [Fog::AutoUnlock]
// ============================================================================

//! @brief Auto @c Lock unlocker.
//!
//! @sa @c AutoLock, @c Lock
struct FOG_HIDDEN AutoUnlock
{
private:
  //! @brief Pointer to locked mutex.
  Lock* _target;

public:
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

private:
  FOG_DISABLE_COPY(AutoUnlock)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_AUTOUNLOCK_H
