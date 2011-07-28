// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDERAPI_P_H
#define _FOG_G2D_RENDER_RENDERAPI_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Render/RenderConstants_p.h>
#include <Fog/G2d/Render/RenderFuncs_p.h>
#include <Fog/G2d/Render/RenderUtil_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Gradient.h>
#include <Fog/G2d/Source/Texture.h>

namespace Fog {

//! @addtogroup Fog_G2d_Render
//! @{

// ============================================================================
// [Fog::G2dRenderApi]
// ============================================================================

//! @internal
//!
//! @brief Fog/G2d render functions.
struct FOG_NO_EXPORT G2dRenderApi
{
  // --------------------------------------------------------------------------
  // [Members - Convert]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _FuncsConvert
  {
    RenderConverterInitFn init;

    RenderVBlitLineFn copy[RENDER_CONVERTER_COPY_COUNT];
    RenderVBlitLineFn bswap[RENDER_CONVERTER_BSWAP_COUNT];

    RenderVBlitLineFn argb32_from_prgb32;
    RenderVBlitLineFn prgb32_from_argb32;

    RenderVBlitLineFn argb64_from_prgb64;
    RenderVBlitLineFn prgb64_from_argb64;

    RenderVBlitLineFn a8_from_dib[RENDER_CONVERTER_DIB_COUNT];
    RenderVBlitLineFn dib_from_a8[RENDER_CONVERTER_DIB_COUNT];

    RenderVBlitLineFn a16_from_dib[RENDER_CONVERTER_DIB_COUNT];
    RenderVBlitLineFn dib_from_a16[RENDER_CONVERTER_DIB_COUNT];

    RenderVBlitLineFn a32_from_dib[RENDER_CONVERTER_DIB_COUNT];
    RenderVBlitLineFn dib_from_a32[RENDER_CONVERTER_DIB_COUNT];

    RenderVBlitLineFn argb32_from_dib[RENDER_CONVERTER_DIB_COUNT];
    RenderVBlitLineFn dib_from_argb32[RENDER_CONVERTER_DIB_COUNT];

    RenderVBlitLineFn argb64_from_dib[RENDER_CONVERTER_DIB_COUNT];
    RenderVBlitLineFn dib_from_argb64[RENDER_CONVERTER_DIB_COUNT];

    // TODO: Not Implemented.
    RenderVBlitLineFn dithered_from_xrgb32[RENDER_DITHER_COUNT];
    RenderVBlitLineFn dithered_from_rgb24 [RENDER_DITHER_COUNT];
  } convert;

  // --------------------------------------------------------------------------
  // [Members - Composite - Core]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _FuncsCompositeCore
  {
    // ------------------------------------------------------------------------
    // [CompositeCore - CBlit]
    // ------------------------------------------------------------------------

    RenderCBlitRectFn cblit_rect[RENDER_CBLIT_COUNT];
    RenderCBlitLineFn cblit_line[RENDER_CBLIT_COUNT];
    RenderCBlitSpanFn cblit_span[RENDER_CBLIT_COUNT];

    // ------------------------------------------------------------------------
    // [CompositeCore - VBlit]
    // ------------------------------------------------------------------------

    RenderVBlitRectFn vblit_rect[IMAGE_FORMAT_COUNT];
    RenderVBlitLineFn vblit_line[IMAGE_FORMAT_COUNT];
    RenderVBlitSpanFn vblit_span[IMAGE_FORMAT_COUNT];
  };

  _FuncsCompositeCore compositeCore[IMAGE_FORMAT_COUNT][RENDER_COMPOSITE_CORE_COUNT];

  // --------------------------------------------------------------------------
  // [Members - Composite - Extended]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _FuncsCompositeExt
  {
    // ------------------------------------------------------------------------
    // [CompositeExt - CBlit]
    // ------------------------------------------------------------------------

    RenderCBlitLineFn cblit_line[RENDER_CBLIT_COUNT];
    RenderCBlitSpanFn cblit_span[RENDER_CBLIT_COUNT];

    // ------------------------------------------------------------------------
    // [CompositeExt - VBlit]
    // ------------------------------------------------------------------------

    RenderVBlitLineFn vblit_line[RENDER_VBLIT_COUNT];
    RenderVBlitSpanFn vblit_span[RENDER_VBLIT_COUNT];
  };

  _FuncsCompositeExt compositeExt[IMAGE_FORMAT_COUNT][RENDER_COMPOSITE_EXT_COUNT];

  // --------------------------------------------------------------------------
  // [Members - Mask]
  // --------------------------------------------------------------------------

  // TODO?
  struct FOG_NO_EXPORT _FuncsMask
  {
    RasterMaskCOpVFn c_op_v;
    RasterMaskVOpCFn v_op_c;
    RasterMaskVOpVFn v_op_v;
  };

  _FuncsMask mask[CLIP_OP_COUNT][IMAGE_FORMAT_COUNT];

  // --------------------------------------------------------------------------
  // [Members - Pattern - Solid]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _FuncsSolid
  {
    // ------------------------------------------------------------------------
    // [Create / Destroy]
    // ------------------------------------------------------------------------

    RenderPatternSolidCreateFn create;
    RenderPatternDestroyFn destroy;

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    RenderPatternPrepareFn prepare;

    // ------------------------------------------------------------------------
    // [Fetch]
    // ------------------------------------------------------------------------

    RenderPatternFetchFn fetch[IMAGE_FORMAT_COUNT];
  };

  _FuncsSolid solid;

  // --------------------------------------------------------------------------
  // [Members - Pattern - Texture]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _FuncsTexture
  {
    // ------------------------------------------------------------------------
    // [Create]
    // ------------------------------------------------------------------------

    RenderPatternTextureCreateFn create;

    struct _Fetch
    {
      // ----------------------------------------------------------------------
      // [Fetch - Simple]
      // ----------------------------------------------------------------------

      RenderPatternFetchFn fetch_simple_align[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
      RenderPatternFetchFn fetch_simple_subx0[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
      RenderPatternFetchFn fetch_simple_sub0y[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
      RenderPatternFetchFn fetch_simple_subxy[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];

      // ----------------------------------------------------------------------
      // [Fetch - Scale]
      // ----------------------------------------------------------------------

      RenderPatternFetchFn fetch_scale_nearest[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
      RenderPatternFetchFn fetch_scale_bilinear[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];

      // ----------------------------------------------------------------------
      // [Fetch - Affine]
      // ----------------------------------------------------------------------

      RenderPatternFetchFn fetch_affine_nearest[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
      RenderPatternFetchFn fetch_affine_bilinear[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];

      // ----------------------------------------------------------------------
      // [Fetch - Projection]
      // ----------------------------------------------------------------------

      RenderPatternFetchFn fetch_proj_nearest[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
      RenderPatternFetchFn fetch_proj_bilinear[IMAGE_FORMAT_COUNT][TEXTURE_TILE_COUNT];
    };

    _Fetch prgb32;
    _Fetch a8;

    _Fetch prgb64;
    _Fetch rgb48;
    _Fetch a16;
  };

  _FuncsTexture texture;

  // --------------------------------------------------------------------------
  // [Members - Pattern - Gradient]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _FuncsGradient
  {
    // ------------------------------------------------------------------------
    // [Interpolate]
    // ------------------------------------------------------------------------

    RenderPatternGradientInterpolateFn interpolate[IMAGE_FORMAT_COUNT];

    // ------------------------------------------------------------------------
    // [Create / Destroy]
    // ------------------------------------------------------------------------

    RenderPatternGradientCreateFn create[GRADIENT_TYPE_COUNT];

    // ------------------------------------------------------------------------
    // [Linear]
    // ------------------------------------------------------------------------

    struct _Linear
    {
      RenderPatternFetchFn fetch_simple_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
      RenderPatternFetchFn fetch_simple_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];

      RenderPatternFetchFn fetch_proj_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
      RenderPatternFetchFn fetch_proj_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
    } linear;

    // ------------------------------------------------------------------------
    // [Radial]
    // ------------------------------------------------------------------------

    struct _Radial
    {
      RenderPatternFetchFn fetch_simple_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
      RenderPatternFetchFn fetch_simple_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];

      RenderPatternFetchFn fetch_proj_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
      RenderPatternFetchFn fetch_proj_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
    } radial;

    // ------------------------------------------------------------------------
    // [Conical]
    // ------------------------------------------------------------------------

    struct _Conical
    {
      RenderPatternFetchFn fetch_simple_nearest[IMAGE_FORMAT_COUNT];
      RenderPatternFetchFn fetch_simple_bilinear[IMAGE_FORMAT_COUNT];
    } conical;

    // ------------------------------------------------------------------------
    // [Rectangular]
    // ------------------------------------------------------------------------

    struct _Rectangular
    {
      RenderPatternFetchFn fetch_simple_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
      RenderPatternFetchFn fetch_simple_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];

      RenderPatternFetchFn fetch_proj_nearest[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
      RenderPatternFetchFn fetch_proj_bilinear[IMAGE_FORMAT_COUNT][GRADIENT_SPREAD_COUNT];
    } rectangular;
  };

  _FuncsGradient gradient;

  // --------------------------------------------------------------------------
  // [Accessors - Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t initConverter(ImageConverterData* d) const
  {
    return convert.init(d);
  }

  // --------------------------------------------------------------------------
  // [Accessors - Composite - Copy]
  // --------------------------------------------------------------------------

  FOG_INLINE RenderVBlitRectFn getCopyRectFn(uint32_t format) const
  {
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);

    return compositeCore[format][COMPOSITE_SRC].vblit_rect[format];
  }

  FOG_INLINE RenderVBlitRectFn getCopyRectFn(uint32_t dstFormat, uint32_t srcFormat) const
  {
    FOG_ASSERT(dstFormat < IMAGE_FORMAT_COUNT);
    FOG_ASSERT(srcFormat < IMAGE_FORMAT_COUNT);

    return compositeCore[dstFormat][COMPOSITE_SRC].vblit_rect[srcFormat];
  }

  FOG_INLINE RenderVBlitLineFn getCopyFullFn(uint32_t format) const
  {
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);

    return compositeCore[format][COMPOSITE_SRC].vblit_line[format];
  }

  FOG_INLINE RenderVBlitLineFn getCopyFullFn(uint32_t dstFormat, uint32_t srcFormat) const
  {
    FOG_ASSERT(dstFormat < IMAGE_FORMAT_COUNT);
    FOG_ASSERT(srcFormat < IMAGE_FORMAT_COUNT);

    return compositeCore[dstFormat][COMPOSITE_SRC].vblit_line[srcFormat];
  }

  // --------------------------------------------------------------------------
  // [Accessors - Composite - Core]
  // --------------------------------------------------------------------------

  FOG_INLINE const _FuncsCompositeCore* get_FuncsCompositeCore(uint32_t format, uint32_t op) const
  {
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);
    FOG_ASSERT(op >= RENDER_COMPOSITE_CORE_START &&
               op <  RENDER_COMPOSITE_CORE_START + RENDER_COMPOSITE_CORE_COUNT);

    return &compositeCore[format][op];
  }

  // --------------------------------------------------------------------------
  // [Accessors - Composite - Extended]
  // --------------------------------------------------------------------------

  FOG_INLINE const _FuncsCompositeExt* get_FuncsCompositeExt(uint32_t format, uint32_t op) const
  {
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);
    FOG_ASSERT(op >= RENDER_COMPOSITE_EXT_START &&
               op <  RENDER_COMPOSITE_EXT_START + RENDER_COMPOSITE_EXT_COUNT);

    return &compositeExt[format][op - RENDER_COMPOSITE_EXT_START];
  }

  // --------------------------------------------------------------------------
  // [Accessors - Composite - Unified]
  // --------------------------------------------------------------------------

  FOG_INLINE RenderCBlitLineFn getCBlitLine(uint32_t dstFormat, uint32_t op, uint32_t isOpaque)
  {
    if (RenderUtil::isCompositeCoreOperator(op))
      return get_FuncsCompositeCore(dstFormat, op)->cblit_line[isOpaque];
    else
      return get_FuncsCompositeExt(dstFormat, op)->cblit_line[isOpaque];
  }

  FOG_INLINE RenderCBlitSpanFn getCBlitSpan(uint32_t dstFormat, uint32_t op, uint32_t isOpaque)
  {
    if (RenderUtil::isCompositeCoreOperator(op))
      return get_FuncsCompositeCore(dstFormat, op)->cblit_span[isOpaque];
    else
      return get_FuncsCompositeExt(dstFormat, op)->cblit_span[isOpaque];
  }

  FOG_INLINE RenderVBlitLineFn getVBlitLine(uint32_t dstFormat, uint32_t op, uint32_t srcFormat)
  {
    if (RenderUtil::isCompositeCoreOperator(op))
      return get_FuncsCompositeCore(dstFormat, op)->vblit_line[srcFormat];
    else
      return get_FuncsCompositeExt(dstFormat, op)->vblit_line[RenderUtil::getCompatVBlitId(dstFormat, srcFormat)];
  }

  FOG_INLINE RenderVBlitSpanFn getVBlitSpan(uint32_t dstFormat, uint32_t op, uint32_t srcFormat)
  {
    if (RenderUtil::isCompositeCoreOperator(op))
      return get_FuncsCompositeCore(dstFormat, op)->vblit_span[srcFormat];
    else
      return get_FuncsCompositeExt(dstFormat, op)->vblit_span[RenderUtil::getCompatVBlitId(dstFormat, srcFormat)];
  }

  // --------------------------------------------------------------------------
  // [Accessors - Mask]
  // --------------------------------------------------------------------------

  FOG_INLINE const _FuncsMask* getMaskFuncs(uint32_t op, uint32_t format) const
  {
    FOG_ASSERT(op < CLIP_OP_COUNT);
    FOG_ASSERT(format < IMAGE_FORMAT_COUNT);
    return &mask[op][format];
  }
};

extern FOG_API G2dRenderApi _g2d_render;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDERAPI_P_H
