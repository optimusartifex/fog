// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGSTYLEATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGSTYLEATTRIBUTE_P_H

// [Dependencies]
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/Xml/Dom/XmlAttribute.h>
#include <Fog/Xml/Dom/XmlElement.h>
#include <Fog/Svg/Global/Constants.h>
#include <Fog/Svg/Tools/SvgCoord.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgStyleAttribute]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
struct FOG_NO_EXPORT SvgStyleAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgStyleAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgStyleAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual String getValue() const;
  virtual err_t setValue(const String& value);

  // --------------------------------------------------------------------------
  // [Getters]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getMask() const { return _mask; }
  FOG_INLINE bool hasStyle(int id) const { return (_mask & (1 << id)) != 0; }

  // --------------------------------------------------------------------------
  // [Style]
  // --------------------------------------------------------------------------

  String getStyle(const ManagedString& name) const;
  err_t setStyle(const ManagedString& name, const String& value);

  String getStyle(int styleId) const;
  err_t setStyle(int styleId, const String& value);

  static int styleToId(const ManagedString& name);

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
  uint8_t _reserved[2];

  Color _fillColor;
  Color _strokeColor;
  Color _stopColor;

  float _opacity;
  float _fillOpacity;
  float _strokeOpacity;
  float _stopOpacity;

  SvgCoord _strokeDashOffset;
  SvgCoord _strokeMiterLimit;
  SvgCoord _strokeWidth;
  SvgCoord _fontSize;
  SvgCoord _letterSpacing;

  PathF _clipPath;
  List<float> _dashArray;

  String _fillUri;
  String _strokeUri;
  String _fontFamily;

private:
  FOG_DISABLE_COPY(SvgStyleAttribute)
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGSTYLEATTRIBUTE_P_H
