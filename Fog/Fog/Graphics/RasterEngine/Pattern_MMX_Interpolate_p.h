// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Dib_MMX_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::MMX - Interpolate]
// ============================================================================

//! @internal
struct FOG_HIDDEN MMX_SYM(Interpolate)
{
  // --------------------------------------------------------------------------
  // [Gradient]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL gradient_prgb32(uint8_t* dst, uint32_t c0, uint32_t c1, int w, int x1, int x2)
  {
    uint8_t* dstCur = dst;

    // Sanity checks.
    FOG_ASSERT(w >= 0 && x1 <= x2);

    sysint_t xw = (x2 - x1);
    if (xw == 0) return;

    // Width is decreased by 1 to fit our gradient schema that first and last
    // points in interpolation are always equal to c0 and c1 recpectively.
    if (w) w--;

    // Loop counter.
    sysint_t i;

    // Fill c0 before gradient start.
    if (x1 < 0)
    {
      i = Math::min((sysint_t)0, x2) - x1;

      xw -= i;
      x1 = 0;

      do { set4(dstCur, c0); dstCur += 4; } while (--i);
      if (xw == 0) return;
    }

    // Fill c0 to c1 using linear interpolation.
    if (x1 < w)
    {
      __m64 c_ar, i_ar;
      __m64 c_gb, i_gb;

      {
        __m64 mmz;
        mmx_t t_ar;
        mmx_t t_gb;

        mmz = _mm_setzero_si64();
        c_ar = _mm_cvtsi32_si64((int32_t)c0);      // c_ar = [    ARGB] c0
        i_ar = _mm_cvtsi32_si64((int32_t)c1);      // i_ar = [    ARGB] c1

        c_ar = _mm_unpacklo_pi8(c_ar, mmz);        // c_ar = [0A0R0G0B] c0
        i_ar = _mm_unpacklo_pi8(i_ar, mmz);        // i_ar = [0A0R0G0B] c1

        c_gb = c_ar;                               // c_gb = [0A0R0G0B] c0
        i_gb = i_ar;                               // i_gb = [0A0R0G0B] c1

        c_ar = _mm_unpackhi_pi16(c_ar, mmz);       // c_ar = [000A000R] c0
        i_ar = _mm_unpackhi_pi16(i_ar, mmz);       // i_ar = [000A000R] c1

        c_gb = _mm_unpacklo_pi16(c_gb, mmz);       // c_gb = [000G000B] c0
        i_gb = _mm_unpacklo_pi16(i_gb, mmz);       // i_gb = [000G000B] c1

        c_ar = _mm_slli_pi32(c_ar, 16);            // c_ar = [0A000R00] c0
        i_ar = _mm_slli_pi32(i_ar, 16);            // i_ar = [0A000R00] c1

        c_gb = _mm_slli_pi32(c_gb, 16);            // c_gb = [0G000B00] c0
        i_gb = _mm_slli_pi32(i_gb, 16);            // i_gb = [0G000B00] c1

        i_ar = _mm_sub_pi32(i_ar, c_ar);           // i_ar = i_ar - c_ar
        i_gb = _mm_sub_pi32(i_gb, c_gb);           // i_gb = i_gb - c_gb

        // Divide.
        // 'c_ar' and 'c_gb' == c0 color and will be incremented by
        // 'i_ar' and 'i_gb' == c1 color / w
        t_ar.m64 = i_ar;                           // t_ar = i_ar
        t_gb.m64 = i_gb;                           // t_gb = i_gb

        t_ar.sd[0] /= (int32_t)w;
        t_ar.sd[1] /= (int32_t)w;
        t_gb.sd[0] /= (int32_t)w;
        t_gb.sd[1] /= (int32_t)w;

        i_ar = t_ar.m64;
        i_gb = t_gb.m64;

        // Offset interpolation to x1.
        t_ar.sd[0] *= (int32_t)x1;
        t_ar.sd[1] *= (int32_t)x1;
        t_gb.sd[0] *= (int32_t)x1;
        t_gb.sd[1] *= (int32_t)x1;

        c_ar = _mm_add_pi32(c_ar, t_ar.m64);       // c_ar += i_ar * offset
        c_gb = _mm_add_pi32(c_gb, t_gb.m64);       // c_gb += i_gb * offset
      }

      i = Math::min(w + 1, x2) - x1;
      x1 += i;

      // Align.
      if ((sysuint_t)(dstCur) & 7)
      {
one:
        __m64 c_p0, c_a0;

        c_p0 = c_gb;                               // c_p0 = c_gb
        c_p0 = _mm_packs_pu16(c_p0, c_ar);         // c_p0 = [A0R0G0B0]
        c_ar = _mm_add_pi32(c_ar, i_ar);           // c_ar += i_ar
        c_p0 = _mm_srli_pi16(c_p0, 8);             // c_p0 = [0A0R0G0B]
        c_gb = _mm_add_pi32(c_gb, i_gb);           // c_gb += i_gb

        mmx_expand_alpha_1x1W(c_a0, c_p0);         // c_a0 = [0A0A0A0A]
        mmx_fill_alpha_1x1W(c_p0);                 // c_p0 = [0F0R0G0B]
        mmx_muldiv255_1x1W(c_p0, c_p0, c_a0);      // c_p0 *= c_a0

        mmx_pack_1x1W(c_p0, c_p0);                 // c_p0 = [    ARGB]
        mmx_store4(dstCur, c_p0);

        dstCur += 4;
        if (--i == 0) goto done;
      }

      // Loop: Two pixels per time.
      while (i >= 2)
      {
        __m64 c_p0, c_a0;
        __m64 c_p1, c_a1;

        c_p0 = c_gb;                               // c_p0 = c_gb
        c_p0 = _mm_packs_pu16(c_p0, c_ar);         // c_p0 = [A0R0G0B0]

        c_gb = _mm_add_pi32(c_gb, i_gb);           // c_gb += GB step (i_gb)
        c_ar = _mm_add_pi32(c_ar, i_ar);           // c_ar += AR step (i_ar)

        c_p1 = c_gb;                               // c_p1 = c_gb
        c_p1 = _mm_packs_pu16(c_p1, c_ar);         // c_p1 = [A0R0G0B0]

        c_p0 = _mm_srli_pi16(c_p0, 8);             // c_p0 = [0A0R0G0B]
        c_p1 = _mm_srli_pi16(c_p1, 8);             // c_p1 = [0A0R0G0B]

        c_gb = _mm_add_pi32(c_gb, i_gb);           // c_gb += i_gb
        c_ar = _mm_add_pi32(c_ar, i_ar);           // c_ar += i_ar

        // c_a0, c_a1 = [0A0A0A0A]
        mmx_expand_alpha_2x1W(c_a0, c_p0, c_a1, c_p1);
        // c_p0, c_p1 = [0F0R0G0B]
        mmx_fill_alpha_2x1W(c_p0, c_p1);
        // c_p0, c_p1 *= c_a0, ms7
        mmx_muldiv255_2x1W(c_p0, c_p0, c_a0, c_p1, c_p1, c_a1);

        c_p0 = _mm_packs_pu16(c_p0, c_p1);         // c_p0 = [ARGBARGB]
        mmx_store8(dstCur, c_p0);

        dstCur += 8;
        i -= 2;
      }

      // Tail.
      if (i) goto one;
    }

    // Fill c1 after gradient end.
done:
    i = x2 - x1;
    if (i) { do { set4(dstCur, c1); dstCur += 4; } while (--i); }

    mmx_end();
  }

  static void FOG_FASTCALL gradient_argb32(uint8_t* dst, uint32_t c0, uint32_t c1, int w, int x1, int x2)
  {
    uint8_t* dstCur = dst;

    // Sanity checks.
    FOG_ASSERT(w >= 0 && x1 <= x2);

    sysint_t xw = (x2 - x1);
    if (xw == 0) return;

    // Width is decreased by 1 to fit our gradient schema that first and last
    // points in interpolation are always equal to c0 and c1 recpectively.
    if (w) w--;

    // Loop counter.
    sysint_t i;

    // Fill c0 before gradient start.
    if (x1 < 0)
    {
      i = Math::min((sysint_t)0, x2) - x1;

      xw -= i;
      x1 = 0;

      do { set4(dstCur, c0); dstCur += 4; } while (--i);
      if (xw == 0) return;
    }

    // Fill c0 to c1 using linear interpolation.
    if (x1 < w)
    {
      __m64 c_ar, i_ar;
      __m64 c_gb, i_gb;

      {
        __m64 mmz;
        mmx_t t_ar;
        mmx_t t_gb;

        mmz = _mm_setzero_si64();
        c_ar = _mm_cvtsi32_si64((int32_t)c0);      // c_ar = [    ARGB] c0
        i_ar = _mm_cvtsi32_si64((int32_t)c1);      // i_ar = [    ARGB] c1

        c_ar = _mm_unpacklo_pi8(c_ar, mmz);        // c_ar = [0A0R0G0B] c0
        i_ar = _mm_unpacklo_pi8(i_ar, mmz);        // i_ar = [0A0R0G0B] c1

        c_gb = c_ar;                               // c_gb = [0A0R0G0B] c0
        i_gb = i_ar;                               // i_gb = [0A0R0G0B] c1

        c_ar = _mm_unpackhi_pi16(c_ar, mmz);       // c_ar = [000A000R] c0
        i_ar = _mm_unpackhi_pi16(i_ar, mmz);       // i_ar = [000A000R] c1

        c_gb = _mm_unpacklo_pi16(c_gb, mmz);       // c_gb = [000G000B] c0
        i_gb = _mm_unpacklo_pi16(i_gb, mmz);       // i_gb = [000G000B] c1

        c_ar = _mm_slli_pi32(c_ar, 16);            // c_ar = [0A000R00] c0
        i_ar = _mm_slli_pi32(i_ar, 16);            // i_ar = [0A000R00] c1

        c_gb = _mm_slli_pi32(c_gb, 16);            // c_gb = [0G000B00] c0
        i_gb = _mm_slli_pi32(i_gb, 16);            // i_gb = [0G000B00] c1

        i_ar = _mm_sub_pi32(i_ar, c_ar);           // i_ar = i_ar - c_ar
        i_gb = _mm_sub_pi32(i_gb, c_gb);           // i_gb = i_gb - c_gb

        // Divide.
        // 'c_ar' and 'c_gb' == c0 color and will be incremented by
        // 'i_ar' and 'i_gb' == c1 color / w
        t_ar.m64 = i_ar;                           // t_ar = i_ar
        t_gb.m64 = i_gb;                           // t_gb = i_gb

        t_ar.sd[0] /= (int32_t)w;
        t_ar.sd[1] /= (int32_t)w;
        t_gb.sd[0] /= (int32_t)w;
        t_gb.sd[1] /= (int32_t)w;

        i_ar = t_ar.m64;
        i_gb = t_gb.m64;

        // Offset interpolation to x1.
        t_ar.sd[0] *= (int32_t)x1;
        t_ar.sd[1] *= (int32_t)x1;
        t_gb.sd[0] *= (int32_t)x1;
        t_gb.sd[1] *= (int32_t)x1;

        c_ar = _mm_add_pi32(c_ar, t_ar.m64);       // c_ar += i_ar * offset
        c_gb = _mm_add_pi32(c_gb, t_gb.m64);       // c_gb += i_gb * offset
      }

      i = Math::min(w + 1, x2) - x1;
      x1 += i;

      // Align.
      if ((sysuint_t)(dstCur) & 7)
      {
one:
        __m64 c_p0;

        c_p0 = c_gb;                               // c_p0 = c_gb
        c_p0 = _mm_packs_pu16(c_p0, c_ar);         // c_p0 = [A0R0G0B0]
        c_ar = _mm_add_pi32(c_ar, i_ar);           // c_ar += AR step (i_ar)
        c_p0 = _mm_srli_pi16(c_p0, 8);             // c_p0 = [0A0R0G0B]
        c_gb = _mm_add_pi32(c_gb, i_gb);           // c_gb += GB step (i_gb)

        mmx_pack_1x1W(c_p0, c_p0);                 // c_p0 = [    ARGB]
        mmx_store4(dstCur, c_p0);
        dstCur += 4;
        if (--i == 0) goto done;
      }

      // Loop: Two pixels per time.
      while (i >= 2)
      {
        __m64 c_p0, c_p1;

        c_p0 = c_gb;                               // c_p0 = c_gb
        c_p0 = _mm_packs_pu16(c_p0, c_ar);         // c_p0 = [A0R0G0B0]

        c_gb = _mm_add_pi32(c_gb, i_gb);           // c_gb += i_gb
        c_ar = _mm_add_pi32(c_ar, i_ar);           // c_ar += i_ar

        c_p1 = c_gb;                               // c_p1 = c_gb
        c_p1 = _mm_packs_pu16(c_p1, c_ar);         // c_p1 = [A0R0G0B0]

        c_p0 = _mm_srli_pi16(c_p0, 8);             // c_p0 = [0A0R0G0B]
        c_p1 = _mm_srli_pi16(c_p1, 8);             // c_p1 = [0A0R0G0B]

        c_gb = _mm_add_pi32(c_gb, i_gb);           // c_gb += i_gb
        c_ar = _mm_add_pi32(c_ar, i_ar);           // c_ar += i_ar

        c_p0 = _mm_packs_pu16(c_p0, c_p1);         // c_p0 = [ARGBARGB]
        mmx_store8(dstCur, c_p0);

        dstCur += 8;
        i -= 2;
      }

      // Tail.
      if (i) goto one;
    }

    // Fill c1 after gradient end.
done:
    i = x2 - x1;
    if (i) { do { set4(dstCur, c1); dstCur += 4; } while (--i); }

    mmx_end();
  }
};

} // RasterEngine namespace
} // Fog namespace
