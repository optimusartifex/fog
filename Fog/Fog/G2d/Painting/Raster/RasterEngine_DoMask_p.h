// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// This file is included twice:
//
//   1. For single-threaded mode (CTX_MT_MODE == 0).
//   2. For multi-threaded mode (CTX_MT_MODE == 1).
//
// The purpose of this file is to simplify development of single-threaded and
// multi-threaded modes, and to keep performance at high level (this is reason
// why an inner loops are implemented twice, but they are similar).

// Macro definitions:
//
// CTX_SYMBOL - Add suffix to the symbol (_ST or _MT), see Raster_Engine_p.h for
// naming convention.
//
// CTX_OFFSET - Context offset (ctx->offset) or 0.
// CTX_DELTA - Context delta (ctx->delta) or 1.
// CTX_STRIDE_WITH_DELTA - Stride multiplied by delta or non-modified stride.

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
// [Fog::RasterPaintEngine - Renderers - Clipping - DoMaskConvert]
// ============================================================================

void RasterPaintEngine::CTX_SYMBOL(_doMaskConvert)(
  RasterContext* ctx, const BoxI* box, sysuint_t count)
{
  // Can't be called with empty region.
  FOG_ASSERT(count > 0);

  const BoxI* curBand;
  const BoxI* endBand = box;
  const BoxI* endRegion = endBand + count;

  int y0;
  int y2;
  int yb = endBand->y2;

  // Mask should be non-initialized. We are not checking it so we can leak if
  // it contains something.
  FOG_ASSERT(ctx->maskY0 == ctx->maskY2);

  y0 = endBand->y0;
  y2 = endRegion[-1].y2;

  ctx->maskY0 = y0;
  ctx->maskY2 = y2;

  do {
    curBand = endBand;
    endBand = curBand + 1;

    y0 = curBand->y0;
    y2 = curBand->y2;

    // Fix uninitialized pointers to spans if needed.
    while (yb < y0) { ctx->setClipSpan(yb, NULL); yb++; }

    // Get the whole band.
    while (endBand != endRegion && endBand->y0 == y0) endBand++;

    do {
      const BoxI* curBox = curBand;
      Span8* first = ctx->allocCSpan();
      Span8* span = first;

      for (;;)
      {
        span->setPositionAndType(curBox->x0, curBox->x1, SPAN_C);
        span->setCMask(0xFF);

        if (++curBox == endBand) break;

        Span8* next = ctx->allocCSpan();
        span->setNext(next);
        span = next;
      }

      span->setNext(NULL);
      ctx->setClipSpan(y0, first);
    } while (++y0 < y2);

    yb = y2;
  } while (endBand != endRegion);
}

// ============================================================================
// [Fog::RasterPaintEngine - Renderers - Clipping - DoMaskBoxes]
// ============================================================================

void RasterPaintEngine::CTX_SYMBOL(_doMaskBoxes)(
  RasterContext* ctx, const BoxI* box, sysuint_t count, uint32_t clipOp)
{
  // Can't be called with empty region.
  FOG_ASSERT(count > 0);

  const BoxI* curInBand;
  const BoxI* endInBand = box;
  const BoxI* endInRegion = endInBand + count;

  int y0 = endInBand->y0;
  int y2 = endInRegion[-1].y2;
  int yb = endInBand->y2;

#if CTX_MT_MODE == 1
  int delta = ctx->delta;
  int offset = ctx->offset;

  y0 = RasterUtil::alignToDelta(y0, CTX_OFFSET, CTX_DELTA);
#endif // CTX_MT_MODE

  // --------------------------------------------------------------------------
  // [Macros - Begin]
  // --------------------------------------------------------------------------

#define RASTER_CLIP_BOX_LOOP_BEGIN() \
  do { \
    curInBand = endInBand; \
    endInBand = curInBand + 1; \
    \
    y0 = curInBand->y0; \
    y2 = curInBand->y2; \
    \
    /* Get the whole band. */ \
    while (endInBand != endInRegion && endInBand->y2 == y2) endInBand++; \
    \
    if (CTX_MT_MODE) \
    { \
      y0 = RasterUtil::alignToDelta(y0, CTX_OFFSET, CTX_DELTA); \
      if (y0 >= y2) \
      { \
        if (y0 >= ctx->maskY2 || endInBand == endInRegion) break; \
        continue; \
      } \
    }

#define RASTER_CLIP_BOX_SKIP_NON_INTERSECT_INPUT() \
    if (y0 < ctx->maskY0) \
    { \
      if (y2 <= ctx->maskY0) continue; \
      \
      if (CTX_MT_MODE) \
      { \
        y0 = RasterUtil::alignToDelta(ctx->maskY0, CTX_OFFSET, CTX_DELTA); \
        if (y0 >= y2) \
        { \
          if (y0 >= ctx->maskY2 || endInBand == endInRegion) break; \
          continue; \
        } \
      } \
    } \
    \
    if (y0 >= ctx->maskY2) break; \
    if (y2 > ctx->maskY2) y2 = ctx->maskY2; \

#define RASTER_CLIP_BOX_HOLE_BEGIN() \
    if (FOG_LIKELY(endInBand != endInRegion) && endInBand->y0 > y2) \
    { \
      int hole_end_y = Math::min<int>(endInBand->y0, ctx->maskY2); \
      while (y0 < hole_end_y) \
      {

#define RASTER_CLIP_BOX_HOLE_END() \
        y0 += CTX_DELTA; \
      } \
    }

#define RASTER_CLIP_BOX_LOOP_END() \
    yb = y2; \
    if (CTX_MT_MODE) yb = RasterUtil::alignToDelta(yb, CTX_OFFSET, CTX_DELTA); \
  } while (endInBand != endInRegion);

#define ADD_NEW_CSPAN() \
  { \
    Span8* __span = ctx->allocCSpan(); \
    curOutSpan->setNext(__span); \
    curOutSpan = __span; \
  }

#define ADD_NEW_VSPAN_COPY(__x0__, __x1__, __type__, __data__) \
  { \
    int __x0 = __x0__; \
    int __x1 = __x1__; \
    uint8_t* __data = __data__; \
    \
    do { \
      int __len = __x1 - __x0; \
      if (__len > RASTER_MASK_VSPAN_MAX_SIZE) \
        __len = RASTER_MASK_VSPAN_MAX_SIZE; \
      \
      Span8* __span = ctx->allocVSpan(__len); \
      __span->setPositionAndType(__x0, __x0 + __len, __type__); \
      memcpy(__span->getGenericMask(), __data, __len); \
      \
      __x0 += __len; \
      __data += __len; \
      \
      curOutSpan->setNext(__span); \
      curOutSpan = __span; \
    } while (__x0 != __x1); \
  }

#define ADD_NEW_VSPAN_ADOPT(__x0__, __x1__, __type__, __data__) \
  { \
    int __x0 = __x0__; \
    int __x1 = __x1__; \
    \
    Span8* __span = ctx->allocCSpan(); \
    __span->setPositionAndType(__x0, __x1, __type__); \
    __span->setGenericMask(__data__); \
    \
    curOutSpan->setNext(__span); \
    curOutSpan = __span; \
  }

#define ADD_CUR_SPAN() \
  { \
    curOutSpan->setNext(curInSpan); \
    curOutSpan = curInSpan; \
  }

#define NEXT_CUR_SPAN(__label__) \
  { \
    curInSpan = curInSpan->getNext(); \
    if (curInSpan == NULL) goto __label__; \
  }

#define NEXT_CUR_SPAN_AND_FREE_VSPAN(__label__) \
  { \
    Span8* __span = curInSpan->getNext(); \
    ctx->freeVSpan(curInSpan); \
    \
    curInSpan = __span; \
    if (curInSpan == NULL) goto __label__; \
  }

  // --------------------------------------------------------------------------
  // [ClipOp]
  // --------------------------------------------------------------------------

  switch (clipOp)
  {
    // ------------------------------------------------------------------------
    // [ClipOp - Replace]
    // ------------------------------------------------------------------------

    case CLIP_OP_REPLACE:
      // TODO: restore() state and then call _doMaskConvert!

      // If mask is not initialized, use _doMaskConvert instead.
      if (ctx->maskY0 == ctx->maskY2)
      {
        CTX_SYMBOL(_doMaskConvert)(ctx, box, count);
        return;
      }
      else
      {
        int w1 = ctx->maskY0;
        int w2 = ctx->maskY2;

        ctx->maskY0 = y0;
        ctx->maskY2 = y2;

        if (w1 < y0)
        {
          do {
            Mask_clearRow(ctx, w1);
          } while (++w1 < y0);
        }
        else if (w1 > y0)
        {
          do {
            ctx->setClipSpan(w1, NULL);
          } while (--w1 > y0);
        }

        if (w2 > y2)
        {
          do {
            w2--;
            Mask_clearRow(ctx, w2);
          } while (w2 > y2);
        }
        else if (w2 < y2)
        {
          do {
            ctx->setClipSpan(w2, NULL);
          } while (++w2 < y0);
        }

        RASTER_CLIP_BOX_LOOP_BEGIN()
          do {
            const BoxI* curInBox = curInBand;

            Span8 baseSpan;
            Span8* curOutSpan = ctx->getClipSpan(y0);

            // If span is not null and it's owned then we need to free it.
            if (curOutSpan != NULL && RASTER_CLIP_SPAN_IS_OWNED(curOutSpan))
            {
              ctx->freeChainedSpans(curOutSpan);
            }

            curOutSpan = &baseSpan;
            do {
              ADD_NEW_CSPAN()
              curOutSpan->setPosition(curInBox->x0, curInBox->x1);
              curOutSpan->setCMask(0xFF);
            } while (++curInBox != endInBand);

            ctx->setClipSpan(y0, baseSpan.getNext());
          } while (++y0 < y2);

          // Clear all "holes", the vertical area not covered by boxes.
          RASTER_CLIP_BOX_HOLE_BEGIN()
            Mask_clearRow(ctx, y0);
          RASTER_CLIP_BOX_HOLE_END()
        RASTER_CLIP_BOX_LOOP_END()
      }
      break;

    // ------------------------------------------------------------------------
    // [ClipOp - Intersect]
    // ------------------------------------------------------------------------

    case CLIP_OP_INTERSECT:
      if (ctx->maskY0 == ctx->maskY2)
      {
        return;
      }
      else
      {
        int w1 = ctx->maskY0;
        int w2 = ctx->maskY2;

        // TODO: Multithreading.
        if (CTX_MT_MODE)
        {
          w1 = RasterUtil::alignToDelta(w1, CTX_OFFSET, CTX_DELTA);
        }

        if (w1 < y0)
        {
          do {
            Mask_clearRow(ctx, w1);
            w1 += CTX_DELTA;
          } while (w1 < y0);
          ctx->maskY0 = w1;
        }

        if (w2 > y2)
        {
          do {
            w2 -= CTX_DELTA;
            Mask_clearRow(ctx, w2);
          } while (w2 > y2);
          ctx->maskY2 = w2;
        }

        RASTER_CLIP_BOX_LOOP_BEGIN()
          // Skip non-intersecting boxes.
          RASTER_CLIP_BOX_SKIP_NON_INTERSECT_INPUT()

          do {
            const BoxI* curInBox = curInBand;
            Span8* curInSpan = ctx->getClipSpan(y0);

            Span8 baseSpan;
            Span8* curOutSpan = &baseSpan;

            // ----------------------------------------------------------------
            // [INTERSECT - OWNED]
            // ----------------------------------------------------------------

            if (RASTER_CLIP_SPAN_IS_OWNED(curInSpan))
            {
              // No spans == no intersection with.
              if (curInSpan == NULL) goto intersectNop;

intersectOwnedBegin:
              // Skip spans that can't intersect.
              int x = curInBox->x0;
              while (curInSpan->getX1() <= x)
              {
intersectOwnedRepeatSpan:
                Span8* next = curInSpan->getNext();
                ctx->freeUnknownSpan(curInSpan);
                if ((curInSpan = next) == NULL) goto intersectDone;
              }

              // Skip boxes that can't intersect.
              x = curInSpan->getX0();
              while (curInBox->x1 <= x)
              {
intersectOwnedRepeatBox:
                if (++curInBox == endInBand) goto intersectOwnedDone;
              }

              // Repeat skip steps if needed.
intersectOwnedAgain:
              x = curInBox->x0;
              if (curInSpan->getX1() <= x) goto intersectOwnedRepeatSpan;

              // Handle the intersection.
              {
                int minX = Math::max<int>(x, curInSpan->getX0());
                int maxX = curInBox->x1;

                // Process all spans that fits into the current box (curInBox).
                if (curInSpan->getX1() <= maxX)
                {
                  // If the curInSpan is CMask then we can reuse it. We can
                  // also reuse overlapped spans.
                  if (curInSpan->isConst() || curInSpan->getX0() >= minX)
                  {
                    // CurInSpan becomes curOutSpan.
                    ADD_CUR_SPAN()
                    curOutSpan->setX0(minX);
                    NEXT_CUR_SPAN(intersectDone)
                  }
                  // If this is VSpan without own memory we can reuse
                  // it too, we only need to adjust the mask pointer.
                  else if (!RASTER_CLIP_IS_EMBEDDED_VSPAN(curInSpan))
                  {
                    // CurInSpan becomes curOutSpan.
                    ADD_CUR_SPAN()
                    curOutSpan->_mask += x - minX;
                    curOutSpan->setX0(minX);
                    NEXT_CUR_SPAN(intersectDone)
                  }
                  else
                  {
                    // We must copy.
                    ADD_NEW_VSPAN_COPY(minX, curInSpan->getX1(), curInSpan->getType(),
                      curInSpan->getGenericMask() - curInSpan->getX0() + minX);
                    NEXT_CUR_SPAN_AND_FREE_VSPAN(intersectDone)
                  }

                  // Merge each next span. We are sure here that each span in
                  // the loop is overlapped (so we don't need to modify it).
                  curOutSpan->setNext(curInSpan);
                  while (curInSpan->getX1() <= maxX)
                  {
                    curOutSpan = curInSpan;
                    NEXT_CUR_SPAN(intersectDone)
                  }

                  x = minX = curInSpan->getX0();
                  if (minX >= maxX) goto intersectOwnedRepeatBox;
                }

                // We have two choices here:
                //
                // 1. Next box not overlaps with current span or it is the last
                //    one (the easiest case).
                if (curInBox + 1 == endInBand || curInBox[1].x0 > curInSpan->getX1())
                {
                  maxX = Math::min<int>(maxX, curInSpan->getX1());

                  // Completely outside?
                  if (minX >= maxX)
                  {
                    x = minX;
                    goto intersectOwnedRepeatBox;
                  }

                  // If the curInSpan is CMask then we can reuse it. We can
                  // also reuse overlapped spans.
                  if (curInSpan->isConst() || curInSpan->getX0() >= minX)
                  {
                    // CurInSpan becomes curOutSpan.
                    ADD_CUR_SPAN()
                    curOutSpan->setPosition(minX, maxX);
                    NEXT_CUR_SPAN(intersectDone)
                  }
                  // If this is VSpan without own memory we can reuse
                  // it too, we only need to adjust the mask pointer.
                  else if (!RASTER_CLIP_IS_EMBEDDED_VSPAN(curInSpan))
                  {
                    // CurInSpan becomes curOutSpan.
                    ADD_CUR_SPAN()
                    curOutSpan->_mask += x - minX;
                    curOutSpan->setPosition(minX, maxX);
                    NEXT_CUR_SPAN(intersectDone)
                  }
                  else if (curInSpan->getX0() == minX)
                  {
                    // Hacky, we will adjust the size of span, because we don't
                    // need to modify mask pointer. So the span type remains
                    // (it's EmbeddedVSpan), but the length (x1) will be decreased.
                    ADD_CUR_SPAN()
                    curOutSpan->setX1(maxX);
                    NEXT_CUR_SPAN(intersectDone)
                  }
                  else
                  {
                    // We must copy.
                    ADD_NEW_VSPAN_COPY(minX, maxX, curInSpan->getType(),
                      curInSpan->getGenericMask() - curInSpan->getX0() + minX)
                    NEXT_CUR_SPAN_AND_FREE_VSPAN(intersectDone)
                  }

                  curInBox++;
                  if (curInBox == endInBand)
                    goto intersectOwnedDone;
                  else
                    goto intersectOwnedBegin;
                }
                // 2. Next box overlaps with current span. In this case we need
                //    to create new span (to finish current box) and then move,
                //    this means doing the entire machinery again.
                else
                {
                  if (curInSpan->isConst())
                  {
                    ADD_NEW_CSPAN()
                    curOutSpan->setPosition(minX, maxX);
                    curOutSpan->setCMask(curInSpan->getConstMask());
                  }
                  else if (!(RASTER_CLIP_IS_EMBEDDED_VSPAN(curInSpan)))
                  {
                    ADD_NEW_VSPAN_ADOPT(minX, maxX, curInSpan->getType(),
                      curInSpan->getGenericMask() + minX - curInSpan->getX0())
                  }
                  else
                  {
                    ADD_NEW_VSPAN_COPY(minX, maxX, curInSpan->getType(),
                      curInSpan->getGenericMask() + minX - curInSpan->getX0())
                  }

                  curInBox++;
                  goto intersectOwnedAgain;
                }
              }
intersectOwnedDone:
              if (curInSpan) ctx->freeChainedSpans(curInSpan);
            }

            // ----------------------------------------------------------------
            // [INTERSECT - SAVED]
            // ----------------------------------------------------------------

            else
            {
              curInSpan = RASTER_CLIP_SPAN_GET_USABLE(curInSpan);

              // Try if it's NOP (if each clip-span is overlapped).
              if (Mask_overlaps(curInSpan, curInBox, endInBand)) goto intersectNop;

intersectSavedBegin:
              // Skip spans that can't intersect.
              int x = curInBox->x0;
              while (curInSpan->getX1() <= x)
              {
intersectSavedRepeatSpan:
                curInSpan = curInSpan->getNext();
                if (curInSpan == NULL) goto intersectDone;
              }

              // Skip boxes that can't intersect.
              x = curInSpan->getX0();
              while (curInBox->x1 <= x)
              {
intersectSavedRepeatBox:
                if (++curInBox == endInBand) goto intersectDone;
              }

              // Repeat skip steps if needed.
intersectSavedAgain:
              x = curInBox->x0;
              if (curInSpan->getX1() <= x) goto intersectSavedRepeatSpan;

              // Handle the intersection.
              {
                int minX = Math::max<int>(x, curInSpan->getX0());
                int maxX = curInBox->x1;

                // Process all spans that fits into the current box (curInBox).
                if (curInSpan->getX1() <= maxX)
                {
                  ADD_NEW_CSPAN()
                  curOutSpan->setPosition(minX, curInSpan->getX1());
                  curOutSpan->setGenericMask(curInSpan->getGenericMask());
                  if (!curOutSpan->isConst())
                    curOutSpan->_mask += minX - curInSpan->getX0();
                  NEXT_CUR_SPAN(intersectDone)

                  while (curInSpan->getX1() <= maxX)
                  {
                    ADD_NEW_CSPAN()
                    curOutSpan->setPosition(curInSpan->getX0(), curInSpan->getX1());
                    curOutSpan->setGenericMask(curInSpan->getGenericMask());
                    NEXT_CUR_SPAN(intersectDone)
                  }

                  minX = curInSpan->getX0();
                }

                // We have two choices here:
                //
                // 1. Next box not overlaps with current span or this box is the
                //    last one (easiest case).
                if (curInBox + 1 == endInBand || curInBox[1].x0 > curInSpan->getX1())
                {
                  maxX = Math::min<int>(maxX, curInSpan->getX1());

                  // Completely outside?
                  if (minX >= maxX)
                  {
                    x = minX;
                    goto intersectSavedRepeatBox;
                  }

                  ADD_NEW_CSPAN()
                  curOutSpan->setPosition(minX, maxX);
                  curOutSpan->setGenericMask(curInSpan->getGenericMask());
                  if (!curOutSpan->isConst())
                    curOutSpan->_mask += minX - curInSpan->getX0();
                  NEXT_CUR_SPAN(intersectDone)

                  curInBox++;
                  if (curInBox == endInBand)
                    goto intersectDone;
                  else
                    goto intersectSavedBegin;
                }
                // 2. Next box overlaps with current span.
                else
                {
                  maxX = Math::min<int>(maxX, curInSpan->getX1());
                  FOG_ASSERT(minX < maxX);

                  ADD_NEW_CSPAN()
                  curOutSpan->setPosition(minX, maxX);
                  curOutSpan->setGenericMask(curInSpan->getGenericMask());
                  if (!curOutSpan->isConst())
                    curOutSpan->_mask += minX - curInSpan->getX0();

                  curInBox++;
                  goto intersectSavedAgain;
                }
              }
            }

intersectDone:
            if (curOutSpan) curOutSpan->setNext(NULL);
            ctx->setClipSpan(y0, baseSpan.getNext());

intersectNop:
            ;
            y0 += CTX_DELTA;
          } while (y0 < y2);

          RASTER_CLIP_BOX_HOLE_BEGIN()
            Mask_clearRow(ctx, y0);
          RASTER_CLIP_BOX_HOLE_END()
        RASTER_CLIP_BOX_LOOP_END()
      }
      break;

    // ------------------------------------------------------------------------
    // [ClipOp - Invalid]
    // ------------------------------------------------------------------------

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // --------------------------------------------------------------------------
  // [Macros - Cleanup]
  // --------------------------------------------------------------------------

#undef RASTER_CLIP_BOX_LOOP_BEGIN
#undef RASTER_CLIP_BOX_SKIP_NON_INTERSECT_INPUT
#undef RASTER_CLIP_BOX_HOLE_BEGIN
#undef RASTER_CLIP_BOX_HOLE_END
#undef RASTER_CLIP_BOX_LOOP_END
#undef ADD_NEW_CSPAN
#undef ADD_NEW_VSPAN_COPY
#undef ADD_NEW_VSPAN_ADOPT
#undef ADD_CUR_SPAN
#undef NEXT_CUR_SPAN
#undef NEXT_CUR_SPAN_AND_FREE_VSPAN
}

// ============================================================================
// [Fog::RasterPaintEngine - Renderers - Clipping - DoMaskPath]
// ============================================================================

void RasterPaintEngine::CTX_SYMBOL(_doMaskPath)(
  RasterContext* ctx, Rasterizer8* ras, uint32_t clipOp)
{
  int y = ras->getBoundingBox().y0;
  int y_end = ras->getBoundingBox().y2;

#if CTX_MT_MODE == 1
  int delta = ctx->delta;
  int offset = ctx->offset;
#endif // CTX_MT_MODE

  if (y < ctx->maskY0) y = ctx->maskY0;
  if (y_end > ctx->maskY2) y_end = ctx->maskY2;

  if (CTX_MT_MODE)
  {
    y = RasterUtil::alignToDelta(y, CTX_OFFSET, CTX_DELTA);
    if (y >= y_end) return;
  }

  switch (clipOp)
  {
    // ------------------------------------------------------------------------
    // [ClipOp - Replace]
    // ------------------------------------------------------------------------

    case CLIP_OP_REPLACE:
    {
      // TODO.
      break;
    }

    // ------------------------------------------------------------------------
    // [ClipOp - Intersect]
    // ------------------------------------------------------------------------

    case CLIP_OP_INTERSECT:
    {
      int w1 = ctx->maskY0;
      int w2 = ctx->maskY2;
      if (w1 == w2) return;

      while (w1 < y)
      {
        Mask_clearRow(ctx, w1);
        w1++;
      }

      while (w2 > y_end)
      {
        w2--;
        Mask_clearRow(ctx, w2);
      }

      ctx->maskY0 = w1;
      ctx->maskY2 = w2;

      for (; y < y_end; y += CTX_DELTA)
      {
        Span8* curInSpan = ctx->getClipSpan(y);

        Span8 baseSpan;
        Span8* curOutSpan = &baseSpan;

        if (RASTER_CLIP_SPAN_IS_OWNED(curInSpan))
        {
          if (curInSpan == NULL) continue;

          Span8* span = ras->sweepScanline(ctx->scanlineSolid8, ctx->buffer, y, curInSpan);
          ctx->freeChainedSpans(curInSpan);

          if (span == NULL)
          {
            ctx->setClipSpan(y, NULL);
            goto intersectNop;
          }
          curInSpan = span;
        }
        else
        {
          curInSpan = ras->sweepScanline(ctx->scanlineSolid8, ctx->buffer, y, RASTER_CLIP_SPAN_GET_USABLE(curInSpan));
          if (curInSpan == NULL)
          {
            ctx->setClipSpan(y, NULL);
            goto intersectNop;
          }
        }

        do {
          if (curInSpan->isConst())
          {
            Span8* span = ctx->allocCSpan();

            span->setPositionAndType(curInSpan->getX0(), curInSpan->getX1(), SPAN_C);
            span->setCMask(curInSpan->getConstMask());

            curOutSpan->setNext(span);
            curOutSpan = span;
          }
          else
          {
            int x0 = curInSpan->getX0();
            int x1 = curInSpan->getX1();
            uint8_t* m = curInSpan->getGenericMask();
            do {
              int len = Math::min<int>(x1 - x0, RASTER_MASK_VSPAN_MAX_SIZE);
              Span8* span = ctx->allocVSpan(len);

              span->setPositionAndType(x0, x0 + len, curInSpan->getType());
              memcpy(span->getGenericMask(), m, len);

              x0 += len;
              m += len;

              curOutSpan->setNext(span);
              curOutSpan = span;
            } while (x0 != x1);
          }

          curInSpan = curInSpan->getNext();
        } while (curInSpan);

        curOutSpan->setNext(NULL);
        ctx->setClipSpan(y, baseSpan.getNext());

intersectNop:
        ;
      }
      break;
    }

    // ------------------------------------------------------------------------
    // [ClipOp - Invalid]
    // ------------------------------------------------------------------------

    default:
      FOG_ASSERT_NOT_REACHED();
      break;
  }
end:
  ;
}

} // Fog namespace

// Cleanup.
#undef CTX_STRIDE_WITH_DELTA
