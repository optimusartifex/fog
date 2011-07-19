// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_GLYPHMETRICS_H
#define _FOG_G2D_TEXT_GLYPHMETRICS_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::GlyphMetricsF]
// ============================================================================

struct FOG_NO_EXPORT GlyphMetricsF
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getHorizontalAdvance() const { return _horizontalAdvance; }
  FOG_INLINE const PointF& getVerticalAdvance() const { return _verticalAdvance; }

  FOG_INLINE void setHorizontalAdvance(const PointF& pt) { _horizontalAdvance = pt; }
  FOG_INLINE void setVerticalAdvance(const PointF& pt) { _verticalAdvance = pt; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _horizontalAdvance.reset();
    _verticalAdvance.reset();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF _horizontalAdvance;
  PointF _verticalAdvance;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::GlyphMetricsF, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_TEXT_GLYPHMETRICS_H
