// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_COLORANALYZER_P_H
#define _FOG_G2D_TOOLS_COLORANALYZER_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_G2d_Tools
//! @{

// ============================================================================
// [Fog::ColorAnalyzer]
// ============================================================================

//! @internal
//!
//! @brief Color analyzerbpp
//!
//! This static class is currently used to analyze alpha values (to determine
//! if they are fully-opaque, fully-transparent or variant.
struct FOG_NO_EXPORT ColorAnalyzer
{
  typedef uint32_t (*AnalyzerFunc)(const uint8_t* data, ssize_t stride, int w, int h, int aPos, int inc);

  static uint32_t analyzeAlpha32(const uint8_t* data, ssize_t stride, int w, int h, int aPos, int inc);
  static uint32_t analyzeAlpha64(const uint8_t* data, ssize_t stride, int w, int h, int aPos, int inc);

  static FOG_INLINE uint32_t analyzeAlphaArgb32(const uint8_t* data, ssize_t stride, int w, int h)
  { return analyzeAlpha32(data, stride, w, h, PIXEL_ARGB32_POS_A, 4); }

  static FOG_INLINE uint32_t analyzeAlphaArgb64(const uint8_t* data, ssize_t stride, int w, int h)
  { return analyzeAlpha32(data, stride, w, h, PIXEL_ARGB64_POS_A, 8); }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TOOLS_COLORANALYZER_P_H
