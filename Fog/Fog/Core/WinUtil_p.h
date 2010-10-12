// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_WINUTIL_H
#define _FOG_CORE_WINUTIL_H

#include <Fog/Core/Build.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/String.h>

namespace Fog {

//! @brief Namespace that contains various functions related to Windows OS
//! development.
namespace WinUtil {

//! @addtogroup Fog_Core_Private
//! @{

FOG_HIDDEN err_t getModuleFileName(HMODULE hModule, String& dst);

//! @}

} // WinUtil namespace
} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
#endif // _FOG_CORE_WINUTIL_H
