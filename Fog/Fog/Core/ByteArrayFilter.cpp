// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/ByteArrayFilter.h>
#include <Fog/Core/Math.h>

namespace Fog {

// ============================================================================
// [Fog::ByteArrayFilter]
// ============================================================================

ByteArrayFilter::ByteArrayFilter()
{
}

ByteArrayFilter::~ByteArrayFilter()
{
}

Range ByteArrayFilter::indexOf(const char* str, sysuint_t slen, uint cs, const Range& range) const
{
  Range m(INVALID_INDEX, INVALID_INDEX);
  if (range.getStart() >= slen) return m;

  sysuint_t rstart = range.getStart();
  sysuint_t rend = Math::min(slen, range.getEnd());

  if (rstart < rend) m = match(str, slen, cs, Range(rstart, rend));
  return m;
}

Range ByteArrayFilter::lastIndexOf(const char* str, sysuint_t slen, uint cs, const Range& range) const
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

sysuint_t ByteArrayFilter::getLength() const
{
  return INVALID_INDEX;
}

} // Fog namespace
