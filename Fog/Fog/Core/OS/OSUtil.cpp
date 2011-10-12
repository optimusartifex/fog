// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/OS/OSUtil.h>

// [Dependencies - C]
#include <errno.h>

namespace Fog {

// ============================================================================
// [Fog::OSUtil - LibC]
// ============================================================================

static err_t FOG_CDECL OSUtil_getErrFromLibCErrorCode(int code)
{
  // For LibC error codes portability, see:
  //   http://www.ioplex.com/~miallen/errcmp.html

  switch (code)
  {
    case 0: return ERR_OK;

#if defined(EPERM)
    case EPERM: return ERR_RT_NOT_PERMITTED;
#endif // EPERM

#if defined(ENOENT)
    case ENOENT: return ERR_PATH_NOT_FOUND;
#endif // ENOENT

#if defined(ESRCH)
    case ESRCH: return ERR_PROCESS_NOT_FOUND;
#endif // ESRCH

#if defined(EINTR)
    case EINTR: return ERR_UNKNOWN;
#endif // EINTR

#if defined(EIO)
    case EIO: return ERR_DEVICE_FAILURE;
#endif // EIO

#if defined(ENXIO)
    case ENXIO: return ERR_DEVICE_NOT_FOUND;
#endif // ENXIO

#if defined(E2BIG)
    case E2BIG: return ERR_PROCESS_ARGUMENTS_TOO_LONG;
#endif // E2BIG

#if defined(ENOEXEC)
    case ENOEXEC: return ERR_NOT_AN_EXECUTABLE;
#endif // ENOEXEC

#if defined(EBADF)
    case EBADF: return ERR_RT_INVALID_HANDLE;
#endif // EBADF

#if defined(ECHILD)
    case ECHILD: return ERR_NO_CHILD_PROCESS;
#endif // ECHILD

#if defined(EAGAIN)
    case EAGAIN: return ERR_RT_BUSY;
#endif // EAGAIN

#if defined(ENOMEM)
    case ENOMEM: return ERR_RT_OUT_OF_MEMORY;
#endif // ENOMEM

#if defined(EACCES)
    case EACCES: return ERR_RT_ACCESS_DENIED;
#endif // EACCES

#if defined(EFAULT)
    case EFAULT: return ERR_RT_INVALID_ARGUMENT;
#endif // EFAULT

#if defined(ENOTBLK)
    case ENOTBLK: return ERR_NOT_A_BLOCK_DEVICE;
#endif // ENOTBLK

#if defined(EBUSY)
    case EBUSY: return ERR_RT_BUSY;
#endif // EBUSY

#if defined(EEXIST)
    case EEXIST: return ERR_PATH_EXISTS;
#endif // EEXIST

#if defined(EXDEV)
    case EXDEV: return ERR_DEVICE_MISMATCH;
#endif // EXDEV

#if defined(ENODEV)
    case ENODEV: return ERR_DEVICE_NOT_FOUND;
#endif // ENODEV

#if defined(ENOTDIR)
    case ENOTDIR: return ERR_NOT_A_DIRECTORY;
#endif // ENOTDIR

#if defined(EISDIR)
    case EISDIR: return ERR_NOT_A_FILE;
#endif // EISDIR

#if defined(EINVAL)
    case EINVAL: return ERR_RT_INVALID_ARGUMENT;
#endif // EINVAL

#if defined(ENFILE)
    case ENFILE: return ERR_TOO_MANY_FILES;
#endif // ENFILE

#if defined(EMFILE)
    case EMFILE: return ERR_TOO_MANY_FILES;
#endif // EMFILE

#if defined(ENOTTY)
    case ENOTTY: return ERR_INVALID_IOCTL;
#endif // ENOTTY

#if defined(ETXTBSY)
    case ETXTBSY: return ERR_RT_BUSY;
#endif // ETXTBSY

#if defined(EFBIG)
    case EFBIG: return ERR_FILE_TOO_LARGE;
#endif // EFBIG

#if defined(ENOSPC)
    case ENOSPC: return ERR_DEVICE_FULL;
#endif // ENOSPC

#if defined(ESPIPE)
    case ESPIPE: return ERR_NON_SEEKABLE;
#endif // ESPIPE

#if defined(EROFS)
    case EROFS: return ERR_DEVICE_READ_ONLY;
#endif // EROFS

#if defined(EMLINK)
    case EMLINK: return ERR_TOO_MANY_LINKS;
#endif // EMLINK

#if defined(EPIPE)
    case EPIPE: return ERR_PIPE_END;
#endif // EPIPE

#if defined(EDOM)
    case EDOM: return ERR_RT_NOT_A_NUMBER;
#endif // EDOM

#if defined(ERANGE)
    case ERANGE: return ERR_RT_OVERFLOW;
#endif // ERANGE

//#if defined(ENOMSG)
//    case ENOMSG: return ERR_UNKNOWN;
//#endif // ENOMSG

//#if defined(EIDRM)
//    case EIDRM: return ERR_UNKNOWN;
//#endif // EIDRM

#if defined(EDEADLK)
    case EDEADLK: return ERR_RT_DEADLOCK;
#endif // EDEADLK

#if defined(ENOTREADY)
    case ENOTREADY: return ERR_DEVICE_NOT_READY;
#endif // ENOTREADY

#if defined(EWRPROTECT)
    case EWRPROTECT: return ERR_DEVICE_READ_ONLY;
#endif // EWRPROTECT

#if defined(ENOLCK)
    case ENOLCK: return ERR_RT_OUT_OF_LOCKS;
#endif // ENOLCK

#if defined(ENOSYS)
    case ENOSYS: return ERR_RT_NOT_IMPLEMENTED;
#endif // ENOSYS

#if defined(EDEADLOCK) && !defined(EDEADLK)
    case EDEADLOCK: return ERR_RT_DEADLOCK;
#endif // EDEADLOCK && !EDEADLK

#if defined(EILSEQ)
    case EILSEQ: return ERR_STRING_INVALID_INPUT;
#endif // EILSEQ

#if defined(EOVERFLOW)
    case EOVERFLOW: return ERR_RT_OVERFLOW;
#endif // EOVERFLOW

#if defined(ENOMEDIUM)
    case ENOMEDIUM: return ERR_DEVICE_NOT_READY;
#endif // ENOMEDIUM

    default:
      return (err_t)code;
  }
}

static err_t FOG_CDECL OSUtil_getErrFromLibCErrno(void)
{
  return _api.osutil_getErrFromLibCErrorCode(errno);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OSUtil_init(void)
{
  _api.osutil_getErrFromLibCErrorCode = OSUtil_getErrFromLibCErrorCode;
  _api.osutil_getErrFromLibCErrno = OSUtil_getErrFromLibCErrno;

#if defined(FOG_OS_WINDOWS)
  // Must be initialized before OSUtil_init() is called.
  FOG_ASSERT(_api.winutil_getErrFromWinErrorCode != NULL);
  FOG_ASSERT(_api.winutil_getErrFromWinLastError != NULL);

  typedef err_t (FOG_CDECL* GetErrFromOSErrorCodeFunc)(int code);

  // Map WinUtil error management to OSUtil.
  _api.osutil_getErrFromOSErrorCode = (GetErrFromOSErrorCodeFunc)_api.winutil_getErrFromWinErrorCode;
  _api.osutil_getErrFromOSLastError = _api.winutil_getErrFromWinLastError;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  // Map LibC error management to OSUtil.
  _api.osutil_getErrFromOSErrorCode = OSUtil_getErrFromLibCErrorCode;
  _api.osutil_getErrFromOSLastError = OSUtil_getErrFromLibCErrno;
#endif // FOG_OS_POSIX
}

} // Fog namespace
