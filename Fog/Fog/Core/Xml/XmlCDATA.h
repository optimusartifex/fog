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

//! @addtogroup Fog_Core_Xml
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

  XmlCDATA(const StringW& data = StringW());
  virtual ~XmlCDATA();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [CDATA Specific]
  // --------------------------------------------------------------------------

  const StringW& getData() const;
  err_t setData(const StringW& data);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  StringW _data;

private:
  _FOG_NO_COPY(XmlCDATA)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLCDATA_H
