// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/TextChunk.h>
#include <Fog/Core/Tools/TextIterator.h>
#include <Fog/G2d/Text/TextLayout.h>

namespace Fog {

// ============================================================================
// [Fog::TextLayout - Construction / Destruction]
// ============================================================================

TextLayout::TextLayout() :
  _deviceInfo(),
  _font(),
  _textChunk(),
  _isMultiLine(false)
{
}

TextLayout::TextLayout(const TextLayout& other) :
  _deviceInfo(other._deviceInfo),
  _font(other._font),
  _textChunk(other._textChunk),
  _lines(other._lines),
  _isMultiLine(other._isMultiLine)
{
}

TextLayout::~TextLayout()
{
}

// ============================================================================
// [Fog::TextLayout - Accessors]
// ============================================================================

err_t TextLayout::setDeviceInfo(const PaintDeviceInfo& deviceInfo)
{
  _deviceInfo = deviceInfo;
  _updatePhysicalFont();
  return _updateLayout();
}

err_t TextLayout::setDeviceInfoAndFont(const PaintDeviceInfo& deviceInfo, const Font& font)
{
  _deviceInfo = deviceInfo;
  _font = font;
  _updatePhysicalFont();
  return _updateLayout();
}

err_t TextLayout::setFont(const Font& font)
{
  _font = font;
  _updatePhysicalFont();
  return _updateLayout();
}

err_t TextLayout::setText(const TextChunk& textChunk)
{
  _textChunk = textChunk;
  return _updateLayout();
}

err_t TextLayout::setText(const StringW& textString)
{
  _textChunk.setText(textString);
  return _updateLayout();
}

// ============================================================================
// [Fog::TextLayout - Clear / Reset]
// ============================================================================

void TextLayout::clear()
{
  _font.reset();
  _textChunk.reset();
  _lines.clear();
  _isMultiLine = false;
}

void TextLayout::reset()
{
  _font.reset();
  _textChunk.reset();
  _lines.reset();
  _isMultiLine = false;
}

// ============================================================================
// [Fog::TextLayout - Update]
// ============================================================================

err_t TextLayout::_updatePhysicalFont()
{
  if (_font.isPhysical())
  {
    _physicalFont = _font;
    return ERR_OK;
  }
  else
  {
    return _deviceInfo.makePhysicalFont(_physicalFont, _font);
  }
}

err_t TextLayout::_updateLayout()
{
  err_t err = ERR_OK;
  Range range = _textChunk.getRange();

  const CharW* beg = _textChunk.getText().getData();
  const CharW* cur = beg + range.getStart();
  const CharW* end = beg + range.getEnd();

  const CharW* startLine = cur;
  _lines.clear();

  do {
    CharW c = cur[0];
    cur++;

    if (c.getValue() < 32)
    {
      const CharW* endLine = NULL;

      if (c == CharW('\n'))
      {
        // Eat \n[\r].
        endLine = cur;
        if (cur != end && cur[1] == CharW('\r')) cur++;
      }
      else if (c == CharW('\r'))
      {
        // Eat \r[\n].
        endLine = cur;
        if (cur != end && cur[1] == CharW('\n')) cur++;
      }

      if (endLine)
      {
        range.setRange((size_t)(startLine - beg),
                       (size_t)(endLine   - beg));

        err = _lines.append(range);
        if (FOG_IS_ERROR(err)) goto _End;
      }

      startLine = cur;
    }
  } while (cur != end);

  // Add the last line (without \n or \r). If there is only this line then
  // _lines will contain only one element and _isMultiLine will be set to
  // false.
  range.setRange((size_t)(startLine - beg),
                 (size_t)(cur       - beg));

  err = _lines.append(range);
  if (FOG_IS_ERROR(err)) goto _End;

  _isMultiLine = _lines.getLength() > 1;

_End:
  if (FOG_IS_ERROR(err))
  {
    _lines.clear();
    _isMultiLine = false;
  }

  return err;
}

// ============================================================================
// [Fog::TextLayout - Operator Overload]
// ============================================================================

TextLayout& TextLayout::operator=(const TextLayout& other)
{
  _font = other._font;
  _textChunk = other._textChunk;
  _lines = other._lines;
  _isMultiLine = other._isMultiLine;

  return *this;
}

} // Fog namespace
