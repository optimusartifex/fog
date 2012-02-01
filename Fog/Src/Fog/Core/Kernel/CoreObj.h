// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_COREOBJ_H
#define _FOG_CORE_KERNEL_COREOBJ_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>
#include <Fog/Core/Kernel/Property.h>
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/Core/Tools/String.h>

// ============================================================================
// [Fog::PropertyBaseCall]
// ============================================================================

//! @internal
//!
//! @brief CoreObj base call implementation, used by property system.
template<typename BaseT>
struct _Fog_PropertyBaseCall
{
  static FOG_INLINE size_t getPropertyIndex(BaseT* self, const Fog::InternedStringW& name)
  {
    return self->BaseT::_getPropertyIndex(name);
  }

  static FOG_INLINE size_t getPropertyIndex(BaseT* self, const Fog::CharW* name, size_t length)
  {
    return self->BaseT::_getPropertyIndex(name, length);
  }

  static FOG_INLINE err_t getPropertyInfo(BaseT* self, size_t index, Fog::PropertyInfo& info)
  {
    return self->BaseT::_getPropertyInfo(index, info);
  }

  static FOG_INLINE err_t getProperty(BaseT* self, size_t index, Fog::StringW& value)
  {
    return self->BaseT::_getProperty(index, value);
  }

  static FOG_INLINE err_t setProperty(BaseT* self, size_t index, const Fog::StringW& value)
  {
    return self->BaseT::_setProperty(index, value);
  }

  static FOG_INLINE err_t resetProperty(BaseT* self, size_t index)
  {
    return self->BaseT::_resetProperty(index);
  }
};

//! @internal
//!
//! @brief Specialization for a class directly inheriting from @ref CoreObj.
//!
//! In such case we want to return default values instead of calling base 
//! class virtual methods.
template<>
struct _Fog_PropertyBaseCall<Fog::CoreObj>
{
  static FOG_INLINE size_t getPropertyIndex(Fog::CoreObj* self, const Fog::InternedStringW& name)
  {
    return Fog::INVALID_INDEX;
  }

  static FOG_INLINE size_t getPropertyIndex(Fog::CoreObj* self, const Fog::CharW* name, size_t length)
  {
    return Fog::INVALID_INDEX;
  }

  static FOG_INLINE err_t getPropertyInfo(Fog::CoreObj* self, size_t index, Fog::PropertyInfo& info)
  {
    return Fog::ERR_OBJ_PROPERTY_NOT_FOUND;
  }

  static FOG_INLINE err_t getProperty(Fog::CoreObj* self, size_t index, Fog::StringW& value)
  {
    return Fog::ERR_OBJ_PROPERTY_NOT_FOUND;
  }

  static FOG_INLINE err_t setProperty(Fog::CoreObj* self, size_t index, const Fog::StringW& value)
  {
    return Fog::ERR_OBJ_PROPERTY_NOT_FOUND;
  }

  static FOG_INLINE err_t resetProperty(Fog::CoreObj* self, size_t index)
  {
    return Fog::ERR_OBJ_PROPERTY_NOT_FOUND;
  }
};

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::ObjInfo]
// ============================================================================

struct FOG_NO_EXPORT ObjInfo
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get size of the object, returned by sizeof(className) operator.
  FOG_INLINE uint32_t getSize() const { return _size; }
  
  //! @brief Get count of all properties defined by the object + base objects.
  FOG_INLINE uint32_t getPropertyCount() const { return _propertyCount; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Object size in bytes.
  uint32_t _size;
  
  //! @brief Count of all properties defined by the object + all base objects.
  uint32_t _propertyCount;
};

// ============================================================================
// [Fog::CoreObj]
// ============================================================================

struct FOG_API CoreObj
{
  typedef CoreObj Self;

  enum { _PROPERTY_INDEX = 0 };
  enum { _PROPERTY_COUNT = 0 };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  CoreObj();
  virtual ~CoreObj() = 0;

  // --------------------------------------------------------------------------
  // [ObjInfo]
  // --------------------------------------------------------------------------

  //! @brief Get size of the object, returned by C++ sizeof() operator.
  //!
  //! @note This function must return size of the last object in inheritance
  //! hierarchy. Strictly speaking, each class should override this method so
  //! the returned information matches the instantiated object. To make life
  //! easier, @ref FOG_CORE_OBJ() takes care of that.
  virtual const ObjInfo* getObjInfo() const = 0;

  // --------------------------------------------------------------------------
  // [Properties - Interface]
  // --------------------------------------------------------------------------

  virtual size_t _getPropertyIndex(const InternedStringW& name) const;
  virtual size_t _getPropertyIndex(const CharW* name, size_t length) const;

  virtual err_t _getPropertyInfo(size_t index, PropertyInfo& info) const;

  virtual err_t _getProperty(size_t index, StringW& value) const;
  virtual err_t _setProperty(size_t index, const StringW& value);
  virtual err_t _resetProperty(size_t index);

  virtual err_t _unresolvedProperty(size_t& newIndex,
    const CharW* name, size_t nameLength, const InternedStringW* nameInterned,
    const StringW* initialValue);

  // --------------------------------------------------------------------------
  // [Properties - Public]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getPropertyIndex(const InternedStringW& name) const { return _getPropertyIndex(name); }
  FOG_INLINE size_t getPropertyIndex(const CharW* name, size_t length) const { return _getPropertyIndex(name, length); }

  bool hasProperty(const StringW& name) const;
  bool hasProperty(const StubW& name) const;

  err_t getProperty(const StringW& name, StringW& value) const;
  err_t getProperty(const StubW& name, StringW& value) const;
  FOG_INLINE err_t getProperty(size_t index, StringW& value) const { return _getProperty(index, value); }

  err_t setProperty(const StringW& name, const StringW& value);
  err_t setProperty(const StubW& name, const StringW& value);
  FOG_INLINE err_t setProperty(size_t index, const StringW& value) { return _setProperty(index, value); }

  err_t resetProperty(const StringW& name);
  err_t resetProperty(const StubW& name);
  FOG_INLINE err_t resetProperty(size_t index) { return _resetProperty(index); }

private:
  _FOG_NO_COPY(CoreObj)
};

// ============================================================================
// [FOG_CORE_OBJ]
// ============================================================================

#define FOG_CORE_OBJ(_Self_, _Base_) \
public: \
  typedef _Self_ Self; \
  typedef _Base_ Base; \
  \
  enum { _PROPERTY_INDEX = Base::_PROPERTY_INDEX + Base::_PROPERTY_COUNT }; \
  \
  virtual const ::Fog::ObjInfo* getObjInfo() const override \
  { \
    static const ::Fog::ObjInfo _info = \
    { \
      static_cast<uint32_t>(sizeof(Self)), \
      static_cast<uint32_t>(Self::_PROPERTY_INDEX + Self::_PROPERTY_COUNT) \
    }; \
    \
    return &_info; \
  }

// ============================================================================
// [FOG_PROPERTY_...]
// ============================================================================

//! @brief Begin of property list definition inside class which extends @ref CoreObj.
#define FOG_PROPERTY_DEF() \
private: \
  template<int CommandT> \
  FOG_INLINE size_t _handleProperty(size_t arg, const void* name, void* value); \
  \
public: \
  virtual size_t _getPropertyIndex(const ::Fog::InternedStringW& name) const override; \
  virtual size_t _getPropertyIndex(const ::Fog::CharW* name, size_t length) const override; \
  virtual err_t _getPropertyInfo(size_t index, ::Fog::PropertyInfo& info) const override; \
  virtual err_t _getProperty(size_t index, ::Fog::StringW& value) const override; \
  virtual err_t _setProperty(size_t index, const ::Fog::StringW& value) override; \
  virtual err_t _resetProperty(size_t index) override; \
  \
  enum { _PROPERTY_COUNTER_BASE = (__COUNTER__ + 1) };

//! @brief Define new read-write property.
#define FOG_PROPERTY_RW(_PropertyName_, _Type_) \
  enum { PROPERTY_##_PropertyName_ = _PROPERTY_INDEX + (__COUNTER__ - _PROPERTY_COUNTER_BASE) }; \
  \
  struct FOG_NO_EXPORT _Property_##_PropertyName_ \
  { \
    typedef _Type_ Type; \
    enum { ID = PROPERTY_##_PropertyName_ }; \
    enum { FLAGS = Fog::NO_FLAGS }; \
    \
    static FOG_INLINE void getValue(Self* self, _Type_& value) { value = self->get##_PropertyName_(); } \
    static FOG_INLINE err_t setValue(Self* self, const _Type_& value) { return self->set##_PropertyName_(value); } \
    static FOG_INLINE err_t resetValue(Self* self) { return self->reset##_PropertyName_(); } \
  };

//! @brief Define new read-only property.
#define FOG_PROPERTY_RO(_Property_, _Type_) \
  enum { PROPERTY_##_PropertyName_ = _PROPERTY_INDEX + (__COUNTER__ - _PROPERTY_COUNTER_BASE) }; \
  \
  struct FOG_NO_EXPORT _Property_##_PropertyName_ \
  { \
    typedef _Type_ Type; \
    enum { ID = PROPERTY_##_PropertyName_ }; \
    enum { FLAGS = ::Fog::PROPERTY_READ_ONLY }; \
    \
    static FOG_INLINE void getValue(Self* self, _Type_& value) { value = self->get##_PropertyName_(); } \
    static FOG_INLINE err_t setValue(Self* self, const _Type_& value) { return ::Fog::ERR_PROPERTY_READ_ONLY; } \
    static FOG_INLINE err_t resetValue(Self* self) { return ::Fog::ERR_PROPERTY_READ_ONLY; } \
  };

//! @brief End of property list definition.
#define FOG_PROPERTY_END() \
  enum { _PROPERTY_COUNT = (__COUNTER__ - _PROPERTY_COUNTER_BASE) };

// ============================================================================
// [...]
// ============================================================================

#define FOG_CORE_OBJ_DEF(_Self_) \
  size_t _Self_::_getPropertyIndex(const ::Fog::InternedStringW& name) const \
  { \
    return const_cast<_Self_*>(this)->_handleProperty<::Fog::PROPERTY_HANDLER_INDEX_STRINGW>(\
      ::Fog::INVALID_INDEX, &name, NULL); \
  } \
  \
  size_t _Self_::_getPropertyIndex(const ::Fog::CharW* name, size_t length) const \
  { \
    return const_cast<_Self_*>(this)->_handleProperty<::Fog::PROPERTY_HANDLER_INDEX_STUBW>(\
      length, name, NULL); \
  } \
  \
  err_t _Self_::_getPropertyInfo(size_t index, ::Fog::PropertyInfo& info) const \
  { \
    return static_cast<err_t>(const_cast<_Self_*>(this)->_handleProperty<::Fog::PROPERTY_HANDLER_GET_INFO>(\
      index, NULL, &info)); \
  } \
  \
  err_t _Self_::_getProperty(size_t index, ::Fog::StringW& value) const \
  { \
    return static_cast<err_t>(const_cast<_Self_*>(this)->_handleProperty<::Fog::PROPERTY_HANDLER_GET_STRINGW>(\
      index, NULL, &value)); \
  } \
  \
  err_t _Self_::_setProperty(size_t index, const ::Fog::StringW& value) \
  { \
    return static_cast<err_t>(_handleProperty<::Fog::PROPERTY_HANDLER_SET_STRINGW>(\
      index, NULL, const_cast<::Fog::StringW*>(&value))); \
  } \
  \
  err_t _Self_::_resetProperty(size_t index) \
  { \
    return static_cast<err_t>(_handleProperty<::Fog::PROPERTY_HANDLER_RESET>(\
      index, NULL, NULL)); \
  } \
  \
  template<int CommandT> \
  FOG_INLINE size_t _Self_::_handleProperty(size_t arg, const void* name, void* value) \
  {

// TODO: Naming of these methods is just for "work". We need to clarify concepts
// and to create really descriptive names and sub-macros used inside. Implementation
// details should be called "IMPL" and public macros "PROPERTY".

#define FOG_CORE_OBJ_PROPERTY_IMPL_START(_PropertyName_, _InternedName_) \
    { \
      /* ------------------------------------------------------------------- */ \
      /* [Begin]                                                             */ \
      /* ------------------------------------------------------------------- */ \
      \
      typedef Self::_Property_##_PropertyName_ PropertyDef; \
      const ::Fog::InternedStringW& internedName = _InternedName_;

#define FOG_CORE_OBJ_PROPERTY_IMPL_END() \
      /* ------------------------------------------------------------------- */ \
      /* [End]                                                               */ \
      /* ------------------------------------------------------------------- */ \
    }

#define FOG_CORE_OBJ_PROPERTY_IMPL_INDEX() \
      /* ------------------------------------------------------------------- */ \
      /* [Index]                                                             */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_INDEX_STRINGW) \
      { \
        if (internedName.eq(*static_cast<const ::Fog::InternedStringW*>(name))) \
          return PropertyDef::ID; \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_INDEX_STUBW) \
      { \
        if (internedName.eqInline(static_cast<const ::Fog::CharW*>(name), arg)) \
          return PropertyDef::ID; \
      }

#define FOG_CORE_OBJ_PROPERTY_BASE(_PropertyName_, _InternedName_, _Serializer_) \
    FOG_CORE_OBJ_PROPERTY_IMPL_START(_PropertyName_, _InternedName_) \
    FOG_CORE_OBJ_PROPERTY_IMPL_INDEX() \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Info]                                                              */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_INFO) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          ::Fog::PropertyInfo* info = static_cast<::Fog::PropertyInfo*>(value); \
          \
          info->setName(_InternedName_); \
          info->setIndex(arg); \
          info->setType(0); \
          info->setFlags(PropertyDef::FLAGS); \
          \
          return ::Fog::ERR_OK; \
        } \
      } \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Get]                                                               */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_STRINGW) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          return _Serializer_.serialize( \
            *static_cast<::Fog::StringW*>(value), this->get##_PropertyName_()); \
        } \
      } \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Set]                                                               */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_SET_STRINGW) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          if ((PropertyDef::FLAGS & ::Fog::PROPERTY_FLAG_READ_ONLY) != 0) \
          { \
            return ::Fog::ERR_PROPERTY_READ_ONLY; \
          } \
          else \
          { \
            PropertyDef::Type t; \
            err_t err = _Serializer_.parse( \
              t, *static_cast<const ::Fog::StringW*>(value)); \
            \
            if (FOG_IS_ERROR(err)) \
              return err; \
            \
            return PropertyDef::setValue(this, t); \
          } \
        } \
      } \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Reset]                                                             */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_RESET) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          if ((PropertyDef::FLAGS & ::Fog::PROPERTY_FLAG_READ_ONLY) != 0) \
            return ::Fog::ERR_PROPERTY_READ_ONLY; \
          else \
            return PropertyDef::resetValue(this); \
        } \
        \
      } \
    FOG_CORE_OBJ_PROPERTY_IMPL_END()

#define FOG_CORE_OBJ_PROPERTY_DEFAULT(_PropertyName_, _InternedName_) \
    FOG_CORE_OBJ_PROPERTY_BASE(_PropertyName_, _InternedName_, _Fog_PropertyIO<PropertyDef::Type>())

#define FOG_CORE_OBJ_PROPERTY_REDIRECT(_PropertyName_, _InternedName_, _ToObject_, _ToId_) \
    FOG_CORE_OBJ_PROPERTY_IMPL_START(_PropertyName_, _InternedName_) \
    FOG_CORE_OBJ_PROPERTY_IMPL_INDEX() \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Info]                                                              */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_INFO) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          ::Fog::PropertyInfo* info = static_cast<::Fog::PropertyInfo*>(value); \
          (_ToObject_)->_getPropertyInfo(_ToId_, *info); \
          \
          /* Redirected property has different id, we need to patch the info. */ \
          info->setIndex(arg); \
          return ::Fog::ERR_OK; \
        } \
      } \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Get]                                                               */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_STRINGW) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          return (_ToObject_)->_getProperty(_ToId_, *static_cast<::Fog::StringW*>(value)); \
        } \
      } \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Set]                                                               */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_SET_STRINGW) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          if ((PropertyDef::FLAGS & ::Fog::PROPERTY_FLAG_READ_ONLY) != 0) \
          { \
            return ::Fog::ERR_PROPERTY_READ_ONLY; \
          } \
          else \
          { \
            return (_ToObject_)->_setProperty(_ToId_, *static_cast<const ::Fog::StringW*>(value)); \
          } \
        } \
      } \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Reset]                                                             */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_RESET) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          if ((PropertyDef::FLAGS & ::Fog::PROPERTY_FLAG_READ_ONLY) != 0) \
            return ::Fog::ERR_PROPERTY_READ_ONLY; \
          else \
            return (_ToObject_)->_resetProperty(_ToId_); \
        } \
        \
      } \
    FOG_CORE_OBJ_PROPERTY_IMPL_END()

#define FOG_CORE_OBJ_PROPERTY_ACCESS(_PropertyName_, _InternedName_, _Get_, _Set_, _Reset_) \
    FOG_CORE_OBJ_PROPERTY_IMPL_START(_PropertyName_, _InternedName_) \
    FOG_CORE_OBJ_PROPERTY_IMPL_INDEX() \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Info]                                                              */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_INFO) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          ::Fog::PropertyInfo* info = static_cast<::Fog::PropertyInfo*>(value); \
          \
          info->setName(_InternedName_); \
          info->setIndex(arg); \
          info->setType(0); \
          info->setFlags(PropertyDef::FLAGS); \
          \
          return ::Fog::ERR_OK; \
        } \
      } \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Get]                                                               */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_STRINGW) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          return _Get_(*static_cast<::Fog::StringW*>(value)); \
        } \
      } \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Set]                                                               */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_SET_STRINGW) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          if ((PropertyDef::FLAGS & ::Fog::PROPERTY_FLAG_READ_ONLY) != 0) \
            return ::Fog::ERR_PROPERTY_READ_ONLY; \
          else \
            return _Set_(*static_cast<const ::Fog::StringW*>(value)); \
        } \
      } \
      \
      /* ------------------------------------------------------------------- */ \
      /* [Reset]                                                             */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_RESET) \
      { \
        if (arg == PropertyDef::ID) \
        { \
          if ((PropertyDef::FLAGS & ::Fog::PROPERTY_FLAG_READ_ONLY) != 0) \
            return ::Fog::ERR_PROPERTY_READ_ONLY; \
          else \
            return _Reset_(); \
        } \
        \
      } \
    FOG_CORE_OBJ_PROPERTY_IMPL_END()

#define FOG_CORE_OBJ_HANDLER(_Handler_) \
    { \
      /* ------------------------------------------------------------------- */ \
      /* [Custom Property Handler]                                           */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_INDEX_STRINGW) \
      { \
        return _Handler_::getPropertyIndex(this, \
          *static_cast<const ::Fog::InternedStringW*>(name)); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_INDEX_STUBW) \
      { \
        return _Handler_::getPropertyIndex(this, \
          static_cast<const ::Fog::CharW*>(name), arg); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_INFO) \
      { \
        return _Handler_::getPropertyInfo(this, \
          arg, *static_cast<::Fog::PropertyInfo*>(value)); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_STRINGW) \
      { \
        return _Handler_::getProperty(this, \
          arg, *static_cast<::Fog::StringW*>(value)); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_SET_STRINGW) \
      { \
        return _Handler_::setProperty(this, \
          arg, *static_cast<const ::Fog::StringW*>(value)); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_RESET) \
      { \
        return _Handler_::resetProperty(this, \
          arg); \
      } \
    } \

#define FOG_CORE_OBJ_END() \
    { \
      /* ------------------------------------------------------------------- */ \
      /* [Property Base-Call]                                                */ \
      /* ------------------------------------------------------------------- */ \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_INDEX_STRINGW) \
      { \
        return _Fog_PropertyBaseCall<Base>::getPropertyIndex(this, \
          *static_cast<const ::Fog::InternedStringW*>(name)); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_INDEX_STUBW) \
      { \
        return _Fog_PropertyBaseCall<Base>::getPropertyIndex(this, \
          static_cast<const ::Fog::CharW*>(name), arg); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_INFO) \
      { \
        return _Fog_PropertyBaseCall<Base>::getPropertyInfo(this, \
          arg, *static_cast<::Fog::PropertyInfo*>(value)); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_GET_STRINGW) \
      { \
        return _Fog_PropertyBaseCall<Base>::getProperty(this, \
          arg, *static_cast<::Fog::StringW*>(value)); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_SET_STRINGW) \
      { \
        return _Fog_PropertyBaseCall<Base>::setProperty(this, \
          arg, *static_cast<const ::Fog::StringW*>(value)); \
      } \
      \
      if (CommandT == ::Fog::PROPERTY_HANDLER_RESET) \
      { \
        return _Fog_PropertyBaseCall<Base>::resetProperty(this, \
          arg); \
      } \
    } \
  };

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_KERNEL_COREOBJ_H
