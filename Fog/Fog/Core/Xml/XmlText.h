// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLTEXT_H
#define _FOG_CORE_XML_XMLTEXT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
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

  XmlText(const StringW& data = StringW());
  virtual ~XmlText();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [Text Specific]
  // --------------------------------------------------------------------------

  virtual StringW getTextContent() const;
  virtual err_t setTextContent(const StringW& text);

  FOG_INLINE const StringW& getData() const { return _data; }
  err_t setData(const StringW& data);
  err_t appendData(const StringW& data);
  err_t deleteData();
  err_t insertData(size_t start, const StringW& data);
  err_t replaceData(const Range& range, const StringW& data);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  StringW _data;

private:
  friend struct XmlElement;
  friend struct XmlDocument;

  _FOG_NO_COPY(XmlText)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLTEXT_H
