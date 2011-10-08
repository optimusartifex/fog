// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Tools/RegionUtil_p.h>

namespace Fog {

// ============================================================================
// [Fog::RegionUtil - GetClosestBox]
// ============================================================================

static const BoxI* FOG_CDECL RegionUtil_getClosestBox(const BoxI* data, size_t length, int y)
{
  FOG_ASSERT(data != NULL);
  FOG_ASSERT(length > 0);

  size_t i;

  if (length <= 16)
  {
    for (size_t i = 0; i < length; i++)
    {
      if (data[i].y0 >= y)
        return &data[i];
    }
    return NULL;
  }
  else
  {
    const BoxI* rect;

    if (data[0].y0 >= y && data[0].y1 < y)
      return data;
    if (data[length - 1].y1 <= y)
      return NULL;

    for (i = length; i != 0; i >>= 1)
    {
      rect = data + (i >> 1);

      // Try match.
      if (y >= rect->y0)
      {
        if (y < rect->y1) break;
        // else: Move left.
      }
      else if (rect->y1 <= y)
      {
        // Move right.
        data = rect + 1;
        i--;
      }
      // else: Move left.
    }

    while (rect[-1].y1 == rect[0].y1) rect--;
    return rect;
  }
}

// ============================================================================
// [Fog::RegionUtil - IsBoxListSorted / IsRectListSorted]
// ============================================================================

static bool FOG_CDECL RegionUtil_isBoxListSorted(const BoxI* data, size_t length)
{
  if (length <= 1)
    return true;

  int y0 = data[0].y0;
  int y1 = data[0].y1;
  int xb = data[0].x1;

  // Detect invalid box.
  if (xb >= data[0].x1 || y0 >= y1)
    return false;

  for (size_t i = 1; i < length; i++)
  {
    // Detect invalid box.
    if (data[i].x0 >= data[i].x1)
      return false;

    // Detect next band.
    if (data[i].y0 != y0 || data[i].y1 != y1)
    {
      // Detect invalid box.
      if (data[i].y0 >= data[i].y1)
        return false;

      // Detect invalid position (non-sorted).
      if (data[i].y0 < y1)
        return false;

      // Ok, prepare for a new band.
      y0 = data[i].y0;
      y1 = data[i].y1;
      xb = data[i].x1;
    }
    else
    {
      // Detect whether the current band advances the last one.
      if (data[i].x0 < xb)
        return false;
      xb = data[i].x1;
    }
  }

  return true;
}

static bool FOG_CDECL RegionUtil_isRectListSorted(const RectI* data, size_t length)
{
  if (length <= 1)
    return true;

  int y = data[0].y;
  int h = data[0].h;
  int x = data[0].x + data[0].w;

  // Detect invalid box.
  if (data[0].w <= 0 || data[0].h <= 0)
    return false;

  for (size_t i = 1; i < length; i++)
  {
    // Detect invalid box.
    if (data[i].w <= 0)
      return false;

    // Detect next band.
    if (data[i].y != y || data[i].h != data[i].h)
    {
      // Detect invalid box.
      if (data[i].h <= 0)
        return false;

      // Detect invalid position (non-sorted).
      if (data[i].y < y + h)
        return false;

      // Ok, prepare for a new band.
      y = data[i].y;
      h = data[i].h;
      x = data[i].x + data[i].w;
    }
    else
    {
      // Detect whether the current band advances the last one.
      if (data[i].x < x)
        return false;
      x = data[i].x + data[i].w;
    }
  }

  return true;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void RegionUtil_init(void)
{
  _api.regionutil_getClosestBox = RegionUtil_getClosestBox;
  _api.regionutil_isBoxListSorted = RegionUtil_isBoxListSorted;
  _api.regionutil_isRectListSorted = RegionUtil_isRectListSorted;
}

} // Fog namespace
