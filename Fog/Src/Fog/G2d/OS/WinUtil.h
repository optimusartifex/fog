// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_OS_WINUTIL_H
#define _FOG_G2D_OS_WINUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/Threading/ThreadLocal.h>

namespace Fog {
namespace WinUtil {

//! @addtogroup Fog_G2d_OS
//! @{

// ============================================================================
// [Fog::WinUtil]
// ============================================================================

static FOG_INLINE HDC getThreadLocalDC()
{
  return fog_api.winutil_getThreadLocalDC();
}

//! @}

} // WinUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_OS_WINUTIL_H
