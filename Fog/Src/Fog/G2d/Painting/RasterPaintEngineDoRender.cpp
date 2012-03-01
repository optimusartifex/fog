// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Acc/AccC.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Imaging/Filters/FeBase.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintContext_p.h>
#include <Fog/G2d/Painting/RasterPaintEngine_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintDoRender - VTable]
// ============================================================================

FOG_NO_EXPORT RasterPaintDoCmd RasterPaintDoRender_vtable[RASTER_MODE_COUNT];

// ============================================================================
// [Fog::RasterPaintDoRender - Filler - Declaration]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintFiller : public RasterFiller
{
  RasterPaintContext* ctx;

  uint8_t* dstPixels;
  ssize_t dstStride;

  struct _CBlit
  {
    RasterCBlitSpanFunc blit;
    RasterClosure* closure;
    RasterSolid* solid;
  };

  struct _VBlit
  {
    RasterVBlitSpanFunc blit;
    RasterClosure* closure;
    RasterPattern* pc;
    MemBuffer* pb;
    RasterPatternFetcher pf;
  };
  
  struct _FBlit
  {
    RasterVBlitSpanFunc blit;
    RasterClosure* closure;

    uint8_t* srcPixels;
    ssize_t srcStride;

    uint srcBpp;
    uint srcBaseY;
  };

  union
  {
    _CBlit c;
    _VBlit v;
    _FBlit f;
  };
};

// ============================================================================
// [Fog::RasterPaintDoRender - Filler - Solid]
// ============================================================================

static void FOG_FASTCALL RasterPaintFiller_prepare_solid_st(RasterPaintFiller* self, int y)
{
  self->dstPixels += self->dstStride * y;
}

static void FOG_FASTCALL RasterPaintFiller_prepare_solid_mt(RasterPaintFiller* self, int y)
{
  int delta = self->ctx->scope.getDelta();

  self->dstPixels += self->dstStride * y;
  self->dstStride *= delta;
}

static void FOG_FASTCALL RasterPaintFiller_process_solid(RasterPaintFiller* self, RasterSpan8* spans)
{
#if defined(FOG_DEBUG)
  RasterUtil::validateSpans<RasterSpan8>(spans, self->ctx->clipBoxI.x0, self->ctx->clipBoxI.x1);
#endif // FOG_DEBUG

  self->c.blit(self->dstPixels, self->c.solid, spans, self->c.closure);
  self->dstPixels += self->dstStride;
}

static void FOG_FASTCALL RasterPaintFiller_skip_solid(RasterPaintFiller* self, int step)
{
  self->dstPixels += self->dstStride * step;
}

// ============================================================================
// [Fog::RasterPaintDoRender - Filler - Pattern]
// ============================================================================

static void FOG_FASTCALL RasterPaintFiller_prepare_pattern_st(RasterPaintFiller* self, int y)
{
  self->v.pc->prepare(&self->v.pf, y, 1, RASTER_FETCH_REFERENCE);
  self->dstPixels += self->dstStride * y;
}

static void FOG_FASTCALL RasterPaintFiller_prepare_pattern_mt(RasterPaintFiller* self, int y)
{
  int delta = self->ctx->scope.getDelta();

  self->v.pc->prepare(&self->v.pf, y, delta, RASTER_FETCH_REFERENCE);
  self->dstPixels += self->dstStride * y;
  self->dstStride *= delta;
}

static void FOG_FASTCALL RasterPaintFiller_process_pattern(RasterPaintFiller* self, RasterSpan8* spans)
{
#if defined(FOG_DEBUG)
  RasterUtil::validateSpans<RasterSpan8>(spans, self->ctx->clipBoxI.x0, self->ctx->clipBoxI.x1);
#endif // FOG_DEBUG

  self->v.pf.fetch(spans, reinterpret_cast<uint8_t*>(self->v.pb->getMem()));
  self->v.blit(self->dstPixels, spans, self->v.closure);
  self->dstPixels += self->dstStride;
}

static void FOG_FASTCALL RasterPaintFiller_skip_pattern(RasterPaintFiller* self, int step)
{
  self->dstPixels += self->dstStride * step;
  self->v.pf.skip(step);
}

// ============================================================================
// [Fog::RasterPaintDoRender - Filler - Filter]
// ============================================================================

static void FOG_FASTCALL RasterPaintFiller_prepare_filter_st(RasterPaintFiller* self, int y)
{
  self->dstPixels += self->dstStride * y;
  self->f.srcPixels += self->f.srcStride * (y - int(self->f.srcBaseY));
}

static void FOG_FASTCALL RasterPaintFiller_prepare_filter_mt(RasterPaintFiller* self, int y)
{
  int delta = self->ctx->scope.getDelta();

  self->v.pc->prepare(&self->v.pf, y, delta, RASTER_FETCH_REFERENCE);

  self->dstPixels += self->dstStride * y;
  self->dstStride *= delta;
  
  self->f.srcPixels += self->f.srcStride * (y - int(self->f.srcBaseY));
  self->f.srcStride *= delta;
}

static void FOG_FASTCALL RasterPaintFiller_process_filter(RasterPaintFiller* self, RasterSpan8* spans)
{
#if defined(FOG_DEBUG)
  RasterUtil::validateSpans<RasterSpan8>(spans, self->ctx->clipBoxI.x0, self->ctx->clipBoxI.x1);
#endif // FOG_DEBUG

  RasterSpan8* s = spans;
  FOG_ASSERT(s != NULL);

  uint8_t* src = self->f.srcPixels;
  uint srcBpp = self->f.srcBpp;

  do {
    s->setData(src + (uint)s->getX0() * srcBpp);
    s = s->getNext();
  } while (s != NULL);

  self->v.blit(self->dstPixels, spans, self->v.closure);

  self->dstPixels += self->dstStride;
  self->f.srcPixels += self->f.srcStride;
}

static void FOG_FASTCALL RasterPaintFiller_skip_filter(RasterPaintFiller* self, int step)
{
  self->dstPixels += self->dstStride * step;
  self->f.srcPixels += self->f.srcStride * step;
}

// ============================================================================
// [Fog::RasterPaintDoRender - PrepareRasterizer]
// ============================================================================

static void FOG_INLINE RasterPaintDoRender_prepareRasterizer(RasterPaintEngine* engine, Rasterizer8* rasterizer)
{
  rasterizer->setSceneBox(engine->ctx.clipBoxI);
  rasterizer->setOpacity(engine->ctx.rasterHints.opacity);

  switch (engine->ctx.clipType)
  {
    case RASTER_CLIP_BOX:
      break;

    case RASTER_CLIP_REGION:
      rasterizer->setClipRegion(engine->ctx.clipRegion.getData(), engine->ctx.clipRegion.getLength());
      break;

    case RASTER_CLIP_MASK:
      // TODO: RasterPaintEngine - clip-mask.
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::RasterPaintDoRender - FillRasterizedShape]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_fillRasterizedShape8(RasterPaintEngine* engine, Rasterizer8* rasterizer)
{
  RasterPaintFiller filler;

  uint8_t* dstPixels = engine->ctx.target.pixels;
  ssize_t dstStride = engine->ctx.target.stride;
  uint32_t dstFormat = engine->ctx.target.format;
  uint32_t compositingOperator = engine->ctx.paintHints.compositingOperator;

  filler.ctx = &engine->ctx;
  filler.dstPixels = dstPixels;
  filler.dstStride = dstStride;

  if (RasterUtil::isSolidContext(engine->ctx.pc) || compositingOperator == COMPOSITE_CLEAR)
  {
_Solid:
    bool isSrcOpaque = Acc::p32PRGB32IsAlphaFF(engine->ctx.solid.prgb32.u32);

    filler._prepare = (RasterFiller::PrepareFunc)RasterPaintFiller_prepare_solid_st;
    filler._process = (RasterFiller::ProcessFunc)RasterPaintFiller_process_solid;
    filler._skip = (RasterFiller::SkipFunc)RasterPaintFiller_skip_solid;

    filler.c.blit = _api_raster.getCBlitSpan(dstFormat, compositingOperator, isSrcOpaque);
    filler.c.closure = &engine->ctx.closure;
    filler.c.solid = &engine->ctx.solid;

    rasterizer->render(&filler, &engine->ctx.scanline8);
  }
  else
  {
    _FOG_RASTER_ENSURE_PATTERN(engine);

    uint32_t srcFormat = engine->ctx.pc->getSrcFormat();
    compositingOperator = RasterUtil::getCompositeModifiedOperator(dstFormat, compositingOperator, engine->ctx.pc->isOpaque());

    if (compositingOperator == COMPOSITE_CLEAR)
      goto _Solid;

    filler._prepare = (RasterFiller::PrepareFunc)RasterPaintFiller_prepare_pattern_st;
    filler._process = (RasterFiller::ProcessFunc)RasterPaintFiller_process_pattern;
    filler._skip = (RasterFiller::SkipFunc)RasterPaintFiller_skip_pattern;

    filler.v.blit = _api_raster.getVBlitSpan(dstFormat, compositingOperator, srcFormat);
    filler.v.closure = &engine->ctx.closure;
    filler.v.pc = engine->ctx.pc;
    filler.v.pb = &engine->ctx.buffer;

    rasterizer->render(&filler, &engine->ctx.scanline8);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintDoRender - FillAll]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_fillAll(
  RasterPaintEngine* engine)
{
  return engine->doCmd->fillNormalizedBoxI(engine, &engine->ctx.clipBoxI);
}

// ============================================================================
// [Fog::RasterPaintDoRender - FillNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_fillNormalizedBoxI(
  RasterPaintEngine* engine, const BoxI* box)
{
  FOG_ASSERT(box->isValid());

  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      // Fast-path (clip-box and full-opacity).
      if (engine->ctx.rasterHints.opacity == 0x100 && engine->ctx.clipType == RASTER_CLIP_BOX)
      {
        uint8_t* dstPixels = engine->ctx.target.pixels;
        ssize_t dstStride = engine->ctx.target.stride;
        uint32_t dstFormat = engine->ctx.target.format;
        uint32_t compositingOperator = engine->ctx.paintHints.compositingOperator;

        int y0 = box->y0;

        int w = box->x1 - box->x0;
        int i = box->y1 - box->y0;

        dstPixels += y0 * dstStride;

        if (RasterUtil::isSolidContext(engine->ctx.pc) || compositingOperator == COMPOSITE_CLEAR)
        {
_Solid:
          bool isSrcOpaque = Acc::p32PRGB32IsAlphaFF(engine->ctx.solid.prgb32.u32);
          RasterCBlitLineFunc blitLine = _api_raster.getCBlitLine(dstFormat, compositingOperator, isSrcOpaque);

          dstPixels += box->x0 * engine->ctx.target.bpp;
          do {
            blitLine(dstPixels, &engine->ctx.solid, w, &engine->ctx.closure);
            dstPixels += dstStride;
          } while (--i);
        }
        else
        {
          _FOG_RASTER_ENSURE_PATTERN(engine);

          RasterPattern* pc = engine->ctx.pc;
          RasterPatternFetcher pf;

          uint32_t srcFormat = pc->getSrcFormat();
          compositingOperator = RasterUtil::getCompositeModifiedOperator(dstFormat, compositingOperator, pc->isOpaque());

          if (compositingOperator == COMPOSITE_CLEAR)
            goto _Solid;

          RasterSpan8 span[1];
          span[0].setPositionAndType(box->x0, box->x1, RASTER_SPAN_C);
          span[0].setConstMask(0x100);
          span[0].setNext(NULL);

          if (RasterUtil::isCompositeCopyOp(dstFormat, srcFormat, compositingOperator))
          {
            pc->prepare(&pf, y0, 1, RASTER_FETCH_COPY);

            dstPixels += box->x0 * engine->ctx.target.bpp;
            do {
              pf.fetch(span, dstPixels);
              dstPixels += dstStride;
            } while (--i);
          }
          else
          {
            pc->prepare(&pf, y0, 1, RASTER_FETCH_REFERENCE);

            RasterVBlitLineFunc blitLine = _api_raster.getVBlitLine(dstFormat, compositingOperator, srcFormat);
            uint8_t* srcPixels = reinterpret_cast<uint8_t*>(engine->ctx.buffer.getMem());

            dstPixels += box->x0 * engine->ctx.target.bpp;

            do {
              pf.fetch(span, srcPixels);
              blitLine(dstPixels, span->getData(), w, &engine->ctx.closure);
              dstPixels += dstStride;
            } while (--i);
          }
        }
        return ERR_OK;
      }
      else
      {
        BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
        RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

        rasterizer->init32x0(*box);
        return RasterPaintDoRender_fillRasterizedShape8(engine, rasterizer);
      }
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_FASTCALL RasterPaintDoRender_fillNormalizedBoxF(
  RasterPaintEngine* engine, const BoxF* box)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      BoxI box24x8(UNINITIALIZED);
      box24x8.x0 = Math::fixed24x8FromFloat(box->x0);
      box24x8.y0 = Math::fixed24x8FromFloat(box->y0);
      box24x8.x1 = Math::fixed24x8FromFloat(box->x1);
      box24x8.y1 = Math::fixed24x8FromFloat(box->y1);

      if (RasterUtil::isBox24x8Aligned(box24x8))
      {
        BoxI boxI(box24x8.x0 >> 8, box24x8.y0 >> 8, box24x8.x1 >> 8, box24x8.y1 >> 8);
        return engine->doCmd->fillNormalizedBoxI(engine, &boxI);
      }

      BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
      RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

      rasterizer->init24x8(box24x8);
      return RasterPaintDoRender_fillRasterizedShape8(engine, rasterizer);
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_FASTCALL RasterPaintDoRender_fillNormalizedBoxD(
  RasterPaintEngine* engine, const BoxD* box)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      BoxI box24x8(UNINITIALIZED);
      box24x8.x0 = Math::fixed24x8FromFloat(box->x0);
      box24x8.y0 = Math::fixed24x8FromFloat(box->y0);
      box24x8.x1 = Math::fixed24x8FromFloat(box->x1);
      box24x8.y1 = Math::fixed24x8FromFloat(box->y1);

      if (RasterUtil::isBox24x8Aligned(box24x8))
      {
        BoxI boxI(box24x8.x0 >> 8, box24x8.y0 >> 8, box24x8.x1 >> 8, box24x8.y1 >> 8);
        return engine->doCmd->fillNormalizedBoxI(engine, &boxI);
      }

      BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
      RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

      rasterizer->init24x8(box24x8);
      return RasterPaintDoRender_fillRasterizedShape8(engine, rasterizer);
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::RasterPaintDoRender - FillNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_fillNormalizedPathF(
  RasterPaintEngine* engine, const PathF* path, const PointF* pt, uint32_t fillRule)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      PathRasterizer8* rasterizer = &engine->ctx.pathRasterizer8;
      RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

      rasterizer->setFillRule(fillRule);
      if (FOG_IS_ERROR(rasterizer->init()))
        return rasterizer->getError();

      rasterizer->addPath(*path, *pt);
      rasterizer->finalize();

      if (rasterizer->isValid())
        return RasterPaintDoRender_fillRasterizedShape8(engine, rasterizer);
      else
        return ERR_OK;
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_FASTCALL RasterPaintDoRender_fillNormalizedPathD(
  RasterPaintEngine* engine, const PathD* path, const PointD* pt, uint32_t fillRule)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      PathRasterizer8* rasterizer = &engine->ctx.pathRasterizer8;
      RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

      rasterizer->setFillRule(fillRule);
      if (FOG_IS_ERROR(rasterizer->init()))
        return rasterizer->getError();

      rasterizer->addPath(*path, *pt);
      rasterizer->finalize();

      if (rasterizer->isValid())
        return RasterPaintDoRender_fillRasterizedShape8(engine, rasterizer);
      else
        return ERR_OK;
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::RasterPaintDoRender - BlitImage]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_blitImageD(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform, uint32_t imageQuality)
{
  BoxD boxClipped(*box);
  engine->getFinalTransformD().mapBox(boxClipped, boxClipped);

  if (!BoxD::intersect(boxClipped, boxClipped, engine->getClipBoxD()))
    return ERR_OK;

  RasterPattern* old = engine->ctx.pc;
  RasterPattern pc;

  FOG_RETURN_ON_ERROR(
    _api_raster.texture.create(&pc,
      engine->ctx.target.format,
      &engine->metaClipBoxI,
      srcImage, srcFragment,
      srcTransform, &engine->dummyColor, TEXTURE_TILE_PAD, imageQuality)
  );

  PathD* path = &engine->ctx.tmpPathD[0];
  path->clear();
  path->box(*box);

  PathClipperD clipper(engine->getClipBoxD());
  PathTmpD<32> tmp;

  err_t err = clipper.clipBox(tmp, *box, engine->getFinalTransformD());
  if (err == ERR_OK)
  {
    PointD pt(0.0, 0.0);
    engine->ctx.pc = &pc;
    err = engine->doCmd->fillNormalizedPathD(engine, &tmp, &pt, FILL_RULE_NON_ZERO);
    engine->ctx.pc = old;
  }

  pc.destroy();
  return err;
}

// ============================================================================
// [Fog::RasterPaintDoRender - BlitNormalizedImageA]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_blitNormalizedImageA(
  RasterPaintEngine* engine, const PointI* pt, const Image* srcImage, const RectI* srcFragment)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      // Fast-path (clip-box and full-opacity).
      if (engine->ctx.clipType == RASTER_CLIP_BOX)
      {
        uint8_t* pixels = engine->ctx.target.pixels;
        ssize_t stride = engine->ctx.target.stride;
        uint32_t format = engine->ctx.target.format;

        const ImageData* srcD = srcImage->_d;
        const uint8_t* srcPixels = srcD->first;
        ssize_t srcStride = srcD->stride;
        uint32_t srcFormat = srcD->format;

        uint32_t compositingOperator = engine->ctx.paintHints.compositingOperator;
        uint32_t opacity = engine->ctx.rasterHints.opacity;

        // --------------------------------------------------------------------------
        // [Clip == Box]
        // --------------------------------------------------------------------------

        int srcWidth = srcFragment->w;
        int srcHeight = srcFragment->h;

        int x0 = pt->x;
        int y0 = pt->y;

        int i = srcHeight;
        FOG_ASSERT(y0 + srcHeight <= engine->ctx.target.size.h);

        pixels += y0 * stride;
        srcPixels += srcFragment->y * srcStride;

        if (opacity == 0x100)
        {
          RasterVBlitLineFunc blitLine;

          pixels += x0 * engine->ctx.target.bpp;
          srcPixels += srcFragment->x * srcD->bytesPerPixel;
          engine->ctx.closure.palette = srcD->palette->_d;

          // If compositing operator is SRC or SRC_OVER then any image format
          // combination is supported. However, if compositing operator is one
          // of other values, then only few image formats can be mixed together.
          if (RasterUtil::isCompositeCoreOp(compositingOperator))
          {
            blitLine = _api_raster.getCompositeCore(format, compositingOperator)->vblit_line[srcFormat];

_BlitImageA8_Opaque:
            do {
              blitLine(pixels, srcPixels, srcWidth, &engine->ctx.closure);

              pixels += stride;
              srcPixels += srcStride;
            } while (--i);
          }
          else
          {
            uint32_t vBlitSrc = _raster_compatibleFormat[format][srcFormat].srcFormat;
            uint32_t vBlitId = _raster_compatibleFormat[format][srcFormat].vblitId;

            blitLine = _api_raster.getCompositeExt(format, compositingOperator)->vblit_line[vBlitId];
            if (srcFormat == vBlitSrc)
              goto _BlitImageA8_Opaque;

            uint8_t* tmpPixels = reinterpret_cast<uint8_t*>(engine->ctx.buffer.getMem());
            RasterVBlitLineFunc cvtLine = _api_raster.getCompositeCore(vBlitSrc, COMPOSITE_SRC)->vblit_line[srcFormat];

            do {
              cvtLine(tmpPixels, srcPixels, srcWidth, &engine->ctx.closure);
              blitLine(pixels, tmpPixels, srcWidth, &engine->ctx.closure);

              pixels += stride;
              srcPixels += srcStride;
            } while (--i);
          }

          engine->ctx.closure.palette = NULL;
        }
        else
        {
          RasterVBlitSpanFunc blitSpan;

          RasterSpan8 span[1];
          span[0].setPositionAndType(x0, x0 + srcWidth, RASTER_SPAN_C);
          span[0].setConstMask(opacity);
          span[0].setNext(NULL);

          srcPixels += srcFragment->x * srcD->bytesPerPixel;
          engine->ctx.closure.palette = srcD->palette->_d;

          // If compositing operator is SRC or SRC_OVER then any image format
          // combination is supported. However, if compositing operator is one
          // of other values, then only few image formats can be mixed together.
          if (RasterUtil::isCompositeCoreOp(compositingOperator))
          {
            blitSpan = _api_raster.getCompositeCore(format, compositingOperator)->vblit_span[srcFormat];

_BlitImageA8_Alpha:
            do {
              // SrcPixels won't be changed, it's just needed to remove the const modifier.
              span[0].setData(const_cast<uint8_t*>(srcPixels));
              blitSpan(pixels, span, &engine->ctx.closure);

              pixels += stride;
              srcPixels += srcStride;
            } while (--i);
          }
          else
          {
            uint32_t vBlitSrc = _raster_compatibleFormat[format][srcFormat].srcFormat;
            uint32_t vBlitId = _raster_compatibleFormat[format][srcFormat].vblitId;

            blitSpan = _api_raster.getCompositeExt(format, compositingOperator)->vblit_span[vBlitId];
            if (srcFormat == vBlitSrc)
              goto _BlitImageA8_Alpha;

            uint8_t* tmpPixels = reinterpret_cast<uint8_t*>(engine->ctx.buffer.getMem());
            RasterVBlitLineFunc cvtLine = _api_raster.getCompositeCore(vBlitSrc, COMPOSITE_SRC)->vblit_line[srcFormat];

            span[0].setData(tmpPixels);

            do {
              cvtLine(tmpPixels, srcPixels, srcWidth, &engine->ctx.closure);
              blitSpan(pixels, span, &engine->ctx.closure);

              pixels += stride;
              srcPixels += srcStride;
            } while (--i);
          }

          engine->ctx.closure.palette = NULL;
        }
        return ERR_OK;
      }
      else
      {
        BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
        RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

        BoxI box(pt->x, pt->y, pt->x + srcFragment->w, pt->y + srcFragment->h);
        rasterizer->init32x0(box);

        RasterPattern* old = engine->ctx.pc;
        RasterPattern pc;

        TransformD tr(TransformD::fromTranslation(PointD(*pt)));
        FOG_RETURN_ON_ERROR(
          _api_raster.texture.create(&pc,
            engine->ctx.target.format,
            &engine->metaClipBoxI,
            srcImage, srcFragment,
            &tr, &engine->dummyColor, TEXTURE_TILE_PAD, IMAGE_QUALITY_NEAREST)
        );

        engine->ctx.pc = &pc;
        err_t err = RasterPaintDoRender_fillRasterizedShape8(engine, rasterizer);
        engine->ctx.pc = old;

        pc.destroy();
        return err;
      }
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::RasterPaintDoRender - BlitNormalizedImage]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_blitNormalizedImageI(
  RasterPaintEngine* engine, const BoxI* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform, uint32_t imageQuality)
{
  // Must be already clipped.
  FOG_ASSERT(engine->ctx.clipBoxI.subsumes(*box));

  RasterPattern* old = engine->ctx.pc;
  RasterPattern pc;

  FOG_RETURN_ON_ERROR(
    _api_raster.texture.create(&pc,
      engine->ctx.target.format,
      &engine->metaClipBoxI,
      srcImage, srcFragment,
      srcTransform, &engine->dummyColor, TEXTURE_TILE_PAD, imageQuality)
  );

  engine->ctx.pc = &pc;
  err_t err = engine->doCmd->fillNormalizedBoxI(engine, box);
  engine->ctx.pc = old;

  pc.destroy();
  return err;
}

static err_t FOG_FASTCALL RasterPaintDoRender_blitNormalizedImageD(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform, uint32_t imageQuality)
{
  // Must be already clipped.
  FOG_ASSERT(engine->getClipBoxD().subsumes(*box));

  RasterPattern* old = engine->ctx.pc;
  RasterPattern pc;

  FOG_RETURN_ON_ERROR(
    _api_raster.texture.create(&pc,
      engine->ctx.target.format,
      &engine->metaClipBoxI,
      srcImage, srcFragment,
      srcTransform, &engine->dummyColor, TEXTURE_TILE_PAD, imageQuality)
  );

  engine->ctx.pc = &pc;
  err_t err = engine->doCmd->fillNormalizedBoxD(engine, box);
  engine->ctx.pc = old;

  pc.destroy();
  return err;
}

// ============================================================================
// [Fog::RasterPaintDoRender - FilterRasterizerShape]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_filterRasterizedShape8(RasterPaintEngine* engine, const FeBase* feBase, Rasterizer8* rasterizer, const BoxI* bBox)
{
  // Destination and source formats are the same.
  RasterFilter ctx;
  FOG_RETURN_ON_ERROR(_api_raster.filter.create[feBase->getFeType()](&ctx,
    feBase, &engine->ctx.filterScale,
    &engine->ctx.buffer,
    engine->ctx.target.format,
    engine->ctx.target.format));

  err_t err;

  RasterFilterImage dImage;
  RasterFilterImage sImage;

  PointI dPos(0, 0);
  RectI sRect(bBox->x0, bBox->y0, bBox->x1 - bBox->x0, bBox->y1 - bBox->y0);

  dImage.size.set(sRect.w, sRect.h);
  dImage.stride = 0;
  dImage.data = NULL;

  sImage.size = engine->ctx.target.size;
  sImage.stride = engine->ctx.target.stride;
  sImage.data = engine->ctx.target.pixels;

  uint32_t bpp = engine->ctx.target.bpp;

  MemBuffer intermediateBuffer;
  err = ctx.doRect(&ctx, &dImage, &dPos, &sImage, &sRect, &intermediateBuffer);

  if (FOG_IS_ERROR(err))
  {
    ctx.destroy(&ctx);
    return err;
  }

  RasterPaintFiller filler;

  filler.ctx = &engine->ctx;
  filler.dstPixels = engine->ctx.target.pixels;
  filler.dstStride = engine->ctx.target.stride;

  filler._prepare = (RasterFiller::PrepareFunc)RasterPaintFiller_prepare_filter_st;
  filler._process = (RasterFiller::ProcessFunc)RasterPaintFiller_process_filter;
  filler._skip = (RasterFiller::SkipFunc)RasterPaintFiller_skip_filter;

  uint32_t format = engine->ctx.target.format;
  filler.f.blit = _api_raster.getVBlitSpan(format, COMPOSITE_SRC, format);
  filler.f.closure = &engine->ctx.closure;

  filler.f.srcPixels = dImage.data - bBox->x0 * bpp;
  filler.f.srcStride = dImage.stride;

  filler.f.srcBpp = bpp;
  filler.f.srcBaseY = bBox->y0;

  rasterizer->render(&filler, &engine->ctx.scanline8);

  ctx.destroy(&ctx);
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintDoRender - FilterNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_filterNormalizedBoxI(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxI* box)
{
  FOG_ASSERT(box->isValid());

  // Destination and source formats are the same.
  RasterFilter ctx;
  FOG_RETURN_ON_ERROR(_api_raster.filter.create[feBase->getFeType()](&ctx,
    feBase, &engine->ctx.filterScale,
    &engine->ctx.buffer,
    engine->ctx.target.format,
    engine->ctx.target.format));

  err_t err;

  RasterFilterImage dImage;
  RasterFilterImage sImage;

  sImage.size = engine->ctx.target.size;
  sImage.stride = engine->ctx.target.stride;
  sImage.data = engine->ctx.target.pixels;

  PointI dPos;
  RectI sRect(box->x0, box->y0, box->x1 - box->x0, box->y1 - box->y0);

  uint32_t opacity = engine->ctx.rasterHints.opacity;
  MemBuffer intermediateBuffer;

  if (opacity == engine->ctx.fullOpacity.u)
  {
    // In case that we are painting with full opacity we can render the effect
    // directly to the destination buffer.
    dImage.size = engine->ctx.target.size;
    dImage.stride = engine->ctx.target.stride;
    dImage.data = engine->ctx.target.pixels;
    dPos.set(box->x0, box->y0);

    err = ctx.doRect(&ctx, &dImage, &dPos, &sImage, &sRect, &intermediateBuffer);
  }
  else
  {
    // Render first to the intermediate buffer (see the NULL assignment to the
    // dImage.data) and then composite the pixels into the destination buffer.
    dImage.size.set(sRect.w, sRect.h);
    dImage.stride = 0;
    dImage.data = NULL;

    dPos.set(0, 0);
    err = ctx.doRect(&ctx, &dImage, &dPos, &sImage, &sRect, &intermediateBuffer);
    
    if (err == ERR_OK)
    {
      int i = sRect.h;

      RasterVBlitSpanFunc blitSpan;
      blitSpan = _api_raster.getCompositeCore(engine->ctx.target.format, COMPOSITE_SRC)->vblit_span[engine->ctx.target.format];

      ssize_t dstStride = engine->ctx.target.stride;
      ssize_t srcStride = dImage.stride;

      uint8_t* dstPixels = engine->ctx.target.pixels + box->y0 * srcStride;
      uint8_t* srcPixels = dImage.data;

      FOG_ASSERT(srcPixels != NULL);

      switch (engine->ctx.precision)
      {
        case IMAGE_PRECISION_BYTE:
        {
          RasterSpan8 span[1];
          span[0].setPositionAndType(box->x0, box->x1, RASTER_SPAN_C);
          span[0].setConstMask(opacity);
          span[0].setNext(NULL);

          do {
            span[0].setData(srcPixels);
            blitSpan(dstPixels, span, &engine->ctx.closure);

            dstPixels += dstStride;
            srcPixels += srcStride;
          } while (--i);
          break;
        }

        case IMAGE_PRECISION_WORD:
        {
          // TODO: 16-bit image processing.
          break;
        }
          
        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }
  }

  ctx.destroy(&ctx);
  return err;
}

static err_t FOG_FASTCALL RasterPaintDoRender_filterNormalizedBoxF(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxF* box)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      BoxI box24x8(UNINITIALIZED);
      box24x8.x0 = Math::fixed24x8FromFloat(box->x0);
      box24x8.y0 = Math::fixed24x8FromFloat(box->y0);
      box24x8.x1 = Math::fixed24x8FromFloat(box->x1);
      box24x8.y1 = Math::fixed24x8FromFloat(box->y1);

      if (RasterUtil::isBox24x8Aligned(box24x8))
      {
        BoxI boxI(box24x8.x0 >> 8, box24x8.y0 >> 8, box24x8.x1 >> 8, box24x8.y1 >> 8);
        return engine->doCmd->filterNormalizedBoxI(engine, feBase, &boxI);
      }
      else
      {
        BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
        RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

        rasterizer->init24x8(box24x8);
        return RasterPaintDoRender_filterRasterizedShape8(engine, feBase, rasterizer, &rasterizer->_boxBounds);
      }
    }
    
    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      return ERR_RT_NOT_IMPLEMENTED;
    }
    
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_FASTCALL RasterPaintDoRender_filterNormalizedBoxD(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxD* box)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      BoxI box24x8(UNINITIALIZED);
      box24x8.x0 = Math::fixed24x8FromFloat(box->x0);
      box24x8.y0 = Math::fixed24x8FromFloat(box->y0);
      box24x8.x1 = Math::fixed24x8FromFloat(box->x1);
      box24x8.y1 = Math::fixed24x8FromFloat(box->y1);

      if (RasterUtil::isBox24x8Aligned(box24x8))
      {
        BoxI boxI(box24x8.x0 >> 8, box24x8.y0 >> 8, box24x8.x1 >> 8, box24x8.y1 >> 8);
        return engine->doCmd->filterNormalizedBoxI(engine, feBase, &boxI);
      }
      else
      {
        BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
        RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

        rasterizer->init24x8(box24x8);
        return RasterPaintDoRender_filterRasterizedShape8(engine, feBase, rasterizer, &rasterizer->_boxBounds);
      }
    }
    
    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      return ERR_RT_NOT_IMPLEMENTED;
    }
    
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::RasterPaintDoRender - FilterNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_filterNormalizedPathF(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, const PointF* pt, uint32_t fillRule)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      PathRasterizer8* rasterizer = &engine->ctx.pathRasterizer8;
      RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

      rasterizer->setFillRule(fillRule);
      if (FOG_IS_ERROR(rasterizer->init()))
        return rasterizer->getError();

      rasterizer->addPath(*path, *pt);
      rasterizer->finalize();

      if (rasterizer->isValid())
        return RasterPaintDoRender_filterRasterizedShape8(engine, feBase, rasterizer, &rasterizer->_boundingBox);
      else
        return ERR_OK;
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_FASTCALL RasterPaintDoRender_filterNormalizedPathD(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, const PointD* pt, uint32_t fillRule)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      PathRasterizer8* rasterizer = &engine->ctx.pathRasterizer8;
      RasterPaintDoRender_prepareRasterizer(engine, rasterizer);

      rasterizer->setFillRule(fillRule);
      if (FOG_IS_ERROR(rasterizer->init()))
        return rasterizer->getError();

      rasterizer->addPath(*path, *pt);
      rasterizer->finalize();

      if (rasterizer->isValid())
        return RasterPaintDoRender_filterRasterizedShape8(engine, feBase, rasterizer, &rasterizer->_boundingBox);
      else
        return ERR_OK;
    }

    case IMAGE_PRECISION_WORD:
    {
      // TODO: 16-bit image processing.
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // Dead code to avoid warning.
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::RasterPaintDoRender - SwitchToMask / DiscardMask]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_switchToMask(RasterPaintEngine* engine)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoRender_discardMask(RasterPaintEngine* engine)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoRender - SaveMask / RestoreMask]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_saveMask(RasterPaintEngine* engine)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoRender_restoreMask(RasterPaintEngine* engine)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - MaskNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_maskNormalizedBoxI(RasterPaintEngine* engine, uint32_t clipOp, const BoxI* box)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoRender_maskNormalizedBoxF(RasterPaintEngine* engine, uint32_t clipOp, const BoxF* box)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoRender_maskNormalizedBoxD(RasterPaintEngine* engine, uint32_t clipOp, const BoxD* box)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoGroup - MaskNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintDoRender_maskNormalizedPathF(RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintDoRender_maskNormalizedPathD(RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintDoRender - Init]
// ============================================================================

void FOG_NO_EXPORT RasterPaintDoRender_init(void)
{
  RasterPaintDoCmd* v = &RasterPaintDoRender_vtable[RASTER_MODE_ST];

  // --------------------------------------------------------------------------
  // [Fill/Stroke]
  // --------------------------------------------------------------------------

  v->fillAll = RasterPaintDoRender_fillAll;
  v->fillNormalizedBoxI = RasterPaintDoRender_fillNormalizedBoxI;
  v->fillNormalizedBoxF = RasterPaintDoRender_fillNormalizedBoxF;
  v->fillNormalizedBoxD = RasterPaintDoRender_fillNormalizedBoxD;
  v->fillNormalizedPathF = RasterPaintDoRender_fillNormalizedPathF;
  v->fillNormalizedPathD = RasterPaintDoRender_fillNormalizedPathD;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  v->blitImageD = RasterPaintDoRender_blitImageD;
  v->blitNormalizedImageA = RasterPaintDoRender_blitNormalizedImageA;
  v->blitNormalizedImageI = RasterPaintDoRender_blitNormalizedImageI;
  v->blitNormalizedImageD = RasterPaintDoRender_blitNormalizedImageD;

  // --------------------------------------------------------------------------
  // [Filter]
  // --------------------------------------------------------------------------

  v->filterNormalizedBoxI = RasterPaintDoRender_filterNormalizedBoxI;
  v->filterNormalizedBoxF = RasterPaintDoRender_filterNormalizedBoxF;
  v->filterNormalizedBoxD = RasterPaintDoRender_filterNormalizedBoxD;
  v->filterNormalizedPathF = RasterPaintDoRender_filterNormalizedPathF;
  v->filterNormalizedPathD = RasterPaintDoRender_filterNormalizedPathD;

  // --------------------------------------------------------------------------
  // [Mask]
  // --------------------------------------------------------------------------

  v->switchToMask = RasterPaintDoRender_switchToMask;
  v->discardMask = RasterPaintDoRender_discardMask;
  v->saveMask = RasterPaintDoRender_saveMask;
  v->restoreMask = RasterPaintDoRender_restoreMask;

  v->maskNormalizedBoxI = RasterPaintDoRender_maskNormalizedBoxI;
  v->maskNormalizedBoxF = RasterPaintDoRender_maskNormalizedBoxF;
  v->maskNormalizedBoxD = RasterPaintDoRender_maskNormalizedBoxD;
  v->maskNormalizedPathF = RasterPaintDoRender_maskNormalizedPathF;
  v->maskNormalizedPathD = RasterPaintDoRender_maskNormalizedPathD;
}

} // Fog namespace
