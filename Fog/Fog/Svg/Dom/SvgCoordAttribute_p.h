// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGCOORDATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGCOORDATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Xml/Dom/XmlAttribute.h>
#include <Fog/Xml/Dom/XmlElement.h>
#include <Fog/Svg/Global/Constants.h>
#include <Fog/Svg/Tools/SvgCoord.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgCoordAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgCoordAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgCoordAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgCoordAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const String& value);

  // --------------------------------------------------------------------------
  // [Coords]
  // --------------------------------------------------------------------------

  FOG_INLINE const SvgCoord& getCoord() const { return _coord; }
  FOG_INLINE float getCoordValue() const { return _coord.value; };
  FOG_INLINE uint32_t getCoordUnit() const { return _coord.unit; };

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
protected:

  SvgCoord _coord;

private:
  FOG_DISABLE_COPY(SvgCoordAttribute)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGCOORDATTRIBUTE_P_H