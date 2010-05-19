// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Dib - Constants]
// ============================================================================

enum RGB24_NATIVE_BYTEPOS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  RGB24_NATIVE_RBYTE = 2,
  RGB24_NATIVE_GBYTE = 1,
  RGB24_NATIVE_BBYTE = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  RGB24_NATIVE_RBYTE = 0,
  RGB24_NATIVE_GBYTE = 1,
  RGB24_NATIVE_BBYTE = 2
#endif // FOG_BYTE_ORDER
};

enum RGB24_SWAPPED_BYTEPOS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  RGB24_SWAPPED_RBYTE = 0,
  RGB24_SWAPPED_GBYTE = 1,
  RGB24_SWAPPED_BBYTE = 2
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  RGB24_SWAPPED_RBYTE = 2,
  RGB24_SWAPPED_GBYTE = 1,
  RGB24_SWAPPED_BBYTE = 0
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::RasterEngine::C - Dib - IO]
// ============================================================================

//! @internal
struct FOG_HIDDEN RGB24_NATIVE_IO
{
  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    return ( (uint32_t)(((const uint16_t*)(p))[0]) | ((uint32_t )p[2] << 16) );
#else
    return ((uint32_t)p[RGB24_NATIVE_RBYTE] << ARGB32_RSHIFT) |
           ((uint32_t)p[RGB24_NATIVE_GBYTE] << ARGB32_GSHIFT) |
           ((uint32_t)p[RGB24_NATIVE_BBYTE] << ARGB32_BSHIFT) ;
#endif
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t pix0)
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    ((uint16_t*)(p))[0] = (uint16_t)pix0;
    p[2] = (uint8_t)(pix0 >> 16);
#else
    p[RGB24_NATIVE_RBYTE] = (uint8_t)(pix0 >> ARGB32_RSHIFT);
    p[RGB24_NATIVE_GBYTE] = (uint8_t)(pix0 >> ARGB32_GSHIFT);
    p[RGB24_NATIVE_BBYTE] = (uint8_t)(pix0 >> ARGB32_BSHIFT);
#endif
  }
};

//! @internal
struct FOG_HIDDEN RGB24_SWAPPED_IO
{
  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
    return ((uint32_t)p[RGB24_SWAPPED_RBYTE] << ARGB32_RSHIFT) |
           ((uint32_t)p[RGB24_SWAPPED_GBYTE] << ARGB32_GSHIFT) |
           ((uint32_t)p[RGB24_SWAPPED_BBYTE] << ARGB32_BSHIFT) ;
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t pix0)
  {
    p[RGB24_SWAPPED_RBYTE] = (uint8_t)(pix0 >> ARGB32_RSHIFT);
    p[RGB24_SWAPPED_GBYTE] = (uint8_t)(pix0 >> ARGB32_GSHIFT);
    p[RGB24_SWAPPED_BBYTE] = (uint8_t)(pix0 >> ARGB32_BSHIFT);
  }
};

//! @internal
struct FOG_HIDDEN RGB16_565_NATIVE_IO
{
  // TODO: Not correct
  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
    uint32_t x = ((const uint16_t*)p)[0];
    uint32_t r = ((x & 0xF800) | ((x & 0xE000) >> 5)) << 8;
    uint32_t g = ((x & 0x07E0) | ((x & 0x0600) >> 6)) << 5;
    uint32_t b = ((x & 0x001C) | ((x & 0x001F) << 5)) >> 2;
    return r | g | b;
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t pix0)
  {
    ((uint16_t*)p)[0] = (uint16_t)(
      ((pix0 >> 8) & 0xF800) |
      ((pix0 >> 5) & 0x07E0) |
      ((pix0 >> 3) & 0x001F));
  }
};

//! @internal
struct FOG_HIDDEN RGB16_565_SWAPPED_IO
{
  // TODO: Not correct
  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
    uint32_t x = Memory::bswap16(((const uint16_t*)p)[0]);
    uint32_t r = ((x & 0xF800) | ((x & 0xE000) >> 5)) << 8;
    uint32_t g = ((x & 0x07E0) | ((x & 0x0600) >> 6)) << 5;
    uint32_t b = ((x & 0x001C) | ((x & 0x001F) << 5)) >> 2;
    return r | g | b;
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t pix0)
  {
    ((uint16_t*)p)[0] = (uint16_t)(
      ((pix0 >> 16) & 0x00F8) |
      ((pix0 >> 13) & 0x0007) |
      ((pix0 <<  3) & 0xE000) |
      ((pix0 <<  5) & 0x1F00));
  }
};

//! @internal
struct FOG_HIDDEN RGB16_555_NATIVE_IO
{
  // TODO: Not correct
  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
    uint32_t x = ((const uint16_t*)p)[0];
    uint32_t r = ((x & 0x7C00) | ((x & 0x7000) >> 5)) << 9;
    uint32_t g = ((x & 0x03E0) | ((x & 0x0380) >> 5)) << 6;
    uint32_t b = ((x & 0x001C) | ((x & 0x001F) << 5)) >> 2;
    return r | g | b;
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t pix0)
  {
    ((uint16_t*)p)[0] = (uint16_t)(
      ((pix0 >> 3) & 0x001F) |
      ((pix0 >> 6) & 0x03E0) |
      ((pix0 >> 9) & 0x7C00));
  }
};

//! @internal
struct FOG_HIDDEN RGB16_555_SWAPPED_IO
{
  // TODO: Not correct
  static FOG_INLINE uint32_t fetch(const uint8_t* p)
  {
    uint32_t x = Memory::bswap16(((const uint16_t*)p)[0]);
    uint32_t r = ((x & 0x7C00) | ((x & 0x7000) >> 5)) << 9;
    uint32_t g = ((x & 0x03E0) | ((x & 0x0380) >> 5)) << 6;
    uint32_t b = ((x & 0x001C) | ((x & 0x001F) << 5)) >> 2;
    return r | g | b;
  }

  static FOG_INLINE void store(uint8_t* p, uint32_t pix0)
  {
    ((uint16_t*)p)[0] = (uint16_t)(
      ((pix0 >> 17) & 0x007C) |
      ((pix0 >> 14) & 0x0003) |
      ((pix0 <<  2) & 0xE000) |
      ((pix0 <<  5) & 0x1F00));
  }
};

// ============================================================================
// [Fog::RasterEngine::C - Dib]
// ============================================================================

//! @internal
struct FOG_HIDDEN DibC
{
  // --------------------------------------------------------------------------
  // [DibC - BSwap]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL bswap16(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint16_t*)dst)[0] = Memory::bswap16(((const uint16_t*)src)[0]);

      dst += 2;
      src += 2;
    } while (--w);
  }

  static void FOG_FASTCALL bswap24(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint8_t s0 = src[0];
      uint8_t s1 = src[1];
      uint8_t s2 = src[2];

      dst[0] = s2;
      dst[1] = s1;
      dst[2] = s0;

      dst += 3;
      src += 3;
    } while (--w);
  }

  static void FOG_FASTCALL bswap32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = Memory::bswap32(((const uint32_t*)src)[0]);

      dst += 4;
      src += 4;
    } while (--w);
  }

  // --------------------------------------------------------------------------
  // [DibC - MemCpy]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL memcpy8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    while ((sysuint_t(dst) & 3))
    {
      Memory::copy1B(dst, src);
      dst++;
      src++;
      if (--w == 0) return;
    }

    while (w >= 32)
    {
      Memory::copy32B(dst, src);
      dst += 32;
      src += 32;
      w -= 32;
    }

    while (w >= 4)
    {
      Memory::copy4B(dst, src);
      dst += 4;
      src += 4;
      w -= 4;
    }

    switch (w)
    {
      case 3: *dst++ = *src++;
      case 2: *dst++ = *src++;
      case 1: *dst++ = *src++;
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
    while (w >= 8)
    {
      Memory::copy32B(dst, src);
      dst += 32;
      src += 32;
      w -= 8;
    }

    switch (w)
    {
      case 7: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 6: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 5: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 4: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 3: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 2: Memory::copy4B(dst, src); dst += 4; src += 4;
      case 1: Memory::copy4B(dst, src); dst += 4; src += 4;
    }
  }

  // --------------------------------------------------------------------------
  // [DibC - Rect]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL cblit_rect_32_helper(
    uint8_t* dst, sysint_t dstStride,
    uint32_t src0,
    int w, int h)
  {
    FOG_ASSERT(w > 0 && h > 0);
    dstStride -= w * 4;

#if FOG_ARCH_BITS == 64
    uint64_t src0_64 = (uint64_t)src0 | ((uint64_t)src0 << 32);

    do {
      int i = w;

      while ((i -= 8) >= 0)
      {
        ((uint64_t*)dst)[0] = src0_64;
        ((uint64_t*)dst)[1] = src0_64;
        ((uint64_t*)dst)[2] = src0_64;
        ((uint64_t*)dst)[3] = src0_64;

        dst += 32;
      }
      i += 8;

      switch (i)
      {
        case 7: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 6: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 5: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 4: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 3: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 2: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 1: ((uint32_t*)dst)[0] = src0; dst += 4;
      }

      dst += dstStride;
    } while (--h);
#else
    do {
      int i = w;

      while ((i -= 8) >= 0)
      {
        ((uint32_t*)dst)[0] = src0;
        ((uint32_t*)dst)[1] = src0;
        ((uint32_t*)dst)[2] = src0;
        ((uint32_t*)dst)[3] = src0;
        ((uint32_t*)dst)[4] = src0;
        ((uint32_t*)dst)[5] = src0;
        ((uint32_t*)dst)[6] = src0;
        ((uint32_t*)dst)[7] = src0;

        dst += 32;
      }
      i += 8;

      switch (i)
      {
        case 7: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 6: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 5: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 4: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 3: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 2: ((uint32_t*)dst)[0] = src0; dst += 4;
        case 1: ((uint32_t*)dst)[0] = src0; dst += 4;
      }

      dst += dstStride;
    } while (--h);
#endif
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

  static void FOG_FASTCALL cblit_rect_8(
    uint8_t* dst, sysint_t dstStride,
    const RasterSolid* src,
    int w, int h, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0 && h > 0);
    dstStride -= w;

#if FOG_ARCH_BITS == 64
    uint64_t src0 = (uint64_t)((src->prgb) >> 24) * FOG_UINT64_C(0x0101010101010101);
#else
    uint32_t src0 = (uint32_t)((src->prgb) >> 24) * 0x01010101;
#endif

    do {
      int i = w;

      while (((sysuint_t)dst & 0x3) && i)
      {
        dst[0] = (uint8_t)src0;
        dst++;
        i--;
      }

      while ((i -= 32) >= 0)
      {
#if FOG_ARCH_BITS == 64
        ((uint64_t*)dst)[0] = src0;
        ((uint64_t*)dst)[1] = src0;
        ((uint64_t*)dst)[2] = src0;
        ((uint64_t*)dst)[3] = src0;
#else
        ((uint32_t*)dst)[0] = src0;
        ((uint32_t*)dst)[1] = src0;
        ((uint32_t*)dst)[2] = src0;
        ((uint32_t*)dst)[3] = src0;
        ((uint32_t*)dst)[4] = src0;
        ((uint32_t*)dst)[5] = src0;
        ((uint32_t*)dst)[6] = src0;
        ((uint32_t*)dst)[7] = src0;
#endif
        dst += 32;
      }
      i += 32;

      while ((i -= 4) >= 0)
      {
        ((uint32_t*)dst)[0] = (uint32_t)src0;
        dst += 4;
      }
      i += 4;

      switch (i)
      {
        case 3: ((uint8_t*)dst)[0] = (uint8_t)src0; dst += 1;
        case 2: ((uint8_t*)dst)[0] = (uint8_t)src0; dst += 1;
        case 1: ((uint8_t*)dst)[0] = (uint8_t)src0; dst += 1;
      }

      dst += dstStride;
    } while (--h);
  }

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

      while ((i -= 8) >= 0)
      {
        Memory::copy64B(dst, src);

        dst += 32;
        src += 32;
      }
      i += 8;

      switch (i)
      {
        case 7: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 6: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 5: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 4: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 3: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 2: Memory::copy4B(dst, src); dst += 4; src += 4;
        case 1: Memory::copy4B(dst, src); dst += 4; src += 4;
      }

      dst += dstStride;
      src += srcStride;
    } while (--h);
  }

  static void FOG_FASTCALL vblit_rect_8(
    uint8_t* dst, sysint_t dstStride,
    const uint8_t* src, sysint_t srcStride,
    int w, int h, const RasterClosure* closure)
  {
    FOG_ASSERT(w > 0 && h > 0);

    do {
      memcpy(dst, src, w);
      dst += dstStride;
      src += srcStride;
    } while (--h);
  }

  // --------------------------------------------------------------------------
  // [DibC - Convert]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL prgb32_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = ColorUtil::premultiply(((const uint32_t*)src)[0]);

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = ((const uint32_t*)src)[0] | 0xFF000000U;

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL azzz32_from_a8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = (uint32_t)src[0] << 24;

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      ((uint32_t*)dst)[0] = srcPal[src[0]];

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_from_argb32_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = ColorUtil::premultiply(Memory::bswap32(((const uint32_t*)src)[0]));

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_xrgb32_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = Memory::bswap32(((const uint32_t*)src)[0]) | 0xFF000000U;

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_rgb24_native(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    while (w >= 4)
    {
      // Following table illustrates how this works on Little Endian:
      //
      // [B G R B] [G R B G] [R B G R]
      // [B G R X] [B G R X] [B G R X] [B G R X]

      uint32_t s0 = ((const uint32_t*)src)[0];
      uint32_t s1 = ((const uint32_t*)src)[1];
      uint32_t s2 = ((const uint32_t*)src)[2];

      ((uint32_t*)dst)[0] = (s0      )              | 0xFF000000U;
      ((uint32_t*)dst)[1] = (s0 >> 24) | (s1 <<  8) | 0xFF000000U;
      ((uint32_t*)dst)[2] = (s1 >> 16) | (s2 << 16) | 0xFF000000U;
      ((uint32_t*)dst)[3] =              (s2 >>  8) | 0xFF000000U;

      dst += 16;
      src += 12;
      w -= 4;
    }
#endif // FOG_BYTE_ORDER

    while (w)
    {
      ((uint32_t*)dst)[0] = RGB24_NATIVE_IO::fetch(src) | 0xFF000000U;
      dst += 4;
      src += 3;
      w--;
    }
  }

  static void FOG_FASTCALL frgb32_from_rgb24_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = RGB24_SWAPPED_IO::fetch(src) | 0xFF000000U;

      dst += 4;
      src += 3;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_rgb16_565_native(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = RGB16_565_NATIVE_IO::fetch(src) | 0xFF000000U;

      dst += 4;
      src += 2;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_rgb16_565_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = RGB16_565_SWAPPED_IO::fetch(src) | 0xFF000000U;

      dst += 4;
      src += 2;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_rgb16_555_native(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = RGB16_555_NATIVE_IO::fetch(src) | 0xFF000000U;

      dst += 4;
      src += 2;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_rgb16_555_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = RGB16_555_SWAPPED_IO::fetch(src) | 0xFF000000U;

      dst += 4;
      src += 2;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_grey8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t grey = src[0];

      ((uint32_t*)dst)[0] = grey | (grey << 8) | (grey << 16) | 0xFF000000U;

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL argb32_from_prgb32(
    uint8_t* _dst, const uint8_t* _src, int w, const RasterClosure* closure)
  {
    uint32_t* dst = reinterpret_cast<uint32_t*>(_dst);
    const uint32_t* src = reinterpret_cast<const uint32_t*>(_src);

    int i = 0;

    while (i < w)
    {
      // We want to identify long runs of constant input pixels and
      // cache the unpremultiplied.
      uint32_t const_in, const_out;

      // Diff is the or of all bitwise differences from const_in
      // during the probe period.  If it is zero after the probe
      // period then every input pixel was identical in the
      // probe.
      unsigned diff = 0;

      // Accumulator for all alphas of the probe period pixels,
      // biased to make the sum zero if the
      unsigned accu = -2*255;

      {
        uint32_t rgba, a, r, g, b, recip;

        rgba = const_in = src[i];
        a = (rgba >> 24);
        accu += a;
        r = (rgba >> 16) & 0xFFU;
        g = (rgba >> 8 ) & 0xFFU;
        b = (rgba      ) & 0xFFU;
        recip = raster_demultiply_reciprocal_table_d[a];

        r = ((r * recip)      ) & 0x00FF0000;
        g = ((g * recip) >>  8) & 0x0000FF00;
        b = ((b * recip) >> 16) & 0x000000FFU;

        dst[i] = const_out = r | g | b | (a << 24);
      }

      if (++i == w) return;

      {
        uint32_t rgba, a, r, g, b, recip;
        rgba = src[i];
        a = (rgba >> 24);
        accu += a;
        r = (rgba >> 16) & 0xFFU;
        g = (rgba >>  8) & 0xFFU;
        b = (rgba      ) & 0xFFU;
        recip = raster_demultiply_reciprocal_table_d[a];

        diff = rgba ^ const_in;

        r = ((r * recip)      ) & 0x00FF0000;
        g = ((g * recip) >>  8) & 0x0000FF00;
        b = ((b * recip) >> 16) & 0x000000FFU;

        dst[i] = r | g | b | (a << 24);
      }

      if (++i == w) return;

      // Fall into special cases if we have special circumstances.
      if (0 != (accu & diff)) continue;

      if (0 == accu)
      {
        // a run of solid pixels.
        uint32_t in;
        while (0xFF000000U == ((in = src[i]) & 0xFF000000U))
        {
          dst[i] = in;
          if (++i == w) return;
        }
      }
      else if (0 == diff)
      {
        // a run of constant pixels.
        while (src[i] == const_in)
        {
          dst[i] = const_out;
          if (++i == w) return;
        }
      }
    }
  }

  static void FOG_FASTCALL argb32_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_ARGB32;

    do {
      ((uint32_t*)dst)[0] = srcPal[src[0]];

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL argb32_from_prgb32_swapped(
    uint8_t* _dst, const uint8_t* _src, int w, const RasterClosure* closure)
  {
    uint32_t* dst = reinterpret_cast<uint32_t*>(_dst);
    const uint32_t* src = reinterpret_cast<const uint32_t*>(_src);

    int i = 0;

    while (i < w)
    {
      // We want to identify long runs of constant input pixels and
      // cache the unpremultiplied.
      uint32_t const_in, const_out;

      // Diff is the or of all bitwise differences from const_in
      // during the probe period.  If it is zero after the probe
      // period then every input pixel was identical in the
      // probe.
      unsigned diff = 0;

      // Accumulator for all alphas of the probe period pixels,
      // biased to make the sum zero if the
      unsigned accu = -2*255;

      {
        uint32_t rgba, a, r, g, b, recip;

        rgba = const_in = src[i];
        a = (rgba      ) & 0xFFU;
        accu += a;
        r = (rgba >>  8) & 0xFFU;
        g = (rgba >> 16) & 0xFFU;
        b = (rgba >> 24);
        recip = raster_demultiply_reciprocal_table_d[a];

        r = ((r * recip)      ) & 0x00FF0000;
        g = ((g * recip) >>  8) & 0x0000FF00;
        b = ((b * recip) >> 16) & 0x000000FFU;

        dst[i] = const_out = r | g | b | (a << 24);
      }

      if (++i == w) return;

      {
        uint32_t rgba, a, r, g, b, recip;
        rgba = src[i];
        a = (rgba      ) & 0xFFU;
        accu += a;
        r = (rgba >>  8) & 0xFFU;
        g = (rgba >> 16) & 0xFFU;
        b = (rgba >> 24);
        recip = raster_demultiply_reciprocal_table_d[a];

        diff = rgba ^ const_in;

        r = ((r * recip)      ) & 0x00FF0000;
        g = ((g * recip) >>  8) & 0x0000FF00;
        b = ((b * recip) >> 16) & 0x000000FFU;

        dst[i] = r | g | b | (a << 24);
      }

      if (++i == w) return;

      // Fall into special cases if we have special circumstances.
      if (0 != (accu & diff)) continue;

      if (0 == accu)
      {
        // a run of solid pixels.
        uint32_t in;
        while (0xFF000000U == ((in = Memory::bswap32(src[i])) & 0xFF000000U))
        {
          dst[i] = in;
          if (++i == w) return;
        }
      }
      else if (0 == diff)
      {
        // a run of constant pixels.
        while (src[i] == const_in)
        {
          dst[i] = const_out;
          if (++i == w) return;
        }
      }
    }
  }

  static void FOG_FASTCALL frgb32_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = ColorUtil::premultiply(((const uint32_t*)src)[0]);

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL fzzz32_from_null(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = 0xFF000000U;

      dst += 4;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      ((uint32_t*)dst)[0] = srcPal[src[0]] | 0xFF000000U;

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_from_argb32_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = Memory::bswap32(ColorUtil::premultiply(((const uint32_t*)src)[0])) | 0xFF000000U;

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL a8_from_axxx32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    src += ARGB32_ABYTE;

    do {
      dst[0] = src[0];

      dst += 1;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL f8_from_null(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      *dst++ = 0xFFU;
    } while (--w);
  }

  static void FOG_FASTCALL a8_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint8_t* srcPal = (uint8_t*)(closure->srcPalette + Palette::INDEX_ARGB32);
    srcPal += ARGB32_ABYTE;

    do {
      dst[0] = srcPal[src[0]];

      dst += 1;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL a8_from_axxx32_swapped(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    src += 3 - ARGB32_ABYTE;

    do {
      dst[0] = src[0];

      dst += 1;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_swapped_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = Memory::bswap32(ColorUtil::premultiply(((const uint32_t*)src)[0]));

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_swapped_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = Memory::bswap32(((const uint32_t*)src)[0] | 0xFF000000U);

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL azzz32_swapped_from_a8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = (uint32_t)src[0];

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_swapped_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      ((uint32_t*)dst)[0] = Memory::bswap32(srcPal[src[0]]);

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL argb32_swapped_from_prgb32(
    uint8_t* _dst, const uint8_t* _src, int w, const RasterClosure* closure)
  {
    uint32_t* dst = reinterpret_cast<uint32_t*>(_dst);
    const uint32_t* src = reinterpret_cast<const uint32_t*>(_src);

    int i = 0;

    while (i < w)
    {
      // We want to identify long runs of constant input pixels and
      // cache the unpremultiplied.
      uint32_t const_in, const_out;

      // Diff is the or of all bitwise differences from const_in
      // during the probe period.  If it is zero after the probe
      // period then every input pixel was identical in the
      // probe.
      unsigned diff = 0;

      // Accumulator for all alphas of the probe period pixels,
      // biased to make the sum zero if the
      unsigned accu = -2*255;

      {
        uint32_t rgba, a, r, g, b, recip;

        rgba = const_in = src[i];
        a = (rgba >> 24);
        accu += a;
        r = (rgba >> 16) & 0xFFU;
        g = (rgba >> 8 ) & 0xFFU;
        b = (rgba      ) & 0xFFU;
        recip = raster_demultiply_reciprocal_table_d[a];

        r = ((r * recip) >>  8) & 0x0000FF00U;
        g = ((g * recip)      ) & 0x00FF0000U;
        b = ((b * recip) <<  8) & 0xFF000000U;

        dst[i] = const_out = r | g | b | a;
      }

      if (++i == w) return;

      {
        uint32_t rgba, a, r, g, b, recip;

        rgba = const_in = src[i];
        a = (rgba >> 24);
        accu += a;
        r = (rgba >> 16) & 0xFFU;
        g = (rgba >> 8 ) & 0xFFU;
        b = (rgba      ) & 0xFFU;
        recip = raster_demultiply_reciprocal_table_d[a];

        diff = rgba ^ const_in;

        r = ((r * recip) >>  8) & 0x0000FF00U;
        g = ((g * recip)      ) & 0x00FF0000U;
        b = ((b * recip) <<  8) & 0xFF000000U;

        dst[i] = r | g | b | a;
      }

      if (++i == w) return;

      // Fall into special cases if we have special circumstances.
      if (0 != (accu & diff)) continue;

      if (0 == accu)
      {
        // a run of solid pixels.
        uint32_t in;
        while (0xFF000000U == ((in = src[i]) & 0xFF000000U))
        {
          dst[i] = Memory::bswap32(in);
          if (++i == w) return;
        }
      }
      else if (0 == diff)
      {
        // a run of constant pixels.
        while (src[i] == const_in)
        {
          dst[i] = const_out;
          if (++i == w) return;
        }
      }
    }
  }

  static void FOG_FASTCALL argb32_swapped_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_ARGB32;

    do {
      ((uint32_t*)dst)[0] = Memory::bswap32(srcPal[src[0]]);

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_swapped_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = Memory::bswap32(ColorUtil::premultiply(((const uint32_t*)src)[0]) | 0xFF000000U);

      dst += 4;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL fzzz32_swapped_from_null(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      ((uint32_t*)dst)[0] = 0x000000FFU;

      dst += 4;
    } while (--w);
  }

  static void FOG_FASTCALL frgb32_swapped_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      ((uint32_t*)dst)[0] = Memory::bswap32(srcPal[src[0]] | 0xFF000000U);

      dst += 4;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL rgb24_native_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    // Align.
    while (w && (sysuint_t)dst & 0x3)
    {
      uint32_t s0 = ((const uint32_t*)src)[0];

      dst[RGB24_NATIVE_RBYTE] = (uint8_t)(s0 >> 16);
      dst[RGB24_NATIVE_GBYTE] = (uint8_t)(s0 >> 8);
      dst[RGB24_NATIVE_BBYTE] = (uint8_t)(s0);

      dst += 3;
      src += 4;
      w--;
    }

    // 4 Pixels per time.
    while (w >= 4)
    {
      // Following table illustrates how this works on Little Endian:
      //
      // [B G R X] [B G R X] [B G R X] [B G R X]
      // [B G R B] [G R B G] [R B G R]
      uint32_t s0 = ((const uint32_t*)src)[0];
      uint32_t s1 = ((const uint32_t*)src)[1];
      uint32_t s2 = ((const uint32_t*)src)[2];
      uint32_t s3 = ((const uint32_t*)src)[3];

      ((uint32_t*)dst)[0] = (s0 & 0x00FFFFFFU) | (s1 << 24); s1 >>= 8;
      ((uint32_t*)dst)[1] = (s1 & 0x0000FFFFU) | (s2 << 16); s2 >>= 16;
      ((uint32_t*)dst)[2] = (s2 & 0x000000FFU) | (s3 <<  8);

      dst += 12;
      src += 16;
      w -= 4;
    }
#endif // FOG_BYTE_ORDER

    while (w)
    {
      uint32_t s0 = ((const uint32_t*)src)[0];

      dst[RGB24_NATIVE_RBYTE] = (uint8_t)(s0 >> 16);
      dst[RGB24_NATIVE_GBYTE] = (uint8_t)(s0 >> 8);
      dst[RGB24_NATIVE_BBYTE] = (uint8_t)(s0);

      dst += 3;
      src += 4;
      w--;
    }
  }

  static void FOG_FASTCALL rgb24_native_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    // Align.
    while (w && (sysuint_t)dst & 0x3)
    {
      uint32_t s0 = ColorUtil::premultiply(((const uint32_t*)src)[0]);

      dst[RGB24_NATIVE_RBYTE] = (uint8_t)(s0 >> 16);
      dst[RGB24_NATIVE_GBYTE] = (uint8_t)(s0 >> 8);
      dst[RGB24_NATIVE_BBYTE] = (uint8_t)(s0);

      dst += 3;
      src += 4;
      w--;
    }

    // 4 Pixels per time.
    while (w >= 4)
    {
      // Following table illustrates how this works on Little Endian:
      //
      // [B G R X] [B G R X] [B G R X] [B G R X]
      // [B G R B] [G R B G] [R B G R]
      uint32_t s0 = ColorUtil::premultiply(((const uint32_t*)src)[0]);
      uint32_t s1 = ColorUtil::premultiply(((const uint32_t*)src)[1]);
      uint32_t s2 = ColorUtil::premultiply(((const uint32_t*)src)[2]);
      uint32_t s3 = ColorUtil::premultiply(((const uint32_t*)src)[3]);

      ((uint32_t*)dst)[0] = (s0 & 0x00FFFFFFU) | (s1 << 24); s1 >>= 8;
      ((uint32_t*)dst)[1] = (s1 & 0x0000FFFFU) | (s2 << 16); s2 >>= 16;
      ((uint32_t*)dst)[2] = (s2 & 0x000000FFU) | (s3 <<  8);

      dst += 12;
      src += 16;
      w -= 4;
    }
#endif // FOG_BYTE_ORDER

    while (w)
    {
      uint32_t s0 = ColorUtil::premultiply(((const uint32_t*)src)[0]);

      dst[RGB24_NATIVE_RBYTE] = (uint8_t)(s0 >> 16);
      dst[RGB24_NATIVE_GBYTE] = (uint8_t)(s0 >> 8);
      dst[RGB24_NATIVE_BBYTE] = (uint8_t)(s0);

      dst += 3;
      src += 4;
      w--;
    }
  }

  static void FOG_FASTCALL zzz24_from_null(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    // Align.
    while (w && (sysuint_t)dst & 0x3)
    {
      dst[0] = 0x00U;
      dst[1] = 0x00U;
      dst[2] = 0x00U;

      dst += 3;
      w--;
    }

    // 4 Pixels per time.
    while (w >= 4)
    {
      ((uint32_t*)dst)[0] = 0x00000000U;
      ((uint32_t*)dst)[1] = 0x00000000U;
      ((uint32_t*)dst)[2] = 0x00000000U;

      dst += 12;
      w -= 4;
    }

    while (w)
    {
      dst[0] = 0x00U;
      dst[1] = 0x00U;
      dst[2] = 0x00U;

      dst += 3;
      w--;
    }
  }

  static void FOG_FASTCALL rgb24_native_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    // Align.
    while (w && (sysuint_t)dst & 0x3)
    {
      uint32_t s0 = srcPal[src[0]];

      dst[RGB24_NATIVE_RBYTE] = (uint8_t)(s0 >> 16);
      dst[RGB24_NATIVE_GBYTE] = (uint8_t)(s0 >> 8);
      dst[RGB24_NATIVE_BBYTE] = (uint8_t)(s0);

      dst += 3;
      src += 1;
      w--;
    }

    // 4 Pixels per time.
    while (w >= 4)
    {
      // Following table illustrates how this works on Little Endian:
      //
      // [X R G B] [X R G B] [X R G B] [X R G B]
      // [B G R B] [G R B G] [R B G R]
      uint32_t s0 = srcPal[src[0]];
      uint32_t s1 = srcPal[src[1]];
      uint32_t s2 = srcPal[src[2]];
      uint32_t s3 = srcPal[src[3]];

      ((uint32_t*)dst)[0] = (s0 & 0x00FFFFFFU) | (s1 << 24); s1 >>= 8;
      ((uint32_t*)dst)[1] = (s1 & 0x0000FFFFU) | (s2 << 16); s2 >>= 16;
      ((uint32_t*)dst)[2] = (s2 & 0x000000FFU) | (s3 <<  8);

      dst += 12;
      src += 4;
      w -= 4;
    }
#endif // FOG_BYTE_ORDER

    while (w)
    {
      uint32_t s0 = srcPal[src[0]];

      dst[RGB24_NATIVE_RBYTE] = (uint8_t)(s0 >> 16);
      dst[RGB24_NATIVE_GBYTE] = (uint8_t)(s0 >> 8);
      dst[RGB24_NATIVE_BBYTE] = (uint8_t)(s0);

      dst += 3;
      src += 1;
      w--;
    }
  }

  static void FOG_FASTCALL rgb24_swapped_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ((const uint32_t*)src)[0];

      dst[RGB24_SWAPPED_RBYTE] = (uint8_t)(s0 >> 16);
      dst[RGB24_SWAPPED_GBYTE] = (uint8_t)(s0 >> 8);
      dst[RGB24_SWAPPED_BBYTE] = (uint8_t)(s0);

      dst += 3;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb24_swapped_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ColorUtil::premultiply(((const uint32_t*)src)[0]);

      dst[RGB24_SWAPPED_RBYTE] = (uint8_t)(s0 >> 16);
      dst[RGB24_SWAPPED_GBYTE] = (uint8_t)(s0 >> 8);
      dst[RGB24_SWAPPED_BBYTE] = (uint8_t)(s0);

      dst += 3;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb24_swapped_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      uint32_t s0 = srcPal[src[0]];

      dst[RGB24_SWAPPED_RBYTE] = (uint8_t)(s0 >> 16);
      dst[RGB24_SWAPPED_GBYTE] = (uint8_t)(s0 >> 8);
      dst[RGB24_SWAPPED_BBYTE] = (uint8_t)(s0);

      dst += 3;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_565_native_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ((const uint32_t*)src)[0];
      RGB16_565_NATIVE_IO::store(dst, s0);

      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_565_native_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ColorUtil::premultiply(((const uint32_t*)src)[0]);
      RGB16_565_NATIVE_IO::store(dst, s0);

      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL zzz16_from_null(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    // Align.
    while (w && (sysuint_t)dst & 0x3)
    {
      ((uint16_t*)dst)[0] = 0x0000U;

      dst += 2;
      w--;
    }

    // 4 Pixels per time.
    while (w >= 4)
    {
      ((uint32_t*)dst)[0] = 0x00000000U;
      ((uint32_t*)dst)[1] = 0x00000000U;

      dst += 8;
      w -= 4;
    }

    while (w)
    {
      ((uint16_t*)dst)[0] = 0x0000U;

      dst += 2;
      w--;
    }
  }

  static void FOG_FASTCALL rgb16_565_native_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      uint32_t s0 = srcPal[src[0]];
      RGB16_565_NATIVE_IO::store(dst, s0);

      dst += 2;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_565_swapped_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ((const uint32_t*)src)[0];
      RGB16_565_SWAPPED_IO::store(dst, s0);

      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_565_swapped_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ColorUtil::premultiply(((const uint32_t*)src)[0]);
      RGB16_565_SWAPPED_IO::store(dst, s0);

      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_565_swapped_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      uint32_t s0 = srcPal[src[0]];
      RGB16_565_SWAPPED_IO::store(dst, s0);

      dst += 2;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_555_native_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ((const uint32_t*)src)[0];
      RGB16_555_NATIVE_IO::store(dst, s0);

      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_555_native_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ColorUtil::premultiply(((const uint32_t*)src)[0]);
      RGB16_555_NATIVE_IO::store(dst, s0);

      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_555_native_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      uint32_t s0 = srcPal[src[0]];
      RGB16_555_NATIVE_IO::store(dst, s0);

      dst += 2;
      src += 1;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_555_swapped_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ((const uint32_t*)src)[0];
      RGB16_555_SWAPPED_IO::store(dst, s0);

      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_555_swapped_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ColorUtil::premultiply(((const uint32_t*)src)[0]);
      RGB16_555_SWAPPED_IO::store(dst, s0);

      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_555_swapped_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      uint32_t s0 = srcPal[src[0]];
      RGB16_555_SWAPPED_IO::store(dst, s0);

      dst += 2;
      src += 1;
    } while (--w);
  }

  // Greyscale from RGB conversion.
  //
  // This formula is used:
  //  0.212671 * R + 0.715160 * G + 0.072169 * B;
  // As:
  //  (13938 * R + 46868 * G + 4730 * B) / 65536

  static void FOG_FASTCALL grey8_from_xrgb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ((const uint32_t*)src)[0];
      uint32_t grey = ((s0 >> 16) & 0xFFU) * 13938 +
                      ((s0 >>  8) & 0xFFU) * 46868 +
                      ((s0      ) & 0xFFU) *  4730 ;
      dst[0] = (uint8_t)(grey >> 16);

      dst += 1;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL grey8_from_argb32(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      uint32_t s0 = ColorUtil::premultiply(((const uint32_t*)src)[0]);
      uint32_t grey = ((s0 >> 16) & 0xFFU) * 13938 +
                      ((s0 >>  8) & 0xFFU) * 46868 +
                      ((s0      ) & 0xFFU) *  4730 ;
      dst[0] = (uint8_t)(grey >> 16);

      dst += 1;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL z8_from_null(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    do {
      dst[0] = 0x00U;

      dst += 1;
    } while (--w);
  }

  static void FOG_FASTCALL grey8_from_i8(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    const uint32_t* srcPal = closure->srcPalette + Palette::INDEX_PRGB32;

    do {
      uint32_t s0 = srcPal[src[0]];
      uint32_t grey = ((s0 >> 16) & 0xFFU) * 13938 +
                      ((s0 >>  8) & 0xFFU) * 46868 +
                      ((s0      ) & 0xFFU) *  4730 ;
      dst[0] = grey >> 16;

      dst += 1;
      src += 1;
    } while (--w);
  }

  // --------------------------------------------------------------------------
  // [DibC - Dither]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL i8rgb232_from_xrgb32_dither(
    uint8_t* dst, const uint8_t* src, int w, const IntPoint& origin, const uint8_t* palConv)
  {
    const uint8_t* dt = DitherMatrix::matrix[origin.getY() & DitherMatrix::MASK];
    int dx = origin.getX();

    do {
      uint32_t c0;
      uint32_t r0, g0, b0, d, dith2, dith3;

      d = dt[dx & DitherMatrix::MASK];

      dith2 = DitherMatrix::shf_dither(d, DitherMatrix::shf_arg(2));
      dith3 = DitherMatrix::shf_dither(d, DitherMatrix::shf_arg(3));

      c0 = ((uint32_t *)src)[0];
      r0 = (c0 & 0x00FF0000) >> 16;
      g0 = (c0 & 0x0000FF00) >> 8;
      b0 = (c0 & 0x000000FFU);

      if (((r0 & 0x3F) >= dith2) && (r0 < 0xC0)) r0 += 64;
      if (((g0 & 0x1F) >= dith3) && (g0 < 0xE0)) g0 += 32;
      if (((b0 & 0x3F) >= dith2) && (b0 < 0xC0)) b0 += 64;

      ((uint8_t *)dst)[0] = palConv[((r0 & 0xC0) >> 1) | ((g0 & 0xE0) >> 3) | ((b0 & 0xC0) >> 6)];

      dx++;
      dst += 1;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL i8rgb222_from_xrgb32_dither(
    uint8_t* dst, const uint8_t* src, int w, const IntPoint& origin, const uint8_t* palConv)
  {
    const uint8_t* dt = DitherMatrix::matrix[origin.getY() & DitherMatrix::MASK];
    int dx = origin.getX();

    do {
      uint32_t c0;
      uint32_t r0, g0, b0, dith2;
      uint32_t r0t, g0t, b0t;

      dith2 = DitherMatrix::shf_dither(dt[dx & DitherMatrix::MASK], DitherMatrix::shf_arg(2)) * 4 / 3;

      c0 = ((uint32_t *)src)[0];
      r0 = (c0 & 0x00FF0000) >> 16;
      g0 = (c0 & 0x0000FF00) >> 8;
      b0 = (c0 & 0x000000FFU);

      // c = c / 85
      r0t = (r0 * 0x00030000) >> 16;
      g0t = (g0 * 0x00030000) >> 16;
      b0t = (b0 * 0x00030000) >> 16;

      if ((r0 - (r0t * 85)) > dith2) r0t++;
      if ((g0 - (g0t * 85)) > dith2) g0t++;
      if ((b0 - (b0t * 85)) > dith2) b0t++;
      ((uint8_t *)dst)[0] = palConv[(r0t<<4)|(g0t<<2)|(b0t)];

      dx++;
      dst += 1;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL i8rgb111_from_xrgb32_dither(
    uint8_t* dst, const uint8_t* src, int w, const IntPoint& origin, const uint8_t* palConv)
  {
    const uint8_t* dt = DitherMatrix::matrix[origin.getY() & DitherMatrix::MASK];
    int dx = origin.getX();

    do {
      uint32_t c0;
      uint32_t r0, g0, b0, dith1;

      dith1 = DitherMatrix::shf_dither(dt[dx & DitherMatrix::MASK], DitherMatrix::shf_arg(0));

      c0 = ((uint32_t *)src)[0];
      r0 = (c0 & 0x00FF0000) >> 16;
      g0 = (c0 & 0x0000FF00) >> 8;
      b0 = (c0 & 0x000000FFU);

      if (r0 > dith1) r0 = 255;
      if (g0 > dith1) g0 = 255;
      if (b0 > dith1) b0 = 255;

      ((uint8_t *)dst)[0] = palConv[
        (((r0 + 1) >> 8) << 2) |
        (((g0 + 1) >> 8) << 1) |
        (((b0 + 1) >> 8)     ) ];

      dx++;
      dst += 1;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_555_native_from_xrgb32_dither(
    uint8_t* dst, const uint8_t* src, int w, const IntPoint& origin)
  {
    const uint8_t* dt = DitherMatrix::matrix[origin.getY() & DitherMatrix::MASK];
    int dx = origin.getX();

    do {
      uint32_t c0;
      uint32_t r0, g0, b0, dith;

      c0 = ((uint32_t *)src)[0];
      r0 = (c0 & 0x00FF0000) >> 16;
      g0 = (c0 & 0x0000FF00) >> 8;
      b0 = (c0 & 0x000000FFU);

      dith = DitherMatrix::shf_dither(dt[dx & DitherMatrix::MASK], DitherMatrix::shf_arg(5));
      if (((r0 & 7) >= dith) && (r0 < 0xF8)) r0 += 8;
      if (((g0 & 7) >= dith) && (g0 < 0xF8)) g0 += 8;
      if (((b0 & 7) >= dith) && (b0 < 0xF8)) b0 += 8;

      ((uint16_t *)dst)[0] = ((r0 & 0xF8) << 7) | ((g0 & 0xF8) << 2) | ((b0 & 0xF8) >> 3);

      dx++;
      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_565_native_from_xrgb32_dither(
    uint8_t* dst, const uint8_t* src, int w, const IntPoint& origin)
  {
    const uint8_t* dt = DitherMatrix::matrix[origin.getY() & DitherMatrix::MASK];
    int dx = origin.getX();

    do {
      uint32_t c0;
      uint32_t r0, g0, b0, d, dith5, dith6;

      c0 = ((uint32_t *)src)[0];
      r0 = (c0 & 0x00FF0000) >> 16;
      g0 = (c0 & 0x0000FF00) >> 8;
      b0 = (c0 & 0x000000FFU);

      d = dt[dx & DitherMatrix::MASK];
      dith5 = DitherMatrix::shf_dither(d, DitherMatrix::shf_arg(5));
      dith6 = DitherMatrix::shf_dither(d, DitherMatrix::shf_arg(6));
      if (((r0 & 7) >= dith5) && (r0 < 0xF8)) r0 += 8;
      if (((g0 & 3) >= dith6) && (g0 < 0xFC)) g0 += 4;
      if (((b0 & 7) >= dith5) && (b0 < 0xF8)) b0 += 8;

      ((uint16_t *)dst)[0] = ((r0 & 0xF8) << 8) | ((g0 & 0xFC) << 3) | ((b0 & 0xF8) >> 3);

      dx++;
      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_555_swapped_from_xrgb32_dither(
    uint8_t* dst, const uint8_t* src, int w, const IntPoint& origin)
  {
    const uint8_t* dt = DitherMatrix::matrix[origin.getY() & DitherMatrix::MASK];
    int dx = origin.getX();

    do {
      uint32_t c0;
      uint32_t r0, g0, b0, dith;

      c0 = ((uint32_t *)src)[0];
      r0 = (c0 & 0x00FF0000) >> 16;
      g0 = (c0 & 0x0000FF00) >> 8;
      b0 = (c0 & 0x000000FFU);

      dith = DitherMatrix::shf_dither(dt[dx & DitherMatrix::MASK], DitherMatrix::shf_arg(5));
      if (((r0 & 7) >= dith) && (r0 < 0xF8)) r0 += 8;
      if (((g0 & 7) >= dith) && (g0 < 0xF8)) g0 += 8;
      if (((b0 & 7) >= dith) && (b0 < 0xF8)) b0 += 8;

      ((uint16_t *)dst)[0] = Memory::bswap16(((r0 & 0xF8) << 7) | ((g0 & 0xF8) << 2) | ((b0 & 0xF8) >> 3));

      dx++;
      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL rgb16_565_swapped_from_xrgb32_dither(
    uint8_t* dst, const uint8_t* src, int w, const IntPoint& origin)
  {
    const uint8_t* dt = DitherMatrix::matrix[origin.getY() & DitherMatrix::MASK];
    int dx = origin.getX();

    do {
      uint32_t c0;
      uint32_t r0, g0, b0, d, dith5, dith6;

      c0 = ((uint32_t *)src)[0];
      r0 = (c0 & 0x00FF0000) >> 16;
      g0 = (c0 & 0x0000FF00) >> 8;
      b0 = (c0 & 0x000000FFU);

      d = dt[dx & DitherMatrix::MASK];
      dith5 = DitherMatrix::shf_dither(d, DitherMatrix::shf_arg(5));
      dith6 = DitherMatrix::shf_dither(d, DitherMatrix::shf_arg(6));
      if (((r0 & 7) >= dith5) && (r0 < 0xF8)) r0 += 8;
      if (((g0 & 3) >= dith6) && (g0 < 0xFC)) g0 += 4;
      if (((b0 & 7) >= dith5) && (b0 < 0xF8)) b0 += 8;

      ((uint16_t *)dst)[0] = Memory::bswap16(((r0 & 0xF8) << 8) | ((g0 & 0xFC) << 3) | ((b0 & 0xF8) >> 3));

      dx++;
      dst += 2;
      src += 4;
    } while (--w);
  }

  static void FOG_FASTCALL prgb32_from_argb32_span(
    uint8_t* _dst, const Span* span, const RasterClosure* closure)
  {
    uint32_t* pixels = reinterpret_cast<uint32_t*>(_dst);

    do {
      uint x1 = (uint)span->getX1();
      uint x2 = (uint)span->getX2();

      while (x1 < x2)
      {
        uint32_t pix0 = pixels[x1];

        if (!RasterUtil::isAlpha0xFF_ARGB32(pix0))
          pixels[x1] = ColorUtil::premultiply(pix0);
        x1++;
      }
    } while ((span = span->getNext()) != NULL);
  }

  static void FOG_FASTCALL argb32_from_prgb32_span(
    uint8_t* _dst, const Span* span, const RasterClosure* closure)
  {
    uint32_t* pixels = reinterpret_cast<uint32_t*>(_dst);

    do {
      uint x1 = (uint)span->getX1();
      uint x2 = (uint)span->getX2();

      while (x1 < x2)
      {
        // We want to identify long runs of constant input pixels and
        // cache the unpremultiplied.
        uint32_t const_in, const_out;

        // Diff is the or of all bitwise differences from const_in
        // during the probe period.  If it is zero after the probe
        // period then every input pixel was identical in the
        // probe.
        unsigned diff = 0;

        // Accumulator for all alphas of the probe period pixels,
        // biased to make the sum zero if the
        unsigned accu = -2*255;

        {
          uint32_t rgba, a, r, g, b, recip;

          rgba = const_in = pixels[x1];
          a = (rgba >> 24);
          accu += a;
          r = (rgba >> 16) & 0xFFU;
          g = (rgba >> 8 ) & 0xFFU;
          b = (rgba      ) & 0xFFU;
          recip = raster_demultiply_reciprocal_table_d[a];

          r = ((r * recip)      ) & 0x00FF0000;
          g = ((g * recip) >>  8) & 0x0000FF00;
          b = ((b * recip) >> 16) & 0x000000FFU;

          pixels[x1] = const_out = r | g | b | (a << 24);
        }

        if (++x1 == x2) break;

        {
          uint32_t rgba, a, r, g, b, recip;
          rgba = pixels[x1];
          a = (rgba >> 24);
          accu += a;
          r = (rgba >> 16) & 0xFFU;
          g = (rgba >>  8) & 0xFFU;
          b = (rgba      ) & 0xFFU;
          recip = raster_demultiply_reciprocal_table_d[a];

          diff = rgba ^ const_in;

          r = ((r * recip)      ) & 0x00FF0000;
          g = ((g * recip) >>  8) & 0x0000FF00;
          b = ((b * recip) >> 16) & 0x000000FFU;

          pixels[x1] = r | g | b | (a << 24);
        }

        if (++x1 == x2) break;

        // Fall into special cases if we have special circumstances.
        if (0 != (accu & diff)) continue;

        if (0 == accu)
        {
          // a run of solid pixels.
          uint32_t in;
          while (0xFF000000U == ((in = pixels[x1]) & 0xFF000000U))
          {
            pixels[x1] = in;
            if (++x1 == x2) goto nextSpan;
          }
        }
        else if (0 == diff)
        {
          // a run of constant pixels.
          while (pixels[x1] == const_in)
          {
            pixels[x1] = const_out;
            if (++x1 == x2) goto nextSpan;
          }
        }
      }
nextSpan:
      ;
    } while ((span = span->getNext()) != NULL);
  }
};

} // RasterEngine namespace
} // Fog namespace
