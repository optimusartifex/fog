// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Build/Build.h>

// Don't build MMX under 64-bit mode. MSVC complains about MMX in 64-bit mode
// and it's always overriden by SSE2 implementation that is guaranted for every
// 64-bit processor.
#if defined(FOG_ARCH_X86)

// ============================================================================
// [Warnings]
// ============================================================================

#if defined(FOG_CC_MSVC)
#pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSVC

// ============================================================================
// [Dependencies]
// ============================================================================

#include <Fog/Core/Intrin_MMX.h>
#include <Fog/Core/Intrin_3dNow.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Misc.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/ArgbAnalyzer_p.h>
#include <Fog/Graphics/ArgbUtil.h>
#include <Fog/Graphics/ByteUtil_p.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/ColorMatrix.h>
#include <Fog/Graphics/Constants_p.h>
#include <Fog/Graphics/DitherMatrix_p.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageFilter.h>
#include <Fog/Graphics/ImageFilterEngine.h>
#include <Fog/Graphics/ImageFilterParams.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/Pattern.h>
#include <Fog/Graphics/RasterEngine_p.h>

// ============================================================================
// [RasterEngine - MMX]
// ============================================================================

#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Dib_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Interpolate_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Scale_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Filters_MMX_p.h>

// ============================================================================
// [Library Initializers]
// ============================================================================

#define MMX_INIT_NAME fog_rasterengine_init_mmx

#if defined(FOG_RASTER_MMX3DNOW)
#undef MMX_INIT_NAME
#define MMX_INIT_NAME fog_rasterengine_init_mmx3dnow
#endif

#if defined(FOG_RASTER_MMXSSE)
#undef MMX_INIT_NAME
#define MMX_INIT_NAME fog_rasterengine_init_mmxsse
#endif

FOG_INIT_DECLARE void MMX_INIT_NAME(void)
{
  using namespace Fog;
  using namespace Fog::RasterEngine;

  FunctionMap* m = functionMap;

  // [Dib - MemCpy]

//m->dib.memcpy8 = MMX_SYM(Dib)::memcpy8;
//m->dib.memcpy16 = MMX_SYM(Dib)::memcpy16;
//m->dib.memcpy24 = MMX_SYM(Dib)::memcpy24;
  m->dib.memcpy32 = MMX_SYM(Dib)::memcpy32;

  // [Dib - Convert]

  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = MMX_SYM(Dib)::memcpy32;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = MMX_SYM(Dib)::prgb32_from_argb32;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_xrgb32;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_A8               ] = MMX_SYM(Dib)::azzz32_from_a8;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_I8               ] = MMX_SYM(Dib)::prgb32_from_i8;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = MMX_SYM(Dib)::bswap32;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = MMX_SYM(Dib)::prgb32_from_argb32_swapped;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = MMX_SYM(Dib)::frgb32_from_xrgb32_swapped;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB24_NATIVE     ] = MMX_SYM(Dib)::frgb32_from_rgb24_native;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = MMX_SYM(Dib)::frgb32_from_rgb24_swapped;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_565_native;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_565_swapped;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_555_native;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_555_swapped;
//m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_GREY8            ] = MMX_SYM(Dib)::frgb32_from_grey8;

//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_PRGB32_NATIVE    ] = MMX_SYM(Dib)::argb32_from_prgb32;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_ARGB32_NATIVE    ] = MMX_SYM(Dib)::memcpy32;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_XRGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_xrgb32;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_A8               ] = MMX_SYM(Dib)::azzz32_from_a8;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_I8               ] = MMX_SYM(Dib)::argb32_from_i8;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = MMX_SYM(Dib)::argb32_from_prgb32_swapped;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = MMX_SYM(Dib)::bswap32;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = MMX_SYM(Dib)::frgb32_from_xrgb32_swapped;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB24_NATIVE     ] = MMX_SYM(Dib)::frgb32_from_rgb24_native;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB24_SWAPPED    ] = MMX_SYM(Dib)::frgb32_from_rgb24_swapped;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_565_native;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_565_swapped;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_555_native;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_555_swapped;
//m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_GREY8            ] = MMX_SYM(Dib)::frgb32_from_grey8;

//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_xrgb32;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_argb32;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_xrgb32;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_A8               ] = MMX_SYM(Dib)::fzzz32_from_null;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_I8               ] = MMX_SYM(Dib)::frgb32_from_i8;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = MMX_SYM(Dib)::frgb32_from_xrgb32_swapped;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = MMX_SYM(Dib)::frgb32_from_argb32_swapped;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = MMX_SYM(Dib)::bswap32;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB24_NATIVE     ] = MMX_SYM(Dib)::frgb32_from_rgb24_native;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = MMX_SYM(Dib)::frgb32_from_rgb24_swapped;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_565_native;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_565_swapped;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_555_native;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_555_swapped;
//m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_GREY8            ] = MMX_SYM(Dib)::frgb32_from_grey8;

//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_PRGB32_NATIVE    ] = MMX_SYM(Dib)::a8_from_axxx32;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_ARGB32_NATIVE    ] = MMX_SYM(Dib)::a8_from_axxx32;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_XRGB32_NATIVE    ] = MMX_SYM(Dib)::f8_from_null;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_A8               ] = MMX_SYM(Dib)::memcpy8;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_I8               ] = MMX_SYM(Dib)::a8_from_i8;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_PRGB32_SWAPPED   ] = MMX_SYM(Dib)::a8_from_axxx32_swapped;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_ARGB32_SWAPPED   ] = MMX_SYM(Dib)::a8_from_axxx32_swapped;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_XRGB32_SWAPPED   ] = MMX_SYM(Dib)::f8_from_null;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB24_NATIVE     ] = MMX_SYM(Dib)::f8_from_null;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB24_SWAPPED    ] = MMX_SYM(Dib)::f8_from_null;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB16_565_NATIVE ] = MMX_SYM(Dib)::f8_from_null;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB16_565_SWAPPED] = MMX_SYM(Dib)::f8_from_null;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB16_555_NATIVE ] = MMX_SYM(Dib)::f8_from_null;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB16_555_SWAPPED] = MMX_SYM(Dib)::f8_from_null;
//m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_GREY8            ] = MMX_SYM(Dib)::f8_from_null;

//m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::bswap32;
//m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::prgb32_swapped_from_argb32;
//m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::frgb32_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::azzz32_swapped_from_a8;
//m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::prgb32_swapped_from_i8;

//m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::argb32_swapped_from_prgb32;
//m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::bswap32;
//m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::frgb32_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::azzz32_swapped_from_a8;
//m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::argb32_swapped_from_i8;

//m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::frgb32_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::frgb32_swapped_from_argb32;
//m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::frgb32_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::fzzz32_swapped_from_null;
//m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::frgb32_swapped_from_i8;

//m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb24_native_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb24_native_from_argb32;
//m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb24_native_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::zzz24_from_null;
//m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::rgb24_native_from_i8;

//m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb24_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb24_swapped_from_argb32;
//m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb24_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::zzz24_from_null;
//m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::rgb24_swapped_from_i8;

  m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb16_565_native_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb16_565_native_from_argb32;
  m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb16_565_native_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::zzz16_from_null;
//m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::rgb16_565_native_from_i8;

//m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb16_565_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb16_565_swapped_from_argb32;
//m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb16_565_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::zzz16_from_null;
//m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::rgb16_565_swapped_from_i8;

  m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb16_555_native_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb16_555_native_from_argb32;
  m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb16_555_native_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::zzz16_from_null;
//m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::rgb16_555_native_from_i8;

//m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb16_555_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb16_555_swapped_from_argb32;
//m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb16_555_swapped_from_xrgb32;
//m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::zzz16_from_null;
//m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::rgb16_555_swapped_from_i8;

//m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_PRGB32] = MMX_SYM(Dib)::grey8_from_xrgb32;
//m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_ARGB32] = MMX_SYM(Dib)::grey8_from_argb32;
//m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_XRGB32] = MMX_SYM(Dib)::grey8_from_xrgb32;
//m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_A8    ] = MMX_SYM(Dib)::z8_from_null;
//m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_I8    ] = MMX_SYM(Dib)::grey8_from_i8;

  // [Interpolate - Gradient]

  m->interpolate.gradient[PIXEL_FORMAT_PRGB32] = MMX_SYM(Interpolate)::gradient_prgb32;
  m->interpolate.gradient[PIXEL_FORMAT_ARGB32] = MMX_SYM(Interpolate)::gradient_argb32;
  m->interpolate.gradient[PIXEL_FORMAT_XRGB32] = MMX_SYM(Interpolate)::gradient_argb32;

  // [Filters - ColorMatrix]

#if defined(FOG_RASTER_MMX3DNOW)
  m->filter.color_matrix[PIXEL_FORMAT_PRGB32] = MMX_SYM(Filter)::color_matrix_prgb32;
  m->filter.color_matrix[PIXEL_FORMAT_ARGB32] = MMX_SYM(Filter)::color_matrix_argb32;
  m->filter.color_matrix[PIXEL_FORMAT_XRGB32] = MMX_SYM(Filter)::color_matrix_xrgb32;
#endif // FOG_RASTER_MMX3DNOW

  // [Composite - Src]

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].cspan = MMX_SYM(CompositeSrc)::prgb32_cspan;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].cspan_a8 = MMX_SYM(CompositeSrc)::prgb32_cspan_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].cspan_a8_const = MMX_SYM(CompositeSrc)::prgb32_cspan_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = MMX_SYM(CompositeSrc)::prgb32_cspan_a8_scanline;

//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = DibSSE2::memcpy32;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = DibSSE2::prgb32_from_argb32;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = DibSSE2::frgb32_from_xrgb32;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_A8] = DibSSE2::azzz32_from_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_I8] = DibSSE2::prgb32_from_i8;

//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = MMX_SYM(CompositeSrc)::prgb32_vspan_prgb32_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = MMX_SYM(CompositeSrc)::prgb32_vspan_argb32_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = MMX_SYM(CompositeSrc)::prgb32_vspan_xrgb32_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_A8] = MMX_SYM(CompositeSrc)::prgb32_vspan_a8_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_I8] = MMX_SYM(CompositeSrc)::prgb32_vspan_i8_a8;

//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = MMX_SYM(CompositeSrc)::prgb32_vspan_prgb32_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = MMX_SYM(CompositeSrc)::prgb32_vspan_argb32_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = MMX_SYM(CompositeSrc)::prgb32_vspan_xrgb32_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = MMX_SYM(CompositeSrc)::prgb32_vspan_a8_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_I8] = MMX_SYM(CompositeSrc)::prgb32_vspan_i8_a8_const;

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].cspan = MMX_SYM(CompositeSrc)::xrgb32_cspan;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].cspan_a8 = MMX_SYM(CompositeSrc)::xrgb32_cspan_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].cspan_a8_const = MMX_SYM(CompositeSrc)::xrgb32_cspan_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = MMX_SYM(CompositeSrc)::xrgb32_cspan_a8_scanline;

//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = DibSSE2::frgb32_from_xrgb32;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = DibSSE2::frgb32_from_argb32;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = DibSSE2::memcpy32;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_A8] = (VSpanFn)CompositeClearSSE2::xrgb32_cspan;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_I8] = DibSSE2::frgb32_from_i8;
 
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = MMX_SYM(CompositeSrc)::xrgb32_vspan_xrgb32_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = MMX_SYM(CompositeSrc)::xrgb32_vspan_argb32_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = MMX_SYM(CompositeSrc)::xrgb32_vspan_xrgb32_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_A8] = (VSpanMskFn)CompositeClearSSE2::xrgb32_cspan_a8;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_I8] = MMX_SYM(CompositeSrc)::xrgb32_vspan_i8_a8;

//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = MMX_SYM(CompositeSrc)::xrgb32_vspan_xrgb32_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = MMX_SYM(CompositeSrc)::xrgb32_vspan_argb32_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = MMX_SYM(CompositeSrc)::xrgb32_vspan_xrgb32_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = (VSpanMskConstFn)CompositeClearSSE2::xrgb32_cspan_a8_const;
//m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_I8] = MMX_SYM(CompositeSrc)::xrgb32_vspan_i8_a8_const;

  // [Composite - SrcOver]

  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = MMX_SYM(CompositeSrcOver)::prgb32_vspan_prgb32;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = MMX_SYM(CompositeSrcOver)::prgb32_vspan_prgb32;
}

#endif // FOG_ARCH_X86
