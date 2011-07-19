// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_BYTE_H
#define _FOG_CORE_TOOLS_BYTE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/CharData.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::Byte]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief Byte.
struct FOG_NO_EXPORT Byte
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  // Template support.
  typedef char PType;
  typedef unsigned char UType;
  typedef unsigned char Value;
  typedef Stub8 Stub;
  typedef ByteArray Sequence;
  typedef ByteArrayFilter Filter;
  typedef ByteArrayMatcher Matcher;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Byte() {}
  FOG_INLINE Byte(const Byte& c) : _value(c._value) {}

  explicit FOG_INLINE Byte(_Uninitialized) {}
#if defined(FOG_CC_HAVE_STANDARD_CHAR_TYPE)
  explicit FOG_INLINE Byte(char c)          : _value((uint8_t)c) {}
#endif // FOG_CC_HAVE_STANDARD_CHAR_TYPE
  explicit FOG_INLINE Byte(signed char c)   : _value((uint8_t)c) {}
  explicit FOG_INLINE Byte(unsigned char c) : _value((uint8_t)c) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the byte value.
  FOG_INLINE uint8_t getValue() const { return _value; }
  //! @brief Set the byte value.
  FOG_INLINE void setValue(uint8_t value) { _value = value; }

  //! @brief Get the byte value converted to a signed integer.
  FOG_INLINE int getInt() const
  {
    return (int)_value;
  }

  //! @brief Set the byte value from an integer.
  FOG_INLINE void setInt(int value)
  {
    FOG_ASSERT(value >= 0 && value <= 0xFF);
    _value = (uint8_t)value;
  }

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isNull() const { return _value == 0; }
  FOG_INLINE bool isAt(uint8_t start, uint8_t end) const { return _value >= start && _value <= end; }

  // --------------------------------------------------------------------------
  // [ASCII Properties]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isAsciiBlank() const { return isAsciiBlank(_value); }
  FOG_INLINE bool isAsciiCntrl() const { return isAsciiCntrl(_value); }
  FOG_INLINE bool isAsciiDigit() const { return isAsciiDigit(_value); }
  FOG_INLINE bool isAsciiGraph() const { return isAsciiGraph(_value); }
  FOG_INLINE bool isAsciiLetter() const { return isAsciiLetter(_value); }
  FOG_INLINE bool isAsciiLetter_() const { return isAsciiLetter_(_value); }
  FOG_INLINE bool isAsciiLower() const { return isAsciiLower(_value); }
  FOG_INLINE bool isAsciiNumlet() const { return isAsciiNumlet(_value); }
  FOG_INLINE bool isAsciiPrint() const { return isAsciiPrint(_value); }
  FOG_INLINE bool isAsciiPunct() const { return isAsciiPunct(_value); }
  FOG_INLINE bool isAsciiSpace() const { return isAsciiSpace(_value); }
  FOG_INLINE bool isAsciiUpper() const { return isAsciiUpper(_value); }
  FOG_INLINE bool isAsciiXDigit() const { return isAsciiXDigit(_value); }
  FOG_INLINE bool isAsciiXLetter() const { return isAsciiXLetter(_value); }

  FOG_INLINE Byte toAsciiLower() const { return Byte(toAsciiLower(_value)); }
  FOG_INLINE Byte toAsciiUpper() const { return Byte(toAsciiUpper(_value)); }

  // Ascii properties are mapped to generic properties when using Byte.
  FOG_INLINE bool isLetter() const { return isLetter(_value); }
  FOG_INLINE bool isLower() const { return isLower(_value); }
  FOG_INLINE bool isNumlet() const { return isNumlet(_value); }
  FOG_INLINE bool isSpace() const { return isSpace(_value); }
  FOG_INLINE bool isUpper() const { return isUpper(_value); }

  FOG_INLINE Byte toLower() const { return Byte(toLower(_value)); }
  FOG_INLINE Byte toUpper() const { return Byte(toUpper(_value)); }

  // --------------------------------------------------------------------------
  // [Equality / Comparison]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Byte& other) { return _value == other._value; }
  FOG_INLINE int compare(const Byte& other) { return (int)_value - (int)other._value; }
  
  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Byte& operator =(const Byte& c) { _value  = c._value; return *this; }
  FOG_INLINE Byte& operator-=(const Byte& c) { _value -= c._value; return *this; }
  FOG_INLINE Byte& operator+=(const Byte& c) { _value += c._value; return *this; }

#if defined(FOG_CC_HAVE_STANDARD_CHAR_TYPE)
  FOG_INLINE Byte& operator =(const char& c) { _value  = (uint8_t)c; return *this; }
  FOG_INLINE Byte& operator-=(const char& c) { _value -= (uint8_t)c; return *this; }
  FOG_INLINE Byte& operator+=(const char& c) { _value += (uint8_t)c; return *this; }
#endif // FOG_CC_HAVE_STANDARD_CHAR_TYPE

  FOG_INLINE Byte& operator =(const signed char& c) { _value  = (uint8_t)c; return *this; }
  FOG_INLINE Byte& operator-=(const signed char& c) { _value -= (uint8_t)c; return *this; }
  FOG_INLINE Byte& operator+=(const signed char& c) { _value += (uint8_t)c; return *this; }

  FOG_INLINE Byte& operator =(const unsigned char& c) { _value  = c; return *this; }
  FOG_INLINE Byte& operator-=(const unsigned char& c) { _value -= c; return *this; }
  FOG_INLINE Byte& operator+=(const unsigned char& c) { _value += c; return *this; }
  
  FOG_INLINE bool operator==(const Byte& c) { return _value == c._value; }
  FOG_INLINE bool operator!=(const Byte& c) { return _value != c._value; }
  FOG_INLINE bool operator>=(const Byte& c) { return _value >= c._value; }
  FOG_INLINE bool operator> (const Byte& c) { return _value >  c._value; }
  FOG_INLINE bool operator<=(const Byte& c) { return _value <= c._value; }
  FOG_INLINE bool operator< (const Byte& c) { return _value <  c._value; }

#if defined(FOG_CC_HAVE_STANDARD_CHAR_TYPE)
  FOG_INLINE bool operator==(const char& c) { return _value == (uint8_t)c; }
  FOG_INLINE bool operator!=(const char& c) { return _value != (uint8_t)c; }
  FOG_INLINE bool operator>=(const char& c) { return _value >= (uint8_t)c; }
  FOG_INLINE bool operator> (const char& c) { return _value >  (uint8_t)c; }
  FOG_INLINE bool operator<=(const char& c) { return _value <= (uint8_t)c; }
  FOG_INLINE bool operator< (const char& c) { return _value <  (uint8_t)c; }
#endif // FOG_CC_HAVE_STANDARD_CHAR_TYPE

  FOG_INLINE bool operator==(const signed char& c) { return _value == (uint8_t)c; }
  FOG_INLINE bool operator!=(const signed char& c) { return _value != (uint8_t)c; }
  FOG_INLINE bool operator>=(const signed char& c) { return _value >= (uint8_t)c; }
  FOG_INLINE bool operator> (const signed char& c) { return _value >  (uint8_t)c; }
  FOG_INLINE bool operator<=(const signed char& c) { return _value <= (uint8_t)c; }
  FOG_INLINE bool operator< (const signed char& c) { return _value <  (uint8_t)c; }

  FOG_INLINE bool operator==(const unsigned char& c) { return _value == (uint8_t)c; }
  FOG_INLINE bool operator!=(const unsigned char& c) { return _value != (uint8_t)c; }
  FOG_INLINE bool operator>=(const unsigned char& c) { return _value >= (uint8_t)c; }
  FOG_INLINE bool operator> (const unsigned char& c) { return _value >  (uint8_t)c; }
  FOG_INLINE bool operator<=(const unsigned char& c) { return _value <= (uint8_t)c; }
  FOG_INLINE bool operator< (const unsigned char& c) { return _value <  (uint8_t)c; }

  // --------------------------------------------------------------------------
  // [Implicit Conversion]
  // --------------------------------------------------------------------------

  FOG_INLINE operator bool() const { return _value != 0; }

#if defined(FOG_CC_HAVE_STANDARD_CHAR_TYPE)
  FOG_INLINE operator char() const { return (char)_value; }
#endif // FOG_CC_HAVE_STANDARD_CHAR_TYPE
  FOG_INLINE operator signed char() const { return (signed char)_value; }
  FOG_INLINE operator unsigned char() const { return (unsigned char)_value; }

  FOG_INLINE operator short() const { return _value; }
  FOG_INLINE operator ushort() const { return _value; }

  FOG_INLINE operator int() const { return _value; }
  FOG_INLINE operator uint() const { return _value; }

  FOG_INLINE operator long() const { return _value; }
  FOG_INLINE operator ulong() const { return _value; }

  FOG_INLINE operator int64_t() const { return _value; }
  FOG_INLINE operator uint64_t() const { return _value; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool isNull(uint8_t c) { return c == 0; }

  static FOG_INLINE bool isAsciiBlank(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_BLANK) != 0; }
  static FOG_INLINE bool isAsciiCntrl(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_PRINT) == 0; }
  static FOG_INLINE bool isAsciiDigit(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_DIGIT) != 0; }
  static FOG_INLINE bool isAsciiGraph(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_GRAPH) != 0; }
  static FOG_INLINE bool isAsciiLetter(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_ALPHA) != 0; }
  static FOG_INLINE bool isAsciiLetter_(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_ALPHA_) != 0; }
  static FOG_INLINE bool isAsciiLower(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_LOWER) != 0; }
  static FOG_INLINE bool isAsciiNumlet(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_ALNUM) != 0; }
  static FOG_INLINE bool isAsciiPrint(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_PRINT) != 0; }
  static FOG_INLINE bool isAsciiPunct(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_PUNCT) != 0; }
  static FOG_INLINE bool isAsciiSpace(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_SPACE) != 0; }
  static FOG_INLINE bool isAsciiUpper(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_UPPER) != 0; }
  static FOG_INLINE bool isAsciiXDigit(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_XDIGIT) != 0; }
  static FOG_INLINE bool isAsciiXLetter(uint8_t c) { return (_charData.asciiCType[c] & ASCII_CLASS_XALPHA) != 0; }

  static FOG_INLINE uint8_t toAsciiLower(uint8_t c) { return _charData.asciiToLower[c]; }
  static FOG_INLINE uint8_t toAsciiUpper(uint8_t c) { return _charData.asciiToUpper[c]; }

  // Ascii properties are mapped to generic properties when using Byte.
  static FOG_INLINE bool isLetter(uint8_t c) { return isAsciiLetter(c); }
  static FOG_INLINE bool isLower(uint8_t c) { return isAsciiLower(c); }
  static FOG_INLINE bool isNumlet(uint8_t c) { return isAsciiNumlet(c); }
  static FOG_INLINE bool isSpace(uint8_t c) { return isAsciiSpace(c); }
  static FOG_INLINE bool isUpper(uint8_t c) { return isAsciiUpper(c); }

  static FOG_INLINE uint8_t toLower(uint8_t c) { return toAsciiLower(c); }
  static FOG_INLINE uint8_t toUpper(uint8_t c) { return toAsciiUpper(c); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Byte value.
  uint8_t _value;
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::Byte, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_TOOLS_BYTE_H
