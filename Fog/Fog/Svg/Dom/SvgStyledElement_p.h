// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGSTYLEDELEMENT_P_H
#define _FOG_SVG_DOM_SVGSTYLEDELEMENT_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Dom/SvgStyleAttribute_p.h>
#include <Fog/Svg/Dom/SvgTransformAttribute_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgStyledElement]
// ============================================================================

// This is not final element, must be overridden.
struct FOG_NO_EXPORT SvgStyledElement : public SvgElement
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  typedef SvgElement base;

  SvgStyledElement(const ManagedStringW& tagName, uint32_t svgType);
  virtual ~SvgStyledElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  virtual err_t _setAttribute(const ManagedStringW& name, const StringW& value);
  virtual err_t _removeAttribute(const ManagedStringW& name);

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;

  // --------------------------------------------------------------------------
  // [SVG Styles]
  // --------------------------------------------------------------------------

  virtual StringW getStyle(const StringW& name) const;
  virtual err_t setStyle(const StringW& name, const StringW& value);

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgStyleAttribute a_style;
  SvgTransformAttribute a_transform;

private:
  _FOG_NO_COPY(SvgStyledElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGSTYLEDELEMENT_P_H
