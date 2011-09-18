// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/C++/Base.h>

// [Guard]
#if !defined(FOG_IDE) && !defined(_FOG_CORE_THREADING_ATOMIC_H)
#error "Fog::Threading::Atomic::MSC_INTRIN - Only Fog/Core/Threading/Atomic.h can include this file."
#else

// Always use compiler intrinsics if available.
#if defined(FOG_CC_MSC) && FOG_CC_MSC >= 1400
# include <intrin.h>
# pragma intrinsic (_InterlockedIncrement)
# pragma intrinsic (_InterlockedCompareExchange)
# pragma intrinsic (_InterlockedDecrement)
# pragma intrinsic (_InterlockedExchange)
# pragma intrinsic (_InterlockedExchangeAdd)
# if FOG_ARCH_BITS == 64
#  pragma intrinsic (_InterlockedIncrement64)
#  pragma intrinsic (_InterlockedCompareExchange64)
#  pragma intrinsic (_InterlockedDecrement64)
#  pragma intrinsic (_InterlockedExchange64)
#  pragma intrinsic (_InterlockedExchangeAdd64)
# endif // FOG_ARCH_BITS == 64
# define _MSINTRIN_(func) _##func
#else
# define _MSINTRIN_(func) func
#endif // FOG_CC_MSC

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
    _MSINTRIN_(InterlockedExchange)((LONG volatile*)atomic, (LONG)value);
  }

  static FOG_INLINE int32_t setXchg(int32_t* atomic, int32_t value)
  {
    return (int32_t)_MSINTRIN_(InterlockedExchange)((LONG volatile*)atomic, (LONG)value);
  }

  static FOG_INLINE bool cmpXchg(int32_t* atomic, int32_t compar, int32_t value)
  {
    return (bool)(_MSINTRIN_(InterlockedCompareExchange)((LONG*)atomic, (LONG)value, (LONG)compar) == (LONG)compar);
  }

  static FOG_INLINE int32_t get(const int32_t* atomic)
  {
    return *(volatile const int32_t *)atomic;
  }

  static FOG_INLINE void inc(int32_t* atomic)
  {
    _MSINTRIN_(InterlockedIncrement)((LONG*)atomic);
  }

  static FOG_INLINE void dec(int32_t* atomic)
  {
    _MSINTRIN_(InterlockedDecrement)((LONG*)atomic);
  }

  static FOG_INLINE void add(int32_t* atomic, int32_t value)
  {
    _MSINTRIN_(InterlockedExchangeAdd)((LONG*)atomic, (LONG)value);
  }

  static FOG_INLINE void sub(int32_t* atomic, int32_t value)
  {
    _MSINTRIN_(InterlockedExchangeAdd)((LONG*)atomic, (LONG)((value ^ 0xFFFFFFFF) + 1U));
  }

  static FOG_INLINE int32_t addXchg(int32_t* atomic, int32_t value)
  {
    return (int32_t)_MSINTRIN_(InterlockedExchangeAdd)((LONG*)atomic, (LONG)value);
  }

  static FOG_INLINE int32_t subXchg(int32_t* atomic, int32_t value)
  {
    return (int32_t)_MSINTRIN_(InterlockedExchangeAdd)((LONG*)atomic, (LONG)((value ^ 0xFFFFFFFF) + 1U));
  }

  static FOG_INLINE bool deref(int32_t* atomic)
  {
    return _MSINTRIN_(InterlockedDecrement)((LONG*)atomic) == 0;
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
    _MSINTRIN_(InterlockedExchange64)((int64_t*)atomic, (int64_t)value);
  }

  static FOG_INLINE int64_t setXchg(int64_t* atomic, int64_t value)
  {
    return (int64_t)_MSINTRIN_(InterlockedExchange64)((int64_t*)atomic, (int64_t)value);
  }

  static FOG_INLINE bool cmpXchg(int64_t* atomic, int64_t compar, int64_t value)
  {
    return (bool)(_MSINTRIN_(InterlockedCompareExchange64)((int64_t*)atomic, (int64_t)value, (int64_t)compar) == (int64_t)compar);
  }

  static FOG_INLINE int64_t get(const int64_t* atomic)
  {
    return *(volatile const int64_t *)atomic;
  }

  static FOG_INLINE void inc(int64_t* atomic)
  {
    _MSINTRIN_(InterlockedIncrement64)((int64_t*)atomic);
  }

  static FOG_INLINE void dec(int64_t* atomic)
  {
    _MSINTRIN_(InterlockedDecrement64)((int64_t*)atomic);
  }

  static FOG_INLINE void add(int64_t* atomic, int64_t value)
  {
    _MSINTRIN_(InterlockedExchangeAdd64)((int64_t*)atomic, (int64_t)value);
  }

  static FOG_INLINE void sub(int64_t* atomic, int64_t value)
  {
    _MSINTRIN_(InterlockedExchangeAdd64)((int64_t*)atomic, (int64_t)((value ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF)) + FOG_UINT64_C(1)));
  }

  static FOG_INLINE int64_t addXchg(int64_t* atomic, int64_t value)
  {
    return (int64_t)_MSINTRIN_(InterlockedExchangeAdd64)((int64_t*)atomic, (int64_t)value);
  }

  static FOG_INLINE int64_t subXchg(int64_t* atomic, int64_t value)
  {
    return (int64_t)_MSINTRIN_(InterlockedExchangeAdd64)((int64_t*)atomic, (int64_t)((value ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF)) + FOG_UINT64_C(1)));
  }

  static FOG_INLINE bool deref(int64_t* atomic)
  {
    return _MSINTRIN_(InterlockedDecrement64)((int64_t*)atomic) == FOG_INT64_C(0);
  }
};

// Specialize.
template<>
struct AtomicImplementation<8> : public AtomicInt64 {};

#endif // FOG_ARCH_BITS == 64

} // Fog namespace

// [Cleanup]
#undef _MSINTRIN_

// [Guard]
#endif // _FOG_CORE_THREADING_ATOMIC_H
