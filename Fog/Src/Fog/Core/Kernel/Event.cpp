// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Kernel/Event.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/Logger.h>

namespace Fog {

// ============================================================================
// [Fog::Event]
// ============================================================================

static Atomic<uint32_t> uid_val = FOG_ATOMIC_SET(EVENT_UID);

Event::Event(uint32_t code, uint32_t flags)
{
  _code = code;
  _flags = flags;
  _receiver = NULL;
  _prev = NULL;
  _next = NULL;
  _wasDeleted = 0;
}

void Event::run()
{
  if (!wasDeleted())
  {
    Object* r = getReceiver();

    // Remove object from its event queue
    {
      Fog::AutoLock locked(Object::_internalLock);

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
  return ( fog_new Event(getCode(), getFlags()) )->_cloned(_receiver);
}

uint32_t Event::uid()
{
  uint32_t id = uid_val.addXchg(1);

  if (id == 0xFFFFFFFF)
  {
    Logger::fatal("Fog::Event", "uid", "Too many UIDs created.");
    Application::terminate(-1);
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
// [Fog::ChildEvent]
// ============================================================================

ChildEvent::ChildEvent(uint32_t code, Object* child) :
  Event(code),
  _child(child)
{
}

ChildEvent::~ChildEvent()
{
}

Event* ChildEvent::clone() const
{
  return ( fog_new ChildEvent(getCode(), getChild()) )->_cloned(_receiver);
}

// ============================================================================
// [Fog::PropertyEvent]
// ============================================================================

PropertyEvent::PropertyEvent(const InternedStringW& name) :
  Event(EVENT_PROPERTY),
  _name(name)
{
}

PropertyEvent::~PropertyEvent()
{
}

Event* PropertyEvent::clone() const
{
  return ( fog_new PropertyEvent(getName()) )->_cloned(_receiver);
}

// ============================================================================
// [Fog::TimerEvent]
// ============================================================================

TimerEvent::TimerEvent(Timer* timer) :
  Event(EVENT_TIMER),
  _timer(timer)
{
}

TimerEvent::~TimerEvent()
{
}

Event* TimerEvent::clone() const
{
  return ( fog_new TimerEvent(getTimer()) )->_cloned(_receiver);
}

} // Fog namespace
