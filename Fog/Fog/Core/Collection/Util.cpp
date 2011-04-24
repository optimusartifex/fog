// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Util.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {

// ============================================================================
// [Fog::Util]
// ============================================================================

sysuint_t Util::getGrowCapacity(sysuint_t headerSize, sysuint_t elementSize, sysuint_t before, sysuint_t after)
{
  // Threshold for excessive growing. If size of data in memory is larger
  // than the threshold, grow will be constant instead of excessive.
  const sysuint_t minThreshold = 128;
  const sysuint_t maxThreshold = 1024 * 1024 * 8;

  FOG_ASSERT(before < after);

  sysuint_t beforeSize = headerSize + before * elementSize;
  sysuint_t afterSize = headerSize + after * elementSize;
  sysuint_t optimal;

  if (afterSize < minThreshold)
  {
    optimal = minThreshold;
  }
  else if (beforeSize < maxThreshold && afterSize < maxThreshold)
  {
    optimal = beforeSize;
    while (optimal < afterSize) optimal += ((optimal + 2) >> 1);
  }
  else
  {
    optimal = Math::max(beforeSize + maxThreshold, afterSize);
  }

  optimal = (optimal + 15) & ~15;
  optimal = ((optimal - headerSize) / elementSize);

  FOG_ASSERT(optimal >= after);
  return optimal;
}

} // Fog namespace
