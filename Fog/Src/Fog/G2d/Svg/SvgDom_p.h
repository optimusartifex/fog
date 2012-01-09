// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SVG_SVGDOM_P_H
#define _FOG_G2D_SVG_SVGDOM_P_H

// [Dependencies]
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
#include <Fog/G2d/Svg/SvgElement.h>

namespace Fog {

// ============================================================================
// [Fog::SvgEnumItem]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct FOG_NO_EXPORT SvgEnumItem
{
  char name[20];
  int value;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::SvgEnumAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgEnumAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgEnumAttribute(XmlElement* element, const ManagedStringW& name, int offset, const SvgEnumItem* items);
  virtual ~SvgEnumAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Coords]
  // --------------------------------------------------------------------------

  FOG_INLINE int getEnumValue() const { return _enumValue; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const SvgEnumItem* _enumItems;
  int _enumValue;

private:
  _FOG_NO_COPY(SvgEnumAttribute)
};

// ============================================================================
// [Fog::SvgCoordAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgCoordAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgCoordAttribute(XmlElement* element, const ManagedStringW& name, int offset);
  virtual ~SvgCoordAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Coords]
  // --------------------------------------------------------------------------

  FOG_INLINE const CoordF& getCoord() const { return _coord; }
  FOG_INLINE float getCoordValue() const { return _coord.value; };
  FOG_INLINE uint32_t getCoordUnit() const { return _coord.unit; };

  FOG_INLINE float getCoordComputed() const
  {
    SvgDocument* doc = reinterpret_cast<SvgDocument*>(_element->getDocument());
    return doc->_dpi.toDeviceSpace(_coord.value, _coord.unit);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  CoordF _coord;

private:
  _FOG_NO_COPY(SvgCoordAttribute)
};

// ============================================================================
// [Fog::SvgImageLinkAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgImageLinkAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgImageLinkAttribute(XmlElement* element, const ManagedStringW& name, int offset);
  virtual ~SvgImageLinkAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual StringW getValue() const;
  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Image]
  // --------------------------------------------------------------------------

  FOG_INLINE void setImage(const Image& image) { _image = image; }
  FOG_INLINE const Image& getImage() const { return _image; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Image _image;
  bool _embedded;

private:
  _FOG_NO_COPY(SvgImageLinkAttribute)
};

// ============================================================================
// [Fog::SvgOffsetAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgOffsetAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgOffsetAttribute(XmlElement* element, const ManagedStringW& name, int offset);
  virtual ~SvgOffsetAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Coords]
  // --------------------------------------------------------------------------

  FOG_INLINE float getOffset() const { return _offset; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _offset;

private:
  _FOG_NO_COPY(SvgOffsetAttribute)
};

// ============================================================================
// [Fog::SvgPathAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgPathAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPathAttribute(XmlElement* element, const ManagedStringW& name, int offset);
  virtual ~SvgPathAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Coords]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathF& getPath() const { return _path; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PathF _path;

private:
  _FOG_NO_COPY(SvgPathAttribute)
};

// ============================================================================
// [Fog::SvgPointsAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgPointsAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPointsAttribute(XmlElement* element, const ManagedStringW& name, bool closePath, int offset);
  virtual ~SvgPointsAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Coords]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathF& getPath() const { return _path; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PathF _path;
  bool _closePath;

private:
  _FOG_NO_COPY(SvgPointsAttribute)
};

// ============================================================================
// [Fog::SvgTransformAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgTransformAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgTransformAttribute(XmlElement* element, const ManagedStringW& name, int offset);
  virtual ~SvgTransformAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FOG_INLINE const TransformF& getTransform() const { return _transform; }
  FOG_INLINE bool isValid() const { return _isValid; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  TransformF _transform;
  bool _isValid;

private:
  _FOG_NO_COPY(SvgTransformAttribute)
};

// ============================================================================
// [Fog::SvgViewBoxAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgViewBoxAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgViewBoxAttribute(XmlElement* element, const ManagedStringW& name, int offset);
  virtual ~SvgViewBoxAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Box]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxF& getBox() const { return _box; }
  FOG_INLINE bool isValid() const { return _isValid; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  BoxF _box;
  bool _isValid;

private:
  _FOG_NO_COPY(SvgViewBoxAttribute)
};

// ============================================================================
// [Fog::SvgStyleAttribute]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct FOG_NO_EXPORT SvgStyleAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgStyleAttribute(XmlElement* element, const ManagedStringW& name, int offset);
  virtual ~SvgStyleAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual StringW getValue() const;
  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getMask() const { return _mask; }
  FOG_INLINE bool hasStyle(int id) const { return (_mask & (1 << id)) != 0; }
  
  FOG_INLINE CoordF getStrokeDashOffset() const { return CoordF(_strokeDashOffsetValue, _strokeDashOffsetUnit); }
  FOG_INLINE CoordF getStrokeMiterLimit() const { return CoordF(_strokeMiterLimitValue, _strokeMiterLimitUnit); }
  FOG_INLINE CoordF getStrokeWidth() const { return CoordF(_strokeWidthValue, _strokeWidthUnit); }
  FOG_INLINE CoordF getFontSize() const { return CoordF(_fontSizeValue, _fontSizeUnit); }
  FOG_INLINE CoordF getLetterSpacing() const { return CoordF(_letterSpacingValue, _letterSpacingUnit); }

  FOG_INLINE void setStrokeDashOffset(const CoordF& coord)
  {
    _strokeDashOffsetValue = coord.value;
    _strokeDashOffsetUnit = (uint8_t)coord.unit;
  }

  FOG_INLINE void setStrokeMiterLimit(const CoordF& coord)
  {
    _strokeMiterLimitValue = coord.value;
    _strokeMiterLimitUnit = (uint8_t)coord.unit;
  }

  FOG_INLINE void setStrokeWidth(const CoordF& coord)
  {
    _strokeWidthValue = coord.value;
    _strokeWidthUnit = (uint8_t)coord.unit;
  }

  FOG_INLINE void setFontSize(const CoordF& coord)
  {
    _fontSizeValue = coord.value;
    _fontSizeUnit = (uint8_t)coord.unit;
  }

  FOG_INLINE void setLetterSpacing(const CoordF& coord)
  {
    _letterSpacingValue = coord.value;
    _letterSpacingUnit = (uint8_t)coord.unit;
  }

  // --------------------------------------------------------------------------
  // [Style]
  // --------------------------------------------------------------------------

  StringW getStyle(const ManagedStringW& name) const;
  err_t setStyle(const ManagedStringW& name, const StringW& value);

  StringW getStyle(int styleId) const;
  err_t setStyle(int styleId, const StringW& value);

  static int styleToId(const ManagedStringW& name);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _mask;

  uint8_t _clipRule;
  uint8_t _fillSource;
  uint8_t _fillRule;
  uint8_t _strokeSource;
  uint8_t _strokeLineCap;
  uint8_t _strokeLineJoin;

  uint8_t _strokeDashOffsetUnit;
  uint8_t _strokeMiterLimitUnit;
  uint8_t _strokeWidthUnit;
  uint8_t _fontSizeUnit;
  uint8_t _letterSpacingUnit;
  uint8_t _reserved_0;

  Color _fillColor;
  Color _strokeColor;
  Color _stopColor;

  float _opacity;
  float _fillOpacity;
  float _strokeOpacity;
  float _stopOpacity;

  float _strokeDashOffsetValue;
  float _strokeMiterLimitValue;
  float _strokeWidthValue;
  float _fontSizeValue;
  float _letterSpacingValue;

  PathF _clipPath;
  List<float> _dashArray;

  StringW _fillUri;
  StringW _strokeUri;
  StringW _fontFamily;

private:
  _FOG_NO_COPY(SvgStyleAttribute)
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::SvgStyledElement]
// ============================================================================

//! @internal
//!
//! @brief SvgElement which supports styles.
//!
//! @note This is not the final element, must be overridden.
struct FOG_NO_EXPORT SvgStyledElement : public SvgElement
{
  typedef SvgElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgStyledElement(const ManagedStringW& tagName, uint32_t svgType);
  virtual ~SvgStyledElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  virtual err_t _setAttribute(const ManagedStringW& name, const StringW& value);
  virtual err_t _removeAttribute(const ManagedStringW& name);

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;

  // --------------------------------------------------------------------------
  // [SVG Styles]
  // --------------------------------------------------------------------------

  virtual StringW getStyle(const StringW& name) const;
  virtual err_t setStyle(const StringW& name, const StringW& value);

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgStyleAttribute a_style;
  SvgTransformAttribute a_transform;

private:
  _FOG_NO_COPY(SvgStyledElement)
};

// ============================================================================
// [Fog::SvgRootElement]
// ============================================================================

struct FOG_NO_EXPORT SvgRootElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRootElement();
  virtual ~SvgRootElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;
  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  SizeF getRootSize() const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgViewBoxAttribute a_viewBox;

  // TODO: Rendering Model.
  // TODO: ViewBox.
  // TODO: PreserveAspectRatio.

private:
  _FOG_NO_COPY(SvgRootElement)
};

// ============================================================================
// [Fog::SvgSolidColorElement]
// ============================================================================

struct FOG_NO_EXPORT SvgSolidColorElement : public SvgStyledElement
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  typedef SvgStyledElement base;

  SvgSolidColorElement();
  virtual ~SvgSolidColorElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  // NONE

private:
  _FOG_NO_COPY(SvgSolidColorElement)
};

// ============================================================================
// [Fog::SvgAbstractGradientElement]
// ============================================================================

struct FOG_NO_EXPORT SvgAbstractGradientElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgAbstractGradientElement(const ManagedStringW& tagName, uint32_t svgType);
  virtual ~SvgAbstractGradientElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Visiting]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static void _walkAndAddColorStops(XmlElement* root, GradientF& gradient);

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgEnumAttribute a_spreadMethod;
  SvgEnumAttribute a_gradientUnits;
  SvgTransformAttribute a_gradientTransform;

private:
  _FOG_NO_COPY(SvgAbstractGradientElement)
};

// ============================================================================
// [Fog::SvgLinearGradientElement]
// ============================================================================

struct FOG_NO_EXPORT SvgLinearGradientElement : public SvgAbstractGradientElement
{
  typedef SvgAbstractGradientElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgLinearGradientElement();
  virtual ~SvgLinearGradientElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x1;
  SvgCoordAttribute a_y1;
  SvgCoordAttribute a_x2;
  SvgCoordAttribute a_y2;

private:
  _FOG_NO_COPY(SvgLinearGradientElement)
};

// ============================================================================
// [Fog::SvgRadialGradientElement]
// ============================================================================

struct FOG_NO_EXPORT SvgRadialGradientElement : public SvgAbstractGradientElement
{
  typedef SvgAbstractGradientElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRadialGradientElement();
  virtual ~SvgRadialGradientElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_cx;
  SvgCoordAttribute a_cy;
  SvgCoordAttribute a_fx;
  SvgCoordAttribute a_fy;
  SvgCoordAttribute a_r;

private:
  _FOG_NO_COPY(SvgRadialGradientElement)
};

// ============================================================================
// [Fog::SvgPatternElement]
// ============================================================================

struct FOG_NO_EXPORT SvgPatternElement : public SvgElement
{
  typedef SvgElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPatternElement();
  virtual ~SvgPatternElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const;

  err_t _createPattern(Pattern& pattern, SvgElement* obj) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgEnumAttribute a_patternUnits;
  SvgTransformAttribute a_patternTransform;
  SvgViewBoxAttribute a_viewBox;

private:
  _FOG_NO_COPY(SvgPatternElement)
};

// ============================================================================
// [Fog::SvgStopElement]
// ============================================================================

struct FOG_NO_EXPORT SvgStopElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgStopElement();
  virtual ~SvgStopElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgOffsetAttribute a_offset;

private:
  _FOG_NO_COPY(SvgStopElement)
};

// ============================================================================
// [Fog::SvgDefsElement]
// ============================================================================

struct FOG_NO_EXPORT SvgDefsElement : public SvgElement
{
  typedef SvgElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgDefsElement();
  virtual ~SvgDefsElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;

private:
  _FOG_NO_COPY(SvgDefsElement)
};

// ============================================================================
// [Fog::SvgGElement]
// ============================================================================

struct FOG_NO_EXPORT SvgGElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgGElement();
  virtual ~SvgGElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

private:
  _FOG_NO_COPY(SvgGElement)
};

// ============================================================================
// [Fog::SvgSymbolElement]
// ============================================================================

struct FOG_NO_EXPORT SvgSymbolElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgSymbolElement();
  virtual ~SvgSymbolElement();

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

private:
  _FOG_NO_COPY(SvgSymbolElement)
};

// ============================================================================
// [Fog::SvgUseElement]
// ============================================================================

struct FOG_NO_EXPORT SvgUseElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgUseElement();
  virtual ~SvgUseElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;
  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;

private:
  _FOG_NO_COPY(SvgUseElement)
};

// ============================================================================
// [Fog::SvgViewElement]
// ============================================================================

struct FOG_NO_EXPORT SvgViewElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgViewElement();
  virtual ~SvgViewElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgViewBoxAttribute a_viewBox;
  // TODO: PreserveAspectRatio.

private:
  _FOG_NO_COPY(SvgViewElement)
};

// ============================================================================
// [Fog::SvgAElement]
// ============================================================================

struct FOG_NO_EXPORT SvgAElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgAElement();
  virtual ~SvgAElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;

private:
  _FOG_NO_COPY(SvgAElement)
};

// ============================================================================
// [Fog::SvgCircleElement]
// ============================================================================

struct FOG_NO_EXPORT SvgCircleElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgCircleElement();
  virtual ~SvgCircleElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_cx;
  SvgCoordAttribute a_cy;
  SvgCoordAttribute a_r;

private:
  _FOG_NO_COPY(SvgCircleElement)
};

// ============================================================================
// [Fog::SvgEllipseElement]
// ============================================================================

struct FOG_NO_EXPORT SvgEllipseElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgEllipseElement();
  virtual ~SvgEllipseElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_cx;
  SvgCoordAttribute a_cy;
  SvgCoordAttribute a_rx;
  SvgCoordAttribute a_ry;

private:
  _FOG_NO_COPY(SvgEllipseElement)
};

// ============================================================================
// [Fog::SvgImageElement]
// ============================================================================

struct FOG_NO_EXPORT SvgImageElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgImageElement();
  virtual ~SvgImageElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;
  virtual err_t onProcess(SvgVisitor* visitor) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgImageLinkAttribute a_href;

private:
  _FOG_NO_COPY(SvgImageElement)
};

// ============================================================================
// [Fog::SvgLineElement]
// ============================================================================

struct FOG_NO_EXPORT SvgLineElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgLineElement();
  virtual ~SvgLineElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x1;
  SvgCoordAttribute a_y1;
  SvgCoordAttribute a_x2;
  SvgCoordAttribute a_y2;

private:
  _FOG_NO_COPY(SvgLineElement)
};

// ============================================================================
// [Fog::SvgPathElement]
// ============================================================================

struct FOG_NO_EXPORT SvgPathElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPathElement();
  virtual ~SvgPathElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgPathAttribute a_d;

private:
  _FOG_NO_COPY(SvgPathElement)
};

// ============================================================================
// [Fog::SvgPolygonElement]
// ============================================================================

struct FOG_NO_EXPORT SvgPolygonElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPolygonElement();
  virtual ~SvgPolygonElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgPointsAttribute a_points;

private:
  _FOG_NO_COPY(SvgPolygonElement)
};

// ============================================================================
// [Fog::SvgPolylineElement]
// ============================================================================

struct FOG_NO_EXPORT SvgPolylineElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPolylineElement();
  virtual ~SvgPolylineElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgPointsAttribute a_points;

private:
  _FOG_NO_COPY(SvgPolylineElement)
};

// ============================================================================
// [Fog::SvgRectElement]
// ============================================================================

struct FOG_NO_EXPORT SvgRectElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgRectElement();
  virtual ~SvgRectElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgCoordAttribute a_rx;
  SvgCoordAttribute a_ry;

private:
  _FOG_NO_COPY(SvgRectElement)
};

// ============================================================================
// [Fog::SvgTextElement]
// ============================================================================

struct FOG_NO_EXPORT SvgTextElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgTextElement();
  virtual ~SvgTextElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;
  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;

  // TODO: Ignored.
  SvgCoordAttribute a_dx;
  // TODO: Ignored.
  SvgCoordAttribute a_dy;
  // TODO: Ignored.
  SvgCoordAttribute a_textLength;

private:
  _FOG_NO_COPY(SvgTextElement)
};

// ============================================================================
// [Fog::SvgTSpanElement]
// ============================================================================

struct FOG_NO_EXPORT SvgTSpanElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgTSpanElement();
  virtual ~SvgTSpanElement();

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;
  virtual err_t onProcess(SvgVisitor* visitor) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;

  // TODO: Ignored.
  SvgCoordAttribute a_dx;
  // TODO: Ignored.
  SvgCoordAttribute a_dy;
  // TODO: Ignored.
  SvgCoordAttribute a_textLength;
  // TODO: Ignored.
  SvgEnumAttribute a_lengthAdjust;

private:
  _FOG_NO_COPY(SvgTSpanElement)
};


//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SVG_SVGDOM_P_H
