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
#include <Fog/Svg/Dom/SvgRootElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgRootElement]
// ============================================================================

SvgRootElement::SvgRootElement() :
  SvgElement(fog_strings->getString(STR_SVG_ELEMENT_svg), SVG_ELEMENT_SVG),
  a_x     (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x     ), FOG_OFFSET_OF(SvgRootElement, a_x     )),
  a_y     (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y     ), FOG_OFFSET_OF(SvgRootElement, a_y     )),
  a_width (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_width ), FOG_OFFSET_OF(SvgRootElement, a_width )),
  a_height(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_height), FOG_OFFSET_OF(SvgRootElement, a_height))
{
}

SvgRootElement::~SvgRootElement()
{
  _removeAttributes();
}

XmlAttribute* SvgRootElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x     )) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y     )) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_width )) return (XmlAttribute*)&a_width;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_height)) return (XmlAttribute*)&a_height;

  return base::_createAttribute(name);
}

err_t SvgRootElement::onRender(SvgRenderContext* context) const
{
  err_t err = ERR_OK;
  SvgVisitor* visitor = context->getVisitor();

  if (visitor)
  {
    if (visitor->canVisit(const_cast<SvgRootElement*>(this)))
    {
      visitor->onBegin(const_cast<SvgRootElement*>(this));
      err = SvgElement::_walkAndRender(this, context);
      visitor->onEnd(const_cast<SvgRootElement*>(this));
    }
  }
  else
  {
    err = SvgElement::_walkAndRender(this, context);
  }

  return err;
}

err_t SvgRootElement::onCalcBoundingBox(RectF* box) const
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

} // Fog namespace
