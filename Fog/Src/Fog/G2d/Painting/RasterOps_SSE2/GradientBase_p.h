// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_SSE2_GRADIENTBASE_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_SSE2_GRADIENTBASE_P_H

// [Dependencies]
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/BaseDefs_p.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/BaseHelpers_p.h>

namespace Fog {
namespace RasterOps_SSE2 {

// ============================================================================
// [Fog::RasterOps_SSE2 - PGradientBase]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT PGradientBase
{
  // ==========================================================================
  // [Interpolate]
  // ==========================================================================

  static void FOG_FASTCALL interpolate_prgb32(uint8_t* _dst, int _wTotal, const ColorStop* stops, size_t length)
  {
    FOG_ASSUME(length >= 1);

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    uint32_t c0 = stops[0].getArgb32();
    uint32_t c1;

    if (length == 1)
    {
      __m128i pix0;
      Acc::m128iCvtSI128FromSI(pix0, c0);
      Acc::m128iPRGB32FromARGB32Lo_PBB(pix0, pix0);

      do {
        Acc::m128iStore4(_dst, pix0);
        _dst += 4;
      } while (--_wTotal);
      return;
    }

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    _wTotal--;

    uint p0 = 0;
    uint p1;

    float wf = (float)(_wTotal << 8);

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    size_t pos;
    for (pos = 0; pos < length; pos++)
    {
      c1 = stops[pos].getArgb32();
      p1 = Math::uround(stops[pos].getOffset() * wf);

      uint len = (p1 >> 8) - (p0 >> 8);
      uint8_t* dst = _dst + (p0 >> 8) * 4;

      if (len > 0)
      {
        int w = len + 1;

        if (c0 == c1)
        {
          __m128i pix0;
          Acc::m128iCvtSI128FromSI(pix0, c0);
          Acc::m128iPRGB32FromARGB32Lo_PBB(pix0, pix0);

          do {
            Acc::m128iStore4(dst, pix0);
            dst += 4;
          } while (--w);
        }
        else
        {
          __m128i pos0xmm;
          __m128i inc0xmm;
          __m128i msk0xmm;

          xmm_t t;

          Acc::m128iCvtSI128FromSI(pos0xmm, c0);
          Acc::m128iCvtSI128FromSI(inc0xmm, c1);

          Acc::m128iUnpackPI16FromPI8Lo(pos0xmm, pos0xmm);
          Acc::m128iUnpackPI16FromPI8Lo(inc0xmm, inc0xmm);

          Acc::m128iCmpGtPI16(msk0xmm, pos0xmm, inc0xmm);
          Acc::m128iRShiftPU16<8>(msk0xmm, msk0xmm);

          Acc::m128iXor(pos0xmm, pos0xmm, msk0xmm);
          Acc::m128iXor(inc0xmm, inc0xmm, msk0xmm);

          Acc::m128iUnpackPI32FromPI16Lo(pos0xmm, pos0xmm, pos0xmm);
          Acc::m128iUnpackPI32FromPI16Lo(inc0xmm, inc0xmm, inc0xmm);

          Acc::m128iSubPI16(inc0xmm, inc0xmm, pos0xmm);
          Acc::m128iLShiftPU32<24>(pos0xmm, pos0xmm);
          Acc::m128iLShiftPU32<24>(inc0xmm, inc0xmm);

          t.m128i = inc0xmm;

          t.ud[0] /= len;
          t.ud[1] /= len;
          t.ud[2] /= len;
          t.ud[3] /= len;

          Acc::m128iAddPI32(pos0xmm, pos0xmm, FOG_XMM_GET_CONST_PI(0080000000800000_0080000000800000));
          inc0xmm = t.m128i;

          if (Acc::p32ARGB32IsAlphaFF(c0) && Acc::p32ARGB32IsAlphaFF(c1))
          {
            Acc::m128iPackPU8FromPU16(msk0xmm, msk0xmm, msk0xmm);
            Acc::m128iExpandPI32FromSI32(msk0xmm, msk0xmm);

            FOG_BLIT_LOOP_32x4_INIT()

            FOG_BLIT_LOOP_32x4_SMALL_BEGIN(G_NoAlpha)
              __m128i pix0xmm;

              Acc::m128iCopy(pix0xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iRShiftPU32<24>(pix0xmm, pix0xmm);
              Acc::m128iPackPU8FromPI32(pix0xmm, pix0xmm, pix0xmm);
              Acc::m128iXor(pix0xmm, pix0xmm, msk0xmm);

              Acc::m128iStore4(dst, pix0xmm);
              dst += 4;
            FOG_BLIT_LOOP_32x4_SMALL_END(G_NoAlpha)

            FOG_BLIT_LOOP_32x4_MAIN_BEGIN(G_NoAlpha)
              __m128i pix0xmm, pix1xmm, pix2xmm;

              Acc::m128iCopy(pix0xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iCopy(pix1xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iRShiftPU32<24>(pix0xmm, pix0xmm);
              Acc::m128iRShiftPU32<24>(pix1xmm, pix1xmm);

              Acc::m128iCopy(pix2xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iPackPI16FromPI32(pix0xmm, pix0xmm, pix1xmm);

              Acc::m128iCopy(pix1xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iRShiftPU32<24>(pix1xmm, pix1xmm);
              Acc::m128iRShiftPU32<24>(pix2xmm, pix2xmm);

              Acc::m128iPackPI16FromPI32(pix2xmm, pix2xmm, pix1xmm);
              Acc::m128iPackPU8FromPU16(pix0xmm, pix0xmm, pix2xmm);

              Acc::m128iXor(pix0xmm, pix0xmm, msk0xmm);
              Acc::m128iStore16a(dst, pix0xmm);

              dst += 16;
            FOG_BLIT_LOOP_32x4_MAIN_END(G_NoAlpha)
          }
          else
          {
            FOG_BLIT_LOOP_32x4_INIT()

            FOG_BLIT_LOOP_32x4_SMALL_BEGIN(G_HasAlpha)
              __m128i pix0xmm;

              Acc::m128iCopy(pix0xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iRShiftPU32<24>(pix0xmm, pix0xmm);
              Acc::m128iPackPI16FromPI32(pix0xmm, pix0xmm);
              Acc::m128iXor(pix0xmm, pix0xmm, msk0xmm);
              Acc::m128iPRGB32FromARGB32Lo_PBW(pix0xmm, pix0xmm);
              Acc::m128iPackPU8FromPU16(pix0xmm, pix0xmm);

              Acc::m128iStore4(dst, pix0xmm);
              dst += 4;
            FOG_BLIT_LOOP_32x4_SMALL_END(G_HasAlpha)

            FOG_BLIT_LOOP_32x4_MAIN_BEGIN(G_HasAlpha)
              __m128i pix0xmm, pix1xmm, pix2xmm;

              Acc::m128iCopy(pix0xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iCopy(pix1xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iRShiftPU32<24>(pix0xmm, pix0xmm);
              Acc::m128iRShiftPU32<24>(pix1xmm, pix1xmm);

              Acc::m128iCopy(pix2xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iPackPI16FromPI32(pix0xmm, pix0xmm, pix1xmm);

              Acc::m128iCopy(pix1xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iRShiftPU32<24>(pix1xmm, pix1xmm);
              Acc::m128iRShiftPU32<24>(pix2xmm, pix2xmm);

              Acc::m128iPackPI16FromPI32(pix2xmm, pix2xmm, pix1xmm);
              Acc::m128iPRGB32FromARGB32_PBW(pix0xmm, pix0xmm);
              Acc::m128iPRGB32FromARGB32_PBW(pix2xmm, pix2xmm);
              Acc::m128iPackPU8FromPU16(pix0xmm, pix0xmm, pix2xmm);

              Acc::m128iXor(pix0xmm, pix0xmm, msk0xmm);
              Acc::m128iStore16a(dst, pix0xmm);

              dst += 16;
            FOG_BLIT_LOOP_32x4_MAIN_END(G_HasAlpha)
          }
        }
      }
      else
      {
        __m128i pix0;

        Acc::m128iCvtSI128FromSI(pix0, c1);
        Acc::m128iPRGB32FromARGB32Lo_PBB(pix0, pix0);
        Acc::m128iStore4(dst, pix0);
      }

      c0 = c1;
      p0 = p1;
    }

    p1 >>= 8;
    if (p1 < (uint)_wTotal)
    {
      __m128i pix0;

      Acc::m128iCvtSI128FromSI(pix0, c1);
      Acc::m128iPRGB32FromARGB32Lo_PBB(pix0, pix0);

      uint8_t* dst = _dst + p1 * 4;
      int w = (uint)_wTotal - p1 + 1;

      FOG_ASSUME(w > 0);
      do {
        Acc::m128iStore4(dst, pix0);
        dst += 4;
      } while (--w);
    }
  }
};

} // RasterOps_SSE2 namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_SSE2_GRADIENTBASE_P_H
