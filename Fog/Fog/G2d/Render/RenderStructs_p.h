// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDERSTRUCTS_P_H
#define _FOG_G2D_RENDER_RENDERSTRUCTS_P_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Math/Fixed.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/ColorStop.h>
#include <Fog/G2d/Source/Gradient.h>
#include <Fog/G2d/Source/Texture.h>
#include <Fog/G2d/Render/RenderFuncs_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Render
//! @{

// ============================================================================
// [Fog::RenderClosure]
// ============================================================================

// Typedef is already in RenderFuncs_p.h.
//
// {{{
// typedef ImageConverterClosure RenderClosure;
// }}}

// ============================================================================
// [Fog::RenderConverterPass]
// ============================================================================

struct FOG_NO_EXPORT RenderConverterPass
{
  //! @brief Alpha mask (after shift).
  uint32_t aMask;
  //! @brief Red mask (after shift).
  uint32_t rMask;
  //! @brief Green mask (after shift).
  uint32_t gMask;
  //! @brief Blue mask (after shift).
  uint32_t bMask;

  //! @brief Alpha scale constant (integer).
  uint32_t aScale;
  //! @brief Red scale constant (integer).
  uint32_t rScale;
  //! @brief Green scale constant (integer).
  uint32_t gScale;
  //! @brief Blue scale constant (integer).
  uint32_t bScale;

  //! @brief Alpha shift.
  uint8_t aShift;
  //! @brief Red shift.
  uint8_t rShift;
  //! @brief Green shift.
  uint8_t gShift;
  //! @brief Blue shift.
  uint8_t bShift;

  //! @brief Fill mask for integer based formats (up to 64 bits per pixel).
  uint64_t fill;
};

// ============================================================================
// [Fog::RenderConverterMulti]
// ============================================================================

//! @internal
//!
//! @brief Structure used by the multi-converter (the converter used to convert
//! between image formats not directly supported by Fog-Framework).
struct FOG_NO_EXPORT RenderConverterMulti
{
  //! @brief Converter data for each pass.
  RenderConverterPass pass[2];

  //! @brief Converter blitter for each pass (convert to/from a middle format).
  RenderVBlitLineFn blit[2];
  //! @brief Middleware, colorspace conversion routine (optional, can be NULL).
  RenderVBlitLineFn middleware;

  //! @brief Destination advance relative to 'step'.
  sysint_t dstAdvance;
  //! @brief Source advance relative to 'step'.
  sysint_t srcAdvance;

  //! @brief How many pixels in chunk to process.
  //!
  //! @note This is very important, becuase the multi-pass converter does not
  //! know about the middle pixel format. It has @c RENDER_CONVERTER_BUFFER_SIZE 
  //! bytes reserved on the stack for conversion and the 'step' * bytesPerPixel 
  //! must fit into that buffer. This member should prevent calculating the step
  //! again and again by the multi-pass blitter.
  int step;
};

// ============================================================================
// [Fog::RenderSolid]
// ============================================================================

//! @internal
//!
//! @brief Solid color for 32-bit and 64-bit image manipulation.
union RenderSolid
{
  ArgbBase64 prgb64;
  ArgbBase32 prgb32;
};

// ============================================================================
// [Fog::RenderPatternContext]
// ============================================================================

//! @internal
//!
//! @brief The context used for pattern generation.
struct FOG_NO_EXPORT RenderPatternContext
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
    RenderPatternPrepareFn prepare, 
    RenderPatternFetchFn fetch, 
    RenderPatternSkipFn skip, 
    RenderPatternDestroyFn destroy)
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

  FOG_INLINE void prepare(RenderPatternFetcher* fetcher, int yPosition, int yAdvance, uint32_t mode) const
  {
    _prepare(this, fetcher, yPosition, yAdvance, mode);
  }

  // --------------------------------------------------------------------------
  // [Members - Core]
  // --------------------------------------------------------------------------

  //! @brief The reference count.
  Atomic<sysuint_t> _refCount;

  //! @brief Prepare function.
  RenderPatternPrepareFn _prepare;
  //! @brief Fetch function (can be @c NULL if @c fetch is initialized by @c prepare).
  RenderPatternFetchFn _fetch;
  //! @brief Skip function (can be @c NULL if @c skip is initialized by @c prepare).
  RenderPatternSkipFn _skip;
  //! @brief Destroy function.
  RenderPatternDestroyFn _destroy;

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

    //! @brief The texture (or texture fragment) bits.
    const uint8_t* bits;
    //! @brief The texture (or texture fragment) color-palette.
    const uint32_t* pal;
    //! @brief The texture (or texture fragment) size.
    int w, h;
    //! @brief The texture (or texture fragment) stride.
    sysint_t stride;
  };

  struct _TextureSimple
  {
    //! @brief The inverse offset.
    int dx, dy;

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
    struct _Simple
    {
      //! @brief Offset.
      double offset;
      double xx, yx;
      Fixed16x16 xx16x16;
    } simple;

    struct _Projection
    {
      double offset;
      double xx, yx, zx;
      double xz, yz, zz;
    } proj;
  };

  struct _GradientRadial
  {
    /*
    double dx, dy;
    double r0, fx0, fy0;
    double r1, fx1, fy1;
    double scale;
    */
  };

  struct _GradientConical
  {
    /*
    double dx, dy;
    double angle;
    */
  };

  struct _GradientPacked
  {
    _GradientBase base;

    union
    {
      _GradientLinear linear;
    };
  };

  // --------------------------------------------------------------------------
  // [Members - Data]
  // --------------------------------------------------------------------------

  union Data
  {
    RenderSolid solid;
    _TexturePacked texture;
    _GradientPacked gradient;

    uint8_t raw[128];
  } _d;

  union Inverse
  {
    Static<TransformF> transformf;
    Static<TransformD> transformd;
  } _inverse;
};

// ============================================================================
// [Fog::RenderPatternFetcher]
// ============================================================================

struct FOG_NO_EXPORT RenderPatternFetcher
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const RenderPatternContext* getContext() const { return _ctx; }
  FOG_INLINE uint32_t getMode() const { return _mode; }

  FOG_INLINE uint8_t* getData() { return _d.raw; }
  FOG_INLINE const uint8_t* getData() const { return _d.raw; }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE void fetch(Span* span, uint8_t* buffer) { _fetch(this, span, buffer); }
  FOG_INLINE void skip(int count) { _skip(this, count); }

  // --------------------------------------------------------------------------
  // [Members - Core]
  // --------------------------------------------------------------------------

  const RenderPatternContext* _ctx;

  RenderPatternFetchFn _fetch;
  RenderPatternSkipFn _skip;

  uint32_t _mode;
#if FOG_ARCH_BITS >= 64
  uint32_t _reserved;
#endif // FOG_ARCH

  // --------------------------------------------------------------------------
  // [Members - Texture]
  // --------------------------------------------------------------------------

  union _TexturePacked
  {
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

  struct _GradientPacked
  {
    _GradientLinear linear;
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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDERSTRUCTS_P_H
