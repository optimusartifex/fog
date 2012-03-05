// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Text/TextDocument.h>

namespace Fog {

// ============================================================================
// [Fog::TextObj - Construction / Destruction]
// ============================================================================

TextObj::TextObj()
{
}

TextObj::~TextObj()
{
}

// ============================================================================
// [Fog::TextNode - Construction / Destruction]
// ============================================================================

TextNode::TextNode(TextDocument* ownerDocument, uint32_t type) :
  _ownerDocument(ownerDocument),
  _parent(NULL),
  _type(static_cast<uint16_t>(type)),
  _flags(TEXT_FLAG_DIRTY)
{
}

TextNode::~TextNode()
{
}

// ============================================================================
// [Fog::TextNode - Invalidate]
// ============================================================================

void TextNode::_invalidate()
{
  _flags |= TEXT_FLAG_DIRTY;

  TextNode* obj = _parent;
  while (obj != NULL && !obj->isDirtyChildren())
  {
    obj->_flags |= TEXT_FLAG_DIRTY_CHILDREN;
    obj = obj->_parent;
  }
}

// ============================================================================
// [Fog::TextNode - Interface]
// ============================================================================

// ============================================================================
// [Fog::TextContainer - Construction / Destruction]
// ============================================================================

TextContainer::TextContainer(TextDocument* ownerDocument, uint32_t type) :
  TextNode(ownerDocument, type)
{
}

TextContainer::~TextContainer()
{
}

// ============================================================================
// [Fog::TextParagraph - Construction / Destruction]
// ============================================================================

TextParagraph::TextParagraph(TextDocument* ownerDocument) :
  TextContainer(ownerDocument, TEXT_OBJECT_PARAGRAPH)
{
}

TextParagraph::~TextParagraph()
{
}

// ============================================================================
// [Fog::TextSpan - Construction / Destruction]
// ============================================================================

TextSpan::TextSpan(TextDocument* ownerDocument, const StringW& text) :
  TextNode(ownerDocument, TEXT_OBJECT_SPAN),
  _text(text)
{
}

TextSpan::~TextSpan()
{
}

// ============================================================================
// [Fog::TextSpan - Interface]
// ============================================================================

err_t TextSpan::setText(const StringW& text)
{
  FOG_RETURN_ON_ERROR(_text.set(text));
  return ERR_OK;
}

err_t TextSpan::resetText()
{
  return setText(StringW::getEmptyInstance());
}

// ============================================================================
// [Fog::TextDocument]
// ============================================================================

TextDocument::TextDocument() :
  TextContainer(NULL, TEXT_OBJECT_DOCUMENT),
  _allocator(2000)
{
}

TextDocument::~TextDocument()
{
}

} // Fog namespace
