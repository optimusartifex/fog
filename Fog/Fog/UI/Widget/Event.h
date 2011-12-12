// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_WIDGET_EVENT_H
#define _FOG_UI_WIDGET_EVENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Event.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Widget;

//! @addtogroup Fog_Gui_Widget
//! @{

// ============================================================================
// [Fog::StateEvent]
// ============================================================================

struct FOG_API StateEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  StateEvent(uint32_t code = 0);
  virtual ~StateEvent();
};

// ============================================================================
// [Fog::VisibilityEvent]
// ============================================================================

struct FOG_API VisibilityEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  VisibilityEvent(uint32_t code = 0);
  virtual ~VisibilityEvent();

  uint32_t _visibilitystate;
};

// ============================================================================
// [Fog::GeometryEvent]
// ============================================================================

struct FOG_API GeometryEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GeometryEvent();
  virtual ~GeometryEvent();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  enum CHANGED_FLAGS
  {
    CHANGED_WIDGET_POSITION = 0x0001,
    CHANGED_WIDGET_SIZE = 0x0002,

    CHANGED_CLIENT_POSITION = 0x0004,
    CHANGED_CLIENT_SIZE = 0x0008,

    CHANGED_WINDOW_POSITION = 0x0010,
    CHANGED_WINDOW_SIZE = 0x0020,

    CHANGED_ORIENTATION = 0x0080
  };

  FOG_INLINE const RectI& getWidgetGeometry() const { return _widgetGeometry; }
  FOG_INLINE const RectI& getClientGeometry() const { return _clientGeometry; }
  FOG_INLINE uint32_t getChangedFlags() const { return _changedFlags; }

  FOG_INLINE bool isChangedWidgetPosition() const { return (_changedFlags & CHANGED_WIDGET_POSITION) != 0; }
  FOG_INLINE bool isChangedWidgetSize    () const { return (_changedFlags & CHANGED_WIDGET_SIZE    ) != 0; }
  FOG_INLINE bool isChangedClientPosition() const { return (_changedFlags & CHANGED_CLIENT_POSITION) != 0; }
  FOG_INLINE bool isChangedClientSize    () const { return (_changedFlags & CHANGED_CLIENT_SIZE    ) != 0; }
  FOG_INLINE bool isChangedWindowPosition() const { return (_changedFlags & CHANGED_WINDOW_POSITION) != 0; }
  FOG_INLINE bool isChangedWindowSize    () const { return (_changedFlags & CHANGED_WINDOW_SIZE    ) != 0; }
  FOG_INLINE bool isChangedOrientation   () const { return (_changedFlags & CHANGED_ORIENTATION    ) != 0; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectI _widgetGeometry;
  RectI _clientGeometry;
  uint32_t _changedFlags;
};

// ============================================================================
// [Fog::OriginEvent]
// ============================================================================

struct FOG_API OriginEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  OriginEvent();
  virtual ~OriginEvent();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointI& getOrigin() const { return _origin; }
  FOG_INLINE const PointI& getDifference() const { return _difference; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointI _origin;
  PointI _difference;
};

// ============================================================================
// [Fog::FocusEvent]
// ============================================================================

struct FOG_API FocusEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FocusEvent(uint32_t code = 0, uint32_t reason = FOCUS_REASON_NONE);
  virtual ~FocusEvent();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getReason() const { return _reason; }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  uint32_t _reason;
};

// ============================================================================
// [Fog::KeyEvent]
// ============================================================================

struct FOG_API KeyEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  KeyEvent(uint32_t code = 0);
  virtual ~KeyEvent();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Returns key code. See @c KEY_CODE enum for possible ones.
  FOG_INLINE uint32_t getKey() const { return _key; }

  //! @brief Returns modifiers. See @c MODIFIER_CODE enum for possible ones.
  FOG_INLINE uint32_t getModifiers() const { return _modifiers; }

  //! @brief Returns key code that depends on the windowing system.
  FOG_INLINE uint32_t getSystemCode() const { return _systemCode; }

  //! @brief Returns key code translated to unicode character, can be zero if it's not possible to translate it.
  FOG_INLINE CharW getUnicode() const { return _unicode; }

  //! @brief Get whether key was repeated by timer.
  FOG_INLINE bool isRepeated() const { return _isRepeated; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Key code.
  uint32_t _key;
  //! @brief Modifiers.
  uint32_t _modifiers;
  //! @brief System key code (OS dependent, shouldn't be used).
  uint32_t _systemCode;
  //! @brief Key code translated to unicode character, can be zero.
  CharW _unicode;
  //! @brief Whether key was repeated by timer.
  bool _isRepeated;
};

// ============================================================================
// [Fog::MouseEvent]
// ============================================================================

struct FOG_API MouseEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MouseEvent(uint32_t code = 0);
  virtual ~MouseEvent();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Returns mouse button that was pressed or released.
  FOG_INLINE uint32_t getButton() const { return _button; }

  //! @brief Returns modifierd.
  FOG_INLINE uint32_t getModifiers() const { return _modifiers; }

  //! @brief Returns mouse position relative to client or non-client
  //! area (depends on the event type).
  FOG_INLINE const PointI& getPosition() const { return _position; }

  //! @brief Get if event was generated outside of widget.
  FOG_INLINE bool isOutside() const { return _isOutside; }

  //! @brief Get whether key was repeated by timer.
  FOG_INLINE bool isRepeated() const { return _isRepeated; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Mouse button that was pressed or released.
  //!
  //! Enter, leave and motion events not using this member, so it's (uint32_t)-1.
  uint32_t _button;

  //! @brief Modifiers.
  uint32_t _modifiers;

  //! @brief Relative mouse position.
  PointI _position;

  //! @brief True if event is outside of widget (grabbing).
  bool _isOutside;

  //! @brief Whether mouse press was repeated by timer.
  bool _isRepeated;
};

// ============================================================================
// [Fog::SelectionEvent]
// ============================================================================

struct FOG_API SelectionEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SelectionEvent(uint32_t code = 0);
  virtual ~SelectionEvent();
};

// ============================================================================
// [Fog::PaintEvent]
// ============================================================================

struct FOG_API PaintEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PaintEvent(uint32_t code = 0);
  virtual ~PaintEvent();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE Painter* getPainter() const { return _painter; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Painter* _painter;
};

// ============================================================================
// [Fog::CloseEvent]
// ============================================================================

struct FOG_API CloseEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  CloseEvent();
  virtual ~CloseEvent();
};

// ============================================================================
// [Fog::CheckEvent]
// ============================================================================

struct FOG_API CheckEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  CheckEvent(uint32_t code = EVENT_CHECK);
  virtual ~CheckEvent();

  // [Methods]

  FOG_INLINE uint getStatus() const { return _status; }

  // [Members]

  uint _status;
};

//! @}

//! @addtogroup Fog_Gui_Layout
//! @{

// ============================================================================
// [Fog::LayoutEvent]
// ============================================================================

struct FOG_API LayoutEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  LayoutEvent(uint32_t code = 0);
  virtual ~LayoutEvent();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_WIDGET_EVENT_H
