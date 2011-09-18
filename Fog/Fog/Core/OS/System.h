// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_SYSTEM_H
#define _FOG_CORE_OS_SYSTEM_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {
namespace System {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::System - Info]
// ============================================================================

static FOG_INLINE StringW getSystemName()
{
  StringW result;
  _api.system.getSystemName(&result);
  return result;
}

static FOG_INLINE StringW getSystemVersion()
{
  StringW result;
  _api.system.getSystemVersion(&result);
  return result;
}

// ============================================================================
// [Fog::System - Info (Windows)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static FOG_INLINE uint32_t getWindowsVersion()
{
  return _api.system.getWindowsVersion();
}

static FOG_INLINE StringW getWindowsDirectory()
{
  StringW result;
  _api.system.getWindowsDirectory(&result);
  return result;
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::System - Memory]
// ============================================================================

static FOG_INLINE uint64_t getAmountOfPhysicalMemory()
{
  return _api.system.getAmountOfPhysicalMemory();
}

static FOG_INLINE uint64_t getAmountOfPhysicalMemoryMB()
{
  return _api.system.getAmountOfPhysicalMemoryMB();
}

// ============================================================================
// [Fog::System - Environment]
// ============================================================================

static FOG_INLINE err_t getEnvironment(const Ascii8& key, StringW& value)
{
  return _api.system.getEnvironmentStubA(&key, &value);
}

static FOG_INLINE err_t getEnvironment(const StringW& key, StringW& value)
{
  return _api.system.getEnvironmentStringW(&key, &value);
}

static FOG_INLINE err_t setEnvironment(const Ascii8& key, const StringW& value)
{
  return _api.system.setEnvironmentStubA(&key, &value);
}

static FOG_INLINE err_t setEnvironment(const StringW& key, const StringW& value)
{
  return _api.system.setEnvironmentStringW(&key, &value);
}

// ============================================================================
// [Fog::System - Error]
// ============================================================================

static FOG_INLINE err_t errorFromOSErrorCode(int code)
{
  return _api.system.errorFromOSErrorCode(code);
}

static FOG_INLINE err_t errorFromOSLastError()
{
  return _api.system.errorFromOSLastError();
}

// ============================================================================
// [Fog::System - Windows Extras]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static FOG_INLINE err_t getModuleFileName(StringW& dst, HMODULE hModule)
{
  return _api.system.getModuleFileName(&dst, hModule);
}

static FOG_INLINE err_t makeWindowsPath(StringW& dst, const StringW& src)
{
  return _api.system.makeWindowsPath(&dst, &src);
}
#endif // FOG_OS_WINDOWS

//! @}

} // System namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_SYSTEM_H
