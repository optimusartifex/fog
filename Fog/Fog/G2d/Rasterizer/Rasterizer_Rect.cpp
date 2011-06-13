// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/Face_C.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Rasterizer/Rasterizer_p.h>
#include <Fog/G2d/Rasterizer/Scanline_p.h>
#include <Fog/G2d/Rasterizer/Span_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>

namespace Fog {

// ============================================================================
// [Fog::Rasterizer8 - Rect - Sweep]
// ============================================================================

static Span8* _Rasterizer8_Rect_sweepSimple(
  Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y)
{
  y -= rasterizer->_boundingBox.y0;

  FOG_ASSERT(rasterizer->_isFinalized);
  FOG_ASSERT((uint)y < (uint)rasterizer->_boundingBox.y1);

  Rasterizer8::ShapeRect* shape = reinterpret_cast<Rasterizer8::ShapeRect*>(rasterizer->_rows);

  int x0 = shape->xLeft;
  int x1 = shape->xRight;
  int w = x1 - x0;

  if (scanline.begin(rasterizer->_boundingBox.x0, rasterizer->_boundingBox.x1) != ERR_OK)
    return NULL;

  const uint32_t* covers = shape->coverageI;

  if (y == 0)
    covers = shape->coverageT;
  else if (y == (shape->bounds.y1 - shape->bounds.y0))
    covers = shape->coverageB;

  scanline.newA8Extra_buf(x0, x0 + 1)[0] = covers[0];
  if (w > 1) scanline.lnkConstSpanOrMerge(x0 + 1, x1, covers[1]);
  if (w > 0 && covers[2]) scanline.lnkA8Extra_buf(x1, x1 + 1)[0] = covers[2];

  if (scanline.close() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  _Rasterizer_dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();
}

static Span8* _Rasterizer8_ShapeRect_sweepRegion(
  Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y,
  const BoxI* clipBoxes, size_t count)
{
  y -= rasterizer->_boundingBox.y0;

  FOG_ASSERT(rasterizer->_isFinalized);
  FOG_ASSERT((uint)y < (uint)rasterizer->_boundingBox.y1);

  Rasterizer8::ShapeRect* shape = reinterpret_cast<Rasterizer8::ShapeRect*>(rasterizer->_rows);

  int x0 = shape->xLeft;
  int x1 = shape->xRight;

  // Clipping.
  const BoxI* clipCur = clipBoxes;
  const BoxI* clipEnd = clipBoxes + count;
  if (FOG_UNLIKELY(clipCur == clipEnd)) return NULL;

  // Current clip box start / end point (not part of clip span).
  int clipX0;
  int clipX1 = clipCur->x1;

  // Advance clip (discard clip-boxes that can't intersect).
  while (clipX1 <= x0)
  {
    if (++clipCur == clipEnd) return NULL;
    clipX1 = clipCur->x1;
  }

  clipX0 = Math::max<int>(clipCur->x0, x0);
  if (clipX0 > x1) return NULL;

  if (scanline.begin(rasterizer->_boundingBox.x0, rasterizer->_boundingBox.x1) != ERR_OK)
    return NULL;

  const uint32_t* covers = shape->coverageI;
  if (FOG_UNLIKELY(y == 0))
    covers = shape->coverageT;
  else if (FOG_UNLIKELY(y == (shape->bounds.y1 - shape->bounds.y0)))
    covers = shape->coverageB;

  if (clipX0 == x0)
  {
    clipX0++;
    scanline.newA8Extra_buf(x0, clipX0)[0] = (uint16_t)covers[0];
  }

  uint16_t midcover = covers[1];
  for (;;)
  {
    if (clipX0 < x1)
    {
      scanline.lnkConstSpanOrMerge(clipX0, Math::min<int>(clipX1, x1), midcover);
    }
    if (clipX1 > x1)
    {
      scanline.lnkConstSpanOrMerge(x1, x1 + 1, covers[2]);
      break;
    }

    if (++clipBoxes == clipEnd) break;
    clipX0 = clipBoxes->x0;
    if (clipX0 > x1) break;
    clipX1 = clipBoxes->x1;
  }

  if (scanline.close() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  _Rasterizer_dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();
}

static Span8* _Rasterizer8_ShapeRect_sweepSpans(
  Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y,
  const Span8* clipSpans)
{
  // TODO: Rasterizer.

  return NULL;
}

// ============================================================================
// [Fog::Rasterizer8 - Rect - Init]
// ============================================================================

FOG_NO_EXPORT void _Rasterizer8_Rect_initSweepFunctions(Rasterizer8* rasterizer)
{
  rasterizer->_sweepSimpleFn = _Rasterizer8_Rect_sweepSimple;
  rasterizer->_sweepRegionFn = _Rasterizer8_ShapeRect_sweepRegion;
  rasterizer->_sweepSpansFn  = _Rasterizer8_ShapeRect_sweepSpans;
}

} // Fog namespace
