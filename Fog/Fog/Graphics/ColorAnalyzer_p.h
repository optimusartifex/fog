// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_COLORANALYZER_P_H
#define _FOG_GRAPHICS_COLORANALYZER_P_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Graphics/Color.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::ColorAnalyzer]
// ============================================================================

//! @internal
//!
//! @brief Color analyzer
//!
//! This static class is currently used to analyze alpha values (to determine
//! if they are fully-opaque, fully-transparent or variant.
struct FOG_HIDDEN ColorAnalyzer
{
  static uint32_t analyzeAlpha(const uint8_t* data, sysuint_t count, sysuint_t alphapos, sysuint_t _inc);

  static FOG_INLINE uint32_t analyzeAlpha(
    const uint32_t* data,
    sysuint_t count,
    sysuint_t alphapos = ARGB32_ABYTE,
    sysuint_t inc = 4)
  {
    return analyzeAlpha((const uint8_t*)data, count, alphapos, inc);
  }

  static FOG_INLINE uint32_t analyzeAlpha(
    const ArgbI* data,
    size_t count)
  {
    return analyzeAlpha((const uint8_t*)data, count, ARGB32_ABYTE, sizeof(ArgbI));
  }

  static FOG_INLINE uint32_t analyzeAlpha(
    const ArgbStop* data,
    sysuint_t count)
  {
    return analyzeAlpha((const uint8_t*)data, count,
      FOG_OFFSET_OF(ArgbStop, _argb) + ARGB32_ABYTE,
      sizeof(ArgbStop));
  }

  static FOG_INLINE uint32_t analyzeAlpha(const List<ArgbStop>& stops)
  {
    return analyzeAlpha(stops.getData(), stops.getLength());
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_COLORANALYZER_P_H
