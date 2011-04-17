// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Constants.h>

#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>
#include <Fog/G2d/Render/RenderInit_p.h>

#include <Fog/G2d/Render/Render_C/CBase_p.h>
#include <Fog/G2d/Render/Render_C/CClear_p.h>
#include <Fog/G2d/Render/Render_C/CConvert_p.h>
#include <Fog/G2d/Render/Render_C/CNop_p.h>
#include <Fog/G2d/Render/Render_C/CSrc_p.h>
#include <Fog/G2d/Render/Render_C/CSrcOver_p.h>
#include <Fog/G2d/Render/Render_C/PGradientBase_p.h>
#include <Fog/G2d/Render/Render_C/PGradientConical_p.h>
#include <Fog/G2d/Render/Render_C/PGradientLinear_p.h>
#include <Fog/G2d/Render/Render_C/PGradientRadial_p.h>
#include <Fog/G2d/Render/Render_C/PGradientRectangular_p.h>
#include <Fog/G2d/Render/Render_C/PGradientTriangular_p.h>
#include <Fog/G2d/Render/Render_C/PTextureAffine_p.h>
#include <Fog/G2d/Render/Render_C/PTextureBase_p.h>
#include <Fog/G2d/Render/Render_C/PTextureProjection_p.h>
#include <Fog/G2d/Render/Render_C/PTextureScale_p.h>
#include <Fog/G2d/Render/Render_C/PTextureSimple_p.h>

// C implementation can be disabled when hardcoding for SSE2.
#if !defined(FOG_HARDCODE_SSE2)
# define FOG_RENDER_INIT_C
#endif // FOG_HARDCODE_SSE2

// TODO:
# define FOG_RENDER_INIT_C

namespace Fog {

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

static void fog_rasterengine_set_nops(G2dRenderApi::CompositeExtFuncs* funcs)
{
  funcs->cblit_line[RENDER_CBLIT_PRGB ] = Render_C::CompositeNop::nop_cblit_line;
  funcs->cblit_span[RENDER_CBLIT_PRGB ] = Render_C::CompositeNop::nop_cblit_span;
  funcs->cblit_line[RENDER_CBLIT_XRGB ] = Render_C::CompositeNop::nop_cblit_line;
  funcs->cblit_span[RENDER_CBLIT_XRGB ] = Render_C::CompositeNop::nop_cblit_span;

  uint i;
  for (i = 0; i < RENDER_VBLIT_COUNT; i++) funcs->vblit_line[i] = Render_C::CompositeNop::nop_vblit_line;
  for (i = 0; i < RENDER_VBLIT_COUNT; i++) funcs->vblit_span[i] = Render_C::CompositeNop::nop_vblit_span;
}

FOG_NO_EXPORT void _g2d_render_init_c(void)
{
  G2dRenderApi& api = _g2d_render;

#if defined(FOG_RENDER_INIT_C)

  // --------------------------------------------------------------------------
  // [Render - Convert]
  // --------------------------------------------------------------------------

  {
    G2dRenderApi::ConvertFuncs& funcs = api.convert;

    RENDER_INIT(copy[RENDER_CONVERTER_COPY_8  ], (ImageConverterBlitLineFn)Render_C::CConvert::copy_8  );
    RENDER_INIT(copy[RENDER_CONVERTER_COPY_16 ], (ImageConverterBlitLineFn)Render_C::CConvert::copy_16 );
    RENDER_INIT(copy[RENDER_CONVERTER_COPY_24 ], (ImageConverterBlitLineFn)Render_C::CConvert::copy_24 );
    RENDER_INIT(copy[RENDER_CONVERTER_COPY_32 ], (ImageConverterBlitLineFn)Render_C::CConvert::copy_32 );
    RENDER_INIT(copy[RENDER_CONVERTER_COPY_48 ], (ImageConverterBlitLineFn)Render_C::CConvert::copy_48 );
    RENDER_INIT(copy[RENDER_CONVERTER_COPY_64 ], (ImageConverterBlitLineFn)Render_C::CConvert::copy_64 );

    RENDER_INIT(bswap[RENDER_CONVERTER_BSWAP_16], (ImageConverterBlitLineFn)Render_C::CConvert::bswap_16);
    RENDER_INIT(bswap[RENDER_CONVERTER_BSWAP_24], (ImageConverterBlitLineFn)Render_C::CConvert::bswap_24);
    RENDER_INIT(bswap[RENDER_CONVERTER_BSWAP_32], (ImageConverterBlitLineFn)Render_C::CConvert::bswap_32);
    RENDER_INIT(bswap[RENDER_CONVERTER_BSWAP_48], (ImageConverterBlitLineFn)Render_C::CConvert::bswap_48);
    RENDER_INIT(bswap[RENDER_CONVERTER_BSWAP_64], (ImageConverterBlitLineFn)Render_C::CConvert::bswap_64);

    // A8 Destination.
    RENDER_POST(a8_from_dib     [RENDER_CONVERTER_DIB_A8                 ]);
    RENDER_POST(a8_from_dib     [RENDER_CONVERTER_DIB_A16                ]);
    RENDER_INIT(a8_from_dib     [RENDER_CONVERTER_DIB_A16_BS             ], Render_C::CConvert::a8_native_from_a16_bs);

    // A8 Source.
    RENDER_POST(dib_from_a8     [RENDER_CONVERTER_DIB_A8                 ]);
    RENDER_POST(dib_from_a8     [RENDER_CONVERTER_DIB_A16                ]);
    RENDER_POST(dib_from_a8     [RENDER_CONVERTER_DIB_A16_BS             ]);

    // A16 Destination.
    RENDER_POST(a16_from_dib    [RENDER_CONVERTER_DIB_A8                 ]);
    RENDER_POST(a16_from_dib    [RENDER_CONVERTER_DIB_A16                ]);
    RENDER_POST(a16_from_dib    [RENDER_CONVERTER_DIB_A16_BS             ]);

    // A16 Source.
    RENDER_POST(dib_from_a16    [RENDER_CONVERTER_DIB_A8                 ]);
    RENDER_POST(dib_from_a16    [RENDER_CONVERTER_DIB_A16                ]);
    RENDER_POST(dib_from_a16    [RENDER_CONVERTER_DIB_A16_BS             ]);

    // ARGB32 Destination.
    RENDER_POST(argb32_from_dib [RENDER_CONVERTER_DIB_RGB16_555          ]);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_RGB16_555_BS       ], Render_C::CConvert::argb32_native_from_rgb16_555_bs);
    RENDER_POST(argb32_from_dib [RENDER_CONVERTER_DIB_RGB16_565          ]);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_RGB16_565_BS       ], Render_C::CConvert::argb32_native_from_rgb16_565_bs);
    RENDER_POST(argb32_from_dib [RENDER_CONVERTER_DIB_RGB24_888          ]);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_RGB24_888_BS       ], Render_C::CConvert::argb32_native_from_rgb24_888_bs);
    RENDER_POST(argb32_from_dib [RENDER_CONVERTER_DIB_RGB32_888          ]);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_RGB32_888_BS       ], Render_C::CConvert::argb32_native_from_rgb32_888_bs);
    RENDER_POST(argb32_from_dib [RENDER_CONVERTER_DIB_RGB48_161616       ]);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_RGB48_161616_BS    ], Render_C::CConvert::argb32_native_from_rgb48_161616_bs);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_RGB48_DIB          ], Render_C::CConvert::argb32_native_from_rgb48_dib);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_RGB48_DIB_BS       ], Render_C::CConvert::argb32_native_from_rgb48_dib_bs);
  
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB16_4444        ], Render_C::CConvert::argb32_native_from_argb16_4444);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB16_4444_BS     ], Render_C::CConvert::argb32_native_from_argb16_4444_bs);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB16_DIB         ], Render_C::CConvert::argb32_native_from_argb16_dib);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB16_DIB_BS      ], Render_C::CConvert::argb32_native_from_argb16_dib_bs);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB24_DIB         ], Render_C::CConvert::argb32_native_from_argb24_dib);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB24_DIB_BS      ], Render_C::CConvert::argb32_native_from_argb24_dib_bs);
    RENDER_POST(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB32_8888        ]);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB32_8888_BS     ], Render_C::CConvert::argb32_native_from_argb32_8888_bs);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB32_DIB         ], Render_C::CConvert::argb32_native_from_argb32_dib);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB32_DIB_BS      ], Render_C::CConvert::argb32_native_from_argb32_dib_bs);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB48_DIB         ], Render_C::CConvert::argb32_native_from_argb48_dib);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB48_DIB_BS      ], Render_C::CConvert::argb32_native_from_argb48_dib_bs);
    RENDER_POST(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB64_16161616    ]);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB64_16161616_BS ], Render_C::CConvert::argb32_native_from_argb64_16161616_bs);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB64_DIB         ], Render_C::CConvert::argb32_native_from_argb64_dib);
    RENDER_INIT(argb32_from_dib [RENDER_CONVERTER_DIB_ARGB64_DIB_BS      ], Render_C::CConvert::argb32_native_from_argb64_dib_bs);

    RENDER_POST(argb32_from_dib [RENDER_CONVERTER_DIB_I8                 ]);

    // ARGB64 Destination.
    RENDER_POST(argb64_from_dib [RENDER_CONVERTER_DIB_RGB16_555          ]);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_RGB16_555_BS       ], Render_C::CConvert::argb64_native_from_rgb16_555_bs);
    RENDER_POST(argb64_from_dib [RENDER_CONVERTER_DIB_RGB16_565          ]);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_RGB16_565_BS       ], Render_C::CConvert::argb64_native_from_rgb16_565_bs);
    RENDER_POST(argb64_from_dib [RENDER_CONVERTER_DIB_RGB24_888          ]);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_RGB24_888_BS       ], Render_C::CConvert::argb64_native_from_rgb24_888_bs);
    RENDER_POST(argb64_from_dib [RENDER_CONVERTER_DIB_RGB32_888          ]);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_RGB32_888_BS       ], Render_C::CConvert::argb64_native_from_rgb32_888_bs);
    RENDER_POST(argb64_from_dib [RENDER_CONVERTER_DIB_RGB48_161616       ]);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_RGB48_161616_BS    ], Render_C::CConvert::argb64_native_from_rgb48_161616_bs);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_RGB48_DIB          ], Render_C::CConvert::argb64_native_from_rgb48_dib);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_RGB48_DIB_BS       ], Render_C::CConvert::argb64_native_from_rgb48_dib_bs);

    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB16_4444        ], Render_C::CConvert::argb64_native_from_argb16_4444);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB16_4444_BS     ], Render_C::CConvert::argb64_native_from_argb16_4444_bs);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB16_DIB         ], Render_C::CConvert::argb64_native_from_argb16_dib);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB16_DIB_BS      ], Render_C::CConvert::argb64_native_from_argb16_dib_bs);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB24_DIB         ], Render_C::CConvert::argb64_native_from_argb24_dib);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB24_DIB_BS      ], Render_C::CConvert::argb64_native_from_argb24_dib_bs);
    RENDER_POST(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB32_8888        ]);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB32_8888_BS     ], Render_C::CConvert::argb64_native_from_argb32_8888_bs);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB32_DIB         ], Render_C::CConvert::argb64_native_from_argb32_dib);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB32_DIB_BS      ], Render_C::CConvert::argb64_native_from_argb32_dib_bs);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB48_DIB         ], Render_C::CConvert::argb64_native_from_argb48_dib);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB48_DIB_BS      ], Render_C::CConvert::argb64_native_from_argb48_dib_bs);
    RENDER_POST(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB64_16161616    ]);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB64_16161616_BS ], Render_C::CConvert::argb64_native_from_argb64_16161616_bs);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB64_DIB         ], Render_C::CConvert::argb64_native_from_argb64_dib);
    RENDER_INIT(argb64_from_dib [RENDER_CONVERTER_DIB_ARGB64_DIB_BS      ], Render_C::CConvert::argb64_native_from_argb64_dib_bs);

    RENDER_POST(argb64_from_dib [RENDER_CONVERTER_DIB_I8                 ]);
  }

  // --------------------------------------------------------------------------
  // [Render - Composite - Src - PRGB32]
  // --------------------------------------------------------------------------

  {
    G2dRenderApi::CompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_PRGB32][RENDER_COMPOSITE_CORE_SRC];

    RENDER_INIT(cblit_line[RENDER_CBLIT_PRGB     ], Render_C::CSrc::prgb32_cblit_prgb32_line);
    RENDER_POST(cblit_line[RENDER_CBLIT_XRGB     ]);

    RENDER_INIT(cblit_span[RENDER_CBLIT_PRGB     ], Render_C::CSrc::prgb32_cblit_prgb32_span);
    RENDER_POST(cblit_span[RENDER_CBLIT_XRGB     ]);

    RENDER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], Render_C::CConvert::copy_32);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_XRGB32   ], Render_C::CSrc::frgb32_vblit_xrgb32_line);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_RGB24    ], Render_C::CSrc::frgb32_vblit_rgb24_line);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_A8       ], Render_C::CSrc::comp8_vblit_a8_line        <Render_C::Access_PRGB32_Native>);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_I8       ], Render_C::CSrc::comp8_vblit_i8_line        <Render_C::Access_PRGB32_Native>);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], Render_C::CSrc::comp8_vblit_prgb64_line    <Render_C::Access_PRGB32_Native>);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_RGB48    ], Render_C::CSrc::comp8_vblit_rgb48_line     <Render_C::Access_PRGB32_Native>);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_A16      ], Render_C::CSrc::comp8_vblit_a16_line       <Render_C::Access_PRGB32_Native>);

    RENDER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], Render_C::CSrc::prgb32_vblit_prgb32_span);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_XRGB32   ], Render_C::CSrc::frgb32_vblit_xrgb32_span);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_RGB24    ], Render_C::CSrc::frgb32_vblit_rgb24_span);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_A8       ], Render_C::CSrc::comp8_vblit_a8_span        <Render_C::Access_PRGB32_Native>);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_I8       ], Render_C::CSrc::comp8_vblit_i8_span        <Render_C::Access_PRGB32_Native>);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], Render_C::CSrc::comp8_vblit_prgb64_span    <Render_C::Access_PRGB32_Native>);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_RGB48    ], Render_C::CSrc::comp8_vblit_rgb48_span     <Render_C::Access_PRGB32_Native>);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_A16      ], Render_C::CSrc::comp8_vblit_a16_span       <Render_C::Access_PRGB32_Native>);
  }

  // --------------------------------------------------------------------------
  // [Render - Composite - Src - XRGB32]
  // --------------------------------------------------------------------------

  {
    G2dRenderApi::CompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_XRGB32][RENDER_COMPOSITE_CORE_SRC];

    RENDER_INIT(cblit_line[RENDER_CBLIT_PRGB     ], Render_C::CSrc::xrgb32_cblit_prgb32_line);
    RENDER_POST(cblit_line[RENDER_CBLIT_XRGB     ]);

    RENDER_INIT(cblit_span[RENDER_CBLIT_PRGB     ], Render_C::CSrc::xrgb32_cblit_prgb32_span);
    RENDER_POST(cblit_span[RENDER_CBLIT_XRGB     ]);

    RENDER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], Render_C::CSrc::frgb32_vblit_xrgb32_line);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_XRGB32   ], Render_C::CConvert::copy_32);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_RGB24    ], Render_C::CSrc::frgb32_vblit_rgb24_line);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_A8       ], Render_C::CSrc::comp8_vblit_a8_line        <Render_C::Access_XRGB32_Native>);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_I8       ], Render_C::CSrc::comp8_vblit_i8_line        <Render_C::Access_XRGB32_Native>);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], Render_C::CSrc::comp8_vblit_prgb64_line    <Render_C::Access_XRGB32_Native>);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_RGB48    ], Render_C::CSrc::comp8_vblit_rgb48_line     <Render_C::Access_XRGB32_Native>);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_A16      ], Render_C::CSrc::comp8_vblit_a16_line       <Render_C::Access_XRGB32_Native>);

    RENDER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], Render_C::CSrc::xrgb32_vblit_prgb32_span);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_XRGB32   ], Render_C::CSrc::frgb32_vblit_xrgb32_span);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_RGB24    ], Render_C::CSrc::frgb32_vblit_rgb24_span);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_A8       ], Render_C::CSrc::comp8_vblit_a8_span        <Render_C::Access_XRGB32_Native>);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_I8       ], Render_C::CSrc::comp8_vblit_i8_span        <Render_C::Access_XRGB32_Native>);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], Render_C::CSrc::comp8_vblit_prgb64_span    <Render_C::Access_XRGB32_Native>);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_RGB48    ], Render_C::CSrc::comp8_vblit_rgb48_span     <Render_C::Access_XRGB32_Native>);
    RENDER_INIT(vblit_span[IMAGE_FORMAT_A16      ], Render_C::CSrc::comp8_vblit_a16_span       <Render_C::Access_XRGB32_Native>);
  }

  // --------------------------------------------------------------------------
  // [Render - Composite - SrcOver - PRGB32]
  // --------------------------------------------------------------------------

  {
    G2dRenderApi::CompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_PRGB32][RENDER_COMPOSITE_CORE_SRC_OVER];

    RENDER_INIT(cblit_line[RENDER_CBLIT_PRGB     ], Render_C::CSrcOver::prgb32_cblit_prgb32_line);
    RENDER_POST(cblit_line[RENDER_CBLIT_XRGB     ]);

    RENDER_INIT(cblit_span[RENDER_CBLIT_PRGB     ], Render_C::CSrcOver::prgb32_cblit_prgb32_span);
    RENDER_POST(cblit_span[RENDER_CBLIT_XRGB     ]);

    RENDER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], Render_C::CSrcOver::prgb32_vblit_prgb32_line);
    RENDER_POST(vblit_line[IMAGE_FORMAT_XRGB32   ]);
    RENDER_POST(vblit_line[IMAGE_FORMAT_RGB24    ]);
  //RENDER_INIT(vblit_line[IMAGE_FORMAT_A8       ], Render_C::CSrc::prgb32_vblit_a8_line);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_I8       ], Render_C::CSrcOver::prgb32_vblit_i8_line);
  //RENDER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], Render_C::CSrc::prgb32_vblit_prgb64_line);
    RENDER_POST(vblit_line[IMAGE_FORMAT_RGB48    ]);
  //RENDER_INIT(vblit_line[IMAGE_FORMAT_A16      ], Render_C::CSrc::prgb32_vblit_a16_line);

    RENDER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], Render_C::CSrcOver::prgb32_vblit_prgb32_span);
    RENDER_POST(vblit_span[IMAGE_FORMAT_XRGB32   ]);
    RENDER_POST(vblit_span[IMAGE_FORMAT_RGB24    ]);
  //RENDER_INIT(vblit_span[IMAGE_FORMAT_A8       ], Render_C::CSrc::prgb32_vblit_a8_span);
  //RENDER_INIT(vblit_span[IMAGE_FORMAT_I8       ], Render_C::CSrc::prgb32_vblit_i8_span);
  //RENDER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], Render_C::CSrc::prgb32_vblit_prgb64_span);
    RENDER_POST(vblit_span[IMAGE_FORMAT_RGB48    ]);
  //RENDER_INIT(vblit_span[IMAGE_FORMAT_A16      ], Render_C::CSrc::prgb32_vblit_a16_span);
  }
  
  // --------------------------------------------------------------------------
  // [Render - Composite - SrcOver - XRGB32]
  // --------------------------------------------------------------------------

  {
    G2dRenderApi::CompositeCoreFuncs& funcs = api.compositeCore[IMAGE_FORMAT_XRGB32][RENDER_COMPOSITE_CORE_SRC_OVER];

    RENDER_INIT(cblit_line[RENDER_CBLIT_PRGB     ], Render_C::CSrcOver::prgb32_cblit_prgb32_line);
    RENDER_POST(cblit_line[RENDER_CBLIT_XRGB     ]);

    RENDER_INIT(cblit_span[RENDER_CBLIT_PRGB     ], Render_C::CSrcOver::prgb32_cblit_prgb32_span);
    RENDER_POST(cblit_span[RENDER_CBLIT_XRGB     ]);

    RENDER_INIT(vblit_line[IMAGE_FORMAT_PRGB32   ], Render_C::CSrcOver::prgb32_vblit_prgb32_line);
    RENDER_POST(vblit_line[IMAGE_FORMAT_XRGB32   ]);
    RENDER_POST(vblit_line[IMAGE_FORMAT_RGB24    ]);
  //RENDER_INIT(vblit_line[IMAGE_FORMAT_A8       ], Render_C::CSrc::prgb32_vblit_a8_line);
    RENDER_INIT(vblit_line[IMAGE_FORMAT_I8       ], Render_C::CSrcOver::prgb32_vblit_i8_line);
  //RENDER_INIT(vblit_line[IMAGE_FORMAT_PRGB64   ], Render_C::CSrc::prgb32_vblit_prgb64_line);
    RENDER_POST(vblit_line[IMAGE_FORMAT_RGB48    ]);
  //RENDER_INIT(vblit_line[IMAGE_FORMAT_A16      ], Render_C::CSrc::prgb32_vblit_a16_line);

    RENDER_INIT(vblit_span[IMAGE_FORMAT_PRGB32   ], Render_C::CSrcOver::prgb32_vblit_prgb32_span);
    RENDER_POST(vblit_span[IMAGE_FORMAT_XRGB32   ]);
    RENDER_POST(vblit_span[IMAGE_FORMAT_RGB24    ]);
  //RENDER_INIT(vblit_span[IMAGE_FORMAT_A8       ], Render_C::CSrc::prgb32_vblit_a8_span);
  //RENDER_INIT(vblit_span[IMAGE_FORMAT_I8       ], Render_C::CSrc::prgb32_vblit_i8_span);
  //RENDER_INIT(vblit_span[IMAGE_FORMAT_PRGB64   ], Render_C::CSrc::prgb32_vblit_prgb64_span);
    RENDER_POST(vblit_span[IMAGE_FORMAT_RGB48    ]);
  //RENDER_INIT(vblit_span[IMAGE_FORMAT_A16      ], Render_C::CSrc::prgb32_vblit_a16_span);
  }
  
  // --------------------------------------------------------------------------
  // [Render - Composite - Clear]
  // --------------------------------------------------------------------------

  {
  }
#endif // FOG_RENDER_INIT_C

  // --------------------------------------------------------------------------
  // [Render - Solid]
  // --------------------------------------------------------------------------

#if defined(FOG_RENDER_INIT_C)
  {
    G2dRenderApi::SolidFuncs& solid = api.solid;

    solid.create = Render_C::Helpers::p_solid_create_solid;
    solid.destroy = Render_C::Helpers::p_solid_destroy;
    solid.prepare = Render_C::Helpers::p_solid_prepare;

    solid.fetch[IMAGE_FORMAT_PRGB32] = Render_C::Helpers::p_solid_fetch_prgb32_xrgb32;
    solid.fetch[IMAGE_FORMAT_XRGB32] = Render_C::Helpers::p_solid_fetch_prgb32_xrgb32;
    solid.fetch[IMAGE_FORMAT_A8    ] = Render_C::Helpers::p_solid_fetch_a8;

    solid.fetch[IMAGE_FORMAT_PRGB64] = Render_C::Helpers::p_solid_fetch_prgb64;
    solid.fetch[IMAGE_FORMAT_RGB48 ] = Render_C::Helpers::p_solid_fetch_rgb48;
    solid.fetch[IMAGE_FORMAT_A16   ] = Render_C::Helpers::p_solid_fetch_a16;
  }
#endif 

  // --------------------------------------------------------------------------
  // [Render - Gradient - Interpolate]
  // --------------------------------------------------------------------------

#if defined(FOG_RENDER_INIT_C)
  {
    G2dRenderApi::GradientFuncs& gradient = api.gradient;

    gradient.interpolate[IMAGE_FORMAT_PRGB32] = Render_C::PGradientBase::interpolate_prgb32;
    gradient.interpolate[IMAGE_FORMAT_XRGB32] = Render_C::PGradientBase::interpolate_prgb32;
  }
#endif 

  // --------------------------------------------------------------------------
  // [Render - Gradient - Linear]
  // --------------------------------------------------------------------------

  {
    G2dRenderApi::GradientFuncs& gradient = api.gradient;

    gradient.create[GRADIENT_TYPE_LINEAR] = Render_C::PGradientLinear::create;

#if defined(FOG_RENDER_INIT_C)
    gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_PAD    ] = Render_C::PGradientLinear::fetch_simple_nearest_pad_prgb32_xrgb32;
    gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_PAD    ] = Render_C::PGradientLinear::fetch_simple_nearest_pad_prgb32_xrgb32;

    gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REPEAT ] = Render_C::PGradientLinear::fetch_simple_nearest_repeat_prgb32_xrgb32;
    gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REPEAT ] = Render_C::PGradientLinear::fetch_simple_nearest_repeat_prgb32_xrgb32;

    gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REFLECT] = Render_C::PGradientLinear::fetch_simple_nearest_reflect_prgb32_xrgb32;
    gradient.linear.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REFLECT] = Render_C::PGradientLinear::fetch_simple_nearest_reflect_prgb32_xrgb32;

    gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_PAD    ] = Render_C::PGradientLinear::fetch_proj_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorPad_PRGB32>;
    gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_PAD    ] = Render_C::PGradientLinear::fetch_proj_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorPad_PRGB32>;

    gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REPEAT ] = Render_C::PGradientLinear::fetch_proj_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorRepeat_PRGB32>;
    gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REPEAT ] = Render_C::PGradientLinear::fetch_proj_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorRepeat_PRGB32>;

    gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REFLECT] = Render_C::PGradientLinear::fetch_proj_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorReflect_PRGB32>;
    gradient.linear.fetch_proj_nearest  [IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REFLECT] = Render_C::PGradientLinear::fetch_proj_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorReflect_PRGB32>;
#endif 
  }

  // --------------------------------------------------------------------------
  // [Render - Gradient - Radial]
  // --------------------------------------------------------------------------

  {
    G2dRenderApi::GradientFuncs& gradient = api.gradient;

    gradient.create[GRADIENT_TYPE_RADIAL] = Render_C::PGradientRadial::create;

#if defined(FOG_RENDER_INIT_C)
    gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_PAD    ] = Render_C::PGradientRadial::fetch_simple_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorPad_PRGB32>;
    gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_PAD    ] = Render_C::PGradientRadial::fetch_simple_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorPad_PRGB32>;

    gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REPEAT ] = Render_C::PGradientRadial::fetch_simple_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorRepeat_PRGB32>;
    gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REPEAT ] = Render_C::PGradientRadial::fetch_simple_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorRepeat_PRGB32>;

    gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_PRGB32][GRADIENT_SPREAD_REFLECT] = Render_C::PGradientRadial::fetch_simple_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorReflect_PRGB32>;
    gradient.radial.fetch_simple_nearest[IMAGE_FORMAT_XRGB32][GRADIENT_SPREAD_REFLECT] = Render_C::PGradientRadial::fetch_simple_nearest_template_prgb32_xrgb32<Render_C::PGradientAccessorReflect_PRGB32>;
#endif
  }

  // --------------------------------------------------------------------------
  // [Render - Texture]
  // --------------------------------------------------------------------------

  {
    G2dRenderApi::TextureFuncs& texture = api.texture;

    texture.create = Render_C::PTextureBase::create;

#if defined(FOG_RENDER_INIT_C)
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_align_pad<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_align_pad<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_align_pad<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_align_pad<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_align_pad<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subx0_pad<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subx0_pad<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subx0_pad<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subx0_pad<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subx0_pad<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_sub0y_pad<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_sub0y_pad<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_sub0y_pad<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_sub0y_pad<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_sub0y_pad<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subxy_pad<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subxy_pad<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subxy_pad<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subxy_pad<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureSimple::fetch_subxy_pad<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_align_repeat<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_align_repeat<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_align_repeat<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_align_repeat<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_align_repeat<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subx0_repeat<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subx0_repeat<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subx0_repeat<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subx0_repeat<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subx0_repeat<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_sub0y_repeat<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_sub0y_repeat<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_sub0y_repeat<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_sub0y_repeat<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_sub0y_repeat<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subxy_repeat<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subxy_repeat<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subxy_repeat<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subxy_repeat<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureSimple::fetch_subxy_repeat<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_align_reflect<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_align_reflect<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_align_reflect<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_align_reflect<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_align_reflect<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subx0_reflect<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subx0_reflect<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subx0_reflect<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subx0_reflect<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subx0_reflect<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_sub0y_reflect<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_sub0y_reflect<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_sub0y_reflect<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_sub0y_reflect<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_sub0y_reflect<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subxy_reflect<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subxy_reflect<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subxy_reflect<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subxy_reflect<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureSimple::fetch_subxy_reflect<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_align_clamp<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_align_clamp<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_align_clamp<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_align_clamp<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_align[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_align_clamp<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subx0_clamp<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subx0_clamp<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subx0_clamp<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subx0_clamp<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_subx0[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subx0_clamp<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_sub0y_clamp<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_sub0y_clamp<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_sub0y_clamp<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_sub0y_clamp<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_sub0y[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_sub0y_clamp<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subxy_clamp<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subxy_clamp<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subxy_clamp<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subxy_clamp<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_simple_subxy[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureSimple::fetch_subxy_clamp<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_nearest_pad<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_nearest_pad<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_nearest_pad<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_nearest_pad<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_nearest_pad<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_bilinear_pad<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_bilinear_pad<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_bilinear_pad<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_A8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_bilinear_pad<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_I8    ][TEXTURE_TILE_PAD    ] = Render_C::PTextureAffine::fetch_affine_bilinear_pad<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_nearest_repeat<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_nearest_repeat<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_nearest_repeat<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_nearest_repeat<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_nearest_repeat<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_bilinear_repeat<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_bilinear_repeat<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_bilinear_repeat<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_bilinear_repeat<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REPEAT ] = Render_C::PTextureAffine::fetch_affine_bilinear_repeat<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_nearest_reflect<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_nearest_reflect<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_nearest_reflect<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_nearest_reflect<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_nearest_reflect<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_bilinear_reflect<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_bilinear_reflect<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_bilinear_reflect<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_A8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_bilinear_reflect<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_I8    ][TEXTURE_TILE_REFLECT] = Render_C::PTextureAffine::fetch_affine_bilinear_reflect<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_nearest_clamp<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_nearest_clamp<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_nearest_clamp<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_nearest_clamp<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_affine_nearest [IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_nearest_clamp<Render_C::PTextureAccessor_PRGB32_From_I8    >;

    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_PRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_bilinear_clamp<Render_C::PTextureAccessor_PRGB32_From_PRGB32>;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_XRGB32][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_bilinear_clamp<Render_C::PTextureAccessor_PRGB32_From_XRGB32>;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_RGB24 ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_bilinear_clamp<Render_C::PTextureAccessor_PRGB32_From_RGB24 >;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_A8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_bilinear_clamp<Render_C::PTextureAccessor_PRGB32_From_A8    >;
    texture.prgb32.fetch_affine_bilinear[IMAGE_FORMAT_I8    ][TEXTURE_TILE_CLAMP  ] = Render_C::PTextureAffine::fetch_affine_bilinear_clamp<Render_C::PTextureAccessor_PRGB32_From_I8    >;
#endif
  }
}

} // Fog namespace
