// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/Constants_p.h>
#include <Fog/Graphics/ColorAnalyzer_p.h>

namespace Fog {

// ============================================================================
// [Fog::ColorAnalyzer]
// ============================================================================

uint32_t ColorAnalyzer::analyzeAlpha(const uint8_t* data, sysuint_t count, sysuint_t alphapos, sysuint_t _inc)
{
  sysuint_t i = count;
  sysuint_t inc = _inc;
  uint8_t mask;

  if (!i) return ALPHA_TRANSPARENT;

  data += alphapos;
  mask = data[0];

  while (--i)
  {
    data += inc;
    if (data[0] != mask) return ALPHA_VARIANT;
  }

  return (int)mask;
}

} // Fog namespace
