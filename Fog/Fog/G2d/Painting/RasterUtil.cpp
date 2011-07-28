// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Painting/RasterUtil_p.h>

namespace Fog {
namespace RasterUtil {

const BoxI* getClosestRect(const BoxI* base, size_t length, int y)
{
  FOG_ASSERT(base != NULL);
  FOG_ASSERT(length > 0);

  size_t i;

  if (length <= 16)
  {
    for (size_t i = 0; i < length; i++)
    {
      if (base[i].y0 >= y)
        return base + i;
    }
    return NULL;
  }
  else
  {
    const BoxI* rect;

    if (base[0].y0 >= y && base[0].y1 < y)
      return base;
    if (base[length - 1].y1 <= y)
      return NULL;

    for (i = length; i != 0; i >>= 1)
    {
      rect = base + (i >> 1);

      // Try match.
      if (y >= rect->y0)
      {
        if (y < rect->y1) break;
        // else: Move left.
      }
      else if (rect->y1 <= y)
      {
        // Move right.
        base = rect + 1;
        i--;
      }
      // else: Move left.
    }

    while (rect[-1].y1 == rect[0].y1) rect--;
    return rect;
  }
}

} // RasterUtil namespace
} // Fog namespace
