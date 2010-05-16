// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_TEXTLAYOUT_H
#define _FOG_GRAPHICS_TEXTLAYOUT_H

// [Dependencies]
#include <Fog/Core/Hash.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Constants.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Text
//! @{

// ============================================================================
// [Fog::TextCaps]
// ============================================================================

struct TextCaps
{
};

// ============================================================================
// [Fog::TextExtents]
// ============================================================================

//! @brief Text width retrieved by @c Fog::Font::getTextExtents() like functions.
struct TextExtents
{
  int beginWidth;
  int advance;
  int endWidth;
};

// ============================================================================
// [Fog::TextLayout]
// ============================================================================

struct FOG_API TextLayout
{
  TextLayout();
  TextLayout(const String& text);
  TextLayout(const TextLayout& other);
  ~TextLayout();

  void reset();

  FOG_INLINE const String& getText() const { return _text; }
  FOG_INLINE bool isMultiLine() const { return _multiLine; }

  err_t setText(const String& text);

  TextLayout& operator=(const TextLayout& other);

private:
  err_t _update();

  String _text;
  List<sysuint_t> _lines;
  bool _multiLine;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_TEXTLAYOUT_H
