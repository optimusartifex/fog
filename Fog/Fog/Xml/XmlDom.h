// [Fog-Xml Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_XMLDOM_H
#define _FOG_XML_XMLDOM_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Hash.h>
#include <Fog/Core/List.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Range.h>
#include <Fog/Core/String.h>
#include <Fog/Xml/Constants.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Stream;
struct XmlAttribute;
struct XmlCDATA;
struct XmlComment;
struct XmlDocument;
struct XmlDomReader;
struct XmlElement;
struct XmlIdManager;
struct XmlPI;
struct XmlReader;
struct XmlText;
struct XmlWriter;

// ============================================================================
// [Fog::XmlIdManager]
// ============================================================================

//! @internal
//!
//! @brief Hash table to store element IDs used by @c XmlDocument. Do not use directly.
struct FOG_API XmlIdManager
{
  // [Construction / Destruction]

  XmlIdManager();
  ~XmlIdManager();

  // [Methods]

  void add(XmlElement* e);
  void remove(XmlElement* e);
  XmlElement* get(const String& id) const;
  XmlElement* get(const Char* idStr, sysuint_t idLen) const;

private:
  void _rehash(sysuint_t capacity);

  // [Members]

  //! @brief Count of buckets.
  sysuint_t _capacity;
  //! @brief Count of nodes.
  sysuint_t _length;

  //! @brief Count of buckets we will expand to if length exceeds _expandLength.
  sysuint_t _expandCapacity;
  //! @brief Count of nodes to grow.
  sysuint_t _expandLength;

  //! @brief Count of buckeds we will shrink to if length gets _shinkLength.
  sysuint_t _shrinkCapacity;
  //! @brief Count of nodes to shrink.
  sysuint_t _shrinkLength;

  //! @brief Buckets.
  XmlElement** _buckets;
  //! @brief Initial buckets up to 16 elements with id attribute
  //! (for small documents).
  XmlElement* _bucketsBuffer[16];
};

// ============================================================================
// [Fog::XmlAttribute]
// ============================================================================

//! @brief Xml attribute.
struct FOG_API XmlAttribute
{
  // [Construction / Destruction]

  XmlAttribute(XmlElement* element, const ManagedString& name, int offset = -1);
  virtual ~XmlAttribute();

  // [Methods]

  //! @brief Return true whether attribute is assigned with element.
  //!
  //! This method is only usable for embedded attributes
  FOG_INLINE bool isAssigned() const { return _element != NULL; }

  FOG_INLINE XmlElement* getElement() const { return _element; }
  FOG_INLINE const String& getName() const { return _name.getString(); }

  virtual String getValue() const;
  virtual err_t setValue(const String& value);

protected:
  virtual void destroy();

  //! @brief Link to element that owns this attribute.
  XmlElement* _element;
  //! @brief Attribute name (managed string).
  ManagedString _name;
  //! @brief Attribute value (or empty if value is provided by overriden class).
  String _value;

  //! @brief Attribute offset in @c XmlElement (relative to @c XmlElement).
  //!
  //! If this attribute is not embedded to the element, the _offset value is -1.
  int _offset;

private:
  friend struct XmlElement;

  FOG_DISABLE_COPY(XmlAttribute)
};

// ============================================================================
// [Fog::XmlElement]
// ============================================================================

//! @brief Xml element.
struct FOG_API XmlElement
{
  // [Construction / Destruction]

  XmlElement(const ManagedString& tagName);
  virtual ~XmlElement();

  // [Type and Flags]

  //! @brief Return element type, see @c Type.
  FOG_INLINE int getType() const { return _type; }

  //! @brief Return true if element is @a TypeElement type.
  FOG_INLINE bool isElement() const { return (_type & XML_ELEMENT_MASK) == XML_ELEMENT_BASE; }
  //! @brief Return true if element is @a TypeText type.
  FOG_INLINE bool isText() const { return (_type & XML_ELEMENT_MASK) == XML_ELEMENT_TEXT; }
  //! @brief Return true if element is @a TypeCDATA type.
  FOG_INLINE bool isCDATA() const { return (_type & XML_ELEMENT_MASK) == XML_ELEMENT_CDATA; }
  //! @brief Return true if element is @a TypePI type.
  FOG_INLINE bool isPI() const { return (_type & XML_ELEMENT_MASK) == XML_ELEMENT_PI; }
  //! @brief Return true if element is @a TypeComment type.
  FOG_INLINE bool isComment() const { return (_type & XML_ELEMENT_MASK) == XML_ELEMENT_COMMENT; }
  //! @brief Return true if element is @a TypeDocument type.
  FOG_INLINE bool isDocument() const { return (_type & XML_ELEMENT_MASK) == XML_ELEMENT_DOCUMENT; }

  //! @brief Return true if element is SVG extension.
  FOG_INLINE bool isSvg() const { return (_type & SVG_ELEMENT_MASK) != 0; }
  //! @brief Return true if element is @c SvgElement.
  FOG_INLINE bool isSvgElement() const { return _type == SVG_ELEMENT_BASE; }
  //! @brief Return true if element is @c SvgDocument.
  FOG_INLINE bool isSvgDocument() const { return _type == SVG_ELEMENT_DOCUMENT; }

  // [Manage / Unmanage]

protected:
  //! @brief Start managing this element.
  virtual void _manage(XmlDocument* doc);
  //! @brief Stop managing this element.
  virtual void _unmanage();

  // [Clone]

public:
  //! @brief Clone this element with all children.
  virtual XmlElement* clone() const;

  // [Serialize]

  virtual void serialize(XmlWriter* writer);

  // [Normalize]

  //! @brief Normalize this element.
  //!
  //! Normalizing means to join text child elements and remove empty ones.
  virtual void normalize();

  // [Dom]

  //! @brief Prepend @a ch element to this element.
  err_t prependChild(XmlElement* ch);

  //! @brief Append @a ch element to this element.
  err_t appendChild(XmlElement* ch);

  //! @brief Remove @a ch element from this element.
  //!
  //! @note Element will not be deleted.
  err_t removeChild(XmlElement* ch);

  //! @brief Replace @a newch element with @a oldch one.
  //!
  //! @note @a oldch element will not be deleted.
  err_t replaceChild(XmlElement* newch, XmlElement* oldch);

  //! @brief Remove and delete child element @a ch.
  err_t deleteChild(XmlElement* ch);

  //! @brief Delete all children elements.
  err_t deleteAll();

  //! @brief Unlink this element from its parent.
  //!
  //! @note this operation is same as removing this node from parent by
  //! parent()->removeChild(this) call.
  err_t unlink();

  //! @brief Unlink, but not unmanage this element from document.
  //! @internal
  err_t _unlinkUnmanaged();

  //! @brief Returns true if @c e element is child of this element or all
  //! its descendents.
  bool contains(XmlElement* e, bool deep = false);

  //! @brief Return xml document.
  FOG_INLINE XmlDocument* getDocument() const { return _document; }

  //! @brief Return parent node.
  FOG_INLINE XmlElement* getParent() const { return _parent; }

  //! @brief Return array of child nodes.
  List<XmlElement*> childNodes() const;

  //! @brief Return first child node.
  FOG_INLINE XmlElement* firstChild() const { return _firstChild; }

  //! @brief Return last child node.
  FOG_INLINE XmlElement* lastChild() const { return _lastChild; }

  //! @brief Return next sibling node.
  FOG_INLINE XmlElement* nextSibling() const { return _nextSibling; }

  //! @brief Return previous sibling node.
  FOG_INLINE XmlElement* previousSibling() const { return _prevSibling; }

  //! @brief Return true if current node is first.
  FOG_INLINE bool isFirst() const { return _prevSibling == NULL; }

  //! @brief Return true if current node is last.
  FOG_INLINE bool isLast() const { return _nextSibling == NULL; }

  //! @brief Return true if current node has parent.
  FOG_INLINE bool hasParent() const { return _parent != NULL; }

  //! @brief Return true if current node has child nodes.
  FOG_INLINE bool hasChildNodes() const { return _firstChild != NULL; }
 
  List<XmlElement*> childNodesByTagName(const String& tagName) const;

  FOG_INLINE XmlElement* firstChildByTagName(const String& tagName) const
  { return _nextChildByTagName(_firstChild, tagName); }

  FOG_INLINE XmlElement* lastChildByTagName(const String& tagName) const
  { return _previousChildByTagName(_lastChild, tagName); }

  FOG_INLINE XmlElement* nextChildByTagName(const String& tagName) const
  { return _nextChildByTagName(_nextSibling, tagName); }

  FOG_INLINE XmlElement* previousChildByTagName(const String& tagName) const
  { return _previousChildByTagName(_prevSibling, tagName); }

  static XmlElement* _nextChildByTagName(XmlElement* refElement, const String& tagName);
  static XmlElement* _previousChildByTagName(XmlElement* refElement, const String& tagName);

  // [Attributes]

  //! @brief Return true if current node contains attributes.
  FOG_INLINE bool hasAttributes() const { return !_attributes.isEmpty(); }

  //! @brief Return array of attributes.
  List<XmlAttribute*> attributes() const;

  bool hasAttribute(const String& name) const;
  err_t setAttribute(const String& name, const String& value);
  String getAttribute(const String& name) const;
  err_t removeAttribute(const String& name);

  err_t removeAttributes();

  // [ID]

  FOG_INLINE String getId() const { return _id; }
  err_t setId(const String& id);

  // [Element and Text]

  FOG_INLINE const String& getTagName() const { return _tagName.getString(); }
  virtual err_t setTagName(const String& name);

  virtual String getTextContent() const;
  virtual err_t setTextContent(const String& text);

  virtual err_t _setAttribute(const ManagedString& name, const String& value);
  virtual String _getAttribute(const ManagedString& name) const;
  virtual err_t _removeAttribute(const ManagedString& name);
  virtual err_t _removeAttributes();

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;
  static void _copyAttributes(XmlElement* dst, XmlElement* src);

protected:
  uint8_t _type;
  uint8_t _reserved0;
  uint8_t _reserved1;
  mutable uint8_t _dirty;
  uint32_t _flags;

  XmlDocument* _document;
  XmlElement* _parent;
  XmlElement* _firstChild;
  XmlElement* _lastChild;
  XmlElement* _nextSibling;
  XmlElement* _prevSibling;

  //! @brief Children.
  mutable List<XmlElement*> _children;
  //! @brief Attributes.
  List<XmlAttribute*> _attributes;

  //! @brief Element tag name.
  ManagedString _tagName;
  //! @brief Element id.
  String _id;
  //! @brief Element id chain managed by @c XmlIdManager.
  XmlElement* _hashNextId;

private:
  friend struct XmlAttribute;
  friend struct XmlIdAttribute;
  friend struct XmlDocument;
  friend struct XmlIdManager;

  FOG_DISABLE_COPY(XmlElement)
};

// ============================================================================
// [Fog::XmlText]
// ============================================================================

//! @brief Xml text element.
struct FOG_API XmlText : public XmlElement
{
  // [Construction / Destruction]

  typedef XmlElement base;

  XmlText(const String& data = String());
  virtual ~XmlText();

  // [Clone]

  virtual XmlElement* clone() const;

  // [Text Specific]

  virtual String getTextContent() const;
  virtual err_t setTextContent(const String& text);

  FOG_INLINE const String& getData() const { return _data; }
  err_t setData(const String& data);
  err_t appendData(const String& data);
  err_t deleteData();
  err_t insertData(sysuint_t start, const String& data);
  err_t replaceData(const Range& range, const String& data);

protected:
  String _data;

private:
  friend struct XmlElement;
  friend struct XmlDocument;

  FOG_DISABLE_COPY(XmlText)
};

// ============================================================================
// [Fog::XmlNoTextElement]
// ============================================================================

//! @brief Xml no text element (convenience, base class for others).
struct FOG_API XmlNoTextElement : public XmlElement
{
  // [Construction / Destruction]

  typedef XmlElement base;

  XmlNoTextElement(const ManagedString& tagName);

  virtual String getTextContent() const;
  virtual err_t setTextContent(const String& text);

private:
  FOG_DISABLE_COPY(XmlNoTextElement)
};

// ============================================================================
// [Fog::XmlComment]
// ============================================================================

//! @brief Xml comment element.
struct FOG_API XmlComment : public XmlNoTextElement
{
  // [Construction / Destruction]

  typedef XmlNoTextElement base;

  XmlComment(const String& data = String());
  virtual ~XmlComment();

  // [Clone]

  virtual XmlElement* clone() const;

  // [Comment Specific]

  const String& getData() const;
  err_t setData(const String& data);

protected:
  String _data;

private:
  friend struct XmlElement;
  friend struct XmlDocument;

  FOG_DISABLE_COPY(XmlComment)
};

// ============================================================================
// [Fog::XmlCDATA]
// ============================================================================

//! @brief Xml CDATA section.
struct FOG_API XmlCDATA : public XmlNoTextElement
{
  // [Construction / Destruction]

  typedef XmlNoTextElement base;

  XmlCDATA(const String& data = String());
  virtual ~XmlCDATA();

  // [Clone]

  virtual XmlElement* clone() const;

  // [CDATA Specific]

  const String& getData() const;
  err_t setData(const String& data);

protected:
  String _data;

private:
  friend struct XmlElement;
  friend struct XmlDocument;

  FOG_DISABLE_COPY(XmlCDATA)
};

// ============================================================================
// [Fog::XmlPI]
// ============================================================================

//! @brief Xml processing instructions.
struct FOG_API XmlPI : public XmlNoTextElement
{
  // [Construction / Destruction]

  typedef XmlNoTextElement base;

  XmlPI(const String& data = String());
  virtual ~XmlPI();

  // [Clone]

  virtual XmlElement* clone() const;

  // [Processing Instruction Specific]

  const String& getData() const;
  err_t setData(const String& data);

protected:
  String _data;

private:
  friend struct XmlElement;
  friend struct XmlDocument;

  FOG_DISABLE_COPY(XmlPI)
};

// ============================================================================
// [Fog::XmlDocument]
// ============================================================================

//! @brief Xml document element.
struct FOG_API XmlDocument : public XmlElement
{
  // [Construction / Destruction]

  typedef XmlElement base;

  XmlDocument();
  virtual ~XmlDocument();

  // [Clone]

  virtual XmlElement* clone() const;

  // [Root Node]

  //! @brief Set document root element to @a e.
  virtual err_t setDocumentRoot(XmlElement* e);

  //! @brief Return document root element.
  FOG_INLINE XmlElement* documentRoot() const { return _documentRoot; }

  // [Clear]

  virtual void clear();

  // [Document Extensions]

  virtual XmlElement* createElement(const ManagedString& tagName);
  static XmlElement* createElementStatic(const ManagedString& tagName);
  virtual XmlDomReader* createDomReader();

  // [Dom]

  XmlElement* getElementById(const String& id) const;
  XmlElement* getElementById(const Utf16& id) const;

  // [Read]

  virtual err_t readFromFile(const String& fileName);
  virtual err_t readFromStream(Stream& stream);
  virtual err_t readFromMemory(const void* mem, sysuint_t size);
  virtual err_t readFromString(const String& str);

  // [DOCTYPE]

  FOG_INLINE const String& getDOCTYPE() const { return _doctype; }
  FOG_INLINE err_t setDOCTYPE(const String& doctype) { return _doctype.set(doctype); }

protected:
  // [Members]

  //! @brief Document root.
  XmlElement* _documentRoot;

  //! @brief Hash table that contains all managed strings and reference counts.
  UnorderedHash<String, sysuint_t> _managedStrings;

  //! @brief Hash table that contains all managed IDs.
  XmlIdManager _elementIdsHash;

  //! @brief DOCTYPE string.
  String _doctype;

private:
  friend struct XmlAttribute;
  friend struct XmlIdAttribute;
  friend struct XmlElement;

  FOG_DISABLE_COPY(XmlDocument)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_XML_XMLDOM_H
