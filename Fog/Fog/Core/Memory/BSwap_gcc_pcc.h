// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/C++/Base.h>

// [Guard]
#if !defined(FOG_IDE) && !defined(_FOG_CORE_MEMORY_BSWAP_H)
#error "Fog::Memory::::BSwap::GCC_PCC - Only Fog/Core/Memory/BSwap.h can include this file."
#else

namespace Fog {
namespace Memory {

static FOG_INLINE uint16_t bswap16(uint16_t x)
{
  uint16_t result;
  __asm__("rlwimi %0,%2,8,16,23" : "=&r" (result) : "0" (x >> 8), "r" (x));
  return result;
}

static FOG_INLINE uint32_t bswap32(uint32_t x)
{
  uint32_t result;

  __asm__("rlwimi %0,%2,24,16,23" : "=&r" (result) : "0" (x>>24), "r" (x));
  __asm__("rlwimi %0,%2,8,8,15"   : "=&r" (result) : "0" (result), "r" (x));
  __asm__("rlwimi %0,%2,24,0,7"   : "=&r" (result) : "0" (result), "r" (x));

  return result;
}

static FOG_INLINE uint64_t bswap64(uint64_t x)
{
  uint32_t lo = (uint32_t)(x & 0xFFFFFFFF); x >>= 32;
  uint32_t hi = (uint32_t)(x);
  x  = bswap32(lo); x <<= 32;
  x |= bswap32(hi);
  return x;
}

} // Memory namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_BSWAP_H
