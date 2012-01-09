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
  int32_t decpt;

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
  fog_api.stringutil_copyA(dst, src, length);
}

static FOG_INLINE void copy(uchar* dst, const uchar* src, size_t length)
{
  fog_api.stringutil_copyA(reinterpret_cast<char*>(dst), reinterpret_cast<const char*>(src), length);
}

static FOG_INLINE void copy(CharW* dst, const char* src, size_t length)
{
  fog_api.stringutil_unicodeFromLatin(dst, src, length);
}

static FOG_INLINE void copy(uint16_t* dst, const uchar* src, size_t length)
{
  fog_api.stringutil_unicodeFromLatin(reinterpret_cast<CharW*>(dst), reinterpret_cast<const char*>(src), length);
}

static FOG_INLINE void copy(CharW* dst, const CharW* src, size_t length)
{
  fog_api.stringutil_copyW(dst, src, length);
}

static FOG_INLINE void copy(uint16_t* dst, const uint16_t* src, size_t length)
{
  fog_api.stringutil_copyW(reinterpret_cast<CharW*>(dst), reinterpret_cast<const CharW*>(src), length);
}

// ============================================================================
// [Fog::StringUtil - UnicodeFromLatin / LatinFromUnicode]
// ============================================================================

static FOG_INLINE void unicodeFromLatin(CharW* dst, const char* src, size_t length)
{
  fog_api.stringutil_unicodeFromLatin(dst, src, length);
}

static FOG_INLINE void unicodeFromLatin(uint16_t* dst, const uchar* src, size_t length)
{
  fog_api.stringutil_unicodeFromLatin(reinterpret_cast<CharW*>(dst), reinterpret_cast<const char*>(src), length);
}

static FOG_INLINE err_t latinFromUnicode(char* dst, const CharW* src, size_t length)
{
  return fog_api.stringutil_latinFromUnicode(dst, src, length);
}

static FOG_INLINE err_t latinFromUnicode(uchar* dst, const uint16_t* src, size_t length)
{
  return fog_api.stringutil_latinFromUnicode(reinterpret_cast<char*>(dst), reinterpret_cast<const CharW*>(src), length);
}

// ============================================================================
// [Fog::StringUtil - Move]
// ============================================================================

static FOG_INLINE void move(char* dst, const char* src, size_t length)
{
  fog_api.stringutil_moveA(dst, src, length);
}

static FOG_INLINE void move(uchar* dst, const uchar* src, size_t length)
{
  fog_api.stringutil_moveA(reinterpret_cast<char*>(dst), reinterpret_cast<const char*>(src), length);
}

static FOG_INLINE void move(CharW* dst, const CharW* src, size_t length)
{
  fog_api.stringutil_moveW(dst, src, length);
}

static FOG_INLINE void move(uint16_t* dst, const uint16_t* src, size_t length)
{
  fog_api.stringutil_moveW(reinterpret_cast<CharW*>(dst), reinterpret_cast<const CharW*>(src), length);
}

// ============================================================================
// [Fog::StringUtil - Fill]
// ============================================================================

static FOG_INLINE void fill(char* dst, char ch, size_t length)
{
  fog_api.stringutil_fillA(dst, ch, length);
}

static FOG_INLINE void fill(uchar* dst, uchar ch, size_t length)
{
  fog_api.stringutil_fillA(reinterpret_cast<char*>(dst), ch, length);
}

static FOG_INLINE void fill(CharW* dst, CharW ch, size_t length)
{
  fog_api.stringutil_fillW(dst, ch._value, length);
}

static FOG_INLINE void fill(uint16_t* dst, uint16_t ch, size_t length)
{
  fog_api.stringutil_fillW(reinterpret_cast<CharW*>(dst), ch, length);
}

// ============================================================================
// [Fog::StringUtil - Len / NLen]
// ============================================================================

static FOG_INLINE size_t len(const char* src)
{
  return fog_api.stringutil_lenA(src);
}

static FOG_INLINE size_t len(const uchar* src)
{
  return fog_api.stringutil_lenA(reinterpret_cast<const char*>(src));
}

static FOG_INLINE size_t len(const CharW* src)
{
  return fog_api.stringutil_lenW(src);
}

static FOG_INLINE size_t len(const uint16_t* src)
{
  return fog_api.stringutil_lenW(reinterpret_cast<const CharW*>(src));
}

static FOG_INLINE size_t nlen(const char* src, size_t max)
{
  return fog_api.stringutil_nLenA(src, max);
}

static FOG_INLINE size_t nlen(const uchar* src, size_t max)
{
  return fog_api.stringutil_nLenA(reinterpret_cast<const char*>(src), max);
}

static FOG_INLINE size_t nlen(const CharW* src, size_t max)
{
  return fog_api.stringutil_nLenW(src, max);
}

static FOG_INLINE size_t nlen(const uint16_t* src, size_t max)
{
  return fog_api.stringutil_nLenW(reinterpret_cast<const CharW*>(src), max);
}

// ============================================================================
// [Fog::StringUtil - Equality]
// ============================================================================

static FOG_INLINE bool eq(const char* a, const char* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_eqA[cs](a, b, length);
}

static FOG_INLINE bool eq(const uchar* a, const uchar* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_eqA[cs](reinterpret_cast<const char*>(a), reinterpret_cast<const char*>(b), length);
}

static FOG_INLINE bool eq(const CharW* a, const CharW* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_eqW[cs](a, b, length);
}

static FOG_INLINE bool eq(const uint16_t* a, const uint16_t* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_eqW[cs](reinterpret_cast<const CharW*>(a), reinterpret_cast<const CharW*>(b), length);
}

static FOG_INLINE bool eq(const char* a, const CharW* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_eqMixed[cs](b, a, length);
}

static FOG_INLINE bool eq(const uchar* a, const uint16_t* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_eqMixed[cs](reinterpret_cast<const CharW*>(b), reinterpret_cast<const char*>(a), length);
}

static FOG_INLINE bool eq(const CharW* a, const char* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_eqMixed[cs](a, b, length);
}

static FOG_INLINE bool eq(const uint16_t* a, const uchar* b, size_t length, uint32_t cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_eqMixed[cs](reinterpret_cast<const CharW*>(a), reinterpret_cast<const char*>(b), length);
}

// ============================================================================
// [Fog::StringUtil - CountOf]
// ============================================================================

static FOG_INLINE size_t countOf(const char* str, size_t length, char ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_countOfA[cs](str, length, ch);
}

static FOG_INLINE size_t countOf(const uchar* str, size_t length, uchar ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_countOfA[cs](reinterpret_cast<const char*>(str), length, ch);
}

static FOG_INLINE size_t countOf(const CharW* str, size_t length, CharW ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_countOfW[cs](str, length, ch._value);
}

static FOG_INLINE size_t countOf(const uint16_t* str, size_t length, uint16_t ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_countOfW[cs](reinterpret_cast<const CharW*>(str), length, ch);
}

// ============================================================================
// [Fog::StringUtil - IndexOf]
// ============================================================================

static FOG_INLINE size_t indexOf(const char* str, size_t length, char ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfCharA[cs](str, length, ch);
}

static FOG_INLINE size_t indexOf(const uchar* str, size_t length, char ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfCharA[cs](reinterpret_cast<const char*>(str), length, ch);
}

static FOG_INLINE size_t indexOf(const CharW* str, size_t length, CharW ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfCharW[cs](str, length, ch._value);
}

static FOG_INLINE size_t indexOf(const uint16_t* str, size_t length, uint16_t ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfCharW[cs](reinterpret_cast<const CharW*>(str), length, ch);
}

static FOG_INLINE size_t indexOf(const char* aStr, size_t aLength, const char* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfStringA[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t indexOf(const uchar* aStr, size_t aLength, const uchar* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfStringA[cs](reinterpret_cast<const char*>(aStr), aLength, reinterpret_cast<const char*>(bStr), bLength);
}

static FOG_INLINE size_t indexOf(const CharW* aStr, size_t aLength, const char* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfStringWA[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t indexOf(const CharW* aStr, size_t aLength, const CharW* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfStringW[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t indexOf(const uint16_t* aStr, size_t aLength, const uchar* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfStringWA[cs](reinterpret_cast<const CharW*>(aStr), aLength, reinterpret_cast<const char*>(bStr), bLength);
}

static FOG_INLINE size_t indexOf(const uint16_t* aStr, size_t aLength, const uint16_t* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfStringW[cs](reinterpret_cast<const CharW*>(aStr), aLength, reinterpret_cast<const CharW*>(bStr), bLength);
}

static FOG_INLINE size_t indexOfAny(const char* str, size_t length, const char* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfAnyA[cs](str, length, charArray, charLength);
}

static FOG_INLINE size_t indexOfAny(const uchar* str, size_t length, const uchar* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfAnyA[cs](reinterpret_cast<const char*>(str), length, reinterpret_cast<const char*>(charArray), charLength);
}

static FOG_INLINE size_t indexOfAny(const CharW* str, size_t length, const CharW* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfAnyW[cs](str, length, charArray, charLength);
}

static FOG_INLINE size_t indexOfAny(const uint16_t* str, size_t length, const uint16_t* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_indexOfAnyW[cs](reinterpret_cast<const CharW*>(str), length, reinterpret_cast<const CharW*>(charArray), charLength);
}

// ============================================================================
// [Fog::StringUtil - LastIndexOf]
// ============================================================================

static FOG_INLINE size_t lastIndexOf(const char* str, size_t length, char ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfCharA[cs](str, length, ch);
}

static FOG_INLINE size_t lastIndexOf(const uchar* str, size_t length, uchar ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfCharA[cs](reinterpret_cast<const char*>(str), length, ch);
}

static FOG_INLINE size_t lastIndexOf(const CharW* str, size_t length, CharW ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfCharW[cs](str, length, ch._value);
}

static FOG_INLINE size_t lastIndexOf(const uint16_t* str, size_t length, uint16_t ch, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfCharW[cs](reinterpret_cast<const CharW*>(str), length, ch);
}

static FOG_INLINE size_t lastIndexOf(const char* aStr, size_t aLength, const char* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfStringA[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t lastIndexOf(const uchar* aStr, size_t aLength, const uchar* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfStringA[cs](reinterpret_cast<const char*>(aStr), aLength, reinterpret_cast<const char*>(bStr), bLength);
}

static FOG_INLINE size_t lastIndexOf(const CharW* aStr, size_t aLength, const char* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfStringWA[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t lastIndexOf(const CharW* aStr, size_t aLength, const CharW* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfStringW[cs](aStr, aLength, bStr, bLength);
}

static FOG_INLINE size_t lastIndexOf(const uint16_t* aStr, size_t aLength, const uchar* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfStringWA[cs](reinterpret_cast<const CharW*>(aStr), aLength, reinterpret_cast<const char*>(bStr), bLength);
}

static FOG_INLINE size_t lastIndexOf(const uint16_t* aStr, size_t aLength, const uint16_t* bStr, size_t bLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfStringW[cs](reinterpret_cast<const CharW*>(aStr), aLength, reinterpret_cast<const CharW*>(bStr), bLength);
}

static FOG_INLINE size_t lastIndexOfAny(const char* str, size_t length, const char* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfAnyA[cs](str, length, charArray, charLength);
}

static FOG_INLINE size_t lastIndexOfAny(const uchar* str, size_t length, const uchar* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfAnyA[cs](reinterpret_cast<const char*>(str), length, reinterpret_cast<const char*>(charArray), charLength);
}

static FOG_INLINE size_t lastIndexOfAny(const CharW* str, size_t length, const CharW* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfAnyW[cs](str, length, charArray, charLength);
}

static FOG_INLINE size_t lastIndexOfAny(const uint16_t* str, size_t length, const uint16_t* charArray, size_t charLength, uint cs = CASE_SENSITIVE)
{
  FOG_ASSERT(cs < CASE_SENSITIVITY_COUNT);
  return fog_api.stringutil_lastIndexOfAnyW[cs](reinterpret_cast<const CharW*>(str), length, reinterpret_cast<const CharW*>(charArray), charLength);
}

// ============================================================================
// [Fog::StringUtil - ValidateUtf8 / ValidateUtf16]
// ============================================================================

static FOG_INLINE err_t validateUtf8(const char* data, size_t length, size_t* invalid)
{
  return fog_api.stringutil_validateUtf8(data, length, invalid);
}

static FOG_INLINE err_t validateUtf8(const uchar* data, size_t length, size_t* invalid)
{
  return fog_api.stringutil_validateUtf8(reinterpret_cast<const char*>(data), length, invalid);
}

static FOG_INLINE err_t validateUtf16(const CharW* data, size_t length, size_t* invalid)
{
  return fog_api.stringutil_validateUtf16(data, length, invalid);
}

static FOG_INLINE err_t validateUtf16(const uint16_t* data, size_t length, size_t* invalid)
{
  return fog_api.stringutil_validateUtf16(reinterpret_cast<const CharW*>(data), length, invalid);
}

// ============================================================================
// [Fog::StringUtil - Canonication]
// ============================================================================

//! @brief Check whether the given string @a data of a given @a length is in
//! canonical form.
//!
//! This function expects that the string is decomposed, so it can check the
//! canonical ordering or combining characters. If the string is not decomposed
//! then the return value is likely to be @c true.
static FOG_INLINE bool isCanonical(const CharW* data, size_t length,
  uint32_t version = CHAR_UNICODE_VERSION_DEFAULT, size_t* where = NULL)
{
  return fog_api.stringutil_isCanonical(data, length, version, where);
}

//! @overload
static FOG_INLINE bool isCanonical(const uint16_t* data, size_t length,
  uint32_t version = CHAR_UNICODE_VERSION_DEFAULT, size_t* where = NULL)
{
  return fog_api.stringutil_isCanonical(reinterpret_cast<const CharW*>(data), length, version, where);
}

//! @brief Make the given string @a data of a given @a length canonical.
//!
//! This function expects that the string is decomposed, so it can check the
//! canonical ordering and fix it. If the string is not decomposed then the 
//! data won't be changed.
static FOG_INLINE void makeCanonical(CharW* data, size_t length,
  uint32_t version = CHAR_UNICODE_VERSION_DEFAULT, size_t from = 0)
{
  fog_api.stringutil_makeCanonical(data, length, version, from);
}

//! @overload
static FOG_INLINE void makeCanonical(uint16_t* data, size_t length,
  uint32_t version = CHAR_UNICODE_VERSION_DEFAULT, size_t from = 0)
{
  fog_api.stringutil_makeCanonical(reinterpret_cast<CharW*>(data), length, version, from);
}

// ============================================================================
// [Fog::StringUtil - Normalization]
// ============================================================================

static FOG_INLINE bool isNormalized(const CharW* data, size_t length, uint32_t normForm, uint32_t version, size_t* where)
{
  return fog_api.stringutil_isNormalized(data, length, normForm, version, where);
}

static FOG_INLINE bool isNormalized(const uint16_t* data, size_t length, uint32_t normForm, uint32_t version, size_t* where)
{
  return fog_api.stringutil_isNormalized(reinterpret_cast<const CharW*>(data), length, normForm, version, where);
}

static FOG_INLINE uint32_t quickCheck(const CharW* data, size_t length, uint32_t normForm, uint32_t version, size_t* where)
{
  return fog_api.stringutil_quickCheck(data, length, normForm, version, where);
}

static FOG_INLINE uint32_t quickCheck(const uint16_t* data, size_t length, uint32_t normForm, uint32_t version, size_t* where)
{
  return fog_api.stringutil_quickCheck(reinterpret_cast<const CharW*>(data), length, normForm, version, where);
}

// ============================================================================
// [Fog::StringUtil - UcsFromUtf8Length / UcsFromUtf16Length]
// ============================================================================

static FOG_INLINE err_t ucsFromUtf8Length(const char* data, size_t length, size_t* ucsLength)
{
  return fog_api.stringutil_ucsFromUtf8Length(data, length, ucsLength);
}

static FOG_INLINE err_t ucsFromUtf8Length(const uchar* data, size_t length, size_t* ucsLength)
{
  return fog_api.stringutil_ucsFromUtf8Length(reinterpret_cast<const char*>(data), length, ucsLength);
}

static FOG_INLINE err_t ucsFromUtf16Length(const CharW* data, size_t length, size_t* ucsLength)
{
  return fog_api.stringutil_ucsFromUtf16Length(data, length, ucsLength);
}

static FOG_INLINE err_t ucsFromUtf16Length(const uint16_t* data, size_t length, size_t* ucsLength)
{
  return fog_api.stringutil_ucsFromUtf16Length(reinterpret_cast<const CharW*>(data), length, ucsLength);
}

// ============================================================================
// [Fog::StringUtil - ITOA / UTOA / FTOA / DTOA]
// ============================================================================

static FOG_INLINE void itoa(NTOAContext* ctx, int64_t n, uint32_t base, uint32_t textCase = TEXT_CASE_UPPER)
{
  return fog_api.stringutil_itoa(ctx, n, base, textCase);
}

static FOG_INLINE void utoa(NTOAContext* ctx, uint64_t n, uint32_t base, uint32_t textCase = TEXT_CASE_UPPER)
{
  return fog_api.stringutil_utoa(ctx, n, base, textCase);
}

static FOG_INLINE void ftoa(NTOAContext* ctx, float d, uint32_t form, int nDigits)
{
  return fog_api.stringutil_dtoa(ctx, double(d), form, nDigits);
}

static FOG_INLINE void dtoa(NTOAContext* ctx, double d, uint32_t form, int nDigits)
{
  return fog_api.stringutil_dtoa(ctx, d, form, nDigits);
}

// ============================================================================
// [Fog::StringUtil - ParseBool]
// ============================================================================

static FOG_INLINE err_t parseBool(bool* dst, const char* str, size_t length, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseBoolA(dst, str, length, pEnd, pFlags);
}

static FOG_INLINE err_t parseBool(bool* dst, const CharW* str, size_t length, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseBoolW(dst, str, length, pEnd, pFlags);
}

// ============================================================================
// [Fog::StringUtil - ParseInt]
// ============================================================================

static FOG_INLINE err_t parseI8(int8_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseI8A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI8(int8_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseI8W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU8(uint8_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseU8A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU8(uint8_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseU8W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI16(int16_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseI16A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI16(int16_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseI16W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU16(uint16_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseU16A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU16(uint16_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseU16W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI32(int32_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseI32A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI32(int32_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseI32W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU32(uint32_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseU32A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU32(uint32_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseU32W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI64(int64_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseI64A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseI64(int64_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseI64W(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU64(uint64_t* dst, const char* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseU64A(dst, str, length, base, pEnd, pFlags);
}

static FOG_INLINE err_t parseU64(uint64_t* dst, const CharW* str, size_t length, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseU64W(dst, str, length, base, pEnd, pFlags);
}

// ============================================================================
// [Fog::StringUtil - ParseReal]
// ============================================================================

static FOG_INLINE err_t parseReal(float* dst,  const char* str, size_t length, char decimalPoint = '.', size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseFloatA(dst, str, length, decimalPoint, pEnd, pFlags);
}

static FOG_INLINE err_t parseReal(float* dst, const CharW* str, size_t length, CharW decimalPoint = CharW('.'), size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseFloatW(dst, str, length, decimalPoint, pEnd, pFlags);
}

static FOG_INLINE err_t parseReal(double* dst, const char* str, size_t length, char decimalPoint = '.', size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseDoubleA(dst, str, length, decimalPoint, pEnd, pFlags);
}

static FOG_INLINE err_t parseReal(double* dst, const CharW* str, size_t length, CharW decimalPoint = CharW('.'), size_t* pEnd = NULL, uint32_t* pFlags = NULL)
{
  return fog_api.stringutil_parseDoubleW(dst, str, length, decimalPoint, pEnd, pFlags);
}

//! @}

} // StringUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_STRINGUTIL_H
