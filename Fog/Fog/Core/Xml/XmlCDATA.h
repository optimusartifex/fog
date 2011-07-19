// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLCDATA_H
#define _FOG_CORE_XML_XMLCDATA_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlNoText.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Fog::XmlCDATA]
// ============================================================================

//! @brief Xml CDATA section.
struct FOG_API XmlCDATA : public XmlNoText
{
  typedef XmlNoText base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlCDATA(const String& data = String());
  virtual ~XmlCDATA();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [CDATA Specific]
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

  _FOG_CLASS_NO_COPY(XmlCDATA)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLCDATA_H
