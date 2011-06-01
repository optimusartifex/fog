// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGOFFSETATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGOFFSETATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Xml/Dom/XmlAttribute.h>
#include <Fog/Xml/Dom/XmlElement.h>
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgOffsetAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgOffsetAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgOffsetAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgOffsetAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const String& value);

  // --------------------------------------------------------------------------
  // [Coords]
  // --------------------------------------------------------------------------

  FOG_INLINE float getOffset() const { return _offset; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
protected:

  float _offset;

private:
  _FOG_CLASS_NO_COPY(SvgOffsetAttribute)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGOFFSETATTRIBUTE_P_H
