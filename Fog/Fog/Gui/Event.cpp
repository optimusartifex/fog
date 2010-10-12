// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Static.h>
#include <Fog/Gui/Event.h>

namespace Fog {

// ============================================================================
// [Fog::LayoutEvent]
// ============================================================================

LayoutEvent::LayoutEvent(uint32_t code) :
  Event(code) 
{
}

LayoutEvent::~LayoutEvent()
{
}

// ============================================================================
// [Fog::StateEvent]
// ============================================================================

StateEvent::StateEvent(uint32_t code) :
  Event(code) 
{
}

StateEvent::~StateEvent()
{
}

// ============================================================================
// [Fog::VisibilityEvent]
// ============================================================================

VisibilityEvent::VisibilityEvent(uint32_t code) :
  Event(code) 
{
}

VisibilityEvent::~VisibilityEvent()
{
}

// ============================================================================
// [Fog::GeometryEvent]
// ============================================================================

GeometryEvent::GeometryEvent() : 
  Event(EVENT_GEOMETRY),
  _widgetGeometry(0, 0, 0, 0),
  _clientGeometry(0, 0, 0, 0),
  _changedFlags(0)
{
}

GeometryEvent::~GeometryEvent()
{
}

// ============================================================================
// [Fog::OriginEvent]
// ============================================================================

OriginEvent::OriginEvent() :
  Event(EVENT_ORIGIN),
  _origin(0, 0)
{
}

OriginEvent::~OriginEvent()
{
}

// ============================================================================
// [Fog::FocusEvent]
// ============================================================================

FocusEvent::FocusEvent(uint32_t code, uint32_t reason) :
  Event(code), _reason(reason) 
{
}

FocusEvent::~FocusEvent()
{
}

// ============================================================================
// [Fog::KeyEvent]
// ============================================================================

KeyEvent::KeyEvent(uint32_t code) :
  Event(code),
  _key(0),
  _modifiers(0),
  _systemCode(0),
  _unicode(0),
  _isRepeated(false)
{
}

KeyEvent::~KeyEvent()
{
}

// ============================================================================
// [Fog::MouseEvent]
// ============================================================================

MouseEvent::MouseEvent(uint32_t code) : 
  Event(code),
  _button(0),
  _modifiers(0),
  _position(-1, -1),
  _isOutside(false),
  _isRepeated(false)
{
}

MouseEvent::~MouseEvent()
{
}

// ============================================================================
// [Fog::SelectionEvent]
// ============================================================================

SelectionEvent::SelectionEvent(uint32_t code) : 
  Event(code)
{
}

SelectionEvent::~SelectionEvent()
{
}

// ============================================================================
// [Fog::PaintEvent]
// ============================================================================

PaintEvent::PaintEvent(uint32_t code) : 
  Event(code),
  _painter(NULL)
{
}

PaintEvent::~PaintEvent()
{
}

// ============================================================================
// [Fog::CloseEvent]
// ============================================================================

CloseEvent::CloseEvent() : 
  Event(EVENT_CLOSE)
{
}

CloseEvent::~CloseEvent()
{
}

// ============================================================================
// [Fog::CheckEvent]
// ============================================================================

CheckEvent::CheckEvent(uint32_t code) : 
  Event(code),
  _status(code == EVENT_CHECK)
{
}

CheckEvent::~CheckEvent()
{
}

// ============================================================================
// [Fog::ThemeEvent]
// ============================================================================

ThemeEvent::ThemeEvent(uint32_t code) : 
  Event(code)
{
}

ThemeEvent::~ThemeEvent()
{
}

AnimationEvent::AnimationEvent(uint32_t code) : 
  Event(code)
{
}

AnimationEvent::~AnimationEvent()
{
}


} // Fog namespace
