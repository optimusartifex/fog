// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBCARETSTATE_H
#define _FOG_UI_ENGINE_FBCARETSTATE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbCaretState]
// ============================================================================

//! @brief Frame-buffer caret state.
struct FOG_NO_EXPORT FbCaretState
{
  FOG_INLINE FbCaretInfo() :
    rect(0, 0, 0, 0),
    color(),
    type(0),
    animation(0)
  {
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FbCaretState& other) const
  {
    return MemOps::eq(this, &other, sizeof(FbCaretState));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator=(const FbCaretState& other)
  {
    MemOps::copy(this, &other, sizeof(FbCaretState));
    return *this;
  }

  FOG_INLINE bool operator==(const FbCaretState& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FbCaretState& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectI rect;
  Argb32 color;
  uint32_t type;
  uint32_t animation;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBCARETSTATE_H
