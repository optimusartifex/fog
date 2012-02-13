// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SVG_SVGCONTEXT_H
#define _FOG_G2D_SVG_SVGCONTEXT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/Coord.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/Line.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Text/Font.h>

namespace Fog {

//! @addtogroup Fog_G2d_Svg
//! @{

// ============================================================================
// [Fog::SvgContextSource]
// ============================================================================

struct FOG_NO_EXPORT SvgContextSource
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SvgContextSource() :
    type(SVG_SOURCE_COLOR),
    opacity(1.0f),
    uriRef(NULL),
    color(0xFF000000)
  {
  }

  FOG_INLINE SvgContextSource(const SvgContextSource& other) :
    type(other.type),
    opacity(other.opacity),
    uriRef(other.uriRef),
    color(other.color)
  {
  }

  FOG_INLINE ~SvgContextSource()
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

  FOG_INLINE SvgContextSource& operator=(const SvgContextSource& other)
  {
    type    = other.type;
    opacity = other.opacity;
    color   = other.color;
    uriRef  = other.uriRef;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t type;
  float opacity;
  SvgElement* uriRef;
  Argb32 color;
};

// ============================================================================
// [Fog::SvgContext]
// ============================================================================

//! @brief SVG context.
struct FOG_API SvgContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgContext(SvgContextExtension* contextExtension);
  virtual ~SvgContext();

  // --------------------------------------------------------------------------
  // [Context Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getContextType() const
  {
    return _contextType;
  }

  FOG_INLINE SvgContextExtension* getContextExtension() const
  {
    return _contextExtension;
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

  FOG_INLINE uint32_t getCompOp() const { return _compOp; }
  FOG_INLINE void setCompOp(uint32_t compOp) { _compOp = compOp; }

  FOG_INLINE float getOpacity() const { return _opacity; }
  FOG_INLINE void setOpacity(float opacity) { _opacity = opacity; }

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  FOG_INLINE const SvgContextSource& getFillSource() const
  {
    return _fillSource;
  }

  FOG_INLINE void setFillSource(const SvgContextSource& source)
  {
    _fillSource = source;
  }

  FOG_INLINE void setFillOpacity(float opacity)
  {
    _fillSource.opacity = opacity;
  }

  FOG_INLINE void setFillNone()
  {
    _fillSource.type = SVG_SOURCE_NONE;
  }

  FOG_INLINE void setFillColor(const Argb32& color)
  {
    _fillSource.type = SVG_SOURCE_COLOR;
    _fillSource.color = color;
  }

  FOG_INLINE void setFillPattern(SvgElement* uriRef)
  {
    _fillSource.type = SVG_SOURCE_URI;
    _fillSource.uriRef = uriRef;
  }

  FOG_INLINE void setFillRule(uint32_t fillRule)
  {
    _fillRule = fillRule;
  }

  // --------------------------------------------------------------------------
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  FOG_INLINE const SvgContextSource& getStrokeSource()
  {
    return _strokeSource;
  }

  FOG_INLINE void setStrokeSource(const SvgContextSource& source)
  {
    _strokeSource = source;
  }

  FOG_INLINE void setStrokeOpacity(float opacity)
  {
    _strokeSource.opacity = opacity;
  }

  FOG_INLINE void setStrokeNone()
  {
    _strokeSource.type = SVG_SOURCE_NONE;
  }

  FOG_INLINE void setStrokeColor(const Argb32& color)
  {
    _strokeSource.type = SVG_SOURCE_COLOR;
    _strokeSource.color = color;
  }

  FOG_INLINE void setStrokePattern(SvgElement* uriRef)
  {
    _strokeSource.type = SVG_SOURCE_URI;
    _strokeSource.uriRef = uriRef;
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

  uint32_t _contextType;
  uint32_t _transformDirty;

  SvgContextExtension* _contextExtension;
  TransformF _transform;

  SvgContextSource _fillSource;
  SvgContextSource _strokeSource;

  PathStrokerParamsF _strokeParams;
  uint32_t _compOp : 8;
  uint32_t _fillRule : 8;
  uint32_t _unused : 16;
  float _opacity;

  PointF _textCursor;
  Font _font;

private:
  _FOG_NO_COPY(SvgContext)
};

// ============================================================================
// [Fog::SvgContextGState]
// ============================================================================

struct FOG_NO_EXPORT SvgContextGState
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

  FOG_INLINE SvgContextGState(SvgContext* context) :
    _context(context),
    _flags(NO_FLAGS)
  {
    _compOp = _context->_compOp;
    _opacity = _context->_opacity;
  }

  FOG_INLINE ~SvgContextGState()
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
    
    _context->_compOp = _compOp;
    _context->_opacity = _opacity;
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
    _transform() = _context->getTransform();
    _flags |= SAVED_TRANSFORM;
  }

  FOG_INLINE void restoreTransform()
  {
    _context->setTransform(_transform);
  }

  // --------------------------------------------------------------------------
  // [Save / Restore - Global]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveGlobal()
  {
    _cursor = _context->_textCursor;
    _flags |= SAVED_GLOBAL;
  }

  FOG_INLINE void restoreGlobal()
  {
    _context->_textCursor = _cursor;
  }

  // --------------------------------------------------------------------------
  // [Save / Restore - Font]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveFont()
  {
    _font.initCustom1(_context->_font);
    _flags |= SAVED_FONT;
  }

  FOG_INLINE void restoreFont()
  {
    _context->_font = _font;
    _font.destroy();
  }

  // --------------------------------------------------------------------------
  // [Save / Restore - Fill]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveFill()
  {
    _fillSource.init(_context->_fillSource);
    _fillRule = _context->_fillRule;
    _flags |= SAVED_FILL;
  }

  FOG_INLINE void restoreFill()
  {
    _context->_fillSource = _fillSource;
    _context->_fillRule = _fillRule;

    _fillSource.destroy();
  }

  // --------------------------------------------------------------------------
  // [Save / Restore - Stroke]
  // --------------------------------------------------------------------------

  FOG_INLINE void saveStroke()
  {
    _strokeSource.init(_context->_strokeSource);
    _strokeParams.init(_context->_strokeParams);
    _flags |= SAVED_STROKE;
  }

  FOG_INLINE void restoreStroke()
  {
    _context->_strokeSource = _strokeSource;
    _context->_strokeParams = _strokeParams;

    _strokeSource.destroy();
    _strokeParams.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  SvgContext* _context;

  uint32_t _flags;
  uint32_t _fillRule : 8;
  uint32_t _compOp : 8;
  uint32_t _unused : 16;

  Static<TransformF> _transform;

  Static<SvgContextSource> _fillSource;
  Static<SvgContextSource> _strokeSource;
  Static<PathStrokerParamsF> _strokeParams;

  Static<Font> _font;
  float _opacity;
  PointF _cursor;

private:
  _FOG_NO_COPY(SvgContextGState)
};

// ============================================================================
// [Fog::SvgContextExtension]
// ============================================================================

//! @brief Svg context extension.
struct FOG_API SvgContextExtension
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgContextExtension(SvgContextExtension* contextExtension = NULL);
  virtual ~SvgContextExtension();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual void attach(SvgContext* context) = 0;
  virtual void release() = 0;

private:
  _FOG_NO_COPY(SvgContextExtension)
};

// ============================================================================
// [Fog::SvgRenderContext]
// ============================================================================

//! @brief SVG Render visitor.
struct FOG_API SvgRenderContext : public SvgContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRenderContext(SvgContextExtension* extension);
  virtual ~SvgRenderContext();

  // --------------------------------------------------------------------------
  // [Painter]
  // --------------------------------------------------------------------------

  FOG_INLINE Painter* getPainter() const { return _painter; }
  FOG_INLINE void setPainter(Painter* painter) { _painter = painter; }
  FOG_INLINE void resetPainter() { _painter = NULL; }

  void initPainter();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t onShape(SvgElement* obj, const ShapeF& shape);
  virtual err_t onImage(SvgElement* obj, const PointF& pt, const Image& image);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Painter* _painter;
  Pattern _patternTmp;

private:
  _FOG_NO_COPY(SvgRenderContext)
};

// ============================================================================
// [Fog::SvgMeasureContext]
// ============================================================================

//! @brief SVG measure.
struct FOG_API SvgMeasureContext : public SvgContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgMeasureContext(SvgContextExtension* contextExtension);
  virtual ~SvgMeasureContext();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxF& getBoundingBox() const { return _bbox; }
  FOG_INLINE bool hasBoundingBox() const { return _hasBBox; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _bbox.reset();
    _hasBBox = false;
  }

  // --------------------------------------------------------------------------
  // [Bound]
  // --------------------------------------------------------------------------

  FOG_INLINE void boundWith(const BoxF& b)
  {
    if (_hasBBox)
      BoxF::bound(_bbox, _bbox, b);
    else
      _bbox = b;
  }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t onShape(SvgElement* obj, const ShapeF& shape);
  virtual err_t onImage(SvgElement* obj, const PointF& pt, const Image& image);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  BoxF _bbox;
  bool _hasBBox;

  PathF _pathTmp;

private:
  _FOG_NO_COPY(SvgMeasureContext)
};

// ============================================================================
// [Fog::SvgHitTestContext]
// ============================================================================

//! @brief SVG Hit-Test visitor.
struct FOG_API SvgHitTestContext : public SvgContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgHitTestContext(SvgContextExtension* contextExtension);
  virtual ~SvgHitTestContext();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getPoint() const { return _point; }
  FOG_INLINE void setPoint(const PointF& pt) { _point = pt; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _result.reset();
  }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t onShape(SvgElement* obj, const ShapeF& shape);
  virtual err_t onImage(SvgElement* obj, const PointF& pt, const Image& image);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  TransformF _invTransform;
  PointF _invPoint;

  PointF _point;
  List<SvgElement*> _result;

  PathF _pathTmp;

private:
  _FOG_NO_COPY(SvgHitTestContext)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SVG_SVGCONTEXT_H
