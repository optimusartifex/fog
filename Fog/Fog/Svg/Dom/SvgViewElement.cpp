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
#include <Fog/Svg/Dom/SvgViewElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgRootElement]
// ============================================================================

SvgViewElement::SvgViewElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_svg), SVG_ELEMENT_VIEW),
  a_viewBox(NULL, FOG_STR_(SVG_ATTRIBUTE_viewBox), FOG_OFFSET_OF(SvgViewElement, a_viewBox))
{
}

SvgViewElement::~SvgViewElement()
{
  _removeAttributes();
}

XmlAttribute* SvgViewElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_viewBox)) return (XmlAttribute*)&a_viewBox;

  return base::_createAttribute(name);
}

err_t SvgViewElement::onProcess(SvgVisitor* visitor) const
{
  if (!hasChildNodes()) return ERR_OK;
  return _visitContainer(visitor);
}

err_t SvgViewElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

} // Fog namespace
