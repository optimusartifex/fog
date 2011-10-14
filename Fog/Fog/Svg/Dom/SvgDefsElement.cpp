// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Svg/Dom/SvgDefsElement_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgDefsElement]
// ============================================================================

SvgDefsElement::SvgDefsElement() :
  SvgElement(FOG_STR_(SVG_ELEMENT_defs), SVG_ELEMENT_DEFS)
{
}

SvgDefsElement::~SvgDefsElement()
{
  _removeAttributes();
}

err_t SvgDefsElement::onProcess(SvgVisitor* visitor) const
{
  // <defs> section is used only to define shared resources. Don't go inside.
  return ERR_OK;
}

} // Fog namespace
