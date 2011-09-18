// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_STRINGUTIL_H
#define _FOG_CORE_TOOLS_STRINGUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/Char.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::NTOAContext]
// ============================================================================

struct NTOAContext
{
  //! @brief Output (pointer to non-null terminated string in @c buffer).
  char* result;
  //! @brief Output length.
  uint32_t length;
  //! @brief Output flags.
  uint32_t negative;
  //! @brief Decimal point position (for dtoa).
  uint32_t decpt;

  //! @brief Output buffer.
  char buffer[256];
};

//! @}

}

namespace Fog {
namespace StringUtil {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::StringUtil - Copy]
// ============================================================================

static FOG_INLINE void copy(char* dst, const char* src, size_t length)
{
  _api.stringutil.copyA(dst, src, length);
}

static FOG_INLINE void copy(uchar* dst, const uchar* src, size_t length)
{
  _api.stringutil.copyA(reinterpret_cast<char*>(dst), reinterpret_cast<const char*>(src), length);
}

static FOG_INLINE void copy(CharW* dst, const char* src, size_t length)
{
  _api.stringutil.unicodeFromLatin(dst, src, length);
}

static FOG_INLINE void copy(uint16_t* dst, const uchar* src, size_t length)
{
  _api.stringutil.unicodeFromLatin(reinterpret_cast<CharW*>(dst), reinterpret_cast<const char*>(src), length);
}

static FOG_INLINE void copy(CharW* dst, const CharW* src, size_t length)
{
  _api.stringutil.copyW(dst, src, length);
}

static FOG_INLINE void copy(uint16_t* dst, const uint16_t* src, size_t length)
{
  _api.stringutil.copyW(reinterpret_cast<CharW*>(dst), reinterpret_cast<const CharW*>(src), length);
}

// ============================================================================
// [Fog::StringUtil - UnicodeFromLatin / LatinFromUnicode]
// ============================================================================

static FOG_INLINE void unicodeFromLatin(CharW* dst, const char* src, size_t length)
{
  _api.stringutil.unicodeFromLatin(dst, src, length);
}

static FOG_INLINE void unicodeFromLatin(uint16_t* dst, const uchar* src, size_t length)
{
  _api.stringutil.unicodeFromLatin(reinterpret_cast<CharW*>(dst), reinterpret_cast<const char*>(src), length);
}

static FOG_INLINE err_t latinFromUnicode(char* dst, const CharW* src, size_t length)
{
  return _api.stringutil.latinFromUnicode(dst, src, length);
}

static FOG_INLINE err_t latinFromUnicode(uchar* dst, const uint16_t* src, size_t length)
{
  return _api.stringutil.latinFromUnicode(reinterpret_cast<char*>(dst), reinterpret_cast<const CharW*>(src), length);
}

// ============================================================================
// [Fog::StringUtil - Move]
// ============================================================================

static FOG_INLINE void move(char* dst, const char* src, size_t length)
{
  _api.stringutil.moveA(dst, src, length);
}

static FOG_INLINE void move(uchar* dst, const uchar* src, size_t length)
{
  _api.stringutil.moveA(reinterpret_cast<char*>(dst), reinterpret_cast<const char*>(src), length);
}

static FOG_INLINE void move(CharW* dst, const CharW* src, size_t length)
{
  _api.stringutil.moveW(dst, src, length);
}

static FOG_INLINE void move(uint16_t* dst, const uint16_t* src, size_t length)
{
  _api.stringutil.moveW(reinterpret_cast<CharW*>(dst), reinterpret_cast<const CharW*>(src), length);
}

// ============================================================================
// [Fog::StringUtil - Fill]
// ============================================================================

static FOG_INLINE void fill(char* dst, char ch, size_t length)
{
  _api.stringutil.fillA(dst, ch, length);
}

static FOG_INLINE void fill(uchar* dst, uchar ch, size_t length)
{
  _api.stringutil.fillA(reinterpret_cast<char*>(dst), ch, length);
}

static FOG_INLINE void fill(CharW* dst, CharW ch, size_t length)
{
  _api.stringutil.fillW(dst, ch._value, length);
}

static FOG_INLINE void fill(uint16_t* dst, uint16_t ch, size_t length)
{
  _api.stringutil.fillW(reinterpret_cast<CharW*>(dst), ch, length);
}

// ============================================================================
// [Fog::StringUtil - Len / NLen]
// ============================================================================

static FOG_INLINE size_t len(const char* src)
{
  return _api.stringutil.lenA(src);
}

static FOG_INLINE size_t len(const uchar* src)
{
  return _api.stringutil.lenA(reinterpret_cast<const char*>(src));
}

static FOG_INLINE size_t len(const CharW* src)
{
  return _api.stringutil.lenW(src);
}

static FOG_INLINE size_t len(const uint16_t* src)
{
  return _api.stringutil.lenW(reinterpret_cast<const CharW*>(src));
}

static FOG_INLINE size_t nlen(const char* src, size_t max)
{
  return _api.stringutil.nLenA(src, max);
}

static FOG_INLINE size_t nlen(const uchar* src, size_t max)
{
  return _api.stringutil.nLenA(reinterpret_cast<const char*>(src), max);
}

static FOG_INLINE size_t nlen(const CharW* src, size_t max)
{
  return _api.stringutil.nLenW(src, max);
}

static FOG_INLINE size_t nlen(const uint16_t* src, size_t max)
{
  return _api.stringutil.nLenW(reinterpret_cast<const CharW*>(src), max);
}

// ============================================================================
// [Fog::StringUtil - Equality]
// ============================================================================

static FOG_INLINE bool eq(const char* a, const char* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.eqA[cs](a, b, length);
}

static FOG_INLINE bool eq(const uchar* a, const uchar* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.eqA[cs](reinterpret_cast<const char*>(a), reinterpret_cast<const char*>(b), length);
}

static FOG_INLINE bool eq(const CharW* a, const CharW* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.eqW[cs](a, b, length);
}

static FOG_INLINE bool eq(const uint16_t* a, const uint16_t* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.eqW[cs](reinterpret_cast<const CharW*>(a), reinterpret_cast<const CharW*>(b), length);
}

static FOG_INLINE bool eq(const char* a, const CharW* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.eqMixed[cs](b, a, length);
}

static FOG_INLINE bool eq(const uchar* a, const uint16_t* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.eqMixed[cs](reinterpret_cast<const CharW*>(b), reinterpret_cast<const char*>(a), length);
}

static FOG_INLINE bool eq(const CharW* a, const char* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.eqMixed[cs](a, b, length);
}

static FOG_INLINE bool eq(const uint16_t* a, const uchar* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.eqMixed[cs](reinterpret_cast<const CharW*>(a), reinterpret_cast<const char*>(b), length);
}

// ============================================================================
// [Fog::StringUtil - CountOf]
// ============================================================================

static FOG_INLINE size_t countOf(const char* str, size_t length, char ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.countOfA[cs](str, length, ch);
}

static FOG_INLINE size_t countOf(const uchar* str, size_t length, uchar ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.countOfA[cs](reinterpret_cast<const char*>(str), length, ch);
}

static FOG_INLINE size_t countOf(const CharW* str, size_t length, CharW ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.countOfW[cs](str, length, ch._value);
}

static FOG_INLINE size_t countOf(const uint16_t* str, size_t length, uint16_t ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.countOfW[cs](reinterpret_cast<const CharW*>(str), length, ch);
}

// ============================================================================
// [Fog::StringUtil - IndexOf]
// ============================================================================

static FOG_INLINE size_t indexOf(const char* str, size_t length, char ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfCharA[cs](str, length, ch);
}

static FOG_INLINE size_t indexOf(const uchar* str, size_t length, char ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfCharA[cs](reinterpret_cast<const char*>(str), length, ch);
}

static FOG_INLINE size_t indexOf(const CharW* str, size_t length, CharW ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfCharW[cs](str, length, ch._value);
}

static FOG_INLINE size_t indexOf(const uint16_t* str, size_t length, uint16_t ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfCharW[cs](reinterpret_cast<const CharW*>(str), length, ch);
}

static FOG_INLINE size_t indexOf(const char* aStr, size_t aLength, const char* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfStringA[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t indexOf(const uchar* aStr, size_t aLength, const uchar* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfStringA[cs](reinterpret_cast<const char*>(aStr), aLength, reinterpret_cast<const char*>(bStr), bLength);
}

static FOG_INLINE size_t indexOf(const CharW* aStr, size_t aLength, const char* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfStringWA[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t indexOf(const CharW* aStr, size_t aLength, const CharW* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfStringW[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t indexOf(const uint16_t* aStr, size_t aLength, const uchar* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfStringWA[cs](reinterpret_cast<const CharW*>(aStr), aLength, reinterpret_cast<const char*>(bStr), bLength);
}

static FOG_INLINE size_t indexOf(const uint16_t* aStr, size_t aLength, const uint16_t* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfStringW[cs](reinterpret_cast<const CharW*>(aStr), aLength, reinterpret_cast<const CharW*>(bStr), bLength);
}

static FOG_INLINE size_t indexOfAny(const char* str, size_t length, const char* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfAnyA[cs](str, length, charArray, charLength);
}

static FOG_INLINE size_t indexOfAny(const uchar* str, size_t length, const uchar* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfAnyA[cs](reinterpret_cast<const char*>(str), length, reinterpret_cast<const char*>(charArray), charLength);
}

static FOG_INLINE size_t indexOfAny(const CharW* str, size_t length, const CharW* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfAnyW[cs](str, length, charArray, charLength);
}

static FOG_INLINE size_t indexOfAny(const uint16_t* str, size_t length, const uint16_t* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.indexOfAnyW[cs](reinterpret_cast<const CharW*>(str), length, reinterpret_cast<const CharW*>(charArray), charLength);
}

// ============================================================================
// [Fog::StringUtil - LastIndexOf]
// ============================================================================

static FOG_INLINE size_t lastIndexOf(const char* str, size_t length, char ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfCharA[cs](str, length, ch);
}

static FOG_INLINE size_t lastIndexOf(const uchar* str, size_t length, uchar ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfCharA[cs](reinterpret_cast<const char*>(str), length, ch);
}

static FOG_INLINE size_t lastIndexOf(const CharW* str, size_t length, CharW ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfCharW[cs](str, length, ch._value);
}

static FOG_INLINE size_t lastIndexOf(const uint16_t* str, size_t length, uint16_t ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfCharW[cs](reinterpret_cast<const CharW*>(str), length, ch);
}

static FOG_INLINE size_t lastIndexOf(const char* aStr, size_t aLength, const char* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfStringA[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t lastIndexOf(const uchar* aStr, size_t aLength, const uchar* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfStringA[cs](reinterpret_cast<const char*>(aStr), aLength, reinterpret_cast<const char*>(bStr), bLength);
}

static FOG_INLINE size_t lastIndexOf(const CharW* aStr, size_t aLength, const char* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfStringWA[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t lastIndexOf(const CharW* aStr, size_t aLength, const CharW* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfStringW[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t lastIndexOf(const uint16_t* aStr, size_t aLength, const uchar* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfStringWA[cs](reinterpret_cast<const CharW*>(aStr), aLength, reinterpret_cast<const char*>(bStr), bLength);
}

static FOG_INLINE size_t lastIndexOf(const uint16_t* aStr, size_t aLength, const uint16_t* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfStringW[cs](reinterpret_cast<const CharW*>(aStr), aLength, reinterpret_cast<const CharW*>(bStr), bLength);
}

static FOG_INLINE size_t lastIndexOfAny(const char* str, size_t length, const char* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfAnyA[cs](str, length, charArray, charLength);
}

static FOG_INLINE size_t lastIndexOfAny(const uchar* str, size_t length, const uchar* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfAnyA[cs](reinterpret_cast<const char*>(str), length, reinterpret_cast<const char*>(charArray), charLength);
}

static FOG_INLINE size_t lastIndexOfAny(const CharW* str, size_t length, const CharW* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfAnyW[cs](str, length, charArray, charLength);
}

static FOG_INLINE size_t lastIndexOfAny(const uint16_t* str, size_t length, const uint16_t* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return _api.stringutil.lastIndexOfAnyW[cs](reinterpret_cast<const CharW*>(str), length, reinterpret_cast<const CharW*>(charArray), charLength);
}

// ============================================================================
// [Fog::StringUtil - ValidateUtf8 / ValidateUtf16]
// ============================================================================

static FOG_INLINE err_t validateUtf8(const char* data, size_t length, size_t* invalid)
{
  return _api.stringutil.validateUtf8(data, length, invalid);
}

static FOG_INLINE err_t validateUtf8(const uchar* data, size_t length, size_t* invalid)
{
  return _api.stringutil.validateUtf8(reinterpret_cast<const char*>(data), length, invalid);
}

static FOG_INLINE err_t validateUtf16(const CharW* data, size_t length, size_t* invalid)
{
  return _api.stringutil.validateUtf16(data, length, invalid);
}

static FOG_INLINE err_t validateUtf16(const uint16_t* data, size_t length, size_t* invalid)
{
  return _api.stringutil.validateUtf16(reinterpret_cast<const CharW*>(data), length, invalid);
}

// ============================================================================
// [Fog::StringUtil - UcsFromUtf8Length / UcsFromUtf16Length]
// ============================================================================

static FOG_INLINE err_t ucsFromUtf8Length(const char* data, size_t length, size_t* ucsLength)
{
  return _api.stringutil.ucsFromUtf8Length(data, length, ucsLength);
}

static FOG_INLINE err_t ucsFromUtf8Length(const uchar* data, size_t length, size_t* ucsLength)
{
  return _api.stringutil.ucsFromUtf8Length(reinterpret_cast<const char*>(data), length, ucsLength);
}

static FOG_INLINE err_t ucsFromUtf16Length(const CharW* data, size_t length, size_t* ucsLength)
{
  return _api.stringutil.ucsFromUtf16Length(data, length, ucsLength);
}

static FOG_INLINE err_t ucsFromUtf16Length(const uint16_t* data, size_t length, size_t* ucsLength)
{
  return _api.stringutil.ucsFromUtf16Length(reinterpret_cast<const CharW*>(data), length, ucsLength);
}

// ============================================================================
// [Fog::StringUtil - ITOA / UTOA / FTOA / DTOA]
// ============================================================================

static FOG_INLINE void itoa(NTOAContext* ctx, int64_t n, uint32_t base, uint32_t textCase = TEXT_CASE_UPPER)
{
  return _api.stringutil.itoa(ctx, n, base, textCase);
}

static FOG_INLINE void utoa(NTOAContext* ctx, uint64_t n, uint32_t base, uint32_t textCase = TEXT_CASE_UPPER)
{
  return _api.stringutil.utoa(ctx, n, base, textCase);
}

static FOG_INLINE void ftoa(NTOAContext* ctx, float d, uint32_t form, int nDigits)
{
  return _api.stringutil.dtoa(ctx, double(d), form, nDigits);
}

static FOG_INLINE void dtoa(NTOAContext* ctx, double d, uint32_t form, int nDigits)
{
  return _api.stringutil.dtoa(ctx, d, form, nDigits);
}

// ============================================================================
// [Fog::StringUtil - ParseBool]
// ============================================================================

static FOG_INLINE err_t parseBool(bool* dst, const char* str, size_t length, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseBoolA(dst, str, length, pEnd, pFlags);
}

static FOG_INLINE err_t parseBool(bool* dst, const CharW* str, size_t length, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseBoolW(dst, str, length, pEnd, pFlags);
}

// ============================================================================
// [Fog::StringUtil - ParseInt]
// ============================================================================

static FOG_INLINE err_t parseI8(int8_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseI8A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI8(int8_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseI8W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU8(uint8_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseU8A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU8(uint8_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseU8W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI16(int16_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseI16A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI16(int16_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseI16W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU16(uint16_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseU16A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU16(uint16_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseU16W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI32(int32_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseI32A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI32(int32_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseI32W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU32(uint32_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseU32A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU32(uint32_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseU32W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI64(int64_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseI64A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI64(int64_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseI64W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU64(uint64_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseU64A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU64(uint64_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseU64W(dst, str, length, base, pEnd, pFlags);
}

// ============================================================================
// [Fog::StringUtil - ParseReal]
// ============================================================================

static FOG_INLINE err_t parseReal(float* dst,  const char* str, size_t length, char decimalPoint = '.', size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseFloatA(dst, str, length, decimalPoint, pEnd, pFlags);
}

static FOG_INLINE err_t parseReal(float* dst, const CharW* str, size_t length, CharW decimalPoint = CharW('.'), size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseFloatW(dst, str, length, decimalPoint, pEnd, pFlags);
}

static FOG_INLINE err_t parseReal(double* dst, const char* str, size_t length, char decimalPoint = '.', size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseDoubleA(dst, str, length, decimalPoint, pEnd, pFlags);
}

static FOG_INLINE err_t parseReal(double* dst, const CharW* str, size_t length, CharW decimalPoint = CharW('.'), size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return _api.stringutil.parseDoubleW(dst, str, length, decimalPoint, pEnd, pFlags);
}

//! @}

} // StringUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_STRINGUTIL_H
