// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_OBJECT_H
#define _FOG_CORE_KERNEL_OBJECT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Delegate.h>
#include <Fog/Core/Kernel/Event.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/HashUtil.h>
#include <Fog/Core/Tools/List.h>

// ============================================================================
// [Forward Declarations]
// ============================================================================

namespace Fog {

// TODO: These should be defined by Global/TypeDefs.h, so remove from here.
// Fog-Core.
struct Event;
struct ManagedString;
struct MetaClass;

// Fog-Gui (object_cast specialization).
struct Layout;
struct Widget;

} // Fog namespace

// ============================================================================
// [Fog::Object - CAPI]
// ============================================================================

//! @addtogroup Fog_Core_System
//! @{

FOG_CAPI_EXTERN void* fog_object_cast_helper(Fog::Object* self, const Fog::MetaClass* targetMetaClass);
FOG_CAPI_EXTERN void* fog_object_cast_string(Fog::Object* self, const char* className);

//! @}

// ============================================================================
// [Fog::Object - Macros]
// ============================================================================

//! @addtogroup Fog_Core_System
//! @{

//!
//! @brief This macro should be used for each class declaration that inherits
//! from @c Fog::Object class to use object dynamic features.
//!
//! This macro creates needed virtual methods and it's used together with
//! @c FOG_IMPLEMENT_OBJECT, see example:
//!
//! @code
//! // header file
//!
//! // some namespace
//! namespace Namespace
//! {
//!   struct MyObject : public Fog::Object
//!   {
//!     // declare that object is MyObject and is derived from Fog::Object
//!     FOG_DECLARE_OBJECT(MyObject, Fog::Object)
//!
//!     MyObject();
//!     ~MyObject();
//!
//!     // ... object members and methods
//!   };
//! }
//!
//! // initialize object info, always use full name with namespace
//! // or meta class information will be invalid!
//! FOG_IMPLEMENT_OBJECT(Namespace::MyObject)
//!
//! // source file
//! namespace Namespace
//! {
//!   MyObject::MyObject()
//!   {
//!   }
//!
//!   MyObject::~MyObject()
//!   {
//!   }
//! }
//! @endcode
//!
//! @c MyObject from example inherits @ref Object and now it's able to use
//! Fog-Framework features like dynamic object identification, event subsystem
//! and properties.
#define FOG_DECLARE_OBJECT(_SelfType_, _BaseType_) \
public: \
  /* ----------------------------------------------------------------------- */ \
  /* [Typedefs]                                                              */ \
  /* ----------------------------------------------------------------------- */ \
  \
  /*! @brief Self type of class. */ \
  typedef _SelfType_ _Self; \
  /*! @brief Base type of class. */ \
  typedef _BaseType_ _Base; \
  /*! @brief Base type of class. */ \
  typedef _Base base; \
  \
  /* ----------------------------------------------------------------------- */ \
  /* [Meta Class]                                                            */ \
  /* ----------------------------------------------------------------------- */ \
  \
  /*! @brief Static meta class instance, internal. */ \
  static const ::Fog::MetaClass* _staticMetaClass; \
  \
  /*! @brief Static method that's used for retrieving meta class information. */ \
  static const ::Fog::MetaClass* getStaticMetaClass(); \
  \
  /*! @brief Virtual method that's used for retrieving meta class information. */ \
  virtual const ::Fog::MetaClass* getObjectMetaClass() const;

#define FOG_IMPLEMENT_OBJECT(_SelfType_) \
  const Fog::MetaClass* _SelfType_::_staticMetaClass; \
  \
  const Fog::MetaClass* _SelfType_::getStaticMetaClass() \
  { \
    /* Static variables. */ \
    static Fog::MetaClass _privateMetaClass; \
    static const char _privateClassName[] = #_SelfType_; \
    \
    if (_staticMetaClass != NULL) \
    { \
      /* Already initialized, expected behavior. */ \
      return _staticMetaClass; \
    } \
    \
    /* Object meta class is not initialized, so we must to initialize now. */ \
    /* To prevent performance degradation we use lock-free initialization. */ \
    \
    /* _privateMetaClass is statically allocated during link time, so all  */ \
    /* members should be set to zeros. We use _privateMetaClass._name as a */ \
    /* state holder. If it's set to zero the it's uninitialized, if it's   */ \
    /* set to one then it's being initialized by another thread and we must*/ \
    /* wait. All other values means that it's initialized.                 */ \
    \
    if (Fog::AtomicCore<size_t>::cmpXchg(reinterpret_cast<size_t*>(&_privateMetaClass.name), 0, 1)) \
    { \
      /* Called the first time, we must initialize the meta class right now. */ \
      _privateMetaClass.base = base::getStaticMetaClass(); \
      _privateMetaClass.hashCode = Fog::HashUtil::hash(StubA(_privateClassName, FOG_ARRAY_SIZE(_privateClassName) - 1)); \
      _privateMetaClass.name = _privateClassName; \
      \
      /* Assign meta class to _staticMetaClass pointer. */ \
      Fog::AtomicCore<size_t>::setXchg((size_t*)(_staticMetaClass), (size_t)(&_privateMetaClass)); \
      \
      /* Everything done. */ \
      return _staticMetaClass; \
    } \
    else \
    { \
      /* Race - Another thread is initializing the meta class, we must wait. */ \
      return _getStaticMetaClassRace(const_cast<Fog::MetaClass**>(&_staticMetaClass)); \
    } \
  } \
  \
  const Fog::MetaClass* _SelfType_::getObjectMetaClass() const \
  { \
    if (_staticMetaClass != NULL) \
    { \
      /* Already initialized, expected behavior. */ \
      return _staticMetaClass; \
    } \
    else \
    { \
      /* Called the first time, need to initialize. */ \
      return getStaticMetaClass(); \
    } \
  }

//! @brief Declare event handler for @c Fog::Object based class.
//!
//! This declaration overrides class virtual event handler. Virtual
//! event handler is fastest way to use event cathing and it's preferred
//! way of communication between @c Fog::Object based objects.
//!
//! This macro creates a fast compiled static table that can't be
//! overriden by addListener() or removeListener() methods.
#define FOG_EVENT_BEGIN() \
  virtual FOG_NO_INLINE void onEvent(::Fog::Event* e) \
  { \
    switch (e->_code) \
    {

//! @brief Add event into event handled declared by
//! @c FOG_EVENT_BEGIN() macro.
//!
//! @param __event_code__ Event code, see event codes.
//! @param __event_class__ Event class (@c Fog::Event or derived)
//! @param __event_handler__ Event handler method (expression without parenthesis).
//! @param __event_type__ Event type, use @c override or @c cascade.
//!
//! Event type should be @c Override, because event handlers are usually
//! virtual methods and these methods are more elegant than declaring
//! them by other way.
#define FOG_EVENT_DEF(_EventCode__, _EventHandler_, _EventClass_, _EventBehavior_) \
      case _EventCode__: \
      { \
        if (::Fog::OBJECT_EVENT_HANDLER_##_EventBehavior_ == ::Fog::OBJECT_EVENT_HANDLER_REVERSE) \
          base::onEvent(e); \
        \
        _EventHandler_(reinterpret_cast<_EventClass_*>(e)); \
        \
        if (::Fog::OBJECT_EVENT_HANDLER_##_EventBehavior_ == ::Fog::OBJECT_EVENT_HANDLER_CASCADE) \
          break; \
        else \
          return; \
      }

//! @brief End of @c FOG_EVENT_BEGIN() macro.
#define FOG_EVENT_END() \
      default: \
        /* event not found here or declared event behavior is to cascade */ \
        /* into an event handler. */ \
        break; \
    } \
    base::onEvent(e); \
  }

// ============================================================================
// [fog_object_cast<>]
// ============================================================================

//! @brief Cast @ref Object* to @c T type.
template <typename T>
FOG_INLINE T fog_object_cast(Fog::Object* object)
{
  return static_cast<T>((Fog::Object*)
    fog_object_cast_helper(object, ((T)NULL)->getStaticMetaClass()) );
}

//! @brief Cast const @ref Object* to @c T type.
template <typename T>
FOG_INLINE T fog_object_cast(const Fog::Object* object)
{
  return static_cast<T>((const Fog::Object*)
    fog_object_cast_helper((Fog::Object*)object, ((T)NULL)->getStaticMetaClass()) );
}

//! @}

namespace Fog {

// ============================================================================
// [Fog::ObjectConnection]
// ============================================================================

//! @addtogroup Fog_Core_Object
//! @{

//! @internal
//!
//! @brief Object connection is used to connect two objects (event dispatcher
//! and receiver).
struct FOG_NO_EXPORT ObjectConnection
{
  ObjectConnection* next;
  Object* attachedObject;
  Object* listener;

  union
  {
    Static< Delegate0<> > delegateVoid;
    Static< Delegate1<Event*> > delegateEvent;
  };

  uint32_t type;
  uint32_t code;
};

// ============================================================================
// [Fog::MetaClass]
// ============================================================================

//! @brief Object record.
struct FOG_NO_EXPORT MetaClass
{
  //! @brief Base meta class.
  //!
  //! Note that @c Fog::Object has this value set to NULL, but all other classes
  //! not.
  const MetaClass* base;

  //! @brief Object class name.
  const char* name;

  //! @brief Object class name hash.
  uint32_t hashCode;
};

// ============================================================================
// [Fog::Object]
// ============================================================================

// TODO: This needs cleanup, moving bad code out, creating proper object
// connection system, threads management, etc...

//! @brief Basic object type used for complex objects in Fog.
//!
//! Fog::Object supports:
//! - Object naming and IDs - see @c getId(), @c setId(), @c getName(), @c setName(), ...
//! - Own RTTI (Run Time Type Information), see @c getClassName(), @c isClassOf(),
//!   also see macros @c FOG_DECLARE_OBJECT() and @c FOG_IMPLEMENT_OBJECT().
//! - Meta class concept (information about class stored in @c MetaClass object).
//! - Event subsystem support, see @c addListener(), @c removeListener(),
//!   @c sendEvent(), @c postEvent(), ...
//! - Properties support, see @c setProperty(), @c getProperty()
struct FOG_API Object
{
  // --------------------------------------------------------------------------
  // [Fog Object System]
  // --------------------------------------------------------------------------

  // Don't use FOG_DECLARE_OBJECT macro here, this base class is special (it's
  // base class so we must do some things by hand).

  // --------------------------------------------------------------------------
  // [Construction & Destruction]
  // --------------------------------------------------------------------------

  Object();
  virtual ~Object();

  //! @brief Destroys object if it's allocated on the heap (calls delete)
  //!
  //! Destroy method is called from @c deref() or from @c Fog::Wrap
  //! template. This is only correct way to delete object from memory,
  //! because destroying process needs to send Fog::DestroyEvent thats
  //! only possible before object destructor is called.
  //!
  //! @note There is no way how to take this back.
  void destroy();

  //! @brief Post destroy later task.
  //!
  //! Use this method to destroy the object instance from the heap later.
  //! Common usage is to destroy instance where you are handling the event
  //! right now (because you can't delete object that is being processed).
  void destroyLater();

  //! @brief Get whether the destroyLater() method was called.
  FOG_INLINE bool isDestroyLaterActive() const
  {
    return (_vType & OBJECT_FLAG_DESTROY_LATER) != 0;
  }

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @brief Get object flags.
  FOG_INLINE uint32_t getObjectFlags() const { return _vType; }

  //! @brief Get whether the object can be deleted using delete operator.
  FOG_INLINE bool isStatic() const { return (_vType & VAR_FLAG_STATIC) != 0; }

  //! @brief Get whether the object is @c Fog::Widget.
  FOG_INLINE bool isWidget() const { return (_vType & OBJECT_FLAG_IS_WIDGET) != 0; }

  //! @brief Get whether the object is @c Fog::Layout.
  FOG_INLINE bool isLayout() const { return (_vType & OBJECT_FLAG_IS_LAYOUT) != 0; }

  // --------------------------------------------------------------------------
  // [Object Id / Name]
  // --------------------------------------------------------------------------

  //! @brief Get object id.
  FOG_INLINE uint32_t getObjectId() const  { return _objectId; }
  //! @brief Get object name.
  FOG_INLINE StringW getObjectName() const { return _objectName; }

  //! @brief Set object id.
  void setObjectId(uint32_t objectId);
  //! @brief Set object name.
  void setObjectName(const StringW& objectName);

  // --------------------------------------------------------------------------
  // [Object Hierarchy]
  // --------------------------------------------------------------------------

  //! @brief Get whether the object has parent.
  FOG_INLINE bool hasParent() const { return _parent != NULL; }

  //! @brief Get whether the object has children.
  FOG_INLINE bool hasChildren() const { return !_children.isEmpty(); }

  //! @brief Get object parent.
  FOG_INLINE Object* getParent() const { return _parent; }

  //! @brief Get object children.
  FOG_INLINE const List<Object*>& getChildren() const { return _children; }

  //! @brief Set parent of this object to @a parent.
  //!
  //! @note This method can be used to reparent object. If a given @a object
  //! has parent then it will be first removed using @c remove() method and
  //! then added using @c add() method.
  err_t setParent(Object* parent);

  //! @brief Add the @a child into the object hierarchy.
  err_t addChild(Object* child);

  //! @brief Remove the @a child from the widget hierarchy.
  err_t removeChild(Object* child);

  //! @brief Add a @a child to a specified @a index.
  //!
  //! @param index The valid index where to insert a @a child.
  //! @param child The valid @ref Object instance.
  //!
  //! This method can be overriden to override object hierarchy management. It
  //! can be called throught @c setParent(), @c addChild() or @c removeChild()
  //! methods.
  virtual err_t _addChild(size_t index, Object* child);

  //! @brief Remove a @a child from the specified @a index.
  //!
  //! @param index The valid index where the @a child is.
  //! @param child The valid @ref Object instance.
  //!
  //! This method can be overriden to override object hierarchy management. It
  //! can be called throught @c setParent(), @c addChild() or @c removeChild()
  //! methods.
  virtual err_t _removeChild(size_t index, Object* child);

  //! @brief Delete all children.
  //!
  //! Called by @ref Object or @ref Widget destructors. Children are deleted
  //! from last to first (the backward direction as they was added into).
  //!
  //! @note All Objects inside that has flag @ref VAR_FLAG_STATIC are removed
  //! from the hierarchy using the @c _removeChild() method, but not deleted.
  err_t _deleteChildren();

  // --------------------------------------------------------------------------
  // [Threading]
  // --------------------------------------------------------------------------

  //! @brief Get home thread.
  //!
  //! Home thread is thread where the instance was created. This thread not
  //! owns the @ref Object instance, but is used by Fog-event subsystem to
  //! deliver events into the correct thread.
  FOG_INLINE Thread* getHomeThread() const { return _homeThread; }

  // --------------------------------------------------------------------------
  // [RTTI]
  // --------------------------------------------------------------------------

  //! @brief Static method that's used for retrieving dynamic class info.
  static const MetaClass* getStaticMetaClass();

  //! @internal
  //!
  //! @brief Called internally by @c getStaticMetaClass() if another thread is
  //! trying to initialize it.
  //!
  //! The reason why this method was introduced is code-bloat - we don't need
  //! it inlined in each @c getStaticMetaClass() method. Never call directly.
  static const MetaClass* _getStaticMetaClassRace(MetaClass** p);

  //! @brief Virtual method that's used for retrieving class info.
  virtual const MetaClass* getObjectMetaClass() const;

  //! @brief Get object class name.
  FOG_INLINE const char* getClassName() const
  { return getObjectMetaClass()->name; }

  //! @brief Get whether the object can be casted to @c T.
  template<typename T>
  FOG_INLINE bool isClassOf() const
  { return fog_object_cast<T>((Object*)this) != NULL; }

  //! @brief Get whether the object can be casted to @a className.
  FOG_INLINE bool isClassOf(const char* className) const
  { return fog_object_cast_string((Object*)this, className) != NULL; }

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  //! @brief Get property @a name to a given @a value.
  err_t getProperty(const StringW& name, Var& dst) const;
  //! @brief Set property @a name to a given @a value.
  err_t setProperty(const StringW& name, const Var& src);

  //! @brief Get property @a name to a given @a value.
  //!
  //! This is virtual version that can be overrided to implement own properties.
  virtual err_t getProperty(const ManagedString& name, Var& dst) const;
  //! @brief Set property @a name to a given @a value.
  //!
  //! This is virtual version that can be overrided to implement own properties.
  virtual err_t setProperty(const ManagedString& name, const Var& src);

  // --------------------------------------------------------------------------
  // [Event Handling]
  // --------------------------------------------------------------------------

  //! @brief Add a pure C function listener @a fn.
  bool addListener(uint32_t code, void (*fn)(Event*));
  //! @overload
  bool addListener(uint32_t code, void (*fn)());

  //! @brief Remove a pure C function listener @a fn.
  bool removeListener(uint32_t code, void (*fn)(Event*));
  //! @overload
  bool removeListener(uint32_t code, void (*fn)());

  //! @overload
  template<class X, class Y>
  FOG_INLINE bool addListener(uint32_t code, Y* target, void (X::*fn)())
  {
    Delegate0<> del(target, fn);
    return _addListener(code, target, (const void*)&del, (uint32_t)OBJECT_EVENT_HANDLER_VOID);
  }

  //! @brief Adds @c a Fog::Object* based function listener @a fn.
  template<class X, class Y, class Z>
  FOG_INLINE bool addListener(uint32_t code, Y* target, void (X::*fn)(Z*))
  {
    Delegate1<Z*> del(target, fn);
    return _addListener(code, target, (const void*)&del, (uint32_t)OBJECT_EVENT_HANDLER_EVENTPTR);
  }

  //! @brief Removes a @c Fog::Object* based function listener @a fn.
  template<class X, class Y>
  FOG_INLINE bool removeListener(uint32_t code, Y* target, void (X::*fn)())
  {
    Delegate0<> del(target, fn);
    return _removeListener(code, target, (const void*)&del, (uint32_t)OBJECT_EVENT_HANDLER_VOID);
  }

  //! @brief Removes a @c Fog::Object* based function listener @a fn.
  template<class X, class Y, class Z>
  FOG_INLINE bool removeListener(uint32_t code, Y* target, void (X::*fn)(Z*))
  {
    Delegate1<Z*> del(target, fn);
    return _removeListener(code, target, (const void*)&del, (uint32_t)OBJECT_EVENT_HANDLER_EVENTPTR);
  }

  uint removeListener(Object* receiver);
  uint removeAllListeners();

  //! @brief Post event @a e that will be processed by object's thread event loop.
  //!
  //! Posted event will be sent by main loop by @c sendEvent() method.
  void postEvent(Event* e);
  //! @brief Post simple event (only event id).
  void postEvent(uint32_t eventId);

  //! @brief Send event @a e immediately.
  void sendEvent(Event* e);
  //! @brief Send simple event (only event id) immediately.
  void sendEventByCode(uint32_t eventId);

  //! @brief Generic event handler.
  //!
  //! All events should be processed through this event handler.
  virtual void onEvent(Event* e);

  //! @brief Create object event.
  virtual void onCreate(CreateEvent* e);
  //! @brief Destroy object event.
  virtual void onDestroy(DestroyEvent* e);
  //! @brief Child added or removed.
  virtual void onChild(ChildEvent* e);
  //! @brief Property changed event.
  virtual void onProperty(PropertyEvent* e);

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static const MetaClass* _staticMetaClass;
  static Static<Lock> _internalLock;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Variable type and flags.
  uint32_t _vType;

  //! @brief Object id.
  //!
  //! Object id is not unique and you can't get global object by it's
  //! id. But it can help you identifying your objects by very fast
  //! way and ability to use C/C++ switch() statement.
  //!
  //! @sa _objectName.
  uint32_t _objectId;

  //! @brief Object name.
  //!
  //! @sa _objectId.
  StringW _objectName;

  //! @brief Object parent.
  Object* _parent;

  //! @brief Object children.
  List<Object*> _children;

  //! @brief The forward connection between us and other objects.
  //!
  //! Contains event id and object connection pair for each object that is
  //! listening us.
  //!
  //! @note Access to this structure must be always locked by @c Object::_internalLock.
  Hash<uint32_t, ObjectConnection*> _forwardConnection;

  //! @brief The backward connection between us and other objects.
  //!
  //! Contains list of object connections, but in backward direction - list
  //! of connections we listen to.
  //!
  //! If object is listening itself then @c _objectConnection and
  //! @c _objectBackReference can contain the same connection.
  //!
  //! @note Access to this structure must be always locked by @c Object::_internalLock.
  List<ObjectConnection*> _backwardConnection;

  //! @brief Object home thread.
  //!
  //! NULL if object is created on anonymous thread. In this case an event
  //! system is unusable for the @ref Object instance and event invocation
  //! means to log warning message and returning an error condition.
  Thread* _homeThread;

  //! @brief Link to last pending event or @c NULL if there are no pending events.
  //!
  //! @note Access to this structure must be always locked by @c Object::_internalLock.
  Event* _events;

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

private:
  bool _addListener(uint32_t code, Object* receiver, const void* del, uint32_t type);
  bool _removeListener(uint32_t code, Object* receiver, const void* del, uint32_t type);
  void _callListeners(Event* e);

  friend struct Event;

  _FOG_NO_COPY(Object)
};

//! @}

} // Fog namespace

// ============================================================================
// [fog_object_cast<Fog::Widget*> Specialization]
// ============================================================================

template<>
FOG_INLINE Fog::Widget* fog_object_cast(Fog::Object* object)
{
  FOG_ASSERT(object != NULL);
  return (Fog::Widget*)(object->isWidget() ? object : NULL);
}

template<>
FOG_INLINE const Fog::Widget* fog_object_cast(const Fog::Object* object)
{
  FOG_ASSERT(object != NULL);
  return (Fog::Widget*)(object->isWidget() ? object : NULL);
}

// ============================================================================
// [fog_object_cast<Fog::Layout*> Specialization]
// ============================================================================

template<>
FOG_INLINE Fog::Layout* fog_object_cast(Fog::Object* object)
{
  FOG_ASSERT(object != NULL);
  return (Fog::Layout*)(object->isLayout() ? object : NULL);
}

template<>
FOG_INLINE const Fog::Layout* fog_object_cast(const Fog::Object* object)
{
  FOG_ASSERT(object != NULL);
  return (Fog::Layout*)(object->isLayout() ? object : NULL);
}

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPE_DECLARE(Fog::ObjectConnection, Fog::TYPE_CATEGORY_SIMPLE)

// [Guard]
#endif // _FOG_CORE_KERNEL_OBJECT_H
