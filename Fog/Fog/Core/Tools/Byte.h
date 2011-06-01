// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_BYTE_H
#define _FOG_CORE_TOOLS_BYTE_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Tools/CharUtil.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::Byte]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief Byte (8-bits) character.
struct FOG_NO_EXPORT Byte
{
  // [Construction / Destruction]

  FOG_INLINE Byte() {}
  FOG_INLINE Byte(const Byte&  c) : _ch(c._ch) {}

  explicit FOG_INLINE Byte(char c) : _ch((uint8_t)c) {}
  explicit FOG_INLINE Byte(uint8_t  c) : _ch(c) {}

  // [Character]

  //! @brief Return 8-bit byte value.
  FOG_INLINE uint8_t ch() const { return _ch; }

  // [Operator Overload]

  FOG_INLINE Byte& operator=(const Byte&    ch) { _ch = ch._ch      ; return *this; }
  FOG_INLINE Byte& operator=(const char&    ch) { _ch = (uint8_t )ch; return *this; }
  FOG_INLINE Byte& operator=(const uint8_t& ch) { _ch = ch          ; return *this; }

  FOG_INLINE Byte& operator-=(const Byte&    ch) { _ch -= ch._ch      ; return *this; }
  FOG_INLINE Byte& operator-=(const char&    ch) { _ch -= (uint8_t )ch; return *this; }
  FOG_INLINE Byte& operator-=(const uint8_t& ch) { _ch -= ch          ; return *this; }

  FOG_INLINE Byte& operator+=(const Byte&    ch) { _ch += ch._ch      ; return *this; }
  FOG_INLINE Byte& operator+=(const char&    ch) { _ch += (uint8_t )ch; return *this; }
  FOG_INLINE Byte& operator+=(const uint8_t& ch) { _ch += ch          ; return *this; }

  // [Implicit Conversion]

  FOG_INLINE operator bool() const { return _ch != 0; }
  FOG_INLINE operator char() const { return (char)_ch; }
  FOG_INLINE operator uint8_t() const { return (uint8_t)_ch; }

  // [Ascii ctypes]

  FOG_INLINE bool isAlpha() const { return CharUtil::isAsciiAlpha(_ch); }
  FOG_INLINE bool isLower() const { return CharUtil::isAsciiLower(_ch); }
  FOG_INLINE bool isUpper() const { return CharUtil::isAsciiUpper(_ch); }
  FOG_INLINE Byte toLower() const { return Byte(CharUtil::toAsciiLower(_ch)); }
  FOG_INLINE Byte toUpper() const { return Byte(CharUtil::toAsciiUpper(_ch)); }

  FOG_INLINE bool isSpace() const { return CharUtil::isAsciiSpace(_ch); }
  FOG_INLINE bool isBlank() const { return CharUtil::isAsciiBlank(_ch); }
  FOG_INLINE bool isDigit() const { return CharUtil::isAsciiDigit(_ch); }
  FOG_INLINE bool isAlnum() const { return CharUtil::isAsciiAlnum(_ch); }
  FOG_INLINE bool isXDigit()const { return CharUtil::isAsciiXDigit(_ch);}
  FOG_INLINE bool isPunct() const { return CharUtil::isAsciiPunct(_ch); }
  FOG_INLINE bool isGraph() const { return CharUtil::isAsciiGraph(_ch); }
  FOG_INLINE bool isPrint() const { return CharUtil::isAsciiPrint(_ch); }
  FOG_INLINE bool isCntrl() const { return CharUtil::isAsciiCntrl(_ch); }

  // Statics.
#define _FOG_BYTE_MAKE_STATICS(_Type_) \
  static FOG_INLINE bool isAlpha(_Type_ ch) { return CharUtil::isAsciiAlpha(ch); } \
  static FOG_INLINE bool isLower(_Type_ ch) { return CharUtil::isAsciiLower(ch); } \
  static FOG_INLINE bool isUpper(_Type_ ch) { return CharUtil::isAsciiUpper(ch); } \
  static FOG_INLINE _Type_ toLower(_Type_ ch) { return CharUtil::toAsciiLower(ch); } \
  static FOG_INLINE _Type_ toUpper(_Type_ ch) { return CharUtil::toAsciiUpper(ch); } \
  \
  static FOG_INLINE bool isSpace(_Type_ ch) { return CharUtil::isAsciiSpace(ch); } \
  static FOG_INLINE bool isBlank(_Type_ ch) { return CharUtil::isAsciiBlank(ch); } \
  static FOG_INLINE bool isDigit(_Type_ ch) { return CharUtil::isAsciiDigit(ch); } \
  static FOG_INLINE bool isAlnum(_Type_ ch) { return CharUtil::isAsciiAlnum(ch); } \
  static FOG_INLINE bool isXDigit(_Type_ ch){ return CharUtil::isAsciiXDigit(ch);} \
  static FOG_INLINE bool isPunct(_Type_ ch) { return CharUtil::isAsciiPunct(ch); } \
  static FOG_INLINE bool isGraph(_Type_ ch) { return CharUtil::isAsciiGraph(ch); } \
  static FOG_INLINE bool isPrint(_Type_ ch) { return CharUtil::isAsciiPrint(ch); } \
  static FOG_INLINE bool isCntrl(_Type_ ch) { return CharUtil::isAsciiCntrl(ch); }

  _FOG_BYTE_MAKE_STATICS(char)
  _FOG_BYTE_MAKE_STATICS(uint8_t)
  _FOG_BYTE_MAKE_STATICS(int16_t)
  _FOG_BYTE_MAKE_STATICS(uint16_t)
  _FOG_BYTE_MAKE_STATICS(int32_t)
  _FOG_BYTE_MAKE_STATICS(uint32_t)

#undef _FOG_BYTE_MAKE_STATICS

  // [Utf8 Helpers]

  FOG_INLINE bool isValidUtf8() const { return isValidUtf8(_ch); }
  FOG_INLINE uint8_t utf8Length() const { return utf8Length(_ch); }

  // Statics.
  static FOG_INLINE bool isValidUtf8(char ch) { return (uint8_t)ch < 0x80 && (uint8_t)ch >= 0xC0; }
  static FOG_INLINE bool isValidUtf8(uint8_t ch) { return ch < 0x80 && ch >= 0xC0; }

  static FOG_INLINE uint8_t utf8Length(char ch) { return utf8LengthTable[(uint8_t)ch]; }
  static FOG_INLINE uint8_t utf8Length(uint8_t ch) { return utf8LengthTable[ch]; }

  // [Members]

  //! @brief Unicode character value.
  uint8_t _ch;
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// [Overloads]
#define __FOG_BYTE_MAKE_COMPARE_OVERLOAD(_TypeA_, _GetA_, _TypeB_, _GetB_) \
static FOG_INLINE bool operator==(_TypeA_, _TypeB_) { return _GetA_ == _GetB_; } \
static FOG_INLINE bool operator!=(_TypeA_, _TypeB_) { return _GetA_ != _GetB_; } \
static FOG_INLINE bool operator<=(_TypeA_, _TypeB_) { return _GetA_ <= _GetB_; } \
static FOG_INLINE bool operator>=(_TypeA_, _TypeB_) { return _GetA_ >= _GetB_; } \
static FOG_INLINE bool operator< (_TypeA_, _TypeB_) { return _GetA_ <  _GetB_; } \
static FOG_INLINE bool operator> (_TypeA_, _TypeB_) { return _GetA_ >  _GetB_; }

__FOG_BYTE_MAKE_COMPARE_OVERLOAD(const Fog::Byte& a, a._ch, const Fog::Byte& b, b._ch)

__FOG_BYTE_MAKE_COMPARE_OVERLOAD(char a, (uint8_t)a, const Fog::Byte& b, b._ch)
__FOG_BYTE_MAKE_COMPARE_OVERLOAD(const Fog::Byte& a, a._ch, char b, (uint8_t)b)

__FOG_BYTE_MAKE_COMPARE_OVERLOAD(uint8_t a, a, const Fog::Byte& b, b._ch)
__FOG_BYTE_MAKE_COMPARE_OVERLOAD(const Fog::Byte& a, a._ch, uint8_t b, b)

#undef __FOG_BYTE_MAKE_COMPARE_OVERLOAD

#define __FOG_BYTE_MAKE_ARITH_OVERLOAD(TYPE_A, GET_A, TYPE_B, GET_B) \
static FOG_INLINE Fog::Byte operator+(TYPE_A, TYPE_B) { return Fog::Byte((uint8_t)(GET_A + GET_B)); } \
static FOG_INLINE Fog::Byte operator-(TYPE_A, TYPE_B) { return Fog::Byte((uint8_t)(GET_A - GET_B)); }

__FOG_BYTE_MAKE_ARITH_OVERLOAD(const Fog::Byte& a, a._ch, const Fog::Byte& b, b._ch)
__FOG_BYTE_MAKE_ARITH_OVERLOAD(char a, (uint8_t)a, const Fog::Byte& b, b._ch)
__FOG_BYTE_MAKE_ARITH_OVERLOAD(const Fog::Byte& a, a._ch, char b, (uint8_t)b)
__FOG_BYTE_MAKE_ARITH_OVERLOAD(uint8_t a, a, const Fog::Byte& b, b._ch)
__FOG_BYTE_MAKE_ARITH_OVERLOAD(const Fog::Byte& a, a._ch, uint8_t b, b)

#undef __FOG_BYTE_MAKE_ARITH_OVERLOAD

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::Byte, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_TOOLS_BYTE_H
