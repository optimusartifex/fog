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
    return _api.hash.unknown_unknown.detach(this, HashVTable<StringA, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.hash.unknown_unknown.reserve(this, HashVTable<StringA, ItemT>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.hash.unknown_unknown.squeeze(this, HashVTable<StringA, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.hash.unknown_unknown.clear(this, HashVTable<StringA, ItemT>::getVTable());
  }

  FOG_INLINE void reset()
  {
    _api.hash.unknown_unknown.reset(this, HashVTable<StringA, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const StubA& key) const
  {
    return _api.hash.stringa_unknown.getStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE bool contains(const StringA& key) const
  {
    return _api.hash.stringa_unknown.getStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE const ItemT& get(const StubA& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringa_unknown.getStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT& get(const StringA& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringa_unknown.getStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const StubA& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringa_unknown.getStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const StubA& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringa_unknown.getStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const StringA& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringa_unknown.getStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const StringA& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringa_unknown.getStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const StubA& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringa_unknown.useStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const StubA& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringa_unknown.useStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const StringA& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringa_unknown.useStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const StringA& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringa_unknown.useStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const StubA& key, const ItemT& item, bool replace = true)
  {
    return _api.hash.stringa_unknown.putStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringA& key, const ItemT& item, bool replace = true)
  {
    return _api.hash.stringa_unknown.putStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t remove(const StubA& key)
  {
    return _api.hash.stringa_unknown.removeStubA(this, HashVTable<StringA, ItemT>::getVTable(), &key);
  }

  FOG_INLINE err_t remove(const StringA& key)
  {
    return _api.hash.stringa_unknown.removeStringA(this, HashVTable<StringA, ItemT>::getVTable(), &key);
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
    return _api.hash.stringa_unknown.eq(this, &other,
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
    _api.hash.unknown_unknown.ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringA, ItemT>& other)
  {
    _api.hash.unknown_unknown.ctorCopy(this, &other);
  }

  FOG_INLINE ~Hash()
  {
    _api.hash.unknown_unknown.dtor(this, HashVTable<StringA, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringA, ItemT>& operator=(const Hash<StringA, ItemT>& other)
  {
    _api.hash.unknown_unknown.copy(this, HashVTable<StringA, ItemT>::getVTable(), &other);
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
    _api.hash.unknown_unknown.ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringA, StringA>& other)
  {
    _api.hash.unknown_unknown.ctorCopy(this, &other);
  }

  FOG_INLINE ~Hash()
  {
    _api.hash.unknown_unknown.dtor(this, HashVTable<StringA, StringA>::getVTable());
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
    return _api.hash.unknown_unknown.detach(this, HashVTable<StringA, StringA>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.hash.unknown_unknown.reserve(this, HashVTable<StringA, StringA>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.hash.unknown_unknown.squeeze(this, HashVTable<StringA, StringA>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.hash.unknown_unknown.clear(this, HashVTable<StringA, StringA>::getVTable());
  }

  FOG_INLINE void reset()
  {
    _api.hash.unknown_unknown.reset(this, HashVTable<StringA, StringA>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const StubA& key) const
  {
    return _api.hash.stringa_stringa.getStubA(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StringA& key) const
  {
    return _api.hash.stringa_stringa.getStringA(this, &key) != NULL;
  }

  FOG_INLINE const StringA& get(const StubA& key, const StringA& notFound) const
  {
    const StringA* item = _api.hash.stringa_stringa.getStubA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringA& get(const StringA& key, const StringA& notFound) const
  {
    const StringA* item = _api.hash.stringa_stringa.getStringA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringA* getPtr(const StubA& key) const
  {
    const StringA* item = _api.hash.stringa_stringa.getStubA(this, &key);

    return item;
  }

  FOG_INLINE const StringA* getPtr(const StubA& key, const StringA* notFound) const
  {
    const StringA* item = _api.hash.stringa_stringa.getStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const StringA* getPtr(const StringA& key) const
  {
    const StringA* item = _api.hash.stringa_stringa.getStringA(this, &key);

    return item;
  }

  FOG_INLINE const StringA* getPtr(const StringA& key, const StringA* notFound) const
  {
    const StringA* item = _api.hash.stringa_stringa.getStringA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringA* usePtr(const StubA& key)
  {
    StringA* item = _api.hash.stringa_stringa.useStubA(this, &key);

    return item;
  }

  FOG_INLINE StringA* usePtr(const StubA& key, StringA* notFound)
  {
    StringA* item = _api.hash.stringa_stringa.useStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringA* usePtr(const StringA& key)
  {
    StringA* item = _api.hash.stringa_stringa.useStringA(this, &key);

    return item;
  }

  FOG_INLINE StringA* usePtr(const StringA& key, StringA* notFound)
  {
    StringA* item = _api.hash.stringa_stringa.useStringA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const StubA& key, const StringA& item, bool replace = true)
  {
    return _api.hash.stringa_stringa.putStubA(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringA& key, const StringA& item, bool replace = true)
  {
    return _api.hash.stringa_stringa.putStringA(this, &key, &item, replace);
  }

  FOG_INLINE err_t remove(const StubA& key)
  {
    return _api.hash.stringa_stringa.removeStubA(this, &key);
  }

  FOG_INLINE err_t remove(const StringA& key)
  {
    return _api.hash.stringa_stringa.removeStringA(this, &key);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringA, StringA>& other) const
  {
    return _api.hash.stringa_unknown.eq(this, &other, HashVTable<StringA, StringA>::getVTable(), StringA::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringA, StringA>& operator=(const Hash<StringA, StringA>& other)
  {
    _api.hash.unknown_unknown.copy(this, HashVTable<StringA, StringA>::getVTable(), &other);
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
    _api.hash.unknown_unknown.ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringA, Var>& other)
  {
    _api.hash.unknown_unknown.ctorCopy(this, &other);
  }

  FOG_INLINE ~Hash()
  {
    _api.hash.unknown_unknown.dtor(this, HashVTable<StringA, Var>::getVTable());
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
    return _api.hash.unknown_unknown.detach(this, HashVTable<StringA, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.hash.unknown_unknown.reserve(this, HashVTable<StringA, Var>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.hash.unknown_unknown.squeeze(this, HashVTable<StringA, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.hash.unknown_unknown.clear(this, HashVTable<StringA, Var>::getVTable());
  }

  FOG_INLINE void reset()
  {
    _api.hash.unknown_unknown.reset(this, HashVTable<StringA, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const StubA& key) const
  {
    return _api.hash.stringa_stringa.getStubA(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StringA& key) const
  {
    return _api.hash.stringa_stringa.getStringA(this, &key) != NULL;
  }

  FOG_INLINE const Var& get(const StubA& key, const Var& notFound) const
  {
    const Var* item = _api.hash.stringa_var.getStubA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var& get(const StringA& key, const Var& notFound) const
  {
    const Var* item = _api.hash.stringa_var.getStringA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const StubA& key) const
  {
    const Var* item = _api.hash.stringa_var.getStubA(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const StubA& key, const Var* notFound) const
  {
    const Var* item = _api.hash.stringa_var.getStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const StringA& key) const
  {
    const Var* item = _api.hash.stringa_var.getStringA(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const StringA& key, const Var* notFound) const
  {
    const Var* item = _api.hash.stringa_var.getStringA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const StubA& key)
  {
    Var* item = _api.hash.stringa_var.useStubA(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const StubA& key, Var* notFound)
  {
    Var* item = _api.hash.stringa_var.useStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const StringA& key)
  {
    Var* item = _api.hash.stringa_var.useStringA(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const StringA& key, Var* notFound)
  {
    Var* item = _api.hash.stringa_var.useStringA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const StubA& key, const Var& item, bool replace = true)
  {
    return _api.hash.stringa_var.putStubA(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringA& key, const Var& item, bool replace = true)
  {
    return _api.hash.stringa_var.putStringA(this, &key, &item, replace);
  }

  FOG_INLINE err_t remove(const StubA& key)
  {
    return _api.hash.stringa_var.removeStubA(this, &key);
  }

  FOG_INLINE err_t remove(const StringA& key)
  {
    return _api.hash.stringa_var.removeStringA(this, &key);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringA, Var>& other) const
  {
    return _api.hash.stringa_unknown.eq(this, &other, HashVTable<StringA, Var>::getVTable(), (EqFunc)_api.var.eq);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringA, Var>& operator=(const Hash<StringA, Var>& other)
  {
    _api.hash.unknown_unknown.copy(this, HashVTable<StringA, Var>::getVTable(), &other);
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
    return _api.hash.unknown_unknown.detach(this, HashVTable<StringW, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.hash.unknown_unknown.reserve(this, HashVTable<StringW, ItemT>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.hash.unknown_unknown.squeeze(this, HashVTable<StringW, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.hash.unknown_unknown.clear(this, HashVTable<StringW, ItemT>::getVTable());
  }

  FOG_INLINE void reset()
  {
    _api.hash.unknown_unknown.reset(this, HashVTable<StringW, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const Ascii8& key) const
  {
    return _api.hash.stringw_unknown.getStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE bool contains(const StubW& key) const
  {
    return _api.hash.stringw_unknown.getStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE bool contains(const StringW& key) const
  {
    return _api.hash.stringw_unknown.getStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE const ItemT& get(const Ascii8& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringw_unknown.getStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT& get(const StubW& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringw_unknown.getStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT& get(const StringW& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringw_unknown.getStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const Ascii8& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringw_unknown.getStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const Ascii8& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringw_unknown.getStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const StubW& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringw_unknown.getStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const StubW& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringw_unknown.getStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const StringW& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringw_unknown.getStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const StringW& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      _api.hash.stringw_unknown.getStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const Ascii8& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringw_unknown.useStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const Ascii8& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringw_unknown.useStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const StubW& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringw_unknown.useStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const StubW& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringw_unknown.useStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const StringW& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringw_unknown.useStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const StringW& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      _api.hash.stringw_unknown.useStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const Ascii8& key, const ItemT& item, bool replace = true)
  {
    return _api.hash.stringw_unknown.putStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t put(const StubW& key, const ItemT& item, bool replace = true)
  {
    return _api.hash.stringw_unknown.putStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringW& key, const ItemT& item, bool replace = true)
  {
    return _api.hash.stringw_unknown.putStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t remove(const Ascii8& key)
  {
    return _api.hash.stringw_unknown.removeStubA(this, HashVTable<StringW, ItemT>::getVTable(), &key);
  }

  FOG_INLINE err_t remove(const StubW& key)
  {
    return _api.hash.stringw_unknown.removeStubW(this, HashVTable<StringW, ItemT>::getVTable(), &key);
  }

  FOG_INLINE err_t remove(const StringW& key)
  {
    return _api.hash.stringw_unknown.removeStringW(this, HashVTable<StringW, ItemT>::getVTable(), &key);
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
    return _api.hash.stringw_unknown.eq(this, &other,
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
    _api.hash.unknown_unknown.ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringW, ItemT>& other)
  {
    _api.hash.unknown_unknown.ctorCopy(this, &other);
  }

  FOG_INLINE ~Hash()
  {
    _api.hash.unknown_unknown.dtor(this, HashVTable<StringW, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringW, ItemT>& operator=(const Hash<StringW, ItemT>& other)
  {
    _api.hash.unknown_unknown.copy(this, HashVTable<StringW, ItemT>::getVTable(), &other);
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
    _api.hash.unknown_unknown.ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringW, StringW>& other)
  {
    _api.hash.unknown_unknown.ctorCopy(this, &other);
  }

  FOG_INLINE ~Hash()
  {
    _api.hash.unknown_unknown.dtor(this, HashVTable<StringW, StringW>::getVTable());
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
    return _api.hash.unknown_unknown.detach(this, HashVTable<StringW, StringW>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.hash.unknown_unknown.reserve(this, HashVTable<StringW, StringW>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.hash.unknown_unknown.squeeze(this, HashVTable<StringW, StringW>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.hash.unknown_unknown.clear(this, HashVTable<StringW, StringW>::getVTable());
  }

  FOG_INLINE void reset()
  {
    _api.hash.unknown_unknown.reset(this, HashVTable<StringW, StringW>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const Ascii8& key) const
  {
    return _api.hash.stringw_stringw.getStubA(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StubW& key) const
  {
    return _api.hash.stringw_stringw.getStubW(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StringW& key) const
  {
    return _api.hash.stringw_stringw.getStringW(this, &key) != NULL;
  }

  FOG_INLINE const StringW& get(const Ascii8& key, const StringW& notFound) const
  {
    const StringW* item = _api.hash.stringw_stringw.getStubA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringW& get(const StubW& key, const StringW& notFound) const
  {
    const StringW* item = _api.hash.stringw_stringw.getStubW(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringW& get(const StringW& key, const StringW& notFound) const
  {
    const StringW* item = _api.hash.stringw_stringw.getStringW(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const StringW* getPtr(const Ascii8& key) const
  {
    const StringW* item = _api.hash.stringw_stringw.getStubA(this, &key);

    return item;
  }

  FOG_INLINE const StringW* getPtr(const Ascii8& key, const StringW* notFound) const
  {
    const StringW* item = _api.hash.stringw_stringw.getStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const StringW* getPtr(const StubW& key) const
  {
    const StringW* item = _api.hash.stringw_stringw.getStubW(this, &key);

    return item;
  }

  FOG_INLINE const StringW* getPtr(const StubW& key, const StringW* notFound) const
  {
    const StringW* item = _api.hash.stringw_stringw.getStubW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const StringW* getPtr(const StringW& key) const
  {
    const StringW* item = _api.hash.stringw_stringw.getStringW(this, &key);

    return item;
  }

  FOG_INLINE const StringW* getPtr(const StringW& key, const StringW* notFound) const
  {
    const StringW* item = _api.hash.stringw_stringw.getStringW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringW* usePtr(const Ascii8& key)
  {
    StringW* item = _api.hash.stringw_stringw.useStubA(this, &key);

    return item;
  }

  FOG_INLINE StringW* usePtr(const Ascii8& key, StringW* notFound)
  {
    StringW* item = _api.hash.stringw_stringw.useStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringW* usePtr(const StubW& key)
  {
    StringW* item = _api.hash.stringw_stringw.useStubW(this, &key);

    return item;
  }

  FOG_INLINE StringW* usePtr(const StubW& key, StringW* notFound)
  {
    StringW* item = _api.hash.stringw_stringw.useStubW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE StringW* usePtr(const StringW& key)
  {
    StringW* item = _api.hash.stringw_stringw.useStringW(this, &key);

    return item;
  }

  FOG_INLINE StringW* usePtr(const StringW& key, StringW* notFound)
  {
    StringW* item = _api.hash.stringw_stringw.useStringW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const Ascii8& key, const StringW& item, bool replace = true)
  {
    return _api.hash.stringw_stringw.putStubA(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StubW& key, const StringW& item, bool replace = true)
  {
    return _api.hash.stringw_stringw.putStubW(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringW& key, const StringW& item, bool replace = true)
  {
    return _api.hash.stringw_stringw.putStringW(this, &key, &item, replace);
  }

  FOG_INLINE err_t remove(const Ascii8& key)
  {
    return _api.hash.stringw_stringw.removeStubA(this, &key);
  }

  FOG_INLINE err_t remove(const StubW& key)
  {
    return _api.hash.stringw_stringw.removeStubW(this, &key);
  }

  FOG_INLINE err_t remove(const StringW& key)
  {
    return _api.hash.stringw_stringw.removeStringW(this, &key);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringW, StringW>& other) const
  {
    return _api.hash.stringw_unknown.eq(this, &other, HashVTable<StringW, StringW>::getVTable(), StringW::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringW, StringW>& operator=(const Hash<StringW, StringW>& other)
  {
    _api.hash.unknown_unknown.copy(this, HashVTable<StringW, StringW>::getVTable(), &other);
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
    _api.hash.unknown_unknown.ctor(this);
  }

  FOG_INLINE Hash(const Hash<StringW, Var>& other)
  {
    _api.hash.unknown_unknown.ctorCopy(this, &other);
  }

  FOG_INLINE ~Hash()
  {
    _api.hash.unknown_unknown.dtor(this, HashVTable<StringW, Var>::getVTable());
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
    return _api.hash.unknown_unknown.detach(this, HashVTable<StringW, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.hash.unknown_unknown.reserve(this, HashVTable<StringW, Var>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.hash.unknown_unknown.squeeze(this, HashVTable<StringW, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.hash.unknown_unknown.clear(this, HashVTable<StringW, Var>::getVTable());
  }

  FOG_INLINE void reset()
  {
    _api.hash.unknown_unknown.reset(this, HashVTable<StringW, Var>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const Ascii8& key) const
  {
    return _api.hash.stringw_stringw.getStubA(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StubW& key) const
  {
    return _api.hash.stringw_stringw.getStubW(this, &key) != NULL;
  }

  FOG_INLINE bool contains(const StringW& key) const
  {
    return _api.hash.stringw_stringw.getStringW(this, &key) != NULL;
  }

  FOG_INLINE const Var& get(const Ascii8& key, const Var& notFound) const
  {
    const Var* item = _api.hash.stringw_var.getStubA(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var& get(const StubW& key, const Var& notFound) const
  {
    const Var* item = _api.hash.stringw_var.getStubW(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var& get(const StringW& key, const Var& notFound) const
  {
    const Var* item = _api.hash.stringw_var.getStringW(this, &key);

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const Ascii8& key) const
  {
    const Var* item = _api.hash.stringw_var.getStubA(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const Ascii8& key, const Var* notFound) const
  {
    const Var* item = _api.hash.stringw_var.getStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const StubW& key) const
  {
    const Var* item = _api.hash.stringw_var.getStubW(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const StubW& key, const Var* notFound) const
  {
    const Var* item = _api.hash.stringw_var.getStubW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE const Var* getPtr(const StringW& key) const
  {
    const Var* item = _api.hash.stringw_var.getStringW(this, &key);

    return item;
  }

  FOG_INLINE const Var* getPtr(const StringW& key, const Var* notFound) const
  {
    const Var* item = _api.hash.stringw_var.getStringW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const Ascii8& key)
  {
    Var* item = _api.hash.stringw_var.useStubA(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const Ascii8& key, Var* notFound)
  {
    Var* item = _api.hash.stringw_var.useStubA(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const StubW& key)
  {
    Var* item = _api.hash.stringw_var.useStubW(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const StubW& key, Var* notFound)
  {
    Var* item = _api.hash.stringw_var.useStubW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE Var* usePtr(const StringW& key)
  {
    Var* item = _api.hash.stringw_var.useStringW(this, &key);

    return item;
  }

  FOG_INLINE Var* usePtr(const StringW& key, Var* notFound)
  {
    Var* item = _api.hash.stringw_var.useStringW(this, &key);

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const Ascii8& key, const Var& item, bool replace = true)
  {
    return _api.hash.stringw_var.putStubA(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StubW& key, const Var& item, bool replace = true)
  {
    return _api.hash.stringw_var.putStubW(this, &key, &item, replace);
  }

  FOG_INLINE err_t put(const StringW& key, const Var& item, bool replace = true)
  {
    return _api.hash.stringw_var.putStringW(this, &key, &item, replace);
  }

  FOG_INLINE err_t remove(const Ascii8& key)
  {
    return _api.hash.stringw_var.removeStubA(this, &key);
  }

  FOG_INLINE err_t remove(const StubW& key)
  {
    return _api.hash.stringw_var.removeStubW(this, &key);
  }

  FOG_INLINE err_t remove(const StringW& key)
  {
    return _api.hash.stringw_var.removeStringW(this, &key);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<StringW, Var>& other) const
  {
    return _api.hash.stringw_unknown.eq(this, &other, HashVTable<StringW, Var>::getVTable(), (EqFunc)_api.var.eq);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<StringW, Var>& operator=(const Hash<StringW, Var>& other)
  {
    _api.hash.unknown_unknown.copy(this, HashVTable<StringW, Var>::getVTable(), &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Hash<StringW, Var>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Hash<StringW, Var>& other) const { return !eq(other); }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_HASHSTRING_H
