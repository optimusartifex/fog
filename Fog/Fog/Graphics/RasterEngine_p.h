// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERENGINE_P_H
#define _FOG_GRAPHICS_RASTERENGINE_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Path.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct BlurParams;
struct ColorLut;
struct ColorMatrix;
struct ConvolveParams;
struct DoubleMatrix;
struct FloatSymmetricConvolveParams;
struct IntSymmetricConvolveParams;
struct Pattern;
struct RasterClosure;
struct RasterPattern;
struct RasterSolid;
struct Span;

// ============================================================================
// [Constants]
// ============================================================================

//! @internal
//!
//! @brief Reciprocal table that can be used to demultiply color value in
//! 24-bit precision.
extern FOG_API const uint32_t raster_demultiply_reciprocal_table_d[256];

//! @internal
//!
//! @brief Reciprocal table that can be used to demultiply color value in
//! 16-bit precision (used in MMX/SSE2 demultiply-code).
extern FOG_API const uint16_t raster_demultiply_reciprocal_table_w[256*4];

//! @internal
//!
//! @brief Reciprocal table that can be used to demultiply color value in
//! floating point precision (for image formats and filters that need floating
//! point accuracy).
extern FOG_API const float raster_demultiply_reciprocal_table_f[256];

// ============================================================================
// [Fog::RasterEngine - Prototypes - Dither]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterDither8Fn)(
  uint8_t* dst, const uint8_t* src, int w, const IntPoint& origin, const uint8_t* palConv);

//! @internal
typedef void (FOG_FASTCALL *RasterDither16Fn)(
  uint8_t* dst, const uint8_t* src, int w, const IntPoint& origin);

// ============================================================================
// [Fog::RasterEngine - Prototypes - Interpolate]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterInterpolateArgbFn)(
  uint8_t* dst, uint32_t c0, uint32_t c1,
  int w, int x1, int x2);

// ============================================================================
// [Fog::RasterEngine - Prototypes - Pattern]
// ============================================================================

// Generic.

//! @internal
typedef err_t (FOG_FASTCALL *RasterPatternInitFn)(
  RasterPattern* ctx, const Pattern& pattern, const DoubleMatrix& matrix);

//! @internal
typedef void (FOG_FASTCALL *RasterPatternDestroyFn)(
  RasterPattern* ctx);

//! @internal
typedef void (FOG_FASTCALL *RasterPatternFetchFn)(
  const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode);

// Solid.

//! @internal
typedef err_t (FOG_FASTCALL *RasterSolidInitFn)(
  RasterPattern* ctx, uint32_t argb);

// Texture.

//! @internal
typedef err_t (FOG_FASTCALL *RasterTextureInitFn)(
  RasterPattern* ctx, const Pattern& pattern, const DoubleMatrix& matrix, uint32_t imageInterpolationType);

//! @internal
typedef err_t (FOG_FASTCALL *RasterTextureInitBlitFn)(
  RasterPattern* ctx, const Image& image, const IntRect& irect, const DoubleMatrix& matrix, uint32_t spread, uint32_t imageInterpolationType);

//! @internal
typedef err_t (FOG_FASTCALL *RasterTextureInitScaleFn)(
  RasterPattern* ctx, const Image& image, int dw, int dh, uint32_t imageInterpolationType);

// Gradient.

//! @internal
typedef err_t (FOG_FASTCALL *RasterGradientInitFn)(
  RasterPattern* ctx, const Pattern& pattern, const DoubleMatrix& matrix, uint32_t colorInterpolationType);

// ============================================================================
// [Fog::RasterEngine - Prototypes - Filter]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterColorLutFn)(
  const ColorLutData* lut,
  uint8_t* dst, const uint8_t* src, int width);

//! @internal
typedef void (FOG_FASTCALL *RasterColorMatrixFn)(
  const ColorMatrix* cm,
  uint8_t* dst, const uint8_t* src, int width);

//! @internal
typedef void (FOG_FASTCALL *RasterCopyAreaFn)(
  const void* context,
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  int w, int h, int offset);

//! @internal
typedef void (FOG_FASTCALL *RasterBlurFn)(
  const BlurParams* params,
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  int w, int h, int offset);

//! @internal
typedef void (FOG_FASTCALL *RasterFloatSymmetricConvolveFn)(
  const FloatSymmetricConvolveParams* params,
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  int w, int h, int offset);

//! @internal
typedef void (FOG_FASTCALL *RasterIntSymmetricConvolveFn)(
  const IntSymmetricConvolveParams* params,
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  int w, int h, int offset);

// ============================================================================
// [Fog::RasterEngine - Prototypes - Composite]
// ============================================================================

//! @internal
typedef void (FOG_FASTCALL *RasterCBlitRectFn)(
  uint8_t* dst, sysint_t dstStride,
  const RasterSolid* src,
  int w, int h, const RasterClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RasterCBlitFullFn)(
  uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RasterCBlitSpanFn)(
  uint8_t* dst, const RasterSolid* src, const Span* span, const RasterClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RasterVBlitRectFn)(
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  int w, int h, const RasterClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RasterVBlitFullFn)(
  uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure);

//! @internal
typedef void (FOG_FASTCALL *RasterVBlitSpanFn)(
  uint8_t* dst, const Span* span, const RasterClosure* closure);

// ============================================================================
// [Fog::RasterEngine - Prototypes - Mask]
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
// [Fog::RasterEngine - Data]
// ============================================================================

//! @internal
extern FOG_HIDDEN const uint16_t raster_linear_blur8_mul[255];

//! @internal
extern FOG_HIDDEN const uint8_t raster_linear_blur8_shr[255];

// ============================================================================
// [Fog::RasterClosure]
// ============================================================================

//! @internal
//!
//! @brief RasterClosure structure that is passed to each compositing or blitting 
//! method.
struct RasterClosure
{
  //! @brief If source pixels are indexed, here is link to 256x2 color entries.
  const uint32_t* srcPalette;

  //! @brief If destination pixels are indexed, here is link to 256x2 color 
  //! entries.
  const uint32_t* dstPalette;
};

// ============================================================================
// [Fog::RasterPattern]
// ============================================================================

//! @internal
//!
//! @brief Context that is used to render patterns.
//!
//! Context contains method that can render pattern span at specific coordinate.
//!
//!
//! @section Explanation of inverted affine transformation
//!
//! We are always using inverted affine transformation to fetch gradient or
//! texture (if affine transformation is in use) and some constructs can be
//! very hard to understand.
//!
//! So these steps are generally needed to successfully fetch the transformed
//! image or gradient:
//!
//! 1. Adjust destination coordinates by [0.5, 0.5] - This adjustment moves the
//!    destination to the center of pixel (remember, center is never [0, 0]).
//!
//!    In the code it's usually written as:
//!
//!      double rx = (double)x + 0.5;
//!      double ry = (double)y + 0.5;
//!
//! 2. Transform the centered coordinate using inverted affine matrix. Matrix
//!    is precalculated so you never invert it in fetcher.
//!
//!    The common formula is:
//!
//!      double sx = rx * ctx->m[MATRIX_SX ] + 
//!                  ry * ctx->m[MATRIX_SHX] + 
//!                  ctx->m[MATRIX_TX];
//!
//!      double sy = rx * ctx->m[MATRIX_SHY] + 
//!                  ry * ctx->m[MATRIX_SY ] +
//!                  ctx->m[MATRIX_TY];
//!
//!    But in many cases the resulting coordinates are converted to 16x16 or 48x16
//!    fixed point, using following code:
//!
//!      int fx = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SX ] + 
//!                                        ry * ctx->m[MATRIX_SHX] + 
//!                                        ctx->m[MATRIX_TX]);
//!
//!      int fy = Math::doubleToFixed16x16(rx * ctx->m[MATRIX_SHY] +
//!                                        ry * ctx->m[MATRIX_SY ] +
//!                                        ctx->m[MATRIX_TY]);
//!
//! 3. After transformation we move back the centered pixel:
//!
//!    Following code for non-fixed point version:
//!
//!      sx -= 0.5;
//!      sy -= 0.5;
//!
//!    And fixed point version:
//!
//!      fx -= 0x8000;
//!      fy -= 0x8000;
struct RasterPattern
{
  //! @brief Reference count.
  Atomic<sysuint_t> refCount;

  //! @brief Fetch function.
  RasterPatternFetchFn fetch;
  //! @brief Destroy function (you must call it if reference count was decreased
  //! to zero).
  RasterPatternDestroyFn destroy;

  //! @brief true if this context is initialized and must be destroyed using
  //! @c destroy() function.
  int initialized;
  //! @brief Pixel format of this context (pixel format that will be fetched
  //! using @c fetch() function.
  uint32_t format;
  //! @brief Depth of this context (this is related to pixel format of course).
  uint32_t depth;
  //! @brief Bytes per pixel fetched by the pattern (related to depth).
  uint32_t bytesPerPixel;

  //! @brief True if context is transformed.
  //!
  //! @note This is never set for translation only matrices. Scale or shear is
  //! condition to set this variable to true.
  uint32_t isTransformed;

  //! @brief Embedded matrix data (no @c Matrix instance here).
  double m[6];

  //! @internal
  //!
  //! @brief Data for solid color context.
  struct Solid
  {
    uint32_t argb;
    uint32_t prgb;
  };

  //! @internal
  //!
  //! @brief Data for texture context.
  struct Texture
  {
    // Private, initialized when context is created. These variables are here
    // for easier access into texture data (it saves some pointer dereferences,
    // i don't know if this is optimization or not, but it's easier to access
    // it in pattern fetchers).
    const uint8_t* bits;
    const uint32_t* pal;
    int w;
    int h;
    sysint_t stride;

    // Must be destroyed when context is destroyed and its type is texture.
    Static<Image> texture;

    // dx & dy.
    //
    // - Exact and subpixel mode - translation point.
    // - Affine transformation - One step in 16.16 fixed point.
    int dx;
    int dy;

    // Used for affine transformation, in 16.16 fp.
    int fxmax;
    int fymax;

    int fxrewind;
    int fyrewind;

    // Subpixel offset at 1.0 scale ratio (used if isTransformed is false).
    uint fY0X0;
    uint fY0X1;
    uint fY1X0;
    uint fY1X1;
  };

  //! @internal
  struct Scale
  {
    int dx;
    int dy;

    int sw;
    int sh;
    int dw;
    int dh;

    int* xpoints;
    uint32_t** ypoints;
    int* xapoints;
    int* yapoints;
    int xup_yup;
  };

  //! @internal
  //!
  //! @brief Shared data for linear, radial and conical gradients.
  struct GenericGradient
  {
    // Dynamically allocated, must be free when context is destroyed and its
    // type is gradient.
    uint32_t* colors;
    // Capacity of 'colors' array.
    int colorsAlloc;
    // Count of colors in 'colors' array.
    int colorsLength;

    double dx;
    double dy;
  };

  //! @internal
  //!
  //! @brief Linear gradient data.
  struct LinearGradient
  {
    // See GenericGradient.
    uint32_t* colors;
    int colorsAlloc;
    int colorsLength;

    double dx;
    double dy;

    double ax;
    double ay;

    double dist;
  };

  //! @internal
  //!
  //! @brief Radial gradient data.
  struct RadialGradient
  {
    // See GenericGradient.
    uint32_t* colors;
    int colorsAlloc;
    int colorsLength;

    double dx;
    double dy;

    double fx;
    double fy;
    double r;
    double r2;
    double fx2;
    double fy2;
    double mul;
  };

  //! @internal
  //!
  //! @brief Conical gradient data.
  struct ConicalGradient
  {
    // See GenericGradient.
    uint32_t* colors;
    int colorsAlloc;
    int colorsLength;

    double dx;
    double dy;

    double angle;
  };

  union
  {
    Solid solid;

    Texture texture;
    Scale scale;

    GenericGradient genericGradient;
    LinearGradient linearGradient;
    RadialGradient radialGradient;
    ConicalGradient conicalGradient;
  };
};

// ============================================================================
// [Fog::RasterSolid]
// ============================================================================

//! @internal
//!
//! @brief RasterSolid source color for image compositing.
struct RasterSolid
{
  //! @brief 32-bit ARGB color, non-premultiplied.
  uint32_t argb;
  //! @brief 32-bit ARGB color, premultiplied.
  uint32_t prgb;
};

// ============================================================================
// [Fog::RasterFuncs]
// ============================================================================

//! @internal
//!
//! @brief Function map contains all low-level raster based manipulation methods.
//!
//! Function map is designed for overriding functions by their better 
//! implementation. For example if processor supports MMX or SSE2, some
//! functions are replaced by MMX/SSE2 versions. This way means that there
//! is not needed to check for these features in higher level API.
struct RasterFuncs
{
  // --------------------------------------------------------------------------
  // [DibFuncs]
  // --------------------------------------------------------------------------

  struct DibFuncs
  {
    // [ByteSwap]

    // Fast byteswap methods.
    RasterVBlitFullFn bswap16;
    RasterVBlitFullFn bswap24;
    RasterVBlitFullFn bswap32;

    // [MemCpy]

    // Fast memcpy methods.
    RasterVBlitFullFn memcpy[IMAGE_FORMAT_COUNT];
    RasterVBlitFullFn memcpy8;
    RasterVBlitFullFn memcpy16;
    RasterVBlitFullFn memcpy24;
    RasterVBlitFullFn memcpy32;

    // [Rect]

    RasterCBlitRectFn cblit_rect[IMAGE_FORMAT_COUNT];
    RasterVBlitRectFn vblit_rect[IMAGE_FORMAT_COUNT];

    // [Convert]

    // Note: We are declaring this array as DIB_FORMAT_COUNT * DIB_FORMAT_COUNT,
    // but it's allowed to use it only by these directions:
    //
    // -  [IMAGE_FORMAT_X] <- [DIB_FORMAT_X]
    // -  [DIB_FORMAT_X] <- [IMAGE_FORMAT_X]
    //
    // The reason is that we never convert one dib format to another, instead
    // we always convert our pixel format to dib or vica versa.
    RasterVBlitFullFn convert[DIB_FORMAT_COUNT][DIB_FORMAT_COUNT];

    // [Dither]

    RasterDither8Fn i8rgb232_from_xrgb32_dither;
    RasterDither8Fn i8rgb222_from_xrgb32_dither;
    RasterDither8Fn i8rgb111_from_xrgb32_dither;

    RasterDither16Fn rgb16_555_native_from_xrgb32_dither;
    RasterDither16Fn rgb16_565_native_from_xrgb32_dither;

    RasterDither16Fn rgb16_555_swapped_from_xrgb32_dither;
    RasterDither16Fn rgb16_565_swapped_from_xrgb32_dither;

    // [Helpers]

    RasterVBlitSpanFn prgb32_from_argb32_span;
    RasterVBlitSpanFn argb32_from_prgb32_span;
  };

  DibFuncs dib;

  // --------------------------------------------------------------------------
  // [InterpolateFuncs]
  // --------------------------------------------------------------------------

  struct InterpolateFuncs
  {
    // [Argb]

    RasterInterpolateArgbFn gradient[IMAGE_FORMAT_COUNT];
  };

  InterpolateFuncs interpolate;

  // --------------------------------------------------------------------------
  // [PatternFuncs]
  // --------------------------------------------------------------------------

  struct PatternFuncs
  {
    // [RasterSolid]

    // NOTE: Here is solid_fetch function, but it's helper for other patterns.
    // It's possible that some pattern is badly initialized or it's allowed
    // null data so the solid color will be used. Also for images with ultra
    // scaling the solid color is good choice.
    // 
    // Painter engine should always use solid color fastpath if pattern
    // degrades to solid color.
    RasterSolidInitFn solid_init;
    RasterPatternFetchFn solid_fetch;

    // [Texture]

    RasterTextureInitFn texture_init;
    RasterTextureInitBlitFn texture_init_blit;
    RasterTextureInitScaleFn texture_init_scale;

    // Exact, no transform.
    RasterPatternFetchFn texture_fetch_exact[IMAGE_FORMAT_COUNT][PATTERN_SPREAD_COUNT];

    // Subpixel accurate, no transform. In case that required fetcher is null,
    // bilinear filtering is used (and must be implemented).
    RasterPatternFetchFn texture_fetch_subx0[IMAGE_FORMAT_COUNT][PATTERN_SPREAD_COUNT];
    RasterPatternFetchFn texture_fetch_sub0y[IMAGE_FORMAT_COUNT][PATTERN_SPREAD_COUNT];
    RasterPatternFetchFn texture_fetch_subxy[IMAGE_FORMAT_COUNT][PATTERN_SPREAD_COUNT];

    // Transform, nearest.
    RasterPatternFetchFn texture_fetch_transform_nearest[IMAGE_FORMAT_COUNT][PATTERN_SPREAD_COUNT];
    // Transform, bilinear.
    RasterPatternFetchFn texture_fetch_transform_bilinear[IMAGE_FORMAT_COUNT][PATTERN_SPREAD_COUNT];

    // Scale, nearest.
    RasterPatternFetchFn texture_fetch_scale_nearest[IMAGE_FORMAT_COUNT];
    // Scale, bilinear.
    RasterPatternFetchFn texture_fetch_scale_bilinear[IMAGE_FORMAT_COUNT];

    // [Linear Gradient]

    RasterGradientInitFn linear_gradient_init;

    // Exact is non-antialiased gradient rendering, also used in antialiased
    // mode for vertical and horizontal gradients.
    RasterPatternFetchFn linear_gradient_fetch_exact[4];

    // Subpixel accurate gradient rendering.
    RasterPatternFetchFn linear_gradient_fetch_subxy[4];

    // [Radial Gradient]

    RasterGradientInitFn radial_gradient_init;
    RasterPatternFetchFn radial_gradient_fetch[4];

    // [Conical Gradient]

    RasterGradientInitFn conical_gradient_init;
    RasterPatternFetchFn conical_gradient_fetch;
  };

  PatternFuncs pattern;

  // --------------------------------------------------------------------------
  // [FilterFuncs]
  // --------------------------------------------------------------------------

  struct FilterFuncs
  {
    // [ColorLut / ColorMatrix]

    RasterColorLutFn color_lut[IMAGE_FORMAT_COUNT];
    RasterColorMatrixFn color_matrix[IMAGE_FORMAT_COUNT];

    // [Copy Area]

    RasterCopyAreaFn copy_area[IMAGE_FORMAT_COUNT];

    // [Box Blur]

    RasterBlurFn box_blur_h[IMAGE_FORMAT_COUNT];
    RasterBlurFn box_blur_v[IMAGE_FORMAT_COUNT];

    // [Linear Blur]

    RasterBlurFn linear_blur_h[IMAGE_FORMAT_COUNT];
    RasterBlurFn linear_blur_v[IMAGE_FORMAT_COUNT];

    // [Symmetric Convolve]

    RasterFloatSymmetricConvolveFn symmetric_convolve_float_h[IMAGE_FORMAT_COUNT];
    RasterFloatSymmetricConvolveFn symmetric_convolve_float_v[IMAGE_FORMAT_COUNT];

    RasterIntSymmetricConvolveFn symmetric_convolve_int_h[IMAGE_FORMAT_COUNT];
    RasterIntSymmetricConvolveFn symmetric_convolve_int_v[IMAGE_FORMAT_COUNT];

    // [Generic Convolve]
  };

  FilterFuncs filter;

  // --------------------------------------------------------------------------
  // [CompositeFuncs]
  // --------------------------------------------------------------------------

  struct CompositeFuncs
  {
    // [CBlit]

    RasterCBlitFullFn cblit_full[IMAGE_FORMAT_COUNT];
    RasterCBlitSpanFn cblit_span[IMAGE_FORMAT_COUNT];

    // [VBlit]

    RasterVBlitFullFn vblit_full[IMAGE_FORMAT_COUNT];
    RasterVBlitSpanFn vblit_span[IMAGE_FORMAT_COUNT];
  };

  CompositeFuncs composite[OPERATOR_COUNT][IMAGE_FORMAT_COUNT];

  FOG_INLINE const CompositeFuncs* getCompositeFuncs(uint32_t op, uint32_t format)
  {
    FOG_ASSERT(op < OPERATOR_COUNT);
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);

    return &composite[op][format];
  }

  // --------------------------------------------------------------------------
  // [MaskFuncs]
  // --------------------------------------------------------------------------

  struct MaskFuncs
  {
    RasterMaskCOpVFn c_op_v;
    RasterMaskVOpCFn v_op_c;
    RasterMaskVOpVFn v_op_v;
  };

  MaskFuncs mask[CLIP_OP_COUNT][IMAGE_FORMAT_COUNT];

  FOG_INLINE const MaskFuncs* getMaskFuncs(uint32_t op, uint32_t format)
  {
    FOG_ASSERT(op < CLIP_OP_COUNT);
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);

    return &mask[op][format];
  }
};

//! @internal
extern FOG_API RasterFuncs rasterFuncs;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_RASTERENGINE_P_H
