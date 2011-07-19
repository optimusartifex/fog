// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGVIEWELEMENT_P_H
#define _FOG_SVG_DOM_SVGVIEWELEMENT_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Svg/Dom/SvgCoordAttribute_p.h>
#include <Fog/Svg/Dom/SvgStyledElement_p.h>
#include <Fog/Svg/Dom/SvgViewBoxAttribute_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgRootElement]
// ============================================================================

struct FOG_NO_EXPORT SvgViewElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgViewElement();
  virtual ~SvgViewElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgViewBoxAttribute a_viewBox;
  // TODO: PreserveAspectRatio.

private:
  _FOG_CLASS_NO_COPY(SvgViewElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGVIEWELEMENT_P_H
