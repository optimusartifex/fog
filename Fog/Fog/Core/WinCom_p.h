// [Fog-Core Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_WINCOM_P_H
#define _FOG_CORE_WINCOM_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#if defined(FOG_OS_WINDOWS)

// ============================================================================
// [Fog-COM - Macros]
// ============================================================================

//! @addtogroup Fog_Core_Private
//! @{

#define FOG_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
  static const GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

#if !defined(FOG_CC_MSVC) && !defined(__uuidof)
#define __uuidof(INTERFACE) IID_##INTERFACE
#endif // !__uuidof

//! @}

// [Guard]
#endif // FOG_OS_WINDOWS
#endif // _FOG_CORE_WINCOM_P_H
