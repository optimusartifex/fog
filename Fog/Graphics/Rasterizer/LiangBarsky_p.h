// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERIZER_LIANGBARSKY_P_H
#define _FOG_GRAPHICS_RASTERIZER_LIANGBARSKY_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Math.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::LiangBarsky]
// ============================================================================

//! @internal
//!
//! @brief Liang-Barsky clipping.
struct FOG_HIDDEN LiangBarsky
{
  // Clipping flags.
  enum FLAGS
  {
    CLIPPED_X1 = 0x4,
    CLIPPED_X2 = 0x1,
    CLIPPED_Y1 = 0x8,
    CLIPPED_Y2 = 0x2,
    CLIPPED_X  = CLIPPED_X1 | CLIPPED_X2,
    CLIPPED_Y  = CLIPPED_Y1 | CLIPPED_Y2
  };

  //----------------------------------------------------------clipping_flags
  // Determine the clipping code of the vertex according to the 
  // Cyrus-Beck line clipping algorithm
  //
  //        |        |
  //  0110  |  0010  | 0011
  //        |        |
  // -------+--------+-------- clipBox.y2
  //        |        |
  //  0100  |  0000  | 0001
  //        |        |
  // -------+--------+-------- clipBox.y1
  //        |        |
  //  1100  |  1000  | 1001
  //        |        |
  //  clipBox.x1  clipBox.x2
  //
  // 
  template<typename T, typename BoxT>
  static FOG_INLINE uint getClippingFlags(T x, T y, const BoxT& clipBox)
  {
    return (x > clipBox.x2) | ((y > clipBox.y2) << 1) | ((x < clipBox.x1) << 2) | ((y < clipBox.y1) << 3);
  }

  template<typename T, typename BoxT>
  static FOG_INLINE uint getClippingFlagsX(T x, const BoxT& clipBox)
  {
    return (x > clipBox.x2) | ((x < clipBox.x1) << 2);
  }

  template<typename T, typename BoxT>
  static FOG_INLINE uint getClippingFlagsY(T y, const BoxT& clipBox)
  {
    return ((y > clipBox.y2) << 1) | ((y < clipBox.y1) << 3);
  }

  // Clip LiangBarsky.
  template<typename T, typename BoxT>
  static FOG_INLINE uint clipLiangBarsky(T x1, T y1, T x2, T y2, const BoxT& clipBox,
    T* FOG_RESTRICT x, T* FOG_RESTRICT y)
  {
    const double nearzero = 1e-30;

    double deltax = x2 - x1, xin, xout, tinx, toutx;
    double deltay = y2 - y1, yin, yout, tiny, touty;
    double tin1;
    double tin2;
    double tout1;
    uint np = 0;

    // Bump off of the vertical.
    if (deltax == 0.0) deltax = (x1 > clipBox.x1) ? -nearzero : nearzero;
    // Bump off of the horizontal.
    if (deltay == 0.0) deltay = (y1 > clipBox.y1) ? -nearzero : nearzero;

    if (deltax > 0.0) 
    {                
      // Points to right.
      xin  = clipBox.x1;
      xout = clipBox.x2;
    }
    else 
    {
      xin  = clipBox.x2;
      xout = clipBox.x1;
    }

    if (deltay > 0.0) 
    {
      // Points up.
      yin  = clipBox.y1;
      yout = clipBox.y2;
    }
    else 
    {
      yin  = clipBox.y2;
      yout = clipBox.y1;
    }
    
    tinx = (xin - x1) / deltax;
    tiny = (yin - y1) / deltay;
    
    if (tinx < tiny) 
    {
      // Hits X first.
      tin1 = tinx;
      tin2 = tiny;
    }
    else
    {
      // Hits Y first.
      tin1 = tiny;
      tin2 = tinx;
    }
    
    if (tin1 <= 1.0) 
    {
      if (tin1 > 0.0) 
      {
        *x++ = (T)xin;
        *y++ = (T)yin;
        ++np;
      }

      if (tin2 <= 1.0)
      {
        toutx = (xout - x1) / deltax;
        touty = (yout - y1) / deltay;
        
        tout1 = (toutx < touty) ? toutx : touty;
        
        if (tin2 > 0.0 || tout1 > 0.0) 
        {
          if (tin2 <= tout1) 
          {
            if (tin2 > 0.0) 
            {
              if (tinx > tiny) 
              {
                *x++ = (T)xin;
                *y++ = (T)(y1 + tinx * deltay);
              }
              else 
              {
                *x++ = (T)(x1 + tiny * deltax);
                *y++ = (T)yin;
              }
              ++np;
            }

            if (tout1 < 1.0) 
            {
              if (toutx < touty) 
              {
                *x++ = (T)xout;
                *y++ = (T)(y1 + toutx * deltay);
              }
              else 
              {
                *x++ = (T)(x1 + touty * deltax);
                *y++ = (T)yout;
              }
            }
            else 
            {
              *x++ = x2;
              *y++ = y2;
            }
            ++np;
          }
          else 
          {
            if (tinx > tiny) 
            {
              *x++ = (T)xin;
              *y++ = (T)yout;
            }
            else 
            {
              *x++ = (T)xout;
              *y++ = (T)yin;
            }
            ++np;
          }
        }
      }
    }
    return np;
  }

  // Move point.
  template<typename T, typename BoxT>
  static FOG_INLINE bool clipMovePoint(T x1, T y1, T x2, T y2, const BoxT& clipBox,
    T* FOG_RESTRICT x,
    T* FOG_RESTRICT y, uint flags)
  {
    if (flags & CLIPPED_X)
    {
      if (x1 == x2) return false;

      T bound = (flags & CLIPPED_X1) ? clipBox.x1 : clipBox.x2;
      *y = (T)(double(bound - x1) * (y2 - y1) / (x2 - x1) + y1);
      *x = bound;
    }

    flags = getClippingFlagsY(*y, clipBox);

    if (flags & CLIPPED_Y)
    {
      if (y1 == y2) return false;

      T bound = (flags & CLIPPED_Y1) ? clipBox.y1 : clipBox.y2;
      *x = (T)(double(bound - y1) * (x2 - x1) / (y2 - y1) + x1);
      *y = bound;
    }

    return true;
  }

  // Clip line segment.
  //
  // Returns:
  //   (ret    ) >= 4  - Fully clipped
  //   (ret & 1) != 0  - First point has been moved
  //   (ret & 2) != 0  - Second point has been moved
  template<typename T, typename BoxT>
  static FOG_INLINE unsigned clipLineSegment(
    T* FOG_RESTRICT x1, T* FOG_RESTRICT y1,
    T* FOG_RESTRICT x2, T* FOG_RESTRICT y2, const BoxT& clipBox)
  {
    uint f1 = getClippingFlags(*x1, *y1, clipBox);
    uint f2 = getClippingFlags(*x2, *y2, clipBox);
    uint ret = 0;

    // Fully visible
    if ((f2 | f1) == 0) return 0;

    // Fully clipped
    if ((f1 & CLIPPED_X) != 0 && (f1 & CLIPPED_X) == (f2 & CLIPPED_X)) return 4;
    if ((f1 & CLIPPED_Y) != 0 && (f1 & CLIPPED_Y) == (f2 & CLIPPED_Y)) return 4;

    T tx1 = *x1;
    T ty1 = *y1;
    T tx2 = *x2;
    T ty2 = *y2;

    if (f1)
    {
      if (!clipMovePoint(tx1, ty1, tx2, ty2, clipBox, x1, y1, f1)) return 4;
      if (*x1 == *x2 && *y1 == *y2) return 4;
      ret |= 1;
    }

    if (f2)
    {
      if (!clipMovePoint(tx1, ty1, tx2, ty2, clipBox, x2, y2, f2)) return 4;
      if (*x1 == *x2 && *y1 == *y2) return 4;
      ret |= 2;
    }

    return ret;
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_RASTERIZER_LIANGBARSKY_P_H
