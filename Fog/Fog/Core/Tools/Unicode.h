// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_UNICODE_H
#define _FOG_CORE_TOOLS_UNICODE_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>

namespace Fog {

// ============================================================================
// [Fog::UTF16_CONST]
// ============================================================================

enum UTF16_CONST
{
  // --------------------------------------------------------------------------
  // [UTF16_BOM]
  // --------------------------------------------------------------------------

  //! @brief UTF-16 Native BOM (Byte-Order-Mark).
  UTF16_BOM_NATIVE = 0xFEFF,
  //! @brief UTF-16 Swapped BOM (Byte-Order-Mark).
  UTF16_BOM_SWAPPED = 0xFFFE,

  // --------------------------------------------------------------------------
  // [UTF16_SURROGATE]
  // --------------------------------------------------------------------------

  //! @brief UTF-16 Leading (HI) surrogate minimum (0xD800)
  UTF16_SURROGATE_LEAD_MIN = 0xD800,
  //! @brief UTF-16 Leading (HI) surrogate maximum (0xDBFF)
  UTF16_SURROGATE_LEAD_MAX = 0xDBFF,

  //! @brief UTF-16 Trailing (LO) surrogate minimum (0xDC00)
  UTF16_SURROGATE_TRAIL_MIN = 0xDC00,
  //! @brief UTF-16 Trailing (LO) surrogate maximum (0xDFFF)
  UTF16_SURROGATE_TRAIL_MAX = 0xDFFF,

  //! @brief UTF-16 Surrogate (LO and HI) base.
  UTF16_SURROGATE_PAIR_BASE = 0xD800,
  //! @brief UTF-16 Surrogate (LO and HI) mask.
  UTF16_SURROGATE_PAIR_MASK = 0xFC00,

  //! @brief Mask to test whether the UTF-16 value is lead or trail surrogate.
  UTF16_SURROGATE_TEST_MASK = 0xF800,

  UTF16_LEAD_OFFSET       = 0x0D800 - (0x10000 >> 10),
  UTF16_SURROGATE_OFFSET  = 0x10000 - (0x0D800 << 10) - 0xDC00
};

// ============================================================================
// [Fog::UTF32_CONST]
// ============================================================================

enum UTF32_CONST
{
  // --------------------------------------------------------------------------
  // [UTF32_BOM]
  // --------------------------------------------------------------------------

  //! @brief UTF-32 Native BOM (Byte-Order-Mark).
  UTF32_BOM_NATIVE = 0x0000FEFFU,
  //! @brief UTF-32 Swapped BOM (Byte-Order-Mark).
  UTF32_BOM_SWAPPED = 0xFFFE0000U
};

// ============================================================================
// [Fog::UNICHAR_CONST]
// ============================================================================

enum UNICHAR_CONST
{
  // --------------------------------------------------------------------------
  // [UNICHAR]
  // --------------------------------------------------------------------------

  //! @brief Maximum valid unicode code-point.
  UNICHAR_MAX = 0x0010FFFF
};

// ============================================================================
// [Fog::Unicode]
// ============================================================================

namespace Unicode {

//! @addtogroup Fog_Core_Tools
//! @{

//! @}

} // Unicode namespace

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_UNICODE_H
