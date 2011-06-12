// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/Core/Xml/XmlPI.h>

namespace Fog {

// ============================================================================
// [Fog::XmlPI]
// ============================================================================

XmlPI::XmlPI(const String& data) :
  XmlNoText(fog_strings->getString(STR_XML__pi)),
  _data(data)
{
  _type = XML_ELEMENT_PI;
  _flags &= ~(XML_ALLOWED_TAG | XML_ALLOWED_ATTRIBUTES);
}

XmlPI::~XmlPI()
{
}

XmlElement* XmlPI::clone() const
{
  return fog_new XmlPI(_data);
}

const String& XmlPI::getData() const
{
  FOG_ASSERT(getType() == XML_ELEMENT_PI);
  return _data;
}

err_t XmlPI::setData(const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_PI);
  return _data.set(data);
}

} // Fog namespace
