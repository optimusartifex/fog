// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLTEXT_H
#define _FOG_CORE_XML_XMLTEXT_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Xml/XmlElement.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct XmlDocument;

// ============================================================================
// [Fog::XmlText]
// ============================================================================

//! @brief Xml text element.
struct FOG_API XmlText : public XmlElement
{
  typedef XmlElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlText(const String& data = String());
  virtual ~XmlText();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [Text Specific]
  // --------------------------------------------------------------------------

  virtual String getTextContent() const;
  virtual err_t setTextContent(const String& text);

  FOG_INLINE const String& getData() const { return _data; }
  err_t setData(const String& data);
  err_t appendData(const String& data);
  err_t deleteData();
  err_t insertData(size_t start, const String& data);
  err_t replaceData(const Range& range, const String& data);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  String _data;

private:
  friend struct XmlElement;
  friend struct XmlDocument;

  _FOG_CLASS_NO_COPY(XmlText)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLTEXT_H
