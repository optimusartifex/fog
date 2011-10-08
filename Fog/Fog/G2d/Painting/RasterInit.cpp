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
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterInit_p.h>

namespace Fog {

FOG_NO_EXPORT void RasterOps_init_C(void);
FOG_NO_EXPORT void RasterOps_init_skipped(void);

// ============================================================================
// [Fog::G2d - Initialization / Finalization]
// ============================================================================

FOG_NO_EXPORT void RasterOps_init(void)
{
  // Install C optimized code (default).
  RasterOps_init_C();

  // Initialize functions marked as 'SKIP'.
  RasterOps_init_skipped();
}

FOG_NO_EXPORT void RasterOps_init_skipped(void)
{
  // Post-initialize function pointers defined by Fog::Raster API. These
  // pointers are always marked as 'SKIP' in the code.

  ApiRaster& api = _api_raster;

  uint i;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - API]
  // --------------------------------------------------------------------------

  ApiRaster::_Convert& convert = api.convert;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - A8 <-> Extended]
  // --------------------------------------------------------------------------

  convert.a8_from_custom    [RASTER_FORMAT_A8               ] = api.convert.copy[RASTER_COPY_8];
  convert.a8_from_custom    [RASTER_FORMAT_A16              ] = api.compositeCore[IMAGE_FORMAT_A8][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A16];

  convert.custom_from_a8    [RASTER_FORMAT_A8               ] = api.convert.copy[RASTER_COPY_8];
  convert.custom_from_a8    [RASTER_FORMAT_A16              ] = api.compositeCore[IMAGE_FORMAT_A16][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A8];
  convert.custom_from_a8    [RASTER_FORMAT_A16_BS           ] = api.compositeCore[IMAGE_FORMAT_A16][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A8];

  // --------------------------------------------------------------------------
  //[RasterOps - Convert - A16 <-> Extended]
  // --------------------------------------------------------------------------

  convert.a16_from_custom   [RASTER_FORMAT_A8               ] = api.compositeCore[IMAGE_FORMAT_A16][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A8];
  convert.a16_from_custom   [RASTER_FORMAT_A16              ] = api.convert.copy[RASTER_COPY_16];
  convert.a16_from_custom   [RASTER_FORMAT_A16_BS           ] = api.convert.bswap[RASTER_BSWAP_16];

  convert.custom_from_a16   [RASTER_FORMAT_A8               ] = api.compositeCore[IMAGE_FORMAT_A8][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A16];
  convert.custom_from_a16   [RASTER_FORMAT_A16              ] = api.convert.copy[RASTER_COPY_16];
  convert.custom_from_a16   [RASTER_FORMAT_A16_BS           ] = api.convert.bswap[RASTER_BSWAP_16];

  // --------------------------------------------------------------------------
  //[RasterOps - Convert - ARGB32 <-> Extended]
  // --------------------------------------------------------------------------

  convert.argb32_from_custom[RASTER_FORMAT_RGB24_888        ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB24];
  convert.argb32_from_custom[RASTER_FORMAT_RGB32_888        ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_XRGB32];
  convert.argb32_from_custom[RASTER_FORMAT_RGB48_161616     ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB48];
  convert.argb32_from_custom[RASTER_FORMAT_ARGB32_8888      ] = api.convert.copy[RASTER_COPY_32];
  convert.argb32_from_custom[RASTER_FORMAT_ARGB64_16161616  ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB64];
  convert.argb32_from_custom[RASTER_FORMAT_I8               ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_I8];

  convert.custom_from_argb32[RASTER_FORMAT_RGB24_888        ] = api.compositeCore[IMAGE_FORMAT_RGB24 ][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB32];

  // TODO:

  // --------------------------------------------------------------------------
  //[RasterOps - Convert - ARGB64 <-> Extended]
  // --------------------------------------------------------------------------

  convert.argb64_from_custom[RASTER_FORMAT_RGB24_888        ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB24];
  convert.argb64_from_custom[RASTER_FORMAT_RGB32_888        ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_XRGB32];
  convert.argb64_from_custom[RASTER_FORMAT_RGB48_161616     ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB48];
  convert.argb64_from_custom[RASTER_FORMAT_ARGB32_8888      ] = api.convert.copy[RASTER_COPY_32];
  convert.argb64_from_custom[RASTER_FORMAT_ARGB64_16161616  ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB64];
  convert.argb64_from_custom[RASTER_FORMAT_I8               ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_I8];

  // TODO:

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src/SrcOver]
  // --------------------------------------------------------------------------

  for (i = 0; i < IMAGE_FORMAT_COUNT; i++)
  {
    ApiRaster::_CompositeCore& fCopy = api.compositeCore[i][RASTER_COMPOSITE_CORE_SRC];
    ApiRaster::_CompositeCore& fOver = api.compositeCore[i][RASTER_COMPOSITE_CORE_SRC_OVER];

    fCopy.cblit_line[RASTER_CBLIT_XRGB     ] = fCopy.cblit_line[RASTER_CBLIT_PRGB];
    fCopy.cblit_span[RASTER_CBLIT_XRGB     ] = fCopy.cblit_span[RASTER_CBLIT_PRGB];

    fOver.cblit_line[RASTER_CBLIT_XRGB     ] = fCopy.cblit_line[RASTER_CBLIT_PRGB];
    fOver.cblit_span[RASTER_CBLIT_XRGB     ] = fCopy.cblit_span[RASTER_CBLIT_PRGB];

    fOver.vblit_line[IMAGE_FORMAT_XRGB32   ] = fCopy.vblit_line[IMAGE_FORMAT_XRGB32   ];
    fOver.vblit_line[IMAGE_FORMAT_RGB24    ] = fCopy.vblit_line[IMAGE_FORMAT_RGB24    ];
    fOver.vblit_line[IMAGE_FORMAT_RGB48    ] = fCopy.vblit_line[IMAGE_FORMAT_RGB48    ];

    fOver.vblit_span[IMAGE_FORMAT_XRGB32   ] = fCopy.vblit_span[IMAGE_FORMAT_XRGB32   ];
    fOver.vblit_span[IMAGE_FORMAT_RGB24    ] = fCopy.vblit_span[IMAGE_FORMAT_RGB24    ];
    fOver.vblit_span[IMAGE_FORMAT_RGB48    ] = fCopy.vblit_span[IMAGE_FORMAT_RGB48    ];
  }

  api.compositeCore[IMAGE_FORMAT_PRGB32][COMPOSITE_SRC].vblit_line[IMAGE_FORMAT_PRGB32] = api.convert.copy[RASTER_COPY_32];
  api.compositeCore[IMAGE_FORMAT_XRGB32][COMPOSITE_SRC].vblit_line[IMAGE_FORMAT_XRGB32] = api.convert.copy[RASTER_COPY_32];
  api.compositeCore[IMAGE_FORMAT_RGB24 ][COMPOSITE_SRC].vblit_line[IMAGE_FORMAT_RGB24 ] = api.convert.copy[RASTER_COPY_24];
  api.compositeCore[IMAGE_FORMAT_A8    ][COMPOSITE_SRC].vblit_line[IMAGE_FORMAT_A8    ] = api.convert.copy[RASTER_COPY_8];
  api.compositeCore[IMAGE_FORMAT_PRGB64][COMPOSITE_SRC].vblit_line[IMAGE_FORMAT_PRGB64] = api.convert.copy[RASTER_COPY_64];
  api.compositeCore[IMAGE_FORMAT_RGB48 ][COMPOSITE_SRC].vblit_line[IMAGE_FORMAT_RGB48 ] = api.convert.copy[RASTER_COPY_48];
  api.compositeCore[IMAGE_FORMAT_A16   ][COMPOSITE_SRC].vblit_line[IMAGE_FORMAT_A16   ] = api.convert.copy[RASTER_COPY_16];
}

} // Fog namespace
