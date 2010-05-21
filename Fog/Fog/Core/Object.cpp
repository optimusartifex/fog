// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/AutoLock.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Event.h>
#include <Fog/Core/EventLoop.h>
#include <Fog/Core/Hash.h>
#include <Fog/Core/HashUtil.h>
#include <Fog/Core/List.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Object.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Core/Thread.h>
#include <Fog/Core/Value.h>

FOG_CVAR_DECLARE Fog::Lock* fog_object_lock;

namespace Fog {

// ============================================================================
// [Fog::Object_Local]
// ============================================================================

struct Object_Local
{
  FOG_INLINE Object_Local()
  {
    static const char object_string[] = "Fog::Object";

    fog_object_lock = &object_lock;

    object_metaClass.base = NULL;
    object_metaClass.name = object_string;
    object_metaClass.hash = HashUtil::hashString(object_string, FOG_ARRAY_SIZE(object_string)-1);
  }
  FOG_INLINE ~Object_Local()
  {
    fog_object_lock = NULL;
  }

  Lock object_lock;
  MetaClass object_metaClass;
};

static Static<Object_Local> object_local;

// ============================================================================
// [Fog::Object]
// ============================================================================

Object::Object() :
  _flags(0),
  _objectId(0),
  _thread(Thread::getCurrent()),
  _events(NULL)
{
}

Object::~Object()
{
  // Delete all connections
  if (!_connection.isEmpty()) removeAllListeners();

  // Demove all posted events.
  if (_events)
  {
    AutoLock locked(*fog_object_lock);

    // set "wasDeleted" in all pending events
    Event* e = _events;
    while (e)
    {
      e->_wasDeleted = 1;
      // Go to previous, because newest pending events are stored
      // first
      e = e->_prev;
    }
  }
}

void Object::deleteLater()
{
  if (!(_flags & OBJ_POSTED_DELETE_LATER_EVENT))
  {
    if (!getThread()) 
    {
      fog_stderr_msg("Fog::Object", "deleteLater", "Can't post DeleteLater event, because object thread has no event loop");
      return;
    }

    _flags |= OBJ_POSTED_DELETE_LATER_EVENT;
    postEvent(new(std::nothrow) DestroyEvent());
  }
}

void Object::destroy()
{
  if (canDelete()) delete this;
}

// ============================================================================
// [Fog::Object - Meta class]
// ============================================================================

const MetaClass* Object::getStaticMetaClass()
{
  return &object_local.instance().object_metaClass;
}

const MetaClass* Object::getMetaClass() const
{
  return &object_local.instance().object_metaClass;
}

// ============================================================================
// [Fog::Object - ID / Name]
// ============================================================================

void Object::setObjectId(uint32_t objectId)
{
  if (_objectId != objectId)
  {
    _objectId = objectId;
  }
}

void Object::setObjectName(const String& objectName)
{
  if (_objectName != objectName)
  {
    _objectName = objectName;
    _objectName.squeeze();
  }
}

// ============================================================================
// [Fog::Object - Properties]
// ============================================================================

err_t Object::getProperty(const String& name, Value& value) const
{
  ManagedString m_name;

  if (m_name.setIfManaged(name) == ERR_RT_OBJECT_NOT_FOUND)
    return ERR_OBJECT_INVALID_PROPERTY;

  return getProperty(m_name, value);
}

err_t Object::setProperty(const String& name, const Value& value)
{
  ManagedString m_name;

  if (m_name.setIfManaged(name) == ERR_RT_OBJECT_NOT_FOUND)
    return ERR_OBJECT_INVALID_PROPERTY;

  return setProperty(m_name, value);
}

err_t Object::getProperty(const ManagedString& name, Value& value) const
{
  FOG_UNUSED(name);
  FOG_UNUSED(value);

  return ERR_OBJECT_INVALID_PROPERTY;
}

err_t Object::setProperty(const ManagedString& name, const Value& value)
{
  FOG_UNUSED(name);
  FOG_UNUSED(value);

  return ERR_OBJECT_INVALID_PROPERTY;
}

// ============================================================================
// [Fog::Object - Listeners]
// ============================================================================

bool Object::addListener(uint32_t code, void (*fn)(Event*))
{
  Delegate1<Event*> del(fn);
  return _addListener(code, NULL, (const void*)&del, DELEGATE_EVENT);
}

bool Object::addListener(uint32_t code, void (*fn)())
{
  Delegate0<> del(fn);
  return _addListener(code, NULL, (const void*)&del, DELEGATE_VOID);
}

bool Object::removeListener(uint32_t code, void (*fn)(Event*))
{
  Delegate1<Event*> del(fn);
  return _removeListener(code, NULL, (const void*)&del, DELEGATE_EVENT);
}

bool Object::removeListener(uint32_t code, void (*fn)())
{
  Delegate0<> del(fn);
  return _removeListener(code, NULL, (const void*)&del, DELEGATE_VOID);
}

uint Object::removeListener(Object* listener)
{
  AutoLock locked(*fog_object_lock);
  uint result = 0;

  ObjectConnection* prev;
  ObjectConnection* conn;
  ObjectConnection* next;

  UnorderedHash<uint32_t, ObjectConnection*>::MutableIterator it(_connection);

  for (it.toStart(); it.isValid(); )
  {
    prev = NULL;
    conn = it.value();

    do {
      next = conn->next;
      if (conn->listener == listener)
      {
        result++;

        if (listener)
        {
          listener->_backref.remove(conn);
        }

        delete conn;

        if (prev)
          prev->next = next;
        else if (next)
          it.value() = next;
        else
        {
          it.remove();
          goto removed;
        }
      }

      prev = conn;
      conn = next;
    } while (conn);

    it.toNext();
removed:
    continue;
  }

  return result;
}

uint Object::removeAllListeners()
{
  AutoLock locked(*fog_object_lock);

  uint result = 0;

  ObjectConnection* conn;
  ObjectConnection* next;

  UnorderedHash<uint32_t, ObjectConnection*>::MutableIterator it(_connection);

  for (it.toStart(); it.isValid(); it.toNext())
  {
    conn = it.value();

    do {
      next = conn->next;
      result++;

      if (conn->listener)
      {
        conn->listener->_backref.remove(conn);
      }

      delete conn;
      conn = next;
    } while (conn);
  }

  _connection.clear();
  return result;
}

// Private.
bool Object::_addListener(uint32_t code, Object* listener, const void* del, uint32_t type)
{
  AutoLock locked(*fog_object_lock);

  ObjectConnection* prev = NULL;
  ObjectConnection* conn = _connection.value(code, NULL);
  Delegate0<> d = *(const Delegate0<> *)del;

  if (conn)
  {
    do {
      if (conn->delegateVoid.instance() == d) return false;
      prev = conn;
      conn = conn->next;
    } while (conn);
  }

  conn = new(std::nothrow) ObjectConnection;
  conn->next = NULL;
  conn->attachedObject = this;
  conn->listener = listener;
  conn->delegateVoid.instance() = d;
  conn->type = type;
  conn->code = code;

  if (prev)
    prev->next = conn;
  else
    _connection.put(code, conn);

  if (listener) listener->_backref.append(conn);
  return true;
}

bool Object::_removeListener(uint32_t code, Object* listener, const void* del, uint32_t type)
{
  AutoLock locked(*fog_object_lock);

  ObjectConnection* prev = NULL;
  ObjectConnection* conn = _connection.value(code, NULL);
  if (!conn) return false;
  Delegate0<> d = *(const Delegate0<> *)del;

  do {
    ObjectConnection* next = conn->next;

    if (conn->delegateVoid.instance() == d)
    {
      FOG_ASSERT(conn->listener == listener);

      if (prev)
        prev->next = next;
      else if (next)
        _connection.put(code, next);
      else
        _connection.remove(code);

      if (listener) listener->_backref.remove(conn);
      
      delete conn;
      return true;
    }

    prev = conn;
    conn = next;
  } while (conn);

  return false;
}

// TODO: Add removing possibility for currently called listeners
void Object::_callListeners(Event* e)
{
  uint32_t code = e->getCode();

  ObjectConnection* conn = _connection.value(code);
  if (!conn) return;

  do {
    if (conn->type == DELEGATE_VOID)
      conn->delegateVoid.instance()();
    else
      conn->delegateEvent.instance()(e);
  } while ((conn = conn->next));
}

// ============================================================================
// [Fog::Object - Events]
// ============================================================================

void Object::postEvent(Event* e)
{
  Thread* thread;
  EventLoop* eventLoop;

  if ((thread = getThread()) == NULL) goto fail;
  if ((eventLoop = thread->getEventLoop()) == NULL) goto fail;

  // Link event with object event queue.
  {
    AutoLock locked(*fog_object_lock);

    e->_prev = this->_events;
    this->_events = e;

    e->_flags |= Event::IS_POSTED;
  }

  // Post event (event is posted as task).
  eventLoop->postTask(e);
  return;

fail:
  fog_stderr_msg("Fog::Object", "postEvent", "Can't post event to object which has no owner thread or event loop. Deleting event.");
  delete e;
}

void Object::postEvent(uint32_t code)
{
  Event* e = new(std::nothrow) Event(code, 0);
  if (e) postEvent(e);
}

void Object::sendEvent(Event* e)
{
  // First set receiver to self.
  e->_receiver = this;

  // Send event to object onEvent().
  onEvent(e);

  // Send event to listeners.
  _callListeners(e);
}

void Object::sendEventById(uint32_t code)
{
  Event e(code, 0);
  sendEvent(&e);
}

// ============================================================================
// [Fog::Object - Event handlers]
// ============================================================================

// onEvent is normally defined by FOG_EVENT_BEGIN() and FOG_EVENT_END() macro,
// but Fog::Object is exception.
void Object::onEvent(Event* e)
{
  switch (e->getCode())
  {
    case EVENT_CREATE:
      onCreate(reinterpret_cast<CreateEvent*>(e));
      break;
    case EVENT_DESTROY:
      onDestroy(reinterpret_cast<DestroyEvent*>(e));
      break;
  }
}

void Object::onCreate(CreateEvent* e)
{
}

void Object::onDestroy(DestroyEvent* e)
{
}

void Object::onPropertyChanged(PropertyChangedEvent* e)
{
}

} // Fog namespace

// ============================================================================
// [fog_object_cast<>]
// ============================================================================

FOG_CAPI_DECLARE void* fog_object_cast_helper(Fog::Object* self, const Fog::MetaClass* targetMetaClass)
{
  using namespace Fog;

  FOG_ASSERT(self);
  const MetaClass* selfMetaClass = self->getMetaClass();

  for (;;)
  {
    // compare meta classes for match. Each class has only one meta class,
    // so pointers comparision is the best way
    if (selfMetaClass == targetMetaClass) return (void*)self;
    // iterate over base classes and return if there is no one
    if ((selfMetaClass = selfMetaClass->base) == NULL) return NULL;
  }
}

FOG_CAPI_DECLARE void* fog_object_cast_string(Fog::Object* self, const char* className)
{
  using namespace Fog;

  FOG_ASSERT(self);

  const MetaClass* metaClass = self->getMetaClass();
  uint32_t classHash = HashUtil::hashString(className, DETECT_LENGTH);

  for (;;)
  {
    // Compare hashes and string class names.
    if (metaClass->hash == classHash && strcmp(
      (const char*)metaClass->name,
      (const char*)className) == 0)
    {
      return (void*)self;
    }

    // Iterate over base classes and return if there is no one.
    if ((metaClass = metaClass->base) == NULL) return NULL;
  }
}

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_object_init(void)
{
  using namespace Fog;

  object_local.init();
  return ERR_OK;
}

FOG_INIT_DECLARE void fog_object_shutdown(void)
{
  using namespace Fog;

  object_local.destroy();
}

