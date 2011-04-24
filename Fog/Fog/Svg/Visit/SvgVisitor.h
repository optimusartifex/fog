// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_VISIT_SVGVISITOR_H
#define _FOG_SVG_VISIT_SVGVISITOR_H

// [Dependencies]
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Svg_Visit
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct XmlElement;

// ============================================================================
// [Fog::SvgVisitor]
// ============================================================================

//! @brief SVG visitor.
//!
//! SVG visitor is interface which can be used to do customized SVG rendering
//! and hit-testing. If used, it can be used to inform visitor which SVG element
//! will be visited (rendered or hit-tested) and possibility to control it
//! (enable / disable).
struct FOG_API SvgVisitor
{
  //! @brief Called to get whether the xml element should be traversed or not.
  virtual bool canVisit(XmlElement* element) = 0;
  //! @brief Called before the @a element is visited.
  virtual void onBegin(XmlElement* element) = 0;
  //! @brief Called after the @a element is visited.
  virtual void onEnd(XmlElement* element) = 0;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_VISIT_SVGVISITOR_H
