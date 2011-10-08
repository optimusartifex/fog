// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_OSINFO_H
#define _FOG_CORE_OS_OSINFO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::OSInfo]
// ============================================================================

struct FOG_NO_EXPORT OSInfo
{
  static FOG_INLINE StringW getName()
  {
    StringW result;
    _api.osinfo_getName(&result);
    return result;
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_OSINFO_H
