// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGOBJECT_H
#define _FOG_SVG_DOM_SVGOBJECT_H

// [Dependencies]
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct SvgRenderContext;

// ============================================================================
// [Fog::SvgObject]
// ============================================================================

//! @brief The SvgObject interface.
struct FOG_API SvgObject
{
  virtual err_t onRender(SvgRenderContext* context) const = 0;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGOBJECT_H
