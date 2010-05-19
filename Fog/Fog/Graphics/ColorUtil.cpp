// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Math.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/ColorUtil.h>

namespace Fog {
namespace ColorUtil {

// ============================================================================
// [Fog::ColorUtil - ARGB <-> AHSV]
// ============================================================================

Argb argbFromAhsv(const Ahsv& ahsv)
{
  int ai, hi, si, vi;
  float f;

  float a = ahsv.getAlpha();
  float h = ahsv.getHue();
  float s = ahsv.getSaturation();
  float v = ahsv.getValue();

  a *= 255.0f;
  v *= 255.0f;

  ai = (int)a;
  vi = (int)v;

  if (Math::feq<float>(s, 0.0f))
  {
    return Argb(ai, vi, vi, vi);
  }

  if (FOG_UNLIKELY(h >= 360.0f || h < 0.0f))
  {
    h = fmodf(h, 360.0f);
    if (h < 0.0f) h += 360.0f;
  }

  h *= (1.0f / 60.0f);
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

Ahsv ahsvFromArgb(Argb argb)
{
  int r = (int)argb.getRed();
  int g = (int)argb.getGreen();
  int b = (int)argb.getBlue();

  int minimum;
  int maximum;
  int delta;

  minimum = (r + g - Math::abs(r - g)) >> 1;
  minimum = (minimum + b - abs(minimum - b)) >> 1;

  maximum = (r + g + Math::abs(r - g)) >> 1;
  maximum = (maximum + b + abs(maximum - b)) >> 1;

  delta = maximum - minimum;

  float a = (1.0f / 255.0f) * (float)(int)argb.getAlpha();
  float h = 0.0f;
  float s = 0.0f;
  float v = (100.0f / 255.0f) * maximum;

  float recip = (delta) ? 100.0f / (6.0f * delta) : 0.0f;

  if (maximum != 0)
  {
    s = (float)(100 * delta) / (float)maximum;
  }

  if (maximum == r)
    h = (float)(g - b);
  else if (maximum == g)
    h = 2.0f * delta + (float)(b - r);
  else
    h = 4.0f * delta + (float)(r - g);

  h *= recip;

  if (h < 0.0f)
    h += 100.0f;
  else if (h > 100.0f)
    h -= 100.0f;

  return Ahsv(a, h, s, v);
}

} // ColorUtil namespace
} // Fog namespace
