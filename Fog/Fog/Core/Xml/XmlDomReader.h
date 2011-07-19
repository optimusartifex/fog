// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLDOMREADER_H
#define _FOG_CORE_XML_XMLDOMREADER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlSaxReader.h>

namespace Fog {

//! @addtogroup Fog_Xml_IO
//! @{

// ============================================================================
// [Fog::XmlDomReader]
// ============================================================================

//! @brief Xml dom reader.
struct FOG_API XmlDomReader : public XmlSaxReader
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlDomReader(XmlDocument* document);
  virtual ~XmlDomReader();

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual err_t onAddElement(const Utf16& tagName);
  virtual err_t onCloseElement(const Utf16& tagName);
  virtual err_t onAddAttribute(const Utf16& name, const Utf16& value);

  virtual err_t onAddText(const Utf16& data, bool isWhiteSpace);
  virtual err_t onAddCDATA(const Utf16& data);
  virtual err_t onAddDOCTYPE(const List<String>& doctype);
  virtual err_t onAddPI(const Utf16& data);
  virtual err_t onAddComment(const Utf16& data);

  FOG_INLINE XmlDocument* getDocument() const { return _document; }

protected:
  XmlDocument* _document;
  XmlElement* _current;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLDOMREADER_H
