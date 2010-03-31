// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Build/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::MMX - Dib]
// ============================================================================

struct FOG_HIDDEN MMX_SYM(Dib)
{
  // --------------------------------------------------------------------------
  // [DibMMX - MemCpy]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL memcpy32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i = w;
    FOG_ASSERT(w);

    if ((i -= 8) >= 0)
    {
      if ((sysuint_t)dst & 0x7)
      {
        copy4(dst, src); dst += 4; src += 4;
        if (--i < 0) goto skip;
      }

      do {
        __m64 m0, m1, m2, m3;

        mmx_load8(m0, src     );
        mmx_load8(m1, src + 8 );
        mmx_store8(dst     , m0);
        mmx_store8(dst + 8 , m1);

        mmx_load8(m2, src + 16);
        mmx_load8(m3, src + 24);
        mmx_store8(dst + 16, m2);
        mmx_store8(dst + 24, m3);

        dst += 32;
        src += 32;
      } while ((i -= 8) >= 0);

      mmx_end();
    }
skip:
    i += 8;

    switch (i)
    {
      case 7: copy4(dst, src); dst += 4; src += 4;
      case 6: copy4(dst, src); dst += 4; src += 4;
      case 5: copy4(dst, src); dst += 4; src += 4;
      case 4: copy4(dst, src); dst += 4; src += 4;
      case 3: copy4(dst, src); dst += 4; src += 4;
      case 2: copy4(dst, src); dst += 4; src += 4;
      case 1: copy4(dst, src); dst += 4; src += 4;
    }
  }

  // --------------------------------------------------------------------------
  // [DibMMX - Convert]
  // --------------------------------------------------------------------------

  // XXXXXXXX RRRRRRRR GGGGGGGG BBBBBBBB ->
  //                   RRRRRGGG GGGBBBBB
  static void FOG_FASTCALL rgb16_565_native_from_xrgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i;
    FOG_ASSERT(w);

    // Align destination to 8 bytes.
    while ((sysuint_t)dst & 7)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 8) & 0xF800) |
        ((pix0 >> 5) & 0x07E0) |
        ((pix0 >> 3) & 0x001F));
      if (--w == 0) return;

      dst += 2;
      src += 4;
    }

    for (i = w >> 3; i; i--, dst += 16, src += 32)
    {
      __m64 pix0mmR, pix1mmR;
      __m64 pix0mmG, pix1mmG;
      __m64 pix0mmB, pix1mmB;

      mmx_load8(pix0mmB, src);
      mmx_load8(pix1mmB, src + 8);

      pix0mmR = _mm_srli_pi32(pix0mmB, 9);
      pix1mmR = _mm_srli_pi32(pix1mmB, 9);

      pix0mmG = _mm_srli_pi32(pix0mmB, 5);
      pix1mmG = _mm_srli_pi32(pix1mmB, 5);

      pix0mmB = _mm_srli_pi32(pix0mmB, 3);
      pix1mmB = _mm_srli_pi32(pix1mmB, 3);

      pix0mmR = _mm_and_si64(pix0mmR, MMX_GET_CONST(00007C0000007C00));
      pix1mmR = _mm_and_si64(pix1mmR, MMX_GET_CONST(00007C0000007C00));

      pix0mmG = _mm_and_si64(pix0mmG, MMX_GET_CONST(000007E0000007E0));
      pix1mmG = _mm_and_si64(pix1mmG, MMX_GET_CONST(000007E0000007E0));

      pix0mmB = _mm_and_si64(pix0mmB, MMX_GET_CONST(0000001F0000001F));
      pix1mmB = _mm_and_si64(pix1mmB, MMX_GET_CONST(0000001F0000001F));

      pix0mmB = _mm_or_si64(pix0mmB, pix0mmG);
      pix1mmB = _mm_or_si64(pix1mmB, pix1mmG);

      pix0mmR = _mm_packs_pi32(pix0mmR, pix1mmR);
      pix0mmB = _mm_packs_pi32(pix0mmB, pix1mmB);

      pix0mmR = _mm_slli_pi16(pix0mmR, 1);
      pix0mmB = _mm_or_si64(pix0mmB, pix0mmR);

      mmx_store8(dst, pix0mmB);

      mmx_load8(pix0mmB, src + 16);
      mmx_load8(pix1mmB, src + 24);

      pix0mmR = _mm_srli_pi32(pix0mmB, 9);
      pix1mmR = _mm_srli_pi32(pix1mmB, 9);

      pix0mmG = _mm_srli_pi32(pix0mmB, 5);
      pix1mmG = _mm_srli_pi32(pix1mmB, 5);

      pix0mmB = _mm_srli_pi32(pix0mmB, 3);
      pix1mmB = _mm_srli_pi32(pix1mmB, 3);

      pix0mmR = _mm_and_si64(pix0mmR, MMX_GET_CONST(00007C0000007C00));
      pix1mmR = _mm_and_si64(pix1mmR, MMX_GET_CONST(00007C0000007C00));

      pix0mmG = _mm_and_si64(pix0mmG, MMX_GET_CONST(000007E0000007E0));
      pix1mmG = _mm_and_si64(pix1mmG, MMX_GET_CONST(000007E0000007E0));

      pix0mmB = _mm_and_si64(pix0mmB, MMX_GET_CONST(0000001F0000001F));
      pix1mmB = _mm_and_si64(pix1mmB, MMX_GET_CONST(0000001F0000001F));

      pix0mmB = _mm_or_si64(pix0mmB, pix0mmG);
      pix1mmB = _mm_or_si64(pix1mmB, pix1mmG);

      pix0mmR = _mm_packs_pi32(pix0mmR, pix1mmR);
      pix0mmB = _mm_packs_pi32(pix0mmB, pix1mmB);

      pix0mmR = _mm_slli_pi16(pix0mmR, 1);
      pix0mmB = _mm_or_si64(pix0mmB, pix0mmR);

      mmx_store8(dst + 8, pix0mmB);
    }

    for (i = w & 7; i; i--, dst += 2, src += 4)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 8) & 0xF800) |
        ((pix0 >> 5) & 0x07E0) |
        ((pix0 >> 3) & 0x001F));
    }

    mmx_end();
  }

  // XXXXXXXX RRRRRRRR GGGGGGGG BBBBBBBB ->
  //                   XRRRRRGG GGGBBBBB
  static void FOG_FASTCALL rgb16_555_native_from_xrgb32(
    uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure)
  {
    sysint_t i;
    FOG_ASSERT(w);

    // Align destination to 8 bytes.
    while ((sysuint_t)dst & 7)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 9) & 0x7C00) |
        ((pix0 >> 6) & 0x03E0) |
        ((pix0 >> 3) & 0x001F));
      if (--w == 0) return;

      dst += 2;
      src += 4;
    }

    for (i = w >> 3; i; i--, dst += 16, src += 32)
    {
      __m64 pix0mmR, pix1mmR;
      __m64 pix0mmG, pix1mmG;
      __m64 pix0mmB, pix1mmB;

      mmx_load8(pix0mmB, src);
      mmx_load8(pix1mmB, src + 8);

      pix0mmR = _mm_srli_pi32(pix0mmB, 9);
      pix1mmR = _mm_srli_pi32(pix1mmB, 9);

      pix0mmG = _mm_srli_pi32(pix0mmB, 6);
      pix1mmG = _mm_srli_pi32(pix1mmB, 6);

      pix0mmB = _mm_srli_pi32(pix0mmB, 3);
      pix1mmB = _mm_srli_pi32(pix1mmB, 3);

      pix0mmR = _mm_and_si64(pix0mmR, MMX_GET_CONST(00007C0000007C00));
      pix1mmR = _mm_and_si64(pix1mmR, MMX_GET_CONST(00007C0000007C00));

      pix0mmG = _mm_and_si64(pix0mmG, MMX_GET_CONST(000003E0000003E0));
      pix1mmG = _mm_and_si64(pix1mmG, MMX_GET_CONST(000003E0000003E0));

      pix0mmB = _mm_and_si64(pix0mmB, MMX_GET_CONST(0000001F0000001F));
      pix1mmB = _mm_and_si64(pix1mmB, MMX_GET_CONST(0000001F0000001F));

      pix0mmB = _mm_or_si64(pix0mmB, pix0mmR);
      pix1mmB = _mm_or_si64(pix1mmB, pix1mmR);

      mmx_load8(pix0mmR, src + 16);
      mmx_load8(pix1mmR, src + 24);

      pix0mmB = _mm_or_si64(pix0mmB, pix0mmG);
      pix1mmB = _mm_or_si64(pix1mmB, pix1mmG);

      pix0mmB = _mm_packs_pi32(pix0mmB, pix1mmB);
      mmx_store8(dst, pix0mmB);

      pix0mmG = _mm_srli_pi32(pix0mmR, 6);
      pix1mmG = _mm_srli_pi32(pix1mmR, 6);

      pix0mmB = _mm_srli_pi32(pix0mmR, 3);
      pix1mmB = _mm_srli_pi32(pix1mmR, 3);

      pix0mmR = _mm_srli_pi32(pix0mmR, 9);
      pix1mmR = _mm_srli_pi32(pix1mmR, 9);

      pix0mmR = _mm_and_si64(pix0mmR, MMX_GET_CONST(00007C0000007C00));
      pix1mmR = _mm_and_si64(pix1mmR, MMX_GET_CONST(00007C0000007C00));

      pix0mmG = _mm_and_si64(pix0mmG, MMX_GET_CONST(000003E0000003E0));
      pix1mmG = _mm_and_si64(pix1mmG, MMX_GET_CONST(000003E0000003E0));

      pix0mmB = _mm_and_si64(pix0mmB, MMX_GET_CONST(0000001F0000001F));
      pix1mmB = _mm_and_si64(pix1mmB, MMX_GET_CONST(0000001F0000001F));

      pix0mmB = _mm_or_si64(pix0mmB, pix0mmR);
      pix1mmB = _mm_or_si64(pix1mmB, pix1mmR);

      pix0mmB = _mm_or_si64(pix0mmB, pix0mmG);
      pix1mmB = _mm_or_si64(pix1mmB, pix1mmG);

      pix0mmB = _mm_packs_pi32(pix0mmB, pix1mmB);
      mmx_store8(dst + 8, pix0mmB);
    }

    for (i = w & 7; i; i--, dst += 2, src += 4)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 9) & 0x7C00) |
        ((pix0 >> 6) & 0x03E0) |
        ((pix0 >> 3) & 0x001F));
    }

    mmx_end();
  }
};

} // RasterEngine namespace
} // Fog namespace

