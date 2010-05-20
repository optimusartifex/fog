// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_SSE2_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::SSE2 - Dib]
// ============================================================================

//! @internal
struct FOG_HIDDEN DibSSE2
{
  // --------------------------------------------------------------------------
  // [DibSSE2 - MemCpy]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL memcpy8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    int i;

    if (w < 16)
    {
      for (i = w; i; i--, dst++, src++) dst[0] = src[0];
    }
    else
    {
      int align = (int)((16 - (sysuint_t(dst) & 15)) & 15);
      w -= align;

      for (i = align; i; i--)
      {
        Memory::copy1B(dst, src);
        dst++;
        src++;
      }

      if (((sysuint_t)src & 15) == 0)
      {
        for (i = w >> 6; i; i--)
        {
          __m128i src0mm;
          __m128i src1mm;
          __m128i src2mm;
          __m128i src3mm;

          _mm_prefetch((const char*)(src + 64), _MM_HINT_T0);

          sse2_load16a(src0mm, src + 0);
          sse2_load16a(src1mm, src + 16);
          sse2_load16a(src2mm, src + 32);
          sse2_load16a(src3mm, src + 48);
          sse2_store16a(dst + 0, src0mm);
          sse2_store16a(dst + 16, src1mm);
          sse2_store16a(dst + 32, src2mm);
          sse2_store16a(dst + 48, src3mm);

          dst += 64;
          src += 64;
        }
      }
      else
      {
        for (i = w >> 6; i; i--)
        {
          __m128i src0mm;
          __m128i src1mm;
          __m128i src2mm;
          __m128i src3mm;

          _mm_prefetch((const char*)(src + 64), _MM_HINT_T0);

          sse2_load16u(src0mm, src + 0);
          sse2_load16u(src1mm, src + 16);
          sse2_load16u(src2mm, src + 32);
          sse2_load16u(src3mm, src + 48);
          sse2_store16a(dst + 0, src0mm);
          sse2_store16a(dst + 16, src1mm);
          sse2_store16a(dst + 32, src2mm);
          sse2_store16a(dst + 48, src3mm);

          dst += 64;
          src += 64;
        }
      }

      w &= 63;

      switch (w >> 2)
      {
        case 15: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 14: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 13: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 12: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 11: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 10: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  9: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  8: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  7: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  6: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  5: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  4: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  3: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  2: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  1: Memory::copy4B(dst, src); dst += 4; src += 4;
      }

      switch (w & 3)
      {
        case  3: Memory::copy1B(dst, src); dst += 1; src += 1;
        case  2: Memory::copy1B(dst, src); dst += 1; src += 1;
        case  1: Memory::copy1B(dst, src); dst += 1; src += 1;
      }
    }
  }

  static void FOG_FASTCALL memcpy16(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    memcpy8(dst, src, w * 2, closure);
  }

  static void FOG_FASTCALL memcpy24(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    memcpy8(dst, src, w * 3, closure);
  }

  static void FOG_FASTCALL memcpy32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    while (sysuint_t(dst) & 15)
    {
      Memory::copy4B(dst, src);
      if (--w == 0) return;
      dst += 4;
      src += 4;
    }

    if (sysuint_t(src) & 15)
    {
      while (w >= 16)
      {
        __m128i src0mm;
        __m128i src1mm;
        __m128i src2mm;
        __m128i src3mm;

        _mm_prefetch((const char*)(src + 64), _MM_HINT_T0);

        sse2_load16u(src0mm, src + 0);
        sse2_load16u(src1mm, src + 16);
        sse2_load16u(src2mm, src + 32);
        sse2_load16u(src3mm, src + 48);
        sse2_store16a(dst + 0, src0mm);
        sse2_store16a(dst + 16, src1mm);
        sse2_store16a(dst + 32, src2mm);
        sse2_store16a(dst + 48, src3mm);

        dst += 64;
        src += 64;
        w -= 16;
      }
    }
    else
    {
      while (w >= 16)
      {
        __m128i src0mm;
        __m128i src1mm;
        __m128i src2mm;
        __m128i src3mm;

        _mm_prefetch((const char*)(src + 64), _MM_HINT_T0);

        sse2_load16a(src0mm, src + 0);
        sse2_load16a(src1mm, src + 16);
        sse2_load16a(src2mm, src + 32);
        sse2_load16a(src3mm, src + 48);
        sse2_store16a(dst + 0, src0mm);
        sse2_store16a(dst + 16, src1mm);
        sse2_store16a(dst + 32, src2mm);
        sse2_store16a(dst + 48, src3mm);

        dst += 64;
        src += 64;
        w -= 16;
      }
    }

    switch (w & 15)
    {
      case 15: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 14: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 13: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 12: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 11: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 10: Memory::copy4B(dst, src); dst += 4; src += 4;
      case  9: Memory::copy4B(dst, src); dst += 4; src += 4;
      case  8: Memory::copy4B(dst, src); dst += 4; src += 4;
      case  7: Memory::copy4B(dst, src); dst += 4; src += 4;
      case  6: Memory::copy4B(dst, src); dst += 4; src += 4;
      case  5: Memory::copy4B(dst, src); dst += 4; src += 4;
      case  4: Memory::copy4B(dst, src); dst += 4; src += 4;
      case  3: Memory::copy4B(dst, src); dst += 4; src += 4;
      case  2: Memory::copy4B(dst, src); dst += 4; src += 4;
      case  1: Memory::copy4B(dst, src); dst += 4; src += 4;
    }
  }

  // --------------------------------------------------------------------------
  // [DibSSE2 - CBlit - Rect]
  // --------------------------------------------------------------------------

#if 1
  static void cblit_rect_32_helper(
    uint8_t* dst, sysint_t dstStride,
    uint32_t src0,
    int w, int h)
  {
    FOG_ASSERT(w > 0 && h > 0);

    if (w < 8)
    {
      dst -= (7 - w) * 4;

      do {
        switch (w)
        {
          case 7: ((uint32_t*)dst)[0] = src0;
          case 6: ((uint32_t*)dst)[1] = src0;
          case 5: ((uint32_t*)dst)[2] = src0;
          case 4: ((uint32_t*)dst)[3] = src0;
          case 3: ((uint32_t*)dst)[4] = src0;
          case 2: ((uint32_t*)dst)[5] = src0;
          case 1: ((uint32_t*)dst)[6] = src0;
            break;
          default:
            FOG_ASSERT_NOT_REACHED();
        }
        dst += dstStride;
      } while (--h);
    }
    else
    {
      __m128i src0xmm = _mm_cvtsi32_si128(src0);
      sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

      dstStride -= w * 4;

      // For 4-byte store we are using general purpose register instead of sse2
      // one, because the instruction is smaller and MSVC is unable to generate
      // good code.
      do {
        int i = w;

        sse2_prefetch_t0(dst + 64);

        while (sysuint_t(dst) & 15)
        {
          ((uint32_t*)dst)[0] = src0;
          dst += 4;
          if (--i == 0) goto largeWidthSkip;
        }

        while (i >= 32)
        {
          sse2_prefetch_t0(dst + 128);

          sse2_store16a(dst +   0, src0xmm);
          sse2_store16a(dst +  16, src0xmm);
          sse2_store16a(dst +  32, src0xmm);
          sse2_store16a(dst +  48, src0xmm);
          sse2_store16a(dst +  64, src0xmm);
          sse2_store16a(dst +  80, src0xmm);
          sse2_store16a(dst +  96, src0xmm);
          sse2_store16a(dst + 112, src0xmm);

          dst += 128;
          i -= 32;
        }

        if (i >= 16)
        {
          sse2_prefetch_t0(dst + 64);

          sse2_store16a(dst +   0, src0xmm);
          sse2_store16a(dst +  16, src0xmm);
          sse2_store16a(dst +  32, src0xmm);
          sse2_store16a(dst +  48, src0xmm);

          dst += 64;
          i -= 16;
        }

        if (i >= 8)
        {
          sse2_store16a(dst +   0, src0xmm);
          sse2_store16a(dst +  16, src0xmm);

          dst += 32;
          i -= 8;
        }

        switch (i)
        {
          case  7: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  6: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  5: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  4: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  3: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  2: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  1: ((uint32_t*)dst)[0] = src0; dst += 4;
        }

largeWidthSkip:
        dst += dstStride;
      } while (--h);
    }
  }
#endif

#if 0
  // This is cblit_rect_32 version that can prefetch next scanline, instead of
  // prefetching next bytes. The benchmarks say some speed improvement, but I'm
  // not sure how the old hardware behaves, the improvement isn't so great to
  // turn it on now.
  //
  // - Petr
  static void cblit_rect_32_helper(
    uint8_t* dst, sysint_t dstStride,
    uint32_t src0,
    int w, int h)
  {
    FOG_ASSERT(w > 0 && h > 0);

    if (w < 8)
    {
      dst -= (7 - w) * 4;

      do {
        switch (w)
        {
          case 7: ((uint32_t*)dst)[0] = src0;
          case 6: ((uint32_t*)dst)[1] = src0;
          case 5: ((uint32_t*)dst)[2] = src0;
          case 4: ((uint32_t*)dst)[3] = src0;
          case 3: ((uint32_t*)dst)[4] = src0;
          case 2: ((uint32_t*)dst)[5] = src0;
          case 1: ((uint32_t*)dst)[6] = src0;
            break;
          default:
            FOG_ASSERT_NOT_REACHED();
        }
        dst += dstStride;
      } while (--h);
    }
    else
    {
      uint8_t* dstNext = dst + dstStride;

      __m128i src0xmm = _mm_cvtsi32_si128(src0);
      sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

      dstStride -= w * 4;

      // For 4-byte store we are using general purpose register instead of sse2
      // one, because the instruction is smaller and MSVC is unable to generate
      // good code.

      // Do all scanlines except the last one.
      while (--h)
      {
        int i = w;

        sse2_prefetch_t0(dstNext);

        while (sysuint_t(dst) & 15)
        {
          ((uint32_t*)dst)[0] = src0;
          dst += 4;
          dstNext += 4;
          if (--i == 0) goto largeWidthSkip;
        }

        while (i >= 32)
        {
          sse2_prefetch_t0(dstNext);

          sse2_store16a(dst +   0, src0xmm);
          sse2_store16a(dst +  16, src0xmm);
          sse2_store16a(dst +  32, src0xmm);
          sse2_store16a(dst +  48, src0xmm);
          sse2_prefetch_t0(dstNext + 64);
          sse2_store16a(dst +  64, src0xmm);
          sse2_store16a(dst +  80, src0xmm);
          sse2_store16a(dst +  96, src0xmm);
          sse2_store16a(dst + 112, src0xmm);

          dst += 128;
          dstNext += 128;
          i -= 32;
        }

        if (i >= 16)
        {
          sse2_prefetch_t0(dstNext);

          sse2_store16a(dst +   0, src0xmm);
          sse2_store16a(dst +  16, src0xmm);
          sse2_store16a(dst +  32, src0xmm);
          sse2_store16a(dst +  48, src0xmm);

          dst += 64;
          dstNext += 64;
          i -= 16;
        }

        if (i >= 8)
        {
          sse2_store16a(dst +   0, src0xmm);
          sse2_store16a(dst +  16, src0xmm);

          dst += 32;
          dstNext += 32;
          i -= 8;
        }

        switch (i)
        {
          case  7: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  6: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  5: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  4: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  3: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  2: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  1: ((uint32_t*)dst)[0] = src0; dst += 4;
        }
        dstNext += i * 4;

largeWidthSkip:
        dst += dstStride;
        dstNext += dstStride;
      }

      {
        int i = w;

        sse2_prefetch_t0(dst + 64);

        while (sysuint_t(dst) & 15)
        {
          ((uint32_t*)dst)[0] = src0;
          dst += 4;
          if (--i == 0) goto tailWidthSkip;
        }

        while (i >= 32)
        {
          sse2_prefetch_t0(dst + 128);

          sse2_store16a(dst +   0, src0xmm);
          sse2_store16a(dst +  16, src0xmm);
          sse2_store16a(dst +  32, src0xmm);
          sse2_store16a(dst +  48, src0xmm);
          sse2_store16a(dst +  64, src0xmm);
          sse2_store16a(dst +  80, src0xmm);
          sse2_store16a(dst +  96, src0xmm);
          sse2_store16a(dst + 112, src0xmm);

          dst += 128;
          i -= 32;
        }

        if (i >= 16)
        {
          sse2_prefetch_t0(dst + 64);

          sse2_store16a(dst +   0, src0xmm);
          sse2_store16a(dst +  16, src0xmm);
          sse2_store16a(dst +  32, src0xmm);
          sse2_store16a(dst +  48, src0xmm);

          dst += 64;
          i -= 16;
        }

        if (i >= 8)
        {
          sse2_store16a(dst +   0, src0xmm);
          sse2_store16a(dst +  16, src0xmm);

          dst += 32;
          i -= 8;
        }

        switch (i)
        {
          case  7: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  6: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  5: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  4: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  3: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  2: ((uint32_t*)dst)[0] = src0; dst += 4;
          case  1: ((uint32_t*)dst)[0] = src0; dst += 4;
        }

tailWidthSkip:
        dst += dstStride;
      }
    }
  }
#endif

  static void FOG_INLINE cblit_rect_32_nt_helper(
    uint8_t* dst, sysint_t dstStride,
    uint32_t src0,
    int w, int h)
  {
    FOG_ASSERT(w > 0 && h > 0);
    dstStride -= w * 4;

    __m128i src0xmm = _mm_cvtsi32_si128(src0);
    sse2_expand_pixel_lo_1x4B(src0xmm, src0xmm);

    do {
      int i = w;

      while (sysuint_t(dst) & 15)
      {
        sse2_store4_nt(dst, src0);
        dst += 4;
        if (--i == 0) goto skip;
      }

      while (i >= 32)
      {
        sse2_store16a_nt(dst +   0, src0xmm);
        sse2_store16a_nt(dst +  16, src0xmm);
        sse2_store16a_nt(dst +  32, src0xmm);
        sse2_store16a_nt(dst +  48, src0xmm);
        sse2_store16a_nt(dst +  64, src0xmm);
        sse2_store16a_nt(dst +  80, src0xmm);
        sse2_store16a_nt(dst +  96, src0xmm);
        sse2_store16a_nt(dst + 112, src0xmm);

        dst += 128;
        i -= 32;
      }

      if (i >= 16)
      {
        sse2_store16a_nt(dst +   0, src0xmm);
        sse2_store16a_nt(dst +  16, src0xmm);
        sse2_store16a_nt(dst +  32, src0xmm);
        sse2_store16a_nt(dst +  48, src0xmm);

        dst += 64;
        i -= 16;
      }

      if (i >= 8)
      {
        sse2_store16a_nt(dst +   0, src0xmm);
        sse2_store16a_nt(dst +  16, src0xmm);

        dst += 32;
        i -= 8;
      }

      switch (i)
      {
        case  7: sse2_store4_nt(dst, src0); dst += 4;
        case  6: sse2_store4_nt(dst, src0); dst += 4;
        case  5: sse2_store4_nt(dst, src0); dst += 4;
        case  4: sse2_store4_nt(dst, src0); dst += 4;
        case  3: sse2_store4_nt(dst, src0); dst += 4;
        case  2: sse2_store4_nt(dst, src0); dst += 4;
        case  1: sse2_store4_nt(dst, src0); dst += 4;
      }

skip:
      dst += dstStride;
    } while (--h);
  }

  static void FOG_FASTCALL cblit_rect_32_prgb(
    uint8_t* dst, sysint_t dstStride,
    const RasterSolid* src,
    int w, int h, const RasterClosure* closure)
  {
    cblit_rect_32_helper(dst, dstStride, src->prgb, w, h);
  }

  static void FOG_FASTCALL cblit_rect_32_argb(
    uint8_t* dst, sysint_t dstStride,
    const RasterSolid* src,
    int w, int h, const RasterClosure* closure)
  {
    cblit_rect_32_helper(dst, dstStride, src->argb, w, h);
  }

  static void FOG_FASTCALL cblit_rect_32_nt_prgb(
    uint8_t* dst, sysint_t dstStride,
    const RasterSolid* src,
    int w, int h, const RasterClosure* closure)
  {
    cblit_rect_32_nt_helper(dst, dstStride, src->prgb, w, h);
  }

  static void FOG_FASTCALL cblit_rect_32_nt_argb(
    uint8_t* dst, sysint_t dstStride,
    const RasterSolid* src,
    int w, int h, const RasterClosure* closure)
  {
    cblit_rect_32_nt_helper(dst, dstStride, src->argb, w, h);
  }

  // --------------------------------------------------------------------------
  // [DibSSE2 - VBlit - Rect]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL vblit_rect_32(
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int w, int h, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0 && h > 0);

    dstStride -= w * 4;
    srcStride -= w * 4;

    do {
      int i = w;

      while ((sysuint_t)dst & 15)
      {
        Memory::copy4B(dst, src);
        dst += 4;
        src += 4;
        if (--i == 0) goto skip;
      }

      if ((sysuint_t)src & 15)
      {
        while (i >= 16)
        {
          __m128i src0mm;
          __m128i src1mm;
          __m128i src2mm;
          __m128i src3mm;

          _mm_prefetch((const char*)(src + 64), _MM_HINT_T0);

          sse2_load16u(src0mm, src + 0);
          sse2_load16u(src1mm, src + 16);
          sse2_load16u(src2mm, src + 32);
          sse2_load16u(src3mm, src + 48);
          sse2_store16a(dst + 0, src0mm);
          sse2_store16a(dst + 16, src1mm);
          sse2_store16a(dst + 32, src2mm);
          sse2_store16a(dst + 48, src3mm);

          dst += 64;
          src += 64;
          i -= 16;
        }
      }
      else
      {
        while (i >= 16)
        {
          __m128i src0mm;
          __m128i src1mm;
          __m128i src2mm;
          __m128i src3mm;

          _mm_prefetch((const char*)(src + 64), _MM_HINT_T0);

          sse2_load16a(src0mm, src + 0);
          sse2_load16a(src1mm, src + 16);
          sse2_load16a(src2mm, src + 32);
          sse2_load16a(src3mm, src + 48);
          sse2_store16a(dst + 0, src0mm);
          sse2_store16a(dst + 16, src1mm);
          sse2_store16a(dst + 32, src2mm);
          sse2_store16a(dst + 48, src3mm);

          dst += 64;
          src += 64;
          i -= 16;
        }
      }

      switch (i)
      {
        case 15: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 14: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 13: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 12: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 11: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 10: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  9: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  8: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  7: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  6: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  5: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  4: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  3: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  2: Memory::copy4B(dst, src); dst += 4; src += 4;
        case  1: Memory::copy4B(dst, src); dst += 4; src += 4;
      }
skip:
      dst += dstStride;
      src += srcStride;
    } while (--h);
  }

  static void FOG_FASTCALL vblit_rect_32_nt(
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int w, int h, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0 && h > 0);

    dstStride -= w * 4;
    srcStride -= w * 4;

    do {
      int i = w;

      while ((sysuint_t)dst & 15)
      {
        sse2_store4_nt(dst, READ_32(src));
        dst += 4;
        src += 4;
        if (--i == 0) goto skip;
      }

      if ((sysuint_t)src & 15)
      {
        while (i >= 16)
        {
          __m128i src0mm;
          __m128i src1mm;
          __m128i src2mm;
          __m128i src3mm;

          _mm_prefetch((const char*)(src + 64), _MM_HINT_T0);

          sse2_load16u(src0mm, src + 0);
          sse2_load16u(src1mm, src + 16);
          sse2_load16u(src2mm, src + 32);
          sse2_load16u(src3mm, src + 48);
          sse2_store16a_nt(dst + 0, src0mm);
          sse2_store16a_nt(dst + 16, src1mm);
          sse2_store16a_nt(dst + 32, src2mm);
          sse2_store16a_nt(dst + 48, src3mm);

          dst += 64;
          src += 64;
          i -= 16;
        }
      }
      else
      {
        while (i >= 16)
        {
          __m128i src0mm;
          __m128i src1mm;
          __m128i src2mm;
          __m128i src3mm;

          _mm_prefetch((const char*)(src + 64), _MM_HINT_T0);

          sse2_load16a(src0mm, src + 0);
          sse2_load16a(src1mm, src + 16);
          sse2_load16a(src2mm, src + 32);
          sse2_load16a(src3mm, src + 48);
          sse2_store16a_nt(dst + 0, src0mm);
          sse2_store16a_nt(dst + 16, src1mm);
          sse2_store16a_nt(dst + 32, src2mm);
          sse2_store16a_nt(dst + 48, src3mm);

          dst += 64;
          src += 64;
          i -= 16;
        }
      }

      switch (i)
      {
        case 15: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case 14: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case 13: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case 12: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case 11: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case 10: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case  9: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case  8: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case  7: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case  6: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case  5: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case  4: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case  3: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case  2: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
        case  1: sse2_store4_nt(dst, READ_32(src)); dst += 4; src += 4;
      }
skip:
      dst += dstStride;
      src += srcStride;
    } while (--h);
  }

  // --------------------------------------------------------------------------
  // [DibSSE2 - Convert]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;

      sse2_load4(src0mm, src);
      sse2_unpack_1x1W(src0mm, src0mm);
      sse2_premultiply_1x1W(src0mm, src0mm);
      sse2_pack_1x1W(src0mm, src0mm);
      sse2_store4(dst, src0mm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i src0mm;
      __m128i src1mm;

      sse2_load16u(src0mm, src);
      sse2_unpack_2x2W(src0mm, src1mm, src0mm);
      sse2_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
      sse2_pack_2x2W(src0mm, src0mm, src1mm);
      sse2_store16a(dst, src0mm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }

  static void FOG_FASTCALL frgb32_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;

      sse2_load4(src0mm, src);
      src0mm = _mm_or_si128(src0mm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store4(dst, src0mm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i src0mm;

      sse2_load16u(src0mm, src);
      src0mm = _mm_or_si128(src0mm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store16a(dst, src0mm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }

  static void FOG_FASTCALL azzz32_from_a8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      ((uint32_t*)dst)[0] = src[0];

      dst += 4;
      src += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i pix0xmm;

      sse2_load4(pix0xmm, src);
      sse2_unpack_1x1D(pix0xmm, pix0xmm);
      pix0xmm = _mm_srli_epi32(pix0xmm, 24);

      sse2_store16a(dst, pix0xmm);

      dst += 16;
      src += 4;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }

  static void FOG_FASTCALL prgb32_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = 
      reinterpret_cast<const uint32_t*>(closure->srcPalette) + Palette::INDEX_PRGB32;

    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;

      sse2_load_1xI8(src0mm, src, srcPal);
      sse2_store4(dst, src0mm);

      dst += 4;
      src += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i src0mm;

      sse2_load_4xI8(src0mm, src, srcPal);
      sse2_store16a(dst, src0mm);

      dst += 16;
      src += 4;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }

  static void FOG_FASTCALL prgb32_from_argb32_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;

      sse2_load4(src0mm, src);
      sse2_unpack_1x1W(src0mm, src0mm);
      sse2_swap_1x1W(src0mm, src0mm);
      sse2_premultiply_1x1W(src0mm, src0mm);
      sse2_pack_1x1W(src0mm, src0mm);
      sse2_store4(dst, src0mm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i src0mm;
      __m128i src1mm;

      sse2_load16u(src0mm, src);
      sse2_unpack_2x2W(src0mm, src1mm, src0mm);
      sse2_swap_2x2W(src0mm, src0mm, src1mm, src1mm);
      sse2_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
      sse2_pack_2x2W(src0mm, src0mm, src1mm);
      sse2_store16a(dst, src0mm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }
  
  static void FOG_FASTCALL argb32_from_prgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;

      sse2_load4(src0mm, src);
      sse2_unpack_1x1W(src0mm, src0mm);
      sse2_demultiply_1x1W_srcbuf(src0mm, src0mm, src);
      sse2_pack_1x1W(src0mm, src0mm);
      sse2_store4(dst, src0mm);

      src += 4;
      dst += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i src0mm, src1mm;

      sse2_load16u(src0mm, src);
      sse2_unpack_2x2W(src0mm, src1mm, src0mm);
      sse2_demultiply_2x2W_srcbuf(src0mm, src0mm, src1mm, src1mm, src);
      sse2_pack_2x2W(src0mm, src0mm, src1mm);
      sse2_store16a(dst, src0mm);

      src += 16;
      dst += 16;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }

  static void FOG_FASTCALL argb32_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = 
      reinterpret_cast<const uint32_t*>(closure->srcPalette) + Palette::INDEX_PRGB32;

    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;

      sse2_load_1xI8(src0mm, src, srcPal);
      sse2_store4(dst, src0mm);

      dst += 4;
      src += 1;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i src0mm;

      sse2_load_4xI8(src0mm, src, srcPal);
      sse2_store16a(dst, src0mm);

      dst += 16;
      src += 4;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }










  static void FOG_FASTCALL frgb32_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;

      sse2_load4(src0mm, src);
      sse2_unpack_1x1W(src0mm, src0mm);
      sse2_premultiply_1x1W(src0mm, src0mm);
      sse2_pack_1x1W(src0mm, src0mm);
      src0mm = _mm_or_si128(src0mm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store4(dst, src0mm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i src0mm;
      __m128i src1mm;

      sse2_load16u(src0mm, src);
      sse2_unpack_2x2W(src0mm, src1mm, src0mm);
      sse2_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
      sse2_pack_2x2W(src0mm, src0mm, src1mm);
      src0mm = _mm_or_si128(src0mm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store16a(dst, src0mm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }

  static void FOG_FASTCALL frgb32_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = 
      reinterpret_cast<const uint32_t*>(closure->srcPalette) + Palette::INDEX_PRGB32;

    do {
      ((uint32_t*)dst)[0] = srcPal[src[0]] | 0xFF000000;

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_argb32_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;

      sse2_load4(src0mm, src);
      sse2_unpack_1x1W(src0mm, src0mm);
      sse2_swap_1x1W(src0mm, src0mm);
      sse2_premultiply_1x1W(src0mm, src0mm);
      sse2_pack_1x1W(src0mm, src0mm);
      src0mm = _mm_or_si128(src0mm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store4(dst, src0mm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i src0mm;
      __m128i src1mm;

      sse2_load16u(src0mm, src);
      sse2_unpack_2x2W(src0mm, src1mm, src0mm);
      sse2_swap_2x2W(src0mm, src0mm, src1mm, src1mm);
      sse2_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
      sse2_pack_2x2W(src0mm, src0mm, src1mm);
      src0mm = _mm_or_si128(src0mm, FOG_GET_SSE_CONST_PI(FF000000FF000000_FF000000FF000000));
      sse2_store16a(dst, src0mm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }

  static void FOG_FASTCALL frgb32_from_rgb24_native(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    while ((sysuint_t)src & 3)
    {
      ((uint32_t*)dst)[0] = RGB24_NATIVE_IO::fetch(src) | 0xFF000000;
      dst += 4;
      src += 3;
      if (--w == 0) return;
    }

    while (w >= 4)
    {
      __m128i src0mm;
      __m128i src1mm;

      sse2_fetch_rgb24_2x2W(src0mm, src1mm, src);
      sse2_pack_2x2W(src0mm, src0mm, src1mm);
      sse2_store16u(dst, src0mm);
      dst += 16;
      src += 12;
      w -= 4;
    }

    while (w)
    {
      ((uint32_t*)dst)[0] = RGB24_NATIVE_IO::fetch(src) | 0xFF000000;
      dst += 4;
      src += 3;
      w--;
    }
  }

  static void FOG_FASTCALL frgb32_from_rgb24_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    while ((sysuint_t)src & 3)
    {
      ((uint32_t*)dst)[0] = RGB24_SWAPPED_IO::fetch(src) | 0xFF000000;
      dst += 4;
      src += 3;
      if (--w == 0) return;
    }

    while (w >= 4)
    {
      __m128i src0mm;
      __m128i src1mm;

      sse2_fetch_bgr24_2x2W(src0mm, src1mm, src);
      sse2_pack_2x2W(src0mm, src0mm, src1mm);
      sse2_store16u(dst, src0mm);
      dst += 16;
      src += 12;
      w -= 4;
    }

    while (w)
    {
      ((uint32_t*)dst)[0] = RGB24_SWAPPED_IO::fetch(src) | 0xFF000000;
      dst += 4;
      src += 3;
      w--;
    }
  }

  static void FOG_FASTCALL prgb32_swapped_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_32xX_INIT();

    SSE2_BLIT_32x4_SMALL_BEGIN(blt)
      __m128i src0mm;

      sse2_load4(src0mm, src);
      sse2_unpack_1x1W(src0mm, src0mm);
      sse2_premultiply_1x1W(src0mm, src0mm);
      sse2_swap_1x1W(src0mm, src0mm);
      sse2_pack_1x1W(src0mm, src0mm);
      sse2_store4(dst, src0mm);

      dst += 4;
      src += 4;
    SSE2_BLIT_32x4_SMALL_END(blt)

    SSE2_BLIT_32x4_MAIN_BEGIN(blt)
      __m128i src0mm;
      __m128i src1mm;

      sse2_load16u(src0mm, src);
      sse2_unpack_2x2W(src0mm, src1mm, src0mm);
      sse2_premultiply_2x2W(src0mm, src0mm, src1mm, src1mm);
      sse2_swap_2x2W(src0mm, src0mm, src1mm, src1mm);
      sse2_pack_2x2W(src0mm, src0mm, src1mm);
      sse2_store16a(dst, src0mm);

      dst += 16;
      src += 16;
    SSE2_BLIT_32x4_MAIN_END(blt)
  }

  static void FOG_FASTCALL a8_from_axxx32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    SSE2_BLIT_8x16_INIT();

    SSE2_BLIT_8x16_SMALL_BEGIN(blt)
      dst[0] = src[ARGB32_ABYTE];

      dst += 1;
      src += 4;
    SSE2_BLIT_8x16_SMALL_END(blt)

    SSE2_BLIT_8x16_MAIN_BEGIN(blt)
      __m128i src0mm;
      __m128i src1mm;

      sse2_load_and_unpack_axxx32_64B(src0mm, src1mm, src);
      sse2_pack_2x2W(src0mm, src0mm, src1mm);
      sse2_store16a(dst, src0mm);

      dst += 16;
      src += 64;
    SSE2_BLIT_8x16_MAIN_END(blt)
  }

  static void FOG_FASTCALL a8_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = 
      reinterpret_cast<const uint32_t*>(closure->srcPalette) + Palette::INDEX_PRGB32;

    do {
      dst[0] = (uint8_t)(srcPal[src[0]] >> 24);

      dst += 1;
      src += 1;
    } while (--w);
  }

  // XXXXXXXX RRRRRRRR GGGGGGGG BBBBBBBB ->
  //                   RRRRRGGG GGGBBBBB
  static void FOG_FASTCALL rgb16_565_native_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    int i;

    // Align destination to 16 bytes.
    while ((sysuint_t)dst & 15)
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
      __m128i pix0xmmR, pix1xmmR;
      __m128i pix0xmmG, pix1xmmG;
      __m128i pix0xmmB, pix1xmmB;

      sse2_load16u(pix0xmmB, src);
      sse2_load16u(pix1xmmB, src + 16);

      pix0xmmR = _mm_srli_epi32(pix0xmmB, 9);
      pix1xmmR = _mm_srli_epi32(pix1xmmB, 9);

      pix0xmmG = _mm_srli_epi32(pix0xmmB, 5);
      pix1xmmG = _mm_srli_epi32(pix1xmmB, 5);

      pix0xmmB = _mm_srli_epi32(pix0xmmB, 3);
      pix1xmmB = _mm_srli_epi32(pix1xmmB, 3);

      pix0xmmR = _mm_and_si128(pix0xmmR, FOG_GET_SSE_CONST_PI(00007C0000007C00_00007C0000007C00));
      pix1xmmR = _mm_and_si128(pix1xmmR, FOG_GET_SSE_CONST_PI(00007C0000007C00_00007C0000007C00));

      pix0xmmG = _mm_and_si128(pix0xmmG, FOG_GET_SSE_CONST_PI(000007E0000007E0_000007E0000007E0));
      pix1xmmG = _mm_and_si128(pix1xmmG, FOG_GET_SSE_CONST_PI(000007E0000007E0_000007E0000007E0));

      pix0xmmB = _mm_and_si128(pix0xmmB, FOG_GET_SSE_CONST_PI(0000001F0000001F_0000001F0000001F));
      pix1xmmB = _mm_and_si128(pix1xmmB, FOG_GET_SSE_CONST_PI(0000001F0000001F_0000001F0000001F));

      pix0xmmB = _mm_or_si128(pix0xmmB, pix0xmmG);
      pix1xmmB = _mm_or_si128(pix1xmmB, pix1xmmG);

      pix0xmmR = _mm_packs_epi32(pix0xmmR, pix0xmmR);
      pix0xmmB = _mm_packs_epi32(pix0xmmB, pix1xmmB);

      pix0xmmR = _mm_slli_epi16(pix0xmmR, 1);
      pix0xmmB = _mm_or_si128(pix0xmmB, pix0xmmR);

      sse2_store16a(dst, pix0xmmB);
    }

    for (i = w & 7; i; i--, dst += 2, src += 4)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 8) & 0xF800) |
        ((pix0 >> 5) & 0x07E0) |
        ((pix0 >> 3) & 0x001F));
    }
  }

  // XXXXXXXX RRRRRRRR GGGGGGGG BBBBBBBB ->
  //                   GGGBBBBB RRRRRGGG
  static void FOG_FASTCALL rgb16_565_swapped_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    int i;

    // Align destination to 16 bytes.
    while ((sysuint_t)dst & 15)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 16) & 0x00F8) |
        ((pix0 >> 13) & 0x0007) |
        ((pix0 <<  3) & 0xE000) |
        ((pix0 <<  5) & 0x1F00));
      if (--w == 0) return;

      dst += 2;
      src += 4;
    }

    for (i = w >> 3; i; i--, dst += 16, src += 32)
    {
      __m128i pix0xmmR0, pix1xmmR0;
      __m128i pix0xmmG0, pix1xmmG0;
      __m128i pix0xmmG1, pix1xmmG1;
      __m128i pix0xmmB0, pix1xmmB0;

      sse2_load16u(pix0xmmB0, src);
      sse2_load16u(pix1xmmB0, src + 16);

      pix0xmmR0 = _mm_srli_epi32(pix0xmmB0, 16);
      pix1xmmR0 = _mm_srli_epi32(pix1xmmB0, 16);

      pix0xmmG0 = _mm_srli_epi32(pix0xmmB0, 13);
      pix1xmmG0 = _mm_srli_epi32(pix1xmmB0, 13);

      pix0xmmG1 = pix0xmmB0;
      pix1xmmG1 = pix1xmmB0;

      pix0xmmB0 = _mm_slli_epi32(pix0xmmB0, 5);
      pix1xmmB0 = _mm_slli_epi32(pix1xmmB0, 5);

      pix0xmmR0 = _mm_and_si128(pix0xmmR0, FOG_GET_SSE_CONST_PI(000000F8000000F8_000000F8000000F8));
      pix1xmmR0 = _mm_and_si128(pix1xmmR0, FOG_GET_SSE_CONST_PI(000000F8000000F8_000000F8000000F8));

      pix0xmmG0 = _mm_and_si128(pix0xmmG0, FOG_GET_SSE_CONST_PI(0000000700000007_0000000700000007));
      pix1xmmG0 = _mm_and_si128(pix1xmmG0, FOG_GET_SSE_CONST_PI(0000000700000007_0000000700000007));

      pix0xmmR0 = _mm_packs_epi32(pix0xmmR0, pix1xmmR0);

      pix0xmmB0 = _mm_and_si128(pix0xmmB0, FOG_GET_SSE_CONST_PI(00001F0000001F00_00001F0000001F00));
      pix1xmmB0 = _mm_and_si128(pix1xmmB0, FOG_GET_SSE_CONST_PI(00001F0000001F00_00001F0000001F00));

      pix0xmmG1 = _mm_and_si128(pix0xmmG1, FOG_GET_SSE_CONST_PI(00001C0000001C00_00001C0000001C00));
      pix1xmmG1 = _mm_and_si128(pix1xmmG1, FOG_GET_SSE_CONST_PI(00001C0000001C00_00001C0000001C00));

      pix0xmmB0 = _mm_packs_epi32(pix0xmmB0, pix1xmmB0);
      pix0xmmG0 = _mm_packs_epi32(pix0xmmG0, pix1xmmG0);
      pix0xmmB0 = _mm_or_si128(pix0xmmB0, pix0xmmR0);
      pix0xmmG1 = _mm_packs_epi32(pix0xmmG1, pix1xmmG1);
      pix0xmmB0 = _mm_or_si128(pix0xmmB0, pix0xmmG0);
      pix0xmmG1 = _mm_slli_epi16(pix0xmmG1, 3);
      pix0xmmB0 = _mm_or_si128(pix0xmmB0, pix0xmmG1);

      sse2_store16a(dst, pix0xmmB0);
    }

    for (i = w & 7; i; i--, dst += 2, src += 4)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 16) & 0x00F8) |
        ((pix0 >> 13) & 0x0007) |
        ((pix0 <<  3) & 0xE000) |
        ((pix0 <<  5) & 0x1F00));
    }
  }

  // XXXXXXXX RRRRRRRR GGGGGGGG BBBBBBBB ->
  //                   XRRRRRGG GGGBBBBB
  static void FOG_FASTCALL rgb16_555_native_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    int i;

    // Align destination to 16 bytes.
    while ((sysuint_t)dst & 15)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 3) & 0x001F) |
        ((pix0 >> 6) & 0x03E0) |
        ((pix0 >> 9) & 0x7C00));
      if (--w == 0) return;

      dst += 2;
      src += 4;
    }

    for (i = w >> 3; i; i--, dst += 16, src += 32)
    {
      __m128i pix0xmmR, pix1xmmR;
      __m128i pix0xmmG, pix1xmmG;
      __m128i pix0xmmB, pix1xmmB;

      sse2_load16u(pix0xmmB, src);
      sse2_load16u(pix1xmmB, src + 16);

      pix0xmmR = _mm_srli_epi32(pix0xmmB, 9);
      pix1xmmR = _mm_srli_epi32(pix1xmmB, 9);

      pix0xmmG = _mm_srli_epi32(pix0xmmB, 6);
      pix1xmmG = _mm_srli_epi32(pix1xmmB, 6);

      pix0xmmB = _mm_srli_epi32(pix0xmmB, 3);
      pix1xmmB = _mm_srli_epi32(pix1xmmB, 3);

      pix0xmmR = _mm_and_si128(pix0xmmR, FOG_GET_SSE_CONST_PI(00007C0000007C00_00007C0000007C00));
      pix1xmmR = _mm_and_si128(pix1xmmR, FOG_GET_SSE_CONST_PI(00007C0000007C00_00007C0000007C00));

      pix0xmmG = _mm_and_si128(pix0xmmG, FOG_GET_SSE_CONST_PI(000003E0000003E0_000003E0000003E0));
      pix1xmmG = _mm_and_si128(pix1xmmG, FOG_GET_SSE_CONST_PI(000003E0000003E0_000003E0000003E0));

      pix0xmmB = _mm_and_si128(pix0xmmB, FOG_GET_SSE_CONST_PI(0000001F0000001F_0000001F0000001F));
      pix1xmmB = _mm_and_si128(pix1xmmB, FOG_GET_SSE_CONST_PI(0000001F0000001F_0000001F0000001F));

      pix0xmmB = _mm_or_si128(pix0xmmB, pix0xmmR);
      pix1xmmB = _mm_or_si128(pix1xmmB, pix1xmmR);

      pix0xmmB = _mm_or_si128(pix0xmmB, pix0xmmG);
      pix1xmmB = _mm_or_si128(pix1xmmB, pix1xmmG);

      pix0xmmB = _mm_packs_epi32(pix0xmmB, pix1xmmB);
      sse2_store16a(dst, pix0xmmB);
    }

    for (i = w & 7; i; i--, dst += 2, src += 4)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 3) & 0x001F) |
        ((pix0 >> 6) & 0x03E0) |
        ((pix0 >> 9) & 0x7C00));
    }
  }

  // XXXXXXXX RRRRRRRR GGGGGGGG BBBBBBBB ->
  //                   GGGBBBBB XRRRRRGG
  static void FOG_FASTCALL rgb16_555_swapped_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    int i;

    // Align destination to 16 bytes.
    while ((sysuint_t)dst & 15)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 17) & 0x007C) |
        ((pix0 >> 14) & 0x0003) |
        ((pix0 <<  2) & 0xE000) |
        ((pix0 <<  5) & 0x1F00));
      if (--w == 0) return;

      dst += 2;
      src += 4;
    }

    for (i = w >> 3; i; i--, dst += 16, src += 32)
    {
      __m128i pix0xmmR0, pix1xmmR0;
      __m128i pix0xmmG0, pix1xmmG0;
      __m128i pix0xmmG1, pix1xmmG1;
      __m128i pix0xmmB0, pix1xmmB0;

      sse2_load16u(pix0xmmB0, src);
      sse2_load16u(pix1xmmB0, src + 16);

      pix0xmmR0 = _mm_srli_epi32(pix0xmmB0, 17);
      pix1xmmR0 = _mm_srli_epi32(pix1xmmB0, 17);

      pix0xmmG0 = _mm_srli_epi32(pix0xmmB0, 14);
      pix1xmmG0 = _mm_srli_epi32(pix1xmmB0, 14);

      pix0xmmG1 = pix0xmmB0;
      pix1xmmG1 = pix1xmmB0;

      pix0xmmB0 = _mm_slli_epi32(pix0xmmB0, 5);
      pix1xmmB0 = _mm_slli_epi32(pix1xmmB0, 5);

      pix0xmmR0 = _mm_and_si128(pix0xmmR0, FOG_GET_SSE_CONST_PI(000000F8000000F8_000000F8000000F8));
      pix1xmmR0 = _mm_and_si128(pix1xmmR0, FOG_GET_SSE_CONST_PI(000000F8000000F8_000000F8000000F8));

      pix0xmmG0 = _mm_and_si128(pix0xmmG0, FOG_GET_SSE_CONST_PI(0000000300000003_0000000300000003));
      pix1xmmG0 = _mm_and_si128(pix1xmmG0, FOG_GET_SSE_CONST_PI(0000000300000003_0000000300000003));

      pix0xmmR0 = _mm_packs_epi32(pix0xmmR0, pix1xmmR0);

      pix0xmmB0 = _mm_and_si128(pix0xmmB0, FOG_GET_SSE_CONST_PI(00001F0000001F00_00001F0000001F00));
      pix1xmmB0 = _mm_and_si128(pix1xmmB0, FOG_GET_SSE_CONST_PI(00001F0000001F00_00001F0000001F00));

      pix0xmmG1 = _mm_and_si128(pix0xmmG1, FOG_GET_SSE_CONST_PI(0000380000003800_0000380000003800));
      pix1xmmG1 = _mm_and_si128(pix1xmmG1, FOG_GET_SSE_CONST_PI(0000380000003800_0000380000003800));

      pix0xmmB0 = _mm_packs_epi32(pix0xmmB0, pix1xmmB0);
      pix0xmmG0 = _mm_packs_epi32(pix0xmmG0, pix1xmmG0);
      pix0xmmB0 = _mm_or_si128(pix0xmmB0, pix0xmmR0);
      pix0xmmG1 = _mm_packs_epi32(pix0xmmG1, pix1xmmG1);
      pix0xmmB0 = _mm_or_si128(pix0xmmB0, pix0xmmG0);
      pix0xmmG1 = _mm_slli_epi16(pix0xmmG1, 2);
      pix0xmmB0 = _mm_or_si128(pix0xmmB0, pix0xmmG1);

      sse2_store16a(dst, pix0xmmB0);
    }

    for (i = w & 7; i; i--, dst += 2, src += 4)
    {
      uint32_t pix0 = READ_32(src);
      ((uint16_t*)dst)[0] = (uint16_t)(
        ((pix0 >> 17) & 0x007C) |
        ((pix0 >> 14) & 0x0003) |
        ((pix0 <<  2) & 0xE000) |
        ((pix0 <<  5) & 0x1F00));
    }
  }
};

} // RasterEngine namespace
} // Fog namespace
