// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGDEFSELEMENT_P_H
#define _FOG_SVG_DOM_SVGDEFSELEMENT_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Svg/Dom/SvgElement.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgDefsElement]
// ============================================================================

struct FOG_NO_EXPORT SvgDefsElement : public SvgElement
{
  typedef SvgElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgDefsElement();
  virtual ~SvgDefsElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;

private:
  _FOG_NO_COPY(SvgDefsElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGDEFSELEMENT_P_H
