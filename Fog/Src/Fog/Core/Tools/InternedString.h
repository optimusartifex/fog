// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_INTERNEDSTRING_H
#define _FOG_CORE_TOOLS_INTERNEDSTRING_H

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

struct InternedStringCacheW;

// ============================================================================
// [Fog::InternedStringW]
// ============================================================================

//! @brief Interned string is memory efficient string with some unique properties.
//!
//! Interned strings are used in Fog-Framework mainly in hash tables and in areas
//! where string content will be probably shared across many string instances.
//! The string data for all equal string are shared and it's guaranted that
//! comparing data pointer is comparing for string equality, thus comparison is
//! very fast (like comparing two numbers). Another extension to the concept is
//! that string hash code is calculated when creating interned string, so the
//! getHashCode() method is inlined, rather than a wrapper to a function call.
//!
//! Why to introduce another string class?
//!
//! Interned string was created mainly for simplicity. In some languages (for
//! example Java or .NET) there is no difference between interned string and
//! string created from char[] array, but in Fog-Framework string is mutable,
//! so it's not possible to keep such concept. However, @ref InternedStringW
//! can be freely casted into @ref StringW, thus there is no difficulty to use
//! it.
//!
//! Interned string in Fog-Framework is simple to use, and it's used internally
//! in many places, for example object property names are stored as interned
//! strings, and xml-dom tag names and attributes (mapped to object properties)
//! are stored as interned strings as well.
//!
//! Some useful links:
//!   - http://en.wikipedia.org/wiki/String_interning (General)
struct FOG_NO_EXPORT InternedStringW
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE InternedStringW()
  {
    fog_api.internedstringw_ctor(this);
  }

  FOG_INLINE InternedStringW(const InternedStringW& other)
  {
    fog_api.internedstringw_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE InternedStringW(InternedStringW&& other) { _string->_d = other._string->_d; other._string->_d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE InternedStringW(const Ascii8& stub, uint32_t options = INTERNED_STRING_OPTION_NONE)
  {
    fog_api.internedstringw_ctorStubA(this, &stub, options);
  }

  explicit FOG_INLINE InternedStringW(const StubW& stub, uint32_t options = INTERNED_STRING_OPTION_NONE)
  {
    fog_api.internedstringw_ctorStubW(this, &stub, options);
  }

  explicit FOG_INLINE InternedStringW(const StringW& str, uint32_t options = INTERNED_STRING_OPTION_NONE)
  {
    fog_api.internedstringw_ctorStringW(this, &str, options);
  }

  FOG_INLINE ~InternedStringW()
  {
    fog_api.internedstringw_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t set(const Ascii8& stub, uint32_t options = INTERNED_STRING_OPTION_NONE)
  {
    return fog_api.internedstringw_setStubA(this, &stub, options);
  }

  FOG_INLINE err_t set(const StubW& stub, uint32_t options = INTERNED_STRING_OPTION_NONE)
  {
    return fog_api.internedstringw_setStubW(this, &stub, options);
  }

  FOG_INLINE err_t set(const StringW& str, uint32_t options = INTERNED_STRING_OPTION_NONE)
  {
    return fog_api.internedstringw_setStringW(this, &str, options);
  }

  FOG_INLINE err_t set(const InternedStringW& str)
  {
    return fog_api.internedstringw_setInterned(this, &str);
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
    fog_api.internedstringw_reset(this);
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

  FOG_INLINE bool eq(const InternedStringW& other) const
  {
    return _string->_d == other._string->_d;
  }

  FOG_INLINE bool eqInline(const char* sData, size_t sLength) const
  {
    const StringDataW* d = _string->_d;

    if (d->length != sLength)
      return false;

    for (size_t i = 0; i < sLength; i++)
      if (d->data[i] != static_cast<uint8_t>(sData[i]))
        return false;

    return true;
  }

  FOG_INLINE bool eqInline(const CharW* sData, size_t sLength) const
  {
    const StringDataW* d = _string->_d;

    if (d->length != sLength)
      return false;

    for (size_t i = 0; i < sLength; i++)
      if (d->data[i] != sData[i])
        return false;

    return true;
  }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const InternedStringW& getEmptyInstance()
  {
    return *reinterpret_cast<InternedStringW*>(fog_api.stringw_oEmpty);
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const InternedStringW* a, const InternedStringW* b)
  {
    return a->_string->_d == b->_string->_d;
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.internedstringw_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Cleanup]
  // --------------------------------------------------------------------------

  static FOG_INLINE void cleanup()
  {
    fog_api.internedstringw_cleanup();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE InternedStringW& operator=(const Ascii8& str) { set(str); return *this; }
  FOG_INLINE InternedStringW& operator=(const StubW& str) { set(str); return *this; }
  FOG_INLINE InternedStringW& operator=(const StringW& str) { set(str); return *this; }
  FOG_INLINE InternedStringW& operator=(const InternedStringW& str) { set(str); return *this; }

  FOG_INLINE bool operator==(const Ascii8& stub) const { return  eq(stub); }
  FOG_INLINE bool operator!=(const Ascii8& stub) const { return !eq(stub); }

  FOG_INLINE bool operator==(const StubW& stub) const { return  eq(stub); }
  FOG_INLINE bool operator!=(const StubW& stub) const { return !eq(stub); }

  FOG_INLINE bool operator==(const StringW& str) const { return  eq(str); }
  FOG_INLINE bool operator!=(const StringW& str) const { return !eq(str); }

  FOG_INLINE bool operator==(const InternedStringW& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const InternedStringW& other) const { return !eq(other); }

  FOG_INLINE operator const StringW&() const { return _string; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<StringW> _string;
};

// ============================================================================
// [Fog::InternedStringCacheW]
// ============================================================================

struct FOG_NO_EXPORT InternedStringCacheW
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the cache length.
  FOG_INLINE size_t getLength() const
  {
    return _length;
  }

  //! @brief Get the list of InternedStringW instances in the cache.
  FOG_INLINE const InternedStringW* getData() const
  {
    return reinterpret_cast<const InternedStringW *>(this + 1);
  }

  //! @brief Return reference to managed string at index @a i.
  FOG_INLINE const InternedStringW& getString(size_t i) const
  {
    FOG_ASSERT_X(i < _length,
      "Fog::InternedStringCacheW::getString() - Index out of bounds.");

    return reinterpret_cast<const InternedStringW *>(this + 1)[i];
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Create ref @c InternedStringCacheW.
  //!
  //! @param strings Array of strings to create. Each string ends with a zero terminator.
  //! @param length Total length of @a strings with all zero terminators.
  //! @param count Count of zero terminated strings in @a strings.
  //! @param name Optional name of this collection for loadable libraries.
  static FOG_INLINE InternedStringCacheW* create(const char* sData, size_t sLength, size_t listLength)
  {
    return fog_api.internedstringcachew_create(sData, sLength, listLength);
  }

  //! @brief Get global @ref InternedStringCacheW.
  static FOG_INLINE InternedStringCacheW* get()
  {
    return fog_api.internedstringcachew_oInstance;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Cache length.
  size_t _length;
};

// ============================================================================
// [Fog::FOG_S_]
// ============================================================================

#define FOG_S(_Id_) (::Fog::InternedStringCacheW::get()->getString(::Fog::STR_##_Id_))

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_INTERNEDSTRING_H
