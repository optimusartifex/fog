// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// This file is included twice:
//   1. Single-threaded mode (CTX_MT_MODE == 0).
//   2. Multi-threaded mode (CTX_MT_MODE == 1).
//
// The purpose of this file is to simplify development of single-threaded and
// multi-threaded modes, and to keep performance at high level (this is reason
// why an inner loops are implemented many times, but for different configurations).

// Macro definitions:
//
// CTX_SYMBOL - Add suffix to the symbol (_ST or _MT), see Raster_Engine_p.h
// for naming convention.
//
// CTX_OFFSET - Context offset (ctx->offset) or 0.
// CTX_DELTA - Context delta (ctx->delta) or 1.
// CTX_STRIDE_WITH_DELTA - Stride multiplied by delta or non-modified stride.

#if !defined(CTX_MT_MODE)
// Included from IDE, make it happy.
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Engine_p.h>
#define CTX_MT_MODE 0
#endif

#if CTX_MT_MODE == 0
//   1. Single-threaded

// These variables are used in code, we make them const so compiler could fold
// them.
#define CTX_STRIDE_WITH_DELTA stride

#else
//   2. Multi-threaded

#define CTX_STRIDE_WITH_DELTA strideWithDelta

#endif

// Need namespace, before including by Raster_p.cpp the namespace was closed.
namespace Fog {

// ============================================================================
// [Fog::RasterPaintEngine - Renderers - Painting - DoPaintBoxes]
// ============================================================================

// Fill rectangles aligned to a pixel grid.
//
// Input data characteristics:
// - Already clipped to SIMPLE or COMPLEX clip region.
void RasterPaintEngine::CTX_SYMBOL(_doPaintBoxes)(
  RasterPaintContext* ctx, const IntBox* box, sysuint_t count)
{
  FOG_ASSERT(ctx->finalClipType != RASTER_CLIP_NULL);

  if (FOG_UNLIKELY(!count)) return;

  const IntBox* curBox;
  const IntBox* endBox = box + count;

  const IntBox* curBand = box;
  const IntBox* endBand;

  uint8_t* pixels = ctx->paintLayer.pixels;
  sysint_t stride = ctx->paintLayer.stride;

#if CTX_MT_MODE == 1
  int delta = ctx->delta;
  int offset = ctx->offset;
  sysint_t strideWithDelta = stride * delta;
#endif // CTX_MT_MODE

  // --------------------------------------------------------------------------
  // [Simple or Region Clip]
  // --------------------------------------------------------------------------

  if (FOG_LIKELY(ctx->finalClipType != RASTER_CLIP_MASK))
  {
    // ------------------------------------------------------------------------
    // [Macros]
    // ------------------------------------------------------------------------

#define RENDER_LOOP_RECT(NAME, BPP, CODE) \
    { \
      curBox = curBand; \
      \
      do { \
        int y1 = curBox->y1; \
        int y2 = curBox->y2; \
        \
        if (CTX_MT_MODE) \
        { \
          y1 = Raster_alignToDelta(y1, CTX_OFFSET, CTX_DELTA); \
          if (y1 >= y2) \
          { \
            while (++curBox != endBox && curBox->y1 < y1) ; \
            \
            if (curBox == endBox) \
              break; \
            else \
              continue; \
          } \
        } \
        \
        int x1 = curBox->x1; \
        \
        int w = curBox->x2 - x1; \
        int h = y2 - y1; \
        \
        uint8_t* pCur = pixels + \
                        (sysint_t)(uint)y1 * stride + \
                        (sysint_t)(uint)x1 * BPP; \
        \
        CODE \
        \
        curBox++; \
      } while (curBox != endBox); \
    }

#define RENDER_LOOP_FULL(NAME, BPP, CODE) \
    { \
      do { \
        endBand = curBand + 1; \
        \
        int y1 = curBand->y1; \
        int y2 = curBand->y2; \
        \
        if (CTX_MT_MODE) \
        { \
          y1 = Raster_alignToDelta(y1, CTX_OFFSET, CTX_DELTA); \
          if (y1 >= y2) \
          { \
            while (++curBand != endBox && curBand->y1 < y1) ; \
            \
            if (curBand == endBox) \
              break; \
            else \
              continue; \
          } \
        } \
        \
        if (FOG_UNLIKELY((endBand != endBox && endBand->y2 == y2))) \
        { \
          while (++endBand != endBox && endBand->y2 == y2) ; \
          \
          uint8_t* pCurBase = pixels + (sysint_t)(uint)y1 * stride; \
          do { \
            for (curBox = curBand; curBox != endBand; curBox++) \
            { \
              int x1 = curBox->x1; \
              int x2 = curBox->x2; \
              \
              int w = x2 - x1; \
              uint8_t* pCur = pCurBase + (uint)x1 * BPP; \
              \
              CODE \
            } \
            \
            pCurBase += CTX_STRIDE_WITH_DELTA; \
            y1 += CTX_DELTA; \
          } while (y1 < y2); \
          \
          curBand = endBand; \
        } \
        else \
        { \
          int x1 = curBand->x1; \
          int x2 = curBand->x2; \
          \
          int w = x2 - x1; \
          \
          uint8_t* pCur = pixels + \
                          (sysint_t)(uint)y1 * stride + \
                          (sysint_t)(uint)x1 * BPP; \
          do { \
            CODE \
            \
            pCur += CTX_STRIDE_WITH_DELTA; \
            y1 += CTX_DELTA; \
          } while (y1 < y2); \
          \
          curBand++; \
        } \
      } while (curBand != endBox); \
    }

#define RENDER_LOOP_SPAN(NAME, SCANLINE, ALPHA, CODE) \
    { \
      SpanExt8 baseSpan; \
      baseSpan.setNext(NULL); \
      baseSpan.setMaskPtr(Span8::cmaskToPtr(ALPHA)); \
      \
      do { \
        endBand = curBand + 1; \
        \
        int y1 = curBand->y1; \
        int y2 = curBand->y2; \
        \
        if (CTX_MT_MODE) \
        { \
          y1 = Raster_alignToDelta(y1, CTX_OFFSET, CTX_DELTA); \
          if (y1 >= y2) \
          { \
            while (++curBand != endBox && curBand->y1 < y1) ; \
            \
            if (curBand == endBox) \
              break; \
            else \
              continue; \
          } \
        } \
        \
        if (FOG_UNLIKELY((endBand != endBox && endBand->y2 == y2))) \
        { \
          SCANLINE.newScanlineNoRealloc(); \
          SCANLINE.addCSpan(curBand->x1, curBand->x2, ALPHA); \
          \
          do { \
            SCANLINE.addCSpan(endBand->x1, endBand->x2, ALPHA); \
          } while (++endBand != endBox && endBand->y2 == y2); \
          \
          if (FOG_LIKELY(SCANLINE.endScanline() == ERR_OK)) \
          { \
            uint8_t* pCur = pixels + (sysint_t)(uint)y1 * stride; \
            Span8* maskSpan = SCANLINE.getSpans(); \
            \
            do { \
              CODE \
              \
              pCur += CTX_STRIDE_WITH_DELTA; \
              y1 += CTX_DELTA; \
            } while (y1 < y2); \
          } \
          \
          curBand = endBand; \
        } \
        else \
        { \
          baseSpan.setPositionAndType(curBand->x1, curBand->x2, SPAN_TYPE_CMASK); \
          \
          uint8_t* pCur = pixels + (sysint_t)(uint)y1 * stride; \
          SpanExt8* maskSpan = &baseSpan; \
          \
          do { \
            CODE \
            \
            pCur += CTX_STRIDE_WITH_DELTA; \
            y1 += CTX_DELTA; \
          } while (y1 < y2); \
          \
          curBand++; \
        } \
      } while (curBand != endBox); \
    }

#define RENDER_LOOP_PATTERN_SRC(NAME, BPP, CODE) \
    { \
      SpanExt8 baseSpan; \
      baseSpan.setNext(NULL); \
      baseSpan.setMaskPtr(Span8::cmaskToPtr(0xFF)); \
      \
      do { \
        endBand = curBand + 1; \
        \
        int y1 = curBand->y1; \
        int y2 = curBand->y2; \
        \
        if (CTX_MT_MODE) \
        { \
          y1 = Raster_alignToDelta(y1, CTX_OFFSET, CTX_DELTA); \
          if (y1 >= y2) \
          { \
            while (++curBand != endBox && curBand->y1 < y1) ; \
            \
            if (curBand == endBox) \
              break; \
            else \
              continue; \
          } \
        } \
        \
        if (FOG_UNLIKELY((endBand != endBox && endBand->y2 == y2))) \
        { \
          while (++endBand != endBox && endBand->y2 == y2) ; \
          \
          uint8_t* pCurBase = pixels + (sysint_t)(uint)y1 * stride; \
          do { \
            for (curBox = curBand; curBox != endBand; curBox++) \
            { \
              int x1 = curBox->x1; \
              int x2 = curBox->x2; \
              \
              baseSpan.setPositionAndType(x1, x2, SPAN_TYPE_CMASK); \
              uint8_t* pCur = pCurBase + (uint)x1 * BPP; \
              \
              CODE \
            } \
            \
            pCurBase += CTX_STRIDE_WITH_DELTA; \
            y1 += CTX_DELTA; \
          } while (y1 < y2); \
          \
          curBand = endBand; \
        } \
        else \
        { \
          int x1 = curBand->x1; \
          int x2 = curBand->x2; \
          \
          baseSpan.setPositionAndType(x1, x2, SPAN_TYPE_CMASK); \
          uint8_t* pCur = pixels + \
                          (sysint_t)(uint)y1 * stride + \
                          (sysint_t)(uint)x1 * BPP; \
          \
          do { \
            CODE \
            \
            pCur += CTX_STRIDE_WITH_DELTA; \
            y1 += CTX_DELTA; \
          } while (y1 < y2); \
          \
          curBand++; \
        } \
      } while (curBand != endBox); \
    }

    // ------------------------------------------------------------------------
    // [Solid Source]
    // ------------------------------------------------------------------------

    if (ctx->ops.sourceType == PAINTER_SOURCE_ARGB)
    {
      uint32_t alpha = ctx->ops.alpha255;
      if (alpha == 0xFF)
      {
#if CTX_MT_MODE == 0
        // Special case, fill boxes using very optimized cblit_rect() if
        // possible. This is the fastest way with the minimum overhead.
        if ((ctx->ops.op == OPERATOR_SRC) ||
            (ctx->ops.op == OPERATOR_SRC_OVER && RasterUtil::isAlpha0xFF_PRGB32(ctx->solid.prgb)))
        {
          RasterCBlitRectFn cblit_rect = rasterFuncs.dib.cblit_rect[ctx->paintLayer.format];
          switch (ctx->paintLayer.bytesPerPixel)
          {
            case 4:
              RENDER_LOOP_RECT(bltSolidRectSrc32Clip, 4,
              {
                cblit_rect(pCur, stride, &ctx->solid, w, h, &ctx->closure);
              });
              break;
            case 1:
              RENDER_LOOP_RECT(bltSolidRectSrc8Clip, 1,
              {
                cblit_rect(pCur, stride, &ctx->solid, w, h, &ctx->closure);
              });
              break;
            default:
              FOG_ASSERT_NOT_REACHED();
          }
        }
        else
#endif // CTX_MT_MODE == 0
        {
          RasterCBlitFullFn cblit_full = ctx->funcs->cblit_full[ctx->ops.sourceType];

          switch (ctx->paintLayer.type)
          {
            case RASTER_LAYER_DIRECT32:
            case RASTER_LAYER_INDIRECT32:
              if (cblit_full != NULL)
              {
                RENDER_LOOP_FULL(bltSolidDirect32Clip, 4,
                {
                  cblit_full(pCur, &ctx->solid, (sysuint_t)w, &ctx->closure);
                });
              }
              else
              {
                cblit_full = rasterFuncs.getCompositeFuncs(
                  ctx->ops.op, ctx->paintLayer.secondaryFormat)->cblit_full[ctx->ops.sourceType];

                RENDER_LOOP_FULL(bltSolidIndirect32Clip, 4,
                {
                  ctx->paintLayer.toSecondaryFull(pCur, pCur, w, &ctx->closure);
                  cblit_full(pCur, &ctx->solid, (sysuint_t)w, &ctx->closure);
                  ctx->paintLayer.fromSecondaryFull(pCur, pCur, w, &ctx->closure);
                });
              }
              break;

            case RASTER_LAYER_DIRECT8:
            {
              RENDER_LOOP_FULL(bltSolidDirect8Clip, 1,
              {
                cblit_full(pCur, &ctx->solid, (sysuint_t)w, &ctx->closure);
              });
              break;
            }

            default:
              FOG_ASSERT_NOT_REACHED();
          }
        }
      }
      else
      {
        RasterCBlitSpanFn cblit_span = ctx->funcs->cblit_span[ctx->ops.sourceType];
        if (cblit_span != NULL)
        {
          RENDER_LOOP_SPAN(bltSolidDirectAlphaClip, ctx->scanlineSolid8, alpha,
          {
            cblit_span(pCur, &ctx->solid, maskSpan, &ctx->closure);
          });
        }
        else
        {
          cblit_span = rasterFuncs.getCompositeFuncs(
            ctx->ops.op, ctx->paintLayer.secondaryFormat)->cblit_span[ctx->ops.sourceType];

          RENDER_LOOP_SPAN(bltSolidIndirectAlphaClip, ctx->scanlineSolid8, alpha,
          {
            ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
            cblit_span(pCur, &ctx->solid, maskSpan, &ctx->closure);
            ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
          });
        }
      }
    }

    // ------------------------------------------------------------------------
    // [Pattern Source]
    // ------------------------------------------------------------------------

    else
    {
      RasterPattern* pctx = ctx->pctx;
      if (!pctx) return;

      uint32_t format = ctx->paintLayer.format;
      uint32_t alpha = ctx->ops.alpha255;

      // FastPath: Do not copy pattern to extra buffer if compositing operator
      // is OPERATOR_SRC. We need to match pattern pixel format and make sure
      // that operator is OPERATOR_SRC or OPERATOR_SRC_OVER (in case that 
      // pattern has no alpha-channel).
      if (alpha == 0xFF && isRawOpaqueBlit(format, pctx->format, ctx->ops.op))
      {
        // NOTE: The isRawOpaqueBlit() method can return true for (PRGB OP XRGB)
        // operation, but texture fetchers always set alpha byte to 0xFF for XRGB
        // format effectively creating 0xFFRRGGBB source that can be used by
        // (PRGB OP PRGB) blitter.
        switch (ctx->paintLayer.type)
        {
          case RASTER_LAYER_DIRECT32:
            RENDER_LOOP_PATTERN_SRC(bltPatternFastDirect32Clip, 4,
            {
              pctx->fetch(pctx, &baseSpan, pCur, y1, PATTERN_FETCH_BUFFER_ONLY);
            })
            break;

          case RASTER_LAYER_INDIRECT32:
            RENDER_LOOP_PATTERN_SRC(bltPatternFastIndirect32Clip, 4,
            {
              pctx->fetch(pctx, &baseSpan, pCur, y1, PATTERN_FETCH_BUFFER_ONLY);
              ctx->paintLayer.fromSecondaryFull(pCur, pCur, x2 - x1, NULL);
            })
            break;

          case RASTER_LAYER_DIRECT8:
            RENDER_LOOP_PATTERN_SRC(bltPatternFastDirect32Clip, 1,
            {
              pctx->fetch(pctx, &baseSpan, pCur, y1, PATTERN_FETCH_BUFFER_ONLY);
            })
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }
      }
      else
      {
        uint8_t* pPatternBuffer = ctx->getBuffer(
          (uint)(ctx->finalClipBox.getWidth()) * pctx->bytesPerPixel);
        if (!pPatternBuffer) return;

        RasterVBlitSpanFn vblit_span = ctx->funcs->vblit_span[pctx->format];
        if (vblit_span != NULL)
        {
          RENDER_LOOP_SPAN(bltPatternCommonDirect32Clip, ctx->scanlineExt8, alpha,
          {
            pctx->fetch(pctx, maskSpan, pPatternBuffer, y1, PATTERN_FETCH_CAN_USE_SRC);
            vblit_span(pCur, maskSpan, &ctx->closure);
          });
        }
        else
        {
          vblit_span = rasterFuncs.getCompositeFuncs(
            ctx->ops.op, ctx->paintLayer.secondaryFormat)->vblit_span[pctx->format];

          RENDER_LOOP_SPAN(bltPatternCommonIndirect32Clip, ctx->scanlineExt8, alpha,
          {
            pctx->fetch(pctx, maskSpan, pPatternBuffer, y1, PATTERN_FETCH_CAN_USE_SRC);
            ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
            vblit_span(pCur, maskSpan, &ctx->closure);
            ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
          });
        }
      }
    }
#undef RENDER_LOOP_RECT
#undef RENDER_LOOP_FULL
#undef RENDER_LOOP_SPAN
#undef RENDER_LOOP_PATTERN_SRC
  }

  // --------------------------------------------------------------------------
  // [Mask Clip]
  // --------------------------------------------------------------------------

  else
  {
    // ------------------------------------------------------------------------
    // [Macros]
    // ------------------------------------------------------------------------

#define RENDER_LOOP_MASK(NAME, SCANLINE, USE_ALPHA, ALPHA, CODE) \
    if (FOG_LIKELY(SCANLINE.prepareScanline(ctx->workClipBox.getWidth()) == ERR_OK)) \
    { \
      int x; \
      \
      int min_y = ctx->maskY1; \
      int y1; \
      int y2; \
      \
      while (curBand->y2 <= min_y) \
      { \
        if (++curBand == endBox) goto NAME##_End; \
      } \
      \
      y1 = Math::max<int>(curBand->y1, min_y); \
      goto NAME##_Loop; \
      \
      do { \
        y1 = curBand->y1; \
NAME##_Loop: \
        y2 = curBand->y2; \
        endBand = curBand + 1; \
        \
        if (y1 >= ctx->maskY2) goto NAME##_End; \
        \
        if (CTX_MT_MODE) \
        { \
          y1 = Raster_alignToDelta(y1, CTX_OFFSET, CTX_DELTA); \
          if (y1 >= y2) \
          { \
            while (++curBand != endBox && curBand->y1 < y1) ; \
            \
            if (curBand == endBox) \
              break; \
            else \
              continue; \
          } \
          if (y1 >= ctx->maskY2) goto NAME##_End; \
        } \
        \
        while ((endBand != endBox && curBand->y2 == y2)) endBand++; \
        if (y2 > ctx->maskY2) y2 = ctx->maskY2; \
        \
        { \
          uint8_t* pCur = pixels + (sysint_t)(uint)y1 * stride; \
          \
          do { \
            const Span8* curSpan = \
              RASTER_CLIP_SPAN_GET_USABLE(ctx->getClipSpan(y1)); \
            if (curSpan == NULL) goto NAME##_Continue; \
            \
            SCANLINE.newScanlineNoRealloc(); \
            \
            curBox = curBand; \
NAME##_Begin: \
            \
            /* Skip spans that can't intersect. */ \
            x = curBox->x1; \
            while (curSpan->getX2() <= x) \
            { \
NAME##_RepeatSpan: \
              curSpan = curSpan->getNext(); \
              if (curSpan == NULL) goto NAME##_DoSpans; \
            } \
            \
            /* Skip boxes that can't intersect. */ \
            x = curSpan->getX1(); \
            while (curBox->x2 <= x) \
            { \
NAME##_RepeatBox: \
              if (++curBox == endBand) goto NAME##_DoSpans; \
            } \
            \
            /* Repeat skip steps if needed. */ \
NAME##_Again: \
            x = curBox->x1; \
            if (curSpan->getX2() <= x) goto NAME##_RepeatSpan; \
            \
            /* Handle the intersection. */ \
            { \
              int x1 = Math::max<int>(x, curSpan->getX1()); \
              int x2_box = curBox->x2; \
              \
              FOG_ASSERT(x1 < x2_box); \
              \
              /* Process all spans that fits into the current box (curBox). */ \
              for (;;) \
              { \
                int x2 = Math::min<int>(curSpan->getX2(), x2_box); \
                \
                if (curSpan->isCMask()) \
                { \
                  uint32_t msk0 = curSpan->getCMask(); \
                  if (USE_ALPHA) \
                    msk0 = Face::b32_1x1MulDiv255(msk0, ALPHA); \
                  SCANLINE.addCSpan(x1, x2, msk0); \
                } \
                else \
                { \
                  const uint8_t* mskp = curSpan->getVMask() - curSpan->getX1() + x1; \
                  \
                  if (USE_ALPHA) \
                  { \
                    SCANLINE.addVSpanAlphaCopyAndMul(x1, x2, curSpan->getType(), mskp, ALPHA); \
                  } \
                  else \
                  { \
                    SCANLINE.addVSpanAlphaAdopt(x1, x2, curSpan->getType(), mskp); \
                  } \
                } \
                \
                if (x2 <= x2_box) \
                { \
                  curSpan = curSpan->getNext(); \
                  if (curSpan == NULL) goto NAME##_DoSpans; \
                  \
                  x1 = curSpan->getX1(); \
                  if (x1 >= x2_box) goto NAME##_RepeatBox; \
                } \
                else \
                { \
                  if (++curBox == endBand) goto NAME##_DoSpans; \
                  if (curBox->x1 < curSpan->getX2()) \
                  { \
                    x1 = Math::max<int>(curBox->x1, curSpan->getX1()); \
                    x2_box = curBox->x2; \
                    continue; \
                  } \
                  else \
                  { \
                    goto NAME##_Begin; \
                  } \
                } \
              } \
            } \
            \
NAME##_DoSpans: \
            if (FOG_LIKELY(SCANLINE.endScanline() == ERR_OK)) \
            { \
              Span* maskSpan = SCANLINE.getSpans(); \
              if (FOG_LIKELY(maskSpan != NULL)) \
              { \
                CODE \
              } \
            } \
            \
NAME##_Continue: \
            pCur += CTX_STRIDE_WITH_DELTA; \
            y1 += CTX_DELTA; \
          } while (y1 < y2); \
        } \
        \
        curBand = endBand; \
      } while (curBand != endBox); \
NAME##_End: \
      ; \
    }

    // ------------------------------------------------------------------------
    // [Solid Source]
    // ------------------------------------------------------------------------

    if (ctx->ops.sourceType == PAINTER_SOURCE_ARGB)
    {
      RasterCBlitSpanFn cblit_span = ctx->funcs->cblit_span[ctx->ops.sourceFormat];
      uint32_t alpha = ctx->ops.alpha255;

      if (cblit_span != NULL)
      {
        if (alpha == 0xFF)
        {
          RENDER_LOOP_MASK(bltSolidDirectOpaqueMask, ctx->scanlineSolid8, false, 0xFF,
          {
            cblit_span(pCur, &ctx->solid, maskSpan, &ctx->closure);
          });
        }
        else
        {
          RENDER_LOOP_MASK(bltSolidDirectAlphaMask, ctx->scanlineSolid8, true, alpha,
          {
            cblit_span(pCur, &ctx->solid, maskSpan, &ctx->closure);
          });
        }
      }
      else
      {
        if (alpha == 0xFF)
        {
          RENDER_LOOP_MASK(bltSolidIndirectOpaqueMask, ctx->scanlineSolid8, false, 0xFF,
          {
            ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
            cblit_span(pCur, &ctx->solid, maskSpan, &ctx->closure);
            ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
          });
        }
        else
        {
          RENDER_LOOP_MASK(bltSolidIndirectAlphaMask, ctx->scanlineSolid8, true, alpha,
          {
            ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
            cblit_span(pCur, &ctx->solid, maskSpan, &ctx->closure);
            ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
          });
        }
      }
    }

    // ------------------------------------------------------------------------
    // [Pattern Source]
    // ------------------------------------------------------------------------

    else
    {
      RasterPattern* pctx = ctx->pctx;
      if (!pctx) return;

      uint8_t* pPatternBuffer = ctx->getBuffer(
        (uint)(ctx->finalClipBox.getWidth()) * pctx->bytesPerPixel);
      if (!pPatternBuffer) return;

      RasterVBlitSpanFn vblit_span = ctx->funcs->vblit_span[pctx->format];
      uint32_t alpha = ctx->ops.alpha255;

      if (vblit_span != NULL)
      {
        if (alpha == 0xFF)
        {
          RENDER_LOOP_MASK(bltPatternDirectOpaqueMask, ctx->scanlineExt8, false, 0xFF,
          {
            pctx->fetch(pctx, maskSpan, pPatternBuffer, y1, PATTERN_FETCH_CAN_USE_SRC);
            vblit_span(pCur, maskSpan, &ctx->closure);
          });
        }
        else
        {
          RENDER_LOOP_MASK(bltPatternDirectAlphaMask, ctx->scanlineExt8, true, alpha,
          {
            pctx->fetch(pctx, maskSpan, pPatternBuffer, y1, PATTERN_FETCH_CAN_USE_SRC);
            vblit_span(pCur, maskSpan, &ctx->closure);
          });
        }
      }
      else
      {
        vblit_span = rasterFuncs.getCompositeFuncs(
          ctx->ops.op, ctx->paintLayer.secondaryFormat)->vblit_span[pctx->format];

        if (alpha == 0xFF)
        {
          RENDER_LOOP_MASK(bltPatternIndirectOpaqueMask, ctx->scanlineExt8, false, 0xFF,
          {
            ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
            pctx->fetch(pctx, maskSpan, pPatternBuffer, y1, PATTERN_FETCH_CAN_USE_SRC);
            vblit_span(pCur, maskSpan, &ctx->closure);
            ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
          });
        }
        else
        {
          RENDER_LOOP_MASK(bltPatternIndirectAlphaMask, ctx->scanlineExt8, true, alpha,
          {
            ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
            pctx->fetch(pctx, maskSpan, pPatternBuffer, y1, PATTERN_FETCH_CAN_USE_SRC);
            vblit_span(pCur, maskSpan, &ctx->closure);
            ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
          });
        }
      }
    }
#undef RENDER_LOOP_MASK
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Renderers - Painting - DoPaintImage]
// ============================================================================

// Blit image aligned to a pixel grid.
//
// Input data characteristics:
// - Already clipped to SIMPLE clip rectangle. If clip region is COMPLEX,
//   additional clipping must be performed by callee (here).
void RasterPaintEngine::CTX_SYMBOL(_doPaintImage)(
  RasterPaintContext* ctx, const IntRect& dst, const Image& image, const IntRect& src)
{
  FOG_ASSERT(ctx->finalClipType != RASTER_CLIP_NULL);

  ImageData* image_d = image._d;
  sysint_t dstStride = ctx->paintLayer.stride;
  sysint_t srcStride = image_d->stride;

  ctx->closure.srcPalette = 
    reinterpret_cast<const uint32_t*>(image._d->palette.getData());

  int y = dst.y;
  int yMax = dst.y + dst.h;

#if CTX_MT_MODE == 1
  int delta = ctx->delta;
  int offset = ctx->offset;
#endif // CTX_MT_MODE

  uint srcBpp = image_d->bytesPerPixel;
  uint alpha = ctx->ops.alpha255;

  // --------------------------------------------------------------------------
  // [Simple or Region Clip]
  // --------------------------------------------------------------------------

  if (FOG_LIKELY(ctx->finalClipType != RASTER_CLIP_MASK))
  {
    int xDstAdjust = src.x - dst.x;

#define RENDER_LOOP_FULL(NAME, BPP, CODE) \
  { \
    /* --------------------------------------------------------------------- */ \
    /* [Simple Clip]                                                         */ \
    /* --------------------------------------------------------------------- */ \
    if (FOG_LIKELY(ctx->finalClipType == RASTER_CLIP_SIMPLE)) \
    { \
      int x = dst.x; \
      \
      uint8_t* dstCur = ctx->paintLayer.pixels + (sysint_t)(uint)(x) * BPP; \
      const uint8_t* srcCur = image_d->first + (sysint_t)(uint)(src.x) * srcBpp; \
      \
      /* Singlethreaded rendering (delta == 1, offset == 0). */ \
      if (!CTX_MT_MODE) \
      { \
        dstCur += (sysint_t)(uint)(y) * dstStride; \
        srcCur += (sysint_t)(uint)(src.y) * srcStride; \
      } \
      /* Multithreaded rendering (delta == X, offset == Y). */ \
      else \
      { \
        y = Raster_alignToDelta(y, CTX_OFFSET, CTX_DELTA); \
        if (y >= yMax) return; \
         \
        dstCur += (sysint_t)(uint)(y) * dstStride; \
        srcCur += (sysint_t)(uint)(src.y + y - dst.y) * srcStride; \
        \
        dstStride *= CTX_DELTA; \
        srcStride *= CTX_DELTA; \
      } \
      \
      int w = dst.w; \
      \
      do { \
        CODE \
        \
        dstCur += dstStride; \
        srcCur += srcStride; \
        y += CTX_DELTA; \
      } while (y < yMax); \
    } \
    /* --------------------------------------------------------------------- */ \
    /* [Region Clip]                                                         */ \
    /* --------------------------------------------------------------------- */ \
    else /* if (ctx->finalClipType == RASTER_CLIP_REGION) */ \
    { \
      const IntBox* clipCur = ctx->finalRegion.getData(); \
      const IntBox* clipTo; \
      const IntBox* clipNext; \
      const IntBox* clipEnd = clipCur + ctx->finalRegion.getLength(); \
      \
      uint8_t* dstBase; \
      const uint8_t* srcBase; \
      \
      int minX = dst.x; \
      int maxX = minX + dst.w; \
      \
      /* Single-threaded. */ \
      if (!CTX_MT_MODE) \
      { \
        /* Advance clip pointer. */ \
NAME##_nodelta_advance: \
        while (clipCur->y2 <= y) \
        { \
          if (++clipCur == clipEnd) goto NAME##_end; \
        } \
        \
        /* Advance to end of the current span list (same y1, y2). */ \
        clipNext = clipCur + 1; \
        while (clipNext != clipEnd && clipCur->y1 == clipNext->y1) clipNext++; \
        \
        /* Skip some rows if needed. */ \
        if (y < clipCur->y1) \
        { \
          y = clipCur->y1; \
          if (y >= yMax) goto NAME##_end; \
        } \
        \
        /* Fix clip width (subtract clip rects if some rects are outside of the paint) */ \
        while (clipCur->x2 <= minX) \
        { \
          if (++clipCur == clipNext) \
          { \
            if (clipCur == clipEnd) \
              goto NAME##_end; \
            else \
              goto NAME##_nodelta_advance; \
          } \
        } \
        \
        clipTo = clipNext - 1; \
        while (clipTo->x1 >= maxX) \
        { \
          if (clipTo == clipCur) \
          { \
            clipCur = clipNext; \
            if (clipCur == clipEnd) \
              goto NAME##_end; \
            else \
              goto NAME##_nodelta_advance; \
          } \
          clipTo--; \
        } \
        \
        dstBase = ctx->paintLayer.pixels + (sysint_t)(uint)(y) * dstStride; \
        srcBase = image_d->first + (sysint_t)(uint)(y + src.y - dst.y) * srcStride; \
        /* Adjust x so we don't need to calculate it in the main loop again and again. */ \
        srcBase += (xDstAdjust) * (int)srcBpp; \
        \
        for (; y < yMax; y++, dstBase += dstStride, srcBase += srcStride) \
        { \
          /* Advance clip pointer if needed. */ \
          if (FOG_UNLIKELY(y >= clipCur->y2)) \
          { \
            clipCur = clipNext; \
            if (clipCur == clipEnd) goto NAME##_end; \
            goto NAME##_nodelta_advance; \
          } \
          \
          const IntBox* cbox = clipCur; \
          int x1 = cbox->x1; \
          if (x1 < minX) x1 = minX; \
          \
          uint8_t* dstCur = dstBase + (sysint_t)(uint)(x1) * BPP; \
          const uint8_t* srcCur = srcBase + (sysint_t)(uint)(x1 + src.x - dst.x) * srcBpp; \
          \
          while (cbox != clipTo) \
          { \
            int w = cbox->x2 - x1; \
            FOG_ASSERT(w > 0); \
            \
            CODE \
            \
            cbox++; \
            \
            uint adv = (uint)(cbox->x1 - x1); \
            x1 += adv; \
            \
            dstCur += adv * BPP; \
            srcCur += adv * srcBpp; \
          } \
          \
          int x2 = cbox->x2; \
          if (x2 > maxX) x2 = maxX; \
          { \
            int w = x2 - x1; \
            FOG_ASSERT(w > 0); \
            \
            CODE \
          } \
        } \
      } \
      /* Multi-threaded */ \
      else \
      { \
        sysint_t srcStrideWithDelta = srcStride * CTX_DELTA; \
        sysint_t dstStrideWithDelta = dstStride * CTX_DELTA; \
        \
        y = Raster_alignToDelta(y, CTX_OFFSET, CTX_DELTA); \
        \
        /* Advance clip pointer. */ \
NAME##_delta_advance: \
        while (clipCur->y2 <= y) \
        { \
          if (++clipCur == clipEnd) goto NAME##_end; \
        } \
        \
        /* Advance to end of the current span list (same y1, y2). */ \
        clipNext = clipCur + 1; \
        while (clipNext != clipEnd && clipCur->y1 == clipNext->y1) clipNext++; \
        \
        /* Skip some rows if needed. */ \
        if (y < clipCur->y1) \
        { \
          y = Raster_alignToDelta(clipCur->y1, CTX_OFFSET, CTX_DELTA); \
          if (y >= yMax) goto NAME##_end; \
          if (y >= clipCur->y2) \
          { \
            clipCur = clipNext; \
            if (clipCur == clipEnd) goto NAME##_end; \
            goto NAME##_delta_advance; \
          } \
        } \
        \
        /* Fix clip width (subtract clip rects if some rects are outside of the paint) */ \
        while (clipCur->x2 <= minX) \
        { \
          if (++clipCur == clipNext) \
          { \
            if (clipCur == clipEnd) \
              goto NAME##_end; \
            else \
              goto NAME##_delta_advance; \
          } \
        } \
        \
        clipTo = clipNext - 1; \
        while (clipTo->x1 >= maxX) \
        { \
          if (clipTo == clipCur) \
          { \
            clipCur = clipNext; \
            if (clipCur == clipEnd) \
              goto NAME##_end; \
            else \
              goto NAME##_delta_advance; \
          } \
          clipTo--; \
        } \
        \
        dstBase = ctx->paintLayer.pixels + (sysint_t)(uint)(y) * dstStride; \
        srcBase = image_d->first + (sysint_t)(uint)(y + src.y - dst.y) * srcStride; \
        \
        for (; y < yMax; y += CTX_DELTA, \
                         dstBase += dstStrideWithDelta, \
                         srcBase += srcStrideWithDelta) \
        { \
          /* Advance clip pointer if needed. */ \
          if (FOG_UNLIKELY(y >= clipCur->y2)) \
          { \
            clipCur = clipNext; \
            if (clipCur == clipEnd) goto NAME##_end; \
            goto NAME##_delta_advance; \
          } \
          \
          const IntBox* cbox = clipCur; \
          int x1 = cbox->x1; \
          if (x1 < minX) x1 = minX; \
          \
          uint8_t* dstCur = dstBase + (sysint_t)(uint)(x1) * BPP; \
          const uint8_t* srcCur = srcBase + (sysint_t)(uint)(x1 + src.x - dst.x) * srcBpp; \
          \
          while (cbox != clipTo) \
          { \
            int w = cbox->x2 - x1; \
            FOG_ASSERT(w > 0); \
            \
            CODE \
            \
            cbox++; \
            \
            uint adv = (uint)(cbox->x1 - x1); \
            x1 += adv; \
            \
            dstCur += adv * BPP; \
            srcCur += adv * srcBpp; \
          } \
          \
          int x2 = cbox->x2; \
          if (x2 > maxX) x2 = maxX; \
          { \
            int w = x2 - x1; \
            FOG_ASSERT(w > 0); \
            \
            CODE \
          } \
        } \
      } \
    } \
NAME##_end: \
    ; \
  }

#define RENDER_LOOP_SPAN(NAME, SCANLINE, ALPHA, CODE) \
  { \
    /* --------------------------------------------------------------------- */ \
    /* [Simple Clip]                                                         */ \
    /* --------------------------------------------------------------------- */ \
    if (FOG_LIKELY(ctx->finalClipType == RASTER_CLIP_SIMPLE)) \
    { \
      uint8_t* dstCur = ctx->paintLayer.pixels; \
      const uint8_t* srcCur = image_d->first + (sysint_t)(uint)(src.x) * srcBpp; \
      \
      /* Singlethreaded rendering (delta == 1, offset == 0). */ \
      if (!CTX_MT_MODE) \
      { \
        dstCur += (sysint_t)(uint)(y) * dstStride; \
        srcCur += (sysint_t)(uint)(src.y) * srcStride; \
      } \
      /* Multithreaded rendering (delta == X, offset == Y). */ \
      else \
      { \
        y = Raster_alignToDelta(y, CTX_OFFSET, CTX_DELTA); \
        if (y >= yMax) return; \
         \
        dstCur += (sysint_t)(uint)(y) * dstStride; \
        srcCur += (sysint_t)(uint)(y + src.y - dst.y) * srcStride; \
        \
        dstStride *= CTX_DELTA; \
        srcStride *= CTX_DELTA; \
      } \
      \
      /* We don't need scanline instance here, just one SPAN. */\
      SpanExt8 baseSpan; \
      baseSpan.setPositionAndType(dst.x, dst.x + dst.w, SPAN_TYPE_CMASK); \
      baseSpan.setMaskPtr(Span8::cmaskToPtr(ALPHA)); \
      baseSpan.setNext(NULL); \
      baseSpan.setData((uint8_t*)srcCur); \
      \
      do { \
        SpanExt8* maskSpan = &baseSpan; \
        CODE \
        \
        dstCur += dstStride; \
        baseSpan._data += srcStride; \
        y += CTX_DELTA; \
      } while (y < yMax); \
    } \
    /* --------------------------------------------------------------------- */ \
    /* [Region Clip]                                                         */ \
    /* --------------------------------------------------------------------- */ \
    else /* if (ctx->finalClipType == RASTER_CLIP_REGION) */ \
    { \
      const IntBox* clipCur = ctx->finalRegion.getData(); \
      const IntBox* clipTo; \
      const IntBox* clipNext; \
      const IntBox* clipEnd = clipCur + ctx->finalRegion.getLength(); \
      \
      uint8_t* dstBase = ctx->paintLayer.pixels; \
      uint8_t* dstCur; \
      \
      const uint8_t* srcBase = image_d->first + (src.y - dst.y) * srcStride; \
      /* Adjust x so we don't need to calculate it in the main loop again and again. */ \
      srcBase += (xDstAdjust) * (int)srcBpp; \
      \
      int minX = dst.x; \
      int maxX = minX + dst.w; \
      \
      /* Single-threaded. */ \
      if (!CTX_MT_MODE) \
      { \
        /* Advance clip pointer. */ \
NAME##_nodelta_advance: \
        while (clipCur->y2 <= y) \
        { \
          if (++clipCur == clipEnd) goto NAME##_end; \
        } \
        \
        /* Advance to end of the current span list (same y1, y2). */ \
        clipNext = clipCur + 1; \
        while (clipNext != clipEnd && clipCur->y1 == clipNext->y1) clipNext++; \
        \
        /* Skip some rows if needed. */ \
        if (y < clipCur->y1) \
        { \
          y = clipCur->y1; \
          if (y >= yMax) goto NAME##_end; \
        } \
        \
        /* Fix clip width (subtract clip rects if some rects are outside of the paint) */ \
        while (clipCur->x2 <= minX) \
        { \
          if (++clipCur == clipNext) \
          { \
            if (clipCur == clipEnd) \
              goto NAME##_end; \
            else \
              goto NAME##_nodelta_advance; \
          } \
        } \
        \
        clipTo = clipNext - 1; \
        while (clipTo->x1 >= maxX) \
        { \
          if (clipTo == clipCur) \
          { \
            clipCur = clipNext; \
            if (clipCur == clipEnd) \
              goto NAME##_end; \
            else \
              goto NAME##_nodelta_advance; \
          } \
          clipTo--; \
        } \
        \
        /* Now build the scanline, setup initial 'data' pointer with 'src'. */ \
        { \
          const uint8_t* srcCur = srcBase + y * srcStride; \
          \
          SCANLINE.newScanlineNoRealloc(); \
          do { \
            int x1 = Math::max<int>(clipCur->x1, minX); \
            int x2 = Math::min<int>(clipCur->x2, maxX); \
            SCANLINE.addCSpan(x1, x2, ALPHA); \
            reinterpret_cast<SpanExt8*>(SCANLINE._spanCurrent)->setData((uint8_t*)srcCur + (x1 * srcBpp)); \
          } while (clipCur != clipTo); \
          if (SCANLINE.endScanline() != ERR_OK) goto NAME##_end; \
          \
          /* We don't need clipCur from now, so init it to clipNext, because we have to do it later. */ \
          clipCur = clipNext; \
        } \
        \
        dstCur = dstBase; \
        goto NAME##_nodelta_inside; \
        \
        for (; y < yMax; y++, dstCur += dstStride) \
        { \
          /* Advance clip pointer if needed. */ \
          if (FOG_UNLIKELY(y >= clipCur->y2)) \
          { \
            if (clipCur == clipEnd) goto NAME##_end; \
            goto NAME##_nodelta_advance; \
          } \
          \
          /* Adjust 'src' */ \
          { \
            Span* maskSpan = SCANLINE.getSpans(); \
            do { \
              reinterpret_cast<SpanExt8*>(maskSpan)->_data += srcStride; \
            } while ((maskSpan = maskSpan->getNext()) != NULL); \
          } \
          \
NAME##_nodelta_inside: \
          { \
            Span* maskSpan = SCANLINE.getSpans(); \
            CODE \
          } \
        } \
      } \
      /* Multi-threaded */ \
      else \
      { \
        sysint_t srcStrideWithDelta = srcStride * CTX_DELTA; \
        sysint_t dstStrideWithDelta = dstStride * CTX_DELTA; \
        \
        y = Raster_alignToDelta(y, CTX_OFFSET, CTX_DELTA); \
        \
        /* Advance clip pointer. */ \
NAME##_delta_advance: \
        while (clipCur->y2 <= y) \
        { \
          if (++clipCur == clipEnd) goto NAME##_end; \
        } \
        \
        /* Advance to end of the current span list (same y1, y2). */ \
        clipNext = clipCur + 1; \
        while (clipNext != clipEnd && clipCur->y1 == clipNext->y1) clipNext++; \
        \
        /* Skip some rows if needed. */ \
        if (y < clipCur->y1) \
        { \
          y = Raster_alignToDelta(clipCur->y1, CTX_OFFSET, CTX_DELTA); \
          if (y >= yMax) goto NAME##_end; \
          if (y >= clipCur->y2) \
          { \
            clipCur = clipNext; \
            if (clipCur == clipEnd) goto NAME##_end; \
            goto NAME##_delta_advance; \
          } \
        } \
        \
        /* Fix clip width (subtract clip rects if some rects are outside of the paint) */ \
        while (clipCur->x2 <= minX) \
        { \
          if (++clipCur == clipNext) \
          { \
            if (clipCur == clipEnd) \
              goto NAME##_end; \
            else \
              goto NAME##_delta_advance; \
          } \
        } \
        \
        clipTo = clipNext - 1; \
        while (clipTo->x1 >= maxX) \
        { \
          if (clipTo == clipCur) \
          { \
            clipCur = clipNext; \
            if (clipCur == clipEnd) \
              goto NAME##_end; \
            else \
              goto NAME##_delta_advance; \
          } \
          clipTo--; \
        } \
        \
        /* Now build the scanline, setup initial 'data' pointer with 'src'. */ \
        { \
          const uint8_t* srcCur = srcBase + y * srcStride; \
          \
          SCANLINE.newScanlineNoRealloc(); \
          do { \
            int x1 = Math::max<int>(clipCur->x1, minX); \
            int x2 = Math::min<int>(clipCur->x2, maxX); \
            SCANLINE.addCSpan(x1, x2, ALPHA); \
            reinterpret_cast<SpanExt8*>(SCANLINE._spanCurrent)->setData((uint8_t*)srcCur + (x1 * srcBpp)); \
          } while (clipCur != clipTo); \
          if (SCANLINE.endScanline() != ERR_OK) goto NAME##_end; \
          \
          /* We don't need clipCur from now, so init it to clipNext, because we have to do it later. */ \
          clipCur = clipNext; \
        } \
        \
        dstCur = dstBase; \
        goto NAME##_delta_inside; \
        \
        for (; y < yMax; y += CTX_DELTA, dstCur += dstStrideWithDelta) \
        { \
          /* Advance clip pointer if needed. */ \
          if (FOG_UNLIKELY(y >= clipCur->y2)) \
          { \
            clipCur = clipNext; \
            if (clipCur == clipEnd) goto NAME##_end; \
            goto NAME##_delta_advance; \
          } \
          \
          /* Adjust 'src' */ \
          { \
            Span* maskSpan = SCANLINE.getSpans(); \
            do { \
              reinterpret_cast<SpanExt8*>(maskSpan)->_data += srcStrideWithDelta; \
            } while ((maskSpan = maskSpan->getNext()) != NULL); \
          } \
          \
NAME##_delta_inside: \
          { \
            Span* maskSpan = SCANLINE.getSpans(); \
            CODE \
          } \
        } \
      } \
    } \
NAME##_end: \
    ; \
  }

    if (alpha == 0xFF)
    {
      RasterVBlitFullFn vblit_full = ctx->funcs->vblit_full[image_d->format];

      switch (ctx->paintLayer.type)
      {
        case RASTER_LAYER_DIRECT32:
        case RASTER_LAYER_INDIRECT32:
          if (vblit_full != NULL)
          {
            RENDER_LOOP_FULL(bltImageOpaqueDirect32Clip, 4,
            {
              vblit_full(dstCur, srcCur, w, &ctx->closure);
            })
          }
          else
          {
            vblit_full = rasterFuncs.getCompositeFuncs(
              ctx->ops.op, ctx->paintLayer.secondaryFormat)->vblit_full[image_d->format];

            RENDER_LOOP_FULL(bltImageOpaqueIndirect32Clip, 4,
            {
              ctx->paintLayer.toSecondaryFull(dstCur, dstCur, w, &ctx->closure);
              vblit_full(dstCur, srcCur, w, &ctx->closure);
              ctx->paintLayer.fromSecondaryFull(dstCur, dstCur, w, &ctx->closure);
            })
          }
          break;

        case RASTER_LAYER_DIRECT8:
          RENDER_LOOP_FULL(bltImageOpaqueDirect8Clip, 1,
          {
            vblit_full(dstCur, srcCur, w, &ctx->closure);
          })
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }
    else
    {
      RasterVBlitSpanFn vblit_span = ctx->funcs->vblit_span[image_d->format];

      if (vblit_span != NULL)
      {
        RENDER_LOOP_SPAN(bltImageAlphaDirectClip, ctx->scanlineExt8, alpha,
        {
          vblit_span(dstCur, maskSpan, &ctx->closure);
        })
      }
      else
      {
        vblit_span = rasterFuncs.getCompositeFuncs(
          ctx->ops.op, ctx->paintLayer.secondaryFormat)->vblit_span[image_d->format];

        RENDER_LOOP_SPAN(bltImageAlphaIndirectClip, ctx->scanlineExt8, alpha,
        {
          ctx->paintLayer.toSecondarySpan(dstCur, maskSpan, &ctx->closure);
          vblit_span(dstCur, maskSpan, &ctx->closure);
          ctx->paintLayer.fromSecondarySpan(dstCur, maskSpan, &ctx->closure);
        })
      }
    }
#undef RENDER_LOOP_FULL
#undef RENDER_LOOP_SPAN
  }

  // --------------------------------------------------------------------------
  // [Mask Clip]
  // --------------------------------------------------------------------------

  else
  {
    int x = dst.x;

    uint8_t* pCur;
    const uint8_t* srcBase;

    int minX = dst.x;
    int maxX = minX + dst.w;

    int xDstAdjust = src.x - dst.x;

    if (y < ctx->maskY1) y = ctx->maskY1;
    if (yMax > ctx->maskY2) yMax = ctx->maskY2;

    if (CTX_MT_MODE)
    {
      y = Raster_alignToDelta(y, CTX_OFFSET, CTX_DELTA);
    }
    if (y >= yMax) return;

    pCur = ctx->paintLayer.pixels + (sysint_t)(uint)(y) * dstStride;

    srcBase = image_d->first + (sysint_t)(uint)(src.y + y - dst.y) * srcStride;
    // Adjust x so we don't need to calculate it in the main loop again and again.
    srcBase += (xDstAdjust) * (int)srcBpp;

    if (CTX_MT_MODE)
    {
      dstStride *= CTX_DELTA;
      srcStride *= CTX_DELTA;
    }

    if (ctx->scanlineExt8.prepareScanline(ctx->workClipBox.getWidth()) != ERR_OK) return;

#define RENDER_LOOP_SPAN(NAME, SCANLINE, USE_ALPHA, ALPHA, CODE) \
    do { \
      Span8* curSpan = RASTER_CLIP_SPAN_GET_USABLE(ctx->getClipSpan(y)); \
      if (curSpan) \
      { \
        int x1; \
        \
        if (curSpan->getX2() <= minX) \
        { \
          do { \
            if ((curSpan = curSpan->getNext()) == NULL) goto NAME##_Skip; \
          } while (curSpan->getX2() <= minX); \
        } \
        \
        x1 = Math::max<int>(minX, curSpan->getX1()); \
        if (x1 >= maxX) goto NAME##_Skip; \
        \
        SCANLINE.newScanlineNoRealloc(); \
        \
        do { \
          int x2 = Math::min<int>(maxX, curSpan->getX2()); \
          \
          if (curSpan->isCMask()) \
          { \
            uint32_t msk0 = curSpan->getCMask(); \
            if (USE_ALPHA) \
              msk0 = Face::b32_1x1MulDiv255(msk0, ALPHA); \
            SCANLINE.addCSpan(x1, x2, msk0); \
          } \
          else \
          { \
            const uint8_t* mskp = curSpan->getVMask() - curSpan->getX1() + x1; \
            \
            if (USE_ALPHA) \
            { \
              SCANLINE.addVSpanAlphaCopyAndMul(x1, x2, curSpan->getType(), mskp, ALPHA); \
            } \
            else \
            { \
              SCANLINE.addVSpanAlphaAdopt(x1, x2, curSpan->getType(), mskp); \
            } \
          } \
          \
          /* Assign src-image data into the span. */ \
          reinterpret_cast<SpanExt8*>(SCANLINE._spanCurrent)->setData( \
            const_cast<uint8_t*>(srcBase + (sysint_t)(uint)(x1) * srcBpp)); \
          \
          if ((curSpan = curSpan->getNext()) == NULL) break; \
        } while ((x1 = curSpan->getX1()) < maxX); \
        \
        if (SCANLINE.endScanline() != ERR_OK) goto NAME##_End;\
        \
        { \
          Span8* maskSpan = SCANLINE.getSpans(); \
          FOG_ASSERT(maskSpan != NULL); \
          \
          CODE \
        } \
      } \
      \
NAME##_Skip: \
      pCur += dstStride; \
      srcBase += srcStride; \
      y += CTX_DELTA; \
    } while (y < yMax); \
    \
NAME##_End: \
    ;

    RasterVBlitSpanFn vblit_span = ctx->funcs->vblit_span[image_d->format];

    if (vblit_span != NULL)
    {
      if (alpha == 0xFF)
      {
        RENDER_LOOP_SPAN(bltImageDirectOpaqueMask, ctx->scanlineExt8, false, 0xFF,
        {
          vblit_span(pCur, maskSpan, &ctx->closure);
        });
      }
      else
      {
        RENDER_LOOP_SPAN(bltImageDirectAlphaMask, ctx->scanlineExt8, true, alpha,
        {
          vblit_span(pCur, maskSpan, &ctx->closure);
        });
      }
    }
    else
    {
      vblit_span = rasterFuncs.getCompositeFuncs(
        ctx->ops.op, ctx->paintLayer.secondaryFormat)->vblit_span[image_d->format];

      if (alpha == 0xFF)
      {
        RENDER_LOOP_SPAN(bltImageIndirectOpaqueMask, ctx->scanlineExt8, false, 0xFF,
        {
          ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
          vblit_span(pCur, maskSpan, &ctx->closure);
          ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
        });
      }
      else
      {
        RENDER_LOOP_SPAN(bltImageIndirectAlphaMask, ctx->scanlineExt8, true, alpha,
        {
          ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
          vblit_span(pCur, maskSpan, &ctx->closure);
          ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
        });
      }
    }
#undef RENDER_LOOP_SPAN
  }

  ctx->closure.srcPalette = NULL;
}

// ============================================================================
// [Fog::RasterPaintEngine - Renderers - Painting - DoPaintGlyphSet]
// ============================================================================

// TODO: Hardcoded to A8 glyph format.
// TODO: Region and mask clipping not implemented yet.
void RasterPaintEngine::CTX_SYMBOL(_doPaintGlyphSet)(
  RasterPaintContext* ctx, const IntPoint& pt, const GlyphSet& glyphSet, const IntBox& boundingBox)
{
  FOG_ASSERT(ctx->finalClipType != RASTER_CLIP_NULL);
  if (FOG_UNLIKELY(!glyphSet.getLength())) return;

  const Glyph* glyphs = glyphSet.glyphs();
  sysuint_t count = glyphSet.getLength();

  int px = pt.x;
  int py = pt.y;

  uint8_t* pixels = ctx->paintLayer.pixels;
  sysint_t stride = ctx->paintLayer.stride;

#if CTX_MT_MODE == 1
  int delta = ctx->delta;
  int offset = ctx->offset;
  sysint_t strideWithDelta = stride * delta;
#endif // CTX_MT_MODE

  // --------------------------------------------------------------------------
  // [Macros]
  // --------------------------------------------------------------------------

#define RENDER_LOOP(NAME, CODE) \
  { \
    SpanExt8 _maskSpan; \
    _maskSpan.setNext(NULL); \
    \
    for (sysuint_t i = 0; i < count; i++) \
    { \
      GlyphData* glyphd = glyphs[i]._d; \
      ImageData* bitmapd = glyphd->bitmap._d; \
      \
      int px1 = px + glyphd->offset.x; \
      int py1 = py + glyphd->offset.y; \
      int px2 = px1 + bitmapd->width; \
      int py2 = py1 + bitmapd->height; \
      \
      px += glyphd->advance; \
      \
      int x1 = px1; if (x1 < boundingBox.x1) x1 = boundingBox.x1; \
      int y1 = py1; if (y1 < boundingBox.y1) y1 = boundingBox.y1; \
      int x2 = px2; if (x2 > boundingBox.x2) x2 = boundingBox.x2; \
      int y2 = py2; if (y2 > boundingBox.y2) y2 = boundingBox.y2; \
      \
      if (CTX_MT_MODE) y1 = Raster_alignToDelta(y1, CTX_OFFSET, CTX_DELTA); \
      \
      int w = x2 - x1; if (w <= 0) continue; \
      int h = y2 - y1; if (h <= 0) continue; \
      \
      uint8_t* pCur = pixels; \
      pCur += (sysint_t)(uint)y1 * stride; \
      \
      sysint_t glyphStride = bitmapd->stride; \
      const uint8_t* pGlyph = bitmapd->first; \
      \
      pGlyph += (sysint_t)(uint)(y1 - py1) * glyphStride; \
      pGlyph += (sysint_t)(uint)(x1 - px1); \
      \
      if (CTX_MT_MODE) glyphStride *= CTX_DELTA; \
      \
      do { \
        _maskSpan.setPositionAndType(x1, x2, SPAN_TYPE_VMASK_A_SPARSE); \
        _maskSpan.setMaskPtr(const_cast<uint8_t*>(pGlyph)); \
        \
        Span8* maskSpan = &_maskSpan; \
        CODE \
        \
        pCur += CTX_STRIDE_WITH_DELTA; \
        pGlyph += glyphStride; \
        y1 += CTX_DELTA; \
      } while (y1 < y2); \
    } \
  }

  // --------------------------------------------------------------------------
  // [Solid Source]
  // --------------------------------------------------------------------------

  if (ctx->ops.sourceType == PAINTER_SOURCE_ARGB)
  {
    if (ctx->ops.alpha255 == 0xFF)
    {
      RasterCBlitSpanFn cblit_span = ctx->funcs->cblit_span[ctx->ops.sourceFormat];

      if (FOG_LIKELY(cblit_span != NULL))
      {
        RENDER_LOOP(bltSolidOpaqueDirect32,
        {
          cblit_span(pCur, &ctx->solid, maskSpan, &ctx->closure);
        })
      }
      else
      {
        cblit_span = rasterFuncs.getCompositeFuncs(
          ctx->ops.op, ctx->paintLayer.secondaryFormat)->cblit_span[ctx->ops.sourceFormat];

        RENDER_LOOP(bltSolidIndirect32,
        {
          ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
          cblit_span(pCur, &ctx->solid, maskSpan, &ctx->closure);
          ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
        })
      }
    }
    else
    {
      // TODO.
    }
  }

  // --------------------------------------------------------------------------
  // [Pattern Source]
  // --------------------------------------------------------------------------

  else
  {
    RasterPattern* pctx = ctx->pctx;
    if (!pctx) return;

    if (ctx->ops.alpha255 == 0xFF)
    {
      RasterVBlitSpanFn vblit_span = ctx->funcs->vblit_span[pctx->format];

      uint8_t* pPatternBuffer = ctx->getBuffer(
        (uint)(ctx->finalClipBox.getWidth()) * pctx->bytesPerPixel);
      if (!pPatternBuffer) return;

      if (FOG_LIKELY(vblit_span != NULL))
      {
        RENDER_LOOP(bltPatternOpaqueDirect32,
        {
          pctx->fetch(pctx, maskSpan, pPatternBuffer, y1, PATTERN_FETCH_CAN_USE_SRC);
          vblit_span(pCur, maskSpan, &ctx->closure);
        })
      }
      else
      {
        vblit_span = rasterFuncs.getCompositeFuncs(
          ctx->ops.op, ctx->paintLayer.secondaryFormat)->vblit_span[pctx->format];

        RENDER_LOOP(bltPatternIndirect32,
        {
          ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
          pctx->fetch(pctx, maskSpan, pPatternBuffer, y1, PATTERN_FETCH_CAN_USE_SRC);
          vblit_span(pCur, maskSpan, &ctx->closure);
          ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
        })
      }
    }
    else
    {
      // TODO.
    }
  }

#undef RENDER_LOOP
}

// ============================================================================
// [Fog::RasterPaintEngine - Renderers - Painting - DoPaintPath]
// ============================================================================

void RasterPaintEngine::CTX_SYMBOL(_doPaintPath)(
  RasterPaintContext* ctx, AnalyticRasterizer8* ras)
{
  FOG_ASSERT(ctx->finalClipType != RASTER_CLIP_NULL);

#define RENDER_LOOP(NAME, SCANLINE, CODE) \
  /* --------------------------------------------------------------------- */ \
  /* [Simple Clip]                                                         */ \
  /* --------------------------------------------------------------------- */ \
  if (FOG_LIKELY(ctx->finalClipType == RASTER_CLIP_SIMPLE)) \
  { \
    pBase = ctx->paintLayer.pixels + y * stride; \
    for (; y < y_end; y += CTX_DELTA, pBase += CTX_STRIDE_WITH_DELTA) \
    { \
      Span8* maskSpan = ras->sweepScanline(SCANLINE, y); \
      if (maskSpan == NULL) continue; \
      \
      uint8_t* pCur = pBase; \
      CODE \
    } \
  } \
  /* --------------------------------------------------------------------- */ \
  /* [Region Clip]                                                         */ \
  /* --------------------------------------------------------------------- */ \
  else if (ctx->finalClipType == RASTER_CLIP_REGION) \
  { \
    const IntBox* clipCur = ctx->finalRegion.getData(); \
    const IntBox* clipTo; \
    const IntBox* clipEnd = clipCur + ctx->finalRegion.getLength(); \
    sysuint_t clipLen; \
    \
    /* Advance clip pointer. */ \
NAME##_advance: \
    while (clipCur->y2 <= y) \
    { \
      if (++clipCur == clipEnd) goto NAME##_end; \
    } \
    /* Advance to end of the current span list (same y1, y2). */ \
    clipTo = clipCur + 1; \
    while (clipTo != clipEnd && clipCur->y1 == clipTo->y1) clipTo++; \
    clipLen = (sysuint_t)(clipTo - clipCur); \
    \
    /* Skip some rows if needed. */ \
    if (y < clipCur->y1) \
    { \
      y = clipCur->y1; \
      if (CTX_MT_MODE) \
      { \
        y = Raster_alignToDelta(y, CTX_OFFSET, CTX_DELTA); \
        if (y >= clipCur->y2) \
        { \
          clipCur = clipTo; \
          if (clipCur == clipEnd) goto NAME##_end; \
          goto NAME##_advance; \
        } \
      } \
    } \
    \
    pBase = ctx->paintLayer.pixels + y * stride; \
    for (; y < y_end; y += CTX_DELTA, pBase += CTX_STRIDE_WITH_DELTA) \
    { \
      /* Advance clip pointer if needed. */ \
      if (FOG_UNLIKELY(y >= clipCur->y2)) \
      { \
        clipCur = clipTo; \
        if (clipCur == clipEnd) goto NAME##_end; \
        goto NAME##_advance; \
      } \
      \
      Span8* maskSpan = ras->sweepScanline(SCANLINE, y, clipCur, clipLen); \
      if (maskSpan == NULL) continue; \
      \
      uint8_t* pCur = pBase; \
      CODE \
    } \
  } \
  /* --------------------------------------------------------------------- */ \
  /* [Mask Clip]                                                           */ \
  /* --------------------------------------------------------------------- */ \
  else /* if (ctx->finalClipType == RASTER_CLIP_MASK) */ \
  { \
    int maskY1 = ctx->maskY1; \
    int maskY2 = ctx->maskY2; \
    \
    if (y < maskY1) y = maskY1; \
    if (y_end > maskY2) y_end = maskY2; \
    \
    /* Skip some rows if needed. */ \
    if (CTX_MT_MODE) \
    { \
      y = Raster_alignToDelta(y, CTX_OFFSET, CTX_DELTA); \
      if (y >= y_end) goto NAME##_end; \
    } \
    \
    pBase = ctx->paintLayer.pixels + y * stride; \
    for (; y < y_end; y += CTX_DELTA, pBase += CTX_STRIDE_WITH_DELTA) \
    { \
      const Span8* clipSpan = ctx->getClipSpan(y); \
      if (clipSpan == NULL) continue; \
      \
      Span8* maskSpan = ras->sweepScanline(SCANLINE, y, RASTER_CLIP_SPAN_GET_USABLE(clipSpan)); \
      if (maskSpan == NULL) continue; \
      \
      uint8_t* pCur = pBase; \
      CODE \
    } \
  } \
NAME##_end: \
  ;

  int y = ras->getBoundingBox().y1;
  int y_end = ras->getBoundingBox().y2;
  sysint_t stride = ctx->paintLayer.stride;

#if CTX_MT_MODE == 1
  int delta = ctx->delta;
  int offset = ctx->offset;
  sysint_t strideWithDelta = stride * delta;
#endif // CTX_MT_MODE

  if (CTX_MT_MODE)
  {
    y = Raster_alignToDelta(y, CTX_OFFSET, CTX_DELTA);
    if (y >= y_end) return;
  }

  uint8_t* pBase;
  const RasterFuncs::CompositeFuncs* funcs = ctx->funcs;

  // --------------------------------------------------------------------------
  // [Solid Source]
  // --------------------------------------------------------------------------

  if (ctx->ops.sourceType == PAINTER_SOURCE_ARGB)
  {
    RasterCBlitSpanFn cblit_span = funcs->cblit_span[ctx->ops.sourceFormat];
    if (cblit_span)
    {
      RENDER_LOOP(bltSolidDirect32, ctx->scanlineSolid8,
      {
        cblit_span(pBase, &ctx->solid, maskSpan, &ctx->closure);
      });
    }
    else
    {
      funcs = rasterFuncs.getCompositeFuncs(ctx->ops.op, ctx->paintLayer.secondaryFormat);

      RENDER_LOOP(bltSolidIndirect32, ctx->scanlineSolid8,
      {
        ctx->paintLayer.toSecondarySpan(pBase, maskSpan, &ctx->closure);
        cblit_span(pBase, &ctx->solid, maskSpan, &ctx->closure);
        ctx->paintLayer.fromSecondarySpan(pBase, maskSpan, &ctx->closure);
      });
    }
  }

  // --------------------------------------------------------------------------
  // [Pattern Source]
  // --------------------------------------------------------------------------

  else
  {
    RasterPattern* pctx = ctx->pctx;
    if (!pctx) return;

    uint8_t* pPatternBuffer = ctx->getBuffer(
      (uint)(ctx->finalClipBox.getWidth()) * pctx->bytesPerPixel);
    if (!pPatternBuffer) return;

    RasterVBlitSpanFn vblit_span = funcs->vblit_span[pctx->format];
    if (vblit_span != NULL)
    {
      RENDER_LOOP(bltPatternDirect32, ctx->scanlineExt8,
      {
        pctx->fetch(pctx, maskSpan, pPatternBuffer, y, PATTERN_FETCH_CAN_USE_SRC);
        vblit_span(pCur, maskSpan, &ctx->closure);
      });
    }
    else
    {
      vblit_span = rasterFuncs.getCompositeFuncs(
        ctx->ops.op, ctx->paintLayer.secondaryFormat)->vblit_span[pctx->format];

      RENDER_LOOP(bltPatternIndirect32, ctx->scanlineExt8,
      {
        pctx->fetch(pctx, maskSpan, pPatternBuffer, y, PATTERN_FETCH_CAN_USE_SRC);
        ctx->paintLayer.toSecondarySpan(pCur, maskSpan, &ctx->closure);
        vblit_span(pCur, maskSpan, &ctx->closure);
        ctx->paintLayer.fromSecondarySpan(pCur, maskSpan, &ctx->closure);
      });
    }
  }
#undef RENDER_LOOP
}

} // Fog namespace

// Cleanup.
#undef CTX_STRIDE_WITH_DELTA
