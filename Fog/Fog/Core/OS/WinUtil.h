// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_WINUTIL_H
#define _FOG_CORE_OS_WINUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::WinVersion]
// ============================================================================

struct FOG_NO_EXPORT WinVersion
{
  uint32_t version;

  uint32_t major;
  uint32_t minor;
  uint32_t build;
};

// ============================================================================
// [Fog::WinUtil]
// ============================================================================

struct FOG_NO_EXPORT WinUtil
{
  static FOG_INLINE err_t getErrFromWinErrorCode(DWORD code)
  {
    return _api.winutil_getErrFromWinErrorCode(code);
  }

  static FOG_INLINE err_t getErrFromWinLastError()
  {
    return _api.winutil_getErrFromWinLastError();
  }

  static FOG_INLINE uint32_t getWinVersion()
  {
    return _api.winutil_getWinVersion(NULL);
  }

  static FOG_INLINE uint32_t getWinVersion(WinVersion& version)
  {
    return _api.winutil_getWinVersion(&version);
  }

  static FOG_INLINE err_t getWinDirectory(StringW& dst)
  {
    return _api.winutil_getWinDirectory(&dst);
  }

  static FOG_INLINE err_t getModuleFileName(StringW& dst, HMODULE hModule)
  {
    return _api.winutil_getModuleFileName(&dst, hModule);
  }

  static FOG_INLINE err_t makeWinPath(StringW& dst, const StringW& src)
  {
    return _api.winutil_makeWinPath(&dst, &src);
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_WINUTIL_H
