// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Build/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::MMX - Core]
// ============================================================================

// These macros were designed to simplify implementation of raster engine for:
// - MMX      - MMX without 3dNow or SSE support.
// - MMX3dNow - MMX with some 3dNow code.
// - MMXSSE   - MMX using SSE extensions (mainly shuffling, different muldiv255)

// Check if possible specializations are correct.
#if defined(FOG_RASTER_MMX3DNOW) && defined(FOG_RASTER_MMXSSE)
#error "Fog::RasterEngine - Multiple specialization declarations"
#endif

// MMX.
#define MMX_SYM(symbol) symbol##MMX

// MMX3dNow.
#if defined(FOG_RASTER_MMX3DNOW)
#undef MMX_SYM
#define MMX_SYM(symbol) symbol##MMX3dNow
#endif

// MMXSSE.
#if defined(FOG_RASTER_MMXSSE)
#undef MMX_SYM
#define MMX_SYM(symbol) symbol##MMXSSE
#endif

// ============================================================================
// [Fog::RasterEngine::MMX - Defines]
// ============================================================================

// These macros were designed to simplify blit functions. The idea is very simple.
// There are usually three loops that you can see in blitters. One is align loop.
// The align loop wants to align data into 8 bytes. When aligning is done, the
// main loop is run and it usually uses 8 byte increments (so 2 ARGB pixels).
// The reason for third loop (tail loop) is that it's needed to process remaining
// pixel that can't fit into main loop.

// There is idea to merge align and tail into one block. This will save some
// library binary size (about 1/4 size of blitting functions) and simplify the
// development. So to not repeat dirty stuff in each function there are macros
// that will do everything for you.

#define MMX_BLIT_32x2_INIT(_dst, _w) \
  sysint_t _a = (sysint_t)(_dst); \
  sysint_t _i = (sysint_t)_w;

#define MMX_BLIT_32x2_SMALL_BEGIN(group) \
  if (_a & 0x7) \
  { \
group##_small:

#define MMX_BLIT_32x2_SMALL_END(group) \
    if (--_i == 0) goto group##_end; \
  }

#define MMX_BLIT_32x2_LARGE_BEGIN(group) \
  while ((_i -= 2) >= 0) \
  { \

#define MMX_BLIT_32x2_LARGE_END(group) \
  } \
  if ((_i += 2) != 0) goto group##_small; \
group##_end: \
  ;

#define MMX_BLIT_TEST_1_PRGB_PIXEL(__src0mm, __tmp0mm, __L_fill, __L_away) \
  __tmp0mm = MMX_GET_CONST(FFFFFFFF00000000); \
  mmx_expand_pixel_lo_1x1B(__src0mm, __src0mm); \
  __tmp0mm = _mm_cmpeq_pi8(__tmp0mm, __src0mm); \
  \
  { \
    register uint __srcMsk0 = (uint)_mm_movemask_pi8(__tmp0mm); \
    __srcMsk0 &= 0x88; \
    \
    if (__srcMsk0 == 0x08) goto __L_away; \
    if (__srcMsk0 == 0x80) goto __L_fill; \
  }

#define MMX_BLIT_TEST_2_PRGB_PIXELS(__src0mm, __tmp0mm, __tmp1mm, __L_fill, __L_away) \
  _mm_ext_fill_si64(__tmp0mm); \
  __tmp1mm = _mm_setzero_si64(); \
  \
  __tmp0mm = _mm_cmpeq_pi8(__tmp0mm, __src0mm); \
  __tmp1mm = _mm_cmpeq_pi8(__tmp1mm, __src0mm); \
  \
  { \
    register uint __srcMsk0 = (uint)_mm_movemask_pi8(__tmp0mm); \
    register uint __srcMsk1 = (uint)_mm_movemask_pi8(__tmp1mm); \
    \
    __srcMsk0 &= 0x88; \
    \
    if (__srcMsk1 == 0xFF) goto __L_away; \
    if (__srcMsk0 == 0x88) goto __L_fill; \
  }

// ============================================================================
// [Fog::RasterEngine::MMX - Constants]
// ============================================================================

#define MMX_DECLARE_CONST_PI8_VAR(name, val0, val1, val2, val3, val4, val5, val6, val7) \
  FOG_ALIGNED_VAR(static const uint8_t, _sse2_const_##name[8], 8) = \
  { \
    (uint8_t)(val7), \
    (uint8_t)(val6), \
    (uint8_t)(val5), \
    (uint8_t)(val4), \
    (uint8_t)(val3), \
    (uint8_t)(val2), \
    (uint8_t)(val1), \
    (uint8_t)(val0)  \
  }

#define MMX_DECLARE_CONST_PI8_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint8_t, _mmx_const_##name[8], 8) = \
  { \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0)  \
  }

#define MMX_DECLARE_CONST_PI16_VAR(name, val0, val1, val2, val3) \
  FOG_ALIGNED_VAR(static const uint16_t, _mmx_const_##name[4], 8) = \
  { \
    (uint16_t)(val3), \
    (uint16_t)(val2), \
    (uint16_t)(val1), \
    (uint16_t)(val0)  \
  }

#define MMX_DECLARE_CONST_PI16_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint16_t, _mmx_const_##name[4], 8) = \
  { \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0)  \
  }

#define MMX_DECLARE_CONST_PI32_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const uint32_t, _mmx_const_##name[2], 8) = \
  { \
    (uint32_t)(val1), \
    (uint32_t)(val0)  \
  }

#define MMX_DECLARE_CONST_PI32_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint32_t, _mmx_const_##name[2], 8) = \
  { \
    (uint32_t)(val0), \
    (uint32_t)(val0)  \
  }

#define MMX_DECLARE_CONST_PI64(name, val0, val1) \
  FOG_ALIGNED_VAR(static const uint64_t, _mmx_const_##name[1], 8) = \
  { \
    (uint64_t)(val0)  \
  }

#define MMX_DECLARE_CONST_PF_SET(name, val0) \
  FOG_ALIGNED_VAR(static const float, _mmx_const_##name[2], 8) = \
  { \
    (float)(val0), \
    (float)(val0)  \
  }

#define MMX_DECLARE_CONST_PF_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const float, _mmx_const_##name[2], 8) = \
  { \
    (float)(val1), \
    (float)(val0)  \
  }

#define MMX_GET_CONST(name) (*(const __m64*)_mmx_const_##name)

// ============================================================================
// [Fog::RasterEngine::MMX - Constants]
// ============================================================================

// MMX masks.
MMX_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
MMX_DECLARE_CONST_PI16_VAR(0080008000800080, 0x0080, 0x0080, 0x0080, 0x0080);
MMX_DECLARE_CONST_PI16_VAR(0101010101010101, 0x0101, 0x0101, 0x0101, 0x0101);

MMX_DECLARE_CONST_PI16_VAR(FF000000FF000000, 0xFF00, 0x0000, 0xFF00, 0x0000);
MMX_DECLARE_CONST_PI16_VAR(00FF000000000000, 0x00FF, 0x0000, 0x0000, 0x0000);
MMX_DECLARE_CONST_PI16_VAR(FFFFFFFF00000000, 0xFFFF, 0xFFFF, 0x0000, 0x0000);

// RGB16 masks.
MMX_DECLARE_CONST_PI16_VAR(0000F8000000F800, 0x0000, 0xF800, 0x0000, 0xF800);
MMX_DECLARE_CONST_PI16_VAR(0000E0000000E000, 0x0000, 0xE000, 0x0000, 0xE000);
MMX_DECLARE_CONST_PI16_VAR(00007C0000007C00, 0x0000, 0x7C00, 0x0000, 0x7C00);
MMX_DECLARE_CONST_PI16_VAR(00001F0000001F00, 0x0000, 0x1F00, 0x0000, 0x1F00);
MMX_DECLARE_CONST_PI16_VAR(000007E0000007E0, 0x0000, 0x07E0, 0x0000, 0x07E0);
MMX_DECLARE_CONST_PI16_VAR(000003E0000003E0, 0x0000, 0x03E0, 0x0000, 0x03E0);
MMX_DECLARE_CONST_PI16_VAR(000000F8000000F8, 0x0000, 0x00F8, 0x0000, 0x00F8);
MMX_DECLARE_CONST_PI16_VAR(0000007C0000007C, 0x0000, 0x007C, 0x0000, 0x007C);
MMX_DECLARE_CONST_PI16_VAR(0000001F0000001F, 0x0000, 0x001F, 0x0000, 0x001F);
MMX_DECLARE_CONST_PI16_VAR(0000000700000007, 0x0000, 0x0007, 0x0000, 0x0007);
MMX_DECLARE_CONST_PI16_VAR(0000000300000003, 0x0000, 0x0003, 0x0000, 0x0003);

// ============================================================================
// [Fog::RasterEngine::MMX - Helpers]
// ============================================================================

// Load.

template<typename T>
static FOG_INLINE void mmx_load4(__m64& dst0, const T* srcp)
{
  dst0 = _mm_cvtsi32_si64(((const uint32_t*)srcp)[0]);
}

template<typename T>
static FOG_INLINE void mmx_load8(__m64& dst0, const T* srcp)
{
  dst0 = reinterpret_cast<const __m64*>(srcp)[0];
}

// Store.

template<typename T>
static FOG_INLINE void mmx_store4(T* dstp, const __m64& src0)
{
  ((uint32_t*)dstp)[0] = _mm_cvtsi64_si32(src0);
}

template<typename T>
static FOG_INLINE void mmx_store8(T* dstp, const __m64& src0)
{
  reinterpret_cast<__m64*>(dstp)[0] = src0;
}

// Unpack.

static FOG_INLINE void mmx_unpack_1x1W(
  __m64& dst0, const __m64& src0)
{
  __m64 mmz = _mm_setzero_si64();
  dst0 = _mm_unpacklo_pi8(src0, mmz);
}

static FOG_INLINE void mmx_unpack_2x1W(
  __m64& dst0,
  __m64& dst1, const __m64& src0)
{
  __m64 mmz = _mm_setzero_si64();
  dst1 = _mm_unpackhi_pi8(src0, mmz);
  dst0 = _mm_unpacklo_pi8(src0, mmz);
}

// Pack.

static FOG_INLINE void mmx_pack_1x1W(
  __m64& dst0, const __m64& src0)
{
  dst0 = _mm_packs_pu16(src0, src0);
}

static FOG_INLINE void mmx_pack_2x1W(
  __m64& dst0, const __m64& src0, const __m64& src1)
{
  dst0 = _mm_packs_pu16(src0, src1);
}

// Negate.

static FOG_INLINE void mmx_negate_1x1W(
  __m64& dst0, const __m64& src0)
{
  dst0 = _mm_xor_si64(src0, MMX_GET_CONST(00FF00FF00FF00FF));
}

static FOG_INLINE void mmx_negate_2x1W(
  __m64& dst0, const __m64& src0,
  __m64& dst1, const __m64& src1)
{
  dst0 = _mm_xor_si64(src0, MMX_GET_CONST(00FF00FF00FF00FF));
  dst1 = _mm_xor_si64(src1, MMX_GET_CONST(00FF00FF00FF00FF));
}

// Swap.

// Expand Pixel.

static FOG_INLINE void mmx_expand_pixel_lo_1x1B(
  __m64& dst0, const __m64& src0)
{
  dst0 = _mm_unpacklo_pi32(src0, src0);
}

static FOG_INLINE void mmx_expand_pixel_hi_1x1B(
  __m64& dst0, const __m64& src0)
{
  dst0 = _mm_unpackhi_pi32(src0, src0);
}

#if defined(FOG_RASTER_MMXSSE)
static FOG_INLINE void mmx_expand_pixel_lo_1x1B_MMXSSE(
  __m64& dst0, const __m64& src0)
{
  dst0 = _mm_shuffle_pi16(src0, _MM_SHUFFLE(1, 0, 1, 0));
}

static FOG_INLINE void mmx_expand_pixel_hi_1x1B_MMXSSE(
  __m64& dst0, const __m64& src0)
{
  dst0 = _mm_shuffle_pi16(src0, _MM_SHUFFLE(3, 2, 3, 2));
}

#define mmx_expand_pixel_lo_1x1B mmx_expand_pixel_lo_1x1B_MMXSSE
#define mmx_expand_pixel_hi_1x1B mmx_expand_pixel_hi_1x1B_MMXSSE
#endif // FOG_RASTER_MMXSSE

// Expand Alpha.

static FOG_INLINE void mmx_expand_alpha_1x1W(
  __m64& dst0, const __m64& src0)
{
  dst0 = _mm_unpackhi_pi16(src0, src0);
  dst0 = _mm_unpackhi_pi16(dst0, dst0);
}

static FOG_INLINE void mmx_expand_alpha_2x1W(
  __m64& dst0, const __m64& src0,
  __m64& dst1, const __m64& src1)
{
  dst0 = _mm_unpackhi_pi16(src0, src0);
  dst1 = _mm_unpackhi_pi16(src1, src1);
  dst0 = _mm_unpackhi_pi16(dst0, dst0);
  dst1 = _mm_unpackhi_pi16(dst1, dst1);
}

#if defined(FOG_RASTER_MMXSSE)
static FOG_INLINE void mmx_expand_alpha_1x1W_MMXSSE(
  __m64& dst0, const __m64& src0)
{
  dst0 = _mm_shuffle_pi16(src0, _MM_SHUFFLE(3, 3, 3, 3));
}

static FOG_INLINE void mmx_expand_alpha_1x2W_MMXSSE(
  __m64& dst0, const __m64& src0,
  __m64& dst1, const __m64& src1)
{
  dst0 = _mm_shuffle_pi16(src0, _MM_SHUFFLE(3, 3, 3, 3));
  dst1 = _mm_shuffle_pi16(src1, _MM_SHUFFLE(3, 3, 3, 3));
}

#define mmx_expand_alpha_1x1W mmx_expand_alpha_1x1W_MMXSSE
#define mmx_expand_alpha_1x2W mmx_expand_alpha_1x2W_MMXSSE
#endif // FOG_RASTER_MMXSSE

// Add.

static FOG_INLINE void mmx_adds_1x1W(
  __m64& dst0, const __m64& a0, const __m64& b0)
{
  dst0 = _mm_adds_pu8(a0, b0);
}

static FOG_INLINE void mmx_adds_2x1W(
  __m64& dst0, const __m64& a0, const __m64& b0,
  __m64& dst1, const __m64& a1, const __m64& b1)
{
  dst0 = _mm_adds_pu8(a0, b0);
  dst1 = _mm_adds_pu8(a1, b1);
}

// Sub.

// Expand Mask.

// Reverse.

// Multiply.

static FOG_INLINE void mmx_muldiv255_1x1W(
  __m64& dst0, const __m64& a0, const __m64& b0)
{
  dst0 = _mm_mullo_pi16(a0, b0);
  dst0 = _mm_adds_pu16(dst0, MMX_GET_CONST(0080008000800080));
  __m64 t0 = _mm_srli_pi16(dst0, 8);
  dst0 = _mm_adds_pu16(dst0, t0);
  dst0 = _mm_srli_pi16(dst0, 8);
}

static FOG_INLINE void mmx_muldiv255_2x1W(
  __m64& dst0, const __m64& a0, const __m64& b0,
  __m64& dst1, const __m64& a1, const __m64& b1)
{
  dst0 = _mm_mullo_pi16(a0, b0);
  dst1 = _mm_mullo_pi16(a1, b1);

  dst0 = _mm_adds_pu16(dst0, MMX_GET_CONST(0080008000800080));
  dst1 = _mm_adds_pu16(dst1, MMX_GET_CONST(0080008000800080));

  __m64 t0 = _mm_srli_pi16(dst0, 8);
  __m64 t1 = _mm_srli_pi16(dst1, 8);

  dst0 = _mm_adds_pu16(dst0, t0);
  dst1 = _mm_adds_pu16(dst1, t1);

  dst0 = _mm_srli_pi16(dst0, 8);
  dst1 = _mm_srli_pi16(dst1, 8);
}

#if defined(FOG_RASTER_MMXSSE)
static FOG_INLINE void mmx_muldiv255_1x1W_MMXSSE(
  __m64& dst0, const __m64& a0, const __m64& b0)
{
  dst0 = _mm_mullo_pi16(a0, b0);
  dst0 = _mm_adds_pu16(dst0, MMX_GET_CONST(0080008000800080));
  dst0 = _mm_mulhi_pu16(dst0, MMX_GET_CONST(0101010101010101));
}

static FOG_INLINE void mmx_muldiv255_2x1W_MMXSSE(
  __m64& dst0, const __m64& a0, const __m64& b0,
  __m64& dst1, const __m64& a1, const __m64& b1)
{
  dst0 = _mm_mullo_pi16(a0, b0);
  dst1 = _mm_mullo_pi16(a1, b1);

  dst0 = _mm_adds_pu16(dst0, MMX_GET_CONST(0080008000800080));
  dst1 = _mm_adds_pu16(dst1, MMX_GET_CONST(0080008000800080));

  dst0 = _mm_mulhi_pu16(dst0, MMX_GET_CONST(0101010101010101));
  dst1 = _mm_mulhi_pu16(dst1, MMX_GET_CONST(0101010101010101));
}

#define mmx_muldiv255_1x1W mmx_muldiv255_1x1W_MMXSSE
#define mmx_muldiv255_2x1W mmx_muldiv255_2x1W_MMXSSE
#endif // FOG_RASTER_MMXSSE

// Fill Alpha.

static FOG_INLINE void mmx_fill_alpha_1x1B(
  __m64& dst0)
{
  dst0 = _mm_or_si64(dst0, MMX_GET_CONST(FF000000FF000000));
}

static FOG_INLINE void mmx_fill_alpha_1x2B(
  __m64& dst0)
{
  dst0 = _mm_or_si64(dst0, MMX_GET_CONST(FF000000FF000000));
}

static FOG_INLINE void mmx_fill_alpha_1x1W(
  __m64& dst0)
{
  dst0 = _mm_or_si64(dst0, MMX_GET_CONST(00FF000000000000));
}

static FOG_INLINE void mmx_fill_alpha_2x1W(
  __m64& dst0,
  __m64& dst1)
{
  dst0 = _mm_or_si64(dst0, MMX_GET_CONST(00FF000000000000));
  dst1 = _mm_or_si64(dst1, MMX_GET_CONST(00FF000000000000));
}

// End.

static FOG_INLINE void mmx_end()
{
  _mm_empty();
}

#if defined(FOG_RASTER_MMX3DNOW)
static FOG_INLINE void mmx_end_MMX3dNow()
{
  _m_femms();
}

#define mmx_end mmx_end_MMX3dNow
#endif

} // RasterEngine namespace
} // Fog namespace
