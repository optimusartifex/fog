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
#include <Fog/Svg/Dom/SvgGElement_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgGElement]
// ============================================================================

SvgGElement::SvgGElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_g), SVG_ELEMENT_G)
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
  if (!hasChildNodes()) { box.reset(); return ERR_GEOMETRY_NONE; }

  // return _walkAndMergeBBox(this
}

} // Fog namespace
