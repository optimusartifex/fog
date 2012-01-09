// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Tools/ColorAnalyzer_p.h>

namespace Fog {

// ============================================================================
// [Fog::ColorAnalyzer]
// ============================================================================

static const uint32_t ColorAnalyzer_result[] =
{
  /* 0x00 -> */ ALPHA_DISTRIBUTION_ZERO,
  /* 0x01 -> */ ALPHA_DISTRIBUTION_ZERO,
  /* 0x02 -> */ ALPHA_DISTRIBUTION_FULL,
  /* 0x03 -> */ ALPHA_DISTRIBUTION_ZERO_OR_FULL
};

uint32_t ColorAnalyzer::analyzeAlpha32(const uint8_t* data, ssize_t stride, int w, int h, int aPos, int inc)
{
  uint32_t mask = 0x00;

  if (w <= 0 || h <= 0) return ALPHA_DISTRIBUTION_ZERO;

  data += aPos;
  stride += w * inc;

  for (int y = 0; y < h; y++, data += stride)
  {
    for (int x = 0; x < w; x++, data += inc)
    {
      uint8_t a = data[0];

      if (a == 0x00)
        mask |= 0x01;
      else if (a == 0xFF)
        mask |= 0x02;
      else
        return ALPHA_DISTRIBUTION_VARIANT;
    }
  }

  FOG_ASSERT(mask != 0x00);
  return ColorAnalyzer_result[mask];
}

uint32_t ColorAnalyzer::analyzeAlpha64(const uint8_t* data, ssize_t stride, int w, int h, int aPos, int inc)
{
  uint32_t mask = 0x00;

  if (w <= 0 || h <= 0) return ALPHA_DISTRIBUTION_ZERO;

  data += aPos;
  stride += w * inc;

  for (int y = 0; y < h; y++, data += stride)
  {
    for (int x = 0; x < w; x++, data += inc)
    {
      uint16_t a = ((const uint16_t*)data)[0];

      if (a == 0x00)
        mask |= 0x01;
      else if (a == 0xFFFF)
        mask |= 0x02;
      else
        return ALPHA_DISTRIBUTION_VARIANT;
    }
  }

  FOG_ASSERT(mask != 0x00);
  return ColorAnalyzer_result[mask];
}

} // Fog namespace
