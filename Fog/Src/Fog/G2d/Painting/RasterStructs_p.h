// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERSTRUCTS_P_H
#define _FOG_G2D_PAINTING_RASTERSTRUCTS_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/ColorStop.h>
#include <Fog/G2d/Source/Gradient.h>
#include <Fog/G2d/Source/Texture.h>

namespace Fog {

//! @addtogroup Fog_G2d_Render
//! @{

// ============================================================================
// [Fog::RasterScope]
// ============================================================================

//! @brief Raster paint-engine per-thread scope.
//!
//! Default value for single-threaded rendering is zero for @c offset and one
//! for @c delta. Any other combination means that multithreading is use. Each
//! thread has the same @c delta value, but different @c offset.
//!
//! For example if four threads are used for rendering, the per-thread scope
//! values will be:
//!  - Thread #0 - Offset==0, Delta==4 (Scanlines 0, 4,  8, 12, 16, ...)
//!  - Thread #1 - Offset==1, Delta==4 (Scanlines 1, 5,  9, 13, 17, ...)
//!  - Thread #2 - Offset==2, Delta==4 (Scanlines 2, 6, 10, 14, 18, ...)
//!  - Thread #3 - Offset==3, Delta==4 (Scanlines 3, 7, 11, 15, 19, ...)
struct FOG_NO_EXPORT RasterScope
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getDelta() const { return _delta; }
  FOG_INLINE int getOffset() const { return _offset; }

  FOG_INLINE void setScope(int delta, int offset)
  {
    _delta = delta;
    _offset = offset;
  }

  FOG_INLINE bool isSingleThreaded() const { return _delta == 1; }
  FOG_INLINE bool isMultiThreaded() const { return _delta > 1; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _delta = 1;
    _offset = 0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Delta (how many scanlines to advance each scanline).
  //!
  //!
  int _delta;

  //! @brief Base offset from zero scanline (positive value at range from zero
  //! to @c delta-1).
  int _offset;
};

// ============================================================================
// [Fog::RasterConvertPass]
// ============================================================================

struct FOG_NO_EXPORT RasterConvertPass
{
  //! @brief Fill mask for integer based formats (up to 64 bits per pixel).
  uint64_t fill;

  //! @brief Alpha shift.
  uint8_t aDstShift;
  //! @brief Red shift.
  uint8_t rDstShift;
  //! @brief Green shift.
  uint8_t gDstShift;
  //! @brief Blue shift.
  uint8_t bDstShift;

  //! @brief Alpha shift.
  uint8_t aSrcShift;
  //! @brief Red shift.
  uint8_t rSrcShift;
  //! @brief Green shift.
  uint8_t gSrcShift;
  //! @brief Blue shift.
  uint8_t bSrcShift;

  //! @brief Alpha mask (after shift).
  uint32_t aSrcMask;
  //! @brief Red mask (after shift).
  uint32_t rSrcMask;
  //! @brief Green mask (after shift).
  uint32_t gSrcMask;
  //! @brief Blue mask (after shift).
  uint32_t bSrcMask;

  //! @brief Alpha scale constant (integer).
  uint32_t aScale;
  //! @brief Red scale constant (integer).
  uint32_t rScale;
  //! @brief Green scale constant (integer).
  uint32_t gScale;
  //! @brief Blue scale constant (integer).
  uint32_t bScale;
};

// ============================================================================
// [Fog::RasterConvertMulti]
// ============================================================================

//! @internal
//!
//! @brief Structure used by the multi-converter (the converter used to convert
//! between image formats not directly supported by Fog-Framework).
struct FOG_NO_EXPORT RasterConvertMulti
{
  //! @brief Converter data for each pass.
  RasterConvertPass pass[2];

  //! @brief Converter blitter for each pass (convert to/from a middle format).
  RasterVBlitLineFunc blit[2];
  //! @brief Middleware, colorspace conversion routine (optional, can be NULL).
  RasterVBlitLineFunc middleware;

  //! @brief Destination advance relative to 'step'.
  ssize_t dstAdvance;
  //! @brief Source advance relative to 'step'.
  ssize_t srcAdvance;

  //! @brief How many pixels in chunk to process.
  //!
  //! @note This is very important, becuase the multi-pass converter does not
  //! know about the middle pixel format. It has @c RASTER_CONVERT_BUFFER_SIZE
  //! bytes reserved on the stack for conversion and the 'step' * bytesPerPixel
  //! must fit into that buffer. This member should prevent calculating the step
  //! again and again by the multi-pass blitter.
  int step;
};

// ============================================================================
// [Fog::RasterSolid]
// ============================================================================

//! @internal
//!
//! @brief Solid color for 32-bit and 64-bit rendering.
union RasterSolid
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
#if FOG_ARCH_BITS >= 64
    prgb64.u64 = 0;
#else
    prgb64.u32[0] = 0;
    prgb64.u32[1] = 0;
#endif // FOG_ARCH_BITS
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  ArgbBase64 prgb64;
  ArgbBase32 prgb32;
};

// ============================================================================
// [Fog::RasterPattern]
// ============================================================================

//! @internal
//!
//! @brief The context used for pattern generation.
struct FOG_NO_EXPORT RasterPattern
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get whether the render-pattern context is initialized.
  FOG_INLINE bool isInitialized() const { return _prepare != NULL; }

  FOG_INLINE uint32_t getDstFormat() const { return _dstFormat; }
  FOG_INLINE uint32_t getDstBPP() const { return _dstBPP; }

  FOG_INLINE uint32_t getSrcFormat() const { return _srcFormat; }
  FOG_INLINE uint32_t getSrcBPP() const { return _srcBPP; }

  template<typename T>
  FOG_INLINE T* getRaw() const { return (T*)(_d.raw); }

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void _initDst(uint32_t format)
  {
    _dstFormat = format;
    _dstBPP = ImageFormatDescription::getByFormat(format).getBytesPerPixel();
  }

  FOG_INLINE void _initSrc(uint32_t format)
  {
    _srcFormat = format;

    // Format is const, this should compile to a single instruction.
    switch (format)
    {
      case IMAGE_FORMAT_PRGB32: _srcBPP = 4; break;
      case IMAGE_FORMAT_XRGB32: _srcBPP = 4; break;
      case IMAGE_FORMAT_RGB24 : _srcBPP = 3; break;
      case IMAGE_FORMAT_A8    : _srcBPP = 1; break;
      case IMAGE_FORMAT_I8    : _srcBPP = 1; break;
      case IMAGE_FORMAT_PRGB64: _srcBPP = 8; break;
      case IMAGE_FORMAT_RGB48 : _srcBPP = 6; break;
      case IMAGE_FORMAT_A16   : _srcBPP = 2; break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }

  FOG_INLINE void _initFuncs(
    RasterPatternPrepareFunc prepare,
    RasterPatternFetchFunc fetch,
    RasterPatternSkipFunc skip,
    RasterPatternDestroyFunc destroy)
  {
    _prepare = prepare;
    _fetch = fetch;
    _skip = skip;
    _destroy = destroy;
  }

  FOG_INLINE void destroy()
  {
    _destroy(this);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _prepare = NULL;
  }

  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  FOG_INLINE void prepare(RasterPatternFetcher* fetcher, int y, int delta, uint32_t mode) const
  {
    _prepare(this, fetcher, y, delta, mode);
  }

  // --------------------------------------------------------------------------
  // [Members - Core]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  Atomic<size_t> _reference;

  //! @brief Prepare function.
  RasterPatternPrepareFunc _prepare;
  //! @brief Fetch function (can be @c NULL if @c fetch is initialized by @c prepare).
  RasterPatternFetchFunc _fetch;
  //! @brief Skip function (can be @c NULL if @c skip is initialized by @c prepare).
  RasterPatternSkipFunc _skip;
  //! @brief Destroy function.
  RasterPatternDestroyFunc _destroy;

  //! @brief Bounding box.
  BoxI _boundingBox;

  //! @brief Destination format.
  uint32_t _dstFormat;
  //! @brief Destination bytes-per-pixel.
  uint32_t _dstBPP;

  //! @brief Source format.
  uint32_t _srcFormat;
  //! @brief Source bytes-per-pixel.
  uint32_t _srcBPP;

  // --------------------------------------------------------------------------
  // [Members - Texture]
  // --------------------------------------------------------------------------

  struct _TextureBase
  {
    //! @brief The texture image.
    Static<Image> texture;

    //! @brief The texture (or texture-fragment) pixels.
    const uint8_t* pixels;
    //! @brief The texture (or texture-fragment) color-palette.
    const uint32_t* pal;
    //! @brief The texture (or texture-fragment) size.
    int w, h;
    //! @brief The texture (or texture-fragment) stride.
    ssize_t stride;

    //! @brief Clamped pixel value (@c TEXTURE_TILE_CLAMP).
    RasterSolid clamp;
  };

  struct _TextureSimple
  {
    //! @brief Translation (inverted).
    int tx, ty;

    //! @brief Used when @c TEXTURE_TILE_REPEAT or @c TEXTURE_TILE_REFLECT
    //! tiling is used.
    int repeatY;

    //! @brief Fractional part in case that texture is misaligned
    //! (translation coordinates aren't integers).
    union
    {
      struct
      {
        //! @brief The 8-bit or 16-bit fractional offset at [0, 0].
        uint32_t fY0X0;
        //! @brief The 8-bit or 16-bit fractional offset at [0, 1].
        uint32_t fY0X1;
        //! @brief The 8-bit or 16-bit fractional offset at [1, 0].
        uint32_t fY1X0;
        //! @brief The 8-bit or 16-bit fractional offset at [1, 1].
        uint32_t fY1X1;
      };

      uint32_t f[4];
    };
  };

  struct _TextureAffine
  {
    double tx, ty;
    double xx, xy;
    double yx, yy;
    double mx, my; // Max X/Y.
    double rx, ry; // Rewind X/Y (for REPEAT/REFLECT tiling).

    // Used by 16x16/48x16 fixed point.
    int xx16x16, xy16x16;
    int mx16x16, my16x16;
    int rx16x16, ry16x16;

    //! @brief Whether the xx is zero or very close (special-case).
    int xxZero;
    //! @brief Whether the yx is zero or very close (special-case).
    int xyZero;

    //! @brief Whether the fixed-point is safe for context bounding box.
    int safeFixedPoint;

    // Used for affine transformation, in 16.16 fp.
    //int fxmax;
    //int fymax;

    //int fxrewind;
    //int fyrewind;
  };

  struct _TexturePacked
  {
    _TextureBase base;

    union
    {
      _TextureSimple simple;
      _TextureAffine affine;
    };
  };

  // --------------------------------------------------------------------------
  // [Members - Gradient]
  // --------------------------------------------------------------------------

  struct _GradientBase
  {
    //! @brief Color stop cache used for this pattern context. Can be @c NULL.
    ColorStopCache* cache;
    //! @brief Color array, shorthand to <code>cache->getData()</code>.
    uint8_t* table;

    //! @brief Length of the color array, shorthand to <code>cache->getLength()</code>.
    int len;
    //! @brief Length of the color array in 16.16 fixed point.
    Fixed16x16 len16x16;
  };

  union _GradientLinear
  {
    struct _Shared
    {
      //! @brief Offset.
      double offset;
    } shared;

    struct _Simple : public _Shared
    {
      double xx, xy;

      //! @brief 16.16 fixed point representation of @c xx.
      Fixed16x16 xx16x16;
    } simple;

    struct _Projection : public _Shared
    {
      double xx, xy, xz;
      double zx, zy, zz;
    } proj;
  };

  union _GradientRadial
  {
    struct _Shared
    {
      double fx, fy;
      double scale;

      double xx, xy;
      double yx, yy;
      double tx, ty;

      double r2mfxfx;
      double r2mfyfy;
      double _2_fxfy;
    } shared;

    struct _Simple : public _Shared
    {
      double b_d;
      double d_d;
      double d_d_x;
      double d_d_y;
      double d_d_d;
    } simple;

    struct _Projection : public _Shared
    {
      double fxOrig;
      double fyOrig;

      double xz, yz;
      double tz;
    } proj;
  };

  union _GradientConical
  {
    struct _Shared
    {
      double xx, xy;
      double yx, yy;
      double tx, ty;

      double offset;
      double scale;
    } shared;

    struct _Simple : public _Shared
    {
    } simple;

    struct _Projection : public _Shared
    {
      double xz;
      double yz;
      double tz;
    } proj;
  };

  union _GradientRectangular
  {
    struct _Shared
    {
      double xx, xy;
      double yx, yy;
      double tx, ty;

      double px0, py0;
      double px1, py1;
    } shared;

    struct _Simple : public _Shared
    {
    } simple;

    struct _Projection : public _Shared
    {
      double xz;
      double yz;
      double tz;
    } proj;
  };

  struct _GradientPacked
  {
    _GradientBase base;

    union
    {
      _GradientLinear linear;
      _GradientRadial radial;
      _GradientConical conical;
      _GradientRectangular rectangular;
    };
  };

  // --------------------------------------------------------------------------
  // [Members - Data]
  // --------------------------------------------------------------------------

  union Data
  {
    RasterSolid solid;
    _TexturePacked texture;
    _GradientPacked gradient;

    uint8_t raw[128];
  } _d;
};

// ============================================================================
// [Fog::RasterPatternFetcher]
// ============================================================================

struct FOG_NO_EXPORT RasterPatternFetcher
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const RasterPattern* getContext() const { return _ctx; }
  FOG_INLINE uint32_t getMode() const { return _mode; }

  FOG_INLINE uint8_t* getData() { return _d.raw; }
  FOG_INLINE const uint8_t* getData() const { return _d.raw; }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE void fetch(RasterSpan* span, uint8_t* buffer) { _fetch(this, span, buffer); }
  FOG_INLINE void skip(int count) { _skip(this, count); }

  // --------------------------------------------------------------------------
  // [Members - Core]
  // --------------------------------------------------------------------------

  const RasterPattern* _ctx;

  RasterPatternFetchFunc _fetch;
  RasterPatternSkipFunc _skip;

  uint32_t _mode;
#if FOG_ARCH_BITS >= 64
  uint32_t _reserved;
#endif // FOG_ARCH

  // --------------------------------------------------------------------------
  // [Members - Texture]
  // --------------------------------------------------------------------------

  union _TexturePacked
  {
    struct _Simple
    {
      int py;
      int dy;
    } simple;

    struct _Affine
    {
      double px, py;
      double dx, dy;
    } affine;
  };

  // --------------------------------------------------------------------------
  // [Members - Gradient]
  // --------------------------------------------------------------------------

  union _GradientLinear
  {
    struct _Simple
    {
      double pt;
      double dt;
    } simple;

    struct _Projection
    {
      double pt;
      double pz;

      double dt;
      double dz;
    } proj;
  };

  // Shared between Radial/Conical/Rectangular.
  union _GradientAny
  {
    struct _Simple
    {
      double px, py;
      double dx, dy;
    } simple;

    struct _Projection
    {
      double px, py, pz;
      double dx, dy, dz;
    } proj;
  };

  struct _GradientPacked
  {
    _GradientLinear linear;
    _GradientAny radial;
    _GradientAny conical;
    _GradientAny rectangular;
  };

  // --------------------------------------------------------------------------
  // [Members - Data]
  // --------------------------------------------------------------------------

  union
  {
    _TexturePacked texture;
    _GradientPacked gradient;

    uint8_t raw[128];
  } _d;
};

// ============================================================================
// [Fog::RasterFiller]
// ============================================================================

struct FOG_NO_EXPORT RasterFiller
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  //! @brief Called by @c rasterize() to prepare for filling
  //!
  //! @param y The Y coordinate where the fill starts.
  typedef void (FOG_FASTCALL *PrepareFunc)(RasterFiller* self, int y);

  //! @brief Called by @c rasterize() to process a scanline.
  //!
  //! After the scanline is rendered the callback must advance position by 1
  //! when running single-threaded or by delta when running multi-threaded.
  typedef void (FOG_FASTCALL *ProcessFunc)(RasterFiller* self, RasterSpan* spans);

  //! @brief Called by @c rasterize() to skip a scanline(s).
  //!
  //! If running single-threaded the @a step parameter is the total scanlines
  //! to skip. When running multi-threaded, the @a step parameter must be
  //! multiplied by delta to get the total count of scanlines to skip.
  typedef void (FOG_FASTCALL *SkipFunc)(RasterFiller* self, int step);

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE void prepare(int y)
  {
    _prepare(this, y);
  }

  FOG_INLINE void process(RasterSpan* spans)
  {
    _process(this, spans);
  }

  FOG_INLINE void skip(int step)
  {
    _skip(this, step);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Prepare callback (set by user).
  PrepareFunc _prepare;
  //! @brief Process callback (set by user).
  ProcessFunc _process;
  //! @brief Skip callback (set by user).
  SkipFunc _skip;
};

// ============================================================================
// [Fog::RasterFilter]
// ============================================================================

struct FOG_NO_EXPORT RasterFilter
{
  // --------------------------------------------------------------------------
  // [Members - ColorLut]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _ColorLut
  {
  };

  // --------------------------------------------------------------------------
  // [Members - ColorMatrix]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _ColorMatrix
  {
  };

  // --------------------------------------------------------------------------
  // [Members - ComponentTransfer]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _ComponentTransfer
  {
  };

  // --------------------------------------------------------------------------
  // [Members - Blur]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _Blur
  {
    RasterSolid extendColor;
    uint32_t extendType;

    int hRadius;
    int vRadius;

    RasterFilterDoConvolveFunc hConvolve;
    RasterFilterDoConvolveFunc vConvolve;
  };

  // --------------------------------------------------------------------------
  // [Members - ConvolveMatrix]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _ConvolveMatrix
  {
  };

  // --------------------------------------------------------------------------
  // [Members - ConvolveSeparable]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _ConvolveSeparable
  {
  };

  // --------------------------------------------------------------------------
  // [Members - Morphology]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _Morphology
  {
  };

  // --------------------------------------------------------------------------
  // [Members - Data]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  Atomic<size_t> reference;

  //! @brief Destroy filter.
  RasterFilterDestroyFunc destroy;

  //! @brief Filter rectangle.
  RasterFilterDoRectFunc doRect;

  //! @brief Filter single line (only for color/pixel based filters).
  //!
  //! This function is only available for color/pixel based filters. Any other
  //! filters which need to access neighbor pixels (convolution) have this
  //! function set to @c NULL.
  RasterFilterDoLineFunc doLine;

  //! @brief Memory buffer instance which can be used as temporary storage.
  MemBuffer* memBuffer;

  //! @brief Destination pixel format.
  uint32_t dstFormat;
  //! @brief Source pixel format.
  uint32_t srcFormat;

  union
  {
    _ColorLut colorLut;
    _ColorMatrix colorMatrix;
    _ComponentTransfer componentTransfer;

    _Blur blur;
    _ConvolveMatrix convolveMatrix;
    _ConvolveSeparable convolveSeparable;
    _Morphology morphology;
  };
};

// ============================================================================
// [Fog::RasterConvolveData]
// ============================================================================

struct FOG_NO_EXPORT RasterConvolve
{
  //! @brief Filter context (immutable at this place).
  RasterFilter* filterCtx;

  //! @brief Destination data.
  uint8_t* dstData;
  //! @brief Destination stride.
  ssize_t dstStride;

  //! @brief Source data.
  uint8_t* srcData;
  //! @brief Source stride.
  ssize_t srcStride;

  //! @brief A table data.
  ssize_t* aTableData;
  //! @brief B table data.
  ssize_t* bTableData;

  //! @brief Offset to the first src pixel (for FE_EXTEND_PAD).
  ssize_t srcFirstOffset;
  //! @brief Last offset to the last src pixel (fog FE_EXTEND_PAD).
  ssize_t srcLastOffset;

  //! @brief Run offset.
  ssize_t runOffset;

  // This mess is here to align extendPixel to 64-bits, because it can be
  // accessed by 8-byte load instruction under 64-bit mode.

  //! @brief Extend color.
  RasterSolid extendColor;
  //! @brief Extend type.
  uint32_t extendType;

  //! @brief Size of A border (lead).
  uint aBorderLeadSize;
  //! @brief Size of A border (tail).
  uint aBorderTailSize;
  //! @brief Size of B border (tail, there is no lead b-border).
  uint bBorderTailSize;

  //! @brief Size of A table.
  uint aTableSize;
  //! @brief Size of B table.
  uint bTableSize;

  //! @brief How many rows or columns to process.
  uint rowSize;
  //! @brief Run size.
  uint runSize;

  //! @brief Kernel radius
  uint kernelRadius;
  //! @brief Kernel size (radius * 2 + 1).
  uint kernelSize;

  //! @brief Stack.
  uint8_t* stack;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERSTRUCTS_P_H
