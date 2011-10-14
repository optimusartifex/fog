// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGENUMATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGENUMATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlAttribute.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/Svg/Dom/SvgEnumItem_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

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
protected:

  const SvgEnumItem* _enumItems;
  int _enumValue;

private:
  _FOG_NO_COPY(SvgEnumAttribute)
};
//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGENUMATTRIBUTE_P_H
