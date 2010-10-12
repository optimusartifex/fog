// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Build.h>

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

#include <Fog/Core/Intrin/IntrinMMX.h>

#if defined(FOG_RASTER_MMX3DNOW)
#include <Fog/Core/Intrin/IntrinMMXExt.h>
#include <Fog/Core/Intrin/Intrin3dNow.h>
#endif // FOG_RASTER_MMX3DNOW

#if defined(FOG_RASTER_MMXSSE)
#include <Fog/Core/Intrin/IntrinSSE.h>
#endif // FOG_RASTER_MMXSSE

#include <Fog/Core/Face/FaceC.h>
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
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/Pattern.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/RasterUtil_p.h>
#include <Fog/Graphics/Span_p.h>
#include <Fog/Graphics/Transform.h>

// ============================================================================
// [RasterEngine - MMX]
// ============================================================================

#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Core_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Src_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_SrcOver_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_DstOver_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_SrcIn_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_DstIn_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_SrcOut_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_DstOut_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_SrcAtop_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_DstAtop_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Xor_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Clear_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Add_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Subtract_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Multiply_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Screen_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Darken_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Lighten_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Difference_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Exclusion_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_Invert_p.h>
#include <Fog/Graphics/RasterEngine/Composite_MMX_InvertRgb_p.h>
#include <Fog/Graphics/RasterEngine/Mask_MMX_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_MMX_Core_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_MMX_Solid_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_MMX_Interpolate_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_MMX_Gradient_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_MMX_Scale_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_MMX_Texture_p.h>
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

  RasterFuncs& f = rasterFuncs;

  // --------------------------------------------------------------------------
  // [Dib - MemCpy]
  // --------------------------------------------------------------------------

  f.dib.memcpy[IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::memcpy32;
  f.dib.memcpy[IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::memcpy32;
  f.dib.memcpy[IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::memcpy32;

//f.dib.memcpy8 = MMX_SYM(Dib)::memcpy8;
//f.dib.memcpy16 = MMX_SYM(Dib)::memcpy16;
//f.dib.memcpy24 = MMX_SYM(Dib)::memcpy24;
  f.dib.memcpy32 = MMX_SYM(Dib)::memcpy32;

  // --------------------------------------------------------------------------
  // [Dib - Convert]
  // --------------------------------------------------------------------------

  f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = MMX_SYM(Dib)::memcpy32;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = MMX_SYM(Dib)::prgb32_from_argb32;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_xrgb32;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_A8               ] = MMX_SYM(Dib)::azzz32_from_a8;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_I8               ] = MMX_SYM(Dib)::prgb32_from_i8;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = MMX_SYM(Dib)::bswap32;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = MMX_SYM(Dib)::prgb32_from_argb32_swapped;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = MMX_SYM(Dib)::frgb32_from_xrgb32_swapped;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB24_NATIVE     ] = MMX_SYM(Dib)::frgb32_from_rgb24_native;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = MMX_SYM(Dib)::frgb32_from_rgb24_swapped;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_565_native;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_565_swapped;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_555_native;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_555_swapped;
//f.dib.convert[IMAGE_FORMAT_PRGB32][DIB_FORMAT_GREY8            ] = MMX_SYM(Dib)::frgb32_from_grey8;

//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_PRGB32_NATIVE    ] = MMX_SYM(Dib)::argb32_from_prgb32;
  f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_ARGB32_NATIVE    ] = MMX_SYM(Dib)::memcpy32;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_XRGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_xrgb32;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_A8               ] = MMX_SYM(Dib)::azzz32_from_a8;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_I8               ] = MMX_SYM(Dib)::argb32_from_i8;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = MMX_SYM(Dib)::argb32_from_prgb32_swapped;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = MMX_SYM(Dib)::bswap32;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = MMX_SYM(Dib)::frgb32_from_xrgb32_swapped;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB24_NATIVE     ] = MMX_SYM(Dib)::frgb32_from_rgb24_native;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB24_SWAPPED    ] = MMX_SYM(Dib)::frgb32_from_rgb24_swapped;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_565_native;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_565_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_565_swapped;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_555_native;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_RGB16_555_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_555_swapped;
//f.dib.convert[IMAGE_FORMAT_ARGB32][DIB_FORMAT_GREY8            ] = MMX_SYM(Dib)::frgb32_from_grey8;

//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_PRGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_xrgb32;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_ARGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_argb32;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_XRGB32_NATIVE    ] = MMX_SYM(Dib)::frgb32_from_xrgb32;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_A8               ] = MMX_SYM(Dib)::fzzz32_from_null;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_I8               ] = MMX_SYM(Dib)::frgb32_from_i8;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_PRGB32_SWAPPED   ] = MMX_SYM(Dib)::frgb32_from_xrgb32_swapped;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_ARGB32_SWAPPED   ] = MMX_SYM(Dib)::frgb32_from_argb32_swapped;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_XRGB32_SWAPPED   ] = MMX_SYM(Dib)::bswap32;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB24_NATIVE     ] = MMX_SYM(Dib)::frgb32_from_rgb24_native;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB24_SWAPPED    ] = MMX_SYM(Dib)::frgb32_from_rgb24_swapped;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_565_native;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_565_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_565_swapped;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_NATIVE ] = MMX_SYM(Dib)::frgb32_from_rgb16_555_native;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_RGB16_555_SWAPPED] = MMX_SYM(Dib)::frgb32_from_rgb16_555_swapped;
//f.dib.convert[IMAGE_FORMAT_XRGB32][DIB_FORMAT_GREY8            ] = MMX_SYM(Dib)::frgb32_from_grey8;

//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_PRGB32_NATIVE    ] = MMX_SYM(Dib)::a8_from_axxx32;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_ARGB32_NATIVE    ] = MMX_SYM(Dib)::a8_from_axxx32;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_XRGB32_NATIVE    ] = MMX_SYM(Dib)::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_A8               ] = MMX_SYM(Dib)::memcpy8;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_I8               ] = MMX_SYM(Dib)::a8_from_i8;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_PRGB32_SWAPPED   ] = MMX_SYM(Dib)::a8_from_axxx32_swapped;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_ARGB32_SWAPPED   ] = MMX_SYM(Dib)::a8_from_axxx32_swapped;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_XRGB32_SWAPPED   ] = MMX_SYM(Dib)::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB24_NATIVE     ] = MMX_SYM(Dib)::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB24_SWAPPED    ] = MMX_SYM(Dib)::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_565_NATIVE ] = MMX_SYM(Dib)::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_565_SWAPPED] = MMX_SYM(Dib)::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_555_NATIVE ] = MMX_SYM(Dib)::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_RGB16_555_SWAPPED] = MMX_SYM(Dib)::f8_from_null;
//f.dib.convert[IMAGE_FORMAT_A8    ][DIB_FORMAT_GREY8            ] = MMX_SYM(Dib)::f8_from_null;

//f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::bswap32;
//f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::prgb32_swapped_from_argb32;
//f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::frgb32_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::azzz32_swapped_from_a8;
//f.dib.convert[DIB_FORMAT_PRGB32_SWAPPED   ][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::prgb32_swapped_from_i8;

//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::argb32_swapped_from_prgb32;
//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::bswap32;
//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::frgb32_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::azzz32_swapped_from_a8;
//f.dib.convert[DIB_FORMAT_ARGB32_SWAPPED   ][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::argb32_swapped_from_i8;

//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::frgb32_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::frgb32_swapped_from_argb32;
//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::frgb32_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::fzzz32_swapped_from_null;
//f.dib.convert[DIB_FORMAT_XRGB32_SWAPPED   ][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::frgb32_swapped_from_i8;

//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb24_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb24_native_from_argb32;
//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb24_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::zzz24_from_null;
//f.dib.convert[DIB_FORMAT_RGB24_NATIVE     ][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::rgb24_native_from_i8;

//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb24_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb24_swapped_from_argb32;
//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb24_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::zzz24_from_null;
//f.dib.convert[DIB_FORMAT_RGB24_SWAPPED    ][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::rgb24_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb16_565_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb16_565_native_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb16_565_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::zzz16_from_null;
//f.dib.convert[DIB_FORMAT_RGB16_565_NATIVE ][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::rgb16_565_native_from_i8;

//f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb16_565_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb16_565_swapped_from_argb32;
//f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb16_565_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::zzz16_from_null;
//f.dib.convert[DIB_FORMAT_RGB16_565_SWAPPED][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::rgb16_565_swapped_from_i8;

  f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb16_555_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb16_555_native_from_argb32;
  f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb16_555_native_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::zzz16_from_null;
//f.dib.convert[DIB_FORMAT_RGB16_555_NATIVE ][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::rgb16_555_native_from_i8;

//f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::rgb16_555_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::rgb16_555_swapped_from_argb32;
//f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::rgb16_555_swapped_from_xrgb32;
//f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::zzz16_from_null;
//f.dib.convert[DIB_FORMAT_RGB16_555_SWAPPED][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::rgb16_555_swapped_from_i8;

//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_PRGB32] = MMX_SYM(Dib)::grey8_from_xrgb32;
//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_ARGB32] = MMX_SYM(Dib)::grey8_from_argb32;
//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_XRGB32] = MMX_SYM(Dib)::grey8_from_xrgb32;
//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_A8    ] = MMX_SYM(Dib)::z8_from_null;
//f.dib.convert[DIB_FORMAT_GREY8            ][IMAGE_FORMAT_I8    ] = MMX_SYM(Dib)::grey8_from_i8;

  // --------------------------------------------------------------------------
  // [Interpolate - Gradient]
  // --------------------------------------------------------------------------

  f.interpolate.gradient[IMAGE_FORMAT_PRGB32] = MMX_SYM(Interpolate)::gradient_prgb32;
  f.interpolate.gradient[IMAGE_FORMAT_ARGB32] = MMX_SYM(Interpolate)::gradient_argb32;
  f.interpolate.gradient[IMAGE_FORMAT_XRGB32] = MMX_SYM(Interpolate)::gradient_argb32;

  // --------------------------------------------------------------------------
  // [Filters - ColorMatrix]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTER_MMX3DNOW)
  f.filter.color_matrix[IMAGE_FORMAT_PRGB32] = MMX_SYM(Filter)::color_matrix_prgb32;
  f.filter.color_matrix[IMAGE_FORMAT_ARGB32] = MMX_SYM(Filter)::color_matrix_argb32;
  f.filter.color_matrix[IMAGE_FORMAT_XRGB32] = MMX_SYM(Filter)::color_matrix_xrgb32;
#endif // FOG_RASTER_MMX3DNOW

  // --------------------------------------------------------------------------
  // [Composite - Src]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_PRGB32] = MMX_SYM(CompositeSrc)::prgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_ARGB32] = MMX_SYM(CompositeSrc)::prgb32_cblit_full;
  f.composite[OPERATOR_SRC][IMAGE_FORMAT_PRGB32].cblit_full[IMAGE_FORMAT_XRGB32] = MMX_SYM(CompositeSrc)::prgb32_cblit_full;

  // --------------------------------------------------------------------------
  // [Composite - SrcOver]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = MMX_SYM(CompositeSrcOver)::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = MMX_SYM(CompositeSrcOver)::prgb32_vblit_prgb32_full;
}

#endif // FOG_ARCH_X86
