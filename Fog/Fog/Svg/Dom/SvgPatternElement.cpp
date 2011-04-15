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
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/Svg/Dom/SvgPatternElement_p.h>
#include <Fog/Svg/Render/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgEnumItem - Data]
// ============================================================================

static const SvgEnumItem svgEnum_gradientUnits[3] =
{
  { "userSpaceOnUse", SVG_USER_SPACE_ON_USE },
  { "objectBoundingBox", SVG_OBJECT_BOUNDING_BOX },
  { "", -1 }
};

// ============================================================================
// [Fog::SvgPatternElement]
// ============================================================================

SvgPatternElement::SvgPatternElement() :
  SvgElement(fog_strings->getString(STR_SVG_ELEMENT_pattern), SVG_ELEMENT_PATTERN),
  a_x           (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x           ), FOG_OFFSET_OF(SvgPatternElement, a_x           )),
  a_y           (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y           ), FOG_OFFSET_OF(SvgPatternElement, a_y           )),
  a_width       (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_width       ), FOG_OFFSET_OF(SvgPatternElement, a_width       )),
  a_height      (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_height      ), FOG_OFFSET_OF(SvgPatternElement, a_height      )),
  a_patternUnits(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_patternUnits), FOG_OFFSET_OF(SvgPatternElement, a_patternUnits), svgEnum_gradientUnits)
{
}

SvgPatternElement::~SvgPatternElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPatternElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x           )) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y           )) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_width       )) return (XmlAttribute*)&a_width;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_height      )) return (XmlAttribute*)&a_height;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_patternUnits)) return (XmlAttribute*)&a_patternUnits;

  return base::_createAttribute(name);
}

err_t SvgPatternElement::onRender(SvgRenderContext* context) const
{
  // Pattern is not rendered by standard rendering process.
  return ERR_OK;
}

err_t SvgPatternElement::onApplyPattern(SvgRenderContext* context, SvgElement* obj, int paintType) const
{
  int w = a_width.isAssigned()  ? (int)a_width.getCoordValue()  : 0;
  int h = a_height.isAssigned() ? (int)a_height.getCoordValue() : 0;

  // TODO: Here the error code should be returned.
  if (w == 0 || h == 0) return ERR_OK;

  Image image;
  FOG_RETURN_ON_ERROR(image.create(SizeI(w, h), IMAGE_FORMAT_PRGB32));  
  image.clear(Color(Argb32(0x00000000)));

  Painter painter(image);
  SvgRenderContext ctx(&painter);
  SvgElement::_walkAndRender(this, &ctx);

  PatternF pattern;
  pattern.setTexture(Texture(image));

  if (paintType == SVG_PAINT_FILL)
    context->setFillPattern(pattern);
  else
    context->setStrokePattern(pattern);

  return ERR_OK;
}

} // Fog namespace
