// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLPI_H
#define _FOG_CORE_XML_XMLPI_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlNoText.h>

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

  XmlPI(const StringW& data = StringW());
  virtual ~XmlPI();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [Processing Instruction Specific]
  // --------------------------------------------------------------------------

  const StringW& getData() const;
  err_t setData(const StringW& data);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  StringW _data;

private:
  _FOG_NO_COPY(XmlPI)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLPI_H
