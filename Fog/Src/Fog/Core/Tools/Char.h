// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_CHAR_H
#define _FOG_CORE_TOOLS_CHAR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Tools/CharData.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::CharA]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
//! @brief Char (ansi).
struct FOG_NO_EXPORT CharA
{
  //! @brief Type used by Fog-Framework in public API.
  typedef char Type;
  //! @brief Value of CharA.
  typedef unsigned char Value;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE CharA() {}
  FOG_INLINE CharA(const CharA& c) : _value(c._value) {}

  explicit FOG_INLINE CharA(_Uninitialized) {}
#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
  explicit FOG_INLINE CharA(char c) : _value((uint8_t)c) {}
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE
  explicit FOG_INLINE CharA(signed char c) : _value((uint8_t)c) {}
  explicit FOG_INLINE CharA(unsigned char c) : _value((uint8_t)c) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the byte value.
  FOG_INLINE uint8_t getValue() const
  {
    return _value;
  }

  //! @brief Set the byte value.
  FOG_INLINE void setValue(uint8_t value)
  {
    _value = value;
  }

  //! @brief Get the byte value converted to a signed integer.
  FOG_INLINE int getInt() const
  {
    return (int)_value;
  }

  //! @brief Set the byte value from an integer.
  FOG_INLINE void setInt(int value)
  {
    FOG_ASSUME(value >= 0 && value <= 0xFF);
    _value = (uint8_t)value;
  }

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isNull() const { return _value == 0; }
  FOG_INLINE bool isAt(uint8_t start, uint8_t end) const { return (_value >= start) & (_value <= end); }

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

  FOG_INLINE CharA toAsciiLower() const { return CharA(toAsciiLower(_value)); }
  FOG_INLINE CharA toAsciiUpper() const { return CharA(toAsciiUpper(_value)); }

  // --------------------------------------------------------------------------
  // [Char Properties]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isLetter() const { return isLetter(_value); }
  FOG_INLINE bool isLower() const { return isLower(_value); }
  FOG_INLINE bool isNumlet() const { return isNumlet(_value); }
  FOG_INLINE bool isSpace() const { return isSpace(_value); }
  FOG_INLINE bool isUpper() const { return isUpper(_value); }

  FOG_INLINE CharA toLower() const { return CharA(toLower(_value)); }
  FOG_INLINE CharA toUpper() const { return CharA(toUpper(_value)); }

  // --------------------------------------------------------------------------
  // [Equality / Comparison]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const CharA& other) { return _value == other._value; }
  FOG_INLINE int compare(const CharA& other) { return (int)_value - (int)other._value; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE CharA& operator =(const CharA& c) { _value  = c._value; return *this; }
  FOG_INLINE CharA& operator-=(const CharA& c) { _value -= c._value; return *this; }
  FOG_INLINE CharA& operator+=(const CharA& c) { _value += c._value; return *this; }

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
  FOG_INLINE CharA& operator =(const char& c) { _value  = (uint8_t)c; return *this; }
  FOG_INLINE CharA& operator-=(const char& c) { _value -= (uint8_t)c; return *this; }
  FOG_INLINE CharA& operator+=(const char& c) { _value += (uint8_t)c; return *this; }
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE

  FOG_INLINE CharA& operator =(const signed char& c) { _value  = (uint8_t)c; return *this; }
  FOG_INLINE CharA& operator-=(const signed char& c) { _value -= (uint8_t)c; return *this; }
  FOG_INLINE CharA& operator+=(const signed char& c) { _value += (uint8_t)c; return *this; }

  FOG_INLINE CharA& operator =(const unsigned char& c) { _value  = c; return *this; }
  FOG_INLINE CharA& operator-=(const unsigned char& c) { _value -= c; return *this; }
  FOG_INLINE CharA& operator+=(const unsigned char& c) { _value += c; return *this; }

  FOG_INLINE bool operator==(const CharA& c) { return _value == c._value; }
  FOG_INLINE bool operator!=(const CharA& c) { return _value != c._value; }
  FOG_INLINE bool operator>=(const CharA& c) { return _value >= c._value; }
  FOG_INLINE bool operator> (const CharA& c) { return _value >  c._value; }
  FOG_INLINE bool operator<=(const CharA& c) { return _value <= c._value; }
  FOG_INLINE bool operator< (const CharA& c) { return _value <  c._value; }

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
  FOG_INLINE bool operator==(const char& c) { return _value == (uint8_t)c; }
  FOG_INLINE bool operator!=(const char& c) { return _value != (uint8_t)c; }
  FOG_INLINE bool operator>=(const char& c) { return _value >= (uint8_t)c; }
  FOG_INLINE bool operator> (const char& c) { return _value >  (uint8_t)c; }
  FOG_INLINE bool operator<=(const char& c) { return _value <= (uint8_t)c; }
  FOG_INLINE bool operator< (const char& c) { return _value <  (uint8_t)c; }
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE

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

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
  FOG_INLINE operator char() const { return (char)_value; }
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE
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
  // [Statics - Common]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool isNull(uint8_t c) { return c == 0; }
  static FOG_INLINE bool isAt(uint8_t c, uint8_t start, uint8_t end) { return (c >= start) & (c <= end); }

  // --------------------------------------------------------------------------
  // [Statics - Ascii]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [Statics - Char]
  // --------------------------------------------------------------------------

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

  //! @brief CharA value.
  uint8_t _value;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::CharW]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
//! @brief Char (unicode).
struct FOG_NO_EXPORT CharW
{
  //! @brief Type used by Fog-Framework in public API.
  typedef uint16_t Type;
  //! @brief Value of CharW.
  typedef uint16_t Value;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE CharW() {}
  FOG_INLINE CharW(const CharW& c) : _value(c._value) {}

  explicit FOG_INLINE CharW(const CharA& ch) : _value(ch._value) {}

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
  explicit FOG_INLINE CharW(char c) : _value((unsigned char)c) {}
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE

  explicit FOG_INLINE CharW(signed char c) : _value((unsigned char)c) {}
  explicit FOG_INLINE CharW(unsigned char c) : _value(c) {}

  explicit FOG_INLINE CharW(signed short c) : _value((unsigned short)c) {}
  explicit FOG_INLINE CharW(unsigned short c) : _value(c) {}

  explicit FOG_INLINE CharW(signed int c) : _value((uint16_t)(unsigned int)c) {}
  explicit FOG_INLINE CharW(unsigned int c) : _value((uint16_t)c) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the 16-bit character value.
  FOG_INLINE uint16_t getValue() const { return _value; }
  //! @brief Set the 16-bit character value.
  FOG_INLINE void setValue(uint16_t value) { _value = value; }

  //! @brief Get the 16-bit character value converted to a signed integer.
  FOG_INLINE int getInt() const
  {
    return (int)_value;
  }

  //! @brief Set the 16-bit character value from an integer.
  FOG_INLINE void setInt(int value)
  {
    FOG_ASSERT(value >= 0 && value <= 0xFFFF);
    _value = (uint16_t)value;
  }

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isNull() const { return _value == 0; }
  FOG_INLINE bool isAt(uint16_t start, uint16_t end) const { return (_value >= start) & (_value <= end); }

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

  FOG_INLINE CharW toAsciiLower() const { return CharW(_value < 128 ? _charData.asciiToLower[_value] : _value); }
  FOG_INLINE CharW toAsciiUpper() const { return CharW(_value < 128 ? _charData.asciiToUpper[_value] : _value); }

  // --------------------------------------------------------------------------
  // [Unicode Properties]
  // --------------------------------------------------------------------------

  FOG_INLINE const CharProperty& getProperty() const { return _charData.getPropertyUCS2Unsafe(_value); }

  FOG_INLINE bool isControl() const { return isControl(_value); }
  FOG_INLINE bool isDigit() const { return isDigit(_value); }
  FOG_INLINE bool isLetter() const { return isLetter(_value); }
  FOG_INLINE bool isLower() const { return isLower(_value); }
  FOG_INLINE bool isMark() const { return isMark(_value); }
  FOG_INLINE bool isNumber() const { return isNumber(_value); }
  FOG_INLINE bool isNumlet() const { return isNumlet(_value); }
  FOG_INLINE bool isPrint() const { return isPrint(_value); }
  FOG_INLINE bool isPunct() const { return isPunct(_value); }
  FOG_INLINE bool isSpace() const { return isSpace(_value); }
  FOG_INLINE bool isSymbol() const { return isSymbol(_value); }
  FOG_INLINE bool isTitle() const { return isTitle(_value); }
  FOG_INLINE bool isUpper() const { return isUpper(_value); }

  FOG_INLINE uint32_t getCategory() const { return getProperty().getCategory(); }
  FOG_INLINE uint32_t getUnicodeVersion() const { return getProperty().getUnicodeVersion(); }
  FOG_INLINE uint32_t getCombiningClass() const { return getProperty().getCombiningClass(); }
  FOG_INLINE uint32_t getScript() const { return getProperty().getScript(); }

  FOG_INLINE uint32_t getGraphemeBreak() const { return getProperty().getGraphemeBreak(); }
  FOG_INLINE uint32_t getWordBreak() const { return getProperty().getWordBreak(); }
  FOG_INLINE uint32_t getSentenceBreak() const { return getProperty().getSentenceBreak(); }
  FOG_INLINE uint32_t getLineBreak() const { return getProperty().getLineBreak(); }

  FOG_INLINE uint32_t getBidi() const { return getProperty().getBidi(); }
  FOG_INLINE uint32_t getJoining() const { return getProperty().getJoining(); }
  FOG_INLINE uint32_t getEastAsianWidth() const { return getProperty().getEastAsianWidth(); }

  FOG_INLINE uint32_t getCompositionExclusion() const { return getProperty().getCompositionExclusion(); }
  FOG_INLINE uint32_t quickCheckNFD() const { return getProperty().quickCheckNFD(); }
  FOG_INLINE uint32_t quickCheckNFC() const { return getProperty().quickCheckNFC(); }
  FOG_INLINE uint32_t quickCheckNFKD() const { return getProperty().quickCheckNFKD(); }
  FOG_INLINE uint32_t quickCheckNFKC() const { return getProperty().quickCheckNFKC(); }

  FOG_INLINE bool hasDecomposition() const { return getProperty().hasDecomposition(); }
  FOG_INLINE uint32_t getDecompositionType() const { return getProperty().getDecompositionType(); }

  FOG_INLINE const uint16_t* getDecompositionData() const { return getProperty().getDecompositionData(); }
  FOG_INLINE uint32_t getDecompositionIndex() const { return getProperty().getDecompositionIndex(); }

  FOG_INLINE bool hasUpper() const { return hasUpper(_value); }
  FOG_INLINE bool hasLower() const { return hasLower(_value); }
  FOG_INLINE bool hasTitle() const { return hasTitle(_value); }
  FOG_INLINE bool hasMirror() const { return hasMirror(_value); }

  FOG_INLINE CharW toUpper() const { return CharW(toUpper(_value)); }
  FOG_INLINE CharW toLower() const { return CharW(toLower(_value)); }
  FOG_INLINE CharW toTitle() const { return CharW(toTitle(_value)); }
  FOG_INLINE CharW toMirror() const { return CharW(toMirror(_value)); }

  // --------------------------------------------------------------------------
  // [Surrogates]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isSurrogate() const { return isSurrogate(_value); }
  FOG_INLINE bool isHiSurrogate() const { return isHiSurrogate(_value); }
  FOG_INLINE bool isLoSurrogate() const { return isLoSurrogate(_value); }

  // --------------------------------------------------------------------------
  // [BOM / BSwap]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isBomMark() const { return _value == UTF16_BOM_MARK; }
  FOG_INLINE bool isBomSwap() const { return _value == UTF16_BOM_SWAP; }
  FOG_INLINE CharW& bswap() { _value = MemOps::bswap16(_value); return *this; }

  // --------------------------------------------------------------------------
  // [Equality / Comparison]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const CharW& other) { return _value == other._value; }
  FOG_INLINE int compare(const CharW& other) { return (int)_value - (int)other._value; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE CharW& operator =(const CharW& c) { _value  = c._value; return *this; }
  FOG_INLINE CharW& operator-=(const CharW& c) { _value -= c._value; return *this; }
  FOG_INLINE CharW& operator+=(const CharW& c) { _value += c._value; return *this; }
  FOG_INLINE CharW operator-(const CharW& c) { return CharW(_value - c._value); }
  FOG_INLINE CharW operator+(const CharW& c) { return CharW(_value + c._value); }

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
  FOG_INLINE CharW& operator =(const char& c) { _value  = (uint8_t)c; return *this; }
  FOG_INLINE CharW& operator-=(const char& c) { _value -= (uint8_t)c; return *this; }
  FOG_INLINE CharW& operator+=(const char& c) { _value += (uint8_t)c; return *this; }
  FOG_INLINE CharW operator-(const char& c) { return CharW(_value - (uint8_t)c); }
  FOG_INLINE CharW operator+(const char& c) { return CharW(_value + (uint8_t)c); }
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE

  FOG_INLINE CharW& operator =(const signed char& c) { _value  = (uint8_t)c; return *this; }
  FOG_INLINE CharW& operator-=(const signed char& c) { _value -= (uint8_t)c; return *this; }
  FOG_INLINE CharW& operator+=(const signed char& c) { _value += (uint8_t)c; return *this; }
  FOG_INLINE CharW operator-(const signed char& c) { return CharW(_value - (uint8_t)c); }
  FOG_INLINE CharW operator+(const signed char& c) { return CharW(_value + (uint8_t)c); }

  FOG_INLINE CharW& operator =(const unsigned char& c) { _value  = c; return *this; }
  FOG_INLINE CharW& operator-=(const unsigned char& c) { _value -= c; return *this; }
  FOG_INLINE CharW& operator+=(const unsigned char& c) { _value += c; return *this; }
  FOG_INLINE CharW operator-(const unsigned char& c) { return CharW(_value - c); }
  FOG_INLINE CharW operator+(const unsigned char& c) { return CharW(_value + c); }

  FOG_INLINE CharW& operator =(const signed short& c) { _value  = (unsigned short)c; return *this; }
  FOG_INLINE CharW& operator-=(const signed short& c) { _value -= (unsigned short)c; return *this; }
  FOG_INLINE CharW& operator+=(const signed short& c) { _value += (unsigned short)c; return *this; }
  FOG_INLINE CharW operator-(const signed short& c) { return CharW(_value - (uint16_t)c); }
  FOG_INLINE CharW operator+(const signed short& c) { return CharW(_value + (uint16_t)c); }

  FOG_INLINE CharW& operator =(const unsigned short& c) { _value  = c; return *this; }
  FOG_INLINE CharW& operator-=(const unsigned short& c) { _value -= c; return *this; }
  FOG_INLINE CharW& operator+=(const unsigned short& c) { _value += c; return *this; }
  FOG_INLINE CharW operator-(const unsigned short& c) { return CharW(_value - c); }
  FOG_INLINE CharW operator+(const unsigned short& c) { return CharW(_value + c); }

  FOG_INLINE CharW& operator =(const signed int& c) { _value  = (uint16_t)(unsigned int)c; return *this; }
  FOG_INLINE CharW& operator-=(const signed int& c) { _value -= (uint16_t)(unsigned int)c; return *this; }
  FOG_INLINE CharW& operator+=(const signed int& c) { _value += (uint16_t)(unsigned int)c; return *this; }
  FOG_INLINE CharW operator-(const signed int& c) { return CharW(_value - (uint16_t)(unsigned int)c); }
  FOG_INLINE CharW operator+(const signed int& c) { return CharW(_value + (uint16_t)(unsigned int)c); }

  FOG_INLINE CharW& operator =(const unsigned int& c) { _value  = (uint16_t)c; return *this; }
  FOG_INLINE CharW& operator-=(const unsigned int& c) { _value -= (uint16_t)c; return *this; }
  FOG_INLINE CharW& operator+=(const unsigned int& c) { _value += (uint16_t)c; return *this; }
  FOG_INLINE CharW operator-(const unsigned int& c) { return CharW(_value - (uint16_t)c); }
  FOG_INLINE CharW operator+(const unsigned int& c) { return CharW(_value + (uint16_t)c); }

  FOG_INLINE bool operator==(const CharW& c) { return _value == c._value; }
  FOG_INLINE bool operator!=(const CharW& c) { return _value != c._value; }
  FOG_INLINE bool operator>=(const CharW& c) { return _value >= c._value; }
  FOG_INLINE bool operator> (const CharW& c) { return _value >  c._value; }
  FOG_INLINE bool operator<=(const CharW& c) { return _value <= c._value; }
  FOG_INLINE bool operator< (const CharW& c) { return _value <  c._value; }

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
  FOG_INLINE bool operator==(const char& c) { return _value == (uint8_t)c; }
  FOG_INLINE bool operator!=(const char& c) { return _value != (uint8_t)c; }
  FOG_INLINE bool operator>=(const char& c) { return _value >= (uint8_t)c; }
  FOG_INLINE bool operator> (const char& c) { return _value >  (uint8_t)c; }
  FOG_INLINE bool operator<=(const char& c) { return _value <= (uint8_t)c; }
  FOG_INLINE bool operator< (const char& c) { return _value <  (uint8_t)c; }
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE

  FOG_INLINE bool operator==(const signed char& c) { return _value == (uint8_t)c; }
  FOG_INLINE bool operator!=(const signed char& c) { return _value != (uint8_t)c; }
  FOG_INLINE bool operator>=(const signed char& c) { return _value >= (uint8_t)c; }
  FOG_INLINE bool operator> (const signed char& c) { return _value >  (uint8_t)c; }
  FOG_INLINE bool operator<=(const signed char& c) { return _value <= (uint8_t)c; }
  FOG_INLINE bool operator< (const signed char& c) { return _value <  (uint8_t)c; }

  FOG_INLINE bool operator==(const unsigned char& c) { return _value == c; }
  FOG_INLINE bool operator!=(const unsigned char& c) { return _value != c; }
  FOG_INLINE bool operator>=(const unsigned char& c) { return _value >= c; }
  FOG_INLINE bool operator> (const unsigned char& c) { return _value >  c; }
  FOG_INLINE bool operator<=(const unsigned char& c) { return _value <= c; }
  FOG_INLINE bool operator< (const unsigned char& c) { return _value <  c; }

  FOG_INLINE bool operator==(const signed short& c) { return _value == (unsigned short)c; }
  FOG_INLINE bool operator!=(const signed short& c) { return _value != (unsigned short)c; }
  FOG_INLINE bool operator>=(const signed short& c) { return _value >= (unsigned short)c; }
  FOG_INLINE bool operator> (const signed short& c) { return _value >  (unsigned short)c; }
  FOG_INLINE bool operator<=(const signed short& c) { return _value <= (unsigned short)c; }
  FOG_INLINE bool operator< (const signed short& c) { return _value <  (unsigned short)c; }

  FOG_INLINE bool operator==(const unsigned short& c) { return _value == c; }
  FOG_INLINE bool operator!=(const unsigned short& c) { return _value != c; }
  FOG_INLINE bool operator>=(const unsigned short& c) { return _value >= c; }
  FOG_INLINE bool operator> (const unsigned short& c) { return _value >  c; }
  FOG_INLINE bool operator<=(const unsigned short& c) { return _value <= c; }
  FOG_INLINE bool operator< (const unsigned short& c) { return _value <  c; }

  FOG_INLINE bool operator==(const signed int& c) { return _value == (unsigned int)c; }
  FOG_INLINE bool operator!=(const signed int& c) { return _value != (unsigned int)c; }
  FOG_INLINE bool operator>=(const signed int& c) { return _value >= (unsigned int)c; }
  FOG_INLINE bool operator> (const signed int& c) { return _value >  (unsigned int)c; }
  FOG_INLINE bool operator<=(const signed int& c) { return _value <= (unsigned int)c; }
  FOG_INLINE bool operator< (const signed int& c) { return _value <  (unsigned int)c; }

  FOG_INLINE bool operator==(const unsigned int& c) { return _value == c; }
  FOG_INLINE bool operator!=(const unsigned int& c) { return _value != c; }
  FOG_INLINE bool operator>=(const unsigned int& c) { return _value >= c; }
  FOG_INLINE bool operator> (const unsigned int& c) { return _value >  c; }
  FOG_INLINE bool operator<=(const unsigned int& c) { return _value <= c; }
  FOG_INLINE bool operator< (const unsigned int& c) { return _value <  c; }

  // --------------------------------------------------------------------------
  // [Implicit Conversion]
  // --------------------------------------------------------------------------

  FOG_INLINE operator uint16_t() const { return (uint16_t)_value; }

  // --------------------------------------------------------------------------
  // [Statics - Common]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool isNull(uint16_t c) { return c == 0; }
  static FOG_INLINE bool isNull(uint32_t c) { return c == 0; }

  static FOG_INLINE bool isAt(uint16_t c, uint16_t start, uint16_t end) { return (c >= start) & (c <= end); }
  static FOG_INLINE bool isAt(uint32_t c, uint32_t start, uint32_t end) { return (c >= start) & (c <= end); }

  // --------------------------------------------------------------------------
  // [Statics - Ascii]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool isAsciiBlank(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_BLANK) != 0 : false; }
  static FOG_INLINE bool isAsciiBlank(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_BLANK) != 0 : false; }

  static FOG_INLINE bool isAsciiCntrl(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_PRINT) == 0 : true; }
  static FOG_INLINE bool isAsciiCntrl(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_PRINT) == 0 : true; }

  static FOG_INLINE bool isAsciiDigit(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_DIGIT) != 0 : false; }
  static FOG_INLINE bool isAsciiDigit(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_DIGIT) != 0 : false; }

  static FOG_INLINE bool isAsciiGraph(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_GRAPH) != 0 : false; }
  static FOG_INLINE bool isAsciiGraph(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_GRAPH) != 0 : false; }

  static FOG_INLINE bool isAsciiLetter(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_ALPHA) != 0 : false; }
  static FOG_INLINE bool isAsciiLetter(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_ALPHA) != 0 : false; }

  static FOG_INLINE bool isAsciiLetter_(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_ALPHA_) != 0 : false; }
  static FOG_INLINE bool isAsciiLetter_(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_ALPHA_) != 0 : false; }

  static FOG_INLINE bool isAsciiLower(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_LOWER) != 0 : false; }
  static FOG_INLINE bool isAsciiLower(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_LOWER) != 0 : false; }

  static FOG_INLINE bool isAsciiNumlet(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_ALNUM) != 0 : false; }
  static FOG_INLINE bool isAsciiNumlet(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_ALNUM) != 0 : false; }

  static FOG_INLINE bool isAsciiPrint(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_PRINT) != 0 : false; }
  static FOG_INLINE bool isAsciiPrint(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_PRINT) != 0 : false; }

  static FOG_INLINE bool isAsciiPunct(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_PUNCT) != 0 : false; }
  static FOG_INLINE bool isAsciiPunct(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_PUNCT) != 0 : false; }

  static FOG_INLINE bool isAsciiSpace(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_SPACE) != 0 : false; }
  static FOG_INLINE bool isAsciiSpace(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_SPACE) != 0 : false; }

  static FOG_INLINE bool isAsciiUpper(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_UPPER) != 0 : false; }
  static FOG_INLINE bool isAsciiUpper(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_UPPER) != 0 : false; }

  static FOG_INLINE bool isAsciiXDigit(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_XDIGIT) != 0 : false; }
  static FOG_INLINE bool isAsciiXDigit(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_XDIGIT) != 0 : false; }

  static FOG_INLINE bool isAsciiXLetter(uint16_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_XALPHA) != 0 : false; }
  static FOG_INLINE bool isAsciiXLetter(uint32_t c) { return c < 128 ? (_charData.asciiCType[c] & ASCII_CLASS_XALPHA) != 0 : false; }

  static FOG_INLINE uint16_t toAsciiLower(uint16_t c) { return c < 128 ? (uint16_t)_charData.asciiToLower[c] : c; }
  static FOG_INLINE uint32_t toAsciiLower(uint32_t c) { return c < 128 ? (uint16_t)_charData.asciiToLower[c] : c; }

  static FOG_INLINE uint16_t toAsciiUpper(uint16_t c) { return c < 128 ? (uint16_t)_charData.asciiToUpper[c] : c; }
  static FOG_INLINE uint32_t toAsciiUpper(uint32_t c) { return c < 128 ? (uint16_t)_charData.asciiToUpper[c] : c; }

  // --------------------------------------------------------------------------
  // [Statics - Unicode]
  // --------------------------------------------------------------------------

  static FOG_INLINE const CharProperty& getProperty(uint16_t c) { return _charData.getPropertyUCS2(c); }
  static FOG_INLINE const CharProperty& getProperty(uint32_t c) { return _charData.getPropertyUCS4(c); }

  static FOG_INLINE const CharProperty& getPropertyUnsafe(uint16_t c) { return _charData.getPropertyUCS2Unsafe(c); }
  static FOG_INLINE const CharProperty& getPropertyUnsafe(uint32_t c) { return _charData.getPropertyUCS4Unsafe(c); }

  static FOG_INLINE bool isCategoryAt(uint16_t c, uint32_t first, uint32_t last)
  {
    FOG_ASSERT(first <= last);
    return getCategory(c) - first <= last - first;
  }

  static FOG_INLINE bool isCategoryAt(uint32_t c, uint32_t first, uint32_t last)
  {
    FOG_ASSERT(first <= last);
    return getCategory(c) - first <= last - first;
  }

  static FOG_INLINE bool isControl(uint16_t c) { return isCategoryAt(c, CHAR_CATEGORY_CC, CHAR_CATEGORY_CN); }
  static FOG_INLINE bool isControl(uint32_t c) { return isCategoryAt(c, CHAR_CATEGORY_CC, CHAR_CATEGORY_CN); }

  static FOG_INLINE bool isDigit(uint16_t c) { return getCategory(c) == CHAR_CATEGORY_NUMBER_DECIMAL; }
  static FOG_INLINE bool isDigit(uint32_t c) { return getCategory(c) == CHAR_CATEGORY_NUMBER_DECIMAL; }

  static FOG_INLINE bool isLetter(uint16_t c) { return isCategoryAt(c, CHAR_CATEGORY_LETTER_UPPERCASE, CHAR_CATEGORY_LETTER_OTHER); }
  static FOG_INLINE bool isLetter(uint32_t c) { return isCategoryAt(c, CHAR_CATEGORY_LETTER_UPPERCASE, CHAR_CATEGORY_LETTER_OTHER); }

  static FOG_INLINE bool isLower(uint16_t c) { return getCategory(c) == CHAR_CATEGORY_LETTER_LOWERCASE; }
  static FOG_INLINE bool isLower(uint32_t c) { return getCategory(c) == CHAR_CATEGORY_LETTER_LOWERCASE; }

  static FOG_INLINE bool isMark(uint16_t c) { return isCategoryAt(c, CHAR_CATEGORY_MARK_SPACING, CHAR_CATEGORY_MARK_NONSPACING); }
  static FOG_INLINE bool isMark(uint32_t c) { return isCategoryAt(c, CHAR_CATEGORY_MARK_SPACING, CHAR_CATEGORY_MARK_NONSPACING); }

  static FOG_INLINE bool isNumber(uint16_t c) { return isCategoryAt(c, CHAR_CATEGORY_NUMBER_DECIMAL, CHAR_CATEGORY_NUMBER_OTHER); }
  static FOG_INLINE bool isNumber(uint32_t c) { return isCategoryAt(c, CHAR_CATEGORY_NUMBER_DECIMAL, CHAR_CATEGORY_NUMBER_OTHER); }

  static FOG_INLINE bool isNumlet(uint16_t c) { return isCategoryAt(c, CHAR_CATEGORY_LETTER_UPPERCASE, CHAR_CATEGORY_NUMBER_OTHER); }
  static FOG_INLINE bool isNumlet(uint32_t c) { return isCategoryAt(c, CHAR_CATEGORY_LETTER_UPPERCASE, CHAR_CATEGORY_NUMBER_OTHER); }

  static FOG_INLINE bool isPrint(uint16_t c) { return !isCategoryAt(c, CHAR_CATEGORY_OTHER_CONTROL, CHAR_CATEGORY_OTHER_UNASSIGNED); }
  static FOG_INLINE bool isPrint(uint32_t c) { return !isCategoryAt(c, CHAR_CATEGORY_OTHER_CONTROL, CHAR_CATEGORY_OTHER_UNASSIGNED); }

  static FOG_INLINE bool isPunct(uint16_t c) { return isCategoryAt(c, CHAR_CATEGORY_PUNCT_CONNECTOR, CHAR_CATEGORY_PUNCT_OTHER); }
  static FOG_INLINE bool isPunct(uint32_t c) { return isCategoryAt(c, CHAR_CATEGORY_PUNCT_CONNECTOR, CHAR_CATEGORY_PUNCT_OTHER); }

  static FOG_INLINE bool isSpace(uint16_t c) { return getProperty(c).isSpace(); }
  static FOG_INLINE bool isSpace(uint32_t c) { return getProperty(c).isSpace(); }

  static FOG_INLINE bool isSymbol(uint16_t c) { return isCategoryAt(c, CHAR_CATEGORY_SYMBOL_MATH, CHAR_CATEGORY_SYMBOL_OTHER); }
  static FOG_INLINE bool isSymbol(uint32_t c) { return isCategoryAt(c, CHAR_CATEGORY_SYMBOL_MATH, CHAR_CATEGORY_SYMBOL_OTHER); }

  static FOG_INLINE bool isTitle(uint16_t c) { return getCategory(c) == CHAR_CATEGORY_LETTER_TITLECASE; }
  static FOG_INLINE bool isTitle(uint32_t c) { return getCategory(c) == CHAR_CATEGORY_LETTER_TITLECASE; }

  static FOG_INLINE bool isUpper(uint16_t c) { return getCategory(c) == CHAR_CATEGORY_LETTER_UPPERCASE; }
  static FOG_INLINE bool isUpper(uint32_t c) { return getCategory(c) == CHAR_CATEGORY_LETTER_UPPERCASE; }

  static FOG_INLINE uint32_t getCategory(uint16_t c) { return getProperty(c).getCategory(); }
  static FOG_INLINE uint32_t getCategory(uint32_t c) { return getProperty(c).getCategory(); }

  static FOG_INLINE uint32_t getUnicodeVersion(uint16_t c) { return getProperty(c).getUnicodeVersion(); }
  static FOG_INLINE uint32_t getUnicodeVersion(uint32_t c) { return getProperty(c).getUnicodeVersion(); }

  static FOG_INLINE uint32_t getCombiningClass(uint16_t c) { return getProperty(c).getCombiningClass(); }
  static FOG_INLINE uint32_t getCombiningClass(uint32_t c) { return getProperty(c).getCombiningClass(); }

  static FOG_INLINE uint32_t getScript(uint16_t c) { return getProperty(c).getScript(); }
  static FOG_INLINE uint32_t getScript(uint32_t c) { return getProperty(c).getScript(); }

  static FOG_INLINE uint32_t getGraphemeBreak(uint16_t c) { return getProperty(c).getGraphemeBreak(); }
  static FOG_INLINE uint32_t getGraphemeBreak(uint32_t c) { return getProperty(c).getGraphemeBreak(); }

  static FOG_INLINE uint32_t getWordBreak(uint16_t c) { return getProperty(c).getWordBreak(); }
  static FOG_INLINE uint32_t getWordBreak(uint32_t c) { return getProperty(c).getWordBreak(); }

  static FOG_INLINE uint32_t getSentenceBreak(uint16_t c) { return getProperty(c).getSentenceBreak(); }
  static FOG_INLINE uint32_t getSentenceBreak(uint32_t c) { return getProperty(c).getSentenceBreak(); }

  static FOG_INLINE uint32_t getLineBreak(uint16_t c) { return getProperty(c).getLineBreak(); }
  static FOG_INLINE uint32_t getLineBreak(uint32_t c) { return getProperty(c).getLineBreak(); }

  static FOG_INLINE uint32_t getBidi(uint16_t c) { return getProperty(c).getBidi(); }
  static FOG_INLINE uint32_t getBidi(uint32_t c) { return getProperty(c).getBidi(); }

  static FOG_INLINE uint32_t getJoining(uint16_t c) { return getProperty(c).getJoining(); }
  static FOG_INLINE uint32_t getJoining(uint32_t c) { return getProperty(c).getJoining(); }

  static FOG_INLINE uint32_t getEastAsianWidth(uint16_t c) { return getProperty(c).getEastAsianWidth(); }
  static FOG_INLINE uint32_t getEastAsianWidth(uint32_t c) { return getProperty(c).getEastAsianWidth(); }

  static FOG_INLINE uint32_t getCompositionExclusion(uint16_t c) { return getProperty(c).getCompositionExclusion(); }
  static FOG_INLINE uint32_t getCompositionExclusion(uint32_t c) { return getProperty(c).getCompositionExclusion(); }

  static FOG_INLINE uint32_t quickCheckNFD(uint16_t c) { return getProperty(c).quickCheckNFD(); }
  static FOG_INLINE uint32_t quickCheckNFD(uint32_t c) { return getProperty(c).quickCheckNFD(); }

  static FOG_INLINE uint32_t quickCheckNFC(uint16_t c) { return getProperty(c).quickCheckNFC(); }
  static FOG_INLINE uint32_t quickCheckNFC(uint32_t c) { return getProperty(c).quickCheckNFC(); }

  static FOG_INLINE uint32_t quickCheckNFKD(uint16_t c) { return getProperty(c).quickCheckNFKD(); }
  static FOG_INLINE uint32_t quickCheckNFKD(uint32_t c) { return getProperty(c).quickCheckNFKD(); }

  static FOG_INLINE uint32_t quickCheckNFKC(uint16_t c) { return getProperty(c).quickCheckNFKC(); }
  static FOG_INLINE uint32_t quickCheckNFKC(uint32_t c) { return getProperty(c).quickCheckNFKC(); }

  static FOG_INLINE bool hasDecomposition(uint16_t c) { return getProperty(c).hasDecomposition(); }
  static FOG_INLINE bool hasDecomposition(uint32_t c) { return getProperty(c).hasDecomposition(); }

  static FOG_INLINE uint32_t getDecompositionType(uint16_t c) { return getProperty(c).getDecompositionType(); }
  static FOG_INLINE uint32_t getDecompositionType(uint32_t c) { return getProperty(c).getDecompositionType(); }

  static FOG_INLINE const uint16_t* getDecompositionData(uint16_t c) { return getProperty(c).getDecompositionData(); }
  static FOG_INLINE const uint16_t* getDecompositionData(uint32_t c) { return getProperty(c).getDecompositionData(); }

  static FOG_INLINE uint32_t getDecompositionIndex(uint16_t c) { return getProperty(c).getDecompositionIndex(); }
  static FOG_INLINE uint32_t getDecompositionIndex(uint32_t c) { return getProperty(c).getDecompositionIndex(); }

  static FOG_INLINE bool hasUpper(uint16_t c)
  {
    const CharProperty& p = getProperty(c);
    return (p.getMappingType() == CHAR_MAPPING_UPPERCASE) ||
           (p.getMappingType() == CHAR_MAPPING_SPECIAL && _charData.special[p.getMappingData()]._upperCaseDiff != 0);
  }

  static FOG_INLINE bool hasUpper(uint32_t c)
  {
    const CharProperty& p = getProperty(c);

    return (p.getMappingType() == CHAR_MAPPING_UPPERCASE) ||
           (p.getMappingType() == CHAR_MAPPING_SPECIAL && _charData.special[p.getMappingData()]._upperCaseDiff != 0);
  }

  static FOG_INLINE bool hasLower(uint16_t c)
  {
    const CharProperty& p = getProperty(c);
    return (p.getMappingType() == CHAR_MAPPING_LOWERCASE) ||
           (p.getMappingType() == CHAR_MAPPING_SPECIAL && _charData.special[p.getMappingData()]._lowerCaseDiff != 0);
  }

  static FOG_INLINE bool hasLower(uint32_t c)
  {
    const CharProperty& p = getProperty(c);
    return (p.getMappingType() == CHAR_MAPPING_LOWERCASE) ||
           (p.getMappingType() == CHAR_MAPPING_SPECIAL && _charData.special[p.getMappingData()]._lowerCaseDiff != 0);
  }

  static FOG_INLINE bool hasTitle(uint16_t c)
  {
    const CharProperty& p = getProperty(c);
    return p.getMappingType() == CHAR_MAPPING_SPECIAL && _charData.special[p.getMappingData()]._titleCaseDiff != 0;
  }

  static FOG_INLINE bool hasTitle(uint32_t c)
  {
    const CharProperty& p = getProperty(c);
    return p.getMappingType() == CHAR_MAPPING_SPECIAL && _charData.special[p.getMappingData()]._titleCaseDiff != 0;
  }

  static FOG_INLINE bool hasMirror(uint16_t c)
  {
    const CharProperty& p = getProperty(c);
    return (p.getMappingType() == CHAR_MAPPING_MIRROR) ||
           (p.getMappingType() == CHAR_MAPPING_SPECIAL && _charData.special[p.getMappingData()]._mirrorDiff != 0);
  }

  static FOG_INLINE bool hasMirror(uint32_t c)
  {
    const CharProperty& p = getProperty(c);
    return (p.getMappingType() == CHAR_MAPPING_MIRROR) ||
           (p.getMappingType() == CHAR_MAPPING_SPECIAL && _charData.special[p.getMappingData()]._mirrorDiff != 0);
  }

  static FOG_INLINE uint16_t toUpper(uint16_t c)
  {
    const CharProperty& p = getProperty(c);
    int32_t x = p.getMappingData();

    if (p.getMappingType() == CHAR_MAPPING_SPECIAL)
      x = _charData.special[x]._upperCaseDiff;
    else if (p.getMappingType() != CHAR_MAPPING_UPPERCASE)
      x = 0;

    x += (int32_t)c;
    FOG_ASSUME(x >= 0 && x < 0x10000);

    return (uint16_t)x;
  }

  static FOG_INLINE uint32_t toUpper(uint32_t c)
  {
    const CharProperty& p = getProperty(c);
    int32_t x = p.getMappingData();

    if (p.getMappingType() == CHAR_MAPPING_SPECIAL)
      x = _charData.special[x]._upperCaseDiff;
    else if (p.getMappingType() != CHAR_MAPPING_UPPERCASE)
      x = 0;

    x += (int32_t)c;
    FOG_ASSUME(x >= 0);

    return (uint32_t)x;
  }

  static FOG_INLINE uint16_t toLower(uint16_t c)
  {
    const CharProperty& p = getProperty(c);
    int32_t x = p.getMappingData();

    if (p.getMappingType() == CHAR_MAPPING_SPECIAL)
      x = _charData.special[x]._lowerCaseDiff;
    else if (p.getMappingType() != CHAR_MAPPING_LOWERCASE)
      x = 0;

    x += (int32_t)c;
    FOG_ASSUME(x >= 0 && x < 0x10000);

    return (uint16_t)x;
  }

  static FOG_INLINE uint32_t toLower(uint32_t c)
  {
    const CharProperty& p = getProperty(c);
    int32_t x = p.getMappingData();

    if (p.getMappingType() == CHAR_MAPPING_SPECIAL)
      x = _charData.special[x]._lowerCaseDiff;
    else if (p.getMappingType() != CHAR_MAPPING_LOWERCASE)
      x = 0;

    x += (int32_t)c;
    FOG_ASSUME(x >= 0);

    return (uint32_t)x;
  }

  static FOG_INLINE uint16_t toTitle(uint16_t c)
  {
    const CharProperty& p = getProperty(c);
    if (p.getMappingType() != CHAR_MAPPING_SPECIAL)
      return c;

    int32_t x = (int32_t)c + _charData.special[p.getMappingData()]._titleCaseDiff;
    FOG_ASSUME(x >= 0 && x < 0x10000);

    return (uint16_t)x;
  }

  static FOG_INLINE uint32_t toTitle(uint32_t c)
  {
    const CharProperty& p = getProperty(c);
    if (p.getMappingType() != CHAR_MAPPING_SPECIAL)
      return c;

    int32_t x = (int32_t)c + _charData.special[p.getMappingData()]._titleCaseDiff;
    FOG_ASSUME(x >= 0);

    return (uint32_t)x;
  }

  static FOG_INLINE uint16_t toMirror(uint16_t c)
  {
    const CharProperty& p = getProperty(c);
    int32_t x = p.getMappingData();

    if (p.getMappingType() == CHAR_MAPPING_SPECIAL)
      x = _charData.special[x]._mirrorDiff;
    else if (p.getMappingType() != CHAR_MAPPING_MIRROR)
      x = 0;

    x += (int32_t)c;
    FOG_ASSUME(x >= 0 && x < 0x10000);

    return (uint16_t)x;
  }

  static FOG_INLINE uint32_t toMirror(uint32_t c)
  {
    const CharProperty& p = getProperty(c);
    int32_t x = p.getMappingData();

    if (p.getMappingType() == CHAR_MAPPING_SPECIAL)
      x = _charData.special[x]._mirrorDiff;
    else if (p.getMappingType() != CHAR_MAPPING_MIRROR)
      x = 0;

    x += (int32_t)c;
    FOG_ASSUME(x >= 0);

    return (uint32_t)x;
  }

  static FOG_INLINE bool isHangulSyllable(uint16_t c)
  {
    return Math::isBounded<uint16_t>(c, CHAR_HANGUL_BASE_S, CHAR_HANGUL_BASE_S + CHAR_HANGUL_COUNT_S - 1);
  }

  static FOG_INLINE bool isHangulSyllable(uint32_t c)
  {
    return Math::isBounded<uint32_t>(c, CHAR_HANGUL_BASE_S, CHAR_HANGUL_BASE_S + CHAR_HANGUL_COUNT_S - 1);
  }

  static FOG_INLINE uint32_t decompose(uint16_t c, uint16_t* dst16, uint32_t* decompositionType)
  {
    return fog_api.charutil_decomposeUcs2To16(c, dst16, decompositionType);
  }

  static FOG_INLINE uint32_t decompose(uint16_t c, uint32_t* dst32, uint32_t* decompositionType)
  {
    return fog_api.charutil_decomposeUcs2To32(c, dst32, decompositionType);
  }

  static FOG_INLINE uint32_t decompose(uint32_t c, uint16_t* dst16, uint32_t* decompositionType)
  {
    return fog_api.charutil_decomposeUcs4To16(c, dst16, decompositionType);
  }

  static FOG_INLINE uint32_t decompose(uint32_t c, uint32_t* dst32, uint32_t* decompositionType)
  {
    return fog_api.charutil_decomposeUcs4To32(c, dst32, decompositionType);
  }

  static FOG_INLINE uint16_t compose(uint16_t a, uint16_t b) { return fog_api.charutil_composeUcs2(a, b); }
  static FOG_INLINE uint32_t compose(uint32_t a, uint32_t b) { return fog_api.charutil_composeUcs4(a, b); }

  static FOG_INLINE uint16_t composeHangul(uint16_t l, uint16_t v)
  {
    return CHAR_HANGUL_BASE_S + (l - CHAR_HANGUL_BASE_L) * (CHAR_HANGUL_COUNT_N    ) +
                                (v - CHAR_HANGUL_BASE_V) * (CHAR_HANGUL_COUNT_T + 1) ;
  }

  static FOG_INLINE uint32_t composeHangul(uint32_t l, uint32_t v)
  {
    return CHAR_HANGUL_BASE_S + (l - CHAR_HANGUL_BASE_L) * (CHAR_HANGUL_COUNT_N    ) +
                                (v - CHAR_HANGUL_BASE_V) * (CHAR_HANGUL_COUNT_T + 1) ;
  }

  static FOG_INLINE uint16_t composeHangul(uint16_t l, uint16_t v, uint16_t t)
  {
    return CHAR_HANGUL_BASE_S + (l - CHAR_HANGUL_BASE_L) * (CHAR_HANGUL_COUNT_N    ) +
                                (v - CHAR_HANGUL_BASE_V) * (CHAR_HANGUL_COUNT_T + 1) +
                                (t - CHAR_HANGUL_BASE_T) + 1;
  }

  static FOG_INLINE uint16_t composeHangul(uint32_t l, uint32_t v, uint32_t t)
  {
    return CHAR_HANGUL_BASE_S + (l - CHAR_HANGUL_BASE_L) * (CHAR_HANGUL_COUNT_N    ) +
                                (v - CHAR_HANGUL_BASE_V) * (CHAR_HANGUL_COUNT_T + 1) +
                                (t - CHAR_HANGUL_BASE_T) + 1;
  }

  static FOG_INLINE bool isSurrogate(uint16_t c) { return (c & 0x0000F800) == 0xD800; }
  static FOG_INLINE bool isSurrogate(uint32_t c) { return (c & 0xFFFFF800) == 0xD800; }

  static FOG_INLINE bool isHiSurrogate(uint16_t c) { return (c & 0x0000FC00) == 0xD800; }
  static FOG_INLINE bool isHiSurrogate(uint32_t c) { return (c & 0xFFFFFC00) == 0xD800; }

  static FOG_INLINE bool isLoSurrogate(uint16_t c) { return (c & 0x0000FC00) == 0xDC00; }
  static FOG_INLINE bool isLoSurrogate(uint32_t c) { return (c & 0xFFFFFC00) == 0xDC00; }

  static FOG_INLINE uint32_t ucs4FromSurrogate(uint16_t hi, uint16_t lo)
  {
    // 0x035FDC00 == 0x0D800 - (0x10000 >> 10)
    return static_cast<uint32_t>(lo) + (static_cast<uint32_t>(hi) << 10) - 0x035FDC00;
  }

  static FOG_INLINE uint32_t ucs4FromSurrogate(uint32_t hi, uint32_t lo)
  {
    // 0x035FDC00 == 0x0D800 - (0x10000 >> 10)
    return lo + (hi << 10) - 0x035FDC00;
  }

  static FOG_INLINE void ucs4ToSurrogate(uint16_t* hi, uint16_t* lo, uint32_t ucs4)
  {
    FOG_ASSERT(ucs4 >= 0x10000);

    *hi = 0xD7C0 + (ucs4 >> 10);
    *lo = 0xDC00 + (ucs4 & 0x03FFU);
  }

  static FOG_INLINE void ucs4ToSurrogate(CharW* hi, CharW* lo, uint32_t ucs4)
  {
    ucs4ToSurrogate(&hi->_value, &lo->_value, ucs4);
  }

  static FOG_INLINE bool ucs4IsBMP(uint32_t c)
  {
    return c < 0x10000;
  }

  static FOG_INLINE bool ucs4IsSMP(uint32_t c)
  {
    return c >= 0x10000;
  }

  static FOG_INLINE bool isBomMark(uint16_t ch) { return ch == UTF16_BOM_MARK; }
  static FOG_INLINE bool isBomSwap(uint16_t ch) { return ch == UTF16_BOM_SWAP; }
  static FOG_INLINE uint16_t bswap(uint16_t ch) { return MemOps::bswap16(ch); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief 16-bit character value.
  uint16_t _value;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::CharT<>]
// ============================================================================

_FOG_CHAR_T(Char)
_FOG_CHAR_A(Char)
_FOG_CHAR_W(Char)

// Internal.
typedef CharA _CharA;
typedef CharW _CharW;

_FOG_CHAR_T(_Char)
_FOG_CHAR_A(_Char)
_FOG_CHAR_W(_Char)

//! @}

} // Fog namespace

#endif // _FOG_CORE_TOOLS_CHAR_H
