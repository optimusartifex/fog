// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_RENDER_SVGRENDER_H
#define _FOG_SVG_RENDER_SVGRENDER_H

// [Dependencies]
#include <Fog/Core/Global/Static.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/Line.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/Svg/Global/Constants.h>
#include <Fog/Svg/Tools/SvgCoord.h>

namespace Fog {

//! @addtogroup Fog_Svg_Render
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct SvgDocument;
struct SvgElement;
struct SvgRenderContext;

// ============================================================================
// [Fog::SvgRenderStyle]
// ============================================================================

struct FOG_NO_EXPORT SvgRenderStyle
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SvgRenderStyle() :
    type(SVG_SOURCE_COLOR),
    opacity(1.0f),
    color(Argb32(0xFF000000))
  {
  }

  FOG_INLINE SvgRenderStyle(const SvgRenderStyle& other) :
    type(other.type),
    opacity(other.opacity),
    color(other.color),
    pattern(other.pattern)
  {
  }

  FOG_INLINE ~SvgRenderStyle()
  {
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE SvgRenderStyle& operator=(const SvgRenderStyle& other)
  {
    type    = other.type;
    opacity = other.opacity;
    color   = other.color;
    pattern = other.pattern;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t type;
  float opacity;
  Color color;
  PatternF pattern;
};

// ============================================================================
// [Fog::SvgRenderContext]
// ============================================================================

//! @brief SVG Rendering context.
struct FOG_API SvgRenderContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRenderContext(Painter* painter);
  ~SvgRenderContext();

  // --------------------------------------------------------------------------
  // [Painter]
  // --------------------------------------------------------------------------

  FOG_INLINE Painter* getPainter() const
  {
    return _painter;
  }

  // --------------------------------------------------------------------------
  // [Matrix]
  // --------------------------------------------------------------------------

  FOG_INLINE TransformF getTransform() const
  {
    TransformF tr(UNINITIALIZED);
    _painter->getTransform(tr);
    return tr;
  }

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  FOG_INLINE const SvgRenderStyle& getFillStyle() const
  {
    return _fillStyle;
  }

  FOG_INLINE void setFillStyle(const SvgRenderStyle& style)
  {
    _fillStyle = style;
  }

  FOG_INLINE void setFillOpacity(float opacity)
  {
    _fillStyle.opacity = opacity;
  }

  FOG_INLINE void setFillNone()
  {
    _fillStyle.type = SVG_SOURCE_NONE;
  }

  FOG_INLINE void setFillColor(const Color& color)
  {
    _fillStyle.type = SVG_SOURCE_COLOR;
    _fillStyle.color = color;
  }

  FOG_INLINE void setFillPattern(const PatternF& pattern)
  {
    _fillStyle.type = SVG_SOURCE_URI;
    _fillStyle.pattern = pattern;
  }

  FOG_INLINE void setFillRule(uint32_t fillRule)
  {
    _fillRule = fillRule;
  }

  // --------------------------------------------------------------------------
  // [Line Parameters]
  // --------------------------------------------------------------------------

  FOG_INLINE const SvgRenderStyle& getStrokeStyle()
  {
    return _strokeStyle;
  }

  FOG_INLINE void setStrokeStyle(const SvgRenderStyle& style)
  {
    _strokeStyle = style;
  }

  FOG_INLINE void setStrokeOpacity(float opacity)
  {
    _strokeStyle.opacity = opacity;
  }

  FOG_INLINE void setStrokeNone()
  {
    _strokeStyle.type = SVG_SOURCE_NONE;
  }

  FOG_INLINE void setStrokeColor(const Color& color)
  {
    _strokeStyle.type = SVG_SOURCE_COLOR;
    _strokeStyle.color = color;
  }

  FOG_INLINE void setStrokePattern(const PatternF& pattern)
  {
    _strokeStyle.type = SVG_SOURCE_URI;
    _strokeStyle.pattern = pattern;
  }

  FOG_INLINE void setStrokeParams(const PathStrokerParamsF& strokeParams)
  {
    _strokeParams = strokeParams;
  }

  FOG_INLINE void setLineWidth(float lineWidth)
  {
    _strokeParams.setLineWidth(lineWidth);
  }

  FOG_INLINE void setMiterLimit(float miterLimit)
  {
    _strokeParams.setMiterLimit(miterLimit);
  }

  FOG_INLINE void setLineCaps(int lineCaps)
  {
    _strokeParams.setLineCaps(lineCaps);
  }

  FOG_INLINE void setLineJoin(int lineJoin)
  {
    _strokeParams.setLineJoin(lineJoin);
  }

  // --------------------------------------------------------------------------
  // [DPI]
  // --------------------------------------------------------------------------

  FOG_INLINE float getDpi() const
  {
    return _dpi; 
  }

  void setDpi(float dpi);

  FOG_INLINE float translateCoord(const SvgCoord& coord) const
  {
    return translateCoord(coord.value, coord.unit);
  }

  FOG_INLINE float translateCoord(float val, uint32_t unit) const
  {
    FOG_ASSERT(unit < SVG_UNIT_INVALID);
    return val * _translateCoordData[unit];
  }

  // --------------------------------------------------------------------------
  // [Draw - Setup]
  // --------------------------------------------------------------------------

  bool setupFill();
  bool setupStroke();

  // --------------------------------------------------------------------------
  // [Draw - Line]
  // --------------------------------------------------------------------------

  FOG_INLINE void drawLine(const LineF& line)
  {
    if (setupStroke()) _painter->drawLine(line);
  }

  FOG_INLINE void drawLine(const LineD& line)
  {
    if (setupStroke()) _painter->drawLine(line);
  }

  // --------------------------------------------------------------------------
  // [Draw - Rect]
  // --------------------------------------------------------------------------

  FOG_INLINE void drawRect(const RectF& rect)
  {
    if (setupFill()) _painter->fillRect(rect);
    if (setupStroke()) _painter->drawRect(rect);
  }

  FOG_INLINE void drawRect(const RectD& rect)
  {
    if (setupFill()) _painter->fillRect(rect);
    if (setupStroke()) _painter->drawRect(rect);
  }

  // --------------------------------------------------------------------------
  // [Draw - Round]
  // --------------------------------------------------------------------------

  FOG_INLINE void drawRound(const RoundF& round)
  {
    if (setupFill()) _painter->fillRound(round);
    if (setupStroke()) _painter->drawRound(round);
  }

  FOG_INLINE void drawRound(const RoundD& round)
  {
    if (setupFill()) _painter->fillRound(round);
    if (setupStroke()) _painter->drawRound(round);
  }

  // --------------------------------------------------------------------------
  // [Draw - Ellipse]
  // --------------------------------------------------------------------------

  FOG_INLINE void drawEllipse(const EllipseF& ellipse)
  {
    if (setupFill()) _painter->fillEllipse(ellipse);
    if (setupStroke()) _painter->drawEllipse(ellipse);
  }

  FOG_INLINE void drawEllipse(const EllipseD& ellipse)
  {
    if (setupFill()) _painter->fillEllipse(ellipse);
    if (setupStroke()) _painter->drawEllipse(ellipse);
  }

  // --------------------------------------------------------------------------
  // [Draw - Path]
  // --------------------------------------------------------------------------

  FOG_INLINE void drawPath(const PathF& path)
  {
    if (setupFill()) _painter->fillPath(path);
    if (setupStroke()) _painter->drawPath(path);
  }

  FOG_INLINE void drawPath(const PathD& path)
  {
    if (setupFill()) _painter->fillPath(path);
    if (setupStroke()) _painter->drawPath(path);
  }

  // --------------------------------------------------------------------------
  // [Blit - Image]
  // --------------------------------------------------------------------------

  FOG_INLINE void blitImage(const PointF& pt, const Image& im)
  {
    _painter->blitImage(pt, im);
  }

  FOG_INLINE void blitImage(const PointD& pt, const Image& im)
  {
    _painter->blitImage(pt, im);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Painter* _painter;

  SvgRenderStyle _fillStyle;
  SvgRenderStyle _strokeStyle;
  PathStrokerParamsF _strokeParams;

  uint32_t _fillRule;

  PointF _textCursor;
  Font _font;

  float _dpi;
  float _translateCoordData[SVG_UNIT_INVALID];

private:
  FOG_DISABLE_COPY(SvgRenderContext)
};

// ============================================================================
// [Fog::SvgRenderState]
// ============================================================================

struct FOG_NO_EXPORT SvgRenderState
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SvgRenderState(SvgRenderContext* context) :
    _context(context),
    _savedFill(false),
    _savedStroke(false),
    _savedTransform(false)
  {
  }

  FOG_INLINE ~SvgRenderState()
  {
    if (_savedFill     ) restoreFill();;
    if (_savedStroke   ) restoreStroke();;
    if (_savedTransform) restoreTransform();;
  }

  // --------------------------------------------------------------------------
  // [Save / Restore]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveTransform()
  {
    _context->getPainter()->getTransform(_transform.instance());
    _savedTransform = true;
  }

  FOG_INLINE void restoreTransform()
  {
    _context->getPainter()->setTransform(_transform.instance());
  }

  FOG_INLINE void saveFill()
  {
    _fillStyle.init(_context->_fillStyle);
    _fillRule = _context->_fillRule;

    _savedFill = true;
  }

  FOG_INLINE void restoreFill()
  {
    _context->_fillStyle = _fillStyle.instance();
    _context->_fillRule = _fillRule;

    _fillStyle.destroy();
  }

  FOG_INLINE void saveStroke()
  {
    _strokeStyle.init(_context->_strokeStyle);
    _strokeParams.init(_context->_strokeParams);

    _savedStroke = true;
  }

  FOG_INLINE void restoreStroke()
  {
    _context->_strokeStyle = _strokeStyle.instance();
    _context->_strokeParams = _strokeParams.instance();

    _strokeStyle.destroy();
    _strokeParams.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  SvgRenderContext* _context;

  Static<SvgRenderStyle> _fillStyle;
  Static<SvgRenderStyle> _strokeStyle;

  Static<PathStrokerParamsF> _strokeParams;
  Static<TransformF> _transform;

  uint32_t _fillRule;
  bool _savedFill;
  bool _savedStroke;
  bool _savedTransform;

private:
  FOG_DISABLE_COPY(SvgRenderState)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_RENDER_SVGRENDER_H
