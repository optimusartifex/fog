// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Dom/SvgPointsAttribute_p.h>
#include <Fog/Svg/Tools/SvgUtil.h>

namespace Fog {

// ============================================================================
// [Fog::SvgPointsAttribute]
// ============================================================================

SvgPointsAttribute::SvgPointsAttribute(XmlElement* element, const ManagedString& name, bool closePath, int offset) :
  XmlAttribute(element, name, offset),
  _closePath(closePath)
{
}

SvgPointsAttribute::~SvgPointsAttribute()
{
}

err_t SvgPointsAttribute::setValue(const String& value)
{
  FOG_RETURN_ON_ERROR(_value.set(value));
  FOG_RETURN_ON_ERROR(SvgUtil::parsePoints(_path, value, _closePath));
  if (!_path.isEmpty() && _closePath) _path.close();

  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingRectDirty = true;
  return ERR_OK;
}
} // Fog namespace
