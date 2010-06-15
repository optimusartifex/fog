// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Static.h>

#if defined(FOG_OS_POSIX)
FOG_CVAR_DECLARE pthread_mutexattr_t fog_lock_recursive_attrs;
#endif // FOG_OS_POSIX

Fog::Static<Fog::Lock> fog_once_lock;

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_lock_init(void)
{
  using namespace Fog;

#if defined(FOG_OS_POSIX)
  pthread_mutexattr_init(&fog_lock_recursive_attrs);
  pthread_mutexattr_settype(&fog_lock_recursive_attrs, PTHREAD_MUTEX_RECURSIVE);
#endif // FOG_OS_POSIX

  fog_once_lock.init();
  return ERR_OK;
}

FOG_INIT_DECLARE void fog_lock_shutdown(void)
{
  using namespace Fog;

  fog_once_lock.destroy();

#if defined(FOG_OS_POSIX)
  pthread_mutexattr_destroy(&fog_lock_recursive_attrs);
#endif // FOG_OS_POSIX
}
