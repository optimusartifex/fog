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
#include <Fog/Svg/Dom/SvgPathAttribute_p.h>
#include <Fog/Svg/Tools/SvgUtil.h>

namespace Fog {

// ============================================================================
// [Fog::SvgPathAttribute]
// ============================================================================

SvgPathAttribute::SvgPathAttribute(XmlElement* element, const ManagedString& name, int offset) :
  XmlAttribute(element, name, offset)
{
}

SvgPathAttribute::~SvgPathAttribute()
{
}

err_t SvgPathAttribute::setValue(const StringW& value)
{
  FOG_RETURN_ON_ERROR(_value.set(value));
  FOG_RETURN_ON_ERROR(SvgUtil::parsePath(_path, value));

  // Build path-info to optimize path operations.
  _path.buildPathInfo();

  if (_element != NULL)
    reinterpret_cast<SvgElement*>(_element)->_boundingBoxDirty = true;

  return ERR_OK;
}

} // Fog namespace
