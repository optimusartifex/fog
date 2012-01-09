// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/CharData.h>
#include <Fog/Core/Tools/CharUtil.h>
#include <Fog/Core/Tools/CharUtilPrivate_p.h>

namespace Fog {

// ============================================================================
// [Fog::CharUtil - ISO-15924]
// ============================================================================

static uint32_t FOG_CDECL CharUtil_getIso15924FromTextScript(uint32_t script)
{
  if (FOG_UNLIKELY(script >= TEXT_SCRIPT_COUNT))
    return 0x00000000;
  else
    return _charData.iso15924FromTextScript[script];
}

static uint32_t FOG_CDECL CharUtil_getTextScriptFromIso15924(uint32_t iso15924)
{
  const CharISO15924Data* data = _charData.textScriptFromIso15924;
  iso15924 = CharUtil::getNormalizedISO15924Name(iso15924);

  for (uint32_t i = 0; i < FOG_ARRAY_SIZE(_charData.textScriptFromIso15924); i++)
  {
    if (iso15924 == data[i].getIso15924())
      return data[i].getScript();
  }
  
  return TEXT_SCRIPT_UNKNOWN;
}

// ============================================================================
// [Fog::CharUtil - Hangul Notes]
// ============================================================================

// Hangul syllable decomposition from unicode mailing list (msg 25765):
//   http://www.mail-archive.com/unicode@unicode.org/msg25765.html
//
// Other interesting links about Hangul:
//   http://www.uni-graz.at/~katzer/korean_hangul_unicode.html
//
// Hangul contains:
//
//   - 19 Lead jamos (L)         - starts at 0x1100.
//   - 21 Vowel jamos (V)        - starts at 0x1161.
//   - 27 Tail jamos (T)         - starts at 0x11A7.
//   - 1117s syllables (LV, LVT) - starts at 0xAC00.
//
// If we index each jamo from [0..count), then the composed unicode 
// character (syllable) can be calculated as:
//
//   - LV  == 0xAC00 + L * 28 * 21 + V * 28
//   - LVT == 0xAC00 + L * 28 * 21 + V * 28 + T + 1
//
// Note that count of tail jamos was increased by 1 to support characters 
// without the tail jamo (LV only), these are valid syllables as well.
//
// So to check whether the Hangul syllable is LV or LVT, simple modulo check 
// can be performed (note the input char is expected to be LV or LVT):
//
//   - bool isSyllableLV (uint16_t c) { return ((c - 0xAC00) % 28) == 0; }
//   - bool isSyllableLVT(uint16_t c) { return ((c - 0xAC00) % 28) != 0; }
//
// Each Hangul precomposed LV syllable has a canonical decomposition into an L+V:
//
// LV:  - L  in U+1100 – U+1112: LBase + ((c – SBase) / NCount)
//      - V  in U+1161 – U+1175: VBase + ((c – SBase) % NCount) / (TCount + 1)
//
// Each Hangul precomposed LVT syllable has a canonical decomposition into an LV+T:
//
// LVT: - LV                   : SBase + (((c – SBase) / NCount) * NCount)
//      - T  in U+11A7 – U+11C2: TBase + ( (c – SBase) % (TCount + 1))

// ============================================================================
// [Fog::CharUtil - Decomposition]
// ============================================================================

template<typename DstType>
static FOG_INLINE uint32_t CharUtil_decomposeHangul(uint16_t c, DstType* dst, uint32_t* decompositionType)
{
  uint16_t s = c - CHAR_HANGUL_BASE_S;

  uint16_t n = s / (CHAR_HANGUL_COUNT_T + 1);
  uint16_t t = s % (CHAR_HANGUL_COUNT_T + 1);

  if (t == 0)
  {
    // Decompose to L+V.
    dst[0] = CHAR_HANGUL_BASE_L + n / CHAR_HANGUL_COUNT_V;
    dst[1] = CHAR_HANGUL_BASE_V + n % CHAR_HANGUL_COUNT_V;
  }
  else
  {
    // Decompose to LV+T.
    dst[0] = s - t;
    dst[1] = t;
  }

  *decompositionType = CHAR_DECOMPOSITION_CANONICAL;
  return 2;
}

static uint32_t FOG_CDECL CharUtil_decomposeUcs2To16(uint16_t c, uint16_t* dst16, uint32_t* decompositionType)
{
  if (CharW::isHangulSyllable(c)) 
    return CharUtil_decomposeHangul<uint16_t>(c, dst16, decompositionType);

  const CharProperty& prop = _charData.getPropertyUCS2Unsafe(c);
  const uint16_t* data = prop.getDecompositionData();
  uint32_t length = *data++;

  for (size_t i = length; i; i--)
    *dst16++ = *data++;

  *decompositionType = prop.getDecompositionType();
  return length;
}

static uint32_t FOG_CDECL CharUtil_decomposeUcs2To32(uint16_t c, uint32_t* dst32, uint32_t* decompositionType)
{
  if (CharW::isHangulSyllable(c)) 
    return CharUtil_decomposeHangul<uint32_t>(c, dst32, decompositionType);

  const CharProperty& prop = _charData.getPropertyUCS2Unsafe(c);
  const uint16_t* data = prop.getDecompositionData();
  uint32_t length = *data++;

  for (size_t i = length; i; i--)
  {
    uint32_t uc = *data++;

    if (CharW::isHiSurrogate(uc))
    {
      uc = CharW::ucs4FromSurrogate(uc, static_cast<uint32_t>(*data++));
      i--;
      length--;
    }

    *dst32++ = uc;
  }

  *decompositionType = prop.getDecompositionType();
  return length;
}

static uint32_t FOG_CDECL CharUtil_decomposeUcs4To16(uint32_t c, uint16_t* dst16, uint32_t* decompositionType)
{
  if (c > UNICODE_MAX)
    return 0;

  if (CharW::isHangulSyllable(c)) 
    return CharUtil_decomposeHangul<uint16_t>(static_cast<uint16_t>(c), dst16, decompositionType);

  const CharProperty& prop = _charData.getPropertyUCS4Unsafe(c);
  const uint16_t* data = prop.getDecompositionData();
  uint32_t length = *data++;

  for (size_t i = length; i; i--)
    *dst16++ = *data++;

  *decompositionType = prop.getDecompositionType();
  return length;
}

static uint32_t FOG_CDECL CharUtil_decomposeUcs4To32(uint32_t c, uint32_t* dst32, uint32_t* decompositionType)
{
  if (c > UNICODE_MAX)
    return 0;

  if (CharW::isHangulSyllable(c)) 
    return CharUtil_decomposeHangul<uint32_t>(static_cast<uint16_t>(c), dst32, decompositionType);

  const CharProperty& prop = _charData.getPropertyUCS4Unsafe(c);
  const uint16_t* data = prop.getDecompositionData();
  uint32_t length = *data++;

  for (size_t i = length; i; i--)
  {
    uint32_t uc = *data++;

    if (CharW::isHiSurrogate(uc))
    {
      uc = CharW::ucs4FromSurrogate(uc, static_cast<uint32_t>(*data++));
      i--;
      length--;
    }

    *dst32++ = uc;
  }

  *decompositionType = prop.getDecompositionType();
  return length;
}

// ============================================================================
// [Fog::CharUtil - Composition]
// ============================================================================

static uint16_t FOG_CDECL CharUtil_composeUcs2(uint16_t a, uint16_t b)
{
  if (CharUtilPrivate::maybeHangul(a))
  {
    //   - LVT == 0xAC00 + L * 28 * 21 + V * 28 + T + 1

    // Handle hangul [L][V].
    uint32_t l = static_cast<uint32_t>(a) - CHAR_HANGUL_BASE_L;
    uint32_t v = static_cast<uint32_t>(b) - CHAR_HANGUL_BASE_V;

    if (l < CHAR_HANGUL_COUNT_L && v < CHAR_HANGUL_COUNT_V)
      return static_cast<uint16_t>(CHAR_HANGUL_BASE_S + (l * CHAR_HANGUL_COUNT_N) + v * (CHAR_HANGUL_COUNT_T + 1));

    // Handle hangul [LV][T].
    uint32_t s = static_cast<uint32_t>(a) - CHAR_HANGUL_BASE_S;
    uint32_t t = static_cast<uint32_t>(b) - CHAR_HANGUL_BASE_T;

    if (s < CHAR_HANGUL_COUNT_S && (s % (CHAR_HANGUL_COUNT_T + 1)) == 0 && t < CHAR_HANGUL_COUNT_T)
      return static_cast<uint16_t>(a + t + 1);
  }

  // Handle composition based on generated unicode tables.
  const CharProperty& prop = CharW::getPropertyUnsafe(b);
  uint32_t id = prop.getCompositionId();

  if (id == 0)
    return 0;
  FOG_ASSERT(id < FOG_ARRAY_SIZE(_charData.compositionIdToIndex) - 1);

  uint32_t start = _charData.compositionIdToIndex[id];
  uint32_t end   = _charData.compositionIdToIndex[id + 1];

  return static_cast<uint16_t>(CharUtilPrivate::composeBSearch(_charData.compositionData + start, end - start, a));
}

static uint32_t FOG_CDECL CharUtil_composeUcs4(uint32_t a, uint32_t b)
{
  if (b > UNICODE_MAX)
    return 0;

  if (CharUtilPrivate::maybeHangul(a))
  {
    // Handle hangul [L][V].
    uint32_t l = a - CHAR_HANGUL_BASE_L;
    uint32_t v = b - CHAR_HANGUL_BASE_V;

    if (l < CHAR_HANGUL_COUNT_L && v < CHAR_HANGUL_COUNT_V)
      return CHAR_HANGUL_BASE_S + (l * CHAR_HANGUL_COUNT_N) + v * (CHAR_HANGUL_COUNT_T + 1);

    // Handle hangul [LV][T].
    uint32_t s = a - CHAR_HANGUL_BASE_S;
    uint32_t t = b - CHAR_HANGUL_BASE_T;

    if (s < CHAR_HANGUL_COUNT_S && (s % (CHAR_HANGUL_COUNT_T + 1)) == 0 && t < CHAR_HANGUL_COUNT_T)
      return a + t + 1;
  }

  const CharProperty& prop = CharW::getPropertyUnsafe(b);
  uint32_t id = prop.getCompositionId();

  if (id == 0)
    return 0;
  FOG_ASSERT(id < FOG_ARRAY_SIZE(_charData.compositionIdToIndex) - 1);

  uint32_t start = _charData.compositionIdToIndex[id];
  uint32_t end   = _charData.compositionIdToIndex[id + 1];

  return CharUtilPrivate::composeBSearch(_charData.compositionData + start, end - start, a);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void CharUtil_init(void)
{
  fog_api.charutil_getIso15924FromTextScript = CharUtil_getIso15924FromTextScript;
  fog_api.charutil_getTextScriptFromIso15924 = CharUtil_getTextScriptFromIso15924;

  fog_api.charutil_decomposeUcs2To16 = CharUtil_decomposeUcs2To16;
  fog_api.charutil_decomposeUcs2To32 = CharUtil_decomposeUcs2To32;

  fog_api.charutil_decomposeUcs4To16 = CharUtil_decomposeUcs4To16;
  fog_api.charutil_decomposeUcs4To32 = CharUtil_decomposeUcs4To32;

  fog_api.charutil_composeUcs2 = CharUtil_composeUcs2;
  fog_api.charutil_composeUcs4 = CharUtil_composeUcs4;
}

} // Fog namespace
