// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGSTYLEDELEMENT_P_H
#define _FOG_SVG_DOM_SVGSTYLEDELEMENT_P_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Dom/SvgStyleAttribute_p.h>
#include <Fog/Svg/Dom/SvgTransformAttribute_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgStyledElement]
// ============================================================================

// This is not final element, must be overriden.
struct FOG_NO_EXPORT SvgStyledElement : public SvgElement
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  typedef SvgElement base;

  SvgStyledElement(const ManagedString& tagName, uint32_t svgType);
  virtual ~SvgStyledElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  virtual err_t _setAttribute(const ManagedString& name, const String& value);
  virtual err_t _removeAttribute(const ManagedString& name);

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;

  // --------------------------------------------------------------------------
  // [SVG Styles]
  // --------------------------------------------------------------------------

  virtual String getStyle(const String& name) const;
  virtual err_t setStyle(const String& name, const String& value);

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgStyleAttribute a_style;
  SvgTransformAttribute a_transform;

private:
  _FOG_CLASS_NO_COPY(SvgStyledElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGSTYLEDELEMENT_P_H
