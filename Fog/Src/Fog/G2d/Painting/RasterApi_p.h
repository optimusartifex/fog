// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERAPI_P_H
#define _FOG_G2D_PAINTING_RASTERAPI_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Gradient.h>
#include <Fog/G2d/Source/Texture.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Rasterizer8;
struct Rasterizer16;

struct BoxRasterizer8;
struct BoxRasterizer16;

struct PathRasterizer8;
struct PathRasterizer16;

struct RasterContext;
struct RasterFiller;
struct RasterPaintEngine;
struct RasterSerializer;

struct RasterScanline8;
struct RasterScanline16;

struct RasterSpan;
struct RasterSpan8;
struct RasterSpan16;

union RasterSolid;
struct RasterPattern;
struct RasterPatternFetcher;

struct RasterFilter;
struct RasterConvolve;

// ============================================================================
// [Fog::Raster - TypeDefs - RasterClosure]
// ============================================================================

typedef ImageConverterClosure RasterClosure;

// ============================================================================
// [Fog::Raster - TypeDefs - RasterConvert]
// ============================================================================

//! @internal
typedef err_t (FOG_FASTCALL *RasterConvertInitFunc)(
  ImageConverterData* d);

//! @internal
typedef void (FOG_FASTCALL *RasterConvertDestroyFunc)(
  ImageConverterData* d);

//! @internal
typedef void (FOG_FASTCALL *RasterConvertSpanFunc)(uint8_t* dst,
  const RasterSolid* src,
  int w,
  const ImageConverterClosure* closure);

// ============================================================================
// [Fog::Raster - TypeDefs - RasterCBlit]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterCBlitRectFunc)(
  uint8_t* dst, ssize_t dstStride,
  const RasterSolid* src,
  int w, int h,
  const RasterClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RasterCBlitLineFunc)(
  uint8_t* dst,
  const RasterSolid* src,
  int w,
  const RasterClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RasterCBlitSpanFunc)(
  uint8_t* dst,
  const RasterSolid* src,
  const RasterSpan* span,
  const RasterClosure* closure);

// ============================================================================
// [Fog::Raster - TypeDefs - RasterVBlit]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterVBlitRectFunc)(
  uint8_t* dst, ssize_t dstStride,
  const uint8_t* src, ssize_t srcStride,
  int w, int h,
  const RasterClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RasterVBlitLineFunc)(
  uint8_t* dst,
  const uint8_t* src,
  int w,
  const RasterClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RasterVBlitSpanFunc)(
  uint8_t* dst,
  const RasterSpan* span,
  const RasterClosure* closure);

// ============================================================================
// [Fog::Raster - TypeDefs - Pattern - Create]
// ============================================================================

//! @internal
typedef err_t (FOG_FASTCALL *RasterSolidCreateFunc)(
  RasterPattern* ctx, uint32_t dstFormat,
  const RasterSolid* solid);

//! @internal
typedef err_t (FOG_FASTCALL *RasterTextureCreateFunc)(
  RasterPattern* ctx, uint32_t dstFormat, const BoxI* boundingBox,
  const Image* srcImage, const RectI* srcFragment, const TransformD* tr,
  const Color* clampColor, uint32_t tileMode,
  uint32_t imageQuality);

//! @internal
typedef err_t (FOG_FASTCALL *RasterGradientCreateFunc)(
  RasterPattern* ctx, uint32_t dstFormat, const BoxI* boundingBox,
  const GradientD* gradient, const TransformD* tr,
  uint32_t gradientQuality);

// ============================================================================
// [Fog::Raster - TypeDefs - Pattern - Destroy]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterPatternDestroyFunc)(
  RasterPattern* ctx);

// ============================================================================
// [Fog::Raster - TypeDefs - Pattern - Prepare]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterPatternPrepareFunc)(
  const RasterPattern* ctx,
  RasterPatternFetcher* fetcher, int y, int delta, uint32_t mode);

// ============================================================================
// [Fog::Raster - TypeDefs - Pattern - Fetch / Skip]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterPatternFetchFunc)(
  RasterPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer);

//! @internal
typedef void (FOG_FASTCALL *RasterPatternSkipFunc)(
  RasterPatternFetcher* fetcher, int step);

// ============================================================================
// [Fog::Raster - TypeDefs - Pattern - Gradient]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterGradientInterpolateFunc)(
  uint8_t* dst, int w, const ColorStop* stops, size_t length);

// ============================================================================
// [Fog::Raster - TypeDefs - Filter - Create]
// ============================================================================

//! @internal
typedef err_t (FOG_FASTCALL *RasterFilterCreateFunc)(
  RasterFilter* ctx,
  const FeBase* feBase,
  const ImageFilterScaleD* feScale,
  MemBuffer* memBuffer,
  uint32_t dstFormat,
  uint32_t srcFormat);

// ============================================================================
// [Fog::Raster - TypeDefs - Filter - Destroy]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterFilterDestroyFunc)(
  RasterFilter* ctx);

// ============================================================================
// [Fog::Raster - TypeDefs - Filter - DoLine]
// ============================================================================

typedef void (FOG_FASTCALL *RasterFilterDoLineFunc)(
  RasterFilter* ctx, uint8_t* dst, const uint8_t* src, int w);

// ============================================================================
// [Fog::Raster - TypeDefs - Filter - DoRect]
// ============================================================================

typedef err_t (FOG_FASTCALL *RasterFilterDoRectFunc)(
  RasterFilter* ctx,
  uint8_t*       dst, ssize_t dstStride, const SizeI* dstSize, const PointI* dstPos,
  const uint8_t* src, ssize_t srcStride, const SizeI* srcSize, const RectI* srcRect);

typedef void (FOG_FASTCALL *RasterFilterDoConvolveFunc)(
  RasterConvolve* ctx);

// ============================================================================
// [Fog::RasterConvertFuncs]
// ============================================================================

struct FOG_NO_EXPORT RasterConvertFuncs
{
  RasterConvertInitFunc init;

  RasterVBlitLineFunc copy[RASTER_COPY_COUNT];
  RasterVBlitLineFunc fill[RASTER_FILL_COUNT];
  RasterVBlitLineFunc bswap[RASTER_BSWAP_COUNT];

  RasterVBlitLineFunc argb32_from_prgb32;
  RasterVBlitLineFunc prgb32_from_argb32;

  RasterVBlitLineFunc argb64_from_prgb64;
  RasterVBlitLineFunc prgb64_from_argb64;

  RasterVBlitLineFunc argb32_from[RASTER_FORMAT_COUNT];
  RasterVBlitLineFunc from_argb32[RASTER_FORMAT_COUNT];

  RasterVBlitLineFunc argb64_from[RASTER_FORMAT_COUNT];
  RasterVBlitLineFunc from_argb64[RASTER_FORMAT_COUNT];

  // TODO: Not Implemented.
  RasterVBlitLineFunc dithered_from_xrgb32[RASTER_DITHER_COUNT];
  RasterVBlitLineFunc dithered_from_rgb24 [RASTER_DITHER_COUNT];
};

// ============================================================================
// [Fog::RasterCompositeCoreFuncs]
// ============================================================================

struct FOG_NO_EXPORT RasterCompositeCoreFuncs
{
  RasterCBlitRectFunc cblit_rect[RASTER_CBLIT_COUNT];
  RasterCBlitLineFunc cblit_line[RASTER_CBLIT_COUNT];
  RasterCBlitSpanFunc cblit_span[RASTER_CBLIT_COUNT];

  RasterVBlitRectFunc vblit_rect[IMAGE_FORMAT_COUNT];
  RasterVBlitLineFunc vblit_line[IMAGE_FORMAT_COUNT];
  RasterVBlitSpanFunc vblit_span[IMAGE_FORMAT_COUNT];
};

// ============================================================================
// [Fog::RasterCompositeExtFuncs]
// ============================================================================

struct FOG_NO_EXPORT RasterCompositeExtFuncs
{
  RasterCBlitLineFunc cblit_line[RASTER_CBLIT_COUNT];
  RasterCBlitSpanFunc cblit_span[RASTER_CBLIT_COUNT];

  RasterVBlitLineFunc vblit_line[RASTER_VBLIT_COUNT];
  RasterVBlitSpanFunc vblit_span[RASTER_VBLIT_COUNT];
};

// ============================================================================
// [Fog::RasterSolidFuncs]
// ============================================================================

struct FOG_NO_EXPORT RasterSolidFuncs
{
  RasterSolidCreateFunc create;
  RasterPatternDestroyFunc destroy;

  RasterPatternPrepareFunc prepare;
  RasterPatternFetchFunc fetch[IMAGE_FORMAT_COUNT];
};

// ============================================================================
// [Fog::RasterTextureFuncs]
// ============================================================================

struct FOG_NO_EXPORT RasterTextureFuncs
{
  RasterTextureCreateFunc create;

  struct FOG_NO_EXPORT _Fetch
  {
    RasterPatternFetchFunc fetch_simple_align[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
    RasterPatternFetchFunc fetch_simple_subx0[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
    RasterPatternFetchFunc fetch_simple_sub0y[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
    RasterPatternFetchFunc fetch_simple_subxy[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];

    RasterPatternFetchFunc fetch_scale_nearest[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
    RasterPatternFetchFunc fetch_scale_bilinear[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];

    RasterPatternFetchFunc fetch_affine_nearest[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
    RasterPatternFetchFunc fetch_affine_bilinear[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];

    RasterPatternFetchFunc fetch_proj_nearest[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
    RasterPatternFetchFunc fetch_proj_bilinear[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
  };

  _Fetch prgb32;
  _Fetch a8;

  _Fetch prgb64;
  _Fetch rgb48;
  _Fetch a16;
};

// ============================================================================
// [Fog::RasterGradientFuncs]
// ============================================================================

struct FOG_NO_EXPORT RasterGradientFuncs
{
  RasterGradientInterpolateFunc interpolate[IMAGE_FORMAT_COUNT];

  RasterGradientCreateFunc create[GRADIENT_TYPE_COUNT];

  struct _Linear
  {
    RasterPatternFetchFunc fetch_simple_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
    RasterPatternFetchFunc fetch_simple_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];

    RasterPatternFetchFunc fetch_proj_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
    RasterPatternFetchFunc fetch_proj_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
  } linear;

  struct _Radial
  {
    RasterPatternFetchFunc fetch_simple_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
    RasterPatternFetchFunc fetch_simple_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];

    RasterPatternFetchFunc fetch_proj_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
    RasterPatternFetchFunc fetch_proj_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
  } radial;

  struct _Conical
  {
    RasterPatternFetchFunc fetch_simple_nearest[IMAGE_FORMAT_COUNT];
    RasterPatternFetchFunc fetch_simple_bilinear[IMAGE_FORMAT_COUNT];
  } conical;

  struct _Rectangular
  {
    RasterPatternFetchFunc fetch_simple_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
    RasterPatternFetchFunc fetch_simple_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];

    RasterPatternFetchFunc fetch_proj_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
    RasterPatternFetchFunc fetch_proj_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
  } rectangular;
};

// ============================================================================
// [Fog::RasterFilterFuncs]
// ============================================================================

struct FOG_NO_EXPORT RasterFilterFuncs
{
  RasterFilterCreateFunc create[FE_TYPE_COUNT];

  struct _Blur
  {
    struct _Box
    {
      RasterFilterDoConvolveFunc convolve_h[IMAGE_FORMAT_COUNT];
      RasterFilterDoConvolveFunc convolve_v[IMAGE_FORMAT_COUNT];
    } box;

    struct _Exp
    {
      RasterFilterDoConvolveFunc convolve_h[IMAGE_FORMAT_COUNT];
      RasterFilterDoConvolveFunc convolve_v[IMAGE_FORMAT_COUNT];
    } exp;
  } blur;
};

// ============================================================================
// [Fog::ApiRaster]
// ============================================================================

//! @internal
//!
//! @brief Fog/G2d render functions.
struct FOG_NO_EXPORT ApiRaster
{
  // --------------------------------------------------------------------------
  // [Accessors - Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t initConverter(ImageConverterData* d) const
  {
    return convert.init(d);
  }

  // --------------------------------------------------------------------------
  // [Accessors - Composite - Copy]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterVBlitRectFunc getCopyRectFunc(uint32_t format) const
  {
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);

    return compositeCore[format][COMPOSITE_SRC].vblit_rect[format];
  }

  FOG_INLINE RasterVBlitRectFunc getCopyRectFunc(uint32_t dstFormat, uint32_t srcFormat) const
  {
    FOG_ASSERT(dstFormat < IMAGE_FORMAT_COUNT);
    FOG_ASSERT(srcFormat < IMAGE_FORMAT_COUNT);

    return compositeCore[dstFormat][COMPOSITE_SRC].vblit_rect[srcFormat];
  }

  FOG_INLINE RasterVBlitLineFunc getCopyFullFunc(uint32_t format) const
  {
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);

    return compositeCore[format][COMPOSITE_SRC].vblit_line[format];
  }

  FOG_INLINE RasterVBlitLineFunc getCopyFullFunc(uint32_t dstFormat, uint32_t srcFormat) const
  {
    FOG_ASSERT(dstFormat < IMAGE_FORMAT_COUNT);
    FOG_ASSERT(srcFormat < IMAGE_FORMAT_COUNT);

    return compositeCore[dstFormat][COMPOSITE_SRC].vblit_line[srcFormat];
  }

  // --------------------------------------------------------------------------
  // [Accessors - Composite - Core]
  // --------------------------------------------------------------------------

  FOG_INLINE const RasterCompositeCoreFuncs* getCompositeCore(uint32_t format, uint32_t op) const
  {
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);
    FOG_ASSERT(op >= RASTER_COMPOSITE_CORE_START &&
               op <  RASTER_COMPOSITE_CORE_START + RASTER_COMPOSITE_CORE_COUNT);

    return &compositeCore[format][op];
  }

  // --------------------------------------------------------------------------
  // [Accessors - Composite - Extended]
  // --------------------------------------------------------------------------

  FOG_INLINE const RasterCompositeExtFuncs* getCompositeExt(uint32_t format, uint32_t op) const
  {
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);
    FOG_ASSERT(op >= RASTER_COMPOSITE_EXT_START &&
               op <  RASTER_COMPOSITE_EXT_START + RASTER_COMPOSITE_EXT_COUNT);

    return &compositeExt[format][op - RASTER_COMPOSITE_EXT_START];
  }

  // --------------------------------------------------------------------------
  // [Accessors - Composite - Unified]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterCBlitLineFunc getCBlitLine(uint32_t dstFormat, uint32_t op, uint32_t isOpaque)
  {
    if (RasterUtil::isCompositeCoreOp(op))
      return getCompositeCore(dstFormat, op)->cblit_line[isOpaque];
    else
      return getCompositeExt(dstFormat, op)->cblit_line[isOpaque];
  }

  FOG_INLINE RasterCBlitSpanFunc getCBlitSpan(uint32_t dstFormat, uint32_t op, uint32_t isOpaque)
  {
    if (RasterUtil::isCompositeCoreOp(op))
      return getCompositeCore(dstFormat, op)->cblit_span[isOpaque];
    else
      return getCompositeExt(dstFormat, op)->cblit_span[isOpaque];
  }

  FOG_INLINE RasterVBlitLineFunc getVBlitLine(uint32_t dstFormat, uint32_t op, uint32_t srcFormat)
  {
    if (RasterUtil::isCompositeCoreOp(op))
      return getCompositeCore(dstFormat, op)->vblit_line[srcFormat];
    else
      return getCompositeExt(dstFormat, op)->vblit_line[RasterUtil::getCompositeCompatVBlitId(dstFormat, srcFormat)];
  }

  FOG_INLINE RasterVBlitSpanFunc getVBlitSpan(uint32_t dstFormat, uint32_t op, uint32_t srcFormat)
  {
    if (RasterUtil::isCompositeCoreOp(op))
      return getCompositeCore(dstFormat, op)->vblit_span[srcFormat];
    else
      return getCompositeExt(dstFormat, op)->vblit_span[RasterUtil::getCompositeCompatVBlitId(dstFormat, srcFormat)];
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RasterConvertFuncs convert;
  RasterCompositeCoreFuncs compositeCore[IMAGE_FORMAT_COUNT][RASTER_COMPOSITE_CORE_COUNT];
  RasterCompositeExtFuncs compositeExt[IMAGE_FORMAT_COUNT][RASTER_COMPOSITE_EXT_COUNT];

  RasterSolidFuncs solid;
  RasterTextureFuncs texture;
  RasterGradientFuncs gradient;

  RasterFilterFuncs filter;
};

extern FOG_API ApiRaster _api_raster;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERAPI_P_H
