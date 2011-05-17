// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/Strings.h>
#include <Fog/Svg/Dom/SvgSymbolElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgSymbolElement]
// ============================================================================

SvgSymbolElement::SvgSymbolElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_symbol), SVG_ELEMENT_SYMBOL)
{
}

SvgSymbolElement::~SvgSymbolElement()
{
  _removeAttributes();
}

err_t SvgSymbolElement::onProcess(SvgVisitor* visitor) const
{
  if (!hasChildNodes()) return ERR_OK;
  return _visitContainer(visitor);
}

} // Fog namespace
