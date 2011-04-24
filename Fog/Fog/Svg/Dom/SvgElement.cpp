// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Svg/Dom/SvgDocument.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Visit/SvgRender.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgElement]
// ============================================================================

SvgElement::SvgElement(const ManagedString& tagName, uint32_t svgType) :
  XmlElement(tagName),
  _svgType(svgType),
  _boundingRectDirty(true),
  _visible(true),
  _unused(0)
{
  _type |= SVG_ELEMENT_MASK;
  _flags &= ~(XML_ALLOWED_TAG);
}

SvgElement::~SvgElement()
{
  // Class that inherits us have to destroy all attributes.
  FOG_ASSERT(_attributes.isEmpty());
}

SvgElement* SvgElement::clone() const
{
  SvgElement* e = reinterpret_cast<SvgElement*>(SvgDocument::createElementStatic(_tagName));
  if (e) _copyAttributes(e, const_cast<SvgElement*>(this));
  return e;
}

XmlAttribute* SvgElement::_createAttribute(const ManagedString& name) const
{
  return base::_createAttribute(name);
}

err_t SvgElement::onRender(SvgRenderContext* context) const
{
  // Default is to render nothing and stop traversing
  return ERR_OK;
}

err_t SvgElement::onRenderShape(SvgRenderContext* context) const
{
  return ERR_OK;
}

err_t SvgElement::onApplyPattern(SvgRenderContext* context, SvgElement* obj, int paintType) const
{
  return ERR_RT_INVALID_OBJECT;
}

err_t SvgElement::onCalcBoundingBox(RectF* box) const
{
  return ERR_RT_INVALID_OBJECT;
}

err_t SvgElement::_walkAndRender(const XmlElement* root, SvgRenderContext* context)
{
  err_t err = ERR_OK;
  XmlElement* e;

  SvgVisitor* visitor = context->getVisitor();
  if (visitor == NULL)
  {
    for (e = root->getFirstChild(); e; e = e->getNextSibling())
    {
      if (e->isSvgElement() && reinterpret_cast<SvgElement*>(e)->getVisible())
      {
        err = reinterpret_cast<SvgElement*>(e)->onRender(context);
        if (FOG_IS_ERROR(err)) break;
      }
    }
  }
  else
  {
    for (e = root->getFirstChild(); e; e = e->getNextSibling())
    {
      if (e->isSvgElement() && reinterpret_cast<SvgElement*>(e)->getVisible() && visitor->canVisit(e))
      {
        visitor->onBegin(e);
        err = reinterpret_cast<SvgElement*>(e)->onRender(context);
        visitor->onEnd(e);

        if (FOG_IS_ERROR(err)) break;
      }
    }
  }

  return err;
}

String SvgElement::getStyle(const String& name) const
{
  return String();
}

err_t SvgElement::setStyle(const String& name, const String& value)
{
  return ERR_RT_INVALID_OBJECT;
}

const RectF& SvgElement::getBoundingRect() const
{
  if (_boundingRectDirty)
  {
    onCalcBoundingBox(&_boundingRect);
    _boundingRectDirty = false;
  }

  return _boundingRect;
}

} // Fog namespace
