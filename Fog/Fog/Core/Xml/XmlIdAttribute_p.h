// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLIDATTRIBUTE_P_H
#define _FOG_CORE_XML_XMLIDATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Core/Xml/XmlAttribute.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Fog::XmlIdAttribute]
// ============================================================================

//! @internal
//!
//! @brief The ID attribute.
struct FOG_NO_EXPORT XmlIdAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlIdAttribute(XmlElement* element, const ManagedStringW& name);
  virtual ~XmlIdAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const StringW& value);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLIDATTRIBUTE_P_H
