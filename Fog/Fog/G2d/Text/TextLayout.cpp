// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Text/TextLayout.h>

namespace Fog {

TextLayout::TextLayout()
{
}

TextLayout::TextLayout(const String& text) :
  _text(text),
  _multiLine(false)
{
  _update();
}

TextLayout::TextLayout(const TextLayout& other) :
  _text(other._text),
  _lines(other._lines),
  _multiLine(other._multiLine)
{

}

TextLayout::~TextLayout()
{
}

void TextLayout::reset()
{
  _text.clear();
  _lines.clear();
  _multiLine = false;
}

err_t TextLayout::setText(const String& text)
{
  err_t err;
  if ((err = _text.set(text))) return err;
  if ((err = _update())) return err;

  return ERR_OK;
}

TextLayout& TextLayout::operator=(const TextLayout& other)
{
  _text = other._text;
  _lines = other._lines;
  _multiLine = other._multiLine;

  return *this;
}

err_t TextLayout::_update()
{
  err_t err = ERR_OK;

  const Char* beg = _text.getData();
  const Char* cur = beg;
  const Char* end = cur + _text.getLength();

  bool multiLine = false;

  _lines.clear();

  do {
    Char c = *cur++;
    if (c == Char('\n'))
    {
      // If this is first row separator append also first line.
      if (multiLine == false)
      {
        _lines.append(0);
        multiLine = true;

        // Discard '\r'.
        if (cur != end && *cur == Char('\r')) cur++;
      }
      if ((err = _lines.append((sysuint_t)(cur - beg)))) goto _End;
    }
  } while (cur != end);

  _multiLine = multiLine;

_End:
  if (FOG_IS_ERROR(err))
  {
    _lines.clear();
    _multiLine = false;
  }

  return err;
}

} // Fog namespace
