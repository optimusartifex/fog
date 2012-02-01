// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Dom/Dom.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Svg/SvgContext.h>
#include <Fog/G2d/Svg/SvgDom.h>

namespace Fog {

// ============================================================================
// [Fog::SvgContext - Construction / Destruction]
// ============================================================================

SvgContext::SvgContext(SvgContextExtension* contextExtension)
{
  _contextType = SVG_CONTEXT_NONE;
  _transformDirty = true;

  _contextExtension = contextExtension;
  _strokeSource.type = SVG_SOURCE_NONE;
  _fillSource.type = SVG_SOURCE_COLOR;

  _fillRule = FILL_RULE_EVEN_ODD;
  _opacity = 1.0f;

  _textCursor.reset();

  if (_contextExtension != NULL)
    _contextExtension->attach(this);
}

SvgContext::~SvgContext()
{
  if (_contextExtension != NULL)
    _contextExtension->release();
}

// ============================================================================
// [Fog::SvgContext - MoveTo]
// ============================================================================

err_t SvgContext::advance(SvgElement* obj)
{
  FOG_ASSERT(obj != NULL);

  List<SvgElement*> elements;
  DomDocument* doc = obj->getOwnerDocument();

  for (;;)
  {
    DomNode* parent = obj->getParentNode();
    elements.append(obj);

    if (parent == doc || parent == NULL)
      break;

    if (!parent->isObjectModelAndNodeType(DOM_OBJECT_MODEL_SVG, DOM_NODE_TYPE_ELEMENT))
      return ERR_RT_INVALID_STATE;

    obj = static_cast<SvgElement*>(parent);
  }

  ListReverseIterator<SvgElement*> it(elements);
  while (it.isValid())
  {
    obj = it.getItem();
    FOG_RETURN_ON_ERROR(obj->onPrepare(this, NULL));
    it.next();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgContext - Interface]
// ============================================================================

err_t SvgContext::onVisit(SvgElement* obj)
{
  SvgContextGState state(this);

  FOG_RETURN_ON_ERROR(obj->onPrepare(this, &state));
  FOG_RETURN_ON_ERROR(obj->onProcess(this));

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgContextExtension]
// ============================================================================

SvgContextExtension::SvgContextExtension(SvgContextExtension* contextExtension)
{
}

SvgContextExtension::~SvgContextExtension()
{
}

// ============================================================================
// [Fog::SvgRenderContext - Helpers]
// ============================================================================

static FOG_INLINE bool SvgRenderContext_setupFill(SvgRenderContext* visitor, SvgElement* obj)
{
  Painter* p = visitor->_painter;

  switch (visitor->_fillSource.type)
  {
    case SVG_SOURCE_NONE:
    case SVG_SOURCE_INVALID:
    {
      break;
    }

    case SVG_SOURCE_COLOR:
    {
      p->setSource(visitor->_fillSource.color);
      goto _FillUsed;
    }

    case SVG_SOURCE_URI:
    {
      Pattern pattern;
      visitor->_fillSource.uriRef->onPattern(visitor, obj, &pattern);
      p->setSource(pattern);
      goto _FillUsed;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }
  return false;

_FillUsed:
  p->setOpacity(visitor->_fillSource.opacity * visitor->_opacity);
  p->setFillRule(visitor->_fillRule);
  return true;
}

static FOG_INLINE bool SvgRenderContext_setupStroke(SvgRenderContext* visitor, SvgElement* obj)
{
  Painter* p = visitor->_painter;

  switch (visitor->_strokeSource.type)
  {
    case SVG_SOURCE_NONE:
    case SVG_SOURCE_INVALID:
    {
      break;
    }

    case SVG_SOURCE_COLOR:
    {
      p->setSource(visitor->_strokeSource.color);
      goto _StrokeUsed;
    }

    case SVG_SOURCE_URI:
    {
      Pattern pattern;

      visitor->_strokeSource.uriRef->onPattern(visitor, obj, &pattern);
      p->setSource(pattern);
      goto _StrokeUsed;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return false;

_StrokeUsed:
  p->setOpacity(visitor->_strokeSource.opacity * visitor->_opacity);
  p->setStrokeParams(visitor->_strokeParams);
  return true;
}

// ============================================================================
// [Fog::SvgRenderContext - Construction / Destruction]
// ============================================================================

SvgRenderContext::SvgRenderContext(SvgContextExtension* contextExtension) :
  SvgContext(contextExtension),
  _painter(NULL)
{
  _contextType = SVG_CONTEXT_RENDER;
}

SvgRenderContext::~SvgRenderContext()
{
}

// ============================================================================
// [Fog::SvgRenderContext - Painter]
// ============================================================================

void SvgRenderContext::initPainter()
{
  _painter->setCompositingOperator(COMPOSITE_SRC_OVER);
  _painter->setFillRule(FILL_RULE_EVEN_ODD);
  _painter->resetStrokeParams();
}

// ============================================================================
// [Fog::SvgRenderContext - Interface]
// ============================================================================

err_t SvgRenderContext::onShape(SvgElement* obj, const ShapeF& shape)
{
  _painter->save();
  _painter->transform(_transform);

  if (shape.isClosed() && SvgRenderContext_setupFill(this, obj))
    _painter->fillShape(shape);

  if (SvgRenderContext_setupStroke(this, obj))
    _painter->drawShape(shape);

  _painter->restore();
  return ERR_OK;
}

err_t SvgRenderContext::onImage(SvgElement* obj, const PointF& pt, const Image& image)
{
  _painter->save();
  _painter->transform(_transform);

  _painter->blitImage(pt, image);

  _painter->restore();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgMeasureContext - Construction / Destruction]
// ============================================================================

SvgMeasureContext::SvgMeasureContext(SvgContextExtension* contextExtension) :
  SvgContext(contextExtension)
{
  _contextType = SVG_CONTEXT_MEASURE;
  reset();
}

SvgMeasureContext::~SvgMeasureContext()
{
}

// ============================================================================
// [Fog::SvgMeasureContext - Interface]
// ============================================================================

err_t SvgMeasureContext::onShape(SvgElement* obj, const ShapeF& shape)
{
  BoxF b;

  if (_fillSource.isPaintable())
  {
    if (shape.getBoundingBox(b, _transform) == ERR_OK)
      boundWith(b);
  }

  if (_strokeSource.isPaintable())
  {
    PathStrokerF stroker(_strokeParams);

    _pathTmp.clear();
    stroker.strokeShape(_pathTmp, shape);

    if (_pathTmp.getBoundingBox(b, _transform) == ERR_OK)
      boundWith(b);
  }

  return ERR_OK;
}

err_t SvgMeasureContext::onImage(SvgElement* obj, const PointF& pt, const Image& image)
{
  BoxF b(pt.x, pt.y, pt.x + (float)image.getWidth(), pt.y + (float)image.getHeight());
  _transform.mapBox(b, b);
  boundWith(b);

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgHitTestContext - Construction / Destruction]
// ============================================================================

SvgHitTestContext::SvgHitTestContext(SvgContextExtension* contextExtension) :
  SvgContext(contextExtension),
  _invTransform(),
  _invPoint(0.0f, 0.0f),
  _point(0.0f, 0.0f)
{
  _contextType = SVG_CONTEXT_HIT_TEST;
}

SvgHitTestContext::~SvgHitTestContext()
{
}

// ============================================================================
// [Fog::SvgHitTestContext - Interface]
// ============================================================================

err_t SvgHitTestContext::onShape(SvgElement* obj, const ShapeF& shape)
{
  if (_transformDirty)
  {
    if (!TransformF::invert(_invTransform, _transform))
      return ERR_OK;

    _invTransform.mapPoint(_invPoint, _point);
    _transformDirty = false;
  }

  if (_fillSource.isPaintable())
  {
    if (shape.hitTest(_invPoint))
      return _result.append(obj);
  }

  if (_strokeSource.isPaintable())
  {
    PathStrokerF stroker(_strokeParams);
    _pathTmp.clear();

    err_t err = stroker.strokeShape(_pathTmp, shape);
    if (err == ERR_GEOMETRY_NONE)
      return ERR_OK;

    if (_pathTmp.hitTest(_invPoint, FILL_RULE_NON_ZERO))
      return _result.append(obj);
  }

  return ERR_OK;
}

err_t SvgHitTestContext::onImage(SvgElement* obj, const PointF& pt, const Image& image)
{
  if (_transformDirty)
  {
    if (!TransformF::invert(_invTransform, _transform))
      return ERR_OK;

    _invTransform.mapPoint(_invPoint, _point);
    _transformDirty = false;
  }

  RectF rect(pt.x, pt.y, (float)image.getWidth(), (float)image.getHeight());
  if (rect.hitTest(_invPoint))
    _result.append(obj);

  return ERR_OK;
}

} // Fog namespace
