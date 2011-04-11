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
#include <Fog/Svg/Dom/SvgUseElement_p.h>
#include <Fog/Svg/Render/SvgRender.h>
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
  while ((c = *idStr).isAlnum() || c == Char('_'))
  {
    if (++idStr == idEnd) break;
  }
  return Utf16(idMark, (sysuint_t)(idStr - idMark));

bail:
  return Utf16((const Char*)NULL, 0);
}

// ============================================================================
// [Fog::SvgUseElement]
// ============================================================================

SvgUseElement::SvgUseElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_use), SVG_ELEMENT_USE)
{
}

SvgUseElement::~SvgUseElement()
{
  _removeAttributes();
}

err_t SvgUseElement::onRenderShape(SvgRenderContext* context) const
{
  String link = _getAttribute(fog_strings->getString(STR_SVG_ATTRIBUTE_xlink_href));
  XmlElement* ref = getDocument()->getElementById(parseHtmlLinkId(link));

  if (ref && ref->isSvgElement())
  {
    reinterpret_cast<SvgElement*>(ref)->onRender(context);
    return ERR_OK;
  }

  return ERR_OK;
}

err_t SvgUseElement::onCalcBoundingBox(RectF* box) const
{
  box->reset();
  return ERR_OK;
}

} // Fog namespace
