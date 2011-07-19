// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_C_ASM_H
#define _FOG_CORE_FACE_FACE_C_ASM_H

// [Dependencies]
#include <Fog/Core/Face/Constants.h>
#include <Fog/Core/Face/Features.h>
#include <Fog/Core/Face/Face_C_Types.h>
#include <Fog/Core/Global/Global.h>

#if defined(FOG_CC_MSC)
# include <stdlib.h>
# include <intrin.h>
# pragma intrinsic(_BitScanReverse)
# pragma intrinsic(_BitScanForward)
# pragma intrinsic(_rotl)
# pragma intrinsic(_rotr)
# pragma intrinsic(__emulu)
#endif // FOG_CC_MSC

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_C
//! @{

// ============================================================================
// [Fog::Face - P32 - MSC
// ============================================================================

#if defined(FOG_CC_MSC)
#if defined(FOG_ARCH_X86   ) || \
    defined(FOG_ARCH_X86_64)

#define FOG_FACE_HAS_FAST_CTZ
#define FOG_FACE_HAS_FAST_CLZ
#define FOG_FACE_HAS_FAST_ROTATE
#define FOG_FACE_HAS_FAST_EMUL

static FOG_INLINE bool p32CLZ(p32& dst, const p32& src)
{
  bool result;
  DWORD dstTmp;

  result = _BitScanReverse(&dstTmp, src);

  dst = 31 - dstTmp;
  return result;
}

static FOG_INLINE bool p32CTZ(p32& dst, const p32& src)
{
  bool result;
  DWORD dstTmp;

  result = _BitScanForward(&dstTmp, src);

  dst = dstTmp;
  return result;
}

template<int N>
static FOG_INLINE void p32LRotate(p32& dst, const p32& src)
{
  if ((N % 32) == 0)
    dst = src;
  else
    dst = _rotl(src, N % 32);
}

template<int N>
static FOG_INLINE void p32RRotate(p32& dst, const p32& src)
{
  if ((N % 32) == 0)
    dst = src;
  else
    dst = _rotr(src, N % 32);
}

static FOG_INLINE uint64_t u64EMul2x32(uint32_t x, uint32_t y)
{
  return (uint64_t)__emulu(x, y);
}

#endif // FOG_ARCH_X86 || FOG_ARCH_X86_64
#endif // FOG_CC_MSC

// ============================================================================
// [Fog::Face - P32 - GNU
// ============================================================================

#if defined(FOG_CC_GNU)
#if defined(FOG_ARCH_X86   ) || \
    defined(FOG_ARCH_X86_64) || \
    defined(FOG_ARCH_ARM   )

#define FOG_FACE_HAS_FAST_CTZ
#define FOG_FACE_HAS_FAST_CLZ

static FOG_INLINE bool p32CLZ(p32& dst, const p32& src)
{
  bool result = (src != 0);
  dst = __builtin_clz(src);
  return result;
}

static FOG_INLINE bool p32CTZ(p32& dst, const p32& src)
{
  bool result = (src != 0);
  dst = __builtin_ctz(src);
  return result;
}

// GCC can recognize bit hacks and can generate ROL/ROR instructions for X86,
// AMD64, and ARM (and maybe other architectures too).
#define FOG_FACE_SET_FAST_ROTATE

#endif // FOG_ARCH_X86 || FOG_ARCH_X86_64 || FOG_ARCH_ARM
#endif // FOG_CC_GNU

// ============================================================================
// [Fog::Face - P32 - Generic
// ============================================================================

#if !defined(FOG_FACE_HAS_FAST_POPCNT)
// From Hackers Delight 'http://www.hackersdelight.org'.
static FOG_INLINE p32 p32PopCnt(p32 x)
{
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  x = (x + (x >> 4)) & 0x0F0F0F0F;
  x = x + (x << 8);
  x = x + (x << 16);
  return x >> 24;
}
#endif // !FOG_FACE_HAS_FAST_POPCNT

// CLZ/CTZ.
#if !defined(FOG_FACE_HAS_FAST_CLZ)
static FOG_INLINE bool p32CLZ(p32& dst, const p32& src)
{
  bool result = src != 0;

  dst = src;
  dst |= dst >> 1;
  dst |= dst >> 2;
  dst |= dst >> 4;
  dst |= dst >> 8;
  dst |= dst >>16;
  dst = p32PopCnt(~dst);

  return result;
}
#endif // !FOG_FACE_HAS_FAST_CLZ

#if !defined(FOG_FACE_HAS_FAST_CTZ)
static FOG_INLINE bool p32CTZ(p32& dst, const p32& src)
{
  bool result = src != 0;
  dst = p32PopCnt(~src & (src - 1));
  return result;
}
#endif // !FOG_FACE_HAS_FAST_CTZ

// LRotate/RRotate.
#if !defined(FOG_FACE_HAS_FAST_ROTATE)

#if defined(FOG_FACE_SET_FAST_ROTATE)
# define FOG_FACE_HAS_FAST_ROTATE
#endif // FOG_FACE_SET_FAST_ROTATE

template<int N>
static FOG_INLINE void p32LRotate(p32& dst, const p32& src)
{
  if ((N % 32) == 0)
    dst = src;
  else
    dst = (src << N) | (src >> (32 - N));
}

template<int N>
static FOG_INLINE void p32RRotate(p32& dst, const p32& src)
{
  if ((N % 32) == 0)
    dst = src;
  else
    dst = (src >> N) | (src << (32 - N));
}

#endif // !FOG_FACE_HAS_FAST_ROTATE

#if !defined(FOG_FACE_HAS_FAST_EMUL)
static FOG_INLINE uint64_t u64EMul2x32(uint32_t x, uint32_t y)
{
  return (uint64_t)x * y;
}
#endif // !FOG_FACE_HAS_EMUL

static FOG_INLINE void p64EMul2x32(p64& dst, const p32& x, const p32& y)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  // p64 is type only when CPU is 64-bit. In this case there is no sense
  // to use EMUL (64-bit multiplication can't overflow in our case).
  dst = (uint64_t)x * y;
#else
  dst.u64 = u64EMul2x32(x, y);
#endif
}

static FOG_INLINE void p32FloorPow2(p32& dst, const p32& src)
{
  dst = src;
  dst |= dst >> 1;
  dst |= dst >> 2;
  dst |= dst >> 4;
  dst |= dst >> 8;
  dst |= dst >> 16;
  dst = dst - (dst >> 1);
}

static FOG_INLINE void p32CeilPow2(p32& dst, const p32& src)
{
  dst = src - 1;
  dst |= dst >> 1;
  dst |= dst >> 2;
  dst |= dst >> 4;
  dst |= dst >> 8;
  dst |= dst >> 16;
  dst = dst + 1;
}

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_C_ASM_H
