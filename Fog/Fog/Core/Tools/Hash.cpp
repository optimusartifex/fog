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

static const uint32_t Hash_primeTable[] =
{
  7,
  17,
  31,
  47,
  83,
  107,
  157,
  211,
  353,
  509,
  769,
  997,
  1217,
  1567,
  2053,
  3011,
  4337,
  5939,
  7757,
  9973,
  13033,
  17783,
  23417,
  31517,
  37991,
  45161,
  50929,
  60637,
  71261,
  85469,
  95789,
  110273,
  131357,
  152287,
  175327,
  202777,
  238649,
  274019,
  310091,
  327869,
  354383,
  384437,
  410453,
  444443,
  488879,
  522229,
  555557,
  589759,
  626009,
  666989,
  708251,
  752797,
  802499,
  855601,
  939007,
  1007497,
  1155851,
  1333357,
  1553099,
  1815361,
  2554477,
  3557441,
  4981877,
  6977491,
  9854011,
  13289083,
  15289117,
  19709071,
  23025773,
  26725759,
  30534103,
  37534097,
  42555229,
  48555223,
  52555219,
  61555237,
  73698109,
  85698133,
  104395303,
  114395317,
  124395317,
  134395351,
  141650939,
  151150943,
  165150127,
  179424691,
  195424711,
  217645177,
  232645211,
  259123471,
  295075153,
  335075159,
  393342739,
  452876863,
  533910137,
  663910123,
  838041647,
  982451653
};

static uint32_t Hash_getClosestPrimeIndex(size_t value)
{
  uint32_t i = 0;

  // Optimize lookup in case that the value is too large.
  if (Hash_primeTable[64] < value)
    i = 64;
  else if (Hash_primeTable[48] < value)
    i = 48;
  else if (Hash_primeTable[32] < value)
    i = 32;
  else if (Hash_primeTable[16] < value)
    i = 16;

  while (i < FOG_ARRAY_SIZE(Hash_primeTable))
  {
    if (Hash_primeTable[i] >= value)
      return i;
    i++;
  }

  // Return the last one.
  return i - 1;
}

static size_t Hash_getClosestPrime(size_t value)
{
  return Hash_primeTable[Hash_getClosestPrimeIndex(value)];
}

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

  uint32_t index = Hash_getClosestPrimeIndex(capacity);
  return Hash_primeTable[index];
}

static size_t FOG_CDECL Hash_calcShrinkCapacity(size_t capacity)
{
  static const size_t threshold = 1024*1024*4;

  capacity /= 2;

  uint32_t index = Hash_getClosestPrimeIndex(capacity);
  if (index > 0) index--;

  return Hash_primeTable[index];
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

  if (d->reference.deref())
    _api.hash.unknown_unknown.dFree(d, v);
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

  HashUntypedData* newd = _api.hash.unknown_unknown.dCreate(newCapacity);
  if (FOG_IS_NULL(newd))
  {
    return ERR_RT_OUT_OF_MEMORY;
  }

  size_t szNode = Hash_alignNodeSize(v->idxItemT + v->szItemT);
  err_t err = newd->nodePool.prealloc(szNode, length);

  if (FOG_IS_ERROR(err))
  {
    _api.hash.unknown_unknown.dFree(newd, v);
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
    _api.hash.unknown_unknown.dFree(d, v);

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
  capacity = Hash_getClosestPrime(capacity);

  if (d->reference.get() != 1 || d->capacity < capacity)
    return _api.hash.unknown_unknown.rehash(self, v, capacity);
  else
    return ERR_OK;
}

static void FOG_CDECL Hash_Unknown_Unknown_squeeze(HashUntyped* self, const HashUntypedVTable* v)
{
  HashUntypedData* d = self->_d;
  size_t optimal = Hash_getClosestPrime(d->length);

  if (optimal < d->capacity)
    _api.hash.unknown_unknown.rehash(self, v, optimal);
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
    _api.hash.unknown_unknown.reset(self, v);
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
    _api.hash.unknown_unknown.dFree(d, v);
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

  if (_api.hash.unknown_unknown.detach(self, v) != ERR_OK)
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
    FOG_RETURN_ON_ERROR(_api.hash.unknown_unknown.reserve(self, v, Hash_primeTable[0]));
    d = self->_d;
  }
  else if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.hash.unknown_unknown.detach(self, v));
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
    _api.hash.unknown_unknown.rehash(self, v, d->expandCapacity);
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
  if (d->reference.get() != 1 || d->length - 1<= d->shrinkLength)
  {
    size_t capacity = Hash_getClosestPrime(d->length - 1);
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
    _api.hash.unknown_unknown.dFree(d, v);
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

  size_t expand = _api.hash.helper.calcExpandCapacity(capacity);
  size_t shrink = _api.hash.helper.calcShrinkCapacity(capacity);

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

  if (_api.hash.unknown_unknown.detach(self, v) != ERR_OK)
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

  if (_api.hash.unknown_unknown.detach(self, v) != ERR_OK)
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
    FOG_RETURN_ON_ERROR(_api.hash.unknown_unknown.reserve(self, v, Hash_primeTable[0]));
    d = self->_d;
  }
  else if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.hash.unknown_unknown.detach(self, v));
    d = self->_d;
  }

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getComputedLength();

  uint32_t hashCode = HashUtil::hash<SrcT_(Stub)>(SrcT_(Stub)(keyData, keyLength));
  uint32_t hashMod = hashCode % d->capacity;

  HashKeyNode<CharT_(String)>** pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&d->data[hashMod]);
  HashKeyNode<CharT_(String)>* node = *pPrev;

  size_t idxKey = v->idxKeyT;
  EqFunc eqKey = v->eqKey;

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
    _api.hash.unknown_unknown.rehash(self, v, d->expandCapacity);
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
    FOG_RETURN_ON_ERROR(_api.hash.unknown_unknown.reserve(self, v, Hash_primeTable[0]));
    d = self->_d;
  }
  else if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(_api.hash.unknown_unknown.detach(self, v));
    d = self->_d;
  }

  const SrcT* keyData = _key->getData();
  size_t keyLength = _key->getLength();

  uint32_t hashCode = _key->getHashCode();
  uint32_t hashMod = hashCode % d->capacity;

  HashKeyNode<CharT_(String)>** pPrev = reinterpret_cast<HashKeyNode<CharT_(String)>**>(&d->data[hashMod]);
  HashKeyNode<CharT_(String)>* node = *pPrev;

  size_t idxKey = v->idxKeyT;
  EqFunc eqKey = v->eqKey;

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
    _api.hash.unknown_unknown.rehash(self, v, d->expandCapacity);
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

  size_t idxKey = v->idxKeyT;
  EqFunc eqKey = v->eqKey;

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
    size_t capacity = Hash_getClosestPrime(d->length - 1);
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

  size_t idxKey = v->idxKeyT;
  EqFunc eqKey = v->eqKey;

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
    size_t capacity = Hash_getClosestPrime(d->length - 1);
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
  if (d->reference.deref())
    return _api.hash.unknown_unknown.dFree(d, &Hash_StringA_StringA_vTable);
}

static const StringA* FOG_CDECL Hash_StringA_StringA_getStubA(const HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<const StringA*>(
    _api.hash.stringa_unknown.getStubA(self, &Hash_StringA_StringA_vTable, key));
}

static const StringA* FOG_CDECL Hash_StringA_StringA_getStringA(const HashUntyped* self, const StringA* key)
{
  return reinterpret_cast<const StringA*>(
    _api.hash.stringa_unknown.getStringA(self, &Hash_StringA_StringA_vTable, key));
}

static StringA* FOG_CDECL Hash_StringA_StringA_useStubA(HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<StringA*>(
    _api.hash.stringa_unknown.useStubA(self, &Hash_StringA_StringA_vTable, key));
}

static StringA* FOG_CDECL Hash_StringA_StringA_useStringA(HashUntyped* self, const StringA* key)
{
  return reinterpret_cast<StringA*>(
    _api.hash.stringa_unknown.useStringA(self, &Hash_StringA_StringA_vTable, key));
}

static err_t FOG_CDECL Hash_StringA_StringA_putStubA(HashUntyped* self, const StubA* key, const StringA* item, bool replace)
{
  return _api.hash.stringa_unknown.putStubA(self, &Hash_StringA_StringA_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringA_StringA_putStringA(HashUntyped* self, const StringA* key, const StringA* item, bool replace)
{
  return _api.hash.stringa_unknown.putStringA(self, &Hash_StringA_StringA_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringA_StringA_removeStubA(HashUntyped* self, const StubA* key)
{
  return _api.hash.stringa_unknown.removeStubA(self, &Hash_StringA_StringA_vTable, key);
}

static err_t FOG_CDECL Hash_StringA_StringA_removeStringA(HashUntyped* self, const StringA* key)
{
  return _api.hash.stringa_unknown.removeStringA(self, &Hash_StringA_StringA_vTable, key);
}

static void FOG_CDECL Hash_StringA_StringA_dFree(HashUntypedData* d)
{
  return _api.hash.unknown_unknown.dFree(d, &Hash_StringA_StringA_vTable);
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
  if (d->reference.deref())
    return _api.hash.unknown_unknown.dFree(d, &Hash_StringA_Var_vTable);
}

static const Var* FOG_CDECL Hash_StringA_Var_getStubA(const HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<const Var*>(
    _api.hash.stringa_unknown.getStubA(self, &Hash_StringA_Var_vTable, key));
}

static const Var* FOG_CDECL Hash_StringA_Var_getStringA(const HashUntyped* self, const StringA* key)
{
  return reinterpret_cast<const Var*>(
    _api.hash.stringa_unknown.getStringA(self, &Hash_StringA_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringA_Var_useStubA(HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<Var*>(
    _api.hash.stringa_unknown.useStubA(self, &Hash_StringA_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringA_Var_useStringA(HashUntyped* self, const StringA* key)
{
  return reinterpret_cast<Var*>(
    _api.hash.stringa_unknown.useStringA(self, &Hash_StringA_Var_vTable, key));
}

static err_t FOG_CDECL Hash_StringA_Var_putStubA(HashUntyped* self, const StubA* key, const Var* item, bool replace)
{
  return _api.hash.stringa_unknown.putStubA(self, &Hash_StringA_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringA_Var_putStringA(HashUntyped* self, const StringA* key, const Var* item, bool replace)
{
  return _api.hash.stringa_unknown.putStringA(self, &Hash_StringA_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringA_Var_removeStubA(HashUntyped* self, const StubA* key)
{
  return _api.hash.stringa_unknown.removeStubA(self, &Hash_StringA_Var_vTable, key);
}

static err_t FOG_CDECL Hash_StringA_Var_removeStringA(HashUntyped* self, const StringA* key)
{
  return _api.hash.stringa_unknown.removeStringA(self, &Hash_StringA_Var_vTable, key);
}

static void FOG_CDECL Hash_StringA_Var_dFree(HashUntypedData* d)
{
  return _api.hash.unknown_unknown.dFree(d, &Hash_StringA_Var_vTable);
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
  if (d->reference.deref())
    return _api.hash.unknown_unknown.dFree(d, &Hash_StringW_StringW_vTable);
}

static const StringW* FOG_CDECL Hash_StringW_StringW_getStubA(const HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<const StringW*>(
    _api.hash.stringw_unknown.getStubA(self, &Hash_StringW_StringW_vTable, key));
}

static const StringW* FOG_CDECL Hash_StringW_StringW_getStubW(const HashUntyped* self, const StubW* key)
{
  return reinterpret_cast<const StringW*>(
    _api.hash.stringw_unknown.getStubW(self, &Hash_StringW_StringW_vTable, key));
}

static const StringW* FOG_CDECL Hash_StringW_StringW_getStringW(const HashUntyped* self, const StringW* key)
{
  return reinterpret_cast<const StringW*>(
    _api.hash.stringw_unknown.getStringW(self, &Hash_StringW_StringW_vTable, key));
}

static StringW* FOG_CDECL Hash_StringW_StringW_useStubA(HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<StringW*>(
    _api.hash.stringw_unknown.useStubA(self, &Hash_StringW_StringW_vTable, key));
}

static StringW* FOG_CDECL Hash_StringW_StringW_useStubW(HashUntyped* self, const StubW* key)
{
  return reinterpret_cast<StringW*>(
    _api.hash.stringw_unknown.useStubW(self, &Hash_StringW_StringW_vTable, key));
}

static StringW* FOG_CDECL Hash_StringW_StringW_useStringW(HashUntyped* self, const StringW* key)
{
  return reinterpret_cast<StringW*>(
    _api.hash.stringw_unknown.useStringW(self, &Hash_StringW_StringW_vTable, key));
}

static err_t FOG_CDECL Hash_StringW_StringW_putStubA(HashUntyped* self, const StubA* key, const StringW* item, bool replace)
{
  return _api.hash.stringw_unknown.putStubA(self, &Hash_StringW_StringW_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_StringW_putStubW(HashUntyped* self, const StubW* key, const StringW* item, bool replace)
{
  return _api.hash.stringw_unknown.putStubW(self, &Hash_StringW_StringW_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_StringW_putStringW(HashUntyped* self, const StringW* key, const StringW* item, bool replace)
{
  return _api.hash.stringw_unknown.putStringW(self, &Hash_StringW_StringW_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_StringW_removeStubA(HashUntyped* self, const StubA* key)
{
  return _api.hash.stringw_unknown.removeStubA(self, &Hash_StringW_StringW_vTable, key);
}

static err_t FOG_CDECL Hash_StringW_StringW_removeStubW(HashUntyped* self, const StubW* key)
{
  return _api.hash.stringw_unknown.removeStubW(self, &Hash_StringW_StringW_vTable, key);
}

static err_t FOG_CDECL Hash_StringW_StringW_removeStringW(HashUntyped* self, const StringW* key)
{
  return _api.hash.stringw_unknown.removeStringW(self, &Hash_StringW_StringW_vTable, key);
}

static void FOG_CDECL Hash_StringW_StringW_dFree(HashUntypedData* d)
{
  return _api.hash.unknown_unknown.dFree(d, &Hash_StringW_StringW_vTable);
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
  if (d->reference.deref())
    return _api.hash.unknown_unknown.dFree(d, &Hash_StringW_Var_vTable);
}

static const Var* FOG_CDECL Hash_StringW_Var_getStubA(const HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<const Var*>(
    _api.hash.stringw_unknown.getStubA(self, &Hash_StringW_Var_vTable, key));
}

static const Var* FOG_CDECL Hash_StringW_Var_getStubW(const HashUntyped* self, const StubW* key)
{
  return reinterpret_cast<const Var*>(
    _api.hash.stringw_unknown.getStubW(self, &Hash_StringW_Var_vTable, key));
}

static const Var* FOG_CDECL Hash_StringW_Var_getStringW(const HashUntyped* self, const StringW* key)
{
  return reinterpret_cast<const Var*>(
    _api.hash.stringw_unknown.getStringW(self, &Hash_StringW_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringW_Var_useStubA(HashUntyped* self, const StubA* key)
{
  return reinterpret_cast<Var*>(
    _api.hash.stringw_unknown.useStubA(self, &Hash_StringW_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringW_Var_useStubW(HashUntyped* self, const StubW* key)
{
  return reinterpret_cast<Var*>(
    _api.hash.stringw_unknown.useStubW(self, &Hash_StringW_Var_vTable, key));
}

static Var* FOG_CDECL Hash_StringW_Var_useStringW(HashUntyped* self, const StringW* key)
{
  return reinterpret_cast<Var*>(
    _api.hash.stringw_unknown.useStringW(self, &Hash_StringW_Var_vTable, key));
}

static err_t FOG_CDECL Hash_StringW_Var_putStubA(HashUntyped* self, const StubA* key, const Var* item, bool replace)
{
  return _api.hash.stringw_unknown.putStubA(self, &Hash_StringW_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_Var_putStubW(HashUntyped* self, const StubW* key, const Var* item, bool replace)
{
  return _api.hash.stringw_unknown.putStubW(self, &Hash_StringW_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_Var_putStringW(HashUntyped* self, const StringW* key, const Var* item, bool replace)
{
  return _api.hash.stringw_unknown.putStringW(self, &Hash_StringW_Var_vTable, key, item, replace);
}

static err_t FOG_CDECL Hash_StringW_Var_removeStubA(HashUntyped* self, const StubA* key)
{
  return _api.hash.stringw_unknown.removeStubA(self, &Hash_StringW_Var_vTable, key);
}

static err_t FOG_CDECL Hash_StringW_Var_removeStubW(HashUntyped* self, const StubW* key)
{
  return _api.hash.stringw_unknown.removeStubW(self, &Hash_StringW_Var_vTable, key);
}

static err_t FOG_CDECL Hash_StringW_Var_removeStringW(HashUntyped* self, const StringW* key)
{
  return _api.hash.stringw_unknown.removeStringW(self, &Hash_StringW_Var_vTable, key);
}

static void FOG_CDECL Hash_StringW_Var_dFree(HashUntypedData* d)
{
  return _api.hash.unknown_unknown.dFree(d, &Hash_StringW_Var_vTable);
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
  // -------------------------------------------------------------------------
  // [Funcs]
  // -------------------------------------------------------------------------

  _api.hash.helper.calcExpandCapacity = Hash_calcExpandCapacity;
  _api.hash.helper.calcShrinkCapacity = Hash_calcShrinkCapacity;

  // Hash<?, ?>
  _api.hash.unknown_unknown.ctor = Hash_Unknown_Unknown_ctor;
  _api.hash.unknown_unknown.ctorCopy = Hash_Unknown_Unknown_ctorCopy;
  _api.hash.unknown_unknown.dtor = Hash_Unknown_Unknown_dtor;
  
  _api.hash.unknown_unknown.detach = Hash_Unknown_Unknown_detach;
  _api.hash.unknown_unknown.rehash = Hash_Unknown_Unknown_rehash;
  _api.hash.unknown_unknown.reserve = Hash_Unknown_Unknown_reserve;
  _api.hash.unknown_unknown.squeeze = Hash_Unknown_Unknown_squeeze;
  
  _api.hash.unknown_unknown.clear = Hash_Unknown_Unknown_clear;
  _api.hash.unknown_unknown.reset = Hash_Unknown_Unknown_reset;
  
  _api.hash.unknown_unknown.get = Hash_Unknown_Unknown_get;
  _api.hash.unknown_unknown.use = Hash_Unknown_Unknown_use;
  _api.hash.unknown_unknown.put = Hash_Unknown_Unknown_put;
  _api.hash.unknown_unknown.remove = Hash_Unknown_Unknown_remove;

  _api.hash.unknown_unknown.copy = Hash_Unknown_Unknown_copy;

  _api.hash.unknown_unknown.dCreate = Hash_Unknown_Unknown_dCreate;
  _api.hash.unknown_unknown.dFree = Hash_Unknown_Unknown_dFree;

  // Hash<int32_t, ?>
  _api.hash.int32_unknown.get;
  _api.hash.int32_unknown.use;
  _api.hash.int32_unknown.put;
  _api.hash.int32_unknown.remove;

  // Hash<int64_t, ?>
  _api.hash.int64_unknown.get;
  _api.hash.int64_unknown.use;
  _api.hash.int64_unknown.put;
  _api.hash.int64_unknown.remove;

  // Hash<StringA, ?>
  _api.hash.stringa_unknown.getStubA = Hash_StringT_Unknown_getStub<char, char>;
  _api.hash.stringa_unknown.getStringA = Hash_StringT_Unknown_getString<char, char>;

  _api.hash.stringa_unknown.useStubA = Hash_StringT_Unknown_useStub<char, char>;
  _api.hash.stringa_unknown.useStringA = Hash_StringT_Unknown_useString<char, char>;

  _api.hash.stringa_unknown.putStubA = Hash_StringT_Unknown_putStub<char, char>;
  _api.hash.stringa_unknown.putStringA = Hash_StringT_Unknown_putString<char, char>;

  _api.hash.stringa_unknown.removeStubA = Hash_StringT_Unknown_removeStub<char, char>;
  _api.hash.stringa_unknown.removeStringA = Hash_StringT_Unknown_removeString<char, char>;

  // Hash<StringA, StringA>
  _api.hash.stringa_stringa.ctor = Hash_StringA_StringA_ctor;
  _api.hash.stringa_stringa.dtor = Hash_StringA_StringA_dtor;

  _api.hash.stringa_stringa.getStubA = Hash_StringA_StringA_getStubA;
  _api.hash.stringa_stringa.getStringA = Hash_StringA_StringA_getStringA;
    
  _api.hash.stringa_stringa.useStubA = Hash_StringA_StringA_useStubA;
  _api.hash.stringa_stringa.useStringA = Hash_StringA_StringA_useStringA;
    
  _api.hash.stringa_stringa.putStubA = Hash_StringA_StringA_putStubA;
  _api.hash.stringa_stringa.putStringA = Hash_StringA_StringA_putStringA;
    
  _api.hash.stringa_stringa.removeStubA = Hash_StringA_StringA_removeStubA;
  _api.hash.stringa_stringa.removeStringA = Hash_StringA_StringA_removeStringA;

  _api.hash.stringa_stringa.dFree = Hash_StringA_StringA_dFree;

  // Hash<StringA, Var>
  _api.hash.stringa_var.ctor = Hash_StringA_Var_ctor;
  _api.hash.stringa_var.dtor = Hash_StringA_Var_dtor;

  _api.hash.stringa_var.getStubA = Hash_StringA_Var_getStubA;
  _api.hash.stringa_var.getStringA = Hash_StringA_Var_getStringA;
    
  _api.hash.stringa_var.useStubA = Hash_StringA_Var_useStubA;
  _api.hash.stringa_var.useStringA = Hash_StringA_Var_useStringA;
    
  _api.hash.stringa_var.putStubA = Hash_StringA_Var_putStubA;
  _api.hash.stringa_var.putStringA = Hash_StringA_Var_putStringA;
    
  _api.hash.stringa_var.removeStubA = Hash_StringA_Var_removeStubA;
  _api.hash.stringa_var.removeStringA = Hash_StringA_Var_removeStringA;

  _api.hash.stringa_var.dFree = Hash_StringA_Var_dFree;

  // Hash<StringW, ?>
  _api.hash.stringw_unknown.getStubA = Hash_StringT_Unknown_getStub<CharW, char>;
  _api.hash.stringw_unknown.getStubW = Hash_StringT_Unknown_getStub<CharW, CharW>;
  _api.hash.stringw_unknown.getStringW = Hash_StringT_Unknown_getString<CharW, CharW>;
    
  _api.hash.stringw_unknown.useStubA = Hash_StringT_Unknown_useStub<CharW, char>;
  _api.hash.stringw_unknown.useStubW = Hash_StringT_Unknown_useStub<CharW, CharW>;
  _api.hash.stringw_unknown.useStringW = Hash_StringT_Unknown_useString<CharW, CharW>;

  _api.hash.stringw_unknown.putStubA = Hash_StringT_Unknown_putStub<CharW, char>;
  _api.hash.stringw_unknown.putStubW = Hash_StringT_Unknown_putStub<CharW, CharW>;
  _api.hash.stringw_unknown.putStringW = Hash_StringT_Unknown_putString<CharW, CharW>;

  _api.hash.stringw_unknown.removeStubA = Hash_StringT_Unknown_removeStub<CharW, char>;
  _api.hash.stringw_unknown.removeStubW = Hash_StringT_Unknown_removeStub<CharW, CharW>;
  _api.hash.stringw_unknown.removeStringW = Hash_StringT_Unknown_removeString<CharW, CharW>;

  // Hash<StringW, StringW>
  _api.hash.stringw_stringw.ctor = Hash_StringW_StringW_ctor;
  _api.hash.stringw_stringw.dtor = Hash_StringW_StringW_dtor;

  _api.hash.stringw_stringw.getStubA = Hash_StringW_StringW_getStubA;
  _api.hash.stringw_stringw.getStubW = Hash_StringW_StringW_getStubW;
  _api.hash.stringw_stringw.getStringW = Hash_StringW_StringW_getStringW;
    
  _api.hash.stringw_stringw.useStubA = Hash_StringW_StringW_useStubA;
  _api.hash.stringw_stringw.useStubW = Hash_StringW_StringW_useStubW;
  _api.hash.stringw_stringw.useStringW = Hash_StringW_StringW_useStringW;
    
  _api.hash.stringw_stringw.putStubA = Hash_StringW_StringW_putStubA;
  _api.hash.stringw_stringw.putStubW = Hash_StringW_StringW_putStubW;
  _api.hash.stringw_stringw.putStringW = Hash_StringW_StringW_putStringW;
    
  _api.hash.stringw_stringw.removeStubA = Hash_StringW_StringW_removeStubA;
  _api.hash.stringw_stringw.removeStubW = Hash_StringW_StringW_removeStubW;
  _api.hash.stringw_stringw.removeStringW = Hash_StringW_StringW_removeStringW;

  _api.hash.stringw_stringw.dFree = Hash_StringW_StringW_dFree;

  // Hash<StringW, Var>
  _api.hash.stringw_var.ctor = Hash_StringW_Var_ctor;
  _api.hash.stringw_var.dtor = Hash_StringW_Var_dtor;

  _api.hash.stringw_var.getStubA = Hash_StringW_Var_getStubA;
  _api.hash.stringw_var.getStubW = Hash_StringW_Var_getStubW;
  _api.hash.stringw_var.getStringW = Hash_StringW_Var_getStringW;
    
  _api.hash.stringw_var.useStubA = Hash_StringW_Var_useStubA;
  _api.hash.stringw_var.useStubW = Hash_StringW_Var_useStubW;
  _api.hash.stringw_var.useStringW = Hash_StringW_Var_useStringW;
    
  _api.hash.stringw_var.putStubA = Hash_StringW_Var_putStubA;
  _api.hash.stringw_var.putStubW = Hash_StringW_Var_putStubW;
  _api.hash.stringw_var.putStringW = Hash_StringW_Var_putStringW;
    
  _api.hash.stringw_var.removeStubA = Hash_StringW_Var_removeStubA;
  _api.hash.stringw_var.removeStubW = Hash_StringW_Var_removeStubW;
  _api.hash.stringw_var.removeStringW = Hash_StringW_Var_removeStringW;
  
  _api.hash.stringw_var.dFree = Hash_StringW_Var_dFree;

  // HashIterator<?, ?>
  _api.hashiterator.start = HashIterator_start;
  _api.hashiterator.next = HashIterator_next;

  // -------------------------------------------------------------------------
  // [Data]
  // -------------------------------------------------------------------------

  HashUntypedData* d;

  // Hash<?, ?>
  d = &Hash_Unknown_Unknown_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_UNKNOWN | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandLength = Hash_primeTable[0];

  Hash_Unknown_Unknown_oEmpty->_d = d;
  _api.hash.unknown_unknown.oEmpty = &Hash_Unknown_Unknown_oEmpty;

  // Hash<StringA, StringA>
  d = &Hash_StringA_StringA_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_HASH_STRINGA_STRINGA | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandCapacity = Hash_primeTable[0];
  d->expandLength = 1;

  Hash_StringA_StringA_oEmpty->_d = d;
  _api.hash.stringa_stringa.oEmpty = &Hash_StringA_StringA_oEmpty;

  // HashVTable<StringA, StringA>
  Hash_StringA_StringA_vTable->idxKeyT = FOG_OFFSET_OF(HashKeyNode<StringA>, key);
  Hash_StringA_StringA_vTable->idxItemT = FOG_OFFSET_OF(HashKeyNode<StringA>, key) + sizeof(StringA);
  Hash_StringA_StringA_vTable->szKeyT = sizeof(StringA);
  Hash_StringA_StringA_vTable->szItemT = sizeof(StringA);
  Hash_StringA_StringA_vTable->ctor = Hash_StringA_StringA_Node_ctor;
  Hash_StringA_StringA_vTable->dtor = Hash_StringA_StringA_Node_dtor;
  Hash_StringA_StringA_vTable->setItem = (HashUntypedVTable::SetItem)_api.stringa.setStringA;
  Hash_StringA_StringA_vTable->hashKey = (HashFunc)_api.stringa.getHashCode;
  Hash_StringA_StringA_vTable->eqKey = (EqFunc)_api.stringa.eqStringA;
  _api.hash.stringa_stringa.vTable = &Hash_StringA_StringA_vTable;

  // Hash<StringA, Var>
  d = &Hash_StringW_Var_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_HASH_STRINGA_VAR | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandCapacity = Hash_primeTable[0];
  d->expandLength = 1;

  Hash_StringA_Var_oEmpty->_d = d;
  _api.hash.stringa_var.oEmpty = &Hash_StringA_Var_oEmpty;

  // HashVTable<StringA, Var>
  Hash_StringA_Var_vTable->idxKeyT = FOG_OFFSET_OF(HashKeyNode<StringA>, key);
  Hash_StringA_Var_vTable->idxItemT = FOG_OFFSET_OF(HashKeyNode<StringA>, key) + sizeof(StringA);
  Hash_StringA_Var_vTable->szKeyT = sizeof(StringA);
  Hash_StringA_Var_vTable->szItemT = sizeof(Var);
  Hash_StringA_Var_vTable->ctor = Hash_StringA_Var_Node_ctor;
  Hash_StringA_Var_vTable->dtor = Hash_StringA_Var_Node_dtor;
  Hash_StringA_Var_vTable->setItem = (HashUntypedVTable::SetItem)_api.var.copy;
  Hash_StringA_Var_vTable->hashKey = (HashFunc)_api.stringa.getHashCode;
  Hash_StringA_Var_vTable->eqKey = (EqFunc)_api.stringa.eqStringA;
  _api.hash.stringa_var.vTable = &Hash_StringA_Var_vTable;

  // Hash<StringW, StringW>
  d = &Hash_StringW_StringW_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_HASH_STRINGW_STRINGW | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandCapacity = Hash_primeTable[0];
  d->expandLength = 1;

  Hash_StringW_StringW_oEmpty->_d = d;
  _api.hash.stringw_stringw.oEmpty = &Hash_StringW_StringW_oEmpty;

  // HashVTable<StringW, StringW>
  Hash_StringW_StringW_vTable->idxKeyT = FOG_OFFSET_OF(HashKeyNode<StringW>, key);
  Hash_StringW_StringW_vTable->idxItemT = FOG_OFFSET_OF(HashKeyNode<StringW>, key) + sizeof(StringW);
  Hash_StringW_StringW_vTable->szKeyT = sizeof(StringW);
  Hash_StringW_StringW_vTable->szItemT = sizeof(StringW);
  Hash_StringW_StringW_vTable->ctor = Hash_StringW_StringW_Node_ctor;
  Hash_StringW_StringW_vTable->dtor = Hash_StringW_StringW_Node_dtor;
  Hash_StringW_StringW_vTable->setItem = (HashUntypedVTable::SetItem)_api.stringw.setStringW;
  Hash_StringW_StringW_vTable->hashKey = (HashFunc)_api.stringw.getHashCode;
  Hash_StringW_StringW_vTable->eqKey = (EqFunc)_api.stringw.eqStringW;
  _api.hash.stringw_stringw.vTable = &Hash_StringW_StringW_vTable;

  // Hash<StringW, Var>
  d = &Hash_StringW_Var_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_HASH_STRINGW_VAR | VAR_FLAG_NONE;
  d->capacity = 1;
  d->expandCapacity = Hash_primeTable[0];
  d->expandLength = 1;

  Hash_StringW_Var_oEmpty->_d = d;
  _api.hash.stringw_var.oEmpty = &Hash_StringW_Var_oEmpty;

  // HashVTable<StringW, Var>
  Hash_StringW_Var_vTable->idxKeyT = FOG_OFFSET_OF(HashKeyNode<StringW>, key);
  Hash_StringW_Var_vTable->idxItemT = FOG_OFFSET_OF(HashKeyNode<StringW>, key) + sizeof(StringW);
  Hash_StringW_Var_vTable->szKeyT = sizeof(StringW);
  Hash_StringW_Var_vTable->szItemT = sizeof(Var);
  Hash_StringW_Var_vTable->ctor = Hash_StringW_Var_Node_ctor;
  Hash_StringW_Var_vTable->dtor = Hash_StringW_Var_Node_dtor;
  Hash_StringW_Var_vTable->setItem = (HashUntypedVTable::SetItem)_api.var.copy;
  Hash_StringW_Var_vTable->hashKey = (HashFunc)_api.stringw.getHashCode;
  Hash_StringW_Var_vTable->eqKey = (EqFunc)_api.stringw.eqStringW;
  _api.hash.stringa_var.vTable = &Hash_StringW_Var_vTable;
}

} // Fog namespace
