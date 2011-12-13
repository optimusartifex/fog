// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBMOUSESTATE_H
#define _FOG_UI_ENGINE_FBMOUSESTATE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbMouseState]
// ============================================================================

//! @brief Frame-buffer mouse state.
struct FOG_NO_EXPORT FbMouseState
{
  FOG_INLINE FbMouseState() :
    position(0, 0),
    hover(0),
    buttons(0)
  {
  }

  explicit FOG_INLINE FbMouseState(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FbMouseState& other) const
  {
    return position == other.position &&
           hover    == other.hoves    &&
           buttons  == other.buttons  ;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator=(const FbMouseState& other)
  {
    position = other.position;
    hover = other.hover;
    buttons = other.buttons;

    return *this;
  }
  
  FOG_INLINE bool operator==(const FbMouseState& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FbMouseState& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Mouse position relative to @c uiWindow (in client area).
  PointI position;
  //! @brief Hover state.
  uint32_t hover;
  //! @brief Pressed buttons.
  uint32_t buttons;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBMOUSESTATE_H
