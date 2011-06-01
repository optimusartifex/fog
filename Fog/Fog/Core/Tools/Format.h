// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_FORMAT_H
#define _FOG_CORE_TOOLS_FORMAT_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::FormatFlags]
// ============================================================================

//! @brief String format flags.
struct FOG_NO_EXPORT FormatFlags
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FormatFlags(sysuint_t precision = NO_PRECISION, sysuint_t width = NO_WIDTH, uint32_t flags = NO_FLAGS) :
    precision(precision),
    width(width),
    flags(flags)
  {
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  sysuint_t precision;
  sysuint_t width;
  uint32_t flags;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::FormatFlags, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_TOOLS_FORMAT_H
