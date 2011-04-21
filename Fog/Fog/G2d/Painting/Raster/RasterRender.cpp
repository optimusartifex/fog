// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Painting/Raster/RasterConstants_p.h>
#include <Fog/G2d/Painting/Raster/RasterContext_p.h>
#include <Fog/G2d/Painting/Raster/RasterFuncs_p.h>
#include <Fog/G2d/Painting/Raster/RasterRender_p.h>
#include <Fog/G2d/Painting/Raster/RasterStructs_p.h>
#include <Fog/G2d/Painting/Raster/RasterUtil_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>
#include <Fog/G2d/Render/RenderStructs_p.h>
#include <Fog/G2d/Render/RenderUtil_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterRender - Mode]
// ============================================================================

struct RasterRenderModeST { enum { _MODE = RASTER_MODE_ST }; };
struct RasterRenderModeMT { enum { _MODE = RASTER_MODE_MT }; };

// ============================================================================
// [Fog::RasterRender - Precision]
// ============================================================================

struct RasterRenderPrecisionByte
{
  enum { _PRECISION = IMAGE_PRECISION_BYTE };
  enum { _OPAQUE_VALUE = 0x100 };

  typedef Span8 SpanP;
  typedef SpanExt8 SpanExtP;

  typedef Scanline8 ScanlineP;
  typedef Rasterizer8 RasterizerP;
};

struct RasterRenderPrecisionWord
{
  enum { _PRECISION = IMAGE_PRECISION_WORD };
  enum { _OPAQUE_VALUE = 0x10000 };

  typedef Span16 SpanP;
  typedef SpanExt16 SpanExtP;
};

// ============================================================================
// [Fog::RasterRender - Clip]
// ============================================================================

struct RasterRenderClipBox    { enum { _CLIP = RASTER_CLIP_BOX    }; };
struct RasterRenderClipRegion { enum { _CLIP = RASTER_CLIP_REGION }; };
struct RasterRenderClipMask   { enum { _CLIP = RASTER_CLIP_MASK   }; };

// ============================================================================
// [Fog::RasterRender - Config]
// ============================================================================

#define _FOG_DEFINE_RASTER_RENDERER_CONFIG(_Config_, _Mode_, _Precision_, _Clip_) \
  struct _Config_ : public _Mode_, public _Precision_, public _Clip_ {}

_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_ST_PB_CB, RasterRenderModeST, RasterRenderPrecisionByte, RasterRenderClipBox   );
_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_ST_PB_CR, RasterRenderModeST, RasterRenderPrecisionByte, RasterRenderClipRegion);
_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_ST_PB_CM, RasterRenderModeST, RasterRenderPrecisionByte, RasterRenderClipMask  );

_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_ST_PW_CB, RasterRenderModeST, RasterRenderPrecisionWord, RasterRenderClipBox   );
_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_ST_PW_CR, RasterRenderModeST, RasterRenderPrecisionWord, RasterRenderClipRegion);
_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_ST_PW_CM, RasterRenderModeST, RasterRenderPrecisionWord, RasterRenderClipMask  );

_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_MT_PB_CB, RasterRenderModeMT, RasterRenderPrecisionByte, RasterRenderClipBox   );
_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_MT_PB_CR, RasterRenderModeMT, RasterRenderPrecisionByte, RasterRenderClipRegion);
_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_MT_PB_CM, RasterRenderModeMT, RasterRenderPrecisionByte, RasterRenderClipMask  );

_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_MT_PW_CB, RasterRenderModeMT, RasterRenderPrecisionWord, RasterRenderClipBox   );
_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_MT_PW_CR, RasterRenderModeMT, RasterRenderPrecisionWord, RasterRenderClipRegion);
_FOG_DEFINE_RASTER_RENDERER_CONFIG(RasterRenderConfig_MT_PW_CM, RasterRenderModeMT, RasterRenderPrecisionWord, RasterRenderClipMask  );

// ============================================================================
// [Fog::RasterRender - Impl]
// ============================================================================

template<typename C>
struct RasterRenderImpl
{
  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  static void initVTable(RasterRenderVTable& v);

  // --------------------------------------------------------------------------
  // [FillRawBox]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL fillRawBoxI(RasterContext& ctx, const BoxI& box);
  static void FOG_FASTCALL fillRawBoxF(RasterContext& ctx, const BoxF& box);
  static void FOG_FASTCALL fillRawBoxD(RasterContext& ctx, const BoxD& box);

  // --------------------------------------------------------------------------
  // [FillRasterizedShape]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL fillRasterizedShape(RasterContext& ctx, void* _rasterizer);

  // --------------------------------------------------------------------------
  // [FillRawPath]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL fillRawPathF(RasterContext& ctx, const PathF& box, uint32_t fillRule);
  static void FOG_FASTCALL fillRawPathD(RasterContext& ctx, const PathD& box, uint32_t fillRule);

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL blitRawImageI(RasterContext& ctx, const PointI& pt, const Image& srcImage, const RectI& srcFragment);
};

template<typename C>
void RasterRenderImpl<C>::initVTable(RasterRenderVTable& v)
{
  v.fillRawBoxI = fillRawBoxI;
  v.fillRawBoxF = fillRawBoxF;
  v.fillRawBoxD = fillRawBoxD;

  v.fillRasterizedShape = fillRasterizedShape;

  v.fillRawPathF = fillRawPathF;
  v.fillRawPathD = fillRawPathD;

  v.blitRawImageI = blitRawImageI;
}

// ============================================================================
// [Fog::RasterRender - FillRawBox]
// ============================================================================

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::fillRawBoxI(RasterContext& ctx, const BoxI& box)
{
  // --------------------------------------------------------------------------
  // [Asserts]
  // --------------------------------------------------------------------------

  // The incoming box should be already clipped.
  FOG_ASSERT(ctx.finalClipBoxI.subsumes(box));

  // --------------------------------------------------------------------------
  // [Base]
  // --------------------------------------------------------------------------

  uint8_t* dstPixels = ctx.layer.pixels;
  sysint_t dstStride = ctx.layer.stride;
  uint32_t dstFormat = ctx.layer.primaryFormat;

  uint32_t compositingOperator = ctx.paintHints.compositingOperator;
  uint32_t opacity = ctx.rasterHints.opacity;

  typename C::SpanExtP staticSpan;

  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  const int delta  = (C::_MODE == RASTER_MODE_MT) ? ctx.delta  : 1;
  const int offset = (C::_MODE == RASTER_MODE_MT) ? ctx.offset : 0;
  sysint_t dstStrideTimesDelta = (C::_MODE == RASTER_MODE_MT) ? dstStride*delta : dstStride;

  // --------------------------------------------------------------------------
  // [Clip == Box]
  // --------------------------------------------------------------------------

  if (C::_CLIP == RASTER_CLIP_BOX)
  {
    int x0 = box.x0;
    int y0 = box.y0;
    int yEnd = box.y1;
    int w = box.getWidth();

    if (C::_MODE == RASTER_MODE_MT)
    {
      y0 = RasterUtil::alignToDelta(y0, offset, delta);
      if (y0 >= yEnd) return;
    }

    dstPixels += y0 * dstStride;

    // ------------------------------------------------------------------------
    // [Source == Solid]
    // ------------------------------------------------------------------------

    if (RasterUtil::isSolidContext(ctx.pc))
    {
      bool isSrcOpaque = Face::p32PRGB32IsAlphaFF(ctx.solid.prgb32.p32);

      if (opacity == C::_OPAQUE_VALUE)
      {
        RenderCBlitLineFn blitLine = _g2d_render.getCBlitLine(dstFormat, compositingOperator, isSrcOpaque);

        dstPixels += x0 * ctx.layer.primaryBPP;
        do {
          blitLine(dstPixels, &ctx.solid, w, &ctx.closure);

          y0 += delta;
          dstPixels += dstStrideTimesDelta;
        } while (y0 < yEnd);
      }
      else
      {
        RenderCBlitSpanFn blitSpan = _g2d_render.getCBlitSpan(dstFormat, compositingOperator, isSrcOpaque);

        staticSpan.setPositionAndType(x0, box.x1, SPAN_C);
        staticSpan.setConstMask(opacity);
        staticSpan.setNext(NULL);

        do {
          blitSpan(dstPixels, &ctx.solid, &staticSpan, &ctx.closure);

          y0 += delta;
          dstPixels += dstStrideTimesDelta;
        } while (y0 < yEnd);
      }
    }

    // ------------------------------------------------------------------------
    // [Source == Pattern]
    // ------------------------------------------------------------------------

    else
    {
      RenderPatternContext* pc = ctx.pc;
      uint8_t* src = ctx.pcRowBuffer.getMemoryBuffer();

      staticSpan.setPositionAndType(x0, box.x1, SPAN_C);
      staticSpan.setConstMask(opacity);
      staticSpan.setNext(NULL);

      RenderPatternFetcher fetcher;
      if (opacity == C::_OPAQUE_VALUE)
      {
        RenderVBlitLineFn blitLine = _g2d_render.getVBlitLine(dstFormat, compositingOperator, pc->getSrcFormat());
        pc->prepare(&fetcher, y0, delta, RENDER_FETCH_REFERENCE);

        dstPixels += x0 * ctx.layer.primaryBPP;
        do {
          fetcher.fetch(&staticSpan, src);
          blitLine(dstPixels, src, w, &ctx.closure);

          y0 += delta;
          dstPixels += dstStrideTimesDelta;
        } while (y0 < yEnd);
      }
      else
      {
        RenderVBlitSpanFn blitSpan = _g2d_render.getVBlitSpan(dstFormat, compositingOperator, pc->getSrcFormat());
        pc->prepare(&fetcher, y0, delta, RENDER_FETCH_REFERENCE);

        do {
          fetcher.fetch(&staticSpan, src);
          blitSpan(dstPixels, &staticSpan, &ctx.closure);

          y0 += delta;
          dstPixels += dstStrideTimesDelta;
        } while (y0 < yEnd);
      }
    }
  }

  // --------------------------------------------------------------------------
  // [Clip == Region]
  // --------------------------------------------------------------------------

  else if (C::_CLIP == RASTER_CLIP_REGION)
  {
    // TODO:
  }

  // --------------------------------------------------------------------------
  // [Clip == Mask]
  // --------------------------------------------------------------------------

  else
  {
    // TODO:
  }
}

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::fillRawBoxF(RasterContext& ctx, const BoxF& box)
{
  if (C::_PRECISION == IMAGE_PRECISION_BYTE)
  {
    ctx.rasterizer8->setSceneBox(ctx.finalClipBoxI);
    ctx.rasterizer8->setFillRule(FILL_RULE_NON_ZERO);
    ctx.rasterizer8->setAlpha(ctx.rasterHints.opacity);
    if (ctx.rasterizer8->initialize() != ERR_OK) return;

    ctx.rasterizer8->addBox(box);
    ctx.rasterizer8->finalize();

    if (ctx.rasterizer8->isValid())
      ctx.renderer->fillRasterizedShape(ctx, &ctx.rasterizer8);
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::fillRawBoxD(RasterContext& ctx, const BoxD& box)
{
  if (C::_PRECISION == IMAGE_PRECISION_BYTE)
  {
    ctx.rasterizer8->setSceneBox(ctx.finalClipBoxI);
    ctx.rasterizer8->setFillRule(FILL_RULE_NON_ZERO);
    ctx.rasterizer8->setAlpha(ctx.rasterHints.opacity);
    if (ctx.rasterizer8->initialize() != ERR_OK) return;

    ctx.rasterizer8->addBox(box);
    ctx.rasterizer8->finalize();

    if (ctx.rasterizer8->isValid())
      ctx.renderer->fillRasterizedShape(ctx, &ctx.rasterizer8);
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

// ============================================================================
// [Fog::RasterRender - FillRasterizedShape]
// ============================================================================

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::fillRasterizedShape(RasterContext& ctx, void* _rasterizer)
{
  // --------------------------------------------------------------------------
  // [Asserts]
  // --------------------------------------------------------------------------

  FOG_ASSERT(_rasterizer != NULL);

  // --------------------------------------------------------------------------
  // [Base]
  // --------------------------------------------------------------------------

  typename C::RasterizerP& rasterizer = *reinterpret_cast<typename C::RasterizerP*>(_rasterizer);
  const BoxI& box = rasterizer.getBoundingBox();

  uint8_t* dstPixels = ctx.layer.pixels;
  sysint_t dstStride = ctx.layer.stride;
  uint32_t dstFormat = ctx.layer.primaryFormat;

  uint32_t compositingOperator = ctx.paintHints.compositingOperator;

  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  const int delta  = (C::_MODE == RASTER_MODE_MT) ? ctx.delta  : 1;
  const int offset = (C::_MODE == RASTER_MODE_MT) ? ctx.offset : 0;
  sysint_t dstStrideTimesDelta = (C::_MODE == RASTER_MODE_MT) ? dstStride*delta : dstStride;

  // --------------------------------------------------------------------------
  // [Clip == Box]
  // --------------------------------------------------------------------------

  if (C::_CLIP == RASTER_CLIP_BOX)
  {
    int y0 = box.y0;
    int yEnd = box.y1;

    if (C::_MODE == RASTER_MODE_MT)
    {
      y0 = RasterUtil::alignToDelta(y0, offset, delta);
      if (y0 >= yEnd) return;
    }

    dstPixels += y0 * dstStride;

    // ------------------------------------------------------------------------
    // [Source == Solid]
    // ------------------------------------------------------------------------

    if (RasterUtil::isSolidContext(ctx.pc))
    {
      bool isSrcOpaque = Face::p32PRGB32IsAlphaFF(ctx.solid.prgb32.p32);
      RenderCBlitSpanFn blitSpan = _g2d_render.getCBlitSpan(dstFormat, compositingOperator, isSrcOpaque);
      typename C::ScanlineP& sl = ctx.scanline8.instance();

      do {
        typename C::SpanP* span = rasterizer.sweepScanline(sl, ctx.tmpMemory, y0);
        if (span) blitSpan(dstPixels, &ctx.solid, span, &ctx.closure);
        
        y0 += delta;
        dstPixels += dstStrideTimesDelta;
      } while (y0 < yEnd);
    }

    // ------------------------------------------------------------------------
    // [Source == Pattern]
    // ------------------------------------------------------------------------

    else
    {
      RenderPatternContext* pc = ctx.pc;
      uint8_t* src = ctx.pcRowBuffer.getMemoryBuffer();

      RenderVBlitSpanFn blitSpan = _g2d_render.getVBlitSpan(dstFormat, compositingOperator, pc->getSrcFormat());
      typename C::ScanlineP& sl = ctx.scanlineExt8.instance();

      RenderPatternFetcher fetcher;
      pc->prepare(&fetcher, y0, delta, RENDER_FETCH_REFERENCE);

      do {
        typename C::SpanP* span = rasterizer.sweepScanline(sl, ctx.tmpMemory, y0);
        if (span)
        {
          fetcher.fetch(span, src);
          blitSpan(dstPixels, span, &ctx.closure);
        }
        else
        {
          fetcher.skip(1);
        }
        
        y0 += delta;
        dstPixels += dstStrideTimesDelta;
      } while (y0 < yEnd);
    }
  }

  // --------------------------------------------------------------------------
  // [Clip == Region]
  // --------------------------------------------------------------------------

  else if (C::_CLIP == RASTER_CLIP_REGION)
  {
    // TODO:
  }

  // --------------------------------------------------------------------------
  // [Clip == Mask]
  // --------------------------------------------------------------------------

  else
  {
    // TODO:
  }
}

// ============================================================================
// [Fog::RasterRender - FillRawPath]
// ============================================================================

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::fillRawPathF(RasterContext& ctx, const PathF& path, uint32_t fillRule)
{
  if (C::_PRECISION == IMAGE_PRECISION_BYTE)
  {
    ctx.rasterizer8->setSceneBox(ctx.finalClipBoxI);
    ctx.rasterizer8->setFillRule(fillRule);
    ctx.rasterizer8->setAlpha(ctx.rasterHints.opacity);
    if (ctx.rasterizer8->initialize() != ERR_OK) return;

    ctx.rasterizer8->addPath(path);
    ctx.rasterizer8->finalize();

    if (ctx.rasterizer8->isValid())
      ctx.renderer->fillRasterizedShape(ctx, &ctx.rasterizer8);
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::fillRawPathD(RasterContext& ctx, const PathD& path, uint32_t fillRule)
{
  if (C::_PRECISION == IMAGE_PRECISION_BYTE)
  {
    ctx.rasterizer8->setSceneBox(ctx.finalClipBoxI);
    ctx.rasterizer8->setFillRule(fillRule);
    ctx.rasterizer8->setAlpha(ctx.rasterHints.opacity);
    if (ctx.rasterizer8->initialize() != ERR_OK) return;

    ctx.rasterizer8->addPath(path);
    ctx.rasterizer8->finalize();

    if (ctx.rasterizer8->isValid())
      ctx.renderer->fillRasterizedShape(ctx, &ctx.rasterizer8);
  }
  else
  {
    // TODO: 16-bit rasterizer.
  }
}

// ============================================================================
// [Fog::RasterRender - Blit]
// ============================================================================

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::blitRawImageI(RasterContext& ctx, const PointI& pt, const Image& srcImage, const RectI& srcFragment)
{
  // --------------------------------------------------------------------------
  // [Asserts]
  // --------------------------------------------------------------------------

  // ...

  // --------------------------------------------------------------------------
  // [Base]
  // --------------------------------------------------------------------------

  uint8_t* dstPixels = ctx.layer.pixels;
  sysint_t dstStride = ctx.layer.stride;
  uint32_t dstFormat = ctx.layer.primaryFormat;

  const ImageData* srcd = srcImage._d;
  const uint8_t* srcPixels = srcd->first;
  sysint_t srcStride = srcd->stride;
  uint32_t srcFormat = srcd->format;

  uint32_t compositingOperator = ctx.paintHints.compositingOperator;
  uint32_t opacity = ctx.rasterHints.opacity;

  typename C::SpanExtP staticSpan;

  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  const int delta  = (C::_MODE == RASTER_MODE_MT) ? ctx.delta  : 1;
  const int offset = (C::_MODE == RASTER_MODE_MT) ? ctx.offset : 0;

  sysint_t dstStrideTimesDelta = (C::_MODE == RASTER_MODE_MT) ? dstStride*delta : dstStride;
  sysint_t srcStrideTimesDelta = (C::_MODE == RASTER_MODE_MT) ? srcStride*delta : srcStride;

  // --------------------------------------------------------------------------
  // [Clip == Box]
  // --------------------------------------------------------------------------

  if (C::_CLIP == RASTER_CLIP_BOX)
  {
    int srcWidth = srcFragment.w;
    int srcHeight = srcFragment.h;

    int x0 = pt.x;
    int y0 = pt.y;
    int yEnd = y0 + srcHeight;

    if (C::_MODE == RASTER_MODE_MT)
    {
      y0 = RasterUtil::alignToDelta(y0, offset, delta);
      if (y0 >= yEnd) return;

      dstPixels += y0 * dstStride;
      srcPixels += (srcFragment.y + y0 - pt.y) * srcStride;
    }
    else
    {
      dstPixels += y0 * dstStride;
      srcPixels += srcFragment.y * srcStride;
    }
    
    if (opacity == C::_OPAQUE_VALUE)
    {
      RenderVBlitLineFn blitLine;

      dstPixels += x0 * ctx.layer.primaryBPP;
      srcPixels += srcFragment.x * srcd->bytesPerPixel;
      ctx.closure.palette = srcd->palette._d;

      // If compositing operator is SRC or SRC_OVER then any image format
      // combination is supported. However, if compositing operator is one
      // of other values, then only few image formats can be mixed together.
      if (RenderUtil::isCompositeCoreOperator(compositingOperator))
      {
        blitLine = _g2d_render.getCompositeCoreFuncs(dstFormat, compositingOperator)->vblit_line[srcFormat];

_Blit_ClipBox_Opaque_Direct:
        do {
          blitLine(dstPixels, srcPixels, srcWidth, &ctx.closure);

          y0 += delta;
          dstPixels += dstStrideTimesDelta;
          srcPixels += srcStrideTimesDelta;
        } while (y0 < yEnd);
      }
      else
      {
        uint32_t vBlitSrc = _g2d_render_compatibleFormat[dstFormat][srcFormat].srcFormat;
        uint32_t vBlitId = _g2d_render_compatibleFormat[dstFormat][srcFormat].vblitId;

        blitLine = _g2d_render.getCompositeExtFuncs(dstFormat, compositingOperator)->vblit_line[vBlitId];
        if (srcFormat == vBlitSrc) goto _Blit_ClipBox_Opaque_Direct;

        uint8_t* tmpPixels = ctx.pcRowBuffer.getMemoryBuffer();
        RenderVBlitLineFn cvtLine = _g2d_render.getCompositeCoreFuncs(vBlitSrc, COMPOSITE_SRC)->vblit_line[srcFormat];

        do {
          cvtLine(tmpPixels, srcPixels, srcWidth, &ctx.closure);
          blitLine(dstPixels, tmpPixels, srcWidth, &ctx.closure);

          y0 += delta;
          dstPixels += dstStrideTimesDelta;
          srcPixels += srcStrideTimesDelta;
        } while (y0 < yEnd);
      }

      ctx.closure.palette = NULL;
    }
    else
    {
      RenderVBlitSpanFn blitSpan;

      staticSpan.setPositionAndType(x0, x0 + srcWidth, SPAN_C);
      staticSpan.setConstMask(opacity);
      staticSpan.setNext(NULL);

      srcPixels += srcFragment.x * srcd->bytesPerPixel;
      ctx.closure.palette = srcd->palette._d;

      // If compositing operator is SRC or SRC_OVER then any image format
      // combination is supported. However, if compositing operator is one
      // of other values, then only few image formats can be mixed together.
      if (RenderUtil::isCompositeCoreOperator(compositingOperator))
      {
        blitSpan = _g2d_render.getCompositeCoreFuncs(dstFormat, compositingOperator)->vblit_span[srcFormat];

_Blit_ClipBox_Alpha_Direct:
        do {
          // SpanExt need non-const data pointer.
          staticSpan.setData(const_cast<uint8_t*>(srcPixels));
          blitSpan(dstPixels, &staticSpan, &ctx.closure);

          y0 += delta;
          dstPixels += dstStrideTimesDelta;
          srcPixels += srcStrideTimesDelta;
        } while (y0 < yEnd);
      }
      else
      {
        uint32_t vBlitSrc = _g2d_render_compatibleFormat[dstFormat][srcFormat].srcFormat;
        uint32_t vBlitId = _g2d_render_compatibleFormat[dstFormat][srcFormat].vblitId;

        blitSpan = _g2d_render.getCompositeExtFuncs(dstFormat, compositingOperator)->vblit_span[vBlitId];
        if (srcFormat == vBlitSrc) goto _Blit_ClipBox_Alpha_Direct;

        uint8_t* tmpPixels = ctx.pcRowBuffer.getMemoryBuffer();
        RenderVBlitLineFn cvtLine = _g2d_render.getCompositeCoreFuncs(vBlitSrc, COMPOSITE_SRC)->vblit_line[srcFormat];

        staticSpan.setData(tmpPixels);

        do {
          cvtLine(tmpPixels, srcPixels, srcWidth, &ctx.closure);
          blitSpan(dstPixels, &staticSpan, &ctx.closure);

          y0 += delta;
          dstPixels += dstStrideTimesDelta;
          srcPixels += srcStrideTimesDelta;
        } while (y0 < yEnd);
      }

      ctx.closure.palette = NULL;
    }
  }

  // --------------------------------------------------------------------------
  // [Clip == Region]
  // --------------------------------------------------------------------------

  else if (C::_CLIP == RASTER_CLIP_REGION)
  {
    // TODO:
  }

  // --------------------------------------------------------------------------
  // [Clip == Mask]
  // --------------------------------------------------------------------------

  else
  {
    // TODO:
  }
}

// ============================================================================
// [Fog::RasterRender - Initializer]
// ============================================================================

FOG_NO_EXPORT RasterRenderVTable _G2d_RasterRender_vtable[RASTER_MODE_COUNT][IMAGE_PRECISION_COUNT][RASTER_CLIP_COUNT];

FOG_NO_EXPORT void _g2d_painter_init_raster_render(void)
{
  RasterRenderImpl<RasterRenderConfig_ST_PB_CB>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_ST][IMAGE_PRECISION_BYTE][RASTER_CLIP_BOX  ]);
  RasterRenderImpl<RasterRenderConfig_ST_PB_CR>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_ST][IMAGE_PRECISION_BYTE][RASTER_CLIP_REGION]);
  RasterRenderImpl<RasterRenderConfig_ST_PB_CM>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_ST][IMAGE_PRECISION_BYTE][RASTER_CLIP_MASK  ]);
  
  RasterRenderImpl<RasterRenderConfig_MT_PB_CB>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_MT][IMAGE_PRECISION_BYTE][RASTER_CLIP_BOX  ]);
  RasterRenderImpl<RasterRenderConfig_MT_PB_CR>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_MT][IMAGE_PRECISION_BYTE][RASTER_CLIP_REGION]);
  RasterRenderImpl<RasterRenderConfig_MT_PB_CM>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_MT][IMAGE_PRECISION_BYTE][RASTER_CLIP_MASK  ]);

  /*
  // TODO: 16-bit rasterizer.
  RasterRenderImpl<RasterRenderConfig_ST_PW_CB>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_ST][IMAGE_PRECISION_WORD][RASTER_CLIP_BOX  ]);
  RasterRenderImpl<RasterRenderConfig_ST_PW_CR>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_ST][IMAGE_PRECISION_WORD][RASTER_CLIP_REGION]);
  RasterRenderImpl<RasterRenderConfig_ST_PW_CM>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_ST][IMAGE_PRECISION_WORD][RASTER_CLIP_MASK  ]);
  
  RasterRenderImpl<RasterRenderConfig_MT_PW_CB>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_MT][IMAGE_PRECISION_WORD][RASTER_CLIP_BOX  ]);
  RasterRenderImpl<RasterRenderConfig_MT_PW_CR>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_MT][IMAGE_PRECISION_WORD][RASTER_CLIP_REGION]);
  RasterRenderImpl<RasterRenderConfig_MT_PW_CM>::initVTable(_G2d_RasterRender_vtable[RASTER_MODE_MT][IMAGE_PRECISION_WORD][RASTER_CLIP_MASK  ]);
  */
}

} // Fog namespace
