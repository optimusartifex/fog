// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/IO/MapFile.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Tools/Byte.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Core/Xml/XmlCDATA.h>
#include <Fog/Core/Xml/XmlComment.h>
#include <Fog/Core/Xml/XmlDocument.h>
#include <Fog/Core/Xml/XmlDomReader.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/Core/Xml/XmlEntity_p.h>
#include <Fog/Core/Xml/XmlPI.h>
#include <Fog/Core/Xml/XmlText.h>

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

// Decode text that contains XML entities into plain form, for example
// &quot;Hello&quot will be decoded to "Hello".
static err_t xmlDecodeText(String& dst, const Utf16& src)
{
  size_t length = src.getLength();

  // Not allowed here.
  FOG_ASSERT(length != DETECT_LENGTH);

  dst.clear();
  if (length == 0) return ERR_OK;

  FOG_RETURN_ON_ERROR(dst.resize(length));

  Char* dstPtr = dst.getDataX();
  const Char* srcPtr = src.getData();
  const Char* srcEnd = srcPtr + length;

  do {
    if (srcPtr[0] != Char('&'))
    {
      dstPtr[0] = srcPtr[0];
      dstPtr++;
      srcPtr++;
    }
    else
    {
      const Char* mark = ++srcPtr;
      while (srcPtr != srcEnd && srcPtr[0] != Char(';')) srcPtr++;

      // TODO: Unterminated entity.
      if (srcPtr == srcEnd)
        break;

      dstPtr[0] = XmlEntity::decode(mark, (size_t)(srcPtr - mark));
      dstPtr++;
      srcPtr++;
    }
  } while (srcPtr != srcEnd);

  dst.finishDataX(dstPtr);
  return ERR_OK;
}

// ============================================================================
// [Fog::XmlDomReader]
// ============================================================================

XmlDomReader::XmlDomReader(XmlDocument* document) :
  _document(document),
  _current(document)
{
}

XmlDomReader::~XmlDomReader()
{
}

err_t XmlDomReader::onAddElement(const Utf16& tagName)
{
  XmlElement* e = _document->createElement(ManagedString(tagName));
  if (!e) return ERR_RT_OUT_OF_MEMORY;

  err_t err = _current->appendChild(e);
  if (FOG_IS_ERROR(err))
  {
    fog_delete(e);
    return err;
  }
  else
  {
    _current = e;
    return ERR_OK;
  }
}

err_t XmlDomReader::onCloseElement(const Utf16& tagName)
{
  if (_current != _document)
  {
    _current = _current->getParent();
    return ERR_OK;
  }
  else
  {
    return ERR_XML_INVALID_CLOSING_TAG;
  }
}

err_t XmlDomReader::onAddAttribute(const Utf16& name, const Utf16& value)
{
  String decodedValue;
  FOG_RETURN_ON_ERROR(xmlDecodeText(decodedValue, value));

  return _current->_setAttribute(ManagedString(name), decodedValue);
}

err_t XmlDomReader::onAddText(const Utf16& data, bool isWhiteSpace)
{
  String decodedData;
  FOG_RETURN_ON_ERROR(xmlDecodeText(decodedData, data));

  if (_current == _document)
  {
    if (isWhiteSpace)
      return ERR_OK;
    else
      return ERR_XML_SYNTAX_ERROR;
  }

  XmlElement* e = fog_new XmlText(decodedData);
  if (!e) return ERR_RT_OUT_OF_MEMORY;

  err_t err = _current->appendChild(e);
  if (FOG_IS_ERROR(err)) fog_delete(e);
  return err;
}

err_t XmlDomReader::onAddCDATA(const Utf16& data)
{
  if (_current == _document) return ERR_XML_DOCUMENT_INVALID_CHILD;

  XmlElement* e = fog_new XmlCDATA(String(data));
  if (!e) return ERR_RT_OUT_OF_MEMORY;

  err_t err = _current->appendChild(e);
  if (FOG_IS_ERROR(err)) fog_delete(e);
  return err;
}

err_t XmlDomReader::onAddDOCTYPE(const List<String>& doctype)
{
  if (_current != _document) return ERR_XML_DOCUMENT_INVALID_CHILD;

  // XML TODO:
  // return _document->setDOCTYPE(doctype);

  return ERR_OK;
}

err_t XmlDomReader::onAddPI(const Utf16& data)
{
  XmlElement* e = fog_new XmlPI(String(data));
  if (!e) return ERR_RT_OUT_OF_MEMORY;

  err_t err = _current->appendChild(e);
  if (FOG_IS_ERROR(err)) fog_delete(e);
  return err;
}

err_t XmlDomReader::onAddComment(const Utf16& data)
{
  XmlElement* e = fog_new XmlComment(String(data));
  if (!e) return ERR_RT_OUT_OF_MEMORY;

  err_t err = _current->appendChild(e);
  if (FOG_IS_ERROR(err)) fog_delete(e);
  return err;
}

} // Fog namespace
