// [Fog-Svg Library - Public API]
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
#include <Fog/Graphics/Stroker.h>
#include <Fog/Svg/Constants.h>
#include <Fog/Svg/SvgDom.h>

namespace Fog {

//! @addtogroup Fog_Svg
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
  // [Construction / Destruction]

  SvgContext(Painter* painter);
  ~SvgContext();

  // [Style]

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
    Argb color;
    Pattern pattern;
  };

  // [Painter]

  FOG_INLINE Painter* getPainter() const
  {
    return _painter;
  }

  // [Matrix]

  FOG_INLINE DoubleMatrix getMatrix() const
  {
    return _painter->getMatrix();
  }

  // [Fill Parameters]

  FOG_INLINE const Style& getFillStyle() const
  {
    return _fillStyle;
  }

  FOG_INLINE void setFillStyle(const Style& style)
  {
    _fillStyle = style;
  }

  FOG_INLINE void setFillNone()
  {
    _fillStyle.type = SVG_SOURCE_NONE;
  }

  FOG_INLINE void setFillColor(Argb color)
  {
    _fillStyle.type = SVG_SOURCE_COLOR;
    _fillStyle.color = color;
  }

  FOG_INLINE void setFillPattern(const Pattern& pattern)
  {
    _fillStyle.type = SVG_SOURCE_URI;
    _fillStyle.pattern = pattern;
  }

  FOG_INLINE void setFillMode(int mode)
  {
    _fillMode = mode;
  }

  // [Line Parameters]

  FOG_INLINE const Style& getStrokeStyle()
  {
    return _lineStyle;
  }

  FOG_INLINE void setStrokeStyle(const Style& style)
  {
    _lineStyle = style;
  }

  FOG_INLINE void setStrokeNone()
  {
    _lineStyle.type = SVG_SOURCE_NONE;
  }

  FOG_INLINE void setStrokeColor(Argb color)
  {
    _lineStyle.type = SVG_SOURCE_COLOR;
    _lineStyle.color = color;
  }

  FOG_INLINE void setStrokePattern(const Pattern& pattern)
  {
    _lineStyle.type = SVG_SOURCE_URI;
    _lineStyle.pattern = pattern;
  }

  FOG_INLINE void setStrokeParams(const StrokeParams& strokeParams)
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

  // [DPI]

  FOG_INLINE double getDpi() const
  {
    return _dpi; 
  }

  void setDpi(double dpi);

  FOG_INLINE double translateCoord(double val, uint32_t unit) const
  {
    FOG_ASSERT(unit < SVG_UNIT_INVALID);
    return val * _translateCoordData[unit];
  }

  // [Draw]

  FOG_INLINE void setupFillStyle()
  {
    if (_fillStyle.type == SVG_SOURCE_COLOR)
      _painter->setSource(_fillStyle.color);
    else
      _painter->setSource(_fillStyle.pattern);

    _painter->setFillMode(_fillMode);
  }

  FOG_INLINE void setupStrokeStyle()
  {
    if (_lineStyle.type == SVG_SOURCE_COLOR)
      _painter->setSource(_lineStyle.color);
    else
      _painter->setSource(_lineStyle.pattern);

    _painter->setStrokeParams(_strokeParams);
  }

  void drawEllipse(const DoublePoint& cp, const DoublePoint& r);
  void drawLine(const DoublePoint& p1, const DoublePoint& p2);
  void drawRect(const DoubleRect& rect);
  void drawRound(const DoubleRect& rect, const DoublePoint& r);
  void drawPath(const DoublePath& path);

  void blitImage(const DoublePoint& pt, const Image& im);

  // [Members]

  Painter* _painter;

  Style _fillStyle;
  Style _lineStyle;
  StrokeParams _strokeParams;
  int _fillMode;

  double _dpi;
  double _translateCoordData[SVG_UNIT_INVALID];

private:
  friend struct SvgContextBackup;

  FOG_DISABLE_COPY(SvgContext)
};

// ============================================================================
// [Fog::SvgContextBackup]
// ============================================================================

struct FOG_HIDDEN SvgContextBackup
{
  FOG_INLINE SvgContextBackup() :
    _context(NULL),
    _matrix(DONT_INITIALIZE),
    _matrixBackup(false)
  {
  }
  
  FOG_INLINE SvgContextBackup(SvgContext* context) :
    _matrix(DONT_INITIALIZE)
  {
    init(context);
  }

  FOG_INLINE ~SvgContextBackup()
  {
    if (_context) destroy();
  }

  FOG_INLINE void init(SvgContext* context)
  {
    _context = context;

    _fillStyle.init(_context->_fillStyle);
    _fillMode = _context->_fillMode;

    _lineStyle.init(_context->_lineStyle);
    _strokeParams.init(_context->_strokeParams);

    _matrixBackup = false;
  }

  FOG_INLINE void destroy()
  {
    _context->_fillStyle = _fillStyle.instance();
    _context->_fillMode = _fillMode;

    _context->_lineStyle = _lineStyle.instance();
    _context->_strokeParams = _strokeParams.instance();

    if (_matrixBackup) _context->_painter->setMatrix(_matrix);

    _fillStyle.destroy();
    _lineStyle.destroy();
    _strokeParams.destroy();
  }

  SvgContext* _context;
  DoubleMatrix _matrix;

  Static<SvgContext::Style> _fillStyle;
  uint32_t _fillMode;

  Static<SvgContext::Style> _lineStyle;
  Static<StrokeParams> _strokeParams;

  bool _matrixBackup;

private:
  friend struct SvgContext;

  FOG_DISABLE_COPY(SvgContextBackup)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_SVGCONTEXT_H
