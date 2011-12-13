// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBPALETTEINFO_H
#define _FOG_UI_ENGINE_FBPALETTEINFO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbPaletteInfo]
// ============================================================================

//! @brief Frame-buffer pelette info.
//!
//! This class is only used in case that the frame-buffer is only 8-bit and
//! contains RGB palette.
struct FOG_NO_EXPORT FbPaletteInfo
{
  // --------------------------------------------------------------------------
  // [Cons]
  // --------------------------------------------------------------------------

  FOG_INLINE FbPaletteInfo()
  {
    MemOps::zero(this, sizeof(FbPaletteInfo));
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FbPaletteInfo& other) const
  {
    return MemOps::eq(this, &other, sizeof(FbPaletteInfo));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator=(const FbPaletteInfo& other)
  {
    MemOps::copy(this, &other, sizeof(FbPaletteInfo));
    return *this;
  }
  
  FOG_INLINE bool operator==(const FbPaletteInfo& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FbPaletteInfo& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Count of entities in map[] 2 to 256.
  uint32_t length;

  //! @brief 8-bit palette mapping.
  uint8_t map[256];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBPALETTEINFO_H
