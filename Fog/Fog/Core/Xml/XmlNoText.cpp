// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Xml/XmlNoText.h>

namespace Fog {

// ============================================================================
// [Fog::XmlNoText]
// ============================================================================

XmlNoText::XmlNoText(const ManagedString& tagName) :
  XmlElement(tagName)
{
}

XmlNoText::~XmlNoText()
{
}

StringW XmlNoText::getTextContent() const
{
  return StringW();
}

err_t XmlNoText::setTextContent(const StringW& text)
{
  return ERR_XML_NOT_A_TEXT_NODE;
}

} // Fog namespace
