// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBEVENT_H
#define _FOG_UI_ENGINE_FBEVENT_H

// [Dependencies]
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbEvent]
// ============================================================================

//! @brief @ref FbWindow base event.
struct FOG_NO_EXPORT FbEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FbEvent(uint32_t code, uint32_t flags) :
    _code(code),
    _flags(flags)
  {
  }

  explicit FOG_INLINE FbEvent(_Uninitialized)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getCode() const { return _code; }
  FOG_INLINE uint32_t getFlags() const { return _flags; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Event code (see @c FB_EVENT_CODE).
  uint32_t _code;
  //! @brief Event flags (see @c FB_EVENT_FLAG).
  uint32_t _flags;
};

// ============================================================================
// [Fog::FbCreateEvent]
// ============================================================================

//! @brief @ref FbWindow create event.
struct FOG_NO_EXPORT FbCreateEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FbCreateEvent(uint32_t code, uint32_t flags) :
    FbEvent(code, flags)
  {
  }

  explicit FOG_INLINE FbCreateEvent(_Uninitialized) :
    FbEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  // None.

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // None.
};

// ============================================================================
// [Fog::FbDestroyEvent]
// ============================================================================

//! @brief @ref FbWindow create event.
struct FOG_NO_EXPORT FbDestroyEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FbDestroyEvent(uint32_t code, uint32_t flags) :
    FbEvent(code, flags)
  {
  }

  explicit FOG_INLINE FbDestroyEvent(_Uninitialized) :
    FbEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  // None.

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // None.
};

// ============================================================================
// [Fog::FbVisibilityEvent]
// ============================================================================

//! @brief @ref FbWindow state event (visible, hidden, or parent hidden).
struct FOG_NO_EXPORT FbVisibilityEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FbVisibilityEvent(uint32_t code, uint32_t flags) :
    FbEvent(code, flags)
  {
  }

  explicit FOG_INLINE FbVisibilityEvent(_Uninitialized) :
    FbEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getVisibility() const { return _visibility; }
  FOG_INLINE uint32_t getOldVisibility() const { return _oldVisibility; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Visibility, see @a VISIBILITY.
  uint32_t _visibility;
  //! @brief Old visibility.
  uint32_t _oldVisibility;
};

// ============================================================================
// [Fog::FbStateEvent]
// ============================================================================

//! @brief @ref FbWindow state event (enabled, disabled, or parent disabled).
struct FOG_NO_EXPORT FbStateEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FbStateEvent(uint32_t code, uint32_t flags) :
    FbEvent(code, flags)
  {
  }

  explicit FOG_INLINE FbStateEvent(_Uninitialized) :
    FbEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getState() const { return _state; }
  FOG_INLINE uint32_t getOldState() const { return _oldState; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief State, see @a STATE.
  uint32_t _state;
  //! @brief Old state.
  uint32_t _oldState;
};

// ============================================================================
// [Fog::FbFocusEvent]
// ============================================================================

//! @brief @ref FbWindow state event (enabled, disabled, or parent disabled).
struct FOG_NO_EXPORT FbFocusEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FbFocusEvent(uint32_t code, uint32_t flags) :
    FbEvent(code, flags)
  {
  }

  explicit FOG_INLINE FbFocusEvent(_Uninitialized) :
    FbEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  // None.

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // None.
};

// ============================================================================
// [Fog::FbGeometryEvent]
// ============================================================================

//! @brief @ref FbWindow geometry (window geometry, and client geometry, and
//! orientation).
struct FOG_NO_EXPORT FbGeometryEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FbGeometryEvent(uint32_t code, uint32_t flags) :
    FbEvent(code, flags)
  {
  }

  explicit FOG_INLINE FbGeometryEvent(_Uninitialized) :
    FbEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get orientation.
  FOG_INLINE uint32_t getOrientation() const { return _orientation; }
  //! @brief Get old orientation.
  FOG_INLINE uint32_t getOldOrientation() const { return _oldOrientation; }

  //! @brief Get window geometry.
  FOG_INLINE const RectI& getWindowGeometry() const { return _windowGeometry; }
  //! @brief Get old window geometry.
  FOG_INLINE const RectI& getOldWindowGeometry() const { return _oldWindowGeometry; }

  //! @brief Get client geometry.
  FOG_INLINE const RectI& getClientGeometry() const { return _clientGeometry; }
  //! @brief Get old client geometry.
  FOG_INLINE const RectI& getOldClientGeometry() const { return _oldClientGeometry; }
  
  FOG_INLINE uint32_t getGeometryFlags() const { return _geometryFlags; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Window orientation (@see ORIENTATION).
  uint32_t _orientation;
  //! @brief Old window orientation.
  uint32_t _oldOrientation;

  //! @brief New window geometry.
  RectI _windowGeometry;
  //! @brief Old window geometry.
  RectI _oldWindowGeometry;

  //! @brief New client geometry.
  RectI _clientGeometry;
  //! @brief Old client geometry.
  RectI _oldClientGeometry;
  
  //! @brief Geometry changed flags.
  uint32_t _geometryFlags;
};

// ============================================================================
// [Fog::FbKeyEvent]
// ============================================================================

//! @brief @ref FbWindow key event (down, up, press, and input).
struct FOG_NO_EXPORT FbKeyEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FbKeyEvent(uint32_t code, uint32_t flags) :
    FbEvent(code, flags)
  {
  }

  explicit FOG_INLINE FbKeyEvent(_Uninitialized) :
    FbEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get mask of all pressed mouse buttons.
  FOG_INLINE uint32_t getButtonMask() const { return _buttonMask; }
  //! @brief Get mask of all active key modifiers.
  FOG_INLINE uint32_t getModifierMask() const { return _modifierMask; }

  //! @brief Get key code, see @ref KEY_CODE.
  FOG_INLINE uint32_t getKey() const { return _key; }
  //! @brief Get system key code.
  //!
  //! @note Operating system and windowing system specific. Please do not depend
  //! on this value and use it only to workaround Fog-Framework bugs or to
  //! access functionality not directly supported by Fog-Framework.
  FOG_INLINE uint32_t getSystemCode() const { return _systemCode; }
  
  //! @brief Get unicode character value of the key event.
  FOG_INLINE uint32_t getUnicode() const { return _unicode; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Mask of all buttons pressed.
  uint32_t _buttonMask;
  //! @brief Key modifiers mask
  uint32_t _modifierMask;

  //! @brief Key code (system independent, see @c KEY_CODE).
  uint32_t _key;
  //! @brief System key code (depends on @c FbEngine implementation).
  //!
  //! @note Can be zero in case that the event was dispatched manually, without
  //! translating the native event.
  uint32_t _systemCode;
  
  //! @brief Unicode value.
  uint32_t _unicode;
};

// ============================================================================
// [Fog::FbMouseEvent]
// ============================================================================

//! @brief @ref FbWindow state event (in, out, move, down, up, click. dblclick).
struct FOG_NO_EXPORT FbMouseEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FbMouseEvent(uint32_t code, uint32_t flags) :
    FbEvent(code, flags)
  {
  }

  explicit FOG_INLINE FbMouseEvent(_Uninitialized) :
    FbEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get mask of all pressed mouse buttons.
  FOG_INLINE uint32_t getButtonMask() const { return _buttonMask; }
  //! @brief Get mask of all active key modifiers.
  FOG_INLINE uint32_t getModifierMask() const { return _modifierMask; }

  //! @brief Get the new mouse position
  FOG_INLINE const PointI& getPosition() const { return _position; }
  //! @brief Get the offset (the change between the new and old mouse position).
  FOG_INLINE const PointI& getOffset() const { return _offset; }

  //! @brief Get mouse button, which caused mouse-down, mouse-up, click, or
  //! double-click event.
  FOG_INLINE uint32_t getButton() const { return _button; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Mask of all buttons pressed.
  uint32_t _buttonMask;
  //! @brief Key modifiers mask
  uint32_t _modifierMask;

  //! @brief Current position.
  PointI _position;
  //! @brief Offset (current position subtracted by the old position).
  PointI _offset;

  //! @brief Button that caused the event, or @c BUTTON_NONE.
  uint32_t _button;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBEVENT_H
