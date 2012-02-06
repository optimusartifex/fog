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
