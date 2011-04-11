// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGIMAGEELEMENT_P_H
#define _FOG_SVG_DOM_SVGIMAGEELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgCoordAttribute_p.h>
#include <Fog/Svg/Dom/SvgImageLinkAttribute_p.h>
#include <Fog/Svg/Dom/SvgStyledElement_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgImageElement]
// ============================================================================

struct FOG_NO_EXPORT SvgImageElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgImageElement();
  virtual ~SvgImageElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRender(SvgRenderContext* context) const;
  virtual err_t onRenderShape(SvgRenderContext* context) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgImageLinkAttribute a_href;

private:
  FOG_DISABLE_COPY(SvgImageElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGIMAGEELEMENT_P_H
