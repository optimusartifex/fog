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
  struct FOG_NO_EXPORT Node
  {
    // [Construction / Destruction]

    FOG_INLINE Node(const StringW& s) :
      string(s),
      next(NULL)
    {
      reference.init(1);

      // This will calculate hash code. After this call we are not calling
      // function for this, we just use the value stored in StringDataW.
      string.getHashCode();
    }

    // Constructor used by @c ManagedString::Cache.
    FOG_INLINE Node(StringDataW* s_d) :
      string(s_d),
      next(NULL)
    {
      reference.init(1);

      // This will calculate hash code. After this call we are not calling
      // function for this, we just use the value stored in StringDataW.
      string.getHashCode();
    }

    FOG_INLINE ~Node()
    {
    }

    // [Methods]

    FOG_INLINE const StringW& getString() const { return string; }
    FOG_INLINE uint32_t getHashCode() const { return string._d->hashCode; }

    // [Ref]

    FOG_INLINE Node* addRef() const { reference.inc(); return const_cast<Node*>(this); }

    // [Members]

    mutable Atomic<size_t> reference;
    StringW string;
    Node* next;

  private:
    _FOG_NO_COPY(Node)
  };

  // [Construction / Destruction]

  ManagedString();
  ManagedString(const ManagedString& other);
  explicit ManagedString(const StringW& s);
  explicit ManagedString(const StubW& s);
  ~ManagedString();

  // [Clear]

  void clear();

  // [Setters]

  err_t set(const ManagedString& s);
  err_t set(const StringW& s);
  err_t set(const StubW& s);

  FOG_INLINE err_t set(const CharW* s, size_t length = DETECT_LENGTH) { return set(StubW(s, length)); }

  err_t setIfManaged(const StringW& s);
  err_t setIfManaged(const StubW& s);

  FOG_INLINE err_t setIfManaged(const CharW* s, size_t length = DETECT_LENGTH) { return setIfManaged(StubW(s, length)); }

  // [Getters]

  FOG_INLINE bool isEmpty() const { return _node == _dnull; }
  FOG_INLINE size_t getReference() const { return _node->reference.get(); }
  FOG_INLINE const StringW& getString() const { return _node->getString(); }
  FOG_INLINE uint32_t getHashCode() const { return _node->getHashCode(); }

  // [Operator Overload]

  FOG_INLINE ManagedString& operator=(const ManagedString& str) { set(str); return *this; }
  FOG_INLINE ManagedString& operator=(const StringW& str) { set(str); return *this; }
  FOG_INLINE ManagedString& operator=(const StubW& str) { set(str); return *this; }

  FOG_INLINE bool operator==(const ManagedString& other) { return _node == other._node; }
  FOG_INLINE bool operator==(const StringW& other) { return _node->string == other; }
  FOG_INLINE bool operator==(const Ascii8& other) { return _node->string == other; }
  FOG_INLINE bool operator==(const StubW& other) { return _node->string == other; }

  FOG_INLINE bool operator!=(const ManagedString& other) { return _node != other._node; }
  FOG_INLINE bool operator!=(const StringW& other) { return _node->string != other; }
  FOG_INLINE bool operator!=(const Ascii8& other) { return _node->string != other; }
  FOG_INLINE bool operator!=(const StubW& other) { return _node->string != other; }

  FOG_INLINE operator const StringW&() const { return _node->string; }

  // [Cache]

  struct FOG_NO_EXPORT Cache
  {
    //! @brief Private constructur used by @c ManagedString::createCache().
    FOG_INLINE Cache(const StringW& name, size_t count) : _name(name), _count(count) {}

    //! @brief Private destructur.
    FOG_INLINE ~Cache() {}

    //! @brief Returns cache name.
    //!
    //! @sa ManagedStringCache::getCacheByName().
    FOG_INLINE const StringW& getName() const { return _name; }

    //! @brief Return count of managed strings in cache.
    FOG_INLINE size_t getCount() const { return _count; }

    //! @brief Return list of all managed strings in cache.
    FOG_INLINE const ManagedString* getList() const { return (ManagedString *)_data; }

    //! @brief Return reference to managed string at index @a i.
    FOG_INLINE const ManagedString& getString(size_t i) const
    {
      FOG_ASSERT_X(i < _count, "Fog::ManagedString::Cache::getString() - Index out of bounds");
      return ((ManagedString *)_data)[i];
    }

    // [Members]

  private:
    //! @brief Cache name.
    StringW _name;
    //! @brief Count of strings in cache.
    size_t _count;
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
  static Cache* createCache(const char* strings, size_t length, size_t count, const StringW& name);

  //! @brief Get managed string cache.
  static Cache* getCacheByName(const StringW& name);

  // [Statics]

  static Node* _dnull;

  // [Members]

  Node* _node;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_MANAGEDSTRING_H
