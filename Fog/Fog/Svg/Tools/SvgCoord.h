// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_TOOLS_SVGCOORD_H
#define _FOG_SVG_TOOLS_SVGCOORD_H

// [Dependencies]
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

//! @addtogroup Fog_Svg_Tools
//! @{

// ============================================================================
// [Fog::SvgCoord]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct FOG_NO_EXPORT SvgCoord
{
  FOG_INLINE SvgCoord() {}
  FOG_INLINE SvgCoord(float value, uint32_t unit) : value(value), unit(unit) {}

  float value;
  uint32_t unit;
};
#include <Fog/Core/C++/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPE_DECLARE(Fog::SvgCoord, Fog::TYPE_CATEGORY_SIMPLE)

// [Guard]
#endif // _FOG_SVG_TOOLS_SVGCOORD_H
