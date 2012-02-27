// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SVG_SVGDOM_H
#define _FOG_G2D_SVG_SVGDOM_H

// [Dependencies]
#include <Fog/Core/Dom/Dom.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/Coord.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Tools/Dpi.h>

namespace Fog {

// ============================================================================
// [Fog::SvgElement]
// ============================================================================

struct FOG_API SvgElement : public DomElement
{
  FOG_DOM_OBJ(SvgElement, DomElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgElement(DomDocument* ownerDocument, const InternedStringW& tagName, uint32_t svgType);
  virtual ~SvgElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgContext* context, SvgContextGState* state) const;
  virtual err_t onProcess(SvgContext* context) const;
  virtual err_t onPattern(SvgContext* context, SvgElement* obj, Pattern* dst) const;

  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;
  virtual err_t onStrokeBoundingBox(BoxF& box, const PathStrokerParamsF& stroke, const TransformF* tr) const;

  err_t _visitContainer(SvgContext* context) const;

  // --------------------------------------------------------------------------
  // [SVG Implementation]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t isBoundingBoxDirty() const { return _boundingBoxDirty; }

  err_t getBoundingBox(BoxF& box) const;
  err_t getBoundingBox(BoxF& box, const TransformF* tr) const;

  // TODO:
  FOG_INLINE void _setDirty() {}

  // --------------------------------------------------------------------------
  // [Visible]
  // --------------------------------------------------------------------------

  FOG_INLINE bool getVisible() const { return _visible; }
  FOG_INLINE void setVisible(bool value) { _visible = value; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  mutable BoxF _computedBoundingBox;

  mutable uint8_t _boundingBoxDirty;
  uint8_t _visible;
  uint8_t _unused_0;
  uint8_t _unused_1;
};

// ============================================================================
// [Fog::SvgStyleData]
// ============================================================================

struct FOG_API SvgStyleData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgStyleData();
  ~SvgStyleData();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t clipRule : 2;
  uint32_t fillRule : 2;
  uint32_t fillSource : 2;
  uint32_t strokeSource : 2;
  uint32_t strokeLineCap : 4;
  uint32_t strokeLineJoin : 4;

  uint32_t strokeDashOffsetUnit : 4;
  uint32_t strokeMiterLimitUnit : 4;
  uint32_t strokeWidthUnit : 4;
  uint32_t fontSizeUnit : 4;

  uint32_t compOp : 8;
  uint32_t letterSpacingUnit : 4;

  // TODO: SVG - Currently unused.
  uint32_t wordSpacingUnit : 4;

  uint32_t unused : 16;

  Argb32 fillColor;
  Argb32 strokeColor;
  Argb32 stopColor;

  float opacity;
  float fillOpacity;
  float strokeOpacity;
  float stopOpacity;

  float strokeDashOffsetValue;
  float strokeMiterLimitValue;
  float strokeWidthValue;
  float fontSizeValue;
  float letterSpacingValue;

  PathF clipPath;
  List<float> strokeDashArray;

  StringW fontFamily;
  StringW fillUri;
  StringW strokeUri;

private:
  _FOG_NO_COPY(SvgStyleData)
};

// ============================================================================
// [Fog::SvgStyleList]
// ============================================================================

struct FOG_API SvgStyle : public DomObj
{
  FOG_DOM_OBJ(SvgStyle, DomObj)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgStyle(SvgElement* ownerElement);
  virtual ~SvgStyle();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  virtual DomObj* _addRef() override;
  virtual void _release() override;

  // --------------------------------------------------------------------------
  // [GC]
  // --------------------------------------------------------------------------

  virtual bool _canCollect() const override;

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  // NOTE: This is quite exception. We define working with properties manually
  // here, because there is overlap between presentation attributes defined by
  // SVG element and presentation attributes defined by style. The SVG-DOM
  // specification says that the presentation attributes defined in CSS style
  // have precedence, but in Fog-Framework these properties are joined together,
  // the last defined property overrides the first defined one, there is no way
  // to specify joining priority.

  enum { _PROPERTY_COUNT = SVG_STYLE_COUNT };
  enum { _PROPERTY_TOTAL = _PROPERTY_INDEX + _PROPERTY_COUNT };

  virtual size_t _getPropertyIndex(const InternedStringW& name) const override;
  virtual size_t _getPropertyIndex(const CharW* name, size_t length) const override;

  virtual err_t _getPropertyInfo(size_t index, PropertyInfo& info) const override;
  virtual err_t _getProperty(size_t index, StringW& value) const override;
  virtual err_t _setProperty(size_t index, const StringW& value) override;
  virtual err_t _resetProperty(size_t index) override;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE SvgElement* getOwnerElement() const { return _ownerElement; }

  // --------------------------------------------------------------------------
  // [Style-Mask]
  // --------------------------------------------------------------------------

  FOG_INLINE uint64_t getStyleMask() const
  {
    return _styleMask;
  }

  FOG_INLINE bool hasStyle(uint32_t styleId) const
  {
    return (_styleMask & ((uint64_t)1 << styleId)) != 0;
  }
  
  // --------------------------------------------------------------------------
  // [Properties - Font Properties]
  // --------------------------------------------------------------------------

  FOG_INLINE CoordF getFontSize() const
  {
    return CoordF(_d.fontSizeValue, _d.fontSizeUnit);
  }

  FOG_INLINE err_t setFontSize(const CoordF& coord)
  {
    _d.fontSizeValue = coord.value;
    _d.fontSizeUnit = (uint8_t)coord.unit;
  
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Properties - Text Properties]
  // --------------------------------------------------------------------------

  FOG_INLINE CoordF getLetterSpacing() const
  {
    return CoordF(_d.letterSpacingValue, _d.letterSpacingUnit);
  }

  FOG_INLINE err_t setLetterSpacing(const CoordF& coord)
  {
    _d.letterSpacingValue = coord.value;
    _d.letterSpacingUnit = (uint8_t)coord.unit;

    return ERR_OK;
  }

  FOG_INLINE void resetLetterSpacing()
  {
  }

  // --------------------------------------------------------------------------
  // [Properties - Other Properties for Visual Media]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Properties - Clipping, Masking, and Compositing Properties]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Properties - Filter Effects Properties]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Properties - Gradient Properties]
  // --------------------------------------------------------------------------

  FOG_INLINE float getStopOpacity() const { return _d.stopOpacity; }
  FOG_INLINE Argb32 getStopColor() const { return _d.stopColor; }

  // --------------------------------------------------------------------------
  // [Properties - Color and Painting properties]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getCompOp() const
  {
    return _d.compOp;
  }
  
  FOG_INLINE err_t setCompOp(uint32_t compOp)
  {
    _d.compOp = compOp;

    return ERR_OK;
  }

  FOG_INLINE CoordF getStrokeDashOffset() const
  {
    return CoordF(_d.strokeDashOffsetValue, _d.strokeDashOffsetUnit);
  }

  FOG_INLINE err_t setStrokeDashOffset(const CoordF& coord)
  {
    _d.strokeDashOffsetValue = coord.value;
    _d.strokeDashOffsetUnit = coord.unit;

    return ERR_OK;
  }

  FOG_INLINE uint32_t getStrokeLineCap()
  {
    return _d.strokeLineCap;
  }

  FOG_INLINE err_t setStrokeLineCap(uint32_t lineCap)
  {
    _d.strokeLineCap = lineCap;

    return ERR_OK;
  }

  FOG_INLINE uint32_t getStrokeLineJoin()
  {
    return _d.strokeLineJoin;
  }

  FOG_INLINE err_t setStrokeLineJoin(uint32_t lineJoin)
  {
    _d.strokeLineJoin = lineJoin;

    return ERR_OK;
  }

  FOG_INLINE CoordF getStrokeMiterLimit() const
  {
    return CoordF(_d.strokeMiterLimitValue, _d.strokeMiterLimitUnit);
  }

  FOG_INLINE err_t setStrokeMiterLimit(const CoordF& coord)
  {
    _d.strokeMiterLimitValue = coord.value;
    _d.strokeMiterLimitUnit = coord.unit;

    return ERR_OK;
  }

  FOG_INLINE float getStrokeOpacity() const
  {
    return _d.strokeOpacity;
  }

  FOG_INLINE err_t setStrokeOpacity(float opacity)
  {
    _d.strokeOpacity = opacity;
    return ERR_OK;
  }

  FOG_INLINE CoordF getStrokeWidth() const
  {
    return CoordF(_d.strokeWidthValue, _d.strokeWidthUnit);
  }

  FOG_INLINE err_t setStrokeWidth(const CoordF& coord)
  {
    _d.strokeWidthValue = coord.value;
    _d.strokeWidthUnit = coord.unit;

    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Style]
  // --------------------------------------------------------------------------

  err_t getStyle(StringW& value) const;
  err_t setStyle(const StringW& value);
  err_t resetStyle();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint64_t _styleMask;
  SvgElement* _ownerElement;
  SvgStyleData _d;
};

// ============================================================================
// [Fog::SvgStylableElement]
// ============================================================================

//! @internal
//!
//! @brief Define new SVG "style" string (this is the redundant main style CSS
//! property).
#define FOG_PROPERTY_STYLE_MAIN(_PropertyName_) \
  enum { PROPERTY_##_PropertyName_ = _PROPERTY_INDEX + (__COUNTER__ - _PROPERTY_COUNTER_BASE) }; \
  \
  struct FOG_NO_EXPORT _Property_##_PropertyName_ \
  { \
    typedef StringW Type; \
    enum { ID = PROPERTY_##_PropertyName_ }; \
    enum { FLAGS = Fog::NO_FLAGS }; \
  };

//! @internal
//!
//! @brief Define new SVG "style" item, linked to @ref SvgStyle.
#define FOG_PROPERTY_STYLE_LINK(_PropertyName_, _Style_) \
  enum { PROPERTY_##_PropertyName_ = _PROPERTY_INDEX + (__COUNTER__ - _PROPERTY_COUNTER_BASE) }; \
  \
  struct FOG_NO_EXPORT _Property_##_PropertyName_ \
  { \
    typedef StringW Type; \
    enum { ID = PROPERTY_##_PropertyName_ }; \
    enum { STYLE = _Style_ }; \
    enum { FLAGS = Fog::NO_FLAGS }; \
    \
    static FOG_INLINE void getValue(Self* self, StringW& value) { self->_style.getProperty(static_cast<size_t>(_Style_), value); } \
    static FOG_INLINE err_t setValue(Self* self, const StringW& value) { return self->_style.setProperty(static_cast<size_t>(_Style_), value); } \
    static FOG_INLINE err_t resetValue(Self* self) { return self->_style.resetProperty(static_cast<size_t>(_Style_)); } \
  };

//! @internal
//!
//! @brief SvgElement which supports styles.
//!
//! @note This is not the final element, must be overridden.
struct FOG_API SvgStylableElement : public SvgElement
{
  FOG_DOM_OBJ(SvgStylableElement, SvgElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgStylableElement(DomDocument* ownerDocument, const InternedStringW& tagName, uint32_t svgType);
  virtual ~SvgStylableElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_STYLE_MAIN(Style)
    FOG_PROPERTY_STYLE_LINK(Font, SVG_STYLE_FONT)
    FOG_PROPERTY_STYLE_LINK(FontFamily, SVG_STYLE_FONT_FAMILY)
    FOG_PROPERTY_STYLE_LINK(FontSize, SVG_STYLE_FONT_SIZE)
    FOG_PROPERTY_STYLE_LINK(Direction, SVG_STYLE_DIRECTION)
    FOG_PROPERTY_STYLE_LINK(LetterSpacing, SVG_STYLE_LETTER_SPACING)
    FOG_PROPERTY_STYLE_LINK(TextDecoration, SVG_STYLE_TEXT_DECORATION)
    FOG_PROPERTY_STYLE_LINK(WordSpacing, SVG_STYLE_WORD_SPACING)
    FOG_PROPERTY_STYLE_LINK(Color, SVG_STYLE_COLOR)
    FOG_PROPERTY_STYLE_LINK(ClipPath, SVG_STYLE_CLIP_PATH)
    FOG_PROPERTY_STYLE_LINK(ClipRule, SVG_STYLE_CLIP_RULE)
    FOG_PROPERTY_STYLE_LINK(Mask, SVG_STYLE_MASK)
    FOG_PROPERTY_STYLE_LINK(Opacity, SVG_STYLE_OPACITY)
    FOG_PROPERTY_STYLE_LINK(EnableBackground, SVG_STYLE_ENABLE_BACKGROUND)
    FOG_PROPERTY_STYLE_LINK(Filter, SVG_STYLE_FILTER)
    FOG_PROPERTY_STYLE_LINK(FloodColor, SVG_STYLE_FLOOD_COLOR)
    FOG_PROPERTY_STYLE_LINK(FloodOpacity, SVG_STYLE_FLOOD_OPACITY)
    FOG_PROPERTY_STYLE_LINK(LightingColor, SVG_STYLE_LIGHTING_COLOR)
    FOG_PROPERTY_STYLE_LINK(StopColor, SVG_STYLE_STOP_COLOR)
    FOG_PROPERTY_STYLE_LINK(StopOpacity, SVG_STYLE_STOP_OPACITY)
    FOG_PROPERTY_STYLE_LINK(CompOp, SVG_STYLE_COMP_OP)
    FOG_PROPERTY_STYLE_LINK(Fill, SVG_STYLE_FILL)
    FOG_PROPERTY_STYLE_LINK(FillOpacity, SVG_STYLE_FILL_OPACITY)
    FOG_PROPERTY_STYLE_LINK(FillRule, SVG_STYLE_FILL_RULE)
    FOG_PROPERTY_STYLE_LINK(ImageRendering, SVG_STYLE_IMAGE_RENDERING)
    FOG_PROPERTY_STYLE_LINK(Marker, SVG_STYLE_MARKER)
    FOG_PROPERTY_STYLE_LINK(MarkerEnd, SVG_STYLE_MARKER_END)
    FOG_PROPERTY_STYLE_LINK(MarkerMid, SVG_STYLE_MARKER_MID)
    FOG_PROPERTY_STYLE_LINK(MarkerStart, SVG_STYLE_MARKER_START)
    FOG_PROPERTY_STYLE_LINK(ShapeRendering, SVG_STYLE_SHAPE_RENDERING)
    FOG_PROPERTY_STYLE_LINK(Stroke, SVG_STYLE_STROKE)
    FOG_PROPERTY_STYLE_LINK(StrokeDashArray, SVG_STYLE_STROKE_DASH_ARRAY)
    FOG_PROPERTY_STYLE_LINK(StrokeDashOffset, SVG_STYLE_STROKE_DASH_OFFSET)
    FOG_PROPERTY_STYLE_LINK(StrokeLineCap, SVG_STYLE_STROKE_LINE_CAP)
    FOG_PROPERTY_STYLE_LINK(StrokeLineJoin, SVG_STYLE_STROKE_LINE_JOIN)
    FOG_PROPERTY_STYLE_LINK(StrokeMiterLimit, SVG_STYLE_STROKE_MITER_LIMIT)
    FOG_PROPERTY_STYLE_LINK(StrokeOpacity, SVG_STYLE_STROKE_OPACITY)
    FOG_PROPERTY_STYLE_LINK(StrokeWidth, SVG_STYLE_STROKE_WIDTH)
    FOG_PROPERTY_STYLE_LINK(TextRendering, SVG_STYLE_TEXT_RENDERING)
  FOG_PROPERTY_END()

  FOG_INLINE SvgStyle* getStyle() const { return const_cast<SvgStyle*>(&_style); }

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgContext* context, SvgContextGState* state) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  SvgStyle _style;
};

// ============================================================================
// [Fog::SvgTransformableElement]
// ============================================================================

struct FOG_API SvgTransformableElement : public SvgStylableElement
{
  FOG_DOM_OBJ(SvgTransformableElement, SvgStylableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgTransformableElement(DomDocument* ownerDocument, const InternedStringW& tagName, uint32_t svgType);
  virtual ~SvgTransformableElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(Transform, TransformF)
  FOG_PROPERTY_END()

  FOG_INLINE const TransformF& getTransform() const { return _transform; }
  err_t setTransform(const TransformF& transform);
  err_t resetTransform();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgContext* context, SvgContextGState* state) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  TransformF _transform;
};

// ============================================================================
// [Fog::SvgRootElement]
// ============================================================================

struct FOG_API SvgRootElement : public SvgStylableElement
{
  FOG_DOM_OBJ(SvgRootElement, SvgStylableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRootElement(DomDocument* ownerDocument);
  virtual ~SvgRootElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(X, CoordF)
    FOG_PROPERTY_RW(Y, CoordF)
    FOG_PROPERTY_RW(Width, CoordF)
    FOG_PROPERTY_RW(Height, CoordF)
    FOG_PROPERTY_RW(ViewBox, BoxF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getX() const { return CoordF(_x, _xUnit); }
  err_t setX(const CoordF& x);
  err_t resetX();

  FOG_INLINE CoordF getY() const { return CoordF(_y, _yUnit); }
  err_t setY(const CoordF& y);
  err_t resetY();

  FOG_INLINE CoordF getWidth() const { return CoordF(_width, _widthUnit); }
  err_t setWidth(const CoordF& width);
  err_t resetWidth();

  FOG_INLINE CoordF getHeight() const { return CoordF(_height, _heightUnit); }
  err_t setHeight(const CoordF& height);
  err_t resetHeight();

  FOG_INLINE const BoxF& getViewBox() const { return _viewBox; }
  err_t setViewBox(const BoxF& viewBox);
  err_t resetViewBox();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgContext* context, SvgContextGState* state) const override;
  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  SizeF getRootSize() const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  BoxF _viewBox;

  float _x;
  float _y;
  float _width;
  float _height;

  uint32_t _xUnit : 4;
  uint32_t _yUnit : 4;
  uint32_t _widthUnit : 4;
  uint32_t _heightUnit : 4;

  // TODO: Rendering Model.
  // TODO: PreserveAspectRatio.
};

// ============================================================================
// [Fog::SvgSolidColorElement]
// ============================================================================

struct FOG_API SvgSolidColorElement : public SvgStylableElement
{
  FOG_DOM_OBJ(SvgSolidColorElement, SvgStylableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgSolidColorElement(DomDocument* ownerDocument);
  virtual ~SvgSolidColorElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
};

// ============================================================================
// [Fog::SvgGradientElement]
// ============================================================================

struct FOG_API SvgGradientElement : public SvgStylableElement
{
  FOG_DOM_OBJ(SvgGradientElement, SvgStylableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgGradientElement(
    DomDocument* ownerDocument,
    const InternedStringW& tagName,
    uint32_t svgType);
  virtual ~SvgGradientElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(SpreadMethod, uint32_t)
    FOG_PROPERTY_RW(GradientUnits, uint32_t)
    FOG_PROPERTY_RW(GradientTransform, TransformF)
  FOG_PROPERTY_END()

  FOG_INLINE uint32_t getSpreadMethod() const { return _spreadMethod; }
  err_t setSpreadMethod(uint32_t spreadMethod);
  err_t resetSpreadMethod();

  FOG_INLINE uint32_t getGradientUnits() const { return _gradientUnits; }
  err_t setGradientUnits(uint32_t gradientUnits);
  err_t resetGradientUnits();

  FOG_INLINE const TransformF& getGradientTransform() const { return _gradientTransform; }
  err_t setGradientTransform(const TransformF& gradientTransform);
  err_t resetGradientTransform();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static void _walkAndAddColorStops(DomElement* root, GradientF& gradient);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  TransformF _gradientTransform;
  uint32_t _spreadMethod : 4;
  uint32_t _gradientUnits : 4;
};

// ============================================================================
// [Fog::SvgLinearGradientElement]
// ============================================================================

struct FOG_API SvgLinearGradientElement : public SvgGradientElement
{
  FOG_DOM_OBJ(SvgLinearGradientElement, SvgGradientElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgLinearGradientElement(DomDocument* ownerDocument);
  virtual ~SvgLinearGradientElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(X1, CoordF)
    FOG_PROPERTY_RW(Y1, CoordF)
    FOG_PROPERTY_RW(X2, CoordF)
    FOG_PROPERTY_RW(Y2, CoordF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getX1() const { return CoordF(_x1, _x1Unit); }
  err_t setX1(const CoordF& x1);
  err_t resetX1();

  FOG_INLINE CoordF getY1() const { return CoordF(_y1, _y1Unit); }
  err_t setY1(const CoordF& y1);
  err_t resetY1();

  FOG_INLINE CoordF getX2() const { return CoordF(_x2, _x2Unit); }
  err_t setX2(const CoordF& x2);
  err_t resetX2();

  FOG_INLINE CoordF getY2() const { return CoordF(_y2, _y2Unit); }
  err_t setY2(const CoordF& y2);
  err_t resetY2();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPattern(SvgContext* context, SvgElement* obj, Pattern* dst) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _x1;
  float _y1;
  float _x2;
  float _y2;

  uint32_t _x1Unit : 4;
  uint32_t _y1Unit : 4;
  uint32_t _x2Unit : 4;
  uint32_t _y2Unit : 4;
};

// ============================================================================
// [Fog::SvgRadialGradientElement]
// ============================================================================

struct FOG_API SvgRadialGradientElement : public SvgGradientElement
{
  FOG_DOM_OBJ(SvgRadialGradientElement, SvgGradientElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRadialGradientElement(DomDocument* ownerDocument);
  virtual ~SvgRadialGradientElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(Cx, CoordF)
    FOG_PROPERTY_RW(Cy, CoordF)
    FOG_PROPERTY_RW(Fx, CoordF)
    FOG_PROPERTY_RW(Fy, CoordF)
    FOG_PROPERTY_RW(R, CoordF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getCx() const { return CoordF(_cx, _cxUnit); }
  err_t setCx(const CoordF& cx);
  err_t resetCx();

  FOG_INLINE CoordF getCy() const { return CoordF(_cy, _cyUnit); }
  err_t setCy(const CoordF& cy);
  err_t resetCy();

  FOG_INLINE CoordF getFx() const { return CoordF(_fx, _fxUnit); }
  err_t setFx(const CoordF& fx);
  err_t resetFx();

  FOG_INLINE CoordF getFy() const { return CoordF(_fy, _fyUnit); }
  err_t setFy(const CoordF& fy);
  err_t resetFy();

  FOG_INLINE CoordF getR() const { return CoordF(_r, _rUnit); }
  err_t setR(const CoordF& r);
  err_t resetR();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPattern(SvgContext* context, SvgElement* obj, Pattern* dst) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _cx;
  float _cy;
  float _fx;
  float _fy;
  float _r;
  
  uint32_t _cxUnit : 4;
  uint32_t _cyUnit : 4;
  uint32_t _fxUnit : 4;
  uint32_t _fyUnit : 4;
  uint32_t _rUnit : 4;

  uint32_t _fxAssigned : 1;
  uint32_t _fyAssigned : 1;
};

// ============================================================================
// [Fog::SvgPatternElement]
// ============================================================================

struct FOG_API SvgPatternElement : public SvgElement
{
  FOG_DOM_OBJ(SvgPatternElement, SvgElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPatternElement(DomDocument* ownerDocument);
  virtual ~SvgPatternElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(X, CoordF)
    FOG_PROPERTY_RW(Y, CoordF)
    FOG_PROPERTY_RW(Width, CoordF)
    FOG_PROPERTY_RW(Height, CoordF)
    FOG_PROPERTY_RW(PatternUnits, uint32_t)
    FOG_PROPERTY_RW(PatternTransform, TransformF)
    FOG_PROPERTY_RW(ViewBox, BoxF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getX() const { return CoordF(_x, _xUnit); }
  err_t setX(const CoordF& x);
  err_t resetX();

  FOG_INLINE CoordF getY() const { return CoordF(_y, _yUnit); }
  err_t setY(const CoordF& y);
  err_t resetY();

  FOG_INLINE CoordF getWidth() const { return CoordF(_width, _widthUnit); }
  err_t setWidth(const CoordF& width);
  err_t resetWidth();

  FOG_INLINE CoordF getHeight() const { return CoordF(_height, _heightUnit); }
  err_t setHeight(const CoordF& height);
  err_t resetHeight();

  FOG_INLINE const TransformF& getPatternTransform() const { return _patternTransform; }
  err_t setPatternTransform(const TransformF& patternTransform);
  err_t resetPatternTransform();

  FOG_INLINE uint32_t getPatternUnits() const { return _patternUnits; }
  err_t setPatternUnits(uint32_t patternUnits);
  err_t resetPatternUnits();

  FOG_INLINE const BoxF& getViewBox() const { return _viewBox; }
  err_t setViewBox(const BoxF& viewBox);
  err_t resetViewBox();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onPattern(SvgContext* context, SvgElement* obj, Pattern* dst) const override;

  err_t _createPattern(Pattern& pattern, SvgElement* obj) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  TransformF _patternTransform;
  BoxF _viewBox;

  float _x;
  float _y;
  float _width;
  float _height;

  uint32_t _xUnit : 4;
  uint32_t _yUnit : 4;
  uint32_t _widthUnit : 4;
  uint32_t _heightUnit : 4;

  uint32_t _patternUnits : 4;
};

// ============================================================================
// [Fog::SvgStopElement]
// ============================================================================

struct FOG_API SvgStopElement : public SvgStylableElement
{
  FOG_DOM_OBJ(SvgStopElement, SvgStylableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgStopElement(DomDocument* ownerDocument);
  virtual ~SvgStopElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(Offset, float)
  FOG_PROPERTY_END()

  FOG_INLINE float getOffset() const { return _offset; }
  err_t setOffset(float offset);
  err_t resetOffset();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _offset;
  bool _offsetAssigned;
};

// ============================================================================
// [Fog::SvgDefsElement]
// ============================================================================

struct FOG_API SvgDefsElement : public SvgElement
{
  FOG_DOM_OBJ(SvgDefsElement, SvgElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgDefsElement(DomDocument* ownerDocument);
  virtual ~SvgDefsElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const;
};

// ============================================================================
// [Fog::SvgGElement]
// ============================================================================

struct FOG_API SvgGElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgGElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgGElement(DomDocument* ownerDocument);
  virtual ~SvgGElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;
};

// ============================================================================
// [Fog::SvgSymbolElement]
// ============================================================================

struct FOG_API SvgSymbolElement : public SvgStylableElement
{
  FOG_DOM_OBJ(SvgSymbolElement, SvgStylableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgSymbolElement(DomDocument* ownerDocument);
  virtual ~SvgSymbolElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const;
};

// ============================================================================
// [Fog::SvgUseElement]
// ============================================================================

struct FOG_API SvgUseElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgUseElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgUseElement(DomDocument* ownerDocument);
  virtual ~SvgUseElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(X, CoordF)
    FOG_PROPERTY_RW(Y, CoordF)
    FOG_PROPERTY_RW(Width, CoordF)
    FOG_PROPERTY_RW(Height, CoordF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getX() const { return CoordF(_x, _xUnit); }
  err_t setX(const CoordF& x);
  err_t resetX();

  FOG_INLINE CoordF getY() const { return CoordF(_y, _yUnit); }
  err_t setY(const CoordF& y);
  err_t resetY();

  FOG_INLINE CoordF getWidth() const { return CoordF(_width, _widthUnit); }
  err_t setWidth(const CoordF& width);
  err_t resetWidth();

  FOG_INLINE CoordF getHeight() const { return CoordF(_height, _heightUnit); }
  err_t setHeight(const CoordF& y);
  err_t resetHeight();

  // --------------------------------------------------------------------------
  // [SVG Methods]
  // --------------------------------------------------------------------------

  SvgElement* getLinkedElement() const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgContext* context, SvgContextGState* state) const override;
  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _x;
  float _y;
  float _width;
  float _height;

  uint32_t _xUnit : 4;
  uint32_t _yUnit : 4;
  uint32_t _widthUnit : 4;
  uint32_t _heightUnit : 4;

  uint32_t _widthAssigned : 1;
  uint32_t _heightAssigned : 1;
};

// ============================================================================
// [Fog::SvgViewElement]
// ============================================================================

struct FOG_API SvgViewElement : public SvgElement
{
  FOG_DOM_OBJ(SvgViewElement, SvgElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgViewElement(DomDocument* ownerDocument);
  virtual ~SvgViewElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(ViewBox, BoxF)
  FOG_PROPERTY_END()

  FOG_INLINE const BoxF& getViewBox() const { return _viewBox; }
  err_t setViewBox(const BoxF& viewBox);
  err_t resetViewBox();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  BoxF _viewBox;
  // TODO: PreserveAspectRatio.
};

// ============================================================================
// [Fog::SvgCircleElement]
// ============================================================================

struct FOG_API SvgCircleElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgCircleElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgCircleElement(DomDocument* ownerDocument);
  virtual ~SvgCircleElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(Cx, CoordF)
    FOG_PROPERTY_RW(Cy, CoordF)
    FOG_PROPERTY_RW(R, CoordF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getCx() const { return CoordF(_cx, _cxUnit); }
  err_t setCx(const CoordF& cx);
  err_t resetCx();

  FOG_INLINE CoordF getCy() const { return CoordF(_cy, _cyUnit); }
  err_t setCy(const CoordF& cy);
  err_t resetCy();

  FOG_INLINE CoordF getR() const { return CoordF(_r, _rUnit); }
  err_t setR(const CoordF& r);
  err_t resetR();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _cx;
  float _cy;
  float _r;

  uint32_t _cxUnit : 4;
  uint32_t _cyUnit : 4;
  uint32_t _rUnit : 4;
};

// ============================================================================
// [Fog::SvgEllipseElement]
// ============================================================================

struct FOG_API SvgEllipseElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgEllipseElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgEllipseElement(DomDocument* ownerDocument);
  virtual ~SvgEllipseElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(Cx, CoordF)
    FOG_PROPERTY_RW(Cy, CoordF)
    FOG_PROPERTY_RW(Rx, CoordF)
    FOG_PROPERTY_RW(Ry, CoordF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getCx() const { return CoordF(_cx, _cxUnit); }
  err_t setCx(const CoordF& cx);
  err_t resetCx();

  FOG_INLINE CoordF getCy() const { return CoordF(_cy, _cyUnit); }
  err_t setCy(const CoordF& cy);
  err_t resetCy();

  FOG_INLINE CoordF getRx() const { return CoordF(_rx, _rxUnit); }
  err_t setRx(const CoordF& rx);
  err_t resetRx();

  FOG_INLINE CoordF getRy() const { return CoordF(_ry, _ryUnit); }
  err_t setRy(const CoordF& ry);
  err_t resetRy();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _cx;
  float _cy;
  float _rx;
  float _ry;

  uint32_t _cxUnit : 4;
  uint32_t _cyUnit : 4;
  uint32_t _rxUnit : 4;
  uint32_t _ryUnit : 4;
};

// ============================================================================
// [Fog::SvgLineElement]
// ============================================================================

struct FOG_API SvgLineElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgLineElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgLineElement(DomDocument* ownerDocument);
  virtual ~SvgLineElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(X1, CoordF)
    FOG_PROPERTY_RW(Y1, CoordF)
    FOG_PROPERTY_RW(X2, CoordF)
    FOG_PROPERTY_RW(Y2, CoordF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getX1() const { return CoordF(_x1, _x1Unit); }
  err_t setX1(const CoordF& x1);
  err_t resetX1();

  FOG_INLINE CoordF getY1() const { return CoordF(_y1, _y1Unit); }
  err_t setY1(const CoordF& y1);
  err_t resetY1();

  FOG_INLINE CoordF getX2() const { return CoordF(_x2, _x2Unit); }
  err_t setX2(const CoordF& x2);
  err_t resetX2();

  FOG_INLINE CoordF getY2() const { return CoordF(_y2, _y2Unit); }
  err_t setY2(const CoordF& y2);
  err_t resetY2();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  float _x1;
  float _y1;
  float _x2;
  float _y2;

  uint32_t _x1Unit : 4;
  uint32_t _y1Unit : 4;
  uint32_t _x2Unit : 4;
  uint32_t _y2Unit : 4;
};

// ============================================================================
// [Fog::SvgPathElement]
// ============================================================================

struct FOG_API SvgPathElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgPathElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPathElement(DomDocument* ownerDocument);
  virtual ~SvgPathElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(D, PathF)
  FOG_PROPERTY_END()

  FOG_INLINE const PathF& getD() const { return _d; }
  err_t setD(const PathF& d);
  err_t resetD();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  PathF _d;
};

// ============================================================================
// [Fog::SvgPolygonElement]
// ============================================================================

struct FOG_API SvgPolygonElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgPolygonElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPolygonElement(DomDocument* ownerDocument);
  virtual ~SvgPolygonElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(Points, PathF)
  FOG_PROPERTY_END()

  FOG_INLINE const PathF& getPoints() const { return _points; }
  err_t setPoints(const PathF& points);
  err_t resetPoints();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  PathF _points;
};

// ============================================================================
// [Fog::SvgPolylineElement]
// ============================================================================

struct FOG_API SvgPolylineElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgPolylineElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPolylineElement(DomDocument* ownerDocument);
  virtual ~SvgPolylineElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(Points, PathF)
  FOG_PROPERTY_END()

  FOG_INLINE const PathF& getPoints() const { return _points; }
  err_t setPoints(const PathF& points);
  err_t resetPoints();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  PathF _points;
};

// ============================================================================
// [Fog::SvgRectElement]
// ============================================================================

struct FOG_API SvgRectElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgRectElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRectElement(DomDocument* ownerDocument);
  virtual ~SvgRectElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(X, CoordF)
    FOG_PROPERTY_RW(Y, CoordF)
    FOG_PROPERTY_RW(Width, CoordF)
    FOG_PROPERTY_RW(Height, CoordF)
    FOG_PROPERTY_RW(Rx, CoordF)
    FOG_PROPERTY_RW(Ry, CoordF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getX() const { return CoordF(_x, _xUnit); }
  err_t setX(const CoordF& x);
  err_t resetX();

  FOG_INLINE CoordF getY() const { return CoordF(_y, _yUnit); }
  err_t setY(const CoordF& y);
  err_t resetY();

  FOG_INLINE CoordF getWidth() const { return CoordF(_width, _widthUnit); }
  err_t setWidth(const CoordF& width);
  err_t resetWidth();

  FOG_INLINE CoordF getHeight() const { return CoordF(_height, _heightUnit); }
  err_t setHeight(const CoordF& height);
  err_t resetHeight();

  FOG_INLINE CoordF getRx() const { return CoordF(_rx, _rxUnit); }
  err_t setRx(const CoordF& rx);
  err_t resetRx();

  FOG_INLINE CoordF getRy() const { return CoordF(_ry, _ryUnit); }
  err_t setRy(const CoordF& ry);
  err_t resetRy();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _x;
  float _y;
  float _width;
  float _height;
  float _rx;
  float _ry;

  uint32_t _xUnit : 4;
  uint32_t _yUnit : 4;
  uint32_t _widthUnit : 4;
  uint32_t _heightUnit : 4;
  uint32_t _rxUnit : 4;
  uint32_t _ryUnit : 4;
  uint32_t _rxAssigned : 1;
  uint32_t _ryAssigned : 1;
};

// ============================================================================
// [Fog::SvgImageElement]
// ============================================================================

struct FOG_API SvgImageElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgImageElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgImageElement(DomDocument* ownerDocument);
  virtual ~SvgImageElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(X, CoordF)
    FOG_PROPERTY_RW(Y, CoordF)
    FOG_PROPERTY_RW(Width, CoordF)
    FOG_PROPERTY_RW(Height, CoordF)
    FOG_PROPERTY_RW(Href, StringW)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getX() const { return CoordF(_x, _xUnit); }
  err_t setX(const CoordF& x);
  err_t resetX();

  FOG_INLINE CoordF getY() const { return CoordF(_y, _yUnit); }
  err_t setY(const CoordF& y);
  err_t resetY();

  FOG_INLINE CoordF getWidth() const { return CoordF(_width, _widthUnit); }
  err_t setWidth(const CoordF& width);
  err_t resetWidth();

  FOG_INLINE CoordF getHeight() const { return CoordF(_height, _heightUnit); }
  err_t setHeight(const CoordF& height);
  err_t resetHeight();

  FOG_INLINE const StringW& getHref() const { return _href; }
  err_t setHref(const StringW& href);
  err_t resetHref();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgContext* context, SvgContextGState* state) const override;
  virtual err_t onProcess(SvgContext* context) const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  StringW _href;
  DomResourceItem* _resource;

  float _x;
  float _y;
  float _width;
  float _height;

  uint32_t _xUnit : 4;
  uint32_t _yUnit : 4;
  uint32_t _widthUnit : 4;
  uint32_t _heightUnit : 4;
};

// ============================================================================
// [Fog::SvgTextPositioningElement]
// ============================================================================

struct FOG_API SvgTextPositioningElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgTextPositioningElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgTextPositioningElement(DomDocument* ownerDocument,
    const InternedStringW& tagName,
    uint32_t svgType);
  virtual ~SvgTextPositioningElement();

  // --------------------------------------------------------------------------
  // [SVG Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(X, CoordF)
    FOG_PROPERTY_RW(Y, CoordF)
    FOG_PROPERTY_RW(Dx, CoordF)
    FOG_PROPERTY_RW(Dy, CoordF)
  FOG_PROPERTY_END()

  FOG_INLINE CoordF getX() const { return CoordF(_x, _xUnit); }
  err_t setX(const CoordF& x);
  err_t resetX();

  FOG_INLINE CoordF getY() const { return CoordF(_y, _yUnit); }
  err_t setY(const CoordF& y);
  err_t resetY();

  FOG_INLINE CoordF getDx() const { return CoordF(_dx, _dxUnit); }
  err_t setDx(const CoordF& dx);
  err_t resetDx();

  FOG_INLINE CoordF getDy() const { return CoordF(_dy, _dyUnit); }
  err_t setDy(const CoordF& dy);
  err_t resetDy();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _x;
  float _y;
  float _dx;
  float _dy;

  uint32_t _xUnit : 4;
  uint32_t _yUnit : 4;
  uint32_t _dxUnit : 4;
  uint32_t _dyUnit : 4;
};

// ============================================================================
// [Fog::SvgTextElement]
// ============================================================================

struct FOG_API SvgTextElement : public SvgTextPositioningElement
{
  FOG_DOM_OBJ(SvgTextElement, SvgTextPositioningElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgTextElement(DomDocument* ownerDocument);
  virtual ~SvgTextElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgContext* context, SvgContextGState* state) const override;
  virtual err_t onProcess(SvgContext* context) const override;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const override;

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  // TODO: Ignored.
  // SvgCoordAttr _textLength;
};

// ============================================================================
// [Fog::SvgTSpanElement]
// ============================================================================

struct FOG_API SvgTSpanElement : public SvgTextPositioningElement
{
  FOG_DOM_OBJ(SvgTSpanElement, SvgTextPositioningElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgTSpanElement(DomDocument* ownerDocument);
  virtual ~SvgTSpanElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgContext* context, SvgContextGState* state) const override;
  virtual err_t onProcess(SvgContext* context) const override;

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  // TODO: Ignored.
  // SvgCoordAttr _textLength;
  // TODO: Ignored.
  // DomEnumAttr _lengthAdjust;
};

// ============================================================================
// [Fog::SvgAElement]
// ============================================================================

struct FOG_API SvgAElement : public SvgTransformableElement
{
  FOG_DOM_OBJ(SvgAElement, SvgTransformableElement)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgAElement(DomDocument* ownerDocument);
  virtual ~SvgAElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgContext* context) const override;
};

// ============================================================================
// [Fog::SvgDocument]
// ============================================================================

struct FOG_API SvgDocument : public DomDocument
{
  FOG_DOM_OBJ(SvgDocument, DomDocument)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgDocument();
  virtual ~SvgDocument();

  // --------------------------------------------------------------------------
  // [DOM Interface]
  // --------------------------------------------------------------------------

  virtual DomDocument* _createDocument() override;
  virtual DomElement* _createElement(const InternedStringW& tagName) override;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual SvgContextExtension* _createContextExtension(const SvgContextExtension* ex);

  // --------------------------------------------------------------------------
  // [DPI / Size]
  // --------------------------------------------------------------------------

  FOG_INLINE float getDpi() const { return _dpi.getDpi(); }
  err_t setDpi(float dpi);

  SizeF getDocumentSize() const;

  // --------------------------------------------------------------------------
  // [Context]
  // --------------------------------------------------------------------------

  err_t onProcess(SvgContext* context);

  err_t render(Painter* painter);
  List<SvgElement*> hitTest(const PointF& pt, const TransformF* tr = NULL);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Dpi _dpi;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SVG_SVGDOM_H
