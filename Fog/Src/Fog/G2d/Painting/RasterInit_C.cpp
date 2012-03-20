// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>

#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterInit_p.h>

#include <Fog/G2d/Painting/RasterOps_C/BaseAccess_p.h>
#include <Fog/G2d/Painting/RasterOps_C/BaseConvert_p.h>
#include <Fog/G2d/Painting/RasterOps_C/BaseDefs_p.h>
#include <Fog/G2d/Painting/RasterOps_C/BaseHelpers_p.h>

#include <Fog/G2d/Painting/RasterOps_C/CompositeBase_p.h>
#include <Fog/G2d/Painting/RasterOps_C/CompositeClear_p.h>
#include <Fog/G2d/Painting/RasterOps_C/CompositeExt_p.h>
#include <Fog/G2d/Painting/RasterOps_C/CompositeNop_p.h>
#include <Fog/G2d/Painting/RasterOps_C/CompositeSrc_p.h>
#include <Fog/G2d/Painting/RasterOps_C/CompositeSrcOver_p.h>

#include <Fog/G2d/Painting/RasterOps_C/GradientBase_p.h>
#include <Fog/G2d/Painting/RasterOps_C/GradientConical_p.h>
#include <Fog/G2d/Painting/RasterOps_C/GradientLinear_p.h>
#include <Fog/G2d/Painting/RasterOps_C/GradientRadial_p.h>
#include <Fog/G2d/Painting/RasterOps_C/GradientRectangular_p.h>

#include <Fog/G2d/Painting/RasterOps_C/FilterBlur_p.h>
#include <Fog/G2d/Painting/RasterOps_C/FilterColorLut_p.h>
#include <Fog/G2d/Painting/RasterOps_C/FilterColorMatrix_p.h>
#include <Fog/G2d/Painting/RasterOps_C/FilterComponentTransfer_p.h>
#include <Fog/G2d/Painting/RasterOps_C/FilterConvolveMatrix_p.h>
#include <Fog/G2d/Painting/RasterOps_C/FilterConvolveSeparable_p.h>
#include <Fog/G2d/Painting/RasterOps_C/FilterMorphology_p.h>

#include <Fog/G2d/Painting/RasterOps_C/TextureBase_p.h>
#include <Fog/G2d/Painting/RasterOps_C/TextureAffine_p.h>
#include <Fog/G2d/Painting/RasterOps_C/TextureProjection_p.h>
#include <Fog/G2d/Painting/RasterOps_C/TextureScale_p.h>
#include <Fog/G2d/Painting/RasterOps_C/TextureSimple_p.h>

// C implementation can be disabled when hardcoding for SSE2.
#if !defined(FOG_HARDCODE_SSE2)
# define FOG_RASTER_INIT_C
#endif // FOG_HARDCODE_SSE2

// TODO: At the moment there is no SSE2 pipeline.
# define FOG_RASTER_INIT_C

namespace Fog {

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void RasterOps_init_C(void)
{
  ApiRaster& api = _api_raster;

#if defined(FOG_RASTER_INIT_C)

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - API]
  // --------------------------------------------------------------------------

  RasterConvertFuncs& convert = api.convert;

  convert.init = RasterOps_C::Convert::init;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - Copy]
  // --------------------------------------------------------------------------

  convert.copy[RASTER_COPY_8] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::copy_8;
  convert.copy[RASTER_COPY_16] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::copy_16;
  convert.copy[RASTER_COPY_24] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::copy_24;
  convert.copy[RASTER_COPY_32] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::copy_32;
  convert.copy[RASTER_COPY_48] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::copy_48;
  convert.copy[RASTER_COPY_64] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::copy_64;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - Fill]
  // --------------------------------------------------------------------------

  convert.fill[RASTER_FILL_8] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::fill_8;
  convert.fill[RASTER_FILL_16] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::fill_16;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - BSwap]
  // --------------------------------------------------------------------------

  convert.bswap[RASTER_BSWAP_16] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::bswap_16;
  convert.bswap[RASTER_BSWAP_24] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::bswap_24;
  convert.bswap[RASTER_BSWAP_32] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::bswap_32;
  convert.bswap[RASTER_BSWAP_48] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::bswap_48;
  convert.bswap[RASTER_BSWAP_64] = (ImageConverterBlitLineFunc)RasterOps_C::Convert::bswap_64;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - Premultiply / Demultiply]
  // --------------------------------------------------------------------------

  convert.prgb32_from_argb32 = (ImageConverterBlitLineFunc)RasterOps_C::Convert::prgb32_from_argb32;
  convert.argb32_from_prgb32 = (ImageConverterBlitLineFunc)RasterOps_C::Convert::argb32_from_prgb32;
  convert.prgb64_from_argb64 = (ImageConverterBlitLineFunc)RasterOps_C::Convert::prgb64_from_argb64;
  convert.argb64_from_prgb64 = (ImageConverterBlitLineFunc)RasterOps_C::Convert::argb64_from_prgb64;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - ARGB32]
  // --------------------------------------------------------------------------

  convert.argb32_from[RASTER_FORMAT_RGB16_555          ] = RasterOps_C::Convert::argb32_from_rgb16_555;
  convert.argb32_from[RASTER_FORMAT_RGB16_555_BS       ] = RasterOps_C::Convert::argb32_from_rgb16_555_bs;
  convert.argb32_from[RASTER_FORMAT_RGB16_565          ] = RasterOps_C::Convert::argb32_from_rgb16_565;
  convert.argb32_from[RASTER_FORMAT_RGB16_565_BS       ] = RasterOps_C::Convert::argb32_from_rgb16_565_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB16_4444        ] = RasterOps_C::Convert::argb32_from_argb16_4444;
  convert.argb32_from[RASTER_FORMAT_ARGB16_4444_BS     ] = RasterOps_C::Convert::argb32_from_argb16_4444_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB16_CUSTOM      ] = RasterOps_C::Convert::argb32_from_argb16_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB16_CUSTOM_BS   ] = RasterOps_C::Convert::argb32_from_argb16_custom_bs;
//convert.argb32_from[RASTER_FORMAT_RGB24_888          ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_RGB24_888_BS       ] = RasterOps_C::Convert::argb32_from_rgb24_888_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB24_CUSTOM      ] = RasterOps_C::Convert::argb32_from_argb24_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB24_CUSTOM_BS   ] = RasterOps_C::Convert::argb32_from_argb24_custom_bs;
//convert.argb32_from[RASTER_FORMAT_RGB32_888          ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_RGB32_888_BS       ] = RasterOps_C::Convert::argb32_from_rgb32_888_bs;
//convert.argb32_from[RASTER_FORMAT_ARGB32_8888        ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_ARGB32_8888_BS     ] = RasterOps_C::Convert::argb32_from_argb32_8888_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB32_CUSTOM      ] = RasterOps_C::Convert::argb32_from_argb32_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB32_CUSTOM_BS   ] = RasterOps_C::Convert::argb32_from_argb32_custom_bs;
//convert.argb32_from[RASTER_FORMAT_RGB48_161616       ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_RGB48_161616_BS    ] = RasterOps_C::Convert::argb32_from_rgb48_161616_bs;
  convert.argb32_from[RASTER_FORMAT_RGB48_CUSTOM       ] = RasterOps_C::Convert::argb32_from_rgb48_custom;
  convert.argb32_from[RASTER_FORMAT_RGB48_CUSTOM_BS    ] = RasterOps_C::Convert::argb32_from_rgb48_custom_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB48_CUSTOM      ] = RasterOps_C::Convert::argb32_from_argb48_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB48_CUSTOM_BS   ] = RasterOps_C::Convert::argb32_from_argb48_custom_bs;
//convert.argb32_from[RASTER_FORMAT_ARGB64_16161616    ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_ARGB64_16161616_BS ] = RasterOps_C::Convert::argb32_from_argb64_16161616_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB64_CUSTOM      ] = RasterOps_C::Convert::argb32_from_argb64_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB64_CUSTOM_BS   ] = RasterOps_C::Convert::argb32_from_argb64_custom_bs;
//convert.argb32_from[RASTER_FORMAT_I8                 ];

  convert.from_argb32[RASTER_FORMAT_RGB16_555          ] = RasterOps_C::Convert::rgb16_555_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB16_555_BS       ] = RasterOps_C::Convert::rgb16_555_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB16_565          ] = RasterOps_C::Convert::rgb16_565_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB16_565_BS       ] = RasterOps_C::Convert::rgb16_565_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB16_4444        ] = RasterOps_C::Convert::argb16_4444_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB16_4444_BS     ] = RasterOps_C::Convert::argb16_4444_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB16_CUSTOM      ] = RasterOps_C::Convert::argb16_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB16_CUSTOM_BS   ] = RasterOps_C::Convert::argb16_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_RGB24_888          ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_RGB24_888_BS       ] = RasterOps_C::Convert::rgb24_888_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB24_CUSTOM      ] = RasterOps_C::Convert::argb24_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB24_CUSTOM_BS   ] = RasterOps_C::Convert::argb24_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_RGB32_888          ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_RGB32_888_BS       ] = RasterOps_C::Convert::rgb32_888_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_ARGB32_8888        ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_ARGB32_8888_BS     ] = RasterOps_C::Convert::argb32_8888_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB32_CUSTOM      ] = RasterOps_C::Convert::argb32_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB32_CUSTOM_BS   ] = RasterOps_C::Convert::argb32_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_RGB48_161616       ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_RGB48_161616_BS    ] = RasterOps_C::Convert::rgb48_161616_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB48_CUSTOM       ] = RasterOps_C::Convert::rgb48_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB48_CUSTOM_BS    ] = RasterOps_C::Convert::rgb48_custom_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB48_CUSTOM      ] = RasterOps_C::Convert::argb48_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB48_CUSTOM_BS   ] = RasterOps_C::Convert::argb48_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_ARGB64_16161616    ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_ARGB64_16161616_BS ] = RasterOps_C::Convert::argb64_16161616_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB64_CUSTOM      ] = RasterOps_C::Convert::argb64_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB64_CUSTOM_BS   ] = RasterOps_C::Convert::argb64_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_I8                 ];

  // --------------------------------------------------------------------------
  //[RasterOps - Convert - ARGB64]
  // --------------------------------------------------------------------------

  convert.argb64_from[RASTER_FORMAT_RGB16_555          ] = RasterOps_C::Convert::argb64_from_rgb16_555;
  convert.argb64_from[RASTER_FORMAT_RGB16_555_BS       ] = RasterOps_C::Convert::argb64_from_rgb16_555_bs;
  convert.argb64_from[RASTER_FORMAT_RGB16_565          ] = RasterOps_C::Convert::argb64_from_rgb16_565;
  convert.argb64_from[RASTER_FORMAT_RGB16_565_BS       ] = RasterOps_C::Convert::argb64_from_rgb16_565_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB16_4444        ] = RasterOps_C::Convert::argb64_from_argb16_4444;
  convert.argb64_from[RASTER_FORMAT_ARGB16_4444_BS     ] = RasterOps_C::Convert::argb64_from_argb16_4444_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB16_CUSTOM      ] = RasterOps_C::Convert::argb64_from_argb16_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB16_CUSTOM_BS   ] = RasterOps_C::Convert::argb64_from_argb16_custom_bs;
//convert.argb64_from[RASTER_FORMAT_RGB24_888          ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_RGB24_888_BS       ] = RasterOps_C::Convert::argb64_from_rgb24_888_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB24_CUSTOM      ] = RasterOps_C::Convert::argb64_from_argb24_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB24_CUSTOM_BS   ] = RasterOps_C::Convert::argb64_from_argb24_custom_bs;
//convert.argb64_from[RASTER_FORMAT_RGB32_888          ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_RGB32_888_BS       ] = RasterOps_C::Convert::argb64_from_rgb32_888_bs;
//convert.argb64_from[RASTER_FORMAT_ARGB32_8888        ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_ARGB32_8888_BS     ] = RasterOps_C::Convert::argb64_from_argb32_8888_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB32_CUSTOM      ] = RasterOps_C::Convert::argb64_from_argb32_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB32_CUSTOM_BS   ] = RasterOps_C::Convert::argb64_from_argb32_custom_bs;
//convert.argb64_from[RASTER_FORMAT_RGB48_161616       ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_RGB48_161616_BS    ] = RasterOps_C::Convert::argb64_from_rgb48_161616_bs;
  convert.argb64_from[RASTER_FORMAT_RGB48_CUSTOM       ] = RasterOps_C::Convert::argb64_from_rgb48_custom;
  convert.argb64_from[RASTER_FORMAT_RGB48_CUSTOM_BS    ] = RasterOps_C::Convert::argb64_from_rgb48_custom_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB48_CUSTOM      ] = RasterOps_C::Convert::argb64_from_argb48_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB48_CUSTOM_BS   ] = RasterOps_C::Convert::argb64_from_argb48_custom_bs;
//convert.argb64_from[RASTER_FORMAT_ARGB64_16161616    ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_ARGB64_16161616_BS ] = RasterOps_C::Convert::argb64_from_argb64_16161616_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB64_CUSTOM      ] = RasterOps_C::Convert::argb64_from_argb64_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB64_CUSTOM_BS   ] = RasterOps_C::Convert::argb64_from_argb64_custom_bs;
//convert.argb64_from[RASTER_FORMAT_I8                 ];

  // TODO: Image conversion.
  /*
  convert.from_argb64[RASTER_FORMAT_RGB16_555          ] = RasterOps_C::Convert::rgb16_555_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB16_555_BS       ] = RasterOps_C::Convert::rgb16_555_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB16_565          ] = RasterOps_C::Convert::rgb16_565_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB16_565_BS       ] = RasterOps_C::Convert::rgb16_565_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB16_4444        ] = RasterOps_C::Convert::argb16_4444_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB16_4444_BS     ] = RasterOps_C::Convert::argb16_4444_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB16_CUSTOM      ] = RasterOps_C::Convert::argb16_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB16_CUSTOM_BS   ] = RasterOps_C::Convert::argb16_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_RGB24_888          ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_RGB24_888_BS       ] = RasterOps_C::Convert::rgb24_888_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB24_CUSTOM      ] = RasterOps_C::Convert::argb24_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB24_CUSTOM_BS   ] = RasterOps_C::Convert::argb24_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_RGB32_888          ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_RGB32_888_BS       ] = RasterOps_C::Convert::rgb32_888_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_ARGB32_8888        ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_ARGB32_8888_BS     ] = RasterOps_C::Convert::argb32_8888_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB32_CUSTOM      ] = RasterOps_C::Convert::argb32_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB32_CUSTOM_BS   ] = RasterOps_C::Convert::argb32_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_RGB48_161616       ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_RGB48_161616_BS    ] = RasterOps_C::Convert::rgb48_161616_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB48_CUSTOM       ] = RasterOps_C::Convert::rgb48_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB48_CUSTOM_BS    ] = RasterOps_C::Convert::rgb48_custom_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB48_CUSTOM      ] = RasterOps_C::Convert::argb48_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB48_CUSTOM_BS   ] = RasterOps_C::Convert::argb48_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_ARGB64_16161616    ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_ARGB64_16161616_BS ] = RasterOps_C::Convert::argb64_16161616_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB64_CUSTOM      ] = RasterOps_C::Convert::argb64_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB64_CUSTOM_BS   ] = RasterOps_C::Convert::argb64_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_I8                 ];
  */

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrc::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrc::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_C::Convert::copy_32);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_XRGB32   ], RasterOps_C::CompositeSrc::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB24    ], RasterOps_C::CompositeSrc::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::prgb32_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrc::prgb32_vblit_i8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::comp8_vblit_prgb64_line    <RasterOps_C::AccessPRGB32>);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB48    ], RasterOps_C::CompositeSrc::comp8_vblit_rgb48_line     <RasterOps_C::AccessPRGB32>);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::prgb32_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrc::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_XRGB32   ], RasterOps_C::CompositeSrc::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB24    ], RasterOps_C::CompositeSrc::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::prgb32_vblit_a8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrc::prgb32_vblit_i8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::comp8_vblit_prgb64_span    <RasterOps_C::AccessPRGB32>);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB48    ], RasterOps_C::CompositeSrc::comp8_vblit_rgb48_span     <RasterOps_C::AccessPRGB32>);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::prgb32_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_CORE_SRC];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrc::xrgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrc::xrgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrc::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_XRGB32   ], RasterOps_C::Convert::copy_32);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB24    ], RasterOps_C::CompositeSrc::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::prgb32_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrc::prgb32_vblit_i8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::comp8_vblit_prgb64_line    <RasterOps_C::AccessXRGB32>);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB48    ], RasterOps_C::CompositeSrc::comp8_vblit_rgb48_line     <RasterOps_C::AccessXRGB32>);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::prgb32_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrc::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_XRGB32   ], RasterOps_C::CompositeSrc::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB24    ], RasterOps_C::CompositeSrc::xrgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::xrgb32_vblit_a8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrc::xrgb32_vblit_i8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::comp8_vblit_prgb64_span    <RasterOps_C::AccessXRGB32>);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB48    ], RasterOps_C::CompositeSrc::comp8_vblit_rgb48_span     <RasterOps_C::AccessXRGB32>);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::xrgb32_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src - RGB24]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_RGB24][RASTER_COMPOSITE_CORE_SRC];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrc::rgb24_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrc::rgb24_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrc::rgb24_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_XRGB32   ], RasterOps_C::CompositeSrc::rgb24_vblit_xrgb32_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB24    ]);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::comp8_vblit_a8_line        <RasterOps_C::AccessRGB24>);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrc::comp8_vblit_i8_line        <RasterOps_C::AccessRGB24>);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::comp8_vblit_prgb64_line    <RasterOps_C::AccessRGB24>);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB48    ], RasterOps_C::CompositeSrc::comp8_vblit_rgb48_line     <RasterOps_C::AccessRGB24>);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::comp8_vblit_a16_line       <RasterOps_C::AccessRGB24>);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrc::rgb24_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_XRGB32   ], RasterOps_C::CompositeSrc::rgb24_vblit_xrgb32_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB24    ], RasterOps_C::CompositeSrc::rgb24_vblit_rgb24_span); // TODO
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::comp8_vblit_a8_span        <RasterOps_C::AccessRGB24>);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrc::comp8_vblit_i8_span        <RasterOps_C::AccessRGB24>);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::comp8_vblit_prgb64_span    <RasterOps_C::AccessRGB24>);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB48    ], RasterOps_C::CompositeSrc::comp8_vblit_rgb48_span     <RasterOps_C::AccessRGB24>);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::comp8_vblit_a16_span       <RasterOps_C::AccessRGB24>);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src - A8]
  // --------------------------------------------------------------------------

  // TODO: Image compositing.

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOver - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC_OVER];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrcOver::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrcOver::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrcOver::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB24    ]);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::prgb32_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrcOver::prgb32_vblit_i8_line);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::prgb32_vblit_prgb64_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB48    ]);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::prgb32_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrcOver::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB24    ]);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::prgb32_vblit_a8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrc::prgb32_vblit_i8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::prgb32_vblit_prgb64_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB48    ]);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::prgb32_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOver - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_CORE_SRC_OVER];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrcOver::xrgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrcOver::xrgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrcOver::xrgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB24    ]);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::prgb32_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrcOver::prgb32_vblit_i8_line);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::prgb32_vblit_prgb64_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB48    ]);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::prgb32_vblit_a16_line);

  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrcOver::xrgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB24    ]);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::prgb32_vblit_a8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrc::prgb32_vblit_i8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::prgb32_vblit_prgb64_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB48    ]);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::prgb32_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOver - RGB24]
  // --------------------------------------------------------------------------

  // TODO: Image compositing.
  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_CORE_SRC_OVER];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrcOver::rgb24_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_C::CompositeSrcOver::rgb24_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrcOver::rgb24_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB24    ]);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::prgb32_vblit_a8_line);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrcOver::rgb24_vblit_i8_line);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::prgb32_vblit_prgb64_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB48    ]);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::prgb32_vblit_a16_line);

  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_C::CompositeSrcOver::rgb24_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB24    ]);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_C::CompositeSrc::prgb32_vblit_a8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_C::CompositeSrc::prgb32_vblit_i8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_C::CompositeSrc::prgb32_vblit_prgb64_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB48    ]);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_C::CompositeSrc::prgb32_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOver - A8]
  // --------------------------------------------------------------------------

  // TODO: Image compositing.

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Clear - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_CLEAR];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], (RasterCBlitLineFunc)RasterOps_C::CompositeClear::prgb32_xblit_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], (RasterCBlitSpanFunc)RasterOps_C::CompositeClear::prgb32_cblit_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], (RasterVBlitLineFunc)RasterOps_C::CompositeClear::prgb32_xblit_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], (RasterVBlitSpanFunc)RasterOps_C::CompositeClear::prgb32_vblit_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Clear - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_CLEAR];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], (RasterCBlitLineFunc)RasterOps_C::CompositeClear::xrgb32_xblit_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], (RasterCBlitSpanFunc)RasterOps_C::CompositeClear::xrgb32_cblit_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], (RasterVBlitLineFunc)RasterOps_C::CompositeClear::xrgb32_xblit_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], (RasterVBlitSpanFunc)RasterOps_C::CompositeClear::xrgb32_vblit_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Clear - RGB24]
  // --------------------------------------------------------------------------
  
  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_RGB24][RASTER_COMPOSITE_EXT_CLEAR];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], (RasterCBlitLineFunc)RasterOps_C::CompositeClear::rgb24_xblit_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], (RasterCBlitSpanFunc)RasterOps_C::CompositeClear::rgb24_cblit_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_RGB24_AND_PRGB32 ], (RasterVBlitLineFunc)RasterOps_C::CompositeClear::rgb24_xblit_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_RGB24_AND_XRGB32 ]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_RGB24_AND_RGB24  ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_RGB24_AND_PRGB32 ], (RasterVBlitSpanFunc)RasterOps_C::CompositeClear::rgb24_vblit_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_RGB24_AND_XRGB32 ]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_RGB24_AND_RGB24  ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Clear - A8]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_A8][RASTER_COMPOSITE_EXT_CLEAR];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], (RasterCBlitLineFunc)RasterOps_C::CompositeClear::a8_xblit_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], (RasterCBlitSpanFunc)RasterOps_C::CompositeClear::a8_cblit_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_A8_AND_PRGB32    ], (RasterVBlitLineFunc)RasterOps_C::CompositeClear::a8_xblit_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_A8_AND_A8        ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_A8_AND_PRGB32    ], (RasterVBlitSpanFunc)RasterOps_C::CompositeClear::a8_vblit_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_A8_AND_A8        ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcIn - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_SRC_IN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSrcIn::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeSrcIn::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSrcIn::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeSrcIn::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeSrcIn::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeSrcIn::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeSrcIn::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeSrcIn::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeSrcIn::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeSrcIn::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeSrcIn::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeSrcIn::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcIn - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_SRC_IN];

    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOut - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_SRC_OUT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSrcOut::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeSrcOut::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSrcOut::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeSrcOut::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeSrcOut::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeSrcOut::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeSrcOut::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeSrcOut::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeSrcOut::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeSrcOut::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeSrcOut::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeSrcOut::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOut - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_SRC_OUT];

    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcAtop - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_SRC_ATOP];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSrcAtop::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSrcAtop::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeSrcAtop::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeSrcAtop::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeSrcAtop::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeSrcAtop::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcAtop - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_SRC_ATOP];

    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstOver - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DST_OVER];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstOver::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDstOver::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstOver::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDstOver::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDstAtop::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeDstAtop::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeDstAtop::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDstAtop::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDstAtop::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeDstAtop::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeDstAtop::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDstAtop::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstOver - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DST_OVER];

    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstIn - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DST_IN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstIn::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstIn::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDstIn::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDstIn::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDstIn::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDstIn::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstIn - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DST_IN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstIn::xrgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstIn::xrgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeDstIn::xrgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeDstIn::xrgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstOut - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DST_OUT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstOut::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstOut::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDstOut::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDstOut::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDstOut::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDstOut::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstOut - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DST_OUT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstOut::xrgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstOut::xrgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeDstOut::xrgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeDstOut::xrgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstAtop - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DST_ATOP];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstAtop::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDstAtop::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDstAtop::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDstAtop::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDstAtop::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDstAtop::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstAtop - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DST_ATOP];

    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Xor - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_XOR];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeXor::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeXor::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeXor::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeXor::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeXor::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeXor::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Xor - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_XOR];

    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_PRGB             ]);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Plus - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_PLUS];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositePlus::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositePlus::prgb32_cblit_prgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositePlus::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositePlus::prgb32_cblit_prgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositePlus::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositePlus::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositePlus::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositePlus::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositePlus::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositePlus::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositePlus::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositePlus::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Plus - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_PLUS];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositePlus::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositePlus::xrgb32_cblit_prgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositePlus::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositePlus::xrgb32_cblit_prgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositePlus::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositePlus::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositePlus::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositePlus::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositePlus::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositePlus::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Minus - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_MINUS];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeMinus::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeMinus::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeMinus::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeMinus::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeMinus::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeMinus::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeMinus::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeMinus::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeMinus::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeMinus::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeMinus::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeMinus::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Minus - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_MINUS];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeMinus::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeMinus::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeMinus::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeMinus::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeMinus::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeMinus::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeMinus::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeMinus::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeMinus::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeMinus::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Multiply - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_MULTIPLY];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeMultiply::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeMultiply::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeMultiply::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeMultiply::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeMultiply::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeMultiply::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeMultiply::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeMultiply::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeMultiply::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeMultiply::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeMultiply::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeMultiply::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Multiply - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_MULTIPLY];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeMultiply::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeMultiply::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeMultiply::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeMultiply::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeMultiply::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeMultiply::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeMultiply::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeMultiply::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeMultiply::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeMultiply::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Screen - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_SCREEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeScreen::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeScreen::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeScreen::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeScreen::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeScreen::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeScreen::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeScreen::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeScreen::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeScreen::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeScreen::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeScreen::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeScreen::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Screen - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_SCREEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeScreen::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeScreen::xrgb32_cblit_prgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeScreen::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeScreen::xrgb32_cblit_prgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeScreen::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeScreen::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeScreen::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeScreen::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeScreen::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeScreen::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Overlay - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_OVERLAY];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeOverlay::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeOverlay::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeOverlay::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeOverlay::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeOverlay::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeOverlay::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeOverlay::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeOverlay::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeOverlay::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeOverlay::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeOverlay::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeOverlay::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Overlay - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_OVERLAY];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeOverlay::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeOverlay::xrgb32_cblit_prgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeOverlay::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeOverlay::xrgb32_cblit_prgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeOverlay::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeOverlay::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeOverlay::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeOverlay::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeOverlay::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeOverlay::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Darken - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DARKEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDarken::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDarken::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDarken::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDarken::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDarken::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeDarken::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeDarken::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDarken::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDarken::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeDarken::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeDarken::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDarken::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Darken - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DARKEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDarken::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDarken::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDarken::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDarken::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeDarken::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeDarken::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeDarken::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeDarken::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeDarken::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeDarken::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Lighten - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_LIGHTEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeLighten::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeLighten::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeLighten::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeLighten::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeLighten::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeLighten::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeLighten::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeLighten::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeLighten::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeLighten::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeLighten::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeLighten::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Lighten - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_LIGHTEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeLighten::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeLighten::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeLighten::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeLighten::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeLighten::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeLighten::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeLighten::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeLighten::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeLighten::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeLighten::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - ColorDodge - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_COLOR_DODGE];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeColorDodge::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeColorDodge::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeColorDodge::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeColorDodge::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeColorDodge::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeColorDodge::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeColorDodge::prgb32_vblit_rgb24_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeColorDodge::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeColorDodge::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeColorDodge::prgb32_vblit_rgb24_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - ColorDodge - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_COLOR_DODGE];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeColorDodge::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeColorDodge::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeColorDodge::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeColorDodge::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeColorDodge::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeColorDodge::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeColorDodge::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeColorDodge::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeColorDodge::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeColorDodge::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - ColorBurn - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_COLOR_BURN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeColorBurn::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeColorBurn::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeColorBurn::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeColorBurn::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeColorBurn::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeColorBurn::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeColorBurn::prgb32_vblit_rgb24_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeColorBurn::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeColorBurn::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeColorBurn::prgb32_vblit_rgb24_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - ColorBurn - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_COLOR_BURN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeColorBurn::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeColorBurn::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeColorBurn::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeColorBurn::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeColorBurn::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeColorBurn::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeColorBurn::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeColorBurn::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeColorBurn::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeColorBurn::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - HardLight - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_HARD_LIGHT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeHardLight::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeHardLight::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeHardLight::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeHardLight::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeHardLight::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeHardLight::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeHardLight::prgb32_vblit_rgb24_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeHardLight::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeHardLight::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeHardLight::prgb32_vblit_rgb24_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - HardLight - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_HARD_LIGHT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeHardLight::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeHardLight::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeHardLight::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeHardLight::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeHardLight::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeHardLight::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeHardLight::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeHardLight::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeHardLight::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeHardLight::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SoftLight - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_SOFT_LIGHT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSoftLight::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeSoftLight::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSoftLight::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeSoftLight::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeSoftLight::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeSoftLight::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeSoftLight::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeSoftLight::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeSoftLight::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeSoftLight::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeSoftLight::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeSoftLight::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SoftLight - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_SOFT_LIGHT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSoftLight::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeSoftLight::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeSoftLight::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeSoftLight::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeSoftLight::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeSoftLight::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeSoftLight::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeSoftLight::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeSoftLight::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeSoftLight::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Difference - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DIFFERENCE];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDifference::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDifference::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDifference::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDifference::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDifference::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeDifference::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeDifference::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDifference::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeDifference::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeDifference::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeDifference::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_C::CompositeDifference::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Difference - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DIFFERENCE];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDifference::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDifference::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeDifference::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeDifference::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeDifference::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeDifference::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeDifference::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeDifference::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeDifference::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeDifference::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Exclusion - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_EXCLUSION];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeExclusion::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeExclusion::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeExclusion::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeExclusion::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeExclusion::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeExclusion::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeExclusion::prgb32_vblit_rgb24_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_C::CompositeExclusion::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_C::CompositeExclusion::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_C::CompositeExclusion::prgb32_vblit_rgb24_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Exclusion - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_EXCLUSION];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeExclusion::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeExclusion::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_C::CompositeExclusion::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_C::CompositeExclusion::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeExclusion::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeExclusion::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeExclusion::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_C::CompositeExclusion::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_C::CompositeExclusion::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_C::CompositeExclusion::xrgb32_vblit_rgb24_span);
  }

#endif // FOG_RASTER_INIT_C

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Solid]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTER_INIT_C)
  {
    RasterSolidFuncs& solid = api.solid;

    solid.create = RasterOps_C::Helpers::p_solid_create_solid;
    solid.destroy = RasterOps_C::Helpers::p_solid_destroy;
    solid.prepare = RasterOps_C::Helpers::p_solid_prepare;

    solid.fetch[IMAGE_FORMAT_PRGB32] = RasterOps_C::Helpers::p_solid_fetch_prgb32_xrgb32;
    solid.fetch[IMAGE_FORMAT_XRGB32] = RasterOps_C::Helpers::p_solid_fetch_prgb32_xrgb32;
    solid.fetch[IMAGE_FORMAT_A8    ] = RasterOps_C::Helpers::p_solid_fetch_a8;

    solid.fetch[IMAGE_FORMAT_PRGB64] = RasterOps_C::Helpers::p_solid_fetch_prgb64;
    solid.fetch[IMAGE_FORMAT_RGB48 ] = RasterOps_C::Helpers::p_solid_fetch_rgb48;
    solid.fetch[IMAGE_FORMAT_A16   ] = RasterOps_C::Helpers::p_solid_fetch_a16;
  }
#endif // FOG_RASTER_INIT_C

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Gradient - API]
  // --------------------------------------------------------------------------

  RasterGradientFuncs& gradient = api.gradient;

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Gradient - Interpolate]
  // --------------------------------------------------------------------------

#if defined(FOG_RASTER_INIT_C)
  gradient.interpolate[IMAGE_FORMAT_PRGB32] = RasterOps_C::PGradientBase::interpolate_prgb32;
  gradient.interpolate[IMAGE_FORMAT_XRGB32] = RasterOps_C::PGradientBase::interpolate_prgb32;
#endif // FOG_RASTER_INIT_C

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Gradient - Linear]
  // --------------------------------------------------------------------------

  gradient.create[GRADIENT_TYPE_LINEAR] = RasterOps_C::PGradientLinear::create;

#if defined(FOG_RASTER_INIT_C)
  gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientLinear::fetch_simple_nearest_pad<RasterOps_C::PGradientAccessor_PRGB32_Base>;
  gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientLinear::fetch_simple_nearest_pad<RasterOps_C::PGradientAccessor_PRGB32_Base>;
  gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientLinear::fetch_simple_nearest_pad<RasterOps_C::PGradientAccessor_A8_Base>;

  gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientLinear::fetch_simple_nearest_repeat<RasterOps_C::PGradientAccessor_PRGB32_Base>;
  gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientLinear::fetch_simple_nearest_repeat<RasterOps_C::PGradientAccessor_PRGB32_Base>;
  gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientLinear::fetch_simple_nearest_repeat<RasterOps_C::PGradientAccessor_A8_Base>;

  gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientLinear::fetch_simple_nearest_reflect<RasterOps_C::PGradientAccessor_PRGB32_Base>;
  gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientLinear::fetch_simple_nearest_reflect<RasterOps_C::PGradientAccessor_PRGB32_Base>;
  gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientLinear::fetch_simple_nearest_reflect<RasterOps_C::PGradientAccessor_A8_Base>;

  gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientLinear::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientLinear::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientLinear::fetch_proj_nearest<RasterOps_C::PGradientAccessor_A8_Pad>;

  gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientLinear::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientLinear::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientLinear::fetch_proj_nearest<RasterOps_C::PGradientAccessor_A8_Repeat>;

  gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientLinear::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientLinear::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientLinear::fetch_proj_nearest<RasterOps_C::PGradientAccessor_A8_Reflect>;
#endif // FOG_RASTER_INIT_C

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Gradient - Radial]
  // --------------------------------------------------------------------------

  gradient.create[GRADIENT_TYPE_RADIAL] = RasterOps_C::PGradientRadial::create;

#if defined(FOG_RASTER_INIT_C)
  gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRadial::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRadial::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRadial::fetch_simple_nearest<RasterOps_C::PGradientAccessor_A8_Pad>;

  gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRadial::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRadial::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRadial::fetch_simple_nearest<RasterOps_C::PGradientAccessor_A8_Repeat>;

  gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRadial::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRadial::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRadial::fetch_simple_nearest<RasterOps_C::PGradientAccessor_A8_Reflect>;

  gradient.radial.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRadial::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.radial.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRadial::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.radial.fetch_proj_nearest  [IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRadial::fetch_proj_nearest<RasterOps_C::PGradientAccessor_A8_Pad>;

  gradient.radial.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRadial::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.radial.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRadial::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.radial.fetch_proj_nearest  [IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRadial::fetch_proj_nearest<RasterOps_C::PGradientAccessor_A8_Repeat>;

  gradient.radial.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRadial::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.radial.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRadial::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.radial.fetch_proj_nearest  [IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRadial::fetch_proj_nearest<RasterOps_C::PGradientAccessor_A8_Reflect>;
#endif // FOG_RASTER_INIT_C

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Gradient - Conical]
  // --------------------------------------------------------------------------

  gradient.create[GRADIENT_TYPE_CONICAL] = RasterOps_C::PGradientConical::create;

#if defined(FOG_RASTER_INIT_C)
  gradient.conical.fetch_simple_nearest[IMAGE_FORMAT_PRGB32] = RasterOps_C::PGradientConical::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Base>;
  gradient.conical.fetch_simple_nearest[IMAGE_FORMAT_XRGB32] = RasterOps_C::PGradientConical::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Base>;
  gradient.conical.fetch_simple_nearest[IMAGE_FORMAT_A8    ] = RasterOps_C::PGradientConical::fetch_simple_nearest<RasterOps_C::PGradientAccessor_A8_Base>;
#endif // FOG_RASTER_INIT_C

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Gradient - Rectangular]
  // --------------------------------------------------------------------------

  gradient.create[GRADIENT_TYPE_RECTANGULAR] = RasterOps_C::PGradientRectangular::create;

#if defined(FOG_RASTER_INIT_C)
  gradient.rectangular.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRectangular::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.rectangular.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRectangular::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.rectangular.fetch_simple_nearest[IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRectangular::fetch_simple_nearest<RasterOps_C::PGradientAccessor_A8_Pad>;

  gradient.rectangular.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRectangular::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.rectangular.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRectangular::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.rectangular.fetch_simple_nearest[IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRectangular::fetch_simple_nearest<RasterOps_C::PGradientAccessor_A8_Repeat>;

  gradient.rectangular.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRectangular::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.rectangular.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRectangular::fetch_simple_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.rectangular.fetch_simple_nearest[IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRectangular::fetch_simple_nearest<RasterOps_C::PGradientAccessor_A8_Reflect>;

  gradient.rectangular.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRectangular::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.rectangular.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRectangular::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Pad>;
  gradient.rectangular.fetch_proj_nearest  [IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_PAD    ] = RasterOps_C::PGradientRectangular::fetch_proj_nearest<RasterOps_C::PGradientAccessor_A8_Pad>;

  gradient.rectangular.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRectangular::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.rectangular.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRectangular::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Repeat>;
  gradient.rectangular.fetch_proj_nearest  [IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REPEAT ] = RasterOps_C::PGradientRectangular::fetch_proj_nearest<RasterOps_C::PGradientAccessor_A8_Repeat>;

  gradient.rectangular.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRectangular::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.rectangular.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRectangular::fetch_proj_nearest<RasterOps_C::PGradientAccessor_PRGB32_Reflect>;
  gradient.rectangular.fetch_proj_nearest  [IMAGE_FORMAT_A8    ][GRADIENT_SPREAD_REFLECT] = RasterOps_C::PGradientRectangular::fetch_proj_nearest<RasterOps_C::PGradientAccessor_A8_Reflect>;
#endif // FOG_RASTER_INIT_C

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Texture - API]
  // --------------------------------------------------------------------------

  RasterTextureFuncs& texture = api.texture;

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Texture - Simple]
  // --------------------------------------------------------------------------

  texture.create = RasterOps_C::PTextureBase::create;

#if defined(FOG_RASTER_INIT_C)
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_align_pad<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_align_pad<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_align_pad<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_align_pad<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_align_pad<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subx0_pad<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subx0_pad<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subx0_pad<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subx0_pad<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subx0_pad<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_sub0y_pad<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_sub0y_pad<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_sub0y_pad<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_sub0y_pad<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_sub0y_pad<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subxy_pad<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subxy_pad<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subxy_pad<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subxy_pad<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureSimple::fetch_subxy_pad<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_align_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_align_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_align_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_align_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_align_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subx0_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subx0_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subx0_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subx0_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subx0_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_sub0y_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_sub0y_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_sub0y_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_sub0y_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_sub0y_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subxy_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subxy_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subxy_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subxy_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureSimple::fetch_subxy_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_align_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_align_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_align_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_align_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_align_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subx0_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subx0_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subx0_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subx0_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subx0_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_sub0y_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_sub0y_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_sub0y_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_sub0y_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_sub0y_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subxy_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subxy_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subxy_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subxy_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureSimple::fetch_subxy_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_align_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_align_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_align_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_align_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_align[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_align_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subx0_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subx0_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subx0_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subx0_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subx0_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_sub0y_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_sub0y_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_sub0y_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_sub0y_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_sub0y_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subxy_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subxy_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subxy_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subxy_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureSimple::fetch_subxy_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Texture - Affine]
  // --------------------------------------------------------------------------

  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_pad<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_pad<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_pad<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_pad<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_pad<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_pad<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_pad<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_pad<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_pad<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_pad<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_repeat<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_nearest_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_nearest_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_nearest_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_nearest_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_nearest_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_reflect<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_nearest_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_PRGB32>;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_XRGB32>;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_RGB24 >;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_A8    >;
  texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = RasterOps_C::PTextureAffine::fetch_affine_bilinear_clamp<RasterOps_C::PTextureAccessor_PRGB32_From_I8    >;

  // --------------------------------------------------------------------------
  // [RasterOps - Pattern - Texture - Projection]
  // --------------------------------------------------------------------------

  // TODO:

#endif // FOG_RASTER_INIT_C

  // --------------------------------------------------------------------------
  // [RasterOps - Filter - API]
  // --------------------------------------------------------------------------

  RasterFilterFuncs& filter = api.filter;

  // --------------------------------------------------------------------------
  // [RasterOps - Filter - Blur]
  // --------------------------------------------------------------------------

  filter.create[FE_TYPE_BLUR] = RasterOps_C::FBlur::create;

  filter.blur.box.h[IMAGE_FORMAT_PRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doBoxH<RasterOps_C::FBlurBoxAccessor_PRGB32>;
  filter.blur.box.h[IMAGE_FORMAT_XRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doBoxH<RasterOps_C::FBlurBoxAccessor_XRGB32>;
  filter.blur.box.h[IMAGE_FORMAT_RGB24 ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doBoxH<RasterOps_C::FBlurBoxAccessor_RGB24 >;
  filter.blur.box.h[IMAGE_FORMAT_A8    ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doBoxH<RasterOps_C::FBlurBoxAccessor_A8    >;

  filter.blur.box.v[IMAGE_FORMAT_PRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doBoxV<RasterOps_C::FBlurBoxAccessor_PRGB32>;
  filter.blur.box.v[IMAGE_FORMAT_XRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doBoxV<RasterOps_C::FBlurBoxAccessor_XRGB32>;
  filter.blur.box.v[IMAGE_FORMAT_RGB24 ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doBoxV<RasterOps_C::FBlurBoxAccessor_RGB24 >;
  filter.blur.box.v[IMAGE_FORMAT_A8    ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doBoxV<RasterOps_C::FBlurBoxAccessor_A8    >;

  filter.blur.stack.h[IMAGE_FORMAT_PRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doStackH<RasterOps_C::FBlurStackAccessor_PRGB32>;
  filter.blur.stack.h[IMAGE_FORMAT_XRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doStackH<RasterOps_C::FBlurStackAccessor_XRGB32>;
  filter.blur.stack.h[IMAGE_FORMAT_RGB24 ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doStackH<RasterOps_C::FBlurStackAccessor_RGB24 >;
  filter.blur.stack.h[IMAGE_FORMAT_A8    ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doStackH<RasterOps_C::FBlurStackAccessor_A8    >;

  filter.blur.stack.v[IMAGE_FORMAT_PRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doStackV<RasterOps_C::FBlurStackAccessor_PRGB32>;
  filter.blur.stack.v[IMAGE_FORMAT_XRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doStackV<RasterOps_C::FBlurStackAccessor_XRGB32>;
  filter.blur.stack.v[IMAGE_FORMAT_RGB24 ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doStackV<RasterOps_C::FBlurStackAccessor_RGB24 >;
  filter.blur.stack.v[IMAGE_FORMAT_A8    ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doStackV<RasterOps_C::FBlurStackAccessor_A8    >;

  filter.blur.exponential.h[IMAGE_FORMAT_PRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doExpH<RasterOps_C::FBlurExpAccessor_PRGB32>;
  filter.blur.exponential.h[IMAGE_FORMAT_XRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doExpH<RasterOps_C::FBlurExpAccessor_XRGB32>;
  filter.blur.exponential.h[IMAGE_FORMAT_RGB24 ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doExpH<RasterOps_C::FBlurExpAccessor_RGB24 >;
  filter.blur.exponential.h[IMAGE_FORMAT_A8    ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doExpH<RasterOps_C::FBlurExpAccessor_A8    >;

  filter.blur.exponential.v[IMAGE_FORMAT_PRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doExpV<RasterOps_C::FBlurExpAccessor_PRGB32>;
  filter.blur.exponential.v[IMAGE_FORMAT_XRGB32] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doExpV<RasterOps_C::FBlurExpAccessor_XRGB32>;
  filter.blur.exponential.v[IMAGE_FORMAT_RGB24 ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doExpV<RasterOps_C::FBlurExpAccessor_RGB24 >;
  filter.blur.exponential.v[IMAGE_FORMAT_A8    ] = (RasterFilterDoBlurFunc)RasterOps_C::FBlur::doExpV<RasterOps_C::FBlurExpAccessor_A8    >;
}

} // Fog namespace
