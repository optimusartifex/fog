// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_MANAGEDSTRING_H
#define _FOG_CORE_TOOLS_MANAGEDSTRING_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ManagedStringCacheW;

// ============================================================================
// [Fog::ManagedStringW]
// ============================================================================

//! @brief Managed string is memory efficient string with some unique properties.
//!
//! Managed strings are used in Fog-Framework mainly in hash tables and in areas
//! where string content will be probably shared across many string instances.
//! The string data for all equal string are shared and it's guaranted that
//! comparing pointer to data is comparing for string equality. So it's very
//! fast to test whether two managed strings are equal. Another extension is that
//! string hash code is calculated when creating managed string, so it's inlined
//! instead of calling private method to get it.
//!
//! Why to introduce another string class?
//!
//! It's easy - performance and easy to use API. If you want to see the code that
//! is using managed strings in detail, you should look at Fog::Object and
//! Fog::Xml. For all XML tags and attributes managed strings are used. The
//! idea is very simple - XML tag names and attributes are very likely to be
//! used in many XML elements, thus we can save memory and processing.
//!
//! Managed string's data are immutable in contrast to the non-shared @c StringW
//! data.
struct FOG_NO_EXPORT ManagedStringW
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ManagedStringW()
  {
    fog_api.managedstringw_ctor(this);
  }

  FOG_INLINE ManagedStringW(const ManagedStringW& other)
  {
    fog_api.managedstringw_ctorCopy(this, &other);
  }

  explicit FOG_INLINE ManagedStringW(const Ascii8& stub, uint32_t options = MANAGED_STRING_OPTION_NONE)
  {
    fog_api.managedstringw_ctorStubA(this, &stub, options);
  }

  explicit FOG_INLINE ManagedStringW(const StubW& stub, uint32_t options = MANAGED_STRING_OPTION_NONE)
  {
    fog_api.managedstringw_ctorStubW(this, &stub, options);
  }

  explicit FOG_INLINE ManagedStringW(const StringW& str, uint32_t options = MANAGED_STRING_OPTION_NONE)
  {
    fog_api.managedstringw_ctorStringW(this, &str, options);
  }

  FOG_INLINE ~ManagedStringW()
  {
    fog_api.managedstringw_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t set(const Ascii8& stub, uint32_t options = MANAGED_STRING_OPTION_NONE)
  {
    return fog_api.managedstringw_setStubA(this, &stub, options);
  }

  FOG_INLINE err_t set(const StubW& stub, uint32_t options = MANAGED_STRING_OPTION_NONE)
  {
    return fog_api.managedstringw_setStubW(this, &stub, options);
  }

  FOG_INLINE err_t set(const StringW& str, uint32_t options = MANAGED_STRING_OPTION_NONE)
  {
    return fog_api.managedstringw_setStringW(this, &str, options);
  }

  FOG_INLINE err_t set(const ManagedStringW& str)
  {
    return fog_api.managedstringw_setManaged(this, &str);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _string->_d->reference.get(); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.managedstringw_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getLength() const
  {
    return _string->_d->length;
  }

  FOG_INLINE bool isEmpty() const
  {
    return _string->_d->length == 0;
  }

  FOG_INLINE uint32_t getHashCode() const
  {
    return _string->_d->hashCode;
  }

  FOG_INLINE const CharW* getData() const
  {
    return _string->_d->data;
  }

  FOG_INLINE const StringW& getString() const
  {
    return _string;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Ascii8& stub) const
  {
    return fog_api.stringw_eqStubA(&_string, &stub);
  }

  FOG_INLINE bool eq(const StubW& stub) const
  {
    return fog_api.stringw_eqStubW(&_string, &stub);
  }

  FOG_INLINE bool eq(const StringW& str) const
  {
    return fog_api.stringw_eqStringW(&_string, &str);
  }

  FOG_INLINE bool eq(const ManagedStringW& other) const
  {
    return _string->_d == other._string->_d;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ManagedStringW* a, const ManagedStringW* b)
  {
    return a->_string->_d == b->_string->_d;
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.managedstringw_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Cleanup]
  // --------------------------------------------------------------------------

  static FOG_INLINE void cleanup()
  {
    fog_api.managedstringw_cleanup();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ManagedStringW& operator=(const Ascii8& str) { set(str); return *this; }
  FOG_INLINE ManagedStringW& operator=(const StubW& str) { set(str); return *this; }
  FOG_INLINE ManagedStringW& operator=(const StringW& str) { set(str); return *this; }
  FOG_INLINE ManagedStringW& operator=(const ManagedStringW& str) { set(str); return *this; }

  FOG_INLINE bool operator==(const Ascii8& stub) const { return  eq(stub); }
  FOG_INLINE bool operator!=(const Ascii8& stub) const { return !eq(stub); }

  FOG_INLINE bool operator==(const StubW& stub) const { return  eq(stub); }
  FOG_INLINE bool operator!=(const StubW& stub) const { return !eq(stub); }

  FOG_INLINE bool operator==(const StringW& str) const { return  eq(str); }
  FOG_INLINE bool operator!=(const StringW& str) const { return !eq(str); }

  FOG_INLINE bool operator==(const ManagedStringW& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ManagedStringW& other) const { return !eq(other); }

  FOG_INLINE operator const StringW&() const { return _string; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<StringW> _string;
};

// ============================================================================
// [Fog::ManagedStringCacheW]
// ============================================================================

struct FOG_NO_EXPORT ManagedStringCacheW
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the cache length.
  FOG_INLINE size_t getLength() const
  {
    return _length;
  }

  //! @brief Get the list of ManagedStringW instances in the cache.
  FOG_INLINE const ManagedStringW* getData() const
  {
    return reinterpret_cast<const ManagedStringW *>(this + 1);
  }

  //! @brief Return reference to managed string at index @a i.
  FOG_INLINE const ManagedStringW& getString(size_t i) const
  {
    FOG_ASSERT_X(i < _length,
      "Fog::ManagedStringCacheW::getString() - Index out of bounds.");

    return reinterpret_cast<const ManagedStringW *>(this + 1)[i];
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Create ref @c ManagedStringCacheW.
  //!
  //! @param strings Array of strings to create. Each string ends with a zero terminator.
  //! @param length Total length of @a strings with all zero terminators.
  //! @param count Count of zero terminated strings in @a strings.
  //! @param name Optional name of this collection for loadable libraries.
  static FOG_INLINE ManagedStringCacheW* create(const char* sData, size_t sLength, size_t listLength)
  {
    return fog_api.managedstringcachew_create(sData, sLength, listLength);
  }

  //! @brief Get global @ref ManagedStringCacheW.
  static FOG_INLINE ManagedStringCacheW* get()
  {
    return fog_api.managedstringcachew_oInstance;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Cache length.
  size_t _length;
};

// ============================================================================
// [Fog::FOG_STR_]
// ============================================================================

#define FOG_STR_(_Id_) (::Fog::ManagedStringCacheW::get()->getString(::Fog::STR_##_Id_))

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_MANAGEDSTRING_H
