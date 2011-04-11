// [Fog-Xml]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/IO/MapFile.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Tools/Byte.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Xml/Global/Constants.h>
#include <Fog/Xml/IO/XmlSaxReader.h>
#include <Fog/Xml/Tools/XmlEntity_p.h>

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

static bool xmlIsWhiteSpace(const Char* buffer, const Char* end)
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

err_t XmlSaxReader::parseFile(const String& fileName)
{
  Stream stream;
  err_t err = stream.openFile(fileName, STREAM_OPEN_READ);
  if (FOG_IS_ERROR(err)) return err;

  return parseStream(stream);
}

err_t XmlSaxReader::parseStream(Stream& stream)
{
  ByteArray buffer;
  stream.readAll(buffer);
  return parseMemory(reinterpret_cast<const void*>(buffer.getData()), buffer.getLength());
}

err_t XmlSaxReader::parseMemory(const void* mem, sysuint_t size)
{
  TextCodec textCodec = _detectEncoding(mem, size);
  if (textCodec.isNull()) textCodec.setCode(TextCodec::UTF8);

  String buffer;
  err_t err = textCodec.toUnicode(buffer, mem, size);
  if (FOG_IS_ERROR(err)) return err;

  return parseString(buffer.getData(), buffer.getLength());
}

err_t XmlSaxReader::parseString(const Char* s, sysuint_t len)
{
  // Check if encoded length is zero (no document).
  if (len == DETECT_LENGTH) len = StringUtil::len(s);
  if (len == 0) return ERR_XML_NO_DOCUMENT;

  const Char* strCur = s;           // Parsing buffer.
  const Char* strEnd = s + len;     // End of buffer.

  const Char* mark = s;             // Mark to start position of currently parsed item.

  const Char* markTagStart  = NULL; // Mark to start position of currently parsed tag name.
  const Char* markTagEnd    = NULL; // Mark to end position of currently parsed tag name.

  const Char* markAttrStart = NULL; // Mark to start of attribute.
  const Char* markAttrEnd   = NULL; // Mark to end of attribute.

  const Char* markDataStart = NULL; // Mark to start of data (CDATA, Comment, attribute text, ...).
  const Char* markDataEnd   = NULL; // Mark to end of data (CDATA, Comment, attribute text, ...).

  Char ch;                          // Current character.
  Char attr;                        // Attribute marker (' or ").

  err_t err = ERR_OK;               // Current error code.
  int state = XML_SAX_STATE_READY;  // Current state.
  int element = XML_SAX_ELEMENT_TAG;// Element type.
  int depth = 0;                    // Current depth.
  bool skipTagText = true;          // skip tag text...?

  // Temporary reusable strings.
  String tempTagName;
  String tempAttrName;
  String tempAttrValue;
  String tempText;
  String tempData;

  List<String> doctype;

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
        if (ch == Char('<'))
        {
          // If there is text, we will call addText().
          if (mark != strCur)
          {
            bool isWhiteSpace = xmlIsWhiteSpace(mark, strCur);

            err = onAddText(Utf16(mark, (sysuint_t)(strCur - mark)), isWhiteSpace);
            if (FOG_IS_ERROR(err)) goto _End;
          }

          state = XML_SAX_STATE_TAG_BEGIN;
          mark = strCur;
        }
        break;

      case XML_SAX_STATE_TAG_BEGIN:
        // Match start tag name (this is probably the most common).
        if (ch.isAlpha() || ch == Char('_') || ch == Char(':'))
        {
          state = XML_SAX_STATE_TAG_NAME;
          markTagStart = strCur;
          break;
        }

        // Match closing tag slash.
        if (ch.ch() == Char('/'))
        {
          state = XML_SAX_STATE_TAG_CLOSE;
          break;
        }

        if (ch.ch() == Char('?'))
        {
          state = XML_SAX_STATE_TAG_QUESTION_MARK;
          break;
        }

        if (ch.ch() == Char('!'))
        {
          state = XML_SAX_STATE_TAG_EXCLAMATION_MARK;
          break;
        }

        if (ch.isSpace()) break;

        // Syntax Error
        err = ERR_XML_SYNTAX_ERROR;
        goto _End;

      case XML_SAX_STATE_TAG_NAME:
        if (ch.isAlnum() || ch == Char('_') || ch == Char(':') || ch == Char('-') || ch == Char('.'))
          break;

        markTagEnd = strCur;

        state = XML_SAX_STATE_TAG_INSIDE;
        depth++;
        element = XML_SAX_ELEMENT_TAG;

        err = onAddElement(Utf16(markTagStart, (sysuint_t)(markTagEnd - markTagStart)));
        if (FOG_IS_ERROR(err)) goto _End;

        // ... go through ...

      case XML_SAX_STATE_TAG_INSIDE:
        if (ch.isSpace()) break;

        // Check for start of xml attribute.
        if (ch.isAlpha() || ch == Char('_'))
        {
          markAttrStart = strCur;
          state = XML_SAX_STATE_TAG_INSIDE_ATTRIBUTE_NAME;
          break;
        }

        // Check for end tag sequence.
        switch (element)
        {
          case XML_SAX_ELEMENT_TAG:
            if (ch == Char('/'))
            {
              element = XML_SAX_ELEMENT_TAG_SELF_CLOSING;
              state = XML_SAX_STATE_TAG_END;
              strCur++;
              goto _Begin;
            }
            if (ch == Char('>')) goto _TagEnd;
            break;
          case XML_SAX_ELEMENT_XML:
            if (ch == Char('?'))
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
        if (ch.isAlnum()  || ch == Char('_') || ch == Char(':') || ch == Char('-') || ch == Char('.')) break;

        markAttrEnd = strCur;

        // Now we expect =
        while (ch.isSpace())
        {
          if (++strCur == strEnd) goto _EndOfInput;
          ch = *strCur;
        }

        if (ch != Char('='))
        {
          err = ERR_XML_SYNTAX_ERROR;
          goto _End;
        }

        if (++strCur == strEnd) goto _EndOfInput;
        ch = *strCur;

        // Now we expect ' or "
        while (ch.isSpace())
        {
          if (++strCur == strEnd) goto _EndOfInput;
          ch = *strCur;
        }

        if (ch == Char('\'') || ch == Char('\"'))
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
          Utf16(markAttrStart, (sysuint_t)(markAttrEnd - markAttrStart)),
          Utf16(markDataStart, (sysuint_t)(markDataEnd - markDataStart)));
        if (FOG_IS_ERROR(err)) goto _End;

        goto _Begin;

      case XML_SAX_STATE_TAG_END:
        if (ch.isSpace()) break;

        if (ch == Char('>'))
        {
_TagEnd:
          state = XML_SAX_STATE_READY;
          mark = ++strCur;

          // Call onCloseElement() only for self-closing ones.
          if (element == XML_SAX_ELEMENT_TAG_SELF_CLOSING)
          {
            depth--;
            err = onCloseElement(Utf16(markTagStart, (sysuint_t)(markTagEnd - markTagStart)));
            if (FOG_IS_ERROR(err)) goto _End;
          }

          goto _Begin;
        }

        break;

      case XML_SAX_STATE_TAG_CLOSE:
        // Only possible sequence here is [StartTagSequence].
        if (ch.isAlpha() || ch == Char('_') || ch == Char(':'))
        {
          state = XML_SAX_STATE_TAG_CLOSE_NAME;
          markTagStart = strCur;
          break;
        }

        err = ERR_XML_SYNTAX_ERROR;
        goto _End;

      case XML_SAX_STATE_TAG_CLOSE_NAME:
        if (ch.isAlnum() || ch == Char('_') || ch == Char(':') || ch == Char('-') || ch == Char('.'))
          break;

        state = XML_SAX_STATE_TAG_CLOSE_END;
        markTagEnd = strCur;

        // ...go through ...

      case XML_SAX_STATE_TAG_CLOSE_END:
        // This is we are waiting for.
        if (ch == Char('>'))
        {
          state = XML_SAX_STATE_READY;
          mark = ++strCur;
          depth--;

          err = onCloseElement(Utf16(markTagStart, (sysuint_t)(markTagEnd - markTagStart)));
          if (FOG_IS_ERROR(err)) goto _End;

          goto _Begin;
        }

        if (ch.isSpace()) break;

        // Syntax Error.
        err = ERR_XML_SYNTAX_ERROR;
        goto _End;

      case XML_SAX_STATE_TAG_QUESTION_MARK:
        if ((sysuint_t)(strEnd - strCur) > 3 && StringUtil::eq(strCur, "xml", 3, CASE_INSENSITIVE) && strCur[3].isSpace())
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
        if ((sysuint_t)(strEnd - strCur) > 1 && StringUtil::eq(strCur, "--", 2, CASE_SENSITIVE))
        {
          state = XML_SAX_STATE_COMMENT;
          strCur += 3;
          goto _Begin;
        }
        else if ((sysuint_t)(strEnd - strCur) > 7 && StringUtil::eq(strCur, "DOCTYPE", 7, CASE_SENSITIVE) && strCur[7].isSpace())
        {
          element = XML_SAX_ELEMENT_DOCTYPE;
          state = XML_SAX_STATE_DOCTYPE;
          strCur += 8;
          doctype.clear();
          goto _Begin;
        }
        else if ((sysuint_t)(strEnd - strCur) > 6 && StringUtil::eq(strCur, "[CDATA[", 7, CASE_SENSITIVE))
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
          if (ch.isAlpha() || ch == Char('_'))
          {
            state = XML_SAX_STATE_DOCTYPE_TEXT;
            markDataStart = ++strCur;
            goto _Begin;
          }

          // End of DOCTYPE
          if (ch == Char('>')) goto _DOCTYPEEnd;
        }
        else
        {
          if (ch == Char('\"'))
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
          if (ch == Char('>'))
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
        if (ch.isAlnum() || ch == Char('_') || ch == Char(':') || ch == Char('-') || ch == Char('.')) break;
        markDataEnd = strCur;
        doctype.append(Utf16(markDataStart, (sysuint_t)(markDataEnd - markDataStart)));

        state = XML_SAX_STATE_DOCTYPE;
        goto _Continue;

      case XML_SAX_STATE_DOCTYPE_ATTRIBUTE:
        if (ch != Char('\"')) break;

        markDataEnd = strCur;
        doctype.append(String(markDataStart, (sysuint_t)(markDataEnd - markDataStart)));

        state = XML_SAX_STATE_DOCTYPE;
        break;

      case XML_SAX_STATE_PI:
      {
        const Char* q = strEnd-1;

        while (strCur < q &&
               strCur[0].ch() != Char('?') &&
               strCur[1].ch() != Char('>')) strCur++;

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

          err = onAddPI(Utf16(markDataStart, (sysuint_t)(markDataEnd - markDataStart)));
          if (FOG_IS_ERROR(err)) goto _End;

          goto _Begin;
        }
      }

      case XML_SAX_STATE_COMMENT:
      {
        const Char* q = strEnd-2;

        while (strCur < q && (
               strCur[0].ch() != Char('-') ||
               strCur[1].ch() != Char('-') ||
               strCur[2].ch() != Char('>'))) strCur++;

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

          err = onAddComment(Utf16(markDataStart, (sysuint_t)(markDataEnd - markDataStart)));
          if (FOG_IS_ERROR(err)) goto _End;

          goto _Begin;
        }
      }

      case XML_SAX_STATE_CDATA:
      {
        const Char* q = strEnd-2;

        while (strCur < q &&
               strCur[0].ch() != Char(']') &&
               strCur[1].ch() != Char(']') &&
               strCur[2].ch() != Char('>')) strCur++;

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

          err = onAddCDATA(Utf16(markDataStart, (sysuint_t)(markDataEnd - markDataStart)));
          if (FOG_IS_ERROR(err)) goto _End;

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

TextCodec XmlSaxReader::_detectEncoding(const void* mem, sysuint_t size)
{
  // first check for BOM
  TextCodec textCodec = TextCodec::fromBom(mem, size);
  if (!textCodec.isNull()) return textCodec;

  const char* ptr = reinterpret_cast<const char*>(mem);
  const char* end = ptr + size;

  if (size < 15) goto _End;

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
        if (*ptr == '>') goto _End;
        if (Byte::isSpace(*ptr) && StringUtil::eq(ptr + 1, "encoding", 8, CASE_INSENSITIVE))
        {
          // We are in "<?xml ..... encoding".
          const char* begin;
          char q;
          ptr += 9;

          // Find '='.
          while (ptr != end && *ptr != '=') ptr++;
          if (ptr == end) goto _End;

          ptr++;

          // We are in "<?xml ..... encoding = "
          while (ptr != end && Byte::isSpace(*ptr)) ptr++;
          if (ptr == end) goto _End;

          q = *ptr++;
          begin = ptr;

          while (ptr != end && *ptr != q) ptr++;
          if (ptr == end) goto _End;

          // Try encoding and return
          textCodec = TextCodec::fromMime(Ascii8(begin, (sysuint_t)(ptr - begin)));
          goto _End;
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

_End:
  return textCodec;
}

} // Fog namespace
