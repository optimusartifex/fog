// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Svg/Dom/SvgViewBoxAttribute_p.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Tools/SvgUtil.h>

namespace Fog {

// ============================================================================
// [Fog::SvgViewBoxAttribute]
// ============================================================================

SvgViewBoxAttribute::SvgViewBoxAttribute(XmlElement* element, const ManagedString& name, int offset) :
  XmlAttribute(element, name, offset)
{
  _box.reset();
}

SvgViewBoxAttribute::~SvgViewBoxAttribute()
{
}

err_t SvgViewBoxAttribute::setValue(const String& value)
{
  err_t err = _value.set(value);
  if (FOG_IS_ERROR(err)) return err;

  SvgUtil::parseViewBox(_box, value);
  _isValid = _box.isValid();

  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingRectDirty = true;
  return ERR_OK;
}

} // Fog namespace
