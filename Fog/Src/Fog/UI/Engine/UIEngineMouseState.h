// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_UIENGINEMOUSESTATE_H
#define _FOG_UI_ENGINE_UIENGINEMOUSESTATE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::UIEngineMouseState]
// ============================================================================

//! @brief Frame-buffer mouse state.
struct FOG_NO_EXPORT UIEngineMouseState
{
  FOG_INLINE UIEngineMouseState()
  {
    reset();
  }

  explicit FOG_INLINE UIEngineMouseState(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineWindow* getWindow() const { return _window; }
  FOG_INLINE const PointI& getPosition() const { return _position; }
  FOG_INLINE uint32_t getHover() const { return _hover; }
  FOG_INLINE uint32_t getButtonMask() const { return _buttonMask; }

  FOG_INLINE void setWindow(UIEngineWindow* window) { _window = window; }
  FOG_INLINE void setPosition(const PointI& position) { _position = position; }
  FOG_INLINE void setHover(uint32_t hover) { _hover = hover; }
  FOG_INLINE void setButtonMask(uint32_t buttonMask) { _buttonMask = buttonMask; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero(this, sizeof(UIEngineMouseState));
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const UIEngineMouseState& other) const
  {
    return _position   == other._position   &&
           _hover      == other._hover      &&
           _buttonMask == other._buttonMask &&
           _window     == other._window     ;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineMouseState& operator=(const UIEngineMouseState& other)
  {
    MemOps::copy(this, &other, sizeof(UIEngineMouseState));
    return *this;
  }
  
  FOG_INLINE bool operator==(const UIEngineMouseState& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const UIEngineMouseState& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // NOTE: It looks weird, but it's better to place _position before _window,
  // because we get better structure packing when compiling for 64-bit mode.

  //! @brief Mouse position relative to @ref UIEngineWindow (in client area).
  PointI _position;
  //! @brief Hover state.
  uint32_t _hover;
  //! @brief Pressed button-mask.
  uint32_t _buttonMask;
  //! @brief Window.
  UIEngineWindow* _window;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_UIENGINEMOUSESTATE_H
