// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/PBuffer.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryBuffer.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Global/Init_p.h>
#include <Fog/G2d/Rasterizer/Rasterizer_p.h>
#include <Fog/G2d/Tools/Region.h>

/************************************************************************

Copyright (c) 1987  x Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
x CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the x Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the x Consortium.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

************************************************************************/

namespace Fog {

// ============================================================================
// [Fog::Region]
// ============================================================================

#define REGION_STACK_SIZE 64

// ============================================================================
// [Fog::Region - Helpers]
// ============================================================================

static FOG_INLINE void _copyRects(BoxI* dest, const BoxI* src, sysuint_t length)
{
  for (sysuint_t i = length; i; i--, dest++, src++) *dest = *src;
}

static void _copyRectsExtents(BoxI* dest, const BoxI* src, sysuint_t length, BoxI* extents)
{
  int extentsX0 = src[0].x0;
  int extentsY0 = src[0].y0;
  int extentsX1 = src[length-1].x1;
  int extentsY1 = src[length-1].y1;

  for (sysuint_t i = length; i; i--, dest++, src++) 
  {
    if (extentsX0 > src->x0) extentsX0 = src->x0;
    if (extentsX1 < src->x1) extentsX1 = src->x1;
    *dest = *src;
  }

  extents->set(extentsX0, extentsY0, extentsX1, extentsY1);
}

// ============================================================================
// [Fog::RegionData]
// ============================================================================

RegionData* RegionData::ref() const
{
  if (flags & IsSharable)
  {
    refCount.inc();
    return const_cast<RegionData*>(this);
  }
  else
  {
    return RegionData::copy(this);
  }
}

void RegionData::deref()
{
  if (refCount.deref() && (flags & IsDynamic) != 0) Memory::free(this);
}

static RegionData* _reallocRegion(RegionData* d, sysuint_t capacity)
{
  if ((d->flags & RegionData::IsDynamic) != 0)
  {
    d = (RegionData*)Memory::realloc(d, RegionData::sizeFor(capacity));
    if (FOG_IS_NULL(d)) return NULL;

    d->capacity = capacity;
    return d;
  }
  else
  {
    return RegionData::create(capacity, &d->extents, d->rects, d->length);
  }
}

RegionData* RegionData::adopt(void* address, sysuint_t capacity)
{
  RegionData* d = (RegionData*)address;
  
  d->refCount.init(1);
  d->flags = IsStrong;
  d->capacity = capacity;
  d->length = 0;
  d->extents.reset();

  return d;
}

RegionData* RegionData::adopt(void* address, sysuint_t capacity, const BoxI& r)
{
  if (!r.isValid() || capacity == 0) return adopt(address, capacity);

  RegionData* d = (RegionData*)address;
  
  d->refCount.init(1);
  d->flags = IsStrong;
  d->capacity = capacity;
  if (r.isValid())
  {
    d->length = 1;
    d->extents = r;
    d->rects[0] = r;
  }
  else
  {
    d->length = 0;
    d->extents.reset();
    d->rects[0].reset();
  }

  return d;
}

RegionData* RegionData::adopt(void* address, sysuint_t capacity, const BoxI* extents, const BoxI* rects, sysuint_t length)
{
  if (capacity < length) create(length, extents, rects, length);

  RegionData* d = (RegionData*)address;
  
  d->refCount.init(1);
  d->flags = IsStrong;
  d->capacity = capacity;
  d->length = length;
  
  if (extents)
  {
    d->extents = *extents;
    _copyRects(d->rects, rects, length);
  }
  else
  {
    _copyRectsExtents(d->rects, rects, length, &d->extents);
  }

  return d;
}

RegionData* RegionData::create(sysuint_t capacity)
{
  if (FOG_UNLIKELY(capacity == 0)) return Region::_dnull->refAlways();

  sysuint_t dsize = sizeFor(capacity);
  RegionData* d = (RegionData*)Memory::alloc(dsize);
  if (FOG_IS_NULL(d)) return NULL;

  d->refCount.init(1);
  d->flags = IsDynamic | IsSharable;
  d->capacity = capacity;
  d->length = 0;

  return d;
}

RegionData* RegionData::create(sysuint_t capacity, const BoxI* extents, const BoxI* rects, sysuint_t length)
{
  if (FOG_UNLIKELY(capacity < length)) capacity = length;

  RegionData* d = create(capacity);
  if (FOG_IS_NULL(d)) return NULL;

  if (FOG_LIKELY(length))
  {
    d->length = length;
    if (extents)
    {
      d->extents = *extents;
      _copyRects(d->rects, rects, length);
    }
    else
    {
      _copyRectsExtents(d->rects, rects, length, &d->extents);
    }
  }

  return d;
}

RegionData* RegionData::copy(const RegionData* other)
{
  if (!other->length) return Region::_dnull->refAlways();

  RegionData* d = create(other->length);
  if (FOG_IS_NULL(d)) return NULL;

  d->length = other->length;
  d->extents = other->extents;
  _copyRects(d->rects, other->rects, other->length);

  return d;
}

// ============================================================================
// [Fog::Region - Statics]
// ============================================================================

// Utility procedure _compress:
//
// Replace r by the region r', where
//   p in r' iff (Quantifer m <= dx) (p + m in r), and
//   quantifier is exists if grow is true, for all if grow is false, and
//   (x,y) + m = (x+m,y) if xdir is true; (x,y+m) if xdir is false.
//
// Thus, if xdir is true and grow is false, r is replaced by the region
// of all points p such that p and the next dx points on the same
// horizontal scan line are all in r.  We do this using by noting
// that p is the head of a run of length 2^i + k iff p is the head
// of a run of length 2^i and p+2^i is the head of a run of length
// k. Thus, the loop invariant: s contains the region corresponding
// to the runs of length shift. r contains the region corresponding
// to the runs of length 1 + dxo & (shift-1), where dxo is the original
// value of dx.  dx = dxo & ~(shift-1).  As parameters, s and t are
// scratch regions, so that we don't have to allocate them on every
// call.
static err_t _compress(Region& r, Region& s, Region& t, uint dx, bool xdir, bool grow)
{
  uint shift = 1;

  FOG_RETURN_ON_ERROR(s.setDeep(r));

  while (dx)
  {
    if (dx & shift)
    {
      FOG_RETURN_ON_ERROR(r.translate(xdir ? PointI(-(int)shift, 0) : PointI(0, -(int)shift)));
      FOG_RETURN_ON_ERROR(r.combine(s, grow ? REGION_OP_UNION : REGION_OP_INTERSECT));

      dx -= shift;
      if (!dx) break;
    }

    FOG_RETURN_ON_ERROR(t.setDeep(s));
    FOG_RETURN_ON_ERROR(s.translate(xdir ? PointI(-(int)shift, 0) : PointI(0, -(int)shift)));
    FOG_RETURN_ON_ERROR(s.combine(t, grow ? REGION_OP_UNION : REGION_OP_INTERSECT));

    shift <<= 1;
  }

  return ERR_OK;
}

// Coalesce:
//   Attempt to merge the boxes in the current band with those in the
//   previous one. Used only by miRegionOp.
//
// Results:
//   dest pointer (can be smaller if coalesced)
//
// Side Effects:
//   If coalescing takes place:
//   - rectangles in the previous band will have their y1 fields
//     altered.
//   - Count may be decreased.
static BoxI* _coalesceHelper(BoxI* dest_ptr, BoxI** prev_start_, BoxI** cur_start_)
{
  BoxI* prev_start = *prev_start_;
  BoxI* cur_start  = *cur_start_;

  sysuint_t c1 = (sysuint_t)( cur_start - prev_start );
  sysuint_t c2 = (sysuint_t)( dest_ptr - cur_start );

  if (c1 == c2 && prev_start->y1 == cur_start->y0)
  {
    // Need to scan their x coords, if prev_start and cur_start will has
    // same x coordinates, we can coalesce it.
    sysuint_t i;
    for (i = 0; i != c1; i++)
    {
      if (prev_start[i].x0 != cur_start[i].x0 || prev_start[i].x1 != cur_start[i].x1)
      {
        // Coalesce isn't possible.
        *prev_start_ = cur_start;
        return dest_ptr;
      }
    }

    // Coalesce.
    int bot = cur_start->y1;
    for (i = 0; i != c1; i++) prev_start[i].setY1(bot);
    return cur_start;
  }

  *prev_start_ = cur_start;
  return dest_ptr;
}

// Inline here produces better results.
static FOG_INLINE BoxI* _coalesce(BoxI* dest_ptr, BoxI** prev_start_, BoxI** cur_start_)
{
  if (*prev_start_ != *cur_start_)
  {
    return _coalesceHelper(dest_ptr, prev_start_, cur_start_);
  }
  else
    return dest_ptr;
}

// Forward declarations for rectangle processor.
static err_t _unitePrivate(Region* dest, const BoxI* src1, sysuint_t count1, const BoxI* src2, sysuint_t count2, bool memOverlap, const BoxI* new_extents);
static err_t _intersectPrivate(Region* dest, const BoxI* src1, sysuint_t count1, const BoxI* src2, sysuint_t count2, bool memOverlap);
static err_t _subtractPrivate(Region* dest, const BoxI* src1, sysuint_t count1, const BoxI* src2, sysuint_t count2, bool memOverlap);
static err_t _appendPrivate(Region* dest, const BoxI* src, sysuint_t length, const BoxI* new_extents);

// Fog::RegionData statics
static RegionData* _reallocRegion(RegionData* d, sysuint_t capacity);

// ============================================================================
// [Fog::Region - Union, Subtraction, Intersection and Append Implementation]
// ============================================================================

static err_t _unitePrivate(Region* dest, const BoxI* src1, sysuint_t count1, const BoxI* src2, sysuint_t count2, bool memOverlap, const BoxI* new_extents)
{
  BoxI* destBegin;                      // Destination begin.
  BoxI* destCur;                        // Destination ptr.

  const BoxI* src1End = src1 + count1;  // End of src1.
  const BoxI* src2End = src2 + count2;  // End of src2.

  const BoxI* src1BandEnd;              // End of current band in src1.
  const BoxI* src2BandEnd;              // End of current band in src2.

  int top;                             // Top of non-overlapping band.
  int bot;                             // Bottom of non-overlapping band.

  int ytop;                            // Top of intersection.
  int ybot;                            // Bottom of intersection.

  BoxI* prevBand;                       // Pointer to start of previous band.
  BoxI*  curBand;                       // Pointer to start of current band.

  sysuint_t minRectsNeeded = (count1 + count2) * 2;

  // Trivial reject.
  if (src1 == src1End) return dest->set(*src2);
  if (src2 == src2End) return dest->set(*src1);

  // New region data in case that it needs it.
  RegionData* newd = NULL;
  sysuint_t length;

  // Local buffer that can be used instead of malloc in most calls
  // can be increased to higher values, but I think that 32 is ok.
  BoxI staticBuffer[32];

  if (memOverlap)
  {
    // Need to allocate new block.
    if (minRectsNeeded < FOG_ARRAY_SIZE(staticBuffer))
    {
      destCur = staticBuffer;
    }
    else
    {
      newd = RegionData::create(minRectsNeeded);
      if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;
      destCur = newd->rects;
    }
  }
  else
  {
    // Can use dest.
    err_t err;
    if ((err = dest->prepare(minRectsNeeded))) { dest->clear(); return err; }
    destCur = dest->_d->rects;
  }

  destBegin = destCur;
  
  // Initialize ybot and ytop.
  // In the upcoming loop, ybot and ytop serve different functions depending
  // on whether the band being handled is an overlapping or non-overlapping
  // band.
  // In the case of a non-overlapping band (only one of the regions
  // has points in the band), ybot is the bottom of the most recent
  // intersection and thus clips the top of the rectangles in that band.
  // ytop is the top of the next intersection between the two regions and
  // serves to clip the bottom of the rectangles in the current band.
  // For an overlapping band (where the two regions intersect), ytop clips
  // the top of the rectangles of both regions and ybot clips the bottoms.

  if (src1->y0 < src2->y0)
    ybot = src1->y0;
  else
    ybot = src2->y0;

  // prevBand serves to mark the start of the previous band so rectangles
  // can be coalesced into larger rectangles. qv. coalesce, above.
  // In the beginning, there is no previous band, so prevBand == curBand
  // (curBand is set later on, of course, but the first band will always
  // start at index 0). prevBand and curBand must be indices because of
  // the possible expansion, and resultant moving, of the new region's
  // array of rectangles.
  prevBand = destCur;

  do {
    curBand = destCur;
    // This algorithm proceeds one source-band (as opposed to a
    // destination band, which is determined by where the two regions
    // intersect) at a time. src1BandEnd and src2BandEnd serve to mark the
    // rectangle after the last one in the current band for their
    // respective regions.
    src1BandEnd = src1;
    src2BandEnd = src2;

    while ((++src1BandEnd != src1End) && (src1BandEnd->y0 == src1->y0)) ;
    while ((++src2BandEnd != src2End) && (src2BandEnd->y0 == src2->y0)) ;

    // First handle the band that doesn't intersect, if any.
    //
    // Note that attention is restricted to one band in the
    // non-intersecting region at once, so if a region has n
    // bands between the current position and the next place it overlaps
    // the other, this entire loop will be passed through n times.
    if (src1->y0 < src2->y0)
    {
      top = Math::max(src1->y0, ybot);
      bot = Math::min(src1->y1, src2->y0);

      if (top != bot)
      {
        const BoxI* ovrlp = src1;
        while (ovrlp != src1BandEnd) { (*destCur++).set(ovrlp->x0, top, ovrlp->x1, bot); ovrlp++; }

        destCur = _coalesce(destCur, &prevBand, &curBand);
      }
    }
    else if (src2->y0 < src1->y0)
    {
      top = Math::max(src2->y0, ybot);
      bot = Math::min(src2->y1, src1->y0);

      if (top != bot)
      {
        const BoxI* ovrlp = src2;
        while (ovrlp != src2BandEnd) { (*destCur++).set(ovrlp->x0, top, ovrlp->x1, bot); ovrlp++; }

        destCur = _coalesce(destCur, &prevBand, &curBand);
      }
    }

    // Now see if we've hit an intersecting band. The two bands only
    // intersect if ybot > ytop.
    ytop = Math::max(src1->y0, src2->y0);
    ybot = Math::min(src1->y1, src2->y1);
    if (ybot > ytop)
    {
      const BoxI* i1 = src1;
      const BoxI* i2 = src2;

      // Unite.
      #define MERGE_RECT(__x0__, __y0__, __x1__, __y1__)            \
        if (destCur != destBegin &&                                 \
         (destCur[-1].y0 == __y0__) &&                              \
         (destCur[-1].y1 == __y1__) &&                              \
         (destCur[-1].x1 >= __x0__))                                \
        {                                                           \
          if (destCur[-1].x1 < __x1__) destCur[-1].x1 = __x1__;     \
        }                                                           \
        else {                                                      \
          (*destCur++).set(__x0__, __y0__, __x1__, __y1__);         \
        }

      while ((i1 != src1BandEnd) && (i2 != src2BandEnd))
      {
        if (i1->x0 < i2->x0)
        {
          MERGE_RECT(i1->x0, ytop, i1->x1, ybot); i1++;
        }
        else
        {
          MERGE_RECT(i2->x0, ytop, i2->x1, ybot); i2++;
        }
      }

      if (i1 != src1BandEnd)
      {
        do {
          MERGE_RECT(i1->x0, ytop, i1->x1, ybot); i1++;
        } while (i1 != src1BandEnd);
      }
      else
      {
        while (i2 != src2BandEnd)
        {
          MERGE_RECT(i2->x0, ytop, i2->x1, ybot); i2++;
        }
      }

      #undef MERGE_RECT

      destCur = _coalesce(destCur, &prevBand, &curBand);
    }

    // If we've finished with a band (y1 == ybot) we skip forward
    // in the region to the next band.
    if (src1->y1 == ybot) src1 = src1BandEnd;
    if (src2->y1 == ybot) src2 = src2BandEnd;
  } while ((src1 != src1End) && (src2 != src2End));

  // Deal with whichever region still has rectangles left.
  if (src1 != src1End || src2 != src2End)
  {
    const BoxI* src;
    const BoxI* srcEnd;

    curBand = destCur;

    if (src1 != src1End)
    {
      src = src1; srcEnd = src1End;
    }
    else
    {
      src = src2; srcEnd = src2End;
    }

    int y0 = src->y0;
    int y1 = Math::max(src->y0, ybot);

    // Append first band and coalesce.
    while (src != srcEnd && src->y0 == y0) { (*destCur++).set(src->x0, y1, src->x1, src->y1); src++; }
    destCur = _coalesce(destCur, &prevBand, &curBand);

    // Append remaining rectangles, coalesce isn't needed.
    while (src != srcEnd) *destCur++ = *src++;
  }

  // Finished, we have complete intersected region in destCur.
  //
  // Its paranoid here, because unite_private is called only if there are
  // rectangles in regions.
  if ((length = (sysuint_t)(destCur - destBegin)) != 0)
  {
    if (memOverlap)
    {
      if (destBegin == staticBuffer)
      {
        err_t err;
        if ((err = dest->reserve(length))) { dest->clear(); return err; }
        _copyRects(dest->_d->rects, destBegin, length);
      }
      else
      {
        dest->_d->deref();
        dest->_d = newd;
      }
    }

    dest->_d->extents = *new_extents;
    dest->_d->length = length;
  }
  else
  {
    if (memOverlap && destBegin == staticBuffer) Memory::free(newd);
    dest->clear();
  }
  return ERR_OK;
}

static err_t _intersectPrivate(Region* dest, const BoxI* src1, sysuint_t count1, const BoxI* src2, sysuint_t count2, bool memOverlap)
{
  BoxI* destBegin;                      // Destination begin.
  BoxI* destCur;                        // Destination ptr.

  const BoxI* src1End = src1 + count1;  // End of src1.
  const BoxI* src2End = src2 + count2;  // End of src2.

  const BoxI* src1BandEnd;              // End of current band in src1.
  const BoxI* src2BandEnd;              // End of current band in src2.

  int ytop;                             // Top of intersection.
  int ybot;                             // Bottom of intersection.

  int extentsX0 = INT_MAX;              // Extents x0 coord (computed in loop).
  int extentsX1 = INT_MIN;              // Extents x1 coord (computed in loop).

  BoxI* prevBand;                       // Pointer to start of previous band.
  BoxI* curBand;                        // Pointer to start of current band.

  // Simplest case, if there is only 1 rect in each -> rects overlap.
  if (count1 == 1 && count2 == 1)
  {
    return dest->set(BoxI(Math::max(src1->x0, src2->x0), Math::max(src1->y0, src2->y0),
                          Math::min(src1->x1, src2->x1), Math::min(src1->y1, src2->y1)));
  }

  sysuint_t minRectsNeeded = (count1 + count2) * 2;

  // Find first rectangle that can intersect.
  for (;;)
  {
    if (src1 == src1End || src2 == src2End) { dest->clear(); return ERR_OK; }

    if (src1->y1 < src2->y0) { src1++; continue; }
    if (src2->y1 < src1->y0) { src2++; continue; }

    break;
  }

  // New region data in case that it needs it.
  RegionData* newd = NULL;
  sysuint_t length;

  // Local buffer that can be used instead of malloc in most calls,
  // can be increased to higher value, but I think that 32 is ok.
  BoxI staticBuffer[32];

  if (memOverlap)
  {
    // Need to allocate new block.
    if (minRectsNeeded < FOG_ARRAY_SIZE(staticBuffer))
    {
      destCur = staticBuffer;
    }
    else
    {
      newd = RegionData::create(minRectsNeeded);
      if (FOG_IS_NULL(newd)) { dest->clear(); return ERR_RT_OUT_OF_MEMORY; }
      destCur = newd->rects;
    }
  }
  else
  {
    // Can use dest.
    err_t err;
    if ((err = dest->prepare(minRectsNeeded))) { dest->clear(); return err; }
    destCur = dest->_d->rects;
  }

  destBegin = destCur;

  // prevBand serves to mark the start of the previous band so rectangles
  // can be coalesced into larger rectangles. qv. miCoalesce, above.
  // In the beginning, there is no previous band, so prevBand == curBand
  // (curBand is set later on, of course, but the first band will always
  // start at index 0). prevBand and curBand must be indices because of
  // the possible expansion, and resultant moving, of the new region's
  // array of rectangles.
  prevBand = destCur;

  do {
    curBand = destCur;

    // This algorithm proceeds one source-band (as opposed to a destination
    // band, which is determined by where the two regions intersect) at a time.
    // src1BandEnd and src2BandEnd serve to mark the rectangle after the last 
    // one in the current band for their respective regions.
    src1BandEnd = src1;
    src2BandEnd = src2;

    while ((src1BandEnd != src1End) && (src1BandEnd->y0 == src1->y0)) src1BandEnd++;
    while ((src2BandEnd != src2End) && (src2BandEnd->y0 == src2->y0)) src2BandEnd++;

    // See if we've hit an intersecting band. The two bands only
    // intersect if ybot > ytop.
    ytop = Math::max(src1->y0, src2->y0);
    ybot = Math::min(src1->y1, src2->y1);
    if (ybot > ytop)
    {
      // Intersect.
      const BoxI* i1 = src1;
      const BoxI* i2 = src2;

      int x0;
      int x1;

      while ((i1 != src1BandEnd) && (i2 != src2BandEnd))
      {
        x0 = Math::max(i1->x0, i2->x0);
        x1 = Math::min(i1->x1, i2->x1);

        // If there's any overlap between the two rectangles, add that
        // overlap to the new region.
        // There's no need to check for subsumption because the only way
        // such a need could arise is if some region has two rectangles
        // right next to each other. Since that should never happen...
        if (x0 < x1)
        {
          // Append rectangle.
          destCur->set(x0, ytop, x1, ybot);
          destCur++;
        }

        // Need to advance the pointers. Shift the one that extends
        // to the right the least, since the other still has a chance to
        // overlap with that region's next rectangle, if you see what I mean.
        if (i1->x1 < i2->x1)
        {
          i1++;
        }
        else if (i2->x1 < i1->x1)
        {
          i2++;
        }
        else
        {
          i1++;
          i2++;
        }
      }

      if (curBand != destCur)
      {
        destCur = _coalesce(destCur, &prevBand, &curBand);

        // Update x0 and x1 extents.
        if (curBand[ 0].x0 < extentsX0) extentsX0 = curBand[ 0].x0;
        if (destCur[-1].x1 > extentsX1) extentsX1 = destCur[-1].x1;
      }
    }

    // If we've finished with a band (y1 == ybot) we skip forward
    // in the region to the next band.
    if (src1->y1 == ybot) src1 = src1BandEnd;
    if (src2->y1 == ybot) src2 = src2BandEnd;
  } while ((src1 != src1End) && (src2 != src2End));

  // Finished, we have complete intersected region in destCur.
  if ((length = sysuint_t(destCur - destBegin)) != 0)
  {
    if (memOverlap)
    {
      if (destBegin == staticBuffer)
      {
        err_t err;
        if ((err = dest->reserve(length))) { dest->clear(); return err; }
        _copyRects(dest->_d->rects, destBegin, length);
      }
      else
      {
        dest->_d->deref();
        dest->_d = newd;
      }
    }

    dest->_d->length = length;
    dest->_d->extents.set(extentsX0, destBegin[0].y0, extentsX1, destCur[-1].y1);
  }
  else
  {
    if (memOverlap && destBegin == staticBuffer) Memory::free(newd);
    dest->clear();
  }
  return ERR_OK;
}

static err_t _subtractPrivate(Region* dest, const BoxI* src1, sysuint_t count1, const BoxI* src2, sysuint_t count2, bool memOverlap)
{
  BoxI* destBegin;                      // Destination begin.
  BoxI* destCur;                        // Destination ptr.
  BoxI* destEnd;                        // Destination end.

  const BoxI* src1End = src1 + count1;  // End of src1.
  const BoxI* src2End = src2 + count2;  // End of src2.

  const BoxI* src1BandEnd;              // End of current band in src1.
  const BoxI* src2BandEnd;              // End of current band in src2.

  int top;                              // Top of non-overlapping band.
  int bot;                              // Bottom of non-overlapping band.

  int ytop;                             // Top of intersection.
  int ybot;                             // Bottom of intersection.

  int extentsX0 = INT_MAX;              // Extents x0 coord (computed in loop).
  int extentsX1 = INT_MIN;              // Extents x1 coord (computed in loop).

  BoxI* prevBand;                       // Pointer to start of previous band.
  BoxI* curBand;                        // Pointer to start of current band.

  sysuint_t minRectsNeeded = (count1 + count2) * 2;

  RegionData* newd;
  sysuint_t length;

  if (memOverlap)
  {
    newd = RegionData::create(minRectsNeeded);
    if (FOG_IS_NULL(newd)) { dest->clear(); return ERR_RT_OUT_OF_MEMORY; }
  }
  else
  {
    err_t err = dest->prepare(minRectsNeeded);
    if (FOG_IS_ERROR(err)) { dest->clear(); return ERR_RT_OUT_OF_MEMORY; }
    newd = dest->_d;
  }

  destCur = newd->rects;
  destEnd = destCur + newd->capacity;
  destBegin = destCur;

  // Initialize ybot and ytop.
  // In the upcoming loop, ybot and ytop serve different functions depending
  // on whether the band being handled is an overlapping or non-overlapping
  // band.
  // In the case of a non-overlapping band (only one of the regions
  // has points in the band), ybot is the bottom of the most recent
  // intersection and thus clips the top of the rectangles in that band.
  // ytop is the top of the next intersection between the two regions and
  // serves to clip the bottom of the rectangles in the current band.
  // For an overlapping band (where the two regions intersect), ytop clips
  // the top of the rectangles of both regions and ybot clips the bottoms.

  if (src1->y0 < src2->y0)
    ybot = src1->y0;
  else
    ybot = src2->y0;

  // prevBand serves to mark the start of the previous band so rectangles
  // can be coalesced into larger rectangles. qv. coalesce, above.
  // In the beginning, there is no previous band, so prevBand == curBand
  // (curBand is set later on, of course, but the first band will always
  // start at index 0). prevBand and curBand must be indices because of
  // the possible expansion, and resultant moving, of the new region's
  // array of rectangles.
  prevBand = destCur;

  // macro for merging rectangles, it's designed to simplify loop, because
  // result of subtraction can be more than 'minRectsNeeded' we need to
  // detect end of the destination buffer.
#define ADD_RECT(__x0__, __y0__, __x1__, __y1__) \
    if (FOG_UNLIKELY(destCur == destEnd))                         \
    {                                                             \
      sysuint_t length = newd->capacity;                          \
      sysuint_t prevBandIndex = (sysuint_t)(prevBand - destBegin);\
      sysuint_t curBandIndex = (sysuint_t)(curBand - destBegin);  \
                                                                  \
      RegionData* _d = _reallocRegion(newd, length * 2);          \
      if (FOG_IS_NULL(_d)) goto _OutOfMemory;                     \
                                                                  \
      newd = _d;                                                  \
      destBegin = newd->rects;                                    \
      destCur = destBegin + length;                               \
      destEnd = destBegin + newd->capacity;                       \
                                                                  \
      prevBand = destBegin + prevBandIndex;                       \
      curBand = destBegin + curBandIndex;                         \
    }                                                             \
    destCur->set(__x0__, __y0__, __x1__, __y1__);                 \
    destCur++;                                                    \
                                                                  \
    if (FOG_UNLIKELY(extentsX0 > __x0__)) extentsX0 = __x0__;     \
    if (FOG_UNLIKELY(extentsX1 < __x1__)) extentsX1 = __x1__

  do {
    curBand = destCur;
    // This algorithm proceeds one source-band (as opposed to a
    // destination band, which is determined by where the two regions
    // intersect) at a time. src1BandEnd and src2BandEnd serve to mark the
    // rectangle after the last one in the current band for their
    // respective regions.
    src1BandEnd = src1;
    src2BandEnd = src2;

    while ((src1BandEnd != src1End) && (src1BandEnd->y0 == src1->y0)) src1BandEnd++;
    while ((src2BandEnd != src2End) && (src2BandEnd->y0 == src2->y0)) src2BandEnd++;

    // First handle the band that doesn't intersect, if any.
    //
    // Note that attention is restricted to one band in the
    // non-intersecting region at once, so if a region has n
    // bands between the current position and the next place it overlaps
    // the other, this entire loop will be passed through n times.
    if (src1->y0 < src2->y0)
    {
      // non overlap (src1) - merge it
      top = Math::max(src1->y0, ybot);
      bot = Math::min(src1->y1, src2->y0);

      if (top != bot)
      {
        const BoxI* ovrlp = src1;
        while (ovrlp != src1BandEnd) { ADD_RECT(ovrlp->x0, top, ovrlp->x1, bot); ovrlp++; }

        destCur = _coalesce(destCur, &prevBand, &curBand);
      }
    }

    // Now see if we've hit an intersecting band. The two bands only
    // intersect if ybot > ytop.
    ytop = Math::max(src1->y0, src2->y0);
    ybot = Math::min(src1->y1, src2->y1);
    if (ybot > ytop)
    {
      const BoxI* i1 = src1;
      const BoxI* i2 = src2;

      int x0 = i1->x0;

      while ((i1 != src1BandEnd) && (i2 != src2BandEnd))
      {
        if (i2->x1 <= x0)
        {
          // Subtrahend missed the boat: go to next subtrahend.
          i2++;
        }
        else if (i2->x0 <= x0)
        {
          // Subtrahend preceeds minuend: nuke left edge of minuend.
          x0 = i2->x1;
          if (x0 >= i1->x1)
          {
            // Minuend completely covered: advance to next minuend and
            // reset left fence to edge of new minuend.
            if (++i1 != src1BandEnd) x0 = i1->x0;
          }
          else
          {
            // Subtrahend now used up since it doesn't extend beyond
            // minuend.
            i2++;
          }
        }
        else if (i2->x0 < i1->x1)
        {
          // x0 part of subtrahend covers part of minuend: add uncovered
          // part of minuend to region and skip to next subtrahend.
          ADD_RECT(x0, ytop, i2->x0, ybot);

          x0 = i2->x1;
          if (x0 >= i1->x1)
          {
            // Minuend used up: advance to new...
            if (++i1 != src1BandEnd) x0 = i1->x0;
          }
          else
          {
            // Subtrahend used up.
            i2++;
          }
        }
        else
        {
          // Minuend used up: add any remaining piece before advancing.
          if (i1->x1 > x0) { ADD_RECT(x0, ytop, i1->x1, ybot); }
          if (++i1 != src1BandEnd) x0 = i1->x0;
        }
      }

      // Add remaining minuend rectangles to region.
      while (i1 != src1BandEnd)
      {
        ADD_RECT(x0, ytop, i1->x1, ybot);
        if (++i1 != src1BandEnd) x0 = i1->x0;
      }

      destCur = _coalesce(destCur, &prevBand, &curBand);
    }

    // If we've finished with a band (y1 == ybot) we skip forward
    // in the region to the next band.
    if (src1->y1 == ybot) src1 = src1BandEnd;
    if (src2->y1 == ybot) src2 = src2BandEnd;
  } while ((src1 != src1End) && (src2 != src2End));

  // Deal with whichever src1 still has rectangles left.
  if (src1 != src1End)
  {
    const BoxI* src;
    const BoxI* srcEnd;

    curBand = destCur;
    src = src1; srcEnd = src1End;

    int y0 = src->y0;
    int y1 = Math::max(src->y0, ybot);

    // Append first band and coalesce.
    while (src != srcEnd && src->y0 == y0)
    {
      ADD_RECT(src->x0, y1, src->x1, src->y1); src++;
    }
    destCur = _coalesce(destCur, &prevBand, &curBand);

    // Append remaining rectangles, coalesce isn't needed.
    while (src != srcEnd) { ADD_RECT(src->x0, src->y0, src->x1, src->y1); src++; }
  }

#undef ADD_RECT

  // Finished, we have complete intersected region in destCur.
  if ((length = sysuint_t(destCur - destBegin)) != 0)
  {
    if (memOverlap) dest->_d->deref();

    newd->extents.set(extentsX0, destBegin[0].y0, extentsX1, destCur[-1].y1);
    newd->length = length;
    dest->_d = newd;
  }
  else
  {
    if (memOverlap) Memory::free(newd);
    dest->clear();
  }
  return ERR_OK;

_OutOfMemory:
  if (memOverlap) Memory::free(newd);
  dest->clear();
  return ERR_RT_OUT_OF_MEMORY;
}

static err_t _appendPrivate(Region* dest, const BoxI* src, sysuint_t length, const BoxI* new_extents)
{
  err_t err;

  // Its guaranted that we CAN append region and its also guaranted that
  // destination region has minimal 1 rectangle, so [-1] index from end
  // must be valid.
  if ((err = dest->reserve(dest->getLength() + length))) return err;

  BoxI* destBegin = dest->_d->rects;
  BoxI* destCur = destBegin + dest->_d->length;

  const BoxI* srcEnd = src + length;

  if (src->y0 == destCur[-1].y0)
  {
    // Here is an interesting case that we will append an band that exist
    // in destination
    //
    // XXXXXXXXX
    // XXXXX<-Here

    // Merge first? (we can increase an existing band).
    if (destCur[-1].x1 == src->x0) { destCur[-1].setX1(src->x1); src++; }

    // Append all other bands.
    while (src != srcEnd && destCur[-1].y0 == src->y0) *destCur++ = *src++;

    // Coalesce.
    {
      BoxI* destBand1Begin;
      BoxI* destBand2Begin = destCur-1;

      int y = destBand2Begin->y0;

      while (destBand2Begin != destBegin && destBand2Begin[-1].y0 == y) destBand2Begin--;

      if (destBand2Begin != destBegin && destBand2Begin[-1].y1 == destBand2Begin[0].y0)
      {
        destBand1Begin = destBand2Begin - 1;
        while (destBand1Begin != destBegin && destBand1Begin[-1].y1 == y) destBand1Begin--;
        
        sysuint_t index1 = (sysuint_t)(destBand2Begin - destBand1Begin);
        sysuint_t index2 = (sysuint_t)(destCur - destBand2Begin);

        if (index1 == index2)
        {
          // It's chance for coalesce, need to compare bands.
          sysuint_t i;
          for (i = 0; i != index1; i++)
          {
            if (destBand1Begin[i].x0 == destBand2Begin[i].x0 &&
                destBand1Begin[i].x1 == destBand2Begin[i].x1) continue;
            goto _NoCoalesce;
          }

          // Coalesce now.
          y = destBand2Begin[0].y1;
          for (i = 0; i != index1; i++)
          {
            destBand1Begin[i].setY1(y);
          }
          destCur -= index1;
        }
      }
    }

_NoCoalesce:
    if (src == srcEnd) goto _End;
  }

  if (src->y0 == destCur[-1].y1)
  {
    // Coalesce, need to find previous band in dest.
    BoxI* destBandBegin = destCur-1;
    while (destBandBegin != destBegin && destBandBegin->y0 == destCur[-1].y0) destBandBegin--;

    const BoxI* srcBandBegin = src;
    const BoxI* srcBandEnd = src;
    while (srcBandEnd != srcEnd && srcBandEnd->y0 == src->y0) srcBandEnd++;

    // Now we have:
    // - source:
    //       srcBandBegin
    //       srcBandEnd
    // - dest:
    //       destBandBegin
    //       destBandEnd == destCur !
    if (srcBandEnd - srcBandBegin == destCur - destBandBegin)
    {
      // Probability for coasesce...
      sysuint_t i, len = (sysuint_t)(srcBandEnd - srcBandBegin);
      for (i = 0; i != len; i++)
      {
        if (srcBandBegin[i].x0 != destBandBegin[i].x0 ||
            srcBandBegin[i].x1 != destBandBegin[i].x1)
        {
          goto _FastAppend;
        }
      }

      // Coalesce success.
      int y1 = srcBandBegin->y1;
      for (i = 0; i != len; i++) destBandBegin[i].setY1(y1);
    }
  }

_FastAppend:
  // Fastest case, fast append, no coasesce...
  while (src != srcEnd) *destCur++ = *src++;

_End:
  dest->_d->length = (sysuint_t)(destCur - destBegin);
  dest->_d->extents = *new_extents;

  return ERR_OK;
}

// ============================================================================
// [Fog::Region - Construction / Destruction]
// ============================================================================

Static<RegionData> Region::_dnull;
Static<RegionData> Region::_dinfinite;

Region* Region::_oempty;
Region* Region::_oinfinite;

Region::Region() :
  _d(_dnull->refAlways())
{
}

Region::Region(const BoxI& rect)
{
  _d = RegionData::create(1, &rect, &rect, 1);
  if (FOG_IS_NULL(_d)) _d = _dnull->refAlways();
}

Region::Region(const RectI& rect)
{
  BoxI t(rect);
  _d = RegionData::create(1, &t, &t, 1);
  if (FOG_IS_NULL(_d)) _d = _dnull->refAlways();
}

Region::Region(const Region& other) :
  _d(other._d->ref())
{
}

Region::~Region()
{
  _d->derefInline();
}

// ============================================================================
// [Fog::Region - Implicit Sharing]
// ============================================================================

err_t Region::_detach()
{
  RegionData* d = _d;

  if (d->refCount.get() > 1)
  {
    if (d->length > 0)
    {
      d = RegionData::create(d->length, &d->extents, d->rects, d->length);
      if (FOG_IS_NULL(d)) return ERR_RT_OUT_OF_MEMORY;
    }
    else
    {
      if (d == _dinfinite.instancep()) return ERR_REGION_INFINITE;

      d = RegionData::create(d->capacity);
      if (FOG_IS_NULL(d)) return ERR_RT_OUT_OF_MEMORY;
      d->extents.reset();
    }
    atomicPtrXchg(&_d, d)->deref();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Region - Flags]
// ============================================================================

uint32_t Region::getType() const
{
  sysuint_t len = _d->length;

  // If t < 2 then it can be 0 or 1, which means REGION_TYPE_EMPTY or 
  // REGION_TYPE_SIMPLE, respectively.
  return isInfinite()
    ? REGION_TYPE_INFINITE
    : len < 2 
      ? (uint32_t)len
      : REGION_TYPE_COMPLEX;
}

err_t Region::setSharable(bool val)
{
  if (isInfinite()) return ERR_RT_INVALID_OBJECT;

  if (isSharable() == val) return ERR_OK;
  FOG_RETURN_ON_ERROR(detach());

  if (val)
    _d->flags |= RegionData::IsSharable;
  else
    _d->flags &= ~RegionData::IsSharable;
  return ERR_OK;
}

err_t Region::setStrong(bool val)
{
  if (isInfinite()) return ERR_RT_INVALID_OBJECT;

  if (isStrong() == val) return ERR_OK;
  FOG_RETURN_ON_ERROR(detach());

  if (val)
    _d->flags |= RegionData::IsStrong;
  else
    _d->flags &= ~RegionData::IsStrong;
  return ERR_OK;
}

// ============================================================================
// [Fog::Region - Container]
// ============================================================================

err_t Region::reserve(sysuint_t n)
{
  if (isInfinite()) return ERR_RT_INVALID_OBJECT;

  RegionData* d = _d;

  if (d->refCount.get() > 1)
  {
_Create:
    RegionData* newd = RegionData::create(n, &d->extents, d->rects, d->length);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;
    atomicPtrXchg(&_d, newd)->derefInline();
  }
  else if (d->capacity < n)
  {
    if (!(d->flags & RegionData::IsDynamic)) goto _Create;

    RegionData* newd = (RegionData *)Memory::realloc(d, RegionData::sizeFor(n));
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;
    newd->capacity = n;
    _d = newd;
  }

  return ERR_OK;
}

err_t Region::prepare(sysuint_t n)
{
  if (_d->refCount.get() > 1)
  {
_Create:
    RegionData* newd = RegionData::create(n);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, newd)->derefInline();
  }
  else if (_d->capacity < n)
  {
    if (!(_d->flags & RegionData::IsDynamic)) goto _Create;

    RegionData* newd = (RegionData *)Memory::realloc(_d, RegionData::sizeFor(n));
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->capacity = n;
    newd->length = 0;
    _d = newd;
  }
  else
  {
    _d->length = 0;
  }

  return ERR_OK;
}

void Region::squeeze()
{
  if (_d->flags & RegionData::IsDynamic)
  {
    sysuint_t length = _d->length;
    if (_d->capacity == length) return;

    if (_d->refCount.get() > 1)
    {
      RegionData* newd = RegionData::create(length);
      if (FOG_IS_NULL(newd)) return;

      newd->length = length;
      newd->extents = _d->extents;
      _copyRects(newd->rects, _d->rects, length);
      atomicPtrXchg(&_d, newd)->deref();
    }
    else
    {
      RegionData* newd = (RegionData *)Memory::realloc(_d, RegionData::sizeFor(length));
      if (FOG_IS_NULL(newd)) return;

      _d = newd;
      _d->capacity = length;
    }
  }
}

// ============================================================================
// [Fog::Region - HitTest]
// ============================================================================

uint32_t Region::hitTest(const PointI& pt) const
{
  if (isInfinite()) return REGION_HIT_IN;

  RegionData* d = _d;

  sysuint_t i;
  sysuint_t length = d->length;
  if (!length) return REGION_HIT_OUT;

  // Check if point position is in extents, if not -> Out.
  if (!(d->extents.contains(pt))) return REGION_HIT_OUT;
  if (length == 1) return REGION_HIT_IN;

  // Binary search for matching position.
  const BoxI* base = d->rects;
  const BoxI* r;

  int x = pt.getX();
  int y = pt.getY();

  for (i = length; i != 0; i >>= 1)
  {
    r = base + (i >> 1);

    // Try match.
    if (y >= r->y0 && y < r->y1)
    {
      if (x >= r->x0)
      {
        if (x < r->x1)
        {
          // Match.
          return REGION_HIT_IN;
        }
        else
        {
          // Move right.
          base = r + 1;
          i--;
        }
      }
      // else: Move left.
    }
    else if (r->y1 <= y)
    {
      // Move right.
      base = r + 1;
      i--;
    }
    // else: Move left.
  }
  return REGION_HIT_OUT;
}

uint32_t Region::hitTest(const RectI& r) const
{
  return hitTest(BoxI(r));
}

uint32_t Region::hitTest(const BoxI& r) const
{
  if (isInfinite()) return REGION_HIT_IN;

  RegionData* d = _d;
  sysuint_t length = d->length;

  // This is (just) a useful optimization.
  if (!length || !d->extents.overlaps(r)) return REGION_HIT_OUT;

  const BoxI* cur = d->rects;
  const BoxI* end = cur + length;

  bool partIn = false;
  bool partOut = false;

  int x = r.x0;
  int y = r.y0;

  // Can stop when both PartOut and PartIn are true, or we reach cur->y1.
  for (; cur < end; cur++)
  {
    // Getting up to speed or skipping remainder of band.
    if (cur->y1 <= y) continue;

    if (cur->y0 > y)
    {
      // Missed part of rectangle above.
      partOut = true;
      if (partIn || (cur->y0 >= r.y1)) break;
      // X guaranteed to be == rect->x0.
      y = cur->y0;
    }

    // Not far enough over yet.
    if (cur->x1 <= x) continue;

    if (cur->x0 > x)
    {
      // Missed part of rectangle to left.
      partOut = true;
      if (partIn) break;
    }

    if (cur->x0 < r.x1)
    {
      // Definitely overlap.
      partIn = true;
      if (partOut) break;
    }

    if (cur->x1 >= r.x1)
    {
      // Finished with this band.
      y = cur->y1;
      if (y >= r.y1) break;
      // Reset x out to left again.
      x = r.x0;
    }
    else
    {
      // Because boxes in a band are maximal width, if the first box
      // to overlap the rectangle doesn't completely cover it in that
      // band, the rectangle must be partially out, since some of it
      // will be uncovered in that band. partIn will have been set true
      // by now...
      break;
    }
  }

  return (partIn)
    ? ((y < r.y1) ? REGION_HIT_PART : REGION_HIT_IN)
    : REGION_HIT_OUT;
}

// ============================================================================
// [Fog::Region - Clear / Reset]
// ============================================================================

void Region::clear()
{
  if (isInfinite())
  {
    atomicPtrXchg(&_d, _dnull->refAlways())->derefInline();
    return;
  }

  RegionData* d = _d;
  if (d->length == 0) return;

  if (d->refCount.get() > 0)
  {
    atomicPtrXchg(&_d, _dnull->refAlways())->derefInline();
  }
  else
  {
    d->length = 0;
    d->extents.reset();
  }
}

void Region::reset()
{
  atomicPtrXchg(&_d, _dnull->refAlways())->derefInline();
}

// ============================================================================
// [Fog::Region - Operations]
// ============================================================================

err_t Region::set(const Region& r)
{
  RegionData* td = _d;
  RegionData* rd = r._d;
  if (td == rd) return ERR_OK;

  if (!r.isInfinite() && ((td->flags & RegionData::IsStrong) || !(rd->flags & RegionData::IsSharable)))
  {
    FOG_RETURN_ON_ERROR(prepare(rd->length));

    td = _d;
    td->length = rd->length;
    td->extents = rd->extents;
    _copyRects(td->rects, rd->rects, rd->length);
  }
  else
  {
    atomicPtrXchg(&_d, rd->refAlways())->derefInline();
  }

  return ERR_OK;
}

err_t Region::set(const RectI& r)
{
  if (!r.isValid()) { clear(); return ERR_OK; }

  FOG_RETURN_ON_ERROR(prepare(1));

  RegionData* d = _d;
  d->length = 1;
  d->extents = BoxI(r);
  d->rects[0] = d->extents;

  return ERR_OK;
}

err_t Region::set(const BoxI& r)
{
  if (!r.isValid()) { clear(); return ERR_OK; }

  FOG_RETURN_ON_ERROR(prepare(1));

  RegionData* d = _d;
  d->length = 1;
  d->extents = r;
  d->rects[0] = r;

  return ERR_OK;
}

err_t Region::setDeep(const Region& r)
{
  if (r.isInfinite())
  {
    atomicPtrXchg(&_d, r._d->refAlways())->derefInline();
    return ERR_OK;
  }

  RegionData* td = _d;
  RegionData* rd = r._d;
  if (td == rd) return ERR_OK;

  FOG_RETURN_ON_ERROR(prepare(rd->length));

  td = _d;
  td->length = rd->length;
  td->extents = rd->extents;
  _copyRects(td->rects, rd->rects, rd->length);

  return ERR_OK;
}

err_t Region::set(const RectI* rects, sysuint_t length)
{
  // TODO: Not optimal.
  FOG_RETURN_ON_ERROR(prepare(length));

  for (sysuint_t i = 0; i < length; i++) combine(rects[i], REGION_OP_UNION);
  return ERR_OK;
}

err_t Region::set(const BoxI* rects, sysuint_t length)
{
  // TODO: Not optimal.
  FOG_RETURN_ON_ERROR(prepare(length));

  for (sysuint_t i = 0; i < length; i++) combine(rects[i], REGION_OP_UNION);
  return ERR_OK;
}

err_t Region::combine(const Region& r, uint32_t combineOp)
{
  err_t err;

  RegionData* td = _d;
  RegionData* rd = r._d;

  switch (combineOp)
  {
    case REGION_OP_REPLACE:
      return set(r);

    case REGION_OP_INTERSECT:
    {
      // If destination is infinite then source is result.
      // If source is infinite then destination is result.
      if (td == _dinfinite.instancep())
        return set(r);
      else if (rd == _dinfinite.instancep())
        return ERR_OK;

      if (td == rd)
        ;
      else if (td->length == 0 || rd->length == 0 || !td->extents.overlaps(rd->extents))
        clear();
      else
        return _intersectPrivate(this, td->rects, td->length, rd->rects, rd->length, true);

      return ERR_OK;
    }

    case REGION_OP_UNION:
    {
      // If destination or source region is infinite then result is infinite.
      if (td == _dinfinite.instancep())
        return ERR_OK;
      else if (rd == _dinfinite.instancep())
        return set(infinite());

      // Union region is same or r is empty... -> nop.
      if (td == rd || rd->length == 0) return ERR_OK;

      // We are empty or r completely subsumes us -> set r.
      if (td->length == 0 || (rd->length == 1 && rd->extents.subsumes(td->extents))) return set(r);

      // We completely subsumes r.
      if (td->length == 1 && rd->length == 1 && td->extents.subsumes(rd->extents)) return ERR_OK;

      // Last optimization can be append.
      const BoxI* tdLast = td->rects + td->length - 1;
      const BoxI* rdFirst = rd->rects;

      BoxI ext(Math::min(td->extents.x0, rd->extents.x0),
               Math::min(td->extents.y0, rd->extents.y0),
               Math::max(td->extents.x1, rd->extents.x1),
               Math::max(td->extents.y1, rd->extents.y1));

      if (tdLast->y1 <= rdFirst->y0 ||
         (tdLast->y0 == rdFirst->y0 &&
          tdLast->y1 == rdFirst->y1 &&
          tdLast->x1 <= rdFirst->x0 ))
      {
        err = _appendPrivate(this, rd->rects, rd->length, &ext);
      }
      else
      {
        err = _unitePrivate(this, td->rects, td->length, rd->rects, rd->length, true, &ext);
      }

      return err;
    }

    case REGION_OP_XOR:
    {
      return combine(*this, *this, r, combineOp);
    }

    case REGION_OP_SUBTRACT:
    {
      // Infinite regions are forbidden for SUBTRACT.
      if (td == _dinfinite.instancep() || rd == _dinfinite.instancep())
      {
        clear();
        return ERR_REGION_INFINITE;
      }

      if (td == rd) 
        clear();
      else if (td->length == 0 || rd->length == 0 || !td->extents.overlaps(rd->extents)) 
        ;
      else
        return _subtractPrivate(this, td->rects, td->length, rd->rects, rd->length, true);

      return ERR_OK;
    }

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

err_t Region::combine(const RectI& r, uint32_t combineOp)
{
  return combine(BoxI(r), combineOp);
}

err_t Region::combine(const BoxI& r, uint32_t combineOp)
{
  RegionData* td = _d;
  err_t err;

  switch (combineOp)
  {
    case REGION_OP_REPLACE:
    {
      return set(r);
    }

    case REGION_OP_INTERSECT:
    {
      if (td == _dinfinite.instancep()) return set(r);

      if (td->length == 0 || !r.isValid() || !td->extents.overlaps(r)) 
      {
        clear();
        return ERR_OK;
      }

      return _intersectPrivate(this, td->rects, td->length, &r, 1, true);
    }

    case REGION_OP_UNION:
    {
      if (td == _dinfinite.instancep()) return ERR_OK;
      if (!r.isValid()) return ERR_OK;

      // We are empty or 'r' completely subsumes us.
      if (td->length == 0 || r.subsumes(td->extents)) return set(r);

      // We completely subsumes src.
      if (td->length == 1 && td->extents.subsumes(r)) return ERR_OK;

      BoxI ext(Math::min(td->extents.x0, r.x0),
               Math::min(td->extents.y0, r.y0),
               Math::max(td->extents.x1, r.x1),
               Math::max(td->extents.y1, r.y1));

      // Last optimization can be append.
      BoxI* tdLast = td->rects + td->length-1;

      if (tdLast->y1 <= r.y0 ||
         (tdLast->y0 == r.y0 &&
          tdLast->y1 == r.y1 &&
          tdLast->x1 <= r.x0 ))
      {
        err = _appendPrivate(this, &r, 1, &ext);
      }
      else
      {
        err = _unitePrivate(this, td->rects, td->length, &r, 1, true, &ext);
      }

      return err;
    }

    case REGION_OP_XOR:
    {
      return combine(*this, *this, r, combineOp);
    }

    case REGION_OP_SUBTRACT:
      // Infinite regions are forbidden for SUBTRACT.
      if (td == _dinfinite.instancep())
      {
        clear();
        return ERR_REGION_INFINITE;
      }

      if (td->length == 0 || !r.isValid() || !td->extents.overlaps(r))
      {
        return ERR_OK;
      }

      return _subtractPrivate(this, td->rects, td->length, &r, 1, true);

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

err_t Region::translate(const PointI& pt)
{
  return translate(*this, *this, pt);
}

err_t Region::shrink(const PointI& pt)
{
  return shrink(*this, *this, pt);
}

err_t Region::frame(const PointI& pt)
{
  return frame(*this, *this, pt);
}

bool Region::eq(const Region& other) const
{
  RegionData* d1 = _d;
  RegionData* d2 = other._d;

  if (d1 == d2) return true;
  if (d1 == _dinfinite.instancep() || d2 == _dinfinite.instancep()) return false;
  if (d1->length != d2->length) return false;

  sysuint_t i;
  sysuint_t length = d1->length;

  for (i = 0; i != length; i++)
  {
    if (d1->rects[i] != d2->rects[i]) return false;
  }

  return true;
}

// ============================================================================
// [Fog::Region - Windows Specific]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

static FOG_INLINE void BoxToRECT(RECT* dest, const BoxI* src)
{
  dest->left   = src->x0;
  dest->top    = src->y0;
  dest->right  = src->x1;
  dest->bottom = src->y1;
}

static FOG_INLINE void RECTToBox(BoxI* dest, const RECT* src)
{
  dest->x0 = src->left;
  dest->y0 = src->top;
  dest->x1 = src->right;
  dest->y1 = src->bottom;
}

HRGN Region::toHRGN() const
{
  RegionData* d = _d;
  sysuint_t i;
  sysuint_t length = d->length;

  PBuffer<4096> mem;
  RGNDATAHEADER *hdr = (RGNDATAHEADER *)mem.alloc(sizeof(RGNDATAHEADER) + length * sizeof(RECT));
  if (FOG_IS_NULL(hdr)) return (HRGN)NULLREGION;

  hdr->dwSize = sizeof(RGNDATAHEADER);
  hdr->iType = RDH_RECTANGLES;
  hdr->nCount = (uint32_t)length;
  hdr->nRgnSize = (DWORD)(length * sizeof(RECT));
  BoxToRECT(&hdr->rcBound, &d->extents);

  RECT* r = (RECT *)((uint8_t *)hdr + sizeof(RGNDATAHEADER));
  for (i = 0; i < length; i++, r++)
  {
    BoxToRECT(r, &d->rects[i]);
  }
  return ExtCreateRegion(NULL, (DWORD)(sizeof(RGNDATAHEADER) + (length * sizeof(RECT))), (RGNDATA *)hdr);
}

err_t Region::fromHRGN(HRGN hrgn)
{
  clear();

  if (hrgn == NULL) return ERR_RT_INVALID_ARGUMENT;
  if (hrgn == (HRGN)NULLREGION) return ERR_OK;

  DWORD size = GetRegionData(hrgn, 0, NULL);
  if (size == 0) return false;

  PBuffer<1024> mem;
  RGNDATAHEADER *hdr = (RGNDATAHEADER *)mem.alloc(size);
  if (hdr) return ERR_RT_OUT_OF_MEMORY;

  if (!GetRegionData(hrgn, size, (RGNDATA*)hdr)) return GetLastError();

  sysuint_t i;
  sysuint_t length = hdr->nCount;
  RECT* r = (RECT*)((uint8_t*)hdr + sizeof(RGNDATAHEADER));

  // TODO: Rects should be already YX sorted, but I'm not sure.
  for (i = 0; i != length; i++, r++)
  {
    combine(BoxI(r->left, r->top, r->right, r->bottom), REGION_OP_UNION);
  }

  return ERR_OK;
}

#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::Region - Statics]
// ============================================================================

err_t Region::combine(Region& dest, const Region& src1, const Region& src2, uint32_t combineOp)
{
  RegionData* destd = dest._d;
  RegionData* src1d = src1._d;
  RegionData* src2d = src2._d;
  err_t err = ERR_OK;

  switch (combineOp)
  {
    case REGION_OP_REPLACE:
    {
      return dest.set(src2);
    }

    case REGION_OP_INTERSECT:
    {
      // Trivial operations.
      if (src1.isInfinite()) { return dest.set(src2); }
      if (src2.isInfinite()) { return dest.set(src1); }

      // Trivial rejects.
      if (FOG_UNLIKELY(src1d == src2d))
        err = dest.set(src1);
      else if (FOG_UNLIKELY(src1d->length == 0 || src2d->length == 0 || !src1d->extents.overlaps(src2d->extents))) 
        dest.clear();
      else
        err = _intersectPrivate(&dest, src1d->rects, src1d->length, src2d->rects, src2d->length, destd == src1d || destd == src2d);

      return err;
    }

    case REGION_OP_UNION:
    {
      // Trivial operations.
      if (src1.isInfinite()) { return dest.set(src1); }
      if (src2.isInfinite()) { return dest.set(src2); }

      if (src1d->length == 0) { return dest.set(src2); }
      if (src2d->length == 0) { return dest.set(src1); }
      if (src1d == src2d    ) { return dest.set(src1); }

      const BoxI* src1first = src1d->rects;
      const BoxI* src2first = src2d->rects;
      const BoxI* src1last = src1first + src1d->length - 1;
      const BoxI* src2last = src2first + src2d->length - 1;

      BoxI ext(Math::min(src1d->extents.x0, src2d->extents.x0),
               Math::min(src1d->extents.y0, src2d->extents.y0),
               Math::max(src1d->extents.x1, src2d->extents.x1),
               Math::max(src1d->extents.y1, src2d->extents.y1));

      err_t err;
      if (src1last->y1 <= src2first->y0)
      {
        err = dest.setDeep(src1);
        if (!err) err = _appendPrivate(&dest, src2first, src2d->length, &ext);
      }
      else if (src2last->y1 <= src1first->y0)
      {
        err = dest.setDeep(src2);
        if (!err) err = _appendPrivate(&dest, src1first, src1d->length, &ext);
      }
      else
      {
        err = _unitePrivate(&dest, src1first, src1d->length, src2first, src2d->length, destd == src1d || destd == src2d, &ext);
      }

      return err;
    }

    case REGION_OP_XOR:
    {
      // Infinite region is forbidden for XOR operation.
      if (src1d == _dinfinite.instancep() || src2d == _dinfinite.instancep())
      {
        dest.clear();
        return ERR_REGION_INFINITE;
      }

      TemporaryRegion<REGION_STACK_SIZE> r1;
      TemporaryRegion<REGION_STACK_SIZE> r2;

      FOG_RETURN_ON_ERROR(combine(r1, src1, src2, REGION_OP_SUBTRACT));
      FOG_RETURN_ON_ERROR(combine(r2, src2, src1, REGION_OP_SUBTRACT));
      FOG_RETURN_ON_ERROR(combine(dest, r1, r2, REGION_OP_UNION));

      return ERR_OK;
    }

    case REGION_OP_SUBTRACT:
    {
      // Infinite region is forbidden for XOR operation.
      if (src1d == _dinfinite.instancep() || src2d == _dinfinite.instancep())
      {
        dest.clear();
        return ERR_REGION_INFINITE;
      }

      // Trivial rejects.
      if (src1d == src2d || src1d->length == 0) 
        dest.clear();
      else if (src2d->length == 0 || !src1d->extents.overlaps(src2d->extents)) 
        err = dest.set(src1);
      else
        err = _subtractPrivate(&dest, src1d->rects, src1d->length, src2d->rects, src2d->length, destd == src1d || destd == src2d);

      return err;
    }

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

err_t Region::combine(Region& dst, const Region& src1, const BoxI& src2, uint32_t combineOp)
{
  return combine(dst, src1, TemporaryRegion<1>(src2), combineOp);
}

err_t Region::combine(Region& dst, const BoxI& src1, const Region& src2, uint32_t combineOp)
{
  return combine(dst, TemporaryRegion<1>(src1), src2, combineOp);
}

err_t Region::combine(Region& dst, const BoxI& src1, const BoxI& src2, uint32_t combineOp)
{
  return combine(dst, TemporaryRegion<1>(src1), TemporaryRegion<1>(src2), combineOp);
}

err_t Region::translate(Region& dest, const Region& src, const PointI& pt)
{
  if (src.isInfinite()) return dest.set(src);

  int x = pt.getX();
  int y = pt.getY();

  if (x == 0 && y == 0) { return dest.set(src); }

  RegionData* dest_d = dest._d;
  RegionData* src_d = src._d;

  sysuint_t i;
  BoxI* dest_r;
  BoxI* src_r;

  if (src_d->length == 0) 
  {
    dest.clear();
  }
  else if (dest_d == src_d && dest_d->refCount.get() == 1)
  {
    dest_d->extents.translate(x, y);
    dest_r = dest_d->rects;
    for (i = dest_d->length; i; i--, dest_r++) dest_r->translate(x, y);
  }
  else
  {
    err_t err = dest.prepare(src_d->length);
    if (FOG_IS_ERROR(err)) return err;

    dest_d = dest._d; 

    dest_d->extents.set(src_d->extents.x0 + x, src_d->extents.y0 + y,
                        src_d->extents.x1 + x, src_d->extents.y1 + y);
    dest_d->length = src_d->length;

    dest_r = dest_d->rects;
    src_r = src_d->rects;

    for (i = dest_d->length; i; i--, dest_r++) 
    {
      dest_r->set(src_r->x0 + x, src_r->y0 + y, src_r->x1 + x, src_r->y1 + y);
    }
  }
  return ERR_OK;
}

err_t Region::shrink(Region& dest, const Region& src, const PointI& pt)
{
  if (src.isInfinite()) return dest.set(src);

  int x = pt.getX();
  int y = pt.getY();
  if (x == 0 && y == 0) return dest.set(src);

  err_t err = dest.setDeep(src);
  if (FOG_IS_ERROR(err)) return err;

  TemporaryRegion<REGION_STACK_SIZE> s;
  TemporaryRegion<REGION_STACK_SIZE> t;
  bool grow;

  if (x) { if ((grow = (x < 0))) { x = -x; } err = _compress(dest, s, t, uint(x) * 2, true , grow); if (FOG_IS_ERROR(err)) return err; }
  if (y) { if ((grow = (y < 0))) { y = -y; } err = _compress(dest, s, t, uint(y) * 2, false, grow); if (FOG_IS_ERROR(err)) return err; }

  return dest.translate(x, y);
}

err_t Region::frame(Region& dest, const Region& src, const PointI& pt)
{
  if (src.isInfinite()) return dest.set(src);

  // In cases that dest == src, we need to backup src.
  TemporaryRegion<REGION_STACK_SIZE> r1;
  TemporaryRegion<REGION_STACK_SIZE> r2;

  FOG_RETURN_ON_ERROR(translate(r2, src, PointI(-pt.getX(), 0)));
  FOG_RETURN_ON_ERROR(translate(r1, src, PointI( pt.getX(), 0)));
  FOG_RETURN_ON_ERROR(r2.combine(r1, REGION_OP_INTERSECT));
  FOG_RETURN_ON_ERROR(translate(r1, src, PointI(0, -pt.getY())));
  FOG_RETURN_ON_ERROR(r2.combine(r1, REGION_OP_INTERSECT));
  FOG_RETURN_ON_ERROR(translate(r1, src, PointI(0,  pt.getY())));
  FOG_RETURN_ON_ERROR(r2.combine(r1, REGION_OP_INTERSECT));
  FOG_RETURN_ON_ERROR(combine(dest, src, r2, REGION_OP_SUBTRACT));

  return ERR_OK;
}

// TODO: Check if it's correct.
err_t Region::intersectAndClip(Region& dst, const Region& src1Region, const Region& src2Region, const BoxI& clip)
{
  const BoxI* src1 = src1Region.getData();
  const BoxI* src2 = src2Region.getData();

  sysuint_t count1 = src1Region.getLength();
  sysuint_t count2 = src2Region.getLength();

  if (src1Region.isInfinite()) { src1 = &clip, count1 = 1; }
  if (src2Region.isInfinite()) { src2 = &clip, count2 = 1; }

  int clipX0 = clip.x0;
  int clipY0 = clip.y0;
  int clipX1 = clip.x1;
  int clipY1 = clip.y1;

  bool memOverlap = (&dst == &src1Region) | 
                    (&dst == &src2Region) ;

  BoxI* destBegin;                      // Destination begin.
  BoxI* destCur;                        // Destination ptr.

  const BoxI* src1End = src1 + count1;  // End of src1.
  const BoxI* src2End = src2 + count2;  // End of src2.

  const BoxI* src1BandEnd;              // End of current band in src1.
  const BoxI* src2BandEnd;              // End of current band in src2.

  int ytop;                             // Top of intersection.
  int ybot;                             // Bottom of intersection.
  int ytopAdjusted;                     // Top of intersection, intersected with clip.
  int ybotAdjusted;                     // Bottom of intersection, intersected with clip.

  int extentsX0 = INT_MAX;              // Extents x0 coord (computed in loop).
  int extentsX1 = INT_MIN;              // Extents x1 coord (computed in loop).

  BoxI* prevBand;                       // Pointer to start of previous band.
  BoxI* curBand;                        // Pointer to start of current band.

  // Simplest case, if there is only 1 rect in each -> rects overlap.
  if (count1 == 1 && count2 == 1)
  {
    return dst.set(BoxI(Math::max(src1->x0, src2->x0), Math::max(src1->y0, src2->y0),
                        Math::min(src1->x1, src2->x1), Math::min(src1->y1, src2->y1)));
  }

  sysuint_t minRectsNeeded = (count1 + count2) * 2;

  // Find first rectangle that can intersect.
  for (;;)
  {
    if (src1 == src1End || src2 == src2End) { dst.clear(); return ERR_OK; }

    if (src1->y1 <= src2->y0 || src1->y1 <= clipY0) { src1++; continue; }
    if (src2->y1 <= src1->y0 || src2->y1 <= clipY0) { src2++; continue; }

    break;
  }

  // New region data in case that it needs it.
  RegionData* newd = NULL;
  sysuint_t length;

  // Local buffer that can be used instead of malloc in most calls
  // can be increased to higher values, but I think that 32 is ok.
  BoxI staticBuffer[32];

  if (memOverlap)
  {
    // Need to allocate new block.
    if (minRectsNeeded <= FOG_ARRAY_SIZE(staticBuffer))
    {
      destCur = staticBuffer;
    }
    else
    {
      newd = RegionData::create(minRectsNeeded);
      if (FOG_IS_NULL(newd)) { dst.clear(); return ERR_RT_OUT_OF_MEMORY; }
      destCur = newd->rects;
    }
  }
  else
  {
    // Can use dest.
    err_t err;
    if ((err = dst.prepare(minRectsNeeded))) { dst.clear(); return err; }
    destCur = dst._d->rects;
  }

  destBegin = destCur;

  // prevBand serves to mark the start of the previous band so rectangles
  // can be coalesced into larger rectangles. qv. miCoalesce, above.
  // In the beginning, there is no previous band, so prevBand == curBand
  // (curBand is set later on, of course, but the first band will always
  // start at index 0). prevBand and curBand must be indices because of
  // the possible expansion, and resultant moving, of the new region's
  // array of rectangles.
  prevBand = destCur;

  do {
    curBand = destCur;

    // This algorithm proceeds one source-band (as opposed to a destination
    // band, which is determined by where the two regions intersect) at a time.
    // src1BandEnd and src2BandEnd serve to mark the rectangle after the last
    // one in the current band for their respective regions.
    src1BandEnd = src1;
    src2BandEnd = src2;

    while ((src1BandEnd != src1End) && (src1BandEnd->y0 == src1->y0)) src1BandEnd++;
    while ((src2BandEnd != src2End) && (src2BandEnd->y0 == src2->y0)) src2BandEnd++;

    // See if we've hit an intersecting band. The two bands only
    // intersect if ybot > ytop.
    ytop = Math::max(src1->y0, src2->y0);
    ybot = Math::min(src1->y1, src2->y1);

    ytopAdjusted = ytop;
    ybotAdjusted = ybot;

    if (ytopAdjusted < clipY0) ytopAdjusted = clipY0;
    else if (ytopAdjusted >= clipY1) break;
    if (ybotAdjusted > clipY1) ybotAdjusted = clipY1;

    if (ybotAdjusted > ytopAdjusted)
    {
      // Intersect.
      const BoxI* i1 = src1;
      const BoxI* i2 = src2;

      int x0;
      int x1;

      while ((i1 != src1BandEnd) && (i2 != src2BandEnd))
      {
        x0 = Math::max(i1->x0, i2->x0, clipX0);
        x1 = Math::min(i1->x1, i2->x1, clipX1);

        // If there's any overlap between the two rectangles, add that
        // overlap to the new region.
        // There's no need to check for subsumption because the only way
        // such a need could arise is if some region has two rectangles
        // right next to each other. Since that should never happen...
        if (x0 < x1)
        {
          // Append rectangle.
          destCur->set(x0, ytopAdjusted, x1, ybotAdjusted);
          destCur++;
        }

        // Need to advance the pointers. Shift the one that extends
        // to the right the least, since the other still has a chance to
        // overlap with that region's next rectangle, if you see what I mean.
        if (i1->x1 < i2->x1)
        {
          i1++;
        }
        else if (i2->x1 < i1->x1)
        {
          i2++;
        }
        else
        {
          i1++;
          i2++;
        }
      }

      if (curBand != destCur)
      {
        destCur = _coalesce(destCur, &prevBand, &curBand);

        // Update x0 and x1 extents.
        if (curBand[ 0].x0 < extentsX0) extentsX0 = curBand[ 0].x0;
        if (destCur[-1].x1 > extentsX1) extentsX1 = destCur[-1].x1;
      }
    }

    // If we've finished with a band (y1 == ybot) we skip forward
    // in the region to the next band.
    if (src1->y1 == ybot) src1 = src1BandEnd;
    if (src2->y1 == ybot) src2 = src2BandEnd;
  } while ((src1 != src1End) && (src2 != src2End));

  // Finished, we have complete intersected region in destCur.
  if ((length = sysuint_t(destCur - destBegin)) != 0)
  {
    if (memOverlap)
    {
      if (destBegin == staticBuffer)
      {
        err_t err;
        if ((err = dst.reserve(length))) { dst.clear(); return err; }
        _copyRects(dst._d->rects, destBegin, length);
      }
      else
      {
        dst._d->deref();
        dst._d = newd;
      }
    }

    dst._d->length = length;
    dst._d->extents.set(extentsX0, destBegin[0].y0, extentsX1, destCur[-1].y1);
  }
  else
  {
    if (memOverlap && destBegin == staticBuffer) Memory::free(newd);
    dst.clear();
  }
  return ERR_OK;
}

// TODO: Check if it's correct.
err_t Region::translateAndClip(Region& dst, const Region& src1Region, const PointI& pt, const BoxI& clip)
{
  if (src1Region.isInfinite()) return dst.set(src1Region);

  int x = pt.x;
  int y = pt.y;

  // Don't waste CPU cycles if translation point is zero (no translation).
  if (x == 0 && y == 0)
  {
    return _intersectPrivate(&dst, src1Region.getData(), src1Region.getLength(), &clip, 1, &dst == &src1Region);
  }

  int clipX0 = clip.x0;
  int clipY0 = clip.y0;
  int clipX1 = clip.x1;
  int clipY1 = clip.y1;

  BoxI* destBegin;
  BoxI* destCur;

  BoxI* prevBand;
  BoxI* curBand;

  RegionData* newd = NULL;

  if (&dst == &src1Region) 
  {
    if (dst.getRefCount() == 1)
    {
      destCur = dst._d->rects;
    }
    else
    {
      newd = RegionData::create(src1Region.getLength());
      if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;
      destCur = newd->rects;
    }
  }
  else
  {
    err_t err = dst.prepare(src1Region.getLength());
    if (FOG_IS_ERROR(err)) return err;
    destCur = dst.getDataX();
  }

  const BoxI* srcCur = src1Region.getData();
  const BoxI* srcEnd = srcCur + src1Region.getLength();

  int srcY0;
  int srcY1;
  int srcY1Orig;

  int extentsX0 = INT_MAX;
  int extentsX1 = INT_MIN;

  destBegin = destCur;
  prevBand = destCur;

  do {
    // Adjust and clip Y0->Y1.
    srcY1Orig = srcCur->y1;
    srcY1 = Math::min(srcY1Orig + y, clipY1);

    if (srcY1 <= clipY0)
    {
_Next:
      for (;;)
      {
        if (++srcCur == srcEnd) goto _End;
        if (srcCur[0].y1 == srcY1Orig) continue;
        break;
      }
      continue;
    }

    srcY0 = Math::max(srcCur->y0 + y, clipY0);
    if (srcY0 >= clipY1) goto _End;
    if (srcY0 >= srcY1) goto _Next;

    // Adjust and clip X0->X1.
    curBand = destCur;
    do {
      int x0 = Math::max(srcCur[0].x0 + x, clipX0);
      int x1 = Math::min(srcCur[0].x1 + x, clipX1);

      if (x0 < x1)
      {
        if (extentsX0 > x0) extentsX0 = x0;
        if (extentsX1 < x1) extentsX1 = x1;

        destCur->set(x0, srcY0, x1, srcY1);
        destCur++;
      }

      if (++srcCur == srcEnd) break;
    } while (srcCur[0].y1 == srcY1Orig);

    // Coalesce.
    destCur = _coalesce(destCur, &prevBand, &curBand);
  } while (srcCur != srcEnd);

_End:
  if (destCur != destBegin)
  {
    if (newd) atomicPtrXchg(&dst._d, newd)->deref();

    dst._d->length = (sysuint_t)(destCur - dst._d->rects);
    dst._d->extents.set(extentsX0, dst._d->rects[0].y0, extentsX1, destCur[-1].y1);
  }
  else
  {
    if (newd) Memory::free(newd);
    dst.clear();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

static Static<Region> _oempty_region;
static Static<Region> _oinfinite_region;

FOG_NO_EXPORT void _g2d_region_init(void)
{
  RegionData* d;

  d = Region::_dnull.instancep();
  d->refCount.init(1);
  d->flags = RegionData::IsSharable;
  d->extents.reset();
  d->rects[0].reset();

  d = Region::_dinfinite.instancep();
  d->refCount.init(1);
  d->flags = RegionData::IsSharable;
  d->extents.set(INT_MIN, INT_MIN, INT_MAX, INT_MAX);
  d->rects[0].set(INT_MIN, INT_MIN, INT_MAX, INT_MAX);

  _oempty_region.instance()._d = Region::_dnull.instancep();
  _oinfinite_region.instance()._d = Region::_dinfinite.instancep();

  Region::_oempty    = _oempty_region.instancep();
  Region::_oinfinite = _oinfinite_region.instancep();
}

FOG_NO_EXPORT void _g2d_region_fini(void)
{
  Region::_dinfinite->refCount.dec();
  Region::_dnull->refCount.dec();
}

} // Fog namespace
