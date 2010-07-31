// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CHAR_H
#define _FOG_CORE_CHAR_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/CharUtil.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_String
//! @{

// ============================================================================
// [Fog::Char]
// ============================================================================

#include <Fog/Core/Compiler/PackByte.h>
//! @brief 16-bit unicode character.
struct FOG_HIDDEN Char
{
  // [Construction / Destruction]

  FOG_INLINE Char() {}
  FOG_INLINE Char(const Char& c) : _ch(c._ch) {}

  FOG_INLINE explicit Char(char     c) : _ch((uint8_t)c) {}
  FOG_INLINE explicit Char(uint8_t  c) : _ch(c) {}
  FOG_INLINE explicit Char(int16_t  c) : _ch((uint16_t)c) {}
  FOG_INLINE explicit Char(uint16_t c) : _ch(c) {}
  FOG_INLINE explicit Char(int32_t  c) : _ch((uint16_t)(uint32_t)c) {}
  FOG_INLINE explicit Char(uint32_t c) : _ch((uint16_t)c) {}

  // [Character]

  //! @brief Return 16-bit character value.
  FOG_INLINE uint16_t ch() const { return _ch; }

  // [Char::operator=]

  FOG_INLINE Char& operator=(const char&     ch) { _ch = (uint8_t )ch          ; return *this; }
  FOG_INLINE Char& operator=(const uint8_t&  ch) { _ch = ch                    ; return *this; }
  FOG_INLINE Char& operator=(const int16_t&  ch) { _ch = (uint16_t)ch          ; return *this; }
  FOG_INLINE Char& operator=(const uint16_t& ch) { _ch = ch                    ; return *this; }
  FOG_INLINE Char& operator=(const int32_t&  ch) { _ch = (uint16_t)(uint32_t)ch; return *this; }
  FOG_INLINE Char& operator=(const uint32_t& ch) { _ch = (uint16_t)ch          ; return *this; }
  FOG_INLINE Char& operator=(const Char&     ch) { _ch = ch._ch                ; return *this; }

  // [Implicit Conversion]

  FOG_INLINE operator bool() const { return _ch != 0; }
  FOG_INLINE operator int16_t() const { return (int16_t)_ch; }
  FOG_INLINE operator uint16_t() const { return (uint16_t)_ch; }
  FOG_INLINE operator int32_t() const { return (int32_t)_ch; }
  FOG_INLINE operator uint32_t() const { return (uint32_t)_ch; }

  // [Ascii CTypes]

  FOG_INLINE bool isAsciiAlpha() const { return CharUtil::isAsciiAlpha(_ch); }
  FOG_INLINE bool isAsciiAlnum() const { return CharUtil::isAsciiAlnum(_ch); }
  FOG_INLINE bool isAsciiLower() const { return CharUtil::isAsciiLower(_ch); }
  FOG_INLINE bool isAsciiUpper() const { return CharUtil::isAsciiUpper(_ch); }
  FOG_INLINE bool isAsciiDigit() const { return CharUtil::isAsciiDigit(_ch); }
  FOG_INLINE bool isAsciiXDigit()const { return CharUtil::isAsciiXDigit(_ch); }
  FOG_INLINE bool isAsciiSpace() const { return CharUtil::isAsciiSpace(_ch); }
  FOG_INLINE bool isAsciiBlank() const { return CharUtil::isAsciiSpace(_ch); }
  FOG_INLINE bool isAsciiPunct() const { return CharUtil::isAsciiPunct(_ch); }
  FOG_INLINE bool isAsciiGraph() const { return CharUtil::isAsciiGraph(_ch); }
  FOG_INLINE bool isAsciiPrint() const { return CharUtil::isAsciiPrint(_ch); }

  FOG_INLINE Char toAsciiLower() const { return Char(CharUtil::toAsciiLower(_ch)); }
  FOG_INLINE Char toAsciiUpper() const { return Char(CharUtil::toAsciiUpper(_ch)); }

  // Statics.
  static FOG_INLINE bool isAsciiAlpha(uint16_t ch) { return CharUtil::isAsciiAlpha(ch); }
  static FOG_INLINE bool isAsciiAlnum(uint16_t ch) { return CharUtil::isAsciiAlnum(ch); }
  static FOG_INLINE bool isAsciiLower(uint16_t ch) { return CharUtil::isAsciiLower(ch); }
  static FOG_INLINE bool isAsciiUpper(uint16_t ch) { return CharUtil::isAsciiUpper(ch); }
  static FOG_INLINE bool isAsciiDigit(uint16_t ch) { return CharUtil::isAsciiDigit(ch); }
  static FOG_INLINE bool isAsciiXDigit(uint16_t ch){ return CharUtil::isAsciiXDigit(ch); }
  static FOG_INLINE bool isAsciiSpace(uint16_t ch) { return CharUtil::isAsciiSpace(ch); }
  static FOG_INLINE bool isAsciiBlank(uint16_t ch) { return CharUtil::isAsciiSpace(ch); }
  static FOG_INLINE bool isAsciiPunct(uint16_t ch) { return CharUtil::isAsciiPunct(ch); }
  static FOG_INLINE bool isAsciiGraph(uint16_t ch) { return CharUtil::isAsciiGraph(ch); }
  static FOG_INLINE bool isAsciiPrint(uint16_t ch) { return CharUtil::isAsciiPrint(ch); }

  static FOG_INLINE Char toAsciiLower(uint16_t ch) { return Char(CharUtil::toAsciiLower(ch)); }
  static FOG_INLINE Char toAsciiUpper(uint16_t ch) { return Char(CharUtil::toAsciiUpper(ch)); }

  // [Unicode CTypes]

  FOG_INLINE bool isAlpha() const { return CharUtil::isAlpha(_ch); }
  FOG_INLINE bool isLower() const { return CharUtil::isLower(_ch); }
  FOG_INLINE bool isUpper() const { return CharUtil::isUpper(_ch); }
  FOG_INLINE Char toLower() const { return Char(CharUtil::toLower(_ch)); }
  FOG_INLINE Char toUpper() const { return Char(CharUtil::toUpper(_ch)); }
  FOG_INLINE Char toAscii() const { return Char(CharUtil::toAscii(_ch)); }

  FOG_INLINE bool isSpace() const { return CharUtil::isSpace(_ch); }
  FOG_INLINE bool isBlank() const { return CharUtil::isBlank(_ch); }
  FOG_INLINE bool isDigit() const { return CharUtil::isDigit(_ch); }
  FOG_INLINE bool isAlnum() const { return CharUtil::isAlnum(_ch); }
  FOG_INLINE bool isXDigit() const { return CharUtil::isXDigit(_ch); }
  FOG_INLINE bool isPunct() const { return CharUtil::isPunct(_ch); }
  FOG_INLINE bool isGraph() const { return CharUtil::isGraph(_ch); }
  FOG_INLINE bool isPrint() const { return CharUtil::isPrint(_ch); }
  FOG_INLINE bool isCntrl() const { return CharUtil::isCntrl(_ch); }

  // Statics.
  static FOG_INLINE bool isAlpha(uint16_t ch) { return CharUtil::isAlpha(ch); }
  static FOG_INLINE bool isLower(uint16_t ch) { return CharUtil::isLower(ch); }
  static FOG_INLINE bool isUpper(uint16_t ch) { return CharUtil::isUpper(ch); }
  static FOG_INLINE uint16_t toLower(uint16_t ch) { return CharUtil::toLower(ch); }
  static FOG_INLINE uint16_t toUpper(uint16_t ch) { return CharUtil::toUpper(ch); }
  static FOG_INLINE uint16_t toAscii(uint16_t ch) { return CharUtil::toAscii(ch); }

  static FOG_INLINE bool isSpace(uint16_t ch) { return CharUtil::isSpace(ch); }
  static FOG_INLINE bool isBlank(uint16_t ch) { return CharUtil::isBlank(ch); }
  static FOG_INLINE bool isDigit(uint16_t ch) { return CharUtil::isDigit(ch); }
  static FOG_INLINE bool isAlnum(uint16_t ch) { return CharUtil::isAlnum(ch); }
  static FOG_INLINE bool isXDigit(uint16_t ch) { return CharUtil::isXDigit(ch); }
  static FOG_INLINE bool isPunct(uint16_t ch) { return CharUtil::isPunct(ch); }
  static FOG_INLINE bool isGraph(uint16_t ch) { return CharUtil::isGraph(ch); }
  static FOG_INLINE bool isPrint(uint16_t ch) { return CharUtil::isPrint(ch); }
  static FOG_INLINE bool isCntrl(uint16_t ch) { return CharUtil::isCntrl(ch); }

  // [Unicode Helpers]

  FOG_INLINE bool isLeadSurrogate() const { return isLeadSurrogate(_ch); }
  FOG_INLINE bool isTrailSurrogate() const { return isTrailSurrogate(_ch); }
  FOG_INLINE bool isSurrogatePair() const { return isSurrogatePair(_ch); }
  FOG_INLINE bool isValid() const { return isValid(_ch); }

  // Statics.
  static FOG_INLINE bool isLeadSurrogate(uint16_t ch) { return (ch & UTF16_LEAD_SURROGATE_MASK) == UTF16_LEAD_SURROGATE_BASE; }
  static FOG_INLINE bool isTrailSurrogate(uint16_t ch) { return (ch & UTF16_TRAIL_SURROGATE_MASK) == UTF16_TRAIL_SURROGATE_BASE; }
  static FOG_INLINE bool isSurrogatePair(uint16_t ch) { return (ch & UTF16_SURROGATE_PAIR_MASK) == UTF16_SURROGATE_PAIR_BASE; }
  static FOG_INLINE bool isValid(uint16_t ch) { return (ch < 0xFFFE); }

  // [BOM / BSwap]


  FOG_INLINE bool isBom() const { return _ch == UTF16_BOM; }
  FOG_INLINE bool isBomSwapped() const { return _ch == UTF16_BOM_Swapped; }

  FOG_INLINE Char& bswap() { _ch = Memory::bswap16(_ch); return *this; }

  // Statics.
  static FOG_INLINE bool isBom(uint16_t ch) { return ch == UTF16_BOM; }
  static FOG_INLINE bool isBomSwapped(uint16_t ch) { return ch == UTF16_BOM_Swapped; }

  static FOG_INLINE uint16_t bswap(uint16_t ch) { return Memory::bswap16(ch); }

  // [Combine]

  static FOG_INLINE Char combine(Char ch, Char comb) { return Char((uint16_t)unicodeCombine(ch._ch, comb._ch)); }

  // [Surrogates]

  static FOG_INLINE uint32_t fromSurrogate(uint16_t uc0, uint16_t uc1)
  {
    return (uint32_t)(
      0x10000U + (((uint32_t)uc0 - UTF16_LEAD_SURROGATE_MIN) << 10) +
                 ( (uint32_t)uc1 - (UTF16_LEAD_SURROGATE_MAX + 1U)));
  }

  static FOG_INLINE void toSurrogatePair(uint32_t uc, uint16_t* uc0, uint16_t* uc1)
  {
    FOG_ASSERT(uc >= 0x10000);

    uint32_t ch = uc - 0x10000;
    *uc0 = (uint16_t)((ch >> 10) + 0xD800);
    *uc1 = (uint16_t)((ch & 0x03FF) + 0xDC00);
  }

  // [Members]

  //! @brief 16-bit unicode character value.
  uint16_t _ch;
};
#include <Fog/Core/Compiler/PackRestore.h>

//! @}

} // Fog namespace

// [Overloads]
#define __FOG_CHAR_MAKE_COMPARE_OVERLOAD(TYPE_A, GET_A, TYPE_B, GET_B) \
static FOG_INLINE bool operator==(TYPE_A, TYPE_B) { return GET_A == GET_B; } \
static FOG_INLINE bool operator!=(TYPE_A, TYPE_B) { return GET_A != GET_B; } \
static FOG_INLINE bool operator<=(TYPE_A, TYPE_B) { return GET_A <= GET_B; } \
static FOG_INLINE bool operator>=(TYPE_A, TYPE_B) { return GET_A >= GET_B; } \
static FOG_INLINE bool operator< (TYPE_A, TYPE_B) { return GET_A <  GET_B; } \
static FOG_INLINE bool operator> (TYPE_A, TYPE_B) { return GET_A >  GET_B; }

__FOG_CHAR_MAKE_COMPARE_OVERLOAD(const Fog::Char& a, a._ch, const Fog::Char& b, b._ch)

__FOG_CHAR_MAKE_COMPARE_OVERLOAD(const Fog::Char& a, a._ch, char b, (uint8_t)b)
__FOG_CHAR_MAKE_COMPARE_OVERLOAD(char a, (uint8_t)a, const Fog::Char& b, b._ch)

__FOG_CHAR_MAKE_COMPARE_OVERLOAD(const Fog::Char& a, a._ch, uint8_t b, b)
__FOG_CHAR_MAKE_COMPARE_OVERLOAD(uint8_t a, a, const Fog::Char& b, b._ch)

__FOG_CHAR_MAKE_COMPARE_OVERLOAD(const Fog::Char& a, a._ch, int16_t b, (uint16_t)b)
__FOG_CHAR_MAKE_COMPARE_OVERLOAD(int16_t a, (uint16_t)a, const Fog::Char& b, b._ch)

__FOG_CHAR_MAKE_COMPARE_OVERLOAD(const Fog::Char& a, a._ch, uint16_t b, b)
__FOG_CHAR_MAKE_COMPARE_OVERLOAD(uint16_t a, a, const Fog::Char& b, b._ch)

#undef __FOG_CHAR_MAKE_COMPARE_OVERLOAD

#define __FOG_CHAR_MAKE_ARITH_OVERLOAD(TYPE_A, GET_A, TYPE_B, GET_B) \
static FOG_INLINE Fog::Char operator+(TYPE_A, TYPE_B) { return Fog::Char(GET_A + GET_B); } \
static FOG_INLINE Fog::Char operator-(TYPE_A, TYPE_B) { return Fog::Char(GET_A - GET_B); }

__FOG_CHAR_MAKE_ARITH_OVERLOAD(const Fog::Char& a, a._ch, const Fog::Char& b, b._ch)
__FOG_CHAR_MAKE_ARITH_OVERLOAD(const Fog::Char& a, a._ch, char b, (uint16_t)(uint8_t)b)
__FOG_CHAR_MAKE_ARITH_OVERLOAD(char a, (uint16_t)(uint8_t)a, const Fog::Char& b, b._ch)
__FOG_CHAR_MAKE_ARITH_OVERLOAD(const Fog::Char& a, a._ch, int16_t b, (uint16_t)b)
__FOG_CHAR_MAKE_ARITH_OVERLOAD(int16_t a, (uint16_t)a, const Fog::Char& b, b._ch)

#undef __FOG_CHAR_MAKE_ARITH_OVERLOAD

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Char, Fog::TYPEINFO_PRIMITIVE)

#endif // _FOG_CORE_CHAR_H
