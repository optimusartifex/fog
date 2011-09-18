// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/C++/Base.h>

// [Guard]
#if !defined(FOG_IDE) && !defined(_FOG_CORE_MEMORY_BSWAP_H)
#error "Fog::Memory::::BSwap::Generic - Only Fog/Core/Memory/BSwap.h can include this file."
#else

namespace Fog {
namespace Memory {

static FOG_INLINE uint16_t bswap16(uint16_t x)
{
  return (x << 8) | (x >> 8) ;
}

static FOG_INLINE uint32_t bswap32(uint32_t x)
{
  return ((x << 24) |
          (x >> 24) |
          ((x << 8) & 0x00FF0000) |
          ((x >> 8) & 0x0000FF00) );
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
