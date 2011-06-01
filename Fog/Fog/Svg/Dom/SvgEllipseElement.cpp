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
#include <Fog/Svg/Dom/SvgEllipseElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgEllipseElement]
// ============================================================================

SvgEllipseElement::SvgEllipseElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_ellipse), SVG_ELEMENT_ELLIPSE),
  a_cx(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_cx), FOG_OFFSET_OF(SvgEllipseElement, a_cx)),
  a_cy(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_cy), FOG_OFFSET_OF(SvgEllipseElement, a_cy)),
  a_rx(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_rx), FOG_OFFSET_OF(SvgEllipseElement, a_rx)),
  a_ry(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_ry), FOG_OFFSET_OF(SvgEllipseElement, a_ry))
{
}

SvgEllipseElement::~SvgEllipseElement()
{
  _removeAttributes();
}

XmlAttribute* SvgEllipseElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_cx)) return (XmlAttribute*)&a_cx;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_cy)) return (XmlAttribute*)&a_cy;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_rx)) return (XmlAttribute*)&a_rx;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_ry)) return (XmlAttribute*)&a_ry;

  return base::_createAttribute(name);
}

err_t SvgEllipseElement::onProcess(SvgVisitor* visitor) const
{
  if (a_rx.isAssigned() && a_ry.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;

    float rx = Math::abs(a_rx.getCoordComputed());
    float ry = Math::abs(a_ry.getCoordComputed());

    if (rx <= 0.0f || ry <= 0.0f) return ERR_OK;

    ShapeF shape(EllipseF(PointF(cx, cy), PointF(rx, ry)));
    return visitor->onShape((SvgElement*)this, shape);
  }

  return ERR_OK;
}

err_t SvgEllipseElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_rx.isAssigned() && a_ry.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;

    float rx = a_rx.getCoordComputed();
    float ry = a_ry.getCoordComputed();

    if (rx <= 0.0f || ry <= 0.0f) goto _Fail;
    return EllipseF(PointF(cx, cy), PointF(rx, ry))._getBoundingBox(box, tr);
  }

_Fail:
  box.reset();
  return ERR_GEOMETRY_INVALID;
}

} // Fog namespace
