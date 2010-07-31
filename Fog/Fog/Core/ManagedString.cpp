// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Lock.h>
#include <Fog/Core/Hash.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/StringUtil.h>

namespace Fog {

// ============================================================================
// [Fog::ManagedStringLocal]
// ============================================================================

#define INITIAL_CAPACITY 256

struct FOG_HIDDEN ManagedStringLocal
{
  typedef ManagedString::Node Node;
  typedef ManagedString::Cache Cache;

  FOG_INLINE ManagedStringLocal() :
    _capacity(INITIAL_CAPACITY),
    _length(0),
    _expandCapacity(INITIAL_CAPACITY * 2),
    _expandLength((sysuint_t)(INITIAL_CAPACITY * 0.9)),
    _shrinkCapacity(0),
    _shrinkLength(0),
    _buckets((Node**)Memory::calloc(INITIAL_CAPACITY * sizeof(Node*))),
    _null(String())
  {
    ManagedString::_dnull = &_null;
  }

  FOG_INLINE ~ManagedStringLocal()
  {
    {
      UnorderedHash<String, Cache*>::ConstIterator it(_hash);
      for (it.toStart(); it.isValid(); it.toNext())
      {
        Cache* c = it.value();
        c->~Cache();
        Memory::free(c);
      }
    }

    // Free allocated memory for buckets and set everything to NULL.
    Memory::free(_buckets);
    ManagedString::_dnull = NULL;
  }

  FOG_INLINE Node* addString(const String& s)
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
        node->refCount.inc();
        return node;
      }

      prev = node;
      node = node->next;
    }

    node = fog_new Node(s);
    node->string.squeeze();
    if (!node) return node;

    if (prev)
      prev->next = node;
    else
      _buckets[hashMod] = node;

    if (++_length >= _expandLength) _rehash(_expandCapacity);
    return node;
  }

  // This function is called from @c MaagedString::createCache().
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
        node->refCount.inc();
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

  FOG_INLINE Node* addUtf16(const Utf16& _s)
  {
    AutoLock locked(_lock);

    const Char* s = _s.getData();
    sysuint_t length = _s.getLength();
    if (length == DETECT_LENGTH) length = StringUtil::len(s);

    FOG_ASSERT(length != 0);

    uint32_t hashCode = HashUtil::hashString(s, length);
    uint32_t hashMod = hashCode % _capacity;

    Node* node = _buckets[hashMod];
    Node* prev = NULL;

    while (node)
    {
      // Node is already here?
      if (node->getHashCode() == hashCode && node->getString().eq(Utf16(s, length)))
      {
        // This can also happen in high concurrent environment. We are trying
        // to do some work without locking and this is simply situation where
        // some other thread was faster. We can't create new hash node, because
        // it already exists, we just reference it.
        node->refCount.inc();
        return node;
      }

      prev = node;
      node = node->next;
    }

    String str;
    if (str.set(Utf16(s, length)) != ERR_OK) return NULL;

    node = fog_new Node(str);
    if (!node) return node;

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
    if (n->refCount.get() > 0) return;

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

  FOG_INLINE Node* refString(const String& s) const
  {
    AutoLock locked(_lock);

    uint32_t hashCode = s.getHashCode();
    uint32_t hashMod = hashCode % _capacity;

    Node* node = _buckets[hashMod];
    while (node)
    {
      if (node->getHashCode() == hashCode && node->string == s) return node->ref();
      node = node->next;
    }
    return NULL;
  }

  FOG_INLINE Node* refUtf16(const Utf16& _s) const
  {
    AutoLock locked(_lock);

    const Char* s = _s.getData();
    sysuint_t length = _s.getLength();
    if (length == DETECT_LENGTH) length = StringUtil::len(s);

    uint32_t hashCode = HashUtil::hashString(s, length);
    uint32_t hashMod = hashCode % _capacity;

    Node* node = _buckets[hashMod];
    while (node)
    {
      if (node->getHashCode() == hashCode && node->string.eq(Utf16(s, length))) return node->ref();
      node = node->next;
    }
    return NULL;
  }

  FOG_NO_INLINE void _rehash(sysuint_t capacity)
  {
    Node** oldBuckets = _buckets;
    Node** newBuckets = (Node**)Memory::calloc(sizeof(Node*) * capacity);
    if (!newBuckets) return;

    sysuint_t i, len = _capacity;
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

    _expandCapacity = UnorderedAbstract::_calcExpandCapacity(capacity);
    _expandLength = (sysuint_t)((sysint_t)_capacity * 0.92);

    _shrinkCapacity = UnorderedAbstract::_calcShrinkCapacity(capacity);
    _shrinkLength = (sysuint_t)((sysint_t)_shrinkCapacity * 0.70);

    atomicPtrXchg(&_buckets, newBuckets);
    if (oldBuckets) Memory::free(oldBuckets);
  }

  // [Members]

  mutable Lock _lock;

  //! @brief Count of buckets.
  sysuint_t _capacity;
  //! @brief Count of nodes.
  sysuint_t _length;

  //! @brief Count of buckets we will expand to if length exceeds _expandLength.
  sysuint_t _expandCapacity;
  //! @brief Count of nodes to grow.
  sysuint_t _expandLength;

  //! @brief Count of buckeds we will shrink to if length gets _shinkLength.
  sysuint_t _shrinkCapacity;
  //! @brief Count of nodes to shrink.
  sysuint_t _shrinkLength;

  //! @brief Buckets.
  Node** _buckets;

  // [Null]

  Node _null;

  // [Managed String Cache]

  UnorderedHash<String, Cache*> _hash;
};

static Static<ManagedStringLocal> managed_local;

// ============================================================================
// [Fog::ManagedString]
// ============================================================================

ManagedString::Node* ManagedString::_dnull;

ManagedString::ManagedString() :
  _node(_dnull->ref())
{
}

ManagedString::ManagedString(const ManagedString& other) :
  _node(other._node->ref())
{
}

ManagedString::ManagedString(const String& s) :
  _node(managed_local->addString(s))
{
  if (_node == NULL) _node = _dnull->ref();
}

ManagedString::ManagedString(const Utf16& s) :
  _node(managed_local->addUtf16(s))
{
  if (_node == NULL) _node = _dnull->ref();
}

ManagedString::~ManagedString()
{
  if (_node->refCount.deref()) managed_local->remove(_node);
}

void ManagedString::clear()
{
  Node* old = atomicPtrXchg(&_node, _dnull->ref());
  if (old->refCount.deref()) managed_local->remove(old);
}

err_t ManagedString::set(const ManagedString& str)
{
  Node* old = atomicPtrXchg(&_node, str._node->ref());
  if (old->refCount.deref()) managed_local->remove(old);
  return ERR_OK;
}

err_t ManagedString::set(const String& str)
{
  Node* node = managed_local->addString(str);

  if (!node)
  {
    clear();
    return ERR_RT_OUT_OF_MEMORY;
  }

  Node* old = atomicPtrXchg(&_node, node);
  if (old->refCount.deref()) managed_local->remove(old);
  return ERR_OK;
}

err_t ManagedString::set(const Utf16& str)
{
  Node* node = managed_local->addUtf16(str);

  if (!node)
  {
    clear();
    return ERR_RT_OUT_OF_MEMORY;
  }

  Node* old = atomicPtrXchg(&_node, node);
  if (old->refCount.deref()) managed_local->remove(old);
  return ERR_OK;
}

err_t ManagedString::setIfManaged(const String& s)
{
  Node* node = managed_local->refString(s);
  if (!node) return ERR_RT_OBJECT_NOT_FOUND;

  Node* old = atomicPtrXchg(&_node, node);
  if (old->refCount.deref()) managed_local->remove(old);
  return ERR_OK;
}

err_t ManagedString::setIfManaged(const Utf16& s)
{
  Node* node = managed_local->refUtf16(s);
  if (!node) return ERR_RT_OBJECT_NOT_FOUND;

  Node* old = atomicPtrXchg(&_node, node);
  if (old->refCount.deref()) managed_local->remove(old);
  return ERR_OK;
}

// ============================================================================
// [Fog::ManagedString::Cache]
// ============================================================================

ManagedString::Cache* ManagedString::createCache(const char* strings, sysuint_t length, sysuint_t count, const String& name)
{
  if (name.isEmpty()) return NULL;

  AutoLock locked(managed_local->_lock);
  Cache* self = managed_local->_hash.value(name, NULL);

  // Get count of 'strings'.
  //
  // NOTE, sizeof() always gives size with null terminator, so for example
  // char[] a = "abc" will give us 4 ! This is reason why we are decrementing
  // it here.
  if (count == DETECT_LENGTH) count = StringUtil::countOf(strings, length - 1, '\0', CASE_SENSITIVE);

  sysuint_t alloc =
    // Fog::ManagedString::Cache structure
    sizeof(Cache) +
    // Fog::ManagedString structure
    sizeof(ManagedString) * count +
    // Fog::String::Data structure
    sizeof(String::Data) * count +
    // Fog::String::Data contains null terminator, so we will decrement it.
    sizeof(Char) * (length - count) +
    // Fog::ManagedString::Node
    sizeof(Node) * count;

  if ((self = (ManagedString::Cache*)Memory::alloc(alloc)) == NULL) return NULL;

  const char* pBeg = strings;
  const char* pCur = strings;
  Node** pList = self->_data;
  uint8_t* pNodes = reinterpret_cast<uint8_t*>(&self->_data[count]);
  uint8_t* pChars = pNodes + sizeof(ManagedString::Node) * count;
  sysuint_t counter = 0;

  for (;;)
  {
    if (!*pCur)
    {
      String::Data* d = (String::Data*)pChars;
      sysuint_t len = (sysuint_t)(pCur - pBeg);

      d->refCount.init(1);
      d->flags = String::Data::IsSharable;
      d->length = len;
      d->capacity = len;
      d->hashCode = 0;
      StringUtil::copy(d->data, pBeg, len + 1);

      *pList++ = managed_local->addNodeNoLock(fog_new_p(pNodes) Node(d));
      pNodes += sizeof(Node);
      pChars += String::Data::sizeFor(len);

      counter++;

      pBeg = ++pCur;
      if (!*pBeg) break;
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

ManagedString::Cache* ManagedString::getCacheByName(const String& name)
{
  AutoLock locked(managed_local->_lock);
  return managed_local->_hash.value(name, NULL);
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_managedstring_init(void)
{
  using namespace Fog;

  managed_local.init();
  return ERR_OK;
}

FOG_INIT_DECLARE void fog_managedstring_shutdown(void)
{
  using namespace Fog;

  managed_local.destroy();
}
