// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/OS/OS.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/ByteArrayTmp_p.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/TextCodec.h>

#if defined(FOG_OS_POSIX)
#include <errno.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif // FOG_OS_POSIX

#if defined(FOG_OS_MAC)
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#endif // FOG_OS_MAC

// [Fog::System - Windows stuff]
#if defined(FOG_OS_WINDOWS) && !defined(PATH_MAX)
#define PATH_MAX 1024
#endif // FOG_OS_WINDOWS && !defined(PATH_MAX)

namespace Fog {

// ============================================================================
// [Fog::OS]
// ============================================================================

struct OS_Local
{
#if defined(FOG_OS_WINDOWS)
  uint32_t windowsVersion;
  String windowsName;
  String windowsVersionString;
  String windowsDirectory;
#endif // FOG_OS_WINDOWS

  OS_Local()
  {
    // [Windows Specific]
#if defined(FOG_OS_WINDOWS)

    // [Get Windows Version]

    windowsName.set(Ascii8("Windows"));
    windowsVersion = 0;

    OSVERSIONINFOW osVersion;
    ZeroMemory(&osVersion, sizeof(OSVERSIONINFOW));
    osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    GetVersionExW(&osVersion);

    switch (osVersion.dwPlatformId)
    {
      case VER_PLATFORM_WIN32_NT:
        windowsName.set(Ascii8("WinNT"));
        windowsVersion |= OS::WIN_VERSION_NT;

        // Windows NT 3.0
        if (osVersion.dwMajorVersion == 3)
        {
          windowsVersion |= OS::WIN_VERSION_NT_3;
        }
        // Windows NT 4.0
        else if (osVersion.dwMajorVersion == 4)
        {
          windowsVersion |= OS::WIN_VERSION_NT_4;
        }
        // Windows 2000, XP or 2003
        else if (osVersion.dwMajorVersion == 5)
        {
          if (osVersion.dwMinorVersion == 0)
          {
            windowsName.set(Ascii8("Win2000"));
            windowsVersion |= OS::WIN_VERSION_2000;
          }
          else if (osVersion.dwMinorVersion == 1)
          {
            windowsName.set(Ascii8("WinXP"));
            windowsVersion |= OS::WIN_VERSION_XP;
          }
          else if (osVersion.dwMinorVersion == 2)
          {
            windowsName.set(Ascii8("Win2003"));
            windowsVersion |= OS::WIN_VERSION_2003;
          }
        }
        // Windows Vista
        else if (osVersion.dwMajorVersion == 6)
        {
          windowsName.set(Ascii8("WinVista"));
          windowsVersion |= OS::WIN_VERSION_VISTA;
        }
        break;
    }

    windowsVersionString.format("%u.%u",
      (uint32_t)osVersion.dwMajorVersion,
      (uint32_t)osVersion.dwMinorVersion);

    // [Get Windows Directory]

    WCHAR winDir[PATH_MAX];
    UINT result = GetWindowsDirectoryW(winDir, PATH_MAX);

    if (!result || result > PATH_MAX)
    {
      windowsDirectory.set(Ascii8("C:/WINDOWS"));
    }
    else
    {
      windowsDirectory.set(reinterpret_cast<const Char*>(winDir));
      windowsDirectory.slashesToPosix();
    }
#endif // FOG_OS_WINDOWS
  }

  ~OS_Local()
  {
  }
};

static Static<OS_Local> _core_os_local;

String OS::getName()
{
  String result;

#if defined(FOG_OS_WINDOWS)
  result = _core_os_local->windowsName;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  utsname info;
  if (uname(&info) >= 0) result.set(Ascii8(info.sysname));
#endif // FOG_OS_POSIX

  return result;
}

String OS::getVersion()
{
  String result;

#if defined(FOG_OS_WINDOWS)
  result = _core_os_local->windowsVersionString;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  utsname info;
  if (uname(&info) >= 0) TextCodec::local8().decode(result, Stub8(info.release, DETECT_LENGTH));
#endif // FOG_OS_POSIX

  return result;
}

uint64_t OS::getAmountOfPhysicalMemory()
{
#if defined(FOG_OS_WINDOWS)
  MEMORYSTATUSEX meminfo;

  meminfo.dwLength = sizeof(meminfo);
  if (!GlobalMemoryStatusEx(&meminfo))
  {
    FOG_ASSERT(0);
    return 0;
  }

  return static_cast<uint64_t>(meminfo.ullTotalPhys);
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
// _SC_PHYS_PAGES is not part of POSIX and not available on OS X
#if defined(FOG_OS_MAC)
  struct host_basic_info hostinfo;
  mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;
  int result = host_info(mach_host_self(),
                         HOST_BASIC_INFO,
                         reinterpret_cast<host_info_t>(&hostinfo),
                         &count);

  FOG_ASSERT(count == HOST_BASIC_INFO_COUNT);

  if (result != KERN_SUCCESS)
  {
    FOG_ASSERT(0);
    return 0;
  }

  return static_cast<uint64_t>(hostinfo.max_mem);
#else
  long pages = sysconf(_SC_PHYS_PAGES);
  long page_size = sysconf(_SC_PAGE_SIZE);

  if (pages == -1 || page_size == -1)
  {
    FOG_ASSERT(0);
    return 0;
  }

  return ((uint64_t)pages * (uint64_t)page_size);
#endif
#endif // FOG_OS_POSIX
}

uint32_t OS::getAmountOfPhysicalMemoryMB()
{
  return static_cast<uint32_t>(getAmountOfPhysicalMemory() / 1048576);
}

#if defined (FOG_OS_WINDOWS)
uint32_t OS::getWindowsVersion()
{
  return _core_os_local->windowsVersion;
}

String OS::getWindowsDirectory()
{
  return _core_os_local->windowsDirectory;
}
#endif // FOG_OS_WINDOWS

err_t OS::getEnv(const String& name, String& value)
{
#if defined(FOG_OS_WINDOWS)
  DWORD sz;

  err_t err;
  if ((err = value.prepare(256))) return err;

  for (;;)
  {
    sz = GetEnvironmentVariableW(reinterpret_cast<const wchar_t*>(name.getData()),
      reinterpret_cast<wchar_t*>(value.getDataX()), (uint32_t)value.getCapacity());

    if (sz == 0)
    {
      return ERR_ENV_GET_FAILED;
    }
    else if (sz > value.getCapacity())
    {
      if ((err = value.prepare(sz)) == ERR_OK) continue;
      return err;
    }
    else if (sz)
    {
      return value.resize(sz);
    }
  }
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  ByteArrayTmp<TEMPORARY_LENGTH> name8;

  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(name8, name));

  const char* e = getenv(name8.getData());
  if (e)
  {
    return TextCodec::local8().decode(value, Stub8(e, DETECT_LENGTH));
  }
  else
  {
    value.clear();
    return ERR_ENV_GET_FAILED;
  }
#endif
}

err_t OS::setEnv(const String& name, const String& value)
{
#if defined(FOG_OS_WINDOWS)
  if (SetEnvironmentVariableW(reinterpret_cast<const wchar_t*>(name.getData()),
    reinterpret_cast<const wchar_t*>(value.getData())))
  {
    return ERR_OK;
  }
  else
  {
    return ERR_ENV_SET_FAILED;
  }
#endif

#if defined(FOG_OS_POSIX)
  ByteArrayTmp<TEMPORARY_LENGTH> name8;
  ByteArrayTmp<TEMPORARY_LENGTH> value8;

  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(name8, name));
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(value8, value));

  int result;
  if (value8.isEmpty())
    result = unsetenv(name8.getData());
  else
    result = setenv(name8.getData(), value8.getData(), 1);

  if (result != 0)
    return ERR_ENV_SET_FAILED;
  else
    return ERR_OK;
#endif
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OS_init(void)
{
  _core_os_local.init();
}

FOG_NO_EXPORT void OS_fini(void)
{
  _core_os_local.destroy();
}

} // Fog namespace
