// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
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

// [Raster_C]
#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#include <Fog/Graphics/RasterEngine/Interpolate_C_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_C_p.h>
#include <Fog/Graphics/RasterEngine/Scale_C_p.h>
#include <Fog/Graphics/RasterEngine/Filters_C_p.h>

// Decide whether pure C implementation is needed. In case that we are compiling
// for SSE2-only machine, we can omit C implementation that will result in
// smaller binary size of the library.
#if !defined(FOG_HARDCODE_SSE2)
# define FOG_RASTERUTIL_INIT_C
#endif // FOG_HARDCODE_SSE2

// ============================================================================
// [Library Initializers]
// ============================================================================

static void fog_rasterengine_set_nops(Fog::RasterEngine::FunctionMap::CompositeFuncs* ops)
{
  using namespace Fog;
  using namespace Fog::RasterEngine;

  ops->cspan = CompositeNopC::cspan;
  ops->cspan_a8 = CompositeNopC::cspan_a8;
  ops->cspan_a8_const = CompositeNopC::cspan_a8_const;
  ops->cspan_a8_scanline = CompositeNopC::cspan_a8_scanline;

  ops->vspan[PIXEL_FORMAT_ARGB32] = (VSpanFn)CompositeNopC::cspan;
  ops->vspan[PIXEL_FORMAT_PRGB32] = (VSpanFn)CompositeNopC::cspan;
  ops->vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeNopC::cspan;
  ops->vspan[PIXEL_FORMAT_A8] = (VSpanFn)CompositeNopC::cspan;
  ops->vspan[PIXEL_FORMAT_I8] = (VSpanFn)CompositeNopC::cspan;

  ops->vspan_a8[PIXEL_FORMAT_ARGB32] = (VSpanMskFn)CompositeNopC::cspan_a8;
  ops->vspan_a8[PIXEL_FORMAT_PRGB32] = (VSpanMskFn)CompositeNopC::cspan_a8;
  ops->vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeNopC::cspan_a8;
  ops->vspan_a8[PIXEL_FORMAT_A8] = (VSpanMskFn)CompositeNopC::cspan_a8;
  ops->vspan_a8[PIXEL_FORMAT_I8] = (VSpanMskFn)CompositeNopC::cspan_a8;

  ops->vspan_a8_const[PIXEL_FORMAT_ARGB32] = (VSpanMskConstFn)CompositeNopC::cspan_a8_const;
  ops->vspan_a8_const[PIXEL_FORMAT_PRGB32] = (VSpanMskConstFn)CompositeNopC::cspan_a8_const;
  ops->vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeNopC::cspan_a8_const;
  ops->vspan_a8_const[PIXEL_FORMAT_A8] = (VSpanMskConstFn)CompositeNopC::cspan_a8_const;
  ops->vspan_a8_const[PIXEL_FORMAT_I8] = (VSpanMskConstFn)CompositeNopC::cspan_a8_const;
}

FOG_INIT_DECLARE void fog_rasterengine_init_c(void)
{
  using namespace Fog;
  using namespace Fog::RasterEngine;

  FunctionMap* m = functionMap;

  // [Dib - ByteSwap]

  m->dib.bswap16 = DibC::bswap16;
  m->dib.bswap24 = DibC::bswap24;
  m->dib.bswap32 = DibC::bswap32;

  // [Dib - MemCpy]

  m->dib.memcpy8 = DibC::memcpy8;
  m->dib.memcpy16 = DibC::memcpy16;
  m->dib.memcpy24 = DibC::memcpy24;
  m->dib.memcpy32 = DibC::memcpy32;

  // [Dib - Convert]

  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = DibC::memcpy32;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = DibC::prgb32_from_argb32;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = DibC::frgb32_from_xrgb32;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_A8               ] = DibC::azzz32_from_a8;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_I8               ] = DibC::prgb32_from_i8;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = DibC::bswap32;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = DibC::prgb32_from_argb32_swapped;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = DibC::frgb32_from_xrgb32_swapped;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB24_NATIVE     ] = DibC::frgb32_from_rgb24_native;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = DibC::frgb32_from_rgb24_swapped;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = DibC::frgb32_from_rgb16_565_native;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = DibC::frgb32_from_rgb16_565_swapped;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = DibC::frgb32_from_rgb16_555_native;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = DibC::frgb32_from_rgb16_555_swapped;
  m->dib.convert[PIXEL_FORMAT_PRGB32][DIB_FORMAT_GREY8            ] = DibC::frgb32_from_grey8;

  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_PRGB32_NATIVE    ] = DibC::argb32_from_prgb32;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_ARGB32_NATIVE    ] = DibC::memcpy32;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_XRGB32_NATIVE    ] = DibC::frgb32_from_xrgb32;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_A8               ] = DibC::azzz32_from_a8;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_I8               ] = DibC::argb32_from_i8;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = DibC::argb32_from_prgb32_swapped;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = DibC::bswap32;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = DibC::frgb32_from_xrgb32_swapped;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB24_NATIVE     ] = DibC::frgb32_from_rgb24_native;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB24_SWAPPED    ] = DibC::frgb32_from_rgb24_swapped;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_NATIVE ] = DibC::frgb32_from_rgb16_565_native;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_SWAPPED] = DibC::frgb32_from_rgb16_565_swapped;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_NATIVE ] = DibC::frgb32_from_rgb16_555_native;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_SWAPPED] = DibC::frgb32_from_rgb16_555_swapped;
  m->dib.convert[PIXEL_FORMAT_ARGB32][DIB_FORMAT_GREY8            ] = DibC::frgb32_from_grey8;

  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = DibC::frgb32_from_xrgb32;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = DibC::frgb32_from_argb32;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = DibC::frgb32_from_xrgb32;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_A8               ] = DibC::fzzz32_from_null;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_I8               ] = DibC::frgb32_from_i8;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = DibC::frgb32_from_xrgb32_swapped;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = DibC::frgb32_from_argb32_swapped;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = DibC::bswap32;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB24_NATIVE     ] = DibC::frgb32_from_rgb24_native;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = DibC::frgb32_from_rgb24_swapped;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = DibC::frgb32_from_rgb16_565_native;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = DibC::frgb32_from_rgb16_565_swapped;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = DibC::frgb32_from_rgb16_555_native;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = DibC::frgb32_from_rgb16_555_swapped;
  m->dib.convert[PIXEL_FORMAT_XRGB32][DIB_FORMAT_GREY8            ] = DibC::frgb32_from_grey8;

  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_PRGB32_NATIVE    ] = DibC::a8_from_axxx32;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_ARGB32_NATIVE    ] = DibC::a8_from_axxx32;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_XRGB32_NATIVE    ] = DibC::f8_from_null;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_A8               ] = DibC::memcpy8;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_I8               ] = DibC::a8_from_i8;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_PRGB32_SWAPPED   ] = DibC::a8_from_axxx32_swapped;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_ARGB32_SWAPPED   ] = DibC::a8_from_axxx32_swapped;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_XRGB32_SWAPPED   ] = DibC::f8_from_null;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB24_NATIVE     ] = DibC::f8_from_null;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB24_SWAPPED    ] = DibC::f8_from_null;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB16_565_NATIVE ] = DibC::f8_from_null;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB16_565_SWAPPED] = DibC::f8_from_null;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB16_555_NATIVE ] = DibC::f8_from_null;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_RGB16_555_SWAPPED] = DibC::f8_from_null;
  m->dib.convert[PIXEL_FORMAT_A8    ][DIB_FORMAT_GREY8            ] = DibC::f8_from_null;

  m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_PRGB32] = DibC::bswap32;
  m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_ARGB32] = DibC::prgb32_swapped_from_argb32;
  m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_XRGB32] = DibC::frgb32_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_A8    ] = DibC::azzz32_swapped_from_a8;
  m->dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][PIXEL_FORMAT_I8    ] = DibC::prgb32_swapped_from_i8;

  m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_PRGB32] = DibC::argb32_swapped_from_prgb32;
  m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_ARGB32] = DibC::bswap32;
  m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_XRGB32] = DibC::frgb32_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_A8    ] = DibC::azzz32_swapped_from_a8;
  m->dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][PIXEL_FORMAT_I8    ] = DibC::argb32_swapped_from_i8;

  m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_PRGB32] = DibC::frgb32_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_ARGB32] = DibC::frgb32_swapped_from_argb32;
  m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_XRGB32] = DibC::frgb32_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_A8    ] = DibC::fzzz32_swapped_from_null;
  m->dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][PIXEL_FORMAT_I8    ] = DibC::frgb32_swapped_from_i8;

  m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_PRGB32] = DibC::rgb24_native_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_ARGB32] = DibC::rgb24_native_from_argb32;
  m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_XRGB32] = DibC::rgb24_native_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_A8    ] = DibC::zzz24_from_null;
  m->dib.convert[DIB_FORMAT_RGB24_NATIVE     ][PIXEL_FORMAT_I8    ] = DibC::rgb24_native_from_i8;

  m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_PRGB32] = DibC::rgb24_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_ARGB32] = DibC::rgb24_swapped_from_argb32;
  m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_XRGB32] = DibC::rgb24_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_A8    ] = DibC::zzz24_from_null;
  m->dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][PIXEL_FORMAT_I8    ] = DibC::rgb24_swapped_from_i8;

  m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_PRGB32] = DibC::rgb16_565_native_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_ARGB32] = DibC::rgb16_565_native_from_argb32;
  m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_XRGB32] = DibC::rgb16_565_native_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_A8    ] = DibC::zzz16_from_null;
  m->dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][PIXEL_FORMAT_I8    ] = DibC::rgb16_565_native_from_i8;

  m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_PRGB32] = DibC::rgb16_565_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_ARGB32] = DibC::rgb16_565_swapped_from_argb32;
  m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_XRGB32] = DibC::rgb16_565_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_A8    ] = DibC::zzz16_from_null;
  m->dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][PIXEL_FORMAT_I8    ] = DibC::rgb16_565_swapped_from_i8;

  m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_PRGB32] = DibC::rgb16_555_native_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_ARGB32] = DibC::rgb16_555_native_from_argb32;
  m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_XRGB32] = DibC::rgb16_555_native_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_A8    ] = DibC::zzz16_from_null;
  m->dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][PIXEL_FORMAT_I8    ] = DibC::rgb16_555_native_from_i8;

  m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_PRGB32] = DibC::rgb16_555_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_ARGB32] = DibC::rgb16_555_swapped_from_argb32;
  m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_XRGB32] = DibC::rgb16_555_swapped_from_xrgb32;
  m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_A8    ] = DibC::zzz16_from_null;
  m->dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][PIXEL_FORMAT_I8    ] = DibC::rgb16_555_swapped_from_i8;

  m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_PRGB32] = DibC::grey8_from_xrgb32;
  m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_ARGB32] = DibC::grey8_from_argb32;
  m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_XRGB32] = DibC::grey8_from_xrgb32;
  m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_A8    ] = DibC::z8_from_null;
  m->dib.convert[DIB_FORMAT_GREY8            ][PIXEL_FORMAT_I8    ] = DibC::grey8_from_i8;

  // [Dib - Dither]

  m->dib.i8rgb232_from_xrgb32_dither = DibC::i8rgb232_from_xrgb32_dither;
  m->dib.i8rgb222_from_xrgb32_dither = DibC::i8rgb222_from_xrgb32_dither;
  m->dib.i8rgb111_from_xrgb32_dither = DibC::i8rgb111_from_xrgb32_dither;

  m->dib.rgb16_555_native_from_xrgb32_dither = DibC::rgb16_555_native_from_xrgb32_dither;
  m->dib.rgb16_565_native_from_xrgb32_dither = DibC::rgb16_565_native_from_xrgb32_dither;

  m->dib.rgb16_555_swapped_from_xrgb32_dither = DibC::rgb16_555_swapped_from_xrgb32_dither;
  m->dib.rgb16_565_swapped_from_xrgb32_dither = DibC::rgb16_565_swapped_from_xrgb32_dither;

  // [Gradient - Interpolation]
#if defined(FOG_RASTERUTIL_INIT_C)
  m->interpolate.gradient[PIXEL_FORMAT_PRGB32] = InterpolateC::gradient_prgb32;
  m->interpolate.gradient[PIXEL_FORMAT_ARGB32] = InterpolateC::gradient_argb32;
  m->interpolate.gradient[PIXEL_FORMAT_XRGB32] = InterpolateC::gradient_xrgb32;
  m->interpolate.gradient[PIXEL_FORMAT_A8] = InterpolateC::gradient_a8;
#endif // FOG_RASTERUTIL_INIT_C

  // [Pattern - Solid]

  m->pattern.solid_init = PatternC::solid_init;
  m->pattern.solid_fetch = PatternC::solid_fetch;

  // [Pattern - Texture]

  m->pattern.texture_init = PatternC::texture_init;
  m->pattern.texture_init_blit = PatternC::texture_init_blit;
  m->pattern.texture_init_scale = ScaleC::texture_init_scale;

#if defined(FOG_RASTERUTIL_INIT_C)
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_PRGB32][SPREAD_NONE] = PatternC::texture_fetch_exact_none_32<PF_PRGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_ARGB32][SPREAD_NONE] = PatternC::texture_fetch_exact_none_32<PF_ARGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_XRGB32][SPREAD_NONE] = PatternC::texture_fetch_exact_none_32<PF_XRGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_I8    ][SPREAD_NONE] = PatternC::texture_fetch_exact_none_32<PF_I8>;

  m->pattern.texture_fetch_exact[PIXEL_FORMAT_PRGB32][SPREAD_PAD] = PatternC::texture_fetch_exact_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_ARGB32][SPREAD_PAD] = PatternC::texture_fetch_exact_pad_32<PF_ARGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_XRGB32][SPREAD_PAD] = PatternC::texture_fetch_exact_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_I8    ][SPREAD_PAD] = PatternC::texture_fetch_exact_pad_32<PF_I8>;

  m->pattern.texture_fetch_exact[PIXEL_FORMAT_PRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_exact_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_ARGB32][SPREAD_REPEAT] = PatternC::texture_fetch_exact_repeat_32<PF_ARGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_XRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_exact_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_I8    ][SPREAD_REPEAT] = PatternC::texture_fetch_exact_repeat_32<PF_I8>;

  m->pattern.texture_fetch_exact[PIXEL_FORMAT_PRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_exact_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_ARGB32][SPREAD_REFLECT] = PatternC::texture_fetch_exact_reflect_32<PF_ARGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_XRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_exact_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_exact[PIXEL_FORMAT_I8    ][SPREAD_REFLECT] = PatternC::texture_fetch_exact_reflect_32<PF_I8>;
#endif // FOG_RASTERUTIL_INIT_C

  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_PRGB32][SPREAD_NONE] = PatternC::texture_fetch_subx0_none_32<PF_PRGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_ARGB32][SPREAD_NONE] = PatternC::texture_fetch_subx0_none_32<PF_ARGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_XRGB32][SPREAD_NONE] = PatternC::texture_fetch_subx0_none_32<PF_XRGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_I8    ][SPREAD_NONE] = PatternC::texture_fetch_subx0_none_32<PF_I8>;

  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_PRGB32][SPREAD_PAD] = PatternC::texture_fetch_subx0_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_ARGB32][SPREAD_PAD] = PatternC::texture_fetch_subx0_pad_32<PF_ARGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_XRGB32][SPREAD_PAD] = PatternC::texture_fetch_subx0_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_I8    ][SPREAD_PAD] = PatternC::texture_fetch_subx0_pad_32<PF_I8>;

  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_PRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_subx0_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_ARGB32][SPREAD_REPEAT] = PatternC::texture_fetch_subx0_repeat_32<PF_ARGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_XRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_subx0_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_I8    ][SPREAD_REPEAT] = PatternC::texture_fetch_subx0_repeat_32<PF_I8>;

  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_PRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_subx0_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_ARGB32][SPREAD_REFLECT] = PatternC::texture_fetch_subx0_reflect_32<PF_ARGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_XRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_subx0_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_subx0[PIXEL_FORMAT_I8    ][SPREAD_REFLECT] = PatternC::texture_fetch_subx0_reflect_32<PF_I8>;

  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_PRGB32][SPREAD_NONE] = PatternC::texture_fetch_sub0y_none_32<PF_PRGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_ARGB32][SPREAD_NONE] = PatternC::texture_fetch_sub0y_none_32<PF_ARGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_XRGB32][SPREAD_NONE] = PatternC::texture_fetch_sub0y_none_32<PF_XRGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_I8    ][SPREAD_NONE] = PatternC::texture_fetch_sub0y_none_32<PF_I8>;

  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_PRGB32][SPREAD_PAD] = PatternC::texture_fetch_sub0y_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_ARGB32][SPREAD_PAD] = PatternC::texture_fetch_sub0y_pad_32<PF_ARGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_XRGB32][SPREAD_PAD] = PatternC::texture_fetch_sub0y_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_I8    ][SPREAD_PAD] = PatternC::texture_fetch_sub0y_pad_32<PF_I8>;

  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_PRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_sub0y_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_ARGB32][SPREAD_REPEAT] = PatternC::texture_fetch_sub0y_repeat_32<PF_ARGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_XRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_sub0y_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_I8    ][SPREAD_REPEAT] = PatternC::texture_fetch_sub0y_repeat_32<PF_I8>;

  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_PRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_sub0y_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_ARGB32][SPREAD_REFLECT] = PatternC::texture_fetch_sub0y_reflect_32<PF_ARGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_XRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_sub0y_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_sub0y[PIXEL_FORMAT_I8    ][SPREAD_REFLECT] = PatternC::texture_fetch_sub0y_reflect_32<PF_I8>;

  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_PRGB32][SPREAD_NONE] = PatternC::texture_fetch_subxy_none_32<PF_PRGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_ARGB32][SPREAD_NONE] = PatternC::texture_fetch_subxy_none_32<PF_ARGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_XRGB32][SPREAD_NONE] = PatternC::texture_fetch_subxy_none_32<PF_XRGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_I8    ][SPREAD_NONE] = PatternC::texture_fetch_subxy_none_32<PF_I8>;

  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_PRGB32][SPREAD_PAD] = PatternC::texture_fetch_subxy_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_ARGB32][SPREAD_PAD] = PatternC::texture_fetch_subxy_pad_32<PF_ARGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_XRGB32][SPREAD_PAD] = PatternC::texture_fetch_subxy_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_I8    ][SPREAD_PAD] = PatternC::texture_fetch_subxy_pad_32<PF_I8>;

  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_PRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_subxy_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_ARGB32][SPREAD_REPEAT] = PatternC::texture_fetch_subxy_repeat_32<PF_ARGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_XRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_subxy_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_I8    ][SPREAD_REPEAT] = PatternC::texture_fetch_subxy_repeat_32<PF_I8>;

  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_PRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_subxy_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_ARGB32][SPREAD_REFLECT] = PatternC::texture_fetch_subxy_reflect_32<PF_ARGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_XRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_subxy_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_subxy[PIXEL_FORMAT_I8    ][SPREAD_REFLECT] = PatternC::texture_fetch_subxy_reflect_32<PF_I8>;

  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_PRGB32][SPREAD_NONE] = PatternC::texture_fetch_transform_nearest_none_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_ARGB32][SPREAD_NONE] = PatternC::texture_fetch_transform_nearest_none_32<PF_ARGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_XRGB32][SPREAD_NONE] = PatternC::texture_fetch_transform_nearest_none_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_I8    ][SPREAD_NONE] = PatternC::texture_fetch_transform_nearest_none_32<PF_I8>;

  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_PRGB32][SPREAD_PAD] = PatternC::texture_fetch_transform_nearest_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_ARGB32][SPREAD_PAD] = PatternC::texture_fetch_transform_nearest_pad_32<PF_ARGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_XRGB32][SPREAD_PAD] = PatternC::texture_fetch_transform_nearest_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_I8    ][SPREAD_PAD] = PatternC::texture_fetch_transform_nearest_pad_32<PF_I8>;

  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_PRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_transform_nearest_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_ARGB32][SPREAD_REPEAT] = PatternC::texture_fetch_transform_nearest_repeat_32<PF_ARGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_XRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_transform_nearest_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_A8    ][SPREAD_REPEAT] = PatternC::texture_fetch_transform_nearest_repeat_a8;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_I8    ][SPREAD_REPEAT] = PatternC::texture_fetch_transform_nearest_repeat_32<PF_I8>;

  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_PRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_transform_nearest_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_ARGB32][SPREAD_REFLECT] = PatternC::texture_fetch_transform_nearest_reflect_32<PF_ARGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_XRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_transform_nearest_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_nearest[PIXEL_FORMAT_I8    ][SPREAD_REFLECT] = PatternC::texture_fetch_transform_nearest_reflect_32<PF_I8>;

  // TODO: Implement all in SSE2
#if defined(FOG_RASTERUTIL_INIT_C)
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_PRGB32][SPREAD_NONE] = PatternC::texture_fetch_transform_bilinear_none_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_ARGB32][SPREAD_NONE] = PatternC::texture_fetch_transform_bilinear_none_32<PF_ARGB32>;
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_XRGB32][SPREAD_NONE] = PatternC::texture_fetch_transform_bilinear_none_32<PF_XRGB32>;
#endif // FOG_RASTERUTIL_INIT_C
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_I8    ][SPREAD_NONE] = PatternC::texture_fetch_transform_bilinear_none_32<PF_I8>;

#if defined(FOG_RASTERUTIL_INIT_C)
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_PRGB32][SPREAD_PAD] = PatternC::texture_fetch_transform_bilinear_pad_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_ARGB32][SPREAD_PAD] = PatternC::texture_fetch_transform_bilinear_pad_32<PF_ARGB32>;
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_XRGB32][SPREAD_PAD] = PatternC::texture_fetch_transform_bilinear_pad_32<PF_PRGB32>;
#endif // FOG_RASTERUTIL_INIT_C
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_I8    ][SPREAD_PAD] = PatternC::texture_fetch_transform_bilinear_pad_32<PF_I8>;

#if defined(FOG_RASTERUTIL_INIT_C)
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_PRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_transform_bilinear_repeat_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_ARGB32][SPREAD_REPEAT] = PatternC::texture_fetch_transform_bilinear_repeat_32<PF_ARGB32>;
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_XRGB32][SPREAD_REPEAT] = PatternC::texture_fetch_transform_bilinear_repeat_32<PF_PRGB32>;
#endif // FOG_RASTERUTIL_INIT_C
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_A8    ][SPREAD_REPEAT] = PatternC::texture_fetch_transform_bilinear_repeat_a8;
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_I8    ][SPREAD_REPEAT] = PatternC::texture_fetch_transform_bilinear_repeat_32<PF_I8>;

#if defined(FOG_RASTERUTIL_INIT_C)
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_PRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_transform_bilinear_reflect_32<PF_PRGB32>;
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_ARGB32][SPREAD_REFLECT] = PatternC::texture_fetch_transform_bilinear_reflect_32<PF_ARGB32>;
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_XRGB32][SPREAD_REFLECT] = PatternC::texture_fetch_transform_bilinear_reflect_32<PF_PRGB32>;
#endif // FOG_RASTERUTIL_INIT_C
  m->pattern.texture_fetch_transform_bilinear[PIXEL_FORMAT_I8    ][SPREAD_REFLECT] = PatternC::texture_fetch_transform_bilinear_reflect_32<PF_I8>;

  m->pattern.texture_fetch_scale_nearest[PIXEL_FORMAT_PRGB32] = ScaleC::texture_fetch_scale_argb32_nn;
  m->pattern.texture_fetch_scale_nearest[PIXEL_FORMAT_ARGB32] = ScaleC::texture_fetch_scale_argb32_nn;
  m->pattern.texture_fetch_scale_nearest[PIXEL_FORMAT_XRGB32] = ScaleC::texture_fetch_scale_argb32_nn;

  m->pattern.texture_fetch_scale_bilinear[PIXEL_FORMAT_PRGB32] = ScaleC::texture_fetch_scale_argb32_aa;
  m->pattern.texture_fetch_scale_bilinear[PIXEL_FORMAT_ARGB32] = ScaleC::texture_fetch_scale_argb32_aa;
  m->pattern.texture_fetch_scale_bilinear[PIXEL_FORMAT_XRGB32] = ScaleC::texture_fetch_scale_argb32_aa;

  // [Pattern - Linear Gradient]

  m->pattern.linear_gradient_init = PatternC::linear_gradient_init;

  m->pattern.linear_gradient_fetch_exact[SPREAD_NONE] = PatternC::linear_gradient_fetch_exact_pad;
  m->pattern.linear_gradient_fetch_exact[SPREAD_PAD] = PatternC::linear_gradient_fetch_exact_pad;
  m->pattern.linear_gradient_fetch_exact[SPREAD_REPEAT] = PatternC::linear_gradient_fetch_exact_repeat;
  m->pattern.linear_gradient_fetch_exact[SPREAD_REFLECT] = PatternC::linear_gradient_fetch_exact_repeat;

#if defined(FOG_RASTERUTIL_INIT_C)
  m->pattern.linear_gradient_fetch_subxy[SPREAD_NONE] = PatternC::linear_gradient_fetch_subxy_pad;
  m->pattern.linear_gradient_fetch_subxy[SPREAD_PAD] = PatternC::linear_gradient_fetch_subxy_pad;
  m->pattern.linear_gradient_fetch_subxy[SPREAD_REPEAT] = PatternC::linear_gradient_fetch_subxy_repeat;
  m->pattern.linear_gradient_fetch_subxy[SPREAD_REFLECT] = PatternC::linear_gradient_fetch_subxy_repeat;
#endif // FOG_RASTERUTIL_INIT_C

  // [Pattern - Radial Gradient]

  m->pattern.radial_gradient_init = PatternC::radial_gradient_init;

#if defined(FOG_RASTERUTIL_INIT_C)
  m->pattern.radial_gradient_fetch[SPREAD_NONE] = PatternC::radial_gradient_fetch_pad;
  m->pattern.radial_gradient_fetch[SPREAD_PAD] = PatternC::radial_gradient_fetch_pad;
#endif // FOG_RASTERUTIL_INIT_C
  m->pattern.radial_gradient_fetch[SPREAD_REPEAT] = PatternC::radial_gradient_fetch_repeat;
  m->pattern.radial_gradient_fetch[SPREAD_REFLECT] = PatternC::radial_gradient_fetch_repeat;

  // [Pattern - Conical Gradient]

  m->pattern.conical_gradient_init = PatternC::conical_gradient_init;

  m->pattern.conical_gradient_fetch = PatternC::conical_gradient_fetch;

  // [Filter - ColorLut]

  m->filter.color_lut[PIXEL_FORMAT_PRGB32] = FilterC::color_lut_prgb32;
  m->filter.color_lut[PIXEL_FORMAT_ARGB32] = FilterC::color_lut_argb32;
  m->filter.color_lut[PIXEL_FORMAT_XRGB32] = FilterC::color_lut_xrgb32;
  m->filter.color_lut[PIXEL_FORMAT_A8] = FilterC::color_lut_a8;

  // [Filter - ColorMatrix]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->filter.color_matrix[PIXEL_FORMAT_PRGB32] = FilterC::color_matrix_prgb32;
  m->filter.color_matrix[PIXEL_FORMAT_ARGB32] = FilterC::color_matrix_argb32;
  m->filter.color_matrix[PIXEL_FORMAT_XRGB32] = FilterC::color_matrix_xrgb32;
  m->filter.color_matrix[PIXEL_FORMAT_A8] = FilterC::color_matrix_a8;
#endif // FOG_RASTERUTIL_INIT_C

  // [Filters - CopyArea]

  m->filter.copy_area[PIXEL_FORMAT_PRGB32] = FilterC::copy_area_32;
  m->filter.copy_area[PIXEL_FORMAT_ARGB32] = FilterC::copy_area_32;
  m->filter.copy_area[PIXEL_FORMAT_XRGB32] = FilterC::copy_area_32;
  m->filter.copy_area[PIXEL_FORMAT_A8] = FilterC::copy_area_8;
  m->filter.copy_area[PIXEL_FORMAT_I8] = FilterC::copy_area_8;

  // [Filters - BoxBlur]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->filter.box_blur_h[PIXEL_FORMAT_ARGB32] = FilterC::box_blur_h_argb32;
  m->filter.box_blur_v[PIXEL_FORMAT_ARGB32] = FilterC::box_blur_v_argb32;
  m->filter.box_blur_h[PIXEL_FORMAT_XRGB32] = FilterC::box_blur_h_xrgb32;
  m->filter.box_blur_v[PIXEL_FORMAT_XRGB32] = FilterC::box_blur_v_xrgb32;
#endif // FOG_RASTERUTIL_INIT_C
  m->filter.box_blur_h[PIXEL_FORMAT_A8] = FilterC::box_blur_h_a8;
  m->filter.box_blur_v[PIXEL_FORMAT_A8] = FilterC::box_blur_v_a8;

  // [Filters - LinearBlur]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->filter.linear_blur_h[PIXEL_FORMAT_ARGB32] = FilterC::linear_blur_h_argb32;
  m->filter.linear_blur_v[PIXEL_FORMAT_ARGB32] = FilterC::linear_blur_v_argb32;
  m->filter.linear_blur_h[PIXEL_FORMAT_XRGB32] = FilterC::linear_blur_h_argb32;
  m->filter.linear_blur_v[PIXEL_FORMAT_XRGB32] = FilterC::linear_blur_v_argb32;
#endif // FOG_RASTERUTIL_INIT_C
  m->filter.linear_blur_h[PIXEL_FORMAT_A8] = FilterC::linear_blur_h_a8;
  m->filter.linear_blur_v[PIXEL_FORMAT_A8] = FilterC::linear_blur_v_a8;

  // [Filters - SymmetricConvolveFloat]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->filter.symmetric_convolve_float_h[PIXEL_FORMAT_ARGB32] = FilterC::symmetric_convolve_float_h_argb32;
  m->filter.symmetric_convolve_float_v[PIXEL_FORMAT_ARGB32] = FilterC::symmetric_convolve_float_v_argb32;
  m->filter.symmetric_convolve_float_h[PIXEL_FORMAT_XRGB32] = FilterC::symmetric_convolve_float_h_argb32;
  m->filter.symmetric_convolve_float_v[PIXEL_FORMAT_XRGB32] = FilterC::symmetric_convolve_float_v_argb32;
#endif // FOG_RASTERUTIL_INIT_C

  m->filter.symmetric_convolve_float_h[PIXEL_FORMAT_A8] = FilterC::symmetric_convolve_float_h_a8;
  m->filter.symmetric_convolve_float_v[PIXEL_FORMAT_A8] = FilterC::symmetric_convolve_float_v_a8;

  // [Composite - NOPS]

  fog_rasterengine_set_nops(&m->composite[OPERATOR_DST][PIXEL_FORMAT_ARGB32]);
  fog_rasterengine_set_nops(&m->composite[OPERATOR_DST][PIXEL_FORMAT_PRGB32]);
  fog_rasterengine_set_nops(&m->composite[OPERATOR_DST][PIXEL_FORMAT_XRGB32]);
  fog_rasterengine_set_nops(&m->composite[OPERATOR_DST][PIXEL_FORMAT_A8]);
  fog_rasterengine_set_nops(&m->composite[OPERATOR_DST][PIXEL_FORMAT_I8]);

  fog_rasterengine_set_nops(&m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_XRGB32]);

  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeNopC::cspan;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeNopC::cspan_a8;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeNopC::cspan_a8_const;

  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeNopC::cspan;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeNopC::cspan_a8;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeNopC::cspan_a8_const;

  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeNopC::cspan;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeNopC::cspan_a8;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeNopC::cspan_a8_const;

  fog_rasterengine_set_nops(&m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_A8]);

  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_A8] = (VSpanFn)CompositeNopC::cspan;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_A8] = (VSpanMskFn)CompositeNopC::cspan_a8;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = (VSpanMskConstFn)CompositeNopC::cspan_a8_const;


  // [Composite - Src]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].cspan = CompositeSrcC::prgb32_cspan;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeSrcC::prgb32_cspan_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeSrcC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeSrcC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = DibC::memcpy32;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = DibC::prgb32_from_argb32;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = DibC::frgb32_from_xrgb32;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_I8] = DibC::prgb32_from_i8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_A8] = DibC::azzz32_from_a8;

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSrcC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSrcC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcC::prgb32_vspan_xrgb32_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_A8] = CompositeSrcC::prgb32_vspan_a8_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_I8] = CompositeSrcC::prgb32_vspan_i8_a8;

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSrcC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSrcC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcC::prgb32_vspan_xrgb32_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = CompositeSrcC::prgb32_vspan_a8_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_I8] = CompositeSrcC::prgb32_vspan_i8_a8_const;

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_ARGB32].vspan[PIXEL_FORMAT_PRGB32] = DibC::argb32_from_prgb32;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_ARGB32].vspan[PIXEL_FORMAT_ARGB32] = DibC::memcpy32;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_ARGB32].vspan[PIXEL_FORMAT_XRGB32] = DibC::frgb32_from_xrgb32;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_ARGB32].vspan[PIXEL_FORMAT_I8] = DibC::argb32_from_i8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_ARGB32].vspan[PIXEL_FORMAT_A8] = DibC::azzz32_from_a8;

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].cspan = CompositeSrcC::xrgb32_cspan;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeSrcC::xrgb32_cspan_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeSrcC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeSrcC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = DibC::frgb32_from_xrgb32;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = DibC::frgb32_from_argb32;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = DibC::memcpy32;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_I8] = DibC::frgb32_from_i8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_A8] = DibC::fzzz32_from_null;

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSrcC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_A8] = (VSpanMskFn)CompositeClearC::xrgb32_cspan_a8;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_I8] = CompositeSrcC::xrgb32_vspan_i8_a8;

  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSrcC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = (VSpanMskConstFn)CompositeClearC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_I8] = CompositeSrcC::xrgb32_vspan_i8_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - SrcOver]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].cspan = CompositeSrcOverC::prgb32_cspan;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeSrcOverC::prgb32_cspan_a8;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeSrcOverC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeSrcOverC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSrcOverC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeSrcOverC::prgb32_vspan_argb32;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = DibC::frgb32_from_xrgb32;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_A8] = CompositeDstOutC::prgb32_vspan_a8;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_I8] = CompositeSrcOverC::prgb32_vspan_i8;

  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSrcOverC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSrcOverC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcC::prgb32_vspan_xrgb32_a8;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_A8] = CompositeDstOutC::prgb32_vspan_a8_a8;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_I8] = CompositeSrcC::prgb32_vspan_i8_a8;

  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSrcOverC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSrcOverC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcC::prgb32_vspan_xrgb32_a8_const;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = CompositeDstOutC::prgb32_vspan_a8_a8_const;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_I8] = CompositeSrcC::prgb32_vspan_i8_a8_const;

  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].cspan = CompositeSrcOverC::xrgb32_cspan;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeSrcOverC::xrgb32_cspan_a8;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeSrcOverC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeSrcOverC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = DibC::memcpy32;

  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - DstOver]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].cspan = CompositeDstOverC::prgb32_cspan;
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeDstOverC::prgb32_cspan_a8;
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeDstOverC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeDstOverC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDstOverC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDstOverC::prgb32_vspan_argb32;
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDstOverC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDstOverC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDstOverC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDstOverC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_DST_OVER][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // PIXEL_FORMAT_XRGB32 - NOP (already set by RasterEngine_Init_C.cpp).

  // [Composite - SrcIn]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].cspan = CompositeSrcInC::prgb32_cspan;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeSrcInC::prgb32_cspan_a8;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeSrcInC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeSrcInC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSrcInC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeSrcInC::prgb32_vspan_argb32;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSrcInC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSrcInC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSrcInC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSrcInC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].cspan = CompositeSrcC::xrgb32_cspan;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeSrcC::xrgb32_cspan_a8;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeSrcC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeSrcC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = DibC::frgb32_from_argb32;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = DibC::frgb32_from_xrgb32;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = DibC::memcpy32;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_A8] = (VSpanFn)CompositeClearC::xrgb32_cspan;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_I8] = DibC::frgb32_from_i8;

  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSrcC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_A8] = (VSpanMskFn)CompositeClearC::xrgb32_cspan_a8;
//  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_I8] = CompositeSrcC::xrgb32_vspan_i8_a8;

  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSrcC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8_const;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8_const;
  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = (VSpanMskConstFn)CompositeClearC::xrgb32_cspan_a8_const;
//  m->composite[OPERATOR_SRC_IN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_I8] = CompositeSrcC::xrgb32_vspan_i8_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - DstIn]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].cspan = CompositeDstInC::prgb32_cspan;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeDstInC::prgb32_cspan_a8;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeDstInC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeDstInC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32;
//m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = NOP;

  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8;
//m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = NOP;

  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8_const;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8_const;
//m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = NOP;

  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].cspan = CompositeDstInC::prgb32_cspan;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeDstInC::prgb32_cspan_a8;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeDstInC::prgb32_cspan_a8_const;
//  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeDstInC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32;
//m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = NOP;

  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8;
//m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = NOP;

  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8_const;
  m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8_const;
//m->composite[OPERATOR_DST_IN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = NOP;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - SrcOut]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].cspan = CompositeSrcOutC::prgb32_cspan;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeSrcOutC::prgb32_cspan_a8;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeSrcOutC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeSrcOutC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSrcOutC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeSrcOutC::prgb32_vspan_argb32;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSrcOutC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSrcOutC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSrcOutC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSrcOutC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].cspan = CompositeClearC::prgb32_cspan;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeClearC::prgb32_cspan_a8;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeClearC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeClearC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = (VSpanFn)CompositeClearC::prgb32_cspan;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = (VSpanFn)CompositeClearC::prgb32_cspan;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeClearC::prgb32_cspan;

  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;

  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_OUT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - DstOut]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].cspan = CompositeDstOutC::prgb32_cspan;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeDstOutC::prgb32_cspan_a8;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeDstOutC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeDstOutC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeClearC::prgb32_cspan;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_A8] = CompositeDstOutC::prgb32_vspan_a8;

  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_A8] = CompositeDstOutC::prgb32_vspan_a8_a8;

  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8_const;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8_const;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = CompositeDstOutC::prgb32_vspan_a8_a8_const;

  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].cspan = CompositeDstOutC::prgb32_cspan;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeDstOutC::prgb32_cspan_a8;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeDstOutC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeDstOutC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeClearC::prgb32_cspan;

  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;

  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8_const;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8_const;
  m->composite[OPERATOR_DST_OUT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - SrcAtop]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].cspan = CompositeSrcAtopC::prgb32_cspan;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeSrcAtopC::prgb32_cspan_a8;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeSrcAtopC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeSrcAtopC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSrcAtopC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeSrcAtopC::prgb32_vspan_argb32;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSrcAtopC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSrcAtopC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSrcAtopC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSrcAtopC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].cspan = CompositeSrcOverC::xrgb32_cspan;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeSrcOverC::xrgb32_cspan_a8;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeSrcOverC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeSrcOverC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = DibC::memcpy32;

  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SRC_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vspan_xrgb32_a8_const;

  // PIXEL_FORMAT_A8     - NOP (already set by RasterEngine_Init_C.cpp).
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - DstAtop]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].cspan = CompositeDstAtopC::prgb32_cspan;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeDstAtopC::prgb32_cspan_a8;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeDstAtopC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeDstAtopC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDstAtopC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDstAtopC::prgb32_vspan_argb32;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDstAtopC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDstAtopC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDstAtopC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDstAtopC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].cspan = CompositeDstInC::prgb32_cspan;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeDstInC::prgb32_cspan_a8;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeDstInC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeDstInC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32;
//m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = NOP;

  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8;
//m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = NOP;

  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8_const;
  m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDstInC::prgb32_vspan_prgb32_or_argb32_a8_const;
//m->composite[OPERATOR_DST_ATOP][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = NOP;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Xor]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].cspan = CompositeXorC::prgb32_cspan;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeXorC::prgb32_cspan_a8;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeXorC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeXorC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeXorC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeXorC::prgb32_vspan_argb32;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeXorC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeXorC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeXorC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeXorC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].cspan = CompositeDstOutC::prgb32_cspan;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeDstOutC::prgb32_cspan_a8;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeDstOutC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeDstOutC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeClearC::xrgb32_cspan;

  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeClearC::xrgb32_cspan_a8;

  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8_const;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vspan_prgb32_or_argb32_a8_const;
  m->composite[OPERATOR_XOR][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeClearC::xrgb32_cspan_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Clear]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].cspan = CompositeClearC::prgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeClearC::prgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeClearC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeClearC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = (VSpanFn)CompositeClearC::prgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = (VSpanFn)CompositeClearC::prgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeClearC::prgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_A8] = (VSpanFn)CompositeClearC::prgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_I8] = (VSpanFn)CompositeClearC::prgb32_cspan;

  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_A8] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_I8] = (VSpanMskFn)CompositeClearC::prgb32_cspan_a8;

  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_I8] = (VSpanMskConstFn)CompositeClearC::prgb32_cspan_a8_const;

  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].cspan = CompositeClearC::xrgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeClearC::xrgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeClearC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeClearC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = (VSpanFn)CompositeClearC::xrgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = (VSpanFn)CompositeClearC::xrgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = (VSpanFn)CompositeClearC::xrgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_A8] = (VSpanFn)CompositeClearC::xrgb32_cspan;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_I8] = (VSpanFn)CompositeClearC::xrgb32_cspan;

  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = (VSpanMskFn)CompositeClearC::xrgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = (VSpanMskFn)CompositeClearC::xrgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = (VSpanMskFn)CompositeClearC::xrgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_A8] = (VSpanMskFn)CompositeClearC::xrgb32_cspan_a8;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_I8] = (VSpanMskFn)CompositeClearC::xrgb32_cspan_a8;

  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = (VSpanMskConstFn)CompositeClearC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = (VSpanMskConstFn)CompositeClearC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = (VSpanMskConstFn)CompositeClearC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_A8] = (VSpanMskConstFn)CompositeClearC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_CLEAR][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_I8] = (VSpanMskConstFn)CompositeClearC::xrgb32_cspan_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Add]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].cspan = CompositeAddC::prgb32_cspan;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeAddC::prgb32_cspan_a8;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeAddC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeAddC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeAddC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeAddC::prgb32_vspan_argb32;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeAddC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeAddC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeAddC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeAddC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeAddC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeAddC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeAddC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].cspan = CompositeAddC::xrgb32_cspan;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeAddC::xrgb32_cspan_a8;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeAddC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeAddC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeAddC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeAddC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeAddC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeAddC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeAddC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeAddC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeAddC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeAddC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_ADD][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeAddC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Subtract]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].cspan = CompositeSubtractC::prgb32_cspan;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeSubtractC::prgb32_cspan_a8;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeSubtractC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeSubtractC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSubtractC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeSubtractC::prgb32_vspan_argb32;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeSubtractC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSubtractC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSubtractC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSubtractC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSubtractC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSubtractC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSubtractC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].cspan = CompositeSubtractC::xrgb32_cspan;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeSubtractC::xrgb32_cspan_a8;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeSubtractC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeSubtractC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSubtractC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeSubtractC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeSubtractC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeSubtractC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeSubtractC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeSubtractC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeSubtractC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeSubtractC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SUBTRACT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeSubtractC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Multiply]

  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].cspan = CompositeMultiplyC::prgb32_cspan;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeMultiplyC::prgb32_cspan_a8;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeMultiplyC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeMultiplyC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeMultiplyC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeMultiplyC::prgb32_vspan_argb32;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeMultiplyC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeMultiplyC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeMultiplyC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeMultiplyC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeMultiplyC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeMultiplyC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeMultiplyC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].cspan = CompositeMultiplyC::xrgb32_cspan;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeMultiplyC::xrgb32_cspan_a8;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeMultiplyC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeMultiplyC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeMultiplyC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeMultiplyC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeMultiplyC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeMultiplyC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeMultiplyC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeMultiplyC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeMultiplyC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeMultiplyC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_MULTIPLY][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeMultiplyC::xrgb32_vspan_xrgb32_a8_const;

  // [Composite - Screen]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].cspan = CompositeScreenC::prgb32_cspan;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeScreenC::prgb32_cspan_a8;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeScreenC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeScreenC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeScreenC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeScreenC::prgb32_vspan_argb32;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeScreenC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeScreenC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeScreenC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeScreenC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeScreenC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeScreenC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeScreenC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].cspan = CompositeScreenC::xrgb32_cspan;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeScreenC::xrgb32_cspan_a8;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeScreenC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeScreenC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeScreenC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeScreenC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeScreenC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeScreenC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeScreenC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeScreenC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeScreenC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeScreenC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_SCREEN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeScreenC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Darken]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].cspan = CompositeDarkenC::prgb32_cspan;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeDarkenC::prgb32_cspan_a8;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeDarkenC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeDarkenC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDarkenC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDarkenC::prgb32_vspan_argb32;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeDarkenC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDarkenC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDarkenC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeDarkenC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDarkenC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDarkenC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeDarkenC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].cspan = CompositeDarkenC::xrgb32_cspan;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeDarkenC::xrgb32_cspan_a8;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeDarkenC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeDarkenC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDarkenC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDarkenC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeDarkenC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDarkenC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDarkenC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeDarkenC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDarkenC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDarkenC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_DARKEN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeDarkenC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Lighten]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].cspan = CompositeLightenC::prgb32_cspan;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeLightenC::prgb32_cspan_a8;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeLightenC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeLightenC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeLightenC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeLightenC::prgb32_vspan_argb32;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeLightenC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeLightenC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeLightenC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeLightenC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeLightenC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeLightenC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeLightenC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].cspan = CompositeLightenC::xrgb32_cspan;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeLightenC::xrgb32_cspan_a8;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeLightenC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeLightenC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeLightenC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeLightenC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeLightenC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeLightenC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeLightenC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeLightenC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeLightenC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeLightenC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_LIGHTEN][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeLightenC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Difference]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].cspan = CompositeDifferenceC::prgb32_cspan;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeDifferenceC::prgb32_cspan_a8;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeDifferenceC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeDifferenceC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDifferenceC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDifferenceC::prgb32_vspan_argb32;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeDifferenceC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDifferenceC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDifferenceC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeDifferenceC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDifferenceC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDifferenceC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeDifferenceC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].cspan = CompositeDifferenceC::xrgb32_cspan;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeDifferenceC::xrgb32_cspan_a8;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeDifferenceC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeDifferenceC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeDifferenceC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeDifferenceC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeDifferenceC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeDifferenceC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeDifferenceC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeDifferenceC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeDifferenceC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeDifferenceC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_DIFFERENCE][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeDifferenceC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Exclusion]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].cspan = CompositeExclusionC::prgb32_cspan;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeExclusionC::prgb32_cspan_a8;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeExclusionC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeExclusionC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeExclusionC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeExclusionC::prgb32_vspan_argb32;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeExclusionC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeExclusionC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeExclusionC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeExclusionC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeExclusionC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeExclusionC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeExclusionC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].cspan = CompositeExclusionC::xrgb32_cspan;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeExclusionC::xrgb32_cspan_a8;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeExclusionC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeExclusionC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeExclusionC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeExclusionC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeExclusionC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeExclusionC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeExclusionC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeExclusionC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeExclusionC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeExclusionC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_EXCLUSION][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeExclusionC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - Invert]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].cspan = CompositeInvertC::prgb32_cspan;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeInvertC::prgb32_cspan_a8;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeInvertC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeInvertC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeInvertC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeInvertC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeInvertC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeInvertC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeInvertC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeInvertC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeInvertC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeInvertC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeInvertC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].cspan = CompositeInvertC::xrgb32_cspan;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeInvertC::xrgb32_cspan_a8;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeInvertC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeInvertC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeInvertC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeInvertC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeInvertC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeInvertC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeInvertC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeInvertC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeInvertC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeInvertC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_INVERT][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeInvertC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C

  // [Composite - InvertRgb]

#if defined(FOG_RASTERUTIL_INIT_C)
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].cspan = CompositeInvertRgbC::prgb32_cspan;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].cspan_a8 = CompositeInvertRgbC::prgb32_cspan_a8;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].cspan_a8_const = CompositeInvertRgbC::prgb32_cspan_a8_const;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].cspan_a8_scanline = CompositeInvertRgbC::prgb32_cspan_a8_scanline;

  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeInvertRgbC::prgb32_vspan_prgb32;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeInvertRgbC::prgb32_vspan_argb32;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeInvertRgbC::prgb32_vspan_xrgb32;

  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeInvertRgbC::prgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeInvertRgbC::prgb32_vspan_argb32_a8;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeInvertRgbC::prgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeInvertRgbC::prgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeInvertRgbC::prgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_PRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeInvertRgbC::prgb32_vspan_xrgb32_a8_const;

  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].cspan = CompositeInvertRgbC::xrgb32_cspan;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].cspan_a8 = CompositeInvertRgbC::xrgb32_cspan_a8;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].cspan_a8_const = CompositeInvertRgbC::xrgb32_cspan_a8_const;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].cspan_a8_scanline = CompositeInvertRgbC::xrgb32_cspan_a8_scanline;

  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeInvertRgbC::xrgb32_vspan_prgb32;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_ARGB32] = CompositeInvertRgbC::xrgb32_vspan_argb32;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_XRGB32] = CompositeInvertRgbC::xrgb32_vspan_xrgb32;

  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_PRGB32] = CompositeInvertRgbC::xrgb32_vspan_prgb32_a8;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_ARGB32] = CompositeInvertRgbC::xrgb32_vspan_argb32_a8;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].vspan_a8[PIXEL_FORMAT_XRGB32] = CompositeInvertRgbC::xrgb32_vspan_xrgb32_a8;

  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_PRGB32] = CompositeInvertRgbC::xrgb32_vspan_prgb32_a8_const;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_ARGB32] = CompositeInvertRgbC::xrgb32_vspan_argb32_a8_const;
  m->composite[OPERATOR_INVERT_RGB][PIXEL_FORMAT_XRGB32].vspan_a8_const[PIXEL_FORMAT_XRGB32] = CompositeInvertRgbC::xrgb32_vspan_xrgb32_a8_const;
#endif // FOG_RASTERUTIL_INIT_C
}
