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
#include <Fog/Svg/Dom/SvgViewElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgRootElement]
// ============================================================================

SvgViewElement::SvgViewElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_svg), SVG_ELEMENT_VIEW),
  a_viewBox(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_viewBox), FOG_OFFSET_OF(SvgViewElement, a_viewBox))
{
}

SvgViewElement::~SvgViewElement()
{
  _removeAttributes();
}

XmlAttribute* SvgViewElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_viewBox)) return (XmlAttribute*)&a_viewBox;

  return base::_createAttribute(name);
}

err_t SvgViewElement::onRenderShape(SvgRenderContext* context) const
{
  return _walkAndRender(this, context);
}

err_t SvgViewElement::onCalcBoundingBox(RectF* box) const
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

} // Fog namespace
