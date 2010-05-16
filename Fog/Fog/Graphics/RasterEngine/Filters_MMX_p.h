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
// [Fog::RasterEngine::MMX - Filter]
// ============================================================================

FOG_MMX_DECLARE_CONST_PF_SET(cf0  , 0.0f  );
FOG_MMX_DECLARE_CONST_PF_SET(cf0_5, 0.5f  );
FOG_MMX_DECLARE_CONST_PF_SET(cf255, 255.0f);

FOG_MMX_DECLARE_CONST_PF_VAR(cf255_0, 255.0f, 0.0f);
FOG_MMX_DECLARE_CONST_PF_VAR(cf0_255, 0.0f, 255.0f);

FOG_MMX_DECLARE_CONST_PF_VAR(cf1_0_div255, 1.0f, 1.0f/255.0f);

FOG_MMX_DECLARE_CONST_PI32_VAR(cf1_0_0x0, 0x3F800000, 0x00000000);
FOG_MMX_DECLARE_CONST_PI32_VAR(cf0x0_1_0, 0x00000000, 0x3F800000);

FOG_MMX_DECLARE_CONST_PI16_VAR(cfm, 0x00FF, 0x0000, 0x0000, 0x00FF);
FOG_MMX_DECLARE_CONST_PI16_VAR(cfa, 0x00FF, 0x0000, 0x0000, 0x0000);

//! @internal
struct FOG_HIDDEN MMX_SYM(Filter)
{
#if defined(FOG_RASTER_MMX3DNOW)
  static FOG_INLINE __m64 color_matrix_load_rg(const float* row) { return *(__m64 *)(&row[0]); }
  static FOG_INLINE __m64 color_matrix_load_ba(const float* row) { return *(__m64 *)(&row[2]); }

  static void FOG_FASTCALL color_matrix_prgb32(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int width)
  {
    FOG_ASSERT(width != 0);

    __m64 pt_rg = _m_pfmul(color_matrix_load_rg(cm->m[4]), FOG_MMX_GET_CONST(cf255));
    __m64 pt_ba = _m_pfmul(color_matrix_load_ba(cm->m[4]), FOG_MMX_GET_CONST(cf255));

    pt_rg = _m_pfadd(pt_rg, FOG_MMX_GET_CONST(cf0_5));
    pt_ba = _m_pfadd(pt_ba, FOG_MMX_GET_CONST(cf0_5));

    for (sysuint_t i = width; i; i--, dst += 4, src += 4)
    {
      __m64 pix_i0;
      __m64 pix_i1;

      __m64 pix_f0;
      __m64 pix_f1;

      __m64 result_t0 = pt_rg;
      __m64 result_t1 = pt_ba;

      __m64 zero = _mm_setzero_si64();

      mmx_load4(pix_i0, src);
      mmx_load4(pix_f0, &raster_demultiply_reciprocal_table_f[src[ARGB32_ABYTE]]);

      pix_i0 = _mm_unpacklo_pi8(pix_i0, zero);
      pix_i1 = _mm_unpackhi_pi16(pix_i0, zero);
      pix_i0 = _mm_unpacklo_pi16(pix_i0, zero);

      // Fetch and demultiply.
      pix_f0 = _mm_unpacklo_pi32(pix_f0, pix_f0);

      pix_i0 = _m_pi2fd(pix_i0);
      pix_i1 = _m_pi2fd(pix_i1);
      pix_i0 = _m_pfmul(pix_i0, pix_f0);

      pix_f0 = _mm_srli_si64(pix_f0, 32);
      pix_f0 = _mm_or_si64(pix_f0, FOG_MMX_GET_CONST(cf1_0_0x0));
      pix_i1 = _m_pfmul(pix_i1, pix_f0);

      // Blue.
      pix_f0 = _mm_unpacklo_pi32(pix_i0, pix_i0);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[2]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[2]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Green.
      pix_f0 = _mm_unpackhi_pi32(pix_i0, pix_i0);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[1]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[1]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Red.
      pix_f0 = _mm_unpacklo_pi32(pix_i1, pix_i1);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[0]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[0]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Alpha.
      pix_f0 = _mm_unpackhi_pi32(pix_i1, pix_i1);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[3]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[3]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Bound.
      result_t0 = _m_pfmin(result_t0, FOG_MMX_GET_CONST(cf255));
      result_t1 = _m_pfmin(result_t1, FOG_MMX_GET_CONST(cf255));

      result_t0 = _m_pfmax(result_t0, FOG_MMX_GET_CONST(cf0));
      result_t1 = _m_pfmax(result_t1, FOG_MMX_GET_CONST(cf0));

      // Premultiply
      pix_f0 = _mm_srli_si64(result_t1, 32);
      pix_f0 = _mm_or_si64(pix_f0, FOG_MMX_GET_CONST(cf1_0_0x0));
      pix_f0 = _m_pfmul(pix_f0, FOG_MMX_GET_CONST(cf1_0_div255));

      result_t1 = _m_pfmul(result_t1, pix_f0);
      pix_f0 = _mm_unpacklo_pi32(pix_f0, pix_f0);
      result_t0 = _m_pfmul(result_t0, pix_f0);

      // Store.
      // t0 == [000G000R] -> [000R0G00] |
      // t1 == [000A000B] -> [0A00000B] .
      result_t0 = _m_pf2id(result_t0);
      result_t1 = _m_pf2id(result_t1);

      pix_i0 = result_t0;
      pix_i1 = result_t1;

      result_t0 = _mm_slli_si64(result_t0, 32);
      result_t1 = _mm_and_si64(result_t1, FOG_MMX_GET_CONST(cfm));

      pix_i0 = _mm_srli_si64(pix_i0, 16);
      pix_i1 = _mm_slli_si64(pix_i1, 16);
      pix_i1 = _mm_and_si64(pix_i1, FOG_MMX_GET_CONST(cfm));

      result_t0 = _mm_or_si64(result_t0, pix_i0);
      result_t1 = _mm_or_si64(result_t1, pix_i1);

      result_t0 = _mm_or_si64(result_t0, result_t1);

      mmx_pack_1x1W(result_t0, result_t0);
      mmx_store4(dst, result_t0);
    }

    mmx_end_MMX3dNow();
  }

  static void FOG_FASTCALL color_matrix_argb32(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int width)
  {
    FOG_ASSERT(width != 0);

    __m64 pt_rg = _m_pfmul(color_matrix_load_rg(cm->m[4]), FOG_MMX_GET_CONST(cf255));
    __m64 pt_ba = _m_pfmul(color_matrix_load_ba(cm->m[4]), FOG_MMX_GET_CONST(cf255));

    pt_rg = _m_pfadd(pt_rg, FOG_MMX_GET_CONST(cf0_5));
    pt_ba = _m_pfadd(pt_ba, FOG_MMX_GET_CONST(cf0_5));

    for (sysuint_t i = width; i; i--, dst += 4, src += 4)
    {
      __m64 pix_i0;
      __m64 pix_i1;

      __m64 pix_f0;
      __m64 pix_f1;

      __m64 result_t0 = pt_rg;
      __m64 result_t1 = pt_ba;

      __m64 zero = _mm_setzero_si64();

      mmx_load4(pix_i0, src);

      pix_i0 = _mm_unpacklo_pi8(pix_i0, zero);
      pix_i1 = _mm_unpackhi_pi16(pix_i0, zero);
      pix_i0 = _mm_unpacklo_pi16(pix_i0, zero);

      pix_i0 = _m_pi2fd(pix_i0);
      pix_i1 = _m_pi2fd(pix_i1);

      // Blue.
      pix_f0 = _mm_unpacklo_pi32(pix_i0, pix_i0);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[2]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[2]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Green.
      pix_f0 = _mm_unpackhi_pi32(pix_i0, pix_i0);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[1]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[1]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Red.
      pix_f0 = _mm_unpacklo_pi32(pix_i1, pix_i1);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[0]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[0]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Alpha.
      pix_f0 = _mm_unpackhi_pi32(pix_i1, pix_i1);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[3]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[3]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Bound.
      result_t0 = _m_pfmin(result_t0, FOG_MMX_GET_CONST(cf255));
      result_t1 = _m_pfmin(result_t1, FOG_MMX_GET_CONST(cf255));

      result_t0 = _m_pfmax(result_t0, FOG_MMX_GET_CONST(cf0));
      result_t1 = _m_pfmax(result_t1, FOG_MMX_GET_CONST(cf0));

      // Store.
      // t0 == [000G000R] -> [000R0G00] |
      // t1 == [000A000B] -> [0A00000B] .
      result_t0 = _m_pf2id(result_t0);
      result_t1 = _m_pf2id(result_t1);

      pix_i0 = result_t0;
      pix_i1 = result_t1;

      result_t0 = _mm_slli_si64(result_t0, 32);
      result_t1 = _mm_and_si64(result_t1, FOG_MMX_GET_CONST(cfm));

      pix_i0 = _mm_srli_si64(pix_i0, 16);
      pix_i1 = _mm_slli_si64(pix_i1, 16);
      pix_i1 = _mm_and_si64(pix_i1, FOG_MMX_GET_CONST(cfm));

      result_t0 = _mm_or_si64(result_t0, pix_i0);
      result_t1 = _mm_or_si64(result_t1, pix_i1);

      result_t0 = _mm_or_si64(result_t0, result_t1);

      mmx_pack_1x1W(result_t0, result_t0);
      mmx_store4(dst, result_t0);
    }

    mmx_end_MMX3dNow();
  }

  static void FOG_FASTCALL color_matrix_xrgb32(
    const ColorMatrix* cm,
    uint8_t* dst, const uint8_t* src, int width)
  {
    FOG_ASSERT(width != 0);

    __m64 pt_rg = _m_pfmul(color_matrix_load_rg(cm->m[4]), FOG_MMX_GET_CONST(cf255));
    __m64 pt_ba = _m_pfmul(color_matrix_load_ba(cm->m[4]), FOG_MMX_GET_CONST(cf255));

    pt_rg = _m_pfadd(pt_rg, _m_pfmul(color_matrix_load_rg(cm->m[3]), FOG_MMX_GET_CONST(cf255)));
    pt_ba = _m_pfadd(pt_ba, _m_pfmul(color_matrix_load_rg(cm->m[3]), FOG_MMX_GET_CONST(cf255)));

    pt_rg = _m_pfadd(pt_rg, FOG_MMX_GET_CONST(cf0_5));
    pt_ba = _m_pfadd(pt_ba, FOG_MMX_GET_CONST(cf0_5));

    for (sysuint_t i = width; i; i--, dst += 4, src += 4)
    {
      __m64 pix_i0;
      __m64 pix_i1;

      __m64 pix_f0;
      __m64 pix_f1;

      __m64 result_t0 = pt_rg;
      __m64 result_t1 = pt_ba;

      __m64 zero = _mm_setzero_si64();

      mmx_load4(pix_i0, src);

      pix_i0 = _mm_unpacklo_pi8(pix_i0, zero);
      pix_i1 = _mm_unpackhi_pi16(pix_i0, zero);
      pix_i0 = _mm_unpacklo_pi16(pix_i0, zero);

      pix_i0 = _m_pi2fd(pix_i0);
      pix_i1 = _m_pi2fd(pix_i1);

      // Blue.
      pix_f0 = _mm_unpacklo_pi32(pix_i0, pix_i0);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[2]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[2]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Green.
      pix_f0 = _mm_unpackhi_pi32(pix_i0, pix_i0);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[1]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[1]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Red.
      pix_f0 = _mm_unpacklo_pi32(pix_i1, pix_i1);
      pix_f1 = pix_f0;

      pix_f0 = _m_pfmul(pix_f0, color_matrix_load_rg(cm->m[0]));
      pix_f1 = _m_pfmul(pix_f1, color_matrix_load_ba(cm->m[0]));

      result_t0 = _m_pfadd(result_t0, pix_f0);
      result_t1 = _m_pfadd(result_t1, pix_f1);

      // Bound.
      result_t0 = _m_pfmin(result_t0, FOG_MMX_GET_CONST(cf255));
      result_t1 = _m_pfmin(result_t1, FOG_MMX_GET_CONST(cf0_255));

      result_t0 = _m_pfmax(result_t0, FOG_MMX_GET_CONST(cf0));
      result_t1 = _m_pfmax(result_t1, FOG_MMX_GET_CONST(cf0));

      // Store.
      // t0 == [000G000R] -> [000R0G00] |
      // t1 == [0000000B] -> [0F00000B] .
      result_t0 = _m_pf2id(result_t0);
      result_t1 = _m_pf2id(result_t1);

      pix_i0 = result_t0;
      result_t0 = _mm_slli_si64(result_t0, 32);
      pix_i0 = _mm_srli_si64(pix_i0, 16);
      result_t1 = _mm_or_si64(result_t1, FOG_MMX_GET_CONST(cfa));

      result_t0 = _mm_or_si64(result_t0, pix_i0);
      result_t0 = _mm_or_si64(result_t0, result_t1);

      mmx_pack_1x1W(result_t0, result_t0);
      mmx_store4(dst, result_t0);
    }

    mmx_end_MMX3dNow();
  }
#endif // FOG_RASTER_MMX3DNOW
};

} // RasterEngine namespace
} // Fog namespace
