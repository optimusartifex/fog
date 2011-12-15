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
#include <Fog/G2d/Source/Argb.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbCaretState]
// ============================================================================

//! @brief Frame-buffer caret state.
struct FOG_NO_EXPORT FbCaretState
{
  FOG_INLINE FbCaretState() :
    _rect(0, 0, 0, 0),
    _color(0xFF000000),
    _type(CARET_TYPE_NORMAL),
    _animation(0)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const RectI& getRect() const { return _rect; }
  FOG_INLINE const Argb32& getColor() const { return _color; }
  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE uint32_t getAnimation() const { return _animation; }

  FOG_INLINE void setRect(const RectI& rect) { _rect = rect; }
  FOG_INLINE void setColor(const Argb32& color) { _color = color; }
  FOG_INLINE void setType(uint32_t type) { _type = type; }
  FOG_INLINE void setAnimation(uint32_t animation) { _animation = animation; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------
  
  FOG_INLINE void reset()
  {
    _rect.reset();
    _color.setPacked32(0xFF000000);
    _type = CARET_TYPE_NORMAL;
    _animation = 0;
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

  FOG_INLINE FbCaretState& operator=(const FbCaretState& other)
  {
    MemOps::copy(this, &other, sizeof(FbCaretState));
    return *this;
  }

  FOG_INLINE bool operator==(const FbCaretState& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FbCaretState& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectI _rect;
  Argb32 _color;
  uint32_t _type;
  uint32_t _animation;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBCARETSTATE_H
