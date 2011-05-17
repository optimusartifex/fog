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
#include <Fog/Svg/Dom/SvgTextElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>
#include <Fog/Svg/Visit/SvgVisitor.h>
#include <Fog/Xml/Dom/XmlText.h>

namespace Fog {

// ============================================================================
// [Fog::SvgTextElement]
// ============================================================================

SvgTextElement::SvgTextElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_text), SVG_ELEMENT_TEXT),
  a_x           (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x         ), FOG_OFFSET_OF(SvgTextElement, a_x           )),
  a_y           (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y         ), FOG_OFFSET_OF(SvgTextElement, a_y           )),
  a_dx          (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_dx        ), FOG_OFFSET_OF(SvgTextElement, a_dx          )),
  a_dy          (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_dy        ), FOG_OFFSET_OF(SvgTextElement, a_dy          )),
  a_textLength  (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_textLength), FOG_OFFSET_OF(SvgTextElement, a_textLength  ))
{
}

SvgTextElement::~SvgTextElement()
{
  _removeAttributes();
}

XmlAttribute* SvgTextElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x           )) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y           )) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_dx          )) return (XmlAttribute*)&a_dx;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_dy          )) return (XmlAttribute*)&a_dy;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_textLength  )) return (XmlAttribute*)&a_textLength;

  return base::_createAttribute(name);
}

err_t SvgTextElement::onProcess(SvgVisitor* visitor) const
{
  err_t err = ERR_OK;
  XmlElement* e;

  float x = a_x.isAssigned() ? a_x.getCoordComputed() : 0.0f;
  float y = a_y.isAssigned() ? a_y.getCoordComputed() : 0.0f;
  visitor->_textCursor.set(x, y);

  for (e = getFirstChild(); e; e = e->getNextSibling())
  {
    if (e->isSvgElement() && reinterpret_cast<SvgElement*>(e)->getVisible())
    {
      err = visitor->onVisit(reinterpret_cast<SvgElement*>(e));
      if (FOG_IS_ERROR(err)) break;
    }

    if (e->isText())
    {
      String text = e->getTextContent();
      text.simplify();

      // TODO: Not optimal, just initial support for text rendering.
      PathD path;
      visitor->_font.getOutline(text, path);
      path.translate(PointD(x, y));

      err = visitor->onPath((SvgElement*)this, path);
      if (FOG_IS_ERROR(err)) break;
    }
  }

  return err;
}

err_t SvgTextElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  // TODO: SVGText bounding box.
  return ERR_RT_NOT_IMPLEMENTED;
}

} // Fog namespace
