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

//! @addtogroup Fog_Core_Xml
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct XmlDocument;
struct XmlSaxReader;
struct XmlSaxWriter;

// ============================================================================
// [Fog::DomExtensionType]
// ============================================================================

union FOG_NO_EXPORT DomExtensionType
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE DomExtensionType(uint32_t group, uint32_t type)
  {
    _packed = (group << 16) | type;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getGroup() const { return _data.group; }
  FOG_INLINE uint32_t getType() const { return _data.type; }

  FOG_INLINE void setGroup(uint32_t group) { _data.group = group; }
  FOG_INLINE void setType(uint32_t type) { _data.type = type; }

  FOG_INLINE bool isExtension(uint32_t group, uint32_t type) const
  {
    return _packed == ((group << 16) | type);
  }

  FOG_INLINE void setExtension(uint32_t group, uint32_t type)
  {
    _packed = ((group << 16) | type);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _Packed
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    uint16_t type;
    uint16_t group;
#else
    uint16_t group;
    uint16_t type;
#endif // FOG_BYTE_ORDER
  } _data;
  
  uint32_t _packed;
};

// ============================================================================
// [Fog::XmlElement]
// ============================================================================

//! @brief Xml element.
struct FOG_API XmlElement
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlElement(const ManagedStringW& tagName);
  virtual ~XmlElement();

  // --------------------------------------------------------------------------
  // [Type and Flags]
  // --------------------------------------------------------------------------

  //! @brief Get element type, see @ref DOM_NODE.
  FOG_INLINE uint32_t getNodeType() const { return _nodeType; }
  //! @brief Get element flags, see @ref DOM_FLAG.
  FOG_INLINE uint32_t getNodeFlags() const { return _nodeFlags; }

  //! @brief Get whether the element is @ref DomElement.
  FOG_INLINE bool isElement() const { return _nodeType == DOM_NODE_ELEMENT; }
  //! @brief Get whether the element is @ref DomAttribute.
  FOG_INLINE bool isAttribute() const { return _nodeType == DOM_NODE_ATTRIBUTE; }
  //! @brief Get whether the element is @ref DomText.
  FOG_INLINE bool isText() const { return _nodeType == DOM_NODE_TEXT; }
  //! @brief Get whether the element is @ref DomCDATA.
  FOG_INLINE bool isCDATA() const { return _nodeType == DOM_NODE_CDATA; }
  //! @brief Get whether the element is @ref DomEntityReference.
  FOG_INLINE bool isEntityReference() const { return _nodeType == DOM_NODE_ENTITY_REFERENCE; }
  //! @brief Get whether the element is @ref DomEntityReference.
  FOG_INLINE bool isEntity() const { return _nodeType == DOM_NODE_ENTITY; }
  //! @brief Get whether the element is @ref DomProcessingInstruction.
  FOG_INLINE bool isProcessingInstruction() const { return _nodeType == DOM_NODE_PROCESSING_INSTRUCTION; }
  //! @brief Get whether the element is @ref DomComment.
  FOG_INLINE bool isComment() const { return _nodeType == DOM_NODE_COMMENT; }
  //! @brief Get whether the element is @ref DomDocument.
  FOG_INLINE bool isDocument() const { return _nodeType == DOM_NODE_DOCUMENT; }
  //! @brief Get whether the element is @ref DomDocumentType.
  FOG_INLINE bool isDocumentType() const { return _nodeType == DOM_NODE_DOCUMENT_TYPE; }
  //! @brief Get whether the element is @ref DomDocumentFragment.
  FOG_INLINE bool isDocumentFragment() const { return _nodeType == DOM_NODE_DOCUMENT_FRAGMENT; }
  //! @brief Get whether the element is @ref DomNotation.
  FOG_INLINE bool isNotation() const { return _nodeType == DOM_NODE_NOTATION; }

  //! @brief Get DOM extension group.
  FOG_INLINE uint32_t getExtensionGroup() const { return _ext.getGroup(); }
  //! @brief Get DOM extension type.
  FOG_INLINE uint32_t getExtensionType() const { return _ext.getType(); }

  //! @brief Get whether the DOM node is of extension @a group. 
  FOG_INLINE bool isExtensionGroup(uint32_t extGroup) const { return _ext.getGroup() == extGroup; }
  //! @brief Get whether the DOM node is of extension @a group. 
  FOG_INLINE bool isExtensionType(uint32_t extType) const { return _ext.getType() == extType; }

  //! @brief Get whether the DOM node is of extension @a group and @a type.
  FOG_INLINE bool isExtensionGroupAndType(uint32_t extGroup, uint32_t extType) const { return _ext.isExtension(extGroup, extType); }
  //! @brief Get whether the DOM node is of extension @a group and @a type.
  FOG_INLINE bool isExtensionGroupAndNode(uint32_t extGroup, uint32_t nodeType) const { return (_ext.getGroup() == extGroup) & (getNodeType() == nodeType); }

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

  virtual err_t _setAttribute(const ManagedStringW& name, const StringW& value);
  virtual StringW _getAttribute(const ManagedStringW& name) const;
  virtual err_t _removeAttribute(const ManagedStringW& name);
  virtual err_t _removeAttributes();

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;
  static void _copyAttributes(XmlElement* dst, XmlElement* src);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief DOM node type, see @ref DOM_NODE.
  uint8_t _nodeType;
  //! @brief DOM node flags, see @ref DOM_FLAG.
  uint8_t _nodeFlags;
  //! @brief Reserved for future use.
  uint8_t _nodeReserved;
  //! @brief Dirty flag.
  mutable uint8_t _nodeDirty;

  //! @brief Dom extension group and type.
  DomExtensionType _ext;

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
  ManagedStringW _tagName;
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
