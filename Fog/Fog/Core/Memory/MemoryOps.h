// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMORYOPS_H
#define _FOG_CORE_MEMORY_MEMORYOPS_H

// [Dependencies]
#include <Fog/Core/Memory/Memory.h>

namespace Fog {
namespace Memory {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::Memory - Read / Write]
// ============================================================================

// TODO: Remove

static FOG_INLINE uint8_t  read_1 (const void* mem) { return ((const uint8_t  *)mem)[0]; }
static FOG_INLINE uint16_t read_2a(const void* mem) { return ((const uint16_t *)mem)[0]; }
static FOG_INLINE uint16_t read_2u(const void* mem) { return ((const uint16_t *)mem)[0]; }
static FOG_INLINE uint32_t read_4a(const void* mem) { return ((const uint32_t *)mem)[0]; }
static FOG_INLINE uint32_t read_4u(const void* mem) { return ((const uint32_t *)mem)[0]; }
static FOG_INLINE uint64_t read_8a(const void* mem) { return ((const uint64_t *)mem)[0]; }
static FOG_INLINE uint64_t read_8u(const void* mem) { return ((const uint64_t *)mem)[0]; }

static FOG_INLINE void write_1 (void* mem, uint8_t  x) { ((uint8_t  *)mem)[0] = x; }
static FOG_INLINE void write_2a(void* mem, uint16_t x) { ((uint16_t *)mem)[0] = x; }
static FOG_INLINE void write_2u(void* mem, uint16_t x) { ((uint16_t *)mem)[0] = x; }
static FOG_INLINE void write_4a(void* mem, uint32_t x) { ((uint32_t *)mem)[0] = x; }
static FOG_INLINE void write_4u(void* mem, uint32_t x) { ((uint32_t *)mem)[0] = x; }
static FOG_INLINE void write_8a(void* mem, uint64_t x) { ((uint64_t *)mem)[0] = x; }
static FOG_INLINE void write_8u(void* mem, uint64_t x) { ((uint64_t *)mem)[0] = x; }

// ============================================================================
// [Fog::Memory - Copy (Exact)]
// ============================================================================

static FOG_INLINE void copy_1(void* dst, const void* src)
{
  ((uint8_t *)dst)[0] = ((const uint8_t *)src)[0];
}

static FOG_INLINE void copy_2(void* dst, const void* src)
{
  ((uint16_t *)dst)[0] = ((const uint16_t *)src)[0];
}

static FOG_INLINE void copy_3(void* dst, const void* src)
{
  ((uint16_t *)dst)[0] = ((const uint16_t *)src)[0];
  ((uint8_t  *)dst)[2] = ((const uint8_t  *)src)[2];
}

static FOG_INLINE void copy_4(void* dst, const void* src)
{
  ((uint32_t *)dst)[0] = ((const uint32_t *)src)[0];
}

static FOG_INLINE void copy_8(void* dst, const void* src)
{
#if FOG_ARCH_BITS >= 64
  ((uint64_t *)dst)[0] = ((const uint64_t *)src)[0];
#else
  uint32_t r0 = ((const uint32_t *)src)[0];
  uint32_t r1 = ((const uint32_t *)src)[1];

  ((uint32_t *)dst)[0] = r0;
  ((uint32_t *)dst)[1] = r1;
#endif
}

static FOG_INLINE void copy_12(void* dst, const void* src)
{
  copy_8(dst, src);
  copy_4((uint8_t*)dst + 8, (uint8_t*)src + 8);
}

static FOG_INLINE void copy_16(void* dst, const void* src)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i xmm0 = _mm_loadu_si128((__m128i *)(src) + 0);
  _mm_storeu_si128((__m128i *)(dst), xmm0);
#else
  copy_8((uint8_t*)(dst) + 0, (const uint8_t*)(src) + 0);
  copy_8((uint8_t*)(dst) + 8, (const uint8_t*)(src) + 8);
#endif
}

static FOG_INLINE void copy_24(void* dst, const void* src)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i xmm0 = _mm_loadu_si128((__m128i *)(src) + 0);
  __m128i xmm1 = _mm_loadl_epi64((__m128i *)(src) + 1);

  _mm_storeu_si128((__m128i*)(dst) + 0, xmm0);
  _mm_storel_epi64((__m128i*)(dst) + 1, xmm1);
#else
  copy_16((uint8_t*)(dst) +  0, (const uint8_t*)(src) +  0);
  copy_8 ((uint8_t*)(dst) + 16, (const uint8_t*)(src) + 16);
#endif
}

static FOG_INLINE void copy_32(void* dst, const void* src)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i xmm0 = _mm_loadu_si128((__m128i *)(src) + 0);
  __m128i xmm1 = _mm_loadu_si128((__m128i *)(src) + 1);

  _mm_storeu_si128((__m128i *)(dst) + 0, xmm0);
  _mm_storeu_si128((__m128i *)(dst) + 1, xmm1);
#else
  copy_16((uint8_t*)(dst) +  0, (const uint8_t*)(src) +  0);
  copy_16((uint8_t*)(dst) + 16, (const uint8_t*)(src) + 16);
#endif
}

static FOG_INLINE void copy_64(void* dst, const void* src)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i xmm0 = _mm_loadu_si128((__m128i *)(src) + 0);
  __m128i xmm1 = _mm_loadu_si128((__m128i *)(src) + 1);
  __m128i xmm2 = _mm_loadu_si128((__m128i *)(src) + 2);
  __m128i xmm3 = _mm_loadu_si128((__m128i *)(src) + 3);

  _mm_storeu_si128((__m128i *)(dst) + 0, xmm0);
  _mm_storeu_si128((__m128i *)(dst) + 1, xmm1);
  _mm_storeu_si128((__m128i *)(dst) + 2, xmm2);
  _mm_storeu_si128((__m128i *)(dst) + 3, xmm3);
#else
  copy_32((uint8_t*)(dst) +  0, (const uint8_t*)(src) +  0);
  copy_32((uint8_t*)(dst) + 32, (const uint8_t*)(src) + 32);
#endif
}

// ============================================================================
// [Fog::Memory - Copy (Templates)]
// ============================================================================

template<sysuint_t Size>
static FOG_INLINE void copy_s(void* _dst, const void* _src)
{
  uint8_t* dst = reinterpret_cast<uint8_t*>(_dst);
  const uint8_t* src = reinterpret_cast<const uint8_t*>(_src);

  for (uint i = 0; i < Size / 64; i++)
  {
    copy_64(dst, src); dst += 64; src += 64;
  }

  if (Size % 64 >= 32) { copy_32(dst, src); dst += 32; src += 32; }
  if (Size % 32 >= 16) { copy_16(dst, src); dst += 16; src += 16; }
  if (Size % 16 >= 8 ) { copy_8 (dst, src); dst += 8 ; src += 8 ; }
  if (Size % 8  >= 4 ) { copy_4 (dst, src); dst += 4 ; src += 4 ; }
  if (Size % 4  >= 2 ) { copy_2 (dst, src); dst += 2 ; src += 2 ; }
  if (Size % 2  == 1 ) { copy_1 (dst, src); }
}

template<typename Type>
static FOG_INLINE void copy_t(Type* dst, const Type* src)
{
  copy_s<sizeof(Type)>((void*)(dst), (const void*)(src));
}

// ============================================================================
// [Fog::Memory - Zero (Exact)]
// ============================================================================

static FOG_INLINE void zero_1(void* dst)
{
  ((uint8_t*)dst)[0] = 0;
}

static FOG_INLINE void zero_2(void* dst)
{
  ((uint16_t*)dst)[0] = 0;
}

static FOG_INLINE void zero_3(void* dst)
{
  ((uint16_t*)dst)[0] = 0;
  ((uint8_t *)dst)[2] = 0;
}

static FOG_INLINE void zero_4(void* dst)
{
  ((uint32_t*)dst)[0] = 0;
}

static FOG_INLINE void zero_8(void* dst)
{
#if FOG_ARCH_BITS >= 64
  ((uint64_t*)dst)[0] = FOG_UINT64_C(0);
#else
  ((uint32_t*)dst)[0] = 0;
  ((uint32_t*)dst)[1] = 0;
#endif
}

static FOG_INLINE void zero_16(void* dst)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i zero = _mm_setzero_si128();
  _mm_storeu_si128(&((__m128i *)dst)[0], zero);
#else
  zero_8((uint8_t*)(dst) + 0);
  zero_8((uint8_t*)(dst) + 8);
#endif
}

static FOG_INLINE void zero_32(void* dst)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i zero = _mm_setzero_si128();
  _mm_storeu_si128((__m128i *)(dst) + 0, zero);
  _mm_storeu_si128((__m128i *)(dst) + 1, zero);
#else
  zero_16((uint8_t*)(dst) +  0);
  zero_16((uint8_t*)(dst) + 16);
#endif
}

static FOG_INLINE void zero_64(void* dst)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i zero = _mm_setzero_si128();
  _mm_storeu_si128((__m128i *)(dst) + 0, zero);
  _mm_storeu_si128((__m128i *)(dst) + 1, zero);
  _mm_storeu_si128((__m128i *)(dst) + 2, zero);
  _mm_storeu_si128((__m128i *)(dst) + 3, zero);
#else
  zero_32((uint8_t*)(dst) +  0);
  zero_32((uint8_t*)(dst) + 32);
#endif
}

// ============================================================================
// [Fog::Memory - Zero (Templates)]
// ============================================================================

template<sysuint_t Size>
static FOG_INLINE void zero_s(void* _dst)
{
  uint8_t* dst = reinterpret_cast<uint8_t*>(_dst);

  for (uint i = 0; i < Size / 64; i++)
  {
    zero_64(dst); dst += 64;
  }

  if (Size % 64 >= 32) { zero_32(dst); dst += 32; }
  if (Size % 32 >= 16) { zero_16(dst); dst += 16; }
  if (Size % 16 >= 8 ) { zero_8 (dst); dst += 8 ; }
  if (Size % 8  >= 4 ) { zero_4 (dst); dst += 4 ; }
  if (Size % 4  >= 2 ) { zero_2 (dst); dst += 2 ; }
  if (Size % 2  == 1 ) { zero_1 (dst); }
}

template<typename Type>
static FOG_INLINE void zero_t(Type* dst)
{
  zero_s<sizeof(Type)>((void*)(dst));
}

// ============================================================================
// [Fog::Memory - Eq (Exact)]
// ============================================================================

static FOG_INLINE bool eq_1(const void* a, const void* b)
{
  return (((uint8_t  *)a)[0] == ((uint8_t  *)b)[0]);
}

static FOG_INLINE bool eq_2(const void* a, const void* b)
{
  return (((uint16_t *)a)[0] == ((uint16_t *)b)[0]);
}

static FOG_INLINE bool eq_3(const void* a, const void* b)
{
  return (((uint16_t *)a)[0] == ((uint16_t *)b)[0]) &
         (((uint8_t  *)a)[2] == ((uint8_t  *)b)[2]) ;
}

static FOG_INLINE bool eq_4(const void* a, const void* b)
{
  return (((uint32_t *)a)[0] == ((uint32_t *)b)[0]);
}

static FOG_INLINE bool eq_8(const void* a, const void* b)
{
#if FOG_ARCH_BITS >= 64
  return ((uint64_t *)a)[0] == ((uint64_t *)b)[0];
#else
  return (((uint32_t *)a)[0] == ((uint32_t *)b)[0]) &
         (((uint32_t *)a)[1] == ((uint32_t *)b)[1]) ;
#endif
}

static FOG_INLINE bool eq_16(const void* a, const void* b)
{
#if FOG_ARCH_BITS >= 64
  return ((uint64_t *)a)[0] == ((uint64_t *)b)[0] &
         ((uint64_t *)a)[1] == ((uint64_t *)b)[1] ;
#elif defined(FOG_HARDCODE_SSE2)
  __m128i xmm0a = _mm_loadu_si128((__m128i*)(a) + 0);
  __m128i xmm0b = _mm_loadu_si128((__m128i*)(b) + 0);

  xmm0a = _mm_cmpeq_epi8(xmm0a, xmm0b);
  return _mm_movemask_epi8(xmm0a) == 0xFFFF;
#else
  return (((uint32_t *)a)[0] == ((uint32_t *)b)[0]) &
         (((uint32_t *)a)[1] == ((uint32_t *)b)[1]) &
         (((uint32_t *)a)[2] == ((uint32_t *)b)[2]) &
         (((uint32_t *)a)[3] == ((uint32_t *)b)[3]) ;
#endif
}

static FOG_INLINE bool eq_32(const void* a, const void* b)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i xmm0a = _mm_loadu_si128((__m128i*)(a) + 0);
  __m128i xmm1a = _mm_loadu_si128((__m128i*)(a) + 1);
  __m128i xmm0b = _mm_loadu_si128((__m128i*)(b) + 0);
  __m128i xmm1b = _mm_loadu_si128((__m128i*)(b) + 1);

  xmm0a = _mm_cmpeq_epi8(xmm0a, xmm0b);
  xmm1a = _mm_cmpeq_epi8(xmm1a, xmm1b);

  xmm0a = _mm_and_si128(xmm0a, xmm1a);
  return _mm_movemask_epi8(xmm0a) == 0xFFFF;
#else
  return eq_16((const uint8_t*)(a) +  0, (const uint8_t*)(b) +  0) &
         eq_16((const uint8_t*)(a) + 16, (const uint8_t*)(b) + 16) ;
#endif // FOG_HARDCODE_SSE2
}

static FOG_INLINE bool eq_48(const void* a, const void* b)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i xmm0a = _mm_loadu_si128((__m128i*)(a) + 0);
  __m128i xmm1a = _mm_loadu_si128((__m128i*)(a) + 1);
  __m128i xmm0b = _mm_loadu_si128((__m128i*)(b) + 0);
  __m128i xmm1b = _mm_loadu_si128((__m128i*)(b) + 1);

  xmm0b = _mm_cmpeq_epi8(xmm0b, xmm0a);
  xmm0a = _mm_loadu_si128((__m128i*)(a) + 2);

  xmm1b = _mm_cmpeq_epi8(xmm1b, xmm1a);
  xmm1a = _mm_loadu_si128((__m128i*)(b) + 2);

  xmm1b = _mm_and_si128(xmm1b, xmm0b);
  xmm0a = _mm_cmpeq_epi8(xmm0a, xmm1a);

  xmm0a = _mm_and_si128(xmm0a, xmm1b);
  return _mm_movemask_epi8(xmm0a) == 0xFFFF;
#else
  return eq_32((const uint8_t*)(a) +  0, (const uint8_t*)(b) +  0) &
         eq_16((const uint8_t*)(a) + 32, (const uint8_t*)(b) + 32) ;
#endif // FOG_HARDCODE_SSE2
}

static FOG_INLINE bool eq_64(const void* a, const void* b)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i xmm0a = _mm_loadu_si128((__m128i*)(a) + 0);
  __m128i xmm1a = _mm_loadu_si128((__m128i*)(a) + 1);
  __m128i xmm0b = _mm_loadu_si128((__m128i*)(b) + 0);
  __m128i xmm1b = _mm_loadu_si128((__m128i*)(b) + 1);

  xmm0b = _mm_cmpeq_epi8(xmm0b, xmm0a);
  xmm0a = _mm_loadu_si128((__m128i*)(a) + 2);

  xmm1b = _mm_cmpeq_epi8(xmm1b, xmm1a);
  xmm1a = _mm_loadu_si128((__m128i*)(b) + 2);

  xmm1b = _mm_and_si128(xmm1b, xmm0b);
  xmm0b = _mm_loadu_si128((__m128i*)(a) + 3);

  xmm0a = _mm_cmpeq_epi8(xmm0a, xmm1a);
  xmm1a = _mm_loadu_si128((__m128i*)(b) + 3);

  xmm0a = _mm_and_si128(xmm0a, xmm1b);
  xmm0b = _mm_cmpeq_epi8(xmm0b, xmm1a);

  xmm0a = _mm_and_si128(xmm0a, xmm0b);
  return _mm_movemask_epi8(xmm0a) == 0xFFFF;
#else
  return eq_32((const uint8_t*)(a) +  0, (const uint8_t*)(b) +  0) &
         eq_32((const uint8_t*)(a) + 32, (const uint8_t*)(b) + 32) ;
#endif // FOG_HARDCODE_SSE2
}

// ============================================================================
// [Fog::Memory - Eq (Templates)]
// ============================================================================

template<sysuint_t Size>
static FOG_INLINE bool eq_s(const void* _a, const void* _b)
{
  bool result = true;

  const uint8_t* a = reinterpret_cast<const uint8_t*>(_a);
  const uint8_t* b = reinterpret_cast<const uint8_t*>(_b);

  for (uint i = 0; i < Size / 64; i++)
  {
    result &= eq_64(a, b);
    if (!result) return result;

    a += 64;
    b += 64;
  }

  if (Size % 64 >= 32) { result &= eq_32(a, b); a += 32; b += 32; }
  if (Size % 32 >= 16) { result &= eq_16(a, b); a += 16; b += 16; }
  if (Size % 16 >= 8 ) { result &= eq_8 (a, b); a += 8 ; b +=  8; }
  if (Size % 8  >= 4 ) { result &= eq_4 (a, b); a += 4 ; b +=  4; }
  if (Size % 4  >= 2 ) { result &= eq_2 (a, b); a += 2 ; b +=  2; }
  if (Size % 2  == 1 ) { result &= eq_1 (a, b); }

  return result;
}

template<typename Type>
static FOG_INLINE bool eq_t(const Type* dst, const Type* src)
{
  return eq_s<sizeof(Type)>((const void*)(dst), (const void*)(src));
}

// ============================================================================
// [Fog::Memory - Xchg (Exact)]
// ============================================================================

static FOG_INLINE void xchg_1(void* a, void* b)
{
  uint8_t t0 = ((uint8_t*)a)[0];
  ((uint8_t*)a)[0] = ((uint8_t*)b)[0];
  ((uint8_t*)b)[0] = t0;
}

static FOG_INLINE void xchg_2(void* a, void* b)
{
  uint16_t t0 = ((uint16_t*)a)[0];
  ((uint16_t*)a)[0] = ((uint16_t*)b)[0];
  ((uint16_t*)b)[0] = t0;
}

static FOG_INLINE void xchg_3(void* a, void* b)
{
  uint16_t t0 = ((uint16_t*)a)[0];
  uint8_t  t1 = ((uint8_t *)a)[2];

  ((uint16_t*)a)[0] = ((uint16_t*)b)[0];
  ((uint8_t *)a)[2] = ((uint8_t *)b)[2];

  ((uint16_t*)b)[0] = t0;
  ((uint8_t *)b)[2] = t1;
}

static FOG_INLINE void xchg_4(void* a, void* b)
{
  uint32_t t0 = ((uint32_t*)a)[0];
  ((uint32_t*)a)[0] = ((uint32_t*)b)[0];
  ((uint32_t*)b)[0] = t0;
}

static FOG_INLINE void xchg_8(void* a, void* b)
{
#if FOG_ARCH_BITS >= 64
  uint64_t t0 = ((uint64_t*)a)[0];
  ((uint64_t*)a)[0] = ((uint64_t*)b)[0];
  ((uint64_t*)b)[0] = t0;
#else
  uint32_t t0 = ((uint32_t*)a)[0];
  uint32_t t1 = ((uint32_t*)a)[1];

  ((uint32_t*)a)[0] = ((uint32_t*)b)[0];
  ((uint32_t*)a)[1] = ((uint32_t*)b)[1];

  ((uint32_t*)b)[0] = t0;
  ((uint32_t*)b)[1] = t1;
#endif
}

static FOG_INLINE void xchg_12(void* a, void* b)
{
  xchg_8((uint8_t*)(a)    , (uint8_t*)(b)    );
  xchg_4((uint8_t*)(a) + 8, (uint8_t*)(b) + 8);
}

static FOG_INLINE void xchg_16(void* a, void* b)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i xmm0a = _mm_loadu_si128((__m128i *)(a));
  __m128i xmm0b = _mm_loadu_si128((__m128i *)(b));

  _mm_storeu_si128((__m128i *)(b), xmm0a);
  _mm_storeu_si128((__m128i *)(a), xmm0b);
#else
  xchg_8((uint8_t*)(a)    , (uint8_t*)(b)    );
  xchg_8((uint8_t*)(a) + 8, (uint8_t*)(b) + 8);
#endif
}

static FOG_INLINE void xchg_24(void* a, void* b)
{
  xchg_16((uint8_t*)(a)     , (uint8_t*)(b)     );
  xchg_8 ((uint8_t*)(a) + 16, (uint8_t*)(b) + 16);
}

static FOG_INLINE void xchg_32(void* a, void* b)
{
#if defined(FOG_HARDCODE_SSE2)
  __m128i t0a = _mm_loadu_si128((__m128i *)(a) + 0);
  __m128i t1a = _mm_loadu_si128((__m128i *)(a) + 1);
  __m128i t0b = _mm_loadu_si128((__m128i *)(b) + 0);
  __m128i t1b = _mm_loadu_si128((__m128i *)(b) + 1);

  _mm_storeu_si128((__m128i *)(b) + 0, t0a);
  _mm_storeu_si128((__m128i *)(b) + 1, t1a);
  _mm_storeu_si128((__m128i *)(a) + 0, t0b);
  _mm_storeu_si128((__m128i *)(a) + 1, t1b);
#else
  xchg_16((uint8_t*)(a)     , (uint8_t*)(b)     );
  xchg_16((uint8_t*)(a) + 16, (uint8_t*)(b) + 16);
#endif
}

// ============================================================================
// [Fog::Memory - Xchg (Templates)]
// ============================================================================

template<sysuint_t Size>
static FOG_INLINE void xchg_s(void* _a, void* _b)
{
  uint8_t* a = reinterpret_cast<uint8_t*>(_a);
  uint8_t* b = reinterpret_cast<uint8_t*>(_b);

  for (uint i = 0; i < Size / 32; i++)
  {
    xchg_32(a, b); a += 32; b += 32;
  }

  if (Size % 32 >= 16) { xchg_16(a, b); a += 16; b += 16; }
  if (Size % 16 >= 8 ) { xchg_8 (a, b); a += 8 ; b += 8 ; }
  if (Size % 8  >= 4 ) { xchg_4 (a, b); a += 4 ; b += 4 ; }
  if (Size % 4  >= 2 ) { xchg_2 (a, b); a += 2 ; b += 2 ; }
  if (Size % 2  == 1 ) { xchg_1 (a, b); }
}


template<typename Type>
static FOG_INLINE void xchg_t(Type* a, Type* b)
{
  xchg_s<sizeof(Type)>((void*)(a), (void*)(b));
}

template<typename T>
static FOG_INLINE void xchgPtr(T** a, T** b)
{
#if FOG_ARCH_BITS == 64
  xchg_8(static_cast<void*>(a), static_cast<void*>(b));
#else
  xchg_4(static_cast<void*>(a), static_cast<void*>(b));
#endif
}

//! @}

} // Memory namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMORYOPS_H
