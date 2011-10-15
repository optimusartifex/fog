// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_HASH_H
#define _FOG_CORE_TOOLS_HASH_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/HashUntyped.h>
#include <Fog/Core/Tools/HashUtil.h>
#include <Fog/Core/Tools/List.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::HashNode<KeyT, ItemT>]
// ============================================================================

template<typename KeyT, typename ItemT>
struct HashNode : public HashUntypedNode
{
  KeyT key;
  ItemT item;
};

// ============================================================================
// [Fog::HashVTableT<KeyT, ItemT>]
// ============================================================================

template<typename KeyT, typename ItemT>
struct HashVTable
{
  typedef HashNode<KeyT, ItemT> NodeType;

  static const HashUntypedVTable vtable;
  static FOG_INLINE const HashUntypedVTable* getVTable() { return &vtable; }

  static HashUntypedNode* FOG_CDECL _ctor(HashUntypedNode* _node, const void* key, const void* item)
  {
    HashNode<KeyT, ItemT>* node = reinterpret_cast< HashNode<KeyT, ItemT>* >(_node);

    fog_new_p(&node->key) KeyT(*reinterpret_cast<const KeyT*>(key));
    fog_new_p(&node->item) ItemT(*reinterpret_cast<const ItemT*>(item));

    return _node;
  }

  static void FOG_CDECL _dtor(HashUntypedNode* _node)
  {
    HashNode<KeyT, ItemT>* node = reinterpret_cast< HashNode<KeyT, ItemT>* >(_node);

    node->key.~KeyT();
    node->item.~ItemT();
  }

  static void FOG_CDECL _setItem(void* dst, const void* src)
  {
    *reinterpret_cast<ItemT*>(dst) = *reinterpret_cast<const ItemT*>(src);
  }

  static uint32_t FOG_CDECL _hashKey(const void* key)
  {
    return HashUtil::hash<KeyT>(*reinterpret_cast<const KeyT*>(key));
  }
};

template<typename KeyT, typename ItemT>
const HashUntypedVTable HashVTable<KeyT, ItemT>::vtable =
{
  (uint32_t)FOG_OFFSET_OF(NodeType, key),
  (uint32_t)FOG_OFFSET_OF(NodeType, item),

  (uint32_t)sizeof(KeyT),
  (uint32_t)sizeof(ItemT),

  HashVTable<KeyT, ItemT>::_ctor,
  HashVTable<KeyT, ItemT>::_dtor,
  HashVTable<KeyT, ItemT>::_setItem,
  HashVTable<KeyT, ItemT>::_hashKey,
  TypeFunc<KeyT>::getEqFunc()
};

template<>
struct HashVTable<StringA, StringA>
{
  static FOG_INLINE const HashUntypedVTable* getVTable() { return fog_api.hash_stringa_stringa_vTable; }
};

template<>
struct HashVTable<StringA, Var>
{
  static FOG_INLINE const HashUntypedVTable* getVTable() { return fog_api.hash_stringa_var_vTable; }
};

template<>
struct HashVTable<StringW, StringW>
{
  static FOG_INLINE const HashUntypedVTable* getVTable() { return fog_api.hash_stringw_stringw_vTable; }
};

template<>
struct HashVTable<StringW, Var>
{
  static FOG_INLINE const HashUntypedVTable* getVTable() { return fog_api.hash_stringw_var_vTable; }
};

// ============================================================================
// [Fog::HashImpl<Unknown, Unknown>]
// ============================================================================

template<typename KeyT, typename ItemT, int IndexableItem>
struct HashImpl {};

template<typename KeyT, typename ItemT>
struct HashImpl<KeyT, ItemT, 0> : public HashUntyped
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE void _ctor()
  {
    fog_api.hash_unknown_unknown_ctor(this);
  }

  FOG_INLINE void _ctorCopy(const HashUntyped& other)
  {
    fog_api.hash_unknown_unknown_ctorCopy(this, &other);
  }

  FOG_INLINE void _dtor()
  {
    fog_api.hash_unknown_unknown_dtor(this, HashVTable<KeyT, ItemT>::getVTable());
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
    return fog_api.hash_unknown_unknown_detach(this, HashVTable<KeyT, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.hash_unknown_unknown_reserve(this, HashVTable<KeyT, ItemT>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.hash_unknown_unknown_squeeze(this, HashVTable<KeyT, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.hash_unknown_unknown_clear(this, HashVTable<KeyT, ItemT>::getVTable());
  }

  FOG_INLINE void reset()
  {
    fog_api.hash_unknown_unknown_reset(this, HashVTable<KeyT, ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const KeyT& key) const
  {
    return fog_api.hash_unknown_unknown_get(this, HashVTable<KeyT, ItemT>::getVTable(), &key) != NULL;
  }

  FOG_INLINE const ItemT& get(const KeyT& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_unknown_unknown_get(this, HashVTable<KeyT, ItemT>::getVTable(), &key));

    if (item != NULL)
      return *item;
    else
      return notFound;
  }

  FOG_INLINE const ItemT* getPtr(const KeyT& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_unknown_unknown_get(this, HashVTable<KeyT, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE const ItemT* getPtr(const KeyT& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_unknown_unknown_get(this, HashVTable<KeyT, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const KeyT& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_unknown_unknown_use(this, HashVTable<KeyT, ItemT>::getVTable(), &key));

    return item;
  }

  FOG_INLINE ItemT* usePtr(const KeyT& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_unknown_unknown_use(this, HashVTable<KeyT, ItemT>::getVTable(), &key));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const KeyT& key, const ItemT& item, bool replace = true)
  {
    return fog_api.hash_unknown_unknown_put(this, HashVTable<KeyT, ItemT>::getVTable(), &key, &item, replace);
  }

  FOG_INLINE err_t remove(const KeyT& key)
  {
    return fog_api.hash_unknown_unknown_remove(this, HashVTable<KeyT, ItemT>::getVTable(), &key);
  }

  // --------------------------------------------------------------------------
  // [Copy]
  // --------------------------------------------------------------------------

  FOG_INLINE void _copy(const HashUntyped& other)
  {
    fog_api.hash_unknown_unknown_copy(this, HashVTable<KeyT, ItemT>::getVTable(), &other);
  }
};

template<typename KeyT, typename ItemT>
struct HashImpl<KeyT, ItemT, 1> : public HashImpl<KeyT, ItemT, 0>
{
  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Hash<KeyT, ItemT>& other) const
  {
    return fog_api.hash_unknown_unknown_eq(this, &other,
      HashVTable<KeyT, ItemT>::getVTable(), TypeFunc<ItemT>::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator==(const Hash<KeyT, ItemT>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Hash<KeyT, ItemT>& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const Hash<KeyT, ItemT>* a, const Hash<KeyT, ItemT>* b)
  {
    return fog_api.hash_unknown_unknown_eq(a, b,
      HashVTable<KeyT, ItemT>::getVTable(), TypeFunc<ItemT>::getEqFunc());
  }
};

// ============================================================================
// [Fog::Hash]
// ============================================================================

template<typename KeyT, typename ItemT>
struct Hash : public HashImpl<KeyT, ItemT, !TypeInfo<ItemT>::NO_EQ>
{
  typedef HashImpl<KeyT, ItemT, 0> Impl;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Hash()
  {
    Impl::_ctor();
  }

  FOG_INLINE Hash(const Hash<KeyT, ItemT>& other)
  {
    Impl::_ctorCopy(other);
  }

  FOG_INLINE ~Hash()
  {
    Impl::_dtor();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<KeyT, ItemT>& operator=(const Hash<KeyT, ItemT>& other)
  {
    Impl::_copy(other);
    return *this;
  }
};

// ============================================================================
// [Fog::HashIterator]
// ============================================================================

template<typename KeyT, typename ItemT>
struct HashIterator : public HashUntypedIterator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new Hash<KeyT, ItemT> iterator.
  FOG_INLINE HashIterator(const Hash<KeyT, ItemT>& container)
  {
    start(&container);
  }

  //! @brief Destroy the Hash<KeyT, ItemT> iterator.
  FOG_INLINE ~HashIterator()
  {
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const KeyT& getKey() const
  {
    FOG_ASSERT_X(isValid(),
      "Fog::HashIterator<?, ?>::getKey() - Iterator is not valid.");

    return reinterpret_cast< HashNode<KeyT, ItemT>*>(_node)->key;
  }

  FOG_INLINE const ItemT& getItem() const
  {
    FOG_ASSERT_X(isValid(),
      "Fog::HashIterator<?, ?>::getItem() - Iterator is not valid.");

    return reinterpret_cast< HashNode<KeyT, ItemT>*>(_node)->item;
  }

  FOG_INLINE bool isValid() const
  {
    return _index != INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [Start / Next]
  // --------------------------------------------------------------------------

  FOG_INLINE bool start(const Hash<KeyT, ItemT>* container)
  {
    _container = (HashUntyped*)container;
    return start();
  }

  FOG_INLINE bool start()
  {
    return fog_api.hashiterator_start(this);
  }

  FOG_INLINE bool next()
  {
    return fog_api.hashiterator_next(this);
  }
};

//! @}

} // Fog namespace

// ============================================================================
// [Include Specialized Containers]
// ============================================================================

#if defined(_FOG_CORE_TOOLS_STRING_H)
# include <Fog/Core/Tools/HashString.h>
#endif // _FOG_CORE_TOOLS_STRING_H

// [Guard]
#endif // _FOG_CORE_TOOLS_HASH_H
