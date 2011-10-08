// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_WINCOM_H
#define _FOG_CORE_OS_WINCOM_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>
#include <Fog/Core/Tools/Stream.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [FOG_COM]
// ============================================================================

#define FOG_COM_DEFINE_GUID(_Name_, L, W1, W2, B1, B2, B3, B4, B5, B6, B7, B8) \
  static const GUID _Name_ = { L, W1, W2, { B1, B2, B3, B4, B5, B6, B7, B8 } }

#if !defined(FOG_CC_MSC) && !defined(__uuidof)
# define __uuidof(_Interface_) IID_##_Interface_
#elif defined(__MINGW32__) 
// __uuidof is only implemented in GCC 4.7
// __uuidof emulation can be switched off when a stable GCC 4.7 becomes popular
# ifdef __uuidof // Undefine the __uuidof macro that exists in pre-4.7 headers
#  undef __uuidof
# endif
# define __uuidof(_Interface_) IID_##_Interface_
#endif // !__uuidof

struct FOG_NO_EXPORT WinCOM
{
  static FOG_INLINE err_t makeIStream(IStream** dst, Stream& src)
  {
    return _api.wincom_makeIStream(dst, &src);
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_WINCOM_H
