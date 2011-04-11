// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/Strings.h>
#include <Fog/Svg/Dom/SvgDefsElement_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgDefsElement]
// ============================================================================

SvgDefsElement::SvgDefsElement() : 
  SvgElement(fog_strings->getString(STR_SVG_ELEMENT_defs), SVG_ELEMENT_DEFS)
{
}

SvgDefsElement::~SvgDefsElement()
{
  _removeAttributes();
}

err_t SvgDefsElement::onRender(SvgRenderContext* context) const
{
  // <defs> section is used only to define shared resources or gradients.
  // Don't go inside.
  return ERR_OK;
}

} // Fog namespace
