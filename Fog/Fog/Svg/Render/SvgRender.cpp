// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Svg/Render/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgRenderContext]
// ============================================================================

SvgRenderContext::SvgRenderContext(Painter* painter) :
  _painter(painter)
{
  _strokeStyle.type = SVG_SOURCE_NONE;
  _fillStyle.type = SVG_SOURCE_COLOR;

  _fillRule = FILL_RULE_EVEN_ODD;
  _opacity = 1.0f;

  _textCursor.reset();
  setDpi(90.0f);

  _painter->save();
  _painter->setCompositingOperator(COMPOSITE_SRC_OVER);
  _painter->setFillRule(FILL_RULE_EVEN_ODD);
  _painter->resetStrokeParams();
}

SvgRenderContext::~SvgRenderContext()
{
  _painter->restore();
}

void SvgRenderContext::setDpi(float dpi)
{
  _dpi = dpi;

  _translateCoordData[SVG_UNIT_NONE] = 1.0f;
  _translateCoordData[SVG_UNIT_PERCENT] = 1.0f;

  // SVG TODO: Em and Ex units.
  _translateCoordData[SVG_UNIT_CM] = dpi * 0.3937007777777778f;
  _translateCoordData[SVG_UNIT_EM] = 1.0f;
  _translateCoordData[SVG_UNIT_EX] = 1.0f;
  _translateCoordData[SVG_UNIT_IN] = dpi;
  _translateCoordData[SVG_UNIT_MM] = dpi * 0.0393700777777778f;
  _translateCoordData[SVG_UNIT_PC] = dpi * 0.1666666666666667f;
  _translateCoordData[SVG_UNIT_PT] = dpi * 0.0138888888888889f;
  _translateCoordData[SVG_UNIT_PX] = 1.0f;
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
