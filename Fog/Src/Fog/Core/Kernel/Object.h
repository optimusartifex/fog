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
// [Fog::Object - CAPI]
// ============================================================================

//! @addtogroup Fog_Core_Kernel
//! @{

FOG_CAPI_EXTERN void* fog_object_cast_meta(Fog::Object* self, const Fog::MetaClass* metaClass);
FOG_CAPI_EXTERN void* fog_object_cast_string(Fog::Object* self, const char* className);

//! @brief Cast @ref Object* to @c TypeT type.
template <typename TypeT>
FOG_INLINE TypeT fog_object_cast(Fog::Object* object)
{
  return static_cast<TypeT>((Fog::Object*)
    fog_object_cast_meta(object, ((TypeT)NULL)->getStaticMetaClass()) );
}

//! @brief Cast const @ref Object* to @c TypeT type.
template <typename TypeT>
FOG_INLINE TypeT fog_object_cast(const Fog::Object* object)
{
  return static_cast<TypeT>((const Fog::Object*)
    fog_object_cast_meta((Fog::Object*)object, ((TypeT)NULL)->getStaticMetaClass()) );
}

//! @}

// ============================================================================
// [Fog::Object - Macros]
// ============================================================================

//! @addtogroup Fog_Core_Kernel
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
  typedef _SelfType_ Self; \
  /*! @brief Base type of class. */ \
  typedef _BaseType_ Base; \
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
      _privateMetaClass.base = Base::getStaticMetaClass(); \
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
    if (FOG_IS_NULL(_staticMetaClass)) \
      /* Called the first time, need to initialize. */ \
      return getStaticMetaClass(); \
    else \
      /* Already initialized, expected behavior. */ \
      return _staticMetaClass; \
  }

//! @brief Declare event handler for @c Fog::Object based class.
//!
//! This declaration overrides class virtual event handler. Virtual
//! event handler is fastest way to use event cathing and it's preferred
//! way of communication between @c Fog::Object based objects.
//!
//! This macro creates a fast compiled static table that can't be
//! overridden by addListener() or removeListener() methods.
#define FOG_EVENT_BEGIN() \
  virtual FOG_NO_INLINE void onEvent(::Fog::Event* e) \
  { \
    switch (e->_code) \
    {

//! @brief Add event into event handled declared by
//! @c FOG_EVENT_BEGIN() macro.
//!
//! @param _EventCode_ Event code, see event codes.
//! @param _EventHandler_ Event handler method (expression without parenthesis).
//! @param _EventClass_ Event class (@c Fog::Event or derived)
//! @param _EventBehavior_ Event type, use @c override or @c cascade.
//!
//! Event type should be @c Override, because event handlers are usually
//! virtual methods and these methods are more elegant than declaring
//! them by other way.
#define FOG_EVENT_DEF(_EventCode_, _EventHandler_, _EventClass_, _EventBehavior_) \
      case _EventCode_: \
      { \
        if (::Fog::OBJECT_EVENT_HANDLER_##_EventBehavior_ == ::Fog::OBJECT_EVENT_HANDLER_REVERSE) \
          Base::onEvent(e); \
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
    Base::onEvent(e); \
  }

//! @}

namespace Fog {

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
// [Fog::ObjectExtra]
// ============================================================================

//! @brief Object extra members - id, children, dynamic properties and object
//! connection.
struct FOG_NO_EXPORT ObjectExtra
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ObjectExtra() {}
  FOG_INLINE ~ObjectExtra() {}

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Object id.
  StringW _id;

  //! @brief Object children.
  List<Object*> _children;

  //! @brief Dynamic properties.
  Hash<StringW, Var> _properties;

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

private:
  FOG_NO_COPY(ObjectExtra)
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

  Object(uint32_t createFlags = OBJECT_CREATE_DEFAULT);
  virtual ~Object();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  // TODO: Return ERR.

  //! @brief Release the object.
  //!
  //! @note There is no way how to take this back.
  void release();

  // TODO: Return ERR.

  //! @brief Schedule release action (posting ReleaseEvent to the event loop).
  //!
  //! Use this method to destroy the object instance from the heap later.
  //! Common usage is to destroy instance where you are handling the event
  //! right now (because you can't delete object that is being processed).
  void scheduleRelease();

  //! @brief Get whether the releaseLater() method was called.
  FOG_INLINE bool isReleaseScheduled() const
  {
    return (_objectFlags & OBJECT_FLAG_RELEASE_SCHEDULED) != 0;
  }

  // --------------------------------------------------------------------------
  // [Var]
  // --------------------------------------------------------------------------

  //! @brief Get whether the object can be deleted using delete operator.
  FOG_INLINE bool isStatic() const { return (_vType & VAR_FLAG_STATIC) != 0; }

  // --------------------------------------------------------------------------
  // [Object Extra]
  // --------------------------------------------------------------------------

  //! @brief Get @c ObjectExtra instance ready for modification.
  //!
  //! @c ObjectExtra was designed to decrease the size needed per @c Object
  //! instance. If the object is only primitive and the most of its features
  //! are not used then it's likely that the @c ObjectExtra won't be created
  //! during the lifetime of the object.
  ObjectExtra* getMutableExtra();

  // --------------------------------------------------------------------------
  // [Object Flags]
  // --------------------------------------------------------------------------

  //! @brief Get object flags.
  FOG_INLINE uint32_t getObjectFlags() const { return _objectFlags; }

  // --------------------------------------------------------------------------
  // [Object Id]
  // --------------------------------------------------------------------------

  //! @brief Get object id (uniquity is not guaranteed).
  FOG_INLINE StringW getId() const { return _objectExtra->_id; }

  //! @brief Set object id.
  err_t setId(const StringW& id);

  // --------------------------------------------------------------------------
  // [Home Thread]
  // --------------------------------------------------------------------------

  //! @brief Get home thread.
  //!
  //! Home thread is thread where the instance was created. This thread not
  //! owns the @ref Object instance, but is used by Fog-event subsystem to
  //! deliver events into the correct thread.
  FOG_INLINE Thread* getHomeThread() const { return _homeThread; }

  //! @brief Move object and all children to a different thread @a thread.
  //!
  //! If @a thread is the current thread where the object lives then this is
  //! a NOP.
  err_t moveToThread(Thread* thread);

  // --------------------------------------------------------------------------
  // [Object Hierarchy]
  // --------------------------------------------------------------------------

  //! @brief Get whether the object has parent.
  FOG_INLINE bool hasParent() const { return _parent != NULL; }

  //! @brief Get object parent or NULL if object hasn't parent.
  FOG_INLINE Object* getParent() const { return _parent; }

  //! @brief Set parent of this object to @a parent.
  //!
  //! @note This method can be used to reparent object. If a given @a object
  //! has parent then it will be first removed using @c removeChild() method and
  //! then added using @c addChild() method.
  err_t setParent(Object* parent);

  //! @brief Get whether the object has children.
  FOG_INLINE bool hasChildren() const { return !_objectExtra->_children.isEmpty(); }

  //! @brief Get object children.
  FOG_INLINE const List<Object*>& getChildren() const { return _objectExtra->_children; }

  //! @brief Add the @a child into the object hierarchy.
  err_t addChild(Object* child);

  //! @brief Remove the @a child from the widget hierarchy.
  err_t removeChild(Object* child);

  //! @brief Add a @a child to a specified @a index.
  //!
  //! @param index The valid index where to insert a @a child.
  //! @param child The valid @ref Object instance.
  //!
  //! This method can be overridden to override object hierarchy management. It
  //! can be called through @c setParent(), @c addChild() or @c removeChild()
  //! methods.
  virtual err_t _addChild(size_t index, Object* child);

  //! @brief Remove a @a child from the specified @a index.
  //!
  //! @param index The valid index where the @a child is.
  //! @param child The valid @ref Object instance.
  //!
  //! This method can be overridden to override object hierarchy management. It
  //! can be called through @c setParent(), @c addChild() or @c removeChild()
  //! methods.
  virtual err_t _removeChild(size_t index, Object* child);

  //! @brief Remove all children.
  //!
  //! Called by @ref Object or @ref Widget destructor. Children are removed
  //! from last to first (the backward direction as they was added).
  //!
  //! @note All Objects inside that has flag @ref VAR_FLAG_STATIC are removed
  //! from the hierarchy using the @c _removeChild() method, but not deleted.
  err_t _removeAll();

  // --------------------------------------------------------------------------
  // [Meta-Class]
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
  {
    return getObjectMetaClass()->name;
  }

  //! @brief Get whether the object can be casted to @c TypeT.
  template<typename TypeT>
  FOG_INLINE bool isClassOf() const
  {
    return fog_object_cast<TypeT>((Object*)this) != NULL;
  }

  //! @brief Get whether the object can be casted to @a className.
  FOG_INLINE bool isClassOf(const char* className) const
  {
    return fog_object_cast_string((Object*)this, className) != NULL;
  }

  // --------------------------------------------------------------------------
  // [Properties - Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get property @a name to a given @a value.
  err_t getProperty(const StringW& name, Var& dst) const;
  //! @brief Set property @a name to a given @a value.
  err_t setProperty(const StringW& name, const Var& src);

  //! @brief Get property @a name to a given @a value.
  err_t getProperty(const InternedStringW& name, Var& dst) const;
  //! @brief Set property @a name to a given @a value.
  err_t setProperty(const InternedStringW& name, const Var& src);

  // --------------------------------------------------------------------------
  // [Properties - Virtual]
  // --------------------------------------------------------------------------

  //! @brief Get property @a name to a given @a value.
  //!
  //! This is virtual version that can be overridden to implement own properties.
  virtual err_t _getProperty(const InternedStringW& name, Var& dst) const;
  //! @brief Set property @a name to a given @a value.
  //!
  //! This is virtual version that can be overridden to implement own properties.
  virtual err_t _setProperty(const InternedStringW& name, const Var& src);

  // --------------------------------------------------------------------------
  // [Event Management]
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

  bool _addListener(uint32_t code, Object* receiver, const void* del, uint32_t type);
  bool _removeListener(uint32_t code, Object* receiver, const void* del, uint32_t type);
  void _callListeners(Event* e);

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

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

  //! @brief Object flags, see @ref OBJECT_FLAG.
  uint32_t _objectFlags;

  //! @brief Reference count.
  Atomic<size_t> _reference;

  //! @brief Object extended members.
  //!
  //! Extended members were designed to reduce a size of @c Object instance
  //! which doesn't use features like object-id / name, hierarchy, dynamic
  //! properties, and object event system.
  ObjectExtra* _objectExtra;

  //! @brief Object parent.
  Object* _parent;

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

private:
  FOG_NO_COPY(Object)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

// TODO: Move
_FOG_TYPE_DECLARE(Fog::ObjectConnection, Fog::TYPE_CATEGORY_SIMPLE)

// [Guard]
#endif // _FOG_CORE_KERNEL_OBJECT_H
