// [Fog-Graphics library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERENGINE_P_H
#define _FOG_GRAPHICS_RASTERENGINE_P_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/Scanline_p.h>

//! @addtogroup Fog_Graphics_Private
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

// Used in function map prototypes.
struct BlurParams;
struct ColorLut;
struct ColorMatrix;
struct ConvolveParams;
struct DoubleMatrix;
struct Pattern;
struct SymmetricConvolveParamsF;
struct SymmetricConvolveParamsI;

namespace RasterEngine {

struct Closure;
struct PatternContext;
struct Solid;

// ============================================================================
// [Fog::RasterEngine - Prototypes - Dither]
// ============================================================================

typedef void (FOG_FASTCALL *Dither8Fn)(
  uint8_t* dst, const uint8_t* src, sysint_t w, const IntPoint& origin, const uint8_t* palConv);

typedef void (FOG_FASTCALL *Dither16Fn)(
  uint8_t* dst, const uint8_t* src, sysint_t w, const IntPoint& origin);

// ============================================================================
// [Fog::RasterEngine - Prototypes - Interpolate]
// ============================================================================

typedef void (FOG_FASTCALL *InterpolateArgbFn)(
  uint8_t* dst, uint32_t c0, uint32_t c1,
  sysint_t w, sysint_t x1, sysint_t x2);

// ============================================================================
// [Fog::RasterEngine - Prototypes - Pattern]
// ============================================================================

// Generic.
typedef err_t (FOG_FASTCALL *PatternInitFn)(
  PatternContext* ctx, const Pattern& pattern, const DoubleMatrix& matrix);

typedef void (FOG_FASTCALL *PatternDestroyFn)(
  PatternContext* ctx);

typedef uint8_t* (FOG_FASTCALL *PatternFetchFn)(
  PatternContext* ctx, uint8_t* dst, int x, int y, int w);

// Solid.
typedef err_t (FOG_FASTCALL *SolidInitFn)(
  PatternContext* ctx, uint32_t prgb);

// Texture.
typedef err_t (FOG_FASTCALL *TextureInitFn)(
  PatternContext* ctx, const Pattern& pattern, const DoubleMatrix& matrix, int interpolationType);

typedef err_t (FOG_FASTCALL *TextureInitBlitFn)(
  PatternContext* ctx, const Image& image, const DoubleMatrix& matrix, int spread, int interpolationType);

typedef err_t (FOG_FASTCALL *TextureInitScaleFn)(
  PatternContext* ctx, const Image& image, int dw, int dh, int interpolationType);

// Gradient.
typedef err_t (FOG_FASTCALL *GradientInitFn)(
  PatternContext* ctx, const Pattern& pattern, const DoubleMatrix& matrix, int interpolationType);

// ============================================================================
// [Fog::RasterEngine - Prototypes - Filter]
// ============================================================================

typedef void (FOG_FASTCALL *ColorLutFn)(
  const ColorLutData* lut,
  uint8_t* dst, const uint8_t* src, sysuint_t width);

typedef void (FOG_FASTCALL *ColorMatrixFn)(
  const ColorMatrix* cm,
  uint8_t* dst, const uint8_t* src, sysuint_t width);

typedef void (FOG_FASTCALL *CopyAreaFn)(
  const void* context,
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  sysuint_t w, sysuint_t h, sysint_t offset);

typedef void (FOG_FASTCALL *BlurFn)(
  const BlurParams* params,
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  sysuint_t w, sysuint_t h, sysint_t offset);

typedef void (FOG_FASTCALL *SymmetricConvolveFloatFn)(
  const SymmetricConvolveParamsF* params,
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  sysuint_t w, sysuint_t h, sysint_t offset);

typedef void (FOG_FASTCALL *SymmetricConvolveIntFn)(
  const SymmetricConvolveParamsI* params,
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  sysuint_t w, sysuint_t h, sysint_t offset);

// ============================================================================
// [Fog::RasterEngine - Prototypes - Composite]
// ============================================================================

typedef void (FOG_FASTCALL *CSpanFn)(
  uint8_t* dst, const Solid* src, sysint_t w, const Closure* closure);
typedef void (FOG_FASTCALL *CSpanMskFn)(
  uint8_t* dst, const Solid* src, const uint8_t* msk, sysint_t w, const Closure* closure);
typedef void (FOG_FASTCALL *CSpanMskConstFn)(
  uint8_t* dst, const Solid* src, uint32_t msk, sysint_t w, const Closure* closure);
typedef void (FOG_FASTCALL *CSpanScanlineFn)(
  uint8_t* dst, const Solid* src, const Scanline32::Span* spans, sysuint_t numSpans, const Closure* closure);

typedef void (FOG_FASTCALL *VSpanFn)(
  uint8_t* dst, const uint8_t* src, sysint_t w, const Closure* closure);
typedef void (FOG_FASTCALL *VSpanMskFn)(
  uint8_t* dst, const uint8_t* src, const uint8_t* msk, sysint_t w, const Closure* closure);
typedef void (FOG_FASTCALL *VSpanMskConstFn)(
  uint8_t* dst, const uint8_t* src, uint32_t msk, sysint_t w, const Closure* closure);

// ============================================================================
// [Fog::RasterEngine - Data]
// ============================================================================

extern FOG_HIDDEN const uint16_t linear_blur8_mul[255]; // [255]
extern FOG_HIDDEN const uint8_t linear_blur8_shr[255];  // [255]

// ============================================================================
// [Fog::RasterEngine - Structures]
// ============================================================================

//! @brief Solid source color for image compositing.
struct Solid
{
  //! @brief 32-bit ARGB color, non-premultiplied.
  uint32_t argb;
  //! @brief 32-bit ARGB color, premultiplied.
  uint32_t prgb;
};

//! @brief Closure structure that is passed to each compositing or blitting 
//! method.
struct Closure
{
  //! @brief If source pixels are indexed, here is link to 256 color entries.
  const Argb* srcPalette;
  //! @brief If destination pixels are indexed, here is link to 256 color entries.
  const Argb* dstPalette;
};

//! @brief Context that is used to render patterns.
//!
//! Context contains method that can render pattern span at specific coordinate.
struct PatternContext
{
  //! @brief Reference count.
  Atomic<sysuint_t> refCount;

  //! @brief Fetch function.
  PatternFetchFn fetch;
  //! @brief Destroy function (you must call it if reference count was decreased
  //! to zero).
  PatternDestroyFn destroy;

  //! @brief true if this context is initialized and must be destroyed using
  //! @c destroy() function.
  int initialized;
  //! @brief Pixel format of this context (pixel format that will be fetched
  //! using @c fetch() function.
  int format;
  //! @brief Depth of this context (this is related to pixel format of course).
  int depth;

  //! @brief True if context is transformed.
  //!
  //! @note This is never set for translation only matrices. Scale or shear is
  //! condition to set this variable to true.
  int isTransformed;

  //! @brief Embedded matrix data (no @c Matrix instance here).
  double m[6];

  //! @brief Data for solid color context.
  struct Solid
  {
    uint32_t prgb;
  };

  //! @brief Data for texture context.
  struct Texture
  {
    // Private, initialized when context is created. These variables are here
    // for easier access into texture data (it saves some pointer dereferences,
    // i don't know if this is optimization or not, but it's easier to access
    // it in pattern fetchers).
    const uint8_t* bits;
    const Argb* pal;
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
// [Fog::RasterEngine - Function Map]
// ============================================================================

//! @brief Function map contains all low-level raster based manipulation methods.
//!
//! Function map is designed for overriding functions by their better 
//! implementation. For example if processor supports MMX or SSE2, some
//! functions are replaced by MMX/SSE2 versions. This way means that there
//! is not needed to check for these features in higher level API.
struct FunctionMap
{
  // [Dib]

  struct DibFuncs
  {
    // [ByteSwap]

    // Fast byteswap methods.
    VSpanFn bswap16;
    VSpanFn bswap24;
    VSpanFn bswap32;

    // [MemCpy]

    // Fast memcpy methods.
    VSpanFn memcpy8;
    VSpanFn memcpy16;
    VSpanFn memcpy24;
    VSpanFn memcpy32;

    // [Convert]

    // Note: We are declaring this array as DIB_FORMAT_COUNT * DIB_FORMAT_COUNT,
    // but it's allowed to use it only by these directions:
    //
    // -  [PIXEL_FORMAT_X] <- [DIB_FORMAT_X]
    // -  [DIB_FORMAT_X] <- [PIXEL_FORMAT_X]
    //
    // The reason is that we never convert one dib format to another, instead
    // we always convert out pixel format to dib or vica versa.
    VSpanFn convert[DIB_FORMAT_COUNT][DIB_FORMAT_COUNT];

    // [Dither]

    Dither8Fn i8rgb232_from_xrgb32_dither;
    Dither8Fn i8rgb222_from_xrgb32_dither;
    Dither8Fn i8rgb111_from_xrgb32_dither;

    Dither16Fn rgb16_555_native_from_xrgb32_dither;
    Dither16Fn rgb16_565_native_from_xrgb32_dither;

    Dither16Fn rgb16_555_swapped_from_xrgb32_dither;
    Dither16Fn rgb16_565_swapped_from_xrgb32_dither;
  };

  DibFuncs dib;

  // [Interpolation]

  struct InterpolateFuncs
  {
    // [Gradient]

    InterpolateArgbFn gradient[PIXEL_FORMAT_COUNT];
  };

  InterpolateFuncs interpolate;

  // [Pattern]

  struct PatternFuncs
  {
    // [Solid]

    // NOTE: Here is solid_fetch function, but it's helper for other patterns.
    // It's possible that some pattern is badly initialized or it's allowed
    // null data so the solid color will be used. Also for images with ultra
    // scaling the solid color is good choice.
    // 
    // Painter engine should always use solid color fastpath if pattern
    // degrades to solid color.
    SolidInitFn solid_init;
    PatternFetchFn solid_fetch;

    // [Texture]

    TextureInitFn texture_init;
    TextureInitBlitFn texture_init_blit;
    TextureInitScaleFn texture_init_scale;

    // Exact, no transform.
    PatternFetchFn texture_fetch_exact[PIXEL_FORMAT_COUNT][SPREAD_COUNT];

    // Subpixel accurate, no transform. In case that required fetcher is null,
    // bilinear filtering is used (and must be implemented).
    PatternFetchFn texture_fetch_subx0[PIXEL_FORMAT_COUNT][SPREAD_COUNT];
    PatternFetchFn texture_fetch_sub0y[PIXEL_FORMAT_COUNT][SPREAD_COUNT];
    PatternFetchFn texture_fetch_subxy[PIXEL_FORMAT_COUNT][SPREAD_COUNT];

    // Transform, nearest.
    PatternFetchFn texture_fetch_transform_nearest[PIXEL_FORMAT_COUNT][SPREAD_COUNT];
    // Transform, bilinear.
    PatternFetchFn texture_fetch_transform_bilinear[PIXEL_FORMAT_COUNT][SPREAD_COUNT];

    // Scale, nearest.
    PatternFetchFn texture_fetch_scale_nearest[PIXEL_FORMAT_COUNT];
    // Scale, bilinear.
    PatternFetchFn texture_fetch_scale_bilinear[PIXEL_FORMAT_COUNT];

    // [Linear Gradient]

    GradientInitFn linear_gradient_init;

    // Exact is non-antialiased gradient rendering, also used in antialiased
    // mode for vertical and horizontal gradients.
    PatternFetchFn linear_gradient_fetch_exact[4];

    // Subpixel accurate gradient rendering.
    PatternFetchFn linear_gradient_fetch_subxy[4];

    // [Radial Gradient]

    GradientInitFn radial_gradient_init;
    PatternFetchFn radial_gradient_fetch[4];

    // [Conical Gradient]

    GradientInitFn conical_gradient_init;
    PatternFetchFn conical_gradient_fetch;
  };

  PatternFuncs pattern;

  // [Filter]

  struct FilterFuncs
  {
    // [ColorLut / ColorMatrix]

    ColorLutFn color_lut[PIXEL_FORMAT_COUNT];
    ColorMatrixFn color_matrix[PIXEL_FORMAT_COUNT];

    // [Copy Area]

    CopyAreaFn copy_area[PIXEL_FORMAT_COUNT];

    // [Box Blur]

    BlurFn box_blur_h[PIXEL_FORMAT_COUNT];
    BlurFn box_blur_v[PIXEL_FORMAT_COUNT];

    // [Linear Blur]

    BlurFn linear_blur_h[PIXEL_FORMAT_COUNT];
    BlurFn linear_blur_v[PIXEL_FORMAT_COUNT];

    // [Symmetric Convolve]

    SymmetricConvolveFloatFn symmetric_convolve_float_h[PIXEL_FORMAT_COUNT];
    SymmetricConvolveFloatFn symmetric_convolve_float_v[PIXEL_FORMAT_COUNT];

    SymmetricConvolveIntFn symmetric_convolve_int_h[PIXEL_FORMAT_COUNT];
    SymmetricConvolveIntFn symmetric_convolve_int_v[PIXEL_FORMAT_COUNT];

    // [Generic Convolve]
  };

  FilterFuncs filter;

  // [Composite]

  struct CompositeFuncs
  {
    // [Span Solid]

    CSpanFn cspan;
    CSpanMskFn cspan_a8;
    CSpanMskConstFn cspan_a8_const;
    CSpanScanlineFn cspan_a8_scanline;

    // [Span Composite]

    VSpanFn vspan[PIXEL_FORMAT_COUNT];
    VSpanMskFn vspan_a8[PIXEL_FORMAT_COUNT];
    VSpanMskConstFn vspan_a8_const[PIXEL_FORMAT_COUNT];
  };

  CompositeFuncs composite[OPERATOR_COUNT][PIXEL_FORMAT_COUNT];
};

extern FOG_API FunctionMap* functionMap;

// ============================================================================
// [Fog::RasterEngine - getRasterOps]
// ============================================================================

FOG_INLINE FunctionMap::CompositeFuncs* getCompositeFuncs(uint32_t format, uint32_t op)
{
  FOG_ASSERT((uint)format < PIXEL_FORMAT_COUNT);
  FOG_ASSERT((uint)op < OPERATOR_COUNT);

  return &functionMap->composite[(uint)op][(uint)format];
}

} // RasterEngine namespace
} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_RASTERENGINE_P_H
