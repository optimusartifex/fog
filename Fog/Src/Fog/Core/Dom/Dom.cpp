// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Dom/Dom.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/XmlIO.h>

namespace Fog {

// Based on the specifications available online, always preferred newer
// specification against the older one, so we are not using some features
// which were removed from DOM-4. 
//
//   http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/DOM3-Core.html
//
//   - This is the DOM-4 Core specification. I found it very complex, mainly
//     the fact athat Attr inherits from Node. This behavior is extremely
//     complicated and is not supported, becuase it was removed from DOM-4
//     specification, available:
//
//   http://www.w3.org/TR/dom/

// ============================================================================
// [The DOM Structure Model]
// ============================================================================
//
// The DOM presents documents as a hierarchy of DomNode objects that also 
// implement other, more specialized interfaces. Some types of nodes may have
// child nodes of various types, and others are leaf nodes that cannot have
// anything below them in the document structure. For XML and HTML, the node
// types, and which node types they may have as children, are as follows:
//
// +--------------------------+--------------------------+--------+-----------+
// | Type                     | Children                 | Parent | Notes     |
// +--------------------------+--------------------------+--------+-----------+
// | DomCDATASection          |                          | YES    |           |
// +--------------------------+--------------------------+--------+-----------+
// | DomComment               |                          | YES    |           |
// +--------------------------+--------------------------+--------+-----------+
// | DomDocument              | DomComment               | NULL   |           |
// |                          | DomDocumentType (0..1)   |        |           |
// |                          | DomElement (0..1)        |        |           |
// |                          | DomProcessingInstruction |        |           |
// +--------------------------+--------------------------+--------+-----------+
// | DomDocumentFragment      | DomCDATASection          | NULL   |           |
// |                          | DomComment               |        |           |
// |                          | DomElement               |        |           |
// |                          | DomProcessingInstruction |        |           |
// |                          | DomText                  |        |           |
// +--------------------------+--------------------------+--------+-----------+
// | DomDocumentType          |                          | YES    |           |
// +--------------------------+--------------------------+--------+-----------+
// | DomElement               | DomCDATASection          | YES    |           |
// |                          | DomComment               |        |           |
// |                          | DomElement               |        |           |
// |                          | DomProcessingInstruction |        |           |
// |                          | DomText                  |        |           |
// +--------------------------+--------------------------+--------+-----------+
// | DomProcessingInstruction |                          | YES    |           |
// +--------------------------+--------------------------+--------+-----------+
// | DomText                  |                          | YES    |           |
// +--------------------------+--------------------------+--------+-----------+
//
// For-Framework DOM implementation also adds some other interfaces, which were
// used to build the final structure:
//
// - DomObj       - Base interface for all DOM objects.
//
// - DomContainer    - Object which can contain child nodes. For example 
//                     DomElement and DomDocument inherits DomContainer, but 
//                     DomText doesn't.

#define M(_Type_) (1 << (DOM_NODE_TYPE_##_Type_))
static const uint32_t DomUtil_hierarchyMap[] =
{
  // NOTE: We allow DOCUMENT_FRAGMENT to be here, this is just quick check.

  // 00: NONE (Doesn't exist).
  0,

  // 01: ELEMENT
  M(CDATA_SECTION)          |
  M(COMMENT)                |
  M(DOCUMENT_FRAGMENT)      |
  M(ELEMENT)                |
  M(PROCESSING_INSTRUCTION) |
  M(TEXT),

  // 02: ATTRIBUTE (Removed in Web-DOM4).
  0, 

  // 03: TEXT
  0,

  // 04: CDATA_SECTION (Removed in Web-DOM4).
  0,

  // 05: ENTITY_REFERENCE (Removed in Web-DOM4).
  M(CDATA_SECTION)          |
  M(COMMENT)                |
  M(DOCUMENT_FRAGMENT)      |
  M(ELEMENT)                |
  M(PROCESSING_INSTRUCTION) |
  M(TEXT),

  // 06: ENTITY (Removed in Web-DOM4).
  0,

  // 07: PROCESSING_INSTRUCTION.
  0,

  // 08: COMMENT.
  0,

  // 09: DOCUMENT.
  M(COMMENT)                |
  M(DOCUMENT_FRAGMENT)      |
  M(DOCUMENT_TYPE)          |
  M(ELEMENT)                |
  M(PROCESSING_INSTRUCTION),

  // 10: DOCUMENT_TYPE.
  0,

  // 11: DOCUMENT_FRAGMENT.
  M(CDATA_SECTION)          |
  M(COMMENT)                |
  M(DOCUMENT_FRAGMENT)      |
  M(ELEMENT)                |
  M(PROCESSING_INSTRUCTION) |
  M(TEXT),

  // 12: NOTATION (Removed in Web-DOM4).
  0
};
#undef M

// ============================================================================
// [Fog::Dom - Defs]
// ============================================================================

#define FOG_DOM_RELEASE(_Type_) \
  FOG_MACRO_BEGIN \
    size_t oldReference = _reference.subXchg(1); \
    FOG_ASSERT(oldReference != 0); \
    \
    /* If the reference has not been decreased to zero then there is nothing */ \
    /* to do.                                                                */ \
    if (oldReference > 1) \
      return; \
    \
    /* Detect double-release also in release-mode and prevend errors which   */ \
    /* can happen when DomNode was released by mistake. This is fatal error, */ \
    /* but we try try to survive.                                            */ \
    if (FOG_UNLIKELY(oldReference == 0)) \
    { \
      _reference.inc(); \
      \
      Logger::fatal("Fog::" #_Type_, "release", \
        "Detected double-release, trying to survive."); \
      return; \
    } \
  FOG_MACRO_END

#define FOG_DOM_TO_GC_QUEUE(_Type_) \
  FOG_MACRO_BEGIN \
    DomDocument* document = getOwnerDocument(); \
    FOG_ASSERT(document != NULL); \
    \
    document->_gc.mark(this); \
  FOG_MACRO_END


// ============================================================================
// [Fog::Dom - Util]
// ============================================================================

static FOG_INLINE bool DomUtil_isWhitespaceOnly(const CharW* sData, size_t sLength)
{
  for (size_t i = 0; i < sLength; i++)
  {
    if (!sData[i].isSpace())
      return false;
  }
  return true;
}

// ============================================================================
// [Fog::DomObj - Construction / Destruction]
// ============================================================================

DomObj::DomObj() {}
DomObj::~DomObj() {}

// ============================================================================
// [Fog::DomContainer_ChildNodeList - Declaration]
// ============================================================================

//! @internal
//! 
//! @brief DomListNode implementation used exclusively by @ref DomContainer.
struct FOG_NO_EXPORT DomContainer_ChildNodeList : public DomNodeList
{
  FOG_DOM_OBJ(DomContainer_ChildNodeList, DomNodeList)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomContainer_ChildNodeList();
  virtual ~DomContainer_ChildNodeList();

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
  // [DOM Interface]
  // --------------------------------------------------------------------------

  virtual DomNode* getNode(size_t index) const override;
  virtual size_t getLength() const override;
};

// ============================================================================
// [Fog::DomContainer_ChildNodeList - Construction / Destruction]
// ============================================================================

DomContainer_ChildNodeList::DomContainer_ChildNodeList() :
  DomNodeList()
{
}

DomContainer_ChildNodeList::~DomContainer_ChildNodeList()
{
}

// ============================================================================
// [Fog::DomContainer_ChildNodeList - Helpers]
// ============================================================================

static FOG_INLINE DomContainer* DomContainer_ChildNodeList_getOwner(
  const DomContainer_ChildNodeList* self)
{
  return (DomContainer*)((uintptr_t)self - FOG_OFFSET_OF(DomContainer, _childNodeList));
}

// ============================================================================
// [Fog::DomContainer_ChildNodeList - AddRef / Release]
// ============================================================================

DomObj* DomContainer_ChildNodeList::_addRef()
{
  DomContainer_ChildNodeList_getOwner(this)->addRef();
  return this;
}

void DomContainer_ChildNodeList::_release()
{
  DomContainer_ChildNodeList_getOwner(this)->release();
}

// Static, never collected.
bool DomContainer_ChildNodeList::_canCollect() const
{
  return false;
}

// ============================================================================
// [Fog::DomContainer_ChildNodeList - NodeList]
// ============================================================================

static err_t DomContainer_updateCache(DomContainer* self);

DomNode* DomContainer_ChildNodeList::getNode(size_t index) const
{
  DomContainer* owner = DomContainer_ChildNodeList_getOwner(this);

  if (FOG_UNLIKELY((owner->_nodeFlags & DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST) != 0))
  {
    err_t err = DomContainer_updateCache(owner);

    // If update cache failed then it must be memory allocation error. So we
    // have no array and we have to do lookup manually. This situation is rare
    // and it will probably never happen, but if it does, then the behavior
    // of this method will be as expected from the caller.
    if (FOG_IS_ERROR(err))
    {
      DomNode* node = owner->_firstChild;
      for (;;)
      {
        if (index == 0)
          return node;
        if (node == NULL)
          return NULL;
        node = node->_nextSibling;
      }
    }
  }

  if (index >= owner->_childNodeArray.getLength())
    return NULL;
  else
    return owner->_childNodeArray.getAt(index);
}

size_t DomContainer_ChildNodeList::getLength() const
{
  DomContainer* owner = DomContainer_ChildNodeList_getOwner(this);

  if (FOG_UNLIKELY((owner->_nodeFlags & DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST) != 0))
  {
    err_t err = DomContainer_updateCache(owner);

    // If update cache failed then calculate the length on-the-fly..
    if (FOG_IS_ERROR(err))
    {
      DomNode* node = owner->_firstChild;
      size_t length = 0;
      
      while (node)
      {
        length++;
        node = node->_nextSibling;
      }
      
      return length;
    }
  }

  return owner->_childNodeArray.getLength();
}

// ============================================================================
// [Fog::DomNode - Helpers]
// ============================================================================

static FOG_INLINE bool DomNode_isContainer(const DomNode* node)
{
  return node->isElement() ||
         node->isDocument() ||
         node->isDocumentFragment();
}

static FOG_INLINE bool DomNode_isCharacterData(const DomNode* node)
{
  return node->isText() ||
         node->isCDATASection() ||
         node->isComment() ||
         node->isProcessingInstruction();
}

static FOG_INLINE bool DomNode_isDocumentCompatible(const DomNode* base, const DomNode* node)
{
  DomDocument* nodeDocument = node->_ownerDocument;

  if (base->isDocument())
    return base == nodeDocument;
  else
    return base->_ownerDocument == nodeDocument;
}

static FOG_INLINE bool DomNode_isAncestorOrItself(const DomNode* base, const DomNode* node)
{
  do {
    if (base == node)
      return true;
    node = node->_parentNode;
  } while (node != NULL);
  
  return false;
}

static FOG_INLINE bool DomNode_isChildNodeCompatible(uint32_t baseNodeType, uint32_t childNodeType)
{
  if (baseNodeType >= FOG_ARRAY_SIZE(DomUtil_hierarchyMap))
    return false;

  if (childNodeType >= FOG_ARRAY_SIZE(DomUtil_hierarchyMap))
    return false;

  return (DomUtil_hierarchyMap[baseNodeType] & (1 << childNodeType)) != 0;
}

static FOG_NO_INLINE bool DomNode_isDocumentFragmentCompatible(const DomNode* base, const DomNode* node)
{
  uint32_t baseNodeType = base->_nodeType;

  FOG_ASSERT(baseNodeType < FOG_ARRAY_SIZE(DomUtil_hierarchyMap));
  FOG_ASSERT(node->_nodeType == DOM_NODE_TYPE_DOCUMENT_FRAGMENT);

  node = static_cast<const DomDocumentFragment*>(node)->_firstChild;

  // Special handling for DomDocument. The DomDocumentType and DomElement can be
  // added only once there. So we just count these and fail if the counter exceeds
  // one.
  if (baseNodeType == DOM_NODE_TYPE_DOCUMENT)
  {
    bool hasDocumentType = static_cast<const DomDocument*>(base)->_documentType;
    bool hasDocumentElement = static_cast<const DomDocument*>(base)->_documentElement;

    while (node != NULL)
    {
      uint32_t childNodeType = node->_nodeType;
      if ((DomUtil_hierarchyMap[baseNodeType] & (1 << childNodeType)) == 0)
        return false;

      // If fragment contains DomDocumentType/DomElement, but it's already
      // contained in DomDocument then the whole DocumentFragment will be refused.
      if ((childNodeType == DOM_NODE_TYPE_DOCUMENT_TYPE && hasDocumentType) ||
          (childNodeType == DOM_NODE_TYPE_ELEMENT && hasDocumentElement))
        return false;

      node = node->_nextSibling;
    }
  }
  else
  {
    while (node != NULL)
    {
      uint32_t childNodeType = node->_nodeType;
      if ((DomUtil_hierarchyMap[baseNodeType] & (1 << childNodeType)) == 0)
        return false;
      node = node->_nextSibling;
    }
  }

  return true;
}

static FOG_INLINE void DomDocument_onChildRemove(DomDocument* self, DomNode* child)
{
  if (child->getNodeType() == DOM_NODE_TYPE_ELEMENT)
    self->_documentElement = NULL;
  else if (child->getNodeType() == DOM_NODE_TYPE_DOCUMENT_TYPE)
    self->_documentType = NULL;
}

static FOG_INLINE void DomDocument_onChildRemove(DomDocument* self, DomNode* first, DomNode* last)
{
  DomNode* node = first;
  for (;;)
  {
    DomDocument_onChildRemove(self, node);
    if (node == last)
      break;
    node = node->_nextSibling;
  }
}

static FOG_INLINE void DomDocument_onChildAdd(DomDocument* self, DomNode* child)
{
  if (child->getNodeType() == DOM_NODE_TYPE_ELEMENT)
    self->_documentElement = static_cast<DomElement*>(child);
  else if (child->getNodeType() == DOM_NODE_TYPE_DOCUMENT_TYPE)
    self->_documentType = static_cast<DomDocumentType*>(child);
}

static FOG_INLINE void DomDocument_onChildAdd(DomDocument* self, DomNode* first, DomNode* last)
{
  DomNode* node = first;
  for (;;)
  {
    DomDocument_onChildAdd(self, node);
    if (node == last)
      break;
    node = node->_nextSibling;
  }
}

static err_t DomNode_insertFragment(DomContainer* self, DomNode* after, DomDocumentFragment* fragment)
{
  FOG_ASSERT(fragment->_nodeType == DOM_NODE_TYPE_DOCUMENT_FRAGMENT);

  DomNode* fragmentFirstChild = fragment->_firstChild;
  DomNode* fragmentLastChild = fragment->_lastChild;

  // Just consistency checking.
  FOG_ASSERT(fragmentFirstChild != NULL);
  FOG_ASSERT(fragmentFirstChild->_previousSibling == NULL);

  FOG_ASSERT(fragmentLastChild != NULL);
  FOG_ASSERT(fragmentLastChild->_nextSibling == NULL);

  fragment->_onChildRemove(fragmentFirstChild, fragmentLastChild);

  if (after == NULL)
  {
    DomNode* selfFirstChild = self->_firstChild;

    // Prepend.
    self->_firstChild = fragmentFirstChild;

    // If container is empty then it's just reparent from fragment to self.
    if (selfFirstChild == NULL)
    {
      FOG_ASSERT(self->_lastChild == NULL);
      self->_lastChild = fragmentLastChild;
    }
    else
    {
      selfFirstChild->_previousSibling = fragmentLastChild;
      fragmentLastChild->_nextSibling = selfFirstChild;
    }
  }
  else
  {
    DomNode* afterRight = after->_nextSibling;

    after->_nextSibling = fragmentFirstChild;
    fragmentFirstChild->_previousSibling = after;

    if (afterRight == NULL)
    {
      self->_lastChild = fragmentLastChild;
    }
    else
    {
      afterRight->_previousSibling = fragmentLastChild;
      fragmentLastChild->_nextSibling = afterRight;
    }
  }

  // Clear DocumentFragment.
  fragment->_firstChild = NULL;
  fragment->_lastChild = NULL;

  fragment->_nodeFlags &= ~(
    DOM_NODE_FLAG_HAS_CHILD_NODES |
    DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST);
  fragment->_childNodeArray.clear();

  DomNode* p = fragmentFirstChild;
  for (;;)
  {
    p->_parentNode = self;
    if (p == fragmentLastChild)
      break;
    p = p->_nextSibling;
  }

  // Handle DomDocument.
  if (self->isDocument())
    DomDocument_onChildAdd(static_cast<DomDocument*>(self), fragmentFirstChild, fragmentLastChild);

  self->_nodeFlags |= DOM_NODE_FLAG_HAS_CHILD_NODES |
                      DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST;
  self->_onChildAdd(fragmentFirstChild, fragmentLastChild);

  return ERR_OK;
}

static err_t DomNode_unlinkSafe(DomNode* node)
{
  DomContainer* parentNode = static_cast<DomContainer*>(node->_parentNode);

  if (parentNode == NULL)
    return ERR_OK;

  // If parent is read-only then we really can't unlink.
  if (parentNode->isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  parentNode->_onChildRemove(node, node);

  DomNode* prev = node->_previousSibling;
  DomNode* next = node->_nextSibling;

  if (prev != NULL)
    prev->_nextSibling = next;
  else
    parentNode->_firstChild = next;
  
  if (next != NULL)
    next->_previousSibling = prev;
  else
    parentNode->_lastChild = prev;

  // Caller should reinitialize these members, but I'd like to be
  // sure they are cleared, becuase if somebody forget to clean them
  // then we get into the serious problem.
  node->_parentNode = NULL;
  node->_previousSibling = NULL;
  node->_nextSibling = NULL;

  // Handle DomDocument.
  if (parentNode->isDocument())
    DomDocument_onChildRemove(static_cast<DomDocument*>(parentNode), node);

  if (parentNode->_firstChild == NULL)
  {
    parentNode->_nodeFlags &= ~(
      DOM_NODE_FLAG_HAS_CHILD_NODES |
      DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST);
    parentNode->_childNodeArray.clear();
  }
  else
  {
    parentNode->_nodeFlags |= DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST;
  }

  return ERR_OK;
}

static err_t DomNode_insertAfterSafe(DomContainer* self, DomNode* newChild, DomNode* refChild)
{
  FOG_ASSERT(newChild != NULL);
  FOG_ASSERT(newChild != refChild);

  FOG_ASSERT(refChild != NULL);
  FOG_ASSERT(refChild->_parentNode == self);

  uint32_t thisNodeType = self->_nodeType;
  uint32_t childNodeType = newChild->_nodeType;

  if (!DomNode_isChildNodeCompatible(thisNodeType, childNodeType))
    return ERR_DOM_HIERARCHY_REQUEST;

  // Handle DocumentFragment.
  if (childNodeType == DOM_NODE_TYPE_DOCUMENT_FRAGMENT)
  {
    if (static_cast<DomDocumentFragment*>(newChild)->_firstChild == NULL)
      return ERR_OK;

    FOG_ASSERT(newChild->_parentNode == NULL);
    if (!DomNode_isDocumentFragmentCompatible(self, newChild))
      return ERR_DOM_HIERARCHY_REQUEST;

    return DomNode_insertFragment(self, refChild, static_cast<DomDocumentFragment*>(newChild));
  }

  // Generate error if newChild is 'this' or ancestor.
  if (DomNode_isAncestorOrItself(self, newChild))
    return ERR_DOM_HIERARCHY_REQUEST;

  // Unlink.
  FOG_RETURN_ON_ERROR(DomNode_unlinkSafe(newChild));

  // Insert.
  newChild->_parentNode = self;

  DomNode* refChildAfter = refChild->_nextSibling;

  newChild->_previousSibling = refChild;
  refChild->_nextSibling = newChild;

  if (refChildAfter != NULL)
  {
    refChildAfter->_previousSibling = newChild;
    newChild->_nextSibling = refChildAfter;
  }
  else
  {
    self->_lastChild = newChild;
    newChild->_nextSibling = NULL;
  }

  // Handle DomDocument.
  if (self->isDocument())
    DomDocument_onChildAdd(static_cast<DomDocument*>(self), newChild);

  self->_nodeFlags |= DOM_NODE_FLAG_HAS_CHILD_NODES |
                      DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST;

  self->_onChildAdd(newChild, newChild);

  return ERR_OK;
}

static err_t DomContainer_removeChildNodes(DomContainer* self)
{
  FOG_ASSERT((self->_nodeFlags & DOM_NODE_FLAG_HAS_CHILD_NODES) != 0);

  DomDocument* ownerDocument = self->getOwnerDocument();

  DomNode* node = self->_firstChild;
  self->_onChildRemove(node, self->_lastChild);

  do {
    DomNode* next = node->_nextSibling;

    node->_parentNode = NULL;
    node->_nextSibling = NULL;
    node->_previousSibling = NULL;

    if (node->_reference.get() == 0 && (node->_nodeFlags & DOM_NODE_FLAG_IN_GC_QUEUE) == 0)
      ownerDocument->_gc.mark(node);

    node = next;
  } while (node);

  if (self->isDocument())
  {
    static_cast<DomDocument*>(self)->_documentElement = NULL;
    static_cast<DomDocument*>(self)->_documentType = NULL;
  }

  self->_nodeFlags &= ~(DOM_NODE_FLAG_HAS_CHILD_NODES | DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST);
  self->_childNodeArray.reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::DomNode - Construction / Destruction]
// ============================================================================

DomNode::DomNode(DomDocument* ownerDocument, uint32_t nodeType) :
  _vType(VAR_TYPE_DOM_NODE),
  _nodeType(static_cast<uint8_t>(nodeType & 0xFF)),
  _nodeFlags(NO_FLAGS),
  _objectType(nodeType),
  _objectPropertyCount(0),
  _ownerDocument(ownerDocument),
  _parentNode(NULL),
  _previousSibling(NULL),
  _nextSibling(NULL)
{
  _reference.init(0);
}

DomNode::~DomNode()
{
}

// ============================================================================
// [Fog::DomNode - AddRef / Release]
// ============================================================================

DomObj* DomNode::_addRef()
{
  _reference.inc();
  return this;
}

void DomNode::_release()
{
  FOG_DOM_RELEASE(DomNode);
  if (_parentNode != NULL)
    return;
  FOG_DOM_TO_GC_QUEUE(DomNode);
}

bool DomNode::_canCollect() const
{
  return _parentNode == NULL && _reference.get() == 0;
}

// ============================================================================
// [Fog::DomNode - Node Name]
// ============================================================================

const InternedStringW& DomNode::getNodeName() const
{
  switch (_nodeType)
  {
    case DOM_NODE_TYPE_CDATA_SECTION:
      return FOG_S(_cdata_section);
    case DOM_NODE_TYPE_COMMENT:
      return FOG_S(_comment);
    case DOM_NODE_TYPE_DOCUMENT:
      return FOG_S(_document);
    case DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
      return FOG_S(_document_fragment);
    case DOM_NODE_TYPE_DOCUMENT_TYPE:
      return static_cast<const DomDocumentType*>(this)->_name;
    case DOM_NODE_TYPE_ELEMENT:
      return static_cast<const DomElement*>(this)->_tagName;
    case DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
      return static_cast<const DomProcessingInstruction*>(this)->_target;
    case DOM_NODE_TYPE_TEXT:
      return FOG_S(_text);
    default:
      return InternedStringW::getEmptyInstance();
  }
}

// ============================================================================
// [Fog::DomNode - Node Value]
// ============================================================================

StringW DomNode::getNodeValue() const
{
  switch (_nodeType)
  {
    case DOM_NODE_TYPE_CDATA_SECTION:
    case DOM_NODE_TYPE_COMMENT:
    case DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
    case DOM_NODE_TYPE_TEXT:
      return static_cast<const DomCharacterData*>(this)->_data;

    case DOM_NODE_TYPE_DOCUMENT:
    case DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
    case DOM_NODE_TYPE_DOCUMENT_TYPE:
    case DOM_NODE_TYPE_ELEMENT:
    default:
      return StringW();
  }
}

err_t DomNode::setNodeValue(const StringW& value)
{
  switch (_nodeType)
  {
    case DOM_NODE_TYPE_CDATA_SECTION:
    case DOM_NODE_TYPE_COMMENT:
    case DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
    case DOM_NODE_TYPE_TEXT:
      return static_cast<DomCharacterData*>(this)->setData(value);

    // When node value it is defined to be null, setting it has no effect,
    // including if the node is read-only.
    case DOM_NODE_TYPE_DOCUMENT:
    case DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
    case DOM_NODE_TYPE_DOCUMENT_TYPE:
    case DOM_NODE_TYPE_ELEMENT:
    default:
      return ERR_OK;
  }
}

// ============================================================================
// [Fog::DomNode - Hierarchy]
// ============================================================================

DomNode* DomNode::getFirstChild() const
{
  if (hasChildNodes())
    return static_cast<const DomContainer*>(this)->_firstChild;
  else
    return NULL;
}

DomNode* DomNode::getLastChild() const
{
  if (hasChildNodes())
    return static_cast<const DomContainer*>(this)->_lastChild;
  else
    return NULL;
}

DomNodeList* DomNode::getChildNodes() const
{
  switch (_nodeType)
  {
    case DOM_NODE_TYPE_DOCUMENT:
    case DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
    case DOM_NODE_TYPE_ELEMENT:
      static_cast<const DomContainer*>(this)->getChildNodes();

    default:
      return NULL;
  }
}

bool DomNode::contains(DomNode* refChild, bool deep) const
{
  if (!hasChildNodes())
    return false;

  DomNode* node = refChild->_parentNode;
  if (!deep)
    return node == this;

  while (node)
  {
    if (node == this)
      return true;
    node = node->_parentNode;
  } while (node);
  return false;      
}

err_t DomNode::prependChild(DomNode* newChild)
{
  if (newChild == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  if (!DomNode_isDocumentCompatible(this, newChild))
    return ERR_DOM_WRONG_DOCUMENT;

  uint32_t thisNodeType = _nodeType;
  uint32_t childNodeType = newChild->_nodeType;

  if (!DomNode_isChildNodeCompatible(thisNodeType, childNodeType))
    return ERR_DOM_HIERARCHY_REQUEST;

  // Handle DocumentFragment.
  if (childNodeType == DOM_NODE_TYPE_DOCUMENT_FRAGMENT)
  {
    if (static_cast<DomDocumentFragment*>(newChild)->_firstChild == NULL)
      return ERR_OK;

    // DocumentFragment has never parentNode.
    FOG_ASSERT(newChild->_parentNode == NULL);

    if (!DomNode_isDocumentFragmentCompatible(this, newChild))
      return ERR_DOM_HIERARCHY_REQUEST;

    return DomNode_insertFragment(
      static_cast<DomContainer*>(this),
      NULL,
      static_cast<DomDocumentFragment*>(newChild));
  }

  // Generate error if newChild is 'this' or ancestor.
  if (DomNode_isAncestorOrItself(this, newChild))
    return ERR_DOM_HIERARCHY_REQUEST;

  // Unlink.
  FOG_RETURN_ON_ERROR(DomNode_unlinkSafe(newChild));

  // Prepend.
  DomContainer* self = static_cast<DomContainer*>(this);

  newChild->_parentNode = self;
  newChild->_nextSibling = self->_firstChild;

  if (self->_firstChild == NULL)
  {
    self->_firstChild = newChild;
    self->_lastChild = newChild;
  }
  else
  {
    self->_firstChild->_previousSibling = newChild;
    self->_firstChild = newChild;
  }

  // Handle DomDocument.
  if (self->isDocument())
    DomDocument_onChildAdd(static_cast<DomDocument*>(self), newChild);

  _nodeFlags |= DOM_NODE_FLAG_HAS_CHILD_NODES |
                DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST;
  _onChildAdd(newChild, newChild);

  return ERR_OK;
}

err_t DomNode::appendChild(DomNode* newChild)
{
  if (newChild == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  if (!DomNode_isDocumentCompatible(this, newChild))
    return ERR_DOM_WRONG_DOCUMENT;

  uint32_t thisNodeType = _nodeType;
  uint32_t childNodeType = newChild->_nodeType;

  if (!DomNode_isChildNodeCompatible(thisNodeType, childNodeType))
    return ERR_DOM_HIERARCHY_REQUEST;

  // Handle DocumentFragment.
  if (childNodeType == DOM_NODE_TYPE_DOCUMENT_FRAGMENT)
  {
    if (static_cast<DomDocumentFragment*>(newChild)->_firstChild == NULL)
      return ERR_OK;

    // DocumentFragment has never parentNode.
    FOG_ASSERT(newChild->_parentNode == NULL);

    if (!DomNode_isDocumentFragmentCompatible(this, newChild))
      return ERR_DOM_HIERARCHY_REQUEST;

    return DomNode_insertFragment(
      static_cast<DomContainer*>(this),
      static_cast<DomContainer*>(this)->_lastChild,
      static_cast<DomDocumentFragment*>(newChild));
  }

  // Generate error if newChild is 'this' or ancestor.
  if (DomNode_isAncestorOrItself(this, newChild))
    return ERR_DOM_HIERARCHY_REQUEST;

  // Unlink.
  FOG_RETURN_ON_ERROR(DomNode_unlinkSafe(newChild));

  // Append.
  DomContainer* self = static_cast<DomContainer*>(this);

  newChild->_parentNode = self;
  newChild->_previousSibling = self->_lastChild;

  if (self->_firstChild == NULL)
  {
    self->_firstChild = newChild;
    self->_lastChild = newChild;
  }
  else
  {
    self->_lastChild->_nextSibling = newChild;
    self->_lastChild = newChild;
  }

  // Handle DomDocument.
  if (self->isDocument())
    DomDocument_onChildAdd(static_cast<DomDocument*>(self), newChild);

  _nodeFlags |= DOM_NODE_FLAG_HAS_CHILD_NODES |
                DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST;
  _onChildAdd(newChild, newChild);

  return ERR_OK;
}

err_t DomNode::removeChild(DomNode* oldChild)
{
  if (oldChild == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  if (oldChild->_parentNode != this)
    return ERR_DOM_NOT_FOUND;

  return oldChild->unlink();
}

err_t DomNode::removeChildNodes()
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  if ((_nodeFlags & DOM_NODE_FLAG_HAS_CHILD_NODES) != 0)
    return DomContainer_removeChildNodes(static_cast<DomContainer*>(this));

  return ERR_OK;
}

err_t DomNode::insertBefore(DomNode* newChild, DomNode* refChild)
{
  if (refChild == NULL)
    return appendChild(newChild);

  if (newChild == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  if (!DomNode_isDocumentCompatible(this, newChild))
    return ERR_DOM_WRONG_DOCUMENT;

  if (refChild->_parentNode != this)
    return ERR_DOM_NOT_FOUND;

  if (newChild == refChild)
    return ERR_OK;

  if (refChild->_previousSibling == NULL)
    return prependChild(newChild);
  else
    return DomNode_insertAfterSafe(static_cast<DomContainer*>(this), newChild, refChild);
}

err_t DomNode::insertAfter(DomNode* newChild, DomNode* refChild)
{
  if (refChild == NULL)
    return appendChild(newChild);

  if (newChild == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  if (!DomNode_isDocumentCompatible(this, newChild))
    return ERR_DOM_WRONG_DOCUMENT;

  if (refChild->_parentNode != this)
    return ERR_DOM_NOT_FOUND;

  if (newChild == refChild)
    return ERR_OK;

  if (refChild->_nextSibling == NULL)
    return appendChild(newChild);
  else
    return DomNode_insertAfterSafe(static_cast<DomContainer*>(this), newChild, refChild);
}

err_t DomNode::replaceChild(DomNode* newChild, DomNode* refChild)
{
  if (newChild == NULL)
    return ERR_RT_INVALID_ARGUMENT;

  if (refChild == NULL)
    return ERR_RT_INVALID_ARGUMENT;
  
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  DomContainer* newChildParent = newChild->_parentNode;
  if (newChildParent != NULL && newChildParent->isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  if (refChild->_parentNode != this)
    return ERR_DOM_NOT_FOUND;

  if (this->isDocument())
  {
    // DomDocument can contain only one DomElement and one DomDocumentType
    // node. We need to check if this assumption will is valid before we
    // try to manipulate DOM.
    DomDocument* doc = static_cast<DomDocument*>(this);

    size_t domElementCount = doc->_documentElement != NULL;
    size_t domDocumentTypeCount = doc->_documentType != NULL;

    domElementCount -= refChild == doc->_documentElement;
    domDocumentTypeCount -= refChild == doc->_documentType;
    
    if (newChild->isDocumentFragment())
    {
      DomNode* node = static_cast<DomDocumentFragment*>(newChild)->_firstChild;
      while (node != NULL)
      {
        domElementCount += node->isElement();
        domDocumentTypeCount += node->isDocumentType();
        node = node->_nextSibling;
      }
    }
    else
    {
      domElementCount += newChild->isElement();
      domDocumentTypeCount += newChild->isDocumentType();
    }

    if (domElementCount > 1 || domDocumentTypeCount > 1)
      return ERR_DOM_HIERARCHY_REQUEST;
  }

  if (this == newChildParent)
  {
    // Unlikely, but possible.
    if (newChild == refChild)
      return ERR_OK;

    // We don't need to check for document compatibility and child
    // compatibility, because all the two children are in document,
    // so they must be compatible to the DOM. So basically we need
    // to remove the refChild, and move newChild into the location
    // where refChild was.
    DomNode* prev = refChild->_previousSibling;

    FOG_RETURN_ON_ERROR(DomNode_unlinkSafe(newChild));
    DomNode_unlinkSafe(refChild);

    if (prev == NULL)
      return prependChild(newChild);
    else
      return DomNode_insertAfterSafe(static_cast<DomContainer*>(this), newChild, prev);
  }

  if (!DomNode_isDocumentCompatible(this, newChild))
    return ERR_DOM_WRONG_DOCUMENT;

  uint32_t thisNodeType = _nodeType;
  uint32_t newNodeType = newChild->_nodeType;

  if (!DomNode_isChildNodeCompatible(thisNodeType, newNodeType))
    return ERR_DOM_HIERARCHY_REQUEST;

  if (newNodeType == DOM_NODE_TYPE_DOCUMENT_FRAGMENT)
  {
    if (static_cast<DomDocumentFragment*>(newChild)->_firstChild == NULL)
      return refChild->unlink();
    
    // DocumentFragment has never parentNode.
    FOG_ASSERT(newChild->_parentNode == NULL);

    if (!DomNode_isDocumentFragmentCompatible(this, newChild))
      return ERR_DOM_HIERARCHY_REQUEST;

    DomNode* prev = refChild->_previousSibling;
    DomNode_unlinkSafe(refChild);

    return DomNode_insertFragment(
      static_cast<DomContainer*>(this), prev, static_cast<DomDocumentFragment*>(newChild));
  }
  else
  {
    // Generate error if newChild is 'this' or ancestor.
    if (DomNode_isAncestorOrItself(this, newChild))
      return ERR_DOM_HIERARCHY_REQUEST;

    // We can simply replace refChild with newChild.
    FOG_RETURN_ON_ERROR(DomNode_unlinkSafe(newChild));

    _onChildRemove(refChild, refChild);

    DomNode* prev = refChild->_previousSibling;
    DomNode* next = refChild->_nextSibling;

    // Link newChild.
    newChild->_parentNode = refChild->_parentNode;
    newChild->_previousSibling = prev;
    newChild->_nextSibling = next;

    if (prev == NULL)
      static_cast<DomContainer*>(this)->_firstChild = newChild;
    else
      prev->_nextSibling = newChild;

    if (next == NULL)
      static_cast<DomContainer*>(this)->_lastChild = newChild;
    else
      next->_previousSibling = newChild;

    // Unlink refChild.
    refChild->_parentNode = NULL;
    refChild->_nextSibling = NULL;
    refChild->_previousSibling = NULL;

    _nodeFlags |= DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST;
    if (this->isDocument())
      DomDocument_onChildAdd(static_cast<DomDocument*>(this), newChild);

    _onChildAdd(newChild, newChild);
    return ERR_OK;
  }
}

err_t DomNode::unlink()
{
  if (_parentNode == NULL)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(DomNode_unlinkSafe(this));
  if (_reference.get() == 0)
    getOwnerDocument()->_gc.mark(this);
  return ERR_OK;
}

// ============================================================================
// [Fog::DomNode - Clone]
// ============================================================================

DomNode* DomNode::cloneNode(bool deep) const
{
  // TODO:
  return NULL;
}

// ============================================================================
// [Fog::DomNode - Normalize]
// ============================================================================

err_t DomNode::normalize(bool deep)
{
  if (!DomNode_isContainer(this))
    return ERR_OK;

  DomNode* node = reinterpret_cast<DomContainer*>(this)->_firstChild;
  DomText* text;

  while (node != NULL)
  {
    DomNode* next = node->_nextSibling;

    if (node->isText())
    {
      if (static_cast<DomText*>(node)->getData().isEmpty())
      {
        if (node->isReadOnly())
          return ERR_DOM_NO_MODIFICATION_ALLOWED;
        FOG_RETURN_ON_ERROR(text->unlink());
      }
      else if (text != NULL)
      {
        if (node->isReadOnly() || text->isReadOnly())
          return ERR_DOM_NO_MODIFICATION_ALLOWED;

        StringW data = StringW(text->_data, static_cast<DomText*>(node)->_data);

        FOG_RETURN_ON_ERROR(node->unlink());
        FOG_RETURN_ON_ERROR(text->setData(data));
      }
      else
      {
        text = static_cast<DomText*>(node);
      }
    }
    else
    {
      if (deep)
        FOG_RETURN_ON_ERROR(node->normalize(deep));
      text = NULL;
    }

    node = next;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::DomNode - CompareDocumentPosition]
// ============================================================================

uint32_t DomNode::compareDocumentPosition(DomNode* other) const
{
  // 1. Let reference be the context object (this).
  //
  // There is not mentioned which value to return in case that other is NULL,
  // we return NO_FLAGS.
  if (other == NULL)
    return NO_FLAGS;

  // 2. If other and reference are the same object, return zero and terminate
  //    these steps.
  if (this == other)
    return NO_FLAGS;

  // 3. If other and reference are not in the same tree, return 
  //    DOCUMENT_POSITION_DISCONNECTED and terminate these steps.
  if (_ownerDocument != other->_ownerDocument)
    return DOM_POSITION_DISCONNECTED;

  // 4. If other is an ancestor of reference, return the result of adding
  //    DOCUMENT_POSITION_CONTAINS to DOCUMENT_POSITION_PRECEDING and terminate
  //    these steps.
  if (DomNode_isAncestorOrItself(this, other->_parentNode))
    return DOM_POSITION_CONTAINS | DOM_POSITION_PRECEDING;

  // 5. If other is a descendant of reference, return the result of adding
  //    DOCUMENT_POSITION_CONTAINED_BY to DOCUMENT_POSITION_FOLLOWING and
  //    terminate these steps.
  if (DomNode_isAncestorOrItself(this->_parentNode, other))
    return DOM_POSITION_CONTAINED_BY | DOM_POSITION_FOLLOWING;

  // 6. If other is preceding reference return DOCUMENT_POSITION_PRECEDING and 
  //    terminate these steps.
  DomNode* node = other;

  for (;;)
  {
    node = node->_previousSibling;
    if (node == NULL)
      break;
    if (node == this)
      return DOM_POSITION_PRECEDING;
  }

  // 7. Return DOCUMENT_POSITION_FOLLOWING.
  return DOM_POSITION_FOLLOWING;
}

// ============================================================================
// [Fog::DomNode - TextContent]
// ============================================================================

StringW DomNode::getTextContent() const
{
  switch (_nodeType)
  {
    case DOM_NODE_TYPE_ELEMENT:
    case DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
    {
      DomNode* node = static_cast<const DomContainer*>(this)->_firstChild;
      DomNode* t;

      if (node == NULL)
        goto _None;

      StringW textContent;

      // Traverse the whole node-tree and create content from all character-data
      // nodes.
      for (;;)
      {
        if (DomNode_isContainer(node))
        {
          t = static_cast<DomContainer*>(node)->_firstChild;
          if (t != NULL)
          {
            node = t;
            continue;
          }
        }
        else if (DomNode_isCharacterData(node))
        {
          textContent.append(static_cast<DomCharacterData*>(node)->_data);
        }

        for (;;)
        {
          t = node->_nextSibling;
          if (t != NULL)
            break;

          node = node->_parentNode;
          if (node == this)
            return textContent;
        }
      }
    }
    
    case DOM_NODE_TYPE_TEXT:
    case DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
    case DOM_NODE_TYPE_CDATA_SECTION:
    case DOM_NODE_TYPE_COMMENT:
      return static_cast<const DomCharacterData*>(this)->_data;
      
    default:
_None:
      return StringW();
  }
}

err_t DomNode::setTextContent(const StringW& textContent)
{
  switch (_nodeType)
  {
    case DOM_NODE_TYPE_ELEMENT:
    case DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
    {
      // We try to cache our
      DomNode* node = static_cast<DomContainer*>(this)->_firstChild;

      // If the first node is text-content, and it's not used (reference is zero)
      // then instead of creating new node, we reuse this one.
      if (node != NULL)
      {
        if (node->_reference.get() == 0 && node->isText() && !textContent.isEmpty())
          node->addRef();
        else
          node = NULL;
      }

      err_t err = removeChildNodes();

      // Handle the possible error case.
      if (FOG_IS_ERROR(err))
      {
        if (node != NULL)
          node->release();
        return err;
      }

      if (node != NULL)
      {
        err = static_cast<DomText*>(node)->setData(textContent);
        if (FOG_IS_ERROR(err))
        {
          node->release();
          return err;
        }
        
        err = appendChild(node);
        if (FOG_IS_ERROR(err))
          node->release();
      }
      else
      {
        node = _ownerDocument->_createTextNode(textContent);
        if (FOG_IS_NULL(node))
          return ERR_RT_OUT_OF_MEMORY;

        err = appendChild(node);
        if (FOG_IS_ERROR(err))
          _ownerDocument->_gc.mark(node);
      }

      return err;
    }

    case DOM_NODE_TYPE_TEXT:
    case DOM_NODE_TYPE_CDATA_SECTION:
    case DOM_NODE_TYPE_COMMENT:
    case DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
    {
      return static_cast<DomCharacterData*>(this)->setData(textContent);
    }

    default:
      return ERR_DOM_TYPE_MISMATCH;
  }
}

// ============================================================================
// [Fog::DomNode - DOM Events]
// ============================================================================

void DomNode::_onChildAdd(DomNode* firstNode, DomNode* lastNode)
{
  FOG_UNUSED(firstNode);
  FOG_UNUSED(lastNode);
}

void DomNode::_onChildRemove(DomNode* firstNode, DomNode* lastNode)
{
  FOG_UNUSED(firstNode);
  FOG_UNUSED(lastNode);
}

// ============================================================================
// [Fog::DomNodeList - Construction / Destruction]
// ============================================================================

DomNodeList::DomNodeList()
{
}

DomNodeList::~DomNodeList()
{
}

// ============================================================================
// [Fog::DomContainer - Construction / Destruction]
// ============================================================================

DomContainer::DomContainer(DomDocument* ownerDocument, uint32_t nodeType) :
  DomNode(ownerDocument, nodeType),
  _firstChild(NULL),
  _lastChild(NULL)
{
  fog_new_p(_childNodeList) DomContainer_ChildNodeList();
}

DomContainer::~DomContainer()
{
  if ((_nodeFlags & DOM_NODE_FLAG_HAS_CHILD_NODES) != 0)
    DomContainer_removeChildNodes(this);

  ((DomContainer_ChildNodeList*)_childNodeList)->~DomContainer_ChildNodeList();
}

// ============================================================================
// [Fog::DomContainer - UpdateCache]
// ============================================================================

static err_t DomContainer_updateCache(DomContainer* self)
{
  self->_childNodeArray.clear();

  DomNode* node = self->_firstChild;
  while (node != NULL)
  {
    FOG_RETURN_ON_ERROR(self->_childNodeArray.append(node));
    node = node->_nextSibling;
  }

  self->_nodeFlags &= ~DOM_NODE_FLAG_DIRTY_CHILD_NODE_LIST;
  return ERR_OK;
}

// ============================================================================
// [Fog::DomElement - Construction / Destruction]
// ============================================================================

DomElement::DomElement(DomDocument* ownerDocument, const InternedStringW& tagName) :
  DomContainer(ownerDocument, DOM_NODE_TYPE_ELEMENT),
  _tagName(tagName),
  _nextId(NULL)
{
  FOG_DOM_ELEMENT_INIT();
}

DomElement::~DomElement()
{
}

// ============================================================================
// [Fog::DomElement - Properties]
// ============================================================================

//! @internal
//!
//! @brief Property handler used by @c DomElement.
struct FOG_NO_EXPORT DomElement_PropertyHandler
{
  static FOG_INLINE size_t getPropertyIndex(DomElement* self, const InternedStringW& name)
  {
    ListIterator<DomAttr> it(self->_attrArray);

    while (it.isValid())
    {
      if (it.getItem().getName() == name)
        return static_cast<size_t>(self->_objectPropertyCount) + it.getIndex();
      it.next();
    }

    return INVALID_INDEX;
  }

  static FOG_INLINE size_t getPropertyIndex(DomElement* self, const CharW* name, size_t length)
  {
    ListIterator<DomAttr> it(self->_attrArray);

    while (it.isValid())
    {
      if (it.getItem().getName().eqInline(name, length))
        return static_cast<size_t>(self->_objectPropertyCount) + it.getIndex();
      it.next();
    }

    return INVALID_INDEX;
  }

  static FOG_INLINE err_t getPropertyInfo(DomElement* self, size_t index, PropertyInfo& info)
  {
    size_t attrIndex = index - static_cast<size_t>(self->_objectPropertyCount);
    if (attrIndex >= self->_attrArray.getLength())
      return ERR_OBJ_PROPERTY_NOT_FOUND;

    const DomAttr& attr = self->_attrArray[attrIndex];

    info.setName(attr.getName());
    info.setIndex(index);
    info.setType(0);
    info.setFlags(PROPERTY_FLAG_DYNAMIC);

    return ERR_OK;
  }

  static FOG_INLINE err_t getProperty(DomElement* self, size_t index, StringW& value)
  {
    size_t attrIndex = index - static_cast<size_t>(self->_objectPropertyCount);
    if (attrIndex >= self->_attrArray.getLength())
      return ERR_OBJ_PROPERTY_NOT_FOUND;

    return value.set(self->_attrArray[attrIndex].getValue());
  }

  static FOG_INLINE err_t setProperty(DomElement* self, size_t index, const StringW& value)
  {
    size_t attrIndex = index - static_cast<size_t>(self->_objectPropertyCount);
    if (attrIndex >= self->_attrArray.getLength())
      return ERR_OBJ_PROPERTY_NOT_FOUND;

    return const_cast<DomAttr&>(self->_attrArray[attrIndex]).setValue(value);
  }
  
  static FOG_INLINE err_t resetProperty(DomElement* self, size_t index)
  {
    size_t attrIndex = index - static_cast<size_t>(self->_objectPropertyCount);
    if (attrIndex >= self->_attrArray.getLength())
      return ERR_OBJ_PROPERTY_NOT_FOUND;

    err_t err = self->_attrArray.removeAt(attrIndex);

    // If count of dynamic properties is decreased to zero we free the storage
    // in array where attributes are stored.
    if (self->_attrArray.getLength() == 0)
      self->_attrArray.reset();

    return err;
  }
};

err_t DomElement::_unresolvedProperty(size_t& newIndex,
  const CharW* name, size_t nameLength, const InternedStringW* nameInterned,
  const StringW* initialValue)
{
  size_t attrBase = _objectPropertyCount;
  size_t attrIndex = attrBase + _attrArray.getLength();
  err_t err;

  if (initialValue == NULL)
    initialValue = &StringW::getEmptyInstance();

  if (nameInterned != NULL)
  {
    err = _attrArray.append(DomAttr(*nameInterned, *initialValue));
  }
  else
  {
    InternedStringW nameInterned_(StubW(name, nameLength));
    err = _attrArray.append(DomAttr(nameInterned_, *initialValue));
  }

  if (err == ERR_OK)
    newIndex = attrIndex;
  return err;
}

FOG_CORE_OBJ_DEF(DomElement)
  FOG_CORE_OBJ_PROPERTY_DEFAULT(Id, FOG_S(id))
  FOG_CORE_OBJ_HANDLER(DomElement_PropertyHandler)
FOG_CORE_OBJ_END()

// ============================================================================
// [Fog::DomElement - Attributes]
// ============================================================================

StringW DomElement::getAttribute(const StringW& name) const
{
  StringW result;
  getProperty(name, result);
  return result;
}

StringW DomElement::getAttribute(const StubW& name) const
{
  StringW result;
  getProperty(name, result);
  return result;
}

// ============================================================================
// [Fog::DomElement - Id]
// ============================================================================

err_t DomElement::setId(const StringW& id)
{
  if (_id == id)
    return ERR_OK;

  // Remove old ID if used.
  if (!_id.isEmpty())
  {
    getOwnerDocument()->_idHash.remove(this);
  }

  // This should never fail.
  err_t err = _id.set(id);
  FOG_ASSERT(err == ERR_OK);

  // Add new ID if used.
  if (!_id.isEmpty())
  {
    // Ensure that the hash code stored in _id is valid, because it's accessed
    // directly without calling getHashCode() again.
    _id.getHashCode();
    getOwnerDocument()->_idHash.add(this);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::DomElement - GetElementsByTagName]
// ============================================================================

DomNodeList* DomElement::getElementsByTagName(const StringW& name) const
{
  // TODO:
  FOG_ASSERT(0);
  return NULL;
}

DomNodeList* DomElement::getElementsByTagName(const StubW& name) const
{
  // TODO:
  FOG_ASSERT(0);
  return NULL;
}

// ============================================================================
// [Fog::DomCharacterData - Construction / Destruction]
// ============================================================================

DomCharacterData::DomCharacterData(DomDocument* ownerDocument, uint32_t nodeType, const StringW& data) :
  DomNode(ownerDocument, nodeType),
  _data(data)
{
  if (DomUtil_isWhitespaceOnly(_data.getData(), _data.getLength()))
    _nodeFlags |= DOM_NODE_FLAG_WHITESPACE_CONTENT;
}

DomCharacterData::~DomCharacterData()
{
}

// ============================================================================
// [Fog::DomCharacterData - DOM Interface]
// ============================================================================

err_t DomCharacterData::setData(const StringW& data)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;
  FOG_RETURN_ON_ERROR(_data.set(data));

  _nodeFlags &= ~DOM_NODE_FLAG_WHITESPACE_CONTENT;
  if (DomUtil_isWhitespaceOnly(_data.getData(), _data.getLength()))
    _nodeFlags |= DOM_NODE_FLAG_WHITESPACE_CONTENT;

  return ERR_OK;
}

err_t DomCharacterData::appendData(const StringW& arg)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;
  FOG_RETURN_ON_ERROR(_data.append(arg));

  if ((_nodeFlags & DOM_NODE_FLAG_WHITESPACE_CONTENT) != 0 &&
      DomUtil_isWhitespaceOnly(arg.getData(), arg.getLength()))
  {
    _nodeFlags &= ~DOM_NODE_FLAG_WHITESPACE_CONTENT;
  }

  return ERR_OK;
}

err_t DomCharacterData::insertData(size_t offset, const StringW& arg)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;
  FOG_RETURN_ON_ERROR(_data.insert(offset, arg));

  if ((_nodeFlags & DOM_NODE_FLAG_WHITESPACE_CONTENT) != 0 &&
      DomUtil_isWhitespaceOnly(arg.getData(), arg.getLength()))
  {
    _nodeFlags &= ~DOM_NODE_FLAG_WHITESPACE_CONTENT;
  }

  return ERR_OK;
}

err_t DomCharacterData::deleteData(size_t offset, size_t count)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  // Prevent overflow.
  if (count > SIZE_MAX - offset)
    count = SIZE_MAX - offset;

  return deleteData(Range(offset, offset + count));
}

err_t DomCharacterData::deleteData(const Range& range)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  // If the specified offset is negative or greater than the number of
  // 16-bit units in data
  size_t dataLength = _data.getLength();
  if (range.getStart() > dataLength)
    return ERR_DOM_INDEX_SIZE;

  FOG_RETURN_ON_ERROR(_data.remove(range));
  return ERR_OK;
}

err_t DomCharacterData::replaceData(size_t offset, size_t count, const StringW& arg)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  // Prevent overflow.
  if (count > SIZE_MAX - offset)
    count = SIZE_MAX - offset;

  return deleteData(Range(offset, offset + count));
}

err_t DomCharacterData::replaceData(const Range& range, const StringW& arg)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  // If the specified offset is negative or greater than the number of
  // 16-bit units in data
  size_t dataLength = _data.getLength();
  if (range.getStart() > dataLength)
    return ERR_DOM_INDEX_SIZE;
  FOG_RETURN_ON_ERROR(_data.replace(range, arg));

  if ((_nodeFlags & DOM_NODE_FLAG_WHITESPACE_CONTENT) != 0)
  {
    if (DomUtil_isWhitespaceOnly(arg.getData(), arg.getLength()))
      _nodeFlags &= ~DOM_NODE_FLAG_WHITESPACE_CONTENT;
  }
  else
  {
    if (DomUtil_isWhitespaceOnly(_data.getData(), _data.getLength()))
      _nodeFlags |= DOM_NODE_FLAG_WHITESPACE_CONTENT;
  }

  return ERR_OK;
}

StringW DomCharacterData::substringData(size_t offset, size_t count)
{
  // Prevent overflow.
  if (count > SIZE_MAX - offset)
    count = SIZE_MAX - offset;

  return substringData(Range(offset, offset + count));
}

StringW DomCharacterData::substringData(const Range& range)
{
  return _data.substring(range);
}

// ============================================================================
// [Fog::DomCharacterData - HasInvalidSequence]
// ============================================================================

bool DomCharacterData::hasInvalidSequence() const
{
  return false;
}

// ============================================================================
// [Fog::DomText - Construction / Destruction]
// ============================================================================

DomText::DomText(
  DomDocument* ownerDocument,
  const StringW& data)
  :
  DomCharacterData(ownerDocument, DOM_NODE_TYPE_TEXT, data)
{
}

DomText::~DomText()
{
}

// ============================================================================
// [Fog::DomText - DOM Interface]
// ============================================================================

bool DomText::isElementContentWhitespace() const
{
  return _nodeFlags & DOM_NODE_FLAG_WHITESPACE_CONTENT;
}

StringW DomText::getWholeText() const
{
  // TODO:
  return StringW();
}

DomNode* DomText::replaceWholeText(const StringW& content)
{
  // TODO:
  return NULL;
}

DomNode* DomText::splitText(size_t offset, err_t* errOut)
{
  err_t errLocal;

  if (errOut == NULL)
    errOut = &errLocal;

  if (isReadOnly())
  {
    *errOut = ERR_DOM_NO_MODIFICATION_ALLOWED;
    return NULL;
  }

  size_t dataLength = _data.getLength();
  if (offset > dataLength)
  {
    *errOut = ERR_DOM_INDEX_SIZE;
    return NULL;
  }

  StringW s(_data, Range(offset, dataLength));
  if (s.isEmpty() && offset != dataLength)
  {
    *errOut = ERR_RT_OUT_OF_MEMORY;
    return NULL;
  }

  DomDocument* doc = getOwnerDocument();
  DomText* node = doc->createTextNode(s);

  if (node == NULL)
  {
    *errOut = ERR_RT_OUT_OF_MEMORY;
    return NULL;
  }

  err_t err = _data.truncate(offset);
  if (FOG_IS_ERROR(err))
  {
    *errOut = err;
    return NULL;
  }

  _data.squeeze();

  *errOut = ERR_OK;
  return node;
}

// ============================================================================
// [Fog::DomCDATASection - Construction / Destruction]
// ============================================================================

DomCDATASection::DomCDATASection(
  DomDocument* ownerDocument,
  const StringW& data)
  :
  DomText(ownerDocument, data)
{
  _nodeType = DOM_NODE_TYPE_CDATA_SECTION;
}

DomCDATASection::~DomCDATASection()
{
}

// ============================================================================
// [Fog::DomCDATASection - HasInvalidSequence]
// ============================================================================

bool DomCDATASection::hasInvalidSequence() const
{
  static const char invalidData[] = { ']', ']', '>' };

  return _data.contains(Ascii8(invalidData, FOG_ARRAY_SIZE(invalidData)));
}

// ============================================================================
// [Fog::DomComment]
// ============================================================================

DomComment::DomComment(
  DomDocument* ownerDocument,
  const StringW& data)
  :
  DomCharacterData(ownerDocument, DOM_NODE_TYPE_COMMENT, data)
{
}

DomComment::~DomComment()
{
}

// ============================================================================
// [Fog::DomComment - HasInvalidSequence]
// ============================================================================

bool DomComment::hasInvalidSequence() const
{
  static const char invalidData[] = { '-', '-' };

  return _data.contains(Ascii8(invalidData, FOG_ARRAY_SIZE(invalidData)));
}

// ============================================================================
// [Fog::DomProcessingInstruction - Construction / Destruction]
// ============================================================================

DomProcessingInstruction::DomProcessingInstruction(
  DomDocument* ownerDocument,
  const InternedStringW& target,
  const StringW& data)
  :
  DomCharacterData(ownerDocument, DOM_NODE_TYPE_PROCESSING_INSTRUCTION, data),
  _target(target)
{
}

DomProcessingInstruction::~DomProcessingInstruction()
{
}

// ============================================================================
// [Fog::DomProcessingInstruction - HasInvalidSequence]
// ============================================================================

bool DomProcessingInstruction::hasInvalidSequence() const
{
  static const char invalidData[] = { '?', '>' };

  return _data.contains(Ascii8(invalidData, FOG_ARRAY_SIZE(invalidData)));
}

// ============================================================================
// [Fog::DomDocumentFragment]
// ============================================================================

DomDocumentFragment::DomDocumentFragment(
  DomDocument* ownerDocument)
  :
  DomContainer(ownerDocument, DOM_NODE_TYPE_DOCUMENT_FRAGMENT)
{
}

DomDocumentFragment::~DomDocumentFragment()
{
}

// ============================================================================
// [Fog::DomDocumentType]
// ============================================================================

DomDocumentType::DomDocumentType(DomDocument* ownerDocument,
  const InternedStringW& name,
  const StringW& publicId,
  const StringW& systemId,
  const StringW& internalSubset)
  :
  DomNode(ownerDocument, DOM_NODE_TYPE_DOCUMENT_TYPE),
  _name(name),
  _publicId(publicId),
  _systemId(systemId),
  _internalSubset(internalSubset)
{
}

DomDocumentType::~DomDocumentType()
{
}

// ============================================================================
// [Fog::DomDocumentIdHash - Construction / Destruction]
// ============================================================================

DomDocumentIdHash::DomDocumentIdHash() :
  _capacity(13),
  _length(0),
  _expandCapacity(47),
  _expandLength(13),
  _shrinkCapacity(0),
  _shrinkLength(0),
  _buckets(_bucketsBuffer)
{
  MemOps::zero(_bucketsBuffer, sizeof(_bucketsBuffer));
}

DomDocumentIdHash::~DomDocumentIdHash()
{
  if (_buckets != _bucketsBuffer)
    MemMgr::free(_buckets);
}

void DomDocumentIdHash::add(DomElement* element)
{
  uint32_t hashCode = element->_id._d->hashCode;
  uint32_t hashMod = hashCode % _capacity;

  DomElement** pPrev = &_buckets[hashMod];
  DomElement* cur = *pPrev;

  while (cur)
  {
    pPrev = &cur->_nextId;
    cur = *pPrev;
  }

  *pPrev = element;
  if (++_length >= _expandLength)
    _rehash(_expandCapacity);
}

void DomDocumentIdHash::remove(DomElement* element)
{
  uint32_t hashCode = element->_id._d->hashCode;
  uint32_t hashMod = hashCode % _capacity;

  DomElement** pPrev = &_buckets[hashMod];
  DomElement* cur = *pPrev;

  while (cur)
  {
    if (cur == element)
    {
      *pPrev = cur->_nextId;
      cur->_nextId = NULL;

      if (--_length <= _shrinkLength)
        _rehash(_shrinkCapacity);
      return;
    }

    pPrev = &cur->_nextId;
    cur = *pPrev;
  }
}

DomElement* DomDocumentIdHash::get(const StringW& id) const
{
  uint32_t hashCode = id.getHashCode();
  uint32_t hashMod = hashCode % _capacity;

  DomElement* cur = _buckets[hashMod];
  while (cur)
  {
    if (cur->_id == id)
      return cur;
    cur = cur->_nextId;
  }
  return NULL;
}

DomElement* DomDocumentIdHash::get(const StubW& id) const
{
  size_t idLength = id.getComputedLength();
  const CharW* idData = id.getData();

  uint32_t hashCode = HashUtil::hash(StubW(idData, idLength));
  uint32_t hashMod = hashCode % _capacity;

  DomElement* cur = _buckets[hashMod];
  while (cur)
  {
    const StringDataW* curId = cur->_id._d;
    if (curId->length == idLength && StringUtil::eq(curId->data, idData, idLength))
      return cur;
    cur = cur->_nextId;
  }
  return NULL;
}

void DomDocumentIdHash::_rehash(size_t capacity)
{
  DomElement** oldBuckets = _buckets;
  DomElement** newBuckets = (DomElement**)MemMgr::calloc(capacity * sizeof(DomElement*));

  if (newBuckets == NULL)
    return;

  size_t i, len = _capacity;
  for (i = 0; i < len; i++)
  {
    DomElement* cur = oldBuckets[i];
    while (cur)
    {
      uint32_t hashMod = cur->_id._d->hashCode % capacity;
      DomElement* next = cur->_nextId;

      DomElement** newPPrev = &newBuckets[hashMod];
      DomElement* newCur = *newPPrev;

      while (newCur)
      {
        newPPrev = &newCur->_nextId;
        newCur = *newPPrev;
      }

      *newPPrev = cur;
      cur->_nextId = NULL;

      cur = next;
    }
  }

  _capacity = capacity;

  _expandCapacity = fog_api.hashhelper_calcExpandCapacity(capacity);
  _expandLength = (size_t)((ssize_t)_capacity * 0.92);

  _shrinkCapacity = fog_api.hashhelper_calcShrinkCapacity(capacity);
  _shrinkLength = (size_t)((ssize_t)_shrinkCapacity * 0.70);

  atomicPtrXchg(&_buckets, newBuckets);

  if (oldBuckets != _bucketsBuffer)
    MemMgr::free(oldBuckets);
}

// ============================================================================
// [Fog::DomDocument - Construction / Destruction]
// ============================================================================

struct DomDocumentGCFuncs
{
  static void FOG_CDECL destroyObject(void* p)
  {
    static_cast<DomObj*>(p)->~DomObj();
  }

  static size_t FOG_CDECL getObjectSize(void* p)
  {
    ObjInfo info;
    static_cast<DomObj*>(p)->getObjInfo(&info);
    return info.getObjectSize();
  }

  static bool FOG_CDECL isObjectUsed(void* p)
  {
    return static_cast<DomNode*>(p)->_parentNode != NULL;
  }

  static bool FOG_CDECL getObjectMark(void* p)
  {
    return (static_cast<DomNode*>(p)->_nodeFlags & DOM_NODE_FLAG_IN_GC_QUEUE) != 0;
  }

  static void FOG_CDECL setObjectMark(void* p, bool mark)
  {
    if (mark)
      static_cast<DomNode*>(p)->_nodeFlags |= DOM_NODE_FLAG_IN_GC_QUEUE;
    else
      static_cast<DomNode*>(p)->_nodeFlags &= ~DOM_NODE_FLAG_IN_GC_QUEUE;
  }
};

static const MemGCFuncs DomDocument_gcFuncs =
{
  DomDocumentGCFuncs::destroyObject,
  DomDocumentGCFuncs::getObjectSize,
  DomDocumentGCFuncs::isObjectUsed,
  DomDocumentGCFuncs::getObjectMark,
  DomDocumentGCFuncs::setObjectMark
};

DomDocument::DomDocument() :
  DomContainer(NULL, DOM_NODE_TYPE_DOCUMENT),
  _gc(&DomDocument_gcFuncs),
  _documentType(NULL),
  _documentElement(NULL),
  _xmlVersion(static_cast<const StringW&>(FOG_S(1_0))),
  _xmlEncoding(static_cast<const StringW&>(FOG_S(UTF_8))),
  _xmlStandalone(false)
{
}

DomDocument::~DomDocument()
{
  // Dom is a container, we need to remove everything from it and then we can
  // run GC to collect everything.
  if ((_nodeFlags & DOM_NODE_FLAG_HAS_CHILD_NODES) != 0)
    DomContainer_removeChildNodes(this);

  _gc.collect();
}

// ============================================================================
// [Fog::DomDocument - AddRef / Release]
// ============================================================================

void DomDocument::_release()
{
  FOG_DOM_RELEASE(DomDocument);
  fog_delete(this);
}

// ============================================================================
// [Fog::DomDocument - Children]
// ============================================================================

DomDocumentType* DomDocument::getDocumentType() const
{
  return _documentType;
}

DomElement* DomDocument::getDocumentElement() const
{
  return _documentElement;
}

// ============================================================================
// [Fog::DomDocument - Create - Public]
// ============================================================================

DomCDATASection* DomDocument::createCDATASection(const StringW& data)
{
  DomCDATASection* node = _createCDATASection(data);
  if (node != NULL)
    _gc.mark(node);
  return node;
}

DomComment* DomDocument::createComment(const StringW& data)
{
  DomComment* node = _createComment(data);
  if (node != NULL)
    _gc.mark(node);
  return node;
}

DomElement* DomDocument::createElement(const StringW& tagName)
{
  InternedStringW tagNameInterned(tagName);

  DomElement* node = _createElement(tagNameInterned);
  if (node != NULL)
    _gc.mark(node);
  return node;
}

DomProcessingInstruction* DomDocument::createProcessingInstruction(const StringW& target, const StringW& data)
{
  InternedStringW targetInterned(target);
  
  DomProcessingInstruction* node = _createProcessingInstruction(targetInterned, data);
  if (node != NULL)
    _gc.mark(node);
  return node;
}

DomText* DomDocument::createTextNode(const StringW& data)
{
  DomText* node = _createTextNode(data);
  if (node != NULL)
    _gc.mark(node);
  return node;
}

// ============================================================================
// [Fog::DomDocument - Create - Private]
// ============================================================================

DomDocument* DomDocument::_createDocument()
{
  return fog_new DomDocument();
}

DomCDATASection* DomDocument::_createCDATASection(
  const StringW& data)
{
  void* p = _gc.alloc(sizeof(DomCDATASection));
  if (FOG_IS_NULL(p))
    return NULL;
  return fog_new_p(p) DomCDATASection(this, data);
}

DomComment* DomDocument::_createComment(
  const StringW& data)
{
  void* p = _gc.alloc(sizeof(DomComment));
  if (FOG_IS_NULL(p))
    return NULL;
  return fog_new_p(p) DomComment(this, data);
}

DomElement* DomDocument::_createElement(
  const InternedStringW& tagName)
{
  void* p = _gc.alloc(sizeof(DomElement));
  if (FOG_IS_NULL(p))
    return NULL;
  return fog_new_p(p) DomElement(this, tagName);
}

DomProcessingInstruction* DomDocument::_createProcessingInstruction(
  const InternedStringW& target,
  const StringW& data)
{
  void* p = _gc.alloc(sizeof(DomProcessingInstruction));
  if (FOG_IS_NULL(p))
    return NULL;
  return fog_new_p(p) DomProcessingInstruction(this, target, data);
}

DomText* DomDocument::_createTextNode(
  const StringW& data)
{
  void* p = _gc.alloc(sizeof(DomText));
  if (FOG_IS_NULL(p))
    return NULL;
  return fog_new_p(p) DomText(this, data);
}

// ============================================================================
// [Fog::DomDocument - CloneDocument]
// ============================================================================

DomDocument* DomDocument::cloneDocument() const
{
  DomDocument* doc = const_cast<DomDocument*>(this)->_createDocument();
  if (FOG_IS_NULL(doc))
    return NULL;

  DomNode* node;
  for (node = getFirstChild(); node != NULL; node = node->getNextSibling())
  {
    DomNode* copy = doc->importNode(node, true);
    if (copy == NULL)
      goto _Fail;

    if (doc->appendChild(copy) != Fog::ERR_OK)
    {
      fog_delete(copy);
      goto _Fail;
    }
  }

_Fail:
  fog_delete(doc);
  return NULL;
}

// ============================================================================
// [Fog::DomDocument - GetElementById]
// ============================================================================

DomElement* DomDocument::getElementById(const StringW& id) const
{
  DomElement* element = _idHash.get(id);

  // Not found.
  if (element == NULL)
    return NULL;

  // Only return elements inside the document.
  if (element->_parentNode != NULL)
    return element;

  // We have to iterate over all chained elements in such case.
  for (;;)
  {
    element = element->_nextId;
    if (element == NULL)
      return NULL;
    if (element->_id == id && element->_parentNode != NULL)
      return element;
  }
}

DomElement* DomDocument::getElementById(const StubW& id) const
{
  const CharW* idData = id.getData();
  size_t idLength = id.getComputedLength();

  DomElement* element = _idHash.get(StubW(idData, idLength));

  // Not found.
  if (element == NULL)
    return element;

  // Only return elements inside the document.
  if (element->_parentNode != NULL)
    return element;

  // We have to iterate over all chained elements in such case.
  for (;;)
  {
    element = element->_nextId;
    if (element == NULL)
      return NULL;
    if (element->_id.eq(StubW(idData, idLength)) && element->_parentNode != NULL)
      return element;
  }
}

// ============================================================================
// [Fog::DomDocument - GetElementsByTagName]
// ============================================================================

DomNodeList* DomDocument::getElementsByTagName(const StringW& tagName) const
{
  // TODO:
  return NULL;
}

DomNodeList* DomDocument::getElementsByTagName(const StubW& tagName) const
{
  // TODO:
  return NULL;
}

// ============================================================================
// [Fog::DomDocument - ImportNode / AdoptNode]
// ============================================================================

DomNode* DomDocument::importNode(DomNode* importedNode, bool deep, err_t* errOut)
{
  // TODO:
  return NULL;
}

// ============================================================================
// [Fog::DomDocument - XML]
// ============================================================================

err_t DomDocument::setDocumentURI(const StringW& documentURI)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  FOG_RETURN_ON_ERROR(_documentURI.set(documentURI));
  return ERR_OK;
}

err_t DomDocument::setXmlEncoding(const StringW& xmlEncoding)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  FOG_RETURN_ON_ERROR(_xmlEncoding.set(xmlEncoding));
  return ERR_OK;
}

err_t DomDocument::setXmlStandalone(bool xmlStandalone)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  _xmlStandalone = xmlStandalone;
  return ERR_OK;
}

err_t DomDocument::setXmlVersion(const StringW& xmlVersion)
{
  if (isReadOnly())
    return ERR_DOM_NO_MODIFICATION_ALLOWED;

  FOG_RETURN_ON_ERROR(_xmlVersion.set(xmlVersion));
  return ERR_OK;
}

// ============================================================================
// [Fog::DomDocument - Read / Write]
// ============================================================================

err_t DomDocument::readFromFile(const StringW& fileName)
{
  DomSaxHandler domHandler(this);
  XmlSaxParser parser(&domHandler);

  return parser.parseFile(fileName);
}

err_t DomDocument::readFromStream(Stream& stream)
{
  DomSaxHandler domHandler(this);
  XmlSaxParser parser(&domHandler);

  return parser.parseStream(stream);
}

err_t DomDocument::readFromMemory(const void* mem, size_t size)
{
  DomSaxHandler domHandler(this);
  XmlSaxParser parser(&domHandler);

  return parser.parseMemory(mem, size);
}

err_t DomDocument::readFromString(const StringW& str)
{
  DomSaxHandler domHandler(this);
  XmlSaxParser parser(&domHandler);

  return parser.parseString(str);
}

err_t DomDocument::readFromString(const StubW& str)
{
  DomSaxHandler domHandler(this);
  XmlSaxParser parser(&domHandler);

  return parser.parseString(str);
}

// ============================================================================
// [Fog::DomDocument - GC]
// ============================================================================

void DomDocument::collect()
{
  _gc.collect();
}

// ============================================================================
// [Fog::DomSaxHandler - Construction / Destruction]
// ============================================================================

DomSaxHandler::DomSaxHandler(DomDocument* document) :
  _document(document),
  _currentContainer(NULL)
{
}
  
DomSaxHandler::~DomSaxHandler()
{
}

// ============================================================================
// [Fog::DomSaxHandler - SAX Interface]
// ============================================================================

err_t DomSaxHandler::onStartDocument()
{
  _currentContainer = _document;
  return ERR_OK;
}

err_t DomSaxHandler::onEndDocument()
{
  _currentContainer = NULL;
  return ERR_OK;
}

err_t DomSaxHandler::onStartElement(const StubW& tagName)
{
  if (FOG_IS_NULL(_currentContainer))
    return ERR_RT_INVALID_STATE;

  if (_currentContainer == _document && _document->getDocumentElement() != NULL)
    return ERR_XML_SAX_INTERNAL;

  InternedStringW tagNameInterned(tagName);
  DomElement* obj = _document->createElement(tagNameInterned);

  if (FOG_IS_NULL(obj))
    return ERR_RT_OUT_OF_MEMORY;

  err_t err = _currentContainer->appendChild(obj);
  if (FOG_IS_ERROR(err))
    return err;

  _currentContainer = obj;
  return ERR_OK;
}

err_t DomSaxHandler::onEndElement(const StubW& tagName)
{
  if (FOG_IS_NULL(_currentContainer) || _currentContainer == _document)
    return ERR_RT_INVALID_STATE;

  _currentContainer = _currentContainer->_parentNode;
  return ERR_OK;
}

err_t DomSaxHandler::onAttribute(const StubW& name, const StubW& value)
{
  if (FOG_IS_NULL(_currentContainer))
    return ERR_RT_INVALID_STATE;

  InternedStringW nameInterned(name);
  StringW valueString(value);

  // In called on DomDocument then the attribute is related to <?xml ... ?> content.
  if (_currentContainer == _document)
  {
    if (nameInterned == FOG_S(version))
      return _document->setXmlVersion(valueString);
    
    if (nameInterned == FOG_S(encoding))
      return _document->setXmlEncoding(valueString);

    if (nameInterned == FOG_S(standalone))
    {
      bool standalone = false;
      if (valueString.parseBool(&standalone) == ERR_OK)
        return _document->setXmlStandalone(standalone);
    }

    return ERR_OK;
  }
  else
  {
    return static_cast<DomElement*>(_currentContainer)->setAttribute(
      nameInterned, valueString);
  }
}

err_t DomSaxHandler::onCharacterData(const StubW& data)
{
  if (FOG_IS_NULL(_currentContainer))
    return ERR_RT_INVALID_STATE;

  StringW dataString(data);
  DomText* obj = _document->createTextNode(dataString);

  if (FOG_IS_NULL(obj))
    return ERR_RT_OUT_OF_MEMORY;

  return _currentContainer->appendChild(obj);
}

err_t DomSaxHandler::onIgnorableWhitespace(const StubW& data)
{
  // TODO:
  return ERR_OK;
}

err_t DomSaxHandler::onDOCTYPE(const List<StringW>& doctype)
{
  // TODO:
  return ERR_OK;
}

err_t DomSaxHandler::onCDATASection(const StubW& data)
{
  if (FOG_IS_NULL(_currentContainer) || _currentContainer == _document)
    return ERR_RT_INVALID_STATE;

  StringW dataString(data);
  DomCDATASection* obj = _document->createCDATASection(dataString);

  if (FOG_IS_NULL(obj))
    return ERR_RT_OUT_OF_MEMORY;

  return _currentContainer->appendChild(obj);
}

err_t DomSaxHandler::onComment(const StubW& data)
{
  if (FOG_IS_NULL(_currentContainer))
    return ERR_RT_INVALID_STATE;

  StringW dataString(data);
  DomComment* obj = _document->createComment(dataString);

  if (FOG_IS_NULL(obj))
    return ERR_RT_OUT_OF_MEMORY;

  return _currentContainer->appendChild(obj);
}

err_t DomSaxHandler::onProcessingInstruction(const StubW& target, const StubW& data)
{
  if (FOG_IS_NULL(_currentContainer))
    return ERR_RT_INVALID_STATE;

  InternedStringW targetInterned(target);
  StringW dataString(data);

  DomProcessingInstruction* obj = 
    _document->createProcessingInstruction(targetInterned, dataString);
  
  if (FOG_IS_NULL(obj))
    return ERR_RT_OUT_OF_MEMORY;

  return _currentContainer->appendChild(obj);
}

err_t DomSaxHandler::onError(const XmlSaxLocation& location, err_t errorCode)
{
  // Not a fatal error, we are not going to stop parsing at this time.
  return ERR_OK;
}

err_t DomSaxHandler::onFatal(const XmlSaxLocation& location, err_t errorCode)
{
  // Returning error from DOM handler means to stop parsing immediately, this
  // is the default for all fatal errors. Override if you need to extend or
  // completely bypass this behavior.
  return errorCode;
}

} // Fog namespace
