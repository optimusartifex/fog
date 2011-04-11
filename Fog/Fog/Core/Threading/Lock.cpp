// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Threading/Lock.h>

#if defined(FOG_OS_POSIX)
FOG_CVAR_DECLARE pthread_mutexattr_t fog_lock_recursive_attrs;
#endif // FOG_OS_POSIX

Fog::Static<Fog::Lock> fog_once_lock;

namespace Fog {

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_lock_init(void)
{
#if defined(FOG_OS_POSIX)
  pthread_mutexattr_init(&fog_lock_recursive_attrs);
  pthread_mutexattr_settype(&fog_lock_recursive_attrs, PTHREAD_MUTEX_RECURSIVE);
#endif // FOG_OS_POSIX

  fog_once_lock.init();
}

FOG_NO_EXPORT void _core_lock_fini(void)
{
  fog_once_lock.destroy();

#if defined(FOG_OS_POSIX)
  pthread_mutexattr_destroy(&fog_lock_recursive_attrs);
#endif // FOG_OS_POSIX
}

} // Fog namespace
