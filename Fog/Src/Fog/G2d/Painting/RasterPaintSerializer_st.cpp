// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/FaceC.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Imaging/Filters/FeBase.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintEngine_p.h>
#include <Fog/G2d/Painting/RasterPaintSerializer_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterState_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintSerializer - VTable]
// ============================================================================

FOG_NO_EXPORT RasterPaintSerializer RasterPaintSerializer_vtable[RASTER_MODE_COUNT];

// ============================================================================
// [Fog::RasterPaintSerializer - Defs]
// ============================================================================

#define _FOG_RASTER_ENSURE_PATTERN(_Engine_) \
  FOG_MACRO_BEGIN \
    if (_Engine_->ctx.pc == NULL) \
    { \
      FOG_RETURN_ON_ERROR(_Engine_->createPatternContext()); \
    } \
  FOG_MACRO_END

// ============================================================================
// [Fog::RasterPaintSerializer - Filler - Declaration]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintFiller : public RasterFiller
{
  RasterContext* ctx;

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
// [Fog::RasterPaintSerializer - Filler - Solid]
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
// [Fog::RasterPaintSerializer - Filler - Pattern]
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
// [Fog::RasterPaintSerializer - Filler - Filter]
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
// [Fog::RasterPaintSerializer - PrepareRasterizer (st)]
// ============================================================================

static void FOG_INLINE RasterPaintSerializer_prepareRasterizer_st(RasterPaintEngine* engine, Rasterizer8* rasterizer)
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
// [Fog::RasterPaintSerializer - FillRasterizedShape (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillRasterizedShape8_st(RasterPaintEngine* engine, Rasterizer8* rasterizer)
{
  RasterPaintFiller filler;

  uint8_t* dstPixels = engine->ctx.layer.pixels;
  ssize_t dstStride = engine->ctx.layer.stride;
  uint32_t dstFormat = engine->ctx.layer.primaryFormat;
  uint32_t compositingOperator = engine->ctx.paintHints.compositingOperator;

  filler.ctx = &engine->ctx;
  filler.dstPixels = dstPixels;
  filler.dstStride = dstStride;

  if (RasterUtil::isSolidContext(engine->ctx.pc) || compositingOperator == COMPOSITE_CLEAR)
  {
_Solid:
    bool isSrcOpaque = Face::p32PRGB32IsAlphaFF(engine->ctx.solid.prgb32.u32);

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
// [Fog::RasterPaintSerializer - FillAll (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillAll_st(
  RasterPaintEngine* engine)
{
  return engine->serializer->fillNormalizedBoxI(engine, &engine->ctx.clipBoxI);
}

// ============================================================================
// [Fog::RasterPaintSerializer - FillPath (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillPathF_st(
  RasterPaintEngine* engine, const PathF* path, uint32_t fillRule)
{
  PathF& tmp = engine->ctx.tmpPathF[1];

  bool hasTransform = engine->ensureFinalTransformF();
  PathClipperF clipper(engine->getClipBoxF());

  if (!hasTransform)
  {
    switch (clipper.measurePath(*path))
    {
      case PATH_CLIPPER_MEASURE_BOUNDED:
        engine->serializer->fillNormalizedPathF(engine, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_MEASURE_UNBOUNDED:
        tmp.clear();
        FOG_RETURN_ON_ERROR(clipper.continuePath(tmp, *path));
        engine->serializer->fillNormalizedPathF(engine, &tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp.clear();
    FOG_RETURN_ON_ERROR(clipper.clipPath(tmp, *path, engine->getFinalTransformF()));
    engine->serializer->fillNormalizedPathF(engine, &tmp, fillRule);
    return ERR_OK;
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillPathD_st(
  RasterPaintEngine* engine, const PathD* path, uint32_t fillRule)
{
  PathD* tmp = &engine->ctx.tmpPathD[1];

  bool hasTransform = (engine->getFinalTransformD()._getType() != TRANSFORM_TYPE_IDENTITY);
  PathClipperD clipper(engine->getClipBoxD());

  if (!hasTransform)
  {
    switch (clipper.measurePath(*path))
    {
      case PATH_CLIPPER_MEASURE_BOUNDED:
        engine->serializer->fillNormalizedPathD(engine, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_MEASURE_UNBOUNDED:
        tmp->clear();
        FOG_RETURN_ON_ERROR(clipper.continuePath(*tmp, *path));
        engine->serializer->fillNormalizedPathD(engine, tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp->clear();
    FOG_RETURN_ON_ERROR(clipper.clipPath(*tmp, *path, engine->getFinalTransformD()));
    engine->serializer->fillNormalizedPathD(engine, tmp, fillRule);
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::RasterPaintSerializer - StrokeAndFillPath (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillStrokedPathF_st(
  RasterPaintEngine* engine, const PathF* path)
{
  if (!engine->ctx.rasterHints.finalTransformF)
  {
    if (engine->getFinalTransformD()._getType() != TRANSFORM_TYPE_IDENTITY)
    {
      engine->stroker.f->_transform->setTransform(engine->stroker.d->getTransform());
      engine->ctx.rasterHints.finalTransformF = 1;
    }
    else
    {
      engine->stroker.f->_transform->reset();
    }
    engine->stroker.f->_isDirty = true;
  }

  if (engine->strokerPrecision == RASTER_PRECISION_D)
  {
    engine->strokerPrecision = RASTER_PRECISION_BOTH;
    engine->stroker.f->_params() = engine->stroker.d->_params();
  }

  PathStrokerF& stroker = engine->stroker.f;
  PathF& tmp = engine->ctx.tmpPathF[0];

  tmp.clear();
  FOG_RETURN_ON_ERROR(stroker.strokePath(tmp, *path));

  return engine->serializer->fillNormalizedPathF(engine, &tmp, FILL_RULE_NON_ZERO);
}

static err_t FOG_FASTCALL RasterPaintSerializer_fillStrokedPathD_st(
  RasterPaintEngine* engine, const PathD* path)
{
  if (engine->strokerPrecision == RASTER_PRECISION_F)
  {
    engine->strokerPrecision = RASTER_PRECISION_BOTH;
    engine->stroker.d->_params() = engine->stroker.f->_params();
    engine->stroker.d->_isDirty = true;
  }

  PathStrokerD& stroker = engine->stroker.d;
  PathD& tmp = engine->ctx.tmpPathD[0];

  tmp.clear();
  FOG_RETURN_ON_ERROR(stroker.strokePath(tmp, *path));

  return engine->serializer->fillNormalizedPathD(engine, &tmp, FILL_RULE_NON_ZERO);
}

// ============================================================================
// [Fog::RasterPaintSerializer - FillNormalizedBox (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedBoxI_st(
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
        uint8_t* dstPixels = engine->ctx.layer.pixels;
        ssize_t dstStride = engine->ctx.layer.stride;
        uint32_t dstFormat = engine->ctx.layer.primaryFormat;
        uint32_t compositingOperator = engine->ctx.paintHints.compositingOperator;

        int y0 = box->y0;

        int w = box->x1 - box->x0;
        int i = box->y1 - box->y0;

        dstPixels += y0 * dstStride;

        if (RasterUtil::isSolidContext(engine->ctx.pc) || compositingOperator == COMPOSITE_CLEAR)
        {
_Solid:
          bool isSrcOpaque = Face::p32PRGB32IsAlphaFF(engine->ctx.solid.prgb32.u32);
          RasterCBlitLineFunc blitLine = _api_raster.getCBlitLine(dstFormat, compositingOperator, isSrcOpaque);

          dstPixels += box->x0 * engine->ctx.layer.primaryBPP;
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

            dstPixels += box->x0 * engine->ctx.layer.primaryBPP;
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

            dstPixels += box->x0 * engine->ctx.layer.primaryBPP;

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
        RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

        rasterizer->init32x0(*box);
        return RasterPaintSerializer_fillRasterizedShape8_st(engine, rasterizer);
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

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedBoxF_st(
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
        return engine->serializer->fillNormalizedBoxI(engine, &boxI);
      }

      BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
      RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

      rasterizer->init24x8(box24x8);
      return RasterPaintSerializer_fillRasterizedShape8_st(engine, rasterizer);
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

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedBoxD_st(
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
        return engine->serializer->fillNormalizedBoxI(engine, &boxI);
      }

      BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
      RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

      rasterizer->init24x8(box24x8);
      return RasterPaintSerializer_fillRasterizedShape8_st(engine, rasterizer);
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
// [Fog::RasterPaintSerializer - FillNormalizedPath (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedPathF_st(
  RasterPaintEngine* engine, const PathF* path, uint32_t fillRule)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      PathRasterizer8* rasterizer = &engine->ctx.pathRasterizer8;
      RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

      rasterizer->setFillRule(fillRule);
      if (FOG_IS_ERROR(rasterizer->init()))
        return rasterizer->getError();

      rasterizer->addPath(*path);
      rasterizer->finalize();

      if (rasterizer->isValid())
        return RasterPaintSerializer_fillRasterizedShape8_st(engine, rasterizer);
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

static err_t FOG_FASTCALL RasterPaintSerializer_fillNormalizedPathD_st(
  RasterPaintEngine* engine, const PathD* path, uint32_t fillRule)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      PathRasterizer8* rasterizer = &engine->ctx.pathRasterizer8;
      RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

      rasterizer->setFillRule(fillRule);
      if (FOG_IS_ERROR(rasterizer->init()))
        return rasterizer->getError();

      rasterizer->addPath(*path);
      rasterizer->finalize();

      if (rasterizer->isValid())
        return RasterPaintSerializer_fillRasterizedShape8_st(engine, rasterizer);
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
// [Fog::RasterPaintSerializer - BlitImage (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_blitImageD_st(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform)
{
  BoxD boxClipped(*box);
  engine->getFinalTransformD().mapBox(boxClipped, boxClipped);

  if (!BoxD::intersect(boxClipped, boxClipped, engine->getClipBoxD()))
    return ERR_OK;

  RasterPattern* old = engine->ctx.pc;
  RasterPattern pc;

  FOG_RETURN_ON_ERROR(
    _api_raster.texture.create(&pc,
      engine->ctx.layer.primaryFormat,
      &engine->metaClipBoxI,
      srcImage, srcFragment,
      srcTransform, &engine->dummyColor, TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
  );

  PathD* path = &engine->ctx.tmpPathD[0];
  path->clear();
  path->box(*box);

  engine->ctx.pc = &pc;
  err_t err = engine->serializer->fillPathD(engine, path, FILL_RULE_NON_ZERO);
  engine->ctx.pc = old;

  pc.destroy();
  return err;
}

// ============================================================================
// [Fog::RasterPaintSerializer - BlitNormalizedImageA (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_blitNormalizedImageA_st(
  RasterPaintEngine* engine, const PointI* pt, const Image* srcImage, const RectI* srcFragment)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      // Fast-path (clip-box and full-opacity).
      if (engine->ctx.clipType == RASTER_CLIP_BOX)
      {
        uint8_t* pixels = engine->ctx.layer.pixels;
        ssize_t stride = engine->ctx.layer.stride;
        uint32_t format = engine->ctx.layer.primaryFormat;

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
        FOG_ASSERT(y0 + srcHeight <= engine->ctx.layer.size.h);

        pixels += y0 * stride;
        srcPixels += srcFragment->y * srcStride;

        if (opacity == 0x100)
        {
          RasterVBlitLineFunc blitLine;

          pixels += x0 * engine->ctx.layer.primaryBPP;
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
        RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

        BoxI box(pt->x, pt->y, pt->x + srcFragment->w, pt->y + srcFragment->h);
        rasterizer->init32x0(box);

        RasterPattern* old = engine->ctx.pc;
        RasterPattern pc;

        TransformD tr(TransformD::fromTranslation(PointD(*pt)));
        FOG_RETURN_ON_ERROR(
          _api_raster.texture.create(&pc,
            engine->ctx.layer.primaryFormat,
            &engine->metaClipBoxI,
            srcImage, srcFragment,
            &tr, &engine->dummyColor, TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
        );

        engine->ctx.pc = &pc;
        err_t err = RasterPaintSerializer_fillRasterizedShape8_st(engine, rasterizer);
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
// [Fog::RasterPaintSerializer - BlitNormalizedImage (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_blitNormalizedImageI_st(
  RasterPaintEngine* engine, const BoxI* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform)
{
  // Must be already clipped.
  FOG_ASSERT(engine->ctx.clipBoxI.subsumes(*box));

  RasterPattern* old = engine->ctx.pc;
  RasterPattern pc;

  FOG_RETURN_ON_ERROR(
    _api_raster.texture.create(&pc,
      engine->ctx.layer.primaryFormat,
      &engine->metaClipBoxI,
      srcImage, srcFragment,
      srcTransform, &engine->dummyColor, TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
  );

  engine->ctx.pc = &pc;
  err_t err = engine->serializer->fillNormalizedBoxI(engine, box);
  engine->ctx.pc = old;

  pc.destroy();
  return err;
}

static err_t FOG_FASTCALL RasterPaintSerializer_blitNormalizedImageD_st(
  RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform)
{
  // Must be already clipped.
  FOG_ASSERT(engine->getClipBoxD().subsumes(*box));

  RasterPattern* old = engine->ctx.pc;
  RasterPattern pc;

  FOG_RETURN_ON_ERROR(
    _api_raster.texture.create(&pc,
      engine->ctx.layer.primaryFormat,
      &engine->metaClipBoxI,
      srcImage, srcFragment,
      srcTransform, &engine->dummyColor, TEXTURE_TILE_PAD, engine->ctx.paintHints.imageQuality)
  );

  engine->ctx.pc = &pc;
  err_t err = engine->serializer->fillNormalizedBoxD(engine, box);
  engine->ctx.pc = old;

  pc.destroy();
  return err;
}

// ============================================================================
// [Fog::RasterPaintSerializer - FilterPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_filterPathF_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule)
{
  PathF* tmp = &engine->ctx.tmpPathF[1];

  bool hasTransform = engine->ensureFinalTransformF();
  PathClipperF clipper(engine->getClipBoxF());

  if (!hasTransform)
  {
    switch (clipper.measurePath(*path))
    {
      case PATH_CLIPPER_MEASURE_BOUNDED:
        engine->serializer->filterNormalizedPathF(engine, feBase, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_MEASURE_UNBOUNDED:
        tmp->clear();
        FOG_RETURN_ON_ERROR(clipper.continuePath(*tmp, *path));
        engine->serializer->filterNormalizedPathF(engine, feBase, tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp->clear();
    FOG_RETURN_ON_ERROR(clipper.clipPath(*tmp, *path, engine->getFinalTransformF()));
    engine->serializer->filterNormalizedPathF(engine, feBase, tmp, fillRule);
    return ERR_OK;
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_filterPathD_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule)
{
  PathD* tmp = &engine->ctx.tmpPathD[1];

  bool hasTransform = (engine->getFinalTransformD()._getType() != TRANSFORM_TYPE_IDENTITY);
  PathClipperD clipper(engine->getClipBoxD());

  if (!hasTransform)
  {
    switch (clipper.measurePath(*path))
    {
      case PATH_CLIPPER_MEASURE_BOUNDED:
        engine->serializer->filterNormalizedPathD(engine, feBase, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_MEASURE_UNBOUNDED:
        tmp->clear();
        FOG_RETURN_ON_ERROR(clipper.continuePath(*tmp, *path));
        engine->serializer->filterNormalizedPathD(engine, feBase, tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp->clear();
    FOG_RETURN_ON_ERROR(clipper.clipPath(*tmp, *path, engine->getFinalTransformD()));
    engine->serializer->filterNormalizedPathD(engine, feBase, tmp, fillRule);
    return ERR_OK;
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_filterStrokedPathF_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path)
{
  if (!engine->ctx.rasterHints.finalTransformF)
  {
    if (engine->getFinalTransformD()._getType() != TRANSFORM_TYPE_IDENTITY)
    {
      engine->stroker.f->_transform->setTransform(engine->stroker.d->getTransform());
      engine->ctx.rasterHints.finalTransformF = 1;
    }
    else
    {
      engine->stroker.f->_transform->reset();
    }
    engine->stroker.f->_isDirty = true;
  }

  if (engine->strokerPrecision == RASTER_PRECISION_D)
  {
    engine->strokerPrecision = RASTER_PRECISION_BOTH;
    engine->stroker.f->_params() = engine->stroker.d->_params();
  }

  PathStrokerF& stroker = engine->stroker.f;
  PathF& tmp = engine->ctx.tmpPathF[0];

  tmp.clear();
  FOG_RETURN_ON_ERROR(stroker.strokePath(tmp, *path));

  return engine->serializer->filterNormalizedPathF(engine, feBase, &tmp, FILL_RULE_NON_ZERO);
}

static err_t FOG_FASTCALL RasterPaintSerializer_filterStrokedPathD_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path)
{
  if (engine->strokerPrecision == RASTER_PRECISION_F)
  {
    engine->strokerPrecision = RASTER_PRECISION_BOTH;
    engine->stroker.d->_params() = engine->stroker.f->_params();
    engine->stroker.d->_isDirty = true;
  }

  PathStrokerD& stroker = engine->stroker.d;
  PathD& tmp = engine->ctx.tmpPathD[0];

  tmp.clear();
  FOG_RETURN_ON_ERROR(stroker.strokePath(tmp, *path));

  return engine->serializer->filterNormalizedPathD(engine, feBase, &tmp, FILL_RULE_NON_ZERO);
}

// ============================================================================
// [Fog::RasterPaintSerializer - FilterRasterizerShape]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_filterRasterizedShape8_st(RasterPaintEngine* engine, const FeBase* feBase, Rasterizer8* rasterizer, const BoxI* bBox)
{
  // Destination and source formats are the same.
  RasterFilter ctx;
  FOG_RETURN_ON_ERROR(_api_raster.filter.create[feBase->getFeType()](&ctx,
    feBase, &engine->ctx.filterScale,
    &engine->ctx.buffer,
    engine->ctx.layer.primaryFormat,
    engine->ctx.layer.primaryFormat));

  err_t err;

  RasterFilterImage dImage;
  RasterFilterImage sImage;

  PointI dPos(0, 0);
  RectI sRect(bBox->x0, bBox->y0, bBox->x1 - bBox->x0, bBox->y1 - bBox->y0);

  dImage.size.set(sRect.w, sRect.h);
  dImage.stride = 0;
  dImage.data = NULL;

  sImage.size = engine->ctx.layer.size;
  sImage.stride = engine->ctx.layer.stride;
  sImage.data = engine->ctx.layer.pixels;

  uint32_t bpp = engine->ctx.layer.primaryBPP;

  MemBuffer intermediateBuffer;
  err = ctx.doRect(&ctx, &dImage, &dPos, &sImage, &sRect, &intermediateBuffer);

  if (FOG_IS_ERROR(err))
  {
    ctx.destroy(&ctx);
    return err;
  }

  RasterPaintFiller filler;

  filler.ctx = &engine->ctx;
  filler.dstPixels = engine->ctx.layer.pixels;
  filler.dstStride = engine->ctx.layer.stride;

  filler._prepare = (RasterFiller::PrepareFunc)RasterPaintFiller_prepare_filter_st;
  filler._process = (RasterFiller::ProcessFunc)RasterPaintFiller_process_filter;
  filler._skip = (RasterFiller::SkipFunc)RasterPaintFiller_skip_filter;

  uint32_t format = engine->ctx.layer.primaryFormat;
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
// [Fog::RasterPaintSerializer - FilterNormalizedBox]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedBoxI_st(
  RasterPaintEngine* engine, const FeBase* feBase, const BoxI* box)
{
  FOG_ASSERT(box->isValid());

  // Destination and source formats are the same.
  RasterFilter ctx;
  FOG_RETURN_ON_ERROR(_api_raster.filter.create[feBase->getFeType()](&ctx,
    feBase, &engine->ctx.filterScale,
    &engine->ctx.buffer,
    engine->ctx.layer.primaryFormat,
    engine->ctx.layer.primaryFormat));

  err_t err;

  RasterFilterImage dImage;
  RasterFilterImage sImage;

  sImage.size = engine->ctx.layer.size;
  sImage.stride = engine->ctx.layer.stride;
  sImage.data = engine->ctx.layer.pixels;

  PointI dPos;
  RectI sRect(box->x0, box->y0, box->x1 - box->x0, box->y1 - box->y0);

  uint32_t opacity = engine->ctx.rasterHints.opacity;
  MemBuffer intermediateBuffer;

  if (opacity == engine->ctx.fullOpacity.u)
  {
    // In case that we are painting with full opacity we can render the effect
    // directly to the destination buffer.
    dImage.size = engine->ctx.layer.size;
    dImage.stride = engine->ctx.layer.stride;
    dImage.data = engine->ctx.layer.pixels;
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
      blitSpan = _api_raster.getCompositeCore(engine->ctx.layer.primaryFormat, COMPOSITE_SRC)->vblit_span[engine->ctx.layer.primaryFormat];

      ssize_t dstStride = engine->ctx.layer.stride;
      ssize_t srcStride = dImage.stride;

      uint8_t* dstPixels = engine->ctx.layer.pixels + box->y0 * srcStride;
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

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedBoxF_st(
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
        return engine->serializer->filterNormalizedBoxI(engine, feBase, &boxI);
      }
      else
      {
        BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
        RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

        rasterizer->init24x8(box24x8);
        return RasterPaintSerializer_filterRasterizedShape8_st(engine, feBase, rasterizer, &rasterizer->_boxBounds);
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

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedBoxD_st(
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
        return engine->serializer->filterNormalizedBoxI(engine, feBase, &boxI);
      }
      else
      {
        BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
        RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

        rasterizer->init24x8(box24x8);
        return RasterPaintSerializer_filterRasterizedShape8_st(engine, feBase, rasterizer, &rasterizer->_boxBounds);
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
// [Fog::RasterPaintSerializer - FilterNormalizedPath]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedPathF_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      PathRasterizer8* rasterizer = &engine->ctx.pathRasterizer8;
      RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

      rasterizer->setFillRule(fillRule);
      if (FOG_IS_ERROR(rasterizer->init()))
        return rasterizer->getError();

      rasterizer->addPath(*path);
      rasterizer->finalize();

      if (rasterizer->isValid())
        return RasterPaintSerializer_filterRasterizedShape8_st(engine, feBase, rasterizer, &rasterizer->_boundingBox);
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

static err_t FOG_FASTCALL RasterPaintSerializer_filterNormalizedPathD_st(
  RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule)
{
  switch (engine->ctx.precision)
  {
    case IMAGE_PRECISION_BYTE:
    {
      PathRasterizer8* rasterizer = &engine->ctx.pathRasterizer8;
      RasterPaintSerializer_prepareRasterizer_st(engine, rasterizer);

      rasterizer->setFillRule(fillRule);
      if (FOG_IS_ERROR(rasterizer->init()))
        return rasterizer->getError();

      rasterizer->addPath(*path);
      rasterizer->finalize();

      if (rasterizer->isValid())
        return RasterPaintSerializer_filterRasterizedShape8_st(engine, feBase, rasterizer, &rasterizer->_boundingBox);
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
// [Fog::RasterPaintSerializer - ClipAll (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipAll_st(
  RasterPaintEngine* engine)
{
  if ((engine->savedStateFlags & RASTER_STATE_CLIPPING) == 0)
    engine->saveClipping();

  engine->masterFlags |= RASTER_NO_PAINT_USER_CLIP;

  engine->ctx.clipBoxI.reset();
  engine->ctx.clipRegion.clear();
  engine->stroker.f->_clipBox.reset();
  engine->stroker.d->_clipBox.reset();

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintSerializer - ClipPath (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipPathF(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  PathF* tmp = &engine->ctx.tmpPathF[1];

  bool hasTransform = engine->ensureFinalTransformF();
  PathClipperF clipper(clipOp == CLIP_OP_REPLACE ? engine->getMetaClipBoxF() : engine->getClipBoxF());

  if (!hasTransform)
  {
    switch (clipper.measurePath(*path))
    {
      case PATH_CLIPPER_MEASURE_BOUNDED:
        engine->serializer->clipNormalizedPathF(engine, clipOp, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_MEASURE_UNBOUNDED:
        tmp->clear();
        FOG_RETURN_ON_ERROR(clipper.continuePath(*tmp, *path));
        engine->serializer->clipNormalizedPathF(engine, clipOp, tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp->clear();
    FOG_RETURN_ON_ERROR(clipper.clipPath(*tmp, *path, engine->getFinalTransformF()));
    engine->serializer->clipNormalizedPathF(engine, clipOp, tmp, fillRule);
    return ERR_OK;
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipPathD(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule)
{
  PathD* tmp = &engine->ctx.tmpPathD[1];

  bool hasTransform = (engine->getFinalTransformD()._getType() != TRANSFORM_TYPE_IDENTITY);
  PathClipperD clipper(clipOp == CLIP_OP_REPLACE ? engine->metaClipBoxD : engine->getClipBoxD());

  if (!hasTransform)
  {
    switch (clipper.measurePath(*path))
    {
      case PATH_CLIPPER_MEASURE_BOUNDED:
        engine->serializer->clipNormalizedPathD(engine, clipOp, path, fillRule);
        return ERR_OK;

      case PATH_CLIPPER_MEASURE_UNBOUNDED:
        tmp->clear();
        FOG_RETURN_ON_ERROR(clipper.continuePath(*tmp, *path));
        engine->serializer->clipNormalizedPathD(engine, clipOp, tmp, fillRule);
        return ERR_OK;

      default:
        return ERR_GEOMETRY_INVALID;
    }
  }
  else
  {
    tmp->clear();
    FOG_RETURN_ON_ERROR(clipper.clipPath(*tmp, *path, engine->getFinalTransformD()));
    engine->serializer->clipNormalizedPathD(engine, clipOp, tmp, fillRule);
    return ERR_OK;
  }
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipStrokedPathF(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path)
{
  if (engine->strokerPrecision == RASTER_PRECISION_D)
  {
    engine->strokerPrecision = RASTER_PRECISION_BOTH;
    engine->stroker.f->_params() = engine->stroker.d->_params();
    engine->stroker.f->_isDirty = true;
  }

  PathStrokerF* stroker = &engine->stroker.f;
  PathF* tmp = &engine->ctx.tmpPathF[0];

  tmp->clear();
  stroker->strokePath(*tmp, *path);
  return engine->serializer->clipNormalizedPathF(engine, clipOp, tmp, FILL_RULE_NON_ZERO);
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipStrokedPathD(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path)
{
  if (engine->strokerPrecision == RASTER_PRECISION_F)
  {
    engine->strokerPrecision = RASTER_PRECISION_BOTH;
    engine->stroker.d->_params() = engine->stroker.f->_params();
    engine->stroker.d->_isDirty = true;
  }

  PathStrokerD* stroker = &engine->stroker.d;
  PathD* tmp = &engine->ctx.tmpPathD[0];

  tmp->clear();
  stroker->strokePath(*tmp, *path);
  return engine->serializer->clipNormalizedPathD(engine, clipOp, tmp, FILL_RULE_NON_ZERO);
}

// ============================================================================
// [Fog::RasterPaintSerializer - ClipNormalizedBox (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedBoxI_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxI* box)
{
  FOG_ASSERT(box->isValid());

  if ((engine->savedStateFlags & RASTER_STATE_CLIPPING) == 0)
    engine->saveClipping();

  Region* newRegion;
  size_t newLength;

  switch (clipOp)
  {
    case CLIP_OP_REPLACE:
      switch (engine->ctx.clipType)
      {
        case RASTER_CLIP_BOX:
          // RasterPaintEngine always clips only to clip-box, if there is a
          // meta-region then we have to clip manually.
_ReplaceTryMeta:
          if (engine->metaRegion.getLength() > 1)
          {
            newRegion = engine->getTemporaryRegion();
            FOG_RETURN_ON_ERROR(Region::intersect(*newRegion, engine->metaRegion, *box));

            newLength = newRegion->getLength();
            box = &newRegion->_d->boundingBox;

            if (newLength == 0)
              return engine->serializer->clipAll(engine);

            if (newLength > 1)
              goto _ReplaceClipRegion;
          }

_ReplaceClipBox:
          engine->masterFlags &= ~RASTER_NO_PAINT_USER_CLIP;

          engine->ctx.clipType = RASTER_CLIP_BOX;
          engine->ctx.clipBoxI = *box;
          engine->stroker.f->_clipBox.setBox(*box);
          engine->stroker.d->_clipBox.setBox(*box);
          return ERR_OK;

        case RASTER_CLIP_REGION:
          // Not used anymore.
          engine->ctx.clipRegion.clear();
          goto _ReplaceTryMeta;

        case RASTER_CLIP_MASK:
          // TODO: RasterPaintEngine - clip-mask.
          goto _ReplaceTryMeta;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;

    case CLIP_OP_INTERSECT:
      switch (engine->ctx.clipType)
      {
        case RASTER_CLIP_BOX:
          // ClipNormalizedBox is always called with already clipped box to
          // the current clip-box or meta clip-box. This means that we don't
          // need to clip it again.
          FOG_ASSERT(engine->ctx.clipBoxI.subsumes(*box));
          goto _ReplaceClipBox;

        case RASTER_CLIP_REGION:
          // Just assure that there is something in current clipRegion, 
          // because clipping is set to RASTER_CLIP_REGION.
          FOG_ASSERT(engine->ctx.clipRegion.getLength() > 1);

          newRegion = engine->getTemporaryRegion();
          FOG_RETURN_ON_ERROR(Region::intersect(*newRegion, engine->ctx.clipRegion, *box));

          newLength = newRegion->getLength();
          box = &newRegion->_d->boundingBox;

          // If the result is empty region, then everything is clipped (and
          // painting is temporary disabled).
          if (newLength == 0)
            return engine->serializer->clipAll(engine);

          // If the result is single rectangle, then we switch to different
          // clip-type (RASTER_CLIP_BOX), which is simpler and much faster.
          if (newLength == 1)
            goto _ReplaceClipBox;

_ReplaceClipRegion:
          // We use swap to prevent old clipRegion to be deallocated. It's
          // likely that it will be used again.
          swap(engine->ctx.clipRegion, *newRegion);

          // And now we have to update all clip-boxes.
          engine->ctx.clipType = RASTER_CLIP_REGION;
          engine->ctx.clipBoxI = *box;
          engine->stroker.f->_clipBox.setBox(engine->ctx.clipBoxI);
          engine->stroker.d->_clipBox.setBox(engine->ctx.clipBoxI);
          return ERR_OK;

        case RASTER_CLIP_MASK:
          // TODO: RasterPaintEngine - clip-mask.
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedBoxF_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxF* box)
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
        return engine->serializer->clipNormalizedBoxI(engine, clipOp, &boxI);
      }

      BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
      rasterizer->setSceneBox(engine->ctx.clipBoxI);
      rasterizer->setOpacity(0x100);

      rasterizer->init24x8(box24x8);
      // TODO:
      // return RasterPaintSerializer_filterRasterizedShape8_st(engine, rasterizer, &rasterizer->_boxBounds);
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

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedBoxD_st(
  RasterPaintEngine* engine, uint32_t clipOp, const BoxD* box)
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
        return engine->serializer->clipNormalizedBoxI(engine, clipOp, &boxI);
      }

      BoxRasterizer8* rasterizer = &engine->ctx.boxRasterizer8;
      rasterizer->setSceneBox(engine->ctx.clipBoxI);
      rasterizer->setOpacity(0x100);

      rasterizer->init24x8(box24x8);
      // TODO:
      // return RasterPaintSerializer_filterRasterizedShape8_st(engine, rasterizer, &rasterizer->_boxBounds);
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
// [Fog::RasterPaintSerializer - ClipNormalizedPath (st)]
// ============================================================================

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedPathF_st(
  RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_FASTCALL RasterPaintSerializer_clipNormalizedPathD_st(
  RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule)
{
  // TODO: Raster paint-engine.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::RasterPaintSerializer - Init (st)]
// ============================================================================

void FOG_NO_EXPORT RasterPaintSerializer_init_st(void)
{
  RasterPaintSerializer* s = &RasterPaintSerializer_vtable[RASTER_MODE_ST];

  // --------------------------------------------------------------------------
  // [Fill/Stroke]
  // --------------------------------------------------------------------------

  s->fillAll = RasterPaintSerializer_fillAll_st;

  s->fillPathF = RasterPaintSerializer_fillPathF_st;
  s->fillPathD = RasterPaintSerializer_fillPathD_st;

  s->fillStrokedPathF = RasterPaintSerializer_fillStrokedPathF_st;
  s->fillStrokedPathD = RasterPaintSerializer_fillStrokedPathD_st;

  s->fillNormalizedBoxI = RasterPaintSerializer_fillNormalizedBoxI_st;
  s->fillNormalizedBoxF = RasterPaintSerializer_fillNormalizedBoxF_st;
  s->fillNormalizedBoxD = RasterPaintSerializer_fillNormalizedBoxD_st;
  s->fillNormalizedPathF = RasterPaintSerializer_fillNormalizedPathF_st;
  s->fillNormalizedPathD = RasterPaintSerializer_fillNormalizedPathD_st;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  s->blitImageD = RasterPaintSerializer_blitImageD_st;

  s->blitNormalizedImageA = RasterPaintSerializer_blitNormalizedImageA_st;
  s->blitNormalizedImageI = RasterPaintSerializer_blitNormalizedImageI_st;
  s->blitNormalizedImageD = RasterPaintSerializer_blitNormalizedImageD_st;

  // --------------------------------------------------------------------------
  // [Filter]
  // --------------------------------------------------------------------------

  s->filterPathF = RasterPaintSerializer_filterPathF_st;
  s->filterPathD = RasterPaintSerializer_filterPathD_st;

  s->filterStrokedPathF = RasterPaintSerializer_filterStrokedPathF_st;
  s->filterStrokedPathD = RasterPaintSerializer_filterStrokedPathD_st;

  s->filterNormalizedBoxI = RasterPaintSerializer_filterNormalizedBoxI_st;
  s->filterNormalizedBoxF = RasterPaintSerializer_filterNormalizedBoxF_st;
  s->filterNormalizedBoxD = RasterPaintSerializer_filterNormalizedBoxD_st;
  s->filterNormalizedPathF = RasterPaintSerializer_filterNormalizedPathF_st;
  s->filterNormalizedPathD = RasterPaintSerializer_filterNormalizedPathD_st;

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  s->clipAll = RasterPaintSerializer_clipAll_st;

  s->clipPathF = RasterPaintSerializer_clipPathF;
  s->clipPathD = RasterPaintSerializer_clipPathD;

  s->clipStrokedPathF = RasterPaintSerializer_clipStrokedPathF;
  s->clipStrokedPathD = RasterPaintSerializer_clipStrokedPathD;

  s->clipNormalizedBoxI = RasterPaintSerializer_clipNormalizedBoxI_st;
  s->clipNormalizedBoxF = RasterPaintSerializer_clipNormalizedBoxF_st;
  s->clipNormalizedBoxD = RasterPaintSerializer_clipNormalizedBoxD_st;
  s->clipNormalizedPathF = RasterPaintSerializer_clipNormalizedPathF_st;
  s->clipNormalizedPathD = RasterPaintSerializer_clipNormalizedPathD_st;
}

} // Fog namespace
