// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLDOCUMENT_H
#define _FOG_CORE_XML_XMLDOCUMENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/Core/Xml/XmlIdManager.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct XmlIdAttribute;
struct XmlDomReader;

// ============================================================================
// [Fog::XmlDocument]
// ============================================================================

//! @brief Xml document element.
struct FOG_API XmlDocument : public XmlElement
{
  typedef XmlElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlDocument();
  virtual ~XmlDocument();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [Root Node]
  // --------------------------------------------------------------------------

  //! @brief Set document root element to @a e.
  virtual err_t setDocumentRoot(XmlElement* e);

  //! @brief Return document root element.
  FOG_INLINE XmlElement* getDocumentRoot() const { return _documentRoot; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  virtual void clear();

  // --------------------------------------------------------------------------
  // [Document Extensions]
  // --------------------------------------------------------------------------

  virtual XmlElement* createElement(const ManagedString& tagName);
  static XmlElement* createElementStatic(const ManagedString& tagName);
  virtual XmlDomReader* createDomReader();

  // --------------------------------------------------------------------------
  // [Dom]
  // --------------------------------------------------------------------------

  XmlElement* getElementById(const StringW& id) const;
  XmlElement* getElementById(const StubW& id) const;

  // --------------------------------------------------------------------------
  // [Read]
  // --------------------------------------------------------------------------

  virtual err_t readFromFile(const StringW& fileName);
  virtual err_t readFromStream(Stream& stream);
  virtual err_t readFromMemory(const void* mem, size_t size);
  virtual err_t readFromString(const StringW& str);

  // --------------------------------------------------------------------------
  // [DOCTYPE]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getDOCTYPE() const { return _doctype; }
  FOG_INLINE err_t setDOCTYPE(const StringW& doctype) { return _doctype.set(doctype); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
protected:

  //! @brief Document root.
  XmlElement* _documentRoot;

  //! @brief Hash table that contains all managed strings and reference counts.
  Hash<StringW, size_t> _managedStrings;

  //! @brief Hash table that contains all managed IDs.
  XmlIdManager _elementIdsHash;

  //! @brief DOCTYPE string.
  StringW _doctype;

private:
  friend struct XmlAttribute;
  friend struct XmlIdAttribute;
  friend struct XmlElement;

  _FOG_NO_COPY(XmlDocument)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLDOCUMENT_H
