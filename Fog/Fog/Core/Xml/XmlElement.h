// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLELEMENT_H
#define _FOG_CORE_XML_XMLELEMENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Xml/XmlAttribute.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct XmlDocument;
struct XmlSaxReader;
struct XmlSaxWriter;

// ============================================================================
// [Fog::XmlElement]
// ============================================================================

//! @brief Xml element.
struct FOG_API XmlElement
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlElement(const ManagedString& tagName);
  virtual ~XmlElement();

  // --------------------------------------------------------------------------
  // [Type and Flags]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [Manage / Unmanage]
  // --------------------------------------------------------------------------

protected:
  //! @brief Start managing this element.
  virtual void _manage(XmlDocument* doc);
  //! @brief Stop managing this element.
  virtual void _unmanage();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

public:
  //! @brief Clone this element with all children.
  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [Serialize]
  // --------------------------------------------------------------------------

  virtual void serialize(XmlSaxWriter* writer);

  // --------------------------------------------------------------------------
  // [Normalize]
  // --------------------------------------------------------------------------

  //! @brief Normalize this element.
  //!
  //! Normalizing means to join text child elements and remove empty ones.
  virtual void normalize();

  // --------------------------------------------------------------------------
  // [Dom]
  // --------------------------------------------------------------------------

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

  //! @internal
  //!
  //! @brief Unlink, but not unmanage this element from document.
  err_t _unlinkUnmanaged();

  //! @brief Returns true if @c e element is child of this element or all
  //! its descendents.
  bool contains(XmlElement* e, bool deep = false);

  //! @brief Get XML-Document.
  FOG_INLINE XmlDocument* getDocument() const { return _document; }

  //! @brief Get parent node.
  FOG_INLINE XmlElement* getParent() const { return _parent; }

  //! @brief Get array of child nodes.
  List<XmlElement*> getChildNodes() const;

  //! @brief Get first child node.
  FOG_INLINE XmlElement* getFirstChild() const { return _firstChild; }

  //! @brief Get last child node.
  FOG_INLINE XmlElement* getLastChild() const { return _lastChild; }

  //! @brief Get next sibling node.
  FOG_INLINE XmlElement* getNextSibling() const { return _nextSibling; }

  //! @brief Get previous sibling node.
  FOG_INLINE XmlElement* getPreviousSibling() const { return _prevSibling; }

  //! @brief Get whether the current node is first.
  FOG_INLINE bool isFirst() const { return _prevSibling == NULL; }

  //! @brief Get whether the current node is last.
  FOG_INLINE bool isLast() const { return _nextSibling == NULL; }

  //! @brief Get whether the current node has parent.
  FOG_INLINE bool hasParent() const { return _parent != NULL; }

  //! @brief Get whether the current node has child nodes.
  FOG_INLINE bool hasChildNodes() const { return _firstChild != NULL; }

  List<XmlElement*> getChildNodesByTagName(const StringW& tagName) const;

  FOG_INLINE XmlElement* getFirstChildByTagName(const StringW& tagName) const
  { return _nextChildByTagName(_firstChild, tagName); }

  FOG_INLINE XmlElement* getLastChildByTagName(const StringW& tagName) const
  { return _previousChildByTagName(_lastChild, tagName); }

  FOG_INLINE XmlElement* getNextChildByTagName(const StringW& tagName) const
  { return _nextChildByTagName(_nextSibling, tagName); }

  FOG_INLINE XmlElement* getPreviousChildByTagName(const StringW& tagName) const
  { return _previousChildByTagName(_prevSibling, tagName); }

  static XmlElement* _nextChildByTagName(XmlElement* refElement, const StringW& tagName);
  static XmlElement* _previousChildByTagName(XmlElement* refElement, const StringW& tagName);

  // --------------------------------------------------------------------------
  // [Attributes]
  // --------------------------------------------------------------------------

  //! @brief Get whether the element contains xml-attributes / properties.
  FOG_INLINE bool hasAttributes() const { return !_attributes.isEmpty(); }

  //! @brief Get the array of attributes.
  List<XmlAttribute*> attributes() const;

  bool hasAttribute(const StringW& name) const;
  err_t setAttribute(const StringW& name, const StringW& value);
  StringW getAttribute(const StringW& name) const;
  err_t removeAttribute(const StringW& name);

  err_t removeAttributes();

  // --------------------------------------------------------------------------
  // [ID]
  // --------------------------------------------------------------------------

  FOG_INLINE StringW getId() const { return _id; }
  err_t setId(const StringW& id);

  // --------------------------------------------------------------------------
  // [Element and Text]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getTagName() const { return _tagName.getString(); }
  virtual err_t setTagName(const StringW& name);

  virtual StringW getTextContent() const;
  virtual err_t setTextContent(const StringW& text);

  virtual err_t _setAttribute(const ManagedString& name, const StringW& value);
  virtual StringW _getAttribute(const ManagedString& name) const;
  virtual err_t _removeAttribute(const ManagedString& name);
  virtual err_t _removeAttributes();

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;
  static void _copyAttributes(XmlElement* dst, XmlElement* src);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

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
  StringW _id;
  //! @brief Element id chain managed by @c XmlIdManager.
  XmlElement* _hashNextId;

private:
  _FOG_NO_COPY(XmlElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLELEMENT_H
