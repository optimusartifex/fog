// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Xml/XmlDocument.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/Core/Xml/XmlIdAttribute_p.h>

namespace Fog {

// ============================================================================
// [Fog::XmlIdAttribute]
// ============================================================================

XmlIdAttribute::XmlIdAttribute(XmlElement* element, const ManagedString& name)
  : XmlAttribute(element, name)
{
}

XmlIdAttribute::~XmlIdAttribute()
{
  XmlElement* element = _element;
  XmlDocument* document = element->getDocument();

  if (document && !element->_id.isEmpty()) document->_elementIdsHash.remove(element);
  element->_id.reset();
}

err_t XmlIdAttribute::setValue(const String& value)
{
  if (_value == value) return ERR_OK;

  XmlElement* element = _element;
  XmlDocument* document = element->getDocument();

  if (document && !element->_id.isEmpty()) document->_elementIdsHash.remove(element);

  // When assigning, we will generate hash code and we are omitting to call
  // this function from XmlIdManager.
  element->_id = value;
  element->_id.getHashCode();
  _value = value;

  if (document && !element->_id.isEmpty()) document->_elementIdsHash.add(element);
  return ERR_OK;
}

} // Fog namespace
