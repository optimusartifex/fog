// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITEEXT_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITEEXT_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/CompositeBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - PixelPRGB32]
// ============================================================================

struct FOG_NO_EXPORT PixelPRGB32
{
  enum { SIZE = 4, HAS_RGB = 1, HAS_ALPHA = 1, HAS_X = 0, HAS_F = 0, IS_PREMULTIPLIED = 1 };

  // --------------------------------------------------------------------------
  // [Pixel32]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [PIxelA8]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [Pixel32]
  // --------------------------------------------------------------------------

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
};

// ============================================================================
// [Fog::RasterOps_C - PixelFRGB32]
// ============================================================================

struct FOG_NO_EXPORT PixelFRGB32
{
  enum { SIZE = 4, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, HAS_F = 1, IS_PREMULTIPLIED = 1 };

  // --------------------------------------------------------------------------
  // [Pixel32]
  // --------------------------------------------------------------------------

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
};

// ============================================================================
// [Fog::RasterOps_C - PixelRGB24]
// ============================================================================

struct FOG_NO_EXPORT PixelRGB24
{
  enum { SIZE = 3, HAS_RGB = 1, HAS_ALPHA = 0, HAS_X = 0, HAS_F = 0, IS_PREMULTIPLIED = 1 };

  // --------------------------------------------------------------------------
  // [Pixel32]
  // --------------------------------------------------------------------------

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
};

// ============================================================================
// [Fog::RasterOps_C - PixelA8]
// ============================================================================

struct FOG_NO_EXPORT PixelA8
{
  enum { SIZE = 1, HAS_RGB = 0, HAS_ALPHA = 1, HAS_X = 0, HAS_F = 0, IS_PREMULTIPLIED = 1 };

  // --------------------------------------------------------------------------
  // [Pixel32]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [PixelA8]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [Pixel32]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [PixelA8]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [Pixel32]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [PixelA8]
  // --------------------------------------------------------------------------

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

// ============================================================================
// [Fog::RasterOps_C - CompositeCondition]
// ============================================================================

template<uint32_t CombineFlags>
struct CompositeCondition
{
  // --------------------------------------------------------------------------
  // [Pixel32]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool p32ProcessDstPixel32(const uint32_t& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_DA_ZERO)
      return !Face::p32PRGB32IsAlpha00(c0);
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_DA_FULL)
      return !Face::p32PRGB32IsAlphaFF(c0);
    else
      return true;
  }

  static FOG_INLINE bool p32ProcessSrcPixel32(const uint32_t& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO)
      return !Face::p32PRGB32IsAlpha00(c0);
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL)
      return !Face::p32PRGB32IsAlphaFF(c0);
    else
      return true;
  }

  // --------------------------------------------------------------------------
  // [PixelA8]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool p32ProcessSrcPixelA8(const uint32_t& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO)
      return c0 == 0x00;
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL)
      return c0 == 0xFF;
    else
      return true;
  }

  // --------------------------------------------------------------------------
  // [Pixel64]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool p64ProcessDstPixel64(const __p64& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_DA_ZERO)
      return !Face::p64PRGB64IsAlpha0000(c0);
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_DA_FULL)
      return !Face::p64PRGB64IsAlphaFFFF(c0);
    else
      return true;
  }

  static FOG_INLINE bool p64ProcessSrcPixel64(const __p64& c0)
  {
    if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO)
      return !Face::p64PRGB64IsAlpha0000(c0);
    else if (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL)
      return !Face::p64PRGB64IsAlphaFFFF(c0);
    else
      return true;
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeGeneric]
// ============================================================================

template<typename CompositeOp, uint32_t CombineFlags, uint32_t PrepareFlags>
struct CompositeGeneric
{
  typedef CompositeCondition<CombineFlags> Cond;

  // ==========================================================================
  // [Defs]
  // ==========================================================================

  enum
  {
    NopIfDaZero    = (CombineFlags & RASTER_COMBINE_NOP_IF_DA_ZERO) != 0,
    NopIfDaFull    = (CombineFlags & RASTER_COMBINE_NOP_IF_DA_FULL) != 0,
    NopIfSaZero    = (CombineFlags & RASTER_COMBINE_NOP_IF_SA_ZERO) != 0,
    NopIfSaFull    = (CombineFlags & RASTER_COMBINE_NOP_IF_SA_FULL) != 0,

    IsUnbound      = (CombineFlags & RASTER_COMBINE_UNBOUND       ) != 0,
    IsUnboundMskIn = (CombineFlags & RASTER_COMBINE_UNBOUND_MSK_IN) != 0,

    PrepareNeeded = (PrepareFlags != RASTER_PRGB_PREPARE_NONE),
    PrepareToFRGB = (PrepareFlags == RASTER_PRGB_PREPARE_FRGB),
    PrepareToZRGB = (PrepareFlags == RASTER_PRGB_PREPARE_ZRGB)
  };

  // ==========================================================================
  // [Helpers - Prepare]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_prepare_pixel32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
    if (DstF::HAS_ALPHA && !SrcF::HAS_ALPHA && SrcF::HAS_X && PrepareNeeded)
    {
      CompositeOp::prgb32_prepare_xrgb32(dst0p, src0p);
    }
  }

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_prepare_pixel32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
    if (DstF::HAS_ALPHA && !SrcF::HAS_ALPHA && SrcF::HAS_X && PrepareNeeded)
    {
      CompositeOp::prgb32_prepare_xrgb32_2031(dst0p_20, src0p_20, dst0p_31, src0p_31);
    }
  }

  // ==========================================================================
  // [Helpers - Op]
  // ==========================================================================

  template<typename DstF, typename SrcF, bool Pack>
  static FOG_INLINE void pixel32_op_pixel32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31)
  {
    if (DstF::HAS_ALPHA)
    {
      if (SrcF::HAS_ALPHA)
        CompositeOp::prgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else if (PrepareToFRGB ||  SrcF::HAS_F)
        CompositeOp::prgb32_op_frgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else if (PrepareToZRGB || !SrcF::HAS_X)
        CompositeOp::prgb32_op_zrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else
        CompositeOp::prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
    }
    else
    {
      if (SrcF::HAS_ALPHA)
        CompositeOp::xrgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else if (DstF::HAS_X || SrcF::HAS_X)
        CompositeOp::xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
      else
        CompositeOp::zrgb32_op_zrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, Pack);
    }
  }

  template<typename DstF, bool Pack>
  static FOG_INLINE void pixel32_op_pixela8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8)
  {
    if (DstF::HAS_ALPHA)
    {
      CompositeOp::prgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, Pack);
    }
    else
    {
      CompositeOp::xrgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8, Pack);
    }
  }

  // ==========================================================================
  // [Pixel32 - CBlit - Pixel32 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_cblit_pixel32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_DstFx1_INIT()

    uint32_t sro0p, sro0p_20, sro0p_31;

    Face::p32Copy(sro0p, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    BLIT_LOOP_DstFx1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;

      DstF::p32LoadPixel32(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
        goto _C_Opaque_Skip;
      
      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

      pixel32_op_pixel32_2031<DstF, SrcF, true>(
        dst0p_20, dst0p_20, sro0p_20,
        dst0p_31, dst0p_31, sro0p_31);
      DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
      dst += DstF::SIZE;
    BLIT_LOOP_DstFx1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - CBlit - Pixel32 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_cblit_pixel32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    uint32_t sro0p, sro0p_20, sro0p_31;

    Face::p32Copy(sro0p, src->prgb32.u32);
    Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

    C_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t dst0p_20, dst0p_31;

        DstF::p32LoadPixel32(dst0p_20, dst);
        if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
          goto _C_Opaque_Skip;
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        pixel32_op_pixel32_2031<DstF, SrcF, true>(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
        dst += DstF::SIZE;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      uint32_t minv0p;

      Face::p32Copy(msk0p, msk0);
      Face::p32Negate256SBW(minv0p, msk0p);

      if (IsUnbound && IsUnboundMskIn)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t src0p_20, src0p_31;
        Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0, src0p_31, sro0p_31, msk0);

        BLIT_LOOP_32x1_BEGIN(C_Mask_UnboundIn)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_UnboundIn)
      }
      else if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t src0p_20, src0p_31;
        Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0, src0p_31, sro0p_31, msk0);

        BLIT_LOOP_32x1_BEGIN(C_Mask)
          uint32_t dst0p_20, dst0p_31;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Skip:
          dst += DstF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------
  
    C_BLIT_SPAN8_A8_GLYPH()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Unbound_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          if (msk0p != 0xFF)
            goto _A8_Glyph_Unbound_Mask;

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Unbound_Skip:
          dst += DstF::SIZE;
          msk += 1;
          continue;

_A8_Glyph_Unbound_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t msk0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Glyph_Skip;

          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0p, src0p_31, sro0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Skip:
          dst += DstF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_A8_EXTRA()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32Load2a(msk0p, msk);

          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Unbound_Skip;

          Face::p32Negate256SBW(minv0p, msk0p);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, sro0p_20,
            dst0p_31, dst0p_31, sro0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Unbound_Skip:
          dst += DstF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t msk0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          Face::p32Load2a(msk0p, msk);

          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, sro0p_20, msk0p, src0p_31, sro0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Skip:
          dst += DstF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra)
      }
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    C_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p_20, dst0p_31;
        uint32_t dinv0p;
        uint32_t msk0p_20, msk0p_31;

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000)
          goto _ARGB32_Skip;

        DstF::p32LoadPixel32(dst0p_20, dst);
        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        pixel32_op_pixel32_2031<DstF, SrcF, true>(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_ARGB32_Skip:
        dst += DstF::SIZE;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32Negate255PBB(msk0p_20, msk0p_20);
        Face::p32UnpackPBWFromPBB_2031(msk0p_20, msk0p_31, msk0p_20);

        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        pixel32_op_pixel32_2031<DstF, SrcF, false>(
          dst0p_20, dst0p_20, sro0p_20,
          dst0p_31, dst0p_31, sro0p_31);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        Face::p32Add(dst0p_20, dst0p_20, dinv0p);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    C_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [Pixel32 - VBlit - Pixel32 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixel32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;
      uint32_t src0p_20, src0p_31;

      DstF::p32LoadPixel32(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
        goto _C_Opaque_Skip;
      SrcF::p32LoadPixel32(src0p_20, src);

      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
      Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

      pixel32_op_pixel32_2031<DstF, SrcF, true>(
        dst0p_20, dst0p_20, src0p_20,
        dst0p_31, dst0p_31, src0p_31);
      DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
      dst += DstF::SIZE;
      src += SrcF::SIZE;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - VBlit - Pixel32 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixel32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_20, src0p_31;

        DstF::p32LoadPixel32(dst0p_20, dst);
        if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
          goto _C_Opaque_Skip;

        SrcF::p32LoadPixel32(src0p_20, src);
        if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
          goto _C_Opaque_Skip;

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

        pixel32_op_pixel32_2031<DstF, SrcF, true>(
          dst0p_20, dst0p_20, src0p_20,
          dst0p_31, dst0p_31, src0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      Face::p32Copy(msk0p, msk0);

      if (IsUnbound && IsUnboundMskIn)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t minv0p;
        Face::p32Negate256SBW(minv0p, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound_MskIn)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Unbound_MskIn_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _C_Mask_Unbound_MskIn_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, src0p_20, msk0p, src0p_31, src0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Unbound_MskIn_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound_MskIn)
      }
      else if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t minv0p;
        Face::p32Negate256SBW(minv0p, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Unbound_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _C_Mask_Unbound_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);

          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(C_Mask)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _C_Mask_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

          Face::p32MulDiv256PBW_SBW_2x(src0p_20, src0p_20, msk0p, src0p_31, src0p_31, msk0p);
          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------
  
    V_BLIT_SPAN8_A8_GLYPH()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;

          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Unbound_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixel32(src0p_20, src);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

          if (msk0p != 0xFF)
            goto _A8_Glyph_Unbound_Mask;

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
          continue;

_A8_Glyph_Unbound_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t msk0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixel32(src0p_20, src);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

          if (msk0p != 0xFF)
            goto _A8_Glyph_Mask;

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
          continue;

_A8_Glyph_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, src0p_20, msk0p, src0p_31, src0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;

          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Unbound_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _A8_Extra_Unbound_Skip;

          Face::p32Load2a(msk0p, msk);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixel32_2031<DstF, SrcF, false>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_20, src0p_31;
          uint32_t msk0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Skip;

          SrcF::p32LoadPixel32(src0p_20, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixel32(src0p_20))
            goto _A8_Extra_Skip;

          Face::p32Load2a(msk0p, msk);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);
          Face::p32MulDiv256PBW_SBW_2x(src0p_20, src0p_20, msk0p, src0p_31, src0p_31, msk0p);

          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, src0p_20,
            dst0p_31, dst0p_31, src0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra)
      }
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_20, src0p_31;

        uint32_t dinv0p;
        uint32_t msk0p_20, msk0p_31;

        Face::p32Load4a(msk0p_20, msk);
        if (msk0p_20 == 0x00000000)
          goto _ARGB32_Skip;

        DstF::p32LoadPixel32(dst0p_20, dst);
        SrcF::p32LoadPixel32(src0p_20, src);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        Face::p32UnpackPBWFromPBB_2031(src0p_20, src0p_31, src0p_20);

        if (msk0p_20 != 0xFFFFFFFF)
          goto _ARGB32_Mask;

        pixel32_op_pixel32_2031<DstF, SrcF, true>(
          dst0p_20, dst0p_20, src0p_20,
          dst0p_31, dst0p_31, src0p_31);
        DstF::p32StorePixel32(dst, dst0p_20);

_ARGB32_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
        continue;

_ARGB32_Mask:
        Face::p32Negate255PBB(msk0p_20, msk0p_20);
        Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        pixel32_op_pixel32_2031<DstF, SrcF, false>(
          dst0p_20, dst0p_20, src0p_20,
          dst0p_31, dst0p_31, src0p_31);

        Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
        Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

        Face::p32Add(dst0p_20, dst0p_20, dinv0p);
        DstF::p32StorePixel32(dst, dst0p_20);

        dst += DstF::SIZE;
        src += SrcF::SIZE;
        msk += 4;
      BLIT_LOOP_32x1_END(ARGB32_Glyph)
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [Pixel32 - VBlit - PixelA8 - Line]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixela8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    BLIT_LOOP_32x1_INIT()

    BLIT_LOOP_32x1_BEGIN(C_Opaque)
      uint32_t dst0p_20, dst0p_31;
      uint32_t src0p_a8;

      DstF::p32LoadPixel32(dst0p_20, dst);
      if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
        goto _C_Opaque_Skip;
      SrcF::p32LoadPixelA8(src0p_a8, src);

      Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
      pixel32_op_pixela8_2031<DstF, true>(
        dst0p_20, dst0p_20,
        dst0p_31, dst0p_31, src0p_a8);
      DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
      dst += DstF::SIZE;
      src += SrcF::SIZE;
    BLIT_LOOP_32x1_END(C_Opaque)
  }

  // ==========================================================================
  // [Pixel32 - VBlit - PixelA8 - Span]
  // ==========================================================================

  template<typename DstF, typename SrcF>
  static FOG_INLINE void pixel32_vblit_pixela8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    V_BLIT_SPAN8_BEGIN(DstF::SIZE)

    // ------------------------------------------------------------------------
    // [C-Opaque]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_OPAQUE()
    {
      BLIT_LOOP_32x1_INIT()

      BLIT_LOOP_32x1_BEGIN(C_Opaque)
        uint32_t dst0p_20, dst0p_31;
        uint32_t src0p_a8;

        DstF::p32LoadPixel32(dst0p_20, dst);
        if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
          goto _C_Opaque_Skip;
        SrcF::p32LoadPixelA8(src0p_a8, src);

        Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
        pixel32_op_pixela8_2031<DstF, true>(
          dst0p_20, dst0p_20,
          dst0p_31, dst0p_31, src0p_a8);
        DstF::p32StorePixel32(dst, dst0p_20);

_C_Opaque_Skip:
        dst += DstF::SIZE;
        src += SrcF::SIZE;
      BLIT_LOOP_32x1_END(C_Opaque)
    }

    // ------------------------------------------------------------------------
    // [C-Mask]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_C_MASK()
    {
      BLIT_LOOP_32x1_INIT()

      uint32_t msk0p;
      Face::p32Copy(msk0p, msk0);

      if (IsUnbound && IsUnboundMskIn)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t minv0p;
        Face::p32Negate256SBW(minv0p, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound_MskIn)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Unbound_MskIn_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _C_Mask_Unbound_MskIn_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          Face::p32MulDiv256SBW(src0p_a8, src0p_a8, msk0p);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Unbound_MskIn_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound_MskIn)
      }
      else if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        uint32_t minv0p;
        Face::p32Negate256SBW(minv0p, msk0p);

        BLIT_LOOP_32x1_BEGIN(C_Mask_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;
          uint32_t dinv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Unbound_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _C_Mask_Unbound_MskIn_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256PBW_SBW_2x_Pack_1032(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(C_Mask)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _C_Mask_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _C_Mask_Unbound_MskIn_Skip;

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256SBW(src0p_a8, src0p_a8, msk0p);
          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_C_Mask_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
        BLIT_LOOP_32x1_END(C_Mask)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Glyph]
    // ------------------------------------------------------------------------
  
    V_BLIT_SPAN8_A8_GLYPH()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;

          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Unbound_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixelA8(src0p_a8, src);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          if (msk0p != 0xFF)
            goto _A8_Glyph_Unbound_Mask;

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
          continue;

_A8_Glyph_Unbound_Mask:
          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Glyph)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;
          uint32_t msk0p;

          Face::p32Load1b(msk0p, msk);
          if (msk0p == 0x00)
            goto _A8_Glyph_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixelA8(src0p_a8, src);

          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32MulDiv256SBW(src0p_a8, src0p_a8,  msk0p);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Glyph_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 1;
        BLIT_LOOP_32x1_END(A8_Glyph)
      }
    }

    // ------------------------------------------------------------------------
    // [A8-Extra]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_A8_EXTRA()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;

          uint32_t dinv0p;
          uint32_t msk0p;
          uint32_t minv0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Unbound_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _A8_Extra_Unbound_Skip;

          Face::p32Load2a(msk0p, msk);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32Negate256SBW(minv0p, msk0p);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dinv0p, dst0p_20, minv0p, dst0p_31, minv0p);
          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);

          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p, dst0p_31, msk0p);
          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(A8_Extra)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;
          uint32_t msk0p;

          DstF::p32LoadPixel32(dst0p_20, dst);
          if ((NopIfDaZero || NopIfDaFull) && !Cond::p32ProcessDstPixel32(dst0p_20))
            goto _A8_Extra_Skip;

          SrcF::p32LoadPixelA8(src0p_a8, src);
          if ((NopIfSaZero || NopIfSaFull) && !Cond::p32ProcessSrcPixelA8(src0p_a8))
            goto _A8_Extra_Skip;

          Face::p32Load2a(msk0p, msk);

          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);
          Face::p32MulDiv256SBW(src0p_a8, src0p_a8, msk0p);

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_A8_Extra_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 2;
        BLIT_LOOP_32x1_END(A8_Extra)
      }
    }

    // ------------------------------------------------------------------------
    // [ARGB32-Glyph]
    // ------------------------------------------------------------------------

    V_BLIT_SPAN8_ARGB32_GLYPH()
    {
      if (IsUnbound)
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph_Unbound)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;

          uint32_t dinv0p;
          uint32_t msk0p_20, msk0p_31;

          Face::p32Load4a(msk0p_20, msk);
          if (msk0p_20 == 0x00000000)
            goto _ARGB32_Unbound_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixelA8(src0p_a8, src);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          if (msk0p_20 != 0xFFFFFFFF)
            goto _ARGB32_Unbound_Mask;

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_ARGB32_Unbound_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 4;
          continue;

_ARGB32_Unbound_Mask:
          Face::p32Negate255PBB(msk0p_20, msk0p_20);
          Face::p32Cvt256PBWFrom255PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
          Face::p32MulDiv256PBW_2x_Pack_2031(dinv0p, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

          pixel32_op_pixela8_2031<DstF, false>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);

          Face::p32Negate256PBW_2x(msk0p_20, msk0p_20, msk0p_31, msk0p_31);
          Face::p32MulDiv256PBW_2x_Pack_2031(dst0p_20, dst0p_20, msk0p_20, dst0p_31, msk0p_31);

          Face::p32Add(dst0p_20, dst0p_20, dinv0p);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 4;
        BLIT_LOOP_32x1_END(ARGB32_Glyph_Unbound)
      }
      else
      {
        BLIT_LOOP_32x1_INIT()

        BLIT_LOOP_32x1_BEGIN(ARGB32_Glyph)
          uint32_t dst0p_20, dst0p_31;
          uint32_t src0p_a8;

          uint32_t dinv0p;
          uint32_t msk0p_20, msk0p_31;

          Face::p32Load4a(msk0p_20, msk);
          if (msk0p_20 == 0x00000000)
            goto _ARGB32_Skip;

          DstF::p32LoadPixel32(dst0p_20, dst);
          SrcF::p32LoadPixelA8(src0p_a8, src);
          Face::p32UnpackPBWFromPBB_2031(dst0p_20, dst0p_31, dst0p_20);

          if (msk0p_20 != 0xFFFFFFFF)
            goto _ARGB32_Mask;

          pixel32_op_pixela8_2031<DstF, true>(
            dst0p_20, dst0p_20,
            dst0p_31, dst0p_31, src0p_a8);
          DstF::p32StorePixel32(dst, dst0p_20);

_ARGB32_Skip:
          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 4;
          continue;

_ARGB32_Mask:
          Face::p32MulDiv256PBW_2x(msk0p_20, msk0p_20, src0p_a8, msk0p_31, msk0p_31, src0p_a8);
          pixel32_op_pixel32_2031<DstF, SrcF, true>(
            dst0p_20, dst0p_20, msk0p_20,
            dst0p_31, dst0p_31, msk0p_31);
          DstF::p32StorePixel32(dst, dst0p_20);

          dst += DstF::SIZE;
          src += SrcF::SIZE;
          msk += 4;
        BLIT_LOOP_32x1_END(ARGB32_Glyph)
      }
    }

    V_BLIT_SPAN8_END()
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_line<PixelPRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_span<PixelPRGB32, PixelPRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_xrgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_line<PixelPRGB32, PixelFRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_cblit_xrgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_span<PixelPRGB32, PixelFRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_line<PixelPRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_span<PixelPRGB32, PixelPRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_xrgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_line<PixelPRGB32, PixelXRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_xrgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_span<PixelPRGB32, PixelXRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_rgb24_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_line<PixelPRGB32, PixelRGB24>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_rgb24_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_span<PixelPRGB32, PixelRGB24>(dst, span, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixela8_line<PixelPRGB32, PixelA8>(dst, src, w, closure);
  }

  // ==========================================================================
  // [PRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL prgb32_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixela8_span<PixelPRGB32, PixelA8>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_line<PixelXRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_prgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_span<PixelXRGB32, PixelPRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_xrgb32_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_line<PixelXRGB32, PixelFRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - CBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_cblit_xrgb32_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_cblit_pixel32_span<PixelXRGB32, PixelFRGB32>(dst, src, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_prgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_line<PixelXRGB32, PixelPRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - PRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_prgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_span<PixelXRGB32, PixelPRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - XRGB32 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_xrgb32_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_line<PixelXRGB32, PixelXRGB32>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - XRGB32 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_xrgb32_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_span<PixelXRGB32, PixelXRGB32>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - RGB24 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_rgb24_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_line<PixelXRGB32, PixelRGB24>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - RGB24 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_rgb24_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixel32_span<PixelXRGB32, PixelRGB24>(dst, span, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Line]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a8_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
    pixel32_vblit_pixela8_line<PixelXRGB32, PixelA8>(dst, src, w, closure);
  }

  // ==========================================================================
  // [XRGB32 - VBlit - A8 - Span]
  // ==========================================================================

  static void FOG_FASTCALL xrgb32_vblit_a8_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
    pixel32_vblit_pixela8_span<PixelXRGB32, PixelA8>(dst, span, closure);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeAdd]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeAdd : public CompositeGeneric<
  CompositeAdd, RASTER_COMBINE_OP_ADD, RASTER_PRGB_PREPARE_FRGB>
{
  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
    Face::p32FillPBB3(dst0p, src0p);
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
    Face::p32FillPBB3(dst0p_31, src0p_31);
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - Pixel32]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - PixelA8]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t b0p_a8_extended;

    Face::p32ExtendPBWFromSBW(b0p_a8_extended, b0p_a8);
    Face::p32Addus255PBW_2x(dst0p_20, a0p_20, b0p_a8_extended, dst0p_31, a0p_31, b0p_a8_extended);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    prgb32_op_a8_2031(dst0p_20, a0p_20, dst0p_31, a0p_31, b0p_a8);
  }

  // --------------------------------------------------------------------------
  // [PixelA8]
  // --------------------------------------------------------------------------

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    Face::p32Addus255SBW(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeSubtract]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeSubtract : public CompositeGeneric<
  CompositeSubtract, RASTER_COMBINE_OP_SUBTRACT, RASTER_PRGB_PREPARE_NONE>
{
  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - Pixel32]
  // --------------------------------------------------------------------------

  // Dca' = Dca - Sca.
  // Da'  = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t dst0p_a0;
    uint32_t dst0p_b0;

    Face::p32ExtractPBW1(dst0p_a0, a0p_31);
    Face::p32ExtractPBW1(dst0p_b0, b0p_31);
    Face::p32OpOverA8(dst0p_a0, dst0p_a0, dst0p_b0);

    Face::p32Subus255PBW(dst0p_20, a0p_20, b0p_20);
    Face::p32Subus255PBW_ZeroPBW1(dst0p_31, a0p_31, b0p_31);

    if (pack)
    {
      Face::p32LShift(dst0p_a0, dst0p_a0, 24);
      Face::p32LShift(dst0p_31, dst0p_31,  8);

      Face::p32Or(dst0p_20, dst0p_20, dst0p_a0);
      Face::p32Or(dst0p_20, dst0p_20, dst0p_31);
    }
    else
    {
      Face::p32LShift(dst0p_a0, dst0p_a0, 16);
      Face::p32Or(dst0p_31, dst0p_31, dst0p_a0);
    }
  }

  // Dca' = Dca - Sca.
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32Subus255PBW(dst0p_20, a0p_20, b0p_20);
    Face::p32Subus255PBW_FillPBW1(dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - PixelA8]
  // --------------------------------------------------------------------------

  // Dca' = Dca - Sa.
  // Dc'  = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t dst0p_a0;
    uint32_t b0p_a8_extended;

    Face::p32ExtendPBWFromSBW(b0p_a8_extended, b0p_a8);

    Face::p32ExtractPBW1(dst0p_a0, a0p_31);
    Face::p32OpOverA8(dst0p_a0, dst0p_a0, b0p_a8);

    Face::p32Subus255PBW(dst0p_20, a0p_20, b0p_a8_extended);
    Face::p32Subus255PBW_ZeroPBW1(dst0p_31, a0p_31, b0p_a8_extended);

    if (pack)
    {
      Face::p32LShift(dst0p_a0, dst0p_a0, 24);
      Face::p32LShift(dst0p_31, dst0p_31,  8);

      Face::p32Or(dst0p_20, dst0p_20, dst0p_a0);
      Face::p32Or(dst0p_20, dst0p_20, dst0p_31);
    }
    else
    {
      Face::p32LShift(dst0p_a0, dst0p_a0, 16);
      Face::p32Or(dst0p_31, dst0p_31, dst0p_a0);
    }

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dca' = Dc - Sa.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t b0p_a8_extended;

    Face::p32ExtendPBWFromSBW(b0p_a8_extended, b0p_a8);
    Face::p32Subus255PBW(dst0p_20, a0p_20, b0p_a8_extended);
    Face::p32Subus255PBW_FillPBW1(dst0p_31, a0p_31, b0p_a8_extended);

    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // --------------------------------------------------------------------------
  // [PixelA8]
  // --------------------------------------------------------------------------

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeMultiply]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeMultiply : public CompositeGeneric<
  CompositeMultiply, RASTER_COMBINE_OP_MULTIPLY, RASTER_PRGB_PREPARE_NONE>
{
  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - Pixel32]
  // --------------------------------------------------------------------------

  // Dca' = Dca.(Sca + 1 - Sa) + Sca.(1 - Da).
  // Da'  = Da.(Sa + 1 - Sa) + Sa.(1 - Sa).
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t a0p_a8;
    uint32_t b0p_a8;
    uint32_t t0p_20;
    uint32_t t0p_31;

    // Da, Sa.
    Face::p32ExtractPBW1(a0p_a8, a0p_31);
    Face::p32ExtractPBW1(b0p_a8, b0p_31);

    // Sca + 1 - Sa.
    Face::p32Add(t0p_20, b0p_20, 0x00FF00FF);
    Face::p32Add(t0p_31, b0p_31, 0x00FF00FF);
    Face::p32SubPBW_SBW_2x(t0p_20, t0p_20, t0p_31, t0p_31, b0p_a8);

    // Dca.(Sca + 1 - Sa).
    Face::p32MulPBW_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);

    // Sca.(1 - Sa).
    Face::p32Negate255SBW(a0p_a8, a0p_a8);
    Face::p32MulPBW_SBW_2x(t0p_20, b0p_20, a0p_a8, t0p_31, b0p_31, a0p_a8);

    // Dca.(Sca + 1 - Sa) + Sca.(1 - Da).
    Face::p32Add_2x(dst0p_20, dst0p_20, t0p_20, dst0p_31, dst0p_31, t0p_31);
    if (pack)
      Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
    else
      Face::p32Div255PBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31);
  }

  // Dca' = Sc.(Dca + 1 - Da).
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t a0p_a8;

    // Da.
    Face::p32ExtractPBW1(a0p_a8, a0p_31);

    // Dca + 1 - Da.
    Face::p32Add(dst0p_20, a0p_20, 0x00FF00FF);
    Face::p32Add(dst0p_31, a0p_31, 0x00FF00FF);
    Face::p32SubPBW_SBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31, a0p_a8);

    // Sc.(Dca + 1 - Da).
    if (pack)
    {
      Face::p32MulDiv255PBW_2x_Pack_20F1(dst0p_20, dst0p_20, b0p_20, dst0p_31, b0p_31);
    }
    else
    {
      Face::p32MulDiv255PBW(dst0p_20, dst0p_20, b0p_20);
      Face::p32MulDiv255PBW_FillPBW1(dst0p_31, dst0p_31, b0p_31);
    }
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dc' = Dc.(Sca + 1 - Sa).
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t b0p_a8;
    uint32_t t0p_20;
    uint32_t t0p_31;

    // Sa.
    Face::p32ExtractPBW1(b0p_a8, b0p_31);

    // Sca + 1 - Sa.
    Face::p32Add(t0p_20, b0p_20, 0x00FF00FF);
    Face::p32Add(t0p_31, b0p_31, 0x00FF00FF);
    Face::p32SubPBW_SBW_2x(t0p_20, t0p_20, t0p_31, t0p_31, b0p_a8);

    // Dc.(Sca + 1 - Sa).
    if (pack)
    {
      Face::p32MulDiv255PBW_2x_Pack_20F1(dst0p_20, dst0p_20, t0p_20, dst0p_31, t0p_31);
    }
    else
    {
      Face::p32MulDiv255PBW(dst0p_20, dst0p_20, t0p_20);
      Face::p32MulDiv255PBW_FillPBW1(dst0p_31, dst0p_31, t0p_31);
    }
  }

  // Dc' = Dc.Sc.
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    if (pack)
    {
      Face::p32MulDiv255PBW_2x_Pack_20F1(dst0p_20, a0p_20, b0p_20, a0p_31, b0p_31);
    }
    else
    {
      Face::p32MulDiv255PBW(dst0p_20, dst0p_20, a0p_20);
      Face::p32MulDiv255PBW_FillPBW1(dst0p_31, dst0p_31, b0p_31);
    }
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - PixelA8]
  // --------------------------------------------------------------------------

  // Dca' = Dca + Sa.(1 - Da).
  // Da'  = Da + Sa.(1 - Da).
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t a0p_a8;
    
    // Sa.(1 - Da).
    Face::p32ExtractPBW1(a0p_a8, a0p_31);
    Face::p32Negate255SBW(a0p_a8, a0p_a8);
    Face::p32MulDiv255SBW(a0p_a8, a0p_a8, b0p_a8);

    // Dca + Sa.(1 - Da).
    if (pack)
    {
      Face::p32ExtendPBBFromSBB(a0p_a8, a0p_a8);
      Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
      Face::p32Add(dst0p_20, a0p_20, a0p_a8);
    }
    else
    {
      Face::p32ExtendPBWFromSBW(a0p_a8, a0p_a8);
      Face::p32Add_2x(dst0p_20, a0p_20, a0p_a8, dst0p_31, a0p_31, a0p_a8);
    }
  }

  // Dc' = Dc.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    if (pack)
      Face::p32PackPBB2031FromPBW(dst0p_20, a0p_20, a0p_31);
    else
      Face::p32Copy_2x(dst0p_20, a0p_20, dst0p_31, a0p_31);
  }

  // --------------------------------------------------------------------------
  // [PixelA8]
  // --------------------------------------------------------------------------

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeScreen]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeScreen : public CompositeGeneric<
  CompositeScreen, RASTER_COMBINE_OP_SCREEN, RASTER_PRGB_PREPARE_NONE>
{
  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - Pixel32]
  // --------------------------------------------------------------------------

  // Dca' = Sca + Dca.(1 - Sca).
  // Da'  = Sa + Da.(1 - Sa).
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t binv0p_20;
    uint32_t binv0p_31;

    // Dca.(1 - Sca).
    Face::p32Negate255PBW_2x(binv0p_20, b0p_20, binv0p_31, b0p_31);
    Face::p32MulDiv255PBW_2x(dst0p_20, dst0p_20, binv0p_20, dst0p_31, dst0p_31, binv0p_31);

    // Sca + Dca.(1 - Sca).
    Face::p32Add_2x(dst0p_20, dst0p_20, b0p_20, dst0p_31, dst0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dca' = Sc + Dca.(1 - Sc).
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t binv0p_20;
    uint32_t binv0p_31;

    // Dca.(1 - Sc).
    Face::p32Negate255PBW_2x(binv0p_20, b0p_20, binv0p_31, b0p_31);
    Face::p32MulDiv255PBW(dst0p_20, dst0p_20, binv0p_20);
    Face::p32MulDiv255PBW_ZeroPBW1(dst0p_31, dst0p_31, binv0p_31);

    // Sc + Dca.(1 - Sc).
    Face::p32Add_2x(dst0p_20, dst0p_20, b0p_20, dst0p_31, dst0p_31, b0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dca' = Sca + Dc.(1 - Sca)
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t binv0p_20;
    uint32_t binv0p_31;

    // Dc.(1 - Sca).
    Face::p32Negate255PBW_2x(binv0p_20, b0p_20, binv0p_31, b0p_31);
    Face::p32MulDiv255PBW(dst0p_20, dst0p_20, binv0p_20);
    Face::p32MulDiv255PBW_ZeroPBW1(dst0p_31, dst0p_31, binv0p_31);

    // Sca + Dc.(1 - Sca).
    Face::p32Add_2x(dst0p_20, dst0p_20, b0p_20, dst0p_31, dst0p_31, b0p_31);
    Face::p32FillPBW1(dst0p_31, dst0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dc' = Sc + Dc(1 - Sc).
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_prgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - PixelA8]
  // --------------------------------------------------------------------------

  // Dca' = Sa + Dca.(1 - Sa).
  // Da'  = Sa + Da.(1 - Sa).
  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    uint32_t a0p_a8;
    
    // Dca.(1 - Sa).
    Face::p32ExtractPBW1(a0p_a8, a0p_31);
    Face::p32Negate255SBW(a0p_a8, a0p_a8);
    Face::p32MulDiv255PBW_SBW_2x(dst0p_20, dst0p_20, a0p_a8, dst0p_31, dst0p_31, a0p_a8);

    // Sa + Dca.(1 - Sa).
    if (pack)
    {
      Face::p32ExtendPBBFromSBB(a0p_a8, a0p_a8);
      Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
      Face::p32Add(dst0p_20, dst0p_20, a0p_a8);
    }
    else
    {
      Face::p32ExtendPBWFromSBW(a0p_a8, a0p_a8);
      Face::p32Add_2x(dst0p_20, dst0p_20, a0p_a8, dst0p_31, dst0p_31, a0p_a8);
    }
  }

  // Dc' = Dc.
  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
    if (pack)
      Face::p32PackPBB2031FromPBW(dst0p_20, a0p_20, a0p_31);
    else
      Face::p32Copy_2x(dst0p_20, a0p_20, dst0p_31, a0p_31);
  }

  // --------------------------------------------------------------------------
  // [PixelA8]
  // --------------------------------------------------------------------------

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};

// ============================================================================
// [Fog::RasterOps_C - CompositeDarken]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeDarken : public CompositeGeneric<
  CompositeDarken, RASTER_COMBINE_OP_DARKEN, RASTER_PRGB_PREPARE_NONE>
{
  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_prepare_xrgb32(
    uint32_t& dst0p, const uint32_t& src0p)
  {
  }

  static FOG_INLINE void prgb32_prepare_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& src0p_20,
    uint32_t& dst0p_31, const uint32_t& src0p_31)
  {
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - Pixel32]
  // --------------------------------------------------------------------------

  // Dca' = min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa).
  // Da'  = min(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
  //      = Sa + Da - Sa.Da.
  static FOG_INLINE void prgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t d0p_20, d0p_31;
    uint32_t s0p_20, s0p_31;

    Face::p32ExtractPBW1(d0p_20, a0p_31);
    Face::p32ExtractPBW1(s0p_20, b0p_31);

    Face::p32Negate255PBW(d0p_20, d0p_20);
    Face::p32Negate255PBW(s0p_20, s0p_20);

    Face::p32Copy(d0p_31, d0p_20);
    Face::p32Copy(s0p_31, s0p_20);

    // Sca.(1 - Da).
    Face::p32MulPBW_SBW_2x(d0p_20, d0p_20, b0p_20, d0p_31, d0p_31, b0p_31);
    // Dca.(1 - Sa).
    Face::p32MulPBW_SBW_2x(s0p_20, s0p_20, a0p_20, s0p_31, s0p_31, a0p_31);
    // Dca.(1 - Sa) + Sca.(1 - Da).
    Face::p32Add_2x(d0p_20, d0p_20, s0p_20, d0p_31, d0p_31, s0p_31);

    // Da.
    Face::p32ExtractPBW1(s0p_20, a0p_31);
    // Sa.
    Face::p32ExtractPBW1(s0p_31, b0p_31);

    // Dca.Sa.
    Face::p32MulPBW_SBW_2x(dst0p_20, a0p_20, s0p_31, dst0p_31, dst0p_31, s0p_31);
    // Sca.Da.
    Face::p32MulPBW_SBW_2x(s0p_20, b0p_20, s0p_20, s0p_31, b0p_31, s0p_20);
    
    // min(Sca.Da, Dca.Sa).
    Face::p32MinPBW_2x(dst0p_20, dst0p_20, s0p_20, dst0p_31, dst0p_31, s0p_31);
    // min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa).
    Face::p32Add_2x(dst0p_20, dst0p_20, d0p_20, dst0p_31, dst0p_31, d0p_31);

    if (pack)
      Face::p32Div255PBW_2x_Pack_0231(dst0p_20, dst0p_20, dst0p_31);
    else
      Face::p32Div255PBW_2x(dst0p_20, dst0p_20, dst0p_31, dst0p_31);
  }

  // Dca' = min(Sc.Da, Dca) + Sc.(1 - Da).
  // Da'  = 1.
  static FOG_INLINE void prgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t d0p_20, d0p_31;
    uint32_t s0p_20, s0p_31;

    Face::p32ExtractPBW1(d0p_20, a0p_31);
    Face::p32Negate255PBW(d0p_20, d0p_20);
    Face::p32Copy(d0p_31, d0p_20);

    // Sc.(1 - Da).
    Face::p32MulDiv255PBW_SBW_2x(d0p_20, d0p_20, b0p_20, d0p_31, d0p_31, b0p_31);

    // Da.
    Face::p32ExtractPBW1(s0p_20, a0p_31);
    // Sa.Da.
    Face::p32MulDiv255PBW_SBW_2x(s0p_20, b0p_20, s0p_20, s0p_31, b0p_31, s0p_20);
    
    // min(Sc.Da, Dca).
    Face::p32MinPBW(dst0p_20, a0p_20, s0p_20);
    Face::p32MinPBW_FillPBW1(dst0p_31, a0p_31, s0p_31);
    // min(Sc.Da, Dca) + Sc.(1 - Da).
    Face::p32Add_2x(dst0p_20, dst0p_20, d0p_20, dst0p_31, dst0p_31, d0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void prgb32_op_frgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  static FOG_INLINE void prgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    prgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // Dc' = min(Sca, Dc.Sa) + Dc.(1 - Sa).
  static FOG_INLINE void xrgb32_op_prgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    uint32_t d0p_20, d0p_31;
    uint32_t sa;

    // Dc.(1 - Sa).
    Face::p32ExtractPBW1(sa, b0p_31);
    Face::p32Negate255SBW(d0p_20, sa);
    Face::p32MulDiv255PBW_SBW_2x(d0p_20, a0p_20, d0p_20, d0p_31, a0p_31, d0p_20);

    Face::p32MulDiv255PBW_SBW_2x(dst0p_20, a0p_20, sa, dst0p_31, a0p_31, sa);

    // min(Sca, Dc.Sa).
    Face::p32MinPBW(dst0p_20, dst0p_20, a0p_20);
    Face::p32MinPBW_FillPBW1(dst0p_31, dst0p_31, a0p_31);

    // min(Sca, Dc.Sa) + dc.(1 - Sa).
    Face::p32Add_2x(dst0p_20, dst0p_20, d0p_20, dst0p_31, dst0p_31, d0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  // Dc' = min(Sc, Dc).
  static FOG_INLINE void xrgb32_op_xrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    Face::p32MinPBW(dst0p_20, a0p_20, b0p_20);
    Face::p32MinPBW_FillPBW1(dst0p_31, a0p_31, b0p_31);
    if (pack) Face::p32PackPBB2031FromPBW(dst0p_20, dst0p_20, dst0p_31);
  }

  static FOG_INLINE void zrgb32_op_zrgb32_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20, const uint32_t& b0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_31,
    bool pack = false)
  {
    xrgb32_op_xrgb32_2031(dst0p_20, a0p_20, b0p_20, dst0p_31, a0p_31, b0p_31, pack);
  }

  // --------------------------------------------------------------------------
  // [Pixel32 - Op - PixelA8]
  // --------------------------------------------------------------------------

  static FOG_INLINE void prgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
  }

  static FOG_INLINE void xrgb32_op_a8_2031(
    uint32_t& dst0p_20, const uint32_t& a0p_20,
    uint32_t& dst0p_31, const uint32_t& a0p_31, const uint32_t& b0p_a8,
    bool pack = false)
  {
  }

  // --------------------------------------------------------------------------
  // [PixelA8]
  // --------------------------------------------------------------------------

  static FOG_INLINE void a8_op_a8(
    uint32_t& dst0p, const uint32_t& a0p, const uint32_t& b0p)
  {
    Face::p32OpOverA8(dst0p, a0p, b0p);
  }
};














} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITEEXT_P_H
