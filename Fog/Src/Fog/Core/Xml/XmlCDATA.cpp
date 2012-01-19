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
  _nodeType = DOM_NODE_CDATA;
  _nodeFlags &= ~(DOM_FLAG_MUTABLE_NAME | DOM_FLAG_MUTABLE_ATTRIBUTES);
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
  FOG_ASSERT(getNodeType() == DOM_NODE_CDATA);

  return _data;
}

err_t XmlCDATA::setData(const StringW& data)
{
  FOG_ASSERT(getNodeType() == DOM_NODE_CDATA);

  return _data.set(data);
}

} // Fog namespace
