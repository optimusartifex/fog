// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Tools/Region.h>
#include <Fog/G2d/Tools/RegionTmp_p.h>
#include <Fog/G2d/Tools/RegionUtil_p.h>

namespace Fog {

// ============================================================================
// [Fog::Region - Global]
// ============================================================================

static Static<RegionData> Region_dEmpty;
static Static<RegionData> Region_dInfinite;

static Static<Region> Region_oEmpty;
static Static<Region> Region_oInfinite;

// ============================================================================
// [Fog::Region - Helpers]
// ============================================================================

#define REGION_STACK_SIZE 64

static FOG_INLINE void Region_copyData(BoxI* dst, const BoxI* src, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    dst[i] = src[i];
  }
}

static void Region_copyDataEx(BoxI* dst, const BoxI* src, size_t length, BoxI* boundingBox)
{
  int minX = src[0].x0;
  int maxX = src[length-1].x1;

  for (size_t i = 0; i < length; i++)
  {
    if (minX > src[i].x0) minX = src[i].x0;
    if (maxX < src[i].x1) maxX = src[i].x1;
    dst[i] = src[i];
  }

  boundingBox->setBox(minX, src[0].y0, maxX, src[length-1].y1);
}

static void Region_copyDataEx(BoxI* dst, const RectI* src, size_t length, BoxI* boundingBox)
{
  int minX = src[0       ].getX0();
  int maxX = src[length-1].getX1();

  for (size_t i = 0; i < length; i++)
  {
    int x0 = src[i].getX0();
    int x1 = src[i].getX1();
    if (minX > x0) minX = x0;
    if (maxX < x1) maxX = x1;
    dst[i].setBox(x0, src[i].getY0(), x1, src[i].getY1());
  }

  boundingBox->setBox(minX, src[0].getY0(), maxX, src[length-1].getY1());
}

// ============================================================================
// [Fog::Region - IsValid]
// ============================================================================

// Validate RegionData instance:
//
//   - If region is empty, its bounding box must be [0, 0, 0, 0]
//   - If region is rectangle, then the data and bounding box must match and
//     can't be invalid.
//   - If region is complex, validate all bands and check whether all bands
//     were coalesced properly.
static bool Region_isValid(const RegionData* d)
{
  FOG_ASSERT(d->capacity >= d->length);
  size_t length = d->length;

  const BoxI* data = d->data;
  const BoxI& bbox = d->boundingBox;

  if (length == 0)
  {
    if (bbox.x0 != 0 || bbox.y0 != 0 || bbox.x1 != 0 || bbox.y1 != 0)
      return false;
    else
      return true;
  }

  if (length == 1)
  {
    if (bbox != data[0] || !bbox.isValid())
      return false;
    else
      return true;
  }

  const BoxI* prevBand = NULL;
  const BoxI* prevEnd = NULL;

  const BoxI* curBand = data;
  const BoxI* curEnd;

  const BoxI* end = data + length;

  // Validated bounding box.
  BoxI vbox = data[0];
  vbox.y1 = data[length-1].y1;

  do {
    curEnd = RegionUtil::getEndBand(curBand, end);

    // Validate current band.
    {
      const BoxI* cur = curBand;
      if (cur[0].x0 >= cur[0].x1)
        return false;

      while (++cur != curEnd)
      {
        // Validate Y.
        if (cur[-1].y0 != cur[0].y0)
          return false;
        if (cur[-1].y1 != cur[0].y1)
          return false;

        // Validate X.
        if (cur[0].x0 >= cur[0].x1)
          return false;
        if (cur[0].x0 < cur[-1].x1)
          return false;
      }
    }

    if (prevBand != NULL && curBand->y0 < prevBand->y1)
      return false;

    // Validate whether the current band and the previous one are properly coalesced.
    if (prevBand != NULL && curBand->y0 == prevBand->y1 && (size_t)(prevEnd - prevBand) == (size_t)(curEnd - curBand))
    {
      size_t i = 0;
      size_t bandLength = (size_t)(prevEnd - prevBand);

      do {
        if (prevBand[i].x0 != curBand[i].x0 || prevBand[i].x1 != curBand[i].x1)
          break;
      } while (++i < bandLength);

      // Improperly coalesced.
      if (i == bandLength)
        return false;
    }

    vbox.x0 = Math::min<int>(vbox.x0, curBand[0].x0);
    vbox.x1 = Math::max<int>(vbox.x1, curEnd[-1].x1);

    prevBand = curBand;
    prevEnd = curEnd;

    curBand = curEnd;
  } while (curBand != end);

  // Validate bounding box.
  if (bbox != vbox)
    return false;

  // Region is valid.
  return true;
}

// ============================================================================
// [Fog::Region - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Region_ctor(Region* self)
{
  self->_d = Region_dEmpty->addRef();
}

static void FOG_CDECL Region_ctorRegion(Region* self, const Region* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL Region_ctorBox(Region* self, const BoxI* box)
{
  RegionData* d;

  if (FOG_UNLIKELY(!box->isValid()))
    goto _Fail;

  if (FOG_IS_NULL(d = _api.region_dCreateBox(1, box)))
    goto _Fail;

  self->_d = d;
  return;

_Fail:
  self->_d = Region_dEmpty->addRef();
}

static void FOG_CDECL Region_ctorRect(Region* self, const RectI* rect)
{
  RegionData* d;
  BoxI box(UNINITIALIZED);

  if (FOG_UNLIKELY(!rect->isValid()))
    goto _Fail;

  box = *rect;
  if (FOG_IS_NULL(d = _api.region_dCreateBox(1, &box)))
    goto _Fail;

  self->_d = d;
  return;

_Fail:
  self->_d = Region_dEmpty->addRef();
}

static void FOG_CDECL Region_dtor(Region* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::Region - Sharing]
// ============================================================================

static err_t FOG_CDECL Region_detach(Region* self)
{
  RegionData* d = self->_d;

  if (d->reference.get() == 1)
    return ERR_OK;

  if (d->length > 0)
  {
    d = _api.region_dCreateRegion(d->length, d->data, d->length, &d->boundingBox);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }
  else
  {
    d = _api.region_dCreate(d->capacity);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->boundingBox.reset();
  }

  atomicPtrXchg(&self->_d, d)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Region - Container]
// ============================================================================

static err_t FOG_CDECL Region_reserve(Region* self, size_t capacity)
{
  RegionData* d = self->_d;

  if (d->reference.get() > 1)
  {
_Create:
    RegionData* newd = _api.region_dCreateRegion(capacity, d->data, d->length, &d->boundingBox);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&self->_d, newd)->release();
  }
  else if (d->capacity < capacity)
  {
    RegionData* newd = _api.region_dRealloc(d, capacity);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;
    self->_d = newd;
  }

  return ERR_OK;
}

static void FOG_CDECL Region_squeeze(Region* self)
{
  RegionData* d = self->_d;

  if ((d->vType & VAR_FLAG_STATIC) != 0)
    return;

  size_t length = d->length;
  if (d->capacity == length)
    return;

  if (d->reference.get() > 1)
  {
    RegionData* newd = _api.region_dCreate(length);
    if (FOG_IS_NULL(newd)) return;

    newd->length = length;
    newd->boundingBox = d->boundingBox;
    Region_copyData(newd->data, d->data, length);

    atomicPtrXchg(&self->_d, newd)->release();
  }
  else
  {
    RegionData* newd = _api.region_dRealloc(d, length);
    if (FOG_IS_NULL(newd)) return;
    self->_d = newd;
  }
}

static err_t FOG_CDECL Region_prepare(Region* self, size_t count)
{
  RegionData* d = self->_d;

  if (d->reference.get() > 1 || d->capacity < count)
  {
_Create:
    RegionData* newd = _api.region_dCreate(count);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&self->_d, newd)->release();
  }
  else
  {
    d->length = 0;
    d->boundingBox.reset();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Region - GetType]
// ============================================================================

static uint32_t FOG_CDECL Region_getType(const Region* self)
{
  RegionData* d = self->_d;
  size_t length = d->length;

  // Empty region length is always zero.
  if (length == 0)
    return REGION_TYPE_EMPTY;

  // Complex region has more than 1 rectangle.
  if (length > 1)
    return REGION_TYPE_COMPLEX;

  // Now we know that region has exactly 1 rectangle, check for infinity.
  if (d == &Region_dInfinite)
    return REGION_TYPE_INFINITE;
  else
    return REGION_TYPE_RECT;
}

// ============================================================================
// [Fog::Region - Clear]
// ============================================================================

static void FOG_CDECL Region_clear(Region* self)
{
  RegionData* d = self->_d;

  if (d == &Region_dInfinite)
    goto _Reset;

  if (d->length == 0)
    return;

  if (d->reference.get() == 1)
  {
    d->length = 0;
    d->boundingBox.reset();
    return;
  }

_Reset:
  atomicPtrXchg(&self->_d, Region_dEmpty->addRef())->release();
}

// ============================================================================
// [Fog::Region - Reset]
// ============================================================================

static void FOG_CDECL Region_reset(Region* self)
{
  atomicPtrXchg(&self->_d, Region_dEmpty->addRef())->release();
}

// ============================================================================
// [Fog::Region - Set]
// ============================================================================

static err_t FOG_CDECL Region_setRegion(Region* self, const Region* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

static err_t FOG_CDECL Region_setDeep(Region* self, const Region* other)
{
  RegionData* d = self->_d;
  RegionData* other_d = other->_d;

  if (d == other_d)
    return ERR_OK;

  if (d == &Region_dInfinite)
  {
    atomicPtrXchg(&self->_d, other_d->addRef())->release();
    return ERR_OK;
  }

  size_t length = other_d->length;
  FOG_RETURN_ON_ERROR(self->prepare(length));

  d = self->_d;
  d->length = other_d->length;
  d->boundingBox = other_d->boundingBox;
  Region_copyData(d->data, other_d->data, length);

  return ERR_OK;
}

static err_t FOG_CDECL Region_setBox(Region* self, const BoxI* box)
{
  if (!box->isValid())
    goto _Invalid;

  {
    FOG_RETURN_ON_ERROR(self->prepare(1));
    RegionData* d = self->_d;

    d->length = 1;
    d->boundingBox = *box;
    d->data[0] = *box;
  }
  return ERR_OK;

_Invalid:
  self->clear();
  return ERR_RT_INVALID_ARGUMENT;
}

static err_t FOG_CDECL Region_setRect(Region* self, const RectI* rect)
{
  if (!rect->isValid())
    goto _Invalid;

  {
    FOG_RETURN_ON_ERROR(self->prepare(1));
    RegionData* d = self->_d;

    d->length = 1;
    d->boundingBox = *rect;
    d->data[0] = d->boundingBox;
  }
  return ERR_OK;

_Invalid:
  self->clear();
  return ERR_RT_INVALID_ARGUMENT;
}

static err_t FOG_CDECL Region_setBoxList(Region* self, const BoxI* data, size_t length)
{
  if (RegionUtil::isBoxListSorted(data, length))
  {
    FOG_RETURN_ON_ERROR(self->prepare(length));

    RegionData* d = self->_d;
    d->length = length;
    Region_copyDataEx(d->data, data, length, &d->boundingBox);
    return ERR_OK;
  }

  // TODO: Not optimal.
  FOG_RETURN_ON_ERROR(self->prepare(length));

  for (size_t i = 0; i < length; i++)
    self->union_(data[i]);

  return ERR_OK;
}

static err_t FOG_CDECL Region_setRectList(Region* self, const RectI* data, size_t length)
{
  if (RegionUtil::isRectListSorted(data, length))
  {
    FOG_RETURN_ON_ERROR(self->prepare(length));

    RegionData* d = self->_d;
    d->length = length;
    Region_copyDataEx(d->data, data, length, &d->boundingBox);
    return ERR_OK;
  }

  // TODO: Not optimal.
  FOG_RETURN_ON_ERROR(self->prepare(length));

  for (size_t i = 0; i < length; i++)
    self->union_(data[i]);

  return ERR_OK;
}

// ============================================================================
// [Fog::Region - Coalesce]
// ============================================================================

static FOG_INLINE BoxI* Region_coalesce(BoxI* p, BoxI** _prevBand, BoxI** _curBand, int y1)
{
  BoxI* prevBand = *_prevBand;
  BoxI* curBand  = *_curBand;

  size_t i;
  size_t length;

  if (prevBand->y1 != curBand->y0)
    goto _Skip;

  i = (size_t)(curBand - prevBand);
  length = (size_t)(p - curBand);

  if (i != length)
    goto _Skip;

  for (i = 0; i < length; i++)
  {
    if (prevBand[i].x0 != curBand[i].x0 || prevBand[i].x1 != curBand[i].x1)
      goto _Skip;
  }

  for (i = 0; i < length; i++)
    prevBand[i].y1 = y1;
  return curBand;

_Skip:
  *_prevBand = curBand;
  return p;
}

// ============================================================================
// [Fog::Region - JoinTo]
// ============================================================================

// Get whether it is possible to join box B after box A.
static FOG_INLINE bool Region_canJoinBoxes(const BoxI& a, const BoxI& b)
{
  return a.y0 == b.y0 && a.y1 == b.y1 && a.x1 <= b.x0;
}

// Join src with the destination.
static err_t FOG_CDECL Region_joinTo(Region* dst,
  const BoxI* src, const BoxI* srcBoundingBox, size_t srcLength)
{
  size_t dstLength = dst->getLength();
  FOG_RETURN_ON_ERROR(dst->reserve(dstLength + srcLength));

  RegionData* d = dst->_d;

  BoxI* dstBegin = d->data;
  BoxI* dstCur = dstBegin + dstLength;
  BoxI* prevBand = NULL;

  const BoxI* srcCur = src;
  const BoxI* srcEnd = src + srcLength;

  // --------------------------------------------------------------------------
  // [Join the Continuous Part]
  // --------------------------------------------------------------------------

  // First try to join the first rectangle in SRC with the last rectangle in
  // DST. This is very special case used by UNION. The image below should
  // clarify the situation:
  //
  // 1) The first source rectangle immediately follows the last destination one.
  // 2) The first source rectangle follows the last destination band.
  //
  //       1)            2)
  //
  //   ..........   ..........
  //   ..DDDDDDDD   ..DDDDDDDD   D - Destination rectangle(s)
  //   DDDDDDSSSS   DDDD   SSS
  //   SSSSSSSS..   SSSSSSSS..   S - Source rectangle(s)
  //   ..........   ..........
  if (dstCur != dstBegin && dstCur[-1].y0 == srcCur->y0)
  {
    BoxI* dstMark = dstCur;
    int y0 = dstCur[-1].y0;
    int y1 = dstCur[-1].y1;

    // This special case require such condition.
    FOG_ASSERT(dstCur[-1].y1 == srcCur->y1);

    // Merge the last destination rectangle with the first source one? (Case 1).
    if (dstCur[-1].x1 == srcCur->x0)
    {
      dstCur[-1].x1 = srcCur->x1;
      srcCur++;
    }

    // Append all other bands (Case 1, 2).
    while (srcCur != srcEnd && srcCur->y0 == y0)
    {
      *dstCur++ = *srcCur++;
    }

    // Coalesce. If we know that boundingBox.y0 != y0 then it means that there
    // are at least two bands in the destination region. We added rectangles
    // to the last band, but this means that the last band can now be coalesced
    // with the previous one.
    //
    // Instead of finding two bands and then comparing them from left-to-right,
    // we find the end of bands and compare then right-to-left.
    if (d->boundingBox.y0 != y0)
    {
      BoxI* band1 = dstMark - 1;
      BoxI* band2 = dstCur - 1;

      while (band1->y0 == y0)
      {
        FOG_ASSERT(band1 != dstBegin);
        band1--;
      }

      prevBand = band1 + 1;

      // The 'band1' points to the end of the previous band.
      // The 'band2' points to the end of the current band.
      FOG_ASSERT(band1->y0 != band2->y0);

      if (band1->y1 == band2->y0)
      {
        while (band1->x0 == band2->x0 && band1->x1 == band2->x1)
        {
          bool finished1 = band1 == dstBegin || band1[-1].y0 != band1[0].y0;
          bool finished2 = band2[-1].y0 != band2[0].y0;

          if (finished1 & finished2)
            goto _DoFirstCoalesce;
          else if (finished1 | finished2)
            goto _SkipFirstCoalesce;

          band1--;
          band2--;
        }
      }
      goto _SkipFirstCoalesce;

_DoFirstCoalesce:
      prevBand = band1;
      dstCur = band2;

      do {
        band1->y1 = y1;
      } while (++band1 != band2);
    }
  }

_SkipFirstCoalesce:

  // --------------------------------------------------------------------------
  // [Join and Attempt to Coalesce the Second Band]
  // --------------------------------------------------------------------------

  if (srcCur == srcEnd)
    goto _End;

  if (prevBand != NULL && prevBand->y1 == srcCur->y0)
  {
    int y0 = srcCur->y0;
    int y1 = srcCur->y1;

    BoxI* band1 = prevBand;
    BoxI* band2 = dstCur;
    BoxI* mark = dstCur;

    do {
      *dstCur++ = *srcCur++;
    } while (srcCur != srcEnd && srcCur->y0 == y0);

    // Now check whether the bands have to be coalesced.
    while (band1 != mark && band1->x0 == band2->x0 && band1->x1 == band2->x1)
    {
      band1++;
      band2++;
    }

    if (band1 == mark)
    {
      for (band1 = prevBand; band1 != mark; band1++)
        band1->y1 = y1;
      dstCur = mark;
    }
  }

  // --------------------------------------------------------------------------
  // [Join the Rest]
  // --------------------------------------------------------------------------

  // We do not need coalese from here, because the bands we process from the
  // source region should be already coalesced.
  while (srcCur != srcEnd)
  {
    *dstCur++ = *srcCur++;
  }

  // --------------------------------------------------------------------------
  // [End]
  // --------------------------------------------------------------------------

_End:
  d->length = (size_t)(dstCur - d->data);
  BoxI::bound(d->boundingBox, d->boundingBox, *srcBoundingBox);

  FOG_ASSERT(Region_isValid(d));
  return ERR_OK;
}

// ============================================================================
// [Fog::Region - JoinAB]
// ============================================================================

static err_t FOG_CDECL Region_joinAB(Region* dst,
  const BoxI* a, const BoxI* aBoundingBox, size_t aLength,
  const BoxI* b, const BoxI* bBoundingBox, size_t bLength)
{
  size_t resultLength = aLength + bLength;

  if (resultLength < aLength || resultLength < bLength)
    return ERR_RT_OUT_OF_MEMORY;

  FOG_RETURN_ON_ERROR(dst->prepare(resultLength));
  RegionData* d = dst->_d;

  d->length = aLength;
  d->boundingBox = *aBoundingBox;

  BoxI* data = d->data;
  for (size_t i = 0; i < aLength; i++)
    data[i] = a[i];

  return Region_joinTo(dst, b, bBoundingBox, bLength);
}

// ============================================================================
// [Fog::Region - Clip]
// ============================================================================

static err_t FOG_CDECL Region_clip(Region* dst, const Region* src, const BoxI* clipBox)
{
  RegionData* d = dst->_d;
  RegionData* src_d = src->_d;

  size_t length = src_d->length;
  FOG_ASSERT(length > 0);

  if (d->reference.get() != 1 || d->capacity < length)
  {
    d = _api.region_dCreate(length);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  FOG_ASSERT(d->capacity >= length);
  BoxI* dCur = d->data;
  BoxI* dEnd = dCur + length;

  BoxI* dPrevBand = NULL;
  BoxI* dCurBand;

  BoxI* sCur = src_d->data;
  BoxI* sEnd = sCur + length;

  int cx0 = clipBox->x0;
  int cy0 = clipBox->y0;
  int cx1 = clipBox->x1;
  int cy1 = clipBox->y1;

  int dBoundingX0 = INT_MAX;
  int dBoundingX1 = INT_MIN;

  // Skip boxes which do not intersect with the clip-box.
  while (sCur->y1 <= cy0)
  {
    if (++sCur == sEnd)
      goto _End;
  }

  // Do the intersection part.
  for (;;)
  {
    FOG_ASSERT(sCur != sEnd);

    int bandY0 = sCur->y0;
    if (bandY0 >= cy1)
      break;

    int y0;
    int y1 = 0; // Be quite.

    // Skip leading boxes which do not intersect with the clip-box.
    while (sCur->x1 <= cx0)
    {
      if (++sCur == sEnd) goto _End;
      if (sCur->y0 != bandY0) goto _Skip;
    }

    dCurBand = dCur;

    // Do the inner part.
    if (sCur->x0 < cx1)
    {
      y0 = Math::max(sCur->y0, cy0);
      y1 = Math::min(sCur->y1, cy1);

      // First box.
      FOG_ASSERT(dCur != dEnd);
      dCur->setBox(Math::max(sCur->x0, cx0), y0, Math::min(sCur->x1, cx1), y1);
      dCur++;

      if (++sCur == sEnd || sCur->y0 != bandY0)
        goto _Merge;

      // Inner boxes.
      while (sCur->x1 <= cx1)
      {
        FOG_ASSERT(dCur != dEnd);
        FOG_ASSERT(sCur->x0 >= cx0 && sCur->x1 <= cx1);

        dCur->setBox(sCur->x0, y0, sCur->x1, y1);
        dCur++;

        if (++sCur == sEnd || sCur->y0 != bandY0)
          goto _Merge;
      }

      // Last box.
      if (sCur->x0 < cx1)
      {
        FOG_ASSERT(dCur != dEnd);
        FOG_ASSERT(sCur->x0 >= cx0);

        dCur->setBox(sCur->x0, y0, Math::min(sCur->x1, cx1), y1);
        dCur++;

        if (++sCur == sEnd || sCur->y0 != bandY0)
          goto _Merge;
      }

      FOG_ASSERT(sCur->x0 >= cx1);
    }

    // Skip trailing boxes which do not intersect with the clip-box.
    while (sCur->x0 >= cx1)
    {
      if (++sCur == sEnd || sCur->y0 != bandY0)
        break;
    }

_Merge:
    if (dCurBand != dCur)
    {
      if (dBoundingX0 > dCurBand[0].x0) dBoundingX0 = dCurBand[0].x0;
      if (dBoundingX1 < dCur   [-1].x1) dBoundingX1 = dCur   [-1].x1;

      if (dPrevBand != NULL)
        dCur = Region_coalesce(dCur, &dPrevBand, &dCurBand, y1);
      else
        dPrevBand = dCurBand;
    }

_Skip:
    if (sCur == sEnd)
      break;
  }

_End:
  length = (size_t)(dCur - d->data);
  d->length = length;

  if (length == 0)
    d->boundingBox.reset();
  else
    d->boundingBox.setBox(dBoundingX0, d->data[0].y0, dBoundingX1, d->data[length-1].y1);
  FOG_ASSERT(Region_isValid(d));

  if (d != dst->_d)
    atomicPtrXchg(&dst->_d, d)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Region - Combine - AB]
// ============================================================================

static err_t FOG_CDECL Region_combineAB(Region* dst,
  const BoxI* aCur, const BoxI* aBoundingBox, size_t aLength,
  const BoxI* bCur, const BoxI* bBoundingBox, size_t bLength,
  uint32_t combineOp, bool memOverlap)
{
  // These two operators must be handled by the caller before the
  // Region_combineAB() is called.
  FOG_ASSUME(combineOp <  REGION_OP_COUNT);
  FOG_ASSUME(combineOp != REGION_OP_REPLACE);
  FOG_ASSUME(combineOp != REGION_OP_SUBTRACT_REV);

  FOG_ASSUME(aLength > 0);
  FOG_ASSUME(bLength > 0);

  // --------------------------------------------------------------------------
  // [Estimated Size]
  // --------------------------------------------------------------------------

  // The resulting count of rectangles after (A && B) can't be larger than
  // (A + B) * 2. For other operators this value is only hint (the resulting
  // count of rectangles can be greater than the estimated value when run into
  // special cases). To prevent checking if there is space for a new rectangle
  // in the output buffer, use the _FOG_REGION_ENSURE_SPACE() macro. It should
  // be called for each band.
  size_t estimatedSize = 8 + (aLength + bLength) * 2;

  if (estimatedSize < aLength || estimatedSize < bLength)
    return ERR_RT_OUT_OF_MEMORY;

  // --------------------------------------------------------------------------
  // [Destination]
  // --------------------------------------------------------------------------

  RegionData* d = dst->_d;
  bool hasCustomData = memOverlap;

  if (memOverlap || d->capacity < estimatedSize || d->reference.get() != 1)
  {
    d = _api.region_dCreate(estimatedSize);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    hasCustomData = true;
  }

  size_t dCapacity = d->capacity;
  BoxI* dCur = d->data;
  BoxI* dEnd = d->data + dCapacity;

  BoxI* dPrevBand = NULL;
  BoxI* dCurBand = NULL;

  int dBoundingX0 = INT_MAX;
  int dBoundingX1 = INT_MIN;

#define _FOG_REGION_ENSURE_SPACE(_Space_, _IsFinal_) \
  FOG_MACRO_BEGIN \
    size_t remain = (size_t)(dEnd - dCur); \
    size_t needed = (size_t)(_Space_); \
    \
    if (FOG_UNLIKELY(remain < needed)) \
    { \
      size_t currentLength = (size_t)(dCur - d->data); \
      size_t prevBandIndex = (size_t)(dPrevBand - d->data); \
      size_t newCapacity = currentLength + needed; \
      \
      if (!_IsFinal_) \
        newCapacity = Math::max<size_t>(64, newCapacity * 2); \
      \
      d->length = currentLength; \
      RegionData* newd = _api.region_dRealloc(d, newCapacity); \
      \
      if (FOG_IS_NULL(newd)) \
        goto _OutOfMemory; \
      \
      d = newd; \
      dCapacity = d->capacity; \
      dCur = d->data + currentLength; \
      dEnd = d->data + dCapacity; \
      \
      if (dPrevBand != NULL) dPrevBand = d->data + prevBandIndex; \
    } \
  FOG_MACRO_END

  // --------------------------------------------------------------------------
  // [Source - A & B]
  // --------------------------------------------------------------------------

  const BoxI* aEnd = aCur + aLength;
  const BoxI* bEnd = bCur + bLength;

  const BoxI* aBandEnd = NULL;
  const BoxI* bBandEnd = NULL;

  int y0, y1;

  switch (combineOp)
  {
    // ------------------------------------------------------------------------
    // [Intersect]
    // ------------------------------------------------------------------------

    case REGION_OP_INTERSECT:
    {
      int yStop = Math::min(aBoundingBox->y1, bBoundingBox->y1);

      // Skip all parts which do not intersect. If there is no intersection
      // detected then this loop can skip into the _Clear label.
      for (;;)
      {
        if (aCur->y1 <= bCur->y0) { if (++aCur == aEnd) goto _Clear; else continue; }
        if (bCur->y1 <= aCur->y0) { if (++bCur == bEnd) goto _Clear; else continue; }
        break;
      }

      FOG_ASSERT(aCur != aEnd);
      FOG_ASSERT(bCur != bEnd);

      aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
      bBandEnd = RegionUtil::getEndBand(bCur, bEnd);

      for (;;)
      {
        // Vertical intersection of current A and B bands.
        y0 = Math::max(aCur->y0, bCur->y0);
        y1 = Math::min(aCur->y1, bCur->y1);

        if (y0 < y1)
        {
          const BoxI* aBand = aCur;
          const BoxI* bBand = bCur;

          dCurBand = dCur;

          for (;;)
          {
            // Skip boxes which do not intersect.
            if (aBand->x1 <= bBand->x0) { if (++aBand == aBandEnd) goto _IntersectBandDone; else continue; }
            if (bBand->x1 <= aBand->x0) { if (++bBand == bBandEnd) goto _IntersectBandDone; else continue; }

            // Horizontal intersection of current A and B boxes.
            int x0 = Math::max(aBand->x0, bBand->x0);
            int x1 = Math::min(aBand->x1, bBand->x1);

            FOG_ASSERT(x0 < x1);
            FOG_ASSERT(dCur != dEnd);

            dCur->setBox(x0, y0, x1, y1);
            dCur++;

            // Advance.
            if (aBand->x1 == x1 && ++aBand == aBandEnd) break;
            if (bBand->x1 == x1 && ++bBand == bBandEnd) break;
          }

          // Update bounding box and coalesce.
_IntersectBandDone:
          if (dCurBand != dCur)
          {
            if (dBoundingX0 > dCurBand[0].x0) dBoundingX0 = dCurBand[0].x0;
            if (dBoundingX1 < dCur   [-1].x1) dBoundingX1 = dCur   [-1].x1;

            if (dPrevBand != NULL)
              dCur = Region_coalesce(dCur, &dPrevBand, &dCurBand, y1);
            else
              dPrevBand = dCurBand;
          }
        }

        // Advance A.
        if (aCur->y1 == y1)
        {
          if ((aCur = aBandEnd) == aEnd || aCur->y0 >= yStop) break;
          aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
        }

        // Advance B.
        if (bCur->y1 == y1)
        {
          if ((bCur = bBandEnd) == bEnd || bCur->y0 >= yStop) break;
          bBandEnd = RegionUtil::getEndBand(bCur, bEnd);
        }
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [Union]
    // ------------------------------------------------------------------------

    case REGION_OP_UNION:
    {
      dBoundingX0 = Math::min(aBoundingBox->x0, bBoundingBox->x0);
      dBoundingX1 = Math::max(aBoundingBox->x1, bBoundingBox->x1);

      aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
      bBandEnd = RegionUtil::getEndBand(bCur, bEnd);

      y0 = Math::min(aCur->y0, bCur->y0);
      for (;;)
      {
        const BoxI* aBand = aCur;
        const BoxI* bBand = bCur;

        _FOG_REGION_ENSURE_SPACE((size_t)(aBandEnd - aBand) + (size_t)(bBandEnd - bBand), false);
        dCurBand = dCur;

        // Merge bands which do not intersect.
        if (bBand->y0 > y0)
        {
          y1 = Math::min(aBand->y1, bBand->y0);
          do {
            FOG_ASSERT(dCur != dEnd);
            dCur->setBox(aBand->x0, y0, aBand->x1, y1);
            dCur++;
          } while (++aBand != aBandEnd);
          goto _UnionBandDone;
        }

        if (aBand->y0 > y0)
        {
          y1 = Math::min(bBand->y1, aBand->y0);
          do {
            FOG_ASSERT(dCur != dEnd);
            dCur->setBox(bBand->x0, y0, bBand->x1, y1);
            dCur++;
          } while (++bBand != bBandEnd);
          goto _UnionBandDone;
        }

        // Vertical intersection of current A and B bands.
        y1 = Math::min(aBand->y1, bBand->y1);
        FOG_ASSERT(y0 < y1);

        do {
          int x0;
          int x1;

          if (aBand->x0 < bBand->x0)
          {
            x0 = aBand->x0;
            x1 = aBand->x1;
            aBand++;
          }
          else
          {
            x0 = bBand->x0;
            x1 = bBand->x1;
            bBand++;
          }

          for (;;)
          {
            bool didAdvance = false;

            while (aBand != aBandEnd && aBand->x0 <= x1)
            {
              x1 = Math::max(x1, aBand->x1);
              aBand++;
              didAdvance = true;
            }

            while (bBand != bBandEnd && bBand->x0 <= x1)
            {
              x1 = Math::max(x1, bBand->x1);
              bBand++;
              didAdvance = true;
            }

            if (!didAdvance)
              break;
          }

#if defined(FOG_DEBUG)
          FOG_ASSERT(dCur != dEnd);
          if (dCur != dCurBand)
            FOG_ASSERT(dCur[-1].x1 < x0);
#endif // FOG_DEBUG

          dCur->setBox(x0, y0, x1, y1);
          dCur++;
        } while (aBand != aBandEnd && bBand != bBandEnd);

        // Merge boxes which do not intersect.
        while (aBand != aBandEnd)
        {
#if defined(FOG_DEBUG)
          FOG_ASSERT(dCur != dEnd);
          if (dCur != dCurBand)
            FOG_ASSERT(dCur[-1].x1 < aBand->x0);
#endif // FOG_DEBUG

          dCur->setBox(aBand->x0, y0, aBand->x1, y1);
          dCur++;
          aBand++;
        }

        while (bBand != bBandEnd)
        {
#if defined(FOG_DEBUG)
          FOG_ASSERT(dCur != dEnd);
          if (dCur != dCurBand)
            FOG_ASSERT(dCur[-1].x1 < bBand->x0);
#endif // FOG_DEBUG

          dCur->setBox(bBand->x0, y0, bBand->x1, y1);
          dCur++;
          bBand++;
        }

        // Coalesce.
_UnionBandDone:
        if (dPrevBand != NULL)
          dCur = Region_coalesce(dCur, &dPrevBand, &dCurBand, y1);
        else
          dPrevBand = dCurBand;

        y0 = y1;

        // Advance A.
        if (aCur->y1 == y1)
        {
          if ((aCur = aBandEnd) == aEnd) break;
          aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
        }

        // Advance B.
        if (bCur->y1 == y1)
        {
          if ((bCur = bBandEnd) == bEnd) break;
          bBandEnd = RegionUtil::getEndBand(bCur, bEnd);
        }

        y0 = Math::max(y0, Math::min(aCur->y0, bCur->y0));
      }

      if (aCur != aEnd) goto _MergeA;
      if (bCur != bEnd) goto _MergeB;
      break;
    }

    // ------------------------------------------------------------------------
    // [Xor]
    // ------------------------------------------------------------------------

    case REGION_OP_XOR:
    {
      aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
      bBandEnd = RegionUtil::getEndBand(bCur, bEnd);

      y0 = Math::min(aCur->y0, bCur->y0);
      for (;;)
      {
        const BoxI* aBand = aCur;
        const BoxI* bBand = bCur;

        _FOG_REGION_ENSURE_SPACE(((size_t)(aBandEnd - aBand) + (size_t)(bBandEnd - bBand)) * 2, false);
        dCurBand = dCur;

        // Merge bands which do not intersect.
        if (bBand->y0 > y0)
        {
          y1 = Math::min(aBand->y1, bBand->y0);
          do {
            FOG_ASSERT(dCur != dEnd);
            dCur->setBox(aBand->x0, y0, aBand->x1, y1);
            dCur++;
          } while (++aBand != aBandEnd);
          goto _XorBandDone;
        }

        if (aBand->y0 > y0)
        {
          y1 = Math::min(bBand->y1, aBand->y0);
          do {
            FOG_ASSERT(dCur != dEnd);
            dCur->setBox(bBand->x0, y0, bBand->x1, y1);
            dCur++;
          } while (++bBand != bBandEnd);
          goto _XorBandDone;
        }

        // Vertical intersection of current A and B bands.
        y1 = Math::min(aBand->y1, bBand->y1);
        FOG_ASSERT(y0 < y1);

        {
          int pos = Math::min(aBand->x0, bBand->x0);
          int x0;
          int x1;

          for (;;)
          {
            if (aBand->x1 <= bBand->x0)
            {
              x0 = Math::max(aBand->x0, pos);
              x1 = aBand->x1;
              pos = x1;
              goto _XorMerge;
            }

            if (bBand->x1 <= aBand->x0)
            {
              x0 = Math::max(bBand->x0, pos);
              x1 = bBand->x1;
              pos = x1;
              goto _XorMerge;
            }

            x0 = pos;
            x1 = Math::max(aBand->x0, bBand->x0);
            pos = Math::min(aBand->x1, bBand->x1);

            if (x0 >= x1)
              goto _XorSkip;

_XorMerge:
            FOG_ASSERT(x0 < x1);
            if (dCur != dCurBand && dCur[-1].x1 == x0)
            {
              dCur[-1].x1 = x1;
            }
            else
            {
              dCur->setBox(x0, y0, x1, y1);
              dCur++;
            }

_XorSkip:
            if (aBand->x1 <= pos) aBand++;
            if (bBand->x1 <= pos) bBand++;

            if (aBand == aBandEnd || bBand == bBandEnd)
              break;
            pos = Math::max(pos, Math::min(aBand->x0, bBand->x0));
          }

          // Merge boxes which do not intersect.
          if (aBand != aBandEnd)
          {
            x0 = Math::max(aBand->x0, pos);
            for (;;)
            {
              x1 = aBand->x1;
              FOG_ASSERT(x0 < x1);
              FOG_ASSERT(dCur != dEnd);

              if (dCur != dCurBand && dCur[-1].x1 == x0)
              {
                dCur[-1].x1 = x1;
              }
              else
              {
                dCur->setBox(x0, y0, x1, y1);
                dCur++;
              }

              if (++aBand == aBandEnd) break;
              x0 = aBand->x0;
            }
          }

          if (bBand != bBandEnd)
          {
            x0 = Math::max(bBand->x0, pos);
            for (;;)
            {
              x1 = bBand->x1;
              FOG_ASSERT(x0 < x1);
              FOG_ASSERT(dCur != dEnd);

              if (dCur != dCurBand && dCur[-1].x1 == x0)
              {
                dCur[-1].x1 = x1;
              }
              else
              {
                dCur->setBox(x0, y0, x1, y1);
                dCur++;
              }

              if (++bBand == bBandEnd) break;
              x0 = bBand->x0;
            }
          }
        }

        // Update bounding box and coalesce.
_XorBandDone:
        if (dCurBand != dCur)
        {
          if (dBoundingX0 > dCurBand[0].x0) dBoundingX0 = dCurBand[0].x0;
          if (dBoundingX1 < dCur   [-1].x1) dBoundingX1 = dCur   [-1].x1;

          if (dPrevBand != NULL)
            dCur = Region_coalesce(dCur, &dPrevBand, &dCurBand, y1);
          else
            dPrevBand = dCurBand;
        }

        y0 = y1;

        // Advance A.
        if (aCur->y1 == y1)
        {
          if ((aCur = aBandEnd) == aEnd) break;
          aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
        }

        // Advance B.
        if (bCur->y1 == y1)
        {
          if ((bCur = bBandEnd) == bEnd) break;
          bBandEnd = RegionUtil::getEndBand(bCur, bEnd);
        }

        y0 = Math::max(y0, Math::min(aCur->y0, bCur->y0));
      }

      if (aCur != aEnd) goto _MergeA;
      if (bCur != bEnd) goto _MergeB;
      break;
    }

    // ------------------------------------------------------------------------
    // [Subtract]
    // ------------------------------------------------------------------------

    case REGION_OP_SUBTRACT:
    {
      aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
      bBandEnd = RegionUtil::getEndBand(bCur, bEnd);

      y0 = Math::min(aCur->y0, bCur->y0);
      for (;;)
      {
        const BoxI* aBand = aCur;
        const BoxI* bBand = bCur;

        _FOG_REGION_ENSURE_SPACE(((size_t)(aBandEnd - aBand) + (size_t)(bBandEnd - bBand)) * 2, false);
        dCurBand = dCur;

        // Merge (A) / Skip (B) bands which do not intersect.
        if (bBand->y0 > y0)
        {
          y1 = Math::min(aBand->y1, bBand->y0);
          do {
            FOG_ASSERT(dCur != dEnd);
            dCur->setBox(aBand->x0, y0, aBand->x1, y1);
            dCur++;
          } while (++aBand != aBandEnd);
          goto _SubtractBandDone;
        }

        if (aBand->y0 > y0)
        {
          y1 = Math::min(bBand->y1, aBand->y0);
          goto _SubtractBandSkip;
        }

        // Vertical intersection of current A and B bands.
        y1 = Math::min(aBand->y1, bBand->y1);
        FOG_ASSERT(y0 < y1);

        {
          int pos = aBand->x0;
          int sub = bBand->x0;

          int x0;
          int x1;

          for (;;)
          {
            if (aBand->x1 <= sub)
            {
              x0 = pos;
              x1 = aBand->x1;
              pos = x1;

              if (x0 < x1)
                goto _SubtractMerge;
              else
                goto _SubtractSkip;
            }

            if (aBand->x0 >= sub)
            {
              pos = bBand->x1;
              goto _SubtractSkip;
            }

            x0 = pos;
            x1 = bBand->x0;
            pos = bBand->x1;

_SubtractMerge:
            FOG_ASSERT(x0 < x1);
            dCur->setBox(x0, y0, x1, y1);
            dCur++;

_SubtractSkip:
            if (aBand->x1 <= pos) aBand++;
            if (bBand->x1 <= pos) bBand++;

            if (aBand == aBandEnd || bBand == bBandEnd)
              break;

            sub = bBand->x0;
            pos = Math::max(pos, aBand->x0);
          }

          // Merge boxes (A) / Ignore boxes (B) which do not intersect.
          while (aBand != aBandEnd)
          {
            x0 = Math::max(aBand->x0, pos);
            x1 = aBand->x1;

            if (x0 < x1)
            {
              FOG_ASSERT(dCur != dEnd);
              dCur->setBox(x0, y0, x1, y1);
              dCur++;
            }
            aBand++;
          }
        }

        // Update bounding box and coalesce.
_SubtractBandDone:
        if (dCurBand != dCur)
        {
          if (dBoundingX0 > dCurBand[0].x0) dBoundingX0 = dCurBand[0].x0;
          if (dBoundingX1 < dCur   [-1].x1) dBoundingX1 = dCur   [-1].x1;

          if (dPrevBand != NULL)
            dCur = Region_coalesce(dCur, &dPrevBand, &dCurBand, y1);
          else
            dPrevBand = dCurBand;
        }

_SubtractBandSkip:
        y0 = y1;

        // Advance A.
        if (aCur->y1 == y1)
        {
          if ((aCur = aBandEnd) == aEnd) break;
          aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
        }

        // Advance B.
        if (bCur->y1 == y1)
        {
          if ((bCur = bBandEnd) == bEnd) break;
          bBandEnd = RegionUtil::getEndBand(bCur, bEnd);
        }

        y0 = Math::max(y0, Math::min(aCur->y0, bCur->y0));
      }

      if (aCur != aEnd) goto _MergeA;
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // --------------------------------------------------------------------------
  // [End]
  // --------------------------------------------------------------------------

_End:
  d->length = (size_t)(dCur - d->data);

  if (d->length == 0)
    d->boundingBox.reset();
  else
    d->boundingBox.setBox(dBoundingX0, d->data[0].y0, dBoundingX1, dCur[-1].y1);

  FOG_ASSERT(Region_isValid(d));
  d = atomicPtrXchg(&dst->_d, d);

  if (hasCustomData)
    d->release();
  return ERR_OK;

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

_Clear:
  if (hasCustomData)
    d->release();

  dst->clear();
  return ERR_OK;

  // --------------------------------------------------------------------------
  // [Out Of Memory]
  // --------------------------------------------------------------------------

_OutOfMemory:
  if (hasCustomData)
    d->release();
  return ERR_RT_OUT_OF_MEMORY;

  // --------------------------------------------------------------------------
  // [Merge]
  // --------------------------------------------------------------------------

_MergeB:
  FOG_ASSERT(aCur == aEnd);

  aCur = bCur;
  aEnd = bEnd;
  aBandEnd = bBandEnd;

_MergeA:
  FOG_ASSERT(aCur != aEnd);

  if (y0 >= aCur->y1)
  {
    if ((aCur = aBandEnd) == aEnd) goto _End;
    aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
  }

  if (y0 < aCur->y0)
    y0 = aCur->y0;

  y1 = aCur->y1;

  _FOG_REGION_ENSURE_SPACE((size_t)(aEnd - aCur), true);
  dCurBand = dCur;

  do {
    FOG_ASSERT(dCur != dEnd);
    dCur->setBox(aCur->x0, y0, aCur->x1, y1);
    dCur++;
  } while (++aCur != aBandEnd);

  if (dBoundingX0 > dCurBand[0].x0) dBoundingX0 = dCurBand[0].x0;
  if (dBoundingX1 < dCur   [-1].x1) dBoundingX1 = dCur   [-1].x1;

  if (dPrevBand != NULL)
    dCur = Region_coalesce(dCur, &dPrevBand, &dCurBand, y1);

  if (aCur == aEnd)
    goto _End;

  if (combineOp == REGION_OP_UNION)
  {
    // Special case for UNION. The bounding box is easily calculated using A
    // and B bounding boxes. We don't need to contribute to the calculation
    // within this loop.
    do {
      FOG_ASSERT(dCur != dEnd);
      dCur->setBox(aCur->x0, aCur->y0, aCur->x1, aCur->y1);
      dCur++;
    } while (++aCur != aEnd);
  }
  else
  {
    do {
      FOG_ASSERT(dCur != dEnd);
      dCur->setBox(aCur->x0, aCur->y0, aCur->x1, aCur->y1);
      dCur++;

      if (dBoundingX0 > aCur->x0) dBoundingX0 = aCur->x0;
      if (dBoundingX1 < aCur->x1) dBoundingX1 = aCur->x1;
    } while (++aCur != aEnd);
  }

  goto _End;
}

// ============================================================================
// [Fog::Region - Combine - Region + Region]
// ============================================================================

// Infinite regions require special handling to keep the special infinite
// region data with the Region instance. However, if the combining operator
// is one of XOR, SUBTRACT, or SUBTRACT_REV, then the infinity information
// might be lost.
static err_t FOG_CDECL Region_combineRegionRegion(Region* dst, const Region* a, const Region* b, uint32_t combineOp)
{
  RegionData* a_d = a->_d;
  RegionData* b_d = b->_d;

  size_t aLength = a_d->length;
  size_t bLength = b_d->length;

  // Run fast-paths when possible. The empty regions are handled too.
  if (aLength <= 1)
  {
    // Handle the infinite region A.
    if (a_d == &Region_dInfinite)
    {
      switch (combineOp)
      {
        case REGION_OP_REPLACE     : goto _SetB;
        case REGION_OP_INTERSECT   : goto _SetB;
        case REGION_OP_UNION       : goto _SetA;
        case REGION_OP_XOR         : break;
        case REGION_OP_SUBTRACT    : break;
        case REGION_OP_SUBTRACT_REV: goto _Clear;

        default:
          return ERR_RT_INVALID_ARGUMENT;
      }
    }

    BoxI aBox(a_d->boundingBox);
    return _api.region_combineBoxRegion(dst, &aBox, b, combineOp);
  }

  if (bLength <= 1)
  {
    // Handle the infinite region B.
    if (b_d == &Region_dInfinite)
    {
      switch (combineOp)
      {
        case REGION_OP_REPLACE     : goto _SetB;
        case REGION_OP_INTERSECT   : goto _SetA;
        case REGION_OP_UNION       : goto _SetB;
        case REGION_OP_XOR         : break;
        case REGION_OP_SUBTRACT    : goto _Clear;
        case REGION_OP_SUBTRACT_REV: break;

        default:
          return ERR_RT_INVALID_ARGUMENT;
      }
    }

    BoxI bBox(b_d->boundingBox);
    return _api.region_combineRegionBox(dst, a, &bBox, combineOp);
  }

  FOG_ASSERT(aLength > 1);
  FOG_ASSERT(bLength > 1);

  switch (combineOp)
  {
    // ------------------------------------------------------------------------
    // [Replace]
    // ------------------------------------------------------------------------

    case REGION_OP_REPLACE:
      goto _SetB;

    // ------------------------------------------------------------------------
    // [Intersect]
    // ------------------------------------------------------------------------

    case REGION_OP_INTERSECT:
      if (!a_d->boundingBox.overlaps(b_d->boundingBox))
        goto _Clear;

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Union & Join]
    // ------------------------------------------------------------------------

    case REGION_OP_UNION:
_Union:
      // Check whether to use JOIN instead of UNION. This is special case, but
      // happens often in case that the region is constructed using UNION
      // operator from many already sorted boxes / regions.
      if (a_d->boundingBox.y1 <= b_d->boundingBox.y0 || Region_canJoinBoxes(a_d->data[aLength - 1], b_d->data[0]))
      {
        if (dst == a)
        {
          return Region_joinTo(dst, b_d->data, &b_d->boundingBox, bLength);
        }
        if (dst == b)
        {
          Region bTmp(*b);

          return Region_joinAB(dst, a_d->data, &a_d->boundingBox, aLength,
                                    b_d->data, &b_d->boundingBox, bLength);
        }
        else
        {
          return Region_joinAB(dst, a_d->data, &a_d->boundingBox, aLength,
                                    b_d->data, &b_d->boundingBox, bLength);
        }
      }

      if (b_d->boundingBox.y1 <= a_d->boundingBox.y0 || Region_canJoinBoxes(b_d->data[bLength - 1], a_d->data[0]))
      {
        if (dst == b)
        {
          return Region_joinTo(dst, a_d->data, &a_d->boundingBox, aLength);
        }
        if (dst == a)
        {
          Region aTmp(*a);

          return Region_joinAB(dst, b_d->data, &b_d->boundingBox, bLength,
                                    a_d->data, &a_d->boundingBox, aLength);
        }
        else
        {
          return Region_joinAB(dst, b_d->data, &b_d->boundingBox, bLength,
                                    a_d->data, &a_d->boundingBox, aLength);
        }
      }

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Xor]
    // ------------------------------------------------------------------------

    case REGION_OP_XOR:
      // Check whether to use UNION instead of XOR.
      if (!a_d->boundingBox.overlaps(b_d->boundingBox))
      {
        combineOp = REGION_OP_UNION;
        goto _Union;
      }

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Subtract]
    // ------------------------------------------------------------------------

    case REGION_OP_SUBTRACT:
      if (!a_d->boundingBox.overlaps(b_d->boundingBox))
        goto _SetA;

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Subtract-Reverse]
    // ------------------------------------------------------------------------

    case REGION_OP_SUBTRACT_REV:
      if (!a_d->boundingBox.overlaps(b_d->boundingBox))
        goto _SetB;

      swap(a, b);
      swap(a_d, b_d);
      swap(aLength, bLength);

      combineOp = REGION_OP_SUBTRACT;
      goto _Combine;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

_Combine:
  return Region_combineAB(dst,
    a_d->data, &a_d->boundingBox, aLength,
    b_d->data, &b_d->boundingBox, bLength,
    combineOp, dst->_d == a_d || dst->_d == b_d);

_Clear:
  dst->clear();
  return ERR_OK;

_SetA:
  return dst->setRegion(*a);

_SetB:
  return dst->setRegion(*b);
}

// ============================================================================
// [Fog::Region - Combine - Region + Box]
// ============================================================================

static err_t FOG_CDECL Region_combineRegionBox(Region* dst, const Region* a, const BoxI* b, uint32_t combineOp)
{
  RegionData* a_d = a->_d;

  // Handle the infinite region A.
  if (a_d == &Region_dInfinite)
  {
    switch (combineOp)
    {
      case REGION_OP_REPLACE     : goto _SetB;
      case REGION_OP_INTERSECT   : goto _SetB;
      case REGION_OP_UNION       : goto _SetA;
      case REGION_OP_XOR         : break;
      case REGION_OP_SUBTRACT    : break;
      case REGION_OP_SUBTRACT_REV: goto _Clear;

      default:
        return ERR_RT_INVALID_ARGUMENT;
    }
  }

  // Run fast-paths when possible. The empty regions are handled too.
  if (a_d->length <= 1)
    return _api.region_combineBoxBox(dst, &a_d->boundingBox, b, combineOp);

  switch (combineOp)
  {
    // ------------------------------------------------------------------------
    // [Replace]
    // ------------------------------------------------------------------------

    case REGION_OP_REPLACE:
      if (!b->isValid())
        goto _Clear;
      else
        goto _SetB;

    // ------------------------------------------------------------------------
    // [Intersect]
    // ------------------------------------------------------------------------

    case REGION_OP_INTERSECT:
      if (!b->isValid() || !b->overlaps(a_d->boundingBox))
        goto _Clear;

      if (b->subsumes(a_d->boundingBox))
        goto _SetA;

      return Region_clip(dst, a, b);

    // ------------------------------------------------------------------------
    // [Union]
    // ------------------------------------------------------------------------

    case REGION_OP_UNION:
      if (!b->isValid())
        goto _SetA;

      // Check whether to use JOIN instead of UNION.
      if (a_d->boundingBox.y1 <= b->y0 || Region_canJoinBoxes(a_d->data[a_d->length-1], *b))
      {
_Join:
        if (dst == a)
          return Region_joinTo(dst, b, b, 1);
        else
          return Region_joinAB(dst, a_d->data, &a_d->boundingBox, a_d->length, b, b, 1);
      }

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Xor]
    // ------------------------------------------------------------------------

    case REGION_OP_XOR:
      if (!b->isValid())
        goto _SetA;

      // Check whether to use JOIN instead of XOR.
      if (a_d->boundingBox.y1 <= b->y0 || Region_canJoinBoxes(a_d->data[a_d->length-1], *b))
        goto _Join;

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Subtract]
    // ------------------------------------------------------------------------

    case REGION_OP_SUBTRACT:
      if (!b->isValid() || !b->overlaps(a_d->boundingBox))
        goto _SetA;

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Subtract-Reverse]
    // ------------------------------------------------------------------------

    case REGION_OP_SUBTRACT_REV:
      if (!b->isValid())
        goto _Clear;

      if (!b->overlaps(a_d->boundingBox))
        goto _SetB;

      return Region_combineAB(dst, b, b, 1, a_d->data, &a_d->boundingBox, a_d->length, REGION_OP_SUBTRACT, dst->_d == a_d);

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

_Combine:
  return Region_combineAB(dst, a_d->data, &a_d->boundingBox, a_d->length, b, b, 1, combineOp, dst->_d == a_d);

_Clear:
  dst->clear();
  return ERR_OK;

_SetA:
  return dst->setRegion(*a);

_SetB:
  return dst->setBox(*b);
}

// ============================================================================
// [Fog::Region - Combine - Box + Region]
// ============================================================================

static err_t FOG_CDECL Region_combineBoxRegion(Region* dst, const BoxI* a, const Region* b, uint32_t combineOp)
{
  RegionData* b_d = b->_d;

  // Handle the infinite region B.
  if (b_d == &Region_dInfinite)
  {
    switch (combineOp)
    {
      case REGION_OP_REPLACE     : goto _SetB;
      case REGION_OP_INTERSECT   : goto _SetA;
      case REGION_OP_UNION       : goto _SetB;
      case REGION_OP_XOR         : break;
      case REGION_OP_SUBTRACT    : goto _Clear;
      case REGION_OP_SUBTRACT_REV: break;

      default:
        return ERR_RT_INVALID_ARGUMENT;
    }
  }

  // Run fast-paths when possible. The empty regions are handled too.
  if (b_d->length <= 1)
    return _api.region_combineBoxBox(dst, a, &b_d->boundingBox, combineOp);

  switch (combineOp)
  {
    // ------------------------------------------------------------------------
    // [Replace]
    // ------------------------------------------------------------------------

    case REGION_OP_REPLACE:
      goto _SetB;

    // ------------------------------------------------------------------------
    // [Intersect]
    // ------------------------------------------------------------------------

    case REGION_OP_INTERSECT:
      if (!a->isValid() || !a->overlaps(b_d->boundingBox))
        goto _Clear;

      if (a->subsumes(b_d->boundingBox))
        goto _SetB;

      return Region_clip(dst, b, a);

    // ------------------------------------------------------------------------
    // [Union]
    // ------------------------------------------------------------------------

    case REGION_OP_UNION:
      if (!a->isValid())
        goto _SetB;

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Xor]
    // ------------------------------------------------------------------------

    case REGION_OP_XOR:
      if (!a->isValid())
        goto _SetB;

      if (!a->overlaps(b_d->boundingBox))
        combineOp = REGION_OP_UNION;

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Subtract]
    // ------------------------------------------------------------------------

    case REGION_OP_SUBTRACT:
      if (!a->isValid())
        goto _Clear;

      if (!a->overlaps(b_d->boundingBox))
        goto _SetA;

      goto _Combine;

    // ------------------------------------------------------------------------
    // [Subtract-Reverse]
    // ------------------------------------------------------------------------

    case REGION_OP_SUBTRACT_REV:
      if (!a->isValid() || !a->overlaps(b_d->boundingBox))
        goto _SetB;

      return Region_combineAB(dst, b_d->data, &b_d->boundingBox, b_d->length, a, a, 1, combineOp, dst->_d == b_d);

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

_Combine:
  return Region_combineAB(dst, a, a, 1, b_d->data, &b_d->boundingBox, b_d->length, combineOp, dst->_d == b_d);

_Clear:
  dst->clear();
  return ERR_OK;

_SetA:
  return dst->setBox(*a);

_SetB:
  return dst->setRegion(*b);
}

// ============================================================================
// [Fog::Region - Combine - Box + Box]
// ============================================================================

static err_t FOG_CDECL Region_combineBoxBox(Region* dst, const BoxI* a, const BoxI* b, uint32_t combineOp)
{
  // There are several combinations of A and B.
  //
  // Special cases:
  //
  //   1) Both rectangles are invalid.
  //   2) Only one rectangle is valid (one invalid).
  //   3) Rectangles do not intersect, but they are continuous on the Y axis.
  //      (In some cases this can be extended that rectangles intersect, but
  //       they left and right coordinates are shared.)
  //   4) Rectangles do not intersect, but they are continuous on the X axis.
  //      (In some cases this can be extended that rectangles intersect, but
  //       they top and bottom coordinates are shared.)
  //
  // Common cases:
  //
  //   5) Rectangles do not intersect and do not share area on the Y axis.
  //   6) Rectangles do not intersect, but they share area on the Y axis.
  //   7) Rectangles intersect.
  //   8) One rectangle overlaps the other.
  //
  //   +--------------+--------------+--------------+--------------+
  //   |       1)     |       2)     |       3)     |       4)     |
  //   |              |              |              |              |
  //   |              |  +--------+  |  +--------+  |  +----+----+ |
  //   |              |  |        |  |  |        |  |  |    |    | |
  //   |              |  |        |  |  |        |  |  |    |    | |
  //   |              |  |        |  |  +--------+  |  |    |    | |
  //   |              |  |        |  |  |        |  |  |    |    | |
  //   |              |  |        |  |  |        |  |  |    |    | |
  //   |              |  |        |  |  |        |  |  |    |    | |
  //   |              |  +--------+  |  +--------+  |  +----+----+ |
  //   |              |              |              |              |
  //   +--------------+--------------+--------------+--------------+
  //   |       5)     |       6)     |       7)     |       8)     |
  //   |              |              |              |              |
  //   |  +----+      | +---+        |  +-----+     |  +--------+  |
  //   |  |    |      | |   |        |  |     |     |  |        |  |
  //   |  |    |      | |   | +----+ |  |  +--+--+  |  |  +--+  |  |
  //   |  +----+      | +---+ |    | |  |  |  |  |  |  |  |  |  |  |
  //   |              |       |    | |  +--+--+  |  |  |  +--+  |  |
  //   |      +----+  |       +----+ |     |     |  |  |        |  |
  //   |      |    |  |              |     +-----+  |  +--------+  |
  //   |      |    |  |              |              |              |
  //   |      +----+  |              |              |              |
  //   +--------------+--------------+--------------+--------------+

  // Maximum number of generated rectangles by any operator is 4.
  BoxI box[4];
  uint boxCount = 0;

  switch (combineOp)
  {
    // ------------------------------------------------------------------------
    // [Replace]
    // ------------------------------------------------------------------------

    case REGION_OP_REPLACE:
_Replace:
      if (!b->isValid()) goto _Clear;
      goto _SetB;

    // ------------------------------------------------------------------------
    // [Intersect]
    // ------------------------------------------------------------------------

    case REGION_OP_INTERSECT:
      // Case 1, 2, 3, 4, 5, 6.
      if (!BoxI::intersect(box[0], *a, *b))
        goto _Clear;

      // Case 7, 8.
      goto _SetBox;

    // ------------------------------------------------------------------------
    // [Union]
    // ------------------------------------------------------------------------

    case REGION_OP_UNION:
      // Case 1, 2.
      if (!a->isValid()) goto _Replace;
      if (!b->isValid()) goto _SetA;

      // Make first the upper rectangle.
      if (a->y0 > b->y0)
        swap(a, b);

_Union:
      // Case 3, 5.
      if (a->y1 <= b->y0)
      {
        box[0] = *a;
        box[1] = *b;
        boxCount = 2;

        // Coalesce (Case 3).
        if (box[0].y1 == box[1].y0 && box[0].x0 == box[1].x0 && box[0].x1 == box[1].x1)
        {
          box[0].y1 = box[1].y1;
          boxCount = 1;
        }

        goto _SetList;
      }

      // Case 4 - with addition that rectangles can intersect.
      //        - with addition that rectangles do not need to be
      //          continuous on the X axis.
      if (a->y0 == b->y0 && a->y1 == b->y1)
      {
        box[0].y0 = a->y0;
        box[0].y1 = a->y1;

        if (a->x0 > b->x0)
          swap(a, b);

        box[0].x0 = a->x0;
        box[0].x1 = a->x1;

        // Intersects or continuous.
        if (b->x0 <= a->x1)
        {
          if (b->x1 > a->x1)
            box[0].x1 = b->x1;
          boxCount = 1;
          goto _SetBox;
        }
        else
        {
          box[1].setBox(b->x0, box[0].y0, b->x1, box[0].y1);
          boxCount = 2;
          goto _SetList;
        }
      }

      // Case 6, 7, 8.
      FOG_ASSERT(b->y0 < a->y1);
      {
        // Top part.
        if (a->y0 < b->y0)
        {
          box[0].setBox(a->x0, a->y0, a->x1, b->y0);
          boxCount = 1;
        }

        // Inner part.
        int iy0 = b->y0;
        int iy1 = Math::min(a->y1, b->y1);

        if (a->x0 > b->x0)
          swap(a, b);

        int ix0 = Math::max(a->x0, b->x0);
        int ix1 = Math::min(a->x1, b->x1);

        if (ix0 > ix1)
        {
          box[boxCount + 0].setBox(a->x0, iy0, a->x1, iy1);
          box[boxCount + 1].setBox(b->x0, iy0, b->x1, iy1);
          boxCount += 2;
        }
        else
        {
          FOG_ASSERT(a->x1 >= ix0 && b->x0 <= ix1);

          // If the A or B subsumes the intersection area, extend also the iy1
          // and skip the bottom part (we join it).
          if (a->x0 <= ix0 && a->x1 >= ix1 && iy1 < a->y1) iy1 = a->y1;
          if (b->x0 <= ix0 && b->x1 >= ix1 && iy1 < b->y1) iy1 = b->y1;
          box[boxCount].setBox(a->x0, iy0, Math::max(a->x1, b->x1), iy1);

          // Coalesce.
          if (boxCount == 1 && box[0].x0 == box[1].x0 && box[0].x1 == box[1].x1)
            box[0].y1 = box[1].y1;
          else
            boxCount++;
        }

        // Bottom part.
        if (a->y1 > iy1)
        {
          box[boxCount].setBox(a->x0, iy1, a->x1, a->y1);
          goto _UnionLastCoalesce;
        }
        else if (b->y1 > iy1)
        {
          box[boxCount].setBox(b->x0, iy1, b->x1, b->y1);
_UnionLastCoalesce:
          if (boxCount == 1 && box[0].x0 == box[1].x0 && box[0].x1 == box[1].x1)
            box[0].y1 = box[1].y1;
          else
            boxCount++;
        }
      }
      goto _SetList;

    // ------------------------------------------------------------------------
    // [Xor]
    // ------------------------------------------------------------------------

    case REGION_OP_XOR:
      if (!a->isValid()) goto _Replace;
      if (!b->isValid()) goto _SetA;

      // Make first the upper rectangle.
      if (a->y0 > b->y0)
        swap(a, b);

      // Case 3, 4, 5, 6.
      //
      // If the input boxes A and B do not intersect then we can use UNION
      // operator instead.
      if (!BoxI::intersect(box[3], *a, *b))
        goto _Union;

      // Case 7, 8.
      //
      // Top part.
      if (a->y0 < b->y0)
      {
        box[0].setBox(a->x0, a->y0, a->x1, b->y0);
        boxCount = 1;
      }

      // Inner part.
      if (a->x0 > b->x0)
        swap(a, b);

      if (a->x0 < box[3].x0)
        box[boxCount++].setBox(a->x0, box[3].y0, box[3].x0, box[3].y1);
      if (b->x1 > box[3].x1)
        box[boxCount++].setBox(box[3].x1, box[3].y0, b->x1, box[3].y1);

      // Bottom part.
      if (a->y1 > b->y1)
        swap(a, b);

      if (b->y1 > box[3].y1)
        box[boxCount++].setBox(b->x0, box[3].y1, b->x1, b->y1);

      if (boxCount == 0)
        goto _Clear;

      goto _SetList;

    // ------------------------------------------------------------------------
    // [Subtract]
    // ------------------------------------------------------------------------

    case REGION_OP_SUBTRACT:
_Subtract:
      if (!a->isValid()) goto _Clear;
      if (!b->isValid()) goto _SetA;

      // Case 3, 4, 5, 6.
      //
      // If the input boxes A and B do not intersect then the result is A.
      if (!BoxI::intersect(box[3], *a, *b))
        goto _SetA;

      // Case 7, 8.
      //
      // Top part.
      if (a->y0 < b->y0)
      {
        box[0].setBox(a->x0, a->y0, a->x1, box[3].y0);
        boxCount = 1;
      }

      // Inner part.
      if (a->x0 < box[3].x0)
        box[boxCount++].setBox(a->x0, box[3].y0, box[3].x0, box[3].y1);
      if (box[3].x1 < a->x0)
        box[boxCount++].setBox(box[3].x1, box[3].y0, a->x1, box[3].y1);

      // Bottom part.
      if (a->y1 > box[3].y1)
        box[boxCount++].setBox(a->x0, box[3].y1, a->x1, a->y1);

      if (boxCount == 0)
        goto _Clear;

      goto _SetList;

    // ------------------------------------------------------------------------
    // [Subtract-Reverse]
    // ------------------------------------------------------------------------

    case REGION_OP_SUBTRACT_REV:
      swap(a, b);
      goto _Subtract;

    // ------------------------------------------------------------------------
    // [Invalid]
    // ------------------------------------------------------------------------

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

_Clear:
  dst->clear();
  return ERR_OK;

_SetA:
  box[0] = *a;
  goto _SetBox;

_SetB:
  box[0] = *b;

_SetBox:
  {
    FOG_ASSERT(box[0].isValid());
    FOG_RETURN_ON_ERROR(dst->prepare(1));

    RegionData* d = dst->_d;
    d->length = 1;

    d->boundingBox = box[0];
    d->data[0] = box[0];

    FOG_ASSERT(Region_isValid(d));
    return ERR_OK;
  }

_SetList:
  {
    FOG_ASSERT(boxCount > 0);
    FOG_RETURN_ON_ERROR(dst->prepare(boxCount));

    RegionData* d = dst->_d;
    d->length = boxCount;
    Region_copyDataEx(d->data, box, boxCount, &d->boundingBox);

    FOG_ASSERT(Region_isValid(d));
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::Region - Translate]
// ============================================================================

static err_t FOG_CDECL Region_translate(Region* dst, const Region* src, const PointI* pt)
{
  RegionData* d = dst->_d;
  RegionData* src_d = src->_d;

  int tx = pt->x;
  int ty = pt->y;

  if (src_d == &Region_dInfinite || (tx | ty) == 0)
  {
    return dst->setRegion(*src);
  }

  size_t length = src_d->length;
  if (length == 0)
  {
    dst->clear();
    return ERR_OK;
  }

  // If the translation cause arithmetic overflow or underflow then we first
  // clip the input region into safe boundary which might be translated.
  bool saturate = ((tx < 0) & (src_d->boundingBox.x0 < INT_MIN - tx)) |
                  ((ty < 0) & (src_d->boundingBox.y0 < INT_MIN - ty)) |
                  ((tx > 0) & (src_d->boundingBox.x1 > INT_MAX - tx)) |
                  ((ty > 0) & (src_d->boundingBox.y1 > INT_MAX - ty)) ;

  if (saturate)
  {
    BoxI clipBox(INT_MIN, INT_MIN, INT_MAX, INT_MAX);
    return _api.region_translateAndClip(dst, src, pt, &clipBox);
  }

  if (d->reference.get() != 1 || d->capacity < length)
  {
    d = _api.region_dCreate(length);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  BoxI* dCur = d->data;
  BoxI* sCur = src_d->data;

  d->length = length;
  d->boundingBox.setBox(src_d->boundingBox.x0 + tx, src_d->boundingBox.y0 + ty,
                        src_d->boundingBox.x1 + tx, src_d->boundingBox.y1 + ty);

  for (size_t i = 0; i < length; i++)
  {
    dCur[i].setBox(sCur->x0 + tx, sCur->y0 + ty, sCur->x1 + tx, sCur->y1 + ty);
  }

  if (d != dst->_d)
    atomicPtrXchg(&dst->_d, d)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Region - TranslateAndClip]
// ============================================================================

static err_t FOG_CDECL Region_translateAndClip(Region* dst, const Region* src, const PointI* pt, const BoxI* clipBox)
{
  RegionData* d = dst->_d;
  RegionData* src_d = src->_d;

  size_t length = src_d->length;
  if (length == 0 || !clipBox->isValid())
  {
    dst->clear();
    return ERR_OK;
  }

  if (src_d == &Region_dInfinite)
    return dst->setBox(*clipBox);

  int tx = pt->x;
  int ty = pt->y;

  // Use faster Region_clip() in case that there is no translation.
  if ((tx | ty) == 0)
    return Region_clip(dst, src, clipBox);

  int cx0 = clipBox->x0;
  int cy0 = clipBox->y0;
  int cx1 = clipBox->x1;
  int cy1 = clipBox->y1;

  if (tx < 0)
  {
    cx0 = Math::min(cx0, INT_MAX + tx);
    cx1 = Math::min(cx1, INT_MAX + tx);
  }
  else if (tx > 0)
  {
    cx0 = Math::max(cx0, INT_MIN + tx);
    cx1 = Math::max(cx1, INT_MIN + tx);
  }

  if (ty < 0)
  {
    cy0 = Math::min(cy0, INT_MAX + ty);
    cy1 = Math::min(cy1, INT_MAX + ty);
  }
  else if (ty > 0)
  {
    cy0 = Math::max(cy0, INT_MIN + ty);
    cy1 = Math::max(cy1, INT_MIN + ty);
  }

  cx0 -= tx;
  cy0 -= ty;

  cx1 -= tx;
  cy1 -= ty;

  if (cx0 >= cx1 || cy0 >= cy1)
  {
    dst->clear();
    return ERR_OK;
  }

  if (d->reference.get() != 1 || d->capacity < length)
  {
    d = _api.region_dCreate(length);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  FOG_ASSERT(d->capacity >= length);

  BoxI* dCur = d->data;
  BoxI* dPrevBand = NULL;
  BoxI* dCurBand;

  BoxI* sCur = src_d->data;
  BoxI* sEnd = sCur + length;

  int dBoundingX0 = INT_MAX;
  int dBoundingX1 = INT_MIN;

  // Skip boxes which do not intersect with the clip-box.
  while (sCur->y1 <= cy0)
  {
    if (++sCur == sEnd)
      goto _End;
  }

  // Do the intersection part.
  for (;;)
  {
    FOG_ASSERT(sCur != sEnd);
    if (sCur->y0 >= cy1)
      break;

    int y0;
    int y1 = 0; // Be quite.
    int bandY0 = sCur->y0;

    dCurBand = dCur;

    // Skip leading boxes which do not intersect with the clip-box.
    while (sCur->x1 <= cx0)
    {
      if (++sCur == sEnd) goto _End;
      if (sCur->y0 != bandY0) goto _Skip;
    }

    // Do the inner part.
    if (sCur->x0 < cx1)
    {
      y0 = Math::max(sCur->y0, cy0) + ty;
      y1 = Math::min(sCur->y1, cy1) + ty;

      // First box.
      FOG_ASSERT(dCur != d->data + d->capacity);
      dCur->setBox(Math::max(sCur->x0, cx0) + tx, y0, Math::min(sCur->x1, cx1) + tx, y1);
      dCur++;

      sCur++;
      if (sCur == sEnd || sCur->y0 != bandY0)
        goto _Merge;

      // Inner boxes.
      while (sCur->x1 <= cx1)
      {
        FOG_ASSERT(dCur != d->data + d->capacity);
        FOG_ASSERT(sCur->x0 >= cx0 && sCur->x1 <= cx1);

        dCur->setBox(sCur->x0 + tx, y0, sCur->x1 + tx, y1);
        dCur++;

        sCur++;
        if (sCur == sEnd || sCur->y0 != bandY0)
          goto _Merge;
      }

      // Last box.
      if (sCur->x0 < cx1)
      {
        FOG_ASSERT(dCur != d->data + d->capacity);
        FOG_ASSERT(sCur->x0 >= cx0);

        dCur->setBox(sCur->x0 + tx, y0, Math::min(sCur->x1, cx1) + tx, y1);
        dCur++;

        sCur++;
        if (sCur == sEnd || sCur->y0 != bandY0)
          goto _Merge;
      }

      FOG_ASSERT(sCur->x0 >= cx1);
    }

    // Skip trailing boxes which do not intersect with the clip-box.
    while (sCur->x0 >= cx1)
    {
      if (++sCur == sEnd) goto _End;
      if (sCur->y0 != bandY0) goto _Merge;
    }

_Merge:
    if (dCurBand != dCur)
    {
      if (dBoundingX0 > dCurBand[0].x0) dBoundingX0 = dCurBand[0].x0;
      if (dBoundingX1 < dCur   [-1].x1) dBoundingX1 = dCur   [-1].x1;

      if (dPrevBand != NULL)
        dCur = Region_coalesce(dCur, &dPrevBand, &dCurBand, y1);
      else
        dPrevBand = dCurBand;
    }

_Skip:
    if (sCur == sEnd)
      break;
  }

_End:
  length = (size_t)(dCur - d->data);
  d->length = length;

  if (length == 0)
    d->boundingBox.reset();
  else
    d->boundingBox.setBox(dBoundingX0, d->data[0].y0, dBoundingX1, d->data[length-1].y1);
  FOG_ASSERT(Region_isValid(d));

  if (d != dst->_d)
    atomicPtrXchg(&dst->_d, d)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Region - IntersectAndClip]
// ============================================================================

static err_t FOG_CDECL Region_intersectAndClip(Region* dst, const Region* a, const Region* b, const BoxI* clipBox)
{
  RegionData* a_d = a->_d;
  RegionData* b_d = b->_d;

  size_t aLength = a_d->length;
  size_t bLength = b_d->length;

  if (aLength == 0 || bLength == 0 || !clipBox->isValid())
  {
    dst->clear();
    return ERR_OK;
  }

  int cx0 = Math::max(clipBox->x0, a_d->boundingBox.x0, b_d->boundingBox.x0);
  int cy0 = Math::max(clipBox->y0, a_d->boundingBox.y0, b_d->boundingBox.y0);
  int cx1 = Math::min(clipBox->x1, a_d->boundingBox.x1, b_d->boundingBox.x1);
  int cy1 = Math::min(clipBox->y1, a_d->boundingBox.y1, b_d->boundingBox.y1);

  if (cx0 >= cx1 || cy0 >= cy1)
  {
    dst->clear();
    return ERR_OK;
  }

  if (aLength == 1 || bLength == 1)
  {
    BoxI newClipBox(cx0, cy0, cx1, cy1);

    if (aLength != 1)
      return Region_clip(dst, a, &newClipBox);
    if (bLength != 1)
      return Region_clip(dst, b, &newClipBox);

    return dst->setBox(newClipBox);
  }

  // --------------------------------------------------------------------------
  // [Destination]
  // --------------------------------------------------------------------------

  RegionData* d = dst->_d;
  bool requireNewData =
    dst == a ||
    dst == b ||
    d->reference.get() != 1;

  size_t dCapacity = d->capacity;

  BoxI* dCur = NULL;
  BoxI* dPrevBand = NULL;
  BoxI* dCurBand = NULL;

  int dBoundingX0 = INT_MAX;
  int dBoundingX1 = INT_MIN;

  // --------------------------------------------------------------------------
  // [Source - A & B]
  // --------------------------------------------------------------------------

  const BoxI* aCur = a_d->data;
  const BoxI* bCur = b_d->data;

  const BoxI* aEnd = aCur + aLength;
  const BoxI* bEnd = bCur + bLength;

  const BoxI* aBandEnd = NULL;
  const BoxI* bBandEnd = NULL;

  int y0, y1;
  size_t estimatedSize;

  // Skip all parts which do not intersect. If there is no intersection
  // detected then this loop can skip into the _Clear label.
  while (aCur->y0 <= cy0) { if (++aCur == aEnd) goto _Clear; }
  while (bCur->y0 <= cy0) { if (++bCur == bEnd) goto _Clear; }

  for (;;)
  {
    bool cont = false;

    while (aCur->y1 <= bCur->y0) { if (++aCur == aEnd) goto _Clear; else cont = true; }
    while (bCur->y1 <= aCur->y0) { if (++bCur == bEnd) goto _Clear; else cont = true; }

    if (cont)
      continue;

    while (aCur->x1 <= cx0 || aCur->x0 >= cx1) { if (++aCur == aEnd) goto _Clear; else cont = true; }
    while (bCur->x1 <= cx0 || bCur->x0 >= cx1) { if (++bCur == bEnd) goto _Clear; else cont = true; }

    if (!cont)
      break;
  }

  if (aCur->y0 >= cy1 || bCur->y0 >= cy1)
  {
_Clear:
    dst->clear();
    return ERR_OK;
  }

  FOG_ASSERT(aCur != aEnd);
  FOG_ASSERT(bCur != bEnd);

  // Update aLength and bLength so the estimated size is closer to the result.
  aLength -= (size_t)(aCur - a_d->data);
  bLength -= (size_t)(bCur - b_d->data);

  // --------------------------------------------------------------------------
  // [Estimated Size]
  // --------------------------------------------------------------------------

  estimatedSize = (aLength + bLength) * 2;
  if (estimatedSize < aLength || estimatedSize < bLength)
    return ERR_RT_OUT_OF_MEMORY;

  // --------------------------------------------------------------------------
  // [Destination]
  // --------------------------------------------------------------------------

  requireNewData |= d->capacity < estimatedSize;
  if (requireNewData)
  {
    d = _api.region_dCreate(estimatedSize);
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  dCur = d->data;

  // --------------------------------------------------------------------------
  // [Intersection]
  // --------------------------------------------------------------------------

  aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
  bBandEnd = RegionUtil::getEndBand(bCur, bEnd);

  for (;;)
  {
    int ym = Math::min(aCur->y1, bCur->y1);
    // Vertical intersection of current A and B bands.
    y0 = Math::max(aCur->y0, bCur->y0, cy0);
    y1 = Math::min(cy1, ym);

    if (y0 < y1)
    {
      const BoxI* aBand = aCur;
      const BoxI* bBand = bCur;

      dCurBand = dCur;

      for (;;)
      {
        // Skip boxes which do not intersect.
        if (aBand->x1 <= bBand->x0) { if (++aBand == aBandEnd) goto _IntersectBandDone; else continue; }
        if (bBand->x1 <= aBand->x0) { if (++bBand == bBandEnd) goto _IntersectBandDone; else continue; }

        // Horizontal intersection of current A and B boxes.
        int x0 = Math::max(aBand->x0, bBand->x0, cx0);
        int xm = Math::min(aBand->x1, bBand->x1);
        int x1 = Math::min(cx1, xm);

        if (x0 < x1)
        {
          FOG_ASSERT(dCur != d->data + d->capacity);
          dCur->setBox(x0, y0, x1, y1);
          dCur++;
        }

        // Advance.
        if (aBand->x1 == xm && (++aBand == aBandEnd || aBand->x0 >= cx1)) break;
        if (bBand->x1 == xm && (++bBand == bBandEnd || bBand->x0 >= cx1)) break;
      }

      // Update bounding box and coalesce.
_IntersectBandDone:
      if (dCurBand != dCur)
      {
        if (dBoundingX0 > dCurBand[0].x0) dBoundingX0 = dCurBand[0].x0;
        if (dBoundingX1 < dCur   [-1].x1) dBoundingX1 = dCur   [-1].x1;

        if (dPrevBand != NULL)
          dCur = Region_coalesce(dCur, &dPrevBand, &dCurBand, y1);
        else
          dPrevBand = dCurBand;
      }
    }

    // Advance A.
    if (aCur->y1 == ym)
    {
      if ((aCur = aBandEnd) == aEnd || aCur->y0 >= cy1)
        break;

      while (aCur->x1 <= cx0 || aCur->x0 >= cx1)
        if (++aCur == aEnd) goto _End;

      aBandEnd = RegionUtil::getEndBand(aCur, aEnd);
    }

    // Advance B.
    if (bCur->y1 == ym)
    {
      if ((bCur = bBandEnd) == bEnd || bCur->y0 >= cy1)
        break;

      while (bCur->x1 <= cx0 || bCur->x0 >= cx1)
        if (++bCur == bEnd) goto _End;

      bBandEnd = RegionUtil::getEndBand(bCur, bEnd);
    }
  }

  // --------------------------------------------------------------------------
  // [End]
  // --------------------------------------------------------------------------

_End:
  d->length = (size_t)(dCur - d->data);

  if (d->length == 0)
    d->boundingBox.reset();
  else
    d->boundingBox.setBox(dBoundingX0, d->data[0].y0, dBoundingX1, dCur[-1].y1);

  FOG_ASSERT(Region_isValid(d));
  d = atomicPtrXchg(&dst->_d, d);

  if (requireNewData)
    d->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Region - HitTest]
// ============================================================================

static uint32_t FOG_CDECL Region_hitTestPoint(const Region* self, const PointI* pt)
{
  RegionData* d = self->_d;

  int x = pt->x;
  int y = pt->y;

  // Check whether the point is in the region bounding-box.
  if (!(d->boundingBox.hitTest(x, y)))
    return REGION_HIT_OUT;

  // If the region is a rectangle, then we are finished.
  size_t length = d->length;
  if (length == 1)
    return REGION_HIT_IN;

  // If the region is small enough then do a naive search.
  const BoxI* data = d->data;
  const BoxI* end = data + length;

  // Find the first interesting rectangle.
  if (length > 64)
  {
    data = RegionUtil::getClosestBox(data, length, y);
    if (data == NULL)
      return REGION_HIT_OUT;
  }
  else
  {
    while (y >= data->y1)
    {
      if (++data == end)
        return REGION_HIT_OUT;
    }
  }

  FOG_ASSERT(data != end);
  if (y < data->y0)
    return REGION_HIT_OUT;

  do {
    if (x >= data->x0 && x < data->x1)
      return REGION_HIT_IN;
    data++;
  } while (data != end && y >= data->y0);

  return REGION_HIT_OUT;
}

static uint32_t FOG_CDECL Region_hitTestBox(const Region* self, const BoxI* box)
{
  if (!box->isValid())
    return REGION_HIT_OUT;

  RegionData* d = self->_d;
  if (d == &Region_dInfinite)
    return REGION_HIT_IN;

  size_t length = d->length;
  if (!length || !d->boundingBox.overlaps(*box))
    return REGION_HIT_OUT;

  const BoxI* data = d->data;
  const BoxI* end = data + length;

  int bx0 = box->x0;
  int by0 = box->y0;
  int bx1 = box->x1;
  int by1 = box->y1;

  // Find the first box in the region which can be compared with the given box.
  // If the region is really complex (more than 64 rectangles) then use the
  // RegionUtil::getClosestBox() which uses bsearch algorithm instead of naive
  // searching.
  if (length > 64)
  {
    data = RegionUtil::getClosestBox(data, length, by0);
    if (data == NULL)
      return REGION_HIT_OUT;
  }
  else
  {
    while (by0 >= data->y1)
    {
      if (++data == end)
        return REGION_HIT_OUT;
    }
  }

  // Now we have the start band. If the band.y0 position is greater than
  // the by1 then we know that a given box is completely outside of the
  // region.
  FOG_ASSERT(data != end);
  if (by1 <= data->y0)
    return REGION_HIT_OUT;

  // There are two main loops, each is optimized for one special case. The
  // first loop is used in case that we know that a given box can only partially
  // hit the region. The algorithm is optimized to find whether there is any
  // intersection (hit). The second loop is optimized to check whether the
  // given box completely covers the region.

  int bandY0 = data->y0;

  // --------------------------------------------------------------------------
  // [Partially or Completely Outside]
  // --------------------------------------------------------------------------

  if (d->boundingBox.x0 > bx0 ||
      bandY0            > by0 ||
      d->boundingBox.x1 < bx1 ||
      d->boundingBox.y1 < by1)
  {
    if (bandY0 >= by1)
      return REGION_HIT_OUT;

    for (;;)
    {
      // Skip leading boxes which do not intersect.
      while (data->x1 <= bx0)
      {
        if (++data == end)
          return REGION_HIT_OUT;
        if (data->y0 != bandY0)
          goto _PartNextBand;
      }

      // If there is an intersection then we are done.
      if (data->x0 < bx1)
        return REGION_HIT_PART;

      // Skip remaining boxes.
      for (;;)
      {
        if (++data == end)
          return REGION_HIT_OUT;
        if (data->y0 != bandY0)
          break;
      }

_PartNextBand:
      bandY0 = data->y0;
      if (bandY0 >= by1)
        return REGION_HIT_OUT;
    }
  }

  // --------------------------------------------------------------------------
  // [Partially or Completely Inside]
  // --------------------------------------------------------------------------

  else
  {
    uint32_t result = REGION_HIT_OUT;

    for (;;)
    {
      // Skip leading boxes which do not intersect.
      while (data->x1 <= bx0)
      {
        if (++data == end)
          return result;
        if (data->y0 != bandY0)
          goto _CompNoIntersection;
      }

      // The intersection (if any) must cover the entire bx0...bx1. If the
      // condition is not true or there is no intersection at all, then we
      // jump into the first loop, because we know that the given box doesn't
      // cover the entire region.
      if (data->x0 > bx0 || data->x1 < bx1)
      {
_CompNoIntersection:
        if (result != REGION_HIT_OUT)
          return result;
        else
          goto _PartNextBand;
      }

      // From now we know that the box is partially in the region, but we are
      // not sure whether the hit test is only partial or complete.
      result = REGION_HIT_PART;

      // Skip remaining boxes.
      for (;;)
      {
        if (++data == end)
          return result;
        if (data->y0 != bandY0)
          break;
      }

      // If this was the last interesting band, then we can safely return.
      int previousY1 = data[-1].y1;
      if (previousY1 >= by1)
        return REGION_HIT_IN;

      bandY0 = data->y0;
      if (previousY1 != bandY0 || bandY0 >= by1)
        return REGION_HIT_PART;
    }
  }
}

static uint32_t FOG_CDECL Region_hitTestRect(const Region* self, const RectI* rect)
{
  if (!rect->isValid())
    return REGION_HIT_OUT;

  BoxI box(*rect);
  return _api.region_hitTestBox(self, &box);
}

// ============================================================================
// [Fog::Region - Equality]
// ============================================================================

static bool FOG_CDECL Region_eq(const Region* a, const Region* b)
{
  const RegionData* a_d = a->_d;
  const RegionData* b_d = b->_d;
  size_t length = a_d->length;

  if (a_d == b_d)
    return true;

  if (length != b_d->length || a_d->boundingBox != b_d->boundingBox)
    return false;

  const BoxI* a_data = a_d->data;
  const BoxI* b_data = b_d->data;

  for (size_t i = 0; i < length; i++)
  {
    if (a_data[i] != b_data[i])
      return false;
  }

  return true;
}

// ============================================================================
// [Fog::Region - RegionData]
// ============================================================================

static RegionData* FOG_CDECL Region_dCreate(size_t capacity)
{
  if (FOG_UNLIKELY(capacity == 0))
    return Region_dEmpty->addRef();

  size_t dSize = RegionData::getSizeOf(capacity);

  RegionData* d = (RegionData*)MemMgr::alloc(dSize);
  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = VAR_TYPE_REGION | VAR_FLAG_NONE;
  FOG_PADDING_ZERO_64(d->padding0_32);

  d->capacity = capacity;
  d->length = 0;
  d->boundingBox.reset();

  return d;
}

static RegionData* FOG_CDECL Region_dCreateBox(size_t capacity, const BoxI* box)
{
  RegionData* d = _api.region_dCreate(capacity);
  if (FOG_IS_NULL(d))
    return NULL;

  d->length = 1;
  d->boundingBox = *box;
  d->data[0] = *box;

  return d;
}

static RegionData* FOG_CDECL Region_dCreateRegion(size_t capacity, const BoxI* data, size_t length, const BoxI* bbox)
{
  if (FOG_UNLIKELY(capacity < length))
    capacity = length;

  RegionData* d = _api.region_dCreate(capacity);
  if (FOG_IS_NULL(d))
    return NULL;

  d->length = length;
  if (length == 0)
    goto _Zero;

  if (bbox == NULL)
    goto _NoBBox;

  d->boundingBox = *bbox;
  Region_copyData(d->data, data, length);
  return d;

_Zero:
  d->boundingBox.reset();
  d->data[0].reset();
  return d;

_NoBBox:
  Region_copyDataEx(d->data, data, length, &d->boundingBox);
  return d;
}

static RegionData* FOG_CDECL Region_dAdopt(void* address, size_t capacity)
{
  RegionData* d = (RegionData*)address;

  d->reference.init(1);
  d->vType = VAR_TYPE_REGION | VAR_FLAG_STATIC;
  FOG_PADDING_ZERO_64(d->padding0_32);

  d->capacity = capacity;
  d->length = 0;
  d->boundingBox.reset();

  return d;
}

static RegionData* FOG_CDECL Region_dAdoptBox(void* address, size_t capacity, const BoxI* box)
{
  if (!box->isValid() || capacity == 0)
    return _api.region_dAdopt(address, capacity);

  RegionData* d = (RegionData*)address;

  d->reference.init(1);
  d->vType = VAR_TYPE_REGION | VAR_FLAG_STATIC;
  FOG_PADDING_ZERO_64(d->padding0_32);

  d->capacity = capacity;
  d->length = 1;

  d->boundingBox = *box;
  d->data[0] = *box;

  return d;
}

static RegionData* FOG_CDECL Region_dAdoptRegion(void* address, size_t capacity, const BoxI* data, size_t length, const BoxI* bbox)
{
  if (capacity < length)
    return _api.region_dCreateRegion(length, data, length, bbox);

  RegionData* d = (RegionData*)address;

  d->reference.init(1);
  d->vType = VAR_TYPE_REGION | VAR_FLAG_STATIC;
  FOG_PADDING_ZERO_64(d->padding0_32);

  d->capacity = capacity;
  d->length = length;

  if (bbox == NULL)
    goto _NoBBox;

  d->boundingBox = *bbox;
  Region_copyData(d->data, data, length);
  return d;

_NoBBox:
  Region_copyDataEx(d->data, data, length, &d->boundingBox);
  return d;
}

static RegionData* FOG_CDECL Region_dRealloc(RegionData* d, size_t capacity)
{
  if ((d->vType & VAR_FLAG_STATIC) == 0)
  {
    d = (RegionData*)MemMgr::realloc(d, RegionData::getSizeOf(capacity));
    if (FOG_IS_NULL(d)) return NULL;

    d->capacity = capacity;
    return d;
  }
  else
  {
    RegionData* newd = _api.region_dCreateRegion(capacity, d->data, d->length, &d->boundingBox);
    if (FOG_IS_NULL(newd)) return newd;

    d->release();
    return newd;
  }
}

static RegionData* FOG_CDECL Region_dCopy(const RegionData* d)
{
  size_t length = d->length;

  if (!length)
    return Region_dEmpty->addRef();

  RegionData* newd = _api.region_dCreate(length);
  if (FOG_IS_NULL(newd)) return NULL;

  newd->length = length;
  newd->boundingBox = d->boundingBox;
  Region_copyData(newd->data, d->data, d->length);

  return newd;
}

static void FOG_CDECL Region_dFree(RegionData* d)
{
  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

// ============================================================================
// [Fog::Region - Windows Support]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static FOG_INLINE void RECTFromBoxI(RECT* dst, const BoxI* src)
{
  int left   = src->x0;
  int top    = src->y0;
  int right  = src->x1;
  int bottom = src->y1;

  dst->left   = left  ;
  dst->top    = top   ;
  dst->right  = right ;
  dst->bottom = bottom;
}

static FOG_INLINE void BoxIFromRECT(BoxI* dst, const RECT* src)
{
  int x0 = src->left;
  int y0 = src->top;
  int x1 = src->right;
  int y1 = src->bottom;

  dst->x0 = x0;
  dst->y0 = y0;
  dst->x1 = x1;
  dst->y1 = y1;
}

static err_t FOG_CDECL Region_hrgnFromRegion(HRGN* dst, const Region* src)
{
  RegionData* d = src->_d;
  size_t length = d->length;

  if (length >= (UINT_MAX - sizeof(RGNDATAHEADER)) / sizeof(RECT))
  {
    *dst = (HRGN)NULLREGION;
    return ERR_RT_OUT_OF_MEMORY;
  }

  DWORD hrgnDataSize = (DWORD)length * sizeof(RECT);

  MemBufferTmp<512> buffer;
  RGNDATAHEADER *hRegionHeader = reinterpret_cast<RGNDATAHEADER *>(buffer.alloc(sizeof(RGNDATAHEADER) + hrgnDataSize));

  if (FOG_IS_NULL(hRegionHeader))
  {
    *dst = (HRGN)NULLREGION;
    return ERR_RT_OUT_OF_MEMORY;
  }

  hRegionHeader->dwSize = sizeof(RGNDATAHEADER);
  hRegionHeader->iType = RDH_RECTANGLES;
  hRegionHeader->nCount = (DWORD)length;
  hRegionHeader->nRgnSize = hrgnDataSize;

  // Copy rectangles to the region.
  {
    RECT* dstData = reinterpret_cast<RECT *>((uint8_t *)hRegionHeader + sizeof(RGNDATAHEADER));
    const BoxI* srcData = d->data;

    for (size_t i = 0; i < length; i++)
    {
      RECTFromBoxI(&dstData[i], &srcData[i]);
    }
    RECTFromBoxI(&hRegionHeader->rcBound, &d->boundingBox);
  }

  HRGN hRegion = ExtCreateRegion(NULL, (DWORD)sizeof(RGNDATAHEADER) + hrgnDataSize, (RGNDATA *)hRegionHeader);
  if (FOG_IS_NULL(hRegion))
  {
    *dst = (HRGN)NULLREGION;
    return ERR_OK;
  }

  *dst = hRegion;
  return ERR_OK;
}

static err_t FOG_CDECL Region_regionFromHRGN(Region* dst, HRGN src)
{
  dst->clear();

  if (src == NULL || src == (HRGN)NULLREGION)
    return ERR_OK;

  DWORD hrgnDataSize = GetRegionData(src, 0, NULL);
  if (hrgnDataSize == 0)
    return ERR_OK;

  MemBufferTmp<512> buffer;
  RGNDATAHEADER* hRegionHeader = reinterpret_cast<RGNDATAHEADER *>(buffer.alloc(hrgnDataSize));

  if (FOG_IS_NULL(hRegionHeader))
    return ERR_RT_OUT_OF_MEMORY;

  if (!GetRegionData(src, hrgnDataSize, (RGNDATA*)hRegionHeader))
    return OSUtil::getErrFromOSLastError();

  FOG_ASSERT(sizeof(RECT) == sizeof(BoxI));

  RECT* srcRect = (RECT*)((uint8_t*)hRegionHeader + sizeof(RGNDATAHEADER));
  BoxI* dstBox = reinterpret_cast<BoxI*>(srcRect);

  size_t length = hRegionHeader->nCount;

  if (FOG_OFFSET_OF(RECT, left  ) != FOG_OFFSET_OF(BoxI, x0) ||
      FOG_OFFSET_OF(RECT, top   ) != FOG_OFFSET_OF(BoxI, y0) ||
      FOG_OFFSET_OF(RECT, right ) != FOG_OFFSET_OF(BoxI, x1) ||
      FOG_OFFSET_OF(RECT, bottom) != FOG_OFFSET_OF(BoxI, y1))
  {
    for (size_t i = 0; i < length; i++)
      BoxIFromRECT(&dstBox[i], &srcRect[i]);
  }

  return dst->setBoxList(dstBox, length);
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_CPU_DECLARE_INITIALIZER_SSE2(Region_init_SSE2)

FOG_NO_EXPORT void Region_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.region_ctor = Region_ctor;
  _api.region_ctorRegion = Region_ctorRegion;
  _api.region_ctorBox = Region_ctorBox;
  _api.region_ctorRect = Region_ctorRect;
  _api.region_dtor = Region_dtor;
  _api.region_detach = Region_detach;
  _api.region_reserve = Region_reserve;
  _api.region_squeeze = Region_squeeze;
  _api.region_prepare = Region_prepare;
  _api.region_getType = Region_getType;
  _api.region_clear = Region_clear;
  _api.region_reset = Region_reset;
  _api.region_setRegion = Region_setRegion;
  _api.region_setDeep = Region_setDeep;
  _api.region_setBox = Region_setBox;
  _api.region_setRect = Region_setRect;
  _api.region_setBoxList = Region_setBoxList;
  _api.region_setRectList = Region_setRectList;
  _api.region_combineRegionRegion = Region_combineRegionRegion;
  _api.region_combineRegionBox = Region_combineRegionBox;
  _api.region_combineBoxRegion = Region_combineBoxRegion;
  _api.region_combineBoxBox = Region_combineBoxBox;
  _api.region_translate = Region_translate;
  _api.region_translateAndClip = Region_translateAndClip;
  _api.region_intersectAndClip = Region_intersectAndClip;
  _api.region_hitTestPoint = Region_hitTestPoint;
  _api.region_hitTestBox = Region_hitTestBox;
  _api.region_hitTestRect = Region_hitTestRect;
  _api.region_eq = Region_eq;

#if defined(FOG_OS_WINDOWS)
  _api.region_hrgnFromRegion = Region_hrgnFromRegion;
  _api.region_regionFromHRGN = Region_regionFromHRGN;
#endif // FOG_OS_WINDOWS

  _api.region_dCreate = Region_dCreate;
  _api.region_dCreateBox = Region_dCreateBox;
  _api.region_dCreateRegion = Region_dCreateRegion;
  _api.region_dAdopt = Region_dAdopt;
  _api.region_dAdoptBox = Region_dAdoptBox;
  _api.region_dAdoptRegion = Region_dAdoptRegion;
  _api.region_dRealloc = Region_dRealloc;
  _api.region_dCopy = Region_dCopy;
  _api.region_dFree = Region_dFree;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  RegionData* d;

  d = &Region_dEmpty;
  d->reference.init(1);

  _api.region_oEmpty = Region_oEmpty.initCustom1(d);

  d = &Region_dInfinite;
  d->reference.init(1);
  d->vType = VAR_TYPE_REGION | VAR_FLAG_NONE;

  d->capacity = 1;
  d->length = 1;
  d->boundingBox.setBox(INT_MIN, INT_MIN, INT_MAX, INT_MAX);
  d->data[0].setBox(INT_MIN, INT_MIN, INT_MAX, INT_MAX);

  _api.region_oInfinite = Region_oInfinite.initCustom1(d);

  // --------------------------------------------------------------------------
  // [CPU Based Optimizations]
  // --------------------------------------------------------------------------

  FOG_CPU_USE_INITIALIZER_SSE2(Region_init_SSE2)
}

} // Fog namespace
