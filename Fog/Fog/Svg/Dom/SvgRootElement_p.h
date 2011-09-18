// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGROOTELEMENT_P_H
#define _FOG_SVG_DOM_SVGROOTELEMENT_P_H

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

struct FOG_NO_EXPORT SvgRootElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRootElement();
  virtual ~SvgRootElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;
  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  SizeF getRootSize() const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgViewBoxAttribute a_viewBox;

  // TODO: Rendering Model.
  // TODO: ViewBox.
  // TODO: PreserveAspectRatio.

private:
  _FOG_NO_COPY(SvgRootElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGROOTELEMENT_P_H
