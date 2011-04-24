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
#include <Fog/Svg/Dom/SvgCircleElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgCircleElement]
// ============================================================================

SvgCircleElement::SvgCircleElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_circle), SVG_ELEMENT_CIRCLE),
  a_cx(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_cx), FOG_OFFSET_OF(SvgCircleElement, a_cx)),
  a_cy(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_cy), FOG_OFFSET_OF(SvgCircleElement, a_cy)),
  a_r (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_r ), FOG_OFFSET_OF(SvgCircleElement, a_r ))
{
}

SvgCircleElement::~SvgCircleElement()
{
  _removeAttributes();
}

XmlAttribute* SvgCircleElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_cx)) return (XmlAttribute*)&a_cx;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_cy)) return (XmlAttribute*)&a_cy;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_r )) return (XmlAttribute*)&a_r;

  return base::_createAttribute(name);
}

err_t SvgCircleElement::onRenderShape(SvgRenderContext* context) const
{
  if (a_r.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordValue() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordValue() : 0.0f;
    float r = Math::abs(a_r.getCoord().value);

    if (r <= 0.0f) return ERR_OK;

    context->drawEllipse(EllipseF(PointF(cx, cy), PointF(r, r)));
    return ERR_OK;
  }
  else
  {
    return ERR_OK;
  }
}

err_t SvgCircleElement::onCalcBoundingBox(RectF* box) const
{
  if (a_r.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordValue() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordValue() : 0.0f;
    float r = Math::abs(a_r.getCoord().value);

    if (r <= 0.0f) goto _Fail;

    box->set(cx - r, cy - r, r * 2.0f, r * 2.0f);
    return ERR_OK;
  }

_Fail:
  box->reset();
  return ERR_OK;
}

} // Fog namespace
