// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/Svg/Dom/SvgDocument.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Visit/SvgMeasure.h>

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
    if (shape.getBoundingBox(b, _transform) == ERR_OK) boundWith(b);
  }

  if (_strokeStyle.isPaintable())
  {
    PathStrokerF stroker(_strokeParams);

    _pathTmp.clear();
    stroker.strokeShape(_pathTmp, shape.getType(), shape.getData());
    if (_pathTmp.getBoundingBox(b, _transform) == ERR_OK) boundWith(b);
  }

  return ERR_OK;
}

err_t SvgMeasure::onPath(SvgElement* obj, const PathF& path)
{
  BoxF b;

  if (_fillStyle.isPaintable())
  {
    if (path.getBoundingBox(b, _transform) == ERR_OK) boundWith(b);
  }

  if (_strokeStyle.isPaintable())
  {
    PathStrokerF stroker(_strokeParams);

    _pathTmp.clear();
    stroker.strokePath(_pathTmp, path);
    if (_pathTmp.getBoundingBox(b, _transform) == ERR_OK) boundWith(b);
  }

  return ERR_OK;
}

err_t SvgMeasure::onPath(SvgElement* obj, const PathD& path)
{
  // TODO: Will be removed...
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
