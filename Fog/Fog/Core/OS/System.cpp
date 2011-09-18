// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/OS/System.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - POSIX]
#if defined(FOG_OS_POSIX)
# include <errno.h>
# include <stdlib.h>
# include <string.h>
# include <sys/statvfs.h>
# include <sys/utsname.h>
# include <unistd.h>
#endif // FOG_OS_POSIX

// [Dependencies - MAC]
#if defined(FOG_OS_MAC)
# include <mach/mach_host.h>
# include <mach/mach_init.h>
#endif // FOG_OS_MAC

namespace Fog {

// ============================================================================
// [Fog::System - Helpers (Windows)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

#if !defined(PATH_MAX)
# define PATH_MAX 1024
#endif // PATH_MAX

#if !defined(SM_SERVERR2)
# define SM_SERVERR2 89
#endif // SM_SERVERR2

struct FOG_NO_EXPORT SystemWindowsInfo
{
  uint32_t version;

  uint32_t msMajorVersion;
  uint32_t msMinorVersion;
  uint32_t msBuildNumber;
};
static SystemWindowsInfo systemWindowsInfo;

static void System_getWindowsInfo()
{
  OSVERSIONINFOEXW os;

  ZeroMemory(&os, sizeof(OSVERSIONINFOEXW));
  os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
  GetVersionExW((OSVERSIONINFOW*)&os);

  systemWindowsInfo.version = OS_WIN_VERSION_UNKNOWN;
  systemWindowsInfo.msMajorVersion = os.dwMajorVersion;
  systemWindowsInfo.msMinorVersion = os.dwMinorVersion;
  systemWindowsInfo.msBuildNumber = os.dwBuildNumber;

  switch (os.dwPlatformId)
  {
    case VER_PLATFORM_WIN32_NT:
      systemWindowsInfo.version = OS_WIN_VERSION_NT;

      switch (os.dwMajorVersion)
      {
        case 0:
        case 1:
        case 2:
          break;

        // Windows NT 3.0
        case 3:
          systemWindowsInfo.version = OS_WIN_VERSION_NT_3;
          break;

        // Windows NT 4.0
        case 4:
          systemWindowsInfo.version = OS_WIN_VERSION_NT_4;
          break;

        // Windows 2000, XP or 2003
        case 5:
          systemWindowsInfo.version = OS_WIN_VERSION_2000;

          if (os.dwMinorVersion == 1)
          {
            systemWindowsInfo.version = OS_WIN_VERSION_XP;
          }

          if (os.dwMinorVersion == 2)
          {
            systemWindowsInfo.version = OS_WIN_VERSION_2003;

            if (::GetSystemMetrics(SM_SERVERR2) != 0)
              systemWindowsInfo.version = OS_WIN_VERSION_2003_R2;
          }
          break;

        // Windows Vista, Windows 7, Windows 2008.
        case 6:
          if (os.wProductType == VER_NT_WORKSTATION)
          {
            systemWindowsInfo.version = OS_WIN_VERSION_VISTA;

            if (os.dwMinorVersion == 1)
              systemWindowsInfo.version = OS_WIN_VERSION_7;

            if (os.dwMinorVersion > 1)
              goto _Future;
          }
          else
          {
            systemWindowsInfo.version = OS_WIN_VERSION_2008;

            if (os.dwMinorVersion == 1)
              systemWindowsInfo.version = OS_WIN_VERSION_2008_R2;

            if (os.dwMinorVersion > 1)
              goto _Future;
          }
          break;

        default:
_Future:
          systemWindowsInfo.version = OS_WIN_VERSION_FUTURE;
          break;
      }
      break;
  }
}

// ============================================================================
// [Fog::System - Info (Windows)]
// ============================================================================

static err_t FOG_CDECL System_getSystemName(StringW* dst)
{
  const char* versionString = "Windows (Future)";

  switch (systemWindowsInfo.version)
  {
    case OS_WIN_VERSION_UNKNOWN : versionString = "Windows (Unknown)"     ; break;
    case OS_WIN_VERSION_NT      : versionString = "Windows NT"            ; break;
    case OS_WIN_VERSION_NT_3    : versionString = "Windows NT 3.0"        ; break;
    case OS_WIN_VERSION_NT_4    : versionString = "Windows NT 4.0"        ; break;
    case OS_WIN_VERSION_2000    : versionString = "Windows 2000"          ; break;
    case OS_WIN_VERSION_XP      : versionString = "Windows XP"            ; break;
    case OS_WIN_VERSION_2003    : versionString = "Windows Server 2003"   ; break;
    case OS_WIN_VERSION_2003_R2 : versionString = "Windows Server 2003 R2"; break;
    case OS_WIN_VERSION_VISTA   : versionString = "Windows Vista"         ; break;
    case OS_WIN_VERSION_2008    : versionString = "Windows Server 2008"   ; break;
    case OS_WIN_VERSION_2008_R2 : versionString = "Windows Server 2008 R2"; break;
    case OS_WIN_VERSION_7       : versionString = "Windows 7"             ; break;
  }

  return dst->set(Ascii8(versionString, DETECT_LENGTH));
}

static err_t FOG_CDECL System_getSystemVersion(StringW* dst)
{
  return dst->format("%u.%u.%u",
    (uint)systemWindowsInfo.msMajorVersion,
    (uint)systemWindowsInfo.msMinorVersion,
    (uint)systemWindowsInfo.msBuildNumber);
}

static uint32_t FOG_CDECL System_getWindowsVersion()
{
  return systemWindowsInfo.version;
}

static err_t FOG_CDECL System_getWindowsDirectory(StringW* dst)
{
  CharW* buf = dst->_prepare(CONTAINER_OP_REPLACE, PATH_MAX);
  if (FOG_IS_NULL(buf))
    return ERR_RT_OUT_OF_MEMORY;

  UINT result = ::GetWindowsDirectoryW(reinterpret_cast<wchar_t*>(buf), PATH_MAX);
  if (result == 0 || result >= PATH_MAX)
    return dst->set(Ascii8("C:/Windows"));

  dst->_modified(buf + result);
  return dst->normalizeSlashes(SLASH_FORM_UNIX);
}

// ============================================================================
// [Fog::System - Memory (Windows)]
// ============================================================================

static uint64_t FOG_CDECL System_getAmountOfPhysicalMemory(void)
{
  MEMORYSTATUSEX meminfo;
  meminfo.dwLength = sizeof(meminfo);

  if (!GlobalMemoryStatusEx(&meminfo))
    return 0;

  return (uint64_t)meminfo.ullTotalPhys;
}

static uint32_t FOG_CDECL System_getAmountOfPhysicalMemoryMB(void)
{
  return (uint32_t)(System::getAmountOfPhysicalMemory() / 1048576);
}

// ============================================================================
// [Fog::System - Environment (Windows)]
// ============================================================================

static err_t FOG_CDECL System_getEnvironmentStubA(const StubA* key, StringW* value)
{
  StringTmpW<TEMPORARY_LENGTH> keyW;
  FOG_RETURN_ON_ERROR(keyW.setAscii8(*key));

  return _api.system.getEnvironmentStringW(&keyW, value);
}

static err_t FOG_CDECL System_getEnvironmentStringW(const StringW* key, StringW* value)
{
  if (key->getLength() == 0)
    return ERR_RT_INVALID_ARGUMENT;

  size_t capacity = 256;

  for (;;)
  {
    CharW* buf = value->_prepare(CONTAINER_OP_REPLACE, capacity);
    capacity = value->getCapacity();

    if (FOG_IS_NULL(buf))
      return ERR_RT_OUT_OF_MEMORY;

    size_t length = ::GetEnvironmentVariableW(
      reinterpret_cast<const wchar_t*>(key->getData()),
      reinterpret_cast<wchar_t*>(buf),
      static_cast<DWORD>(Math::min<size_t>(capacity, UINT32_MAX)));

    if (length == 0)
      return System::errorFromOSLastError();

    if (length > capacity)
    {
      capacity = length;
      continue;
    }

    value->_modified(buf + length);
    return ERR_OK;
  }
}

static err_t FOG_CDECL System_setEnvironmentStubA(const StubA* key, const StringW* value)
{
  StringTmpW<TEMPORARY_LENGTH> keyW;
  FOG_RETURN_ON_ERROR(keyW.setAscii8(*key));

  return _api.system.setEnvironmentStringW(&keyW, value);
}

static err_t FOG_CDECL System_setEnvironmentStringW(const StringW* key, const StringW* value)
{
  if (key->getLength() == 0)
    return ERR_RT_INVALID_ARGUMENT;

  if (::SetEnvironmentVariableW(
    reinterpret_cast<const wchar_t*>(key->getData()),
    reinterpret_cast<const wchar_t*>(value->getData())))
  {
    return ERR_OK;
  }
  else
  {
    return System::errorFromOSLastError();
  }
}

// ============================================================================
// [Fog::System - Error (Windows)]
// ============================================================================

static err_t FOG_CDECL System_errorFromOSErrorCode(int code)
{
  // TODO:
  return ERR_UNKNOWN;
}

static err_t FOG_CDECL System_errorFromOSLastError()
{
  return System::errorFromOSErrorCode(::GetLastError());
}

// ============================================================================
// [Fog::System - Extras (Windows)]
// ============================================================================

static err_t FOG_CDECL System_getModuleFileName(StringW* dst, HMODULE hModule)
{
  size_t capacity = 128;

  for (;;)
  {
    CharW* buf = dst->_prepare(CONTAINER_OP_REPLACE, capacity);
    capacity = dst->getCapacity();

    if (FOG_IS_NULL(buf))
      return ERR_RT_OUT_OF_MEMORY;

    size_t length = ::GetModuleFileNameW(hModule,
      reinterpret_cast<wchar_t*>(buf),
      static_cast<DWORD>(Math::min<size_t>(capacity + 1, UINT32_MAX)));

    if (length == 0)
      return System::errorFromOSLastError();

    if (length >= capacity)
    {
      capacity *= 2;
      continue;
    }

    dst->_modified(buf + length);
    return ERR_OK;
  }
}

static err_t FOG_CDECL System_makeWindowsPath(StringW* dst, const StringW* src)
{
  // TODO: Rewrite, not optimal, toAbsolutePath() should be modified to append
  // the path into *dst instead of replace it (use CONTAINER_OP_...)
  FOG_RETURN_ON_ERROR(FileSystem::toAbsolutePath(*dst, StringW(), *src));
  FOG_RETURN_ON_ERROR(dst->normalizeSlashes(SLASH_FORM_WINDOWS));
  FOG_RETURN_ON_ERROR(dst->prepend(Ascii8("\\\\?\\")));

  return ERR_OK;
}

#endif // FOG_OS_WINDOWS









// ============================================================================
// [Fog::System - Helpers (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)

// ============================================================================
// [Fog::System - Info (Posix)]
// ============================================================================

static err_t FOG_CDECL System_getSystemName(StringW* dst)
{
  utsname info;

  if (uname(&info) < 0)
  {
    dst->clear();
    return ERR_UNKNOWN;
  }

  return dst->set(Ascii8(info.sysname));
}

static err_t FOG_CDECL System_getSystemVersion(StringW* dst)
{
  utsname info;

  if (uname(&info) < 0)
  {
    dst->clear();
    return ERR_UNKNOWN;
  }

  return TextCodec::local8().decode(*dst, StubA(info.release, DETECT_LENGTH));
}

// ============================================================================
// [Fog::System - Memory (Posix)]
// ============================================================================

static uint64_t FOG_CDECL System_getAmountOfPhysicalMemory(void)
{
// _SC_PHYS_PAGES is not part of POSIX and not available on OS X.
#if defined(FOG_OS_MAC)
  struct host_basic_info hostinfo;
  mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;

  int result = host_info(mach_host_self(),
                         HOST_BASIC_INFO,
                         reinterpret_cast<host_info_t>(&hostinfo),
                         &count);
  FOG_ASSERT(count == HOST_BASIC_INFO_COUNT);

  if (result != KERN_SUCCESS)
    return 0;

  return static_cast<uint64_t>(hostinfo.max_mem);
#else
  long pages = sysconf(_SC_PHYS_PAGES);
  long pageSize = sysconf(_SC_PAGE_SIZE);

  if (pages == -1 || pageSize == -1)
    return 0;

  return (uint64_t)pages * (ulong)pageSize;
#endif
}

static uint32_t FOG_CDECL System_getAmountOfPhysicalMemoryMB(void)
{
  return (uint32_t)(System::getAmountOfPhysicalMemory() / 1048576);
}

// ============================================================================
// [Fog::System - Environment (Posix)]
// ============================================================================

static err_t System_getEnvironmentPrivate(const char* key, StringW* value)
{
  if (key[0] == 0)
    return ERR_RT_INVALID_ARGUMENT;

  const char* var = ::getenv(key);

  if (var == NULL)
  {
    value->clear();
    return System::errorFromOSLastError();
  }

  return TextCodec::local8().decode(*value, StubA(var, DETECT_LENGTH));

}

static err_t FOG_CDECL System_getEnvironmentStubA(const StubA* key, StringW* value)
{
  if (key->getLength() == DETECT_LENGTH || key->getData()[key->getLength()] == 0)
    return System_getEnvironmentPrivate(key->getData(), value);

  StringTmpA<TEMPORARY_LENGTH> keyA;
  FOG_RETURN_ON_ERROR(keyA.set(*key));

  return System_getEnvironmentPrivate(keyA.getData(), value);
}

static err_t FOG_CDECL System_getEnvironmentStringW(const StringW* key, StringW* value)
{
  StringTmpA<TEMPORARY_LENGTH> keyA;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(keyA, *key));

  return System_getEnvironmentPrivate(keyA.getData(), value);
}

static err_t System_setEnvironmentPrivate(const char* key, const StringW* value)
{
  if (key[0] == 0)
    return ERR_RT_INVALID_ARGUMENT;

  int result;

  if (value == NULL)
  {
    result = ::unsetenv(key);
  }
  else
  {
    StringTmpA<TEMPORARY_LENGTH> valueA;
    FOG_RETURN_ON_ERROR(TextCodec::local8().encode(valueA, *value));

    result = ::setenv(key, valueA.getData(), 1);
  }

  if (result != 0)
    return System::errorFromOSLastError();
  else
    return ERR_OK;
}

static err_t FOG_CDECL System_setEnvironmentStubA(const StubA* key, const StringW* value)
{
  if (key->getLength() == DETECT_LENGTH || key->getData()[key->getLength()] == 0)
    return System_setEnvironmentPrivate(key->getData(), value);

  StringTmpA<TEMPORARY_LENGTH> keyA;
  FOG_RETURN_ON_ERROR(keyA.set(*key));

  return System_setEnvironmentPrivate(keyA.getData(), value);
}

static err_t FOG_CDECL System_setEnvironmentStringW(const StringW* key, const StringW* value)
{
  StringTmpA<TEMPORARY_LENGTH> keyA;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(keyA, *key));

  return System_setEnvironmentPrivate(keyA.getData(), value);
}

// ============================================================================
// [Fog::System - Error (Posix)]
// ============================================================================

static err_t FOG_CDECL System_errorFromOSErrorCode(int code)
{
  // TODO:
  return ERR_UNKNOWN;
}

static err_t FOG_CDECL System_errorFromOSLastError()
{
  return System::errorFromOSErrorCode(errno);
}

#endif // FOG_OS_POSIX

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void System_init(void)
{
  _api.system.getSystemName = System_getSystemName;
  _api.system.getSystemVersion = System_getSystemVersion;

#if defined(FOG_OS_WINDOWS)
  _api.system.getWindowsVersion = System_getWindowsVersion;
  _api.system.getWindowsDirectory = System_getWindowsDirectory;
#endif // FOG_OS_WINDOWS

  _api.system.getAmountOfPhysicalMemory = System_getAmountOfPhysicalMemory;
  _api.system.getAmountOfPhysicalMemoryMB = System_getAmountOfPhysicalMemoryMB;

  _api.system.getEnvironmentStubA = System_getEnvironmentStubA;
  _api.system.getEnvironmentStringW = System_getEnvironmentStringW;

  _api.system.setEnvironmentStubA = System_setEnvironmentStubA;
  _api.system.setEnvironmentStringW = System_setEnvironmentStringW;

  _api.system.errorFromOSErrorCode = System_errorFromOSErrorCode;
  _api.system.errorFromOSLastError = System_errorFromOSLastError;

#if defined(FOG_OS_WINDOWS)
  _api.system.getModuleFileName = System_getModuleFileName;
  _api.system.makeWindowsPath = System_makeWindowsPath;
#endif // FOG_OS_WINDOWS
}

} // Fog namespace
