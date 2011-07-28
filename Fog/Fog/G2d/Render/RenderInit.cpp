// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Cpu/Cpu.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>
#include <Fog/G2d/Render/RenderInit_p.h>

namespace Fog {

// ============================================================================
// [Fog::G2d - Initialization / Finalization]
// ============================================================================

FOG_NO_EXPORT void Render_init(void)
{
  // Install C optimized code (default).
  Render_initC();

  // Post-initialize function pointers.
  Render_initPost();
}

FOG_NO_EXPORT void Render_initPost(void)
{
  // Post-initialize function pointers defined by Fog::G2d::Render API. These
  // pointers are always marked as POST_INITIALIZED in the code.

  G2dRenderApi& api = _g2d_render;

  uint i;

  // --------------------------------------------------------------------------
  // [Render - Convert]
  // --------------------------------------------------------------------------

  {
    G2dRenderApi::_FuncsConvert& f = api.convert;

    // A8 Destination.
    f.a8_from_dib     [RENDER_CONVERTER_DIB_A8               ] = api.convert.copy[RENDER_CONVERTER_COPY_8];
    f.a8_from_dib     [RENDER_CONVERTER_DIB_A16              ] = api.compositeCore[IMAGE_FORMAT_A8][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A16];

    // A8 Source.
    f.dib_from_a8     [RENDER_CONVERTER_DIB_A8               ] = api.convert.copy[RENDER_CONVERTER_COPY_8];
    f.dib_from_a8     [RENDER_CONVERTER_DIB_A16              ] = api.compositeCore[IMAGE_FORMAT_A16][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A8];
    f.dib_from_a8     [RENDER_CONVERTER_DIB_A16_BS           ] = api.compositeCore[IMAGE_FORMAT_A16][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A8];

    // A16 Destination.
    f.a16_from_dib    [RENDER_CONVERTER_DIB_A8               ] = api.compositeCore[IMAGE_FORMAT_A16][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A8];
    f.a16_from_dib    [RENDER_CONVERTER_DIB_A16              ] = api.convert.copy[RENDER_CONVERTER_COPY_16];
    f.a16_from_dib    [RENDER_CONVERTER_DIB_A16_BS           ] = api.convert.bswap[RENDER_CONVERTER_BSWAP_16];

    // A16 Source.
    f.dib_from_a16    [RENDER_CONVERTER_DIB_A8               ] = api.compositeCore[IMAGE_FORMAT_A8][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_A16];
    f.dib_from_a16    [RENDER_CONVERTER_DIB_A16              ] = api.convert.copy[RENDER_CONVERTER_COPY_16];
    f.dib_from_a16    [RENDER_CONVERTER_DIB_A16_BS           ] = api.convert.bswap[RENDER_CONVERTER_BSWAP_16];

    // ARGB32 Destination.
    f.argb32_from_dib [RENDER_CONVERTER_DIB_RGB24_888        ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB24];
    f.argb32_from_dib [RENDER_CONVERTER_DIB_RGB32_888        ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_XRGB32];
    f.argb32_from_dib [RENDER_CONVERTER_DIB_RGB48_161616     ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB48];
    f.argb32_from_dib [RENDER_CONVERTER_DIB_ARGB32_8888      ] = api.convert.copy[RENDER_CONVERTER_COPY_32];
    f.argb32_from_dib [RENDER_CONVERTER_DIB_ARGB64_16161616  ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB64];
    f.argb32_from_dib [RENDER_CONVERTER_DIB_I8               ] = api.compositeCore[IMAGE_FORMAT_PRGB32][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_I8];

    // ARGB32 Source.
    // TODO:

    // ARGB64 Destination.
    f.argb64_from_dib [RENDER_CONVERTER_DIB_RGB24_888        ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB24];
    f.argb64_from_dib [RENDER_CONVERTER_DIB_RGB32_888        ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_XRGB32];
    f.argb64_from_dib [RENDER_CONVERTER_DIB_RGB48_161616     ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_RGB48];
    f.argb64_from_dib [RENDER_CONVERTER_DIB_ARGB32_8888      ] = api.convert.copy[RENDER_CONVERTER_COPY_32];
    f.argb64_from_dib [RENDER_CONVERTER_DIB_ARGB64_16161616  ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_PRGB64];
    f.argb64_from_dib [RENDER_CONVERTER_DIB_I8               ] = api.compositeCore[IMAGE_FORMAT_PRGB64][RENDER_COMPOSITE_CORE_SRC].vblit_line[IMAGE_FORMAT_I8];

    // ARGB64 Source.
    // TODO:
  }

  // --------------------------------------------------------------------------
  // [Render - Composite - Src/SrcOver]
  // --------------------------------------------------------------------------

  {
    for (i = 0; i < IMAGE_FORMAT_COUNT; i++)
    {
      G2dRenderApi::_FuncsCompositeCore& fSrc     = api.compositeCore[i][RENDER_COMPOSITE_CORE_SRC];
      G2dRenderApi::_FuncsCompositeCore& fSrcOver = api.compositeCore[i][RENDER_COMPOSITE_CORE_SRC_OVER];

      fSrc.cblit_line[RENDER_CBLIT_XRGB] = fSrc.cblit_line[RENDER_CBLIT_PRGB];
      fSrc.cblit_span[RENDER_CBLIT_XRGB] = fSrc.cblit_span[RENDER_CBLIT_PRGB];

      fSrcOver.cblit_line[RENDER_CBLIT_XRGB     ] = fSrc.cblit_line[RENDER_CBLIT_PRGB];
      fSrcOver.cblit_span[RENDER_CBLIT_XRGB     ] = fSrc.cblit_span[RENDER_CBLIT_PRGB];

      fSrcOver.vblit_line[IMAGE_FORMAT_XRGB32   ] = fSrc.vblit_line[IMAGE_FORMAT_XRGB32   ];
      fSrcOver.vblit_line[IMAGE_FORMAT_RGB24    ] = fSrc.vblit_line[IMAGE_FORMAT_RGB24    ];
      fSrcOver.vblit_line[IMAGE_FORMAT_RGB48    ] = fSrc.vblit_line[IMAGE_FORMAT_RGB48    ];

      fSrcOver.vblit_span[IMAGE_FORMAT_XRGB32   ] = fSrc.vblit_span[IMAGE_FORMAT_XRGB32   ];
      fSrcOver.vblit_span[IMAGE_FORMAT_RGB24    ] = fSrc.vblit_span[IMAGE_FORMAT_RGB24    ];
      fSrcOver.vblit_span[IMAGE_FORMAT_RGB48    ] = fSrc.vblit_span[IMAGE_FORMAT_RGB48    ];
    }
  }
}

} // Fog namespace
