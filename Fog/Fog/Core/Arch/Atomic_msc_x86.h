// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Build.h>

// [Guard]
#if !defined(FOG_IDE) && !defined(_FOG_CORE_ATOMIC_H)
#error "Fog::Atomic::MSC_X86 - Only Fog/Core/Atomic.h can include this file."
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
    (*(volatile int32_t *)atomic) = value;
  }

  static FOG_INLINE int32_t setXchg(int32_t* atomic, int32_t value)
  {
    __asm {
      mov ecx, atomic
      mov edx, value
      lock xchg dword ptr[ecx], edx
      mov value, edx
    }
    return value;
  }

  static FOG_INLINE bool cmpXchg(int32_t* atomic, int32_t compar, int32_t value)
  {
    __asm {
      mov ecx, atomic
      mov edx, value
      mov eax, compar
      lock cmpxchg dword ptr[ecx], edx
      mov value, eax
    }
    return value == compar;
  }
  
  static FOG_INLINE int32_t get(const int32_t* atomic)
  {
    return (*(volatile int32_t *)atomic);
  }

  static FOG_INLINE void inc(int32_t* atomic)
  {
    __asm {
      mov ecx, atomic
      lock inc dword ptr[ecx]
    }
  }
  
  static FOG_INLINE void dec(int32_t* atomic)
  {
    __asm {
      mov ecx, atomic
      lock dec dword ptr[ecx]
    }
  }
  
  static FOG_INLINE void add(int32_t* atomic, int32_t value)
  {
    __asm {
      mov ecx, atomic
      mov edx, value
      lock add dword ptr[ecx], edx
    }
  }
  
  static FOG_INLINE void sub(int32_t* atomic, int32_t value)
  {
    __asm {
      mov ecx, atomic
      mov edx, value
      lock sub dword ptr[ecx], edx
    }
  }
  
  static FOG_INLINE int32_t addXchg(int32_t* atomic, int32_t value)
  {
    __asm {
      mov ecx, atomic
      mov edx, value
      lock xadd dword ptr[ecx], edx
      mov value, edx
    }
    return value;
  }
  
  static FOG_INLINE int32_t subXchg(int32_t* atomic, int32_t value)
  {
    value = -value;
    return addXchg(atomic, value);
  }
  
  static FOG_INLINE bool deref(int32_t* atomic)
  {
    char result;
    __asm {
      mov ecx, atomic
      lock dec dword ptr[ecx]
      setne result
    }
    return !result;
  }
};

// Specialize.
template<>
struct AtomicOperationHelper<4> : public AtomicInt32 {};

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_ATOMIC_H
