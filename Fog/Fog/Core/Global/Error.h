// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_ERROR_H
#define _FOG_CORE_GLOBAL_ERROR_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::Error]
// ============================================================================

//! @brief Error handling and error to string translaton.
struct FOG_API Error
{
  // --------------------------------------------------------------------------
  // [Error Translation]
  // --------------------------------------------------------------------------

  typedef void (*TranslatorFn)(String& dst, err_t code);

  static String getMessage(err_t code);

  static err_t registerErrorRange(err_t start, err_t end, TranslatorFn fn);
  static err_t unregisterErrorRange(err_t start, err_t end);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_ERROR_H
