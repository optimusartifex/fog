// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERUTIL_P_H
#define _FOG_GRAPHICS_RASTERUTIL_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Graphics/Constants.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::RasterUtil]
// ============================================================================

//! @internal
//!
//! @brief Utility methods used by raster engine.
struct FOG_HIDDEN RasterUtil
{
  //! @brief Get whether ARGB32 pixel @a c0 has alpha set to 0xFF (255 in decimal).
  static FOG_INLINE bool isAlpha0xFF_ARGB32(uint32_t c0) { return (c0 >= 0xFF000000); }

  //! @brief Get whether ARGB32 pixel @a c0 has alpha set to 0x00 (0 in decimal).
  static FOG_INLINE bool isAlpha0x00_ARGB32(uint32_t c0) { return (c0 <= 0x00FFFFFF); }

  //! @brief Get whether PRGB32 pixel @a c0 has alpha set to 0xFF (255 in decimal).
  static FOG_INLINE bool isAlpha0xFF_PRGB32(uint32_t c0) { return (c0 >= 0xFF000000); }

  //! @brief Get whether PRGB32 pixel @a c0 has alpha set to 0x00 (0 in decimal).
  static FOG_INLINE bool isAlpha0x00_PRGB32(uint32_t c0) { return (c0 == 0x00000000); }

  //! @brief Extend 8-bit mask to 32-bit value.
  static FOG_INLINE uint32_t extendMask8(uint32_t msk0) { return msk0 * 0x01010101; }

  //! @brief Get whether the msk0 mask is 32-bit extended mask (used in asserts).
  static FOG_INLINE bool isMask8Extended(uint32_t msk0)
  {
    return ((msk0 & 0x000000FF) == ((msk0 & 0x0000FF00) >>  8) ||
            (msk0 & 0x000000FF) == ((msk0 & 0x00FF0000) >> 16) ||
            (msk0 & 0x000000FF) == ((msk0 & 0xFF000000) >> 24) );
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_RASTERUTIL_P_H
