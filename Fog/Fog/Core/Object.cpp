// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Event.h>
#include <Fog/Core/EventLoop.h>
#include <Fog/Core/Hash.h>
#include <Fog/Core/HashUtil.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Object.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Core/Thread.h>
#include <Fog/Core/Value.h>

namespace Fog {

// ============================================================================
// [Fog::Object]
// ============================================================================

const MetaClass* Object::_staticMetaClass;
Static<Lock> Object::_internalLock;

Object::Object() :
  _objectFlags(0),
  _objectId(0),
  _parent(NULL),
  _homeThread(Thread::getCurrent()),
  _events(NULL)
{
}

Object::~Object()
{
  // Delete all children.
  err_t err = _deleteChildren();
  if (err != ERR_OK)
  {
    fog_stderr_msg("Fog::Object", "~Object()", "_deleteChildren() failed (%u).");
  }

  // Delete all connections.
  if (!_forwardConnection.isEmpty())
  {
    removeAllListeners();
  }

  // Delete all posted events.
  if (_events)
  {
    AutoLock locked(Object::_internalLock.instance());

    // Set "wasDeleted" for all pending events.
    Event* e = _events;
    while (e)
    {
      // Event will be deleted by event loop, we need only to mark it for
      // deletion so it won't be processed (called).
      e->_wasDeleted = 1;

      // Go to previous, because newest pending events are stored first.
      e = e->_prev;
    }
  }
}

void Object::destroyLater()
{
  if (!(_objectFlags & OBJECT_FLAG_DESTROY_LATER))
  {
    if (!getHomeThread()) 
    {
      fog_stderr_msg("Fog::Object", "destroyLater", "Can't post DeleteLater event, because object thread has no event loop");
      return;
    }

    _objectFlags |= OBJECT_FLAG_DESTROY_LATER;
    postEvent(fog_new DestroyEvent());
  }
}

void Object::destroy()
{
  // First delete all children. This is last oportunity to remove them on
  // current class instance (destructors are changing vtable).
  err_t err = _deleteChildren();
  if (err != ERR_OK)
  {
    fog_stderr_msg("Fog::Object", "destroy()", "_deleteChildren() failed (%u).");
  }

  if (canDelete()) fog_delete(this);
}

// ============================================================================
// [Fog::Object - Meta class]
// ============================================================================

const MetaClass* Object::getStaticMetaClass()
{
  return _staticMetaClass;
}

const MetaClass* Object::_getStaticMetaClassRace(MetaClass** p)
{
  while (AtomicCore<sysuint_t>::get((sysuint_t*)p) == 0)
  {
    // Yield is not optimal, but this should really rarely happen and if we
    // had the luck then there is 100% probability that it will not happen 
    // again.
    Thread::_yield();
  }

  return *p;
}

const MetaClass* Object::getObjectMetaClass() const
{
  return _staticMetaClass;
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
// [Fog::Object - Object Hierarchy]
// ============================================================================

err_t Object::setParent(Object* parent)
{
  // If our parent is a given parent then do nothing, it's not error.
  if (_parent == parent) 
  {
    return ERR_OK;
  }

  // If we have a parent then it's needed to break the hierarchy first.
  if (_parent != NULL) 
  {
    FOG_RETURN_ON_ERROR(_parent->removeChild(this));
  }

  // Now set the new parent. NULL is valid value so we must make sure the
  // object is not null before calling its method.
  if (parent)
  {
    FOG_RETURN_ON_ERROR(parent->addChild(this));
  }

  return ERR_OK;
}

err_t Object::addChild(Object* child)
{
  FOG_ASSERT(child != NULL);

  // If object is already in our hierarchy then we return quitly.
  if (child->getParent() == this)
    return ERR_OK;

  // If object has different parent then it's an error.
  if (child->hasParent())
    return ERR_OBJECT_ALREADY_PART_OF_HIERARCHY;

  // Okay, now we can call the virtual _addChild() method.
  return _addChild(_children.getLength(), child);
}

err_t Object::removeChild(Object* child)
{
  FOG_ASSERT(child != NULL);

  if (child->getParent() != this)
    return ERR_OBJECT_NOT_PART_OF_HIERARCHY;

  sysuint_t index = _children.indexOf(child);
  // It must be here if parent check passed!
  FOG_ASSERT(index != INVALID_INDEX);

  // Okay, now we can call the virtual _removeChild() method.
  return _removeChild(index, child);
}

err_t Object::_addChild(sysuint_t index, Object* child)
{
  // Index must be valid or equal to children list length (this means append).
  FOG_ASSERT(index <= _children.getLength());

  FOG_RETURN_ON_ERROR(_children.insert(index, child));
  child->_parent = this;

  // Send 'child-add' event.
  ChildEvent e(EVENT_CHILD_ADD, child);
  sendEvent(&e);

  return ERR_OK;
}

err_t Object::_removeChild(sysuint_t index, Object* child)
{
  // Index must be valid.
  FOG_ASSERT(index < _children.getLength() && _children.at(index) == child);

  FOG_RETURN_ON_ERROR(_children.removeAt(index));
  child->_parent = NULL;

  // Send 'child-remove' event.
  ChildEvent e(EVENT_CHILD_REMOVE, child);
  sendEvent(&e);

  return ERR_OK;
}

err_t Object::_deleteChildren()
{
  err_t err = ERR_OK;

  sysuint_t index;
  while ((index = _children.getLength()) != 0)
  {
    Object* child = _children.at(--index);
    if ((err = _removeChild(index, child)) != ERR_OK) break;
  }

  return err;
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
  return _addListener(code, NULL, (const void*)&del, OBJECT_EVENT_HANDLER_EVENTPTR);
}

bool Object::addListener(uint32_t code, void (*fn)())
{
  Delegate0<> del(fn);
  return _addListener(code, NULL, (const void*)&del, OBJECT_EVENT_HANDLER_VOID);
}

bool Object::removeListener(uint32_t code, void (*fn)(Event*))
{
  Delegate1<Event*> del(fn);
  return _removeListener(code, NULL, (const void*)&del, OBJECT_EVENT_HANDLER_EVENTPTR);
}

bool Object::removeListener(uint32_t code, void (*fn)())
{
  Delegate0<> del(fn);
  return _removeListener(code, NULL, (const void*)&del, OBJECT_EVENT_HANDLER_VOID);
}

uint Object::removeListener(Object* listener)
{
  AutoLock locked(Object::_internalLock.instance());
  uint result = 0;

  ObjectConnection* prev;
  ObjectConnection* conn;
  ObjectConnection* next;

  UnorderedHash<uint32_t, ObjectConnection*>::MutableIterator it(_forwardConnection);

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
          listener->_backwardConnection.remove(conn);
        }

        fog_delete(conn);

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
  AutoLock locked(Object::_internalLock.instance());

  uint result = 0;

  ObjectConnection* conn;
  ObjectConnection* next;

  UnorderedHash<uint32_t, ObjectConnection*>::MutableIterator it(_forwardConnection);

  for (it.toStart(); it.isValid(); it.toNext())
  {
    conn = it.value();

    do {
      next = conn->next;
      result++;

      if (conn->listener)
      {
        conn->listener->_backwardConnection.remove(conn);
      }

      fog_delete(conn);
      conn = next;
    } while (conn);
  }

  _forwardConnection.clear();
  return result;
}

// Private.
bool Object::_addListener(uint32_t code, Object* listener, const void* del, uint32_t type)
{
  AutoLock locked(Object::_internalLock.instance());

  ObjectConnection* prev = NULL;
  ObjectConnection* conn = _forwardConnection.value(code, NULL);
  Delegate0<> d = *(const Delegate0<> *)del;

  if (conn)
  {
    do {
      if (conn->delegateVoid.instance() == d) return false;
      prev = conn;
      conn = conn->next;
    } while (conn);
  }

  conn = fog_new ObjectConnection;
  // TODO: if (conn == NULL) ...
  conn->next = NULL;
  conn->attachedObject = this;
  conn->listener = listener;
  conn->delegateVoid.instance() = d;
  conn->type = type;
  conn->code = code;

  if (prev)
    prev->next = conn;
  else
    _forwardConnection.put(code, conn);

  if (listener) listener->_backwardConnection.append(conn);
  return true;
}

bool Object::_removeListener(uint32_t code, Object* listener, const void* del, uint32_t type)
{
  AutoLock locked(Object::_internalLock.instance());

  ObjectConnection* prev = NULL;
  ObjectConnection* conn = _forwardConnection.value(code, NULL);
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
        _forwardConnection.put(code, next);
      else
        _forwardConnection.remove(code);

      if (listener) listener->_backwardConnection.remove(conn);
      
      fog_delete(conn);
      return true;
    }

    prev = conn;
    conn = next;
  } while (conn);

  return false;
}

// OBJECT TODO: Add removing possibility for currently called listeners
void Object::_callListeners(Event* e)
{
  uint32_t code = e->getCode();

  ObjectConnection* conn = _forwardConnection.value(code);
  if (!conn) return;

  do {
    switch (conn->type)
    {
      case OBJECT_EVENT_HANDLER_VOID:
        conn->delegateVoid.instance()();
        break;
      case OBJECT_EVENT_HANDLER_EVENTPTR:
        conn->delegateEvent.instance()(e);
        break;
      default:
        FOG_ASSERT_NOT_REACHED();
        break;
    }
  } while ((conn = conn->next));
}

// ============================================================================
// [Fog::Object - Events]
// ============================================================================

void Object::postEvent(Event* e)
{
  Thread* thread;
  EventLoop* eventLoop;

  if ((thread = getHomeThread()) == NULL) goto fail;
  if ((eventLoop = thread->getEventLoop()) == NULL) goto fail;

  // Link event with object event queue.
  {
    AutoLock locked(_internalLock.instance());

    e->_prev = this->_events;
    this->_events = e;

    e->_flags |= Event::IS_POSTED;
  }

  // Post event (event is posted as task).
  eventLoop->postTask(e);
  return;

fail:
  fog_stderr_msg("Fog::Object", "postEvent", "Can't post event to object which has no home thread or event loop. Deleting event.");
  fog_delete(e);
}

void Object::postEvent(uint32_t code)
{
  Event* e = fog_new Event(code, 0);
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

void Object::sendEventByCode(uint32_t code)
{
  Event e(code, 0);
  sendEvent(&e);
}

// ============================================================================
// [Fog::Object - Event handlers]
// ============================================================================

// The onEvent() method is normally defined by FOG_EVENT_BEGIN() and
// FOG_EVENT_END() macros, but Fog::Object is an exception.
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

    case EVENT_CHILD_ADD:
    case EVENT_CHILD_REMOVE:
      onChild(reinterpret_cast<ChildEvent*>(e));
      break;

    case EVENT_PROPERTY:
      onProperty(reinterpret_cast<PropertyEvent*>(e));
      break;
  }
}

void Object::onCreate(CreateEvent* e)
{
}

void Object::onDestroy(DestroyEvent* e)
{
}

void Object::onChild(ChildEvent* e)
{
}

void Object::onProperty(PropertyEvent* e)
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
  const MetaClass* selfMetaClass = self->getObjectMetaClass();

  // Iterate over meta classes and try to find the target one.
  do {
    // Compare meta classes for match. Each class has only one meta class,
    // so pointers comparision is the best way here.
    if (selfMetaClass == targetMetaClass)
      return (void*)self;
    
    // Not match? Try base meta class, again and again until there is no
    // base class (Fog::Object is root).
  } while ((selfMetaClass = selfMetaClass->base) != NULL);

  // No match.
  return NULL;
}

FOG_CAPI_DECLARE void* fog_object_cast_string(Fog::Object* self, const char* className)
{
  using namespace Fog;

  FOG_ASSERT(self);

  const MetaClass* metaClass = self->getObjectMetaClass();
  uint32_t classHash = HashUtil::makeStringHash(className, DETECT_LENGTH);

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

  // Initialize the Object meta class.
  static MetaClass _privateObjectMetaClass;
  static const char _privateObjectClassName[] = "Fog::Object";

  _privateObjectMetaClass.base = NULL;
  _privateObjectMetaClass.name = _privateObjectClassName;
  _privateObjectMetaClass.hash = HashUtil::makeStringHash(
    _privateObjectClassName, FOG_ARRAY_SIZE(_privateObjectClassName) - 1);

  Object::_staticMetaClass = &_privateObjectMetaClass;

  // Initialize the internal lock.
  Object::_internalLock.init();

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_object_shutdown(void)
{
  using namespace Fog;

  // The meta class shouldn't be used from now.
  Object::_staticMetaClass = NULL;

  // Destroy the internal lock.
  Object::_internalLock.destroy();
}

