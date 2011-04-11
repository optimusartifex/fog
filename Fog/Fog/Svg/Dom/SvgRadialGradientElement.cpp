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
#include <Fog/Svg/Dom/SvgRadialGradientElement_p.h>
#include <Fog/Svg/Render/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgRadialGradientElement]
// ============================================================================

SvgRadialGradientElement::SvgRadialGradientElement() :
  SvgAbstractGradientElement(fog_strings->getString(STR_SVG_ELEMENT_radialGradient), SVG_ELEMENT_RADIAL_GRADIENT),
  a_cx(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_cx), FOG_OFFSET_OF(SvgRadialGradientElement, a_cx)),
  a_cy(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_cy), FOG_OFFSET_OF(SvgRadialGradientElement, a_cy)),
  a_fx(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_fx), FOG_OFFSET_OF(SvgRadialGradientElement, a_fx)),
  a_fy(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_fy), FOG_OFFSET_OF(SvgRadialGradientElement, a_fx)),
  a_r (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_r ), FOG_OFFSET_OF(SvgRadialGradientElement, a_r ))
{
}

SvgRadialGradientElement::~SvgRadialGradientElement()
{
  _removeAttributes();
}

XmlAttribute* SvgRadialGradientElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_cx)) return (XmlAttribute*)&a_cx;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_cy)) return (XmlAttribute*)&a_cy;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_fx)) return (XmlAttribute*)&a_fx;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_fy)) return (XmlAttribute*)&a_fy;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_r )) return (XmlAttribute*)&a_r;

  return base::_createAttribute(name);
}

err_t SvgRadialGradientElement::onRender(SvgRenderContext* context) const
{
  return ERR_OK;
}

err_t SvgRadialGradientElement::onApplyPattern(SvgRenderContext* context, SvgElement* obj, int paintType) const
{
  // SVG TODO: Radial Gradient.

  RadialGradientF gradient;

  // Set spread method.
  if (a_spreadMethod.isAssigned()) gradient.setGradientSpread(a_spreadMethod.getEnumValue());

  // Setup start and end points.
  if (!a_gradientUnits.isAssigned() || a_gradientUnits.getEnumValue() == SVG_OBJECT_BOUNDING_BOX)
  {
    RectF bbox = obj->getBoundingRect();

    float cx = a_cx.isAssigned() ? context->translateCoord(a_cx.getCoord()) : 0.5f;
    float cy = a_cy.isAssigned() ? context->translateCoord(a_cy.getCoord()) : 0.5f;
    float fx = a_fx.isAssigned() ? context->translateCoord(a_fx.getCoord()) : cx;
    float fy = a_fy.isAssigned() ? context->translateCoord(a_fy.getCoord()) : cy;
    float r  = a_r.isAssigned() ? context->translateCoord(a_r.getCoord()) : 0.5f;

    cx = bbox.x + bbox.w * cx;
    cy = bbox.y + bbox.h * cy;
    fx = bbox.x + bbox.w * fx;
    fy = bbox.y + bbox.h * fy;
    r = Math::min(bbox.w, bbox.h) * r;

    gradient.setCenter(PointF(cx, cy));
    gradient.setFocal(PointF(fx, fy));
    gradient.setRadius(r);
  }
  else if (a_cx.isAssigned() && a_cy.isAssigned() && a_cx.isAssigned() && a_r.isAssigned())
  {
    float cx = context->translateCoord(a_cx.getCoord());
    float cy = context->translateCoord(a_cy.getCoord());
    float fx = a_fx.isAssigned() ? context->translateCoord(a_fx.getCoord()) : cx;
    float fy = a_fy.isAssigned() ? context->translateCoord(a_fy.getCoord()) : cy;
    float r  = context->translateCoord(a_r.getCoord());

    gradient.setCenter(PointF(cx, cy));
    gradient.setFocal(PointF(fx, fy));
    gradient.setRadius(r);
  }
  else
  {
    // SVG TODO: Is this error?
    Debug::dbgFunc("Fog::SvgRadialGradient", "onApplyPattern", "Unsupported combination.\n");
  }

  // Add color stops.
  _walkAndAddColorStops(const_cast<SvgRadialGradientElement*>(this), gradient);

  // Create PatternF instance and set transform.
  PatternF pattern(gradient);
  if (a_gradientTransform.isAssigned()) pattern.setTransform(a_gradientTransform.getTransform());

  if (paintType == SVG_PAINT_FILL)
    context->setFillPattern(pattern);
  else
    context->setStrokePattern(pattern);

  return ERR_OK;
}

} // Fog namespace
