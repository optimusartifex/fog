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
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/Svg/Dom/SvgDocument.h>
#include <Fog/Svg/Dom/SvgPatternElement_p.h>
#include <Fog/Svg/Visit/SvgRender.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

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
  a_x               (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_x               ), FOG_OFFSET_OF(SvgPatternElement, a_x               )),
  a_y               (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_y               ), FOG_OFFSET_OF(SvgPatternElement, a_y               )),
  a_width           (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_width           ), FOG_OFFSET_OF(SvgPatternElement, a_width           )),
  a_height          (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_height          ), FOG_OFFSET_OF(SvgPatternElement, a_height          )),
  a_patternUnits    (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_patternUnits    ), FOG_OFFSET_OF(SvgPatternElement, a_patternUnits    ), svgEnum_gradientUnits),
  a_patternTransform(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_patternTransform), FOG_OFFSET_OF(SvgPatternElement, a_patternTransform)),
  a_viewBox         (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_viewBox         ), FOG_OFFSET_OF(SvgPatternElement, a_viewBox         ))
{
}

SvgPatternElement::~SvgPatternElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPatternElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_x               )) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_y               )) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_width           )) return (XmlAttribute*)&a_width;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_height          )) return (XmlAttribute*)&a_height;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_patternUnits    )) return (XmlAttribute*)&a_patternUnits;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_patternTransform)) return (XmlAttribute*)&a_patternTransform;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_viewBox         )) return (XmlAttribute*)&a_viewBox;

  return base::_createAttribute(name);
}

err_t SvgPatternElement::onProcess(SvgVisitor* visitor) const
{
  return ERR_OK;
}

err_t SvgPatternElement::onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const
{
  PatternF pattern;
  FOG_RETURN_ON_ERROR(_createPattern(pattern, obj));

  if (paintType == SVG_PAINT_FILL)
    visitor->setFillPattern(pattern);
  else
    visitor->setStrokePattern(pattern);
  return ERR_OK;
}

err_t SvgPatternElement::_createPattern(PatternF& pattern, SvgElement* obj) const
{
  String link = _getAttribute(fog_strings->getString(STR_SVG_ATTRIBUTE_xlink_href));

  if (!link.isEmpty() && link.getAt(0) == Char('#'))
  {
    XmlElement* e = getDocument()->getElementById(Utf16(link.getData() + 1, link.getLength() - 1));
    if (e != NULL && e->isSvg() && reinterpret_cast<SvgElement*>(e)->getSvgType() == SVG_ELEMENT_PATTERN)
    {
      SvgPatternElement* pe = reinterpret_cast<SvgPatternElement*>(e);
      FOG_RETURN_ON_ERROR(pe->_createPattern(pattern, obj));
      goto _AssignTransform;
    }
  }

  // TODO: Object bounding box support.
  {
    int w = a_width.isAssigned()  ? (int)a_width.getCoordValue()  : 0;
    int h = a_height.isAssigned() ? (int)a_height.getCoordValue() : 0;
    if (w == 0 || h == 0) return ERR_IMAGE_INVALID_SIZE;

    Image image;
    FOG_RETURN_ON_ERROR(image.create(SizeI(w, h), IMAGE_FORMAT_PRGB32));

    Painter painter(image);
    painter.setSource(Argb32(0x00000000));
    painter.setCompositingOperator(COMPOSITE_SRC);
    painter.fillAll();

    if (a_viewBox.isAssigned() && a_viewBox.isValid())
    {
      const BoxF& box = a_viewBox.getBox();
      float bbw = box.getWidth();
      float bbh = box.getHeight();

      if (bbw > MATH_EPSILON_F && bbh > MATH_EPSILON_F)
      {
        TransformF tr(
          float(w) / box.getWidth(), 0.0f,
          0.0f, float(h) / box.getHeight(),
          -box.x0, -box.y0);
        painter.setTransform(tr);
      }
    }

    SvgRender render(&painter);
    _visitContainer(&render);
    painter.end();

    pattern.setTexture(Texture(image, TEXTURE_TILE_REPEAT));

    float tx = a_x.isAssigned() ? a_x.getCoordValue() : 0.0f;
    float ty = a_y.isAssigned() ? a_y.getCoordValue() : 0.0f;
    pattern.translate(PointF(tx, ty));
  }

_AssignTransform:
  if (a_patternTransform.isAssigned() && a_patternTransform.isValid())
    pattern.transform(a_patternTransform.getTransform());

  return ERR_OK;
}

} // Fog namespace
