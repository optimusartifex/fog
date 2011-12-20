// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_UIENGINEPALETTEINFO_H
#define _FOG_UI_ENGINE_UIENGINEPALETTEINFO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::UIEnginePaletteInfo]
// ============================================================================

//! @brief Frame-buffer pelette info.
//!
//! This class is only used in case that the frame-buffer is only 8-bit and
//! contains RGB palette.
struct FOG_NO_EXPORT UIEnginePaletteInfo
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEnginePaletteInfo()
  {
    MemOps::zero(this, sizeof(UIEnginePaletteInfo));
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getLength() const { return _length; }
  FOG_INLINE void setLength(uint32_t length) { _length = length; }

  FOG_INLINE uint8_t* getMap() { return _map; }
  FOG_INLINE const uint8_t* getMap() const { return _map; }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const UIEnginePaletteInfo& other) const
  {
    return MemOps::eq(this, &other, sizeof(UIEnginePaletteInfo));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEnginePaletteInfo& operator=(const UIEnginePaletteInfo& other)
  {
    MemOps::copy(this, &other, sizeof(UIEnginePaletteInfo));
    return *this;
  }
  
  FOG_INLINE bool operator==(const UIEnginePaletteInfo& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const UIEnginePaletteInfo& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Count of entities in map[] 2 to 256.
  uint32_t _length;

  //! @brief 8-bit palette mapping.
  uint8_t _map[256];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_UIENGINEPALETTEINFO_H
