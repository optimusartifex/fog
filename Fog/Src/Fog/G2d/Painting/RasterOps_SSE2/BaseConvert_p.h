// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_SSE2_BASECONVERT_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_SSE2_BASECONVERT_P_H

// [Dependencies - RasterOps_C]
#include <Fog/G2d/Painting/RasterOps_C/BaseConvert_p.h>

// [Dependencies - RasterOps_SSE2]
#include <Fog/G2d/Painting/RasterOps_SSE2/BaseDefs_p.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/CompositeBase_p.h>

namespace Fog {
namespace RasterOps_SSE2 {

// ============================================================================
// [Fog::RasterOps_SSE2 - Convert]
// ============================================================================

struct FOG_NO_EXPORT Convert
{
  // ==========================================================================
  // [Fill - 8]
  // ==========================================================================

  static void FOG_FASTCALL fill_8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSUME(w > 0);

    if (w < 16)
    {
      uint8_t p0 = 0xFFU;
      do {
        *dst++ = p0;
      } while (--w);
    }
    else
    {
      __m128i xmm0;
      uint8_t* mark = dst;

      Acc::m128iFill(xmm0);
      Acc::m128iStore16u(dst, xmm0);

      dst = (uint8_t*)( (uintptr_t(dst) + 15) & ~(uintptr_t)15 );
      w -= (int)(size_t)(dst - mark);

      while ((w -= 64) >= 0)
      {
        Acc::m128iStore16a(dst +  0, xmm0);
        Acc::m128iStore16a(dst + 16, xmm0);
        Acc::m128iStore16a(dst + 32, xmm0);
        Acc::m128iStore16a(dst + 48, xmm0);
        dst += 64;
      }

      w += 64;

      while ((w -= 16) >= 0)
      {
        Acc::m128iStore16a(dst +  0, xmm0);
        dst += 16;
      }

      w += 16;
      Acc::m128iStore16u(dst + (uint)w - 16, xmm0);
    }
  }

  // ==========================================================================
  // [Fill - 16]
  // ==========================================================================

  static void FOG_FASTCALL fill_16(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    FOG_ASSUME(w > 0);
    FOG_ASSERT_ALIGNED(dst, 2);

    if (w < 8)
    {
      uint32_t p0 = 0xFFFFFFFFU;
      do {
        Acc::p32Store2a(dst, p0);
        dst += 2;
      } while (--w);
    }
    else
    {
      __m128i xmm0;
      uint8_t* mark = dst;

      Acc::m128iFill(xmm0);
      Acc::m128iStore16u(dst, xmm0);

      dst = (uint8_t*)( (uintptr_t(dst) + 15) & ~(uintptr_t)15 );
      w -= (int)(size_t)(dst - mark) >> 1;

      while ((w -= 32) >= 0)
      {
        Acc::m128iStore16a(dst +  0, xmm0);
        Acc::m128iStore16a(dst + 16, xmm0);
        Acc::m128iStore16a(dst + 32, xmm0);
        Acc::m128iStore16a(dst + 48, xmm0);
        dst += 64;
      }

      w += 32;

      while ((w -= 8) >= 0)
      {
        Acc::m128iStore16a(dst +  0, xmm0);
        dst += 16;
      }

      w += 8;
      Acc::m128iStore16u(dst + (uint)w * 2 - 16, xmm0);
    }
  }
};

} // RasterOps_SSE2 namespace
} // Fog namespace

#endif // _FOG_G2D_PAINTING_RASTEROPS_SSE2_BASECONVERT_P_H
