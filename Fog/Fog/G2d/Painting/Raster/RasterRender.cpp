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
  // [FillTransformedBox]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL fillTransformedBoxI(RasterContext& ctx, const BoxI& box);
  static void FOG_FASTCALL fillTransformedBoxF(RasterContext& ctx, const BoxF& box);
  static void FOG_FASTCALL fillTransformedBoxD(RasterContext& ctx, const BoxD& box);

  // --------------------------------------------------------------------------
  // [FillRasterizedShape]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL fillRasterizedShape(RasterContext& ctx, void* _rasterizer);

  // --------------------------------------------------------------------------
  // [FillTransformedPath]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL fillTransformedPathF(RasterContext& ctx, const PathF& box, uint32_t fillRule);
  static void FOG_FASTCALL fillTransformedPathD(RasterContext& ctx, const PathD& box, uint32_t fillRule);
};

template<typename C>
void RasterRenderImpl<C>::initVTable(RasterRenderVTable& v)
{
  v.fillTransformedBoxI = fillTransformedBoxI;
  v.fillTransformedBoxF = fillTransformedBoxF;
  v.fillTransformedBoxD = fillTransformedBoxD;

  v.fillRasterizedShape = fillRasterizedShape;

  v.fillTransformedPathF = fillTransformedPathF;
  v.fillTransformedPathD = fillTransformedPathD;
}

// ============================================================================
// [Fog::RasterRender - FillTransformedBox]
// ============================================================================

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::fillTransformedBoxI(RasterContext& ctx, const BoxI& box)
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
  uint32_t paintOpacity = ctx.rasterHints.opacity;

  typename C::SpanExtP staticSpan;

  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  int delta = 1;
  int offset = 0;
  sysint_t dstStrideTimesDelta = dstStride;

  if (C::_MODE == RASTER_MODE_MT)
  {
    delta = ctx.delta;
    offset = ctx.offset;
    dstStrideTimesDelta *= delta;
  }

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

      if (paintOpacity == C::_OPAQUE_VALUE)
      {
        RenderCBlitLineFn blitFull = _g2d_render.getCBlitLine(dstFormat, compositingOperator, isSrcOpaque);

        dstPixels += x0 * ctx.layer.primaryBPP;
        do {
          blitFull(dstPixels, &ctx.solid, w, &ctx.closure);

          y0 += delta;
          dstPixels += dstStrideTimesDelta;
        } while (y0 < yEnd);
      }
      else
      {
        RenderCBlitSpanFn blitSpan = _g2d_render.getCBlitSpan(dstFormat, compositingOperator, isSrcOpaque);

        staticSpan.setPositionAndType(x0, box.x1, SPAN_C);
        staticSpan.setConstMask(paintOpacity);
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
      staticSpan.setConstMask(paintOpacity);
      staticSpan.setNext(NULL);

      RenderPatternFetcher fetcher;
      if (paintOpacity == C::_OPAQUE_VALUE)
      {
        RenderVBlitLineFn blitFull = _g2d_render.getVBlitLine(dstFormat, compositingOperator, pc->getSrcFormat());
        pc->prepare(&fetcher, y0, delta, RENDER_FETCH_REFERENCE);

        dstPixels += x0 * ctx.layer.primaryBPP;
        do {
          fetcher.fetch(&staticSpan, src);
          blitFull(dstPixels, src, w, &ctx.closure);

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
  }

  // --------------------------------------------------------------------------
  // [Clip == Mask]
  // --------------------------------------------------------------------------

  else
  {
  }
}

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::fillTransformedBoxF(RasterContext& ctx, const BoxF& box)
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
void FOG_FASTCALL RasterRenderImpl<C>::fillTransformedBoxD(RasterContext& ctx, const BoxD& box)
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

  int delta = 1;
  int offset = 0;
  sysint_t dstStrideTimesDelta = dstStride;

  if (C::_MODE == RASTER_MODE_MT)
  {
    delta = ctx.delta;
    offset = ctx.offset;
    dstStrideTimesDelta *= delta;
  }

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
  }

  // --------------------------------------------------------------------------
  // [Clip == Mask]
  // --------------------------------------------------------------------------

  else
  {
  }
}

// ============================================================================
// [Fog::RasterRender - FillTransformedPath]
// ============================================================================

template<typename C>
void FOG_FASTCALL RasterRenderImpl<C>::fillTransformedPathF(RasterContext& ctx, const PathF& path, uint32_t fillRule)
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
void FOG_FASTCALL RasterRenderImpl<C>::fillTransformedPathD(RasterContext& ctx, const PathD& path, uint32_t fillRule)
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
