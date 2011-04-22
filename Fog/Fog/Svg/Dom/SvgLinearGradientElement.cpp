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
#include <Fog/Svg/Dom/SvgLinearGradientElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgLinearGradientElement]
// ============================================================================

SvgLinearGradientElement::SvgLinearGradientElement() :
  SvgAbstractGradientElement(fog_strings->getString(STR_SVG_ELEMENT_linearGradient), SVG_ELEMENT_LINEAR_GRADIENT),
  a_x1(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x1), FOG_OFFSET_OF(SvgLinearGradientElement, a_x1)),
  a_y1(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y1), FOG_OFFSET_OF(SvgLinearGradientElement, a_y1)),
  a_x2(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x2), FOG_OFFSET_OF(SvgLinearGradientElement, a_x2)),
  a_y2(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y2), FOG_OFFSET_OF(SvgLinearGradientElement, a_y2))
{
}

SvgLinearGradientElement::~SvgLinearGradientElement()
{
  _removeAttributes();
}

XmlAttribute* SvgLinearGradientElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x1)) return (XmlAttribute*)&a_x1;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y1)) return (XmlAttribute*)&a_y1;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x2)) return (XmlAttribute*)&a_x2;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y2)) return (XmlAttribute*)&a_y2;

  return base::_createAttribute(name);
}

err_t SvgLinearGradientElement::onRender(SvgRenderContext* context) const
{
  return ERR_OK;
}

err_t SvgLinearGradientElement::onApplyPattern(SvgRenderContext* context, SvgElement* obj, int paintType) const
{
  // SVG TODO: Linear Gradient.

  LinearGradientF gradient;
  TransformF tr;

  // Set spread method.
  if (a_spreadMethod.isAssigned()) gradient.setGradientSpread(a_spreadMethod.getEnumValue());

  // Setup start and end points.
  if (!a_gradientUnits.isAssigned() || a_gradientUnits.getEnumValue() == SVG_OBJECT_BOUNDING_BOX)
  {
    // BoundingBox coordinates.
    RectF bbox = obj->getBoundingRect();

    float x1 = a_x1.isAssigned() ? context->translateCoord(a_x1.getCoord()) : 0.0f;
    float y1 = a_y1.isAssigned() ? context->translateCoord(a_y1.getCoord()) : 0.0f;
    float x2 = a_x2.isAssigned() ? context->translateCoord(a_x2.getCoord()) : 1.0f;
    float y2 = a_y2.isAssigned() ? context->translateCoord(a_y2.getCoord()) : 0.0f;

    tr._type = TRANSFORM_TYPE_SCALING;
    tr._00 = bbox.w;
    tr._11 = bbox.h;
    tr._20 = bbox.x;
    tr._21 = bbox.y;

    gradient.setStart(PointF(x1, y1));
    gradient.setEnd(PointF(x2, y2));
  }
  else if (a_x1.isAssigned() && a_y1.isAssigned() && a_x2.isAssigned() && a_y2.isAssigned())
  {
    // UserSpaceOnUse coordinates.
    float x1 = context->translateCoord(a_x1.getCoord());
    float y1 = context->translateCoord(a_y1.getCoord());
    float x2 = context->translateCoord(a_x2.getCoord());
    float y2 = context->translateCoord(a_y2.getCoord());

    gradient.setStart(PointF(x1, y1));
    gradient.setEnd(PointF(x2, y2));
  }
  else
  {
    // SVG TODO: Is this error?
    Debug::dbgFunc("Fog::SvgLinearGradient", "onApplyPattern", "Unsupported combination.\n");
  }

  // Add color stops.
  _walkAndAddColorStops(const_cast<SvgLinearGradientElement*>(this), gradient);

  if (a_gradientTransform.isAssigned())
    tr.transform(a_gradientTransform.getTransform(), MATRIX_ORDER_APPEND);

  // Create PatternF instance and set transform.
  PatternF pattern(gradient);
  pattern.setTransform(tr);

  if (paintType == SVG_PAINT_FILL)
    context->setFillPattern(pattern);
  else
    context->setStrokePattern(pattern);

  return ERR_OK;
}

} // Fog namespace
