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
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/Svg/Tools/SvgCoord.h>

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

  SvgStyleAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgStyleAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual StringW getValue() const;
  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Getters]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getMask() const { return _mask; }
  FOG_INLINE bool hasStyle(int id) const { return (_mask & (1 << id)) != 0; }

  // --------------------------------------------------------------------------
  // [Style]
  // --------------------------------------------------------------------------

  StringW getStyle(const ManagedString& name) const;
  err_t setStyle(const ManagedString& name, const StringW& value);

  StringW getStyle(int styleId) const;
  err_t setStyle(int styleId, const StringW& value);

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
