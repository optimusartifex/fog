// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/MathVec.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Algorithm.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/ContainerUtil.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/ListReal.h>
#include <Fog/Core/Tools/ListString.h>
#include <Fog/Core/Tools/ListVar.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/Core/Tools/Var.h>

namespace Fog {

// ===========================================================================
// [Fog::List - Debug]
// ===========================================================================

#if defined(FOG_DEBUG)
#define FOG_LIST_VERIFY_DATA(d) \
  FOG_ASSERT(d->length <= d->capacity); \
  FOG_ASSERT(d->start + d->length <= d->capacity); \
  FOG_ASSERT(d->start + d->length == d->end)
#else
#define FOG_LIST_VERIFY_DATA(d) \
  FOG_NOP
#endif // FOG_DEBUG

// ===========================================================================
// [Fog::List - Global]
// ===========================================================================

static Static<ListUntypedData> List_Unknown_dEmpty;
static Static<ListUntypedData> List_StringA_dEmpty;
static Static<ListUntypedData> List_StringW_dEmpty;
static Static<ListUntypedData> List_Var_dEmpty;

static Static<ListUntyped> List_Unknown_oEmpty;
static Static<ListUntyped> List_StringA_oEmpty;
static Static<ListUntyped> List_StringW_oEmpty;
static Static<ListUntyped> List_Var_oEmpty;

// ===========================================================================
// [Fog::List - Untyped - Construction / Destruction]
// ===========================================================================

static void FOG_CDECL List_ctor(ListUntyped* self)
{
  self->_d = List_Unknown_dEmpty->addRef();
}

static void FOG_CDECL List_ctorCopy(ListUntyped* self, const ListUntyped* other)
{
  self->_d = other->_d->addRef();
}

// ===========================================================================
// [Fog::List - Untyped - IndexOf]
// ===========================================================================

template<int Direction>
static size_t FOG_CDECL List_indexOf_4B(const ListUntyped* self, const Range* range, const void* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;

  const uint8_t* p = reinterpret_cast<const uint8_t*>(d->data);
  p += (Direction == 1) ? rStart * 4 : rEnd * 4 - 4;

  uint32_t item0 = reinterpret_cast<const uint32_t*>(item)[0];
  size_t i = rEnd - rStart;

  do {
    if (reinterpret_cast<const uint32_t*>(p)[0] == item0)
      goto _Match0;
    p += Direction * 4;
  } while (--i);

  return INVALID_INDEX;

_Match0:
  return (size_t)(p - reinterpret_cast<const uint8_t*>(d->data)) / 4;
}

template<int Direction>
static size_t FOG_CDECL List_indexOf_8B(const ListUntyped* self, const Range* range, const void* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;

  const uint8_t* p = reinterpret_cast<const uint8_t*>(d->data);
  p += (Direction == 1) ? rStart * 8 : rEnd * 8 - 8;

  size_t i = rEnd - rStart;

  if (FOG_ARCH_BITS >= 64 && ((size_t)p & 0x7) == 0)
  {
    uint64_t item0 = reinterpret_cast<const uint64_t*>(item)[0];

    while (i >= 4)
    {
      if (reinterpret_cast<const uint64_t*>(p)[Direction * 0] == item0) goto _Match0;
      if (reinterpret_cast<const uint64_t*>(p)[Direction * 1] == item0) goto _Match1;
      if (reinterpret_cast<const uint64_t*>(p)[Direction * 2] == item0) goto _Match2;
      if (reinterpret_cast<const uint64_t*>(p)[Direction * 3] == item0) goto _Match3;

      p += Direction * 32;
      i -= 4;
    }

    while (i)
    {
      if (reinterpret_cast<const uint64_t*>(p)[0] == item0) goto _Match0;

      p += Direction * 8;
      i--;
    }
  }
  else
  {
    uint32_t item0 = reinterpret_cast<const uint32_t*>(item)[0];
    uint32_t item1 = reinterpret_cast<const uint32_t*>(item)[1];

    do {
      if (reinterpret_cast<const uint32_t*>(p)[0] == item0 &&
          reinterpret_cast<const uint32_t*>(p)[1] == item1)
        goto _Match0;
      p += Direction * 8;
    } while (--i);
  }

  return INVALID_INDEX;

_Match3:
  p += Direction * 8;

_Match2:
  p += Direction * 8;

_Match1:
  p += Direction * 8;

_Match0:
  return (size_t)(p - reinterpret_cast<const uint8_t*>(d->data)) / 8;
}

template<int Direction>
static size_t FOG_CDECL List_indexOf_16B(const ListUntyped* self, const Range* range, const void* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;

  const uint8_t* p = reinterpret_cast<const uint8_t*>(d->data);
  p += (Direction == 1) ? rStart * 16 : rEnd * 16 - 16;

  size_t i = rEnd - rStart;

  if (FOG_ARCH_BITS >= 64 && ((size_t)p & 0x7) == 0)
  {
    uint64_t item0 = reinterpret_cast<const uint64_t*>(item)[0];
    uint64_t item1 = reinterpret_cast<const uint64_t*>(item)[1];

    do {
      if (reinterpret_cast<const uint64_t*>(p)[0] == item0 &&
          reinterpret_cast<const uint64_t*>(p)[1] == item1)
        goto _Match;
      p += Direction * 16;
    } while (--i);
  }
  else
  {
    uint32_t item0 = reinterpret_cast<const uint32_t*>(item)[0];
    uint32_t item1 = reinterpret_cast<const uint32_t*>(item)[1];
    uint32_t item2 = reinterpret_cast<const uint32_t*>(item)[2];
    uint32_t item3 = reinterpret_cast<const uint32_t*>(item)[3];

    do {
      if (reinterpret_cast<const uint32_t*>(p)[0] == item0 &&
          reinterpret_cast<const uint32_t*>(p)[1] == item1 &&
          reinterpret_cast<const uint32_t*>(p)[2] == item2 &&
          reinterpret_cast<const uint32_t*>(p)[3] == item3)
        goto _Match;
      p += Direction * 16;
    } while (--i);
  }

  return INVALID_INDEX;

_Match:
  return (size_t)(p - reinterpret_cast<const uint8_t*>(d->data)) / 16;
}

// ===========================================================================
// [Fog::List - Untyped - BinrayEq / CustomEq]
// ===========================================================================

static bool FOG_CDECL List_Untyped_binaryEq(const ListUntyped* a, const ListUntyped* b, size_t szItemT)
{
  ListUntypedData* a_d = a->_d;
  ListUntypedData* b_d = b->_d;

  size_t length = a_d->length;
  if (length != b_d->length)
    return false;

  return MemOps::eq(a_d->data, b_d->data, length * szItemT);
}

static bool FOG_CDECL List_Untyped_customEq(const ListUntyped* a, const ListUntyped* b, size_t szItemT, EqFunc eqFunc)
{
  ListUntypedData* a_d = a->_d;
  ListUntypedData* b_d = b->_d;

  size_t length = a_d->length;
  if (length != b_d->length)
    return false;

  const uint8_t* aData = reinterpret_cast<const uint8_t*>(a_d->data);
  const uint8_t* bData = reinterpret_cast<const uint8_t*>(b_d->data);

  for (size_t i = length; i; i--)
  {
    if (!eqFunc(aData, bData))
      return false;

    aData += szItemT;
    bData += szItemT;
  }

  return true;
}

// ===========================================================================
// [Fog::List - Untyped - Data]
// ===========================================================================

static size_t FOG_CDECL List_dGetSizeOf(size_t szItemT, size_t capacity)
{
  size_t maxSize = (SIZE_MAX - sizeof(ListUntypedData)) / szItemT;
  size_t dSize = sizeof(ListUntypedData) + szItemT * capacity;

  if (capacity > maxSize)
    return 0;

  return dSize;
}

static ListUntypedData* FOG_CDECL List_dCreate(size_t szItemT, size_t capacity)
{
  FOG_ASSERT(capacity > 0);

  size_t dSize = List_dGetSizeOf(szItemT, capacity);
  if (dSize == 0)
    return NULL;

  ListUntypedData* d = (ListUntypedData*)MemMgr::alloc(dSize);
  if (FOG_IS_NULL(d))
    return NULL;

  // Init 'd'.
  d->reference.init(1);
  d->vType = VAR_TYPE_NULL | VAR_FLAG_NONE;
  FOG_PADDING_ZERO_64(d->padding0_32);

  d->capacity = capacity;
  d->length = 0;
  d->start = 0;
  d->end = 0;

  d->data = reinterpret_cast<char*>(d) + sizeof(ListUntypedData);

  FOG_PADDING_ZERO_64(d->padding1_64);
  FOG_PADDING_ZERO_32(d->padding1_32);

  return d;
}

template<size_t CHUNK_SIZE>
FOG_INLINE void List_dCopyItem(void* dst, const void* src, size_t szItemT)
{
  FOG_ASSUME(szItemT > 0);
  MemOps::copy_small(dst, src, szItemT);
}

template<>
FOG_INLINE void List_dCopyItem<4>(void* dst, const void* src, size_t szItemT)
{
  FOG_ASSUME(szItemT % 4 == 0);

  uint32_t* dst4 = reinterpret_cast<uint32_t*>(dst);
  const uint32_t* src4 = reinterpret_cast<const uint32_t*>(src);

  size_t i = szItemT >> 2;
  FOG_ASSUME(szItemT > 0);

  do {
    *dst4++ = *src4++;
  } while (--i);
}

template<size_t CHUNK_SIZE>
FOG_INLINE void List_dSwapItem(void* dst, void* src, size_t szItemT)
{
  uint8_t* dst1 = reinterpret_cast<uint8_t*>(dst);
  uint8_t* src1 = reinterpret_cast<uint8_t*>(src);

  FOG_ASSUME(szItemT > 0);
  for (size_t i = 0; i < szItemT; i++)
  {
    uint8_t t = dst1[i];
    dst1[i] = src1[i];
    src1[i] = t;
  }
}

template<>
FOG_INLINE void List_dSwapItem<4>(void* dst, void* src, size_t szItemT)
{
  uint32_t* dst1 = reinterpret_cast<uint32_t*>(dst);
  uint32_t* src1 = reinterpret_cast<uint32_t*>(src);

  szItemT >>= 2;
  FOG_ASSUME(szItemT > 0);

  for (size_t i = 0; i < szItemT; i++)
  {
    uint32_t t = dst1[i];
    dst1[i] = src1[i];
    src1[i] = t;
  }
}

// ===========================================================================
// [Fog::List - Untyped - Sort]
// ===========================================================================

static int FOG_CDECL List_compareItemAscending(const void* a, const void* b, const void* data) { return ((CompareFunc)data)(a, b); }
static int FOG_CDECL List_compareItemDescending(const void* a, const void* b, const void* data) { return -((CompareFunc)data)(a, b); }

struct CompareItemWrapper
{
  CompareExFunc func;
  const void* data;
};

static int FOG_CDECL List_compareItemWrapper(const void* a, const void* b, const void* data)
{
  const CompareItemWrapper* wrapper = reinterpret_cast<const CompareItemWrapper*>(data);
  return wrapper->func(a, b, wrapper->data);
}

static err_t FOG_CDECL ListDetached_sort(ListUntyped* self, size_t szItemT, uint32_t sortOrder, CompareFunc compareFunc)
{
  ListUntypedData* d = self->_d;
  CompareExFunc wrapper;

  if (sortOrder == SORT_ORDER_ASCENDING)
    wrapper = (CompareExFunc)List_compareItemAscending;
  else
    wrapper = (CompareExFunc)List_compareItemDescending;

  Algorithm::qsort(d->data, d->length, szItemT, wrapper, (const void*)compareFunc);
  return ERR_OK;
}

static err_t FOG_CDECL ListDetached_sortEx(ListUntyped* self, size_t szItemT, uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
{
  ListUntypedData* d = self->_d;

  if (sortOrder == SORT_ORDER_ASCENDING)
  {
    Algorithm::qsort(d->data, d->length, szItemT, compareFunc, data);
  }
  else
  {
    CompareItemWrapper wrapper;
    wrapper.func = compareFunc;
    wrapper.data = data;
    Algorithm::qsort(d->data, d->length, szItemT, List_compareItemWrapper, &wrapper);
  }

  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Simple - Data]
// ===========================================================================

static void List_Simple_dFree(ListUntypedData* d)
{
  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

static FOG_INLINE void List_Simple_dRelease(ListUntypedData* d)
{
  if (d->reference.deref())
    List_Simple_dFree(d);
}

// ===========================================================================
// [Fog::List - Simple - Construction / Destruction]
// ===========================================================================

static void FOG_CDECL List_Simple_ctorSlice(ListUntyped* self, size_t szItemT, const ListUntyped* other, const Range* range)
{
  ListUntypedData* d;
  ListUntypedData* other_d = other->_d;

  size_t length = other_d->length;
  size_t rStart, rEnd;

  if (Range::fit(rStart, rEnd, length, range))
  {
    if (rEnd - rStart == length)
      return List_ctorCopy(self, other);

    length = rEnd - rStart;
    d = List_dCreate(szItemT, length);

    if (FOG_IS_NULL(d))
      goto _Fail;

    d->vType |= other_d->vType & VAR_TYPE_MASK;
    d->length = length;
    d->end = length;
    MemOps::copy(d->data, other_d->data, length * szItemT);
  }
  else
  {
_Fail:
    d = List_Unknown_dEmpty->addRef();
  }

  self->_d = d;
}

static void FOG_CDECL List_Simple_dtor(ListUntyped* self)
{
  ListUntypedData* d = self->_d;

  if (d != NULL)
    List_Simple_dRelease(d);
}

// ===========================================================================
// [Fog::List - Simple - Sharing]
// ===========================================================================

static err_t FOG_CDECL List_Simple_detach(ListUntyped* self, size_t szItemT)
{
  ListUntypedData* d = self->_d;

  if (d->reference.get() == 1)
    return ERR_OK;

  size_t length = d->length;
  ListUntypedData* newd = (ListUntypedData*)fog_api.list_untyped_dCreate(szItemT, length);

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = length;
  newd->end = length;
  MemOps::copy(newd->data, d->data, length * szItemT);

  List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Simple - Container]
// ===========================================================================

static err_t FOG_CDECL List_Simple_reserve(ListUntyped* self, size_t szItemT, size_t capacity)
{
  ListUntypedData* d = self->_d;
  size_t length = d->length;

  if (d->reference.get() == 1 && d->capacity >= capacity)
  {
    size_t remain = d->capacity - d->end;
    size_t needed = capacity - length;

    if (remain >= needed)
      return ERR_OK;

    // Instead of creating new instance of the list data we can simply move
    // the items to accomplish the demanded capacity. This function is always
    // used to reserve space at the end of the list so it's safe.
    if (remain + d->start > needed)
    {
      char* p = d->getArray();

      MemOps::move(p, d->data, length * szItemT);
      d->start = 0;
      d->end = length;
      d->data = p;
      return ERR_OK;
    }
  }

  ListUntypedData* newd = fog_api.list_untyped_dCreate(szItemT, capacity);
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = length;
  newd->end = length;
  MemOps::copy(newd->data, d->data, length * szItemT);

  List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));
  return ERR_OK;
}

static void* FOG_CDECL List_Simple_prepare(ListUntyped* self, size_t szItemT, uint32_t cntOp, size_t length)
{
  ListUntypedData* d = self->_d;

  if (cntOp == CONTAINER_OP_REPLACE)
  {
    if (length == 0)
    {
      fog_api.list_simple_clear(self);
      return self->_d->data;
    }

    if (d->reference.get() > 1 || d->capacity < length)
    {
      ListUntypedData* newd = List_dCreate(szItemT, length);

      if (FOG_IS_NULL(newd))
        return NULL;

      newd->vType |= d->vType & VAR_TYPE_MASK;
      newd->length = length;
      newd->end = length;
      FOG_LIST_VERIFY_DATA(newd);

      List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));
      return newd->data;
    }
    else
    {
      char* p = d->getArray();

      d->length = length;
      d->start = 0;
      d->end = length;
      d->data = p;
      FOG_LIST_VERIFY_DATA(d);

      return p;
    }
  }
  else
  {
    size_t dLength = d->length;

    if (length == 0)
      return d->data + dLength * szItemT;

    if (d->reference.get() > 1 || d->capacity - d->end < length)
    {
      if (fog_api.list_simple_growRight(self, szItemT, length) != ERR_OK)
        return NULL;

      d = self->_d;
    }

    d->length += length;
    d->end += length;
    FOG_LIST_VERIFY_DATA(d);

    return d->data + dLength * szItemT;
  }
}

static err_t FOG_CDECL List_Simple_growLeft(ListUntyped* self, size_t szItemT, size_t length)
{
  ListUntypedData* d = self->_d;
  size_t remainLeft = d->start;
  size_t remainRight = d->capacity - d->end;

  if (d->reference.get() == 1 && length <= remainLeft)
    return ERR_OK;

  size_t before = d->length + remainRight;

  if (!Math::canSum(before, length))
    return ERR_RT_OUT_OF_MEMORY;

  size_t after = before + length;
  size_t optimal = ContainerUtil::getGrowCapacity(sizeof(ListUntypedData), szItemT, before, after);

  if (optimal == 0)
    return ERR_RT_OUT_OF_MEMORY;

  ListUntypedData* newd = fog_api.list_untyped_dCreate(szItemT, optimal);

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  size_t dLength = d->length;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = dLength;
  newd->end = remainLeft + dLength;
  newd->data = newd->getArray() + remainLeft * szItemT;
  FOG_LIST_VERIFY_DATA(newd);

  MemOps::copy(newd->data, d->data, dLength * szItemT);
  List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));

  return ERR_OK;
}

static err_t FOG_CDECL List_Simple_growRight(ListUntyped* self, size_t szItemT, size_t length)
{
  ListUntypedData* d = self->_d;
  size_t remainRight = d->capacity - d->end;

  if (d->reference.get() == 1 && length <= remainRight)
    return ERR_OK;

  size_t before = d->length;

  if (!Math::canSum(before, length))
    return ERR_RT_OUT_OF_MEMORY;

  size_t after = before + length;
  size_t optimal = ContainerUtil::getGrowCapacity(sizeof(ListUntypedData), szItemT, before, after);

  if (optimal == 0)
    return ERR_RT_OUT_OF_MEMORY;

  ListUntypedData* newd = fog_api.list_untyped_dCreate(szItemT, optimal);

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = before;
  newd->end = before;
  FOG_LIST_VERIFY_DATA(newd);

  MemOps::copy(newd->data, d->data, before * szItemT);
  List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));

  return ERR_OK;
}

static void FOG_CDECL List_Simple_squeeze(ListUntyped* self, size_t szItemT)
{
  ListUntypedData* d = self->_d;

  size_t capacity = d->capacity;
  size_t length = d->length;

  // 64 bytes is minimum for squeeze.
  if (capacity - length == 0 || (capacity - length) * szItemT < 64)
    return;

  ListUntypedData* newd = fog_api.list_untyped_dCreate(szItemT, length);
  if (FOG_IS_NULL(newd))
    return;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = length;
  newd->end = length;
  FOG_LIST_VERIFY_DATA(newd);

  MemOps::copy(newd->data, d->data, length * szItemT);
  List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));
}

// ===========================================================================
// [Fog::List - Simple - Accessors]
// ===========================================================================

template<size_t CHUNK_SIZE>
static err_t FOG_CDECL List_Simple_setAt(ListUntyped* self, size_t szItemT, size_t index, const void* item)
{
  ListUntypedData* d = self->_d;
  size_t length = d->length;

  if (index >= length)
    return ERR_RT_INVALID_ARGUMENT;

  if (d->reference.get() > 1)
  {
    FOG_RETURN_ON_ERROR(fog_api.list_simple_detach(self, szItemT));
    d = self->_d;
  }

  char* p = d->data + index * szItemT;
  List_dCopyItem<CHUNK_SIZE>(p, item, szItemT);

  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Simple - Clear / Reset]
// ===========================================================================

static void FOG_CDECL List_Simple_clear(ListUntyped* self)
{
  ListUntypedData* d = self->_d;

  if (d->length == 0)
    return;

  if (d->reference.get() > 1)
  {
    List_Simple_dRelease(atomicPtrXchg(&self->_d, List_Unknown_dEmpty->addRef()));
  }
  else
  {
    d->length = 0;
    d->start = 0;
    d->end = 0;
    d->data = d->getArray();
  }
}

static void FOG_CDECL List_Simple_reset(ListUntyped* self)
{
  List_Simple_dRelease(atomicPtrXchg(&self->_d, List_Unknown_dEmpty->addRef()));
}

// ===========================================================================
// [Fog::List - Simple - Op]
// ===========================================================================

static err_t FOG_CDECL List_Simple_opList(ListUntyped* self, size_t szItemT, uint32_t cntOp, const ListUntyped* src, const Range* srcRange)
{
  size_t sLength = src->_d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, sLength, srcRange))
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      fog_api.list_simple_clear(self);
    return ERR_OK;
  }

  if (self == src)
    return fog_api.list_simple_slice(self, szItemT, srcRange);

  char* p = reinterpret_cast<char*>(fog_api.list_simple_prepare(self, szItemT, cntOp, sLength));
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  MemOps::copy(p, src->_d->data + rStart * szItemT, sLength * szItemT);
  return ERR_OK;
}

static err_t FOG_CDECL List_Simple_opData(ListUntyped* self, size_t szItemT, uint32_t cntOp, const void* data, size_t dataLength)
{
  if (dataLength == 0)
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      fog_api.list_simple_clear(self);
    return ERR_OK;
  }

  char* p = reinterpret_cast<char*>(fog_api.list_simple_prepare(self, szItemT, cntOp, dataLength));
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  MemOps::copy(p, data, dataLength * szItemT);
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Simple - Append]
// ===========================================================================

template<size_t CHUNK_SIZE>
static err_t FOG_CDECL List_Simple_appendItem(ListUntyped* self, size_t szItemT, const void* item)
{
  ListUntypedData* d = self->_d;
  size_t length = d->length;

  if (d->reference.get() > 1 || d->end == d->capacity)
  {
    FOG_RETURN_ON_ERROR(fog_api.list_simple_growRight(self, szItemT, 1));
    d = self->_d;
  }

  char* p = d->data + length * szItemT;

  d->length++;
  d->end++;
  FOG_LIST_VERIFY_DATA(d);

  List_dCopyItem<CHUNK_SIZE>(p, item, szItemT);
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Simple - Insert]
// ===========================================================================

template<size_t CHUNK_SIZE>
static err_t FOG_CDECL List_Simple_insertItem(ListUntyped* self, size_t szItemT, size_t index, const void* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  if (index >= d->length)
    return fog_api.list_simple_appendItem(self, szItemT, item);

  if (d->reference.get() == 1)
  {
    size_t half = length / 2;

    if (index < half)
    {
      if (d->start == 0)
        goto _CreateNew;

      char* p = d->data - szItemT;

      d->start--;
      d->length++;
      d->data = p;
      FOG_LIST_VERIFY_DATA(d);

      MemOps::move(p, p + szItemT, index * szItemT);
      List_dCopyItem<CHUNK_SIZE>(p + index * szItemT, item, szItemT);

      return ERR_OK;
    }
    else
    {
      if (d->end == d->capacity)
        goto _CreateNew;

      char* p = d->data + index * szItemT;

      d->end++;
      d->length++;
      FOG_LIST_VERIFY_DATA(d);

      MemOps::move(p + szItemT, p, (length - index) * szItemT);
      List_dCopyItem<CHUNK_SIZE>(p, item, szItemT);

      return ERR_OK;
    }
  }

_CreateNew:
  {
    size_t after = length + 1;
    size_t grow = ContainerUtil::getGrowCapacity(sizeof(ListUntypedData), szItemT, length, after);

    if (grow == 0)
      return ERR_RT_OUT_OF_MEMORY;

    ListUntypedData* newd = List_dCreate(szItemT, grow);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    newd->vType |= d->vType & VAR_TYPE_MASK;
    newd->length = after;
    newd->end = after;

    size_t szBefore = index * szItemT;
    size_t szAfter = (length - index) * szItemT;

    char* p = newd->data;

    MemOps::copy(p, d->data, szBefore);
    p += szBefore;

    List_dCopyItem<CHUNK_SIZE>(p, item, szItemT);
    p += szItemT;

    MemOps::copy(p, d->data + szBefore, szAfter);

    List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));
    return ERR_OK;
  }
}

// ===========================================================================
// [Fog::List - Simple - Remove]
// ===========================================================================

static err_t FOG_CDECL List_Simple_remove(ListUntyped* self, size_t szItemT, const Range* range)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return ERR_OK;

  size_t rLength = rEnd - rStart;

  if (rLength == length)
  {
    fog_api.list_simple_clear(self);
    return ERR_OK;
  }

  size_t finalLength = length - rLength;

  if (d->reference.get() > 1)
  {
    ListUntypedData* newd = List_dCreate(szItemT, finalLength);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    newd->vType |= d->vType & VAR_TYPE_MASK;
    newd->length = finalLength;
    newd->end = finalLength;
    MemOps::copy(newd->data, d->data + rStart * szItemT, finalLength * szItemT);

    List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));
    return ERR_OK;
  }
  else
  {
    char* p = d->data;

    if (rStart < length - rEnd)
    {
      d->length -= rLength;
      d->start += rLength;
      d->data += rLength * szItemT;
      FOG_LIST_VERIFY_DATA(d);

      MemOps::move(p + rLength * szItemT, p, rStart * szItemT);
    }
    else
    {
      d->length -= rLength;
      d->end -= rLength;
      FOG_LIST_VERIFY_DATA(d);

      MemOps::move(p + rStart * szItemT, p + rEnd * szItemT, (length - rEnd) * szItemT);
    }

    return ERR_OK;
  }
}

// ===========================================================================
// [Fog::List - Simple - Replace]
// ===========================================================================

static err_t FOG_CDECL List_Simple_replace(ListUntyped* self, size_t szItemT, const Range* range, const ListUntyped* src, const Range* srcRange)
{
  ListUntypedData* d = self->_d;
  ListUntypedData* src_d = src->_d;

  size_t sStart, sEnd;

  if (!Range::fit(sStart, sEnd, src_d->length, srcRange))
    return ERR_OK;

  size_t before = d->length;
  size_t dStart, dEnd;

  if (!Range::fit(dStart, dEnd, before, range) && dStart >= before)
    return fog_api.list_simple_opList(self, szItemT, CONTAINER_OP_APPEND, src, srcRange);

  size_t sLength = sEnd - sStart;

  size_t gap = dEnd - dStart;
  size_t after = before - gap;

  if (!Math::canSum(after, sLength))
    return ERR_RT_OUT_OF_MEMORY;

  after += sLength;

  if (d->reference.get() > 1 || d->capacity < after || self == src)
  {
    ListUntypedData* newd = List_dCreate(szItemT, after);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    newd->vType |= d->vType & VAR_TYPE_MASK;
    newd->length = after;
    newd->end = after;
    FOG_LIST_VERIFY_DATA(newd);

    char* p = newd->data;

    MemOps::copy(p, d->data, dStart * szItemT);
    p += dStart * szItemT;

    MemOps::copy(p, src_d->data + sStart * szItemT, sLength * szItemT);
    p += sLength * szItemT;

    MemOps::copy(p, d->data + dEnd * szItemT, (before - dEnd) * szItemT);

    List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));
    return ERR_OK;
  }
  else
  {
    char* p = d->data;
    const char* src_p = src_d->data + sStart * szItemT;

    if (gap == sLength)
    {
      MemOps::copy(p + dStart * szItemT, src_p, sLength * szItemT);
      return ERR_OK;
    }

    size_t leftLength = dStart;
    size_t rightLength = before - gap - leftLength;

    if (gap > sLength)
    {
      size_t diff = gap - sLength;

      if (leftLength < rightLength)
      {
        d->start += diff;
        d->length -= diff;
        d->data += diff * szItemT;
        FOG_LIST_VERIFY_DATA(d);

        MemOps::move(p + (diff         ) * szItemT, p    , dStart * szItemT);
        MemOps::copy(p + (diff + dStart) * szItemT, src_p, sLength * szItemT);

        return ERR_OK;
      }
      else
      {
        d->end -= diff;
        d->length -= diff;
        FOG_LIST_VERIFY_DATA(d);

        MemOps::copy(p + (dStart          ) * szItemT, src_p, sLength * szItemT);
        MemOps::move(p + (dStart + sLength) * szItemT, p + (before - gap) * szItemT, rightLength * szItemT);

        return ERR_OK;
      }
    }
    else
    {
      size_t diff = sLength - gap;

      if (leftLength < rightLength && d->start >= diff)
      {
        d->start -= diff;
        d->length += diff;
        d->data -= diff * szItemT;
        FOG_LIST_VERIFY_DATA(d);

        MemOps::move(p - (diff         ) * szItemT, p    , dStart * szItemT);
        MemOps::copy(p + (dStart - diff) * szItemT, src_p, sLength * szItemT);

        return ERR_OK;
      }
      else if (leftLength > rightLength && d->capacity - d->end >= diff)
      {
        d->end += diff;
        d->length += diff;
        FOG_LIST_VERIFY_DATA(d);

        MemOps::move(p + (dEnd + diff) * szItemT, p + dEnd * szItemT, rightLength * szItemT);
        MemOps::copy(p + dStart * szItemT, src_p, sLength * szItemT);

        return ERR_OK;
      }
    }

    // Fallback if no special-path was given. We know that there is capacity
    // in the list to replace the are with a new, but we also know that we cant
    // move only to left or right. So instead of making expensive calculations
    // and checks, we move the start index to 0 and just merge the remaining
    // parts of the list.

    if (d->start != 0)
    {
      MemOps::move(d->getArray(), p, leftLength * szItemT);
      p = d->getArray() + leftLength * szItemT;
    }

    MemOps::move(p + sLength * szItemT, p + dEnd * szItemT, rightLength * szItemT);
    MemOps::copy(p, src_p, sLength * szItemT);

    d->length = after;
    d->start = 0;
    d->end = after;
    d->data = d->getArray();

    return ERR_OK;
  }
}

// ===========================================================================
// [Fog::List - Simple - Slice]
// ===========================================================================

static err_t FOG_CDECL List_Simple_slice(ListUntyped* self, size_t szItemT, const Range* range)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t sStart, sEnd;

  if (!Range::fit(sStart, sEnd, length, range))
  {
    fog_api.list_simple_clear(self);
    return ERR_OK;
  }

  if (sEnd - sStart == length)
    return ERR_OK;

  if (d->reference.get() > 1)
  {
    length = sEnd - sStart;

    ListUntypedData* newd = List_dCreate(szItemT, length);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    newd->vType |= d->vType & VAR_TYPE_MASK;
    newd->length = length;
    newd->end = length;
    FOG_LIST_VERIFY_DATA(d);

    MemOps::copy(newd->data, d->data, length * szItemT);

    List_Simple_dRelease(atomicPtrXchg(&self->_d, newd));
    return ERR_OK;
  }
  else
  {
    d->length = sEnd - sStart;
    d->start += sStart;
    d->end -= length - sEnd;
    d->data += sStart * szItemT;
    FOG_LIST_VERIFY_DATA(d);

    return ERR_OK;
  }
}

// ===========================================================================
// [Fog::List - Simple - Sort]
// ===========================================================================

static err_t FOG_CDECL List_Simple_sort(ListUntyped* self, size_t szItemT, uint32_t sortOrder, CompareFunc compareFunc)
{
  FOG_RETURN_ON_ERROR(fog_api.list_simple_detach(self, szItemT));
  return ListDetached_sort(self, szItemT, sortOrder, compareFunc);
}

static err_t FOG_CDECL List_Simple_sortEx(ListUntyped* self, size_t szItemT, uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
{
  FOG_RETURN_ON_ERROR(fog_api.list_simple_detach(self, szItemT));
  return ListDetached_sortEx(self, szItemT, sortOrder, compareFunc, data);
}

// ===========================================================================
// [Fog::List - Simple - Swap]
// ===========================================================================

template<size_t CHUNK_SIZE>
static err_t FOG_CDECL List_Simple_swapItems(ListUntyped* self, size_t szItemT, size_t index1, size_t index2)
{
  ListUntypedData* d = self->_d;
  size_t length = d->length;

  if (index1 >= length || index2 >= length)
    return ERR_RT_INVALID_ARGUMENT;

  if (d->reference.get() > 1)
  {
    FOG_RETURN_ON_ERROR(fog_api.list_simple_detach(self, szItemT));
    d = self->_d;
  }

  char* p = d->data;
  List_dSwapItem<CHUNK_SIZE>(p + index1 * szItemT, p + index2 * szItemT, szItemT);

  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Simple - Copy]
// ===========================================================================

static void FOG_CDECL List_Simple_copy(ListUntyped* self, const ListUntyped* other)
{
  List_Simple_dRelease(atomicPtrXchg(&self->_d, other->_d->addRef()));
}

// ===========================================================================
// [Fog::List - Movable - Helpers]
// ===========================================================================

static ListUntypedData* List_Unknown_getDEmptyForType(uint32_t vType)
{
  switch (vType)
  {
    case VAR_TYPE_UNKNOWN:
      return &List_Unknown_dEmpty;

    case VAR_TYPE_LIST_STRING_A:
      return &List_StringA_dEmpty;

    case VAR_TYPE_LIST_STRING_W:
      return &List_StringW_dEmpty;

    case VAR_TYPE_LIST_VAR:
      return &List_Var_dEmpty;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
  
  return NULL;
}

// ===========================================================================
// [Fog::List - Movable - Data]
// ===========================================================================

static void List_Unknown_dFree(ListUntypedData* d, const ListUntypedVTable* v)
{
  v->dtor(d->data, d->length);

  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

static FOG_INLINE void List_Unknown_dRelease(ListUntypedData* d, const ListUntypedVTable* v)
{
  if (d->reference.deref())
    List_Unknown_dFree(d, v);
}

// ===========================================================================
// [Fog::List - Movable - Construction / Destruction]
// ===========================================================================

static void FOG_CDECL List_Unknown_ctorSlice(ListUntyped* self, const ListUntypedVTable* v, const ListUntyped* other, const Range* range)
{
  ListUntypedData* d;
  ListUntypedData* other_d = other->_d;

  size_t length = other_d->length;
  size_t rStart, rEnd;

  if (Range::fit(rStart, rEnd, length, range))
  {
    if (rEnd - rStart == length)
      return List_ctorCopy(self, other);

    length = rEnd - rStart;
    d = List_dCreate(v->szItemT, length);

    if (FOG_IS_NULL(d))
      goto _Fail;

    d->vType |= other_d->vType & VAR_TYPE_MASK;
    d->length = length;
    d->end = length;
    v->ctor(d->data, other_d->data, length);
  }
  else
  {
_Fail:
    d = List_Unknown_getDEmptyForType(other_d->vType & VAR_TYPE_MASK)->addRef();
  }

  self->_d = d;
}

static void FOG_CDECL List_Unknown_dtor(ListUntyped* self, const ListUntypedVTable* v)
{
  ListUntypedData* d = self->_d;

  if (d != NULL)
    List_Unknown_dRelease(d, v);
}

// ===========================================================================
// [Fog::List - Movable - Sharing]
// ===========================================================================

static err_t FOG_CDECL List_Unknown_detach(ListUntyped* self, const ListUntypedVTable* v)
{
  ListUntypedData* d = self->_d;

  if (d->reference.get() == 1)
    return ERR_OK;

  size_t length = d->length;
  ListUntypedData* newd = (ListUntypedData*)fog_api.list_untyped_dCreate(v->szItemT, length);

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = length;
  newd->end = length;

  if (d->reference.get() == 1)
  {
    MemOps::copy(newd->data, d->data, length * v->szItemT);
    self->_d = newd;
    List_Simple_dRelease(d);
  }
  else
  {
    v->ctor(newd->data, d->data, length);
    List_Unknown_dRelease(atomicPtrXchg(&self->_d, newd), v);
  }

  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Movable - Container]
// ===========================================================================

static err_t FOG_CDECL List_Unknown_reserve(ListUntyped* self, const ListUntypedVTable* v, size_t capacity)
{
  ListUntypedData* d = self->_d;
  size_t length = d->length;

  if (d->reference.get() == 1 && d->capacity >= capacity)
  {
    size_t remain = d->capacity - d->end;
    size_t needed = capacity - length;

    if (remain >= needed)
      return ERR_OK;

    // Instead of creating new instance of the list data we can simply move
    // the items to accomplish the demanded capacity. This function is always
    // used to reserve space at the end of the list so it's safe.
    if (remain + d->start > needed)
    {
      char* p = d->getArray();

      MemOps::move(p, d->data, length * v->szItemT);
      d->start = 0;
      d->end = length;
      d->data = p;
      return ERR_OK;
    }
  }

  ListUntypedData* newd = fog_api.list_untyped_dCreate(v->szItemT, capacity);
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = length;
  newd->end = length;

  if (d->reference.get() == 1)
  {
    MemOps::copy(newd->data, d->data, length * v->szItemT);
    self->_d = newd;
    List_Simple_dRelease(d);
  }
  else
  {
    v->ctor(newd->data, d->data, length);
    List_Unknown_dRelease(atomicPtrXchg(&self->_d, newd), v);
  }

  return ERR_OK;
}

static void* FOG_CDECL List_Unknown_prepare(ListUntyped* self, const ListUntypedVTable* v, uint32_t cntOp, size_t length)
{
  ListUntypedData* d = self->_d;

  if (cntOp == CONTAINER_OP_REPLACE)
  {
    if (length == 0)
    {
      fog_api.list_unknown_clear(self, v);
      return self->_d->data;
    }

    if (d->reference.get() > 1 || d->capacity < length)
    {
      ListUntypedData* newd = List_dCreate(v->szItemT, length);

      if (FOG_IS_NULL(newd))
        return NULL;

      newd->vType |= d->vType & VAR_TYPE_MASK;
      newd->length = length;
      newd->end = length;
      FOG_LIST_VERIFY_DATA(newd);

      List_Unknown_dRelease(atomicPtrXchg(&self->_d, newd), v);
      return newd->data;
    }
    else
    {
      char* p = d->getArray();
      v->dtor(d->data, d->length);

      d->length = length;
      d->start = 0;
      d->end = length;
      d->data = p;
      FOG_LIST_VERIFY_DATA(d);

      return p;
    }
  }
  else
  {
    size_t dLength = d->length;

    if (length == 0)
      return d->data + dLength * v->szItemT;

    if (d->reference.get() > 1 || d->capacity - d->end < length)
    {
      if (fog_api.list_unknown_growRight(self, v, length) != ERR_OK)
        return NULL;

      d = self->_d;
    }

    d->length += length;
    d->end += length;
    FOG_LIST_VERIFY_DATA(d);

    return d->data + dLength * v->szItemT;
  }
}

static void* FOG_CDECL List_Unknown_prepareAppendItem(ListUntyped* self, const ListUntypedVTable* v)
{
  ListUntypedData* d = self->_d;
  size_t length = d->length;

  if (d->reference.get() > 1 || d->end == d->capacity)
  {
    if (fog_api.list_unknown_growRight(self, v, 1) != ERR_OK)
      return NULL;
    d = self->_d;
  }

  char* p = d->data + length * v->szItemT;

  d->length++;
  d->end++;
  FOG_LIST_VERIFY_DATA(d);

  return p;
}

static void* FOG_CDECL List_Unknown_prepareInsertItem(ListUntyped* self, const ListUntypedVTable* v, size_t index)
{
  ListUntypedData* d = self->_d;
  size_t szItemT = v->szItemT;

  size_t length = d->length;
  if (index >= d->length)
    return fog_api.list_unknown_prepareAppendItem(self, v);

  if (d->reference.get() == 1)
  {
    size_t half = length / 2;

    if (index < half)
    {
      if (d->start == 0)
        goto _CreateNew;

      char* p = d->data - szItemT;

      d->start--;
      d->length++;
      d->data = p;
      FOG_LIST_VERIFY_DATA(d);

      MemOps::move(p, p + szItemT, index * szItemT);
      return p + index * szItemT;
    }
    else
    {
      if (d->end == d->capacity)
        goto _CreateNew;

      char* p = d->data + index * szItemT;

      d->end++;
      d->length++;
      FOG_LIST_VERIFY_DATA(d);

      MemOps::move(p + szItemT, p, (length - index) * szItemT);
      return p;
    }
  }

_CreateNew:
  {
    size_t after = length + 1;
    size_t grow = ContainerUtil::getGrowCapacity(sizeof(ListUntypedData), szItemT, length, after);

    if (grow == 0)
      return NULL;

    ListUntypedData* newd = List_dCreate(szItemT, grow);
    if (FOG_IS_NULL(newd))
      return NULL;

    newd->vType |= d->vType & VAR_TYPE_MASK;
    newd->length = after;
    newd->end = after;

    size_t szBefore = index * szItemT;
    size_t szAfter = (length - index) * szItemT;

    char* p = newd->data;

    if (d->reference.get() == 1)
    {
      MemOps::copy(p, d->data, szBefore);
      p += szBefore;
      MemOps::copy(p + v->szItemT, d->data + szBefore, szAfter);

      self->_d = newd;
      List_Simple_dRelease(d);
      return p;
    }
    else
    {
      v->ctor(p, d->data, index);
      p += szBefore;
      v->ctor(p + v->szItemT, d->data + szBefore, length - index);

      List_Unknown_dRelease(atomicPtrXchg(&self->_d, newd), v);
      return p;
    }
  }
}

static err_t FOG_CDECL List_Unknown_growLeft(ListUntyped* self, const ListUntypedVTable* v, size_t length)
{
  ListUntypedData* d = self->_d;
  size_t remainLeft = d->start;
  size_t remainRight = d->capacity - d->end;

  if (d->reference.get() == 1 && length <= remainLeft)
    return ERR_OK;

  size_t before = d->length + remainRight;

  if (!Math::canSum(before, length))
    return ERR_RT_OUT_OF_MEMORY;

  size_t after = before + length;
  size_t optimal = ContainerUtil::getGrowCapacity(sizeof(ListUntypedData), v->szItemT, before, after);

  if (optimal == 0)
    return ERR_RT_OUT_OF_MEMORY;

  ListUntypedData* newd = fog_api.list_untyped_dCreate(v->szItemT, optimal);

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  size_t dLength = d->length;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = dLength;
  newd->end = remainLeft + dLength;
  newd->data = newd->getArray() + remainLeft * v->szItemT;
  FOG_LIST_VERIFY_DATA(newd);

  if (d->reference.get() == 1)
  {
    MemOps::copy(newd->data, d->data, dLength * v->szItemT);
    self->_d = newd;
    List_Simple_dRelease(d);
  }
  else
  {
    v->ctor(newd->data, d->data, dLength);
    self->_d = newd;
    List_Unknown_dRelease(d, v);
  }

  return ERR_OK;
}

static err_t FOG_CDECL List_Unknown_growRight(ListUntyped* self, const ListUntypedVTable* v, size_t length)
{
  ListUntypedData* d = self->_d;
  size_t remainRight = d->capacity - d->end;

  if (d->reference.get() == 1 && length <= remainRight)
    return ERR_OK;

  size_t before = d->length;

  if (!Math::canSum(before, length))
    return ERR_RT_OUT_OF_MEMORY;

  size_t after = before + length;
  size_t optimal = ContainerUtil::getGrowCapacity(sizeof(ListUntypedData), v->szItemT, before, after);

  if (optimal == 0)
    return ERR_RT_OUT_OF_MEMORY;

  ListUntypedData* newd = fog_api.list_untyped_dCreate(v->szItemT, optimal);

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = before;
  newd->end = before;
  FOG_LIST_VERIFY_DATA(newd);

  if (d->reference.get() == 1)
  {
    MemOps::copy(newd->data, d->data, before * v->szItemT);
    self->_d = newd;
    List_Simple_dRelease(d);
  }
  else
  {
    v->ctor(newd->data, d->data, before);
    self->_d = newd;
    List_Unknown_dRelease(d, v);
  }

  return ERR_OK;
}

static void FOG_CDECL List_Unknown_squeeze(ListUntyped* self, const ListUntypedVTable* v)
{
  ListUntypedData* d = self->_d;

  size_t capacity = d->capacity;
  size_t length = d->length;

  // 64 bytes is minimum for squeeze.
  if (capacity - length == 0 || (capacity - length) * v->szItemT < 64)
    return;

  ListUntypedData* newd = fog_api.list_untyped_dCreate(v->szItemT, length);
  if (FOG_IS_NULL(newd))
    return;

  newd->vType |= d->vType & VAR_TYPE_MASK;
  newd->length = length;
  newd->end = length;
  FOG_LIST_VERIFY_DATA(newd);

  if (d->reference.get() == 1)
  {
    MemOps::copy(newd->data, d->data, length * v->szItemT);
    self->_d = newd;
    List_Simple_dRelease(d);
  }
  else
  {
    v->ctor(newd->data, d->data, length);
    List_Unknown_dRelease(atomicPtrXchg(&self->_d, newd), v);
  }
}

// ===========================================================================
// [Fog::List - Movable - Clear / Reset]
// ===========================================================================

static void FOG_CDECL List_Unknown_clear(ListUntyped* self, const ListUntypedVTable* v)
{
  ListUntypedData* d = self->_d;

  if (d->length == 0)
    return;

  if (d->reference.get() > 1)
  {
    List_Unknown_dRelease(
      atomicPtrXchg(&self->_d, List_Unknown_getDEmptyForType(self->_d->vType & VAR_TYPE_MASK)->addRef()), v);
  }
  else
  {
    v->dtor(d->data, d->length);

    d->length = 0;
    d->start = 0;
    d->end = 0;
    d->data = d->getArray();
  }
}

static void FOG_CDECL List_Unknown_reset(ListUntyped* self, const ListUntypedVTable* v)
{
  List_Unknown_dRelease(
    atomicPtrXchg(&self->_d, List_Unknown_getDEmptyForType(self->_d->vType & VAR_TYPE_MASK)->addRef()), v);
}

// ===========================================================================
// [Fog::List - Movable - Op]
// ===========================================================================

static err_t FOG_CDECL List_Unknown_opList(ListUntyped* self, const ListUntypedVTable* v, uint32_t cntOp, const ListUntyped* src, const Range* srcRange)
{
  size_t sLength = src->_d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, sLength, srcRange))
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      fog_api.list_unknown_clear(self, v);
    return ERR_OK;
  }

  if (self == src)
    return fog_api.list_unknown_slice(self, v, srcRange);

  char* p = reinterpret_cast<char*>(fog_api.list_unknown_prepare(self, v, cntOp, sLength));
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  v->ctor(p, src->_d->data + rStart * v->szItemT, sLength);
  return ERR_OK;
}

static err_t FOG_CDECL List_Unknown_opData(ListUntyped* self, const ListUntypedVTable* v, uint32_t cntOp, const void* data, size_t dataLength)
{
  if (dataLength == 0)
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      fog_api.list_unknown_clear(self, v);
    return ERR_OK;
  }

  char* p = reinterpret_cast<char*>(fog_api.list_unknown_prepare(self, v, cntOp, dataLength));
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  v->ctor(p, data, dataLength);
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Movable - Remove]
// ===========================================================================

static err_t FOG_CDECL List_Unknown_remove(ListUntyped* self, const ListUntypedVTable* v, const Range* range)
{
  ListUntypedData* d = self->_d;
  size_t szItemT = v->szItemT;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return ERR_OK;

  size_t rLength = rEnd - rStart;
  if (rLength == length)
  {
    fog_api.list_unknown_clear(self, v);
    return ERR_OK;
  }

  size_t finalLength = length - rLength;

  if (d->reference.get() > 1)
  {
    ListUntypedData* newd = List_dCreate(szItemT, finalLength);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    newd->vType |= d->vType & VAR_TYPE_MASK;
    newd->length = finalLength;
    newd->end = finalLength;

    v->ctor(newd->data, d->data + rStart * szItemT, finalLength);
    List_Unknown_dRelease(atomicPtrXchg(&self->_d, newd), v);
    return ERR_OK;
  }
  else
  {
    char* p = d->data;

    if (rStart < length - rEnd)
    {
      d->length -= rLength;
      d->start += rLength;
      d->data += rLength * szItemT;
      FOG_LIST_VERIFY_DATA(d);

      MemOps::move(p + rLength * szItemT, p, rStart * szItemT);
    }
    else
    {
      d->length -= rLength;
      d->end -= rLength;
      FOG_LIST_VERIFY_DATA(d);

      MemOps::move(p + rStart * szItemT, p + rEnd * szItemT, (length - rEnd) * szItemT);
    }

    return ERR_OK;
  }
}

// ===========================================================================
// [Fog::List - Movable - Replace]
// ===========================================================================

static err_t FOG_CDECL List_Unknown_replace(ListUntyped* self, const ListUntypedVTable* v, const Range* range, const ListUntyped* src, const Range* srcRange)
{
  ListUntypedData* d = self->_d;
  size_t szItemT = v->szItemT;

  ListUntypedData* src_d = src->_d;
  size_t sStart, sEnd;

  if (!Range::fit(sStart, sEnd, src_d->length, srcRange))
    return ERR_OK;

  size_t before = d->length;
  size_t dStart, dEnd;

  if (!Range::fit(dStart, dEnd, before, range) && dStart >= before)
    return fog_api.list_unknown_opList(self, v, CONTAINER_OP_APPEND, src, srcRange);

  size_t sLength = sEnd - sStart;

  size_t gap = dEnd - dStart;
  size_t after = before - gap;

  if (!Math::canSum(after, sLength))
    return ERR_RT_OUT_OF_MEMORY;

  after += sLength;

  if (d->reference.get() > 1 || d->capacity < after || self == src)
  {
    ListUntypedData* newd = List_dCreate(szItemT, after);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    newd->vType |= d->vType & VAR_TYPE_MASK;
    newd->length = after;
    newd->end = after;
    FOG_LIST_VERIFY_DATA(newd);

    char* p = newd->data;

    if (d->reference.get() == 1)
    {
      MemOps::copy(p, d->data, dStart * szItemT);
      p += dStart * szItemT;

      v->ctor(p, src_d->data + sStart * szItemT, sLength);
      p += sLength * szItemT;

      MemOps::copy(p, d->data + dEnd * szItemT, (before - dEnd) * szItemT);
      v->dtor(d->data + dStart * szItemT, gap);

      self->_d = newd;
      List_Simple_dRelease(d);
    }
    else
    {
      v->ctor(p, d->data, dStart);
      p += dStart * szItemT;

      v->ctor(p, src_d->data + sStart * szItemT, sLength);
      p += sLength * szItemT;

      v->ctor(p, d->data + dEnd * szItemT, (before - dEnd));
      List_Unknown_dRelease(atomicPtrXchg(&self->_d, newd), v);
    }
    return ERR_OK;
  }
  else
  {
    char* p = d->data;
    const char* src_p = src_d->data + sStart * szItemT;

    v->dtor(p + dStart * szItemT, gap);

    if (gap == sLength)
    {
      v->ctor(p + dStart * szItemT, src_p, sLength);
      return ERR_OK;
    }

    size_t leftLength = dStart;
    size_t rightLength = before - gap - leftLength;

    if (gap > sLength)
    {
      size_t diff = gap - sLength;

      if (leftLength < rightLength)
      {
        d->start += diff;
        d->length -= diff;
        d->data += diff * szItemT;
        FOG_LIST_VERIFY_DATA(d);

        MemOps::move(p + (diff         ) * szItemT, p    , dStart * szItemT);
        v->ctor(p + (diff + dStart) * szItemT, src_p, sLength);

        return ERR_OK;
      }
      else
      {
        d->end -= diff;
        d->length -= diff;
        FOG_LIST_VERIFY_DATA(d);

        v->ctor(p + (dStart          ) * szItemT, src_p, sLength);
        MemOps::move(p + (dStart + sLength) * szItemT, p + (before - gap) * szItemT, rightLength * szItemT);

        return ERR_OK;
      }
    }
    else
    {
      size_t diff = sLength - gap;

      if (leftLength < rightLength && d->start >= diff)
      {
        d->start -= diff;
        d->length += diff;
        d->data -= diff * szItemT;
        FOG_LIST_VERIFY_DATA(d);

        MemOps::move(p - (diff         ) * szItemT, p    , dStart * szItemT);
        v->ctor(p + (dStart - diff) * szItemT, src_p, sLength);

        return ERR_OK;
      }
      else if (leftLength > rightLength && d->capacity - d->end >= diff)
      {
        d->end += diff;
        d->length += diff;
        FOG_LIST_VERIFY_DATA(d);

        MemOps::move(p + (dEnd + diff) * szItemT, p + dEnd * szItemT, rightLength * szItemT);
        v->ctor(p + dStart * szItemT, src_p, sLength);

        return ERR_OK;
      }
    }

    // Fallback if no special-path was given. We know that there is capacity
    // in the list to replace the are with a new, but we also know that we cant
    // move only to left or right. So instead of making expensive calculations
    // and checks, we move the start index to 0 and just merge the remaining
    // parts of the list.

    if (d->start != 0)
    {
      MemOps::move(d->getArray(), p, leftLength * szItemT);
      p = d->getArray() + leftLength * szItemT;
    }

    MemOps::move(p + sLength * szItemT, p + dEnd * szItemT, rightLength * szItemT);
    v->ctor(p, src_p, sLength);

    d->length = after;
    d->start = 0;
    d->end = after;
    d->data = d->getArray();

    return ERR_OK;
  }
}

// ===========================================================================
// [Fog::List - Movable - Slice]
// ===========================================================================

static err_t FOG_CDECL List_Unknown_slice(ListUntyped* self, const ListUntypedVTable* v, const Range* range)
{
  ListUntypedData* d = self->_d;
  size_t szItemT = v->szItemT;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
  {
    fog_api.list_unknown_clear(self, v);
    return ERR_OK;
  }

  if (rEnd - rStart == length)
    return ERR_OK;

  if (d->reference.get() > 1)
  {
    length = rEnd - rStart;

    ListUntypedData* newd = List_dCreate(szItemT, length);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    newd->vType |= d->vType & VAR_TYPE_MASK;
    newd->length = length;
    newd->end = length;
    FOG_LIST_VERIFY_DATA(d);

    v->ctor(newd->data, d->data, length);

    List_Unknown_dRelease(atomicPtrXchg(&self->_d, newd), v);
    return ERR_OK;
  }
  else
  {
    v->dtor(d->data, rStart);
    v->dtor(d->data + rEnd * szItemT, (length - rEnd));

    d->length = rEnd - rStart;
    d->start += rStart;
    d->end -= length - rEnd;
    d->data += rStart * szItemT;
    FOG_LIST_VERIFY_DATA(d);

    return ERR_OK;
  }
}

// ===========================================================================
// [Fog::List - Movable - Sort]
// ===========================================================================

static err_t FOG_CDECL List_Unknown_sort(ListUntyped* self, const ListUntypedVTable* v, uint32_t sortOrder, CompareFunc compareFunc)
{
  FOG_RETURN_ON_ERROR(fog_api.list_unknown_detach(self, v));
  return ListDetached_sort(self, v->szItemT, sortOrder, compareFunc);
}

static err_t FOG_CDECL List_Unknown_sortEx(ListUntyped* self, const ListUntypedVTable* v, uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
{
  FOG_RETURN_ON_ERROR(fog_api.list_unknown_detach(self, v));
  return ListDetached_sortEx(self, v->szItemT, sortOrder, compareFunc, data);
}

// ===========================================================================
// [Fog::List - Movable - Copy]
// ===========================================================================

static void List_Unknown_copy(ListUntyped* self, const ListUntypedVTable* v, const ListUntyped* other)
{
  List_Unknown_dRelease(atomicPtrXchg(&self->_d, other->_d->addRef()), v);
}

// ===========================================================================
// [Fog::List - Float - Op]
// ===========================================================================

static err_t FOG_CDECL List_Float_opListD(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange)
{
  // Impossible (List<float> and List<double> can't share the same data same).
  FOG_ASSERT(self != src);

  size_t sLength = src->_d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, sLength, srcRange))
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      fog_api.list_simple_clear(self);
    return ERR_OK;
  }

  float* p = reinterpret_cast<float*>(fog_api.list_simple_prepare(self, sizeof(float), cntOp, sLength));
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  Math::vFloatFromDouble(p, reinterpret_cast<const double*>(src->_d->data) + rStart, sLength);
  return ERR_OK;
}

static err_t FOG_CDECL List_Float_opDataD(ListUntyped* self, uint32_t cntOp, const void* data, size_t dataLength)
{
  if (dataLength == 0)
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      fog_api.list_simple_clear(self);
    return ERR_OK;
  }

  float* p = reinterpret_cast<float*>(fog_api.list_simple_prepare(self, sizeof(float), cntOp, dataLength));
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  Math::vFloatFromDouble(p, reinterpret_cast<const double*>(data), dataLength);
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Double - Op]
// ===========================================================================

static err_t FOG_CDECL List_Double_opListF(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange)
{
  // Impossible.
  FOG_ASSERT(self != src);

  size_t sLength = src->_d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, sLength, srcRange))
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      fog_api.list_simple_clear(self);
    return ERR_OK;
  }

  double* p = reinterpret_cast<double*>(fog_api.list_simple_prepare(self, sizeof(double), cntOp, sLength));
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  Math::vDoubleFromFloat(p, reinterpret_cast<const float*>(src->_d->data) + rStart, sLength);
  return ERR_OK;
}

static err_t FOG_CDECL List_Double_opDataF(ListUntyped* self, uint32_t cntOp, const void* data, size_t dataLength)
{
  if (dataLength == 0)
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      fog_api.list_simple_clear(self);
    return ERR_OK;
  }

  double* p = reinterpret_cast<double*>(fog_api.list_simple_prepare(self, sizeof(double), cntOp, dataLength));
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  Math::vDoubleFromFloat(p, reinterpret_cast<const float*>(data), dataLength);
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - String - VTable]
// ===========================================================================

static ListUntypedVTable List_StringA_vTable;
static ListUntypedVTable List_StringW_vTable;

static void FOG_CDECL List_StringA_vTable_ctor(void* _dst, const void* _src, size_t length)
{
  StringA* dst = reinterpret_cast<StringA*>(_dst);
  const StringA* src = reinterpret_cast<const StringA*>(_src);

  for (size_t i = 0; i < length; i++)
    dst[i]._d = src[i]._d->addRef();
}

static void FOG_CDECL List_StringW_vTable_ctor(void* _dst, const void* _src, size_t length)
{
  StringW* dst = reinterpret_cast<StringW*>(_dst);
  const StringW* src = reinterpret_cast<const StringW*>(_src);

  for (size_t i = 0; i < length; i++)
    dst[i]._d = src[i]._d->addRef();
}

static void FOG_CDECL List_StringA_vTable_dtor(void* _dst, size_t length)
{
  StringA* dst = reinterpret_cast<StringA*>(_dst);

  for (size_t i = 0; i < length; i++)
    dst[i]._d->release();
}

static void FOG_CDECL List_StringW_vTable_dtor(void* _dst, size_t length)
{
  StringW* dst = reinterpret_cast<StringW*>(_dst);

  for (size_t i = 0; i < length; i++)
    dst[i]._d->release();
}

template<typename CharT>
FOG_STATIC_INLINE_T ListUntypedVTable* List_StringT_getVTable() { return NULL; }

template<>
FOG_STATIC_INLINE_T ListUntypedVTable* List_StringT_getVTable<char>() { return &List_StringA_vTable; }

template<>
FOG_STATIC_INLINE_T ListUntypedVTable* List_StringT_getVTable<CharW>() { return &List_StringW_vTable; }

// ===========================================================================
// [Fog::List - String - DEmpty]
// ===========================================================================

template<typename CharT>
FOG_STATIC_INLINE_T ListUntypedData* List_StringT_getDEmpty() { return NULL; }

template<>
FOG_STATIC_INLINE_T ListUntypedData* List_StringT_getDEmpty<char>() { return &List_StringA_dEmpty; }

template<>
FOG_STATIC_INLINE_T ListUntypedData* List_StringT_getDEmpty<CharW>() { return &List_StringW_dEmpty; }

// ===========================================================================
// [Fog::List - String - Construction / Destruction]
// ===========================================================================

template<typename CharT>
static void FOG_CDECL List_StringT_ctor(ListUntyped* self)
{
  self->_d = List_StringT_getDEmpty<CharT>()->addRef();
}

template<typename CharT>
static void FOG_CDECL List_StringT_ctorSlice(ListUntyped* self, const ListUntyped* other, const Range* range)
{
  List_Unknown_ctorSlice(self, List_StringT_getVTable<CharT>(), other, range);
}

template<typename CharT>
static void FOG_CDECL List_StringT_dtor(ListUntyped* self)
{
  ListUntypedData* d = self->_d;

  if (d != NULL)
    List_Unknown_dRelease(self->_d, List_StringT_getVTable<CharT>());
}

// ===========================================================================
// [Fog::List - String - Sharing]
// ===========================================================================

template<typename CharT>
static err_t FOG_CDECL List_StringT_detach(ListUntyped* self)
{
  return List_Unknown_detach(self, List_StringT_getVTable<CharT>());
}

// ===========================================================================
// [Fog::List - String - Container]
// ===========================================================================

template<typename CharT>
static err_t FOG_CDECL List_StringT_reserve(ListUntyped* self, size_t capacity)
{
  return List_Unknown_reserve(self, List_StringT_getVTable<CharT>(), capacity);
}

template<typename CharT>
static void* FOG_CDECL List_StringT_prepare(ListUntyped* self, uint32_t cntOp, size_t length)
{
  return List_Unknown_prepare(self, List_StringT_getVTable<CharT>(), cntOp, length);
}

template<typename CharT>
static void* FOG_CDECL List_StringT_prepareAppendItem(ListUntyped* self)
{
  return List_Unknown_prepareAppendItem(self, List_StringT_getVTable<CharT>());
}

template<typename CharT>
static void* FOG_CDECL List_StringT_prepareInsertItem(ListUntyped* self, size_t index)
{
  return List_Unknown_prepareInsertItem(self, List_StringT_getVTable<CharT>(), index);
}

template<typename CharT>
static err_t FOG_CDECL List_StringT_growLeft(ListUntyped* self, size_t length)
{
  return List_Unknown_growLeft(self, List_StringT_getVTable<CharT>(), length);
}

template<typename CharT>
static err_t FOG_CDECL List_StringT_growRight(ListUntyped* self, size_t length)
{
  return List_Unknown_growRight(self, List_StringT_getVTable<CharT>(), length);
}

template<typename CharT>
static void FOG_CDECL List_StringT_squeeze(ListUntyped* self)
{
  List_Unknown_squeeze(self, List_StringT_getVTable<CharT>());
}

// ===========================================================================
// [Fog::List - String - Accessors]
// ===========================================================================

static err_t FOG_CDECL List_StringA_setAtStubA(ListUntyped* self, size_t index, const StubA* item)
{
  FOG_RETURN_ON_ERROR(reinterpret_cast< List<StringA>* >(self)->detach());
  return reinterpret_cast< List<StringA>* >(self)->setAtX(index, *item);
}

static err_t FOG_CDECL List_StringA_setAtStringA(ListUntyped* self, size_t index, const StringA* item)
{
  FOG_RETURN_ON_ERROR(reinterpret_cast< List<StringA>* >(self)->detach());
  return reinterpret_cast< List<StringA>* >(self)->setAtX(index, *item);
}

static err_t FOG_CDECL List_StringW_setAtStubA(ListUntyped* self, size_t index, const StubA* item)
{
  FOG_RETURN_ON_ERROR(reinterpret_cast< List<StringW>* >(self)->detach());
  return reinterpret_cast< List<StringW>* >(self)->setAtX(index, Ascii8(item->getData(), item->getLength()));
}

static err_t FOG_CDECL List_StringW_setAtStubW(ListUntyped* self, size_t index, const StubW* item)
{
  FOG_RETURN_ON_ERROR(reinterpret_cast< List<StringW>* >(self)->detach());
  return reinterpret_cast< List<StringW>* >(self)->setAtX(index, *item);
}

static err_t FOG_CDECL List_StringW_setAtStringW(ListUntyped* self, size_t index, const StringW* item)
{
  FOG_RETURN_ON_ERROR(reinterpret_cast< List<StringW>* >(self)->detach());
  return reinterpret_cast< List<StringW>* >(self)->setAtX(index, *item);
}

// ===========================================================================
// [Fog::List - String - Clear / Reset]
// ===========================================================================

template<typename CharT>
static void FOG_CDECL List_StringT_clear(ListUntyped* self)
{
  ListUntypedData* d = self->_d;

  if (d->length == 0)
    return;

  if (d->reference.get() > 1)
  {
    List_Unknown_dRelease(
      atomicPtrXchg(&self->_d, List_StringT_getDEmpty<CharT>()->addRef()),
      List_StringT_getVTable<CharT>());
  }
  else
  {
    List_StringT_getVTable<CharT>()->dtor(d->data, d->length);

    d->length = 0;
    d->start = 0;
    d->end = 0;
    d->data = d->getArray();
  }
}

template<typename CharT>
static void FOG_CDECL List_StringT_reset(ListUntyped* self)
{
  List_Unknown_dRelease(
    atomicPtrXchg(&self->_d, List_StringT_getDEmpty<CharT>()->addRef()),
    List_StringT_getVTable<CharT>());
}

// ===========================================================================
// [Fog::List - String - Op]
// ===========================================================================

template<typename CharT>
static err_t FOG_CDECL List_StringT_opList(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange)
{
  return List_Unknown_opList(self, List_StringT_getVTable<CharT>(), cntOp, src, srcRange);
}

// ===========================================================================
// [Fog::List - String - Append]
// ===========================================================================

static err_t FOG_CDECL List_StringA_appendStubA(ListUntyped* self, const StubA* item)
{
  const char* itemData = item->getData();
  size_t itemLength = item->getComputedLength();

  Static<StringA> itemString;
  itemString.initCustom1(StubA(itemData, itemLength));

  void* p;

  if (itemString->getLength() != itemLength || (p = List_StringT_prepareAppendItem<char>(self)) == NULL)
  {
    itemString.destroy();
    return ERR_RT_OUT_OF_MEMORY;
  }

  reinterpret_cast<StringA*>(p)->_d = itemString->_d;
  return ERR_OK;
}

static err_t FOG_CDECL List_StringA_appendStringA(ListUntyped* self, const StringA* item)
{
  void* p = List_StringT_prepareAppendItem<char>(self);
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  reinterpret_cast<StringA*>(p)->_d = item->_d->addRef();
  return ERR_OK;
}

static err_t FOG_CDECL List_StringW_appendStubA(ListUntyped* self, const StubA* item)
{
  const char* itemData = item->getData();
  size_t itemLength = item->getComputedLength();

  Static<StringW> itemString;
  itemString.initCustom1(Ascii8(itemData, itemLength));

  void* p;

  if (itemString->getLength() != itemLength || (p = List_StringT_prepareAppendItem<char>(self)) == NULL)
  {
    itemString.destroy();
    return ERR_RT_OUT_OF_MEMORY;
  }

  reinterpret_cast<StringW*>(p)->_d = itemString->_d;
  return ERR_OK;
}

static err_t FOG_CDECL List_StringW_appendStubW(ListUntyped* self, const StubW* item)
{
  const CharW* itemData = item->getData();
  size_t itemLength = item->getComputedLength();

  Static<StringW> itemString;
  itemString.initCustom1(StubW(itemData, itemLength));

  void* p;

  if (itemString->getLength() != itemLength || (p = List_StringT_prepareAppendItem<char>(self)) == NULL)
  {
    itemString.destroy();
    return ERR_RT_OUT_OF_MEMORY;
  }

  reinterpret_cast<StringW*>(p)->_d = itemString->_d;
  return ERR_OK;
}

static err_t FOG_CDECL List_StringW_appendStringW(ListUntyped* self, const StringW* item)
{
  void* p = List_StringT_prepareAppendItem<char>(self);
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  reinterpret_cast<StringW*>(p)->_d = item->_d->addRef();
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - String - Insert]
// ===========================================================================

static err_t FOG_CDECL List_StringA_insertStubA(ListUntyped* self, size_t index, const StubA* item)
{
  const char* itemData = item->getData();
  size_t itemLength = item->getComputedLength();

  Static<StringA> itemString;
  itemString.initCustom1(StubA(itemData, itemLength));

  void* p;

  if (itemString->getLength() != itemLength || (p = List_StringT_prepareInsertItem<char>(self, index)) == NULL)
  {
    itemString.destroy();
    return ERR_RT_OUT_OF_MEMORY;
  }

  reinterpret_cast<StringA*>(p)->_d = itemString->_d;
  return ERR_OK;
}

static err_t FOG_CDECL List_StringA_insertStringA(ListUntyped* self, size_t index, const StringA* item)
{
  void* p = List_StringT_prepareInsertItem<char>(self, index);
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  reinterpret_cast<StringA*>(p)->_d = item->_d->addRef();
  return ERR_OK;
}

static err_t FOG_CDECL List_StringW_insertStubA(ListUntyped* self, size_t index, const StubA* item)
{
  const char* itemData = item->getData();
  size_t itemLength = item->getComputedLength();

  Static<StringW> itemString;
  itemString.initCustom1(Ascii8(itemData, itemLength));

  void* p;

  if (itemString->getLength() != itemLength || (p = List_StringT_prepareInsertItem<char>(self, index)) == NULL)
  {
    itemString.destroy();
    return ERR_RT_OUT_OF_MEMORY;
  }

  reinterpret_cast<StringW*>(p)->_d = itemString->_d;
  return ERR_OK;
}

static err_t FOG_CDECL List_StringW_insertStubW(ListUntyped* self, size_t index, const StubW* item)
{
  const CharW* itemData = item->getData();
  size_t itemLength = item->getComputedLength();

  Static<StringW> itemString;
  itemString.initCustom1(StubW(itemData, itemLength));

  void* p;

  if (itemString->getLength() != itemLength || (p = List_StringT_prepareInsertItem<char>(self, index)) == NULL)
  {
    itemString.destroy();
    return ERR_RT_OUT_OF_MEMORY;
  }

  reinterpret_cast<StringW*>(p)->_d = itemString->_d;
  return ERR_OK;
}

static err_t FOG_CDECL List_StringW_insertStringW(ListUntyped* self, size_t index, const StringW* item)
{
  void* p = List_StringT_prepareInsertItem<char>(self, index);
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  reinterpret_cast<StringW*>(p)->_d = item->_d->addRef();
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - String - Remove]
// ===========================================================================

template<typename CharT>
static err_t FOG_CDECL List_StringT_remove(ListUntyped* self, const Range* range)
{
  return List_Unknown_remove(self, List_StringT_getVTable<CharT>(), range);
}

// ===========================================================================
// [Fog::List - String - Replace]
// ===========================================================================

template<typename CharT>
static err_t FOG_CDECL List_StringT_replace(ListUntyped* self, const Range* range, const ListUntyped* src, const Range* srcRange)
{
  return List_Unknown_replace(self, List_StringT_getVTable<CharT>(), range, src, srcRange);
}

// ===========================================================================
// [Fog::List - String - Slice]
// ===========================================================================

template<typename CharT>
static err_t FOG_CDECL List_StringT_slice(ListUntyped* self, const Range* range)
{
  return List_Unknown_slice(self, List_StringT_getVTable<CharT>(), range);
}

// ===========================================================================
// [Fog::List - String - IndexOf]
// ===========================================================================

template<typename CharT, typename SrcT, int Inc>
static size_t List_StringT_indexOfPrivate(const CharT_(String)* data, size_t start, size_t end, const SrcT* itemData, size_t itemLength)
{
  for (size_t i = start; i != end; i += Inc)
  {
    const CharT_(String)& s = data[i];
    if (s.getLength() == itemLength && StringUtil::eq(s.getData(), itemData, itemLength))
      return i;
  }

  return INVALID_INDEX;
}

template<typename CharT, typename SrcT>
static size_t FOG_CDECL List_StringT_indexOfStub(const ListUntyped* self, const Range* range, const SrcT_(Stub)* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;

  return List_StringT_indexOfPrivate<CharT, SrcT, 1>(reinterpret_cast<CharT_(String)*>(d->data), rStart, rEnd, item->getData(), item->getComputedLength());
}

template<typename CharT, typename SrcT>
static size_t FOG_CDECL List_StringT_indexOfString(const ListUntyped* self, const Range* range, const SrcT_(String)* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;


  return List_StringT_indexOfPrivate<CharT, SrcT, 1>(reinterpret_cast<CharT_(String)*>(d->data), rStart, rEnd, item->getData(), item->getLength());
}

// ===========================================================================
// [Fog::List - String - LastIndexOf]
// ===========================================================================

template<typename CharT, typename SrcT>
static size_t FOG_CDECL List_StringT_lastIndexOfStub(const ListUntyped* self, const Range* range, const SrcT_(Stub)* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;

  return List_StringT_indexOfPrivate<CharT, SrcT, -1>(reinterpret_cast<CharT_(String)*>(d->data), rEnd - 1, rStart - 1, item->getData(), item->getComputedLength());
}

template<typename CharT, typename SrcT>
static size_t FOG_CDECL List_StringT_lastIndexOfString(const ListUntyped* self, const Range* range, const SrcT_(String)* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;

  return List_StringT_indexOfPrivate<CharT, SrcT, -1>(reinterpret_cast<CharT_(String)*>(d->data), rEnd - 1, rStart - 1, item->getData(), item->getLength());
}

// ===========================================================================
// [Fog::List - String - Sort]
// ===========================================================================

template<typename CharT>
static err_t FOG_CDECL List_StringT_sort(ListUntyped* self, uint32_t sortOrder, CompareFunc compareFunc)
{
  FOG_RETURN_ON_ERROR(List_StringT_detach<CharT>(self));
  return ListDetached_sort(self, sizeof(CharT_(String)), sortOrder, compareFunc);
}

template<typename CharT>
static err_t FOG_CDECL List_StringT_sortEx(ListUntyped* self, uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
{
  FOG_RETURN_ON_ERROR(List_StringT_detach<CharT>(self));
  return ListDetached_sortEx(self, sizeof(CharT_(String)), sortOrder, compareFunc, data);
}

// ===========================================================================
// [Fog::List - String - Swap]
// ===========================================================================

template<typename CharT>
static err_t List_StringT_swapItems(ListUntyped* self, size_t index1, size_t index2)
{
  size_t length = self->_d->length;
  if (index1 >= length || index2 >= length)
    return ERR_RT_INVALID_ARGUMENT;

  FOG_RETURN_ON_ERROR(reinterpret_cast< List<CharT_(String)>* >(self)->detach());

  CharT_(String)* p = reinterpret_cast< CharT_(String)* >(self->_d->data);
  swap(p[index1], p[index2]);

  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Var - VTable]
// ===========================================================================

static ListUntypedVTable List_Var_vTable;

static void FOG_CDECL List_Var_vTable_ctor(void* _dst, const void* _src, size_t length)
{
  Var* dst = reinterpret_cast<Var*>(_dst);
  const Var* src = reinterpret_cast<const Var*>(_src);

  for (size_t i = 0; i < length; i++)
    dst[i]._d = src[i]._d->addRef();
}

static void FOG_CDECL List_Var_vTable_dtor(void* _dst, size_t length)
{
  Var* dst = reinterpret_cast<Var*>(_dst);

  for (size_t i = 0; i < length; i++)
    dst[i]._d->release();
}

// ===========================================================================
// [Fog::List - Var - Construction / Destruction]
// ===========================================================================

static void FOG_CDECL List_Var_ctor(ListUntyped* self)
{
  self->_d = List_Var_dEmpty->addRef();
}

static void FOG_CDECL List_Var_ctorSlice(ListUntyped* self, const ListUntyped* other, const Range* range)
{
  List_Unknown_ctorSlice(self, &List_Var_vTable, other, range);
}

static void FOG_CDECL List_Var_dtor(ListUntyped* self)
{
  ListUntypedData* d = self->_d;

  if (d != NULL)
    List_Unknown_dRelease(d, &List_Var_vTable);
}

// ===========================================================================
// [Fog::List - Var - Sharing]
// ===========================================================================

static err_t FOG_CDECL List_Var_detach(ListUntyped* self)
{
  return List_Unknown_detach(self, &List_Var_vTable);
}

// ===========================================================================
// [Fog::List - Var - Container]
// ===========================================================================

static err_t FOG_CDECL List_Var_reserve(ListUntyped* self, size_t capacity)
{
  return List_Unknown_reserve(self, &List_Var_vTable, capacity);
}

static void* FOG_CDECL List_Var_prepareAppendItem(ListUntyped* self)
{
  return List_Unknown_prepareAppendItem(self, &List_Var_vTable);
}

static void* FOG_CDECL List_Var_prepareInsertItem(ListUntyped* self, size_t index)
{
  return List_Unknown_prepareInsertItem(self, &List_Var_vTable, index);
}

static err_t FOG_CDECL List_Var_growLeft(ListUntyped* self, size_t length)
{
  return List_Unknown_growLeft(self, &List_Var_vTable, length);
}

static err_t FOG_CDECL List_Var_growRight(ListUntyped* self, size_t length)
{
  return List_Unknown_growRight(self, &List_Var_vTable, length);
}

static void FOG_CDECL List_Var_squeeze(ListUntyped* self)
{
  List_Unknown_squeeze(self, &List_Var_vTable);
}

// ===========================================================================
// [Fog::List - Var - Accessors]
// ===========================================================================

static err_t FOG_CDECL List_Var_setAt(ListUntyped* self, size_t index, const Var* item)
{
  FOG_RETURN_ON_ERROR(reinterpret_cast< List<Var>* >(self)->detach());
  return reinterpret_cast< List<Var>* >(self)->setAtX(index, *item);
}

// ===========================================================================
// [Fog::List - Var - Clear / Reset]
// ===========================================================================

static void FOG_CDECL List_Var_clear(ListUntyped* self)
{
  ListUntypedData* d = self->_d;

  if (d->length == 0)
    return;

  if (d->reference.get() > 1)
  {
    List_Unknown_dRelease(
      atomicPtrXchg(&self->_d, List_Var_dEmpty->addRef()),
      &List_Var_vTable);
  }
  else
  {
    List_Var_vTable.dtor(d->data, d->length);

    d->length = 0;
    d->start = 0;
    d->end = 0;
    d->data = d->getArray();
  }
}

static void FOG_CDECL List_Var_reset(ListUntyped* self)
{
  List_Unknown_dRelease(
    atomicPtrXchg(&self->_d, List_Var_dEmpty->addRef()),
    &List_Var_vTable);
}

// ===========================================================================
// [Fog::List - Var - Op]
// ===========================================================================

static err_t FOG_CDECL List_Var_opList(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange)
{
  return List_Unknown_opList(self, &List_Var_vTable, cntOp, src, srcRange);
}

// ===========================================================================
// [Fog::List - Var - Append]
// ===========================================================================

static err_t FOG_CDECL List_Var_append(ListUntyped* self, const Var* item)
{
  void* p = List_Var_prepareAppendItem(self);
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  reinterpret_cast<Var*>(p)->_d = item->_d->addRef();
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Var - Insert]
// ===========================================================================

static err_t FOG_CDECL List_Var_insert(ListUntyped* self, size_t index, const Var* item)
{
  void* p = List_Var_prepareInsertItem(self, index);
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  reinterpret_cast<Var*>(p)->_d = item->_d->addRef();
  return ERR_OK;
}

// ===========================================================================
// [Fog::List - Var - Remove]
// ===========================================================================

static err_t FOG_CDECL List_Var_remove(ListUntyped* self, const Range* range)
{
  return List_Unknown_remove(self, &List_Var_vTable, range);
}

// ===========================================================================
// [Fog::List - Var - Replace]
// ===========================================================================

static err_t FOG_CDECL List_Var_replace(ListUntyped* self, const Range* range, const ListUntyped* src, const Range* srcRange)
{
  return List_Unknown_replace(self, &List_Var_vTable, range, src, srcRange);
}

// ===========================================================================
// [Fog::List - Var - Slice]
// ===========================================================================

static err_t FOG_CDECL List_Var_slice(ListUntyped* self, const Range* range)
{
  return List_Unknown_slice(self, &List_Var_vTable, range);
}

// ===========================================================================
// [Fog::List - Var - IndexOf]
// ===========================================================================

template<int Inc>
static size_t List_Var_indexOfPrivate(const Var* data, size_t start, size_t end, const Var* item)
{
  for (size_t i = start; i != end; i += Inc)
  {
    const Var& v = data[i];

    if (v == *item)
      return i;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL List_Var_indexOf(const ListUntyped* self, const Range* range, const Var* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;


  return List_Var_indexOfPrivate<1>(reinterpret_cast<const Var*>(d->data), rStart, rEnd, item);
}

// ===========================================================================
// [Fog::List - Var - LastIndexOf]
// ===========================================================================

static size_t FOG_CDECL List_Var_lastIndexOf(const ListUntyped* self, const Range* range, const Var* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;

  return List_Var_indexOfPrivate<-1>(reinterpret_cast<Var*>(d->data), rEnd - 1, rStart - 1, item);
}

// ===========================================================================
// [Fog::List - Var - Sort]
// ===========================================================================

static err_t FOG_CDECL List_Var_sort(ListUntyped* self, uint32_t sortOrder, CompareFunc compareFunc)
{
  FOG_RETURN_ON_ERROR(List_Var_detach(self));
  return ListDetached_sort(self, sizeof(Var), sortOrder, compareFunc);
}

static err_t FOG_CDECL List_Var_sortEx(ListUntyped* self, uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
{
  FOG_RETURN_ON_ERROR(List_Var_detach(self));
  return ListDetached_sortEx(self, sizeof(Var), sortOrder, compareFunc, data);
}

// ===========================================================================
// [Fog::List - Var - Swap]
// ===========================================================================

static err_t List_Var_swapItems(ListUntyped* self, size_t index1, size_t index2)
{
  size_t length = self->_d->length;
  if (index1 >= length || index2 >= length)
    return ERR_RT_INVALID_ARGUMENT;

  FOG_RETURN_ON_ERROR(reinterpret_cast< List<Var>* >(self)->detach());

  Var* p = reinterpret_cast< Var* >(self->_d->data);
  swap(p[index1], p[index2]);

  return ERR_OK;
}

// ===========================================================================
// [Init / Fini]
// ===========================================================================

FOG_CPU_DECLARE_INITIALIZER_SSE2( List_init_SSE2(void) )

FOG_NO_EXPORT void List_init(void)
{
  // -------------------------------------------------------------------------
  // [Funcs]
  // -------------------------------------------------------------------------

  fog_api.list_untyped_ctor = List_ctor;
  fog_api.list_untyped_ctorCopy = List_ctorCopy;

  fog_api.list_untyped_indexOf_4B = List_indexOf_4B<1>;
  fog_api.list_untyped_indexOf_8B = List_indexOf_8B<1>;
  fog_api.list_untyped_indexOf_16B = List_indexOf_16B<1>;

  fog_api.list_untyped_lastIndexOf_4B = List_indexOf_4B<-1>;
  fog_api.list_untyped_lastIndexOf_8B = List_indexOf_8B<-1>;
  fog_api.list_untyped_lastIndexOf_16B = List_indexOf_16B<-1>;

  fog_api.list_untyped_binaryEq = List_Untyped_binaryEq;
  fog_api.list_untyped_customEq = List_Untyped_customEq;

  fog_api.list_untyped_dCreate = List_dCreate;

  fog_api.list_simple_ctorSlice = List_Simple_ctorSlice;
  fog_api.list_simple_dtor = List_Simple_dtor;

  fog_api.list_simple_detach = List_Simple_detach;
  fog_api.list_simple_reserve = List_Simple_reserve;
  fog_api.list_simple_prepare = List_Simple_prepare;
  fog_api.list_simple_growLeft = List_Simple_growLeft;
  fog_api.list_simple_growRight = List_Simple_growRight;
  fog_api.list_simple_squeeze = List_Simple_squeeze;

  fog_api.list_simple_setAt    = List_Simple_setAt<1>;
  fog_api.list_simple_setAt_4x = List_Simple_setAt<4>;

  fog_api.list_simple_clear = List_Simple_clear;
  fog_api.list_simple_reset = List_Simple_reset;

  fog_api.list_simple_opList = List_Simple_opList;
  fog_api.list_simple_opData = List_Simple_opData;

  fog_api.list_simple_appendItem    = List_Simple_appendItem<1>;
  fog_api.list_simple_appendItem_4x = List_Simple_appendItem<4>;

  fog_api.list_simple_insertItem    = List_Simple_insertItem<1>;
  fog_api.list_simple_insertItem_4x = List_Simple_insertItem<4>;

  fog_api.list_simple_remove = List_Simple_remove;
  fog_api.list_simple_replace = List_Simple_replace;
  fog_api.list_simple_slice = List_Simple_slice;

  fog_api.list_simple_sort = List_Simple_sort;
  fog_api.list_simple_sortEx = List_Simple_sortEx;
  fog_api.list_simple_swapItems    = List_Simple_swapItems<1>;
  fog_api.list_simple_swapItems_4x = List_Simple_swapItems<4>;

  fog_api.list_simple_copy = List_Simple_copy;

  fog_api.list_simple_dRelease = List_Simple_dRelease;
  fog_api.list_simple_dFree = List_Simple_dFree;

  fog_api.list_unknown_ctorSlice = List_Unknown_ctorSlice;
  fog_api.list_unknown_dtor = List_Unknown_dtor;

  fog_api.list_unknown_detach = List_Unknown_detach;
  fog_api.list_unknown_reserve = List_Unknown_reserve;
  fog_api.list_unknown_prepare = List_Unknown_prepare;
  fog_api.list_unknown_prepareAppendItem = List_Unknown_prepareAppendItem;
  fog_api.list_unknown_prepareInsertItem = List_Unknown_prepareInsertItem;
  fog_api.list_unknown_growLeft = List_Unknown_growLeft;
  fog_api.list_unknown_growRight = List_Unknown_growRight;
  fog_api.list_unknown_squeeze = List_Unknown_squeeze;

  fog_api.list_unknown_clear = List_Unknown_clear;
  fog_api.list_unknown_reset = List_Unknown_reset;

  fog_api.list_unknown_opList = List_Unknown_opList;
  fog_api.list_unknown_opData = List_Unknown_opData;

  fog_api.list_unknown_remove = List_Unknown_remove;
  fog_api.list_unknown_replace = List_Unknown_replace;
  fog_api.list_unknown_slice = List_Unknown_slice;

  fog_api.list_unknown_sort = List_Unknown_sort;
  fog_api.list_unknown_sortEx = List_Unknown_sortEx;

  fog_api.list_unknown_copy = List_Unknown_copy;

  fog_api.list_unknown_dRelease = List_Unknown_dRelease;
  fog_api.list_unknown_dFree = List_Unknown_dFree;

  fog_api.list_float_opListD = List_Float_opListD;
  fog_api.list_float_opDataD = List_Float_opDataD;

  fog_api.list_double_opListF = List_Double_opListF;
  fog_api.list_double_opDataF = List_Double_opDataF;

  fog_api.list_stringa_ctor = List_StringT_ctor<char>;
  fog_api.list_stringa_ctorSlice = List_StringT_ctorSlice<char>;
  fog_api.list_stringa_dtor = List_StringT_dtor<char>;

  fog_api.list_stringa_detach = List_StringT_detach<char>;
  fog_api.list_stringa_reserve = List_StringT_reserve<char>;
  fog_api.list_stringa_growLeft = List_StringT_growLeft<char>;
  fog_api.list_stringa_growRight = List_StringT_growRight<char>;
  fog_api.list_stringa_squeeze = List_StringT_squeeze<char>;

  fog_api.list_stringa_setAtStubA = List_StringA_setAtStubA;
  fog_api.list_stringa_setAtStringA = List_StringA_setAtStringA;

  fog_api.list_stringa_clear = List_StringT_clear<char>;
  fog_api.list_stringa_reset = List_StringT_reset<char>;

  fog_api.list_stringa_opList = List_StringT_opList<char>;

  fog_api.list_stringa_appendStubA = List_StringA_appendStubA;
  fog_api.list_stringa_appendStringA = List_StringA_appendStringA;

  fog_api.list_stringa_insertStubA = List_StringA_insertStubA;
  fog_api.list_stringa_insertStringA = List_StringA_insertStringA;

  fog_api.list_stringa_remove = List_StringT_remove<char>;
  fog_api.list_stringa_replace = List_StringT_replace<char>;
  fog_api.list_stringa_slice = List_StringT_slice<char>;

  fog_api.list_stringa_indexOfStubA = List_StringT_indexOfStub<char, char>;
  fog_api.list_stringa_indexOfStringA = List_StringT_indexOfString<char, char>;

  fog_api.list_stringa_lastIndexOfStubA = List_StringT_lastIndexOfStub<char, char>;
  fog_api.list_stringa_lastIndexOfStringA = List_StringT_lastIndexOfString<char, char>;

  fog_api.list_stringa_sort = List_StringT_sort<char>;
  fog_api.list_stringa_sortEx = List_StringT_sortEx<char>;
  fog_api.list_stringa_swapItems = List_StringT_swapItems<char>;

  fog_api.list_stringw_ctor = List_StringT_ctor<CharW>;
  fog_api.list_stringw_ctorSlice = List_StringT_ctorSlice<CharW>;
  fog_api.list_stringw_dtor = List_StringT_dtor<CharW>;

  fog_api.list_stringw_detach = List_StringT_detach<CharW>;
  fog_api.list_stringw_reserve = List_StringT_reserve<CharW>;
  fog_api.list_stringw_growLeft = List_StringT_growLeft<CharW>;
  fog_api.list_stringw_growRight = List_StringT_growRight<CharW>;
  fog_api.list_stringw_squeeze = List_StringT_squeeze<CharW>;

  fog_api.list_stringw_setAtStubA = List_StringW_setAtStubA;
  fog_api.list_stringw_setAtStubW = List_StringW_setAtStubW;
  fog_api.list_stringw_setAtStringW = List_StringW_setAtStringW;

  fog_api.list_stringw_clear = List_StringT_clear<CharW>;
  fog_api.list_stringw_reset = List_StringT_reset<CharW>;

  fog_api.list_stringw_opList = List_StringT_opList<CharW>;

  fog_api.list_stringw_appendStubA = List_StringW_appendStubA;
  fog_api.list_stringw_appendStubW = List_StringW_appendStubW;
  fog_api.list_stringw_appendStringW = List_StringW_appendStringW;

  fog_api.list_stringw_insertStubA = List_StringW_insertStubA;
  fog_api.list_stringw_insertStubW = List_StringW_insertStubW;
  fog_api.list_stringw_insertStringW = List_StringW_insertStringW;

  fog_api.list_stringw_remove = List_StringT_remove<CharW>;
  fog_api.list_stringw_replace = List_StringT_replace<CharW>;
  fog_api.list_stringw_slice = List_StringT_slice<CharW>;

  fog_api.list_stringw_indexOfStubA = List_StringT_indexOfStub<CharW, char>;
  fog_api.list_stringw_indexOfStubW = List_StringT_indexOfStub<CharW, CharW>;
  fog_api.list_stringw_indexOfStringW = List_StringT_indexOfString<CharW, CharW>;

  fog_api.list_stringw_lastIndexOfStubA = List_StringT_lastIndexOfStub<CharW, char>;
  fog_api.list_stringw_lastIndexOfStubW = List_StringT_lastIndexOfStub<CharW, CharW>;
  fog_api.list_stringw_lastIndexOfStringW = List_StringT_lastIndexOfString<CharW, CharW>;

  fog_api.list_stringw_sort = List_StringT_sort<CharW>;
  fog_api.list_stringw_sortEx = List_StringT_sortEx<CharW>;
  fog_api.list_stringw_swapItems = List_StringT_swapItems<CharW>;

  fog_api.list_var_ctor = List_Var_ctor;
  fog_api.list_var_ctorSlice = List_Var_ctorSlice;
  fog_api.list_var_dtor = List_Var_dtor;

  fog_api.list_var_detach = List_Var_detach;
  fog_api.list_var_reserve = List_Var_reserve;
  fog_api.list_var_growLeft = List_Var_growLeft;
  fog_api.list_var_growRight = List_Var_growRight;
  fog_api.list_var_squeeze = List_Var_squeeze;

  fog_api.list_var_setAt = List_Var_setAt;

  fog_api.list_var_clear = List_Var_clear;
  fog_api.list_var_reset = List_Var_reset;

  fog_api.list_var_opList = List_Var_opList;

  fog_api.list_var_append = List_Var_append;
  fog_api.list_var_insert = List_Var_insert;

  fog_api.list_var_remove = List_Var_remove;
  fog_api.list_var_replace = List_Var_replace;
  fog_api.list_var_slice = List_Var_slice;

  fog_api.list_var_indexOf = List_Var_indexOf;
  fog_api.list_var_lastIndexOf = List_Var_lastIndexOf;

  fog_api.list_var_sort = List_Var_sort;
  fog_api.list_var_sortEx = List_Var_sortEx;
  fog_api.list_var_swapItems = List_Var_swapItems;

  // -------------------------------------------------------------------------
  // [Data]
  // -------------------------------------------------------------------------

  ListUntypedData* d;

  // List<?>
  d = &List_Unknown_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_UNKNOWN | VAR_FLAG_NONE;
  d->data = d->getArray();

  List_Unknown_oEmpty->_d = d;
  fog_api.list_untyped_oEmpty = &List_Unknown_oEmpty;

  // List<StringA>
  d = &List_StringA_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_LIST_STRING_A | VAR_FLAG_NONE;
  d->data = d->getArray();

  List_StringA_oEmpty->_d = d;
  fog_api.list_stringa_oEmpty = &List_StringA_oEmpty;

  List_StringA_vTable.szItemT = sizeof(StringA);
  List_StringA_vTable.ctor = List_StringA_vTable_ctor;
  List_StringA_vTable.dtor = List_StringA_vTable_dtor;
  fog_api.list_stringa_vTable = &List_StringA_vTable;

  // List<StringW>
  d = &List_StringW_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_LIST_STRING_W | VAR_FLAG_NONE;
  d->data = d->getArray();

  List_StringW_oEmpty->_d = d;
  fog_api.list_stringw_oEmpty = &List_StringW_oEmpty;

  List_StringW_vTable.szItemT = sizeof(StringW);
  List_StringW_vTable.ctor = List_StringW_vTable_ctor;
  List_StringW_vTable.dtor = List_StringW_vTable_dtor;
  fog_api.list_stringw_vTable = &List_StringW_vTable;

  // List<Var>
  d = &List_Var_dEmpty;
  d->reference.init(1);
  d->vType = VAR_TYPE_LIST_VAR | VAR_FLAG_NONE;
  d->data = d->getArray();

  List_Var_oEmpty->_d = d;
  fog_api.list_var_oEmpty = &List_Var_oEmpty;

  List_Var_vTable.szItemT = sizeof(Var);
  List_Var_vTable.ctor = List_Var_vTable_ctor;
  List_Var_vTable.dtor = List_Var_vTable_dtor;
  fog_api.list_var_vTable = &List_Var_vTable;

  // --------------------------------------------------------------------------
  // [CPU Based Optimizations]
  // --------------------------------------------------------------------------

  FOG_CPU_USE_INITIALIZER_SSE2( List_init_SSE2() )
}

} // Fog namespace
