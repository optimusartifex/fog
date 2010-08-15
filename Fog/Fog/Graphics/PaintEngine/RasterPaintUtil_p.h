// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTUTIL_P_H
#define _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTUTIL_P_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Graphics/Geometry.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::RasterPaintEngine - Util - Multi-Threading]
// ============================================================================

static FOG_INLINE int Raster_alignToDelta(int y, int offset, int delta)
{
  FOG_ASSERT(offset >= 0);
  FOG_ASSERT(delta >= 1);
  FOG_ASSERT(offset < delta);

  // At this time we can expect that Y coordinate is always positive. I added
  // the assert here so if this will fail we will know, but it should remain!
  // Raster just can't contain negative coordinate, its [0, 0, width, height]
  FOG_ASSERT(y >= 0);

  uint mody = (uint)y % (uint)delta;

  // Not needed, Y is always positive.
  // if (mody < 0) mody += delta;

  uint newy = (uint)y - mody + (uint)offset;
  if (newy < (uint)y) newy += delta;

  return (int)newy;
}

// ============================================================================
// [Fog::RasterPaintEngine - Util - Painting]
// ============================================================================

//! @brief Whether the source rectangle @a src can be converted to aligned
//! rectangle @a dst, used to call various fast-paths.
static FOG_INLINE bool Raster_canAlignToGrid(IntBox& dst, const DoubleRect& src, double x, double y)
{
  int rx = Math::iround((src.x + x) * 256.0);
  if ((rx & 0xFF) != 0x00) return false;

  int ry = Math::iround((src.y + y) * 256.0);
  if ((ry & 0xFF) != 0x00) return false;

  int rw = Math::iround((src.w) * 256.0);
  if ((rw & 0xFF) != 0x00) return false;

  int rh = Math::iround((src.h) * 256.0);
  if ((rh & 0xFF) != 0x00) return false;

  dst.x1 = (int)(rx >> 8);
  dst.y1 = (int)(ry >> 8);
  dst.x2 = dst.x1 + (int)(rw >> 8);
  dst.y2 = dst.y1 + (int)(rh >> 8);

  return true;
}

#define RASTER_ENTER_PAINT_FUNC() \
  FOG_BEGIN_MACRO \
    if (FOG_UNLIKELY(ctx.state != 0)) \
    { \
      if (ctx.state & (RASTER_STATE_NO_PAINT_MASK)) return ERR_OK; \
      if (ctx.state & (RASTER_STATE_PENDING_MASK)) _postPending(); \
    } \
  FOG_END_MACRO

#define RASTER_ENTER_PAINT_COND(__condition__) \
  FOG_BEGIN_MACRO \
    if (FOG_UNLIKELY(!(__condition__))) return ERR_OK; \
    \
    if (FOG_UNLIKELY(ctx.state != 0)) \
    { \
      if (ctx.state & (RASTER_STATE_NO_PAINT_MASK)) return ERR_OK; \
      if (ctx.state & (RASTER_STATE_PENDING_MASK)) _postPending(); \
    } \
  FOG_END_MACRO

#define RASTER_ENTER_PAINT_IMAGE(__image__) \
  FOG_BEGIN_MACRO \
    if (FOG_UNLIKELY(__image__.isEmpty())) return ERR_OK; \
    \
    if (FOG_UNLIKELY(ctx.state != 0)) \
    { \
      /* Image is source so we need to exclude RASTER_STATE_NO_PAINT_SOURCE flag. */ \
      if (ctx.state & (RASTER_STATE_NO_PAINT_MASK & ~(RASTER_STATE_NO_PAINT_SOURCE))) return ERR_OK; \
      if (ctx.state & (RASTER_STATE_PENDING_MASK)) _postPending(); \
    } \
  FOG_END_MACRO

// Called by serializers to ensure that pattern context is created if needed.
#define RASTER_SERIALIZE_ENSURE_PATTERN() \
  FOG_BEGIN_MACRO \
    if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN && FOG_UNLIKELY(_getRasterPatternContext() == NULL)) \
      return ERR_RT_OUT_OF_MEMORY; \
  FOG_END_MACRO

// ============================================================================
// [Fog::RasterPaintEngine - Util - Clipping]
// ============================================================================

// Get usable span instance from span retrieved from mask. We encode 'owned'
// bit into pointer itself so it's needed to clear it to access the instance.
#define RASTER_CLIP_SPAN_GET_USABLE(__span__) \
  ( (Span8*) ((sysuint_t)(__span__) & ~1) )

// Get whether a given span instance is 'owned' or not. Owned means that it
// can be directly manipulated by clipping method. If span is not owned then
// you must replace the entire clip row with new span list.
#define RASTER_CLIP_SPAN_IS_OWNED(__span__) \
  (((sysuint_t)(__span__) & 0x1) == 0)

// Whether a given span instance is VSpan that contains own embedded clip mask.
#define RASTER_CLIP_IS_EMBEDDED_VSPAN(__span__) \
  (reinterpret_cast<const uint8_t*>(__span__) + sizeof(RasterClipVSpan8) == __span__->getMaskPtr())

#define RASTER_ENTER_CLIP_FUNC() \
  FOG_BEGIN_MACRO \
    if (clipOp >= CLIP_OP_COUNT) return ERR_RT_INVALID_ARGUMENT; \
    \
    if (FOG_UNLIKELY(ctx.state & RASTER_STATE_NO_PAINT_WORK_REGION)) \
      return ERR_OK; \
  FOG_END_MACRO

#define RASTER_ENTER_CLIP_COND(__condition__) \
  FOG_BEGIN_MACRO \
    if (clipOp >= CLIP_OP_COUNT) return ERR_RT_INVALID_ARGUMENT; \
    \
    if (FOG_UNLIKELY(ctx.state & RASTER_STATE_NO_PAINT_WORK_REGION)) \
      return ERR_OK; \
    \
    if (FOG_UNLIKELY(!(__condition__))) \
      return _clipOpNull(clipOp); \
    \
  FOG_END_MACRO

//! @brief Get clip type from source region (src).
static FOG_INLINE uint32_t Raster_getClipType(const Region& src)
{
  sysuint_t len = src.getLength();
  if (len > 2) len = 2;
  return (uint32_t)len;
}

// TODO: Verify and use.
//
// Binary search region (YX sorted rectangles) and match the first one that
// contains a given 'y'. If there is no such region then the next one will be
// returned
static FOG_INLINE const IntBox* Raster_searchRegion(const IntBox* start, sysuint_t length, int y)
{
  FOG_ASSERT(start != NULL);
  FOG_ASSERT(length > 0);

  // Binary search for matching position.
  const IntBox* base = start;
  const IntBox* r;

  sysuint_t i;

  for (i = length; i != 0; i >>= 1)
  {
    r = base + (i >> 1);

    // Try match.
    if (y >= r->y1)
    {
      if (y < r->y2) return r;
      // else: Move left.
    }
    else if (r->y2 <= y)
    {
      // Move right.
      base = r + 1;
      i--;
    }
    // else: Move left.
  }
  return r;
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTENGINE_RASTERPAINTUTIL_P_H
