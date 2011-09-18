// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_TEXTCHUNK_H
#define _FOG_G2D_TEXT_TEXTCHUNK_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Range.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::TextChunk]
// ============================================================================

struct FOG_NO_EXPORT TextChunk
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE TextChunk() :
    _text(),
    _range(0, 0)
  {
  }

  explicit FOG_INLINE TextChunk(const StringW& text) :
    _text(text),
    _range(0, text.getLength())
  {
  }

  FOG_INLINE TextChunk(const StringW& text, const Range& range) :
    _text(text),
    _range(range)
  {
    _normalizeRange();
  }

  FOG_INLINE ~TextChunk()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getText() const { return _text; }
  FOG_INLINE const Range& getRange() const { return _range; }

  FOG_INLINE void setText(const StringW& text)
  {
    _text = text;
    _range.setRange(0, text.getLength());
  }

  FOG_INLINE void setText(const StringW& text, const Range& range)
  {
    _text = text;
    _range = range;

    _normalizeRange();
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _text.reset();
    _range.setRange(0, 0);
  }

  // --------------------------------------------------------------------------
  // [Normalize]
  // --------------------------------------------------------------------------

  FOG_INLINE void _normalizeRange()
  {
    size_t length = _text.getLength();

    _range._start = Math::min(length, _range._start);
    _range._end   = Math::min(length, _range._end  );

    if (_range._start > _range._end) _range.setRange(0, 0);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TextChunk& operator=(const TextChunk& other)
  {
    _text = other._text;
    _range = other._range;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief StringW where the span is.
  StringW _text;
  //! @brief Range within the string.
  Range _range;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPE_DECLARE(Fog::TextChunk, Fog::TYPE_CATEGORY_MOVABLE)

// [Guard]
#endif // _FOG_G2D_TEXT_TEXTCHUNK_H
