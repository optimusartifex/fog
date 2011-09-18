// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_HASHUNTYPED_H
#define _FOG_CORE_TOOLS_HASHUNTYPED_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemPool.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::HashUntypedVTable]
// ============================================================================

struct HashUntypedVTable
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  typedef HashUntypedNode* (FOG_CDECL* Ctor)(HashUntypedNode* node, const void* key, const void* item);
  typedef void (FOG_CDECL* Dtor)(HashUntypedNode* node);
  typedef void (FOG_CDECL* SetItem)(void* dst, const void* src);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t idxKeyT;
  uint32_t idxItemT;

  uint32_t szKeyT;
  uint32_t szItemT;

  Ctor ctor;
  Dtor dtor;
  SetItem setItem;

  HashFunc hashKey;
  EqFunc eqKey;
};

// ============================================================================
// [Fog::HashUntypedNode]
// ============================================================================

struct FOG_NO_EXPORT HashUntypedNode
{
  HashUntypedNode* next;
};

// ============================================================================
// [Fog::HashKeyNode]
// ============================================================================

template<typename KeyT>
struct HashKeyNode : HashUntypedNode
{
  KeyT key;
};

// ============================================================================
// [Fog::HashUntypedData]
// ============================================================================

struct FOG_NO_EXPORT HashUntypedData
{
  // --------------------------------------------------------------------------
  // [AddRef]
  // --------------------------------------------------------------------------

  FOG_INLINE HashUntypedData* addRef() const
  {
    reference.inc();
    return const_cast<HashUntypedData*>(this);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t capacity)
  {
    return sizeof(HashUntypedData) + capacity * sizeof(void*);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible to the VarData header in the second
  // form called - "implicitly shared container". The members must be binary
  // compatible to the header below:
  //
  // +==============+============+============================================+
  // | Size         | Name       | Description / Purpose                      |
  // +==============+============+============================================+
  // | size_t       | reference  | Atomic reference count, can be managed by  |
  // |              |            | VarData without calling container specific |
  // |              |            | methods.                                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | vType      | Variable type and flags.                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | padding0_32| Not used by the Var. This member is only   |
  // |              |            | defined for 64-bit compilation to pad      |
  // |              |            | other members!                             |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | capacity   | Capacity of the container (items).         |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | length     | Length of the container (items).           |
  // +==============+============+============================================+
  //
  // List of possible variable types:
  //
  // +===========================+============================================+
  // | VAR_TYPE_...              | Description / Purpose                      |
  // +===========================+============================================+
  // | UNKNOWN                   | Hash<?, ?>, where the type is unknown or   |
  // |                           | not supported by Var. Instance of this list|
  // |                           | can't be casted to the Hash<StringA, Var>, |
  // |                           | Hash<StringW, Var>, or Var.                |
  // +---------------------------+--------------------------------------------+
  // | HASH_STRINGA_STRINGA      | Hash<StringA, StringA>, typed list         |
  // |                           | compatible to Hash<StringA, Var> and Var.  |
  // +---------------------------+--------------------------------------------+
  // | HASH_STRINGA_VAR          | Hash<StringA, Var>, typed list compatible  |
  // |                           | to Var.                                    |
  // +---------------------------+--------------------------------------------+
  // | HASH_STRINGW_STRINGW      | Hash<StringW, StringW>, typed list         |
  // |                           | compatible to Hash<StringW, Var> and Var.  |
  // +---------------------------+--------------------------------------------+
  // | HASH_STRINGW_VAR          | Hash<StringW, Var>, typed list compatible  |
  // |                           | to Var.                                    |
  // +===========================+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  //! @brief Padding (0.32).
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Capacity.
  size_t capacity;
  //! @brief Length.
  size_t length;

  //! @brief Count of buckets we will expand to if length becomes greater than 
  //! @c _expandLength.
  size_t expandCapacity;
  //! @brief Count of nodes to grow.
  size_t expandLength;

  //! @brief Count of buckets we will shrink to if length becomes less than
  //! @c _shinkLength.
  size_t shrinkCapacity;
  //! @brief Count of nodes to shrink.
  size_t shrinkLength;

  //! @brief @c HashUntypedNode allocator.
  MemPool nodePool;

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  //! @brief Nodes.
  HashUntypedNode* data[1];
};

// ============================================================================
// [Fog::HashUntyped]
// ============================================================================

struct FOG_NO_EXPORT HashUntyped
{
  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const
  {
    return _d->reference.get();
  }

  FOG_INLINE bool isDetached() const
  {
    return _d->reference.get() == 1;
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  FOG_INLINE size_t getLength() const { return _d->length; }
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE HashUntypedNode* const* _getUntypedData() const
  {
    return _d->data;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(HashUntypedData)
};

// ============================================================================
// [Fog::HashUntypedIterator]
// ============================================================================

struct FOG_NO_EXPORT HashUntypedIterator
{
  HashUntyped* _container;
  HashUntypedNode* _node;

  size_t _index;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_HASHUNTYPED_H
