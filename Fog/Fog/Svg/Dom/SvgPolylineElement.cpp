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
#include <Fog/Svg/Dom/SvgPolylineElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgPolylineElement]
// ============================================================================

SvgPolylineElement::SvgPolylineElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_polyline), SVG_ELEMENT_POLYLINE),
  a_points(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_points), false, FOG_OFFSET_OF(SvgPolylineElement, a_points))
{
}

SvgPolylineElement::~SvgPolylineElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPolylineElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_points)) return (XmlAttribute*)&a_points;

  return base::_createAttribute(name);
}

err_t SvgPolylineElement::onRenderShape(SvgRenderContext* context) const
{
  if (a_points.isAssigned())
  {
    const PathF& path = a_points.getPath();
    context->drawPath(path);
    return ERR_OK;
  }
  else
  {
    return ERR_OK;
  }
}

err_t SvgPolylineElement::onCalcBoundingBox(RectF* box) const
{
  if (a_points.isAssigned())
  {
    const PathF& path = a_points.getPath();
    box->setRect(path.getBoundingRect());
    return ERR_OK;
  }
  else
  {
    box->reset();
    return ERR_OK;
  }
}

} // Fog namespace
