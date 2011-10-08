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

#if defined(FOG_OS_POSIX)
#include <errno.h>
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::OSUtil - Error (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)
static err_t FOG_CDECL OSUtil_getErrFromOSErrorCode(int code)
{
  // TODO:
  return ERR_UNKNOWN;
}

static err_t FOG_CDECL OSUtil_getErrFromOSLastError()
{
  return OSUtil::getErrFromOSErrorCode(errno);
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OSUtil_init(void)
{
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
  _api.osutil_getErrFromOSErrorCode = OSUtil_getErrFromOSErrorCode;
  _api.osutil_getErrFromOSLastError = OSUtil_getErrFromOSLastError;
#endif // FOG_OS_POSIX
}

} // Fog namespace
