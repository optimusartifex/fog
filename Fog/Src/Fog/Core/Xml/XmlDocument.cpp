// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Xml/XmlDocument.h>
#include <Fog/Core/Xml/XmlDomReader.h>

namespace Fog {

// ============================================================================
// [Fog::XmlDocument]
// ============================================================================

XmlDocument::XmlDocument() :
  XmlElement(FOG_STR_(XML__document)),
  _documentRoot(NULL)
{
  _nodeType = DOM_NODE_DOCUMENT;
  _nodeFlags &= ~(DOM_FLAG_MUTABLE_DOM | DOM_FLAG_MUTABLE_NAME);

  // Link to self.
  _document = this;
}

XmlDocument::~XmlDocument()
{
  // Here is important to release all managed resources. Normally this is done
  // in XmlElement destructor, but if we go there, the managed hash table will
  // not exist at this time and segfault will occur. So destroy everything here.
  _removeAttributes();
  deleteAll();

  // And clear _document pointer
  _document = NULL;
}

XmlElement* XmlDocument::clone() const
{
  XmlDocument* doc = fog_new XmlDocument();
  if (!doc) return NULL;

  for (XmlElement* ch = getFirstChild(); ch; ch = ch->getNextSibling())
  {
    XmlElement* e = ch->clone();
    if (e && doc->appendChild(e) != ERR_OK) fog_delete(e);
  }

  return doc;
}

XmlElement* XmlDocument::createElement(const ManagedStringW& tagName)
{
  return createElementStatic(tagName);
}

XmlElement* XmlDocument::createElementStatic(const ManagedStringW& tagName)
{
  return fog_new XmlElement(tagName);
}

XmlDomReader* XmlDocument::createDomReader()
{
  return fog_new XmlDomReader(this);
}

err_t XmlDocument::setDocumentRoot(XmlElement* e)
{
  if (_firstChild) return ERR_XML_DOCUMENT_HAS_ALREADY_ROOT;
  return appendChild(e);
}

void XmlDocument::clear()
{
  deleteAll();
}

XmlElement* XmlDocument::getElementById(const StringW& id) const
{
  if (id.isEmpty()) return NULL;
  return _elementIdsHash.get(id);
}

XmlElement* XmlDocument::getElementById(const StubW& id) const
{
  const CharW* idData = id.getData();
  size_t idLen = id.getComputedLength();

  if (idLen == 0) return NULL;
  return _elementIdsHash.get(idData, idLen);
}

err_t XmlDocument::readFromFile(const StringW& fileName)
{
  clear();

  XmlDomReader* reader = createDomReader();
  if (FOG_IS_NULL(reader)) return ERR_RT_OUT_OF_MEMORY;

  err_t err = reader->parseFile(fileName);
  fog_delete(reader);
  return err;
}

err_t XmlDocument::readFromStream(Stream& stream)
{
  clear();

  XmlDomReader* reader = createDomReader();
  if (FOG_IS_NULL(reader)) return ERR_RT_OUT_OF_MEMORY;

  err_t err = reader->parseStream(stream);
  fog_delete(reader);
  return err;
}

err_t XmlDocument::readFromMemory(const void* mem, size_t size)
{
  clear();

  XmlDomReader* reader = createDomReader();
  if (FOG_IS_NULL(reader)) return ERR_RT_OUT_OF_MEMORY;

  err_t err = reader->parseMemory(mem, size);
  fog_delete(reader);
  return err;
}

err_t XmlDocument::readFromString(const StringW& str)
{
  clear();

  XmlDomReader* reader = createDomReader();
  if (FOG_IS_NULL(reader)) return ERR_RT_OUT_OF_MEMORY;

  err_t err = reader->parseString(str);
  fog_delete(reader);
  return err;
}

} // Fog namespace
