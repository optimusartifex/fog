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
#include <Fog/Core/Xml/XmlDocument.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/Core/Xml/XmlIdAttribute_p.h>
#include <Fog/Core/Xml/XmlText.h>

namespace Fog {

// ============================================================================
// [Fog::XmlElement]
// ============================================================================

XmlElement::XmlElement(const ManagedStringW& tagName) :
  _type(XML_ELEMENT_BASE),
  _reserved0(0),
  _reserved1(0),
  _dirty(0),
  _flags(XML_ALLOWED_DOM_MANIPULATION | XML_ALLOWED_TAG | XML_ALLOWED_ATTRIBUTES),
  _document(NULL),
  _parent(NULL),
  _firstChild(NULL),
  _lastChild(NULL),
  _nextSibling(NULL),
  _prevSibling(NULL),
  _tagName(tagName),
  _hashNextId(NULL)
{
  if (_tagName.isEmpty()) _tagName = FOG_STR_(XML_unnamed);
}

XmlElement::~XmlElement()
{
  if (_firstChild) deleteAll();
  if (_parent) unlink();
  _removeAttributes();
}

void XmlElement::_manage(XmlDocument* doc)
{
  FOG_ASSERT(_document == NULL);

  _document = doc;
  if (!_id.isEmpty()) _document->_elementIdsHash.add(this);

  if (_document->_documentRoot == NULL && isElement())
  {
    _document->_documentRoot = this;
  }

  XmlElement* e = _firstChild;
  while (e)
  {
    e->_manage(doc);
    e = e->_nextSibling;
  }
}

void XmlElement::_unmanage()
{
  FOG_ASSERT(_document != NULL);

  if (_document->_documentRoot == this) _document->_documentRoot = NULL;

  if (!_id.isEmpty()) _document->_elementIdsHash.remove(this);

  XmlElement* e = _firstChild;
  while (e)
  {
    e->_unmanage();
    e = e->_nextSibling;
  }

  _document = NULL;
}

XmlElement* XmlElement::clone() const
{
  XmlElement* e = fog_new XmlElement(_tagName);
  if (e) _copyAttributes(e, const_cast<XmlElement*>(this));
  return e;
}

void XmlElement::serialize(XmlSaxWriter* writer)
{
}

void XmlElement::normalize()
{
  // Normalizing means to remove empty text nodes and join siblings.
  XmlElement* e = _firstChild;
  XmlText* text = NULL;

  while (e)
  {
    XmlElement* next = e->_nextSibling;

    if (e->getType() == XML_ELEMENT_TEXT)
    {
      if (((XmlText *)e)->_data.isEmpty())
      {
        // Remove empty text node.
        deleteChild(e);
      }
      else if (text != NULL)
      {
        // text is not NULL, this means that we have previous text node, so
        // we will join there this text node.
        text->appendData(((XmlText*)e)->_data);
        deleteChild(e);
      }
      else
      {
        text = (XmlText*)e;
      }
    }
    else
    {
      e->normalize();
      text = NULL;
    }

    e = next;
  }
}

err_t XmlElement::prependChild(XmlElement* ch)
{
  if (ch->contains(this, true)) return ERR_XML_CYCLIC;

  if (_document == ch->_document)
    FOG_RETURN_ON_ERROR(ch->_unlinkUnmanaged());
  else if (ch->_parent)
    FOG_RETURN_ON_ERROR(ch->unlink());

  ch->_parent = this;
  ch->_nextSibling = _firstChild;

  if (!hasChildNodes())
  {
    _firstChild = ch;
    _lastChild = ch;
  }
  else
  {
    _firstChild->_prevSibling = ch;
    _firstChild = ch;
  }

  _dirty = true;

  if ( _document)
  {
    if (_document != ch->_document) ch->_manage(_document);
  }
  return ERR_OK;
}

err_t XmlElement::appendChild(XmlElement* ch)
{
  if (ch->contains(this, true)) return ERR_XML_CYCLIC;

  err_t err = ERR_OK;
  if (_document == ch->_document)
    err = ch->_unlinkUnmanaged();
  else if (ch->_parent)
    err = ch->unlink();

  if (FOG_IS_ERROR(err)) return err;

  ch->_parent = this;
  ch->_prevSibling = _lastChild;

  if (!hasChildNodes())
  {
    _firstChild = ch;
    _lastChild = ch;
  }
  else
  {
    _lastChild->_nextSibling = ch;
    _lastChild = ch;
  }

  _dirty = true;

  if ( _document)
  {
    if (_document != ch->_document) ch->_manage(_document);
  }
  return ERR_OK;
}

err_t XmlElement::removeChild(XmlElement* ch)
{
  if (ch->_parent == this)
    return ch->unlink();
  else
    return ERR_XML_INVALID_CHILD;
}

err_t XmlElement::replaceChild(XmlElement* newch, XmlElement* oldch)
{
  if (oldch == NULL) return appendChild(newch);

  if (oldch->_parent != this) return ERR_XML_INVALID_CHILD;
  if ((oldch->_flags & XML_ALLOWED_DOM_MANIPULATION) == 0) return ERR_XML_MANUPULATION_NOT_ALLOWED;
  if (newch->contains(this, true)) return ERR_XML_CYCLIC;

  err_t err = ERR_OK;
  if (_document == newch->_document)
    err = newch->_unlinkUnmanaged();
  else if (newch->_parent)
    err = newch->unlink();
  if (FOG_IS_ERROR(err)) return err;

  newch->_parent = this;
  newch->_prevSibling = oldch->_prevSibling;
  newch->_nextSibling = oldch->_nextSibling;

  oldch->_unmanage();
  oldch->_parent = NULL;
  oldch->_prevSibling = NULL;
  oldch->_nextSibling = NULL;

  if (newch->_prevSibling == NULL)
    _firstChild = newch;
  if (newch->_nextSibling == NULL)
    _lastChild = newch;

  if ( _document)
  {
    if (_document != newch->_document) newch->_manage(_document);
  }
  return ERR_OK;
}

err_t XmlElement::deleteChild(XmlElement* ch)
{
  if (ch->_parent == this)
  {
    err_t err = ch->unlink();
    if (FOG_IS_ERROR(err)) return err;

    fog_delete(ch);
    return ERR_OK;
  }
  else
    return ERR_XML_INVALID_CHILD;
}

err_t XmlElement::deleteAll()
{
  XmlElement* e = _firstChild;
  if (e == NULL) return ERR_OK;

  do {
    XmlElement* next = e->_nextSibling;

    if (e->_document) e->_unmanage();
    e->_parent = NULL;
    e->_prevSibling = NULL;
    e->_nextSibling = NULL;
    fog_delete(e);

    e = next;
  } while (e);

  _firstChild = NULL;
  _lastChild = NULL;
  _dirty = 0;
  _children.reset();

  return ERR_OK;
}

err_t XmlElement::unlink()
{
  if (_parent == NULL) return ERR_OK;
  if ((_flags & XML_ALLOWED_DOM_MANIPULATION) == 0) return ERR_XML_MANUPULATION_NOT_ALLOWED;

  if (_document) _unmanage();
  return _unlinkUnmanaged();
}

err_t XmlElement::_unlinkUnmanaged()
{
  if (_parent == NULL) return ERR_OK;
  if ((_flags & XML_ALLOWED_DOM_MANIPULATION) == 0) return ERR_XML_MANUPULATION_NOT_ALLOWED;

  XmlElement* next = _nextSibling;
  XmlElement* prev = _prevSibling;

  if (prev)
    prev->_nextSibling = next;
  else
    _parent->_firstChild = next;

  if (next)
    next->_prevSibling = prev;
  else
    _parent->_lastChild = prev;

  _parent->_dirty = true;

  _parent = NULL;
  _prevSibling = NULL;
  _nextSibling = NULL;

  return ERR_OK;
}

bool XmlElement::contains(XmlElement* e, bool deep)
{
  if (!deep) return e->_parent == this;

  XmlElement* cur = this;
  do {
    if (cur == e) return true;
    cur = cur->_parent;
  } while (cur);
  return false;
}

List<XmlElement*> XmlElement::getChildNodes() const
{
  if (_dirty)
  {
    _children.clear();
    XmlElement* e = _firstChild;
    while (e)
    {
      _children.append(e);
      e = e->_nextSibling;
    }
    _dirty = false;
  }

  return _children;
}

List<XmlElement*> XmlElement::getChildNodesByTagName(const StringW& tagName) const
{
  List<XmlElement*> elms;
  ManagedStringW tagNameM(tagName, MANAGED_STRING_OPTION_LOOKUP);

  if (tagNameM.isEmpty())
    return elms;

  for (XmlElement* e = getFirstChild(); e; e = e->getNextSibling())
  {
    if (e->_tagName == tagNameM) elms.append(e);
  }

  return elms;
}

XmlElement* XmlElement::_nextChildByTagName(XmlElement* refElement, const StringW& tagName)
{
  XmlElement* e = refElement;
  if (e == NULL) return e;

  while ((e = e->getNextSibling()))
  {
    if (e->_tagName == tagName) break;
  }
  return e;
}

XmlElement* XmlElement::_previousChildByTagName(XmlElement* refElement, const StringW& tagName)
{
  XmlElement* e = refElement;
  if (e == NULL) return e;

  while ((e = e->getPreviousSibling()))
  {
    if (e->_tagName == tagName) break;
  }
  return e;
}

List<XmlAttribute*> XmlElement::attributes() const
{
  return _attributes;
}

bool XmlElement::hasAttribute(const StringW& name) const
{
  size_t length = _attributes.getLength();
  if (length == 0)
    return false;

  ManagedStringW m_name(name, MANAGED_STRING_OPTION_LOOKUP);
  if (m_name.isEmpty())
    return false;

  XmlAttribute** attrs = (XmlAttribute**)_attributes.getData();
  for (size_t i = 0; i < length; i++)
  {
    if (attrs[i]->_name == m_name)
      return true;
  }

  return false;
}

err_t XmlElement::setAttribute(const StringW& name, const StringW& value)
{
  if ((_flags & XML_ALLOWED_ATTRIBUTES) == 0)
    return ERR_XML_ATTRIBUTES_NOT_ALLOWED;

  if (name.isEmpty())
    return ERR_XML_INVALID_ATTRIBUTE;

  ManagedStringW m_name(name);
  if (m_name.isEmpty())
    return ERR_RT_OUT_OF_MEMORY;

  return _setAttribute(m_name, value);
}

StringW XmlElement::getAttribute(const StringW& name) const
{
  ManagedStringW m_name(name, MANAGED_STRING_OPTION_LOOKUP);

  if (m_name.isEmpty())
    return StringW();

  return _getAttribute(m_name);
}

err_t XmlElement::removeAttribute(const StringW& name)
{
  if ((_flags & XML_ALLOWED_ATTRIBUTES) == 0)
    return ERR_XML_ATTRIBUTES_NOT_ALLOWED;

  ManagedStringW m_name(name, MANAGED_STRING_OPTION_LOOKUP);

  if (m_name.isEmpty())
    return ERR_XML_ATTRIBUTE_NOT_EXISTS;

  return _removeAttribute(m_name);
}

err_t XmlElement::removeAttributes()
{
  if ((_flags & XML_ALLOWED_ATTRIBUTES) == 0)
    return ERR_XML_ATTRIBUTES_NOT_ALLOWED;

  size_t i = 0;
  while (i < _attributes.getLength())
  {
    if (_removeAttribute(_attributes.getAt(i)->_name) != ERR_OK) i++;
  }

  return ERR_OK;
}

err_t XmlElement::setId(const StringW& id)
{
  return setAttribute(FOG_STR_(XML_ATTRIBUTE_id), id);
}

err_t XmlElement::setTagName(const StringW& name)
{
  if ((_flags & XML_ALLOWED_TAG) == 0)
    return ERR_XML_TAG_CHANGE_NOT_ALLOWED;

  if (name.isEmpty())
    return ERR_XML_INVALID_TAG_NAME;

  return _tagName.set(name);
}

StringW XmlElement::getTextContent() const
{
  // If we use the standard behavior that is in browsers, we should traverse
  // between all nodes and check for text content. I think this is right
  // behavior for us.

  // First try fast-path.
  if (_firstChild == _lastChild && _firstChild->getType() == XML_ELEMENT_TEXT)
  {
    return ((XmlText*)_firstChild)->_data;
  }
  else
  {
    StringW s;
    for (XmlElement* e = getFirstChild(); e; e = e->getNextSibling()) s.append(e->getTextContent());
    return s;
  }
}

err_t XmlElement::setTextContent(const StringW& text)
{
  // First try fast-path.
  if (_firstChild == _lastChild && _firstChild->getType() == XML_ELEMENT_TEXT)
  {
    ((XmlText*)_firstChild)->_data = text;
    return ERR_OK;
  }
  else
  {
    deleteAll();
    XmlText* e = fog_new XmlText(text);
    if (!e) return ERR_RT_OUT_OF_MEMORY;
    return appendChild(e);
  }
}

err_t XmlElement::_setAttribute(const ManagedStringW& name, const StringW& value)
{
  size_t i, len = _attributes.getLength();
  XmlAttribute** attrs = (XmlAttribute**)_attributes.getData();

  for (i = 0; i < len; i++)
  {
    if (attrs[i]->_name == name) return attrs[i]->setValue(value);
  }

  // Attribute not found, create the new one.
  XmlAttribute* a = _createAttribute(name);
  if (FOG_IS_NULL(a)) return ERR_RT_OUT_OF_MEMORY;

  a->_element = this;

  err_t err = _attributes.append(a);
  if (FOG_IS_ERROR(err))
  {
    a->destroy();
    return err;
  }

  return a->setValue(value);
}

StringW XmlElement::_getAttribute(const ManagedStringW& name) const
{
  size_t i, len = _attributes.getLength();

  XmlAttribute** attrs = (XmlAttribute**)_attributes.getData();
  for (i = 0; i < len; i++)
  {
    if (attrs[i]->_name == name) return attrs[i]->getValue();
  }

  return StringW();
}

err_t XmlElement::_removeAttribute(const ManagedStringW& name)
{
  size_t i, len = _attributes.getLength();
  XmlAttribute** attrs = (XmlAttribute**)_attributes.getData();

  for (i = 0; i < len; i++)
  {
    XmlAttribute* a = attrs[i];
    if (a->_name == name)
    {
      _attributes.removeAt(i);
      a->destroy();
      return ERR_OK;
    }
  }

  return ERR_XML_ATTRIBUTE_NOT_EXISTS;
}

err_t XmlElement::_removeAttributes()
{
  size_t i, len = _attributes.getLength();
  if (!len) return ERR_OK;

  List<XmlAttribute*> attributes = _attributes;
  _attributes.reset();

  XmlAttribute** attrs = (XmlAttribute**)attributes.getData();
  for (i = 0; i < len; i++)
  {
    XmlAttribute* a = attrs[i];
    a->destroy();
  }

  return ERR_OK;
}

XmlAttribute* XmlElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(XML_ATTRIBUTE_id))
    return fog_new XmlIdAttribute(const_cast<XmlElement*>(this), name);
  else
    return fog_new XmlAttribute(const_cast<XmlElement*>(this), name);
}

void XmlElement::_copyAttributes(XmlElement* dst, XmlElement* src)
{
  size_t i, len = src->_attributes.getLength();
  XmlAttribute** attrs = (XmlAttribute**)src->_attributes.getData();
  for (i = 0; i < len; i++)
  {
    dst->setAttribute(attrs[i]->getName(), attrs[i]->getValue());
  }
}


} // Fog namespace
