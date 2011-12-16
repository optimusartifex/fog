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
#include <Fog/Svg/Dom/SvgGElement_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgGElement]
// ============================================================================

SvgGElement::SvgGElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_g), SVG_ELEMENT_G)
{
}

SvgGElement::~SvgGElement()
{
  _removeAttributes();
}

err_t SvgGElement::onProcess(SvgVisitor* visitor) const
{
  if (!hasChildNodes()) return ERR_OK;
  return _visitContainer(visitor);
}

err_t SvgGElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (!hasChildNodes())
  {
    box.reset();
    return ERR_GEOMETRY_NONE;
  }

  // TODO: It seems that there is nothing to do, because all render specific
  // options are related to a single SVG element, not to a group.
  // return _walkAndMergeBBox(this);

  return ERR_RT_NOT_IMPLEMENTED;
}

} // Fog namespace
