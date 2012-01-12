// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_BSWAP_H
#define _FOG_CORE_MEMORY_BSWAP_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

#if defined(FOG_CC_MSC)
# pragma intrinsic (_byteswap_ushort)
# pragma intrinsic (_byteswap_ulong)
# pragma intrinsic (_byteswap_uint64)
#endif // FOG_CC_MSC

namespace Fog {
namespace MemOps {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::Memory - BSwap - Implementation]
// ============================================================================

// [Fog::BSwap - GNU Intrinsics]
#if defined(FOG_CC_GNU) && FOG_CC_GNU_VERSION_GE(4, 3, 0)
static FOG_INLINE uint16_t bswap16(uint16_t x) { return (x << 8) || (x >> 8); }
static FOG_INLINE uint32_t bswap32(uint32_t x) { return __builtin_bswap32(x); }
static FOG_INLINE uint64_t bswap64(uint64_t x) { return __builtin_bswap64(x); }
#define _FOG_HAS_BSWAP64

// [Fog::BSwap - GNU X64 Asm]
#elif defined(FOG_CC_GNU) && defined(FOG_ARCH_X86_64)
static FOG_INLINE uint16_t bswap16(uint16_t x) { __asm__("xchgb %b0, %h0" : "=Q"(x) : "0"(x)); return x; }
static FOG_INLINE uint32_t bswap32(uint32_t x) { __asm__("bswapl %0"      : "=r"(x) : "0"(x)); return x; }
static FOG_INLINE uint64_t bswap64(uint64_t x) { __asm__("bswapq %0"      : "=r"(x) : "0"(x)); return x; }
#define _FOG_HAS_BSWAP64

// [Fog::BSwap - GNU X86 Asm]
#elif defined(FOG_CC_GNU) && defined(FOG_ARCH_X86)
static FOG_INLINE uint16_t bswap16(uint16_t x) { __asm__("xchgb %b0, %h0" : "=q"(x) : "0"(x)); return x; }
static FOG_INLINE uint32_t bswap32(uint32_t x) { __asm__("bswapl %0"      : "=r"(x) : "0"(x)); return x; }

// [Fog::BSwap - GNU PowerPC Asm]
#elif defined(FOG_CC_GNU) && (defined(FOG_ARCH_PPC))
static FOG_INLINE uint16_t bswap16(uint16_t x)
{
  uint16_t result;
  __asm__("rlwimi %0, %2,  8, 16, 23" : "=&r" (result) : "0" (x >> 8), "r" (x));
  return result;
}
static FOG_INLINE uint32_t bswap32(uint32_t x)
{
  uint32_t result;
  __asm__("rlwimi %0, %2, 24, 16, 23" : "=&r" (result) : "0" (x >>24), "r" (x));
  __asm__("rlwimi %0, %2,  8,  8, 15" : "=&r" (result) : "0" (result), "r" (x));
  __asm__("rlwimi %0, %2, 24,  0,  7" : "=&r" (result) : "0" (result), "r" (x));
  return result;
}

// [Fog::BSwap - MSC|Borland X86 Asm]
#elif defined(FOG_CC_MSC) && defined(FOG_ARCH_X86) && ((FOG_CC_MSC < 1400) || defined(FOG_CC_BORLAND))
static FOG_INLINE uint16_t bswap16(uint16_t x)
{
  __asm {
    mov ax, x;
    xchg al, ah;
    mov x, ax;
  }
  return x;
}
static FOG_INLINE uint32_t bswap32(uint32_t x)
{
  __asm {
    mov eax, x;
    bswap eax;
    mov x, eax;
  }
  return x;
}

// [Fog::BSwap - MSC Intrinsics]
#elif defined(FOG_CC_MSC)
static FOG_INLINE uint16_t bswap16(uint16_t x) { return _byteswap_ushort(x); }
static FOG_INLINE uint32_t bswap32(uint32_t x) { return _byteswap_ulong (x); }
static FOG_INLINE uint64_t bswap64(uint64_t x) { return _byteswap_uint64(x); }
#define _FOG_HAS_BSWAP64

// [Fog::BSwap - Generic 16|32]
#else
static FOG_INLINE uint16_t bswap16(uint16_t x) { return ((x <<  8) | (x >>  8)); }
static FOG_INLINE uint32_t bswap32(uint32_t x) { return ((x << 24) | (x >> 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00)); }
#endif

// [Fog::BSwap - Generic 64]
#if !defined(_FOG_HAS_BSWAP64)
static FOG_INLINE uint64_t bswap64(uint64_t x)
{
  UInt64Bits u; u.u64 = x;
  return ((uint64_t)bswap32(u.u32Lo) << 32) + ((uint64_t)bswap32(u.u32Hi));
}
#undef _FOG_HAS_BSWAP64
#endif // _FOG_HAS_BSWAP64

// ============================================================================
// [Fog::Memory - BSwap - Helpers]
// ============================================================================

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
# define _FOG_BSWAP(_LE_, _BE_) _LE_
#else
# define _FOG_BSWAP(_LE_, _BE_) _BE_
#endif // FOG_BYTE_ORDER

static FOG_INLINE uint16_t bswap16le(uint16_t x) { return _FOG_BSWAP(x, bswap16(x)); }
static FOG_INLINE uint32_t bswap32le(uint32_t x) { return _FOG_BSWAP(x, bswap32(x)); }
static FOG_INLINE uint64_t bswap64le(uint64_t x) { return _FOG_BSWAP(x, bswap64(x)); }

static FOG_INLINE uint16_t bswap16be(uint16_t x) { return _FOG_BSWAP(bswap16(x), x); }
static FOG_INLINE uint32_t bswap32be(uint32_t x) { return _FOG_BSWAP(bswap32(x), x); }
static FOG_INLINE uint64_t bswap64be(uint64_t x) { return _FOG_BSWAP(bswap64(x), x); }

#undef _FOG_BSWAP

//! @}

} // MemOps namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_BSWAP_H
