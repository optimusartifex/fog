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
  FOG_INLINE FbMouseState()
  {
    reset();
  }

  explicit FOG_INLINE FbMouseState(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE FbWindow* getWindow() const { return _window; }
  FOG_INLINE const PointI& getPosition() const { return _position; }
  FOG_INLINE uint32_t getHover() const { return _hover; }
  FOG_INLINE uint32_t getButtonMask() const { return _buttonMask; }

  FOG_INLINE void setWindow(FbWindow* window) { _window = window; }
  FOG_INLINE void setPosition(const PointI& position) { _position = position; }
  FOG_INLINE void setHover(uint32_t hover) { _hover = hover; }
  FOG_INLINE void setButtonMask(uint32_t buttonMask) { _buttonMask = buttonMask; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero(this, sizeof(FbMouseState));
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FbMouseState& other) const
  {
    return _position   == other._position   &&
           _hover      == other._hover      &&
           _buttonMask == other._buttonMask &&
           _window     == other._window     ;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FbMouseState& operator=(const FbMouseState& other)
  {
    MemOps::copy(this, &other, sizeof(FbMouseState));
    return *this;
  }
  
  FOG_INLINE bool operator==(const FbMouseState& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FbMouseState& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // NOTE: It looks weird, but it's better to place _position before _window,
  // because we get better structure packing when compiling for 64-bit mode.

  //! @brief Mouse position relative to @ref FbWindow (in client area).
  PointI _position;
  //! @brief Hover state.
  uint32_t _hover;
  //! @brief Pressed button-mask.
  uint32_t _buttonMask;
  //! @brief Window.
  FbWindow* _window;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBMOUSESTATE_H
