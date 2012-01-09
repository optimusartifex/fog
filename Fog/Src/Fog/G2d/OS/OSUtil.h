// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_OS_OSUTIL_H
#define _FOG_G2D_OS_OSUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

#if defined(FOG_OS_WINDOWS)
#include <Fog/G2d/OS/WinUtil.h>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_MAC)
#include <Fog/G2d/OS/MacUtil.h>
#endif // FOG_OS_MAC

// [Guard]
#endif // _FOG_G2D_OS_OSUTIL_H
