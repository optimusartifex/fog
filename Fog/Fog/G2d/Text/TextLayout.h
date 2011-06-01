// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_TEXTLAYOUT_H
#define _FOG_G2D_TEXT_TEXTLAYOUT_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/TextChunk.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Painting/PaintDeviceInfo.h>
#include <Fog/G2d/Text/Font.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::TextLayout]
// ============================================================================

//! @brief Simple text-layout class.
struct FOG_API TextLayout
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextLayout();
  TextLayout(const TextLayout& other);
  ~TextLayout();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PaintDeviceInfo& getDeviceInfo() const { return _deviceInfo; }
  FOG_INLINE const Font& getFont() const { return _font; }
  FOG_INLINE const Font& getPhysicalFont() const { return _physicalFont; }

  err_t setDeviceInfo(const PaintDeviceInfo& deviceInfo);
  err_t setDeviceInfoAndFont(const PaintDeviceInfo& deviceInfo, const Font& font);
  err_t setFont(const Font& font);

  FOG_INLINE const TextChunk& getText() const { return _textChunk; }
  err_t setText(const TextChunk& textChunk);
  err_t setText(const String& textString);

  FOG_INLINE const List<Range>& getLines() { return _lines; }
  FOG_INLINE bool isMultiLine() const { return _isMultiLine; }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  void clear();
  void reset();

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  err_t _updatePhysicalFont();
  err_t _updateLayout();

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  TextLayout& operator=(const TextLayout& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Paint device information.
  PaintDeviceInfo _deviceInfo;
  //! @brief The input font used to layout the text.
  Font _font;
  //! @brief The input font converted to a physical font (using @c PaintDeviceInfo).
  Font _physicalFont;

  //! @brief Text used by te layout.
  TextChunk _textChunk;

  List<Range> _lines;
  bool _isMultiLine;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::TextLayout, Fog::TYPEINFO_MOVABLE)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_DECLARE(Fog::TextLayout,
{
  swap(a._deviceInfo, b._deviceInfo);
  swap(a._font, b._font);
  swap(a._physicalFont, b._physicalFont);
  swap(a._textChunk, b._textChunk);
  swap(a._lines, b._lines);
  swap(a._isMultiLine, b._isMultiLine);
})

// [Guard]
#endif // _FOG_G2D_TEXT_TEXTLAYOUT_H
