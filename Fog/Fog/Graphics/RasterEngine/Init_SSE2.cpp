// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/ByteSIMD_p.h>
#include <Fog/Core/Intrin/IntrinSSE2.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Misc.h>
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

// [Fog::RasterEngine::SSE2]
#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_SSE2_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_SSE2_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Core_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Src_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_SrcOver_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_DstOver_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_SrcIn_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_DstIn_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_SrcOut_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_DstOut_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_SrcAtop_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_DstAtop_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Xor_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Clear_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Add_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Subtract_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Multiply_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Screen_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Darken_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Lighten_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Difference_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Exclusion_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_Invert_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSE2_InvertRgb_p.h>
#include <Fog/Graphics/RasterEngine/Mask_SSE2_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_SSE2_Core_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_SSE2_Solid_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_SSE2_Interpolate_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_SSE2_Gradient_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_SSE2_Scale_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_SSE2_Texture_p.h>
#include <Fog/Graphics/RasterEngine/Filters_SSE2_p.h>

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE void fog_rasterengine_init_sse2(void)
{
  using namespace Fog;
  using namespace Fog::RasterEngine;

  RasterFuncs& f = rasterFuncs;

  // --------------------------------------------------------------------------
  // [Dib - MemCpy]
  // --------------------------------------------------------------------------

  f.dib.memcpy[IMAGE_FORMAT_PRGB32] = DibSSE2::memcpy32;
  f.dib.memcpy[IMAGE_FORMAT_ARGB32] = DibSSE2::memcpy32;
  f.dib.memcpy[IMAGE_FORMAT_XRGB32] = DibSSE2::memcpy32;
  f.dib.memcpy[IMAGE_FORMAT_A8    ] = DibSSE2::memcpy8;
  f.dib.memcpy[IMAGE_FORMAT_I8    ] = DibSSE2::memcpy8;

  f.dib.memcpy8 = DibSSE2::memcpy8;
  f.dib.memcpy16 = DibSSE2::memcpy16;
  f.dib.memcpy24 = DibSSE2::memcpy24;
  f.dib.memcpy32 = DibSSE2::memcpy32;

  // --------------------------------------------------------------------------
  // [Dib - Rect]
  // --------------------------------------------------------------------------

  f.dib.cblit_rect[IMAGE_FORMAT_PRGB32] = DibSSE2::cblit_rect_32_prgb;
  f.dib.cblit_rect[IMAGE_FORMAT_ARGB32] = DibSSE2::cblit_rect_32_argb;
  f.dib.cblit_rect[IMAGE_FORMAT_XRGB32] = DibSSE2::cblit_rect_32_prgb;

  f.dib.vblit_rect[IMAGE_FORMAT_PRGB32] = DibSSE2::vblit_rect_32;
  f.dib.vblit_rect[IMAGE_FORMAT_ARGB32] = DibSSE2::vblit_rect_32;
  f.dib.vblit_rect[IMAGE_FORMAT_XRGB32] = DibSSE2::vblit_rect_32;

  // --------------------------------------------------------------------------
  // [Dib - Convert]
  // --------------------------------------------------------------------------

  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = DibSSE2::memcpy32;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = DibSSE2::prgb32_from_argb32;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = DibSSE2::frgb32_from_xrgb32;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_A8               ] = DibSSE2::azzz32_from_a8;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_I8               ] = DibSSE2::prgb32_from_i8;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = DibSSE2::bswap32;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = DibSSE2::prgb32_from_argb32_swapped;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = DibSSE2::frgb32_from_xrgb32_swapped;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB24_NATIVE     ] = DibSSE2::frgb32_from_rgb24_native;
  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = DibSSE2::frgb32_from_rgb24_swapped;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = DibSSE2::frgb32_from_rgb16_565_native;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = DibSSE2::frgb32_from_rgb16_565_swapped;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = DibSSE2::frgb32_from_rgb16_555_native;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = DibSSE2::frgb32_from_rgb16_555_swapped;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_GREY8            ] = DibSSE2::frgb32_from_grey8;

  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_PRGB32_NATIVE    ] = DibSSE2::argb32_from_prgb32;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_ARGB32_NATIVE    ] = DibSSE2::memcpy32;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_XRGB32_NATIVE    ] = DibSSE2::frgb32_from_xrgb32;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_A8               ] = DibSSE2::azzz32_from_a8;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_I8               ] = DibSSE2::argb32_from_i8;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = DibSSE2::argb32_from_prgb32_swapped;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = DibSSE2::bswap32;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = DibSSE2::frgb32_from_xrgb32_swapped;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB24_NATIVE     ] = DibSSE2::frgb32_from_rgb24_native;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB24_SWAPPED    ] = DibSSE2::frgb32_from_rgb24_swapped;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_NATIVE ] = DibSSE2::frgb32_from_rgb16_565_native;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_SWAPPED] = DibSSE2::frgb32_from_rgb16_565_swapped;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_NATIVE ] = DibSSE2::frgb32_from_rgb16_555_native;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_SWAPPED] = DibSSE2::frgb32_from_rgb16_555_swapped;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_GREY8            ] = DibSSE2::frgb32_from_grey8;

  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = DibSSE2::frgb32_from_xrgb32;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = DibSSE2::frgb32_from_argb32;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = DibSSE2::frgb32_from_xrgb32;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_A8               ] = DibSSE2::fzzz32_from_null;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_I8               ] = DibSSE2::frgb32_from_i8;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = DibSSE2::frgb32_from_xrgb32_swapped;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = DibSSE2::frgb32_from_argb32_swapped;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = DibSSE2::bswap32;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB24_NATIVE     ] = DibSSE2::frgb32_from_rgb24_native;
  f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = DibSSE2::frgb32_from_rgb24_swapped;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = DibSSE2::frgb32_from_rgb16_565_native;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = DibSSE2::frgb32_from_rgb16_565_swapped;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = DibSSE2::frgb32_from_rgb16_555_native;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = DibSSE2::frgb32_from_rgb16_555_swapped;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_GREY8            ] = DibSSE2::frgb32_from_grey8;

  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_PRGB32_NATIVE    ] = DibSSE2::a8_from_axxx32;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_ARGB32_NATIVE    ] = DibSSE2::a8_from_axxx32;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_XRGB32_NATIVE    ] = DibSSE2::f8_from_null;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_A8               ] = DibSSE2::memcpy8;
  f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_I8               ] = DibSSE2::a8_from_i8;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_PRGB32_SWAPPED   ] = DibSSE2::a8_from_axxx32_swapped;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_ARGB32_SWAPPED   ] = DibSSE2::a8_from_axxx32_swapped;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_XRGB32_SWAPPED   ] = DibSSE2::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB24_NATIVE     ] = DibSSE2::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB24_SWAPPED    ] = DibSSE2::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_565_NATIVE ] = DibSSE2::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_565_SWAPPED] = DibSSE2::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_555_NATIVE ] = DibSSE2::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_555_SWAPPED] = DibSSE2::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_GREY8            ] = DibSSE2::f8_from_null;

//f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_PRGB32] = DibSSE2::bswap32;
  f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_ARGB32] = DibSSE2::prgb32_swapped_from_argb32;
//f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_XRGB32] = DibSSE2::frgb32_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_A8    ] = DibSSE2::azzz32_swapped_from_a8;
//f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_I8    ] = DibSSE2::prgb32_swapped_from_i8;

//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_PRGB32] = DibSSE2::argb32_swapped_from_prgb32;
//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_ARGB32] = DibSSE2::bswap32;
//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_XRGB32] = DibSSE2::frgb32_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_A8    ] = DibSSE2::azzz32_swapped_from_a8;
//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_I8    ] = DibSSE2::argb32_swapped_from_i8;

//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_PRGB32] = DibSSE2::frgb32_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_ARGB32] = DibSSE2::frgb32_swapped_from_argb32;
//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_XRGB32] = DibSSE2::frgb32_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_A8    ] = DibSSE2::fzzz32_swapped_from_null;
//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_I8    ] = DibSSE2::frgb32_swapped_from_i8;

//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_PRGB32] = DibSSE2::rgb24_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_ARGB32] = DibSSE2::rgb24_native_from_argb32;
//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_XRGB32] = DibSSE2::rgb24_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_A8    ] = DibSSE2::zzz24_from_null;
//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_I8    ] = DibSSE2::rgb24_native_from_i8;

//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_PRGB32] = DibSSE2::rgb24_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_ARGB32] = DibSSE2::rgb24_swapped_from_argb32;
//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_XRGB32] = DibSSE2::rgb24_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_A8    ] = DibSSE2::zzz24_from_null;
//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_I8    ] = DibSSE2::rgb24_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_PRGB32] = DibSSE2::rgb16_565_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_ARGB32] = DibSSE2::rgb16_565_native_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_XRGB32] = DibSSE2::rgb16_565_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_A8    ] = DibSSE2::zzz16_from_null;
//f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_I8    ] = DibSSE2::rgb16_565_native_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_PRGB32] = DibSSE2::rgb16_565_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_ARGB32] = DibSSE2::rgb16_565_swapped_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_XRGB32] = DibSSE2::rgb16_565_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_A8    ] = DibSSE2::zzz16_from_null;
//f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_I8    ] = DibSSE2::rgb16_565_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_PRGB32] = DibSSE2::rgb16_555_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_ARGB32] = DibSSE2::rgb16_555_native_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_XRGB32] = DibSSE2::rgb16_555_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_A8    ] = DibSSE2::zzz16_from_null;
//f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_I8    ] = DibSSE2::rgb16_555_native_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_PRGB32] = DibSSE2::rgb16_555_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_ARGB32] = DibSSE2::rgb16_555_swapped_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_XRGB32] = DibSSE2::rgb16_555_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_A8    ] = DibSSE2::zzz16_from_null;
//f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_I8    ] = DibSSE2::rgb16_555_swapped_from_i8;

//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_PRGB32] = DibSSE2::grey8_from_xrgb32;
//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_ARGB32] = DibSSE2::grey8_from_argb32;
//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_XRGB32] = DibSSE2::grey8_from_xrgb32;
//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_A8    ] = DibSSE2::z8_from_null;
//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_I8    ] = DibSSE2::grey8_from_i8;

  // --------------------------------------------------------------------------
  // [Composite - Src]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::prgb32_cblit_span;

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = DibSSE2::memcpy32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = DibSSE2::prgb32_from_argb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibSSE2::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::prgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_A8    ] = DibSSE2::azzz32_from_a8;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeSrcSSE2::prgb32_vblit_a8_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_I8    ] = DibSSE2::prgb32_from_i8;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeSrcSSE2::prgb32_vblit_i8_span;

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_PRGB32] = DibSSE2::argb32_from_prgb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_ARGB32] = DibSSE2::memcpy32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibSSE2::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_I8    ] = DibSSE2::argb32_from_i8;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_ARGB32].vblit_full[IMAGE_FORMAT_A8    ] = DibSSE2::azzz32_from_a8;

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = DibSSE2::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = DibSSE2::frgb32_from_argb32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibSSE2::memcpy32;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_I8    ] = DibSSE2::frgb32_from_i8;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeSrcSSE2::xrgb32_vblit_i8_span;

  // --------------------------------------------------------------------------
  // [Composite - SrcOver]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::prgb32_cblit_span;

  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibSSE2::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::prgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeDstOutSSE2::prgb32_vblit_a8_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeDstOutSSE2::prgb32_vblit_a8_span;
//  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_I8    ] = CompositeSrcOverSSE2::prgb32_vblit_i8_full;
//  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeSrcOverSSE2::prgb32_vblit_i8_span;

  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibSSE2::memcpy32;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - DstOver]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOverSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOverSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOverSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOverSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDstOverSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDstOverSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOverSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOverSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOverSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOverSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDstOverSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DST_OVER][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDstOverSSE2::prgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - SrcIn]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcInSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcInSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcInSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcInSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcInSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcInSSE2::prgb32_cblit_span;

  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcInSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcInSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcInSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcInSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcInSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcInSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = DibSSE2::frgb32_from_xrgb32;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcSSE2::xrgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = DibSSE2::frgb32_from_argb32;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibSSE2::memcpy32;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_vblit_xrgb32_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_I8    ] = DibSSE2::frgb32_from_i8;

  // --------------------------------------------------------------------------
  // [Composite - DstIn]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::prgb32_cblit_span;
//f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = NOP;
//f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = NOP;

  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::prgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::prgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::prgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::prgb32_vblit_prgb32_or_argb32_span;
//f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = NOP;
//f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = NOP;

  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::xrgb32_cblit_span;
//f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = NOP;
//f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = NOP;

  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::xrgb32_vblit_prgb32_or_argb32_span;
//f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = NOP;
//f.composite[OPERATOR_DST_IN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = NOP;

  // --------------------------------------------------------------------------
  // [Composite - SrcOut]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOutSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOutSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOutSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOutSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcOutSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcOutSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOutSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOutSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOutSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOutSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcOutSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcOutSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_I8    ] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_SRC_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeClearSSE2::xrgb32_vblit_span;

  // --------------------------------------------------------------------------
  // [Composite - DstOut]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::prgb32_cblit_span;

  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::prgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::prgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::prgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::prgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::prgb32_vblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::prgb32_vblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeDstOutSSE2::prgb32_vblit_a8_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeDstOutSSE2::prgb32_vblit_a8_span;

  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_cblit_span;

  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeDstOutSSE2::xrgb32_vblit_a8_full;
  f.composite[OPERATOR_DST_OUT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeDstOutSSE2::xrgb32_vblit_a8_span;

  // --------------------------------------------------------------------------
  // [Composite - SrcAtop]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcAtopSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcAtopSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcAtopSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcAtopSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcInSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcInSSE2::prgb32_cblit_span;

  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcAtopSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcAtopSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcAtopSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcAtopSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcInSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcInSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_cblit_span;

  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSrcOverSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = DibSSE2::memcpy32;
  f.composite[OPERATOR_SRC_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - DstAtop]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstAtopSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstAtopSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstAtopSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstAtopSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDstOverSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDstOverSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstAtopSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstAtopSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstAtopSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstAtopSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDstOverSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDstOverSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::xrgb32_cblit_span;
//f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = NOP;
//f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = NOP;

  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstInSSE2::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstInSSE2::xrgb32_vblit_prgb32_or_argb32_span;
//f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = NOP;
//f.composite[OPERATOR_DST_ATOP][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = NOP;

  // --------------------------------------------------------------------------
  // [Composite - Xor]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeXorSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeXorSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeXorSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeXorSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcOutSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcOutSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeXorSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeXorSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeXorSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeXorSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSrcOutSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSrcOutSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_cblit_span;

  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDstOutSSE2::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::xrgb32_vblit_prgb32_or_argb32_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDstOutSSE2::xrgb32_vblit_prgb32_or_argb32_span;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_XOR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_vblit_span;

  // --------------------------------------------------------------------------
  // [Composite - Clear]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::prgb32_cblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::prgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::prgb32_cblit_span;

  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::prgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::prgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::prgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::prgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_I8    ] = CompositeClearSSE2::prgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeClearSSE2::prgb32_vblit_span;

  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::xrgb32_cblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_cblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_cblit_span;

  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = CompositeClearSSE2::xrgb32_vblit_span;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_I8    ] = CompositeClearSSE2::xrgb32_vblit_full;
  f.composite[OPERATOR_CLEAR][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_I8    ] = CompositeClearSSE2::xrgb32_vblit_span;

  // --------------------------------------------------------------------------
  // [Composite - Add]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeAddSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeAddSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeAddSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeAddSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeAddSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeAddSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeAddSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeAddSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeAddSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeAddSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeAddSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeAddSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeAddSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeAddSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeAddSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeAddSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeAddSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeAddSSE2::xrgb32_cblit_prgb32_span;

  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeAddSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeAddSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeAddSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeAddSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeAddSSE2::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeAddSSE2::xrgb32_vblit_xrgb32_span;
//f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_A8    ] = NOP;
//f.composite[OPERATOR_ADD][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_A8    ] = NOP;

  // --------------------------------------------------------------------------
  // [Composite - Subtract]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSubtractSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSubtractSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSubtractSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSubtractSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSubtractSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSubtractSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSubtractSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSubtractSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSubtractSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSubtractSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSubtractSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSubtractSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeSubtractSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeSubtractSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeSubtractSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeSubtractSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeSubtractSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeSubtractSSE2::xrgb32_cblit_prgb32_span;

  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSubtractSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeSubtractSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeSubtractSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeSubtractSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeSubtractSSE2::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SUBTRACT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeSubtractSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - Multiply]
  // --------------------------------------------------------------------------

  // TODO

  // --------------------------------------------------------------------------
  // [Composite - Screen]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeScreenSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeScreenSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeScreenSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeScreenSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeScreenSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeScreenSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeScreenSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeScreenSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeScreenSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeScreenSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeScreenSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeScreenSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeScreenSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeScreenSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeScreenSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeScreenSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeScreenSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeScreenSSE2::xrgb32_cblit_prgb32_span;

  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeScreenSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeScreenSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeScreenSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeScreenSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeScreenSSE2::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_SCREEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeScreenSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - Darken]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDarkenSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDarkenSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDarkenSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDarkenSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDarkenSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDarkenSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDarkenSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDarkenSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDarkenSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDarkenSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDarkenSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDarkenSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDarkenSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDarkenSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDarkenSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDarkenSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDarkenSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDarkenSSE2::xrgb32_cblit_prgb32_span;

  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDarkenSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDarkenSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDarkenSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDarkenSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDarkenSSE2::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DARKEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDarkenSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - Lighten]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeLightenSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeLightenSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeLightenSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeLightenSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeLightenSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeLightenSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeLightenSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeLightenSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeLightenSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeLightenSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeLightenSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeLightenSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeLightenSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeLightenSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeLightenSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeLightenSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeLightenSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeLightenSSE2::xrgb32_cblit_prgb32_span;

  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeLightenSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeLightenSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeLightenSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeLightenSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeLightenSSE2::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_LIGHTEN][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeLightenSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - Difference]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDifferenceSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDifferenceSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDifferenceSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDifferenceSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDifferenceSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDifferenceSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDifferenceSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDifferenceSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDifferenceSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDifferenceSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDifferenceSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDifferenceSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeDifferenceSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeDifferenceSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeDifferenceSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeDifferenceSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeDifferenceSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeDifferenceSSE2::xrgb32_cblit_prgb32_span;

  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeDifferenceSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeDifferenceSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeDifferenceSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeDifferenceSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeDifferenceSSE2::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_DIFFERENCE][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeDifferenceSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - Exclusion]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeExclusionSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeExclusionSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeExclusionSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeExclusionSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeExclusionSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeExclusionSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeExclusionSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeExclusionSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeExclusionSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeExclusionSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeExclusionSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeExclusionSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeExclusionSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeExclusionSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeExclusionSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeExclusionSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeExclusionSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeExclusionSSE2::xrgb32_cblit_prgb32_span;

  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeExclusionSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeExclusionSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeExclusionSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeExclusionSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeExclusionSSE2::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_EXCLUSION][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeExclusionSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - Invert]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertSSE2::xrgb32_cblit_prgb32_span;

  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertSSE2::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_INVERT][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Composite - InvertRgb]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbSSE2::prgb32_cblit_prgb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbSSE2::prgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbSSE2::prgb32_cblit_prgb32_span;

  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbSSE2::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbSSE2::prgb32_vblit_prgb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbSSE2::prgb32_vblit_argb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbSSE2::prgb32_vblit_argb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbSSE2::prgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_PRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbSSE2::prgb32_vblit_xrgb32_span;

  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbSSE2::xrgb32_cblit_prgb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbSSE2::xrgb32_cblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].cblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbSSE2::xrgb32_cblit_prgb32_span;

  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbSSE2::xrgb32_vblit_prgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_PRGB32] = CompositeInvertRgbSSE2::xrgb32_vblit_prgb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbSSE2::xrgb32_vblit_argb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_ARGB32] = CompositeInvertRgbSSE2::xrgb32_vblit_argb32_span;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbSSE2::xrgb32_vblit_xrgb32_full;
  f.composite[OPERATOR_INVERT_RGB][IMAGE_FORMAT_XRGB32].vblit_span[IMAGE_FORMAT_XRGB32] = CompositeInvertRgbSSE2::xrgb32_vblit_xrgb32_span;

  // --------------------------------------------------------------------------
  // [Interpolate - Gradient]
  // --------------------------------------------------------------------------

  f.interpolate.gradient[IMAGE_FORMAT_PRGB32] = InterpolateSSE2::gradient_prgb32;
  f.interpolate.gradient[IMAGE_FORMAT_ARGB32] = InterpolateSSE2::gradient_argb32;
  f.interpolate.gradient[IMAGE_FORMAT_XRGB32] = InterpolateSSE2::gradient_argb32;

  // --------------------------------------------------------------------------
  // [Pattern - Texture]
  // --------------------------------------------------------------------------

  // TODO: None, Pad

  f.pattern.texture_fetch_exact[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REPEAT] = PatternTextureSSE2::fetch_exact_repeat_32;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REPEAT] = PatternTextureSSE2::fetch_exact_repeat_32;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REPEAT] = PatternTextureSSE2::fetch_exact_repeat_32;

  f.pattern.texture_fetch_exact[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureSSE2::fetch_exact_reflect_32;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REFLECT] = PatternTextureSSE2::fetch_exact_reflect_32;
  f.pattern.texture_fetch_exact[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureSSE2::fetch_exact_reflect_32;

  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureSSE2::fetch_trans_affine_bilinear_none_32<PP_PRGB32_SSE2>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_NONE   ] = PatternTextureSSE2::fetch_trans_affine_bilinear_none_32<PP_ARGB32_SSE2>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_NONE   ] = PatternTextureSSE2::fetch_trans_affine_bilinear_none_32<PP_PRGB32_SSE2>;

  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureSSE2::fetch_trans_affine_bilinear_pad_32<PP_PRGB32_SSE2>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_PAD    ] = PatternTextureSSE2::fetch_trans_affine_bilinear_pad_32<PP_ARGB32_SSE2>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_PAD    ] = PatternTextureSSE2::fetch_trans_affine_bilinear_pad_32<PP_PRGB32_SSE2>;

  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureSSE2::fetch_trans_affine_bilinear_repeat_32<PP_PRGB32_SSE2>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureSSE2::fetch_trans_affine_bilinear_repeat_32<PP_ARGB32_SSE2>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REPEAT ] = PatternTextureSSE2::fetch_trans_affine_bilinear_repeat_32<PP_PRGB32_SSE2>;

  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_PRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureSSE2::fetch_trans_affine_bilinear_reflect_32<PP_PRGB32_SSE2>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_ARGB32][PATTERN_SPREAD_REFLECT] = PatternTextureSSE2::fetch_trans_affine_bilinear_reflect_32<PP_ARGB32_SSE2>;
  f.pattern.texture_fetch_trans_affine_bilinear[IMAGE_FORMAT_XRGB32][PATTERN_SPREAD_REFLECT] = PatternTextureSSE2::fetch_trans_affine_bilinear_reflect_32<PP_PRGB32_SSE2>;

  // --------------------------------------------------------------------------
  // [Pattern - Linear Gradient]
  // --------------------------------------------------------------------------
#if 0
  f.pattern.linear_gradient_fetch_subxy[PATTERN_SPREAD_NONE   ] = PatternSSE2::linear_gradient_fetch_subxy_pad;
  f.pattern.linear_gradient_fetch_subxy[PATTERN_SPREAD_PAD    ] = PatternSSE2::linear_gradient_fetch_subxy_pad;
  f.pattern.linear_gradient_fetch_subxy[PATTERN_SPREAD_REPEAT ] = PatternSSE2::linear_gradient_fetch_subxy_repeat;
  f.pattern.linear_gradient_fetch_subxy[PATTERN_SPREAD_REFLECT] = PatternSSE2::linear_gradient_fetch_subxy_repeat;

  // --------------------------------------------------------------------------
  // [Pattern - Radial Gradient]
  // --------------------------------------------------------------------------

  f.pattern.radial_gradient_fetch[PATTERN_SPREAD_NONE] = PatternSSE2::radial_gradient_fetch_pad;
  f.pattern.radial_gradient_fetch[PATTERN_SPREAD_PAD] = PatternSSE2::radial_gradient_fetch_pad;
#endif
  // --------------------------------------------------------------------------
  // [Filters - ColorMatrix]
  // --------------------------------------------------------------------------

  f.filter.color_matrix[IMAGE_FORMAT_PRGB32] = FilterSSE2::color_matrix_prgb32;
  f.filter.color_matrix[IMAGE_FORMAT_ARGB32] = FilterSSE2::color_matrix_argb32;
  f.filter.color_matrix[IMAGE_FORMAT_XRGB32] = FilterSSE2::color_matrix_xrgb32;
  f.filter.color_matrix[IMAGE_FORMAT_A8    ] = FilterSSE2::color_matrix_a8;

  // --------------------------------------------------------------------------
  // [Filters - BoxBlur]
  // --------------------------------------------------------------------------

  f.filter.box_blur_h[IMAGE_FORMAT_ARGB32] = FilterSSE2::box_blur_h_argb32;
  f.filter.box_blur_v[IMAGE_FORMAT_ARGB32] = FilterSSE2::box_blur_v_argb32;

  f.filter.box_blur_h[IMAGE_FORMAT_XRGB32] = FilterSSE2::box_blur_h_xrgb32;
  f.filter.box_blur_v[IMAGE_FORMAT_XRGB32] = FilterSSE2::box_blur_v_xrgb32;

  // --------------------------------------------------------------------------
  // [Filters - LinearBlur]
  // --------------------------------------------------------------------------

  f.filter.linear_blur_h[IMAGE_FORMAT_ARGB32] = FilterSSE2::linear_blur_h_argb32;
  f.filter.linear_blur_h[IMAGE_FORMAT_XRGB32] = FilterSSE2::linear_blur_h_argb32;

  f.filter.linear_blur_v[IMAGE_FORMAT_ARGB32] = FilterSSE2::linear_blur_v_argb32;
  f.filter.linear_blur_v[IMAGE_FORMAT_XRGB32] = FilterSSE2::linear_blur_v_argb32;

  // --------------------------------------------------------------------------
  // [Filters - SymmetricConvolveFloat]
  // --------------------------------------------------------------------------

  f.filter.symmetric_convolve_float_h[IMAGE_FORMAT_ARGB32] = FilterSSE2::symmetric_convolve_float_h_argb32;
  f.filter.symmetric_convolve_float_h[IMAGE_FORMAT_XRGB32] = FilterSSE2::symmetric_convolve_float_h_argb32;
  f.filter.symmetric_convolve_float_v[IMAGE_FORMAT_ARGB32] = FilterSSE2::symmetric_convolve_float_v_argb32;
  f.filter.symmetric_convolve_float_v[IMAGE_FORMAT_XRGB32] = FilterSSE2::symmetric_convolve_float_v_argb32;
}
