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
#include <Fog/Core/Xml/XmlText.h>

namespace Fog {

// ============================================================================
// [Fog::XmlText]
// ============================================================================

XmlText::XmlText(const StringW& data) :
  XmlElement(FOG_STR_(XML__text)),
  _data(data)
{
  _nodeType = DOM_NODE_TEXT;
  _nodeFlags &= ~(DOM_FLAG_MUTABLE_NAME | DOM_FLAG_MUTABLE_ATTRIBUTES);
}

XmlText::~XmlText()
{
}

XmlElement* XmlText::clone() const
{
  return fog_new XmlText(_data);
}

StringW XmlText::getTextContent() const
{
  FOG_ASSERT(getNodeType() == DOM_NODE_TEXT);
  return _data;
}

err_t XmlText::setTextContent(const StringW& text)
{
  FOG_ASSERT(getNodeType() == DOM_NODE_TEXT);
  return _data.set(text);
}

err_t XmlText::setData(const StringW& data)
{
  FOG_ASSERT(getNodeType() == DOM_NODE_TEXT);
  return _data.set(data);
}

err_t XmlText::appendData(const StringW& data)
{
  FOG_ASSERT(getNodeType() == DOM_NODE_TEXT);
  return _data.append(data);
}

err_t XmlText::deleteData()
{
  FOG_ASSERT(getNodeType() == DOM_NODE_TEXT);
  _data.clear();
  return ERR_OK;
}

err_t XmlText::insertData(size_t start, const StringW& data)
{
  FOG_ASSERT(getNodeType() == DOM_NODE_TEXT);
  return _data.insert(start, data);
}

err_t XmlText::replaceData(const Range& range, const StringW& data)
{
  FOG_ASSERT(getNodeType() == DOM_NODE_TEXT);
  return _data.replace(range, data);
}

} // Fog namespace
