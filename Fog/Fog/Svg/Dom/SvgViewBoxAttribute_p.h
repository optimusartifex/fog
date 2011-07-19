// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGVIEWBOXATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGVIEWBOXATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlAttribute.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/Svg/Global/Constants.h>
#include <Fog/Svg/Tools/SvgCoord.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgCoordAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgViewBoxAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgViewBoxAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgViewBoxAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const String& value);

  // --------------------------------------------------------------------------
  // [Box]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxF& getBox() const { return _box; }
  FOG_INLINE bool isValid() const { return _isValid; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  BoxF _box;
  bool _isValid;

private:
  _FOG_CLASS_NO_COPY(SvgViewBoxAttribute)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGVIEWBOXATTRIBUTE_P_H
