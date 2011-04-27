// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGTRANSFORMATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGTRANSFORMATTRIBUTE_P_H

// [Dependencies]
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/Xml/Dom/XmlAttribute.h>
#include <Fog/Xml/Dom/XmlElement.h>
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgTransformAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgTransformAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgTransformAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgTransformAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const String& value);

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FOG_INLINE const TransformF& getTransform() const { return _transform; }
  FOG_INLINE bool isValid() const { return _isValid; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  TransformF _transform;
  bool _isValid;

private:
  FOG_DISABLE_COPY(SvgTransformAttribute)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGTRANSFORMATTRIBUTE_P_H
