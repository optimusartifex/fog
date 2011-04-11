// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGGELEMENT_P_H
#define _FOG_SVG_DOM_SVGGELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgStyledElement_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgGElement]
// ============================================================================

struct FOG_NO_EXPORT SvgGElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgGElement();
  virtual ~SvgGElement();

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRenderShape(SvgRenderContext* context) const;

private:
  FOG_DISABLE_COPY(SvgGElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGGELEMENT_P_H
