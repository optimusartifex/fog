// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_CHAR_H
#define _FOG_CORE_TOOLS_CHAR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Tools/CharData.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::Char]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief 16-bit unicode character.
struct FOG_NO_EXPORT Char
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  // Template support.
  typedef Char PType;
  typedef Char UType;
  typedef uint16_t Value;
  typedef Utf16 Stub;
  typedef String Sequence;
  typedef StringFilter Filter;
  typedef StringMatcher Matcher;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Char() {}
  FOG_INLINE Char(const Char& c) : _value(c._value) {}

#if defined(FOG_CC_HAVE_STANDARD_CHAR_TYPE)
  explicit FOG_INLINE Char(char c) : _value((unsigned char)c) {}
#endif // FOG_CC_HAVE_STANDARD_CHAR_TYPE

  explicit FOG_INLINE Char(signed char c) : _value((unsigned char)c) {}
  explicit FOG_INLINE Char(unsigned char c) : _value(c) {}

  explicit FOG_INLINE Char(signed short c) : _value((unsigned short)c) {}
  explicit FOG_INLINE Char(unsigned short c) : _value(c) {}

  explicit FOG_INLINE Char(signed int c) : _value((uint16_t)(unsigned int)c) {}
  explicit FOG_INLINE Char(unsigned int c) : _value((uint16_t)c) {}

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
  FOG_INLINE bool isAt(uint16_t start, uint16_t end) const { return _value >= start && _value <= end; }

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

  FOG_INLINE Char toAsciiLower() const { return Char(_value < 128 ? _charData.asciiToLower[_value] : _value); }
  FOG_INLINE Char toAsciiUpper() const { return Char(_value < 128 ? _charData.asciiToUpper[_value] : _value); }

  // --------------------------------------------------------------------------
  // [Unicode Properties]
  // --------------------------------------------------------------------------
  
  FOG_INLINE const CharProperty& getProperty() const { return _charData.getPropertyUCS2(_value); }

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

  FOG_INLINE Char toUpper() const { return Char(toUpper(_value)); }
  FOG_INLINE Char toLower() const { return Char(toLower(_value)); }
  FOG_INLINE Char toTitle() const { return Char(toTitle(_value)); }

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
  FOG_INLINE Char& bswap() { _value = Memory::bswap16(_value); return *this; }

  // --------------------------------------------------------------------------
  // [Equality / Comparison]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Char& other) { return _value == other._value; }
  FOG_INLINE int compare(const Char& other) { return (int)_value - (int)other._value; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Char& operator =(const Char& c) { _value  = c._value; return *this; }
  FOG_INLINE Char& operator-=(const Char& c) { _value -= c._value; return *this; }
  FOG_INLINE Char& operator+=(const Char& c) { _value += c._value; return *this; }
  FOG_INLINE Char operator-(const Char& c) { return Char(_value - c._value); }
  FOG_INLINE Char operator+(const Char& c) { return Char(_value + c._value); }

#if defined(FOG_CC_HAVE_STANDARD_CHAR_TYPE)
  FOG_INLINE Char& operator =(const char& c) { _value  = (uint8_t)c; return *this; }
  FOG_INLINE Char& operator-=(const char& c) { _value -= (uint8_t)c; return *this; }
  FOG_INLINE Char& operator+=(const char& c) { _value += (uint8_t)c; return *this; }
  FOG_INLINE Char operator-(const char& c) { return Char(_value - (uint8_t)c); }
  FOG_INLINE Char operator+(const char& c) { return Char(_value + (uint8_t)c); }
#endif // FOG_CC_HAVE_STANDARD_CHAR_TYPE

  FOG_INLINE Char& operator =(const signed char& c) { _value  = (uint8_t)c; return *this; }
  FOG_INLINE Char& operator-=(const signed char& c) { _value -= (uint8_t)c; return *this; }
  FOG_INLINE Char& operator+=(const signed char& c) { _value += (uint8_t)c; return *this; }
  FOG_INLINE Char operator-(const signed char& c) { return Char(_value - (uint8_t)c); }
  FOG_INLINE Char operator+(const signed char& c) { return Char(_value + (uint8_t)c); }

  FOG_INLINE Char& operator =(const unsigned char& c) { _value  = c; return *this; }
  FOG_INLINE Char& operator-=(const unsigned char& c) { _value -= c; return *this; }
  FOG_INLINE Char& operator+=(const unsigned char& c) { _value += c; return *this; }
  FOG_INLINE Char operator-(const unsigned char& c) { return Char(_value - c); }
  FOG_INLINE Char operator+(const unsigned char& c) { return Char(_value + c); }

  FOG_INLINE Char& operator =(const signed short& c) { _value  = (unsigned short)c; return *this; }
  FOG_INLINE Char& operator-=(const signed short& c) { _value -= (unsigned short)c; return *this; }
  FOG_INLINE Char& operator+=(const signed short& c) { _value += (unsigned short)c; return *this; }
  FOG_INLINE Char operator-(const signed short& c) { return Char(_value - (uint16_t)c); }
  FOG_INLINE Char operator+(const signed short& c) { return Char(_value + (uint16_t)c); }

  FOG_INLINE Char& operator =(const unsigned short& c) { _value  = c; return *this; }
  FOG_INLINE Char& operator-=(const unsigned short& c) { _value -= c; return *this; }
  FOG_INLINE Char& operator+=(const unsigned short& c) { _value += c; return *this; }
  FOG_INLINE Char operator-(const unsigned short& c) { return Char(_value - c); }
  FOG_INLINE Char operator+(const unsigned short& c) { return Char(_value + c); }

  FOG_INLINE Char& operator =(const signed int& c) { _value  = (uint16_t)(unsigned int)c; return *this; }
  FOG_INLINE Char& operator-=(const signed int& c) { _value -= (uint16_t)(unsigned int)c; return *this; }
  FOG_INLINE Char& operator+=(const signed int& c) { _value += (uint16_t)(unsigned int)c; return *this; }
  FOG_INLINE Char operator-(const signed int& c) { return Char(_value - (uint16_t)(unsigned int)c); }
  FOG_INLINE Char operator+(const signed int& c) { return Char(_value + (uint16_t)(unsigned int)c); }

  FOG_INLINE Char& operator =(const unsigned int& c) { _value  = (uint16_t)c; return *this; }
  FOG_INLINE Char& operator-=(const unsigned int& c) { _value -= (uint16_t)c; return *this; }
  FOG_INLINE Char& operator+=(const unsigned int& c) { _value += (uint16_t)c; return *this; }
  FOG_INLINE Char operator-(const unsigned int& c) { return Char(_value - (uint16_t)c); }
  FOG_INLINE Char operator+(const unsigned int& c) { return Char(_value + (uint16_t)c); }

  FOG_INLINE bool operator==(const Char& c) { return _value == c._value; }
  FOG_INLINE bool operator!=(const Char& c) { return _value != c._value; }
  FOG_INLINE bool operator>=(const Char& c) { return _value >= c._value; }
  FOG_INLINE bool operator> (const Char& c) { return _value >  c._value; }
  FOG_INLINE bool operator<=(const Char& c) { return _value <= c._value; }
  FOG_INLINE bool operator< (const Char& c) { return _value <  c._value; }

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
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool isNull(uint16_t c) { return c == 0; }
  static FOG_INLINE bool isNull(uint32_t c) { return c == 0; }

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

  static FOG_INLINE const CharProperty& getProperty(uint16_t c) { return _charData.getPropertyUCS2(c); }
  static FOG_INLINE const CharProperty& getProperty(uint32_t c) { return _charData.getPropertyUCS2(c); }

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

    int32_t x = (int32_t)c + _charData.special[x]._titleCaseDiff;
    FOG_ASSUME(x >= 0 && x < 0x10000);

    return (uint16_t)x;
  }

  static FOG_INLINE uint32_t toTitle(uint32_t c)
  {
    const CharProperty& p = getProperty(c);
    if (p.getMappingType() != CHAR_MAPPING_SPECIAL)
      return c;

    int32_t x = (int32_t)c + _charData.special[x]._titleCaseDiff;
    FOG_ASSUME(x >= 0);

    return (uint32_t)x;
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
    return (uint32_t)lo + ((uint32_t)hi << 10) - 0x035FDC00;
  }

  static FOG_INLINE void ucs4ToSurrogate(uint16_t* hi, uint16_t* lo, uint32_t ucs4)
  {
    FOG_ASSERT(ucs4 >= 0x10000);

    *hi = 0xD7C0 + (ucs4 >> 10);
    *lo = 0xDC00 + (ucs4 & 0x03FFU);
  }

  static FOG_INLINE void ucs4ToSurrogate(Char* hi, Char* lo, uint32_t ucs4)
  {
    ucs4ToSurrogate(&hi->_value, &lo->_value, ucs4);
  }

  static FOG_INLINE bool isBomMark(uint16_t ch) { return ch == UTF16_BOM_MARK; }
  static FOG_INLINE bool isBomSwap(uint16_t ch) { return ch == UTF16_BOM_SWAP; }
  static FOG_INLINE uint16_t bswap(uint16_t ch) { return Memory::bswap16(ch); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief 16-bit character value.
  uint16_t _value;
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::Char, Fog::TYPEINFO_PRIMITIVE)

#endif // _FOG_CORE_TOOLS_CHAR_H
