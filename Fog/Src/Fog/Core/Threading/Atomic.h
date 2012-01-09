// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_ATOMIC_H
#define _FOG_CORE_THREADING_ATOMIC_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_Core_Atomic
//! @{

// ============================================================================
// [Fog::AtomicImplementation]
// ============================================================================

template<size_t SizeI>
struct AtomicImplementation {};

} // Fog namespace

// ============================================================================
// [Include platform and compiler dependent implementation]
// ============================================================================

#if defined(FOG_CC_GNU) && (defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64))
# include <Fog/Core/Threading/Atomic_gcc_x86x64.h>
#elif defined(FOG_CC_GNU) || defined(FOG_CC_CLANG)
# include <Fog/Core/Threading/Atomic_gcc_intrin.h>
#elif (defined(FOG_CC_MSC) && FOG_CC_MSC < 1400) || defined(FOG_CC_BORLAND)
# include <Fog/Core/Threading/Atomic_msc_x86.h>
#elif (defined(FOG_CC_MSC))
# include <Fog/Core/Threading/Atomic_msc_intrin.h>
#endif

namespace Fog {

// ============================================================================
// [Fog::AtomicCore]
// ============================================================================

template<typename TypeT>
struct AtomicCore
{
  typedef TypeT Type;

  typedef AtomicImplementation<sizeof(TypeT)> __Backend;
  typedef typename __Backend::Type __BackendType;

  static FOG_INLINE void set(TypeT* atomic, TypeT value)
  {
    __Backend::set((__BackendType*)atomic, (__BackendType)value);
  }

  static FOG_INLINE TypeT setXchg(TypeT* atomic, TypeT value)
  {
    return (TypeT)__Backend::setXchg((__BackendType*)atomic, (__BackendType)value);
  }

  static FOG_INLINE bool cmpXchg(TypeT* atomic, TypeT compar, TypeT value)
  {
    return __Backend::cmpXchg((__BackendType*)atomic, (__BackendType)compar, (__BackendType)value);
  }

  static FOG_INLINE TypeT get(const TypeT* atomic)
  {
    return (TypeT)__Backend::get((const __BackendType*)atomic);
  }

  static FOG_INLINE void inc(TypeT* atomic)
  {
    __Backend::inc((__BackendType*)atomic);
  }

  static FOG_INLINE void dec(TypeT* atomic)
  {
    __Backend::dec((__BackendType*)atomic);
  }

  static FOG_INLINE void add(TypeT* atomic, TypeT value)
  {
    __Backend::add((__BackendType*)atomic, (__BackendType)value);
  }

  static FOG_INLINE void sub(TypeT* atomic, TypeT value)
  {
    __Backend::sub((__BackendType*)atomic, (__BackendType)value);
  }

  static FOG_INLINE TypeT addXchg(TypeT* atomic, TypeT value)
  {
    return (TypeT)__Backend::addXchg((__BackendType*)atomic, (__BackendType)value);
  }

  static FOG_INLINE TypeT subXchg(TypeT* atomic, TypeT value)
  {
    return (TypeT)__Backend::subXchg((__BackendType*)atomic, (__BackendType)value);
  }

  static FOG_INLINE bool deref(TypeT* atomic)
  {
    return __Backend::deref((__BackendType*)atomic);
  }
};

// ============================================================================
// [Fog::Atomic<TypeT>]
// ============================================================================

template<typename TypeT>
struct Atomic
{
  typedef AtomicCore<TypeT> Op;

  mutable TypeT _atomic;

  FOG_INLINE void init(TypeT value) { _atomic = value; }
  FOG_INLINE void set(TypeT value) { Op::set(&_atomic, value); }
  FOG_INLINE TypeT setXchg(TypeT value) { return Op::setXchg(&_atomic, value); }
  FOG_INLINE bool cmpXchg(TypeT compar, TypeT value) { return Op::cmpXchg(&_atomic, compar, value); }
  FOG_INLINE TypeT get() const{ return Op::get(&_atomic); }
  FOG_INLINE void inc() { Op::inc(&_atomic); }
  FOG_INLINE void dec() { Op::dec(&_atomic); }
  FOG_INLINE void add(TypeT value) { Op::add(&_atomic, value); }
  FOG_INLINE void sub(TypeT value) { Op::sub(&_atomic, value); }
  FOG_INLINE TypeT addXchg(TypeT value) { return Op::addXchg(&_atomic, value); }
  FOG_INLINE TypeT subXchg(TypeT value) { return Op::subXchg(&_atomic, value); }
  FOG_INLINE bool deref() { return Op::deref(&_atomic); }
};

template<typename TypeT>
static FOG_INLINE TypeT* atomicPtrXchg(TypeT** atomic, TypeT* value)
{
  return (TypeT*)(void*)AtomicCore<void*>::setXchg((void**)atomic, (void*)value);
}

//! @brief Macro to help setting @c Fog::Atomic structure in static data.
#define FOG_ATOMIC_SET(_Value_) { _Value_ }

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_ATOMIC_H
