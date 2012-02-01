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
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/Core/Tools/StringUtil.h>

namespace Fog {

// ============================================================================
// [Fog::InternedStringNodeW]
// ============================================================================

struct FOG_NO_EXPORT InternedStringNodeW
{
  InternedStringNodeW* next;
  Static<StringW> string;
};

// ============================================================================
// [Fog::InternedStringHashW]
// ============================================================================

struct FOG_NO_EXPORT InternedStringHashW
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  InternedStringHashW();
  ~InternedStringHashW();

  // --------------------------------------------------------------------------
  // [Add]
  // --------------------------------------------------------------------------

  StringDataW* addStubA(const char* sData, size_t sLength, uint32_t hashCode);
  StringDataW* addStubW(const CharW* sData, size_t sLength, uint32_t hashCode);

  void addList(InternedStringW* listData, size_t listLength);

  // --------------------------------------------------------------------------
  // [Lookup]
  // --------------------------------------------------------------------------

  StringDataW* lookupStubA(const char* sData, size_t sLength, uint32_t hashCode) const;
  StringDataW* lookupStubW(const CharW* sData, size_t sLength, uint32_t hashCode) const;

  // --------------------------------------------------------------------------
  // [Management]
  // --------------------------------------------------------------------------

  void _rehash(size_t capacity);
  InternedStringNodeW* _cleanup();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Count of buckets.
  size_t _capacity;
  //! @brief Count of nodes.
  size_t _length;

  //! @brief Count of buckets we will expand to if length exceeds _expandLength.
  size_t _expandCapacity;
  //! @brief Count of nodes to grow.
  size_t _expandLength;

  //! @brief Nodes.
  InternedStringNodeW** _data;
  InternedStringNodeW* _staticData[1];
};

// ============================================================================
// [Fog::InternedStringHashW - Construction / Destruction]
// ============================================================================

InternedStringHashW::InternedStringHashW()
{
  _capacity = 1;
  _length = 0;

  _expandCapacity = 1;
  _expandLength = HashUtil::getClosestPrime(0);

  _data = _staticData;
  _staticData[0] = NULL;
}

InternedStringHashW::~InternedStringHashW()
{
  if (_data != _staticData)
    MemMgr::free(_data);
}

// ============================================================================
// [Fog::InternedStringHashW - Add]
// ============================================================================

StringDataW* InternedStringHashW::addStubA(const char* sData, size_t sLength, uint32_t hashCode)
{
  uint32_t hashMod = hashCode % _capacity;

  InternedStringNodeW** pPrev = &_data[hashMod];
  InternedStringNodeW* node = *pPrev;

  while (node)
  {
    StringDataW* d = node->string->_d;

    if (d->length == sLength && StringUtil::eq(d->data, sData, sLength))
      return d->addRef();

    pPrev = &node->next;
    node = *pPrev;
  }

  node = reinterpret_cast<InternedStringNodeW*>(
    MemMgr::alloc(sizeof(InternedStringNodeW) + StringDataW::getSizeOf(sLength)));

  if (FOG_IS_NULL(node))
    return NULL;

  StringDataW* d = reinterpret_cast<StringDataW*>(node + 1);
  d->reference.init(2);
  d->vType = VAR_TYPE_STRINGW | VAR_FLAG_STRING_INTERNED;
  d->hashCode = hashCode;
  d->capacity = sLength;
  d->length = sLength;
  StringUtil::copy(d->data, sData, sLength);

  node->next = NULL;
  node->string->_d = d;
  *pPrev = node;

  if (++_length >= _expandLength)
    _rehash(_expandCapacity);

  return d;
}

StringDataW* InternedStringHashW::addStubW(const CharW* sData, size_t sLength, uint32_t hashCode)
{
  uint32_t hashMod = hashCode % _capacity;

  InternedStringNodeW** pPrev = &_data[hashMod];
  InternedStringNodeW* node = *pPrev;

  while (node)
  {
    StringDataW* d = node->string->_d;

    if (d->length == sLength && StringUtil::eq(d->data, sData, sLength))
      return d->addRef();

    pPrev = &node->next;
    node = *pPrev;
  }

  node = reinterpret_cast<InternedStringNodeW*>(
    MemMgr::alloc(sizeof(InternedStringNodeW) + StringDataW::getSizeOf(sLength)));

  if (FOG_IS_NULL(node))
    return NULL;

  StringDataW* d = reinterpret_cast<StringDataW*>(node + 1);
  d->reference.init(2);
  d->vType = VAR_TYPE_STRINGW | VAR_FLAG_STRING_INTERNED;
  d->hashCode = hashCode;
  d->capacity = sLength;
  d->length = sLength;
  StringUtil::copy(d->data, sData, sLength);

  node->next = NULL;
  node->string->_d = d;
  *pPrev = node;

  if (++_length >= _expandLength)
    _rehash(_expandCapacity);

  return d;
}

void InternedStringHashW::addList(InternedStringW* listData, size_t listLength)
{
  if (_length + listLength >= _expandLength)
  {
    size_t expandCapacity = HashUtil::getClosestPrime((_length + listLength) / 10 * 11);
    _rehash(expandCapacity);
  }

  for (size_t i = 0; i < listLength; i++)
  {
    StringDataW* d = listData[i]._string->_d;
    const CharW* sData = d->data;

    size_t sLength = d->length;
    FOG_ASSERT(sLength > 0);

    uint32_t hashMod = d->hashCode % _capacity;

    InternedStringNodeW** pPrev = &_data[hashMod];
    InternedStringNodeW* node = *pPrev;

    while (node)
    {
      StringDataW* node_d = node->string->_d;

      if (node_d->length == sLength && StringUtil::eq(node_d->data, sData, sLength))
      {
        listData[i]._string->_d = node_d->addRef();

        d->reference.init(0);
        d = NULL;
        break;
      }

      pPrev = &node->next;
      node = *pPrev;
    }

    if (d)
    {
      node = reinterpret_cast<InternedStringNodeW*>(reinterpret_cast<uint8_t*>(d) - sizeof(InternedStringNodeW));
      node->next = NULL;
      *pPrev = node;
    }
  }
}

// ============================================================================
// [Fog::InternedStringHashW - Lookup]
// ============================================================================

StringDataW* InternedStringHashW::lookupStubA(const char* sData, size_t sLength, uint32_t hashCode) const
{
  uint32_t hashMod = hashCode % _capacity;
  InternedStringNodeW* node = _data[hashMod];

  while (node)
  {
    StringDataW* d = node->string->_d;

    if (d->length == sLength && StringUtil::eq(d->data, sData, sLength))
      return d->addRef();

    node = node->next;
  }

  return NULL;
}

StringDataW* InternedStringHashW::lookupStubW(const CharW* sData, size_t sLength, uint32_t hashCode) const
{
  uint32_t hashMod = hashCode % _capacity;
  InternedStringNodeW* node = _data[hashMod];

  while (node)
  {
    StringDataW* d = node->string->_d;

    if (d->length == sLength && StringUtil::eq(d->data, sData, sLength))
      return d->addRef();

    node = node->next;
  }

  return NULL;
}

// ============================================================================
// [Fog::InternedStringHashW - Management]
// ============================================================================

void InternedStringHashW::_rehash(size_t capacity)
{
  InternedStringNodeW** oldData = _data;
  InternedStringNodeW** newData = (InternedStringNodeW**)MemMgr::calloc(sizeof(InternedStringNodeW*) * capacity);

  if (FOG_IS_NULL(newData))
    return;

  size_t i, len = _capacity;
  for (i = 0; i < len; i++)
  {
    InternedStringNodeW* node = oldData[i];

    while (node)
    {
      uint32_t hashMod = node->string->_d->hashCode % capacity;
      InternedStringNodeW* next = node->next;

      node->next = newData[hashMod];
      newData[hashMod] = node;

      node = next;
    }
  }

  _capacity = capacity;
  if (_capacity <= SIZE_MAX / 19)
    _expandLength = (_capacity * 19) / 20;
  else
    _expandLength = size_t(double(_capacity) * 0.95);

  if (oldData != _staticData)
    MemMgr::free(oldData);

  _data = newData;
}

InternedStringNodeW* InternedStringHashW::_cleanup()
{
  InternedStringNodeW* unusedNodes = NULL;
  InternedStringNodeW** data = _data;

  size_t i, len = _capacity;
  size_t removedCount = 0;

  for (i = 0; i < len; i++)
  {
    InternedStringNodeW** pPrev = &data[i];
    InternedStringNodeW* node = *pPrev;

    while (node)
    {
      InternedStringNodeW* next = node->next;
      if ((node->string->_d->vType & VAR_FLAG_STRING_CACHED) == 0 && node->string->_d->reference.cmpXchg(1, 0))
      {
        *pPrev = next;

        node->next = unusedNodes;
        unusedNodes = node;

        node = next;
        removedCount++;
      }
      else
      {
        pPrev = &node->next;
        node = next;
      }
    }
  }

  _length -= removedCount;

  if (_length < _capacity / 4)
  {
    _rehash(HashUtil::getClosestPrime((_length / 10) * 11));
  }

  return unusedNodes;
}

// ============================================================================
// [Fog::InternedStringW - Global]
// ============================================================================

static Static<Lock> InternedStringW_lock;
static Static<InternedStringHashW> InternedStringW_hash;
static Static<InternedStringW> InternedStringW_oEmpty;

// ============================================================================
// [Fog::InternedStringW - Construction / Destruction]
// ============================================================================

static void FOG_CDECL InternedStringW_ctor(InternedStringW* self)
{
  self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
}

static void FOG_CDECL InternedStringW_ctorCopy(InternedStringW* self, const InternedStringW* other)
{
  self->_string->_d = other->_string->_d->addRef();
}

static err_t FOG_CDECL InternedStringW_ctorStubA(InternedStringW* self, const StubA* stub, uint32_t options)
{
  const char* sData = stub->getData();
  size_t sLength = stub->getComputedLength();

  if (!sLength)
  {
    self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
    return ERR_OK;
  }

  uint32_t hashCode = HashUtil::hash(StubA(sData, sLength));

  AutoLock locked(InternedStringW_lock);
  StringDataW* d;

  if ((options & INTERNED_STRING_OPTION_LOOKUP) != 0)
  {
    d = InternedStringW_hash->lookupStubA(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OBJECT_NOT_FOUND;
    }
  }
  else
  {
    d = InternedStringW_hash->addStubA(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  self->_string->_d = d;
  return ERR_OK;
}
static err_t FOG_CDECL InternedStringW_ctorStubW(InternedStringW* self, const StubW* stub, uint32_t options)
{
  const CharW* sData = stub->getData();
  size_t sLength = stub->getComputedLength();

  if (!sLength)
  {
    self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
    return ERR_OK;
  }

  uint32_t hashCode = HashUtil::hash(StubW(sData, sLength));

  AutoLock locked(InternedStringW_lock);
  StringDataW* d;

  if ((options & INTERNED_STRING_OPTION_LOOKUP) != 0)
  {
    d = InternedStringW_hash->lookupStubW(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OBJECT_NOT_FOUND;
    }
  }
  else
  {
    d = InternedStringW_hash->addStubW(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  self->_string->_d = d;
  return ERR_OK;
}

static err_t FOG_CDECL InternedStringW_ctorStringW(InternedStringW* self, const StringW* str, uint32_t options)
{
  StringDataW* d = str->_d;

  if ((d->vType & VAR_FLAG_STRING_INTERNED) != 0)
  {
    self->_string->_d = d->addRef();
    return ERR_OK;
  }

  if (d->length == 0)
  {
    self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
    return ERR_OK;
  }

  uint32_t hashCode = str->getHashCode();
  AutoLock locked(InternedStringW_lock);

  if ((options & INTERNED_STRING_OPTION_LOOKUP) != 0)
  {
    d = InternedStringW_hash->lookupStubW(d->data, d->length, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OBJECT_NOT_FOUND;
    }
  }
  else
  {
    d = InternedStringW_hash->addStubW(d->data, d->length, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  self->_string->_d = d;
  return ERR_OK;
}

static void FOG_CDECL InternedStringW_dtor(InternedStringW* self)
{
  StringDataW* d = self->_string->_d;

  if (d != NULL)
    d->reference.dec();
}

// ============================================================================
// [Fog::InternedStringW - Set]
// ============================================================================

static err_t FOG_CDECL InternedStringW_setStubA(InternedStringW* self, const StubA* stub, uint32_t options)
{
  const char* sData = stub->getData();
  size_t sLength = stub->getComputedLength();

  if (!sLength)
  {
    atomicPtrXchg(&self->_string->_d, fog_api.stringw_oEmpty->_d->addRef())->reference.dec();
    return ERR_OK;
  }

  uint32_t hashCode = HashUtil::hash(StubA(sData, sLength));

  AutoLock locked(InternedStringW_lock);
  StringDataW* d;

  if ((options & INTERNED_STRING_OPTION_LOOKUP) != 0)
  {
    d = InternedStringW_hash->lookupStubA(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OBJECT_NOT_FOUND;
  }
  else
  {
    d = InternedStringW_hash->addStubA(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  atomicPtrXchg(&self->_string->_d, d)->reference.dec();
  return ERR_OK;
}

static err_t FOG_CDECL InternedStringW_setStubW(InternedStringW* self, const StubW* stub, uint32_t options)
{
  const CharW* sData = stub->getData();
  size_t sLength = stub->getComputedLength();

  if (!sLength)
  {
    atomicPtrXchg(&self->_string->_d, fog_api.stringw_oEmpty->_d->addRef())->reference.dec();
    return ERR_OK;
  }

  uint32_t hashCode = HashUtil::hash(StubW(sData, sLength));

  AutoLock locked(InternedStringW_lock);
  StringDataW* d;

  if ((options & INTERNED_STRING_OPTION_LOOKUP) != 0)
  {
    d = InternedStringW_hash->lookupStubW(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OBJECT_NOT_FOUND;
  }
  else
  {
    d = InternedStringW_hash->addStubW(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  atomicPtrXchg(&self->_string->_d, d)->reference.dec();
  return ERR_OK;
}

static err_t FOG_CDECL InternedStringW_setStringW(InternedStringW* self, const StringW* str, uint32_t options)
{
  StringDataW* d = str->_d;

  if ((d->vType & VAR_FLAG_STRING_INTERNED) != 0 || d->length == 0)
  {
    atomicPtrXchg(&self->_string->_d, d->addRef())->reference.dec();
    return ERR_OK;
  }

  uint32_t hashCode = str->getHashCode();
  AutoLock locked(InternedStringW_lock);

  if ((options & INTERNED_STRING_OPTION_LOOKUP) != 0)
  {
    d = InternedStringW_hash->lookupStubW(d->data, d->length, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OBJECT_NOT_FOUND;
  }
  else
  {
    d = InternedStringW_hash->addStubW(d->data, d->length, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  atomicPtrXchg(&self->_string->_d, d)->reference.dec();
  return ERR_OK;
}

static err_t FOG_CDECL InternedStringW_setInterned(InternedStringW* self, const InternedStringW* str)
{
  atomicPtrXchg(&self->_string->_d, str->_string->_d->addRef())->reference.dec();
  return ERR_OK;
}

// ============================================================================
// [Fog::InternedStringW - Reset]
// ============================================================================

static void FOG_CDECL InternedStringW_reset(InternedStringW* self)
{
  atomicPtrXchg(&self->_string->_d, fog_api.stringw_oEmpty->_d->addRef())->reference.dec();
}

// ============================================================================
// [Fog::InternedStringW - Eq]
// ============================================================================

static bool FOG_CDECL InternedStringW_eq(const InternedStringW* a, const InternedStringW* b)
{
  return a->_string->_d == b->_string->_d;
}

// ============================================================================
// [Fog::InternedStringCacheW - Cleanup]
// ============================================================================

static void FOG_CDECL InternedStringW_cleanup(void)
{
  InternedStringNodeW* node;

  {
    AutoLock locked(InternedStringW_lock);
    node = InternedStringW_hash->_cleanup();
  }

  while (node)
  {
    InternedStringNodeW* next = node->next;
    MemMgr::free(node);
    node = next;
  }
}

static void FOG_CDECL InternedStringW_cleanupFunc(void* closure, uint32_t reason)
{
  InternedStringW::cleanup();
}

// ============================================================================
// [Fog::InternedStringCacheW - Create]
// ============================================================================

static FOG_INLINE void InternedStringCacheW_chcopy(CharW* dst, const char* src, size_t length)
{
  for (size_t i = 0; i < length; i++)
    dst[i] = src[i];
}

static InternedStringCacheW* FOG_CDECL InternedStringCacheW_create(const char* sData, size_t sLength, size_t listLength)
//  const char* strings, size_t length, size_t count)
{
  // First subtract two NULL terminators if contained sData[]. This can happed
  // in case that sizeof() keyword was used to obtain count of characters in
  // sData.
  if (sLength >= 2 && sData[sLength-1] == '\0' && sData[sLength-2] == '\0')
    sLength--;

  // Detect count of list items.
  if (listLength == DETECT_LENGTH)
    listLength = StringUtil::countOf(sData, sLength, '\0', CASE_SENSITIVE);

  // InternedStringNodeW + InternedStringW + StringDataW + CharW[], sizeof(size_t) == alignment.
  size_t itemSize = sizeof(InternedStringW) + sizeof(InternedStringNodeW) + sizeof(StringDataW) - sizeof(CharW) + sizeof(size_t);
  size_t allocSize = sizeof(InternedStringCacheW) + itemSize * listLength + sizeof(CharW) * (sLength - listLength);

  InternedStringCacheW* self = reinterpret_cast<InternedStringCacheW*>(MemMgr::alloc(allocSize));

  if (FOG_IS_NULL(self))
    return NULL;

  self->_length = listLength;

  const char* sMark = sData;
  const char* sPtr = sData;
  const char* sEnd = sData + sLength;

  InternedStringW* pListBase = reinterpret_cast<InternedStringW*>(self + 1);
  InternedStringW* pList = pListBase;

  uint8_t* pData = reinterpret_cast<uint8_t*>(pListBase + listLength);
  size_t counter = 0;

#if defined(FOG_DEBUG)
  uint8_t* pEnd = pData + allocSize;
#endif // FOG_DEBUG

  for (;;)
  {
    if (sPtr[0] == 0)
    {
      InternedStringNodeW* node = reinterpret_cast<InternedStringNodeW*>(pData);
      pData += sizeof(InternedStringNodeW);

      StringDataW* d = reinterpret_cast<StringDataW*>(pData);
      pData += sizeof(StringDataW) - sizeof(CharW);

      size_t len = (size_t)(sPtr - sMark);

      node->next = NULL;
      node->string->_d = d;

      d->reference.init(2);
      d->vType = VAR_TYPE_STRINGW | VAR_FLAG_STRING_INTERNED | VAR_FLAG_STRING_CACHED;
      d->hashCode = HashUtil::hash(StubA(sMark, len));
      d->length = len;
      d->capacity = len;

      InternedStringCacheW_chcopy(d->data, sMark, len);
      d->data[len] = 0;
      pData = (uint8_t*)((size_t)pData + (sizeof(CharW) * len + sizeof(size_t) - 1) & ~(size_t)(sizeof(size_t) - 1));

      pList->_string->_d = d;
      pList++;
      counter++;

      if (++sPtr == sEnd)
        break;
      sMark = sPtr;
    }
    else
      sPtr++;
  }

  FOG_ASSERT(listLength == counter);
  FOG_ASSERT(pData <= pEnd);

  AutoLock locked(InternedStringW_lock);
  InternedStringW_hash->addList(pListBase, listLength);

  return self;
}

// ============================================================================
// [Fog::InternedStringCacheW - Global]
// ============================================================================

static const char InternedStringCacheW_data[] =
{
  "1.0\0"

  "#cdata-section\0"
  "#comment\0"
  "#document\0"
  "#document-fragment\0"
  "#text\0"
  
  "ANI\0"
  "APNG\0"
  "BMP\0"
  "GIF\0"
  "ICO\0"
  "JPEG\0"
  "LBM\0"
  "MNG\0"
  "PCX\0"
  "PNG\0"
  "PNM\0"
  "TGA\0"
  "TIFF\0"
  "USERPROFILE\0"
  "UTF-8\0"
  "XBM\0"
  "XML\0"
  "XPM\0"

  "a\0"
  "actualFrame\0"
  "angle\0"
  "ani\0"
  "apng\0"
  "bmp\0"
  "circle\0"
  "clip\0"
  "clip-path\0"
  "clip-rule\0"
  "clipPath\0"
  "color\0"
  "compression\0"
  "cursor\0"
  "cx\0"
  "cy\0"
  "d\0"
  "defs\0"
  "depth\0"
  "direction\0"
  "display\0"
  "dx\0"
  "dy\0"
  "ellipse\0"
  "enable-background\0"
  "encoding\0"
  "fill\0"
  "fill-opacity\0"
  "fill-rule\0"
  "filter\0"
  "flood-color\0"
  "flood-opacity\0"
  "font\0"
  "font-family\0"
  "font-size\0"
  "font-size-adjust\0"
  "font-stretch\0"
  "font-style\0"
  "font-variant\0"
  "font-weight\0"
  "framesCount\0"
  "fx\0"
  "fy\0"
  "g\0"
  "gif\0"
  "gradientTransform\0"
  "gradientUnits\0"
  "height\0"
  "ico\0"
  "id\0"
  "image\0"
  "image-rendering\0"
  "jfi\0"
  "jfif\0"
  "jpg\0"
  "jpeg\0"
  "lbm\0"
  "lengthAdjust\0"
  "letter-spacing\0"
  "lighting-color\0"
  "line\0"
  "linearGradient\0"
  "marker\0"
  "marker_end\0"
  "marker_mid\0"
  "marker_start\0"
  "mask\0"
  "mng\0"
  "name\0"
  "none\0"
  "offset\0"
  "opacity\0"
  "overflow\0"
  "path\0"
  "pattern\0"
  "patternTransform\0"
  "patternUnits\0"
  "pcx\0"
  "planes\0"
  "png\0"
  "pnm\0"
  "points\0"
  "polygon\0"
  "polyline\0"
  "preserveAspectRatio\0"
  "progress\0"
  "quality\0"
  "r\0"
  "radialGradient\0"
  "ras\0"
  "rect\0"
  "rotate\0"
  "rx\0"
  "ry\0"
  "shape-rendering\0"
  "skipFileHeader\0"
  "solidColor\0"
  "spreadMethod\0"
  "standalone\0"
  "stop\0"
  "stop-color\0"
  "stop-opacity\0"
  "stroke\0"
  "stroke-dasharray\0"
  "stroke-dashoffset\0"
  "stroke-linecap\0"
  "stroke-linejoin\0"
  "stroke-miterlimit\0"
  "stroke-opacity\0"
  "stroke-width\0"
  "style\0"
  "svg\0"
  "symbol\0"
  "text\0"
  "text-decoration\0"
  "text-rendering\0"
  "textLength\0"
  "textPath\0"
  "tga\0"
  "tif\0"
  "tiff\0"
  "transform\0"
  "tref\0"
  "tspan\0"
  "use\0"
  "version\0"
  "view\0"
  "viewBox\0"
  "visibility\0"
  "width\0"
  "word_spacing\0"
  "x\0"
  "x1\0"
  "x2\0"
  "xbm\0"
  "xlink:href\0"
  "xml\0"
  "xpm\0"
  "y\0"
  "y1\0"
  "y2\0"

  // --------------------------------------------------------------------------
  // [Fog/Core - EventLoop]
  // --------------------------------------------------------------------------

  "Core.Default\0"
  "Core.Win\0"
  "Core.Mac\0"

  "UI.Win\0"
  "UI.Mac\0"
  "UI.X11\0"
};

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void InternedString_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.internedstringw_ctor = InternedStringW_ctor;
  fog_api.internedstringw_ctorCopy = InternedStringW_ctorCopy;
  fog_api.internedstringw_ctorStubA = InternedStringW_ctorStubA;
  fog_api.internedstringw_ctorStubW = InternedStringW_ctorStubW;
  fog_api.internedstringw_ctorStringW = InternedStringW_ctorStringW;
  fog_api.internedstringw_dtor = InternedStringW_dtor;

  fog_api.internedstringw_setStubA = InternedStringW_setStubA;
  fog_api.internedstringw_setStubW = InternedStringW_setStubW;
  fog_api.internedstringw_setStringW = InternedStringW_setStringW;
  fog_api.internedstringw_setInterned = InternedStringW_setInterned;

  fog_api.internedstringw_reset = InternedStringW_reset;
  fog_api.internedstringw_eq = InternedStringW_eq;
  fog_api.internedstringw_cleanup = InternedStringW_cleanup;

  fog_api.internedstringcachew_create = InternedStringCacheW_create;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  InternedStringW_oEmpty->_string->_d = fog_api.stringw_oEmpty->_d;
  fog_api.internedstringw_oEmpty = &InternedStringW_oEmpty;

  InternedStringW_lock.init();
  InternedStringW_hash.init();

  MemMgr::registerCleanupFunc(InternedStringW_cleanupFunc, NULL);

  fog_api.internedstringcachew_oInstance = InternedStringCacheW::create(
    InternedStringCacheW_data,
    FOG_ARRAY_SIZE(InternedStringCacheW_data),
    STR_COUNT);
}

FOG_NO_EXPORT void InternedString_fini(void)
{
  MemMgr::unregisterCleanupFunc(InternedStringW_cleanupFunc, NULL);

  InternedStringW_hash.destroy();
  InternedStringW_lock.destroy();

  fog_api.internedstringcachew_oInstance = NULL;
}

} // Fog namespace
