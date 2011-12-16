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

//! @brief Frame-buffer base event.
struct FOG_NO_EXPORT FbEvent
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE uint32_t getFlags() const { return _flags; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Event type (see @c FB_EVENT_CODE).
  uint32_t _type;
  //! @brief Event flags (see @c FB_EVENT_FLAG).
  uint32_t _flags;
};

//! @brief Frame-buffer geometry event.
struct FOG_NO_EXPORT FbGeometryEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getOrientation() const { return _orientation; }

  FOG_INLINE const RectI& getWindowGeometry() const { return _windowGeometry; }
  FOG_INLINE const RectI& getClientGeometry() const { return _clientGeometry; }

  FOG_INLINE const RectI& getOldWindowGeometry() const { return _oldWindowGeometry; }
  FOG_INLINE const RectI& getOldClientGeometry() const { return _oldClientGeometry; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Window orientation (@see ORIENTATION).
  uint32_t _orientation;
  //! @brief Reserved (alignment).
  uint32_t _reserved;

  //! @brief New window geometry.
  RectI _windowGeometry;
  //! @brief New client geometry.
  RectI _clientGeometry;

  //! @brief Old window geometry.
  RectI _oldWindowGeometry;
  //! @brief Old client geometry.
  RectI _oldClientGeometry;
};

//! @brief Frame-buffer key event.
struct FOG_NO_EXPORT FbKeyEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getButtonMask() const { return _buttonMask; }
  FOG_INLINE uint32_t getModifierMask() const { return _modifierMask; }

  FOG_INLINE uint32_t getKey() const { return _key; }
  FOG_INLINE uint32_t getSystemCode() const { return _systemCode; }

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
};

//! @brief Frame-buffer mouse event.
struct FOG_NO_EXPORT FbMouseEvent : public FbEvent
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getButtonMask() const { return _buttonMask; }
  FOG_INLINE uint32_t getModifierMask() const { return _modifierMask; }

  FOG_INLINE const PointI& getPosition() const { return _position; }
  FOG_INLINE const PointI& getOffset() const { return _offset; }

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

  //! @brief Button that caused the action, or @c BUTTON_NONE.
  uint32_t _button;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBEVENT_H
