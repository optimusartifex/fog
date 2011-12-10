// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/HashString.h>
#include <Fog/Core/Tools/HashUntyped.h>
#include <Fog/Core/Tools/HashUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/Var.h>

namespace Fog {

// ============================================================================
// [Fog::Hash - Global]
// ============================================================================

static Static<HashUntypedData> Hash_Unknown_Unknown_dEmpty;
static Static<HashUntypedData> Hash_StringA_StringA_dEmpty;
static Static<HashUntypedData> Hash_StringW_StringW_dEmpty;
static Static<HashUntypedData> Hash_StringA_Var_dEmpty;
static Static<HashUntypedData> Hash_StringW_Var_dEmpty;

static Static<HashUntyped> Hash_Unknown_Unknown_oEmpty;
static Static<HashUntyped> Hash_StringA_StringA_oEmpty;
static Static<HashUntyped> Hash_StringW_StringW_oEmpty;
static Static<HashUntyped> Hash_StringA_Var_oEmpty;
static Static<HashUntyped> Hash_StringW_Var_oEmpty;

static Static<HashUntypedVTable> Hash_StringA_StringA_vTable;
static Static<HashUntypedVTable> Hash_StringW_StringW_vTable;
static Static<HashUntypedVTable> Hash_StringA_Var_vTable;
static Static<HashUntypedVTable> Hash_StringW_Var_vTable;

// ============================================================================
// [Fog::Hash - Primes]
// ============================================================================

static FOG_INLINE size_t Hash_alignNodeSize(size_t size)
{
  return (size + sizeof(size_t) - 1) & ~((size_t)sizeof(size_t)-1);
}

// ============================================================================
// [Fog::Hash - Helpers]
// ============================================================================

static size_t FOG_CDECL Hash_calcExpandCapacity(size_t capacity)
{
  static const size_t threshold = 1024*1024*4;

  if (capacity < threshold)
    capacity *= 2;
  else
    capacity += threshold;

  return HashUtil::getClosestPrime(capacity);
}

static size_t FOG_CDECL Hash_calcShrinkCapacity(size_t capacity)
{
  capacity /= 2;
  return HashUtil::getClosestPrime(capacity, -1);
}

static HashUntypedData* Hash_Unknown_Unknown_getDEmptyForType(uint32_t vType)
{
  switch (vType)
  {
    case VAR_TYPE_UNKNOWN:
      return &Hash_Unknown_Unknown_dEmpty;

    case VAR_TYPE_HASH_STRINGA_STRINGA:
      return &Hash_StringA_StringA_dEmpty;

    case VAR_TYPE_HASH_STRINGA_VAR:
      return &Hash_StringA_Var_dEmpty;

    case VAR_TYPE_HASH_STRINGW_STRINGW:
      return &Hash_StringW_StringW_dEmpty;

    case VAR_TYPE_HASH_STRINGW_VAR:
      return &Hash_StringW_Var_dEmpty;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
  
  return NULL;
}

// ============================================================================
// [Fog::Hash<Unknown, Unknown> - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Hash_Unknown_Unknown_ctor(HashUntyped* self)
{
  self->_d = Hash_Unknown_Unknown_dEmpty->addRef();
}

static void FOG_CDECL Hash_Unknown_Unknown_ctorCopy(HashUntyped* self, const HashUntyped* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL Hash_Unknown_Unknown_dtor(HashUntyped* self, const HashUntypedVTable* v)
{
  HashUntypedData* d = self->_d;

  if (d != NULL && d->reference.deref())
    fog_api.hash_unknown_unknown_dFree(d, v);
}

// ============================================================================
// [Fog::Hash<Unknown, Unknown> - Sharing]
// ============================================================================

static err_t FOG_CDECL Hash_Unknown_Unknown_rehashExclude(HashUntyped* self, const HashUntypedVTable* v, size_t capacity, HashUntypedNode* exclude)
{
  HashUntypedData* d = self->_d;
  size_t length = d->length;

  size_t oldCapacity = d->capacity;
  size_t newCapacity = capacity;

  HashUntypedData* newd = fog_api.hash_unknown_unknown_dCreate(newCapacity);
  if (FOG_IS_NULL(newd))
  {
    return ERR_RT_OUT_OF_MEMORY;
  }

  size_t szNode = Hash_alignNodeSize(v->idxItemT + v->szItemT);
  err_t err = newd->nodePool.prealloc(szNode, length);

  if (FOG_IS_ERROR(err))
  {
    fog_api.hash_unknown_unknown_dFree(newd, v);
    return err;
  }

  HashUntypedNode** oData = d->data;
  HashUntypedNode** nData = newd->data;

  size_t i;

  size_t idxKey = v->idxKeyT;
  size_t idxItem = v->idxItemT;

#if defined(FOG_DEBUG)
  bool excluded = false;
#endif // FOG_DEBUG

  // Optimization. If the capacity of 'd' is equal to the capacity of 'newd'
  // then we do not need to call haskKey() for each element to get the index
  // into data[] array, we just create key/item pairs in the new location.
  if (newCapacity == oldCapacity)
  {
    for (i = 0; i < oldCapacity; i++)
    {
      HashUntypedNode** nPrev = &nData[i];
      HashUntypedNode* oNode = oData[i];

      while (oNode)
      {
#if defined(FOG_DEBUG)
        excluded |= (oNode == exclude);
#endif // FOG_DEBUG

        if (oNode != exclude)
        {
          HashUntypedNode* nNode = reinterpret_cast<HashUntypedNode*>(newd->nodePool.alloc(szNode));

          // We preallocated all nodes, it's not possible to get NULL here.
          FOG_ASSERT(nNode != NULL);

          *nPrev = v->ctor(nNode, reinterpret_cast<uint8_t*>(oNode) + idxKey,
                                  reinterpret_cast<uint8_t*>(oNode) + idxItem);
          nPrev = &nNode->next;
        }
        oNode = oNode->next;
      }

      *nPrev = NULL;
    }
  }
  else
  {
    for (i = 0; i < oldCapacity; i++)
    {
      HashUntypedNode* oNode = oData[i];

      while (oNode)
      {
#if defined(FOG_DEBUG)
        excluded |= (oNode == exclude);
#endif // FOG_DEBUG

        if (oNode != exclude)
        {
          uint32_t hashCode = v->hashKey(reinterpret_cast<uint8_t*>(oNode) + idxKey);
          uint32_t hashMod = hashCode % newCapacity;

          HashUntypedNode** nPrev = &d->data[hashMod];
          HashUntypedNode* nNode = reinterpret_cast<HashUntypedNode*>(newd->nodePool.alloc(szNode));

          // We preallocated all nodes, it's not possible to get NULL here.
          FOG_ASSERT(nNode != NULL);

          v->ctor(nNode, reinterpret_cast<uint8_t*>(oNode) + idxKey,
                         reinterpret_cast<uint8_t*>(oNode) + idxItem);

          nNode->next = *nPrev;
          *nPrev = nNode->next;
        }

        oNode = oNode->next;
      }
    }
  }

#if defined(FOG_DEBUG)
  if (exclude != NULL)
  {
    FOG_ASSERT_X(excluded,
      "Fog::Hash<?, ?::rehashExclude() - Exclude node specified, but not found.");
  }
#endif // FOG_DEBUG

  newd->length = d->length - (exclude != NULL);

  d = atomicPtrXchg(&self->_d, newd);
  if (d->reference.deref())
    fog_api.hash_unknown_unknown_dFree(d, v);

  return ERR_OK;
}

static err_t FOG_CDECL Hash_Unknown_Unknown_detach(HashUntyped* self, const HashUntypedVTable* v)
{
  HashUntypedData* d = self->_d;

  if (d->reference.get() == 1)
    return ERR_OK;

  return Hash_Unknown_Unknown_rehashExclude(self, v, d->capacity, NULL);
}

// ============================================================================
// [Fog::Hash<Unknown, Unknown> - Container]
// ============================================================================

static err_t FOG_CDECL Hash_Unknown_Unknown_rehash(HashUntyped* self, const HashUntypedVTable* v, size_t capacity)
{
  return Hash_Unknown_Unknown_rehashExclude(self, v, capacity, NULL);
}

static err_t FOG_CDECL Hash_Unknown_Unknown_reserve(HashUntyped* self, const HashUntypedVTable* v, size_t capacity)
{
  HashUntypedData* d = self->_d;
  capacity = HashUtil::getClosestPrime(capacity);

  if (d->reference.get() != 1 || d->capacity < capacity)
    return fog_api.hash_unknown_unknown_rehash(self, v, capacity);
  else
    return ERR_OK;
}

static void FOG_CDECL Hash_Unknown_Unknown_squeeze(HashUntyped* self, const HashUntypedVTable* v)
{
  HashUntypedData* d = self->_d;
  size_t optimal = HashUtil::getClosestPrime(d->length);

  if (optimal < d->capacity)
    fog_api.hash_unknown_unknown_rehash(self, v, optimal);
}

// ============================================================================
// [Fog::Hash<Unknown, Unknown> - Clear / Reset]
// ============================================================================

static void FOG_CDECL Hash_Unknown_Unknown_clear(HashUntyped* self, const HashUntypedVTable* v)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return;

  if (d->reference.get() != 1)
  {
    fog_api.hash_unknown_unknown_reset(self, v);
    return;
  }

  size_t i, capacity = d->capacity;
  HashUntypedNode** data = d->data;

  for (i = 0; i < capacity; i++)
  {
    HashUntypedNode* node = data[i];

    if (node != NULL)
    {
      do {
        HashUntypedNode* next = node->next;

        v->dtor(node);
        d->nodePool.free(node);

        node = next;
      } while (node);

      data[i] = NULL;
    }
  }
  d->length = 0;
}

static void FOG_CDECL Hash_Unknown_Unknown_reset(HashUntyped* self, const HashUntypedVTable* v)
{
  HashUntypedData* d = atomicPtrXchg(&self->_d,
    Hash_Unknown_Unknown_getDEmptyForType(self->_d->vType & VAR_TYPE_MASK)->addRef());

  if (d->reference.deref())
    fog_api.hash_unknown_unknown_dFree(d, v);
}

// ============================================================================
// [Fog::Hash<Unknown, Unknown> - Accessors]
// ============================================================================

static const void* FOG_CDECL Hash_Unknown_Unknown_get(const HashUntyped* self, const HashUntypedVTable* v, const void* key)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return NULL;

  uint32_t hashCode = v->hashKey(key);
  uint32_t hashMod = hashCode % d->capacity;

  HashUntypedNode* node = d->data[hashMod];

  size_t idxKey = v->idxKeyT;
  EqFunc eqKey = v->eqKey;

  while (node)
  {
    if (eqKey(reinterpret_cast<uint8_t*>(node) + idxKey, key))
      goto _Match;

    node = node->next;
  }

  return NULL;

_Match:
  return reinterpret_cast<uint8_t*>(node) + v->idxItemT;
}

static void* FOG_CDECL Hash_Unknown_Unknown_use(HashUntyped* self, const HashUntypedVTable* v, const void* key)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return NULL;

  uint32_t hashCode = v->hashKey(key);
  uint32_t hashMod;

  HashUntypedNode** pPrev;
  HashUntypedNode* node;

  size_t idxKey = v->idxKeyT;
  EqFunc eqKey = v->eqKey;

_Repeat:
  hashMod = hashCode % d->capacity;

  pPrev = &d->data[hashMod];
  node = *pPrev;

  while (node)
  {
    if (eqKey(reinterpret_cast<uint8_t*>(node) + idxKey, key))
      goto _Match;

    pPrev = &node->next;
    node = *pPrev;
  }
  return NULL;

_Match:
  if (d->reference.get() == 1)
    return reinterpret_cast<uint8_t*>(node) + v->idxItemT;

  if (fog_api.hash_unknown_unknown_detach(self, v) != ERR_OK)
    return NULL;

  d = self->_d;
  goto _Repeat;
}

static err_t FOG_CDECL Hash_Unknown_Unknown_put(HashUntyped* self, const HashUntypedVTable* v, const void* key, const void* item, bool replace)
{
  HashUntypedData* d = self->_d;

  // If the length is zero then there is a chance that the MemPool allocator is
  // not initialized (we do not initialize before the first element is added to
  // the hash table).
  if (d->length == 0)
  {
    FOG_RETURN_ON_ERROR(fog_api.hash_unknown_unknown_reserve(self, v, HashUtil::getClosestPrime(0)));
    d = self->_d;
  }
  else if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(fog_api.hash_unknown_unknown_detach(self, v));
    d = self->_d;
  }

  uint32_t hashCode = v->hashKey(key);
  uint32_t hashMod = hashCode % d->capacity;

  HashUntypedNode** pPrev = &d->data[hashMod];
  HashUntypedNode* node = *pPrev;

  size_t idxKey = v->idxKeyT;
  EqFunc eqKey = v->eqKey;

  while (node)
  {
    if (eqKey(reinterpret_cast<uint8_t*>(node) + idxKey, key))
      goto _Match;

    pPrev = &node->next;
    node = *pPrev;
  }

  node = reinterpret_cast<HashUntypedNode*>(d->nodePool.alloc(Hash_alignNodeSize(v->idxItemT + v->szItemT)));
  if (FOG_IS_NULL(node))
    return ERR_RT_OUT_OF_MEMORY;

  node->next = NULL;
  v->ctor(node, key, item);

  *pPrev = node;

  if (++d->length >= d->expandLength)
    fog_api.hash_unknown_unknown_rehash(self, v, d->expandCapacity);
  return ERR_OK;

_Match:
  if (!replace)
    return ERR_RT_OBJECT_ALREADY_EXISTS;

  v->setItem(reinterpret_cast<uint8_t*>(node) + v->idxItemT, item);
  return ERR_OK;
}

static err_t FOG_CDECL Hash_Unknown_Unknown_remove(HashUntyped* self, const HashUntypedVTable* v, const void* key)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return ERR_RT_OBJECT_NOT_FOUND;

  uint32_t hashCode = v->hashKey(key);
  uint32_t hashMod = hashCode % d->capacity;

  HashUntypedNode** pPrev = &d->data[hashMod];
  HashUntypedNode* node = *pPrev;

  size_t idxKey = v->idxKeyT;
  EqFunc eqKey = v->eqKey;

  while (node)
  {
    if (eqKey(reinterpret_cast<uint8_t*>(node) + idxKey, key))
      goto _Match;

    pPrev = &node->next;
    node = *pPrev;
  }
  return ERR_RT_OBJECT_NOT_FOUND;

_Match:
  if (d->reference.get() != 1 || d->length - 1 <= d->shrinkLength)
  {
    size_t capacity = HashUtil::getClosestPrime(d->length - 1);
    return Hash_Unknown_Unknown_rehashExclude(self, v, capacity, node);
  }
  else
  {
    *pPrev = node->next;
    v->dtor(node);
    d->nodePool.free(node);
    d->length--;
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::Hash<Unknown, Unknown> - Copy]
// ============================================================================

static void FOG_CDECL Hash_Unknown_Unknown_copy(HashUntyped* self, const HashUntypedVTable* v, const HashUntyped* other)
{
  HashUntypedData* d = atomicPtrXchg(&self->_d, other->_d->addRef());
  if (d->reference.deref())
    fog_api.hash_unknown_unknown_dFree(d, v);
}

// ============================================================================
// [Fog::Hash<Unknown, Unknown> - Equality]
// ============================================================================

static bool FOG_CDECL Hash_Unknown_Unknown_eq(const HashUntyped* a, const HashUntyped* b, const HashUntypedVTable* v, EqFunc itemEqFunc)
{
  const HashUntypedData* a_d = a->_d;
  const HashUntypedData* b_d = b->_d;

  if (a_d->length != b_d->length)
    return false;

  if (a_d == b_d)
    return true;

  if (a_d->length == 0)
    return true;

  // We must choose one table, iterate, and lookup into the other one. Instead
  // of choosing 'a' or 'b' we choose the table which has less capacity, so we
  // do not iterate over empty table if it's nearly empty.
  size_t aIndex = 0;

  size_t aCapacity = a_d->capacity;
  size_t bCapacity = b_d->capacity;

  if (aCapacity > bCapacity)
  {
    swap(a, b);
    swap(a_d, b_d);
    swap(aCapacity, bCapacity);
  }

  const HashUntypedNode* const* aData = a_d->data;
  const HashUntypedNode* const* bData = b_d->data;

  size_t idxKey = v->idxKeyT;
  size_t idxItem = v->idxItemT;
  EqFunc eqKey = v->eqKey;

  // If capacity of 'a' is equal to the capacity of 'b' then we can save some
  // CPU time, because we don't need to compute hashCode and hashMod. HashMod
  // is equal to 'aIndex' and hashCode is not needed at all.
  if (aCapacity == bCapacity)
  {
    do {
      const HashUntypedNode* aNode = aData[aIndex];
      const HashUntypedNode* bNode = bData[aIndex];

      if (aNode)
      {
        do {
          for (;;)
          {
            if (bNode == NULL)
            {
              return false;
            }

            if (!eqKey(reinterpret_cast<const uint8_t*>(aNode) + idxKey,
                       reinterpret_cast<const uint8_t*>(bNode) + idxKey))
            {
              bNode = bNode->next;
              continue;
            }

            if (!itemEqFunc(reinterpret_cast<const uint8_t*>(aNode) + idxItem,
                            reinterpret_cast<const uint8_t*>(bNode) + idxItem))
            {
              return false;
            }

            break;
          }

          aNode = aNode->next;
        } while (aNode);
      }
      else
      {
        if (bNode)
          return false;
      }
    } while (++aIndex < aCapacity);
  }
  else
  {
    do {
      const HashUntypedNode* aNode = aData[aIndex];

      while (aNode)
      {
        uint32_t hashCode = v->hashKey(reinterpret_cast<const uint8_t*>(aNode) + idxKey);
        uint32_t hashMod = hashCode % bCapacity;

        const HashUntypedNode* bNode = bData[hashMod];
        for (;;)
        {
          if (bNode == NULL)
          {
            return false;
          }

          if (!eqKey(reinterpret_cast<const uint8_t*>(aNode) + idxKey,
                     reinterpret_cast<const uint8_t*>(bNode) + idxKey))
          {
            bNode = bNode->next;
            continue;
          }

          if (!itemEqFunc(reinterpret_cast<const uint8_t*>(aNode) + idxItem,
                          reinterpret_cast<const uint8_t*>(bNode) + idxItem))
          {
            return false;
          }

          break;
        }

        aNode = aNode->next;
      }
    } while (++aIndex < aCapacity);
  }

  return true;
}

// ============================================================================
// [Fog::Hash<Unknown, Unknown> - HashUntypedData]
// ============================================================================

static HashUntypedData* FOG_CDECL Hash_Unknown_Unknown_dCreate(size_t capacity)
{
  if (capacity >= (SIZE_MAX - sizeof(HashUntypedData)) / sizeof(void*))
    return NULL;

  HashUntypedData* d = reinterpret_cast<HashUntypedData*>(
    MemMgr::calloc(HashUntypedData::getSizeOf(capacity))
  );

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = VAR_TYPE_UNKNOWN | VAR_FLAG_NONE;
  FOG_PADDING_ZERO_64(d->padding0_32);

  d->capacity = capacity;
  d->length = 0;

  size_t expand = fog_api.hashhelper_calcExpandCapacity(capacity);
  size_t shrink = fog_api.hashhelper_calcShrinkCapacity(capacity);

  d->expandCapacity = expand;
  d->shrinkCapacity = shrink;

  if (capacity <= SIZE_MAX / 19)
    d->expandLength = (capacity * 19) / 20;
  else
    d->expandLength = size_t(double(capacity) * 0.95);

  if (shrink == capacity)
    d->shrinkCapacity = 0;
  else if (shrink <= SIZE_MAX / 4)
    d->shrinkLength = (shrink * 4) / 5;
  else
    d->shrinkLength = size_t(double(shrink) * 0.80);

  fog_new_p(&d->nodePool) MemPool();

  return d;
}

static void FOG_CDECL Hash_Unknown_Unknown_dFree(HashUntypedData* d, const HashUntypedVTable* v)
{
  size_t i, capacity = d->capacity;
  HashUntypedNode** data = d->data;

  for (i = 0; i < capacity; i++)
  {
    HashUntypedNode* node = data[i];

    while (node)
    {
      HashUntypedNode* next = node->next;
      // We do not free node here, the ~MemPool() destructor does the job
      // for all nodes.
      v->dtor(node);
      node = next;
    }
  }

  d->nodePool.~MemPool();

  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

// ============================================================================
// [Fog::Hash<StringT, Unknown>]
// ============================================================================

template<typename CharT, typename SrcT>
static const void* FOG_CDECL Hash_StringT_Unknown_getStub(const HashUntyped* self, const HashUntypedVTable* v, const SrcT_(Stub)* _key)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return NULL;

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getComputedLength();

  uint32_t hashCode = HashUtil::hash<SrcT_(Stub)>(SrcT_(Stub)(keyData, keyLength));
  uint32_t hashMod = hashCode % d->capacity;

  HashKeyNode<CharT_(String)>* node = reinterpret_cast<HashKeyNode<CharT_(String)>*>(d->data[hashMod]);
  while (node)
  {
    if (node->key.getLength() == keyLength && StringUtil::eq(node->key.getData(), keyData, keyLength))
      goto _Match;

    node = reinterpret_cast<HashKeyNode<CharT_(String)>*>(node->next);
  }

  return NULL;

_Match:
  return reinterpret_cast<uint8_t*>(node) + v->idxItemT;
}

template<typename CharT, typename SrcT>
static const void* FOG_CDECL Hash_StringT_Unknown_getString(const HashUntyped* self, const HashUntypedVTable* v, const SrcT_(String)* _key)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return NULL;

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getLength();

  uint32_t hashCode = _key->getHashCode();
  uint32_t hashMod = hashCode % d->capacity;

  HashKeyNode<CharT_(String)>* node = reinterpret_cast<HashKeyNode<CharT_(String)>*>(d->data[hashMod]);
  while (node)
  {
    if (node->key.getLength() == keyLength && StringUtil::eq(node->key.getData(), keyData, keyLength))
      goto _Match;

    node = reinterpret_cast<HashKeyNode<CharT_(String)>*>(node->next);
  }

  return NULL;

_Match:
  return reinterpret_cast<uint8_t*>(node) + v->idxItemT;
}

template<typename CharT, typename SrcT>
static void* FOG_CDECL Hash_StringT_Unknown_useStub(HashUntyped* self, const HashUntypedVTable* v, const SrcT_(Stub)* _key)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return NULL;

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getComputedLength();

  uint32_t hashCode = HashUtil::hash<SrcT_(Stub)>(SrcT_(Stub)(keyData, keyLength));
  uint32_t hashMod;

  HashKeyNode<CharT_(String)>** pPrev;
  HashKeyNode<CharT_(String)>* node;

_Repeat:
  hashMod = hashCode % d->capacity;

  pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&d->data[hashMod]);
  node = *pPrev;

  while (node)
  {
    if (node->key.getLength() == keyLength && StringUtil::eq(node->key.getData(), keyData, keyLength))
      goto _Match;

    pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&node->next);
    node = *pPrev;
  }
  return NULL;

_Match:
  if (d->reference.get() == 1)
    return reinterpret_cast<uint8_t*>(node) + v->idxItemT;

  if (fog_api.hash_unknown_unknown_detach(self, v) != ERR_OK)
    return NULL;

  d = self->_d;
  goto _Repeat;
}

template<typename CharT, typename SrcT>
static void* FOG_CDECL Hash_StringT_Unknown_useString(HashUntyped* self, const HashUntypedVTable* v, const SrcT_(String)* _key)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return NULL;

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getLength();

  uint32_t hashCode = _key->getHashCode();
  uint32_t hashMod;

  HashKeyNode<CharT_(String)>** pPrev;
  HashKeyNode<CharT_(String)>* node;

_Repeat:
  hashMod = hashCode % d->capacity;

  pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&d->data[hashMod]);
  node = *pPrev;

  while (node)
  {
    if (node->key.getLength() == keyLength && StringUtil::eq(node->key.getData(), keyData, keyLength))
      goto _Match;

    pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&node->next);
    node = *pPrev;
  }
  return NULL;

_Match:
  if (d->reference.get() == 1)
    return reinterpret_cast<uint8_t*>(node) + v->idxItemT;

  if (fog_api.hash_unknown_unknown_detach(self, v) != ERR_OK)
    return NULL;

  d = self->_d;
  goto _Repeat;
}

template<typename CharT, typename SrcT>
static err_t FOG_CDECL Hash_StringT_Unknown_putStub(HashUntyped* self, const HashUntypedVTable* v, const SrcT_(Stub)* _key, const void* item, bool replace)
{
  HashUntypedData* d = self->_d;

  // If the length is zero then there is a chance that the MemPool allocator is
  // not initialized (we do not initialize before the first element is added to
  // the hash table).
  if (d->length == 0)
  {
    FOG_RETURN_ON_ERROR(fog_api.hash_unknown_unknown_reserve(self, v, HashUtil::getClosestPrime(0)));
    d = self->_d;
  }
  else if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(fog_api.hash_unknown_unknown_detach(self, v));
    d = self->_d;
  }

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getComputedLength();

  uint32_t hashCode = HashUtil::hash<SrcT_(Stub)>(SrcT_(Stub)(keyData, keyLength));
  uint32_t hashMod = hashCode % d->capacity;

  HashKeyNode<CharT_(String)>** pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&d->data[hashMod]);
  HashKeyNode<CharT_(String)>* node = *pPrev;

  while (node)
  {
    if (node->key.getLength() == keyLength && StringUtil::eq(node->key.getData(), keyData, keyLength))
      goto _Match;

    pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&node->next);
    node = *pPrev;
  }

  {
    node = reinterpret_cast<HashKeyNode<CharT_(String)>*>(d->nodePool.alloc(Hash_alignNodeSize(v->idxItemT + v->szItemT)));
    if (FOG_IS_NULL(node))
      return ERR_RT_OUT_OF_MEMORY;

    CharT_(String) keyString;
    err_t err = keyString._set(SrcT_(Stub)(keyData, keyLength));

    if (FOG_IS_ERROR(err))
    {
      d->nodePool.free(node);
      return err;
    }

    node->next = NULL;
    v->ctor(node, &keyString, item);

    *pPrev = node;
  }


  if (++d->length >= d->expandLength)
    fog_api.hash_unknown_unknown_rehash(self, v, d->expandCapacity);
  return ERR_OK;

_Match:
  if (!replace)
    return ERR_RT_OBJECT_ALREADY_EXISTS;

  v->setItem(reinterpret_cast<uint8_t*>(node) + v->idxItemT, item);
  return ERR_OK;
}

template<typename CharT, typename SrcT>
static err_t FOG_CDECL Hash_StringT_Unknown_putString(HashUntyped* self, const HashUntypedVTable* v, const SrcT_(String)* _key, const void* item, bool replace)
{
  HashUntypedData* d = self->_d;

  // If the length is zero then there is a chance that the MemPool allocator is
  // not initialized (we do not initialize before the first element is added to
  // the hash table).
  if (d->length == 0)
  {
    FOG_RETURN_ON_ERROR(fog_api.hash_unknown_unknown_reserve(self, v, HashUtil::getClosestPrime(0)));
    d = self->_d;
  }
  else if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(fog_api.hash_unknown_unknown_detach(self, v));
    d = self->_d;
  }

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getLength();

  uint32_t hashCode = _key->getHashCode();
  uint32_t hashMod = hashCode % d->capacity;

  HashKeyNode<CharT_(String)>** pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&d->data[hashMod]);
  HashKeyNode<CharT_(String)>* node = *pPrev;

  while (node)
  {
    if (node->key.getLength() == keyLength && StringUtil::eq(node->key.getData(), keyData, keyLength))
      goto _Match;

    pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&node->next);
    node = *pPrev;
  }

  node = reinterpret_cast<HashKeyNode<CharT_(String)>*>(d->nodePool.alloc(Hash_alignNodeSize(v->idxItemT + v->szItemT)));
  if (FOG_IS_NULL(node))
    return ERR_RT_OUT_OF_MEMORY;

  node->next = NULL;
  v->ctor(node, _key, item);

  *pPrev = node;

  if (++d->length >= d->expandLength)
    fog_api.hash_unknown_unknown_rehash(self, v, d->expandCapacity);
  return ERR_OK;

_Match:
  if (!replace)
    return ERR_RT_OBJECT_ALREADY_EXISTS;

  v->setItem(reinterpret_cast<uint8_t*>(node) + v->idxItemT, item);
  return ERR_OK;
}

template<typename CharT, typename SrcT>
static err_t FOG_CDECL Hash_StringT_Unknown_removeStub(HashUntyped* self, const HashUntypedVTable* v, const SrcT_(Stub)* _key)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return ERR_RT_OBJECT_NOT_FOUND;

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getComputedLength();

  uint32_t hashCode = HashUtil::hash<SrcT_(Stub)>(SrcT_(Stub)(keyData, keyLength));
  uint32_t hashMod = hashCode % d->capacity;

  HashKeyNode<CharT_(String)>** pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&d->data[hashMod]);
  HashKeyNode<CharT_(String)>* node = *pPrev;

  while (node)
  {
    if (node->key.getLength() == keyLength && StringUtil::eq(node->key.getData(), keyData, keyLength))
      goto _Match;

    pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&node->next);
    node = *pPrev;
  }
  return ERR_RT_OBJECT_NOT_FOUND;

_Match:
  if (d->reference.get() != 1 || d->length - 1<= d->shrinkLength)
  {
    size_t capacity = HashUtil::getClosestPrime(d->length - 1);
    return Hash_Unknown_Unknown_rehashExclude(self, v, capacity, node);
  }
  else
  {
    *pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>*>(node->next);
    v->dtor(node);
    d->nodePool.free(node);
    d->length--;
    return ERR_OK;
  }
}

template<typename CharT, typename SrcT>
static err_t FOG_CDECL Hash_StringT_Unknown_removeString(HashUntyped* self, const HashUntypedVTable* v, const SrcT_(String)* _key)
{
  HashUntypedData* d = self->_d;
  if (d->length == 0)
    return ERR_RT_OBJECT_NOT_FOUND;

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getLength();

  uint32_t hashCode = _key->getHashCode();
  uint32_t hashMod = hashCode % d->capacity;

  HashKeyNode<CharT_(String)>** pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&d->data[hashMod]);
  HashKeyNode<CharT_(String)>* node = *pPrev;

  while (node)
  {
    if (node->key.getLength() == keyLength && StringUtil::eq(node->key.getData(), keyData, keyLength))
      goto _Match;

    pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&node->next);
    node = *pPrev;
  }
  return ERR_RT_OBJECT_NOT_FOUND;

_Match:
  if (d->reference.get() != 1 || d->length - 1<= d->shrinkLength)
  {
    size_t capacity = HashUtil::getClosestPrime(d->length - 1);
    return Hash_Unknown_Unknown_rehashExclude(self, v, capacity, node);
  }
  else
  {
    *pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>*>(node->next);
    v->dtor(node);
    d->nodePool.free(node);
    d->length--;
    return ERR_OK;
  }
}

template<typename CharT>
static bool FOG_CDECL Hash_StringT_Unknown_eq(const HashUntyped* a, const HashUntyped* b, const HashUntypedVTable* v, EqFunc itemEqFunc)
{
  const HashUntypedData* a_d = a->_d;
  const HashUntypedData* b_d = b->_d;

  if (a_d->length != b_d->length)
    return false;

  if (a_d == b_d)
    return true;

  if (a_d->length == 0)
    return true;

  // We must choose one table, iterate, and lookup into the other one. Instead
  // of choosing 'a' or 'b' we choose the table which has less capacity, so we
  // do not iterate over empty table if it's nearly empty.
  size_t aIndex = 0;

  size_t aCapacity = a_d->capacity;
  size_t bCapacity = b_d->capacity;

  if (aCapacity > bCapacity)
  {
    swap(a, b);
    swap(a_d, b_d);
    swap(aCapacity, bCapacity);
  }

  const HashKeyNode<CharT_(String)>* const* aData = reinterpret_cast<HashKeyNode<CharT_(String)>* const*>(a_d->data);
  const HashKeyNode<CharT_(String)>* const* bData = reinterpret_cast<HashKeyNode<CharT_(String)>* const*>(b_d->data);

  size_t idxItem = v->idxItemT;

  // If capacity of 'a' is equal to the capacity of 'b' then we can save some
  // CPU time, because we don't need to compute hashCode and hashMod. HashMod
  // is equal to 'aIndex' and hashCode is not needed at all.
  if (aCapacity == bCapacity)
  {
    do {
      const HashKeyNode<CharT_(String)>* aNode = aData[aIndex];
      const HashKeyNode<CharT_(String)>* bNode = bData[aIndex];

      if (aNode)
      {
        do {
          for (;;)
          {
            if (bNode == NULL)
            {
              return false;
            }

            if (aNode->key != bNode->key)
            {
              bNode = reinterpret_cast<const HashKeyNode<CharT_(String)>*>(bNode->next);
              continue;
            }

            if (!itemEqFunc(reinterpret_cast<const uint8_t*>(aNode) + idxItem,
                            reinterpret_cast<const uint8_t*>(bNode) + idxItem))
            {
              return false;
            }

            break;
          }

          aNode = reinterpret_cast<const HashKeyNode<CharT_(String)>*>(aNode->next);
        } while (aNode);
      }
      else
      {
        if (bNode)
          return false;
      }
    } while (++aIndex < aCapacity);
  }
  else
  {
    do {
      const HashKeyNode<CharT_(String)>* aNode = aData[aIndex];

      while (aNode)
      {
        uint32_t hashCode = aNode->key._d->hashCode;
        uint32_t hashMod = hashCode % bCapacity;

        const HashKeyNode<CharT_(String)>* bNode = bData[hashMod];
        for (;;)
        {
          if (bNode == NULL)
          {
            return false;
          }

          if (aNode->key != bNode->key)
          {
            bNode = reinterpret_cast<const HashKeyNode<CharT_(String)>*>(bNode->next);
            continue;
          }

          if (!itemEqFunc(reinterpret_cast<const uint8_t*>(aNode) + idxItem,
                          reinterpret_cast<const uint8_t*>(bNode) + idxItem))
          {
            return false;
          }

          break;
        }

        aNode = reinterpret_cast<const HashKeyNode<CharT_(String)>*>(aNode->next);
      }
    } while (++aIndex < aCapacity);
  }

  return true;
}

// ============================================================================
// [Fog::Hash<StringA, StringA>]
// ============================================================================

static HashUntypedNode* FOG_CDECL Hash_StringA_StringA_Node_ctor(HashUntypedNode* _node, const void* key, const void* item)
{
  HashNode<StringA, StringA>* node = reinterpret_cast<HashNode<StringA, StringA>*>(_node);

  node->key._d = reinterpret_cast<const StringA*>(key)->_d->addRef();
  node->item._d = reinterpret_cast<const StringA*>(item)->_d->addRef();

  return _node;
}

static void FOG_CDECL Hash_StringA_StringA_Node_dtor(HashUntypedNode* _node)
{
  HashNode<StringA, StringA>* node = reinterpret_cast<HashNode<StringA, StringA>*>(_node);

  node->key._d->release();
  node->item._d->release();
}

static void FOG_CDECL Hash_StringA_StringA_ctor(HashUntyped* self)
{
  self->_d = Hash_StringA_StringA_dEmpty->addRef();
}

static void FOG_CDECL Hash_StringA_StringA_dtor(HashUntyped* self)
{
  HashUntypedData* d = self->_d;

  if (d != NULL && d->reference.deref())
    fog_api.hash_unknown_unknown_dFree(d, &Hash_StringA_StringA_vTable);
}

static const StringA* FOG_CDECL Hash_StringA_StringA_getStubA(const HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<const StringA*>(
    fog_api.hash_stringa_unknown_getStubA(self, &Hash_StringA_StringA_vTable, key));
}

static const StringA* FOG_CDECL Hash_StringA_StringA_getStringA(const HashUntyped* self, const StringA* key)
{
  return reinterpret_cast<const StringA*>(
    fog_api.hash_stringa_unknown_getStringA(self, &Hash_StringA_StringA_vTable, key));
}

static StringA* FOG_CDECL Hash_StringA_StringA_useStubA(HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<StringA*>(
    fog_api.hash_stringa_unknown_useStubA(self, &Hash_StringA_StringA_vTable, key));
}

static StringA* FOG_CDECL Hash_StringA_StringA_useStringA(HashUntyped* self, const StringA* key)
{
  return reinterpret_cast<StringA*>(
    fog_api.hash_stringa_unknown_useStringA(self, &Hash_StringA_StringA_vTable, key));
}

static err_t FOG_CDECL Hash_StringA_StringA_putStubA(HashUntyped* self, const StubA* key, const StringA* item, bool replace)
{
  return fog_api.hash_stringa_unknown_putStubA(self, &Hash_StringA_StringA_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringA_StringA_putStringA(HashUntyped* self, const StringA* key, const StringA* item, bool replace)
{
  return fog_api.hash_stringa_unknown_putStringA(self, &Hash_StringA_StringA_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringA_StringA_removeStubA(HashUntyped* self, const StubA* key)
{
  return fog_api.hash_stringa_unknown_removeStubA(self, &Hash_StringA_StringA_vTable, key);
}

static err_t FOG_CDECL Hash_StringA_StringA_removeStringA(HashUntyped* self, const StringA* key)
{
  return fog_api.hash_stringa_unknown_removeStringA(self, &Hash_StringA_StringA_vTable, key);
}

static void FOG_CDECL Hash_StringA_StringA_dFree(HashUntypedData* d)
{
  fog_api.hash_unknown_unknown_dFree(d, &Hash_StringA_StringA_vTable);
}

// ============================================================================
// [Fog::Hash<StringA, Var>]
// ============================================================================

static HashUntypedNode* FOG_CDECL Hash_StringA_Var_Node_ctor(HashUntypedNode* _node, const void* key, const void* item)
{
  HashNode<StringA, Var>* node = reinterpret_cast<HashNode<StringA, Var>*>(_node);

  node->key._d = reinterpret_cast<const StringA*>(key)->_d->addRef();
  node->item._d = reinterpret_cast<const Var*>(item)->_d->addRef();

  return _node;
}

static void FOG_CDECL Hash_StringA_Var_Node_dtor(HashUntypedNode* _node)
{
  HashNode<StringA, Var>* node = reinterpret_cast<HashNode<StringA, Var>*>(_node);

  node->key._d->release();
  node->item._d->release();
}

static void FOG_CDECL Hash_StringA_Var_ctor(HashUntyped* self)
{
  self->_d = Hash_StringA_Var_dEmpty->addRef();
}

static void FOG_CDECL Hash_StringA_Var_dtor(HashUntyped* self)
{
  HashUntypedData* d = self->_d;

  if (d != NULL && d->reference.deref())
    fog_api.hash_unknown_unknown_dFree(d, &Hash_StringA_Var_vTable);
}

static const Var* FOG_CDECL Hash_StringA_Var_getStubA(const HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<const Var*>(
    fog_api.hash_stringa_unknown_getStubA(self, &Hash_StringA_Var_vTable, key));
}

static const Var* FOG_CDECL Hash_StringA_Var_getStringA(const HashUntyped* self, const StringA* key)
{
  return reinterpret_cast<const Var*>(
    fog_api.hash_stringa_unknown_getStringA(self, &Hash_StringA_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringA_Var_useStubA(HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<Var*>(
    fog_api.hash_stringa_unknown_useStubA(self, &Hash_StringA_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringA_Var_useStringA(HashUntyped* self, const StringA* key)
{
  return reinterpret_cast<Var*>(
    fog_api.hash_stringa_unknown_useStringA(self, &Hash_StringA_Var_vTable, key));
}

static err_t FOG_CDECL Hash_StringA_Var_putStubA(HashUntyped* self, const StubA* key, const Var* item, bool replace)
{
  return fog_api.hash_stringa_unknown_putStubA(self, &Hash_StringA_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringA_Var_putStringA(HashUntyped* self, const StringA* key, const Var* item, bool replace)
{
  return fog_api.hash_stringa_unknown_putStringA(self, &Hash_StringA_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringA_Var_removeStubA(HashUntyped* self, const StubA* key)
{
  return fog_api.hash_stringa_unknown_removeStubA(self, &Hash_StringA_Var_vTable, key);
}

static err_t FOG_CDECL Hash_StringA_Var_removeStringA(HashUntyped* self, const StringA* key)
{
  return fog_api.hash_stringa_unknown_removeStringA(self, &Hash_StringA_Var_vTable, key);
}

static void FOG_CDECL Hash_StringA_Var_dFree(HashUntypedData* d)
{
  fog_api.hash_unknown_unknown_dFree(d, &Hash_StringA_Var_vTable);
}

// ============================================================================
// [Fog::Hash<StringW, StringW>]
// ============================================================================

static HashUntypedNode* FOG_CDECL Hash_StringW_StringW_Node_ctor(HashUntypedNode* _node, const void* key, const void* item)
{
  HashNode<StringW, StringW>* node = reinterpret_cast<HashNode<StringW, StringW>*>(_node);

  node->key._d = reinterpret_cast<const StringW*>(key)->_d->addRef();
  node->item._d = reinterpret_cast<const StringW*>(item)->_d->addRef();

  return _node;
}

static void FOG_CDECL Hash_StringW_StringW_Node_dtor(HashUntypedNode* _node)
{
  HashNode<StringW, StringW>* node = reinterpret_cast<HashNode<StringW, StringW>*>(_node);

  node->key._d->release();
  node->item._d->release();
}

static void FOG_CDECL Hash_StringW_StringW_ctor(HashUntyped* self)
{
  self->_d = Hash_StringW_StringW_dEmpty->addRef();
}

static void FOG_CDECL Hash_StringW_StringW_dtor(HashUntyped* self)
{
  HashUntypedData* d = self->_d;

  if (d != NULL && d->reference.deref())
    fog_api.hash_unknown_unknown_dFree(d, &Hash_StringW_StringW_vTable);
}

static const StringW* FOG_CDECL Hash_StringW_StringW_getStubA(const HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<const StringW*>(
    fog_api.hash_stringw_unknown_getStubA(self, &Hash_StringW_StringW_vTable, key));
}

static const StringW* FOG_CDECL Hash_StringW_StringW_getStubW(const HashUntyped* self, const StubW* key)
{
  return reinterpret_cast<const StringW*>(
    fog_api.hash_stringw_unknown_getStubW(self, &Hash_StringW_StringW_vTable, key));
}

static const StringW* FOG_CDECL Hash_StringW_StringW_getStringW(const HashUntyped* self, const StringW* key)
{
  return reinterpret_cast<const StringW*>(
    fog_api.hash_stringw_unknown_getStringW(self, &Hash_StringW_StringW_vTable, key));
}

static StringW* FOG_CDECL Hash_StringW_StringW_useStubA(HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<StringW*>(
    fog_api.hash_stringw_unknown_useStubA(self, &Hash_StringW_StringW_vTable, key));
}

static StringW* FOG_CDECL Hash_StringW_StringW_useStubW(HashUntyped* self, const StubW* key)
{
  return reinterpret_cast<StringW*>(
    fog_api.hash_stringw_unknown_useStubW(self, &Hash_StringW_StringW_vTable, key));
}

static StringW* FOG_CDECL Hash_StringW_StringW_useStringW(HashUntyped* self, const StringW* key)
{
  return reinterpret_cast<StringW*>(
    fog_api.hash_stringw_unknown_useStringW(self, &Hash_StringW_StringW_vTable, key));
}

static err_t FOG_CDECL Hash_StringW_StringW_putStubA(HashUntyped* self, const StubA* key, const StringW* item, bool replace)
{
  return fog_api.hash_stringw_unknown_putStubA(self, &Hash_StringW_StringW_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_StringW_putStubW(HashUntyped* self, const StubW* key, const StringW* item, bool replace)
{
  return fog_api.hash_stringw_unknown_putStubW(self, &Hash_StringW_StringW_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_StringW_putStringW(HashUntyped* self, const StringW* key, const StringW* item, bool replace)
{
  return fog_api.hash_stringw_unknown_putStringW(self, &Hash_StringW_StringW_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_StringW_removeStubA(HashUntyped* self, const StubA* key)
{
  return fog_api.hash_stringw_unknown_removeStubA(self, &Hash_StringW_StringW_vTable, key);
}

static err_t FOG_CDECL Hash_StringW_StringW_removeStubW(HashUntyped* self, const StubW* key)
{
  return fog_api.hash_stringw_unknown_removeStubW(self, &Hash_StringW_StringW_vTable, key);
}

static err_t FOG_CDECL Hash_StringW_StringW_removeStringW(HashUntyped* self, const StringW* key)
{
  return fog_api.hash_stringw_unknown_removeStringW(self, &Hash_StringW_StringW_vTable, key);
}

static void FOG_CDECL Hash_StringW_StringW_dFree(HashUntypedData* d)
{
  fog_api.hash_unknown_unknown_dFree(d, &Hash_StringW_StringW_vTable);
}

// ============================================================================
// [Fog::Hash<StringW, Var>]
// ============================================================================

static HashUntypedNode* FOG_CDECL Hash_StringW_Var_Node_ctor(HashUntypedNode* _node, const void* key, const void* item)
{
  HashNode<StringW, Var>* node = reinterpret_cast<HashNode<StringW, Var>*>(_node);

  node->key._d = reinterpret_cast<const StringW*>(key)->_d->addRef();
  node->item._d = reinterpret_cast<const Var*>(item)->_d->addRef();

  return _node;
}

static void FOG_CDECL Hash_StringW_Var_Node_dtor(HashUntypedNode* _node)
{
  HashNode<StringW, Var>* node = reinterpret_cast<HashNode<StringW, Var>*>(_node);

  node->key._d->release();
  node->item._d->release();
}

static void FOG_CDECL Hash_StringW_Var_ctor(HashUntyped* self)
{
  self->_d = Hash_StringW_Var_dEmpty->addRef();
}

static void FOG_CDECL Hash_StringW_Var_dtor(HashUntyped* self)
{
  HashUntypedData* d = self->_d;

  if (d != NULL && d->reference.deref())
    fog_api.hash_unknown_unknown_dFree(d, &Hash_StringW_Var_vTable);
}

static const Var* FOG_CDECL Hash_StringW_Var_getStubA(const HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<const Var*>(
    fog_api.hash_stringw_unknown_getStubA(self, &Hash_StringW_Var_vTable, key));
}

static const Var* FOG_CDECL Hash_StringW_Var_getStubW(const HashUntyped* self, const StubW* key)
{
  return reinterpret_cast<const Var*>(
    fog_api.hash_stringw_unknown_getStubW(self, &Hash_StringW_Var_vTable, key));
}

static const Var* FOG_CDECL Hash_StringW_Var_getStringW(const HashUntyped* self, const StringW* key)
{
  return reinterpret_cast<const Var*>(
    fog_api.hash_stringw_unknown_getStringW(self, &Hash_StringW_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringW_Var_useStubA(HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<Var*>(
    fog_api.hash_stringw_unknown_useStubA(self, &Hash_StringW_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringW_Var_useStubW(HashUntyped* self, const StubW* key)
{
  return reinterpret_cast<Var*>(
    fog_api.hash_stringw_unknown_useStubW(self, &Hash_StringW_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringW_Var_useStringW(HashUntyped* self, const StringW* key)
{
  return reinterpret_cast<Var*>(
    fog_api.hash_stringw_unknown_useStringW(self, &Hash_StringW_Var_vTable, key));
}

static err_t FOG_CDECL Hash_StringW_Var_putStubA(HashUntyped* self, const StubA* key, const Var* item, bool replace)
{
  return fog_api.hash_stringw_unknown_putStubA(self, &Hash_StringW_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_Var_putStubW(HashUntyped* self, const StubW* key, const Var* item, bool replace)
{
  return fog_api.hash_stringw_unknown_putStubW(self, &Hash_StringW_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_Var_putStringW(HashUntyped* self, const StringW* key, const Var* item, bool replace)
{
  return fog_api.hash_stringw_unknown_putStringW(self, &Hash_StringW_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_Var_removeStubA(HashUntyped* self, const StubA* key)
{
  return fog_api.hash_stringw_unknown_removeStubA(self, &Hash_StringW_Var_vTable, key);
}

static err_t FOG_CDECL Hash_StringW_Var_removeStubW(HashUntyped* self, const StubW* key)
{
  return fog_api.hash_stringw_unknown_removeStubW(self, &Hash_StringW_Var_vTable, key);
}

static err_t FOG_CDECL Hash_StringW_Var_removeStringW(HashUntyped* self, const StringW* key)
{
  return fog_api.hash_stringw_unknown_removeStringW(self, &Hash_StringW_Var_vTable, key);
}

static void FOG_CDECL Hash_StringW_Var_dFree(HashUntypedData* d)
{
  fog_api.hash_unknown_unknown_dFree(d, &Hash_StringW_Var_vTable);
}

// ============================================================================
// [Fog::HashIterator<?, ?>]
// ============================================================================

static bool FOG_CDECL HashIterator_start(HashUntypedIterator* i)
{
  FOG_ASSERT(i->_container != NULL);
  HashUntyped* self = i->_container;

  if (self == NULL)
    return false;

  HashUntypedData* d = self->_d;
  HashUntypedNode* node = NULL;

  if (d->length == 0)
  {
    i->_node = NULL;
    i->_index = INVALID_INDEX;
    return false;
  }

  HashUntypedNode** nodeList = d->data;

  size_t capacity = d->capacity;
  size_t index = 0;

  do {
    node = nodeList[index];
    if (node != NULL) goto _Match;
  } while (++index < capacity);

  // We checked the length so this is impossible.
  FOG_ASSERT(false);

  i->_node = NULL;
  i->_index = INVALID_INDEX;
  return false;

_Match:
  i->_node = node;
  i->_index = index;
  return true;
}

static bool FOG_CDECL HashIterator_next(HashUntypedIterator* i)
{
  FOG_ASSERT(i->_container != NULL);

  // It is possible that the node is NULL. In this case the element was removed
  // from the Hash and there is no previous one to assign. Instead of traversing
  // back we just set it to NULL.
  HashUntypedNode* node = i->_node;
  if (node != NULL && node->next != NULL)
  {
    i->_node = node->next;
    return true;
  }

  HashUntyped* self = i->_container;
  HashUntypedData* d = self->_d;

  HashUntypedNode** nodeList = d->data;

  size_t capacity = d->capacity;
  size_t index = i->_index;

  while (++index < capacity)
  {
    node = nodeList[index];
    if (node != NULL) goto _Match;
  }

  // End.
  i->_node = NULL;
  i->_index = INVALID_INDEX;
  return false;

_Match:
  i->_node = node;
  i->_index = index;
  return true;
}

// ===========================================================================
// [Init / Fini]
// ===========================================================================

FOG_NO_EXPORT void Hash_init(void)
{
  size_t firstPrime = HashUtil::getClosestPrime(0);

  // -------------------------------------------------------------------------
  // [Funcs]
  // -------------------------------------------------------------------------

  fog_api.hashhelper_calcExpandCapacity = Hash_calcExpandCapacity;
  fog_api.hashhelper_calcShrinkCapacity = Hash_calcShrinkCapacity;

  // Hash<?, ?>
  fog_api.hash_unknown_unknown_ctor = Hash_Unknown_Unknown_ctor;
  fog_api.hash_unknown_unknown_ctorCopy = Hash_Unknown_Unknown_ctorCopy;
  fog_api.hash_unknown_unknown_dtor = Hash_Unknown_Unknown_dtor;

  fog_api.hash_unknown_unknown_detach = Hash_Unknown_Unknown_detach;
  fog_api.hash_unknown_unknown_rehash = Hash_Unknown_Unknown_rehash;
  fog_api.hash_unknown_unknown_reserve = Hash_Unknown_Unknown_reserve;
  fog_api.hash_unknown_unknown_squeeze = Hash_Unknown_Unknown_squeeze;

  fog_api.hash_unknown_unknown_clear = Hash_Unknown_Unknown_clear;
  fog_api.hash_unknown_unknown_reset = Hash_Unknown_Unknown_reset;

  fog_api.hash_unknown_unknown_get = Hash_Unknown_Unknown_get;
  fog_api.hash_unknown_unknown_use = Hash_Unknown_Unknown_use;
  fog_api.hash_unknown_unknown_put = Hash_Unknown_Unknown_put;
  fog_api.hash_unknown_unknown_remove = Hash_Unknown_Unknown_remove;

  fog_api.hash_unknown_unknown_copy = Hash_Unknown_Unknown_copy;
  fog_api.hash_unknown_unknown_eq = Hash_Unknown_Unknown_eq;

  fog_api.hash_unknown_unknown_dCreate = Hash_Unknown_Unknown_dCreate;
  fog_api.hash_unknown_unknown_dFree = Hash_Unknown_Unknown_dFree;

  // Hash<int32_t, ?>
  fog_api.hash_int32_unknown_get;
  fog_api.hash_int32_unknown_use;
  fog_api.hash_int32_unknown_put;
  fog_api.hash_int32_unknown_remove;

  // Hash<int64_t, ?>
  fog_api.hash_int64_unknown_get;
  fog_api.hash_int64_unknown_use;
  fog_api.hash_int64_unknown_put;
  fog_api.hash_int64_unknown_remove;

  // Hash<StringA, ?>
  fog_api.hash_stringa_unknown_getStubA = Hash_StringT_Unknown_getStub<char, char>;
  fog_api.hash_stringa_unknown_getStringA = Hash_StringT_Unknown_getString<char, char>;

  fog_api.hash_stringa_unknown_useStubA = Hash_StringT_Unknown_useStub<char, char>;
  fog_api.hash_stringa_unknown_useStringA = Hash_StringT_Unknown_useString<char, char>;

  fog_api.hash_stringa_unknown_putStubA = Hash_StringT_Unknown_putStub<char, char>;
  fog_api.hash_stringa_unknown_putStringA = Hash_StringT_Unknown_putString<char, char>;

  fog_api.hash_stringa_unknown_removeStubA = Hash_StringT_Unknown_removeStub<char, char>;
  fog_api.hash_stringa_unknown_removeStringA = Hash_StringT_Unknown_removeString<char, char>;

  fog_api.hash_stringa_unknown_eq = Hash_StringT_Unknown_eq<char>;

  // Hash<StringA, StringA>
  fog_api.hash_stringa_stringa_ctor = Hash_StringA_StringA_ctor;
  fog_api.hash_stringa_stringa_dtor = Hash_StringA_StringA_dtor;

  fog_api.hash_stringa_stringa_getStubA = Hash_StringA_StringA_getStubA;
  fog_api.hash_stringa_stringa_getStringA = Hash_StringA_StringA_getStringA;

  fog_api.hash_stringa_stringa_useStubA = Hash_StringA_StringA_useStubA;
  fog_api.hash_stringa_stringa_useStringA = Hash_StringA_StringA_useStringA;

  fog_api.hash_stringa_stringa_putStubA = Hash_StringA_StringA_putStubA;
  fog_api.hash_stringa_stringa_putStringA = Hash_StringA_StringA_putStringA;

  fog_api.hash_stringa_stringa_removeStubA = Hash_StringA_StringA_removeStubA;
  fog_api.hash_stringa_stringa_removeStringA = Hash_StringA_StringA_removeStringA;

  fog_api.hash_stringa_stringa_dFree = Hash_StringA_StringA_dFree;

  // Hash<StringA, Var>
  fog_api.hash_stringa_var_ctor = Hash_StringA_Var_ctor;
  fog_api.hash_stringa_var_dtor = Hash_StringA_Var_dtor;

  fog_api.hash_stringa_var_getStubA = Hash_StringA_Var_getStubA;
  fog_api.hash_stringa_var_getStringA = Hash_StringA_Var_getStringA;

  fog_api.hash_stringa_var_useStubA = Hash_StringA_Var_useStubA;
  fog_api.hash_stringa_var_useStringA = Hash_StringA_Var_useStringA;

  fog_api.hash_stringa_var_putStubA = Hash_StringA_Var_putStubA;
  fog_api.hash_stringa_var_putStringA = Hash_StringA_Var_putStringA;

  fog_api.hash_stringa_var_removeStubA = Hash_StringA_Var_removeStubA;
  fog_api.hash_stringa_var_removeStringA = Hash_StringA_Var_removeStringA;

  fog_api.hash_stringa_var_dFree = Hash_StringA_Var_dFree;

  // Hash<StringW, ?>
  fog_api.hash_stringw_unknown_getStubA = Hash_StringT_Unknown_getStub<CharW, char>;
  fog_api.hash_stringw_unknown_getStubW = Hash_StringT_Unknown_getStub<CharW, CharW>;
  fog_api.hash_stringw_unknown_getStringW = Hash_StringT_Unknown_getString<CharW, CharW>;

  fog_api.hash_stringw_unknown_useStubA = Hash_StringT_Unknown_useStub<CharW, char>;
  fog_api.hash_stringw_unknown_useStubW = Hash_StringT_Unknown_useStub<CharW, CharW>;
  fog_api.hash_stringw_unknown_useStringW = Hash_StringT_Unknown_useString<CharW, CharW>;

  fog_api.hash_stringw_unknown_putStubA = Hash_StringT_Unknown_putStub<CharW, char>;
  fog_api.hash_stringw_unknown_putStubW = Hash_StringT_Unknown_putStub<CharW, CharW>;
  fog_api.hash_stringw_unknown_putStringW = Hash_StringT_Unknown_putString<CharW, CharW>;

  fog_api.hash_stringw_unknown_removeStubA = Hash_StringT_Unknown_removeStub<CharW, char>;
  fog_api.hash_stringw_unknown_removeStubW = Hash_StringT_Unknown_removeStub<CharW, CharW>;
  fog_api.hash_stringw_unknown_removeStringW = Hash_StringT_Unknown_removeString<CharW, CharW>;

  fog_api.hash_stringw_unknown_eq = Hash_StringT_Unknown_eq<CharW>;

  // Hash<StringW, StringW>
  fog_api.hash_stringw_stringw_ctor = Hash_StringW_StringW_ctor;
  fog_api.hash_stringw_stringw_dtor = Hash_StringW_StringW_dtor;

  fog_api.hash_stringw_stringw_getStubA = Hash_StringW_StringW_getStubA;
  fog_api.hash_stringw_stringw_getStubW = Hash_StringW_StringW_getStubW;
  fog_api.hash_stringw_stringw_getStringW = Hash_StringW_StringW_getStringW;

  fog_api.hash_stringw_stringw_useStubA = Hash_StringW_StringW_useStubA;
  fog_api.hash_stringw_stringw_useStubW = Hash_StringW_StringW_useStubW;
  fog_api.hash_stringw_stringw_useStringW = Hash_StringW_StringW_useStringW;

  fog_api.hash_stringw_stringw_putStubA = Hash_StringW_StringW_putStubA;
  fog_api.hash_stringw_stringw_putStubW = Hash_StringW_StringW_putStubW;
  fog_api.hash_stringw_stringw_putStringW = Hash_StringW_StringW_putStringW;

  fog_api.hash_stringw_stringw_removeStubA = Hash_StringW_StringW_removeStubA;
  fog_api.hash_stringw_stringw_removeStubW = Hash_StringW_StringW_removeStubW;
  fog_api.hash_stringw_stringw_removeStringW = Hash_StringW_StringW_removeStringW;

  fog_api.hash_stringw_stringw_dFree = Hash_StringW_StringW_dFree;

  // Hash<StringW, Var>
  fog_api.hash_stringw_var_ctor = Hash_StringW_Var_ctor;
  fog_api.hash_stringw_var_dtor = Hash_StringW_Var_dtor;

  fog_api.hash_stringw_var_getStubA = Hash_StringW_Var_getStubA;
  fog_api.hash_stringw_var_getStubW = Hash_StringW_Var_getStubW;
  fog_api.hash_stringw_var_getStringW = Hash_StringW_Var_getStringW;

  fog_api.hash_stringw_var_useStubA = Hash_StringW_Var_useStubA;
  fog_api.hash_stringw_var_useStubW = Hash_StringW_Var_useStubW;
  fog_api.hash_stringw_var_useStringW = Hash_StringW_Var_useStringW;

  fog_api.hash_stringw_var_putStubA = Hash_StringW_Var_putStubA;
  fog_api.hash_stringw_var_putStubW = Hash_StringW_Var_putStubW;
  fog_api.hash_stringw_var_putStringW = Hash_StringW_Var_putStringW;

  fog_api.hash_stringw_var_removeStubA = Hash_StringW_Var_removeStubA;
  fog_api.hash_stringw_var_removeStubW = Hash_StringW_Var_removeStubW;
  fog_api.hash_stringw_var_removeStringW = Hash_StringW_Var_removeStringW;

  fog_api.hash_stringw_var_dFree = Hash_StringW_Var_dFree;

  // HashIterator<?, ?>
  fog_api.hashiterator_start = HashIterator_start;
  fog_api.hashiterator_next = HashIterator_next;

  // -------------------------------------------------------------------------
  // [Data]
  // -------------------------------------------------------------------------

  HashUntypedData* d;

  // Hash<?, ?>
  d = &Hash_Unknown_Unknown_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_UNKNOWN | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandCapacity = HashUtil::getClosestPrime(0);
  d->expandLength = 1;

  Hash_Unknown_Unknown_oEmpty->_d = d;
  fog_api.hash_unknown_unknown_oEmpty = &Hash_Unknown_Unknown_oEmpty;

  // Hash<StringA, StringA>
  d = &Hash_StringA_StringA_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_HASH_STRINGA_STRINGA | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandCapacity = firstPrime;
  d->expandLength = 1;

  Hash_StringA_StringA_oEmpty->_d = d;
  fog_api.hash_stringa_stringa_oEmpty = &Hash_StringA_StringA_oEmpty;

  // HashVTable<StringA, StringA>
  Hash_StringA_StringA_vTable->idxKeyT = FOG_OFFSET_OF(HashKeyNode<StringA>, key);
  Hash_StringA_StringA_vTable->idxItemT = FOG_OFFSET_OF(HashKeyNode<StringA>, key) + sizeof(StringA);
  Hash_StringA_StringA_vTable->szKeyT = sizeof(StringA);
  Hash_StringA_StringA_vTable->szItemT = sizeof(StringA);
  Hash_StringA_StringA_vTable->ctor = Hash_StringA_StringA_Node_ctor;
  Hash_StringA_StringA_vTable->dtor = Hash_StringA_StringA_Node_dtor;
  Hash_StringA_StringA_vTable->setItem = (HashUntypedVTable::SetItem)fog_api.stringa_setStringA;
  Hash_StringA_StringA_vTable->hashKey = (HashFunc)fog_api.stringa_getHashCode;
  Hash_StringA_StringA_vTable->eqKey = (EqFunc)fog_api.stringa_eqStringA;
  fog_api.hash_stringa_stringa_vTable = &Hash_StringA_StringA_vTable;

  // Hash<StringA, Var>
  d = &Hash_StringW_Var_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_HASH_STRINGA_VAR | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandCapacity = firstPrime;
  d->expandLength = 1;

  Hash_StringA_Var_oEmpty->_d = d;
  fog_api.hash_stringa_var_oEmpty = &Hash_StringA_Var_oEmpty;

  // HashVTable<StringA, Var>
  Hash_StringA_Var_vTable->idxKeyT = FOG_OFFSET_OF(HashKeyNode<StringA>, key);
  Hash_StringA_Var_vTable->idxItemT = FOG_OFFSET_OF(HashKeyNode<StringA>, key) + sizeof(StringA);
  Hash_StringA_Var_vTable->szKeyT = sizeof(StringA);
  Hash_StringA_Var_vTable->szItemT = sizeof(Var);
  Hash_StringA_Var_vTable->ctor = Hash_StringA_Var_Node_ctor;
  Hash_StringA_Var_vTable->dtor = Hash_StringA_Var_Node_dtor;
  Hash_StringA_Var_vTable->setItem = (HashUntypedVTable::SetItem)fog_api.var_copy;
  Hash_StringA_Var_vTable->hashKey = (HashFunc)fog_api.stringa_getHashCode;
  Hash_StringA_Var_vTable->eqKey = (EqFunc)fog_api.stringa_eqStringA;
  fog_api.hash_stringa_var_vTable = &Hash_StringA_Var_vTable;

  // Hash<StringW, StringW>
  d = &Hash_StringW_StringW_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_HASH_STRINGW_STRINGW | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandCapacity = firstPrime;
  d->expandLength = 1;

  Hash_StringW_StringW_oEmpty->_d = d;
  fog_api.hash_stringw_stringw_oEmpty = &Hash_StringW_StringW_oEmpty;

  // HashVTable<StringW, StringW>
  Hash_StringW_StringW_vTable->idxKeyT = FOG_OFFSET_OF(HashKeyNode<StringW>, key);
  Hash_StringW_StringW_vTable->idxItemT = FOG_OFFSET_OF(HashKeyNode<StringW>, key) + sizeof(StringW);
  Hash_StringW_StringW_vTable->szKeyT = sizeof(StringW);
  Hash_StringW_StringW_vTable->szItemT = sizeof(StringW);
  Hash_StringW_StringW_vTable->ctor = Hash_StringW_StringW_Node_ctor;
  Hash_StringW_StringW_vTable->dtor = Hash_StringW_StringW_Node_dtor;
  Hash_StringW_StringW_vTable->setItem = (HashUntypedVTable::SetItem)fog_api.stringw_setStringW;
  Hash_StringW_StringW_vTable->hashKey = (HashFunc)fog_api.stringw_getHashCode;
  Hash_StringW_StringW_vTable->eqKey = (EqFunc)fog_api.stringw_eqStringW;
  fog_api.hash_stringw_stringw_vTable = &Hash_StringW_StringW_vTable;

  // Hash<StringW, Var>
  d = &Hash_StringW_Var_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_HASH_STRINGW_VAR | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandCapacity = firstPrime;
  d->expandLength = 1;

  Hash_StringW_Var_oEmpty->_d = d;
  fog_api.hash_stringw_var_oEmpty = &Hash_StringW_Var_oEmpty;

  // HashVTable<StringW, Var>
  Hash_StringW_Var_vTable->idxKeyT = FOG_OFFSET_OF(HashKeyNode<StringW>, key);
  Hash_StringW_Var_vTable->idxItemT = FOG_OFFSET_OF(HashKeyNode<StringW>, key) + sizeof(StringW);
  Hash_StringW_Var_vTable->szKeyT = sizeof(StringW);
  Hash_StringW_Var_vTable->szItemT = sizeof(Var);
  Hash_StringW_Var_vTable->ctor = Hash_StringW_Var_Node_ctor;
  Hash_StringW_Var_vTable->dtor = Hash_StringW_Var_Node_dtor;
  Hash_StringW_Var_vTable->setItem = (HashUntypedVTable::SetItem)fog_api.var_copy;
  Hash_StringW_Var_vTable->hashKey = (HashFunc)fog_api.stringw_getHashCode;
  Hash_StringW_Var_vTable->eqKey = (EqFunc)fog_api.stringw_eqStringW;
  fog_api.hash_stringa_var_vTable = &Hash_StringW_Var_vTable;
}

} // Fog namespace
