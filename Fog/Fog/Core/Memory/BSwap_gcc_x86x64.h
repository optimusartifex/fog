// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Config/Config.h>

// [Guard]
#if !defined(FOG_IDE) && !defined(_FOG_CORE_MEMORY_BSWAP_H)
#error "Fog::Memory::::BSwap::GCC_X86_X64 - Only Fog/Core/Memory/BSwap.h can include this file."
#else

namespace Fog {
namespace Memory {

static FOG_INLINE uint16_t bswap16(uint16_t x)
{
#if defined(FOG_ARCH_X86)
  __asm__("xchgb %b0,%h0" : "=q" (x) :  "0" (x));
  return x;
#else
  __asm__("xchgb %b0,%h0" : "=Q" (x) :  "0" (x));
  return x;
#endif
}

static FOG_INLINE uint32_t bswap32(uint32_t x)
{
#if defined(FOG_ARCH_X86)
  __asm__("bswap %0" : "=r" (x) : "0" (x));
  return x;
#else
  __asm__("bswapl %0" : "=r" (x) : "0" (x));
  return x;
#endif
}

static FOG_INLINE uint64_t bswap64(uint64_t x)
{
#if defined(FOG_ARCH_X86)
  UInt64Union v;
  v.u64 = x;
  __asm__("bswapl %0\n"
          "bswapl %1\n"
          "xchgl %0,%1"
      : "=r" (v.u32Lo), "=r" (v.u32Hi)
      : "0" (v.u32Lo), "1" (v.u32Hi));
  return v.u64;
#else
  __asm__("bswapq %0" : "=r" (x) : "0" (x));
  return x;
#endif
}

} // Memory namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_BSWAP_H
