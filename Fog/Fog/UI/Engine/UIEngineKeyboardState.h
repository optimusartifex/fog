// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_UIENGINEKEYBOARDSTATE_H
#define _FOG_UI_ENGINE_UIENGINEKEYBOARDSTATE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::UIEngineKeyboardState]
// ============================================================================

//! @brief Frame-buffer keyboard state.
struct FOG_NO_EXPORT UIEngineKeyboardState
{
  FOG_INLINE UIEngineKeyboardState()
  {
    reset();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getModifiers() const { return _modifiers; }
  FOG_INLINE void setModifiers(uint32_t modifiers) { _modifiers = modifiers; }

  FOG_INLINE uint8_t* getKeyMap() { return _keyMap; }
  FOG_INLINE const uint8_t* getKeyMap() const { return _keyMap; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero(this, sizeof(UIEngineKeyboardState));
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const UIEngineKeyboardState& other) const
  {
    return MemOps::eq(this, &other, sizeof(UIEngineKeyboardState));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineKeyboardState& operator=(const UIEngineKeyboardState& other)
  {
    MemOps::copy(this, &other, sizeof(UIEngineKeyboardState));
    return *this;
  }
  
  FOG_INLINE bool operator==(const UIEngineKeyboardState& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const UIEngineKeyboardState& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _modifiers;
  uint8_t _keyMap[256];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_UIENGINEKEYBOARDSTATE_H
