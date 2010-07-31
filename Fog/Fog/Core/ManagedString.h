// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MANAGEDSTRING_H
#define _FOG_CORE_MANAGEDSTRING_H

// [Dependencies]
#include <Fog/Core/Atomic.h>
#include <Fog/Core/String.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_String
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ManagedStringCache;

// ============================================================================
// [Fog::ManagedString]
// ============================================================================

//! @brief Managed string is memory effective string (managed).
//!
//! Managed strings are used in Fog library mainly in hash tables and in areas
//! where string content will be probably shared across many string instances.
//! The string data for all equal string are shared and it's guaranted that 
//! comparing pointer to data is comparing for string equality. So it's very
//! fast to test whether two managed strings are equal or not. Another extension
//! is that string hash code is calculated when creating managed string, so
//! it's also only inlined getter (instead of another call in Fog::StringX class).
//!
//! Why to introduce another string class?
//!
//! It's easy - performance and easy to use API. If you want to see the code that
//! is using managed strings in detail, you should look at Fog-Xml API. For all
//! XML tags and attributes are used managed strings. The idea is very simple -
//! XML tag name and attribute names are very unlikely to change, but they can
//! be processed many times. This means that we need very often to compare tag
//! and attribute names, but we will never change them (in fact, there is no API
//! to change attribute name, you can only add, read or remove it).
//!
//! Managed string's data are immutable.
struct FOG_API ManagedString
{
  // [Node]

  //! @brief Node in internal hash table.
  struct FOG_HIDDEN Node
  {
    // [Construction / Destruction]

    FOG_INLINE Node(const String& s) : 
      string(s),
      next(NULL)
    {
      refCount.init(1);

      // This will calculate hash code. After this call we are not calling
      // function for this, we just use the value stored in String::Data.
      string.getHashCode();
    }

    // Constructor used by @c ManagedString::Cache.
    FOG_INLINE Node(String::Data* s_d) :
      string(s_d),
      next(NULL)
    {
      refCount.init(1);

      // This will calculate hash code. After this call we are not calling
      // function for this, we just use the value stored in String::Data.
      string.getHashCode();
    }

    FOG_INLINE ~Node()
    {
    }

    // [Methods]

    FOG_INLINE const String& getString() const { return string; }
    FOG_INLINE uint32_t getHashCode() const { return string._d->hashCode; }

    // [Ref]

    FOG_INLINE Node* ref() const { refCount.inc(); return const_cast<Node*>(this); }

    // [Members]

    mutable Atomic<sysuint_t> refCount;
    String string;
    Node* next;

  private:
    FOG_DISABLE_COPY(Node)
  };

  // [Construction / Destruction]

  ManagedString();
  ManagedString(const ManagedString& other);
  explicit ManagedString(const String& s);
  explicit ManagedString(const Utf16& s);
  ~ManagedString();

  // [Clear]

  void clear();

  // [Setters]

  err_t set(const ManagedString& s);
  err_t set(const String& s);
  err_t set(const Utf16& s);

  FOG_INLINE err_t set(const Char* s, sysuint_t length = DETECT_LENGTH) { return set(Utf16(s, length)); }

  err_t setIfManaged(const String& s);
  err_t setIfManaged(const Utf16& s);

  FOG_INLINE err_t setIfManaged(const Char* s, sysuint_t length = DETECT_LENGTH) { return setIfManaged(Utf16(s, length)); }

  // [Getters]

  FOG_INLINE bool isEmpty() const { return _node == _dnull; }
  FOG_INLINE sysuint_t refCount() const { return _node->refCount.get(); }
  FOG_INLINE const String& getString() const { return _node->getString(); }
  FOG_INLINE uint32_t getHashCode() const { return _node->getHashCode(); }

  // [Operator Overload]

  FOG_INLINE ManagedString& operator=(const ManagedString& str) { set(str); return *this; }
  FOG_INLINE ManagedString& operator=(const String& str) { set(str); return *this; }
  FOG_INLINE ManagedString& operator=(const Utf16& str) { set(str); return *this; }

  FOG_INLINE bool operator==(const ManagedString& other) { return _node == other._node; }
  FOG_INLINE bool operator==(const String& other) { return _node->string == other; }
  FOG_INLINE bool operator==(const Ascii8& other) { return _node->string == other; }
  FOG_INLINE bool operator==(const Utf16& other) { return _node->string == other; }

  FOG_INLINE bool operator!=(const ManagedString& other) { return _node != other._node; }
  FOG_INLINE bool operator!=(const String& other) { return _node->string != other; }
  FOG_INLINE bool operator!=(const Ascii8& other) { return _node->string != other; }
  FOG_INLINE bool operator!=(const Utf16& other) { return _node->string != other; }

  FOG_INLINE operator const String&() const { return _node->string; }

  // [Cache]

  struct FOG_HIDDEN Cache
  {
    //! @brief Private constructur used by @c ManagedString::createCache().
    FOG_INLINE Cache(const String& name, sysuint_t count) : _name(name), _count(count) {}

    //! @brief Private destructur.
    FOG_INLINE ~Cache() {}

    //! @brief Returns cache name.
    //!
    //! @sa ManagedStringCache::getCacheByName().
    FOG_INLINE const String& getName() const { return _name; }

    //! @brief Return count of managed strings in cache.
    FOG_INLINE sysuint_t getCount() const { return _count; }

    //! @brief Return list of all managed strings in cache.
    FOG_INLINE const ManagedString* getList() const { return (ManagedString *)_data; }

    //! @brief Return reference to managed string at index @a i.
    FOG_INLINE const ManagedString& getString(sysuint_t i) const
    {
      FOG_ASSERT_X(i < _count, "Fog::ManagedString::Cache::getString() - Index out of bounds");
      return ((ManagedString *)_data)[i];
    }

    // [Members]

  private:
    //! @brief Cache name.
    String _name;
    //! @brief Count of strings in cache.
    sysuint_t _count;
    //! @brief Continuous cache memory.
    Node* _data[1];

    friend struct ManagedString;
  };

  //! @brief Create managed string cache (@c ManagedString::Cache).
  //!
  //! @param strings Array of strings to create. Each string ends with zero terminator.
  //! @param length Total length of @a strings with all zero terminators.
  //! @param count Count of zero terminated strings in @a strings.
  //! @param name Optional name of this collection for loadable libraries.
  static Cache* createCache(const char* strings, sysuint_t length, sysuint_t count, const String& name);

  //! @brief Get managed string cache.
  static Cache* getCacheByName(const String& name);

  // [Statics]

  static Node* _dnull;

  // [Members]

  Node* _node;
};

//! @}

} // Fog namespace

FOG_DECLARE_TYPEINFO(Fog::ManagedString, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_CORE_MANAGEDSTRING_H
