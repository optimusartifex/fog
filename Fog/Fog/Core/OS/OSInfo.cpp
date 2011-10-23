// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
# include <Fog/Core/OS/WinUtil.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <errno.h>
# include <sys/utsname.h>
# include <unistd.h>
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::OSInfo]
// ============================================================================

static err_t FOG_CDECL OSInfo_getName(StringW* dst)
{
#if defined(FOG_OS_WINDOWS)
  WinVersion versionData;
  const char* versionString = "Windows (Future)";

  switch (WinUtil::getWinVersion(versionData))
  {
    case WIN_VERSION_UNKNOWN : versionString = "Windows (Unknown)"     ; break;
    case WIN_VERSION_NT      : versionString = "Windows NT"            ; break;
    case WIN_VERSION_NT_3    : versionString = "Windows NT 3.0"        ; break;
    case WIN_VERSION_NT_4    : versionString = "Windows NT 4.0"        ; break;
    case WIN_VERSION_2000    : versionString = "Windows 2000"          ; break;
    case WIN_VERSION_XP      : versionString = "Windows XP"            ; break;
    case WIN_VERSION_2003    : versionString = "Windows Server 2003"   ; break;
    case WIN_VERSION_2003_R2 : versionString = "Windows Server 2003 R2"; break;
    case WIN_VERSION_VISTA   : versionString = "Windows Vista"         ; break;
    case WIN_VERSION_2008    : versionString = "Windows Server 2008"   ; break;
    case WIN_VERSION_2008_R2 : versionString = "Windows Server 2008 R2"; break;
    case WIN_VERSION_7       : versionString = "Windows 7"             ; break;
  }

  return dst->format("%s %u.%u.%u",
    versionString,
    versionData.major,
    versionData.minor,
    versionData.build);
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  utsname info;
  if (uname(&info) < 0)
  {
    dst->clear();
    return OSUtil::getErrFromLibCErrno();
  }

  return dst->format("%s %s", info.sysname, info.release);
#endif // FOG_OS_POSIX
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OSInfo_init(void)
{
  fog_api.osinfo_getName = OSInfo_getName;
}

} // Fog namespace
