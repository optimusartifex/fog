// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Threading/Lock.h>

Fog::Static<Fog::Lock> fog_once_lock;

namespace Fog {

// ============================================================================
// [Fog::Lock (Windows)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static void FOG_CDECL Lock_ctor(Lock* self)
{
  ::InitializeCriticalSection(&self->_handle);
  // ::InitializeCriticalSectionAndSpinCount(&self->_handle, 2000);
}

static void FOG_CDECL Lock_dtor(Lock* self)
{
  ::DeleteCriticalSection(&self->_handle);
}

static bool FOG_STDCALL Lock_tryLock(Lock* self)
{
  return TryEnterCriticalSection(&self->_handle) != 0;
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::Lock (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)
static FOG_CVAR_DECLARE pthread_mutexattr_t Lock_recursiveAttributes;

static void FOG_CDECL Lock_ctor(Lock* self)
{
  pthread_mutex_init(&self->_handle, &Lock_recursiveAttributes);
}

static void FOG_CDECL Lock_dtor(Lock* self)
{
  pthread_mutex_destroy(&self->_handle);
}

static bool FOG_CDECL Lock_tryLock(Lock* self)
{
  return pthread_mutex_trylock(&self->_handle) == 0;
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Lock_init(void)
{
  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  typedef void (FOG_STDCALL *WinLockFunc)(Lock* self);
  typedef void (FOG_STDCALL *WinUnlockFunc)(Lock* self);

  fog_api.lock_ctor = Lock_ctor;
  fog_api.lock_dtor = Lock_dtor;
  fog_api.lock_lock = (WinLockFunc)EnterCriticalSection;
  fog_api.lock_tryLock = Lock_tryLock;
  fog_api.lock_unlock = (WinUnlockFunc)LeaveCriticalSection;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  typedef void (FOG_CDECL *PosixLockFunc)(Lock* self);
  typedef void (FOG_CDECL *PosixUnlockFunc)(Lock* self);

  fog_api.lock_ctor = Lock_ctor;
  fog_api.lock_dtor = Lock_dtor;
  fog_api.lock_lock = (PosixLockFunc)pthread_mutex_lock;
  fog_api.lock_tryLock = Lock_tryLock;
  fog_api.lock_unlock = (PosixUnlockFunc)pthread_mutex_unlock;
#endif // FOG_OS_POSIX

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_POSIX)
  pthread_mutexattr_init(&Lock_recursiveAttributes);
  pthread_mutexattr_settype(&Lock_recursiveAttributes, PTHREAD_MUTEX_RECURSIVE);
#endif // FOG_OS_POSIX

  fog_once_lock.init();
}

FOG_NO_EXPORT void Lock_fini(void)
{
  fog_once_lock.destroy();

#if defined(FOG_OS_POSIX)
  pthread_mutexattr_destroy(&Lock_recursiveAttributes);
#endif // FOG_OS_POSIX
}

} // Fog namespace
