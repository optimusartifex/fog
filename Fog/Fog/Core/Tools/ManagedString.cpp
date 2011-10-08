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
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/HashUtil.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/StringUtil.h>

namespace Fog {

// ============================================================================
// [Fog::ManagedStringLocal]
// ============================================================================

#define INITIAL_CAPACITY 256

struct FOG_NO_EXPORT ManagedStringLocal
{
  typedef ManagedString::Node Node;
  typedef ManagedString::Cache Cache;

  FOG_INLINE ManagedStringLocal() :
    _capacity(INITIAL_CAPACITY),
    _length(0),
    _expandCapacity(INITIAL_CAPACITY * 2),
    _expandLength((size_t)(INITIAL_CAPACITY * 0.9)),
    _shrinkCapacity(0),
    _shrinkLength(0),
    _buckets((Node**)MemMgr::calloc(INITIAL_CAPACITY * sizeof(Node*))),
    _null(StringW())
  {
    ManagedString::_dnull = &_null;
  }

  FOG_INLINE ~ManagedStringLocal()
  {
    {
      HashIterator<StringW, Cache*> it(_hash);
      while (it.isValid())
      {
        Cache* c = it.getItem();
        c->~Cache();
        MemMgr::free(c);
        it.next();
      }
    }

    // Free allocated memory for buckets and set everything to NULL.
    MemMgr::free(_buckets);
    ManagedString::_dnull = NULL;
  }

  FOG_INLINE Node* addString(const StringW& s)
  {
    AutoLock locked(_lock);

    uint32_t hashCode = s.getHashCode();
    uint32_t hashMod = hashCode % _capacity;

    Node* node = _buckets[hashMod];
    Node* prev = NULL;

    while (node)
    {
      // Node is already here?
      if (node->getHashCode() == hashCode && node->getString() == s)
      {
        // This can also happen in high concurrent environment. We are trying
        // to do some work without locking and this is simply situation where
        // some other thread was faster. We can't create new hash node, because
        // it already exists, we just reference it.
        node->reference.inc();
        return node;
      }

      prev = node;
      node = node->next;
    }

    node = fog_new Node(s);
    if (FOG_IS_NULL(node)) return NULL;
    node->string.squeeze();

    if (prev)
      prev->next = node;
    else
      _buckets[hashMod] = node;

    if (++_length >= _expandLength) _rehash(_expandCapacity);
    return node;
  }

  // This function is called from @c ManagedString::createCache().
  FOG_INLINE Node* addNodeNoLock(Node* n)
  {
    uint32_t hashCode = n->getHashCode();
    uint32_t hashMod = hashCode % _capacity;

    Node* node = _buckets[hashMod];
    Node* prev = NULL;

    while (node)
    {
      // Node is already here? But different pointer.
      if (node->getHashCode() == hashCode && node->getString() == n->getString())
      {
        node->reference.inc();
        return node;
      }

      prev = node;
      node = node->next;
    }

    if (prev)
      prev->next = n;
    else
      _buckets[hashMod] = n;

    if (++_length >= _expandLength) _rehash(_expandCapacity);
    return n;
  }

  FOG_INLINE Node* addUtf16(const StubW& _s)
  {
    AutoLock locked(_lock);

    const CharW* sData = _s.getData();
    size_t sLength = _s.getComputedLength();

    FOG_ASSERT(sLength != 0);

    uint32_t hashCode = HashUtil::hash(StubW(sData, sLength));
    uint32_t hashMod = hashCode % _capacity;

    Node* node = _buckets[hashMod];
    Node* prev = NULL;

    while (node)
    {
      // Node is already here?
      if (node->getHashCode() == hashCode && node->getString().eq(StubW(sData, sLength)))
      {
        // This can also happen in high concurrent environment. We are trying
        // to do some work without locking and this is simply situation where
        // some other thread was faster. We can't create new hash node, because
        // it already exists, we just reference it.
        node->reference.inc();
        return node;
      }

      prev = node;
      node = node->next;
    }

    StringW str;
    if (str.set(StubW(sData, sLength)) != ERR_OK) return NULL;

    node = fog_new Node(str);
    if (FOG_IS_NULL(node)) return NULL;

    if (prev)
      prev->next = node;
    else
      _buckets[hashMod] = node;

    if (++_length >= _expandLength) _rehash(_expandCapacity);
    return node;
  }

  FOG_INLINE void remove(Node* n)
  {
    FOG_ASSERT(n != &_null);

    AutoLock locked(_lock);

    // If some thread referenced it before our lock.
    if (n->reference.get() > 0) return;

    uint32_t hashCode = n->getHashCode();
    uint32_t hashMod = hashCode % _capacity;

    Node* node = _buckets[hashMod];
    Node* prev = NULL;

    while (node)
    {
      if (node == n)
      {
        if (prev)
          prev->next = node->next;
        else
          _buckets[hashMod] = node->next;

        fog_delete(node);
        if (--_length <= _shrinkLength) _rehash(_shrinkCapacity);
        return;
      }

      prev = node;
      node = node->next;
    }
  }

  FOG_INLINE Node* refString(const StringW& s) const
  {
    AutoLock locked(_lock);

    uint32_t hashCode = s.getHashCode();
    uint32_t hashMod = hashCode % _capacity;

    Node* node = _buckets[hashMod];
    while (node)
    {
      if (node->getHashCode() == hashCode && node->string == s) return node->addRef();
      node = node->next;
    }
    return NULL;
  }

  FOG_INLINE Node* refUtf16(const StubW& _s) const
  {
    AutoLock locked(_lock);

    const CharW* sData = _s.getData();
    size_t sLength = _s.getComputedLength();

    uint32_t hashCode = HashUtil::hash(StubW(sData, sLength));
    uint32_t hashMod = hashCode % _capacity;

    Node* node = _buckets[hashMod];
    while (node)
    {
      if (node->getHashCode() == hashCode && node->string.eq(StubW(sData, sLength))) return node->addRef();
      node = node->next;
    }

    return NULL;
  }

  FOG_NO_INLINE void _rehash(size_t capacity)
  {
    Node** oldBuckets = _buckets;
    Node** newBuckets = (Node**)MemMgr::calloc(sizeof(Node*) * capacity);
    if (FOG_IS_NULL(newBuckets)) return;

    size_t i, len = _capacity;
    for (i = 0; i < len; i++)
    {
      Node* node = oldBuckets[i];
      while (node)
      {
        uint32_t hashMod = node->getHashCode() % capacity;
        Node* next = node->next;

        node->next = newBuckets[hashMod];
        newBuckets[hashMod] = node;

        node = next;
      }
    }

    _capacity = capacity;

    _expandCapacity = _api.hashhelper_calcExpandCapacity(capacity);
    _expandLength = (size_t)((ssize_t)_capacity * 0.92);

    _shrinkCapacity = _api.hashhelper_calcShrinkCapacity(capacity);
    _shrinkLength = (size_t)((ssize_t)_shrinkCapacity * 0.70);

    atomicPtrXchg(&_buckets, newBuckets);
    if (oldBuckets) MemMgr::free(oldBuckets);
  }

  // [Members]

  mutable Lock _lock;

  //! @brief Count of buckets.
  size_t _capacity;
  //! @brief Count of nodes.
  size_t _length;

  //! @brief Count of buckets we will expand to if length exceeds _expandLength.
  size_t _expandCapacity;
  //! @brief Count of nodes to grow.
  size_t _expandLength;

  //! @brief Count of buckeds we will shrink to if length gets _shinkLength.
  size_t _shrinkCapacity;
  //! @brief Count of nodes to shrink.
  size_t _shrinkLength;

  //! @brief Buckets.
  Node** _buckets;

  // [Null]

  Node _null;

  // [Managed StringW Cache]

  Hash<StringW, Cache*> _hash;
};

static Static<ManagedStringLocal> managed_local;

// ============================================================================
// [Fog::ManagedString]
// ============================================================================

ManagedString::Node* ManagedString::_dnull;

ManagedString::ManagedString() :
  _node(_dnull->addRef())
{
}

ManagedString::ManagedString(const ManagedString& other) :
  _node(other._node->addRef())
{
}

ManagedString::ManagedString(const StringW& s) :
  _node(managed_local->addString(s))
{
  if (_node == NULL) _node = _dnull->addRef();
}

ManagedString::ManagedString(const StubW& s) :
  _node(managed_local->addUtf16(s))
{
  if (_node == NULL) _node = _dnull->addRef();
}

ManagedString::~ManagedString()
{
  if (_node->reference.deref()) managed_local->remove(_node);
}

void ManagedString::clear()
{
  Node* old = atomicPtrXchg(&_node, _dnull->addRef());
  if (old->reference.deref()) managed_local->remove(old);
}

err_t ManagedString::set(const ManagedString& str)
{
  Node* old = atomicPtrXchg(&_node, str._node->addRef());
  if (old->reference.deref()) managed_local->remove(old);
  return ERR_OK;
}

err_t ManagedString::set(const StringW& str)
{
  Node* node = managed_local->addString(str);

  if (FOG_IS_NULL(node))
  {
    clear();
    return ERR_RT_OUT_OF_MEMORY;
  }

  Node* old = atomicPtrXchg(&_node, node);
  if (old->reference.deref()) managed_local->remove(old);
  return ERR_OK;
}

err_t ManagedString::set(const StubW& str)
{
  Node* node = managed_local->addUtf16(str);

  if (FOG_IS_NULL(node))
  {
    clear();
    return ERR_RT_OUT_OF_MEMORY;
  }

  Node* old = atomicPtrXchg(&_node, node);
  if (old->reference.deref()) managed_local->remove(old);
  return ERR_OK;
}

err_t ManagedString::setIfManaged(const StringW& s)
{
  Node* node = managed_local->refString(s);
  if (FOG_IS_NULL(node)) return ERR_RT_OBJECT_NOT_FOUND;

  Node* old = atomicPtrXchg(&_node, node);
  if (old->reference.deref()) managed_local->remove(old);
  return ERR_OK;
}

err_t ManagedString::setIfManaged(const StubW& s)
{
  Node* node = managed_local->refUtf16(s);
  if (FOG_IS_NULL(node)) return ERR_RT_OBJECT_NOT_FOUND;

  Node* old = atomicPtrXchg(&_node, node);
  if (old->reference.deref()) managed_local->remove(old);
  return ERR_OK;
}

// ============================================================================
// [Fog::ManagedString::Cache]
// ============================================================================

ManagedString::Cache* ManagedString::createCache(const char* strings, size_t length, size_t count, const StringW& name)
{
  if (name.isEmpty()) return NULL;

  AutoLock locked(managed_local->_lock);
  Cache* self = managed_local->_hash.get(name, NULL);

  // Get count of 'strings'.
  //
  // NOTE, sizeof() always gives size with null terminator, so for example
  // char[] a = "abc" will give us 4 ! This is reason why we are decrementing
  // it here.
  if (count == DETECT_LENGTH) count = StringUtil::countOf(strings, length - 1, '\0', CASE_SENSITIVE);

  size_t alloc =
    // Fog::ManagedString::Cache structure
    sizeof(Cache) +
    // Fog::ManagedString structure
    sizeof(ManagedString) * count +
    // Fog::StringDataW structure
    sizeof(StringDataW) * count +
    // Fog::StringDataW contains null terminator, subtract it.
    sizeof(CharW) * (length - count) +
    // Fog::ManagedString::Node
    sizeof(Node) * count;

  if ((self = (ManagedString::Cache*)MemMgr::alloc(alloc)) == NULL) return NULL;

  const char* pBeg = strings;
  const char* pCur = strings;
  Node** pList = self->_data;
  uint8_t* pNodes = reinterpret_cast<uint8_t*>(&self->_data[count]);
  uint8_t* pChars = pNodes + sizeof(ManagedString::Node) * count;
  size_t counter = 0;

  for (;;)
  {
    if (pCur[0] == 0)
    {
      StringDataW* d = (StringDataW*)pChars;
      size_t len = (size_t)(pCur - pBeg);

      d->reference.init(1);
      d->vType = VAR_TYPE_STRINGW | VAR_FLAG_NONE;
      d->length = len;
      d->capacity = len;
      d->hashCode = 0;
      StringUtil::copy(d->data, pBeg, len + 1);

      *pList++ = managed_local->addNodeNoLock(fog_new_p(pNodes) Node(d));
      pNodes += sizeof(Node);
      pChars += StringDataW::getSizeOf(len);

      counter++;

      pBeg = ++pCur;
      if (pBeg[0] == 0) break;
    }
    else
      pCur++;
  }

  FOG_ASSERT(count == counter);
  fog_new_p(self) Cache(name, counter);
  self->_name = name;
  self->_name.squeeze();

  managed_local->_hash.put(self->_name, self);
  return self;
}

ManagedString::Cache* ManagedString::getCacheByName(const StringW& name)
{
  AutoLock locked(managed_local->_lock);
  return managed_local->_hash.get(name, NULL);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ManagedString_init(void)
{
  managed_local.init();
}

FOG_NO_EXPORT void ManagedString_fini(void)
{
  managed_local.destroy();
}

} // Fog namespace
