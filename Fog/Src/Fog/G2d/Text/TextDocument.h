// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_TEXTDOCUMENT_H
#define _FOG_G2D_TEXT_TEXTDOCUMENT_H

// [Dependencies]
#include <Fog/Core/Memory/MemZoneAllocator.h>
#include <Fog/Core/Kernel/CoreObj.h>
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/TextLayout.h>
#include <Fog/G2d/Text/TextRect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::TextObj]
// ============================================================================

//! @brief Base instance for text object inside @ref TextLayout.
struct FOG_API TextObj : public CoreObj
{
  FOG_CORE_OBJ(TextObj, CoreObj)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextObj();
  virtual ~TextObj();

private:
  FOG_NO_COPY(TextObj)  
};

//! @brief Macro used to declare a new DOM object.
#define FOG_TEXT_OBJ(_Self_, _Base_) \
  FOG_CORE_OBJ(_Self_, _Base_)

// ============================================================================
// [Fog::TextNode]
// ============================================================================

//! @brief Base instance for text object inside @ref TextLayout.
struct FOG_API TextNode : public TextObj
{
  FOG_CORE_OBJ(TextNode, TextObj)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextNode(TextDocument* ownerDocument, uint32_t type);
  virtual ~TextNode();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE TextDocument* getOwnerDocument() const { return _ownerDocument; }
  FOG_INLINE TextObj* getParent() const { return _parent; }

  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE uint32_t getFlags() const { return _flags; }

  FOG_INLINE bool isSpan() const { return _type == TEXT_OBJECT_SPAN; }
  FOG_INLINE bool isDocument() const { return _type == TEXT_OBJECT_DOCUMENT; }
  FOG_INLINE bool isParagraph() const { return _type == TEXT_OBJECT_PARAGRAPH; }

  FOG_INLINE bool isDirty() const { return (_flags & TEXT_FLAG_DIRTY) != 0; }
  FOG_INLINE bool isDirtyChildren() const { return (_flags & TEXT_FLAG_DIRTY_CHILDREN) != 0; }

  // --------------------------------------------------------------------------
  // [Invalidate]
  // --------------------------------------------------------------------------

  FOG_INLINE void invalidate()
  {
    if (!isDirty())
      _invalidate();
  }

  void _invalidate();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Owner document.
  TextDocument* _ownerDocument;
  //! @brief Parent.
  TextNode* _parent;

  //! @brief Type, see @ref TEXT_OBJECT.
  uint16_t _type;
  //! @brief Flags.
  uint16_t _flags;
#if FOG_ARCH_BITS >= 64
  //! @brief Additional data, used to align to 32-bytes in 64-bit mode.
  uint32_t _data;
#endif // FOG_ARCH_BITS >= 64

private:
  FOG_NO_COPY(TextNode)
};

// ============================================================================
// [Fog::TextContainer]
// ============================================================================

struct FOG_API TextContainer : public TextNode
{
  FOG_TEXT_OBJ(TextContainer, TextNode)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextContainer(TextDocument* ownerDocument, uint32_t type);
  virtual ~TextContainer();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  FOG_NO_COPY(TextContainer)
};

// ============================================================================
// [Fog::TextParagraph]
// ============================================================================

struct FOG_API TextParagraph : public TextContainer
{
  FOG_TEXT_OBJ(TextParagraph, TextContainer)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextParagraph(TextDocument* ownerDocument);
  virtual ~TextParagraph();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  FOG_NO_COPY(TextParagraph)
};

// ============================================================================
// [Fog::TextSpan]
// ============================================================================

struct FOG_API TextSpan : public TextNode
{
  FOG_TEXT_OBJ(TextSpan, TextNode)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextSpan(TextDocument* ownerDocument, const StringW& text);
  virtual ~TextSpan();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getText() const { return _text; }
  virtual err_t setText(const StringW& text);
  virtual err_t resetText();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Text.
  StringW _text;

private:
  FOG_NO_COPY(TextSpan)
};

// ============================================================================
// [Fog::TextDocument]
// ============================================================================

struct FOG_API TextDocument : public TextContainer
{
  FOG_TEXT_OBJ(TextDocument, TextContainer)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextDocument();
  virtual ~TextDocument();

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Allocator used to alloc text objects.
  MemZoneAllocator _allocator;

private:
  FOG_NO_COPY(TextDocument)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_TEXTDOCUMENT_H
