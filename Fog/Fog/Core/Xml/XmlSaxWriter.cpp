// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Core/Xml/XmlEntity_p.h>
#include <Fog/Core/Xml/XmlSaxWriter.h>

namespace Fog {

// ============================================================================
// [Fog::XmlSaxWriter]
// ============================================================================

XmlSaxWriter::XmlSaxWriter()
{
}

XmlSaxWriter::~XmlSaxWriter()
{
}

} // Fog namespace














// STOPPED HERE!
#if 0
#define __XML_CHAR_ERROR(n) return n
  //XmlNode* doc = _doc._root; // new xml tree
  //XmlNode* cur;              // current node
  //XmlAttribute *attributes;  // attributes

  // ===== Find first tag =====
  for (;;)
  {
    if (strCur == strEnd) __XML_CHAR_ERROR(XmlReaderMissingRootTag);
    else if (strCur->isSpace()) strCur++;
    else if (*strCur == '<') break;
    else __XML_CHAR_ERROR(XmlReaderMissingRootTag);
  }

  // ---------------------------------------------------------------------
  // ===== Loop (*strCur is now '<' --- beginning of first tag) =====
  for (;;)
  {
    // next character
    strCur++;

    // ----- New tag...? -----
    if (strCur->isAlpha() || *strCur == '_' || *strCur == ':' || *strCur > 127)
    {
      if (!cur) __XML_CHAR_ERROR(XmlReaderUnmatchedClosingTag);

      // === Clean attributes ===
      attributes = NULL;

      // === Parse tag name ===
      const CharW* tag = strCur;
      uint tagLength = xmlStrCSPN(strCur, strEnd, XML_CHAR_SPACE | XML_CHAR_SLASH | XML_CHAR_GT);

      strCur += tagLength;

      // === New tag attributes ===
      while (strCur != strEnd && *strCur != '/' && *strCur != '>')
      {
        // Skip spaces
        while (strCur->isSpace()) strCur++;

        // Temp variable where is stored beginning of attribute name or text
        const CharW* begin = strCur;

        strCur += xmlStrCSPN(strCur, strEnd, XML_CHAR_SPACE | XML_CHAR_EQUAL | XML_CHAR_SLASH | XML_CHAR_GT);
        if (*strCur == '=' || strCur->isSpace())
        {
          // Create a new attribute.
          {
            XmlAttribute *a = fog_new XmlAttribute();

            a->_prev = attributes;
            a->_next = NULL;
            a->setName(StringW(begin, (size_t)(strCur - begin)));

            if (attributes) attributes->_next = a;
            attributes = a;
          }

          // q can be " or '
          CharW q = *(strCur += xmlStrSPN(strCur, strEnd, XML_CHAR_SPACE | XML_CHAR_EQUAL));

          // Get attribute value
          if (q == '\"' || q == '\'')
          {
            // Save begin of attribute text
            begin = ++strCur;
            while (strCur != strEnd && *strCur != q) strCur++;

            // Copy attribute text to attribute object Text must be decoded
            decode(attributes->_value, begin, (size_t)(strCur - begin));

            // Assign last node, but this node can be damaged.
            if (strCur == strEnd)
            {
              openTag(&cur, tag, tagLength, attributes);
              closeTag(&cur);

              __XML_CHAR_ERROR(XmlReaderMissingAttribute);
            }

            strCur++;
          }
        }
      }

      // === Self closing tag...? ===
      if (*strCur == '/')
      {
        // Assign node
        openTag(&cur, tag, tagLength, attributes);
        closeTag(&cur);

        // Skip spaces...Allow <SelfClosingNode/ >
        for (;;)
        {
          // strEnd of input will cause error
          if (++strCur == strEnd) __XML_CHAR_ERROR(XmlReaderMissingTag);
          // Skip spaces
          if (strCur->isSpace()) continue;
          // 'Only '>' is valid
          else if (*strCur == '>')
            break;
          else
            __XML_CHAR_ERROR(XmlReaderMissingTag);
        }
      }

      // === Open tag...? ===
      else if (*strCur == '>')
      {
        openTag(&cur, tag, tagLength, attributes);
      }

      // === Error, Missing > ===
      else
      {
        // Assign last node, but this node can be damaged.
        openTag(&cur, tag, tagLength, attributes);
        closeTag(&cur);

        __XML_CHAR_ERROR(XmlReaderMissingTag);
      }

      skipTagText = false;
    }


    // ----- Close tag -----
    else if (*strCur == '/')
    {
      if (!cur) __XML_CHAR_ERROR(XmlReaderUnmatchedClosingTag);

      closeTag(&cur);
      while (strCur != strEnd && *strCur != '>') strCur++;
      if (strCur == strEnd) __XML_CHAR_ERROR(XmlReaderMissingTag);
    }


    // ----- Comment -----
    else if (strCur + 3 <= strEnd && StringW::Raw::eq(strCur, "!--", 3))
    {
      const CharW* dataStart = strCur += 3;

      for (;;)
      {
        if (strCur + 3 >= strEnd) __XML_CHAR_ERROR(XmlReaderUnclosedComment);
        else if (StringUtil::eq(strCur, "-->", 3)) { strCur += 3; break; }
        else strCur++;
      }

      const CharW* dataEnd = strCur - 3;
      addComment(StringW(dataStart, (size_t)(dataEnd - dataStart)));
    }


    // ----- CDATA -----
    else if (strCur + 8 <= strEnd && StringW::Raw::eq(strCur, "![CDATA[", 8))
    {
#if 0
      //FIXME:
      if ((strCur = WUniStrStr(strCur, U_("]]>")) ))
      {
        //WXmlCharContent(root, D + 8, (S += 2) - D - 10, 0);
      }
      else
      {
        __XML_CHAR_ERROR(XmlReaderUnclosedCDATA);
      }
#endif
    }


    // ----- !DOCTYPE -----
    else if (strCur + 8 <= strEnd && StringW::Raw::eq(strCur, "!DOCTYPE", 8))
    {
      uint i;
      for (i = 0; strCur != strEnd && ((!i && *strCur != '>') || (i && (*strCur != ']' ||
                   strCur[xmlStrSPN(strCur + 1, strEnd, XML_CHAR_SPACE) + 1] != '>')));
                   i = (*strCur == '[') ? 1 : i)
      {
        strCur += xmlStrCSPN(strCur + 1, strEnd, XML_CHAR_BRACKETS | XML_CHAR_GT) + 1;
      }

      // Unclosed <!DOCTYPE
      if (strCur == strEnd) __XML_CHAR_ERROR(XmlReaderUnclosedDOCTYPE);
    }


    // ----- <?...?> processing instructions -----
    else if (*strCur == '?')
    {
      CharW* dataStart = ++strCur;

      do {
        strCur = xmlStrCHR(strCur, strEnd, CharW('?'));
      } while (strCur && ++strCur != strEnd && *strCur != '>');

      // Unclosed <?
      if (!strCur)
      {
        __XML_CHAR_ERROR(XmlReaderUnclosedPI);
      }
      else
      {
        const CharW* dataEnd = strCur - 3;
        addPI(StringW(dataStart, (size_t)(dataEnd - dataStart)));
      }
    }


    // ----- Syntax Error -----
    else
    {
      __XML_CHAR_ERROR(EXmlSyntaxError);
    }


    // ----- check for end -----
    if (strCur == strEnd) break;


    // ----- tag character content -----
    {
      const CharW* begin = ++strCur;
      while (strCur != strEnd && *strCur != '<') strCur++;

      if (strCur == strEnd) break;
      if (skipTagText) continue;

      // Remove white spaces, but don't remove &..; spaces
      while (begin->isSpace()) begin++;

      const CharW* tr = strCur-1;
      while (tr > begin && tr->isSpace()) tr--;
      tr++;

      // Decode Xml text (&..;) to text
      if (tr - begin) decode(cur->_text, begin, size_t( tr - begin ));
      skipTagText = true;
    }
  }
  // ---------------------------------------------------------------------

  if (cur)
  {
    if (cur->tag().isEmpty())
      __XML_CHAR_ERROR(XmlReaderMissingRootTag);
    else
      __XML_CHAR_ERROR(XmlReaderMissingTag);
  }

  return err;
#undef __XML_CHAR_ERROR
#endif

#if 0
// Decode xml &entities; into normal unicode text
void XmlSaxReader::decode(StringW& dest, const CharW* buffer, size_t length)
{
  // This will probabbly never happen
  if (length == DetectLength) length = StringW::Raw::len(buffer);

  const CharW* end = buffer + length;
  CharW* p = dest._reserve(length);

__begin:
  while (buffer != end)
  {
    if (*buffer == '&')
    {
      const CharW* begin = ++buffer;

      while (buffer != end)
      {
        if (*buffer == ';')
        {
          CharW uc = XmlEntity::decode(begin, size_t( buffer - begin ));
          buffer++;
          if (!uc.isNull())
          {
            *p++ = uc;
            goto __begin;
          }
          else
            goto __copy;
        }
        buffer++;
      }

      // end of input, parse everything
      core_stderr_msg("XmlSaxReader", "decode", "End of input in entity");
__copy:
      {
        size_t i = size_t( buffer - (--begin) );
        StringW::Raw::copy(p, begin, i);
        p += i;
      }
      if (buffer == end)
        break;
      else
        goto __begin;
    }
    *p++ = *buffer++;
  }

  dest._modified(p);
}

struct CORE_HIDDEN XmlSaxWriter
{
  XmlDocument& _doc;   // link to xml document
  Stream& _stream;     // link to stream
  StringA _target;   // target encoded buffer

  TextCodec _textCodec;
  TextCodecState _textState;

  XmlSaxWriter(XmlDocument& doc, Stream& stream) :
    _doc(doc),
    _stream(stream),
    _textCodec(doc.textCodec())
  {
    // reserve memory
    _target.reserve(1024);

    // setup encoder
    _textState.map = xml_escape_mask;
    _textState.replace = xml_escape_fn;
  }

  void run();
  void writeNodesR(const XmlNode* node, uint depth);
  void writeAttributes(const XmlNode* node);

  inline void writeRaw(const StringW& s) { _textCodec.appendFromUtf32(_target, s); }
  inline void writeStr(const StringW& s) { _textCodec.appendFromUtf32(_target, s, &_textState); }
  inline void writeChar(CharW uc) { _textCodec.appendFromUtf32(_target, &uc, sizeof(Char)); }

  inline void writeSpaces(size_t count)
  {
    size_t i;
    for (i = 0; i != count; i++) writeChar(CharW(' '));
  }

  inline void flush() { _stream.write(_target); _target.clear(); }
};

void XmlSaxWriter::run()
{
  StringW str;
  str.format("<?xml version=\"1.0\" encoding=\"%s\" ?>\n", _textCodec.mime());

  writeRaw(str);
  writeNodesR(_doc.constRoot(), 0);

  flush();
}

void XmlSaxWriter::writeNodesR(const XmlNode* node, uint depth)
{
  const XmlNode* current = node;

  while (current)
  {
    writeSpaces(depth);

    // Make all unnamed tags as 'Unnamed'
    StringW tag;
    if (current->tag().isEmpty())
      tag = "Unnamed";
    else
      tag = current->tag();

    if (current->children())
    {
      // Not self closing tag

      // Write <Tag [Attributes]>
      writeChar(CharW('<'));
      writeStr(tag);
      writeAttributes(current);
      writeChar(CharW('>'));

      // Write Text\n
      writeStr(current->text());
      writeChar(CharW('\n'));

      // Write children
      writeNodesR(current->children(), depth+1);

      // Write </Tag>\n
      writeSpaces(depth);
      writeChar(CharW('<'));
      writeChar(CharW('/'));
      writeStr(tag);
      writeChar(CharW('>'));
      writeChar(CharW('\n'));
    }
    else
    {
      // Self closing tag
      if (current->text().isEmpty())
      {
        // Write <Tag [Attributes] />\n
        writeChar(CharW('<'));
        writeStr(tag);
        writeAttributes(current);
        writeChar(CharW(' '));
        writeChar(CharW('/'));
        writeChar(CharW('>'));
        writeChar(CharW('\n'));
      }
      else
      {
        // Write <Tag [Attributes]>Text</Tag>\n
        writeChar(CharW('<'));
        writeStr(tag);
        writeAttributes(current);
        writeChar(CharW('>'));
        writeStr(current->text());
        writeChar(CharW('<'));
        writeChar(CharW('/'));
        writeStr(tag);
        writeChar(CharW('>'));
        writeChar(CharW('\n'));
      }
    }

    current = current->next();
    if (_target.getLength() > 512) flush();
  }
}

void XmlSaxWriter::writeAttributes(const XmlNode* node)
{
  static const CharW q = CharW('\"');

  const XmlAttribute* current = node->attributes();

  if (!current) return;

  while (current)
  {
    // Write Attribute="Text"
    writeChar(CharW(' '));
    writeStr(current->name());
    writeChar(CharW('='));
    writeChar(q);
    writeStr(current->value());
    writeChar(q);

    current = current->next();
  }
}

Value XmlDocument::writeFile(const StringW& fileName)
{
  Stream stream;
  Value result;

  result = stream.openFile(fileName,
    STREAM_OPEN_WRITE |
    STREAM_OPEN_TRUNCATE |
    STREAM_OPEN_CREATE |
    STREAM_OPEN_CREATE_PATH);

  if (result.ok())
    return writeStream(stream);
  else
    return result;
}

Value XmlDocument::writeStream(Core::Stream& stream)
{
  XmlSaxWriter w(*this, stream);
  w.run();

  return Core::Value();
  //return stream.error();
}

// [Core::]
}
#endif
