// [Fog-Xml]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_DOM_XMLIDATTRIBUTE_P_H
#define _FOG_XML_DOM_XMLIDATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Xml/Dom/XmlAttribute.h>

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

  XmlIdAttribute(XmlElement* element, const ManagedString& name);
  virtual ~XmlIdAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const String& value);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_XML_DOM_XMLIDATTRIBUTE_P_H
