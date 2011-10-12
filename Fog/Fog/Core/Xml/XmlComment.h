// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLCOMMENT_H
#define _FOG_CORE_XML_XMLCOMMENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
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

  XmlComment(const StringW& data = StringW());
  virtual ~XmlComment();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [Comment Specific]
  // --------------------------------------------------------------------------

  const StringW& getData() const;
  err_t setData(const StringW& data);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  StringW _data;

private:
  _FOG_NO_COPY(XmlComment)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLCOMMENT_H
