// [Fog-Xml]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_DOM_XMLPI_H
#define _FOG_XML_DOM_XMLPI_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Xml/Dom/XmlNoText.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Fog::XmlPI]
// ============================================================================

//! @brief Xml processing instructions.
struct FOG_API XmlPI : public XmlNoText
{
  typedef XmlNoText base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlPI(const String& data = String());
  virtual ~XmlPI();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [Processing Instruction Specific]
  // --------------------------------------------------------------------------

  const String& getData() const;
  err_t setData(const String& data);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  String _data;

private:
  friend struct XmlElement;
  friend struct XmlDocument;

  _FOG_CLASS_NO_COPY(XmlPI)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_XML_DOM_XMLPI_H
