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
// [Fog::ManagedStringNodeW]
// ============================================================================

struct FOG_NO_EXPORT ManagedStringNodeW
{
  ManagedStringNodeW* next;
  Static<StringW> string;
};

// ============================================================================
// [Fog::ManagedStringHashW]
// ============================================================================

struct FOG_NO_EXPORT ManagedStringHashW
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ManagedStringHashW();
  ~ManagedStringHashW();

  // --------------------------------------------------------------------------
  // [Add]
  // --------------------------------------------------------------------------

  StringDataW* addStubA(const char* sData, size_t sLength, uint32_t hashCode);
  StringDataW* addStubW(const CharW* sData, size_t sLength, uint32_t hashCode);

  void addList(ManagedStringW* listData, size_t listLength);

  // --------------------------------------------------------------------------
  // [Lookup]
  // --------------------------------------------------------------------------

  StringDataW* lookupStubA(const char* sData, size_t sLength, uint32_t hashCode) const;
  StringDataW* lookupStubW(const CharW* sData, size_t sLength, uint32_t hashCode) const;

  // --------------------------------------------------------------------------
  // [Management]
  // --------------------------------------------------------------------------

  void _rehash(size_t capacity);
  ManagedStringNodeW* _cleanup();

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
  ManagedStringNodeW** _data;
  ManagedStringNodeW* _staticData[1];
};

// ============================================================================
// [Fog::ManagedStringHashW - Construction / Destruction]
// ============================================================================

ManagedStringHashW::ManagedStringHashW()
{
  _capacity = 1;
  _length = 0;

  _expandCapacity = 1;
  _expandLength = HashUtil::getClosestPrime(0);

  _data = _staticData;
  _staticData[0] = NULL;
}

ManagedStringHashW::~ManagedStringHashW()
{
  if (_data != _staticData)
    MemMgr::free(_data);
}

// ============================================================================
// [Fog::ManagedStringHashW - Add]
// ============================================================================

StringDataW* ManagedStringHashW::addStubA(const char* sData, size_t sLength, uint32_t hashCode)
{
  uint32_t hashMod = hashCode % _capacity;

  ManagedStringNodeW** pPrev = &_data[hashMod];
  ManagedStringNodeW* node = *pPrev;

  while (node)
  {
    StringDataW* d = node->string->_d;

    if (d->length == sLength && StringUtil::eq(d->data, sData, sLength))
      return d->addRef();

    pPrev = &node->next;
    node = *pPrev;
  }

  node = reinterpret_cast<ManagedStringNodeW*>(
    MemMgr::alloc(sizeof(ManagedStringNodeW) + StringDataW::getSizeOf(sLength)));

  if (FOG_IS_NULL(node))
    return NULL;

  StringDataW* d = reinterpret_cast<StringDataW*>(node + 1);
  d->reference.init(2);
  d->vType = VAR_TYPE_STRINGW | VAR_FLAG_STRING_MANAGED;
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

StringDataW* ManagedStringHashW::addStubW(const CharW* sData, size_t sLength, uint32_t hashCode)
{
  uint32_t hashMod = hashCode % _capacity;

  ManagedStringNodeW** pPrev = &_data[hashMod];
  ManagedStringNodeW* node = *pPrev;

  while (node)
  {
    StringDataW* d = node->string->_d;

    if (d->length == sLength && StringUtil::eq(d->data, sData, sLength))
      return d->addRef();

    pPrev = &node->next;
    node = *pPrev;
  }

  node = reinterpret_cast<ManagedStringNodeW*>(
    MemMgr::alloc(sizeof(ManagedStringNodeW) + StringDataW::getSizeOf(sLength)));

  if (FOG_IS_NULL(node))
    return NULL;

  StringDataW* d = reinterpret_cast<StringDataW*>(node + 1);
  d->reference.init(2);
  d->vType = VAR_TYPE_STRINGW | VAR_FLAG_STRING_MANAGED;
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

void ManagedStringHashW::addList(ManagedStringW* listData, size_t listLength)
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

    ManagedStringNodeW** pPrev = &_data[hashMod];
    ManagedStringNodeW* node = *pPrev;

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
      node = reinterpret_cast<ManagedStringNodeW*>(reinterpret_cast<uint8_t*>(d) - sizeof(ManagedStringNodeW));
      node->next = NULL;
      *pPrev = node;
    }
  }
}

// ============================================================================
// [Fog::ManagedStringHashW - Lookup]
// ============================================================================

StringDataW* ManagedStringHashW::lookupStubA(const char* sData, size_t sLength, uint32_t hashCode) const
{
  uint32_t hashMod = hashCode % _capacity;
  ManagedStringNodeW* node = _data[hashMod];

  while (node)
  {
    StringDataW* d = node->string->_d;

    if (d->length == sLength && StringUtil::eq(d->data, sData, sLength))
      return d->addRef();

    node = node->next;
  }

  return NULL;
}

StringDataW* ManagedStringHashW::lookupStubW(const CharW* sData, size_t sLength, uint32_t hashCode) const
{
  uint32_t hashMod = hashCode % _capacity;
  ManagedStringNodeW* node = _data[hashMod];

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
// [Fog::ManagedStringHashW - Management]
// ============================================================================

void ManagedStringHashW::_rehash(size_t capacity)
{
  ManagedStringNodeW** oldData = _data;
  ManagedStringNodeW** newData = (ManagedStringNodeW**)MemMgr::calloc(sizeof(ManagedStringNodeW*) * capacity);

  if (FOG_IS_NULL(newData))
    return;

  size_t i, len = _capacity;
  for (i = 0; i < len; i++)
  {
    ManagedStringNodeW* node = oldData[i];

    while (node)
    {
      uint32_t hashMod = node->string->_d->hashCode % capacity;
      ManagedStringNodeW* next = node->next;

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

ManagedStringNodeW* ManagedStringHashW::_cleanup()
{
  ManagedStringNodeW* unusedNodes = NULL;
  ManagedStringNodeW** data = _data;

  size_t i, len = _capacity;
  size_t removedCount = 0;

  for (i = 0; i < len; i++)
  {
    ManagedStringNodeW** pPrev = &data[i];
    ManagedStringNodeW* node = *pPrev;

    while (node)
    {
      ManagedStringNodeW* next = node->next;
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
// [Fog::ManagedStringW - Global]
// ============================================================================

static Static<Lock> ManagedStringW_lock;
static Static<ManagedStringHashW> ManagedStringW_hash;
static Static<ManagedStringW> ManagedStringW_oEmpty;

// ============================================================================
// [Fog::ManagedStringW - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ManagedStringW_ctor(ManagedStringW* self)
{
  self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
}

static void FOG_CDECL ManagedStringW_ctorCopy(ManagedStringW* self, const ManagedStringW* other)
{
  self->_string->_d = other->_string->_d->addRef();
}

static err_t FOG_CDECL ManagedStringW_ctorStubA(ManagedStringW* self, const StubA* stub, uint32_t options)
{
  const char* sData = stub->getData();
  size_t sLength = stub->getComputedLength();

  if (!sLength)
  {
    self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
    return ERR_OK;
  }

  uint32_t hashCode = HashUtil::hash(StubA(sData, sLength));

  AutoLock locked(ManagedStringW_lock);
  StringDataW* d;

  if ((options & MANAGED_STRING_OPTION_LOOKUP) != 0)
  {
    d = ManagedStringW_hash->lookupStubA(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OBJECT_NOT_FOUND;
    }
  }
  else
  {
    d = ManagedStringW_hash->addStubA(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  self->_string->_d = d;
  return ERR_OK;
}
static err_t FOG_CDECL ManagedStringW_ctorStubW(ManagedStringW* self, const StubW* stub, uint32_t options)
{
  const CharW* sData = stub->getData();
  size_t sLength = stub->getComputedLength();

  if (!sLength)
  {
    self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
    return ERR_OK;
  }

  uint32_t hashCode = HashUtil::hash(StubW(sData, sLength));

  AutoLock locked(ManagedStringW_lock);
  StringDataW* d;

  if ((options & MANAGED_STRING_OPTION_LOOKUP) != 0)
  {
    d = ManagedStringW_hash->lookupStubW(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OBJECT_NOT_FOUND;
    }
  }
  else
  {
    d = ManagedStringW_hash->addStubW(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  self->_string->_d = d;
  return ERR_OK;
}

static err_t FOG_CDECL ManagedStringW_ctorStringW(ManagedStringW* self, const StringW* str, uint32_t options)
{
  StringDataW* d = str->_d;

  if ((d->vType & VAR_FLAG_STRING_MANAGED) != 0)
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
  AutoLock locked(ManagedStringW_lock);

  if ((options & MANAGED_STRING_OPTION_LOOKUP) != 0)
  {
    d = ManagedStringW_hash->lookupStubW(d->data, d->length, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OBJECT_NOT_FOUND;
    }
  }
  else
  {
    d = ManagedStringW_hash->addStubW(d->data, d->length, hashCode);
    if (FOG_IS_NULL(d))
    {
      self->_string->_d = fog_api.stringw_oEmpty->_d->addRef();
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  self->_string->_d = d;
  return ERR_OK;
}

static void FOG_CDECL ManagedStringW_dtor(ManagedStringW* self)
{
  StringDataW* d = self->_string->_d;

  if (d != NULL)
    d->reference.dec();
}

// ============================================================================
// [Fog::ManagedStringW - Set]
// ============================================================================

static err_t FOG_CDECL ManagedStringW_setStubA(ManagedStringW* self, const StubA* stub, uint32_t options)
{
  const char* sData = stub->getData();
  size_t sLength = stub->getComputedLength();

  if (!sLength)
  {
    atomicPtrXchg(&self->_string->_d, fog_api.stringw_oEmpty->_d->addRef())->reference.dec();
    return ERR_OK;
  }

  uint32_t hashCode = HashUtil::hash(StubA(sData, sLength));

  AutoLock locked(ManagedStringW_lock);
  StringDataW* d;

  if ((options & MANAGED_STRING_OPTION_LOOKUP) != 0)
  {
    d = ManagedStringW_hash->lookupStubA(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OBJECT_NOT_FOUND;
  }
  else
  {
    d = ManagedStringW_hash->addStubA(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  atomicPtrXchg(&self->_string->_d, d)->reference.dec();
  return ERR_OK;
}

static err_t FOG_CDECL ManagedStringW_setStubW(ManagedStringW* self, const StubW* stub, uint32_t options)
{
  const CharW* sData = stub->getData();
  size_t sLength = stub->getComputedLength();

  if (!sLength)
  {
    atomicPtrXchg(&self->_string->_d, fog_api.stringw_oEmpty->_d->addRef())->reference.dec();
    return ERR_OK;
  }

  uint32_t hashCode = HashUtil::hash(StubW(sData, sLength));

  AutoLock locked(ManagedStringW_lock);
  StringDataW* d;

  if ((options & MANAGED_STRING_OPTION_LOOKUP) != 0)
  {
    d = ManagedStringW_hash->lookupStubW(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OBJECT_NOT_FOUND;
  }
  else
  {
    d = ManagedStringW_hash->addStubW(sData, sLength, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  atomicPtrXchg(&self->_string->_d, d)->reference.dec();
  return ERR_OK;
}

static err_t FOG_CDECL ManagedStringW_setStringW(ManagedStringW* self, const StringW* str, uint32_t options)
{
  StringDataW* d = str->_d;

  if ((d->vType & VAR_FLAG_STRING_MANAGED) != 0 || d->length == 0)
  {
    atomicPtrXchg(&self->_string->_d, d->addRef())->reference.dec();
    return ERR_OK;
  }

  uint32_t hashCode = str->getHashCode();
  AutoLock locked(ManagedStringW_lock);

  if ((options & MANAGED_STRING_OPTION_LOOKUP) != 0)
  {
    d = ManagedStringW_hash->lookupStubW(d->data, d->length, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OBJECT_NOT_FOUND;
  }
  else
  {
    d = ManagedStringW_hash->addStubW(d->data, d->length, hashCode);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  atomicPtrXchg(&self->_string->_d, d)->reference.dec();
  return ERR_OK;
}

static err_t FOG_CDECL ManagedStringW_setManaged(ManagedStringW* self, const ManagedStringW* str)
{
  atomicPtrXchg(&self->_string->_d, str->_string->_d->addRef())->reference.dec();
  return ERR_OK;
}

// ============================================================================
// [Fog::ManagedStringW - Reset]
// ============================================================================

static void FOG_CDECL ManagedStringW_reset(ManagedStringW* self)
{
  atomicPtrXchg(&self->_string->_d, fog_api.stringw_oEmpty->_d->addRef())->reference.dec();
}

// ============================================================================
// [Fog::ManagedStringW - Eq]
// ============================================================================

static bool FOG_CDECL ManagedStringW_eq(const ManagedStringW* a, const ManagedStringW* b)
{
  return a->_string->_d == b->_string->_d;
}

// ============================================================================
// [Fog::ManagedStringCacheW - Cleanup]
// ============================================================================

static void FOG_CDECL ManagedStringW_cleanup(void)
{
  ManagedStringNodeW* node;

  {
    AutoLock locked(ManagedStringW_lock);
    node = ManagedStringW_hash->_cleanup();
  }

  while (node)
  {
    ManagedStringNodeW* next = node->next;
    MemMgr::free(node);
    node = next;
  }
}

static void FOG_CDECL ManagedStringW_cleanupFunc(void* closure, uint32_t reason)
{
  ManagedStringW::cleanup();
}

// ============================================================================
// [Fog::ManagedStringCacheW - Create]
// ============================================================================

static FOG_INLINE void ManagedStringCacheW_chcopy(CharW* dst, const char* src, size_t length)
{
  for (size_t i = 0; i < length; i++)
    dst[i] = src[i];
}

static ManagedStringCacheW* FOG_CDECL ManagedStringCacheW_create(const char* sData, size_t sLength, size_t listLength)
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

  // ManagedStringNodeW + ManagedStringW + StringDataW + CharW[], sizeof(size_t) == alignment.
  size_t itemSize = sizeof(ManagedStringW) + sizeof(ManagedStringNodeW) + sizeof(StringDataW) - sizeof(CharW) + sizeof(size_t);
  size_t allocSize = sizeof(ManagedStringCacheW) + itemSize * listLength + sizeof(CharW) * (sLength - listLength);

  ManagedStringCacheW* self = reinterpret_cast<ManagedStringCacheW*>(MemMgr::alloc(allocSize));

  if (FOG_IS_NULL(self))
    return NULL;

  self->_length = listLength;

  const char* sMark = sData;
  const char* sPtr = sData;
  const char* sEnd = sData + sLength;

  ManagedStringW* pListBase = reinterpret_cast<ManagedStringW*>(self + 1);
  ManagedStringW* pList = pListBase;

  uint8_t* pData = reinterpret_cast<uint8_t*>(pListBase + listLength);
  uint8_t* pEnd = pData + allocSize;
  size_t counter = 0;

  for (;;)
  {
    if (sPtr[0] == 0)
    {
      ManagedStringNodeW* node = reinterpret_cast<ManagedStringNodeW*>(pData);
      pData += sizeof(ManagedStringNodeW);

      StringDataW* d = reinterpret_cast<StringDataW*>(pData);
      pData += sizeof(StringDataW) - sizeof(CharW);

      size_t len = (size_t)(sPtr - sMark);

      node->next = NULL;
      node->string->_d = d;

      d->reference.init(2);
      d->vType = VAR_TYPE_STRINGW | VAR_FLAG_STRING_MANAGED | VAR_FLAG_STRING_CACHED;
      d->hashCode = HashUtil::hash(StubA(sMark, len));
      d->length = len;
      d->capacity = len;

      ManagedStringCacheW_chcopy(d->data, sMark, len);
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

  AutoLock locked(ManagedStringW_lock);
  ManagedStringW_hash->addList(pListBase, listLength);

  return self;
}

// ============================================================================
// [Fog::ManagedStringCacheW - Global]
// ============================================================================

static const char ManagedStringCacheW_data[] =
{
  // --------------------------------------------------------------------------
  // [Fog/Core - Object / Property System]
  // --------------------------------------------------------------------------

  "id\0"
  "name\0"

  // --------------------------------------------------------------------------
  // [Fog/Core - Xml]
  // --------------------------------------------------------------------------

  "unnamed\0"
  "id\0"
  "style\0"

  "#text\0"
  "#cdata\0"
  "#pi\0"
  "#comment\0"
  "#document\0"

  // --------------------------------------------------------------------------
  // [Fog/Core - OS]
  // --------------------------------------------------------------------------

  "USERPROFILE\0"

  // --------------------------------------------------------------------------
  // [Fog/G2d - Imaging - Image Codec]
  // --------------------------------------------------------------------------

  "width\0"
  "height\0"
  "depth\0"
  "planes\0"
  "actualFrame\0"
  "framesCount\0"
  "progress\0"
  "quality\0"
  "compression\0"
  "skipFileHeader\0"

  // --------------------------------------------------------------------------
  // [Fog/G2d - Imaging - Image Type]
  // --------------------------------------------------------------------------

  "ANI\0"
  "APNG\0"
  "BMP\0"
  "FLI\0"
  "FLC\0"
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
  "XBM\0"
  "XPM\0"

  // --------------------------------------------------------------------------
  // [Fog/G2d - Imaging - Image Extension]
  // --------------------------------------------------------------------------

  "ani\0"
  "apng\0"
  "bmp\0"
  "fli\0"
  "flc\0"
  "gif\0"
  "ico\0"
  "jfi\0"
  "jfif\0"
  "jpg\0"
  "jpeg\0"
  "lbm\0"
  "mng\0"
  "pcx\0"
  "png\0"
  "pnm\0"
  "ras\0"
  "tga\0"
  "tif\0"
  "tiff\0"
  "xbm\0"
  "xpm\0"

  // --------------------------------------------------------------------------
  // [Fog/G2d - Svg]
  // --------------------------------------------------------------------------

  "none\0"
  "a\0"
  "circle\0"
  "clipPath\0"
  "defs\0"
  "ellipse\0"
  "g\0"
  "image\0"
  "line\0"
  "linearGradient\0"
  "marker\0"
  "mask\0"
  "path\0"
  "pattern\0"
  "polygon\0"
  "polyline\0"
  "radialGradient\0"
  "rect\0"
  "solidColor\0"
  "stop\0"
  "svg\0"
  "symbol\0"
  "text\0"
  "textPath\0"
  "tref\0"
  "tspan\0"
  "use\0"
  "view\0"

  "angle\0"
  "cx\0"
  "cy\0"
  "d\0"
  "dx\0"
  "dy\0"
  "fx\0"
  "fy\0"
  "gradientTransform\0"
  "gradientUnits\0"
  "height\0"
  "lengthAdjust\0"
  "offset\0"
  "patternTransform\0"
  "patternUnits\0"
  "points\0"
  "preserveAspectRatio\0"
  "r\0"
  "rotate\0"
  "rx\0"
  "ry\0"
  "spreadMethod\0"
  "textLength\0"
  "transform\0"
  "viewBox\0"
  "width\0"
  "x\0"
  "x1\0"
  "x2\0"
  "xlink:href\0"
  "y\0"
  "y1\0"
  "y2\0"

  "clip-path\0"
  "clip-rule\0"
  "enable-background\0"
  "fill\0"
  "fill-opacity\0"
  "fill-rule\0"
  "filter\0"
  "font-family\0"
  "font-size\0"
  "letter-spacing\0"
  "mask\0"
  "opacity\0"
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
};

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ManagedString_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.managedstringw_ctor = ManagedStringW_ctor;
  fog_api.managedstringw_ctorCopy = ManagedStringW_ctorCopy;
  fog_api.managedstringw_ctorStubA = ManagedStringW_ctorStubA;
  fog_api.managedstringw_ctorStubW = ManagedStringW_ctorStubW;
  fog_api.managedstringw_ctorStringW = ManagedStringW_ctorStringW;
  fog_api.managedstringw_dtor = ManagedStringW_dtor;

  fog_api.managedstringw_setStubA = ManagedStringW_setStubA;
  fog_api.managedstringw_setStubW = ManagedStringW_setStubW;
  fog_api.managedstringw_setStringW = ManagedStringW_setStringW;
  fog_api.managedstringw_setManaged = ManagedStringW_setManaged;

  fog_api.managedstringw_reset = ManagedStringW_reset;
  fog_api.managedstringw_eq = ManagedStringW_eq;
  fog_api.managedstringw_cleanup = ManagedStringW_cleanup;

  fog_api.managedstringcachew_create = ManagedStringCacheW_create;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  ManagedStringW_oEmpty->_string->_d = fog_api.stringw_oEmpty->_d;
  fog_api.managedstringw_oEmpty = &ManagedStringW_oEmpty;

  ManagedStringW_lock.init();
  ManagedStringW_hash.init();

  MemMgr::registerCleanupFunc(ManagedStringW_cleanupFunc, NULL);

  fog_api.managedstringcachew_oInstance = ManagedStringCacheW::create(
    ManagedStringCacheW_data,
    FOG_ARRAY_SIZE(ManagedStringCacheW_data),
    STR_COUNT);
}

FOG_NO_EXPORT void ManagedString_fini(void)
{
  MemMgr::unregisterCleanupFunc(ManagedStringW_cleanupFunc, NULL);

  ManagedStringW_hash.destroy();
  ManagedStringW_lock.destroy();

  fog_api.managedstringcachew_oInstance = NULL;
}

} // Fog namespace
