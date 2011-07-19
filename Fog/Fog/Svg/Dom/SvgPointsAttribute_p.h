// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGPOINTSATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGPOINTSATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlAttribute.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgPointsAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgPointsAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPointsAttribute(XmlElement* element, const ManagedString& name, bool closePath, int offset);
  virtual ~SvgPointsAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const String& value);

  // --------------------------------------------------------------------------
  // [Coords]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathF& getPath() const { return _path; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
protected:

  PathF _path;
  bool _closePath;

private:
  _FOG_CLASS_NO_COPY(SvgPointsAttribute)
};
//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGPOINTSATTRIBUTE_P_H
