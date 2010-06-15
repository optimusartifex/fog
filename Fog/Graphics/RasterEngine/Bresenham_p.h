// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERENGINE_BRESENHAM_P_H
#define _FOG_GRAPHICS_RASTERENGINE_BRESENHAM_P_H

// [Dependencies]
#include <Fog/Graphics/RasterEngine/C_p.h>

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::Raster - BresenhamLineIterator]
// ============================================================================

//! @internal
//!
//! @brief Class that's used by software painters to draw aliased line using
//! bresenham algorithm.
//!
//! This class is internal and it adventage is that is can clip line using
//! only arithetic (no floating point), so it can be used together with MMX
//! or 3dNow! instructions.
//!
//! Use @c init() or @c initAndClip() to initialize variables. If
//! @c initAndClip() will return @c false then variables are not set correctly
//! and using of this structure is not defined (line is totally clipped out).
//!
//! For horizontal oriented lines must be used functions with 'h' prefix, for
//! vertical oriented lines 'v' prefix.
//!
//! Use @c hInc() and @c vInc to iterate through line, use @c hDone() and
//! @c vDone() to check if iterator is at end point (the point given in
//! @c init - x2 y2).
struct FOG_HIDDEN BresenhamLineIterator
{
  int x, dx, incx, endx;
  int y, dy, incy, endy;
  int dc;
  uint32_t clip;

  enum CLIP
  {
    CLIP_LX1 = 0x0001,
    CLIP_LX2 = 0x0002,
    CLIP_LXC = 0x0003,
    CLIP_RX1 = 0x0004,
    CLIP_RX2 = 0x0008,
    CLIP_RXC = 0x000C,
    CLIP_TY1 = 0x0010,
    CLIP_TY2 = 0x0020,
    CLIP_TYC = 0x0030,
    CLIP_BY1 = 0x0040,
    CLIP_BY2 = 0x0080,
    CLIP_BYC = 0x00C0
  };

  void initNoClip(int x1, int y1, int x2, int y2)
  {
    x = x1;
    y = y1;

    dx = x2 - x1; incx = 1; if (dx < 0) { incx = -1; dx = -dx; }
    dy = y2 - y1; incy = 1; if (dy < 0) { incy = -1; dy = -dy; }

    endx = x2;
    endy = y2;

    if (hOriented())
      dc = dx >> 1;
    else
      dc = dy >> 1;
  }

  bool initAndClip(int x1, int y1, int x2, int y2, int clip_x1, int clip_y1, int clip_x2, int clip_y2)
  {
    clip = 0;

    if (x1 < clip_x1) clip |= CLIP_LX1;
    if (x2 < clip_x1) clip |= CLIP_LX2;
    if (y1 < clip_y1) clip |= CLIP_TY1;
    if (y2 < clip_y1) clip |= CLIP_TY2;

    if (x1 > clip_x2) clip |= CLIP_RX1;
    if (x2 > clip_x2) clip |= CLIP_RX2;
    if (y1 > clip_y2) clip |= CLIP_BY1;
    if (y2 > clip_y2) clip |= CLIP_BY2;

    if ((clip & (CLIP_LXC)) == CLIP_LXC) return false;
    if ((clip & (CLIP_RXC)) == CLIP_RXC) return false;
    if ((clip & (CLIP_TYC)) == CLIP_TYC) return false;
    if ((clip & (CLIP_BYC)) == CLIP_BYC) return false;

    initNoClip(x1, y1, x2, y2);

    // If there is nothing to clip, return now
    if (clip == 0) return true;

    int max = -1;
    int n = -1;

    // clip horizontal oriented line.
    if (hOriented())
    {
      if (incx > 0)
      {
        if (clip_x1 > endx) return false;
        if (clip_x1 > x) hInc(clip_x1 - x);
      }
      else
      {
        if (clip_x2 < endx) return false;
        if (clip_x2 < x) hInc(x - clip_x2);
      }

      if (hDone()) return false;

      if (incy > 0)
      {
        while (y < clip_y1)
        {
          hInc();
          if (hDone()) return false;
        }
        if (endy > clip_y2)
        {
          max = clip_y2 - y;
          if (max < 0) return false;
        }
      }
      else
      {
        while (y > clip_y2)
        {
          hInc();
          if (hDone()) return false;
        }
        if (endy < clip_y1)
        {
          max = y - clip_y1;
          if (max < 0) return false;
        }
      }

      if (max >= 0) n = (int)( ((int64_t)dc + ((int64_t)max * dx)) / dy );

      if (incx > 0)
      {
        if (n >= 0) endx = x + n;
        if (endx > clip_x2) endx = clip_x2;
        if (endx <= x) return false;
      }
      else
      {
        if (n >= 0) endx = x - n;
        if (endx < clip_x1) endx = clip_x1;
        if (endx >= x) return false;
      }
    }
    // clip vertical oriented line
    else
    {
      if (incy > 0)
      {
        if (clip_y1 > endy) return false;
        if (clip_y1 > y) vInc(clip_y1 - y);
      }
      else
      {
        if (clip_y2 < endy) return false;
        if (clip_y2 < y) vInc(y - clip_y2);
      }

      if (vDone()) return false;

      if (incx > 0)
      {
        while (x < clip_x1)
        {
          vInc();
          if (vDone()) return false;
        }
        if (endx > clip_x2)
        {
          max = clip_x2 - x;
          if (max < 0) return false;
        }
      }
      else
      {
        while (x > clip_x2)
        {
          vInc();
          if (vDone()) return false;
        }
        if (endx < clip_x1)
        {
          max = x - clip_x1;
          if (max < 0) return false;
        }
      }

      if (max >= 0) n = (int)( ((int64_t)dc + ((int64_t)max * dy)) / dx );

      if (incy > 0)
      {
        if (n >= 0) endy = y + n;
        if (endy > clip_y2) endy = clip_y2;
        if (endy <= y) return false;
      }
      else
      {
        if (n >= 0) endy = y - n;
        if (endy < clip_y1) endy = clip_y1;
        if (endy >= y) return false;
      }
    }

    return true;
  }

  FOG_INLINE bool initAndClip(int x1, int y1, int x2, int y2, const IntBox& clip)
  {
    return initAndClip(x1, y1, x2, y2, clip.getX1(), clip.getY1(), clip.getX2()-1, clip.getY2()-1);
  }

  FOG_INLINE bool initAndClip(int x1, int y1, int x2, int y2, const IntRect& clip)
  {
    return initAndClip(x1, y1, x2, y2, clip.getX1(), clip.getY1(), clip.getX2()-1, clip.getY2()-1);
  }

  // [Horizontal oriented line functions]

  FOG_INLINE bool hOriented() const { return dx >= dy; }

  FOG_INLINE void hInc()
  {
    x += incx;
    if ((dc -= dy) <= 0) { y += incy; dc += dx; }
  }

  FOG_INLINE void hIncPtr(uint8_t** _p, sysint_t _pIncX, sysint_t _pIncY)
  {
    x += incx; *_p += _pIncX;
    if ((dc -= dy) <= 0) { y += incy; *_p += _pIncY; dc += dx; }
  }

  FOG_INLINE void hInc(int i)
  {
    x += incx * i;
    int64_t xc = (int64_t)dc - (int64_t)i * (int64_t)dy;

    if (xc <= 0)
    {
      int d = 1 - (int)(xc / dx);
      y += incy * d;
      dc = (int)(xc % dx) + dx;
    }
    else
      dc = (int)xc;
  }

  FOG_INLINE bool hDone() const { return x == endx; }

  // [Vertical oriented line functions]

  FOG_INLINE bool vOriented() const { return dx < dy; }

  FOG_INLINE void vInc()
  {
    y += incy;
    if ((dc -= dx) <= 0) { x += incx; dc += dy; }
  }

  FOG_INLINE void vIncPtr(uint8_t** _p, sysint_t _pIncX, sysint_t _pIncY)
  {
    y += incy; *_p += _pIncY;
    if ((dc -= dx) <= 0) { x += incx; *_p += _pIncX; dc += dy; }
  }

  FOG_INLINE void vInc(int i)
  {
    y += incy * i;

    int64_t xc = (int64_t)dc - (int64_t)i * (int64_t)dx;
    if (xc <= 0)
    {
      int d = 1 - (int)(xc / dy);
      x += incx * d;
      dc = (int)(xc % dy) + dy;
    }
    else
      dc = (int)xc;
  }

  FOG_INLINE bool vDone() const { return y == endy; }
};

} // RasterEngine namespace
} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_RASTERENGINE_BRESENHAM_P_H
