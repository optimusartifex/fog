// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Kernel/Event.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Memory/MemPool.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/HashUtil.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/Var.h>

namespace Fog {

// ============================================================================
// [Fog::Object - Statics]
// ============================================================================

const MetaClass* Object::_staticMetaClass;
Static<Lock> Object::_internalLock;

static Static<ObjectExtra> Object_extraNull;
static Static<Lock> Object_memPoolLock;
static Static<MemPool> Object_memPoolExtra;
static Static<MemPool> Object_memPoolConn;

// ============================================================================
// [Fog::Object - Helpers]
// ============================================================================

static ObjectExtra* ObjectExtra_create()
{
  ObjectExtra* extra;

  { // Synchronized.
    AutoLock locked(Object_memPoolLock);

    extra = reinterpret_cast<ObjectExtra*>(Object_memPoolExtra->alloc(sizeof(ObjectExtra)));
    if (FOG_IS_NULL(extra))
      return NULL;
  }

  return fog_new_p(extra) ObjectExtra();
}

static void ObjectExtra_destroy(ObjectExtra* extra)
{
  extra->~ObjectExtra();

  { // Synchronized.
    AutoLock locked(Object_memPoolLock);
    Object_memPoolExtra->free(extra);
  }
}

// ============================================================================
// [Fog::Object - Construction / Destruction]
// ============================================================================

enum { OBJECT_VTYPE_FLAGS = OBJECT_CREATE_STATIC };
enum { OBJECT_ADOPT_FLAGS = OBJECT_CREATE_WRAPPED };

Object::Object(uint32_t createFlags) :
  _vType(VAR_TYPE_OBJECT_REF | VAR_FLAG_NONE | (createFlags & OBJECT_VTYPE_FLAGS)),
  _objectFlags(createFlags & OBJECT_ADOPT_FLAGS),
  _objectExtra(&Object_extraNull),
  _parent(NULL),
  _homeThread(Thread::getCurrentThread()),
  _events(NULL)
{
}

Object::~Object()
{
  ObjectExtra* extra = _objectExtra;

  if (extra != &Object_extraNull)
  {
    // Delete all children.
    err_t err = _removeAll();

    if (FOG_IS_ERROR(err))
    {
      Debug::dbgFunc("Fog::Object", "~Object()", "_removeAll() failed, error=%u.\n.", err);
    }

    // Delete all connections.
    if (!extra->_forwardConnection.isEmpty())
    {
      removeAllListeners();
    }

    _objectExtra = &Object_extraNull;
    ObjectExtra_destroy(extra);
  }

  // Delete all posted events.
  if (_events)
  {
    AutoLock locked(Object::_internalLock);

    // Set "wasDeleted" for all pending events.
    Event* e = _events;

    while (e)
    {
      // Event will be deleted by an event loop, we need only to mark it for
      // deletion so it won't be processed (called).
      e->_wasDeleted = 1;

      // Go to previous, because newest pending events are stored first.
      e = e->_prev;
    }
  }
}

// ============================================================================
// [Fog::Object - Release]
// ============================================================================

void Object::release()
{
  // First delete all children. This is last opportunity to remove them on
  // current class instance (destructor changes virtual table).
  err_t err = _removeAll();

  if (FOG_IS_ERROR(err))
  {
    Debug::dbgFunc("Fog::Object", "release", "_removeAll() failed, error=%u.\n", err);
  }

  // Use delete operator only in case that the object is dynamically allocated
  // using fog_new() operator.
  if (!isStatic())
  {
    fog_delete(this);
  }
}

void Object::scheduleRelease()
{
  if ((_objectFlags & OBJECT_FLAG_RELEASE_SCHEDULED) != 0)
    return;

  Thread* thread = getHomeThread();

  if (thread == NULL)
  {
    Debug::dbgFunc("Fog::Object", "scheduleRelease",
      "Can't post RELEASE event, because there is no suitable event loop.\n");
    return;
  }

  _objectFlags |= OBJECT_FLAG_RELEASE_SCHEDULED;
  postEvent(fog_new DestroyEvent());
}

// ============================================================================
// [Fog::Object - Object Extra]
// ============================================================================

//! @brief Get @c ObjectExtra instance ready for modification.
ObjectExtra* Object::getMutableExtra()
{
  if (FOG_LIKELY(_objectExtra != &Object_extraNull))
    return _objectExtra;

  ObjectExtra* extra = ObjectExtra_create();
  if (FOG_IS_NULL(extra))
    return NULL;

  _objectExtra = extra;
  return extra;
}

// ============================================================================
// [Fog::Object - Meta Class]
// ============================================================================

const MetaClass* Object::getStaticMetaClass()
{
  return _staticMetaClass;
}

const MetaClass* Object::_getStaticMetaClassRace(MetaClass** p)
{
  while (AtomicCore<size_t>::get((size_t*)p) == 0)
  {
    // Yield is not optimal, but this should really rarely happen and if we
    // had the luck then there is 100% probability that it will not happen
    // again.
    Thread::yield();
  }

  return *p;
}

const MetaClass* Object::getObjectMetaClass() const
{
  return _staticMetaClass;
}

// ============================================================================
// [Fog::Object - Object Id]
// ============================================================================

err_t Object::setId(const StringW& id)
{
  if (id == _objectExtra->_id)
    return ERR_OK;

  ObjectExtra* extra = getMutableExtra();
  if (FOG_IS_NULL(extra))
    return ERR_RT_OUT_OF_MEMORY;

  FOG_RETURN_ON_ERROR(extra->_id.set(id));
  return ERR_OK;
}

// ============================================================================
// [Fog::Object - Home Thread]
// ============================================================================

err_t Object::moveToThread(Thread* thread)
{
  if (_homeThread == thread)
    return ERR_OK;

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::Object - Object Hierarchy]
// ============================================================================

err_t Object::setParent(Object* parent)
{
  // If our parent is a given parent then do nothing, it's not error.
  if (_parent == parent)
    return ERR_OK;

  // If we have a parent then it's needed to break the hierarchy first.
  if (_parent != NULL)
    FOG_RETURN_ON_ERROR(_parent->removeChild(this));

  // Now set the new parent. NULL is valid value so we must make sure the
  // object is not null before calling its method.
  if (parent)
    FOG_RETURN_ON_ERROR(parent->addChild(this));

  return ERR_OK;
}

err_t Object::addChild(Object* child)
{
  if (FOG_IS_NULL(child))
    return ERR_RT_INVALID_ARGUMENT;

  // If object is already in our hierarchy then we return quickly.
  if (child->getParent() == this)
    return ERR_OK;

  // If object has different parent then it's an error.
  if (child->hasParent())
    return ERR_OBJECT_HAS_PARENT;

  // Okay, now we can call the virtual _addChild() method.
  return _addChild(_objectExtra->_children.getLength(), child);
}

err_t Object::removeChild(Object* child)
{
  if (FOG_IS_NULL(child))
    return ERR_RT_INVALID_ARGUMENT;

  if (child->getParent() != this)
    return ERR_OBJECT_NOT_FOUND;

  size_t index = _objectExtra->_children.indexOf(child);
  // It must be here if parent check passed!
  FOG_ASSERT(index != INVALID_INDEX);

  // Okay, now we can call the virtual _removeChild() method.
  return _removeChild(index, child);
}

err_t Object::_addChild(size_t index, Object* child)
{
  // Index must be valid or equal to children list length.
  FOG_ASSERT(index <= _objectExtra->_children.getLength());

  ObjectExtra* extra = getMutableExtra();
  if (FOG_IS_NULL(extra))
    return ERR_RT_OUT_OF_MEMORY;

  FOG_RETURN_ON_ERROR(extra->_children.insert(index, child));
  child->_parent = this;

  // Send 'child-add' event.
  ChildEvent e(EVENT_CHILD_ADD, child);
  sendEvent(&e);

  return ERR_OK;
}

err_t Object::_removeChild(size_t index, Object* child)
{
  ObjectExtra* extra = _objectExtra;

  // Index must be valid.
  FOG_ASSERT(index  < extra->_children.getLength() &&
             child == extra->_children.getAt(index));

  // Extra must be mutable if we found the children.
  FOG_ASSERT(extra != &Object_extraNull);

  FOG_RETURN_ON_ERROR(extra->_children.removeAt(index));
  child->_parent = NULL;

  // Send 'child-remove' event.
  ChildEvent e(EVENT_CHILD_REMOVE, child);
  sendEvent(&e);

  return ERR_OK;
}

err_t Object::_removeAll()
{
  ObjectExtra* extra = _objectExtra;
  if (extra->_children.isEmpty())
    return ERR_OK;

  err_t err = ERR_OK;
  size_t index;

  while ((index = extra->_children.getLength()) != 0)
  {
    Object* child = extra->_children.getAt(--index);
    err = _removeChild(index, child);

    if (FOG_IS_ERROR(err))
      break;
  }

  return err;
}

// ============================================================================
// [Fog::Object - Property - Validate]
// ============================================================================

// Object name can contain characters [A-Z], [a-z], '-', '_', [0-9]. Numbers
// are not allowed for the first character.
static err_t Object_validatePropertyName(const StringW& name)
{
  const CharW* data = name.getData();
  size_t length = name.getLength();

  if (length == 0)
    return ERR_PROPERTY_INVALID;

  CharW c = data[0];
  if (!(c.isAsciiLetter() || c == CharW('_') || c == CharW('-')))
    return ERR_PROPERTY_INVALID;

  for (size_t i = 1; i < length; i++)
  {
    if (!(c.isAsciiNumlet() || c == CharW('_') || c == CharW('-')))
      return ERR_PROPERTY_INVALID;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Object - Property - Dynamic]
// ============================================================================

static err_t Object_getDynamicProperty(const Object* self, const ManagedStringW& name, Var& dst)
{
  const ObjectExtra* extra = self->_objectExtra;
  if (extra == &Object_extraNull)
    return ERR_PROPERTY_NOT_FOUND;

  const Var* var = extra->_properties.getPtr(name);
  if (var == NULL)
    return ERR_PROPERTY_NOT_FOUND;
  else
    return dst.setVar(*var);
}

static err_t Object_setDynamicProperty(Object* self, const ManagedStringW& name, const Var& src)
{
  ObjectExtra* extra = self->getMutableExtra();
  if (FOG_IS_NULL(extra))
    return ERR_RT_OUT_OF_MEMORY;

  Var* var = extra->_properties.usePtr(name);
  if (var != NULL)
  {
    if (*var == src)
      return ERR_OK;

    FOG_RETURN_ON_ERROR(var->setVar(src));
  }
  else
  {
    FOG_RETURN_ON_ERROR(extra->_properties.put(name, src));
  }

  PropertyEvent e(name);
  self->sendEvent(&e);

  return ERR_OK;
}

// ============================================================================
// [Fog::Object - Property - Accessors]
// ============================================================================

err_t Object::getProperty(const StringW& name, Var& dst) const
{
  ManagedStringW m_name(name, MANAGED_STRING_OPTION_LOOKUP);

  if (m_name.isEmpty())
  {
    FOG_RETURN_ON_ERROR(Object_validatePropertyName(name));

    // If the property name is valid then we must ERR_PROPERTY_NOT_FOUND.
    return ERR_PROPERTY_NOT_FOUND;
  }
  else
  {
    err_t err = _getProperty(m_name, dst);

    // If the property doesn't exist try dynamically added properties.
    if (err == ERR_PROPERTY_NOT_FOUND)
      err = Object_getDynamicProperty(this, m_name, dst);
    return err;
  }
}

err_t Object::setProperty(const StringW& name, const Var& src)
{
  ManagedStringW m_name(name, MANAGED_STRING_OPTION_LOOKUP);

  if (m_name.isEmpty())
  {
    // Return error if the property name isn't valid. We are going to create
    // new ManagedStringW so we must be sure that the internal hash table used
    // to store managed strings won't be polluted by strings we actually don't
    // need.
    FOG_RETURN_ON_ERROR(Object_validatePropertyName(name));

    // Create a new ManagedStringW.
    FOG_RETURN_ON_ERROR(m_name.set(name));
  }
  else
  {
    // We know that there is a ManagedStringW, so try Object::_setProperty().
    err_t err = _setProperty(m_name, src);

    // If something bad happened (for example wrong argument type) then we
    // report it immediately.
    if (err != ERR_PROPERTY_NOT_FOUND)
      return err;

    // We are going to set dynamic property (this means that it can be created)
    // so we must be sure that the property name is valid.
    FOG_RETURN_ON_ERROR(Object_validatePropertyName(name));
  }

  // Set dynamic property.
  return Object_setDynamicProperty(this, m_name, src);
}

err_t Object::getProperty(const ManagedStringW& name, Var& dst) const
{
  err_t err = _getProperty(name, dst);
  if (err != ERR_PROPERTY_NOT_FOUND)
    return err;

  err = Object_getDynamicProperty(this, name, dst);
  if (err != ERR_PROPERTY_NOT_FOUND)
    return err;

  FOG_RETURN_ON_ERROR(Object_validatePropertyName(name));
  return ERR_PROPERTY_NOT_FOUND;
}

err_t Object::setProperty(const ManagedStringW& name, const Var& src)
{
  err_t err = _setProperty(name, src);
  if (err != ERR_PROPERTY_NOT_FOUND)
    return err;

  // If property doesn't exist create a dynamic property.
  FOG_RETURN_ON_ERROR(Object_validatePropertyName(name));
  return Object_setDynamicProperty(this, name, src);
}

// ============================================================================
// [Fog::Object - Property - Virtual]
// ============================================================================

err_t Object::_getProperty(const ManagedStringW& name, Var& dst) const
{
  FOG_UNUSED(name);
  FOG_UNUSED(dst);

  if (name == FOG_STR_(OBJECT_id))
    return dst.setString(_objectExtra->_id);

  return ERR_PROPERTY_NOT_FOUND;
}

err_t Object::_setProperty(const ManagedStringW& name, const Var& src)
{
  FOG_UNUSED(name);
  FOG_UNUSED(src);

  if (name == FOG_STR_(OBJECT_id))
  {
    StringW src_string;
    FOG_RETURN_ON_ERROR(src.getString(src_string));
    return setId(src_string);
  }

  return ERR_PROPERTY_NOT_FOUND;
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
  ObjectExtra* extra = _objectExtra;
  if (extra == &Object_extraNull)
    return 0;

  AutoLock locked(Object::_internalLock);
  uint result = 0;

  ObjectConnection* prev;
  ObjectConnection* conn;
  ObjectConnection* next;

_Begin:
  {
    HashIterator<uint32_t, ObjectConnection*> it(extra->_forwardConnection);

    while (it.isValid())
    {
      uint32_t key = it.getKey();

      prev = NULL;
      conn = it.getItem();

      do {
        next = conn->next;
        if (conn->listener == listener)
        {
          result++;

          if (listener)
          {
            size_t index = listener->_objectExtra->_backwardConnection.indexOf(conn);
            FOG_ASSERT(index != INVALID_INDEX);
            listener->_objectExtra->_backwardConnection.removeAt(index);
          }

          fog_delete(conn);

          if (prev)
            prev->next = next;
          else if (next)
          {
            extra->_forwardConnection.put(key, next);
            goto _Begin;
          }
          else
          {
            extra->_forwardConnection.remove(key);
            goto _Begin;
          }
        }

        prev = conn;
        conn = next;
      } while (conn);

      it.next();
    }
  }

  return result;
}

uint Object::removeAllListeners()
{
  ObjectExtra* extra = _objectExtra;
  if (extra == &Object_extraNull)
    return 0;

  AutoLock locked(Object::_internalLock);

  uint result = 0;

  ObjectConnection* conn;
  ObjectConnection* next;

  HashIterator<uint32_t, ObjectConnection*> it(extra->_forwardConnection);


  while (it.isValid())
  {
    conn = it.getItem();

    do {
      next = conn->next;
      result++;

      if (conn->listener)
      {
        size_t index = conn->listener->_objectExtra->_backwardConnection.indexOf(conn);
        FOG_ASSERT(index != INVALID_INDEX);
        conn->listener->_objectExtra->_backwardConnection.removeAt(index);
      }

      fog_delete(conn);
      conn = next;
    } while (conn);

    it.next();
  }

  extra->_forwardConnection.clear();
  return result;
}

// Private.
bool Object::_addListener(uint32_t code, Object* listener, const void* del, uint32_t type)
{
  AutoLock locked(Object::_internalLock);

  ObjectExtra* extra = getMutableExtra();
  if (FOG_IS_NULL(extra))
    return false;

  if (listener->getMutableExtra() == NULL)
    return false;

  ObjectConnection* prev = NULL;
  ObjectConnection* conn = extra->_forwardConnection.get(code, NULL);
  Delegate0<> d = *(const Delegate0<> *)del;

  if (conn)
  {
    do {
      if (conn->delegateVoid() == d)
        return false;
      prev = conn;
      conn = conn->next;
    } while (conn);
  }

  conn = fog_new ObjectConnection;
  // TODO: if (conn == NULL) ...
  conn->next = NULL;
  conn->attachedObject = this;
  conn->listener = listener;
  conn->delegateVoid() = d;
  conn->type = type;
  conn->code = code;

  if (prev)
    prev->next = conn;
  else
    extra->_forwardConnection.put(code, conn);

  if (listener)
    listener->_objectExtra->_backwardConnection.append(conn);

  return true;
}

bool Object::_removeListener(uint32_t code, Object* listener, const void* del, uint32_t type)
{
  ObjectExtra* extra = _objectExtra;
  if (FOG_IS_NULL(extra))
    return false;

  AutoLock locked(Object::_internalLock);

  ObjectConnection* prev = NULL;
  ObjectConnection* conn = extra->_forwardConnection.get(code, NULL);
  if (!conn) return false;
  Delegate0<> d = *(const Delegate0<> *)del;

  do {
    ObjectConnection* next = conn->next;

    if (conn->delegateVoid() == d)
    {
      FOG_ASSERT(conn->listener == listener);

      if (prev)
        prev->next = next;
      else if (next)
        extra->_forwardConnection.put(code, next);
      else
        extra->_forwardConnection.remove(code);

      if (listener)
      {
        size_t index = listener->_objectExtra->_backwardConnection.indexOf(conn);
        FOG_ASSERT(index != INVALID_INDEX);
        listener->_objectExtra->_backwardConnection.removeAt(index);
      }

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
  ObjectExtra* extra = _objectExtra;
  if (FOG_IS_NULL(extra))
    return;

  uint32_t code = e->getCode();

  ObjectConnection* conn = extra->_forwardConnection.get(code, NULL);
  if (!conn) return;

  do {
    switch (conn->type)
    {
      case OBJECT_EVENT_HANDLER_VOID:
        conn->delegateVoid()();
        break;
      case OBJECT_EVENT_HANDLER_EVENTPTR:
        conn->delegateEvent()(e);
        break;
      default:
        FOG_ASSERT_NOT_REACHED();
        break;
    }
  } while ((conn = conn->next));
}

// ============================================================================
// [Fog::Object - Event Management]
// ============================================================================

void Object::postEvent(Event* e)
{
  Thread* thread = getHomeThread();

  if (FOG_IS_NULL(thread))
    goto _Fail;

  if (!thread->getEventLoop().isCreated())
    goto _Fail;

  // Link event with object`s event queue.
  {
    AutoLock locked(_internalLock);

    e->_prev = this->_events;
    this->_events = e;

    e->_flags |= Event::IS_POSTED;
  }

  // Post event (event is posted as task).
  thread->getEventLoop().postTask(e);
  return;

_Fail:
  Debug::dbgFunc("Fog::Object", "postEvent",
    "Can't post event to object which has no home thread or event loop. Event destroyed!\n");
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
// [Fog::Object - Event Handlers]
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

void Object::onCreate(CreateEvent* e) {}
void Object::onDestroy(DestroyEvent* e) {}
void Object::onChild(ChildEvent* e) {}
void Object::onProperty(PropertyEvent* e) {}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Object_init(void)
{
  // Initialize the Object meta class.
  static MetaClass _privateObjectMetaClass;
  static const char _privateObjectClassName[] = "Fog::Object";

  _privateObjectMetaClass.base = NULL;
  _privateObjectMetaClass.name = _privateObjectClassName;

  _privateObjectMetaClass.hashCode = HashUtil::hash(StubA(_privateObjectClassName, FOG_ARRAY_SIZE(_privateObjectClassName) - 1));

  Object::_staticMetaClass = &_privateObjectMetaClass;

  // Initialize the locks.
  Object::_internalLock.init();
  Object_memPoolLock.init();

  // Initialize the memory pools.
  Object_memPoolExtra.init();
  Object_memPoolConn.init();

  // Initialize the ObjectExtra null (initial) instance.
  Object_extraNull.init();
}

FOG_NO_EXPORT void Object_fini(void)
{
  // Destroy the shared ObjectExtra instance.
  Object_extraNull.destroy();

  // Destroy the memory pools.
  Object_memPoolConn.destroy();
  Object_memPoolExtra.destroy();

  // Destroy the locks.
  Object_memPoolLock.destroy();
  Object::_internalLock.destroy();
}

} // Fog namespace

// ============================================================================
// [Fog::Object - C-API - fog_object_cast<>]
// ============================================================================

FOG_CAPI_DECLARE void* fog_object_cast_meta(Fog::Object* self, const Fog::MetaClass* metaClass)
{
  using namespace Fog;

  if (FOG_IS_NULL(self))
    return NULL;

  const MetaClass* selfClass = self->getObjectMetaClass();

  // Iterate over meta classes and try to find the target one.
  do {
    // Compare meta classes for match. Each class has only one meta class,
    // so pointers comparison is the best way here.
    if (selfClass == metaClass)
      return (void*)self;

    // Not match? Try base meta class, again and again until there is no
    // base class (Fog::Object is root).
    selfClass = selfClass->base;
  } while (selfClass != NULL);

  // No match.
  return NULL;
}

FOG_CAPI_DECLARE void* fog_object_cast_string(Fog::Object* self, const char* className)
{
  using namespace Fog;

  if (FOG_IS_NULL(self))
    return NULL;

  const MetaClass* metaClass = self->getObjectMetaClass();
  uint32_t classHash = HashUtil::hash(StubA(className, DETECT_LENGTH));

  for (;;)
  {
    // Compare hashes and string class names.
    if (metaClass->hashCode == classHash && strcmp((const char*)metaClass->name, (const char*)className) == 0)
    {
      return (void*)self;
    }

    // Iterate over base classes and return if there is no one.
    if ((metaClass = metaClass->base) == NULL)
    {
      return NULL;
    }
  }
}
