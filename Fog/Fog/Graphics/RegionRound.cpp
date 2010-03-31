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
#include <Fog/Core/Memory.h>
#include <Fog/Core/Std.h>
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
// [Fog::Region - Round]
// ============================================================================

err_t Region::round(const IntRect& r, uint xradius, uint yradius, bool fill)
{
  Data* selfd;

  // Normalize and make coordinates correct
  int x1 = r.getX1();
  int y1 = r.getY1();
  int x2 = r.getX2();
  int y2 = r.getY2();

  uint width;
  uint height;
  uint ewidth;
  uint eheight;
  uint space;
  err_t err;

  if (x1 >= x2 || y1 >= y2)
  {
    clear();
    return ERR_RT_INVALID_ARGUMENT;
  }

  width  = (uint)(x2 - x1);
  height = (uint)(y2 - y1);

  ewidth  = xradius < (UINT_MAX/2) ? (xradius << 1) : (UINT_MAX);
  eheight = yradius < (UINT_MAX/2) ? (yradius << 1) : (UINT_MAX);

  if (ewidth > width) ewidth = width;
  if (eheight > height) eheight = height;

  // Check if we can do a normal rectangle instead.
  if (ewidth < 2 || eheight < 2)
  {
    return set(IntBox(r));
  }

  ewidth  &= ~1;
  eheight &= ~1;

  space = (eheight * 2 + 1);

  if (fill == 0)
  {
    space <<= 1;
    if (space > (height << 1)) space = (height << 1);
  }

  if ((err = prepare(space))) { clear(); return err; }
  selfd = _d;

  IntBox* rectCur = selfd->rects;
  IntBox* reverse; // will be used later

  // Ellipse algorithm, based on an article by K. Porter
  // in DDJ Graphics Programming Column, 8/89

  int asq, asq2;
  int bsq, bsq2;
  int d;
  int xd, yd;

  asq = (int)(ewidth * ewidth / 4);           // a^2
  bsq = (int)(eheight * eheight / 4);         // b^2
  d = bsq - asq * (int)eheight / 2 + asq / 4; // b^2 - a^2b + a^2/4
  xd = 0;
  yd = asq * (int)eheight;                    // 2a^2b

  asq2 = asq * 2;
  bsq2 = bsq * 2;

  int lx1 = x1 + (int)(ewidth >> 1);
  int lx2 = x2 - (int)(ewidth >> 1);

  if (fill)
  {
    // -------------------------------
    // Filled round
    // -------------------------------

    // Loop to draw first half of quadrant.
    while (xd < yd)
    {
      // If nearest pixel is toward the center.
      if (d > 0)
      {
        // Move toward center.
        rectCur->set(lx1, y1, lx2, y1+1);
        rectCur++;

        y1++;
        yd -= asq2;
        d  -= yd;
      }
      // Next horizontal point.
      lx1--;
      lx2++;
      xd += bsq2;
      d  += bsq + xd;
    }

    // Loop to draw second half of quadrant.
    d += (3 * (asq-bsq) / 2 - (xd+yd)) / 2;
    while (yd > 0)
    {
      // Next vertical point.
      if (rectCur != selfd->rects && rectCur[-1].getX1() == lx1)
      {
        rectCur[-1].setY2(y1+1);
      }
      else
      {
        rectCur->set(lx1, y1, lx2, y1+1);
        rectCur++;
      }
      y1++;

      // If nearest pixel is outside ellipse.
      if (d < 0)
      {
        // Move away from center.
        lx1--;
        lx2++;
        xd += bsq2;
        d  += xd;
      }
      yd -= asq2;
      d  += asq - yd;
    }

    // Check...If we hasn't rectangles in region now, we can safe skip some steps.
    if (rectCur == selfd->rects) goto _clear;

    // Remember last rectangle and last height in region (because this height
    // can be increased by rectCur++ call and this can cause larger
    // rectangle than we need.
    reverse = rectCur-1;

    // Add the inside rectangle.
    {
      sysuint_t outside = (sysuint_t)(y1 - r.getY1()) << 1;
      if (outside < height)
      {
        sysuint_t inside = height - outside;
        rectCur->set(lx1, y1, lx2, y1 + inside);
        rectCur++;

        y1 += inside;
      }
    }

    // Next step is very simple, we was computed first symmetric half and
    // we need second.
    do {
      yd = reverse->y2 - reverse->y1;
      rectCur->set(reverse->x1, y1, reverse->x2, y1 + yd); y1 += yd;
      rectCur++;
    } while (reverse-- != selfd->rects);

    selfd->length = rectCur - selfd->rects;
  }
  else
  {
    // -------------------------------
    // Non-filled round
    // -------------------------------
    IntBox* rectEnd = rectCur + space - 1;
    int len = 0;

    // Loop to draw first half of quadrant.
    while (xd < yd)
    {
      // If nearest pixel is toward the center.
      if (d > 0)
      {
        // Move toward center.
        if (rectCur == selfd->rects)
        {
          (*rectCur++).set(lx1, y1, lx2, y1+1);
          (*rectEnd--).set(lx1, y2-1, lx2, y2);
        }
        else
        {
          (*rectCur++).set(lx1, y1, lx1+len, y1+1);
          (*rectCur++).set(lx2-len, y1, lx2, y1+1);
          (*rectEnd--).set(lx2-len, y2-1, lx2, y2);
          (*rectEnd--).set(lx1, y2-1, lx1+len, y2);
        }

        y1++;
        y2--;
        yd -= asq2;
        d  -= yd;
        len = 0;
      }
      // Next horizontal point.
      lx1--;
      lx2++;
      len++;
      xd += bsq2;
      d  += bsq + xd;
    }

    // Loop to draw second half of quadrant.
    d += (3 * (asq-bsq) / 2 - (xd+yd)) / 2;
    while (yd > 0)
    {
      // Next vertical point.
      if (rectCur == selfd->rects)
      {
        (*rectCur++).set(lx1, y1, lx2, y1+1);
        (*rectEnd--).set(lx1, y2-1, lx2, y2);
      }
      else if (rectCur[-1].getX2() == lx2)
      {
        rectCur[-2].setY2(y1+1);
        rectCur[-1].setY2(y1+1);
        rectEnd[1].y1--;
        rectEnd[2].y1--;
      }
      else
      {
        (*rectCur++).set(lx1, y1, lx1+1, y1+1);
        (*rectCur++).set(lx2-1, y1, lx2, y1+1);
        (*rectEnd--).set(lx2-1, y2-1, lx2, y2);
        (*rectEnd--).set(lx1, y2-1, lx1+1, y2);
      }
      y1++;
      y2--;

      // If nearest pixel is outside ellipse.
      if (d < 0)
      {
        // Move away from center.
        lx1--;
        lx2++;
        xd += bsq2;
        d  += xd;
      }
      yd -= asq2;
      d  += asq - yd;
    }

    // Check...If we haven't rectangles in region now, we can safe skip some steps.
    if (rectCur == selfd->rects) goto _clear;

    // Remember last rectangle and last height in region (because this height
    // can be increased by rectCur++ call and this can cause larger
    // rectangle than we need.
    reverse = rectCur-1;

    yd = reverse->y2 - reverse->y1;

    sysuint_t count = (sysuint_t)(rectCur - selfd->rects);

    // Add the inside rectangles
    if (y1 < y2)
    {
      (*rectCur++).set(lx1, y1, lx1+1, y2);
      (*rectCur++).set(lx2-1, y1, lx2, y2);
      y1 = y2;
      memmove(rectCur, rectEnd + 1, sizeof(IntBox) * count);
      selfd->length = (count << 1) + 2;
    }
    else
    {
      memmove(rectCur, rectEnd + 1, sizeof(IntBox) * count);
      selfd->length = (count << 1);
    }
  }
  selfd->extents = r;
  return ERR_OK;

_clear:
  clear();
  return ERR_OK;
}

} // Fog namespace
