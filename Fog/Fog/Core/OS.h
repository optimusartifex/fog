// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_H
#define _FOG_CORE_OS_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/String.h>

namespace Fog {

// ============================================================================
// [Fog::OS]
// ============================================================================

//! @brief Contains useful functions for getting system information.
namespace OS {

//! @addtogroup Fog_Core_Environment
//! @{

//! @brief Get the name of the host operating system.
FOG_API String getName();

//! @brief Get the version of the host operating system.
FOG_API String getVersion();

//! @brief Get the number of bytes of physical memory on the current machine.
FOG_API uint64_t getAmountOfPhysicalMemory();

//! @brief Get the number of megabytes of physical memory on the current machine.
FOG_API uint32_t getAmountOfPhysicalMemoryMB();

#if defined(FOG_OS_WINDOWS)
enum WIN_VERSION
{
  WIN_VERSION_UNKNOWN    = 0,

  //! @brief Set if system is WinNT based (can't be together with @c WIN_VERSION_98).
  WIN_VERSION_NT         = 0x0001,
  //! @brief Set if system is WinNT 4.x.
  WIN_VERSION_NT_3       = 0x0002,
  //! @brief Set if system is WinNT 4.x.
  WIN_VERSION_NT_4       = 0x0004,
  //! @brief Set if system is Windows 2000.
  WIN_VERSION_2000       = 0x0008,
  //! @brief Set if system is Windows XP.
  WIN_VERSION_XP         = 0x0010,
  //! @brief Set if system is Windows 2003.
  WIN_VERSION_2003       = 0x0020,
  //! @brief Set if system is Windows Vista.
  WIN_VERSION_VISTA      = 0x0040
};

FOG_API uint32_t getWindowsVersion();
FOG_API String getWindowsDirectory();
#endif // FOG_OS_WINDOWS

FOG_API err_t getEnv(const String& name, String& value);
FOG_API err_t setEnv(const String& name, const String& value);

//! @}

} // OS namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_H
