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
#include <Fog/Svg/Dom/SvgAElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgAElement]
// ============================================================================

SvgAElement::SvgAElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_a), SVG_ELEMENT_A)
{
}

SvgAElement::~SvgAElement()
{
  _removeAttributes();
}

XmlAttribute* SvgAElement::_createAttribute(const ManagedString& name) const
{
  return base::_createAttribute(name);
}

err_t SvgAElement::onProcess(SvgVisitor* visitor) const
{
  return _visitContainer(visitor);
}

} // Fog namespace
