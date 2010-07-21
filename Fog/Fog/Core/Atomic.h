// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_ATOMIC_H
#define _FOG_CORE_ATOMIC_H

// [Dependencies]
#include <Fog/Core/Build.h>

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Fog::AtomicOperationHelper]
// ============================================================================

template<sysuint_t IntSize>
struct AtomicOperationHelper {};

} // Fog namespace

// ============================================================================
// [Include platform and compiler dependent implementation]
// ============================================================================

#if defined(FOG_CC_GNU) && (defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64))
# include <Fog/Core/Arch/Atomic_gcc_x86x64.h>
#elif defined(FOG_CC_CLANG) && (defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64))
# include <Fog/Core/Arch/Atomic_clang_x86x64.h>
#elif defined(FOG_CC_GNU) || defined(FOG_CC_CLANG)
# include <Fog/Core/Arch/Atomic_gcc_intrin.h>
#elif (defined(FOG_CC_MSVC) && FOG_CC_MSVC < 1400) || defined(FOG_CC_BORLAND)
# include <Fog/Core/Arch/Atomic_msc_x86.h>
#elif (defined(FOG_CC_MSVC))
# include <Fog/Core/Arch/Atomic_msc_intrin.h>
#endif

namespace Fog {

// ============================================================================
// [Fog::AtomicOperation]
// ============================================================================

template<typename T>
struct AtomicOperation
{
  typedef T Type;

  typedef AtomicOperationHelper<sizeof(T)> __Backend;
  typedef typename __Backend::Type __BackendType;

  static FOG_INLINE void set(T* atomic, T value)
  {
    __Backend::set((__BackendType*)atomic, (__BackendType)value);
  }
  
  static FOG_INLINE T setXchg(T* atomic, T value)
  {
    return (T)__Backend::setXchg((__BackendType*)atomic, (__BackendType)value);
  }

  static FOG_INLINE bool cmpXchg(T* atomic, T compar, T value)
  {
    return __Backend::cmpXchg((__BackendType*)atomic, (__BackendType)compar, (__BackendType)value);
  }
  
  static FOG_INLINE T get(const T* atomic)
  {
    return (T)__Backend::get((const __BackendType*)atomic);
  }

  static FOG_INLINE void inc(T* atomic)
  {
    __Backend::inc((__BackendType*)atomic);
  }
  
  static FOG_INLINE void dec(T* atomic)
  {
    __Backend::dec((__BackendType*)atomic);
  }
  
  static FOG_INLINE void add(T* atomic, T value)
  {
    __Backend::add((__BackendType*)atomic, (__BackendType)value);
  }
  
  static FOG_INLINE void sub(T* atomic, T value)
  {
    __Backend::sub((__BackendType*)atomic, (__BackendType)value);
  }
  
  static FOG_INLINE T addXchg(T* atomic, T value)
  {
    return (T)__Backend::addXchg((__BackendType*)atomic, (__BackendType)value);
  }
  
  static FOG_INLINE T subXchg(T* atomic, T value)
  {
    return (T)__Backend::subXchg((__BackendType*)atomic, (__BackendType)value);
  }
  
  static FOG_INLINE bool deref(T* atomic)
  {
    return __Backend::deref((__BackendType*)atomic);
  }
};

// ============================================================================
// [Fog::Atomic<T>]
// ============================================================================

template<typename T>
struct Atomic
{
  typedef AtomicOperation<T> Op;

  mutable T _atomic;

  FOG_INLINE void init(T value)              { _atomic = value; }
  FOG_INLINE void set(T value)               { Op::set(&_atomic, value); }
  FOG_INLINE T setXchg(T value)              { return Op::setXchg(&_atomic, value); }
  FOG_INLINE bool cmpXchg(T compar, T value) { return Op::cmpXchg(&_atomic, compar, value); }
  FOG_INLINE T get() const                   { return Op::get(&_atomic); }
  FOG_INLINE void inc()                      { Op::inc(&_atomic); }
  FOG_INLINE void dec()                      { Op::dec(&_atomic); }
  FOG_INLINE void add(T value)               { Op::add(&_atomic, value); }
  FOG_INLINE void sub(T value)               { Op::sub(&_atomic, value); }
  FOG_INLINE T addXchg(T value)              { return Op::addXchg(&_atomic, value); }
  FOG_INLINE T subXchg(T value)              { return Op::subXchg(&_atomic, value); }
  FOG_INLINE bool deref()                    { return Op::deref(&_atomic); }
};

template<typename T>
static FOG_INLINE T* atomicPtrXchg(T** atomic, T* value)
{
  return (T*)(void*)AtomicOperation<void*>::setXchg((void**)atomic, (void*)value);
}

//! @brief Macro to help setting @c Fog::Atomic structure in static data.
#define FOG_ATOMIC_SET(value) { value }

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<T>]
// ============================================================================

#include <Fog/Core/TypeInfo.h>
FOG_DECLARE_TYPEINFO_TEMPLATE1(Fog::Atomic, typename, T, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_ATOMIC_H
