// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/StringFilter.h>

namespace Fog {

// ============================================================================
// [Fog::StringFilter]
// ============================================================================

StringFilter::StringFilter()
{
}

StringFilter::~StringFilter()
{
}

Range StringFilter::indexOf(const Char* str, sysuint_t slen, uint cs, const Range& range) const
{
  Range m(INVALID_INDEX, INVALID_INDEX);
  if (range.getStart() >= slen) return m;

  sysuint_t rstart = range.getStart();
  sysuint_t rend = Math::min(slen, range.getEnd());

  if (rstart < rend) m = match(str, slen, cs, Range(rstart, rend));
  return m;
}

Range StringFilter::lastIndexOf(const Char* str, sysuint_t slen, uint cs, const Range& range) const
{
  Range m(INVALID_INDEX, INVALID_INDEX);
  if (range.getStart() >= slen) return m;

  sysuint_t rstart = range.getStart();
  sysuint_t rend = Math::min(slen, range.getEnd());

  for (;;)
  {
    Range t = match(str, slen, cs, Range(rstart, rend));
    if (t.getStart() == INVALID_INDEX) break;

    rstart = t.getEnd();
    m = t;
  }

  return m;
}

sysuint_t StringFilter::getLength() const
{
  return INVALID_INDEX;
}

} // Fog namespace
