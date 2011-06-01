// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Svg/Dom/SvgCoordAttribute_p.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Tools/SvgUtil.h>

namespace Fog {

// ============================================================================
// [Fog::SvgCoordAttribute]
// ============================================================================

SvgCoordAttribute::SvgCoordAttribute(XmlElement* element, const ManagedString& name, int offset) :
  XmlAttribute(element, name, offset)
{
  _coord.value = 0.0;
  _coord.unit = UNIT_PX;
}

SvgCoordAttribute::~SvgCoordAttribute()
{
}

err_t SvgCoordAttribute::setValue(const String& value)
{
  err_t err = _value.set(value);
  if (FOG_IS_ERROR(err)) return err;

  SvgUtil::parseCoord(_coord, value);

  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingBoxDirty = true;
  return ERR_OK;
}

} // Fog namespace
