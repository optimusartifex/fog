// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_ENVIRONMENT_H
#define _FOG_CORE_OS_ENVIRONMENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::Environment]
// ============================================================================

struct FOG_NO_EXPORT Environment
{
  static FOG_INLINE err_t getValue(const Ascii8& key, StringW& value)
  {
    return _api.environment_getValueStubA(&key, &value);
  }

  static FOG_INLINE err_t getValue(const StringW& key, StringW& value)
  {
    return _api.environment_getValueStringW(&key, &value);
  }

  static FOG_INLINE err_t setValue(const Ascii8& key, const StringW& value)
  {
    return _api.environment_setValueStubA(&key, &value);
  }

  static FOG_INLINE err_t setValue(const StringW& key, const StringW& value)
  {
    return _api.environment_setValueStringW(&key, &value);
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_ENVIRONMENT_H
