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
#include <Fog/Svg/Dom/SvgLineElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

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

err_t SvgLineElement::onProcess(SvgVisitor* visitor) const
{
  float x1 = a_x1.isAssigned() ? a_x1.getCoordComputed() : 0.0f;
  float y1 = a_y1.isAssigned() ? a_y1.getCoordComputed() : 0.0f;
  float x2 = a_x2.isAssigned() ? a_x2.getCoordComputed() : 0.0f;
  float y2 = a_y2.isAssigned() ? a_y2.getCoordComputed() : 0.0f;

  ShapeF shape(LineF(x1, y1, x2, y2));
  return visitor->onShape((SvgElement*)this, shape);
}

err_t SvgLineElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  float x1 = a_x1.isAssigned() ? a_x1.getCoordComputed() : 0.0f;
  float y1 = a_y1.isAssigned() ? a_y1.getCoordComputed() : 0.0f;
  float x2 = a_x2.isAssigned() ? a_x2.getCoordComputed() : 0.0f;
  float y2 = a_y2.isAssigned() ? a_y2.getCoordComputed() : 0.0f;

  box.setBox(x1, y1, x2, y2);
  if (tr) tr->mapPoints(reinterpret_cast<PointF*>(&box), reinterpret_cast<PointF*>(&box), 2);

  if (box.x0 > box.x1) swap(box.x0, box.x1);
  if (box.y0 > box.y1) swap(box.y0, box.y1);
  return ERR_OK;
}

} // Fog namespace
