// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Svg/Visit/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgRenderContext]
// ============================================================================

SvgRenderContext::SvgRenderContext(Painter* painter, SvgVisitor* visitor) :
  _painter(painter),
  _visitor(visitor)
{
  _strokeStyle.type = SVG_SOURCE_NONE;
  _fillStyle.type = SVG_SOURCE_COLOR;

  _fillRule = FILL_RULE_EVEN_ODD;
  _opacity = 1.0f;

  _textCursor.reset();

  _painter->save();
  _painter->setCompositingOperator(COMPOSITE_SRC_OVER);
  _painter->setFillRule(FILL_RULE_EVEN_ODD);
  _painter->resetStrokeParams();
}

SvgRenderContext::~SvgRenderContext()
{
  _painter->restore();
}

bool SvgRenderContext::setupFill()
{
  switch (_fillStyle.type)
  {
    case SVG_SOURCE_NONE:
    case SVG_SOURCE_INVALID:
      return false;

    case SVG_SOURCE_COLOR:
      _painter->setSource(_fillStyle.color);
      goto _FillUsed;

    case SVG_SOURCE_URI:
      _painter->setSource(_fillStyle.pattern);
_FillUsed:
      _painter->setOpacity(_fillStyle.opacity * _opacity);
      _painter->setFillRule(_fillRule);
      return true;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return false;
}

bool SvgRenderContext::setupStroke()
{
  switch (_strokeStyle.type)
  {
    case SVG_SOURCE_NONE:
    case SVG_SOURCE_INVALID:
      return false;

    case SVG_SOURCE_COLOR:
      _painter->setSource(_strokeStyle.color);
      goto _StrokeUsed;

    case SVG_SOURCE_URI:
      _painter->setSource(_strokeStyle.pattern);
_StrokeUsed:
      _painter->setOpacity(_strokeStyle.opacity * _opacity);
      _painter->setStrokeParams(_strokeParams);
      return true;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return false;
}

} // Fog namespace
