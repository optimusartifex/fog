// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_COM_COM_H
#define _FOG_CORE_COM_COM_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

// ============================================================================
// [Fog::Core - COM Macros]
// ============================================================================

//! @addtogroup Fog_Core_COM
//! @{

#define FOG_COM_DEFINE_GUID(_Name_, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
  static const GUID _Name_ = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

#if !defined(FOG_CC_MSC) && !defined(__uuidof)
#define __uuidof(_Interface_) IID_##_Interface_
#endif // !__uuidof

//! @}

// [Guard]
#endif // _FOG_CORE_COM_COM_H
