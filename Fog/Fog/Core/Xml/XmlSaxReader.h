// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLSAXREADER_H
#define _FOG_CORE_XML_XMLSAXREADER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/TextCodec.h>

namespace Fog {

//! @addtogroup Fog_Xml_IO
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct XmlDocument;
struct XmlElement;

// ============================================================================
// [Fog::XmlSaxReader]
// ============================================================================

//! @brief Xml reader.
struct FOG_API XmlSaxReader
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlSaxReader();
  virtual ~XmlSaxReader();

  // --------------------------------------------------------------------------
  // [Parser]
  // --------------------------------------------------------------------------

  err_t parseFile(const String& fileName);
  err_t parseStream(Stream& stream);
  err_t parseMemory(const void* mem, size_t size);
  err_t parseString(const Char* str, size_t len);

  FOG_INLINE err_t parseString(const String& str)
  { return parseString(str.getData(), str.getLength()); }

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
  static void _detectEncoding(TextCodec& tc, const void* mem, size_t size);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLSAXREADER_H
