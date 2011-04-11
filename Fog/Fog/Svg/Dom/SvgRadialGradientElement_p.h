// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGRADIALGRADIENTELEMENT_P_H
#define _FOG_SVG_DOM_SVGRADIALGRADIENTELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgCoordAttribute_p.h>
#include <Fog/Svg/Dom/SvgAbstractGradientElement_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgRadialGradientElement]
// ============================================================================

struct FOG_NO_EXPORT SvgRadialGradientElement : public SvgAbstractGradientElement
{
  typedef SvgAbstractGradientElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRadialGradientElement();
  virtual ~SvgRadialGradientElement();

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

  SvgCoordAttribute a_cx;
  SvgCoordAttribute a_cy;
  SvgCoordAttribute a_fx;
  SvgCoordAttribute a_fy;
  SvgCoordAttribute a_r;

private:
  FOG_DISABLE_COPY(SvgRadialGradientElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGRADIALGRADIENTELEMENT_P_H
