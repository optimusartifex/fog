// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SVG_SVGHITTEST_H
#define _FOG_G2D_SVG_SVGHITTEST_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Svg/SvgVisitor.h>

namespace Fog {

//! @addtogroup Fog_G2d_Svg
//! @{

// ============================================================================
// [Fog::SvgHitTest]
// ============================================================================

//! @brief SVG Hit-Test visitor.
struct FOG_API SvgHitTest : public SvgVisitor
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgHitTest(const PointF& pt = PointF(0.0f, 0.0f));
  virtual ~SvgHitTest();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getPoint() const { return _point; }
  FOG_INLINE void setPoint(const PointF& pt) { _point = pt; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _result.reset();
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

  TransformF _invTransform;
  PointF _invPoint;

  PointF _point;
  List<SvgElement*> _result;

  PathF _pathTmp;

private:
  _FOG_NO_COPY(SvgHitTest)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SVG_SVGHITTEST_H
