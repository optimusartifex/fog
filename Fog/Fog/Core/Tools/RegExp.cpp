// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/RegExp.h>
#include <Fog/Core/Tools/VarId.h>

namespace Fog {

// ============================================================================
// [Fog::RegExp - Global]
// ============================================================================

static Static<RegExpDataA> RegExpA_dEmpty;
static Static<RegExpDataW> RegExpW_dEmpty;

static Static<RegExpA> RegExpA_oEmpty;
static Static<RegExpW> RegExpW_oEmpty;

template<typename CharT>
FOG_STATIC_INLINE_T CharT_(RegExpData)* RegExpT_getDEmpty() { return NULL; }

template<>
FOG_STATIC_INLINE_T RegExpDataA* RegExpT_getDEmpty<char>() { return &RegExpA_dEmpty; }

template<>
FOG_STATIC_INLINE_T RegExpDataW* RegExpT_getDEmpty<CharW>() { return &RegExpW_dEmpty; }

// ============================================================================
// [Fog::RegExp - Implementation - Constants]
// ============================================================================

typedef uint8_t BM_Skip;

enum
{
  //! @brief How many bits has one size_t integer (machine specific).
  REGEXP_SIZE_T_BITS = sizeof(size_t) * 8,

  //! @brief Maximum characters which can be skipped by boyer-moore  algorithm.
  REGEXP_BOYER_MOORE_MAX = 255,

  //! @brief The range used for character-mask (covers the whole unsigned char).
  REGEXP_MASK_SIZE_A = 256,
  //! @brief The range used for character-mask (covers many world alphabets).
  REGEXP_MASK_SIZE_W = 1024
};

enum REGEXP_INTERNAL
{
  REGEXP_INTERNAL_NONE     = REGEXP_TYPE_NONE,
  REGEXP_INTERNAL_PATTERN  = REGEXP_TYPE_PATTERN,
  REGEXP_INTERNAL_WILDCARD = REGEXP_TYPE_WILDCARD,
  REGEXP_INTERNAL_REGEXP   = REGEXP_TYPE_REGEXP,

  REGEXP_INTERNAL_CHAR     = 0x10
};

// ============================================================================
// [Fog::RegExp - Implementation - Common]
// ============================================================================

template<typename CharT>
static void FOG_CDECL RegExpCommonT_destroy(CharT_(RegExpData)* d)
{
  d->pattern.destroy();
  MemMgr::free(d);
}

template<typename CharT>
static bool FOG_CDECL RegExpCommonT_match(CharT_(RegExpData)* d, const CharT* sData, size_t sLength, const Range* sRange, Range* out)
{
  out->setRange(INVALID_INDEX, INVALID_INDEX);
  return false;
}

// ============================================================================
// [Fog::RegExp - Implementation - Char]
// ============================================================================

struct FOG_NO_EXPORT RegExpCharSingleDataA : public RegExpDataA
{
  char single;
};

struct FOG_NO_EXPORT RegExpCharSingleDataW : public RegExpDataW
{
  CharW single;
};

_FOG_CHAR_T(RegExpCharSingleData)
_FOG_CHAR_A(RegExpCharSingleData)
_FOG_CHAR_W(RegExpCharSingleData)

struct FOG_NO_EXPORT RegExpCharArrayDataA : public RegExpDataA
{
  size_t mask[REGEXP_MASK_SIZE_A / REGEXP_SIZE_T_BITS];
};

struct FOG_NO_EXPORT RegExpCharArrayDataW : public RegExpDataW
{
  size_t mask[REGEXP_MASK_SIZE_W / REGEXP_SIZE_T_BITS];

  size_t extraSize;
  CharW extraData[1];
};

_FOG_CHAR_T(RegExpCharArrayData)
_FOG_CHAR_A(RegExpCharArrayData)
_FOG_CHAR_W(RegExpCharArrayData)

template<typename CharT>
static bool FOG_CDECL RegExpCharT_match_single(CharT_(RegExpData)* _d, const CharT* sData, size_t sLength, const Range* sRange, Range* out)
{
  CharT_(RegExpCharSingleData)* d = reinterpret_cast<CharT_(RegExpCharSingleData)*>(_d);

  const CharT* sCur = sData + sRange->getStart();
  const CharT* sEnd = sData + sRange->getEnd();
  const CharT* sEnd4 = sEnd - 3;

  CharT c = d->single;

  while (sCur < sEnd4)
  {
    if (sCur[0] == c) goto _Match0;
    if (sCur[1] == c) goto _Match1;
    if (sCur[2] == c) goto _Match2;
    if (sCur[3] == c) goto _Match3;
    sCur += 4;
  }

  while (sCur < sEnd)
  {
    if (sCur[0] == c) goto _Match0;
    sCur++;
  }

  out->setRange(INVALID_INDEX, INVALID_INDEX);
  return false;

_Match3:
  sCur++;

_Match2:
  sCur++;

_Match1:
  sCur++;

_Match0:
  {
    size_t i = (size_t)(sCur - sData);
    out->setRange(i, i + 1);
    return true;
  }
}

static bool FOG_CDECL RegExpCharA_match_array(RegExpDataA* _d, const char* sData, size_t sLength, const Range* sRange, Range* out)
{
  RegExpCharArrayDataA* d = reinterpret_cast<RegExpCharArrayDataA*>(_d);

  size_t i;
  const size_t* mask = d->mask;

  sLength = sRange->getEnd();
  for (i = sRange->getStart(); i < sLength; i++)
  {
    size_t c = sData[i];
    if (mask[c / REGEXP_SIZE_T_BITS] & ((size_t)1 << (c % REGEXP_SIZE_T_BITS)))
      goto _Match;
  }

  out->setRange(INVALID_INDEX, INVALID_INDEX);
  return false;

_Match:
  out->setRange(i, i + 1);
  return true;
}

static bool FOG_CDECL RegExpCharW_match_array(RegExpDataW* _d, const CharW* sData, size_t sLength, const Range* sRange, Range* out)
{
  RegExpCharArrayDataW* d = reinterpret_cast<RegExpCharArrayDataW*>(_d);

  size_t i;
  size_t extraSize = d->extraSize;

  const size_t* mask = d->mask;
  const CharW* extraData = d->extraData;

  sLength = sRange->getEnd();
  for (i = sRange->getStart(); i < sLength; i++)
  {
    size_t c = sData[i];

    if (c < REGEXP_MASK_SIZE_W)
    {
      if (mask[c / REGEXP_SIZE_T_BITS] & ((size_t)1 << (c % REGEXP_SIZE_T_BITS)))
        goto _Match;
    }
    else
    {
      for (size_t j = 0; j < extraSize; j++)
        if (c == extraData[j])
          goto _Match;
    }
  }

  out->setRange(INVALID_INDEX, INVALID_INDEX);
  return false;

_Match:
  out->setRange(i, i + 1);
  return true;
}

template<typename CharT>
static CharT_(RegExpData)* RegExpCharT_create(const CharT_(String)* pattern, uint32_t type, const CharT* charArray, size_t charLength, uint32_t cs)
{
  FOG_ASSERT(charArray != NULL);
  FOG_ASSERT(charLength > 0);

  // --------------------------------------------------------------------------
  // [SingleChar]
  // --------------------------------------------------------------------------

  if (charLength == 1)
  {
    CharT c = charArray[0];
    if (cs == CASE_SENSITIVE || CharT_Func::toLower(c) == CharT_Func::toUpper(c))
    {
      CharT_(RegExpCharSingleData)* d = reinterpret_cast<CharT_(RegExpCharSingleData)*>(
        MemMgr::alloc(sizeof(CharT_(RegExpCharSingleData)))
      );

      if (FOG_IS_NULL(d))
        return NULL;

      // Initialize the data.
      d->reference.init(1);
      d->vType = VarId<CharT_(RegExp)>::ID | VAR_FLAG_NONE;
      d->type = (uint8_t)type;
      d->internalType = REGEXP_INTERNAL_CHAR;
      d->caseSensitivity = (uint8_t)cs;
      d->reserved = 0;

      d->destroy = RegExpCommonT_destroy<CharT>;
      d->match = RegExpCharT_match_single<CharT>;

      d->pattern.initCustom1(*pattern);
      d->fixedLength = 1;

      d->single = c;

      return d;
    }
  }

  // --------------------------------------------------------------------------
  // [MultiChar]
  // --------------------------------------------------------------------------

  {
    size_t dsize = sizeof(CharT_(RegExpCharArrayData));

    if (sizeof(CharT) != 1)
    {
      // Reserve space for extra characters. When matching case-insensitive, it's
      // needed to reserve extra space for lowercase, uppercase, and titlecase
      // characters.
      if (cs == CASE_SENSITIVE)
        dsize += charLength * sizeof(CharT);
      else
        dsize += charLength * sizeof(CharT) * 3; // Upper/Lower/Title.
    }

    CharT_(RegExpCharArrayData)* d = reinterpret_cast<CharT_(RegExpCharArrayData)*>(
      MemMgr::alloc(dsize)
    );

    if (FOG_IS_NULL(d))
      return NULL;

    // Initialize the data.
    d->reference.init(1);
    d->vType = VarId<CharT_(RegExp)>::ID | VAR_FLAG_NONE;
    d->type = (uint8_t)type;
    d->internalType = REGEXP_INTERNAL_CHAR;
    d->caseSensitivity = (uint8_t)cs;
    d->reserved = 0;

    d->destroy = RegExpCommonT_destroy<CharT>;

    if (sizeof(CharT) == 1)
      d->match = (CharT_(RegExpData)::Match)RegExpCharA_match_array;
    else
      d->match = (CharT_(RegExpData)::Match)RegExpCharW_match_array;

    d->pattern.initCustom1(*pattern);
    d->fixedLength = 1;

    size_t* mask = d->mask;
    CharT* extra;
    if (sizeof(CharT) != 1) extra = (CharT*)reinterpret_cast<RegExpCharArrayDataW*>(d)->extraData;

    size_t i;

    for (i = 0; i < FOG_ARRAY_SIZE(d->mask); i++)
    {
      mask[i] = 0;
    }

    // NOTE: Mask of CharW implementation is limited to REGEXP_MASK_SIZE_W.
    // All characters which do not fit into this criteria are added to the
    // 'extra' buffer and matched one-by-one.
    if (cs == CASE_SENSITIVE)
    {
      for (i = 0; i < charLength; i++)
      {
        CharT_Value c = charArray[i];

        if (sizeof(CharT) != 1 && c >= REGEXP_MASK_SIZE_W)
          *extra++ = c;
        else
          mask[c / REGEXP_SIZE_T_BITS] |= ((size_t)1 << (c % REGEXP_SIZE_T_BITS));
      }
    }
    else
    {
      for (i = 0; i < charLength; i++)
      {
        CharT_Value c = charArray[i];

        if (sizeof(CharT) == 1)
        {
          CharT_Value cLower = CharT_Func::toLower(c);
          CharT_Value cUpper = CharT_Func::toUpper(c);

          mask[cLower / REGEXP_SIZE_T_BITS] |= ((size_t)1 << (cLower % REGEXP_SIZE_T_BITS));
          mask[cUpper / REGEXP_SIZE_T_BITS] |= ((size_t)1 << (cUpper % REGEXP_SIZE_T_BITS));
        }
        else
        {
          // Unicode contains lowercase, uppercase, and titlecase characters. We
          // need to match all of them.
          uint16_t cLower = CharW::toLower((uint16_t)c);
          uint16_t cUpper = CharW::toUpper((uint16_t)c);
          uint16_t cTitle = CharW::toTitle((uint16_t)c);

          if (cLower != cTitle)
          {
            if (cLower >= REGEXP_MASK_SIZE_W)
              *extra++ = (CharT_Value)cLower;
            else
              mask[cLower / REGEXP_SIZE_T_BITS] |= ((size_t)1 << (cLower % REGEXP_SIZE_T_BITS));
          }

          if (cUpper != cTitle)
          {
            if (cUpper >= REGEXP_MASK_SIZE_W)
              *extra++ = (CharT_Value)cUpper;
            else
              mask[cUpper / REGEXP_SIZE_T_BITS] |= ((size_t)1 << (cUpper % REGEXP_SIZE_T_BITS));
          }

          if (true)
          {
            if (cTitle >= REGEXP_MASK_SIZE_W)
              *extra++ = (CharT_Value)cTitle;
            else
              mask[cTitle / REGEXP_SIZE_T_BITS] |= ((size_t)1 << (cTitle % REGEXP_SIZE_T_BITS));
          }
        }
      }
    }

    if (sizeof(CharT) != 1)
      reinterpret_cast<RegExpCharArrayDataW*>(d)->extraSize =
        (size_t)((CharW*)extra - reinterpret_cast<RegExpCharArrayDataW*>(d)->extraData);

    return d;
  }
}

// ============================================================================
// [Fog::RegExp - Implementation - Pattern]
// ============================================================================

// Pattern matching is implemented using efficient Boyer-Moore algorithm. There
// is only initialization overhead, which is needed to build a skip table, which
// is used to skip parts of an input text where is no match. This algorithm is
// very efficient if the pattern is long, because many parts of text can be
// entirely skipped, but not as efficient for small patterns, or patterns which
// contain repeating sequence(s).
//
// The skip value was choosen to be unsigned char, which can hold integer up to
// 255, so our implementation can skip max 255 bytes per iteration. This has
// several reasons. The first is that the skip table is only 256 bytes long.
// This improves cache locality (it's a lookup table, smaller == better), the
// other advantage is memory improvement without performance overhead. We could
// extend BM_Skip to uint16_t in case that there is be problems with long pattern
// matchin (but I do not expect that).

struct FOG_NO_EXPORT RegExpPatternDataA : public RegExpDataA
{
  //! @brief Boyer-Moore Skip table.
  BM_Skip skip[256];
};

struct FOG_NO_EXPORT RegExpPatternDataW : public RegExpDataW
{
  //! @brief Boyer-Moore Skip table.
  BM_Skip skip[256];
};

_FOG_CHAR_T(RegExpPatternData)
_FOG_CHAR_A(RegExpPatternData)
_FOG_CHAR_W(RegExpPatternData)

template<typename CharT>
static bool FOG_CDECL RegExpPatternT_match_cs(CharT_(RegExpData)* _d, const CharT* sData, size_t sLength, const Range* sRange, Range* out)
{
  CharT_(RegExpPatternData)* d = reinterpret_cast<CharT_(RegExpPatternData)*>(_d);

  FOG_ASSERT(sLength != DETECT_LENGTH);
  FOG_ASSERT(sRange->isValid());
  FOG_ASSERT(sRange->getStart() <= sLength);

  size_t pLength = d->fixedLength;

  FOG_ASSERT(pLength == d->pattern->getLength());
  FOG_ASSERT(pLength > 1);

  // Simple reject.
  if (pLength > sLength)
  {
    out->setRange(INVALID_INDEX, INVALID_INDEX);
    return false;
  }

  const CharT* sCur = sData + sRange->getStart() + pLength - 1;
  const CharT* sEnd = sData + sRange->getEnd();

  const CharT* pData = d->pattern->getData();
  const BM_Skip* skip = d->skip;

  do {
    size_t i = sizeof(CharT) == 1 ? skip[(CharT_Value)*sCur       ]
                                  : skip[(CharT_Value)*sCur & 0xFF];

    sCur += i;
    if (i == 0)
    {
      const CharT* s = ++sCur - pLength;

      for (;;)
      {
        if (s[i] != pData[i])
          break;

        if (++i != pLength - (sizeof(CharT) == 1))
          continue;

        // Match.
        i = (size_t)(s - sData);
        out->setRange(i, i + pLength);
        return true;
      }
    }
  } while (sCur < sEnd);

  out->setRange(INVALID_INDEX, INVALID_INDEX);
  return false;
}

template<typename CharT>
static bool FOG_CDECL RegExpPatternT_match_ci(CharT_(RegExpData)* _d, const CharT* sData, size_t sLength, const Range* sRange, Range* out)
{
  CharT_(RegExpPatternData)* d = reinterpret_cast<CharT_(RegExpPatternData)*>(_d);

  FOG_ASSERT(sLength != DETECT_LENGTH);
  FOG_ASSERT(sRange->isValid());
  FOG_ASSERT(sRange->getStart() <= sLength);

  size_t pLength = d->fixedLength;

  FOG_ASSERT(pLength == d->pattern->getLength());
  FOG_ASSERT(pLength > 1);

  // Simple reject.
  if (pLength > sLength)
  {
    out->setRange(INVALID_INDEX, INVALID_INDEX);
    return false;
  }

  const CharT* sCur = sData + sRange->getStart() + pLength - 1;
  const CharT* sEnd = sData + sRange->getEnd();

  const CharT* pData = d->pattern->getData();
  const BM_Skip* skip = d->skip;

  do {
    size_t i = sizeof(CharT) == 1 ? skip[(CharT_Value)CharT_Func::toLower(*sCur)       ]
                                  : skip[(CharT_Value)CharT_Func::toLower(*sCur) & 0xFF];

    sCur += i;
    if (i == 0)
    {
      const CharT* s = ++sCur - pLength;

      for (;;)
      {
        if (CharT_Func::toLower(s[i]) != CharT_Func::toLower(pData[i]))
          break;

        if (++i != pLength - (sizeof(CharT) == 1))
          continue;

        // Match.
        i = (size_t)(s - sData);
        out->setRange(i, i + pLength);
        return true;
      }
    }
  } while (sCur < sEnd);

  out->setRange(INVALID_INDEX, INVALID_INDEX);
  return false;
}

template<typename CharT>
static CharT_(RegExpData)* RegExpPatternT_create(const CharT_(String)* pattern, uint32_t type, uint32_t cs)
{
  CharT_(RegExpPatternData)* d = reinterpret_cast<CharT_(RegExpPatternData)*>(
    MemMgr::alloc(sizeof(CharT_(RegExpPatternData)))
  );

  if (FOG_IS_NULL(d))
    return NULL;

  const CharT* pData = pattern->getData();
  size_t pLength = pattern->getLength();

  // Caller must guarantee that the pattern length is greater than 1.
  FOG_ASSERT(pLength > 1);

  // Initialize the data.
  d->reference.init(1);
  d->vType = VarId<CharT_(RegExp)>::ID | VAR_FLAG_NONE;
  d->type = (uint8_t)type;
  d->internalType = REGEXP_INTERNAL_PATTERN;
  d->caseSensitivity = (uint8_t)cs;
  d->reserved = 0;

  d->destroy = RegExpCommonT_destroy<CharT>;
  d->match = (cs == CASE_SENSITIVE)
    ? (CharT_(RegExpData)::Match)RegExpPatternT_match_cs<CharT>
    : (CharT_(RegExpData)::Match)RegExpPatternT_match_ci<CharT>;

  d->pattern.initCustom1(*pattern);
  d->fixedLength = pLength;

  // Initialize the Boyer-Moore skip table.
  size_t i;

  BM_Skip* skip = d->skip;
  BM_Skip skipMax = (BM_Skip)Math::min<size_t>(pLength, REGEXP_BOYER_MOORE_MAX);

  // Fill the table using the maximum skip value.
  for (i = 0; i < 256 / 4; i++)
  {
    skip[0] = skipMax;
    skip[1] = skipMax;
    skip[2] = skipMax;
    skip[3] = skipMax;
    skip += 4;
  }

  // Now initialize the skip table using the characters found in the 'pattern'.
  skip = d->skip;
  i = pLength;

  if (cs == CASE_SENSITIVE)
  {
    do {
      skip[(CharT_Value)*pData++] = (BM_Skip)Math::min<size_t>(--i, REGEXP_BOYER_MOORE_MAX);
    } while (i);
  }
  else
  {
    do {
      skip[(CharT_Value)CharT_Func::toLower(*pData++)] = (BM_Skip)Math::min<size_t>(--i, REGEXP_BOYER_MOORE_MAX);
    } while (i);
  }

  return d;
}

// ============================================================================
// [Fog::RegExp - Construction / Destruction]
// ============================================================================

template<typename CharT>
static void FOG_CDECL RegExpT_ctor(CharT_(RegExp)* self)
{
  self->_d = RegExpT_getDEmpty<CharT>()->addRef();
}

template<typename CharT>
static void FOG_CDECL RegExpT_ctorCopy(CharT_(RegExp)* self, const CharT_(RegExp)* other)
{
  self->_d = other->_d->addRef();
}

template<typename CharT>
static void FOG_CDECL RegExpT_dtor(CharT_(RegExp)* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::RegExp - Reset]
// ============================================================================

template<typename CharT>
static void FOG_CDECL RegExpT_reset(CharT_(RegExp)* self)
{
  atomicPtrXchg(&self->_d, RegExpT_getDEmpty<CharT>()->addRef())->release();
}

// ============================================================================
// [Fog::RegExp - Copy]
// ============================================================================

template<typename CharT>
static void FOG_CDECL RegExpT_copy(CharT_(RegExp)* self, const CharT_(RegExp)* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
}

// ============================================================================
// [Fog::RegExp - Create]
// ============================================================================

template<typename CharT, typename SrcT>
static err_t FOG_CDECL RegExpT_createStub(CharT_(RegExp)* self, const SrcT_(Stub)* pattern, uint32_t type, uint32_t cs)
{
  if (type >= REGEXP_TYPE_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  CharT_(String) pStr;
  FOG_RETURN_ON_ERROR(pStr._set(*pattern));
  return self->create(pStr, type, cs);
}

template<typename CharT, typename SrcT>
static err_t FOG_CDECL RegExpT_createString(CharT_(RegExp)* self, const SrcT_(String)* pattern, uint32_t type, uint32_t cs)
{
  if (type >= REGEXP_TYPE_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  if (cs >= CASE_SENSITIVITY_COUNT)
    cs = CASE_INSENSITIVE;

  CharT_(RegExpData)* d = NULL;

  size_t pLength = pattern->getLength();
  if (pLength == 0) goto _None;

  switch (type)
  {
    case REGEXP_TYPE_NONE:
    {
_None:
      self->reset();
      return ERR_OK;
    }

    case REGEXP_TYPE_PATTERN:
    {
      if (pLength == 1)
        d = RegExpCharT_create<CharT>(pattern, REGEXP_TYPE_PATTERN, pattern->getData(), 1, cs);
      else
        d = RegExpPatternT_create<CharT>(pattern, REGEXP_TYPE_PATTERN, cs);

      if (FOG_IS_NULL(d))
      {
        self->reset();
        return ERR_RT_OUT_OF_MEMORY;
      }
      break;
    }

    case REGEXP_TYPE_WILDCARD:
    {
      // TODO:
      return ERR_RT_NOT_IMPLEMENTED;
    }

    case REGEXP_TYPE_REGEXP:
    {
      // TODO:
      return ERR_RT_NOT_IMPLEMENTED;
    }
  }

  FOG_ASSERT(d != NULL);

  atomicPtrXchg(&self->_d, d)->release();
  return ERR_OK;
}




// ============================================================================
// [Fog::RegExp - IndexIn]
// ============================================================================

template<typename CharT>
static bool FOG_CDECL RegExpT_indexIn(const CharT_(RegExp)* self, const CharT* sData, size_t sLength, const Range* sRange, Range* out)
{
  CharT_(RegExpData)* d = self->_d;
  size_t rStart, rEnd;

  if (sRange == NULL)
  {
    rStart = 0;
    rEnd = sLength;
  }
  else
  {
    rStart = sRange->getStart();
    rEnd = Math::min(sRange->getEnd(), sLength);
  }

  if (rStart >= rEnd)
  {
    out->setRange(INVALID_INDEX, INVALID_INDEX);
    return false;
  }

  Range newRange(rStart, rEnd);
  return d->match(d, sData, sLength, &newRange, out);
}

// ============================================================================
// [Fog::RegExp - LastIndexIn]
// ============================================================================

template<typename CharT>
static bool FOG_CDECL RegExpT_lastIndexIn(const CharT_(RegExp)* self, const CharT* sData, size_t sLength, const Range* sRange, Range* out)
{
  CharT_(RegExpData)* d = self->_d;
  size_t rStart, rEnd;

  if (sRange == NULL)
  {
    rStart = 0;
    rEnd = sLength;
  }
  else
  {
    rStart = sRange->getStart();
    rEnd = Math::min(sRange->getEnd(), sLength);
  }

  out->setRange(INVALID_INDEX, INVALID_INDEX);

  if (rStart >= rEnd)
    return false;

  for (;;)
  {
    Range r(rStart, rEnd);
    if (!d->match(d, sData, sLength, &r, &r))
      break;

    rStart = r.getEnd();
    *out = r;
  }

  return out->getStart() != INVALID_INDEX;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void RegExp_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.regexpa.ctor = RegExpT_ctor<char>;
  _api.regexpa.ctorCopy = RegExpT_ctorCopy<char>;
  _api.regexpa.dtor = RegExpT_dtor<char>;
  _api.regexpa.reset = RegExpT_reset<char>;
  _api.regexpa.copy = RegExpT_copy<char>;
  _api.regexpa.createStubA = RegExpT_createStub<char, char>;
  _api.regexpa.createStringA = RegExpT_createString<char, char>;
  _api.regexpa.indexIn = RegExpT_indexIn<char>;
  _api.regexpa.lastIndexIn = RegExpT_lastIndexIn<char>;

  _api.regexpw.ctor = RegExpT_ctor<CharW>;
  _api.regexpw.ctorCopy = RegExpT_ctorCopy<CharW>;
  _api.regexpw.dtor = RegExpT_dtor<CharW>;
  _api.regexpw.reset = RegExpT_reset<CharW>;
  _api.regexpw.copy = RegExpT_copy<CharW>;
  _api.regexpw.createStubA = RegExpT_createStub<CharW, char>;
  _api.regexpw.createStubW = RegExpT_createStub<CharW, CharW>;
  _api.regexpw.createStringW = RegExpT_createString<CharW, CharW>;
  _api.regexpw.indexIn = RegExpT_indexIn<CharW>;
  _api.regexpw.lastIndexIn = RegExpT_lastIndexIn<CharW>;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  RegExpDataA* da = &RegExpA_dEmpty;
  RegExpDataW* dw = &RegExpW_dEmpty;

  da->reference.init(1);
  da->vType = VAR_TYPE_REGEXPA | VAR_FLAG_NONE;
  da->type = REGEXP_TYPE_NONE;
  da->internalType = REGEXP_INTERNAL_NONE;
  da->caseSensitivity = CASE_SENSITIVE;
  da->reserved = 0;

  da->destroy = NULL;
  da->match = RegExpCommonT_match<char>;

  da->pattern->_d = _api.stringa.oEmpty->_d;
  da->fixedLength = INVALID_INDEX;

  dw->reference.init(1);
  dw->vType = VAR_TYPE_REGEXPW | VAR_FLAG_NONE;
  dw->type = REGEXP_TYPE_NONE;
  dw->internalType = REGEXP_INTERNAL_NONE;
  dw->caseSensitivity = CASE_SENSITIVE;
  dw->reserved = 0;

  dw->destroy = NULL;
  dw->match = RegExpCommonT_match<CharW>;

  dw->pattern->_d = _api.stringw.oEmpty->_d;
  dw->fixedLength = INVALID_INDEX;

  _api.regexpa.oEmpty = RegExpA_oEmpty.initCustom1(da);
  _api.regexpw.oEmpty = RegExpW_oEmpty.initCustom1(dw);
}

} // Fog namespace
