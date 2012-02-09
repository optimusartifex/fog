// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_BASEACCESS_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_BASEACCESS_P_H

// [Dependencies]
#include <Fog/Core/Face/FaceC.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - PixelPRGB32]
// ============================================================================

struct FOG_NO_EXPORT PixelPRGB32
{
  enum { SIZE = 4, HAS_RGB = 1, HAS_ALPHA = 1, HAS_X = 0, HAS_F = 0, IS_PREMULTIPLIED = 1 };

  // ==========================================================================
  // [Pixel32]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixel32(uint32_t& dst0p, const void* src)
  {
    Face::p32Load4a(dst0p, src);
  }

  static FOG_INLINE void p32LoadPixel32_2031(uint32_t& dst0p_20, uint32_t& dst0p_31, const void* src)
  {
    Face::p32Load4a(dst0p_20, src);
    Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
  }

  static FOG_INLINE void p32StorePixel32(void* dst, const uint32_t& src0p)
  {
    Face::p32Store4a(dst, src0p);
  }

  static FOG_INLINE void p32StorePixel32_2031(void* dst, const uint32_t& src0p_20, const uint32_t& src0p_31)
  {
    uint32_t src0p;

    Face::p32PackPBB2031FromPBW(src0p, src0p_20, src0p_31);
    Face::p32Store4a(dst, src0p);
  }

  // ==========================================================================
  // [PixelA8]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixelA8(uint32_t& dst0p, const void* src)
  {
    Face::p32Load1b(dst0p, static_cast<const uint8_t*>(src) + PIXEL_ARGB32_POS_A);
  }

  static FOG_INLINE void p32StorePixelA8(void* dst, const uint32_t& src0p)
  {
    uint32_t a0p;
    Face::p32ExtendPBBFromSBB(a0p, src0p);
    Face::p32Store4a(dst, a0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - PixelXRGB32]
// ============================================================================

struct FOG_NO_EXPORT PixelXRGB32
{
  enum { SIZE = 4, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 1, HAS_F = 0, IS_PREMULTIPLIED = 1 };

  // ==========================================================================
  // [Pixel32]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixel32(uint32_t& dst0p, const void* src)
  {
    Face::p32Load4a(dst0p, src);
  }

  static FOG_INLINE void p32LoadPixel32_2031(uint32_t& dst0p_20, uint32_t& dst0p_31, const void* src)
  {
    Face::p32Load4a(dst0p_20, src);
    Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
  }

  static FOG_INLINE void p32StorePixel32(void* dst, const uint32_t& src0p)
  {
    Face::p32Store4a(dst, src0p);
  }

  static FOG_INLINE void p32StorePixel32_2031(void* dst, const uint32_t& src0p_20, const uint32_t& src0p_31)
  {
    uint32_t src0p;

    Face::p32PackPBB2031FromPBW(src0p, src0p_20, src0p_31);
    Face::p32Store4a(dst, src0p);
  }

  // ==========================================================================
  // [PixelA8]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixelA8(uint32_t& dst0p, const void* src)
  {
    dst0p = 0xFF;
  }
};

// ============================================================================
// [Fog::RasterOps_C - PixelFRGB32]
// ============================================================================

struct FOG_NO_EXPORT PixelFRGB32
{
  enum { SIZE = 4, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, HAS_F = 1, IS_PREMULTIPLIED = 1 };

  // ==========================================================================
  // [Pixel32]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixel32(uint32_t& dst0p, const void* src)
  {
    Face::p32Load4a(dst0p, src);
  }

  static FOG_INLINE void p32LoadPixel32_2031(uint32_t& dst0p_20, uint32_t& dst0p_31, const void* src)
  {
    Face::p32Load4a(dst0p_20, src);
    Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
  }

  static FOG_INLINE void p32StorePixel32(void* dst, const uint32_t& src0p)
  {
    Face::p32Store4a(dst, src0p);
  }

  static FOG_INLINE void p32StorePixel32_2031(void* dst, const uint32_t& src0p_20, const uint32_t& src0p_31)
  {
    uint32_t src0p;

    Face::p32PackPBB2031FromPBW(src0p, src0p_20, src0p_31);
    Face::p32Store4a(dst, src0p);
  }

  // ==========================================================================
  // [PixelA8]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixelA8(uint32_t& dst0p, const void* src)
  {
    dst0p = 0xFF;
  }
};

// ============================================================================
// [Fog::RasterOps_C - PixelRGB24]
// ============================================================================

struct FOG_NO_EXPORT PixelRGB24
{
  enum { SIZE = 3, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, HAS_F = 0, IS_PREMULTIPLIED = 1 };

  // ==========================================================================
  // [Pixel32]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixel32(uint32_t& dst0p, const void* src)
  {
    Face::p32Load3b(dst0p, src);
  }

  static FOG_INLINE void p32LoadPixel32_2031(uint32_t& dst0p_20, uint32_t& dst0p_31, const void* src)
  {
    dst0p_20  = (static_cast<uint32_t>(static_cast<const uint8_t*>(src)[PIXEL_RGB24_POS_R]) << 16);
    dst0p_31  = (static_cast<uint32_t>(static_cast<const uint8_t*>(src)[PIXEL_RGB24_POS_G])      );
    dst0p_20 += (static_cast<uint32_t>(static_cast<const uint8_t*>(src)[PIXEL_RGB24_POS_B])      );
  }

  static FOG_INLINE void p32StorePixel32(void* dst, const uint32_t& src0p)
  {
    Face::p32Store3b(dst, src0p);
  }

  static FOG_INLINE void p32StorePixel32_2031(void* dst, const uint32_t& src0p_20, const uint32_t& src0p_31)
  {
    static_cast<uint8_t*>(dst)[PIXEL_RGB24_POS_B] = static_cast<uint8_t>(src0p_20);
    static_cast<uint8_t*>(dst)[PIXEL_RGB24_POS_G] = static_cast<uint8_t>(src0p_31);
    static_cast<uint8_t*>(dst)[PIXEL_RGB24_POS_R] = static_cast<uint8_t>(src0p_20 >> 16);
  }

  // ==========================================================================
  // [PixelA8]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixelA8(uint32_t& dst0p, const void* src)
  {
    dst0p = 0xFF;
  }
};

// ============================================================================
// [Fog::RasterOps_C - PixelA8]
// ============================================================================

struct FOG_NO_EXPORT PixelA8
{
  enum { SIZE = 1, HAS_RGB = 0, HAS_ALPHA = 1, HAS_X = 0, HAS_F = 0, IS_PREMULTIPLIED = 1 };

  // ==========================================================================
  // [Pixel32]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixel32(uint32_t& dst0p, const void* src)
  {
    Face::p32Load1b(dst0p, src);
    Face::p32ExtendPBBFromSBB(dst0p, dst0p);
  }

  static FOG_INLINE void p32LoadPixel32_2031(uint32_t& dst0p_20, uint32_t& dst0p_31, const void* src)
  {
    Face::p32Load1b(dst0p_20, src);

    Face::p32ExtendPBWFromSBW(dst0p_20, dst0p_20);
    Face::p32Copy(dst0p_31, dst0p_20);
  }

  static FOG_INLINE void p32StorePixel32(void* dst, const uint32_t& src0p)
  {
    uint32_t a0p = src0p >> 24;
    Face::p32Store1b(dst, a0p);
  }

  static FOG_INLINE void p32StorePixel32_2031(void* dst, const uint32_t& src0p_20, const uint32_t& src0p_31)
  {
    static_cast<uint8_t*>(dst)[0] = static_cast<uint8_t>(src0p_31 >> 16);
  }

  // ==========================================================================
  // [PixelA8]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixelA8(uint32_t& dst0p, const void* src)
  {
    Face::p32Load1b(dst0p, static_cast<const uint8_t*>(src));
  }

  static FOG_INLINE void p32StorePixelA8(void* dst, const uint32_t& src0p)
  {
    Face::p32Store1b(static_cast<uint8_t*>(dst), src0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - PixelPRGB64]
// ============================================================================

struct FOG_NO_EXPORT PixelPRGB64
{
  enum { SIZE = 8, HAS_RGB = 1, HAS_ALPHA = 1, HAS_X = 0, HAS_F = 0, IS_PREMULTIPLIED = 1 };

  // ==========================================================================
  // [Pixel32]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixel32(uint32_t& dst0p, const void* src)
  {
    uint32_t src0p_10, src0p_32;

    Face::p32Load8a(src0p_10, src0p_32, src);
    Face::p32ARGB32FromARGB64_1032(dst0p, src0p_10, src0p_32);
  }

  static FOG_INLINE void p32LoadPixel32_2031(uint32_t& dst0p_20, uint32_t& dst0p_31, const void* src)
  {
#if defined(FOG_ARCH_NATIVE_P64)
    __p64 dst0p_lo;
    __p64 dst0p_hi;

    Face::p64Load8a(dst0p_lo, src);
    Face::p64Copy(dst0p_hi, dst0p_lo);

    Face::p64And(dst0p_lo, dst0p_lo, FOG_UINT64_C(0x0000FF000000FF00));
    Face::p64And(dst0p_hi, dst0p_hi, FOG_UINT64_C(0xFF000000FF000000));

    dst0p_20 = static_cast<uint32_t>(_FOG_FACE_COMBINE_2(dst0p_lo >> 24, dst0p_lo >>  8));
    dst0p_31 = static_cast<uint32_t>(_FOG_FACE_COMBINE_2(dst0p_hi >> 56, dst0p_hi >> 24));
#else
    uint32_t dst0p_lo;
    uint32_t dst0p_hi;

    Face::p32Load8a(dst0p_lo, dst0p_hi, src);

    Face::p32And(dst0p_lo, dst0p_lo, 0xFF00FF00);
    Face::p32And(dst0p_hi, dst0p_hi, 0xFF00FF00);

    dst0p_20 = _FOG_FACE_COMBINE_2((dst0p_lo >> 8) & 0xFF, (dst0p_hi <<  8));
    dst0p_31 = _FOG_FACE_COMBINE_2((dst0p_hi >> 8) & 0xFF, (dst0p_lo >> 24));
#endif
  }

  static FOG_INLINE void p32StorePixel32(void* dst, const uint32_t& src0p)
  {
    uint32_t src0p_10, src0p_32;

    Face::p32PRGB64FromPRGB32(src0p_10, src0p_32, src0p);
    Face::p32Store8a(dst, src0p_10, src0p_32);
  }

  static FOG_INLINE void p32StorePixel32_2031(void* dst, const uint32_t& src0p_20, const uint32_t& src0p_31)
  {
#if defined(FOG_ARCH_NATIVE_P64)
    uint32_t t0 = _FOG_FACE_COMBINE_2(src0p_20 & 0x000000FF, src0p_31 << 16);
    uint32_t t1 = _FOG_FACE_COMBINE_2(src0p_20 >> 16, src0p_31 & 0x00FF0000); 

    __p64 dst0p = static_cast<uint64_t>(t0) + (static_cast<uint64_t>(t1) << 32);
    Face::p64Store8a(dst, dst0p);
#else
    uint32_t t0 = src0p_20 * 0x0101;
    uint32_t t1 = src0p_31 * 0x0101;
    uint32_t t2 = _FOG_FACE_COMBINE_2(src0p_20 & 0xFFFF, src0p_31 << 16);
    uint32_t t3 = _FOG_FACE_COMBINE_2(src0p_20 >> 16, src0p_31 & 0xFFFF0000);

    Face::p32Store8a(dst, t2, t3);
#endif
  }

  // ==========================================================================
  // [PixelA8]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixelA8(uint32_t& dst0p, const void* src)
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    Face::p32Load1b(dst0p, static_cast<const uint8_t*>(src) + PIXEL_ARGB64_POS_A * 2 + 1);
#else
    Face::p32Load1b(dst0p, static_cast<const uint8_t*>(src) + PIXEL_ARGB64_POS_A * 2 + 0);
#endif
  }

  static FOG_INLINE void p32StorePixelA8(void* dst, const uint32_t& src0p)
  {
#if defined(FOG_ARCH_NATIVE_P64)
    __p64 a0p;

    Face::p64ExtendPBBFromSBB(a0p, static_cast<uint64_t>(src0p));
    Face::p64Store8a(static_cast<uint8_t*>(dst) + 0, a0p);
#else
    uint32_t a0p;

    Face::p32ExtendPBBFromSBB(a0p, src0p);
    Face::p32Store4a(static_cast<uint8_t*>(dst) + 0, a0p);
    Face::p32Store4a(static_cast<uint8_t*>(dst) + 4, a0p);
#endif
  }
};

// ============================================================================
// [Fog::RasterOps_C - PixelA16]
// ============================================================================

struct FOG_NO_EXPORT PixelA16
{
  enum { SIZE = 1, HAS_RGB = 0, HAS_ALPHA = 1, HAS_X = 0, HAS_F = 0, IS_PREMULTIPLIED = 1 };

  // ==========================================================================
  // [Pixel32]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixel32(uint32_t& dst0p, const void* src)
  {
    p32LoadPixelA8(dst0p, src);
    Face::p32ExtendPBBFromSBB(dst0p, dst0p);
  }

  static FOG_INLINE void p32LoadPixel32_2031(uint32_t& dst0p_20, uint32_t& dst0p_31, const void* src)
  {
    p32LoadPixelA8(dst0p_20, src);

    Face::p32ExtendPBWFromSBW(dst0p_20, dst0p_20);
    Face::p32Copy(dst0p_31, dst0p_20);
  }

  static FOG_INLINE void p32StorePixel32(void* dst, const uint32_t& src0p)
  {
    uint32_t a0p = src0p >> 24;
    Face::p32ExtendPBBFromSBB_ZZ10(a0p, a0p);
    Face::p32Store2a(dst, a0p);
  }

  static FOG_INLINE void p32StorePixel32_2031(void* dst, const uint32_t& src0p_20, const uint32_t& src0p_31)
  {
    uint32_t a0p = src0p_31 >> 16;
    Face::p32ExtendPBBFromSBB_ZZ10(a0p, a0p);
    Face::p32Store2a(dst, a0p);
  }

  // ==========================================================================
  // [PixelA8]
  // ==========================================================================

  static FOG_INLINE void p32LoadPixelA8(uint32_t& dst0p, const void* src)
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    Face::p32Load1b(dst0p, static_cast<const uint8_t*>(src) + 1);
#else
    Face::p32Load1b(dst0p, static_cast<const uint8_t*>(src) + 0);
#endif
  }

  static FOG_INLINE void p32StorePixelA8(void* dst, const uint32_t& src0p)
  {
    uint32_t a0p;

    Face::p32ExtendPBBFromSBB_ZZ10(a0p, src0p);
    Face::p32Store2a(static_cast<uint8_t*>(dst), a0p);
  }
};

// TODO: Classes below should be replaced by PixelXXX classes.

// ============================================================================
// [Fog::RasterOps_C - AccessPRGB32]
// ============================================================================

struct AccessPRGB32
{
  enum { SIZE = 4, HAS_RGB = 1, HAS_ALPHA = 1, HAS_X = 0, IS_PREMULTIPLIED = 1 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load4a(dst0, srcp);
  }

  static FOG_INLINE void p32LoadXRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load4a(dst0, srcp);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const uint32_t& src0)
  {
    Face::p32Store4a(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const uint32_t& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const uint32_t& src0)
  {
    uint32_t t0;
    Face::p32FillPBB3(t0, src0);
    Face::p32Store4a(dstp, t0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  // TODO: 64-bit image processing.
};

// ============================================================================
// [Fog::RasterOps_C - AccessXRGB32]
// ============================================================================

struct AccessXRGB32
{
  enum { SIZE = 4, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 1, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load4a(dst0, srcp);
    Face::p32FillPBB3(dst0, dst0);
  }

  static FOG_INLINE void p32LoadXRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load4a(dst0, srcp);
  }

  static FOG_INLINE void p32LoadZRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load4a(dst0, srcp);
    Face::p32ZeroPBB3(dst0, dst0);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const uint32_t& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const uint32_t& src0)
  {
    Face::p32Store4a(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const uint32_t& src0)
  {
    uint32_t t0;
    Face::p32FillPBB3(t0, src0);
    Face::p32Store4a(dstp, t0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  // TODO: 64-bit image processing.
};

// ============================================================================
// [Fog::RasterOps_C - AccessRGB24]
// ============================================================================

struct AccessRGB24
{
  enum { SIZE = 3, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load3b(dst0, srcp);
    Face::p32FillPBB3(dst0, dst0);
  }

  static FOG_INLINE void p32LoadXRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load3b(dst0, srcp);
  }

  static FOG_INLINE void p32LoadZRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load3b(dst0, srcp);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const uint32_t& src0)
  {
    Face::p32Store3b(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const uint32_t& src0)
  {
    Face::p32Store3b(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const uint32_t& src0)
  {
    Face::p32Store3b(dstp, src0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  // TODO: 64-bit image processing.
};

// ============================================================================
// [Fog::RasterOps_C - AccessRGB16_555]
// ============================================================================

struct AccessRGB16_555
{
  enum { SIZE = 2, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load2a(dst0, srcp);
    Face::p32FRGB32FromRGB16_555(dst0, dst0);
  }

  static FOG_INLINE void p32LoadXRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load2a(dst0, srcp);
    Face::p32ZRGB32FromRGB16_555(dst0, dst0);
  }

  static FOG_INLINE void p32LoadZRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load2a(dst0, srcp);
    Face::p32ZRGB32FromRGB16_555(dst0, dst0);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const uint32_t& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const uint32_t& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const uint32_t& src0)
  {
    uint32_t t0;
    Face::p32RGB16_555FromXRGB32(t0, src0);
    Face::p32Store2a(dstp, t0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  // TODO: 64-bit image processing.
};

// ============================================================================
// [Fog::RasterOps_C - AccessRGB16_565]
// ============================================================================

struct AccessRGB16_565
{
  enum { SIZE = 2, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load2a(dst0, srcp);
    Face::p32FRGB32FromRGB16_565(dst0, dst0);
  }

  static FOG_INLINE void p32LoadXRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load2a(dst0, srcp);
    Face::p32ZRGB32FromRGB16_565(dst0, dst0);
  }

  static FOG_INLINE void p32LoadZRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32Load2a(dst0, srcp);
    Face::p32ZRGB32FromRGB16_565(dst0, dst0);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const uint32_t& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const uint32_t& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const uint32_t& src0)
  {
    uint32_t t0;
    Face::p32RGB16_565FromXRGB32(t0, src0);
    Face::p32Store2a(dstp, t0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  // TODO: 64-bit image processing.
};

// ============================================================================
// [Fog::RasterOps_C - AccessPRGB64]
// ============================================================================

struct AccessPRGB64
{
  enum { SIZE = 8, HAS_RGB = 1, HAS_ALPHA = 1, HAS_X = 0, IS_PREMULTIPLIED = 1 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(uint32_t& dst0, const void* srcp)
  {
    uint32_t src0p_10, src0p_32;

    Face::p32Load8a(src0p_10, src0p_32, srcp);
    Face::p32ARGB32FromARGB64_1032(dst0, src0p_10, src0p_32);
  }

  static FOG_INLINE void p32LoadXRGB32(uint32_t& dst0, const void* srcp)
  {
    uint32_t src0p_10, src0p_32;

    Face::p32Load8a(src0p_10, src0p_32, srcp);
    Face::p32ZRGB32FromARGB64_1032(dst0, src0p_10, src0p_32);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const uint32_t& src0)
  {
    uint32_t src0p_10, src0p_32;

    Face::p32PRGB64FromPRGB32(src0p_10, src0p_32, src0);
    Face::p32Store8a(dstp, src0p_10, src0p_32);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const uint32_t& src0)
  {
    uint32_t src0p_10, src0p_32;

    Face::p32FRGB64FromXRGB32(src0p_10, src0p_32, src0);
    Face::p32Store8a(dstp, src0p_10, src0p_32);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const uint32_t& src0)
  {
    uint32_t src0p_10, src0p_32;

    Face::p32FRGB64FromZRGB32(src0p_10, src0p_32, src0);
    Face::p32Store8a(dstp, src0p_10, src0p_32);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB64(uint32_t& dst0_10, uint32_t& dst0_32, const void* srcp)
  {
    Face::p32Load8a(dst0_10, dst0_32, srcp);
  }

  static FOG_INLINE void p32LoadXRGB64(uint32_t& dst0_10, uint32_t& dst0_32, const void* srcp)
  {
    Face::p32Load8a(dst0_10, dst0_32, srcp);
  }

  static FOG_INLINE void p32StorePRGB64(void* dstp, const uint32_t& src0_10, const uint32_t& src0_32)
  {
    Face::p32Store8a(dstp, src0_10, src0_32);
  }

  static FOG_INLINE void p32StoreXRGB64(void* dstp, const uint32_t& src0_10, const uint32_t& src0_32)
  {
    uint32_t src0_F2;

    Face::p32FillPWW1(src0_F2, src0_32);
    Face::p32Store8a(dstp, src0_10, src0_F2);
  }
};

// ============================================================================
// [Fog::RasterOps_C - AccessRGB48]
// ============================================================================

struct AccessRGB48
{
  enum { SIZE = 6, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32RGB48LoadToFRGB32(dst0, srcp);
  }

  static FOG_INLINE void p32LoadXRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32RGB48LoadToZRGB32(dst0, srcp);
  }

  static FOG_INLINE void p32LoadZRGB32(uint32_t& dst0, const void* srcp)
  {
    Face::p32RGB48LoadToZRGB32(dst0, srcp);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const uint32_t& src0)
  {
    Face::p32RGB48StoreFromXRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const uint32_t& src0)
  {
    Face::p32RGB48StoreFromXRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const uint32_t& src0)
  {
    Face::p32RGB48StoreFromXRGB32(dstp, src0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB64(uint32_t& dst0_10, uint32_t& dst0_32, const void* srcp)
  {
    uint32_t src0_r, src0_g, src0_b;

    Face::p32RGB48Load(src0_r, src0_g, src0_b, srcp);
    dst0_10 = _FOG_FACE_COMBINE_2(src0_b, src0_g << 16);
    dst0_32 = _FOG_FACE_COMBINE_2(src0_r, 0xFFFF0000);
  }

  static FOG_INLINE void p32LoadXRGB64(uint32_t& dst0_10, uint32_t& dst0_32, const void* srcp)
  {
    uint32_t src0_r, src0_g, src0_b;

    Face::p32RGB48Load(src0_r, src0_g, src0_b, srcp);
    dst0_10 = _FOG_FACE_COMBINE_2(src0_b, src0_g << 16);
    dst0_32 = src0_r;
  }

  static FOG_INLINE void p32StorePRGB64(void* dstp, const uint32_t& src0_10, const uint32_t& src0_32)
  {
    // TODO: 64-bit image processing.
  }

  static FOG_INLINE void p32StoreXRGB64(void* dstp, const uint32_t& src0_10, const uint32_t& src0_32)
  {
    // TODO: 64-bit image processing.
  }
};

} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_BASEACCESS_P_H
