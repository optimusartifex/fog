// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDER_C_ACCESS_P_H
#define _FOG_G2D_RENDER_RENDER_C_ACCESS_P_H

// [Dependencies]
#include <Fog/Core/Face/Face_C_P32.h>

namespace Fog {
namespace Render_C {

// ============================================================================
// [Fog::Render_C - Access - PRGB32_Native
// ============================================================================

struct Access_PRGB32_Native
{
  enum { SIZE = 4, HAS_RGB = 1, HAS_ALPHA = 1, HAS_X = 0, IS_PREMULTIPLIED = 1 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load4aNative(dst0, srcp);
  }

  static FOG_INLINE void p32LoadXRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load4aNative(dst0, srcp);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32Store4aNative(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const Face::p32& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32 t0;
    Face::p32FillPBB3(t0, src0);
    Face::p32Store4aNative(dstp, t0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------
};



// ============================================================================
// [Fog::Render_C - Access - XRGB32_Native
// ============================================================================

struct Access_XRGB32_Native
{
  enum { SIZE = 4, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 1, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load4aNative(dst0, srcp);
    Face::p32FillPBB3(dst0, dst0);
  }

  static FOG_INLINE void p32LoadXRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load4aNative(dst0, srcp);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const Face::p32& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32Store4aNative(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32 t0;
    Face::p32FillPBB3(t0, src0);
    Face::p32Store4aNative(dstp, t0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------
};



// ============================================================================
// [Fog::Render_C - Access - RGB24_Native
// ============================================================================

struct Access_RGB24_Native
{
  enum { SIZE = 3, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load3bNative(dst0, srcp);
    Face::p32FillPBB3(dst0, dst0);
  }

  static FOG_INLINE void p32LoadXRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load3bNative(dst0, srcp);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32Store3bNative(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32Store3bNative(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32Store3bNative(dstp, src0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------
};



// ============================================================================
// [Fog::Render_C - Access - RGB16_555_Native
// ============================================================================

struct Access_RGB16_555_Native
{
  enum { SIZE = 2, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load2aNative(dst0, srcp);
    Face::p32FRGB32FromRGB16_555(dst0, dst0);
  }

  static FOG_INLINE void p32LoadXRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load2aNative(dst0, srcp);
    Face::p32ZRGB32FromRGB16_555(dst0, dst0);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const Face::p32& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const Face::p32& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32 t0;
    Face::p32RGB16_555FromXRGB32(t0, src0);
    Face::p32Store2aNative(dstp, t0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------
};



// ============================================================================
// [Fog::Render_C - Access - RGB16_565_Native
// ============================================================================

struct Access_RGB16_565_Native
{
  enum { SIZE = 2, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load2aNative(dst0, srcp);
    Face::p32FRGB32FromRGB16_565(dst0, dst0);
  }

  static FOG_INLINE void p32LoadXRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32Load2aNative(dst0, srcp);
    Face::p32ZRGB32FromRGB16_565(dst0, dst0);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const Face::p32& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const Face::p32& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32 t0;
    Face::p32RGB16_565FromXRGB32(t0, src0);
    Face::p32Store2aNative(dstp, t0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------
};



// ============================================================================
// [Fog::Render_C - Access - PRGB64_Native
// ============================================================================

struct Access_PRGB64_Native
{
  enum { SIZE = 8, HAS_RGB = 1, HAS_ALPHA = 1, HAS_X = 0, IS_PREMULTIPLIED = 1 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadARGB32(Face::p32& dst0, const void* srcp)
  {
    // TODO:
  }

  static FOG_INLINE void p32LoadPRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32 src0p_10, src0p_32;

    Face::p32Load8aNative(src0p_10, src0p_32, srcp);
    Face::p32ARGB32FromARGB64_1032(dst0, src0p_10, src0p_32);
  }

  static FOG_INLINE void p32LoadXRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32 src0p_10, src0p_32;

    Face::p32Load8aNative(src0p_10, src0p_32, srcp);
    Face::p32ZRGB32FromARGB64_1032(dst0, src0p_10, src0p_32);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const Face::p32& src0)
  {
    // TODO
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const Face::p32& src0)
  {
    p32StoreZRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const Face::p32& src0)
  {
    // TODO
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadPRGB64(Face::p32& dst0_10, Face::p32& dst0_32, const void* srcp)
  {
    Face::p32Load8aNative(dst0_10, dst0_32, srcp);
  }

  static FOG_INLINE void p32LoadXRGB64(Face::p32& dst0_10, Face::p32& dst0_32, const void* srcp)
  {
    Face::p32Load8aNative(dst0_10, dst0_32, srcp);
  }

  static FOG_INLINE void p32StorePRGB64(void* dstp, const Face::p32& src0_10, const Face::p32& src0_32)
  {
    Face::p32Store8aNative(dstp, src0_10, src0_32);
  }

  static FOG_INLINE void p32StoreXRGB64(void* dstp, const Face::p32& src0_10, const Face::p32& src0_32)
  {
    Face::p32 src0_F2;

    Face::p32FillPWW1(src0_F2, src0_32);
    Face::p32Store8aNative(dstp, src0_10, src0_F2);
  }
};



// ============================================================================
// [Fog::Render_C - Access - RGB48_Native
// ============================================================================

struct Access_RGB48_Native
{
  enum { SIZE = 6, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, IS_PREMULTIPLIED = 0 };

  // --------------------------------------------------------------------------
  // [32-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadARGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32RGB48LoadToFRGB32(dst0, srcp);
  }

  static FOG_INLINE void p32LoadPRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32RGB48LoadToFRGB32(dst0, srcp);
  }

  static FOG_INLINE void p32LoadXRGB32(Face::p32& dst0, const void* srcp)
  {
    Face::p32RGB48LoadToZRGB32(dst0, srcp);
  }

  static FOG_INLINE void p32StoreARGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32RGB48StoreFromARGB32(dstp, src0);
  }

  static FOG_INLINE void p32StorePRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32RGB48StoreFromXRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreXRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32RGB48StoreFromXRGB32(dstp, src0);
  }

  static FOG_INLINE void p32StoreZRGB32(void* dstp, const Face::p32& src0)
  {
    Face::p32RGB48StoreFromXRGB32(dstp, src0);
  }

  // --------------------------------------------------------------------------
  // [64-Bits Per Pixel]
  // --------------------------------------------------------------------------

  static FOG_INLINE void p32LoadARGB64(Face::p32& dst0_10, Face::p32& dst0_32, const void* srcp)
  {
    Face::p32 src0_r, src0_g, src0_b;

    Face::p32RGB48Load(src0_r, src0_g, src0_b, srcp);
    dst0_10 = _FOG_FACE_COMBINE_2(src0_b, src0_g << 16);
    dst0_32 = _FOG_FACE_COMBINE_2(src0_r, 0xFFFF0000);
  }

  static FOG_INLINE void p32LoadPRGB64(Face::p32& dst0_10, Face::p32& dst0_32, const void* srcp)
  {
    Face::p32 src0_r, src0_g, src0_b;

    Face::p32RGB48Load(src0_r, src0_g, src0_b, srcp);
    dst0_10 = _FOG_FACE_COMBINE_2(src0_b, src0_g << 16);
    dst0_32 = _FOG_FACE_COMBINE_2(src0_r, 0xFFFF0000);
  }

  static FOG_INLINE void p32LoadXRGB64(Face::p32& dst0_10, Face::p32& dst0_32, const void* srcp)
  {
    Face::p32 src0_r, src0_g, src0_b;

    Face::p32RGB48Load(src0_r, src0_g, src0_b, srcp);
    dst0_10 = _FOG_FACE_COMBINE_2(src0_b, src0_g << 16);
    dst0_32 = src0_r;
  }

  static FOG_INLINE void p32StoreARGB64(void* dstp, const Face::p32& src0_10, const Face::p32& src0_32)
  {
    // TODO
  }

  static FOG_INLINE void p32StorePRGB64(void* dstp, const Face::p32& src0_10, const Face::p32& src0_32)
  {
    // TODO
  }

  static FOG_INLINE void p32StoreXRGB64(void* dstp, const Face::p32& src0_10, const Face::p32& src0_32)
  {
    // TODO
  }
};

} // Render_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_ACCESS_P_H
