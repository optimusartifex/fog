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
#include <Fog/Svg/Dom/SvgPathElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

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

err_t SvgPathElement::onProcess(SvgVisitor* visitor) const
{
  if (_unused)
  {
    printf("A");
  }

  if (!a_d.isAssigned()) return ERR_OK;

  const PathF& path = a_d.getPath();
  return visitor->onPath((SvgElement*)this, path);
}

err_t SvgPathElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_d.isAssigned())
  {
    const PathF& path = a_d.getPath();
    return path._getBoundingBox(box, tr);
  }
  else
  {
    box.reset();
    return ERR_OK;
  }
}

} // Fog namespace
