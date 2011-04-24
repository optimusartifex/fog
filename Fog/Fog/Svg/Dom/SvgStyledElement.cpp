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
#include <Fog/Svg/Dom/SvgStyledElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>
#include <Fog/Xml/Dom/XmlDocument.h>

// Disable the MSC specific warning.
#if defined(FOG_CC_MSC)
# pragma warning(disable:4355) // 'this' used in base member initializer list.
#endif // FOG_CC_MSC

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

static int svgStyleToId(const ManagedString& name)
{
  int i;

  for (i = 0; i < SVG_STYLE_INVALID; i++)
  {
    if (name == fog_strings->getString(i + STR_SVG_STYLE_NAMES)) break;
  }

  return i;
}

static Utf16 parseCssLinkId(const String& url)
{
  const Char* idStr;
  const Char* idEnd;
  const Char* idMark;

  Char quot;

  if (url.getLength() < 7) goto _Bail;

  idStr = url.getData();
  idEnd = idStr + url.getLength();

  // Add url(
  idStr += 4;

  // Detect quot, if used.
  quot = idStr[0];
  if (quot == Char('\"') || quot == Char('\''))
  {
    idStr++;
    idEnd--;
  }
  else
  {
    quot = Char(')');
  }

  // Invalid ID.
  if (idStr + 1 >= idEnd || idStr[0] != Char('#')) goto _Bail;
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
  return Utf16(idMark, (sysuint_t)(idStr - idMark));

_Bail:
  return Utf16((const Char*)NULL, 0);
}

// ============================================================================
// [Fog::SvgStyledElement]
// ============================================================================

SvgStyledElement::SvgStyledElement(const ManagedString& tagName, uint32_t svgType) :
  SvgElement(tagName, svgType),
  a_style    (this, fog_strings->getString(STR_XML_ATTRIBUTE_style), FOG_OFFSET_OF(SvgStyledElement, a_style)),
  a_transform(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_transform), FOG_OFFSET_OF(SvgStyledElement, a_transform))
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

XmlAttribute* SvgStyledElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_XML_ATTRIBUTE_style)) return (XmlAttribute*)&a_style;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_transform)) return (XmlAttribute*)&a_transform;

  return base::_createAttribute(name);
}

err_t SvgStyledElement::_setAttribute(const ManagedString& name, const String& value)
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

err_t SvgStyledElement::_removeAttribute(const ManagedString& name)
{
  if (name == fog_strings->getString(STR_XML_ATTRIBUTE_style)) return ERR_XML_ATTRIBUTE_CANT_BE_REMOVED;

  return base::_removeAttribute(name);
}

err_t SvgStyledElement::onRender(SvgRenderContext* context) const
{
  SvgRenderState state(context);
  err_t err = ERR_OK;

  uint32_t styleMask = a_style.getMask();
  bool isTransformed = a_transform.isAssigned() & a_transform.isValid();

  // Before render: Apply transformations and setup styles defined by this element.
  if (styleMask != 0 || isTransformed)
  {
    if (isTransformed)
    {
      state.saveTransform();
      context->getPainter()->transform(a_transform.getTransform());
    }

    // Setup global parameters.
    if (styleMask & ((1 << SVG_STYLE_OPACITY)))
    {
      state.saveGlobal();
      context->setOpacity(a_style._opacity);
    }

    // Setup font parameters.
    if (styleMask & ((1 << SVG_STYLE_FONT_FAMILY) |
                     (1 << SVG_STYLE_FONT_SIZE  )))
    {
      state.saveFont();

      if (styleMask & (1 << SVG_STYLE_FONT_FAMILY))
      {
        context->_font.setFamily(a_style._fontFamily);
      }

      if (styleMask & (1 << SVG_STYLE_FONT_SIZE))
      {
        context->_font.setSize(a_style._fontSize.value);
      }
    }

    // Setup fill parameters.
    if (styleMask & ((1 << SVG_STYLE_FILL               ) |
                     (1 << SVG_STYLE_FILL_OPACITY       ) |
                     (1 << SVG_STYLE_FILL_RULE          )))
    {
      state.saveFill();

      if (styleMask & (1 << SVG_STYLE_FILL))
      {
        switch (a_style._fillSource)
        {
          case SVG_SOURCE_NONE:
          case SVG_SOURCE_INVALID:
          {
            context->setFillNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            context->setFillColor(a_style._fillColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            XmlElement* r = getDocument()->getElementById(parseCssLinkId(a_style._fillUri));
            if (r && r->isSvg())
            {
              reinterpret_cast<SvgElement*>(r)->onApplyPattern(context, const_cast<SvgStyledElement*>(this), SVG_PAINT_FILL);
            }
            break;
          }
        }
      }

      if (styleMask & (1 << SVG_STYLE_FILL_OPACITY))
      {
        context->setFillOpacity(a_style._fillOpacity);
      }

      if (styleMask & (1 << SVG_STYLE_FILL_RULE))
      {
        context->setFillRule(a_style._fillRule);
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
      state.saveStroke();

      if (styleMask & (1 << SVG_STYLE_STROKE))
      {
        switch (a_style._strokeSource)
        {
          case SVG_SOURCE_NONE:
          case SVG_SOURCE_INVALID:
          {
            context->setStrokeNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            context->setStrokeColor(a_style._strokeColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            XmlElement* r = getDocument()->getElementById(parseCssLinkId(a_style._strokeUri));
            if (r && r->isSvg())
            {
              reinterpret_cast<SvgElement*>(r)->onApplyPattern(context, const_cast<SvgStyledElement*>(this), SVG_PAINT_STROKE);
            }
            break;
          }
        }
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_LINE_CAP))
      {
        context->setLineCaps(a_style._strokeLineCap);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_LINE_JOIN))
      {
        context->setLineJoin(a_style._strokeLineJoin);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_MITER_LIMIT))
      {
        float miterLimit = context->translateCoord(a_style._strokeMiterLimit);
        context->setMiterLimit(miterLimit);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_OPACITY))
      {
        context->setStrokeOpacity(a_style._strokeOpacity);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_WIDTH))
      {
        float lineWidth = context->translateCoord(a_style._strokeWidth);
        context->setLineWidth(lineWidth);
      }
    }
  }

  // Actual render: object or children (depends to Svg type).
  err = onRenderShape(context);

  // After render: SvgContextBackup destructor will restore SvgContext state
  // if modified.
  return err;
}

String SvgStyledElement::getStyle(const String& name) const
{
  ManagedString managedName;
  String result;
  int id;

  if (managedName.setIfManaged(name) == ERR_OK &&
      (uint)(id = svgStyleToId(managedName)) < SVG_STYLE_INVALID)
  {
    result = a_style.getStyle(id);
  }

  return result;
}

err_t SvgStyledElement::setStyle(const String& name, const String& value)
{
  ManagedString managedName;
  err_t err = ERR_SVG_INVALID_STYLE_NAME;
  int id;

  if (managedName.setIfManaged(name) == ERR_OK &&
      (uint)(id = svgStyleToId(managedName)) < SVG_STYLE_INVALID)
  {
    err = a_style.setStyle(id, value);
  }

  return err;
}

} // Fog namespace
