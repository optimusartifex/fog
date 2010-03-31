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
// [Fog::RasterEngine::SSE2 - Defines]
// ============================================================================

// These macros were designed to simplify blit functions. The idea is very simple.
// There are usually three loops that you can see in blitters. One is align loop.
// The align loop wants to align data into 16 bytes. When aligning is done, the
// main loop is run and it usually uses 16 byte increments (so 4 ARGB pixels).
// The reason for third loop (tail loop) is that it's needed to process remaining
// pixels that can't fit into main loop.
//
// There is idea to merge align and tail loop into one loop. This will save some
// library binary size (about 1/4 size of blitting functions) and simplify the
// development. So to not repeat dirty stuff in each function there are macros
// that will do everything for you.
//
// The two loops are named SMALL and LARGE.
//
// BLIT_[32/24/8]_SSE2_INIT(dst, w) - This macro will declare '_i' variable that
// contains initial value for small loop and '_j' variable that contains count
// of pixels to process in tail loop. It also modifies _w variable to contain
// only how many cycles will be processed by large loop (4 pixels at time), not
// whole width in pixels.
//
// 32-bit entities:
//   - 1 pixel at time:
//     - SSE2_BLIT_32x4_SMALL_BEGIN(dst) - Small loop begin.
//     - SSE2_BLIT_32x4_SMALL_END(dst) - Small loop end.
//
//   - 4 pixels at time:
//     - SSE2_BLIT_32x4_LARGE_BEGIN(dst) - Main loop begin.
//     - SSE2_BLIT_32x4_LARGE_END(dst) - Main loop end.
//
// 8-bit entities:
//   - 1 pixel at time:
//     - SSE2_BLIT_8x16_SMALL_BEGIN(dst) - Small loop begin.
//     - SSE2_BLIT_8x16_SMALL_END(dst) - Small loop end.
//
//   - 16 pixels at time:
//     - SSE2_BLIT_8x16_LARGE_BEGIN(dst) - Main loop begin.
//     - SSE2_BLIT_8x16_LARGE_END(dst) - Main loop end.
//
// Because compilers can be quite missed from our machinery, it's needed
// to follow some rules to help them to optimize this code:
// - declare temporary variables (mainly sse2 registers) in local loop scope.
// - do not add anything between SSE2_BLIT_32x4_SMALL_END and SSE2_BLIT_32x4_LARGE_BEGIN.

static const sysint_t __m32x4AlignBy[16] =
{
  0         , // 0x00
  SYSINT_MAX, // 0x01
  SYSINT_MAX, // 0x02
  SYSINT_MAX, // 0x03
  3         , // 0x04
  SYSINT_MAX, // 0x05
  SYSINT_MAX, // 0x06
  SYSINT_MAX, // 0x07
  2         , // 0x08
  SYSINT_MAX, // 0x09
  SYSINT_MAX, // 0x0A
  SYSINT_MAX, // 0x0B
  1         , // 0x0C
  SYSINT_MAX, // 0x0D
  SYSINT_MAX, // 0x0E
  SYSINT_MAX  // 0x0F
};

// 32-bit entities:

#define SSE2_BLIT_32x4_INIT(_dst, _w) \
  sysuint_t _i = (sysuint_t)__m32x4AlignBy[(sysuint_t)_dst & 15]; \
  sysuint_t _j; \
  \
  if (_i > (sysuint_t)_w) _i = (sysuint_t)_w; \
  _w -= _i; \
  _j = _w & 3; \
  if ((_w >>= 2) == 0) _i += _j;

#define SSE2_BLIT_32x4_SMALL_BEGIN(group) \
  if (_i) \
  { \
group: \
    do {

#define SSE2_BLIT_32x4_SMALL_END(group) \
    } while (--_i); \
    if (!w) goto group##_end; \
  }

#define SSE2_BLIT_32x4_LARGE_BEGIN(group) \
  do {

#define SSE2_BLIT_32x4_LARGE_END(group) \
  } while (--w); \
  \
  if ((_i = _j)) goto group; \
group##_end: \
  ;

// Alternatives to 'SSE2_BLIT_32x4_SMALL_BEGIN'
#define SSE2_BLIT_32x4_SMALL_PREPARE(group) \
  if (_i) \
  { \
group:

#define SSE2_BLIT_32x4_SMALL_DO(group) \
    do {

// 8-bit entities:

#define SSE2_BLIT_8x4_INIT(_dst, _w) \
  sysuint_t _i = (sysuint_t)_w; \
  sysuint_t _j = 0; \
  \
  if (_i >= 4) \
  { \
    sysint_t align = ((sysint_t)4 - ((sysint_t)(_dst) & 3)) & 3; \
    \
    if (_w - align < 4) \
    { \
      _w = 0; \
    } \
    else \
    { \
      _i = align; \
      _w -= (sysint_t)_i; \
      _j = _w & 3; \
      _w >>= 2; \
    } \
  } \
  else \
  { \
    _w = 0; \
  }

#define SSE2_BLIT_8x4_SMALL_BEGIN(group) \
  if (_i) \
  { \
group: \
    do {

#define SSE2_BLIT_8x4_SMALL_END(group) \
    } while (--_i); \
    if (!w) goto group##_end; \
  } \

#define SSE2_BLIT_8x4_LARGE_BEGIN(group) \
  do {

#define SSE2_BLIT_8x4_LARGE_END(group) \
  } while (--w); \
  \
  if ((_i = _j)) goto group; \
group##_end: \
  ;

#define SSE2_BLIT_8x16_INIT(_dst, _w) \
  sysuint_t _i = (sysuint_t)_w; \
  sysuint_t _j = 0; \
  \
  if (_i >= 16) \
  { \
    sysint_t align = ((sysint_t)16 - ((sysint_t)(_dst) & 15)) & 15; \
    \
    if (_w - align < 16) \
    { \
      _w = 0; \
    } \
    else \
    { \
      _i = align; \
      _w -= (sysint_t)_i; \
      _j = _w & 15; \
      _w >>= 4; \
    } \
  } \
  else \
  { \
    _w = 0; \
  }

#define SSE2_BLIT_8x16_SMALL_BEGIN(group) \
  if (_i) \
  { \
group: \
    do {

#define SSE2_BLIT_8x16_SMALL_END(group) \
    } while (--_i); \
    if (!w) goto group##_end; \
  } \

#define SSE2_BLIT_8x16_LARGE_BEGIN(group) \
  do {

#define SSE2_BLIT_8x16_LARGE_END(group) \
  } while (--w); \
  \
  if ((_i = _j)) goto group; \
group##_end: \
  ;

#define SSE2_BLIT_GENERIC_END(group) \
group##_end: \
  ;

// This is heavily optimized inner loop test for all opaque pixels or all
// transparent ones. This is invention that comes from BlitJit project and
// this code was translated originally from BlitJit code generator. If C++
// compiler is smart then this code is limit what you can do while using
// 4-pixels per time.
#define SSE2_BLIT_TEST_4_PRGB_PIXELS(__src0xmm, __tmp0xmm, __tmp1xmm, __L_fill, __L_away) \
  _mm_ext_fill_si128(__tmp0xmm); \
  __tmp1xmm = _mm_setzero_si128(); \
  \
  __tmp0xmm = _mm_cmpeq_epi8(__tmp0xmm, __src0xmm); \
  __tmp1xmm = _mm_cmpeq_epi8(__tmp1xmm, __src0xmm); \
  \
  { \
    register uint __srcMsk0 = (uint)_mm_movemask_epi8(__tmp0xmm); \
    register uint __srcMsk1 = (uint)_mm_movemask_epi8(__tmp1xmm); \
    \
    __srcMsk0 &= 0x8888; \
    \
    if (__srcMsk1 == 0xFFFF) goto __L_away; \
    if (__srcMsk0 == 0x8888) goto __L_fill; \
  }

#define SSE2_BLIT_TEST_4_ARGB_PIXELS(__src0xmm, __tmp0xmm, __tmp1xmm, __L_fill, __L_away) \
  __tmp1xmm = _mm_setzero_si128(); \
  _mm_ext_fill_si128(__tmp0xmm); \
  \
  __tmp1xmm = _mm_cmpeq_epi8(__tmp1xmm, __src0xmm); \
  __tmp0xmm = _mm_cmpeq_epi8(__tmp0xmm, __src0xmm); \
  \
  { \
    register uint __srcMsk1 = (uint)_mm_movemask_epi8(__tmp1xmm); \
    register uint __srcMsk0 = (uint)_mm_movemask_epi8(__tmp0xmm); \
    \
    __srcMsk1 &= 0x8888; \
    __srcMsk0 &= 0x8888; \
    \
    if (__srcMsk1 == 0x8888) goto __L_away; \
    if (__srcMsk0 == 0x8888) goto __L_fill; \
  }

// ============================================================================
// [Fog::RasterEngine::SSE2 - Constants]
// ============================================================================

#define SSE2_DECLARE_CONST_PI8_VAR(name, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9, val10, val11, val12, val13, val14, val15) \
  FOG_ALIGNED_VAR(static const uint8_t, _sse2_const_##name[16], 16) = \
  { \
    (uint8_t)(val15), \
    (uint8_t)(val14), \
    (uint8_t)(val13), \
    (uint8_t)(val12), \
    (uint8_t)(val11), \
    (uint8_t)(val10), \
    (uint8_t)(val9), \
    (uint8_t)(val8), \
    (uint8_t)(val7), \
    (uint8_t)(val6), \
    (uint8_t)(val5), \
    (uint8_t)(val4), \
    (uint8_t)(val3), \
    (uint8_t)(val2), \
    (uint8_t)(val1), \
    (uint8_t)(val0)  \
  }

#define SSE2_DECLARE_CONST_PI8_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint8_t, _sse2_const_##name[16], 16) = \
  { \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0), \
    (uint8_t)(val0)  \
  }

#define SSE2_DECLARE_CONST_PI16_VAR(name, val0, val1, val2, val3, val4, val5, val6, val7) \
  FOG_ALIGNED_VAR(static const uint16_t, _sse2_const_##name[8], 16) = \
  { \
    (uint16_t)(val7), \
    (uint16_t)(val6), \
    (uint16_t)(val5), \
    (uint16_t)(val4), \
    (uint16_t)(val3), \
    (uint16_t)(val2), \
    (uint16_t)(val1), \
    (uint16_t)(val0)  \
  }

#define SSE2_DECLARE_CONST_PI16_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint16_t, _sse2_const_##name[8], 16) = \
  { \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0), \
    (uint16_t)(val0)  \
  }

#define SSE2_DECLARE_CONST_PI32_VAR(name, val0, val1, val2, val3) \
  FOG_ALIGNED_VAR(static const uint32_t, _sse2_const_##name[4], 16) = \
  { \
    (uint32_t)(val3), \
    (uint32_t)(val2), \
    (uint32_t)(val1), \
    (uint32_t)(val0)  \
  }

#define SSE2_DECLARE_CONST_PI32_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint32_t, _sse2_const_##name[4], 16) = \
  { \
    (uint32_t)(val0), \
    (uint32_t)(val0), \
    (uint32_t)(val0), \
    (uint32_t)(val0)  \
  }

#define SSE2_DECLARE_CONST_PI64_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const uint64_t, _sse2_const_##name[2], 16) = \
  { \
    (uint64_t)(val1), \
    (uint64_t)(val0)  \
  }

#define SSE2_DECLARE_CONST_PI64_SET(name, val0) \
  FOG_ALIGNED_VAR(static const uint64_t, _sse2_const_##name[2], 16) = \
  { \
    (uint64_t)(val0), \
    (uint64_t)(val0)  \
  }

#define SSE2_DECLARE_CONST_PS_VAR(name, val0, val1, val2, val3) \
  FOG_ALIGNED_VAR(static const float, _sse2_const_##name[4], 16) = \
  { \
    (float)(val3), \
    (float)(val2), \
    (float)(val1), \
    (float)(val0)  \
  }

#define SSE2_DECLARE_CONST_PS_SET(name, val0) \
  FOG_ALIGNED_VAR(static const float, _sse2_const_##name[4], 16) = \
  { \
    (float)(val0), \
    (float)(val0), \
    (float)(val0), \
    (float)(val0)  \
  }

#define SSE2_DECLARE_CONST_PD_VAR(name, val0, val1) \
  FOG_ALIGNED_VAR(static const double, _sse2_const_##name[2], 16) = \
  { \
    val1, \
    val0  \
  }

#define SSE2_DECLARE_CONST_PD_SET(name, val0) \
  FOG_ALIGNED_VAR(static const double, _sse2_const_##name[2], 16) = \
  { \
    val0, \
    val0  \
  }

#define SSE2_GET_CONST_PD(name) (*(const __m128d*)_sse2_const_##name)
#define SSE2_GET_CONST_PS(name) (*(const __m128 *)_sse2_const_##name)
#define SSE2_GET_CONST_PI(name) (*(const __m128i*)_sse2_const_##name)

// W masks.
SSE2_DECLARE_CONST_PI16_VAR(0080008000800080_0080008000800080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080);
SSE2_DECLARE_CONST_PI16_VAR(000000FF00FF00FF_000000FF00FF00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF);
SSE2_DECLARE_CONST_PI16_VAR(000000FF00FF00FF_00FF00FF00FF00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
SSE2_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_000000FF00FF00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x00FF, 0x00FF, 0x00FF);
SSE2_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_00FF00FF00FF00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
SSE2_DECLARE_CONST_PI16_VAR(FF000000000000FF_FF000000000000FF, 0xFF00, 0x0000, 0x0000, 0x00FF, 0xFF00, 0x0000, 0x0000, 0x00FF);
SSE2_DECLARE_CONST_PI16_VAR(0101010101010101_0101010101010101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101);
SSE2_DECLARE_CONST_PI16_VAR(FFFFFFFFFFFFFFFF_FFFFFFFFFFFFFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
SSE2_DECLARE_CONST_PI16_VAR(FFFFFFFFFFFFFFFF_0000000000000000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000);
SSE2_DECLARE_CONST_PI16_VAR(00FF000000000000_00FF000000000000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000);
SSE2_DECLARE_CONST_PI16_VAR(0000000000000000_00FF000000000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000);
SSE2_DECLARE_CONST_PI16_VAR(00FF000000000000_0000000000000000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000);
SSE2_DECLARE_CONST_PI16_VAR(FF000000FF000000_FF000000FF000000, 0xFF00, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000, 0xFF00, 0x0000);
SSE2_DECLARE_CONST_PI16_VAR(000000FF000000FF_000000FF000000FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF);
SSE2_DECLARE_CONST_PI16_VAR(00000000000000FF_00000000000000FF, 0x0000, 0x0000, 0x0000, 0x00FF, 0x0000, 0x0000, 0x0000, 0x00FF);
SSE2_DECLARE_CONST_PI16_VAR(00FFFFFF00FFFFFF_00FFFFFF00FFFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF, 0x00FF, 0xFFFF);
SSE2_DECLARE_CONST_PI16_VAR(0000000100000001_0000000100000001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001);

SSE2_DECLARE_CONST_PI16_VAR(00FF00FF00FF00FF_0000000000000000, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000);
SSE2_DECLARE_CONST_PI16_VAR(0000000000000000_00FF00FF00FF00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x00FF, 0x00FF, 0x00FF);
SSE2_DECLARE_CONST_PI16_VAR(0001000200020002_0001000200020002, 0x0001, 0x0002, 0x0002, 0x0002, 0x0001, 0x0002, 0x0002, 0x0002);

SSE2_DECLARE_CONST_PI16_VAR(0000000000000000_FFFFFFFFFFFFFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
SSE2_DECLARE_CONST_PI16_VAR(0000000000000000_0101010101010101, 0x0000, 0x0000, 0x0000, 0x0000, 0x0101, 0x0101, 0x0101, 0x0101);

// RGB16 masks.
SSE2_DECLARE_CONST_PI16_VAR(0000F8000000F800_0000F8000000F800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800);
SSE2_DECLARE_CONST_PI16_VAR(0000E0000000E000_0000E0000000E000, 0x0000, 0xE000, 0x0000, 0xE000, 0x0000, 0xE000, 0x0000, 0xE000);
SSE2_DECLARE_CONST_PI16_VAR(00007C0000007C00_00007C0000007C00, 0x0000, 0x7C00, 0x0000, 0x7C00, 0x0000, 0x7C00, 0x0000, 0x7C00);
SSE2_DECLARE_CONST_PI16_VAR(0000700000007000_0000700000007000, 0x0000, 0x7000, 0x0000, 0x7000, 0x0000, 0x7000, 0x0000, 0x7000);
SSE2_DECLARE_CONST_PI16_VAR(0000380000003800_0000380000003800, 0x0000, 0x3800, 0x0000, 0x3800, 0x0000, 0x3800, 0x0000, 0x3800);
SSE2_DECLARE_CONST_PI16_VAR(00001F0000001F00_00001F0000001F00, 0x0000, 0x1F00, 0x0000, 0x1F00, 0x0000, 0x1F00, 0x0000, 0x1F00);
SSE2_DECLARE_CONST_PI16_VAR(00001C0000001C00_00001C0000001C00, 0x0000, 0x1C00, 0x0000, 0x1C00, 0x0000, 0x1C00, 0x0000, 0x1C00);
SSE2_DECLARE_CONST_PI16_VAR(000007E0000007E0_000007E0000007E0, 0x0000, 0x07E0, 0x0000, 0x07E0, 0x0000, 0x07E0, 0x0000, 0x07E0);
SSE2_DECLARE_CONST_PI16_VAR(000003E0000003E0_000003E0000003E0, 0x0000, 0x03E0, 0x0000, 0x03E0, 0x0000, 0x03E0, 0x0000, 0x03E0);
SSE2_DECLARE_CONST_PI16_VAR(000000F8000000F8_000000F8000000F8, 0x0000, 0x00F8, 0x0000, 0x00F8, 0x0000, 0x00F8, 0x0000, 0x00F8);
SSE2_DECLARE_CONST_PI16_VAR(0000007C0000007C_0000007C0000007C, 0x0000, 0x007C, 0x0000, 0x007C, 0x0000, 0x007C, 0x0000, 0x007C);
SSE2_DECLARE_CONST_PI16_VAR(0000001F0000001F_0000001F0000001F, 0x0000, 0x001F, 0x0000, 0x001F, 0x0000, 0x001F, 0x0000, 0x001F);
SSE2_DECLARE_CONST_PI16_VAR(0000000700000007_0000000700000007, 0x0000, 0x0007, 0x0000, 0x0007, 0x0000, 0x0007, 0x0000, 0x0007);
SSE2_DECLARE_CONST_PI16_VAR(0000000300000003_0000000300000003, 0x0000, 0x0003, 0x0000, 0x0003, 0x0000, 0x0003, 0x0000, 0x0003);

// D masks
SSE2_DECLARE_CONST_PI32_VAR(0001000000000000_0000000000000000, 0x00010000, 0x00000000, 0x00000000, 0x00000000);
SSE2_DECLARE_CONST_PI32_VAR(0001000000010000_0000000000000000, 0x00010000, 0x00010000, 0x00000000, 0x00000000);

// Float masks
SSE2_DECLARE_CONST_PI32_VAR(7FFFFFFF7FFFFFFF_7FFFFFFF7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF);
// 1.0, 0x0, 0x0, 0x0, for float demultiply.
SSE2_DECLARE_CONST_PI32_VAR(3F80000000000000_0000000000000000, 0x3F800000, 0x00000000, 0x00000000, 0x00000000);

// Double masks
SSE2_DECLARE_CONST_PI32_VAR(7FFFFFFFFFFFFFFF_7FFFFFFFFFFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF);
SSE2_DECLARE_CONST_PD_VAR(0_5_0_5, 0.5, 0.5);
SSE2_DECLARE_CONST_PD_VAR(1_0_1_0, 1.0, 1.0);
SSE2_DECLARE_CONST_PD_VAR(65536_0_65536_0, 65536.0, 65536.0);
SSE2_DECLARE_CONST_PD_VAR(32768_0_32768_0, 32768.0, 32768.0);

// ============================================================================
// [Fog::RasterEngine::SSE2 - Helpers]
// ============================================================================

static FOG_INLINE void sse2_mul_const_4D(__m128i& dst, const __m128i& a, const __m128i& b)
{
  __m128i tmp = _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1));

  dst = _mm_mul_epu32(a, b);
  tmp = _mm_mul_epu32(tmp, b);
  tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(2, 3, 0, 1));
  dst = _mm_or_si128(dst, tmp);
}

// Load.

template<typename SrcT>
static FOG_INLINE void sse2_load4(__m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_cvtsi32_si128(((int *)srcp)[0]);
}

template<typename SrcT>
static FOG_INLINE void sse2_load8(__m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_loadl_epi64((__m128i*)(srcp));
}

template<typename SrcT>
static FOG_INLINE void sse2_load16a(__m128i& dst0, const SrcT* srcp)
{
  dst0 = _mm_load_si128((__m128i *)(srcp));
}

template<typename SrcT>
static FOG_INLINE void sse2_load16u(__m128i& dst0, const SrcT* srcp)
{
  // Following code should be faster:
  // dst0 = _mm_loadl_epi64((__m128i*)(srcp));
  // dst0 = reinterpret_cast<__m128i>(_mm_loadh_pi(reinterpret_cast<__m128>(dst0), ((__m64*)srcp) + 1));

  dst0 = _mm_loadu_si128((__m128i *)(srcp));
}

// Store.

template<typename DstT>
static FOG_INLINE void sse2_store4(DstT* dstp, const __m128i& src0)
{
  ((int *)dstp)[0] = _mm_cvtsi128_si32(src0);
}

template<typename DstT>
static FOG_INLINE void sse2_store8(DstT* dstp, const __m128i& src0)
{
  _mm_storel_epi64((__m128i*)(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void sse2_store16a(DstT* dstp, const __m128i& src0)
{
  _mm_store_si128((__m128i *)(dstp), src0);
}

template<typename DstT>
static FOG_INLINE void sse2_store16u(DstT* dstp, const __m128i& src0)
{
  _mm_storeu_si128((__m128i *)(dstp), src0);
}

// Unpack.

static FOG_INLINE void sse2_unpack_1x1W(
  __m128i& dst0, const __m128i& data)
{
  __m128i xmmz = _mm_setzero_si128();
  dst0 = _mm_unpacklo_epi8(data, xmmz);
}

static FOG_INLINE void sse2_unpack_1x2W(
  __m128i& dst0, const __m128i& data)
{
  __m128i xmmz = _mm_setzero_si128();
  dst0 = _mm_unpacklo_epi8(data, xmmz);
}

static FOG_INLINE void sse2_unpack_1x1W(
  __m128i& dst0, uint32_t data)
{
  sse2_unpack_1x1W(dst0, _mm_cvtsi32_si128(data));
}

static FOG_INLINE void sse2_unpack_2x2W(
  __m128i& dst0,
  __m128i& dst1, __m128i data)
{
  __m128i xmmz = _mm_setzero_si128();
  dst1 = _mm_unpackhi_epi8(data, xmmz);
  dst0 = _mm_unpacklo_epi8(data, xmmz);
}

static FOG_INLINE void sse2_unpack_1x1D(
  __m128i& dst0, const __m128i& src0)
{
  __m128i xmmz = _mm_setzero_si128();
  dst0 = _mm_unpacklo_epi8(src0, xmmz);
  dst0 = _mm_unpacklo_epi16(dst0, xmmz);
}

static FOG_INLINE void sse2_unpack_1x1D(
  __m128i& dst0, uint32_t src0)
{
  sse2_unpack_1x1D(dst0, _mm_cvtsi32_si128(src0));
}

static FOG_INLINE void sse2_unpack_to_float(__m128& dst0, __m128i pix0)
{
  __m128i xmmz = _mm_setzero_si128();
  pix0 = _mm_unpacklo_epi8(pix0, xmmz);
  pix0 = _mm_unpacklo_epi16(pix0, xmmz);
  dst0 = _mm_cvtepi32_ps(pix0);
}

// Pack.

static FOG_INLINE void sse2_pack_1x1W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_packus_epi16(src0, src0);
}

static FOG_INLINE void sse2_pack_2x2W(
  __m128i& dst0, const __m128i& src0, const __m128i& src1)
{
  dst0 = _mm_packus_epi16(src0, src1);
}

static FOG_INLINE void sse2_pack_alpha(__m128i& dst0, const __m128i& src0)
{
  __m128i mmz = _mm_setzero_si128();
  dst0 = src0;

  dst0 = _mm_srli_epi32(dst0, 24);
  dst0 = _mm_packus_epi16(dst0, mmz);
  dst0 = _mm_packus_epi16(dst0, mmz);
}

static FOG_INLINE uint32_t sse2_pack_alpha_to_uint32(const __m128i& src)
{
  __m128i dst;
  sse2_pack_alpha(dst, src);
  return _mm_cvtsi128_si32(dst);
}

static FOG_INLINE void sse2_pack_1x1D(
  __m128i& dst0, __m128i& src0)
{
  dst0 = _mm_packs_epi32(src0, src0);
  dst0 = _mm_packus_epi16(dst0, dst0);
}

static FOG_INLINE void sse2_pack_from_float(__m128i& dst0, __m128 pix0)
{
  dst0 = _mm_cvtps_epi32(pix0);
  dst0 = _mm_packs_epi32(dst0, dst0);
  dst0 = _mm_packus_epi16(dst0, dst0);
}

// Combine / Split.

static FOG_INLINE void sse2_combine_1x1B_1x1B(__m128i& hi, const __m128i& lo)
{
  hi = _mm_shuffle_epi32(hi, _MM_SHUFFLE(3, 2, 0, 3));
  hi = _mm_or_si128(hi, lo);
}

static FOG_INLINE void sse2_split_1x1B_1x1B(__m128i& hi, const __m128i& lo)
{
  hi = _mm_shuffle_epi32(hi, _MM_SHUFFLE(3, 2, 3, 1));
  FOG_UNUSED(lo);
}

static FOG_INLINE void sse2_combine_1x1W_1x1W(__m128i& hi, const __m128i& lo)
{
  hi = _mm_shuffle_epi32(hi, _MM_SHUFFLE(1, 0, 3, 2));
}

static FOG_INLINE void sse2_split_1x1W_1x1W(__m128i& hi, const __m128i& lo)
{
  hi = _mm_shuffle_epi32(hi, _MM_SHUFFLE(1, 0, 3, 2));
  FOG_UNUSED(lo);
}

// Negate.

static FOG_INLINE void sse2_negate_1x1W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_xor_si128(src0, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
}

static FOG_INLINE void sse2_negate_1x2W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_xor_si128(src0, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
}

static FOG_INLINE void sse2_negate_1x2W_lo(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_xor_si128(src0, SSE2_GET_CONST_PI(0000000000000000_00FF00FF00FF00FF));
}

static FOG_INLINE void sse2_negate_1x2W_hi(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_xor_si128(src0, SSE2_GET_CONST_PI(00FF00FF00FF00FF_0000000000000000));
}

static FOG_INLINE void sse2_negate_2x2W(
  __m128i& dst0, const __m128i& src0,
  __m128i& dst1, const __m128i& src1)
{
  dst0 = _mm_xor_si128(src0, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
  dst1 = _mm_xor_si128(src1, SSE2_GET_CONST_PI(00FF00FF00FF00FF_00FF00FF00FF00FF));
}

// Swap.

static FOG_INLINE void sse2_swap_1x1W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(0, 1, 2, 3));
}

static FOG_INLINE void sse2_swap_1x2W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(0, 1, 2, 3));
  dst0 = _mm_shufflehi_epi16(dst0, _MM_SHUFFLE(0, 1, 2, 3));
}

static FOG_INLINE void sse2_swap_2x2W(
  __m128i& dst0, const __m128i& src0,
  __m128i& dst1, const __m128i& src1)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(0, 1, 2, 3));
  dst1 = _mm_shufflelo_epi16(src1, _MM_SHUFFLE(0, 1, 2, 3));
  dst0 = _mm_shufflehi_epi16(dst0, _MM_SHUFFLE(0, 1, 2, 3));
  dst1 = _mm_shufflehi_epi16(dst1, _MM_SHUFFLE(0, 1, 2, 3));
}

// Expand Pixel.

static FOG_INLINE void sse2_expand_pixel_1x1W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shuffle_epi32(src0, _MM_SHUFFLE(1, 0, 1, 0));
}

static FOG_INLINE void sse2_expand_pixel_lo_1x2W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shuffle_epi32(src0, _MM_SHUFFLE(1, 0, 1, 0));
}

static FOG_INLINE void sse2_expand_pixel_hi_1x2W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shuffle_epi32(src0, _MM_SHUFFLE(3, 2, 3, 2));
}

static FOG_INLINE void sse2_expand_pixel_lo_1x4B(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shuffle_epi32(src0, _MM_SHUFFLE(0, 0, 0, 0));
}

// Expand Alpha.

static FOG_INLINE void sse2_expand_alpha_1x4B(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_slli_epi32(src0, 24);
  dst0 = _mm_unpacklo_epi8(dst0, dst0);
  dst0 = _mm_unpacklo_epi16(dst0, dst0);
}

static FOG_INLINE void sse2_expand_alpha_1x16B(
  __m128i& dst0, const __m128i& src0)
{
  sse2_expand_alpha_1x4B(dst0, src0);
  sse2_expand_pixel_lo_1x4B(dst0, dst0);
}

static FOG_INLINE void sse2_expand_alpha_1x1W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(3, 3, 3, 3));
}

static FOG_INLINE void sse2_expand_alpha_1x2W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(3, 3, 3, 3));
  dst0 = _mm_shufflehi_epi16(dst0, _MM_SHUFFLE(3, 3, 3, 3));
}

static FOG_INLINE void sse2_expand_alpha_2x2W(
  __m128i& dst0, const __m128i& src0,
  __m128i& dst1, const __m128i& src1)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(3, 3, 3, 3));
  dst1 = _mm_shufflelo_epi16(src1, _MM_SHUFFLE(3, 3, 3, 3));
  dst0 = _mm_shufflehi_epi16(dst0, _MM_SHUFFLE(3, 3, 3, 3));
  dst1 = _mm_shufflehi_epi16(dst1, _MM_SHUFFLE(3, 3, 3, 3));
}

// Expand one alpha value and unpack in to 1x4D format.
static FOG_INLINE void sse2_expand_a8_1x16B(__m128i& dst0, uint32_t src0)
{
  dst0 = _mm_cvtsi32_si128(src0);
  dst0 = _mm_shuffle_epi32(dst0, _MM_SHUFFLE(0, 0, 0, 0));
  dst0 = _mm_packs_epi32(dst0, dst0);
  dst0 = _mm_packus_epi16(dst0, dst0);
}

static FOG_INLINE void sse2_expand_a8_1x8W(__m128i& dst0, uint32_t src0)
{
  dst0 = _mm_cvtsi32_si128(src0);
  dst0 = _mm_shuffle_epi32(dst0, _MM_SHUFFLE(0, 0, 0, 0));
  dst0 = _mm_packs_epi32(dst0, dst0);
}

// Expand Alpha Reversed.

static FOG_INLINE void sse2_expand_alpha_rev_1x1W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void sse2_expand_alpha_rev_1x1W(
  __m128i& dst0, uint32_t src0)
{
  sse2_expand_alpha_rev_1x1W(dst0, _mm_cvtsi32_si128(src0));
}

static FOG_INLINE void sse2_expand_alpha_rev_1x2W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(0, 0, 0, 0));
  dst0 = _mm_shufflehi_epi16(dst0, _MM_SHUFFLE(0, 0, 0, 0));
}

static FOG_INLINE void sse2_expand_alpha_rev_2x2W(
  __m128i& dst0, const __m128i& src0,
  __m128i& dst1, const __m128i& src1)
{
  dst0 = _mm_shufflelo_epi16(src0, _MM_SHUFFLE(0, 0, 0, 0));
  dst1 = _mm_shufflelo_epi16(src1, _MM_SHUFFLE(0, 0, 0, 0));
  dst0 = _mm_shufflehi_epi16(dst0, _MM_SHUFFLE(0, 0, 0, 0));
  dst1 = _mm_shufflehi_epi16(dst1, _MM_SHUFFLE(0, 0, 0, 0));
}

// Add.

static FOG_INLINE void sse2_adds_1x1W(
  __m128i& dst0, const __m128i& a0, const __m128i& b0)
{
  dst0 = _mm_adds_epu8(a0, b0);
}

static FOG_INLINE void sse2_adds_2x2W(
  __m128i& dst0, const __m128i& a0, const __m128i& b0,
  __m128i& dst1, const __m128i& a1, const __m128i& b1)
{
  dst0 = _mm_adds_epu8(a0, b0);
  dst1 = _mm_adds_epu8(a1, b1);
}

static FOG_INLINE void sse2_adds_1x1B(
  __m128i& dst0, const __m128i& a0, const __m128i& b0)
{
  dst0 = _mm_adds_epu8(a0, b0);
}

static FOG_INLINE void sse2_adds_1x4B(
  __m128i& dst0, const __m128i& a0, const __m128i& b0)
{
  dst0 = _mm_adds_epu8(a0, b0);
}

// Sub.

static FOG_INLINE void sse2_subs_1x1W(
  __m128i& dst0, const __m128i& a0, const __m128i& b0)
{
  dst0 = _mm_subs_epu8(a0, b0);
}

static FOG_INLINE void sse2_subs_2x2W(
  __m128i& dst0, const __m128i& a0, const __m128i& b0,
  __m128i& dst1, const __m128i& a1, const __m128i& b1)
{
  dst0 = _mm_subs_epu8(a0, b0);
  dst1 = _mm_subs_epu8(a1, b1);
}

static FOG_INLINE void sse2_subs_1x1B(
  __m128i& dst0, const __m128i& a0, const __m128i& b0)
{
  dst0 = _mm_subs_epu8(a0, b0);
}

static FOG_INLINE void sse2_subs_1x4B(
  __m128i& dst0, const __m128i& a0, const __m128i& b0)
{
  dst0 = _mm_subs_epu8(a0, b0);
}

// Expand Mask.

static FOG_INLINE void sse2_expand_mask_1x4B_to_1x4WW(
  __m128i& dst0, const __m128i& msk0)
{
  __m128i xmmz = _mm_setzero_si128();

  dst0 = _mm_unpacklo_epi8(msk0, msk0);
  dst0 = _mm_unpacklo_epi8(dst0, xmmz);
}

static FOG_INLINE void sse2_expand_mask_1x4B_to_1x4WW(
  __m128i& dst0, uint32_t msk0)
{
  sse2_expand_mask_1x4B_to_1x4WW(dst0, _mm_cvtsi32_si128(msk0));
}

static FOG_INLINE void sse2_expand_mask_1x1W(
  __m128i& dst0, uint32_t msk)
{
  sse2_expand_alpha_rev_1x1W(dst0, _mm_cvtsi32_si128(msk));
}

static FOG_INLINE void sse2_expand_mask_2x2W(
  __m128i& dst0, __m128i& dst1, const __m128i& msk0)
{
  sse2_unpack_1x1W(dst0, msk0);
  dst0 = _mm_shuffle_epi32(dst0, _MM_SHUFFLE(1, 0, 1, 0));

  dst1 = _mm_shufflelo_epi16(dst0, _MM_SHUFFLE(2, 2, 2, 2));
  dst0 = _mm_shufflelo_epi16(dst0, _MM_SHUFFLE(0, 0, 0, 0));
  dst1 = _mm_shufflehi_epi16(dst1, _MM_SHUFFLE(3, 3, 3, 3));
  dst0 = _mm_shufflehi_epi16(dst0, _MM_SHUFFLE(1, 1, 1, 1));
}

static FOG_INLINE void sse2_expand_mask_2x2W(
  __m128i& dst0, __m128i& dst1, uint32_t msk)
{
  dst0 = _mm_cvtsi32_si128(msk);
  sse2_expand_mask_2x2W(dst0, dst1, dst0);
}

static FOG_INLINE void sse2_expand_mask_1x1D(
  __m128i& dst0, uint32_t msk)
{
  dst0 = _mm_shuffle_epi32(_mm_cvtsi32_si128(msk), _MM_SHUFFLE(0, 0, 0, 0));
}

// Reverse

static FOG_INLINE void sse2_reverse_1x2W(__m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_shuffle_epi32(src0, _MM_SHUFFLE(1, 0, 3, 2));
}

// Multiply.

static FOG_INLINE void sse2_muldiv255_1x1W(
  __m128i& dst0, const __m128i& data0, const __m128i& alpha0)
{
  dst0 = _mm_mullo_epi16(data0, alpha0);
  dst0 = _mm_adds_epu16(dst0, SSE2_GET_CONST_PI(0080008000800080_0080008000800080));
  dst0 = _mm_mulhi_epu16(dst0, SSE2_GET_CONST_PI(0101010101010101_0101010101010101));
}

static FOG_INLINE void sse2_muldiv255_1x2W(
  __m128i& dst0, const __m128i& data0, const __m128i& alpha0)
{
  sse2_muldiv255_1x1W(dst0, data0, alpha0);
}

static FOG_INLINE void sse2_muldiv255_2x2W(
  __m128i& dst0, const __m128i& data0, const __m128i& alpha0,
  __m128i& dst1, const __m128i& data1, const __m128i& alpha1)
{
  dst0 = _mm_mullo_epi16(data0, alpha0);
  dst1 = _mm_mullo_epi16(data1, alpha1);
  dst0 = _mm_adds_epu16(dst0, SSE2_GET_CONST_PI(0080008000800080_0080008000800080));
  dst1 = _mm_adds_epu16(dst1, SSE2_GET_CONST_PI(0080008000800080_0080008000800080));
  dst0 = _mm_mulhi_epu16(dst0, SSE2_GET_CONST_PI(0101010101010101_0101010101010101));
  dst1 = _mm_mulhi_epu16(dst1, SSE2_GET_CONST_PI(0101010101010101_0101010101010101));
}

// Fill Alpha.

static FOG_INLINE void sse2_fill_alpha_1x1B(
  __m128i& dst0)
{
  dst0 = _mm_or_si128(dst0, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
}

static FOG_INLINE void sse2_fill_alpha_1x4B(
  __m128i& dst0)
{
  dst0 = _mm_or_si128(dst0, SSE2_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
}

static FOG_INLINE void sse2_fill_alpha_1x1W(
  __m128i& dst0)
{
  dst0 = _mm_or_si128(dst0, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
}

static FOG_INLINE void sse2_fill_alpha_1x2W(
  __m128i& dst0)
{
  dst0 = _mm_or_si128(dst0, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
}

static FOG_INLINE void sse2_fill_alpha_1x2W_lo(
  __m128i& dst0)
{
  dst0 = _mm_or_si128(dst0, SSE2_GET_CONST_PI(0000000000000000_00FF000000000000));
}

static FOG_INLINE void sse2_fill_alpha_1x2W_hi(
  __m128i& dst0)
{
  dst0 = _mm_or_si128(dst0, SSE2_GET_CONST_PI(00FF000000000000_0000000000000000));
}

static FOG_INLINE void sse2_fill_alpha_2x2W(
  __m128i& dst0,
  __m128i& dst1)
{
  dst0 = _mm_or_si128(dst0, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
  dst1 = _mm_or_si128(dst1, SSE2_GET_CONST_PI(00FF000000000000_00FF000000000000));
}

// Zero Alpha.

static FOG_INLINE void sse2_zero_alpha_1x1B(
  __m128i& dst0)
{
  dst0 = _mm_and_si128(dst0, SSE2_GET_CONST_PI(00FFFFFF00FFFFFF_00FFFFFF00FFFFFF));
}

static FOG_INLINE void sse2_zero_alpha_1x4B(
  __m128i& dst0)
{
  dst0 = _mm_and_si128(dst0, SSE2_GET_CONST_PI(00FFFFFF00FFFFFF_00FFFFFF00FFFFFF));
}

static FOG_INLINE void sse2_zero_alpha_1x1W(
  __m128i& dst0)
{
  dst0 = _mm_and_si128(dst0, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
}

static FOG_INLINE void sse2_zero_alpha_1x2W(
  __m128i& dst0)
{
  dst0 = _mm_and_si128(dst0, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
}

static FOG_INLINE void sse2_zero_alpha_1x2W_lo(
  __m128i& dst0)
{
  dst0 = _mm_and_si128(dst0, SSE2_GET_CONST_PI(00FF00FF00FF00FF_000000FF00FF00FF));
}

static FOG_INLINE void sse2_zero_alpha_1x2W_hi(
  __m128i& dst0)
{
  dst0 = _mm_and_si128(dst0, SSE2_GET_CONST_PI(000000FF00FF00FF_00FF00FF00FF00FF));
}

static FOG_INLINE void sse2_zero_alpha_2x2W(
  __m128i& dst0,
  __m128i& dst1)
{
  dst0 = _mm_and_si128(dst0, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
  dst1 = _mm_and_si128(dst1, SSE2_GET_CONST_PI(000000FF00FF00FF_000000FF00FF00FF));
}

static FOG_INLINE void sse2_zero_pixel_lo_1x1W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_and_si128(src0, SSE2_GET_CONST_PI(FFFFFFFFFFFFFFFF_0000000000000000));
}

static FOG_INLINE void sse2_zero_pixel_hi_1x1W(
  __m128i& dst0, const __m128i& src0)
{
  dst0 = _mm_and_si128(src0, SSE2_GET_CONST_PI(0000000000000000_FFFFFFFFFFFFFFFF));
}

// Premultiply.

static FOG_INLINE void sse2_premultiply_1x1W(
  __m128i& dst0, const __m128i& src0)
{
  __m128i alpha0;

  sse2_expand_alpha_1x1W(alpha0, src0);
  sse2_fill_alpha_1x1W(alpha0);
  sse2_muldiv255_1x1W(dst0, src0, alpha0);
}

static FOG_INLINE void sse2_premultiply_1x2W(
  __m128i& dst0, const __m128i& src0)
{
  __m128i alpha0;

  sse2_expand_alpha_1x2W(alpha0, src0);
  sse2_fill_alpha_1x2W(alpha0);
  sse2_muldiv255_1x2W(dst0, src0, alpha0);
}

static FOG_INLINE void sse2_premultiply_2x2W(
  __m128i& dst0, const __m128i& src0,
  __m128i& dst1, const __m128i& src1)
{
  __m128i alpha0;
  __m128i alpha1;

  sse2_expand_alpha_2x2W(
    alpha0, src0,
    alpha1, src1);
  sse2_fill_alpha_2x2W(
    alpha0,
    alpha1);
  sse2_muldiv255_2x2W(
    dst0, src0, alpha0,
    dst1, src1, alpha1);
}

// Demultiply.

static FOG_INLINE void sse2_demultiply_1x1W(__m128i& dst0, const __m128i& src0)
{
  __m128i recip;
  uint32_t index;
  uint8_t buffer[8];

  sse2_store8(buffer, src0);

  index = buffer[6];

  sse2_load8(recip, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index * 8);
  dst0 = _mm_slli_epi16(dst0, 8);
  dst0 = _mm_mulhi_epu16(dst0, recip);
}

static FOG_INLINE void sse2_demultiply_2x2W(__m128i& dst0, const __m128i& src0, __m128i& dst1, const __m128i& src1)
{
  __m128i recip0, recip1;
  uint32_t index0;
  uint32_t index1;
  uint8_t buffer[16];

  sse2_store16u(buffer, src0);

  index0 = buffer[6];
  index1 = buffer[14];

  sse2_load8(recip0, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index0 * 8);
  sse2_load8(recip1, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index1 * 8);

  sse2_store16u(buffer, src1);

  dst0 = _mm_slli_epi16(src0, 8);
  dst1 = _mm_slli_epi16(src1, 8);

  recip1 = _mm_shuffle_epi32(recip1, _MM_SHUFFLE(1, 0, 3, 2));
  recip0 = _mm_or_si128(recip0, recip1);

  dst0 = _mm_mulhi_epu16(dst0, recip0);

  index0 = buffer[6];
  index1 = buffer[14];

  sse2_load8(recip0, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index0 * 8);
  sse2_load8(recip1, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index1 * 8);

  recip1 = _mm_shuffle_epi32(recip1, _MM_SHUFFLE(1, 0, 3, 2));
  recip0 = _mm_or_si128(recip0, recip1);

  dst1 = _mm_mulhi_epu16(dst1, recip0);
}

static FOG_INLINE void sse2_demultiply_1x1W_srcbuf(__m128i& dst0, const __m128i& src0, const uint8_t* srcBuf)
{
  __m128i recip;
  uint32_t index;

  index = srcBuf[ARGB32_ABYTE];

  sse2_load8(recip, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index * 8);
  dst0 = _mm_slli_epi16(dst0, 8);
  dst0 = _mm_mulhi_epu16(dst0, recip);
}

static FOG_INLINE void sse2_demultiply_2x2W_srcbuf(__m128i& dst0, const __m128i& src0, __m128i& dst1, const __m128i& src1, const uint8_t* srcBuf)
{
  __m128i recip0, recip1;
  uint32_t index0;
  uint32_t index1;

  index0 = srcBuf[ARGB32_ABYTE];
  index1 = srcBuf[ARGB32_ABYTE + 4];

  sse2_load8(recip0, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index0 * 8);
  sse2_load8(recip1, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index1 * 8);

  dst0 = _mm_slli_epi16(src0, 8);
  dst1 = _mm_slli_epi16(src1, 8);

  recip1 = _mm_shuffle_epi32(recip1, _MM_SHUFFLE(1, 0, 3, 2));
  recip0 = _mm_or_si128(recip0, recip1);

  dst0 = _mm_mulhi_epu16(dst0, recip0);

  index0 = srcBuf[ARGB32_ABYTE + 8];
  index1 = srcBuf[ARGB32_ABYTE + 12];

  sse2_load8(recip0, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index0 * 8);
  sse2_load8(recip1, (int8_t*)ArgbUtil::demultiply_reciprocal_table_w + index1 * 8);

  recip1 = _mm_shuffle_epi32(recip1, _MM_SHUFFLE(1, 0, 3, 2));
  recip0 = _mm_or_si128(recip0, recip1);

  dst1 = _mm_mulhi_epu16(dst1, recip0);
}

// Lerp (Interpolation).

static FOG_INLINE void sse2_lerp_1x1W(
  __m128i& dst0, const __m128i& src0, const __m128i& alpha0)
{
  __m128i ialpha0;
  __m128i tmp0;

  sse2_muldiv255_1x1W(tmp0, src0, alpha0);
  sse2_negate_1x1W(ialpha0, alpha0);
  sse2_muldiv255_1x1W(dst0, dst0, ialpha0);
  sse2_adds_1x1W(dst0, dst0, tmp0);
}

static FOG_INLINE void sse2_lerp_2x2W(
  __m128i& dst0, const __m128i& src0, const __m128i& alpha0,
  __m128i& dst1, const __m128i& src1, const __m128i& alpha1)
{
  __m128i ialpha0, ialpha1;
  __m128i tmp0, tmp1;

  sse2_muldiv255_2x2W(tmp0, src0, alpha0, tmp1, src1, alpha1);
  sse2_negate_2x2W(ialpha0, alpha0, ialpha1, alpha1);
  sse2_muldiv255_2x2W(dst0, dst0, ialpha0, dst1, dst1, ialpha1);
  sse2_adds_2x2W(dst0, dst0, tmp0, dst1, dst1, tmp1);
}

static FOG_INLINE void sse2_lerp_ialpha_1x1W(
  __m128i& dst0, const __m128i& src0, const __m128i& alpha0, const __m128i& ialpha0)
{
  __m128i tmp0;

  sse2_muldiv255_1x1W(tmp0, src0, alpha0);
  sse2_muldiv255_1x1W(dst0, dst0, ialpha0);
  sse2_adds_1x1W(dst0, dst0, tmp0);
}

static FOG_INLINE void sse2_lerp_ialpha_2x2W(
  __m128i& dst0, const __m128i& src0, const __m128i& alpha0, const __m128i& ialpha0,
  __m128i& dst1, const __m128i& src1, const __m128i& alpha1, const __m128i& ialpha1)
{
  __m128i tmp0, tmp1;

  sse2_muldiv255_2x2W(tmp0, src0, alpha0, tmp1, src1, alpha1);
  sse2_muldiv255_2x2W(dst0, dst0, ialpha0, dst1, dst1, ialpha1);
  sse2_adds_2x2W(dst0, dst0, tmp0, dst1, dst1, tmp1);
}

// Over.

static FOG_INLINE void sse2_over_ialpha_1x1W(
  __m128i& dst0, const __m128i& src0, const __m128i& ialpha0)
{
  sse2_muldiv255_1x1W(dst0, dst0, ialpha0);
  sse2_adds_1x1W(dst0, dst0, src0);
}

static FOG_INLINE void sse2_over_1x1W(
  __m128i& dst0, const __m128i& src0, const __m128i& alpha0)
{
  __m128i ialpha0;

  sse2_negate_1x1W(ialpha0, alpha0);
  sse2_muldiv255_1x1W(dst0, dst0, ialpha0);
  sse2_adds_1x1W(dst0, dst0, src0);
}

static FOG_INLINE void sse2_over_ialpha_2x2W(
  __m128i& dst0, const __m128i& src0, const __m128i& ialpha0,
  __m128i& dst1, const __m128i& src1, const __m128i& ialpha1)
{
  sse2_muldiv255_2x2W(
    dst0, dst0, ialpha0,
    dst1, dst1, ialpha1);
  sse2_adds_2x2W(dst0, dst0, src0, dst1, dst1, src1);
}

static FOG_INLINE void sse2_over_2x2W(
  __m128i& dst0, const __m128i& src0, const __m128i& alpha0,
  __m128i& dst1, const __m128i& src1, const __m128i& alpha1)
{
  __m128i ialpha0;
  __m128i ialpha1;

  sse2_negate_2x2W(
    ialpha0, alpha0,
    ialpha1, alpha1);
  sse2_muldiv255_2x2W(
    dst0, dst0, ialpha0,
    dst1, dst1, ialpha1);
  sse2_adds_2x2W(dst0, dst0, src0, dst1, dst1, src1);
}

static FOG_INLINE void sse2_over_1x1W(
  __m128i& dst0, __m128i& src0)
{
  __m128i src0ia;

  sse2_expand_alpha_1x1W(src0ia, src0);
  sse2_negate_1x1W(src0ia, src0ia);
  sse2_over_ialpha_1x1W(dst0, src0, src0ia);
}

static FOG_INLINE void sse2_over_2x2W(
  __m128i& dst0, __m128i& src0,
  __m128i& dst1, __m128i& src1)
{
  __m128i src0ia;
  __m128i src1ia;

  sse2_expand_alpha_2x2W(src0ia, src0, src1ia, src1);
  sse2_negate_2x2W(src0ia, src0ia, src1ia, src1ia);
  sse2_over_ialpha_2x2W(dst0, src0, src0ia, dst1, src1, src1ia);
}

// Fetch RGB24/BGR24.

static FOG_INLINE void sse2_fetch_rgb24_1x1W(__m128i& dst0, const uint8_t* srcp)
{
  sse2_unpack_1x1W(dst0, READ_32(srcp));
  sse2_fill_alpha_1x1W(dst0);
}

static FOG_INLINE void sse2_fetch_rgb24_2x2W(__m128i& dst0, __m128i& dst1, const uint8_t* srcp)
{
  __m128i xmmz = _mm_setzero_si128();

  sse2_load8(dst0, srcp + 0);                                 // dst0 = [G2 B2 R1 G1 B1 R0 G0 B0]
  sse2_load8(dst1, srcp + 4);                                 // dst1 = [R3 G3 B3 R2 G2 B2 R1 G1]

  dst0 = _mm_slli_epi64(dst0, 8);                            // dst0 = [B2 R1 G1 B1 R0 G0 B0   ]
  dst1 = _mm_srli_epi64(dst1, 8);                            // dst1 = [   R3 G3 B3 R2 G2 B2 R1]

  dst0 = _mm_or_si128(dst0, SSE2_GET_CONST_PI(FF000000000000FF_FF000000000000FF));           // dst0 = [FF R1 G1 B1 R0 G0 B0 FF]
  dst1 = _mm_or_si128(dst1, SSE2_GET_CONST_PI(FF000000000000FF_FF000000000000FF));           // dst1 = [FF R3 G3 B3 R2 G2 B2 FF]

  dst0 = _mm_unpacklo_epi8(dst0, xmmz);
  dst1 = _mm_unpacklo_epi8(dst1, xmmz);

  dst0 = _mm_shufflelo_epi16(dst0, _MM_SHUFFLE(0, 3, 2, 1)); // dst0 = [FF R1 G1 B1 FF R0 G0 B0]
  dst1 = _mm_shufflelo_epi16(dst1, _MM_SHUFFLE(0, 3, 2, 1)); // dst1 = [FF R3 G3 B3 FF R2 G2 B2]
}

static FOG_INLINE void sse2_fetch_bgr24_2x2W(__m128i& dst0, __m128i& dst1, const uint8_t* srcp)
{
  __m128i xmmz = _mm_setzero_si128();

  sse2_load8(dst0, srcp + 0);                                 // dst0 = [G2 R2 B1 G1 R1 B0 G0 R0]
  sse2_load8(dst1, srcp + 4);                                 // dst1 = [B3 G3 R3 B2 G2 R2 B1 G1]

  dst0 = _mm_slli_epi64(dst0, 8);                            // dst0 = [R2 B1 G1 R1 B0 G0 R0   ]
  dst1 = _mm_srli_epi64(dst1, 8);                            // dst1 = [   B3 G3 R3 B2 G2 R2 B1]

  dst0 = _mm_or_si128(dst0, SSE2_GET_CONST_PI(FF000000000000FF_FF000000000000FF));           // dst0 = [FF B1 G1 R1 B0 G0 R0 FF]
  dst1 = _mm_or_si128(dst1, SSE2_GET_CONST_PI(FF000000000000FF_FF000000000000FF));           // dst1 = [FF B3 G3 R3 B2 G2 R2 FF]

  dst0 = _mm_unpacklo_epi8(dst0, xmmz);
  dst1 = _mm_unpacklo_epi8(dst1, xmmz);

  dst0 = _mm_shufflelo_epi16(dst0, _MM_SHUFFLE(0, 1, 2, 3)); // dst0 = [FF B1 G1 R1 FF R0 G0 B0]
  dst1 = _mm_shufflelo_epi16(dst1, _MM_SHUFFLE(0, 1, 2, 3)); // dst1 = [FF B3 G3 R3 FF R2 G2 B2]

  dst0 = _mm_shufflehi_epi16(dst0, _MM_SHUFFLE(0, 1, 2, 3)); // dst0 = [FF R1 G1 B1 FF R0 G0 B0]
  dst1 = _mm_shufflehi_epi16(dst1, _MM_SHUFFLE(0, 1, 2, 3)); // dst1 = [FF R3 G3 B3 FF R2 G2 B2]
}

// Mask analyzer.

static FOG_INLINE void sse2_analyze_mask_16B_zero(uint32_t& msk0isZero, const __m128i& msk0)
{
  __m128i t0;

  t0 = _mm_setzero_si128();
  t0 = _mm_cmpeq_epi8(t0, msk0);
  msk0isZero = _mm_movemask_epi8(t0);
}

static FOG_INLINE void sse2_analyze_mask_16B_full(uint32_t& msk0isFull, const __m128i& msk0)
{
  __m128i t0;

  t0 = _mm_setzero_si128();
  t0 = _mm_cmpeq_epi8(t0, t0);
  t0 = _mm_cmpeq_epi8(t0, msk0);
  msk0isFull = _mm_movemask_epi8(t0);
}

static FOG_INLINE void sse2_analyze_mask_16B(uint32_t& msk0IsZero, uint32_t& msk0IsFull, const __m128i& msk0)
{
  __m128i t0;
  __m128i t1;

  t1 = _mm_setzero_si128();
  t0 = _mm_setzero_si128();
  t1 = _mm_cmpeq_epi8(t1, t1);
  t0 = _mm_cmpeq_epi8(t0, msk0);
  t1 = _mm_cmpeq_epi8(t1, msk0);

  msk0IsZero = _mm_movemask_epi8(t0);
  msk0IsFull = _mm_movemask_epi8(t1);
}

// Misc / Unsorted.

static FOG_INLINE void sse2_load_1xA8(__m128i& dst0, const uint8_t* p)
{
  dst0 = _mm_cvtsi32_si128(p[0]);
}

static FOG_INLINE void sse2_load_4xA8(__m128i& dst0, const uint8_t* p)
{
  sse2_load4(dst0, p);
  sse2_unpack_1x1D(dst0, dst0);
  dst0 = _mm_slli_epi32(dst0, 24);
}

static FOG_INLINE void sse2_load_1xI8(__m128i& dst0, const uint8_t* p, const Argb* pal)
{
  const uint8_t* p0 = (uint8_t*)(&pal[p[0]]);
  sse2_load4(dst0, p0);
}

static FOG_INLINE void sse2_load_4xI8(__m128i& dst0, const uint8_t* p, const Argb* pal)
{
  __m128i dst1;
  __m128i dst2;

  const uint8_t* p0;
  const uint8_t* p1;

  p0 = (uint8_t*)(&pal[p[0]]);
  p1 = (uint8_t*)(&pal[p[1]]);

  sse2_load4(dst0, p0);
  sse2_load4(dst1, p1);

  dst1 = _mm_shuffle_epi32(dst1, _MM_SHUFFLE(3, 3, 0, 3));

  p0 = (uint8_t*)(&pal[p[2]]);
  p1 = (uint8_t*)(&pal[p[3]]);

  dst0 = _mm_or_si128(dst0, dst1);

  sse2_load4(dst1, p0);
  sse2_load4(dst2, p1);

  dst1 = _mm_shuffle_epi32(dst1, _MM_SHUFFLE(3, 0, 3, 3));
  dst2 = _mm_shuffle_epi32(dst2, _MM_SHUFFLE(0, 3, 3, 3));

  dst1 = _mm_or_si128(dst1, dst2);
  dst0 = _mm_or_si128(dst0, dst1);
}

static FOG_INLINE void sse2_load_and_unpack_axxx32_64B(__m128i& dst0, __m128i& dst1, const uint8_t* src)
{
  __m128i dst2;

  sse2_load16u(dst0, src);
  sse2_load16u(dst1, src + 16);
  dst0 = _mm_srli_epi32(dst0, 24);
  dst1 = _mm_srli_epi32(dst1, 24);
  dst0 = _mm_packs_epi32(dst0, dst1);

  sse2_load16u(dst1, src + 32);
  sse2_load16u(dst2, src + 48);
  dst1 = _mm_srli_epi32(dst1, 24);
  dst2 = _mm_srli_epi32(dst1, 24);
  dst1 = _mm_packs_epi32(dst1, dst2);
}

} // RasterEngine namespace
} // Fog namespace
