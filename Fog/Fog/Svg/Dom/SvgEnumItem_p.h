// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGENUMITEM_P_H
#define _FOG_SVG_DOM_SVGENUMITEM_P_H

// [Dependencies]
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct String;

// ============================================================================
// [Fog::SvgEnumItem]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
struct FOG_NO_EXPORT SvgEnumItem
{
  char name[20];
  int value;
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::SvgEnumItem - Methods]
// ============================================================================

FOG_NO_EXPORT int svgGetEnumId(const String& value, const SvgEnumItem* items);

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGENUMITEM_P_H
