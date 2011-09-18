// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Svg/Dom/SvgEnumItem_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgEnumItem - Methods]
// ============================================================================

int svgGetEnumId(const StringW& value, const SvgEnumItem* items)
{
  while (items->name[0] != '\0')
  {
    if (value == Ascii8(items->name)) break;
    items++;
  }

  return items->value;
}

} // Fog namespace
