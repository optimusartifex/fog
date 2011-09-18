// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Svg/Dom/SvgEnumAttribute_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgEnumAttribute]
// ============================================================================

SvgEnumAttribute::SvgEnumAttribute(XmlElement* element, const ManagedString& name, int offset, const SvgEnumItem* items) :
  XmlAttribute(element, name, offset),
  _enumItems(items),
  _enumValue(-1)
{
}

SvgEnumAttribute::~SvgEnumAttribute()
{
}

err_t SvgEnumAttribute::setValue(const StringW& value)
{
  err_t err = _value.set(value);
  if (FOG_IS_ERROR(err)) return err;

  _enumValue = svgGetEnumId(value, _enumItems);
  return ERR_OK;
}

} // Fog namespace
