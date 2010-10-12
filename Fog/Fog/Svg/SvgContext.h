// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_SVGCONTEXT_H
#define _FOG_SVG_SVGCONTEXT_H

// [Dependencies]
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Painter.h>
#include <Fog/Graphics/PaintEngine.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Svg/Constants.h>
#include <Fog/Svg/SvgDom.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct SvgContext;
struct SvgContextBackup;
struct SvgDocument;
struct SvgElement;

// ============================================================================
// [Fog::SvgContext]
// ============================================================================

//! @brief SVG Rendering context.
struct FOG_API SvgContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgContext(Painter* painter);
  ~SvgContext();

  // --------------------------------------------------------------------------
  // [Style]
  // --------------------------------------------------------------------------

  struct FOG_HIDDEN Style
  {
    // [Construction / Destruction]

    FOG_INLINE Style() :
      type(SVG_SOURCE_COLOR),
      opacity(1.0),
      color(0xFF000000)
    {
    }

    FOG_INLINE Style(const Style& other) :
      type(other.type),
      opacity(other.opacity),
      color(other.color),
      pattern(other.pattern)
    {
    }

    FOG_INLINE ~Style()
    {
    }

    // [Operator Overload]

    FOG_INLINE Style& operator=(const Style& other)
    {
      type    = other.type;
      opacity = other.opacity;
      color   = other.color;
      pattern = other.pattern;
      return *this;
    }

    // [Members]

    uint32_t type;
    float opacity;
    ArgbI color;
    Pattern pattern;
  };

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

  FOG_INLINE TransformD getTransform() const
  {
    return _painter->getTransform();
  }

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  FOG_INLINE const Style& getFillStyle() const
  {
    return _fillStyle;
  }

  FOG_INLINE void setFillStyle(const Style& style)
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

  FOG_INLINE void setFillColor(ArgbI color)
  {
    _fillStyle.type = SVG_SOURCE_COLOR;
    _fillStyle.color = color;
  }

  FOG_INLINE void setFillPattern(const Pattern& pattern)
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

  FOG_INLINE const Style& getStrokeStyle()
  {
    return _lineStyle;
  }

  FOG_INLINE void setStrokeStyle(const Style& style)
  {
    _lineStyle = style;
  }

  FOG_INLINE void setStrokeOpacity(float opacity)
  {
    _lineStyle.opacity = opacity;
  }

  FOG_INLINE void setStrokeNone()
  {
    _lineStyle.type = SVG_SOURCE_NONE;
  }

  FOG_INLINE void setStrokeColor(ArgbI color)
  {
    _lineStyle.type = SVG_SOURCE_COLOR;
    _lineStyle.color = color;
  }

  FOG_INLINE void setStrokePattern(const Pattern& pattern)
  {
    _lineStyle.type = SVG_SOURCE_URI;
    _lineStyle.pattern = pattern;
  }

  FOG_INLINE void setStrokeParams(const PathStrokeParams& strokeParams)
  {
    _strokeParams = strokeParams;
  }

  FOG_INLINE void setLineWidth(double lineWidth)
  {
    _strokeParams.setLineWidth(lineWidth);
  }

  FOG_INLINE void setLineCaps(int lineCaps)
  {
    _strokeParams.setLineCaps(lineCaps);
  }

  FOG_INLINE void setLineJoin(int lineJoin)
  {
    _strokeParams.setLineJoin(lineJoin);
  }

  FOG_INLINE void setMiterLimit(double miterLimit)
  {
    _strokeParams.setMiterLimit(miterLimit);
  }

  // --------------------------------------------------------------------------
  // [DPI]
  // --------------------------------------------------------------------------

  FOG_INLINE float getDpi() const
  {
    return _dpi; 
  }

  void setDpi(float dpi);

  FOG_INLINE float translateCoord(float val, uint32_t unit) const
  {
    FOG_ASSERT(unit < SVG_UNIT_INVALID);
    return val * _translateCoordData[unit];
  }

  FOG_INLINE double translateCoord(double val, uint32_t unit) const
  {
    FOG_ASSERT(unit < SVG_UNIT_INVALID);
    return val * (double)_translateCoordData[unit];
  }

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  FOG_INLINE void setupFillStyle()
  {
    if (_fillStyle.type == SVG_SOURCE_COLOR)
      _painter->setSource(_fillStyle.color);
    else
      _painter->setSource(_fillStyle.pattern);

    _painter->setAlpha(_fillStyle.opacity);
    _painter->setFillRule(_fillRule);
  }

  FOG_INLINE void setupStrokeStyle()
  {
    if (_lineStyle.type == SVG_SOURCE_COLOR)
      _painter->setSource(_lineStyle.color);
    else
      _painter->setSource(_lineStyle.pattern);

    _painter->setAlpha(_lineStyle.opacity);
    _painter->setStrokeParams(_strokeParams);
  }

  void drawEllipse(const PointD& cp, const PointD& r);
  void drawLine(const PointD& p1, const PointD& p2);
  void drawRect(const RectD& rect);
  void drawRound(const RectD& rect, const PointD& r);
  void drawPath(const PathD& path);

  void drawImage(const PointD& pt, const Image& im);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Painter* _painter;

  Style _fillStyle;
  Style _lineStyle;
  PathStrokeParams _strokeParams;
  uint32_t _fillRule;

  float _dpi;
  float _translateCoordData[SVG_UNIT_INVALID];

private:
  friend struct SvgContextBackup;

  FOG_DISABLE_COPY(SvgContext)
};

// ============================================================================
// [Fog::SvgContextBackup]
// ============================================================================

struct FOG_HIDDEN SvgContextBackup
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SvgContextBackup() :
    _context(NULL),
    _transform(DONT_INITIALIZE),
    _transformBackup(false)
  {
  }
  
  FOG_INLINE SvgContextBackup(SvgContext* context) :
    _transform(DONT_INITIALIZE)
  {
    init(context);
  }

  FOG_INLINE ~SvgContextBackup()
  {
    if (_context) destroy();
  }

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(SvgContext* context)
  {
    _context = context;

    _fillStyle.init(_context->_fillStyle);
    _fillRule = _context->_fillRule;

    _lineStyle.init(_context->_lineStyle);
    _strokeParams.init(_context->_strokeParams);

    _transformBackup = false;
  }

  FOG_INLINE void destroy()
  {
    _context->_fillStyle = _fillStyle.instance();
    _context->_fillRule = _fillRule;

    _context->_lineStyle = _lineStyle.instance();
    _context->_strokeParams = _strokeParams.instance();

    if (_transformBackup) _context->_painter->setTransform(_transform);

    _fillStyle.destroy();
    _lineStyle.destroy();
    _strokeParams.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  SvgContext* _context;
  TransformD _transform;

  Static<SvgContext::Style> _fillStyle;
  uint32_t _fillRule;

  Static<SvgContext::Style> _lineStyle;
  Static<PathStrokeParams> _strokeParams;

  bool _transformBackup;

private:
  friend struct SvgContext;

  FOG_DISABLE_COPY(SvgContextBackup)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_SVGCONTEXT_H
