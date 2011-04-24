// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/Svg/Dom/SvgEnumItem_p.h>
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

// ============================================================================
// [Fog::SvgEnumItem - Methods]
// ============================================================================

int svgGetEnumId(const String& value, const SvgEnumItem* items)
{
  while (items->name[0] != '\0')
  {
    if (value == Ascii8(items->name)) break;
    items++;
  }

  return items->value;
}

} // Fog namespace
