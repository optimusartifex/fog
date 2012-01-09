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
#include <Fog/Core/Xml/XmlEntity_p.h>
#include <Fog/Core/Xml/XmlSaxReader.h>

namespace Fog {

// ============================================================================
// [Fog::XmlSaxReader - Constants]
// ============================================================================

//! @internal.
//!
//! @brief Parser state.
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
  XML_SAX_STATE_PI,
  //! @brief Parser is in <!-- --> sequence.
  XML_SAX_STATE_COMMENT,
  //! @brief Parser is in <![[CDATA ]]> sequence.
  XML_SAX_STATE_CDATA
};

//! @internal.
//!
//! @brief Type of currently parsed element.
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
// [Fog::XmlSaxReader - Helpers]
// ============================================================================

static bool xmlIsWhiteSpace(const CharW* buffer, const CharW* end)
{
  while (buffer < end)
  {
    if (!buffer->isSpace()) return false;
    buffer++;
  }
  return true;
}

// ============================================================================
// [Fog::XmlSaxReader]
// ============================================================================

XmlSaxReader::XmlSaxReader()
{
}

XmlSaxReader::~XmlSaxReader()
{
}

err_t XmlSaxReader::parseFile(const StringW& fileName)
{
  Stream stream;
  err_t err = stream.openFile(fileName, STREAM_OPEN_READ);
  if (FOG_IS_ERROR(err)) return err;

  return parseStream(stream);
}

err_t XmlSaxReader::parseStream(Stream& stream)
{
  StringA buffer;
  stream.readAll(buffer);
  return parseMemory(reinterpret_cast<const void*>(buffer.getData()), buffer.getLength());
}

err_t XmlSaxReader::parseMemory(const void* mem, size_t size)
{
  TextCodec textCodec = TextCodec::utf8();
  _detectEncoding(textCodec, mem, size);

  StringW buffer;
  err_t err = textCodec.decode(buffer, StubA(reinterpret_cast<const char*>(mem), size));
  if (FOG_IS_ERROR(err)) return err;

  return parseString(buffer.getData(), buffer.getLength());
}

err_t XmlSaxReader::parseString(const CharW* s, size_t len)
{
  // Check if encoded length is zero (no document).
  if (len == DETECT_LENGTH) len = StringUtil::len(s);
  if (len == 0) return ERR_XML_NO_DOCUMENT;

  const CharW* strCur = s;           // Parsing buffer.
  const CharW* strEnd = s + len;     // End of buffer.

  const CharW* mark = s;             // Mark to start position of currently parsed item.

  const CharW* markTagStart  = NULL; // Mark to start position of currently parsed tag name.
  const CharW* markTagEnd    = NULL; // Mark to end position of currently parsed tag name.

  const CharW* markAttrStart = NULL; // Mark to start of attribute.
  const CharW* markAttrEnd   = NULL; // Mark to end of attribute.

  const CharW* markDataStart = NULL; // Mark to start of data (CDATA, Comment, attribute text, ...).
  const CharW* markDataEnd   = NULL; // Mark to end of data (CDATA, Comment, attribute text, ...).

  CharW ch;                          // Current character.
  CharW attr;                        // Attribute marker (' or ").

  err_t err = ERR_OK;               // Current error code.
  int state = XML_SAX_STATE_READY;  // Current state.
  int element = XML_SAX_ELEMENT_TAG;// Element type.
  int depth = 0;                    // Current depth.

  List<StringW> doctype;

  for (;;)
  {
_Begin:
    if (strCur == strEnd) break;
    ch = *strCur;

_Continue:
    switch (state)
    {
      case XML_SAX_STATE_READY:
        // If xml char has special meaning, we will process it, otherwise go away.
        if (ch == CharW('<'))
        {
          // If there is text, we will call addText().
          if (mark != strCur)
          {
            bool isWhiteSpace = xmlIsWhiteSpace(mark, strCur);

            err = onAddText(StubW(mark, (size_t)(strCur - mark)), isWhiteSpace);
            if (FOG_IS_ERROR(err))
              goto _End;
          }

          state = XML_SAX_STATE_TAG_BEGIN;
          mark = strCur;
        }
        break;

      case XML_SAX_STATE_TAG_BEGIN:
        // Match start tag name (this is probably the most common).
        if (ch.isLetter() || ch == CharW('_') || ch == CharW(':'))
        {
          state = XML_SAX_STATE_TAG_NAME;
          markTagStart = strCur;
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

        if (ch.isSpace()) break;

        // Syntax Error
        err = ERR_XML_SYNTAX_ERROR;
        goto _End;

      case XML_SAX_STATE_TAG_NAME:
        if (ch.isLetter() || ch == CharW('_') || ch == CharW(':') || ch == CharW('-') || ch == CharW('.'))
          break;

        markTagEnd = strCur;

        state = XML_SAX_STATE_TAG_INSIDE;
        depth++;
        element = XML_SAX_ELEMENT_TAG;

        err = onAddElement(StubW(markTagStart, (size_t)(markTagEnd - markTagStart)));
        if (FOG_IS_ERROR(err))
          goto _End;

        // ... go through ...

      case XML_SAX_STATE_TAG_INSIDE:
        if (ch.isSpace()) break;

        // Check for start of xml attribute.
        if (ch.isLetter() || ch == CharW('_'))
        {
          markAttrStart = strCur;
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
              strCur++;
              goto _Begin;
            }
            if (ch == CharW('>')) goto _TagEnd;
            break;
          case XML_SAX_ELEMENT_XML:
            if (ch == CharW('?'))
            {
              state = XML_SAX_STATE_TAG_END;
              strCur++;
              goto _Begin;
            }
            break;
        }

        err = ERR_XML_SYNTAX_ERROR;
        goto _End;

      case XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_NAME:
        if (ch.isNumlet()  || ch == CharW('_') || ch == CharW(':') || ch == CharW('-') || ch == CharW('.')) break;

        markAttrEnd = strCur;

        // Now we expect =
        while (ch.isSpace())
        {
          if (++strCur == strEnd)
            goto _EndOfInput;
          ch = *strCur;
        }

        if (ch != CharW('='))
        {
          err = ERR_XML_SYNTAX_ERROR;
          goto _End;
        }

        if (++strCur == strEnd)
          goto _EndOfInput;
        ch = *strCur;

        // Now we expect ' or "
        while (ch.isSpace())
        {
          if (++strCur == strEnd) goto _EndOfInput;
          ch = *strCur;
        }

        if (ch == CharW('\'') || ch == CharW('\"'))
        {
          attr = ch;
          state = XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_VALUE;
          markDataStart = ++strCur;
          goto _Begin;
        }
        else
        {
          err = ERR_XML_SYNTAX_ERROR;
          goto _End;
        }

      case XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_VALUE:
        if (ch != attr) break;

        markDataEnd = strCur;
        strCur++;
        state = XML_SAX_STATE_TAG_INSIDE;

        err = onAddAttribute(
          StubW(markAttrStart, (size_t)(markAttrEnd - markAttrStart)),
          StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));
        if (FOG_IS_ERROR(err))
          goto _End;

        goto _Begin;

      case XML_SAX_STATE_TAG_END:
        if (ch.isSpace()) break;

        if (ch == CharW('>'))
        {
_TagEnd:
          state = XML_SAX_STATE_READY;
          mark = ++strCur;

          // Call onCloseElement() only for self-closing ones.
          if (element == XML_SAX_ELEMENT_TAG_SELF_CLOSING)
          {
            depth--;
            err = onCloseElement(StubW(markTagStart, (size_t)(markTagEnd - markTagStart)));
            if (FOG_IS_ERROR(err))
              goto _End;
          }

          goto _Begin;
        }

        break;

      case XML_SAX_STATE_TAG_CLOSE:
        // Only possible sequence here is [StartTagSequence].
        if (ch.isLetter() || ch == CharW('_') || ch == CharW(':'))
        {
          state = XML_SAX_STATE_TAG_CLOSE_NAME;
          markTagStart = strCur;
          break;
        }

        err = ERR_XML_SYNTAX_ERROR;
        goto _End;

      case XML_SAX_STATE_TAG_CLOSE_NAME:
        if (ch.isNumlet() || ch == CharW('_') || ch == CharW(':') || ch == CharW('-') || ch == CharW('.'))
          break;

        state = XML_SAX_STATE_TAG_CLOSE_END;
        markTagEnd = strCur;

        // ...go through ...

      case XML_SAX_STATE_TAG_CLOSE_END:
        // This is we are waiting for.
        if (ch == CharW('>'))
        {
          state = XML_SAX_STATE_READY;
          mark = ++strCur;
          depth--;

          err = onCloseElement(StubW(markTagStart, (size_t)(markTagEnd - markTagStart)));
          if (FOG_IS_ERROR(err))
            goto _End;

          goto _Begin;
        }

        if (ch.isSpace()) break;

        // Syntax Error.
        err = ERR_XML_SYNTAX_ERROR;
        goto _End;

      case XML_SAX_STATE_TAG_QUESTION_MARK:
        if ((size_t)(strEnd - strCur) > 3 && StringUtil::eq(strCur, "xml", 3, CASE_INSENSITIVE) && strCur[3].isSpace())
        {
          element = XML_SAX_ELEMENT_XML;
          state = XML_SAX_STATE_TAG_INSIDE;
          strCur += 4;
          goto _Begin;
        }
        else
        {
          markDataStart = strCur;
          state = XML_SAX_STATE_PI;
        }
        break;

      case XML_SAX_STATE_TAG_EXCLAMATION_MARK:
        if ((size_t)(strEnd - strCur) > 1 && StringUtil::eq(strCur, "--", 2, CASE_SENSITIVE))
        {
          state = XML_SAX_STATE_COMMENT;
          strCur += 3;
          goto _Begin;
        }
        else if ((size_t)(strEnd - strCur) > 7 && StringUtil::eq(strCur, "DOCTYPE", 7, CASE_SENSITIVE) && strCur[7].isSpace())
        {
          element = XML_SAX_ELEMENT_DOCTYPE;
          state = XML_SAX_STATE_DOCTYPE;
          strCur += 8;
          doctype.clear();
          goto _Begin;
        }
        else if ((size_t)(strEnd - strCur) > 6 && StringUtil::eq(strCur, "[CDATA[", 7, CASE_SENSITIVE))
        {
          element = XML_SAX_ELEMENT_CDATA;
          state = XML_SAX_STATE_CDATA;
          strCur += 7;
          goto _Begin;
        }
        else
        {
          err = ERR_XML_SYNTAX_ERROR;
          goto _End;
        }

      case XML_SAX_STATE_DOCTYPE:
        if (ch.isSpace()) break;

        if (doctype.getLength() < 2)
        {
          if (ch.isLetter() || ch == CharW('_'))
          {
            state = XML_SAX_STATE_DOCTYPE_TEXT;
            markDataStart = ++strCur;
            goto _Begin;
          }

          // End of DOCTYPE
          if (ch == CharW('>')) goto _DOCTYPEEnd;
        }
        else
        {
          if (ch == CharW('\"'))
          {
            if (doctype.getLength() < 4)
            {
              state = XML_SAX_STATE_DOCTYPE_ATTRIBUTE;
              markDataStart = ++strCur;
              goto _Begin;
            }
            else
            {
              err = ERR_XML_SYNTAX_ERROR;
              goto _End;
            }
          }
          if (ch == CharW('>'))
          {
_DOCTYPEEnd:
            if ((err = onAddDOCTYPE(doctype))) return err;
            state = XML_SAX_STATE_READY;
            mark = ++strCur;
            goto _Begin;
          }
        }

        err = ERR_XML_SYNTAX_ERROR;
        break;

      case XML_SAX_STATE_DOCTYPE_TEXT:
        if (ch.isNumlet() || ch == CharW('_') || ch == CharW(':') || ch == CharW('-') || ch == CharW('.')) break;
        markDataEnd = strCur;
        doctype.append(StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));

        state = XML_SAX_STATE_DOCTYPE;
        goto _Continue;

      case XML_SAX_STATE_DOCTYPE_ATTRIBUTE:
        if (ch != CharW('\"')) break;

        markDataEnd = strCur;
        doctype.append(StringW(markDataStart, (size_t)(markDataEnd - markDataStart)));

        state = XML_SAX_STATE_DOCTYPE;
        break;

      case XML_SAX_STATE_PI:
      {
        const CharW* q = strEnd-1;

        while (strCur < q &&
               strCur[0] != CharW('?') &&
               strCur[1] != CharW('>')) strCur++;

        if (strCur == q)
        {
          err = ERR_XML_SYNTAX_ERROR;
          goto _End;
        }
        else
        {
          markDataEnd = strCur;
          strCur += 2;

          state = XML_SAX_STATE_READY;
          mark = strCur;

          err = onAddPI(StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));
          if (FOG_IS_ERROR(err))
            goto _End;

          goto _Begin;
        }
      }

      case XML_SAX_STATE_COMMENT:
      {
        const CharW* q = strEnd-2;

        while (strCur < q && (
               strCur[0] != CharW('-') ||
               strCur[1] != CharW('-') ||
               strCur[2] != CharW('>'))) strCur++;

        if (strCur == q)
        {
          err = ERR_XML_SYNTAX_ERROR;
          goto _End;
        }
        else
        {
          markDataEnd = strCur;
          strCur += 3;

          state = XML_SAX_STATE_READY;
          mark = strCur;

          err = onAddComment(StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));
          if (FOG_IS_ERROR(err))
            goto _End;

          goto _Begin;
        }
      }

      case XML_SAX_STATE_CDATA:
      {
        const CharW* q = strEnd-2;

        while (strCur < q &&
               strCur[0] != CharW(']') &&
               strCur[1] != CharW(']') &&
               strCur[2] != CharW('>')) strCur++;

        if (strCur == q)
        {
          err = ERR_XML_SYNTAX_ERROR;
          goto _End;
        }
        else
        {
          markDataEnd = strCur;
          strCur += 3;

          state = XML_SAX_STATE_READY;
          mark = strCur;

          err = onAddCDATA(StubW(markDataStart, (size_t)(markDataEnd - markDataStart)));
          if (FOG_IS_ERROR(err))
            goto _End;

          goto _Begin;
        }
      }

      default:
        err = ERR_XML_INTERNAL;
        goto _End;
    }

    strCur++;
  }

_EndOfInput:
  if (depth > 0 || state != XML_SAX_STATE_READY)
  {
    err = ERR_XML_SYNTAX_ERROR;
  }

_End:
  return err;
}

void XmlSaxReader::_detectEncoding(TextCodec& tc, const void* mem, size_t size)
{
  // first check for BOM
  if (tc.createFromBom(mem, size) == ERR_OK) return;

  const char* ptr = reinterpret_cast<const char*>(mem);
  const char* end = ptr + size;

  if (size < 15) return;

  while (ptr != end)
  {
    // TODO: Detect UTF16LE, UTF16BE, UTF32LE, UTF32BE

    if  (ptr[0] == '<' && ptr < end - 5 &&
         ptr[1] == '?' &&
        (ptr[2] == 'x' || ptr[2] == 'X') &&
        (ptr[3] == 'm' || ptr[2] == 'M') &&
        (ptr[4] == 'l' || ptr[2] == 'L'))
    {
      // Xml header, we are in "<?xml".
      ptr += 5;

      while(ptr + 9 < end)
      {
        if (*ptr == '>') return;
        if (CharA::isSpace(*ptr) && StringUtil::eq(ptr + 1, "encoding", 8, CASE_INSENSITIVE))
        {
          // We are in "<?xml ..... encoding".
          const char* begin;
          char q;
          ptr += 9;

          // Find '='.
          while (ptr != end && *ptr != '=') ptr++;
          if (ptr == end) return;

          ptr++;

          // We are in "<?xml ..... encoding = "
          while (ptr != end && CharA::isSpace(*ptr)) ptr++;
          if (ptr == end) return;

          q = *ptr++;
          begin = ptr;

          while (ptr != end && *ptr != q) ptr++;
          if (ptr == end) return;

          // Try encoding and return.
          tc.createFromMime(Ascii8(begin, (size_t)(ptr - begin)));
          return;
        }
        ptr++;
      }
    }
    else if (ptr[0] == '<')
    {
      // xml header not found, default encoding is UTF-8
      break;
    }

    ptr++;
  }
}

} // Fog namespace
