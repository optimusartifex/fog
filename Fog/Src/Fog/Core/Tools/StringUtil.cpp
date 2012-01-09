// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/CharData.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>

namespace Fog {

// ============================================================================
// [Fog::StringUtil - Copy]
// ============================================================================

template<typename CharT>
static void FOG_CDECL StringUtil_copy(CharT* dst, const CharT* src, size_t length)
{
  for (size_t i = 0; i < length; i++)
    dst[i] = src[i];
}

// ============================================================================
// [Fog::StringUtil - LatinFromUnicode / UnicodeFromLatin]
// ============================================================================

static err_t FOG_CDECL StringUtil_latinFromUnicode(char* dst, const CharW* src, size_t length)
{
  err_t err = ERR_OK;

  for (size_t i = 0; i < length; i++)
  {
    uint16_t uc = src[i];

    if (uc > 255)
    {
      uc = '?';
      err = ERR_STRING_LOST;
    }

    dst[i] = (uint8_t)uc;
  }

  return err;
}

static void FOG_CDECL StringUtil_unicodeFromLatin(CharW* dst, const char* src, size_t length)
{
  for (size_t i = 0; i < length; i++)
    dst[i] = src[i];
}

// ============================================================================
// [Fog::StringUtil - Move]
// ============================================================================

template<typename CharT>
static void FOG_CDECL StringUtil_move(CharT* dst, const CharT* src, size_t length)
{
  size_t i;

  if (dst <= src)
  {
    for (i = 0; i < length; i++)
      dst[i] = src[i];
  }
  else
  {
    i = length;
    while (i)
    {
      i--;
      dst[i] = src[i];
    }
  }
}

// ============================================================================
// [Fog::StringUtil - Fill]
// ============================================================================

template<typename CharT>
static void FOG_CDECL StringUtil_fill(CharT* dst, CharT_Char::Type ch, size_t length)
{
  for (size_t i = 0; i < length; i++)
    dst[i] = ch;
}

// ============================================================================
// [Fog::StringUtil - Len / NLen]
// ============================================================================

template<typename CharT>
static size_t FOG_CDECL StringUtil_len(const CharT* src)
{
  size_t i;
  for (i = 0; src[i] != 0; i++)
    continue;
  return i;
}

template<typename CharT>
static size_t FOG_CDECL StringUtil_nLen(const CharT* src, size_t max)
{
  size_t i;

  if (FOG_IS_NULL(src))
    return 0;

  for (i = 0; i < max; i++)
    if (src[i] == 0)
      break;

  return i;
}

// ============================================================================
// [Fog::StringUtil - Equality]
// ============================================================================

static bool FOG_CDECL StringUtil_eqA_cs(const char* a, const char* b, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    if (a[i] != b[i])
      return false;
  }
  return true;
}

static bool FOG_CDECL StringUtil_eqA_ci(const char* a, const char* b, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    if (CharA::toLower(a[i]) != CharA::toLower(b[i]))
      return false;
  }
  return true;
}

static bool FOG_CDECL StringUtil_eqW_cs(const CharW* a, const CharW* b, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    if (a[i] != b[i])
      return false;
  }
  return true;
}

static bool FOG_CDECL StringUtil_eqW_ci(const CharW* a, const CharW* b, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    if (CharW::toLower(a[i]) != CharW::toLower(b[i]))
      return false;
  }
  return true;
}

static bool FOG_CDECL StringUtil_eqMixed_cs(const CharW* a, const char* b, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    if (a[i] != b[i])
      return false;
  }
  return true;
}

static bool FOG_CDECL StringUtil_eqMixed_ci(const CharW* a, const char* b, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    if (CharW::toLower(a[i]) != CharA::toLower((unsigned char)b[i]))
      return false;
  }
  return true;
}

// ============================================================================
// [Fog::StringUtil - CountOf]
// ============================================================================

static size_t FOG_CDECL StringUtil_countOfA_cs(const char* str, size_t length, char ch)
{
  size_t n = 0;

  for (size_t i = 0; i < length; i++)
  {
    n += (str[i] == ch);
  }

  return n;
}

static size_t FOG_CDECL StringUtil_countOfA_ci(const char* str, size_t length, char ch)
{
  size_t n = 0;

  char cLower = CharA::toLower(ch);
  char cUpper = CharA::toUpper(ch);

  if (cLower == cUpper)
    return StringUtil::countOf(str, length, ch, CASE_SENSITIVE);

  for (size_t i = 0; i < length; i++)
  {
    n += (str[i] == cLower);
    n += (str[i] == cUpper);
  }

  return n;
}

static size_t FOG_CDECL StringUtil_countOfW_cs(const CharW* str, size_t length, uint16_t ch)
{
  size_t n = 0;

  for (size_t i = 0; i < length; i++)
  {
    n += str[i] == ch;
  }

  return n;
}

static size_t FOG_CDECL StringUtil_countOfW_ci(const CharW* str, size_t length, uint16_t ch)
{
  size_t n = 0;

  uint16_t cLower = CharW::toLower(ch);
  uint16_t cUpper = CharW::toUpper(ch);

  if (cLower == cUpper)
    return StringUtil::countOf(str, length, CharW(ch), CASE_SENSITIVE);

  for (size_t i = 0; i < length; i++)
  {
    n += (CharW::toLower(str[i]) == cLower);
  }

  return n;
}

// ============================================================================
// [Fog::StringUtil - CharMap (Helpers)]
// ============================================================================

static void FOG_INLINE CharMapA_build_cs(size_t* map, const char* charArray, size_t charLength)
{
  size_t i;

  for (i = 0; i < (256 / (sizeof(size_t) * 8)); i++)
  {
    map[i] = 0;
  }

  for (i = 0; i < charLength; i++)
  {
    size_t c = (unsigned char)charArray[i];
    size_t cIdx = c / (sizeof(size_t) * 8);
    size_t cBit = c % (sizeof(size_t) * 8);

    map[cIdx] |= ((size_t)1 << cBit);
  }
}

static void FOG_INLINE CharMapA_build_ci(size_t* map, const char* charArray, size_t charLength)
{
  size_t i;

  for (i = 0; i < (256 / (sizeof(size_t) * 8)); i++)
  {
    map[i] = 0;
  }

  for (i = 0; i < charLength; i++)
  {
    size_t cLower = (unsigned char)CharA::toLower(charArray[i]);
    size_t cUpper = (unsigned char)CharA::toUpper(charArray[i]);

    size_t cIdx;
    size_t cBit;

    cIdx = cLower / (sizeof(size_t) * 8);
    cBit = cLower % (sizeof(size_t) * 8);
    map[cIdx] |= ((size_t)1 << cBit);

    cIdx = cUpper / (sizeof(size_t) * 8);
    cBit = cUpper % (sizeof(size_t) * 8);
    map[cIdx] |= ((size_t)1 << cBit);
  }
}

static size_t FOG_INLINE CharMapA_indexOf(const size_t* map, const char* str, size_t length)
{
  size_t i;

  for (i = 0; i < length; i++)
  {
    size_t c = (unsigned char)str[i];
    size_t cIdx = c / (sizeof(size_t) * 8);
    size_t cBit = c % (sizeof(size_t) * 8);

    if (map[cIdx] & ((size_t)1 << cBit))
      return i;
  }

  return INVALID_INDEX;
}

static size_t FOG_INLINE CharMapA_lastIndexOf(const size_t* map, const char* str, size_t length)
{
  size_t i = length;

  while (i)
  {
    size_t c = (unsigned char)str[--i];
    size_t cIdx = c / (sizeof(size_t) * 8);
    size_t cBit = c % (sizeof(size_t) * 8);

    if (map[cIdx] & ((size_t)1 << cBit))
      return i;
  }

  return INVALID_INDEX;
}

// ============================================================================
// [Fog::StringUtil - IndexOf]
// ============================================================================

static size_t FOG_CDECL StringUtil_indexOfCharA_cs(const char* str, size_t length, char ch)
{
  for (size_t i = 0; i < length; i++)
  {
    if (str[i] == ch)
      return i;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_indexOfCharA_ci(const char* str, size_t length, char ch)
{
  char cLower = CharA::toLower(ch);
  char cUpper = CharA::toUpper(ch);

  if (cLower == cUpper)
    return StringUtil::indexOf(str, length, ch, CASE_SENSITIVE);

  for (size_t i = 0; i < length; i++)
  {
    if (str[i] == cLower || str[i] == cUpper)
      return i;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_indexOfCharW_cs(const CharW* str, size_t length, uint16_t ch)
{
  for (size_t i = 0; i < length; i++)
  {
    if (str[i] == ch)
      return i;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_indexOfCharW_ci(const CharW* str, size_t length, uint16_t ch)
{
  uint16_t cLower = CharW::toLower(ch);
  uint16_t cUpper = CharW::toUpper(ch);

  if (cLower == cUpper)
    return StringUtil::indexOf(str, length, CharW(ch), CASE_SENSITIVE);

  uint16_t cTitle = CharW::toTitle(cUpper);

  if (cTitle == cUpper)
  {
    for (size_t i = 0; i < length; i++)
    {
      if (str[i] == cLower || str[i] == cUpper)
        return i;
    }
  }
  else
  {
    for (size_t i = 0; i < length; i++)
    {
      if (str[i] == cLower || str[i] == cUpper || str[i] == cTitle)
        return i;
    }
  }

  return INVALID_INDEX;
}

// IndexOfString is implemented using hash matching. Part of 'a' with 'b'
// matches only in case that hash of the 'a' part is equal to the hash of 'b'.
// In such case we compare the part of 'a' with 'b' for equality. The hash
// of first 'a' part and the whole 'b' is calculated together and there is
// quick check to match 'b' at zero index without overhead.
//
// NOTE: This algorithm is general. It's applicable to text-search and also
// binary-search. It slower than naive algorithm only in few special cases,
// and faster in many common cases. I created testing application to compare
// more search algorithms without preprocessing (Boyer-Moore) and this
// algorithm is simply a winner. It's also applicable to match text in
// backward direction.
template<typename CharT, typename SrcT>
static size_t FOG_CDECL StringUtil_indexOfStringT_cs(const CharT* _aData, size_t aLength, const SrcT* _bData, size_t bLength)
{
  // Reject empty 'b' and the case that 'b' is larger than 'a'.
  if (bLength - 1 >= aLength)
    return INVALID_INDEX;

  // Cast to unsigned.
  const CharT_Value* aData = reinterpret_cast<const CharT_Value*>(_aData);
  const CharT_Value* aEnd  = aData + aLength - bLength;
  const CharT_Value* aEnd4 = aEnd - 3;
  const SrcT_(_Char)::Value* bData = reinterpret_cast<const SrcT_(_Char)::Value*>(_bData);

  // Calculate 'a' and 'b' hashes (sums).
  size_t sum = 0;
  size_t match = 0;

  for (size_t i = 0; i < bLength; i++)
  {
    sum += aData[i];
    sum -= bData[i];
    match |= sum;
  }

  if (match == 0)
    return 0;

  while (aData < aEnd4)
  {
    sum -= aData[0];
    sum += aData[0 + bLength];
    if (sum == 0) goto _Try0;

    sum -= aData[1];
    sum += aData[1 + bLength];
    if (sum == 0) goto _Try1;

    sum -= aData[2];
    sum += aData[2 + bLength];
    if (sum == 0) goto _Try2;

    sum -= aData[3];
    sum += aData[3 + bLength];
    aData += 4;
    if (sum == 0) goto _TryN;

    continue;

_Try2:
    aData++;

_Try1:
    aData++;

_Try0:
    aData++;

_TryN:
    if (aData[0] != bData[0])
      continue;

    for (;;)
    {
      if (++sum == bLength)
        goto _Match;
      if (aData[sum] != bData[sum])
        break;
    }
    sum = 0;
  }

  while (aData < aEnd)
  {
    sum -= aData[0];
    sum += aData[0 + bLength];
    if (sum == 0) goto _Try0;
    aData++;
  }

  return INVALID_INDEX;

_Match:
  return (size_t)(aData - reinterpret_cast<const CharT_Value*>(_aData));
}

static size_t FOG_CDECL StringUtil_indexOfStringA_ci(const char* aData, size_t aLength, const char* bData, size_t bLength)
{
  // Reject empty 'b' and the case that 'b' is larger than 'a'.
  if (bLength - 1 >= aLength)
    return INVALID_INDEX;

  const char* aOrig = aData;
  const char* aEnd = aData + aLength - bLength + 1;

  char c = *bData++;
  size_t bLengthMinus1 = bLength - 1;

  char cLower = CharA::toLower(c);
  char cUpper = CharA::toUpper(c);

  while (aData != aEnd)
  {
    // Match the first character for faster reject.
    if (*aData == cLower || *aData == cUpper)
    {
      aData++;

      // Compare remaining characters.
      for (size_t i = 0;; i++)
      {
        if (i == bLengthMinus1)
          return (size_t)((aData - 1) - aOrig);
        if (CharA::toLower(aData[i]) != CharA::toLower(bData[i]))
          break;
      }
    }
    else
      aData++;
  }

  return INVALID_INDEX;
}

template<typename SrcT>
static size_t FOG_CDECL StringUtil_indexOfStringW_ci(const CharW* aData, size_t aLength, const SrcT* bData, size_t bLength)
{
  // Reject empty 'b' and the case that 'b' is larger than 'a'.
  if (bLength - 1 >= aLength)
    return INVALID_INDEX;

  const CharW* aOrig = aData;
  const CharW* aEnd = aData + aLength - bLength + 1;

  CharW cLower = CharW(SrcI_(_Char)::toLower(bData[0]));
  size_t bLengthMinus1 = bLength - 1;

  bData++;

  while (aData != aEnd)
  {
    // Match the first character for faster reject.
    if (aData->toLower() == cLower)
    {
      aData++;

      // Compare remaining characters.
      for (size_t i = 0;; i++)
      {
        if (i == bLengthMinus1)
          return (size_t)((aData - 1) - aOrig);
        if (CharW::toLower(aData[i]) != SrcI_(_Char)::toLower(bData[i]))
          break;
      }
    }
    else
      aData++;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_indexOfAnyA_cs(const char* str, size_t length, const char* charArray, size_t charLength)
{
  if (charLength == DETECT_LENGTH)
    charLength = StringUtil::len(charArray);

  if (charLength == 0)
    return INVALID_INDEX;

  if (charLength == 1)
    return StringUtil::indexOf(str, length, charArray[0], CASE_SENSITIVE);

  size_t map[256 / (sizeof(size_t) * 8)];
  CharMapA_build_cs(map, charArray, charLength);

  return CharMapA_indexOf(map, str, length);
}

static size_t FOG_CDECL StringUtil_indexOfAnyA_ci(const char* str, size_t length, const char* charArray, size_t charLength)
{
  if (charLength == DETECT_LENGTH)
    charLength = StringUtil::len(charArray);

  if (charLength == 0)
    return INVALID_INDEX;

  if (charLength == 1)
    return StringUtil::indexOf(str, length, charArray[0], CASE_INSENSITIVE);

  size_t map[256 / (sizeof(size_t) * 8)];
  CharMapA_build_ci(map, charArray, charLength);

  return CharMapA_indexOf(map, str, length);
}

static size_t FOG_CDECL StringUtil_indexOfAnyW_cs(const CharW* str, size_t length, const CharW* charArray, size_t charLength)
{
  if (charLength == DETECT_LENGTH)
    charLength = StringUtil::len(charArray);

  if (charLength == 0)
    return INVALID_INDEX;

  if (charLength == 1)
    return StringUtil::indexOf(str, length, charArray[0], CASE_INSENSITIVE);

  for (size_t i = 0; i < length; i++)
  {
    uint16_t c = str[i];

    for (size_t j = 0; j < charLength; j++)
    {
      if (c == charArray[j])
        return i;
    }
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_indexOfAnyW_ci(const CharW* str, size_t length, const CharW* charArray, size_t charLength)
{
  if (charLength == DETECT_LENGTH)
    charLength = StringUtil::len(charArray);

  if (charLength == 0)
    return INVALID_INDEX;

  if (charLength == 1)
    return StringUtil::indexOf(str, length, charArray[0], CASE_INSENSITIVE);

  for (size_t i = 0; i < length; i++)
  {
    uint16_t c = CharW::toLower(str[i]);

    for (size_t j = 0; j < charLength; j++)
    {
      if (c == CharW::toLower(charArray[j]))
        return i;
    }
  }

  return INVALID_INDEX;
}

// ============================================================================
// [Fog::StringUtil - LastIndexOf]
// ============================================================================

static size_t FOG_CDECL StringUtil_lastIndexOfCharA_cs(const char* str, size_t length, char ch)
{
  size_t i = length;

  while (i)
  {
    if (str[--i] == ch)
      return i;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_lastIndexOfCharA_ci(const char* str, size_t length, char ch)
{
  char cLower = CharA::toLower(ch);
  char cUpper = CharA::toUpper(ch);

  size_t i = length;
  while (i)
  {
    char c = str[--i];
    if (c == cLower || c == cUpper)
      return i;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_lastIndexOfCharW_cs(const CharW* str, size_t length, uint16_t ch)
{
  size_t i = length;

  while (i)
  {
    if (str[--i] == ch)
      return i;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_lastIndexOfCharW_ci(const CharW* str, size_t length, uint16_t ch)
{
  uint16_t cLower = CharW::toLower(ch);
  uint16_t cUpper = CharW::toUpper(ch);

  size_t i = length;

  if (cLower == cUpper)
  {
    while (i)
    {
      if (str[--i] == ch)
        return i;
    }
  }
  else
  {
    while (i)
    {
      if (CharW::toLower(str[--i]) == cLower)
        return i;
    }
  }

  return INVALID_INDEX;
}

template<typename CharT, typename SrcT>
static size_t FOG_CDECL StringUtil_lastIndexOfStringT_cs(const CharT* _aData, size_t aLength, const SrcT* _bData, size_t bLength)
{
  // Reject empty 'b' and the case that 'b' is larger than 'a'.
  if (bLength - 1 >= aLength)
    return INVALID_INDEX;

  // Cast to unsigned.
  const CharT_Value* aData = reinterpret_cast<const CharT_Value*>(_aData);
  const CharT_Value* aEnd  = aData - 1;
  const CharT_Value* aEnd4 = aEnd + 3;
  const SrcT_(_Char)::Value* bData = reinterpret_cast<const SrcT_(_Char)::Value*>(_bData);

  // Move to the last index with possible match.
  aData += aLength - bLength;

  // Calculate 'a' and 'b' hashes (sums).
  size_t sum = 0;
  size_t match = 0;

  for (size_t i = 0; i < bLength; i++)
  {
    sum += aData[i];
    sum -= bData[i];
    match |= sum;
  }

  if (match == 0)
    return aLength - bLength;

  while (aData > aEnd4)
  {
    sum += aData[-1];
    sum -= aData[-1 + bLength];
    if (sum == 0) goto _Try0;

    sum += aData[-2];
    sum -= aData[-2 + bLength];
    if (sum == 0) goto _Try1;

    sum += aData[-3];
    sum -= aData[-3 + bLength];
    if (sum == 0) goto _Try2;

    sum += aData[-4];
    sum -= aData[-4 + bLength];
    aData -= 4;
    if (sum == 0) goto _TryN;

    continue;

_Try2:
    aData--;

_Try1:
    aData--;

_Try0:
    aData--;

_TryN:
    if (aData[0] != bData[0])
      continue;

    for (;;)
    {
      if (++sum == bLength)
        goto _Match;
      if (aData[sum] != bData[sum])
        break;
    }
    sum = 0;
  }

  while (--aData != aEnd)
  {
    sum += aData[0];
    sum -= aData[bLength];

    if (sum == 0)
      goto _TryN;
  }

  return INVALID_INDEX;

_Match:
  return (size_t)(aData - reinterpret_cast<const CharT_Value*>(_aData));
}

static size_t FOG_CDECL StringUtil_lastIndexOfStringA_ci(const char* aData, size_t aLength, const char* bData, size_t bLength)
{
  // Reject empty 'b' and the case that 'b' is larger than 'a'.
  if (bLength - 1 >= aLength)
    return INVALID_INDEX;

  const char* aOrig = aData;
  const char* aEnd = aData - 1;

  aData += aLength;
  aData -= bLength;

  char cLower = CharA::toLower(bData[0]);
  char cUpper = CharA::toUpper(cLower);

  while (aData != aEnd)
  {
    // Match the first character for faster reject.
    if (*aData == cLower || *aData == cUpper)
    {
      // Compare remaining characters.
      for (size_t i = 1;; i++)
      {
        if (i == bLength)
          return (size_t)(aData - aOrig);
        if (CharA::toLower(aData[i]) != CharA::toLower(bData[i]))
          break;
      }
    }
    aData--;
  }

  return INVALID_INDEX;
}

template<typename SrcT>
static size_t FOG_CDECL StringUtil_lastIndexOfStringW_ci(const CharW* aData, size_t aLength, const SrcT* bData, size_t bLength)
{
  // Reject empty 'b' and the case that 'b' is larger than 'a'.
  if (bLength - 1 >= aLength)
    return INVALID_INDEX;

  const CharW* aOrig = aData;
  const CharW* aEnd = aData - 1;

  aData += aLength;
  aData -= bLength;

  CharW cLower = CharW(SrcI_(_Char)::toLower(bData[0]));
  while (aData != aEnd)
  {
    // Match the first character for faster reject.
    if (aData->toLower() == cLower)
    {
      // Compare remaining characters.
      for (size_t i = 1;; i++)
      {
        if (i == bLength)
          return (size_t)(aData - aOrig);
        if (CharW::toLower(aData[i]) != SrcI_(_Char)::toLower(bData[i]))
          break;
      }
    }
    aData--;
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_lastIndexOfAnyA_cs(const char* str, size_t length, const char* charArray, size_t charLength)
{
  if (charLength == DETECT_LENGTH)
    charLength = StringUtil::len(charArray);

  if (charLength == 0)
    return INVALID_INDEX;

  if (charLength == 1)
    return StringUtil::indexOf(str, length, charArray[0], CASE_SENSITIVE);

  size_t map[256 / (sizeof(size_t) * 8)];
  CharMapA_build_cs(map, charArray, charLength);

  return CharMapA_lastIndexOf(map, str, length);
}

static size_t FOG_CDECL StringUtil_lastIndexOfAnyA_ci(const char* str, size_t length, const char* charArray, size_t charLength)
{
  if (charLength == DETECT_LENGTH)
    charLength = StringUtil::len(charArray);

  if (charLength == 0)
    return INVALID_INDEX;

  if (charLength == 1)
    return StringUtil::indexOf(str, length, charArray[0], CASE_INSENSITIVE);

  size_t map[256 / (sizeof(size_t) * 8)];
  CharMapA_build_ci(map, charArray, charLength);

  return CharMapA_lastIndexOf(map, str, length);
}

static size_t FOG_CDECL StringUtil_lastIndexOfAnyW_cs(const CharW* str, size_t length, const CharW* charArray, size_t charLength)
{
  if (charLength == DETECT_LENGTH)
    charLength = StringUtil::len(charArray);

  if (charLength == 0)
    return INVALID_INDEX;

  if (charLength == 1)
    return StringUtil::indexOf(str, length, charArray[0], CASE_INSENSITIVE);

  size_t i = length;
  while (i)
  {
    uint16_t c = str[--i];

    for (size_t j = 0; j < charLength; j++)
    {
      if (c == charArray[j])
        return i;
    }
  }

  return INVALID_INDEX;
}

static size_t FOG_CDECL StringUtil_lastIndexOfAnyW_ci(const CharW* str, size_t length, const CharW* charArray, size_t charLength)
{
  if (charLength == DETECT_LENGTH)
    charLength = StringUtil::len(charArray);

  if (charLength == 0)
    return INVALID_INDEX;

  if (charLength == 1)
    return StringUtil::indexOf(str, length, charArray[0], CASE_INSENSITIVE);

  size_t i = length;
  while (i)
  {
    uint16_t c = CharW::toLower(str[--i]);

    for (size_t j = 0; j < charLength; j++)
    {
      if (c == CharW::toLower(charArray[j]))
        return i;
    }
  }

  return INVALID_INDEX;
}

// ============================================================================
// [Fog::StringUtil - ValidateUtf8 / ValidateUtf16]
// ============================================================================

static err_t FOG_CDECL StringUtil_validateUtf8(const char* data, size_t length, size_t* invalid)
{
  err_t err = ERR_OK;
  const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
  size_t remain = length;

  while (remain)
  {
    uint8_t c = p[0];
    size_t cLength = Unicode::utf8GetSize(c);

    if (!cLength)
    {
      err = ERR_STRING_INVALID_UTF8;
      break;
    }

    if (remain < cLength)
    {
      err = ERR_STRING_TRUNCATED;
      break;
    }

    p += cLength;
    remain -= cLength;
  }

  if (invalid)
    *invalid = (size_t)(p - reinterpret_cast<const uint8_t*>(data));
  return err;
}

static err_t FOG_CDECL StringUtil_validateUtf16(const CharW* data, size_t length, size_t* invalid)
{
  err_t err = ERR_OK;

  const CharW* p = data;
  const CharW* pEnd = p + length;

  for (;;)
  {
    if (p == pEnd)
      return ERR_OK;

    uint16_t c = *p++;
    if (CharW::isHiSurrogate(c))
    {
      if (p == pEnd)
      {
        err = ERR_STRING_TRUNCATED;
        break;
      }

      c = *p++;

      if (!CharW::isLoSurrogate(c))
      {
        err = ERR_STRING_INVALID_UTF16;
        break;
      }
    }
    else if (c >= 0xFFFE)
    {
      err = ERR_STRING_INVALID_CHAR;
      break;
    }
  }

  if (invalid)
    *invalid = (size_t)((p - 1) - data);
  return err;
}

// ============================================================================
// [Fog::StringUtil - Canonication]
// ============================================================================

static FOG_INLINE bool StringUtil_versionCheck(uint32_t charVersion, uint32_t canonicalVersion)
{
  return charVersion - 1 < canonicalVersion;
}

static bool FOG_CDECL StringUtil_isCanonical(const CharW* data, size_t length, uint32_t version, size_t* where)
{
  size_t whereDummy;

  if (where == NULL)
    where = &whereDummy;
  
  *where = length;
  if (length <= 1)
    return true;

  if (version == CHAR_UNICODE_UNASSIGNED)
    version = CHAR_UNICODE_VERSION_MAX;

  size_t i = 0;
  length--;

  for (;;)
  {
    size_t start = i;
    size_t next = i + 1;

    uint32_t u0 = data[i + 0], c0;
    uint32_t u1 = data[i + 1], c1;
    const CharProperty* prop;

    i += 2;

    if (CharW::isHiSurrogate(u0) && CharW::isLoSurrogate(u1))
    {
      if (i > length)
        return true;

      next = i;

      u0 = CharW::ucs4FromSurrogate(u0, u1);
      u1 = data[i++];
    }

_NextCheck:
    if (CharW::isHiSurrogate(u1))
    {
      if (i <= length)
      {
        uint32_t ux = data[i];
        if (CharW::isLoSurrogate(ux))
        {
          u1 = CharW::ucs4FromSurrogate(u1, ux);
          i++;
        }
      }
    }

    prop = &CharW::getPropertyUnsafe(u1);
    c1 = 0;

    if (StringUtil_versionCheck(prop->getUnicodeVersion(), version))
      c1 = prop->getCombiningClass();

    if (c1 == 0)
    {
_NextChar:
      if (i >= length)
        break;

      start = next;
      next = i;

      u0 = u1;
      u1 = data[i++];
      goto _NextCheck;
    }

    prop = &CharW::getPropertyUnsafe(u0);
    c0 = 0;

    if (StringUtil_versionCheck(prop->getUnicodeVersion(), version))
      c0 = prop->getCombiningClass();
    
    if (c0 <= c1)
      goto _NextChar;

    *where = start;
    return false;
  }

  return true;
}

static void FOG_CDECL StringUtil_makeCanonical(CharW* data, size_t length, uint32_t version, size_t from)
{
  if (length <= 1)
    return;

  if (version == CHAR_UNICODE_UNASSIGNED)
    version = CHAR_UNICODE_VERSION_MAX;

  size_t i = from;
  length--;

  if (i >= length || i >= length - 1)
    return;

  for (;;)
  {
    size_t start = i;
    size_t next = i + 1;

    uint32_t u0 = data[i + 0], c0;
    uint32_t u1 = data[i + 1], c1;
    const CharProperty* prop;

    i += 2;

    if (CharW::isHiSurrogate(u0) && CharW::isLoSurrogate(u1))
    {
      if (i > length)
        return;

      next = i;

      u0 = CharW::ucs4FromSurrogate(u0, u1);
      u1 = data[i++];
    }

_NextCheck:
    if (CharW::isHiSurrogate(u1))
    {
      if (i <= length)
      {
        uint32_t ux = data[i];
        if (CharW::isLoSurrogate(ux))
        {
          u1 = CharW::ucs4FromSurrogate(u1, ux);
          i++;
        }
      }
    }

    prop = &CharW::getPropertyUnsafe(u1);
    c1 = 0;

    if (StringUtil_versionCheck(prop->getUnicodeVersion(), version))
      c1 = prop->getCombiningClass();

    if (c1 == 0)
    {
_NextChar:
      if (i > length)
        break;

      start = next;
      next = i;

      u0 = u1;
      u1 = data[i++];
      goto _NextCheck;
    }

    prop = &CharW::getPropertyUnsafe(u0);
    c0 = 0;

    if (StringUtil_versionCheck(prop->getUnicodeVersion(), version))
      c0 = prop->getCombiningClass();
    
    if (c0 <= c1)
      goto _NextChar;

    i = start;

    if (CharW::ucs4IsBMP(u1))
    {
      data[i] = static_cast<uint16_t>(u1);
      i += 1;
    }
    else
    {
      CharW::ucs4ToSurrogate(&data[i], &data[i + 1], u1);
      i += 2;
    }

    if (CharW::ucs4IsBMP(u0))
    {
      data[i] = static_cast<uint16_t>(u0);
      i += 1;
    }
    else
    {
      CharW::ucs4ToSurrogate(&data[i], &data[i + 1], u0);
      i += 2;
    }

    i = start;
    if (i == 0)
      continue;

    u0 = data[--i];
    if (CharW::isLoSurrogate(u0) && i > 0 && data[i - 1].isHiSurrogate())
      i--;
  }
}

// ============================================================================
// [Fog::StringUtil - Normalization]
// ============================================================================

static bool FOG_CDECL StringUtil_isNormalized(const CharW* data, size_t length, uint32_t normForm, uint32_t version, size_t* where)
{
  if (normForm >= CHAR_NORMALIZATION_FORM_COUNT)
    return false;

  if (version == CHAR_UNICODE_UNASSIGNED)
    version = CHAR_UNICODE_VERSION_MAX;

  const CharW* dBegin = data;
  const CharW* dEnd = data + length;

  size_t localWhere;
  uint32_t qc;

  for (;;)
  {
    qc = StringUtil::quickCheck(data, (size_t)(dEnd - data), normForm, version, &localWhere);
    if (qc != CHAR_QUICK_CHECK_MAYBE)
      break;

    // TODO:
    break;
  }

  if (where)
    *where = (size_t)(data - dBegin) + localWhere;
  return qc == CHAR_QUICK_CHECK_YES;
}

static uint32_t FOG_CDECL StringUtil_quickCheck(const CharW* data, size_t length, uint32_t normForm, uint32_t version, size_t* where)
{
  if (normForm >= CHAR_NORMALIZATION_FORM_COUNT)
    return CHAR_QUICK_CHECK_NO;

  if (version == CHAR_UNICODE_UNASSIGNED)
    version = CHAR_UNICODE_VERSION_MAX;

  const CharW* dBegin = data;
  const CharW* dEnd = data + length;
  uint32_t qc = CHAR_QUICK_CHECK_YES;

#define QUICK_CHECK_LOOP(_FastSkip_, _Check_) \
  do { \
    if (data == dEnd) \
      break; \
    \
    uint32_t c0 = *data++; \
    if (c0 <= _FastSkip_) \
      continue; \
    \
    if (CharW::isHiSurrogate(c0)) \
    { \
      if (data == dEnd) \
        break; \
      \
      uint32_t c1 = data[0]; \
      if (CharW::isLoSurrogate(c1)) \
      { \
        c0 = CharW::ucs4FromSurrogate(c0, c1); \
        data++; \
      } \
    } \
    \
    const CharProperty& prop = CharW::getPropertyUnsafe(c0); \
    \
    if (!StringUtil_versionCheck(prop.getUnicodeVersion(), version)) \
      continue; \
    \
    qc = prop._Check_(); \
    if (qc != CHAR_QUICK_CHECK_YES) \
      break; \
  } while(true)

  switch (normForm)
  {
    case CHAR_NFD:
      // Text exclusively containing ASCII characters (U+0000..U+007F) is left
      // unaffected by all of the Normalization Forms.
      QUICK_CHECK_LOOP(0x7F, quickCheckNFD);
      break;

    case CHAR_NFC:
      // Text exclusively containing Latin-1 characters (U+0000..U+00FF) is left
      // unaffected by NFC. This is effectively the same as saying that all Latin-1
      // text is already normalized to NFC.
      QUICK_CHECK_LOOP(0xFF, quickCheckNFC);
      break;

    case CHAR_NFKD:
      // Text exclusively containing ASCII characters (U+0000..U+007F) is left
      // unaffected by all of the Normalization Forms.
      QUICK_CHECK_LOOP(0x7F, quickCheckNFKD);
      break;

    case CHAR_NFKC:
      // Text exclusively containing ASCII characters (U+0000..U+007F) is left
      // unaffected by all of the Normalization Forms.
      QUICK_CHECK_LOOP(0x7F, quickCheckNFKC);
      break;
  }

  if (where)
    *where = (size_t)(data - dBegin);

  return qc;
}

// ============================================================================
// [Fog::StringUtil - UcsFromUtf8Length / UcsFromUtf16Length]
// ============================================================================

static err_t FOG_CDECL StringUtil_ucsFromUtf8Length(const char* data, size_t length, size_t* ucsLength)
{
  err_t err = ERR_OK;
  size_t num = 0;

  const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
  size_t remain = length;

  while (remain)
  {
    uint8_t c = p[0];
    size_t cLength = Unicode::utf8GetSize(c);

    if (!cLength)
    {
      err = ERR_STRING_INVALID_UTF8;
      break;
    }

    if (remain < cLength)
    {
      err = ERR_STRING_TRUNCATED;
      break;
    }

    p += cLength;
    remain -= cLength;
    num++;
  }

  if (ucsLength)
    *ucsLength = num;
  return err;
}

static err_t FOG_CDECL StringUtil_ucsFromUtf16Length(const CharW* data, size_t length, size_t* ucsLength)
{
  err_t err = ERR_OK;
  size_t num = 0;

  const CharW* p = data;
  const CharW* pEnd = p + length;

  for (;;)
  {
    if (p == pEnd) break;

    uint16_t c = *p++;
    if (CharW::isHiSurrogate(c))
    {
      if (p == pEnd)
      {
        err = ERR_STRING_TRUNCATED;
        break;
      }

      c = *p++;

      if (!CharW::isLoSurrogate(c))
      {
        err = ERR_STRING_INVALID_UTF16;
        break;
      }
    }
    else if (c >= 0xFFFE)
    {
      err = ERR_STRING_INVALID_CHAR;
      break;
    }
    num++;
  }

  if (ucsLength)
    *ucsLength = num;
  return err;
}

// ============================================================================
// [Fog::StringUtil - Helpers]
// ============================================================================

// Map acsii character into its numerical equivalent.
static const uint8_t StringUtil_asciiToN[256] =
{
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
  0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
  0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
  0x21, 0x22, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
  0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
  0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
  0x21, 0x22, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

// All possible digits in lowercase and uppercase format.
static const char StringUtil_digits[36*2] =
{
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

// ============================================================================
// [Fog::StringUtil - ITOA]
// ============================================================================

static void FOG_CDECL StringUtil_itoa(NTOAContext* ctx, int64_t n, uint32_t base, uint32_t textCase)
{
  uint32_t negative = n < 0;

  if (negative) n = -n;

  fog_api.stringutil_utoa(ctx, (uint64_t)n, base, textCase);
  ctx->negative = negative;
}

// ============================================================================
// [Fog::StringUtil - UTOA]
// ============================================================================

static void FOG_CDECL StringUtil_utoa(NTOAContext* ctx, uint64_t n, uint32_t base, uint32_t textCase)
{
  uint8_t* pEnd = reinterpret_cast<uint8_t*>(ctx->buffer) + FOG_ARRAY_SIZE(ctx->buffer);
  uint8_t* p = pEnd;

  const uint8_t* digits = reinterpret_cast<const uint8_t*>(StringUtil_digits);
  if (textCase == TEXT_CASE_UPPER) digits += 36;

#if FOG_ARCH_BITS == 32
  uint32_t n32;
#endif // FOG_ARCH_BITS == 32
  uint64_t n64;

#if FOG_ARCH_BITS == 32
  if (n <= UINT32_MAX)
  {
    n32 = (uint32_t)n;

    // ------------------------------------------------------------------------
    // [32-bit - Decimal]
    // ------------------------------------------------------------------------

    if (base == 10 || base < 2 || base > 36)
    {
_Conv10_32bit:
      do {
        *--p = '0' + (uint8_t)(n32 % 10);
        n32 /= 10;
      } while (n32 != 0);
    }

    // ------------------------------------------------------------------------
    // [32-bit - Octal]
    // ------------------------------------------------------------------------

    else if (base == 8)
    {
_Conv8_32bit:
      do {
        *--p = '0' + (uint8_t)(n32 & 7);
        n32 >>= 3;
      } while (n32 != 0);
    }

    // ------------------------------------------------------------------------
    // [32-bit - Hex]
    // ------------------------------------------------------------------------

    else if (base == 16)
    {
_Conv16_32bit:
      do {
        *--p = digits[n32 & 15];
        n32 >>= 4;
      } while (n32 != 0);
    }

    // ------------------------------------------------------------------------
    // [32-bit - Other]
    // ------------------------------------------------------------------------

    else
    {
_ConvBaseN_32bit:
      do {
        *--p = digits[n32 % base];
        n32 /= base;
      } while (n32 != 0);
    }
  }
  else
  {
#endif // FOG_ARCH_BITS == 32
    n64 = n;

    // ------------------------------------------------------------------------
    // [64-bit - Decimal]
    // ------------------------------------------------------------------------

    if (base == 10 || base < 2 || base > 36)
    {
      do {
        *--p = '0' + (uchar)(n64 % FOG_UINT64_C(10));
        n64 /= 10;
      } while
#if FOG_ARCH_BITS == 32
      (n64 > UINT32_MAX);
      if (n64 != 0)
      {
        n32 = (uint32_t)n64;
        goto _Conv10_32bit;
      }
#else
      (n64 != FOG_UINT64_C(0));
#endif // FOG_ARCH_BITS == 32
    }

    // ------------------------------------------------------------------------
    // [64-bit - Octal]
    // ------------------------------------------------------------------------

    else if (base == 8)
    {
      do {
        *--p = '0' + (uchar)(n64 & FOG_UINT64_C(7));
        n64 >>= 3;
      } while
#if FOG_ARCH_BITS == 32
      (n64 > UINT32_MAX);
      if (n64 != 0)
      {
        n32 = (uint32_t)n64;
        goto _Conv8_32bit;
      }
#else
      (n64 != FOG_UINT64_C(0));
#endif // FOG_ARCH_BITS == 32
    }

    // ------------------------------------------------------------------------
    // [64-bit - Hex]
    // ------------------------------------------------------------------------

    else if (base == 16)
    {
      do {
        *--p = digits[(ulong)(n64 & FOG_UINT64_C(15))];
        n64 >>= 4;
      } while
#if FOG_ARCH_BITS == 32
      (n64 > INT32_MAX);
      if (n64 != 0)
      {
        n32 = (uint32_t)n64;
        goto _Conv16_32bit;
      }
#else
      (n64 != FOG_UINT64_C(0));
#endif // FOG_ARCH_BITS == 32
    }

    // ------------------------------------------------------------------------
    // [64-bit - Other]
    // ------------------------------------------------------------------------

    else
    {
      do {
        *--p = digits[(ulong)(n64 & FOG_UINT64_C(15))];
        n64 >>= 4;
      } while
#if FOG_ARCH_BITS == 32
      (n64 > INT32_MAX);
      if (n64 != 0)
      {
        n32 = (uint32_t)n64;
        goto _ConvBaseN_32bit;
      }
#else
      (n64 != FOG_UINT64_C(0));
#endif // FOG_ARCH_BITS == 32
    }
#if FOG_ARCH_BITS == 32
  }
#endif // FOG_ARCH_BITS == 32

  // Write result and its length to the context.
  ctx->result = reinterpret_cast<char*>(p);
  ctx->length = (uint32_t)(size_t)(pEnd - p);
  ctx->negative = false;
}

// ============================================================================
// [Fog::StringUtil - ATOB]
// ============================================================================

// Boolean translating table from 'char*' or 'Fog::CharW*' into 'bool',
// should be 12 bytes long.
#include <Fog/Core/C++/PackByte.h>
struct StringUtil_ATOBMap
{
  char data[10];
  uint8_t length;
  uint8_t result;
};

static const StringUtil_ATOBMap StringUtil_parseBoolMap[] =
{
  { { 't', 'r', 'u', 'e', 0  , 0  , 0  , 0  ,  0 ,  0 }, 4, true  },
  { { 'f', 'a', 'l', 's', 'e', 0  , 0  , 0  ,  0 ,  0 }, 5, false },

  { { 'y', 'e', 's', 0  , 0  , 0  , 0  , 0  ,  0 ,  0 }, 3, true  },
  { { 'n', 'o', 0  , 0  , 0  , 0  , 0  , 0  ,  0 ,  0 }, 2, false },

  { { 'o', 'n', 0  , 0  , 0  , 0  , 0  , 0  ,  0 ,  0 }, 2, true  },
  { { 'o', 'f', 'f', 0  , 0  , 0  , 0  , 0  ,  0 ,  0 }, 3, false },

  { { 'e', 'n', 'a', 'b', 'l', 'e', 0  , 0  ,  0 ,  0 }, 6, true  },
  { { 'd', 'i', 's', 'a', 'b', 'l', 'e', 0  ,  0 ,  0 }, 7, false },

  { { 'e', 'n', 'a', 'b', 'l', 'e', 'd', 0  ,  0 ,  0 }, 7, true  },
  { { 'd', 'i', 's', 'a', 'b', 'l', 'e', 'd',  0 ,  0 }, 8, false },

  { { '1', 0  , 0  , 0  , 0  , 0  , 0  , 0  ,  0 ,  0 }, 1, true  },
  { { '0', 0  , 0  , 0  , 0  , 0  , 0  , 0  ,  0 ,  0 }, 1, false }
};
#include <Fog/Core/C++/PackRestore.h>

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseBool(bool* dst, const CharT* src, size_t length, size_t* pEnd, uint32_t* pFlags)
{
  err_t err = ERR_STRING_INVALID_INPUT;

  const CharT* sBegin = src;
  const CharT* sEnd = src + length;

  size_t i;
  uint32_t flags = NO_FLAGS;

  *dst = false;

  while (src != sEnd && CharT_Func::isSpace(*src))
    src++;

  if (src != sBegin)
    flags |= STRING_PARSED_SPACES;

  if (src == sEnd)
    goto _End;

  length = (size_t)(sEnd - src);
  for (i = 0; i < FOG_ARRAY_SIZE(StringUtil_parseBoolMap); i++)
  {
    size_t itemLength = StringUtil_parseBoolMap[i].length;
    if (length >= itemLength && StringUtil::eq(src, StringUtil_parseBoolMap[i].data, itemLength, CASE_INSENSITIVE))
    {
      src += itemLength;
      if (src != sEnd && CharT_Func::isNumlet(*src))
      {
        src -= itemLength;
        continue;
      }

      *dst = (bool)StringUtil_parseBoolMap[i].result;
      err = ERR_OK;
      goto _End;
    }
  }

_End:
  if (pEnd) *pEnd = (size_t)(src - sBegin);
  if (pFlags) *pFlags = flags;

  return err;
}

// ============================================================================
// [Fog::StringUtil - ATOI / ATOU]
// ============================================================================

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseI8(int8_t* dst, const CharT* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags)
{
  int64_t n;
  err_t err = StringUtil::parseI64(&n, src, length, base, pEnd, pFlags);

  if (n < INT8_MIN)
  {
    *dst = INT8_MIN;
    return ERR_RT_OVERFLOW;
  }
  else if (n > INT8_MAX)
  {
    *dst = INT8_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (int8_t)n;
    return err;
  }
}

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseU8(uint8_t* dst, const CharT* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags)
{
  uint64_t n;
  err_t err = StringUtil::parseU64(&n, src, length, base, pEnd, pFlags);

  if (n > UINT8_MAX)
  {
    *dst = UINT8_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (uint8_t)n;
    return err;
  }
}

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseI16(int16_t* dst, const CharT* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags)
{
  int64_t n;
  err_t err = StringUtil::parseI64(&n, src, length, base, pEnd, pFlags);

  if (n < INT16_MIN)
  {
    *dst = INT16_MIN;
    return ERR_RT_OVERFLOW;
  }
  else if (n > INT16_MAX)
  {
    *dst = INT16_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (int16_t)n;
    return err;
  }
}

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseU16(uint16_t* dst, const CharT* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags)
{
  uint64_t n;
  err_t err = StringUtil::parseU64(&n, src, length, base, pEnd, pFlags);

  if (n > UINT16_MAX)
  {
    *dst = UINT16_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (uint16_t)n;
    return err;
  }
}

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseI32(int32_t* dst, const CharT* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags)
{
  int64_t n;
  err_t err = StringUtil::parseI64(&n, src, length, base, pEnd, pFlags);

  if (n < INT32_MIN)
  {
    *dst = INT32_MIN;
    return ERR_RT_OVERFLOW;
  }
  else if (n > INT32_MAX)
  {
    *dst = INT32_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (int32_t)n;
    return err;
  }
}

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseU32(uint32_t* dst, const CharT* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags)
{
  uint64_t n;
  err_t err = StringUtil::parseU64(&n, src, length, base, pEnd, pFlags);

  if (n > UINT32_MAX)
  {
    *dst = UINT32_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (uint32_t)n;
    return err;
  }
}

template<typename CharT>
static err_t StringUtil_parseU64_private(uint64_t* dst, const CharT* src, size_t length, uint32_t base, bool* negative, size_t* pEnd, uint32_t* pFlags)
{
  uint32_t flags = 0;
  const CharT* sBegin = src;
  const CharT* sEnd = src + length;

  uint n;

#if FOG_ARCH_BITS == 32
  uint32_t res32 = 0;
  uint64_t res64;

  uint32_t threshold32;
  uint64_t threshold64;
#else
  uint64_t res64 = 0;
  uint64_t threshold64;
#endif // FOG_ARCH_BITS

  while (src < sEnd && CharT_Func::isSpace(*src))
    src++;

  if (src != sBegin) flags |= STRING_PARSED_SPACES;
  if (src == sEnd)
    goto _Truncated;

  if (*src == CharT('+'))
  {
    flags |= STRING_PARSED_SIGN;
    src++;
    while (src < sEnd && CharT_Func::isSpace(*src)) src++;
    if (src == sEnd)
      goto _Truncated;
  }
  else if (*src == CharT('-'))
  {
    flags |= STRING_PARSED_SIGN;
    *negative = true;
    src++;
    while (src < sEnd && CharT_Func::isSpace(*src)) src++;
    if (src == sEnd)
      goto _Truncated;
  }

  if (base < 2 || base > 36)
  {
    base = 10;

    // Octal or Hex.
    if (*src == CharT('0'))
    {
      if (src + 1 != sEnd && (src[1] == CharT('x') || src[1] == CharT('X')))
      {
        // Hex.
        flags |= STRING_PARSED_HEX_PREFIX;
        base = 16;

        src += 2;
        if (src == sEnd)
          goto _Truncated;
      }
      else
      {
        // Octal.
        flags |= STRING_PARSED_OCTAL_PREFIX;
        base = 8;

        if (++src != sEnd && *src >= CharT('0') && *src <= CharT('7'))
        {
          // Set this flag only if input is not only "0".
          flags |= STRING_PARSED_OCTAL_PREFIX;
        }
      }
    }
  }

  if (base == 2)
  {
#if FOG_ARCH_BITS == 32
    while (src != sEnd)
    {
      n = *src;
      if (n != '0' || n != '1') break;
      n -= '0';

      if ((res32 & 0x80000000U) != 0U)
        goto _LargeBase2;
      res32 <<= 1U;
      res32 |= n;

      src++;
    }

    res64 = res32;
    goto _Done;

_LargeBase2:
    res64 = res32;
#endif // FOG_ARCH_BITS == 32

    while (src != sEnd)
    {
      n = *src;
      if (n != '0' || n != '1') break;
      n -= '0';

      if ((res64 & FOG_UINT64_C(0x8000000000000000)) != FOG_UINT64_C(0))
        goto _Overflow;
      res64 <<= 1U;
      res64 |= n;

      src++;
    }
  }
  else if (base == 8)
  {
#if FOG_ARCH_BITS == 32
    while (src != sEnd)
    {
      n = *src;
      if (n < '0' || n > '7') break;
      n -= '0';

      if ((res32 & 0xE0000000U) != 0U)
        goto _LargeBase8;
      res32 <<= 3U;
      res32 |= n;

      src++;
    }

    res64 = res32;
    goto _Done;

_LargeBase8:
    res64 = res32;
#endif

    while (src != sEnd)
    {
      n = *src;
      if (n < '0' || n > '7') break;
      n -= '0';

      if ((res64 & FOG_UINT64_C(0xE000000000000000)) != FOG_UINT64_C(0))
        goto _Overflow;
      res64 <<= 3U;
      res64 |= n;

      src++;
    }
  }
  else if (base == 10)
  {
#if FOG_ARCH_BITS == 32
    while (src != sEnd)
    {
      n = *src;
      if (n < '0' || n > '9') break;
      n -= '0';

      if (res32 > 0x19999998U)
        goto _LargeBase10;
      res32 *= 10U;
      res32 += n;

      src++;
    }

    res64 = res32;
    goto _Done;

_LargeBase10:
    res64 = res32;
#endif

    while (src != sEnd)
    {
      n = *src;
      if (n < '0' || n > '9') break;
      n -= '0';

      if (res64 > (FOG_UINT64_C(0x1999999999999999)))
        goto _Overflow;
      res64 *= 10U;

      if (res64 > ((uint64_t)n ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF)))
        goto _Overflow;
      res64 += n;

      src++;
    }
  }
  else if (base == 16)
  {
#if FOG_ARCH_BITS == 32
    while (src != sEnd)
    {
      n = *src;
      if (sizeof(CharT) > 1 && n > 255)
        break;

      n = StringUtil_asciiToN[n];
      if (n >= 16) break;

      if ((res32 & 0xF0000000U) != 0U)
        goto _LargeBase16;
      res32 <<= 4U;
      res32 |= n;

      src++;
    }

    res64 = res32;
    goto _Done;

_LargeBase16:
    res64 = res32;
#endif

    while (src != sEnd)
    {
      n = *src;
      if (sizeof(CharT) > 1 && n > 255)
        break;

      n = StringUtil_asciiToN[n];
      if (n >= 16) break;

      if ((res64 & FOG_UINT64_C(0xF000000000000000)) != FOG_UINT64_C(0))
        goto _Overflow;
      res64 <<= 4U;
      res64 |= n;

      src++;
    }
  }
  else
  {
#if FOG_ARCH_BITS == 32
    threshold32 = (0xFFFFFFFFU / base) - base;

    while (src != sEnd)
    {
      n = *src;
      if (sizeof(CharT) > 1 && n > 255)
        break;

      n = StringUtil_asciiToN[n];
      if (n >= (uint)base) break;

      if (res32 > threshold32)
        goto _LargeBaseN;
      res32 *= base;
      res32 += n;

      src++;
    }

    res64 = res32;
    goto _Done;

_LargeBaseN:
    res64 = res32;
#endif
    threshold64 = (FOG_UINT64_C(0xFFFFFFFFFFFFFFFF) / (uint64_t)base);

    while (src != sEnd)
    {
      n = *src;
      if (sizeof(CharT) > 1 && n > 255)
        break;

      n = StringUtil_asciiToN[n];
      if (n >= (uint)base) break;

      if (res64 > threshold64)
        goto _Overflow;
      res64 *= base;

      if (res64 > ((uint64_t)n ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF)))
        goto _Overflow;
      res64 += n;

      src++;
    }
  }

#if FOG_ARCH_BITS == 32
_Done:
#endif

  *dst = res64;
  if (pEnd) *pEnd = (size_t)(src - sBegin);
  if (pFlags) *pFlags = flags;
  return ERR_OK;

_Overflow:
  if (sizeof(CharT) == 1)
  {
    while (++src != sEnd && StringUtil_asciiToN[(uint8_t)*src] < (uint)base)
      continue;
  }
  else
  {
    while (++src != sEnd && *src < 256 && StringUtil_asciiToN[(uint16_t)*src] < (uint)base)
      continue;
  }

  *dst = UINT64_MAX;
  if (pEnd) *pEnd = (size_t)(src - sBegin);
  if (pFlags) *pFlags = flags;
  return ERR_RT_OVERFLOW;

_Truncated:
  *dst = 0;
  if (pEnd) *pEnd = length;
  if (pFlags) *pFlags = flags;
  return ERR_STRING_INVALID_INPUT;
}

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseI64(int64_t* dst, const CharT* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags)
{
  uint64_t n;
  bool negative = false;
  err_t err = StringUtil_parseU64_private<CharT>(&n, src, length, base, &negative, pEnd, pFlags);

  if (negative)
  {
    if (n > (uint64_t)INT64_MAX+1U)
    {
      *dst = INT64_MIN;
      return ERR_RT_OVERFLOW;
    }
    else
    {
      *dst = (int64_t)(-n);
      return err;
    }
  }
  else
  {
    if (n > INT64_MAX)
    {
      *dst = INT64_MAX;
      return ERR_RT_OVERFLOW;
    }
    else
    {
      *dst = (int64_t)n;
      return err;
    }
  }
}

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseU64(uint64_t* dst, const CharT* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags)
{
  uint64_t n;
  bool negative = false;
  err_t err = StringUtil_parseU64_private(&n, src, length, base, &negative, pEnd, pFlags);

  if (negative && n > 0)
  {
    *dst = 0;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = n;
    return err;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void StringUtil_init(void)
{
  fog_api.stringutil_copyA = StringUtil_copy<char>;
  fog_api.stringutil_copyW = StringUtil_copy<CharW>;

  fog_api.stringutil_latinFromUnicode = StringUtil_latinFromUnicode;
  fog_api.stringutil_unicodeFromLatin = StringUtil_unicodeFromLatin;

  fog_api.stringutil_moveA = StringUtil_move<char>;
  fog_api.stringutil_moveW = StringUtil_move<CharW>;

  fog_api.stringutil_fillA = StringUtil_fill<char>;
  fog_api.stringutil_fillW = StringUtil_fill<CharW>;

  fog_api.stringutil_lenA = StringUtil_len<char>;
  fog_api.stringutil_lenW = StringUtil_len<CharW>;

  fog_api.stringutil_nLenA = StringUtil_nLen<char>;
  fog_api.stringutil_nLenW = StringUtil_nLen<CharW>;

  fog_api.stringutil_eqA[CASE_SENSITIVE  ] = StringUtil_eqA_cs;
  fog_api.stringutil_eqA[CASE_INSENSITIVE] = StringUtil_eqA_ci;

  fog_api.stringutil_eqW[CASE_SENSITIVE  ] = StringUtil_eqW_cs;
  fog_api.stringutil_eqW[CASE_INSENSITIVE] = StringUtil_eqW_ci;

  fog_api.stringutil_eqMixed[CASE_SENSITIVE  ] = StringUtil_eqMixed_cs;
  fog_api.stringutil_eqMixed[CASE_INSENSITIVE] = StringUtil_eqMixed_ci;

  fog_api.stringutil_countOfA[CASE_SENSITIVE  ] = StringUtil_countOfA_cs;
  fog_api.stringutil_countOfA[CASE_INSENSITIVE] = StringUtil_countOfA_ci;

  fog_api.stringutil_countOfW[CASE_SENSITIVE  ] = StringUtil_countOfW_cs;
  fog_api.stringutil_countOfW[CASE_INSENSITIVE] = StringUtil_countOfW_ci;

  fog_api.stringutil_indexOfCharA[CASE_SENSITIVE  ] = StringUtil_indexOfCharA_cs;
  fog_api.stringutil_indexOfCharA[CASE_INSENSITIVE] = StringUtil_indexOfCharA_ci;

  fog_api.stringutil_indexOfCharW[CASE_SENSITIVE  ] = StringUtil_indexOfCharW_cs;
  fog_api.stringutil_indexOfCharW[CASE_INSENSITIVE] = StringUtil_indexOfCharW_ci;

  fog_api.stringutil_indexOfStringA[CASE_SENSITIVE  ] = StringUtil_indexOfStringT_cs<char, char>;
  fog_api.stringutil_indexOfStringA[CASE_INSENSITIVE] = StringUtil_indexOfStringA_ci;

  fog_api.stringutil_indexOfStringWA[CASE_SENSITIVE  ] = StringUtil_indexOfStringT_cs<CharW, char>;
  fog_api.stringutil_indexOfStringWA[CASE_INSENSITIVE] = StringUtil_indexOfStringW_ci<char>;

  fog_api.stringutil_indexOfStringW[CASE_SENSITIVE  ] = StringUtil_indexOfStringT_cs<CharW, CharW>;
  fog_api.stringutil_indexOfStringW[CASE_INSENSITIVE] = StringUtil_indexOfStringW_ci<CharW>;

  fog_api.stringutil_indexOfAnyA[CASE_SENSITIVE  ] = StringUtil_indexOfAnyA_cs;
  fog_api.stringutil_indexOfAnyA[CASE_INSENSITIVE] = StringUtil_indexOfAnyA_ci;

  fog_api.stringutil_indexOfAnyW[CASE_SENSITIVE  ] = StringUtil_indexOfAnyW_cs;
  fog_api.stringutil_indexOfAnyW[CASE_INSENSITIVE] = StringUtil_indexOfAnyW_ci;

  fog_api.stringutil_lastIndexOfCharA[CASE_SENSITIVE  ] = StringUtil_lastIndexOfCharA_cs;
  fog_api.stringutil_lastIndexOfCharA[CASE_INSENSITIVE] = StringUtil_lastIndexOfCharA_ci;

  fog_api.stringutil_lastIndexOfCharW[CASE_SENSITIVE  ] = StringUtil_lastIndexOfCharW_cs;
  fog_api.stringutil_lastIndexOfCharW[CASE_INSENSITIVE] = StringUtil_lastIndexOfCharW_ci;

  fog_api.stringutil_lastIndexOfStringA[CASE_SENSITIVE  ] = StringUtil_lastIndexOfStringT_cs<char, char>;
  fog_api.stringutil_lastIndexOfStringA[CASE_INSENSITIVE] = StringUtil_lastIndexOfStringA_ci;

  fog_api.stringutil_lastIndexOfStringWA[CASE_SENSITIVE  ] = StringUtil_lastIndexOfStringT_cs<CharW, char>;
  fog_api.stringutil_lastIndexOfStringWA[CASE_INSENSITIVE] = StringUtil_lastIndexOfStringW_ci<char>;

  fog_api.stringutil_lastIndexOfStringW[CASE_SENSITIVE  ] = StringUtil_lastIndexOfStringT_cs<CharW, CharW>;
  fog_api.stringutil_lastIndexOfStringW[CASE_INSENSITIVE] = StringUtil_lastIndexOfStringW_ci<CharW>;

  fog_api.stringutil_lastIndexOfAnyA[CASE_SENSITIVE  ] = StringUtil_lastIndexOfAnyA_cs;
  fog_api.stringutil_lastIndexOfAnyA[CASE_INSENSITIVE] = StringUtil_lastIndexOfAnyA_ci;

  fog_api.stringutil_lastIndexOfAnyW[CASE_SENSITIVE  ] = StringUtil_lastIndexOfAnyW_cs;
  fog_api.stringutil_lastIndexOfAnyW[CASE_INSENSITIVE] = StringUtil_lastIndexOfAnyW_ci;

  fog_api.stringutil_validateUtf8 = StringUtil_validateUtf8;
  fog_api.stringutil_validateUtf16 = StringUtil_validateUtf16;

  fog_api.stringutil_isCanonical = StringUtil_isCanonical;
  fog_api.stringutil_makeCanonical = StringUtil_makeCanonical;

  fog_api.stringutil_isNormalized = StringUtil_isNormalized;
  fog_api.stringutil_quickCheck = StringUtil_quickCheck;

  fog_api.stringutil_ucsFromUtf8Length = StringUtil_ucsFromUtf8Length;
  fog_api.stringutil_ucsFromUtf16Length = StringUtil_ucsFromUtf16Length;

  fog_api.stringutil_itoa = StringUtil_itoa;
  fog_api.stringutil_utoa = StringUtil_utoa;

  fog_api.stringutil_parseBoolA = StringUtil_parseBool<char>;
  fog_api.stringutil_parseBoolW = StringUtil_parseBool<CharW>;

  fog_api.stringutil_parseI8A = StringUtil_parseI8<char>;
  fog_api.stringutil_parseI8W = StringUtil_parseI8<CharW>;

  fog_api.stringutil_parseU8A = StringUtil_parseU8<char>;
  fog_api.stringutil_parseU8W = StringUtil_parseU8<CharW>;

  fog_api.stringutil_parseI16A = StringUtil_parseI16<char>;
  fog_api.stringutil_parseI16W = StringUtil_parseI16<CharW>;

  fog_api.stringutil_parseU16A = StringUtil_parseU16<char>;
  fog_api.stringutil_parseU16W = StringUtil_parseU16<CharW>;

  fog_api.stringutil_parseI32A = StringUtil_parseI32<char>;
  fog_api.stringutil_parseI32W = StringUtil_parseI32<CharW>;

  fog_api.stringutil_parseU32A = StringUtil_parseU32<char>;
  fog_api.stringutil_parseU32W = StringUtil_parseU32<CharW>;

  fog_api.stringutil_parseI64A = StringUtil_parseI64<char>;
  fog_api.stringutil_parseI64W = StringUtil_parseI64<CharW>;

  fog_api.stringutil_parseU64A = StringUtil_parseU64<char>;
  fog_api.stringutil_parseU64W = StringUtil_parseU64<CharW>;
}

} // Fog namespace
