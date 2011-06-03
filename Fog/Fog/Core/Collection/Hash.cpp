// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>

namespace Fog {

// ============================================================================
// [Fog::UnorderedAbstract]
// ============================================================================

Static<UnorderedAbstract::Data> UnorderedAbstract::_dnull;

UnorderedAbstract::Data* UnorderedAbstract::_allocData(size_t capacity)
{
  size_t dsize =
    sizeof(Data) - sizeof(void*) + capacity * sizeof(void**);

  Data* d = (Data*)Memory::calloc(dsize);
  if (FOG_IS_NULL(d)) return NULL;

  d->refCount.init(1);
  d->capacity = capacity;

  d->expandCapacity = _calcExpandCapacity(capacity);
  d->expandLength = (size_t)((sysint_t)d->capacity * 0.92);

  d->shrinkCapacity = _calcShrinkCapacity(capacity);
  d->shrinkLength = (size_t)((sysint_t)d->shrinkCapacity * 0.70);

  return d;
}

void UnorderedAbstract::_freeData(Data* d)
{
  Memory::free(d);
}

size_t UnorderedAbstract::_calcExpandCapacity(size_t capacity)
{
  static const size_t threshold = 1024*1024*4;

  if (capacity < threshold)
    return capacity << 1;
  else
    return capacity + threshold;
}

size_t UnorderedAbstract::_calcShrinkCapacity(size_t capacity)
{
  static const size_t threshold = 1024*1024*4;

  if (capacity < threshold)
    return capacity >> 1;
  else
    return capacity - threshold;
}

bool UnorderedAbstract::_rehash(size_t bc)
{
  Data* newd = _allocData(bc);
  if (FOG_IS_NULL(newd)) return false;

  size_t i, len = _d->capacity;
  for (i = 0; i < len; i++)
  {
    Node* node = (Node*)(_d->buckets[i]);
    while (node)
    {
      uint32_t hashMod = node->hashCode % bc;
      Node* next = node->next;

      node->next = (Node*)newd->buckets[hashMod];
      newd->buckets[hashMod] = node;

      node = next;
    }
  }

  newd->length = _d->length;
  Data* old = atomicPtrXchg(&_d, newd);

  old->refCount.dec();
  if (old != _dnull.instancep()) _freeData(old);

  return true;
}

void UnorderedAbstract::_Iterator::_toBegin()
{
  if (FOG_UNLIKELY(_hash->isEmpty()))
  {
    _node = NULL;
    _index = INVALID_INDEX;
    return;
  }

  size_t i, len = _hash->_d->capacity;
  UnorderedAbstract::Node* node;

  for (i = 0; i < len; i++)
  {
    node = (UnorderedAbstract::Node*)(_hash->_d->buckets[i]);
    if (node) break;
  }

  // We checked isEmpty(), node can't be NULL
  FOG_ASSERT(node != NULL);

  _node = node;
  _index = i;
}

void UnorderedAbstract::_Iterator::_toNext()
{
  size_t i, len = _hash->_d->capacity;
  UnorderedAbstract::Node* node = _node;

  // Bail out if there is problem
  if (FOG_UNLIKELY(node == NULL)) return;

  // If node exists index must point to correct bucket
  FOG_ASSERT(_index < len);

  // Chains
  if (FOG_UNLIKELY(node->next))
  {
    _node = node->next;
    return;
  }

  for (i = _index + 1; i < len; i++)
  {
    node = (UnorderedAbstract::Node*)(_hash->_d->buckets[i]);
    if (node) { _node = node; _index = i; return; }
  }

  _node = NULL;
  _index = INVALID_INDEX;
}

UnorderedAbstract::Node* UnorderedAbstract::_Iterator::_removeCurrent()
{
  Node* node = _node;
  size_t i = _index;

  if (node == NULL) return NULL;

  _toNext();

  Node* n = (Node*)_hash->_d->buckets[i];
  Node* prev = NULL;

  // Find node to remove. We need 'prev' pointer to remove it from chains.
  while (n != node)
  {
    prev = node;
    node = node->next;
    // Can't go out of range, becuase we know node bucked index.
    FOG_ASSERT(node != NULL);
  }

  if (prev)
    prev->next = node->next;
  else
    _hash->_d->buckets[i] = node->next;

  _hash->_d->length--;
  return n;
}

// ===========================================================================
// [Fog::Core - Library Initializers]
// ===========================================================================

FOG_NO_EXPORT void _core_hash_init(void)
{
  UnorderedAbstract::Data* d = UnorderedAbstract::_dnull.instancep();
  d->refCount.init(1);
  d->capacity = 1;
  d->length = 0;
  d->expandCapacity = 0;
  d->expandLength = 128;
  d->shrinkCapacity = 0;
  d->shrinkLength = 0;
  d->buckets[0] = NULL;
}

FOG_NO_EXPORT void _core_hash_fini(void)
{
  UnorderedAbstract::Data* d = UnorderedAbstract::_dnull.instancep();
  d->refCount.dec();
}

} // Fog namespace
