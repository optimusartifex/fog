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
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/Core/Xml/XmlIdManager.h>

namespace Fog {

// ============================================================================
// [Fog::XmlIdManager]
// ============================================================================

XmlIdManager::XmlIdManager() :
  _capacity(16),
  _length(0),
  _expandCapacity(64),
  _expandLength(16),
  _shrinkCapacity(0),
  _shrinkLength(0),
  _buckets(_bucketsBuffer)
{
  Memory::zero(_bucketsBuffer, sizeof(_bucketsBuffer));
}

XmlIdManager::~XmlIdManager()
{
  if (_buckets != _bucketsBuffer) Memory::free(_buckets);
}

void XmlIdManager::add(XmlElement* e)
{
  uint32_t hashCode = e->_id._d->hashCode;
  uint32_t hashMod = hashCode % _capacity;

  XmlElement* node = _buckets[hashMod];
  XmlElement* prev = NULL;

  while (node)
  {
    prev = node;
    node = node->_hashNextId;
  }

  if (prev)
    prev->_hashNextId = e;
  else
    _buckets[hashMod] = e;
  if (++_length >= _expandLength) _rehash(_expandCapacity);
}

void XmlIdManager::remove(XmlElement* e)
{
  uint32_t hashCode = e->_id._d->hashCode;
  uint32_t hashMod = hashCode % _capacity;

  XmlElement* node = _buckets[hashMod];
  XmlElement* prev = NULL;

  while (node)
  {
    if (node == e)
    {
      if (prev)
        prev->_hashNextId = node->_hashNextId;
      else
        _buckets[hashMod] = node->_hashNextId;

      node->_hashNextId = NULL;
      if (--_length <= _shrinkLength) _rehash(_shrinkCapacity);
      return;
    }

    prev = node;
    node = node->_hashNextId;
  }
}

XmlElement* XmlIdManager::get(const String& id) const
{
  uint32_t hashCode = id.getHashCode();
  uint32_t hashMod = hashCode % _capacity;

  XmlElement* node = _buckets[hashMod];
  while (node)
  {
    if (node->_id == id) return node;
    node = node->_hashNextId;
  }
  return NULL;
}

XmlElement* XmlIdManager::get(const Char* idStr, size_t idLen) const
{
  uint32_t hashCode = HashUtil::makeStringHash(idStr, idLen);
  uint32_t hashMod = hashCode % _capacity;

  XmlElement* node = _buckets[hashMod];
  while (node)
  {
    if (node->_id._d->hashCode == hashCode && StringUtil::eq(node->_id.getData(), idStr, idLen)) return node;
    node = node->_hashNextId;
  }
  return NULL;
}

void XmlIdManager::_rehash(size_t capacity)
{
  XmlElement** oldBuckets = _buckets;
  XmlElement** newBuckets = (XmlElement**)Memory::calloc(sizeof(XmlElement*) * capacity);
  if (!newBuckets) return;

  size_t i, len = _capacity;
  for (i = 0; i < len; i++)
  {
    XmlElement* node = oldBuckets[i];
    while (node)
    {
      uint32_t hashMod = node->_id._d->hashCode % capacity;
      XmlElement* next = node->_hashNextId;

      XmlElement* newCur = newBuckets[hashMod];
      XmlElement* newPrev = NULL;
      while (newCur) { newPrev = newCur; newCur = newCur->_hashNextId; }

      if (newPrev)
        newPrev->_hashNextId = node;
      else
        newBuckets[hashMod] = node;
      node->_hashNextId = NULL;

      node = next;
    }
  }

  _capacity = capacity;

  _expandCapacity = UnorderedAbstract::_calcExpandCapacity(capacity);
  _expandLength = (size_t)((sysint_t)_capacity * 0.92);

  _shrinkCapacity = UnorderedAbstract::_calcShrinkCapacity(capacity);
  _shrinkLength = (size_t)((sysint_t)_shrinkCapacity * 0.70);

  atomicPtrXchg(&_buckets, newBuckets);
  if (oldBuckets != _bucketsBuffer) Memory::free(oldBuckets);
}

} // Fog namespace
