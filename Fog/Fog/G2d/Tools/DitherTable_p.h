// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_DITHERTABLE_P_H
#define _FOG_G2D_TOOLS_DITHERTABLE_P_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

namespace Fog {

//! @addtogroup Fog_G2d_Tools
//! @{

// ============================================================================
// [Fog::DitherTable]
// ============================================================================

//! @internal
//!
//! @brief Functions and tables that can be used to dithering
//! implementation
//!
//! You probabbly don't need to use this class, because it's mainly
//! for internals in Fog library. It's used by pixel converters with
//! dithering enabled.
//!
//! @note Dithering can be configured to use small dithering tables
//! instead of large ones that can save 16kb of library size. Default
//! are large dithering tables that produces better images.
//!
//! @note Dithering tables were stolen from EVAS - High performance canvas
//! library. Evas is BSD licensed, main developer is rasterman <www.rasterman.com>
struct FOG_NO_EXPORT DitherTable
{
  // --------------------------------------------------------------------------
  // [Constants]
  // --------------------------------------------------------------------------

  enum {
#ifdef FOG_SMALL_DITHER
    //! @brief Size of dithering table.
    SIZE = 4,        // Always 2^N
    BITS = 4,        // Count of bits used per element
    DIV = 16,
    MASK = SIZE - 1
#else
    //! @brief Size of dithering table.
    SIZE = 128,      // Always 2^N
    BITS = 6,        // Count of bits used per element
    DIV = 64,
    MASK = SIZE - 1
#endif // FOG_SMALL_DITHER
  };

  // --------------------------------------------------------------------------
  // [Table]
  // --------------------------------------------------------------------------

  // To find correct position in dithering table, use this formula:
  // table[X & Mask][Y & Mask]
  static const uint8_t matrix[SIZE][SIZE];

  // --------------------------------------------------------------------------
  // [Helpers]
  //
  // Helpers that helps us to use this dithering table for
  // more image types and bit depths
  // --------------------------------------------------------------------------

  //! @brief Shifting argument helper.
  template<typename T>
  static FOG_INLINE T shf_arg(const T& t)
  {
    return BITS - (8 - t);
  }

  //! @brief Shifting dither helper.
  template<typename T>
  static FOG_INLINE T shf_dither(T value, int shift)
  {
    if (shift >= 0)
      return value >> (uint)shift;
    else
      return value << (uint)(-shift);
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TOOLS_DITHERTABLE_P_H
