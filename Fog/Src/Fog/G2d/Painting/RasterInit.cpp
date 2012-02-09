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
#include <Fog/G2d/Painting/RasterOps_C/CompositeNop_p.h>

namespace Fog {

FOG_NO_EXPORT void RasterOps_init_C(void);
FOG_NO_EXPORT void RasterOps_init_skipped(void);

// TODO: Rename, and use...
static void fog_rasterengine_set_nops(RasterCompositeExtFuncs* funcs)
{
  funcs->cblit_line[RASTER_CBLIT_PRGB ] = RasterOps_C::CompositeNop::nop_cblit_line;
  funcs->cblit_span[RASTER_CBLIT_PRGB ] = RasterOps_C::CompositeNop::nop_cblit_span;
  funcs->cblit_line[RASTER_CBLIT_XRGB ] = RasterOps_C::CompositeNop::nop_cblit_line;
  funcs->cblit_span[RASTER_CBLIT_XRGB ] = RasterOps_C::CompositeNop::nop_cblit_span;

  uint i;
  for (i = 0; i < RASTER_VBLIT_COUNT; i++) funcs->vblit_line[i] = RasterOps_C::CompositeNop::nop_vblit_line;
  for (i = 0; i < RASTER_VBLIT_COUNT; i++) funcs->vblit_span[i] = RasterOps_C::CompositeNop::nop_vblit_span;
}

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

// Initialize CBlit by CompositeCore operator.
#define INIT_CBLIT_BY_COP(_DstFormat_, _SrcFormat_, _DstExtFormat_, _ByExtFormat_) \
    api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_DstExtFormat_].cblit_line[RASTER_CBLIT_##_SrcFormat_] = \
      api.compositeCore[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_CORE_##_ByExtFormat_].cblit_line[RASTER_CBLIT_##_SrcFormat_]; \
    \
    api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_DstExtFormat_].cblit_span[RASTER_CBLIT_##_SrcFormat_] = \
      api.compositeCore[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_CORE_##_ByExtFormat_].cblit_span[RASTER_CBLIT_##_SrcFormat_]

// Initialize CBlit by CompositeExt operator.
#define INIT_CBLIT_BY_EOP(_DstFormat_, _SrcFormat_, _DstExtFormat_, _ByExtFormat_) \
    api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_DstExtFormat_].cblit_line[RASTER_CBLIT_##_SrcFormat_] = \
      api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_ByExtFormat_].cblit_line[RASTER_CBLIT_##_SrcFormat_]; \
    \
    api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_DstExtFormat_].cblit_span[RASTER_CBLIT_##_SrcFormat_] = \
      api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_ByExtFormat_].cblit_span[RASTER_CBLIT_##_SrcFormat_]

// Initialize VBlit by CompositeCore operator.
#define INIT_VBLIT_BY_COP(_DstFormat_, _SrcFormat_, _DstExtFormat_, _ByExtFormat_) \
    api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_DstExtFormat_].vblit_line[RASTER_VBLIT_##_DstFormat_##_AND_##_SrcFormat_] = \
      api.compositeCore[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_CORE_##_ByExtFormat_].vblit_line[IMAGE_FORMAT_##_SrcFormat_]; \
    \
    api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_DstExtFormat_].vblit_span[RASTER_VBLIT_##_DstFormat_##_AND_##_SrcFormat_] = \
      api.compositeCore[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_CORE_##_ByExtFormat_].vblit_span[IMAGE_FORMAT_##_SrcFormat_]

// Initialize VBlit by CompositeExt operator.
#define INIT_VBLIT_BY_EOP(_DstFormat_, _SrcFormat_, _DstExtFormat_, _ByExtFormat_) \
    api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_DstExtFormat_].vblit_line[RASTER_VBLIT_##_DstFormat_##_AND_##_SrcFormat_] = \
      api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_ByExtFormat_].vblit_line[RASTER_VBLIT_##_DstFormat_##_AND_##_SrcFormat_]; \
    \
    api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_DstExtFormat_].vblit_span[RASTER_VBLIT_##_DstFormat_##_AND_##_SrcFormat_] = \
      api.compositeExt[IMAGE_FORMAT_##_DstFormat_][RASTER_COMPOSITE_EXT_##_ByExtFormat_].vblit_span[RASTER_VBLIT_##_DstFormat_##_AND_##_SrcFormat_]

FOG_NO_EXPORT void RasterOps_init_skipped(void)
{
  // Post-initialize function pointers defined by Fog::Raster API. These
  // pointers are always marked as 'SKIP' in the code.

  ApiRaster& api = _api_raster;
  uint i, j;

  // --------------------------------------------------------------------------
  // [RasterOps - Convert - API]
  // --------------------------------------------------------------------------

  RasterConvertFuncs& convert = api.convert;

  // --------------------------------------------------------------------------
  //[RasterOps - Convert - ARGB32 <-> Extended]
  // --------------------------------------------------------------------------

  convert.argb32_from[RASTER_FORMAT_RGB24_888          ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB24 ];
  convert.argb32_from[RASTER_FORMAT_RGB32_888          ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_XRGB32];
  convert.argb32_from[RASTER_FORMAT_ARGB32_8888        ] = api.convert.copy[RASTER_COPY_32];
  convert.argb32_from[RASTER_FORMAT_RGB48_161616       ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB48];
  convert.argb32_from[RASTER_FORMAT_ARGB64_16161616    ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB64];
  convert.argb32_from[RASTER_FORMAT_I8                 ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_I8    ];

  convert.from_argb32[RASTER_FORMAT_RGB24_888          ] = api.compositeCore[IMAGE_FORMAT_RGB24 ][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB32];
  convert.from_argb32[RASTER_FORMAT_RGB32_888          ] = api.compositeCore[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB32];
  convert.from_argb32[RASTER_FORMAT_RGB48_161616       ] = api.compositeCore[IMAGE_FORMAT_RGB48 ][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB32];
  convert.from_argb32[RASTER_FORMAT_ARGB32_8888        ] = api.convert.copy[RASTER_COPY_32];
  convert.from_argb32[RASTER_FORMAT_ARGB64_16161616    ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB32];

  // --------------------------------------------------------------------------
  //[RasterOps - Convert - ARGB64 <-> Extended]
  // --------------------------------------------------------------------------

  convert.argb64_from[RASTER_FORMAT_RGB24_888          ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB24 ];
  convert.argb64_from[RASTER_FORMAT_RGB32_888          ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_XRGB32];
  convert.argb64_from[RASTER_FORMAT_ARGB32_8888        ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB32];
  convert.argb64_from[RASTER_FORMAT_RGB48_161616       ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB48 ];
  convert.argb64_from[RASTER_FORMAT_ARGB64_16161616    ] = api.convert.copy[RASTER_COPY_64];
  convert.argb64_from[RASTER_FORMAT_I8                 ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_I8    ];

  convert.from_argb64[RASTER_FORMAT_RGB24_888          ] = api.compositeCore[IMAGE_FORMAT_RGB24 ][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB64];
  convert.from_argb64[RASTER_FORMAT_RGB32_888          ] = api.compositeCore[IMAGE_FORMAT_XRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB64];
  convert.from_argb64[RASTER_FORMAT_ARGB32_8888        ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB64];
  convert.from_argb64[RASTER_FORMAT_RGB48_161616       ] = api.compositeCore[IMAGE_FORMAT_RGB48 ][RASTER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB64];
  convert.from_argb64[RASTER_FORMAT_ARGB64_16161616    ] = api.convert.copy[RASTER_COPY_64];

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Src/SrcOver]
  // --------------------------------------------------------------------------

  for (i = 0; i < IMAGE_FORMAT_COUNT; i++)
  {
    RasterCompositeCoreFuncs& fCopy = api.compositeCore[i][RASTER_COMPOSITE_CORE_SRC];
    RasterCompositeCoreFuncs& fOver = api.compositeCore[i][RASTER_COMPOSITE_CORE_SRC_OVER];

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

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Dst (NOP)]
  // --------------------------------------------------------------------------

  for (i = 0; i < IMAGE_FORMAT_COUNT; i++)
  {
    RasterCompositeExtFuncs& fDst = api.compositeExt[i][RASTER_COMPOSITE_EXT_DST];

    fDst.cblit_line[RASTER_CBLIT_PRGB] = RasterOps_C::CompositeNop::nop_cblit_line;
    fDst.cblit_line[RASTER_CBLIT_XRGB] = RasterOps_C::CompositeNop::nop_cblit_line;

    fDst.cblit_span[RASTER_CBLIT_PRGB] = RasterOps_C::CompositeNop::nop_cblit_span;
    fDst.cblit_span[RASTER_CBLIT_XRGB] = RasterOps_C::CompositeNop::nop_cblit_span;

    for (j = 0; j < RASTER_VBLIT_INVALID; j++)
    {
      fDst.vblit_line[j] = RasterOps_C::CompositeNop::nop_vblit_line;
      fDst.vblit_span[j] = RasterOps_C::CompositeNop::nop_vblit_span;
    }
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Clear]
  // --------------------------------------------------------------------------

  for (i = 0; i < IMAGE_FORMAT_COUNT; i++)
  {
    RasterCompositeExtFuncs& fClear = api.compositeExt[i][RASTER_COMPOSITE_EXT_CLEAR];

    for (j = 1; j < RASTER_VBLIT_INVALID; j++)
    {
      fClear.vblit_line[j] = fClear.vblit_line[0];
      fClear.vblit_span[j] = fClear.vblit_span[0];
    }
  }

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcIn]
  // --------------------------------------------------------------------------

  // Replace 'XRGB SrcIn PRGB' by 'XRGB Src PRGB'.
  // Replace 'XRGB SrcIn XRGB' by 'XRGB Src XRGB'.
  INIT_CBLIT_BY_COP(XRGB32, PRGB  , SRC_IN  , SRC     );
  INIT_CBLIT_BY_COP(XRGB32, XRGB  , SRC_IN  , SRC     );
  INIT_VBLIT_BY_COP(XRGB32, PRGB32, SRC_IN  , SRC     );
  INIT_VBLIT_BY_COP(XRGB32, XRGB32, SRC_IN  , SRC     );
  INIT_VBLIT_BY_COP(XRGB32, RGB24 , SRC_IN  , SRC     );

  INIT_CBLIT_BY_COP(RGB24 , PRGB  , SRC_IN  , SRC     );
  INIT_CBLIT_BY_COP(RGB24 , XRGB  , SRC_IN  , SRC     );
  INIT_VBLIT_BY_COP(RGB24 , PRGB32, SRC_IN  , SRC     );
  INIT_VBLIT_BY_COP(RGB24 , XRGB32, SRC_IN  , SRC     );
  INIT_VBLIT_BY_COP(RGB24 , RGB24 , SRC_IN  , SRC     );

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcOut]
  // --------------------------------------------------------------------------

  // Replace 'XRGB SrcOut PRGB' by 'XRGB Clear PRGB'.
  // Replace 'XRGB SrcOut XRGB' by 'XRGB Clear XRGB'.
  INIT_CBLIT_BY_EOP(XRGB32, PRGB  , SRC_OUT , CLEAR   );
  INIT_CBLIT_BY_EOP(XRGB32, XRGB  , SRC_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(XRGB32, PRGB32, SRC_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(XRGB32, XRGB32, SRC_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(XRGB32, RGB24 , SRC_OUT , CLEAR   );

  INIT_CBLIT_BY_EOP(RGB24 , PRGB  , SRC_OUT , CLEAR   );
  INIT_CBLIT_BY_EOP(RGB24 , XRGB  , SRC_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(RGB24 , PRGB32, SRC_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(RGB24 , XRGB32, SRC_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(RGB24 , RGB24 , SRC_OUT , CLEAR   );

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - SrcAtop]
  // --------------------------------------------------------------------------

  // Replace 'PRGB SrcAtop XRGB' by 'PRGB SrcIn XRGB'.
  INIT_CBLIT_BY_EOP(PRGB32, PRGB  , SRC_ATOP, SRC_IN  );
  INIT_VBLIT_BY_EOP(PRGB32, XRGB32, SRC_ATOP, SRC_IN  );
  INIT_VBLIT_BY_EOP(PRGB32, RGB24 , SRC_ATOP, SRC_IN  );

  // Replace 'XRGB SrcAtop PRGB' by 'XRGB SrcOver PRGB'.
  // Replace 'XRGB SrcAtop XRGB' by 'XRGB Src XRGB'.
  INIT_CBLIT_BY_COP(XRGB32, PRGB  , SRC_ATOP, SRC_OVER);
  INIT_CBLIT_BY_COP(XRGB32, XRGB  , SRC_ATOP, SRC     );
  INIT_VBLIT_BY_COP(XRGB32, PRGB32, SRC_ATOP, SRC_OVER);
  INIT_VBLIT_BY_COP(XRGB32, XRGB32, SRC_ATOP, SRC     );
  INIT_VBLIT_BY_COP(XRGB32, RGB24 , SRC_ATOP, SRC     );

  INIT_CBLIT_BY_COP(RGB24 , PRGB  , SRC_ATOP, SRC_OVER);
  INIT_CBLIT_BY_COP(RGB24 , XRGB  , SRC_ATOP, SRC     );
  INIT_VBLIT_BY_COP(RGB24 , PRGB32, SRC_ATOP, SRC_OVER);
  INIT_VBLIT_BY_COP(RGB24 , XRGB32, SRC_ATOP, SRC     );
  INIT_VBLIT_BY_COP(RGB24 , RGB24 , SRC_ATOP, SRC     );

  // Replace 'A SrcAtop A' by 'A Dst A'.
  INIT_CBLIT_BY_EOP(A8    , PRGB  , SRC_ATOP, DST     );
  INIT_VBLIT_BY_EOP(A8    , PRGB32, SRC_ATOP, DST     );
  INIT_VBLIT_BY_EOP(A8    , A8    , SRC_ATOP, DST     );

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstOver]
  // --------------------------------------------------------------------------

  // Replace 'XRGB DstOver PRGB' by 'XRGB Dst PRGB'.
  // Replace 'XRGB DstOver XRGB' by 'XRGB Dst XRGB'.
  INIT_CBLIT_BY_EOP(XRGB32, PRGB  , DST_OVER, DST     );
  INIT_CBLIT_BY_EOP(XRGB32, XRGB  , DST_OVER, DST     );
  INIT_VBLIT_BY_EOP(XRGB32, PRGB32, DST_OVER, DST     );
  INIT_VBLIT_BY_EOP(XRGB32, XRGB32, DST_OVER, DST     );
  INIT_VBLIT_BY_EOP(XRGB32, RGB24 , DST_OVER, DST     );

  INIT_CBLIT_BY_EOP(RGB24 , PRGB  , DST_OVER, DST     );
  INIT_CBLIT_BY_EOP(RGB24 , XRGB  , DST_OVER, DST     );
  INIT_VBLIT_BY_EOP(RGB24 , PRGB32, DST_OVER, DST     );
  INIT_VBLIT_BY_EOP(RGB24 , XRGB32, DST_OVER, DST     );
  INIT_VBLIT_BY_EOP(RGB24 , RGB24 , DST_OVER, DST     );

  // Replace 'A DstOver A' by 'A SrcOver A'.
  INIT_CBLIT_BY_COP(A8    , PRGB  , DST_OVER, SRC_OVER);
  INIT_VBLIT_BY_COP(A8    , PRGB32, DST_OVER, SRC_OVER);
  INIT_VBLIT_BY_COP(A8    , A8    , DST_OVER, SRC_OVER);

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstIn]
  // --------------------------------------------------------------------------

  // Replace 'PRGB DstIn XRGB' by 'PRGB Dst XRGB'.
  INIT_CBLIT_BY_EOP(PRGB32, PRGB  , DST_IN  , DST     );
  INIT_VBLIT_BY_EOP(PRGB32, XRGB32, DST_IN  , DST     );
  INIT_VBLIT_BY_EOP(PRGB32, RGB24 , DST_IN  , DST     );

  // Replace 'XRGB DstIn XRGB' by 'XRGB Dst XRGB'.
  INIT_CBLIT_BY_EOP(XRGB32, XRGB  , DST_IN  , DST     );
  INIT_VBLIT_BY_EOP(XRGB32, XRGB32, DST_IN  , DST     );
  INIT_VBLIT_BY_EOP(XRGB32, RGB24 , DST_IN  , DST     );

  INIT_CBLIT_BY_EOP(RGB24 , XRGB  , DST_IN  , DST     );
  INIT_VBLIT_BY_EOP(RGB24 , XRGB32, DST_IN  , DST     );
  INIT_VBLIT_BY_EOP(RGB24 , RGB24 , DST_IN  , DST     );

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstOut]
  // --------------------------------------------------------------------------

  // Replace 'PRGB DstOut XRGB' by 'PRGB Clear XRGB'.
  INIT_CBLIT_BY_EOP(PRGB32, PRGB  , DST_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(PRGB32, XRGB32, DST_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(PRGB32, RGB24 , DST_OUT , CLEAR   );

  // Replace 'XRGB DstOut XRGB' by 'XRGB Clear XRGB'.
  INIT_CBLIT_BY_EOP(XRGB32, XRGB  , DST_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(XRGB32, XRGB32, DST_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(XRGB32, RGB24 , DST_OUT , CLEAR   );

  INIT_CBLIT_BY_EOP(RGB24 , XRGB  , DST_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(RGB24 , XRGB32, DST_OUT , CLEAR   );
  INIT_VBLIT_BY_EOP(RGB24 , RGB24 , DST_OUT , CLEAR   );

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - DstAtop]
  // --------------------------------------------------------------------------

  // Replace 'PRGB DstAtop XRGB' by 'PRGB DstOver XRGB'.
  INIT_CBLIT_BY_EOP(PRGB32, PRGB  , DST_ATOP, DST_OVER);
  INIT_VBLIT_BY_EOP(PRGB32, XRGB32, DST_ATOP, DST_OVER);
  INIT_VBLIT_BY_EOP(PRGB32, RGB24 , DST_ATOP, DST_OVER);

  // Replace 'XRGB DstAtop PRGB' by 'XRGB DstIn PRGB'.
  // Replace 'XRGB DstAtop XRGB' by 'XRGB Dst XRGB'.
  INIT_CBLIT_BY_EOP(XRGB32, PRGB  , DST_ATOP, DST_IN  );
  INIT_CBLIT_BY_EOP(XRGB32, XRGB  , DST_ATOP, DST     );
  INIT_VBLIT_BY_EOP(XRGB32, PRGB32, DST_ATOP, DST_IN  );
  INIT_VBLIT_BY_EOP(XRGB32, XRGB32, DST_ATOP, DST     );
  INIT_VBLIT_BY_EOP(XRGB32, RGB24 , DST_ATOP, DST     );

  INIT_CBLIT_BY_EOP(RGB24 , PRGB  , DST_ATOP, DST_IN  );
  INIT_CBLIT_BY_EOP(RGB24 , XRGB  , DST_ATOP, DST     );
  INIT_VBLIT_BY_EOP(RGB24 , PRGB32, DST_ATOP, DST_IN  );
  INIT_VBLIT_BY_EOP(RGB24 , XRGB32, DST_ATOP, DST     );
  INIT_VBLIT_BY_EOP(RGB24 , RGB24 , DST_ATOP, DST     );

  // Replace 'A DstOver A' by 'A Src A'.
  INIT_CBLIT_BY_COP(A8    , PRGB  , DST_OVER, SRC     );
  INIT_VBLIT_BY_COP(A8    , PRGB32, DST_OVER, SRC     );
  INIT_VBLIT_BY_COP(A8    , A8    , DST_OVER, SRC     );

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Xor]
  // --------------------------------------------------------------------------

  // Replace 'PRGB Xor XRGB' by 'PRGB SrcOut XRGB'.
  INIT_CBLIT_BY_EOP(PRGB32, PRGB  , XOR     , SRC_OUT );
  INIT_VBLIT_BY_EOP(PRGB32, XRGB32, XOR     , SRC_OUT );
  INIT_VBLIT_BY_EOP(PRGB32, RGB24 , XOR     , SRC_OUT );

  // Replace 'XRGB Xor PRGB' by 'XRGB DstOut PRGB'.
  // Replace 'XRGB Xor XRGB' by 'XRGB Clear XRGB'.
  INIT_CBLIT_BY_EOP(XRGB32, PRGB  , XOR     , DST_OUT );
  INIT_CBLIT_BY_EOP(XRGB32, XRGB  , XOR     , CLEAR   );
  INIT_VBLIT_BY_EOP(XRGB32, PRGB32, XOR     , DST_OUT );
  INIT_VBLIT_BY_EOP(XRGB32, XRGB32, XOR     , CLEAR   );
  INIT_VBLIT_BY_EOP(XRGB32, RGB24 , XOR     , CLEAR   );

  INIT_CBLIT_BY_EOP(RGB24 , PRGB  , XOR     , DST_OUT );
  INIT_CBLIT_BY_EOP(RGB24 , XRGB  , XOR     , CLEAR   );
  INIT_VBLIT_BY_EOP(RGB24 , PRGB32, XOR     , DST_OUT );
  INIT_VBLIT_BY_EOP(RGB24 , XRGB32, XOR     , CLEAR   );
  INIT_VBLIT_BY_EOP(RGB24 , RGB24 , XOR     , CLEAR   );

  // --------------------------------------------------------------------------
  // [RasterOps - Composite - Exclusion]
  // --------------------------------------------------------------------------

  // Replace 'PRGB Exclusion A8' by 'PRGB Difference A8'.
  INIT_VBLIT_BY_EOP(PRGB32, A8    , EXCLUSION, DIFFERENCE);
}

} // Fog namespace
