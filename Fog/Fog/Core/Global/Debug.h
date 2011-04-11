// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_DEBUG_H
#define _FOG_CORE_GLOBAL_DEBUG_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

FOG_CVAR_EXTERN uint32_t fog_failed;

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::Debug]
// ============================================================================

//! @brief Debugging.
struct FOG_API Debug
{
  static void dbgFormat(const char* fmt, ...);
  static void dbgFunc(const char* className, const char* methodName, const char* fmt, ...);

  static void FOG_NO_RETURN failFormat(const char* fmt, ...);
  static void FOG_NO_RETURN failFunc(const char* className, const char* methodName, const char* fmt, ...);

  static void FOG_NO_RETURN assertMessage(const char* fileName, int line, const char* msg);
  static void FOG_NO_RETURN assertFormat(const char* fileName, int line, const char* fmt, ...);

  static void FOG_NO_RETURN exit(int code);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_DEBUG_H
