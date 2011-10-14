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
#include <Fog/Svg/Dom/SvgSolidColorElement_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgSolidColorElement]
// ============================================================================

SvgSolidColorElement::SvgSolidColorElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_solidColor), SVG_ELEMENT_SOLID_COLOR)
{
}

SvgSolidColorElement::~SvgSolidColorElement()
{
  _removeAttributes();
}

XmlAttribute* SvgSolidColorElement::_createAttribute(const ManagedStringW& name) const
{
  return base::_createAttribute(name);
}

err_t SvgSolidColorElement::onProcess(SvgVisitor* visitor) const
{
  return ERR_OK;
}

} // Fog namespace
