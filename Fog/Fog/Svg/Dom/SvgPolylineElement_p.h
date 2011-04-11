// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGPOLYLINEELEMENT_P_H
#define _FOG_SVG_DOM_SVGPOLYLINEELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgPointsAttribute_p.h>
#include <Fog/Svg/Dom/SvgStyledElement_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgPolylineElement]
// ============================================================================

struct FOG_NO_EXPORT SvgPolylineElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPolylineElement();
  virtual ~SvgPolylineElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRenderShape(SvgRenderContext* context) const;
  virtual err_t onCalcBoundingBox(RectF* box) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgPointsAttribute a_points;

private:
  FOG_DISABLE_COPY(SvgPolylineElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGPOLYLINEELEMENT_P_H
