// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_HASHSTRING_H
#define _FOG_CORE_TOOLS_HASHSTRING_H

// [Dependencies]
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::HashStringAImpl<Unknown>]
// ============================================================================

template<typename ItemT, int IndexableItem>
struct HashStringAImpl {};

template<typename ItemT>
struct HashStringAImpl<ItemT, 0> : public HashUntyped
{
  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  using HashUntyped::getReference;
  using HashUntyped::isDetached;

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _detach();
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.hash_unknown_unknown_detach(this, HashVTable<StringA, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.hash_unknown_unknown_reserve(this, HashVTable<StringA, ItemT>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.hash_unknown_unknown_squeeze(this, HashVTable<StringA, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.hash_unknown_unknown_clear(this, HashVTable<StringA, ItemT>::getVTable());
  }

  FOG_INLINE void reset()
  {
    fog_api.hash_unknown_unknown_reset(this, HashVTable<StringA, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const StubA& key) const
  {
    return fog_api.hash_stringa_unknown_getStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE bool contains(const StringA& key) const
  {
    return fog_api.hash_stringa_unknown_getStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE const ItemT& get(const StubA& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringa_unknown_getStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT& get(const StringA& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringa_unknown_getStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const StubA& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringa_unknown_getStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const StubA& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringa_unknown_getStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const StringA& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringa_unknown_getStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const StringA& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringa_unknown_getStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const StubA& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringa_unknown_useStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const StubA& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringa_unknown_useStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const StringA& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringa_unknown_useStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const StringA& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringa_unknown_useStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const StubA& key, const ItemT& item, bool replace = true)
  {
    return fog_api.hash_stringa_unknown_putStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringA& key, const ItemT& item, bool replace = true)
  {
    return fog_api.hash_stringa_unknown_putStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t remove(const StubA& key)
  {
    return fog_api.hash_stringa_unknown_removeStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key);
  }

  FOG_INLINE err_t remove(const StringA& key)
  {
    return fog_api.hash_stringa_unknown_removeStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key);
  }
};

template<typename ItemT>
struct HashStringAImpl<ItemT, 1> : public HashStringAImpl<ItemT, 0>
{
  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringA, ItemT>& other) const
  {
    return fog_api.hash_stringa_unknown_eq(this, &other,
      HashVTable<StringA, ItemT>::getVTable(), TypeFunc<ItemT>::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator==(const Hash<StringA, ItemT>& other) { return  eq(other); }
  FOG_INLINE bool operator!=(const Hash<StringA, ItemT>& other) { return !eq(other); }
};

// ============================================================================
// [Fog::Hash<StringA, Unknown>]
// ============================================================================

template<typename ItemT>
struct Hash<StringA, ItemT> : public HashStringAImpl<ItemT, !TypeInfo<ItemT>::NO_EQ>
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Hash()
  {
    fog_api.hash_unknown_unknown_ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringA, ItemT>& other)
  {
    fog_api.hash_unknown_unknown_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Hash(Hash<StringA, ItemT>&& other) { HashUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE ~Hash()
  {
    fog_api.hash_unknown_unknown_dtor(this, HashVTable<StringA, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringA, ItemT>& operator=(const Hash<StringA, ItemT>& other)
  {
    fog_api.hash_unknown_unknown_copy(this, HashVTable<StringA, ItemT>::getVTable(), &other);
    return *this;
  }
};

// ============================================================================
// [Fog::Hash<StringA, StringA>]
// ============================================================================

template<>
struct Hash<StringA, StringA> : public HashUntyped
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Hash()
  {
    fog_api.hash_unknown_unknown_ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringA, StringA>& other)
  {
    fog_api.hash_unknown_unknown_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Hash(Hash<StringA, StringA>&& other) { HashUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE ~Hash()
  {
    fog_api.hash_unknown_unknown_dtor(this, HashVTable<StringA, StringA>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  using HashUntyped::getReference;
  using HashUntyped::isDetached;

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _detach();
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.hash_unknown_unknown_detach(this, HashVTable<StringA, StringA>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.hash_unknown_unknown_reserve(this, HashVTable<StringA, StringA>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.hash_unknown_unknown_squeeze(this, HashVTable<StringA, StringA>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.hash_unknown_unknown_clear(this, HashVTable<StringA, StringA>::getVTable());
  }

  FOG_INLINE void reset()
  {
    fog_api.hash_unknown_unknown_reset(this, HashVTable<StringA, StringA>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const StubA& key) const
  {
    return fog_api.hash_stringa_stringa_getStubA(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StringA& key) const
  {
    return fog_api.hash_stringa_stringa_getStringA(this, &key) != NULL;
  }

  FOG_INLINE const StringA& get(const StubA& key, const StringA& notFound) const
  {
    const StringA* item = fog_api.hash_stringa_stringa_getStubA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringA& get(const StringA& key, const StringA& notFound) const
  {
    const StringA* item = fog_api.hash_stringa_stringa_getStringA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringA* getPtr(const StubA& key) const
  {
    const StringA* item = fog_api.hash_stringa_stringa_getStubA(this, &key);

    return item;
  }

  FOG_INLINE const StringA* getPtr(const StubA& key, const StringA* notFound) const
  {
    const StringA* item = fog_api.hash_stringa_stringa_getStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const StringA* getPtr(const StringA& key) const
  {
    const StringA* item = fog_api.hash_stringa_stringa_getStringA(this, &key);

    return item;
  }

  FOG_INLINE const StringA* getPtr(const StringA& key, const StringA* notFound) const
  {
    const StringA* item = fog_api.hash_stringa_stringa_getStringA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringA* usePtr(const StubA& key)
  {
    StringA* item = fog_api.hash_stringa_stringa_useStubA(this, &key);

    return item;
  }

  FOG_INLINE StringA* usePtr(const StubA& key, StringA* notFound)
  {
    StringA* item = fog_api.hash_stringa_stringa_useStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringA* usePtr(const StringA& key)
  {
    StringA* item = fog_api.hash_stringa_stringa_useStringA(this, &key);

    return item;
  }

  FOG_INLINE StringA* usePtr(const StringA& key, StringA* notFound)
  {
    StringA* item = fog_api.hash_stringa_stringa_useStringA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const StubA& key, const StringA& item, bool replace = true)
  {
    return fog_api.hash_stringa_stringa_putStubA(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringA& key, const StringA& item, bool replace = true)
  {
    return fog_api.hash_stringa_stringa_putStringA(this, &key, &item, replace);
  }

  FOG_INLINE err_t remove(const StubA& key)
  {
    return fog_api.hash_stringa_stringa_removeStubA(this, &key);
  }

  FOG_INLINE err_t remove(const StringA& key)
  {
    return fog_api.hash_stringa_stringa_removeStringA(this, &key);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringA, StringA>& other) const
  {
    return fog_api.hash_stringa_unknown_eq(this, &other, HashVTable<StringA, StringA>::getVTable(), StringA::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringA, StringA>& operator=(const Hash<StringA, StringA>& other)
  {
    fog_api.hash_unknown_unknown_copy(this, HashVTable<StringA, StringA>::getVTable(), &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Hash<StringA, StringA>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Hash<StringA, StringA>& other) const { return !eq(other); }
};

// ============================================================================
// [Fog::Hash<StringA, Var>]
// ============================================================================

template<>
struct Hash<StringA, Var> : public HashUntyped
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Hash()
  {
    fog_api.hash_unknown_unknown_ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringA, Var>& other)
  {
    fog_api.hash_unknown_unknown_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Hash(Hash<StringA, Var>&& other) { HashUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE ~Hash()
  {
    fog_api.hash_unknown_unknown_dtor(this, HashVTable<StringA, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  using HashUntyped::getReference;
  using HashUntyped::isDetached;

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _detach();
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.hash_unknown_unknown_detach(this, HashVTable<StringA, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.hash_unknown_unknown_reserve(this, HashVTable<StringA, Var>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.hash_unknown_unknown_squeeze(this, HashVTable<StringA, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.hash_unknown_unknown_clear(this, HashVTable<StringA, Var>::getVTable());
  }

  FOG_INLINE void reset()
  {
    fog_api.hash_unknown_unknown_reset(this, HashVTable<StringA, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const StubA& key) const
  {
    return fog_api.hash_stringa_stringa_getStubA(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StringA& key) const
  {
    return fog_api.hash_stringa_stringa_getStringA(this, &key) != NULL;
  }

  FOG_INLINE const Var& get(const StubA& key, const Var& notFound) const
  {
    const Var* item = fog_api.hash_stringa_var_getStubA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var& get(const StringA& key, const Var& notFound) const
  {
    const Var* item = fog_api.hash_stringa_var_getStringA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const StubA& key) const
  {
    const Var* item = fog_api.hash_stringa_var_getStubA(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const StubA& key, const Var* notFound) const
  {
    const Var* item = fog_api.hash_stringa_var_getStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const StringA& key) const
  {
    const Var* item = fog_api.hash_stringa_var_getStringA(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const StringA& key, const Var* notFound) const
  {
    const Var* item = fog_api.hash_stringa_var_getStringA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const StubA& key)
  {
    Var* item = fog_api.hash_stringa_var_useStubA(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const StubA& key, Var* notFound)
  {
    Var* item = fog_api.hash_stringa_var_useStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const StringA& key)
  {
    Var* item = fog_api.hash_stringa_var_useStringA(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const StringA& key, Var* notFound)
  {
    Var* item = fog_api.hash_stringa_var_useStringA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const StubA& key, const Var& item, bool replace = true)
  {
    return fog_api.hash_stringa_var_putStubA(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringA& key, const Var& item, bool replace = true)
  {
    return fog_api.hash_stringa_var_putStringA(this, &key, &item, replace);
  }

  FOG_INLINE err_t remove(const StubA& key)
  {
    return fog_api.hash_stringa_var_removeStubA(this, &key);
  }

  FOG_INLINE err_t remove(const StringA& key)
  {
    return fog_api.hash_stringa_var_removeStringA(this, &key);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringA, Var>& other) const
  {
    return fog_api.hash_stringa_unknown_eq(this, &other, HashVTable<StringA, Var>::getVTable(), (EqFunc)fog_api.var_eq);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringA, Var>& operator=(const Hash<StringA, Var>& other)
  {
    fog_api.hash_unknown_unknown_copy(this, HashVTable<StringA, Var>::getVTable(), &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Hash<StringA, Var>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Hash<StringA, Var>& other) const { return !eq(other); }
};

// ============================================================================
// [Fog::HashStringWImpl<Unknown>]
// ============================================================================

template<typename ItemT, int IndexableItem>
struct HashStringWImpl {};

template<typename ItemT>
struct HashStringWImpl<ItemT, 0> : public HashUntyped
{
  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  using HashUntyped::getReference;
  using HashUntyped::isDetached;

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _detach();
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.hash_unknown_unknown_detach(this, HashVTable<StringW, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.hash_unknown_unknown_reserve(this, HashVTable<StringW, ItemT>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.hash_unknown_unknown_squeeze(this, HashVTable<StringW, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.hash_unknown_unknown_clear(this, HashVTable<StringW, ItemT>::getVTable());
  }

  FOG_INLINE void reset()
  {
    fog_api.hash_unknown_unknown_reset(this, HashVTable<StringW, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const Ascii8& key) const
  {
    return fog_api.hash_stringw_unknown_getStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE bool contains(const StubW& key) const
  {
    return fog_api.hash_stringw_unknown_getStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE bool contains(const StringW& key) const
  {
    return fog_api.hash_stringw_unknown_getStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE const ItemT& get(const Ascii8& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringw_unknown_getStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT& get(const StubW& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringw_unknown_getStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT& get(const StringW& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringw_unknown_getStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const Ascii8& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringw_unknown_getStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const Ascii8& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringw_unknown_getStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const StubW& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringw_unknown_getStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const StubW& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringw_unknown_getStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const StringW& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringw_unknown_getStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const StringW& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_stringw_unknown_getStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const Ascii8& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringw_unknown_useStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const Ascii8& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringw_unknown_useStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const StubW& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringw_unknown_useStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const StubW& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringw_unknown_useStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const StringW& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringw_unknown_useStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const StringW& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_stringw_unknown_useStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const Ascii8& key, const ItemT& item, bool replace = true)
  {
    return fog_api.hash_stringw_unknown_putStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t put(const StubW& key, const ItemT& item, bool replace = true)
  {
    return fog_api.hash_stringw_unknown_putStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringW& key, const ItemT& item, bool replace = true)
  {
    return fog_api.hash_stringw_unknown_putStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t remove(const Ascii8& key)
  {
    return fog_api.hash_stringw_unknown_removeStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key);
  }

  FOG_INLINE err_t remove(const StubW& key)
  {
    return fog_api.hash_stringw_unknown_removeStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key);
  }

  FOG_INLINE err_t remove(const StringW& key)
  {
    return fog_api.hash_stringw_unknown_removeStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key);
  }
};

template<typename ItemT>
struct HashStringWImpl<ItemT, 1> : public HashStringWImpl<ItemT, 0>
{
  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringW, ItemT>& other) const
  {
    return fog_api.hash_stringw_unknown_eq(this, &other,
      HashVTable<StringW, ItemT>::getVTable(), TypeFunc<ItemT>::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator==(const Hash<StringW, ItemT>& other) { return  eq(other); }
  FOG_INLINE bool operator!=(const Hash<StringW, ItemT>& other) { return !eq(other); }
};

// ============================================================================
// [Fog::Hash<StringW, Unknown>]
// ============================================================================

template<typename ItemT>
struct Hash<StringW, ItemT> : public HashStringWImpl<ItemT, !TypeInfo<ItemT>::NO_EQ>
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Hash()
  {
    fog_api.hash_unknown_unknown_ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringW, ItemT>& other)
  {
    fog_api.hash_unknown_unknown_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Hash(Hash<StringW, ItemT>&& other) { HashUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE ~Hash()
  {
    fog_api.hash_unknown_unknown_dtor(this, HashVTable<StringW, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringW, ItemT>& operator=(const Hash<StringW, ItemT>& other)
  {
    fog_api.hash_unknown_unknown_copy(this, HashVTable<StringW, ItemT>::getVTable(), &other);
    return *this;
  }
};

// ============================================================================
// [Fog::Hash<StringW, StringW>]
// ============================================================================

template<>
struct Hash<StringW, StringW> : public HashUntyped
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Hash()
  {
    fog_api.hash_unknown_unknown_ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringW, StringW>& other)
  {
    fog_api.hash_unknown_unknown_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Hash(Hash<StringW, StringW>&& other) { HashUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE ~Hash()
  {
    fog_api.hash_unknown_unknown_dtor(this, HashVTable<StringW, StringW>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  using HashUntyped::getReference;
  using HashUntyped::isDetached;

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _detach();
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.hash_unknown_unknown_detach(this, HashVTable<StringW, StringW>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.hash_unknown_unknown_reserve(this, HashVTable<StringW, StringW>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.hash_unknown_unknown_squeeze(this, HashVTable<StringW, StringW>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.hash_unknown_unknown_clear(this, HashVTable<StringW, StringW>::getVTable());
  }

  FOG_INLINE void reset()
  {
    fog_api.hash_unknown_unknown_reset(this, HashVTable<StringW, StringW>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const Ascii8& key) const
  {
    return fog_api.hash_stringw_stringw_getStubA(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StubW& key) const
  {
    return fog_api.hash_stringw_stringw_getStubW(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StringW& key) const
  {
    return fog_api.hash_stringw_stringw_getStringW(this, &key) != NULL;
  }

  FOG_INLINE const StringW& get(const Ascii8& key, const StringW& notFound) const
  {
    const StringW* item = fog_api.hash_stringw_stringw_getStubA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringW& get(const StubW& key, const StringW& notFound) const
  {
    const StringW* item = fog_api.hash_stringw_stringw_getStubW(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringW& get(const StringW& key, const StringW& notFound) const
  {
    const StringW* item = fog_api.hash_stringw_stringw_getStringW(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringW* getPtr(const Ascii8& key) const
  {
    const StringW* item = fog_api.hash_stringw_stringw_getStubA(this, &key);

    return item;
  }

  FOG_INLINE const StringW* getPtr(const Ascii8& key, const StringW* notFound) const
  {
    const StringW* item = fog_api.hash_stringw_stringw_getStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const StringW* getPtr(const StubW& key) const
  {
    const StringW* item = fog_api.hash_stringw_stringw_getStubW(this, &key);

    return item;
  }

  FOG_INLINE const StringW* getPtr(const StubW& key, const StringW* notFound) const
  {
    const StringW* item = fog_api.hash_stringw_stringw_getStubW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const StringW* getPtr(const StringW& key) const
  {
    const StringW* item = fog_api.hash_stringw_stringw_getStringW(this, &key);

    return item;
  }

  FOG_INLINE const StringW* getPtr(const StringW& key, const StringW* notFound) const
  {
    const StringW* item = fog_api.hash_stringw_stringw_getStringW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringW* usePtr(const Ascii8& key)
  {
    StringW* item = fog_api.hash_stringw_stringw_useStubA(this, &key);

    return item;
  }

  FOG_INLINE StringW* usePtr(const Ascii8& key, StringW* notFound)
  {
    StringW* item = fog_api.hash_stringw_stringw_useStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringW* usePtr(const StubW& key)
  {
    StringW* item = fog_api.hash_stringw_stringw_useStubW(this, &key);

    return item;
  }

  FOG_INLINE StringW* usePtr(const StubW& key, StringW* notFound)
  {
    StringW* item = fog_api.hash_stringw_stringw_useStubW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringW* usePtr(const StringW& key)
  {
    StringW* item = fog_api.hash_stringw_stringw_useStringW(this, &key);

    return item;
  }

  FOG_INLINE StringW* usePtr(const StringW& key, StringW* notFound)
  {
    StringW* item = fog_api.hash_stringw_stringw_useStringW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const Ascii8& key, const StringW& item, bool replace = true)
  {
    return fog_api.hash_stringw_stringw_putStubA(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StubW& key, const StringW& item, bool replace = true)
  {
    return fog_api.hash_stringw_stringw_putStubW(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringW& key, const StringW& item, bool replace = true)
  {
    return fog_api.hash_stringw_stringw_putStringW(this, &key, &item, replace);
  }

  FOG_INLINE err_t remove(const Ascii8& key)
  {
    return fog_api.hash_stringw_stringw_removeStubA(this, &key);
  }

  FOG_INLINE err_t remove(const StubW& key)
  {
    return fog_api.hash_stringw_stringw_removeStubW(this, &key);
  }

  FOG_INLINE err_t remove(const StringW& key)
  {
    return fog_api.hash_stringw_stringw_removeStringW(this, &key);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringW, StringW>& other) const
  {
    return fog_api.hash_stringw_unknown_eq(this, &other, HashVTable<StringW, StringW>::getVTable(), StringW::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringW, StringW>& operator=(const Hash<StringW, StringW>& other)
  {
    fog_api.hash_unknown_unknown_copy(this, HashVTable<StringW, StringW>::getVTable(), &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Hash<StringW, StringW>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Hash<StringW, StringW>& other) const { return !eq(other); }
};

// ============================================================================
// [Fog::Hash<StringW, Var>]
// ============================================================================

template<>
struct Hash<StringW, Var> : public HashUntyped
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Hash()
  {
    fog_api.hash_unknown_unknown_ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringW, Var>& other)
  {
    fog_api.hash_unknown_unknown_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Hash(Hash<StringW, Var>&& other) { HashUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE ~Hash()
  {
    fog_api.hash_unknown_unknown_dtor(this, HashVTable<StringW, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  using HashUntyped::getReference;
  using HashUntyped::isDetached;

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _detach();
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.hash_unknown_unknown_detach(this, HashVTable<StringW, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.hash_unknown_unknown_reserve(this, HashVTable<StringW, Var>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.hash_unknown_unknown_squeeze(this, HashVTable<StringW, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.hash_unknown_unknown_clear(this, HashVTable<StringW, Var>::getVTable());
  }

  FOG_INLINE void reset()
  {
    fog_api.hash_unknown_unknown_reset(this, HashVTable<StringW, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const Ascii8& key) const
  {
    return fog_api.hash_stringw_stringw_getStubA(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StubW& key) const
  {
    return fog_api.hash_stringw_stringw_getStubW(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StringW& key) const
  {
    return fog_api.hash_stringw_stringw_getStringW(this, &key) != NULL;
  }

  FOG_INLINE const Var& get(const Ascii8& key, const Var& notFound) const
  {
    const Var* item = fog_api.hash_stringw_var_getStubA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var& get(const StubW& key, const Var& notFound) const
  {
    const Var* item = fog_api.hash_stringw_var_getStubW(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var& get(const StringW& key, const Var& notFound) const
  {
    const Var* item = fog_api.hash_stringw_var_getStringW(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const Ascii8& key) const
  {
    const Var* item = fog_api.hash_stringw_var_getStubA(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const Ascii8& key, const Var* notFound) const
  {
    const Var* item = fog_api.hash_stringw_var_getStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const StubW& key) const
  {
    const Var* item = fog_api.hash_stringw_var_getStubW(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const StubW& key, const Var* notFound) const
  {
    const Var* item = fog_api.hash_stringw_var_getStubW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const StringW& key) const
  {
    const Var* item = fog_api.hash_stringw_var_getStringW(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const StringW& key, const Var* notFound) const
  {
    const Var* item = fog_api.hash_stringw_var_getStringW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const Ascii8& key)
  {
    Var* item = fog_api.hash_stringw_var_useStubA(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const Ascii8& key, Var* notFound)
  {
    Var* item = fog_api.hash_stringw_var_useStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const StubW& key)
  {
    Var* item = fog_api.hash_stringw_var_useStubW(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const StubW& key, Var* notFound)
  {
    Var* item = fog_api.hash_stringw_var_useStubW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const StringW& key)
  {
    Var* item = fog_api.hash_stringw_var_useStringW(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const StringW& key, Var* notFound)
  {
    Var* item = fog_api.hash_stringw_var_useStringW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const Ascii8& key, const Var& item, bool replace = true)
  {
    return fog_api.hash_stringw_var_putStubA(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StubW& key, const Var& item, bool replace = true)
  {
    return fog_api.hash_stringw_var_putStubW(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringW& key, const Var& item, bool replace = true)
  {
    return fog_api.hash_stringw_var_putStringW(this, &key, &item, replace);
  }

  FOG_INLINE err_t remove(const Ascii8& key)
  {
    return fog_api.hash_stringw_var_removeStubA(this, &key);
  }

  FOG_INLINE err_t remove(const StubW& key)
  {
    return fog_api.hash_stringw_var_removeStubW(this, &key);
  }

  FOG_INLINE err_t remove(const StringW& key)
  {
    return fog_api.hash_stringw_var_removeStringW(this, &key);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringW, Var>& other) const
  {
    return fog_api.hash_stringw_unknown_eq(this, &other, HashVTable<StringW, Var>::getVTable(), (EqFunc)fog_api.var_eq);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringW, Var>& operator=(const Hash<StringW, Var>& other)
  {
    fog_api.hash_unknown_unknown_copy(this, HashVTable<StringW, Var>::getVTable(), &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Hash<StringW, Var>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Hash<StringW, Var>& other) const { return !eq(other); }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_HASHSTRING_H
