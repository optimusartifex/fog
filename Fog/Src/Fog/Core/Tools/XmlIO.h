// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_XMLIO_H
#define _FOG_CORE_TOOLS_XMLIO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/TextCodec.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::XmlSaxLocation]
// ============================================================================

//! @brief XML-SAX location.
struct FOG_NO_EXPORT XmlSaxLocation
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE XmlSaxLocation() :
    _line(0),
    _column(0)
  {
  }

  FOG_INLINE XmlSaxLocation(uint32_t line, uint32_t column) :
    _line(line),
    _column(column)
  {
  }

  explicit FOG_INLINE XmlSaxLocation(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getLine() const { return _line; }
  FOG_INLINE uint32_t getColumn() const { return _column; }

  FOG_INLINE void setLine(uint32_t line) { _line = line; }
  FOG_INLINE void setColumn(uint32_t column) { _column = column; }

  FOG_INLINE void setLocation(uint32_t line, uint32_t column)
  {
    _line = line;
    _column = column;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _line;
  uint32_t _column;
};

// ============================================================================
// [Fog::XmlSaxHandler]
// ============================================================================

//! @brief XML-SAX parser.
struct FOG_API XmlSaxHandler
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlSaxHandler();
  virtual ~XmlSaxHandler();

  // --------------------------------------------------------------------------
  // [SAX - Interface]
  // --------------------------------------------------------------------------

  virtual err_t onStartDocument() = 0;
  virtual err_t onEndDocument() = 0;

  virtual err_t onStartElement(const StubW& tagName) = 0;
  virtual err_t onEndElement(const StubW& tagName) = 0;

  virtual err_t onAttribute(const StubW& name, const StubW& value) = 0;

  virtual err_t onCharacterData(const StubW& data) = 0;
  virtual err_t onIgnorableWhitespace(const StubW& data) = 0;

  virtual err_t onDOCTYPE(const List<StringW>& doctype) = 0;
  virtual err_t onCDATASection(const StubW& data) = 0;
  virtual err_t onComment(const StubW& data) = 0;
  virtual err_t onProcessingInstruction(const StubW& target, const StubW& data) = 0;

  virtual err_t onError(const XmlSaxLocation& location, err_t errorCode) = 0;
  virtual err_t onFatal(const XmlSaxLocation& location, err_t errorCode) = 0;

private:
  _FOG_NO_COPY(XmlSaxHandler)
};

// ============================================================================
// [Fog::XmlSaxParser]
// ============================================================================

//! @brief XML-SAX reader.
struct FOG_API XmlSaxParser
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlSaxParser(XmlSaxHandler* handler = NULL);
  virtual ~XmlSaxParser();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get SAX handler.
  FOG_INLINE XmlSaxHandler* getHandler() const { return _handler; }
  //! @brief Set SAX handler, which will receive SAX events.
  FOG_INLINE void setHandler(XmlSaxHandler* handler) { _handler = handler; }

  // --------------------------------------------------------------------------
  // [Parse]
  // --------------------------------------------------------------------------

  err_t parseFile(const StringW& fileName);
  err_t parseStream(Stream& stream);
  err_t parseMemory(const void* mem, size_t size);
  err_t parseString(const StringW& str);
  err_t parseString(const StubW& str);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief SAX handler.
  XmlSaxHandler* _handler;

private:
  _FOG_NO_COPY(XmlSaxParser)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_XMLIO_H
