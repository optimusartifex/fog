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
#include <Fog/Svg/Dom/SvgPathElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgPathElement]
// ============================================================================

SvgPathElement::SvgPathElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_path), SVG_ELEMENT_PATH),
  a_d(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_d), FOG_OFFSET_OF(SvgPathElement, a_d))
{
}

SvgPathElement::~SvgPathElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPathElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_d)) return (XmlAttribute*)&a_d;

  return base::_createAttribute(name);
}

err_t SvgPathElement::onRenderShape(SvgRenderContext* context) const
{
  if (a_d.isAssigned())
  {
    const PathF& path = a_d.getPath();
    context->drawPath(path);
    return ERR_OK;
  }
  else
  {
    return ERR_OK;
  }
}

err_t SvgPathElement::onCalcBoundingBox(RectF* box) const
{
  if (a_d.isAssigned())
  {
    const PathF& path = a_d.getPath();
    box->set(path.getBoundingRect());
    return ERR_OK;
  }
  else
  {
    box->reset();
    return ERR_OK;
  }
}

} // Fog namespace
