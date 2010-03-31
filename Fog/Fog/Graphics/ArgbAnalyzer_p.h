// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_ARGBANALYZER_P_H
#define _FOG_GRAPHICS_ARGBANALYZER_P_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Graphics/Argb.h>

//! @addtogroup Fog_Graphics_Private
//! @{

namespace Fog {

// ============================================================================
// [Fog::ArgbAnalyzer]
// ============================================================================

struct FOG_HIDDEN ArgbAnalyzer
{
  static int analyzeAlpha(const uint8_t* data, sysuint_t count, sysuint_t alphapos, sysuint_t _inc);

  static FOG_INLINE int analyzeAlpha(
    const uint32_t* data,
    sysuint_t count,
    sysuint_t alphapos = ARGB32_ABYTE,
    sysuint_t inc = 4)
  {
    return analyzeAlpha((const uint8_t*)data, count, alphapos, inc);
  }

  static FOG_INLINE int analyzeAlpha(
    const Argb* data,
    size_t count)
  {
    return analyzeAlpha((const uint8_t*)data, count, ARGB32_ABYTE, sizeof(Argb));
  }

  static FOG_INLINE int analyzeAlpha(
    const ArgbStop* data,
    sysuint_t count)
  {
    return analyzeAlpha((const uint8_t*)data, count,
      FOG_OFFSET_OF(ArgbStop, argb) + ARGB32_ABYTE,
      sizeof(ArgbStop));
  }

  static FOG_INLINE int analyzeAlpha(const List<ArgbStop>& stops)
  {
    return analyzeAlpha(stops.getData(), stops.getLength());
  }
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_ARGBANALYZER_P_H
