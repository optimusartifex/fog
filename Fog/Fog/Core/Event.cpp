// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Atomic.h>
#include <Fog/Core/AutoLock.h>
#include <Fog/Core/Event.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Object.h>

namespace Fog {

// ============================================================================
// [Fog::Event]
// ============================================================================

static Atomic<uint32_t> uid_val = FOG_ATOMIC_SET(EVENT_UID);

void Event::run()
{
  if (!wasDeleted())
  {
    Object* r = getReceiver();

    // Remove object from its event queue
    {
      Fog::AutoLock locked(*fog_object_lock);

      if (this == r->_events)
      {
        // This is LAST event
        FOG_ASSERT(_prev == NULL);
        FOG_ASSERT(_next == NULL);

        r->_events = NULL;
      }
      else
      {
        // This is not last event
        FOG_ASSERT(_prev == NULL);
        FOG_ASSERT(_next);

        _next->_prev = NULL;
        _next = NULL;
      }
    }

    r->onEvent(this);
  }
}

Event* Event::clone() const 
{
  return ( new(std::nothrow) Event(getCode(), getFlags()) )->_cloned(getReceiver());
}

uint32_t Event::uid()
{
  uint32_t id = uid_val.addXchg(1);

  if (id == (uint32_t)-1)
  {
    fog_fail("Fog::Event::uid() - Event counter reached to the end.");
  }

  return id;
}

// ============================================================================
// [Fog::CreateEvent]
// ============================================================================

CreateEvent::CreateEvent() : Event(EVENT_CREATE)
{
}

// ============================================================================
// [Fog::DestroyEvent]
// ============================================================================

DestroyEvent::DestroyEvent()  : Event(EVENT_DESTROY)
{
}

void DestroyEvent::run()
{
  Event::run();
}

// ============================================================================
// [Fog::TimerEvent]
// ============================================================================

TimerEvent::TimerEvent(Timer* timer) : 
  Event(EVENT_TIMER),
  _timer(timer)
{
}

Event* TimerEvent::clone()
{
  return ( new(std::nothrow) TimerEvent(getTimer()) )->_cloned(getReceiver());
}

// ============================================================================
// [Fog::PropertyChangedEvent]
// ============================================================================

PropertyChangedEvent::PropertyChangedEvent(const String& name) :
  Event(EVENT_CHANGE_PROPERTY),
  _name(name)
{
}

PropertyChangedEvent::~PropertyChangedEvent()
{
}

Event* PropertyChangedEvent::clone() const
{ 
  return ( new(std::nothrow) PropertyChangedEvent(getName()) )->_cloned(getReceiver());
}

} // Fog namespace
