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
#include <Fog/Svg/Dom/SvgSolidColorElement_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgSolidColorElement]
// ============================================================================

SvgSolidColorElement::SvgSolidColorElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_solidColor), SVG_ELEMENT_SOLID_COLOR)
{
}

SvgSolidColorElement::~SvgSolidColorElement()
{
  _removeAttributes();
}

XmlAttribute* SvgSolidColorElement::_createAttribute(const ManagedString& name) const
{
  return base::_createAttribute(name);
}

err_t SvgSolidColorElement::onProcess(SvgVisitor* visitor) const
{
  return ERR_OK;
}

} // Fog namespace
