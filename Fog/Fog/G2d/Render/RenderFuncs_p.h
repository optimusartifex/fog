// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDERFUNCS_P_H
#define _FOG_G2D_RENDER_RENDERFUNCS_P_H

// [Dependencies]
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Render
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

// Fog/G2d.
struct Color;
struct ColorStop;
struct ColorStopCache;
struct ColorStopList;
struct ImageConverterClosure;
struct ImageConverterData;
struct GradientF;
struct GradientD;
struct PatternF;
struct PatternD;
struct Span;
struct Texture;
struct TransformF;
struct TransformD;

// Fog/G2d/Render.
typedef ImageConverterClosure RenderClosure;
union  RenderSolid;
struct RenderPatternContext;
struct RenderPatternFetcher;

// ============================================================================
// [Fog::Render - Prototype - Convert]
// ============================================================================

//! @internal
typedef err_t (FOG_FASTCALL *RenderConverterInitFn)(
  ImageConverterData* d);

//! @internal
typedef void (FOG_FASTCALL *RenderConverterDestroyFn)(
  ImageConverterData* d);

//! @internal
typedef void (FOG_FASTCALL *RenderConverterSpanFn)(
  uint8_t* dst,
  const RenderSolid* src,
  int w,
  const ImageConverterClosure* closure);

// ============================================================================
// [Fog::Render - Prototype - CBlit]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderCBlitRectFn)(
  uint8_t* dst, sysint_t dstStride,
  const RenderSolid* src,
  int w, int h,
  const RenderClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RenderCBlitLineFn)(
  uint8_t* dst,
  const RenderSolid* src,
  int w,
  const RenderClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RenderCBlitSpanFn)(
  uint8_t* dst,
  const RenderSolid* src,
  const Span* span,
  const RenderClosure* closure);

// ============================================================================
// [Fog::Render - Prototype - VBlit]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderVBlitRectFn)(
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  int w, int h,
  const RenderClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RenderVBlitLineFn)(
  uint8_t* dst,
  const uint8_t* src,
  int w,
  const RenderClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RenderVBlitSpanFn)(
  uint8_t* dst,
  const Span* span,
  const RenderClosure* closure);

// ============================================================================
// [Fog::Render - Prototype - Mask]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterMaskCOpVFn)(
  uint8_t* dst, uint32_t a, const uint8_t* b, int w);

//! @internal
typedef void (FOG_FASTCALL *RasterMaskVOpCFn)(
  uint8_t* dst, const uint8_t* a, uint32_t b, int w);

//! @internal
typedef void (FOG_FASTCALL *RasterMaskVOpVFn)(
  uint8_t* dst, const uint8_t* a, const uint8_t* b, int w);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Create]
// ============================================================================

//! @internal
typedef err_t (FOG_FASTCALL *RenderPatternSolidCreateFn)(
  RenderPatternContext* ctx, uint32_t dstFormat,
  const RenderSolid& solid);

//! @internal
typedef err_t (FOG_FASTCALL *RenderPatternTextureCreateFn)(
  RenderPatternContext* ctx, uint32_t dstFormat, const BoxI& boundingBox,
  const Image& srcImage, const RectI& srcFragment, const TransformD& tr,
  const Color& clampColor, uint32_t tileMode,
  uint32_t imageQuality);

//! @internal
typedef err_t (FOG_FASTCALL *RenderPatternGradientCreateFn)(
  RenderPatternContext* ctx, uint32_t dstFormat, const BoxI& boundingBox,
  const GradientD& gradient, const TransformD& tr,
  uint32_t gradientQuality);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Interpolate]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderPatternGradientInterpolateFn)(
  uint8_t* dst, int w, const ColorStop* stops, sysuint_t length);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Destroy]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderPatternDestroyFn)(
  RenderPatternContext* ctx);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Prepare]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderPatternPrepareFn)(
  const RenderPatternContext* ctx,
  RenderPatternFetcher* fetcher, int y, int delta, uint32_t mode);

// ============================================================================
// [Fog::Render - Prototype - Pattern - Fetch / Skip]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RenderPatternFetchFn)(
  RenderPatternFetcher* fetcher, Span* span, uint8_t* buffer);

//! @internal
typedef void (FOG_FASTCALL *RenderPatternSkipFn)(
  RenderPatternFetcher* fetcher, int step);


//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDERFUNCS_P_H
