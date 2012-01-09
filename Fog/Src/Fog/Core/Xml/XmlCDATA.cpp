// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Xml/XmlCDATA.h>

namespace Fog {

// ============================================================================
// [Fog::XmlCDATA]
// ============================================================================

XmlCDATA::XmlCDATA(const StringW& data) :
  XmlNoText(FOG_STR_(XML__cdata)),
  _data(data)
{
  _type = XML_ELEMENT_CDATA;
  _flags &= ~(XML_ALLOWED_TAG | XML_ALLOWED_ATTRIBUTES);
}

XmlCDATA::~XmlCDATA()
{
}

XmlElement* XmlCDATA::clone() const
{
  return fog_new XmlCDATA(_data);
}

const StringW& XmlCDATA::getData() const
{
  FOG_ASSERT(getType() == XML_ELEMENT_CDATA);
  return _data;
}

err_t XmlCDATA::setData(const StringW& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_CDATA);
  return _data.set(data);
}

} // Fog namespace
