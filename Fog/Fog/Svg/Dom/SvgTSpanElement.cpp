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
#include <Fog/Svg/Dom/SvgTSpanElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>
#include <Fog/Xml/Dom/XmlText.h>

namespace Fog {

// ============================================================================
// [Fog::SvgEnumItem - Data]
// ============================================================================

static const SvgEnumItem svgEnum_lengthAdjust[3] =
{
  { "spacing", SVG_LENGTH_ADJUST_SPACING },
  { "spacingAndGlyphs", SVG_LENGTH_ADJUST_SPACING_AND_GLYPHS },
  { "", -1 }
};

// ============================================================================
// [Fog::SvgTSpanElement]
// ============================================================================

SvgTSpanElement::SvgTSpanElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ELEMENT_text), SVG_ELEMENT_TSPAN),
  a_x           (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x           ), FOG_OFFSET_OF(SvgTSpanElement, a_x           )),
  a_y           (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y           ), FOG_OFFSET_OF(SvgTSpanElement, a_y           )),
  a_dx          (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_dx          ), FOG_OFFSET_OF(SvgTSpanElement, a_dx          )),
  a_dy          (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_dy          ), FOG_OFFSET_OF(SvgTSpanElement, a_dy          )),
  a_textLength  (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_textLength  ), FOG_OFFSET_OF(SvgTSpanElement, a_textLength  )),
  a_lengthAdjust(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_lengthAdjust), FOG_OFFSET_OF(SvgTSpanElement, a_lengthAdjust), svgEnum_lengthAdjust)
{
}

SvgTSpanElement::~SvgTSpanElement()
{
  _removeAttributes();
}

XmlAttribute* SvgTSpanElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x         )) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y         )) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_dx        )) return (XmlAttribute*)&a_dx;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_dy        )) return (XmlAttribute*)&a_dy;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_textLength)) return (XmlAttribute*)&a_textLength;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_lengthAdjust)) return (XmlAttribute*)&a_lengthAdjust;

  return base::_createAttribute(name);
}

err_t SvgTSpanElement::onRenderShape(SvgRenderContext* context) const
{
  err_t err = ERR_OK;

  float x = context->_textCursor.x;
  float y = context->_textCursor.y;

  if (a_dx.isAssigned()) x += a_dx.getCoordValue();
  if (a_dy.isAssigned()) y += a_dy.getCoordValue();

  String text = getTextContent();
  text.simplify();

  // TODO: Not optimal, just initial support for text rendering.
  PathD path;
  context->_font.getOutline(text, path);
  path.translate(PointD(x, y));
  context->drawPath(path);

  context->_textCursor.set(x, y);
  return err;
}

} // Fog namespace
