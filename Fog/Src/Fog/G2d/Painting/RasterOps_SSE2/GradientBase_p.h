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

  static void FOG_FASTCALL interpolate_prgb32(uint8_t* _dst, int _w, const ColorStop* stops, size_t length)
  {
    FOG_ASSUME(length >= 1);

    // ------------------------------------------------------------------------
    // [Solid]
    // ------------------------------------------------------------------------

    if (length == 1)
    {
      RasterOps_C::Helpers::p_fill_prgb32(_dst, stops[0].getArgb32(), _w);
      return;
    }

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    _w--;

    uint p0 = 0;
    uint p1;

    uint32_t c0 = stops[0].getArgb32();
    uint32_t c1;

    float wf = (float)(_w << 8);

    // ------------------------------------------------------------------------
    // [Loop]
    // ------------------------------------------------------------------------

    size_t pos;
    for (pos = 0; pos < length; pos++)
    {
      c1 = stops[pos].getArgb32();
      p1 = Math::uround(stops[pos].getOffset() * wf);
      uint len = (p1 >> 8) - (p0 >> 8);

      uint32_t* dst = reinterpret_cast<uint32_t*>(_dst) + (p0 >> 8);
      uint i;

      if (len > 0)
      {
        if (c0 == c1)
        {
          uint32_t cp;
          Acc::p32PRGB32FromARGB32(cp, c0);

          for (i = 0; i < len; i++) dst[i] = cp;
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

            for (i = 0; i <= len; i++)
            {
              __m128i pix0xmm;

              Acc::m128iCopy(pix0xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iRShiftPU32<24>(pix0xmm, pix0xmm);
              Acc::m128iPackPU8FromPI32(pix0xmm, pix0xmm, pix0xmm);
              Acc::m128iXor(pix0xmm, pix0xmm, msk0xmm);
              Acc::m128iStore4(&dst[i], pix0xmm);
            }
          }
          else
          {
            for (i = 0; i <= len; i++)
            {
              __m128i pix0xmm;
              __m128i alpha0xmm;

              Acc::m128iCopy(pix0xmm, pos0xmm);
              Acc::m128iAddPI32(pos0xmm, pos0xmm, inc0xmm);

              Acc::m128iRShiftPU32<24>(pix0xmm, pix0xmm);
              Acc::m128iPackPI16FromPI32(pix0xmm, pix0xmm);
              Acc::m128iXor(pix0xmm, pix0xmm, msk0xmm);
              Acc::m128iShufflePI16Lo<3, 3, 3, 3>(alpha0xmm, pix0xmm);
              Acc::m128iFillPBWi<3>(pix0xmm, pix0xmm);
              Acc::m128iMulDiv255PI16(pix0xmm, pix0xmm, alpha0xmm);
              Acc::m128iPackPU8FromPU16(pix0xmm, pix0xmm);
              Acc::m128iStore4(&dst[i], pix0xmm);
            }
          }
        }
      }
      else
      {
        uint32_t cp;
        Acc::p32PRGB32FromARGB32(cp, c1);

        dst[0] = cp;
      }

      c0 = c1;
      p0 = p1;
    }

    p1 >>= 8;
    if (p1 < (uint)_w)
    {
      uint32_t cp;
      Acc::p32PRGB32FromARGB32(cp, c1);

      uint32_t* dst = reinterpret_cast<uint32_t*>(_dst) + p1;
      uint i, len = (uint)_w - p1 + 1;

      for (i = 0; i < len; i++) dst[i] = cp;
    }
  }
};

} // RasterOps_SSE2 namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_SSE2_GRADIENTBASE_P_H
