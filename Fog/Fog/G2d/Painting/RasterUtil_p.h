// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERUTIL_P_H
#define _FOG_G2D_PAINTING_RASTERUTIL_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Convert.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Render/RenderStructs_p.h>

namespace Fog {
namespace RasterUtil {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterUtil - Debugging]
// ============================================================================

#if defined(FOG_DEBUG)
// Check whether all spans are inside screen.
template<typename SpanT>
static FOG_INLINE void validateSpans(const SpanT* span, int screenX0, int screenX1)
{
  while (span != NULL)
  {
    int x0 = span->getX0();
    int x1 = span->getX1();

    FOG_ASSERT(x0 < x1);
    FOG_ASSERT(x0 >= screenX0);
    FOG_ASSERT(x1 <= screenX1);

    span = span->getNext();
  }
}

#endif // FOG_DEBUG

// ============================================================================
// [Fog::RasterUtil - Multi-Threading]
// ============================================================================

static FOG_INLINE int alignToDelta(int y, int offset, int delta)
{
  FOG_ASSERT(offset >= 0);
  FOG_ASSERT(delta >= 1);
  FOG_ASSERT(offset < delta);

  // At this time we can expect that Y coordinate is always positive. I added
  // the assert here so if this will fail we will know, but it should remain!
  // Raster just can't contain negative coordinate, its [0, 0, width, height].
  FOG_ASSERT(y >= 0);

  uint mody = (uint)y % (uint)delta;

  // Not needed, Y is always positive.
  // if (mody < 0) mody += delta;

  uint newy = (uint)y - mody + (uint)offset;
  if (newy < (uint)y) newy += delta;

  return (int)newy;
}

// ============================================================================
// [Fog::RasterUtil - Pattern]
// ============================================================================

static FOG_INLINE bool isSolidContext(RenderPatternContext* pc)
{
  return (size_t)pc == (size_t)0x1;
}

static FOG_INLINE bool isPatternContext(RenderPatternContext* pc)
{
  return (size_t)pc > (size_t)0x1;
}

// ============================================================================
// [Fog::RasterUtil - CompositingOperator]
// ============================================================================

static FOG_INLINE bool isCompositingOperatorNop(uint32_t op)
{
  return op == COMPOSITE_DST;
}

static FOG_INLINE bool isCompositeCopyOp(uint32_t dstFormat, uint32_t srcFormat, uint32_t compositingOperator)
{
  if (compositingOperator > COMPOSITE_SRC_OVER)
    return false;

  const ImageFormatDescription& dstDescription = ImageFormatDescription::getByFormat(dstFormat);
  const ImageFormatDescription& srcDescription = ImageFormatDescription::getByFormat(srcFormat);

  if (dstDescription.getDepth() != srcDescription.getDepth())
    return false;

  return compositingOperator == COMPOSITE_SRC || srcDescription.getASize() == 0;
};

//! @brief Whether the source rectangle @a src can be converted to aligned
//! rectangle @a dst, used to call various fast-paths.
static FOG_INLINE bool canAlignToGrid(BoxI& dst, const RectD& src, double x, double y)
{
  int rx = Math::iround((src.x + x) * 256.0);
  if ((rx & 0xFF) != 0x00) return false;

  int ry = Math::iround((src.y + y) * 256.0);
  if ((ry & 0xFF) != 0x00) return false;

  int rw = Math::iround((src.w) * 256.0);
  if ((rw & 0xFF) != 0x00) return false;

  int rh = Math::iround((src.h) * 256.0);
  if ((rh & 0xFF) != 0x00) return false;

  dst.x0 = (int)(rx >> 8);
  dst.y0 = (int)(ry >> 8);
  dst.x1 = dst.x0 + (int)(rw >> 8);
  dst.y1 = dst.y0 + (int)(rh >> 8);

  return true;
}

// ============================================================================
// [Fog::RasterUtil - Mask]
// ============================================================================

// Get usable span instance from span retrieved from mask. We encode 'owned'
// bit into pointer itself so it's needed to clear it to access the instance.
#define RASTER_CLIP_SPAN_GET_USABLE(__span__) \
  ( (RasterSpan8*) ((size_t)(__span__) & ~1) )

// Get whether a given span instance is 'owned' or not. Owned means that it
// can be directly manipulated by clipping method. If span is not owned then
// you must replace the entire clip row with new span list.
#define RASTER_CLIP_SPAN_IS_OWNED(__span__) \
  (((size_t)(__span__) & 0x1) == 0)

// Whether a given span instance is VSpan that contains own embedded clip mask.
#define RASTER_CLIP_IS_EMBEDDED_VSPAN(__span__) \
  (reinterpret_cast<const uint8_t*>(__span__) + sizeof(RasterMaskSpan8) == __span__->getGenericMask())

#define RASTER_ENTER_CLIP_FUNC() \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY(clipOp >= CLIP_OP_COUNT)) return ERR_RT_INVALID_ARGUMENT; \
    \
    if (FOG_UNLIKELY(ctx.state & RASTER_CONTEXT_NO_PAINT_WORK_REGION)) \
      return ERR_OK; \
  FOG_MACRO_END

#define RASTER_ENTER_CLIP_COND(__condition__) \
  FOG_MACRO_BEGIN \
    if (FOG_UNLIKELY(clipOp >= CLIP_OP_COUNT)) return ERR_RT_INVALID_ARGUMENT; \
    \
    if (FOG_UNLIKELY(ctx.state & RASTER_CONTEXT_NO_PAINT_WORK_REGION)) \
      return ERR_OK; \
    \
    if (FOG_UNLIKELY(!(__condition__))) \
      return _clipOpNull(clipOp); \
    \
  FOG_MACRO_END

//! @internal
//!
//! @brief Binary search for the first rectangle usable for a scanline @a y.
//!
//! @param base The first rectangle in region.
//! @param length Count of rectangles in @a base array.
//! @param y The Y position to match
//! 
//! If there are no rectangles which matches the Y coordinate and the last
//! @c rect.y1 is lower or equal to @a y, @c NULL is returned. If there
//! is rectangle where @c rect.y0 >= @a y then the first rectangle in the list
//! is returned.
//!
//! The @a base and @a length parameters come in the most cases from @c Region
//! instance, but can be constructed manually.
FOG_NO_EXPORT const BoxI* getClosestRect(const BoxI* base, size_t length, int y);

//! @internal
//!
//! @brief Get the end band of current horizontal rectangle list.
static FOG_INLINE const BoxI* getEndBand(const BoxI* base, const BoxI* end)
{
  int y0 = base[0].y0;
  const BoxI* cur = base;

  while (++cur != end && cur[0].y0 == y0)
    continue;

  return cur;
}

//! @}

} // RasterUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERUTIL_P_H
