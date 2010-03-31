// [Fog-Core Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_WINUTIL_H
#define _FOG_CORE_WINUTIL_H

// [Dependencies]
#include <Fog/Build/Build.h>

#if defined(FOG_OS_WINDOWS)

#include <Fog/Core/String.h>

//! @addtogroup Fog_Core_Private
//! @{

namespace Fog {

//! @brief Namespace that contains various functions related to Windows OS
//! development.
namespace WinUtil {

FOG_HIDDEN err_t getModuleFileName(HMODULE hModule, String& dst);

} // WinUtil namespace
} // Fog namespace

//! @}

#endif // FOG_OS_WINDOWS

// [Guard]
#endif // _FOG_CORE_WINUTIL_H
