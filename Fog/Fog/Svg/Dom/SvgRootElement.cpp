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
#include <Fog/Svg/Dom/SvgRootElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgRootElement]
// ============================================================================

SvgRootElement::SvgRootElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_svg), SVG_ELEMENT_SVG),
  a_x      (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x      ), FOG_OFFSET_OF(SvgRootElement, a_x      )),
  a_y      (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y      ), FOG_OFFSET_OF(SvgRootElement, a_y      )),
  a_width  (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_width  ), FOG_OFFSET_OF(SvgRootElement, a_width  )),
  a_height (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_height ), FOG_OFFSET_OF(SvgRootElement, a_height )),
  a_viewBox(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_viewBox), FOG_OFFSET_OF(SvgRootElement, a_viewBox))
{
}

SvgRootElement::~SvgRootElement()
{
  _removeAttributes();
}

XmlAttribute* SvgRootElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x      )) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y      )) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_width  )) return (XmlAttribute*)&a_width;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_height )) return (XmlAttribute*)&a_height;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_viewBox)) return (XmlAttribute*)&a_viewBox;

  return base::_createAttribute(name);
}

err_t SvgRootElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  if (a_viewBox.isAssigned() && a_viewBox.isValid())
  {
    if (state) state->saveTransform();

    SizeF size = getRootSize();
    BoxF box = a_viewBox.getBox();

    TransformF tr(
      size.w / box.getWidth(), 0.0f,
      0.0f, size.h / box.getHeight(),
      -box.x0, -box.y0);
    visitor->transform(tr);
  }

  return ERR_OK;
}

err_t SvgRootElement::onProcess(SvgVisitor* visitor) const
{
  if (!hasChildNodes()) return ERR_OK;
  return _visitContainer(visitor);
}

err_t SvgRootElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

SizeF SvgRootElement::getRootSize() const
{
  SizeF size(0.0f, 0.0f);

  // Width/Height of document are assigned.
  if (a_width.isAssigned() && a_height.isAssigned())
  {
    float w = a_width.getCoordComputed();
    float h = a_height.getCoordComputed();

    size.set(w, h);
  }

  // If size is not specified of value is invalid, initialize it to
  // safe values.
  if (size.w <= 0.0f || size.h <= 0.0f)
  {
    size.w = 128.0f;
    size.h = 128.0f;
  }

  return size;
}

} // Fog namespace
