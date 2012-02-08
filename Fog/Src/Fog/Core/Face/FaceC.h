// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACEC_H
#define _FOG_CORE_FACE_FACEC_H

// [Dependencies]
#include <Fog/Core/Face/Constants.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/BSwap.h>

// [Dependencies - MSC]
#if defined(FOG_CC_MSC)
# include <stdlib.h>
# include <intrin.h>
# pragma intrinsic(_BitScanReverse)
# pragma intrinsic(_BitScanForward)
# pragma intrinsic(_rotl)
# pragma intrinsic(_rotr)
# pragma intrinsic(__emulu)
#endif // FOG_CC_MSC

//! @defgroup Fog_Face_C Packed 32-bit and 64-bit datatype manipulation in C++.
//! @ingroup Fog_Face
//!
//! Collection of typedefs and functions to work with scalar bytes or bytes
//! packed in 32-bit or 64-bit unsigned integers.
//!
//! Functions defined by @ref Fog_Face_C do operation on:
//!
//!   1. scalar byte:
//!      - suffix "SBB"
//!   2. packed bytes (0xB3B2B1B0):
//!      - suffix "PBB"
//!   3. scalar byte as word:
//!      - suffix "SBW"
//!   4. packed bytes as words (0x__B0__B0, etc...):
//!      - suffix "PBW"
//!
//! Data format for working with pixels in 32-bit mode:
//!
//!   - PBB b3210 [0xAARRGGBB]
//!
//!   - PBW b_2_0 [0x00RR00BB]
//!   - PBW b_3_1 [0x00AA00GG]
//!
//! Data format for working with pixels in 64-bit mode:
//!
//!   - PBB b3210 [0xAARRGGBB]
//!
//!   - PBW b_3_2_1_0 [0x00AA00GG00RR00BB]
//!
//! The reason to not unpack bytes to 0x00AA00RR/00GG00BB is that it's more
//! expensive, unpacking even and odd bytes separately is simpler and working
//! with the data is the same. Notice that the position of bytes/pixels is
//! different to position when using MMX/SSE2 where unpacking to AARRGGBB is
//! done using single instruction.

// ============================================================================
// [_FOG_FACE_U64...]
// ============================================================================

#if defined(FOG_ARCH_NATIVE_P64)
# define _FOG_FACE_U64(_Val_) _Val_
# define _FOG_FACE_U64_LO(_Val_) ((uint32_t) ((_Val_)      ))
# define _FOG_FACE_U64_HI(_Val_) ((uint32_t) ((_Val_) >> 32))
# define _FOG_FACE_U64_SET(_Val_, _Lo_, _Hi_) \
  FOG_MACRO_BEGIN \
    _Val_ = ( (uint64_t)(_Lo_) | ((uint64_t)(_Hi_) << 32) ) \
  FOG_MACRO_END
#else
# define _FOG_FACE_U64(_Val_) (_Val_.u64)
# define _FOG_FACE_U64_LO(_Val_) (_Val_.u32Lo)
# define _FOG_FACE_U64_HI(_Val_) (_Val_.u32Hi)
# define _FOG_FACE_U64_SET(_Val_, _Lo_, _Hi_) \
  FOG_MACRO_BEGIN \
    _Val_.u32Lo = _Lo_; \
    _Val_.u32Hi = _Hi_; \
  FOG_MACRO_END
#endif // FOG_ARCH_NATIVE_P64

#define _U64 _FOG_FACE_U64

// ============================================================================
// [_FOG_FACE_COMBINE_...]
// ============================================================================

#if defined(FOG_ARCH_HAS_FAST_ADD)
# define _FOG_FACE_COMBINE_2(_X0_, _X1_) ((_X0_) + (_X1_))
# define _FOG_FACE_COMBINE_3(_X0_, _X1_, _X2_) ((_X0_) + (_X1_) + (_X2_))
# define _FOG_FACE_COMBINE_4(_X0_, _X1_, _X2_, _X3_) ((_X0_) + (_X1_) + (_X2_) + (_X3_))
# define _FOG_FACE_COMBINE_5(_X0_, _X1_, _X2_, _X3_, _X4_) ((_X0_) + (_X1_) + (_X2_) + (_X3_) + (_X4_))
#else
# define _FOG_FACE_COMBINE_2(_X0_, _X1_) ((_X0_) | (_X1_))
# define _FOG_FACE_COMBINE_3(_X0_, _X1_, _X2_) ((_X0_) | (_X1_) | (_X2_))
# define _FOG_FACE_COMBINE_4(_X0_, _X1_, _X2_, _X3_) ((_X0_) | (_X1_) | (_X2_) | (_X3_))
# define _FOG_FACE_COMBINE_5(_X0_, _X1_, _X2_, _X3_, _X4_) ((_X0_) | (_X1_) | (_X2_) | (_X3_) | (_X4_))
#endif // FOG_ARCH_HAS_FAST_ADD

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_C
//! @{

// ============================================================================
// [Fog::Face - P32 - MSC]
// ============================================================================

#if defined(FOG_CC_MSC) && (defined(FOG_ARCH_X86   ) || \
                            defined(FOG_ARCH_X86_64))

#define FOG_FACE_HAS_FAST_CTZ
#define FOG_FACE_HAS_FAST_CLZ
#define FOG_FACE_HAS_FAST_ROTATE
#define FOG_FACE_HAS_FAST_EMUL

static FOG_INLINE bool p32CLZ(uint32_t& dst, const uint32_t& src)
{
  bool result;
  DWORD dstTmp;

  result = _BitScanReverse(&dstTmp, src);

  dst = 31 - dstTmp;
  return result;
}

static FOG_INLINE bool p32CTZ(uint32_t& dst, const uint32_t& src)
{
  bool result;
  DWORD dstTmp;

  result = _BitScanForward(&dstTmp, src);

  dst = dstTmp;
  return result;
}

template<int N>
static FOG_INLINE void p32LRotate(uint32_t& dst, const uint32_t& src)
{
  if ((N % 32) == 0)
    dst = src;
  else
    dst = _rotl(src, N % 32);
}

template<int N>
static FOG_INLINE void p32RRotate(uint32_t& dst, const uint32_t& src)
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
#endif // FOG_CC_MSC && (FOG_ARCH_X86 || FOG_ARCH_X86_64)

// ============================================================================
// [Fog::Face - P32 - GNU]
// ============================================================================

#if defined(FOG_CC_GNU) && (defined(FOG_ARCH_X86   ) || \
                            defined(FOG_ARCH_X86_64) || \
                            defined(FOG_ARCH_ARM   ))

#define FOG_FACE_HAS_FAST_CTZ
#define FOG_FACE_HAS_FAST_CLZ

static FOG_INLINE bool p32CLZ(uint32_t& dst, const uint32_t& src)
{
  bool result = (src != 0);
  dst = __builtin_clz(src);
  return result;
}

static FOG_INLINE bool p32CTZ(uint32_t& dst, const uint32_t& src)
{
  bool result = (src != 0);
  dst = __builtin_ctz(src);
  return result;
}

// GCC can recognize bit hacks and can generate ROL/ROR instructions for X86,
// AMD64, and ARM (and maybe other architectures too).
#define FOG_FACE_SET_FAST_ROTATE

#endif // FOG_CC_GNU && (FOG_ARCH_X86 || FOG_ARCH_X86_64 || FOG_ARCH_ARM)

// ============================================================================
// [Fog::Face - P32 - Generic]
// ============================================================================

#if !defined(FOG_FACE_HAS_FAST_POPCNT)
// From Hackers Delight 'http://www.hackersdelight.org'.
static FOG_INLINE uint32_t p32PopCnt(uint32_t x)
{
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  x = (x + (x >> 4)) & 0x0F0F0F0F;
  x = x + (x << 8);
  x = x + (x << 16);
  return x >> 24;
}
#endif // !FOG_FACE_HAS_FAST_POPCNT

// CLZ / CTZ.
#if !defined(FOG_FACE_HAS_FAST_CLZ)
static FOG_INLINE bool p32CLZ(uint32_t& dst, const uint32_t& src)
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
static FOG_INLINE bool p32CTZ(uint32_t& dst, const uint32_t& src)
{
  bool result = src != 0;
  dst = p32PopCnt(~src & (src - 1));
  return result;
}
#endif // !FOG_FACE_HAS_FAST_CTZ

// LRotate / RRotate.
#if !defined(FOG_FACE_HAS_FAST_ROTATE)

#if defined(FOG_FACE_SET_FAST_ROTATE)
# define FOG_FACE_HAS_FAST_ROTATE
#endif // FOG_FACE_SET_FAST_ROTATE

template<int N>
static FOG_INLINE void p32LRotate(uint32_t& dst, const uint32_t& src)
{
  if ((N % 32) == 0)
    dst = src;
  else
    dst = (src << N) | (src >> (32 - N));
}

template<int N>
static FOG_INLINE void p32RRotate(uint32_t& dst, const uint32_t& src)
{
  if ((N % 32) == 0)
    dst = src;
  else
    dst = (src >> N) | (src << (32 - N));
}

#endif // !FOG_FACE_HAS_FAST_ROTATE

// EMul2x32.
#if !defined(FOG_FACE_HAS_FAST_EMUL)
static FOG_INLINE uint64_t u64EMul2x32(uint32_t x, uint32_t y)
{
  return (uint64_t)x * y;
}
#endif // !FOG_FACE_HAS_EMUL

// ============================================================================
// [Fog::Face - P16 - BSwap]
// ============================================================================

static FOG_INLINE void p16BSwap16(uint16_t& dst0, const uint16_t& src0)
{
  dst0 = MemOps::bswap16(src0);
}

// ============================================================================
// [Fog::Face - P32 - BSwap]
// ============================================================================

static FOG_INLINE void p32BSwap16(uint32_t& dst0, const uint32_t& src0)
{
  dst0 = MemOps::bswap16(static_cast<uint16_t>(src0));
}

static FOG_INLINE void p32BSwap32(uint32_t& dst0, const uint32_t& src0)
{
  dst0 = MemOps::bswap32(src0);
}

// ============================================================================
// [Fog::Face - P8 - Load]
// ============================================================================

static FOG_INLINE void p8Load1b(uint8_t& dst0, const void* srcp)
{
  dst0 = static_cast<const uint8_t*>(srcp)[0];
}

// ============================================================================
// [Fog::Face - P8 - Store]
// ============================================================================

static FOG_INLINE void p8Store1b(void* dstp, const uint8_t& src0)
{
  static_cast<uint8_t*>(dstp)[0] = src0;
}

// ============================================================================
// [Fog::Face - P16 - Load]
// ============================================================================

static FOG_INLINE void p16Load1b(uint16_t& dst0, const void* srcp)
{
  dst0 = static_cast<const uint8_t*>(srcp)[0];
}

static FOG_INLINE void p16Load2a(uint16_t& dst0, const void* srcp)
{
  FOG_ASSERT_ALIGNED(srcp, 2);
  dst0 = static_cast<const uint16_t*>(srcp)[0];
}

static FOG_INLINE void p16Load2aBSwap(uint16_t& dst0, const void* srcp)
{
  p16Load2a(dst0, srcp);
  p16BSwap16(dst0, dst0);
}

static FOG_INLINE void p16Load2u(uint16_t& dst0, const void* srcp)
{
#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  dst0 = ((const uint16_t*)srcp)[0];
#elif defined(FOG_PACKED_TYPE)
  dst0 = static_cast<FOG_PACKED_TYPE(const uint16_t)*>(srcp)[0];
#elif FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_2(static_cast<uint16_t>(static_cast<const uint8_t*>(srcp)[0])     ,
                             static_cast<uint16_t>(static_cast<const uint8_t*>(srcp)[1]) << 8);
#else
  dst0 = _FOG_FACE_COMBINE_2(static_cast<uint16_t>(static_cast<const uint8_t*>(srcp)[0]) << 8,
                             static_cast<uint16_t>(static_cast<const uint8_t*>(srcp)[1])     );
#endif // FOG_ARCH_UNALIGNED_ACCESS_16
}

static FOG_INLINE void p16Load2uBSwap(uint16_t& dst0, const void* srcp)
{
#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  dst0 = ((const uint16_t*)srcp)[0];
  p16BSwap16(dst0, dst0);
#elif defined(FOG_PACKED_TYPE)
  dst0 = static_cast<FOG_PACKED_TYPE(const uint16_t)*>(srcp)[0];
  p16BSwap16(dst0, dst0);
#elif FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_2(static_cast<uint16_t>(static_cast<const uint8_t*>(srcp)[0]) << 8,
                             static_cast<uint16_t>(static_cast<const uint8_t*>(srcp)[1])     );
#else
  dst0 = _FOG_FACE_COMBINE_2(static_cast<uint16_t>(static_cast<const uint8_t*>(srcp)[0])     ,
                             static_cast<uint16_t>(static_cast<const uint8_t*>(srcp)[1]) << 8);
#endif // FOG_ARCH_UNALIGNED_ACCESS_16
}

// ============================================================================
// [Fog::Face - P16 - Store]
// ============================================================================

static FOG_INLINE void p16Store1b(void* dstp, const uint16_t& src0)
{
  ((uint8_t *)dstp)[0] = (uint8_t )(src0);
}

static FOG_INLINE void p16Store2a(void* dstp, const uint16_t& src0)
{
  FOG_ASSERT_ALIGNED(dstp, 2);
  static_cast<uint16_t*>(dstp)[0] = (uint16_t)(src0);
}

static FOG_INLINE void p16Store2aBSwap(void* dstp, const uint16_t& src0)
{
  uint16_t src0_bs;

  p16BSwap16(src0_bs, src0);
  p16Store2a(dstp, src0_bs);
}

static FOG_INLINE void p16Store2u(void* dstp, const uint16_t& src0)
{
#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  static_cast<uint16_t*>(dstp)[0] = src0;
#elif defined(FOG_PACKED_TYPE)
  static_cast<FOG_PACKED_TYPE(uint16_t)*>(dstp)[0] = src0;
#elif FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  static_cast<uint_8t*>(dstp)[0] = static_cast<uint8_t>(src0      );
  static_cast<uint_8t*>(dstp)[1] = static_cast<uint8_t>(src0 >>  8);
#else
  static_cast<uint_8t*>(dstp)[0] = static_cast<uint8_t>(src0 >>  8);
  static_cast<uint_8t*>(dstp)[1] = static_cast<uint8_t>(src0      );
#endif // FOG_ARCH_UNALIGNED_ACCESS_16
}

static FOG_INLINE void p16Store2uBSwap(void* dstp, const uint16_t& src0)
{
#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  uint16_t src0_bs;
  p16BSwap16(src0_bs, src0);
  static_cast<uint16_t*>(dstp)[0] = src0_bs;
#elif defined(FOG_PACKED_TYPE)
  uint16_t src0_bs;
  p16BSwap16(src0_bs, src0);
  static_cast<FOG_PACKED_TYPE(uint16_t)*>(dstp)[0] = src0_bs;
#elif FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  static_cast<uint_8t*>(dstp)[0] = static_cast<uint8_t>(src0  >> 8);
  static_cast<uint_8t*>(dstp)[1] = static_cast<uint8_t>(src0      );
#else
  static_cast<uint_8t*>(dstp)[0] = static_cast<uint8_t>(src0      );
  static_cast<uint_8t*>(dstp)[1] = static_cast<uint8_t>(src0  >> 8);
#endif // FOG_ARCH_UNALIGNED_ACCESS_16
}

// ============================================================================
// [Fog::Face - P32 - Load]
// ============================================================================

static FOG_INLINE void p32Load1b(uint32_t& dst0, const void* srcp)
{
  dst0 = ((const uint8_t *)srcp)[0];
}

static FOG_INLINE void p32Load2a(uint32_t& dst0, const void* srcp)
{
  FOG_ASSERT_ALIGNED(srcp, 2);
  dst0 = ((const uint16_t*)srcp)[0];
}

static FOG_INLINE void p32Load2aBSwap(uint32_t& dst0, const void* srcp)
{
  p32Load2a(dst0, srcp);
  p32BSwap16(dst0, dst0);
}

static FOG_INLINE void p32Load2u(uint32_t& dst0, const void* srcp)
{
#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  dst0 = ((const uint16_t*)srcp)[0];
#elif defined(FOG_PACKED_TYPE)
  dst0 = static_cast<FOG_PACKED_TYPE(const uint16_t)*>(srcp)[0];
#elif FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_2(static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[0])     ,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[1]) << 8);
#else
  dst0 = _FOG_FACE_COMBINE_2(static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[0]) << 8,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[1])     );
#endif // FOG_ARCH_UNALIGNED_ACCESS_16
}

static FOG_INLINE void p32Load2uBSwap(uint32_t& dst0, const void* srcp)
{
#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  dst0 = ((const uint16_t*)srcp)[0];
  p32BSwap16(dst0, dst0);
#elif defined(FOG_PACKED_TYPE)
  dst0 = static_cast<FOG_PACKED_TYPE(const uint16_t)*>(srcp)[0];
  p32BSwap16(dst0, dst0);
#elif FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_2(static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[0]) << 8,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[1])     );
#else
  dst0 = _FOG_FACE_COMBINE_2(static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[0])     ,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[1]) << 8);
#endif // FOG_ARCH_UNALIGNED_ACCESS_16
}

static FOG_INLINE void p32Load3b(uint32_t& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
# if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_2( (uint32_t)((const uint16_t*)(src8 + 0))[0],
                              (uint32_t)((const uint8_t *)(src8 + 2))[0] << 16);
# else
  dst0 = _FOG_FACE_COMBINE_2( (uint32_t)((const uint8_t *)(src8 + 0))[0] << 16),
                              (uint32_t)((const uint16_t*)(src8 + 1))[0]);
# endif // FOG_BYTE_ORDER
#else
# if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_3( (uint32_t)((const uint8_t *)(src8 + 0))[0] << 16,
                              (uint32_t)((const uint8_t *)(src8 + 1))[0] <<  8,
                              (uint32_t)((const uint8_t *)(src8 + 2))[0]      );
# else
  dst0 = _FOG_FACE_COMBINE_3( (uint32_t)((const uint8_t *)(src8 + 0))[0]      ,
                              (uint32_t)((const uint8_t *)(src8 + 1))[0] <<  8,
                              (uint32_t)((const uint8_t *)(src8 + 2))[0] << 16);
# endif // FOG_BYTE_ORDER
#endif
}

static FOG_INLINE void p32Load3bBSwap(uint32_t& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_3( (uint32_t)((const uint8_t *)(src8 + 0))[0]      ,
                              (uint32_t)((const uint8_t *)(src8 + 1))[0] <<  8,
                              (uint32_t)((const uint8_t *)(src8 + 2))[0] << 16);
#else
  dst0 = _FOG_FACE_COMBINE_3( (uint32_t)((const uint8_t *)(src8 + 0))[0] << 16,
                              (uint32_t)((const uint8_t *)(src8 + 1))[0] <<  8,
                              (uint32_t)((const uint8_t *)(src8 + 2))[0]      );
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Load4a(uint32_t& dst0, const void* srcp)
{
  FOG_ASSERT_ALIGNED(srcp, 4);
  dst0 = ((const uint32_t*)srcp)[0];
}

static FOG_INLINE void p32Load4aBSwap(uint32_t& dst0, const void* srcp)
{
  p32Load4a(dst0, srcp);
  p32BSwap32(dst0, dst0);
}

static FOG_INLINE void p32Load4u(uint32_t& dst0, const void* srcp)
{
#if defined(FOG_ARCH_UNALIGNED_ACCESS_32)
  dst0 = ((const uint32_t*)srcp)[0];
#elif defined(FOG_PACKED_TYPE)
  dst0 = static_cast<FOG_PACKED_TYPE(const uint32_t)*>(srcp)[0];
#elif FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_4(static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[0])      ,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[1]) <<  8,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[2]) << 16,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[3]) << 24);
#else
  dst0 = _FOG_FACE_COMBINE_4(static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[0]) << 24,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[1]) << 16,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[2]) <<  8,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[3])      );
#endif // FOG_ARCH_UNALIGNED_ACCESS_16
}

static FOG_INLINE void p32Load4uBSwap(uint32_t& dst0, const void* srcp)
{
#if defined(FOG_ARCH_UNALIGNED_ACCESS_32)
  dst0 = static_cast<const uint32_t*>(srcp)[0];
  p32BSwap32(dst0, dst0);
#elif defined(FOG_PACKED_TYPE)
  dst0 = static_cast<FOG_PACKED_TYPE(const uint32_t)*>(srcp)[0];
  p32BSwap32(dst0, dst0);
#elif FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  dst0 = _FOG_FACE_COMBINE_4(static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[0]) << 24,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[1]) << 16,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[2]) <<  8,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[3])      );
#else
  dst0 = _FOG_FACE_COMBINE_4(static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[0])      ,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[1]) <<  8,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[2]) << 16,
                             static_cast<uint32_t>(static_cast<const uint8_t*>(srcp)[3]) << 24);
#endif // FOG_ARCH_UNALIGNED_ACCESS_16
}

// [00 11 22 33]
// [LO LO HI HI]

// [00 11 22 33]
// [LO LO HI HI]

static FOG_INLINE void p32Load8a(uint32_t& dst0Lo, uint32_t& dst0Hi, const void* srcp)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [7 6 5 4|3 2 1 0]
  dst0Lo = ((const uint32_t*)srcp)[0];
  dst0Hi = ((const uint32_t*)srcp)[1];
#else
  // [0 1 2 3|4 5 6 7]
  dst0Hi = ((const uint32_t*)srcp)[0];
  dst0Lo = ((const uint32_t*)srcp)[1];
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Load8u(uint32_t& dst0Lo, uint32_t& dst0Hi, const void* srcp)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [7 6 5 4|3 2 1 0]
  dst0Lo = ((const uint32_t*)srcp)[0];
  dst0Hi = ((const uint32_t*)srcp)[1];
#else
  // [0 1 2 3|4 5 6 7]
  dst0Hi = ((const uint32_t*)srcp)[0];
  dst0Lo = ((const uint32_t*)srcp)[1];
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Load8aRaw(uint32_t& dst0Lo, uint32_t& dst0Hi, const void* srcp)
{
  dst0Lo = ((const uint32_t*)srcp)[0];
  dst0Hi = ((const uint32_t*)srcp)[1];
}

static FOG_INLINE void p32Load8uRaw(uint32_t& dst0Lo, uint32_t& dst0Hi, const void* srcp)
{
  dst0Lo = ((const uint32_t*)srcp)[0];
  dst0Hi = ((const uint32_t*)srcp)[1];
}

// ============================================================================
// [Fog::Face - P32 - Store]
// ============================================================================

static FOG_INLINE void p32Store1b(void* dstp, const uint32_t& src0)
{
  ((uint8_t *)dstp)[0] = (uint8_t )(src0);
}

static FOG_INLINE void p32Store2a(void* dstp, const uint32_t& src0)
{
  FOG_ASSERT_ALIGNED(dstp, 2);
  ((uint16_t*)dstp)[0] = (uint16_t)(src0);
}

static FOG_INLINE void p32Store2u(void* dstp, const uint32_t& src0)
{
  ((uint16_t*)dstp)[0] = (uint16_t)(src0);
}

static FOG_INLINE void p32Store2aBSwap(void* dstp, const uint32_t& src0)
{
  ((uint16_t*)dstp)[0] = MemOps::bswap16( (uint16_t)(src0) );
}

static FOG_INLINE void p32Store2uBSwap(void* dstp, const uint32_t& src0)
{
  ((uint16_t*)dstp)[0] = MemOps::bswap16( (uint16_t)(src0) );
}

static FOG_INLINE void p32Store3b(void* dstp, const uint32_t& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  ((uint16_t*)(dst8 + 0))[0] = (uint16_t)(src0      );
  ((uint8_t *)(dst8 + 2))[0] = (uint8_t )(src0 >> 16);
#else
  ((uint8_t *)(dst8 + 0))[0] = (uint8_t )(src0 >> 16);
  ((uint16_t*)(dst8 + 1))[0] = (uint16_t)(src0      );
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Store3bBSwap(void* dstp, const uint32_t& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  ((uint8_t *)(dst8 + 0))[0] = (uint8_t)(src0      );
  ((uint8_t *)(dst8 + 1))[0] = (uint8_t)(src0 >>  8);
  ((uint8_t *)(dst8 + 2))[0] = (uint8_t)(src0 >> 16);
#else
  ((uint8_t *)(dst8 + 0))[0] = (uint8_t)(src0 >> 16);
  ((uint8_t *)(dst8 + 1))[0] = (uint8_t)(src0 >>  8);
  ((uint8_t *)(dst8 + 2))[0] = (uint8_t)(src0      );
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Store4a(void* dstp, const uint32_t& src0) { ((uint32_t*)dstp)[0] = (uint32_t)(src0); }
static FOG_INLINE void p32Store4u(void* dstp, const uint32_t& src0) { ((uint32_t*)dstp)[0] = (uint32_t)(src0); }

static FOG_INLINE void p32Store4aBSwap(void* dstp, const uint32_t& src0) { ((uint32_t*)dstp)[0] = MemOps::bswap32( (uint32_t)(src0) ); }
static FOG_INLINE void p32Store4uBSwap(void* dstp, const uint32_t& src0) { ((uint32_t*)dstp)[0] = MemOps::bswap32( (uint32_t)(src0) ); }

static FOG_INLINE void p32Store8a(void* dstp, const uint32_t& src0Lo, const uint32_t& src0Hi)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [7 6 5 4|3 2 1 0]
  ((uint32_t*)dstp)[0] = src0Lo;
  ((uint32_t*)dstp)[1] = src0Hi;
#else
  // [0 1 2 3|4 5 6 7]
  ((uint32_t*)dstp)[0] = src0Hi;
  ((uint32_t*)dstp)[1] = src0Lo;
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Store8u(void* dstp, const uint32_t& src0Lo, const uint32_t& src0Hi)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  // [7 6 5 4|3 2 1 0]
  ((uint32_t*)dstp)[0] = src0Lo;
  ((uint32_t*)dstp)[1] = src0Hi;
#else
  // [0 1 2 3|4 5 6 7]
  ((uint32_t*)dstp)[0] = src0Hi;
  ((uint32_t*)dstp)[1] = src0Lo;
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void p32Store8aRaw(void* dstp, const uint32_t& src0Lo, const uint32_t& src0Hi)
{
  ((uint32_t*)dstp)[0] = src0Lo;
  ((uint32_t*)dstp)[1] = src0Hi;
}

static FOG_INLINE void p32Store8uRaw(void* dstp, const uint32_t& src0Lo, const uint32_t& src0Hi)
{
  ((uint32_t*)dstp)[0] = src0Lo;
  ((uint32_t*)dstp)[1] = src0Hi;
}

// ============================================================================
// [Fog::Face - P32 - Zero]
// ============================================================================

//! @brief Clear the @a dst0.
//!
//! @verbatim
//! dst0.u32[0] = 0
//! @endverbatim
static FOG_INLINE void p32Zero(uint32_t& dst0)
{
  dst0 = 0;
}

//! @brief Clear the @a dst0/dst1.
//!
//! @verbatim
//! dst0.u32[0] = 0
//! dst1.u32[0] = 0
//! @endverbatim
static FOG_INLINE void p32Zero_2x(uint32_t& dst0, uint32_t& dst1)
{
  dst0 = 0;
  dst1 = 0;
}

// ============================================================================
// [Fog::Face - P32 - Copy]
// ============================================================================

//! @brief Copy @a x0 to @a dst0.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Copy(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0;
}

//! @brief Copy @a x0/x1 to @a dst0/dst1.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0]
//! dst1.u32[0] = x1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Copy_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  dst0 = x0;
  dst1 = x1;
}

// ============================================================================
// [Fog::Face - P32 - Extract]
// ============================================================================

static FOG_INLINE void p32ExtractPBB0(uint32_t& dst0, const uint32_t& x0) { dst0 = (x0      ) & 0xFFU; }
static FOG_INLINE void p32ExtractPBB1(uint32_t& dst0, const uint32_t& x0) { dst0 = (x0 >>  8) & 0xFFU; }
static FOG_INLINE void p32ExtractPBB2(uint32_t& dst0, const uint32_t& x0) { dst0 = (x0 >> 16) & 0xFFU; }
static FOG_INLINE void p32ExtractPBB3(uint32_t& dst0, const uint32_t& x0) { dst0 = (x0 >> 24);         }

static FOG_INLINE void p32ExtractPBW0(uint32_t& dst0, const uint32_t& x0) { dst0 = (x0      ) & 0xFFU; }
static FOG_INLINE void p32ExtractPBW1(uint32_t& dst0, const uint32_t& x0) { dst0 = (x0 >> 16);         }

static FOG_INLINE void p32ExtractPWW0(uint32_t& dst0, const uint32_t& x0) { dst0 = (x0      ) & 0xFFFFU; }
static FOG_INLINE void p32ExtractPWW1(uint32_t& dst0, const uint32_t& x0) { dst0 = (x0 >> 16);           }

// ============================================================================
// [Fog::Face - P32 - As]
// ============================================================================

static FOG_INLINE uint32_t p32PBB0AsU32(const uint32_t& x0) { return (x0      ) & 0xFFU; }
static FOG_INLINE uint32_t p32PBB1AsU32(const uint32_t& x0) { return (x0 >>  8) & 0xFFU; }
static FOG_INLINE uint32_t p32PBB2AsU32(const uint32_t& x0) { return (x0 >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p32PBB3AsU32(const uint32_t& x0) { return (x0 >> 24) & 0xFFU; }

static FOG_INLINE uint32_t p32PBW0AsU32(const uint32_t& x0) { return (x0      ) & 0xFFU; }
static FOG_INLINE uint32_t p32PBW1AsU32(const uint32_t& x0) { return (x0 >> 16) & 0xFFU; }

static FOG_INLINE uint32_t p32PWW0AsU32(const uint32_t& x0) { return (x0      ) & 0xFFFFU; }
static FOG_INLINE uint32_t p32PWW1AsU32(const uint32_t& x0) { return (x0 >> 16) & 0xFFFFU; }

// ============================================================================
// [Fog::Face - P32 - Pack]
// ============================================================================

//! @brief Pack x0_31 and x0_20 into single P32 value.
//!
//! @verbatim
//! dst0.u8[0] = x0_20.u16[0] or x0_20.u8[0]
//! dst0.u8[1] = x0_31.u16[0] or x0_31.u8[0]
//! dst0.u8[2] = x0_20.u16[1] or x0_20.u8[2]
//! dst0.u8[3] = x0_31.u16[1] or x0_31.u8[2]
//! @endverbatim
static FOG_INLINE void p32PackPBB2031FromPBW(
  uint32_t& dst0, const uint32_t& x0_20, const uint32_t& x0_31)
{
  dst0 = (x0_31 << 8) | x0_20;
}

// ============================================================================
// [Fog::Face - P32 - Unpack]
// ============================================================================

//! @brief Unpack 2nd and 0st BYTE of a single DWORD into the uint32_t.
//!
//! @verbatim
//! dst0.u16[0] = x0_20.u8[0]
//! dst0.u16[1] = x0_20.u8[2]
//! @endverbatim
static FOG_INLINE void p32UnpackPBWFromPBB_20(
  uint32_t& dst0, const uint32_t& x0_20)
{
  dst0 = x0_20 & 0x00FF00FFU;
}

//! @brief Unpack 3rd and 1st BYTE of a single DWORD into the uint32_t.
//!
//! @verbatim
//! dst0.u16[0] = x0_31.u8[1]
//! dst0.u16[1] = x0_31.u8[3]
//! @endverbatim
static FOG_INLINE void p32UnpackPBWFromPBB_31(
  uint32_t& dst0, const uint32_t& x0_31)
{
  dst0 = (x0_31 >> 8) & 0x00FF00FFU;
}

//! @brief Unpack all bytes of DWORD into two uint32_t values.
//!
//! @verbatim
//! dst0_20.u16[0] = x0.u8[0]
//! dst0_20.u16[1] = x0.u8[2]
//! dst0_31.u16[0] = x0.u8[1]
//! dst0_31.u16[1] = x0.u8[3]
//! @endverbatim
static FOG_INLINE void p32UnpackPBWFromPBB_2031(
  uint32_t& dst0_20, uint32_t& dst0_31, const uint32_t& x0)
{
  uint32_t t0 = x0;

  dst0_20 = t0;
  dst0_31 = t0 >> 8;

  dst0_20 &= 0x00FF00FFU;
  dst0_31 &= 0x00FF00FFU;
}

//! @brief Unpack '20_1' bytes of DWORD into two uint32_t values.
//!
//! @verbatim
//! dst0_20.u16[0] = x0.u8[0]
//! dst0_20.u16[1] = x0.u8[2]
//! dst0_31.u16[0] = x0.u8[1]
//! dst0_31.u16[1] = 0x00
//! @endverbatim
static FOG_INLINE void p32UnpackPBWFromPBB_20Z1(
  uint32_t& dst0_20, uint32_t& dst0_Z1, const uint32_t& x0)
{
  uint32_t t0 = x0;

  dst0_20 = t0;
  dst0_Z1 = t0 >> 8;

  dst0_20 &= 0x00FF00FFU;
  dst0_Z1 &= 0x000000FFU;
}

// ============================================================================
// [Fog::Face - P32 - Zero / Fill]
// ============================================================================

static FOG_INLINE void p32ZeroPBB0(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 & 0xFFFFFF00U; }
static FOG_INLINE void p32ZeroPBB1(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 & 0xFFFF00FFU; }
static FOG_INLINE void p32ZeroPBB2(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 & 0xFF00FFFFU; }
static FOG_INLINE void p32ZeroPBB3(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 & 0x00FFFFFFU; }

static FOG_INLINE void p32ZeroPBW0(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 & 0xFFFFFF00U; }
static FOG_INLINE void p32ZeroPBW1(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 & 0xFF00FFFFU; }

static FOG_INLINE void p32ZeroPWW0(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 & 0xFFFF0000U; }
static FOG_INLINE void p32ZeroPWW1(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 & 0x0000FFFFU; }

static FOG_INLINE void p32FillPBB0(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 | 0x000000FFU; }
static FOG_INLINE void p32FillPBB1(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 | 0x0000FF00U; }
static FOG_INLINE void p32FillPBB2(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 | 0x00FF0000U; }
static FOG_INLINE void p32FillPBB3(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 | 0xFF000000U; }

static FOG_INLINE void p32FillPBW0(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 | 0x00FF0000U; }
static FOG_INLINE void p32FillPBW1(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 | 0x000000FFU; }

static FOG_INLINE void p32FillPWW0(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 | 0x0000FFFFU; }
static FOG_INLINE void p32FillPWW1(uint32_t& dst0, const uint32_t& x0) { dst0 = x0 | 0xFFFF0000U; }

// ============================================================================
// [Fog::Face - P32 - Replace]
// ============================================================================

static FOG_INLINE void p32ReplacePBB0(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x000000FFU, u0      ); }
static FOG_INLINE void p32ReplacePBB1(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x0000FF00U, u0 <<  8); }
static FOG_INLINE void p32ReplacePBB2(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x00FF0000U, u0 << 16); }
static FOG_INLINE void p32ReplacePBB3(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0xFF000000U, u0 << 24); }

static FOG_INLINE void p32ReplacePBW0(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x000000FFU, u0      ); }
static FOG_INLINE void p32ReplacePBW1(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x00FF0000U, u0 << 16); }

static FOG_INLINE void p32ReplacePWW0(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x000000FFU, u0      ); }
static FOG_INLINE void p32ReplacePWW1(uint32_t& dst0, const uint32_t& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & 0x00FF0000U, u0 << 16); }

// ============================================================================
// [Fog::Face - P32 - Logical / Arithmetic]
// ============================================================================

//! @brief Scalar combine (AND or OR, depends on platform).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] +| y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Combine(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = _FOG_FACE_COMBINE_2(x0, y0);
}

//! @brief Scalar combine (AND or OR, depends on platform) (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] +| y0.u32[0]
//! dst1.u32[0] = x1.u32[0] +| y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Combine_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = _FOG_FACE_COMBINE_2(x0, y0);
  uint32_t t1 = _FOG_FACE_COMBINE_2(x1, y1);

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar AND.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] & y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32And(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = x0 & y0;
}

//! @brief Scalar AND (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] & y0.u32[0]
//! dst1.u32[0] = x1.u32[0] & y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32And_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = x0 & y0;
  uint32_t t1 = x1 & y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar OR.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] | y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Or(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = x0 | y0;
}

//! @brief Scalar OR (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] | y0.u32[0]
//! dst1.u32[0] = x1.u32[0] | y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Or_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = x0 | y0;
  uint32_t t1 = x1 | y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar XOR.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] ^ y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Xor(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = x0 ^ y0;
}

//! @brief Scalar XOR (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] ^ y0.u32[0]
//! dst1.u32[0] = x1.u32[0] ^ y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Xor_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = x0 ^ y0;
  uint32_t t1 = x1 ^ y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar negate.
//!
//! @verbatim
//! dst0.u32[0] = ~x0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Neg(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = ~x0;
}

//! @brief Scalar negate (2x).
//!
//! @verbatim
//! dst0.u32[0] = ~x0.u32[0]
//! dst1.u32[0] = ~x1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Neg_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  uint32_t t0 = ~x0;
  uint32_t t1 = ~x1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar add.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Add(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = x0 + y0;
}

//! @brief Scalar add (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0]
//! dst1.u32[0] = x1.u32[0] + y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Add_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = x0 + y0;
  uint32_t t1 = x1 + y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar subtract.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] - y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Sub(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = x0 - y0;
}

//! @brief Scalar subtract (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] - y0.u32[0]
//! dst1.u32[0] = x1.u32[0] - y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Sub_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = x0 - y0;
  uint32_t t1 = x1 - y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar multiply.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] * y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Mul(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = x0 * y0;
}

//! @brief Scalar multiply (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] * y0.u32[0]
//! dst1.u32[0] = x1.u32[0] * y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Mul_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = x0 * y0;
  uint32_t t1 = x1 * y1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar divide.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] / y0.u32[0]
//! @endverbatim
static FOG_INLINE void p32Div(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = x0 / y0;
}

//! @brief Scalar divide (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] / y0.u32[0]
//! dst1.u32[0] = x1.u32[0] / y1.u32[0]
//! @endverbatim
static FOG_INLINE void p32Div_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  dst0 = x0 / y0;
  dst1 = x1 / y1;
}

// ============================================================================
// [Fog::Face - P32 - Cvt]
// ============================================================================

// Cvt256From255(x):
//
//   == x + (x > 127)
//
//   == (x *   129) >>  7
//   == (x *   258) >>  8
//   == (x *   516) >>  9
//   == (x *  1032) >> 10
//   == (x *  2064) >> 11
//   == (x *  4128) >> 12
//   == (x *  8256) >> 13
//   == (x * 16512) >> 14
//   == (x * 33024) >> 15
//   == (x * 66048) >> 16

static FOG_INLINE void p32Cvt256SBWFrom255SBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 + (x0 > 127);
}

static FOG_INLINE void p32Cvt256PBWFrom255PBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 + ((x0 >> 7) & 0x00010001U);
}

static FOG_INLINE void p32Cvt256PBWFrom255PBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  uint32_t t0 = x0 + ((x0 >> 7) & 0x00010001U);
  uint32_t t1 = x1 + ((x1 >> 7) & 0x00010001U);

  dst0 = t0;
  dst1 = t1;
}

static FOG_INLINE void p32Cvt255SBWFrom256SBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 - (x0 > 127);
}

static FOG_INLINE void p32Cvt255PBWFrom256PBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 - ((x0 >> 7) & 0x00010001U);
}

static FOG_INLINE void p32Cvt255PBWFrom256PBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  uint32_t t0 = x0 - ((x0 >> 7) & 0x00010001U);
  uint32_t t1 = x1 - ((x1 >> 7) & 0x00010001U);

  dst0 = t0;
  dst1 = t1;
}

// ============================================================================
// [Fog::Face - P32 - Extend]
// ============================================================================

//! @brief Extend the LO byte in @c x0 and copy result into @c dst0.
//!
//! @verbatim
//! dst0 = (x0 << 24) | (x0 << 16) | (x0 << 8) | x0
//! @endverbatim
static FOG_INLINE void p32ExtendPBBFromSBB(
  uint32_t& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = x0 * 0x01010101U;
#else
  dst0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 8);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 16);
#endif
}

static FOG_INLINE void p32ExtendPBBFromSBB_ZZ10(
  uint32_t& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = x0 * 0x00000101U;
#else
  dst0 = _FOG_FACE_COMBINE_2(x0  , x0 <<  8);
#endif
}

static FOG_INLINE void p32ExtendPBBFromSBB_Z210(
  uint32_t& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = x0 * 0x00010101U;
#else
  uint32_t t0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(t0  , t0 <<  8);
  dst0 = _FOG_FACE_COMBINE_2(dst0, t0 << 16);
#endif
}

//! @brief Extend src0.B0 to dst0, src0.B1 must be zero before call.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p32ExtendPBWFromSBW(
  uint32_t& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = x0 * 0x00010001U;
#else
  dst0 = _FOG_FACE_COMBINE_2(x0, x0 << 16);
#endif
}

// ============================================================================
// [Fog::Face - P32 - Expand]
// ============================================================================

static FOG_INLINE void p32ExpandPBBFromPBB0(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 & 0xFFU;
  p32ExtendPBBFromSBB(dst0, dst0);
}

static FOG_INLINE void p32ExpandPBBFromPBB1(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = (x0 >> 8) & 0xFFU;
  p32ExtendPBBFromSBB(dst0, dst0);
}

static FOG_INLINE void p32ExpandPBBFromPBB2(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = (x0 >> 16) & 0xFFU;
  p32ExtendPBBFromSBB(dst0, dst0);
}

static FOG_INLINE void p32ExpandPBBFromPBB3(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 >> 24;
  p32ExtendPBBFromSBB(dst0, dst0);
}

//! @brief Expand scalar byte into uint32_t.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p32ExpandPBWFromPBW0(
  uint32_t& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = (x0 & 0xFFU) * 0x00010001U;
#else
  dst0 = _FOG_FACE_COMBINE_2(x0 & 0xFFU, x0 << 16);
#endif
}

//! @brief Expand src0.B0 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! dst1.u16[0] = x1.u16[0]
//! dst1.u16[1] = x1.u16[0]
//! @endverbatim
static FOG_INLINE void p32ExpandPBWFromPBW0_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  p32ExpandPBWFromPBW0(dst0, x0);
  p32ExpandPBWFromPBW0(dst1, x1);
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[1]
//! dst0.u16[1] = x0.u16[1]
//! @endverbatim
static FOG_INLINE void p32ExpandPBWFromPBW1(
  uint32_t& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = (x0 >> 16) * 0x00010001U;
#else
  dst0 = _FOG_FACE_COMBINE_2(x0 & 0x00FF0000U, x0 >> 16);
#endif
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[1]
//! dst0.u16[1] = x0.u16[1]
//! dst1.u16[0] = x1.u16[1]
//! dst1.u16[1] = x1.u16[1]
//! @endverbatim
static FOG_INLINE void p32ExpandPBWFromPBW1_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  p32ExpandPBWFromPBW1(dst0, x0);
  p32ExpandPBWFromPBW1(dst1, x1);
}

// ============================================================================
// [Fog::Face - P32 - LShift / RShift]
// ============================================================================

static FOG_INLINE void p32LShift(
  uint32_t& dst0, const uint32_t& x0, uint32_t s0)
{
  dst0 = x0 << s0;
}

static FOG_INLINE void p32LShift_2x(
  uint32_t& dst0, const uint32_t& x0, uint32_t s0,
  uint32_t& dst1, const uint32_t& x1, uint32_t s1)
{
  dst0 = x0 << s0;
  dst1 = x1 << s1;
}

static FOG_INLINE void p32LShiftTruncatePBW(
  uint32_t& dst0, const uint32_t& x0, uint32_t s0)
{
  FOG_ASSERT(s0 <= 8);

  dst0 = (x0 << s0) & 0x00FF00FFU;
}

static FOG_INLINE void p32LShiftTruncatePBW_2x(
  uint32_t& dst0, const uint32_t& x0, uint32_t s0,
  uint32_t& dst1, const uint32_t& x1, uint32_t s1)
{
  FOG_ASSERT(s0 <= 8);
  FOG_ASSERT(s1 <= 8);

  dst0 = (x0 << s0) & 0x00FF00FFU;
  dst1 = (x1 << s1) & 0x00FF00FFU;
}

static FOG_INLINE void p32LShiftBy1(
  uint32_t& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_HAS_FAST_ADD)
  dst0 = x0 + x0;
#else
  dst0 = x0 << 1;
#endif // FOG_ARCH_HAS_FAST_ADD
}

static FOG_INLINE void p32LShiftBy1_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
#if defined(FOG_ARCH_HAS_FAST_ADD)
  dst0 = x0 + x0;
  dst1 = x1 + x1;
#else
  dst0 = x0 << 1;
  dst1 = x1 << 1;
#endif // FOG_ARCH_HAS_FAST_ADD
}

static FOG_INLINE void p32LShiftBy1PBW0(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 + (x0 & 0x000000FFU);
}

static FOG_INLINE void p32LShiftBy1PBW0_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  dst0 = x0 + (x0 & 0x000000FFU);
  dst1 = x1 + (x1 & 0x000000FFU);
}

static FOG_INLINE void p32LShiftBy1PBW1(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 + (x0 & 0x00FF0000U);
}

static FOG_INLINE void p32LShiftBy1PBW1_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  dst0 = x0 + (x0 & 0x00FF0000U);
  dst1 = x1 + (x1 & 0x00FF0000U);
}

static FOG_INLINE void p32RShift(
  uint32_t& dst0, const uint32_t& x0, uint32_t s0)
{
  dst0 = x0 >> s0;
}

static FOG_INLINE void p32RShift_2x(
  uint32_t& dst0, const uint32_t& x0, uint32_t s0,
  uint32_t& dst1, const uint32_t& x1, uint32_t s1)
{
  dst0 = x0 >> s0;
  dst1 = x1 >> s1;
}

static FOG_INLINE void p32RShiftTruncatePBW(
  uint32_t& dst0, const uint32_t& x0, uint32_t s0)
{
  FOG_ASSERT(s0 <= 8);

  dst0 = (x0 >> s0) & 0x00FF00FFU;
}

static FOG_INLINE void p32RShiftTruncatePBW_2x(
  uint32_t& dst0, const uint32_t& x0, uint32_t s0,
  uint32_t& dst1, const uint32_t& x1, uint32_t s1)
{
  FOG_ASSERT(s0 <= 8);
  FOG_ASSERT(s1 <= 8);

  dst0 = (x0 >> s0) & 0x00FF00FFU;
  dst1 = (x1 >> s1) & 0x00FF00FFU;
}

// ============================================================================
// [Fog::Face - P32 - Negate255/256]
// ============================================================================

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! @endverbatim
static FOG_INLINE void p32Negate255SBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 ^ 0xFFU;
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst1.u16[0] = 255 - x1.u16[0]
//! @endverbatim
static FOG_INLINE void p32Negate255SBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  p32Negate255SBW(dst0, x0);
  p32Negate255SBW(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! @endverbatim
static FOG_INLINE void p32Negate256SBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = 256U - x0;
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst1.u16[0] = 256 - x1.u16[0]
//! @endverbatim
static FOG_INLINE void p32Negate256SBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  p32Negate256SBW(dst0, x0);
  p32Negate256SBW(dst1, x1);
}

//! @verbatim
//! dst0.u8[0] = 255 - x0.u8[0]
//! dst0.u8[1] = 255 - x0.u8[1]
//! dst0.u8[2] = 255 - x0.u8[2]
//! dst0.u8[3] = 255 - x0.u8[3]
//! @endverbatim
static FOG_INLINE void p32Negate255PBB(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = ~x0;
}

//! @verbatim
//! dst0.u8[0] = 255 - x0.u8[0]
//! dst0.u8[1] = 255 - x0.u8[1]
//! dst0.u8[2] = 255 - x0.u8[2]
//! dst0.u8[3] = 255 - x0.u8[3]
//! dst1.u8[0] = 255 - x1.u8[0]
//! dst1.u8[1] = 255 - x1.u8[1]
//! dst1.u8[2] = 255 - x1.u8[2]
//! dst1.u8[3] = 255 - x1.u8[3]
//! @endverbatim
static FOG_INLINE void p32Negate255PBB_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  dst0 = ~x0;
  dst1 = ~x1;
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst0.u16[1] = 255 - x0.u16[1]
//! @endverbatim
static FOG_INLINE void p32Negate255PBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 ^ 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst0.u16[1] = 255 - x0.u16[1]
//! dst1.u16[0] = 255 - x1.u16[0]
//! dst1.u16[1] = 255 - x1.u16[1]
//! @endverbatim
static FOG_INLINE void p32Negate255PBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  p32Negate255PBW(dst0, x0);
  p32Negate255PBW(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst0.u16[1] = 256 - x0.u16[1]
//! @endverbatim
static FOG_INLINE void p32Negate256PBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = 0x01000100U - x0;
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst0.u16[1] = 256 - x0.u16[1]
//! dst1.u16[0] = 256 - x1.u16[0]
//! dst1.u16[1] = 256 - x1.u16[1]
//! @endverbatim
static FOG_INLINE void p32Negate256PBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  dst0 = 0x01000100U - x0;
  dst1 = 0x01000100U - x1;
}

static FOG_INLINE void p32Negate255PBW0(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 ^ 0x000000FFU;
}

static FOG_INLINE void p32Negate255PBW0_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  dst0 = x0 ^ 0x000000FFU;
  dst1 = x1 ^ 0x000000FFU;
}

static FOG_INLINE void p32Negate255PBW1(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = x0 ^ 0x00FF0000U;
}

static FOG_INLINE void p32Negate255PBW1_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  dst0 = x0 ^ 0x00FF0000U;
  dst1 = x1 ^ 0x00FF0000U;
}

// ============================================================================
// [Fog::Face - P32 - Min]
// ============================================================================

//! @brief Take smaller value from @a u0 and @a u and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(x0, u0)
//! @endverbatim
static FOG_INLINE void p32MinPBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t x0Lo = x0 & 0xFFFFU;
  uint32_t x0Hi = x0 >> 16;

  if (x0Lo > u0) x0Lo = u0;
  if (x0Hi > u0) x0Hi = u0;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi << 16);
}

//! @brief Take smaller value from @a x0/u0 and @a x1/u1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(x0, u0)
//! dst1 = min(x1, u1)
//! @endverbatim
static FOG_INLINE void p32MinPBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u1)
{
  uint32_t x0Lo = x0 & 0x0000FFFFU;
  uint32_t x1Lo = x1 & 0x0000FFFFU;

  if (x0Lo > u0) x0Lo = u0;
  if (x1Lo > u1) x1Lo = u1;

  uint32_t x0Hi = x0 & 0xFFFF0000U;
  uint32_t x1Hi = x1 & 0xFFFF0000U;

  uint32_t t0Hi = u0 << 16;
  uint32_t t1Hi = u1 << 16;

  if (x0Hi > t0Hi) x0Hi = t0Hi;
  if (x1Hi > t1Hi) x1Hi = t1Hi;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi);
  dst1 = _FOG_FACE_COMBINE_2(x1Lo, x1Hi);
}

//! @brief Take smaller value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(x0, y0)
//! @endverbatim
static FOG_INLINE void p32MinPBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  uint32_t x0Lo = x0 & 0x0000FFFFU;
  uint32_t y0Lo = y0 & 0x0000FFFFU;

  uint32_t x0Hi = x0;
  uint32_t y0Hi = y0;

  if (x0Lo > y0Lo) x0Lo = y0Lo;
  if (x0Hi > y0Hi) x0Hi = y0Hi;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi & 0xFFFF0000);
}

//! @brief Take smaller value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0.u16[0] = min(x0.u16[0], y0.u16[0])
//! dst0.u16[1] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MinPBW_ZeroPBW1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  uint32_t x0Lo = x0 & 0x0000FFFFU;
  uint32_t y0Lo = y0 & 0x0000FFFFU;
  if (x0Lo > y0Lo) x0Lo = y0Lo;
  dst0 = x0Lo;
}

//! @brief Take smaller value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0.u16[0] = min(x0.u16[0], y0.u16[0])
//! dst0.u16[1] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MinPBW_FillPBW1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  uint32_t x0Lo = x0 | 0x00FF0000U;
  uint32_t y0Lo = y0 | 0x00FF0000U;
  if (x0Lo > y0Lo) x0Lo = y0Lo;
  dst0 = x0Lo;
}

//! @brief Take smaller value from @a x0/y0 and @c x1/y1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(x0, y0)
//! dst1 = min(x1, y1)
//! @endverbatim
static FOG_INLINE void p32MinPBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  p32MinPBW(dst0, x0, y0);
  p32MinPBW(dst1, x1, y1);
}

// ============================================================================
// [Fog::Face - P32 - Max]
// ============================================================================

//! @brief Take larger value from @a x0/u0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(x0, u0)
//! @endverbatim
static FOG_INLINE void p32MaxPBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t x0Lo = x0 & 0x0000FFFFU;
  uint32_t x0Hi = x0 >> 16;

  if (x0Lo < u0) x0Lo = u0;
  if (x0Hi < u0) x0Hi = u0;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi << 16);
}

//! @brief Take larger value from @a x0/u0 and @a x1/u1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(x0, u0)
//! dst1 = max(x1, u1)
//! @endverbatim
static FOG_INLINE void p32MaxPBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u1)
{
  uint32_t x0Lo = x0 & 0x0000FFFFU;
  uint32_t x1Lo = x1 & 0x0000FFFFU;

  if (x0Lo < u0) x0Lo = u0;
  if (x1Lo < u1) x1Lo = u1;

  uint32_t x0Hi = x0 & 0xFFFF0000U;
  uint32_t x1Hi = x1 & 0xFFFF0000U;

  uint32_t t0Hi = u0 << 16;
  uint32_t t1Hi = u1 << 16;

  if (x0Hi < t0Hi) x0Hi = t0Hi;
  if (x1Hi < t1Hi) x1Hi = t1Hi;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi);
  dst1 = _FOG_FACE_COMBINE_2(x1Lo, x1Hi);
}

//! @brief Take larger value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(x0, y0)
//! @endverbatim
static FOG_INLINE void p32MaxPBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  uint32_t x0Lo = x0 & 0x0000FFFFU;
  uint32_t y0Lo = y0 & 0x0000FFFFU;

  uint32_t x0Hi = x0;
  uint32_t y0Hi = y0;

  if (x0Lo < y0Lo) x0Lo = y0Lo;
  if (x0Hi < y0Hi) x0Hi = y0Hi;

  dst0 = _FOG_FACE_COMBINE_2(x0Lo, x0Hi & 0xFFFF0000);
}

//! @brief Take smaller value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0.u16[0] = max(x0.u16[0], y0.u16[0])
//! dst0.u16[1] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MaxPBW_ZeroPBW1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  uint32_t x0Lo = x0 & 0x0000FFFFU;
  uint32_t y0Lo = y0 & 0x0000FFFFU;
  if (x0Lo < y0Lo) x0Lo = y0Lo;
  dst0 = x0Lo;
}

//! @brief Take smaller value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0.u16[0] = max(x0.u16[0], y0.u16[0])
//! dst0.u16[1] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MaxPBW_FillPBW1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  uint32_t x0Lo = x0 | 0x00FF0000U;
  uint32_t y0Lo = y0 | 0x00FF0000U;
  if (x0Lo < y0Lo) x0Lo = y0Lo;
  dst0 = x0Lo;
}

//! @brief Take larger value from @a x0/y0 and @a x1/y1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(x0, y0)
//! dst1 = max(x1, y1)
//! @endverbatim
static FOG_INLINE void p32MaxPBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  p32MaxPBW(dst0, x0, y0);
  p32MaxPBW(dst1, x1, y1);
}

// ============================================================================
// [Fog::Face - P32 - Saturate]
// ============================================================================

static FOG_INLINE void p32Saturate255SBW(
  uint32_t& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_HAS_CMOV)
  dst0 = x0;
  if (dst0 > 0xFF) dst0 = 0xFF;
#else
  dst0 = (x0 | (0x0100U - ((x0 >> 8)))) & 0xFFU;
#endif
}

//! @brief Saturate @a dst0. Can be used after two BYTEs were added and
//! resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! @endverbatim
static FOG_INLINE void p32Saturate255PBW(
  uint32_t& dst0, const uint32_t& x0)
{
  // NOTE: The following code will work (and is more precise) too:
  //
  // dst0 = (x0 | (0x01000100U - ((x0 >> 8) & 0x00010001U))) & 0x00FF00FFU;
  //
  // The reason why the 0x00FF00FF is also used to mask (x0 >> 8) is that the
  // mask can be stored in register so it will be reused by another computation.

  dst0 = (x0 | (0x01000100U - ((x0 >> 8) & 0x00FF00FFU))) & 0x00FF00FFU;
}

//! @brief Saturate @a dst0/dst1. Can be used after two BYTEs were added
//! and resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! dst1 = min(dst1, 255)
//! @endverbatim
static FOG_INLINE void p32Saturate255PBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  p32Saturate255PBW(dst0, x0);
  p32Saturate255PBW(dst1, x1);
}

//! @brief Saturate @a dst0. Can be used after two BYTEs were added and
//! resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 511)
//! @endverbatim
static FOG_INLINE void p32Saturate511PBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = (x0 | (0x02000200U - ((x0 >> 9) & 0x00010001U))) & 0x01FF01FFU;
}

//! @brief Saturate @a dst0/dst1. Can be used after two BYTEs were added
//! and resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 511)
//! dst1 = min(dst1, 511)
//! @endverbatim
static FOG_INLINE void p32Saturate511PBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  p32Saturate511PBW(dst0, x0);
  p32Saturate511PBW(dst1, x1);
}

// ============================================================================
// [Fog::Face - P32 - Add / Addus]
// ============================================================================

static FOG_INLINE void p32AddPBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t t0;
  p32ExtendPBWFromSBW(t0, u0);
  p32Add(dst0, x0, t0);
}

static FOG_INLINE void p32AddPBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u01)
{
  uint32_t t01;
  p32ExtendPBWFromSBW(t01, u01);
  p32Add_2x(dst0, x0, t01, dst1, x1, t01);
}

static FOG_INLINE void p32AddPBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0, t1;
  p32ExtendPBWFromSBW(t0, u0);
  p32ExtendPBWFromSBW(t1, u1);
  p32Add_2x(dst0, x0, t0, dst1, x1, t1);
}

static FOG_INLINE void p32Addus255SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  p32Add(dst0, x0, y0);
  p32Saturate255SBW(dst0, dst0);
}

static FOG_INLINE void p32Addus255PBB(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t t1 = ((uint64_t)y0 | ((uint64_t)y0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 += t1;
  t0 |= FOG_UINT64_C(0x0100010001000100) - ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(x0, x0 >> 24));
#else
  uint32_t t0 = (x0     ) & 0x00FF00FFU;
  uint32_t t1 = (x0 >> 8) & 0x00FF00FFU;

  t0 += (y0     ) & 0x00FF00FFU;
  t1 += (y0 >> 8) & 0x00FF00FFU;

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 = (t0 & 0x00FF00FFU);
  t1 = (t1 & 0x00FF00FFU) << 8;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif // FOG_ARCH_BITS
}

static FOG_INLINE void p32Addus255PBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  p32Add(dst0, x0, y0);
  p32Saturate255PBW(dst0, dst0);
}

static FOG_INLINE void p32Addus255PBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  p32Add_2x(dst0, x0, y0, dst1, x1, y1);
  p32Saturate255PBW_2x(dst0, dst0, dst1, dst1);
}

static FOG_INLINE void p32Addus255PBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t t0;
  p32ExtendPBWFromSBW(t0, u0);
  p32Addus255PBW(dst0, x0, t0);
}

static FOG_INLINE void p32Addus255PBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0, t1;
  p32ExtendPBWFromSBW(t0, u0);
  p32ExtendPBWFromSBW(t1, u1);
  p32Addus255PBW_2x(dst0, x0, t0, dst1, x1, t1);
}

// ============================================================================
// [Fog::Face - P32 - Sub / Subus]
// ============================================================================

static FOG_INLINE void p32SubPBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t t0;
  p32ExtendPBWFromSBW(t0, u0);
  p32Sub(dst0, x0, t0);
}

static FOG_INLINE void p32SubPBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u01)
{
  uint32_t t01;
  p32ExtendPBWFromSBW(t01, u01);
  p32Sub_2x(dst0, x0, t01, dst1, x1, t01);
}

static FOG_INLINE void p32SubPBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0, t1;
  p32ExtendPBWFromSBW(t0, u0);
  p32ExtendPBWFromSBW(t1, u1);
  p32Sub_2x(dst0, x0, t0, dst1, x1, t1);
}

static FOG_INLINE void p32Subus255PBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  p32Negate255PBW(dst0, x0);
  p32Add(dst0, dst0, y0);
  p32Saturate255PBW(dst0, dst0);
  p32Negate255PBW(dst0, dst0);
}

//! @brief Special version of p32Subus255PBW, dst0.w[1] is set to zero.
static FOG_INLINE void p32Subus255PBW_ZeroPBW1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  p32Negate255PBW(dst0, x0);
  p32Add(dst0, dst0, y0);
  p32ZeroPBW1(dst0, dst0);
  p32Saturate255SBW(dst0, dst0);
  p32Negate255SBW(dst0, dst0);
}

//! @brief Special version of p32Subus255PBW, dst0.w[1] is set to 255.
static FOG_INLINE void p32Subus255PBW_FillPBW1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  p32Negate255PBW(dst0, x0);
  p32Add(dst0, dst0, y0);
  p32ZeroPBW1(dst0, dst0);
  p32Saturate255SBW(dst0, dst0);
  p32Negate255PBW(dst0, dst0);
}

static FOG_INLINE void p32Subus255PBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  p32Negate255PBW_2x(dst0, x0, dst1, x1);
  p32Add_2x(dst0, dst0, y0, dst1, dst1, y1);
  p32Saturate255PBW_2x(dst0, dst0, dst1, dst1);
  p32Negate255PBW_2x(dst0, dst0, dst1, dst1);
}

static FOG_INLINE void p32Subus255SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = x0 - y0;

#if defined(FOG_ARCH_HAS_CMOV)
  if ((int32_t)dst0 < 0) dst0 = 0;
#else
  dst0 &= (dst0 >> 24) ^ 0xFFU;
#endif
}

static FOG_INLINE void p32Subus255PBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t t0;
  p32ExtendPBWFromSBW(t0, u0);
  p32Subus255PBW(dst0, x0, t0);
}

static FOG_INLINE void p32Subus255PBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0, t1;
  p32ExtendPBWFromSBW(t0, u0);
  p32ExtendPBWFromSBW(t1, u1);
  p32Subus255PBW_2x(dst0, x0, t0, dst1, x1, t1);
}

// ============================================================================
// [Fog::Face - P32 - AddSub / AddSubus]
// ============================================================================

//! @brief Scalar add and subtract.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0] - z0.u32[0]
//! @endverbatim
static FOG_INLINE void p32AddSub(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  dst0 = x0 + y0 - z0;
}

//! @brief Scalar add and subtract (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0] - z0.u32[0]
//! dst1.u32[0] = x1.u32[0] + y1.u32[0] - z1.u32[0]
//! @endverbatim
static FOG_INLINE void p32AddSub_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1)
{
  uint32_t t0 = x0 + y0 - z0;
  uint32_t t1 = x1 + y1 - z1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar add, subtract and saturate.
//!
//! @verbatim
//! dst0.u16[0] = saturate255(x0.u16[0] + y0.u16[0] - z0.u16[0])
//! dst0.u16[1] = saturate255(x0.u16[1] + y0.u16[1] - z0.u16[1])
//! @endverbatim
static FOG_INLINE void p32AddSubus255PBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  uint32_t t0;

  p32Add(t0, x0, y0);
  p32Xor(t0, t0, 0x01FF01FFU);
  p32Add(t0, t0, z0);
  p32Saturate511PBW(t0, t0);
  p32Xor(t0, t0, 0x01FF01FFU);
  p32Saturate255PBW(t0, t0);
}

//! @brief Scalar add, subtract and saturate (2x).
//!
//! @verbatim
//! dst0.u16[0] = saturate255(x0.u16[0] + y0.u16[0] - z0.u16[0])
//! dst0.u16[1] = saturate255(x0.u16[1] + y0.u16[1] - z0.u16[1])
//! dst1.u16[0] = saturate255(x1.u16[0] + y1.u16[0] - z1.u16[0])
//! dst1.u16[1] = saturate255(x1.u16[1] + y1.u16[1] - z1.u16[1])
//! @endverbatim
static FOG_INLINE void p32AddSubus255PBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1)
{
  uint32_t t0, t1;
  p32AddSubus255PBW(t0, x0, y0, z0);
  p32AddSubus255PBW(t1, x1, y1, z1);

  dst0 = t0;
  dst1 = t1;
}

// ============================================================================
// [Fog::Face - P32 - Div]
// ============================================================================

//! @brief Scalar divide by 255.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] / 255).
//! @endverbatim
static FOG_INLINE void p32Div255SBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = ((x0 << 8U) + x0 + 256U) >> 16U;
}

//! @brief Packed divide by 255.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] / 255).
//! dst0.u16[1] = (x0.u16[1] / 255).
//! @endverbatim
static FOG_INLINE void p32Div255PBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = ((x0 + ((x0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @brief Packed divide by 255 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] / 255).
//! dst0.u16[1] = (x0.u16[1] / 255).
//! dst1.u16[0] = (x1.u16[0] / 255).
//! dst1.u16[1] = (x1.u16[1] / 255).
//! @endverbatim
static FOG_INLINE void p32Div255PBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  uint32_t t0 = ((x0 + ((x0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 + ((x1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Packed divide by 255 and Pack 0231.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] / 255).
//! dst0.u16[1] = (x0.u16[1] / 255).
//! dst1.u16[0] = (x1.u16[0] / 255).
//! dst1.u16[1] = (x1.u16[1] / 255).
//! @endverbatim
static FOG_INLINE void p32Div255PBW_2x_Pack_0231(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& x1)
{
  uint32_t t0 = ((x0 + ((x0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 + ((x1 >> 8) & 0x00FF00FFU) + 0x00800080U)     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed divide by 255 and Pack 02Z1.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] / 255).
//! dst0.u16[1] = (x0.u16[1] / 255).
//! dst1.u16[0] = (x1.u16[0] / 255).
//! dst1.u16[1] = 0x00.
//! @endverbatim
static FOG_INLINE void p32Div255PBW_2x_Pack_02Z1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& x1)
{
  uint32_t t0 = ((x0 + ((x0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 + ((x1 >> 8) & 0x000000FFU) + 0x00000080U)     ) & 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed divide by 255 and Pack 02Z1.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] / 255).
//! dst0.u16[1] = (x0.u16[1] / 255).
//! dst1.u16[0] = (x1.u16[0] / 255).
//! dst1.u16[1] = 0xFF.
//! @endverbatim
static FOG_INLINE void p32Div255PBW_2x_Pack_02F1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& x1)
{
  uint32_t t0 = ((x0 + ((x0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 + ((x1 >> 8) & 0x000000FFU) + 0x00000080U)     ) & 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_3(t0, t1, 0xFF000000);
}

//! @brief Scalar Divide by 256.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] / 256).
//! @endverbatim
static FOG_INLINE void p32Div256SBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = (x0 >> 8);
}

//! @brief Packed Divide by 256.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] / 256).
//! dst0.u16[1] = (x0.u16[1] / 256).
//! @endverbatim
static FOG_INLINE void p32Div256PBW(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = (x0 >> 8) & 0x00FF00FF;
}

//! @brief Packed Divide by 256 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] / 256).
//! dst0.u16[1] = (x0.u16[1] / 256).
//! dst1.u16[0] = (x1.u16[0] / 256).
//! dst1.u16[1] = (x1.u16[1] / 256).
//! @endverbatim
static FOG_INLINE void p32Div256PBW_2x(
  uint32_t& dst0, const uint32_t& x0,
  uint32_t& dst1, const uint32_t& x1)
{
  uint32_t t0 = (x0 >> 8) & 0x00FF00FF;
  uint32_t t1 = (x1 >> 8) & 0x00FF00FF;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Scalar Divide by 65535.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] / 65535).
//! @endverbatim
static FOG_INLINE void p32Div65535SWD(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = ((x0 + (x0 >> 16) + 0x8000U) >> 16);
}

//! @brief Scalar Divide by 65536.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] / 65536).
//! @endverbatim
static FOG_INLINE void p32Div65536SWD(
  uint32_t& dst0, const uint32_t& x0)
{
  dst0 = (x0 >> 16);
}

// ============================================================================
// [Fog::Face - P32 - Mul]
// ============================================================================

//! @brief Packed<-Scalar multiply.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0])
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0])
//! @endverbatim
static FOG_INLINE void p32MulPBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  dst0 = x0 * u0;
}

//! @brief Packed<-Scalar multiply.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0])
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0])
//! dst1.u16[0] = (x1.u16[0] * u1.u32[0])
//! dst1.u16[1] = (x1.u16[1] * u1.u32[0])
//! @endverbatim
static FOG_INLINE void p32MulPBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  dst0 = t0;
  dst1 = t1;
}

//! @brief Packed<-Packed multiply.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0])
//! dst0.u16[1] = (x0.u16[1] * u0.u32[1])
//! @endverbatim
static FOG_INLINE void p32MulPBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = _FOG_FACE_COMBINE_2((x0 & 0x000000FF) * (y0 & 0x000000FF), (x0 >> 16) * (y0 & 0x00FF0000));
}

//! @brief Packed<-Packed multiply.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0])
//! dst0.u16[1] = (x0.u16[1] * u0.u32[1])
//! dst1.u16[0] = (x1.u16[0] * u1.u32[0])
//! dst1.u16[1] = (x1.u16[1] * u1.u32[1])
//! @endverbatim
static FOG_INLINE void p32MulPBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = _FOG_FACE_COMBINE_2((x0 & 0x000000FF) * (y0 & 0x000000FF), (x0 >> 16) * (y0 & 0x00FF0000));
  uint32_t t1 = _FOG_FACE_COMBINE_2((x1 & 0x000000FF) * (y1 & 0x000000FF), (x1 >> 16) * (y1 & 0x00FF0000));

  dst0 = t0;
  dst1 = t1;
}

// ============================================================================
// [Fog::Face - P32 - MulDiv]
// ============================================================================

//! @brief Scalar Multiply and divide by 255.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] * u0.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255SBW(
  uint32_t &dst0, const uint32_t& x0, const uint32_t& u0)
{
  dst0 = x0 * u0;
  dst0 = ((dst0 + (dst0 >> 8) + 0x80U) >> 8);
}

//! @brief Scalar Multiply and divide by 256.
//!
//! @verbatim
//! dst0 = (x0.u32[0] * u0.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  dst0 = (x0 * u0) >> 8;
}

//! @brief Scalar Multiply and divide by 65535.
//!
//! @verbatim
//! dst0 = (x0.u32[0] * u0.u32[0]) / 65535
//! @endverbatim
static FOG_INLINE void p32MulDiv65535SWD(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  dst0 = x0 * u0;
  dst0 = ((dst0 + (dst0 >> 16) + 0x8000U) >> 16);
}

//! @brief Scalar Multiply and divide by 65536.
//!
//! @verbatim
//! dst0 = (x0.u32[0] * u0.u32[0]) / 65536
//! @endverbatim
static FOG_INLINE void p32MulDiv65536SWD(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  dst0 = (x0 * u0) >> 16;
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u32[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.u32[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.u32[0]) / 255
//! dst0.u8[3] = (x0.u8[3] * u0.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  t0 *= u0;
  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(t0, t0 >> 24));
#else
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * u0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * u0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif // FOG_ARCH_BITS
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.u32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.u32[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * u0.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * u0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * u0;

  t0 = (t0 & 0xFF00FF00U) >> 8;
  t1 = (t1 & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u32[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.u32[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.u32[0]) / 255
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_SBW_Z210(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
  t0 *= u0;
  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0x0000008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x000000FF00FF00FF);
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(t0, t0 >> 24));
#else
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * u0;
  uint32_t t1 = ((x0 >> 8) & 0x000000FFU) * u0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8)              ) + 0x00000080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif // FOG_ARCH_BITS
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.u32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.u32[0]) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_SBW_Z210(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t t0 = (x0 & 0x00FF00FFU) * u0;
  uint32_t t1 = (x0 & 0x0000FF00U) * u0;

  t0 = (t0 & 0xFF00FF00U);
  t1 = (t1 & 0x00FF0000U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u32[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.u32[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.u32[0]) / 255
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_SBW_F210(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
  t0 *= u0;
  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0xFF00008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  dst0 = (uint32_t)_FOG_FACE_COMBINE_2(t0, t0 >> 24);
#else
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * u0;
  uint32_t t1 = ((x0 >> 8) & 0x000000FFU) * u0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8)              ) + 0xFF000080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif // FOG_ARCH_BITS
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.u32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.u32[0]) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_SBW_F210(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t t0 = (x0 & 0x00FF00FFU) * u0;
  uint32_t t1 = (x0 & 0x0000FF00U) * u0;

  t0 = (t0 & 0xFF00FF00U);
  t1 = (t1 & 0x00FF0000U);

  dst0 = (_FOG_FACE_COMBINE_2(t0, t1) >> 8) | 0xFF000000;
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0.u8[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0.u8[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0.u8[2]) / 255
//! dst0.u8[3] = (x0.u8[3] * y0.u8[3]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = (uint64_t)(x0 & 0x000000FFU) * (uint64_t)(y0 & 0x000000FFU); // 0x000000000000____;
  uint64_t t1 = (uint64_t)(x0 & 0x0000FF00U) * (uint64_t)(y0 & 0x0000FF00U); // 0x00000000____0000;
  uint64_t t2 = (uint64_t)(x0 & 0x00FF0000U) * (uint64_t)(y0 & 0x00FF0000U); // 0x0000____00000000;
  uint64_t t3 = (uint64_t)(x0 & 0xFF000000U) * (uint64_t)(y0 & 0xFF000000U); // 0x____000000000000;

  t0 = _FOG_FACE_COMBINE_2(t0, t1);
  t2 = _FOG_FACE_COMBINE_2(t2, t3);
  t0 = _FOG_FACE_COMBINE_2(t0, t2);

  t0 = t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080);

  dst0 = ((uint32_t)(t0 >>  8) & 0x000000FFU) |
         ((uint32_t)(t0 >> 16) & 0x0000FF00U) |
         ((uint32_t)(t0 >> 24) & 0x00FF0000U) |
         ((uint32_t)(t0 >> 32) & 0xFF000000U) ;
#else
  uint32_t t0 = (x0 & 0x000000FFU) * (y0 & 0x000000FFU);
  uint32_t t1 = (x0 & 0x0000FF00U) * (y0 & 0x0000FF00U);

  uint32_t t2 = (x0 & 0x00FF0000U) * ((y0 >> 16) & 0x000000FFU);
  uint32_t t3 = (x0 >> 24        ) * ((y0 >> 24)              );

  t0 = _FOG_FACE_COMBINE_2(t0, t1);
  t2 = _FOG_FACE_COMBINE_2(t2, t3);

  t0 = t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U;
  t2 = t2 + ((t2 >> 8) & 0x00FF00FFU) + 0x00800080U;

  dst0 = _FOG_FACE_COMBINE_2(t0 >> 16   , (t2 << 16)) & 0xFF00FF00U;
  dst0 = _FOG_FACE_COMBINE_2(dst0, (t0 >>  8) & 0x000000FFU);
  dst0 = _FOG_FACE_COMBINE_2(dst0, (t2 >>  8) & 0x00FF0000U);
#endif // FOG_ARCH_BITS
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0.u8[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0.u8[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0.u8[2]) / 255
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_Z210(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = (uint64_t)(x0 & 0x000000FFU) * (uint64_t)(y0 & 0x000000FFU); // 0x000000000000____;
  uint64_t t1 = (uint64_t)(x0 & 0x0000FF00U) * (uint64_t)(y0 & 0x0000FF00U); // 0x00000000____0000;
  uint64_t t2 = (uint64_t)(x0 & 0x00FF0000U) * (uint64_t)(y0 & 0x00FF0000U); // 0x0000____00000000;

  t0 = _FOG_FACE_COMBINE_2(t0, t1);
  t0 = _FOG_FACE_COMBINE_2(t0, t2);

  t0 = t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080);

  dst0 = ((uint32_t)(t0 >>  8) & 0x000000FFU) |
         ((uint32_t)(t0 >> 16) & 0x0000FF00U) |
         ((uint32_t)(t0 >> 24) & 0x00FF0000U) ;
#else
  uint32_t t0 = (x0 & 0x000000FFU) * ((y0      ) & 0xFFU);
  uint32_t t1 = (x0 & 0x00FF0000U) * ((y0 >> 16) & 0xFFU);
  uint32_t t2 = ((x0 >> 8) & 0xFFU)* ((y0 >>  8) & 0xFFU);

  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t0 = t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U;
  t2 = t2 + ((t2 >> 8)              ) + 0x00000080U;

  dst0 = _FOG_FACE_COMBINE_2((t0 >> 8) & 0x00FF00FF, t2 & 0x0000FF00U);
#endif // FOG_ARCH_BITS
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_10.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_10.u16[1]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_32.u16[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * y0_32.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_1032(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_10, const uint32_t& y0_32)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_10; xm >>= 8;
  uint32_t t1 = (xm & 0xFFU) * y0_10; xm >>= 8;
  uint32_t t2 = (xm & 0xFFU) * y0_32; xm >>= 8;
  uint32_t t3 = (xm        ) * y0_32;

  dst0 = _FOG_FACE_COMBINE_4(((t0 >>  8) & 0x000000FFU), ((t1 >> 16) & 0x0000FF00U) ,
                             ((t2 >>  8) & 0x00FF0000U), ((t3      ) & 0xFF000000U) );
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_10.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_10.u16[1]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_32.u16[0]) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_10Z2(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_10, const uint32_t& y0_32)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_10; xm >>= 8;
  uint32_t t1 = (xm & 0xFFU) * y0_10; xm >>= 8;
  uint32_t t2 = (xm & 0xFFU) * y0_32;

  dst0 = _FOG_FACE_COMBINE_3(((t0 >>  8) & 0x000000FFU),
                             ((t1 >> 16) & 0x0000FF00U),
                             ((t2 <<  8) & 0x00FF0000U));
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_10.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_10.u16[1]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_32.u16[0]) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_10F2(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_10, const uint32_t& y0_32)
{
  p32MulDiv256PBB_PBW_10Z2(dst0, x0, y0_10, y0_32);
  p32FillPBB3(dst0, dst0);
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[0]) / 255
//! dst0.u8[3] = (x0.u8[3] * y0_31.u16[1]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_PBW_2031(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_20, const uint32_t& y0_31)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_20; xm >>= 8; // 0 0x0000FF00
  uint32_t t1 = (xm & 0xFFU) * y0_31; xm >>= 8; // 1 0x0000FF00
  uint32_t t2 = (xm & 0xFFU) * y0_20; xm >>= 8; // 2 0xFF000000
  uint32_t t3 = (xm        ) * y0_31;           // 3 0xFF000000

  t0 = (t0 & 0x0000FFFF) | (t1 & 0x0000FFFF);
  t2 = (t2 & 0x0000FFFF) | (t3 & 0x0000FFFF);

  t0 = t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U;
  t2 = t2 + ((t2 >> 8) & 0x00FF00FFU) + 0x00800080U;

  dst0 = _FOG_FACE_COMBINE_2((t0 >> 8) & 0x00FF00FF, t2 & 0xFF00FF00U);
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[1]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * y0_31.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_2031(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_20, const uint32_t& y0_31)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_20; xm >>= 8;
  uint32_t t1 = (xm & 0xFFU) * y0_31; xm >>= 8;
  uint32_t t2 = (xm & 0xFFU) * y0_20; xm >>= 8;
  uint32_t t3 = (xm        ) * y0_31;

  dst0 = _FOG_FACE_COMBINE_4(((t0 >>  8) & 0x000000FFU), (t1 & 0x0000FF00U) ,
                             ((t2 >>  8) & 0x00FF0000U), (t3 & 0xFF000000U) );
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[0]) / 255
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBB_PBW_20Z1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_20, const uint32_t& y0_31)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_20; xm >>= 8; // 0 0x0000FF00
  uint32_t t1 = (xm & 0xFFU) * y0_31; xm >>= 8; // 1 0x0000FF00
  uint32_t t2 = (xm & 0xFFU) * y0_20; xm >>= 8; // 2 0xFF000000

  t0 = (t0 & 0x0000FFFF) | (t1 & 0x0000FFFF);

  t0 = t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U;
  t2 = t2 + ((t2 >> 8) & 0x00FF00FFU) + 0x00800080U;

  dst0 = _FOG_FACE_COMBINE_2((t0 >> 8) & 0x00FF00FF, t2 & 0x0000FF00U);
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[1]) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_20Z1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_20, const uint32_t& y0_31)
{
  uint32_t xm = x0;
  uint32_t t0 = (xm & 0xFFU) * y0_20; xm >>= 8;
  uint32_t t1 = (xm & 0xFFU) * y0_31; xm >>= 8;
  uint32_t t2 = (xm        ) * y0_20;

  dst0 = _FOG_FACE_COMBINE_3(((t0 >>  8) & 0x000000FFU),
                             ((t1      ) & 0x0000FF00U),
                             ((t2      ) & 0x00FF0000U));
}

//! @brief Packed multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0_20.u16[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * y0_31.u16[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * y0_20.u16[1]) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBB_PBW_20F1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0_20, const uint32_t& y0_31)
{
  p32MulDiv256PBB_PBW_20Z1(dst0, x0, y0_20, y0_31);
  p32FillPBB3(dst0, dst0);
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  dst0 = x0 * u0;
  dst0 = ((dst0 + ((dst0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @brief Packed<-Scalar multiply and divide by 255 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0]) / 255
//! dst1.u16[0] = (x1.u16[0] * u1.u32[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * u1.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  dst0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  dst1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  dst0 = ((x0 * u0) >> 8) & 0x00FF00FFU;
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u32[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * u0.u32[0]) / 256
//! dst1.u16[0] = (x1.u16[0] * u1.u32[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * u1.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = ((x0 * u0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 * u1) >> 8) & 0x00FF00FFU;

  dst0 = t0;
  dst1 = t1;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = ((x0 & 0x000000FFU) * (y0 & 0x000000FFU)) | ((x0 & 0x00FF0000U) * (y0 >> 16));
  dst0 = ((dst0 + ((dst0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = 0x00
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_ZeroPBW1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = (x0 & 0xFF) * (y0 & 0xFF);
  dst0 = ((dst0 + (dst0 >> 8) + 0x80U) >> 8);
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = 0xFF
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_FillPBW1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = (x0 & 0xFF) * (y0 & 0xFF);
  dst0 = ((dst0 + (dst0 >> 8) + 0xFF000080U) >> 8);
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 255
//! dst1.u16[0] = (x1.u16[0] * y1.u16[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * y1.u16[1]) / 255
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = ((x0 & 0x000000FFU) * (y0 & 0x000000FFU)) | ((x0 & 0x00FF0000U) * (y0 >> 16));
  uint32_t t1 = ((x1 & 0x000000FFU) * (y1 & 0x000000FFU)) | ((x1 & 0x00FF0000U) * (y1 >> 16));

  dst0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  dst1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

static FOG_INLINE void p32MulDiv255PBW_2x_Pack_2031(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0,
  const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = ((x0 & 0x000000FFU) * (y0 & 0x000000FFU)) | ((x0 & 0x00FF0000U) * (y0 >> 16));
  uint32_t t1 = ((x1 & 0x000000FFU) * (y1 & 0x000000FFU)) | ((x1 & 0x00FF0000U) * (y1 >> 16));

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U)     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32MulDiv255PBW_2x_Pack_20Z1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0,
  const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = ((x0 & 0x000000FFU) * (y0 & 0x000000FFU)) | ((x0 & 0x00FF0000U) * (y0 >> 16));
  uint32_t t1 = ((x1 & 0x000000FFU) * (y1 & 0x000000FFU));

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x000000FFU) + 0x00000080U)     ) & 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32MulDiv255PBW_2x_Pack_20F1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0,
  const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = ((x0 & 0x000000FFU) * (y0 & 0x000000FFU)) | ((x0 & 0x00FF0000U) * (y0 >> 16));
  uint32_t t1 = ((x1 & 0x000000FFU) * (y1 & 0x000000FFU));

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x000000FFU) + 0xFF000080U)     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0)
{
  dst0 = ((((x0 & 0x0000FFFFU) * (y0 & 0x0000FFFFU)) | ((x0 & 0xFFFF0000U) * (y0 >> 16))) >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 256
//! dst1.u16[0] = (x1.u16[0] * y1.u16[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * y1.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = ((((x0 & 0x0000FFFFU) * (y0 & 0x0000FFFFU)) | ((x0 & 0xFFFF0000U) * (y0 >> 16))) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((((x1 & 0x0000FFFFU) * (y1 & 0x0000FFFFU)) | ((x1 & 0xFFFF0000U) * (y1 >> 16))) >> 8) & 0x00FF00FFU;

  dst0 = t0;
  dst1 = t1;
}

static FOG_INLINE void p32MulDiv256PBW_2x_Pack_2031(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0,
  const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = ((((x0 & 0x0000FFFFU) * (y0 & 0x0000FFFFU)) | ((x0 & 0xFFFF0000U) * (y0 >> 16))) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((((x1 & 0x0000FFFFU) * (y1 & 0x0000FFFFU)) | ((x1 & 0xFFFF0000U) * (y1 >> 16)))     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32MulDiv256PBW_2x_Pack_20Z1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0,
  const uint32_t& x1, const uint32_t& y1)
{
  uint32_t t0 = ((((x0 & 0x0000FFFFU) * (y0 & 0x0000FFFFU)) | ((x0 & 0xFFFF0000U) * (y0 >> 16))) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 & 0x0000FFFFU) * (y1 & 0x0000FFFFU)) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32MulDiv256PBW_2x_Pack_20F1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0,
  const uint32_t& x1, const uint32_t& y1)
{
  p32MulDiv256PBW_2x_Pack_20Z1(dst0, x0, y0, x1, y1);
  p32FillPBB3(dst0, dst0);
}

static FOG_INLINE void p32MulDiv65535PWW_SWD(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  uint32_t t0 = (x0 & 0xFFFF) * u0;
  uint32_t t1 = (x0    >> 16) * u0;

  dst0 = _FOG_FACE_COMBINE_2((t0 + (t0 >> 16) + 0x8000U) >> 16,
                             (t1 + (t1 >> 16) + 0x8000U) & 0xFFFF0000U);
}

// ============================================================================
// [Fog::Face - P32 - MulDiv_Pack]
// ============================================================================

//! @brief Packed<-Scalar multiply and divide by 255 (2x) and pack (2031).
//!
//! @verbatim
//! dst0 = pack2031((x0 * u0) / 255, (x1 * u1) / 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_2x_Pack_2031(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& u0,
  const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U)     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32MulDiv255PBW_SBW_2x_Pack_20F1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& u0,
  const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x000000FFU) + 0xFF000080U)     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (2031).
//!
//! @verbatim
//! dst0 = pack2031((x0 * u0) / 256, (x1 * u1) / 256)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_2031(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& u0,
  const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = ((x0 * u0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 * u1)     ) & 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (20Z1).
//!
//! @verbatim
//! dst0 = pack20Z1((x0 * u0) / 256, (x1 * u1) / 256)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_20Z1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& u0,
  const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = ((x0 * u0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 * u1)     ) & 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (20F1).
//!
//! @verbatim
//! dst0 = pack20F1((x0 * u0) / 256, (x1 * u1) / 256)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_20F1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& u0,
  const uint32_t& x1, const uint32_t& u1)
{
  p32MulDiv256PBW_SBW_2x_Pack_20Z1(dst0, x0, u0, x1, u1);
  p32FillPBB3(dst0, dst0);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (1032).
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_1032(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& u0,
  const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  dst0 = _FOG_FACE_COMBINE_4((t0 >>  8) & 0x000000FF, (t0 >> 16) & 0x0000FF00,
                             (t1 <<  8) & 0x00FF0000, (t1      ) & 0xFF000000);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (10Z2).
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_10Z2(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& u0,
  const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  dst0 = _FOG_FACE_COMBINE_3((t0 >>  8) & 0x000000FF, (t0 >> 16) & 0x0000FF00,
                             (t1 <<  8) & 0x00FF0000);
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x) and pack (10Z2).
static FOG_INLINE void p32MulDiv256PBW_SBW_2x_Pack_10F2(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& u0,
  const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  dst0 = _FOG_FACE_COMBINE_4((t0 >>  8) & 0x000000FF, (t0 >> 16) & 0x0000FF00,
                             (t1 <<  8) & 0x00FF0000, 0xFF000000);
}

//! @brief Packed<-Scalar multiply and divide by 255 (2x) and pack (20Z1).
//!
//! @verbatim
//! dst0 = pack((x0 * u0) / 255, (x1 * u1) / 255) & 0x00FFFFFF
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_2x_Pack_20Z1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& u0,
  const uint32_t& x1, const uint32_t& u1)
{
  uint32_t t0 = x0 * u0;
  uint32_t t1 = x1 * u1;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U)     ) & 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

// ============================================================================
// [Fog::Face - P32 - MulDiv_Add]
// ============================================================================

//! @verbatim
//! dst0 = (x0 * y0) / 255 + z0
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_Add(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  p32MulDiv255PBW(dst0, x0, y0);
  p32Add(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = (x0 * y0) / 255 + z0
//! dst1 = (x1 * y1) / 255 + z1
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_Add_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1)
{
  p32MulDiv255PBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Add_2x(dst0, dst0, z0, dst1, dst1, z1);
}

//! @verbatim
//! dst0 = (x0 * y0) / 256 + z0
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_Add(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  p32MulDiv256PBW(dst0, x0, y0);
  p32Add(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = (x0 * y0) / 256 + z0
//! dst1 = (x1 * y1) / 256 + z1
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_Add_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1)
{
  p32MulDiv256PBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Add_2x(dst0, dst0, z0, dst1, dst1, z1);
}

//! @verbatim
//! dst0.u8[0] = min((x0.u8[0] * y0.u32[0]) / 255 + z0.u8[0], 255)
//! dst0.u8[1] = min((x0.u8[1] * y0.u32[1]) / 255 + z0.u8[1], 255)
//! dst0.u8[2] = min((x0.u8[2] * y0.u32[2]) / 255 + z0.u8[2], 255)
//! dst0.u8[3] = min((x0.u8[3] * y0.u32[3]) / 255 + z0.u8[3], 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv255_PBB_SBB_Addus255PBB(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = ((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t t1 = ((uint64_t)z0 | ((uint64_t)z0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 *= y0;
  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 += t1;
  t0 |= FOG_UINT64_C(0x0100010001000100) - ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(t0, t0 >> 24));
#else
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * y0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * y0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;

  t0 += (z0     ) & 0x00FF00FFU;
  t1 += (z0 >> 8) & 0x00FF00FFU;

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 = (t0 & 0x00FF00FFU);
  t1 = (t1 & 0x00FF00FFU) << 8;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
#endif // FOG_ARCH_BITS
}

//! @verbatim
//! dst0 = min((x0 * y0) / 255 + z0, 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_AddusPBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  p32MulDiv255PBW_SBW(dst0, x0, y0);
  p32Addus255PBW(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = min((x0 * y0) / 255 + z0, 255)
//! dst1 = min((x1 * y1) / 255 + z1, 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_SBW_AddusPBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1)
{
  p32MulDiv255PBW_SBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Addus255PBW_2x(dst0, dst0, z1, dst1, dst1, z1);
}

//! @verbatim
//! dst0 = min((x0 * y0) / 256 + z0, 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_AddusPBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  p32MulDiv256PBW_SBW(dst0, x0, y0);
  p32Addus255PBW(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = min((z0 * y0) / 256 + z0, 255)
//! dst1 = min((z1 * y1) / 256 + z1, 255)
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_SBW_AddusPBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1)
{
  p32MulDiv256PBW_SBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Addus255PBW_2x(dst0, dst0, z1, dst1, dst1, z1);
}

//! @verbatim
//! dst0 = (x0 * y0) / 255 + z0
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_Addus255PBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  p32MulDiv255PBW(dst0, x0, y0);
  p32Addus255PBW(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = (x0 * y0) / 255 + z0
//! dst1 = (x1 * y1) / 255 + z1
//! @endverbatim
static FOG_INLINE void p32MulDiv255PBW_Addus255PBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1)
{
  p32MulDiv255PBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Addus255PBW_2x(dst0, dst0, z0, dst1, dst1, z1);
}

//! @verbatim
//! dst0 = (x0 * y0) / 256 + z0
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_Addus255PBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  p32MulDiv256PBW(dst0, x0, y0);
  p32Addus255PBW(dst0, dst0, z0);
}

//! @verbatim
//! dst0 = (x0 * y0) / 256 + z0
//! dst1 = (x1 * y1) / 256 + z1
//! @endverbatim
static FOG_INLINE void p32MulDiv256PBW_Addus255PBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1)
{
  p32MulDiv256PBW_2x(dst0, x0, y0, dst1, x1, y1);
  p32Addus255PBW_2x(dst0, dst0, z0, dst1, dst1, z1);
}

// ============================================================================
// [Fog::Face - P32 - Lerp]
// ============================================================================

//! @brief Scalar interpolate at interval [0, 255].
//!
//! @verbatim
//! dst0 = ( (x0 * z0) + (y0 * (255 - z0)) ) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  dst0 = x0 * z0 + y0 * (z0 ^ 0xFFU);
  dst0 = (dst0 + (dst0 >> 8) + 0x80U) >> 8;
}

//! @brief Scalar interpolate at interval [0, 255].
//!
//! @verbatim
//! dst0 = ( (x0 * z0) + (y0 * w0) ) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  dst0 = x0 * z0 + y0 * w0;
  dst0 = (dst0 + (dst0 >> 8) + 0x80U) >> 8;
}

//! @brief Scalar interpolate at interval [0, 256].
//!
//! @verbatim
//! dst0 = ( (x0 * z0) + (y0 * (256 - w0)) ) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  dst0 = x0 * z0 + y0 * (256U - z0);
  dst0 = dst0 >> 8;
}

//! @brief Scalar interpolate at interval [0, 256].
//!
//! @verbatim
//! dst0 = ( (x0 * z0) + (y0 * w0) ) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  dst0 = x0 * z0 + y0 * w0;
  dst0 = dst0 >> 8;
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * (255 - z0.u32[0])) / 255
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * (255 - z0.u32[0])) / 255
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * (255 - z0.u32[0])) / 255
//! dst0.u8[3] = (x0.u8[3] * z0.u32[0] + y0.u8[3] * (255 - z0.u32[0])) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255PBB_SBB(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  uint32_t zm = z0;

  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * zm;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * zm;

  zm ^= 0xFF;
  t0 += ((y0     ) & 0x00FF00FFU) * zm;
  t1 += ((y0 >> 8) & 0x00FF00FFU) * zm;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * w0.u32[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * w0.u32[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * w0.u32[0]) / 255
//! dst0.u8[3] = (x0.u8[3] * z0.u32[0] + y0.u8[3] * w0.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255PBB_SBB(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * z0 + ((y0     ) & 0x00FF00FFU) * w0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * z0 + ((y0 >> 8) & 0x00FF00FFU) * w0;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U) >> 8;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * (256 - z0.u32[0])) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * (256 - z0.u32[0])) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * (256 - z0.u32[0])) / 256
//! dst0.u8[3] = (x0.u8[3] * z0.u32[0] + y0.u8[3] * (256 - z0.u32[0])) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  uint32_t zm = z0;

  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * zm;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * zm;

  zm = 256 - zm;
  t0 += ((y0     ) & 0x00FF00FFU) * zm;
  t1 += ((y0 >> 8) & 0x00FF00FFU) * zm;

  t0 = (t0 & 0xFF00FF00U) >> 8;
  t1 = (t1 & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * (256 - z0.u32[0])) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * (256 - z0.u32[0])) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * (256 - z0.u32[0])) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW_10Z2(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  uint32_t zm = z0;

  uint32_t t0 = (x0 & 0x00FF00FFU) * zm;
  uint32_t t1 = (x0 & 0x0000FF00U) * zm;

  zm = 256 - zm;
  t0 += (y0 & 0x00FF00FFU) * zm;
  t1 += (y0 & 0x0000FF00U) * zm;

  t0 &= 0xFF00FF00U;
  t1 &= 0x00FF0000U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * (256 - z0.u32[0])) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * (256 - z0.u32[0])) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * (256 - z0.u32[0])) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW_10F2(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0)
{
  p32Lerp256PBB_SBW_10Z2(dst0, x0, y0, z0);
  p32FillPBB3(dst0, dst0);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * w0.u32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * w0.u32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * w0.u32[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * z0.u32[0] + y0.u8[3] * w0.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  uint32_t t0 = ((x0     ) & 0x00FF00FFU) * z0 + ((y0     ) & 0x00FF00FFU) * w0;
  uint32_t t1 = ((x0 >> 8) & 0x00FF00FFU) * z0 + ((y0 >> 8) & 0x00FF00FFU) * w0;

  t0 = (t0 & 0xFF00FF00U) >> 8;
  t1 = (t1 & 0xFF00FF00U);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * w0.u32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * w0.u32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * w0.u32[0]) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW_10Z2(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  uint32_t t0 = (x0 & 0x00FF00FFU) * z0 + (y0 & 0x00FF00FFU) * w0;
  uint32_t t1 = (x0 & 0x0000FF00U) * z0 + (y0 & 0x0000FF00U) * w0;

  t0 &= 0xFF00FF00U;
  t1 &= 0x00FF0000U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;
}

//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * z0.u32[0] + y0.u8[0] * w0.u32[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * z0.u32[0] + y0.u8[1] * w0.u32[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * z0.u32[0] + y0.u8[2] * w0.u32[0]) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_SBW_10F2(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  p32Lerp256PBB_SBW_10Z2(dst0, x0, y0, z0, w0);
  p32FillPBB3(dst0, dst0);
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255PBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  dst0 = (x0 * z0) + (y0 * w0);
  dst0 = ((dst0 + ((dst0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 255
//! dst1.u16[0] = (x1.u16[0] * z1.u32[0] + y1.u16[0] * w1.u32[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * z1.u32[0] + y1.u16[1] * w1.u32[0]) / 255
//! @endverbatim
static FOG_INLINE void p32Lerp255PBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1, const uint32_t& w1)
{
  uint32_t t0 = (x0 * z0) + (y0 * w0);
  uint32_t t1 = (x1 * z1) + (y1 * w1);

  dst0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  dst1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  dst0 = (x0 * z0) + (y0 * w0);
  dst0 = (dst0 >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 256
//! dst1.u16[0] = (x1.u16[0] * z1.u32[0] + y1.u16[0] * w1.u32[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * z1.u32[0] + y1.u16[1] * w1.u32[0]) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1, const uint32_t& w1)
{
  uint32_t t0 = (x0 * z0) + (y0 * w0);
  uint32_t t1 = (x1 * z1) + (y1 * w1);

  dst0 = (t0 >> 8) & 0x00FF00FFU;
  dst1 = (t1 >> 8) & 0x00FF00FFU;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = ( x0.u8[3] * z0_32.u16[1] + y0.u8[3] * (256 - z0_32.u16[1]) ) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_1032(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0_10, const uint32_t& z0_32)
{
  uint32_t xm = x0;
  uint32_t ym = y0;

  uint32_t t0;
  uint32_t t1;

  t0 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_10 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_10 >> 16   )) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, t1 & 0xFF00);

  xm >>= 16;
  ym >>= 16;

  t1 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_32 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 << 16));

  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_32 >> 16   )) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 & 0xFF00) << 16);

  dst0 = t0;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_10Z2(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0_10, const uint32_t& z0_32)
{
  uint32_t xm = x0;
  uint32_t ym = y0;

  uint32_t t0;
  uint32_t t1;

  t0 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_10 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_10 >> 16   )) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, t1 & 0xFF00);

  xm >>= 16;
  ym >>= 16;

  t1 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_32 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 << 16));

  dst0 = t0;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = 0xFF
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_10F2(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0_10, const uint32_t& z0_32)
{
  p32Lerp256PBB_PBW_10Z2(dst0, x0, y0, z0_10, z0_32);
  p32FillPBB3(dst0, dst0);
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = ( x0.u8[3] * z0_32.u16[1] + y0.u8[3] * (256 - z0_32.u16[1]) ) / 256
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_2031(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0_20, const uint32_t& z0_31)
{
  uint32_t xm = x0;
  uint32_t ym = y0;

  uint32_t t0;
  uint32_t t1;

  t0 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_20 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_31 & 0xFFFF)) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, t1 & 0xFF00);

  xm >>= 16;
  ym >>= 16;

  t1 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_20 >> 16)) >> 8) + (ym & 0x00FF) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 << 16));

  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_31 >> 16)) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 & 0xFF00) << 16);

  dst0 = t0;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_20Z1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0_20, const uint32_t& z0_31)
{
  uint32_t xm = x0;
  uint32_t ym = y0;

  uint32_t t0;
  uint32_t t1;

  t0 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_20 & 0xFFFF)) >> 8) + (ym & 0x00FF) );
  t1 = (uint32_t)( ((((xm & 0xFF00) - (ym & 0xFF00)) * (z0_31 & 0xFFFF)) >> 8) + (ym & 0xFF00) );
  t0 = _FOG_FACE_COMBINE_2(t0, t1 & 0xFF00);

  xm >>= 16;
  ym >>= 16;

  t1 = (uint32_t)( ((((xm & 0x00FF) - (ym & 0x00FF)) * (z0_20 >> 16)) >> 8) + (ym & 0x00FF) );
  t0 = _FOG_FACE_COMBINE_2(t0, (t1 << 16));

  dst0 = t0;
}

//! @verbatim
//! dst0.u8[0] = ( x0.u8[0] * z0_10.u16[0] + y0.u8[0] * (256 - z0_10.u16[0]) ) / 256
//! dst0.u8[1] = ( x0.u8[1] * z0_10.u16[1] + y0.u8[1] * (256 - z0_10.u16[1]) ) / 256
//! dst0.u8[2] = ( x0.u8[2] * z0_32.u16[0] + y0.u8[2] * (256 - z0_32.u16[0]) ) / 256
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p32Lerp256PBB_PBW_20F1(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0_20, const uint32_t& z0_31)
{
  p32Lerp256PBB_PBW_20Z1(dst0, x0, y0, z0_20, z0_31);
  p32FillPBB3(dst0, dst0);
}

//! @verbatim
//! dst0.u16[0] = min((x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 255, 255)
//! dst0.u16[1] = min((x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 255, 255)
//! @endverbatim
static FOG_INLINE void p32Lerpus255PBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  uint32_t t0;

  p32MulDiv255PBW_SBW(t0  , x0, z0);
  p32MulDiv255PBW_SBW(dst0, y0, z0);
  p32Addus255PBW(dst0, dst0, t0);
}

//! @verbatim
//! dst0.u16[0] = min((x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 255, 255)
//! dst0.u16[1] = min((x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 255, 255)
//! dst1.u16[0] = min((x1.u16[0] * z1.u32[0] + y1.u16[0] * w1.u32[0]) / 255, 255)
//! dst1.u16[1] = min((x1.u16[1] * z1.u32[0] + y1.u16[1] * w1.u32[0]) / 255, 255)
//! @endverbatim
static FOG_INLINE void p32Lerpus255PBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1, const uint32_t& w1)
{
  uint32_t t0, t1;

  p32MulDiv255PBW_SBW_2x(t0, x0, z0, t1, x1, z1);
  p32MulDiv255PBW_SBW_2x(dst0, y0, z0, dst1, y1, z1);
  p32Addus255PBW_2x(dst0, dst0, t0, dst1, dst1, t1);
}

//! @verbatim
//! dst0.u16[0] = min((x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 256, 255)
//! dst0.u16[1] = min((x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 256, 255)
//! @endverbatim
static FOG_INLINE void p32Lerpus256PBW_SBW(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0)
{
  uint32_t t0 = ((x0 * z0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((y0 * w0) >> 8) & 0x00FF00FFU;

  p32Addus255PBW(dst0, t0, t1);
}

//! @verbatim
//! dst0.u16[0] = min((x0.u16[0] * z0.u32[0] + y0.u16[0] * w0.u32[0]) / 256, 255)
//! dst0.u16[1] = min((x0.u16[1] * z0.u32[0] + y0.u16[1] * w0.u32[0]) / 256, 255)
//! dst1.u16[0] = min((x1.u16[0] * z1.u32[0] + y1.u16[0] * w1.u32[0]) / 256, 255)
//! dst1.u16[1] = min((x1.u16[1] * z1.u32[0] + y1.u16[1] * w1.u32[0]) / 256, 255)
//! @endverbatim
static FOG_INLINE void p32Lerpus256PBW_SBW_2x(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0,
  uint32_t& dst1, const uint32_t& x1, const uint32_t& y1, const uint32_t& z1, const uint32_t& w1)
{
  uint32_t t0 = ((x0 * z0) >> 8) & 0x00FF00FFU;
  uint32_t t1 = ((x1 * z1) >> 8) & 0x00FF00FFU;

  dst0 = ((y0 * w0) >> 8) & 0x00FF00FFU;
  dst1 = ((y1 * w1) >> 8) & 0x00FF00FFU;

  p32Addus255PBW_2x(dst0, dst0, t0, dst1, dst1, t1);
}

// ============================================================================
// [Fog::Face - P32 - Lerp_Pack]
// ============================================================================

static FOG_INLINE void p32Lerp255_Pack2031(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0,
  const uint32_t& x1, const uint32_t& y1, const uint32_t& z1, const uint32_t& w1)
{
  uint32_t t0 = x0 * z0 + y0 * w0;
  uint32_t t1 = x1 * z1 + y1 * w1;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) << 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32Lerp255_Pack20Z1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0,
  const uint32_t& x1, const uint32_t& y1, const uint32_t& z1, const uint32_t& w1)
{
  uint32_t t0 = x0 * z0 + y0 * w0;
  uint32_t t1 = x1 * z1 + y1 * w1;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) << 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32Lerp256_Pack2031(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0,
  const uint32_t& x1, const uint32_t& y1, const uint32_t& z1, const uint32_t& w1)
{
  uint32_t t0 = x0 * z0 + y0 * w0;
  uint32_t t1 = x1 * z1 + y1 * w1;

  t0 >>= 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0xFF00FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

static FOG_INLINE void p32Lerp256_Pack20Z1(
  uint32_t& dst0,
  const uint32_t& x0, const uint32_t& y0, const uint32_t& z0, const uint32_t& w0,
  const uint32_t& x1, const uint32_t& y1, const uint32_t& z1, const uint32_t& w1)
{
  uint32_t t0 = x0 * z0 + y0 * w0;
  uint32_t t1 = x1 * z1 + y1 * w1;

  t0 >>= 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0x0000FF00U;

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);
}

// ============================================================================
// [Fog::Face - P32 - Special]
// ============================================================================

static FOG_INLINE void p32MulDiv256PBB_3Z1Z_(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  dst0 = (((x0 & 0xFF00FF00U) >> 8) * u0) & 0xFF00FF00U;
}

static FOG_INLINE void p32MulDiv256PBB_Z2Z0_(
  uint32_t& dst0, const uint32_t& x0, const uint32_t& u0)
{
  dst0 = (((x0 & 0x00FF00FFU) * u0) >> 8) & 0x00FF00FFU;
}

// ============================================================================
// [Fog::Face - P32 - FloorPow2]
// ============================================================================

static FOG_INLINE void p32FloorPow2(uint32_t& dst, const uint32_t& src)
{
  dst  = src;
  dst |= dst >> 1;
  dst |= dst >> 2;
  dst |= dst >> 4;
  dst |= dst >> 8;
  dst |= dst >> 16;
  dst  = dst - (dst >> 1);
}

// ============================================================================
// [Fog::Face - P32 - CeilPow2]
// ============================================================================

static FOG_INLINE void p32CeilPow2(uint32_t& dst, const uint32_t& src)
{
  dst  = src - 1;
  dst |= dst >> 1;
  dst |= dst >> 2;
  dst |= dst >> 4;
  dst |= dst >> 8;
  dst |= dst >> 16;
  dst  = dst + 1;
}

// ============================================================================
// [Fog::Face - P64 - EMul2x32]
// ============================================================================

static FOG_INLINE void p64EMul2x32(__p64& dst, const uint32_t& x, const uint32_t& y)
{
#if defined(FOG_ARCH_NATIVE_P64)
  // __p64 is type only when CPU is 64-bit. In this case there is no sense
  // to use EMUL (64-bit multiplication can't overflow in our case).
  dst = (uint64_t)x * y;
#else
  dst.u64 = u64EMul2x32(x, y);
#endif
}

// ============================================================================
// [Fog::Face - U64 - Read / Store]
// ============================================================================

#if defined(FOG_ARCH_NATIVE_P64)

static FOG_INLINE void p64Load1b(__p64& dst0, const void* srcp) { dst0 = ((const uint8_t*)srcp)[0]; }
static FOG_INLINE void p64Store1b(void* dstp, const __p64& src0) { ((uint8_t *)dstp)[0] = (uint8_t )src0; }

static FOG_INLINE void p64Load2a(__p64& dst0, const void* srcp) { dst0 = ((const uint16_t*)srcp)[0]; }
static FOG_INLINE void p64Load2u(__p64& dst0, const void* srcp) { dst0 = ((const uint16_t*)srcp)[0]; }

static FOG_INLINE void p64Load2aBSwap(__p64& dst0, const void* srcp) { dst0 = MemOps::bswap16( ((const uint16_t*)srcp)[0] ); }
static FOG_INLINE void p64Load2uBSwap(__p64& dst0, const void* srcp) { dst0 = MemOps::bswap16( ((const uint16_t*)srcp)[0] ); }

static FOG_INLINE void p64Store2a(void* dstp, const __p64& src0) { ((uint16_t*)dstp)[0] = (uint16_t)src0; }
static FOG_INLINE void p64Store2u(void* dstp, const __p64& src0) { ((uint16_t*)dstp)[0] = (uint16_t)src0; }

static FOG_INLINE void p64Store2aBSwap(void* dstp, const __p64& src0) { ((uint16_t*)dstp)[0] = MemOps::bswap16( (uint16_t)src0 ); }
static FOG_INLINE void p64Store2uBSwap(void* dstp, const __p64& src0) { ((uint16_t*)dstp)[0] = MemOps::bswap16( (uint16_t)src0 ); }

static FOG_INLINE void p64Load4a(__p64& dst0, const void* srcp) { dst0 = ((const uint32_t*)srcp)[0]; }
static FOG_INLINE void p64Load4u(__p64& dst0, const void* srcp) { dst0 = ((const uint32_t*)srcp)[0]; }

static FOG_INLINE void p64Load4aBSwap(__p64& dst0, const void* srcp) { dst0 = MemOps::bswap32( ((const uint32_t*)srcp)[0] ); }
static FOG_INLINE void p64Load4uBSwap(__p64& dst0, const void* srcp) { dst0 = MemOps::bswap32( ((const uint32_t*)srcp)[0] ); }

static FOG_INLINE void p64Store4a(void* dstp, const __p64& src0) { ((uint32_t*)dstp)[0] = (uint32_t)src0; }
static FOG_INLINE void p64Store4u(void* dstp, const __p64& src0) { ((uint32_t*)dstp)[0] = (uint32_t)src0; }

static FOG_INLINE void p64Store4aBSwap(void* dstp, const __p64& src0) { ((uint32_t*)dstp)[0] = MemOps::bswap32( (uint32_t)src0 ); }
static FOG_INLINE void p64Store4uBSwap(void* dstp, const __p64& src0) { ((uint32_t*)dstp)[0] = MemOps::bswap32( (uint32_t)src0 ); }

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN

static FOG_INLINE void p64Load6a(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint32_t*)(src8 + 0))[0] ),
                              (uint64_t)( ((const uint16_t*)(src8 + 4))[0] ) << 32);
}

static FOG_INLINE void p64Load6u(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint32_t*)(src8 + 0))[0] ),
                              (uint64_t)( ((const uint16_t*)(src8 + 4))[0] ) << 32);
}

static FOG_INLINE void p64Load6aBSwap(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint32_t*)(src8 + 0))[0] ),
                              (uint64_t)( ((const uint16_t*)(src8 + 4))[0] ) << 32);
  dst0 = MemOps::bswap64(dst0) >> 16;
}

static FOG_INLINE void p64Load6uBSwap(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint32_t*)(src8 + 0))[0] ),
                              (uint64_t)( ((const uint16_t*)(src8 + 4))[0] ) << 32);
  dst0 = MemOps::bswap64(dst0) >> 16;
}

static FOG_INLINE void p64Store6a(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)(src0);
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)(src0 >> 32);
}

static FOG_INLINE void p64Store6u(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)(src0);
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)(src0 >> 32);
}

static FOG_INLINE void p64Store6aBSwap(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);
  uint64_t x0 = MemOps::bswap64(src0) >> 16;

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)(x0);
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)(x0 >> 32);
}

static FOG_INLINE void p64Store6uBSwap(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);
  uint64_t x0 = MemOps::bswap64(src0) >> 16;

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)(x0);
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)(x0 >> 32);
}

#else

static FOG_INLINE void p64Load6a(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint16_t*)(src8 + 0))[0] ) << 32,
                              (uint64_t)( ((const uint32_t*)(src8 + 2))[0] ) );
}

static FOG_INLINE void p64Load6u(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint16_t*)(src8 + 0))[0] ) << 32,
                              (uint64_t)( ((const uint32_t*)(src8 + 2))[0] ) );
}

static FOG_INLINE void p64Load6aBSwap(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint16_t*)(src8 + 0))[0] ) << 32,
                              (uint64_t)( ((const uint32_t*)(src8 + 2))[0] ) );
  dst0 = MemOps::bswap64(dst0) >> 16;
}

static FOG_INLINE void p64Load6uBSwap(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint16_t*)(src8 + 0))[0] ) << 32,
                              (uint64_t)( ((const uint32_t*)(src8 + 2))[0] ) );
  dst0 = MemOps::bswap64(dst0) >> 16;
}

static FOG_INLINE void p64Store6a(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = (uint32_t)(src0 >> 32);
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)(src0);
}

static FOG_INLINE void p64Store6u(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = (uint32_t)(src0 >> 32);
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)(src0);
}

static FOG_INLINE void p64Store6aBSwap(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);
  uint64_t x0 = MemOps::bswap64(src0);

  ((uint16_t*)(dst8 + 0))[0] = (uint32_t)(x0 >> 32);
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)(x0);
}

static FOG_INLINE void p64Store6uBSwap(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);
  uint64_t x0 = MemOps::bswap64(src0);

  ((uint16_t*)(dst8 + 0))[0] = (uint32_t)(x0 >> 32);
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)(x0);
}

#endif // FOG_BYTE_ORDER

static FOG_INLINE void p64Load8a(__p64& dst0, const void* srcp) { dst0 = ((const uint64_t*)srcp)[0]; }
static FOG_INLINE void p64Load8u(__p64& dst0, const void* srcp) { dst0 = ((const uint64_t*)srcp)[0]; }

static FOG_INLINE void p64Load8aBSwap(__p64& dst0, const void* srcp) { dst0 = MemOps::bswap64( ((const uint64_t*)srcp)[0] ); }
static FOG_INLINE void p64Load8uBSwap(__p64& dst0, const void* srcp) { dst0 = MemOps::bswap64( ((const uint64_t*)srcp)[0] ); }

static FOG_INLINE void p64Store8a(void* dstp, const __p64& src0) { ((uint64_t*)dstp)[0] = (uint64_t)src0; }
static FOG_INLINE void p64Store8u(void* dstp, const __p64& src0) { ((uint64_t*)dstp)[0] = (uint64_t)src0; }

static FOG_INLINE void p64Store8aBSwap(void* dstp, const __p64& src0) { ((uint64_t*)dstp)[0] = MemOps::bswap64( (uint64_t)src0 ); }
static FOG_INLINE void p64Store8uBSwap(void* dstp, const __p64& src0) { ((uint64_t*)dstp)[0] = MemOps::bswap64( (uint64_t)src0 ); }

#else

static FOG_INLINE void p64Load1b(__p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint8_t*)srcp)[0]; }
static FOG_INLINE void p64Store1b(void* dstp, const __p64& src0) { ((uint8_t *)dstp)[0] = (uint8_t )src0.u32Lo; }

static FOG_INLINE void p64Load2a(__p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint16_t*)srcp)[0]; }
static FOG_INLINE void p64Load2u(__p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint16_t*)srcp)[0]; }

static FOG_INLINE void p64Load2aBSwap(__p64& dst0, const void* srcp) { dst0.u32Lo = MemOps::bswap16( ((const uint16_t*)srcp)[0] ); }
static FOG_INLINE void p64Load2uBSwap(__p64& dst0, const void* srcp) { dst0.u32Lo = MemOps::bswap16( ((const uint16_t*)srcp)[0] ); }

static FOG_INLINE void p64Store2a(void* dstp, const __p64& src0) { ((uint16_t*)dstp)[0] = (uint16_t)src0.u32Lo; }
static FOG_INLINE void p64Store2u(void* dstp, const __p64& src0) { ((uint16_t*)dstp)[0] = (uint16_t)src0.u32Lo; }

static FOG_INLINE void p64Store2aBSwap(void* dstp, const __p64& src0) { ((uint16_t*)dstp)[0] = MemOps::bswap16( (uint16_t)src0.u32Lo ); }
static FOG_INLINE void p64Store2uBSwap(void* dstp, const __p64& src0) { ((uint16_t*)dstp)[0] = MemOps::bswap16( (uint16_t)src0.u32Lo ); }

static FOG_INLINE void p64Load4a(__p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint32_t*)srcp)[0]; }
static FOG_INLINE void p64Load4u(__p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint32_t*)srcp)[0]; }

static FOG_INLINE void p64Load4aBSwap(__p64& dst0, const void* srcp) { dst0.u32Lo = MemOps::bswap32( ((const uint32_t*)srcp)[0] ); }
static FOG_INLINE void p64Load4uBSwap(__p64& dst0, const void* srcp) { dst0.u32Lo = MemOps::bswap32( ((const uint32_t*)srcp)[0] ); }

static FOG_INLINE void p64Store4a(void* dstp, const __p64& src0) { ((uint32_t*)dstp)[0] = (uint32_t)src0.u32Lo; }
static FOG_INLINE void p64Store4u(void* dstp, const __p64& src0) { ((uint32_t*)dstp)[0] = (uint32_t)src0.u32Lo; }

static FOG_INLINE void p64Store4aBSwap(void* dstp, const __p64& src0) { ((uint32_t*)dstp)[0] = MemOps::bswap32( (uint32_t)src0.u32Lo ); }
static FOG_INLINE void p64Store4uBSwap(void* dstp, const __p64& src0) { ((uint32_t*)dstp)[0] = MemOps::bswap32( (uint32_t)src0.u32Lo ); }

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN

static FOG_INLINE void p64Load6a(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Lo = ((const uint32_t*)(src8 + 0))[0];
  dst0.u32Hi = ((const uint16_t*)(src8 + 4))[0];
}

static FOG_INLINE void p64Load6u(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Lo = ((const uint32_t*)(src8 + 0))[0];
  dst0.u32Hi = ((const uint16_t*)(src8 + 4))[0];
}

static FOG_INLINE void p64Load6aBSwap(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Hi = MemOps::bswap16( ((const uint16_t*)(src8 + 0))[0] );
  dst0.u32Lo = MemOps::bswap32( ((const uint32_t*)(src8 + 2))[0] );
}

static FOG_INLINE void p64Load6uBSwap(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Hi = MemOps::bswap16( ((const uint16_t*)(src8 + 0))[0] );
  dst0.u32Lo = MemOps::bswap32( ((const uint32_t*)(src8 + 2))[0] );
}

static FOG_INLINE void p64Store6a(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)src0.u32Lo;
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)src0.u32Hi;
}

static FOG_INLINE void p64Store6u(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)src0.u32Lo;
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)src0.u32Hi;
}

static FOG_INLINE void p64Store6aBSwap(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = MemOps::bswap16((uint16_t)src0.u32Hi);
  ((uint32_t*)(dst8 + 2))[0] = MemOps::bswap32((uint32_t)src0.u32Lo);
}

static FOG_INLINE void p64Store6uBSwap(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = MemOps::bswap16((uint16_t)src0.u32Hi);
  ((uint32_t*)(dst8 + 2))[0] = MemOps::bswap32((uint32_t)src0.u32Lo);
}

#else

static FOG_INLINE void p64Load6a(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Hi = ((const uint16_t*)(src8 + 0))[0];
  dst0.u32Lo = ((const uint32_t*)(src8 + 2))[0];
}

static FOG_INLINE void p64Load6u(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Hi = ((const uint16_t*)(src8 + 0))[0];
  dst0.u32Lo = ((const uint32_t*)(src8 + 2))[0];
}

static FOG_INLINE void p64Load6aBSwap(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Lo = MemOps::bswap32( ((const uint32_t*)(src8 + 0))[0] );
  dst0.u32Hi = MemOps::bswap16( ((const uint16_t*)(src8 + 4))[0] );
}

static FOG_INLINE void p64Load6uBSwap(__p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Lo = MemOps::bswap32( ((const uint32_t*)(src8 + 0))[0] );
  dst0.u32Hi = MemOps::bswap16( ((const uint16_t*)(src8 + 4))[0] );
}

static FOG_INLINE void p64Store6a(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = (uint16_t)src0.u32Hi;
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)src0.u32Lo;

}
static FOG_INLINE void p64Store6u(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = (uint16_t)src0.u32Hi;
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)src0.u32Lo;
}

static FOG_INLINE void p64Store6aBSwap(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = MemOps::bswap32( (uint32_t)src0.u32Lo );
  ((uint16_t*)(dst8 + 4))[0] = MemOps::bswap16( (uint16_t)src0.u32Hi );

}
static FOG_INLINE void p64Store6uBSwap(void* dstp, const __p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = MemOps::bswap32( (uint32_t)src0.u32Lo );
  ((uint16_t*)(dst8 + 4))[0] = MemOps::bswap16( (uint16_t)src0.u32Hi );
}

#endif // FOG_BYTE_ORDER

static FOG_INLINE void p64Load8a(__p64& dst0, const void* srcp)
{
  dst0.u32Lo = ((const uint32_t*)srcp)[0];
  dst0.u32Hi = ((const uint32_t*)srcp)[1];
}

static FOG_INLINE void p64Load8u(__p64& dst0, const void* srcp)
{
  dst0.u32Lo = ((const uint32_t*)srcp)[0];
  dst0.u32Hi = ((const uint32_t*)srcp)[1];
}

static FOG_INLINE void p64Load8aBSwap(__p64& dst0, const void* srcp)
{
  dst0.u32Hi = MemOps::bswap32( ((const uint32_t*)srcp)[0] );
  dst0.u32Lo = MemOps::bswap32( ((const uint32_t*)srcp)[1] );
}

static FOG_INLINE void p64Load8uBSwap(__p64& dst0, const void* srcp)
{
  dst0.u32Hi = MemOps::bswap32( ((const uint32_t*)srcp)[0] );
  dst0.u32Lo = MemOps::bswap32( ((const uint32_t*)srcp)[1] );
}

static FOG_INLINE void p64Store8a(void* dstp, const __p64& src0)
{
  ((uint32_t*)dstp)[0] = src0.u32Lo;
  ((uint32_t*)dstp)[1] = src0.u32Hi;
}

static FOG_INLINE void p64Store8u(void* dstp, const __p64& src0)
{
  ((uint32_t*)dstp)[0] = src0.u32Lo;
  ((uint32_t*)dstp)[1] = src0.u32Hi;
}

static FOG_INLINE void p64Store8aBSwap(void* dstp, const __p64& src0)
{
  ((uint32_t*)dstp)[0] = MemOps::bswap32( src0.u32Hi );
  ((uint32_t*)dstp)[1] = MemOps::bswap32( src0.u32Lo );
}

static FOG_INLINE void p64Store8uBSwap(void* dstp, const __p64& src0)
{
  ((uint32_t*)dstp)[0] = MemOps::bswap32( src0.u32Hi );
  ((uint32_t*)dstp)[1] = MemOps::bswap32( src0.u32Lo );
}

#endif // FOG_ARCH_NATIVE_P64

// ============================================================================
// [Fog::Face - P64 - Zero]
// ============================================================================

//! @brief Clear the @a dst0.
//!
//! @verbatim
//! dst0.u32[0] = 0
//! dst0.u32[1] = 0
//! @endverbatim
static FOG_INLINE void p64Zero(__p64& dst0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = 0;
#else
  dst0.u32Lo = 0;
  dst0.u32Hi = 0;
#endif
}

//! @brief Clear the @a dst0/dst1.
//!
//! @verbatim
//! dst0.u32[0] = 0
//! dst0.u32[1] = 0
//! dst1.u32[0] = 0
//! dst1.u32[1] = 0
//! @endverbatim
static FOG_INLINE void p64Zero_2x(__p64& dst0, __p64& dst1)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = 0;
  dst1 = 0;
#else
  dst0.u32Lo = 0;
  dst0.u32Hi = 0;
  dst1.u32Lo = 0;
  dst1.u32Hi = 0;
#endif
}

// ============================================================================
// [Fog::Face - P64 - Copy]
// ============================================================================

//! @brief Copy @a x0 to @a dst0.
//!
//! @verbatim
//! dst0 = x0
//! @endverbatim
static FOG_INLINE void p64Copy(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0;
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;
#endif
}

//! @brief Copy @a x0/x1 to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = x0
//! dst1 = x1
//! @endverbatim
static FOG_INLINE void p64Copy_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0;
  dst1 = x1;
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;
  dst1.u32Lo = x1.u32Lo;
  dst1.u32Hi = x1.u32Hi;
#endif
}

// ============================================================================
// [Fog::Face - P64 - From]
// ============================================================================

static FOG_INLINE __p64 p64FromU32(const uint32_t& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  return (uint64_t)x0;
#else
  __p64 packed;
  packed.u32Lo = x0;
  return packed;
#endif
}

static FOG_INLINE __p64 p64FromU64(const uint64_t& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  return (uint64_t)x0;
#else
  __p64 packed;
  packed.u64 = x0;
  return packed;
#endif
}

static FOG_INLINE uint64_t u64FromP64(const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  return (uint64_t)x0;
#else
  return x0.u64;
#endif
}

static FOG_INLINE uint32_t u32FromP64PDD0(const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  return (uint32_t)x0;
#else
  return x0.u32Lo;
#endif
}

static FOG_INLINE uint32_t u32FromP64PDD1(const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  return (uint32_t)(x0 >> 32);
#else
  return x0.u32Hi;
#endif
}

// ============================================================================
// [Fog::Face - P64 - As]
// ============================================================================

#if defined(FOG_ARCH_NATIVE_P64)

static FOG_INLINE uint32_t p64PBB0AsU32(const __p64& x0) { return (uint32_t)(x0      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB1AsU32(const __p64& x0) { return (uint32_t)(x0 >>  8) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB2AsU32(const __p64& x0) { return (uint32_t)(x0 >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB3AsU32(const __p64& x0) { return (uint32_t)(x0 >> 24) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB4AsU32(const __p64& x0) { return (uint32_t)(x0 >> 32) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB5AsU32(const __p64& x0) { return (uint32_t)(x0 >> 40) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB6AsU32(const __p64& x0) { return (uint32_t)(x0 >> 48) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB7AsU32(const __p64& x0) { return (uint32_t)(x0 >> 56) & 0xFFU; }

static FOG_INLINE uint32_t p64PBW0AsU32(const __p64& x0) { return (uint32_t)(x0      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW1AsU32(const __p64& x0) { return (uint32_t)(x0 >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW2AsU32(const __p64& x0) { return (uint32_t)(x0 >> 32) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW3AsU32(const __p64& x0) { return (uint32_t)(x0 >> 48); }

static FOG_INLINE uint32_t p64PWW0AsU32(const __p64& x0) { return (uint32_t)(x0      ) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW1AsU32(const __p64& x0) { return (uint32_t)(x0 >> 16) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW2AsU32(const __p64& x0) { return (uint32_t)(x0 >> 32) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW3AsU32(const __p64& x0) { return (uint32_t)(x0 >> 48); }

static FOG_INLINE uint32_t p64PWD0AsU32(const __p64& x0) { return (uint32_t)(x0); }
static FOG_INLINE uint32_t p64PWD1AsU32(const __p64& x0) { return (uint32_t)(x0 >> 32); }

static FOG_INLINE uint32_t p64PDD0AsU32(const __p64& x0) { return (uint32_t)(x0); }
static FOG_INLINE uint32_t p64PDD1AsU32(const __p64& x0) { return (uint32_t)(x0 >> 32); }

#else

static FOG_INLINE uint32_t p64PBB0AsU32(const __p64& x0) { return (x0.u32Lo      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB1AsU32(const __p64& x0) { return (x0.u32Lo >>  8) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB2AsU32(const __p64& x0) { return (x0.u32Lo >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB3AsU32(const __p64& x0) { return (x0.u32Lo >> 24);         }

static FOG_INLINE uint32_t p64PBB4AsU32(const __p64& x0) { return (x0.u32Hi      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB5AsU32(const __p64& x0) { return (x0.u32Hi >>  8) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB6AsU32(const __p64& x0) { return (x0.u32Hi >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB7AsU32(const __p64& x0) { return (x0.u32Hi >> 24);         }

static FOG_INLINE uint32_t p64PBW0AsU32(const __p64& x0) { return (x0.u32Lo      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW1AsU32(const __p64& x0) { return (x0.u32Lo >> 16);         }
static FOG_INLINE uint32_t p64PBW2AsU32(const __p64& x0) { return (x0.u32Hi      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW3AsU32(const __p64& x0) { return (x0.u32Hi >> 16);         }

static FOG_INLINE uint32_t p64PWW0AsU32(const __p64& x0) { return (x0.u32Lo      ) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW1AsU32(const __p64& x0) { return (x0.u32Lo >> 16);           }
static FOG_INLINE uint32_t p64PWW2AsU32(const __p64& x0) { return (x0.u32Hi      ) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW3AsU32(const __p64& x0) { return (x0.u32Hi >> 16);           }

static FOG_INLINE uint32_t p64PWD0AsU32(const __p64& x0) { return (x0.u32Lo); }
static FOG_INLINE uint32_t p64PWD1AsU32(const __p64& x0) { return (x0.u32Hi); }

static FOG_INLINE uint32_t p64PDD0AsU32(const __p64& x0) { return (x0.u32Lo); }
static FOG_INLINE uint32_t p64PDD1AsU32(const __p64& x0) { return (x0.u32Hi); }

#endif // FOG_ARCH_NATIVE_P64

// ============================================================================
// [Fog::Face - P64 - Pack]
// ============================================================================

//! @brief Pack u64_1x4b value into DWORD.
static FOG_INLINE void p64Pack2031(uint32_t& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(_U64(x0), _U64(x0) >> 24));
#else
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(_FOG_FACE_U64_LO(x0), _FOG_FACE_U64_HI(x0) << 8));
#endif
}

//! @brief Pack u64_1x4b value into DWORD.
static FOG_INLINE void p64Pack2031_RShift8(uint32_t& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(_U64(x0 >> 8), _U64(x0) >> 32));
#else
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(_FOG_FACE_U64_LO(x0) >> 8, _FOG_FACE_U64_HI(x0)));
#endif
}

// ============================================================================
// [Fog::Face - P64 - Unpack]
// ============================================================================

//! @brief Unpack 2031 BYTEs of @a x0 into one __p64 value.
static FOG_INLINE void p64UnpackPBWFromPBB2031(
  __p64& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  _U64(dst0) = (uint64_t)((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0     ) & 0x00FF00FFU;
  dst0.u32Hi = (x0 >> 8) & 0x00FF00FFU;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Unpack 2031 BYTEs of @a x0 into one __p64 value.
static FOG_INLINE void p64UnpackPBWFromPBB2031(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  _U64(dst0) = (uint64_t)(_U64(x0) | (_U64(x0) << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo     ) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Lo >> 8) & 0x00FF00FFU;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Unpack '20_1' bytes of DWORD into one __p64 value.
static FOG_INLINE void p64UnpackPBWFromPBB20Z1(
  __p64& dst0, const uint32_t& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  _U64(dst0) = (uint64_t)((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
#else
  dst0.u32Lo = (x0     ) & 0x00FF00FFU;
  dst0.u32Hi = (x0 >> 8) & 0x000000FFU;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Unpack '20_1' bytes of DWORD into one __p64 value.
static FOG_INLINE void p64UnpackPBWFromPBB20Z1(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  _U64(dst0) = (uint64_t)(x0 | (x0 << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo     ) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Lo >> 8) & 0x000000FFU;
#endif // FOG_ARCH_NATIVE_P64
}

// ============================================================================
// [Fog::Face - P64 - Zero / Fill]
// ============================================================================

#if defined(FOG_ARCH_NATIVE_P64)

static FOG_INLINE void p64ZeroPBB0(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFFFFFF); }
static FOG_INLINE void p64ZeroPBB1(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFF00FF); }
static FOG_INLINE void p64ZeroPBB2(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFF00FFFF); }
static FOG_INLINE void p64ZeroPBB3(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFF00FFFFFF); }
static FOG_INLINE void p64ZeroPBB4(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFF00FFFFFFFF); }
static FOG_INLINE void p64ZeroPBB5(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFF00FFFFFFFFFF); }
static FOG_INLINE void p64ZeroPBB6(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFF00FFFFFFFFFFFF); }
static FOG_INLINE void p64ZeroPBB7(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0x00FFFFFFFFFFFFFF); }

static FOG_INLINE void p64ZeroPBW0(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFFFF00); }
static FOG_INLINE void p64ZeroPBW1(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFF00FFFF); }
static FOG_INLINE void p64ZeroPBW2(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFF00FFFFFFFF); }
static FOG_INLINE void p64ZeroPBW3(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFF00FFFFFFFFFFFF); }

static FOG_INLINE void p64ZeroPWW0(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFF0000); }
static FOG_INLINE void p64ZeroPWW1(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFF0000FFFF); }
static FOG_INLINE void p64ZeroPWW2(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFF0000FFFFFFFF); }
static FOG_INLINE void p64ZeroPWW3(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0x0000FFFFFFFFFFFF); }

static FOG_INLINE void p64ZeroPWD0(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFF0000); }
static FOG_INLINE void p64ZeroPWD1(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFF0000FFFFFFFF); }

static FOG_INLINE void p64ZeroPDD0(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFF00000000); }
static FOG_INLINE void p64ZeroPDD1(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0x00000000FFFFFFFF); }

static FOG_INLINE void p64FillPBB0(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000000000FF); }
static FOG_INLINE void p64FillPBB1(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000000000FF00); }
static FOG_INLINE void p64FillPBB2(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000000000FF0000); }
static FOG_INLINE void p64FillPBB3(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000FF000000); }
static FOG_INLINE void p64FillPBB4(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000FF00000000); }
static FOG_INLINE void p64FillPBB5(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000FF0000000000); }
static FOG_INLINE void p64FillPBB6(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00FF000000000000); }
static FOG_INLINE void p64FillPBB7(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0xFF00000000000000); }

static FOG_INLINE void p64FillPBW0(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000000000FF); }
static FOG_INLINE void p64FillPBW1(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000000000FF0000); }
static FOG_INLINE void p64FillPBW2(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000FF00000000); }
static FOG_INLINE void p64FillPBW3(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00FF000000000000); }

static FOG_INLINE void p64FillPWW0(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000000000FFFF); }
static FOG_INLINE void p64FillPWW1(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000FFFF0000); }
static FOG_INLINE void p64FillPWW2(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000FFFF00000000); }
static FOG_INLINE void p64FillPWW3(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0xFFFF000000000000); }

static FOG_INLINE void p64FillPWD0(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000000000FFFF); }
static FOG_INLINE void p64FillPWD1(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000FFFF00000000); }

static FOG_INLINE void p64FillPDD0(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000FFFFFFFF); }
static FOG_INLINE void p64FillPDD1(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0xFFFFFFFF00000000); }

static FOG_INLINE void p64ZeroPBB3_7(__p64& dst0, const __p64& x0) { dst0 = x0 & FOG_UINT64_C(0x00FFFFFF00FFFFFF); }
static FOG_INLINE void p64FillPBB3_7(__p64& dst0, const __p64& x0) { dst0 = x0 | FOG_UINT64_C(0xFF000000FF000000); }

#else

static FOG_INLINE void p64ZeroPBB0(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFFFFFF00U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBB1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFFFF00FFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBB2(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFF00FFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBB3(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo & 0x00FFFFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBB4(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFFFFFF00U; }
static FOG_INLINE void p64ZeroPBB5(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFFFF00FFU; }
static FOG_INLINE void p64ZeroPBB6(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFF00FFFFU; }
static FOG_INLINE void p64ZeroPBB7(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0x00FFFFFFU; }

static FOG_INLINE void p64ZeroPBW0(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFFFFFF00U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBW1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFF00FFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBW2(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFFFFFF00U; }
static FOG_INLINE void p64ZeroPBW3(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFF00FFFFU; }

static FOG_INLINE void p64ZeroPWW0(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFFFF0000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPWW1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo & 0x0000FFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPWW2(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFFFF0000U; }
static FOG_INLINE void p64ZeroPWW3(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0x0000FFFFU; }

static FOG_INLINE void p64ZeroPWD0(__p64& dst0, const __p64& x0) { dst0.u32Lo = 0; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPWD1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = 0; }

static FOG_INLINE void p64ZeroPDD0(__p64& dst0, const __p64& x0) { dst0.u32Lo = 0; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPDD1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = 0; }

static FOG_INLINE void p64FillPBB0(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo | 0x000000FFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBB1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo | 0x0000FF00U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBB2(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo | 0x00FF0000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBB3(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo | 0xFF000000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBB4(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x000000FFU; }
static FOG_INLINE void p64FillPBB5(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x0000FF00U; }
static FOG_INLINE void p64FillPBB6(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x00FF0000U; }
static FOG_INLINE void p64FillPBB7(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0xFF000000U; }

static FOG_INLINE void p64FillPBW0(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo | 0x000000FFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBW1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo | 0x00FF0000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBW2(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x000000FFU; }
static FOG_INLINE void p64FillPBW3(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x00FF0000U; }

static FOG_INLINE void p64FillPWW0(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo | 0x0000FFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPWW1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo | 0xFFFF0000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPWW2(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x0000FFFFU; }
static FOG_INLINE void p64FillPWW3(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0xFFFF0000U; }

static FOG_INLINE void p64FillPWD0(__p64& dst0, const __p64& x0) { dst0.u32Lo = 0x0000FFFF; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPWD1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = 0x0000FFFF; }

static FOG_INLINE void p64FillPDD0(__p64& dst0, const __p64& x0) { dst0.u32Lo = 0xFFFFFFFF; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPDD1(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = 0xFFFFFFFF; }

static FOG_INLINE void p64ZeroPBB3_7(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo & 0x00FFFFFFU; dst0.u32Hi = x0.u32Hi & 0x00FFFFFFU; }
static FOG_INLINE void p64FillPBB3_7(__p64& dst0, const __p64& x0) { dst0.u32Lo = x0.u32Lo | 0xFF000000U; dst0.u32Hi = x0.u32Hi | 0xFF000000U; }

#endif // FOG_ARCH_NATIVE_P64

// ============================================================================
// [Fog::Face - P64 - Replace]
// ============================================================================

#if defined(FOG_ARCH_NATIVE_P64)

static FOG_INLINE void p64ReplacePBB0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000000000FF), u0      ); }
static FOG_INLINE void p64ReplacePBB1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FF00), u0 <<  8); }
static FOG_INLINE void p64ReplacePBB2(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000000000FF0000), u0 << 16); }
static FOG_INLINE void p64ReplacePBB3(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FF000000), u0 << 24); }
static FOG_INLINE void p64ReplacePBB4(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000FF00000000), u0 << 32); }
static FOG_INLINE void p64ReplacePBB5(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FF0000000000), u0 << 40); }
static FOG_INLINE void p64ReplacePBB6(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00FF000000000000), u0 << 48); }
static FOG_INLINE void p64ReplacePBB7(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFF00000000000000), u0 << 56); }

static FOG_INLINE void p64ReplacePBW0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000000000FF), u0      ); }
static FOG_INLINE void p64ReplacePBW1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000000000FF0000), u0 << 16); }
static FOG_INLINE void p64ReplacePBW2(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000FF00000000), u0 << 32); }
static FOG_INLINE void p64ReplacePBW3(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00FF000000000000), u0 << 48); }

static FOG_INLINE void p64ReplacePWW0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FFFF), u0      ); }
static FOG_INLINE void p64ReplacePWW1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FFFF0000), u0 << 16); }
static FOG_INLINE void p64ReplacePWW2(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FFFF00000000), u0 << 32); }
static FOG_INLINE void p64ReplacePWW3(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFFFF000000000000), u0 << 48); }

static FOG_INLINE void p64ReplacePWD0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FFFF), u0      ); }
static FOG_INLINE void p64ReplacePWD1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FFFF00000000), u0 << 32); }

static FOG_INLINE void p64ReplacePDD0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FFFFFFFF), u0      ); }
static FOG_INLINE void p64ReplacePDD1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFFFFFFFF00000000), u0 << 32); }

static FOG_INLINE void p64ReplacePBB0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000000000FF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePBB1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FF00), (uint64_t)u0 <<  8); }
static FOG_INLINE void p64ReplacePBB2(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000000000FF0000), (uint64_t)u0 << 16); }
static FOG_INLINE void p64ReplacePBB3(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FF000000), (uint64_t)u0 << 24); }
static FOG_INLINE void p64ReplacePBB4(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000FF00000000), (uint64_t)u0 << 32); }
static FOG_INLINE void p64ReplacePBB5(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FF0000000000), (uint64_t)u0 << 40); }
static FOG_INLINE void p64ReplacePBB6(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00FF000000000000), (uint64_t)u0 << 48); }
static FOG_INLINE void p64ReplacePBB7(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFF00000000000000), (uint64_t)u0 << 56); }

static FOG_INLINE void p64ReplacePBW0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000000000FF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePBW1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000000000FF0000), (uint64_t)u0 << 16); }
static FOG_INLINE void p64ReplacePBW2(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000FF00000000), (uint64_t)u0 << 32); }
static FOG_INLINE void p64ReplacePBW3(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00FF000000000000), (uint64_t)u0 << 48); }

static FOG_INLINE void p64ReplacePWW0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FFFF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePWW1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FFFF0000), (uint64_t)u0 << 16); }
static FOG_INLINE void p64ReplacePWW2(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FFFF00000000), (uint64_t)u0 << 32); }
static FOG_INLINE void p64ReplacePWW3(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFFFF000000000000), (uint64_t)u0 << 48); }

static FOG_INLINE void p64ReplacePWD0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FFFF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePWD1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FFFF00000000), (uint64_t)u0 << 32); }

static FOG_INLINE void p64ReplacePDD0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FFFFFFFF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePDD1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFFFFFFFF00000000), (uint64_t)u0 << 32); }

#else

static FOG_INLINE void p64ReplacePBB0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x000000FFU, u0.u32Lo      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x0000FF00U, u0.u32Lo <<  8); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB2(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, u0.u32Lo << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB3(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFF000000U, u0.u32Lo << 24); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB4(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x000000FFU, u0.u32Lo      ); }
static FOG_INLINE void p64ReplacePBB5(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x0000FF00U, u0.u32Lo <<  8); }
static FOG_INLINE void p64ReplacePBB6(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x00FF0000U, u0.u32Lo << 16); }
static FOG_INLINE void p64ReplacePBB7(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0xFF000000U, u0.u32Lo << 24); }

static FOG_INLINE void p64ReplacePBW0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x000000FFU, u0.u32Lo      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBW1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, u0.u32Lo << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBW2(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x000000FFU, u0.u32Lo      ); }
static FOG_INLINE void p64ReplacePBW3(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x00FF0000U, u0.u32Lo << 16); }

static FOG_INLINE void p64ReplacePWW0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x0000FFFFU, u0.u32Lo      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWW1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFFFF0000U, u0.u32Lo << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWW2(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x0000FFFFU, u0.u32Lo      ); }
static FOG_INLINE void p64ReplacePWW3(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0xFFFF0000U, u0.u32Lo << 16); }

static FOG_INLINE void p64ReplacePWD0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = u0.u32Lo; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWD1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = u0.u32Lo; }

static FOG_INLINE void p64ReplacePDD0(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = u0.u32Lo; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePDD1(__p64& dst0, const __p64& x0, const __p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = u0.u32Lo; }

static FOG_INLINE void p64ReplacePBB0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x000000FFU, u0      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x0000FF00U, u0 <<  8); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB2(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, u0 << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB3(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFF000000U, u0 << 24); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB4(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x000000FFU, u0     ); }
static FOG_INLINE void p64ReplacePBB5(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x0000FF00U, u0 <<  8); }
static FOG_INLINE void p64ReplacePBB6(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x00FF0000U, u0 << 16); }
static FOG_INLINE void p64ReplacePBB7(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0xFF000000U, u0 << 24); }

static FOG_INLINE void p64ReplacePBW0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x000000FFU, u0      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBW1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, u0 << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBW2(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x000000FFU, u0      ); }
static FOG_INLINE void p64ReplacePBW3(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x00FF0000U, u0 << 16); }

static FOG_INLINE void p64ReplacePWW0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x0000FFFFU, u0      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWW1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFFFF0000U, u0 << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWW2(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x0000FFFFU, u0      ); }
static FOG_INLINE void p64ReplacePWW3(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0xFFFF0000U, u0 << 16); }

static FOG_INLINE void p64ReplacePWD0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = u0; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWD1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = u0; }

static FOG_INLINE void p64ReplacePDD0(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = u0; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePDD1(__p64& dst0, const __p64& x0, const uint32_t& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = u0; }

#endif // FOG_ARCH_NATIVE_P64

// ============================================================================
// [Fog::Face - P64 - Logical / Arithmetic]
// ============================================================================

#if defined(FOG_ARCH_NATIVE_P64)
#define _FOG_FACE_64BIT_OP(_Dst_, _X_, _Y_, _Op_) \
  FOG_MACRO_BEGIN \
    _Dst_ = _X_ _Op_ _Y_; \
  FOG_MACRO_END
#else
#define _FOG_FACE_64BIT_OP(_Dst_, _X_, _Y_, _Op_) \
  FOG_MACRO_BEGIN \
    _Dst_.u32Lo = _X_.u32Lo _Op_ _Y_.u32Lo; \
    _Dst_.u32Hi = _X_.u32Hi _Op_ _Y_.u32Hi; \
  FOG_MACRO_END
#endif

//! @brief Scalar combine (AND or OR, depends on platform).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] +| y0.u32[0]
//! @endverbatim
static FOG_INLINE void p64Combine(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = _FOG_FACE_COMBINE_2(x0, y0);
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo, y0.u32Lo);
  dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi, y0.u32Hi);
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Scalar combine (AND or OR, depends on platform) (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] +| y0.u32[0]
//! dst1.u32[0] = x1.u32[0] +| y1.u32[0]
//! @endverbatim
static FOG_INLINE void p64Combine_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = _FOG_FACE_COMBINE_2(x0, y0);
  dst1 = _FOG_FACE_COMBINE_2(x1, y1);
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo, y0.u32Lo);
  dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi, y0.u32Hi);
  dst1.u32Lo = _FOG_FACE_COMBINE_2(x1.u32Lo, y1.u32Lo);
  dst1.u32Hi = _FOG_FACE_COMBINE_2(x1.u32Hi, y1.u32Hi);
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Scalar AND.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] & y0.u32[0]
//! @endverbatim
static FOG_INLINE void p64And(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, &);
}

//! @brief Scalar AND (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] & y0.u32[0]
//! dst1.u32[0] = x1.u32[0] & y1.u32[0]
//! @endverbatim
static FOG_INLINE void p64And_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, &);
  _FOG_FACE_64BIT_OP(dst1, x1, y1, &);
}

//! @brief Scalar OR.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] | y0.u32[0]
//! @endverbatim
static FOG_INLINE void p64Or(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, |);
}

//! @brief Scalar OR (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] | y0.u32[0]
//! dst0.u32[1] = x0.u32[1] | y0.u32[1]
//! dst1.u32[0] = x1.u32[0] | y1.u32[0]
//! dst1.u32[1] = x1.u32[1] | y1.u32[1]
//! @endverbatim
static FOG_INLINE void p64Or_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, |);
  _FOG_FACE_64BIT_OP(dst1, x1, y1, |);
}

//! @brief Scalar XOR.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] ^ y0.u32[0]
//! dst0.u32[1] = x0.u32[1] ^ y0.u32[1]
//! @endverbatim
static FOG_INLINE void p64Xor(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, ^);
}

//! @brief Scalar XOR (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] ^ y0.u32[0]
//! dst0.u32[1] = x0.u32[1] ^ y0.u32[1]
//! dst1.u32[0] = x1.u32[0] ^ y1.u32[0]
//! dst1.u32[1] = x1.u32[1] ^ y1.u32[1]
//! @endverbatim
static FOG_INLINE void p64Xor_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, ^);
  _FOG_FACE_64BIT_OP(dst1, x1, y1, ^);
}

//! @brief Scalar negate.
//!
//! @verbatim
//! dst0.u32[0] = ~x0.u32[0]
//! dst0.u32[1] = ~x0.u32[1]
//! @endverbatim
static FOG_INLINE void p64Neg(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = ~x0;
#else
  dst0.u32Lo = ~x0.u32Lo;
  dst0.u32Hi = ~x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Scalar negate (2x).
//!
//! @verbatim
//! dst0.u32[0] = ~x0.u32[0]
//! dst0.u32[1] = ~x0.u32[1]
//! dst1.u32[0] = ~x1.u32[0]
//! dst1.u32[1] = ~x1.u32[1]
//! @endverbatim
static FOG_INLINE void p64Neg_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = ~x0;
  dst1 = ~x1;
#else
  dst0.u32Lo = ~x0.u32Lo;
  dst0.u32Hi = ~x0.u32Hi;
  dst1.u32Lo = ~x1.u32Lo;
  dst1.u32Hi = ~x1.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Scalar add.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0]
//! @endverbatim
static FOG_INLINE void p64Add(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) + _FOG_FACE_U64(y0);
}

//! @brief Scalar add (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0]
//! dst0.u32[1] = x0.u32[1] + y0.u32[1]
//! dst1.u32[0] = x1.u32[0] + y1.u32[0]
//! dst1.u32[1] = x1.u32[1] + y1.u32[1]
//! @endverbatim
static FOG_INLINE void p64Add_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) + _FOG_FACE_U64(y0);
  _FOG_FACE_U64(dst1) = _FOG_FACE_U64(x1) + _FOG_FACE_U64(y1);
}

//! @brief Scalar subtract.
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] - y0.u64[0]
//! @endverbatim
static FOG_INLINE void p64Sub(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) - _FOG_FACE_U64(y0);
}

//! @brief Scalar subtract (2x).
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] - y0.u64[0]
//! dst1.u64[0] = x1.u64[0] - y1.u64[0]
//! @endverbatim
static FOG_INLINE void p64Sub_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) - _FOG_FACE_U64(y0);
  _FOG_FACE_U64(dst1) = _FOG_FACE_U64(x1) - _FOG_FACE_U64(y1);
}

//! @brief Scalar multiply.
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] * y0.u64[0]
//! @endverbatim
static FOG_INLINE void p64Mul(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) * _FOG_FACE_U64(y0);
}

//! @brief Scalar multiply (2x).
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] * y0.u64[0]
//! dst1.u64[0] = x1.u64[0] * y1.u64[0]
//! @endverbatim
static FOG_INLINE void p64Mul_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) * _FOG_FACE_U64(y0);
  _FOG_FACE_U64(dst1) = _FOG_FACE_U64(x1) * _FOG_FACE_U64(y1);
}

//! @brief Scalar divide.
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] / y0.u64[0]
//! @endverbatim
static FOG_INLINE void p64Div(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) / _FOG_FACE_U64(y0);
}

//! @brief Scalar divide (2x).
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] / y0.u64[0]
//! dst1.u64[0] = x1.u64[0] / y1.u64[0]
//! @endverbatim
static FOG_INLINE void p64Div_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) / _FOG_FACE_U64(y0);
  _FOG_FACE_U64(dst1) = _FOG_FACE_U64(x1) / _FOG_FACE_U64(y1);
}

// ============================================================================
// [Fog::Face - P64 - Cvt]
// ============================================================================

static FOG_INLINE void p64Cvt256SBWFrom255SBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 + (x0 > 127);
#else
  dst0.u32Lo = x0.u32Lo + (x0.u32Lo > 127);
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Cvt256PBWFrom255PBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 + ((x0 >> 7) & FOG_UINT64_C(0x0001000100010001));
#else
  dst0.u32Lo = x0.u32Lo + ((x0.u32Lo >> 7) & 0x00010001U);
  dst0.u32Hi = x0.u32Hi + ((x0.u32Hi >> 7) & 0x00010001U);
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Cvt256PBWFrom255PBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Cvt256PBWFrom255PBW(dst0, x0);
  p64Cvt256PBWFrom255PBW(dst1, x1);
}

static FOG_INLINE void p64Cvt255SBWFrom256SBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 - (x0 > 127);
#else
  dst0.u32Lo = x0.u32Lo - (x0.u32Lo > 127);
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Cvt255PBWFrom256PBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 - ((x0 >> 7) & FOG_UINT64_C(0x0001000100010001));
#else
  dst0.u32Lo = x0.u32Lo - ((x0.u32Lo >> 7) * 0x00010001U);
  dst0.u32Hi = x0.u32Hi - ((x0.u32Hi >> 7) * 0x00010001U);
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Cvt255PBWFrom256PBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Cvt255PBWFrom256PBW(dst0, x0);
  p64Cvt255PBWFrom256PBW(dst1, x1);
}

// ============================================================================
// [Fog::Face - P64 - Extend]
// ============================================================================

//! @brief Extend the LO byte in @c x0 and copy result into @c dst0.
//!
//! @verbatim
//! dst0.u8[0] = x0.u8[0]
//! dst0.u8[1] = x0.u8[0]
//! dst0.u8[2] = x0.u8[0]
//! dst0.u8[3] = x0.u8[0]
//! dst0.u8[4] = x0.u8[0]
//! dst0.u8[5] = x0.u8[0]
//! dst0.u8[6] = x0.u8[0]
//! dst0.u8[7] = x0.u8[0]
//! @endverbatim
static FOG_INLINE void p64ExtendPBBFromSBB(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)

#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = x0 * FOG_UINT64_C(0x0101010101010101);
#else
  dst0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 <<  8);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 16);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 32);
#endif // FOG_ARCH_HAS_FAST_MUL

#else

#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0.u32Lo = x0.u32Lo * 0x01010101U;
  dst0.u32Hi = dst0.u32Lo;
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Lo = _FOG_FACE_COMBINE_2(dst0.u32Lo, dst0.u32Lo <<  8);
  dst0.u32Lo = _FOG_FACE_COMBINE_2(dst0.u32Lo, dst0.u32Lo << 16);
  dst0.u32Hi = dst0.u32Lo;
#endif // FOG_ARCH_HAS_FAST_MUL

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Extend src0.B0 to dst0, src0.B1 must be zero before call.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! dst0.u16[2] = x0.u16[0]
//! dst0.u16[3] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64ExtendPBWFromSBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)

#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = x0 * FOG_UINT64_C(0x0001000100010001);
#else
  dst0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 16);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 32);
#endif // FOG_ARCH_HAS_FAST_MUL

#else

#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0.u32Lo = x0.u32Lo * FOG_UINT64_C(0x00010001);
  dst0.u32Hi = dst0.u32Lo;
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo, x0.u32Lo << 16);
  dst0.u32Hi = dst0.u32Lo;
#endif // FOG_ARCH_HAS_FAST_MUL

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Extend src0.B0 to dst0, src0.B1 must be zero before call.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! dst0.u16[2] = x0.u16[0]
//! dst0.u16[3] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64ExtendPWWFromSWW(
  __p64& dst0, const __p64& x0)
{
  p64ExtendPBWFromSBW(dst0, x0);
}
// ============================================================================
// [Fog::Face - P64 - Expand]
// ============================================================================

//! @brief Expand scalar byte into __p64.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64ExpandPBWFromPBW0(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)

#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = (x0 & FOG_UINT64_C(0xFF)) * FOG_UINT64_C(0x0001000100010001);
#else
  dst0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 16);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 32);
#endif // FOG_ARCH_HAS_FAST_MUL

#else

#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0.u32Lo = (x0.u32Lo & 0xFF) * 0x00010001U;
  dst0.u32Hi = dst0.u32Lo;
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFF, x0.u32Lo << 16);
  dst0.u32Hi = dst0.u32Lo;
#endif // FOG_ARCH_HAS_FAST_MUL

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Expand src0.B0 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! dst1.u16[0] = x1.u16[0]
//! dst1.u16[1] = x1.u16[0]
//! @endverbatim
static FOG_INLINE void p64ExpandPBWFromPBW0_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64ExpandPBWFromPBW0(dst0, x0);
  p64ExpandPBWFromPBW0(dst1, x1);
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[1]
//! dst0.u16[1] = x0.u16[1]
//! @endverbatim
static FOG_INLINE void p64ExpandPBWFromPBW1(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)

#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0 = ((x0 >> 16) & FOG_UINT64_C(0xFF)) * FOG_UINT64_C(0x0001000100010001);
#else
  dst0 = x0 & FOG_UINT64_C(0x0000000000FF0000);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 >> 16);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 32);
#endif // FOG_ARCH_HAS_FAST_MUL

#else

#if defined(FOG_ARCH_HAS_FAST_MUL)
  dst0.u32Lo = (x0.u32Lo >> 16) * 0x00010001U;
  dst0.u32Hi = dst0.u32Lo;
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, x0.u32Lo >> 16);
  dst0.u32Hi = dst0.u32Lo;
#endif // FOG_ARCH_HAS_FAST_MUL

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[1]
//! dst0.u16[1] = x0.u16[1]
//! dst1.u16[0] = x1.u16[1]
//! dst1.u16[1] = x1.u16[1]
//! @endverbatim
static FOG_INLINE void p64ExpandPBWFromPBW1_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64ExpandPBWFromPBW1(dst0, x0);
  p64ExpandPBWFromPBW1(dst1, x1);
}

static FOG_INLINE void p64ExpandPWWFromPWW0(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 & FOG_UINT64_C(0xFFFF);
#else
  dst0.u32Lo = x0.u32Lo & 0xFFFF;
  dst0.u32Hi = 0;
#endif // FOG_ARCH_NATIVE_P64

  p64ExtendPWWFromSWW(dst0, dst0);
}

static FOG_INLINE void p64ExpandPWWFromPWW1(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = (x0 >> 16) & FOG_UINT64_C(0xFFFF);
#else
  dst0.u32Lo = x0.u32Lo >> 16;
  dst0.u32Hi = 0;
#endif // FOG_ARCH_NATIVE_P64

  p64ExtendPWWFromSWW(dst0, dst0);
}

static FOG_INLINE void p64ExpandPWWFromPWW2(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = (x0 >> 32) & FOG_UINT64_C(0xFFFF);
#else
  dst0.u32Lo = x0.u32Hi;
  dst0.u32Hi = 0;
#endif // FOG_ARCH_NATIVE_P64

  p64ExtendPWWFromSWW(dst0, dst0);
}

static FOG_INLINE void p64ExpandPWWFromPWW3(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 >> 48;
#else
  dst0.u32Lo = x0.u32Hi >> 16;
  dst0.u32Hi = 0;
#endif // FOG_ARCH_NATIVE_P64

  p64ExtendPWWFromSWW(dst0, dst0);
}

// ============================================================================
// [Fog::Face - P64 - LShift / RShift]
// ============================================================================

static FOG_INLINE void p64LShift(
  __p64& dst0, const __p64& x0, uint32_t s0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 << s0;
#else
  dst0.u32Lo = x0.u32Lo << s0;
  dst0.u32Hi = x0.u32Hi << s0;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64LShift_2x(
  __p64& dst0, const __p64& x0, uint32_t s0,
  __p64& dst1, const __p64& x1, uint32_t s1)
{
  p64LShift(dst0, x0, s0);
  p64LShift(dst1, x1, s1);
}

static FOG_INLINE void p64LShiftTruncatePBW(
  __p64& dst0, const __p64& x0, uint32_t s0)
{
  FOG_ASSERT(s0 <= 8);

#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = (x0 << s0) & FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo << s0) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Hi << s0) & 0x00FF00FFU;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64LShiftTruncatePBW_2x(
  __p64& dst0, const __p64& x0, uint32_t s0,
  __p64& dst1, const __p64& x1, uint32_t s1)
{
  p64LShiftTruncatePBW(dst0, x0, s0);
  p64LShiftTruncatePBW(dst1, x1, s1);
}

static FOG_INLINE void p64LShiftBy1(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)

#if defined(FOG_ARCH_HAS_FAST_ADD)
  dst0 = x0 + x0;
#else
  dst0 = x0 << 1;
#endif // FOG_ARCH_HAS_FAST_ADD

#else

#if defined(FOG_ARCH_HAS_FAST_ADD)
  dst0.u32Lo = x0.u32Lo + x0.u32Lo;
  dst0.u32Hi = x0.u32Hi + x0.u32Hi;
#else
  dst0.u32Lo = x0.u32Lo << 1;
  dst0.u32Hi = x0.u32Hi << 1;
#endif // FOG_ARCH_HAS_FAST_ADD

#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64LShiftBy1_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64LShiftBy1(dst0, x0);
  p64LShiftBy1(dst1, x1);
}

static FOG_INLINE void p64LShiftBy1PBW0(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 + (x0 & FOG_UINT64_C(0x00000000000000FF));
#else
  dst0.u32Lo = x0.u32Lo + (x0.u32Lo & 0x000000FFU);
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64LShiftBy1PBW0_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64LShiftBy1PBW0(dst0, x0);
  p64LShiftBy1PBW0(dst1, x1);
}

static FOG_INLINE void p64LShiftBy1PBW1(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 + (x0 & FOG_UINT64_C(0x0000000000FF0000));
#else
  dst0.u32Lo = x0.u32Lo + (x0.u32Lo & 0x00FF0000U);
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64LShiftBy1PBW1_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64LShiftBy1PBW1(dst0, x0);
  p64LShiftBy1PBW1(dst1, x1);
}

static FOG_INLINE void p64LShiftBy1PBW2(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 + (x0 & FOG_UINT64_C(0x000000FF00000000));
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi + (x0.u32Hi & 0x000000FFU);
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64LShiftBy1PBW2_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64LShiftBy1PBW2(dst0, x0);
  p64LShiftBy1PBW2(dst1, x1);
}

static FOG_INLINE void p64LShiftBy1PBW3(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 + (x0 & FOG_UINT64_C(0x00FF000000000000));
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi + (x0.u32Hi & 0x00FF0000U);
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64LShiftBy1PBW3_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64LShiftBy1PBW3(dst0, x0);
  p64LShiftBy1PBW3(dst1, x1);
}

static FOG_INLINE void p64RShift(
  __p64& dst0, const __p64& x0, uint32_t s0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 >> s0;
#else
  dst0.u32Lo = x0.u32Lo >> s0;
  dst0.u32Hi = x0.u32Hi >> s0;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64RShift_2x(
  __p64& dst0, const __p64& x0, uint32_t s0,
  __p64& dst1, const __p64& x1, uint32_t s1)
{
  p64RShift(dst0, x0, s0);
  p64RShift(dst1, x1, s1);
}

static FOG_INLINE void p64RShiftTruncatePBW(
  __p64& dst0, const __p64& x0, uint32_t s0)
{
  FOG_ASSERT(s0 <= 8);

#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = (x0 >> s0) & FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo >> s0) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Hi >> s0) & 0x00FF00FFU;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64RShiftTruncatePBW_2x(
  __p64& dst0, const __p64& x0, uint32_t s0,
  __p64& dst1, const __p64& x1, uint32_t s1)
{
  p64RShiftTruncatePBW(dst0, x0, s0);
  p64RShiftTruncatePBW(dst1, x1, s1);
}

// ============================================================================
// [Fog::Face - P64 - Negate255/256]
// ============================================================================

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64Negate255SBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 ^ FOG_UINT64_C(0xFF);
#else
  dst0.u32Lo = x0.u32Lo ^ 0xFFU;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst1.u16[0] = 255 - x1.u16[0]
//! @endverbatim
static FOG_INLINE void p64Negate255SBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Negate255SBW(dst0, x0);
  p64Negate255SBW(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64Negate256SBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = FOG_UINT64_C(256) - x0;
#else
  dst0.u32Lo = 256U - x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst1.u16[0] = 256 - x1.u16[0]
//! @endverbatim
static FOG_INLINE void p64Negate256SBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Negate256SBW(dst0, x0);
  p64Negate256SBW(dst1, x1);
}

//! @verbatim
//! dst0.u8[0] = 255 - x0.u8[0]
//! dst0.u8[1] = 255 - x0.u8[1]
//! dst0.u8[2] = 255 - x0.u8[2]
//! dst0.u8[3] = 255 - x0.u8[3]
//! @endverbatim
static FOG_INLINE void p64Negate255PBB(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = ~x0;
#else
  dst0.u32Lo = ~x0.u32Lo;
  dst0.u32Hi = ~x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

//! @verbatim
//! dst0.u8[0] = 255 - x0.u8[0]
//! dst0.u8[1] = 255 - x0.u8[1]
//! dst0.u8[2] = 255 - x0.u8[2]
//! dst0.u8[3] = 255 - x0.u8[3]
//! dst1.u8[0] = 255 - x1.u8[0]
//! dst1.u8[1] = 255 - x1.u8[1]
//! dst1.u8[2] = 255 - x1.u8[2]
//! dst1.u8[3] = 255 - x1.u8[3]
//! @endverbatim
static FOG_INLINE void p64Negate255PBB_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Negate255PBB(dst0, x0);
  p64Negate255PBB(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst0.u16[1] = 255 - x0.u16[1]
//! @endverbatim
static FOG_INLINE void p64Negate255PBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 ^ FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = x0.u32Lo ^ 0x00FF00FFU;
  dst0.u32Hi = x0.u32Hi ^ 0x00FF00FFU;
#endif // FOG_ARCH_NATIVE_P64
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst0.u16[1] = 255 - x0.u16[1]
//! dst1.u16[0] = 255 - x1.u16[0]
//! dst1.u16[1] = 255 - x1.u16[1]
//! @endverbatim
static FOG_INLINE void p64Negate255PBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Negate255PBW(dst0, x0);
  p64Negate255PBW(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst0.u16[1] = 256 - x0.u16[1]
//! @endverbatim
static FOG_INLINE void p64Negate256PBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = FOG_UINT64_C(0x0100010001000100) - x0;
#else
  dst0.u32Lo = 0x01000100U - x0.u32Lo;
  dst0.u32Hi = 0x01000100U - x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst0.u16[1] = 256 - x0.u16[1]
//! dst1.u16[0] = 256 - x1.u16[0]
//! dst1.u16[1] = 256 - x1.u16[1]
//! @endverbatim
static FOG_INLINE void p64Negate256PBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Negate256PBW(dst0, x0);
  p64Negate256PBW(dst1, x1);
}

static FOG_INLINE void p64Negate255PBW0(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 ^ FOG_UINT64_C(0x00000000000000FF);
#else
  dst0.u32Lo = x0.u32Lo ^ 0x000000FFU;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Negate255PBW0_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Negate255PBW0(dst0, x0);
  p64Negate255PBW0(dst1, x1);
}

static FOG_INLINE void p64Negate255PBW1(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 ^ FOG_UINT64_C(0x0000000000FF0000);
#else
  dst0.u32Lo = x0.u32Lo ^ 0x00FF0000U;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Negate255PBW1_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Negate255PBW1(dst0, x0);
  p64Negate255PBW1(dst1, x1);
}

static FOG_INLINE void p64Negate255PBW2(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 ^ FOG_UINT64_C(0x000000FF00000000);
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi ^ 0x000000FFU;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Negate255PBW2_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Negate255PBW2(dst0, x0);
  p64Negate255PBW2(dst1, x1);
}

static FOG_INLINE void p64Negate255PBW3(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 ^ FOG_UINT64_C(0x00FF000000000000);
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi ^ 0x00FF0000U;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Negate255PBW3_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Negate255PBW3(dst0, x0);
  p64Negate255PBW3(dst1, x1);
}

// ============================================================================
// [Fog::Face - P64 - Min]
// ============================================================================

//! @brief Take smaller value from @a u0 and @a u and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(x0, u0)
//! @endverbatim
static FOG_INLINE void p64MinPBW_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  uint64_t x0_0 = x0 & FOG_UINT64_C(0x000000000000FFFF);
  uint64_t x0_1 = x0 & FOG_UINT64_C(0x00000000FFFF0000);
  uint64_t x0_2 = x0 & FOG_UINT64_C(0x0000FFFF00000000);
  uint64_t x0_3 = x0 & FOG_UINT64_C(0xFFFF000000000000);
  uint64_t t0 = u0;

  if (x0_0 > t0) x0_0 = t0;
  t0 <<= 16;
  if (x0_1 > t0) x0_1 = t0;
  t0 <<= 16;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_1);
  if (x0_2 > t0) x0_2 = t0;
  t0 <<= 16;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_2);
  if (x0_3 > t0) x0_3 = t0;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_3);

  dst0 = x0_0;

#else

  uint32_t x0_0 = x0.u32Lo & 0x0000FFFFU;
  uint32_t x0_1 = x0.u32Lo & 0xFFFF0000U;
  uint32_t x0_2 = x0.u32Hi & 0x0000FFFFU;
  uint32_t x0_3 = x0.u32Hi & 0xFFFF0000U;

  uint32_t t0_0 = u0.u32Lo;
  uint32_t t0_1 = u0.u32Lo << 16;

  if (x0_0 > t0_0) x0_0 = t0_0;
  if (x0_1 > t0_1) x0_1 = t0_1;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_1);

  if (x0_2 > t0_0) x0_2 = t0_0;
  if (x0_3 > t0_1) x0_3 = t0_1;
  x0_2 = _FOG_FACE_COMBINE_2(x0_2, x0_3);

  dst0.u32Lo = x0_0;
  dst0.u32Hi = x0_2;

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Take smaller value from @a x0/u0 and @a x1/u1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(x0, u0)
//! dst1 = min(x1, u1)
//! @endverbatim
static FOG_INLINE void p64MinPBW_SBW_2x(
  __p64& dst0, const __p64& x0, const __p64& u0,
  __p64& dst1, const __p64& x1, const __p64& u1)
{
  p64MinPBW_SBW(dst0, x0, u0);
  p64MinPBW_SBW(dst1, x1, u1);
}

//! @brief Take smaller value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(x0, y0)
//! @endverbatim
static FOG_INLINE void p64MinPBW(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  uint64_t t0_0;
  uint64_t t0_1;
  uint64_t t0_2;

  t0_0 = x0 & FOG_UINT64_C(0x000000000000FFFF);
  t0_1 = y0 & FOG_UINT64_C(0x000000000000FFFF);
  if (t0_0 > t0_1) t0_0 = t0_1;

  t0_1 = x0 & FOG_UINT64_C(0x00000000FFFF0000);
  t0_2 = y0 & FOG_UINT64_C(0x00000000FFFF0000);
  if (t0_1 > t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_1 = x0 & FOG_UINT64_C(0x0000FFFF00000000);
  t0_2 = y0 & FOG_UINT64_C(0x0000FFFF00000000);
  if (t0_1 > t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_1 = x0 & FOG_UINT64_C(0xFFFF000000000000);
  t0_2 = y0 & FOG_UINT64_C(0xFFFF000000000000);
  if (t0_1 > t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);
  dst0 = t0_0;

#else

  uint32_t t0_0;
  uint32_t t0_1;
  uint32_t t0_2;

  t0_0 = x0.u32Lo & 0x0000FFFFU;
  t0_1 = y0.u32Lo & 0x0000FFFFU;
  if (t0_0 > t0_1) t0_0 = t0_1;

  t0_1 = x0.u32Lo & 0xFFFF0000U;
  t0_2 = y0.u32Lo & 0xFFFF0000U;
  if (t0_1 > t0_2) t0_1 = t0_2;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_0 = x0.u32Hi & 0x0000FFFFU;
  t0_1 = y0.u32Hi & 0x0000FFFFU;
  if (t0_0 > t0_1) t0_0 = t0_1;

  t0_1 = x0.u32Hi & 0xFFFF0000U;
  t0_2 = y0.u32Hi & 0xFFFF0000U;
  if (t0_1 > t0_2) t0_1 = t0_2;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0_0, t0_1);

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Take smaller value from @a x0/y0 and @c x1/y1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(x0, y0)
//! dst1 = min(x1, y1)
//! @endverbatim
static FOG_INLINE void p64MinPBW_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  p64MinPBW(dst0, x0, y0);
  p64MinPBW(dst1, x1, y1);
}

// ============================================================================
// [Fog::Face - P64 - Max]
// ============================================================================

//! @brief Take larger value from @a x0/u0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(x0, u0)
//! @endverbatim
static FOG_INLINE void p64MaxPBW_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  uint64_t x0_0 = x0 & FOG_UINT64_C(0x000000000000FFFF);
  uint64_t x0_1 = x0 & FOG_UINT64_C(0x00000000FFFF0000);
  uint64_t x0_2 = x0 & FOG_UINT64_C(0x0000FFFF00000000);
  uint64_t x0_3 = x0 & FOG_UINT64_C(0xFFFF000000000000);
  uint64_t t0 = u0;

  if (x0_0 < t0) x0_0 = t0;
  t0 <<= 16;
  if (x0_1 < t0) x0_1 = t0;
  t0 <<= 16;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_1);
  if (x0_2 < t0) x0_2 = t0;
  t0 <<= 16;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_2);
  if (x0_3 < t0) x0_3 = t0;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_3);

  dst0 = x0_0;

#else

  uint32_t x0_0 = x0.u32Lo & 0x0000FFFFU;
  uint32_t x0_1 = x0.u32Lo & 0xFFFF0000U;
  uint32_t x0_2 = x0.u32Hi & 0x0000FFFFU;
  uint32_t x0_3 = x0.u32Hi & 0xFFFF0000U;

  uint32_t t0_0 = u0.u32Lo;
  uint32_t t0_1 = u0.u32Lo << 16;

  if (x0_0 < t0_0) x0_0 = t0_0;
  if (x0_1 < t0_1) x0_1 = t0_1;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_1);

  if (x0_2 < t0_0) x0_2 = t0_0;
  if (x0_3 < t0_1) x0_3 = t0_1;
  x0_2 = _FOG_FACE_COMBINE_2(x0_2, x0_3);

  dst0.u32Lo = x0_0;
  dst0.u32Hi = x0_2;

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Take larger value from @a x0/u0 and @a x1/u1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(x0, u0)
//! dst1 = max(x1, u1)
//! @endverbatim
static FOG_INLINE void p64MaxPBW_SBW_2x(
  __p64& dst0, const __p64& x0, const __p64& u0,
  __p64& dst1, const __p64& x1, const __p64& u1)
{
  p64MaxPBW_SBW(dst0, x0, u0);
  p64MaxPBW_SBW(dst1, x1, u1);
}

//! @brief Take larger value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(x0, y0)
//! @endverbatim
static FOG_INLINE void p64MaxPBW(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  uint64_t t0_0;
  uint64_t t0_1;
  uint64_t t0_2;

  t0_0 = x0 & FOG_UINT64_C(0x000000000000FFFF);
  t0_1 = y0 & FOG_UINT64_C(0x000000000000FFFF);
  if (t0_0 < t0_1) t0_0 = t0_1;

  t0_1 = x0 & FOG_UINT64_C(0x00000000FFFF0000);
  t0_2 = y0 & FOG_UINT64_C(0x00000000FFFF0000);
  if (t0_1 < t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_1 = x0 & FOG_UINT64_C(0x0000FFFF00000000);
  t0_2 = y0 & FOG_UINT64_C(0x0000FFFF00000000);
  if (t0_1 < t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_1 = x0 & FOG_UINT64_C(0xFFFF000000000000);
  t0_2 = y0 & FOG_UINT64_C(0xFFFF000000000000);
  if (t0_1 < t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);
  dst0 = t0_0;

#else

  uint32_t t0_0;
  uint32_t t0_1;
  uint32_t t0_2;

  t0_0 = x0.u32Lo & 0x0000FFFFU;
  t0_1 = y0.u32Lo & 0x0000FFFFU;
  if (t0_0 < t0_1) t0_0 = t0_1;

  t0_1 = x0.u32Lo & 0xFFFF0000U;
  t0_2 = y0.u32Lo & 0xFFFF0000U;
  if (t0_1 < t0_2) t0_1 = t0_2;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_0 = x0.u32Hi & 0x0000FFFFU;
  t0_1 = y0.u32Hi & 0x0000FFFFU;
  if (t0_0 < t0_1) t0_0 = t0_1;

  t0_1 = x0.u32Hi & 0xFFFF0000U;
  t0_2 = y0.u32Hi & 0xFFFF0000U;
  if (t0_1 < t0_2) t0_1 = t0_2;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0_0, t0_1);

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Take larger value from @a x0/y0 and @a x1/y1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(x0, y0)
//! dst1 = max(x1, y1)
//! @endverbatim
static FOG_INLINE void p64MaxPBW_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  p64MaxPBW(dst0, x0, y0);
  p64MaxPBW(dst1, x1, y1);
}

// ============================================================================
// [Fog::Face - P64 - Saturate]
// ============================================================================

static FOG_INLINE void p64Saturate255SBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)

#if defined(FOG_ARCH_HAS_CMOV)
  dst0 = x0;
  if (dst0 > 0xFF) dst0 = 0xFF;
#else
  dst0 = (x0 | (FOG_UINT64_C(0x0100) - (x0 >> 8))) & 0xFFU;
#endif

#else

#if defined(FOG_ARCH_HAS_CMOV)
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;
  if (dst0.u32Lo > 0xFF) dst0.u32Lo = 0xFF;
#else
  dst0.u32Lo = (x0.u32Lo | (0x0100U - (x0.u32Lo >> 8))) & 0xFFU;
  dst0.u32Hi = x0.u32Hi;
#endif

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Saturate @a dst0. Can be used after two BYTEs were added and
//! resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! @endverbatim
static FOG_INLINE void p64Saturate255PBW(
  __p64& dst0, const __p64& x0)
{
  // NOTE: The following code will work (and is more precise) too:
  //
  // dst0  = x0 | (FOG_UINT64_C(0x0100010001000100) - ((x0 >> 8) & FOG_UINT64_C(0x0001000100010001)));
  // dst0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  //
  // The reason why the 0x00FF00FF is also used to mask (x0 >> 8) is that the
  // mask can be stored in register so it will be reused by another computation.

#if defined(FOG_ARCH_NATIVE_P64)

  dst0  = x0 | (FOG_UINT64_C(0x0100010001000100) - ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)));
  dst0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

#else

  dst0.u32Lo = (x0.u32Lo | (0x01000100U - ((x0.u32Lo >> 8) & 0x00FF00FFU))) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Hi | (0x01000100U - ((x0.u32Hi >> 8) & 0x00FF00FFU))) & 0x00FF00FFU;

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Saturate @a dst0/dst1. Can be used after two BYTEs were added
//! and resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! dst1 = min(dst1, 255)
//! @endverbatim
static FOG_INLINE void p64Saturate255PBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Saturate255PBW(dst0, x0);
  p64Saturate255PBW(dst1, x1);
}

//! @brief Saturate @a dst0. Can be used after two BYTEs were added and
//! resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 511)
//! @endverbatim
static FOG_INLINE void p64Saturate511PBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  dst0  = x0 | (FOG_UINT64_C(0x0200020002000200) - ((x0 >> 9) & FOG_UINT64_C(0x0001000100010001)));
  dst0 &= FOG_UINT64_C(0x01FF01FF01FF01FF);

#else

  dst0.u32Lo = (x0.u32Lo | (0x02000200U - ((x0.u32Lo >> 9) & 0x00010001U))) & 0x01FF01FFU;
  dst0.u32Hi = (x0.u32Hi | (0x02000200U - ((x0.u32Hi >> 9) & 0x00010001U))) & 0x01FF01FFU;

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Saturate @a dst0/dst1. Can be used after two BYTEs were added
//! and resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 511)
//! dst1 = min(dst1, 511)
//! @endverbatim
static FOG_INLINE void p64Saturate511PBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1)
{
  p64Saturate511PBW(dst0, x0);
  p64Saturate511PBW(dst1, x1);
}

// ============================================================================
// [Fog::Face - P64 - Add / Addus]
// ============================================================================

static FOG_INLINE void p64AddPBW_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
  __p64 t0;
  p64ExtendPBWFromSBW(t0, u0);
  p64Add(dst0, x0, t0);
}

static FOG_INLINE void p64AddPBW_SBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1, const __p64& u01)
{
  __p64 t01;
  p64ExtendPBWFromSBW(t01, u01);
  p64Add_2x(dst0, x0, t01, dst1, x1, t01);
}

static FOG_INLINE void p64AddPBW_SBW_2x(
  __p64& dst0, const __p64& x0, const __p64& u0,
  __p64& dst1, const __p64& x1, const __p64& u1)
{
  __p64 t0, t1;
  p64ExtendPBWFromSBW(t0, u0);
  p64ExtendPBWFromSBW(t1, u1);
  p64Add_2x(dst0, x0, t0, dst1, x1, t1);
}

static FOG_INLINE void p64Addus255SBW(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  p64Add(dst0, x0, y0);
  p64Saturate255SBW(dst0, dst0);
}

static FOG_INLINE void p64Addus255PBB(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  uint64_t t0 = (x0     ) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t t1 = (x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 += (y0     ) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  t1 += (y0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 |= FOG_UINT64_C(0x0100010001000100) - ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  t1 |= FOG_UINT64_C(0x0100010001000100) - ((t1 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));

  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  t1 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1 << 8);

#else

  uint32_t t0;
  uint32_t t1;

  t0 = ((x0.u32Lo     ) & 0x00FF00FFU) + ((y0.u32Lo     ) & 0x00FF00FFU);
  t1 = ((x0.u32Lo >> 8) & 0x00FF00FFU) + ((y0.u32Lo >> 8) & 0x00FF00FFU);

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1 << 8);

  t0 = ((x0.u32Hi     ) & 0x00FF00FFU) + ((y0.u32Hi     ) & 0x00FF00FFU);
  t1 = ((x0.u32Hi >> 8) & 0x00FF00FFU) + ((y0.u32Hi >> 8) & 0x00FF00FFU);

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1 << 8);

#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Addus255PBW(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  p64Add(dst0, x0, y0);
  p64Saturate255PBW(dst0, dst0);
}

static FOG_INLINE void p64Addus255PBW_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  p64Add_2x(dst0, x0, y0, dst1, x1, y1);
  p64Saturate255PBW_2x(dst0, dst0, dst1, dst1);
}

static FOG_INLINE void p64Addus255PBW_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
  __p64 t0;
  p64ExtendPBWFromSBW(t0, u0);
  p64Addus255PBW(dst0, x0, t0);
}

static FOG_INLINE void p64Addus255PBW_SBW_2x(
  __p64& dst0, const __p64& x0, const __p64& u0,
  __p64& dst1, const __p64& x1, const __p64& u1)
{
  __p64 t0, t1;
  p64ExtendPBWFromSBW(t0, u0);
  p64ExtendPBWFromSBW(t1, u1);
  p64Addus255PBW_2x(dst0, x0, t0, dst1, x1, t1);
}

// ============================================================================
// [Fog::Face - P64 - Sub / Subus]
// ============================================================================

static FOG_INLINE void p64SubPBW_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
  __p64 t0;
  p64ExtendPBWFromSBW(t0, u0);
  p64Sub(dst0, x0, t0);
}

static FOG_INLINE void p64SubPBW_SBW_2x(
  __p64& dst0, const __p64& x0,
  __p64& dst1, const __p64& x1, const __p64& u01)
{
  __p64 t01;
  p64ExtendPBWFromSBW(t01, u01);
  p64Sub_2x(dst0, x0, t01, dst1, x1, t01);
}

static FOG_INLINE void p64SubPBW_SBW_2x(
  __p64& dst0, const __p64& x0, const __p64& u0,
  __p64& dst1, const __p64& x1, const __p64& u1)
{
  __p64 t0, t1;
  p64ExtendPBWFromSBW(t0, u0);
  p64ExtendPBWFromSBW(t1, u1);
  p64Sub_2x(dst0, x0, t0, dst1, x1, t1);
}

static FOG_INLINE void p64Subus255PBW(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
  p64Negate255PBW(dst0, x0);
  p64Add(dst0, dst0, y0);
  p64Saturate255PBW(dst0, dst0);
  p64Negate255PBW(dst0, dst0);
}

static FOG_INLINE void p64Subus255PBW_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  p64Negate255PBW_2x(dst0, x0, dst1, x1);
  p64Add_2x(dst0, dst0, y0, dst1, dst1, y1);
  p64Saturate255PBW_2x(dst0, dst0, dst1, dst1);
  p64Negate255PBW_2x(dst0, dst0, dst1, dst1);
}

static FOG_INLINE void p64Subus255SBW(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  dst0 = x0 - y0;

#if defined(FOG_ARCH_HAS_CMOV)
  if ((int64_t)dst0 < 0) dst0 = 0;
#else
  dst0 &= (dst0 >> 56) ^ FOG_UINT64_C(0xFF);
#endif // FOG_ARCH_HAS_CMOV

#else

  dst0.u32Lo = x0.u32Lo - y0.u32Lo;
  dst0.u32Hi = x0.u32Hi;

#if defined(FOG_ARCH_HAS_CMOV)
  if ((int32_t)dst0.u32Lo < 0) dst0.u32Lo = 0;
#else
  dst0.u32Lo &= (dst0.u32Lo >> 24) ^ 0xFFU;
#endif // FOG_ARCH_HAS_CMOV

#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64Subus255PBW_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
  __p64 t0;
  p64ExtendPBWFromSBW(t0, u0);
  p64Subus255PBW(dst0, x0, t0);
}

static FOG_INLINE void p64Subus255PBW_SBW_2x(
  __p64& dst0, const __p64& x0, const __p64& u0,
  __p64& dst1, const __p64& x1, const __p64& u1)
{
  __p64 t0, t1;
  p64ExtendPBWFromSBW(t0, u0);
  p64ExtendPBWFromSBW(t1, u1);
  p64Subus255PBW_2x(dst0, x0, t0, dst1, x1, t1);
}
















// ============================================================================
// [Fog::Face - P64 - AddSub / AddSubus]
// ============================================================================

//! @brief Scalar add and subtract.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0] - z0.u32[0]
//! @endverbatim
static FOG_INLINE void p64AddSub(
  __p64& dst0, const __p64& x0, const __p64& y0, const __p64& z0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 + y0 - z0;
#else
  dst0.u32Lo = x0.u32Lo + y0.u32Lo - z0.u32Lo;
  dst0.u32Hi = x0.u32Hi + y0.u32Hi - z0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

static FOG_INLINE void p64AddSub_2x(
  __p64& dst0, const __p64& x0, const __p64& y0, const __p64& z0,
  __p64& dst1, const __p64& x1, const __p64& y1, const __p64& z1)
{
  p64AddSub(dst0, x0, y0, z0);
  p64AddSub(dst1, x1, y1, z1);
}

//! @verbatim
//! dst0 = saturate255(x0 + y0 - z0)
//! @endverbatim
static FOG_INLINE void p64AddSubus255PBW(
  __p64& dst0, const __p64& x0, const __p64& y0, const __p64& z0)
{
  __p64 t0;

  p64Add(t0, x0, y0);
  p64Xor(t0, t0, p64FromU64(FOG_UINT64_C(0x01FF01FF01FF01FF)));
  p64Add(t0, t0, z0);
  p64Saturate511PBW(t0, t0);
  p64Xor(t0, t0, p64FromU64(FOG_UINT64_C(0x01FF01FF01FF01FF)));
  p64Saturate255PBW(t0, t0);
}

//! @verbatim
//! dst0 = saturate255(x0 + y0 - z0)
//! dst1 = saturate255(x1 + y1 - z1)
//! @endverbatim
static FOG_INLINE void p64AddSubus255PBW_2x(
  __p64& dst0, const __p64& x0, const __p64& y0, const __p64& z0,
  __p64& dst1, const __p64& x1, const __p64& y1, const __p64& z1)
{
  p64AddSubus255PBW(dst0, x0, y0, z0);
  p64AddSubus255PBW(dst1, x1, y1, z1);
}

// ============================================================================
// [Fog::Face - P64 - Div]
// ============================================================================

//! @brief Scalar divide by 255.
//!
//! @verbatim
//! dst0 = (x0 / 255).
//! @endverbatim
static FOG_INLINE void p64Div255SBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = ((x0 << 8U) + x0 + 256U) >> 16U;
#else
  dst0.u32Lo = ((x0.u32Lo << 8U) + x0.u32Lo + 256U) >> 16U;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Scalar Divide by 256.
//!
//! @verbatim
//! dst0 = (x0 / 256).
//! @endverbatim
static FOG_INLINE void p64Div256SBW(
  __p64& dst0, const __p64& x0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 >> 8;
#else
  dst0.u32Lo = x0.u32Lo >> 8;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

// ============================================================================
// [Fog::Face - P64 - MulDiv]
// ============================================================================

//! @brief Scalar Multiply and divide by 255.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] * u0.u32[0]) / 255
//! dst0.u32[1] = (x0.u32[1])
//! @endverbatim
static FOG_INLINE void p64MulDiv255SBW(
  __p64 &dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 * u0;
  dst0 = (dst0 + (dst0 >> 8) + 0x80U) >> 8;
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;

  dst0.u32Lo = dst0.u32Lo * u0.u32Lo;
  dst0.u32Lo = (dst0.u32Lo + (dst0.u32Lo >> 8) + 0x80U) >> 8;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Scalar Multiply and divide by 256.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] * u0.u32[0]) / 256
//! dst0.u32[1] = (x0.u32[1])
//! @endverbatim
static FOG_INLINE void p64MulDiv256SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = (x0 * u0) >> 8;
#else
  dst0.u32Lo = (x0.u32Lo * u0.u32Lo) >> 8;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.u64[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.u64[0]) / 255
//! dst0.u8[3] = (x0.u8[3] * u0.u64[0]) / 255
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 255
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 255
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 255
//! dst0.u8[7] = (x0.u8[7] * u0.u64[0]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  uint64_t t0 = ((x0     ) & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;
  uint64_t t1 = ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;

  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  t1 = (t1 + ((t1 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080));

  t0 = t0 & FOG_UINT64_C(0x00FF00FF00FF00FF);
  t1 = t1 & FOG_UINT64_C(0xFF00FF00FF00FF00);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);

#else

  uint32_t t0;
  uint32_t t1;

  t0 = ((x0.u32Lo     ) & 0x00FF00FFU) * u0.u32Lo;
  t1 = ((x0.u32Lo >> 8) & 0x00FF00FFU) * u0.u32Lo;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FF) + 0x00800080)     ) & 0xFF00FF00;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1);

  t0 = ((x0.u32Hi     ) & 0x00FF00FFU) * u0.u32Lo;
  t1 = ((x0.u32Hi >> 8) & 0x00FF00FFU) * u0.u32Lo;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FF) + 0x00800080)     ) & 0xFF00FF00;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1);

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.u64[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.u64[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * u0.u64[0]) / 256
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 256
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 256
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 256
//! dst0.u8[7] = (x0.u8[7] * u0.u64[0]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBB_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  uint64_t t0 = ((x0     ) & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;
  uint64_t t1 = ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;

  t0 = (t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  t1 = (t1     ) & FOG_UINT64_C(0xFF00FF00FF00FF00);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);

#else

  uint32_t t0;
  uint32_t t1;

  t0 = ((x0.u32Lo     ) & 0x00FF00FFU) * u0.u32Lo;
  t1 = ((x0.u32Lo >> 8) & 0x00FF00FFU) * u0.u32Lo;

  t0 = (t0 >> 8) & 0x00FF00FF;
  t1 = (t1     ) & 0xFF00FF00;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1);

  t0 = ((x0.u32Hi     ) & 0x00FF00FFU) * u0.u32Lo;
  t1 = ((x0.u32Hi >> 8) & 0x00FF00FFU) * u0.u32Lo;

  t0 = (t0 >> 8) & 0x00FF00FF;
  t1 = (t1     ) & 0xFF00FF00;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1);

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.u64[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.u64[0]) / 255
//! dst0.u8[3] = 0x00
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 255
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 255
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 255
//! dst0.u8[7] = 0x00
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB_SBW_Z654Z210(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  uint64_t t0 = (x0 & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;
  uint64_t t1 = (x0 & FOG_UINT64_C(0x0000FF000000FF00)) * u0;

  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080));
  t1 = (t1 + ((t1 >> 8) & FOG_UINT64_C(0x0000FF000000FF00)) + FOG_UINT64_C(0x0000800000008000));

  t0 &= FOG_UINT64_C(0xFF00FF00FF00FF00);
  t1 &= FOG_UINT64_C(0x00FF000000FF0000);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

#else

  uint32_t t0;
  uint32_t t1;

  t0 = (x0.u32Lo & 0x00FF00FFU) * u0.u32Lo;
  t1 = (x0.u32Lo & 0x0000FF00U) * u0.u32Lo;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x0000FF00U) + 0x00008000U);

  t0 &= 0xFF00FF00;
  t1 &= 0x00FF0000;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

  t0 = (x0.u32Hi & 0x00FF00FFU) * u0.u32Lo;
  t1 = (x0.u32Hi & 0x0000FF00U) * u0.u32Lo;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x0000FF00U) + 0x00008000U);

  t0 &= 0xFF00FF00;
  t1 &= 0x00FF0000;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 256
//! dst0.u8[1] = (x0.u8[2] * u0.u64[0]) / 256
//! dst0.u8[2] = (x0.u8[3] * u0.u64[0]) / 256
//! dst0.u8[3] = 0x00
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 256
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 256
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 256
//! dst0.u8[7] = 0x00
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBB_SBW_Z654Z210(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)

  uint64_t t0 = (x0 & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;
  uint64_t t1 = (x0 & FOG_UINT64_C(0x0000FF000000FF00)) * u0;

  t0 = t0 & FOG_UINT64_C(0xFF00FF00FF00FF00);
  t1 = t1 & FOG_UINT64_C(0x00FF000000FF0000);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

#else

  uint32_t t0;
  uint32_t t1;

  t0 = (x0.u32Lo & 0x00FF00FFU) * u0.u32Lo;
  t1 = (x0.u32Lo & 0x0000FF00U) * u0.u32Lo;

  t0 &= 0xFF00FF00;
  t1 &= 0x00FF0000;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

  t0 = (x0.u32Hi & 0x00FF00FFU) * u0.u32Lo;
  t1 = (x0.u32Hi & 0x0000FF00U) * u0.u32Lo;

  t0 &= 0xFF00FF00;
  t1 &= 0x00FF0000;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.u64[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.u64[0]) / 255
//! dst0.u8[3] = 0xFF
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 255
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 255
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 255
//! dst0.u8[7] = 0xFF
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB_SBW_F654F210(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
  p64MulDiv255PBB_SBW_F654F210(dst0, x0, u0);

#if defined(FOG_ARCH_NATIVE_P64)
  dst0 |= FOG_UINT64_C(0xFF000000FF000000);
#else
  dst0.u32Lo |= 0xFF000000U;
  dst0.u32Hi |= 0xFF000000U;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.u64[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.u64[0]) / 256
//! dst0.u8[3] = 0xFF
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 256
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 256
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 256
//! dst0.u8[7] = 0xFF
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBB_SBW_F654F210(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
  p64MulDiv256PBB_SBW_F654F210(dst0, x0, u0);

#if defined(FOG_ARCH_NATIVE_P64)
  dst0 |= FOG_UINT64_C(0xFF000000FF000000);
#else
  dst0.u32Lo |= 0xFF000000U;
  dst0.u32Hi |= 0xFF000000U;
#endif // FOG_ARCH_NATIVE_P64
}

/*

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0.u8[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0.u8[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0.u8[2]) / 255
//! dst0.u8[3] = (x0.u8[3] * y0.u8[3]) / 255
//! dst0.u8[4] = (x0.u8[4] * y0.u8[4]) / 255
//! dst0.u8[5] = (x0.u8[5] * y0.u8[5]) / 255
//! dst0.u8[6] = (x0.u8[6] * y0.u8[6]) / 255
//! dst0.u8[7] = (x0.u8[7] * y0.u8[7]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0.u8[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0.u8[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0.u8[2]) / 255
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB_Z654Z210(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
}

*/

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u64[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * u0.u64[0]) / 255
//! dst0.u16[2] = (x0.u16[2] * u0.u64[0]) / 255
//! dst0.u16[3] = (x0.u16[3] * u0.u64[0]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBW_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = x0 * u0;
  dst0 = (dst0 + ((dst0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  dst0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = x0.u32Lo * u0.u32Lo;
  dst0.u32Hi = x0.u32Hi * u0.u32Lo;

  dst0.u32Lo = (dst0.u32Lo + ((dst0.u32Lo >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8;
  dst0.u32Hi = (dst0.u32Hi + ((dst0.u32Hi >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8;

  dst0.u32Lo &= 0x00FF00FFU;
  dst0.u32Hi &= 0x00FF00FFU;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Packed<-Scalar multiply and divide by 255 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u64[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * u0.u64[0]) / 255
//! dst0.u16[2] = (x0.u16[2] * u0.u64[0]) / 255
//! dst0.u16[3] = (x0.u16[3] * u0.u64[0]) / 255
//! dst1.u16[0] = (x1.u16[0] * u1.u64[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * u1.u64[0]) / 255
//! dst1.u16[2] = (x1.u16[2] * u1.u64[0]) / 255
//! dst1.u16[3] = (x1.u16[3] * u1.u64[0]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBW_SBW_2x(
  __p64& dst0, const __p64& x0, const __p64& u0,
  __p64& dst1, const __p64& x1, const __p64& u1)
{
  p64MulDiv255PBW_SBW(dst0, x0, u0);
  p64MulDiv255PBW_SBW(dst1, x1, u1);
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u64[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * u0.u64[0]) / 256
//! dst0.u16[2] = (x0.u16[2] * u0.u64[0]) / 256
//! dst0.u16[3] = (x0.u16[3] * u0.u64[0]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBW_SBW(
  __p64& dst0, const __p64& x0, const __p64& u0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst0 = (x0 * u0) >> 8;
  dst0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo * u0.u32Lo) >> 8;
  dst0.u32Hi = (x0.u32Hi * u0.u32Lo) >> 8;

  dst0.u32Lo &= 0x00FF00FFU;
  dst0.u32Hi &= 0x00FF00FFU;
#endif // FOG_ARCH_NATIVE_P64
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u64[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * u0.u64[0]) / 256
//! dst0.u16[2] = (x0.u16[2] * u0.u64[0]) / 256
//! dst0.u16[3] = (x0.u16[3] * u0.u64[0]) / 256
//! dst1.u16[0] = (x1.u16[0] * u1.u64[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * u1.u64[0]) / 256
//! dst1.u16[2] = (x1.u16[2] * u1.u64[0]) / 256
//! dst1.u16[3] = (x1.u16[3] * u1.u64[0]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBW_SBW_2x(
  __p64& dst0, const __p64& x0, const __p64& u0,
  __p64& dst1, const __p64& x1, const __p64& u1)
{
  p64MulDiv256PBW_SBW(dst0, x0, u0);
  p64MulDiv256PBW_SBW(dst1, x1, u1);
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 255
//! dst0.u16[2] = (x0.u16[2] * y0.u16[2]) / 255
//! dst0.u16[3] = (x0.u16[3] * y0.u16[3]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBW(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  uint64_t ym = y0;
  uint64_t t0;
  uint64_t t1;

  t0 = (x0 & FOG_UINT64_C(0x00000000000000FF)) * (ym & 0xFFU); ym >>= 16;
  t1 = (x0 & FOG_UINT64_C(0x0000000000FF0000)) * (ym & 0xFFU); ym >>= 16;
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t1 = (x0 & FOG_UINT64_C(0x000000FF00000000)) * (ym & 0xFFU); ym >>= 16;
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t1 = (x0 & FOG_UINT64_C(0x00FF000000000000)) * (ym & 0xFFU);
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t0  = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = t0;
#else
  uint32_t t0 = _FOG_FACE_COMBINE_2( (x0.u32Lo & 0x000000FFU) * (y0.u32Lo & 0xFFU), (x0.u32Lo & 0x00FF0000U) * (y0.u32Lo >> 16) );
  uint32_t t1 = _FOG_FACE_COMBINE_2( (x0.u32Hi & 0x000000FFU) * (y0.u32Hi & 0xFFU), (x0.u32Hi & 0x00FF0000U) * (y0.u32Hi >> 16) );

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8;
  t1 = (t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  dst0.u32Lo = t0;
  dst0.u32Hi = t1;
#endif // FOG_ARCH_NATIVE_P64
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 255
//! dst1.u16[0] = (x1.u16[0] * y1.u16[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * y1.u16[1]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBW_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  p64MulDiv255PBW(dst0, x0, y0);
  p64MulDiv255PBW(dst1, x1, y1);
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBW(
  __p64& dst0, const __p64& x0, const __p64& y0)
{
#if defined(FOG_ARCH_NATIVE_P64)
  uint64_t ym = y0;
  uint64_t t0;
  uint64_t t1;

  t0 = (x0 & FOG_UINT64_C(0x00000000000000FF)) * (ym & 0xFFU); ym >>= 16;
  t1 = (x0 & FOG_UINT64_C(0x0000000000FF0000)) * (ym & 0xFFU); ym >>= 16;
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t1 = (x0 & FOG_UINT64_C(0x000000FF00000000)) * (ym & 0xFFU); ym >>= 16;
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t1 = (x0 & FOG_UINT64_C(0x00FF000000000000)) * (ym & 0xFFU);
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t0 >>= 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = t0;
#else
  uint32_t t0 = _FOG_FACE_COMBINE_2( (x0.u32Lo & 0x000000FFU) * (y0.u32Lo & 0xFFU), (x0.u32Lo & 0x00FF0000U) * (y0.u32Lo >> 16) );
  uint32_t t1 = _FOG_FACE_COMBINE_2( (x0.u32Hi & 0x000000FFU) * (y0.u32Hi & 0xFFU), (x0.u32Hi & 0x00FF0000U) * (y0.u32Hi >> 16) );

  t0 >>= 8;
  t1 >>= 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  dst0.u32Lo = t0;
  dst0.u32Hi = t1;
#endif // FOG_ARCH_NATIVE_P64
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 256
//! dst1.u16[0] = (x1.u16[0] * y1.u16[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * y1.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBW_2x(
  __p64& dst0, const __p64& x0, const __p64& y0,
  __p64& dst1, const __p64& x1, const __p64& y1)
{
  p64MulDiv256PBW(dst0, x0, y0);
  p64MulDiv256PBW(dst1, x1, y1);
}

// ============================================================================
// [Fog::Face - P64 - FloorPow2]
// ============================================================================

static FOG_INLINE void p64FloorPow2(__p64& dst, const __p64& src)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst  = src;
  dst |= dst >> 1;
  dst |= dst >> 2;
  dst |= dst >> 4;
  dst |= dst >> 8;
  dst |= dst >> 16;
  dst |= dst >> 32;
  dst  = dst - (dst >> 1);
#else
  uint64_t n = src.u64;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n |= n >> 32;
  dst.u64 = n - (n >> 1);
#endif // FOG_ARCH_NATIVE_P64
}

// ============================================================================
// [Fog::Face - P64 - CeilPow2]
// ============================================================================

static FOG_INLINE void p64CeilPow2(__p64& dst, const __p64& src)
{
#if defined(FOG_ARCH_NATIVE_P64)
  dst  = src - 1;
  dst |= dst >> 1;
  dst |= dst >> 2;
  dst |= dst >> 4;
  dst |= dst >> 8;
  dst |= dst >> 16;
  dst |= dst >> 32;
  dst  = dst + 1;
#else
  uint64_t n = src.u64;
  n  = n - 1;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n |= n >> 32;
  dst.u64 = n + 1;
#endif // FOG_ARCH_NATIVE_P64
}

//! @}

} // Face namespace
} // Fog namespace

#undef _U64

// [Guard]
#endif // _FOG_CORE_FACE_FACEC_H
