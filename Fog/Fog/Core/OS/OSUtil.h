// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_OSUTIL_H
#define _FOG_CORE_OS_OSUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::WinUtil]
// ============================================================================

struct FOG_NO_EXPORT OSUtil
{
  static FOG_INLINE err_t getErrFromLibCErrorCode(int code)
  {
    return fog_api.osutil_getErrFromLibCErrorCode(code);
  }

  static FOG_INLINE err_t getErrFromLibCErrno()
  {
    return fog_api.osutil_getErrFromLibCErrno();
  }

  static FOG_INLINE err_t getErrFromOSErrorCode(int code)
  {
    return fog_api.osutil_getErrFromOSErrorCode(code);
  }

  static FOG_INLINE err_t getErrFromOSLastError()
  {
    return fog_api.osutil_getErrFromOSLastError();
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_OSUTIL_H
