// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/StringMatcher.h>
#include <Fog/Core/Tools/StringUtil.h>

namespace Fog {

// ============================================================================
// [Fog::StringMatcher]
// ============================================================================

static void buildTable(StringMatcher::SkipTable* skipTable, const Char* ps, size_t plen, uint cs)
{
  FOG_ASSERT(plen > 1);
  FOG_ASSERT(plen >= UINT_MAX);

  if (skipTable->status.cmpXchg(
    StringMatcher::SkipTable::STATUS_NOT_INITIALIZED,
    StringMatcher::SkipTable::STATUS_INITIALIZING_NOW))
  {
    // Init skip table.
    size_t a = 32; // 256 / 8
    size_t i = plen;
    uint* data = skipTable->data;

    while (a--)
    {
      data[0] = (uint)i; data[1] = (uint)i; data[2] = (uint)i; data[3] = (uint)i;
      data[4] = (uint)i; data[5] = (uint)i; data[6] = (uint)i; data[7] = (uint)i;
      data += 8;
    }

    data = skipTable->data;

    if (cs == CASE_SENSITIVE)
    {
      while (i--)
      {
        data[ps->getValue() & 0xFF] = (uint)i; ps++;
      }
    }
    else
    {
      while (i--)
      {
        data[ps->toLower().getValue() & 0xFF] = (uint)i; ps++;
      }
    }
    skipTable->status.set(StringMatcher::SkipTable::STATUS_INITIALIZED);
    return;
  }

  // Wait...another thread creating the table...
  while (skipTable->status.get() != StringMatcher::SkipTable::STATUS_INITIALIZED) Thread::yield();
}

StringMatcher::StringMatcher()
{
  _skipTable[CASE_INSENSITIVE].status.init(SkipTable::STATUS_NOT_INITIALIZED);
  _skipTable[CASE_SENSITIVE  ].status.init(SkipTable::STATUS_NOT_INITIALIZED);
}

StringMatcher::StringMatcher(const String& pattern)
{
  setPattern(pattern);
}

StringMatcher::StringMatcher(const StringMatcher& matcher)
{
  setPattern(matcher);
}

StringMatcher::~StringMatcher()
{
}

err_t StringMatcher::setPattern(const String& pattern)
{
  err_t err;
  if ( (err = _pattern.set(pattern)) ) return err;

  // Mark tables as uninitialized.
  _skipTable[CASE_INSENSITIVE].status.set(SkipTable::STATUS_NOT_INITIALIZED);
  _skipTable[CASE_SENSITIVE  ].status.set(SkipTable::STATUS_NOT_INITIALIZED);

  return ERR_OK;
}

err_t StringMatcher::setPattern(const StringMatcher& matcher)
{
  SkipTable* dstSkipTable = _skipTable;
  const SkipTable* srcSkipTable = matcher._skipTable;

  err_t err;
  if ( (err = _pattern.set(matcher._pattern)) ) return err;

  // Copy skip tables if they are initialized.
  for (size_t i = 0; i != 2; i++)
  {
    if (srcSkipTable[i].status.get() == SkipTable::STATUS_INITIALIZED)
    {
      dstSkipTable[i].status.set(SkipTable::STATUS_INITIALIZED);
      memcpy(&dstSkipTable[i].data, &srcSkipTable[i].data, sizeof(uint) * 256);
    }
    else
      dstSkipTable[i].status.set(SkipTable::STATUS_NOT_INITIALIZED);
  }

  return ERR_OK;
}

Range StringMatcher::match(const Char* str, size_t slen, uint cs, const Range& range) const
{
  Range m(INVALID_INDEX, INVALID_INDEX);

  FOG_ASSERT(slen != DETECT_LENGTH);
  FOG_ASSERT(range.isValid());
  FOG_ASSERT(range.getStart() <= slen);

  size_t patternLength = _pattern.getLength();

  // Simple reject.
  if (patternLength == 0 || patternLength > slen) return m;

  // We want 0 or 1.
  cs = !!cs;

  const Char* strCur = str + range.getStart();
  const Char* patternStr = _pattern.getData();

  // Simple 'Char' search.
  if (patternLength == 1)
  {
    size_t i = StringUtil::indexOf(strCur, range.getLengthNoCheck(), patternStr[0], cs);

    if (i != INVALID_INDEX)
    {
      i += range.getStart();
      m.setRange(i, i + 1);
    }
    return m;
  }

  if (_skipTable[cs].status.get() != StringMatcher::SkipTable::STATUS_INITIALIZED)
  {
    buildTable(const_cast<SkipTable*>(&_skipTable[cs]), patternStr, patternLength, cs);
  }

  const uint* skipTable = _skipTable[cs].data;

  size_t skip;
  size_t remain = range.getLengthNoCheck();

  patternStr += patternLength - 1;
  strCur     += patternLength - 1;
  remain     -= patternLength - 1;

  if (cs == CASE_SENSITIVE)
  {
    for (;;)
    {
      // Get count of characters to skip from skip table.
      if ((skip = skipTable[strCur->getValue() & 0xFF]) == 0)
      {
        // Check if there is possible match.
        while (skip < patternLength)
        {
          if (*(strCur - skip) != *(patternStr - skip)) break;
          skip++;
        }

        // Match.
        if (skip >= patternLength)
          return Range((size_t)(strCur - str) - skip + 1, patternLength);

        if (skipTable[(strCur - skip)->getValue() & 0xFF] == patternLength)
          skip = patternLength - skip;
        else
          skip = 1;
      }

      if (remain < skip) break;
      strCur += skip;
      remain -= skip;
    }
  }
  else
  {
    for (;;)
    {
      // Get count of characters to skip from skip table.
      if ((skip = skipTable[strCur->toLower().getValue() & 0xFF]) == 0)
      {
        // Check if there is possible match.
        while (skip < patternLength)
        {
          if ((strCur - skip)->toLower() != (patternStr - skip)->toLower() ) break;
          skip++;
        }

        // Match.
        if (skip >= patternLength)
          return Range((size_t)(strCur - str) - skip + 1, patternLength);

        if (skipTable[(strCur - skip)->toLower().getValue() & 0xFF] == patternLength)
          skip = patternLength - skip;
        else
          skip = 1;
      }

      if (remain < skip) break;
      strCur += skip;
      remain -= skip;
    }
  }

  return m;
}

size_t StringMatcher::getLength() const
{
  return _pattern.getLength();
}

} // Fog namespace
