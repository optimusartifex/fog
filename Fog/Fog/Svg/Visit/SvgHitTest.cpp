// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/Svg/Dom/SvgDocument.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Visit/SvgHitTest.h>

namespace Fog {

// ============================================================================
// [Fog::SvgHitTest - Construction / Destruction]
// ============================================================================

SvgHitTest::SvgHitTest(const PointF& pt)
  : _point(pt)
{
}

SvgHitTest::~SvgHitTest()
{
}

// ============================================================================
// [Fog::SvgHitTest - Interface]
// ============================================================================

err_t SvgHitTest::onShape(SvgElement* obj, const ShapeF& shape)
{
  if (_transformDirty)
  {
    if (!TransformF::invert(_invTransform, _transform)) return ERR_OK;
    _invTransform.mapPoint(_invPoint, _point);
    _transformDirty = false;
  }

  if (_fillStyle.isPaintable())
  {
    if (shape.hitTest(_invPoint)) return _result.append(obj);
  }

  if (_strokeStyle.isPaintable())
  {
    PathStrokerF stroker(_strokeParams);
    _pathTmp.clear();
    stroker.strokeShape(_pathTmp, shape.getType(), shape.getData());
    if (_pathTmp.hitTest(_invPoint, FILL_RULE_NON_ZERO)) return _result.append(obj);
  }

  return ERR_OK;
}

err_t SvgHitTest::onPath(SvgElement* obj, const PathF& path)
{
  if (_transformDirty)
  {
    if (!TransformF::invert(_invTransform, _transform)) return ERR_OK;
    _invTransform.mapPoint(_invPoint, _point);
    _transformDirty = false;
  }

  if (_fillStyle.isPaintable())
  {
    if (path.hitTest(_invPoint, _fillRule)) return _result.append(obj);
  }

  if (_strokeStyle.isPaintable())
  {
    PathStrokerF stroker(_strokeParams);
    _pathTmp.clear();
    stroker.strokePath(_pathTmp, path);
    if (_pathTmp.hitTest(_invPoint, FILL_RULE_NON_ZERO)) return _result.append(obj);
  }

  return ERR_OK;
}

err_t SvgHitTest::onImage(SvgElement* obj, const PointF& pt, const Image& image)
{
  if (_transformDirty)
  {
    if (!TransformF::invert(_invTransform, _transform)) return ERR_OK;
    _invTransform.mapPoint(_invPoint, _point);
    _transformDirty = false;
  }

  RectF rect(pt.x, pt.y, (float)image.getWidth(), (float)image.getHeight());
  if (rect.hitTest(_invPoint)) _result.append(obj);
  return ERR_OK;
}

} // Fog namespace
