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
#include <Fog/Svg/Dom/SvgCircleElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgCircleElement]
// ============================================================================

SvgCircleElement::SvgCircleElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_circle), SVG_ELEMENT_CIRCLE),
  a_cx(NULL, FOG_STR_(SVG_ATTRIBUTE_cx), FOG_OFFSET_OF(SvgCircleElement, a_cx)),
  a_cy(NULL, FOG_STR_(SVG_ATTRIBUTE_cy), FOG_OFFSET_OF(SvgCircleElement, a_cy)),
  a_r (NULL, FOG_STR_(SVG_ATTRIBUTE_r ), FOG_OFFSET_OF(SvgCircleElement, a_r ))
{
}

SvgCircleElement::~SvgCircleElement()
{
  _removeAttributes();
}

XmlAttribute* SvgCircleElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_cx)) return (XmlAttribute*)&a_cx;
  if (name == FOG_STR_(SVG_ATTRIBUTE_cy)) return (XmlAttribute*)&a_cy;
  if (name == FOG_STR_(SVG_ATTRIBUTE_r )) return (XmlAttribute*)&a_r;

  return base::_createAttribute(name);
}

err_t SvgCircleElement::onProcess(SvgVisitor* visitor) const
{
  if (a_r.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;
    float r = Math::abs(a_r.getCoordComputed());

    if (r <= 0.0f) return ERR_OK;

    ShapeF shape(CircleF(PointF(cx, cy), r));
    return visitor->onShape((SvgElement*)this, shape);
  }

  return ERR_OK;
}

err_t SvgCircleElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_r.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;
    float r = Math::abs(a_r.getCoordComputed());

    if (r <= 0.0f) goto _Fail;
    return CircleF(PointF(cx, cy), r)._getBoundingBox(box, tr);
  }

_Fail:
  box.reset();
  return ERR_OK;
}

} // Fog namespace
