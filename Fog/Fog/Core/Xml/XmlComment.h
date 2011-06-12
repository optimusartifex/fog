// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLCOMMENT_H
#define _FOG_CORE_XML_XMLCOMMENT_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Xml/XmlNoText.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Fog::XmlComment]
// ============================================================================

//! @brief Xml comment element.
struct FOG_API XmlComment : public XmlNoText
{
  typedef XmlNoText base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlComment(const String& data = String());
  virtual ~XmlComment();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [Comment Specific]
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

  _FOG_CLASS_NO_COPY(XmlComment)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLCOMMENT_H
