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
#include <Fog/G2d/Global/Init_G2d_p.h>
#include <Fog/G2d/Source/ColorStop.h>
#include <Fog/G2d/Source/ColorStopCache.h>
#include <Fog/G2d/Source/ColorStopList.h>

namespace Fog {

// ============================================================================
// [Fog::ColorStopList - Helpers]
// ============================================================================

enum COLOR_STOP_LIST_VALIDITY
{
  COLOR_STOP_LIST_INVALID_OFFSET = 0x01,
  COLOR_STOP_LIST_NOT_SORTED = 0x02
};

static uint _G2d_ColorStopList_validate(const ColorStop* stops, sysuint_t length)
{
  uint result = 0;
  if (length == 0) return result;

  float pos = stops[0].getOffset();
  if (!Math::isBounded(pos, 0.0f, 1.0f)) result |= COLOR_STOP_LIST_INVALID_OFFSET;

  for (sysuint_t i = 1; i < length; i++)
  {
    float cur = stops[i].getOffset();
    if (pos > cur) result |= COLOR_STOP_LIST_NOT_SORTED;
    if (!Math::isBounded(cur, 0.0f, 1.0f)) result |= COLOR_STOP_LIST_INVALID_OFFSET;
    pos = cur;
  }

  return result;
}

static void _G2d_ColorStopList_sort(ColorStop* stops, sysuint_t length)
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

ColorStopList::ColorStopList() :
  _d(_dnull->ref())
{
}

ColorStopList::ColorStopList(const ColorStopList& other) :
  _d(other._d->ref())
{
}

ColorStopList::~ColorStopList()
{
  _d->deref();
}

// ============================================================================
// [Fog::ColorStopList - Data]
// ============================================================================

err_t ColorStopList::reserve(sysuint_t n)
{
  if (isDetached() && n > _d->capacity) return ERR_OK;

  sysuint_t length = _d->length;
  if (n < length) n = length;

  ColorStopListData* newd = _dalloc(n);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->length = length;
  memcpy(newd->data, _d->data, length * sizeof(ColorStop));

  ColorStopListData* oldd = atomicPtrXchg(&_d, newd);
  if (oldd->refCount.deref())
  {
    newd->stopCachePrgb32 = oldd->stopCachePrgb32;
    Memory::free(oldd);
  }

  _d = newd;
  return ERR_OK;
}

void ColorStopList::squeeze()
{
  sysuint_t length = _d->length;
  if (length == _d->capacity) return;

  ColorStopListData* newd = _dalloc(length);
  if (FOG_UNLIKELY(newd == NULL)) return;

  newd->length = length;
  memcpy(newd->data, _d->data, length * sizeof(ColorStop));

  ColorStopListData* oldd = atomicPtrXchg(&_d, newd);
  if (oldd->refCount.deref())
  {
    newd->stopCachePrgb32 = oldd->stopCachePrgb32;
    Memory::free(oldd);
  }

  _d = newd;
}

// ============================================================================
// [Fog::ColorStopList - Clear / Reset]
// ============================================================================

void ColorStopList::clear()
{
  if (!isDetached())
  {
    atomicPtrXchg(&_d, _dnull->ref())->deref();
  }
  else
  {
    _d->length = 0;

    ColorStopCache* oldc = atomicPtrXchg(&_d->stopCachePrgb32, (ColorStopCache*)NULL);
    if (oldc) ColorStopCache::destroy(oldc);
  }
}

void ColorStopList::reset()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

// ============================================================================
// [Fog::ColorStopList - IsOpaque]
// ============================================================================

bool ColorStopList::isOpaque() const
{
  sysuint_t i, length = getLength();
  const ColorStop* stops = getList();

  for (i = 0; i < length; i++)
  {
    if (!stops[i].getColor().isOpaque()) return false;
  }

  return true;
}

bool ColorStopList::isOpaque_ARGB32() const
{
  sysuint_t i, length = getLength();
  const ColorStop* stops = getList();

  for (i = 0; i < length; i++)
  {
    if (!stops[i].getColor().isOpaque_ARGB32()) return false;
  }

  return true;
}

// ============================================================================
// [Fog::ColorStopList - Accessors]
// ============================================================================

err_t ColorStopList::setList(const ColorStopList& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return ERR_OK;
}

err_t ColorStopList::setList(const List<ColorStop>& stops)
{
  return setList(stops.getData(), stops.getLength());
}

err_t ColorStopList::setList(const ColorStop* stops, sysuint_t length)
{
  if (FOG_UNLIKELY(length == 0)) { clear(); return ERR_OK; }

  uint validity = _G2d_ColorStopList_validate(stops, length);
  if (validity & COLOR_STOP_LIST_INVALID_OFFSET) return ERR_RT_INVALID_ARGUMENT;

  // Detach or Resize.
  if (!isDetached() || length < _d->capacity)
  {
    ColorStopListData* newd = _dalloc(length);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, newd)->deref();
  }

  memcpy(_d->data, stops, length * sizeof(ColorStop));
  _d->length = length;

  if (validity & COLOR_STOP_LIST_NOT_SORTED)
  {
    _G2d_ColorStopList_sort(_d->data, length);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::ColorStopList - Manipulation]
// ============================================================================

err_t ColorStopList::add(const ColorStop& stop)
{
  if (!stop.isValid()) return ERR_RT_INVALID_ARGUMENT;

  sysuint_t i, length = _d->length;
  const ColorStop* stops = _d->data;

  for (i = 0; i < length; i++)
  {
    if (stops[i].getOffset() > stop.getOffset()) break;
  }

  // Detach or Resize.
  if (!isDetached() || length == _d->capacity)
  {
    ColorStopListData* newd = _dalloc(Util::getGrowCapacity(
      sizeof(ColorStopListData) - sizeof(ColorStop), sizeof(ColorStop), length, length + 1));
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    memcpy(newd->data, _d->data, i * sizeof(ColorStop));
    memcpy(newd->data + i + 1, _d->data + i, (length - i) * sizeof(ColorStop));
    newd->length = length;

    atomicPtrXchg(&_d, newd)->deref();
  }
  // Insert.
  else
  {
    _d->destroyCache();
    memmove(_d->data + i, _d->data + i + 1, (length - i) * sizeof(ColorStop));
  }

  _d->data[i] = stop;
  _d->length++;
  return ERR_OK;
}

err_t ColorStopList::remove(float offset)
{
  sysuint_t start = indexOf(offset);
  sysuint_t length = _d->length;
  if (start == INVALID_INDEX) return ERR_RT_INVALID_ARGUMENT;

  const ColorStop* stops = _d->data;
  sysuint_t end = start + 1;

  while (end < length)
  {
    if (stops[end].getOffset() != offset) break;
  }

  return removeAt(Range(start, end));
}

err_t ColorStopList::remove(const ColorStop& stop)
{
  if (!stop.isValid()) return ERR_RT_INVALID_ARGUMENT;

  sysuint_t i, length = _d->length;
  const ColorStop* stops = _d->data;

  for (i = 0; i < length; i++)
  {
    float offset = stops[i].getOffset();
    if (offset < stop.getOffset()) continue;
    if (offset > stop.getOffset()) break;
    if (stops[i]._color == stop._color) return removeAt(i);
  }

  return ERR_RT_OBJECT_NOT_FOUND;
}

err_t ColorStopList::removeAt(sysuint_t index)
{
  sysuint_t length = _d->length;
  if (index >= length) return ERR_RT_INVALID_ARGUMENT;

  if (FOG_LIKELY(isDetached()))
  {
    _d->destroyCache();
    memmove(_d->data + index, _d->data + index + 1, (length - index - 1) * sizeof(ColorStop));
    _d->length--;
    return ERR_OK;
  }
  else
  {
    ColorStopListData* newd = _dalloc(length);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    memcpy(newd->data, _d->data, index * sizeof(ColorStop));
    memcpy(newd->data + index, _d->data + index + 1, (length - index - 1) * sizeof(ColorStop));
    newd->length = length - 1;

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
}

err_t ColorStopList::removeAt(const Range& range)
{
  sysuint_t start = range.getStart();
  sysuint_t end = range.getEnd();
  sysuint_t length = _d->length;

  if (start >= length) return ERR_RT_INVALID_ARGUMENT;
  if (end > length) end = length;

  sysuint_t rlen = end - start;

  if (FOG_LIKELY(isDetached()))
  {
    _d->destroyCache();
    memmove(_d->data + start, _d->data + end, (length - end) * sizeof(ColorStop));
    _d->length -= rlen;
    return ERR_OK;
  }
  else
  {
    ColorStopListData* newd = _dalloc(length);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    memcpy(newd->data, _d->data, start * sizeof(ColorStop));
    memcpy(newd->data + start, _d->data + end, (length - end) * sizeof(ColorStop));
    newd->length = length - rlen;

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
}

err_t ColorStopList::removeAt(const RangeF& range)
{
  if (!range.isValid()) return ERR_RT_INVALID_ARGUMENT;

  sysuint_t len = _d->length;
  if (len == 0) return ERR_OK;

  const ColorStop* stops = _d->data;
  float startOffset = range.getStart();
  float endOffset = range.getEnd();

  sysuint_t startIndex;
  sysuint_t endIndex;

  // Find the start index.
  for (startIndex = 0; startIndex < len; startIndex++)
  {
    if (stops[startIndex].getOffset() >= startOffset) break;
  }

  // Find the end index.
  for (endIndex = startIndex; endIndex < len; endIndex++)
  {
    if (stops[endIndex].getOffset() > endOffset) break;
  }

  return (startIndex < endIndex) ? removeAt(Range(startIndex, endIndex)) : ERR_OK;
}

sysuint_t ColorStopList::indexOf(float offset) const
{
  sysuint_t i, length = _d->length;
  const ColorStop* stops = _d->data;

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
// [Fog::ColorStopList - Statics]
// ============================================================================

Static<ColorStopListData> ColorStopList::_dnull;

ColorStopListData* ColorStopList::_dalloc(sysuint_t capacity)
{
  ColorStopListData* d = reinterpret_cast<ColorStopListData*>(
    Memory::alloc(ColorStopListData::sizeFor(capacity)));
  if (FOG_UNLIKELY(d == NULL)) return NULL;

  d->refCount.init(1);
  d->capacity = capacity;
  d->length = 0;
  d->stopCachePrgb32 = NULL;

  return d;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_colorstoplist_init(void)
{
  ColorStopListData* d = ColorStopList::_dnull.instancep();

  d->refCount.init(1);
  d->capacity = 0;
  d->length = 0;
  d->stopCachePrgb32 = NULL;
}

FOG_NO_EXPORT void _g2d_colorstoplist_fini(void)
{
  ColorStopList::_dnull->refCount.dec();
}

} // Fog namespace
