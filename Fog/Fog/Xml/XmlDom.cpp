// [Fog-Xml Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Hash.h>
#include <Fog/Core/List.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Xml/Constants.h>
#include <Fog/Xml/XmlDom.h>
#include <Fog/Xml/XmlEntity_p.h>
#include <Fog/Xml/XmlReader.h>
#include <Fog/Xml/XmlWriter.h>

namespace Fog {

// ============================================================================
// [Fog::XmlAttribute]
// ============================================================================

XmlAttribute::XmlAttribute(XmlElement* element, const ManagedString& name, int offset) :
  _element(element),
  _name(name),
  _offset(offset)
{
}

XmlAttribute::~XmlAttribute()
{
}

String XmlAttribute::getValue() const
{
  return _value;
}

err_t XmlAttribute::setValue(const String& value)
{
  return _value.set(value);
}

void XmlAttribute::destroy()
{
  if (_offset == -1)
  {
    fog_delete(this);
  }
  else
  {
    _value.free();
  }
}

// ============================================================================
// [Fog::XmlIdAttribute]
// ============================================================================

struct FOG_HIDDEN XmlIdAttribute : public XmlAttribute
{
  // [Construction / Destruction]

  XmlIdAttribute(XmlElement* element, const ManagedString& name);
  virtual ~XmlIdAttribute();

  // [Methods]

  virtual err_t setValue(const String& value);
};

XmlIdAttribute::XmlIdAttribute(XmlElement* element, const ManagedString& name)
  : XmlAttribute(element, name)
{
}

XmlIdAttribute::~XmlIdAttribute()
{
  XmlElement* element = _element;
  XmlDocument* document = element->getDocument();

  if (document && !element->_id.isEmpty()) document->_elementIdsHash.remove(element);
  element->_id.free();
}

err_t XmlIdAttribute::setValue(const String& value)
{
  if (_value == value) return ERR_OK;

  XmlElement* element = _element;
  XmlDocument* document = element->getDocument();

  if (document && !element->_id.isEmpty()) document->_elementIdsHash.remove(element);

  // When assigning, we will generate hash code and we are omitting to call
  // this function from XmlIdManager.
  element->_id = value;
  element->_id.getHashCode();
  _value = value;

  if (document && !element->_id.isEmpty()) document->_elementIdsHash.add(element);
  return ERR_OK;
}

// ============================================================================
// [Fog::XmlElement]
// ============================================================================

XmlElement::XmlElement(const ManagedString& tagName) :
  _type(XML_ELEMENT_BASE),
  _dirty(0),
  _reserved0(0),
  _reserved1(0),
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
  if (_tagName.isEmpty()) _tagName = fog_strings->getString(STR_XML_unnamed);
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

void XmlElement::serialize(XmlWriter* writer)
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

  err_t err = ERR_OK;
  if (_document == ch->_document)
    err = ch->_unlinkUnmanaged();
  else if (ch->_parent)
    err = ch->unlink();
  if (err) return err;

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

  if (err) return err;

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
  if (err) return err;

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
    if (err) return err;

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
  _children.free();

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

List<XmlElement*> XmlElement::childNodes() const
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

List<XmlElement*> XmlElement::childNodesByTagName(const String& tagName) const
{
  List<XmlElement*> elms;

  ManagedString tagNameM;
  if (tagNameM.setIfManaged(tagName) != ERR_OK) return elms;

  for (XmlElement* e = firstChild(); e; e = e->nextSibling())
  {
    if (e->_tagName == tagNameM) elms.append(e);
  }

  return elms;
}

XmlElement* XmlElement::_nextChildByTagName(XmlElement* refElement, const String& tagName)
{
  XmlElement* e = refElement;
  if (e == NULL) return e;

  while ((e = e->nextSibling()))
  {
    if (e->_tagName == tagName) break;
  }
  return e;
}

XmlElement* XmlElement::_previousChildByTagName(XmlElement* refElement, const String& tagName)
{
  XmlElement* e = refElement;
  if (e == NULL) return e;

  while ((e = e->previousSibling()))
  {
    if (e->_tagName == tagName) break;
  }
  return e;
}

List<XmlAttribute*> XmlElement::attributes() const
{
  return _attributes;
}

bool XmlElement::hasAttribute(const String& name) const
{
  sysuint_t i, len = _attributes.getLength();
  if (!len) return false;
  
  ManagedString managedName;
  if (managedName.setIfManaged(name) != ERR_OK) return false;

  XmlAttribute** attrs = (XmlAttribute**)_attributes.getData();
  for (i = 0; i < len; i++)
  {
    if (attrs[i]->_name == managedName) return true;
  }

  return false;
}

err_t XmlElement::setAttribute(const String& name, const String& value)
{
  if ((_flags & XML_ALLOWED_ATTRIBUTES) == 0) return ERR_XML_ATTRIBUTES_NOT_ALLOWED;
  if (name.isEmpty()) return ERR_XML_INVALID_ATTRIBUTE;

  ManagedString managedName;
  err_t err = managedName.set(name);
  if (err) return err;

  return _setAttribute(managedName, value);
}

String XmlElement::getAttribute(const String& name) const
{
  ManagedString managedName;
  if (managedName.setIfManaged(name) != ERR_OK) return String();

  return _getAttribute(managedName);
}

err_t XmlElement::removeAttribute(const String& name)
{
  if ((_flags & XML_ALLOWED_ATTRIBUTES) == 0) return ERR_XML_ATTRIBUTES_NOT_ALLOWED;

  ManagedString managedName;
  if (managedName.setIfManaged(name) != ERR_OK) return ERR_XML_ATTRIBUTE_NOT_EXISTS;

  return _removeAttribute(managedName);
}

err_t XmlElement::removeAttributes()
{
  if ((_flags & XML_ALLOWED_ATTRIBUTES) == 0) return ERR_XML_ATTRIBUTES_NOT_ALLOWED;

  sysuint_t i = 0;
  while (i < _attributes.getLength())
  {
    if (_removeAttribute(_attributes.at(i)->_name) != ERR_OK) i++;
  }

  return ERR_OK;
}

err_t XmlElement::setId(const String& id)
{
  return setAttribute(fog_strings->getString(STR_XML_id), id);
}

err_t XmlElement::setTagName(const String& name)
{
  if ((_flags & XML_ALLOWED_TAG) == 0) return ERR_XML_TAG_CHANGE_NOT_ALLOWED;
  if (name.isEmpty()) return ERR_XML_INVALID_TAG_NAME;

  return _tagName.set(name);
}

String XmlElement::getTextContent() const
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
    String s;
    for (XmlElement* e = firstChild(); e; e = e->nextSibling()) s.append(e->getTextContent());
    return s;
  }
}

err_t XmlElement::setTextContent(const String& text)
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

err_t XmlElement::_setAttribute(const ManagedString& name, const String& value)
{
  sysuint_t i, len = _attributes.getLength();
  XmlAttribute** attrs = (XmlAttribute**)_attributes.getData();

  for (i = 0; i < len; i++)
  {
    if (attrs[i]->_name == name) return attrs[i]->setValue(value);
  }

  // Attribute not found, create new one.
  XmlAttribute* a = _createAttribute(name);
  if (!a) return ERR_RT_OUT_OF_MEMORY;
  a->_element = this;

  err_t err = _attributes.append(a);
  if (err != ERR_OK)
  {
    a->destroy();
    return err;
  }

  return a->setValue(value);
}

String XmlElement::_getAttribute(const ManagedString& name) const
{
  sysuint_t i, len = _attributes.getLength();

  XmlAttribute** attrs = (XmlAttribute**)_attributes.getData();
  for (i = 0; i < len; i++)
  {
    if (attrs[i]->_name == name) return attrs[i]->getValue();
  }

  return String();
}

err_t XmlElement::_removeAttribute(const ManagedString& name)
{
  sysuint_t i, len = _attributes.getLength();
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
  sysuint_t i, len = _attributes.getLength();
  if (!len) return ERR_OK;

  List<XmlAttribute*> attributes = _attributes;
  _attributes.free();

  XmlAttribute** attrs = (XmlAttribute**)attributes.getData();
  for (i = 0; i < len; i++)
  {
    XmlAttribute* a = attrs[i];
    a->destroy();
  }

  return ERR_OK;
}

XmlAttribute* XmlElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_XML_id))
    return fog_new XmlIdAttribute(const_cast<XmlElement*>(this), name);
  else
    return fog_new XmlAttribute(const_cast<XmlElement*>(this), name);
}

void XmlElement::_copyAttributes(XmlElement* dst, XmlElement* src)
{
  sysuint_t i, len = src->_attributes.getLength();
  XmlAttribute** attrs = (XmlAttribute**)src->_attributes.getData();
  for (i = 0; i < len; i++)
  {
    dst->setAttribute(attrs[i]->getName(), attrs[i]->getValue());
  }
}

// ============================================================================
// [Fog::XmlText]
// ============================================================================

XmlText::XmlText(const String& data) :
  XmlElement(fog_strings->getString(STR_XML__text)),
  _data(data)
{
  _type = XML_ELEMENT_TEXT;
  _flags &= ~(XML_ALLOWED_TAG | XML_ALLOWED_ATTRIBUTES);
}

XmlText::~XmlText()
{
}

XmlElement* XmlText::clone() const
{
  return fog_new XmlText(_data);
}

String XmlText::getTextContent() const
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data;
}

err_t XmlText::setTextContent(const String& text)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.set(text);
}

err_t XmlText::setData(const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.set(data);
}

err_t XmlText::appendData(const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.append(data);
}

err_t XmlText::deleteData()
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  _data.clear();
  return ERR_OK;
}

err_t XmlText::insertData(sysuint_t start, const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.insert(start, data);
}

err_t XmlText::replaceData(const Range& range, const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_TEXT);
  return _data.replace(range, data);
}

// ============================================================================
// [Fog::XmlNoTextElement]
// ============================================================================

XmlNoTextElement::XmlNoTextElement(const ManagedString& tagName) :
  XmlElement(tagName)
{
}

String XmlNoTextElement::getTextContent() const
{
  return String();
}

err_t XmlNoTextElement::setTextContent(const String& text)
{
  return ERR_XML_NOT_A_TEXT_NODE;
}

// ============================================================================
// [Fog::XmlComment]
// ============================================================================

XmlComment::XmlComment(const String& data) :
  XmlNoTextElement(fog_strings->getString(STR_XML__comment)),
  _data(data)
{
  _type = XML_ELEMENT_COMMENT;
  _flags &= ~(XML_ALLOWED_TAG | XML_ALLOWED_ATTRIBUTES);
}

XmlComment::~XmlComment()
{
}

XmlElement* XmlComment::clone() const
{
  return fog_new XmlComment(_data);
}

const String& XmlComment::getData() const
{
  FOG_ASSERT(getType() == XML_ELEMENT_COMMENT);
  return _data;
}

err_t XmlComment::setData(const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_COMMENT);
  return _data.set(data);
}

// ============================================================================
// [Fog::XmlCDATA]
// ============================================================================

XmlCDATA::XmlCDATA(const String& data) :
  XmlNoTextElement(fog_strings->getString(STR_XML__cdata)),
  _data(data)
{
  _type = XML_ELEMENT_CDATA;
  _flags &= ~(XML_ALLOWED_TAG | XML_ALLOWED_ATTRIBUTES);
}

XmlCDATA::~XmlCDATA()
{
}

XmlElement* XmlCDATA::clone() const
{
  return fog_new XmlCDATA(_data);
}

const String& XmlCDATA::getData() const
{
  FOG_ASSERT(getType() == XML_ELEMENT_CDATA);
  return _data;
}

err_t XmlCDATA::setData(const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_CDATA);
  return _data.set(data);
}

// ============================================================================
// [Fog::XmlPI]
// ============================================================================

XmlPI::XmlPI(const String& data) :
  XmlNoTextElement(fog_strings->getString(STR_XML__pi)),
  _data(data)
{
  _type = XML_ELEMENT_PI;
  _flags &= ~(XML_ALLOWED_TAG | XML_ALLOWED_ATTRIBUTES);
}

XmlPI::~XmlPI()
{
}

XmlElement* XmlPI::clone() const
{
  return fog_new XmlPI(_data);
}

const String& XmlPI::getData() const
{
  FOG_ASSERT(getType() == XML_ELEMENT_PI);
  return _data;
}

err_t XmlPI::setData(const String& data)
{
  FOG_ASSERT(getType() == XML_ELEMENT_PI);
  return _data.set(data);
}

// ============================================================================
// [Fog::XmlIdManager]
// ============================================================================

XmlIdManager::XmlIdManager() :
  _capacity(16),
  _length(0),
  _expandCapacity(64),
  _expandLength(16),
  _shrinkCapacity(0),
  _shrinkLength(0),
  _buckets(_bucketsBuffer)
{
  Memory::zero(_bucketsBuffer, sizeof(_bucketsBuffer));
}

XmlIdManager::~XmlIdManager()
{
  if (_buckets != _bucketsBuffer) Memory::free(_buckets);
}

void XmlIdManager::add(XmlElement* e)
{
  uint32_t hashCode = e->_id._d->hashCode;
  uint32_t hashMod = hashCode % _capacity;

  XmlElement* node = _buckets[hashMod];
  XmlElement* prev = NULL;

  while (node)
  {
    prev = node;
    node = node->_hashNextId;
  }

  if (prev)
    prev->_hashNextId = e;
  else
    _buckets[hashMod] = e;
  if (++_length >= _expandLength) _rehash(_expandCapacity);
}

void XmlIdManager::remove(XmlElement* e)
{
  uint32_t hashCode = e->_id._d->hashCode;
  uint32_t hashMod = hashCode % _capacity;

  XmlElement* node = _buckets[hashMod];
  XmlElement* prev = NULL;

  while (node)
  {
    if (node == e)
    {
      if (prev)
        prev->_hashNextId = node->_hashNextId;
      else
        _buckets[hashMod] = node->_hashNextId;

      node->_hashNextId = NULL;
      if (--_length <= _shrinkLength) _rehash(_shrinkCapacity);
      return;
    }

    prev = node;
    node = node->_hashNextId;
  }
}

XmlElement* XmlIdManager::get(const String& id) const
{
  uint32_t hashCode = id.getHashCode();
  uint32_t hashMod = hashCode % _capacity;

  XmlElement* node = _buckets[hashMod];
  while (node)
  {
    if (node->_id == id) return node;
    node = node->_hashNextId;
  }
  return NULL;
}

XmlElement* XmlIdManager::get(const Char* idStr, sysuint_t idLen) const
{
  uint32_t hashCode = HashUtil::hashString(idStr, idLen);
  uint32_t hashMod = hashCode % _capacity;

  XmlElement* node = _buckets[hashMod];
  while (node)
  {
    if (node->_id._d->hashCode == hashCode && StringUtil::eq(node->_id.getData(), idStr, idLen)) return node;
    node = node->_hashNextId;
  }
  return NULL;
}

void XmlIdManager::_rehash(sysuint_t capacity)
{
  XmlElement** oldBuckets = _buckets;
  XmlElement** newBuckets = (XmlElement**)Memory::calloc(sizeof(XmlElement*) * capacity);
  if (!newBuckets) return;

  sysuint_t i, len = _capacity;
  for (i = 0; i < len; i++)
  {
    XmlElement* node = oldBuckets[i];
    while (node)
    {
      uint32_t hashMod = node->_id._d->hashCode % capacity;
      XmlElement* next = node->_hashNextId;

      XmlElement* newCur = newBuckets[hashMod];
      XmlElement* newPrev = NULL;
      while (newCur) { newPrev = newCur; newCur = newCur->_hashNextId; }

      if (newPrev)
        newPrev->_hashNextId = node;
      else
        newBuckets[hashMod] = node;
      node->_hashNextId = NULL;

      node = next;
    }
  }

  _capacity = capacity;

  _expandCapacity = UnorderedAbstract::_calcExpandCapacity(capacity);
  _expandLength = (sysuint_t)((sysint_t)_capacity * 0.92);

  _shrinkCapacity = UnorderedAbstract::_calcShrinkCapacity(capacity);
  _shrinkLength = (sysuint_t)((sysint_t)_shrinkCapacity * 0.70);

  atomicPtrXchg(&_buckets, newBuckets);
  if (oldBuckets != _bucketsBuffer) Memory::free(oldBuckets);
}

// ============================================================================
// [Fog::XmlDocument]
// ============================================================================

XmlDocument::XmlDocument() :
  XmlElement(fog_strings->getString(STR_XML__document)),
  _documentRoot(NULL)
{
  _type = XML_ELEMENT_DOCUMENT;
  _flags &= ~(XML_ALLOWED_DOM_MANIPULATION | XML_ALLOWED_TAG);

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

  for (XmlElement* ch = firstChild(); ch; ch = ch->nextSibling())
  {
    XmlElement* e = ch->clone();
    if (e && doc->appendChild(e) != ERR_OK) fog_delete(e);
  }

  return doc;
}

XmlElement* XmlDocument::createElement(const ManagedString& tagName)
{
  return createElementStatic(tagName);
}

XmlElement* XmlDocument::createElementStatic(const ManagedString& tagName)
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

XmlElement* XmlDocument::getElementById(const String& id) const
{
  if (id.isEmpty()) return NULL;
  return _elementIdsHash.get(id);
}

XmlElement* XmlDocument::getElementById(const Utf16& id) const
{
  const Char* idStr = id.getData();
  sysuint_t idLen = id.getLength();

  if (idLen == DETECT_LENGTH) idLen = StringUtil::len(idStr);
  if (idLen == 0) return NULL;

  return _elementIdsHash.get(idStr, idLen);
}

err_t XmlDocument::readFromFile(const String& fileName)
{
  clear();

  XmlDomReader* reader = createDomReader();
  if (!reader) return ERR_RT_OUT_OF_MEMORY;

  err_t err = reader->parseFile(fileName);
  fog_delete(reader);
  return err;
}

err_t XmlDocument::readFromStream(Stream& stream)
{
  clear();

  XmlDomReader* reader = createDomReader();
  if (!reader) return ERR_RT_OUT_OF_MEMORY;

  err_t err = reader->parseStream(stream);
  fog_delete(reader);
  return err;
}

err_t XmlDocument::readFromMemory(const void* mem, sysuint_t size)
{
  clear();

  XmlDomReader* reader = createDomReader();
  if (!reader) return ERR_RT_OUT_OF_MEMORY;

  err_t err = reader->parseMemory(mem, size);
  fog_delete(reader);
  return err;
}

err_t XmlDocument::readFromString(const String& str)
{
  clear();

  XmlDomReader* reader = createDomReader();
  if (!reader) return ERR_RT_OUT_OF_MEMORY;

  err_t err = reader->parseString(str);
  fog_delete(reader);
  return err;
}

} // Fog namespace
