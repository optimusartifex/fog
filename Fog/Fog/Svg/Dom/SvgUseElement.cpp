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
#include <Fog/Svg/Dom/SvgUseElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>
#include <Fog/Xml/Dom/XmlDocument.h>

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

static Utf16 parseHtmlLinkId(const String& url)
{
  const Char* idStr;
  const Char* idEnd;
  const Char* idMark;
  Char c;

  if (url.getLength() < 2) goto bail;

  idStr = url.getData();
  idEnd = idStr + url.getLength();

  if (*idStr != Char('#')) goto bail;
  idStr++;

  while (idStr->isSpace())
  {
    if (++idStr == idEnd) goto bail;
  }

  idMark = idStr;
  while ((c = *idStr).isAlnum() || c == Char('-') || c == Char('_'))
  {
    if (++idStr == idEnd) break;
  }
  return Utf16(idMark, (size_t)(idStr - idMark));

bail:
  return Utf16((const Char*)NULL, 0);
}

// ============================================================================
// [Fog::SvgUseElement]
// ============================================================================

SvgUseElement::SvgUseElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_use), SVG_ELEMENT_USE),
  a_x(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x), FOG_OFFSET_OF(SvgUseElement, a_x)),
  a_y(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y), FOG_OFFSET_OF(SvgUseElement, a_y))
{
}

SvgUseElement::~SvgUseElement()
{
  _removeAttributes();
}

XmlAttribute* SvgUseElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x)) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y)) return (XmlAttribute*)&a_y;

  return base::_createAttribute(name);
}

err_t SvgUseElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  float tx = 0.0f;
  float ty = 0.0f;

  if (a_x.isAssigned()) tx = a_x.getCoordComputed();
  if (a_y.isAssigned()) ty = a_y.getCoordComputed();

  if (tx != 0.0f || ty != 0.0f)
  {
    if (state && !state->hasState(SvgGState::SAVED_TRANSFORM)) state->saveTransform();
    visitor->translate(PointF(tx, ty));
  }

  return ERR_OK;
}

err_t SvgUseElement::onProcess(SvgVisitor* visitor) const
{
  err_t err = ERR_OK;

  String link = _getAttribute(fog_strings->getString(STR_SVG_ATTRIBUTE_xlink_href));
  XmlElement* ref = getDocument()->getElementById(parseHtmlLinkId(link));

  if (ref && ref->isSvgElement())
    err = visitor->onVisit(reinterpret_cast<SvgElement*>(ref));

  return err;
}

err_t SvgUseElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  box.reset();
  return ERR_OK;
}

} // Fog namespace
