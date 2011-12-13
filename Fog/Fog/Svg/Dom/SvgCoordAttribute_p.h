// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGCOORDATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGCOORDATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlAttribute.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/G2d/Geometry/Coord.h>
#include <Fog/Svg/Dom/SvgDocument.h>

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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGCOORDATTRIBUTE_P_H
