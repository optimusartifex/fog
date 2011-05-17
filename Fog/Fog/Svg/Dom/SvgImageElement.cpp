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
#include <Fog/Svg/Dom/SvgImageElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgImageElement]
// ============================================================================

SvgImageElement::SvgImageElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_image), SVG_ELEMENT_IMAGE),
  a_x      (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x         ), FOG_OFFSET_OF(SvgImageElement, a_x     )),
  a_y      (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y         ), FOG_OFFSET_OF(SvgImageElement, a_y     )),
  a_width  (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_width     ), FOG_OFFSET_OF(SvgImageElement, a_width )),
  a_height (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_height    ), FOG_OFFSET_OF(SvgImageElement, a_height)),
  a_href   (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_xlink_href), FOG_OFFSET_OF(SvgImageElement, a_href  ))
{
}

SvgImageElement::~SvgImageElement()
{
  _removeAttributes();
}

XmlAttribute* SvgImageElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x         )) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y         )) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_width     )) return (XmlAttribute*)&a_width;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_height    )) return (XmlAttribute*)&a_height;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_xlink_href)) return (XmlAttribute*)&a_href;

  return base::_createAttribute(name);
}

err_t SvgImageElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  // There is only transformation attribute which can be applied to the image.
  if (a_transform.isAssigned() && a_transform.isValid())
  {
    if (state && !state->hasState(SvgGState::SAVED_TRANSFORM))
      state->saveTransform();
    visitor->transform(a_transform.getTransform());
  }

  return ERR_OK;
}

err_t SvgImageElement::onProcess(SvgVisitor* visitor) const
{
  if (!a_href._embedded || a_href._image.isEmpty()) return ERR_OK;

  float x = a_x.isAssigned() ? a_x.getCoordComputed() : 0.0f;
  float y = a_y.isAssigned() ? a_y.getCoordComputed() : 0.0f;

  return visitor->onImage((SvgElement*)this, PointF(x, y), a_href._image);
}

} // Fog namespace
