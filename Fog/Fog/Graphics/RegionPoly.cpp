// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Std.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Region.h>

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
// [Fog::Region - Polygon]
// ============================================================================

// This file contains a few macros to help track the edge of a filled object.
// The object is assumed to be filled in scanline order, and thus the algorithm
// used is an extension of Bresenham's line drawing algorithm which assumes
// that y is always the major axis.
//
// Since these pieces of code are the same for any filled shape, it is more
// convenient to gather the library in one place, but since these pieces of
// code are also in the inner loops of output primitives, procedure call
// overhead is out of the question. See the author for a derivation if needed.

// In scan converting polygons, we want to choose those pixels which are inside
// the polygon. Thus, we add .5 to the starting x coordinate for both left and
// right edges. Now we choose the first pixel which is inside the pgon for the
// left edge and the first pixel which is outside the pgon for the right edge.
// Draw the left pixel, but not the right.
//
// How to add .5 to the starting x coordinate:
//     If the edge is moving to the right, then subtract dy from the
// error term from the general form of the algorithm.
//     If the edge is moving to the left, then add dy to the error term.
//
// The reason for the difference between edges moving to the left and edges
// moving to the right is simple:  If an edge is moving to the right, then
// we want the algorithm to flip immediately. If it is moving to the left,
// then we don't want it to flip until we traverse an entire pixel.
#define BRESINITPGON(dy, x1, x2, x_start, d, m, m1, incr1, incr2) \
{ \
  int dx; /* local storage */ \
  \
  /* if the edge is horizontal, then it is ignored */ \
  /* and assumed not to be processed.  Otherwise, do this stuff. */ \
  if ((dy) != 0) \
  { \
    x_start = (x1); \
    dx = (x2) - x_start; \
    if (dx < 0) \
    { \
      m = dx / (dy); \
      m1 = m - 1; \
      incr1 = -2 * dx + 2 * (dy) * m1; \
      incr2 = -2 * dx + 2 * (dy) * m; \
      d = 2 * m * (dy) - 2 * dx - 2 * (dy); \
    } \
    else \
    { \
      m = dx / (dy); \
      m1 = m + 1; \
      incr1 = 2 * dx - 2 * (dy) * m1; \
      incr2 = 2 * dx - 2 * (dy) * m; \
      d = -2 * m * (dy) + 2 * dx; \
    } \
  } \
}

#define BRESINCRPGON(d, minval, m, m1, incr1, incr2) \
{ \
  if (m1 > 0) \
  { \
    if (d > 0) \
    { \
      minval += m1; \
      d += incr1; \
    } \
    else \
    { \
      minval += m; \
      d += incr2; \
    } \
  } \
  else \
  { \
    if (d >= 0) \
    { \
      minval += m1; \
      d += incr1; \
    } \
    else \
    { \
      minval += m; \
      d += incr2; \
    } \
  } \
}

// This structure contains all of the information needed to run the bresenham
// algorithm. The variables may be hardcoded into the declarations instead of
// using this structure to make use of register declarations.
typedef struct
{
  int minor_axis;   // minor axis
  int d;            // decision variable
  int m, m1;        // slope and slope+1
  int incr1, incr2; // error increments
} BRESINFO;


#define BRESINITPGONSTRUCT(dmaj, min1, min2, bres) \
        BRESINITPGON(dmaj, min1, min2, bres.minor_axis, bres.d, \
                     bres.m, bres.m1, bres.incr1, bres.incr2)

#define BRESINCRPGONSTRUCT(bres) \
        BRESINCRPGON(bres.d, bres.minor_axis, bres.m, bres.m1, bres.incr1, bres.incr2)



// These are the data structures needed to scan convert regions. Two different
// scan conversion methods are available -- the even-odd method, and the
// winding number method. The even-odd rule states that a point is inside the
// polygon if a ray drawn from that point in any direction will pass through an
// odd number of path segments. By the winding number rule, a point is decided
// to be inside the polygon if a ray drawn from that point in any direction
// passes through a different number of clockwise and counter-clockwise path
// segments.
//
// These data structures are adapted somewhat from the algorithm in
// (Foley/Van Dam) for scan converting polygons. The basic algorithm is to start
// at the top (smallest y) of the polygon, stepping down to the bottom of
// the polygon by incrementing the y coordinate. We keep a list of edges which
// the current scanline crosses, sorted by x. This list is called the Active
// Edge Table (AET) As we change the y-coordinate, we update each entry in in
// the active edge table to reflect the edges new xcoord. This list must be sorted
// at each scanline in case two edges intersect.
//
// We also keep a data structure known as the Edge Table (ET), which keeps track
// of all the edges which the current scanline has not yet reached. The ET is
// basically a list of ScanLineList structures containing a list of edges which
// are entered at a given scanline. There is one ScanLineList per scanline at
// which an edge is entered. When we enter a new edge, we move it from the ET to
// the AET.
//
// From the AET, we can implement the even-odd rule as in (Foley/Van Dam). The
// winding number rule is a little trickier.// We also keep the EdgeTableEntries
// in the AET linked by the nextWETE (winding EdgeTableEntry) link. This allows
// the edges to be linked just as before for updating purposes, but only uses
// the edges linked by the nextWETE link as edges representing spans of the
// polygon to drawn (as with the even-odd rule).

// For the winding number rule.
#define CLOCKWISE          1
#define COUNTERCLOCKWISE  -1

struct EdgeTableEntry
{
  int ymax;                  // ycoord at which we exit this edge.
  BRESINFO bres;             // Bresenham info to run the edge
  EdgeTableEntry* next;      // next in the list
  EdgeTableEntry* back;      // for insertion sort
  EdgeTableEntry* nextWETE;  // for winding num rule
  int ClockWise;             // flag for winding number rule
};


struct ScanLineList
{
  int scanline;              // the scanline represented
  EdgeTableEntry* edgelist;  // header node
  ScanLineList* next;        // next in the list
};

struct EdgeTable
{
  int ymax;                  // ymax for the polygon
  int ymin;                  // ymin for the polygon
  ScanLineList scanlines;    // header node
};


// Here is a struct to help with storage allocation
// so we can allocate a big chunk at a time, and then take
// pieces from this heap when we need to.
#define SLLSPERBLOCK 100

struct ScanLineListBlock
{
  ScanLineList SLLs[SLLSPERBLOCK];
  ScanLineListBlock* next;
};

// a few macros for the inner loops of the fill code where
// performance considerations don't allow a procedure call.
//
// Evaluate the given edge at the given scanline.
// If the edge has expired, then we leave it and fix up
// the active edge table; otherwise, we increment the
// x value to be ready for the next scanline.
// The winding number rule is in effect, so we must notify
// the caller when the edge has been removed so he
// can reorder the Winding Active Edge Table.
#define EVALUATEEDGEWINDING(pAET, pPrevAET, y, fixWAET) \
{ \
  if (pAET->ymax == y) \
  { \
    /* leaving this edge */ \
    pPrevAET->next = pAET->next; \
    pAET = pPrevAET->next; \
    fixWAET = 1; \
    if (pAET) pAET->back = pPrevAET; \
  } \
  else \
  { \
    BRESINCRPGONSTRUCT(pAET->bres); \
    pPrevAET = pAET; \
    pAET = pAET->next; \
  } \
}

// Evaluate the given edge at the given scanline.
// If the edge has expired, then we leave it and fix up
// the active edge table; otherwise, we increment the
// x value to be ready for the next scanline.
// The even-odd rule is in effect.
#define EVALUATEEDGEEVENODD(pAET, pPrevAET, y) \
{ \
  if (pAET->ymax == y) \
  { \
    /* leaving this edge */ \
    pPrevAET->next = pAET->next; \
    pAET = pPrevAET->next; \
    if (pAET) pAET->back = pPrevAET; \
  } \
  else \
  { \
    BRESINCRPGONSTRUCT(pAET->bres); \
    pPrevAET = pAET; \
    pAET = pAET->next; \
  } \
}

// Number of points to buffer before sending them off to scanlines() :
// Must be an even number.
#define NUMPTSTOBUFFER 16384

// Used to allocate buffers for points and link the buffers together.
struct IntPointBlock
{
  IntPoint pts[NUMPTSTOBUFFER];
  IntPointBlock* next;
};

#define LARGE_COORDINATE INT32_MAX
#define SMALL_COORDINATE INT32_MIN

// Insert the given edge into the edge table. First we must find the correct
// bucket in the Edge table, then find the right slot in the bucket. Finally,
// we can insert it.
static bool insertEdgeInET(
  EdgeTable *ET,
  EdgeTableEntry* ETE,
  int scanline,
  ScanLineListBlock* *SLLBlock,
  sysuint_t *iSLLBlock)
{
  EdgeTableEntry* start, *prev;
  ScanLineList* pSLL, *pPrevSLL;
  ScanLineListBlock* tmpSLLBlock;

  // Find the right bucket to put the edge into.
  pPrevSLL = &ET->scanlines;
  pSLL = pPrevSLL->next;
  while (pSLL && (pSLL->scanline < scanline))
  {
    pPrevSLL = pSLL;
    pSLL = pSLL->next;
  }

  // Reassign pSLL (pointer to ScanLineList) if necessary.
  if ((!pSLL) || (pSLL->scanline > scanline))
  {
    if (*iSLLBlock > SLLSPERBLOCK-1)
    {
      tmpSLLBlock = (ScanLineListBlock* )Memory::alloc(sizeof(ScanLineListBlock));
      if (!tmpSLLBlock) return false;
      (*SLLBlock)->next = tmpSLLBlock;
      tmpSLLBlock->next = (ScanLineListBlock* )NULL;
      *SLLBlock = tmpSLLBlock;
      *iSLLBlock = 0;
    }
    pSLL = &((*SLLBlock)->SLLs[(*iSLLBlock)++]);

    pSLL->next = pPrevSLL->next;
    pSLL->edgelist = (EdgeTableEntry*)NULL;
    pPrevSLL->next = pSLL;
  }
  pSLL->scanline = scanline;

  // Now insert the edge in the right bucket.
  prev = (EdgeTableEntry* )NULL;
  start = pSLL->edgelist;

  while (start && (start->bres.minor_axis < ETE->bres.minor_axis))
  {
    prev = start;
    start = start->next;
  }
  ETE->next = start;

  if (prev)
    prev->next = ETE;
  else
    pSLL->edgelist = ETE;

  return true;
}

// This routine creates the edge table for scan converting polygons.
// The Edge Table (ET) looks like:
//
// EdgeTable
//  --------
// |  ymax  |        ScanLineLists
// |scanline|-->------------>-------------->...
//  --------   |scanline|   |scanline|
//             |edgelist|   |edgelist|
//             ---------    ---------
//                 |             |
//                 |             |
//                 V             V
//           list of ETEs   list of ETEs
//
// where ETE is an EdgeTableEntry data structure, and there is one ScanLineList
// per scanline at which an edge is initially entered.
static bool createETandAET(
  const sysuint_t *Count,
  sysuint_t Polygons,
  const IntPoint* pts,
  EdgeTable *ET,
  EdgeTableEntry* AET,
  EdgeTableEntry* pETEs,
  ScanLineListBlock* pSLLBlock
)
{
  const IntPoint* top, *bottom;
  const IntPoint* PrevPt, *CurrPt, *EndPt;
  sysuint_t poly, count;
  sysuint_t iSLLBlock = 0;
  int dy;

  // Initialize the Active Edge Table.
  AET->next = (EdgeTableEntry* )NULL;
  AET->back = (EdgeTableEntry* )NULL;
  AET->nextWETE = (EdgeTableEntry* )NULL;
  AET->bres.minor_axis = SMALL_COORDINATE;

  // Initialize the Edge Table.
  ET->scanlines.next = (ScanLineList* )NULL;
  ET->ymax = SMALL_COORDINATE;
  ET->ymin = LARGE_COORDINATE;
  pSLLBlock->next = (ScanLineListBlock* )NULL;

  EndPt = pts - 1;
  for (poly = 0; poly < Polygons; poly++)
  {
    count = Count[poly];
    EndPt += count;
    if (count < 2) continue;

    PrevPt = EndPt;

    // For each vertex in the array of points. In this loop we are dealing 
    // with two vertices at a time -- these make up one edge of the polygon.
    while (count--)
    {
      CurrPt = pts++;

      // Find out which point is above and which is below.
      if (PrevPt->getY() > CurrPt->getY())
      {
        bottom = PrevPt; top = CurrPt;
        pETEs->ClockWise = 0;
      }
      else
      {
        bottom = CurrPt; top = PrevPt;
        pETEs->ClockWise = 1;
      }

      // Don't add horizontal edges to the Edge table.
      if (bottom->getY() != top->getY())
      {
        // -1 so we don't get last scanline.
        pETEs->ymax = bottom->getY()-1;

        // Initialize integer edge algorithm.
        dy = bottom->getY() - top->getY();
        BRESINITPGONSTRUCT(dy, top->getX(), bottom->getX(), pETEs->bres);

        if (!insertEdgeInET(ET, pETEs, top->getY(), &pSLLBlock, &iSLLBlock)) return false;

        if (PrevPt->getY() > ET->ymax) ET->ymax = PrevPt->getY();
        if (PrevPt->getY() < ET->ymin) ET->ymin = PrevPt->getY();
        pETEs++;
      }

      PrevPt = CurrPt;
    }
  }
  return true;
}

// This routine moves EdgeTableEntries from the EdgeTable into the Active Edge
// Table, leaving them sorted by smaller x coordinate.
static void loadAET(EdgeTableEntry* AET, EdgeTableEntry* ETEs)
{
  EdgeTableEntry* pPrevAET;
  EdgeTableEntry* tmp;

  pPrevAET = AET;
  AET = AET->next;
  while (ETEs)
  {
    while (AET && (AET->bres.minor_axis < ETEs->bres.minor_axis))
    {
      pPrevAET = AET;
      AET = AET->next;
    }
    tmp = ETEs->next;
    ETEs->next = AET;
    if (AET) AET->back = ETEs;
    ETEs->back = pPrevAET;
    pPrevAET->next = ETEs;
    pPrevAET = ETEs;

    ETEs = tmp;
  }
}

// This routine links the AET by the nextWETE (winding EdgeTableEntry) link for
// use by the winding number rule.  The final Active Edge Table (AET) might 
// look something like:
//
// AET
// ----------  ---------   ---------
// |ymax    |  |ymax    |  |ymax    |
// | ...    |  |...     |  |...     |
// |next    |->|next    |->|next    |->...
// |nextWETE|  |nextWETE|  |nextWETE|
// ---------   ---------   ^--------
//     |                   |       |
//     V------------------->       V---> ...
static void computeWAET(EdgeTableEntry* AET)
{
  EdgeTableEntry* pWETE;
  int inside = 1;
  int isInside = 0;

  AET->nextWETE = (EdgeTableEntry* )NULL;
  pWETE = AET;
  AET = AET->next;

  while (AET)
  {
    if (AET->ClockWise)
      isInside++;
    else
      isInside--;

    if ((!inside && !isInside) || ( inside &&  isInside))
    {
      pWETE->nextWETE = AET;
      pWETE = AET;
      inside = !inside;
    }
    AET = AET->next;
  }
  pWETE->nextWETE = (EdgeTableEntry* )NULL;
}

// Just a simple insertion sort using pointers and back pointers to sort 
// the Active Edge Table.
static bool insertionSort(EdgeTableEntry* AET)
{
  EdgeTableEntry* pETEchase;
  EdgeTableEntry* pETEinsert;
  EdgeTableEntry* pETEchaseBackTMP;
  bool changed = false;

  AET = AET->next;
  while (AET)
  {
    pETEinsert = AET;
    pETEchase = AET;
    while (pETEchase->back->bres.minor_axis > AET->bres.minor_axis)
    {
      pETEchase = pETEchase->back;
    }

    AET = AET->next;
    if (pETEchase != pETEinsert)
    {
      pETEchaseBackTMP = pETEchase->back;
      pETEinsert->back->next = AET;
      if (AET) AET->back = pETEinsert->back;
      pETEinsert->next = pETEchase;
      pETEchase->back->next = pETEinsert;
      pETEchase->back = pETEinsert;
      pETEinsert->back = pETEchaseBackTMP;
      changed = true;
    }
  }
  return changed;
}

// Clean up our act.
static void freeStorage(ScanLineListBlock* pSLLBlock)
{
  ScanLineListBlock* tmpSLLBlock;

  while (pSLLBlock)
  {
    tmpSLLBlock = pSLLBlock->next;
    Memory::free(pSLLBlock);
    pSLLBlock = tmpSLLBlock;
  }
}

// Create an array of rectangles from a list of points.
static err_t ptsToRegion(Region* self,
  sysuint_t numFullPtBlocks, sysuint_t icurPtBlock,
  IntPointBlock* firstPtBlock)
{
  err_t err = self->prepare(((numFullPtBlocks * NUMPTSTOBUFFER) + icurPtBlock) >> 1);
  if (err) return err;

  IntPointBlock* curPtBlock = firstPtBlock;
  IntBox* rects = self->_d->rects - 1;
  sysuint_t length = 0;

  int extentsX1 = LARGE_COORDINATE;
  int extentsX2 = SMALL_COORDINATE;

  for (; numFullPtBlocks != sysuint_t(-1); numFullPtBlocks--)
  {
    // the loop uses 2 points per iteration
    IntPoint* pts;
    int i = NUMPTSTOBUFFER >> 1;

    if (!numFullPtBlocks) i = icurPtBlock >> 1;
    for (pts = curPtBlock->pts; i--; pts += 2)
    {
      if (pts[0].getX() == pts[1].getX()) continue;
      if (length &&
          pts[0].getX() == rects->getX1() &&
          pts[0].getY() == rects->getY2() &&
          pts[1].getX() == rects->getX2() &&
          (length == 1 || rects[-1].getY1() != rects->getY1()) &&
          (i && pts[2].getY() > pts[1].getY()) )
      {
        rects->setY2(pts[1].getY() + 1);
        continue;
      }
      length++;
      rects++;
      rects->setX1(pts[0].getX());
      rects->setY1(pts[0].getY());
      rects->setX2(pts[1].getX());
      rects->setY2(pts[1].getY() + 1);
      if (rects->getX1() < extentsX1) extentsX1 = rects->getX1();
      if (rects->getX2() > extentsX2) extentsX2 = rects->getX2();
    }
    curPtBlock = curPtBlock->next;
  }

  if (length)
    self->_d->extents.set(extentsX1, self->_d->rects[0].getY1(), extentsX2, rects->getY2());
  else
    self->_d->extents.clear();

  self->_d->length = length;
  return ERR_OK;
}

err_t Region::polygon(const IntPoint* pts, sysuint_t count, uint fillRule)
{
  return polyPolygon(pts, &count, 1, fillRule);
}

err_t Region::polyPolygon(const IntPoint* src, const sysuint_t *count, sysuint_t polygons, uint fillRule)
{
  EdgeTableEntry *pAET;            // Active Edge Table
  int y;                           // current scanline
  int iPts = 0;                    // number of pts in buffer
  EdgeTableEntry *pWETE;           // Winding Edge Table Entry
  ScanLineList *pSLL;              // current scanLineList
  IntPoint *pts;                   // output buffer
  EdgeTableEntry *pPrevAET;        // ptr to previous AET
  EdgeTable ET;                    // header node for ET
  EdgeTableEntry AET;              // header node for AET
  EdgeTableEntry *pETEs;           // EdgeTableEntries pool
  ScanLineListBlock SLLBlock;      // header for scanlinelist
  IntPointBlock firstPtBlock, *curPtBlock; // PtBlock buffers
  IntPointBlock *tmpPtBlock;
  sysuint_t numFullPtBlocks = 0;
  sysuint_t poly, total;
  bool fixWAET = false;
  err_t err = ERR_OK;

  // Checks.
  if (!count)
  {
    clear();
    return ERR_OK;
  }

  if (!src || !polygons) 
  {
    clear();
    return ERR_RT_INVALID_ARGUMENT;
  }

  // Special case - Rectangle
  if (polygons == 1 &&
      (*count == 4 ||
      (*count == 5 && src[4].getX() == src[0].getX() && src[4].getY() == src[0].getY())) &&
      ( (src[0].getY() == src[1].getY() &&
         src[1].getX() == src[2].getX() &&
         src[2].getY() == src[3].getY() &&
         src[3].getX() == src[0].getX()) ||
        (src[0].getX() == src[1].getX() &&
         src[1].getY() == src[2].getY() &&
         src[2].getX() == src[3].getX() &&
         src[3].getY() == src[0].getY()) ))
  {
    return set(IntBox(
      Math::min(src[0].getX(), src[2].getX()),
      Math::min(src[0].getY(), src[2].getY()),
      Math::max(src[0].getX(), src[2].getX()) + 1,
      Math::max(src[0].getY(), src[2].getY()) + 1));
  }

  for (poly = total = 0; poly < polygons; poly++) total += count[poly];

  pETEs = (EdgeTableEntry *)Memory::alloc(sizeof(EdgeTableEntry) * total);
  if (!pETEs) { clear(); return ERR_RT_OUT_OF_MEMORY; }

  pts = firstPtBlock.pts;
  if (!createETandAET(count, polygons, src, &ET, &AET, pETEs, &SLLBlock)) goto outOfMemory;
  pSLL = ET.scanlines.next;
  curPtBlock = &firstPtBlock;

  if (fillRule != FILL_NON_ZERO)
  {
    // For each scanline.
    for (y = ET.ymin; y < ET.ymax; y++)
    {
      // Add a new edge to the active edge table when we
      // get to the next edge.
      if (pSLL != NULL && y == pSLL->scanline)
      {
        loadAET(&AET, pSLL->edgelist);
        pSLL = pSLL->next;
      }
      pPrevAET = &AET;
      pAET = AET.next;

      // For each active edge.
      while (pAET)
      {
        pts->set(pAET->bres.minor_axis, y);
        pts++;
        iPts++;

        // send out the buffer.
        if (iPts == NUMPTSTOBUFFER)
        {
          tmpPtBlock = (IntPointBlock *)Memory::alloc(sizeof(IntPointBlock));
          if (!tmpPtBlock) goto outOfMemory;
          curPtBlock->next = tmpPtBlock;
          curPtBlock = tmpPtBlock;
          pts = curPtBlock->pts;
          numFullPtBlocks++;
          iPts = 0;
        }
        EVALUATEEDGEEVENODD(pAET, pPrevAET, y);
      }
      insertionSort(&AET);
    }
  }
  // fillRule == FillNonZero
  else
  {
    // For each scanline.
    for (y = ET.ymin; y < ET.ymax; y++)
    {
      // Add a new edge to the active edge table when we
      // get to the next edge.
      if (pSLL != NULL && y == pSLL->scanline)
      {
        loadAET(&AET, pSLL->edgelist);
        computeWAET(&AET);
        pSLL = pSLL->next;
      }
      pPrevAET = &AET;
      pAET = AET.next;
      pWETE = pAET;

      // For each active edge.
      while (pAET)
      {
        // add to the buffer only those edges that
        // are in the Winding active edge table.
        if (pWETE == pAET)
        {
          pts->set(pAET->bres.minor_axis, y);
          pts++;
          iPts++;

          // Send out the buffer.
          if (iPts == NUMPTSTOBUFFER)
          {
            tmpPtBlock = (IntPointBlock *)Memory::alloc(sizeof(IntPointBlock));
            if (!tmpPtBlock) goto outOfMemory;
            curPtBlock->next = tmpPtBlock;
            curPtBlock = tmpPtBlock;
            pts = curPtBlock->pts;
            numFullPtBlocks++;
            iPts = 0;
          }
          pWETE = pWETE->nextWETE;
        }
        EVALUATEEDGEWINDING(pAET, pPrevAET, y, fixWAET);
      }

      // Recompute the winding active edge table if
      // we just resorted or have exited an edge.
      if (insertionSort(&AET) || fixWAET)
      {
        computeWAET(&AET);
        fixWAET = false;
      }
    }
  }

end:
  freeStorage(SLLBlock.next);
  if (!err) ptsToRegion(this, numFullPtBlocks, iPts, &firstPtBlock);

  for (curPtBlock = firstPtBlock.next; --numFullPtBlocks != sysuint_t(-1);)
  {
    tmpPtBlock = curPtBlock->next;
    Memory::free(curPtBlock);
    curPtBlock = tmpPtBlock;
  }

  Memory::free(pETEs);
  return err;

outOfMemory:
  err = ERR_RT_OUT_OF_MEMORY;
  goto end;
}

} // Fog namespace
