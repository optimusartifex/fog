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
#include <Fog/Svg/Dom/SvgRectElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgRectElement]
// ============================================================================

SvgRectElement::SvgRectElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_rect), SVG_ELEMENT_RECT),
  a_x     (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x     ), FOG_OFFSET_OF(SvgRectElement, a_x     )),
  a_y     (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y     ), FOG_OFFSET_OF(SvgRectElement, a_y     )),
  a_width (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_width ), FOG_OFFSET_OF(SvgRectElement, a_width )),
  a_height(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_height), FOG_OFFSET_OF(SvgRectElement, a_height)),
  a_rx    (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_rx    ), FOG_OFFSET_OF(SvgRectElement, a_rx    )),
  a_ry    (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_ry    ), FOG_OFFSET_OF(SvgRectElement, a_ry    ))
{
}

SvgRectElement::~SvgRectElement()
{
  _removeAttributes();
}

XmlAttribute* SvgRectElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x     )) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y     )) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_width )) return (XmlAttribute*)&a_width;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_height)) return (XmlAttribute*)&a_height;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_rx    )) return (XmlAttribute*)&a_rx;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_ry    )) return (XmlAttribute*)&a_ry;

  return base::_createAttribute(name);
}

err_t SvgRectElement::onProcess(SvgVisitor* visitor) const
{
  if (a_width.isAssigned() && a_height.isAssigned())
  {
    float rw = a_width.getCoordComputed();
    float rh = a_height.getCoordComputed();
    if (rw <= 0.0f || rh <= 0.0f) return ERR_GEOMETRY_INVALID;

    float rx = a_x.isAssigned() ? a_x.getCoordComputed() : 0.0f;
    float ry = a_y.isAssigned() ? a_y.getCoordComputed() : 0.0f;

    float radx = a_rx.isAssigned() ? a_rx.getCoordComputed() : 0.0f;
    float rady = a_ry.isAssigned() ? a_ry.getCoordComputed() : 0.0f;

    if (!a_rx.isAssigned() && a_ry.isAssigned()) radx = rady;
    if (!a_ry.isAssigned() && a_rx.isAssigned()) rady = radx;

    ShapeF shape;

    if (radx <= float(0.0) || rady <= float(0.0))
      shape.setRect(RectF(rx, ry, rw, rh));
    else
      shape.setRound(RoundF(rx, ry, rw, rh, radx, rady));
    return visitor->onShape((SvgElement*)this, shape);
  }

  return ERR_OK;
}

err_t SvgRectElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_width.isAssigned() && a_height.isAssigned())
  {
    float rw = a_width.getCoordComputed();
    float rh = a_height.getCoordComputed();
    if (rw < 0.0f || rh < 0.0f) goto _Fail;

    float rx = a_x.isAssigned() ? a_x.getCoordComputed() : 0.0f;
    float ry = a_y.isAssigned() ? a_y.getCoordComputed() : 0.0f;

    box.setRect(rx, ry, rw, rh);
    if (tr) tr->mapBox(box, box);

    return ERR_OK;
  }

_Fail:
  box.reset();
  return ERR_OK;
}

} // Fog namespace
