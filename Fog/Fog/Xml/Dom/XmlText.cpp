// [Fog-Xml]
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
#include <Fog/Xml/Dom/XmlText.h>

namespace Fog {

// ============================================================================
// [Fog::XmlText]
// ============================================================================

XmlText::XmlText(const String& data) :
  XmlElement(fog_strings->getString(STR_XML__text)),
  _data(data)
{
  _type = XML_ELEMENT_TEXT;
  _flags &= ~(XML_ALLOWED_TAG | XML_ALLOWED_ATTRIBUTES);
}

XmlText::~XmlText()
{
}

XmlElement* XmlText::clone() const
{
  return fog_new XmlText(_data);
}

String XmlText::getTextContent() const
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data;
}

err_t XmlText::setTextContent(const String& text)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.set(text);
}

err_t XmlText::setData(const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.set(data);
}

err_t XmlText::appendData(const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.append(data);
}

err_t XmlText::deleteData()
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  _data.clear();
  return ERR_OK;
}

err_t XmlText::insertData(size_t start, const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.insert(start, data);
}

err_t XmlText::replaceData(const Range& range, const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.replace(range, data);
}

} // Fog namespace
