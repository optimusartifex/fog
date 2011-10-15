// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_REGIONUTIL_P_H
#define _FOG_G2D_TOOLS_REGIONUTIL_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {
namespace RegionUtil {

//! @addtogroup Fog_G2d_Tools
//! @{

// ============================================================================
// [Fog::RegionUtil]
// ============================================================================

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
static FOG_INLINE const BoxI* getClosestBox(const BoxI* data, size_t length, int y)
{
  return fog_api.regionutil_getClosestBox(data, length, y);
}

//! @internal
static FOG_INLINE bool isBoxListSorted(const BoxI* data, size_t length)
{
  return fog_api.regionutil_isBoxListSorted(data, length);
}

//! @internal
static FOG_INLINE bool isRectListSorted(const RectI* data, size_t length)
{
  return fog_api.regionutil_isRectListSorted(data, length);
}

//! @internal
//!
//! @brief Get the end band of current horizontal rectangle list.
static FOG_INLINE const BoxI* getEndBand(const BoxI* data, const BoxI* end)
{
  int y0 = data[0].y0;
  const BoxI* cur = data;

  while (++cur != end && cur[0].y0 == y0)
    continue;

  return cur;
}

//! @}

} // RegionUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TOOLS_REGIONUTIL_P_H
