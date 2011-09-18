// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_VISIT_SVGRENDER_H
#define _FOG_SVG_VISIT_SVGRENDER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

//! @addtogroup Fog_Svg_Visit
//! @{

// ============================================================================
// [Fog::SvgRender]
// ============================================================================

//! @brief SVG Render visitor.
struct FOG_API SvgRender : public SvgVisitor
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRender(Painter* painter);
  virtual ~SvgRender();

  // --------------------------------------------------------------------------
  // [Painter]
  // --------------------------------------------------------------------------

  FOG_INLINE Painter* getPainter() const
  {
    return _painter;
  }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t onShape(SvgElement* obj, const ShapeF& shape);
  virtual err_t onPath(SvgElement* obj, const PathF& path);
  virtual err_t onImage(SvgElement* obj, const PointF& pt, const Image& image);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Painter* _painter;

private:
  _FOG_NO_COPY(SvgRender)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_VISIT_SVGRENDER_H
