// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Util.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/G2d/Source/ColorStop.h>
#include <Fog/G2d/Source/ColorStopCache.h>
#include <Fog/G2d/Source/ColorStopList.h>

namespace Fog {

// ============================================================================
// [Fog::ColorStopList - Global]
// ============================================================================

static Static<ColorStopListData> ColorStopList_dEmpty;
static Static<ColorStopList> ColorStopList_oEmpty;

// ============================================================================
// [Fog::ColorStopList - Helpers]
// ============================================================================

enum COLOR_STOP_LIST_VALIDITY
{
  COLOR_STOP_LIST_INVALID_OFFSET = 0x01,
  COLOR_STOP_LIST_NOT_SORTED = 0x02
};

static uint ColorStopList_validate(const ColorStop* stops, size_t length)
{
  uint result = 0;
  if (length == 0) return result;

  float pos = stops[0].getOffset();
  if (!Math::isBounded(pos, 0.0f, 1.0f)) result |= COLOR_STOP_LIST_INVALID_OFFSET;

  for (size_t i = 1; i < length; i++)
  {
    float cur = stops[i].getOffset();
    if (pos > cur) result |= COLOR_STOP_LIST_NOT_SORTED;
    if (!Math::isBounded(cur, 0.0f, 1.0f)) result |= COLOR_STOP_LIST_INVALID_OFFSET;
    pos = cur;
  }

  return result;
}

static void ColorStopList_sort(ColorStop* stops, size_t length)
{
  // Insertion sort is used, because the order of stops with the same offset
  // must be preserved (see qsort/isort documentation for differences).
  ColorStop* i = stops + 1;
  ColorStop* j = stops;
  ColorStop* limit = stops + length;

  while (i < limit)
  {
    for (; j[0].getOffset() > j[1].getOffset(); j--)
    {
      swap(j[0], j[1]);
      if (j == stops) break;
    }

    j = i;
    i++;
  }
}

// ============================================================================
// [Fog::ColorStopList - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ColorStopList_ctor(ColorStopList* self)
{
  self->_d = ColorStopList_dEmpty->addRef();
}

static void FOG_CDECL ColorStopList_ctorCopy(ColorStopList* self, const ColorStopList* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL ColorStopList_dtor(ColorStopList* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::ColorStopList - Container]
// ============================================================================

static err_t FOG_CDECL ColorStopList_reserve(ColorStopList* self, size_t n)
{
  ColorStopListData* d = self->_d;

  if (d->reference.get() == 1 && d->capacity >= n)
    return ERR_OK;

  size_t length = d->length;
  if (n < length) n = length;

  ColorStopListData* newd = _api.colorstoplist_dCreate(n);
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->length = length;
  MemOps::copy(newd->data, d->data, length * sizeof(ColorStop));

  d = atomicPtrXchg(&self->_d, newd);
  if (d->reference.deref())
  {
    newd->stopCachePrgb32 = d->stopCachePrgb32;
    MemMgr::free(d);
  }

  return ERR_OK;
}

static void FOG_CDECL ColorStopList_squeeze(ColorStopList* self)
{
  ColorStopListData* d = self->_d;

  size_t length = d->length;
  if (length == d->capacity) return;

  ColorStopListData* newd = _api.colorstoplist_dCreate(length);
  if (FOG_IS_NULL(newd))
    return;

  newd->length = length;
  memcpy(newd->data, d->data, length * sizeof(ColorStop));

  d = atomicPtrXchg(&self->_d, newd);
  if (d->reference.deref())
  {
    newd->stopCachePrgb32 = d->stopCachePrgb32;
    MemMgr::free(d);
  }
}

// ============================================================================
// [Fog::ColorStopList - Accessors]
// ============================================================================

static err_t FOG_CDECL ColorStopList_setData(ColorStopList* self, const ColorStop* stops, size_t length)
{
  ColorStopListData* d = self->_d;

  if (FOG_UNLIKELY(length == 0))
  {
    self->clear();
    return ERR_OK;
  }

  uint validity = ColorStopList_validate(stops, length);
  if (validity & COLOR_STOP_LIST_INVALID_OFFSET)
    return ERR_RT_INVALID_ARGUMENT;

  // Detach or resize.
  if (d->reference.get() > 1 || d->capacity < length)
  {
    ColorStopListData* newd = _api.colorstoplist_dCreate(length);

    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&self->_d, newd)->release();
    d = newd;
  }

  d->length = length;
  MemOps::copy(d->data, stops, length * sizeof(ColorStop));

  if (validity & COLOR_STOP_LIST_NOT_SORTED)
    ColorStopList_sort(d->data, length);

  return ERR_OK;
}

// ============================================================================
// [Fog::ColorStopList - IsOpaque]
// ============================================================================

static bool FOG_CDECL ColorStopList_isOpaque(const ColorStopList* self)
{
  ColorStopListData* d = self->_d;

  size_t i, length = d->length;
  const ColorStop* stops = d->data;

  for (i = 0; i < length; i++)
  {
    if (!stops[i].getColor().isOpaque())
      return false;
  }

  return true;
}

static bool FOG_CDECL ColorStopList_isOpaqueARGB32(const ColorStopList* self)
{
  ColorStopListData* d = self->_d;

  size_t i, length = d->length;
  const ColorStop* stops = d->data;

  for (i = 0; i < length; i++)
  {
    if (!stops[i].getColor().isOpaqueARGB32())
      return false;
  }

  return true;
}

// ============================================================================
// [Fog::ColorStopList - Clear / Reset]
// ============================================================================

static void FOG_CDECL ColorStopList_clear(ColorStopList* self)
{
  ColorStopListData* d = self->_d;

  if (d->reference.get() > 1)
  {
    atomicPtrXchg(&self->_d, ColorStopList_dEmpty->addRef())->release();
  }
  else
  {
    d->length = 0;

    ColorStopCache* cache = atomicPtrXchg(&d->stopCachePrgb32, (ColorStopCache*)NULL);
    if (cache)
      ColorStopCache::destroy(cache);
  }
}

static void FOG_CDECL ColorStopList_reset(ColorStopList* self)
{
  atomicPtrXchg(&self->_d, ColorStopList_dEmpty->addRef())->release();
}

// ============================================================================
// [Fog::ColorStopList - Methods]
// ============================================================================

static err_t FOG_CDECL ColorStopList_addStop(ColorStopList* self, const ColorStop* stop)
{
  if (!stop->isValid())
    return ERR_RT_INVALID_ARGUMENT;

  ColorStopListData* d = self->_d;

  size_t i, length = d->length;
  const ColorStop* stops = d->data;

  for (i = 0; i < length; i++)
  {
    if (stops[i].getOffset() > stop->getOffset())
      break;
  }

  // Detach or Resize.
  if (d->reference.get() > 1 || length == d->capacity)
  {
    ColorStopListData* newd = _api.colorstoplist_dCreate(CollectionUtil::getGrowCapacity(
      sizeof(ColorStopListData) - sizeof(ColorStop), sizeof(ColorStop), length, length + 1));

    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    MemOps::copy(newd->data, d->data, i * sizeof(ColorStop));
    MemOps::copy(newd->data + i + 1, d->data + i, (length - i) * sizeof(ColorStop));
    newd->length = length;

    atomicPtrXchg(&self->_d, newd)->release();
    d = newd;
  }
  // Insert.
  else
  {
    d->destroyCache();
    MemOps::move(d->data + i, d->data + i + 1, (length - i) * sizeof(ColorStop));
  }

  d->data[i] = *stop;
  d->length++;
  return ERR_OK;
}

static err_t FOG_CDECL ColorStopList_removeOffset(ColorStopList* self, float offset)
{
  ColorStopListData* d = self->_d;

  size_t start = self->indexOf(offset);
  size_t length = d->length;

  if (start == INVALID_INDEX)
    return ERR_RT_INVALID_ARGUMENT;

  const ColorStop* stops = d->data;
  size_t end = start + 1;

  while (end < length)
  {
    if (stops[end].getOffset() != offset)
      break;
  }

  return self->removeRange(Range(start, end));
}

static err_t FOG_CDECL ColorStopList_removeStop(ColorStopList* self, const ColorStop* stop)
{
  if (!stop->isValid())
    return ERR_RT_INVALID_ARGUMENT;

  ColorStopListData* d = self->_d;

  size_t i, length = d->length;
  const ColorStop* stops = d->data;
  float stopOffset = stop->getOffset();

  for (i = 0; i < length; i++)
  {
    float offset = stops[i].getOffset();

    if (offset < stopOffset) continue;
    if (offset > stopOffset) break;

    if (stops[i]._color == stop->_color)
      return self->removeAt(i);
  }

  return ERR_RT_OBJECT_NOT_FOUND;
}

static err_t FOG_CDECL ColorStopList_removeAt(ColorStopList* self, size_t index)
{
  ColorStopListData* d = self->_d;
  size_t length = d->length;

  if (index >= length)
    return ERR_RT_INVALID_ARGUMENT;

  if (d->reference.get() == 1)
  {
    d->destroyCache();
    MemOps::move(d->data + index, d->data + index + 1, (length - index - 1) * sizeof(ColorStop));
    d->length--;
    return ERR_OK;
  }
  else
  {
    ColorStopListData* newd = _api.colorstoplist_dCreate(length);

    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    MemOps::copy(newd->data, d->data, index * sizeof(ColorStop));
    MemOps::copy(newd->data + index, d->data + index + 1, (length - index - 1) * sizeof(ColorStop));
    newd->length = length - 1;

    atomicPtrXchg(&self->_d, newd)->release();
    return ERR_OK;
  }
}

static err_t FOG_CDECL ColorStopList_removeRange(ColorStopList* self, const Range* range)
{
  ColorStopListData* d = self->_d;
  size_t length = d->length;

  size_t start = range->getStart();
  size_t end = range->getEnd();

  if (start >= length)
    return ERR_RT_INVALID_ARGUMENT;

  if (end > length)
    end = length;

  size_t rlen = end - start;

  if (d->reference.get() == 1)
  {
    d->destroyCache();
    MemOps::move(d->data + start, d->data + end, (length - end) * sizeof(ColorStop));
    d->length -= rlen;
    return ERR_OK;
  }
  else
  {
    ColorStopListData* newd = _api.colorstoplist_dCreate(length);

    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    MemOps::copy(newd->data, d->data, start * sizeof(ColorStop));
    MemOps::copy(newd->data + start, d->data + end, (length - end) * sizeof(ColorStop));
    newd->length = length - rlen;

    atomicPtrXchg(&self->_d, newd)->release();
    return ERR_OK;
  }
}

static err_t FOG_CDECL ColorStopList_removeInterval(ColorStopList* self, const IntervalF* interval)
{
  if (!interval->isValid())
    return ERR_RT_INVALID_ARGUMENT;

  ColorStopListData* d = self->_d;
  size_t length = d->length;

  if (length == 0)
    return ERR_OK;

  const ColorStop* stops = d->data;
  float min = interval->getMin();
  float max = interval->getMax();

  // Find the min/max index.
  size_t minI;
  size_t maxI;

  for (minI =    0; minI < length; minI++) { if (stops[minI].getOffset() >= min) break; }
  for (maxI = minI; maxI < length; maxI++) { if (stops[maxI].getOffset() >  max) break; }

  return (minI < maxI) ? self->removeRange(Range(minI, maxI)) : ERR_OK;
}

static size_t FOG_CDECL ColorStopList_indexOfOffset(const ColorStopList* self, float offset)
{
  ColorStopListData* d = self->_d;

  size_t i, length = d->length;
  const ColorStop* stops = d->data;

  for (i = 0; i < length; i++)
  {
    float stopOffset = stops[i].getOffset();

    if (stopOffset < offset) continue;
    if (stopOffset > offset) break;

    return i;
  }

  return INVALID_INDEX;
}

// ============================================================================
// [Fog::ColorStopList - Copy]
// ============================================================================

static err_t FOG_CDECL ColorStopList_copy(ColorStopList* self, const ColorStopList* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::ColorStopList - Equality]
// ============================================================================

static bool FOG_CDECL ColorStopList_eq(const ColorStopList* a, const ColorStopList* b)
{
  ColorStopListData* a_d = a->_d;
  ColorStopListData* b_d = b->_d;

  size_t length = a_d->length;
  if (length != b_d->length)
    return false;

  return MemOps::eq(a_d->data, b_d->data, length * sizeof(ColorStop));
}

// ============================================================================
// [Fog::ColorStopList - Data]
// ============================================================================

static ColorStopListData* FOG_CDECL ColorStopList_dCreate(size_t capacity)
{
  ColorStopListData* d = reinterpret_cast<ColorStopListData*>(
    MemMgr::alloc(ColorStopListData::getSizeOf(capacity)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = VAR_TYPE_COLOR_STOP_LIST | VAR_FLAG_NONE;
  FOG_PADDING_ZERO_64(d->padding0_32);

  d->capacity = capacity;
  d->length = 0;
  d->stopCachePrgb32 = NULL;

  return d;
}

static void FOG_CDECL ColorStopList_dFree(ColorStopListData* d)
{
  if (d->stopCachePrgb32)
    d->stopCachePrgb32->release();

  MemMgr::free(d);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ColorStopList_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.colorstoplist_ctor = ColorStopList_ctor;
  _api.colorstoplist_ctorCopy = ColorStopList_ctorCopy;
  _api.colorstoplist_dtor = ColorStopList_dtor;
  _api.colorstoplist_reserve = ColorStopList_reserve;
  _api.colorstoplist_squeeze = ColorStopList_squeeze;
  _api.colorstoplist_setData = ColorStopList_setData;
  _api.colorstoplist_isOpaque = ColorStopList_isOpaque;
  _api.colorstoplist_isOpaqueARGB32 = ColorStopList_isOpaqueARGB32;
  _api.colorstoplist_clear = ColorStopList_clear;
  _api.colorstoplist_reset = ColorStopList_reset;
  _api.colorstoplist_addStop = ColorStopList_addStop;
  _api.colorstoplist_removeOffset = ColorStopList_removeOffset;
  _api.colorstoplist_removeStop = ColorStopList_removeStop;
  _api.colorstoplist_removeAt = ColorStopList_removeAt;
  _api.colorstoplist_removeRange = ColorStopList_removeRange;
  _api.colorstoplist_removeInterval = ColorStopList_removeInterval;
  _api.colorstoplist_indexOfOffset = ColorStopList_indexOfOffset;
  _api.colorstoplist_copy = ColorStopList_copy;
  _api.colorstoplist_eq = ColorStopList_eq;

  _api.colorstoplist_dCreate = ColorStopList_dCreate;
  _api.colorstoplist_dFree = ColorStopList_dFree;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  ColorStopListData* d = &ColorStopList_dEmpty;

  d->reference.init(1);
  d->vType = VAR_TYPE_COLOR_STOP_LIST | VAR_FLAG_NONE;
  d->stopCachePrgb32 = NULL;

  _api.colorstoplist_oEmpty = ColorStopList_oEmpty.initCustom1(d);
}

} // Fog namespace
