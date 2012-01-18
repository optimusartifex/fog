// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/FilterBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Base]
// ============================================================================

// The Stack Blur Algorithm was invented by Mario Klingemann,
// mario@quasimondo.com and described here:
//
//   http://incubator.quasimondo.com/processing/fast_blur_deluxe.php
//   (search phrase "Stackblur: Fast But Goodlooking").
//
// The major improvement is that there's no more division table that was very
// expensive to create for large blur radii. Insted, for 8-bit per channel and
// radius not exceeding 254 the division is replaced by multiplication and shift.
//
// Blurring and convolution based effects need to access the neighboring pixels.
// It's fine until you need to access pixels outside the image boundary, in this
// case there are several modes possible, each very specific when it comes to
// implementation. So instead of making big switch() in code, or instead of
// making several versions of blur there is a table where are stored all
// indices of pixels which are outside of the raster, and are mapped to the
// indices inside.
//
// So in general the blur effector contains five different loops, processed in
// the current order:
//
//   1. 'aBorderLeadSize' - Process the leading border pixels, used only by 
//      FE_EXTEND_COLOR or FE_EXTEND_PAD).
//      - No data is write at this step.
//
//   2. 'aTableSize' - Process the pixels which offset is stored in aTableData.
//      - No data is write at this step, ATable is used for indices to pixels.
//
//   3. 'aBorderTailSize' - Process the tailing border pixels, used only by
//      FE_EXTEND_COLOR or FE_EXTEND_PAD as a special case where it's needed
//      to process pixels from the second border.
//      - No data is write at this step.
//
//   4. 'runSize - Standard run-loop, which should do the most of the work.
//      - One pixel is read and stored per loop iteration.
//      - No checking for borders, generally the fastest loop.
//
//   5. 'bTableSize' - Process the pixels which offset is stored in bTableData,
//      - One pixel is read and stored per loop iteration.
//
//   6. 'bBorderTailSize' - Process the tailing border pixels.
//      - No data is read at this step, one pixel is stored per iteration.
//
// The naming convention is strict. For initial processing the prefix 'a' is
// used (aBorderSize, aTableSize, ...). For final processing the prefix 'b' is
// used (bBorderSize, bTableSize, ...).
//
// For SSE2 version please see RasterOps_SSE2 directory.

// How many pixels to process horizontally in BlurV. The problem here is that
// when using the standard way (1 pixel per run) then there is unpredictable
// random memory access, which is costly. So we process several pixels in the
// same memory location to reduce the effect of random memory access, but of
// course we do more memory reads/writes, but these should be fast, because the
// reads/writes are continuous and should be already in L1/L2 cache.
//
// Higher value means more memory used by blur stack, but should increase 
// performance. Experiments are welcome.
enum { BLUR_RECT_V_HLINE_COUNT = 16 };

// A-Precision of alpha parameter in exponential blur (fixed point 0.A).
enum { BLUR_APREC = 12 };

// Z-Precision of state parameter in exponential blur (fixed point 8.Z).
enum { BLUR_ZPREC = 10 };

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Run - PRGB32]
// ============================================================================

template<typename IntT>
struct FOG_NO_EXPORT FBlurRun_PRGB32
{
  typedef FBlurRun_PRGB32 Run;
  typedef FBaseAccessor_PRGB32::Pixel Pixel;

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    a = 0;
    r = 0;
    g = 0;
    b = 0;
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE void set(const Run& run, IntT scale = 1)
  {
    a = run.a * scale;
    r = run.r * scale;
    g = run.g * scale;
    b = run.b * scale;
  }
  
  FOG_INLINE void set(const Pixel& pix, IntT scale = 1)
  {
    a = static_cast<IntT>((pix >> 24)       ) * scale;
    r = static_cast<IntT>((pix >> 16) & 0xFF) * scale;
    g = static_cast<IntT>((pix >>  8) & 0xFF) * scale;
    b = static_cast<IntT>((pix      ) & 0xFF) * scale;
  }

  // --------------------------------------------------------------------------
  // [Ops]
  // --------------------------------------------------------------------------

  FOG_INLINE void add(const Pixel& pix, IntT scale = 1)
  {
    a += static_cast<IntT>((pix >> 24)       ) * scale;
    r += static_cast<IntT>((pix >> 16) & 0xFF) * scale;
    g += static_cast<IntT>((pix >>  8) & 0xFF) * scale;
    b += static_cast<IntT>((pix      ) & 0xFF) * scale;
  }

  FOG_INLINE void add(const Run& run, IntT scale = 1)
  {
    a += run.a * scale;
    r += run.r * scale;
    g += run.g * scale;
    b += run.b * scale;
  }

  FOG_INLINE void sub(const Pixel& pix, IntT scale = 1)
  {
    a -= static_cast<IntT>((pix >> 24)       ) * scale;
    r -= static_cast<IntT>((pix >> 16) & 0xFF) * scale;
    g -= static_cast<IntT>((pix >>  8) & 0xFF) * scale;
    b -= static_cast<IntT>((pix      ) & 0xFF) * scale;
  }

  FOG_INLINE void sub(const Run& run, IntT scale = 1)
  {
    a -= run.a * scale;
    r -= run.r * scale;
    g -= run.g * scale;
    b -= run.b * scale;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  IntT a, r, g, b;
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Run - XRGB32]
// ============================================================================

template<typename IntT>
struct FOG_NO_EXPORT FBlurRun_XRGB32
{
  typedef FBlurRun_XRGB32 Run;
  typedef FBaseAccessor_XRGB32::Pixel Pixel;

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    r = 0;
    g = 0;
    b = 0;
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE void set(const Run& run, IntT scale = 1)
  {
    r = run.r * scale;
    g = run.g * scale;
    b = run.b * scale;
  }

  FOG_INLINE void set(const Pixel& pix, IntT scale = 1)
  {
    r = static_cast<IntT>((pix >> 16) & 0xFF) * scale;
    g = static_cast<IntT>((pix >>  8) & 0xFF) * scale;
    b = static_cast<IntT>((pix      ) & 0xFF) * scale;
  }

  // --------------------------------------------------------------------------
  // [Ops]
  // --------------------------------------------------------------------------

  FOG_INLINE void add(const Pixel& pix, IntT scale = 1)
  {
    r += static_cast<IntT>((pix >> 16) & 0xFF) * scale;
    g += static_cast<IntT>((pix >>  8) & 0xFF) * scale;
    b += static_cast<IntT>((pix      ) & 0xFF) * scale;
  }

  FOG_INLINE void add(const Run& run, IntT scale = 1)
  {
    r += run.r * scale;
    g += run.g * scale;
    b += run.b * scale;
  }

  FOG_INLINE void sub(const Pixel& pix, IntT scale = 1)
  {
    r -= static_cast<IntT>((pix >> 16) & 0xFF) * scale;
    g -= static_cast<IntT>((pix >>  8) & 0xFF) * scale;
    b -= static_cast<IntT>((pix      ) & 0xFF) * scale;
  }

  FOG_INLINE void sub(const Run& run, IntT scale = 1)
  {
    r -= run.r * scale;
    g -= run.g * scale;
    b -= run.b * scale;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  IntT r, g, b;
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Run - RGB24]
// ============================================================================

template<typename IntT>
struct FOG_NO_EXPORT FBlurRun_RGB24
{
  typedef FBlurRun_RGB24 Run;
  typedef FBaseAccessor_RGB24::Pixel Pixel;

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    r = 0;
    g = 0;
    b = 0;
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE void set(const Run& run, IntT scale = 1)
  {
    r = run.r * scale;
    g = run.g * scale;
    b = run.b * scale;
  }

  FOG_INLINE void set(const Pixel& pix, IntT scale = 1)
  {
    r = static_cast<IntT>((pix >> 16) & 0xFF) * scale;
    g = static_cast<IntT>((pix >>  8) & 0xFF) * scale;
    b = static_cast<IntT>((pix      ) & 0xFF) * scale;
  }

  // --------------------------------------------------------------------------
  // [Ops]
  // --------------------------------------------------------------------------

  FOG_INLINE void add(const Pixel& pix, IntT scale = 1)
  {
    r += static_cast<IntT>((pix >> 16) & 0xFF) * scale;
    g += static_cast<IntT>((pix >>  8) & 0xFF) * scale;
    b += static_cast<IntT>((pix      ) & 0xFF) * scale;
  }

  FOG_INLINE void add(const Run& run, IntT scale = 1)
  {
    r += run.r * scale;
    g += run.g * scale;
    b += run.b * scale;
  }

  FOG_INLINE void sub(const Pixel& pix, IntT scale = 1)
  {
    r -= static_cast<IntT>((pix >> 16) & 0xFF) * scale;
    g -= static_cast<IntT>((pix >>  8) & 0xFF) * scale;
    b -= static_cast<IntT>((pix      ) & 0xFF) * scale;
  }

  FOG_INLINE void sub(const Run& run, IntT scale = 1)
  {
    r -= run.r * scale;
    g -= run.g * scale;
    b -= run.b * scale;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  IntT r, g, b;
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Run - A8]
// ============================================================================

template<typename IntT>
struct FOG_NO_EXPORT FBlurRun_A8
{
  typedef FBlurRun_A8 Run;
  typedef FBaseAccessor_A8::Pixel Pixel;

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    a = 0;
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE void set(const Run& run, IntT scale = 1)
  {
    a = run.a * scale;
  }

  FOG_INLINE void set(const Pixel& pix, IntT scale = 1)
  {
    a = static_cast<IntT>(pix) * scale;
  }

  // --------------------------------------------------------------------------
  // [Ops]
  // --------------------------------------------------------------------------

  FOG_INLINE void add(const Pixel& pix, IntT scale = 1)
  {
    a += static_cast<IntT>(pix) * scale;
  }

  FOG_INLINE void add(const Run& run, IntT scale = 1)
  {
    a += run.a * scale;
  }

  FOG_INLINE void sub(const Pixel& pix, IntT scale = 1)
  {
    a -= static_cast<IntT>(pix) * scale;
  }

  FOG_INLINE void sub(const Run& run, IntT scale = 1)
  {
    a -= run.a * scale;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  IntT a;
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Simple - Accessor - PRGB32]
// ============================================================================

template<typename IntT>
struct FOG_NO_EXPORT FBlurBaseAccessor_PRGB32 : public FBaseAccessor_PRGB32
{
  typedef FBlurRun_PRGB32<IntT> Run;

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchRunM(const Run& run, const uint8_t* src)
  {
    uint32_t pix;
    Face::p32Load4a(pix, src);
    run.set(pix);
  }

  static FOG_INLINE void fetchRunT(const Run& run, const uint8_t* src)
  {
    fetchRunM(run, src);
  }

  static FOG_INLINE void storeRunM(uint8_t* dst, const Run& run, IntT scale, uint32_t shift)
  {
    uint32_t pix = _FOG_FACE_COMBINE_4(
      static_cast<uint32_t>((run.a * scale) >> shift) << 24,
      static_cast<uint32_t>((run.r * scale) >> shift) << 16,
      static_cast<uint32_t>((run.g * scale) >> shift) <<  8,
      static_cast<uint32_t>((run.b * scale) >> shift)      );
    Face::p32Store4a(dst, pix);
  }

  static FOG_INLINE void storeRunT(uint8_t* dst, const Run& run, IntT scale, uint32_t shift)
  {
    storeRunM(dst, run, scale, shift);
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Simple - Accessor - XRGB32]
// ============================================================================

template<typename IntT>
struct FOG_NO_EXPORT FBlurBaseAccessor_XRGB32 : public FBaseAccessor_XRGB32
{
  typedef FBlurRun_XRGB32<IntT> Run;

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchRunM(const Run& run, const uint8_t* src)
  {
    uint32_t pix;
    Face::p32Load4a(pix, src);
    run.set(pix);
  }

  static FOG_INLINE void fetchRunT(const Run& run, const uint8_t* src)
  {
    fetchRunM(run, src);
  }

  static FOG_INLINE void storeRunM(uint8_t* dst, const Run& run, IntT scale, uint32_t shift)
  {
    uint32_t pix = _FOG_FACE_COMBINE_4(
      0xFF000000,
      static_cast<uint32_t>((run.r * scale) >> shift) << 16,
      static_cast<uint32_t>((run.g * scale) >> shift) <<  8,
      static_cast<uint32_t>((run.b * scale) >> shift)      );
    Face::p32Store4a(dst, pix);
  }

  static FOG_INLINE void storeRunT(uint8_t* dst, const Run& run, IntT scale, uint32_t shift)
  {
    storeRunM(dst, run, scale, shift);
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Simple - Accessor - RGB24]
// ============================================================================

template<typename IntT>
struct FOG_NO_EXPORT FBlurBaseAccessor_RGB24 : public FBaseAccessor_RGB24
{
  typedef FBlurRun_RGB24<IntT> Run;

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchRunM(const Run& run, const uint8_t* src)
  {
    run.r = static_cast<IntT>(src[PIXEL_RGB24_POS_R]);
    run.g = static_cast<IntT>(src[PIXEL_RGB24_POS_G]);
    run.b = static_cast<IntT>(src[PIXEL_RGB24_POS_B]);
  }

  static FOG_INLINE void fetchRunT(const Run& run, const uint8_t* src)
  {
    uint32_t pix;
    Face::p32Load4a(pix, src);
    run.set(pix);
  }

  static FOG_INLINE void storeRunM(uint8_t* dst, const Run& run, IntT scale, uint32_t shift)
  {
    dst[PIXEL_RGB24_POS_R] = static_cast<uint8_t>((run.r * scale) >> shift);
    dst[PIXEL_RGB24_POS_G] = static_cast<uint8_t>((run.g * scale) >> shift);
    dst[PIXEL_RGB24_POS_B] = static_cast<uint8_t>((run.b * scale) >> shift);
  }

  static FOG_INLINE void storeRunT(uint8_t* dst, const Run& run, IntT scale, uint32_t shift)
  {
    uint32_t pix = _FOG_FACE_COMBINE_3(
      static_cast<uint32_t>((run.r * scale) >> shift) << 16,
      static_cast<uint32_t>((run.g * scale) >> shift) <<  8,
      static_cast<uint32_t>((run.b * scale) >> shift)      );
    Face::p32Store4a(dst, pix);
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Simple - Accessor - A8]
// ============================================================================

template<typename IntT>
struct FOG_NO_EXPORT FBlurBaseAccessor_A8 : public FBaseAccessor_A8
{
  typedef FBlurRun_A8<IntT> Run;

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  static FOG_INLINE void fetchRunM(const Run& run, uint8_t* src)
  {
    run.r = static_cast<IntT>(src[0]);
  }

  static FOG_INLINE void fetchRunT(const Run& run, uint8_t* src)
  {
    fethRunM(run, src);
  }

  static FOG_INLINE void storeRunM(uint8_t* dst, const Run& run, IntT scale, uint32_t shift)
  {
    dst[0] = static_cast<uint8_t>((run.a * scale) >> shift);
  }

  static FOG_INLINE void storeRunT(uint8_t* dst, const Run& run, IntT scale, uint32_t shift)
  {
    storeRunM(dst, run, scale, shift);
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Box - Accessors]
// ============================================================================

typedef FBlurBaseAccessor_PRGB32<uint32_t> FBlurBoxAccessor_PRGB32;
typedef FBlurBaseAccessor_XRGB32<uint32_t> FBlurBoxAccessor_XRGB32;
typedef FBlurBaseAccessor_RGB24 <uint32_t> FBlurBoxAccessor_RGB24;
typedef FBlurBaseAccessor_A8    <uint32_t> FBlurBoxAccessor_A8;

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Stack - Accessors]
// ============================================================================

typedef FBlurBaseAccessor_PRGB32<uint32_t> FBlurStackAccessor_PRGB32;
typedef FBlurBaseAccessor_XRGB32<uint32_t> FBlurStackAccessor_XRGB32;
typedef FBlurBaseAccessor_RGB24 <uint32_t> FBlurStackAccessor_RGB24;
typedef FBlurBaseAccessor_A8    <uint32_t> FBlurStackAccessor_A8;

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur - Exponential - Accessors]
// ============================================================================

struct FOG_NO_EXPORT FBlurExpAccessor_PRGB32 : public FBlurBaseAccessor_PRGB32<int32_t>
{
  static FOG_INLINE void blurPixel(Run& run, const Pixel& pix, int32_t aValue)
  {
    run.a += (aValue * (static_cast<int32_t>(Math::shift<uint32_t, 24 - BLUR_ZPREC>(pix & 0xFF000000)) - run.a)) >> BLUR_APREC;
    run.r += (aValue * (static_cast<int32_t>(Math::shift<uint32_t, 16 - BLUR_ZPREC>(pix & 0x00FF0000)) - run.r)) >> BLUR_APREC;
    run.g += (aValue * (static_cast<int32_t>(Math::shift<uint32_t,  8 - BLUR_ZPREC>(pix & 0x0000FF00)) - run.g)) >> BLUR_APREC;
    run.b += (aValue * (static_cast<int32_t>(Math::shift<uint32_t,  0 - BLUR_ZPREC>(pix & 0x000000FF)) - run.b)) >> BLUR_APREC;
  }
  
  static FOG_INLINE void blurPixel(Run& run, const Run& src, int32_t aValue)
  {
    run.a += (aValue * ((src.a << BLUR_ZPREC) - run.a)) >> BLUR_APREC;
    run.r += (aValue * ((src.r << BLUR_ZPREC) - run.r)) >> BLUR_APREC;
    run.g += (aValue * ((src.g << BLUR_ZPREC) - run.g)) >> BLUR_APREC;
    run.b += (aValue * ((src.b << BLUR_ZPREC) - run.b)) >> BLUR_APREC;
  }

  static FOG_INLINE void blurRunM(Run& run, const uint8_t* src, int32_t aValue)
  {
    uint32_t pix;
    Face::p32Load4a(pix, src);
    blurPixel(run, pix, aValue);
  }

  static FOG_INLINE void blurRunT(Run& run, const uint8_t* src, int32_t aValue)
  {
    blurRunM(run, src, aValue);
  }
  
  static FOG_INLINE void storeRunM(uint8_t* dst, const Run& run)
  {
    uint32_t pix = _FOG_FACE_COMBINE_4(
      (run.a >> BLUR_ZPREC) << 24,
      (run.r >> BLUR_ZPREC) << 16,
      (run.g >> BLUR_ZPREC) <<  8,
      (run.b >> BLUR_ZPREC) <<  0);
    Face::p32Store4a(dst, pix);
  }

  static FOG_INLINE void storeRunT(uint8_t* dst, const Run& run)
  {
    storeRunM(dst, run);
  }
};

struct FOG_NO_EXPORT FBlurExpAccessor_XRGB32 : public FBlurBaseAccessor_XRGB32<int32_t>
{
  static FOG_INLINE void blurPixel(Run& run, const Pixel& pix, int32_t aValue)
  {
    run.r += (aValue * (static_cast<int32_t>(Math::shift<uint32_t, 16 - BLUR_ZPREC>(pix & 0x00FF0000)) - run.r)) >> BLUR_APREC;
    run.g += (aValue * (static_cast<int32_t>(Math::shift<uint32_t,  8 - BLUR_ZPREC>(pix & 0x0000FF00)) - run.g)) >> BLUR_APREC;
    run.b += (aValue * (static_cast<int32_t>(Math::shift<uint32_t,  0 - BLUR_ZPREC>(pix & 0x000000FF)) - run.b)) >> BLUR_APREC;
  }
  
  static FOG_INLINE void blurPixel(Run& run, const Run& src, int32_t aValue)
  {
    run.r += (aValue * ((src.r << BLUR_ZPREC) - run.r)) >> BLUR_APREC;
    run.g += (aValue * ((src.g << BLUR_ZPREC) - run.g)) >> BLUR_APREC;
    run.b += (aValue * ((src.b << BLUR_ZPREC) - run.b)) >> BLUR_APREC;
  }

  static FOG_INLINE void blurRunM(Run& run, const uint8_t* src, int32_t aValue)
  {
    uint32_t pix;
    Face::p32Load4a(pix, src);
    blurPixel(run, pix, aValue);
  }

  static FOG_INLINE void blurRunT(Run& run, const uint8_t* src, int32_t aValue)
  {
    blurRunM(run, src, aValue);
  }
  
  static FOG_INLINE void storeRunM(uint8_t* dst, const Run& run)
  {
    uint32_t pix = _FOG_FACE_COMBINE_4(
      0xFF000000,
      (run.r >> BLUR_ZPREC) << 16,
      (run.g >> BLUR_ZPREC) <<  8,
      (run.b >> BLUR_ZPREC) <<  0);
    Face::p32Store4a(dst, pix);
  }

  static FOG_INLINE void storeRunT(uint8_t* dst, const Run& run)
  {
    storeRunM(dst, run);
  }
};

struct FOG_NO_EXPORT FBlurExpAccessor_RGB24 : public FBlurBaseAccessor_RGB24<int32_t>
{
  static FOG_INLINE void blurPixel(Run& run, const Pixel& pix, int32_t aValue)
  {
    run.r += (aValue * (static_cast<int32_t>(Math::shift<uint32_t, 16 - BLUR_ZPREC>(pix & 0x00FF0000)) - run.r)) >> BLUR_APREC;
    run.g += (aValue * (static_cast<int32_t>(Math::shift<uint32_t,  8 - BLUR_ZPREC>(pix & 0x0000FF00)) - run.g)) >> BLUR_APREC;
    run.b += (aValue * (static_cast<int32_t>(Math::shift<uint32_t,  0 - BLUR_ZPREC>(pix & 0x000000FF)) - run.b)) >> BLUR_APREC;
  }

  static FOG_INLINE void blurPixel(Run& run, const Run& src, int32_t aValue)
  {
    run.r += (aValue * ((src.r << BLUR_ZPREC) - run.r)) >> BLUR_APREC;
    run.g += (aValue * ((src.g << BLUR_ZPREC) - run.g)) >> BLUR_APREC;
    run.b += (aValue * ((src.b << BLUR_ZPREC) - run.b)) >> BLUR_APREC;
  }

  static FOG_INLINE void blurRunM(Run& run, const uint8_t* src, int32_t aValue)
  {
    run.r += (aValue * (static_cast<int32_t>(src[PIXEL_RGB24_POS_R]) - run.r)) >> BLUR_APREC;
    run.g += (aValue * (static_cast<int32_t>(src[PIXEL_RGB24_POS_G]) - run.g)) >> BLUR_APREC;
    run.b += (aValue * (static_cast<int32_t>(src[PIXEL_RGB24_POS_B]) - run.b)) >> BLUR_APREC;
  }

  static FOG_INLINE void blurRunT(Run& run, const uint8_t* src, int32_t aValue)
  {
    uint32_t pix;
    Face::p32Load4a(pix, src);
    blurPixel(run, pix, aValue);
  }

  static FOG_INLINE void storeRunM(uint8_t* dst, const Run& run)
  {
    dst[PIXEL_RGB24_POS_R] = static_cast<uint8_t>(run.r >> BLUR_ZPREC);
    dst[PIXEL_RGB24_POS_G] = static_cast<uint8_t>(run.g >> BLUR_ZPREC);
    dst[PIXEL_RGB24_POS_B] = static_cast<uint8_t>(run.b >> BLUR_ZPREC);
  }
  
  static FOG_INLINE void storeRunT(uint8_t* dst, const Run& run)
  {
    uint32_t pix = _FOG_FACE_COMBINE_3(
      (run.r >> BLUR_ZPREC) << 16,
      (run.g >> BLUR_ZPREC) <<  8,
      (run.b >> BLUR_ZPREC) <<  0);
    Face::p32Store4a(dst, pix);
  }
};

struct FOG_NO_EXPORT FBlurExpAccessor_A8 : public FBlurBaseAccessor_A8<int32_t>
{
  static FOG_INLINE void blurPixel(Run& run, const Pixel& pix, int32_t aValue)
  {
    run.a += (aValue * (static_cast<int32_t>(pix << BLUR_ZPREC) - run.a)) >> BLUR_APREC;
  }

  static FOG_INLINE void blurPixel(Run& run, const Run& src, int32_t aValue)
  {
    run.a += (aValue * ((src.a << BLUR_ZPREC) - run.a)) >> BLUR_APREC;
  }

  static FOG_INLINE void blurRunM(Run& run, const uint8_t* src, int32_t aValue)
  {
    run.a += (aValue * (static_cast<int32_t>(src[0]) - run.a)) >> BLUR_APREC;
  }

  static FOG_INLINE void blurRunT(Run& run, const uint8_t* src, int32_t aValue)
  {
    blurRunM(run, src, aValue);
  }

  static FOG_INLINE void storeRunM(uint8_t* dst, const Run& run)
  {
    dst[0] = static_cast<uint8_t>(run.a >> BLUR_ZPREC);
  }

  static FOG_INLINE void storeRunT(uint8_t* dst, const Run& run)
  {
    storeRunM(dst, run);
  }
};

// ============================================================================
// [Fog::RasterOps_C - Filter - Blur]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT FBlur
{
  // ==========================================================================
  // [Blur - Create]
  // ==========================================================================

  static err_t FOG_FASTCALL create(
    RasterFilter* ctx, const FeBase* feBase, const ImageFilterScaleD* feScale,
    MemBuffer* memBuffer,
    uint32_t dstFormat,
    uint32_t srcFormat)
  {
    if (dstFormat == IMAGE_FORMAT_I8 || srcFormat == IMAGE_FORMAT_I8)
      return ERR_IMAGE_INVALID_FORMAT;

    // TODO: We should allow to mix some basic formats in the future.
    if (dstFormat != srcFormat)
      return ERR_IMAGE_INVALID_FORMAT;

    FOG_ASSERT(feBase->getFeType() == FE_TYPE_BLUR);
    const FeBlur* feData = static_cast<const FeBlur*>(feBase);

    uint32_t blurType = feData->_blurType;

    ctx->reference.init(1);
    ctx->destroy = destroy;

    ctx->doRect = doRect;
    ctx->doLine = NULL;

    ctx->memBuffer = memBuffer;
    ctx->dstFormat = dstFormat;
    ctx->srcFormat = srcFormat;

    ctx->blur.blurType = blurType;
    ctx->blur.extendColor.reset();
    ctx->blur.extendType = feData->_extendType;

    if (ctx->blur.extendType == FE_EXTEND_COLOR)
    {
      if (ImageFormatDescription::getByFormat(dstFormat).is16Bpc())
      {
        Argb64 argb64 = feData->_extendColor().getArgb64();
        Face::p64PRGB64FromARGB64(ctx->blur.extendColor.prgb64.p64, argb64.p64);
      }
      else
      {
        Argb32 argb32 = feData->_extendColor().getArgb32();
        Face::p32PRGB32FromARGB32(ctx->blur.extendColor.prgb32.p32, argb32.p32);
      }
    }

    float hRadiusScale = 1.0f;
    float vRadiusScale = 1.0f;

    if (feScale != NULL)
    {
      hRadiusScale = float(feScale->_pt.x);
      vRadiusScale = float(feScale->_pt.y);
    }

    float hRadius = Math::bound<float>(Math::abs(feData->_hRadius * hRadiusScale), 0.0f, FE_BLUR_LIMIT_RADIUS);
    float vRadius = Math::bound<float>(Math::abs(feData->_vRadius * vRadiusScale), 0.0f, FE_BLUR_LIMIT_RADIUS);

    if (feScale != NULL && feScale->isSwapped())
      swap(hRadius, vRadius);

    ctx->blur.hRadius = hRadius;
    ctx->blur.vRadius = vRadius;

    switch (blurType)
    {
      case FE_BLUR_TYPE_BOX:
      default:
        ctx->blur.hConvolve = _api_raster.filter.blur.box.h[srcFormat];
        ctx->blur.vConvolve = _api_raster.filter.blur.box.v[srcFormat];
        break;

      case FE_BLUR_TYPE_STACK:
        ctx->blur.hConvolve = _api_raster.filter.blur.stack.h[srcFormat];
        ctx->blur.vConvolve = _api_raster.filter.blur.stack.v[srcFormat];
        break;

      case FE_BLUR_TYPE_EXPONENTIAL:
        ctx->blur.hConvolve = _api_raster.filter.blur.exponential.h[srcFormat];
        ctx->blur.vConvolve = _api_raster.filter.blur.exponential.v[srcFormat];
        break;
    }

    return ERR_OK;
  }

  // ==========================================================================
  // [Blur - Destroy]
  // ==========================================================================

  static void FOG_FASTCALL destroy(
    RasterFilter* ctx)
  {
    // Just be safe and detect possible NULL pointer dereference.
    ctx->destroy = NULL;
    ctx->doRect = NULL;
    ctx->doLine = NULL;
  }

  // ==========================================================================
  // [Blur - DoSimpleRect]
  // ==========================================================================

  static err_t FOG_FASTCALL doRect(
    RasterFilter* ctx,
    RasterFilterImage* dst, const PointI* dstPos,
    RasterFilterImage* src, const RectI* srcRect,
    MemBuffer* intermediateBuffer)
  {
    FOG_ASSERT(srcRect->x >= 0);
    FOG_ASSERT(srcRect->y >= 0);
    FOG_ASSERT(srcRect->x + srcRect->w <= src->size.w);
    FOG_ASSERT(srcRect->y + srcRect->h <= src->size.h);

    MemBufferTmp<1024> memBufferTmp;
    MemBuffer* memBuffer = &memBufferTmp;

    int i;
    int kernelRadius;
    int kernelSize;

    const ImageFormatDescription& dstDesc = ImageFormatDescription::getByFormat(ctx->dstFormat);
    const ImageFormatDescription& srcDesc = ImageFormatDescription::getByFormat(ctx->srcFormat);

    int intermediateHeight;
    int extendTop;
    int extendBottom;

    ssize_t intermediateStride = 0;
    uint8_t* intermediateData = NULL;

    // Always use 4 bytes of stack storage per pixel when working with 24-bit.
    int stackBpp = srcDesc.getBytesPerPixel();
    if (stackBpp == 3)
      stackBpp = 4;

    RasterFilterBlur blurCtx;
    blurCtx.filterCtx = ctx;

    if (ctx->memBuffer)
      memBuffer = ctx->memBuffer;

    // ------------------------------------------------------------------------
    // [Base]
    // ------------------------------------------------------------------------

    int vRadiusInt = Math::iround(ctx->blur.vRadius);

    extendTop = Math::min(vRadiusInt, srcRect->y);
    extendBottom = Math::min(vRadiusInt, src->size.h - srcRect->y - srcRect->h);

    if ((extendTop | extendBottom) != 0)
    {
      intermediateHeight = srcRect->h + extendTop + extendBottom;
      intermediateStride = srcRect->w * dstDesc.getBytesPerPixel();
      intermediateData = reinterpret_cast<uint8_t*>(intermediateBuffer->alloc(intermediateHeight * intermediateStride));

      if (FOG_IS_NULL(intermediateData))
        return ERR_RT_OUT_OF_MEMORY;
    }

    // Move closer.
    blurCtx.extendType = ctx->blur.extendType;
    blurCtx.extendColor.prgb64.p64 = ctx->blur.extendColor.prgb64.p64;

    // ------------------------------------------------------------------------
    // [Horizontal]
    // ------------------------------------------------------------------------

    kernelRadius = Math::iround(ctx->blur.hRadius);
    kernelSize = kernelRadius * 2 + 1;

    if (intermediateData)
    {
      blurCtx.dstData = intermediateData;
      blurCtx.dstStride = intermediateStride;
    }
    else
    {
      blurCtx.dstData = dst->data + dstPos->y * dst->stride +
                        dstPos->x * (int)dstDesc.getBytesPerPixel();
      blurCtx.dstStride = dst->stride;
    }

    blurCtx.srcData = src->data + (srcRect->y - extendTop) * src->stride;
    blurCtx.srcStride = src->stride;

    i = Math::max(src->size.w - srcRect->x, 0);
    blurCtx.rowSize = srcRect->h + extendTop + extendBottom;

    if (ctx->blur.blurType <= FE_BLUR_TYPE_STACK)
    {
      // BoxBlur and StackBlur use different approach than ExpBlur.
      FOG_ASSERT(ctx->blur.blurType == FE_BLUR_TYPE_BOX ||
                 ctx->blur.blurType == FE_BLUR_TYPE_STACK);

      blurCtx.runSize      = (i > kernelRadius) ? Math::min(i - kernelRadius, srcRect->w) : 1;
      blurCtx.runOffset    = (srcRect->x + kernelRadius + 1) * (int)srcDesc.getBytesPerPixel();

      blurCtx.aTableSize   = kernelSize;
      blurCtx.bTableSize   = Math::min<int>(srcRect->w - blurCtx.runSize, kernelRadius);

      blurCtx.kernelRadius = kernelRadius;
      blurCtx.kernelSize   = kernelSize;
    }
    else
    {
      blurCtx.runSize      = i;
      blurCtx.runOffset    = 0;

      blurCtx.aTableSize   = kernelRadius;
      blurCtx.bTableSize   = kernelRadius;

      blurCtx.kernelRadius = kernelRadius;
      blurCtx.kernelSize   = kernelSize;
    }
  
    blurCtx.srcFirstOffset = 0;
    blurCtx.srcLastOffset  = (src->size.w - 1) * (int)srcDesc.getBytesPerPixel();

    if (memBuffer->alloc((blurCtx.aTableSize + blurCtx.bTableSize) * sizeof(ssize_t) + 
                         (kernelSize * stackBpp)) == NULL)
    {
      return ERR_RT_OUT_OF_MEMORY;
    }

    blurCtx.aTableData = reinterpret_cast<ssize_t*>(memBuffer->getMem());
    blurCtx.bTableData = blurCtx.aTableData + blurCtx.aTableSize;
    blurCtx.stack = reinterpret_cast<uint8_t*>(blurCtx.bTableData + blurCtx.bTableSize);

    if (FOG_IS_NULL(blurCtx.aTableData))
      return ERR_RT_OUT_OF_MEMORY;

    fillSimpleBorderTables(&blurCtx, srcRect->x - kernelRadius, 0, src->size.w - 1, srcDesc.getBytesPerPixel());
  
    if (ctx->blur.blurType <= FE_BLUR_TYPE_STACK)
    {
      FOG_ASSERT(blurCtx.aTableSize + blurCtx.aBorderLeadSize + blurCtx.aBorderTailSize == kernelSize);
      FOG_ASSERT(blurCtx.bTableSize + blurCtx.bBorderTailSize + blurCtx.runSize == srcRect->w);
    }
    else
    {
      FOG_ASSERT(blurCtx.aTableSize + blurCtx.aBorderLeadSize +
                 blurCtx.bTableSize + blurCtx.bBorderTailSize + blurCtx.runSize == srcRect->w + kernelRadius * 2);
    }

    // Logger::debug("Fog::RasterOps_C::FBlur", "doRect", "RectH | dst=[%d %d] src=[%d %d %d %d] a=[lead=%d table=%d tail=%d] run=%d b=[table=%d tail=%d]",
    //   dstPos->x,
    //   dstPos->y,
    //   srcRect->x,
    //   srcRect->y,
    //   srcRect->w,
    //   srcRect->h,
    //   blurCtx.aBorderLeadSize,
    //   blurCtx.aTableSize,
    //   blurCtx.aBorderTailSize,
    //   blurCtx.runSize,
    //   blurCtx.bTableSize,
    //   blurCtx.bBorderTailSize);

    ctx->blur.hConvolve(&blurCtx);

    // ------------------------------------------------------------------------
    // [Vertical]
    // ------------------------------------------------------------------------

    kernelRadius = Math::iround(ctx->blur.vRadius);
    kernelSize = kernelRadius * 2 + 1;

    blurCtx.dstData = dst->data + dstPos->y * dst->stride +
                                  dstPos->x * (int)dstDesc.getBytesPerPixel();
    blurCtx.dstStride = dst->stride;

    if (intermediateData)
    {
      blurCtx.srcData = intermediateData;
      blurCtx.srcStride = intermediateStride;
    }
    else
    {
      blurCtx.srcData = dst->data + dstPos->y * dst->stride + dstPos->x * (int)dstDesc.getBytesPerPixel();
      blurCtx.srcStride = dst->stride;
    }

    i = Math::max(src->size.h - srcRect->y, 0);
    blurCtx.rowSize = srcRect->w;

    if (ctx->blur.blurType <= FE_BLUR_TYPE_STACK)
    {
      // BoxBlur and StackBlur use different approach than ExpBlur.
      FOG_ASSERT(ctx->blur.blurType == FE_BLUR_TYPE_BOX ||
                 ctx->blur.blurType == FE_BLUR_TYPE_STACK);

      blurCtx.runSize      = (i > kernelRadius) ? Math::min(i - kernelRadius, srcRect->h) : 1;
      blurCtx.runOffset    = (extendTop + kernelRadius + 1) * blurCtx.srcStride;

      blurCtx.aTableSize   = kernelSize;
      blurCtx.bTableSize   = Math::min<int>(srcRect->h - blurCtx.runSize, kernelRadius);

      blurCtx.kernelRadius = kernelRadius;
      blurCtx.kernelSize   = kernelSize;
    }
    else
    {
      blurCtx.runSize      = i;
      blurCtx.runOffset    = 0;

      blurCtx.aTableSize   = kernelRadius;
      blurCtx.bTableSize   = kernelRadius;

      blurCtx.kernelRadius = kernelRadius;
      blurCtx.kernelSize   = kernelSize;
    }

    blurCtx.srcFirstOffset = 0;
    blurCtx.srcLastOffset  = (srcRect->h - 1 + extendTop + extendBottom) * blurCtx.srcStride;

    if (memBuffer->alloc((blurCtx.aTableSize + blurCtx.bTableSize) * sizeof(ssize_t) +
                         (kernelSize * stackBpp) * BLUR_RECT_V_HLINE_COUNT) == NULL)
    {
      return ERR_RT_OUT_OF_MEMORY;
    }

    blurCtx.aTableData = reinterpret_cast<ssize_t*>(memBuffer->getMem());
    blurCtx.bTableData = blurCtx.aTableData + blurCtx.aTableSize;
    blurCtx.stack = reinterpret_cast<uint8_t*>(blurCtx.bTableData + blurCtx.bTableSize);

    fillSimpleBorderTables(&blurCtx, extendTop - kernelRadius, 0, srcRect->h - 1 + extendTop + extendBottom, blurCtx.srcStride);

    if (ctx->blur.blurType <= FE_BLUR_TYPE_STACK)
    {
      FOG_ASSERT(blurCtx.aTableSize + blurCtx.aBorderLeadSize + blurCtx.aBorderTailSize == kernelSize);
      FOG_ASSERT(blurCtx.bTableSize + blurCtx.bBorderTailSize + blurCtx.runSize == srcRect->h);
    }
    else
    {
      FOG_ASSERT(blurCtx.aTableSize + blurCtx.aBorderLeadSize +
                 blurCtx.bTableSize + blurCtx.bBorderTailSize + blurCtx.runSize == srcRect->h + kernelRadius * 2);
    }

    // Logger::debug("Fog::RasterOps_C::FBlur", "doRect", "RectV | dst=[%d %d] src=[%d %d %d %d] a=[lead=%d table=%d tail=%d] run=%d b=[table=%d tail=%d]",
    //   dstPos->x,
    //   dstPos->y,
    //   srcRect->x,
    //   srcRect->y,
    //   srcRect->w,
    //   srcRect->h,
    //   blurCtx.aBorderLeadSize,
    //   blurCtx.aTableSize,
    //   blurCtx.aBorderTailSize,
    //   blurCtx.runSize,
    //   blurCtx.bTableSize,
    //   blurCtx.bBorderTailSize);

    ctx->blur.vConvolve(&blurCtx);
    return ERR_OK;
  }

  // ==========================================================================
  // [Blur - DoSimpleRect - FillBorderTables]
  // ==========================================================================

  static void FOG_FASTCALL fillSimpleBorderTables(
    RasterFilterBlur* blurCtx, int t, int tMin, int tMax, ssize_t tMul)
  {
    uint i;
    int tRepeat = tMax - tMin + 1;

    blurCtx->aBorderLeadSize = 0;
    blurCtx->aBorderTailSize = 0;
    blurCtx->bBorderTailSize = 0;

    switch (blurCtx->extendType)
    {
      case FE_EXTEND_COLOR:
      case FE_EXTEND_PAD:
      {
        // Catch each pixel which is outside of the raster and setup sizes of
        // lead and tail borders. We need to decrease size of aTableSize every
        // time we set the border lead/tail size.
        if (t < tMin)
        {
          blurCtx->aBorderLeadSize = tMin - t;
          blurCtx->aTableSize -= blurCtx->aBorderLeadSize;
          t = tMin;
        }
        
        if (t + int(blurCtx->aTableSize) > tMax + 1)
        {
          blurCtx->aBorderTailSize = (t + blurCtx->aTableSize) - (tMax + 1);
          blurCtx->aTableSize -= blurCtx->aBorderTailSize;

          blurCtx->bBorderTailSize = blurCtx->bTableSize;
          blurCtx->bTableSize = 0;
        }
        else 
        {
          int m = t + int(blurCtx->kernelSize) + int(blurCtx->runSize) + int(blurCtx->bTableSize);
          if (m > tMax)
          {
            blurCtx->bBorderTailSize = Math::min<int>(m - tMax, int(blurCtx->bTableSize));
            blurCtx->bTableSize -= blurCtx->bBorderTailSize;
          }
        }

        // Now it's safe to continue using Repeat mode.
        goto _Repeat;
      }

      case FE_EXTEND_REPEAT:
      {
        // Repeat 't'.
        t -= tMin;
        t %= tRepeat;
        if (t < 0)
          t += tRepeat;
        t += tMin;

_Repeat:
        for (i = 0; i < blurCtx->aTableSize; i++)
        {
          blurCtx->aTableData[i] = t * tMul;

          if (++t > tMax)
            t = tMin;
        }

        t += blurCtx->runSize;
        if (t >= tMax)
          t -= tRepeat;

        for (i = 0; i < blurCtx->bTableSize; i++)
        {
          blurCtx->bTableData[i] = t * tMul;

          if (++t > tMax)
            t = tMin;
        }
        break;
      }

      case FE_EXTEND_REFLECT:
      {
        // Reflect 't'.
        int tRepeat2 = tRepeat * 2;
        
        t -= tMin;
        t %= tRepeat2;
        if (t < 0)
          t += tRepeat2;

        for (i = 0; i < blurCtx->aTableSize; i++)
        {
          blurCtx->aTableData[i] = ((t < tRepeat ? t : tRepeat2 - t) + tMin) * tMul;
          if (++t > tRepeat2)
            t = 0;
        }

        t += blurCtx->runSize;
        t %= tRepeat2;

        for (i = 0; i < blurCtx->bTableSize; i++)
        {
          blurCtx->bTableData[i] = ((t < tRepeat ? t : tRepeat2 - t) + tMin) * tMul;
          if (++t > tRepeat2)
            t = 0;
        }
        break;
      }
    }
  }

  // ==========================================================================
  // [Blur - Box - Helpers]
  // ==========================================================================

  //! @brief Get reciprocal for 16-bit value @a val.
  //!
  //! Used to scale SUM of A, R, G, B pixels into the final pixels, which is
  //! stored to the destination buffer.
  static FOG_INLINE int getBoxBlurReciprocal(int val)
  {
    return (val + 65535) / val;
  }

  // ==========================================================================
  // [Blur - Box - Horizontal]
  // ==========================================================================

  template<typename Accessor>
  static void FOG_FASTCALL doBoxH(
    RasterFilterBlur* blurCtx)
  {
    uint8_t* dst = blurCtx->dstData;
    uint8_t* src = blurCtx->srcData;

    ssize_t dstStride = blurCtx->dstStride;
    ssize_t srcStride = blurCtx->srcStride;

    uint runHeight = blurCtx->rowSize;
    uint runSize = blurCtx->runSize;

    uint32_t sumMul = getBoxBlurReciprocal(blurCtx->kernelSize);
    uint32_t sumShr = 16;

    ssize_t* aTableData = blurCtx->aTableData;
    ssize_t* bTableData = blurCtx->bTableData;
    uint8_t* stackBuf = blurCtx->stack;
    uint8_t* stackEnd = stackBuf + blurCtx->kernelSize * Accessor::STACK_BPP;
    
    uint aBorderLeadSize = blurCtx->aBorderLeadSize;
    uint aBorderTailSize = blurCtx->aBorderTailSize;
    uint bBorderTailSize = blurCtx->bBorderTailSize;

    uint aTableSize = blurCtx->aTableSize;
    uint bTableSize = blurCtx->bTableSize;

    uint i, r;
    for (r = 0; r < runHeight; r++)
    {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;

#if defined(FOG_DEBUG)
      // NOTE: Code which uses these variables need to be #ifdefed too,
      // because assertions can be also enabled in release (not normal,
      // but possible to catch different types of bugs). So please put
      // assertions related to bounds checking to #ifdef block.
      uint8_t* srcEnd = src + srcStride;
      uint8_t* dstEnd = dst + dstStride;
#endif // FOG_DEBUG

      uint8_t* stackPtr = stackBuf;

      typename Accessor::Run run;
      typename Accessor::Pixel pix;

      run.reset();

      // ----------------------------------------------------------------------
      // [A-Border - Lead]
      // ----------------------------------------------------------------------

      i = aBorderLeadSize;
      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pix, srcPtr + blurCtx->srcFirstOffset);

        run.add(pix, i);
        do {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::storePixelT(stackPtr, pix);
          stackPtr += Accessor::STACK_BPP;
        } while (--i);
      }

      // ----------------------------------------------------------------------
      // [A-Border - Table]
      // ----------------------------------------------------------------------

      for (i = 0; i < aTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + aTableData[i] >= src && srcPtr + aTableData[i] < srcEnd);
#endif // FOG_DEBUG
        FOG_ASSERT(stackPtr < stackEnd);

        Accessor::fetchPixelM(pix, srcPtr + aTableData[i]);
        Accessor::storePixelT(stackPtr, pix);
        run.add(pix);

        stackPtr += Accessor::STACK_BPP;
      }

      // ----------------------------------------------------------------------
      // [A-Border - Tail]
      // ----------------------------------------------------------------------

      i = aBorderTailSize;
      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pix, srcPtr + blurCtx->srcLastOffset);

        run.add(pix, i);
        do {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::storePixelT(stackPtr, pix);
          stackPtr += Accessor::STACK_BPP;
        } while (--i);
      }

      // ----------------------------------------------------------------------
      // [Run-Loop]
      // ----------------------------------------------------------------------

      FOG_ASSERT(stackPtr == stackEnd);
      FOG_ASSERT(runSize != 0);

      stackPtr = stackBuf;
      srcPtr += blurCtx->runOffset;

      i = runSize;
      goto _First;

      do {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr < srcEnd);
#endif // FOG_DEBUG
        FOG_ASSERT(stackPtr < stackEnd);

        Accessor::fetchPixelT(pix, stackPtr);
        run.sub(pix);

        Accessor::fetchPixelM(pix, srcPtr);
        Accessor::storePixelT(stackPtr, pix);
        run.add(pix);

        srcPtr += Accessor::PIXEL_BPP;
        stackPtr += Accessor::STACK_BPP;

        if (stackPtr == stackEnd)
          stackPtr = stackBuf;

_First:
#if defined(FOG_DEBUG)
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG

        Accessor::storeRunM(dstPtr, run, sumMul, sumShr);
        dstPtr += Accessor::PIXEL_BPP;
      } while (--i);

      // ----------------------------------------------------------------------
      // [B-Border - Table]
      // ----------------------------------------------------------------------

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + bTableData[i] >= src && srcPtr + bTableData[i] < srcEnd);
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG
        FOG_ASSERT(stackPtr < stackEnd);

        Accessor::fetchPixelT(pix, stackPtr);
        run.sub(pix);

        Accessor::fetchPixelM(pix, srcPtr + bTableData[i]);
        Accessor::storePixelT(stackPtr, pix);
        run.add(pix);
        Accessor::storeRunM(dstPtr, run, sumMul, sumShr);

        stackPtr += Accessor::STACK_BPP;
        dstPtr += Accessor::PIXEL_BPP;

        if (stackPtr == stackEnd)
          stackPtr = stackBuf;
      }

      // ----------------------------------------------------------------------
      // [B-Border - Tail]
      // ----------------------------------------------------------------------

      i = bBorderTailSize;
      if (i != 0)
      {
        typename Accessor::Pixel pixB;
        typename Accessor::Run cmpB;

        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pixB, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pixB, srcPtr + blurCtx->srcLastOffset);

        cmpB.set(pixB);
        do {
#if defined(FOG_DEBUG)
          FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::fetchPixelT(pix, stackPtr);
          Accessor::storePixelT(stackPtr, pixB);
          run.sub(pix);
          run.add(cmpB);

          Accessor::storeRunM(dstPtr, run, sumMul, sumShr);

          dstPtr += Accessor::PIXEL_BPP;
          stackPtr += Accessor::STACK_BPP;

          if (stackPtr == stackEnd)
            stackPtr = stackBuf;
        } while (--i);
      }

      dst += dstStride;
      src += srcStride;
    }
  }

  // ==========================================================================
  // [Blur - Box - Vertical]
  // ==========================================================================

  template<typename Accessor>
  static void FOG_FASTCALL doBoxV(
    RasterFilterBlur* blurCtx)
  {
    uint8_t* dst = blurCtx->dstData;
    uint8_t* src = blurCtx->srcData;

    ssize_t dstStride = blurCtx->dstStride;
    ssize_t srcStride = blurCtx->srcStride;

    uint runWidth = blurCtx->rowSize;
    uint runSize = blurCtx->runSize;

    uint32_t sumMul = getBoxBlurReciprocal(blurCtx->kernelSize);
    uint32_t sumShr = 16;

    ssize_t* aTableData = blurCtx->aTableData;
    ssize_t* bTableData = blurCtx->bTableData;
    uint8_t* stackBuf = blurCtx->stack;
    
    uint aBorderLeadSize = blurCtx->aBorderLeadSize;
    uint aBorderTailSize = blurCtx->aBorderTailSize;
    uint bBorderTailSize = blurCtx->bBorderTailSize;

    uint aTableSize = blurCtx->aTableSize;
    uint bTableSize = blurCtx->bTableSize;

    uint i, r = 0;

    do {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;
      uint8_t* stackPtr = stackBuf;

      typename Accessor::Run run[BLUR_RECT_V_HLINE_COUNT];
      typename Accessor::Pixel pix;

      uint xLength = Math::min<uint>(runWidth - r, BLUR_RECT_V_HLINE_COUNT);
      uint x;

      uint8_t* stackEnd = stackBuf + blurCtx->kernelSize * xLength * Accessor::STACK_BPP;

      // ----------------------------------------------------------------------
      // [A-Border - Lead]
      // ----------------------------------------------------------------------

      i = aBorderLeadSize;
      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
        {
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
          run[0].reset();
          run[0].add(pix, i);

          for (x = 1; x < xLength; x++)
          {
            run[x].set(run[0]);
          }

          i *= xLength;
          do {
            FOG_ASSERT(stackPtr < stackEnd);

            Accessor::storePixelT(stackPtr, pix);
            stackPtr += Accessor::STACK_BPP;
          } while (--i);
        }
        else
        {
          uint8_t* srcBase = srcPtr + blurCtx->srcFirstOffset;
          uint8_t* stackBase = stackPtr;

          for (x = 0; x < xLength; x++)
          {
            FOG_ASSERT(stackPtr < stackEnd);

            Accessor::fetchPixelM(pix, srcBase + x * Accessor::PIXEL_BPP);
            run[x].reset();
            run[x].add(pix, i);

            Accessor::storePixelT(stackPtr, pix);
            stackPtr += Accessor::STACK_BPP;
          }

          while (--i)
          {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackPtr < stackEnd);

              Accessor::fetchPixelT(pix, stackBase + x * Accessor::STACK_BPP);
              Accessor::storePixelT(stackPtr, pix);

              stackPtr += Accessor::STACK_BPP;
            }
          }
        }
      }
      else
      {
        for (x = 0; x < xLength; x++)
        {
          run[x].reset();
        }
      }

      // ----------------------------------------------------------------------
      // [A-Border - Table]
      // ----------------------------------------------------------------------

      for (i = 0; i < aTableSize; i++)
      {
        uint8_t* srcTab = srcPtr + aTableData[i];

        for (x = 0; x < xLength; x++)
        {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::fetchPixelM(pix, srcTab + x * Accessor::PIXEL_BPP);
          Accessor::storePixelT(stackPtr, pix);
          run[x].add(pix);

          stackPtr += Accessor::STACK_BPP;
        }
      }

      // ----------------------------------------------------------------------
      // [A-Border - Tail]
      // ----------------------------------------------------------------------

      i = aBorderTailSize;
      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
        {
          Accessor::fetchPixelS(pix, blurCtx->extendColor);

          for (x = 0; x < xLength; x++)
          {
            run[x].add(pix, i);
          }

          i *= xLength;
          do {
            FOG_ASSERT(stackPtr < stackEnd);

            Accessor::storePixelT(stackPtr, pix);
            stackPtr += Accessor::STACK_BPP;
          } while (--i);
        }
        else
        {
          uint8_t* srcBase = srcPtr + blurCtx->srcLastOffset;
          uint8_t* stackBase = stackPtr;

          for (x = 0; x < xLength; x++)
          {
            FOG_ASSERT(stackPtr < stackEnd);

            Accessor::fetchPixelM(pix, srcBase + x * Accessor::PIXEL_BPP);
            Accessor::storePixelT(stackPtr, pix);
            run[x].add(pix, i);

            stackPtr += Accessor::STACK_BPP;
          }

          while (--i)
          {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackPtr < stackEnd);

              Accessor::fetchPixelT(pix, stackBase + x * Accessor::STACK_BPP);
              Accessor::storePixelT(stackPtr, pix);

              stackPtr += Accessor::STACK_BPP;
            }
          }
        }
      }

      // ----------------------------------------------------------------------
      // [Run-Loop]
      // ----------------------------------------------------------------------

      FOG_ASSERT(stackPtr == stackEnd);
      FOG_ASSERT(runSize != 0);

      stackPtr = stackBuf;
      srcPtr += blurCtx->runOffset;

      for (x = 0; x < xLength; x++)
      {
        Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);
      }

      dstPtr += dstStride;
      i = runSize;

      while (--i)
      {
        for (x = 0; x < xLength; x++)
        {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::fetchPixelT(pix, stackPtr);
          run[x].sub(pix);

          Accessor::fetchPixelM(pix, srcPtr + x * Accessor::PIXEL_BPP);
          Accessor::storePixelT(stackPtr, pix);
          run[x].add(pix);

          Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);
          stackPtr += Accessor::STACK_BPP;
        }

        if (stackPtr == stackEnd)
          stackPtr = stackBuf;

        srcPtr += srcStride;
        dstPtr += dstStride;
      }
      
      // ----------------------------------------------------------------------
      // [B-Border - Table]
      // ----------------------------------------------------------------------

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
        uint8_t* srcBase = srcPtr + bTableData[i];

        for (x = 0; x < xLength; x++)
        {
          FOG_ASSERT(stackPtr < stackEnd);

          Accessor::fetchPixelT(pix, stackPtr);
          run[x].sub(pix);

          Accessor::fetchPixelM(pix, srcBase + x * Accessor::PIXEL_BPP);
          Accessor::storePixelT(stackPtr, pix);
          run[x].add(pix);

          Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);
          stackPtr += Accessor::STACK_BPP;
        }

        if (stackPtr == stackEnd)
          stackPtr = stackBuf;

        dstPtr += dstStride;
      }

      // ----------------------------------------------------------------------
      // [B-Border - Tail]
      // ----------------------------------------------------------------------

      i = bBorderTailSize;
      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
        {
          typename Accessor::Pixel pixB;
          typename Accessor::Run cmpB;

          Accessor::fetchPixelS(pixB, blurCtx->extendColor);
          cmpB.set(pixB);

          do {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackPtr < stackEnd);

              Accessor::fetchPixelT(pix, stackPtr);
              Accessor::storePixelT(stackPtr, pixB);
              run[x].sub(pix);
              run[x].add(cmpB);

              Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);
              stackPtr += Accessor::STACK_BPP;
            }

            if (stackPtr == stackEnd)
              stackPtr = stackBuf;

            dstPtr += dstStride;
          } while (--i);
        }
        else
        {
          uint8_t* srcBase = srcPtr + blurCtx->srcLastOffset;
          
          do {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackPtr < stackEnd);

              Accessor::fetchPixelT(pix, stackPtr);
              run[x].sub(pix);

              Accessor::fetchPixelM(pix, srcBase + x * Accessor::PIXEL_BPP);
              Accessor::storePixelT(stackPtr, pix);
              run[x].add(pix);

              Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);
              stackPtr += Accessor::STACK_BPP;
            }

            if (stackPtr == stackEnd)
              stackPtr = stackBuf;

            dstPtr += dstStride;
          } while (--i);
        }
      }

      dst += xLength * Accessor::PIXEL_BPP;
      src += xLength * Accessor::PIXEL_BPP;

      r += xLength;
    } while (r < runWidth);
  }

  // ==========================================================================
  // [Blur - Stack - Horizontal]
  // ==========================================================================

  static FOG_INLINE uint getStackBlurALeadConst(uint sz)
  {
    uint i = sz;
    while (sz)
    {
      i += --sz;
    }
    return i;
  }

  template<typename Accessor>
  static void FOG_FASTCALL doStackH(
    RasterFilterBlur* blurCtx)
  {
    uint8_t* dst = blurCtx->dstData;
    uint8_t* src = blurCtx->srcData;

    ssize_t dstStride = blurCtx->dstStride;
    ssize_t srcStride = blurCtx->srcStride;

    uint runHeight = blurCtx->rowSize;
    uint runSize = blurCtx->runSize;

    uint32_t sumMul = _raster_blur_stack_8_mul[blurCtx->kernelRadius];
    uint32_t sumShr = _raster_blur_stack_8_shr[blurCtx->kernelRadius];

    ssize_t* aTableData = blurCtx->aTableData;
    ssize_t* bTableData = blurCtx->bTableData;
    uint8_t* stackBuf = blurCtx->stack;
    uint8_t* stackEnd = stackBuf + blurCtx->kernelSize * Accessor::STACK_BPP;
    
    uint aBorderLeadSize = blurCtx->aBorderLeadSize;
    uint aBorderLeadMul = getStackBlurALeadConst(aBorderLeadSize);

    uint aBorderTailSize = blurCtx->aBorderTailSize;
    uint aBorderTailMul = getStackBlurALeadConst(aBorderTailSize);

    uint bBorderTailSize = blurCtx->bBorderTailSize;

    uint aTableSize = blurCtx->aTableSize;
    uint bTableSize = blurCtx->bTableSize;

    uint kernelRadius = blurCtx->kernelRadius;
    uint kernelSize = blurCtx->kernelSize;

    uint i, r;
    for (r = 0; r < runHeight; r++)
    {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;

#if defined(FOG_DEBUG)
      // NOTE: Code which uses these variables need to be #ifdefed too,
      // because assertions can be also enabled in release (not normal,
      // but possible to catch different types of bugs). So please put
      // assertions related to bounds checking to #ifdef block.
      uint8_t* srcEnd = src + srcStride;
      uint8_t* dstEnd = dst + dstStride;
#endif // FOG_DEBUG

      uint8_t* stackA = stackBuf;
      uint8_t* stackB = NULL;

      typename Accessor::Run run;
      typename Accessor::Run runA;
      typename Accessor::Run runB;

      typename Accessor::Pixel pix;
      typename Accessor::Run cmp0;

      run.reset();
      runA.reset();
      runB.reset();

      // ----------------------------------------------------------------------
      // [A-Border - Lead]
      // ----------------------------------------------------------------------

      uint32_t pos = 0;
      i = aBorderLeadSize;

      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pix, srcPtr + blurCtx->srcFirstOffset);

        cmp0.set(pix);
        runB.add(cmp0, i);
        run.add(cmp0, aBorderLeadMul);

        pos += i;
        do {
          FOG_ASSERT(stackA < stackEnd);

          Accessor::storePixelT(stackA, pix);
          stackA += Accessor::STACK_BPP;
        } while (--i);
      }

      // ----------------------------------------------------------------------
      // [A-Border - Table]
      // ----------------------------------------------------------------------

      for (i = 0; i < aTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + aTableData[i] >= src && srcPtr + aTableData[i] < srcEnd);
#endif // FOG_DEBUG
        FOG_ASSERT(stackA < stackEnd);

        Accessor::fetchPixelM(pix, srcPtr + aTableData[i]);
        Accessor::storePixelT(stackA, pix);
        
        cmp0.set(pix);
        stackA += Accessor::STACK_BPP;

        if (pos > kernelRadius)
        {
          runA.add(cmp0);
          run.add(cmp0, kernelSize - pos);
          pos++;
        }
        else
        {
          pos++;
          runB.add(cmp0);
          run.add(cmp0, pos);
        }
      }

      // ----------------------------------------------------------------------
      // [A-Border - Tail]
      // ----------------------------------------------------------------------

      i = aBorderTailSize;
      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pix, srcPtr + blurCtx->srcLastOffset);
        
        cmp0.set(pix);
        runA.add(cmp0, i);
        run.add(cmp0, aBorderTailMul);

        do {
          FOG_ASSERT(stackA < stackEnd);

          Accessor::storePixelT(stackA, pix);
          stackA += Accessor::STACK_BPP;
        } while (--i);
      }

      // ----------------------------------------------------------------------
      // [Run-Loop]
      // ----------------------------------------------------------------------

      FOG_ASSERT(stackA == stackEnd);
      FOG_ASSERT(runSize != 0);

      stackA = stackBuf;
      stackB = stackBuf + (kernelRadius + 1) * Accessor::STACK_BPP;
      srcPtr += blurCtx->runOffset;

      i = runSize;
      goto _First;

      do {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr < srcEnd);
#endif // FOG_DEBUG
        FOG_ASSERT(stackA < stackEnd);
        FOG_ASSERT(stackB < stackEnd);

        Accessor::fetchPixelT(pix, stackA);
        run.sub(runB);
        runB.sub(pix);

        Accessor::fetchPixelM(pix, srcPtr);
        Accessor::storePixelT(stackA, pix);
        runA.add(pix);
        run.add(runA);

        Accessor::fetchPixelT(pix, stackB);
        cmp0.set(pix);
        runB.add(cmp0);
        runA.sub(cmp0);

        srcPtr += Accessor::PIXEL_BPP;
        stackA += Accessor::STACK_BPP;
        stackB += Accessor::STACK_BPP;

        if (stackA == stackEnd)
          stackA = stackBuf;
        if (stackB == stackEnd)
          stackB = stackBuf;

_First:
#if defined(FOG_DEBUG)
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG

        Accessor::storeRunM(dstPtr, run, sumMul, sumShr);
        dstPtr += Accessor::PIXEL_BPP;
      } while (--i);

      // ----------------------------------------------------------------------
      // [B-Border - Table]
      // ----------------------------------------------------------------------

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + bTableData[i] >= src && srcPtr + bTableData[i] < srcEnd);
#endif // FOG_DEBUG
        FOG_ASSERT(stackA < stackEnd);
        FOG_ASSERT(stackB < stackEnd);

        Accessor::fetchPixelT(pix, stackA);
        run.sub(runB);
        runB.sub(pix);

        Accessor::fetchPixelM(pix, srcPtr + bTableData[i]);
        Accessor::storePixelT(stackA, pix);
        runA.add(pix);
        run.add(runA);

        Accessor::fetchPixelT(pix, stackB);
        cmp0.set(pix);
        runB.add(cmp0);
        runA.sub(cmp0);

        srcPtr += Accessor::PIXEL_BPP;
        stackA += Accessor::STACK_BPP;
        stackB += Accessor::STACK_BPP;

        if (stackA == stackEnd)
          stackA = stackBuf;
        if (stackB == stackEnd)
          stackB = stackBuf;

#if defined(FOG_DEBUG)
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG

        Accessor::storeRunM(dstPtr, run, sumMul, sumShr);
        dstPtr += Accessor::PIXEL_BPP;
      }

      // ----------------------------------------------------------------------
      // [B-Border - Tail]
      // ----------------------------------------------------------------------

      i = bBorderTailSize;
      if (i != 0)
      {
        typename Accessor::Pixel pixB;
        typename Accessor::Run cmpB;

        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pixB, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pixB, srcPtr + blurCtx->srcLastOffset);

        cmpB.set(pixB);
        do {
          FOG_ASSERT(stackA < stackEnd);
          FOG_ASSERT(stackB < stackEnd);

          Accessor::fetchPixelT(pix, stackA);
          run.sub(runB);
          runB.sub(pix);

          Accessor::storePixelT(stackA, pixB);
          runA.add(cmpB);
          run.add(runA);

          Accessor::fetchPixelT(pix, stackB);
          cmp0.set(pix);
          runB.add(cmp0);
          runA.sub(cmp0);

          srcPtr += Accessor::PIXEL_BPP;
          stackA += Accessor::STACK_BPP;
          stackB += Accessor::STACK_BPP;

          if (stackA == stackEnd)
            stackA = stackBuf;
          if (stackB == stackEnd)
            stackB = stackBuf;

#if defined(FOG_DEBUG)
          FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG

          Accessor::storeRunM(dstPtr, run, sumMul, sumShr);
          dstPtr += Accessor::PIXEL_BPP;
        } while (--i);
      }

      dst += dstStride;
      src += srcStride;
    }
  }

  // ==========================================================================
  // [Blur - Stack - Vertical]
  // ==========================================================================

  template<typename Accessor>
  static void FOG_FASTCALL doStackV(
    RasterFilterBlur* blurCtx)
  {
    uint8_t* dst = blurCtx->dstData;
    uint8_t* src = blurCtx->srcData;

    ssize_t dstStride = blurCtx->dstStride;
    ssize_t srcStride = blurCtx->srcStride;

    uint runWidth = blurCtx->rowSize;
    uint runSize = blurCtx->runSize;

    uint32_t sumMul = _raster_blur_stack_8_mul[blurCtx->kernelRadius];
    uint32_t sumShr = _raster_blur_stack_8_shr[blurCtx->kernelRadius];

    ssize_t* aTableData = blurCtx->aTableData;
    ssize_t* bTableData = blurCtx->bTableData;
    uint8_t* stackBuf = blurCtx->stack;
    
    uint aBorderLeadSize = blurCtx->aBorderLeadSize;
    uint aBorderLeadMul = getStackBlurALeadConst(aBorderLeadSize);

    uint aBorderTailSize = blurCtx->aBorderTailSize;
    uint aBorderTailMul = getStackBlurALeadConst(aBorderTailSize);

    uint bBorderTailSize = blurCtx->bBorderTailSize;

    uint aTableSize = blurCtx->aTableSize;
    uint bTableSize = blurCtx->bTableSize;

    uint kernelRadius = blurCtx->kernelRadius;
    uint kernelSize = blurCtx->kernelSize;

    uint i, r = 0;

    do {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;

      uint8_t* stackA = stackBuf;
      uint8_t* stackB = NULL;

      typename Accessor::Run run[BLUR_RECT_V_HLINE_COUNT];
      typename Accessor::Run runA[BLUR_RECT_V_HLINE_COUNT];
      typename Accessor::Run runB[BLUR_RECT_V_HLINE_COUNT];

      typename Accessor::Pixel pix;
      typename Accessor::Run cmp0;

      uint xLength = Math::min<uint>(runWidth - r, BLUR_RECT_V_HLINE_COUNT);
      uint x;

      uint8_t* stackEnd = stackBuf + blurCtx->kernelSize * xLength * Accessor::STACK_BPP;

      for (x = 0; x < xLength; x++)
      {
        run[x].reset();
        runA[x].reset();
        runB[x].reset();
      }

      // ----------------------------------------------------------------------
      // [A-Border - Lead]
      // ----------------------------------------------------------------------

      uint32_t pos = 0;
      i = aBorderLeadSize;

      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
        {
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
          
          cmp0.set(pix);
          runB[0].add(cmp0, i);
          run[0].add(cmp0, aBorderLeadMul);

          for (x = 1; x < xLength; x++)
          {
            runB[x].set(runB[0]);
            run[x].set(run[0]);
          }

          pos += i;
          i *= xLength;

          do {
            FOG_ASSERT(stackA < stackEnd);

            Accessor::storePixelT(stackA, pix);
            stackA += Accessor::STACK_BPP;
          } while (--i);
        }
        else
        {
          uint8_t* srcBase = srcPtr + blurCtx->srcFirstOffset;
          uint8_t* stackBase = stackA;

          for (x = 0; x < xLength; x++)
          {
            FOG_ASSERT(stackA < stackEnd);

            Accessor::fetchPixelM(pix, srcBase + x * Accessor::PIXEL_BPP);
            cmp0.set(pix);

            runB[x].add(cmp0, i);
            run[x].add(cmp0, aBorderLeadMul);

            Accessor::storePixelT(stackA, pix);
            stackA += Accessor::STACK_BPP;
          }

          while (--i)
          {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackA < stackEnd);

              Accessor::fetchPixelT(pix, stackBase + x * Accessor::STACK_BPP);
              Accessor::storePixelT(stackA, pix);
              stackA += Accessor::STACK_BPP;
            }
          }
        }
      }

      // ----------------------------------------------------------------------
      // [A-Border - Table]
      // ----------------------------------------------------------------------

      for (i = 0; i < aTableSize; i++)
      {
        uint8_t* srcTab = srcPtr + aTableData[i];

        for (x = 0; x < xLength; x++)
        {
          FOG_ASSERT(stackA < stackEnd);

          Accessor::fetchPixelM(pix, srcTab + x * Accessor::PIXEL_BPP);
          Accessor::storePixelT(stackA, pix);
          
          cmp0.set(pix);
          stackA += Accessor::STACK_BPP;

          if (pos > kernelRadius)
          {
            runA[x].add(cmp0);
            run[x].add(cmp0, kernelSize - pos);
          }
          else
          {
            runB[x].add(cmp0);
            run[x].add(cmp0, pos + 1);
          }
        }
        pos++;
      }

      // ----------------------------------------------------------------------
      // [A-Border - Tail]
      // ----------------------------------------------------------------------

      i = aBorderTailSize;
      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
        {
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
          cmp0.set(pix);

          for (x = 0; x < xLength; x++)
          {
            runA[x].add(cmp0, i);
            run[x].add(cmp0, aBorderTailMul);
          }

          i *= xLength;
          do {
            FOG_ASSERT(stackA < stackEnd);

            Accessor::storePixelT(stackA, pix);
            stackA += Accessor::STACK_BPP;
          } while (--i);
        }
        else
        {
          uint8_t* srcBase = srcPtr + blurCtx->srcLastOffset;
          uint8_t* stackBase = stackA;

          for (x = 0; x < xLength; x++)
          {
            FOG_ASSERT(stackA < stackEnd);

            Accessor::fetchPixelM(pix, srcBase + x * Accessor::PIXEL_BPP);
            cmp0.set(pix);

            runA[x].add(cmp0, i);
            run[x].add(cmp0, aBorderTailMul);

            Accessor::storePixelT(stackA, pix);
            stackA += Accessor::STACK_BPP;
          }

          while (--i)
          {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackA < stackEnd);

              Accessor::fetchPixelT(pix, stackBase + x * Accessor::STACK_BPP);
              Accessor::storePixelT(stackA, pix);
              stackA += Accessor::STACK_BPP;
            }
          }
        }
      }

      // ----------------------------------------------------------------------
      // [Run-Loop]
      // ----------------------------------------------------------------------

      FOG_ASSERT(stackA == stackEnd);
      FOG_ASSERT(runSize != 0);

      stackA = stackBuf;
      stackB = stackBuf + (kernelRadius + 1) * xLength * Accessor::STACK_BPP;
      srcPtr += blurCtx->runOffset;

      for (x = 0; x < xLength; x++)
      {
        Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);
      }

      dstPtr += dstStride;
      i = runSize;

      while (--i)
      {
        for (x = 0; x < xLength; x++)
        {
          FOG_ASSERT(stackA < stackEnd);
          FOG_ASSERT(stackB < stackEnd);

          Accessor::fetchPixelT(pix, stackA);
          run[x].sub(runB[x]);
          runB[x].sub(pix);

          Accessor::fetchPixelM(pix, srcPtr + x * Accessor::PIXEL_BPP);
          Accessor::storePixelT(stackA, pix);
          runA[x].add(pix);
          run[x].add(runA[x]);

          Accessor::fetchPixelT(pix, stackB);
          cmp0.set(pix);
          runB[x].add(cmp0);
          runA[x].sub(cmp0);

          Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);
          stackA += Accessor::STACK_BPP;
          stackB += Accessor::STACK_BPP;
        }

        if (stackA == stackEnd)
          stackA = stackBuf;
        if (stackB == stackEnd)
          stackB = stackBuf;

        srcPtr += srcStride;
        dstPtr += dstStride;
      }

      // ----------------------------------------------------------------------
      // [B-Border - Table]
      // ----------------------------------------------------------------------

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
        uint8_t* srcBase = srcPtr + bTableData[i];

        for (x = 0; x < xLength; x++)
        {
          FOG_ASSERT(stackA < stackEnd);
          FOG_ASSERT(stackB < stackEnd);

          Accessor::fetchPixelT(pix, stackA);
          run[x].sub(runB[x]);
          runB[x].sub(pix);

          Accessor::fetchPixelM(pix, srcBase + x * Accessor::PIXEL_BPP);
          Accessor::storePixelT(stackA, pix);
          runA[x].add(pix);
          run[x].add(runA[x]);

          Accessor::fetchPixelT(pix, stackB);
          cmp0.set(pix);
          runB[x].add(cmp0);
          runA[x].sub(cmp0);

          Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);
          srcPtr += Accessor::PIXEL_BPP;
          stackA += Accessor::STACK_BPP;
          stackB += Accessor::STACK_BPP;
        }

        if (stackA == stackEnd)
          stackA = stackBuf;
        if (stackB == stackEnd)
          stackB = stackBuf;

        dstPtr += dstStride;
      }

      // ----------------------------------------------------------------------
      // [B-Border - Tail]
      // ----------------------------------------------------------------------

      i = bBorderTailSize;
      if (i != 0)
      {
        if (blurCtx->extendType == FE_EXTEND_COLOR)
        {
          typename Accessor::Pixel pixB;
          typename Accessor::Run cmpB;

          Accessor::fetchPixelS(pixB, blurCtx->extendColor);
          cmpB.set(pixB);

          do {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackA < stackEnd);
              FOG_ASSERT(stackB < stackEnd);

              Accessor::fetchPixelT(pix, stackA);
              run[x].sub(runB[x]);
              runB[x].sub(pix);

              Accessor::storePixelT(stackA, pixB);
              runA[x].add(cmpB);
              run[x].add(runA[x]);

              Accessor::fetchPixelT(pix, stackB);
              cmp0.set(pix);
              runB[x].add(cmp0);
              runA[x].sub(cmp0);

              Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);
              srcPtr += Accessor::PIXEL_BPP;
              stackA += Accessor::STACK_BPP;
              stackB += Accessor::STACK_BPP;
            }

            if (stackA == stackEnd)
              stackA = stackBuf;
            if (stackB == stackEnd)
              stackB = stackBuf;

            dstPtr += dstStride;
          } while (--i);
        }
        else
        {
          uint8_t* srcBase = srcPtr + blurCtx->srcLastOffset;
          
          do {
            for (x = 0; x < xLength; x++)
            {
              FOG_ASSERT(stackA < stackEnd);
              FOG_ASSERT(stackB < stackEnd);

              Accessor::fetchPixelT(pix, stackA);
              run[x].sub(runB[x]);
              runB[x].sub(pix);

              Accessor::fetchPixelM(pix, srcBase + x * Accessor::PIXEL_BPP);
              Accessor::storePixelT(stackA, pix);
              runA[x].add(pix);
              run[x].add(runA[x]);

              Accessor::fetchPixelT(pix, stackB);
              cmp0.set(pix);
              runB[x].add(cmp0);
              runA[x].sub(cmp0);

              Accessor::storeRunM(dstPtr + x * Accessor::PIXEL_BPP, run[x], sumMul, sumShr);

              srcPtr += Accessor::PIXEL_BPP;
              stackA += Accessor::STACK_BPP;
              stackB += Accessor::STACK_BPP;
            }

            if (stackA == stackEnd)
              stackA = stackBuf;
            if (stackB == stackEnd)
              stackB = stackBuf;

            dstPtr += dstStride;
          } while (--i);
        }
      }

      dst += xLength * Accessor::PIXEL_BPP;
      src += xLength * Accessor::PIXEL_BPP;

      r += xLength;
    } while (r < runWidth);
  }

  // ==========================================================================
  // [Blur - Exponential - Horizontal]
  // ==========================================================================

  template<typename Accessor>
  static void FOG_FASTCALL doExpH(
    RasterFilterBlur* blurCtx)
  {
    uint8_t* dst = blurCtx->dstData;
    uint8_t* src = blurCtx->srcData;

    ssize_t dstStride = blurCtx->dstStride;
    ssize_t srcStride = blurCtx->srcStride;

    uint runHeight = blurCtx->rowSize;
    uint runSize = blurCtx->runSize;

    int aValue = (int)(float(1 << BLUR_APREC) * (1.0f - Math::exp(-2.3f / (blurCtx->filterCtx->blur.hRadius + 1.0f))));

    ssize_t* aTableData = blurCtx->aTableData;
    ssize_t* bTableData = blurCtx->bTableData;

    uint8_t* stackBuf = blurCtx->stack;                               

    uint aBorderSize = blurCtx->aBorderLeadSize;
    uint bBorderSize = blurCtx->bBorderTailSize;

    uint aTableSize = blurCtx->aTableSize;
    uint bTableSize = blurCtx->bTableSize;

    uint i, r;
    for (r = 0; r < runHeight; r++)
    {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;

#if defined(FOG_DEBUG)
      // NOTE: Code which uses these variables need to be #ifdefed too,
      // because assertions can be also enabled in release (not normal,
      // but possible to catch different types of bugs). So please put
      // assertions related to bounds checking to #ifdef block.
      uint8_t* srcEnd = src + srcStride;
      uint8_t* dstEnd = dst + dstStride;
#endif // FOG_DEBUG

      uint8_t* stackPtr = stackBuf;

      typename Accessor::Run run;
      typename Accessor::Pixel pix;

      run.reset();

      // ----------------------------------------------------------------------
      // [A-Border - Lead]
      // ----------------------------------------------------------------------

      i = aBorderSize;
      if (i != 0)
      {
        typename Accessor::Run border;
        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pix, srcPtr + blurCtx->srcFirstOffset);

        border.set(pix);
        do {
          Accessor::blurPixel(run, border, aValue);
        } while (--i);
      }

      // ----------------------------------------------------------------------
      // [A-Border - Table]
      // ----------------------------------------------------------------------

      for (i = 0; i < aTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + aTableData[i] >= src && srcPtr + aTableData[i] < srcEnd);
#endif // FOG_DEBUG

        Accessor::blurRunM(run, srcPtr + aTableData[i], aValue);
      }

      // ----------------------------------------------------------------------
      // [Run-Loop]
      // ----------------------------------------------------------------------

      FOG_ASSERT(runSize != 0);
      i = runSize;

      do {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr < srcEnd);
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG

        Accessor::blurRunM(run, srcPtr, aValue);
        Accessor::storeRunM(dstPtr, run);

        srcPtr += Accessor::PIXEL_BPP;
        dstPtr += Accessor::PIXEL_BPP;
      } while (--i);

      // ----------------------------------------------------------------------
      // [B-Border - Table]
      // ----------------------------------------------------------------------

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr + bTableData[i] >= src && srcPtr + bTableData[i] < srcEnd);
#endif // FOG_DEBUG

        Accessor::blurRunM(run, srcPtr + bTableData[i], aValue);
        Accessor::storeRunT(stackPtr, run);

        stackPtr += Accessor::STACK_BPP;
      }

      // ----------------------------------------------------------------------
      // [B-Border - Tail]
      // ----------------------------------------------------------------------

      i = bBorderSize;
      if (i != 0)
      {
        typename Accessor::Run border;

        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pix, srcPtr + blurCtx->srcLastOffset);

        border.set(pix);
        do {
          Accessor::blurPixel(run, border, aValue);
          Accessor::storeRunT(stackPtr, run);
          stackPtr += Accessor::STACK_BPP;
        } while (--i);
      }

      // ----------------------------------------------------------------------
      // [B-Border - Secondary]
      // ----------------------------------------------------------------------

      for (i = bTableSize + bBorderSize; i; i--)
      {
        stackPtr -= Accessor::STACK_BPP;
        Accessor::blurRunT(run, stackPtr, aValue);
      }

      // ----------------------------------------------------------------------
      // [Run-Loop - Secondary]
      // ----------------------------------------------------------------------

      FOG_ASSERT(runSize != 0);
      i = runSize;

      do {
        dstPtr -= Accessor::PIXEL_BPP;

#if defined(FOG_DEBUG)
        FOG_ASSERT(srcPtr < srcEnd);
        FOG_ASSERT(dstPtr < dstEnd);
#endif // FOG_DEBUG

        Accessor::blurRunM(run, dstPtr, aValue);
        Accessor::storeRunM(dstPtr, run);
      } while (--i);

      dst += dstStride;
      src += srcStride;
    }
  }

  // ==========================================================================
  // [Blur - Exponential - Vertical]
  // ==========================================================================

  template<typename Accessor>
  static void FOG_FASTCALL doExpV(
    RasterFilterBlur* blurCtx)
  {
    uint8_t* dst = blurCtx->dstData;
    uint8_t* src = blurCtx->srcData;

    ssize_t dstStride = blurCtx->dstStride;
    ssize_t srcStride = blurCtx->srcStride;

    uint runHeight = blurCtx->rowSize;
    uint runSize = blurCtx->runSize;

    int aValue = (int)(float(1 << BLUR_APREC) * (1.0f - Math::exp(-2.3f / (blurCtx->filterCtx->blur.vRadius + 1.0f))));

    ssize_t* aTableData = blurCtx->aTableData;
    ssize_t* bTableData = blurCtx->bTableData;

    uint8_t* stackBuf = blurCtx->stack;                               

    uint aBorderSize = blurCtx->aBorderLeadSize;
    uint bBorderSize = blurCtx->bBorderTailSize;

    uint aTableSize = blurCtx->aTableSize;
    uint bTableSize = blurCtx->bTableSize;

    uint i, r;
    for (r = 0; r < runHeight; r++)
    {
      uint8_t* dstPtr = dst;
      uint8_t* srcPtr = src;

      uint8_t* stackPtr = stackBuf;

      typename Accessor::Run run;
      typename Accessor::Pixel pix;

      run.reset();

      // ----------------------------------------------------------------------
      // [A-Border - Lead]
      // ----------------------------------------------------------------------

      i = aBorderSize;
      if (i != 0)
      {
        typename Accessor::Run border;
        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pix, srcPtr + blurCtx->srcFirstOffset);

        border.set(pix);
        do {
          Accessor::blurPixel(run, border, aValue);
        } while (--i);
      }

      // ----------------------------------------------------------------------
      // [A-Border - Table]
      // ----------------------------------------------------------------------

      for (i = 0; i < aTableSize; i++)
      {
        Accessor::blurRunM(run, srcPtr + aTableData[i], aValue);
      }

      // ----------------------------------------------------------------------
      // [Run-Loop]
      // ----------------------------------------------------------------------

      FOG_ASSERT(runSize != 0);
      i = runSize;

      do {
        Accessor::blurRunM(run, srcPtr, aValue);
        Accessor::storeRunM(dstPtr, run);

        srcPtr += srcStride;
        dstPtr += dstStride;
      } while (--i);

      // ----------------------------------------------------------------------
      // [B-Border - Table]
      // ----------------------------------------------------------------------

      srcPtr = src;
      for (i = 0; i < bTableSize; i++)
      {
        Accessor::blurRunM(run, srcPtr + bTableData[i], aValue);
        Accessor::storeRunT(stackPtr, run);

        stackPtr += Accessor::STACK_BPP;
      }

      // ----------------------------------------------------------------------
      // [B-Border - Tail]
      // ----------------------------------------------------------------------

      i = bBorderSize;
      if (i != 0)
      {
        typename Accessor::Run border;

        if (blurCtx->extendType == FE_EXTEND_COLOR)
          Accessor::fetchPixelS(pix, blurCtx->extendColor);
        else
          Accessor::fetchPixelM(pix, srcPtr + blurCtx->srcLastOffset);

        border.set(pix);
        do {
          Accessor::blurPixel(run, border, aValue);
          Accessor::storeRunT(stackPtr, run);
          stackPtr += Accessor::STACK_BPP;
        } while (--i);
      }

      // ----------------------------------------------------------------------
      // [B-Border - Secondary]
      // ----------------------------------------------------------------------

      for (i = bTableSize + bBorderSize; i; i--)
      {
        stackPtr -= Accessor::STACK_BPP;
        Accessor::blurRunT(run, stackPtr, aValue);
      }

      // ----------------------------------------------------------------------
      // [Run-Loop - Secondary]
      // ----------------------------------------------------------------------

      FOG_ASSERT(runSize != 0);
      i = runSize;

      do {
        dstPtr -= dstStride;

        Accessor::blurRunM(run, dstPtr, aValue);
        Accessor::storeRunM(dstPtr, run);
      } while (--i);

      dst += Accessor::PIXEL_BPP;
      src += Accessor::PIXEL_BPP;
    }
  }
};

} // Render namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_FILTERBLUR_P_H
