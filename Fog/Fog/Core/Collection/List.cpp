// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Collection/Util.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>

namespace Fog {

// ===========================================================================
// [Fog::ListPrivate_ - Debug]
// ===========================================================================

#if defined(FOG_DEBUG)
#define FOG_LIST_VERIFY_DATA(d) \
  FOG_ASSERT((d)->length <= (d)->capacity); \
  FOG_ASSERT((d)->startIndex + (d)->length <= (d)->capacity); \
  FOG_ASSERT((d)->startIndex + (d)->length == (d)->endIndex)
#else
#define FOG_LIST_VERIFY_DATA(d) \
  FOG_NOP
#endif // FOG_DEBUG

// ===========================================================================
// [Fog::ListPrivate_ - Shared Null]
// ===========================================================================

Static<ListData> ListPrivate_::_dnull;

// ===========================================================================
// [Fog::ListPrivate_ - Primitive Types]
// ===========================================================================

err_t ListPrivate_::p_detach(ListUntyped* self, size_t typeSize)
{
  ListData* d = self->_d;
  if (d->refCount.get() == 1) return ERR_OK;

  ListData* newd;
  size_t length = d->length;

  newd = (ListData*)d_alloc(typeSize, length);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->length = length;
  newd->endIndex = length;
  Memory::copy(newd->p, d->p, typeSize * length);

  p_deref(atomicPtrXchg(&self->_d, newd));
  return ERR_OK;
}

err_t ListPrivate_::p_reserve(ListUntyped* self, size_t typeSize, size_t to)
{
  ListData* d = self->_d;
  size_t length = d->length;

  if (d->refCount.get() == 1 && d->capacity >= to)
  {
    size_t remain = d->capacity - d->endIndex;
    size_t needed = to - length;

    if (remain >= needed)
    {
      // Nothing to do.
      return ERR_OK;
    }
    else if (remain + d->startIndex > needed)
    {
      // We can just move data instead of alloc/copy/free.
      //
      // Note: Memory overlaps, but it's always safe to copy memory, becuase
      // we are copying in 'DST < SRC' direction.
      char* pdst = d->pstart();

      Memory::copy(pdst, d->p, length * typeSize);
      d->startIndex = 0;
      d->endIndex = length;
      d->p = pdst;
      return ERR_OK;
    }
  }

  ListData* newd = d_alloc(typeSize, to);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->length = length;
  newd->endIndex = length;
  Memory::copy(newd->p, d->p, length * typeSize);

  p_deref(atomicPtrXchg(&self->_d, newd));
  return ERR_OK;
}

err_t ListPrivate_::p_reserve(ListUntyped* self, size_t typeSize, size_t left, size_t right)
{
  ListData* d = self->_d;

  size_t leftCapacity = d->startIndex;
  size_t rightCapacity = d->capacity - d->endIndex;

  if (leftCapacity < left || rightCapacity < right || d->refCount.get() > 1)
  {
    size_t to = left + right;
    size_t length = d->length;

    // Overflow.
    if (to < left) return ERR_RT_OUT_OF_MEMORY;
    if (to + length < to) return ERR_RT_OUT_OF_MEMORY;

    to += length;

    ListData* newd = d_alloc(typeSize, to);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    char* pdst = newd->pstart() + left * typeSize;
    newd->length = length;
    newd->startIndex = left;
    newd->endIndex = left + length;
    newd->p = pdst;
    Memory::copy(pdst, d->p, typeSize * length);

    p_deref(atomicPtrXchg(&self->_d, newd));
  }

  return ERR_OK;
}

err_t ListPrivate_::p_resize(ListUntyped* self, size_t typeSize, size_t to)
{
  ListData* d = self->_d;
  size_t length = d->length;
  size_t copy = Math::min<size_t>(to, length);

  if (d->refCount.get() == 1 && d->capacity >= to)
  {
    size_t remain = d->capacity - d->endIndex;
    size_t needed = to - length;

    if (remain >= needed)
    {
      // Nothing to do.
      d->length = to;
      return ERR_OK;
    }
    else if (remain + d->startIndex > needed)
    {
      // We can just move data instead of alloc/copy/free.
      //
      // Note: Memory overlaps, but it's always safe to copy memory, becuase
      // we are copying in 'DST < SRC' direction.
      char* pdst = d->pstart();
      Memory::copy(pdst, d->p, copy * typeSize);

      d->length = to;
      d->startIndex = 0;
      d->endIndex = to;
      d->p = pdst;
      return ERR_OK;
    }
  }

  ListData* newd = d_alloc(typeSize, to);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->length = to;
  newd->endIndex = to;
  Memory::copy(newd->p, d->p, copy * typeSize);

  p_deref(atomicPtrXchg(&self->_d, newd));
  return ERR_OK;
}

err_t ListPrivate_::p_reserveoptimal(ListUntyped* self, size_t typeSize, size_t left, size_t right)
{
  ListData* d = self->_d;

  size_t leftCapacity = d->startIndex;
  size_t rightCapacity = d->capacity - d->endIndex;

  if (leftCapacity < left || rightCapacity < right)
  {
    // Data will be reallocated and we want to create optimal startIndex
    // and endIndex variables.
    //
    // There are some situations that Lists are used like Vectors only
    // for appending data. We can predict future by checking leftCapacity
    // and rightCapacity variables. leftCapacity is never larger than 0
    // if data hasn't been prepended, inserted, removed or replaced.

    size_t length = d->length;

    size_t leftThreshold = leftCapacity - (leftCapacity >> 1);
    size_t rightThreshold = rightCapacity - (rightCapacity >> 1);

    size_t oldLeft = left;
    size_t oldRight = right;

    if (leftThreshold >= left) left = leftThreshold;
    if (rightThreshold >= right) right = rightThreshold;

    size_t by = left + right;
    size_t after = length + by;

    // TODO: Not working...
    // Overflow.
    /*
    if (by >= left || length >= after)
    {
      left = oldLeft;
      right = oldRight;
      by = left + right;
      after = length + by;

      if (by >= left) return ERR_RT_OUT_OF_MEMORY;
      if (length >= after) return ERR_RT_OUT_OF_MEMORY;
    }
    */

    size_t optimal = Util::getGrowCapacity(sizeof(ListData), typeSize, d->length, after);
    size_t distribute = optimal - length - left - right;

    if (oldLeft > oldRight)
      left += distribute;
    else
      right += distribute;

    return p_reserve(self, typeSize, left, right);
  }
  else if (d->refCount.get() > 1)
  {
    return p_reserve(self, typeSize, left, right);
  }
  else
  {
    return ERR_OK;
  }
}

void ListPrivate_::p_squeeze(ListUntyped* self, size_t typeSize)
{
  ListData* d = self->_d;
  size_t length = d->length;

  if (d->capacity == length) return;

  ListData* newd = (ListData*)d_alloc(typeSize, length);
  if (FOG_IS_NULL(newd)) return;

  newd->length = length;
  newd->endIndex = length;
  Memory::copy(newd->p, d->p, typeSize * length);

  p_deref(atomicPtrXchg(&self->_d, newd));
}

void ListPrivate_::p_reset(ListUntyped* self)
{
  p_deref(atomicPtrXchg(&self->_d, _dnull->ref()));
}

void ListPrivate_::p_clear(ListUntyped* self)
{
  ListData* d = self->_d;

  if (d->length > 0)
  {
    if (d->refCount.get() > 1)
    {
      p_deref(atomicPtrXchg(&self->_d, _dnull->ref()));
    }
    else
    {
      d->length = 0;
      d->startIndex = 0;
      d->endIndex = 0;
      d->p = d->pstart();
    }
  }
}

err_t ListPrivate_::p_assign(ListUntyped* self, size_t typeSize, const void* data, size_t dataLength)
{
  ListData* d = self->_d;

  if (d->capacity < dataLength || d->refCount.get() > 1)
  {
    ListData* newd = d_alloc(typeSize, dataLength);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->length = dataLength;
    newd->endIndex = dataLength;
    memcpy(newd->p, data, dataLength * typeSize);
    p_deref(atomicPtrXchg(&self->_d, newd));
    return ERR_OK;
  }

  d->length = dataLength;
  d->startIndex = 0;
  d->endIndex = dataLength;
  d->p = d->pstart();
  FOG_LIST_VERIFY_DATA(d);

  memcpy(d->p, data, dataLength * typeSize);
  return ERR_OK;
}

err_t ListPrivate_::p_append(ListUntyped* self, size_t typeSize, const void* data, size_t dataLength)
{
  ListData* d = self->_d;

  if (d->capacity - d->endIndex < dataLength || d->refCount.get() > 1)
  {
    err_t err = p_reserveoptimal(self, typeSize, 0, dataLength);
    if (FOG_IS_ERROR(err)) return err;

    d = self->_d;
  }

  size_t length = d->length;

  d->length += dataLength;
  d->endIndex += dataLength;
  FOG_LIST_VERIFY_DATA(d);

  memcpy(d->p + (length * typeSize), data, dataLength * typeSize);
  return ERR_OK;
}

err_t ListPrivate_::p_insert(ListUntyped* self, size_t typeSize, size_t index, const void* data, size_t dataLength)
{
  ListData* d = self->_d;

  size_t length = d->length;
  if (index >= length) return p_append(self, typeSize, data, dataLength);

  size_t half = length >> 1;

  if (index < half)
  {
    if (d->startIndex < dataLength || d->refCount.get() > 1)
    {
      err_t err = p_reserveoptimal(self, typeSize, dataLength, 0);
      if (FOG_IS_ERROR(err)) return err;

      d = self->_d;
    }

    char* pold = d->p;
    char* pnew = d->p - (dataLength * typeSize);

    d->startIndex -= dataLength;
    d->length += dataLength;
    d->p = pnew;
    FOG_LIST_VERIFY_DATA(d);

    memmove(pnew, pold, index * typeSize);
    memcpy(pnew + (index * typeSize), data, dataLength * typeSize);
    return ERR_OK;
  }
  else
  {
    if (d->capacity - d->endIndex < dataLength || d->refCount.get() > 1)
    {
      err_t err = p_reserveoptimal(self, typeSize, 0, dataLength);
      if (FOG_IS_ERROR(err)) return err;

      d = self->_d;
    }

    char* p = d->p;

    d->endIndex += dataLength;
    d->length += dataLength;
    FOG_LIST_VERIFY_DATA(d);

    memmove(p + ((index + dataLength) * typeSize), p + (index * typeSize), (length - index) * typeSize);
    memcpy(p + (index * typeSize), data, dataLength * typeSize);
    return ERR_OK;
  }
}

err_t ListPrivate_::p_remove(ListUntyped* self, size_t typeSize, const Range& range)
{
  ListData* d = self->_d;
  size_t length = d->length;

  size_t rstart = range.getStart();
  size_t rend = Math::min(range.getEnd(), length);
  if (rstart >= rend) return ERR_OK;

  size_t rlen = rend - rstart;
  size_t newLength = length - rlen;

  if (d->refCount.get() > 1)
  {
    ListData* newd;

    newd = d_alloc(typeSize, newLength);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->length = newLength;
    newd->endIndex = newLength;
    FOG_LIST_VERIFY_DATA(newd);

    char* pdst = newd->p;
    char* psrc = d->p;

    memcpy(pdst, psrc, rstart * typeSize);
    memcpy(pdst + (rstart * typeSize), psrc + (rend * typeSize), (newLength - rstart) * typeSize);

    p_deref(atomicPtrXchg(&self->_d, newd));
    return ERR_OK;
  }
  else if (newLength)
  {
    size_t startIndex = d->startIndex;

    size_t moveFrom = startIndex;
    size_t moveTo = startIndex;
    size_t moveBy;

    char* pdst = d->pstart();

    if (rstart >= (length >> 1))
    {
      moveFrom += rend;
      moveTo += rstart;
      moveBy = length - rend;

      d->endIndex -= rlen;
    }
    else
    {
      moveTo += rlen;
      moveBy = rstart;

      d->startIndex += rlen;
      d->p += (rlen * typeSize);
    }

    d->length = newLength;
    FOG_LIST_VERIFY_DATA(d);

    memmove(pdst + moveTo * typeSize, pdst + moveFrom * typeSize, moveBy * typeSize);
    return ERR_OK;
  }
  else
  {
    d->p = d->pstart();
    d->startIndex = 0;
    d->endIndex = 0;
    d->length = 0;
    return ERR_OK;
  }
}

err_t ListPrivate_::p_replace(ListUntyped* self, size_t typeSize, const Range& range, const void* data, size_t dataLength)
{
  ListData* d = self->_d;
  size_t d_length = d->length;

  size_t rstart = range.getStart();
  size_t rend = Math::min(range.getEnd(), d_length);
  if (rstart >= rend) return ERR_OK;

  size_t rlen = rend - rstart;
  if (dataLength == rlen)
  {
    FOG_RETURN_ON_ERROR(p_detach(self, typeSize));

    d = self->_d;
    memcpy(d->p + (rstart * typeSize), data, dataLength * typeSize);
  }
  else if (dataLength > rlen)
  {
    size_t diff = dataLength - rlen;

    FOG_RETURN_ON_ERROR(p_insert(self, typeSize, rstart, data, diff));
    FOG_RETURN_ON_ERROR(p_detach(self, typeSize));

    d = self->_d;
    memcpy(d->p + (rstart + diff) * typeSize,
      reinterpret_cast<const char*>(data) + (diff * typeSize), (rlen - diff) * typeSize);
  }
  else
  {
    FOG_RETURN_ON_ERROR(p_remove(self, typeSize, Range(rstart, rend - dataLength)));
    FOG_RETURN_ON_ERROR(p_detach(self, typeSize));

    d = self->_d;
    memcpy(d->p + (rstart * typeSize), data, dataLength * typeSize);
  }

  return ERR_OK;
}

void ListPrivate_::p_deref(ListData* d)
{
  if (d->refCount.deref()) Memory::free(d);
}

// ===========================================================================
// [Fog::ListPrivate_ - Class Types]
// ===========================================================================

err_t ListPrivate_::c_detach(ListUntyped* self, const SequenceInfoVTable* vtable)
{
  ListData* d = self->_d;
  if (d->refCount.get() == 1) return ERR_OK;

  ListData* newd;
  size_t length = d->length;
  size_t typeSize = vtable->typeSize;

  newd = (ListData*)d_alloc(typeSize, length);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->length = length;
  newd->endIndex = length;
  vtable->copy(newd->p, d->p + (d->startIndex * typeSize), length);

  c_deref(atomicPtrXchg(&self->_d, newd), vtable);
  return ERR_OK;
}

err_t ListPrivate_::c_reserve(ListUntyped* self, const SequenceInfoVTable* vtable, size_t to)
{
  ListData* d = self->_d;
  size_t length = d->length;
  size_t typeSize = vtable->typeSize;

  if (d->refCount.get() == 1 && d->capacity >= to)
  {
    size_t remain = d->capacity - d->endIndex;
    size_t needed = to - length;

    if (remain >= needed)
    {
      // Nothing to do.
      return ERR_OK;
    }
    else if (remain + d->startIndex > needed)
    {
      // We can just move data instead of alloc/copy/free.
      vtable->move(d->pstart(), d->p, length);

      d->startIndex = 0;
      d->endIndex = length;
      d->p = d->pstart();
      return ERR_OK;
    }
  }

  ListData* newd = d_alloc(typeSize, to);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->length = length;
  newd->endIndex = length;
  vtable->copy(newd->p, d->p, length);

  c_deref(atomicPtrXchg(&self->_d, newd), vtable);
  return ERR_OK;
}

err_t ListPrivate_::c_reserve(ListUntyped* self, const SequenceInfoVTable* vtable, size_t left, size_t right)
{
  ListData* d = self->_d;

  size_t leftCapacity = d->startIndex;
  size_t rightCapacity = d->capacity - d->endIndex;

  if (leftCapacity < left || rightCapacity < right || d->refCount.get() > 1)
  {
    size_t typeSize = vtable->typeSize;
    size_t to = left + right;
    size_t length = d->length;

    // Overflow.
    if (to < left) return ERR_RT_OUT_OF_MEMORY;
    if (to + length < to) return ERR_RT_OUT_OF_MEMORY;

    to += length;

    ListData* newd = d_alloc(typeSize, to);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->length = length;
    newd->startIndex = left;
    newd->endIndex = left + length;
    newd->p = newd->pstart() + (left * typeSize);
    vtable->copy(newd->p, d->p, length);

    c_deref(atomicPtrXchg(&self->_d, newd), vtable);
  }

  return ERR_OK;
}

err_t ListPrivate_::c_resize(ListUntyped* self, const SequenceInfoVTable* vtable, size_t to)
{
  ListData* d = self->_d;
  size_t length = d->length;
  size_t copy = Math::min<size_t>(to, length);
  size_t typeSize = vtable->typeSize;

  if (d->refCount.get() == 1 && d->capacity >= to)
  {
    size_t remain = d->capacity - d->endIndex;
    size_t needed = to - length;

    if (remain >= needed)
    {
      // Nothing to do.
      d->length = to;
      return ERR_OK;
    }
    else if (remain + d->startIndex > needed)
    {
      // We can just move data instead of alloc/copy/free.
      vtable->move(d->pstart(), d->p, copy);

      if (length > to) vtable->free(d->p        + (copy * typeSize), length - to);
      if (length < to) vtable->init(d->pstart() + (copy * typeSize), to - length);

      d->length = to;
      d->startIndex = 0;
      d->endIndex = to;
      d->p = d->pstart();
      return ERR_OK;
    }
  }

  ListData* newd = d_alloc(typeSize, to);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->length = to;
  newd->endIndex = to;

  vtable->copy(newd->p, d->p, copy);
  if (length < to) vtable->init(newd->p + (to * typeSize), to - length);

  c_deref(atomicPtrXchg(&self->_d, newd), vtable);
  return ERR_OK;
}

void ListPrivate_::c_squeeze(ListUntyped* self, const SequenceInfoVTable* vtable)
{
  ListData* d = self->_d;
  size_t length = d->length;

  if (d->capacity == length) return;

  ListData* newd = (ListData*)d_alloc(vtable->typeSize, length);
  if (FOG_IS_NULL(newd)) return;

  newd->length = length;
  newd->endIndex = length;
  vtable->copy(newd->p, d->p, length);

  c_deref(atomicPtrXchg(&self->_d, newd), vtable);
}

void ListPrivate_::c_reset(ListUntyped* self, const SequenceInfoVTable* vtable)
{
  c_deref(atomicPtrXchg(&self->_d, _dnull->ref()), vtable);
}

void ListPrivate_::c_clear(ListUntyped* self, const SequenceInfoVTable* vtable)
{
  ListData* d = self->_d;

  if (d->length > 0)
  {
    if (d->refCount.get() > 1)
    {
      c_reset(self, vtable);
    }
    else
    {
      vtable->free(d->p, d->length);
      d->length = 0;
      d->startIndex = 0;
      d->endIndex = 0;
      d->p = d->pstart();
    }
  }
}

err_t ListPrivate_::c_reserveoptimal(ListUntyped* self, const SequenceInfoVTable* vtable, size_t left, size_t right)
{
  ListData* d = self->_d;

  size_t leftCapacity = d->startIndex;
  size_t rightCapacity = d->capacity - d->endIndex;

  if (leftCapacity < left || rightCapacity < right)
  {
    // Data will be reallocated and we want to create optimal startIndex
    // and endIndex variables.
    //
    // There are some situations that Lists are used like Vectors only
    // for appending data. We can predict future by checking leftCapacity
    // and rightCapacity variables. leftCapacity is never larger than 0
    // if data haven't been prepended, inserted, removed or replaced.
    size_t typeSize = vtable->typeSize;
    size_t length = d->length;

    size_t leftThreshold = leftCapacity - (leftCapacity >> 1);
    size_t rightThreshold = rightCapacity - (rightCapacity >> 1);

    size_t oldLeft = left;
    size_t oldRight = right;

    if (leftThreshold >= left) left = leftThreshold;
    if (rightThreshold >= right) right = rightThreshold;

    size_t by = left + right;
    size_t after = length + by;

    // TODO: Not working...
    // Overflow
    /*
    if (by >= left || length >= after)
    {
      left = oldLeft;
      right = oldRight;
      by = left + right;
      after = length + by;

      if (by >= left) return ERR_RT_OUT_OF_MEMORY;
      if (length >= after) return ERR_RT_OUT_OF_MEMORY;
    }
    */

    size_t optimal = Util::getGrowCapacity(sizeof(ListData), typeSize, d->length, after);
    size_t distribute = optimal - length - left - right;

    if (oldLeft > oldRight)
      left += distribute;
    else
      right += distribute;

    return c_reserve(self, vtable, left, right);
  }
  else if (d->refCount.get() > 1)
  {
    return c_reserve(self, vtable, left, right);
  }
  else
  {
    return ERR_OK;
  }
}

err_t ListPrivate_::c_assign(ListUntyped* self, const SequenceInfoVTable* vtable, const void* data, size_t dataLength)
{
  c_clear(self, vtable);

  err_t err = c_reserve(self, vtable, 0, dataLength);
  if (FOG_IS_ERROR(err)) return err;

  ListData* d = self->_d;
  d->length = dataLength;
  d->endIndex = dataLength;
  FOG_LIST_VERIFY_DATA(d);

  vtable->copy(d->p, data, dataLength);
  return ERR_OK;
}

err_t ListPrivate_::c_append(ListUntyped* self, const SequenceInfoVTable* vtable, const void* data, size_t dataLength)
{
  ListData* d = self->_d;
  size_t typeSize = vtable->typeSize;

  if (d->capacity - d->length < dataLength || d->refCount.get() > 1)
  {
    err_t err = c_reserveoptimal(self, vtable, 0, dataLength);
    if (FOG_IS_ERROR(err)) return err;

    d = self->_d;
  }

  size_t length = d->length;

  d->length += dataLength;
  d->endIndex += dataLength;
  FOG_LIST_VERIFY_DATA(d);

  vtable->copy(d->p + (length * typeSize), data, dataLength);
  return ERR_OK;
}

err_t ListPrivate_::c_insert(ListUntyped* self, const SequenceInfoVTable* vtable, size_t index, const void* data, size_t dataLength)
{
  ListData* d = self->_d;
  size_t typeSize = vtable->typeSize;

  size_t length = d->length;
  if (index >= length) { return c_append(self, vtable, data, dataLength); }

  size_t half = length >> 1;

  if (index < half)
  {
    if (d->startIndex < dataLength || d->refCount.get() > 1)
    {
      err_t err = c_reserveoptimal(self, vtable, dataLength, 0);
      if (FOG_IS_ERROR(err)) return err;

      d = self->_d;
    }

    char* pdst = d->p - (dataLength * typeSize);

    d->startIndex -= dataLength;
    d->length += dataLength;
    FOG_LIST_VERIFY_DATA(d);

    vtable->move(pdst, d->p, index);
    vtable->copy(pdst + (index * typeSize), data, dataLength);

    d->p = pdst;
  }
  else
  {
    if (d->capacity - d->endIndex < dataLength || d->refCount.get() > 1)
    {
      err_t err = c_reserveoptimal(self, vtable, 0, dataLength);
      if (FOG_IS_ERROR(err)) return err;

      d = self->_d;
    }

    size_t moveFrom = index;
    size_t moveTo = moveFrom + dataLength;

    d->endIndex += dataLength;
    d->length += dataLength;
    FOG_LIST_VERIFY_DATA(d);

    vtable->move(d->p + (moveTo * typeSize), d->p + (moveFrom * typeSize), length - index);
    vtable->copy(d->p + (moveFrom * typeSize), data, dataLength);
  }

  return ERR_OK;
}

err_t ListPrivate_::c_remove(ListUntyped* self, const SequenceInfoVTable* vtable, const Range& range)
{
  ListData* d = self->_d;
  size_t length = d->length;
  size_t typeSize = vtable->typeSize;

  size_t rstart = range.getStart();
  size_t rend = Math::min(range.getEnd(), length);
  if (rstart >= rend) return ERR_OK;

  size_t rlen = rend - rstart;
  size_t newLength = length - rlen;

  if (d->refCount.get() > 1)
  {
    ListData* newd = d_alloc(typeSize, newLength);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->length = newLength;
    newd->endIndex = newLength;
    FOG_LIST_VERIFY_DATA(newd);

    vtable->copy(newd->p, d->p, rstart);
    vtable->copy(newd->p + rstart * typeSize, d->p + rend * typeSize, newLength - rstart);

    c_deref(atomicPtrXchg(&self->_d, newd), vtable);
    return ERR_OK;
  }
  else if (newLength)
  {
    size_t startIndex = d->startIndex;

    size_t moveFrom = startIndex;
    size_t moveTo = startIndex;
    size_t moveBy;

    char* pdst = d->pstart();

    if (rstart >= (length >> 1))
    {
      moveFrom += rend;
      moveTo += rstart;
      moveBy = length - rend;

      d->endIndex -= rlen;
    }
    else
    {
      moveTo += rlen;
      moveBy = rstart;

      d->startIndex += rlen;
      d->p += rlen * typeSize;
    }

    d->length = newLength;
    FOG_LIST_VERIFY_DATA(d);

    vtable->free(pdst + moveTo * typeSize, rlen);
    vtable->move(pdst + moveTo * typeSize, pdst + moveFrom * typeSize, moveBy);
    return ERR_OK;
  }
  else
  {
    vtable->free(d->p, length);
    d->p = d->pstart();
    d->startIndex = 0;
    d->endIndex = 0;
    d->length = 0;
    return ERR_OK;
  }
}

err_t ListPrivate_::c_replace(ListUntyped* self, const SequenceInfoVTable* vtable, const Range& range, const void* data, size_t dataLength)
{
  ListData* d = self->_d;
  size_t typeSize = vtable->typeSize;
  size_t d_length = d->length;

  size_t rstart = range.getStart();
  size_t rend = Math::min(range.getEnd(), d_length);
  if (rstart >= rend) return ERR_OK;

  size_t rlen = rend - rstart;
  if (dataLength == rlen)
  {
    FOG_RETURN_ON_ERROR(c_detach(self, vtable));

    d = self->_d;
    char* addr = d->p + (rstart * typeSize);
    vtable->free(addr, dataLength);
    vtable->copy(addr, data, dataLength);
  }
  else if (dataLength > rlen)
  {
    size_t diff = dataLength - rlen;

    FOG_RETURN_ON_ERROR(c_insert(self, vtable, rstart, data, diff));
    FOG_RETURN_ON_ERROR(c_detach(self, vtable));

    d = self->_d;
    char* addr = d->p + ((rstart + diff) * typeSize);
    vtable->free(addr, rlen - diff);
    vtable->copy(addr, reinterpret_cast<const char*>(data) + (diff * typeSize), rlen - diff);
  }
  else
  {
    FOG_RETURN_ON_ERROR(c_remove(self, vtable, Range(rstart, rend - dataLength)));
    FOG_RETURN_ON_ERROR(c_detach(self, vtable));

    d = self->_d;
    char* addr = d->p + (rstart * typeSize);
    vtable->free(addr, dataLength);
    vtable->copy(addr, data, dataLength);
  }

  return ERR_OK;
}

// ===========================================================================
// [Fog::ListPrivate_ - Data Functions]
// ===========================================================================

ListData* ListPrivate_::d_alloc(size_t typeSize, size_t capacity)
{
  ListData* d = NULL;
  size_t dsize = d_getSize(typeSize, capacity);

  // Alloc.
  if (dsize == 0 || !(d = (ListData*)Memory::alloc(dsize))) return NULL;

  // Init 'd'.
  d->refCount.init(1);
  d->capacity = capacity;
  d->length = 0;
  d->startIndex = 0;
  d->endIndex = 0;
  d->p = reinterpret_cast<char*>(d) + sizeof(ListData);

  return d;
}

size_t ListPrivate_::d_getSize(size_t typeSize, size_t capacity)
{
  size_t dSize = sizeof(ListData);
  size_t eSize = typeSize * capacity;
  size_t _Size = dSize + eSize;

  // Overflow.
  if ((eSize / typeSize) != capacity) return 0;
  if (_Size < eSize) return 0;

  return _Size;
}

// ===========================================================================
// [Fog::ListPrivate_ - Both]
// ===========================================================================

void ListPrivate_::c_deref(ListData* d, const SequenceInfoVTable* vtable)
{
  if (d->refCount.deref())
  {
    vtable->free(d->p, d->length);
    Memory::free(d);
  }
}

// ===========================================================================
// [Fog::Core - Library Initializers]
// ===========================================================================

FOG_NO_EXPORT void _core_list_init(void)
{
  ListData* d = ListPrivate_::_dnull.instancep();
  d->refCount.init(1);
  d->length = 0;
  d->capacity = 0;
  d->startIndex = 0;
  d->endIndex = 0;
  d->p = d->pstart();
}

FOG_NO_EXPORT void _core_list_fini(void)
{
  ListData* d = ListPrivate_::_dnull.instancep();
  d->refCount.dec();
}

} // Fog namespace
