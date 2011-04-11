// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Svg/Dom/SvgTransformAttribute_p.h>
#include <Fog/Svg/Tools/SvgUtil.h>

namespace Fog {

// ============================================================================
// [Fog::SvgTransformAttribute]
// ============================================================================

SvgTransformAttribute::SvgTransformAttribute(XmlElement* element, const ManagedString& name, int offset) :
    XmlAttribute(element, name, offset)
{
}

SvgTransformAttribute::~SvgTransformAttribute()
{
}

err_t SvgTransformAttribute::setValue(const String& value)
{
  err_t err = _value.set(value);
  if (FOG_IS_ERROR(err)) return err;

  _isValid = (SvgUtil::parseTransform(_transform, value) == ERR_OK);
  return ERR_OK;
}

} // Fog namespace
