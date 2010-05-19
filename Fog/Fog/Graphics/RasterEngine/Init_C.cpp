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
#include <Fog/Graphics/ByteUtil_p.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/ColorAnalyzer_p.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/ColorMatrix.h>
#include <Fog/Graphics/ColorUtil.h>
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
#include <Fog/Graphics/RasterUtil_p.h>
#include <Fog/Graphics/Span_p.h>

// [Fog::RasterEngine::C]
#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Core_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Src_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_SrcOver_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_DstOver_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_SrcIn_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_DstIn_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_SrcOut_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_DstOut_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_SrcAtop_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_DstAtop_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Xor_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Clear_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Add_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Subtract_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Multiply_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Screen_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Darken_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Lighten_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Difference_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Exclusion_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_Invert_p.h>
#include <Fog/Graphics/RasterEngine/Composite_C_InvertRgb_p.h>
#include <Fog/Graphics/RasterEngine/Mask_C_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_C_Core_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_C_Solid_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_C_Interpolate_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_C_Gradient_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_C_Scale_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_C_Texture_p.h>
#include <Fog/Graphics/RasterEngine/Filters_C_p.h>

// Decide whether pure C implementation is needed. In case that we are compiling
// for SSE2-only machine, we can omit C/MMX implementation that will result in
// a smaller binary size of the library.
#if !defined(FOG_HARDCODE_SSE2)
# define FOG_RASTERUTIL_INIT_C
#endif // FOG_HARDCODE_SSE2

// ============================================================================
// [Library Initializers]
// ============================================================================

static void fog_rasterengine_set_nops(Fog::RasterFuncs::CompositeFuncs* ops)
{
  using namespace Fog;
  using namespace Fog::RasterEngine;

  ops->cblit_full[IMAGE_FORMAT_PRGB32] = CompositeNopC::cblit_full;
  ops->cblit_span[IMAGE_FORMAT_PRGB32] = CompositeNopC::cblit_span;
  ops->cblit_full[IMAGE_FORMAT_ARGB32] = CompositeNopC::cblit_full;
  ops->cblit_span[IMAGE_FORMAT_ARGB32] = CompositeNopC::cblit_span;
  ops->cblit_full[IMAGE_FORMAT_XRGB32] = CompositeNopC::cblit_full;
  ops->cblit_span[IMAGE_FORMAT_XRGB32] = CompositeNopC::cblit_span;

  ops->vblit_full[IMAGE_FORMAT_ARGB32] = CompositeNopC::vblit_full;
  ops->vblit_span[IMAGE_FORMAT_ARGB32] = CompositeNopC::vblit_span;
  ops->vblit_full[IMAGE_FORMAT_PRGB32] = CompositeNopC::vblit_full;
  ops->vblit_span[IMAGE_FORMAT_PRGB32] = CompositeNopC::vblit_span;
  ops->vblit_full[IMAGE_FORMAT_XRGB32] = CompositeNopC::vblit_full;
  ops->vblit_span[IMAGE_FORMAT_XRGB32] = CompositeNopC::vblit_span;
  ops->vblit_full[IMAGE_FORMAT_A8    ] = CompositeNopC::vblit_full;
  ops->vblit_span[IMAGE_FORMAT_A8    ] = CompositeNopC::vblit_span;
  ops->vblit_full[IMAGE_FORMAT_I8    ] = CompositeNopC::vblit_full;
  ops->vblit_span[IMAGE_FORMAT_I8    ] = CompositeNopC::vblit_span;
}

FOG_INIT_DECLARE void fog_rasterengine_init_c(void)
{
  using namespace Fog;
  using namespace Fog::RasterEngine;

  RasterFuncs& f = rasterFuncs;

  // --------------------------------------------------------------------------
  // [Dib - ByteSwap]
  // --------------------------------------------------------------------------

  f.dib.bswap16 = DibC::bswap16;
  f.dib.bswap24 = DibC::bswap24;
  f.dib.bswap32 = DibC::bswap32;

  // --------------------------------------------------------------------------
  // [Dib - MemCpy]
  // --------------------------------------------------------------------------

  f.dib.memcpy[IMAGE_FORMAT_PRGB32] = DibC::memcpy32;
  f.dib.memcpy[IMAGE_FORMAT_ARGB32] = DibC::memcpy32;
  f.dib.memcpy[IMAGE_FORMAT_XRGB32] = DibC::memcpy32;
  f.dib.memcpy[IMAGE_FORMAT_A8    ] = DibC::memcpy8;
  f.dib.memcpy[IMAGE_FORMAT_I8    ] = DibC::memcpy8;

  f.dib.memcpy8 = DibC::memcpy8;
  f.dib.memcpy16 = DibC::memcpy16;
  f.dib.memcpy24 = DibC::memcpy24;
  f.dib.memcpy32 = DibC::memcpy32;

  // --------------------------------------------------------------------------
  // [Dib - Rect]
  // --------------------------------------------------------------------------

  f.dib.cblit_rect[IMAGE_FORMAT_PRGB32] = DibC::cblit_rect_32_prgb;
  f.dib.cblit_rect[IMAGE_FORMAT_ARGB32] = DibC::cblit_rect_32_argb;
  f.dib.cblit_rect[IMAGE_FORMAT_XRGB32] = DibC::cblit_rect_32_prgb;
  f.dib.cblit_rect[IMAGE_FORMAT_A8    ] = DibC::cblit_rect_8;
  f.dib.cblit_rect[IMAGE_FORMAT_I8    ] = DibC::cblit_rect_8;

  f.dib.vblit_rect[IMAGE_FORMAT_PRGB32] = DibC::vblit_rect_32;
  f.dib.vblit_rect[IMAGE_FORMAT_ARGB32] = DibC::vblit_rect_32;
  f.dib.vblit_rect[IMAGE_FORMAT_XRGB32] = DibC::vblit_rect_32;
  f.dib.vblit_rect[IMAGE_FORMAT_A8    ] = DibC::vblit_rect_8;
  f.dib.vblit_rect[IMAGE_FORMAT_I8    ] = DibC::vblit_rect_8;

  // --------------------------------------------------------------------------
  // [Dib - Convert]
  // --------------------------------------------------------------------------

  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = DibC::memcpy32;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = DibC::prgb32_from_argb32;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = DibC::frgb32_from_xrgb32;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_A8               ] = DibC::azzz32_from_a8;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_I8               ] = DibC::prgb32_from_i8;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = DibC::bswap32;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = DibC::prgb32_from_argb32_swapped;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = DibC::frgb32_from_xrgb32_swapped;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB24_NATIVE     ] = DibC::frgb32_from_rgb24_native;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = DibC::frgb32_from_rgb24_swapped;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = DibC::frgb32_from_rgb16_565_native;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = DibC::frgb32_from_rgb16_565_swapped;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = DibC::frgb32_from_rgb16_555_native;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = DibC::frgb32_from_rgb16_555_swapped;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_GREY8            ] = DibC::frgb32_from_grey8;

  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_PRGB32_NATIVE    ] = DibC::argb32_from_prgb32;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_ARGB32_NATIVE    ] = DibC::memcpy32;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_XRGB32_NATIVE    ] = DibC::frgb32_from_xrgb32;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_A8               ] = DibC::azzz32_from_a8;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_I8               ] = DibC::argb32_from_i8;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = DibC::argb32_from_prgb32_swapped;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = DibC::bswap32;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = DibC::frgb32_from_xrgb32_swapped;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB24_NATIVE     ] = DibC::frgb32_from_rgb24_native;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB24_SWAPPED    ] = DibC::frgb32_from_rgb24_swapped;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_NATIVE ] = DibC::frgb32_from_rgb16_565_native;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_SWAPPED] = DibC::frgb32_from_rgb16_565_swapped;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_NATIVE ] = DibC::frgb32_from_rgb16_555_native;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_SWAPPED] = DibC::frgb32_from_rgb16_555_swapped;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_GREY8            ] = DibC::frgb32_from_grey8;

  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = DibC::frgb32_from_xrgb32;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = DibC::frgb32_from_argb32;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = DibC::frgb32_from_xrgb32;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_A8               ] = DibC::fzzz32_from_null;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_I8               ] = DibC::frgb32_from_i8;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = DibC::frgb32_from_xrgb32_swapped;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = DibC::frgb32_from_argb32_swapped;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = DibC::bswap32;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB24_NATIVE     ] = DibC::frgb32_from_rgb24_native;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = DibC::frgb32_from_rgb24_swapped;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = DibC::frgb32_from_rgb16_565_native;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = DibC::frgb32_from_rgb16_565_swapped;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = DibC::frgb32_from_rgb16_555_native;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = DibC::frgb32_from_rgb16_555_swapped;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_GREY8            ] = DibC::frgb32_from_grey8;

  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_PRGB32_NATIVE    ] = DibC::a8_from_axxx32;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_ARGB32_NATIVE    ] = DibC::a8_from_axxx32;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_XRGB32_NATIVE    ] = DibC::f8_from_null;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_A8               ] = DibC::memcpy8;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_I8               ] = DibC::a8_from_i8;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_PRGB32_SWAPPED   ] = DibC::a8_from_axxx32_swapped;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_ARGB32_SWAPPED   ] = DibC::a8_from_axxx32_swapped;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_XRGB32_SWAPPED   ] = DibC::f8_from_null;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB24_NATIVE     ] = DibC::f8_from_null;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB24_SWAPPED    ] = DibC::f8_from_null;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_565_NATIVE ] = DibC::f8_from_null;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_565_SWAPPED] = DibC::f8_from_null;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_555_NATIVE ] = DibC::f8_from_null;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_555_SWAPPED] = DibC::f8_from_null;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_GREY8            ] = DibC::f8_from_null;

  f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_PRGB32] = DibC::bswap32;
  f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_ARGB32] = DibC::prgb32_swapped_from_argb32;
  f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_XRGB32] = DibC::frgb32_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_A8    ] = DibC::azzz32_swapped_from_a8;
  f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_I8    ] = DibC::prgb32_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_PRGB32] = DibC::argb32_swapped_from_prgb32;
  f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_ARGB32] = DibC::bswap32;
  f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_XRGB32] = DibC::frgb32_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_A8    ] = DibC::azzz32_swapped_from_a8;
  f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_I8    ] = DibC::argb32_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_PRGB32] = DibC::frgb32_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_ARGB32] = DibC::frgb32_swapped_from_argb32;
  f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_XRGB32] = DibC::frgb32_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_A8    ] = DibC::fzzz32_swapped_from_null;
  f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_I8    ] = DibC::frgb32_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_PRGB32] = DibC::rgb24_native_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_ARGB32] = DibC::rgb24_native_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_XRGB32] = DibC::rgb24_native_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_A8    ] = DibC::zzz24_from_null;
  f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_I8    ] = DibC::rgb24_native_from_i8;

  f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_PRGB32] = DibC::rgb24_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_ARGB32] = DibC::rgb24_swapped_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_XRGB32] = DibC::rgb24_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_A8    ] = DibC::zzz24_from_null;
  f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_I8    ] = DibC::rgb24_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_PRGB32] = DibC::rgb16_565_native_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_ARGB32] = DibC::rgb16_565_native_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_XRGB32] = DibC::rgb16_565_native_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_A8    ] = DibC::zzz16_from_null;
  f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_I8    ] = DibC::rgb16_565_native_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_PRGB32] = DibC::rgb16_565_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_ARGB32] = DibC::rgb16_565_swapped_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_XRGB32] = DibC::rgb16_565_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_A8    ] = DibC::zzz16_from_null;
  f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_I8    ] = DibC::rgb16_565_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_PRGB32] = DibC::rgb16_555_native_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_ARGB32] = DibC::rgb16_555_native_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_XRGB32] = DibC::rgb16_555_native_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_A8    ] = DibC::zzz16_from_null;
  f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_I8    ] = DibC::rgb16_555_native_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_PRGB32] = DibC::rgb16_555_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_ARGB32] = DibC::rgb16_555_swapped_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_XRGB32] = DibC::rgb16_555_swapped_from_xrgb32;
  f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_A8    ] = DibC::zzz16_from_null;
  f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_I8    ] = DibC::rgb16_555_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_PRGB32] = DibC::grey8_from_xrgb32;
  f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_ARGB32] = DibC::grey8_from_argb32;
  f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_XRGB32] = DibC::grey8_from_xrgb32;
  f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_A8    ] = DibC::z8_from_null;
  f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_I8    ] = DibC::grey8_from_i8;

  // --------------------------------------------------------------------------
  // [Dib - Dither]
  // --------------------------------------------------------------------------

  f.dib.i8rgb232_from_xrgb32_dither = DibC::i8rgb232_from_xrgb32_dither;
  f.dib.i8rgb222_from_xrgb32_dither = DibC::i8rgb222_from_xrgb32_dither;
  f.dib.i8rgb111_from_xrgb32_dither = DibC::i8rgb111_from_xrgb32_dither;

  f.dib.rgb16_555_native_from_xrgb32_dither = DibC::rgb16_555_native_from_xrgb32_dither;
  f.dib.rgb16_565_native_from_xrgb32_dither = DibC::rgb16_565_native_from_xrgb32_dither;

  f.dib.rgb16_555_swapped_from_xrgb32_dither = DibC::rgb16_555_swapped_from_xrgb32_dither;
  f.dib.rgb16_565_swapped_from_xrgb32_dither = DibC::rgb16_565_swapped_from_xrgb32_dither;

  // --------------------------------------------------------------------------
  // [Dib - Helpers]
  // --------------------------------------------------------------------------

  f.dib.prgb32_from_argb32_span = DibC::prgb32_from_argb32_span;
  f.dib.argb32_from_prgb32_span = DibC::argb32_from_prgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - Src]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::prgb32_cblit_span;

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = DibC::memcpy32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = DibC::prgb32_from_argb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibC::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::prgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_A8    ] = DibC::azzz32_from_a8;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeSrcC::prgb32_vblit_a8_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_I8    ] = DibC::prgb32_from_i8;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeSrcC::prgb32_vblit_i8_span;

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_PRGB32] = DibC::argb32_from_prgb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_ARGB32] = DibC::memcpy32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibC::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_I8    ] = DibC::argb32_from_i8;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_A8    ] = DibC::azzz32_from_a8;

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = DibC::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = DibC::frgb32_from_argb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibC::memcpy32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_I8    ] = DibC::frgb32_from_i8;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeSrcC::xrgb32_vblit_i8_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Dst]
  // --------------------------------------------------------------------------

  fog_rasterengine_set_nops(&f.composite[OPERATOR_DST][IMAGE_FORMAT_PRGB32]);
  fog_rasterengine_set_nops(&f.composite[OPERATOR_DST][IMAGE_FORMAT_ARGB32]);
  fog_rasterengine_set_nops(&f.composite[OPERATOR_DST][IMAGE_FORMAT_XRGB32]);
  fog_rasterengine_set_nops(&f.composite[OPERATOR_DST][IMAGE_FORMAT_A8]);
  fog_rasterengine_set_nops(&f.composite[OPERATOR_DST][IMAGE_FORMAT_I8]);

  // --------------------------------------------------------------------------
  // [Composite - SrcOver]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::prgb32_cblit_span;

  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibC::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::prgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeDstOutC::prgb32_vblit_a8_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeDstOutC::prgb32_vblit_a8_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_I8    ] = CompositeSrcOverC::prgb32_vblit_i8_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeSrcOverC::prgb32_vblit_i8_span;

  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibC::memcpy32;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - DstOver]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOverC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOverC::prgb32_cblit_span;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOverC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOverC::prgb32_cblit_span;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDstOverC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDstOverC::prgb32_cblit_span;

  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOverC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOverC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOverC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOverC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  fog_rasterengine_set_nops(&f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_XRGB32]);

  // --------------------------------------------------------------------------
  // [Composite - SrcIn]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcInC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcInC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcInC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcInC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcInC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcInC::prgb32_cblit_span;

  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcInC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcInC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcInC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcInC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = DibC::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = DibC::frgb32_from_argb32;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibC::memcpy32;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_I8    ] = DibC::frgb32_from_i8;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - DstIn]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInC::prgb32_cblit_span;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInC::prgb32_cblit_span;
#endif // FOG_RASTERUTIL_INIT_C
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeNopC::cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeNopC::cblit_span;

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInC::prgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInC::prgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInC::prgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInC::prgb32_vblit_prgb32_or_argb32_span;
#endif // FOG_RASTERUTIL_INIT_C
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeNopC::vblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeNopC::vblit_span;

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInC::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInC::xrgb32_cblit_span;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInC::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInC::xrgb32_cblit_span;
#endif // FOG_RASTERUTIL_INIT_C
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeNopC::cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeNopC::cblit_span;

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInC::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInC::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInC::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInC::xrgb32_vblit_prgb32_or_argb32_span;
#endif // FOG_RASTERUTIL_INIT_C
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeNopC::vblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeNopC::vblit_span;

  // --------------------------------------------------------------------------
  // [Composite - SrcOut]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOutC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOutC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOutC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOutC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_cblit_span;

  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOutC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOutC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOutC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOutC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_I8    ] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeClearC::xrgb32_vblit_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - DstOut]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::prgb32_cblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::prgb32_cblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::prgb32_cblit_span;

  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::prgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::prgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::prgb32_vblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::prgb32_vblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeDstOutC::prgb32_vblit_a8_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeDstOutC::prgb32_vblit_a8_span;

  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::xrgb32_cblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::xrgb32_cblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_cblit_span;

  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeDstOutC::xrgb32_vblit_a8_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeDstOutC::xrgb32_vblit_a8_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - SrcAtop]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcAtopC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcAtopC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcAtopC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcAtopC::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcInC::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcInC::prgb32_cblit_span;

  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcAtopC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcAtopC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcAtopC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcAtopC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcInC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibC::memcpy32;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - DstAtop]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstAtopC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstAtopC::prgb32_cblit_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstAtopC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstAtopC::prgb32_cblit_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDstOverC::prgb32_cblit_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDstOverC::prgb32_cblit_span;

  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstAtopC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstAtopC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstAtopC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstAtopC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDstOverC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInC::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInC::xrgb32_cblit_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInC::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInC::xrgb32_cblit_span;
#endif // FOG_RASTERUTIL_INIT_C
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeNopC::cblit_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeNopC::cblit_span;

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInC::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInC::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInC::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInC::xrgb32_vblit_prgb32_or_argb32_span;
#endif // FOG_RASTERUTIL_INIT_C
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeNopC::vblit_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeNopC::vblit_span;

  // --------------------------------------------------------------------------
  // [Composite - Xor]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeXorC::prgb32_cblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeXorC::prgb32_cblit_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeXorC::prgb32_cblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeXorC::prgb32_cblit_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_cblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_cblit_span;

  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeXorC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeXorC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeXorC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeXorC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcOutC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::xrgb32_cblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::xrgb32_cblit_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::xrgb32_cblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::xrgb32_cblit_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_cblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_cblit_span;

  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutC::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutC::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_vblit_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Clear]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearC::prgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearC::prgb32_cblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearC::prgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearC::prgb32_cblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::prgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::prgb32_cblit_span;

  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearC::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearC::prgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearC::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearC::prgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::prgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearC::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearC::prgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_I8    ] = CompositeClearC::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeClearC::prgb32_vblit_span;

  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearC::xrgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearC::xrgb32_cblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearC::xrgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearC::xrgb32_cblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_cblit_span;

  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearC::xrgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_I8    ] = CompositeClearC::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeClearC::xrgb32_vblit_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Add]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeAddC::prgb32_cblit_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeAddC::prgb32_cblit_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeAddC::prgb32_cblit_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeAddC::prgb32_cblit_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeAddC::prgb32_cblit_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeAddC::prgb32_cblit_span;

  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeAddC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeAddC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeAddC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeAddC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeAddC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeAddC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeAddC::xrgb32_cblit_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeAddC::xrgb32_cblit_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeAddC::xrgb32_cblit_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeAddC::xrgb32_cblit_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeAddC::xrgb32_cblit_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeAddC::xrgb32_cblit_span;

  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeAddC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeAddC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeAddC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeAddC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeAddC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeAddC::xrgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeNopC::vblit_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeNopC::vblit_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Subtract]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSubtractC::prgb32_cblit_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSubtractC::prgb32_cblit_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSubtractC::prgb32_cblit_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSubtractC::prgb32_cblit_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSubtractC::prgb32_cblit_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSubtractC::prgb32_cblit_span;

  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSubtractC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSubtractC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSubtractC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSubtractC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSubtractC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSubtractC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSubtractC::xrgb32_cblit_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSubtractC::xrgb32_cblit_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSubtractC::xrgb32_cblit_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSubtractC::xrgb32_cblit_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSubtractC::xrgb32_cblit_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSubtractC::xrgb32_cblit_span;

  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSubtractC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSubtractC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSubtractC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSubtractC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSubtractC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSubtractC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Multiply]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeMultiplyC::prgb32_cblit_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeMultiplyC::prgb32_cblit_span;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeMultiplyC::prgb32_cblit_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeMultiplyC::prgb32_cblit_span;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeMultiplyC::prgb32_cblit_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeMultiplyC::prgb32_cblit_span;

  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeMultiplyC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeMultiplyC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeMultiplyC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeMultiplyC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeMultiplyC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeMultiplyC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeMultiplyC::xrgb32_cblit_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeMultiplyC::xrgb32_cblit_span;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeMultiplyC::xrgb32_cblit_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeMultiplyC::xrgb32_cblit_span;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeMultiplyC::xrgb32_cblit_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeMultiplyC::xrgb32_cblit_span;

  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeMultiplyC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeMultiplyC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeMultiplyC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeMultiplyC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeMultiplyC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_MULTIPLY][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeMultiplyC::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - Screen]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeScreenC::prgb32_cblit_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeScreenC::prgb32_cblit_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeScreenC::prgb32_cblit_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeScreenC::prgb32_cblit_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeScreenC::prgb32_cblit_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeScreenC::prgb32_cblit_span;

  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeScreenC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeScreenC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeScreenC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeScreenC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeScreenC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeScreenC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeScreenC::xrgb32_cblit_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeScreenC::xrgb32_cblit_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeScreenC::xrgb32_cblit_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeScreenC::xrgb32_cblit_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeScreenC::xrgb32_cblit_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeScreenC::xrgb32_cblit_span;

  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeScreenC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeScreenC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeScreenC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeScreenC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeScreenC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeScreenC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Darken]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDarkenC::prgb32_cblit_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDarkenC::prgb32_cblit_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDarkenC::prgb32_cblit_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDarkenC::prgb32_cblit_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDarkenC::prgb32_cblit_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDarkenC::prgb32_cblit_span;

  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDarkenC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDarkenC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDarkenC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDarkenC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDarkenC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDarkenC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDarkenC::xrgb32_cblit_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDarkenC::xrgb32_cblit_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDarkenC::xrgb32_cblit_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDarkenC::xrgb32_cblit_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDarkenC::xrgb32_cblit_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDarkenC::xrgb32_cblit_span;

  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDarkenC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDarkenC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDarkenC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDarkenC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDarkenC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDarkenC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Lighten]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeLightenC::prgb32_cblit_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeLightenC::prgb32_cblit_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeLightenC::prgb32_cblit_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeLightenC::prgb32_cblit_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeLightenC::prgb32_cblit_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeLightenC::prgb32_cblit_span;

  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeLightenC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeLightenC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeLightenC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeLightenC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeLightenC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeLightenC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeLightenC::xrgb32_cblit_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeLightenC::xrgb32_cblit_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeLightenC::xrgb32_cblit_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeLightenC::xrgb32_cblit_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeLightenC::xrgb32_cblit_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeLightenC::xrgb32_cblit_span;

  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeLightenC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeLightenC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeLightenC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeLightenC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeLightenC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeLightenC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Difference]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDifferenceC::prgb32_cblit_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDifferenceC::prgb32_cblit_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDifferenceC::prgb32_cblit_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDifferenceC::prgb32_cblit_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDifferenceC::prgb32_cblit_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDifferenceC::prgb32_cblit_span;

  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDifferenceC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDifferenceC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDifferenceC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDifferenceC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDifferenceC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDifferenceC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDifferenceC::xrgb32_cblit_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDifferenceC::xrgb32_cblit_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDifferenceC::xrgb32_cblit_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDifferenceC::xrgb32_cblit_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDifferenceC::xrgb32_cblit_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDifferenceC::xrgb32_cblit_span;

  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDifferenceC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDifferenceC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDifferenceC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDifferenceC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDifferenceC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDifferenceC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Exclusion]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeExclusionC::prgb32_cblit_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeExclusionC::prgb32_cblit_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeExclusionC::prgb32_cblit_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeExclusionC::prgb32_cblit_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeExclusionC::prgb32_cblit_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeExclusionC::prgb32_cblit_span;

  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeExclusionC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeExclusionC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeExclusionC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeExclusionC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeExclusionC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeExclusionC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeExclusionC::xrgb32_cblit_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeExclusionC::xrgb32_cblit_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeExclusionC::xrgb32_cblit_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeExclusionC::xrgb32_cblit_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeExclusionC::xrgb32_cblit_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeExclusionC::xrgb32_cblit_span;

  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeExclusionC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeExclusionC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeExclusionC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeExclusionC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeExclusionC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeExclusionC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - Invert]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertC::prgb32_cblit_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertC::prgb32_cblit_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertC::prgb32_cblit_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertC::prgb32_cblit_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertC::prgb32_cblit_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertC::prgb32_cblit_span;

  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertC::xrgb32_cblit_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertC::xrgb32_cblit_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertC::xrgb32_cblit_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertC::xrgb32_cblit_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertC::xrgb32_cblit_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertC::xrgb32_cblit_span;

  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Composite - InvertRgb]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbC::prgb32_cblit_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbC::prgb32_cblit_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbC::prgb32_cblit_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbC::prgb32_cblit_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbC::prgb32_cblit_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbC::prgb32_cblit_span;

  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbC::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbC::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbC::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbC::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbC::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbC::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbC::xrgb32_cblit_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbC::xrgb32_cblit_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbC::xrgb32_cblit_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbC::xrgb32_cblit_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbC::xrgb32_cblit_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbC::xrgb32_cblit_span;

  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbC::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbC::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbC::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbC::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbC::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbC::xrgb32_vblit_xrgb32_span;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Interpolate - Gradient]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.interpolate.gradient[IMAGE_FORMAT_PRGB32] = InterpolateC::gradient_prgb32;
  f.interpolate.gradient[IMAGE_FORMAT_ARGB32] = InterpolateC::gradient_argb32;
  f.interpolate.gradient[IMAGE_FORMAT_XRGB32] = InterpolateC::gradient_xrgb32;
  f.interpolate.gradient[IMAGE_FORMAT_A8] = InterpolateC::gradient_a8;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Pattern - Solid]
  // --------------------------------------------------------------------------

  f.pattern.solid_init = PatternSolidC::init;
  f.pattern.solid_fetch = PatternSolidC::fetch;

  // --------------------------------------------------------------------------
  // [Pattern - Texture]
  // --------------------------------------------------------------------------

  f.pattern.texture_init = PatternTextureC::init;
  f.pattern.texture_init_blit = PatternTextureC::init_blit;
  f.pattern.texture_init_scale = PatternScaleC::init;

#if defined(FOG_RASTERUTIL_INIT_C)
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_exact_none_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_exact_none_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_exact_none_32<PatternTextureC::FMT_XRGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_exact_none_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_exact[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_exact_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_exact_pad_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_exact_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_exact_pad_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_exact[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_exact_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_exact_repeat_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_exact_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_exact_repeat_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_exact[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_exact_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_exact_reflect_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_exact_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_exact_reflect_32<PatternTextureC::FMT_I8>;
#endif // FOG_RASTERUTIL_INIT_C

  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_subx0_none_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_subx0_none_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_subx0_none_32<PatternTextureC::FMT_XRGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_subx0_none_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_subx0_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_subx0_pad_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_subx0_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_subx0_pad_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_subx0_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_subx0_repeat_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_subx0_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_subx0_repeat_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_subx0_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_subx0_reflect_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_subx0_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subx0[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_subx0_reflect_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_sub0y_none_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_sub0y_none_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_sub0y_none_32<PatternTextureC::FMT_XRGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_sub0y_none_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_sub0y_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_sub0y_pad_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_sub0y_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_sub0y_pad_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_sub0y_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_sub0y_repeat_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_sub0y_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_sub0y_repeat_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_sub0y_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_sub0y_reflect_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_sub0y_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_sub0y[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_sub0y_reflect_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_subxy_none_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_subxy_none_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_subxy_none_32<PatternTextureC::FMT_XRGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_subxy_none_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_subxy_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_subxy_pad_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_subxy_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_subxy_pad_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_subxy_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_subxy_repeat_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_subxy_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_subxy_repeat_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_subxy_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_subxy_reflect_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_subxy_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_subxy[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_subxy_reflect_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_trans_affine_nearest_none_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_trans_affine_nearest_none_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_trans_affine_nearest_none_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_trans_affine_nearest_none_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_trans_affine_nearest_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_trans_affine_nearest_pad_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_trans_affine_nearest_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_trans_affine_nearest_pad_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_trans_affine_nearest_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_trans_affine_nearest_repeat_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_trans_affine_nearest_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_trans_affine_nearest_repeat_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_trans_affine_nearest_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_trans_affine_nearest_reflect_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_trans_affine_nearest_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_nearest[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_trans_affine_nearest_reflect_32<PatternTextureC::FMT_I8>;

  // TODO: Implement all in SSE2
#if defined(FOG_RASTERUTIL_INIT_C)
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_trans_affine_bilinear_none_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_trans_affine_bilinear_none_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_trans_affine_bilinear_none_32<PatternTextureC::FMT_XRGB32>;
#endif // FOG_RASTERUTIL_INIT_C
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_NONE   ] = PatternTextureC::fetch_trans_affine_bilinear_none_32<PatternTextureC::FMT_I8>;

#if defined(FOG_RASTERUTIL_INIT_C)
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_trans_affine_bilinear_pad_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_trans_affine_bilinear_pad_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_trans_affine_bilinear_pad_32<PatternTextureC::FMT_PRGB32>;
#endif // FOG_RASTERUTIL_INIT_C
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_PAD    ] = PatternTextureC::fetch_trans_affine_bilinear_pad_32<PatternTextureC::FMT_I8>;

#if defined(FOG_RASTERUTIL_INIT_C)
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_trans_affine_bilinear_repeat_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_trans_affine_bilinear_repeat_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_trans_affine_bilinear_repeat_32<PatternTextureC::FMT_PRGB32>;
#endif // FOG_RASTERUTIL_INIT_C
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REPEAT ] = PatternTextureC::fetch_trans_affine_bilinear_repeat_32<PatternTextureC::FMT_I8>;

#if defined(FOG_RASTERUTIL_INIT_C)
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_trans_affine_bilinear_reflect_32<PatternTextureC::FMT_PRGB32>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_trans_affine_bilinear_reflect_32<PatternTextureC::FMT_ARGB32>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_trans_affine_bilinear_reflect_32<PatternTextureC::FMT_PRGB32>;
#endif // FOG_RASTERUTIL_INIT_C
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_I8    ][PATTERN_SPREAD_REFLECT] = PatternTextureC::fetch_trans_affine_bilinear_reflect_32<PatternTextureC::FMT_I8>;

  f.pattern.texture_fetch_scale_nearest[IMAGE_FORMAT_PRGB32] = PatternScaleC::fetch_argb32_nn;
  f.pattern.texture_fetch_scale_nearest[IMAGE_FORMAT_ARGB32] = PatternScaleC::fetch_argb32_nn;
  f.pattern.texture_fetch_scale_nearest[IMAGE_FORMAT_XRGB32] = PatternScaleC::fetch_argb32_nn;

  f.pattern.texture_fetch_scale_bilinear[IMAGE_FORMAT_PRGB32] = PatternScaleC::fetch_argb32_aa;
  f.pattern.texture_fetch_scale_bilinear[IMAGE_FORMAT_ARGB32] = PatternScaleC::fetch_argb32_aa;
  f.pattern.texture_fetch_scale_bilinear[IMAGE_FORMAT_XRGB32] = PatternScaleC::fetch_argb32_aa;

  // --------------------------------------------------------------------------
  // [Pattern - Linear Gradient]
  // --------------------------------------------------------------------------

  f.pattern.linear_gradient_init = PatternGradientC::init_linear;

  f.pattern.linear_gradient_fetch_exact[PATTERN_SPREAD_NONE   ] = PatternGradientC::fetch_linear_exact_pad;
  f.pattern.linear_gradient_fetch_exact[PATTERN_SPREAD_PAD    ] = PatternGradientC::fetch_linear_exact_pad;
  f.pattern.linear_gradient_fetch_exact[PATTERN_SPREAD_REPEAT ] = PatternGradientC::fetch_linear_exact_repeat;
  f.pattern.linear_gradient_fetch_exact[PATTERN_SPREAD_REFLECT] = PatternGradientC::fetch_linear_exact_repeat;

//#if defined(FOG_RASTERUTIL_INIT_C)
  f.pattern.linear_gradient_fetch_subxy[PATTERN_SPREAD_NONE   ] = PatternGradientC::fetch_linear_subxy_pad;
  f.pattern.linear_gradient_fetch_subxy[PATTERN_SPREAD_PAD    ] = PatternGradientC::fetch_linear_subxy_pad;
  f.pattern.linear_gradient_fetch_subxy[PATTERN_SPREAD_REPEAT ] = PatternGradientC::fetch_linear_subxy_repeat;
  f.pattern.linear_gradient_fetch_subxy[PATTERN_SPREAD_REFLECT] = PatternGradientC::fetch_linear_subxy_repeat;
//#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Pattern - Radial Gradient]
  // --------------------------------------------------------------------------

  f.pattern.radial_gradient_init = PatternGradientC::init_radial;

//#if defined(FOG_RASTERUTIL_INIT_C)
  f.pattern.radial_gradient_fetch[PATTERN_SPREAD_NONE   ] = PatternGradientC::fetch_radial_pad;
  f.pattern.radial_gradient_fetch[PATTERN_SPREAD_PAD    ] = PatternGradientC::fetch_radial_pad;
//#endif // FOG_RASTERUTIL_INIT_C
  f.pattern.radial_gradient_fetch[PATTERN_SPREAD_REPEAT ] = PatternGradientC::fetch_radial_repeat;
  f.pattern.radial_gradient_fetch[PATTERN_SPREAD_REFLECT] = PatternGradientC::fetch_radial_repeat;

  // --------------------------------------------------------------------------
  // [Pattern - Conical Gradient]
  // --------------------------------------------------------------------------

  f.pattern.conical_gradient_init = PatternGradientC::init_conical;
  f.pattern.conical_gradient_fetch = PatternGradientC::fetch_conical;

  // --------------------------------------------------------------------------
  // [Filter - ColorLut]
  // --------------------------------------------------------------------------

  f.filter.color_lut[IMAGE_FORMAT_PRGB32] = FilterC::color_lut_prgb32;
  f.filter.color_lut[IMAGE_FORMAT_ARGB32] = FilterC::color_lut_argb32;
  f.filter.color_lut[IMAGE_FORMAT_XRGB32] = FilterC::color_lut_xrgb32;
  f.filter.color_lut[IMAGE_FORMAT_A8    ] = FilterC::color_lut_a8;

  // --------------------------------------------------------------------------
  // [Filter - ColorMatrix]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.filter.color_matrix[IMAGE_FORMAT_PRGB32] = FilterC::color_matrix_prgb32;
  f.filter.color_matrix[IMAGE_FORMAT_ARGB32] = FilterC::color_matrix_argb32;
  f.filter.color_matrix[IMAGE_FORMAT_XRGB32] = FilterC::color_matrix_xrgb32;
  f.filter.color_matrix[IMAGE_FORMAT_A8    ] = FilterC::color_matrix_a8;
#endif // FOG_RASTERUTIL_INIT_C

  // --------------------------------------------------------------------------
  // [Filters - CopyArea]
  // --------------------------------------------------------------------------

  f.filter.copy_area[IMAGE_FORMAT_PRGB32] = FilterC::copy_area_32;
  f.filter.copy_area[IMAGE_FORMAT_ARGB32] = FilterC::copy_area_32;
  f.filter.copy_area[IMAGE_FORMAT_XRGB32] = FilterC::copy_area_32;
  f.filter.copy_area[IMAGE_FORMAT_A8    ] = FilterC::copy_area_8;
  f.filter.copy_area[IMAGE_FORMAT_I8    ] = FilterC::copy_area_8;

  // --------------------------------------------------------------------------
  // [Filters - BoxBlur]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.filter.box_blur_h[IMAGE_FORMAT_ARGB32] = FilterC::box_blur_h_argb32;
  f.filter.box_blur_v[IMAGE_FORMAT_ARGB32] = FilterC::box_blur_v_argb32;
  f.filter.box_blur_h[IMAGE_FORMAT_XRGB32] = FilterC::box_blur_h_xrgb32;
  f.filter.box_blur_v[IMAGE_FORMAT_XRGB32] = FilterC::box_blur_v_xrgb32;
#endif // FOG_RASTERUTIL_INIT_C
  f.filter.box_blur_h[IMAGE_FORMAT_A8    ] = FilterC::box_blur_h_a8;
  f.filter.box_blur_v[IMAGE_FORMAT_A8    ] = FilterC::box_blur_v_a8;

  // --------------------------------------------------------------------------
  // [Filters - LinearBlur]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.filter.linear_blur_h[IMAGE_FORMAT_ARGB32] = FilterC::linear_blur_h_argb32;
  f.filter.linear_blur_v[IMAGE_FORMAT_ARGB32] = FilterC::linear_blur_v_argb32;
  f.filter.linear_blur_h[IMAGE_FORMAT_XRGB32] = FilterC::linear_blur_h_argb32;
  f.filter.linear_blur_v[IMAGE_FORMAT_XRGB32] = FilterC::linear_blur_v_argb32;
#endif // FOG_RASTERUTIL_INIT_C
  f.filter.linear_blur_h[IMAGE_FORMAT_A8    ] = FilterC::linear_blur_h_a8;
  f.filter.linear_blur_v[IMAGE_FORMAT_A8    ] = FilterC::linear_blur_v_a8;

  // --------------------------------------------------------------------------
  // [Filters - SymmetricConvolveFloat]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTERUTIL_INIT_C)
  f.filter.symmetric_convolve_float_h[IMAGE_FORMAT_ARGB32] = FilterC::symmetric_convolve_float_h_argb32;
  f.filter.symmetric_convolve_float_v[IMAGE_FORMAT_ARGB32] = FilterC::symmetric_convolve_float_v_argb32;
  f.filter.symmetric_convolve_float_h[IMAGE_FORMAT_XRGB32] = FilterC::symmetric_convolve_float_h_argb32;
  f.filter.symmetric_convolve_float_v[IMAGE_FORMAT_XRGB32] = FilterC::symmetric_convolve_float_v_argb32;
#endif // FOG_RASTERUTIL_INIT_C

  f.filter.symmetric_convolve_float_h[IMAGE_FORMAT_A8    ] = FilterC::symmetric_convolve_float_h_a8;
  f.filter.symmetric_convolve_float_v[IMAGE_FORMAT_A8    ] = FilterC::symmetric_convolve_float_v_a8;

  // --------------------------------------------------------------------------
  // [Mask]
  // --------------------------------------------------------------------------

  f.mask[CLIP_OP_REPLACE  ][IMAGE_FORMAT_A8].c_op_v = MaskReplaceC::a8_c_op_v;
  f.mask[CLIP_OP_REPLACE  ][IMAGE_FORMAT_A8].v_op_c = MaskReplaceC::a8_v_op_c;
  f.mask[CLIP_OP_REPLACE  ][IMAGE_FORMAT_A8].v_op_v = MaskReplaceC::a8_v_op_v;

  f.mask[CLIP_OP_INTERSECT][IMAGE_FORMAT_A8].c_op_v = MaskIntersectC::a8_c_op_v;
  f.mask[CLIP_OP_INTERSECT][IMAGE_FORMAT_A8].v_op_c = MaskIntersectC::a8_v_op_c;
  f.mask[CLIP_OP_INTERSECT][IMAGE_FORMAT_A8].v_op_v = MaskIntersectC::a8_v_op_v;
}
