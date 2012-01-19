// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Svg/SvgDocument.h>
#include <Fog/G2d/Svg/SvgElement.h>
#include <Fog/G2d/Svg/SvgMeasure.h>

namespace Fog {

// ============================================================================
// [Fog::SvgMeasure - Construction / Destruction]
// ============================================================================

SvgMeasure::SvgMeasure()
{
  reset();
}

SvgMeasure::~SvgMeasure()
{
}

// ============================================================================
// [Fog::SvgMeasure - Interface]
// ============================================================================

err_t SvgMeasure::onShape(SvgElement* obj, const ShapeF& shape)
{
  BoxF b;

  if (_fillStyle.isPaintable())
  {
    if (shape.getBoundingBox(b, _transform) == ERR_OK)
      boundWith(b);
  }

  if (_strokeStyle.isPaintable())
  {
    PathStrokerF stroker(_strokeParams);

    _pathTmp.clear();
    stroker.strokeShape(_pathTmp, shape);
    if (_pathTmp.getBoundingBox(b, _transform) == ERR_OK) boundWith(b);
  }

  return ERR_OK;
}

err_t SvgMeasure::onImage(SvgElement* obj, const PointF& pt, const Image& image)
{
  BoxF b(pt.x, pt.y, pt.x + (float)image.getWidth(), pt.y + (float)image.getHeight());
  _transform.mapBox(b, b);
  boundWith(b);

  return ERR_OK;
}

} // Fog namespace
