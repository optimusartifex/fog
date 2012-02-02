// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_DOM_DOM_H
#define _FOG_CORE_DOM_DOM_H

// [Dependencies]
#include <Fog/Core/Dom/DomResourceManager.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/CoreObj.h>
#include <Fog/Core/Memory/MemGCAllocator.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Var.h>
#include <Fog/Core/Tools/XmlIO.h>

namespace Fog {

//! @addtogroup Fog_Core_Dom
//! @{

// ============================================================================
// [Fog::DomObj]
// ============================================================================

//! @brief DOM base object, managed by @ref DomDocument.
struct FOG_API DomObj : public CoreObj
{
  FOG_CORE_OBJ(DomObj, CoreObj)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomObj();
  virtual ~DomObj();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  virtual DomObj* _addRef() = 0;
  virtual void _release() = 0;

  FOG_INLINE DomObj* addRef() { return _addRef(); }
  FOG_INLINE void release() { _release(); }

  // --------------------------------------------------------------------------
  // [GC]
  // --------------------------------------------------------------------------

  virtual bool _canCollect() const = 0;
};

//! @brief Macro used to declare a new DOM object.
#define FOG_DOM_OBJ(_Self_, _Base_) \
  FOG_CORE_OBJ(_Self_, _Base_) \
  \
  FOG_INLINE _Self_* addRef() \
  { \
    return static_cast<_Self_*>(this->_addRef()); \
  }

// ============================================================================
// [Fog::DomAttr]
// ============================================================================

//! @brief DOM attribute.
struct FOG_NO_EXPORT DomAttr
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE DomAttr(const InternedStringW& name, const StringW& value) :
    _name(name),
    _value(value)
  {
  }

  FOG_INLINE DomAttr(const DomAttr& other) :
    _name(other._name),
    _value(other._value)
  {
  }

  FOG_INLINE ~DomAttr()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const InternedStringW& getName() const { return _name; }

  FOG_INLINE const StringW& getValue() const { return _value; }
  FOG_INLINE err_t setValue(const StringW& value) { return _value.set(value); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE DomAttr& operator=(const DomAttr& other)
  {
    _name = other._name;
    _value = other._value;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Name of attribute.
  InternedStringW _name;
  //! @brief Attribute value.
  StringW _value;
};

// ============================================================================
// [Fog::DomNodeList]
// ============================================================================

struct FOG_API DomNodeList : public DomObj
{
  FOG_DOM_OBJ(DomNodeList, DomObj)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomNodeList();
  virtual ~DomNodeList();

  // --------------------------------------------------------------------------
  // [List]
  // --------------------------------------------------------------------------

  virtual DomNode* getNode(size_t index) const = 0;
  virtual size_t getLength() const = 0;
};

// ============================================================================
// [Fog::DomNode]
// ============================================================================

//! @brief DOM node.
struct FOG_API DomNode : public DomObj
{
  FOG_DOM_OBJ(DomNode, DomObj)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomNode(DomDocument* ownerDocument, uint32_t nodeType);
  virtual ~DomNode();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  virtual DomObj* _addRef() override;
  virtual void _release() override;

  // --------------------------------------------------------------------------
  // [CanCollect]
  // --------------------------------------------------------------------------

  virtual bool _canCollect() const override;

  // --------------------------------------------------------------------------
  // [Node Type]
  // --------------------------------------------------------------------------

  //! @brief Get node type, see @ref DOM_NODE_TYPE.
  FOG_INLINE uint32_t getNodeType() const { return _nodeType; }

  //! @brief Get whether the node is @ref DomCDATA.
  FOG_INLINE bool isCDATASection() const { return _nodeType == DOM_NODE_TYPE_CDATA_SECTION; }
  //! @brief Get whether the node is @ref DomComment.
  FOG_INLINE bool isComment() const { return _nodeType == DOM_NODE_TYPE_COMMENT; }
  //! @brief Get whether the node is @ref DomDocument.
  FOG_INLINE bool isDocument() const { return _nodeType == DOM_NODE_TYPE_DOCUMENT; }
  //! @brief Get whether the node is @ref DomDocumentFragment.
  FOG_INLINE bool isDocumentFragment() const { return _nodeType == DOM_NODE_TYPE_DOCUMENT_FRAGMENT; }
  //! @brief Get whether the node is @ref DomDocumentType.
  FOG_INLINE bool isDocumentType() const { return _nodeType == DOM_NODE_TYPE_DOCUMENT_TYPE; }
  //! @brief Get whether the node is @ref DomElement.
  FOG_INLINE bool isElement() const { return _nodeType == DOM_NODE_TYPE_ELEMENT; }
  //! @brief Get whether the node is @ref DomProcessingInstruction.
  FOG_INLINE bool isProcessingInstruction() const { return _nodeType == DOM_NODE_TYPE_PROCESSING_INSTRUCTION; }
  //! @brief Get whether the node is @ref DomText.
  FOG_INLINE bool isText() const { return _nodeType == DOM_NODE_TYPE_TEXT; }

  // --------------------------------------------------------------------------
  // [Node Flags]
  // --------------------------------------------------------------------------

  //! @brief Get node flags, see @ref DOM_NODE_FLAG.
  FOG_INLINE uint32_t getNodeFlags() const
  {
    return _nodeFlags;
  }

  //! @brief Get whether the node attributes and all descendants are read-only.
  FOG_INLINE bool isReadOnly() const
  {
    return (_nodeFlags & DOM_NODE_FLAG_READ_ONLY) != 0;
  }

  //! @brief Get whether the node is in GC queue.
  FOG_INLINE bool isInGCQueue() const
  {
    return (_nodeFlags & DOM_NODE_FLAG_IN_GC_QUEUE) != 0;
  }

  // --------------------------------------------------------------------------
  // [ObjectModel / ObjectType]
  // --------------------------------------------------------------------------

  //! @brief Get node object model.
  FOG_INLINE uint32_t getObjectModel() const { return _objectModel; }

  //! @brief Get node object type.
  //!
  //! @note Object type of @ref DOM_OBJECT_MODEL_XML is the same as node type.
  FOG_INLINE uint32_t getObjectType() const { return _objectType; }

  //! @brief Get whether the DOM node is using @a objectModel.
  FOG_INLINE bool isObjectModel(uint32_t objectModel) const
  {
    return _objectModel == objectModel;
  }

  //! @brief Get whether the DOM object model is @a objectModel and object type
  //! is @a objectType.
  FOG_INLINE bool isObjectModelAndObjectType(uint32_t objectModel, uint32_t objectType) const
  {
    return (_objectModel == objectModel) & (_objectType == objectType);
  }

  //! @brief Get whether the DOM object model is @a objectModel and node type
  //! is @a nodeType.
  FOG_INLINE bool isObjectModelAndNodeType(uint32_t objectModel, uint32_t nodeType) const
  {
    return (_objectModel == objectModel) & (_nodeType == nodeType);
  }

  // --------------------------------------------------------------------------
  // [Node Name]
  // --------------------------------------------------------------------------

  //! @brief Get node name.
  const InternedStringW& getNodeName() const;

  // --------------------------------------------------------------------------
  // [Node Value]
  // --------------------------------------------------------------------------

  //! @brief Get node value.
  StringW getNodeValue() const;
  //! @brief Set node value.
  err_t setNodeValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Hierarchy]
  // --------------------------------------------------------------------------

  //! @brief Get owner document.
  FOG_INLINE DomDocument* getOwnerDocument() const { return _ownerDocument; }

  //! @brief Get whether node has parent node.
  FOG_INLINE bool hasParentNode() const { return _parentNode != NULL; }
  //! @brief Get parent node.
  FOG_INLINE DomContainer* getParentNode() const { return _parentNode; }

  //! @brief Get previous sibling.
  FOG_INLINE DomNode* getPreviousSibling() const { return _previousSibling; }
  //! @brief Get next sibling.
  FOG_INLINE DomNode* getNextSibling() const { return _nextSibling; }

  //! @brief Get whether the node contains child nodes.
  FOG_INLINE bool hasChildNodes() const { return (_nodeFlags & DOM_NODE_FLAG_HAS_CHILD_NODES) != 0; }
  //! @brief Get child node list.
  DomNodeList* getChildNodes() const;

  //! @brief Get whether node contains @a refChild.
  bool contains(DomNode* refChild, bool deep = true) const;

  DomNode* getFirstChild() const;
  DomNode* getLastChild() const;

  err_t prependChild(DomNode* newChild);
  err_t appendChild(DomNode* newChild);

  err_t removeChild(DomNode* oldChild);
  err_t removeChildNodes();

  err_t insertBefore(DomNode* newChild, DomNode* refChild);
  err_t insertAfter(DomNode* newChild, DomNode* refChild);
  err_t replaceChild(DomNode* newChild, DomNode* refChild);
  err_t unlink();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  DomNode* cloneNode(bool deep) const;

  // --------------------------------------------------------------------------
  // [Normalize]
  // --------------------------------------------------------------------------

  err_t normalize(bool deep);

  // --------------------------------------------------------------------------
  // [CompareDocumentPosition]
  // --------------------------------------------------------------------------
  
  uint32_t compareDocumentPosition(DomNode* other) const;

  // --------------------------------------------------------------------------
  // [TextContent]
  // --------------------------------------------------------------------------

  StringW getTextContent() const;
  err_t setTextContent(const StringW& textContent);

  // --------------------------------------------------------------------------
  // [Events]
  // --------------------------------------------------------------------------

  // NOTE: Base implementation does nothing, reimplement to extend the 
  // functionality, but never manipulate DOM inside these handlers!

  virtual void _onChildAdd(DomNode* firstNode, DomNode* lastNode);
  virtual void _onChildRemove(DomNode* firstNode, DomNode* lastNode);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Variable type and flags.
  uint32_t _vType;

  //! @brief DOM node type, see @ref DOM_NODE_TYPE.
  uint8_t _nodeType;
  //! @brief DOM node flags, see @ref DOM_NODE_FLAG.
  uint8_t _nodeFlags;
  //! @brief DOM object model, see @ref DOM_OBJECT_MODEL.
  uint8_t _objectModel;
  //! @brief DOM object class, see @ref DOM_OBJECT_TYPE.
  uint8_t _objectType;

  //! @brief Reference count.
  Atomic<size_t> _reference;

  //! @brief Owner document.
  DomDocument* _ownerDocument;
  //! @brief Parent @ref DomNode.
  DomContainer* _parentNode;
  //! @brief Previous sibling @ref DomNode.
  DomNode* _previousSibling;
  //! @brief Next sibling @ref DomNode.
  DomNode* _nextSibling;
};

// ============================================================================
// [Fog::DomContainer]
// ============================================================================

struct FOG_API DomContainer : public DomNode
{
  FOG_DOM_OBJ(DomContainer, DomNode)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomContainer(DomDocument* ownerDocument, uint32_t nodeType);
  virtual ~DomContainer();

  // --------------------------------------------------------------------------
  // [DOM Interface]
  // --------------------------------------------------------------------------

  FOG_INLINE DomNode* getFirstChild() const { return _firstChild; }
  FOG_INLINE DomNode* getLastChild() const { return _lastChild; } 
  FOG_INLINE DomNodeList* getChildNodes() const { return (DomNodeList*)_childNodeList; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Child node list.
  //!
  //! @note We use uint8_t[] array here to hide the implementation details of
  //! the list.
  uint8_t _childNodeList[sizeof(DomNodeList)];
  //! @brief Cached child node list array[].
  List<DomNode*> _childNodeArray;

  //! @brief Link to first child.
  DomNode* _firstChild;
  //! @brief Link to last child.
  DomNode* _lastChild;
};

// ============================================================================
// [Fog::DomElement]
// ============================================================================

//! @brief DOM element.
struct FOG_API DomElement : public DomContainer
{
  FOG_DOM_OBJ(DomElement, DomContainer)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomElement(DomDocument* ownerDocument, const InternedStringW& tagName);
  virtual ~DomElement();

  // --------------------------------------------------------------------------
  // [Tag Name]
  // --------------------------------------------------------------------------

  //! @brief Get element tag name.
  FOG_INLINE const InternedStringW& getTagName() const { return _tagName; }

  // --------------------------------------------------------------------------
  // [Properties - Interface]
  // --------------------------------------------------------------------------

  virtual err_t _unresolvedProperty(size_t& newIndex,
    const CharW* name, size_t nameLength, const InternedStringW* nameInterned,
    const StringW* initialValue) override;

  // --------------------------------------------------------------------------
  // [Attributes]
  // --------------------------------------------------------------------------

  //! @brief Get whether an attribute with a given @a name is specified.
  FOG_INLINE bool hasAttribute(const StringW& name) const { return hasProperty(name); }
  //! @overload
  FOG_INLINE bool hasAttribute(const StubW& name) const { return hasProperty(name); }

  //! @brief Get attribute value by @a name.
  StringW getAttribute(const StringW& name) const;
  //! @overload
  StringW getAttribute(const StubW& name) const;

  //! @brief Add a new attribute named @a name.
  //!
  //! If an attribute with that name is already present in the element, its 
  //! value is changed to be that of the value parameter
  FOG_INLINE err_t setAttribute(const StringW& name, const StringW& value) { return setProperty(name, value); }
  //! @overload
  FOG_INLINE err_t setAttribute(const StubW& name, const StringW& value) { return setProperty(name, value); }

  //! @brief Remove an attribute by @a name.
  FOG_INLINE err_t removeAttribute(const StringW& name) { return resetProperty(name); }
  //! @overload
  FOG_INLINE err_t removeAttribute(const StubW& name) { return resetProperty(name); }

  //! @brief Remove all element attributes which can be removed.
  void removeAllAttributes();

  // --------------------------------------------------------------------------
  // [Id]
  // --------------------------------------------------------------------------

  //! @brief Get whether the element has an id.
  FOG_INLINE bool hasId() const { return !_id.isEmpty(); }
  //! @brief Get element id.
  FOG_INLINE const StringW& getId() const { return _id; }

  //! @brief Set element id.
  err_t setId(const StringW& id);
  //! @brief Reset element id.
  FOG_INLINE err_t resetId() { return setId(StringW::getEmptyInstance()); }

  // --------------------------------------------------------------------------
  // [GetElementsByTagName]
  // --------------------------------------------------------------------------

  //! @brief Get a @ref DomNodeList of all descendant @ref DomElement`s with a
  //! given tag name, in document order.
  DomNodeList* getElementsByTagName(const StringW& name) const;
  //! @overload
  DomNodeList* getElementsByTagName(const StubW& name) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Tag name.
  InternedStringW _tagName;
  //! @brief Attribute array.
  List<DomAttr> _attrArray;

  //! @brief Element ID.
  StringW _id;
  //! @brief Next element in hash table which stored element IDs (internal).
  DomElement* _nextId;

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  FOG_PROPERTY_DEF()
    FOG_PROPERTY_RW(Id, StringW)
  FOG_PROPERTY_END()
};

// ============================================================================
// [Fog::DomCharacterData]
// ============================================================================

//! @brief DOM character data.
struct FOG_API DomCharacterData : public DomNode
{
  FOG_DOM_OBJ(DomCharacterData, DomNode)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomCharacterData(DomDocument* ownerDocument, uint32_t nodeType, const StringW& data);
  virtual ~DomCharacterData();

  // --------------------------------------------------------------------------
  // [DOM Interface]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getData() const { return _data; }
  FOG_INLINE size_t getLength() const { return _data.getLength(); }

  err_t setData(const StringW& data);

  err_t appendData(const StringW& arg);
  err_t insertData(size_t offset, const StringW& arg);

  err_t deleteData(size_t offset, size_t count);
  err_t deleteData(const Range& range);

  err_t replaceData(size_t offset, size_t count, const StringW& arg);
  err_t replaceData(const Range& range, const StringW& arg);

  StringW substringData(size_t offset, size_t count);
  StringW substringData(const Range& range);

  // --------------------------------------------------------------------------
  // [HasInvalidSequence]
  // --------------------------------------------------------------------------

  virtual bool hasInvalidSequence() const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  StringW _data;
};

// ============================================================================
// [Fog::DomText]
// ============================================================================

//! @brief DOM text.
struct FOG_API DomText : public DomCharacterData
{
  FOG_DOM_OBJ(DomText, DomCharacterData)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomText(DomDocument* ownerDocument, const StringW& data);
  virtual ~DomText();

  // --------------------------------------------------------------------------
  // [DOM Interface]
  // --------------------------------------------------------------------------

  //! @brief Get whether this text node contains element content whitespace,
  //! often abusively called "ignorable whitespace".
  //!
  //! The text node is determined to contain whitespace in element content
  //! during the load of the document or if validation occurs while using
  //! @ref DomDocument::normalizeDocument().
  bool isElementContentWhitespace() const;

  //! @brief Get all text of logically-adjacent text nodes to this node,
  //! concatenated in document order.
  StringW getWholeText() const;

  //! @brief Replace the text of the current node and all logically-adjacent 
  //! text nodes with the specified text.
  DomNode* replaceWholeText(const StringW& content);

  //! @brief Break this node into two nodes at the specified offset, keeping
  //! both in the tree as siblings
  DomNode* splitText(size_t offset, err_t* errOut = NULL);
};

// ============================================================================
// [Fog::DomCDATASection]
// ============================================================================

//! @brief DOM CDATA section.
struct FOG_API DomCDATASection : public DomText
{
  FOG_DOM_OBJ(DomCDATASection, DomCharacterData)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomCDATASection(
    DomDocument* ownerDocument,
    const StringW& data);
  virtual ~DomCDATASection();

  // --------------------------------------------------------------------------
  // [HasInvalidSequence]
  // --------------------------------------------------------------------------

  virtual bool hasInvalidSequence() const override;
};

// ============================================================================
// [Fog::DomComment]
// ============================================================================

//! @brief DOM comment.
struct FOG_API DomComment : public DomCharacterData
{
  FOG_DOM_OBJ(DomComment, DomCharacterData)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomComment(DomDocument* ownerDocument, const StringW& data);
  virtual ~DomComment();

  // --------------------------------------------------------------------------
  // [HasInvalidSequence]
  // --------------------------------------------------------------------------

  virtual bool hasInvalidSequence() const override;
};

// ============================================================================
// [Fog::DomProcessingInstruction]
// ============================================================================

//! @brief DOM processing instruction.
struct FOG_API DomProcessingInstruction : public DomCharacterData
{
  FOG_DOM_OBJ(DomProcessingInstruction, DomCharacterData)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomProcessingInstruction(
    DomDocument* ownerDocument,
    const InternedStringW& target,
    const StringW& data);
  virtual ~DomProcessingInstruction();

  // --------------------------------------------------------------------------
  // [DOM Interface]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getTarget() const { return _target; }

  // --------------------------------------------------------------------------
  // [HasInvalidSequence]
  // --------------------------------------------------------------------------

  virtual bool hasInvalidSequence() const override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  InternedStringW _target;
};

// ============================================================================
// [Fog::DomDocumentFragment]
// ============================================================================

//! @brief DOM document fragment.
struct FOG_API DomDocumentFragment : public DomContainer
{
  FOG_DOM_OBJ(DomDocumentFragment, DomContainer)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomDocumentFragment(DomDocument* ownerDocument);
  virtual ~DomDocumentFragment();
};

// ============================================================================
// [Fog::DomDocumentType]
// ============================================================================

//! @brief DOM document fragment.
struct FOG_API DomDocumentType : public DomNode
{
  FOG_DOM_OBJ(DomDocumentType, DomNode)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomDocumentType(
    DomDocument* ownerDocument,
    const InternedStringW& name,
    const StringW& publicId,
    const StringW& systemId,
    const StringW& internalSubset);
  virtual ~DomDocumentType();

  // --------------------------------------------------------------------------
  // [DOM Interface]
  // --------------------------------------------------------------------------

  FOG_INLINE const InternedStringW& getName() const { return _name; }

  FOG_INLINE const StringW& getPublicId() const { return _publicId; }
  FOG_INLINE const StringW& getSystemId() const { return _systemId; }
  FOG_INLINE const StringW& getInternalSubset() const { return _internalSubset; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  InternedStringW _name;

  StringW _publicId;
  StringW _systemId;
  StringW _internalSubset;
};

// ============================================================================
// [Fog::DomDocument]
// ============================================================================

//! @brief DOM document.
struct FOG_API DomDocument : public DomContainer
{
  FOG_DOM_OBJ(DomDocument, DomContainer)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomDocument();
  virtual ~DomDocument();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  virtual void _release() override;

  // --------------------------------------------------------------------------
  // [DOM Container]
  // --------------------------------------------------------------------------

  DomDocumentType* getDocumentType() const;
  DomElement* getDocumentElement() const;

  // --------------------------------------------------------------------------
  // [DOM Create - Public]
  // --------------------------------------------------------------------------

  DomCDATASection* createCDATASection(
    const StringW& data);

  DomComment* createComment(
    const StringW& data);

  DomElement* createElement(
    const StringW& tagName);

  DomProcessingInstruction* createProcessingInstruction(
    const StringW& target,
    const StringW& data);

  DomText* createTextNode(
    const StringW& data);

  // --------------------------------------------------------------------------
  // [DOM Create - Private]
  // --------------------------------------------------------------------------

  virtual DomDocument* _createDocument();

  virtual DomCDATASection* _createCDATASection(
    const StringW& data);

  virtual DomComment* _createComment(
    const StringW& data);
  
  virtual DomElement* _createElement(
    const InternedStringW& tagName);

  virtual DomProcessingInstruction* _createProcessingInstruction(
    const InternedStringW& target,
    const StringW& data);

  virtual DomText* _createTextNode(
    const StringW& data);

  template<typename ElementT>
  FOG_INLINE ElementT* _newElementT()
  {
    void* p = _gc.alloc(sizeof(ElementT));
    if (FOG_IS_NULL(p))
      return NULL;
    return fog_new_p(p) ElementT(this);
  }

  template<typename ElementT>
  FOG_INLINE ElementT* _newElementT(
    const InternedStringW& tagName)
  {
    void* p = _gc.alloc(sizeof(ElementT));
    if (FOG_IS_NULL(p))
      return NULL;
    return fog_new_p(p) ElementT(this, tagName);
  }

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  //! @brief Create a copy of this document.
  //!
  //! In case that it's not possible, the implementation can return @c NULL.
  virtual DomDocument* cloneDocument() const;

  // --------------------------------------------------------------------------
  // [GetElementById]
  // --------------------------------------------------------------------------

  DomElement* getElementById(const StringW& id) const;
  DomElement* getElementById(const StubW& id) const;

  // --------------------------------------------------------------------------
  // [GetElementsByTagName]
  // --------------------------------------------------------------------------

  DomNodeList* getElementsByTagName(const StringW& tagName) const;
  DomNodeList* getElementsByTagName(const StubW& tagName) const;

  // --------------------------------------------------------------------------
  // [ImportNode / AdoptNode]
  // --------------------------------------------------------------------------

  DomNode* importNode(DomNode* importedNode, bool deep, err_t* errOut = NULL);

  // --------------------------------------------------------------------------
  // [XML]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getDocumentURI() const { return _documentURI; }
  err_t setDocumentURI(const StringW& documentURI);

  FOG_INLINE const StringW& getXmlEncoding() const { return _xmlEncoding; }
  err_t setXmlEncoding(const StringW& xmlEncoding);

  FOG_INLINE bool getXmlStandalone() const { return _xmlStandalone; }
  err_t setXmlStandalone(bool xmlStandalone);

  FOG_INLINE const StringW& getXmlVersion() const { return _xmlVersion; }
  err_t setXmlVersion(const StringW& xmlVersion);

  // --------------------------------------------------------------------------
  // [Read / Write]
  // --------------------------------------------------------------------------

  //! @brief Read DOM document from file @a fileName.
  err_t readFromFile(const StringW& fileName);
  //! @brief Read DOM document from @a stream.
  err_t readFromStream(Stream& stream);
  err_t readFromMemory(const void* mem, size_t size);
  err_t readFromString(const StringW& str);
  err_t readFromString(const StubW& str);

  // --------------------------------------------------------------------------
  // [Resource Manager]
  // --------------------------------------------------------------------------

  FOG_INLINE DomResourceManager* getResourceManager() const
  {
    return const_cast<DomResourceManager*>(&_resourceManager);
  }

  // --------------------------------------------------------------------------
  // [GC]
  // --------------------------------------------------------------------------

  //! @brief Collect (destroy) all nodes which are not connected into the
  //! DOM-Tree and their reference count is zero.
  //!
  //! This function should be called after DOM has been manipulated.
  void collect();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Document type.
  DomDocumentType* _documentType;
  //! @brief Document element (root).
  DomElement* _documentElement;

  //! @brief Resource manager.
  DomResourceManager _resourceManager;

  //! @brief Memory allocator and garbage collector.
  MemGCAllocator _gc;

  //! @brief Element<->ID table.
  void* _domIdHash;

  //! @brief Document URI.
  StringW _documentURI;
  //! @brief Get XML version string.
  StringW _xmlVersion;
  //! @brief Get XML encoding.
  StringW _xmlEncoding;
  //! @brief Get XML standalone attribute.
  bool _xmlStandalone;
};

// ============================================================================
// [Fog::DomSaxHandler]
// ============================================================================

struct FOG_API DomSaxHandler : public XmlSaxHandler
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomSaxHandler(DomDocument* document);
  virtual ~DomSaxHandler();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE DomDocument* getDocument() const { return _document; }

  // --------------------------------------------------------------------------
  // [SAX - Interface]
  // --------------------------------------------------------------------------

  virtual err_t onStartDocument() override;
  virtual err_t onEndDocument() override;

  virtual err_t onStartElement(const StubW& tagName) override;
  virtual err_t onEndElement(const StubW& tagName) override;
  virtual err_t onAttribute(const StubW& name, const StubW& value) override;

  virtual err_t onCharacterData(const StubW& data) override;
  virtual err_t onIgnorableWhitespace(const StubW& data) override;

  virtual err_t onDOCTYPE(const List<StringW>& doctype) override;
  virtual err_t onCDATASection(const StubW& data) override;
  virtual err_t onComment(const StubW& data) override;
  virtual err_t onProcessingInstruction(const StubW& target, const StubW& data) override;

  virtual err_t onError(const XmlSaxLocation& location, err_t errorCode) override;
  virtual err_t onFatal(const XmlSaxLocation& location, err_t errorCode) override;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief DOM document.
  DomDocument* _document;
  //! @brief Current node.
  DomContainer* _currentContainer;

private:
  _FOG_NO_COPY(DomSaxHandler)
};

// ============================================================================
// [Fog::DomGCScope]
// ============================================================================

struct FOG_NO_EXPORT DomAutoGC
{
  explicit FOG_INLINE DomAutoGC(DomDocument& document) :
    _document(&document)
  {
    _document->_gc.enterScope(&_scope);
  }

  explicit FOG_INLINE DomAutoGC(DomDocument* document) :
    _document(document)
  {
    if (_document != NULL)
      _document->_gc.enterScope(&_scope);
  }

  FOG_INLINE ~DomAutoGC()
  {
    if (_document != NULL)
      _document->_gc.leaveScope(&_scope);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to @ref DomDocument.
  DomDocument* _document;
  //! @brief Scope data.
  MemGCScope _scope;

private:
  _FOG_NO_COPY(DomAutoGC)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_DOM_DOM_H
