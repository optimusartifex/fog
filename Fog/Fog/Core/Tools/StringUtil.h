// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_STRINGUTIL_H
#define _FOG_CORE_TOOLS_STRINGUTIL_H

// [Dependencies]
#include <Fog/Core/Tools/Char.h>

#include <string.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::StringUtil::Forward Declarations]
// ============================================================================

struct ByteArray;
struct String;

namespace StringUtil {

// ============================================================================
// [Fog::StringUtil::ParserFlags]
// ============================================================================

// TODO: Move to constants.
enum PARSER_FLAGS
{
  //! @brief Some spaces was parsed and skipped in input.
  PARSED_SPACES = (1 << 1),
  //! @brief Sign was parsed.
  PARSED_SIGN = (1 << 2),
  //! @brief @c "0" octal prefix was parsed.
  PARSED_OCTAL_PREFIX = (1 << 3),
  //! @brief @c "0x" or @c "0X" hexadecimal prefix was parsed.
  PARSED_HEX_PREFIX = (1 << 4),
  //! @brief Decimal point was parsed.
  PARSED_DECIMAL_POINT = (1 << 5),
  //! @brief Exponent (@c "E" or @c "e") was parsed.
  PARSED_EXPONENT = (1 << 6)
};

// ============================================================================
// [Fog::StringUtil::Unicode Tools]
// ============================================================================

FOG_API err_t validateUtf8(const char* str, sysuint_t len, sysuint_t* invalidPos = NULL);
FOG_API err_t validateUtf16(const Char* str, sysuint_t len, sysuint_t* invalidPos = NULL);

FOG_API err_t getNumUtf8Chars(const char* str, sysuint_t len, sysuint_t* charsCount);
FOG_API err_t getNumUtf16Chars(const Char* str, sysuint_t len, sysuint_t* charsCount);

FOG_API bool unicodeToLatin1(char* dst, const Char* src, sysuint_t length);

// ============================================================================
// [Fog::StringUtil::Mem]
// ============================================================================

FOG_INLINE void copy(char* dst, const char* src, sysuint_t length) { ::memcpy(dst, src, length); }
FOG_INLINE void move(char* dst, const char* src, sysuint_t length) { ::memmove(dst, src, length); }
FOG_INLINE void fill(char* dst, char ch, sysuint_t length) { ::memset(dst, (uint8_t)ch, length); }

FOG_INLINE sysuint_t len(const char* s) { return ::strlen(s); }

#if defined(__MINGW32__) || defined(FOG_OS_MAC)
FOG_INLINE sysuint_t nlen(const char* str, sysuint_t maxlen)
{
  const char* p = str;
  if (FOG_IS_NULL(p)) return 0;

  const char* end = str + maxlen;
  while (p < end && *p) p++;
  return (sysuint_t)(p - str);
}
#else
FOG_INLINE sysuint_t nlen(const char* str, sysuint_t maxlen)
{
  // Compiler built-ins should be always better than ours.
  return ::strnlen(str, maxlen);
}
#endif

FOG_API void copy(Char* dst, const Char* src, sysuint_t length);
FOG_API void copy(Char* dst, const char* src, sysuint_t length);
FOG_API void move(Char* dst, const Char* src, sysuint_t length);
FOG_API void fill(Char* dst, Char ch, sysuint_t length);

FOG_API sysuint_t len(const Char* str);
FOG_API sysuint_t nlen(const Char* str, sysuint_t maxlen);

FOG_API sysuint_t len(const uint32_t* str);
FOG_API sysuint_t nlen(const uint32_t* str, sysuint_t maxlen);

FOG_API bool eq(const char* a, const char* b, sysuint_t length, uint cs = CASE_SENSITIVE);
FOG_API bool eq(const Char* a, const Char* b, sysuint_t length, uint cs = CASE_SENSITIVE);
FOG_API bool eq(const Char* a, const char* b, sysuint_t length, uint cs = CASE_SENSITIVE);

FOG_API sysuint_t countOf(const char* str, sysuint_t length, char ch, uint cs = CASE_SENSITIVE);
FOG_API sysuint_t countOf(const Char* str, sysuint_t length, Char ch, uint cs = CASE_SENSITIVE);

FOG_API sysuint_t indexOf(const char* str, sysuint_t length, char ch, uint cs = CASE_SENSITIVE);
FOG_API sysuint_t indexOf(const Char* str, sysuint_t length, Char ch, uint cs = CASE_SENSITIVE);

FOG_API sysuint_t indexOf(const char* aStr, sysuint_t aLength, const char* bStr, sysuint_t bLength, uint cs = CASE_SENSITIVE);
FOG_API sysuint_t indexOf(const Char* aStr, sysuint_t aLength, const Char* bStr, sysuint_t bLength, uint cs = CASE_SENSITIVE);

FOG_API sysuint_t indexOfAny(const char* str, sysuint_t length, const char* ch, sysuint_t count, uint cs = CASE_SENSITIVE);
FOG_API sysuint_t indexOfAny(const Char* str, sysuint_t length, const Char* ch, sysuint_t count, uint cs = CASE_SENSITIVE);

FOG_API sysuint_t lastIndexOf(const char* str, sysuint_t length, char ch, uint cs = CASE_SENSITIVE);
FOG_API sysuint_t lastIndexOf(const Char* str, sysuint_t length, Char ch, uint cs = CASE_SENSITIVE);

FOG_API sysuint_t lastIndexOfAny(const char* str, sysuint_t length, const char* ch, sysuint_t count, uint cs = CASE_SENSITIVE);
FOG_API sysuint_t lastIndexOfAny(const Char* str, sysuint_t length, const Char* ch, sysuint_t count, uint cs = CASE_SENSITIVE);

// ============================================================================
// [Fog::StringUtil::NTOA]
// ============================================================================

struct NTOAOut
{
  //! @brief Output (pointer to non null terminated string in @c buf).
  char* result;
  //! @brief Output length.
  sysuint_t length;
  //! @brief Decimal point position (for dtoa).
  uint32_t decpt;
  //! @brief output flags.
  uint32_t negative;
  //! @brief Output buffer that must fit the largest possible result.
  char buffer[256];
};

FOG_API void itoa(int64_t n, int base, bool uppercase, NTOAOut* out);
FOG_API void utoa(uint64_t n, int base, bool uppercase, NTOAOut* out);
FOG_API void dtoa(double d, int mode, int ndigits, NTOAOut* out);

// ============================================================================
// [Fog::StringUtil::Ato[b|i|u|f|d]]
// ============================================================================

FOG_API err_t atob(const char* str, sysuint_t length, bool* dst, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);

FOG_API err_t atoi8(const char* str, sysuint_t length, int8_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atou8(const char* str, sysuint_t length, uint8_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atoi16(const char* str, sysuint_t length, int16_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atou16(const char* str, sysuint_t length, uint16_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atoi32(const char* str, sysuint_t length, int32_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atou32(const char* str, sysuint_t length, uint32_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atoi64(const char* str, sysuint_t length, int64_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atou64(const char* str, sysuint_t length, uint64_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);

FOG_API err_t atof(const char* str, sysuint_t length, float* dst, char decimalPoint = '.', sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atod(const char* str, sysuint_t length, double* dst, char decimalPoint = '.', sysuint_t* end = NULL, uint32_t* parserFlags = NULL);

FOG_API err_t atob(const Char* str, sysuint_t length, bool* dst, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);

FOG_API err_t atoi8(const Char* str, sysuint_t length, int8_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atou8(const Char* str, sysuint_t length, uint8_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atoi16(const Char* str, sysuint_t length, int16_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atou16(const Char* str, sysuint_t length, uint16_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atoi32(const Char* str, sysuint_t length, int32_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atou32(const Char* str, sysuint_t length, uint32_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atoi64(const Char* str, sysuint_t length, int64_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atou64(const Char* str, sysuint_t length, uint64_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL);

FOG_API err_t atof(const Char* str, sysuint_t length, float* dst, Char decimalPoint = Char('.'), sysuint_t* end = NULL, uint32_t* parserFlags = NULL);
FOG_API err_t atod(const Char* str, sysuint_t length, double* dst, Char decimalPoint = Char('.'), sysuint_t* end = NULL, uint32_t* parserFlags = NULL);

// ============================================================================
// [Fog::StringUtil::Hex / Base64]
// ============================================================================

FOG_API err_t fromHex(ByteArray& dst, const ByteArray& src, uint32_t cntOp = CONTAINER_OP_REPLACE);
FOG_API err_t toHex(ByteArray& dst, const ByteArray& src, uint32_t cntOp = CONTAINER_OP_REPLACE, int outputCase = OUTPUT_CASE_UPPER);

FOG_API err_t fromBase64(ByteArray& dst, const ByteArray& src, uint32_t cntOp = CONTAINER_OP_REPLACE);
FOG_API err_t fromBase64(ByteArray& dst, const String& src, uint32_t cntOp = CONTAINER_OP_REPLACE);

FOG_API err_t fromBase64(ByteArray& dst, const char* src, sysuint_t srcLength, uint32_t cntOp = CONTAINER_OP_REPLACE);
FOG_API err_t fromBase64(ByteArray& dst, const Char* src, sysuint_t srcLength, uint32_t cntOp = CONTAINER_OP_REPLACE);

FOG_API err_t toBase64(ByteArray& dst, const ByteArray& src, uint32_t cntOp = CONTAINER_OP_REPLACE);
FOG_API err_t toBase64(String& dst, const ByteArray& src, uint32_t cntOp = CONTAINER_OP_REPLACE);

FOG_API err_t toBase64(ByteArray& dst, const char* src, sysuint_t srcLength, uint32_t cntOp = CONTAINER_OP_REPLACE);
FOG_API err_t toBase64(String& dst, const char* src, sysuint_t srcLength, uint32_t cntOp = CONTAINER_OP_REPLACE);

//! @}

} // StringUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_STRINGUTIL_H
