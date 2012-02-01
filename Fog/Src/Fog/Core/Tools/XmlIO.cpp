// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Core/Tools/XmlEntity_p.h>
#include <Fog/Core/Tools/XmlIO.h>

namespace Fog {

// ============================================================================
// [Fog::XmlSaxHandler - Construction / Destruction]
// ============================================================================

XmlSaxHandler::XmlSaxHandler() {}
XmlSaxHandler::~XmlSaxHandler() {}

// ============================================================================
// [Fog::XmlSaxParser - Constants]
// ============================================================================

//! @internal.
//!
//! @brief @ref XmlSaxParser state.
enum XML_SAX_STATE
{
  //! @brief Parser is ready to parse (no errors, not inside tag declaration).
  XML_SAX_STATE_READY = 0,
  //! @brief Parser is in tag < state.
  XML_SAX_STATE_TAG_BEGIN,
  //! @brief Parser is in tag <[StartNameCharacter] state.
  XML_SAX_STATE_TAG_NAME,
  //! @brief Parser is inside tag after name declaration.
  XML_SAX_STATE_TAG_INSIDE,
  //! @brief Parser is inside attribute name declaration.
  XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_NAME,
  //! @brief Parser is inside attribute value declaration.
  XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_VALUE,
  //! @brief Parser is inside tag and it's waiting for >.
  XML_SAX_STATE_TAG_END,
  //! @brief Parser is in tag </ state.
  XML_SAX_STATE_TAG_CLOSE,
  //! @brief Parser is in tag </[StartNameCharacter] state.
  XML_SAX_STATE_TAG_CLOSE_NAME,
  //! @brief Parser is in tag </[StartNameCharacter][NameCharacter]*. state.
  XML_SAX_STATE_TAG_CLOSE_END,
  //! @brief Parser is in <? declaration.
  XML_SAX_STATE_TAG_QUESTION_MARK,
  //! @brief Parser is in <! declaration.
  XML_SAX_STATE_TAG_EXCLAMATION_MARK,
  //! @brief Parser is in <!DOCTYPE declaration.
  XML_SAX_STATE_DOCTYPE,
  //! @brief Parser is in <!DOCTYPE text
  XML_SAX_STATE_DOCTYPE_TEXT,
  //! @brief Parser is in <!DOCTYPE attribute.
  XML_SAX_STATE_DOCTYPE_ATTRIBUTE,
  //! @brief Parser is in <? ?> or <% %> sequence.
  XML_SAX_STATE_PROCESSING_INSTRUCTION,
  //! @brief Parser is in <!-- --> sequence.
  XML_SAX_STATE_COMMENT,
  //! @brief Parser is in <![[CDATA ]]> sequence.
  XML_SAX_STATE_CDATA
};

//! @internal.
//!
//! @brief @ref XmlSaxParser element.
enum XML_SAX_ELEMENT
{
  //! @brief Element is standard tag element.
  XML_SAX_ELEMENT_TAG = 0,
  //! @brief Self-closing element.
  XML_SAX_ELEMENT_TAG_SELF_CLOSING,
  //! @brief Element is XML declaration.
  XML_SAX_ELEMENT_XML,
  //! @brief Element is CDATA declaration.
  XML_SAX_ELEMENT_CDATA,
  //! @brief Element is DOCTYPE declaration.
  XML_SAX_ELEMENT_DOCTYPE
};

// ============================================================================
// [Fog::XmlSaxParser - Helpers]
// ============================================================================

//! @internal
//!
//! @brief Convert 8-bit ASCII uppercase/lowercase to lowercase, using fast AND
//! operation (no branch).
static FOG_INLINE uint8_t XmlUtil_lo8(uint8_t c)
{
  return c & 0xDF;
}

// NameStartChar ::= 
//   ":" | [A-Z] | 
//   "_" | [a-z] | 
//   [U+0000C0-U+0000D6] |
//   [U+0000D8-U+0000F6] |
//   [U+0000F8-U+0002FF] |
//   [U+000370-U+00037D] |
//   [U+00037F-U+001FFF] | 
//   [U+00200C-U+00200D] | 
//   [U+002070-U+00218F] | 
//   [U+002C00-U+002FEF] | 
//   [U+003001-U+00D7FF] | 
//   [U+00F900-U+00FDCF] | 
//   [U+00FDF0-U+00FFFD] |
//   [U+010000-U+0EFFFF]
static FOG_INLINE bool XmlUtil_isNameStartChar(const CharW& c)
{
  return c.isLetter() ||
         c == CharW(':') ||
         c == CharW('_') ;
}

// NameChar ::=
//   NameStartChar | 
//   "-" | 
//   "." | 
//   [0-9]           | 
//   [U+00B7]        | 
//   [U+0300-U+036F] | 
//   [U+203F-U+2040]
static FOG_INLINE bool XmlUtil_isNameChar(const CharW& c)
{
  return c.isNumlet() ||
         c == CharW(':') ||
         c == CharW('_') ||
         c == CharW('-') ||
         c == CharW('.') ;
}

// TODO: Not correct.
static FOG_INLINE bool XmlUtil_isWhitespace(const CharW& c)
{
  return c.isSpace();
}

// TODO: Probably wrong according to the XML specification.
static bool XmlSaxParser_isWhiteSpace(const CharW* buffer, const CharW* end)
{
  while (buffer < end)
  {
    if (!buffer->isSpace()) return false;
    buffer++;
  }
  return true;
}

static void XmlSaxParser_detectEncoding(TextCodec& tc, const void* mem, size_t size)
{
  // first check for BOM
  if (tc.createFromBom(mem, size) == ERR_OK) return;

  const uint8_t* p = reinterpret_cast<const uint8_t*>(mem);
  const uint8_t* pEnd = p + size;

  if (size < 15)
    return;

  bool parsedUtf8Bom;

  while (p != pEnd)
  {
    // ------------------------------------------------------------------------
    // [Detect BOM]
    // ------------------------------------------------------------------------

    if (p < (pEnd - 2) &&
      p[0] == 0xFF &&
      p[1] == 0xFE)
    {
      tc.createFromCode(TEXT_ENCODING_UTF16_LE);
      return;
    }

    else if (p < (pEnd - 2) &&
      p[0] == 0xFE &&
      p[1] == 0xFF)
    {
      tc.createFromCode(TEXT_ENCODING_UTF16_BE);
      return;
    }

    else if (
      p < (pEnd - 3) &&
      p[0] == 0xEF &&
      p[1] == 0xBB &&
      p[2] == 0xBF)
    {
      parsedUtf8Bom = true;
      p += 3;
      continue;
    }

    else if (p < (pEnd - 4) &&
      p[0] == 0xFF &&
      p[1] == 0xFE &&
      p[2] == 0x00 &&
      p[3] == 0x00)
    {
      tc.createFromCode(TEXT_ENCODING_UTF32_LE);
      return;
    }

    else if (p < (pEnd - 4) &&
      p[0] == 0x00 &&
      p[1] == 0x00 &&
      p[2] == 0xFE &&
      p[3] == 0xFF)
    {
      tc.createFromCode(TEXT_ENCODING_UTF32_BE);
      return;
    }

    // ------------------------------------------------------------------------
    // [Detect 8-Bit Encoding]
    // ------------------------------------------------------------------------

    else if (
      p < (pEnd - 5) &&
      p[0]              == '<' &&
      p[1]              == '?' &&
      XmlUtil_lo8(p[2]) == 'x' &&
      XmlUtil_lo8(p[3]) == 'm' &&
      XmlUtil_lo8(p[4]) == 'l')
    {
      // Xml header, we are in "<?xml".
      p += 5;

      while (p + 9 < pEnd)
      {
        if (*p == '>')
          return;

        if (CharA::isSpace(*p) && StringUtil::eq(
          reinterpret_cast<const char*>(p) + 1, "encoding", 8, CASE_INSENSITIVE))
        {
          // We are in "<?xml ..... encoding".
          const uint8_t* begin;
          char q;
          p += 9;

          // Find '='.
          while (p != pEnd && *p != '=') p++;
          if (p == pEnd)
            return;

          p++;

          // We are in "<?xml ..... encoding = "
          while (p != pEnd && CharA::isSpace(*p)) p++;
          if (p == pEnd)
            return;

          q = *p++;
          begin = p;

          while (p != pEnd && *p != q) p++;
          if (p == pEnd)
            return;

          // Try encoding and return.
          tc.createFromMime(Ascii8(begin, (size_t)(p - begin)));
          return;
        }
        p++;
      }
    }

    // ------------------------------------------------------------------------
    // [Detect UTF-16]
    // ------------------------------------------------------------------------

    else if (
      p < (pEnd - 10) &&
      p[0]              == '<' && p[1] == '\0' &&
      p[2]              == '?' && p[3] == '\0' &&
      XmlUtil_lo8(p[4]) == 'x' && p[5] == '\0' &&
      XmlUtil_lo8(p[6]) == 'm' && p[7] == '\0' &&
      XmlUtil_lo8(p[8]) == 'l' && p[9] == '\0')
    {
      tc.createFromCode(TEXT_ENCODING_UTF16_LE);
      return;
    }

    else if (
      p < (pEnd - 10) &&
      p[0] == '\0' && p[1]              == '<' &&
      p[2] == '\0' && p[3]              == '?' &&
      p[4] == '\0' && XmlUtil_lo8(p[5]) == 'x' &&
      p[6] == '\0' && XmlUtil_lo8(p[7]) == 'm' &&
      p[8] == '\0' && XmlUtil_lo8(p[9]) == 'l')
    {
      tc.createFromCode(TEXT_ENCODING_UTF16_LE);
      return;
    }

    // ------------------------------------------------------------------------
    // [...]
    // ------------------------------------------------------------------------

    else if (p[0] == '<')
    {
      // XML header not found, default encoding is UTF-8
      break;
    }

    p++;
  }
}

// ============================================================================
// [Fog::XmlSaxParser - Construction / Destruction]
// ============================================================================

XmlSaxParser::XmlSaxParser(XmlSaxHandler* handler) :
  _handler(handler)
{
}

XmlSaxParser::~XmlSaxParser()
{
}

// ============================================================================
// [Fog::XmlSaxParser - Parse]
// ============================================================================

err_t XmlSaxParser::parseFile(const StringW& fileName)
{
  Stream stream;
  err_t err = stream.openFile(fileName, STREAM_OPEN_READ);

  if (FOG_IS_ERROR(err)) 
    return err;

  return parseStream(stream);
}

err_t XmlSaxParser::parseStream(Stream& stream)
{
  StringA buffer;
  stream.readAll(buffer);
  return parseMemory(reinterpret_cast<const void*>(buffer.getData()), buffer.getLength());
}

err_t XmlSaxParser::parseMemory(const void* mem, size_t size)
{
  TextCodec textCodec = TextCodec::utf8();
  XmlSaxParser_detectEncoding(textCodec, mem, size);

  StringW buffer;
  err_t err = textCodec.decode(buffer, StubA(reinterpret_cast<const char*>(mem), size));

  if (FOG_IS_ERROR(err))
    return err;

  return parseString(StubW(buffer.getData(), buffer.getLength()));
}

err_t XmlSaxParser::parseString(const StringW& str)
{
  StringW copy(str);
  return parseString(StubW(copy.getData(), copy.getLength()));
}

err_t XmlSaxParser::parseString(const StubW& str)
{
  const CharW* sData = str.getData();
  size_t sLength = str.getComputedLength();

  // Check if encoded length is zero (no document).
  if (sLength == DETECT_LENGTH)
    sLength = StringUtil::len(sData);

  if (sLength == 0)
    return ERR_XML_SAX_NO_DOCUMENT;

  const CharW* sPtr = sData;           // Current pointer.
  const CharW* sEnd = sData + sLength; // End of buffer.

  const CharW* mark = sData;           // Mark to start position of currently parsed item.

  const CharW* markTagStart  = NULL;   // Mark to start position of currently parsed tag name.
  const CharW* markTagEnd    = NULL;   // Mark to end position of currently parsed tag name.

  const CharW* markAttrStart = NULL;   // Mark to start of attribute.
  const CharW* markAttrEnd   = NULL;   // Mark to end of attribute.

  const CharW* markDataStart = NULL;   // Mark to start of data (CDATA, Comment, attribute text, ...).
  const CharW* markDataEnd   = NULL;   // Mark to end of data (CDATA, Comment, attribute text, ...).

  CharW ch;                            // Current character.
  CharW attr;                          // Attribute marker (' or ").

  err_t err = ERR_OK;                  // Current error code.
  uint state = XML_SAX_STATE_READY;    // Current state.
  uint element = XML_SAX_ELEMENT_TAG;  // Element type.
  uint depth = 0;                      // Current depth.

  List<StringW> doctype;

  // TODO: Not used at this time.
  XmlSaxLocation location(0, 0);

  _handler->onStartDocument();
  for (;;)
  {
_Begin:
    if (sPtr == sEnd) break;
    ch = *sPtr;

_Continue:
    switch (state)
    {
      case XML_SAX_STATE_READY:
      {
        // If xml char has special meaning, we will process it, otherwise go away.
        if (ch == CharW('<'))
        {
          // If there is text, we will call addText().
          if (mark != sPtr)
          {
            bool isWhiteSpace = XmlSaxParser_isWhiteSpace(mark, sPtr);

            if (isWhiteSpace)
              err = _handler->onIgnorableWhitespace(StubW(mark, (size_t)(sPtr - mark)));
            else
              err = _handler->onCharacterData(StubW(mark, (size_t)(sPtr - mark)));

            if (FOG_IS_ERROR(err))
              goto _End;
          }

          state = XML_SAX_STATE_TAG_BEGIN;
          mark = sPtr;
        }
        break;
      }

      case XML_SAX_STATE_TAG_BEGIN:
      {
        // Match start tag name (this is probably the most common).
        if (XmlUtil_isNameStartChar(ch))
        {
          state = XML_SAX_STATE_TAG_NAME;
          markTagStart = sPtr;
          break;
        }

        // Match closing tag slash.
        if (ch == CharW('/'))
        {
          state = XML_SAX_STATE_TAG_CLOSE;
          break;
        }

        if (ch == CharW('?'))
        {
          state = XML_SAX_STATE_TAG_QUESTION_MARK;
          break;
        }

        if (ch == CharW('!'))
        {
          state = XML_SAX_STATE_TAG_EXCLAMATION_MARK;
          break;
        }

        if (XmlUtil_isWhitespace(ch))
          break;

        // Syntax Error
        err = ERR_XML_SAX_SYNTAX;
        goto _End;
      }

      case XML_SAX_STATE_TAG_NAME:
      {
        if (XmlUtil_isNameChar(ch))
          break;

        markTagEnd = sPtr;

        state = XML_SAX_STATE_TAG_INSIDE;
        depth++;
        element = XML_SAX_ELEMENT_TAG;

        err = _handler->onStartElement(StubW(markTagStart, (size_t)(markTagEnd - markTagStart)));
        if (FOG_IS_ERROR(err))
          goto _End;

        // ... Fall through ...
      }

      case XML_SAX_STATE_TAG_INSIDE:
      {
        if (XmlUtil_isWhitespace(ch))
          break;

        // Check for start of xml attribute.
        if (XmlUtil_isNameStartChar(ch))
        {
          markAttrStart = sPtr;
          state = XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_NAME;
          break;
        }

        // Check for end tag sequence.
        switch (element)
        {
          case XML_SAX_ELEMENT_TAG:
            if (ch == CharW('/'))
            {
              element = XML_SAX_ELEMENT_TAG_SELF_CLOSING;
              state = XML_SAX_STATE_TAG_END;
              sPtr++;
              goto _Begin;
            }
            if (ch == CharW('>'))
              goto _TagEnd;
            break;
          case XML_SAX_ELEMENT_XML:
            if (ch == CharW('?'))
            {
              state = XML_SAX_STATE_TAG_END;
              sPtr++;
              goto _Begin;
            }
            break;
        }

        err = ERR_XML_SAX_SYNTAX;
        goto _End;
      }

      case XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_NAME:
      {
        if (XmlUtil_isNameChar(ch))
          break;

        markAttrEnd = sPtr;

        // Now we expect =
        while (XmlUtil_isWhitespace(ch))
        {
          if (++sPtr == sEnd)
            goto _EndOfInput;
          ch = *sPtr;
        }

        if (ch != CharW('='))
        {
          err = ERR_XML_SAX_SYNTAX;
          goto _End;
        }

        if (++sPtr == sEnd)
          goto _EndOfInput;
        ch = *sPtr;

        // Now we expect ' or "
        while (XmlUtil_isWhitespace(ch))
        {
          if (++sPtr == sEnd)
            goto _EndOfInput;
          ch = *sPtr;
        }

        if (ch == CharW('\'') || ch == CharW('\"'))
        {
          attr = ch;
          state = XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_VALUE;
          markDataStart = ++sPtr;
          goto _Begin;
        }
        else
        {
          err = ERR_XML_SAX_SYNTAX;
          goto _End;
        }
      }

      case XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_VALUE:
      {
        if (ch != attr)
          break;

        markDataEnd = sPtr;
        sPtr++;
        state = XML_SAX_STATE_TAG_INSIDE;

        err = _handler->onAttribute(
          StubW(markAttrStart, (size_t)(markAttrEnd - markAttrStart)),
          StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));

        if (FOG_IS_ERROR(err))
        {
          err = _handler->onError(location, err);
          if (FOG_IS_ERROR(err))
            goto _End;
        }

        goto _Begin;
      }

      case XML_SAX_STATE_TAG_END:
      {
        if (XmlUtil_isWhitespace(ch))
          break;

        if (ch == CharW('>'))
        {
_TagEnd:
          state = XML_SAX_STATE_READY;
          mark = ++sPtr;

          // Call onCloseElement() only for self-closing ones.
          if (element == XML_SAX_ELEMENT_TAG_SELF_CLOSING)
          {
            depth--;
            err = _handler->onEndElement(StubW(markTagStart, (size_t)(markTagEnd - markTagStart)));
            if (FOG_IS_ERROR(err))
              goto _End;
          }

          goto _Begin;
        }
        break;
      }

      case XML_SAX_STATE_TAG_CLOSE:
      {
        // Only possible sequence here is [StartTagSequence].
        if (ch.isLetter() || ch == CharW('_') || ch == CharW(':'))
        {
          state = XML_SAX_STATE_TAG_CLOSE_NAME;
          markTagStart = sPtr;
          break;
        }

        err = ERR_XML_SAX_SYNTAX;
        goto _End;
      }

      case XML_SAX_STATE_TAG_CLOSE_NAME:
      {
        if (ch.isNumlet() || ch == CharW('_') || ch == CharW(':') || ch == CharW('-') || ch == CharW('.'))
          break;

        state = XML_SAX_STATE_TAG_CLOSE_END;
        markTagEnd = sPtr;

        // ... Fall through ...
      }

      case XML_SAX_STATE_TAG_CLOSE_END:
      {
        // This is we are waiting for.
        if (ch == CharW('>'))
        {
          state = XML_SAX_STATE_READY;
          mark = ++sPtr;
          depth--;

          err = _handler->onEndElement(StubW(markTagStart, (size_t)(markTagEnd - markTagStart)));
          if (FOG_IS_ERROR(err))
            goto _End;

          goto _Begin;
        }

        if (XmlUtil_isWhitespace(ch))
          break;

        // Syntax Error.
        err = ERR_XML_SAX_SYNTAX;
        goto _End;
      }

      case XML_SAX_STATE_TAG_QUESTION_MARK:
      {
        const CharW* targetStart = sPtr;
        const CharW* targetEnd = sPtr;

        // Parse 'Target'.
        if (XmlUtil_isNameStartChar(sPtr[0]))
        {
          for (;;)
          {
            if (++sPtr == sEnd)
              goto _EndOfInput;
            if (!XmlUtil_isNameChar(sPtr[0]))
              break;
          }

          targetEnd = sPtr;
        }
        else
        {
          // We support "no-target", like <? ... ?> and <% ... %>.
        }

        // Parse 'S'.
        if (!XmlUtil_isWhitespace(sPtr[0]))
        {
          err = ERR_XML_SAX_SYNTAX;
          goto _End;
        }

        for (;;)
        {
          if (++sPtr == sEnd)
            goto _EndOfInput;
            
          if (!XmlUtil_isWhitespace(sPtr[0]))
            break;
        }

        markDataStart = sPtr;

        // Special case '<?xml...'.
        if ((size_t)(targetEnd - targetStart) == 3 &&
            StringUtil::eq(targetStart, "xml", 3, CASE_INSENSITIVE))
        {
          element = XML_SAX_ELEMENT_XML;
          state = XML_SAX_STATE_TAG_INSIDE;
          goto _Begin;
        }

        // Parse 'Data'.
        const CharW* q = sEnd - 1;

        while (sPtr < q && (sPtr[0] != CharW('?') || sPtr[1] != CharW('>')))
          sPtr++;

        if (sPtr == q)
        {
          err = ERR_XML_SAX_SYNTAX;
          goto _End;
        }
        else
        {
          markDataEnd = sPtr;
          sPtr += 2;

          state = XML_SAX_STATE_READY;
          mark = sPtr;

          err = _handler->onProcessingInstruction(
            StubW(targetStart, (size_t)(targetEnd - targetStart)),
            StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));

          if (FOG_IS_ERROR(err))
            goto _End;

          goto _Begin;
        }
      }

      case XML_SAX_STATE_TAG_EXCLAMATION_MARK:
      {
        if ((size_t)(sEnd - sPtr) > 1 && StringUtil::eq(sPtr, "--", 2, CASE_SENSITIVE))
        {
          state = XML_SAX_STATE_COMMENT;
          sPtr += 3;
          goto _Begin;
        }
        else if ((size_t)(sEnd - sPtr) > 7 && StringUtil::eq(sPtr, "DOCTYPE", 7, CASE_SENSITIVE) && XmlUtil_isWhitespace(sPtr[7]))
        {
          element = XML_SAX_ELEMENT_DOCTYPE;
          state = XML_SAX_STATE_DOCTYPE;
          sPtr += 8;
          doctype.clear();
          goto _Begin;
        }
        else if ((size_t)(sEnd - sPtr) > 6 && StringUtil::eq(sPtr, "[CDATA[", 7, CASE_SENSITIVE))
        {
          element = XML_SAX_ELEMENT_CDATA;
          state = XML_SAX_STATE_CDATA;
          sPtr += 7;
          goto _Begin;
        }
        else
        {
          err = ERR_XML_SAX_SYNTAX;
          goto _End;
        }
      }

      case XML_SAX_STATE_DOCTYPE:
      {
        if (ch.isSpace()) break;

        if (doctype.getLength() < 2)
        {
          if (ch.isLetter() || ch == CharW('_'))
          {
            state = XML_SAX_STATE_DOCTYPE_TEXT;
            markDataStart = ++sPtr;
            goto _Begin;
          }

          // End of DOCTYPE
          if (ch == CharW('>'))
            goto _DOCTYPEEnd;
        }
        else
        {
          if (ch == CharW('\"'))
          {
            if (doctype.getLength() < 4)
            {
              state = XML_SAX_STATE_DOCTYPE_ATTRIBUTE;
              markDataStart = ++sPtr;
              goto _Begin;
            }
            else
            {
              err = ERR_XML_SAX_SYNTAX;
              goto _End;
            }
          }
          if (ch == CharW('>'))
          {
_DOCTYPEEnd:
            err = _handler->onDOCTYPE(doctype);

            if (FOG_IS_ERROR(err))
              return err;

            state = XML_SAX_STATE_READY;
            mark = ++sPtr;
            goto _Begin;
          }
        }

        err = ERR_XML_SAX_SYNTAX;
        break;
      }

      case XML_SAX_STATE_DOCTYPE_TEXT:
      {
        if (ch.isNumlet() || ch == CharW('_') || ch == CharW(':') || ch == CharW('-') || ch == CharW('.')) break;
        markDataEnd = sPtr;
        doctype.append(StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));

        state = XML_SAX_STATE_DOCTYPE;
        goto _Continue;
      }

      case XML_SAX_STATE_DOCTYPE_ATTRIBUTE:
      {
        if (ch != CharW('\"')) break;

        markDataEnd = sPtr;
        doctype.append(StringW(markDataStart, (size_t)(markDataEnd - markDataStart)));

        state = XML_SAX_STATE_DOCTYPE;
        break;
      }

      case XML_SAX_STATE_COMMENT:
      {
        const CharW* q = sEnd - 2;

        while (sPtr < q && (sPtr[0] != CharW('-') || sPtr[1] != CharW('-') || sPtr[2] != CharW('>')))
          sPtr++;

        if (sPtr == q)
        {
          err = ERR_XML_SAX_SYNTAX;
          goto _End;
        }
        else
        {
          markDataEnd = sPtr;
          sPtr += 3;

          state = XML_SAX_STATE_READY;
          mark = sPtr;

          err = _handler->onComment(StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));
          if (FOG_IS_ERROR(err))
            goto _End;

          goto _Begin;
        }
      }

      case XML_SAX_STATE_CDATA:
      {
        const CharW* q = sEnd - 2;

        while (sPtr < q && sPtr[0] != CharW(']') && sPtr[1] != CharW(']') && sPtr[2] != CharW('>'))
          sPtr++;

        if (sPtr == q)
        {
          err = ERR_XML_SAX_SYNTAX;
          goto _End;
        }
        else
        {
          markDataEnd = sPtr;
          sPtr += 3;

          state = XML_SAX_STATE_READY;
          mark = sPtr;

          err = _handler->onCDATASection(StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));
          if (FOG_IS_ERROR(err))
            goto _End;

          goto _Begin;
        }
      }

      default:
      {
        err = ERR_XML_SAX_INTERNAL;
        goto _End;
      }
    }

    sPtr++;
  }

_EndOfInput:
  if (depth > 0 || state != XML_SAX_STATE_READY)
  {
    err = ERR_XML_SAX_SYNTAX;
  }

_End:
  _handler->onEndDocument();
  return err;
}

} // Fog namespace
