// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGSTYLEATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGSTYLEATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlAttribute.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/G2d/Geometry/Coord.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGSTYLEATTRIBUTE_P_H
