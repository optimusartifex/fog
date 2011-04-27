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
#include <Fog/Svg/Visit/SvgRender.h>

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

err_t SvgEllipseElement::onRenderShape(SvgRenderContext* context) const
{
  if (a_rx.isAssigned() && a_ry.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;
    float rx = Math::abs(a_rx.getCoordComputed());
    float ry = Math::abs(a_ry.getCoordComputed());

    if (rx <= 0.0f || ry <= 0.0f) return ERR_OK;

    context->drawEllipse(EllipseF(PointF(cx, cy), PointF(rx, ry)));
    return ERR_OK;
  }
  else
  {
    return ERR_OK;
  }
}

err_t SvgEllipseElement::onCalcBoundingBox(RectF* box) const
{
  if (a_rx.isAssigned() && a_ry.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;

    float rx = a_rx.getCoordComputed();
    float ry = a_ry.getCoordComputed();

    if (rx <= 0.0f || ry <= 0.0f) goto _Fail;

    box->setRect(cx - rx, cy - ry, rx * 2.0f, ry * 2.0f);
    return ERR_OK;
  }

_Fail:
  box->reset();
  return ERR_OK;
}

} // Fog namespace
