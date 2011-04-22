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
#include <Fog/Svg/Dom/SvgLineElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgLineElement]
// ============================================================================

SvgLineElement::SvgLineElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_line), SVG_ELEMENT_LINE),
  a_x1(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x1), FOG_OFFSET_OF(SvgLineElement, a_x1)),
  a_y1(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y1), FOG_OFFSET_OF(SvgLineElement, a_y1)),
  a_x2(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x2), FOG_OFFSET_OF(SvgLineElement, a_x2)),
  a_y2(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y2), FOG_OFFSET_OF(SvgLineElement, a_y2))
{
}

SvgLineElement::~SvgLineElement()
{
  _removeAttributes();
}

XmlAttribute* SvgLineElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x1)) return (XmlAttribute*)&a_x1;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y1)) return (XmlAttribute*)&a_y1;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x2)) return (XmlAttribute*)&a_x2;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y2)) return (XmlAttribute*)&a_y2;

  return base::_createAttribute(name);
}

err_t SvgLineElement::onRenderShape(SvgRenderContext* context) const
{
  float x1 = a_x1.isAssigned() ? a_x1.getCoordValue() : 0.0f;
  float y1 = a_y1.isAssigned() ? a_y1.getCoordValue() : 0.0f;
  float x2 = a_x2.isAssigned() ? a_x2.getCoordValue() : 0.0f;
  float y2 = a_y2.isAssigned() ? a_y2.getCoordValue() : 0.0f;

  context->drawLine(LineF(PointF(x1, y1), PointF(x2, y2)));
  return ERR_OK;
}

err_t SvgLineElement::onCalcBoundingBox(RectF* box) const
{
  float x1 = a_x1.isAssigned() ? a_x1.getCoordValue() : 0.0f;
  float y1 = a_y1.isAssigned() ? a_y1.getCoordValue() : 0.0f;
  float x2 = a_x2.isAssigned() ? a_x2.getCoordValue() : 0.0f;
  float y2 = a_y2.isAssigned() ? a_y2.getCoordValue() : 0.0f;

  float x = (x1 < x2) ? x1 : x2;
  float y = (y1 < y2) ? y1 : y2;
  float w = (x1 < x2) ? x2 - x1 : x1 - x2;
  float h = (y1 < y2) ? y2 - y1 : y1 - y2;

  box->set(x, y, w, h);
  return ERR_OK;
}

} // Fog namespace
