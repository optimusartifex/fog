// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGABSTRACTGRADIENTELEMENT_P_H
#define _FOG_SVG_DOM_SVGABSTRACTGRADIENTELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgEnumAttribute_p.h>
#include <Fog/Svg/Dom/SvgStyledElement_p.h>
#include <Fog/Svg/Dom/SvgTransformAttribute_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct GradientF;
struct GradientD;

// ============================================================================
// [Fog::SvgAbstractGradientElement]
// ============================================================================

struct FOG_NO_EXPORT SvgAbstractGradientElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgAbstractGradientElement(const ManagedString& tagName, uint32_t svgType);
  virtual ~SvgAbstractGradientElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Visiting]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static void _walkAndAddColorStops(XmlElement* root, GradientF& gradient);

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgEnumAttribute a_spreadMethod;
  SvgEnumAttribute a_gradientUnits;
  SvgTransformAttribute a_gradientTransform;

private:
  FOG_DISABLE_COPY(SvgAbstractGradientElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGABSTRACTGRADIENTELEMENT_P_H
