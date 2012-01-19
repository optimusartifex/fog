// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SVG_SVGVISITOR_H
#define _FOG_G2D_SVG_SVGVISITOR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/Coord.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/Line.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Text/Font.h>

namespace Fog {

//! @addtogroup Fog_G2d_Svg
//! @{

// ============================================================================
// [Fog::SvgStyle]
// ============================================================================

struct FOG_NO_EXPORT SvgStyle
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SvgStyle() :
    type(SVG_SOURCE_COLOR),
    opacity(1.0f),
    color(Argb32(0xFF000000))
  {
  }

  FOG_INLINE SvgStyle(const SvgStyle& other) :
    type(other.type),
    opacity(other.opacity),
    color(other.color),
    pattern(other.pattern)
  {
  }

  FOG_INLINE ~SvgStyle()
  {
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isPaintable() const
  {
    return type == SVG_SOURCE_COLOR || type == SVG_SOURCE_URI;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE SvgStyle& operator=(const SvgStyle& other)
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
  Pattern pattern;
};

// ============================================================================
// [Fog::SvgVisitor]
// ============================================================================

//! @brief SVG Visitor.
struct FOG_API SvgVisitor
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgVisitor();
  virtual ~SvgVisitor();

  // --------------------------------------------------------------------------
  // [Visitor Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getVisitorType() const
  {
    return _visitorType;
  }

  // --------------------------------------------------------------------------
  // [Advance]
  // --------------------------------------------------------------------------

  err_t advance(SvgElement* obj);

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE const TransformF& getTransform() const
  {
    return _transform;
  }

  FOG_INLINE void setTransform(const TransformF& tr)
  {
    _transformDirty = true;
    _transform = tr;
  }

  FOG_INLINE void translate(const PointF& pt)
  {
    _transformDirty = true;
    _transform.translate(pt, MATRIX_ORDER_PREPEND);
  }

  FOG_INLINE void transform(const TransformF& tr)
  {
    _transformDirty = true;
    _transform.transform(tr, MATRIX_ORDER_PREPEND);
  }

  // --------------------------------------------------------------------------
  // [Global Parameters]
  // --------------------------------------------------------------------------

  FOG_INLINE float getOpacity() { return _opacity; }
  FOG_INLINE void setOpacity(float opacity) { _opacity = opacity; }

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  FOG_INLINE const SvgStyle& getFillStyle() const
  {
    return _fillStyle;
  }

  FOG_INLINE void setFillStyle(const SvgStyle& style)
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
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  FOG_INLINE const SvgStyle& getStrokeStyle()
  {
    return _strokeStyle;
  }

  FOG_INLINE void setStrokeStyle(const SvgStyle& style)
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

  FOG_INLINE void setStrokePattern(const Pattern& pattern)
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
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t onVisit(SvgElement* obj);

  virtual err_t onShape(SvgElement* obj, const ShapeF& shape) = 0;
  virtual err_t onImage(SvgElement* obj, const PointF& pt, const Image& image) = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _visitorType;

  uint32_t _transformDirty;
  TransformF _transform;

  SvgStyle _fillStyle;
  SvgStyle _strokeStyle;

  PathStrokerParamsF _strokeParams;
  uint32_t _fillRule;
  float _opacity;

  PointF _textCursor;
  Font _font;

private:
  _FOG_NO_COPY(SvgVisitor)
};

// ============================================================================
// [Fog::SvgGState]
// ============================================================================

struct FOG_NO_EXPORT SvgGState
{
  // --------------------------------------------------------------------------
  // [Constants]
  // --------------------------------------------------------------------------

  enum SAVED_FLAGS
  {
    SAVED_GLOBAL    = 0x0001,
    SAVED_TRANSFORM = 0x0002,
    SAVED_FONT      = 0x0004,
    SAVED_FILL      = 0x0008,
    SAVED_STROKE    = 0x0010
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SvgGState(SvgVisitor* visitor) :
    _visitor(visitor),
    _flags(NO_FLAGS)
  {
  }

  FOG_INLINE ~SvgGState()
  {
    uint32_t flags = _flags;
    if (flags != NO_FLAGS)
    {
      if (flags & SAVED_GLOBAL   ) restoreGlobal();
      if (flags & SAVED_TRANSFORM) restoreTransform();
      if (flags & SAVED_FONT     ) restoreFont();
      if (flags & SAVED_FILL     ) restoreFill();
      if (flags & SAVED_STROKE   ) restoreStroke();
    }
  }

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t hasState(uint32_t state) const
  {
    return (_flags & state) != 0;
  }

  // --------------------------------------------------------------------------
  // [Save / Restore - Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveTransform()
  {
    _transform() = _visitor->getTransform();
    _flags |= SAVED_TRANSFORM;
  }

  FOG_INLINE void restoreTransform()
  {
    _visitor->setTransform(_transform);
  }

  // --------------------------------------------------------------------------
  // [Save / Restore - Global]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveGlobal()
  {
    _opacity = _visitor->getOpacity();
    _cursor = _visitor->_textCursor;
    _flags |= SAVED_GLOBAL;
  }

  FOG_INLINE void restoreGlobal()
  {
    _visitor->_opacity = _opacity;
    _visitor->_textCursor = _cursor;
  }

  // --------------------------------------------------------------------------
  // [Save / Restore - Font]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveFont()
  {
    _font.initCustom1(_visitor->_font);
    _flags |= SAVED_FONT;
  }

  FOG_INLINE void restoreFont()
  {
    _visitor->_font = _font;
    _font.destroy();
  }

  // --------------------------------------------------------------------------
  // [Save / Restore - Fill]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveFill()
  {
    _fillStyle.init(_visitor->_fillStyle);
    _fillRule = _visitor->_fillRule;
    _flags |= SAVED_FILL;
  }

  FOG_INLINE void restoreFill()
  {
    _visitor->_fillStyle = _fillStyle;
    _visitor->_fillRule = _fillRule;

    _fillStyle.destroy();
  }

  // --------------------------------------------------------------------------
  // [Save / Restore - Stroke]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveStroke()
  {
    _strokeStyle.init(_visitor->_strokeStyle);
    _strokeParams.init(_visitor->_strokeParams);
    _flags |= SAVED_STROKE;
  }

  FOG_INLINE void restoreStroke()
  {
    _visitor->_strokeStyle = _strokeStyle;
    _visitor->_strokeParams = _strokeParams;

    _strokeStyle.destroy();
    _strokeParams.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  SvgVisitor* _visitor;

  uint32_t _flags;
  uint32_t _fillRule;

  Static<TransformF> _transform;

  Static<SvgStyle> _fillStyle;
  Static<SvgStyle> _strokeStyle;
  Static<PathStrokerParamsF> _strokeParams;

  Static<Font> _font;
  float _opacity;
  PointF _cursor;

private:
  _FOG_NO_COPY(SvgGState)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SVG_SVGVISITOR_H
