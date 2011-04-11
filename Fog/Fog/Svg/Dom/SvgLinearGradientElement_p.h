// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGLINEARGRADIENTELEMENT_P_H
#define _FOG_SVG_DOM_SVGLINEARGRADIENTELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgAbstractGradientElement_p.h>
#include <Fog/Svg/Dom/SvgCoordAttribute_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgLinearGradientElement]
// ============================================================================

struct FOG_NO_EXPORT SvgLinearGradientElement : public SvgAbstractGradientElement
{
  typedef SvgAbstractGradientElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgLinearGradientElement();
  virtual ~SvgLinearGradientElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRender(SvgRenderContext* context) const;
  virtual err_t onApplyPattern(SvgRenderContext* context, SvgElement* obj, int paintType) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x1;
  SvgCoordAttribute a_y1;
  SvgCoordAttribute a_x2;
  SvgCoordAttribute a_y2;

private:
  FOG_DISABLE_COPY(SvgLinearGradientElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGLINEARGRADIENTELEMENT_P_H
