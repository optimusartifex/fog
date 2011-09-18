// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_VISIT_SVGMEASURE_H
#define _FOG_SVG_VISIT_SVGMEASURE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

namespace Fog {

//! @addtogroup Fog_Svg_Visit
//! @{

// ============================================================================
// [Fog::SvgMeasure]
// ============================================================================

//! @brief SVG measure.
struct FOG_API SvgMeasure : public SvgVisitor
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgMeasure();
  virtual ~SvgMeasure();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxF& getBoundingBox() const { return _bbox; }
  FOG_INLINE bool hasBoundingBox() const { return _hasBBox; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _bbox.reset();
    _hasBBox = false;
  }

  // --------------------------------------------------------------------------
  // [Bound]
  // --------------------------------------------------------------------------

  FOG_INLINE void boundWith(const BoxF& b)
  {
    if (_hasBBox)
      BoxF::bound(_bbox, _bbox, b);
    else
      _bbox = b;
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

  BoxF _bbox;
  bool _hasBBox;

  PathF _pathTmp;

private:
  _FOG_NO_COPY(SvgMeasure)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_VISIT_SVGMEASURE_H
