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
#include <Fog/Core/Xml/XmlDocument.h>
#include <Fog/Svg/Dom/SvgUseElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

static StubW parseHtmlLinkId(const StringW& url)
{
  const CharW* idStr;
  const CharW* idEnd;
  const CharW* idMark;
  CharW c;

  if (url.getLength() < 2) goto bail;

  idStr = url.getData();
  idEnd = idStr + url.getLength();

  if (*idStr != CharW('#')) goto bail;
  idStr++;

  while (idStr->isSpace())
  {
    if (++idStr == idEnd) goto bail;
  }

  idMark = idStr;
  while ((c = *idStr).isAsciiNumlet() || c == CharW('-') || c == CharW('_'))
  {
    if (++idStr == idEnd) break;
  }
  return StubW(idMark, (size_t)(idStr - idMark));

bail:
  return StubW((const CharW*)NULL, 0);
}

// ============================================================================
// [Fog::SvgUseElement]
// ============================================================================

SvgUseElement::SvgUseElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_use), SVG_ELEMENT_USE),
  a_x(NULL, FOG_STR_(SVG_ATTRIBUTE_x), FOG_OFFSET_OF(SvgUseElement, a_x)),
  a_y(NULL, FOG_STR_(SVG_ATTRIBUTE_y), FOG_OFFSET_OF(SvgUseElement, a_y))
{
}

SvgUseElement::~SvgUseElement()
{
  _removeAttributes();
}

XmlAttribute* SvgUseElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x)) return (XmlAttribute*)&a_x;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y)) return (XmlAttribute*)&a_y;

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

  StringW link = _getAttribute(FOG_STR_(SVG_ATTRIBUTE_xlink_href));
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
