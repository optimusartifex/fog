// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGTSPANELEMENT_P_H
#define _FOG_SVG_DOM_SVGTSPANELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgCoordAttribute_p.h>
#include <Fog/Svg/Dom/SvgEnumAttribute_p.h>
#include <Fog/Svg/Dom/SvgStyledElement_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgStopElement]
// ============================================================================

struct FOG_NO_EXPORT SvgTSpanElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgTSpanElement();
  virtual ~SvgTSpanElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRenderShape(SvgRenderContext* context) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;

  // TODO: Ignored.
  SvgCoordAttribute a_dx; 
  // TODO: Ignored.
  SvgCoordAttribute a_dy;
  // TODO: Ignored.
  SvgCoordAttribute a_textLength;
  // TODO: Ignored.
  SvgEnumAttribute a_lengthAdjust;

private:
  FOG_DISABLE_COPY(SvgTSpanElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGTSPANELEMENT_P_H
