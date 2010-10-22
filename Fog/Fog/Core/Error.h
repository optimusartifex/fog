// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_ERROR_H
#define _FOG_CORE_ERROR_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Debugging
//! @{

// ============================================================================
// [Fog::Error]
// ============================================================================

//! @brief Error handling and translating error codes to string representation.
struct FOG_API Error
{
  typedef void (*TranslatorFn)(String& dst, err_t code);

  static String getMessage(err_t code);

  static err_t registerRange(err_t start, err_t end, TranslatorFn fn);
  static err_t unregisterRange(err_t start, err_t end);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_ERROR_H
