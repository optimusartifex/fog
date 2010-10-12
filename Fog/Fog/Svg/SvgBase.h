// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_SVGBASE_H
#define _FOG_SVG_SVGBASE_H

// [Dependencies]
#include <Fog/Core/Char.h>
#include <Fog/Core/List.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Path.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgCoord]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
struct FOG_HIDDEN SvgCoord
{
  FOG_INLINE SvgCoord() {}
  FOG_INLINE SvgCoord(float value, uint32_t unit) : value(value), unit(unit) {}

  float value;
  uint32_t unit;
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

FOG_DECLARE_TYPEINFO(Fog::SvgCoord, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_SVG_SVGBASE_H
