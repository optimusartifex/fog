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
#include <Fog/Svg/Dom/SvgRadialGradientElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

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

err_t SvgRadialGradientElement::onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const
{
  // SVG TODO: Radial Gradient.

  RadialGradientF gradient;

  // Set spread method.
  if (a_spreadMethod.isAssigned()) gradient.setGradientSpread(a_spreadMethod.getEnumValue());

  // Setup start and end points.
  if (!a_gradientUnits.isAssigned() || a_gradientUnits.getEnumValue() == SVG_OBJECT_BOUNDING_BOX)
  {
    BoxF bbox;
    obj->getBoundingBox(bbox);

    float bw = bbox.getWidth();
    float bh = bbox.getHeight();

    float cx = a_cx.isAssigned() ? a_cx.getCoordValue() : 0.5f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordValue() : 0.5f;
    float fx = a_fx.isAssigned() ? a_fx.getCoordValue() : cx;
    float fy = a_fy.isAssigned() ? a_fy.getCoordValue() : cy;
    float rx = a_r.isAssigned() ? a_r.getCoordValue() : 0.5f;
    float ry = rx;

    cx = bbox.x0 + bw * cx;
    cy = bbox.y0 + bh * cy;
    fx = bbox.x0 + bw * fx;
    fy = bbox.y0 + bh * fy;
    rx = bw * rx;
    ry = bh * ry;

    gradient.setCenter(PointF(cx, cy));
    gradient.setFocal(PointF(fx, fy));
    gradient.setRadius(PointF(rx, ry));
  }
  else
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.5f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.5f;
    float fx = a_fx.isAssigned() ? a_fx.getCoordComputed() : cx;
    float fy = a_fy.isAssigned() ? a_fy.getCoordComputed() : cy;
    float rx = a_r.isAssigned() ? a_r.getCoordComputed() : 0.5f;
    float ry = rx;

    // TODO: Percentages to the current view-port.

    gradient.setCenter(PointF(cx, cy));
    gradient.setFocal(PointF(fx, fy));
    gradient.setRadius(PointF(rx, ry));
  }

  // Add color stops.
  _walkAndAddColorStops(const_cast<SvgRadialGradientElement*>(this), gradient);

  // Create Pattern instance.
  Pattern pattern(gradient, a_gradientTransform.isAssigned()
    ? a_gradientTransform.getTransform()
    : TransformF::identity()
  );

  if (paintType == SVG_PAINT_FILL)
    visitor->setFillPattern(pattern);
  else
    visitor->setStrokePattern(pattern);

  return ERR_OK;
}

} // Fog namespace
