// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDERFUNCS_P_H
#define _FOG_G2D_RENDER_RENDERFUNCS_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_G2d_Render
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

// Fog/G2d/Painting.
struct RasterSpan;

typedef ImageConverterClosure RenderClosure;
union RenderSolid;
struct RenderPatternContext;
struct RenderPatternFetcher;

// ============================================================================
// [Fog::Render - Prototype - Convert]
// ============================================================================

//! @internal
typedef err_t (FOG_FASTCALL *RenderConverterInitFunc)(
  ImageConverterData* d);

//! @internal
typedef void (FOG_FASTCALL *RenderConverterDestroyFunc)(
  ImageConverterData* d);

//! @internal
typedef void (FOG_FASTCALL *RenderConverterSpanFunc)(
  uint8_t* dst,
  const RenderSolid* src,
  int w,
  const ImageConverterClosure* closure);

// ============================================================================
// [Fog::Render - Prototype - CBlit]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderCBlitRectFunc)(
  uint8_t* dst, ssize_t dstStride,
  const RenderSolid* src,
  int w, int h,
  const RenderClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RenderCBlitLineFunc)(
  uint8_t* dst,
  const RenderSolid* src,
  int w,
  const RenderClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RenderCBlitSpanFunc)(
  uint8_t* dst,
  const RenderSolid* src,
  const RasterSpan* span,
  const RenderClosure* closure);

// ============================================================================
// [Fog::Render - Prototype - VBlit]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderVBlitRectFunc)(
  uint8_t* dst, ssize_t dstStride,
  const uint8_t* src, ssize_t srcStride,
  int w, int h,
  const RenderClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RenderVBlitLineFunc)(
  uint8_t* dst,
  const uint8_t* src,
  int w,
  const RenderClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RenderVBlitSpanFunc)(
  uint8_t* dst,
  const RasterSpan* span,
  const RenderClosure* closure);

// ============================================================================
// [Fog::Render - Prototype - Mask]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterMaskCOpVFunc)(
  uint8_t* dst, uint32_t a, const uint8_t* b, int w);

//! @internal
typedef void (FOG_FASTCALL *RasterMaskVOpCFunc)(
  uint8_t* dst, const uint8_t* a, uint32_t b, int w);

//! @internal
typedef void (FOG_FASTCALL *RasterMaskVOpVFunc)(
  uint8_t* dst, const uint8_t* a, const uint8_t* b, int w);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Create]
// ============================================================================

//! @internal
typedef err_t (FOG_FASTCALL *RenderPatternSolidCreateFunc)(
  RenderPatternContext* ctx, uint32_t dstFormat,
  const RenderSolid& solid);

//! @internal
typedef err_t (FOG_FASTCALL *RenderPatternTextureCreateFunc)(
  RenderPatternContext* ctx, uint32_t dstFormat, const BoxI& boundingBox,
  const Image& srcImage, const RectI& srcFragment, const TransformD& tr,
  const Color& clampColor, uint32_t tileMode,
  uint32_t imageQuality);

//! @internal
typedef err_t (FOG_FASTCALL *RenderPatternGradientCreateFunc)(
  RenderPatternContext* ctx, uint32_t dstFormat, const BoxI& boundingBox,
  const GradientD& gradient, const TransformD& tr,
  uint32_t gradientQuality);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Interpolate]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderPatternGradientInterpolateFunc)(
  uint8_t* dst, int w, const ColorStop* stops, size_t length);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Destroy]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderPatternDestroyFunc)(
  RenderPatternContext* ctx);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Prepare]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderPatternPrepareFunc)(
  const RenderPatternContext* ctx,
  RenderPatternFetcher* fetcher, int y, int delta, uint32_t mode);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Fetch / Skip]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderPatternFetchFunc)(
  RenderPatternFetcher* fetcher, RasterSpan* span, uint8_t* buffer);

//! @internal
typedef void (FOG_FASTCALL *RenderPatternSkipFunc)(
  RenderPatternFetcher* fetcher, int step);


//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDERFUNCS_P_H
