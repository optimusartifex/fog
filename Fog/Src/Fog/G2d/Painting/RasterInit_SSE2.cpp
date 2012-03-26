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

#include <Fog/G2d/Painting/RasterOps_SSE2/BaseAccess_p.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/BaseConvert_p.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/BaseDefs_p.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/BaseHelpers_p.h>

#include <Fog/G2d/Painting/RasterOps_SSE2/CompositeBase_p.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/CompositeClear_p.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/CompositeExt_p.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/CompositeSrc_p.h>
#include <Fog/G2d/Painting/RasterOps_SSE2/CompositeSrcOver_p.h>

namespace Fog {

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void RasterOps_init_SSE2(void)
{
  ApiRaster& api = _api_raster;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - API]
  // --------------------------------------------------------------------------

  RasterConvertFuncs& convert = api.convert;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - Copy]
  // --------------------------------------------------------------------------
  /*
  convert.copy[RASTER_COPY_8] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::copy_8;
  convert.copy[RASTER_COPY_16] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::copy_16;
  convert.copy[RASTER_COPY_24] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::copy_24;
  convert.copy[RASTER_COPY_32] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::copy_32;
  convert.copy[RASTER_COPY_48] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::copy_48;
  convert.copy[RASTER_COPY_64] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::copy_64;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - Fill]
  // --------------------------------------------------------------------------

  convert.fill[RASTER_FILL_8] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::fill_8;
  convert.fill[RASTER_FILL_16] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::fill_16;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - BSwap]
  // --------------------------------------------------------------------------

  convert.bswap[RASTER_BSWAP_16] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::bswap_16;
  convert.bswap[RASTER_BSWAP_24] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::bswap_24;
  convert.bswap[RASTER_BSWAP_32] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::bswap_32;
  convert.bswap[RASTER_BSWAP_48] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::bswap_48;
  convert.bswap[RASTER_BSWAP_64] = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::bswap_64;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - Premultiply / Demultiply]
  // --------------------------------------------------------------------------

  convert.prgb32_from_argb32 = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::prgb32_from_argb32;
  convert.prgb64_from_argb64 = (ImageConverterBlitLineFunc)RasterOps_SSE2::Convert::prgb64_from_argb64;
  */
  // --------------------------------------------------------------------------
  // [RasterOps - Convert - ARGB32]
  // --------------------------------------------------------------------------

  /*
  convert.argb32_from[RASTER_FORMAT_RGB16_555          ] = RasterOps_SSE2::Convert::argb32_from_rgb16_555;
  convert.argb32_from[RASTER_FORMAT_RGB16_555_BS       ] = RasterOps_SSE2::Convert::argb32_from_rgb16_555_bs;
  convert.argb32_from[RASTER_FORMAT_RGB16_565          ] = RasterOps_SSE2::Convert::argb32_from_rgb16_565;
  convert.argb32_from[RASTER_FORMAT_RGB16_565_BS       ] = RasterOps_SSE2::Convert::argb32_from_rgb16_565_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB16_4444        ] = RasterOps_SSE2::Convert::argb32_from_argb16_4444;
  convert.argb32_from[RASTER_FORMAT_ARGB16_4444_BS     ] = RasterOps_SSE2::Convert::argb32_from_argb16_4444_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB16_CUSTOM      ] = RasterOps_SSE2::Convert::argb32_from_argb16_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB16_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb32_from_argb16_custom_bs;
//convert.argb32_from[RASTER_FORMAT_RGB24_888          ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_RGB24_888_BS       ] = RasterOps_SSE2::Convert::argb32_from_rgb24_888_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB24_CUSTOM      ] = RasterOps_SSE2::Convert::argb32_from_argb24_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB24_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb32_from_argb24_custom_bs;
//convert.argb32_from[RASTER_FORMAT_RGB32_888          ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_RGB32_888_BS       ] = RasterOps_SSE2::Convert::argb32_from_rgb32_888_bs;
//convert.argb32_from[RASTER_FORMAT_ARGB32_8888        ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_ARGB32_8888_BS     ] = RasterOps_SSE2::Convert::argb32_from_argb32_8888_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB32_CUSTOM      ] = RasterOps_SSE2::Convert::argb32_from_argb32_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB32_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb32_from_argb32_custom_bs;
//convert.argb32_from[RASTER_FORMAT_RGB48_161616       ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_RGB48_161616_BS    ] = RasterOps_SSE2::Convert::argb32_from_rgb48_161616_bs;
  convert.argb32_from[RASTER_FORMAT_RGB48_CUSTOM       ] = RasterOps_SSE2::Convert::argb32_from_rgb48_custom;
  convert.argb32_from[RASTER_FORMAT_RGB48_CUSTOM_BS    ] = RasterOps_SSE2::Convert::argb32_from_rgb48_custom_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB48_CUSTOM      ] = RasterOps_SSE2::Convert::argb32_from_argb48_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB48_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb32_from_argb48_custom_bs;
//convert.argb32_from[RASTER_FORMAT_ARGB64_16161616    ] = SKIP;
  convert.argb32_from[RASTER_FORMAT_ARGB64_16161616_BS ] = RasterOps_SSE2::Convert::argb32_from_argb64_16161616_bs;
  convert.argb32_from[RASTER_FORMAT_ARGB64_CUSTOM      ] = RasterOps_SSE2::Convert::argb32_from_argb64_custom;
  convert.argb32_from[RASTER_FORMAT_ARGB64_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb32_from_argb64_custom_bs;
//convert.argb32_from[RASTER_FORMAT_I8                 ];

  convert.from_argb32[RASTER_FORMAT_RGB16_555          ] = RasterOps_SSE2::Convert::rgb16_555_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB16_555_BS       ] = RasterOps_SSE2::Convert::rgb16_555_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB16_565          ] = RasterOps_SSE2::Convert::rgb16_565_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB16_565_BS       ] = RasterOps_SSE2::Convert::rgb16_565_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB16_4444        ] = RasterOps_SSE2::Convert::argb16_4444_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB16_4444_BS     ] = RasterOps_SSE2::Convert::argb16_4444_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB16_CUSTOM      ] = RasterOps_SSE2::Convert::argb16_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB16_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb16_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_RGB24_888          ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_RGB24_888_BS       ] = RasterOps_SSE2::Convert::rgb24_888_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB24_CUSTOM      ] = RasterOps_SSE2::Convert::argb24_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB24_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb24_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_RGB32_888          ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_RGB32_888_BS       ] = RasterOps_SSE2::Convert::rgb32_888_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_ARGB32_8888        ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_ARGB32_8888_BS     ] = RasterOps_SSE2::Convert::argb32_8888_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB32_CUSTOM      ] = RasterOps_SSE2::Convert::argb32_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB32_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb32_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_RGB48_161616       ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_RGB48_161616_BS    ] = RasterOps_SSE2::Convert::rgb48_161616_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB48_CUSTOM       ] = RasterOps_SSE2::Convert::rgb48_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_RGB48_CUSTOM_BS    ] = RasterOps_SSE2::Convert::rgb48_custom_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB48_CUSTOM      ] = RasterOps_SSE2::Convert::argb48_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB48_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb48_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_ARGB64_16161616    ] = SKIP;
  convert.from_argb32[RASTER_FORMAT_ARGB64_16161616_BS ] = RasterOps_SSE2::Convert::argb64_16161616_bs_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB64_CUSTOM      ] = RasterOps_SSE2::Convert::argb64_custom_from_argb32;
  convert.from_argb32[RASTER_FORMAT_ARGB64_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb64_custom_bs_from_argb32;
//convert.from_argb32[RASTER_FORMAT_I8                 ];

  // --------------------------------------------------------------------------
  //[RasterOps - Convert - ARGB64]
  // --------------------------------------------------------------------------

  convert.argb64_from[RASTER_FORMAT_RGB16_555          ] = RasterOps_SSE2::Convert::argb64_from_rgb16_555;
  convert.argb64_from[RASTER_FORMAT_RGB16_555_BS       ] = RasterOps_SSE2::Convert::argb64_from_rgb16_555_bs;
  convert.argb64_from[RASTER_FORMAT_RGB16_565          ] = RasterOps_SSE2::Convert::argb64_from_rgb16_565;
  convert.argb64_from[RASTER_FORMAT_RGB16_565_BS       ] = RasterOps_SSE2::Convert::argb64_from_rgb16_565_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB16_4444        ] = RasterOps_SSE2::Convert::argb64_from_argb16_4444;
  convert.argb64_from[RASTER_FORMAT_ARGB16_4444_BS     ] = RasterOps_SSE2::Convert::argb64_from_argb16_4444_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB16_CUSTOM      ] = RasterOps_SSE2::Convert::argb64_from_argb16_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB16_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb64_from_argb16_custom_bs;
//convert.argb64_from[RASTER_FORMAT_RGB24_888          ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_RGB24_888_BS       ] = RasterOps_SSE2::Convert::argb64_from_rgb24_888_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB24_CUSTOM      ] = RasterOps_SSE2::Convert::argb64_from_argb24_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB24_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb64_from_argb24_custom_bs;
//convert.argb64_from[RASTER_FORMAT_RGB32_888          ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_RGB32_888_BS       ] = RasterOps_SSE2::Convert::argb64_from_rgb32_888_bs;
//convert.argb64_from[RASTER_FORMAT_ARGB32_8888        ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_ARGB32_8888_BS     ] = RasterOps_SSE2::Convert::argb64_from_argb32_8888_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB32_CUSTOM      ] = RasterOps_SSE2::Convert::argb64_from_argb32_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB32_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb64_from_argb32_custom_bs;
//convert.argb64_from[RASTER_FORMAT_RGB48_161616       ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_RGB48_161616_BS    ] = RasterOps_SSE2::Convert::argb64_from_rgb48_161616_bs;
  convert.argb64_from[RASTER_FORMAT_RGB48_CUSTOM       ] = RasterOps_SSE2::Convert::argb64_from_rgb48_custom;
  convert.argb64_from[RASTER_FORMAT_RGB48_CUSTOM_BS    ] = RasterOps_SSE2::Convert::argb64_from_rgb48_custom_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB48_CUSTOM      ] = RasterOps_SSE2::Convert::argb64_from_argb48_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB48_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb64_from_argb48_custom_bs;
//convert.argb64_from[RASTER_FORMAT_ARGB64_16161616    ] = SKIP;
  convert.argb64_from[RASTER_FORMAT_ARGB64_16161616_BS ] = RasterOps_SSE2::Convert::argb64_from_argb64_16161616_bs;
  convert.argb64_from[RASTER_FORMAT_ARGB64_CUSTOM      ] = RasterOps_SSE2::Convert::argb64_from_argb64_custom;
  convert.argb64_from[RASTER_FORMAT_ARGB64_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb64_from_argb64_custom_bs;
//convert.argb64_from[RASTER_FORMAT_I8                 ];
  */

  // TODO: Image conversion.
  /*
  convert.from_argb64[RASTER_FORMAT_RGB16_555          ] = RasterOps_SSE2::Convert::rgb16_555_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB16_555_BS       ] = RasterOps_SSE2::Convert::rgb16_555_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB16_565          ] = RasterOps_SSE2::Convert::rgb16_565_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB16_565_BS       ] = RasterOps_SSE2::Convert::rgb16_565_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB16_4444        ] = RasterOps_SSE2::Convert::argb16_4444_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB16_4444_BS     ] = RasterOps_SSE2::Convert::argb16_4444_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB16_CUSTOM      ] = RasterOps_SSE2::Convert::argb16_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB16_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb16_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_RGB24_888          ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_RGB24_888_BS       ] = RasterOps_SSE2::Convert::rgb24_888_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB24_CUSTOM      ] = RasterOps_SSE2::Convert::argb24_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB24_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb24_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_RGB32_888          ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_RGB32_888_BS       ] = RasterOps_SSE2::Convert::rgb32_888_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_ARGB32_8888        ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_ARGB32_8888_BS     ] = RasterOps_SSE2::Convert::argb32_8888_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB32_CUSTOM      ] = RasterOps_SSE2::Convert::argb32_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB32_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb32_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_RGB48_161616       ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_RGB48_161616_BS    ] = RasterOps_SSE2::Convert::rgb48_161616_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB48_CUSTOM       ] = RasterOps_SSE2::Convert::rgb48_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_RGB48_CUSTOM_BS    ] = RasterOps_SSE2::Convert::rgb48_custom_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB48_CUSTOM      ] = RasterOps_SSE2::Convert::argb48_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB48_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb48_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_ARGB64_16161616    ] = SKIP;
  convert.from_argb64[RASTER_FORMAT_ARGB64_16161616_BS ] = RasterOps_SSE2::Convert::argb64_16161616_bs_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB64_CUSTOM      ] = RasterOps_SSE2::Convert::argb64_custom_from_argb64;
  convert.from_argb64[RASTER_FORMAT_ARGB64_CUSTOM_BS   ] = RasterOps_SSE2::Convert::argb64_custom_bs_from_argb64;
//convert.from_argb64[RASTER_FORMAT_I8                 ];
  */

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src - PRGB32]
  // --------------------------------------------------------------------------
/*
  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrc::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrc::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::Convert::copy_32);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_XRGB32   ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB24    ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_i8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_prgb64_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB48    ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_rgb48_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_XRGB32   ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB24    ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_a8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_i8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_prgb64_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB48    ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_rgb48_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_CORE_SRC];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrc::xrgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrc::xrgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_XRGB32   ], RasterOps_SSE2::Convert::copy_32);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB24    ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_i8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_prgb64_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB48    ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_rgb48_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_XRGB32   ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB24    ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_a8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_i8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_prgb64_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB48    ], RasterOps_SSE2::CompositeSrc::prgb32_vblit_rgb48_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrc::xrgb32_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src - RGB24]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_RGB24][RASTER_COMPOSITE_CORE_SRC];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrc::rgb24_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrc::rgb24_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_XRGB32   ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_xrgb32_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB24    ]);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_i8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_prgb64_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB48    ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_rgb48_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_XRGB32   ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_xrgb32_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB24    ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_a8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_i8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_prgb64_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB48    ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_rgb48_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrc::rgb24_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src - A8]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_A8][RASTER_COMPOSITE_CORE_SRC];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrc::a8_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrc::a8_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrc::a8_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_XRGB32   ], RasterOps_SSE2::CompositeSrc::a8_vblit_white_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB24    ], RasterOps_SSE2::CompositeSrc::a8_vblit_white_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrc::a8_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrc::a8_vblit_i8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrc::a8_vblit_prgb64_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_RGB48    ], RasterOps_SSE2::CompositeSrc::a8_vblit_white_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrc::a8_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrc::a8_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_XRGB32   ], RasterOps_SSE2::CompositeSrc::a8_vblit_white_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB24    ], RasterOps_SSE2::CompositeSrc::a8_vblit_white_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrc::a8_vblit_a8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrc::a8_vblit_i8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrc::a8_vblit_prgb64_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_RGB48    ], RasterOps_SSE2::CompositeSrc::a8_vblit_white_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrc::a8_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOver - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC_OVER];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrcOver::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrcOver::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB24    ]);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_i8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_prgb64_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB48    ]);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB24    ]);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_a8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_i8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_prgb64_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB48    ]);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOver - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_CORE_SRC_OVER];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrcOver::xrgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrcOver::xrgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB24    ]);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_a8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_i8_line);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_prgb64_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB48    ]);
    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB24    ]);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_a8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_i8_span);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_prgb64_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB48    ]);
    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrcOver::prgb32_vblit_a16_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOver - RGB24]
  // --------------------------------------------------------------------------

  {
    RasterCompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_RGB24][RASTER_COMPOSITE_CORE_SRC_OVER];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrcOver::rgb24_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB     ], RasterOps_SSE2::CompositeSrcOver::rgb24_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB     ]);

    FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB24    ]);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_a8_line);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_i8_line);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_prgb64_line);
    FOG_RASTER_SKIP(vblit_line[IMAGE_FORMAT_RGB48    ]);
  //FOG_RASTER_INIT(vblit_line[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_a16_line);

    FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_XRGB32   ]);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB24    ]);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A8       ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_a8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_I8       ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_i8_span);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_prgb64_span);
    FOG_RASTER_SKIP(vblit_span[IMAGE_FORMAT_RGB48    ]);
  //FOG_RASTER_INIT(vblit_span[IMAGE_FORMAT_A16      ], RasterOps_SSE2::CompositeSrcOver::rgb24_vblit_a16_span);
  }
  */
  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOver - A8]
  // --------------------------------------------------------------------------

  // TODO: Image compositing.
  /*
  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Clear - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_CLEAR];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], (RasterCBlitLineFunc)RasterOps_SSE2::CompositeClear::prgb32_xblit_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], (RasterCBlitSpanFunc)RasterOps_SSE2::CompositeClear::prgb32_cblit_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], (RasterVBlitLineFunc)RasterOps_SSE2::CompositeClear::prgb32_xblit_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], (RasterVBlitSpanFunc)RasterOps_SSE2::CompositeClear::prgb32_vblit_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Clear - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_CLEAR];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], (RasterCBlitLineFunc)RasterOps_SSE2::CompositeClear::xrgb32_xblit_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], (RasterCBlitSpanFunc)RasterOps_SSE2::CompositeClear::xrgb32_cblit_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], (RasterVBlitLineFunc)RasterOps_SSE2::CompositeClear::xrgb32_xblit_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], (RasterVBlitSpanFunc)RasterOps_SSE2::CompositeClear::xrgb32_vblit_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Clear - RGB24]
  // --------------------------------------------------------------------------
  
  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_RGB24][RASTER_COMPOSITE_EXT_CLEAR];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], (RasterCBlitLineFunc)RasterOps_SSE2::CompositeClear::rgb24_xblit_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], (RasterCBlitSpanFunc)RasterOps_SSE2::CompositeClear::rgb24_cblit_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_RGB24_AND_PRGB32 ], (RasterVBlitLineFunc)RasterOps_SSE2::CompositeClear::rgb24_xblit_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_RGB24_AND_XRGB32 ]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_RGB24_AND_RGB24  ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_RGB24_AND_PRGB32 ], (RasterVBlitSpanFunc)RasterOps_SSE2::CompositeClear::rgb24_vblit_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_RGB24_AND_XRGB32 ]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_RGB24_AND_RGB24  ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Clear - A8]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_A8][RASTER_COMPOSITE_EXT_CLEAR];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], (RasterCBlitLineFunc)RasterOps_SSE2::CompositeClear::a8_xblit_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], (RasterCBlitSpanFunc)RasterOps_SSE2::CompositeClear::a8_cblit_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_A8_AND_PRGB32    ], (RasterVBlitLineFunc)RasterOps_SSE2::CompositeClear::a8_xblit_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_A8_AND_A8        ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_A8_AND_PRGB32    ], (RasterVBlitSpanFunc)RasterOps_SSE2::CompositeClear::a8_vblit_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_A8_AND_A8        ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcIn - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_SRC_IN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSrcIn::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeSrcIn::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSrcIn::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeSrcIn::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSrcIn::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeSrcIn::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeSrcIn::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeSrcIn::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSrcIn::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeSrcIn::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeSrcIn::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeSrcIn::prgb32_vblit_a8_span);
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

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSrcOut::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeSrcOut::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSrcOut::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeSrcOut::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSrcOut::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeSrcOut::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeSrcOut::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeSrcOut::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSrcOut::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeSrcOut::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeSrcOut::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeSrcOut::prgb32_vblit_a8_span);
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

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSrcAtop::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSrcAtop::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSrcAtop::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeSrcAtop::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSrcAtop::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeSrcAtop::prgb32_vblit_a8_span);
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

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstOver::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDstOver::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstOver::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDstOver::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_a8_span);
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

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstIn::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstIn::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstIn::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDstIn::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstIn::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDstIn::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstIn - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DST_IN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstIn::xrgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstIn::xrgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstIn::xrgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstIn::xrgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstOut - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DST_OUT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstOut::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstOut::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstOut::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDstOut::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstOut::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDstOut::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstOut - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DST_OUT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstOut::xrgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstOut::xrgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstOut::xrgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstOut::xrgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstAtop - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DST_ATOP];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstAtop::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDstAtop::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDstAtop::prgb32_vblit_a8_span);
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

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeXor::prgb32_cblit_prgb32_line);
    FOG_RASTER_SKIP(cblit_line[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeXor::prgb32_cblit_prgb32_span);
    FOG_RASTER_SKIP(cblit_span[RASTER_CBLIT_XRGB             ]);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeXor::prgb32_vblit_prgb32_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeXor::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeXor::prgb32_vblit_prgb32_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32]);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ]);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeXor::prgb32_vblit_a8_span);
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

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositePlus::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositePlus::prgb32_cblit_prgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositePlus::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositePlus::prgb32_cblit_prgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositePlus::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositePlus::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositePlus::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositePlus::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositePlus::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositePlus::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositePlus::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositePlus::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Plus - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_PLUS];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositePlus::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositePlus::xrgb32_cblit_prgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositePlus::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositePlus::xrgb32_cblit_prgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositePlus::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositePlus::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositePlus::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositePlus::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositePlus::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositePlus::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Minus - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_MINUS];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeMinus::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeMinus::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeMinus::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeMinus::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeMinus::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeMinus::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeMinus::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeMinus::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeMinus::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeMinus::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeMinus::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeMinus::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Minus - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_MINUS];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeMinus::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeMinus::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeMinus::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeMinus::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeMinus::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeMinus::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeMinus::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeMinus::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeMinus::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeMinus::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Multiply - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_MULTIPLY];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeMultiply::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeMultiply::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeMultiply::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeMultiply::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeMultiply::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeMultiply::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeMultiply::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeMultiply::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeMultiply::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeMultiply::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeMultiply::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeMultiply::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Multiply - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_MULTIPLY];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeMultiply::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeMultiply::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeMultiply::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeMultiply::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeMultiply::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeMultiply::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeMultiply::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeMultiply::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeMultiply::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeMultiply::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Screen - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_SCREEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeScreen::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeScreen::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeScreen::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeScreen::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeScreen::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeScreen::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeScreen::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeScreen::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeScreen::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeScreen::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeScreen::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeScreen::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Screen - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_SCREEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeScreen::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeScreen::xrgb32_cblit_prgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeScreen::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeScreen::xrgb32_cblit_prgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeScreen::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeScreen::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeScreen::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeScreen::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeScreen::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeScreen::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Overlay - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_OVERLAY];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeOverlay::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeOverlay::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeOverlay::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeOverlay::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeOverlay::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeOverlay::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeOverlay::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeOverlay::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeOverlay::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeOverlay::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeOverlay::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeOverlay::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Overlay - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_OVERLAY];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeOverlay::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeOverlay::xrgb32_cblit_prgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeOverlay::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeOverlay::xrgb32_cblit_prgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeOverlay::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeOverlay::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeOverlay::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeOverlay::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeOverlay::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeOverlay::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Darken - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DARKEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDarken::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDarken::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDarken::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDarken::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDarken::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDarken::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDarken::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDarken::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDarken::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDarken::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDarken::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDarken::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Darken - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DARKEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDarken::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDarken::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDarken::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDarken::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDarken::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDarken::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDarken::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDarken::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDarken::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDarken::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Lighten - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_LIGHTEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeLighten::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeLighten::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeLighten::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeLighten::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeLighten::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeLighten::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeLighten::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeLighten::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeLighten::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeLighten::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeLighten::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeLighten::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Lighten - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_LIGHTEN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeLighten::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeLighten::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeLighten::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeLighten::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeLighten::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeLighten::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeLighten::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeLighten::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeLighten::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeLighten::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - ColorDodge - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_COLOR_DODGE];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeColorDodge::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeColorDodge::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeColorDodge::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeColorDodge::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeColorDodge::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeColorDodge::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeColorDodge::prgb32_vblit_rgb24_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeColorDodge::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeColorDodge::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeColorDodge::prgb32_vblit_rgb24_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - ColorDodge - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_COLOR_DODGE];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeColorDodge::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeColorDodge::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeColorDodge::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeColorDodge::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeColorDodge::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeColorDodge::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeColorDodge::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeColorDodge::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeColorDodge::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeColorDodge::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - ColorBurn - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_COLOR_BURN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeColorBurn::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeColorBurn::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeColorBurn::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeColorBurn::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeColorBurn::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeColorBurn::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeColorBurn::prgb32_vblit_rgb24_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeColorBurn::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeColorBurn::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeColorBurn::prgb32_vblit_rgb24_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - ColorBurn - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_COLOR_BURN];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeColorBurn::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeColorBurn::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeColorBurn::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeColorBurn::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeColorBurn::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeColorBurn::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeColorBurn::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeColorBurn::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeColorBurn::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeColorBurn::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - HardLight - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_HARD_LIGHT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeHardLight::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeHardLight::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeHardLight::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeHardLight::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeHardLight::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeHardLight::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeHardLight::prgb32_vblit_rgb24_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeHardLight::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeHardLight::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeHardLight::prgb32_vblit_rgb24_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - HardLight - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_HARD_LIGHT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeHardLight::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeHardLight::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeHardLight::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeHardLight::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeHardLight::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeHardLight::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeHardLight::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeHardLight::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeHardLight::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeHardLight::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SoftLight - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_SOFT_LIGHT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSoftLight::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeSoftLight::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSoftLight::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeSoftLight::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSoftLight::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeSoftLight::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeSoftLight::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeSoftLight::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSoftLight::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeSoftLight::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeSoftLight::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeSoftLight::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SoftLight - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_SOFT_LIGHT];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSoftLight::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeSoftLight::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeSoftLight::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeSoftLight::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSoftLight::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeSoftLight::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeSoftLight::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeSoftLight::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeSoftLight::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeSoftLight::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Difference - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_DIFFERENCE];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDifference::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDifference::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDifference::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDifference::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDifference::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDifference::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDifference::prgb32_vblit_rgb24_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDifference::prgb32_vblit_a8_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDifference::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDifference::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDifference::prgb32_vblit_rgb24_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ], RasterOps_SSE2::CompositeDifference::prgb32_vblit_a8_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Difference - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_DIFFERENCE];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDifference::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDifference::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeDifference::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeDifference::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDifference::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDifference::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDifference::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeDifference::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeDifference::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeDifference::xrgb32_vblit_rgb24_span);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Exclusion - PRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_EXT_EXCLUSION];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeExclusion::prgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeExclusion::prgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeExclusion::prgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeExclusion::prgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeExclusion::prgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeExclusion::prgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeExclusion::prgb32_vblit_rgb24_line);
    FOG_RASTER_SKIP(vblit_line[RASTER_VBLIT_PRGB32_AND_A8    ]);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_PRGB32], RasterOps_SSE2::CompositeExclusion::prgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_XRGB32], RasterOps_SSE2::CompositeExclusion::prgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_PRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeExclusion::prgb32_vblit_rgb24_span);
    FOG_RASTER_SKIP(vblit_span[RASTER_VBLIT_PRGB32_AND_A8    ]);
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Exclusion - XRGB32]
  // --------------------------------------------------------------------------

  {
    RasterCompositeExtFuncs& funcs = api.compositeExt[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_EXT_EXCLUSION];

    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeExclusion::xrgb32_cblit_prgb32_line);
    FOG_RASTER_INIT(cblit_line[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeExclusion::xrgb32_cblit_xrgb32_line);

    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_PRGB             ], RasterOps_SSE2::CompositeExclusion::xrgb32_cblit_prgb32_span);
    FOG_RASTER_INIT(cblit_span[RASTER_CBLIT_XRGB             ], RasterOps_SSE2::CompositeExclusion::xrgb32_cblit_xrgb32_span);

    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeExclusion::xrgb32_vblit_prgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeExclusion::xrgb32_vblit_xrgb32_line);
    FOG_RASTER_INIT(vblit_line[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeExclusion::xrgb32_vblit_rgb24_line);

    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_PRGB32], RasterOps_SSE2::CompositeExclusion::xrgb32_vblit_prgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_XRGB32], RasterOps_SSE2::CompositeExclusion::xrgb32_vblit_xrgb32_span);
    FOG_RASTER_INIT(vblit_span[RASTER_VBLIT_XRGB32_AND_RGB24 ], RasterOps_SSE2::CompositeExclusion::xrgb32_vblit_rgb24_span);
  }
  */
}

} // Fog namespace
