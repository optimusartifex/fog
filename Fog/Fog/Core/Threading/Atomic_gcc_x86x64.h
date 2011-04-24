// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Config/Config.h>

// [Guard]
#if !defined(FOG_IDE) && !defined(_FOG_CORE_THREADING_ATOMIC_H)
#error "Fog::Threading::Atomic::GCC_X86_X64 - Only Fog/Core/Threading/Atomic.h can include this file."
#else

namespace Fog {

// ============================================================================
// [Fog::AtomicInt32]
// ============================================================================

//! @internal
struct AtomicInt32
{
  typedef int32_t Type;

  static FOG_INLINE void set(int32_t* atomic, int32_t value)
  {
    __asm__ __volatile__
    (
      "movl %1, %0\n"
        : "=m" (*atomic)
        : "ir" (value), "m" (*atomic)
    );
  }

  static FOG_INLINE int32_t setXchg(int32_t* atomic, int32_t value)
  {
    __asm__ __volatile__
    (
      "xchgl %0,%1"
        : "=r" (value)
        : "m" (*atomic), "0" (value)
        : "memory"
    );
    return value;
  }

  static FOG_INLINE bool cmpXchg(int32_t* atomic, int32_t compar, int32_t value)
  {
    unsigned char result;
    __asm__ __volatile__
    (
      "lock cmpxchgl %2,%3\n"
      "sete %1\n"
        : "=a" (value), "=qm" (result)
        : "r" (value), "m" (*atomic), "0" (compar)
        : "memory"
    );
    return (bool)result;
  }

  static FOG_INLINE int32_t get(const int32_t* atomic)
  {
    int32_t result;
    __asm__ __volatile__
    (
      "movl %1, %0\n"
        : "=r" (result)
        : "m" (*atomic)
    );
    return result;
  }

  static FOG_INLINE void inc(int32_t* atomic)
  {
    __asm__ __volatile__
    (
      "lock incl %0\n"
        : "=m" (*atomic)
        : "m" (*atomic)
    );
  }

  static FOG_INLINE void dec(int32_t* atomic)
  {
    __asm__ __volatile__
    (
      "lock decl %0\n"
        : "=m" (*atomic)
        : "m" (*atomic)
    );
  }

  static FOG_INLINE void add(int32_t* atomic, int32_t value)
  {
    __asm__ __volatile__
    (
      "lock addl %1, %0\n"
        : "=m" (*atomic)
        : "ir" (value), "m" (*atomic)
    );
  }

  static FOG_INLINE void sub(int32_t* atomic, int32_t value)
  {
    __asm__ __volatile__
    (
      "lock subl %1, %0\n"
        : "=m" (*atomic)
        : "ir" (value), "m" (*atomic)
    );
  }

  static FOG_INLINE int32_t addXchg(int32_t* atomic, int32_t value)
  {
    int32_t result;
    __asm__ __volatile__
    (
      "lock xaddl %0, %1\n"
        : "=r" (result), "=m" (*atomic)
        : "0" (value), "m" (*atomic)
    );
    return result;
  }

  static FOG_INLINE int32_t subXchg(int32_t* atomic, int32_t value)
  {
    int32_t result;
    __asm__ __volatile__
    (
      "lock xaddl %0, %1\n"
        : "=r" (result), "=m" (*atomic)
        : "0" ((value ^ 0xFFFFFFFF) + 1U), "m" (*atomic)
    );
    return result;
  }

  static FOG_INLINE bool deref(int32_t* atomic)
  {
    char result;

    __asm__ __volatile__
    (
      "lock decl %0\n"
      "setne %1\n"
        : "=m" (*atomic), "=qm" (result)
        : "m" (*atomic)
    );
    return !result;
  }
};

// Specialize.
template<>
struct AtomicImplementation<4> : public AtomicInt32 {};

// ============================================================================
// [Fog::AtomicInt64]
// ============================================================================

#if FOG_ARCH_BITS == 64

//! @internal
struct AtomicInt64
{
  typedef int64_t Type;

  static FOG_INLINE void set(int64_t* atomic, int64_t value)
  {
    __asm__ __volatile__
    (
      "movq %1, %0\n"
        : "=m" (*atomic)
        : "ir" (value), "m" (*atomic)
    );
  }

  static FOG_INLINE int64_t setXchg(int64_t* atomic, int64_t value)
  {
    __asm__ __volatile__
    (
      "xchgq %0,%1"
        : "=r" (value)
        : "m" (*atomic), "0" (value)
        : "memory"
    );
    return value;
  }

  static FOG_INLINE bool cmpXchg(int64_t* atomic, int64_t compar, int64_t value)
  {
    unsigned char result;
    __asm__ __volatile__
    (
      "lock cmpxchgq %2,%3\n"
      "sete %1\n"
        : "=a" (value), "=qm" (result)
        : "r" (value), "m" (*atomic), "0" (compar)
        : "memory"
    );
    return (bool)result;
  }

  static FOG_INLINE int64_t get(const int64_t* atomic)
  {
    int64_t result;
    __asm__ __volatile__
    (
      "movq %1, %0\n"
        : "=r" (result)
        : "m" (*atomic)
    );
    return result;
  }

  static FOG_INLINE void inc(int64_t* atomic)
  {
    __asm__ __volatile__
    (
      "lock incq %0\n"
        : "=m" (*atomic)
        : "m" (*atomic)
    );
  }

  static FOG_INLINE void dec(int64_t* atomic)
  {
    __asm__ __volatile__
    (
      "lock decq %0\n"
        : "=m" (*atomic)
        : "m" (*atomic)
    );
  }

  static FOG_INLINE void add(int64_t* atomic, int64_t value)
  {
    __asm__ __volatile__
    (
      "lock addq %1, %0\n"
        : "=m" (*atomic)
        : "ir" (value), "m" (*atomic)
    );
  }

  static FOG_INLINE void sub(int64_t* atomic, int64_t value)
  {
    __asm__ __volatile__
    (
      "lock subq %1, %0\n"
        : "=m" (*atomic)
        : "ir" (value), "m" (*atomic)
    );
  }

  static FOG_INLINE int64_t addXchg(int64_t* atomic, int64_t value)
  {
    int64_t result;
    __asm__ __volatile__
    (
      "lock xaddq %0, %1\n"
        : "=r" (result), "=m" (*atomic)
        : "0" (value), "m" (*atomic)
    );
    return result;
  }

  static FOG_INLINE int64_t subXchg(int64_t* atomic, int64_t value)
  {
    int64_t result;
    __asm__ __volatile__
    (
      "lock xaddq %0, %1\n"
        : "=r" (result), "=m" (*atomic)
        : "0" ((-value ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF)) + FOG_UINT64_C(1)), "m" (*atomic)
    );
    return result;
  }

  static FOG_INLINE bool deref(int64_t* atomic)
  {
    char result;

    __asm__ __volatile__
    (
      "lock decq %0\n"
      "setne %1\n"
        : "=m" (*atomic), "=qm" (result)
        : "m" (*atomic)
    );
    return !result;
  }
};

// Specialize.
template<>
struct AtomicImplementation<8> : public AtomicInt64 {};

#endif // FOG_ARCH_BITS == 64

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_ATOMIC_H
