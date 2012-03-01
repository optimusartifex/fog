// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBASE_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBASE_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/BaseDefs_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Accessor - Documentation]
// ============================================================================

// Suffix:
//
//   'S' - Solid (fetch or store).
//   'M' - Memory used by Fog::Image (pixel format dependent).
//   'T' - Temporary memory, usually referred as 'stack' memory.

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Accessor - PRGB32]
// ============================================================================

struct FOG_NO_EXPORT FBaseAccessor_PRGB32
{
  // --------------------------------------------------------------------------
  // [Defs]
  // --------------------------------------------------------------------------

  typedef uint32_t Pixel;

  enum { PIXEL_BPP = 4 };
  enum { STACK_BPP = 4 };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchPixelS(Pixel& dst, const RasterSolid& src)
  {
    dst = src.prgb32.u32;
  }

  static FOG_INLINE void fetchPixelM(Pixel& dst, const uint8_t* src)
  {
    Acc::p32Load4a(dst, src);
  }
  
  static FOG_INLINE void fetchPixelT(Pixel& dst, const uint8_t* src)
  {
    Acc::p32Load4a(dst, src);
  }

  static FOG_INLINE void storePixelM(uint8_t* dst, const Pixel& src)
  {
    Acc::p32Store4a(dst, src);
  }

  static FOG_INLINE void storePixelT(uint8_t* dst, const Pixel& src)
  {
    Acc::p32Store4a(dst, src);
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Accessor - XRGB32]
// ============================================================================

struct FOG_NO_EXPORT FBaseAccessor_XRGB32
{
  // --------------------------------------------------------------------------
  // [Defs]
  // --------------------------------------------------------------------------

  typedef uint32_t Pixel;

  enum { PIXEL_BPP = 4 };
  enum { STACK_BPP = 4 };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchPixelS(Pixel& dst, const RasterSolid& src)
  {
    dst = src.prgb32.u32;
  }

  static FOG_INLINE void fetchPixelM(Pixel& dst, const uint8_t* src)
  {
    Acc::p32Load4a(dst, src);
  }

  static FOG_INLINE void fetchPixelT(Pixel& dst, const uint8_t* src)
  {
    Acc::p32Load4a(dst, src);
  }

  static FOG_INLINE void storePixelM(uint8_t* dst, const Pixel& src)
  {
    Acc::p32Store4a(dst, src);
  }
  
  static FOG_INLINE void storePixelT(uint8_t* dst, const Pixel& src)
  {
    Acc::p32Store4a(dst, src);
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Accessor - RGB24]
// ============================================================================

struct FOG_NO_EXPORT FBaseAccessor_RGB24
{
  // --------------------------------------------------------------------------
  // [Defs]
  // --------------------------------------------------------------------------

  typedef uint32_t Pixel;

  enum { PIXEL_BPP = 3 };
  enum { STACK_BPP = 4 };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchPixelS(Pixel& dst, const RasterSolid& src)
  {
    dst = src.prgb32.u32;
  }

  static FOG_INLINE void fetchPixelM(Pixel& dst, const uint8_t* src)
  {
    Acc::p32Load3b(dst, src);
  }

  static FOG_INLINE void fetchPixelT(Pixel& dst, const uint8_t* src)
  {
    Acc::p32Load4a(dst, src);
  }

  static FOG_INLINE void storePixelM(uint8_t* dst, const Pixel& src)
  {
    Acc::p32Store3b(dst, src);
  }
  
  static FOG_INLINE void storePixelT(uint8_t* dst, const Pixel& src)
  {
    Acc::p32Store4a(dst, src);
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Base - Accessor - A8]
// ============================================================================

struct FOG_NO_EXPORT FBaseAccessor_A8
{
  // --------------------------------------------------------------------------
  // [Defs]
  // --------------------------------------------------------------------------

  typedef uint8_t Pixel;

  enum { PIXEL_BPP = 1 };
  enum { STACK_BPP = 1 };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchPixelS(Pixel& dst, const RasterSolid& src)
  {
    dst = src.prgb32.a;
  }

  static FOG_INLINE void fetchPixelM(Pixel& dst, const uint8_t* src)
  {
    Acc::p8Load1b(dst, src);
  }

  static FOG_INLINE void fetchPixelT(Pixel& dst, const uint8_t* src)
  {
    Acc::p8Load1b(dst, src);
  }

  static FOG_INLINE void storePixelM(uint8_t* dst, const Pixel& src)
  {
    Acc::p8Store1b(dst, src);
  }
  
  static FOG_INLINE void storePixelT(uint8_t* dst, const Pixel& src)
  {
    Acc::p8Store1b(dst, src);
  }
};

} // RasterOps_C namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBASE_P_H
