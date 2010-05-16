// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_EVENT_H
#define _FOG_GUI_EVENT_H

// [Dependencies]
#include <Fog/Core/Event.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Widget;
struct Painter;

//! @addtogroup Fog_Gui_Widget
//! @{

// ============================================================================
// [Fog::ChildEvent]
// ============================================================================

struct FOG_API ChildEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ChildEvent(uint32_t code = 0, Widget* child = NULL);
  virtual ~ChildEvent();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE Widget* child() const { return _child; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Widget* _child;
};

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
};

// ============================================================================
// [Fog::ConfigureEvent]
// ============================================================================

struct FOG_API ConfigureEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ConfigureEvent();
  virtual ~ConfigureEvent();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  enum CHANGED_FLAGS
  {
    CHANGED_POSITION = (1 << 0),
    CHANGED_SIZE = (1 << 1),
    CHANGED_WINDOW_POSITION = (1 << 2),
    CHANGED_WINDOW_SIZE = (1 << 3),
    CHANGED_ORIENTATION = (1 << 4)
  };

  FOG_INLINE const IntRect& getGeometry() const { return _geometry; }
  FOG_INLINE uint32_t getChangedFlags() const { return _changed; }

  FOG_INLINE bool isChangedPosition() const { return (_changed & CHANGED_POSITION) != 0; }
  FOG_INLINE bool isChangedSize() const { return (_changed & CHANGED_SIZE) != 0; }
  FOG_INLINE bool isChangedWindowPosition() const { return (_changed & CHANGED_WINDOW_POSITION) != 0; }
  FOG_INLINE bool isChangedWindowSize() const { return (_changed & CHANGED_WINDOW_SIZE) != 0; }
  FOG_INLINE bool isChangedOrientation() const { return (_changed & CHANGED_ORIENTATION) != 0; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // TODO: Is this needed?
  IntRect _geometry;
  uint32_t _changed;
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

  FOG_INLINE const IntPoint& getOrigin() const { return _origin; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  IntPoint _origin;
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
  
  //! @brief Returns key code that depends to windowing system.
  FOG_INLINE uint32_t getSystemCode() const { return _systemCode; }
  
  //! @brief Returns key code translated to unicode character, can be zero if it's not possible to translate it.
  FOG_INLINE Char getUnicode() const { return _unicode; }

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
  Char _unicode;
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
  //! area (depends to event type).
  FOG_INLINE const IntPoint& getPosition() const { return _position; }

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
  IntPoint _position;

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

// ============================================================================
// [Fog::ThemeEvent]
// ============================================================================

struct FOG_API ThemeEvent : public Event
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ThemeEvent(uint32_t code);
  virtual ~ThemeEvent();
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
#endif // _FOG_GUI_EVENT_H
