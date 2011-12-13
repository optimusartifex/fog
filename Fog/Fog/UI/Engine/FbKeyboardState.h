// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBKEYBOARDSTATE_H
#define _FOG_UI_ENGINE_FBKEYBOARDSTATE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbKeyboardState]
// ============================================================================

//! @brief Frame-buffer keyboard state.
struct FOG_NO_EXPORT FbKeyboardState
{
  FOG_INLINE FbKeyboardState()
  {
    MemOps::zero(this, sizeof(FbKeyboardState));
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FbKeyboardState& other) const
  {
    return MemOps::eq(this, &other, sizeof(FbKeyboardState));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator=(const FbKeyboardState& other)
  {
    MemOps::copy(this, &other, sizeof(FbKeyboardState));
    return *this;
  }
  
  FOG_INLINE bool operator==(const FbKeyboardState& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FbKeyboardState& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t modifiers;
  uint8_t keymap[256];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBKEYBOARDSTATE_H
