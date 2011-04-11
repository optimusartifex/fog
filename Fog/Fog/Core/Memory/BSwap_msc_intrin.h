// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Config/Config.h>

// [Guard]
#if !defined(FOG_IDE) && !defined(_FOG_CORE_MEMORY_BSWAP_H)
#error "Fog::Memory::::BSwap::MSC_Intrin - Only Fog/Core/Memory/BSwap.h can include this file."
#else

#pragma intrinsic (_byteswap_ushort)
#pragma intrinsic (_byteswap_ulong)
#pragma intrinsic (_byteswap_uint64)

namespace Fog {
namespace Memory {

static FOG_INLINE uint16_t bswap16(uint16_t x)
{
  return _byteswap_ushort(x);
}

static FOG_INLINE uint32_t bswap32(uint32_t x)
{
  return _byteswap_ulong(x);
}

static FOG_INLINE uint64_t bswap64(uint64_t x)
{
  return _byteswap_uint64(x);
}

} // Memory namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_BSWAP_H
