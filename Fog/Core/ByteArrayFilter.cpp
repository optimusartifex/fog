// [Fog-Core Library - Public API]
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

Range ByteArrayFilter::indexOf(const char* str, sysuint_t length, uint cs, const Range& range) const
{
  Range m(INVALID_INDEX, INVALID_INDEX);
  if (range.index >= length) return m;

  sysuint_t i = range.index;
  sysuint_t e = (length - i < range.length) ? length - i : range.index + range.length;

  m = match(str, length, cs, Range(i, e - i));
  return m;
}

Range ByteArrayFilter::lastIndexOf(const char* str, sysuint_t length, uint cs, const Range& range) const
{
  Range m(INVALID_INDEX, INVALID_INDEX);
  if (range.index >= length) return m;

  sysuint_t i = range.index;
  sysuint_t e = (length - i < range.length) ? length - i : range.index + range.length;

  for (;;)
  {
    Range t = match(str, length, cs, Range(i, e - i));
    if (t.index == INVALID_INDEX) break;

    i = t.index + t.length;
    m = t;
  }

  return m;
}

sysuint_t ByteArrayFilter::getLength() const
{
  return INVALID_INDEX;
}

} // Fog namespace
