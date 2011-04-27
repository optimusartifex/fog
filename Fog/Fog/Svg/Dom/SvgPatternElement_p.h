// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGPATTERNELEMENT_P_H
#define _FOG_SVG_DOM_SVGPATTERNELEMENT_P_H

// [Dependencies]
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/Svg/Dom/SvgCoordAttribute_p.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Dom/SvgEnumAttribute_p.h>
#include <Fog/Svg/Dom/SvgTransformAttribute_p.h>
#include <Fog/Svg/Dom/SvgViewBoxAttribute_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgPatternElement]
// ============================================================================

struct FOG_NO_EXPORT SvgPatternElement : public SvgElement
{
  typedef SvgElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPatternElement();
  virtual ~SvgPatternElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRender(SvgRenderContext* context) const;
  virtual err_t onApplyPattern(SvgRenderContext* context, SvgElement* obj, int paintType) const;

  err_t _createPattern(PatternF& pattern, SvgElement* obj) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgEnumAttribute a_patternUnits;
  SvgTransformAttribute a_patternTransform;
  SvgViewBoxAttribute a_viewBox;

private:
  FOG_DISABLE_COPY(SvgPatternElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGPATTERNELEMENT_P_H
