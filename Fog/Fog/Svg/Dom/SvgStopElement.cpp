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
#include <Fog/Svg/Dom/SvgStopElement_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgStopElement]
// ============================================================================

SvgStopElement::SvgStopElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_stop), SVG_ELEMENT_STOP),
  a_offset(NULL, FOG_STR_(SVG_ATTRIBUTE_offset), FOG_OFFSET_OF(SvgStopElement, a_offset))
{
}

SvgStopElement::~SvgStopElement()
{
  _removeAttributes();
}

XmlAttribute* SvgStopElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_offset)) return (XmlAttribute*)&a_offset;

  return base::_createAttribute(name);
}

err_t SvgStopElement::onProcess(SvgVisitor* visitor) const
{
  return ERR_OK;
}

} // Fog namespace
