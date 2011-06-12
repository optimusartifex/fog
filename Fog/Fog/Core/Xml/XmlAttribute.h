// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLATTRIBUTE_H
#define _FOG_CORE_XML_XMLATTRIBUTE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct XmlElement;

// ============================================================================
// [Fog::XmlAttribute]
// ============================================================================

//! @brief Xml attribute.
struct FOG_API XmlAttribute
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlAttribute(XmlElement* element, const ManagedString& name, int offset = -1);
  virtual ~XmlAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Return true whether attribute is assigned with element.
  //!
  //! This method is only usable for embedded attributes
  FOG_INLINE bool isAssigned() const { return _element != NULL; }

  FOG_INLINE XmlElement* getElement() const { return _element; }
  FOG_INLINE const String& getName() const { return _name.getString(); }

  virtual String getValue() const;
  virtual err_t setValue(const String& value);

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

protected:
  virtual void destroy();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to element that owns this attribute.
  XmlElement* _element;
  //! @brief Attribute name (managed string).
  ManagedString _name;
  //! @brief Attribute value (or empty if value is provided by overriden class).
  String _value;

  //! @brief Attribute offset in @c XmlElement (relative to @c XmlElement).
  //!
  //! If the attribute is not embedded in the element, the @c _offset value
  //! is set to -1.
  int _offset;

private:
  friend struct XmlElement;

  _FOG_CLASS_NO_COPY(XmlAttribute)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLATTRIBUTE_H
