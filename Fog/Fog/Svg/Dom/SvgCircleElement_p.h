// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGCIRCLEELEMENT_P_H
#define _FOG_SVG_DOM_SVGCIRCLEELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgCoordAttribute_p.h>
#include <Fog/Svg/Dom/SvgStyledElement_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgCircleElement]
// ============================================================================

struct FOG_NO_EXPORT SvgCircleElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgCircleElement();
  virtual ~SvgCircleElement();

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

  SvgCoordAttribute a_cx;
  SvgCoordAttribute a_cy;
  SvgCoordAttribute a_r;

private:
  FOG_DISABLE_COPY(SvgCircleElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGCIRCLEELEMENT_P_H
