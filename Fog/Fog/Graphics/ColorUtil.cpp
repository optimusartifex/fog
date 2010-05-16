// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/ColorUtil.h>

namespace Fog {
namespace ArgbUtil {

// ============================================================================
// [Fog::ArgbUtil - RGB <-> HSV]
// ============================================================================

uint32_t argbFromAhsv(float a, float h, float s, float v)
{
  int ai, hi, si, vi;
  float f;

  a *= 255.0f;
  v *= 255.0f;

  ai = (int)a;
  vi = (int)v;

  if (s == 0.0f)
  {
    return Argb(ai, vi, vi, vi);
  }

  if (h >= 360.0f || h < 0.0f)
  {
    h = fmodf(h, 360.0f);
    if (h < 0.0f) h += 360.0f;
  }

  h /= 60.0f;
  hi = (int)h;
  f = h - (float)hi;

  s *= v;
  f *= s;
  s = v - s;

  si = (int)s;

  switch (hi)
  {
    default:
    case 0: return Argb(ai, vi, (int)(s + f), si);
    case 1: return Argb(ai, (int)(v - f), vi, si);
    case 2: return Argb(ai, si, vi, (int)(s + f));
    case 3: return Argb(ai, si, (int)(v - f), vi);
    case 4: return Argb(ai, (int)(s + f), si, vi);
    case 5: return Argb(ai, vi, si, (int)(v - f));
  }
}

} // ArgbUtil namespace
} // Fog namespace
