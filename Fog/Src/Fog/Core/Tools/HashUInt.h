// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_HASHUINT_H
#define _FOG_CORE_TOOLS_HASHUINT_H

// [Dependencies]
#include <Fog/Core/Tools/Hash.h>

namespace Fog {
  
//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::HashUInt32Impl<Unknown>]
// ============================================================================

template<typename KeyT, typename ItemT, int IndexableItem>
struct HashUInt32Impl {};

template<typename KeyT, typename ItemT>
struct HashUInt32Impl<KeyT, ItemT, 0> : public HashUntyped
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
    return fog_api.hash_unknown_unknown_detach(this, HashVTable<uint32_t, ItemT>::getVTable());
  }
  
  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------
  
  using HashUntyped::getCapacity;
  using HashUntyped::getLength;
  using HashUntyped::isEmpty;
  
  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.hash_unknown_unknown_reserve(this, HashVTable<uint32_t, ItemT>::getVTable(), capacity);
  }
  
  FOG_INLINE void squeeze()
  {
    fog_api.hash_unknown_unknown_squeeze(this, HashVTable<uint32_t, ItemT>::getVTable());
  }
  
  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------
  
  FOG_INLINE void clear()
  {
    fog_api.hash_unknown_unknown_clear(this, HashVTable<uint32_t, ItemT>::getVTable());
  }
  
  FOG_INLINE void reset()
  {
    fog_api.hash_unknown_unknown_reset(this, HashVTable<uint32_t, ItemT>::getVTable());
  }
  
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool contains(const KeyT& key) const
  {
    return fog_api.hash_uint32_unknown_get(this, HashVTable<uint32_t, ItemT>::getVTable(), static_cast<uint32_t>(key)) != NULL;
  }
  
  FOG_INLINE const ItemT& get(const KeyT& key, const ItemT& notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_uint32_unknown_get(this, HashVTable<uint32_t, ItemT>::getVTable(), static_cast<uint32_t>(key)));
    
    if (item != NULL)
      return *item;
    else
      return notFound;
  }
  
  FOG_INLINE const ItemT* getPtr(const KeyT& key) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_uint32_unknown_get(this, HashVTable<uint32_t, ItemT>::getVTable(), static_cast<uint32_t>(key)));
    
    return item;
  }

  FOG_INLINE const ItemT* getPtr(const KeyT& key, const ItemT* notFound) const
  {
    const ItemT* item = reinterpret_cast<const ItemT*>(
      fog_api.hash_uint32_unknown_get(this, HashVTable<uint32_t, ItemT>::getVTable(), static_cast<uint32_t>(key)));

    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE ItemT* usePtr(const KeyT& key)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_uint32_unknown_use(this, HashVTable<uint32_t, ItemT>::getVTable(), static_cast<uint32_t>(key)));
    
    return item;
  }
  
  FOG_INLINE ItemT* usePtr(const KeyT& key, ItemT* notFound)
  {
    ItemT* item = reinterpret_cast<ItemT*>(
      fog_api.hash_uint32_unknown_use(this, HashVTable<uint32_t, ItemT>::getVTable(), static_cast<uint32_t>(key)));
    
    if (item != NULL)
      return item;
    else
      return notFound;
  }

  FOG_INLINE err_t put(const uint32_t& key, const ItemT& item, bool replace = true)
  {
    return fog_api.hash_uint32_unknown_put(this, HashVTable<uint32_t, ItemT>::getVTable(), key, &item, replace);
  }
  
  FOG_INLINE err_t remove(const uint32_t& key)
  {
    return fog_api.hash_uint32_unknown_remove(this, HashVTable<uint32_t, ItemT>::getVTable(), key);
  }
};

template<typename KeyT, typename ItemT>
struct HashUInt32Impl<KeyT, ItemT, 1> : public HashUInt32Impl<KeyT, ItemT, 0>
{
  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool eq(const Hash<KeyT, ItemT>& other) const
  {
    return fog_api.hash_unknown_unknown_eq(this, &other, HashVTable<uint32_t, ItemT>::getVTable(), TypeFunc<ItemT>::getEqFunc());
  }
  
  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool operator==(const Hash<KeyT, ItemT>& other) { return  eq(other); }
  FOG_INLINE bool operator!=(const Hash<KeyT, ItemT>& other) { return !eq(other); }
};

// ============================================================================
// [Fog::Hash<uint32_t, Unknown>]
// ============================================================================

template<typename ItemT>
struct Hash<uint32_t, ItemT> : public HashUInt32Impl<uint32_t, ItemT, !TypeInfo<ItemT>::NO_EQ>
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------
  
  FOG_INLINE Hash()
  {
    fog_api.hash_unknown_unknown_ctor(this);
  }
  
  FOG_INLINE Hash(const Hash<uint32_t, ItemT>& other)
  {
    fog_api.hash_unknown_unknown_ctorCopy(this, &other);
  }
  
#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Hash(Hash<uint32_t, ItemT>&& other) { HashUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE
  
  FOG_INLINE ~Hash()
  {
    fog_api.hash_unknown_unknown_dtor(this, HashVTable<uint32_t, ItemT>::getVTable());
  }
  
  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------
  
  FOG_INLINE const Hash<uint32_t, ItemT>& operator=(const Hash<uint32_t, ItemT>& other)
  {
    fog_api.hash_unknown_unknown_copy(this, HashVTable<uint32_t, ItemT>::getVTable(), &other);
    return *this;
  }
};

// ============================================================================
// [Fog::Hash<int32_t, Unknown>]
// ============================================================================

template<typename ItemT>
struct Hash<int32_t, ItemT> : public HashUInt32Impl<int32_t, ItemT, !TypeInfo<ItemT>::NO_EQ>
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------
  
  FOG_INLINE Hash()
  {
    fog_api.hash_unknown_unknown_ctor(this);
  }
  
  FOG_INLINE Hash(const Hash<int32_t, ItemT>& other)
  {
    fog_api.hash_unknown_unknown_ctorCopy(this, &other);
  }
  
#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Hash(Hash<int32_t, ItemT>&& other) { HashUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE
  
  FOG_INLINE ~Hash()
  {
    fog_api.hash_unknown_unknown_dtor(this, HashVTable<int32_t, ItemT>::getVTable());
  }
  
  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------
  
  FOG_INLINE const Hash<int32_t, ItemT>& operator=(const Hash<int32_t, ItemT>& other)
  {
    fog_api.hash_unknown_unknown_copy(this, HashVTable<int32_t, ItemT>::getVTable(), &other);
    return *this;
  }
};

//! @}
  
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_HASHUINT_H
