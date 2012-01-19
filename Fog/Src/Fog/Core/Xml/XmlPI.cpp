// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Xml/XmlPI.h>

namespace Fog {

// ============================================================================
// [Fog::XmlPI]
// ============================================================================

XmlPI::XmlPI(const StringW& data) :
  XmlNoText(FOG_STR_(XML__pi)),
  _data(data)
{
  _nodeType = DOM_NODE_PROCESSING_INSTRUCTION;
  _nodeFlags &= ~(DOM_FLAG_MUTABLE_NAME | DOM_FLAG_MUTABLE_ATTRIBUTES);
}

XmlPI::~XmlPI()
{
}

XmlElement* XmlPI::clone() const
{
  return fog_new XmlPI(_data);
}

const StringW& XmlPI::getData() const
{
  FOG_ASSERT(getNodeType() == DOM_NODE_PROCESSING_INSTRUCTION);

  return _data;
}

err_t XmlPI::setData(const StringW& data)
{
  FOG_ASSERT(getNodeType() == DOM_NODE_PROCESSING_INSTRUCTION);

  return _data.set(data);
}

} // Fog namespace
