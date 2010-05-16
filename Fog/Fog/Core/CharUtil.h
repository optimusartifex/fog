// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CHARUTIL_H
#define _FOG_CORE_CHARUTIL_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_Essentials
//! @{

// ============================================================================
// [Ascii and Unicode Data]
// ============================================================================

extern FOG_API const uint8_t asciiCTypeData[256];
extern FOG_API const uint8_t asciiCTypeToLowerData[256];
extern FOG_API const uint8_t asciiCTypeToUpperData[256];

extern FOG_API const uint32_t unicodeCTypeData[2048];
extern FOG_API uint32_t unicodeCombine(uint32_t uc, uint32_t comb);

// ============================================================================
// [Utf8 Length Table]
// ============================================================================

//! @brief UTF-8 Length table.
//!
//! From UTF-8 to Unicode UCS-4:
//! Let's take a UTF-8 byte sequence. The first byte in a new sequence will tell
//! us how long the sequence is. Let's call the subsequent decimal bytes z y x w v u.
//! - If z is between and including 0 - 127, then there is 1 byte z.
//!   The decimal Unicode value ud = the value of z.
//! - If z is between and including 192 - 223, then there are 2 bytes z y;
//!   ud = (z-192)*64 + (y-128)
//! - If z is between and including 224 - 239, then there are 3 bytes z y x;
//!   ud = (z-224)*4096 + (y-128)*64 + (x-128)
//! - If z is between and including 240 - 247, then there are 4 bytes z y x w;
//!   ud = (z-240)*262144 + (y-128)*4096 + (x-128)*64 + (w-128)
//! - If z is between and including 248 - 251, then there are 5 bytes z y x w v;
//!   ud = (z-248)*16777216 + (y-128)*262144 + (x-128)*4096 + (w-128)*64 + (v-128)
//! - If z is 252 or 253, then there are 6 bytes z y x w v u;
//!   ud = (z-252)*1073741824 + (y-128)*16777216 + (x-128)*262144 + (w-128)*4096 + (v-128)*64 + (u-128)
//! - If z = 254 or 255 then there is something wrong!
//!
//! The table looks like this:
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//!   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
//!   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
//!   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
//!   4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0
//! - Zeros are invalid UTF-8 characters
extern FOG_API const uint8_t utf8LengthTable[256];

// ============================================================================
// [Unicode Constants]
// ============================================================================

enum UNICODE_CHARS
{
  // UTF-16

  UTF16_BOM = 0xFEFF,
  UTF16_BOM_Swapped = 0xFFFE,

  // Leading (high) surrogates are from 0xD800 - 0xDBFF.

  //! @brief Leading (high) surrogate minimum (0xD800)
  UTF16_LEAD_SURROGATE_MIN = 0xD800U,
  //! @brief Leading (high) surrogate maximum (0xDBFF)
  UTF16_LEAD_SURROGATE_MAX = 0xDBFFU,

  UTF16_LEAD_SURROGATE_BASE = 0xD800U,
  UTF16_LEAD_SURROGATE_MASK = 0xFC00U,

  // Trailing (low) surrogates are from 0xDC00 - 0xDFFF.

  //! @brief Trailing (low) surrogate minimum (0xDC00)
  UTF16_TRAIL_SURROGATE_MIN = 0xDC00U,
  //! @brief Trailing (low) surrogate maximum (0xDFFF)
  UTF16_TRAIL_SURROGATE_MAX = 0xDFFFU,

  UTF16_TRAIL_SURROGATE_MASK = 0xFC00U,
  UTF16_TRAIL_SURROGATE_BASE = 0xDC00U,

  UTF16_SURROGATE_PAIR_BASE = 0xD800U,
  UTF16_SURROGATE_PAIR_MASK = 0xF800U,

  // Offsets.

  UTF16_LEAD_SURROGATE_OFFSET = UTF16_LEAD_SURROGATE_MIN - (0x10000U >> 10),
  UTF16_SURROGATE_OFFSET  = 0x10000U - (UTF16_LEAD_SURROGATE_MIN << 10) - UTF16_TRAIL_SURROGATE_MIN,

  // UTF-32.

  UTF32_BOM = 0x0000FEFF,
  UTF32_BOM_Swapped = 0x0000FEFF,

  // Unicode.

  //! @brief Maximum valid value for a Unicode code point
  UNICODE_LAST = 0x0010FFFFU
};

// ============================================================================
// [Fog::CharUtil]
// ============================================================================

//! @brief CType namespace
struct FOG_HIDDEN CharUtil
{
  // [Ascii CTypes]

  enum ASCII_CTYPE
  {
    //! @brief Mask for all lowercase characters: <code>abcdefghijklmnopqrstuvwxyz</code>.
    ASCII_CTYPE_LOWER = (1 << 0),
    //! @brief Mask for all uppercase characters: <code>ABCDEFGHIJKLMNOPQRSTUVWXYZ</code>.
    ASCII_CTYPE_UPPER = (1 << 1),
    //! @brief Mask for all hex characters except digits: <code>abcdefABCDEF</code>.
    ASCII_CTYPE_HEX = (1 << 2),
    //! @brief Mask for all ascii digits: <code>0123456789</code>
    ASCII_CTYPE_DIGIT = (1 << 3),
    //! @brief Mask for underscore character: <code>_</code>
    ASCII_CTYPE_UNDERSCORE = (1 << 4),
    //! @brief Mask for all whitespaces: <code>space \\n \\r \\t \\f \\v</code>
    ASCII_CTYPE_SPACE = (1 << 5),
    //! @brief Mask for all puncuation characters.
    ASCII_CTYPE_PUNCT = (1 << 6),

    //! @brief Mask for all alpha characters, combines @c AsciiCType_Lower and @c AsciiCType_Upper.
    ASCII_CTYPE_ALPHA = ASCII_CTYPE_LOWER | ASCII_CTYPE_UPPER,
    //! @brief Mask for all alpha and numeric characters, combines @c AsciiCType_MaskAlpha and @c AsciiCType_MaskDigit.
    ASCII_CTYPE_ALNUM = ASCII_CTYPE_ALPHA | ASCII_CTYPE_DIGIT,
    //! @brief Mask for all graphics characters, combines @c AsciiCType_Punct and @c AsciiCType_Alnum.
    ASCII_CTYPE_GRAPH = ASCII_CTYPE_PUNCT | ASCII_CTYPE_ALNUM,
    //! @brief Mask for all hex characters, combines @c AsciiCType_Digit and @c AsciiCType_Hex.
    ASCII_CTYPE_XDIGIT = ASCII_CTYPE_DIGIT | ASCII_CTYPE_HEX
  };

#define __FOG_CTYPE_ASCII_OVERLOAD(type, conditional, use) \
  static FOG_INLINE bool isAsciiAlpha(type ch) { return (conditional) && (asciiCTypeData[use] & ASCII_CTYPE_ALPHA) != 0; } \
  static FOG_INLINE bool isAsciiAlnum(type ch) { return (conditional) && (asciiCTypeData[use] & ASCII_CTYPE_ALNUM) != 0; } \
  static FOG_INLINE bool isAsciiLower(type ch) { return (conditional) && (asciiCTypeData[use] & ASCII_CTYPE_LOWER) != 0; } \
  static FOG_INLINE bool isAsciiUpper(type ch) { return (conditional) && (asciiCTypeData[use] & ASCII_CTYPE_UPPER) != 0; } \
  static FOG_INLINE bool isAsciiDigit(type ch) { return (conditional) && (asciiCTypeData[use] & ASCII_CTYPE_DIGIT) != 0; } \
  static FOG_INLINE bool isAsciiXDigit(type ch){ return (conditional) && (asciiCTypeData[use] & ASCII_CTYPE_XDIGIT) != 0; } \
  static FOG_INLINE bool isAsciiSpace(type ch) { return (conditional) && (asciiCTypeData[use] & ASCII_CTYPE_SPACE) != 0; } \
  static FOG_INLINE bool isAsciiBlank(type ch) { return (conditional) && (ch == ' ' || ch == '\t'); } \
  static FOG_INLINE bool isAsciiPunct(type ch) { return (conditional) && (asciiCTypeData[use] & ASCII_CTYPE_PUNCT) != 0; } \
  static FOG_INLINE bool isAsciiGraph(type ch) { return (conditional) && (asciiCTypeData[use] & ASCII_CTYPE_GRAPH) != 0; } \
  static FOG_INLINE bool isAsciiPrint(type ch) { return (conditional) && ((asciiCTypeData[use] & ASCII_CTYPE_GRAPH) != 0 || ch == ' '); } \
  static FOG_INLINE bool isAsciiCntrl(type ch) { return !isAsciiPrint(ch); } \
  static FOG_INLINE type toAsciiLower(type ch) { return (conditional) ? (type)asciiCTypeToLowerData[use] : ch; } \
  static FOG_INLINE type toAsciiUpper(type ch) { return (conditional) ? (type)asciiCTypeToUpperData[use] : ch; }

  __FOG_CTYPE_ASCII_OVERLOAD(char, true, (uint8_t)(ch))
  __FOG_CTYPE_ASCII_OVERLOAD(uint8_t, true, ch)
  __FOG_CTYPE_ASCII_OVERLOAD(int16_t, (uint32_t)(ch) < 256 , (uint8_t)(ch))
  __FOG_CTYPE_ASCII_OVERLOAD(uint16_t, (uint32_t)(ch) < 256 , (uint8_t)(ch))
  __FOG_CTYPE_ASCII_OVERLOAD(int32_t, (uint32_t)(ch) < 256 , (uint8_t)(ch))
  __FOG_CTYPE_ASCII_OVERLOAD(uint32_t, (uint32_t)(ch) < 256 , (uint8_t)(ch))
#undef __FOG_CTYPE_ASCII_OVERLOAD

  // [Unicode CTypes]

#define __FOG_CTYPE_UNICODE_OVERLOAD(type, conditional, use) \
  static FOG_INLINE bool isAlpha(type ch) { return ch < 2048 ? (unicodeCTypeData[ch] & 0xC0000000) != 0 : false; } \
  static FOG_INLINE bool isLower(type ch) { return ch < 2048 ? (unicodeCTypeData[ch] & 0x80000000) != 0 : false; } \
  static FOG_INLINE bool isUpper(type ch) { return ch < 2048 ? (unicodeCTypeData[ch] & 0x40000000) != 0 : false; } \
  static FOG_INLINE type toLower(type ch) { return type(ch < 2048 ? (unicodeCTypeData[ch]      ) & 2047 : ch); } \
  static FOG_INLINE type toUpper(type ch) { return type(ch < 2048 ? (unicodeCTypeData[ch] >> 11) & 2047 : ch); } \
  static FOG_INLINE type toAscii(type ch) { return type(ch < 2048 ? (unicodeCTypeData[ch] >> 22) & 0x7F : 0 ); } \
  \
  static FOG_INLINE bool isSpace(type ch) { return (isAsciiSpace(ch)); } \
  static FOG_INLINE bool isBlank(type ch) { return (isAsciiBlank(ch)); } \
  static FOG_INLINE bool isDigit(type ch) { return (isAsciiDigit(ch)); } \
  static FOG_INLINE bool isAlnum(type ch) { return (isAlpha(ch) || isDigit(ch)); } \
  static FOG_INLINE bool isXDigit(type ch){ return (isAsciiXDigit(ch)); } \
  static FOG_INLINE bool isPunct(type ch) { return (ch >= type(' ') && !isAlnum(ch)); } \
  static FOG_INLINE bool isGraph(type ch) { return (isAlnum(ch) || isPunct(ch)); } \
  static FOG_INLINE bool isPrint(type ch) { return (isGraph(ch) || isSpace(ch)); } \
  static FOG_INLINE bool isCntrl(type ch) { return !isPrint(ch); }

  __FOG_CTYPE_UNICODE_OVERLOAD(uint16_t, (ch < 2048), ch)
  __FOG_CTYPE_UNICODE_OVERLOAD(uint32_t, (ch < 2048), ch)
#undef __FOG_CTYPE_UNICODE_OVERLOAD
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_CHARUTIL_H
