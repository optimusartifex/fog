// [Fog-Xml Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_XMLREADER_H
#define _FOG_XML_XMLREADER_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Xml/Constants.h>

namespace Fog {

//! @addtogroup Fog_Xml_IO
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct XmlDocument;
struct XmlElement;

// ============================================================================
// [Fog::XmlReader]
// ============================================================================

//! @brief Xml reader.
struct FOG_API XmlReader
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlReader();
  virtual ~XmlReader();

  // --------------------------------------------------------------------------
  // [Parser]
  // --------------------------------------------------------------------------

  err_t parseFile(const String& fileName);
  err_t parseStream(Stream& stream);
  err_t parseMemory(const void* mem, sysuint_t size);
  err_t parseString(const Char* str, sysuint_t len);

  FOG_INLINE err_t parseString(const String& str)
  { return parseString(str.getData(), str.getLength()); }

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  //! @brief Parser state.
  enum ParserState
  {
    //! @brief Parser is ready to parse (no errors, not inside tag declaration).
    StateReady = 0,
    //! @brief Parser is in tag < state.
    StateTagBegin,
    //! @brief Parser is in tag <[StartNameCharacter] state.
    StateTagName,
    //! @brief Parser is inside tag after name declaration.
    StateTagInside,
    //! @brief Parser is inside attribute name declaration.
    StateTagInsideAttrName,
    //! @brief Parser is inside attribute value declaration.
    StateTagInsideAttrValue,
    //! @brief Parser is inside tag and it's waiting for >.
    StateTagEnd,
    //! @brief Parser is in tag </ state.
    StateTagClose,
    //! @brief Parser is in tag </[StartNameCharacter] state.
    StateTagCloseName,
    //! @brief Parser is in tag </[StartNameCharacter][NameCharacter]*. state.
    StateTagCloseEnd,
    //! @brief Parser is in <? declaration.
    StateTagQuestionMark,
    //! @brief Parser is in <! declaration.
    StateTagExclamationMark,
    //! @brief Parser is in <!DOCTYPE declaration.
    StateDOCTYPE,
    //! @brief Parser is in <!DOCTYPE text
    StateDOCTYPEText,
    //! @brief Parser is in <!DOCTYPE attribute.
    StateDOCTYPEAttr,
    //! @brief Parser is in <? ?> or <% %> sequence.
    StatePI,
    //! @brief Parser is in <!-- --> sequence.
    StateComment,
    //! @brief Parser is in <![[CDATA ]]> sequence.
    StateCDATA
  };

  //! @brief Type of currently parsed element.
  enum ElementType
  {
    //! @brief Element is standard tag element.
    ElementTag = 0,
    //! @brief Self-closing element.
    ElementSelfClosingTag,
    //! @brief Element is XML declaration.
    ElementXML,
    //! @brief Element is CDATA declaration.
    ElementCDATA,
    //! @brief Element is DOCTYPE declaration.
    ElementDOCTYPE
  };

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual err_t onAddElement(const Utf16& tagName) = 0;
  virtual err_t onCloseElement(const Utf16& tagName) = 0;
  virtual err_t onAddAttribute(const Utf16& name, const Utf16& value) = 0;

  virtual err_t onAddText(const Utf16& data, bool isWhiteSpace) = 0;
  virtual err_t onAddCDATA(const Utf16& data) = 0;
  virtual err_t onAddDOCTYPE(const List<String>& doctype) = 0;
  virtual err_t onAddPI(const Utf16& data) = 0;
  virtual err_t onAddComment(const Utf16& data) = 0;

protected:
  static TextCodec _detectEncoding(const void* mem, sysuint_t size);
};

// ============================================================================
// [Fog::XmlDomReader]
// ============================================================================

//! @brief Xml dom reader.
struct FOG_API XmlDomReader : public XmlReader
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlDomReader(XmlDocument* document);
  virtual ~XmlDomReader();

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual err_t onAddElement(const Utf16& tagName);
  virtual err_t onCloseElement(const Utf16& tagName);
  virtual err_t onAddAttribute(const Utf16& name, const Utf16& value);

  virtual err_t onAddText(const Utf16& data, bool isWhiteSpace);
  virtual err_t onAddCDATA(const Utf16& data);
  virtual err_t onAddDOCTYPE(const List<String>& doctype);
  virtual err_t onAddPI(const Utf16& data);
  virtual err_t onAddComment(const Utf16& data);

  FOG_INLINE XmlDocument* getDocument() const { return _document; }

protected:
  XmlDocument* _document;
  XmlElement* _current;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_XML_XMLREADER_H
