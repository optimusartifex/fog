// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FONTMETRICS_H
#define _FOG_G2D_TEXT_FONTMETRICS_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/HashUtil.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FontMetricsF]
// ============================================================================

//! @brief Font metrics (float).
struct FOG_NO_EXPORT FontMetricsF
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getHeight() const { return _height; }
  FOG_INLINE float getAscent() const { return _ascent; }
  FOG_INLINE float getDescent() const { return _descent; }
  FOG_INLINE float getInternalLeading() const { return _internalLeading; }
  FOG_INLINE float getExternalLeading() const { return _externalLeading; }
  FOG_INLINE float getAverageWidth() const { return _averageWidth; }
  FOG_INLINE float getMaximumWidth() const { return _maximumWidth; }

  FOG_INLINE void setHeight(float height) { _height = height; }
  FOG_INLINE void setAscent(float ascent) { _ascent = ascent; }
  FOG_INLINE void setDescent(float descent) { _descent = descent; }
  FOG_INLINE void setInternalLeading(float internalLeading) { _internalLeading = internalLeading; }
  FOG_INLINE void setExternalLeading(float externalLeading) { _externalLeading = externalLeading; }
  FOG_INLINE void setAverageWidth(float averageWidth) { _averageWidth = averageWidth; }
  FOG_INLINE void setMaximumWidth(float maximumWidth) { _maximumWidth = maximumWidth; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<FontMetricsF>(this);
  }

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return HashUtil::hashVectorD(this, 7);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Font height.
  float _height;
  //! @brief Font ascent.
  float _ascent;
  //! @brief Font descent.
  float _descent;
  //! @brief Internal leading.
  float _internalLeading;
  //! @brief External leading.
  float _externalLeading;

  //! @brief Average width.
  float _averageWidth;
  //! @brief Maximum width of largest character.
  float _maximumWidth;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FONTMETRICS_H
