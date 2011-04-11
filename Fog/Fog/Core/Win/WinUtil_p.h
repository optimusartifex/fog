// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_WIN_WINUTIL_H
#define _FOG_CORE_WIN_WINUTIL_H

#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Tools/String.h>

//! @namespace Fog::WinUtil
//!
//! @brief Namespace that contains various functions related to Windows OS
//! development.

namespace Fog {
namespace WinUtil {

//! @addtogroup Fog_Core_Win
//! @{

FOG_NO_EXPORT err_t getModuleFileName(HMODULE hModule, String& dst);

//! @}

} // WinUtil namespace
} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
#endif // _FOG_CORE_WIN_WINUTIL_H
