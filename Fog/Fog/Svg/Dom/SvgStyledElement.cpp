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
#include <Fog/Svg/Dom/SvgDocument.h>
#include <Fog/Svg/Dom/SvgStyledElement_p.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

// Disable the MSC specific warning.
#if defined(FOG_CC_MSC)
# pragma warning(disable:4355) // 'this' used in base member initializer list.
#endif // FOG_CC_MSC

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

static int svgStyleToId(const ManagedStringW& name)
{
  int i;

  for (i = 0; i < SVG_STYLE_INVALID; i++)
  {
    if (name == ManagedStringCacheW::get()->getString(i + STR_SVG_STYLE_NAMES))
      break;
  }

  return i;
}

static StubW parseCssLinkId(const StringW& url)
{
  const CharW* idStr;
  const CharW* idEnd;
  const CharW* idMark;

  CharW quot;

  if (url.getLength() < 7) goto _Bail;

  idStr = url.getData();
  idEnd = idStr + url.getLength();

  // Add url(
  idStr += 4;

  // Detect quot, if used.
  quot = idStr[0];
  if (quot == CharW('\"') || quot == CharW('\''))
  {
    idStr++;
    idEnd--;
  }
  else
  {
    quot = CharW(')');
  }

  // Invalid ID.
  if (idStr + 1 >= idEnd || idStr[0] != CharW('#')) goto _Bail;
  idStr++;

  while (idStr->isSpace())
  {
    if (++idStr == idEnd) goto _Bail;
  }

  idMark = idStr;
  while (idStr[0] != quot)
  {
    if (++idStr == idEnd) goto _Bail;
  }
  return StubW(idMark, (size_t)(idStr - idMark));

_Bail:
  return StubW((const CharW*)NULL, 0);
}

// ============================================================================
// [Fog::SvgStyledElement]
// ============================================================================

SvgStyledElement::SvgStyledElement(const ManagedStringW& tagName, uint32_t svgType) :
  SvgElement(tagName, svgType),
  a_style    (this, FOG_STR_(XML_ATTRIBUTE_style), FOG_OFFSET_OF(SvgStyledElement, a_style)),
  a_transform(NULL, FOG_STR_(SVG_ATTRIBUTE_transform), FOG_OFFSET_OF(SvgStyledElement, a_transform))
{
  // Style attribute is always added as default and can't be removed.
  _attributes.append(&a_style);
}

SvgStyledElement::~SvgStyledElement()
{
  // Class that inherits us must destroy all attributes (a_style must be
  // removed from _attributes too).
  FOG_ASSERT(_attributes.isEmpty());
}

XmlAttribute* SvgStyledElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(XML_ATTRIBUTE_style)) return (XmlAttribute*)&a_style;
  if (name == FOG_STR_(SVG_ATTRIBUTE_transform)) return (XmlAttribute*)&a_transform;

  return base::_createAttribute(name);
}

err_t SvgStyledElement::_setAttribute(const ManagedStringW& name, const StringW& value)
{
  // Add css-style instead of attribute.
  int id = svgStyleToId(name);
  if ((uint)id < SVG_STYLE_INVALID)
  {
    // Do not return an error value here, because if Xml/Svg parser see error
    // then parsing is over. This means that there is probability we can't
    // render correctly some image, but we want to render what we can.
    a_style.setStyle(id, value);
    return ERR_OK;
  }

  return base::_setAttribute(name, value);
}

err_t SvgStyledElement::_removeAttribute(const ManagedStringW& name)
{
  if (name == FOG_STR_(XML_ATTRIBUTE_style)) return ERR_XML_ATTRIBUTE_CANT_BE_REMOVED;

  return base::_removeAttribute(name);
}

err_t SvgStyledElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  // Apply transformations and setup styles defined by this element.
  uint32_t styleMask = a_style.getMask();
  bool isTransformed = a_transform.isAssigned() & a_transform.isValid();

  if (styleMask != 0 || isTransformed)
  {
    SvgDocument* doc = reinterpret_cast<SvgDocument*>(getDocument());

    if (isTransformed)
    {
      if (state && !state->hasState(SvgGState::SAVED_TRANSFORM))
        state->saveTransform();
      visitor->transform(a_transform.getTransform());
    }

    // Setup global parameters.
    if (styleMask & ((1 << SVG_STYLE_OPACITY)))
    {
      if (state) state->saveGlobal();
      visitor->setOpacity(a_style._opacity);
    }

    // Setup font parameters.
    if (styleMask & ((1 << SVG_STYLE_FONT_FAMILY) |
                     (1 << SVG_STYLE_FONT_SIZE  )))
    {
      if (state) state->saveFont();

      StringW family = visitor->_font.getFamily();
      float size = visitor->_font.getHeight();

      if (styleMask & (1 << SVG_STYLE_FONT_FAMILY))
      {
        family = a_style._fontFamily;
      }

      if (styleMask & (1 << SVG_STYLE_FONT_SIZE))
      {
        size = doc->_dpi.toDeviceSpace(
          a_style._fontSizeValue, a_style._fontSizeUnit);
      }

      visitor->_font.create(family, size, UNIT_NONE);
    }

    // Setup fill parameters.
    if (styleMask & ((1 << SVG_STYLE_FILL               ) |
                     (1 << SVG_STYLE_FILL_OPACITY       ) |
                     (1 << SVG_STYLE_FILL_RULE          )))
    {
      if (state) state->saveFill();

      if (styleMask & (1 << SVG_STYLE_FILL))
      {
        switch (a_style._fillSource)
        {
          case SVG_SOURCE_NONE:
          case SVG_SOURCE_INVALID:
          {
            visitor->setFillNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            visitor->setFillColor(a_style._fillColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            XmlElement* r = getDocument()->getElementById(parseCssLinkId(a_style._fillUri));
            if (r && r->isSvg())
            {
              reinterpret_cast<SvgElement*>(r)->onPattern(visitor, const_cast<SvgStyledElement*>(this), SVG_PAINT_FILL);
            }
            break;
          }
        }
      }

      if (styleMask & (1 << SVG_STYLE_FILL_OPACITY))
      {
        visitor->setFillOpacity(a_style._fillOpacity);
      }

      if (styleMask & (1 << SVG_STYLE_FILL_RULE))
      {
        visitor->setFillRule(a_style._fillRule);
      }
    }

    // Setup stroke parameters.
    if (styleMask & ((1 << SVG_STYLE_STROKE             ) |
                     (1 << SVG_STYLE_STROKE_DASH_ARRAY  ) |
                     (1 << SVG_STYLE_STROKE_DASH_OFFSET ) |
                     (1 << SVG_STYLE_STROKE_LINE_CAP    ) |
                     (1 << SVG_STYLE_STROKE_LINE_JOIN   ) |
                     (1 << SVG_STYLE_STROKE_MITER_LIMIT ) |
                     (1 << SVG_STYLE_STROKE_OPACITY     ) |
                     (1 << SVG_STYLE_STROKE_WIDTH       )))
    {
      if (state) state->saveStroke();

      if (styleMask & (1 << SVG_STYLE_STROKE))
      {
        switch (a_style._strokeSource)
        {
          case SVG_SOURCE_NONE:
          case SVG_SOURCE_INVALID:
          {
            visitor->setStrokeNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            visitor->setStrokeColor(a_style._strokeColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            XmlElement* r = getDocument()->getElementById(parseCssLinkId(a_style._strokeUri));
            if (r && r->isSvg())
            {
              reinterpret_cast<SvgElement*>(r)->onPattern(visitor, const_cast<SvgStyledElement*>(this), SVG_PAINT_STROKE);
            }
            break;
          }
        }
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_LINE_CAP))
      {
        visitor->setLineCaps(a_style._strokeLineCap);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_LINE_JOIN))
      {
        visitor->setLineJoin(a_style._strokeLineJoin);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_MITER_LIMIT))
      {
        float miterLimit = doc->_dpi.toDeviceSpace(
          a_style._strokeMiterLimitValue, a_style._strokeMiterLimitUnit);
        visitor->setMiterLimit(miterLimit);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_OPACITY))
      {
        visitor->setStrokeOpacity(a_style._strokeOpacity);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_WIDTH))
      {
        float lineWidth = doc->_dpi.toDeviceSpace(
          a_style._strokeWidthValue, a_style._strokeWidthUnit);
        visitor->setLineWidth(lineWidth);
      }
    }
  }

  return ERR_OK;
}

StringW SvgStyledElement::getStyle(const StringW& name) const
{
  StringW result;
  ManagedStringW m_name(name, MANAGED_STRING_OPTION_LOOKUP);

  if (m_name.isEmpty())
    return result;

  int id = svgStyleToId(m_name);
  if ((uint)id >= SVG_STYLE_INVALID)
    return result;

  return a_style.getStyle(id);
}

err_t SvgStyledElement::setStyle(const StringW& name, const StringW& value)
{
  ManagedStringW m_name(name, MANAGED_STRING_OPTION_LOOKUP);

  if (m_name.isEmpty())
    return ERR_SVG_INVALID_STYLE_NAME;

  int id = svgStyleToId(m_name);
  if ((uint)id >= SVG_STYLE_INVALID)
    return ERR_SVG_INVALID_STYLE_NAME;

  return a_style.setStyle(id, value);
}

} // Fog namespace
