// [Fog-Xml]
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
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/IO/MapFile.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Tools/Byte.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Xml/Dom/XmlDocument.h>
#include <Fog/Xml/Dom/XmlElement.h>
#include <Fog/Xml/Dom/XmlCDATA.h>
#include <Fog/Xml/Dom/XmlComment.h>
#include <Fog/Xml/Dom/XmlPI.h>
#include <Fog/Xml/Dom/XmlText.h>
#include <Fog/Xml/Global/Constants.h>
#include <Fog/Xml/IO/XmlDomReader.h>

namespace Fog {

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
  return _current->_setAttribute(ManagedString(name), String(value));
}

err_t XmlDomReader::onAddText(const Utf16& data, bool isWhiteSpace)
{
  if (_current == _document)
  {
    if (isWhiteSpace)
      return ERR_OK;
    else
      return ERR_XML_SYNTAX_ERROR;
  }

  XmlElement* e = fog_new XmlText(String(data));
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
