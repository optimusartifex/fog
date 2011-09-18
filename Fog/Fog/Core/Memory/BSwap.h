// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_BSWAP_H
#define _FOG_CORE_MEMORY_BSWAP_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

namespace Fog {
namespace Memory {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::Memory - BSwap]
// ============================================================================

static FOG_INLINE uint16_t bswap16(uint16_t x);
static FOG_INLINE uint32_t bswap32(uint32_t x);
static FOG_INLINE uint64_t bswap64(uint64_t x);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
# define _FOG_BSWAP(_LE_, _BE_) _LE_
#else
# define _FOG_BSWAP(_LE_, _BE_) _BE_
#endif

static FOG_INLINE uint16_t bswap16le(uint16_t x) { return _FOG_BSWAP(x, bswap16(x)); }
static FOG_INLINE uint32_t bswap32le(uint32_t x) { return _FOG_BSWAP(x, bswap32(x)); }
static FOG_INLINE uint64_t bswap64le(uint64_t x) { return _FOG_BSWAP(x, bswap64(x)); }

static FOG_INLINE uint16_t bswap16be(uint16_t x) { return _FOG_BSWAP(bswap16(x), x); }
static FOG_INLINE uint32_t bswap32be(uint32_t x) { return _FOG_BSWAP(bswap32(x), x); }
static FOG_INLINE uint64_t bswap64be(uint64_t x) { return _FOG_BSWAP(bswap64(x), x); }

#undef _FOG_BSWAP

//! @}

} // Memory namespace
} // Fog namespace

// ============================================================================
// [Include platform and compiler dependent implementation]
// ============================================================================

#if defined(FOG_CC_GNU) && (defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64))
# include <Fog/Core/Memory/BSwap_gcc_x86x64.h>
#elif defined(FOG_CC_GNU) && (defined(FOG_ARCH_PPC))
# include <Fog/Core/Memory/BSwap_gcc_ppc.h>
#elif defined(FOG_CC_MSC) && (FOG_CC_MSC < 1400) || defined(FOG_CC_BORLAND)
# include <Fog/Core/Memory/BSwap_msc_x86.h>
#elif defined(FOG_CC_MSC)
# include <Fog/Core/Memory/BSwap_msc_intrin.h>
#else
# include <Fog/Core/Memory/BSwap_generic.h>
#endif

// [Guard]
#endif // _FOG_CORE_MEMORY_BSWAP_H
