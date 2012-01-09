// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERUTIL_P_H
#define _FOG_G2D_PAINTING_RASTERUTIL_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>

// ============================================================================
// [Forward Declarations]
// ============================================================================

namespace Fog {
struct RasterPattern;
} // Fog namespace

namespace Fog {
namespace RasterUtil {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterUtil - Composite - CoreOp / ExtOp]
// ============================================================================

static FOG_INLINE bool isCompositeCoreOp(uint32_t compositingOperator)
{
  return compositingOperator < RASTER_COMPOSITE_CORE_COUNT;
}

static FOG_INLINE bool isCompositeExtOp(uint32_t compositingOperator)
{
  return compositingOperator >= RASTER_COMPOSITE_CORE_COUNT;
}

// ============================================================================
// [Fog::RasterUtil - Composite - CopyOp / NopOp]
// ============================================================================

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

static FOG_INLINE bool isCompositeNopOp(uint32_t op)
{
  return op == COMPOSITE_DST;
}

// ============================================================================
// [Fog::RasterUtil - Composite - Compat]
// ============================================================================

static FOG_INLINE uint32_t getCompositeCompatFormat(uint32_t dstFormat, uint32_t srcFormat)
{
  FOG_ASSERT(dstFormat <= IMAGE_FORMAT_COUNT);
  FOG_ASSERT(srcFormat <= IMAGE_FORMAT_COUNT);

  return _g2d_render_compatibleFormat[dstFormat][srcFormat].srcFormat;
}

static FOG_INLINE uint32_t getCompositeCompatVBlitId(uint32_t dstFormat, uint32_t srcFormat)
{
  FOG_ASSERT(dstFormat <= IMAGE_FORMAT_COUNT);
  FOG_ASSERT(srcFormat <= IMAGE_FORMAT_COUNT);

  return _g2d_render_compatibleFormat[dstFormat][srcFormat].vblitId;
}

// ============================================================================
// [Fog::RasterUtil - Debug]
// ============================================================================

// TODO: RLE - Not needed.
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
// [Fog::RasterUtil - Scope]
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

static FOG_INLINE bool isSolidContext(RasterPattern* pc)
{
  return (size_t)pc == (size_t)0x1;
}

static FOG_INLINE bool isPatternContext(RasterPattern* pc)
{
  return (size_t)pc > (size_t)0x1;
}

// ============================================================================
// [Fog::RasterUtil - Geometry]
// ============================================================================

static FOG_INLINE bool isBox24x8Aligned(const BoxI& box)
{
  return ((box.x0 | box.y0 | box.x1 | box.y1) & 0xFF) == 0;
}

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

//! @}

} // RasterUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERUTIL_P_H
