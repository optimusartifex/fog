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
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Gradient.h>
#include <Fog/Svg/Dom/SvgAbstractGradientElement_p.h>
#include <Fog/Svg/Dom/SvgEnumItem_p.h>
#include <Fog/Svg/Dom/SvgStopElement_p.h>
#include <Fog/Xml/Dom/XmlDocument.h>

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

static const SvgEnumItem svgEnum_spreadMethod[4] =
{
  { "pad", GRADIENT_SPREAD_PAD },
  { "reflect", GRADIENT_SPREAD_REFLECT },
  { "repeat", GRADIENT_SPREAD_REPEAT },
  { "", -1 }
};

// ============================================================================
// [Fog::SvgAbstractGradientElement]
// ============================================================================

SvgAbstractGradientElement::SvgAbstractGradientElement(const ManagedString& tagName, uint32_t svgType) :
  SvgStyledElement(tagName, svgType),
  a_spreadMethod     (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_spreadMethod     ), FOG_OFFSET_OF(SvgAbstractGradientElement, a_spreadMethod     ), svgEnum_spreadMethod),
  a_gradientUnits    (NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_gradientUnits    ), FOG_OFFSET_OF(SvgAbstractGradientElement, a_gradientUnits    ), svgEnum_gradientUnits),
  a_gradientTransform(NULL, fog_strings->getString(STR_SVG_ATTRIBUTE_gradientTransform), FOG_OFFSET_OF(SvgAbstractGradientElement, a_gradientTransform))
{
}

SvgAbstractGradientElement::~SvgAbstractGradientElement()
{
  _removeAttributes();
}

XmlAttribute* SvgAbstractGradientElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_spreadMethod     )) return (XmlAttribute*)&a_spreadMethod;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_gradientUnits    )) return (XmlAttribute*)&a_gradientUnits;
  if (name == fog_strings->getString(STR_SVG_ATTRIBUTE_gradientTransform)) return (XmlAttribute*)&a_gradientTransform;

  return base::_createAttribute(name);
}

void SvgAbstractGradientElement::_walkAndAddColorStops(XmlElement* root, GradientF& gradient)
{
  bool stopsParsed = false;
  XmlElement* stop;
  int depth = 0;

_Start:
  for (stop = root->firstChild(); stop; stop = stop->nextSibling())
  {
    if (stop->isSvg() && reinterpret_cast<SvgElement*>(stop)->getSvgType() == SVG_ELEMENT_STOP)
    {
      SvgStopElement* _stop = reinterpret_cast<SvgStopElement*>(stop);

      if (_stop->a_offset.isAssigned() && _stop->a_style.hasStyle(SVG_STYLE_STOP_COLOR))
      {
        float offset = _stop->a_offset.getOffset();
        Color color(_stop->a_style._stopColor);

        if (_stop->a_style.hasStyle(SVG_STYLE_STOP_OPACITY))
        {
          color.setAlpha(_stop->a_style._stopOpacity);
        }

        gradient.addStop(ColorStop(offset, color));
        stopsParsed = true;
      }
    }
  }

  if (!stopsParsed)
  {
    XmlElement* e;
    String link = root->_getAttribute(fog_strings->getString(STR_SVG_ATTRIBUTE_xlink_href));

    if ((!link.isEmpty() && link.getAt(0) == Char('#')) && 
        (e = root->getDocument()->getElementById(Utf16(link.getData() + 1, link.getLength() - 1))))
    {
      root = e;
      if (++depth == 32) return;
      goto _Start;
    }
  }
}

} // Fog namespace
