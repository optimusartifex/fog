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
#include <Fog/Core/Xml/XmlComment.h>

namespace Fog {

// ============================================================================
// [Fog::XmlComment]
// ============================================================================

XmlComment::XmlComment(const StringW& data) :
  XmlNoText(FOG_STR_(XML__comment)),
  _data(data)
{
  _nodeType = DOM_NODE_COMMENT;
  _nodeFlags &= ~(DOM_FLAG_MUTABLE_NAME | DOM_FLAG_MUTABLE_ATTRIBUTES);
}

XmlComment::~XmlComment()
{
}

XmlElement* XmlComment::clone() const
{
  return fog_new XmlComment(_data);
}

const StringW& XmlComment::getData() const
{
  FOG_ASSERT(getNodeType() == DOM_NODE_COMMENT);

  return _data;
}

err_t XmlComment::setData(const StringW& data)
{
  FOG_ASSERT(getNodeType() == DOM_NODE_COMMENT);

  return _data.set(data);
}

} // Fog namespace
