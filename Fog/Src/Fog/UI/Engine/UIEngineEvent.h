// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_UIENGINEEVENT_H
#define _FOG_UI_ENGINE_UIENGINEEVENT_H

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
// [Fog::UIEngineEvent]
// ============================================================================

//! @brief @ref UIEngineWindow base event.
struct FOG_NO_EXPORT UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineEvent(uint32_t code, uint32_t flags) :
    _code(code),
    _flags(flags)
  {
  }

  explicit FOG_INLINE UIEngineEvent(_Uninitialized)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getCode() const { return _code; }
  FOG_INLINE uint32_t getFlags() const { return _flags; }

  FOG_INLINE uint32_t isAccepted() const { return (_flags & UI_ENGINE_EVENT_FLAG_ACCEPT) != 0; }

  FOG_INLINE void accept() { _flags |= UI_ENGINE_EVENT_FLAG_ACCEPT; }
  FOG_INLINE void reject() { _flags &= ~UI_ENGINE_EVENT_FLAG_ACCEPT; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Event code (see @c UI_ENGINE_EVENT_CODE).
  uint32_t _code;
  //! @brief Event flags (see @c UI_ENGINE_EVENT_FLAG).
  uint32_t _flags;
};

// ============================================================================
// [Fog::UIEngineCreateEvent]
// ============================================================================

//! @brief @ref UIEngineWindow create event.
struct FOG_NO_EXPORT UIEngineCreateEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineCreateEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineCreateEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
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
// [Fog::UIEngineDestroyEvent]
// ============================================================================

//! @brief @ref UIEngineWindow destroy event.
struct FOG_NO_EXPORT UIEngineDestroyEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineDestroyEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineDestroyEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
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
// [Fog::UIEngineCloseEvent]
// ============================================================================

//! @brief @ref UIEngineWindow close event.
struct FOG_NO_EXPORT UIEngineCloseEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineCloseEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineCloseEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
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
// [Fog::UIEngineStateEvent]
// ============================================================================

//! @brief @ref UIEngineWindow state event.
struct FOG_NO_EXPORT UIEngineStateEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineStateEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineStateEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
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
// [Fog::UIEngineFocusEvent]
// ============================================================================

//! @brief @ref UIEngineWindow focus event (focus-in, focus-out).
struct FOG_NO_EXPORT UIEngineFocusEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineFocusEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineFocusEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
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
// [Fog::UIEngineGeometryEvent]
// ============================================================================

//! @brief @ref UIEngineWindow geometry (window geometry, and client geometry, and
//! orientation).
struct FOG_NO_EXPORT UIEngineGeometryEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineGeometryEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineGeometryEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
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
// [Fog::UIEngineKeyEvent]
// ============================================================================

//! @brief @ref UIEngineWindow key event (down, up, press, and input).
struct FOG_NO_EXPORT UIEngineKeyEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineKeyEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineKeyEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get mask of all mouse buttons pressed.
  FOG_INLINE uint32_t getButtonMask() const { return _buttonMask; }
  //! @brief Get mask of all active modifiers.
  FOG_INLINE uint32_t getModifierMask() const { return _modifierMask; }

  //! @brief Get key code, see @ref KEY_CODE.
  FOG_INLINE uint32_t getKeyCode() const { return _keyCode; }
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

  //! @brief Buttons mask.
  uint32_t _buttonMask;
  //! @brief Modifiers mask
  uint32_t _modifierMask;

  //! @brief Key code (system independent, see @c KEY_CODE).
  uint32_t _keyCode;
  //! @brief System key code (depends on @c UIEngine implementation).
  //!
  //! @note Can be zero in case that the event was dispatched manually, without
  //! translating the native event.
  uint32_t _systemCode;
  
  //! @brief Unicode value.
  uint32_t _unicode;
};

// ============================================================================
// [Fog::UIEngineMouseEvent]
// ============================================================================

//! @brief @ref UIEngineWindow mouse event (in, out, move, down, up, click, 
//! or dblclick).
struct FOG_NO_EXPORT UIEngineMouseEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineMouseEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineMouseEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get mask of all mouse buttons pressed.
  FOG_INLINE uint32_t getButtonMask() const { return _buttonMask; }
  //! @brief Get mask of all active modifiers.
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

  //! @brief Buttons mask.
  uint32_t _buttonMask;
  //! @brief Modifiers mask
  uint32_t _modifierMask;

  //! @brief Current position.
  PointI _position;
  //! @brief Offset (current position subtracted by the old position).
  PointI _offset;

  //! @brief Button that caused the event, or @c BUTTON_NONE.
  uint32_t _button;
};

// ============================================================================
// [Fog::UIEngineWheelEvent]
// ============================================================================

//! @brief @ref UIEngineWindow wheel event.
struct FOG_NO_EXPORT UIEngineWheelEvent : public UIEngineMouseEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineWheelEvent(uint32_t code, uint32_t flags) :
    UIEngineMouseEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineWheelEvent(_Uninitialized) :
    UIEngineMouseEvent(UNINITIALIZED)
  {
  }
};

// ============================================================================
// [Fog::UIEngineUpdateEvent]
// ============================================================================

//! @brief @ref UIEngineWindow update event.
struct FOG_NO_EXPORT UIEngineUpdateEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineUpdateEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEngineUpdateEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
  {
  }
};

// ============================================================================
// [Fog::UIEnginePaintEvent]
// ============================================================================

//! @brief @ref UIEngineWindow paint event.
struct FOG_NO_EXPORT UIEnginePaintEvent : public UIEngineEvent
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEnginePaintEvent(uint32_t code, uint32_t flags) :
    UIEngineEvent(code, flags)
  {
  }

  explicit FOG_INLINE UIEnginePaintEvent(_Uninitialized) :
    UIEngineEvent(UNINITIALIZED)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get painter.
  FOG_INLINE Painter* getPainter() const { return _painter; }

  //! @brief Get bounding rectangle which was set to painter.
  //!
  //! Can be used to optimize calculations needed to paint, but would be
  //! probably unused most of time.
  FOG_INLINE const RectI& getPaintRect() const { return _paintRect; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Painter* _painter;
  RectI _paintRect;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_UIENGINEEVENT_H
